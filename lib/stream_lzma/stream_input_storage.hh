/*****************************************************************************
 * $LastChangedDate: 2013-09-04 00:31:53 -0400 (Wed, 04 Sep 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Abstract class for storage of a stream (input).
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

// StreamInput/OutputStorage are very similar identical except for comments and Read()/Write().

#ifndef BASE_STREAM_INPUT_STORAGE_HH
#define BASE_STREAM_INPUT_STORAGE_HH 1

#include "base.hh"
#include "stream_defs.hh"

namespace base {

class StreamInput;

////////////////////////////////////////////////////////////////////////////////
/// @brief Abstract class for storage of a stream (input).
///
/// A derivative of StreamInputStorage is the concrete component of the Composition design pattern.
/// The abstract component is the InputStream.
///
/// Example:
/// StreamInput in( new StreamInputStorageDerived() );
/// in >> field;
///
class StreamInputStorage /*extendable*/ : public Shared
{
PREVENT_COPYING( StreamInputStorage )
friend class StreamInput;

//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * The constructor/destructor shouldn't open/close the storage.
     * Rather, let StreamInput call Open() and Close().
     ***************************************************************************/
    public: StreamInputStorage( void )    { }
    public: virtual ~StreamInputStorage() { }

//------------------------------------------------------------------------------
// Methods (internal to stream classes):
//------------------------------------------------------------------------------

    /***************************************************************************
     * (For StreamInput.)
     * Open the storage of the stream (for input).
     ***************************************************************************/
    protected: virtual bool Open( void ) = 0;

    /***************************************************************************
     * (For StreamInput.)
     * Close the storage of the stream.
     ***************************************************************************/
    protected: virtual bool Close( void ) = 0;

    /***************************************************************************
     * (For StreamInput.)
     * Read from storage into a memory buffer.
     * Returns:
     * If > 0, number of bytes read.
     * If <= 0, error or EOF (STREAM_ERROR,STREAM_EOF).
    ***************************************************************************/
    protected: virtual StreamSize Read( char* buf/*OUT*/, const StreamSize count ) = 0;

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

};

} // namespace base

#endif // BASE_STREAM_INPUT_STORAGE_HH
