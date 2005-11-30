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
extern int *__errno_location (void) __attribute__ ((__const__));
typedef int __jmp_buf[6];
typedef int __sig_atomic_t;
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
typedef struct __jmp_buf_tag
  {
    __jmp_buf __jmpbuf;
    int __mask_was_saved;
    __sigset_t __saved_mask;
  } jmp_buf[1];
extern int setjmp (jmp_buf __env) ;
extern int __sigsetjmp (struct __jmp_buf_tag __env[1], int __savemask) ;
extern int _setjmp (struct __jmp_buf_tag __env[1]) ;
extern void longjmp (struct __jmp_buf_tag __env[1], int __val)
             __attribute__ ((__noreturn__));
extern void _longjmp (struct __jmp_buf_tag __env[1], int __val)
             __attribute__ ((__noreturn__));
typedef struct __jmp_buf_tag sigjmp_buf[1];
extern void siglongjmp (sigjmp_buf __env, int __val)
             __attribute__ ((__noreturn__));
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
int yyparse(void) ;
void yyerror();
region parse_region;
static int stmt_count;
static int compstmt_count;
static type_element current_declspecs = ((void *)0);
static attribute prefix_attributes = ((void *)0);
int unevaluated_expression;
typedef struct spec_stack *spec_stack;
struct spec_stack {
  type_element declspecs;
  attribute attributes;
  spec_stack next;
};
static spec_stack declspec_stack;
static region ds_region;
static void pop_declspec_stack(void)
{
  current_declspecs = declspec_stack->declspecs;
  prefix_attributes = declspec_stack->attributes;
  declspec_stack = declspec_stack->next;
}
static void push_declspec_stack(void)
{
  spec_stack news;
  news = (sizeof(struct spec_stack) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((ds_region), sizeof(struct spec_stack)) : (((void)0), __rc_typed_ralloc)((ds_region), sizeof(struct spec_stack), 0));
  news->declspecs = current_declspecs;
  news->attributes = prefix_attributes;
  news->next = declspec_stack;
  declspec_stack = news;
}
int do_parse(void)
{
  int result;
  ds_region = newsubregion(parse_region);
  result = yyparse();
  deleteregion_ptr(&ds_region);
  return result;
}
void yyprint();
static const char yytranslate[] =
{
       0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 70, 2, 2, 2, 60, 51, 2,
      67, 73, 58, 56, 75, 57, 66, 59, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 46, 72,
       2, 44, 2, 45, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 68, 2, 76, 50, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 71, 49, 74, 69, 2, 2, 2,
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
       6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
      26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 47, 48,
      52, 53, 54, 55, 61, 62, 63, 64, 65
};
static const short yyprhs[] =
{
       0, 0, 1, 3, 4, 7, 8, 12, 14, 16,
      22, 25, 29, 34, 39, 42, 46, 49, 52, 54,
      55, 56, 64, 69, 70, 71, 79, 84, 85, 86,
      93, 97, 99, 101, 103, 105, 107, 109, 111, 113,
     115, 117, 119, 121, 123, 125, 126, 128, 130, 132,
     136, 138, 141, 144, 147, 150, 153, 158, 161, 166,
     168, 170, 172, 177, 178, 186, 188, 192, 196, 200,
     204, 208, 212, 216, 220, 224, 228, 232, 236, 240,
     244, 250, 251, 257, 261, 265, 267, 269, 271, 275,
     279, 280, 285, 290, 297, 302, 306, 310, 313, 316,
     318, 320, 323, 325, 327, 328, 330, 333, 335, 338,
     341, 345, 347, 349, 352, 355, 360, 365, 369, 372,
     374, 376, 379, 382, 383, 384, 389, 394, 398, 402,
     406, 409, 412, 415, 419, 420, 423, 426, 429, 432,
     436, 437, 440, 443, 445, 447, 450, 453, 455, 457,
     460, 463, 466, 470, 471, 474, 476, 478, 480, 482,
     487, 492, 494, 496, 498, 500, 502, 504, 508, 510,
     514, 515, 520, 521, 528, 532, 533, 540, 544, 545,
     547, 549, 551, 554, 561, 563, 567, 568, 570, 575,
     582, 587, 589, 591, 593, 595, 597, 601, 603, 604,
     607, 609, 613, 615, 619, 621, 629, 635, 640, 644,
     649, 650, 651, 657, 658, 659, 665, 667, 669, 673,
     678, 683, 687, 691, 695, 697, 702, 707, 711, 715,
     719, 721, 726, 730, 734, 739, 743, 747, 749, 751,
     752, 760, 766, 769, 770, 778, 784, 787, 788, 797,
     798, 806, 809, 810, 812, 813, 815, 817, 820, 821,
     825, 828, 832, 835, 839, 842, 844, 847, 849, 853,
     856, 861, 865, 867, 871, 873, 875, 879, 882, 885,
     886, 888, 890, 893, 894, 897, 901, 905, 908, 913,
     918, 922, 926, 930, 933, 935, 937, 940, 943, 944,
     946, 949, 950, 951, 953, 955, 958, 962, 964, 967,
     969, 973, 980, 986, 992, 995, 998, 1003, 1004, 1009,
    1011, 1014, 1016, 1018, 1020, 1023, 1024, 1029, 1031, 1035,
    1036, 1037, 1045, 1051, 1054, 1055, 1056, 1057, 1070, 1071,
    1078, 1081, 1084, 1087, 1091, 1098, 1107, 1118, 1131, 1135,
    1140, 1142, 1150, 1158, 1164, 1168, 1174, 1177, 1180, 1181,
    1183, 1184, 1186, 1187, 1189, 1191, 1195, 1200, 1202, 1206,
    1207, 1210, 1213, 1214, 1219, 1222, 1223, 1225, 1227, 1231,
    1233, 1237, 1242, 1247, 1252, 1257, 1262, 1263, 1266, 1268,
    1271, 1273, 1277, 1279, 1281, 1285, 1286, 1288, 1290, 1292,
    1294, 1296, 1298, 1300
};
static const short yyrhs[] =
{
      -1, 78, 0, 0, 79, 81, 0, 0, 78, 80,
      81, 0, 83, 0, 82, 0, 31, 67, 94, 73,
      72, 0, 224, 81, 0, 115, 129, 72, 0, 122,
     115, 129, 72, 0, 118, 115, 128, 72, 0, 122,
      72, 0, 118, 115, 72, 0, 1, 72, 0, 1,
      74, 0, 72, 0, 0, 0, 118, 115, 154, 84,
     110, 85, 186, 0, 118, 115, 154, 1, 0, 0,
       0, 122, 115, 157, 86, 110, 87, 186, 0, 122,
     115, 157, 1, 0, 0, 0, 115, 157, 88, 110,
      89, 186, 0, 115, 157, 1, 0, 3, 0, 4,
       0, 90, 0, 90, 0, 51, 0, 57, 0, 56,
       0, 62, 0, 63, 0, 69, 0, 70, 0, 37,
       0, 38, 0, 96, 0, 0, 96, 0, 97, 0,
     103, 0, 97, 75, 103, 0, 105, 0, 58, 101,
       0, 224, 101, 0, 93, 101, 0, 48, 91, 0,
      99, 98, 0, 99, 67, 173, 73, 0, 100, 98,
       0, 100, 67, 173, 73, 0, 15, 0, 33, 0,
      98, 0, 67, 173, 73, 101, 0, 0, 67, 173,
      73, 71, 102, 145, 74, 0, 101, 0, 103, 56,
     103, 0, 103, 57, 103, 0, 103, 58, 103, 0,
     103, 59, 103, 0, 103, 60, 103, 0, 103, 54,
     103, 0, 103, 55, 103, 0, 103, 53, 103, 0,
     103, 52, 103, 0, 103, 51, 103, 0, 103, 49,
     103, 0, 103, 50, 103, 0, 103, 48, 103, 0,
     103, 47, 103, 0, 103, 45, 94, 46, 103, 0,
       0, 103, 45, 104, 46, 103, 0, 103, 44, 103,
       0, 103, 43, 103, 0, 3, 0, 11, 0, 107,
       0, 67, 94, 73, 0, 67, 1, 73, 0, 0,
      67, 106, 188, 73, 0, 105, 67, 95, 73, 0,
      39, 67, 103, 75, 173, 73, 0, 105, 68, 94,
      76, 0, 105, 66, 90, 0, 105, 65, 90, 0,
     105, 62, 0, 105, 63, 0, 108, 0, 109, 0,
     108, 109, 0, 12, 0, 13, 0, 0, 112, 0,
     112, 111, 0, 14, 0, 226, 14, 0, 14, 226,
       0, 226, 14, 226, 0, 113, 0, 181, 0, 112,
     113, 0, 113, 181, 0, 120, 115, 128, 72, 0,
     123, 115, 129, 72, 0, 120, 115, 72, 0, 123,
      72, 0, 117, 0, 181, 0, 114, 117, 0, 117,
     181, 0, 0, 0, 118, 115, 128, 72, 0, 122,
     115, 129, 72, 0, 118, 115, 148, 0, 122, 115,
     151, 0, 118, 115, 72, 0, 122, 72, 0, 224,
     117, 0, 126, 119, 0, 122, 126, 119, 0, 0,
     119, 127, 0, 119, 225, 0, 119, 138, 0, 126,
     121, 0, 123, 126, 121, 0, 0, 121, 127, 0,
     121, 225, 0, 123, 0, 138, 0, 122, 123, 0,
     122, 138, 0, 226, 0, 225, 0, 123, 226, 0,
     123, 225, 0, 126, 125, 0, 175, 126, 125, 0,
       0, 125, 127, 0, 228, 0, 159, 0, 4, 0,
       6, 0, 32, 67, 94, 73, 0, 32, 67, 173,
      73, 0, 228, 0, 226, 0, 159, 0, 130, 0,
     131, 0, 133, 0, 130, 75, 133, 0, 135, 0,
     131, 75, 133, 0, 0, 31, 67, 107, 73, 0,
       0, 154, 132, 137, 44, 134, 144, 0, 154, 132,
     137, 0, 0, 157, 132, 137, 44, 136, 144, 0,
     157, 132, 137, 0, 0, 138, 0, 139, 0, 140,
       0, 138, 140, 0, 34, 67, 67, 141, 73, 73,
       0, 142, 0, 141, 75, 142, 0, 0, 143, 0,
     143, 67, 3, 73, 0, 143, 67, 3, 75, 96,
      73, 0, 143, 67, 95, 73, 0, 92, 0, 225,
       0, 228, 0, 226, 0, 103, 0, 71, 145, 74,
       0, 1, 0, 0, 146, 164, 0, 147, 0, 146,
      75, 147, 0, 103, 0, 71, 145, 74, 0, 1,
       0, 68, 103, 14, 103, 76, 44, 147, 0, 68,
     103, 76, 44, 147, 0, 68, 103, 76, 147, 0,
      92, 46, 147, 0, 66, 92, 44, 147, 0, 0,
       0, 154, 149, 110, 150, 188, 0, 0, 0, 157,
     152, 110, 153, 188, 0, 155, 0, 157, 0, 67,
     155, 73, 0, 155, 67, 217, 223, 0, 155, 68,
      94, 76, 0, 155, 68, 76, 0, 58, 176, 155,
       0, 138, 116, 155, 0, 4, 0, 156, 67, 217,
     223, 0, 156, 68, 94, 76, 0, 156, 68, 76,
       0, 58, 176, 156, 0, 138, 116, 156, 0, 4,
       0, 157, 67, 217, 223, 0, 67, 157, 73, 0,
      58, 176, 157, 0, 157, 68, 94, 76, 0, 157,
      68, 76, 0, 138, 116, 157, 0, 3, 0, 90,
       0, 0, 17, 158, 71, 160, 166, 74, 137, 0,
      17, 71, 166, 74, 137, 0, 17, 158, 0, 0,
      18, 158, 71, 161, 166, 74, 137, 0, 18, 71,
     166, 74, 137, 0, 18, 158, 0, 0, 16, 158,
      71, 162, 171, 165, 74, 137, 0, 0, 16, 71,
     163, 171, 165, 74, 137, 0, 16, 158, 0, 0,
      75, 0, 0, 75, 0, 167, 0, 167, 168, 0,
       0, 167, 168, 72, 0, 167, 72, 0, 124, 115,
     169, 0, 124, 115, 0, 175, 115, 169, 0, 175,
     115, 0, 1, 0, 224, 168, 0, 170, 0, 169,
      75, 170, 0, 154, 137, 0, 154, 46, 103, 137,
       0, 46, 103, 137, 0, 172, 0, 171, 75, 172,
       0, 1, 0, 90, 0, 90, 44, 103, 0, 124,
     174, 0, 175, 174, 0, 0, 177, 0, 226, 0,
     175, 226, 0, 0, 176, 226, 0, 67, 177, 73,
       0, 58, 176, 177, 0, 58, 176, 0, 177, 67,
     210, 223, 0, 177, 68, 94, 76, 0, 177, 68,
      76, 0, 67, 210, 223, 0, 68, 94, 76, 0,
      68, 76, 0, 179, 0, 194, 0, 179, 194, 0,
     179, 181, 0, 0, 178, 0, 1, 72, 0, 0,
       0, 184, 0, 185, 0, 184, 185, 0, 36, 222,
      72, 0, 188, 0, 1, 188, 0, 71, 0, 187,
     182, 74, 0, 187, 182, 183, 114, 180, 74, 0,
     187, 182, 183, 1, 74, 0, 187, 182, 183, 178,
      74, 0, 190, 193, 0, 190, 1, 0, 19, 67,
      94, 73, 0, 0, 22, 192, 193, 21, 0, 195,
       0, 203, 193, 0, 195, 0, 203, 0, 188, 0,
      94, 72, 0, 0, 189, 20, 196, 193, 0, 189,
       0, 189, 20, 1, 0, 0, 0, 21, 197, 67,
      94, 73, 198, 193, 0, 191, 67, 94, 73, 72,
       0, 191, 1, 0, 0, 0, 0, 23, 67, 205,
      72, 199, 205, 72, 200, 205, 73, 201, 193, 0,
       0, 24, 67, 94, 73, 202, 193, 0, 27, 72,
       0, 28, 72, 0, 29, 72, 0, 29, 94, 72,
       0, 31, 204, 67, 94, 73, 72, 0, 31, 204,
      67, 94, 46, 206, 73, 72, 0, 31, 204, 67,
      94, 46, 206, 46, 206, 73, 72, 0, 31, 204,
      67, 94, 46, 206, 46, 206, 46, 209, 73, 72,
       0, 30, 91, 72, 0, 30, 58, 94, 72, 0,
      72, 0, 40, 67, 98, 75, 173, 73, 72, 0,
      41, 67, 98, 75, 173, 73, 72, 0, 42, 67,
      94, 73, 193, 0, 25, 103, 46, 0, 25, 103,
      14, 103, 46, 0, 26, 46, 0, 91, 46, 0,
       0, 226, 0, 0, 94, 0, 0, 207, 0, 208,
       0, 207, 75, 208, 0, 12, 67, 94, 73, 0,
     107, 0, 209, 75, 107, 0, 0, 211, 212, 0,
     214, 73, 0, 0, 215, 72, 213, 212, 0, 1,
      73, 0, 0, 14, 0, 215, 0, 215, 75, 111,
       0, 216, 0, 215, 75, 216, 0, 118, 115, 156,
     137, 0, 118, 115, 157, 137, 0, 118, 115, 174,
     137, 0, 122, 115, 157, 137, 0, 122, 115, 174,
     137, 0, 0, 218, 219, 0, 212, 0, 220, 73,
       0, 221, 0, 220, 75, 221, 0, 3, 0, 91,
       0, 222, 75, 91, 0, 0, 227, 0, 35, 0,
       7, 0, 9, 0, 5, 0, 10, 0, 5, 0,
       8, 0
};
static const short yyrline[] =
{
       0, 297, 302, 312, 312, 314, 314, 318, 320, 321,
     326, 331, 340, 343, 346, 348, 352, 353, 354, 360,
     360, 360, 370, 373, 373, 373, 382, 385, 385, 385,
     394, 399, 401, 404, 408, 412, 414, 416, 418, 420,
     422, 424, 426, 428, 432, 439, 442, 445, 450, 453,
     457, 459, 462, 465, 472, 477, 487, 490, 493, 498,
     502, 506, 508, 510, 510, 564, 566, 568, 570, 572,
     574, 576, 578, 580, 582, 584, 586, 588, 590, 592,
     594, 596, 596, 602, 604, 608, 615, 616, 617, 619,
     621, 621, 636, 638, 640, 642, 644, 647, 649, 654,
     658, 660, 664, 666, 672, 674, 675, 683, 686, 688,
     690, 699, 701, 702, 703, 710, 714, 717, 722, 731,
     733, 734, 735, 742, 752, 762, 766, 769, 772, 775,
     779, 781, 790, 793, 797, 799, 801, 806, 810, 813,
     817, 820, 822, 834, 836, 837, 839, 843, 845, 846,
     848, 860, 863, 867, 869, 877, 878, 879, 883, 885,
     887, 893, 894, 895, 898, 902, 906, 908, 911, 913,
     916, 919, 924, 924, 931, 937, 937, 944, 949, 952,
     956, 960, 963, 967, 972, 975, 979, 982, 984, 987,
     991, 998, 1000, 1001, 1002, 1007, 1009, 1013, 1018, 1023,
    1026, 1028, 1033, 1035, 1037, 1041, 1043, 1045, 1047, 1049,
    1053, 1053, 1053, 1073, 1073, 1073, 1096, 1098, 1103, 1106,
    1108, 1110, 1112, 1119, 1121, 1129, 1132, 1134, 1136, 1143,
    1145, 1151, 1154, 1156, 1158, 1160, 1167, 1169, 1172, 1176,
    1176, 1183, 1187, 1189, 1189, 1193, 1197, 1199, 1199, 1203,
    1203, 1207, 1211, 1213, 1216, 1218, 1222, 1225, 1230, 1232,
    1234, 1249, 1253, 1259, 1262, 1268, 1270, 1275, 1277, 1281,
    1285, 1288, 1293, 1295, 1297, 1302, 1305, 1309, 1312, 1316,
    1319, 1322, 1324, 1328, 1331, 1335, 1339, 1341, 1343, 1345,
    1347, 1349, 1351, 1353, 1363, 1379, 1381, 1383, 1387, 1389,
    1392, 1395, 1401, 1403, 1409, 1411, 1414, 1421, 1423, 1426,
    1429, 1431, 1433, 1436, 1441, 1445, 1448, 1459, 1459, 1471,
    1474, 1478, 1481, 1486, 1489, 1492, 1492, 1500, 1509, 1511,
    1511, 1511, 1523, 1530, 1533, 1533, 1533, 1533, 1542, 1542,
    1551, 1556, 1561, 1565, 1569, 1574, 1579, 1583, 1587, 1592,
    1598, 1599, 1601, 1603, 1611, 1614, 1617, 1620, 1627, 1630,
    1633, 1636, 1641, 1643, 1646, 1648, 1652, 1659, 1662, 1668,
    1668, 1676, 1678, 1678, 1685, 1691, 1693, 1703, 1705, 1709,
    1711, 1717, 1722, 1726, 1730, 1734, 1743, 1743, 1750, 1752,
    1756, 1759, 1763, 1768, 1770, 1776, 1778, 1781, 1788, 1792,
    1794, 1798, 1800, 1804
};
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "IDENTIFIER", "TYPENAME", "USER_QUALIFIER",
  "TYPE_VARIABLE", "SCSPEC", "TYPESPEC", "TYPE_QUAL", "FN_QUAL",
  "CONSTANT", "STRING", "MAGIC_STRING", "ELLIPSIS", "SIZEOF", "ENUM",
  "STRUCT", "UNION", "IF", "ELSE", "WHILE", "DO", "FOR", "SWITCH", "CASE",
  "DEFAULT", "BREAK", "CONTINUE", "RETURN", "GOTO", "ASM_KEYWORD",
  "TYPEOF", "ALIGNOF", "ATTRIBUTE", "EXTENSION", "LABEL", "REALPART",
  "IMAGPART", "VA_ARG", "ASSERT_TYPE", "CHANGE_TYPE", "DEEP_RESTRICT",
  "ASSIGN", "'='", "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'",
  "EQCOMPARE", "ARITHCOMPARE", "LSHIFT", "RSHIFT", "'+'", "'-'", "'*'",
  "'/'", "'%'", "UNARY", "PLUSPLUS", "MINUSMINUS", "HYPERUNARY",
  "POINTSAT", "'.'", "'('", "'['", "'~'", "'!'", "'{'", "';'", "')'",
  "'}'", "','", "']'", "program", "extdefs", "@1", "@2", "extdef",
  "datadef", "fndef", "@3", "@4", "@5", "@6", "@7", "@8", "identifier",
  "id_label", "idword", "unop", "expr", "exprlist", "nonnull_exprlist",
  "nonnull_exprlist_", "unary_expr", "sizeof", "alignof", "cast_expr",
  "@9", "expr_no_commas", "@10", "primary", "@11", "string",
  "string_list", "string_component", "old_style_parm_decls",
  "ellipsis_decl", "datadecls", "datadecl", "decls", "setspecs",
  "setattrs", "decl", "typed_declspecs", "reserved_declspecs",
  "typed_declspecs_no_prefix_attr", "reserved_declspecs_no_prefix_attr",
  "declmods", "declmods_no_prefix_attr", "typed_typespecs",
  "reserved_typespecquals", "typespec", "typespecqual_reserved",
  "initdecls", "notype_initdecls", "initdecls_", "notype_initdecls_",
  "maybeasm", "initdcl", "@12", "notype_initdcl", "@13",
  "maybe_attribute", "attributes", "attributes_", "attribute",
  "attribute_list", "attrib", "any_word", "init", "initlist_maybe_comma",
  "initlist1", "initelt", "nested_function", "@14", "@15",
  "notype_nested_function", "@16", "@17", "declarator",
  "after_type_declarator", "parm_declarator", "notype_declarator", "tag",
  "structsp", "@18", "@19", "@20", "@21", "maybecomma", "maybecomma_warn",
  "component_decl_list", "component_decl_list2", "component_decl",
  "components", "component_declarator", "enumlist", "enumerator",
  "typename", "absdcl", "nonempty_type_quals", "type_quals", "absdcl1",
  "stmts", "stmt_or_labels", "xstmts", "errstmt", "pushlevel",
  "maybe_label_decls", "label_decls", "label_decl", "compstmt_or_error",
  "compstmt_start", "compstmt", "simple_if", "if_prefix", "do_stmt_start",
  "@22", "labeled_stmt", "stmt_or_label", "stmt", "@23", "@24", "@25",
  "@26", "@27", "@28", "@29", "label", "maybe_type_qual", "xexpr",
  "asm_operands", "nonnull_asm_operands", "asm_operand", "asm_clobbers",
  "parmlist", "@30", "parmlist_1", "@31", "parmlist_2", "parms", "parm",
  "parmlist_or_identifiers", "@32", "parmlist_or_identifiers_1",
  "identifiers", "old_parameter", "identifiers_or_typenames", "fn_quals",
  "extension", "scspec", "type_qual", "fn_qual", "type_spec", 0
};
static const short yyr1[] =
{
       0, 77, 77, 79, 78, 80, 78, 81, 81, 81,
      81, 82, 82, 82, 82, 82, 82, 82, 82, 84,
      85, 83, 83, 86, 87, 83, 83, 88, 89, 83,
      83, 90, 90, 91, 92, 93, 93, 93, 93, 93,
      93, 93, 93, 93, 94, 95, 95, 96, 97, 97,
      98, 98, 98, 98, 98, 98, 98, 98, 98, 99,
     100, 101, 101, 102, 101, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 104, 103, 103, 103, 105, 105, 105, 105, 105,
     106, 105, 105, 105, 105, 105, 105, 105, 105, 107,
     108, 108, 109, 109, 110, 110, 110, 111, 111, 111,
     111, 112, 112, 112, 112, 113, 113, 113, 113, 114,
     114, 114, 114, 115, 116, 117, 117, 117, 117, 117,
     117, 117, 118, 118, 119, 119, 119, 119, 120, 120,
     121, 121, 121, 122, 122, 122, 122, 123, 123, 123,
     123, 124, 124, 125, 125, 126, 126, 126, 126, 126,
     126, 127, 127, 127, 128, 129, 130, 130, 131, 131,
     132, 132, 134, 133, 133, 136, 135, 135, 137, 137,
     138, 139, 139, 140, 141, 141, 142, 142, 142, 142,
     142, 143, 143, 143, 143, 144, 144, 144, 145, 145,
     146, 146, 147, 147, 147, 147, 147, 147, 147, 147,
     149, 150, 148, 152, 153, 151, 154, 154, 155, 155,
     155, 155, 155, 155, 155, 156, 156, 156, 156, 156,
     156, 157, 157, 157, 157, 157, 157, 157, 158, 160,
     159, 159, 159, 161, 159, 159, 159, 162, 159, 163,
     159, 159, 164, 164, 165, 165, 166, 166, 167, 167,
     167, 168, 168, 168, 168, 168, 168, 169, 169, 170,
     170, 170, 171, 171, 171, 172, 172, 173, 173, 174,
     174, 175, 175, 176, 176, 177, 177, 177, 177, 177,
     177, 177, 177, 177, 178, 179, 179, 179, 180, 180,
     181, 182, 183, 183, 184, 184, 185, 186, 186, 187,
     188, 188, 188, 188, 189, 189, 190, 192, 191, 193,
     193, 194, 194, 195, 195, 196, 195, 195, 195, 197,
     198, 195, 195, 195, 199, 200, 201, 195, 202, 195,
     195, 195, 195, 195, 195, 195, 195, 195, 195, 195,
     195, 195, 195, 195, 203, 203, 203, 203, 204, 204,
     205, 205, 206, 206, 207, 207, 208, 209, 209, 211,
     210, 212, 213, 212, 212, 214, 214, 214, 214, 215,
     215, 216, 216, 216, 216, 216, 218, 217, 219, 219,
     220, 220, 221, 222, 222, 223, 223, 224, 225, 226,
     226, 227, 227, 228
};
static const short yyr2[] =
{
       0, 0, 1, 0, 2, 0, 3, 1, 1, 5,
       2, 3, 4, 4, 2, 3, 2, 2, 1, 0,
       0, 7, 4, 0, 0, 7, 4, 0, 0, 6,
       3, 1, 1, 1, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 0, 1, 1, 1, 3,
       1, 2, 2, 2, 2, 2, 4, 2, 4, 1,
       1, 1, 4, 0, 7, 1, 3, 3, 3, 3,
       3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
       5, 0, 5, 3, 3, 1, 1, 1, 3, 3,
       0, 4, 4, 6, 4, 3, 3, 2, 2, 1,
       1, 2, 1, 1, 0, 1, 2, 1, 2, 2,
       3, 1, 1, 2, 2, 4, 4, 3, 2, 1,
       1, 2, 2, 0, 0, 4, 4, 3, 3, 3,
       2, 2, 2, 3, 0, 2, 2, 2, 2, 3,
       0, 2, 2, 1, 1, 2, 2, 1, 1, 2,
       2, 2, 3, 0, 2, 1, 1, 1, 1, 4,
       4, 1, 1, 1, 1, 1, 1, 3, 1, 3,
       0, 4, 0, 6, 3, 0, 6, 3, 0, 1,
       1, 1, 2, 6, 1, 3, 0, 1, 4, 6,
       4, 1, 1, 1, 1, 1, 3, 1, 0, 2,
       1, 3, 1, 3, 1, 7, 5, 4, 3, 4,
       0, 0, 5, 0, 0, 5, 1, 1, 3, 4,
       4, 3, 3, 3, 1, 4, 4, 3, 3, 3,
       1, 4, 3, 3, 4, 3, 3, 1, 1, 0,
       7, 5, 2, 0, 7, 5, 2, 0, 8, 0,
       7, 2, 0, 1, 0, 1, 1, 2, 0, 3,
       2, 3, 2, 3, 2, 1, 2, 1, 3, 2,
       4, 3, 1, 3, 1, 1, 3, 2, 2, 0,
       1, 1, 2, 0, 2, 3, 3, 2, 4, 4,
       3, 3, 3, 2, 1, 1, 2, 2, 0, 1,
       2, 0, 0, 1, 1, 2, 3, 1, 2, 1,
       3, 6, 5, 5, 2, 2, 4, 0, 4, 1,
       2, 1, 1, 1, 2, 0, 4, 1, 3, 0,
       0, 7, 5, 2, 0, 0, 0, 12, 0, 6,
       2, 2, 2, 3, 6, 8, 10, 12, 3, 4,
       1, 7, 7, 5, 3, 5, 2, 2, 0, 1,
       0, 1, 0, 1, 1, 3, 4, 1, 3, 0,
       2, 2, 0, 4, 2, 0, 1, 1, 3, 1,
       3, 4, 4, 4, 4, 4, 0, 2, 1, 2,
       1, 3, 1, 1, 3, 0, 1, 1, 1, 1,
       1, 1, 1, 1
};
static const short yydefact[] =
{
       3, 5, 0, 0, 0, 157, 400, 158, 398, 403,
     399, 0, 0, 0, 0, 0, 0, 397, 18, 4,
       8, 7, 0, 123, 123, 143, 134, 144, 180, 181,
     156, 0, 148, 147, 155, 6, 16, 17, 31, 32,
     249, 238, 251, 258, 242, 258, 246, 0, 0, 0,
     237, 283, 0, 0, 165, 168, 124, 0, 0, 14,
       0, 145, 134, 146, 150, 149, 132, 182, 10, 0,
     247, 0, 0, 239, 0, 243, 85, 86, 102, 103,
      59, 60, 42, 43, 0, 0, 35, 37, 36, 0,
      38, 39, 0, 40, 41, 0, 0, 44, 47, 61,
       0, 0, 65, 48, 50, 87, 99, 100, 0, 0,
     279, 153, 0, 279, 281, 186, 0, 0, 11, 0,
       0, 30, 0, 386, 0, 0, 178, 224, 283, 0,
      15, 0, 164, 166, 124, 0, 216, 217, 0, 0,
     133, 135, 137, 163, 136, 162, 161, 274, 275, 254,
     272, 0, 178, 265, 260, 123, 257, 123, 0, 258,
     178, 258, 0, 33, 54, 51, 0, 0, 0, 0,
      53, 0, 0, 0, 55, 0, 57, 0, 0, 81,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 97, 98, 0, 0, 45, 0,
     101, 52, 159, 283, 369, 0, 277, 280, 151, 160,
     153, 278, 282, 34, 191, 0, 184, 187, 192, 194,
     193, 233, 284, 232, 169, 170, 236, 0, 395, 0,
     235, 0, 0, 28, 105, 0, 123, 123, 140, 112,
     177, 179, 0, 0, 13, 0, 0, 22, 0, 178,
     386, 0, 12, 26, 0, 0, 255, 0, 254, 241,
     262, 259, 264, 266, 0, 245, 0, 0, 89, 88,
     309, 301, 0, 0, 9, 49, 0, 0, 84, 83,
       0, 0, 79, 78, 76, 77, 75, 74, 73, 71,
      72, 66, 67, 68, 69, 70, 96, 95, 0, 46,
       0, 287, 0, 395, 0, 293, 0, 369, 0, 154,
     152, 0, 186, 45, 0, 402, 401, 231, 396, 0,
     392, 376, 123, 123, 388, 0, 377, 379, 387, 0,
     390, 234, 300, 0, 107, 106, 113, 147, 114, 0,
     118, 0, 140, 138, 175, 222, 218, 167, 223, 20,
     174, 395, 221, 0, 24, 276, 273, 178, 0, 0,
     178, 261, 267, 263, 178, 178, 0, 302, 91, 63,
      62, 56, 58, 0, 0, 92, 94, 286, 285, 291,
     370, 292, 395, 290, 0, 183, 185, 85, 0, 171,
     374, 279, 279, 371, 372, 0, 389, 0, 0, 29,
     307, 109, 108, 117, 0, 0, 170, 139, 141, 142,
       0, 0, 172, 219, 220, 0, 250, 178, 178, 0,
     269, 0, 240, 244, 0, 0, 310, 0, 303, 304,
       0, 80, 82, 288, 289, 188, 0, 190, 230, 283,
     369, 124, 178, 178, 178, 283, 178, 178, 0, 378,
     380, 391, 308, 110, 115, 116, 197, 0, 195, 176,
      21, 0, 25, 248, 271, 178, 268, 93, 393, 0,
       0, 85, 157, 0, 329, 317, 0, 0, 0, 0,
       0, 0, 0, 0, 358, 0, 0, 0, 350, 0,
       0, 298, 0, 123, 123, 0, 0, 120, 323, 327,
       0, 0, 295, 321, 322, 0, 305, 204, 0, 0,
       0, 0, 202, 0, 252, 200, 0, 287, 0, 386,
       0, 381, 382, 383, 287, 384, 385, 373, 0, 173,
     270, 306, 0, 312, 0, 0, 0, 360, 0, 0,
     356, 340, 341, 342, 0, 0, 0, 0, 359, 0,
       0, 0, 357, 324, 121, 299, 0, 122, 0, 130,
       0, 313, 297, 296, 0, 315, 314, 319, 0, 333,
       0, 131, 0, 0, 0, 0, 64, 0, 199, 189,
     228, 283, 229, 395, 227, 0, 196, 394, 0, 0,
       0, 361, 0, 0, 0, 354, 343, 0, 348, 0,
       0, 0, 0, 0, 311, 129, 0, 127, 210, 0,
     128, 213, 328, 0, 320, 0, 0, 0, 0, 203,
     208, 201, 0, 225, 226, 316, 0, 318, 334, 338,
       0, 349, 0, 0, 0, 0, 125, 0, 126, 0,
     326, 0, 209, 0, 0, 207, 330, 360, 0, 355,
     362, 0, 0, 0, 353, 211, 214, 332, 0, 206,
       0, 0, 339, 0, 0, 363, 364, 344, 0, 0,
       0, 0, 0, 331, 335, 0, 362, 0, 0, 351,
     352, 212, 215, 205, 360, 0, 0, 345, 365, 0,
     366, 0, 0, 336, 367, 0, 346, 0, 0, 0,
     337, 347, 368, 0, 0, 0
};
static const short yydefgoto[] =
{
     703, 1, 2, 3, 19, 20, 21, 248, 411, 254,
     415, 125, 333, 163, 489, 511, 95, 490, 298, 97,
      98, 99, 100, 101, 102, 430, 103, 281, 104, 168,
     105, 106, 107, 233, 335, 234, 235, 491, 22, 120,
     492, 322, 66, 236, 343, 323, 25, 110, 208, 26,
     141, 131, 53, 132, 54, 126, 133, 461, 55, 410,
     240, 241, 28, 29, 215, 216, 217, 459, 513, 514,
     515, 607, 637, 670, 610, 639, 671, 225, 136, 580,
     137, 42, 30, 159, 161, 151, 69, 578, 257, 71,
      72, 156, 361, 362, 149, 150, 112, 206, 113, 116,
     207, 495, 496, 556, 239, 367, 427, 428, 429, 399,
     271, 498, 499, 500, 501, 536, 566, 502, 567, 613,
     535, 660, 647, 684, 697, 648, 568, 547, 592, 664,
     665, 666, 695, 303, 304, 324, 448, 325, 326, 327,
     228, 229, 328, 329, 330, 469, 317, 108, 32, 33,
     318, 34
};
static const short yypact[] =
{
      77, 92, 1772, 1772, 184,-32768,-32768,-32768,-32768,-32768,
  -32768, 102, 110, 112, 113, 131, 136,-32768,-32768,-32768,
  -32768,-32768, 300,-32768, 710, 323,-32768, 99,-32768,-32768,
  -32768, 1772,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768, 146,-32768, 157,-32768, 161, 2537, 2476, 169,
  -32768,-32768, 300, 178, 192,-32768, 99, 704, 172,-32768,
     300, 323,-32768, 99,-32768,-32768, 1081,-32768,-32768, 375,
  -32768, 216, 581,-32768, 228,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768, 252, 348,-32768,-32768,-32768, 2537,
  -32768,-32768, 1808,-32768,-32768, 2537, 260,-32768, 266,-32768,
    2598, 2659,-32768, 3004, 421,-32768, 381,-32768, 2537, 275,
     175,-32768, 283, 991,-32768, 1458, 250, 225,-32768, 90,
     300,-32768, 292,-32768, 1234, 2034, 99,-32768,-32768, 90,
  -32768, 293, 307,-32768, 99, 792, 341, 392, 303, 746,
    1081,-32768, 99,-32768,-32768,-32768,-32768,-32768, 346, 331,
  -32768, 375, 99,-32768,-32768,-32768, 326, 1105, 2916,-32768,
      99,-32768, 2537,-32768,-32768,-32768, 343, 350, 355, 356,
  -32768, 359, 2537, 1808,-32768, 1808,-32768, 2537, 2537, 2537,
    2537, 2537, 2537, 2537, 2537, 2537, 2537, 2537, 2537, 2537,
    2537, 2537, 2537, 2537,-32768,-32768, 348, 348, 2537, 2537,
  -32768,-32768,-32768,-32768, 175, 1296,-32768, 407, 649,-32768,
  -32768,-32768,-32768,-32768,-32768, 222,-32768, 372,-32768,-32768,
  -32768, 392,-32768,-32768,-32768, 410, 392, 381, 98, 826,
  -32768, 373, 380,-32768, 1432, 1247,-32768, 938,-32768,-32768,
     411, 99, 534, 282,-32768, 90, 90,-32768, 2034, 99,
  -32768, 1358,-32768,-32768, 2034, 2537, 348, 383, 331,-32768,
     466,-32768, 466,-32768, 436,-32768, 441, 2939,-32768,-32768,
  -32768,-32768, 425, 2339,-32768, 3004, 446, 452, 3004, 3004,
     481, 496, 1847, 1742, 931, 794, 1914, 1469, 729, 377,
     377, 476, 476,-32768,-32768,-32768,-32768,-32768, 471,-32768,
     474, 316, 324, 98, 960,-32768, 475,-32768, 1420,-32768,
     649, 479, 1458, 2720, 484,-32768,-32768,-32768,-32768, 486,
  -32768,-32768,-32768, 1370,-32768, 490, 144,-32768,-32768, 241,
  -32768,-32768,-32768, 55, 69,-32768,-32768, 551,-32768, 257,
  -32768, 300,-32768, 553,-32768, 341,-32768,-32768, 341,-32768,
     523, 98,-32768, 497,-32768, 3004,-32768, 99, 500, 2537,
     154, 501,-32768, 501, 99, 99, 1105, 35,-32768,-32768,
  -32768,-32768,-32768, 2537, 2537,-32768,-32768, 407,-32768,-32768,
  -32768,-32768, 98,-32768, 503,-32768,-32768, 264, 504,-32768,
  -32768, 424, 179,-32768,-32768, 1308,-32768, 575, 355,-32768,
  -32768,-32768, 69,-32768, 516, 519, 122, 553,-32768,-32768,
    2021, 55,-32768,-32768,-32768, 55,-32768, 99, 2968, 2537,
  -32768, 466,-32768,-32768, 522, 348,-32768, 1556, 560,-32768,
    1091, 1453, 1453,-32768,-32768,-32768, 2537,-32768,-32768,-32768,
     179, 99, 52, 238, 99,-32768, 238, 99, 960,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768, 1091, 3004,-32768,
  -32768, 2021,-32768,-32768,-32768, 2968,-32768,-32768,-32768, 207,
     298, 556, 558, 539,-32768,-32768, 542, 543, 2537, 565,
     540, 547, 2277, 63, 69, 555, 562, 563,-32768, 568,
     566, 2153, 1013,-32768, 1178, 561, 867,-32768,-32768, 607,
    1628, 40,-32768,-32768,-32768, 2408,-32768,-32768, 348, 2537,
    1091, 586, 3004, 567, 559,-32768, 564, 398, 550,-32768,
    1482,-32768,-32768,-32768, 220,-32768,-32768,-32768, 569,-32768,
  -32768,-32768, 348,-32768, 2537, 577, 2215, 2537, 2537, 2856,
  -32768,-32768,-32768,-32768, 570, 2537, 574, 580,-32768, 2781,
    2781, 2537,-32768,-32768,-32768,-32768, 578,-32768, 273,-32768,
     300,-32768,-32768,-32768, 1700,-32768,-32768,-32768, 2215,-32768,
    2537,-32768, 605, 2809, 588, 1950,-32768, 1165,-32768,-32768,
     435,-32768, 435, 98,-32768, 587,-32768,-32768, 595, 2537,
     630,-32768, 597, 598, 2537,-32768,-32768, 601,-32768, 2537,
    2084, 585, 600, 603,-32768,-32768, 606,-32768, 584, 624,
  -32768, 313,-32768, 2215,-32768, 604, 1950, 2537, 1879,-32768,
  -32768,-32768, 698,-32768,-32768,-32768, 626,-32768,-32768,-32768,
    2986,-32768, 26, 1105, 1105, 2215,-32768, 2034,-32768, 2034,
  -32768, 625,-32768, 2909, 1950,-32768,-32768, 2537, 2215,-32768,
     658, 628, 633, 650,-32768,-32768,-32768,-32768, 680,-32768,
    2215, 653,-32768, 662, 145, 655,-32768,-32768, 659, 665,
     355, 355, 1950,-32768,-32768, 2537, 658, 667, 658,-32768,
  -32768,-32768,-32768,-32768, 2537, 661, 165,-32768,-32768, 668,
  -32768, 381, 671,-32768,-32768, 314,-32768, 2215, 674, 381,
  -32768,-32768,-32768, 749, 758,-32768
};
static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768, 66,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768, -11, -71, -103,-32768, -44, 447, -166,
  -32768, -77,-32768,-32768, -58,-32768, 502,-32768,-32768,-32768,
    -214,-32768, 660, -226, 364,-32768, 527,-32768, 38, -105,
    -418, 15, 705,-32768, 426, 18, -14, -12, 571, -13,
    -156, -301, -55,-32768,-32768, -129, -68,-32768,-32768,-32768,
     -22, 23,-32768, 7,-32768, 457,-32768, 309, -393,-32768,
    -450,-32768,-32768,-32768,-32768,-32768,-32768, -50, -72, -351,
      76, 517, -30,-32768,-32768,-32768,-32768,-32768, 515, -3,
  -32768, 616, 529, 371, 643, 546, -85, -80, -7, -119,
    -151, 304,-32768,-32768, -196,-32768,-32768,-32768, 376, -207,
  -32768, -149,-32768,-32768,-32768,-32768, -434, 310, -359,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768, -332,-32768, -482, 127,
  -32768, 129,-32768, 498,-32768, -249,-32768,-32768,-32768, 416,
    -220,-32768,-32768,-32768, 415,-32768, -260, 13, 57, 338,
  -32768, -39
};
static const short yytable[] =
{
      41, 41, 41, 96, 109, 138, 249, 169, 135, 242,
      61, 62, 214, 314, 164, 31, 31, 23, 23, 272,
      24, 24, 349, 174, 176, 27, 27, 146, 354, 246,
     351, 165, 299, 211, 67, 111, 143, 170, 404, 338,
     442, 569, 74, 379, 31, 56, 23, 63, 167, 24,
     201, 224, 309, 302, 27, 380, 398, 243, 148, 111,
     155, 58, 60, 67, 528, 157, 38, 39, 503, 35,
      67, 425, 650, 554, 6, 56, 220, -1, 10, 111,
     231, 134, 64, 56, 301, 158, 16, 571, 276, 142,
     277, 413, -2, 50, 127, 504, 249, 68, 57, 651,
     210, 146, 590, 315, 213, 38, 39, 570, 316, 426,
     143, 237, 238, 38, 39, 38, 39, 574, 64, 519,
     520, 545, 433, 144, 16, 620, 270, 621, 117, 167,
     259, 167, 503, 16, 614, 280, 139, 503, 265, 56,
     148, 67, 134, 56, 210, 111, 155, 299, 128, 67,
     377, 157, 134, 122, 309, 300, 264, 129, 266, 504,
     111, 306, 111, 142, 504, 661, 642, 582, 645, 146,
     345, 158, 218, 40, 348, 50, 127, 347, 143, 640,
      47, 43, 50, 45, 400, 296, 297, 408, 16, 123,
     124, 676, 221, 260, 659, 262, 226, 144, 48, 527,
     419, 654, 689, 49, 460, 117, 16, 353, 462, 214,
     360, 691, 360, 16, 662, 370, 394, 70, 677, 395,
     237, 238, 683, 50, 342, 6, 673, 350, 73, 10,
     128, 497, 75, 203, 237, 238, 115, 445, 692, 129,
     237, 238, 204, 205, 130, 148, 440, 205, 67, 452,
     118, 408, 27, 50, 16, 6, 36, 606, 37, 10,
      50, 127, 400, 700, 384, 134, 400, 119, 134, 134,
     516, 146, 16, 220, 339, 341, 50, 127, 445, 531,
     143, 424, 532, 134, 16, 134, 405, 440, 205, 302,
     152, 16, 123, 124, 64, 311, 557, 312, 223, 583,
     562, 213, 160, 50, 146, 123, 124, 16, 51, 61,
      62, 444, 447, 143, 396, 128, 397, 52, 221, 162,
     517, 6, 226, 623, 129, 10, 524, 27, 6, 403,
       8, 128, 10, 171, 16, 416, 518, 435, 420, 436,
     129, 172, 422, 423, 122, 605, 63, -170, 202, 250,
     251, 38, 39, 111, 468, 346, 209, -170, 51, 227,
     391, 392, 134, 65, 56, 244, 377, 52, 146, 218,
     332, 360, 533, 377, 203, 252, 147, 143, 38, 39,
     123, 124, 245, 204, 205, -170, 114, 698, -170, 699,
     255, 307, 308, 78, 79, 463, 464, 378, 261, 65,
     409, 50, 438, 6, 145, 572, 256, 10, 250, 251,
     114, 655, 546, 656, 441, 56, 268, 406, 27, 213,
     521, 522, 523, 269, 525, 526, 270, 50, 438, 273,
     114, 274, 16, 189, 190, 191, 192, 193, 544, 313,
     505, 122, 493, 530, 134, 494, 213, 201, 67, 331,
      27, 212, 332, 219, 222, 344, 439, 357, 16, 123,
     124, 587, 622, 56, 409, 440, 205, 443, 446, 50,
     127, 27, 601, 602, 307, 308, 585, 694, 145, 249,
      61, 62, 439, 194, 195, 702, 196, 197, 198, 199,
     588, 440, 205, 591, 593, 212, 114, 213, 368, 213,
      16, 597, 519, 520, 505, 609, 493, 603, 608, 494,
     364, 114, 359, 114, 27, 365, 117, 63, 505, 371,
     493, 681, 682, 494, 128, 372, 615, 373, 27, 44,
      46, 558, 560, 129, 191, 192, 193, 50, 127, 6,
     441, 441, 374, 10, 375, 626, 145, 56, 652, 653,
     376, 381, 385, 50, 438, 632, 167, 389, 6, 390,
       8, 9, 10, 393, 213, 402, 213, 412, 16, 11,
      12, 13, 337, 414, 417, 65, 421, 437, 320, 434,
     222, 134, 153, 56, 16, 5, 6, 7, 454, 9,
      10, 455, 128, 221, 226, 467, 425, 11, 12, 13,
     221, 129, -31, 591, -32, 213, 534, 213, 581, 537,
     538, 540, 541, 15, 552, 122, 17, 52, -170, 542,
     111, 111, 549, 237, 238, 237, 238, 564, -170, 550,
     551, 685, 575, 213, 577, 561, 611, 579, 553, 222,
     591, 576, 596, 586, 589, 441, 598, 599, 145, 616,
     219, 627, 604, 154, 6, -256, -170, 9, 10, -170,
     633, 213, 619, 624, 267, 11, 12, 13, 625, 628,
     663, 629, 401, 631, 275, 634, 635, 641, 636, 278,
     279, 145, 282, 283, 284, 285, 286, 287, 288, 289,
     290, 291, 292, 293, 294, 295, 638, 657, 221, 646,
     667, 50, 438, 6, 114, 121, 668, 10, -27, -27,
     -27, -27, -27, -27, 5, 6, 7, 8, 9, 10,
     -27, -27, -27, 669, 672, 674, 11, 12, 13, 675,
     678, 679, 16, 337, 690, 122, -27, 680, -170, 687,
     453, 693, 15, 696, 16, 145, 701, 253, -170, 704,
     -23, -23, -23, -23, -23, -23, 581, 355, 705, 449,
     388, 336, -23, -23, -23, 52, 200, 140, 407, 386,
     529, 123, 124, 358, 263, -27, -170, 122, -23, -170,
    -170, 310, 59, 187, 188, 189, 190, 191, 192, 193,
    -170, 363, 466, 247, 258, 555, -19, -19, -19, -19,
     -19, -19, 356, 686, 506, 382, 563, 688, -19, -19,
     -19, 450, 451, 123, 124, 0, 0, -23, -170, 0,
       0, -170, 548, 122, -19, 0, -170, 319, 0, 320,
       5, 6, 7, 8, 9, 10, -170, 0, 0, 0,
     321, 0, 11, 12, 13, 184, 185, 186, 187, 188,
     189, 190, 191, 192, 193, 222, 0, 0, 15, 0,
      16, 418, 222, -19, -170, 0, 0, -170, 232, 0,
     471, 39, 0, 0, 0, 431, 432, 0, 77, 78,
      79, 0, 80, 0, 0, 0, 473, 0, 474, 475,
     476, 477, 478, 479, 480, 481, 482, 483, 484, -375,
      81, 0, 17, 0, 82, 83, 84, 485, 486, 487,
       0, 0, 458, 0, 0, 85, 0, 0, 86, 0,
       0, 465, 0, 87, 88, 89, 0, 0, 0, 90,
      91, 0, 512, 0, 92, 0, 93, 94, 270, 488,
       0, -294, 5, 6, 7, 8, 9, 10, 0, 0,
       0, 0, 0, 0, 11, 12, 13, 0, 0, 512,
     222, 319, 0, 458, 5, 6, 7, 8, 9, 10,
      15, 114, 114, 0, 321, 0, 11, 12, 13, 0,
     539, 183, 184, 185, 186, 187, 188, 189, 190, 191,
     192, 193, 15, 0, 16, 5, 6, 7, 0, 9,
      10, 0, 0, 0, 0, 0, 0, 11, 12, 13,
     340, 573, 512, 0, 232, 0, -119, -119, -119, -119,
    -119, -119, -119, 15, -119, -119, -119, 0, -119, -119,
    -119, -119, -119, -375, -119, -119, -119, -119, -119, -119,
    -119, -119, -119, -119, -119, -119, -119, -119, -119, 203,
    -119, -119, -119, -119, -119, -119, 0, 0, 204, 205,
       0, -119, 0, 0, -119, 0, 0, 0, 0, -119,
    -119, -119, 0, 0, 0, -119, -119, 512, 0, 512,
    -119, 0, -119, -119, -119, -119, 6, -119, 8, 9,
      10, 0, 507, 0, 471, 39, 630, 11, 12, 13,
       0, 0, 77, 78, 79, 0, 80, 0, 0, 5,
       6, 7, 0, 9, 10, 16, 0, 0, 512, 643,
     512, 11, 12, 13, 81, 0, 17, 0, 82, 83,
      84, 0, 0, 0, 0, 0, 0, 15, 0, 85,
       0, 0, 86, 0, 0, 0, 512, 87, 88, 89,
       0, 0, 0, 90, 91, 0, 0, 508, 92, 509,
      93, 94, 510, 0, 0, -198, 507, 0, 471, 39,
       0, 0, 0, 0, 512, 0, 77, 78, 79, 0,
      80, 0, 5, 6, 7, 8, 9, 10, 0, 0,
       0, 0, 0, 0, 11, 12, 13, 0, 81, 0,
      17, 0, 82, 83, 84, 0, 0, 0, 0, 0,
      15, 0, 16, 85, 0, 0, 86, 0, 0, 0,
       0, 87, 88, 89, 0, 0, 0, 90, 91, 0,
       0, 508, 92, 509, 93, 94, 510, 76, 0, -253,
       0, 0, 0, 0, 0, 77, 78, 79, 232, 80,
     559, -111, -111, -111, -111, -111, -111, 0, 0, 0,
       0, -111, 0, -111, -111, -111, 0, 81, 0, 17,
       0, 82, 83, 84, 0, 0, 0, 0, 0, -111,
       0, 0, 85, 0, 0, 86, 0, 0, 0, 0,
      87, 88, 89, 0, 0, 0, 90, 91, 0, 76,
       0, 92, 0, 93, 94, 0, 0, 77, 78, 79,
     230, 80, 5, 6, 7, 8, 9, 10, -111, 0,
       0, 0, 334, 0, 11, 12, 13, 0, 0, 81,
       0, 17, 0, 82, 83, 84, 0, 0, 0, 0,
      15, 0, 16, 0, 85, 0, 0, 86, 0, 0,
       0, 0, 87, 88, 89, 0, 0, 0, 90, 91,
       0, 76, 0, 92, 0, 93, 94, 0, 0, 77,
      78, 79, 305, 80, 5, 6, 7, 8, 9, 10,
       0, 0, 0, 0, 0, 0, 11, 12, 13, 0,
       0, 81, 0, 17, 0, 82, 83, 84, 0, 0,
       0, 0, 15, 0, 16, 0, 85, 0, 0, 86,
       0, 0, 0, 0, 87, 88, 89, 0, 0, 0,
      90, 91, 0, 76, 0, 92, 0, 93, 94, 0,
       0, 77, 78, 79, 352, 80, 5, 6, 7, 8,
       9, 10, 0, 0, 0, 0, 334, 0, 11, 12,
      13, 0, 0, 81, 0, 17, 0, 82, 83, 84,
       0, 38, 39, 6, 15, 8, 9, 10, 85, 0,
       0, 86, 0, 0, 0, 0, 87, 88, 89, 0,
       0, 0, 90, 91, 0, 76, 0, 92, 0, 93,
      94, 0, 0, 77, 78, 79, 383, 80, 179, 0,
     180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
     190, 191, 192, 193, 0, 81, 0, 17, 0, 82,
      83, 84, 186, 187, 188, 189, 190, 191, 192, 193,
      85, 0, 0, 86, 0, 0, 0, 0, 87, 88,
      89, 0, 0, 0, 90, 91, 0, 0, 0, 92,
       0, 93, 94, 0, 0, 0, 0, 470, 584, 471,
     472, 6, 7, 8, 9, 10, 0, 77, 78, 79,
       0, 80, 11, 12, 13, 473, 0, 474, 475, 476,
     477, 478, 479, 480, 481, 482, 483, 484, 15, 81,
      16, 17, 0, 82, 83, 84, 485, 486, 487, 0,
       0, 0, 0, 0, 85, 0, 0, 86, 0, 0,
       0, 0, 87, 88, 89, 0, 0, 0, 90, 91,
       0, 0, 0, 92, 0, 93, 94, 270, 488, 565,
       0, 471, 39, 0, 0, 0, 0, 0, 0, 77,
      78, 79, 0, 80, 0, 0, 0, 473, 0, 474,
     475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
       0, 81, 0, 17, 0, 82, 83, 84, 485, 486,
     487, 0, 0, 0, 0, 0, 85, 0, 0, 86,
       0, 0, 0, 0, 87, 88, 89, 0, 0, 0,
      90, 91, 0, 0, 0, 92, 0, 93, 94, 270,
     488, 612, 0, -325, -325, 0, 0, 0, 0, 0,
       0, -325, -325, -325, 0, -325, 0, 0, 0, -325,
       0, -325, -325, -325, -325, -325, -325, -325, -325, -325,
    -325, -325, 0, -325, 0, -325, 0, -325, -325, -325,
    -325, -325, -325, 0, 0, 0, 0, 0, -325, 0,
       0, -325, 0, 0, 0, 0, -325, -325, -325, 0,
       0, 0, -325, -325, 0, 0, 0, -325, 0, -325,
    -325, -325, -325, 4, 0, -123, 5, 6, 7, 8,
       9, 10, 0, 0, 0, 0, 0, 0, 11, 12,
      13, 182, 183, 184, 185, 186, 187, 188, 189, 190,
     191, 192, 193, 14, 15, 0, 16, 17, 0, 166,
       0, 76, 5, 6, 7, 0, 9, 10, 0, 77,
      78, 79, 0, 80, 11, 12, 13, 0, 0, 0,
    -123, 0, 0, 0, 0, 0, 0, 0, 0, -123,
      15, 81, 0, 17, 18, 82, 83, 84, 0, 0,
       0, 0, 0, 0, 0, 0, 85, 0, 0, 86,
       0, 0, 0, 0, 87, 88, 89, 0, 0, 0,
      90, 91, 0, 0, 0, 92, 0, 93, 94, -90,
     507, 0, 471, 39, 0, 0, 0, 0, 0, 0,
      77, 78, 79, 0, 80, 181, 182, 183, 184, 185,
     186, 187, 188, 189, 190, 191, 192, 193, 0, 0,
       0, 0, 81, 0, 17, 0, 82, 83, 84, 0,
       0, 0, 0, 644, 0, 0, 0, 85, 0, 0,
      86, 0, 0, 0, 0, 87, 88, 89, 0, 0,
       0, 90, 91, 0, 0, 508, 92, 509, 93, 94,
     510, 507, 0, 471, 39, 0, 0, 0, 0, 0,
       0, 77, 78, 79, 0, 80, 185, 186, 187, 188,
     189, 190, 191, 192, 193, 0, 0, 0, 0, 0,
       0, 0, 0, 81, 0, 17, 0, 82, 83, 84,
       0, 0, 0, 0, 0, 0, 0, 0, 85, 0,
       0, 86, 0, 0, 0, 0, 87, 88, 89, 0,
       0, 0, 90, 91, 0, 0, 508, 92, 509, 93,
      94, 510, 456, 0, 76, 0, 0, 0, 0, 0,
       0, 0, 77, 78, 79, 232, 80, 0, 5, 6,
       7, 8, 9, 10, 0, 0, 0, 0, 0, 0,
      11, 12, 13, 0, 81, 0, 17, 0, 82, 83,
      84, 0, 0, 0, 0, 0, 15, 0, 0, 85,
       0, 0, 86, 0, 0, 0, 0, 87, 88, 89,
       0, 0, 0, 90, 91, 166, 0, 76, 92, 0,
      93, 94, 457, 0, 0, 77, 78, 79, 0, 80,
       0, 0, 0, 0, 0, -104, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 81, 0, 17,
       0, 82, 83, 84, 0, 0, 0, 0, 0, 0,
       0, 0, 85, 0, 0, 86, 0, 0, 0, 0,
      87, 88, 89, 0, 0, 0, 90, 91, 0, 0,
       0, 92, 0, 93, 94, -90, 471, 472, 6, 7,
       8, 9, 10, 0, 77, 78, 79, 0, 80, 11,
      12, 13, 473, 0, 474, 475, 476, 477, 478, 479,
     480, 481, 482, 483, 484, 15, 81, 16, 17, 0,
      82, 83, 84, 485, 486, 487, 0, 0, 0, 0,
       0, 85, 0, 0, 86, 0, 0, 0, 0, 87,
      88, 89, 0, 0, 0, 90, 91, 0, 471, 39,
      92, 0, 93, 94, 270, 488, 77, 78, 79, 0,
      80, 0, 0, 0, 473, 0, 474, 475, 476, 477,
     478, 479, 480, 481, 482, 483, 484, 0, 81, 0,
      17, 0, 82, 83, 84, 485, 486, 487, 0, 0,
       0, 0, 0, 85, 0, 0, 86, 0, 0, 0,
       0, 87, 88, 89, 0, 0, 0, 90, 91, 0,
      76, 0, 92, 0, 93, 94, 270, 488, 77, 78,
      79, 0, 80, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      81, 0, 17, 0, 82, 83, 84, 0, 0, 0,
       0, 0, 0, 0, 0, 85, 0, 0, 86, 0,
       0, 0, 0, 87, 88, 89, 0, 0, 0, 90,
      91, 0, 76, 0, 92, 0, 93, 94, 0, 543,
      77, 78, 79, 0, 80, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 81, 0, 17, 0, 82, 83, 84, 0,
       0, 0, 0, 0, 0, 0, 0, 85, 0, 0,
      86, 0, 0, 0, 0, 87, 88, 89, 0, 0,
       0, 90, 91, 0, 0, 0, 92, 0, 93, 94,
     369, 76, 5, 6, 7, 8, 9, 10, 0, 77,
      78, 79, 0, 80, 11, 12, 13, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      15, 81, 16, 17, 0, 82, 83, 84, 0, 0,
       0, 0, 0, 0, 0, 0, 85, 0, 0, 86,
       0, 0, 0, 0, 87, 88, 89, 0, 0, 0,
      90, 91, 0, 0, 0, 92, 0, 93, 94, 76,
       5, 6, 7, 0, 9, 10, 0, 77, 78, 79,
       0, 80, 11, 12, 13, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 15, 81,
       0, 17, 0, 82, 83, 84, 0, 0, 0, 0,
       0, 0, 0, 0, 85, 0, 0, 86, 0, 0,
       0, 0, 87, 88, 89, 0, 0, 0, 90, 91,
      76, 0, 0, 92, 0, 93, 94, 0, 77, 78,
      79, 0, 80, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      81, 0, 17, 0, 82, 83, 84, 0, 0, 0,
       0, 0, 0, 0, 0, 85, 0, 0, 86, 0,
       0, 0, 0, 87, 88, 89, 0, 0, 0, 90,
      91, 76, 0, 0, 92, 0, 93, 94, 0, 77,
      78, 79, 0, 80, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 81, 0, 17, 0, 82, 83, 84, 0, 0,
       0, 0, 0, 0, 0, 0, 85, 0, 0, 86,
       0, 0, 0, 0, 87, 88, 89, 0, 0, 0,
      90, 91, 76, 0, 0, 173, 0, 93, 94, 0,
      77, 78, 79, 0, 80, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 81, 0, 17, 0, 82, 83, 84, 0,
       0, 0, 0, 0, 0, 0, 0, 85, 0, 0,
      86, 0, 0, 0, 0, 87, 88, 89, 0, 0,
       0, 90, 91, 387, 0, 0, 175, 0, 93, 94,
       0, 77, 78, 79, 0, 80, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 81, 0, 17, 0, 82, 83, 84,
       0, 0, 0, 0, 0, 0, 0, 0, 85, 0,
       0, 86, 0, 0, 0, 0, 87, 88, 89, 0,
       0, 0, 90, 91, 76, 0, 0, 92, 0, 93,
      94, 0, 77, 78, 79, 0, 80, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 81, 0, 17, 0, 82, 83,
      84, 0, 0, 617, 0, 0, 0, 0, 0, 85,
       0, 0, 86, 0, 0, 0, 0, 87, 88, 89,
       0, 0, 0, 90, 91, 0, 0, 0, 600, 0,
      93, 94, 177, 178, 179, 0, 180, 181, 182, 183,
     184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
     594, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 618, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 177,
     178, 179, 595, 180, 181, 182, 183, 184, 185, 186,
     187, 188, 189, 190, 191, 192, 193, 153, 0, 0,
       5, 6, 7, 0, 9, 10, 0, 0, 0, 0,
       0, 0, 11, 12, 13, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 15, 0,
       0, 17, 177, 178, 179, 0, 180, 181, 182, 183,
     184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 177, 178, 179, 658, 180, 181, 182, 183,
     184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
       0, 0, 16, 0, 0, 0, 0, 0, 0, 0,
       0, 177, 178, 179, 366, 180, 181, 182, 183, 184,
     185, 186, 187, 188, 189, 190, 191, 192, 193, 177,
     178, 179, 649, 180, 181, 182, 183, 184, 185, 186,
     187, 188, 189, 190, 191, 192, 193, 177, 178, 179,
       0, 180, 181, 182, 183, 184, 185, 186, 187, 188,
     189, 190, 191, 192, 193
};
static const short yycheck[] =
{
      11, 12, 13, 47, 48, 60, 135, 92, 58, 128,
      24, 24, 115, 227, 85, 2, 3, 2, 3, 168,
       2, 3, 248, 100, 101, 2, 3, 66, 254, 134,
     250, 89, 198, 113, 27, 48, 66, 95, 339, 235,
     391, 1, 45, 303, 31, 22, 31, 24, 92, 31,
     108, 119, 208, 204, 31, 304, 1, 129, 69, 72,
      72, 23, 24, 56, 457, 72, 3, 4, 427, 3,
      63, 36, 46, 491, 5, 52, 115, 0, 9, 92,
     124, 58, 25, 60, 203, 72, 34, 505, 173, 66,
     175, 351, 0, 3, 4, 427, 225, 31, 22, 73,
     113, 140, 536, 5, 115, 3, 4, 67, 10, 74,
     140, 125, 125, 3, 4, 3, 4, 510, 61, 67,
      68, 58, 382, 66, 34, 575, 71, 577, 52, 173,
     152, 175, 491, 34, 568, 179, 60, 496, 160, 116,
     151, 134, 119, 120, 157, 158, 158, 313, 58, 142,
     301, 158, 129, 31, 310, 199, 159, 67, 161, 491,
     173, 205, 175, 140, 496, 647, 616, 518, 618, 208,
     242, 158, 115, 71, 246, 3, 4, 245, 208, 613,
      67, 71, 3, 71, 333, 196, 197, 343, 34, 67,
      68, 46, 116, 155, 644, 157, 120, 140, 67, 448,
      46, 635, 684, 67, 411, 129, 34, 251, 415, 312,
     260, 46, 262, 34, 648, 273, 72, 71, 73, 75,
     234, 234, 672, 3, 237, 5, 660, 249, 71, 9,
      58, 427, 71, 58, 248, 248, 67, 58, 73, 67,
     254, 254, 67, 68, 72, 256, 67, 68, 241, 398,
      72, 407, 229, 3, 34, 5, 72, 558, 74, 9,
       3, 4, 411, 697, 308, 242, 415, 75, 245, 246,
     436, 310, 34, 312, 236, 237, 3, 4, 58, 72,
     310, 366, 75, 260, 34, 262, 341, 67, 68, 440,
      74, 34, 67, 68, 237, 73, 492, 75, 73, 519,
     496, 312, 74, 3, 343, 67, 68, 34, 58, 323,
     323, 391, 392, 343, 73, 58, 75, 67, 242, 67,
     439, 5, 246, 583, 67, 9, 445, 304, 5, 72,
       7, 58, 9, 73, 34, 357, 441, 73, 360, 75,
      67, 75, 364, 365, 31, 72, 323, 34, 73, 67,
      68, 3, 4, 366, 425, 73, 73, 44, 58, 67,
     322, 323, 339, 25, 341, 72, 517, 67, 407, 312,
      72, 421, 74, 524, 58, 72, 1, 407, 3, 4,
      67, 68, 75, 67, 68, 72, 48, 73, 75, 75,
      44, 67, 68, 12, 13, 417, 418, 73, 72, 61,
     343, 3, 4, 5, 66, 508, 75, 9, 67, 68,
      72, 637, 483, 639, 391, 392, 73, 341, 395, 430,
     442, 443, 444, 73, 446, 447, 71, 3, 4, 73,
      92, 72, 34, 56, 57, 58, 59, 60, 482, 67,
     427, 31, 427, 465, 421, 427, 457, 505, 441, 76,
     427, 113, 72, 115, 116, 44, 58, 74, 34, 67,
      68, 532, 581, 440, 407, 67, 68, 391, 392, 3,
       4, 448, 549, 550, 67, 68, 520, 691, 140, 608,
     494, 494, 58, 62, 63, 699, 65, 66, 67, 68,
     534, 67, 68, 537, 538, 157, 158, 508, 73, 510,
      34, 545, 67, 68, 491, 560, 491, 551, 558, 491,
      74, 173, 46, 175, 491, 74, 440, 494, 505, 73,
     505, 670, 671, 505, 58, 73, 570, 46, 505, 12,
      13, 493, 494, 67, 58, 59, 60, 3, 4, 5,
     517, 518, 46, 9, 73, 589, 208, 524, 633, 634,
      76, 76, 73, 3, 4, 599, 600, 73, 5, 73,
       7, 8, 9, 73, 575, 14, 577, 44, 34, 16,
      17, 18, 234, 76, 74, 237, 75, 73, 3, 76,
     242, 558, 1, 560, 34, 4, 5, 6, 72, 8,
       9, 72, 58, 517, 518, 73, 36, 16, 17, 18,
     524, 67, 46, 647, 46, 616, 67, 618, 58, 67,
      67, 46, 72, 32, 46, 31, 35, 67, 34, 72,
     633, 634, 67, 637, 637, 639, 639, 20, 44, 67,
      67, 675, 46, 644, 75, 74, 560, 73, 72, 301,
     684, 74, 72, 74, 67, 622, 72, 67, 310, 44,
     312, 21, 74, 72, 5, 74, 72, 8, 9, 75,
      75, 672, 74, 76, 162, 16, 17, 18, 73, 72,
      12, 73, 334, 72, 172, 75, 73, 73, 72, 177,
     178, 343, 180, 181, 182, 183, 184, 185, 186, 187,
     188, 189, 190, 191, 192, 193, 72, 72, 622, 73,
      72, 3, 4, 5, 366, 1, 73, 9, 4, 5,
       6, 7, 8, 9, 4, 5, 6, 7, 8, 9,
      16, 17, 18, 73, 44, 72, 16, 17, 18, 67,
      75, 72, 34, 395, 73, 31, 32, 72, 34, 72,
     402, 73, 32, 72, 34, 407, 72, 1, 44, 0,
       4, 5, 6, 7, 8, 9, 58, 255, 0, 395,
     313, 234, 16, 17, 18, 67, 106, 62, 342, 312,
     461, 67, 68, 258, 158, 71, 72, 31, 32, 75,
      34, 210, 72, 54, 55, 56, 57, 58, 59, 60,
      44, 262, 421, 1, 151, 491, 4, 5, 6, 7,
       8, 9, 256, 676, 428, 307, 496, 678, 16, 17,
      18, 395, 397, 67, 68, -1, -1, 71, 72, -1,
      -1, 75, 484, 31, 32, -1, 34, 1, -1, 3,
       4, 5, 6, 7, 8, 9, 44, -1, -1, -1,
      14, -1, 16, 17, 18, 51, 52, 53, 54, 55,
      56, 57, 58, 59, 60, 517, -1, -1, 32, -1,
      34, 359, 524, 71, 72, -1, -1, 75, 1, -1,
       3, 4, -1, -1, -1, 373, 374, -1, 11, 12,
      13, -1, 15, -1, -1, -1, 19, -1, 21, 22,
      23, 24, 25, 26, 27, 28, 29, 30, 31, 73,
      33, -1, 35, -1, 37, 38, 39, 40, 41, 42,
      -1, -1, 410, -1, -1, 48, -1, -1, 51, -1,
      -1, 419, -1, 56, 57, 58, -1, -1, -1, 62,
      63, -1, 430, -1, 67, -1, 69, 70, 71, 72,
      -1, 74, 4, 5, 6, 7, 8, 9, -1, -1,
      -1, -1, -1, -1, 16, 17, 18, -1, -1, 457,
     622, 1, -1, 461, 4, 5, 6, 7, 8, 9,
      32, 633, 634, -1, 14, -1, 16, 17, 18, -1,
     478, 50, 51, 52, 53, 54, 55, 56, 57, 58,
      59, 60, 32, -1, 34, 4, 5, 6, -1, 8,
       9, -1, -1, -1, -1, -1, -1, 16, 17, 18,
      72, 509, 510, -1, 1, -1, 3, 4, 5, 6,
       7, 8, 9, 32, 11, 12, 13, -1, 15, 16,
      17, 18, 19, 73, 21, 22, 23, 24, 25, 26,
      27, 28, 29, 30, 31, 32, 33, 34, 35, 58,
      37, 38, 39, 40, 41, 42, -1, -1, 67, 68,
      -1, 48, -1, -1, 51, -1, -1, -1, -1, 56,
      57, 58, -1, -1, -1, 62, 63, 575, -1, 577,
      67, -1, 69, 70, 71, 72, 5, 74, 7, 8,
       9, -1, 1, -1, 3, 4, 594, 16, 17, 18,
      -1, -1, 11, 12, 13, -1, 15, -1, -1, 4,
       5, 6, -1, 8, 9, 34, -1, -1, 616, 617,
     618, 16, 17, 18, 33, -1, 35, -1, 37, 38,
      39, -1, -1, -1, -1, -1, -1, 32, -1, 48,
      -1, -1, 51, -1, -1, -1, 644, 56, 57, 58,
      -1, -1, -1, 62, 63, -1, -1, 66, 67, 68,
      69, 70, 71, -1, -1, 74, 1, -1, 3, 4,
      -1, -1, -1, -1, 672, -1, 11, 12, 13, -1,
      15, -1, 4, 5, 6, 7, 8, 9, -1, -1,
      -1, -1, -1, -1, 16, 17, 18, -1, 33, -1,
      35, -1, 37, 38, 39, -1, -1, -1, -1, -1,
      32, -1, 34, 48, -1, -1, 51, -1, -1, -1,
      -1, 56, 57, 58, -1, -1, -1, 62, 63, -1,
      -1, 66, 67, 68, 69, 70, 71, 3, -1, 74,
      -1, -1, -1, -1, -1, 11, 12, 13, 1, 15,
      72, 4, 5, 6, 7, 8, 9, -1, -1, -1,
      -1, 14, -1, 16, 17, 18, -1, 33, -1, 35,
      -1, 37, 38, 39, -1, -1, -1, -1, -1, 32,
      -1, -1, 48, -1, -1, 51, -1, -1, -1, -1,
      56, 57, 58, -1, -1, -1, 62, 63, -1, 3,
      -1, 67, -1, 69, 70, -1, -1, 11, 12, 13,
      76, 15, 4, 5, 6, 7, 8, 9, 71, -1,
      -1, -1, 14, -1, 16, 17, 18, -1, -1, 33,
      -1, 35, -1, 37, 38, 39, -1, -1, -1, -1,
      32, -1, 34, -1, 48, -1, -1, 51, -1, -1,
      -1, -1, 56, 57, 58, -1, -1, -1, 62, 63,
      -1, 3, -1, 67, -1, 69, 70, -1, -1, 11,
      12, 13, 76, 15, 4, 5, 6, 7, 8, 9,
      -1, -1, -1, -1, -1, -1, 16, 17, 18, -1,
      -1, 33, -1, 35, -1, 37, 38, 39, -1, -1,
      -1, -1, 32, -1, 34, -1, 48, -1, -1, 51,
      -1, -1, -1, -1, 56, 57, 58, -1, -1, -1,
      62, 63, -1, 3, -1, 67, -1, 69, 70, -1,
      -1, 11, 12, 13, 76, 15, 4, 5, 6, 7,
       8, 9, -1, -1, -1, -1, 14, -1, 16, 17,
      18, -1, -1, 33, -1, 35, -1, 37, 38, 39,
      -1, 3, 4, 5, 32, 7, 8, 9, 48, -1,
      -1, 51, -1, -1, -1, -1, 56, 57, 58, -1,
      -1, -1, 62, 63, -1, 3, -1, 67, -1, 69,
      70, -1, -1, 11, 12, 13, 76, 15, 45, -1,
      47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
      57, 58, 59, 60, -1, 33, -1, 35, -1, 37,
      38, 39, 53, 54, 55, 56, 57, 58, 59, 60,
      48, -1, -1, 51, -1, -1, -1, -1, 56, 57,
      58, -1, -1, -1, 62, 63, -1, -1, -1, 67,
      -1, 69, 70, -1, -1, -1, -1, 1, 76, 3,
       4, 5, 6, 7, 8, 9, -1, 11, 12, 13,
      -1, 15, 16, 17, 18, 19, -1, 21, 22, 23,
      24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
      34, 35, -1, 37, 38, 39, 40, 41, 42, -1,
      -1, -1, -1, -1, 48, -1, -1, 51, -1, -1,
      -1, -1, 56, 57, 58, -1, -1, -1, 62, 63,
      -1, -1, -1, 67, -1, 69, 70, 71, 72, 1,
      -1, 3, 4, -1, -1, -1, -1, -1, -1, 11,
      12, 13, -1, 15, -1, -1, -1, 19, -1, 21,
      22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
      -1, 33, -1, 35, -1, 37, 38, 39, 40, 41,
      42, -1, -1, -1, -1, -1, 48, -1, -1, 51,
      -1, -1, -1, -1, 56, 57, 58, -1, -1, -1,
      62, 63, -1, -1, -1, 67, -1, 69, 70, 71,
      72, 1, -1, 3, 4, -1, -1, -1, -1, -1,
      -1, 11, 12, 13, -1, 15, -1, -1, -1, 19,
      -1, 21, 22, 23, 24, 25, 26, 27, 28, 29,
      30, 31, -1, 33, -1, 35, -1, 37, 38, 39,
      40, 41, 42, -1, -1, -1, -1, -1, 48, -1,
      -1, 51, -1, -1, -1, -1, 56, 57, 58, -1,
      -1, -1, 62, 63, -1, -1, -1, 67, -1, 69,
      70, 71, 72, 1, -1, 3, 4, 5, 6, 7,
       8, 9, -1, -1, -1, -1, -1, -1, 16, 17,
      18, 49, 50, 51, 52, 53, 54, 55, 56, 57,
      58, 59, 60, 31, 32, -1, 34, 35, -1, 1,
      -1, 3, 4, 5, 6, -1, 8, 9, -1, 11,
      12, 13, -1, 15, 16, 17, 18, -1, -1, -1,
      58, -1, -1, -1, -1, -1, -1, -1, -1, 67,
      32, 33, -1, 35, 72, 37, 38, 39, -1, -1,
      -1, -1, -1, -1, -1, -1, 48, -1, -1, 51,
      -1, -1, -1, -1, 56, 57, 58, -1, -1, -1,
      62, 63, -1, -1, -1, 67, -1, 69, 70, 71,
       1, -1, 3, 4, -1, -1, -1, -1, -1, -1,
      11, 12, 13, -1, 15, 48, 49, 50, 51, 52,
      53, 54, 55, 56, 57, 58, 59, 60, -1, -1,
      -1, -1, 33, -1, 35, -1, 37, 38, 39, -1,
      -1, -1, -1, 44, -1, -1, -1, 48, -1, -1,
      51, -1, -1, -1, -1, 56, 57, 58, -1, -1,
      -1, 62, 63, -1, -1, 66, 67, 68, 69, 70,
      71, 1, -1, 3, 4, -1, -1, -1, -1, -1,
      -1, 11, 12, 13, -1, 15, 52, 53, 54, 55,
      56, 57, 58, 59, 60, -1, -1, -1, -1, -1,
      -1, -1, -1, 33, -1, 35, -1, 37, 38, 39,
      -1, -1, -1, -1, -1, -1, -1, -1, 48, -1,
      -1, 51, -1, -1, -1, -1, 56, 57, 58, -1,
      -1, -1, 62, 63, -1, -1, 66, 67, 68, 69,
      70, 71, 1, -1, 3, -1, -1, -1, -1, -1,
      -1, -1, 11, 12, 13, 1, 15, -1, 4, 5,
       6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
      16, 17, 18, -1, 33, -1, 35, -1, 37, 38,
      39, -1, -1, -1, -1, -1, 32, -1, -1, 48,
      -1, -1, 51, -1, -1, -1, -1, 56, 57, 58,
      -1, -1, -1, 62, 63, 1, -1, 3, 67, -1,
      69, 70, 71, -1, -1, 11, 12, 13, -1, 15,
      -1, -1, -1, -1, -1, 71, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, 33, -1, 35,
      -1, 37, 38, 39, -1, -1, -1, -1, -1, -1,
      -1, -1, 48, -1, -1, 51, -1, -1, -1, -1,
      56, 57, 58, -1, -1, -1, 62, 63, -1, -1,
      -1, 67, -1, 69, 70, 71, 3, 4, 5, 6,
       7, 8, 9, -1, 11, 12, 13, -1, 15, 16,
      17, 18, 19, -1, 21, 22, 23, 24, 25, 26,
      27, 28, 29, 30, 31, 32, 33, 34, 35, -1,
      37, 38, 39, 40, 41, 42, -1, -1, -1, -1,
      -1, 48, -1, -1, 51, -1, -1, -1, -1, 56,
      57, 58, -1, -1, -1, 62, 63, -1, 3, 4,
      67, -1, 69, 70, 71, 72, 11, 12, 13, -1,
      15, -1, -1, -1, 19, -1, 21, 22, 23, 24,
      25, 26, 27, 28, 29, 30, 31, -1, 33, -1,
      35, -1, 37, 38, 39, 40, 41, 42, -1, -1,
      -1, -1, -1, 48, -1, -1, 51, -1, -1, -1,
      -1, 56, 57, 58, -1, -1, -1, 62, 63, -1,
       3, -1, 67, -1, 69, 70, 71, 72, 11, 12,
      13, -1, 15, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      33, -1, 35, -1, 37, 38, 39, -1, -1, -1,
      -1, -1, -1, -1, -1, 48, -1, -1, 51, -1,
      -1, -1, -1, 56, 57, 58, -1, -1, -1, 62,
      63, -1, 3, -1, 67, -1, 69, 70, -1, 72,
      11, 12, 13, -1, 15, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, 33, -1, 35, -1, 37, 38, 39, -1,
      -1, -1, -1, -1, -1, -1, -1, 48, -1, -1,
      51, -1, -1, -1, -1, 56, 57, 58, -1, -1,
      -1, 62, 63, -1, -1, -1, 67, -1, 69, 70,
      71, 3, 4, 5, 6, 7, 8, 9, -1, 11,
      12, 13, -1, 15, 16, 17, 18, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      32, 33, 34, 35, -1, 37, 38, 39, -1, -1,
      -1, -1, -1, -1, -1, -1, 48, -1, -1, 51,
      -1, -1, -1, -1, 56, 57, 58, -1, -1, -1,
      62, 63, -1, -1, -1, 67, -1, 69, 70, 3,
       4, 5, 6, -1, 8, 9, -1, 11, 12, 13,
      -1, 15, 16, 17, 18, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 32, 33,
      -1, 35, -1, 37, 38, 39, -1, -1, -1, -1,
      -1, -1, -1, -1, 48, -1, -1, 51, -1, -1,
      -1, -1, 56, 57, 58, -1, -1, -1, 62, 63,
       3, -1, -1, 67, -1, 69, 70, -1, 11, 12,
      13, -1, 15, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      33, -1, 35, -1, 37, 38, 39, -1, -1, -1,
      -1, -1, -1, -1, -1, 48, -1, -1, 51, -1,
      -1, -1, -1, 56, 57, 58, -1, -1, -1, 62,
      63, 3, -1, -1, 67, -1, 69, 70, -1, 11,
      12, 13, -1, 15, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, 33, -1, 35, -1, 37, 38, 39, -1, -1,
      -1, -1, -1, -1, -1, -1, 48, -1, -1, 51,
      -1, -1, -1, -1, 56, 57, 58, -1, -1, -1,
      62, 63, 3, -1, -1, 67, -1, 69, 70, -1,
      11, 12, 13, -1, 15, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, 33, -1, 35, -1, 37, 38, 39, -1,
      -1, -1, -1, -1, -1, -1, -1, 48, -1, -1,
      51, -1, -1, -1, -1, 56, 57, 58, -1, -1,
      -1, 62, 63, 3, -1, -1, 67, -1, 69, 70,
      -1, 11, 12, 13, -1, 15, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, 33, -1, 35, -1, 37, 38, 39,
      -1, -1, -1, -1, -1, -1, -1, -1, 48, -1,
      -1, 51, -1, -1, -1, -1, 56, 57, 58, -1,
      -1, -1, 62, 63, 3, -1, -1, 67, -1, 69,
      70, -1, 11, 12, 13, -1, 15, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, 33, -1, 35, -1, 37, 38,
      39, -1, -1, 14, -1, -1, -1, -1, -1, 48,
      -1, -1, 51, -1, -1, -1, -1, 56, 57, 58,
      -1, -1, -1, 62, 63, -1, -1, -1, 67, -1,
      69, 70, 43, 44, 45, -1, 47, 48, 49, 50,
      51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
      14, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, 76, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, 43,
      44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
      54, 55, 56, 57, 58, 59, 60, 1, -1, -1,
       4, 5, 6, -1, 8, 9, -1, -1, -1, -1,
      -1, -1, 16, 17, 18, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 32, -1,
      -1, 35, 43, 44, 45, -1, 47, 48, 49, 50,
      51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, 43, 44, 45, 76, 47, 48, 49, 50,
      51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
      -1, -1, 34, -1, -1, -1, -1, -1, -1, -1,
      -1, 43, 44, 45, 75, 47, 48, 49, 50, 51,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 43,
      44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
      54, 55, 56, 57, 58, 59, 60, 43, 44, 45,
      -1, 47, 48, 49, 50, 51, 52, 53, 54, 55,
      56, 57, 58, 59, 60
};
union yyalloc
{
  short yyss;
  struct yystype yyvs;
};
int yydebug;
int yyparse (void);
int yychar; struct yystype yylval; int yynerrs;
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
  struct yystype yyvsa[200];
  struct yystype *yyvs = yyvsa;
  register struct yystype *yyvsp;
  unsigned int yystacksize = 200;
  struct yystype yyval;
  int yylen;
  do { if (yydebug) fprintf (stderr, "Starting parse\n"); } while (0);
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
          (union yyalloc *) __builtin_alloca (((yystacksize) * (sizeof (short) + sizeof (struct yystype)) + (sizeof (union yyalloc) - 1)));
        if (! yyptr)
          goto yyoverflowlab;
        do { unsigned int yynewbytes; __builtin_memcpy (&yyptr->yyss, yyss, (yysize) * sizeof (*(yyss))); yyss = &yyptr->yyss; yynewbytes = yystacksize * sizeof (*yyss) + (sizeof (union yyalloc) - 1); yyptr += yynewbytes / sizeof (*yyptr); } while (0);
        do { unsigned int yynewbytes; __builtin_memcpy (&yyptr->yyvs, yyvs, (yysize) * sizeof (*(yyvs))); yyvs = &yyptr->yyvs; yynewbytes = yystacksize * sizeof (*yyvs) + (sizeof (union yyalloc) - 1); yyptr += yynewbytes / sizeof (*yyptr); } while (0);
        if (yyss1 != yyssa)
          do { ; } while (0);
      }
      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      do { if (yydebug) fprintf (stderr, "Stack size increased to %lu\n", (unsigned long int) yystacksize); } while (0);
      if (yyssp >= yyss + yystacksize - 1)
        goto yyabortlab;
    }
  do { if (yydebug) fprintf (stderr, "Entering state %d\n", yystate); } while (0);
  goto yybackup;
