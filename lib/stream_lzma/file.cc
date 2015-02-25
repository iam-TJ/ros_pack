/******************************************************************************
 * $LastChangedDate: 2014-01-29 18:05:54 -0500 (Wed, 29 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   File functions.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define BASE_FILE_CC 1
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include "base.hh"
#include "buffer_byte.hh"
#include "buffer_string.hh"
#include "file.hh"

namespace base {

/*******************************************************************************
 * 
 *******************************************************************************/
bool IfFileExists( const string& fname )
{
    FILE* file = fopen( fname.c_str(), "rb" );
    if ( file != nullptr )
    {
        fclose( file );
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
 * 
 *******************************************************************************/
long FileSize( FILE* file )
{
    int fd = fileno( file );  // fileno() shouldn't ever fail
    struct stat s;
    if ( fstat( fd, &s ) == 0 )
        return long(s.st_size);
    else
        return -1L;
}

/*******************************************************************************
 * 
 *******************************************************************************/
long FileSize( const string& fname )
{
    FILE* file = fopen( fname.c_str(), "rb" );
    if ( file != nullptr )
    {
        const long size = FileSize( file );
        fclose( file );
        return size;
    }
    else
    {
        // Error.
        return -1;
    }
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool ReadFile( const string& fname, string& obuf )
{
    obuf.erase();  // clear buffer
    FILE* file = fopen( fname.c_str(), "rb" );
    if ( file )
    {
        long fileSize = FileSize( file );  // returns -1 if error
        if ( fileSize > 0 )
        {
            ByteBuffer<> byteBuffer( fileSize );
            uint bytesRead = fread( byteBuffer.Get(), 1, fileSize, file );  // should return 0..fileSize
            obuf.assign( byteBuffer.Get(), bytesRead );                     // assign to string the bytes actually read
        }
        fclose( file );
    }
    return obuf.size() > 0;
}

/*******************************************************************************
 * 
 *******************************************************************************/
bool ReadFile( const string& fname, StringBuffer& strbuf )
{
    return ReadFile( fname, strbuf.mBuf );
}

} // namespace base
