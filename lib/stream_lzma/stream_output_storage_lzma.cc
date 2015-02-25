/******************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   C++ stream implemented with LZMA/XZ compression.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define BASE_STREAM_OUTPUT_STORAGE_LZMA_CC 1
#include "base.hh"
#include "file.hh"
#include "buffer_string.hh"
#include "stream_defs.hh"
#include "stream_output_storage.hh"
#include "stream_output_storage_lzma.hh"

namespace base {

////////////////////////////////////////////////////////////////////////////////
/////////////////////////  StreamOutputStorageLZMA  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamOutputStorageLZMA::StreamOutputStorageLZMA( const string& pathname )
:   mPathname(pathname),
    mOpen(false),
    mFile(nullptr),
    mOutputBuf{},  // std::array
    mLzmaStream{}
{
ASSERT( not mPathname.empty() );

    // (Do not open file yet, wait until StreamOutput will call Open().)
    mLzmaStream.next_in   = nullptr;
    mLzmaStream.avail_in  = 0;
    mLzmaStream.next_out  = nullptr;
    mLzmaStream.avail_out = 0;
}

StreamOutputStorageLZMA::~StreamOutputStorageLZMA()
{
    // Call Close() to ensure memory is released.
    Close();
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool StreamOutputStorageLZMA::Open( void )
{
ASSERT( not mOpen );  // don't re-open
ASSERT( not mPathname.empty() );

    // Open a file to write LZMA-compressed data into.
    mFile = fopen( mPathname.c_str(), "wb" );
    if ( mFile == nullptr )
    {
        std::cerr << "base:StreamOutputStorageLZMA: Failed to open file '" << mPathname << "'" << std::endl;
        return false;  // failed/error
    }

    // Prepare LZMA stream for use later by Write().
    mLzmaStream = LZMA_STREAM_INIT;
    mLzmaStream.next_in   = nullptr;
    mLzmaStream.avail_in  = 0;
    mLzmaStream.next_out  = nullptr;
    mLzmaStream.avail_out = 0;
    if ( lzma_easy_encoder( &mLzmaStream, COMPRESSION_LEVEL, LZMA_CHECK_CRC64 ) != LZMA_OK )
        throw std::runtime_error( "base:StreamOutputStorageLZMA: lzma_easy_encoder" );

    // Opened OK.
    mOpen = true;
    return true;
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool StreamOutputStorageLZMA::Close( void )
{
    // In this class, Close() is responsible for freeing memory and must tolerate re-closing.
    // Not an error if already closed as Close() could be called after failed open.

    if ( mOpen )
    {
        // 1. Write the remaining last chunk of LZMA's buffer.
        static uchar sNothing[10];
        mLzmaStream.next_in   = sNothing;        // no more input
        mLzmaStream.avail_in  = 0;               // no more input
        mLzmaStream.next_out  = &mOutputBuf[0];
        mLzmaStream.avail_out = OUTPUT_BUF_SIZE;
        CompressChunk( true/*inputHasClosed*/ );

        // 2. Close LZMA.
        lzma_end( &mLzmaStream );

        // 3. Close file.
        if ( mFile != nullptr )
        {
            fclose( mFile );
            mFile = nullptr;
        }

        // 4. Last step.
        mOpen = false;
    }

    return true;
}

/*******************************************************************************
 * 
 *******************************************************************************/
StreamSize StreamOutputStorageLZMA::Write( const char* buf, const StreamSize count )
{
    mLzmaStream.next_in   = reinterpret_cast<const uchar*>(buf);
    mLzmaStream.avail_in  = count;
    mLzmaStream.next_out  = &mOutputBuf[0];
    mLzmaStream.avail_out = OUTPUT_BUF_SIZE;
    return CompressChunk( false/*inputHasClosed*/ );
}

/*******************************************************************************
 * 
 *******************************************************************************/
StreamSize StreamOutputStorageLZMA::CompressChunk( const bool inputHasClosed )
{
ASSERT( mOpen );
ASSERT( mOutputBuf.size() >= OUTPUT_BUF_SIZE );  // >= because of extra padding

    // .........................................................................
    // Notes:
    // - This is based on XZ's example 01_compress_easy.c.
    // - avail_in is the count of bytes that remain to be input.
    //   LZMA decreases avail_in as it reads bytes,
    //   and when avail_in falls to zero, the user (this) must advance
    //   the next_in,avail_in to the next input chunk.
    // - "This reads up to strm->avail_in bytes of input starting
    //    from strm->next_in. avail_in will be decremented and
    //    next_in incremented by an equal amount to match the
    //    number of input bytes consumed."
    //    Up to strm->avail_out bytes of compressed output will be
    //    written starting from strm->next_out. avail_out and next_out
    //    will be incremented by an equal amount to match the number
    //            ^^^ (avail_out should decrement --Palosim--)
    //    of output bytes written.
    //    The encoder has to do internal buffering, which means that
    //    it may take quite a bit of input before the same data is
    //    available in compressed form in the output buffer."
    // - "Normally the return value of lzma_code() will be LZMA_OK
    //    until everything has been encoded."
    //    Once everything has been encoded successfully, the
    //    return value of lzma_code() will be LZMA_STREAM_END."
    // - "It is important to check for LZMA_STREAM_END. Do not
    //    assume that getting ret != LZMA_OK would mean that
    //    everything has gone well."
    // - "When lzma_code() has returned LZMA_STREAM_END,
    //   the output buffer is likely to be only partially
    //    full. Calculate how much new data there is to
    //    be written to the output file."
    // .........................................................................

    if ( UX( not mOpen ) )
        return defs::STREAM_EOF;

    // Loop until the file has been successfully compressed or until an error occurs.
    const StreamSize count = mLzmaStream.avail_in;
    lzma_action lzmaAction = inputHasClosed ? LZMA_FINISH : LZMA_RUN;
    while ( true )
    {
        // If input hasn't closed (count>0) but compression of this chunk is done,
        // then return the count, and this will be called again.
        if ( (mLzmaStream.avail_in == 0) and (not inputHasClosed) )
            return count;

        // Run LZMA compression.
        const lzma_ret lzmaRet = lzma_code( &mLzmaStream, lzmaAction );
        if ( (lzmaRet == LZMA_OK)
          or (lzmaRet == LZMA_STREAM_END) )
        {
            // Write chunk to file.
            const size_t writeSize = OUTPUT_BUF_SIZE - mLzmaStream.avail_out;
            ASSERT( writeSize >= 0 );
            if ( writeSize > 0 )
            {
                const size_t written = fwrite( &mOutputBuf[0], 1, writeSize, mFile );
                if ( UX( written != writeSize ) )
                {
                    std::cerr << "base:StreamOutputStorageLZMA: Failed to write '" << mPathname << "' writeSize=" << writeSize << " written=" << written << std::endl;
                    return defs::STREAM_ERROR;  // error
                }
            }

            // "Reset next_out and avail_out."
            mLzmaStream.next_out  = &mOutputBuf[0];
            mLzmaStream.avail_out = OUTPUT_BUF_SIZE;

            // Is LZMA finished?
            if ( lzmaRet == LZMA_STREAM_END )
                return count;
        }
        else
        {
            // "It's not LZMA_OK nor LZMA_STREAM_END, so it must be an error code. See lzma/base.h"
            std::cerr << "base:StreamOutputStorageLZMA: LZMA error '" << mPathname << "' lzmaRet=" << lzmaRet << std::endl;
            return defs::STREAM_ERROR;  // error
        }
    }

    return count;  // (never reached)
}

} // namespace base