yybackup:
  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yydefault;
  if (yychar == -2)
    {
      do { if (yydebug) fprintf (stderr, "Reading a token: "); } while (0);
      yychar = yylex ();
    }
  if (yychar <= 0)
    {
      yychar1 = 0;
      yychar = 0;
      do { if (yydebug) fprintf (stderr, "Now at end of input.\n"); } while (0);
    }
  else
    {
      yychar1 = ((unsigned)(yychar) <= 308 ? yytranslate[yychar] : 229);
      if (yydebug)
        {
          fprintf (stderr, "Next token is %d (%s",
                     yychar, yytname[yychar1]);
          yyprint(stderr,yychar,yylval);
          fprintf (stderr, ")\n");
        }
    }
  yyn += yychar1;
  if (yyn < 0 || yyn > 3064 || yycheck[yyn] != yychar1)
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
  if (yyn == 705)
    goto yyacceptlab;
  do { if (yydebug) fprintf (stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]); } while (0);
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
  if (yydebug)
    {
      int yyi;
      fprintf (stderr, "Reducing via rule %d (line %d), ",
                 yyn, yyrline[yyn]);
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
        fprintf (stderr, "%s ", yytname[yyrhs[yyi]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
  switch (yyn) {
case 1:
{ if (pedantic)
                    pedwarn("ANSI C forbids an empty source file");
                  the_program = ((void *)0);
                ;
    break;}
case 2:{
                  the_program = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 304, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 304, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                ;
    break;}
case 3:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 4:
{ yyval.u.decl = yyvsp[0].u.decl; ;
    break;}
case 5:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 6:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 9:
{
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_asm_decl (parse_region, yyvsp[-4].u.itoken.location, new_asm_stmt(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-2].u.expr, ((void *)0), ((void *)0), ((void *)0), ((void *)0)))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 323, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 10:{ pedantic = yyvsp[-1].u.itoken.i;
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_extension_decl(parse_region, yyvsp[-1].u.itoken.location, yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 328, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 11:
{ if (pedantic)
                    error("ANSI C forbids data definition with no type or storage class");
                  else if (!flag_traditional)
                    warning("data definition has no type or storage class");
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-1].u.decl->loc, ((void *)0), ((void *)0), yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 338, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 12:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 341, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 13:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 344, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 14:{ pedwarn("empty declaration"); ;
    break;}
case 15:{ shadow_tag(yyvsp[-2].u.telement);
            yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-2].u.telement->loc, current_declspecs, prefix_attributes, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 350, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
            pop_declspec_stack(); ;
    break;}
case 16:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 17:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 18:{ if (pedantic)
                    pedwarn("ANSI C does not allow extra `;' outside of a function");
                  yyval.u.decl = ((void *)0); ;
    break;}
case 19:
{ if (!start_function(current_declspecs, yyvsp[0].u.declarator,
                                      prefix_attributes, 0))
                    { yyerror ("syntax error"); goto yyerrlab1; }; ;
    break;}
case 20:{ store_parm_decls(({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 366, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 366, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })); ;
    break;}
case 21:{ yyval.u.decl = finish_function(yyvsp[0].u.stmt);
                  pop_declspec_stack(); ;
    break;}
case 22:{ yyval.u.decl = new_error_decl(parse_region, last_location);
                  pop_declspec_stack(); ;
    break;}
case 23:{ if (!start_function(current_declspecs, yyvsp[0].u.declarator,
                                      prefix_attributes, 0))
                    { yyerror ("syntax error"); goto yyerrlab1; }; ;
    break;}
case 24:{ store_parm_decls(yyvsp[0].u.decl); ;
    break;}
case 25:{ yyval.u.decl = finish_function(yyvsp[0].u.stmt);
                  pop_declspec_stack(); ;
    break;}
case 26:{ yyval.u.decl = new_error_decl(parse_region, last_location);
                  pop_declspec_stack(); ;
    break;}
case 27:{ if (!start_function(((void *)0), yyvsp[0].u.declarator,
                                      prefix_attributes, 0))
                    { yyerror ("syntax error"); goto yyerrlab1; }; ;
    break;}
case 28:{ store_parm_decls(yyvsp[0].u.decl); ;
    break;}
case 29:{ yyval.u.decl = finish_function(yyvsp[0].u.stmt);
                  pop_declspec_stack(); ;
    break;}
case 30:{ yyval.u.decl = new_error_decl(parse_region, last_location);
                  pop_declspec_stack(); ;
    break;}
case 33:
{ yyval.u.id_label = new_id_label(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id); ;
    break;}
case 34:
{ yyval.u.word = new_word(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id); ;
    break;}
case 35:
{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_address_of; ;
    break;}
case 36:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_unary_minus; ;
    break;}
case 37:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_unary_plus; ;
    break;}
case 38:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_preincrement; ;
    break;}
case 39:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_predecrement; ;
    break;}
case 40:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_bitnot; ;
    break;}
