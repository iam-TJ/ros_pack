/******************************************************************************
 * $LastChangedDate: 2014-02-05 23:13:43 -0500 (Wed, 05 Feb 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   File functions.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#ifndef BASE_FILE_HH
#define BASE_FILE_HH 1

#include "buffer_string.hh"

namespace base {

/*******************************************************************************
 * @return True if file exists.
 *******************************************************************************/
bool IfFileExists( const string& fname );

/*******************************************************************************
 * @return Size of an opened file else -1.
 *******************************************************************************/
long FileSize( FILE* file );

/*******************************************************************************
 * @return Size of a file else -1.
 *******************************************************************************/
long FileSize( const string& fname );

/*******************************************************************************
 * Read a file into a C++ string.
 *
 * ReadFile() can be used to read a file in terms of a stream (string to std::istringstream):
 * string buf;
 * if ( not ReadFile( pathname, buf ) )
 *    THROW_EXCEPTION();
 * std::istringstream ss( buf );
 *******************************************************************************/
bool ReadFile( const string& fname, string& obuf );

/*******************************************************************************
 * Read a file into a StringBuffer object.
 * A StringBuffer can be used with shptr.
 *******************************************************************************/
bool ReadFile( const string& fname, StringBuffer& strbuf );

} // namespace base

#endif /* BASE_FILE_HH */
