/*+-------------------------------------------------------------------
//
//  Copyright (C) Silicon Software GmbH, 1999 - 2009.
//
//  File:       os_type.h
//
//  Contents:   definitions for platform dependent types
//
//--------------------------------------------------------------------*/

#ifndef __OS_TYPE_H
#define __OS_TYPE_H

/** @cond DOXY_SKIP */

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif

#ifdef _WIN32

#ifndef __attribute__
#define __attribute__(x)
#endif

#include <windows.h>

#include <basetsd.h>

typedef SSIZE_T ssize_t;

#else /* _WIN32 */

#include <unistd.h>

#define INVALID_HANDLE_VALUE -1

typedef int HANDLE;

#endif /* _WIN32 */

/** @endcond */

/** \var frameindex_t
		Integer type for frame/image counts and subbuffer counts.
    This type represents a 32 bit number on 32 bit systems (WinXP /Vista 32, Linux x86_32) and
		a 64 bit number on 64 bit systems (WinXP / Vista 64, Linux amd64)
*/

#ifdef _WIN32
#ifdef _WIN64
typedef int64_t frameindex_t;
#else /* _WIN64 */
#ifdef _MSC_VER
typedef int _W64 frameindex_t;
#else /* _MSC_VER */
typedef int frameindex_t;
#endif /* _MSC_VER */
#endif /* _WIN64 */
#else /* _WIN32 */
typedef long frameindex_t;
#endif /* _WIN32 */

/** @cond DOXY_SKIP */

/* the declarations have previously been in this file.
 * Remove that include eventually. */
#include "os_funcs.h"

/** @endcond */

#endif /* __OS_TYPE_H */