case 41:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_not; ;
    break;}
case 42:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_realpart; ;
    break;}
case 43:{ yyval.u.itoken = yyvsp[0].u.itoken; yyval.u.itoken.i = kind_imagpart; ;
    break;}
case 44:
{ if (yyvsp[0].u.expr->next)
                    yyval.u.expr = make_comma(yyvsp[0].u.expr->loc, yyvsp[0].u.expr);
                  else
                    yyval.u.expr = yyvsp[0].u.expr; ;
    break;}
case 45:
{ yyval.u.expr = ((void *)0); ;
    break;}
case 47:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 447, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 447, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 48:
{ yyval.u.expr = yyvsp[0].u.expr; ;
    break;}
case 49:{ yyval.u.expr = expression_chain(yyvsp[0].u.expr, yyvsp[-2].u.expr); ;
    break;}
case 51:
{ yyval.u.expr = make_dereference(yyvsp[-1].u.itoken.location, yyvsp[0].u.expr); ;
    break;}
case 52:
{ yyval.u.expr = make_extension_expr(yyvsp[-1].u.itoken.location, yyvsp[0].u.expr);
                  pedantic = yyvsp[-1].u.itoken.i; ;
    break;}
case 53:{ yyval.u.expr = make_unary(yyvsp[-1].u.itoken.location, yyvsp[-1].u.itoken.i, yyvsp[0].u.expr);
                ;
    break;}
