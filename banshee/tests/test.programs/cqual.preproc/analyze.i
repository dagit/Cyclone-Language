typedef unsigned int size_t;
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
typedef struct _IO_FILE FILE;
typedef struct _IO_FILE __FILE;
typedef long int wchar_t;
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
typedef __builtin_va_list __gnuc_va_list;
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
extern void *malloc (size_t __size) __attribute__ ((__malloc__));
extern void *calloc (size_t __nmemb, size_t __size)
             __attribute__ ((__malloc__));
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
extern char *__strdup (__const char *__string) __attribute__ ((__malloc__));
extern char *__strndup (__const char *__string, size_t __n)
             __attribute__ ((__malloc__));
extern void __assert_fail (__const char *__assertion, __const char *__file,
                           unsigned int __line, __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert_perror_fail (int __errnum, __const char *__file,
                                  unsigned int __line,
                                  __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert (const char *__assertion, const char *__file, int __line)
             __attribute__ ((__noreturn__));
typedef __gnuc_va_list va_list;
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
struct array;
struct array *new_array(region r, size_t initialsize,
                        size_t typesize, type_t typeinfo);
void *array_extend(struct array *a, int by);
void array_reset(struct array *a);
size_t array_length(struct array *a);
void *array_data(struct array *a);
typedef struct wchar_array_a *wchar_array; wchar_array new_wchar_array(region r, size_t initialsize); wchar_t *wchar_array_extend(wchar_array a, int by); void wchar_array_reset(wchar_array a); size_t wchar_array_length(wchar_array a); wchar_t *wchar_array_data(wchar_array a);
extern void __assert_fail (__const char *__assertion, __const char *__file,
                           unsigned int __line, __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert_perror_fail (int __errnum, __const char *__file,
                                  unsigned int __line,
                                  __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert (const char *__assertion, const char *__file, int __line)
             __attribute__ ((__noreturn__));
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
void *xmalloc(size_t size);
void *xrealloc(void *p, size_t newsize);
unsigned long align_to(unsigned long n, unsigned long alignment);
unsigned long lcm(unsigned long x, unsigned long y);
void __fail(const char *file, unsigned int line,
            const char *func, const char *fmt, ...) __attribute__ ((__noreturn__));
void __user_error(const char *file, unsigned int line,
                  const char *func, const char *fmt, ...)
  __attribute__ ((__noreturn__));
char *rstrcat(region, const char *, const char *);
char *rstrscat(region, ...);
const char *inttostr(region r, int);
char *rsprintf(region r, const char *fmt, ...);
char *rvsprintf(region r, const char *fmt, va_list args);
char *ptr_to_ascii(void *ptr);
unsigned long ptr_hash(void *ptr);
bool ptr_eq(void *ptr1, void *ptr2);
unsigned long string_hash(const char *str);
bool string_eq(const char *s1, const char *s2);
int ptr_cmp(void *ptr1, void *ptr2);
const char *name_with_loc(region r, const char *name, location loc);
typedef int id_declaration_list;
typedef struct typelist *typelist;
typedef struct type *type;
typedef struct known_cst *known_cst;
typedef struct AST_node *node;
typedef struct AST_declaration *declaration;
typedef struct AST_statement *statement;
typedef struct AST_expression *expression;
typedef struct AST_type_element *type_element;
typedef struct AST_declarator *declarator;
typedef struct AST_label *label;
typedef struct AST_asm_decl *asm_decl;
typedef struct AST_data_decl *data_decl;
typedef struct AST_extension_decl *extension_decl;
typedef struct AST_ellipsis_decl *ellipsis_decl;
typedef struct AST_enumerator *enumerator;
typedef struct AST_oldidentifier_decl *oldidentifier_decl;
typedef struct AST_function_decl *function_decl;
typedef struct AST_implicit_decl *implicit_decl;
typedef struct AST_variable_decl *variable_decl;
typedef struct AST_field_decl *field_decl;
typedef struct AST_asttype *asttype;
typedef struct AST_typename *typename;
typedef struct AST_type_variable *type_variable;
typedef struct AST_typeof_expr *typeof_expr;
typedef struct AST_typeof_type *typeof_type;
typedef struct AST_attribute *attribute;
typedef struct AST_rid *rid;
typedef struct AST_user_qual *user_qual;
typedef struct AST_qualifier *qualifier;
typedef struct AST_tag_ref *tag_ref;
typedef struct AST_function_declarator *function_declarator;
typedef struct AST_pointer_declarator *pointer_declarator;
typedef struct AST_array_declarator *array_declarator;
typedef struct AST_identifier_declarator *identifier_declarator;
typedef struct AST_asm_stmt *asm_stmt;
typedef struct AST_compound_stmt *compound_stmt;
typedef struct AST_if_stmt *if_stmt;
typedef struct AST_labeled_stmt *labeled_stmt;
typedef struct AST_expression_stmt *expression_stmt;
typedef struct AST_breakable_stmt *breakable_stmt;
typedef struct AST_conditional_stmt *conditional_stmt;
typedef struct AST_switch_stmt *switch_stmt;
typedef struct AST_for_stmt *for_stmt;
typedef struct AST_break_stmt *break_stmt;
typedef struct AST_continue_stmt *continue_stmt;
typedef struct AST_return_stmt *return_stmt;
typedef struct AST_goto_stmt *goto_stmt;
typedef struct AST_computed_goto_stmt *computed_goto_stmt;
typedef struct AST_empty_stmt *empty_stmt;
typedef struct AST_assert_type_stmt *assert_type_stmt;
typedef struct AST_change_type_stmt *change_type_stmt;
typedef struct AST_deep_restrict_stmt *deep_restrict_stmt;
typedef struct AST_unary *unary;
typedef struct AST_binary *binary;
typedef struct AST_comma *comma;
typedef struct AST_sizeof_type *sizeof_type;
typedef struct AST_alignof_type *alignof_type;
typedef struct AST_label_address *label_address;
typedef struct AST_cast *cast;
typedef struct AST_cast_list *cast_list;
typedef struct AST_conditional *conditional;
typedef struct AST_identifier *identifier;
typedef struct AST_compound_expr *compound_expr;
typedef struct AST_function_call *function_call;
typedef struct AST_array_ref *array_ref;
typedef struct AST_field_ref *field_ref;
typedef struct AST_init_list *init_list;
typedef struct AST_init_index *init_index;
typedef struct AST_init_field *init_field;
typedef struct AST_lexical_cst *lexical_cst;
typedef struct AST_string_cst *string_cst;
typedef struct AST_string *string;
typedef struct AST_id_label *id_label;
typedef struct AST_case_label *case_label;
typedef struct AST_default_label *default_label;
typedef struct AST_word *word;
typedef struct AST_asm_operand *asm_operand;
typedef struct AST_declaration *error_decl;
typedef struct AST_tag_ref *struct_ref;
typedef struct AST_tag_ref *union_ref;
typedef struct AST_tag_ref *enum_ref;
typedef struct AST_statement *error_stmt;
typedef struct AST_conditional_stmt *while_stmt;
typedef struct AST_conditional_stmt *dowhile_stmt;
typedef struct AST_expression *error_expr;
typedef struct AST_unary *dereference;
typedef struct AST_unary *extension_expr;
typedef struct AST_unary *sizeof_expr;
typedef struct AST_unary *alignof_expr;
typedef struct AST_unary *realpart;
typedef struct AST_unary *imagpart;
typedef struct AST_unary *address_of;
typedef struct AST_unary *unary_minus;
typedef struct AST_unary *unary_plus;
typedef struct AST_unary *conjugate;
typedef struct AST_unary *preincrement;
typedef struct AST_unary *predecrement;
typedef struct AST_unary *postincrement;
typedef struct AST_unary *postdecrement;
typedef struct AST_unary *bitnot;
typedef struct AST_unary *not;
typedef struct AST_binary *plus;
typedef struct AST_binary *minus;
typedef struct AST_binary *times;
typedef struct AST_binary *divide;
typedef struct AST_binary *modulo;
typedef struct AST_binary *lshift;
typedef struct AST_binary *rshift;
typedef struct AST_binary *leq;
typedef struct AST_binary *geq;
typedef struct AST_binary *lt;
typedef struct AST_binary *gt;
typedef struct AST_binary *eq;
typedef struct AST_binary *ne;
typedef struct AST_binary *bitand;
typedef struct AST_binary *bitor;
typedef struct AST_binary *bitxor;
typedef struct AST_binary *andand;
typedef struct AST_binary *oror;
typedef struct AST_binary *assign;
typedef struct AST_binary *plus_assign;
typedef struct AST_binary *minus_assign;
typedef struct AST_binary *times_assign;
typedef struct AST_binary *divide_assign;
typedef struct AST_binary *modulo_assign;
typedef struct AST_binary *lshift_assign;
typedef struct AST_binary *rshift_assign;
typedef struct AST_binary *bitand_assign;
typedef struct AST_binary *bitor_assign;
typedef struct AST_binary *bitxor_assign;
typedef enum {
  kind_node = 42,
  postkind_node = 168,
  kind_declaration = 43,
  postkind_declaration = 54,
  kind_statement = 55,
  postkind_statement = 76,
  kind_expression = 77,
  postkind_expression = 143,
  kind_type_element = 144,
  postkind_type_element = 156,
  kind_declarator = 157,
  postkind_declarator = 161,
  kind_label = 162,
  postkind_label = 165,
  kind_asm_decl = 44,
  postkind_asm_decl = 44,
  kind_data_decl = 45,
  postkind_data_decl = 45,
  kind_extension_decl = 46,
  postkind_extension_decl = 46,
  kind_ellipsis_decl = 47,
  postkind_ellipsis_decl = 47,
  kind_enumerator = 48,
  postkind_enumerator = 48,
  kind_oldidentifier_decl = 49,
  postkind_oldidentifier_decl = 49,
  kind_function_decl = 50,
  postkind_function_decl = 50,
  kind_implicit_decl = 51,
  postkind_implicit_decl = 51,
  kind_variable_decl = 52,
  postkind_variable_decl = 52,
  kind_field_decl = 53,
  postkind_field_decl = 53,
  kind_asttype = 166,
  postkind_asttype = 166,
  kind_typename = 145,
  postkind_typename = 145,
  kind_type_variable = 146,
  postkind_type_variable = 146,
  kind_typeof_expr = 147,
  postkind_typeof_expr = 147,
  kind_typeof_type = 148,
  postkind_typeof_type = 148,
  kind_attribute = 149,
  postkind_attribute = 149,
  kind_rid = 150,
  postkind_rid = 150,
  kind_user_qual = 151,
  postkind_user_qual = 151,
  kind_qualifier = 152,
  postkind_qualifier = 152,
  kind_tag_ref = 153,
  postkind_tag_ref = 156,
  kind_function_declarator = 158,
  postkind_function_declarator = 158,
  kind_pointer_declarator = 159,
  postkind_pointer_declarator = 159,
  kind_array_declarator = 160,
  postkind_array_declarator = 160,
  kind_identifier_declarator = 161,
  postkind_identifier_declarator = 161,
  kind_asm_stmt = 56,
  postkind_asm_stmt = 56,
  kind_compound_stmt = 57,
  postkind_compound_stmt = 57,
  kind_if_stmt = 58,
  postkind_if_stmt = 58,
  kind_labeled_stmt = 59,
  postkind_labeled_stmt = 59,
  kind_expression_stmt = 60,
  postkind_expression_stmt = 60,
  kind_breakable_stmt = 61,
  postkind_breakable_stmt = 66,
  kind_conditional_stmt = 62,
  postkind_conditional_stmt = 65,
  kind_switch_stmt = 63,
  postkind_switch_stmt = 63,
  kind_for_stmt = 66,
  postkind_for_stmt = 66,
  kind_break_stmt = 67,
  postkind_break_stmt = 67,
  kind_continue_stmt = 68,
  postkind_continue_stmt = 68,
  kind_return_stmt = 69,
  postkind_return_stmt = 69,
  kind_goto_stmt = 70,
  postkind_goto_stmt = 70,
  kind_computed_goto_stmt = 71,
  postkind_computed_goto_stmt = 71,
  kind_empty_stmt = 72,
  postkind_empty_stmt = 72,
  kind_assert_type_stmt = 73,
  postkind_assert_type_stmt = 73,
  kind_change_type_stmt = 74,
  postkind_change_type_stmt = 74,
  kind_deep_restrict_stmt = 75,
  postkind_deep_restrict_stmt = 75,
  kind_unary = 78,
  postkind_unary = 96,
  kind_binary = 97,
  postkind_binary = 127,
  kind_comma = 128,
  postkind_comma = 128,
  kind_sizeof_type = 129,
  postkind_sizeof_type = 129,
  kind_alignof_type = 130,
  postkind_alignof_type = 130,
  kind_label_address = 131,
  postkind_label_address = 131,
  kind_cast = 79,
  postkind_cast = 79,
  kind_cast_list = 132,
  postkind_cast_list = 132,
  kind_conditional = 133,
  postkind_conditional = 133,
  kind_identifier = 134,
  postkind_identifier = 134,
  kind_compound_expr = 135,
  postkind_compound_expr = 135,
  kind_function_call = 136,
  postkind_function_call = 136,
  kind_array_ref = 98,
  postkind_array_ref = 98,
  kind_field_ref = 80,
  postkind_field_ref = 80,
  kind_init_list = 137,
  postkind_init_list = 137,
  kind_init_index = 138,
  postkind_init_index = 138,
  kind_init_field = 139,
  postkind_init_field = 139,
  kind_lexical_cst = 140,
  postkind_lexical_cst = 141,
  kind_string_cst = 141,
  postkind_string_cst = 141,
  kind_string = 142,
  postkind_string = 142,
  kind_id_label = 163,
  postkind_id_label = 163,
  kind_case_label = 164,
  postkind_case_label = 164,
  kind_default_label = 165,
  postkind_default_label = 165,
  kind_word = 167,
  postkind_word = 167,
  kind_asm_operand = 168,
  postkind_asm_operand = 168,
  kind_error_decl = 54,
  postkind_error_decl = 54,
  kind_struct_ref = 154,
  postkind_struct_ref = 154,
  kind_union_ref = 155,
  postkind_union_ref = 155,
  kind_enum_ref = 156,
  postkind_enum_ref = 156,
  kind_error_stmt = 76,
  postkind_error_stmt = 76,
  kind_while_stmt = 64,
  postkind_while_stmt = 64,
  kind_dowhile_stmt = 65,
  postkind_dowhile_stmt = 65,
  kind_error_expr = 143,
  postkind_error_expr = 143,
  kind_dereference = 81,
  postkind_dereference = 81,
  kind_extension_expr = 82,
  postkind_extension_expr = 82,
  kind_sizeof_expr = 83,
  postkind_sizeof_expr = 83,
  kind_alignof_expr = 84,
  postkind_alignof_expr = 84,
  kind_realpart = 85,
  postkind_realpart = 85,
  kind_imagpart = 86,
  postkind_imagpart = 86,
  kind_address_of = 87,
  postkind_address_of = 87,
  kind_unary_minus = 88,
  postkind_unary_minus = 88,
  kind_unary_plus = 89,
  postkind_unary_plus = 89,
  kind_conjugate = 90,
  postkind_conjugate = 90,
  kind_preincrement = 91,
  postkind_preincrement = 91,
  kind_predecrement = 92,
  postkind_predecrement = 92,
  kind_postincrement = 93,
  postkind_postincrement = 93,
  kind_postdecrement = 94,
  postkind_postdecrement = 94,
  kind_bitnot = 95,
  postkind_bitnot = 95,
  kind_not = 96,
  postkind_not = 96,
  kind_plus = 99,
  postkind_plus = 99,
  kind_minus = 100,
  postkind_minus = 100,
  kind_times = 101,
  postkind_times = 101,
  kind_divide = 102,
  postkind_divide = 102,
  kind_modulo = 103,
  postkind_modulo = 103,
  kind_lshift = 104,
  postkind_lshift = 104,
  kind_rshift = 105,
  postkind_rshift = 105,
  kind_leq = 106,
  postkind_leq = 106,
  kind_geq = 107,
  postkind_geq = 107,
  kind_lt = 108,
  postkind_lt = 108,
  kind_gt = 109,
  postkind_gt = 109,
  kind_eq = 110,
  postkind_eq = 110,
  kind_ne = 111,
  postkind_ne = 111,
  kind_bitand = 112,
  postkind_bitand = 112,
  kind_bitor = 113,
  postkind_bitor = 113,
  kind_bitxor = 114,
  postkind_bitxor = 114,
  kind_andand = 115,
  postkind_andand = 115,
  kind_oror = 116,
  postkind_oror = 116,
  kind_assign = 117,
  postkind_assign = 117,
  kind_plus_assign = 118,
  postkind_plus_assign = 118,
  kind_minus_assign = 119,
  postkind_minus_assign = 119,
  kind_times_assign = 120,
  postkind_times_assign = 120,
  kind_divide_assign = 121,
  postkind_divide_assign = 121,
  kind_modulo_assign = 122,
  postkind_modulo_assign = 122,
  kind_lshift_assign = 123,
  postkind_lshift_assign = 123,
  kind_rshift_assign = 124,
  postkind_rshift_assign = 124,
  kind_bitand_assign = 125,
  postkind_bitand_assign = 125,
  kind_bitor_assign = 126,
  postkind_bitor_assign = 126,
  kind_bitxor_assign = 127,
  postkind_bitxor_assign = 127
} ast_kind;
extern location last_location, dummy_location;
enum rid
{
  RID_UNUSED,
  RID_INT,
  RID_CHAR,
  RID_FLOAT,
  RID_DOUBLE,
  RID_VOID,
  RID_UNSIGNED,
  RID_SHORT,
  RID_LONG,
  RID_SIGNED,
  RID_INLINE,
  RID_COMPLEX,
  RID_AUTO,
  RID_STATIC,
  RID_EXTERN,
  RID_REGISTER,
  RID_TYPEDEF,
  RID_MAX
};
extern char * token_buffer;
extern bool in_system_header;
extern bool in_prelude;
int yylex(void);
void init_lex(void);
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
typedef void *hash_key;
typedef void *hash_data;
typedef unsigned long (*hash_fn)(hash_key k);
typedef bool (*keyeq_fn)(hash_key k1, hash_key k2);
typedef void (*hash_apply_fn)(hash_key k, hash_data d, void *arg);
typedef hash_data (*hash_map_fn)(hash_key k, hash_data d, void *arg);
typedef struct Hash_table *hash_table;
hash_table make_hash_table(region rhash, unsigned long size, hash_fn hash,
                           keyeq_fn cmp);
hash_table make_string_hash_table(region rhash, unsigned long size);
void hash_table_reset(hash_table ht);
unsigned long hash_table_size(hash_table ht);
bool hash_table_hash_search(hash_table ht, keyeq_fn cmp,
                            hash_key k, hash_data *d);
bool hash_table_lookup(hash_table ht, hash_key k, hash_data *d);
bool hash_table_insert(hash_table ht, hash_key k, hash_data d);
bool hash_table_remove(hash_table ht, hash_key k);
hash_table hash_table_copy(region rhash, hash_table ht);
hash_table hash_table_map(region r, hash_table ht, hash_map_fn f, void *arg);
typedef struct bucket *bucket;
typedef struct
{
  hash_table ht;
  unsigned long i;
  bucket cur;
} hash_table_scanner;
void hash_table_scan(hash_table ht, hash_table_scanner *);
bool hash_table_next(hash_table_scanner *, hash_key *k, hash_data *d);
typedef int (*keycmp_fn)(hash_key k1, hash_key k2);
struct sorted_entry
{
  hash_key k;
  hash_data d;
};
typedef struct
{
  region r;
  unsigned long i;
  unsigned long size;
  struct sorted_entry *entries;
} hash_table_scanner_sorted;
void hash_table_scan_sorted(hash_table ht, keycmp_fn f,
                            hash_table_scanner_sorted *htss);
bool hash_table_next_sorted(hash_table_scanner_sorted *htss, hash_key *k,
                            hash_data *d);
typedef struct env *env;
env new_env(region r, env parent);
env env_parent(env e);
region env_region(env e);
void *env_lookup(env e, const char *s, bool this_level_only);
void env_add(env e, const char *s, void *value);
typedef hash_table_scanner env_scanner;
void env_scan(env e, env_scanner *scanner);
bool env_next(env_scanner *scanner, const char **name, void **value);
typedef struct Aloctype *aloctype;
typedef struct Alocreftype *alocreftype;
typedef struct Alocfntype *alocfntype;
typedef struct Effecttype *effecttype;
typedef struct Rinf_const *rinf_const;
extern effecttype effecttype_empty;
void init_aloctypes(void);
aloctype make_aloctype(alocreftype tau, alocfntype fn);
aloctype make_aloctype_fresh(void);
void unify_aloctype(aloctype al1, aloctype al2);
void mkleq_aloctype(aloctype al1, aloctype al2);
alocreftype proj_tau(aloctype al);
alocfntype proj_lam(aloctype al);
int print_aloctype(printf_func pf, aloctype al, int depth);
alocreftype alocreftype_fresh(void);
alocreftype alocreftype_var(const char * name);
alocreftype alocreftype_ref(aloctype pointsto);
void unify_alocreftype(alocreftype alref1, alocreftype alref2);
void mkleq_alocreftype(alocreftype alref1, alocreftype alref2);
aloctype deref(alocreftype tau);
int print_alocreftype(printf_func pf, alocreftype alref, int depth);
alocfntype alocfntype_fresh(void);
alocfntype alocfntype_var(const char * name);
alocfntype alocfntype_fn(aloctype * params, effecttype eff, aloctype returns);
void unify_alocfntype(alocfntype alfn1, alocfntype alfn2);
void mkleq_alocfntype(alocfntype alfn1, alocfntype alfn2);
int print_alocfntype(printf_func pf, alocfntype alfn, int depth);
effecttype effecttype_read(alocreftype base);
effecttype effecttype_write(alocreftype base);
effecttype effecttype_any(alocreftype base);
effecttype effecttype_read_reach(alocreftype base);
effecttype effecttype_write_reach(alocreftype base);
effecttype effecttype_any_reach(alocreftype base);
effecttype effecttype_union(effecttype e1, effecttype e2);
effecttype effecttype_inter(effecttype e1, effecttype e2);
effecttype effecttype_var(const char * name);
effecttype effecttype_fresh(void);
void mkleq_effecttype(effecttype e1, effecttype e2);
void mkeq_effecttype(effecttype e1, effecttype e2);
void unify_effecttype(effecttype e1, effecttype e2);
int print_effecttype(printf_func pf, effecttype e);
rinf_const mk_rinf_const(location loc,
                         alocreftype r_al, alocreftype old_al,
                         alocreftype top_al,
                         effecttype r_type, effecttype pointsto_type,
                         effecttype r_body, effecttype body, effecttype env);
void check_rinf_consts(void);
bool is_rinf_satisfied(rinf_const c);
bool is_rinf_used(rinf_const c);
int print_rinf_info(printf_func pf, rinf_const c);
typedef struct field_declaration {
  struct field_declaration *next;
  const char *name;
  type type;
  field_decl ast;
  int bitwidth;
  size_t offset;
  bool offset_cc:1;
  bool packed:1;
} *field_declaration;
typedef struct tag_declaration {
  int kind;
  const char *name;
  type reptype;
  env fields;
  field_declaration fieldlist;
  tag_ref ast;
  struct tag_declaration *shadowed;
  bool defined:1;
  bool being_defined:1;
  bool fields_const:1;
  bool fields_volatile:1;
  bool transparent_union:1;
  bool size_cc:1;
  bool packed:1;
  size_t size, alignment;
  qtype qtype;
} *tag_declaration;
typedef enum { decl_variable, decl_constant, decl_function,
               decl_typedef, decl_error, decl_magic_string } data_kind;
typedef struct data_declaration {
  data_kind kind;
  const char *name;
  type type;
  struct data_declaration *shadowed;
  struct data_declaration *global;
  declaration definition;
  declaration ast;
  expression initialiser;
  bool islimbo:1;
  bool isexternalscope:1;
  bool isfilescoperef:1;
  bool needsmemory:1;
  bool isused:1;
  bool in_system_header:1;
  bool in_prelude:1;
  bool defined:1;
  bool isallocated:1;
  bool addresstaken:1;
  bool __init:1;
  enum { function_implicit, function_normal, function_static, function_nested }
    ftype:2;
  bool isinline:1;
  bool isexterninline:1;
  bool noreturn:1;
  typelist oldstyle_args;
  char * alias;
  enum { variable_register, variable_static, variable_normal } vtype:2;
  bool islocal:1;
  bool isparameter:1;
  known_cst value;
  const wchar_t *chars;
  size_t chars_length;
  qtype qtype;
  qtype fs_qtype;
  alocreftype alref;
  compound_stmt cs;
} *data_declaration;
typedef struct label_declaration {
  const char *name;
  bool explicitly_declared:1;
  bool used:1;
  id_label firstuse;
  id_label definition;
  function_decl containing_function;
} *label_declaration;
typedef struct environment
{
  struct environment * parent;
  function_decl fdecl;
  bool parm_level;
  env id_env;
  env tag_env;
} *environment;
extern data_declaration bad_decl;
typedef struct {
  enum { cval_variable, cval_unk, cval_address,
         cval_float, cval_float_complex,
         cval_uint, cval_uint_complex,
         cval_sint, cval_sint_complex } kind;
  union {
    struct {
      long double d, d_i;
    } fl;
    struct {
      size_t isize;
      union {
        largest_int si;
        largest_uint ui;
      } real;
      union {
        largest_int si_i;
        largest_uint ui_i;
        struct {
          struct data_declaration *ddecl;
          struct label_declaration *ldecl;
        } addr;
      } imag;
    } i;
  } u;
} cval;
extern cval cval_top;
extern cval cval_unknown;
extern cval cval_zero;
void cval_init(void);
cval make_cval_signed(largest_int i, type t);
cval make_cval_unsigned(largest_uint i, type t);
cval make_cval_float(long double d);
cval make_cval_complex(cval r, cval i);
cval make_cval_address(data_declaration ddecl, label_declaration ldecl,
                       largest_int offset);
bool cval_isinteger(cval c);
bool cval_iscomplex(cval c);
bool cval_knownbool(cval c);
bool cval_boolvalue(cval c);
bool cval_knownvalue(cval c);
largest_uint cval_uint_value(cval c);
largest_int cval_sint_value(cval c);
long double cval_float_value(cval c);
bool cval_isone(cval c);
cval cval_cast(cval c, type to);
cval cval_not(cval c);
cval cval_negate(cval c);
cval cval_bitnot(cval c);
cval cval_conjugate(cval c);
cval cval_realpart(cval c);
cval cval_imagpart(cval c);
cval cval_add(cval c1, cval c2);
cval cval_sub(cval c1, cval c2);
cval cval_times(cval c1, cval c2);
cval cval_divide(cval c1, cval c2);
cval cval_modulo(cval c1, cval c2);
cval cval_lshift(cval c1, cval c2);
cval cval_rshift(cval c1, cval c2);
cval cval_bitand(cval c1, cval c2);
cval cval_bitor(cval c1, cval c2);
cval cval_bitxor(cval c1, cval c2);
cval cval_eq(cval c1, cval c2);
cval cval_leq(cval c1, cval c2);
bool uint_inrange(largest_uint x, type t);
bool sint_inrange(largest_int x, type t);
bool cval_inrange(cval c, type t);
largest_int cval_intcompare(cval c1, cval c2);
extern region parse_region;
typedef enum
{
  no_qualifiers = 0,
  transparent_qualifier = 1,
const_qualifier = 2,
volatile_qualifier = 4,
restrict_qualifier = 8,
  last_qualifier
} type_quals;
typedef struct user_qual_list
{
  user_qual qual;
  struct user_qual_list *next;
} *user_qual_list;
user_qual_list new_user_qual_list(user_qual, user_qual_list);
user_qual_list copy_user_qual_list(user_qual_list l);
bool member_user_qual_list(user_qual_list l, char *q);
user_qual_list union_user_qual_list(user_qual_list left,
                                    user_qual_list right);
bool empty_user_qual_list(user_qual_list);
user_qual_list type_user_quals(type t);
extern type float_type, double_type, long_double_type,
  int_type, unsigned_int_type, long_type, unsigned_long_type,
  long_long_type, unsigned_long_long_type, short_type, unsigned_short_type,
  char_type, char_array_type, wchar_type, wchar_array_type,
  unsigned_char_type, signed_char_type, void_type, ptr_void_type,
  size_t_type, ptrdiff_t_type, intptr_type;
extern type error_type;
void init_types(void);
type make_complex_type(type t);
type make_base_type(type t);
type make_qualified_type(type t, type_quals qualifiers,
                         user_qual_list user_quals);
type qualify_type1(type t, type t1);
type qualify_type2(type t, type t1, type t2);
type align_type(type t, int new_alignment);
type make_pointer_type(type t);
type make_array_type(type t, expression size);
type make_function_type(type t, typelist argtypes, bool varargs, type_quals varargs_quals, user_qual_list varargs_user_quals, bool oldstyle);
type make_tagged_type(tag_declaration d);
typelist new_typelist(region r);
void typelist_append(typelist l, type t);
bool empty_typelist(typelist l);
typedef struct typelist_element *typelist_scanner;
void typelist_scan(typelist tl, typelist_scanner *scanner);
type typelist_next(typelist_scanner *scanner);
size_t type_size(type t);
size_t type_alignment(type t);
bool type_has_size(type t);
bool type_size_cc(type t);
type common_type(type t1, type t2);
bool type_equal(type t1, type t2);
bool type_equal_unqualified(type t1, type t2);
bool type_compatible(type t1, type t2);
bool type_compatible_unqualified(type t1, type t2);
bool type_self_promoting(type t);
bool type_incomplete(type t);
char *qualifier_name(type_quals q);
type_quals type_qualifiers(type t);
bool qual_const(type_quals q);
bool qual_volatile(type_quals q);
bool qual_restrict(type_quals q);
bool force_qual_const(type_quals q);
bool force_qual_volatile(type_quals q);
bool force_qual_restrict(type_quals q);
bool type_const(type t);
bool type_volatile(type t);
bool type_restrict(type t);
bool type_transparent(type t);
bool type_readonly(type t);
bool type_atomic(type t);
bool type_plain_char(type t);
bool type_signed_char(type t);
bool type_unsigned_char(type t);
bool type_short(type t);
bool type_unsigned_short(type t);
bool type_int(type t);
bool type_unsigned_int(type t);
bool type_long(type t);
bool type_unsigned_long(type t);
bool type_long_long(type t);
bool type_unsigned_long_long(type t);
bool type_long_double(type t);
bool type_tagged(type t);
bool type_integral(type t);
bool type_floating(type t);
bool type_complex(type t);
bool type_float(type t);
bool type_double(type t);
bool type_void(type t);
bool type_char(type t);
bool type_function(type t);
bool type_array(type t);
bool type_pointer(type t);
bool type_enum(type t);
bool type_struct(type t);
bool type_union(type t);
bool type_integer(type t);
bool type_unsigned(type t);
bool type_smallerthanint(type t);
bool type_real(type t);
bool type_arithmetic(type t);
bool type_scalar(type t);
bool type_aggregate(type t);
type make_unsigned_type(type t);
type type_function_return_type(type t);
typelist type_function_arguments(type t);
bool type_function_varargs(type t);
type_quals type_function_varargs_quals(type t);
user_qual_list type_function_varargs_user_quals(type t);
bool type_function_oldstyle(type t);
bool self_promoting_args(type fntype);
type type_points_to(type t);
type type_array_of(type t);
type type_array_of_base(type t);
expression type_array_size(type t);
tag_declaration type_tag(type t);
type type_base(type t);
qtype tag_decl_qtype(tag_declaration td);
tag_ref tag_decl_to_tag_ref(tag_declaration td);
void tag_decl_set_qtype(tag_declaration td, qtype qt);
bool tag_decl_is_defined(tag_declaration td);
const char * tag_decl_name(tag_declaration td);
const char * field_decl_name(field_declaration fd);
typedef field_declaration tag_decl_scanner;
void tag_decl_scan(tag_declaration, tag_decl_scanner *);
field_declaration tag_decl_next(tag_decl_scanner *);
void type2ast(region r, location loc, type t, declarator inside,
              declarator *d, type_element *modifiers);
bool type_contains_pointers(type t);
bool type_contains_union_with_pointers(type t);
bool type_contains_quals(type t);
bool type_contains_user_quals(type t);
type type_default_conversion(type from);
type function_call_type(function_call fcall);
void name_tag(tag_declaration tag);
type type_for_size(int size, bool isunsigned);
type type_for_cval(cval c, bool isunsigned);
type make_type_var(cstring cs);
bool type_var(type t);
const char *type_name(type t);
         enum { struct_type, union_type, enum_type };
node new_node(region r, location loc);
declaration new_declaration(region r, location loc);
statement new_statement(region r, location loc);
expression new_expression(region r, location loc);
type_element new_type_element(region r, location loc);
declarator new_declarator(region r, location loc);
label new_label(region r, location loc);
asm_decl new_asm_decl(region r, location loc, asm_stmt asm_stmt);
data_decl new_data_decl(region r, location loc, type_element modifiers, attribute attributes, declaration decls);
extension_decl new_extension_decl(region r, location loc, declaration decl);
ellipsis_decl new_ellipsis_decl(region r, location loc, type_element qualifiers);
enumerator new_enumerator(region r, location loc, cstring cstring, expression arg1, data_declaration ddecl);
oldidentifier_decl new_oldidentifier_decl(region r, location loc, cstring cstring, data_declaration ddecl);
function_decl new_function_decl(region r, location loc, declarator declarator, type_element qualifiers, attribute attributes, declaration old_parms, statement stmt, function_decl parent_function, data_declaration ddecl);
implicit_decl new_implicit_decl(region r, location loc, identifier ident);
variable_decl new_variable_decl(region r, location loc, declarator declarator, attribute attributes, expression arg1, asm_stmt asm_stmt, data_declaration ddecl);
field_decl new_field_decl(region r, location loc, declarator declarator, attribute attributes, expression arg1);
asttype new_asttype(region r, location loc, declarator declarator, type_element qualifiers);
typename new_typename(region r, location loc, data_declaration ddecl);
type_variable new_type_variable(region r, location loc, cstring cstring);
typeof_expr new_typeof_expr(region r, location loc, expression arg1);
typeof_type new_typeof_type(region r, location loc, asttype asttype);
attribute new_attribute(region r, location loc, word word1, word word2, expression args);
rid new_rid(region r, location loc, enum rid id);
user_qual new_user_qual(region r, location loc, cstring cstring);
qualifier new_qualifier(region r, location loc, enum rid id);
tag_ref new_tag_ref(region r, location loc, word word1, attribute attributes, declaration fields, bool defined);
function_declarator new_function_declarator(region r, location loc, declarator declarator, declaration parms, type_element qualifiers, environment env);
pointer_declarator new_pointer_declarator(region r, location loc, declarator declarator, type_element qualifiers);
array_declarator new_array_declarator(region r, location loc, declarator declarator, expression arg1);
identifier_declarator new_identifier_declarator(region r, location loc, cstring cstring);
asm_stmt new_asm_stmt(region r, location loc, expression arg1, asm_operand asm_operands1, asm_operand asm_operands2, string asm_clobbers, type_element qualifiers);
compound_stmt new_compound_stmt(region r, location loc, id_label id_labels, declaration decls, statement stmts, environment env);
if_stmt new_if_stmt(region r, location loc, expression condition, statement stmt1, statement stmt2);
labeled_stmt new_labeled_stmt(region r, location loc, label label, statement stmt);
expression_stmt new_expression_stmt(region r, location loc, expression arg1);
breakable_stmt new_breakable_stmt(region r, location loc);
conditional_stmt new_conditional_stmt(region r, location loc, expression condition, statement stmt);
switch_stmt new_switch_stmt(region r, location loc, expression condition, statement stmt);
for_stmt new_for_stmt(region r, location loc, expression arg1, expression arg2, expression arg3, statement stmt);
break_stmt new_break_stmt(region r, location loc);
continue_stmt new_continue_stmt(region r, location loc);
return_stmt new_return_stmt(region r, location loc, expression arg1);
goto_stmt new_goto_stmt(region r, location loc, id_label id_label);
computed_goto_stmt new_computed_goto_stmt(region r, location loc, expression arg1);
empty_stmt new_empty_stmt(region r, location loc);
assert_type_stmt new_assert_type_stmt(region r, location loc, expression arg1, asttype asttype);
change_type_stmt new_change_type_stmt(region r, location loc, expression arg1, asttype asttype);
deep_restrict_stmt new_deep_restrict_stmt(region r, location loc, expression arg1, statement stmt);
unary new_unary(region r, location loc, expression arg1);
binary new_binary(region r, location loc, expression arg1, expression arg2);
comma new_comma(region r, location loc, expression arg1);
sizeof_type new_sizeof_type(region r, location loc, asttype asttype);
alignof_type new_alignof_type(region r, location loc, asttype asttype);
label_address new_label_address(region r, location loc, id_label id_label);
cast new_cast(region r, location loc, expression arg1, asttype asttype);
cast_list new_cast_list(region r, location loc, asttype asttype, expression init_expr);
conditional new_conditional(region r, location loc, expression condition, expression arg1, expression arg2);
identifier new_identifier(region r, location loc, cstring cstring, data_declaration ddecl);
compound_expr new_compound_expr(region r, location loc, statement stmt);
function_call new_function_call(region r, location loc, expression arg1, expression args, asttype va_arg_call);
array_ref new_array_ref(region r, location loc, expression arg1, expression arg2);
field_ref new_field_ref(region r, location loc, expression arg1, cstring cstring, location cstring_loc);
init_list new_init_list(region r, location loc, expression args);
init_index new_init_index(region r, location loc, expression arg1, expression arg2, expression init_expr);
init_field new_init_field(region r, location loc, word word1, expression init_expr);
lexical_cst new_lexical_cst(region r, location loc, cstring cstring);
string_cst new_string_cst(region r, location loc, cstring cstring, wchar_t * chars, size_t length);
string new_string(region r, location loc, expression strings, data_declaration ddecl);
id_label new_id_label(region r, location loc, cstring cstring);
case_label new_case_label(region r, location loc, expression arg1, expression arg2);
default_label new_default_label(region r, location loc);
word new_word(region r, location loc, cstring cstring);
asm_operand new_asm_operand(region r, location loc, string string, expression arg1);
error_decl new_error_decl(region r, location loc);
struct_ref new_struct_ref(region r, location loc, word word1, attribute attributes, declaration fields, bool defined);
union_ref new_union_ref(region r, location loc, word word1, attribute attributes, declaration fields, bool defined);
enum_ref new_enum_ref(region r, location loc, word word1, attribute attributes, declaration fields, bool defined);
error_stmt new_error_stmt(region r, location loc);
while_stmt new_while_stmt(region r, location loc, expression condition, statement stmt);
dowhile_stmt new_dowhile_stmt(region r, location loc, expression condition, statement stmt);
error_expr new_error_expr(region r, location loc);
dereference new_dereference(region r, location loc, expression arg1);
extension_expr new_extension_expr(region r, location loc, expression arg1);
sizeof_expr new_sizeof_expr(region r, location loc, expression arg1);
alignof_expr new_alignof_expr(region r, location loc, expression arg1);
realpart new_realpart(region r, location loc, expression arg1);
imagpart new_imagpart(region r, location loc, expression arg1);
address_of new_address_of(region r, location loc, expression arg1);
unary_minus new_unary_minus(region r, location loc, expression arg1);
unary_plus new_unary_plus(region r, location loc, expression arg1);
conjugate new_conjugate(region r, location loc, expression arg1);
preincrement new_preincrement(region r, location loc, expression arg1);
predecrement new_predecrement(region r, location loc, expression arg1);
postincrement new_postincrement(region r, location loc, expression arg1);
postdecrement new_postdecrement(region r, location loc, expression arg1);
bitnot new_bitnot(region r, location loc, expression arg1);
not new_not(region r, location loc, expression arg1);
plus new_plus(region r, location loc, expression arg1, expression arg2);
minus new_minus(region r, location loc, expression arg1, expression arg2);
times new_times(region r, location loc, expression arg1, expression arg2);
divide new_divide(region r, location loc, expression arg1, expression arg2);
modulo new_modulo(region r, location loc, expression arg1, expression arg2);
lshift new_lshift(region r, location loc, expression arg1, expression arg2);
rshift new_rshift(region r, location loc, expression arg1, expression arg2);
leq new_leq(region r, location loc, expression arg1, expression arg2);
geq new_geq(region r, location loc, expression arg1, expression arg2);
lt new_lt(region r, location loc, expression arg1, expression arg2);
gt new_gt(region r, location loc, expression arg1, expression arg2);
eq new_eq(region r, location loc, expression arg1, expression arg2);
ne new_ne(region r, location loc, expression arg1, expression arg2);
bitand new_bitand(region r, location loc, expression arg1, expression arg2);
bitor new_bitor(region r, location loc, expression arg1, expression arg2);
bitxor new_bitxor(region r, location loc, expression arg1, expression arg2);
andand new_andand(region r, location loc, expression arg1, expression arg2);
oror new_oror(region r, location loc, expression arg1, expression arg2);
assign new_assign(region r, location loc, expression arg1, expression arg2);
plus_assign new_plus_assign(region r, location loc, expression arg1, expression arg2);
minus_assign new_minus_assign(region r, location loc, expression arg1, expression arg2);
times_assign new_times_assign(region r, location loc, expression arg1, expression arg2);
divide_assign new_divide_assign(region r, location loc, expression arg1, expression arg2);
modulo_assign new_modulo_assign(region r, location loc, expression arg1, expression arg2);
lshift_assign new_lshift_assign(region r, location loc, expression arg1, expression arg2);
rshift_assign new_rshift_assign(region r, location loc, expression arg1, expression arg2);
bitand_assign new_bitand_assign(region r, location loc, expression arg1, expression arg2);
bitor_assign new_bitor_assign(region r, location loc, expression arg1, expression arg2);
bitxor_assign new_bitxor_assign(region r, location loc, expression arg1, expression arg2);
struct AST_node {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_declaration {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_statement {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_expression {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
};
struct AST_type_element {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_declarator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_label {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  label next_label;
  compound_stmt enclosing_cs;
  store store_in;
};
struct AST_asm_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  asm_stmt asm_stmt;
};
struct AST_data_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type_element modifiers;
  attribute attributes;
  declaration decls;
};
struct AST_extension_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declaration decl;
};
struct AST_ellipsis_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type_element qualifiers;
};
struct AST_enumerator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
  expression arg1;
  data_declaration ddecl;
};
struct AST_oldidentifier_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
  data_declaration ddecl;
};
struct AST_function_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  type_element qualifiers;
  attribute attributes;
  declaration old_parms;
  statement stmt;
  function_decl parent_function;
  data_declaration ddecl;
  function_declarator fdeclarator;
  type declared_type;
  env undeclared_variables;
  env base_labels;
  env scoped_labels;
  breakable_stmt current_loop;
  int nlocals;
  effect scope_env;
};
struct AST_implicit_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  identifier ident;
};
struct AST_variable_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  attribute attributes;
  expression arg1;
  asm_stmt asm_stmt;
  data_declaration ddecl;
  type declared_type;
  bool forward;
  effect arg1_eff;
  rinfo rinfo;
};
struct AST_field_decl {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  attribute attributes;
  expression arg1;
};
struct AST_asttype {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  type_element qualifiers;
  type type;
};
struct AST_typename {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  data_declaration ddecl;
};
struct AST_type_variable {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
};
struct AST_typeof_expr {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
};
struct AST_typeof_type {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  asttype asttype;
};
struct AST_attribute {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  word word1;
  word word2;
  expression args;
};
struct AST_rid {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  enum rid id;
};
struct AST_user_qual {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
};
struct AST_qualifier {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  enum rid id;
};
struct AST_tag_ref {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  word word1;
  attribute attributes;
  declaration fields;
  bool defined;
  tag_declaration tdecl;
};
struct AST_function_declarator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  declaration parms;
  type_element qualifiers;
  environment env;
};
struct AST_pointer_declarator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  type_element qualifiers;
};
struct AST_array_declarator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  declarator declarator;
  expression arg1;
};
struct AST_identifier_declarator {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
};
struct AST_asm_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
  asm_operand asm_operands1;
  asm_operand asm_operands2;
  string asm_clobbers;
  type_element qualifiers;
};
struct AST_compound_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  id_label id_labels;
  declaration decls;
  statement stmts;
  compound_stmt enclosing_cs;
  int visited;
  environment env;
  effect scope_env;
  effect filter_env;
  dd_list confine_expressions;
  dd_list drinfolist;
};
struct AST_if_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression condition;
  statement stmt1;
  statement stmt2;
};
struct AST_labeled_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  label label;
  statement stmt;
};
struct AST_expression_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
};
struct AST_breakable_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  compound_stmt enclosing_cs;
  breakable_stmt parent_loop;
  store break_dest;
  store continue_dest;
};
struct AST_conditional_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  compound_stmt enclosing_cs;
  breakable_stmt parent_loop;
  store break_dest;
  store continue_dest;
  expression condition;
  statement stmt;
};
struct AST_switch_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  compound_stmt enclosing_cs;
  breakable_stmt parent_loop;
  store break_dest;
  store continue_dest;
  expression condition;
  statement stmt;
  label next_label;
};
struct AST_for_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  compound_stmt enclosing_cs;
  breakable_stmt parent_loop;
  store break_dest;
  store continue_dest;
  expression arg1;
  expression arg2;
  expression arg3;
  statement stmt;
};
struct AST_break_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  breakable_stmt parent_loop;
  compound_stmt enclosing_cs;
};
struct AST_continue_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  breakable_stmt parent_loop;
  compound_stmt enclosing_cs;
};
struct AST_return_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
  compound_stmt enclosing_cs;
};
struct AST_goto_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  id_label id_label;
  compound_stmt enclosing_cs;
};
struct AST_computed_goto_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
};
struct AST_empty_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
};
struct AST_assert_type_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
  asttype asttype;
  qtype qtype;
};
struct AST_change_type_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
  asttype asttype;
  qtype qtype;
};
struct AST_deep_restrict_stmt {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  expression arg1;
  statement stmt;
  drinfo drinfo;
};
struct AST_unary {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
};
struct AST_binary {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  expression arg2;
};
struct AST_comma {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
};
struct AST_sizeof_type {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  asttype asttype;
};
struct AST_alignof_type {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  asttype asttype;
};
struct AST_label_address {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  id_label id_label;
};
struct AST_cast {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  asttype asttype;
};
struct AST_cast_list {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  asttype asttype;
  qtype astqtype;
  expression init_expr;
};
struct AST_conditional {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression condition;
  expression arg1;
  expression arg2;
};
struct AST_identifier {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  cstring cstring;
  data_declaration ddecl;
  aloc aloc;
};
struct AST_compound_expr {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  statement stmt;
  effect filter_env;
};
struct AST_function_call {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  expression args;
  asttype va_arg_call;
};
struct AST_array_ref {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  expression arg2;
  expression alt;
};
struct AST_field_ref {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  cstring cstring;
  location cstring_loc;
};
struct AST_init_list {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression args;
};
struct AST_init_index {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression arg1;
  expression arg2;
  expression init_expr;
};
struct AST_init_field {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  word word1;
  expression init_expr;
};
struct AST_lexical_cst {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  cstring cstring;
};
struct AST_string_cst {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  cstring cstring;
  wchar_t * chars;
  size_t length;
};
struct AST_string {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  type type;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  qtype qtype;
  expression strings;
  data_declaration ddecl;
};
struct AST_id_label {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  label next_label;
  compound_stmt enclosing_cs;
  store store_in;
  cstring cstring;
  label_declaration ldecl;
};
struct AST_case_label {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  label next_label;
  compound_stmt enclosing_cs;
  store store_in;
  expression arg1;
  expression arg2;
};
struct AST_default_label {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  label next_label;
  compound_stmt enclosing_cs;
  store store_in;
};
struct AST_word {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  cstring cstring;
};
struct AST_asm_operand {
  ast_kind kind;
  location loc;
  node next;
  node parent;
  string string;
  expression arg1;
};
node node_chain(node l1, node l2);
declaration declaration_chain(declaration l1, declaration l2);
statement statement_chain(statement l1, statement l2);
expression expression_chain(expression l1, expression l2);
type_element type_element_chain(type_element l1, type_element l2);
declarator declarator_chain(declarator l1, declarator l2);
label label_chain(label l1, label l2);
asm_decl asm_decl_chain(asm_decl l1, asm_decl l2);
data_decl data_decl_chain(data_decl l1, data_decl l2);
extension_decl extension_decl_chain(extension_decl l1, extension_decl l2);
ellipsis_decl ellipsis_decl_chain(ellipsis_decl l1, ellipsis_decl l2);
enumerator enumerator_chain(enumerator l1, enumerator l2);
oldidentifier_decl oldidentifier_decl_chain(oldidentifier_decl l1, oldidentifier_decl l2);
function_decl function_decl_chain(function_decl l1, function_decl l2);
implicit_decl implicit_decl_chain(implicit_decl l1, implicit_decl l2);
variable_decl variable_decl_chain(variable_decl l1, variable_decl l2);
field_decl field_decl_chain(field_decl l1, field_decl l2);
asttype asttype_chain(asttype l1, asttype l2);
typename typename_chain(typename l1, typename l2);
type_variable type_variable_chain(type_variable l1, type_variable l2);
typeof_expr typeof_expr_chain(typeof_expr l1, typeof_expr l2);
typeof_type typeof_type_chain(typeof_type l1, typeof_type l2);
attribute attribute_chain(attribute l1, attribute l2);
rid rid_chain(rid l1, rid l2);
user_qual user_qual_chain(user_qual l1, user_qual l2);
qualifier qualifier_chain(qualifier l1, qualifier l2);
tag_ref tag_ref_chain(tag_ref l1, tag_ref l2);
function_declarator function_declarator_chain(function_declarator l1, function_declarator l2);
pointer_declarator pointer_declarator_chain(pointer_declarator l1, pointer_declarator l2);
array_declarator array_declarator_chain(array_declarator l1, array_declarator l2);
identifier_declarator identifier_declarator_chain(identifier_declarator l1, identifier_declarator l2);
asm_stmt asm_stmt_chain(asm_stmt l1, asm_stmt l2);
compound_stmt compound_stmt_chain(compound_stmt l1, compound_stmt l2);
if_stmt if_stmt_chain(if_stmt l1, if_stmt l2);
labeled_stmt labeled_stmt_chain(labeled_stmt l1, labeled_stmt l2);
expression_stmt expression_stmt_chain(expression_stmt l1, expression_stmt l2);
breakable_stmt breakable_stmt_chain(breakable_stmt l1, breakable_stmt l2);
conditional_stmt conditional_stmt_chain(conditional_stmt l1, conditional_stmt l2);
switch_stmt switch_stmt_chain(switch_stmt l1, switch_stmt l2);
for_stmt for_stmt_chain(for_stmt l1, for_stmt l2);
break_stmt break_stmt_chain(break_stmt l1, break_stmt l2);
continue_stmt continue_stmt_chain(continue_stmt l1, continue_stmt l2);
return_stmt return_stmt_chain(return_stmt l1, return_stmt l2);
goto_stmt goto_stmt_chain(goto_stmt l1, goto_stmt l2);
computed_goto_stmt computed_goto_stmt_chain(computed_goto_stmt l1, computed_goto_stmt l2);
empty_stmt empty_stmt_chain(empty_stmt l1, empty_stmt l2);
assert_type_stmt assert_type_stmt_chain(assert_type_stmt l1, assert_type_stmt l2);
change_type_stmt change_type_stmt_chain(change_type_stmt l1, change_type_stmt l2);
deep_restrict_stmt deep_restrict_stmt_chain(deep_restrict_stmt l1, deep_restrict_stmt l2);
unary unary_chain(unary l1, unary l2);
binary binary_chain(binary l1, binary l2);
comma comma_chain(comma l1, comma l2);
sizeof_type sizeof_type_chain(sizeof_type l1, sizeof_type l2);
alignof_type alignof_type_chain(alignof_type l1, alignof_type l2);
label_address label_address_chain(label_address l1, label_address l2);
cast cast_chain(cast l1, cast l2);
cast_list cast_list_chain(cast_list l1, cast_list l2);
conditional conditional_chain(conditional l1, conditional l2);
identifier identifier_chain(identifier l1, identifier l2);
compound_expr compound_expr_chain(compound_expr l1, compound_expr l2);
function_call function_call_chain(function_call l1, function_call l2);
array_ref array_ref_chain(array_ref l1, array_ref l2);
field_ref field_ref_chain(field_ref l1, field_ref l2);
init_list init_list_chain(init_list l1, init_list l2);
init_index init_index_chain(init_index l1, init_index l2);
init_field init_field_chain(init_field l1, init_field l2);
lexical_cst lexical_cst_chain(lexical_cst l1, lexical_cst l2);
string_cst string_cst_chain(string_cst l1, string_cst l2);
string string_chain(string l1, string l2);
id_label id_label_chain(id_label l1, id_label l2);
case_label case_label_chain(case_label l1, case_label l2);
default_label default_label_chain(default_label l1, default_label l2);
word word_chain(word l1, word l2);
asm_operand asm_operand_chain(asm_operand l1, asm_operand l2);
error_decl error_decl_chain(error_decl l1, error_decl l2);
struct_ref struct_ref_chain(struct_ref l1, struct_ref l2);
union_ref union_ref_chain(union_ref l1, union_ref l2);
enum_ref enum_ref_chain(enum_ref l1, enum_ref l2);
error_stmt error_stmt_chain(error_stmt l1, error_stmt l2);
while_stmt while_stmt_chain(while_stmt l1, while_stmt l2);
dowhile_stmt dowhile_stmt_chain(dowhile_stmt l1, dowhile_stmt l2);
error_expr error_expr_chain(error_expr l1, error_expr l2);
dereference dereference_chain(dereference l1, dereference l2);
extension_expr extension_expr_chain(extension_expr l1, extension_expr l2);
sizeof_expr sizeof_expr_chain(sizeof_expr l1, sizeof_expr l2);
alignof_expr alignof_expr_chain(alignof_expr l1, alignof_expr l2);
realpart realpart_chain(realpart l1, realpart l2);
imagpart imagpart_chain(imagpart l1, imagpart l2);
address_of address_of_chain(address_of l1, address_of l2);
unary_minus unary_minus_chain(unary_minus l1, unary_minus l2);
unary_plus unary_plus_chain(unary_plus l1, unary_plus l2);
conjugate conjugate_chain(conjugate l1, conjugate l2);
preincrement preincrement_chain(preincrement l1, preincrement l2);
predecrement predecrement_chain(predecrement l1, predecrement l2);
postincrement postincrement_chain(postincrement l1, postincrement l2);
postdecrement postdecrement_chain(postdecrement l1, postdecrement l2);
bitnot bitnot_chain(bitnot l1, bitnot l2);
not not_chain(not l1, not l2);
plus plus_chain(plus l1, plus l2);
minus minus_chain(minus l1, minus l2);
times times_chain(times l1, times l2);
divide divide_chain(divide l1, divide l2);
modulo modulo_chain(modulo l1, modulo l2);
lshift lshift_chain(lshift l1, lshift l2);
rshift rshift_chain(rshift l1, rshift l2);
leq leq_chain(leq l1, leq l2);
geq geq_chain(geq l1, geq l2);
lt lt_chain(lt l1, lt l2);
gt gt_chain(gt l1, gt l2);
eq eq_chain(eq l1, eq l2);
ne ne_chain(ne l1, ne l2);
bitand bitand_chain(bitand l1, bitand l2);
bitor bitor_chain(bitor l1, bitor l2);
bitxor bitxor_chain(bitxor l1, bitxor l2);
andand andand_chain(andand l1, andand l2);
oror oror_chain(oror l1, oror l2);
assign assign_chain(assign l1, assign l2);
plus_assign plus_assign_chain(plus_assign l1, plus_assign l2);
minus_assign minus_assign_chain(minus_assign l1, minus_assign l2);
times_assign times_assign_chain(times_assign l1, times_assign l2);
divide_assign divide_assign_chain(divide_assign l1, divide_assign l2);
modulo_assign modulo_assign_chain(modulo_assign l1, modulo_assign l2);
lshift_assign lshift_assign_chain(lshift_assign l1, lshift_assign l2);
rshift_assign rshift_assign_chain(rshift_assign l1, rshift_assign l2);
bitand_assign bitand_assign_chain(bitand_assign l1, bitand_assign l2);
bitor_assign bitor_assign_chain(bitor_assign l1, bitor_assign l2);
bitxor_assign bitxor_assign_chain(bitxor_assign l1, bitxor_assign l2);
typedef struct AST_ast_generic
{
  ast_kind kind;
} *ast_generic;
extern declaration the_program;
unary newkind_unary(region r, ast_kind kind, location location, expression arg1);
binary newkind_binary(region r, ast_kind kind, location location,
                      expression arg1, expression arg2);
