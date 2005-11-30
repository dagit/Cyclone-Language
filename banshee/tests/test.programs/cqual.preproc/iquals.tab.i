typedef unsigned int size_t;
extern void *memcpy (void *__restrict __dest,
                     __const void *__restrict __src, size_t __n) ;
extern void *memmove (void *__dest, __const void *__src, size_t __n)
            ;
extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
                      int __c, size_t __n)
            ;
extern void *memset (void *__s, int __c, size_t __n) ;
extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
             __attribute__ ((__pure__));
extern void *memchr (__const void *__s, int __c, size_t __n)
              __attribute__ ((__pure__));
extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
            ;
extern char *strncpy (char *__restrict __dest,
                      __const char *__restrict __src, size_t __n) ;
extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
            ;
extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
                      size_t __n) ;
extern int strcmp (__const char *__s1, __const char *__s2)
             __attribute__ ((__pure__));
extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
             __attribute__ ((__pure__));
extern int strcoll (__const char *__s1, __const char *__s2)
             __attribute__ ((__pure__));
extern size_t strxfrm (char *__restrict __dest,
                       __const char *__restrict __src, size_t __n) ;
extern char *strdup (__const char *__s) __attribute__ ((__malloc__));
extern char *strchr (__const char *__s, int __c) __attribute__ ((__pure__));
extern char *strrchr (__const char *__s, int __c) __attribute__ ((__pure__));
extern size_t strcspn (__const char *__s, __const char *__reject)
             __attribute__ ((__pure__));
extern size_t strspn (__const char *__s, __const char *__accept)
             __attribute__ ((__pure__));
extern char *strpbrk (__const char *__s, __const char *__accept)
             __attribute__ ((__pure__));
extern char *strstr (__const char *__haystack, __const char *__needle)
             __attribute__ ((__pure__));
extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
            ;
extern char *__strtok_r (char *__restrict __s,
                         __const char *__restrict __delim,
                         char **__restrict __save_ptr) ;
extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
                       char **__restrict __save_ptr) ;
extern size_t strlen (__const char *__s) __attribute__ ((__pure__));
extern char *strerror (int __errnum) ;
extern char *strerror_r (int __errnum, char *__buf, size_t __buflen) ;
extern void __bzero (void *__s, size_t __n) ;
extern void bcopy (__const void *__src, void *__dest, size_t __n) ;
extern void bzero (void *__s, size_t __n) ;
extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
             __attribute__ ((__pure__));
extern char *index (__const char *__s, int __c) __attribute__ ((__pure__));
extern char *rindex (__const char *__s, int __c) __attribute__ ((__pure__));
extern int ffs (int __i) __attribute__ ((__const__));
extern int strcasecmp (__const char *__s1, __const char *__s2)
             __attribute__ ((__pure__));
extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
             __attribute__ ((__pure__));
extern char *strsep (char **__restrict __stringp,
                     __const char *__restrict __delim) ;
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;
__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
typedef unsigned long long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned int __nlink_t;
typedef long int __off_t;
typedef long long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef int __daddr_t;
typedef long int __swblk_t;
typedef int __key_t;
typedef int __clockid_t;
typedef int __timer_t;
typedef long int __blksize_t;
typedef long int __blkcnt_t;
typedef long long int __blkcnt64_t;
typedef unsigned long int __fsblkcnt_t;
typedef unsigned long long int __fsblkcnt64_t;
typedef unsigned long int __fsfilcnt_t;
typedef unsigned long long int __fsfilcnt64_t;
typedef int __ssize_t;
typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;
typedef int __intptr_t;
typedef unsigned int __socklen_t;
extern void *__rawmemchr (const void *__s, int __c);
extern __inline char *__strcpy_small (char *, __uint16_t, __uint16_t,
                                      __uint32_t, __uint32_t, size_t);
extern __inline char *
__strcpy_small (char *__dest,
                __uint16_t __src0_2, __uint16_t __src4_2,
                __uint32_t __src0_4, __uint32_t __src4_4,
                size_t __srclen)
{
  union {
    __uint32_t __ui;
    __uint16_t __usi;
    unsigned char __uc;
  } *__u = (void *) __dest;
  switch ((unsigned int) __srclen)
    {
    case 1:
      __u->__uc = '\0';
      break;
    case 2:
      __u->__usi = __src0_2;
      break;
    case 3:
      __u->__usi = __src0_2;
      __u = __extension__ ((void *) __u + 2);
      __u->__uc = '\0';
      break;
    case 4:
      __u->__ui = __src0_4;
      break;
    case 5:
      __u->__ui = __src0_4;
      __u = __extension__ ((void *) __u + 4);
      __u->__uc = '\0';
      break;
    case 6:
      __u->__ui = __src0_4;
      __u = __extension__ ((void *) __u + 4);
      __u->__usi = __src4_2;
      break;
    case 7:
      __u->__ui = __src0_4;
      __u = __extension__ ((void *) __u + 4);
      __u->__usi = __src4_2;
      __u = __extension__ ((void *) __u + 2);
      __u->__uc = '\0';
      break;
    case 8:
      __u->__ui = __src0_4;
      __u = __extension__ ((void *) __u + 4);
      __u->__ui = __src4_4;
      break;
    }
  return __dest;
}
extern __inline size_t __strcspn_c1 (__const char *__s, int __reject);
extern __inline size_t
__strcspn_c1 (__const char *__s, int __reject)
{
  register size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject)
    ++__result;
  return __result;
}
extern __inline size_t __strcspn_c2 (__const char *__s, int __reject1,
                                     int __reject2);
extern __inline size_t
__strcspn_c2 (__const char *__s, int __reject1, int __reject2)
{
  register size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject1
         && __s[__result] != __reject2)
    ++__result;
  return __result;
}
extern __inline size_t __strcspn_c3 (__const char *__s, int __reject1,
                                     int __reject2, int __reject3);
extern __inline size_t
__strcspn_c3 (__const char *__s, int __reject1, int __reject2,
              int __reject3)
{
  register size_t __result = 0;
  while (__s[__result] != '\0' && __s[__result] != __reject1
         && __s[__result] != __reject2 && __s[__result] != __reject3)
    ++__result;
  return __result;
}
extern __inline size_t __strspn_c1 (__const char *__s, int __accept);
extern __inline size_t
__strspn_c1 (__const char *__s, int __accept)
{
  register size_t __result = 0;
  while (__s[__result] == __accept)
    ++__result;
  return __result;
}
extern __inline size_t __strspn_c2 (__const char *__s, int __accept1,
                                    int __accept2);
extern __inline size_t
__strspn_c2 (__const char *__s, int __accept1, int __accept2)
{
  register size_t __result = 0;
  while (__s[__result] == __accept1 || __s[__result] == __accept2)
    ++__result;
  return __result;
}
extern __inline size_t __strspn_c3 (__const char *__s, int __accept1,
                                    int __accept2, int __accept3);
extern __inline size_t
__strspn_c3 (__const char *__s, int __accept1, int __accept2, int __accept3)
{
  register size_t __result = 0;
  while (__s[__result] == __accept1 || __s[__result] == __accept2
         || __s[__result] == __accept3)
    ++__result;
  return __result;
}
extern __inline char *__strpbrk_c2 (__const char *__s, int __accept1,
                                     int __accept2);
extern __inline char *
__strpbrk_c2 (__const char *__s, int __accept1, int __accept2)
{
  while (*__s != '\0' && *__s != __accept1 && *__s != __accept2)
    ++__s;
  return *__s == '\0' ? ((void *)0) : (char *) (size_t) __s;
}
extern __inline char *__strpbrk_c3 (__const char *__s, int __accept1,
                                     int __accept2, int __accept3);