case 54:
{
                  yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(make_label_address(yyvsp[-1].u.itoken.location, yyvsp[0].u.id_label)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 474, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); });
                  use_label(yyvsp[0].u.id_label);
                ;
    break;}
case 55:{
                  yyval.u.expr = make_sizeof_expr(yyvsp[-1].u.itoken.location, yyvsp[0].u.expr);
                  unevaluated_expression--; ;
    break;}
case 56:{ yyval.u.expr = make_sizeof_type(yyvsp[-3].u.itoken.location, yyvsp[-1].u.type);
                  unevaluated_expression--; ;
    break;}
case 57:{ yyval.u.expr = make_alignof_expr(yyvsp[-1].u.itoken.location, yyvsp[0].u.expr);
                  unevaluated_expression--; ;
    break;}
case 58:{ yyval.u.expr = make_alignof_type(yyvsp[-3].u.itoken.location, yyvsp[-1].u.type);
                  unevaluated_expression--; ;
    break;}
case 59:
{ unevaluated_expression++; yyval.u.itoken = yyvsp[0].u.itoken; ;
    break;}
case 60:
{ unevaluated_expression++; yyval.u.itoken = yyvsp[0].u.itoken; ;
    break;}
case 62:
{ yyval.u.expr = make_cast(yyvsp[-3].u.itoken.location, yyvsp[-2].u.type, yyvsp[0].u.expr); ;
    break;}
