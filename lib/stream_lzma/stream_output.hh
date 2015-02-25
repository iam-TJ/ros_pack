/*****************************************************************************
 * $LastChangedDate: 2013-08-07 07:40:22 -0400 (Wed, 07 Aug 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Output stream class composed with a stream storage class.
 * @remarks Based on code written by Nicolai Josuttis.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

#ifndef BASE_STREAM_OUTPUT_HH
#define BASE_STREAM_OUTPUT_HH 1

#include <iostream>
#include <streambuf>
#include "base.hh"

namespace base {

class StreamOutputStorage;

////////////////////////////////////////////////////////////////////////////////
/// @brief Class for an output stream class composed with a class for the stream's storage.
///
/// This is the generic component of the Composition design pattern.
/// The concrete component is the StreamOutpuStorage that is a constructor arg.
///
/// The idea is to decouple the C++ stream from its storage.
/// For example, StreamOutputStorageZip and StreamOutputStorageLZMA can use compressed files
/// as the storage of a C++ stream.  Yet StreamOutputStorage can be written
/// ignorant of the specific storage.
///
/// The streambuf code is derived from Nicolai Josuttis's STL book.
///
/// @verbatim
/// Example:
///     StreamOutput out( new StreamOutputStorageDerived() );
///     out << field;
/// @endverbatim
///
class StreamOutput : public std::ostream
{
PREVENT_COPYING(StreamOutput)
//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * Compose StreamOutput with a StreamOutputStorage object.
     ***************************************************************************/
    public: StreamOutput( shptr<StreamOutputStorage> streamOutputStorage );
    public: virtual ~StreamOutput();

//------------------------------------------------------------------------------
// (internal class)
//------------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////
    /// @brief streambuf (internal)
    ///
    private: class StreamBuffer : public std::streambuf
    {
    PREVENT_COPYING(StreamBuffer)
        // Methods:
        public: StreamBuffer( shptr<StreamOutputStorage> streamOutputStorage );
        public: virtual ~StreamBuffer();
        private: virtual int overflow( int c ) override;  // write one char
        private: virtual std::streamsize xsputn( const char* buf, std::streamsize count ) override;  // write multiple chars

        // Data:
        private: shptr<StreamOutputStorage> mStreamOutputStorage;  ///< the storage of the stream
    };

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

    private: StreamBuffer mStreamBuffer;
};

} // namespace base

#endif // BASE_STREAM_OUTPUT_HH
