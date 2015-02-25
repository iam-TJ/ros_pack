/******************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   C++ stream implemented with LZMA/XZ compression.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define BASE_STREAM_INPUT_STORAGE_LZMA_CC 1
#include "base.hh"
#include "file.hh"
#include "buffer_string.hh"
#include "stream_defs.hh"
#include "stream_input_storage.hh"
#include "stream_input_storage_lzma.hh"

namespace base {

////////////////////////////////////////////////////////////////////////////////
/////////////////////////  StreamInputStorageLZMA  /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamInputStorageLZMA::StreamInputStorageLZMA( const string& pathname )
:   mPathname(pathname),
    mOpen(false),
    mInputStringBuf{},
    mInputStringIdx(0),
    mLzmaStream{}
{
ASSERT( not mPathname.empty() );

    // (Do not open file yet, wait until StreamInput will call Open().)
    mLzmaStream.next_in   = nullptr;
    mLzmaStream.avail_in  = 0;
    mLzmaStream.next_out  = nullptr;
    mLzmaStream.avail_out = 0;
}

StreamInputStorageLZMA::~StreamInputStorageLZMA()
{
    // Call Close() to ensure memory is released.
    Close();
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool StreamInputStorageLZMA::Open( void )
{
ASSERT( not mOpen );  // don't re-open
ASSERT( not mPathname.empty() );

    // Open the compressed file.
    // Read it into a memory buffer (owned by this).
    // ReadFile() has OS-specific code.
    if ( not ReadFile( mPathname, mInputStringBuf ) )
    {
    #if DEBUG
        CDEBUG << "base:StreamInputStorageLZMA: Failed to open file '" << mPathname << "'" << std::endl;
        CDEBUG << "base:StreamInputStorageLZMA: mStringBuf.IfEmpty()=" << mInputStringBuf.IfEmpty() << std::endl;
    #endif
        return false;  // failed/error
    }

    // Prepare LZMA stream for use later by Read().
    mLzmaStream = LZMA_STREAM_INIT;
    mLzmaStream.next_in   = mInputStringBuf.GetUchars();
    mLzmaStream.avail_in  = std::min( INPUT_CHUNK_SIZE, mInputStringBuf.Size() );
    mLzmaStream.next_out  = nullptr;  // to be assigned from Read() arg
    mLzmaStream.avail_out = 0;    // to be assigned from Read() arg
    if ( lzma_stream_decoder( &mLzmaStream, UINT64_MAX, LZMA_CONCATENATED ) != LZMA_OK )
        throw std::runtime_error( "base:StreamInputStorageLZMA: lzma_stream_decoder" );

    // Opened OK.
    mOpen = true;
    return true;
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool StreamInputStorageLZMA::Close( void )
{
    // In this class, Close() is responsible for freeing memory and must tolerate re-closing.
    // Not an error if already closed as Close() could be called after failed open.

    if ( mOpen )
    {
        // Free StringBuffer.
        mInputStringBuf.Erase();

        // Free lzma_stream.
        lzma_end( &mLzmaStream );

        // Last step.
        mOpen = false;
    }

    return true;
}

/*******************************************************************************
 * 
 *******************************************************************************/
StreamSize StreamInputStorageLZMA::Read( char* buf/*OUT*/, const StreamSize count )
{
ASSERT( mOpen );

    // Notes:
    // - This is based on XZ's example 02_decompress.c.
    // - avail_in is the count of bytes that remain to be input.
    //   LZMA decreases avail_in as it reads bytes,
    //   and when avail_in falls to zero, the user (this) must advance
    //   the next_in,avail_in to the next input chunk.

    if ( UX( not mOpen ) )
        return defs::STREAM_EOF;

    // To hold a decompressed chunk.
    mLzmaStream.next_out  = reinterpret_cast<uchar*>(buf);
    mLzmaStream.avail_out = count;

    // Continue loop until LZMA finished decompressing this chunk of input.
    lzma_action lzmaAction = LZMA_RUN;
    while ( mLzmaStream.avail_out > 0 )
    {
        // Advance read index if LZMA has read the entire chunk (LZMA decremented avail_in down to 0).
        // avail_in will be 0 while LZMA is finished decompressing its final chunk.
        if ( (mLzmaStream.avail_in == 0) and (lzmaAction == LZMA_RUN) )
        {
            mInputStringIdx += INPUT_CHUNK_SIZE;
            if ( mInputStringIdx < mInputStringBuf.Size() )
            {
                // More input remains.
                StreamSize remaining = mInputStringBuf.Size() - mInputStringIdx;
                ASSERT( remaining >= 0 );
                if ( remaining > INPUT_CHUNK_SIZE )
                    remaining = INPUT_CHUNK_SIZE;
                mLzmaStream.next_in  = reinterpret_cast<const uchar*>( mInputStringBuf.GetChars() + mInputStringIdx );
                mLzmaStream.avail_in = remaining;

                ASSERT( mInputStringIdx + remaining <= mInputStringBuf.Size() );  // this too is a size which can be equal to StringBuf size
            }
            else
            {
                // No more input.
                // But continue looping to extract the remainder of LZMA's buffer.
                mInputStringIdx -= INPUT_CHUNK_SIZE;  // undo
                lzmaAction = LZMA_FINISH;
            }
        }

        // Don't overrun input buffer (end=end is ok so <=).
      //ASSERT( mLzmaStream.next_in + mLzmaStream.avail_in < reinterpret_cast<const uchar*>(mInputStringBuf.GetChars() + mInputStringBuf.Size()) );
        ASSERT( mLzmaStream.next_in + mLzmaStream.avail_in <= reinterpret_cast<const uchar*>(mInputStringBuf.GetChars() + mInputStringBuf.Size()) );
        ASSERT( mInputStringIdx + mLzmaStream.avail_in <= mInputStringBuf.Size() );  // alternative check

        // Decompress the next chunk.
        const lzma_ret lzmaRet = lzma_code( &mLzmaStream, lzmaAction );
        switch ( lzmaRet )
        {
            case LZMA_OK: case LZMA_NO_CHECK: case LZMA_UNSUPPORTED_CHECK: case LZMA_GET_CHECK:
            {
                // NOP
            }
            break;

            case LZMA_STREAM_END:
            {
                // LZMA has finished.
                const StreamSize bytesRead = count - StreamSize(mLzmaStream.avail_out);
                ASSERT( bytesRead >= 0 );
                return bytesRead;
            }
            break;

            case LZMA_MEM_ERROR: case LZMA_MEMLIMIT_ERROR: case LZMA_FORMAT_ERROR: case LZMA_OPTIONS_ERROR:
            case LZMA_DATA_ERROR: case LZMA_BUF_ERROR: case LZMA_PROG_ERROR:
            {
                return defs::STREAM_ERROR;  // error
            }
            break;
        }
    }

    return count;
}

} // namespace base
