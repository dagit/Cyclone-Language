/* This takes the place of sys/config.h and _ansi.h */
#ifndef _CYCLONE_CONFIG_H_
#define _CYCLONE_CONFIG_H_
/* Add definitions here that are suitable for your platform */

#define __RAND_MAX 0x7fffffff
typedef int __int32_t;
typedef unsigned int __uint32_t;

/* CYGWIN definitions */
#if defined(__CYGWIN32__) || defined(__CYGWIN__)
#ifndef __cdecl
#define __cdecl __attribute__((cdecl))
#endif
/* the following is used in errno.cyc */
#define __CYCLONE_SYS_NUM_ERR 135
#else /* architectures besides CYGWIN */
#define __CYCLONE_SYS_NUM_ERR 124
#ifndef __cdecl
#define __cdecl
#endif
#endif /* architectures besides CYGWIN */

#define _EXFUN(name, proto) __cdecl name proto

#endif /* _CYCLONE_CONFIG_H */
