/*****************************************************************************
 * $LastChangedDate: 2013-09-04 00:31:53 -0400 (Wed, 04 Sep 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Abstract class for storage of a stream (output).
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

// StreamInput/OutputStorage are very similar identical except for comments and Read()/Write().

#ifndef BASE_STREAM_STORAGE_OUTPUT_HH
#define BASE_STREAM_STORAGE_OUTPUT_HH 1

#include "stream_defs.hh"

namespace base {

class StreamOutput;

////////////////////////////////////////////////////////////////////////////////
/// @brief Abstract class for storage of a stream (output).
///
/// A derivative of StreamOutputStorage is the concrete component of the Composition design pattern.
/// The abstract component is the OutputStream.
///
/// Example:
/// StreamOutput out( new StreamOutputStorageDerived() );
/// out << field0;
///
class StreamOutputStorage /*extendable*/ : public Shared
{
PREVENT_COPYING( StreamOutputStorage )
friend class StreamOutput;

//------------------------------------------------------------------------------
// Methods:
//------------------------------------------------------------------------------

    /***************************************************************************
     * The constructor/destructor shouldn't open/close the storage.
     * Rather, let StreamOutput call Open() and Close().
     ***************************************************************************/
    public: StreamOutputStorage( void )    { }
    public: virtual ~StreamOutputStorage() { }

//------------------------------------------------------------------------------
// Methods (internal to stream classes):
//------------------------------------------------------------------------------

    /***************************************************************************
     * (For StreamOutput.)
     * Open the storage of the stream (for output).
     ***************************************************************************/
    protected: virtual bool Open( void ) = 0;

    /***************************************************************************
     * (For StreamOutput.)
     * Close the storage of the stream.
     ***************************************************************************/
    protected: virtual bool Close( void ) = 0;

    /***************************************************************************
     * (For StreamOutput.)
     * Write to storage from a memory buffer.
     * Returns:
     * If > 0, number of bytes read.
     * If <= 0, error or EOF (STREAM_ERROR,STREAM_EOF).
     ***************************************************************************/
    private: virtual StreamSize Write( const char* buf, const StreamSize count ) = 0;

//------------------------------------------------------------------------------
// Data:
//------------------------------------------------------------------------------

};

} // namespace base

#endif // BASE_STREAM_STORAGE_OUTPUT_HH
