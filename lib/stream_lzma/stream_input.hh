/*****************************************************************************
 * $LastChangedDate: 2013-08-07 07:40:22 -0400 (Wed, 07 Aug 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Input stream class composed with a stream storage class.
 * @remarks Based on code written by Nicolai Josuttis.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

#ifndef BASE_STREAM_INPUT_HH
#define BASE_STREAM_INPUT_HH 1

#include <iostream>
#include <streambuf>
#include "base.hh"

namespace base {

class StreamInputStorage;

////////////////////////////////////////////////////////////////////////////////
/// @brief Class for an input stream class composed with a class for the stream's storage.
///
/// This is the generic component of the Composition design pattern.
/// The concrete component is the StreamInputStorage that is a constructor arg.
///
/// The idea is to decouple the C++ stream from its storage.
/// For example, StreamInputStorageZip and StreamInputStorageLZMA can use compressed files
/// as the storage of a C++ stream.  Yet StreamInputStorage can be written
/// ignorant of the specific storage.
///
/// The streambuf code is derived from Nicolai Josuttis's STL book.
///
/// @verbatim
/// Example:
///     StreamInput in( new StreamInputStorageDerived() );
///     in >> field;
/// @endverbatim
///
class StreamInput : public std::istream
{
PREVENT_COPYING(StreamInput)
//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * Compose StreamInput with a StreamInputStorage object.
     ***************************************************************************/
    public: StreamInput( shptr<StreamInputStorage> streamInputStorage );
    public: virtual ~StreamInput();

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
        public: StreamBuffer( shptr<StreamInputStorage> streamInputStorage );
        public: virtual ~StreamBuffer();
        private: virtual int underflow( void ) override;

        // Data:
        private: CLASS_CONSTEXPR int PUTBACK_SIZE = 4;    ///< size of putback area
        private: CLASS_CONSTEXPR int DATA_SIZE    = 1024; ///< size of the data buffer
        private: CLASS_CONSTEXPR int BUFFER_SIZE  = PUTBACK_SIZE + DATA_SIZE;

        private: shptr<StreamInputStorage> mStreamInputStorage;  ///< the source of the stream
        private: char                      mBuffer[BUFFER_SIZE]; ///< data buffer
    };

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

    private: StreamBuffer mStreamBuffer;
};

} // namespace base

#endif // BASE_STREAM_INPUT_HH
