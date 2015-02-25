/*****************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   StringBuffer class.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

#ifndef BASE_BUFFER_STRING_HH
#define BASE_BUFFER_STRING_HH 1

#include "base.hh"

namespace base {

////////////////////////////////////////////////////////////////////////////////
/// @brief Basic string buffer class.
///
/// One purpose is to wrap a C++ string for sharing it.
/// For speed/convenience, to directly manipulate the C++ string and to avoid copying,
/// mBuf is exposed.  But the methods should be used whenever possible.
///
class StringBuffer final
{
public:
                  StringBuffer( void ) { }
                  ~StringBuffer() { }
    void          Set( const char* buf )           { mBuf.assign( buf ); }
    void          Set( const char* buf, int len )  { mBuf.assign( buf, len ); }
    void          Set( const uchar* buf, int len ) { mBuf.assign( reinterpret_cast<const char*>(buf), len ); }
    void          Erase( void )           { mBuf.erase(); }
    bool          IfEmpty( void )   const { return mBuf.size() == 0; }
    uint          Size( void )      const { return mBuf.size(); }
    const string* GetString( void ) const { return &mBuf; }
    const char*   GetChars( void )  const { return mBuf.c_str(); }
    const uchar*  GetUchars( void ) const { return reinterpret_cast<const uchar*>(mBuf.c_str()); }

public:
    string  mBuf;  ///< public for speed/convenience but use methods if appropriate
};

} // namespace base

#endif // BASE_BUFFER_STRING_HH
