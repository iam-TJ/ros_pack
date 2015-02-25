/*****************************************************************************
 * $LastChangedDate: 2013-08-07 07:40:22 -0400 (Wed, 07 Aug 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Stream definitions.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS (PALOSIM, LLC) WWW.PALOSIM.COM
 *****************************************************************************/

#ifndef BASE_STREAM_DEFS_HH
#define BASE_STREAM_DEFS_HH 1

namespace base {

//typedef size_t StreamSize;
typedef long StreamSize;

//------------------------------------------------------------------------------
namespace defs {
// A value <= 0 indicates an I/O problem (error or EOF).
const StreamSize STREAM_EOF   = 0;   // C stdio EOF=-1
const StreamSize STREAM_ERROR = -1;  // typical UNIX error
} // namespace defs
//------------------------------------------------------------------------------

} // namespace base

#endif // BASE_STREAM_DEFS_HH
