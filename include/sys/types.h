#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#if defined (_WIN32) || defined (__CYGWIN__)
#define __MS_types__
#endif

#ifdef __i386__
#if defined (GO32) || defined (__MSDOS__)
#define __MS_types__
#endif
#endif

# include <stddef.h>
# include <machine/types.h>

/* To ensure the stat struct's layout doesn't change when sizeof(int), etc.
   changes, we assume sizeof short and long never change and have all types
   used to define struct stat use them and not int where possible.
   Where not possible, _ST_INTxx are used.  It would be preferable to not have
   such assumptions, but until the extra fluff is necessary, it's avoided.
   No 64 bit targets use stat yet.  What to do about them is postponed
   until necessary.  */
#ifdef __GNUC__
#define _ST_INT32 __attribute__ ((__mode__ (__SI__)))
#else
#define _ST_INT32
#endif

# ifndef	_POSIX_SOURCE

#  define	physadr		physadr_t
#  define	quad		quad_t

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
# endif	/*!_POSIX_SOURCE */

#ifndef __clock_t_defined
typedef _CLOCK_T_ clock_t;
#define __clock_t_defined
#endif

#ifndef __time_t_defined
typedef unsigned long time_t;
#define __time_t_defined

struct timeval {
  long tv_sec;
  long tv_usec;
};

struct timespec { // MSH: Only used for pselect() ?
  time_t  tv_sec;   /* Seconds */
  long    tv_nsec;  /* Nanoseconds */
};

struct itimerspec {
  struct timespec  it_interval;  /* Timer period */
  struct timespec  it_value;     /* Timer expiration */
};
#endif

typedef	long	daddr_t;
typedef	char *	caddr_t;

#ifdef __MS_types__
typedef	unsigned long	ino_t;
#else
#ifdef __sparc__
typedef	unsigned long	ino_t;
#else
typedef	unsigned short	ino_t;
#endif
#endif

#ifdef __MS_types__
typedef unsigned long vm_offset_t;
typedef unsigned long vm_size_t;

#define __BIT_TYPES_DEFINED__

typedef char int8_t;
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
typedef long long int64_t;
typedef unsigned long long u_int64_t;
typedef int32_t register_t;
#endif /* __MS_types__ */

typedef	short	dev_t;

typedef	long	off_t;
typedef unsigned int mode_t;
typedef	unsigned short	uid_t;
typedef	unsigned short	gid_t;
typedef int pid_t;
typedef	long key_t;
typedef long ssize_t;

typedef unsigned short nlink_t;

#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

typedef	long	fd_mask;
#define	NFDBITS	(4 /*==sizeof(fd_mask)*/ * 8) /* bits per mask */
#ifndef	howmany
#define howmany(x,y) (((x)+((y)-1))/(y))
#endif

typedef	struct _types_fd_set {
  fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n,p)   ((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#define	FD_CLR(n,p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#define	FD_ISSET(n,p) ((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#define	FD_ZERO(p)	({ \
     size_t __i; \
     char *__tmp = (char *)p; \
     for (__i = 0; __i < sizeof (*(p)); ++__i) \
       *__tmp++ = 0; \
})

#undef __MS_types__
#undef _ST_INT32

typedef unsigned int socklen_t;

#endif	/* _SYS_TYPES_H */