case 63:{
                ;
    break;}
case 64:{
                  location initlist_location;
                  if (yyvsp[-1].u.expr)
                    initlist_location = yyvsp[-1].u.expr->loc;
                  else
                    initlist_location = yyvsp[-6].u.itoken.location;
                  yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_cast_list(parse_region, yyvsp[-6].u.itoken.location, yyvsp[-5].u.type, ({ast_generic tEmPcast = (ast_generic)(new_init_list(parse_region, initlist_location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 526, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 526, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); });
                  yyval.u.expr->type = yyvsp[-5].u.type->type;
                  yyval.u.expr->lvalue = 1;
                  if (type_array(yyval.u.expr->type))
                    yyval.u.expr->lvalue = 1;
                  if (pedantic)
                    pedwarn("ANSI C forbids constructor expressions");
                ;
    break;}
case 66:
{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_plus, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 67:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_minus, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 68:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_times, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 69:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_divide, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 70:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_modulo, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 71:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_lshift, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 72:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_rshift, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 73:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, yyvsp[-1].u.itoken.i, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 74:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, yyvsp[-1].u.itoken.i, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 75:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_bitand, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 76:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_bitor, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 77:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_bitxor, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 78:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_andand, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 79:{ yyval.u.expr = make_binary(yyvsp[-1].u.itoken.location, kind_oror, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 80:{ yyval.u.expr = make_conditional(yyvsp[-3].u.itoken.location, yyvsp[-4].u.expr, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 81:{ if (pedantic)
                    pedwarn("ANSI C forbids omitting the middle term of a ?: expression");
                ;
    break;}
case 82:{ yyval.u.expr = make_conditional(yyvsp[-3].u.itoken.location, yyvsp[-4].u.expr, ((void *)0), yyvsp[0].u.expr); ;
    break;}
case 83:{ yyval.u.expr = make_assign(yyvsp[-1].u.itoken.location, kind_assign, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 84:{ yyval.u.expr = make_assign(yyvsp[-1].u.itoken.location, yyvsp[-1].u.itoken.i, yyvsp[-2].u.expr, yyvsp[0].u.expr); ;
    break;}
case 85:
{
                  if (yychar == -2)
                    yychar = yylex ();
                  yyval.u.expr = make_identifier(yyvsp[0].idtoken.location, yyvsp[0].idtoken.id, yychar == '(');
                ;
    break;}
case 86:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.constant); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 615, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 87:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.string); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 616, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 88:{ yyval.u.expr = yyvsp[-1].u.expr; ;
    break;}
case 89:{ yyval.u.expr = make_error_expr(last_location); ;
    break;}
case 90:{ if (current_function_decl == 0)
                    {
                      error("braced-group within expression allowed only inside a function");
                      goto yyerrlab1;
                    }
                    push_label_level();
                ;
    break;}
case 91:{
                  pop_label_level();
                  if (pedantic)
                    pedwarn("ANSI C forbids braced-groups within expressions");
                  yyval.u.expr = make_compound_expr(yyvsp[-3].u.itoken.location, yyvsp[-1].u.stmt);
                ;
    break;}
case 92:{ yyval.u.expr = make_function_call(yyvsp[-2].u.itoken.location, yyvsp[-3].u.expr, yyvsp[-1].u.expr); ;
    break;}
case 93:{ yyval.u.expr = make_va_arg(yyvsp[-5].u.itoken.location, yyvsp[-3].u.expr, yyvsp[-1].u.type); ;
    break;}
case 94:{ yyval.u.expr = make_array_ref(yyvsp[-2].u.itoken.location, yyvsp[-3].u.expr, yyvsp[-1].u.expr); ;
    break;}
case 95:{ yyval.u.expr = make_field_ref(yyvsp[-1].u.itoken.location, yyvsp[-2].u.expr, yyvsp[0].idtoken.id, yyvsp[0].idtoken.location); ;
    break;}
case 96:{ yyval.u.expr = make_field_ref(yyvsp[-1].u.itoken.location, make_dereference(yyvsp[-1].u.itoken.location, yyvsp[-2].u.expr),
                                      yyvsp[0].idtoken.id, yyvsp[0].idtoken.location); ;
    break;}
case 97:{ yyval.u.expr = make_postincrement(yyvsp[0].u.itoken.location, yyvsp[-1].u.expr); ;
    break;}
case 98:{ yyval.u.expr = make_postdecrement(yyvsp[0].u.itoken.location, yyvsp[-1].u.expr); ;
    break;}
case 99:
{ yyval.u.string = make_string(yyvsp[0].u.expr->loc, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 655, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 655, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); })); ;
    break;}
case 100:
{ yyval.u.expr = yyvsp[0].u.expr; ;
    break;}
case 101:{ yyval.u.expr = expression_chain(yyvsp[0].u.expr, yyvsp[-1].u.expr); ;
    break;}
case 102:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.expr); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 665, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 103:{ yyval.u.expr = make_identifier(yyvsp[0].idtoken.location, yyvsp[0].idtoken.id, 0);
          ;
    break;}
case 104:
{ yyval.u.decl = ((void *)0); ;
    break;}
case 106:
{ if (pedantic)
                    pedwarn("ANSI C does not permit use of `varargs.h'");
                  yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-1].u.decl);
                ;
    break;}
case 107:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_ellipsis_decl(parse_region, yyvsp[0].u.itoken.location, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 685, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 108:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_ellipsis_decl(parse_region, yyvsp[0].u.itoken.location, yyvsp[-1].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 687, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 109:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_ellipsis_decl(parse_region, yyvsp[-1].u.itoken.location, yyvsp[0].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 689, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 110:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_ellipsis_decl(parse_region, yyvsp[-1].u.itoken.location, type_element_chain(yyvsp[-2].u.telement, yyvsp[0].u.telement))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 691, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 112:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 113:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-1].u.decl); ;
    break;}
case 114:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 115:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 712, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 116:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 715, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 117:{ shadow_tag_warned(yyvsp[-2].u.telement, 1);
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-2].u.telement->loc, current_declspecs, prefix_attributes, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 719, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack();
                  pedwarn("empty declaration"); ;
    break;}
