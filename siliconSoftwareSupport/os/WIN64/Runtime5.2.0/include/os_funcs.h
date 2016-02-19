/*+-------------------------------------------------------------------
//
//  Copyright (C) Silicon Software GmbH, 1999 - 2009.
//
//  File:       os_funcs.h
//
//  Contents:   definitions for platform abstraction functions
//
//-------------------------------------------------------------------- */

#ifndef __OS_FUNCS_H
#define __OS_FUNCS_H

#ifndef _WIN32

#define ERROR_SUCCESS 0
#define NO_ERROR 0L

#include <errno.h>
static inline int GetLastError(void)
{
	return errno;
}

#include <unistd.h>
#define Sleep(x) usleep((x) * 1000)

#define UI64(x)	((uint64_t)(x##ULL))
#define UI32(x)	((uint32_t)(x##UI))

#else /* _WIN32 */

#include <windows.h>

#define UI64(x)	(x##ui64)
#define UI32(x)	(x##ui32)

#endif /* _WIN32 */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#endif /* __OS_FUNCS_H */
