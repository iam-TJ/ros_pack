/******************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   C++ input stream implemented with LZMA/XZ decompression.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#ifndef STREAM_INPUT_STORAGE_LIBLZMA_HH
#define STREAM_INPUT_STORAGE_LIBLZMA_HH 1

#include <lzma.h>
#include "buffer_string.hh"
#include "stream_defs.hh"
#include "stream_input_storage.hh"

namespace base {

class StreamInput;

////////////////////////////////////////////////////////////////////////////////
/// @brief C++ input stream implemented with LZMA/XZ decompression.
///
/// This implements the StreamInputStorage interface using LZMA/XZ decompression.
/// A generic StreamInput object will call these methods.
///
class StreamInputStorageLZMA final : public StreamInputStorage
{
PREVENT_COPYING( StreamInputStorageLZMA )
friend class StreamInput;  // needs access to Open() etc

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------

    private: const uint INPUT_CHUNK_SIZE  = 0x1000;
    private: const uint OUTPUT_CHUNK_SIZE = 0x1000;

//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * @param   pathname
     *          Pathname of compressed file.
     ***************************************************************************/
    public: StreamInputStorageLZMA( const string& pathname );
    public: virtual ~StreamInputStorageLZMA();

//------------------------------------------------------------------------------
// Methods (internal to stream classes):
//------------------------------------------------------------------------------

    /***************************************************************************
     * (For StreamInput.)
     * Open the storage of the stream.
     ***************************************************************************/
    private: virtual bool Open( void ) override;

    /***************************************************************************
     * (For StreamInput.)
     * Close the storage of the stream.
     ***************************************************************************/
    private: virtual bool Close( void ) override;

    /***************************************************************************
     * (For StreamInput.)
     * Read from storage into a memory buffer.
     * Returns:
     * If > 0, number of bytes read.
     * If <= 0, error or EOF (STREAM_ERROR,STREAM_EOF).
     ***************************************************************************/
    private: virtual StreamSize Read( char* buf/*OUT*/, const StreamSize count ) override;

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

    private: const string mPathname;       ///< pathname of compressed file
    private: bool         mOpen;           ///< if opened
    private: StringBuffer mInputStringBuf; ///< will contain compressed file that was read
    private: uint         mInputStringIdx; ///< index into input StringBuffer
    private: lzma_stream  mLzmaStream;     ///< underlying LZMA encoder/decoder
};

} // namespace base

#endif // STREAM_INPUT_STORAGE_LIBLZMA_HH