case 118:{ pedwarn("empty declaration");
                  yyval.u.decl = ((void *)0); ;
    break;}
case 120:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 121:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-1].u.decl); ;
    break;}
case 122:
{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 123:
{
                  push_declspec_stack();
                  pending_xref_error();
                  split_type_elements(yyvsp[0].u.telement,
                                      &current_declspecs, &prefix_attributes);
                ;
    break;}
case 124:
{ prefix_attributes = attribute_chain(prefix_attributes,
                                                      yyvsp[0].u.attribute);
                                                           ;
    break;}
case 125:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 764, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 126:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-3].u.telement->loc, current_declspecs, prefix_attributes, yyvsp[-1].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 767, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 127:{ yyval.u.decl = yyvsp[0].u.decl;
                  pop_declspec_stack(); ;
    break;}
case 128:{ yyval.u.decl = yyvsp[0].u.decl;
                  pop_declspec_stack(); ;
    break;}
case 129:{ shadow_tag(yyvsp[-2].u.telement);
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-2].u.telement->loc, current_declspecs, prefix_attributes, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 777, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 130:{ pedwarn("empty declaration"); ;
    break;}
case 131:{ pedantic = yyvsp[-1].u.itoken.i;
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_extension_decl(parse_region, yyvsp[-1].u.itoken.location, yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 783, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 132:
{ yyval.u.telement = yyvsp[-1].u.telement; yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 792, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 133:{ yyval.u.telement = type_element_chain(yyvsp[-2].u.telement, yyvsp[-1].u.telement); yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 794, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 134:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 135:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 136:{ if (extra_warnings)
                    warning("`%s' is not at beginning of declaration",
                            rid_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "c-parse.y", 804, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })));
                  yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 137:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.attribute); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 807, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); })); ;
    break;}
case 138:
{ yyval.u.telement = yyvsp[-1].u.telement; yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 812, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 139:{ yyval.u.telement = type_element_chain(yyvsp[-2].u.telement, yyvsp[-1].u.telement); yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 814, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 140:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 141:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 142:{ if (extra_warnings)
                    warning("`%s' is not at beginning of declaration",
                            rid_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "c-parse.y", 825, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })));
                  yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 144:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.attribute); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 836, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 145:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 146:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.attribute); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 840, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); })); ;
    break;}