extern __inline char *
__strpbrk_c3 (__const char *__s, int __accept1, int __accept2,
              int __accept3)
{
  while (*__s != '\0' && *__s != __accept1 && *__s != __accept2
         && *__s != __accept3)
    ++__s;
  return *__s == '\0' ? ((void *)0) : (char *) (size_t) __s;
}
extern __inline char *__strtok_r_1c (char *__s, char __sep, char **__nextp);
extern __inline char *
__strtok_r_1c (char *__s, char __sep, char **__nextp)
{
  char *__result;
  if (__s == ((void *)0))
    __s = *__nextp;
  while (*__s == __sep)
    ++__s;
  __result = ((void *)0);
  if (*__s != '\0')
    {
      __result = __s++;
      while (*__s != '\0')
        if (*__s++ == __sep)
          {
            __s[-1] = '\0';
            break;
          }
      *__nextp = __s;
    }
  return __result;
}
extern char *__strsep_g (char **__stringp, __const char *__delim);
extern __inline char *__strsep_1c (char **__s, char __reject);
extern __inline char *
__strsep_1c (char **__s, char __reject)
{
  register char *__retval = *__s;
  if (__retval != ((void *)0) && (*__s = (__extension__ (__builtin_constant_p (__reject) && (__reject) == '\0' ? (char *) __rawmemchr (__retval, __reject) : strchr (__retval, __reject)))) != ((void *)0))
    *(*__s)++ = '\0';
  return __retval;
}
extern __inline char *__strsep_2c (char **__s, char __reject1, char __reject2);
extern __inline char *
__strsep_2c (char **__s, char __reject1, char __reject2)
{
  register char *__retval = *__s;
  if (__retval != ((void *)0))
    {
      register char *__cp = __retval;
      while (1)
        {
          if (*__cp == '\0')
            {
              __cp = ((void *)0);
          break;
            }
          if (*__cp == __reject1 || *__cp == __reject2)
            {
              *__cp++ = '\0';
              break;
            }
          ++__cp;
        }
      *__s = __cp;
    }
  return __retval;
}
extern __inline char *__strsep_3c (char **__s, char __reject1, char __reject2,
                                   char __reject3);
extern __inline char *
__strsep_3c (char **__s, char __reject1, char __reject2, char __reject3)
{
  register char *__retval = *__s;
  if (__retval != ((void *)0))
    {
      register char *__cp = __retval;
      while (1)
        {
          if (*__cp == '\0')
            {
              __cp = ((void *)0);
          break;
            }
          if (*__cp == __reject1 || *__cp == __reject2 || *__cp == __reject3)
            {
              *__cp++ = '\0';
              break;
            }
          ++__cp;
        }
      *__s = __cp;
    }
  return __retval;
}
extern void *malloc (size_t __size) __attribute__ ((__malloc__));
extern void *calloc (size_t __nmemb, size_t __size)
             __attribute__ ((__malloc__));
extern char *__strdup (__const char *__string) __attribute__ ((__malloc__));
extern char *__strndup (__const char *__string, size_t __n)
             __attribute__ ((__malloc__));
typedef long int wchar_t;
typedef struct
  {
    int quot;
    int rem;
  } div_t;
typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;
extern size_t __ctype_get_mb_cur_max (void) ;
extern double atof (__const char *__nptr) __attribute__ ((__pure__));
extern int atoi (__const char *__nptr) __attribute__ ((__pure__));
extern long int atol (__const char *__nptr) __attribute__ ((__pure__));
__extension__ extern long long int atoll (__const char *__nptr)
             __attribute__ ((__pure__));
extern double strtod (__const char *__restrict __nptr,
                      char **__restrict __endptr) ;
extern long int strtol (__const char *__restrict __nptr,
                        char **__restrict __endptr, int __base) ;
extern unsigned long int strtoul (__const char *__restrict __nptr,
                                  char **__restrict __endptr, int __base)
            ;
__extension__
extern long long int strtoq (__const char *__restrict __nptr,
                             char **__restrict __endptr, int __base) ;
__extension__
extern unsigned long long int strtouq (__const char *__restrict __nptr,
                                       char **__restrict __endptr, int __base)
            ;
__extension__
extern long long int strtoll (__const char *__restrict __nptr,
                              char **__restrict __endptr, int __base) ;
__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
                                        char **__restrict __endptr, int __base)
            ;
extern double __strtod_internal (__const char *__restrict __nptr,
                                 char **__restrict __endptr, int __group)
            ;
extern float __strtof_internal (__const char *__restrict __nptr,
                                char **__restrict __endptr, int __group)
            ;
extern long double __strtold_internal (__const char *__restrict __nptr,
                                       char **__restrict __endptr,
                                       int __group) ;
extern long int __strtol_internal (__const char *__restrict __nptr,
                                   char **__restrict __endptr,
                                   int __base, int __group) ;
extern unsigned long int __strtoul_internal (__const char *__restrict __nptr,
                                             char **__restrict __endptr,
                                             int __base, int __group) ;
__extension__
extern long long int __strtoll_internal (__const char *__restrict __nptr,
                                         char **__restrict __endptr,
                                         int __base, int __group) ;
__extension__
extern unsigned long long int __strtoull_internal (__const char *
                                                   __restrict __nptr,
                                                   char **__restrict __endptr,
                                                   int __base, int __group)
            ;
extern __inline double
strtod (__const char *__restrict __nptr, char **__restrict __endptr)
{
  return __strtod_internal (__nptr, __endptr, 0);
}
extern __inline long int
strtol (__const char *__restrict __nptr, char **__restrict __endptr,
        int __base)
{
  return __strtol_internal (__nptr, __endptr, __base, 0);
}
extern __inline unsigned long int
strtoul (__const char *__restrict __nptr, char **__restrict __endptr,
         int __base)
{
  return __strtoul_internal (__nptr, __endptr, __base, 0);
}
__extension__ extern __inline long long int
strtoq (__const char *__restrict __nptr, char **__restrict __endptr,
        int __base)
{
  return __strtoll_internal (__nptr, __endptr, __base, 0);
}
__extension__ extern __inline unsigned long long int
strtouq (__const char *__restrict __nptr, char **__restrict __endptr,
         int __base)
{
  return __strtoull_internal (__nptr, __endptr, __base, 0);
}
__extension__ extern __inline long long int
strtoll (__const char *__restrict __nptr, char **__restrict __endptr,
         int __base)
{
  return __strtoll_internal (__nptr, __endptr, __base, 0);
}
__extension__ extern __inline unsigned long long int
strtoull (__const char * __restrict __nptr, char **__restrict __endptr,
          int __base)
{
  return __strtoull_internal (__nptr, __endptr, __base, 0);
}
extern __inline double
atof (__const char *__nptr)
{
  return strtod (__nptr, (char **) ((void *)0));
}
extern __inline int
atoi (__const char *__nptr)
{
  return (int) strtol (__nptr, (char **) ((void *)0), 10);
}
extern __inline long int
atol (__const char *__nptr)
{
  return strtol (__nptr, (char **) ((void *)0), 10);
}
__extension__ extern __inline long long int
atoll (__const char *__nptr)
{
  return strtoll (__nptr, (char **) ((void *)0), 10);
}
extern char *l64a (long int __n) ;
extern long int a64l (__const char *__s) __attribute__ ((__pure__));
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
typedef __loff_t loff_t;
typedef __ino_t ino_t;
typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __uid_t uid_t;
typedef __off_t off_t;
typedef __pid_t pid_t;
typedef __id_t id_t;
typedef __ssize_t ssize_t;
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
typedef __key_t key_t;
typedef __time_t time_t;
typedef __clockid_t clockid_t;
typedef __timer_t timer_t;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));
typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));
typedef int register_t __attribute__ ((__mode__ (__word__)));
typedef int __sig_atomic_t;
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
typedef __sigset_t sigset_t;
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
typedef __suseconds_t suseconds_t;
typedef long int __fd_mask;
typedef struct
  {
    __fd_mask __fds_bits[1024 / (8 * sizeof (__fd_mask))];
  } fd_set;
typedef __fd_mask fd_mask;
extern int select (int __nfds, fd_set *__restrict __readfds,
                   fd_set *__restrict __writefds,
                   fd_set *__restrict __exceptfds,
                   struct timeval *__restrict __timeout) ;
