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
extern type builtin_va_list_type;
extern data_declaration builtin_va_arg_decl;
void pending_xref_error(void);
void init_semantics(void);
extern environment current_env;
data_declaration lookup_id(const char *s, bool this_level_only);
data_declaration lookup_global_id(const char *s);
void shadow_tag(type_element elements);
void shadow_tag_warned(type_element elements, int warned);
declarator make_function_declarator(location l, declarator d, declaration parms, type_element quals);
bool start_function(type_element elements, declarator d, attribute attribs,
                    bool nested);
void store_parm_decls(declaration old_parms);
declaration finish_function(statement body);
extern function_decl current_function_decl;
void pushlevel(bool parm_level);
environment poplevel(void);
enum { var_typedef, var_register, var_normal, var_static, var_extern };
void split_type_elements(type_element tlist, type_element *declspecs,
                         attribute *attributes);
declaration start_decl(declarator d, asm_stmt astmt, type_element elements,
                       bool initialised, attribute extra_attributes,
                       attribute attributes);
declaration finish_decl(declaration decl, expression init);
declaration declare_parameter(declarator d, type_element elements,
                              attribute extra_attributes,
                              attribute attributes,
                              bool abstract);
void mark_forward_parameters(declaration parms);
declaration declare_old_parameter(location l, cstring id);
type_element start_struct(location l, ast_kind skind, word tag);
type_element finish_struct(type_element t, declaration fields,
                           attribute attribs);
type_element xref_tag(location l, ast_kind skind, word tag);
type_element start_enum(location l, word tag);
type_element finish_enum(type_element t, declaration names,
                         attribute attribs);
declaration make_field(declarator d, expression bitfield,
                       type_element elements, attribute extra_attributes,
                       attribute attributes);
declaration make_enumerator(location loc, cstring id, expression value);
asttype make_type(type_element elements, declarator d);
int save_directive(char *directive);
char *rid_name(rid r);
statement chain_with_labels(statement l1, statement l2);
const char *declarator_name(declarator d);
data_declaration lookup_id(const char *s, bool this_level_only);
extern function_decl current_function_decl;
data_declaration implicitly_declare(identifier fnid);
void push_label_level(void);
void pop_label_level(void);
void init_data_declaration(data_declaration dd, declaration ast,
                           const char *name, type t);
data_declaration declare(environment env, data_declaration from,
                         bool ignore_shadow);
data_declaration declare_string(const char *name, bool wide, size_t length);
environment new_environment(region r, environment parent, bool parm_level);
tag_declaration declare_tag(tag_ref t);
tag_declaration lookup_tag(tag_ref t, bool this_level_only);
tag_declaration declare_global_tag(tag_ref t);
tag_declaration lookup_global_tag(tag_ref t);
extern char *input_filename;
extern char *main_input_filename;
extern int lineno;
extern int filepos;
extern FILE *finput;
struct file_stack
  {
    char *name;
    struct file_stack *next;
    int line;
    int filepos;
    int indent_level;
  };
extern struct file_stack *input_file_stack;
extern int input_file_stack_tick;
void set_input(FILE *f, char *filename);
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
void check_condition(const char *context, expression e);
void check_switch(expression e);
void check_void_return(void);
void check_return(expression e);
void check_computed_goto(expression e);
void lookup_label(id_label label);
void use_label(id_label label);
void define_label(id_label label);
void declare_label(id_label label);
void check_labels(void);
void check_case(label case_label);
void check_default(label default_label);
void check_break(statement break_statement);
void check_continue(statement continue_statement);
void push_loop(breakable_stmt loop_statement);
void pop_loop(void);
label_declaration new_label_declaration(region r, const char *name, id_label firstuse);
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
extern data_declaration gcc_builtin_classify_type, builtin_memcpy, builtin_memcmp,
  builtin_strcpy, builtin_strcmp, builtin_strlen;
