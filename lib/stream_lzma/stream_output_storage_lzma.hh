/******************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   C++ output stream implemented with LZMA/XZ decompression.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#ifndef STREAM_OUTPUT_STORAGE_LIBLZMA_HH
#define STREAM_OUTPUT_STORAGE_LIBLZMA_HH 1

#include <cstdio>
#include <array>
#include <lzma.h>
#include "buffer_string.hh"
#include "stream_defs.hh"
#include "stream_output_storage.hh"

namespace base {

class StreamOutput;

////////////////////////////////////////////////////////////////////////////////
/// @brief LZMA-compressed file as a storage of a C++ stream.
///
/// This implements the StreamOutputStorage interface using LZMA/XZ decompression.
/// A generic StreamOutput object will call these methods.
///
class StreamOutputStorageLZMA final : public StreamOutputStorage
{
PREVENT_COPYING( StreamOutputStorageLZMA )
friend class StreamOutput;  // needs access to Open() etc

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------

    private: CLASS_CONSTEXPR uint COMPRESSION_LEVEL = 4;
    private: CLASS_CONSTEXPR uint OUTPUT_BUF_SIZE = 0x1000;
    private: enum class EWriteChunk { YES, NO };

//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * @param   pathname
     *          Pathname of compressed file.
     ***************************************************************************/
    public: StreamOutputStorageLZMA( const string& pathname );
    public: virtual ~StreamOutputStorageLZMA();

//------------------------------------------------------------------------------
// Methods (internal to stream classes):
//------------------------------------------------------------------------------

    /***************************************************************************
     * (For StreamOutput.)
     * Open the storage of the stream.
     ***************************************************************************/
    private: virtual bool Open( void ) override;

    /***************************************************************************
     * (For StreamOutput.)
     * Close the storage of the stream.
     ***************************************************************************/
    private: virtual bool Close( void ) override;

    /***************************************************************************
     * (For StreamOutput.)
     * Write to storage from a memory buffer.
     * Returns:
     * If > 0, number of bytes read.
     * If <= 0, error or EOF (STREAM_ERROR,STREAM_EOF).
     ***************************************************************************/
    private: virtual StreamSize Write( const char* buf, const StreamSize count ) override;

    /***************************************************************************
     * Subroutines.
     ***************************************************************************/
    private: StreamSize CompressChunk( const bool inputHasClosed );

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

    private: typedef std::array<uchar,OUTPUT_BUF_SIZE+32> OutputBuf;

    private: const string mPathname;   ///< pathname of compressed file
    private: bool         mOpen;       ///< if compressed file was opened
    private: FILE*        mFile;       ///< file to write into
    private: OutputBuf    mOutputBuf;  ///< holds output of LZMA which will be written to file
    private: lzma_stream  mLzmaStream; ///< underlying LZMA encoder/decoder
};

} // namespace base

#endif // STREAM_OUTPUT_STORAGE_LIBLZMA_HH
