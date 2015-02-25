/******************************************************************************
 * $LastChangedDate: 2013-09-01 20:32:42 -0400 (Sun, 01 Sep 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Class for an input stream class composed with a class for the stream's storage.
 * @remarks Based on code written by Nicolai Josuttis.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define BASE_STREAM_INPUT_CC 1
#include <cstring>
#include <cstdio> // EOF
#include <limits>
#include "base.hh"
#include "stream_input.hh"
#include "stream_input_storage.hh"

namespace base {

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  StreamInput  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamInput::StreamInput( shptr<StreamInputStorage> streamInputStorage )
:   std::istream(0),  // call ctor variant instead of the default ctor (for arcane reason)
    mStreamBuffer(streamInputStorage)
{
COMPILE_TIME_ASSERT( std::numeric_limits<StreamSize>::is_signed, "StreamSize should be signed" );

    // Set the underlying stream buffer (doesn't do a read despite its name).
    rdbuf( &mStreamBuffer );
}

StreamInput::~StreamInput()
{
    // NOP
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////  StreamInput::StreamBuffer   //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 
 *******************************************************************************/
StreamInput::StreamBuffer::StreamBuffer( shptr<StreamInputStorage> streamInputStorage )
:   mStreamInputStorage(streamInputStorage)
  //mBuffer[]
{
    // Open the storage of the stream.
    mStreamInputStorage->Open();

    // "Sets values for the pointers that define both the boundaries
    //  of the accessible part of the controlled input sequence and the get pointer itself."
    setg( mBuffer + DATA_SIZE,    // beginning of putback area
          mBuffer + DATA_SIZE,    // read position
          mBuffer + DATA_SIZE );  // end position
}

StreamInput::StreamBuffer::~StreamBuffer()
{
    // Close the storage of the stream.
    mStreamInputStorage->Close();
}

/*******************************************************************************
 * 
 *******************************************************************************/
int StreamInput::StreamBuffer::underflow( void )
{
    // Insert new characters into the buffer.

    // Is read position before end of buffer?
    if ( gptr() < egptr() )
        return traits_type::to_int_type( *gptr() );

    // Process size of putback area.
    // - use number of characters read.
    // - but at most size of putback area
    int numPutback  = gptr() - eback();
ASSERT( numPutback >= 0 );
    if ( numPutback > PUTBACK_SIZE )
        numPutback = PUTBACK_SIZE;

    // Copy up to PUTBACK_SIZE characters previously read into the putback area.
    char*       dest = mBuffer + (PUTBACK_SIZE - numPutback);
    const char* src  = gptr() - numPutback;
        ASSERT( (dest >= mBuffer) and (&dest[numPutback] <= &mBuffer[BUFFER_SIZE]) );  // check out-of-bounds
        ASSERT( (src  >= mBuffer) and (&src[numPutback]  <= &mBuffer[BUFFER_SIZE]) );
    memmove( dest, src, numPutback );

    // Read at most DATA_SIZE new characters.
    // A value <= 0 means an I/O problem (error or EOF).
    StreamSize num = mStreamInputStorage->Read( mBuffer + PUTBACK_SIZE, DATA_SIZE );  // similar to UNIX read()
    if ( num <= 0 )
        return EOF;  // stdio EOF

    // Reset buffer pointers.
    setg( mBuffer + (PUTBACK_SIZE - numPutback),  // beginning of putback area
          mBuffer + PUTBACK_SIZE,                 // read position
          mBuffer + PUTBACK_SIZE + num );         // end of buffer

    // Return next character.
    return traits_type::to_int_type( *gptr() );
}

} // namespace base