bool builtin_declaration(data_declaration dd);
void builtins_init(void);
type builtin_va_list_type;
data_declaration builtin_va_arg_decl;
function_decl current_function_decl;
data_declaration bad_decl;
static type implicit_function_type;
environment current_env;
static environment global_env;
static tag_ref pending_invalid_xref;
static known_cst onecst, zerocst;
static expression oneexpr;
environment new_environment(region r, environment parent, bool parm_level)
{
  environment env = (sizeof(struct environment) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((r), sizeof(struct environment)) : (((void)0), __rc_typed_ralloc)((r), sizeof(struct environment), 0));
  env->parent = parent;
  env->parm_level = parm_level;
  if (parent)
    {
      env->fdecl = parent->fdecl;
      env->id_env = new_env(r, parent->id_env);
      if (parent->parm_level)
        env->tag_env = parent->tag_env;
      else
        env->tag_env = new_env(r, parent->tag_env);
    }
  else
    {
      env->id_env = new_env(r, ((void *)0));
      env->tag_env = new_env(r, ((void *)0));
    }
  return env;
}
void init_data_declaration(data_declaration dd, declaration ast,
                           const char *name, type t)
{
  dd->kind = 0;
  dd->name = name;
  dd->type = t;
  dd->shadowed = ((void *)0);
  dd->global = ((void *)0);
  dd->ast = ast;
  dd->definition = ((void *)0);
  dd->isexternalscope = 0;
  dd->isfilescoperef = 0;
  dd->needsmemory = 0;
  dd->isused = 0;
  dd->in_system_header = ast->loc->in_system_header;
  dd->in_prelude = in_prelude;
  dd->defined = 0;
  dd->isallocated = 0;
  dd->addresstaken = 0;
  dd->ftype = 0;
  dd->isinline = 0;
  dd->isexterninline = 0;
  dd->__init = 0;
  dd->noreturn = 0;
  dd->alias = ((void *)0);
  dd->oldstyle_args = ((void *)0);
  dd->vtype = 0;
  dd->islocal = 0;
  dd->isparameter = 0;
  dd->islimbo = 0;
  dd->value = ((void *)0);
  dd->chars = ((void *)0);
  dd->chars_length = 0;
  dd->qtype = ((void *)0);
  dd->fs_qtype = ((void *)0);
  dd->alref = ((void *)0);
}
data_declaration lookup_id(const char *s, bool this_level_only)
{
  return env_lookup(current_env->id_env, s, this_level_only);
}
data_declaration lookup_global_id(const char *s)
{
  return env_lookup(global_env->id_env, s, 1);
}
data_declaration declare(environment b, data_declaration from,
                         bool ignore_shadow)
{
  data_declaration dd = (sizeof(struct data_declaration) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct data_declaration)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct data_declaration), 0));
  *dd = *from;
  if (dd->name)
    {
      if (flag_traditional && dd->isfilescoperef && !lookup_id(dd->name, 0))
        b = global_env;
      if (b != global_env)
        {
          data_declaration shadowed = lookup_id(dd->name, 0);
          if (shadowed && shadowed->islocal && !dd->isfilescoperef
              && !b->parm_level
              && b->parent->parm_level
              && env_lookup(b->parent->id_env, dd->name, 1))
            {
              if (shadowed->isparameter)
                pedwarn("declaration of `%s' shadows a parameter", dd->name);
              else
                pedwarn("declaration of `%s' shadows a symbol from the parameter list",
                         dd->name);
            }
          else if (warn_shadow && !ignore_shadow && !dd->isfilescoperef)
            {
              char *warnstring = 0;
              if (dd->isparameter
                  && b->parent->parm_level)
                ;
              else if (shadowed && shadowed->isparameter)
                warnstring = "declaration of `%s' shadows a parameter";
              else if (shadowed && shadowed->islocal)
                warnstring = "declaration of `%s' shadows previous local";
              else if (shadowed)
                warnstring = "declaration of `%s' shadows global declaration";
              if (warnstring)
                (error_shadow ? error : warning)(warnstring, dd->name);
            }
        }
      if (dd->isfilescoperef && b != global_env && !lookup_global_id(dd->name))
        {
          data_declaration limbodd = (sizeof(struct data_declaration) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct data_declaration)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct data_declaration), 0));
          *limbodd = *dd;
          limbodd->islimbo = 1;
          env_add(global_env->id_env, limbodd->name, limbodd);
          ((void) ((!dd->shadowed) ? 0 : (__assert_fail ("!dd->shadowed", "semantics.c", 203, __PRETTY_FUNCTION__), 0)));
          dd->shadowed = limbodd;
          if (dd->kind == decl_function && dd->ftype == function_implicit &&
              mesg_implicit_function_declaration &&
              !builtin_declaration(dd))
            warning_or_error(mesg_implicit_function_declaration != 2,
                             "implicit declaration of function `%s'", dd->name);
        }
      else if (dd->kind == decl_function && dd->ftype == function_implicit &&
               warn_traditional)
        warning("function `%s' was previously declared within a block",
                dd->name);
      if (warn_main && (b == global_env || dd->isfilescoperef) &&
          dd->kind != decl_function && !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (dd->name) && __builtin_constant_p ("main") && (__s1_len = strlen (dd->name), __s2_len = strlen ("main"), (!((size_t)(const void *)((dd->name) + 1) - (size_t)(const void *)(dd->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (dd->name), (__const char *) ("main"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (dd->name) && ((size_t)(const void *)((dd->name) + 1) - (size_t)(const void *)(dd->name) == 1) && (__s1_len = strlen (dd->name), __s1_len < 4) ? (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (dd->name))[0] - ((__const unsigned char *) (__const char *)("main"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (dd->name))[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("main"); register int __result = (((__const unsigned char *) (__const char *) (dd->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (dd->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) && (__s2_len = strlen ("main"), __s2_len < 4) ? (__builtin_constant_p (dd->name) && ((size_t)(const void *)((dd->name) + 1) - (size_t)(const void *)(dd->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (dd->name))[0] - ((__const unsigned char *) (__const char *)("main"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (dd->name))[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (dd->name))[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (dd->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("main"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; }))) : strcmp (dd->name, "main")))); }))
        warning_with_decl(dd->ast, "`main' is usually a function");
   }
  ((void) ((!dd->islimbo) ? 0 : (__assert_fail ("!dd->islimbo", "semantics.c", 232, __PRETTY_FUNCTION__), 0)));
  env_add(b->id_env, dd->name, dd);
  return dd;
}
tag_declaration declare_tag_env(environment env, tag_ref t)
{
  tag_declaration tdecl = (sizeof(struct tag_declaration) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct tag_declaration)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct tag_declaration), 0));
  const char *name = t->word1 ? t->word1->cstring.data : ((void *)0);
  tdecl->kind = t->kind;
  tdecl->name = name;
  tdecl->ast = t;
  if (name)
    {
      tdecl->shadowed = env_lookup(env->tag_env, name, 0);
      if (tdecl->shadowed && warn_shadow)
        (error_shadow ? error : warning)
          ("tag %s shadows enclosing struct/union/enum", name);
    }
  else
    tdecl->shadowed = ((void *)0);
  env_add(env->tag_env, name, tdecl);
  return tdecl;
}
tag_declaration lookup_tag_env(environment env, tag_ref t, bool this_level_only)
{
  tag_declaration found =
    env_lookup(env->tag_env, t->word1->cstring.data, this_level_only);
  if (found && found->kind != t->kind)
    pending_invalid_xref = t;
  return found;
}
tag_declaration declare_tag(tag_ref t)
{
  return declare_tag_env(current_env, t);
}
tag_declaration lookup_tag(tag_ref t, bool this_level_only)
{
  return lookup_tag_env(current_env, t, this_level_only);
}
tag_declaration declare_global_tag(tag_ref t)
{
  return declare_tag_env(global_env, t);
}
tag_declaration lookup_global_tag(tag_ref t)
{
  return lookup_tag_env(global_env, t, 1);
}
void shadow_tag(type_element elements)
{
  shadow_tag_warned(elements, 0);
}
void shadow_tag_warned(type_element elements, int warned)
{
  type_element elem;
  int found_tag = 0;
  pending_invalid_xref = 0;
  for (elem = (elements); elem; elem = ({ast_generic tEmPcast = (ast_generic)(elem->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 320, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }))
    {
      ast_kind code = elem->kind;
      if (code == kind_struct_ref || code == kind_union_ref ||
          code == kind_enum_ref)
        {
          tag_ref tag = ({ast_generic tEmPcast = (ast_generic)(elem); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)", "semantics.c", 327, __PRETTY_FUNCTION__), 0))); (tag_ref)(tEmPcast); });
          word name = tag->word1;
          found_tag++;
          if (name == 0)
            {
              if (warned != 1 && code != kind_enum_ref)
                {
                  pedwarn ("unnamed struct/union that defines no instances");
                  warned = 1;
                }
            }
          else
            {
              void *tagdecl = lookup_tag(tag, 1);
              if (tagdecl == 0)
                declare_tag(tag);
              else
                pending_xref_error();
            }
        }
      else
        {
          if (!warned && ! in_system_header)
            {
              warning("useless keyword or type name in empty declaration");
              warned = 2;
            }
        }
    }
  if (found_tag > 1)
    error("two types specified in one empty declaration");
  if (warned != 1)
    {
      if (found_tag == 0)
        pedwarn("empty declaration");
    }
}
void pending_xref_error(void)
{
  if (pending_invalid_xref != 0)
    {
      error_with_location(pending_invalid_xref->loc,
                          "`%s' defined as wrong kind of tag",
                          pending_invalid_xref->word1->cstring.data);
      pending_invalid_xref = 0;
    }
}
bool is_void_parms(declaration parms)
{
  data_decl dd;
  variable_decl vd;
  if (!parms || parms->next || !((parms)->kind >= kind_data_decl && (parms)->kind <= postkind_data_decl))
    return 0;
  dd = ({ast_generic tEmPcast = (ast_generic)(parms); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "semantics.c", 396, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
  vd = ({ast_generic tEmPcast = (ast_generic)(dd->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "semantics.c", 397, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
  ((void) ((!vd->next) ? 0 : (__assert_fail ("!vd->next", "semantics.c", 398, __PRETTY_FUNCTION__), 0)));
  return !vd->declarator && dd->modifiers && !dd->modifiers->next &&
    ((((dd->modifiers)->kind >= kind_rid && (dd->modifiers)->kind <= postkind_rid) && ({ast_generic tEmPcast = (ast_generic)(dd->modifiers); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "semantics.c", 401, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })->id == RID_VOID) ||
     (((dd->modifiers)->kind >= kind_typename && (dd->modifiers)->kind <= postkind_typename) && type_void(vd->declared_type)));
}
const char *tagkind_name(int tagkind)
{
  switch (tagkind)
    {
    case kind_struct_ref: return "struct";
    case kind_union_ref: return "union";
    case kind_enum_ref: return "enum";
    default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 412, __PRETTY_FUNCTION__), 0))); return ((void *)0);
    }
}
static void parmlist_tags_warning(environment parm_env)
{
  env_scanner scan_tags;
  const char *tagname;
  tag_declaration tagdecl;
  static bool already = 0;
  env_scan(parm_env->tag_env, &scan_tags);
  while (env_next(&scan_tags, &tagname, (void **)&tagdecl))
    {
      int kind = tagdecl->kind;
      const char *kindname = tagkind_name(kind);
      if (kind == kind_union_ref && !tagname && !pedantic)
        continue;
      if (tagname)
        warning("`%s %s' declared inside parameter list", kindname,
                tagname);
      else
        warning("anonymous %s declared inside parameter list", kindname);
      if (!already)
        {
          warning("its scope is only this definition or declaration,");
          warning("which is probably not what you want.");
          already = 1;
        }
    }
}
static type_quals parse_qualifiers(location l, type_element qlist,
                                   user_qual_list *user_quals);
typelist make_arg_types(bool definition, declaration parameters, bool *varargs,
                        type_quals *varargs_quals,
                        user_qual_list *varargs_user_quals)
{
  declaration parameter;
  typelist arg_types = new_typelist(parse_region);
  *varargs = 0;
  *varargs_quals = no_qualifiers;
  *varargs_user_quals = ((void *)0);
  if (!is_void_parms(parameters))
    for (parameter = (parameters); parameter; parameter = ({ast_generic tEmPcast = (ast_generic)(parameter->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 466, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
      if (((parameter)->kind >= kind_ellipsis_decl && (parameter)->kind <= postkind_ellipsis_decl))
        {
          *varargs = 1;
          *varargs_quals = parse_qualifiers(parameter->loc,
                                   ({ast_generic tEmPcast = (ast_generic)(parameter); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_ellipsis_decl && (tEmPcast)->kind <= postkind_ellipsis_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_ellipsis_decl && (tEmPcast)->kind <= postkind_ellipsis_decl)", "semantics.c", 471, __PRETTY_FUNCTION__), 0))); (ellipsis_decl)(tEmPcast); })->qualifiers,
                                            varargs_user_quals);
        }
      else if (((parameter)->kind >= kind_error_decl && (parameter)->kind <= postkind_error_decl))
        {
          *varargs = 1;
          arg_types = new_typelist(parse_region);
          typelist_append(arg_types, error_type);
          return arg_types;
        }
      else
        {
          data_decl dp = ({ast_generic tEmPcast = (ast_generic)(parameter); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "semantics.c", 486, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
          variable_decl vp = ({ast_generic tEmPcast = (ast_generic)(dp->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "semantics.c", 487, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
          ((void) ((!vp->next) ? 0 : (__assert_fail ("!vp->next", "semantics.c", 489, __PRETTY_FUNCTION__), 0)));
          if (!vp->ddecl->name && definition)
            error_with_decl(dp->decls, "parameter name omitted");
          typelist_append(arg_types, vp->ddecl->type);
        }
  return arg_types;
}
bool new_style(declaration parameters)
{
  return parameters && !((parameters)->kind >= kind_oldidentifier_decl && (parameters)->kind <= postkind_oldidentifier_decl);
}
static void check_duplicate_rid(int specbits, rid rspec)
{
  if (specbits & 1 << rspec->id)
    pedwarn_with_location(rspec->loc, "duplicate `%s'", rid_name(rspec));
}
static void check_duplicate_qualifiers1(location l, type_quals new1, type_quals old)
{
  if (old & new1)
    pedwarn_with_location(l, "duplicate `%s'", qualifier_name(new1));
}
static void check_duplicate_qualifiers(location l, type_quals new, type_quals old)
{
  type_quals qual;
  for (qual = 1; qual < last_qualifier; qual <<= 1)
    if (new & qual)
      check_duplicate_qualifiers1(l, qual, old);
}
static type_quals parse_qualifiers(location l, type_element qlist,
                                   user_qual_list *user_quals_out)
{
  type_element qual;
  type_quals tqs = no_qualifiers;
  user_qual_list user_quals = ((void *)0);
  for (qual = (qlist); qual; qual = ({ast_generic tEmPcast = (ast_generic)(qual->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 532, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }))
    {
      switch (qual->kind)
        {
        case kind_qualifier:
          {
            qualifier q = ({ast_generic tEmPcast = (ast_generic)(qual); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)", "semantics.c", 538, __PRETTY_FUNCTION__), 0))); (qualifier)(tEmPcast); });
            check_duplicate_qualifiers1(q->loc, q->id, tqs);
            tqs |= q->id;
          }
          break;
        case kind_user_qual:
          user_quals = new_user_qual_list(({ast_generic tEmPcast = (ast_generic)(qual); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_user_qual && (tEmPcast)->kind <= postkind_user_qual)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_user_qual && (tEmPcast)->kind <= postkind_user_qual)", "semantics.c", 544, __PRETTY_FUNCTION__), 0))); (user_qual)(tEmPcast); }),
                                          user_quals);
          break;
        default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 547, __PRETTY_FUNCTION__), 0)));
        }
    }
   *user_quals_out = user_quals;
   return tqs;
}
void parse_declarator(type_element modifiers, declarator d, bool bitfield,
                      int *oclass, bool *oinlinep, const char **oname,
                      type *ot, bool *owarn_defaulted_int,
                      function_declarator *ofunction_declarator)
{
  location loc = d ? d->loc : modifiers->loc;
  int specbits = 0, nclasses = 0;
  type_quals specquals = no_qualifiers;
  bool longlong = 0;
  const char *printname;
  type_element spec;
  type t = ((void *)0);
  bool modified;
  user_qual_list user_quals = ((void *)0);
  *owarn_defaulted_int = 0;
  *oname = declarator_name(d);
  printname = *oname ? *oname : "type name";
  *oclass = 0;
  for (spec = (modifiers); spec; spec = ({ast_generic tEmPcast = (ast_generic)(spec->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 577, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }))
    {
      type newtype = ((void *)0);
      switch (spec->kind)
        {
        case kind_rid:
          {
            rid rspec = ({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "semantics.c", 585, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); });
            int id = rspec->id;
            switch (id)
              {
              case RID_INT: newtype = int_type; break;
              case RID_CHAR: newtype = char_type; break;
              case RID_FLOAT: newtype = float_type; break;
              case RID_DOUBLE: newtype = double_type; break;
              case RID_VOID: newtype = void_type; break;
              case RID_AUTO: case RID_STATIC: case RID_EXTERN:
              case RID_REGISTER: case RID_TYPEDEF:
                *oclass = id;
                nclasses++;
                break;
              case RID_LONG:
                if (specbits & 1 << RID_LONG)
                  {
                    if (longlong)
                      error_with_location(spec->loc,
                                          "`long long long' is too long for GCC");
                    else
                      {
                        if (pedantic && !in_system_header)
                          pedwarn_with_location(spec->loc,
                                                "ANSI C does not support `long long'");
                        longlong = 1;
                      }
                    break;
                  }
              default:
                check_duplicate_rid(specbits, rspec);
                break;
              }
            specbits |= 1 << id;
            break;
          }
        case kind_qualifier:
          {
            qualifier q = ({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)", "semantics.c", 627, __PRETTY_FUNCTION__), 0))); (qualifier)(tEmPcast); });
            int id = q->id;
            check_duplicate_qualifiers1(loc, id, specquals);
            specquals |= id;
            break;
          }
        case kind_typename:
          newtype = ({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_typename && (tEmPcast)->kind <= postkind_typename)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_typename && (tEmPcast)->kind <= postkind_typename)", "semantics.c", 635, __PRETTY_FUNCTION__), 0))); (typename)(tEmPcast); })->ddecl->type;
          break;
        case kind_typeof_type:
          newtype = ({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_typeof_type && (tEmPcast)->kind <= postkind_typeof_type)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_typeof_type && (tEmPcast)->kind <= postkind_typeof_type)", "semantics.c", 638, __PRETTY_FUNCTION__), 0))); (typeof_type)(tEmPcast); })->asttype->type;
          break;
        case kind_typeof_expr:
          newtype = ({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_typeof_expr && (tEmPcast)->kind <= postkind_typeof_expr)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_typeof_expr && (tEmPcast)->kind <= postkind_typeof_expr)", "semantics.c", 641, __PRETTY_FUNCTION__), 0))); (typeof_expr)(tEmPcast); })->arg1->type;
          break;
        case kind_struct_ref: case kind_union_ref: case kind_enum_ref:
          newtype = make_tagged_type(({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)", "semantics.c", 644, __PRETTY_FUNCTION__), 0))); (tag_ref)(tEmPcast); })->tdecl);
          break;
        case kind_user_qual:
          user_quals = new_user_qual_list(({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_user_qual && (tEmPcast)->kind <= postkind_user_qual)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_user_qual && (tEmPcast)->kind <= postkind_user_qual)", "semantics.c", 647, __PRETTY_FUNCTION__), 0))); (user_qual)(tEmPcast); }), user_quals);
          break;
        case kind_type_variable:
          newtype = make_type_var(({ast_generic tEmPcast = (ast_generic)(spec); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_variable && (tEmPcast)->kind <= postkind_type_variable)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_variable && (tEmPcast)->kind <= postkind_type_variable)", "semantics.c", 650, __PRETTY_FUNCTION__), 0))); (type_variable)(tEmPcast); })->cstring);
          break;
        default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 652, __PRETTY_FUNCTION__), 0))); break;
        }
      if (newtype)
        {
          if (t)
            error_with_location(spec->loc,
               "two or more data types in declaration of `%s'", printname);
          else
            t = newtype;
        }
    }
  if ((specbits & 1 << RID_LONG) && !longlong && (specbits & 1 << RID_DOUBLE))
    {
      specbits &= ~(1 << RID_LONG);
      t = long_double_type;
    }
  modified = !!(specbits & (1 << RID_LONG | 1 << RID_SHORT |
                            1 << RID_SIGNED | 1 << RID_UNSIGNED));
  if (!t)
    {
      if (specbits & 1 << RID_COMPLEX)
        {
          if (!modified)
            {
              specbits |= 1 << RID_DOUBLE;
              t = double_type;
            }
          else
            {
              specbits |= 1 << RID_INT;
              t = int_type;
            }
        }
      else
        {
          if (!modified)
            *owarn_defaulted_int = 1;
          specbits |= 1 << RID_INT;
          t = int_type;
        }
    }
  if (nclasses > 1)
    error_with_location(loc, "multiple storage classes in declaration of `%s'", printname);
  if (modified)
    {
      int ok = 0;
      if ((specbits & 1 << RID_LONG) && (specbits & 1 << RID_SHORT))
        error_with_location(loc, "both long and short specified for `%s'", printname);
      else if ((specbits & 1 << RID_SHORT) && !type_integral(t))
        {
          static int already = 0;
          error_with_location(loc, "short invalid for `%s'", printname);
          if (!already && !pedantic)
            {
              error_with_location(loc, "short is only valid with integral types");
              already = 1;
            }
        }
      else if ((specbits & 1 << RID_LONG) && !type_integral(t))
        {
          static int already = 0;
          error_with_location(loc, "long invalid for `%s'", printname);
          if (!already && !pedantic)
            {
              error_with_location(loc, "long is only valid with integral types or double");
              already = 1;
            }
        }
      else if ((specbits & 1 << RID_SIGNED) && (specbits & 1 << RID_UNSIGNED))
        error_with_location(loc, "both signed and unsigned specified for `%s'", printname);
      else if (((specbits & 1 << RID_SIGNED) || (specbits & 1 << RID_UNSIGNED))
               && !(specbits & (1 << RID_INT | 1 << RID_CHAR)))
        error_with_location(loc, "signed or unsigned invalid for `%s'", printname);
      else
        ok = 1;
      if (! ok)
        {
          specbits &= ~(1 << RID_LONG | 1 << RID_SHORT
                        | 1 << RID_UNSIGNED | 1 << RID_SIGNED);
          longlong = 0;
        }
    }
  if ((specbits & 1 << RID_COMPLEX) && !(type_integral(t) || type_floating(t)))
    {
      error_with_location(loc, "complex invalid for `%s'", printname);
      specbits &= ~(1 << RID_COMPLEX);
    }
  if ((specbits & 1 << RID_UNSIGNED)
      || (bitfield && flag_traditional
          && (!explicit_flag_signed_bitfields || !flag_signed_bitfields))
      || (bitfield && !flag_signed_bitfields
          && ((specbits & 1 << RID_INT) || (specbits & 1 << RID_CHAR))
          && !(specbits & 1 << RID_SIGNED)))
    {
      if (longlong)
        t = unsigned_long_long_type;
      else if (specbits & 1 << RID_LONG)
        t = unsigned_long_type;
      else if (specbits & 1 << RID_SHORT)
        t = unsigned_short_type;
      else if (t == char_type)
        t = unsigned_char_type;
      else
        t = unsigned_int_type;
    }
  else if ((specbits & 1 << RID_SIGNED) && (specbits & 1 << RID_CHAR))
    t = signed_char_type;
  else if (longlong)
    t = long_long_type;
  else if (specbits & 1 << RID_LONG)
    t = long_type;
  else if (specbits & 1 << RID_SHORT)
    t = short_type;
  if (specbits & 1 << RID_COMPLEX)
    t = make_complex_type(t);
  check_duplicate_qualifiers(loc, specquals, type_qualifiers(t));
  specquals |= type_qualifiers(t);
  t = make_qualified_type(t, specquals, user_quals);
  *oinlinep = !!(specbits & 1 << RID_INLINE);
  if (pedantic && type_function(t) && (type_const(t) || type_volatile(t)) &&
      !in_system_header)
    pedwarn_with_location(loc, "ANSI C forbids const or volatile function types");
  while (d && d->kind != kind_identifier_declarator)
    {
      switch (d->kind)
        {
        case kind_array_declarator:
          {
            array_declarator ad = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)", "semantics.c", 815, __PRETTY_FUNCTION__), 0))); (array_declarator)(tEmPcast); });
            expression size = ad->arg1;
            d = ad->declarator;
            if (type_void(t))
              {
                error_with_location(d->loc,
                                    "declaration of `%s' as array of voids", printname);
                t = error_type;
              }
            if (type_function(t))
              {
                error_with_location(d->loc,
                                    "declaration of `%s' as array of functions", printname);
                t = error_type;
              }
            if (size && ((size)->kind >= kind_error_expr && (size)->kind <= postkind_error_expr))
              t = error_type;
            if (t == error_type)
              continue;
            if (size)
              {
                if (!type_integer(size->type))
                  {
                    error_with_location(d->loc,
                                        "size of array `%s' has non-integer type", printname);
                    size = oneexpr;
                  }
                if (pedantic && definite_zero(size))
                  pedwarn_with_location(d->loc,
                                        "ANSI C forbids zero-size array `%s'", printname);
                if (size->cst && cval_isinteger((size->cst)->cval))
                  {
                    if (pedantic)
                      constant_overflow_warning(size->cst);
                    if (cval_intcompare(size->cst->cval, cval_zero) < 0)
                      {
                        error_with_location(d->loc,
                                            "size of array `%s' is negative", printname);
                        size = oneexpr;
                      }
                  }
                else
                  {
                    if (!(current_function_decl || current_env->parm_level))
                      {
                        if (size->cst)
                          error_with_location(d->loc, "type size can't be explicitly evaluated");
                        else
                          error_with_location(d->loc, "variable-size type declared outside of any function");
                      }
                    else if (pedantic)
                      {
                        if (size->cst)
                          pedwarn_with_location(d->loc, "ANSI C forbids array `%s' whose size can't be evaluated", printname);
                        else
                          pedwarn_with_location(d->loc, "ANSI C forbids variable-size array `%s'", printname);
                      }
                    size->cst = ((void *)0);
                  }
              }
            t = make_array_type(t, size);
            break;
          }
        case kind_function_declarator:
          {
            function_declarator fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)", "semantics.c", 896, __PRETTY_FUNCTION__), 0))); (function_declarator)(tEmPcast); });
            user_qual_list user_quals;
            type_quals fnquals = parse_qualifiers(fd->loc, fd->qualifiers,
                                                  &user_quals);
            d = fd->declarator;
            if (ofunction_declarator)
              *ofunction_declarator = fd;
            if (t == error_type)
              t = int_type;
            if (type_function(t))
              {
                error_with_location(d->loc,
                                    "`%s' declared as function returning a function",
                      printname);
                t = int_type;
              }
            if (type_array(t))
              {
                error_with_location(d->loc,
                                    "`%s' declared as function returning an array",
                      printname);
                t = int_type;
              }
            if (flag_traditional && type_float(t))
              t = qualify_type1(double_type, t);
            if (new_style(fd->parms))
              {
                bool varargs;
                bool definition = ofunction_declarator &&
                  d && d->kind == kind_identifier_declarator;
                type_quals varargs_quals;
                user_qual_list varargs_user_quals;
                typelist argtypes = make_arg_types(definition, fd->parms,
                                                   &varargs, &varargs_quals,
                                                   &varargs_user_quals);
                t = make_function_type(t, argtypes, varargs,
                                       varargs_quals, varargs_user_quals,
                                       0);
              }
            else
              t = make_function_type(t, ((void *)0), 0, no_qualifiers, ((void *)0),1);
            t = make_qualified_type(t, fnquals, user_quals);
            break;
          }
        case kind_pointer_declarator:
          {
            pointer_declarator pd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)", "semantics.c", 956, __PRETTY_FUNCTION__), 0))); (pointer_declarator)(tEmPcast); });
            type_quals quals;
            user_qual_list user_quals;
            d = pd->declarator;
            quals = parse_qualifiers(pd->loc, pd->qualifiers,
                                     &user_quals);
            t = make_qualified_type(make_pointer_type(t),
                                    quals, user_quals);
            break;
          }
        default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 968, __PRETTY_FUNCTION__), 0)));
        }
    }
  *ot = t;
}
declarator make_function_declarator(location l, declarator d, declaration parms, type_element quals)
{
  declaration parm;
  environment penv = poplevel();
  function_declarator fd =
    new_function_declarator(parse_region, l, d, parms, quals, penv);
  if (new_style(parms) && !is_void_parms(parms))
    for (parm = (parms); parm; parm = ({ast_generic tEmPcast = (ast_generic)(parm->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 984, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
      if (!((parm)->kind >= kind_ellipsis_decl && (parm)->kind <= postkind_ellipsis_decl) && !((parm)->kind >= kind_error_decl && (parm)->kind <= postkind_error_decl))
        {
          variable_decl vp = ({ast_generic tEmPcast = (ast_generic)(({ast_generic tEmPcast = (ast_generic)(parm); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "semantics.c", 987, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); })->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "semantics.c", 987, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
          if (!vp->ddecl)
            {
              error_with_location(l, "parameter declared void");
              vp->ddecl = bad_decl;
            }
          else if (!vp->ddecl->isused)
            {
              const char *pname = vp->ddecl->name;
              if (!pname)
                pname = "((anonymous))";
              error_with_location(l, "parameter '%s' has just a forward declaration", pname);
            }
        }
  parmlist_tags_warning(penv);
  return ({ast_generic tEmPcast = (ast_generic)(fd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "semantics.c", 1007, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); });
}
static char *redeclaration_error_message(data_declaration newdecl,
                                         data_declaration olddecl,
                                         bool newinitialised)
{
  if (olddecl->islimbo)
    return 0;
  if (newdecl->kind == decl_typedef)
    {
      if (flag_traditional && type_compatible(newdecl->type, olddecl->type))
        return 0;
      if (olddecl->in_system_header || newdecl->in_system_header)
        return 0;
      return "redefinition of `%s'";
    }
  else if (newdecl->kind == decl_function)
    {
      if (olddecl->definition && newdecl->definition
          && !(olddecl->isexterninline && !newdecl->isexterninline))
        return "redefinition of `%s'";
      return 0;
    }
  else if (newdecl->kind == decl_constant)
    return "redefinition of `%s'";
  else if (current_env == global_env)
    {
      if (newdecl->isfilescoperef || olddecl->isfilescoperef)
        return 0;
      if (newinitialised && olddecl->initialiser)
        return "redefinition of `%s'";
      if (olddecl->isexternalscope && !newdecl->isexternalscope)
        return "conflicting declarations of `%s'";
      return 0;
    }
  else
    {
      if (!(newdecl->isexternalscope && olddecl->isexternalscope))
        return "redeclaration of `%s'";
      return 0;
    }
}
static bool looks_like_malloc_redeclaration(type t1, type t2)
{
  type t1return = type_function_return_type(t1);
  type t2return = type_function_return_type(t2);
  return
    type_function_oldstyle(t1) &&
    type_pointer(t1return) && type_pointer(t2return) &&
    type_void(type_points_to(t2return)) &&
    type_char(type_points_to(t1return)) &&
    self_promoting_args(t2);
}
void show_previous_decl(void (*message)(declaration d, const char *format, ...),
                        data_declaration olddecl)
{
  if (olddecl->kind == decl_function && olddecl->ftype == function_implicit)
    message(olddecl->ast, "previous implicit declaration of `%s'", olddecl->name);
  else
    message(olddecl->ast, "previous declaration of `%s'", olddecl->name);
}
static int duplicate_decls(data_declaration newdecl, data_declaration olddecl,
                           bool different_binding_level, bool newinitialised)
{
  type oldtype = olddecl->type;
  type newtype = newdecl->type;
  char *errmsg = 0;
  const char *name = olddecl->name;
  void (*previous_message)(declaration d, const char *format, ...) = ((void *)0);
  bool types_match;
  ((void) ((!(newdecl->kind == decl_function && newdecl->ftype == function_implicit)) ? 0 : (__assert_fail ("!(newdecl->kind == decl_function && newdecl->ftype == function_implicit)", "semantics.c", 1124, __PRETTY_FUNCTION__), 0)));
  if (newdecl->kind != olddecl->kind)
    {
      bool iswarning =
        (flag_traditional && different_binding_level) || olddecl->islimbo;
      warning_or_error(iswarning,
                       "`%s' redeclared as different kind of symbol", name);
      show_previous_decl(iswarning ? warning_with_decl : error_with_decl, olddecl);
      newdecl->shadowed = olddecl;
      return 0;
    }
  if (newtype == error_type || oldtype == error_type)
    types_match = 0;
  else
    types_match = type_compatible_unqualified(newtype, oldtype);
  if ((oldtype == void_type || types_match) && newdecl->isparameter &&
      !olddecl->isused)
    return 1;
  if (flag_traditional && olddecl->kind == decl_function
      && olddecl->ftype == function_implicit)
    ;
  else if (!types_match && olddecl->kind == decl_function
           && (olddecl->in_system_header || newdecl->in_system_header)
           && (looks_like_malloc_redeclaration(oldtype, newtype) ||
               looks_like_malloc_redeclaration(newtype, oldtype)))
    {
      if (pedantic)
        pedwarn_with_decl(newdecl->ast, "conflicting types for `%s'", name);
      if (type_void(type_points_to(type_function_return_type(oldtype))))
        newdecl->type = newtype = oldtype;
      newdecl->in_system_header = 1;
    }
  else if (!types_match
           && !(olddecl->kind == decl_function && !pedantic
                && type_function_oldstyle(newtype)
                && type_compatible(type_function_return_type(oldtype),
                                   type_function_return_type(newtype))))
    {
      void (*message)(const char *format, ...) =
        olddecl->islimbo ? warning : error;
      previous_message =
        olddecl->islimbo ? warning_with_decl : error_with_decl;
      message("conflicting types for `%s'", name);
      if (newdecl->kind == decl_function
          && type_compatible(type_function_return_type(oldtype),
                             type_function_return_type(newtype))
          && ((type_function_oldstyle(oldtype) && !olddecl->definition)
              || (type_function_oldstyle(newtype) && !newdecl->definition)))
        {
          if (type_function_varargs(newtype) || type_function_varargs(oldtype))
            {
              message("A parameter list with an ellipsis can't match");
              message("an empty parameter name list declaration.");
            }
          else
            {
              typelist_scanner scanargs;
              type t;
              typelist args = type_function_arguments(oldtype);
              if (!args)
                args = type_function_arguments(newtype);
              typelist_scan(args, &scanargs);
              while ((t = typelist_next(&scanargs)))
                if (!type_self_promoting(t))
                  {
                    message("An argument type that has a default promotion");
                    message("can't match an empty parameter name list declaration.");
                    break;
                  }
            }
        }
    }
  else
    {
      errmsg = redeclaration_error_message(newdecl, olddecl, newinitialised);
      if (errmsg)
        {
          error_with_decl(newdecl->ast, errmsg, name);
          previous_message = error_with_decl;
        }
      else if (newdecl->kind == decl_typedef &&
               (olddecl->in_system_header || newdecl->in_system_header))
        {
          warning_with_decl(newdecl->ast, "redefinition of `%s'");
          previous_message = warning_with_decl;
        }
      else if (olddecl->kind == decl_function
               && olddecl->oldstyle_args
               && !type_function_oldstyle(newtype))
        {
          int nargs;
          typelist_scanner oldparms, newparms;
          typelist_scan(olddecl->oldstyle_args, &oldparms);
          typelist_scan(type_function_arguments(newtype), &newparms);
          nargs = 1;
          errmsg = ((void *)0);
          for (;;)
            {
              type oldparm = typelist_next(&oldparms);
              type newparm = typelist_next(&newparms);
              if (!oldparm && !newparm)
                break;
              if (!oldparm || !newparm)
                {
                  errmsg = "prototype for `%s' follows and number of arguments";
                  break;
                }
              if (!type_compatible(oldparm, newparm)
                  && !(flag_traditional
                       && type_equal_unqualified(oldparm, int_type)
                       && type_equal_unqualified(newparm, unsigned_int_type)))
                {
                  errmsg = "prototype for `%s' follows and argument %d";
                  break;
                }
              nargs++;
            }
          if (errmsg)
            {
              warning_or_error_with_decl(olddecl->islimbo, newdecl->ast,
                                         errmsg, name, nargs);
              warning_or_error_with_decl(olddecl->islimbo, olddecl->ast,
                              "doesn't match non-prototype definition here");
            }
          else
            {
              warning_with_decl(newdecl->ast, "prototype for `%s' follows",
                                name);
              warning_with_decl(olddecl->ast, "non-prototype definition here");
            }
        }
      else if (newdecl->kind == decl_function)
        {
          if (!olddecl->isinline && newdecl->isinline)
            {
              if (olddecl->isused)
                {
                  warning("`%s' declared inline after being called", name);
                  previous_message = warning_with_decl;
                }
              if (olddecl->definition)
                {
                  warning("`%s' declared inline after its definition", name);
                  previous_message = warning_with_decl;
                }
            }
          if (newdecl->ftype == function_static &&
              olddecl->ftype == function_implicit)
            {
              pedwarn("`%s' was declared implicitly `extern' and later `static'", name);
              previous_message = pedwarn_with_decl;
            }
          else if (newdecl->ftype == function_static &&
                   olddecl->ftype == function_normal)
            {
              pedwarn("static declaration for `%s' follows non-static", name);
              previous_message = pedwarn_with_decl;
            }
        }
      else if (newdecl->kind == decl_variable)
        {
          if (pedantic &&
              olddecl->isexternalscope && !newdecl->isexternalscope)
            {
              pedwarn("static declaration for `%s' follows non-static", name);
              previous_message = pedwarn_with_decl;
            }
          if (!type_const(oldtype) && type_const(newtype))
            warning("const declaration for `%s' follows non-const", name);
          else if (pedantic &&
                   (type_const(oldtype) != type_const(newtype)
                    || type_volatile(oldtype) != type_volatile(newtype)))
            {
              pedwarn("type qualifiers for `%s' conflict with previous decl",
                      name);
              previous_message = pedwarn_with_decl;
            }
        }
    }
  if (errmsg == 0 && warn_redundant_decls && !olddecl->islimbo &&
      !(olddecl->isfilescoperef && !newdecl->isfilescoperef))
    {
      warning_with_decl(newdecl->ast, "redundant redeclaration of `%s' in same scope", name);
      previous_message = warning_with_decl;
    }
  if (previous_message)
    show_previous_decl(previous_message, olddecl);
  if (newdecl->isinline && !olddecl->definition)
    olddecl->isinline = 1;
  newdecl->isinline = olddecl->isinline;
  if (different_binding_level)
    {
      ((void) ((newdecl->isfilescoperef && !newdecl->needsmemory) ? 0 : (__assert_fail ("newdecl->isfilescoperef && !newdecl->needsmemory", "semantics.c", 1380, __PRETTY_FUNCTION__), 0)));
      ((void) ((!(newdecl->kind == decl_variable && newdecl->vtype == variable_static)) ? 0 : (__assert_fail ("!(newdecl->kind == decl_variable && newdecl->vtype == variable_static)", "semantics.c", 1381, __PRETTY_FUNCTION__), 0)));
      ((void) ((!(newdecl->kind == decl_function && (newdecl->ftype == function_implicit || newdecl->ftype == function_nested))) ? 0 : (__assert_fail ("!(newdecl->kind == decl_function && (newdecl->ftype == function_implicit || newdecl->ftype == function_nested))", "semantics.c", 1383, __PRETTY_FUNCTION__), 0)));
      newdecl->shadowed = olddecl;
      newdecl->definition = olddecl->definition;
      newdecl->defined = olddecl->defined;
      newdecl->isinline = olddecl->isinline;
      newdecl->isexterninline = olddecl->isexterninline;
      newdecl->oldstyle_args = olddecl->oldstyle_args;
      if (olddecl->in_system_header)
        newdecl->in_system_header = 1;
      newdecl->isexternalscope = olddecl->isexternalscope;
      return 0;
    }
  ((void) ((!newdecl->oldstyle_args) ? 0 : (__assert_fail ("!newdecl->oldstyle_args", "semantics.c", 1405, __PRETTY_FUNCTION__), 0)));
  if (types_match)
    olddecl->type = common_type(newtype, oldtype);
  else if (newtype != error_type)
    olddecl->type = newtype;
  olddecl->islimbo = 0;
  if (newdecl->definition)
    {
      olddecl->definition = newdecl->definition;
      olddecl->defined = newdecl->defined;
    }
  olddecl->isexternalscope &= newdecl->isexternalscope;
  olddecl->isfilescoperef &= newdecl->isfilescoperef;
  olddecl->needsmemory |= newdecl->needsmemory;
  olddecl->noreturn |= newdecl->noreturn;
  olddecl->__init |= newdecl->__init;
  if (olddecl->alias == ((void *)0))
    olddecl->alias = newdecl->alias;
  ((void) ((newdecl->alias == ((void *)0) || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (olddecl->alias) && __builtin_constant_p (newdecl->alias) && (__s1_len = strlen (olddecl->alias), __s2_len = strlen (newdecl->alias), (!((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (olddecl->alias), (__const char *) (newdecl->alias), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (olddecl->alias) && ((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) && (__s1_len = strlen (olddecl->alias), __s1_len < 4) ? (__builtin_constant_p (newdecl->alias) && ((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - ((__const unsigned char *) (__const char *)(newdecl->alias))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (newdecl->alias); register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (newdecl->alias) && ((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) && (__s2_len = strlen (newdecl->alias), __s2_len < 4) ? (__builtin_constant_p (olddecl->alias) && ((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - ((__const unsigned char *) (__const char *)(newdecl->alias))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (olddecl->alias); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (newdecl->alias))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; }))) : strcmp (olddecl->alias, newdecl->alias)))); })) ? 0 : (__assert_fail ("newdecl->alias == ((void *)0) || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (olddecl->alias) && __builtin_constant_p (newdecl->alias) && (__s1_len = strlen (olddecl->alias), __s2_len = strlen (newdecl->alias), (!((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (olddecl->alias), (__const char *) (newdecl->alias), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (olddecl->alias) && ((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) && (__s1_len = strlen (olddecl->alias), __s1_len < 4) ? (__builtin_constant_p (newdecl->alias) && ((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - ((__const unsigned char *) (__const char *)(newdecl->alias))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (newdecl->alias); register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (newdecl->alias) && ((size_t)(const void *)((newdecl->alias) + 1) - (size_t)(const void *)(newdecl->alias) == 1) && (__s2_len = strlen (newdecl->alias), __s2_len < 4) ? (__builtin_constant_p (olddecl->alias) && ((size_t)(const void *)((olddecl->alias) + 1) - (size_t)(const void *)(olddecl->alias) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[0] - ((__const unsigned char *) (__const char *)(newdecl->alias))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (olddecl->alias))[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (olddecl->alias); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (newdecl->alias))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (newdecl->alias))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (newdecl->alias))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (newdecl->alias))[3]); } } __result; }))) : strcmp (olddecl->alias, newdecl->alias)))); })", "semantics.c", 1430, __PRETTY_FUNCTION__), 0)));
  if (newdecl->kind == decl_function)
    {
      if (olddecl->ftype != function_static)
        olddecl->ftype = newdecl->ftype;
      if ((olddecl->definition && !olddecl->isexterninline) ||
          (newdecl->definition && !newdecl->isexterninline))
        olddecl->isexterninline = 0;
      else if (olddecl->isexterninline || newdecl->isexterninline)
        olddecl->isexterninline = 1;
    }
  else if (newdecl->kind == decl_variable)
    {
      if (olddecl->vtype != variable_static)
        olddecl->vtype = variable_static;
    }
  olddecl->in_system_header = newdecl->in_system_header =
    olddecl->in_system_header || newdecl->in_system_header;
  olddecl->ast = newdecl->ast;
  return 1;
}
void ignored_attribute(attribute attr)
{
  warning_with_location(attr->loc, "`%s' attribute directive ignored",
                        attr->word1->cstring.data);
}
static void transparent_union_argument(data_declaration ddecl)
{
  ddecl->type = make_qualified_type
    (ddecl->type, type_qualifiers(ddecl->type) | transparent_qualifier, ((void *)0));
}
void handle_attribute(attribute attr, data_declaration ddecl,
                      field_declaration fdecl, tag_declaration tdecl)
{
  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("transparent_union") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("transparent_union"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("transparent_union") + 1) - (size_t)(const void *)("transparent_union") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("transparent_union"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("transparent_union") && ((size_t)(const void *)(("transparent_union") + 1) - (size_t)(const void *)("transparent_union") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("transparent_union"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("transparent_union"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("transparent_union"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("transparent_union"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("transparent_union"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("transparent_union") && ((size_t)(const void *)(("transparent_union") + 1) - (size_t)(const void *)("transparent_union") == 1) && (__s2_len = strlen ("transparent_union"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("transparent_union"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("transparent_union"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("transparent_union"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("transparent_union"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("transparent_union"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("transparent_union"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("transparent_union"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("transparent_union"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "transparent_union")))); }) ||
      !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("__transparent_union__") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("__transparent_union__"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__transparent_union__") + 1) - (size_t)(const void *)("__transparent_union__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("__transparent_union__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("__transparent_union__") && ((size_t)(const void *)(("__transparent_union__") + 1) - (size_t)(const void *)("__transparent_union__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__transparent_union__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__transparent_union__"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__transparent_union__") && ((size_t)(const void *)(("__transparent_union__") + 1) - (size_t)(const void *)("__transparent_union__") == 1) && (__s2_len = strlen ("__transparent_union__"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__transparent_union__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__transparent_union__"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "__transparent_union__")))); }))
    {
      if (attr->word2 || attr->args)
        error_with_location(attr->loc, "wrong number of arguments specified for `transparent_union' attribute");
      if (ddecl && ddecl->kind == decl_variable && ddecl->isparameter &&
          type_union(ddecl->type))
        transparent_union_argument(ddecl);
      else if (ddecl && ddecl->kind == decl_typedef && type_union(ddecl->type))
        transparent_union_argument(ddecl);
      else if (tdecl && tdecl->kind == kind_union_ref)
        {
          tdecl->transparent_union = 1;
        }
      else
        ignored_attribute(attr);
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("packed") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("packed"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("packed") + 1) - (size_t)(const void *)("packed") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("packed"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("packed") && ((size_t)(const void *)(("packed") + 1) - (size_t)(const void *)("packed") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("packed"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("packed"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("packed"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("packed"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("packed"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("packed") && ((size_t)(const void *)(("packed") + 1) - (size_t)(const void *)("packed") == 1) && (__s2_len = strlen ("packed"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("packed"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("packed"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("packed"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("packed"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("packed"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("packed"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("packed"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("packed"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "packed")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("__packed__") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("__packed__"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__packed__") + 1) - (size_t)(const void *)("__packed__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("__packed__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("__packed__") && ((size_t)(const void *)(("__packed__") + 1) - (size_t)(const void *)("__packed__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__packed__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__packed__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__packed__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__packed__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__packed__"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__packed__") && ((size_t)(const void *)(("__packed__") + 1) - (size_t)(const void *)("__packed__") == 1) && (__s2_len = strlen ("__packed__"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__packed__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__packed__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__packed__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__packed__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__packed__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__packed__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__packed__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__packed__"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "__packed__")))); }))
    {
      if (tdecl)
        tdecl->packed = 1;
      else if (fdecl)
        fdecl->packed = 1;
      else
        ignored_attribute(attr);
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("noreturn") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("noreturn"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("noreturn"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("noreturn") && ((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("noreturn"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("noreturn"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("noreturn") && ((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) && (__s2_len = strlen ("noreturn"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("noreturn"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("noreturn"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "noreturn")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("__noreturn__") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("__noreturn__"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__noreturn__") + 1) - (size_t)(const void *)("__noreturn__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("__noreturn__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("__noreturn__") && ((size_t)(const void *)(("__noreturn__") + 1) - (size_t)(const void *)("__noreturn__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__noreturn__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__noreturn__"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__noreturn__") && ((size_t)(const void *)(("__noreturn__") + 1) - (size_t)(const void *)("__noreturn__") == 1) && (__s2_len = strlen ("__noreturn__"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__noreturn__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__noreturn__"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "__noreturn__")))); }))
    {
      if (ddecl)
        ddecl->noreturn = 1;
      else
        ignored_attribute(attr);
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("alias") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("alias"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("alias") + 1) - (size_t)(const void *)("alias") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("alias"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("alias") && ((size_t)(const void *)(("alias") + 1) - (size_t)(const void *)("alias") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("alias"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("alias"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("alias"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("alias"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("alias"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("alias") && ((size_t)(const void *)(("alias") + 1) - (size_t)(const void *)("alias") == 1) && (__s2_len = strlen ("alias"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("alias"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("alias"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("alias"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("alias"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("alias"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("alias"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("alias"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("alias"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "alias")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("__alias__") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("__alias__"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__alias__") + 1) - (size_t)(const void *)("__alias__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("__alias__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("__alias__") && ((size_t)(const void *)(("__alias__") + 1) - (size_t)(const void *)("__alias__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__alias__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__alias__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__alias__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__alias__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__alias__"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__alias__") && ((size_t)(const void *)(("__alias__") + 1) - (size_t)(const void *)("__alias__") == 1) && (__s2_len = strlen ("__alias__"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__alias__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__alias__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__alias__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__alias__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__alias__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__alias__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__alias__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__alias__"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "__alias__")))); }))
    {
      if (attr->args && ((attr->args)->kind >= kind_string && (attr->args)->kind <= postkind_string))
        {
          string str = (string)attr->args;
          ddecl->alias = (((void)0), __rc_rstralloc)(parse_region, str->ddecl->chars_length);
          wcstombs(ddecl->alias, str->ddecl->chars, str->ddecl->chars_length);
        }
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("section") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("section"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("section") + 1) - (size_t)(const void *)("section") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("section"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("section") && ((size_t)(const void *)(("section") + 1) - (size_t)(const void *)("section") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("section"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("section"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("section"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("section"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("section"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("section") && ((size_t)(const void *)(("section") + 1) - (size_t)(const void *)("section") == 1) && (__s2_len = strlen ("section"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("section"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("section"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("section"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("section"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("section"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("section"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("section"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("section"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "section")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (attr->word1->cstring.data) && __builtin_constant_p ("__section__") && (__s1_len = strlen (attr->word1->cstring.data), __s2_len = strlen ("__section__"), (!((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__section__") + 1) - (size_t)(const void *)("__section__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (attr->word1->cstring.data), (__const char *) ("__section__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) && (__s1_len = strlen (attr->word1->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("__section__") && ((size_t)(const void *)(("__section__") + 1) - (size_t)(const void *)("__section__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__section__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__section__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__section__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__section__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__section__"); register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__section__") && ((size_t)(const void *)(("__section__") + 1) - (size_t)(const void *)("__section__") == 1) && (__s2_len = strlen ("__section__"), __s2_len < 4) ? (__builtin_constant_p (attr->word1->cstring.data) && ((size_t)(const void *)((attr->word1->cstring.data) + 1) - (size_t)(const void *)(attr->word1->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[0] - ((__const unsigned char *) (__const char *)("__section__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("__section__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("__section__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (attr->word1->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("__section__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (attr->word1->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__section__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__section__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__section__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__section__"))[3]); } } __result; }))) : strcmp (attr->word1->cstring.data, "__section__")))); }))
    {
      if (attr->args && !attr->args->next &&
          attr->args->kind == kind_string)
        {
          const char *s = string_to_charp(parse_region, ({ast_generic tEmPcast = (ast_generic)(attr->args); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_string && (tEmPcast)->kind <= postkind_string)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_string && (tEmPcast)->kind <= postkind_string)", "semantics.c", 1532, __PRETTY_FUNCTION__), 0))); (string)(tEmPcast); }));
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (s) && __builtin_constant_p (".text.init") && (__s1_len = strlen (s), __s2_len = strlen (".text.init"), (!((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((".text.init") + 1) - (size_t)(const void *)(".text.init") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (s), (__const char *) (".text.init"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) && (__s1_len = strlen (s), __s1_len < 4) ? (__builtin_constant_p (".text.init") && ((size_t)(const void *)((".text.init") + 1) - (size_t)(const void *)(".text.init") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".text.init"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".text.init"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".text.init"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".text.init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (".text.init"); register int __result = (((__const unsigned char *) (__const char *) (s))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (".text.init") && ((size_t)(const void *)((".text.init") + 1) - (size_t)(const void *)(".text.init") == 1) && (__s2_len = strlen (".text.init"), __s2_len < 4) ? (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".text.init"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".text.init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".text.init"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".text.init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (s); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (".text.init"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (".text.init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (".text.init"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (".text.init"))[3]); } } __result; }))) : strcmp (s, ".text.init")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (s) && __builtin_constant_p (".data.init") && (__s1_len = strlen (s), __s2_len = strlen (".data.init"), (!((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((".data.init") + 1) - (size_t)(const void *)(".data.init") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (s), (__const char *) (".data.init"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) && (__s1_len = strlen (s), __s1_len < 4) ? (__builtin_constant_p (".data.init") && ((size_t)(const void *)((".data.init") + 1) - (size_t)(const void *)(".data.init") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".data.init"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".data.init"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".data.init"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".data.init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (".data.init"); register int __result = (((__const unsigned char *) (__const char *) (s))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (".data.init") && ((size_t)(const void *)((".data.init") + 1) - (size_t)(const void *)(".data.init") == 1) && (__s2_len = strlen (".data.init"), __s2_len < 4) ? (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".data.init"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".data.init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".data.init"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".data.init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (s); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (".data.init"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (".data.init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (".data.init"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (".data.init"))[3]); } } __result; }))) : strcmp (s, ".data.init")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (s) && __builtin_constant_p (".init.text") && (__s1_len = strlen (s), __s2_len = strlen (".init.text"), (!((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((".init.text") + 1) - (size_t)(const void *)(".init.text") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (s), (__const char *) (".init.text"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) && (__s1_len = strlen (s), __s1_len < 4) ? (__builtin_constant_p (".init.text") && ((size_t)(const void *)((".init.text") + 1) - (size_t)(const void *)(".init.text") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".init.text"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".init.text"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".init.text"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".init.text"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (".init.text"); register int __result = (((__const unsigned char *) (__const char *) (s))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (".init.text") && ((size_t)(const void *)((".init.text") + 1) - (size_t)(const void *)(".init.text") == 1) && (__s2_len = strlen (".init.text"), __s2_len < 4) ? (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".init.text"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".init.text"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".init.text"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".init.text"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (s); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (".init.text"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (".init.text"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (".init.text"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (".init.text"))[3]); } } __result; }))) : strcmp (s, ".init.text")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (s) && __builtin_constant_p (".init.data") && (__s1_len = strlen (s), __s2_len = strlen (".init.data"), (!((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((".init.data") + 1) - (size_t)(const void *)(".init.data") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (s), (__const char *) (".init.data"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) && (__s1_len = strlen (s), __s1_len < 4) ? (__builtin_constant_p (".init.data") && ((size_t)(const void *)((".init.data") + 1) - (size_t)(const void *)(".init.data") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".init.data"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".init.data"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".init.data"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".init.data"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (".init.data"); register int __result = (((__const unsigned char *) (__const char *) (s))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (".init.data") && ((size_t)(const void *)((".init.data") + 1) - (size_t)(const void *)(".init.data") == 1) && (__s2_len = strlen (".init.data"), __s2_len < 4) ? (__builtin_constant_p (s) && ((size_t)(const void *)((s) + 1) - (size_t)(const void *)(s) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (s))[0] - ((__const unsigned char *) (__const char *)(".init.data"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[1] - ((__const unsigned char *) (__const char *) (".init.data"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (s))[2] - ((__const unsigned char *) (__const char *) (".init.data"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (s))[3] - ((__const unsigned char *) (__const char *) (".init.data"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (s); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (".init.data"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (".init.data"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (".init.data"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (".init.data"))[3]); } } __result; }))) : strcmp (s, ".init.data")))); }))
            {
              if (ddecl)
                ddecl->__init = 1;
              else
                ignored_attribute(attr);
          }
        }
    }
}
void handle_attributes(attribute alist, data_declaration ddecl,
                       field_declaration fdecl, tag_declaration tdecl)
{
  for (alist = (alist); alist; alist = ({ast_generic tEmPcast = (ast_generic)(alist->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)", "semantics.c", 1551, __PRETTY_FUNCTION__), 0))); (attribute)(tEmPcast); }))
    handle_attribute(alist, ddecl, fdecl, tdecl);
}
void ignored_attributes(attribute alist)
{
  for (alist = (alist); alist; alist = ({ast_generic tEmPcast = (ast_generic)(alist->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)", "semantics.c", 1557, __PRETTY_FUNCTION__), 0))); (attribute)(tEmPcast); }))
    ignored_attribute(alist);
}
bool is_doublecharstar(type t)
{
  if (!type_pointer(t))
    return 0;
  t = type_points_to(t);
  if (!type_pointer(t))
    return 0;
  return type_equal_unqualified(type_points_to(t), char_type);
}
void check_function(data_declaration dd, declaration fd, int class,
                    bool inlinep, const char *name, type function_type,
                    bool nested, bool isdeclaration, bool defaulted_int)
{
  type return_type;
  if (defaulted_int && (warn_implicit_int || warn_return_type))
    warning("return-type defaults to `int'");
  return_type = type_function_return_type(function_type);
  if (pedantic && type_void(return_type) &&
      (type_const(return_type) || type_volatile(return_type)) &&
      !in_system_header)
    pedwarn("ANSI C forbids const or volatile void function return type");
  if (type_volatile(function_type) && !type_void(return_type))
    warning("`noreturn' function returns non-void value");
  if (inlinep && !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("main") && (__s1_len = strlen (name), __s2_len = strlen ("main"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("main"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("main"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("main"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) && (__s2_len = strlen ("main"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("main"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("main"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("main"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("main"))[3]); } } __result; }))) : strcmp (name, "main")))); }) && !nested)
    {
      warning("cannot inline function `main'");
      inlinep = 0;
    }
  if (warn_main && !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("main") && __builtin_constant_p (name) && (__s1_len = strlen ("main"), __s2_len = strlen (name), (!((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("main"), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) && (__s1_len = strlen ("main"), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("main"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp ("main", name)))); }) && !nested)
    {
      if (!type_equal_unqualified(return_type, int_type))
        pedwarn("return type of `%s' is not `int'", name);
      if (!type_function_oldstyle(function_type))
        {
          typelist_scanner scanargs;
          type argtype;
          int argct = 0;
          typelist_scan(type_function_arguments(function_type), &scanargs);
          while ((argtype = typelist_next(&scanargs)))
            {
              ++argct;
              switch (argct)
                {
                case 1:
                  if (!type_equal_unqualified(argtype, int_type))
                    pedwarn("first argument of `%s' should be `int'", name);
                  break;
                case 2:
                  if (!is_doublecharstar(argtype))
                    pedwarn("second argument of `%s' should be `char **'",
                            name);
                  break;
                case 3:
                  if (!is_doublecharstar(argtype))
                    pedwarn("third argument of `%s' should probably be `char **'",
                            name);
                  break;
                }
            }
          if (argct > 0 && (argct < 2 || argct > 3))
            pedwarn("`%s' takes only zero or two arguments", name);
          if (argct == 3 && pedantic)
            pedwarn("third argument of `%s' is deprecated", name);
          if (class == RID_STATIC)
            pedwarn("`%s' is normally a non-static function", name);
        }
    }
  init_data_declaration(dd, fd, name, function_type);
  dd->kind = decl_function;
  dd->isexternalscope = 0;
  if (nested)
    dd->ftype = function_nested;
  else if (class == RID_STATIC)
    dd->ftype = function_static;
  else
    {
      dd->ftype = function_normal;
      dd->isexternalscope = 1;
    }
  dd->needsmemory = !isdeclaration;
  dd->isinline = inlinep;
  dd->isexterninline = inlinep && class == RID_EXTERN;
  dd->isfilescoperef = dd->isexterninline || isdeclaration;
}
data_declaration declare_string(const char *name, bool wide, size_t length)
{
  struct data_declaration tempdecl;
  expression expr_l = build_uint_constant(parse_region, dummy_location, size_t_type, length);
  type value_type = make_array_type(wide ? wchar_type : char_type, expr_l);
  init_data_declaration(&tempdecl, new_error_decl(parse_region, dummy_location),
                        name, value_type);
  tempdecl.kind = decl_magic_string;
  tempdecl.needsmemory = 1;
  tempdecl.in_system_header = 1;
  tempdecl.vtype = variable_static;
  tempdecl.chars_length = length;
  tempdecl.chars = (((void)0), __rc_typed_rarrayalloc)((parse_region), (length), sizeof(wchar_t), 0);
  return declare(current_env, &tempdecl, 1);
}
static void declare_magic_string(const char *name, const char *value)
{
  int i, l = strlen(value);
  data_declaration dd = declare_string(name, 0, l);
  wchar_t *chars = (wchar_t *)dd->chars;
  for (i = 0; i < l; i++)
    chars[i] = value[i];
}
static void declare_builtin_type(const char *name, type t)
{
  struct data_declaration tempdecl;
  init_data_declaration(&tempdecl, new_error_decl(parse_region, dummy_location),
                        name, t);
  tempdecl.kind = decl_typedef;
  tempdecl.in_system_header = 1;
  declare(current_env, &tempdecl, 1);
}
static tag_declaration make_anonymous_struct(void)
{
  tag_ref tref = newkind_tag_ref(parse_region, kind_struct_ref, dummy_location,
                                 ((void *)0), ((void *)0), ((void *)0), 1);
  return declare_global_tag(tref);
}
static void declare_builtin_types(void)
{
  builtin_va_list_type =
    make_pointer_type(make_tagged_type(make_anonymous_struct()));
  declare_builtin_type("__builtin_va_list", builtin_va_list_type);
}
static data_declaration declare_builtin(const char *name, data_kind kind, type t)
{
  struct data_declaration tempdecl;
  init_data_declaration(&tempdecl, new_error_decl(parse_region, dummy_location),
                        name, t);
  tempdecl.kind = kind;
  tempdecl.needsmemory = 1;
  tempdecl.in_system_header = 1;
  tempdecl.vtype = variable_static;
  return declare(global_env, &tempdecl, 1);
}
data_declaration declare_builtin_identifier(const char *name, type t)
{
  return declare_builtin(name, decl_variable, t);
}
data_declaration declare_builtin_function(const char *name, type t)
{
  return declare_builtin(name, decl_function, t);
}
static void declare_builtin_identifiers(void)
{
  typelist emptylist = new_typelist(parse_region);
  type default_function_type = make_function_type(int_type, emptylist, 0, no_qualifiers, ((void *)0), 1);
  builtin_va_arg_decl = declare_builtin_identifier("=va_arg", int_type);
  declare_builtin_function("__builtin_constant_p", default_function_type);
}
static void declare_function_name(void)
{
  const char *name, *printable_name;
  if (current_function_decl == ((void *)0))
    {
      name = "";
      printable_name = "top level";
    }
  else
    {
      name = current_function_decl->ddecl->name;
      printable_name = name;
    }
  declare_magic_string("__func__", name);
  declare_magic_string("__FUNCTION__", name);
  declare_magic_string("__PRETTY_FUNCTION__", printable_name);
}
bool start_function(type_element elements, declarator d, attribute attribs,
                    bool nested)
{
  int class;
  bool inlinep;
  const char *name;
  type function_type;
  bool defaulted_int, old_decl_has_prototype, normal_function;
  data_declaration old_decl, ddecl;
  function_decl fdecl;
  function_declarator fdeclarator;
  struct data_declaration tempdecl;
  env_scanner scan;
  const char *id;
  void *idval;
  if (!nested)
    ((void) ((current_env == global_env && current_function_decl == ((void *)0)) ? 0 : (__assert_fail ("current_env == global_env && current_function_decl == ((void *)0)", "semantics.c", 1804, __PRETTY_FUNCTION__), 0)));
  parse_declarator(elements, d, 0, &class, &inlinep, &name, &function_type,
                   &defaulted_int, &fdeclarator);
  if (!type_function(function_type))
    return 0;
  fdecl = new_function_decl(parse_region, d->loc, d, elements, attribs,
                            ((void *)0), ((void *)0), current_function_decl, ((void *)0));
  fdecl->declared_type = function_type;
  fdecl->undeclared_variables = new_env(parse_region, ((void *)0));
  fdecl->current_loop = ((void *)0);
  if (class == RID_AUTO)
    {
      if (pedantic || !nested)
        pedwarn("function definition declared `auto'");
      class = 0;
    }
  else if (class == RID_REGISTER)
    {
      error("function definition declared `register'");
      class = 0;
    }
  else if (class == RID_TYPEDEF)
    {
      error("function definition declared `typedef'");
      class = 0;
    }
  else if (class == RID_EXTERN && nested)
    {
      error("nested function `%s' declared `extern'", name);
      class = 0;
    }
  else if ((class == RID_STATIC || inlinep) && nested)
    {
      if (pedantic)
        pedwarn("invalid storage class for function `%s'", name);
      class = 0;
    }
  if (!type_void(type_function_return_type(function_type)) &&
      type_incomplete(type_function_return_type(function_type)))
    {
      error("return-type is an incomplete type");
      function_type = qualify_type1
        (make_function_type(void_type,
                            type_function_arguments(function_type),
                            type_function_varargs(function_type),
                            type_function_varargs_quals(function_type),
                            type_function_varargs_user_quals(function_type),
                            type_function_oldstyle(function_type)),
         function_type);
    }
  old_decl = lookup_id(name, 1);
  old_decl_has_prototype = old_decl && old_decl->kind == decl_function &&
    !type_function_oldstyle(old_decl->type);
  normal_function = !nested && class != RID_STATIC;
  if (warn_strict_prototypes
      && type_function_oldstyle(function_type)
      && !old_decl_has_prototype)
    warning("function declaration isn't a prototype");
  else if (warn_missing_prototypes
           && normal_function && !old_decl_has_prototype
           && __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("main") && __builtin_constant_p (name) && (__s1_len = strlen ("main"), __s2_len = strlen (name), (!((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("main"), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) && (__s1_len = strlen ("main"), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("main"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp ("main", name)))); }))
    warning("no previous prototype for `%s'", name);
  else if (warn_missing_prototypes
           && old_decl && old_decl->ftype == function_implicit)
    warning("`%s' was used with no prototype before its definition", name);
  else if (warn_missing_declarations
           && normal_function && !old_decl && __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("main") && __builtin_constant_p (name) && (__s1_len = strlen ("main"), __s2_len = strlen (name), (!((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("main"), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) && (__s1_len = strlen ("main"), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p ("main") && ((size_t)(const void *)(("main") + 1) - (size_t)(const void *)("main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("main"))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("main"))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("main"))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("main"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp ("main", name)))); }))
    warning("no previous declaration for `%s'", name);
  else if (warn_missing_declarations
           && old_decl && old_decl->ftype == function_implicit)
    warning("`%s' was used with no declaration before its definition", name);
  check_function(&tempdecl, ({ast_generic tEmPcast = (ast_generic)(fdecl); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 1893, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), class,
                 inlinep, name, function_type, nested, 0, defaulted_int);
  tempdecl.definition = tempdecl.ast;
  tempdecl.defined = 1;
  if (old_decl_has_prototype && type_function_oldstyle(function_type) &&
      type_compatible(type_function_return_type(old_decl->type),
                      type_function_return_type(function_type)))
    {
      function_type = qualify_type1
        (make_function_type(type_function_return_type(function_type),
                            type_function_arguments(old_decl->type),
                            type_function_varargs(old_decl->type),
                            type_function_varargs_quals(old_decl->type),
                            type_function_varargs_user_quals(old_decl->type),
                            0), old_decl->type);
      tempdecl.type = function_type;
    }
  if (old_decl && duplicate_decls(&tempdecl, old_decl, 0, 0))
    ddecl = old_decl;
  else
    ddecl = declare(current_env, &tempdecl, 0);
  fdecl->base_labels = fdecl->scoped_labels =
    new_env(parse_region,
            current_function_decl ? current_function_decl->scoped_labels : ((void *)0));
  fdecl->ddecl = ddecl;
  fdecl->fdeclarator = fdeclarator;
  current_env = fdeclarator->env;
  current_env->fdecl = current_function_decl = fdecl;
  env_scan(current_env->id_env, &scan);
  while (env_next(&scan, &id, &idval))
    {
      data_declaration iddecl = idval;
      if (iddecl->kind == decl_variable)
        {
          ((void) ((iddecl->isparameter) ? 0 : (__assert_fail ("iddecl->isparameter", "semantics.c", 1939, __PRETTY_FUNCTION__), 0)));
        }
    }
  handle_attributes(attribs, ddecl, ((void *)0), ((void *)0));
  return 1;
}
data_declaration implicitly_declare(identifier fnid)
{
  struct data_declaration tempdecl;
  declaration pseudo_ast =
    ({ast_generic tEmPcast = (ast_generic)(new_implicit_decl(parse_region, fnid->loc, fnid)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 1953, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
  init_data_declaration(&tempdecl, pseudo_ast,
                        fnid->cstring.data, implicit_function_type);
  tempdecl.kind = decl_function;
  tempdecl.isexternalscope = 1;
  tempdecl.isfilescoperef = 1;
  tempdecl.ftype = function_implicit;
  tempdecl.shadowed = lookup_global_id(tempdecl.name);
  return declare(current_env, &tempdecl, 0);
}
void store_parm_decls(declaration old_parms)
{
  if (!oldstyle_function(current_function_decl))
    {
      if (old_parms)
        error_with_decl(({ast_generic tEmPcast = (ast_generic)(current_function_decl); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 1978, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }),
                        "parm types given both in parmlist and separately");
    }
  else
    {
      oldidentifier_decl parm, parms;
      current_function_decl->old_parms = old_parms;
      parms = ({ast_generic tEmPcast = (ast_generic)(current_function_decl->fdeclarator->parms); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)", "semantics.c", 1990, __PRETTY_FUNCTION__), 0))); (oldidentifier_decl)(tEmPcast); });
      for (parm = (parms); parm; parm = ({ast_generic tEmPcast = (ast_generic)(parm->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)", "semantics.c", 1992, __PRETTY_FUNCTION__), 0))); (oldidentifier_decl)(tEmPcast); }))
        if (parm->ddecl->type == void_type)
          {
            parm->ddecl->type = int_type;
            if (extra_warnings)
              warning_with_decl(({ast_generic tEmPcast = (ast_generic)(parm); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 1998, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }),
                                "type of `%s' defaults to `int'",
                                parm->cstring.data);
          }
    }
  declare_function_name();
}
declaration finish_function(statement body)
{
  declaration fn = ({ast_generic tEmPcast = (ast_generic)(current_function_decl); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2011, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
  current_function_decl->stmt = body;
  ((void) ((current_env->parm_level) ? 0 : (__assert_fail ("current_env->parm_level", "semantics.c", 2014, __PRETTY_FUNCTION__), 0)));
  poplevel();
  check_labels();
  current_function_decl = current_function_decl->parent_function;
  return fn;
}
void pushlevel(bool parm_level)
{
  current_env = new_environment(parse_region, current_env, parm_level);
}
environment poplevel(void)
{
  environment old = current_env;
  current_env = current_env->parent;
  return old;
}
void push_label_level(void)
{
  current_function_decl->scoped_labels =
    new_env(parse_region, current_function_decl->scoped_labels);
}
void pop_label_level(void)
{
  check_labels();
  current_function_decl->scoped_labels =
    env_parent(current_function_decl->scoped_labels);
  ((void) ((current_function_decl->scoped_labels) ? 0 : (__assert_fail ("current_function_decl->scoped_labels", "semantics.c", 2049, __PRETTY_FUNCTION__), 0)));
}
const char *declarator_name(declarator d)
{
  while (d)
    {
      switch (d->kind)
        {
        case kind_identifier_declarator:
          return ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier_declarator && (tEmPcast)->kind <= postkind_identifier_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier_declarator && (tEmPcast)->kind <= postkind_identifier_declarator)", "semantics.c", 2059, __PRETTY_FUNCTION__), 0))); (identifier_declarator)(tEmPcast); })->cstring.data;
        case kind_function_declarator:
          d = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_declarator && (tEmPcast)->kind <= postkind_function_declarator)", "semantics.c", 2061, __PRETTY_FUNCTION__), 0))); (function_declarator)(tEmPcast); })->declarator; break;
        case kind_array_declarator:
          d = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_array_declarator && (tEmPcast)->kind <= postkind_array_declarator)", "semantics.c", 2063, __PRETTY_FUNCTION__), 0))); (array_declarator)(tEmPcast); })->declarator; break;
        case kind_pointer_declarator:
          d = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_pointer_declarator && (tEmPcast)->kind <= postkind_pointer_declarator)", "semantics.c", 2065, __PRETTY_FUNCTION__), 0))); (pointer_declarator)(tEmPcast); })->declarator; break;
        default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 2066, __PRETTY_FUNCTION__), 0)));
        }
    }
  return ((void *)0);
}
void check_parameter(data_declaration dd,
                     type_element elements, variable_decl vd)
{
  int class;
  bool inlinep;
  const char *name, *printname;
  bool defaulted_int;
  type parm_type;
  parse_declarator(elements, vd->declarator, 0,
                   &class, &inlinep, &name, &parm_type, &defaulted_int, ((void *)0));
  vd->declared_type = parm_type;
  printname = name ? name : "type name";
  if (class && class != RID_REGISTER)
    {
      error("storage class specified for parameter `%s'", printname);
      class = 0;
    }
  if (inlinep)
    pedwarn_with_decl(({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2094, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }),
                      "parameter `%s' declared `inline'", printname);
  if (type_array(parm_type))
    parm_type =
      make_pointer_type(qualify_type1(type_array_of(parm_type), parm_type));
  else if (type_function(parm_type))
    parm_type = make_pointer_type(parm_type);
  init_data_declaration(dd, ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2106, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), name, parm_type);
  dd->kind = decl_variable;
  dd->definition = dd->ast;
  dd->defined = 1;
  dd->isexternalscope = 0;
  dd->isfilescoperef = 0;
  dd->needsmemory = 0;
  dd->isused = 1;
  dd->vtype = class == RID_REGISTER ? variable_register : variable_normal;
  dd->islocal = dd->isparameter = 1;
}
declaration start_decl(declarator d, asm_stmt astmt, type_element elements,
                       bool initialised, attribute extra_attributes,
                       attribute attributes)
{
  variable_decl vd =
    new_variable_decl(parse_region, d->loc, d, extra_attributes, ((void *)0),
                      astmt, ((void *)0));
  struct data_declaration tempdecl;
  data_declaration ddecl = ((void *)0), old_decl;
  if (current_env->parm_level)
    {
      check_parameter(&tempdecl, elements, vd);
      if (type_void(tempdecl.type))
        {
          error("parameter `%s' declared void", tempdecl.name);
          tempdecl.type = int_type;
        }
      if (oldstyle_function(current_function_decl))
        {
          if (flag_traditional &&
              type_equal_unqualified(tempdecl.type, float_type))
            tempdecl.type = qualify_type1(double_type, tempdecl.type);
          old_decl = lookup_id(tempdecl.name, 1);
          if (old_decl && duplicate_decls(&tempdecl, old_decl, 0, 0))
            {
              ddecl = old_decl;
              ddecl->type = tempdecl.type;
              ddecl->ast = ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2161, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
              ddecl->isused = 1;
            }
          else
            {
              error("declaration for parameter `%s' but no such parameter",
                    tempdecl.name);
              ddecl = declare(current_env, &tempdecl, 0);
            }
        }
      else
        ddecl = declare(current_env, &tempdecl, 0);
      if (initialised)
        error("parameter `%s' is initialized",
              vd->ddecl->name ? vd->ddecl->name : "type name");
    }
  else
    {
      int class;
      bool inlinep;
      const char *name, *printname;
      bool defaulted_int;
      type var_type;
      bool different_binding_level = 0;
      parse_declarator(elements, d, 0, &class, &inlinep, &name, &var_type,
                       &defaulted_int, ((void *)0));
      vd->declared_type = var_type;
      printname = name ? name : "type name";
      if (warn_implicit_int && defaulted_int && !type_function(var_type))
        warning("type defaults to `int' in declaration of `%s'", printname);
      init_data_declaration(&tempdecl, ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2196, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), name, var_type);
      if (class == RID_EXTERN && initialised)
        {
          if (current_env == global_env)
            warning("`%s' initialized and declared `extern'", printname);
          else
            error("`%s' has both `extern' and initializer", printname);
        }
      if (class == RID_AUTO && current_env == global_env)
        {
          error("top-level declaration of `%s' specifies `auto'", printname);
          class = 0;
        }
      if (class == RID_TYPEDEF)
        {
          if (initialised && (pedantic || elements->next))
            error("typedef `%s' is initialized", printname);
          tempdecl.kind = decl_typedef;
          tempdecl.definition = tempdecl.ast;
          tempdecl.defined = 1;
          tempdecl.isexternalscope = 0;
          tempdecl.isfilescoperef = 0;
          tempdecl.needsmemory = 0;
        }
      else if (type_function(var_type))
        {
          bool nested = current_env != global_env && class == RID_AUTO;
          if (initialised)
            error("function `%s' is initialized like a variable",
                  printname);
          if (class == RID_AUTO && pedantic)
            pedwarn("invalid storage class for function `%s'", name);
          if (class == RID_REGISTER)
            {
              error("invalid storage class for function `%s'", name);
              class = 0;
            }
          if (current_env != global_env && pedantic
              && (class == RID_STATIC || class == RID_INLINE))
            pedwarn("invalid storage class for function `%s'", name);
          check_function(&tempdecl, ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2252, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), class, inlinep,
                         name, var_type, nested, 1, defaulted_int);
        }
      else
        {
          int extern_ref = !initialised && class == RID_EXTERN;
          if (type_void(var_type) &&
              !(class == RID_EXTERN ||
                (current_env == global_env &&
                 !(class == RID_STATIC || class == RID_REGISTER))))
            {
              error("variable `%s' declared void", printname);
              var_type = int_type;
            }
          if (inlinep)
            pedwarn("variable `%s' declared `inline'", printname);
          tempdecl.kind = decl_variable;
          tempdecl.vtype =
            class == RID_REGISTER ? variable_register :
            class == RID_STATIC ? variable_static :
            variable_normal;
          tempdecl.isfilescoperef = extern_ref;
          if (!extern_ref)
            {
              tempdecl.definition = tempdecl.ast;
              tempdecl.defined = 1;
            }
          if (current_env == global_env)
            {
              tempdecl.isexternalscope =
                class != RID_STATIC && class != RID_REGISTER;
              tempdecl.needsmemory = !extern_ref;
              tempdecl.islocal = 0;
            }
          else
            {
              tempdecl.isexternalscope = extern_ref;
              tempdecl.needsmemory = class == RID_STATIC;
              tempdecl.islocal = !(extern_ref || class == RID_STATIC);
            }
        }
      if (warn_nested_externs && tempdecl.isfilescoperef &&
          current_env != global_env && !tempdecl.in_system_header)
        warning("nested extern declaration of `%s'", printname);
      old_decl = lookup_id(name, 1);
      if (!old_decl && tempdecl.isfilescoperef)
        {
          old_decl = lookup_global_id(name);
          if (old_decl && old_decl->kind == decl_typedef)
            old_decl = ((void *)0);
          if (old_decl)
            different_binding_level = 1;
        }
      if (old_decl &&
          duplicate_decls(&tempdecl, old_decl, different_binding_level, initialised))
        ddecl = old_decl;
      else
        ddecl = declare(current_env, &tempdecl, 0);
    }
  ((void) ((ddecl) ? 0 : (__assert_fail ("ddecl", "semantics.c", 2347, __PRETTY_FUNCTION__), 0)));
  vd->ddecl = ddecl;
  handle_attributes(extra_attributes, ddecl, ((void *)0), ((void *)0));
  handle_attributes(attributes, ddecl, ((void *)0), ((void *)0));
  return ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2353, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
}
declaration finish_decl(declaration decl, expression init)
{
  variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(decl); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "semantics.c", 2360, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
  data_declaration dd = vd->ddecl;
  vd->arg1 = init;
  dd->initialiser = init;
  if (init)
    {
      if (dd->kind == decl_typedef)
        dd->type = init->type;
      else if (type_array(dd->type))
        {
          if (!type_array_size(dd->type))
            {
              expression size = build_uint_constant(parse_region, dummy_location, size_t_type, 0);
              dd->type = make_array_type(type_array_of(dd->type), size);
            }
        }
      else
        check_assignment(dd->type, default_conversion_for_assignment(init),
                         init, "initialization", ((void *)0), ((void *)0), 0);
    }
  return decl;
}
declaration declare_parameter(declarator d, type_element elements,
                              attribute extra_attributes, attribute attributes,
                              bool abstract)
{
  location l =
    d ? d->loc : elements->loc;
  variable_decl vd =
    new_variable_decl(parse_region, l, d, attributes, ((void *)0), ((void *)0), ((void *)0));
  data_decl dd =
    new_data_decl(parse_region, l, elements, extra_attributes,
                  ({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2404, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }));
  data_declaration ddecl = ((void *)0), old_decl = ((void *)0);
  struct data_declaration tempdecl;
  check_parameter(&tempdecl, elements, vd);
  if (tempdecl.name)
    old_decl = lookup_id(tempdecl.name, 1);
  if (old_decl && duplicate_decls(&tempdecl, old_decl, 0, 0))
    {
      ddecl = old_decl;
      ddecl->isused = 1;
    }
  else if (!type_void(tempdecl.type))
    ddecl = declare(current_env, &tempdecl, 0);
  if (ddecl)
    {
      if (type_union(ddecl->type) && type_tag(ddecl->type)->transparent_union)
        transparent_union_argument(ddecl);
      handle_attributes(extra_attributes, ddecl, ((void *)0), ((void *)0));
      handle_attributes(attributes, ddecl, ((void *)0), ((void *)0));
    }
  else
    {
      ignored_attributes(extra_attributes);
      ignored_attributes(attributes);
    }
  vd->ddecl = ddecl;
  return ({ast_generic tEmPcast = (ast_generic)(dd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2439, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
}
void mark_forward_parameters(declaration parms)
{
  declaration parm;
  for (parm = (parms); parm; parm = ({ast_generic tEmPcast = (ast_generic)(parm->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2449, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
    if (((parm)->kind >= kind_data_decl && (parm)->kind <= postkind_data_decl))
      {
        data_decl pd = ({ast_generic tEmPcast = (ast_generic)(parm); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "semantics.c", 2452, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
        variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(pd->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "semantics.c", 2453, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
        vd->forward = 1;
        vd->ddecl->isused = 0;
      }
}
declaration declare_old_parameter(location l, cstring id)
{
  oldidentifier_decl d = new_oldidentifier_decl(parse_region, l, id, ((void *)0));
  data_declaration ddecl;
  if ((ddecl = lookup_id(id.data, 1)))
    error("duplicate parameter name `%s' in parameter list", id.data);
  else
    {
      struct data_declaration tempdecl;
      init_data_declaration(&tempdecl, ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2473, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), id.data,
                            void_type);
      tempdecl.kind = decl_variable;
      tempdecl.definition = tempdecl.ast;
      tempdecl.defined = 1;
      tempdecl.isexternalscope = 0;
      tempdecl.isfilescoperef = 0;
      tempdecl.needsmemory = 0;
      tempdecl.vtype = variable_normal;
      tempdecl.islocal = tempdecl.isparameter = 1;
      ddecl = declare(current_env, &tempdecl, 0);
    }
  d->ddecl = ddecl;
  return ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2487, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
}
type_element start_struct(location l, ast_kind skind, word tag)
{
  tag_ref tref = newkind_tag_ref(parse_region, skind, l, tag, ((void *)0), ((void *)0), 1);
  tag_declaration tdecl = tag ? lookup_tag(tref, 1) : ((void *)0);
  pending_xref_error();
  if (tdecl && tdecl->kind == skind)
    {
      if (tdecl->defined || tdecl->being_defined)
        {
          error((skind == kind_union_ref ? "redefinition of `union %s'"
                 : "redefinition of `struct %s'"),
                tag->cstring.data);
          tdecl = declare_tag(tref);
        }
    }
  else
    tdecl = declare_tag(tref);
  tref->tdecl = tdecl;
  tdecl->being_defined = 1;
  return ({ast_generic tEmPcast = (ast_generic)(tref); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 2514, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); });
}
type_element finish_struct(type_element t, declaration fields,
                           attribute attribs)
{
  tag_ref s = ({ast_generic tEmPcast = (ast_generic)(t); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)", "semantics.c", 2523, __PRETTY_FUNCTION__), 0))); (tag_ref)(tEmPcast); });
  tag_declaration tdecl = s->tdecl;
  bool hasmembers = 0;
  field_declaration *nextfield = &tdecl->fieldlist;
  declaration fdecl;
  size_t offset = 0, alignment = 8U, size = 0;
  bool size_cc = 1;
  bool isunion = tdecl->kind == kind_union_ref;
  s->fields = fields;
  s->attributes = attribs;
  handle_attributes(attribs, ((void *)0), ((void *)0), tdecl);
  tdecl->fields = new_env(parse_region, ((void *)0));
  tdecl->defined = 1;
  tdecl->being_defined = 0;
  for (fdecl = (fields); fdecl; fdecl = ({ast_generic tEmPcast = (ast_generic)(fdecl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2539, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
    {
      declaration fieldlist;
      data_decl flist;
      field_decl field;
      fieldlist = fdecl;
      while (((fieldlist)->kind >= kind_extension_decl && (fieldlist)->kind <= postkind_extension_decl))
        fieldlist = ({ast_generic tEmPcast = (ast_generic)(fieldlist); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "semantics.c", 2548, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); })->decl;
      flist = ({ast_generic tEmPcast = (ast_generic)(fieldlist); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "semantics.c", 2549, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
      for (field = (({ast_generic tEmPcast = (ast_generic)(flist->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)", "semantics.c", 2551, __PRETTY_FUNCTION__), 0))); (field_decl)(tEmPcast); })); field; field = ({ast_generic tEmPcast = (ast_generic)(field->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_field_decl && (tEmPcast)->kind <= postkind_field_decl)", "semantics.c", 2551, __PRETTY_FUNCTION__), 0))); (field_decl)(tEmPcast); }))
        {
          int class;
          bool inlinep;
          const char *name, *printname;
          bool defaulted_int;
          type field_type, base_field_type, tmpft;
          location floc = field->loc;
          field_declaration fdecl = (sizeof(struct field_declaration) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct field_declaration)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct field_declaration), 0));
          long long bitwidth;
          size_t falign, fsize;
          parse_declarator(flist->modifiers, field->declarator,
                           field->arg1 != ((void *)0), &class, &inlinep, &name,
                           &tmpft, &defaulted_int, ((void *)0));
          field_type = tmpft;
          printname = name ? name : "(anonymous)";
          if (name)
            hasmembers = 1;
          if (!tdecl->fields_const)
            {
              base_field_type = type_base(field_type);
              if (type_const(base_field_type) ||
                  ((type_struct(base_field_type) || type_union(base_field_type)) &&
                   type_tag(base_field_type)->fields_const))
                tdecl->fields_const = 1;
            }
          if (type_volatile(field_type))
            tdecl->fields_volatile = 1;
          if (type_function(field_type))
            {
              error_with_location(floc, "field `%s' declared as a function", printname);
              field_type = make_pointer_type(field_type);
            }
          else if (type_void(field_type))
            {
              error_with_location(floc, "field `%s' declared void", printname);
              field_type = error_type;
            }
          else if (type_incomplete(field_type))
            {
              error_with_location(floc, "field `%s' has incomplete type", printname);
              field_type = error_type;
            }
          *nextfield = fdecl;
          nextfield = &fdecl->next;
          fdecl->name = name;
          fdecl->type = field_type;
          fdecl->ast = field;
          if (name)
            {
              if (env_lookup(tdecl->fields, name, 1))
                error_with_location(floc, "duplicate member `%s'", name);
              env_add(tdecl->fields, name, fdecl);
            }
          handle_attributes(field->attributes, ((void *)0), fdecl, ((void *)0));
          handle_attributes(flist->attributes, ((void *)0), fdecl, ((void *)0));
          field_type = fdecl->type;
          bitwidth = -1;
          if (field->arg1)
            {
              known_cst cwidth = field->arg1->cst;
              if (!type_integer(field_type))
                error_with_location(floc, "bit-field `%s' has invalid type", printname);
              else if (!(type_integer(field->arg1->type) && cval_isinteger((cwidth)->cval)))
                error_with_location(floc, "bit-field `%s' width not an integer constant",
                                    printname);
              else
                {
                  largest_uint width = cval_uint_value((cwidth)->cval);
                  if (pedantic)
                    constant_overflow_warning(cwidth);
                  if (!type_unsigned(cwidth->type) &&
                      cval_sint_value((cwidth)->cval) < 0)
                    error_with_location(floc, "negative width in bit-field `%s'", printname);
                  else if (width > type_size(field_type) * 8U)
                    pedwarn_with_location(floc, "width of `%s' exceeds its type", printname);
                  else if (width == 0 && name)
                    error_with_location(floc, "zero width for bit-field `%s'", name);
                  else
                    bitwidth = width;
                }
            }
          fdecl->bitwidth = bitwidth;
          fdecl->offset_cc = size_cc || isunion;
          if (type_size_cc(field_type))
            fsize = type_size(field_type) * 8U;
          else
            {
              size_cc = 0;
              fsize = 8U;
            }
          falign = (type_has_size(field_type) ? type_alignment(field_type) : 1)
            * 8U;
          if (bitwidth == 0)
            {
              offset = align_to(offset, falign);
              falign = 1;
            }
          else if (bitwidth > 0)
            {
              if (((offset + bitwidth + falign - 1) / falign - offset / falign)
                  > fsize / falign)
                offset = align_to(offset, falign);
            }
          else
            {
              offset = align_to(offset, falign);
              bitwidth = fsize;
            }
          fdecl->offset = offset;
          if (!isunion)
            {
              offset += bitwidth;
              size = offset;
            }
          else
            size = bitwidth > size ? bitwidth : size;
          alignment = lcm(alignment, falign);
        }
    }
  if (pedantic && !hasmembers)
    pedwarn("%s has no %smembers",
            (s->kind == kind_union_ref ? "union" : "structure"),
            (fields ? "named " : ""));
  tdecl->size = align_to(size, alignment) / 8U;
  tdecl->alignment = alignment / 8U;
  tdecl->size_cc = size_cc;
  return t;
}
type_element xref_tag(location l, ast_kind skind, word tag)
{
  tag_ref tref = newkind_tag_ref(parse_region, skind, l, tag, ((void *)0), ((void *)0), 0);
  tag_declaration tdecl = lookup_tag(tref, 0);
  if (!tdecl)
    tdecl = declare_tag(tref);
  tref->tdecl = tdecl;
  return ({ast_generic tEmPcast = (ast_generic)(tref); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 2752, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); });
}
static known_cst last_enum_value;
type_element start_enum(location l, word tag)
{
  enum_ref tref = new_enum_ref(parse_region, l, tag, ((void *)0), ((void *)0), 1);
  tag_declaration tdecl = tag ? lookup_tag(tref, 1) : ((void *)0);
  pending_xref_error();
  if (tdecl && tdecl->kind == kind_enum_ref)
    {
      if (tdecl->defined)
        error("redefinition of `enum %s'", tag->cstring.data);
    }
  else
    tdecl = declare_tag(tref);
  tref->tdecl = tdecl;
  tdecl->being_defined = 1;
  tdecl->packed = flag_short_enums;
  last_enum_value = ((void *)0);
  return ({ast_generic tEmPcast = (ast_generic)(tref); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 2778, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); });
}
type_element finish_enum(type_element t, declaration names,
                         attribute attribs)
{
  tag_ref s = ({ast_generic tEmPcast = (ast_generic)(t); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_tag_ref && (tEmPcast)->kind <= postkind_tag_ref)", "semantics.c", 2786, __PRETTY_FUNCTION__), 0))); (tag_ref)(tEmPcast); });
  tag_declaration tdecl = s->tdecl;
  cval smallest, largest;
  bool enum_isunsigned;
  type type_smallest, type_largest, enum_reptype;
  enumerator v, values = ({ast_generic tEmPcast = (ast_generic)(names); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "semantics.c", 2791, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); });
  s->fields = names;
  s->attributes = attribs;
  handle_attributes(attribs, ((void *)0), ((void *)0), tdecl);
  tdecl->fields = 0;
  tdecl->defined = 1;
  tdecl->being_defined = 0;
  if (!names)
    smallest = largest = cval_zero;
  else
    {
      smallest = largest = value_of_enumerator(values);
      for (v = (({ast_generic tEmPcast = (ast_generic)(values->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "semantics.c", 2809, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); })); v; v = ({ast_generic tEmPcast = (ast_generic)(v->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "semantics.c", 2809, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); }))
        {
          cval vv = value_of_enumerator(v);
          if (cval_intcompare(vv, largest) > 0)
            largest = vv;
          if (cval_intcompare(vv, smallest) < 0)
            smallest = vv;
        }
    }
  enum_isunsigned = cval_intcompare(smallest, cval_zero) >= 0;
  type_smallest = type_for_cval(smallest, enum_isunsigned);
  type_largest = type_for_cval(largest, enum_isunsigned);
  ((void) ((type_smallest) ? 0 : (__assert_fail ("type_smallest", "semantics.c", 2824, __PRETTY_FUNCTION__), 0)));
  if (!type_largest)
    {
      ((void) ((!enum_isunsigned) ? 0 : (__assert_fail ("!enum_isunsigned", "semantics.c", 2827, __PRETTY_FUNCTION__), 0)));
      warning("enumeration values exceed range of largest integer");
      type_largest = long_long_type;
    }
  if (type_size(type_smallest) > type_size(type_largest))
    enum_reptype = type_smallest;
  else
    enum_reptype = type_largest;
  if (!tdecl->packed && type_size(enum_reptype) < type_size(int_type))
    enum_reptype = int_type;
  tdecl->reptype = enum_reptype;
  tdecl->size_cc = 1;
  tdecl->size = type_size(enum_reptype);
  tdecl->alignment = type_alignment(enum_reptype);
  for (v = (values); v; v = ({ast_generic tEmPcast = (ast_generic)(v->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "semantics.c", 2847, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); }))
    {
      known_cst val = v->ddecl->value;
      val->type = enum_reptype;
      val->cval = cval_cast(val->cval, enum_reptype);
    }
  return t;
}
declaration make_enumerator(location loc, cstring id, expression value)
{
  declaration ast = ({ast_generic tEmPcast = (ast_generic)(new_enumerator(parse_region, loc, id, value, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2860, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
  struct data_declaration tempdecl;
  data_declaration ddecl, old_decl;
  known_cst cval = ((void *)0);
  init_data_declaration(&tempdecl, ast, id.data, int_type);
  tempdecl.kind = decl_constant;
  tempdecl.definition = ast;
  tempdecl.defined = 1;
  if (value)
    {
      if (!type_integer(value->type) || !value->cst ||
          !cval_isinteger((value->cst)->cval))
        error("enumerator value for `%s' not integer constant", id.data);
      else
        cval = value->cst;
    }
  if (!cval)
    {
      if (last_enum_value)
        {
          type addtype = type_unsigned(last_enum_value->type) ?
            unsigned_long_long_type : long_long_type;
          cval = fold_add(addtype, last_enum_value, onecst);
        }
      else
        cval = zerocst;
    }
  if (pedantic && !cval_inrange(cval->cval, int_type))
    {
      pedwarn("ANSI C restricts enumerator values to range of `int'");
      cval = zerocst;
    }
  tempdecl.value = last_enum_value = cval;
  if (type_size(cval->type) >= type_size(int_type))
    tempdecl.value->type = cval->type;
  else
    tempdecl.value->type =
      type_for_size(type_size(int_type), flag_traditional && type_unsigned(cval->type));
  old_decl = lookup_id(id.data, 1);
  if (old_decl && duplicate_decls(&tempdecl, old_decl, 0, 0))
    ddecl = old_decl;
  else
    ddecl = declare(current_env, &tempdecl, 0);
  ({ast_generic tEmPcast = (ast_generic)(ast); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_enumerator && (tEmPcast)->kind <= postkind_enumerator)", "semantics.c", 2915, __PRETTY_FUNCTION__), 0))); (enumerator)(tEmPcast); })->ddecl = ddecl;
  return ast;
}
declaration make_field(declarator d, expression bitfield,
                       type_element elements, attribute extra_attributes,
                       attribute attributes)
{
  location l = d ? d->loc : bitfield->loc;
  return
    ({ast_generic tEmPcast = (ast_generic)(new_field_decl(parse_region, l, d, extra_attributes, bitfield)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "semantics.c", 2933, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
}
asttype make_type(type_element elements, declarator d)
{
  location l = elements ? elements->loc : d->loc;
  int class;
  bool inlinep;
  const char *name;
  bool defaulted_int;
  asttype t = new_asttype(parse_region, l, d, elements);
  parse_declarator(t->qualifiers, t->declarator, 0, &class, &inlinep, &name,
                   &t->type, &defaulted_int, ((void *)0));
  ((void) ((t->type) ? 0 : (__assert_fail ("t->type", "semantics.c", 2950, __PRETTY_FUNCTION__), 0)));
  return t;
}
int save_directive(char *directive)
{
  static char *directive_buffer = ((void *)0);
  static unsigned buffer_length = 0;
  char *p;
  char *buffer_limit;
  int looking_for = 0;
  int char_escaped = 0;
  if (buffer_length == 0)
    {
      directive_buffer = (char *)xmalloc (128);
      buffer_length = 128;
    }
  buffer_limit = &directive_buffer[buffer_length];
  for (p = directive_buffer; ; )
    {
      int c;
      if (p >= buffer_limit)
        {
          unsigned bytes_used = (p - directive_buffer);
          buffer_length *= 2;
          directive_buffer
            = (char *)xrealloc (directive_buffer, buffer_length);
          p = &directive_buffer[bytes_used];
          buffer_limit = &directive_buffer[buffer_length];
        }
      c = _IO_getc (finput);
      if ((c == ' ' || c == '\t') && p == directive_buffer)
        continue;
      if (looking_for == 0
          && (c == '\n' || c == (-1)))
        {
          ungetc (c, finput);
          c = '\0';
        }
      *p++ = c;
      if (c == 0)
        break;
      if (looking_for)
        {
          if (looking_for == c && !char_escaped)
            looking_for = 0;
        }
      else
        if (c == '\'' || c == '"')
          looking_for = c;
      char_escaped = (c == '\\' && ! char_escaped);
    }
  return '\n';
}
static char *rid_name_int(int id)
{
  switch (id)
    {
    case RID_INT: return "int";
    case RID_CHAR: return "char";
    case RID_FLOAT: return "float";
    case RID_DOUBLE: return "double";
    case RID_VOID: return "void";
    case RID_UNSIGNED: return "unsigned";
    case RID_SHORT: return "short";
    case RID_LONG: return "long";
    case RID_AUTO: return "auto";
    case RID_STATIC: return "static";
    case RID_EXTERN: return "extern";
    case RID_REGISTER: return "register";
    case RID_TYPEDEF: return "typedef";
    case RID_SIGNED: return "signed";
    case RID_INLINE: return "__inline";
    case RID_COMPLEX: return "__complex";
    default: ((void) ((0) ? 0 : (__assert_fail ("0", "semantics.c", 3047, __PRETTY_FUNCTION__), 0))); return ((void *)0);
    }
}
char *rid_name(rid r)
{
  return rid_name_int(r->id);
}
statement chain_with_labels(statement l1, statement l2)
{
  node last, last_label;
  if (!l1) return l2;
  if (!l2) return l1;
  last_label = last = last_node(({ast_generic tEmPcast = (ast_generic)(l1); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "semantics.c", 3066, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }));
  while (last_label->kind == kind_labeled_stmt)
    {
      labeled_stmt ls = ({ast_generic tEmPcast = (ast_generic)(last_label); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)", "semantics.c", 3070, __PRETTY_FUNCTION__), 0))); (labeled_stmt)(tEmPcast); });
      if (!ls->stmt)
        {
          ls->stmt = l2;
          return l1;
        }
      last_label = ({ast_generic tEmPcast = (ast_generic)(ls->stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "semantics.c", 3077, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); });
    }
  last->next = ({ast_generic tEmPcast = (ast_generic)(l2); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "semantics.c", 3080, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); });
  return l1;
}
void split_type_elements(type_element tlist, type_element *odeclspecs,
                         attribute *oattributes)
{
  type_element declspecs = ((void *)0);
  attribute attributes = ((void *)0);
  while (tlist)
    {
      type_element te = tlist;
      tlist = ({ast_generic tEmPcast = (ast_generic)(tlist->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "semantics.c", 3094, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); });
      te->next = ((void *)0);
      if (te->kind == kind_attribute)
        attributes = attribute_chain(attributes, ({ast_generic tEmPcast = (ast_generic)(te); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)", "semantics.c", 3098, __PRETTY_FUNCTION__), 0))); (attribute)(tEmPcast); }));
      else
        declspecs = type_element_chain(declspecs, te);
    }
  *odeclspecs = declspecs;
  *oattributes = attributes;
}
void init_semantics(void)
{
  global_env = current_env = new_environment(parse_region, ((void *)0), 0);
  bad_decl = (sizeof(struct data_declaration) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct data_declaration)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct data_declaration), 0));
  bad_decl->kind = decl_error;
  bad_decl->name = "undeclared";
  bad_decl->type = error_type;
  bad_decl->ast = new_error_decl(parse_region, last_location);
  implicit_function_type = make_function_type(int_type, ((void *)0), 0, no_qualifiers, ((void *)0), 1);
  declare_function_name ();
  declare_builtin_types();
  declare_builtin_identifiers();
  onecst = make_signed_cst(1, int_type);
  zerocst = make_signed_cst(0, int_type);
  oneexpr = build_uint_constant(parse_region, dummy_location, size_t_type, 1);
}
const char CANON_IDENT_57e3c27a4f3e42dc1d0b82e649d0e87a[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/semantics.c";