case 149:
{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 150:{ if (extra_warnings )
                    warning("`%s' is not at beginning of declaration",
                            rid_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "c-parse.y", 851, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })));
                  yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 151:
{ yyval.u.telement = yyvsp[-1].u.telement; yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 862, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 152:{ yyval.u.telement = type_element_chain(yyvsp[-2].u.telement, yyvsp[-1].u.telement); yyvsp[-1].u.telement->next = ({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 864, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }); ;
    break;}
case 153:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 154:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 157:
{
                  yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_typename(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 882, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 158:{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_type_variable(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 884, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 159:{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_typeof_expr(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 886, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 160:{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_typeof_type(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-1].u.type)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 888, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 164:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 899, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 899, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 165:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 903, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 903, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 167:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 168:
{ yyval.u.decl = yyvsp[0].u.decl; ;
    break;}
case 169:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 170:
{ yyval.u.asm_stmt = ((void *)0); ;
    break;}
case 171:{ yyval.u.asm_stmt = new_asm_stmt(parse_region, yyvsp[-3].u.itoken.location, ({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.string); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 920, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }),
                                    ((void *)0), ((void *)0), ((void *)0), ((void *)0)); ;
    break;}
case 172:
{ yyval.u.decl = start_decl(yyvsp[-3].u.declarator, yyvsp[-2].u.asm_stmt, current_declspecs, 1,
                                        yyvsp[-1].u.attribute, prefix_attributes); ;
    break;}
case 173:
{ yyval.u.decl = finish_decl(yyvsp[-1].u.decl, yyvsp[0].u.expr); ;
    break;}
case 174:{ declaration d = start_decl(yyvsp[-2].u.declarator, yyvsp[-1].u.asm_stmt, current_declspecs, 0,
                                             yyvsp[0].u.attribute, prefix_attributes);
                  yyval.u.decl = finish_decl(d, ((void *)0)); ;
    break;}
case 175:
{ yyval.u.decl = start_decl(yyvsp[-3].u.declarator, yyvsp[-2].u.asm_stmt, current_declspecs, 1,
                                         yyvsp[-1].u.attribute, prefix_attributes); ;
    break;}
case 176:
{ yyval.u.decl = finish_decl(yyvsp[-1].u.decl, yyvsp[0].u.expr); ;
    break;}
case 177:{ declaration d = start_decl(yyvsp[-2].u.declarator, yyvsp[-1].u.asm_stmt, current_declspecs, 0,
                                             yyvsp[0].u.attribute, prefix_attributes);
                  yyval.u.decl = finish_decl(d, ((void *)0)); ;
    break;}
case 178:
{ yyval.u.attribute = ((void *)0); ;
    break;}
case 179:{ yyval.u.attribute = yyvsp[0].u.attribute; ;
    break;}
case 180:
{ yyval.u.attribute = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.attribute)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 957, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_attribute && (tEmPcast)->kind <= postkind_attribute)", "c-parse.y", 957, __PRETTY_FUNCTION__), 0))); (attribute)(tEmPcast); }); ;
    break;}
case 181:
{ yyval.u.attribute = yyvsp[0].u.attribute; ;
    break;}
case 182:{ yyval.u.attribute = attribute_chain(yyvsp[0].u.attribute, yyvsp[-1].u.attribute); ;
    break;}
case 183:
{ yyval.u.attribute = yyvsp[-2].u.attribute; ;
    break;}
case 184:
{ yyval.u.attribute = yyvsp[0].u.attribute; ;
    break;}
case 185:{ yyval.u.attribute = attribute_chain(yyvsp[0].u.attribute, yyvsp[-2].u.attribute); ;
    break;}
case 186:
{ yyval.u.attribute = ((void *)0); ;
    break;}
case 187:{ yyval.u.attribute = new_attribute(parse_region, yyvsp[0].u.word->loc, yyvsp[0].u.word, ((void *)0), ((void *)0)); ;
    break;}
case 188:{ yyval.u.attribute = new_attribute
                    (parse_region, yyvsp[-3].u.word->loc, yyvsp[-3].u.word, new_word(parse_region, yyvsp[-1].idtoken.location, yyvsp[-1].idtoken.id), ((void *)0)); ;
    break;}
case 189:{ yyval.u.attribute = new_attribute
                    (parse_region, yyvsp[-4].u.itoken.location, yyvsp[-5].u.word, new_word(parse_region, yyvsp[-3].idtoken.location, yyvsp[-3].idtoken.id), yyvsp[-1].u.expr);
                ;
    break;}
case 190:{ yyval.u.attribute = new_attribute(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-3].u.word, ((void *)0), yyvsp[-1].u.expr); ;
    break;}
case 192:
{ yyval.u.word = new_word(parse_region, yyvsp[0].u.telement->loc, str2cstring(parse_region, rid_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "c-parse.y", 1000, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })))); ;
    break;}
case 193:
{ yyval.u.word = new_word(parse_region, yyvsp[0].u.telement->loc, str2cstring(parse_region, rid_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_rid && (tEmPcast)->kind <= postkind_rid)", "c-parse.y", 1001, __PRETTY_FUNCTION__), 0))); (rid)(tEmPcast); })))); ;
    break;}
case 194:
{ yyval.u.word = new_word(parse_region, yyvsp[0].u.telement->loc, str2cstring(parse_region, qualifier_name(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.telement); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_qualifier && (tEmPcast)->kind <= postkind_qualifier)", "c-parse.y", 1002, __PRETTY_FUNCTION__), 0))); (qualifier)(tEmPcast); })->id))); ;
    break;}
case 196:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_list(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1011, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); });
                  yyval.u.expr->type = error_type; ;
    break;}
case 197:{ yyval.u.expr = make_error_expr(last_location); ;
    break;}
case 198:
{ if (pedantic)
                    pedwarn("ANSI C forbids empty initializer braces");
                  yyval.u.expr = ((void *)0); ;
    break;}
case 199:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.expr); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1023, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1023, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1023, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 201:
{ yyval.u.expr = expression_chain(yyvsp[0].u.expr, yyvsp[-2].u.expr); ;
    break;}
case 202:
{ yyval.u.expr = yyvsp[0].u.expr; ;
    break;}