typedef __blkcnt_t blkcnt_t;
typedef __fsblkcnt_t fsblkcnt_t;
typedef __fsfilcnt_t fsfilcnt_t;
struct __sched_param
  {
    int __sched_priority;
  };
struct _pthread_fastlock
{
  long int __status;
  int __spinlock;
};
typedef struct _pthread_descr_struct *_pthread_descr;
typedef struct __pthread_attr_s
{
  int __detachstate;
  int __schedpolicy;
  struct __sched_param __schedparam;
  int __inheritsched;
  int __scope;
  size_t __guardsize;
  int __stackaddr_set;
  void *__stackaddr;
  size_t __stacksize;
} pthread_attr_t;
__extension__ typedef long long __pthread_cond_align_t;
typedef struct
{
  struct _pthread_fastlock __c_lock;
  _pthread_descr __c_waiting;
  char __padding[48 - sizeof (struct _pthread_fastlock)
                 - sizeof (_pthread_descr) - sizeof (__pthread_cond_align_t)];
  __pthread_cond_align_t __align;
} pthread_cond_t;
typedef struct
{
  int __dummy;
} pthread_condattr_t;
typedef unsigned int pthread_key_t;
typedef struct
{
  int __m_reserved;
  int __m_count;
  _pthread_descr __m_owner;
  int __m_kind;
  struct _pthread_fastlock __m_lock;
} pthread_mutex_t;
typedef struct
{
  int __mutexkind;
} pthread_mutexattr_t;
typedef int pthread_once_t;
typedef unsigned long int pthread_t;
extern long int random (void) ;
extern void srandom (unsigned int __seed) ;
extern char *initstate (unsigned int __seed, char *__statebuf,
                        size_t __statelen) ;
extern char *setstate (char *__statebuf) ;
struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };
extern int random_r (struct random_data *__restrict __buf,
                     int32_t *__restrict __result) ;
extern int srandom_r (unsigned int __seed, struct random_data *__buf) ;
extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
                        size_t __statelen,
                        struct random_data *__restrict __buf) ;
extern int setstate_r (char *__restrict __statebuf,
                       struct random_data *__restrict __buf) ;
extern int rand (void) ;
extern void srand (unsigned int __seed) ;
extern int rand_r (unsigned int *__seed) ;
extern double drand48 (void) ;
extern double erand48 (unsigned short int __xsubi[3]) ;
extern long int lrand48 (void) ;
extern long int nrand48 (unsigned short int __xsubi[3]) ;
extern long int mrand48 (void) ;
extern long int jrand48 (unsigned short int __xsubi[3]) ;
extern void srand48 (long int __seedval) ;
extern unsigned short int *seed48 (unsigned short int __seed16v[3]) ;
extern void lcong48 (unsigned short int __param[7]) ;
struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };
extern int drand48_r (struct drand48_data *__restrict __buffer,
                      double *__restrict __result) ;
extern int erand48_r (unsigned short int __xsubi[3],
                      struct drand48_data *__restrict __buffer,
                      double *__restrict __result) ;
extern int lrand48_r (struct drand48_data *__restrict __buffer,
                      long int *__restrict __result) ;
extern int nrand48_r (unsigned short int __xsubi[3],
                      struct drand48_data *__restrict __buffer,
                      long int *__restrict __result) ;
extern int mrand48_r (struct drand48_data *__restrict __buffer,
                      long int *__restrict __result) ;
extern int jrand48_r (unsigned short int __xsubi[3],
                      struct drand48_data *__restrict __buffer,
                      long int *__restrict __result) ;
extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
            ;
extern int seed48_r (unsigned short int __seed16v[3],
                     struct drand48_data *__buffer) ;
extern int lcong48_r (unsigned short int __param[7],
                      struct drand48_data *__buffer) ;
extern void *realloc (void *__ptr, size_t __size) __attribute__ ((__malloc__));
extern void free (void *__ptr) ;
extern void cfree (void *__ptr) ;
extern void *alloca (size_t __size) ;
extern void *valloc (size_t __size) __attribute__ ((__malloc__));
extern void abort (void) __attribute__ ((__noreturn__));
extern int atexit (void (*__func) (void)) ;
extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
            ;
extern void exit (int __status) __attribute__ ((__noreturn__));
extern char *getenv (__const char *__name) ;
extern char *__secure_getenv (__const char *__name) ;
extern int putenv (char *__string) ;
extern int setenv (__const char *__name, __const char *__value, int __replace)
            ;
extern int unsetenv (__const char *__name) ;
extern int clearenv (void) ;
extern char *mktemp (char *__template) ;
extern int mkstemp (char *__template) ;
extern char *mkdtemp (char *__template) ;
extern int system (__const char *__command) ;
extern char *realpath (__const char *__restrict __name,
                       char *__restrict __resolved) ;
typedef int (*__compar_fn_t) (__const void *, __const void *);
extern void *bsearch (__const void *__key, __const void *__base,
                      size_t __nmemb, size_t __size, __compar_fn_t __compar);
extern void qsort (void *__base, size_t __nmemb, size_t __size,
                   __compar_fn_t __compar);
extern int abs (int __x) __attribute__ ((__const__));
extern long int labs (long int __x) __attribute__ ((__const__));
extern div_t div (int __numer, int __denom)
             __attribute__ ((__const__));
extern ldiv_t ldiv (long int __numer, long int __denom)
             __attribute__ ((__const__));
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
                   int *__restrict __sign) ;
extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
                   int *__restrict __sign) ;
extern char *gcvt (double __value, int __ndigit, char *__buf) ;
extern char *qecvt (long double __value, int __ndigit,
                    int *__restrict __decpt, int *__restrict __sign) ;
extern char *qfcvt (long double __value, int __ndigit,
                    int *__restrict __decpt, int *__restrict __sign) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf) ;
extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
                   int *__restrict __sign, char *__restrict __buf,
                   size_t __len) ;
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
                   int *__restrict __sign, char *__restrict __buf,
                   size_t __len) ;
extern int qecvt_r (long double __value, int __ndigit,
                    int *__restrict __decpt, int *__restrict __sign,
                    char *__restrict __buf, size_t __len) ;
extern int qfcvt_r (long double __value, int __ndigit,
                    int *__restrict __decpt, int *__restrict __sign,
                    char *__restrict __buf, size_t __len) ;
extern int mblen (__const char *__s, size_t __n) ;
extern int mbtowc (wchar_t *__restrict __pwc,
                   __const char *__restrict __s, size_t __n) ;
extern int wctomb (char *__s, wchar_t __wchar) ;
extern size_t mbstowcs (wchar_t *__restrict __pwcs,
                        __const char *__restrict __s, size_t __n) ;
extern size_t wcstombs (char *__restrict __s,
                        __const wchar_t *__restrict __pwcs, size_t __n)
            ;
extern int rpmatch (__const char *__response) ;
extern int getloadavg (double __loadavg[], int __nelem) ;
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
typedef struct _IO_FILE FILE;
typedef struct _IO_FILE __FILE;
typedef unsigned int wint_t;
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
enum
{
  __GCONV_OK = 0,
  __GCONV_NOCONV,
  __GCONV_NODB,
  __GCONV_NOMEM,
  __GCONV_EMPTY_INPUT,
  __GCONV_FULL_OUTPUT,
  __GCONV_ILLEGAL_INPUT,
  __GCONV_INCOMPLETE_INPUT,
  __GCONV_ILLEGAL_DESCRIPTOR,
  __GCONV_INTERNAL_ERROR
};
enum
{
  __GCONV_IS_LAST = 0x0001,
  __GCONV_IGNORE_ERRORS = 0x0002
};
struct __gconv_step;
struct __gconv_step_data;
struct __gconv_loaded_object;
struct __gconv_trans_data;
typedef int (*__gconv_fct) (struct __gconv_step *, struct __gconv_step_data *,
                            __const unsigned char **, __const unsigned char *,
                            unsigned char **, size_t *, int, int);
