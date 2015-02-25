/******************************************************************************
 * $LastChangedDate: 2013-09-01 20:32:42 -0400 (Sun, 01 Sep 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Class for an output stream class composed with a class for the stream's storage.
 * @remarks Based on code written by Nicolai Josuttis.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define BASE_STREAM_OUTPUT_CC 1
#include <cstring>
#include <cstdio> // EOF
#include <limits>
#include "base.hh"
#include "stream_output.hh"
#include "stream_output_storage.hh"

namespace base {

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  StreamOutput  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamOutput::StreamOutput( shptr<StreamOutputStorage> streamOutputStorage )
:   std::ostream(0),  // call ctor variant instead of the default ctor (for arcane reason)
    mStreamBuffer(streamOutputStorage)
{
COMPILE_TIME_ASSERT( std::numeric_limits<StreamSize>::is_signed, "StreamSize should be signed" );

    // Set the underlying stream buffer (doesn't do a read despite its name).
    rdbuf( &mStreamBuffer );
}

StreamOutput::~StreamOutput()
{
    // NOP
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////  StreamOutput::StreamBuffer   /////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamOutput::StreamBuffer::StreamBuffer( shptr<StreamOutputStorage> streamOutputStorage )
:   mStreamOutputStorage(streamOutputStorage)
{
    // Open the storage of the stream.
    mStreamOutputStorage->Open();
}

StreamOutput::StreamBuffer::~StreamBuffer()
{
    // Close the storage of the stream.
    mStreamOutputStorage->Close();
}

/*******************************************************************************
 * 
 *******************************************************************************/
int StreamOutput::StreamBuffer::overflow( int c_int )
{
    if ( EX( c_int != EOF ) )
    {
    ASSERT( (c_int >= 0) and (c_int <= 0xff) );

        char c = char(c_int);
        return mStreamOutputStorage->Write( &c, 1 );
    }
    else
    {
        return EOF;
    }
}

std::streamsize StreamOutput::StreamBuffer::xsputn( const char* buf, std::streamsize count )
{
    return mStreamOutputStorage->Write( buf, count );
}

} // namespace base