tag_ref newkind_tag_ref(region r, ast_kind kind, location location, word word1, attribute attributes, declaration fields, bool defined);
node last_node(node n);
int chain_length(node n);
node ast_chain(node l1, node l2);
void insert_before(node *list, node before, node n);
node ast_reverse(node l);
void AST_set_parents(node n);
void AST_print(node n);
typedef dd_list identifier_set; typedef dd_list_pos identifier_set_scanner; static inline identifier_set empty_identifier_set(region r) { return dd_new_list(r); } static inline identifier_set identifier_set_copy(region r, identifier_set s) { if (s == ((void *)0)) return ((void *)0); return dd_copy(r, s); } static inline bool identifier_set_empty(identifier_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool identifier_set_member(int (*cmp)(identifier, identifier), identifier_set s, identifier elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp, (void *) elt) != ((void *)0); } static inline int identifier_set_size(identifier_set s) { if (s == ((void *)0)) return 0; return dd_length(s); } static inline bool identifier_set_insert(region r, identifier_set *s, identifier elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool identifier_set_insert_last(region r, identifier_set *s, identifier elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline identifier_set identifier_set_union(identifier_set s1, identifier_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool identifier_set_single(identifier_set s) { return identifier_set_size(s) == 1; } static inline void identifier_set_sort(int (*cmp)(identifier, identifier), identifier_set s) { if (s == ((void *)0)) return; dd_sort(s, (set_cmp_fn) cmp); } static inline void identifier_set_remove_dups(int (*cmp)(identifier, identifier), identifier_set s) { if (s == ((void *)0)) return; dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void identifier_set_scan(identifier_set s, identifier_set_scanner *ss) { if (s == ((void *)0)) *ss = ((void *)0); else *ss = dd_first(s); } static inline identifier identifier_set_next(identifier_set_scanner *ss) { identifier result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((identifier)((*ss)->data)); *ss = ((*ss)->next); return result; };
extern int flag_volatile;
extern int flag_volatile_global;
extern int flag_syntax_only;
extern int flag_pedantic_errors;
extern int flag_pack_struct;
extern int inhibit_warnings;
extern int extra_warnings;
extern int warnings_are_errors;
extern int warn_unused;
extern int warn_uninitialized;
extern int warn_shadow;
extern int error_shadow;
extern int warn_switch;
extern int warn_return_type;
extern int warn_cast_align;
extern int warn_id_clash;
extern unsigned id_clash_len;
extern int warn_larger_than;
extern unsigned larger_than_size;
extern int warn_inline;
extern int warn_aggregate_return;
extern int dollars_in_ident;
extern int flag_cond_mismatch;
extern int flag_no_asm;
extern int flag_hosted;
extern int warn_implicit;
extern int warn_write_strings;
extern int warn_pointer_arith;
extern int warn_strict_prototypes;
extern int warn_redundant_decls;
extern int warn_nested_externs;
extern int warn_cast_qual;
extern int warn_bad_function_cast;
extern int warn_traditional;
extern int warn_format;
extern int warn_char_subscripts;
extern int warn_conversion;
extern int warn_main;
extern int warn_multichar;
extern int flag_traditional;
extern int flag_allow_single_precision;
extern int warn_parentheses;
extern int warn_missing_braces;
extern int warn_sign_compare;
extern int mesg_implicit_function_declaration;
extern bool pedantic;
extern int warn_implicit_int;
extern int warn_missing_prototypes;
extern int warn_missing_declarations;
extern int quiet_flag;
extern int flag_signed_char;
extern int flag_short_enums;
extern int flag_signed_bitfields;
extern int explicit_flag_signed_bitfields;
extern int flag_parse_only;
extern int flag_pam_mode;
extern int flag_pam_html;
extern int flag_print_quals_graph;
extern int flag_strict_const;
extern int flag_print_results;
extern int flag_casts_preserve;
extern int flag_const_subtyping;
extern int flag_flow_sensitive;
extern int flag_poly;
extern int flag_aloc_subtyping;
extern int flag_force_flow_sensitive;
extern int flag_print_lin;
extern int flag_driver;
extern int flag_casts_warn;
extern int flag_ugly;
extern int flag_statistics;
extern int flag_print_stores;
extern int flag_confine_inf;
extern int flag_discover_scopes;
extern int flag_confine_inf_aggressive;
extern int flag_restrict_inf;
extern int flag_print_assert_type_failures;
int flag_strong_updates;
extern int flag_errors_only;
extern int flag_context_summary;
extern int flag_warn_dangerous_globals;
extern int flag_measure_consts;
extern int errorcount;
extern int warningcount;
void verror_with_file_and_line(const char *filename, int lineno,
                               const char *format, va_list args);
void verror_with_location(location l, const char *format, va_list args);
void verror_with_decl(declaration d, const char *format, va_list args);
void verror(const char *format, va_list args);
void error(const char *format, ...);
void error_with_decl(declaration d, const char *format, ...);
void error_with_location(location l, const char *format, ...);
void vfatal(const char *format, va_list args);
void fatal(const char *format, ...);
void vwarning_with_file_and_line(const char *filename, int lineno,
                                 const char *format, va_list args);
void vwarning_with_location(location l, const char *format, va_list args);
void vwarning_with_decl(declaration d, const char *format, va_list args);
void vwarning(const char *format, va_list args);
void warning(const char *format, ...);
void warning_with_file_and_line(const char *filename, int lineno,
                                const char *format, ...);
void warning_with_decl(declaration d, const char *format, ...);
void warning_with_location(location l, const char *format, ...);
void warning_or_error(bool iswarning, const char *format, ...);
void warning_or_error_with_file_and_line(bool iswarning,
                                         const char *filename, int lineno,
                                         const char *format, ...);
void warning_or_error_with_decl(bool iswarning, declaration d,
                                const char *format, ...);
void warning_or_error_with_location(bool iswarning, location l,
                                    const char *format, ...);
void pedwarn(const char *format, ...);
void pedwarn_with_decl(declaration d, const char *format, ...);
void pedwarn_with_location(location l, const char *format, ...);
extern char *progname;
extern int copy_argc;
extern char **copy_argv;
data_declaration get_parameter(declaration d);
function_declarator get_fdeclarator(declarator d);
bool oldstyle_function(function_decl fn);
bool is_localvar(expression e);
compound_stmt parent_block(node n);
function_decl parent_function(node n);
expression expression_of_stmt(compound_expr ce);
bool expression_used(expression e);
bool is_assignment(void *e);
bool is_increment(void *e);
data_declaration base_identifier(data_declaration d);
bool same_function(data_declaration d1, data_declaration d2);
bool call_to(data_declaration fnd, function_call fce);
expression ignore_fields(expression e);
bool zero_expression(expression e);
expression build_int_constant(region r, location loc, type t, largest_int c);
expression build_uint_constant(region r, location loc, type t, largest_uint c);
expression build_identifier(region r, location loc, data_declaration id);
cval value_of_enumerator(enumerator e);
char *string_to_charp(region r, string s);
expression make_error_expr(location loc);
expression make_comma(location loc, expression elist);
expression make_dereference(location loc, expression e);
expression make_extension_expr(location loc, expression e);
expression make_unary(location loc, int unop, expression e);
expression make_label_address(location loc, id_label label);
expression make_sizeof_expr(location loc, expression e);
expression make_sizeof_type(location loc, asttype t);
expression make_alignof_expr(location loc, expression e);
expression make_alignof_type(location loc, asttype t);
expression make_cast(location loc, asttype t, expression e);
expression make_binary(location loc, int binop, expression e1, expression e2);
expression make_conditional(location loc, expression cond,
                            expression true, expression false);
expression make_assign(location loc, int assignop, expression left, expression right);
expression make_identifier(location loc, cstring id, bool maybe_implicit);
expression make_compound_expr(location loc, statement block);
expression make_function_call(location loc, expression fn, expression arglist);
expression make_va_arg(location loc, expression arg, asttype type);
expression make_array_ref(location loc, expression array, expression index);
expression make_field_ref(location loc, expression object, cstring field,
                          location cstring_loc);
expression make_field_indirectref(location loc, expression object,
                                  cstring field);
expression make_postincrement(location loc, expression e);
expression make_preincrement(location loc, expression e);
expression make_postdecrement(location loc, expression e);
expression make_predecrement(location loc, expression e);
string make_string(location loc, expression string_components);
bool check_assignment(type lhstype, type rhstype, expression rhs,
                      const char *context, data_declaration fundecl,
                      const char *funname, int parmnum);
bool check_conversion(type to, type from);
type default_conversion(expression e);
type default_conversion_for_assignment(expression e);
extern data_declaration gcc_builtin_classify_type, builtin_memcpy, builtin_memcmp,
  builtin_strcpy, builtin_strcmp, builtin_strlen;
bool builtin_declaration(data_declaration dd);
void builtins_init(void);
struct yystype {
  union {
    void *ptr;
    asm_operand asm_operand;
    asm_stmt asm_stmt;
    attribute attribute;
    lexical_cst constant;
    string_cst string_cst;
    declaration decl;
    declarator declarator;
    expression expr;
    id_label id_label;
    label label;
    node node;
    statement stmt;
    conditional_stmt cstmt;
    for_stmt for_stmt;
    string string;
    type_element telement;
    asttype type;
    word word;
    struct {
      location location;
      int i;
    } itoken;
    struct {
      location location;
      expression expr;
      int i;
    } iexpr;
    struct {
      statement stmt;
      int i;
    } istmt;
  } u;
  struct {
    location location;
    cstring id;
    data_declaration decl;
  } idtoken;
};
int yyparse(void) ;
extern region parse_region;
extern int unevaluated_expression;
int parse(char*);
struct known_cst {
  type type;
  cval cval;
};
known_cst make_unknown_cst(type t);
known_cst make_cst(cval c, type t);
known_cst make_address_cst(data_declaration ddecl, label_declaration ldecl,
                           largest_int offset, type t);
known_cst make_signed_cst(largest_int x, type t);
known_cst make_unsigned_cst(largest_uint x, type t);
lexical_cst fold_lexical_int(type itype, location loc, cstring tok,
                             bool iscomplex, largest_uint intvalue, bool overflow);
lexical_cst fold_lexical_real(type realtype, location loc, cstring tok);
lexical_cst fold_lexical_char(location loc, cstring tok,
                              bool wide_flag, int charvalue);
string_cst fold_lexical_string(location loc, cstring tok,
                               bool wide_flag, wchar_array stringvalue);
known_cst fold_label_address(expression e);
known_cst fold_sizeof(expression e, type stype);
known_cst fold_alignof(expression e, type atype);
known_cst fold_cast(expression e);
known_cst fold_address_of(expression e);
known_cst fold_unary(expression e);
known_cst fold_binary(type restype, expression e);
known_cst fold_conditional(expression e);
known_cst fold_identifier(expression e, data_declaration decl);
known_cst fold_add(type restype, known_cst c1, known_cst c2);
known_cst foldaddress_identifier(expression e, data_declaration decl);
known_cst foldaddress_string(string s);
known_cst foldaddress_field_ref(known_cst object, field_declaration fdecl);
bool definite_null(expression e);
bool definite_zero(expression e);
bool definite_one(expression e);
bool is_zero_constant(known_cst c);
bool is_one_constant(known_cst c);
void constant_overflow_warning(known_cst c);
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
typedef struct Qtypelist *qtypelist;
typedef qtypelist qtypelist_scanner;
typedef void *field_scanner;
void init_qtype(void);
void qtypelist_scan(qtypelist l, qtypelist_scanner *);
qtype qtypelist_next(qtypelist_scanner *);
bool qtype_error(qtype qt);
bool qtype_primitive(qtype qt);
bool qtype_char(qtype qt);
bool qtype_signed_char(qtype qt);
bool qtype_unsigned_char(qtype qt);
bool qtype_int(qtype qt);
bool qtype_unsigned_int(qtype qt);
bool qtype_float(qtype qt);
bool qtype_complex(qtype qt);
bool qtype_aggregate(qtype qt);
bool qtype_struct(qtype qt);
bool qtype_union(qtype qt);
bool qtype_transparent(qtype qt);
bool qtype_void(qtype qt);
bool qtype_pointer(qtype qt);
bool qtype_pointer_loc(qtype qt);
bool qtype_pointer_or_loc(qtype qt);
bool qtype_function(qtype qt);
bool qtype_varargs(qtype qt);
bool qtype_oldstyle(qtype qt);
bool qtype_array(qtype qt);
bool qtype_unsigned(qtype qt);
bool qtype_smallerthanint(qtype qt);
bool qtype_integral(qtype qt);
bool qtype_string(qtype qt);
bool qtype_var(qtype qt);
bool qtype_scheme(qtype qt);
bool qtype_void_pointer(qtype qt);
bool qtype_char_pointer(qtype qt);
extern qtype error_qtype;
qtype mkqtype_void(qual);
qtype mkqtype_bool(qual);
qtype mkqtype_char(qual);
qtype mkqtype_int(qual);
qtype mkqtype_unsigned_int(qual);
qtype mkqtype_long(qual);
qtype mkqtype_unsigned_long(qual);
qtype mkqtype_long_long(qual);
qtype mkqtype_unsigned_long_long(qual);
qtype mkqtype_float(qual);
qtype mkqtype_double(qual);
qtype mkqtype_size_t(qual);
qtype mkqtype_ptrdiff_t(qual);
qtype mkqtype_pointer(location loc, qual, qtype, aloc);
qtype mkqtype_pointer_loc(location loc, qual, aloc);
qtype mkqtype_function(qual q, bool varargs, qual vquals, bool oldstyle,
                       qtype ret, qtypelist args, store store_in,
                       store store_out, aloc al, effect eff);
qtype mkqtype_var(qual q, const char *name);
qtype mkqtype_fresh(void);
qtype complex_base_qtype(qtype qt);
qtype points_to_qtype(qtype qt);
aloc aloc_qtype(qtype qt);
qtype array_of_qtype(qtype qt);
expression array_size_qtype(qtype qt);
aloc array_aloc_qtype(qtype qt);
qtype return_qtype(qtype qt);
qtypelist arg_qtypes(qtype qt);
effect effect_qtype(qtype qt);
qual vqual_qtype(qtype qt);
store store_in_qtype(qtype qt);
store store_out_qtype(qtype qt);
const char *tag_name_qtype(qtype qt);
qtype field_qtype(qtype qt, cstring field);
void field_scan(qtype qt, field_scanner *);
void field_scan_at(qtype qt, cstring field, field_scanner *);
qtype field_next(field_scanner *);
qual qual_qtype(qtype qt);
bool has_qual_qtype(qtype qt, qual bound);
effect alocs_qtype(qtype qt);
effect rwr_alocs_qtype(qtype qt);
effect defn_effect_qtype(qtype qt);
effect constr_effect_qtype(qtype qt, eff_kind k);
qtype decl_to_qtype(type lifted_t, location loc, const char *name,
                    bool preferred, bool isglobal, bool generalize,
                    bool noreturn, bool __init);
qtype get_fdecl_qtype(const char *tag_name, field_declaration fdecl, type ft, location loc);
qtype type_to_qtype(type, const char *, location loc);
qtype copy_qtype(qtype qt, location loc);
qtype copy_qtype_poly(qtype qt, location loc);
qtype generalize_qtype(qtype qt);
qtype instantiate_qtype(qtype qt, location loc);
bool match_qtype(qtype qt1, qtype qt2,
                 void (*match_quals)(qual q1, qual q2, void *arg), void *arg);
bool match_type(type t1, type t2);
bool mkleq_qtype(location loc, qual_gate *qgate, qtype qt1, qtype qt2);
bool mkeq_qtype(location loc, qual_gate *qgate, qtype qt1, qtype qt2);
bool mkinst_qtype(location loc, qual_gate *qgate, qtype qt1, qtype qt2, polarity p);
bool unify_qtype(location loc, qtype qt1, qtype qt2);
bool mkeq_qtype_cast(location loc, qual_gate *qgate, qtype qt1, qtype qt2);
bool lub_qtype(location loc, qual_gate *qgate, qtype qt1, qtype qt2, qtype *lub);
bool eq_qtype(qtype qt1, qtype qt2);
bool varargs_constrain_quals_qtype(location loc, qtype qt, qual q);
void mknin_effect_qtype(location loc, effect e, qtype qt);
qtype default_conversion_qtype(location loc, qtype qt);
bool mkleq_assign_convert(location loc, qual_gate *qgate, qtype rhs_qtype,
                          qtype lhs_qtype);
int print_qtype(printf_func, qtype, store);
int print_qtype_raw(printf_func, qtype, store);
int print_qtype_qf(printf_func, pr_qual_fn, qtype, store, bool);
const char *color_qtype(qtype, store);
bool nontriv_soln_qtype(qtype qt, store s);
store ref_qtype_store(location loc, qtype qt, store in);
qtype qtype_to_fs_qtype_with_quals(location loc, qtype qt, store s);
qtype qtype_to_fs_qtype(location loc, qtype qt);
bool store_aloc_qtype(qtype qt, store s, aloc al);
bool toplvl_qual_fs_qtype(qtype qt);
void mk_no_qual_qtype_fs(location loc, qtype qt, store s);
qtype points_to_fs_qtype(qtype qt, store s);
qtype default_conversion_fs_qtype(location loc, qtype fs_qt);
bool mkleq_fs_qtype(location loc, qtype qt1, qtype qt2, store s);
bool mkleq_fs_assign_convert(location loc, qtype rhs_qtype, qtype lhs_qtype,
                             store s);
store assign_flow_sensitive(location loc, const char *err_msg,
                            qtype rhs_qtype, qtype lhs_qtype,
                            store s, bool strong);
void init_effects(void);
void init_store(void);
aloc make_aloc(const char *name, qtype points_to, bool global);
void set_points_to_aloc(aloc al, qtype points_to);
const char *name_aloc(aloc al);
unsigned long hash_aloc(aloc al);
bool global_aloc(aloc al);
unsigned long hash_aloc(aloc al);
qtype points_to_aloc(aloc al);
bool eq_aloc(aloc al1, aloc al2);
void unify_aloc(location loc, aloc al1, aloc al2);
void inst_aloc(location loc,aloc al1, aloc al2, polarity p);
int print_aloc(printf_func pf, aloc al);
extern effect effect_empty;
aloc aloc_effect(effect e);
void mark_aloc_interesting(aloc al);
effect effect_single(aloc al);
effect effect_alloc(aloc al);
effect effect_rwr(aloc al);
effect effect_r(aloc al);
effect effect_wr(aloc al);
effect effect_constr(aloc al, eff_kind k);
effect effect_var(const char *name);
effect effect_fresh(void);
effect effect_union(effect e1, effect e2);
effect effect_inter(effect e1, effect e2);
void mark_effect_interesting(effect e);
bool eq_effect(effect e1, effect e2);
void mkleq_effect(effect e1, effect e2);
void mkinst_effect(location l,effect e1, effect e2, polarity p);
void mkeq_effect(effect e1, effect e2);
void unify_effect(effect e1, effect e2);
void mknin_aloc_effect(location loc, aloc al, eff_kind kind, effect e);
void mknin_effect(location loc, effect e1, effect e2);
rinf_const mkrinf_constraint(location loc,
                             aloc r_al, aloc old_al,
                             aloc top_al,
                             effect r_type, effect pt_type,
                             effect r_body, effect env);
void set_bodyeffect(rinf_const rc, effect body);
void forall_aloc_mknin_effect(location loc, eff_kind kind, effect e);
void forall_aloc_in_effect_mknin_effect(location loc,
                                        eff_kind kind1, effect e1,
                                        eff_kind kind2, effect e2);
bool check_nin_aloc_effect(aloc al, effect e);
void check_nin(void);
void check_rinf_constraints(void);
int print_effect(printf_func pf, effect e);
bool eq_path_sig(int *p1, int *p2);
bool eq_store(store s1, store s2);
unsigned long hash_store(store s);
store make_store_var(location loc, const char *name);
const char *name_store(store s);
store make_store_filter(location loc, store s, effect e);
store make_store_ref(location loc, store s, aloc al);
store make_store_ref_effect(location loc, store s, effect e);
store make_store_ow(location loc, store s1, store s2, effect e);
store make_store_assign(location loc, const char *err_msg, store s, aloc al,
                        qtype qt, bool strong);
void mkleq_store(location loc, const char *err_msg, store left, store right);
void mkinst_store(location loc, const char *err_msg, store left, store right,
                  polarity p);
void mkeq_store(location loc, const char *err_msg, store s1, store s2);
store lub_store(location loc, const char *err_msg, store s1, store s2);
void unify_store(location loc, const char *err_msg, store s1, store s2);
void propagate_store_cell_forward(store s, aloc al);
void propagate_store_cell_backward(store s, aloc al);
void compute_lins(void);
int lin_from_store(store s, aloc al);
qtype qtype_from_store(store s, aloc al);
int print_store(printf_func pf, store s);
int print_store_cells(printf_func pf, pr_qual_fn pr_qual, store s,
                      bool print_alocs);
int print_store_raw(printf_func pf, store s);
void mk_confine_inf_const(drinfo dri);
void check_effect_constraints(void);
typedef enum {sev_err, sev_warn, sev_info} severity;
void init_qerror(void);
extern bool qerrors;
extern growbuf qual_error_detail;
extern growbuf qtype_error_detail;
int qual_ed_printf(const char *fmt, ...);
int qtype_ed_printf(const char *fmt, ...);
struct Rinfo {
  bool isglobal;
  effect flocs;
  effect flocs_alloc;
  qtype fqtype;
  qtype fs_fqtype;
  effect rlocs;
  effect rlocs_alloc;
  qtype rqtype;
  qtype fs_rqtype;
  effect arg1_eff;
  qual lin;
};
struct Drinfo {
  effect effect;
  expression expression;
  qtype qtype;
  qtype rqtype;
  qtype fs_qtype;
  qtype fs_rqtype;
  location location;
  effect env;
  compound_stmt cs;
  effect body_eff;
  bool failed;
  bool inference;
};
struct ExprDrinfoPair {
  expression e;
  drinfo drinfo;
};
void analyze(declaration program);
qtype analyze_field_declaration(const char *name, field_declaration fd);
void analyze_tag_ref(tag_ref);
void init_analyze(void);
void finish_analyze(void);
extern function_decl current_function_decl;
void report_qerror(location loc, severity sev, const char *format, ...);
location location_declaration(declaration);
data_declaration root_ddecl(data_declaration ddecl);
bool static_ddecl(data_declaration ddecl);
identifier_declarator get_id_declarator(declarator d);
bool is_undefined_global(data_declaration ddecl, store s);
bool is_unannotated_global(data_declaration ddecl, store s);
void warn_if_dangerous_global(data_declaration ddecl, store s);
typedef void (*traverse_global_fn)(data_declaration, void *);
void traverse_globals(traverse_global_fn f, void *arg);
void traverse_globals_sorted(traverse_global_fn f, void *arg);
effect get_global_effect(void);
data_declaration get_main_ddecl(void);
void measure_consts(void);
extern bool have_stream_quals;
extern qual open_unchecked_qual;
extern qual read_unchecked_qual;
extern qual write_unchecked_qual;
extern qual readwrite_unchecked_qual;
extern qual open_qual;
extern qual read_qual;
extern qual write_qual;
extern qual readwrite_qual;
extern qual closed_qual;
extern bool have_sleepy_quals;
extern qual enabled_qual;
extern qual disabled_qual;
extern qtype interrupt_status_qtype;
bool file_pointer_qtype(qtype qt);
         const char *combine_colors_pam(const char *color1, const char *color2);
typedef struct Gqtype* gqtype;
typedef struct Gqtype_dt* gqtype_dt;
typedef int (*pr_gqtype_qf_fn)(gqtype, printf_func, pr_qual_fn, store, bool);
typedef int (*pr_gqtype_fn)(gqtype, printf_func, store);
typedef const char* (*color_gqtype_fn)(gqtype, store);
struct Gqtype {
  gqtype_dt dt;
};
struct Gqtype_dt {
  pr_gqtype_qf_fn print_gqtype_qf;
  pr_gqtype_fn print_gqtype;
  color_gqtype_fn color_gqtype;
};
gqtype_dt mk_gqtype_dt(region,
                       pr_gqtype_qf_fn,
                       pr_gqtype_fn,
                       color_gqtype_fn);
void enter_pam_mode(void);
void pam_add_file(const char *name);
void pam_add_error(const char *func, location loc, severity s,
                   const char *fmt, va_list args);
void pam_add_overlay_file(location loc, const char *idname, gqtype qt);
void pam_add_overlay_anchor(location loc);
void pam_add_overlay_lin(location loc, store store, aloc al);
void pam_add_overlay_flow_sensitive(location loc, const char *idname,
                                    gqtype qt, store s);
void pam_add_overlay_confined_exp(location loc, expression e, gqtype qt,
                                  store s);
void print_results(void);
void init_pam(void);
typedef bool(*ddecl_pred)(void *addecl, store s);
typedef struct pred_info {
  ddecl_pred pred;
  const char *buffer;
  store store;
} *pred_info;
typedef struct pred_info2 {
  ddecl_pred pred;
  bool flag;
  store store;
} *pred_info2;
typedef void (*click_func)(const char *click, const char *file,
                           const char *name, void *arg);
typedef enum { lpos, rpos, apos } context;
typedef struct einfo {
  qtype qt;
  effect eff;
  bool ismalloc;
} einfo;
typedef struct sinfo {
  effect eff;
} sinfo;
typedef struct dinfo {
  effect eff;
  effect alocs;
  dd_list drinfolist;
} dinfo;
typedef struct operator
{
  const char *name;
  enum sig_kind { sig_deref, sig_addr, sig_unop, sig_binop, sig_assign } sig;
  qtype qt;
} *operator;
extern struct operator operators[];
inline operator find_op_name(const char *name);
void reset_operators(void);
bool match_signature(enum sig_kind k, qtype qt, location loc);
bool add_op_signature(operator op, qtype qt, location loc);
bool add_op_constraints(operator op, einfo ret, einfo arg1, einfo arg2,
                        location loc);
einfo do_assign(einfo arg1, einfo arg2, location loc, effect eff);
einfo do_cast(einfo arg1, type cast_t, location loc, context context);
einfo do_sizealign_of(const char *tag, location loc, context context);
einfo do_boolop(location, bool is_eq_or_neq, bool dna1, bool dna2,
                einfo arg1, einfo arg2, effect, operator);
sinfo do_change_type(einfo exp, type t, qtype *qt, location loc);
sinfo do_assert_type(einfo exp, type t, qtype *qt, location loc, location tloc);
einfo put_id_in_context(location, einfo, context);
bool init_string(location, qtype lhs_qtype, qtype rhs_qtype);
inline void mk_effect_leq_global_env(effect e);
inline void mk_effect_leq_global_effect(effect e);
extern void qtype_mklhs_nonconst(location, qtype);
extern bool mkNonConst_qual(location, qual);
extern bool mkNonConst_pointer(location, qtype);
extern bool mkNonConst_aggregate(location, qtype);
extern bool mkConst_pointer(location, qtype);
inline einfo mkeinfo(qtype qt, effect eff, bool ismalloc);
inline sinfo mksinfo(effect eff);
inline dinfo mkdinfo(effect eff, effect alocs);
bool file_pointer_qtype(qtype qt);
static einfo dummy_einfo = {qt: ((void *)0), eff: ((void *)0)};
static void init_magic(void);
static void add_declarator_overlay(declarator, qtype qt);
void analyze(declaration);
static dinfo analyze_declaration(declaration, effect env,
                                 dd_list drinfolist);
static dinfo analyze_operator_declaration(variable_decl);
void analyze_tag_ref(tag_ref);
static sinfo analyze_init(location loc, qtype lhs_qtype, expression rhs,
                          effect env, dd_list drinfolist);
static sinfo analyze_statement(statement, effect env, dd_list drinfolist);
static einfo analyze_expression(expression, context, effect env,
                                dd_list drinfolist);
static einfo analyze_unary_expression(unary, context, effect env,
                                      dd_list drinfolist);
static einfo analyze_binary_expression(binary, context, effect env,
                                       dd_list drinfolist);
static einfo analyze_magic_function_call(function_call, qtype, effect env,
                                         dd_list drinfolist);
static dinfo confine_expressions(compound_stmt cs, dd_list drinfolist, effect env);
static effect enforce_confine(compound_stmt cs, effect eff, qtype qt);
ast_kind assign_op_kind(binary);
bool restrict_variable_decl(variable_decl vd, bool isglobal);
effect enforce_restricts(declaration d, effect eff, qtype qt);
void enforce_var_decl_readonly(variable_decl vd, effect eff);
void enforce_readonly(declaration d, effect eff);
effect filter_effects(qtype qt, effect alocs);
void unparse_start(FILE *);
void prt_variable_decl(variable_decl);
void prt_expression(expression, int);
const char *binary_op_name(ast_kind);
bool is_void_parms(declaration);
bool equal_expressions(expression e1, expression e2);
extern hash_table globals;
extern effect global_effect;
extern effect global_env;
static compound_stmt cur_cs = ((void *)0);
static dd_list definitions = ((void *)0);
static enum {state_orig, state_init, state_finish} state = state_orig;
void init_analyze(void)
{
  globals = make_string_hash_table(parse_region, 100);
  cur_cs = ((void *)0);
  current_function_decl = ((void *)0);
  global_effect = effect_var("<global>");
  global_env = effect_var("<global-alocs>");
  definitions = dd_new_list(parse_region);
  if (restrict_qual && !global_effect)
    __fail("analyze.c", 112, __FUNCTION__, "Must load config file before init_analyze\n");
  reset_operators();
  state = state_init;
  init_magic();
}
void finish_analyze(void)
{
  if (flag_warn_dangerous_globals && !flag_pam_mode && !flag_flow_sensitive)
    traverse_globals_sorted((traverse_global_fn) warn_if_dangerous_global,
                            ((void *)0));
  check_effect_constraints();
  if (flag_restrict_inf)
    {
      printf("Checking restrict inference constraints...\n");
      check_rinf_constraints();
    }
  state = state_finish;
}
void check_init_quals(void) {
  dd_list_pos pos;
  if (!(init_qual && noninit_qual))
    return;
  for (pos = dd_first((definitions)); !(!(pos)->next); pos = ((pos)->next))
    {
      data_declaration root;
      bool may_be_init;
      bool declared_init;
      root = root_ddecl(((data_declaration)((pos)->data)));
      may_be_init = !leq_qual(noninit_qual, qual_qtype(root->qtype));
      declared_init = has_ub_qual(qual_qtype(root->qtype), init_qual);
      if (may_be_init && !declared_init && !root->isinline &&
          !root->addresstaken)
        report_qerror(location_declaration(root->ast), sev_info,
                      "Found undeclared %s %s\n",
                      qtype_function(root->qtype)
                      ? "__init function"
                      : "__initdata variable",
                      root->name);
    }
}
struct const_data {
  int numPos;
  int numOrig;
  int numInf;
};
void measure_consts_qtype(qtype qt, struct const_data *cd) {
  if (qtype_pointer(qt)) {
    cd->numPos++;
    if (!qtype_function(points_to_qtype(qt)))
      {
        if (has_lb_qual(qual_qtype(qt), const_qual))
          cd->numOrig++;
        if (!leq_qual(qual_qtype(qt), nonconst_qual))
          cd->numInf++;
      }
    measure_consts_qtype(points_to_qtype(qt), cd);
  }
  else if (qtype_function(qt)) {
    qtypelist_scanner args;
    qtype argqt;
    qtypelist_scan(arg_qtypes(qt), &args);
    while ((argqt = qtypelist_next(&args)))
      measure_consts_qtype(argqt, cd);
    measure_consts_qtype(return_qtype(qt), cd);
  }
}
void measure_consts(void) {
  dd_list_pos pos;
  struct const_data cd = { 0, 0, 0, };
  if (!const_qual || !nonconst_qual) {
    fprintf(stderr, "-fmeasure-consts specified but const/nonconst qualifiers undefined\n");
    return;
  }
  for (pos = dd_first((definitions)); !(!(pos)->next); pos = ((pos)->next))
    {
      data_declaration root;
      struct const_data old = cd;
      root = root_ddecl(((data_declaration)((pos)->data)));
      if (!qtype_function(root->qtype) || root->in_prelude)
        continue;
      measure_consts_qtype(root->qtype, &cd);
      printf("%s: %d/%d/%d\n", root->name, cd.numOrig-old.numOrig,
             cd.numInf-old.numInf, cd.numPos-old.numPos);
    }
  printf("Total: %d/%d/%d\n", cd.numOrig, cd.numInf, cd.numPos);
}
const char *context_tostring(context c)
{
  switch (c)
    {
    case lpos: return "lpos";
    case apos: return "&pos";
    case rpos: return "rpos";
    default: return "<context?>";
    }
}
data_declaration root_ddecl(data_declaration ddecl)
{
  ((void) ((ddecl) ? 0 : (__assert_fail ("ddecl", "analyze.c", 246, __PRETTY_FUNCTION__), 0)));
  while (ddecl->shadowed)
    ddecl = ddecl->shadowed;
  return ddecl;
}
bool static_ddecl(data_declaration ddecl)
{
  ddecl = root_ddecl(ddecl);
  return ((ddecl->kind == decl_variable && ddecl->vtype == variable_static) ||
          (ddecl->kind == decl_function && ddecl->ftype == function_static));
}
identifier_declarator get_id_declarator(declarator d)
{
  switch (d->kind)
    {
    case kind_function_declarator:
      {
        function_declarator fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)", "analyze.c", 267, __PRETTY_FUNCTION__), 0))); (function_declarator)(tEmPcast); });
        return get_id_declarator(fd->declarator);
      }
      break;
    case kind_pointer_declarator:
      {
        pointer_declarator pd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)", "analyze.c", 273, __PRETTY_FUNCTION__), 0))); (pointer_declarator)(tEmPcast); });
        return get_id_declarator(pd->declarator);
      }
      break;
    case kind_array_declarator:
      {
        array_declarator ad = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)", "analyze.c", 279, __PRETTY_FUNCTION__), 0))); (array_declarator)(tEmPcast); });
        return get_id_declarator(ad->declarator);
      }
      break;
    case kind_identifier_declarator:
      {
        identifier_declarator id = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier_declarator && (tEmPcast)->kind <= postkind_identifier_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier_declarator && (tEmPcast)->kind <= postkind_identifier_declarator)", "analyze.c", 285, __PRETTY_FUNCTION__), 0))); (identifier_declarator)(tEmPcast); });
        return id;
      }
      break;
    default:
      __fail("analyze.c", 290, __FUNCTION__, "(at %s:%ld) " "Unexpected declarator kind 0x%x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static void add_declarator_overlay(declarator d, qtype qt)
{
  identifier_declarator id = get_id_declarator(d);
  cstring name = id->cstring;
  pam_add_overlay_file(id->loc, name.data, (gqtype) qt);
}
location location_declarator(declarator d)
{
  identifier_declarator id = get_id_declarator(d);
  return id->loc;
}
location location_declaration(declaration d)
{
  switch(d->kind)
    {
    case kind_variable_decl:
      {
        variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "analyze.c", 320, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
        return location_declarator(vd->declarator);
      }
    case kind_oldidentifier_decl:
      {
        return d->loc;
      }
    case kind_function_decl:
      {
        function_decl fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)", "analyze.c", 329, __PRETTY_FUNCTION__), 0))); (function_decl)(tEmPcast); });
        return location_declarator(fd->declarator);
      }
    case kind_field_decl:
      {
        field_decl fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)", "analyze.c", 334, __PRETTY_FUNCTION__), 0))); (field_decl)(tEmPcast); });
        if (!fd->declarator)
          return fd->loc;
        else
          return location_declarator(fd->declarator);
      }
    case kind_enumerator:
      {
        enumerator e = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "analyze.c", 343, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); });
        return e->loc;
      }
    case kind_implicit_decl:
      {
        implicit_decl id = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_implicit_decl && (tEmPcast)->kind <= postkind_implicit_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_implicit_decl && (tEmPcast)->kind <= postkind_implicit_decl)", "analyze.c", 348, __PRETTY_FUNCTION__), 0))); (implicit_decl)(tEmPcast); });
        return id->loc;
      }
    case kind_error_decl:
      {
        return d->loc;
      }
    default:
      __fail("analyze.c", 356, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind 0x%x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static qtype cur_return_qtype(void)
{
  qtype fun_qtype, ret_qtype;
  ((void) ((current_function_decl) ? 0 : (__assert_fail ("current_function_decl", "analyze.c", 366, __PRETTY_FUNCTION__), 0)));
  fun_qtype = root_ddecl(current_function_decl->ddecl)->qtype;
  ret_qtype = return_qtype(fun_qtype);
  return ret_qtype;
}
static qual cur_function_qual(void)
{
  qtype fun_qtype;
  ((void) ((current_function_decl) ? 0 : (__assert_fail ("current_function_decl", "analyze.c", 376, __PRETTY_FUNCTION__), 0)));
  fun_qtype = root_ddecl(current_function_decl->ddecl)->qtype;
  return qual_qtype(fun_qtype);
}
ast_kind assign_op_kind(binary e)
{
  switch(e->kind)
    {
    case kind_plus_assign: return kind_plus;
    case kind_minus_assign: return kind_minus;
    case kind_times_assign: return kind_times;
    case kind_divide_assign: return kind_divide;
    case kind_modulo_assign: return kind_modulo;
    case kind_lshift_assign: return kind_lshift;
    case kind_rshift_assign: return kind_rshift;
    case kind_bitand_assign: return kind_bitand;
    case kind_bitor_assign: return kind_bitor;
    case kind_bitxor_assign: return kind_bitxor;
    default:
      __fail("analyze.c", 397, __FUNCTION__, "(at %s:%ld) " "Unexpected assign_op_kind kind 0x%x\n", ((e->loc)->filename), ((e->loc)->lineno), e->kind);
    }
}
bool file_pointer_qtype(qtype qt)
{
  if (qtype_pointer(qt))
    {
      qtype points_to;
      points_to = points_to_qtype(qt);
      return (qtype_struct(points_to) &&
              tag_name_qtype(points_to) &&
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (tag_name_qtype(points_to)) && __builtin_constant_p ("_IO_FILE") && (__s1_len = strlen (tag_name_qtype(points_to)), __s2_len = strlen ("_IO_FILE"), (!((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (tag_name_qtype(points_to)), (__const char *) ("_IO_FILE"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (tag_name_qtype(points_to)) && ((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) && (__s1_len = strlen (tag_name_qtype(points_to)), __s1_len < 4) ? (__builtin_constant_p ("_IO_FILE") && ((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - ((__const unsigned char *) (__const char *)("_IO_FILE"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("_IO_FILE"); register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("_IO_FILE") && ((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) && (__s2_len = strlen ("_IO_FILE"), __s2_len < 4) ? (__builtin_constant_p (tag_name_qtype(points_to)) && ((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - ((__const unsigned char *) (__const char *)("_IO_FILE"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (tag_name_qtype(points_to)); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; }))) : strcmp (tag_name_qtype(points_to), "_IO_FILE")))); }));
    }
  return 0;
}
static inline operator find_op_kind(ast_kind k)
{
  operator op;
  switch (k)
    {
    case kind_unary_minus: op = operators; break;
    case kind_unary_plus: op = operators+1; break;
    case kind_bitnot: op = operators+2; break;
    case kind_not: op = operators+3; break;
    case kind_times: op = operators+4; break;
    case kind_divide: op = operators+5; break;
    case kind_modulo: op = operators+6; break;
    case kind_lshift: op = operators+7; break;
    case kind_rshift: op = operators+8; break;
    case kind_lt: op = operators+9; break;
    case kind_gt: op = operators+10; break;
    case kind_leq: op = operators+11; break;
    case kind_geq: op = operators+12; break;
    case kind_eq: op = operators+13; break;
    case kind_ne: op = operators+14; break;
    case kind_bitand: op = operators+15; break;
    case kind_bitor: op = operators+16; break;
    case kind_bitxor: op = operators+17; break;
    case kind_andand: op = operators+18; break;
    case kind_oror: op = operators+19; break;
    case kind_dereference: op = operators+20; break;
    default: op = ((void *)0);
    }
  if (op && op->qt)
    return op;
  else
    return ((void *)0);
}
bool add_ddecl_qtype(data_declaration ddecl, bool generalize)
{
  ((void) ((ddecl->kind != decl_typedef) ? 0 : (__assert_fail ("ddecl->kind != decl_typedef", "analyze.c", 477, __PRETTY_FUNCTION__), 0)));
  if (!ddecl->qtype)
    {
      type t, lifted_t;
      location loc;
      bool preferred;
      bool isglobal;
      data_declaration root;
      t = ddecl->type;
      if (type_function(t) || type_array(t)|| ddecl->kind == decl_constant)
        lifted_t = t;
      else
        lifted_t = make_pointer_type(t);
      if (ddecl->definition)
        {
          loc = location_declaration(ddecl->definition);
          preferred = 1;
        }
      else
        {
          loc = location_declaration(ddecl->ast);
          preferred = 0;
        }
      root = root_ddecl(ddecl);
      isglobal = (root->kind == decl_function ||
                  root->isexternalscope ||
                  root->vtype == variable_static);
      ddecl->qtype = decl_to_qtype(lifted_t,
                                   loc,
                                   ddecl->name,
                                   preferred,
                                   isglobal,
                                   generalize,
                                   ddecl->noreturn,
                                   ddecl->__init);
      return 1;
    }
  else
    return 0;
}
void add_global_ddecl_qtype(data_declaration ddecl, bool generalize)
{
  data_declaration old_ddecl;
  location loc;
  ddecl = root_ddecl(ddecl);
  loc = location_declaration(ddecl->ast);
  ((void) ((ddecl->isexternalscope) ? 0 : (__assert_fail ("ddecl->isexternalscope", "analyze.c", 544, __PRETTY_FUNCTION__), 0)));
  if (ddecl->qtype)
    return;
  if (hash_table_lookup(globals, (hash_key) ddecl->name,
                        (hash_data *) &old_ddecl))
    {
      if (old_ddecl->in_prelude && !ddecl->in_prelude)
        {
          ddecl->qtype = old_ddecl->qtype;
          ddecl->global = old_ddecl;
          return;
        }
      if (match_type(ddecl->type, old_ddecl->type))
        ddecl->qtype = old_ddecl->qtype;
      else
        {
          ((void) ((add_ddecl_qtype(ddecl, generalize)) ? 0 : (__assert_fail ("add_ddecl_qtype(ddecl, generalize)", "analyze.c", 573, __PRETTY_FUNCTION__), 0)));
          if (qtype_scheme(old_ddecl->qtype) || qtype_scheme(ddecl->qtype))
            {
              report_qerror(loc, sev_warn,
                            "multiple polymorphic types for ``%s''"
                            " -- not checking for consistency", ddecl->name);
            }
          else if (unify_qtype(loc, old_ddecl->qtype, ddecl->qtype))
            {
              report_qerror(loc, sev_err, "conflicting types for ``%s''",
                            ddecl->name);
              report_qerror(old_ddecl->ast->loc, sev_info,
                            "-- previous declaration of ``%s''", ddecl->name);
            }
        }
      if (qtype_function(ddecl->qtype) && !qtype_oldstyle(ddecl->qtype) &&
          qtype_function(old_ddecl->qtype) && qtype_oldstyle(old_ddecl->qtype))
        {
          ((void) ((!(hash_table_insert(globals, (hash_key) ddecl->name, ddecl))) ? 0 : (__assert_fail ("!(hash_table_insert(globals, (hash_key) ddecl->name, ddecl))", "analyze.c", 595, __PRETTY_FUNCTION__), 0)));
          old_ddecl->global = ddecl;
          old_ddecl = ddecl;
        }
      old_ddecl->defined = ddecl->defined =
        (old_ddecl->defined || ddecl->defined);
      old_ddecl->isused = ddecl->isused =
        (old_ddecl->isused || ddecl->isused);
      old_ddecl->addresstaken = ddecl->addresstaken =
        (old_ddecl->addresstaken || ddecl->addresstaken);
      ddecl->global = old_ddecl;
    }
  else
    {
      ((void) ((add_ddecl_qtype(ddecl, generalize)) ? 0 : (__assert_fail ("add_ddecl_qtype(ddecl, generalize)", "analyze.c", 622, __PRETTY_FUNCTION__), 0)));
      if (ddecl->in_prelude && !ddecl->defined)
        ddecl->defined = 1;
      ((void) ((hash_table_insert(globals, (hash_key) ddecl->name, ddecl)) ? 0 : (__assert_fail ("hash_table_insert(globals, (hash_key) ddecl->name, ddecl)", "analyze.c", 625, __PRETTY_FUNCTION__), 0)));
    }
}
void add_static_ddecl_qtype(data_declaration ddecl, bool generalize)
{
  data_declaration old_ddecl;
  ddecl = root_ddecl(ddecl);
  ((void) ((static_ddecl(ddecl)) ? 0 : (__assert_fail ("static_ddecl(ddecl)", "analyze.c", 637, __PRETTY_FUNCTION__), 0)));
  if (ddecl->qtype)
    return;
  if (hash_table_lookup(globals, (hash_key) ddecl->name,
                        (hash_data *) &old_ddecl))
    {
      if (old_ddecl->in_prelude && static_ddecl(old_ddecl) &&
          !ddecl->in_prelude)
        {
          ddecl->qtype = old_ddecl->qtype;
          ddecl->global = old_ddecl;
          return;
        }
      if (ddecl->in_prelude)
        report_qerror(location_declaration(ddecl->ast), sev_err,
                      "ignoring static prelude declaration -- conflict with "
                      "non-static prelude declaration");
    }
  else if (ddecl->in_prelude)
    {
      add_ddecl_qtype(ddecl, generalize);
      ((void) ((ddecl->qtype) ? 0 : (__assert_fail ("ddecl->qtype", "analyze.c", 659, __PRETTY_FUNCTION__), 0)));
      ddecl->defined = 1;
      ((void) ((hash_table_insert(globals, (hash_key) ddecl->name, ddecl)) ? 0 : (__assert_fail ("hash_table_insert(globals, (hash_key) ddecl->name, ddecl)", "analyze.c", 661, __PRETTY_FUNCTION__), 0)));
    }
  add_ddecl_qtype(ddecl, generalize);
}
void __add_ddecl_qtype(data_declaration ddecl, bool generalize)
{
  ddecl = root_ddecl(ddecl);
  if (ddecl->qtype)
    return;
  if (ddecl->isexternalscope)
    add_global_ddecl_qtype(ddecl, generalize);
  else if (static_ddecl(ddecl))
    add_static_ddecl_qtype(ddecl, generalize);
  else
    add_ddecl_qtype(ddecl, generalize);
}
void mark_used(data_declaration ddecl)
{
  ddecl = root_ddecl(ddecl);
  ddecl->isused = 1;
  if (ddecl->isexternalscope && ddecl->global)
    ddecl->global->isused = 1;
}
void mark_address_taken(data_declaration ddecl)
{
  ddecl = root_ddecl(ddecl);
  ddecl->addresstaken = 1;
  if (ddecl->isexternalscope && ddecl->global)
    ddecl->global->addresstaken = 1;
}
bool is_undefined_global(data_declaration ddecl, store s)
{
  ddecl = root_ddecl(ddecl);
  return (ddecl->isused && !ddecl->defined &&
          (nontriv_soln_qtype(ddecl->qtype, ((void *)0)) ||
           flag_flow_sensitive));
}
bool is_unannotated_global(data_declaration ddecl, store s)
{
  ddecl = root_ddecl(ddecl);
  return (!is_undefined_global(ddecl, s) &&
          ddecl->isused &&
          qtype_function(ddecl->qtype) &&
          qtype_varargs(ddecl->qtype) &&
          !ddecl->in_prelude);
}
void warn_if_dangerous_global(data_declaration ddecl, store s)
{
  if (is_undefined_global(ddecl, s))
 report_qerror(location_declaration(ddecl->ast), sev_err,
          "``%s'' used but not defined", ddecl->name);
  if (is_unannotated_global(ddecl, s))
    report_qerror(location_declaration(ddecl->ast), sev_info,
          "vararg function ``%s'' does not have polymorphic type",
          ddecl->name);
}
effect get_global_effect(void)
{
  ((void) ((state == state_finish) ? 0 : (__assert_fail ("state == state_finish", "analyze.c", 760, __PRETTY_FUNCTION__), 0)));
  return global_effect;
}
data_declaration get_main_ddecl(void)
{
  data_declaration main_ddecl;
  ((void) ((state == state_finish) ? 0 : (__assert_fail ("state == state_finish", "analyze.c", 769, __PRETTY_FUNCTION__), 0)));
  if (hash_table_lookup(globals, (hash_key) "main", (hash_data *) &main_ddecl))
    return root_ddecl(main_ddecl);
  else
    return ((void *)0);
}
void analyze(declaration program)
{
  declaration d;
  dinfo next_info;
  dd_list global_drinfolist;
  global_drinfolist = dd_new_list(parse_region);
  interrupt_status_qtype = ((void *)0);
  unparse_start(stdout);
  pam_add_file(program->loc->filename);
  AST_set_parents(({ast_generic tEmPcast = (ast_generic)(program); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "analyze.c", 799, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }));
  for (d = (program); d; d = ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 800, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
    {
      next_info = analyze_declaration(d, global_env, global_drinfolist);
      mk_effect_leq_global_effect(next_info.eff);
      mk_effect_leq_global_env(next_info.alocs);
    }
}
static dinfo analyze_declaration(declaration d, effect env, dd_list drinfolist)
{
  switch (d->kind)
    {
    case kind_asm_decl:
      {
        return mkdinfo(effect_empty, effect_empty);
      };
    case kind_data_decl:
      {
        data_decl dd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "analyze.c", 825, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
        declaration decl;
        effect eff, local_env, alocs;
        eff = effect_empty;
        local_env = env;
        alocs = effect_empty;
        for (decl = (dd->decls); decl; decl = ({ast_generic tEmPcast = (ast_generic)(decl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 833, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })) {
          dinfo next;
          next = analyze_declaration(decl, local_env, drinfolist);
          local_env = effect_union(local_env, next.alocs);
          alocs = effect_union(alocs, next.alocs);
          eff = effect_union(eff, next.eff);
        }
        if (!dd->decls && dd->modifiers && ((dd->modifiers)->kind >= kind_tag_ref && (dd->modifiers)->kind <= postkind_tag_ref))
          analyze_tag_ref(({ast_generic tEmPcast = (ast_generic)(dd->modifiers); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)", "analyze.c", 845, __PRETTY_FUNCTION__), 0))); (tag_ref)(tEmPcast); }));
        return mkdinfo(eff, alocs);
      };
    case kind_variable_decl:
      {
        variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "analyze.c", 851, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
        data_declaration root;
        effect eff, alocs;
        bool isglobal;
        root = root_ddecl(vd->ddecl);
        if ((!(__extension__ (__builtin_constant_p (4) && ((__builtin_constant_p ("_op_") && strlen ("_op_") < ((size_t) (4))) || (__builtin_constant_p (root->name) && strlen (root->name) < ((size_t) (4)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("_op_") && __builtin_constant_p (root->name) && (__s1_len = strlen ("_op_"), __s2_len = strlen (root->name), (!((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("_op_"), (__const char *) (root->name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) && (__s1_len = strlen ("_op_"), __s1_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (root->name); register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s2_len = strlen (root->name), __s2_len < 4) ? (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("_op_"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (root->name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; }))) : strcmp ("_op_", root->name)))); }) : strncmp ("_op_", root->name, 4))) && strlen(root->name) >= 4))
          return analyze_operator_declaration(vd);
        if (root->kind == decl_typedef)
          {
            if (type_enum(root->type))
              analyze_tag_ref(type_tag(root->type)->ast);
            return mkdinfo(effect_empty, effect_empty);
          }
        __add_ddecl_qtype(root, 1);
        eff = effect_empty;
        alocs = effect_empty;
        isglobal = (root->kind == decl_function ||
                    root->isexternalscope ||
                    root->vtype == variable_static);
        if (root->qtype)
          {
            if (isglobal)
              {
                mk_effect_leq_global_env(alocs_qtype(root->qtype));
                mk_effect_leq_global_effect(defn_effect_qtype(root->qtype));
              }
            else
              {
                alocs = effect_union(alocs_qtype(root->qtype), alocs);
                eff = effect_union(defn_effect_qtype(root->qtype), eff);
              }
          }
        if (restrict_variable_decl(vd, isglobal))
          {
            if (isglobal)
              {
                mknin_effect(vd->loc, vd->rinfo->flocs, global_effect);
                if (flag_flow_sensitive)
                  mk_effect_leq_global_effect(vd->rinfo->flocs_alloc);
              }
            else
              {
                mknin_effect(vd->loc, vd->rinfo->rlocs, env);
                alocs = effect_union(vd->rinfo->rlocs, alocs);
                if (flag_flow_sensitive)
                  eff = effect_union(vd->rinfo->rlocs_alloc, eff);
              }
          }
        if (isglobal)
          enforce_var_decl_readonly(vd, global_effect);
        if (vd->arg1)
          {
            qtype init_qt;
            sinfo arg1;
            if (vd->rinfo && vd->rinfo->isglobal)
              init_qt = vd->rinfo->fqtype;
            else
              init_qt = root->qtype;
            if (!qtype_array(init_qt))
              init_qt = points_to_qtype(init_qt);
            arg1 = analyze_init(vd->loc, init_qt, vd->arg1, env, drinfolist);
            vd->arg1_eff = arg1.eff;
            if (isglobal)
              eff = effect_union(eff, arg1.eff);
          }
        if (vd->rinfo)
          root->qtype = vd->rinfo->rqtype;
        if (root->alias != ((void *)0))
          {
            data_declaration alias_ddecl;
            if (hash_table_lookup(globals, (hash_key) root->alias, (hash_data *) &alias_ddecl))
              {
                alias_ddecl = root_ddecl(alias_ddecl);
                unify_qtype(vd->loc, alias_ddecl->qtype, root->qtype);
              }
          }
        if (have_sleepy_quals &&
            !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (root->name) && __builtin_constant_p ("fake_interrupt_status") && (__s1_len = strlen (root->name), __s2_len = strlen ("fake_interrupt_status"), (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fake_interrupt_status") + 1) - (size_t)(const void *)("fake_interrupt_status") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (root->name), (__const char *) ("fake_interrupt_status"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s1_len = strlen (root->name), __s1_len < 4) ? (__builtin_constant_p ("fake_interrupt_status") && ((size_t)(const void *)(("fake_interrupt_status") + 1) - (size_t)(const void *)("fake_interrupt_status") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (root->name))[0] - ((__const unsigned char *) (__const char *)("fake_interrupt_status"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[1] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[2] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (root->name))[3] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fake_interrupt_status"); register int __result = (((__const unsigned char *) (__const char *) (root->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (root->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fake_interrupt_status") && ((size_t)(const void *)(("fake_interrupt_status") + 1) - (size_t)(const void *)("fake_interrupt_status") == 1) && (__s2_len = strlen ("fake_interrupt_status"), __s2_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (root->name))[0] - ((__const unsigned char *) (__const char *)("fake_interrupt_status"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[1] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (root->name))[2] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (root->name))[3] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (root->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fake_interrupt_status"))[3]); } } __result; }))) : strcmp (root->name, "fake_interrupt_status")))); }))
          {
            if (interrupt_status_qtype != ((void *)0))
              ((void) ((interrupt_status_qtype == root->qtype) ? 0 : (__assert_fail ("interrupt_status_qtype == root->qtype", "analyze.c", 977, __PRETTY_FUNCTION__), 0)));
            else
              interrupt_status_qtype = root->qtype;
          }
        if ((flag_pam_mode || flag_print_results) &&
            root->kind != decl_typedef)
          add_declarator_overlay(vd->declarator, root->qtype);
        return mkdinfo(eff, alocs);
      }
      break;
    case kind_function_decl:
      {
        function_decl fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)", "analyze.c", 997, __PRETTY_FUNCTION__), 0))); (function_decl)(tEmPcast); }), old_function_decl;
        data_declaration root;
        qtype ret_qtype;
        effect param_rtype_alocs;
        effect alocs, local_env, eff;
        int cur_body = 0;
        effect first_bodyeff = ((void *)0),bodyeff = ((void *)0);
        dd_list bodyeffs = ((void *)0);
        region scratch_region = ((void *)0);
        rinf_const rc = ((void *)0);
        root = root_ddecl(fd->ddecl);
        dd_add_last(parse_region, definitions, root);
        if (flag_restrict_inf)
          {
            scratch_region = newregion();
            bodyeffs = dd_new_list(scratch_region);
          }
        __add_ddecl_qtype(root, 1);
        if (flag_pam_mode || flag_print_results)
          add_declarator_overlay(fd->declarator, root->qtype);
        if (root->global && root->global->in_prelude)
          return mkdinfo(effect_empty, effect_empty);
        if (qtype_scheme(root->qtype))
          {
            report_qerror(fd->loc, sev_warn,
                          "Ignoring definition of polymorphic function");
            return mkdinfo(effect_empty, effect_empty);
          }
        if (!root->__init && !(root->ftype == function_static) && noninit_qual)
          if (mkleq_qual(fd->loc, &open_qgate, qual_qtype(root->qtype),
                         noninit_qual))
            report_qerror(fd->loc, sev_err,
                          "inconsistent __init declaration");
        alocs = alocs_qtype(root->qtype);
        old_function_decl = current_function_decl;
        current_function_decl = fd;
        ret_qtype = return_qtype(root->qtype);
        mk_effect_leq_global_effect(effect_qtype(root->qtype));
        mk_effect_leq_global_effect(defn_effect_qtype(root->qtype));
        local_env = env;
        eff = effect_empty;
        param_rtype_alocs = effect_empty;
        {
          declaration arg;
          qtypelist_scanner old_qtypes;
          int i;
          i = 1;
          qtypelist_scan(arg_qtypes(root->qtype), &old_qtypes);
          if (!is_void_parms(fd->fdeclarator->parms))
            for (arg = (fd->fdeclarator->parms); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 1068, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
              if (!((arg)->kind >= kind_ellipsis_decl && (arg)->kind <= postkind_ellipsis_decl))
                {
                  data_declaration root_vd;
                  variable_decl argvd;
                  oldidentifier_decl oid;
                  qtype old_qtype, arg_qt;
                  if (arg->kind == kind_data_decl)
                    {
                      data_decl argd = ({ast_generic tEmPcast = (ast_generic)(arg); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "analyze.c", 1080, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
                      argvd = ({ast_generic tEmPcast = (ast_generic)(argd->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "analyze.c", 1082, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
                      ((void) ((!argvd->next) ? 0 : (__assert_fail ("!argvd->next", "analyze.c", 1083, __PRETTY_FUNCTION__), 0)));
                      oid = ((void *)0);
                      root_vd = root_ddecl(argvd->ddecl);
                    }
                  else
                    {
                      oid = ({ast_generic tEmPcast = (ast_generic)(arg); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)", "analyze.c", 1091, __PRETTY_FUNCTION__), 0))); (oldidentifier_decl)(tEmPcast); });
                      argvd = ((void *)0);
                      root_vd = root_ddecl(oid->ddecl);
                    }
                  __add_ddecl_qtype(root_vd, 0);
                  ((void) ((root_vd->qtype) ? 0 : (__assert_fail ("root_vd->qtype", "analyze.c", 1097, __PRETTY_FUNCTION__), 0)));
                  old_qtype = qtypelist_next(&old_qtypes);
                  arg_qt = points_to_qtype(root_vd->qtype);
                  if (old_qtype && unify_qtype(arg->loc, old_qtype, arg_qt))
                    report_qerror(arg->loc, sev_err, "type of parameter %d doesn't match previous uses", i);
                  else if (!old_qtype && arg->kind == kind_data_decl)
                    report_qerror(arg->loc, sev_err, "parameter %d doesn't appear in previous uses", i);
                  if (old_qtype)
                    param_rtype_alocs = effect_union(param_rtype_alocs,
                                                     alocs_qtype(old_qtype));
                  eff = effect_union(eff, defn_effect_qtype(root_vd->qtype));
                  local_env = effect_union(local_env,
                                           alocs_qtype(root_vd->qtype));
                  if (arg->kind == kind_data_decl &&
                      restrict_variable_decl(argvd, 0))
                    {
                      mknin_effect(argvd->loc, argvd->rinfo->rlocs, local_env);
                      local_env = effect_union(argvd->rinfo->rlocs, local_env);
                      if (flag_flow_sensitive)
                        eff = effect_union(eff, argvd->rinfo->rlocs_alloc);
                      root_vd->qtype = argvd->rinfo->rqtype;
                    }
                  else if (flag_restrict_inf &&
                           arg->kind == kind_data_decl &&
                           type_pointer(root_vd->type))
                    {
                      qtype new_rqtype;
                      aloc top_al,r_al,old_al;
                      location loc;
                      old_al = aloc_qtype(arg_qt);
                      r_al = make_aloc(rstrcat(parse_region,
                                               name_aloc(old_al),"_r"),
                                       points_to_qtype(arg_qt),0);
                      loc = argvd->loc;
                      new_rqtype =
                        mkqtype_pointer(loc,
                                        qual_qtype(arg_qt),
                                        points_to_qtype(arg_qt),
                                        r_al);
                      top_al =
                        make_aloc(rstrcat(parse_region,
                               name_aloc(aloc_qtype(root_vd->qtype)),"_r"),
                                  new_rqtype,0);
                      root_vd ->qtype =
                        mkqtype_pointer(loc,
                                        qual_qtype(root_vd->qtype),
                                        new_rqtype,
                                        top_al);
                      bodyeff =
                        effect_var(rstrcat(parse_region,root->name,
                                           rstrcat(parse_region,
                                                   "body_eff",
                                                   inttostr(parse_region,
                                                            cur_body++))));
                      if (rc)
                        set_bodyeffect(rc,bodyeff);
                      if (((!(dd_first((bodyeffs)))->next)))
                        first_bodyeff = bodyeff;
                      dd_add_last(scratch_region,bodyeffs,bodyeff);
                      rc = mkrinf_constraint(loc,r_al,old_al,top_al,
                                              alocs_qtype(ret_qtype),
                                              alocs_qtype(points_to_qtype(arg_qt)),
                                              bodyeff, local_env);
                      local_env = effect_union(local_env,
                                           alocs_qtype(root_vd->qtype));
                    }
                  if (flag_pam_mode || flag_print_results)
                    {
                      if (arg->kind == kind_data_decl)
                        add_declarator_overlay(argvd->declarator,
                                               root_vd->qtype);
                      else
                        pam_add_overlay_file(oid->loc,
                                             oid->cstring.data,
                                            (gqtype) root_vd->qtype);
                    }
                  i++;
                }
          if (qtypelist_next(&old_qtypes))
            report_qerror(fd->loc, sev_err,
                          "inconsistent number of arguments for %s",
                          root->name);
        }
        if (flag_restrict_inf && rc)
          {
            dd_list_pos cur;
            effect prev = ((void *)0);
            bodyeff =
              effect_var(rstrcat(parse_region,root->name,
                                 rstrcat(parse_region,"body_eff",
                                         inttostr(parse_region,cur_body++))));
            set_bodyeffect(rc,bodyeff);
            dd_add_last(scratch_region,bodyeffs,bodyeff);
            for (cur = dd_first((bodyeffs)); !(!(cur)->next); cur = ((cur)->next))
              {
                effect e = ((effect)((cur)->data));
                if (prev)
                  mkleq_effect(prev,e);
                prev = e;
              }
          }
        {
          sinfo body;
          ((void) ((((fd->stmt)->kind >= kind_compound_stmt && (fd->stmt)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((fd->stmt)->kind >= kind_compound_stmt && (fd->stmt)->kind <= postkind_compound_stmt)", "analyze.c", 1244, __PRETTY_FUNCTION__), 0)));
          body = analyze_statement(fd->stmt, local_env, drinfolist);
          eff = effect_union(eff,
                             enforce_restricts(fd->fdeclarator->parms,
                                               body.eff,
                                               ret_qtype));
          if (flag_restrict_inf && rc)
            {
              ((void) ((first_bodyeff) ? 0 : (__assert_fail ("first_bodyeff", "analyze.c", 1255, __PRETTY_FUNCTION__), 0)));
              mkleq_effect(eff,first_bodyeff);
              eff = bodyeff;
            }
          if (flag_restrict_inf)
            deleteregion(scratch_region);
          enforce_readonly(fd->fdeclarator->parms, eff);
        }
        fd->scope_env = effect_union(param_rtype_alocs,
                                     filter_effects(((void *)0), env));
        mark_effect_interesting(fd->scope_env);
        eff = effect_inter(eff, fd->scope_env);
        mkleq_effect(eff, effect_qtype(root->qtype));
        current_function_decl = old_function_decl;
        return mkdinfo(effect_empty, alocs);
      };
    case kind_extension_decl:
      {
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "analyze.c", 1281, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        return analyze_declaration(ed->decl, env, drinfolist);
      };
    default:
      __fail("analyze.c", 1286, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind 0x%x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static dinfo analyze_operator_declaration(variable_decl vd)
{
  data_declaration root;
  operator op;
  root = root_ddecl(vd->ddecl);
  ((void) (((!(__extension__ (__builtin_constant_p (4) && ((__builtin_constant_p ("_op_") && strlen ("_op_") < ((size_t) (4))) || (__builtin_constant_p (root->name) && strlen (root->name) < ((size_t) (4)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("_op_") && __builtin_constant_p (root->name) && (__s1_len = strlen ("_op_"), __s2_len = strlen (root->name), (!((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("_op_"), (__const char *) (root->name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) && (__s1_len = strlen ("_op_"), __s1_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (root->name); register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s2_len = strlen (root->name), __s2_len < 4) ? (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("_op_"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (root->name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; }))) : strcmp ("_op_", root->name)))); }) : strncmp ("_op_", root->name, 4))) && strlen(root->name) >= 4)) ? 0 : (__assert_fail ("(!(__extension__ (__builtin_constant_p (4) && ((__builtin_constant_p (\"_op_\") && strlen (\"_op_\") < ((size_t) (4))) || (__builtin_constant_p (root->name) && strlen (root->name) < ((size_t) (4)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (\"_op_\") && __builtin_constant_p (root->name) && (__s1_len = strlen (\"_op_\"), __s2_len = strlen (root->name), (!((size_t)(const void *)((\"_op_\") + 1) - (size_t)(const void *)(\"_op_\") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (\"_op_\"), (__const char *) (root->name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (\"_op_\") && ((size_t)(const void *)((\"_op_\") + 1) - (size_t)(const void *)(\"_op_\") == 1) && (__s1_len = strlen (\"_op_\"), __s1_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (root->name); register int __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s2_len = strlen (root->name), __s2_len < 4) ? (__builtin_constant_p (\"_op_\") && ((size_t)(const void *)((\"_op_\") + 1) - (size_t)(const void *)(\"_op_\") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (\"_op_\"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (\"_op_\"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (root->name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; }))) : strcmp (\"_op_\", root->name)))); }) : strncmp (\"_op_\", root->name, 4))) && strlen(root->name) >= 4)", "analyze.c", 1297, __PRETTY_FUNCTION__), 0)));
  if (!root->isexternalscope)
    {
      report_qerror(vd->loc, sev_err,
                    "Operator not declared at top level");
      return mkdinfo(effect_empty, effect_empty);
    }
  add_global_ddecl_qtype(root, 1);
  op = find_op_name(root->name);
  if (op && op->qt)
    report_qerror(vd->loc, sev_err,
                  "redefinition of operator signature");
  else if (op)
    {
      if (add_op_signature(op, root->qtype, vd->loc))
        report_qerror(vd->loc, sev_err,
                      "definition of operator with incorrect signature");
      root->qtype = op->qt;
    }
  else
    report_qerror(vd->loc, sev_err,
                  "definition of non-existent operator signature");
  if ((flag_pam_mode || flag_print_results) &&
      root->kind != decl_typedef)
    add_declarator_overlay(vd->declarator, root->qtype);
  return mkdinfo(effect_empty, effect_empty);
}
qtype analyze_field_declaration(const char *name, field_declaration fd)
{
  qtype result;
  result = get_fdecl_qtype(name, fd, fd->type, location_declaration(({ast_generic tEmPcast = (ast_generic)(fd->ast); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 1338, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })));
  if ((flag_pam_mode || flag_print_results) && fd->name)
    add_declarator_overlay(fd->ast->declarator, result);
  mk_effect_leq_global_effect(defn_effect_qtype(result));
  mk_effect_leq_global_env(effect_single(aloc_qtype(result)));
  return result;
}
void analyze_tag_ref(tag_ref tr)
{
  if (((tr)->kind >= kind_struct_ref && (tr)->kind <= postkind_struct_ref) || ((tr)->kind >= kind_union_ref && (tr)->kind <= postkind_union_ref))
    ;
  else
    {
      declaration d;
      for (d = (tr->fields); d; d = ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 1361, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
        {
          enumerator e = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "analyze.c", 1363, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); });
          data_declaration root;
          root = root_ddecl(e->ddecl);
          if (!root->qtype)
            {
              __add_ddecl_qtype(root, 0);
              if (flag_pam_mode || flag_print_results)
                pam_add_overlay_file(e->loc,
                                     e->cstring.data,
                                     (gqtype) root->qtype);
            }
        }
    }
}
static sinfo analyze_init(location loc, qtype lhs_qtype, expression rhs,
                          effect env, dd_list drinfolist)
{
  if (rhs->kind == kind_string && (qtype_string(lhs_qtype)))
    {
      einfo arg1;
      ((void) ((qtype_string(lhs_qtype)) ? 0 : (__assert_fail ("qtype_string(lhs_qtype)", "analyze.c", 1391, __PRETTY_FUNCTION__), 0)));
      arg1 = analyze_expression(rhs, rpos, env, drinfolist);
      if (init_string(rhs->loc, lhs_qtype, arg1.qt))
        report_qerror(rhs->loc, sev_err, "incompatible initializer");
      return mksinfo(arg1.eff);
    }
  else if (rhs->kind == kind_init_list && (qtype_array(lhs_qtype)))
    {
      init_list il = ({ast_generic tEmPcast = (ast_generic)(rhs); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)", "analyze.c", 1403, __PRETTY_FUNCTION__), 0))); (init_list)(tEmPcast); });
      qtype lhs_contents_qtype;
      expression e;
      sinfo init;
      effect eff;
      ((void) ((qtype_array(lhs_qtype)) ? 0 : (__assert_fail ("qtype_array(lhs_qtype)", "analyze.c", 1409, __PRETTY_FUNCTION__), 0)));
      lhs_contents_qtype = array_of_qtype(lhs_qtype);
      eff = effect_empty;
      for (e = (il->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 1414, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        if (e->kind == kind_init_index)
          {
            init_index ii = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_index && (tEmPcast)->kind <= postkind_init_index)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_index && (tEmPcast)->kind <= postkind_init_index)", "analyze.c", 1419, __PRETTY_FUNCTION__), 0))); (init_index)(tEmPcast); });
            einfo arg1, arg2;
            arg1 = analyze_expression(ii->arg1, rpos, env, drinfolist);
            eff = effect_union(eff, arg1.eff);
            if (ii->arg2)
              {
                arg2 = analyze_expression(ii->arg2, rpos, env, drinfolist);
                eff = effect_union(eff, arg2.eff);
              }
            init = analyze_init(loc, lhs_contents_qtype, ii->init_expr, env,
                                drinfolist);
            eff = effect_union(eff, init.eff);
          }
        else
          {
            init = analyze_init(loc, lhs_contents_qtype, e, env, drinfolist);
            eff = effect_union(eff, init.eff);
          }
      return mksinfo(eff);
    }
  else if (rhs->kind == kind_init_list && (qtype_struct(lhs_qtype) ||
                                           qtype_union(lhs_qtype)))
    {
      init_list il = ({ast_generic tEmPcast = (ast_generic)(rhs); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)", "analyze.c", 1444, __PRETTY_FUNCTION__), 0))); (init_list)(tEmPcast); });
      field_scanner fs;
      qtype fqt;
      expression e;
      effect eff;
      fqt = ((void *)0);
      field_scan(lhs_qtype, &fs);
      eff = effect_empty;
      for (e = (il->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 1454, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        {
          expression init_expr;
          sinfo init;
          if (e->kind == kind_init_field)
            {
              init_field ifd = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_field && (tEmPcast)->kind <= postkind_init_field)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_field && (tEmPcast)->kind <= postkind_init_field)", "analyze.c", 1465, __PRETTY_FUNCTION__), 0))); (init_field)(tEmPcast); });
              field_scan_at(lhs_qtype, ifd->word1->cstring, &fs);
              if (!fs)
                {
                  report_qerror(loc, sev_err,
                                "unknown field `%s' in aggregate initialization",
                                ifd->word1->cstring);
                  break;
                }
              init_expr = ifd->init_expr;
            }
          else
            init_expr = e;
          fqt = field_next(&fs);
          if (!fqt)
            {
              report_qerror(loc, sev_err,
                            "aggregate initailized with wrong number of fields");
              break;
            }
          if (!qtype_array(fqt))
            fqt = points_to_qtype(fqt);
          init = analyze_init(loc, fqt, init_expr, env, drinfolist);
          eff = effect_union(eff, init.eff);
        }
      return mksinfo(eff);
    }
  else
    {
      einfo arg1;
      arg1 = analyze_expression(rhs, rpos, env, drinfolist);
      if (mkleq_assign_convert(loc, &fi_qgate, arg1.qt, lhs_qtype))
        report_qerror(loc, sev_err, "incompatible right-hand side in initialization");
      return mksinfo(arg1.eff);
    }
}
static sinfo analyze_statement(statement s, effect env, dd_list drinfolist)
{
  switch (s->kind)
    {
    case kind_asm_stmt:
      return mksinfo(effect_empty);
      break;
    case kind_compound_stmt:
      {
        compound_stmt cs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)", "analyze.c", 1525, __PRETTY_FUNCTION__), 0))); (compound_stmt)(tEmPcast); });
        declaration decl;
        statement stmt;
        effect eff, local_env;
        dd_list new_drinfolist;
        new_drinfolist = dd_copy(parse_region, drinfolist);
        cs->enclosing_cs = cur_cs;
        cur_cs = cs;
        eff = effect_empty;
        local_env = env;
        for (decl = (cs->decls); decl; decl = ({ast_generic tEmPcast = (ast_generic)(decl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 1540, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          {
            dinfo next;
            ((void) ((decl->kind != kind_asm_decl) ? 0 : (__assert_fail ("decl->kind != kind_asm_decl", "analyze.c", 1544, __PRETTY_FUNCTION__), 0)));
            next = analyze_declaration(decl, local_env, new_drinfolist);
            eff = effect_union(eff, next.eff);
            local_env = effect_union(local_env, next.alocs);
          }
        {
          dinfo d = confine_expressions(cs, new_drinfolist, local_env);
          local_env = effect_union(local_env, d.alocs);
          eff = effect_union(d.eff, eff);
        }
        cs->scope_env = local_env;
        mark_effect_interesting(cs->scope_env);
        for (stmt = (cs->stmts); stmt; stmt = ({ast_generic tEmPcast = (ast_generic)(stmt->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "analyze.c", 1561, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }))
          {
            sinfo next = analyze_statement(stmt, local_env, new_drinfolist);
            eff = effect_union(eff, next.eff);
          }
        eff = enforce_restricts(cs->decls, eff, ((void *)0));
        enforce_readonly(cs->decls, eff);
        eff = enforce_confine(cs, eff, ((void *)0));
        cs->filter_env = filter_effects(((void *)0), env);
        mark_effect_interesting(cs->filter_env);
        eff = effect_inter(eff, cs->filter_env);
        cur_cs = cs->enclosing_cs;
        return mksinfo(eff);
      };
      break;
    case kind_if_stmt:
      {
        if_stmt is = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)", "analyze.c", 1591, __PRETTY_FUNCTION__), 0))); (if_stmt)(tEmPcast); });
        einfo guard;
        sinfo then;
        effect eff;
        guard = analyze_expression(is->condition, rpos, env, drinfolist);
        then = analyze_statement(is->stmt1, env, drinfolist);
        eff = effect_union(guard.eff, then.eff);
        if (is->stmt2)
          {
            sinfo els;
            els = analyze_statement(is->stmt2, env, drinfolist);
            eff = effect_union(eff, els.eff);
          }
        return mksinfo(eff);
      };
      break;
    case kind_labeled_stmt:
      {
        labeled_stmt ls = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)", "analyze.c", 1613, __PRETTY_FUNCTION__), 0))); (labeled_stmt)(tEmPcast); });
        ls->label->enclosing_cs = cur_cs;
        return analyze_statement(ls->stmt, env, drinfolist);
      };
      break;
    case kind_expression_stmt:
      {
        expression_stmt es = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)", "analyze.c", 1622, __PRETTY_FUNCTION__), 0))); (expression_stmt)(tEmPcast); });
        einfo ei;
        ei = analyze_expression(es->arg1, rpos, env, drinfolist);
        return mksinfo(ei.eff);
      };
      break;
    case kind_while_stmt:
      {
        while_stmt ws = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_while_stmt && (tEmPcast)->kind <= postkind_while_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_while_stmt && (tEmPcast)->kind <= postkind_while_stmt)", "analyze.c", 1631, __PRETTY_FUNCTION__), 0))); (while_stmt)(tEmPcast); });
        einfo guard;
        sinfo body;
        effect eff;
        ws->enclosing_cs = cur_cs;
        guard = analyze_expression(ws->condition, rpos, env, drinfolist);
        body = analyze_statement(ws->stmt, env, drinfolist);
        eff = effect_union(guard.eff, body.eff);
        return mksinfo(eff);
      };
      break;
    case kind_dowhile_stmt:
      {
        dowhile_stmt dws = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_dowhile_stmt && (tEmPcast)->kind <= postkind_dowhile_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_dowhile_stmt && (tEmPcast)->kind <= postkind_dowhile_stmt)", "analyze.c", 1646, __PRETTY_FUNCTION__), 0))); (dowhile_stmt)(tEmPcast); });
        sinfo body;
        effect eff;
        dws->enclosing_cs = cur_cs;
        body = analyze_statement(dws->stmt, env, drinfolist);
        eff = body.eff;
        if (!definite_zero(dws->condition))
          {
            einfo guard;
            guard = analyze_expression(dws->condition, rpos, env, drinfolist);
            eff = effect_union(eff, guard.eff);
          }
        return mksinfo(eff);
      };
      break;
    case kind_switch_stmt:
      {
        switch_stmt ss = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_switch_stmt && (tEmPcast)->kind <= postkind_switch_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_switch_stmt && (tEmPcast)->kind <= postkind_switch_stmt)", "analyze.c", 1668, __PRETTY_FUNCTION__), 0))); (switch_stmt)(tEmPcast); });
        einfo guard;
        sinfo body;
        effect eff;
        ss->enclosing_cs = cur_cs;
        guard = analyze_expression(ss->condition, rpos, env, drinfolist);
        body = analyze_statement(ss->stmt, env, drinfolist);
        eff = effect_union(guard.eff, body.eff);
        return mksinfo(eff);
      };
      break;
    case kind_for_stmt:
      {
        for_stmt fs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_for_stmt && (tEmPcast)->kind <= postkind_for_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_for_stmt && (tEmPcast)->kind <= postkind_for_stmt)", "analyze.c", 1684, __PRETTY_FUNCTION__), 0))); (for_stmt)(tEmPcast); });
        sinfo body;
        effect eff;
        fs->enclosing_cs = cur_cs;
        eff = effect_empty;
        if (fs->arg1)
          {
            einfo arg1;
            arg1 = analyze_expression(fs->arg1, rpos, env, drinfolist);
            eff = effect_union(eff, arg1.eff);
          }
        if (fs->arg2)
          {
            einfo arg2;
            arg2 = analyze_expression(fs->arg2, rpos, env, drinfolist);
            eff = effect_union(eff, arg2.eff);
          }
        body = analyze_statement(fs->stmt, env, drinfolist);
        eff = effect_union(eff, body.eff);
        if (fs->arg3)
          {
            einfo arg3;
            arg3 = analyze_expression(fs->arg3, rpos, env, drinfolist);
            eff = effect_union(eff, arg3.eff);
          }
        return mksinfo(eff);
      };
      break;
    case kind_return_stmt:
      {
        return_stmt rs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_return_stmt && (tEmPcast)->kind <= postkind_return_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_return_stmt && (tEmPcast)->kind <= postkind_return_stmt)", "analyze.c", 1724, __PRETTY_FUNCTION__), 0))); (return_stmt)(tEmPcast); });
        qtype ret_qtype;
        effect eff;
        rs->enclosing_cs = cur_cs;
        ret_qtype = cur_return_qtype();
        eff = effect_empty;
        if (rs->arg1)
          {
            einfo arg1;
            arg1 = analyze_expression(rs->arg1, rpos, env, drinfolist);
            eff = effect_union(eff, arg1.eff);
            if (mkleq_assign_convert(rs->loc, &fi_qgate, arg1.qt, ret_qtype))
              report_qerror(rs->loc, sev_err, "return type incompatible with function type");
          }
        return mksinfo(eff);
      };
      break;
    case kind_computed_goto_stmt:
      {
        computed_goto_stmt cgs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_computed_goto_stmt && (tEmPcast)->kind <= postkind_computed_goto_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_computed_goto_stmt && (tEmPcast)->kind <= postkind_computed_goto_stmt)", "analyze.c", 1759, __PRETTY_FUNCTION__), 0))); (computed_goto_stmt)(tEmPcast); });
        einfo arg1;
        arg1 = analyze_expression(cgs->arg1, rpos, env, drinfolist);
        return mksinfo(arg1.eff);
      };
      break;
    case kind_break_stmt:
      {
        break_stmt bs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_break_stmt && (tEmPcast)->kind <= postkind_break_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_break_stmt && (tEmPcast)->kind <= postkind_break_stmt)", "analyze.c", 1769, __PRETTY_FUNCTION__), 0))); (break_stmt)(tEmPcast); });
        bs->enclosing_cs = cur_cs;
        return mksinfo(effect_empty);
      }
    case kind_continue_stmt:
      {
        continue_stmt cs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_continue_stmt && (tEmPcast)->kind <= postkind_continue_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_continue_stmt && (tEmPcast)->kind <= postkind_continue_stmt)", "analyze.c", 1776, __PRETTY_FUNCTION__), 0))); (continue_stmt)(tEmPcast); });
        cs->enclosing_cs = cur_cs;
        return mksinfo(effect_empty);
      }
    case kind_goto_stmt:
      {
        goto_stmt gs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_goto_stmt && (tEmPcast)->kind <= postkind_goto_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_goto_stmt && (tEmPcast)->kind <= postkind_goto_stmt)", "analyze.c", 1783, __PRETTY_FUNCTION__), 0))); (goto_stmt)(tEmPcast); });
        gs->enclosing_cs = cur_cs;
        return mksinfo(effect_empty);
      }
    case kind_empty_stmt:
      return mksinfo(effect_empty);
    case kind_change_type_stmt:
      {
        change_type_stmt ct = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_change_type_stmt && (tEmPcast)->kind <= postkind_change_type_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_change_type_stmt && (tEmPcast)->kind <= postkind_change_type_stmt)", "analyze.c", 1792, __PRETTY_FUNCTION__), 0))); (change_type_stmt)(tEmPcast); });
        einfo arg1;
        arg1 = analyze_expression(ct->arg1, lpos, env, drinfolist);
        return do_change_type(arg1, ct->asttype->type, &ct->qtype, ct->loc);
      }
      break;
    case kind_assert_type_stmt:
      {
        assert_type_stmt at = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_assert_type_stmt && (tEmPcast)->kind <= postkind_assert_type_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_assert_type_stmt && (tEmPcast)->kind <= postkind_assert_type_stmt)", "analyze.c", 1801, __PRETTY_FUNCTION__), 0))); (assert_type_stmt)(tEmPcast); });
        einfo arg1;
        arg1 = analyze_expression(at->arg1, rpos, env, drinfolist);
        return do_assert_type(arg1, at->asttype->type, &at->qtype, at->loc, at->asttype->loc);
      }
      break;
    case kind_deep_restrict_stmt:
      {
        deep_restrict_stmt dr = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_deep_restrict_stmt && (tEmPcast)->kind <= postkind_deep_restrict_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_deep_restrict_stmt && (tEmPcast)->kind <= postkind_deep_restrict_stmt)", "analyze.c", 1810, __PRETTY_FUNCTION__), 0))); (deep_restrict_stmt)(tEmPcast); });
        dd_list new_drinfolist;
        drinfo dri;
        aloc old_aloc, new_aloc;
        einfo arg1;
        sinfo stmt;
        effect eff = effect_empty;
        arg1 = analyze_expression(dr->arg1, lpos, env, drinfolist);
        ((void) ((qtype_pointer(arg1.qt)) ? 0 : (__assert_fail ("qtype_pointer(arg1.qt)", "analyze.c", 1820, __PRETTY_FUNCTION__), 0)));
        old_aloc = aloc_qtype(arg1.qt);
        new_aloc = make_aloc(rstrcat(parse_region,name_aloc(old_aloc),
                                     "_r"), points_to_qtype(arg1.qt),
                             global_aloc(old_aloc));
        mark_aloc_interesting(old_aloc);
        mark_aloc_interesting(new_aloc);
        dri = (sizeof(struct Drinfo) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct Drinfo)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct Drinfo), 0));
        dri->qtype = arg1.qt;
        dri->rqtype = mkqtype_pointer(dr->loc,
                                      qual_qtype(arg1.qt),
                                      points_to_qtype(arg1.qt), new_aloc);
        dri->effect = arg1.eff;
        dri->expression = dr->arg1;
        dri->location = dr->loc;
        dri->cs = ((void *)0);
        dr->drinfo = dri;
        dri->failed = 0;
        dri->inference = 0;
        mknin_effect_qtype(dri->location, effect_rwr(new_aloc),
                           points_to_qtype(arg1.qt));
        mknin_effect(dri->location, effect_single(new_aloc), env);
        env = effect_union(effect_single(new_aloc), env);
        if (flag_flow_sensitive)
          eff = effect_union(effect_alloc(new_aloc), eff);
        forall_aloc_mknin_effect(dri->location, eff_wr, dri->effect);
        forall_aloc_mknin_effect(dri->location, eff_alloc, dri->effect);
        new_drinfolist = dd_copy(parse_region, drinfolist);
        dd_add_first(parse_region, new_drinfolist, dri);
        stmt = analyze_statement(dr->stmt, env, new_drinfolist);
        mknin_effect(dri->location, effect_rwr(aloc_qtype(dri->qtype)),
                     stmt.eff);
        forall_aloc_in_effect_mknin_effect(dri->location,
                                           eff_r, dri->effect, eff_wr,
                                           stmt.eff);
        eff = effect_union(eff, stmt.eff);
        eff = effect_union(eff, effect_rwr(aloc_qtype(dri->qtype)));
        return mksinfo(eff);
      }
    default:
      __fail("analyze.c", 1875, __FUNCTION__, "(at %s:%ld) " "Unexpected statement kind 0x%x\n", ((s->loc)->filename), ((s->loc)->lineno), s->kind);
      break;
    }
}
static einfo analyze_expression(expression e, context context, effect env,
                                dd_list drinfolist)
{
  einfo result;
  dd_list_pos cur;
  {
    drinfo curdri = ((void *)0);
    for (cur = dd_first((drinfolist)); !(!(cur)->next); cur = ((cur)->next))
      {
        drinfo dri = ((drinfo)((cur)->data));
        if (equal_expressions(e, dri->expression))
          {
            if (!curdri || (!dri->inference && curdri->inference))
              curdri = dri;
          }
      }
    if (curdri)
      {
        if (context == rpos)
          {
            ((void) ((qtype_pointer(curdri->rqtype)) ? 0 : (__assert_fail ("qtype_pointer(curdri->rqtype)", "analyze.c", 1916, __PRETTY_FUNCTION__), 0)));
            e->qtype = points_to_qtype(curdri->rqtype);
            return mkeinfo(e->qtype,
                           effect_union(effect_r(aloc_qtype(curdri->rqtype)),
                                        curdri->effect),
                           0);
          }
        else
          {
            e->qtype = curdri->rqtype;
            return mkeinfo(e->qtype, curdri->effect, 0);
          }
      }
  }
  switch(e->kind) {
  case kind_comma:
    {
      comma c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_comma && (tEmPcast)->kind <= postkind_comma)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_comma && (tEmPcast)->kind <= postkind_comma)", "analyze.c", 1936, __PRETTY_FUNCTION__), 0))); (comma)(tEmPcast); });
      expression e2;
      effect eff;
      eff = effect_empty;
      for (e2 = (c->arg1); e2; e2 = ({ast_generic tEmPcast = (ast_generic)(e2->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 1941, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        {
          result = analyze_expression(e2, e2->next ? rpos : context, env,
                                      drinfolist);
          eff = effect_union(eff, result.eff);
        }
      if (!result.qt)
        __fail("analyze.c", 1948, __FUNCTION__, "(at %s:%ld) " "Empty comma expression\n", ((c->loc)->filename), ((c->loc)->lineno), 0);
      result = mkeinfo(result.qt, eff, 0);
    };
    break;
  case kind_sizeof_type:
    result = do_sizealign_of("sizeof_type", e->loc, context);
    break;
  case kind_alignof_type:
    result = do_sizealign_of("alignof_type", e->loc, context);
    break;
  case kind_label_address:
    {
      qtype points_to, qt;
      aloc loc;
      ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 1966, __PRETTY_FUNCTION__), 0)));
      points_to = mkqtype_void(make_qvar("&&_p", e->loc,0,0));
      loc = make_aloc("&&", points_to,0);
      mk_effect_leq_global_env(effect_single(loc));
      qt = mkqtype_pointer(e->loc,
                           make_qvar("&&", e->loc, 0,0),
                           points_to,
                           loc);
      mk_effect_leq_global_effect(effect_alloc(aloc_qtype(qt)));
      result = mkeinfo(qt, effect_empty, 0);
    }
    break;
  case kind_cast:
    {
      cast c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_cast && (tEmPcast)->kind <= postkind_cast)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_cast && (tEmPcast)->kind <= postkind_cast)", "analyze.c", 1984, __PRETTY_FUNCTION__), 0))); (cast)(tEmPcast); });
      einfo arg1;
      arg1 = analyze_expression(c->arg1, context, env, drinfolist);
      result = do_cast(arg1, c->asttype->type, c->loc, context);
    };
    break;
  case kind_cast_list:
    {
      cast_list cl = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_cast_list && (tEmPcast)->kind <= postkind_cast_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_cast_list && (tEmPcast)->kind <= postkind_cast_list)", "analyze.c", 1994, __PRETTY_FUNCTION__), 0))); (cast_list)(tEmPcast); });
      qtype qt;
      sinfo init;
      ((void) ((context == rpos || context == apos) ? 0 : (__assert_fail ("context == rpos || context == apos", "analyze.c", 1998, __PRETTY_FUNCTION__), 0)));
      qt = type_to_qtype(cl->type, "castlist", cl->loc);
      ((void) ((!cl->astqtype) ? 0 : (__assert_fail ("!cl->astqtype", "analyze.c", 2000, __PRETTY_FUNCTION__), 0)));
      cl->astqtype = qt;
      init = analyze_init(cl->loc, qt, cl->init_expr, env, drinfolist);
      if (context == apos)
        {
          aloc loc = make_aloc("castlist", qt,1);
          qt = mkqtype_pointer(cl->loc,
                               make_qvar("&castlist", cl->loc, 0, 0),
                               qt, loc);
        }
      qt = default_conversion_qtype(cl->loc, qt);
      result = mkeinfo(qt, init.eff, 0);
    };
    break;
  case kind_conditional:
    {
      conditional c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_conditional && (tEmPcast)->kind <= postkind_conditional)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_conditional && (tEmPcast)->kind <= postkind_conditional)", "analyze.c", 2018, __PRETTY_FUNCTION__), 0))); (conditional)(tEmPcast); });
      einfo cond, arg1, arg2;
      effect eff;
      qtype qt;
      cond = analyze_expression(c->condition, rpos, env, drinfolist);
      eff = cond.eff;
      if (c->arg1)
        {
          arg1 = analyze_expression(c->arg1, context, env, drinfolist);
          eff = effect_union(eff, arg1.eff);
        }
      else
        arg1 = cond;
      arg2 = analyze_expression(c->arg2, context, env, drinfolist);
      eff = effect_union(eff, arg2.eff);
      if (context == rpos)
        qt = type_to_qtype(c->type, "lub", c->loc);
      else
        qt = type_to_qtype(make_pointer_type(c->type), "lub", c->loc);
      if (qtype_void(arg1.qt))
        mkleq_assign_convert(c->loc, &fi_qgate, arg1.qt, qt);
      else if (qtype_void(arg2.qt))
        mkleq_assign_convert(c->loc, &fi_qgate, arg2.qt, qt);
      else if (mkleq_assign_convert(c->loc, &fi_qgate, arg1.qt, qt) ||
               mkleq_assign_convert(c->loc, &fi_qgate, arg2.qt, qt))
        report_qerror(c->loc, sev_err,
                      "incompatible branches of conditional");
      result = mkeinfo(qt, eff, 0);
    };
    break;
  case kind_identifier:
    {
      identifier id = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)", "analyze.c", 2055, __PRETTY_FUNCTION__), 0))); (identifier)(tEmPcast); });
      data_declaration root;
      qtype qt;
      root = root_ddecl(id->ddecl);
      if ((!(__extension__ (__builtin_constant_p (4) && ((__builtin_constant_p ("_op_") && strlen ("_op_") < ((size_t) (4))) || (__builtin_constant_p (root->name) && strlen (root->name) < ((size_t) (4)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("_op_") && __builtin_constant_p (root->name) && (__s1_len = strlen ("_op_"), __s2_len = strlen (root->name), (!((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("_op_"), (__const char *) (root->name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) && (__s1_len = strlen ("_op_"), __s1_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (root->name); register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s2_len = strlen (root->name), __s2_len < 4) ? (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("_op_"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (root->name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; }))) : strcmp ("_op_", root->name)))); }) : strncmp ("_op_", root->name, 4))) && strlen(root->name) >= 4))
        report_qerror(id->loc, sev_err,
                      "use of built-in operator name");
      mark_used(root);
      if (id->parent->kind != kind_function_call ||
          ({ast_generic tEmPcast = (ast_generic)(id->parent); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)", "analyze.c", 2066, __PRETTY_FUNCTION__), 0))); (function_call)(tEmPcast); })->arg1 != e)
        mark_address_taken(root);
      qt = root->qtype;
      if (!qt)
        {
          if ((type_function(id->ddecl->type) &&
               id->ddecl->ftype == function_implicit) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__func__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__func__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__func__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__func__") && ((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__func__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__func__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__func__") && ((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) && (__s2_len = strlen ("__func__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__func__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__func__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__func__")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__FUNCTION__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__FUNCTION__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__FUNCTION__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__FUNCTION__") && ((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__FUNCTION__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__FUNCTION__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__FUNCTION__") && ((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) && (__s2_len = strlen ("__FUNCTION__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__FUNCTION__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__FUNCTION__")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__PRETTY_FUNCTION__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__PRETTY_FUNCTION__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__PRETTY_FUNCTION__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__PRETTY_FUNCTION__") && ((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__PRETTY_FUNCTION__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__PRETTY_FUNCTION__") && ((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) && (__s2_len = strlen ("__PRETTY_FUNCTION__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__PRETTY_FUNCTION__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__PRETTY_FUNCTION__")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("=va_arg") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("=va_arg"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("=va_arg") + 1) - (size_t)(const void *)("=va_arg") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("=va_arg"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("=va_arg") && ((size_t)(const void *)(("=va_arg") + 1) - (size_t)(const void *)("=va_arg") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("=va_arg"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("=va_arg"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("=va_arg"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("=va_arg"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("=va_arg"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("=va_arg") && ((size_t)(const void *)(("=va_arg") + 1) - (size_t)(const void *)("=va_arg") == 1) && (__s2_len = strlen ("=va_arg"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("=va_arg"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("=va_arg"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("=va_arg"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("=va_arg"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("=va_arg"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("=va_arg"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("=va_arg"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("=va_arg"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "=va_arg")))); }))
            {
              __add_ddecl_qtype(root, 1);
              qt = root->qtype;
              mk_effect_leq_global_effect(defn_effect_qtype(qt));
            }
          else if (root->kind == decl_constant &&
                   root->ast->kind == kind_enumerator)
            {
              analyze_tag_ref(({ast_generic tEmPcast = (ast_generic)(root->ast->parent); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enum_ref && (tEmPcast)->kind <= postkind_enum_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enum_ref && (tEmPcast)->kind <= postkind_enum_ref)", "analyze.c", 2101, __PRETTY_FUNCTION__), 0))); (enum_ref)(tEmPcast); }));
              ((void) ((root->qtype) ? 0 : (__assert_fail ("root->qtype", "analyze.c", 2102, __PRETTY_FUNCTION__), 0)));
              qt = root->qtype;
            }
          else
            __user_error("analyze.c", 2106, __FUNCTION__, "(at %s:%ld) " "Use before declaration\n", ((id->loc)->filename), ((id->loc)->lineno), 0);
        }
      if (qtype_scheme(qt))
        qt = instantiate_qtype(qt, id->loc);
      if (flag_pam_mode || flag_print_results)
        pam_add_overlay_file(id->loc, root->name, (gqtype)qt);
      if (root->kind == decl_constant)
        {
          if (context != rpos)
            {
              report_qerror(id->loc, sev_err, "invalid lvalue");
              qt = error_qtype;
            }
          result = mkeinfo(qt, effect_empty, 0);
        }
      else
        {
          result = put_id_in_context(id->loc,
                                     mkeinfo(qt, effect_empty, 0),
                                     context);
          if (flag_poly && flag_flow_sensitive && qtype_function(qt)) {
            id->aloc = aloc_qtype(result.qt);
          }
        }
    };
    break;
  case kind_compound_expr:
    {
      compound_expr ce = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_expr && (tEmPcast)->kind <= postkind_compound_expr)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_expr && (tEmPcast)->kind <= postkind_compound_expr)", "analyze.c", 2142, __PRETTY_FUNCTION__), 0))); (compound_expr)(tEmPcast); });
      compound_stmt cs = ({ast_generic tEmPcast = (ast_generic)(ce->stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)", "analyze.c", 2143, __PRETTY_FUNCTION__), 0))); (compound_stmt)(tEmPcast); });
      statement cur_stmt;
      declaration d;
      qtype qt;
      effect eff, local_env;
      dd_list new_drinfolist;
      new_drinfolist = dd_copy(parse_region, drinfolist);
      cs->enclosing_cs = cur_cs;
      cur_cs = cs;
      if (cs->id_labels)
        __fail("analyze.c", 2156, __FUNCTION__, "(at %s:%ld) " "Unimplemented: id_labels\n", ((cs->loc)->filename), ((cs->loc)->lineno), 0);
      eff = effect_empty;
      local_env = env;
      for (d = (cs->decls); d; d = ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 2162, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
        {
          dinfo next;
          ((void) ((d->kind != kind_asm_decl) ? 0 : (__assert_fail ("d->kind != kind_asm_decl", "analyze.c", 2166, __PRETTY_FUNCTION__), 0)));
          next = analyze_declaration(d, local_env, new_drinfolist);
          eff = effect_union(eff, next.eff);
          local_env = effect_union(local_env, next.alocs);
        }
      cs->scope_env = local_env;
      mark_effect_interesting(cs->scope_env);
      cur_stmt = cs->stmts;
      while (cur_stmt && cur_stmt->next)
        {
          sinfo next = analyze_statement(cur_stmt, local_env, new_drinfolist);
          eff = effect_union(eff, next.eff);
          cur_stmt = ({ast_generic tEmPcast = (ast_generic)(cur_stmt->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "analyze.c", 2191, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
        }
      if (cur_stmt && ((cur_stmt)->kind >= kind_expression_stmt && (cur_stmt)->kind <= postkind_expression_stmt))
        {
          einfo next;
          next = analyze_expression(({ast_generic tEmPcast = (ast_generic)(cur_stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)", "analyze.c", 2200, __PRETTY_FUNCTION__), 0))); (expression_stmt)(tEmPcast); })->arg1,
                                    context,
                                    local_env, new_drinfolist);
          eff = effect_union(eff, next.eff);
          qt = next.qt;
        }
      else
        {
          qt = mkqtype_void(make_qvar("compound", ce->loc, 0,0));
          if (cur_stmt)
            {
              sinfo next = analyze_statement(cur_stmt, local_env,
                                             new_drinfolist);
              eff = effect_union(eff, next.eff);
            }
        }
      eff = enforce_restricts(cs->decls, eff, qt);
      enforce_readonly(cs->decls, eff);
      ce->filter_env = filter_effects(qt, env);
      mark_effect_interesting(ce->filter_env);
      eff = effect_inter(eff, ce->filter_env);
      cur_cs = cs->enclosing_cs;
      result = mkeinfo(qt, eff, 0);
    };
    break;
  case kind_function_call:
    {
      function_call fc = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)", "analyze.c", 2236, __PRETTY_FUNCTION__), 0))); (function_call)(tEmPcast); });
      einfo f_info, magic_info;
      qtype fqt;
      qtypelist args;
      expression arg;
      qtypelist_scanner args_qs;
      int i;
      effect eff;
      ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2252, __PRETTY_FUNCTION__), 0)));
      if (fc->va_arg_call)
        {
          qtype va_arg_qtype;
          va_arg_qtype = type_to_qtype(fc->va_arg_call->type,
                                       "_va_arg",
                                       fc->loc);
          result = mkeinfo(va_arg_qtype, effect_empty, 0);
          break;
        }
      f_info = analyze_expression(fc->arg1, rpos, env, drinfolist);
      fqt = points_to_qtype(f_info.qt);
      args = arg_qtypes(fqt);
      magic_info = analyze_magic_function_call(fc, fqt, env, drinfolist);
      if (magic_info.qt)
        {
          magic_info.eff = effect_union(f_info.eff, magic_info.eff);
          result = magic_info;
          break;
        }
      if (exists_effect_qual) {
        if (mkleq_qual(fc->loc, &effect_qgate, qual_qtype(fqt), cur_function_qual()))
          {
            report_qerror(fc->loc, sev_err,
                          "WARNING: $noninit function calls $init function\n");
          }
      }
      eff = effect_union(f_info.eff, effect_qtype(fqt));
      qtypelist_scan(args, &args_qs);
      i = 1;
      for (arg = (fc->args); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 2292, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        {
          einfo arg_info;
          qtype formal_qt;
          bool warned_oldstyle;
          arg_info = analyze_expression(arg, rpos, env, drinfolist);
          eff = effect_union(eff, arg_info.eff);
          formal_qt = qtypelist_next(&args_qs);
          warned_oldstyle = 0;
          if (!formal_qt)
            {
              if (!qtype_varargs(fqt) && !qtype_oldstyle(fqt))
                report_qerror(arg->loc, sev_err, "too many actual arguments for function call.\n");
              else if (qtype_oldstyle(fqt) && !warned_oldstyle)
                {
                  if (fc->arg1->kind == kind_identifier)
                    {
                      identifier fid = ({ast_generic tEmPcast = (ast_generic)(fc->arg1); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)", "analyze.c", 2314, __PRETTY_FUNCTION__), 0))); (identifier)(tEmPcast); });
                      if (!builtin_declaration(fid->ddecl))
                        report_qerror(fc->loc, sev_warn,
                                      "warning: ignoring argument types "
                                      "in call to oldstyle function");
                    }
                  warned_oldstyle = 1;
                }
              if (qtype_varargs(fqt) && vqual_qtype(fqt))
                {
                  qual vq;
                  vq = vqual_qtype(fqt);
                  if (varargs_constrain_quals_qtype(arg->loc,
                                                    arg_info.qt, vq))
                    report_qerror(arg->loc, sev_err,
                                  "type of actual argument %d doesn't match varargs qualifier", i);
                }
            }
          else
            if (mkleq_assign_convert(arg->loc, &fi_qgate,
                                     arg_info.qt, formal_qt))
              report_qerror(arg->loc, sev_err, "type of actual argument %d doesn't match type of formal", i);
          i++;
        }
      if (qtypelist_next(&args_qs))
        __user_error("analyze.c", 2343, __FUNCTION__, "(at %s:%ld) " "Not enough actual arguments.\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
      result = mkeinfo(return_qtype(fqt), eff, 0);
    };
    break;
  case kind_array_ref:
    {
      array_ref ar = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_array_ref && (tEmPcast)->kind <= postkind_array_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_array_ref && (tEmPcast)->kind <= postkind_array_ref)", "analyze.c", 2350, __PRETTY_FUNCTION__), 0))); (array_ref)(tEmPcast); });
      if (!flag_confine_inf)
        {
          expression array, plus, star_plus;
          if (type_array(ar->arg1->type))
            array = ar->arg1;
          else
            array = ar->arg2;
          array->lvalue = 1;
          array->cst = ((void *)0);
          plus = make_binary(ar->loc, kind_plus, ar->arg1, ar->arg2);
          star_plus = make_dereference(ar->loc, plus);
          ((void) ((!ar->alt) ? 0 : (__assert_fail ("!ar->alt", "analyze.c", 2367, __PRETTY_FUNCTION__), 0)));
          ar->alt = star_plus;
        }
      ((void) ((ar->alt) ? 0 : (__assert_fail ("ar->alt", "analyze.c", 2371, __PRETTY_FUNCTION__), 0)));
      result = analyze_expression(ar->alt, context, env, drinfolist);
    };
    break;
  case kind_field_ref:
    {
      field_ref fr = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_field_ref && (tEmPcast)->kind <= postkind_field_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_field_ref && (tEmPcast)->kind <= postkind_field_ref)", "analyze.c", 2381, __PRETTY_FUNCTION__), 0))); (field_ref)(tEmPcast); });
      einfo arg1;
      qtype field_qt;
      effect eff;
      arg1 = analyze_expression(fr->arg1, context, env, drinfolist);
      eff = arg1.eff;
      if (qtype_error(arg1.qt))
        field_qt = error_qtype;
      else
        {
          qtype qt, ag_qt;
          qt = arg1.qt;
          if (context == lpos || context == apos)
            {
              ag_qt = points_to_qtype(qt);
              if (flag_measure_consts && context == lpos)
                mkNonConst_qual(fr->loc, qual_qtype(qt));
              eff = effect_union(eff, effect_r(aloc_qtype(qt)));
            }
          else
            ag_qt = qt;
          ((void) ((qtype_aggregate(ag_qt)) ? 0 : (__assert_fail ("qtype_aggregate(ag_qt)", "analyze.c", 2407, __PRETTY_FUNCTION__), 0)));
          field_qt = field_qtype(ag_qt, fr->cstring);
        }
      if (flag_pam_mode || flag_print_results)
        pam_add_overlay_file(fr->cstring_loc, fr->cstring.data, (gqtype) field_qt);
      result = put_id_in_context(fr->loc,
                                 mkeinfo(field_qt, eff, 0),
                                 context);
    };
    break;
  case kind_init_list:
    {
      __fail("analyze.c", 2439, __FUNCTION__, "(at %s:%ld) " "Unexpected init list\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
    };
    break;
  case kind_init_index:
    {
      __fail("analyze.c", 2445, __FUNCTION__, "(at %s:%ld) " "Unexpected init index\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
    };
    break;
  case kind_init_field:
    {
      __fail("analyze.c", 2450, __FUNCTION__, "(at %s:%ld) " "Unexpected init field\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
    };
  case kind_lexical_cst:
    {
      lexical_cst lc = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_lexical_cst && (tEmPcast)->kind <= postkind_lexical_cst)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_lexical_cst && (tEmPcast)->kind <= postkind_lexical_cst)", "analyze.c", 2454, __PRETTY_FUNCTION__), 0))); (lexical_cst)(tEmPcast); });
      qtype qt;
      ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2457, __PRETTY_FUNCTION__), 0)));
      qt = type_to_qtype(lc->type, "__cst", lc->loc);
      result = mkeinfo(qt, effect_empty, 0);
    };
    break;
  case kind_string:
    {
      qtype points_to, qt;
      aloc loc;
      ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2475, __PRETTY_FUNCTION__), 0)));
      points_to = mkqtype_char(make_qvar("__string_p", e->loc, 0,1));
      loc = make_aloc("__string", points_to,1);
      mk_effect_leq_global_env(effect_single(loc));
      qt = mkqtype_pointer(e->loc,
                           make_qvar("__string", e->loc, 0,1),
                           points_to,
                           loc);
      mk_effect_leq_global_effect(effect_alloc(loc));
      result = mkeinfo(qt, effect_empty, 0);
    };
    break;
  default:
    if (((e)->kind >= kind_unary && (e)->kind <= postkind_unary))
      result = analyze_unary_expression(({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_unary && (tEmPcast)->kind <= postkind_unary)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_unary && (tEmPcast)->kind <= postkind_unary)", "analyze.c", 2492, __PRETTY_FUNCTION__), 0))); (unary)(tEmPcast); }), context, env,
                                        drinfolist);
    else if (((e)->kind >= kind_binary && (e)->kind <= postkind_binary))
      result = analyze_binary_expression(({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_binary && (tEmPcast)->kind <= postkind_binary)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_binary && (tEmPcast)->kind <= postkind_binary)", "analyze.c", 2495, __PRETTY_FUNCTION__), 0))); (binary)(tEmPcast); }), context, env,
                                         drinfolist);
    else
      __fail("analyze.c", 2498, __FUNCTION__, "(at %s:%ld) " "Unexpected expr kind 0x%x\n", ((e->loc)->filename), ((e->loc)->lineno), e->kind);
  }
  e->qtype = result.qt;
  return result;
}
static einfo analyze_unary_expression(unary e, context context, effect env,
                                      dd_list drinfolist)
{
  operator op;
  op = find_op_kind(e->kind);
  switch (e->kind)
    {
    case kind_dereference:
      {
        einfo arg1, result;
        effect eff = effect_empty;
        arg1 = analyze_expression(e->arg1, rpos, env, drinfolist);
        eff = arg1.eff;
        if (!qtype_pointer(arg1.qt))
          {
            report_qerror(e->loc, sev_err, "dereference of non-pointer type (probably a type incompatibility across files)");
            return mkeinfo(error_qtype, effect_empty, 0);
          }
        ((void) ((qtype_pointer(arg1.qt)) ? 0 : (__assert_fail ("qtype_pointer(arg1.qt)", "analyze.c", 2532, __PRETTY_FUNCTION__), 0)));
        if (context == lpos || context == apos)
          result = mkeinfo(arg1.qt, eff, 0);
        else if (context == rpos)
          {
            eff = effect_union(eff, effect_r(aloc_qtype(arg1.qt)));
            if (exists_effect_qual) {
              if (mkleq_qual(e->loc, &effect_qgate, qual_qtype(arg1.qt), cur_function_qual()))
                {
                  report_qerror(e->loc, sev_err,
                                "WARNING: $noninit function uses $init data\n");
                }
            }
            if (qtype_function(arg1.qt))
              __fail("analyze.c", 2549, __FUNCTION__, "(at %s:%ld) " "Unexpected function in rpos", ((e->loc)->filename), ((e->loc)->lineno), 0);
            else
             result = mkeinfo(default_conversion_qtype(e->loc, points_to_qtype(arg1.qt)),
                              eff, 0);
          }
        else
          __fail("analyze.c", 2555, __FUNCTION__, "(at %s:%ld) " "Unexpected context %d\n", ((e->loc)->filename), ((e->loc)->lineno), context);
        if (context != apos && op)
          {
            einfo result_rpos;
            if (context == rpos)
              result_rpos = result;
            else
              result_rpos = mkeinfo(default_conversion_qtype(e->loc,
                                                             points_to_qtype(arg1.qt)),
                                    eff, 0);
            if (add_op_constraints(op, result_rpos, arg1, dummy_einfo,
                                   e->loc))
              report_qerror(e->loc, sev_err,
                            "Operands incompatible with operator definition");
          }
        return result;
      }
      break;
    case kind_address_of:
      {
        ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2584, __PRETTY_FUNCTION__), 0)));
        if (type_function(e->arg1->type))
          return analyze_expression(e->arg1, context, env, drinfolist);
        else
          return analyze_expression(e->arg1, apos, env, drinfolist);
      }
      break;
    case kind_extension_expr:
      {
        return analyze_expression(e->arg1, context, env, drinfolist);
      }
      break;
    case kind_sizeof_expr:
      return do_sizealign_of("sizeof", e->loc, context);
    case kind_alignof_expr:
      return do_sizealign_of("alignof", e->loc, context);
    case kind_realpart:
    case kind_imagpart:
      {
        einfo arg1;
        ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2605, __PRETTY_FUNCTION__), 0)));
        arg1 = analyze_expression(e->arg1, rpos, env, drinfolist);
        ((void) ((qtype_complex(arg1.qt)) ? 0 : (__assert_fail ("qtype_complex(arg1.qt)", "analyze.c", 2607, __PRETTY_FUNCTION__), 0)));
        return mkeinfo(complex_base_qtype(arg1.qt), arg1.eff, 0);
      }
      break;
    case kind_unary_plus:
      {
        return analyze_expression(e->arg1, context, env, drinfolist);
      }
      break;
    case kind_unary_minus:
    case kind_conjugate:
    case kind_bitnot:
      {
        ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2624, __PRETTY_FUNCTION__), 0)));
        return analyze_expression(e->arg1, rpos, env, drinfolist);
      }
      break;
    case kind_not:
      {
        einfo arg1;
        qtype qt;
        ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2633, __PRETTY_FUNCTION__), 0)));
        arg1 = analyze_expression(e->arg1, rpos, env, drinfolist);
        qt = mkqtype_bool(make_qvar("!", e->loc, 0,0));
        return mkeinfo(qt, arg1.eff, 0);
      }
      break;
    case kind_preincrement:
    case kind_postincrement:
    case kind_predecrement:
    case kind_postdecrement:
      {
        einfo arg1;
        effect eff;
        ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2647, __PRETTY_FUNCTION__), 0)));
        arg1 = analyze_expression(e->arg1, lpos, env, drinfolist);
        qtype_mklhs_nonconst(e->loc, arg1.qt);
        eff = effect_union(arg1.eff, effect_wr(aloc_qtype(arg1.qt)));
        return mkeinfo(points_to_qtype(arg1.qt),
                       eff, 0);
      }
      break;
    default:
      __fail("analyze.c", 2656, __FUNCTION__, "(at %s:%ld) " "Unexpected unary op kind 0x%x\n", ((e->loc)->filename), ((e->loc)->lineno), e->kind);
    }
}
bool isassignment(binary e)
{
  return (e->kind == kind_plus_assign ||
          e->kind == kind_minus_assign ||
          e->kind == kind_modulo_assign ||
          e->kind == kind_lshift_assign ||
          e->kind == kind_rshift_assign ||
          e->kind == kind_times_assign ||
          e->kind == kind_divide_assign ||
          e->kind == kind_bitand_assign ||
          e->kind == kind_bitor_assign ||
          e->kind == kind_bitxor_assign ||
          e->kind == kind_assign);
}
static einfo analyze_binary_expression(binary e, context context, effect env,
                                       dd_list drinfolist)
{
  einfo arg1, arg2;
  operator op;
  effect eff;
  ((void) ((context == rpos) ? 0 : (__assert_fail ("context == rpos", "analyze.c", 2683, __PRETTY_FUNCTION__), 0)));
  op = find_op_kind(e->kind);
  if (isassignment(e))
    arg1 = analyze_expression(e->arg1, lpos, env, drinfolist);
  else
    arg1 = analyze_expression(e->arg1, rpos, env, drinfolist);
  arg2 = analyze_expression(e->arg2, rpos, env, drinfolist);
  eff = effect_union(arg1.eff, arg2.eff);
  if (qtype_error(arg1.qt) || qtype_error(arg2.qt))
    return mkeinfo(error_qtype, eff, 0);
  switch (e->kind)
    {
    case kind_plus:
      {
        qtype qt;
        if (qtype_pointer(arg1.qt) && qtype_pointer(arg2.qt))
          __user_error("analyze.c", 2702, __FUNCTION__, "(at %s:%ld) " "Unexpected pointer addition\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
        if (qtype_pointer(arg1.qt) && !qtype_pointer(arg2.qt))
          qt = arg1.qt;
        else if (!qtype_pointer(arg1.qt) && qtype_pointer(arg2.qt))
          qt = arg2.qt;
        else
          {
            if (mkeq_qtype(e->loc, &fi_qgate, arg1.qt, arg2.qt))
              report_qerror(e->loc, sev_err,
                            "incompatible operands of ``%s''",
                            binary_op_name(e->kind));
            qt = arg1.qt;
          }
        return mkeinfo(qt, eff, 0);
      }
      break;
    case kind_minus:
      {
        qtype qt;
        if (qtype_pointer(arg1.qt) && qtype_pointer(arg2.qt))
          qt = mkqtype_ptrdiff_t(make_qvar("__ptr diff", e->loc, 0,0));
        else if (qtype_pointer(arg1.qt) && !qtype_pointer(arg2.qt))
          qt = arg1.qt;
        else if (!qtype_pointer(arg1.qt) && qtype_pointer(arg2.qt))
          qt = arg2.qt;
        else
          {
            if (mkeq_qtype(e->loc, &fi_qgate, arg1.qt, arg2.qt))
              report_qerror(e->loc, sev_err,
                            "incompatible operands of ``%s''",
                            binary_op_name(e->kind));
            qt = arg1.qt;
          }
        return mkeinfo(qt, eff, 0);
      }
      break;
    case kind_modulo:
    case kind_lshift:
    case kind_rshift:
      {
        return mkeinfo(arg1.qt, eff, 0);
      }
    case kind_modulo_assign:
    case kind_lshift_assign:
    case kind_rshift_assign:
      {
        qtype_mklhs_nonconst(e->loc, arg1.qt);
        return mkeinfo(points_to_qtype(arg1.qt),
                       effect_union(eff, effect_wr(aloc_qtype(arg1.qt))),
                       0);
      }
    case kind_times:
    case kind_divide:
    case kind_bitand:
    case kind_bitor:
    case kind_bitxor:
      {
        if (mkeq_qtype(e->loc, &fi_qgate, arg1.qt, arg2.qt))
          report_qerror(e->loc, sev_err,
                        "incompatible operands of ``%s''",
                        binary_op_name(e->kind));
        return mkeinfo(arg1.qt, eff, 0);
      }
      break;
    case kind_leq:
    case kind_geq:
    case kind_lt:
    case kind_gt:
    case kind_eq:
    case kind_ne:
    case kind_andand:
    case kind_oror:
      {
        return do_boolop(e->loc,
                         (e->kind == kind_eq || e->kind == kind_ne),
                         definite_null(e->arg1),
                         definite_null(e->arg2),
                         arg1,
                         arg2,
                         eff,
                         op);
      }
    case kind_plus_assign:
    case kind_minus_assign:
      {
        qtype arg1_pt;
        arg1_pt = points_to_qtype(arg1.qt);
        if ((qtype_pointer(arg1_pt) && qtype_pointer(arg2.qt)))
          __user_error("analyze.c", 2799, __FUNCTION__, "(at %s:%ld) " "Unexpected pointer addition\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
        if (qtype_pointer(arg1_pt) && !qtype_pointer(arg2.qt))
          {
            qtype_mklhs_nonconst(e->loc, arg1.qt);
            return mkeinfo(arg1_pt,
                           effect_union(eff, effect_wr(aloc_qtype(arg1.qt))),
                           0);
          }
      }
    case kind_times_assign:
    case kind_divide_assign:
    case kind_bitand_assign:
    case kind_bitor_assign:
    case kind_bitxor_assign:
    case kind_assign:
      {
        return do_assign(arg1, arg2, e->loc, eff);
      }
    default:
      __fail("analyze.c", 2822, __FUNCTION__, "(at %s:%ld) " "Unexpected binary op kind 0x%x\n", ((e->loc)->filename), ((e->loc)->lineno), e->kind);
    }
}
bool restrict_variable_decl(variable_decl vd, bool isglobal)
{
  data_declaration root;
  qtype rqtype, lqtype;
  root = root_ddecl(vd->ddecl);
  if (!root->qtype)
    return 0;
  lqtype = root->qtype;
  if (!qtype_pointer(lqtype))
    return 0;
  rqtype = points_to_qtype(lqtype);
  if (restrict_qual &&
      has_qual_qtype(rqtype, restrict_qual))
    {
      ((void) ((qtype_pointer(rqtype)) ? 0 : (__assert_fail ("qtype_pointer(rqtype)", "analyze.c", 2855, __PRETTY_FUNCTION__), 0)));
      if (const_qual && has_qual_qtype(lqtype, const_qual))
        {
          rinfo ri;
          qtype rqtype_points_to, new_rqtype, new_lqtype;
          aloc old_aloc, old_toplevel_aloc, new_aloc, new_toplevel_aloc;
          ((void) ((!vd->rinfo) ? 0 : (__assert_fail ("!vd->rinfo", "analyze.c", 2866, __PRETTY_FUNCTION__), 0)));
          old_toplevel_aloc = aloc_qtype(lqtype);
          old_aloc = aloc_qtype(rqtype);
          rqtype_points_to = points_to_qtype(rqtype);
          new_aloc = make_aloc(rstrcat(parse_region,name_aloc(old_aloc),"_r"),
                               rqtype_points_to,global_aloc(old_aloc));
          new_rqtype = mkqtype_pointer(vd->loc,
                                       qual_qtype(rqtype),
                                       rqtype_points_to,
                                       new_aloc);
          new_toplevel_aloc = make_aloc(rstrcat(parse_region,
                                                name_aloc(old_toplevel_aloc),
                                                "_rtl"),
                                        new_rqtype,
                                        global_aloc(old_toplevel_aloc));
          new_lqtype = mkqtype_pointer(vd->loc,
                                       qual_qtype(lqtype),
                                       new_rqtype,
                                       new_toplevel_aloc);
          ri = (sizeof(struct Rinfo) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct Rinfo)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct Rinfo), 0));
          ri->isglobal = isglobal;
          ri->fs_fqtype = ((void *)0);
          ri->fs_rqtype = ((void *)0);
          ri->lin = ((void *)0);
          if (!isglobal)
            {
              ri->flocs = effect_rwr(old_aloc);
              ri->flocs_alloc = effect_alloc(old_aloc);
              ri->fqtype = lqtype;
              ri->rlocs = effect_rwr(new_aloc);
              ri->rlocs_alloc = effect_alloc(new_aloc);
              ri->rqtype = new_lqtype;
            }
          else
            {
              ri->flocs = effect_rwr(new_aloc);
              ri->flocs_alloc = effect_alloc(new_aloc);
              ri->fqtype = new_lqtype;
              ri->rlocs = effect_rwr(old_aloc);
              ri->rlocs_alloc = effect_alloc(old_aloc);
              ri->rqtype = lqtype;
            }
          vd->rinfo = ri;
          mark_aloc_interesting(old_aloc);
          mark_aloc_interesting(new_aloc);
          mknin_effect_qtype(vd->loc, effect_rwr(new_aloc),
                             points_to_qtype(rqtype));
          return 1;
        }
      else
        report_qerror(vd->loc, sev_err,
                      "lvalue of restrict-qualified pointer is "
                      "non-const -- ignoring restrict");
    }
  return 0;
}
effect enforce_var_decl_restrict(variable_decl vd, effect eff, qtype qt)
{
  if (vd && vd->rinfo)
    {
      mknin_effect(vd->loc, vd->rinfo->flocs, eff);
      if (qt != ((void *)0) && !(eq_effect(vd->rinfo->rlocs, effect_empty)))
        mknin_effect_qtype(vd->loc, vd->rinfo->rlocs, qt);
      eff = effect_union(eff, vd->rinfo->flocs);
    }
  eff = effect_union(eff, vd->arg1_eff);
  return eff;
}
effect enforce_restricts(declaration d, effect eff, qtype qt)
{
  if (d == ((void *)0))
    return eff;
  eff = enforce_restricts(({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 2976, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), eff, qt);
  switch (d->kind)
    {
    case kind_asm_decl:
    case kind_function_decl:
    case kind_ellipsis_decl:
    case kind_oldidentifier_decl:
      return eff;
    case kind_extension_decl:
      {
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "analyze.c", 2986, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        ((void) ((ed->decl->next == ((void *)0)) ? 0 : (__assert_fail ("ed->decl->next == ((void *)0)", "analyze.c", 2988, __PRETTY_FUNCTION__), 0)));
        return enforce_restricts(ed->decl, eff, qt);
      }
    case kind_data_decl:
      {
        declaration vd;
        for (vd = (({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "analyze.c", 2995, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); })->decls); vd; vd = ({ast_generic tEmPcast = (ast_generic)(vd->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 2995, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          eff = enforce_var_decl_restrict(({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "analyze.c", 2996, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); }), eff, qt);
        return eff;
      }
    default:
      __fail("analyze.c", 3000, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind %x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
effect filter_effects(qtype qt, effect alocs)
{
  effect filter;
  filter = alocs;
  if (qt)
    filter = effect_union(filter, alocs_qtype(qt));
  if (current_function_decl)
    filter = effect_union(filter, alocs_qtype(cur_return_qtype()));
  return filter;
}
static int fopen_count = 0;
static int alloc_count = 0;
bool have_stream_quals = 0;
bool have_sleepy_quals = 0;
qual open_unchecked_qual = ((void *)0);
qual read_unchecked_qual = ((void *)0);
qual write_unchecked_qual = ((void *)0);
qual readwrite_unchecked_qual = ((void *)0);
qual open_qual = ((void *)0);
qual read_qual = ((void *)0);
qual write_qual = ((void *)0);
qual readwrite_qual = ((void *)0);
qual closed_qual = ((void *)0);
qual enabled_qual = ((void *)0);
qual disabled_qual = ((void *)0);
qual readonly_qual = ((void *)0);
qtype interrupt_status_qtype = ((void *)0);
static void init_magic(void)
{
  fopen_count = 0;
  alloc_count = 0;
  open_unchecked_qual = find_qual("$open_unchecked");
  read_unchecked_qual = find_qual("$read_unchecked");
  write_unchecked_qual = find_qual("$write_unchecked");
  readwrite_unchecked_qual = find_qual("$readwrite_unchecked");
  open_qual = find_qual("$open");
  read_qual = find_qual("$read");
  write_qual = find_qual("$write");
  readwrite_qual = find_qual("$readwrite");
  closed_qual = find_qual("$closed");
  have_stream_quals = (open_unchecked_qual &&
                       read_unchecked_qual &&
                       write_unchecked_qual &&
                       readwrite_unchecked_qual &&
                       open_qual &&
                       read_qual &&
                       write_qual &&
                       readwrite_qual &&
                       closed_qual);
  enabled_qual = find_qual("$enabled");
  disabled_qual = find_qual("$disabled");
  have_sleepy_quals = (enabled_qual && disabled_qual);
  readonly_qual = find_qual("$readonly");
}
static einfo analyze_magic_function_call(function_call fc, qtype fqt,
                                         effect env, dd_list drinfolist)
{
  if (fc->arg1->kind == kind_identifier)
    {
      identifier id = ({ast_generic tEmPcast = (ast_generic)(fc->arg1); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)", "analyze.c", 3076, __PRETTY_FUNCTION__), 0))); (identifier)(tEmPcast); });
      if ((!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) && (__s2_len = strlen ("fopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fdopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fdopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fdopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fdopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) && (__s2_len = strlen ("fdopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fdopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fdopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("tmpfile") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("tmpfile"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("tmpfile"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("tmpfile") && ((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("tmpfile"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("tmpfile"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("tmpfile") && ((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) && (__s2_len = strlen ("tmpfile"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("tmpfile"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("tmpfile"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; }))) : strcmp (id->cstring.data, "tmpfile")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("safefopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("safefopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("safefopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("safefopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) && (__s2_len = strlen ("safefopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("safefopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "safefopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) && (__s2_len = strlen ("popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "popen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("my_popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("my_popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("my_popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("my_popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) && (__s2_len = strlen ("my_popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("my_popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "my_popen")))); })) &&
          have_stream_quals)
        {
          expression arg;
          effect eff;
          qtype ret_qt, file_qt, result_qt;
          const char *count, *new_al_name, *new_qual_name;
          qual new_qual;
          aloc new_al;
          eff = effect_empty;
          for (arg = (fc->args); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 3093, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
            {
              einfo arg_info = analyze_expression(arg, rpos, env, drinfolist);
              eff = effect_union(eff, arg_info.eff);
            }
          ret_qt = return_qtype(fqt);
          file_qt = points_to_qtype(ret_qt);
          count = inttostr(parse_region, fopen_count++);
          new_al_name = rstrscat(parse_region, name_aloc(aloc_qtype(ret_qt)),
                                 "_inst", count, ((void *)0));
          new_qual_name = rstrscat(parse_region, name_qual(qual_qtype(ret_qt)),
                                   "_inst", count, ((void *)0));
          new_al = make_aloc(new_al_name, file_qt,0);
          mark_aloc_interesting(new_al);
          eff = effect_union(eff, effect_alloc(new_al));
          new_qual = make_qvar(new_qual_name, id->loc, 1,0);
          result_qt = mkqtype_pointer(id->loc, new_qual, file_qt, new_al);
          return mkeinfo(result_qt, eff, 0);
        }
      if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("malloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("malloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("malloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("malloc") && ((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("malloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("malloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("malloc") && ((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) && (__s2_len = strlen ("malloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("malloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("malloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "malloc")))); }) ||
          !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("xmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("xmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("xmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("xmalloc") && ((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("xmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("xmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("xmalloc") && ((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) && (__s2_len = strlen ("xmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("xmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("xmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "xmalloc")))); }) ||
          !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("vmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("vmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("vmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("vmalloc") && ((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("vmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("vmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("vmalloc") && ((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) && (__s2_len = strlen ("vmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("vmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("vmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "vmalloc")))); }) ||
          !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) && (__s2_len = strlen ("kmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmalloc")))); }) ||
          !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("ioremap") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("ioremap"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("ioremap"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("ioremap") && ((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("ioremap"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("ioremap"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("ioremap") && ((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) && (__s2_len = strlen ("ioremap"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("ioremap"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("ioremap"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; }))) : strcmp (id->cstring.data, "ioremap")))); }) ||
          !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmem_cache_alloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmem_cache_alloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmem_cache_alloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmem_cache_alloc") && ((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmem_cache_alloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmem_cache_alloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmem_cache_alloc") && ((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) && (__s2_len = strlen ("kmem_cache_alloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmem_cache_alloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmem_cache_alloc")))); }))
        {
          qtype ret_qt, pointsto;
          aloc loc;
          expression arg;
          effect eff;
          const char *count;
          const char *new_void_name, *new_aloc_name, *new_void_pointer_name;
          eff = effect_empty;
          for (arg = (fc->args); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "analyze.c", 3129, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
            {
              einfo arg_info = analyze_expression(arg, rpos, env, drinfolist);
              eff = effect_union(eff, arg_info.eff);
            }
          count = inttostr(parse_region, alloc_count++);
          new_void_name = rstrcat(parse_region, "alloc_p_inst", count);
          new_aloc_name = rstrcat(parse_region, "alloc_inst", count);
          new_void_pointer_name = rstrcat(parse_region, "alloc_inst", count);
          pointsto = mkqtype_void(make_qvar(new_void_name, fc->loc, 0,0));
          loc = make_aloc(new_aloc_name, pointsto,1);
          ret_qt = mkqtype_pointer(fc->loc,
                                   make_qvar(new_void_pointer_name, fc->loc,
                                             0,0),
                                   pointsto,
                                   loc);
          eff = effect_union(effect_alloc(loc), eff);
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) && (__s2_len = strlen ("kmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmalloc")))); }) && have_sleepy_quals)
            {
              ((void) ((interrupt_status_qtype != ((void *)0)) ? 0 : (__assert_fail ("interrupt_status_qtype != ((void *)0)", "analyze.c", 3151, __PRETTY_FUNCTION__), 0)));
              eff = effect_union(effect_wr(aloc_qtype(interrupt_status_qtype)),
                                 eff);
            }
          return mkeinfo(ret_qt, eff, 1);
        }
      else
        return mkeinfo(((void *)0), ((void *)0), 0);
    }
  else
    return mkeinfo(((void *)0), ((void *)0), 0);
}
void enforce_var_decl_readonly(variable_decl vd, effect eff)
{
  data_declaration root;
  qtype pqtype;
  if (!vd || !vd->ddecl)
    return;
  root = root_ddecl(vd->ddecl);
  if (!root->qtype || !qtype_pointer(root->qtype) ||
      !qtype_pointer((pqtype = points_to_qtype(root->qtype))))
    return;
  if (readonly_qual && has_qual_qtype(pqtype, readonly_qual))
    mknin_effect(vd->loc, effect_wr(aloc_qtype(pqtype)), eff);
}
void enforce_readonly(declaration d, effect eff)
{
  if (d == ((void *)0))
    return;
  enforce_readonly(({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 3198, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), eff);
  switch (d->kind)
    {
    case kind_asm_decl:
    case kind_function_decl:
    case kind_ellipsis_decl:
    case kind_oldidentifier_decl:
      return;
    case kind_extension_decl:
      {
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "analyze.c", 3208, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        ((void) ((ed->decl->next == ((void *)0)) ? 0 : (__assert_fail ("ed->decl->next == ((void *)0)", "analyze.c", 3210, __PRETTY_FUNCTION__), 0)));
        enforce_readonly(ed->decl, eff);
        return;
      }
    case kind_data_decl:
      {
        declaration vd;
        for (vd = (({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "analyze.c", 3218, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); })->decls); vd; vd = ({ast_generic tEmPcast = (ast_generic)(vd->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "analyze.c", 3218, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          enforce_var_decl_readonly(({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "analyze.c", 3219, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); }), eff);
        return;
      }
    default:
      __fail("analyze.c", 3223, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind %x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static dinfo confine_expressions(compound_stmt cs, dd_list drinfolist, effect env)
{
  dd_list_pos cur;
  effect alocs, eff;
  eff = effect_empty;
  alocs = effect_empty;
  if (!cs->confine_expressions)
    return mkdinfo(eff, alocs);
  for (cur = dd_first((cs->confine_expressions)); !(!(cur)->next); cur = ((cur)->next))
    {
      exprdrinfo ed = ((exprdrinfo)((cur)->data));
      expression e = ed->e;
      drinfo dri;
      aloc old_aloc, new_aloc;
      einfo arg1;
      arg1 = analyze_expression(e, lpos, env, drinfolist);
      ((void) ((qtype_pointer(arg1.qt)) ? 0 : (__assert_fail ("qtype_pointer(arg1.qt)", "analyze.c", 3252, __PRETTY_FUNCTION__), 0)));
      old_aloc = aloc_qtype(arg1.qt);
      new_aloc = make_aloc(rstrcat(parse_region,name_aloc(old_aloc),
                                   "_r"), points_to_qtype(arg1.qt),
                           global_aloc(old_aloc));
      mark_aloc_interesting(old_aloc);
      mark_aloc_interesting(new_aloc);
      dri = (sizeof(struct Drinfo) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct Drinfo)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct Drinfo), 0));
      dri->qtype = arg1.qt;
      dri->rqtype = mkqtype_pointer(e->loc,
                                    qual_qtype(arg1.qt),
                                    points_to_qtype(arg1.qt), new_aloc);
      dri->effect = arg1.eff;
      dri->location = cs->loc;
      dri->cs = cs;
      dri->expression = e;
      dri->env = env;
      dri->failed = 0;
      dri->inference = 1;
      dd_add_first(parse_region, drinfolist, dri);
      ed->drinfo = dri;
      alocs = effect_union(alocs_qtype(dri->rqtype), alocs);
    }
  return mkdinfo(eff, alocs);
}
static effect enforce_confine(compound_stmt cs, effect eff, qtype qt)
{
  dd_list_pos cur;
  if (!cs->confine_expressions)
    return eff;
  for (cur = dd_first((cs->confine_expressions)); !(!(cur)->next); cur = ((cur)->next))
    {
      exprdrinfo ed = ((exprdrinfo)((cur)->data));
      if (ed->drinfo && ed->drinfo->inference)
        {
          (ed->drinfo)->body_eff = effect_var("<confine body>");
          mkleq_effect(eff, (ed->drinfo)->body_eff);
          mk_confine_inf_const(ed->drinfo);
          eff = effect_union((ed->drinfo)->body_eff,
                             effect_rwr(aloc_qtype((ed->drinfo)->qtype)));
        }
    }
  return eff;
}
const char CANON_IDENT_159d7744eb6d311db4edf7385045c03f[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/analyze.c";
