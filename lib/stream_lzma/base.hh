/*****************************************************************************
 * $LastChangedDate: 2013-08-07 07:40:22 -0400 (Wed, 07 Aug 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Base code.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

#ifndef BASE_HH
#define BASE_HH 1

#include <string>
#include <cassert>
#include <exception>
#include <stdexcept>
#include <iostream>

using std::string;

namespace base {

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned char uchar;
typedef unsigned int uint;

#define ASSERT assert
#define COMPILE_TIME_ASSERT( COND, TEXT ) static_assert( COND, TEXT )

// #############################################################################
// TODO/FIXME REPLACE WITH A SMART PTR CLASS.
class Shared { };
template<typename SHARED> class shptr
{
public:
    shptr( void )
    {
        mObj = nullptr;
    }

    shptr( SHARED* obj )
    {
        mObj = obj;
    }

    SHARED* operator->( void ) const
    {
        return mObj;
    }

    SHARED* PTR( void ) const
    {
        return mObj;
    }

    private: SHARED* mObj;
};

#define PREVENT_COPYING( CLASS )

#define CLASS_CONSTEXPR static const

#define EX( EXPR ) (EXPR)
#define UX( EXPR ) (EXPR)

} // namespace base

#endif // BASE_HH
