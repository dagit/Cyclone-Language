/* This takes the place of sys/config.h and _ansi.h */
#ifndef _CYCLONE_CONFIG_H_
#define _CYCLONE_CONFIG_H_
/* Add definitions here that are suitable for your platform */

#define __RAND_MAX 0x7fffffff
typedef int __int32_t;
typedef unsigned int __uint32_t;

#define _PTR void *
#define _AND ,
#define _NOARGS void
#define _CONST const
#define _VOLATILE volatile
#define _SIGNED signed
#define _DOTS , ...
#define _VOID void


/* CYGWIN definitions */
#if defined(__CYGWIN32__) || defined(__CYGWIN__)
#if defined(__INSIDE_CYGWIN__) || defined(_COMPILING_NEWLIB)
#define __IMPORT
#else 
#define __IMPORT __attribute__((dllimport))
#endif
#ifndef __cdecl
#define __cdecl __attribute__((cdecl))
#endif
/* the following is used in errno.cyc */
#define __CYCLONE_SYS_NUM_ERR 135
#else /* architectures besides CYGWIN */
#define __CYCLONE_SYS_NUM_ERR 124
#define __IMPORT
#ifndef __cdecl
#define __cdecl
#endif
#endif /* architectures besides CYGWIN */

#define _EXFUN(name, proto) __cdecl name proto
#define _EXPARM(name, proto) (* __cdecl name) proto
#define _DEFUN(name, arglist, args) name(args)
#define _DEFUN_VOID(name) name(_NOARGS)
#define _CAST_VOID (void)
#define _LONG_DOUBLE long double
#define _ATTRIBUTE(attrs) __attribute__ (attrs)

#endif /* _CYCLONE_CONFIG_H */