case 203:{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_list(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1036, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 204:
{ yyval.u.expr = make_error_expr(last_location); ;
    break;}
case 205:
{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_index(parse_region, yyvsp[-6].u.itoken.location, yyvsp[-5].u.expr, yyvsp[-3].u.expr, yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1042, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 206:{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_index(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-3].u.expr, ((void *)0), yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1044, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 207:{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_index(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-2].u.expr, ((void *)0), yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1046, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 208:{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_field(parse_region, yyvsp[-2].u.word->loc, yyvsp[-2].u.word, yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1048, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 209:{ yyval.u.expr = ({ast_generic tEmPcast = (ast_generic)(new_init_field(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-2].u.word, yyvsp[0].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1050, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }); ;
    break;}
case 210:
{ if (!start_function(current_declspecs, yyvsp[0].u.declarator,
                                      prefix_attributes, 1))
                    {
                      { yyerror ("syntax error"); goto yyerrlab1; };
                    }
                  ;
    break;}
case 211:{ store_parm_decls(({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1062, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1062, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })); ;
    break;}
case 212:
{ yyval.u.decl = finish_function(yyvsp[0].u.stmt); ;
    break;}
case 213:
{ if (!start_function(current_declspecs, yyvsp[0].u.declarator,
                                      prefix_attributes, 1))
                    {
                      { yyerror ("syntax error"); goto yyerrlab1; };
                    }
                ;
    break;}
case 214:{ store_parm_decls(({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1082, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1082, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })); ;
    break;}
case 215:
{ yyval.u.decl = finish_function(yyvsp[0].u.stmt); ;
    break;}
case 218:
{ yyval.u.declarator = yyvsp[-1].u.declarator; ;
    break;}
case 219:{ yyval.u.declarator = make_function_declarator(yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.decl, yyvsp[0].u.telement); ;
    break;}
case 220:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1109, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 221:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-1].u.itoken.location, yyvsp[-2].u.declarator, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1111, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 222:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_pointer_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[0].u.declarator, yyvsp[-1].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1113, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 223:
{ yyval.u.declarator = yyvsp[0].u.declarator; ;
    break;}
case 224:
{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_identifier_declarator(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1121, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 225:
{ yyval.u.declarator = make_function_declarator(yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.decl, yyvsp[0].u.telement); ;
    break;}
case 226:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1133, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 227:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-1].u.itoken.location, yyvsp[-2].u.declarator, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1135, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 228:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_pointer_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[0].u.declarator, yyvsp[-1].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1137, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 229:
{ yyval.u.declarator = yyvsp[0].u.declarator; ;
    break;}
case 230:
{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_identifier_declarator(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1145, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 231:
{ yyval.u.declarator = make_function_declarator(yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.decl, yyvsp[0].u.telement); ;
    break;}
case 232:{ yyval.u.declarator = yyvsp[-1].u.declarator; ;
    break;}
case 233:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_pointer_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[0].u.declarator, yyvsp[-1].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1157, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 234:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1159, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 235:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-1].u.itoken.location, yyvsp[-2].u.declarator, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1161, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 236:
{ yyval.u.declarator = yyvsp[0].u.declarator; ;
    break;}
case 237:
{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_identifier_declarator(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1169, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 238:
{ yyval.u.word = new_word(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id); ;
    break;}
case 239:
{ yyval.u.telement = start_struct(yyvsp[-2].u.itoken.location, kind_struct_ref, yyvsp[-1].u.word);
                ;
    break;}
case 240:{ yyval.u.telement = finish_struct(yyvsp[-3].u.telement, yyvsp[-2].u.decl, yyvsp[0].u.attribute); ;
    break;}
case 241:{ yyval.u.telement = finish_struct(start_struct(yyvsp[-4].u.itoken.location, kind_struct_ref, ((void *)0)),
                                     yyvsp[-2].u.decl, yyvsp[0].u.attribute);
                ;
    break;}
case 242:{ yyval.u.telement = xref_tag(yyvsp[-1].u.itoken.location, kind_struct_ref, yyvsp[0].u.word); ;
    break;}
case 243:{ yyval.u.telement = start_struct (yyvsp[-2].u.itoken.location, kind_union_ref, yyvsp[-1].u.word); ;
    break;}
case 244:{ yyval.u.telement = finish_struct(yyvsp[-3].u.telement, yyvsp[-2].u.decl, yyvsp[0].u.attribute); ;
    break;}
case 245:{ yyval.u.telement = finish_struct(start_struct(yyvsp[-4].u.itoken.location, kind_union_ref, ((void *)0)),
                                     yyvsp[-2].u.decl, yyvsp[0].u.attribute);
                ;
    break;}
case 246:{ yyval.u.telement = xref_tag(yyvsp[-1].u.itoken.location, kind_union_ref, yyvsp[0].u.word); ;
    break;}
case 247:{ yyval.u.telement = start_enum(yyvsp[-2].u.itoken.location, yyvsp[-1].u.word); ;
    break;}
case 248:{ yyval.u.telement = finish_enum(yyvsp[-4].u.telement, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[-3].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1202, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1202, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), yyvsp[0].u.attribute); ;
    break;}
case 249:{ yyval.u.telement = start_enum(yyvsp[-1].u.itoken.location, ((void *)0)); ;
    break;}
case 250:{ yyval.u.telement = finish_enum(yyvsp[-4].u.telement, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[-3].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1206, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1206, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), yyvsp[0].u.attribute); ;
    break;}
case 251:{ yyval.u.telement = xref_tag(yyvsp[-1].u.itoken.location, kind_enum_ref, yyvsp[0].u.word); ;
    break;}
case 255:
{ if (pedantic) pedwarn("comma at end of enumerator list"); ;
    break;}
case 256:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1224, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1224, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 257:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((declaration_chain(yyvsp[0].u.decl, yyvsp[-1].u.decl))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1226, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1226, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pedwarn("no semicolon at end of struct or union"); ;
    break;}
case 258:
{ yyval.u.decl = ((void *)0); ;
    break;}
case 259:{ yyval.u.decl = declaration_chain(yyvsp[-1].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 260:{ if (pedantic)
                    pedwarn("extra semicolon in struct or union specified");
                   yyval.u.decl = yyvsp[-1].u.decl; ;
    break;}
case 261:
{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-2].u.telement->loc, current_declspecs, prefix_attributes, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1251, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1251, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1251, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 262:{ if (pedantic)
                    pedwarn("ANSI C forbids member declarations with no members");
                  shadow_tag(yyvsp[-1].u.telement);
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-1].u.telement->loc, current_declspecs, prefix_attributes, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1257, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 263:{ yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-2].u.telement->loc, current_declspecs, prefix_attributes, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1260, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1260, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1260, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 264:{ if (pedantic)
                    pedwarn("ANSI C forbids member declarations with no members");
                  shadow_tag(yyvsp[-1].u.telement);
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_data_decl(parse_region, yyvsp[-1].u.telement->loc, current_declspecs, prefix_attributes, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1266, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); });
                  pop_declspec_stack(); ;
    break;}
case 265:{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 266:{ pedantic = yyvsp[-1].u.itoken.i;
                  yyval.u.decl = ({ast_generic tEmPcast = (ast_generic)(new_extension_decl(parse_region, yyvsp[-1].u.itoken.location, yyvsp[0].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1272, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }); ;
    break;}
case 268:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 269:
{ yyval.u.decl = make_field(yyvsp[-1].u.declarator, ((void *)0), current_declspecs,
                                  yyvsp[0].u.attribute, prefix_attributes); ;
    break;}
case 270:{ yyval.u.decl = make_field(yyvsp[-3].u.declarator, yyvsp[-1].u.expr, current_declspecs,
                                  yyvsp[0].u.attribute, prefix_attributes); ;
    break;}
case 271:{ yyval.u.decl = make_field(((void *)0), yyvsp[-1].u.expr, current_declspecs,
                                  yyvsp[0].u.attribute, prefix_attributes); ;
    break;}
case 273:
{ yyval.u.decl = declaration_chain(yyvsp[0].u.decl, yyvsp[-2].u.decl); ;
    break;}
case 274:{ yyval.u.decl = ((void *)0); ;
    break;}
case 275:
{ yyval.u.decl = make_enumerator(yyvsp[0].idtoken.location, yyvsp[0].idtoken.id, ((void *)0)); ;
    break;}
case 276:{ yyval.u.decl = make_enumerator(yyvsp[-2].idtoken.location, yyvsp[-2].idtoken.id, yyvsp[0].u.expr); ;
    break;}
case 277:
{ yyval.u.type = make_type(yyvsp[-1].u.telement, yyvsp[0].u.declarator); ;
    break;}
case 278:{ yyval.u.type = make_type(yyvsp[-1].u.telement, yyvsp[0].u.declarator); ;
    break;}
case 279:
{ yyval.u.declarator = ((void *)0); ;
    break;}
case 282:
{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 283:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 284:{ yyval.u.telement = type_element_chain(yyvsp[-1].u.telement, yyvsp[0].u.telement); ;
    break;}
case 285:
{ yyval.u.declarator = yyvsp[-1].u.declarator; ;
    break;}
case 286:
{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_pointer_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[0].u.declarator, yyvsp[-1].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1340, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 287:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_pointer_declarator(parse_region, yyvsp[-1].u.itoken.location, ((void *)0), yyvsp[0].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1342, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 288:{ yyval.u.declarator = make_function_declarator(yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.decl, yyvsp[0].u.telement); ;
    break;}
case 289:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-3].u.declarator, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1346, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 290:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-1].u.itoken.location, yyvsp[-2].u.declarator, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1348, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 291:{ yyval.u.declarator = make_function_declarator(yyvsp[-2].u.itoken.location, ((void *)0), yyvsp[-1].u.decl, yyvsp[0].u.telement); ;
    break;}
case 292:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-2].u.itoken.location, ((void *)0), yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1352, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 293:{ yyval.u.declarator = ({ast_generic tEmPcast = (ast_generic)(new_array_declarator(parse_region, yyvsp[-1].u.itoken.location, ((void *)0), ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declarator && (tEmPcast)->kind <= postkind_declarator)", "c-parse.y", 1354, __PRETTY_FUNCTION__), 0))); (declarator)(tEmPcast); }); ;
    break;}
case 294:
{
                  if (pedantic && yyvsp[0].u.istmt.i)
                    pedwarn("ANSI C forbids label at end of compound statement");
                  if (yyvsp[0].u.istmt.i)
                    {
                      statement last_label = ({ast_generic tEmPcast = (ast_generic)(last_node(({ast_generic tEmPcast = (ast_generic)(yyvsp[0].u.istmt.stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1371, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1371, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                      chain_with_labels(last_label, ({ast_generic tEmPcast = (ast_generic)(new_empty_stmt(parse_region, last_label->loc)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1373, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }));
                    }
                  yyval.u.stmt = yyvsp[0].u.istmt.stmt;
                ;
    break;}
case 296:
{ yyval.u.istmt.i = yyvsp[0].u.istmt.i; yyval.u.istmt.stmt = chain_with_labels(yyvsp[-1].u.istmt.stmt, yyvsp[0].u.istmt.stmt); ;
    break;}
case 297:{ yyval.u.istmt.i = 0; yyval.u.istmt.stmt = new_error_stmt(parse_region, last_location); ;
    break;}
case 298:
{ yyval.u.stmt = ((void *)0); ;
    break;}
case 301:
{ pushlevel(0); ;
    break;}
case 302:
{ yyval.u.id_label = ((void *)0); ;
    break;}
case 303:{ if (pedantic)
                    pedwarn("ANSI C forbids label declarations");
                  yyval.u.id_label = ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[0].u.id_label)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1406, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_id_label && (tEmPcast)->kind <= postkind_id_label)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_id_label && (tEmPcast)->kind <= postkind_id_label)", "c-parse.y", 1406, __PRETTY_FUNCTION__), 0))); (id_label)(tEmPcast); }); ;
    break;}
case 305:
{ yyval.u.id_label = id_label_chain(yyvsp[0].u.id_label, yyvsp[-1].u.id_label); ;
    break;}
case 306:
{ yyval.u.id_label = yyvsp[-1].u.id_label; ;
    break;}
case 308:
{ yyval.u.stmt = yyvsp[0].u.stmt; ;
    break;}
case 309:
{ yyval.u.itoken = yyvsp[0].u.itoken; compstmt_count++; ;
    break;}
case 310:
{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_compound_stmt(parse_region, yyvsp[-2].u.itoken.location, ((void *)0), ((void *)0), ((void *)0), poplevel())); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1430, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 311:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_compound_stmt(parse_region, yyvsp[-5].u.itoken.location, yyvsp[-3].u.id_label, ({ast_generic tEmPcast = (ast_generic)(ast_reverse(({ast_generic tEmPcast = (ast_generic)((yyvsp[-2].u.decl)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_node && (tEmPcast)->kind <= postkind_node)", "c-parse.y", 1432, __PRETTY_FUNCTION__), 0))); (node)(tEmPcast); }))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "c-parse.y", 1432, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), yyvsp[-1].u.stmt, poplevel())); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1432, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 312:{ poplevel();
                  yyval.u.stmt = new_error_stmt(parse_region, last_location); ;
    break;}
case 313:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_compound_stmt(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-2].u.id_label, ((void *)0), yyvsp[-1].u.stmt, poplevel())); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1437, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 314:
{ yyval.u.istmt.stmt = ({ast_generic tEmPcast = (ast_generic)(new_if_stmt(parse_region, yyvsp[-1].u.iexpr.location, yyvsp[-1].u.iexpr.expr, yyvsp[0].u.stmt, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1443, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  yyval.u.istmt.i = yyvsp[-1].u.iexpr.i; ;
    break;}
case 315:
{ yyval.u.istmt.i = yyvsp[-1].u.iexpr.i; yyval.u.istmt.stmt = new_error_stmt(parse_region, last_location); ;
    break;}
case 316:
{ yyval.u.iexpr.location = yyvsp[-3].u.itoken.location;
                  yyval.u.iexpr.i = stmt_count;
                  yyval.u.iexpr.expr = yyvsp[-1].u.expr;
                  check_condition("if", yyvsp[-1].u.expr); ;
    break;}
case 317:
{ stmt_count++;
                  compstmt_count++;
                  yyval.u.cstmt = ({ast_generic tEmPcast = (ast_generic)(new_dowhile_stmt(parse_region, yyvsp[0].u.itoken.location, ((void *)0), ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)", "c-parse.y", 1463, __PRETTY_FUNCTION__), 0))); (conditional_stmt)(tEmPcast); });
                 push_loop(({ast_generic tEmPcast = (ast_generic)(yyval.u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)", "c-parse.y", 1465, __PRETTY_FUNCTION__), 0))); (breakable_stmt)(tEmPcast); })); ;
    break;}
case 318:{ yyval.u.cstmt = yyvsp[-2].u.cstmt;
                  yyval.u.cstmt->stmt = yyvsp[-1].u.stmt; ;
    break;}
case 319:
{ yyval.u.stmt = yyvsp[0].u.stmt; ;
    break;}
case 320:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_labeled_stmt(parse_region, yyvsp[-1].u.label->loc, yyvsp[-1].u.label, yyvsp[0].u.stmt)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1475, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 321:
{ yyval.u.istmt.i = 0; yyval.u.istmt.stmt = yyvsp[0].u.stmt; ;
    break;}
case 322:{ yyval.u.istmt.i = 1; yyval.u.istmt.stmt = ({ast_generic tEmPcast = (ast_generic)(new_labeled_stmt(parse_region, yyvsp[0].u.label->loc, yyvsp[0].u.label, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1482, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 323:
{ stmt_count++; yyval.u.stmt = yyvsp[0].u.stmt; ;
    break;}
case 324:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_expression_stmt(parse_region, yyvsp[-1].u.expr->loc, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1491, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 325:{ yyvsp[-1].u.istmt.i = stmt_count; ;
    break;}
case 326:{ if (extra_warnings && stmt_count == yyvsp[-3].u.istmt.i)
                    warning("empty body in an else-statement");
                  yyval.u.stmt = yyvsp[-3].u.istmt.stmt;
                  ({ast_generic tEmPcast = (ast_generic)(yyval.u.stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)", "c-parse.y", 1498, __PRETTY_FUNCTION__), 0))); (if_stmt)(tEmPcast); })->stmt2 = yyvsp[0].u.stmt;
                ;
    break;}
case 327:{
                  if (extra_warnings && stmt_count++ == yyvsp[0].u.istmt.i)
                    warning_with_location (yyvsp[0].u.istmt.stmt->loc,
                                           "empty body in an if-statement");
                  yyval.u.stmt = yyvsp[0].u.istmt.stmt; ;
    break;}
case 328:{ yyval.u.stmt = new_error_stmt(parse_region, last_location); ;
    break;}
case 329:{ stmt_count++; ;
    break;}
case 330:{ check_condition("while", yyvsp[-1].u.expr);
                  yyval.u.cstmt = ({ast_generic tEmPcast = (ast_generic)(new_while_stmt(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-1].u.expr, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)", "c-parse.y", 1515, __PRETTY_FUNCTION__), 0))); (conditional_stmt)(tEmPcast); });
                  push_loop(({ast_generic tEmPcast = (ast_generic)(yyval.u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)", "c-parse.y", 1518, __PRETTY_FUNCTION__), 0))); (breakable_stmt)(tEmPcast); })); ;
    break;}
case 331:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1520, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  yyvsp[-1].u.cstmt->stmt = yyvsp[0].u.stmt;
                  pop_loop(); ;
    break;}
case 332:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(yyvsp[-4].u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1524, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  yyvsp[-4].u.cstmt->condition = yyvsp[-2].u.expr;
                  check_condition("do-while", yyvsp[-2].u.expr);
                  pop_loop(); ;
    break;}
case 333:{ yyval.u.stmt = new_error_stmt(parse_region, last_location);
                  pop_loop(); ;
    break;}
case 334:
{ stmt_count++; ;
    break;}
case 335:
{ if (yyvsp[-1].u.expr) check_condition("for", yyvsp[-1].u.expr); ;
    break;}
case 336:{ yyval.u.for_stmt = new_for_stmt(parse_region, yyvsp[-9].u.itoken.location, yyvsp[-7].u.expr, yyvsp[-4].u.expr, yyvsp[-1].u.expr, ((void *)0));
                  push_loop(({ast_generic tEmPcast = (ast_generic)(yyval.u.for_stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)", "c-parse.y", 1537, __PRETTY_FUNCTION__), 0))); (breakable_stmt)(tEmPcast); })); ;
    break;}
case 337:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.for_stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1539, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  yyvsp[-1].u.for_stmt->stmt = yyvsp[0].u.stmt;
                  pop_loop(); ;
    break;}
case 338:{ stmt_count++; check_switch(yyvsp[-1].u.expr);
                  yyval.u.cstmt = ({ast_generic tEmPcast = (ast_generic)(new_switch_stmt(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-1].u.expr, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_conditional_stmt && (tEmPcast)->kind <= postkind_conditional_stmt)", "c-parse.y", 1544, __PRETTY_FUNCTION__), 0))); (conditional_stmt)(tEmPcast); });
                  push_loop(({ast_generic tEmPcast = (ast_generic)(yyval.u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_breakable_stmt && (tEmPcast)->kind <= postkind_breakable_stmt)", "c-parse.y", 1546, __PRETTY_FUNCTION__), 0))); (breakable_stmt)(tEmPcast); })); ;
    break;}
case 339:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.cstmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1548, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  yyvsp[-1].u.cstmt->stmt = yyvsp[0].u.stmt;
                  pop_loop(); ;
    break;}
case 340:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_break_stmt(parse_region, yyvsp[-1].u.itoken.location)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1553, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  check_break(yyval.u.stmt);
                ;
    break;}
case 341:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_continue_stmt(parse_region, yyvsp[-1].u.itoken.location)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1558, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  check_continue(yyval.u.stmt);
                ;
    break;}
case 342:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_return_stmt(parse_region, yyvsp[-1].u.itoken.location, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1563, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  check_void_return(); ;
    break;}
case 343:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_return_stmt(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1567, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  check_return(yyvsp[-1].u.expr); ;
    break;}
case 344:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_asm_stmt(parse_region, yyvsp[-5].u.itoken.location, yyvsp[-2].u.expr, ((void *)0), ((void *)0), ((void *)0), yyvsp[-4].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1571, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 345:
{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_asm_stmt(parse_region, yyvsp[-7].u.itoken.location, yyvsp[-4].u.expr, yyvsp[-2].u.asm_operand, ((void *)0), ((void *)0), yyvsp[-6].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1576, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 346:
{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_asm_stmt(parse_region, yyvsp[-9].u.itoken.location, yyvsp[-6].u.expr, yyvsp[-4].u.asm_operand, yyvsp[-2].u.asm_operand, ((void *)0), yyvsp[-8].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1581, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 347:
{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_asm_stmt(parse_region, yyvsp[-11].u.itoken.location, yyvsp[-8].u.expr, yyvsp[-6].u.asm_operand, yyvsp[-4].u.asm_operand, yyvsp[-2].u.string, yyvsp[-10].u.telement)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1586, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 348:{ stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_goto_stmt(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-1].u.id_label)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1589, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  use_label(yyvsp[-1].u.id_label);
                ;
    break;}
case 349:{ if (pedantic)
                    pedwarn("ANSI C forbids `goto *expr;'");
                  stmt_count++;
                  yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_computed_goto_stmt(parse_region, yyvsp[-3].u.itoken.location, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1596, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
                  check_computed_goto(yyvsp[-1].u.expr); ;
    break;}
case 350:
{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_empty_stmt(parse_region, yyvsp[0].u.itoken.location)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1598, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 351:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_assert_type_stmt(parse_region, yyvsp[-6].u.itoken.location, yyvsp[-4].u.expr, yyvsp[-2].u.type)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1600, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 352:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_change_type_stmt(parse_region, yyvsp[-6].u.itoken.location, yyvsp[-4].u.expr, yyvsp[-2].u.type)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1602, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 353:{ yyval.u.stmt = ({ast_generic tEmPcast = (ast_generic)(new_deep_restrict_stmt(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-2].u.expr, yyvsp[0].u.stmt)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "c-parse.y", 1604, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }); ;
    break;}
case 354:
{ yyval.u.label = ({ast_generic tEmPcast = (ast_generic)(new_case_label(parse_region, yyvsp[-2].u.itoken.location, yyvsp[-1].u.expr, ((void *)0))); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)", "c-parse.y", 1612, __PRETTY_FUNCTION__), 0))); (label)(tEmPcast); });
                  check_case(yyval.u.label); ;
    break;}
case 355:{ yyval.u.label = ({ast_generic tEmPcast = (ast_generic)(new_case_label(parse_region, yyvsp[-4].u.itoken.location, yyvsp[-3].u.expr, yyvsp[-1].u.expr)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)", "c-parse.y", 1615, __PRETTY_FUNCTION__), 0))); (label)(tEmPcast); });
                  check_case(yyval.u.label); ;
    break;}
case 356:{ yyval.u.label = ({ast_generic tEmPcast = (ast_generic)(new_default_label(parse_region, yyvsp[-1].u.itoken.location)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)", "c-parse.y", 1618, __PRETTY_FUNCTION__), 0))); (label)(tEmPcast); });
                  check_default(yyval.u.label); ;
    break;}
case 357:{ yyval.u.label = ({ast_generic tEmPcast = (ast_generic)(yyvsp[-1].u.id_label); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_label && (tEmPcast)->kind <= postkind_label)", "c-parse.y", 1621, __PRETTY_FUNCTION__), 0))); (label)(tEmPcast); });
                  define_label(yyvsp[-1].u.id_label); ;
    break;}
case 358:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 360:
{ yyval.u.expr = ((void *)0); ;
    break;}
case 362:
{ yyval.u.asm_operand = ((void *)0); ;
    break;}
case 365:
{ yyval.u.asm_operand = asm_operand_chain(yyvsp[-2].u.asm_operand, yyvsp[0].u.asm_operand); ;
    break;}
case 366:
{ yyval.u.asm_operand = new_asm_operand(parse_region, yyvsp[-3].u.expr->loc,
                                       make_string(yyvsp[-3].u.expr->loc, ({ast_generic tEmPcast = (ast_generic)(yyvsp[-3].u.expr); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "c-parse.y", 1655, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); })),
                                       yyvsp[-1].u.expr); ;
    break;}
case 367:
{ yyval.u.string = yyvsp[0].u.string; ;
    break;}
case 368:{ yyval.u.string = string_chain(yyvsp[-2].u.string, yyvsp[0].u.string); ;
    break;}
case 369:
{ pushlevel(1); ;
    break;}
case 370:{ yyval.u.decl = yyvsp[0].u.decl;
                ;
    break;}
case 371:
{ yyval.u.decl = yyvsp[-1].u.decl; ;
    break;}
case 372:{ if (pedantic)
                    pedwarn("ANSI C forbids forward parameter declarations");
                  mark_forward_parameters(yyvsp[-1].u.decl);
                  ;
    break;}
case 373:{ yyval.u.decl = declaration_chain(yyvsp[-3].u.decl, yyvsp[0].u.decl); ;
    break;}
case 374:{ yyval.u.decl = new_error_decl(parse_region, last_location); ;
    break;}
case 375:
{ yyval.u.decl = ((void *)0); ;
    break;}
case 376:{ yyval.u.decl = new_error_decl(parse_region, last_location);
                  error("ANSI C requires a named argument before `...'");
                ;
    break;}
case 377:{ yyval.u.decl = yyvsp[0].u.decl; ;
    break;}
case 378:{ yyval.u.decl = declaration_chain(yyvsp[-2].u.decl, yyvsp[0].u.decl); ;
    break;}
case 380:
{ yyval.u.decl = declaration_chain(yyvsp[-2].u.decl, yyvsp[0].u.decl); ;
    break;}
case 381:
{ yyval.u.decl = declare_parameter(yyvsp[-1].u.declarator, current_declspecs, yyvsp[0].u.attribute,
                                         prefix_attributes, 0);
                  pop_declspec_stack(); ;
    break;}
case 382:{ yyval.u.decl = declare_parameter(yyvsp[-1].u.declarator, current_declspecs, yyvsp[0].u.attribute,
                                         prefix_attributes, 0);
                  pop_declspec_stack(); ;
    break;}
case 383:{ yyval.u.decl = declare_parameter(yyvsp[-1].u.declarator, current_declspecs, yyvsp[0].u.attribute,
                                         prefix_attributes, 1);
                pop_declspec_stack(); ;
    break;}
case 384:{ yyval.u.decl = declare_parameter(yyvsp[-1].u.declarator, current_declspecs, yyvsp[0].u.attribute,
                                         prefix_attributes, 0);
                  pop_declspec_stack(); ;
    break;}
case 385:{ yyval.u.decl = declare_parameter(yyvsp[-1].u.declarator, current_declspecs, yyvsp[0].u.attribute,
                                         prefix_attributes, 1);
                  pop_declspec_stack(); ;
    break;}
case 386:
{ pushlevel(1); ;
    break;}
case 387:{ yyval.u.decl = yyvsp[0].u.decl;
                                                                      ;
    break;}
case 389:
{ yyval.u.decl = yyvsp[-1].u.decl; ;
    break;}
case 390:
{ yyval.u.decl = yyvsp[0].u.decl; ;
    break;}
case 391:{ yyval.u.decl = declaration_chain(yyvsp[-2].u.decl, yyvsp[0].u.decl); ;
    break;}
case 392:
{ yyval.u.decl = declare_old_parameter(yyvsp[0].idtoken.location, yyvsp[0].idtoken.id); ;
    break;}
case 393:
{ yyval.u.id_label = yyvsp[0].u.id_label; declare_label(yyvsp[0].u.id_label); ;
    break;}
case 394:{ yyval.u.id_label = id_label_chain(yyvsp[-2].u.id_label, yyvsp[0].u.id_label);
                  declare_label(yyvsp[0].u.id_label); ;
    break;}
case 395:
{ yyval.u.telement = ((void *)0); ;
    break;}
case 396:
{ yyval.u.telement = yyvsp[0].u.telement; ;
    break;}
case 397:
{ yyval.u.itoken.location = yyvsp[0].u.itoken.location;
                  yyval.u.itoken.i = pedantic;
                  pedantic = 0; ;
    break;}
case 398:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_rid(parse_region, yyvsp[0].u.itoken.location, yyvsp[0].u.itoken.i)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1789, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 399:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_qualifier(parse_region, yyvsp[0].u.itoken.location, yyvsp[0].u.itoken.i)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1793, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 400:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_user_qual(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1794, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 401:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_qualifier(parse_region, yyvsp[0].u.itoken.location, yyvsp[0].u.itoken.i)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1799, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 402:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_user_qual(parse_region, yyvsp[0].idtoken.location, yyvsp[0].idtoken.id)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1800, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
case 403:
{ yyval.u.telement = ({ast_generic tEmPcast = (ast_generic)(new_rid(parse_region, yyvsp[0].u.itoken.location, yyvsp[0].u.itoken.i)); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_type_element && (tEmPcast)->kind <= postkind_type_element)", "c-parse.y", 1805, __PRETTY_FUNCTION__), 0))); (type_element)(tEmPcast); }); ;
    break;}
}
  yyvsp -= yylen;
  yyssp -= yylen;
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (yyssp1 != yyssp)
        fprintf (stderr, " %d", *++yyssp1);
      fprintf (stderr, "\n");
    }
  *++yyvsp = yyval;
  yyn = yyr1[yyn];
  yystate = yypgoto[yyn - 77] + *yyssp;
  if (yystate >= 0 && yystate <= 3064 && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - 77];
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
      do { if (yydebug) fprintf (stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]); } while (0);
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
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
        fprintf (stderr, " %d", *++yyssp1);
      fprintf (stderr, "\n");
    }
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == -32768)
    goto yyerrdefault;
  yyn += 1;
  if (yyn < 0 || yyn > 3064 || yycheck[yyn] != 1)
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
  if (yyn == 705)
    goto yyacceptlab;
  do { if (yydebug) fprintf (stderr, "Shifting error token, "); } while (0);
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
const char CANON_IDENT_39934814c8ae4e12f980f021110a345b[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/c-parse.tab.c";