typedef wint_t (*__gconv_btowc_fct) (struct __gconv_step *, unsigned char);
typedef int (*__gconv_init_fct) (struct __gconv_step *);
typedef void (*__gconv_end_fct) (struct __gconv_step *);
typedef int (*__gconv_trans_fct) (struct __gconv_step *,
                                  struct __gconv_step_data *, void *,
                                  __const unsigned char *,
                                  __const unsigned char **,
                                  __const unsigned char *, unsigned char **,
                                  size_t *);
typedef int (*__gconv_trans_context_fct) (void *, __const unsigned char *,
                                          __const unsigned char *,
                                          unsigned char *, unsigned char *);
typedef int (*__gconv_trans_query_fct) (__const char *, __const char ***,
                                        size_t *);
typedef int (*__gconv_trans_init_fct) (void **, const char *);
typedef void (*__gconv_trans_end_fct) (void *);
struct __gconv_trans_data
{
  __gconv_trans_fct __trans_fct;
  __gconv_trans_context_fct __trans_context_fct;
  __gconv_trans_end_fct __trans_end_fct;
  void *__data;
  struct __gconv_trans_data *__next;
};
struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;
  int __counter;
  char *__from_name;
  char *__to_name;
  __gconv_fct __fct;
  __gconv_btowc_fct __btowc_fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;
  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;
  int __stateful;
  void *__data;
};
struct __gconv_step_data
{
  unsigned char *__outbuf;
  unsigned char *__outbufend;
  int __flags;
  int __invocation_counter;
  int __internal_use;
  __mbstate_t *__statep;
  __mbstate_t __state;
  struct __gconv_trans_data *__trans;
};
typedef struct __gconv_info
{
  size_t __nsteps;
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data [0];
} *__gconv_t;
typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
struct _IO_jump_t; struct _IO_FILE;
typedef void _IO_lock_t;
struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};
enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
struct _IO_FILE {
  int _flags;
  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;
  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _flags2;
  __off_t _old_offset;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;
  __off64_t _offset;
  void *__pad1;
  void *__pad2;
  int _mode;
  char _unused2[15 * sizeof (int) - 2 * sizeof (void *)];
};
typedef struct _IO_FILE _IO_FILE;
struct _IO_FILE_plus;
extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);
typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
                                 size_t __n);
typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);
typedef int __io_close_fn (void *__cookie);
extern int __underflow (_IO_FILE *) ;
extern int __uflow (_IO_FILE *) ;
extern int __overflow (_IO_FILE *, int) ;
extern wint_t __wunderflow (_IO_FILE *) ;
extern wint_t __wuflow (_IO_FILE *) ;
extern wint_t __woverflow (_IO_FILE *, wint_t) ;
extern int _IO_getc (_IO_FILE *__fp) ;
extern int _IO_putc (int __c, _IO_FILE *__fp) ;
extern int _IO_feof (_IO_FILE *__fp) ;
extern int _IO_ferror (_IO_FILE *__fp) ;
extern int _IO_peekc_locked (_IO_FILE *__fp) ;
extern void _IO_flockfile (_IO_FILE *) ;
extern void _IO_funlockfile (_IO_FILE *) ;
extern int _IO_ftrylockfile (_IO_FILE *) ;
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
                        __gnuc_va_list, int *__restrict) ;
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
                         __gnuc_va_list) ;
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t) ;
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t) ;
extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int) ;
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int) ;
extern void _IO_free_backup_area (_IO_FILE *) ;
typedef _G_fpos_t fpos_t;
extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;
extern int remove (__const char *__filename) ;
extern int rename (__const char *__old, __const char *__new) ;
extern FILE *tmpfile (void) ;
extern char *tmpnam (char *__s) ;
extern char *tmpnam_r (char *__s) ;
extern char *tempnam (__const char *__dir, __const char *__pfx)
             __attribute__ ((__malloc__));
extern int fclose (FILE *__stream) ;
extern int fflush (FILE *__stream) ;
extern int fflush_unlocked (FILE *__stream) ;
extern FILE *fopen (__const char *__restrict __filename,
                    __const char *__restrict __modes) ;
extern FILE *freopen (__const char *__restrict __filename,
                      __const char *__restrict __modes,
                      FILE *__restrict __stream) ;
extern FILE *fdopen (int __fd, __const char *__modes) ;
extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) ;
extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
                    int __modes, size_t __n) ;
extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
                       size_t __size) ;
extern void setlinebuf (FILE *__stream) ;
extern int fprintf (FILE *__restrict __stream,
                    __const char *__restrict __format, ...) ;
extern int printf (__const char *__restrict __format, ...) ;
extern int sprintf (char *__restrict __s,
                    __const char *__restrict __format, ...) ;
extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
                     __gnuc_va_list __arg) ;
extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg)
            ;
extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
                     __gnuc_va_list __arg) ;
extern int snprintf (char *__restrict __s, size_t __maxlen,
                     __const char *__restrict __format, ...)
             __attribute__ ((__format__ (__printf__, 3, 4)));
extern int vsnprintf (char *__restrict __s, size_t __maxlen,
                      __const char *__restrict __format, __gnuc_va_list __arg)
             __attribute__ ((__format__ (__printf__, 3, 0)));
extern int fscanf (FILE *__restrict __stream,
                   __const char *__restrict __format, ...) ;
extern int scanf (__const char *__restrict __format, ...) ;
extern int sscanf (__const char *__restrict __s,
                   __const char *__restrict __format, ...) ;
extern int fgetc (FILE *__stream) ;
extern int getc (FILE *__stream) ;
extern int getchar (void) ;
extern int getc_unlocked (FILE *__stream) ;
extern int getchar_unlocked (void) ;
extern int fgetc_unlocked (FILE *__stream) ;
extern int fputc (int __c, FILE *__stream) ;
extern int putc (int __c, FILE *__stream) ;
extern int putchar (int __c) ;
extern int fputc_unlocked (int __c, FILE *__stream) ;
extern int putc_unlocked (int __c, FILE *__stream) ;
extern int putchar_unlocked (int __c) ;
extern int getw (FILE *__stream) ;
extern int putw (int __w, FILE *__stream) ;
extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
            ;
extern char *gets (char *__s) ;
extern int fputs (__const char *__restrict __s, FILE *__restrict __stream)
            ;
extern int puts (__const char *__s) ;
extern int ungetc (int __c, FILE *__stream) ;
extern size_t fread (void *__restrict __ptr, size_t __size,
                     size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
                      size_t __n, FILE *__restrict __s) ;
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
                              size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
                               size_t __n, FILE *__restrict __stream) ;
extern int fseek (FILE *__stream, long int __off, int __whence) ;
extern long int ftell (FILE *__stream) ;
extern void rewind (FILE *__stream) ;
extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos)
            ;
