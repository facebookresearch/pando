/*+-------------------------------------------------------------------
//
//  Copyright (C) Silicon Software GmbH, 1999 - 2017.
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

#ifndef IS_ALIGNED
#define IS_ALIGNED(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)
#endif

#if defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define HAS_CLOSURES
#endif
#elif defined(_MSC_VER)
#if _MSC_VER >= 1600
#define HAS_CLOSURES
#endif
#else
#pragma message("WARNING: You need to implement the check for HAS_CLOSURES for this compiler")
#endif

#if defined(__GNUC__)
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#endif /* __OS_FUNCS_H */