extern int fsetpos (FILE *__stream, __const fpos_t *__pos) ;
extern void clearerr (FILE *__stream) ;
extern int feof (FILE *__stream) ;
extern int ferror (FILE *__stream) ;
extern void clearerr_unlocked (FILE *__stream) ;
extern int feof_unlocked (FILE *__stream) ;
extern int ferror_unlocked (FILE *__stream) ;
extern void perror (__const char *__s) ;
extern int sys_nerr;
extern __const char *__const sys_errlist[];
extern int fileno (FILE *__stream) ;
extern int fileno_unlocked (FILE *__stream) ;
extern FILE *popen (__const char *__command, __const char *__modes) ;
extern int pclose (FILE *__stream) ;
extern char *ctermid (char *__s) ;
extern void flockfile (FILE *__stream) ;
extern int ftrylockfile (FILE *__stream) ;
extern void funlockfile (FILE *__stream) ;
extern __inline int
vprintf (__const char *__restrict __fmt, __gnuc_va_list __arg)
{
  return vfprintf (stdout, __fmt, __arg);
}
extern __inline int
getchar (void)
{
  return _IO_getc (stdin);
}
extern __inline int
getc_unlocked (FILE *__fp)
{
  return ((__fp)->_IO_read_ptr >= (__fp)->_IO_read_end ? __uflow (__fp) : *(unsigned char *) (__fp)->_IO_read_ptr++);
}
extern __inline int
getchar_unlocked (void)
{
  return ((stdin)->_IO_read_ptr >= (stdin)->_IO_read_end ? __uflow (stdin) : *(unsigned char *) (stdin)->_IO_read_ptr++);
}
extern __inline int
putchar (int __c)
{
  return _IO_putc (__c, stdout);
}
extern __inline int
fputc_unlocked (int __c, FILE *__stream)
{
  return (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}
extern __inline int
putc_unlocked (int __c, FILE *__stream)
{
  return (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}
extern __inline int
putchar_unlocked (int __c)
{
  return (((stdout)->_IO_write_ptr >= (stdout)->_IO_write_end) ? __overflow (stdout, (unsigned char) (__c)) : (unsigned char) (*(stdout)->_IO_write_ptr++ = (__c)));
}
extern __inline int
feof_unlocked (FILE *__stream)
{
  return (((__stream)->_flags & 0x10) != 0);
}
extern __inline int
ferror_unlocked (FILE *__stream)
{
  return (((__stream)->_flags & 0x20) != 0);
}
typedef struct region_ *region;
extern region permanent;
void region_init(void);
region newregion(void);
region newsubregion(region parent);
typedef int type_t;
void *__rc_typed_ralloc(region r, size_t size, type_t type);
void *__rc_typed_rarrayalloc(region r, size_t n, size_t size, type_t type);
void *__rc_typed_rarrayextend(region r, void *old, size_t n, size_t size, type_t type);
void typed_rarraycopy(void *to, void *from, size_t n, size_t size, type_t type);
void *__rc_ralloc_small0(region r, size_t size);
char *__rc_rstralloc(region r, size_t size);
char *__rc_rstralloc0(region r, size_t size);
char *__rc_rstrdup(region r, const char *s);
char *__rc_rstrextend(region r, const char *old, size_t newsize);
char *__rc_rstrextend0(region r, const char *old, size_t newsize);
void deleteregion(region r);
void deleteregion_ptr(region *r);
void deleteregion_array(int n, region *regions);
region regionof(void *ptr);
typedef void (*nomem_handler)(void);
nomem_handler set_nomem_handler(nomem_handler newhandler);
void findrefs(region r, void *from, void *to);
void findgrefs(region r);
void findrrefs(region r, region from);
typedef unsigned char bool;
typedef struct {
  char *data;
  int length;
} cstring;
cstring make_cstring(region r, const char *s, int l);
cstring str2cstring(region r, const char *s);
typedef long long largest_int;
typedef unsigned long long largest_uint;
typedef int (*dd_cmp_fn) (void*, void*);
typedef struct dd_list *dd_list;
typedef struct dd_list_pos
{
  struct dd_list_pos *next;
  struct dd_list_pos *previous;
  void *data;
} *dd_list_pos;
dd_list dd_new_list(region r);
void dd_add_first(region r, dd_list l, void *data);
void dd_add_last(region r, dd_list l, void *data);
void dd_insert_before(region r, dd_list_pos where, void *data);
void dd_insert_after(region r, dd_list_pos where, void *data);
void dd_remove(dd_list_pos what);
dd_list_pos dd_first(dd_list l);
dd_list_pos dd_last(dd_list l);
unsigned long dd_length(dd_list l);
void dd_append(dd_list l1, dd_list l2);
dd_list dd_copy(region r, dd_list l);
void dd_free_list(dd_list l, void (*delete)(dd_list_pos p));
dd_list_pos dd_find(dd_list l, void *find);
dd_list_pos dd_search(dd_list l, dd_cmp_fn f, void *find);
void dd_sort (dd_list l, dd_cmp_fn f);
void dd_remove_all_matches_from(dd_list_pos begin, dd_cmp_fn f, void* find);
void dd_remove_dups(dd_list l, dd_cmp_fn f);
static inline dd_list dd_fix_null(region r, dd_list l)
{
  if (l == ((void *)0))
    return dd_new_list(r);
  else
    return l;
}
typedef int (*set_cmp_fn)(void *e1, void *e2);
typedef struct Location
{
  char *filename;
  unsigned long lineno;
  unsigned long filepos;
  bool in_system_header;
  int location_index;
} *location;
unsigned long location_hash(location loc);
bool location_eq(location loc1, location loc2);
int location_cmp(location loc1, location loc2);
int location_index(location loc);
typedef dd_list loc_set; typedef dd_list_pos loc_set_scanner; static inline loc_set empty_loc_set (region r) { return dd_new_list(r); } static inline loc_set loc_set_copy(region r, loc_set s) { if (s == ((void *)0)) return ((void *)0); else return dd_copy(r, s); } static inline bool loc_set_empty(loc_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool loc_set_member(loc_set s, location elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) location_cmp, (void *) elt) != ((void *)0); } static inline int loc_set_size(loc_set s) { if (s == ((void *)0)) return 0; else return dd_length(s); } static inline bool loc_set_insert(region r, loc_set *s, location elt) { *s = dd_fix_null(r, *s); if (! loc_set_member(*s, elt)) { dd_add_first(r, *s, (void *) elt); return 1; } return 0; } static inline bool loc_set_insert_last(region r, loc_set *s, location elt) { *s = dd_fix_null(r, *s); if (! loc_set_member(*s, elt)) { dd_add_last(r, *s, (void *) elt); return 1; } return 0; } static inline bool loc_set_insert_nocheck(region r, loc_set *s, location elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool loc_set_insert_last_nocheck(region r, loc_set *s, location elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline void loc_set_remove(loc_set *s, location elt) { if (*s) dd_remove_all_matches_from(dd_first(*s), (dd_cmp_fn) location_cmp, (void *) elt); } static inline loc_set loc_set_union(loc_set s1, loc_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); dd_remove_dups(s1, (dd_cmp_fn)location_cmp); return s1; } static inline loc_set loc_set_union_nocheck(loc_set s1, loc_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool loc_set_single(loc_set s) { return loc_set_size(s) == 1; } static inline void loc_set_sort(loc_set s) { if (s) dd_sort(s, (set_cmp_fn) location_cmp); } static inline void loc_set_remove_dups(int (*cmp)(location, location), loc_set s) { if (s) dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void loc_set_scan(loc_set s, loc_set_scanner *ss) { if (s) *ss = dd_first(s); else *ss = ((void *)0); } static inline location loc_set_next(loc_set_scanner *ss) { location result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((location)((*ss)->data)); *ss = ((*ss)->next); return result; };
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
__extension__
typedef unsigned long long int uint64_t;
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
__extension__
typedef long long int int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
__extension__
typedef unsigned long long int uint_least64_t;
typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
__extension__
typedef long long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
__extension__
typedef unsigned long long int uint_fast64_t;
typedef int intptr_t;
typedef unsigned int uintptr_t;
__extension__
typedef long long int intmax_t;
__extension__
typedef unsigned long long int uintmax_t;
typedef unsigned int *bitset;
unsigned int sizeof_bitset(unsigned int nbits);
bitset bitset_new(region r, unsigned int nbits);
bitset bitset_copy(region r, bitset b);
void bitset_assign(bitset b1, bitset b2);
bool bitset_empty(bitset b);
bool bitset_empty_range(bitset b, unsigned int first, unsigned int last);
bool bitset_full_range(bitset b, unsigned int first, unsigned int last);
void bitset_insert_all(bitset b);
bool bitset_insert(bitset b, unsigned int elt);
bool bitset_remove(bitset b, unsigned int elt);
bool bitset_member(bitset b, unsigned int elt);
bool bitset_intersect(bitset b1, const bitset b2);
void bitset_print(bitset b);
unsigned long bitset_hash(bitset b);
bool bitset_eq(bitset b1, bitset b2);
typedef struct growbuf *growbuf;
growbuf growbuf_new(region, int);
void growbuf_reset(growbuf);
int gprintf(growbuf, const char *, ...);
int gvprintf(growbuf, const char *, va_list);
char *growbuf_contents(growbuf);
bool growbuf_empty(growbuf);
typedef struct Qtype *qtype;
typedef struct Abstract_loc *aloc;
typedef struct Effect *effect;
typedef struct Store *store;
typedef struct Rinfo *rinfo;
typedef struct Drinfo *drinfo;
typedef struct ExprDrinfoPair *exprdrinfo;
typedef int (*printf_func)(const char *fmt, ...);
typedef enum {p_neg = -1, p_non = 0, p_pos = 1, p_sub = 2} polarity;
typedef enum {
  eff_any = 0,
  eff_rwr,
  eff_r,
  eff_wr,
  eff_alloc,
  eff_last = eff_alloc } eff_kind;
void load_config_file_quals(const char *);
typedef struct type_qualifier *qual;
int cmp_qual(qual left, qual right);
typedef dd_list qual_set; typedef dd_list_pos qual_set_scanner; static inline qual_set empty_qual_set (region r) { return dd_new_list(r); } static inline qual_set qual_set_copy(region r, qual_set s) { if (s == ((void *)0)) return ((void *)0); else return dd_copy(r, s); } static inline bool qual_set_empty(qual_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool qual_set_member(qual_set s, qual elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp_qual, (void *) elt) != ((void *)0); } static inline int qual_set_size(qual_set s) { if (s == ((void *)0)) return 0; else return dd_length(s); } static inline bool qual_set_insert(region r, qual_set *s, qual elt) { *s = dd_fix_null(r, *s); if (! qual_set_member(*s, elt)) { dd_add_first(r, *s, (void *) elt); return 1; } return 0; } static inline bool qual_set_insert_last(region r, qual_set *s, qual elt) { *s = dd_fix_null(r, *s); if (! qual_set_member(*s, elt)) { dd_add_last(r, *s, (void *) elt); return 1; } return 0; } static inline bool qual_set_insert_nocheck(region r, qual_set *s, qual elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool qual_set_insert_last_nocheck(region r, qual_set *s, qual elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline void qual_set_remove(qual_set *s, qual elt) { if (*s) dd_remove_all_matches_from(dd_first(*s), (dd_cmp_fn) cmp_qual, (void *) elt); } static inline qual_set qual_set_union(qual_set s1, qual_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); dd_remove_dups(s1, (dd_cmp_fn)cmp_qual); return s1; } static inline qual_set qual_set_union_nocheck(qual_set s1, qual_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool qual_set_single(qual_set s) { return qual_set_size(s) == 1; } static inline void qual_set_sort(qual_set s) { if (s) dd_sort(s, (set_cmp_fn) cmp_qual); } static inline void qual_set_remove_dups(int (*cmp)(qual, qual), qual_set s) { if (s) dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void qual_set_scan(qual_set s, qual_set_scanner *ss) { if (s) *ss = dd_first(s); else *ss = ((void *)0); } static inline qual qual_set_next(qual_set_scanner *ss) { qual result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((qual)((*ss)->data)); *ss = ((*ss)->next); return result; };
typedef struct Qual_edge* qual_edge;
typedef dd_list qual_edge_set; typedef dd_list_pos qual_edge_set_scanner; static inline qual_edge_set empty_qual_edge_set(region r) { return dd_new_list(r); } static inline qual_edge_set qual_edge_set_copy(region r, qual_edge_set s) { if (s == ((void *)0)) return ((void *)0); return dd_copy(r, s); } static inline bool qual_edge_set_empty(qual_edge_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool qual_edge_set_member(int (*cmp)(qual_edge, qual_edge), qual_edge_set s, qual_edge elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp, (void *) elt) != ((void *)0); } static inline int qual_edge_set_size(qual_edge_set s) { if (s == ((void *)0)) return 0; return dd_length(s); } static inline bool qual_edge_set_insert(region r, qual_edge_set *s, qual_edge elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool qual_edge_set_insert_last(region r, qual_edge_set *s, qual_edge elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline qual_edge_set qual_edge_set_union(qual_edge_set s1, qual_edge_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool qual_edge_set_single(qual_edge_set s) { return qual_edge_set_size(s) == 1; } static inline void qual_edge_set_sort(int (*cmp)(qual_edge, qual_edge), qual_edge_set s) { if (s == ((void *)0)) return; dd_sort(s, (set_cmp_fn) cmp); } static inline void qual_edge_set_remove_dups(int (*cmp)(qual_edge, qual_edge), qual_edge_set s) { if (s == ((void *)0)) return; dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void qual_edge_set_scan(qual_edge_set s, qual_edge_set_scanner *ss) { if (s == ((void *)0)) *ss = ((void *)0); else *ss = dd_first(s); } static inline qual_edge qual_edge_set_next(qual_edge_set_scanner *ss) { qual_edge result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((qual_edge)((*ss)->data)); *ss = ((*ss)->next); return result; };
extern int flag_print_quals_graph;
typedef enum { level_value, level_ref } level_qual_t;
typedef enum { sign_pos, sign_neg, sign_eq } sign_qual_t;
typedef enum { flow_up, flow_down } flow_qual_t;
typedef unsigned int gate_word;
typedef struct Qual_gate {
  bool forward:1;
  bool backward:1;
  gate_word mask[(((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8))];
} qual_gate;
extern qual const_qual;
extern qual nonconst_qual;
extern qual volatile_qual;
extern qual restrict_qual;
extern qual noreturn_qual;
extern qual init_qual;
extern qual noninit_qual;
extern bool exists_effect_qual;
extern bool exists_ptrflow_qual;
extern bool exists_fieldflow_qual;
extern bool exists_fieldptrflow_qual;
extern bool used_fs_qual;
qual_gate open_qgate;
qual_gate fi_qgate;
qual_gate fs_qgate;
qual_gate effect_qgate;
qual_gate ptrflow_down_pos_qgate;
qual_gate ptrflow_down_neg_qgate;
qual_gate ptrflow_up_pos_qgate;
qual_gate ptrflow_up_neg_qgate;
qual_gate fieldflow_down_pos_qgate;
qual_gate fieldflow_down_neg_qgate;
qual_gate fieldflow_up_pos_qgate;
qual_gate fieldflow_up_neg_qgate;
qual_gate fieldptrflow_down_pos_qgate;
qual_gate fieldptrflow_down_neg_qgate;
qual_gate fieldptrflow_up_pos_qgate;
qual_gate fieldptrflow_up_neg_qgate;
typedef void (*qual_traverse_fn)(qual q, void *arg);
typedef void (*edge_traverse_fn)(qual left, qual right, location loc,
                                 polarity p,bool first,bool bidi,void *arg);
const char *polarity_to_string(polarity p);
void init_quals(void);
void end_define_pos(void);
void finish_quals(void);
void dispose_quals(void);
void begin_po_qual(void);
void end_po_qual(void);
void set_po_flow_sensitive(void);
void set_po_nonprop(void);
void set_po_effect(void);
qual add_qual(const char *name);
void add_qual_lt(qual left, qual right);
void add_color_qual(qual q, const char *color);
void add_level_qual(qual q, level_qual_t lev);
void add_sign_qual(qual q, sign_qual_t sign);
void add_ptrflow_qual(qual q, flow_qual_t f);
void add_fieldflow_qual(qual q, flow_qual_t f);
void add_fieldptrflow_qual(qual q, flow_qual_t f);
qual_set get_constant_quals (void);
qual find_qual(const char *name);
level_qual_t level_qual(qual q);
sign_qual_t sign_qual(qual q);
bool flow_sensitive_qual(qual q);
bool nonprop_qual(qual q);
bool variable_qual(qual q);
bool constant_qual(qual q);
qual make_qvar(const char *name, location loc, bool preferred, bool global);
qual make_fresh_qvar(const char *base_name, location loc);
qual_set get_variable_quals (void);
bool store_aloc_qual(qual q, store s, aloc al);
void mk_no_qual_qual(location loc, qual q);
void set_global_qual (qual q, bool global);
bool global_qual(qual q);
bool preferred_qual(qual q);
location location_qual(qual q);
qual_edge_set lb_qual(region r, qual q);
qual_edge_set cs_lb_qual(region r, qual q);
qual_edge_set ub_qual(region r, qual q);
qual_edge_set cs_ub_qual(region r, qual q);
int error_count_qual(qual q);
void scan_qual_bounds(qual q, qual_traverse_fn f, void *arg);
void scan_qual_bounds_sorted(qual q, qual_traverse_fn f, void *arg);
qual qual_edge_other_end (qual_edge qe, qual q);
location qual_edge_loc (qual_edge qe);
bool mkleq_qual(location loc, qual_gate *qgate, qual left, qual right);
bool mkeq_qual(location loc, qual_gate *qgate, qual left, qual right);
bool mkinst_qual(location loc, qual_gate *qgate, qual left, qual right, polarity p);
bool unify_qual(location loc, qual left, qual right);
bool cond_mkleq_qual(location loc, qual_gate *qgate1, qual l1, qual r1,
                     qual_gate *qgate2, qual l2, qual r2);
bool copy_constraint (qual_edge e, qual q1, qual q2);
bool eq_qual(qual left, qual right);
bool leq_qual(qual left, qual right);
bool cs_leq_qual(qual left, qual right);
bool has_lb_qual(qual q, qual bound);
bool cs_has_lb_qual(qual q, qual bound);
bool has_ub_qual(qual q, qual bound);
bool cs_has_ub_qual(qual q, qual bound);
bool has_qual(qual q, qual bound);
bool cs_has_qual(qual q, qual bound);
bool has_fs_qual(qual q);
bool may_leq_qual (qual q1, qual q2);
bool cs_may_leq_qual (qual q1, qual q2);
typedef int (*pr_qual_fn)(printf_func, qual);
int print_qual(printf_func, qual);
int cs_print_qual(printf_func, qual);
int print_qual_raw(printf_func, qual);
const char *name_qual(qual q);
bool set_qual_name(qual q, const char *name);
const char *unique_name_qual(qual q);
const char *color_qual(qual q);
void print_qual_graph(qual q, const char *file, bool po_bounds_only);
void traverse_shortest_path_edges(qual q, qual c, edge_traverse_fn f,
                                  void *arg);
void traverse_error_path_edges(qual q,edge_traverse_fn f,void* arg);
void cs_traverse_error_path_edges(qual q,edge_traverse_fn f,void* arg);
extern int num_hotspots;
extern qual *hotspots;
void reset_flow_sensitive_quals(void);
typedef enum {sev_err, sev_warn, sev_info} severity;
void init_qerror(void);
extern bool qerrors;
extern growbuf qual_error_detail;
extern growbuf qtype_error_detail;
int qual_ed_printf(const char *fmt, ...);
int qtype_ed_printf(const char *fmt, ...);
  int yylex(void);
  void yyerror(char *, ...);
  extern char *yytext;
  int line = 0;
  char *filename = ((void *)0);
  qual lookup_add_var(char *name);
typedef union {
  char *str;
  qual qual;
} yystype;
static const char yytranslate[] =
{
       0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 1, 3, 4, 5,
       6, 7, 8, 9
};
static const short yyr1[] =
{
       0, 10, 10, 11, 11, 11, 11, 12, 12
};
static const short yyr2[] =
{
       0, 0, 2, 3, 3, 3, 7, 1, 1
};
static const short yydefact[] =
{
       1, 7, 8, 1, 0, 2, 0, 0, 0, 4,
       3, 5, 0, 0, 0, 6, 0, 0, 0
};
static const short yydefgoto[] =
{
       5, 3, 4
};
static const short yypact[] =
{
       0,-32768,-32768, 0, 2,-32768, 0, 0, 0,-32768,
      -1,-32768, 0, 4, 0,-32768, 12, 13,-32768
};
static const short yypgoto[] =
{
      14,-32768, -6
};
static const short yytable[] =
{
       9, 10, 11, 1, 2, 12, 13, 6, 15, 7,
       8, 14, 17, 18, 16
};
static const short yycheck[] =
{
       6, 7, 8, 3, 4, 6, 12, 5, 14, 7,
       8, 7, 0, 0, 0
};
union yyalloc
{
  short yyss;
  yystype yyvs;
};
int yyparse (void);
int yychar; yystype yylval; int yynerrs;
int
yyparse ()
{
  register int yystate;
  register int yyn;
  int yyresult;
  int yyerrstatus;
  int yychar1 = 0;
  short yyssa[200];
  short *yyss = yyssa;
  register short *yyssp;
  yystype yyvsa[200];
  yystype *yyvs = yyvsa;
  register yystype *yyvsp;
  unsigned int yystacksize = 200;
  yystype yyval;
  int yylen;
                                          ;
  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = -2;
  yyssp = yyss;
  yyvsp = yyvs;
  goto yysetstate;
 yynewstate:
  yyssp++;
 yysetstate:
  *yyssp = yystate;
  if (yyssp >= yyss + yystacksize - 1)
    {
      unsigned int yysize = yyssp - yyss + 1;
      if (yystacksize >= 10000)
        goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > 10000)
        yystacksize = 10000;
      {
        short *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) __builtin_alloca (((yystacksize) * (sizeof (short) + sizeof (yystype)) + (sizeof (union yyalloc) - 1)));
        if (! yyptr)
          goto yyoverflowlab;
        do { unsigned int yynewbytes; __builtin_memcpy (&yyptr->yyss, yyss, (yysize) * sizeof (*(yyss))); yyss = &yyptr->yyss; yynewbytes = yystacksize * sizeof (*yyss) + (sizeof (union yyalloc) - 1); yyptr += yynewbytes / sizeof (*yyptr); } while (0);
        do { unsigned int yynewbytes; __builtin_memcpy (&yyptr->yyvs, yyvs, (yysize) * sizeof (*(yyvs))); yyvs = &yyptr->yyvs; yynewbytes = yystacksize * sizeof (*yyvs) + (sizeof (union yyalloc) - 1); yyptr += yynewbytes / sizeof (*yyptr); } while (0);
        if (yyss1 != yyssa)
          do { ; } while (0);
      }
      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
                                                   ;
      if (yyssp >= yyss + yystacksize - 1)
        goto yyabortlab;
    }
                                                      ;
  goto yybackup;
yybackup:
  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yydefault;
  if (yychar == -2)
    {
                                               ;
      yychar = yylex ();
    }
  if (yychar <= 0)
    {
      yychar1 = 0;
      yychar = 0;
                                                    ;
    }
  else
    {
      yychar1 = ((unsigned)(yychar) <= 263 ? yytranslate[yychar] : 13);
    }
  yyn += yychar1;
  if (yyn < 0 || yyn > 14 || yycheck[yyn] != yychar1)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == -32768)
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;
  if (yyn == 18)
    goto yyacceptlab;
                                        ;
  if (yychar != 0)
    yychar = -2;
  *++yyvsp = yylval;
  if (yyerrstatus)
    yyerrstatus--;
  yystate = yyn;
  goto yynewstate;
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen];
  switch (yyn) {
case 3:
{
              if (mkleq_qual(((void *)0), &fi_qgate, yyvsp[-2].qual, yyvsp[0].qual))
                {
                  yyerror("Unsatisfiable constraint");
                  goto yyabortlab;
                }
            ;
    break;}
case 4:{
              if (mkeq_qual(((void *)0), &fi_qgate, yyvsp[-2].qual, yyvsp[0].qual))
                {
                  yyerror("Unsatisfiable constraint");
                  goto yyabortlab;
                }
            ;
    break;}
case 5:{
              if (unify_qual(((void *)0), yyvsp[-2].qual, yyvsp[0].qual))
                {
                  yyerror("Unsatisfiable constraint");
                  goto yyabortlab;
                }
            ;
    break;}
case 6:{
              if (cond_mkleq_qual(((void *)0), &fi_qgate, yyvsp[-6].qual, yyvsp[-4].qual, &fi_qgate, yyvsp[-2].qual, yyvsp[0].qual))
                {
                  yyerror("Unsatisfiable constraint");
                  goto yyabortlab;
                }
            ;
    break;}
case 7:
{ yyval.qual = find_qual(yyvsp[0].str);
                      if (!yyval.qual)
                        {
                          yyerror("Unknown qualifier %s", yyvsp[0].str);
                          goto yyabortlab;
                        }
                    ;
    break;}
case 8:
{ yyval.qual = lookup_add_var(yyvsp[0].str); ;
    break;}
}
  yyvsp -= yylen;
  yyssp -= yylen;
  *++yyvsp = yyval;
  yyn = yyr1[yyn];
  yystate = yypgoto[yyn - 10] + *yyssp;
  if (yystate >= 0 && yystate <= 14 && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - 10];
  goto yynewstate;
yyerrlab:
  if (!yyerrstatus)
    {
      ++yynerrs;
        yyerror ("parse error");
    }
  goto yyerrlab1;
yyerrlab1:
  if (yyerrstatus == 3)
    {
      if (yychar == 0)
        goto yyabortlab;
                                            ;
      yychar = -2;
    }
  yyerrstatus = 3;
  goto yyerrhandle;
yyerrdefault:
yyerrpop:
  if (yyssp == yyss)
    goto yyabortlab;
  yyvsp--;
  yystate = *--yyssp;
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yyerrdefault;
  yyn += 1;
  if (yyn < 0 || yyn > 14 || yycheck[yyn] != 1)
    goto yyerrdefault;
  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == -32768)
        goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;
  if (yyn == 18)
    goto yyacceptlab;
                                                ;
  *++yyvsp = yylval;
  yystate = yyn;
  goto yynewstate;
yyacceptlab:
  yyresult = 0;
  goto yyreturn;
yyabortlab:
  yyresult = 1;
  goto yyreturn;
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
yyreturn:
  if (yyss != yyssa)
    do { ; } while (0);
  return yyresult;
}
void yyerror(char *format, ...)
{
  va_list args;
  fprintf(stderr, "%s:%d: ", filename, line);
  __builtin_stdarg_start((args),format);
  vfprintf(stderr, format, args);
  __builtin_va_end(args);
  fprintf(stderr, "\n");
}
void fatal(const char *format, ...)
{
  va_list args;
  __builtin_stdarg_start((args),format);
  vfprintf(stderr, format, args);
  __builtin_va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}
struct Store {};
struct Aloc {};
void propagate_store_cell_forward(store s, aloc al) {}
void propagate_store_cell_backward(store s, aloc al) {}
void report_qerror(location loc, severity sev, const char *format, ...) {}
void pam_add_overlay_anchor(location loc) {}
region iquals_region;
location dummy_location = ((void *)0);
typedef struct var_list {
  char *name;
  qual qual;
  struct var_list *next;
} *var_list;
var_list vars = ((void *)0);
qual lookup_add_var(char *name)
{
  var_list cur = vars;
  var_list new;
  while (cur)
    {
      if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (cur->name) && __builtin_constant_p (name) && (__s1_len = strlen (cur->name), __s2_len = strlen (name), (!((size_t)(const void *)((cur->name) + 1) - (size_t)(const void *)(cur->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (cur->name), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (cur->name) && ((size_t)(const void *)((cur->name) + 1) - (size_t)(const void *)(cur->name) == 1) && (__s1_len = strlen (cur->name), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (cur->name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cur->name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) (cur->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cur->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p (cur->name) && ((size_t)(const void *)((cur->name) + 1) - (size_t)(const void *)(cur->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (cur->name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cur->name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cur->name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (cur->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp (cur->name, name)))); }))
        return cur->qual;
      cur = cur->next;
    }
  new = (sizeof(struct var_list) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((iquals_region), sizeof(struct var_list)) : (((void)0), __rc_typed_ralloc)((iquals_region), sizeof(struct var_list), 0));
  new->name = name;
  new->qual = make_qvar(name, dummy_location, 0, 0);
  new->next = vars;
  vars = new;
  return new->qual;
}
void print_results(void)
{
  var_list cur;
  for (cur = vars; cur; cur = cur->next)
    {
      printf("%s: ", cur->name);
      print_qual(printf, cur->qual);
      printf("\n");
    }
}
int main(int argc, char **argv)
{
  extern void yyrestart(FILE *);
  char *config_file = ((void *)0);
  FILE *file;
  int i, err;
  region_init();
  iquals_region = newregion();
  for (i = 1; i < argc; i++)
    {
      if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (argv[i]) && __builtin_constant_p ("-config") && (__s1_len = strlen (argv[i]), __s2_len = strlen ("-config"), (!((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-config") + 1) - (size_t)(const void *)("-config") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (argv[i]), (__const char *) ("-config"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) && (__s1_len = strlen (argv[i]), __s1_len < 4) ? (__builtin_constant_p ("-config") && ((size_t)(const void *)(("-config") + 1) - (size_t)(const void *)("-config") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)("-config"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) ("-config"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) ("-config"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) ("-config"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-config"); register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-config") && ((size_t)(const void *)(("-config") + 1) - (size_t)(const void *)("-config") == 1) && (__s2_len = strlen ("-config"), __s2_len < 4) ? (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)("-config"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) ("-config"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) ("-config"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) ("-config"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (argv[i]); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-config"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-config"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-config"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-config"))[3]); } } __result; }))) : strcmp (argv[i], "-config")))); }))
        {
          if (i < argc - 1)
            {
              i++;
              config_file = argv[i];
            }
          else
            fatal("Missing -config file");
        }
      else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (argv[i]) && __builtin_constant_p ("-g") && (__s1_len = strlen (argv[i]), __s2_len = strlen ("-g"), (!((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-g") + 1) - (size_t)(const void *)("-g") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (argv[i]), (__const char *) ("-g"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) && (__s1_len = strlen (argv[i]), __s1_len < 4) ? (__builtin_constant_p ("-g") && ((size_t)(const void *)(("-g") + 1) - (size_t)(const void *)("-g") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)("-g"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) ("-g"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) ("-g"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) ("-g"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-g"); register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-g") && ((size_t)(const void *)(("-g") + 1) - (size_t)(const void *)("-g") == 1) && (__s2_len = strlen ("-g"), __s2_len < 4) ? (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)("-g"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) ("-g"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) ("-g"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) ("-g"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (argv[i]); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-g"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-g"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-g"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-g"))[3]); } } __result; }))) : strcmp (argv[i], "-g")))); }))
        flag_print_quals_graph = 1;
      else
        {
          if (!filename)
            filename = argv[i];
          else
            fatal("Multiple input filenames");
        }
    }
  if (!filename)
    {
      printf("Usage: iquals [-config <file>|-g] <filename>\n");
      printf("File format:\n\n");
      printf("\tq1 <= q2                inequality\n");
      printf("\tq1  = q2                equality\n");
      printf("\tq1 == q2                unification\n");
      printf("\tq1 <= q2 ==> q3 <= q4   conditional inequality\n");
      exit(0);
    }
  init_quals();
  if (config_file)
    load_config_file_quals(config_file);
  end_define_pos();
  file = fopen(filename, "r");
  if (!file)
    {
      fprintf(stderr, "Unable to open ``%s'': ", filename);
      perror("");
      exit(1);
    }
  yyrestart(file);
  line = 1;
  err = yyparse();
  fclose(file);
  finish_quals();
  print_results();
  if (err)
    exit(1);
  else
    exit(0);
}
const char CANON_IDENT_dfc3f32be231e94a9088733b45d23610[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/iquals.tab.c";
