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
struct flock
  {
    short int l_type;
    short int l_whence;
    __off_t l_start;
    __off_t l_len;
    __pid_t l_pid;
  };
extern int fcntl (int __fd, int __cmd, ...) ;
extern int open (__const char *__file, int __oflag, ...) ;
extern int creat (__const char *__file, __mode_t __mode) ;
extern int lockf (int __fd, int __cmd, __off_t __len) ;
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
void analyze_flow_sensitive(declaration program);
void init_flow_sensitive(void);
void finish_flow_sensitive(void);
bool is_externally_visible_fn(data_declaration ddecl);
extern store global_store;
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
typedef enum { lpos,
               rpos,
               apos,
               cpos, } context;
typedef struct einfo {
  qtype qt;
  store out;
  store true_out;
  store false_out;
  bool ismalloc;
} einfo;
typedef struct sinfo {
  store out;
} sinfo;
typedef struct dinfo {
  store out;
} dinfo;
void analyze_flow_sensitive(declaration);
static dinfo analyze_declaration(declaration, store, dd_list drinfolist);
static sinfo analyze_init(location, qtype, expression, store,
                          dd_list drinfolist);
static sinfo analyze_statement(statement, store, dd_list drinfolist);
static einfo analyze_expression(expression, context, store,
                                dd_list drinfolist);
static einfo analyze_unary_expression(unary, context, store,
                                      dd_list drinfolist);
static einfo analyze_binary_expression(binary, context, store,
                                       dd_list drinfolist);
static einfo analyze_magic_function_call(function_call, qtype, store,
                                         dd_list drinfolist);
static einfo split_file_pointer_context(location loc, qtype qt, store s);
bool file_pointer_fs_qtype(qtype qt, store s);
static store allocate_qtype(location loc, qtype qt, store in);
static einfo put_id_in_fs_context(location, qtype, context, store, aloc);
static store confine_expressions(compound_stmt cs, store in,
                                 dd_list drinfolist);
static store merge_confines(compound_stmt cs, store in);
static void print_queue(void);
static void queue_print_exp(expression expr, store s, aloc al);
static void queue_print_stmt(statement stmt, store s, aloc al);
void prt_expression(expression e, int context_priority);
void prt_statement(statement s);
static store tfr_jumped_blocks(store s, compound_stmt from, compound_stmt to);
static store allocate_ddecl(data_declaration ddecl, store s);
static store merge_restricts(declaration, store);
void print_assert_type_failures(void);
bool is_void_parms(declaration);
bool isassignment(binary e);
const char *binary_op_name(ast_kind);
bool equal_expressions(expression e1, expression e2);
static store toplvl_store = ((void *)0);
static store global_store_ext = ((void *)0);
static dd_list function_defns = ((void *)0);
static dd_list printqueue = ((void *)0);
static dd_list assert_type_qtypes = ((void *)0);
static dd_list assert_type_qtypes_hax = ((void *)0);
void init_flow_sensitive(void)
{
  current_function_decl = ((void *)0);
  global_store = make_store_var(((void *)0), "global");
  global_store_ext = make_store_var(((void *)0), "global");
  function_defns = dd_new_list(parse_region);
  printqueue = dd_new_list(parse_region);
  assert_type_qtypes = dd_new_list(parse_region);
  assert_type_qtypes_hax = dd_new_list(parse_region);
  reset_flow_sensitive_quals();
}
void finish_flow_sensitive(void)
{
  data_declaration main_ddecl;
  mkleq_store(((void *)0), "inconsistent flow-sensitive qualifiers",
              global_store_ext, global_store);
  main_ddecl = get_main_ddecl();
  if (main_ddecl && main_ddecl->definition)
    {
      qtype fqt;
      store main_store_in;
      effect main_effect;
      fqt = main_ddecl->qtype;
      main_store_in = store_in_qtype(fqt);
      main_effect = effect_qtype(fqt);
      mkleq_store(location_declaration(main_ddecl->ast),
                  "inconsistent stores at main entry",
                  make_store_filter(((void *)0), toplvl_store, main_effect),
                  main_store_in);
    }
  else if (flag_driver)
    {
      bool found;
      dd_list_pos cur;
      found = 0;
      for (cur = dd_first((function_defns)); !(!(cur)->next); cur = ((cur)->next))
        {
          function_decl fd;
          data_declaration ddecl;
          fd = ((function_decl)((cur)->data));
          ddecl = root_ddecl(fd->ddecl);
          if (is_externally_visible_fn(ddecl))
            {
              qtype fqt;
              store store_in, store_out;
              effect eff;
              found = 1;
              fprintf(stderr, "Assuming %s is an interface function\n",
                      ddecl->name);
              fqt = ddecl->qtype;
              store_in = store_in_qtype(fqt);
              store_out = store_out_qtype(fqt);
              eff = effect_qtype(fqt);
              mkleq_store(location_declaration(ddecl->ast),
                          "inconsistent stores at interface function entry",
                          make_store_filter(((void *)0), toplvl_store, eff),
                          store_in);
              mkleq_store(location_declaration(ddecl->ast),
                          "inconsistent stores at interface function exit",
                          make_store_ow(((void *)0), store_out, toplvl_store, eff),
                          toplvl_store);
            }
        }
      if (!found)
        report_qerror(((void *)0), sev_warn,
                      "No interface functions found\n");
    }
  else
    report_qerror(((void *)0), sev_warn,
                  "No main function -- initial environment discarded");
  if (!flag_strong_updates)
    compute_lins();
  if (!flag_pam_mode)
    traverse_globals_sorted((traverse_global_fn) warn_if_dangerous_global,
                            global_store);
  if (flag_print_lin)
    print_queue();
  if (flag_print_assert_type_failures)
    print_assert_type_failures();
}
static inline einfo mkeinfo(qtype qt, store out, store true_out,
                            store false_out, bool ismalloc)
{
  struct einfo result = {qt: qt,
                         out: out,
                         true_out: true_out,
                         false_out: false_out,
                         ismalloc: ismalloc};
  return result;
}
static inline store true_out_einfo(einfo ei)
{
  if (ei.true_out)
    return ei.true_out;
  else
    {
      ((void) ((ei.out) ? 0 : (__assert_fail ("ei.out", "flow.c", 234, __PRETTY_FUNCTION__), 0)));
      return ei.out;
    }
}
static inline store false_out_einfo(einfo ei)
{
  if (ei.false_out)
    return ei.false_out;
  else
    {
      ((void) ((ei.out) ? 0 : (__assert_fail ("ei.out", "flow.c", 245, __PRETTY_FUNCTION__), 0)));
      return ei.out;
    }
}
static inline sinfo mksinfo(store out)
{
  struct sinfo result = {out: out};
  return result;
}
static inline dinfo mkdinfo(store out)
{
  struct dinfo result = {out: out};
  return result;
}
bool is_externally_visible_fn(data_declaration ddecl)
{
  ddecl = root_ddecl(ddecl);
  return (ddecl->kind == decl_function &&
          !ddecl->isexterninline &&
          (ddecl->ftype != function_static || ddecl->addresstaken));
}
static void add_flow_sensitive_declarator_overlay(declarator d, qtype fs_qtype,
                                                  store s)
{
  identifier_declarator id = get_id_declarator(d);
  cstring name = id->cstring;
  pam_add_overlay_flow_sensitive(id->loc, name.data, (gqtype) fs_qtype, s);
}
static bool init_restrict(variable_decl vd, store s)
{
  bool result;
  qtype fs_fqtype, fs_rqtype;
  ((void) ((vd->rinfo) ? 0 : (__assert_fail ("vd->rinfo", "flow.c", 290, __PRETTY_FUNCTION__), 0)));
  result = 0;
  fs_fqtype = vd->rinfo->fs_fqtype;
  fs_rqtype = vd->rinfo->fs_rqtype;
  result = mkleq_qual(vd->loc,
                      &fs_qgate,
                      qual_qtype(fs_fqtype),
                      qual_qtype(fs_rqtype)) || result;
  fs_fqtype = points_to_fs_qtype(fs_fqtype, s);
  fs_rqtype = points_to_fs_qtype(fs_rqtype, s);
  result = mkleq_qual(vd->loc,
                      &fs_qgate,
                      qual_qtype(fs_fqtype),
                      qual_qtype(fs_rqtype)) || result;
  fs_fqtype = points_to_fs_qtype(fs_fqtype, s);
  fs_rqtype = points_to_fs_qtype(fs_rqtype, s);
  result = mkleq_qtype(vd->loc, &fs_qgate, fs_fqtype, fs_rqtype) || result;
  return result;
}
static void add_ddecl_fs_qtype(data_declaration ddecl, store s)
{
  location loc;
  if (!ddecl->qtype || ddecl->fs_qtype)
    return;
  ddecl = root_ddecl(ddecl);
  loc = location_declaration(ddecl->ast);
  if (ddecl->global)
    {
      data_declaration global;
      global = ddecl->global;
      if (!global)
        {
          ddecl->fs_qtype = qtype_to_fs_qtype_with_quals(loc,
                                                         ddecl->qtype,
                                                         global_store);
          return;
        }
      if (global && !global->fs_qtype)
          global->fs_qtype =
            qtype_to_fs_qtype_with_quals(location_declaration(global->ast),
                                         global->qtype,
                                         global_store);
      if (global->in_prelude && !ddecl->in_prelude)
        {
          ddecl->fs_qtype = global->fs_qtype;
          return;
        }
      if (match_type(ddecl->type, global->type))
        ddecl->fs_qtype = global->fs_qtype;
      else
        {
          ddecl->fs_qtype = qtype_to_fs_qtype_with_quals(loc,
                                                         ddecl->qtype,
                                                         global_store);
          if (unify_qtype(loc, global->fs_qtype, ddecl->fs_qtype))
            {
              report_qerror(loc, sev_err, "conflicting types for ``%s''",
                            ddecl->name);
              report_qerror(global->ast->loc, sev_info,
                            "-- previous declaration of ``%s''", ddecl->name);
            }
        }
    }
  else
    ddecl->fs_qtype = qtype_to_fs_qtype_with_quals(loc,
                                                   ddecl->qtype,
                                                   s);
}
void analyze_flow_sensitive(declaration program)
{
  declaration d;
  dinfo next_info;
  dd_list global_drinfolist;
  global_drinfolist = dd_new_list(parse_region);
  toplvl_store = global_store;
  for (d = (program); d; d = ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 397, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
    {
      next_info = analyze_declaration(d, toplvl_store, global_drinfolist);
      toplvl_store = next_info.out;
    }
}
static dinfo analyze_declaration(declaration d, store in, dd_list drinfolist)
{
  switch (d->kind)
    {
    case kind_asm_decl:
      return mkdinfo(in);
    case kind_data_decl:
      {
        data_decl dd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "flow.c", 414, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
        declaration decl;
        store cur_store;
        cur_store = in;
        for (decl = (dd->decls); decl; decl = ({ast_generic tEmPcast = (ast_generic)(decl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 420, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); })) {
          dinfo next;
          next = analyze_declaration(decl, cur_store, drinfolist);
          cur_store = next.out;
        }
        return mkdinfo(cur_store);
      }
    case kind_variable_decl:
      {
        variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "flow.c", 430, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
        store cur_store;
        data_declaration root;
        cur_store = in;
        root = root_ddecl(vd->ddecl);
        if ((!(__extension__ (__builtin_constant_p (4) && ((__builtin_constant_p ("_op_") && strlen ("_op_") < ((size_t) (4))) || (__builtin_constant_p (root->name) && strlen (root->name) < ((size_t) (4)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("_op_") && __builtin_constant_p (root->name) && (__s1_len = strlen ("_op_"), __s2_len = strlen (root->name), (!((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("_op_"), (__const char *) (root->name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) && (__s1_len = strlen ("_op_"), __s1_len < 4) ? (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (root->name); register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (root->name) && ((size_t)(const void *)((root->name) + 1) - (size_t)(const void *)(root->name) == 1) && (__s2_len = strlen (root->name), __s2_len < 4) ? (__builtin_constant_p ("_op_") && ((size_t)(const void *)(("_op_") + 1) - (size_t)(const void *)("_op_") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("_op_"))[0] - ((__const unsigned char *) (__const char *)(root->name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("_op_"))[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("_op_"))[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("_op_"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (root->name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (root->name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (root->name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (root->name))[3]); } } __result; }))) : strcmp ("_op_", root->name)))); }) : strncmp ("_op_", root->name, 4))) && strlen(root->name) >= 4))
          {
            report_qerror(vd->loc, sev_warn,
                          "warning: ignoring operator declaration in flow-sensitive analysis");
            return mkdinfo(cur_store);
          }
        if (vd->rinfo)
          root->qtype = vd->rinfo->fqtype;
        cur_store = allocate_ddecl(root, cur_store);
        add_ddecl_fs_qtype(root, cur_store);
        if (vd->arg1)
          {
            qtype init_qt;
            sinfo arg1;
            init_qt = root->fs_qtype;
            if (!qtype_error(init_qt) && !qtype_array(init_qt))
              init_qt = points_to_fs_qtype(init_qt, cur_store);
            arg1 = analyze_init(vd->loc, init_qt, vd->arg1, cur_store, drinfolist);
            cur_store = arg1.out;
          }
        if (vd->rinfo)
          {
            ((void) ((!vd->rinfo->fs_rqtype && !vd->rinfo->fs_fqtype) ? 0 : (__assert_fail ("!vd->rinfo->fs_rqtype && !vd->rinfo->fs_fqtype", "flow.c", 475, __PRETTY_FUNCTION__), 0)));
            ((void) ((qtype_pointer(vd->rinfo->rqtype)) ? 0 : (__assert_fail ("qtype_pointer(vd->rinfo->rqtype)", "flow.c", 476, __PRETTY_FUNCTION__), 0)));
            cur_store = ref_qtype_store(vd->loc, vd->rinfo->rqtype, cur_store);
            cur_store = make_store_ref_effect(vd->loc, cur_store,
                                              vd->rinfo->rlocs);
            vd->rinfo->fs_fqtype = root->fs_qtype;
            vd->rinfo->fs_rqtype =
              qtype_to_fs_qtype_with_quals(vd->loc, vd->rinfo->rqtype,
                                           cur_store);
            if (init_restrict(vd, cur_store))
              report_qerror(vd->loc, sev_err,
                            "incompatible right-hand side in restricted "
                            "location initialization");
            root->fs_qtype = vd->rinfo->fs_rqtype;
          }
        if ((flag_pam_mode || flag_print_results) &&
            root->kind != decl_typedef)
          add_flow_sensitive_declarator_overlay(vd->declarator,
                                                root->fs_qtype,
                                                cur_store);
        return mkdinfo(cur_store);
      }
    case kind_function_decl:
      {
        function_decl fd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_decl && (tEmPcast)->kind <= postkind_function_decl)", "flow.c", 511, __PRETTY_FUNCTION__), 0))); (function_decl)(tEmPcast); }), old_function_decl;
        data_declaration root;
        store cur_store;
        dd_add_last(parse_region, function_defns, fd);
        root = root_ddecl(fd->ddecl);
        if (qtype_scheme(root->qtype))
          return mkdinfo(in);
        add_ddecl_fs_qtype(root, in);
        if (root->global && root->global->in_prelude)
          return mkdinfo(in);
        global_store_ext = allocate_qtype(location_declaration(root->ast),
                                          root->qtype,
                                          global_store_ext);
        if (flag_pam_mode || flag_print_results)
          add_flow_sensitive_declarator_overlay(fd->declarator,
                                                root->fs_qtype,
                                                in);
        old_function_decl = current_function_decl;
        current_function_decl = fd;
        if (qtype_error(root->fs_qtype))
          cur_store = make_store_var(fd->loc, "error_store");
        else
          cur_store = store_in_qtype(root->fs_qtype);
        {
          declaration arg;
          qtypelist_scanner old_qtypes;
          int i;
          i = 1;
          if (qtype_error(root->fs_qtype))
            old_qtypes = ((void *)0);
          else
            qtypelist_scan(arg_qtypes(root->fs_qtype), &old_qtypes);
          if (!is_void_parms(fd->fdeclarator->parms))
            for (arg = (fd->fdeclarator->parms); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 561, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
              if (!((arg)->kind >= kind_ellipsis_decl && (arg)->kind <= postkind_ellipsis_decl))
                {
                  data_declaration root_vd;
                  variable_decl argvd;
                  oldidentifier_decl oid;
                  qtype old_qtype, arg_qt;
                  if (arg->kind == kind_data_decl)
                    {
                      data_decl argd = ({ast_generic tEmPcast = (ast_generic)(arg); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "flow.c", 573, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
                      argvd = ({ast_generic tEmPcast = (ast_generic)(argd->decls); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "flow.c", 575, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
                      ((void) ((!argvd->next) ? 0 : (__assert_fail ("!argvd->next", "flow.c", 576, __PRETTY_FUNCTION__), 0)));
                      root_vd = root_ddecl(argvd->ddecl);
                      oid = ((void *)0);
                    }
                  else
                    {
                      oid = ({ast_generic tEmPcast = (ast_generic)(arg); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_oldidentifier_decl && (tEmPcast)->kind <= postkind_oldidentifier_decl)", "flow.c", 584, __PRETTY_FUNCTION__), 0))); (oldidentifier_decl)(tEmPcast); });
                      argvd = ((void *)0);
                      root_vd = root_ddecl(oid->ddecl);
                    }
                  ((void) ((!root_vd->fs_qtype) ? 0 : (__assert_fail ("!root_vd->fs_qtype", "flow.c", 591, __PRETTY_FUNCTION__), 0)));
                  ((void) ((qtype_pointer(root_vd->qtype)) ? 0 : (__assert_fail ("qtype_pointer(root_vd->qtype)", "flow.c", 592, __PRETTY_FUNCTION__), 0)));
                  if (argvd && argvd->rinfo)
                    root_vd->qtype = argvd->rinfo->fqtype;
                  cur_store =
                    allocate_qtype(location_declaration(root_vd->ast),
                                   root_vd->qtype,
                                   cur_store);
                  root_vd->fs_qtype =
                    qtype_to_fs_qtype_with_quals(arg->loc,
                                                 root_vd->qtype,
                                                 cur_store);
                  arg_qt = points_to_fs_qtype(root_vd->fs_qtype, cur_store);
                  if (!qtype_error(root->fs_qtype))
                    {
                      old_qtype = qtypelist_next(&old_qtypes);
                      if (old_qtype &&
                          unify_qtype(arg->loc, old_qtype, arg_qt))
                        report_qerror(arg->loc, sev_err,
                                      "type of parameter %d doesn't match "
                                      "previous uses",
                                      i);
                      else if (!old_qtype && arg->kind == kind_data_decl)
                        report_qerror(arg->loc, sev_err,
                                      "parameter %d doesn't appear in "
                                      "previous uses", i);
                    }
                  if (argvd && argvd->rinfo)
                    {
                      ((void) ((!argvd->rinfo->fs_rqtype && !argvd->rinfo->fs_fqtype) ? 0 : (__assert_fail ("!argvd->rinfo->fs_rqtype && !argvd->rinfo->fs_fqtype", "flow.c", 631, __PRETTY_FUNCTION__), 0)));
                      ((void) ((qtype_pointer(argvd->rinfo->rqtype)) ? 0 : (__assert_fail ("qtype_pointer(argvd->rinfo->rqtype)", "flow.c", 633, __PRETTY_FUNCTION__), 0)));
                      cur_store = ref_qtype_store(arg->loc,
                                                  argvd->rinfo->rqtype,
                                                  cur_store);
                      cur_store = make_store_ref_effect(arg->loc,
                                                        cur_store,
                                                        argvd->rinfo->rlocs);
                      argvd->rinfo->fs_fqtype = root_vd->fs_qtype;
                      argvd->rinfo->fs_rqtype =
                        qtype_to_fs_qtype_with_quals(arg->loc,
                                                     argvd->rinfo->rqtype,
                                                     cur_store);
                      if (init_restrict(argvd, cur_store))
                        report_qerror(argvd->loc, sev_err,
                                      "type of restricted parameter %d "
                                      "doesn't match previous uses",
                                      i);
                      root_vd->fs_qtype = argvd->rinfo->fs_rqtype;
                    }
                  i++;
                  if (flag_pam_mode || flag_print_results)
                    {
                      if (arg->kind == kind_data_decl)
                       add_flow_sensitive_declarator_overlay(argvd->declarator,
                                                             root_vd->fs_qtype,
                                                             cur_store);
                      else
                        pam_add_overlay_flow_sensitive(oid->loc,
                                                       oid->cstring.data,
                                                       (gqtype)root_vd->fs_qtype,
                                                       cur_store);
                    }
                }
        }
        {
          sinfo body;
          ((void) ((((fd->stmt)->kind >= kind_compound_stmt && (fd->stmt)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((fd->stmt)->kind >= kind_compound_stmt && (fd->stmt)->kind <= postkind_compound_stmt)", "flow.c", 685, __PRETTY_FUNCTION__), 0)));
          body = analyze_statement(fd->stmt, cur_store, drinfolist);
          cur_store = body.out;
          cur_store = merge_restricts(fd->fdeclarator->parms, body.out);
          cur_store = make_store_filter(fd->loc, cur_store, fd->scope_env);
          if (!qtype_error(root->fs_qtype))
            mkleq_store(fd->loc,
                        "inconsitent stores at function exit",
                        cur_store, store_out_qtype(root->fs_qtype));
        }
        current_function_decl = old_function_decl;
        return mkdinfo(in);
      }
    case kind_extension_decl:
      {
        dinfo result;
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "flow.c", 704, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        result = analyze_declaration(ed->decl, in, drinfolist);
        return result;
      }
    default:
      __fail("flow.c", 710, __FUNCTION__, "Unexpected decl kind %x\n", d->kind);
    }
}
static bool init_string(location loc, qtype lhs_qtype, qtype rhs_qtype,
                        store in)
{
  bool result = 0;
  qtype lhs_contents_qtype, rhs_contents_qtype;
  lhs_contents_qtype = array_of_qtype(lhs_qtype);
  rhs_contents_qtype = points_to_fs_qtype(rhs_qtype, in);
  result = mkleq_qual(loc,
                      &fs_qgate,
                      qual_qtype(rhs_contents_qtype),
                      qual_qtype(lhs_contents_qtype)) || result;
  result = mkleq_qual(loc,
                      &fs_qgate,
                      qual_qtype(rhs_qtype),
                      qual_qtype(lhs_qtype)) || result;
  return result;
}
static sinfo analyze_init(location loc, qtype lhs_qtype, expression rhs,
                          store in, dd_list drinfolist)
{
  if (rhs->kind == kind_string && qtype_string(lhs_qtype))
    {
      einfo arg1;
      arg1 = analyze_expression(rhs, rpos, in, drinfolist);
      if (init_string(rhs->loc, lhs_qtype, arg1.qt, in))
        report_qerror(rhs->loc, sev_err, "incompatibile initializer");
      return mksinfo(arg1.out);
    }
  if (rhs->kind == kind_init_list && (qtype_array(lhs_qtype)))
    {
      init_list il = ({ast_generic tEmPcast = (ast_generic)(rhs); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)", "flow.c", 750, __PRETTY_FUNCTION__), 0))); (init_list)(tEmPcast); });
      store cur_store;
      expression e;
      sinfo init;
      cur_store = in;
      for (e = (il->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 757, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        if (e->kind == kind_init_index)
          {
            init_index ii = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_index && (tEmPcast)->kind <= postkind_init_index)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_index && (tEmPcast)->kind <= postkind_init_index)", "flow.c", 760, __PRETTY_FUNCTION__), 0))); (init_index)(tEmPcast); });
            einfo arg1, arg2;
            arg1 = analyze_expression(ii->arg1, rpos, cur_store, drinfolist);
            cur_store = arg1.out;
            if (ii->arg2)
              {
                arg2 = analyze_expression(ii->arg2, rpos, cur_store, drinfolist);
                cur_store = arg2.out;
              }
            init = analyze_init(loc, array_of_qtype(lhs_qtype),
                                ii->init_expr, cur_store, drinfolist);
            cur_store = init.out;
          }
        else
          {
            init = analyze_init(loc, array_of_qtype(lhs_qtype),
                                e, cur_store, drinfolist);
            cur_store = init.out;
          }
      return mksinfo(cur_store);
     }
  if (rhs->kind == kind_init_list && (qtype_struct(lhs_qtype) ||
                                      qtype_union(lhs_qtype)))
     {
       init_list il = ({ast_generic tEmPcast = (ast_generic)(rhs); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_list && (tEmPcast)->kind <= postkind_init_list)", "flow.c", 787, __PRETTY_FUNCTION__), 0))); (init_list)(tEmPcast); });
       field_scanner fs;
       qtype fqt;
       expression e;
       store cur_store;
       fqt = ((void *)0);
       field_scan(lhs_qtype, &fs);
       cur_store = in;
       for (e = (il->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 797, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
         {
           expression init_expr;
           sinfo init;
           if (e->kind == kind_init_field)
             {
               init_field ifd = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_init_field && (tEmPcast)->kind <= postkind_init_field)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_init_field && (tEmPcast)->kind <= postkind_init_field)", "flow.c", 808, __PRETTY_FUNCTION__), 0))); (init_field)(tEmPcast); });
               field_scan_at(lhs_qtype, ifd->word1->cstring, &fs);
               if (!fs)
                 break;
               init_expr = ifd->init_expr;
             }
           else
             init_expr = e;
           fqt = field_next(&fs);
           if (!fqt)
             break;
           if (!qtype_array(fqt))
             fqt = points_to_fs_qtype(fqt, cur_store);
           init = analyze_init(loc, fqt, init_expr, cur_store, drinfolist);
           cur_store = init.out;
         }
       return mksinfo(cur_store);
     }
  {
    einfo arg1;
    arg1 = analyze_expression(rhs, rpos, in, drinfolist);
    if (mkleq_fs_assign_convert(loc, arg1.qt, lhs_qtype, arg1.out))
      report_qerror(loc, sev_err,
                    "incompatible right-hand side in initialization");
    return mksinfo(arg1.out);
  }
}
static sinfo analyze_statement(statement s, store in, dd_list drinfolist)
{
  switch (s->kind)
    {
    case kind_asm_stmt:
      return mksinfo(in);
      break;
    case kind_compound_stmt:
      {
        compound_stmt cs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)", "flow.c", 864, __PRETTY_FUNCTION__), 0))); (compound_stmt)(tEmPcast); });
        declaration decl;
        statement stmt;
        store cur_store, out;
        dd_list new_drinfolist;
        new_drinfolist = dd_copy(parse_region, drinfolist);
        cs->drinfolist = new_drinfolist;
        cur_store = in;
        for (decl = (cs->decls); decl; decl = ({ast_generic tEmPcast = (ast_generic)(decl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 877, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          {
            dinfo next;
            next = analyze_declaration(decl, cur_store, new_drinfolist);
            cur_store = next.out;
          }
        cur_store = confine_expressions(cs, cur_store, new_drinfolist);
        for (stmt = (cs->stmts); stmt; stmt = ({ast_generic tEmPcast = (ast_generic)(stmt->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "flow.c", 888, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); }))
          {
            sinfo next = analyze_statement(stmt, cur_store, new_drinfolist);
            cur_store = next.out;
          }
        cur_store = merge_restricts(cs->decls, cur_store);
        cur_store = merge_confines(cs, cur_store);
        out = make_store_filter(cs->loc, cur_store, cs->filter_env);
        return mksinfo(out);
      };
      break;
    case kind_if_stmt:
      {
        if_stmt is = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_if_stmt && (tEmPcast)->kind <= postkind_if_stmt)", "flow.c", 912, __PRETTY_FUNCTION__), 0))); (if_stmt)(tEmPcast); });
        einfo guard;
        sinfo then, els;
        store result;
        guard = analyze_expression(is->condition, cpos, in, drinfolist);
        then = analyze_statement(is->stmt1, true_out_einfo(guard), drinfolist);
        if (is->stmt2)
          {
            els = analyze_statement(is->stmt2, false_out_einfo(guard),
                                    drinfolist);
            result = lub_store(is->loc,
                               "inconsistent then and else branch stores",
                               els.out, then.out);
          }
        else
          {
            result = lub_store(is->loc,
                               "inconsistent then and fall-through stores",
                               then.out, false_out_einfo(guard));
          }
        return mksinfo(result);
      };
      break;
    case kind_labeled_stmt:
      {
        labeled_stmt ls = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_labeled_stmt && (tEmPcast)->kind <= postkind_labeled_stmt)", "flow.c", 941, __PRETTY_FUNCTION__), 0))); (labeled_stmt)(tEmPcast); });
        sinfo body;
        if (ls->label->store_in)
          mkleq_store(ls->loc,
                      "inconsistent stores at label entry",
                      in, ls->label->store_in);
        else
          {
            ls->label->store_in = make_store_var(ls->loc, "label");
            mkleq_store(ls->loc,
                        "inconsitent stores at label entry",
                        in, ls->label->store_in);
          }
        body = analyze_statement(ls->stmt, ls->label->store_in, drinfolist);
        return body;
      };
      break;
    case kind_expression_stmt:
      {
        expression_stmt es = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)", "flow.c", 961, __PRETTY_FUNCTION__), 0))); (expression_stmt)(tEmPcast); });
        einfo ei;
        ei = analyze_expression(es->arg1, rpos, in, drinfolist);
        return mksinfo(ei.out);
      };
      break;
    case kind_while_stmt:
      {
        while_stmt ws = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_while_stmt && (tEmPcast)->kind <= postkind_while_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_while_stmt && (tEmPcast)->kind <= postkind_while_stmt)", "flow.c", 970, __PRETTY_FUNCTION__), 0))); (while_stmt)(tEmPcast); });
        einfo guard;
        sinfo body;
        ws->break_dest = make_store_var(ws->loc, "ws_break");
        ws->continue_dest = make_store_var(ws->loc, "ws_continue");
        mkleq_store(ws->loc, "inconsitent stores at loop entry", in,
                    ws->continue_dest);
        guard = analyze_expression(ws->condition, rpos, ws->continue_dest,
                                   drinfolist);
        body = analyze_statement(ws->stmt, guard.out, drinfolist);
        mkleq_store(ws->loc, "inconsistent stores at loop body exit",
                    body.out, ws->continue_dest);
        if (!definite_one(ws->condition))
          mkleq_store(ws->loc, "inconsistent stores at loop exit",
                      guard.out, ws->break_dest);
        return mksinfo(ws->break_dest);
      };
      break;
    case kind_dowhile_stmt:
      {
        dowhile_stmt dws = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_dowhile_stmt && (tEmPcast)->kind <= postkind_dowhile_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_dowhile_stmt && (tEmPcast)->kind <= postkind_dowhile_stmt)", "flow.c", 997, __PRETTY_FUNCTION__), 0))); (dowhile_stmt)(tEmPcast); });
        einfo guard;
        sinfo body;
        store before_body_store;
        dws->break_dest = make_store_var(dws->loc, "dws_break");
        dws->continue_dest = make_store_var(dws->loc, "dws_continue");
        before_body_store = make_store_var(dws->loc, "dws_body");
        mkleq_store(dws->loc, "inconsistent stores at loop entry",
                    in, before_body_store);
        body = analyze_statement(dws->stmt, before_body_store, drinfolist);
        mkleq_store(dws->loc, "inconsistent stores at loop body entry",
                    body.out, dws->continue_dest);
        if (definite_zero(dws->condition))
          mkleq_store(dws->loc, "inconsistent stores at loop exit",
                      dws->continue_dest, dws->break_dest);
        else if (definite_one(dws->condition))
          {
            guard = analyze_expression(dws->condition, rpos,
                                       dws->continue_dest, drinfolist);
            mkleq_store(dws->loc, "inconsistent stores at loop body entry",
                        guard.out, before_body_store);
          }
        else
          {
            guard = analyze_expression(dws->condition, rpos,
                                       dws->continue_dest, drinfolist);
            mkleq_store(dws->loc, "inconsistent stores at loop body entry",
                        guard.out, before_body_store);
            mkleq_store(dws->loc, "inconsistent stores at loop exit",
                        guard.out, dws->break_dest);
          }
        return mksinfo(dws->break_dest);
      };
      break;
    case kind_switch_stmt:
      {
        switch_stmt ss = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_switch_stmt && (tEmPcast)->kind <= postkind_switch_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_switch_stmt && (tEmPcast)->kind <= postkind_switch_stmt)", "flow.c", 1045, __PRETTY_FUNCTION__), 0))); (switch_stmt)(tEmPcast); });
        einfo guard;
        sinfo body;
        label cases;
        bool has_default;
        ss->break_dest = make_store_var(ss->loc, "break_dest");
        guard = analyze_expression(ss->condition, rpos, in, drinfolist);
        has_default = 0;
        for (cases = ss->next_label; cases; cases = cases->next_label)
          {
            switch (cases->kind)
              {
              case kind_default_label:
                has_default = 1;
              case kind_case_label:
                {
                  store temp;
                  cases->store_in = make_store_var(cases->loc, "ss_case");
                  ((void) ((cases->enclosing_cs) ? 0 : (__assert_fail ("cases->enclosing_cs", "flow.c", 1066, __PRETTY_FUNCTION__), 0)));
                  temp = tfr_jumped_blocks(guard.out, ss->enclosing_cs,
                                           cases->enclosing_cs);
                  mkleq_store(ss->loc, "inconsistent stores at case entry",
                              temp, cases->store_in);
                }
                break;
              default:
                __fail("flow.c", 1074, __FUNCTION__, "Unexpected label kind %x in switch\n", cases->kind);
              }
          }
        body = analyze_statement(ss->stmt,
                                 make_store_var(ss->loc, "ss_start"),
                                 drinfolist);
        mkleq_store(ss->loc, "inconsistent stores at body exit",
                    body.out, ss->break_dest);
        if (!has_default)
          mkleq_store(ss->loc, "inconsistent stores at body exit",
                      guard.out, ss->break_dest);
        return mksinfo(ss->break_dest);
      };
      break;
    case kind_for_stmt:
      {
        for_stmt fs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_for_stmt && (tEmPcast)->kind <= postkind_for_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_for_stmt && (tEmPcast)->kind <= postkind_for_stmt)", "flow.c", 1096, __PRETTY_FUNCTION__), 0))); (for_stmt)(tEmPcast); });
        sinfo body;
        store cur_store;
        fs->break_dest = make_store_var(fs->loc, "fs_break");
        fs->continue_dest = make_store_var(fs->loc, "fs_continue");
        cur_store = in;
        if (fs->arg1)
          {
            einfo arg1;
            arg1 = analyze_expression(fs->arg1, rpos, cur_store, drinfolist);
            cur_store = arg1.out;
          }
        mkleq_store(fs->loc, "inconsistent stores at loop entry",
                    cur_store, fs->continue_dest);
        cur_store = fs->continue_dest;
        if (fs->arg2)
          {
            einfo arg2;
            arg2 = analyze_expression(fs->arg2, rpos, cur_store, drinfolist);
            cur_store = arg2.out;
            mkleq_store(fs->loc, "inconsistent stores at loop exit",
                        cur_store, fs->break_dest);
          }
        body = analyze_statement(fs->stmt, cur_store, drinfolist);
        cur_store = body.out;
        if (fs->arg3)
          {
            einfo arg3;
            arg3 = analyze_expression(fs->arg3, rpos, cur_store, drinfolist);
            cur_store = arg3.out;
          }
        mkleq_store(fs->loc, "inconsistent stores at loop entry",
                    cur_store, fs->continue_dest);
        return mksinfo(fs->break_dest);
      };
      break;
    case kind_return_stmt:
      {
        return_stmt rs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_return_stmt && (tEmPcast)->kind <= postkind_return_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_return_stmt && (tEmPcast)->kind <= postkind_return_stmt)", "flow.c", 1145, __PRETTY_FUNCTION__), 0))); (return_stmt)(tEmPcast); });
        qtype fun_qtype;
        store cur_store;
        ((void) ((current_function_decl) ? 0 : (__assert_fail ("current_function_decl", "flow.c", 1149, __PRETTY_FUNCTION__), 0)));
        fun_qtype = root_ddecl(current_function_decl->ddecl)->fs_qtype;
        cur_store = in;
        if (rs->arg1)
          {
            einfo arg1;
            arg1 = analyze_expression(rs->arg1, rpos, in, drinfolist);
            cur_store = arg1.out;
            if (!qtype_error(fun_qtype) &&
                mkleq_fs_assign_convert(rs->loc, arg1.qt,
                                        return_qtype(fun_qtype), arg1.out))
              report_qerror(rs->loc, sev_err,
                            "return type incompatible with function type");
          }
        cur_store = tfr_jumped_blocks(cur_store, rs->enclosing_cs, ((void *)0));
        cur_store = make_store_filter(rs->loc,
                                      cur_store,
                                      current_function_decl->scope_env);
        if (!qtype_error(fun_qtype))
          mkleq_store(rs->loc, "inconsistent stores at function exit",
                      cur_store, store_out_qtype(fun_qtype));
        return mksinfo(make_store_var(rs->loc, "return"));
      };
      break;
    case kind_computed_goto_stmt:
      {
        computed_goto_stmt cgs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_computed_goto_stmt && (tEmPcast)->kind <= postkind_computed_goto_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_computed_goto_stmt && (tEmPcast)->kind <= postkind_computed_goto_stmt)", "flow.c", 1187, __PRETTY_FUNCTION__), 0))); (computed_goto_stmt)(tEmPcast); });
        einfo arg1;
        arg1 = analyze_expression(cgs->arg1, rpos, in,
                                  drinfolist);
        return mksinfo(make_store_var(cgs->loc, "computed_goto"));
      };
      break;
    case kind_break_stmt:
      {
        break_stmt bs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_break_stmt && (tEmPcast)->kind <= postkind_break_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_break_stmt && (tEmPcast)->kind <= postkind_break_stmt)", "flow.c", 1197, __PRETTY_FUNCTION__), 0))); (break_stmt)(tEmPcast); });
        store cur_store;
        cur_store = tfr_jumped_blocks(in, bs->enclosing_cs,
                                      bs->parent_loop->enclosing_cs);
        cur_store = make_store_filter(bs->loc, cur_store,
                                   bs->parent_loop->enclosing_cs->scope_env);
        mkleq_store(bs->loc,
                    (bs->parent_loop->kind == kind_switch_stmt ?
                     "inconsistent stores at switch exit" :
                     "inconsistent stores at loop exit"),
                    cur_store, bs->parent_loop->break_dest);
        return mksinfo(make_store_var(bs->loc, "break_stmt"));
      }
      break;
    case kind_continue_stmt:
      {
        continue_stmt cs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_continue_stmt && (tEmPcast)->kind <= postkind_continue_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_continue_stmt && (tEmPcast)->kind <= postkind_continue_stmt)", "flow.c", 1216, __PRETTY_FUNCTION__), 0))); (continue_stmt)(tEmPcast); });
        store cur_store;
        cur_store = tfr_jumped_blocks(in, cs->enclosing_cs,
                                      cs->parent_loop->enclosing_cs);
        cur_store = make_store_filter(cs->loc, cur_store,
                                   cs->parent_loop->enclosing_cs->scope_env);
        mkleq_store(cs->loc,
                    (cs->parent_loop->kind == kind_switch_stmt ?
                     "inconsistent stores at switch exit" :
                     "inconsistent stores at loop exit"),
                    cur_store, cs->parent_loop->continue_dest);
        return mksinfo(make_store_var(cs->loc, "continue_stmt"));
      }
      break;
    case kind_goto_stmt:
      {
        goto_stmt gs = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_goto_stmt && (tEmPcast)->kind <= postkind_goto_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_goto_stmt && (tEmPcast)->kind <= postkind_goto_stmt)", "flow.c", 1234, __PRETTY_FUNCTION__), 0))); (goto_stmt)(tEmPcast); });
        label_declaration ldecl;
        id_label target;
        store cur_store;
        ldecl = gs->id_label->ldecl;
        target = ldecl->definition;
        if (!target->store_in)
          target->store_in = make_store_var(target->loc, "label");
        cur_store = tfr_jumped_blocks(in, gs->enclosing_cs,
                                      target->enclosing_cs);
        cur_store = make_store_filter(gs->loc, cur_store,
                                      target->enclosing_cs->scope_env);
        mkleq_store(gs->loc, "inconsistent stores at unconditional branch",
                    cur_store, target->store_in);
        return mksinfo(make_store_var(gs->loc, "goto_stmt"));
      }
      break;
    case kind_empty_stmt:
      return mksinfo(in);
    case kind_change_type_stmt:
      {
        change_type_stmt ct = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_change_type_stmt && (tEmPcast)->kind <= postkind_change_type_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_change_type_stmt && (tEmPcast)->kind <= postkind_change_type_stmt)", "flow.c", 1257, __PRETTY_FUNCTION__), 0))); (change_type_stmt)(tEmPcast); });
        einfo arg1;
        aloc al;
        store out;
        qtype new_qt;
        arg1 = analyze_expression(ct->arg1, lpos, in, drinfolist);
        out = arg1.out;
        al = aloc_qtype(arg1.qt);
        new_qt = qtype_to_fs_qtype_with_quals(ct->loc, ct->qtype, out);
        out = assign_flow_sensitive(ct->loc,
                                    "incompatible types in change_type",
                                    new_qt, arg1.qt, out, 1);
        if (flag_pam_mode || flag_print_results)
          pam_add_overlay_lin(ct->loc, out, al);
        if (flag_print_lin)
          queue_print_stmt((statement) s, out, al);
      return mksinfo(out);
      }
      break;
    case kind_assert_type_stmt:
      {
        assert_type_stmt at = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_assert_type_stmt && (tEmPcast)->kind <= postkind_assert_type_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_assert_type_stmt && (tEmPcast)->kind <= postkind_assert_type_stmt)", "flow.c", 1282, __PRETTY_FUNCTION__), 0))); (assert_type_stmt)(tEmPcast); });
        einfo arg1;
        qtype assert_qt;
        arg1 = analyze_expression(at->arg1, rpos, in, drinfolist);
        assert_qt = qtype_to_fs_qtype_with_quals(at->loc, at->qtype, in);
        if (mkleq_fs_qtype(at->loc, arg1.qt, assert_qt, in))
          report_qerror(at->loc, sev_err,
                        "unsatisfiable assert_type");
        dd_add_first(parse_region, assert_type_qtypes, arg1.qt);
        dd_add_first(parse_region, assert_type_qtypes_hax, at->loc);
        return mksinfo(arg1.out);
      }
      break;
    case kind_deep_restrict_stmt:
      {
        deep_restrict_stmt dr = ({ast_generic tEmPcast = (ast_generic)(s); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_deep_restrict_stmt && (tEmPcast)->kind <= postkind_deep_restrict_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_deep_restrict_stmt && (tEmPcast)->kind <= postkind_deep_restrict_stmt)", "flow.c", 1299, __PRETTY_FUNCTION__), 0))); (deep_restrict_stmt)(tEmPcast); });
        dd_list new_drinfolist;
        einfo arg1;
        sinfo stmt;
        store cur_store;
        bool result;
        qtype rhs;
        ((void) ((dr->drinfo) ? 0 : (__assert_fail ("dr->drinfo", "flow.c", 1307, __PRETTY_FUNCTION__), 0)));
        arg1 = analyze_expression(dr->arg1, lpos, in, drinfolist);
        cur_store = arg1.out;
        dr->drinfo->fs_qtype = arg1.qt;
        cur_store = ref_qtype_store(dr->loc, dr->drinfo->rqtype, cur_store);
        ((void) ((!dr->drinfo->inference) ? 0 : (__assert_fail ("!dr->drinfo->inference", "flow.c", 1317, __PRETTY_FUNCTION__), 0)));
        ((void) ((!dr->drinfo->failed) ? 0 : (__assert_fail ("!dr->drinfo->failed", "flow.c", 1318, __PRETTY_FUNCTION__), 0)));
        dr->drinfo->fs_rqtype =
          qtype_to_fs_qtype(dr->loc, dr->drinfo->rqtype);
        result = mkleq_qual(dr->loc, &fs_qgate, qual_qtype(dr->drinfo->fs_qtype),
                            qual_qtype(dr->drinfo->fs_rqtype));
        ((void) ((qtype_pointer_loc(dr->drinfo->fs_rqtype)) ? 0 : (__assert_fail ("qtype_pointer_loc(dr->drinfo->fs_rqtype)", "flow.c", 1329, __PRETTY_FUNCTION__), 0)));
        ((void) ((qtype_pointer_loc(dr->drinfo->fs_qtype)) ? 0 : (__assert_fail ("qtype_pointer_loc(dr->drinfo->fs_qtype)", "flow.c", 1330, __PRETTY_FUNCTION__), 0)));
        rhs = points_to_fs_qtype(dr->drinfo->fs_qtype, cur_store);
        cur_store = assign_flow_sensitive(dr->loc,
                                          "incompatible right-hand side in "
                                          "confined location initialization",
                                          rhs,
                                          dr->drinfo->fs_rqtype,
                                          cur_store, 0);
        if (result)
          report_qerror(dr->loc, sev_err,
                        "incompatible right-hand side in deeprestricted "
                        "location initialization");
        new_drinfolist = dd_copy(parse_region, drinfolist);
        dd_add_first(parse_region, new_drinfolist, dr->drinfo);
        stmt = analyze_statement(dr->stmt, cur_store, new_drinfolist);
        cur_store = stmt.out;
        {
          qtype rqtype, fqtype;
          fqtype = dr->drinfo->fs_qtype;
          rqtype = points_to_fs_qtype(dr->drinfo->fs_rqtype, in);
          cur_store = assign_flow_sensitive(dr->loc,
                           "incompatible types at deep-restrict scope end",
                                     rqtype, fqtype, cur_store, 0);
        }
        return mksinfo(cur_store);
      }
    break;
    default:
      __fail("flow.c", 1369, __FUNCTION__, "Unexpected statement kind %x\n", s->kind);
    }
}
static einfo analyze_expression(expression e, context context, store in,
                                dd_list drinfolist)
{
  {
    dd_list_pos cur;
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
        qtype qt = curdri->fs_rqtype;
        ((void) ((!curdri->failed) ? 0 : (__assert_fail ("!curdri->failed", "flow.c", 1404, __PRETTY_FUNCTION__), 0)));
        if (context == rpos)
          qt = points_to_fs_qtype(qt, in);
        pam_add_overlay_confined_exp(e->loc, e, (gqtype) qt, in);
        return mkeinfo(qt, in, ((void *)0), ((void *)0), 0);
      }
  }
  switch(e->kind) {
  case kind_comma:
    {
      comma c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_comma && (tEmPcast)->kind <= postkind_comma)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_comma && (tEmPcast)->kind <= postkind_comma)", "flow.c", 1420, __PRETTY_FUNCTION__), 0))); (comma)(tEmPcast); });
      einfo last;
      expression e2;
      last = mkeinfo(((void *)0), in, ((void *)0), ((void *)0), 0);
      for (e2 = (c->arg1); e2; e2 = ({ast_generic tEmPcast = (ast_generic)(e2->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 1425, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        last = analyze_expression(e2, context, last.out, drinfolist);
      return last;
    };
    break;
  case kind_sizeof_type:
  case kind_alignof_type:
  case kind_lexical_cst:
  case kind_sizeof_expr:
  case kind_alignof_expr:
    {
      qtype qt;
      qt = qtype_to_fs_qtype_with_quals(e->loc, e->qtype, in);
      return mkeinfo(qt, in, ((void *)0), ((void *)0), 0);
    }
    break;
  case kind_label_address:
  case kind_string:
    {
      qtype qt;
      qt = qtype_to_fs_qtype_with_quals(e->loc, e->qtype, in);
      global_store_ext = allocate_qtype(e->loc, e->qtype, global_store_ext);
      return mkeinfo(qt, in, ((void *)0), ((void *)0), 0);
    }
    break;
  case kind_cast:
    {
      cast c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_cast && (tEmPcast)->kind <= postkind_cast)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_cast && (tEmPcast)->kind <= postkind_cast)", "flow.c", 1455, __PRETTY_FUNCTION__), 0))); (cast)(tEmPcast); });
      einfo arg1;
      type cast_t;
      qtype qt;
      store cur_store;
      arg1 = analyze_expression(c->arg1, context, in, drinfolist);
      cur_store = arg1.out;
      cast_t = c->asttype->type;
      qt = qtype_to_fs_qtype_with_quals(c->loc, e->qtype, in);
      if (arg1.ismalloc)
        cur_store = allocate_qtype(c->loc, e->qtype, cur_store);
      if (flag_casts_warn && !arg1.ismalloc && !type_contains_quals(cast_t))
        {
          mk_no_qual_qtype_fs(c->loc, arg1.qt, cur_store);
          mk_no_qual_qtype_fs(c->loc, qt, cur_store);
        }
      return mkeinfo(qt, cur_store, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_cast_list:
    {
      cast_list cl = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_cast_list && (tEmPcast)->kind <= postkind_cast_list)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_cast_list && (tEmPcast)->kind <= postkind_cast_list)", "flow.c", 1502, __PRETTY_FUNCTION__), 0))); (cast_list)(tEmPcast); });
      qtype qt;
      sinfo init;
      qt = qtype_to_fs_qtype_with_quals(cl->loc, cl->astqtype, in);
      init = analyze_init(cl->loc, qt, cl->init_expr, in, drinfolist);
      qt = default_conversion_fs_qtype(cl->loc, qt);
      return mkeinfo(qt, init.out, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_conditional:
    {
      conditional c = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_conditional && (tEmPcast)->kind <= postkind_conditional)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_conditional && (tEmPcast)->kind <= postkind_conditional)", "flow.c", 1515, __PRETTY_FUNCTION__), 0))); (conditional)(tEmPcast); });
      einfo cond, arg1, arg2;
      store out;
      qtype fs_qt;
      cond = analyze_expression(c->condition, rpos, in, drinfolist);
      if (c->arg1)
        arg1 = analyze_expression(c->arg1,
                                  context == cpos ? rpos : context,
                                  cond.out, drinfolist);
      else
        arg1 = cond;
      arg2 = analyze_expression(c->arg2,
                                context == cpos ? rpos : context,
                                cond.out, drinfolist);
      out = lub_store(c->loc, "inconsistent ? and : stores",
                      arg1.out, arg2.out);
      fs_qt = qtype_to_fs_qtype(c->loc, c->qtype);
      if (qtype_void(arg1.qt))
        mkleq_fs_assign_convert(c->loc, arg1.qt, fs_qt, out);
      else if (qtype_void(arg2.qt))
        mkleq_fs_assign_convert(c->loc, arg2.qt, fs_qt, out);
      else if (mkleq_fs_assign_convert(c->loc, arg1.qt, fs_qt, out) ||
               mkleq_fs_assign_convert(c->loc, arg2.qt, fs_qt, out))
        report_qerror(c->loc, sev_err,
                      "incompatible branches of conditional");
      return mkeinfo(fs_qt, out, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_identifier:
    {
      identifier id = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)", "flow.c", 1551, __PRETTY_FUNCTION__), 0))); (identifier)(tEmPcast); });
      data_declaration root;
      qtype qt;
      root = root_ddecl(id->ddecl);
      qt = root->fs_qtype;
      if (!qt)
        {
          add_ddecl_fs_qtype(root, in);
          qt = root->fs_qtype;
        }
      if ((type_function(id->ddecl->type) &&
          id->ddecl->ftype == function_implicit) ||
         !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__func__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__func__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__func__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__func__") && ((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__func__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__func__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__func__") && ((size_t)(const void *)(("__func__") + 1) - (size_t)(const void *)("__func__") == 1) && (__s2_len = strlen ("__func__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__func__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__func__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__func__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__func__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__func__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__func__")))); }) ||
         !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__FUNCTION__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__FUNCTION__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__FUNCTION__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__FUNCTION__") && ((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__FUNCTION__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__FUNCTION__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__FUNCTION__") && ((size_t)(const void *)(("__FUNCTION__") + 1) - (size_t)(const void *)("__FUNCTION__") == 1) && (__s2_len = strlen ("__FUNCTION__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__FUNCTION__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__FUNCTION__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__FUNCTION__")))); }) ||
         !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->ddecl->name) && __builtin_constant_p ("__PRETTY_FUNCTION__") && (__s1_len = strlen (id->ddecl->name), __s2_len = strlen ("__PRETTY_FUNCTION__"), (!((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->ddecl->name), (__const char *) ("__PRETTY_FUNCTION__"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) && (__s1_len = strlen (id->ddecl->name), __s1_len < 4) ? (__builtin_constant_p ("__PRETTY_FUNCTION__") && ((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__PRETTY_FUNCTION__"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"); register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("__PRETTY_FUNCTION__") && ((size_t)(const void *)(("__PRETTY_FUNCTION__") + 1) - (size_t)(const void *)("__PRETTY_FUNCTION__") == 1) && (__s2_len = strlen ("__PRETTY_FUNCTION__"), __s2_len < 4) ? (__builtin_constant_p (id->ddecl->name) && ((size_t)(const void *)((id->ddecl->name) + 1) - (size_t)(const void *)(id->ddecl->name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[0] - ((__const unsigned char *) (__const char *)("__PRETTY_FUNCTION__"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->ddecl->name))[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->ddecl->name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("__PRETTY_FUNCTION__"))[3]); } } __result; }))) : strcmp (id->ddecl->name, "__PRETTY_FUNCTION__")))); }))
        global_store_ext = allocate_qtype(id->loc, qt, global_store_ext);
      if (flag_pam_mode || flag_print_results)
        pam_add_overlay_flow_sensitive(id->loc, root->name,(gqtype) qt, in);
      if (root->kind != decl_constant)
        return put_id_in_fs_context(id->loc, qt, context, in, id->aloc);
      else
        return mkeinfo(qt, in, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_compound_expr:
    {
      compound_expr ce = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_expr && (tEmPcast)->kind <= postkind_compound_expr)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_expr && (tEmPcast)->kind <= postkind_compound_expr)", "flow.c", 1582, __PRETTY_FUNCTION__), 0))); (compound_expr)(tEmPcast); });
      compound_stmt cs = ({ast_generic tEmPcast = (ast_generic)(ce->stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_compound_stmt && (tEmPcast)->kind <= postkind_compound_stmt)", "flow.c", 1583, __PRETTY_FUNCTION__), 0))); (compound_stmt)(tEmPcast); });
      statement cur_stmt;
      declaration d;
      qtype qt;
      store cur_store, out;
      dd_list new_drinfolist;
      new_drinfolist = dd_copy(parse_region, drinfolist);
      cs->drinfolist = new_drinfolist;
      ((void) ((!cs->id_labels) ? 0 : (__assert_fail ("!cs->id_labels", "flow.c", 1593, __PRETTY_FUNCTION__), 0)));
      cur_store = in;
      for (d = (cs->decls); d; d = ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 1599, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
        {
          dinfo next;
          ((void) ((d->kind != kind_asm_decl) ? 0 : (__assert_fail ("d->kind != kind_asm_decl", "flow.c", 1603, __PRETTY_FUNCTION__), 0)));
          next = analyze_declaration(d, cur_store, new_drinfolist);
          cur_store = next.out;
        }
      cur_store = confine_expressions(cs, cur_store, new_drinfolist);
      cur_stmt = cs->stmts;
      while (cur_stmt && cur_stmt->next)
        {
          sinfo next = analyze_statement(cur_stmt, cur_store, new_drinfolist);
          cur_store = next.out;
          cur_stmt = ({ast_generic tEmPcast = (ast_generic)(cur_stmt->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_statement && (tEmPcast)->kind <= postkind_statement)", "flow.c", 1617, __PRETTY_FUNCTION__), 0))); (statement)(tEmPcast); });
        }
      if (cur_stmt && ((cur_stmt)->kind >= kind_expression_stmt && (cur_stmt)->kind <= postkind_expression_stmt))
        {
          einfo next;
          next = analyze_expression(({ast_generic tEmPcast = (ast_generic)(cur_stmt); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression_stmt && (tEmPcast)->kind <= postkind_expression_stmt)", "flow.c", 1625, __PRETTY_FUNCTION__), 0))); (expression_stmt)(tEmPcast); })->arg1,
                                    context,
                                    cur_store, new_drinfolist);
          qt = next.qt;
          cur_store = next.out;
        }
      else
        {
          qt = mkqtype_void(make_qvar("compound", ce->loc, 0,0));
          if (cur_stmt)
            {
              sinfo next = analyze_statement(cur_stmt, cur_store,
                                             new_drinfolist);
              cur_store = next.out;
            }
        }
      cur_store = merge_restricts(cs->decls, cur_store);
      cur_store = merge_confines(cs, cur_store);
      out = make_store_filter(ce->loc, cur_store, ce->filter_env);
      return mkeinfo(qt, out, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_function_call:
    {
      function_call fc = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_function_call && (tEmPcast)->kind <= postkind_function_call)", "flow.c", 1661, __PRETTY_FUNCTION__), 0))); (function_call)(tEmPcast); });
      einfo f_info, magic_info;
      qtype fqt;
      qtypelist args;
      expression arg;
      qtypelist_scanner args_qs;
      int i;
      store cur_store, out;
      if (fc->va_arg_call)
        {
          qtype va_arg_fs_qtype;
          va_arg_fs_qtype = qtype_to_fs_qtype(fc->loc, fc->qtype);
          return mkeinfo(va_arg_fs_qtype, in, ((void *)0), ((void *)0), 0);
        }
      cur_store = in;
      f_info = analyze_expression(fc->arg1, rpos, cur_store, drinfolist);
      cur_store = f_info.out;
      fqt = ((void *)0);
      if (!qtype_error(f_info.qt))
        fqt = points_to_fs_qtype(f_info.qt, cur_store);
      if (qtype_error(f_info.qt) || qtype_error(fqt))
        {
          for (arg = (fc->args); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 1690, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
            {
              einfo arg_info;
              arg_info = analyze_expression(arg, rpos, cur_store,
                                            drinfolist);
              cur_store = arg_info.out;
            }
          return mkeinfo(error_qtype, cur_store, ((void *)0), ((void *)0), 0);
        }
      magic_info = analyze_magic_function_call(fc, fqt, cur_store, drinfolist);
      if (magic_info.out)
        return magic_info;
      args = arg_qtypes(fqt);
      qtypelist_scan(args, &args_qs);
      i = 1;
      for (arg = (fc->args); arg; arg = ({ast_generic tEmPcast = (ast_generic)(arg->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 1712, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
        {
          einfo arg_info;
          qtype formal_qt;
          arg_info = analyze_expression(arg, rpos, cur_store, drinfolist);
          cur_store = arg_info.out;
          formal_qt = qtypelist_next(&args_qs);
          if (formal_qt)
            {
              if (mkleq_fs_assign_convert(arg->loc, arg_info.qt, formal_qt,
                                          cur_store))
                report_qerror(arg->loc, sev_err,
                              "type of actual argument %d doesn't match "
                              "type of formal", i);
            }
          i++;
        }
      mkleq_store(fc->loc,
                  "inconsistent stores at function entry",
                  make_store_filter(fc->loc, cur_store, effect_qtype(fqt)),
                  store_in_qtype(fqt));
      if (noreturn_qual &&
          has_qual_qtype(points_to_qtype(fc->arg1->qtype), noreturn_qual))
        out = make_store_var(fc->loc, "noreturn");
      else
        {
          cur_store = make_store_ow(fc->loc, store_out_qtype(fqt), cur_store,
                                    effect_qtype(fqt));
          out = make_store_var(fc->loc, "func_out");
          mkleq_store(fc->loc, "inconsistent stores at function return",
                      cur_store, out);
        }
      return mkeinfo(return_qtype(fqt), out, ((void *)0), ((void *)0), 0);
    };
    break;
  case kind_array_ref:
    {
      array_ref ar = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_array_ref && (tEmPcast)->kind <= postkind_array_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_array_ref && (tEmPcast)->kind <= postkind_array_ref)", "flow.c", 1762, __PRETTY_FUNCTION__), 0))); (array_ref)(tEmPcast); });
      ((void) ((ar->alt) ? 0 : (__assert_fail ("ar->alt", "flow.c", 1764, __PRETTY_FUNCTION__), 0)));
      return analyze_expression(ar->alt, context, in, drinfolist);
    };
    break;
  case kind_field_ref:
    {
      field_ref fr = ({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_field_ref && (tEmPcast)->kind <= postkind_field_ref)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_field_ref && (tEmPcast)->kind <= postkind_field_ref)", "flow.c", 1770, __PRETTY_FUNCTION__), 0))); (field_ref)(tEmPcast); });
      einfo arg1;
      qtype field_qt;
      arg1 = analyze_expression(fr->arg1, context, in, drinfolist);
      if (qtype_error(arg1.qt))
        return arg1;
      {
        qtype qt, ag_qt;
        qt = arg1.qt;
        if (context == lpos || context == apos)
          ag_qt = points_to_fs_qtype(qt, arg1.out);
        else
          ag_qt = qt;
        if (qtype_error(ag_qt))
          field_qt = error_qtype;
        else
          {
            ((void) ((qtype_aggregate(ag_qt)) ? 0 : (__assert_fail ("qtype_aggregate(ag_qt)", "flow.c", 1793, __PRETTY_FUNCTION__), 0)));
            field_qt = field_qtype(ag_qt, fr->cstring);
          }
      }
      if (flag_pam_mode || flag_print_results)
        pam_add_overlay_flow_sensitive(fr->cstring_loc, fr->cstring.data,
                                       (gqtype)field_qt, arg1.out);
      return put_id_in_fs_context(fr->loc, field_qt, context, arg1.out, ((void *)0));
    };
    break;
  case kind_init_list:
  case kind_init_index:
  case kind_init_field:
    __fail("flow.c", 1808, __FUNCTION__, "Unexpected expression kind %d\n", e->kind);
    break;
  default:
    if (((e)->kind >= kind_unary && (e)->kind <= postkind_unary))
      return analyze_unary_expression(({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_unary && (tEmPcast)->kind <= postkind_unary)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_unary && (tEmPcast)->kind <= postkind_unary)", "flow.c", 1812, __PRETTY_FUNCTION__), 0))); (unary)(tEmPcast); }), context, in, drinfolist);
    else if (((e)->kind >= kind_binary && (e)->kind <= postkind_binary))
      return analyze_binary_expression(({ast_generic tEmPcast = (ast_generic)(e); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_binary && (tEmPcast)->kind <= postkind_binary)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_binary && (tEmPcast)->kind <= postkind_binary)", "flow.c", 1814, __PRETTY_FUNCTION__), 0))); (binary)(tEmPcast); }), context, in,
                                       drinfolist);
    else
      __fail("flow.c", 1817, __FUNCTION__, "Unexpected expression kind %x\n", e->kind);
  }
}
static einfo analyze_unary_expression(unary e, context context, store in,
                                      dd_list drinfolist)
{
  switch (e->kind)
    {
    case kind_dereference:
      {
        einfo arg1;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        if (qtype_error(arg1.qt))
          return arg1;
        ((void) ((qtype_pointer_loc(arg1.qt)) ? 0 : (__assert_fail ("qtype_pointer_loc(arg1.qt)", "flow.c", 1838, __PRETTY_FUNCTION__), 0)));
        if (context == lpos || context == apos)
            return mkeinfo(arg1.qt, arg1.out, ((void *)0), ((void *)0), 0);
        else if (context == rpos || context == cpos || context == apos)
          {
            qtype qt;
            ((void) ((!qtype_function(arg1.qt)) ? 0 : (__assert_fail ("!qtype_function(arg1.qt)", "flow.c", 1847, __PRETTY_FUNCTION__), 0)));
            qt = points_to_fs_qtype(arg1.qt, arg1.out);
            qt = default_conversion_fs_qtype(e->loc, qt);
            return mkeinfo(qt, arg1.out, ((void *)0), ((void *)0), 0);
          }
        else
          __fail("flow.c", 1854, __FUNCTION__, "Unexpected context %d\n", context);
      }
    case kind_address_of:
      {
        if (type_function(e->arg1->type))
          return analyze_expression(e->arg1, context, in, drinfolist);
        else
          return analyze_expression(e->arg1, apos, in, drinfolist);
      }
    case kind_extension_expr:
      return analyze_expression(e->arg1, context, in, drinfolist);
    case kind_realpart:
    case kind_imagpart:
      {
        einfo arg1;
        qtype qt;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        ((void) ((qtype_complex(arg1.qt)) ? 0 : (__assert_fail ("qtype_complex(arg1.qt)", "flow.c", 1880, __PRETTY_FUNCTION__), 0)));
        qt = complex_base_qtype(arg1.qt);
        return mkeinfo(qt, arg1.out, ((void *)0), ((void *)0), 0);
      }
    case kind_unary_plus:
      return analyze_expression(e->arg1, context, in, drinfolist);
    case kind_unary_minus:
    case kind_conjugate:
    case kind_bitnot:
      ((void) ((context == rpos || context == cpos) ? 0 : (__assert_fail ("context == rpos || context == cpos", "flow.c", 1893, __PRETTY_FUNCTION__), 0)));
      return analyze_expression(e->arg1, context, in, drinfolist);
    case kind_not:
      {
        einfo arg1;
        arg1 = analyze_expression(e->arg1, context, in, drinfolist);
        if (context == cpos)
          return mkeinfo(arg1.qt, arg1.out, arg1.false_out, arg1.true_out, 0);
        else
          return arg1;
      }
    case kind_preincrement:
    case kind_postincrement:
    case kind_predecrement:
    case kind_postdecrement:
      {
        einfo result;
        result = analyze_expression(e->arg1, lpos, in, drinfolist);
        result.qt = points_to_fs_qtype(result.qt, in);
        if (context == cpos)
          {
            result.true_out = result.out;
            result.false_out = result.out;
            result.out = ((void *)0);
          }
        return result;
      }
    default:
      __fail("flow.c", 1923, __FUNCTION__, "Unexpected unary expression kind %x\n", e->kind);
    }
}
static einfo analyze_binary_expression(binary e, context context, store in,
                                       dd_list drinfolist)
{
  ((void) ((context == rpos || context == cpos) ? 0 : (__assert_fail ("context == rpos || context == cpos", "flow.c", 1930, __PRETTY_FUNCTION__), 0)));
  switch (e->kind)
    {
    case kind_plus:
      {
        einfo arg1, arg2;
        qtype qt;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        if (qtype_pointer_loc(arg1.qt) && qtype_pointer_loc(arg2.qt))
          __user_error("flow.c", 1942, __FUNCTION__, "(at %s:%ld) " "Unexpected pointer addition\n", ((e->loc)->filename), ((e->loc)->lineno), 0);
        else if (qtype_pointer_loc(arg1.qt) && !qtype_pointer_loc(arg2.qt))
          qt = arg1.qt;
        else if (!qtype_pointer_loc(arg1.qt) && qtype_pointer_loc(arg2.qt))
          qt = arg2.qt;
        else
          {
            if (mkeq_qtype(e->loc, &fs_qgate, arg1.qt, arg2.qt))
              report_qerror(e->loc, sev_err,
                            "incompatible operands of ``%s''",
                            binary_op_name(e->kind));
            qt = arg1.qt;
          }
        return mkeinfo(qt, arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_minus:
      {
        einfo arg1, arg2;
        qtype qt;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        if (qtype_pointer_loc(arg1.qt) && qtype_pointer_loc(arg2.qt))
          qt = qtype_to_fs_qtype(e->loc,
                mkqtype_ptrdiff_t(make_qvar("__ptr diff", e->loc, 0,0)));
        else if (qtype_pointer_loc(arg1.qt) && !qtype_pointer_loc(arg2.qt))
          qt = arg1.qt;
        else if (!qtype_pointer_loc(arg1.qt) && qtype_pointer_loc(arg2.qt))
          qt = arg2.qt;
        else
          {
            if (mkeq_qtype(e->loc, &fs_qgate, arg1.qt, arg2.qt))
              report_qerror(e->loc, sev_err,
                            "incompatible operands of ``%s''",
                            binary_op_name(e->kind));
            qt = arg1.qt;
          }
        return mkeinfo(qt, arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_modulo:
    case kind_lshift:
    case kind_rshift:
      {
        einfo arg1, arg2;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        return mkeinfo(arg1.qt, arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_times:
    case kind_divide:
    case kind_bitand:
    case kind_bitor:
    case kind_bitxor:
      {
        einfo arg1, arg2;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        if (mkeq_qtype(e->loc, &fs_qgate, arg1.qt, arg2.qt))
          report_qerror(e->loc, sev_err,
                        "incompatible operands of ``%s''",
                        binary_op_name(e->kind));
        return mkeinfo(arg1.qt, arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_andand:
      {
        einfo arg1, arg2;
        qtype qt;
        arg1 = analyze_expression(e->arg1, cpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, context, true_out_einfo(arg1),
                                  drinfolist);
        qt = mkqtype_bool(make_qvar("__boolop", e->loc, 0,0));
        if (context == cpos)
          return mkeinfo(qt, ((void *)0),
                         true_out_einfo(arg2),
                         lub_store(e->loc,
                                   "inconsistent stores for false &&",
                                   false_out_einfo(arg1),
                                   false_out_einfo(arg2)),
                         0);
        else
          return mkeinfo(qt,
                         lub_store(e->loc,
                                   "inconsistent stores for &&",
                                   false_out_einfo(arg1),
                                   arg2.out),
                         ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_oror:
      {
        einfo arg1, arg2;
        qtype qt;
        arg1 = analyze_expression(e->arg1, cpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, context, false_out_einfo(arg1),
                                  drinfolist);
        qt = mkqtype_bool(make_qvar("__boolop", e->loc, 0,0));
        if (context == cpos)
          return mkeinfo(qt, ((void *)0),
                         lub_store(e->loc,
                                   "inconsistent stores for true ||",
                                   true_out_einfo(arg1),
                                   true_out_einfo(arg2)),
                         false_out_einfo(arg2),
                         0);
        else
          return mkeinfo(qt,
                         lub_store(e->loc,
                                   "inconsistent stores for ||",
                                   true_out_einfo(arg1),
                                   arg2.out),
                         ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_eq:
    case kind_ne:
      {
        qtype qt;
        qt = mkqtype_bool(make_qvar("__boolop", e->loc, 0,0));
        if (context == cpos && have_stream_quals &&
            ((file_pointer_qtype(e->arg1->qtype) && definite_null(e->arg2)) ||
             (definite_null(e->arg1) && file_pointer_qtype(e->arg2->qtype))))
          {
            einfo fp, null;
            if (file_pointer_qtype(e->arg1->qtype))
              {
                null = analyze_expression(e->arg2, rpos, in, drinfolist);
                fp = analyze_expression(e->arg1, cpos, null.out, drinfolist);
              }
            else
              {
                null = analyze_expression(e->arg1, rpos, in, drinfolist);
                fp = analyze_expression(e->arg2, cpos, null.out, drinfolist);
              }
            if (e->kind == kind_eq)
              return mkeinfo(qt, ((void *)0), false_out_einfo(fp),
                             true_out_einfo(fp), 0);
            else
              return mkeinfo(qt, ((void *)0), true_out_einfo(fp),
                             false_out_einfo(fp), 0);
          }
        else
          {
            einfo arg1, arg2;
            arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
            arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
            return mkeinfo(qt, arg2.out, ((void *)0), ((void *)0), 0);
          }
      }
      break;
    case kind_leq:
    case kind_geq:
    case kind_lt:
    case kind_gt:
      {
        einfo arg1, arg2;
        arg1 = analyze_expression(e->arg1, rpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        return mkeinfo(mkqtype_bool(make_qvar("__boolop", e->loc, 0,0)),
                       arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_modulo_assign:
    case kind_lshift_assign:
    case kind_rshift_assign:
      {
        einfo arg1, arg2;
        qtype qt;
        arg1 = analyze_expression(e->arg1, lpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        if (!qtype_error(arg1.qt))
          qt = points_to_fs_qtype(arg1.qt, arg2.out);
        else
          qt = error_qtype;
        return mkeinfo(qt, arg2.out, ((void *)0), ((void *)0), 0);
      }
      break;
    case kind_plus_assign:
    case kind_minus_assign:
    case kind_times_assign:
    case kind_divide_assign:
    case kind_bitand_assign:
    case kind_bitor_assign:
    case kind_bitxor_assign:
    case kind_assign:
      {
        einfo arg1, arg2;
        qtype qt;
        store out;
        arg1 = analyze_expression(e->arg1, lpos, in, drinfolist);
        arg2 = analyze_expression(e->arg2, rpos, arg1.out, drinfolist);
        out = arg2.out;
        if (arg2.ismalloc)
          {
            out = allocate_qtype(e->loc, points_to_qtype(e->arg1->qtype), out);
            if (flag_casts_warn)
              {
                qt = points_to_fs_qtype(arg1.qt, out);
                return mkeinfo(qt, out, ((void *)0), ((void *)0), 0);
              }
          }
        if (!qtype_error(arg1.qt))
          {
            qt = points_to_fs_qtype(arg1.qt, out);
            if ((e->kind == kind_plus_assign ||
                 e->kind == kind_minus_assign) &&
                qtype_pointer_loc(qt) && !qtype_pointer_loc(arg2.qt))
              return mkeinfo(qt, out, ((void *)0), ((void *)0), 0);
            out = assign_flow_sensitive(e->loc,
                                        "incompatible types in assignment",
                                        arg2.qt, arg1.qt, out, 0);
            if (flag_print_lin)
              queue_print_exp((expression) e, out, aloc_qtype(arg1.qt));
          }
        else
          qt = error_qtype;
        if (have_stream_quals && context == cpos &&
            file_pointer_fs_qtype(qt, out))
          return split_file_pointer_context(e->loc, qt, out);
        else
          return mkeinfo(qt, out, ((void *)0), ((void *)0), 0);
      }
      break;
    default:
      __fail("flow.c", 2216, __FUNCTION__, "(at %s:%ld) " "Unexpected binary op kind 0x%x\n", ((e->loc)->filename), ((e->loc)->lineno), e->kind);
    }
}
einfo put_id_in_fs_context(location loc, qtype qt, context c, store s, aloc al)
{
  if (qtype_error(qt))
                    ;
  else if (qtype_array(qt))
    {
      if (c == rpos || c == cpos)
        qt = default_conversion_fs_qtype(loc, qt);
      else if (c == apos)
        qt = mkqtype_pointer_loc(loc,
                                 make_fresh_qvar("q", loc),
                                 array_aloc_qtype(qt));
      else
        __fail("flow.c", 2239, __FUNCTION__, "Unexpected context %d\n", c);
    }
  else if (qtype_function(qt))
    {
      ((void) ((c == rpos || c == cpos) ? 0 : (__assert_fail ("c == rpos || c == cpos", "flow.c", 2243, __PRETTY_FUNCTION__), 0)));
      qt = mkqtype_pointer_loc(loc,
                               make_fresh_qvar("q", loc),
                               al ? al : aloc_qtype(qt) );
    }
  else if (c == lpos || c == apos)
                    ;
  else if (c == rpos || c == cpos)
    {
      if (qtype_pointer_loc(qt))
        qt = points_to_fs_qtype(qt, s);
      else
        __fail("flow.c", 2256, __FUNCTION__, "Unexpected qtype\n");
    }
  else
    __fail("flow.c", 2259, __FUNCTION__, "Unexpected context %d\n", c);
  if (have_stream_quals && c == cpos && file_pointer_fs_qtype(qt, s))
    return split_file_pointer_context(loc, qt, s);
  else
    return mkeinfo(qt, s, ((void *)0), ((void *)0), 0);
}
static store allocate_qtype(location loc, qtype qt, store in)
{
  return ref_qtype_store(loc, qt, in);
}
static compound_stmt get_enclosing_scope(compound_stmt cs1, compound_stmt cs2)
{
  compound_stmt temp;
  if (cs1 == cs2) return cs1;
  temp = cs1;
  while (temp != ((void *)0))
    {
      temp->visited = 1;
      temp = temp->enclosing_cs;
    }
  temp = cs2;
  while (temp != ((void *)0))
    {
      if (temp->visited == 1)
        {
          while (cs1 != ((void *)0))
            { cs1->visited = 0; cs1 = cs1->enclosing_cs; }
          while (cs2->visited)
            { cs2->visited = 0; cs2 = cs2->enclosing_cs; }
          return temp;
        }
      temp = temp->enclosing_cs;
    }
  while (cs1 != ((void *)0))
    { cs1->visited = 0; cs1 = cs1->enclosing_cs; }
  while (cs2 != ((void *)0) && cs2->visited)
    { cs2->visited = 0; cs2 = cs2->enclosing_cs; }
  return ((void *)0);
}
static store ref_decls(store s, declaration d)
{
  if (!d)
    return s;
  s = ref_decls(s, ({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 2332, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }));
  switch (d->kind)
    {
    case kind_asm_decl:
    case kind_function_decl:
    case kind_ellipsis_decl:
    case kind_oldidentifier_decl:
      return s;
    case kind_extension_decl:
      {
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "flow.c", 2342, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        ((void) ((ed->decl->next == ((void *)0)) ? 0 : (__assert_fail ("ed->decl->next == ((void *)0)", "flow.c", 2344, __PRETTY_FUNCTION__), 0)));
        return ref_decls(s, ed->decl);
      }
    case kind_data_decl:
      {
        data_decl dd = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "flow.c", 2349, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); });
        declaration decl;
        for (decl = (dd->decls); decl; decl = ({ast_generic tEmPcast = (ast_generic)(decl->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 2352, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          {
            variable_decl vd = ({ast_generic tEmPcast = (ast_generic)(decl); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "flow.c", 2354, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); });
            data_declaration root;
            ((void) ((!vd->asm_stmt) ? 0 : (__assert_fail ("!vd->asm_stmt", "flow.c", 2357, __PRETTY_FUNCTION__), 0)));
            root = root_ddecl(vd->ddecl);
            if (root->qtype &&
                root->definition &&
                root->kind == decl_variable &&
                !root->isexternalscope &&
                root->vtype != variable_static)
              s = allocate_qtype(location_declaration(root->ast),
                                 root->qtype,
                                 s);
          }
        return s;
      }
    default:
      __fail("flow.c", 2375, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind %x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static store tfr_jumped_blocks(store s, compound_stmt from, compound_stmt to)
{
  compound_stmt enclosing_cs;
  dd_list_pos cur;
  dd_list cs_list = dd_new_list(parse_region);
  dd_list new_drinfolist;
  enclosing_cs = get_enclosing_scope(from, to);
  while (enclosing_cs != to)
    {
      ((void) ((to) ? 0 : (__assert_fail ("to", "flow.c", 2401, __PRETTY_FUNCTION__), 0)));
      s = ref_decls(s, to->decls);
      dd_add_first(parse_region, cs_list, to);
      to = to->enclosing_cs;
    }
  if(enclosing_cs)
    {
      ((void) ((enclosing_cs->drinfolist) ? 0 : (__assert_fail ("enclosing_cs->drinfolist", "flow.c", 2415, __PRETTY_FUNCTION__), 0)));
      new_drinfolist = dd_copy(parse_region, enclosing_cs->drinfolist);
    }
  else
    new_drinfolist = dd_new_list(parse_region);
  for (cur = dd_first((cs_list)); !(!(cur)->next); cur = ((cur)->next))
    {
      compound_stmt cs = ((compound_stmt)((cur)->data));
      s = confine_expressions(cs, s, new_drinfolist);
    }
  while (enclosing_cs != from)
    {
      ((void) ((from) ? 0 : (__assert_fail ("from", "flow.c", 2429, __PRETTY_FUNCTION__), 0)));
      s = merge_restricts(from->decls, s);
      s = merge_confines(from, s);
      from = from->enclosing_cs;
    }
  return s;
}
static store allocate_ddecl(data_declaration ddecl, store s)
{
  location loc;
  ddecl = root_ddecl(ddecl);
  loc = location_declaration(ddecl->ast);
  if (ddecl->qtype && ddecl->definition && !ddecl->isallocated)
    {
      if (ddecl->kind == decl_variable)
        {
          if (ddecl->isexternalscope || ddecl->vtype == variable_static)
              global_store_ext =
                allocate_qtype(loc,
                               ddecl->qtype,
                               global_store_ext);
          else
            s = allocate_qtype(loc, ddecl->qtype, s);
        }
      else if (ddecl->kind == decl_function)
        global_store_ext = allocate_qtype(loc, ddecl->qtype,
                                          global_store_ext);
      ddecl->isallocated = 1;
    }
  return s;
}
static store merge_var_decl_restrict(variable_decl vd, store in)
{
  if (vd && vd->rinfo)
    {
      qtype rqtype, fqtype;
      fqtype = points_to_fs_qtype(vd->rinfo->fs_fqtype, in);
      rqtype = points_to_fs_qtype(vd->rinfo->fs_rqtype, in);
      rqtype = points_to_fs_qtype(rqtype, in);
      in = assign_flow_sensitive(vd->loc,
                                 "incompatible types at restrict scope end",
                                 rqtype, fqtype, in, 0);
      return in;
    }
  else
    return in;
}
static store merge_restricts(declaration d, store in)
{
  if (!d)
    return in;
  in = merge_restricts(({ast_generic tEmPcast = (ast_generic)(d->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 2506, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }), in);
  switch (d->kind)
    {
    case kind_asm_decl:
    case kind_function_decl:
    case kind_ellipsis_decl:
    case kind_oldidentifier_decl:
      return in;
    case kind_extension_decl:
      {
        extension_decl ed = ({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_extension_decl && (tEmPcast)->kind <= postkind_extension_decl)", "flow.c", 2516, __PRETTY_FUNCTION__), 0))); (extension_decl)(tEmPcast); });
        ((void) ((ed->decl->next == ((void *)0)) ? 0 : (__assert_fail ("ed->decl->next == ((void *)0)", "flow.c", 2518, __PRETTY_FUNCTION__), 0)));
        return merge_restricts(ed->decl, in);
      }
    case kind_data_decl:
      {
        declaration vd;
        for (vd = (({ast_generic tEmPcast = (ast_generic)(d); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_data_decl && (tEmPcast)->kind <= postkind_data_decl)", "flow.c", 2525, __PRETTY_FUNCTION__), 0))); (data_decl)(tEmPcast); })->decls); vd; vd = ({ast_generic tEmPcast = (ast_generic)(vd->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_declaration && (tEmPcast)->kind <= postkind_declaration)", "flow.c", 2525, __PRETTY_FUNCTION__), 0))); (declaration)(tEmPcast); }))
          in = merge_var_decl_restrict(({ast_generic tEmPcast = (ast_generic)(vd); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_variable_decl && (tEmPcast)->kind <= postkind_variable_decl)", "flow.c", 2526, __PRETTY_FUNCTION__), 0))); (variable_decl)(tEmPcast); }), in);
        return in;
      }
    default:
      __fail("flow.c", 2531, __FUNCTION__, "(at %s:%ld) " "Unexpected decl kind %x\n", ((d->loc)->filename), ((d->loc)->lineno), d->kind);
    }
}
static store confine_expressions(compound_stmt cs, store in, dd_list drinfolist)
{
  dd_list_pos cur;
  store cur_store = in;
  if (!cs->confine_expressions)
    return cur_store;
  for (cur = dd_first((cs->confine_expressions)); !(!(cur)->next); cur = ((cur)->next))
    {
      exprdrinfo ed = ((exprdrinfo)((cur)->data));
      if (ed->drinfo && ed->drinfo->inference && !ed->drinfo->failed)
        {
          bool result;
          einfo arg1;
          qtype rhs;
          arg1 = analyze_expression(ed->drinfo->expression, lpos, cur_store,
                                    drinfolist);
          cur_store = arg1.out;
          ed->drinfo->fs_qtype = arg1.qt;
          cur_store = ref_qtype_store(ed->drinfo->location,
                                      ed->drinfo->rqtype, cur_store);
          ed->drinfo->fs_rqtype =
            qtype_to_fs_qtype(ed->drinfo->location, ed->drinfo->rqtype);
          result = mkleq_qual(ed->drinfo->location,
                              &fs_qgate,
                              qual_qtype(ed->drinfo->fs_qtype),
                              qual_qtype(ed->drinfo->fs_rqtype));
          ((void) ((qtype_pointer_loc(ed->drinfo->fs_rqtype)) ? 0 : (__assert_fail ("qtype_pointer_loc(ed->drinfo->fs_rqtype)", "flow.c", 2577, __PRETTY_FUNCTION__), 0)));
          ((void) ((qtype_pointer_loc(ed->drinfo->fs_qtype)) ? 0 : (__assert_fail ("qtype_pointer_loc(ed->drinfo->fs_qtype)", "flow.c", 2578, __PRETTY_FUNCTION__), 0)));
          rhs = points_to_fs_qtype(ed->drinfo->fs_qtype, cur_store);
          cur_store = assign_flow_sensitive(ed->drinfo->location,
                                             "incompatible right-hand side in "
                                           "confined location initialization",
                                            rhs,
                                            ed->drinfo->fs_rqtype,
                                             cur_store, 0);
          if (result)
            report_qerror(ed->drinfo->location, sev_err,
                          "incompatible right-hand side in confined "
                          "location initialization");
          dd_add_first(parse_region, drinfolist, ed->drinfo);
        }
    }
  return cur_store;
}
static store merge_confines(compound_stmt cs, store in)
{
  dd_list_pos cur;
  store cur_store = in;
  if (!cs->confine_expressions)
    return cur_store;
  for (cur = dd_first((cs->confine_expressions)); !(!(cur)->next); cur = ((cur)->next))
    {
      exprdrinfo ed = ((exprdrinfo)((cur)->data));
      if (ed->drinfo && !ed->drinfo->failed)
        {
          qtype rqtype, fqtype;
          fqtype = ed->drinfo->fs_qtype;
          rqtype = points_to_fs_qtype(ed->drinfo->fs_rqtype, cur_store);
          cur_store = assign_flow_sensitive(ed->drinfo->location,
                           "incompatible types at confine scope end",
                                            rqtype, fqtype, cur_store, 0);
        }
    }
  return cur_store;
}
static qual string_mode_to_qual(const char *mode)
{
  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("r") && (__s1_len = strlen (mode), __s2_len = strlen ("r"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("r") + 1) - (size_t)(const void *)("r") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("r"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("r") && ((size_t)(const void *)(("r") + 1) - (size_t)(const void *)("r") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("r"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("r") && ((size_t)(const void *)(("r") + 1) - (size_t)(const void *)("r") == 1) && (__s2_len = strlen ("r"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("r"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("r"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("r"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("r"))[3]); } } __result; }))) : strcmp (mode, "r")))); }) ||
      !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("rb") && (__s1_len = strlen (mode), __s2_len = strlen ("rb"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("rb") + 1) - (size_t)(const void *)("rb") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("rb"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("rb") && ((size_t)(const void *)(("rb") + 1) - (size_t)(const void *)("rb") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("rb"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("rb"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("rb"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("rb"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("rb"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("rb") && ((size_t)(const void *)(("rb") + 1) - (size_t)(const void *)("rb") == 1) && (__s2_len = strlen ("rb"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("rb"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("rb"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("rb"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("rb"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("rb"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("rb"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("rb"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("rb"))[3]); } } __result; }))) : strcmp (mode, "rb")))); }))
    return read_unchecked_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("w") && (__s1_len = strlen (mode), __s2_len = strlen ("w"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("w"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("w") && ((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("w"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("w") && ((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) && (__s2_len = strlen ("w"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("w"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; }))) : strcmp (mode, "w")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("wb") && (__s1_len = strlen (mode), __s2_len = strlen ("wb"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("wb") + 1) - (size_t)(const void *)("wb") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("wb"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("wb") && ((size_t)(const void *)(("wb") + 1) - (size_t)(const void *)("wb") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("wb"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("wb"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("wb"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("wb"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("wb"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("wb") && ((size_t)(const void *)(("wb") + 1) - (size_t)(const void *)("wb") == 1) && (__s2_len = strlen ("wb"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("wb"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("wb"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("wb"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("wb"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("wb"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("wb"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("wb"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("wb"))[3]); } } __result; }))) : strcmp (mode, "wb")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("a") && (__s1_len = strlen (mode), __s2_len = strlen ("a"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("a") + 1) - (size_t)(const void *)("a") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("a"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("a") && ((size_t)(const void *)(("a") + 1) - (size_t)(const void *)("a") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("a"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("a") && ((size_t)(const void *)(("a") + 1) - (size_t)(const void *)("a") == 1) && (__s2_len = strlen ("a"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("a"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("a"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("a"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("a"))[3]); } } __result; }))) : strcmp (mode, "a")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("ab") && (__s1_len = strlen (mode), __s2_len = strlen ("ab"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("ab") + 1) - (size_t)(const void *)("ab") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("ab"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("ab") && ((size_t)(const void *)(("ab") + 1) - (size_t)(const void *)("ab") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("ab"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("ab"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("ab"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("ab"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("ab"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("ab") && ((size_t)(const void *)(("ab") + 1) - (size_t)(const void *)("ab") == 1) && (__s2_len = strlen ("ab"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("ab"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("ab"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("ab"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("ab"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("ab"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("ab"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("ab"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("ab"))[3]); } } __result; }))) : strcmp (mode, "ab")))); }))
    return write_unchecked_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("r+") && (__s1_len = strlen (mode), __s2_len = strlen ("r+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("r+") + 1) - (size_t)(const void *)("r+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("r+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("r+") && ((size_t)(const void *)(("r+") + 1) - (size_t)(const void *)("r+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("r+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("r+") && ((size_t)(const void *)(("r+") + 1) - (size_t)(const void *)("r+") == 1) && (__s2_len = strlen ("r+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("r+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("r+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("r+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("r+"))[3]); } } __result; }))) : strcmp (mode, "r+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("rb+") && (__s1_len = strlen (mode), __s2_len = strlen ("rb+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("rb+") + 1) - (size_t)(const void *)("rb+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("rb+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("rb+") && ((size_t)(const void *)(("rb+") + 1) - (size_t)(const void *)("rb+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("rb+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("rb+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("rb+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("rb+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("rb+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("rb+") && ((size_t)(const void *)(("rb+") + 1) - (size_t)(const void *)("rb+") == 1) && (__s2_len = strlen ("rb+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("rb+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("rb+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("rb+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("rb+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("rb+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("rb+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("rb+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("rb+"))[3]); } } __result; }))) : strcmp (mode, "rb+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("r+b") && (__s1_len = strlen (mode), __s2_len = strlen ("r+b"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("r+b") + 1) - (size_t)(const void *)("r+b") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("r+b"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("r+b") && ((size_t)(const void *)(("r+b") + 1) - (size_t)(const void *)("r+b") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r+b"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r+b"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r+b"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("r+b"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("r+b") && ((size_t)(const void *)(("r+b") + 1) - (size_t)(const void *)("r+b") == 1) && (__s2_len = strlen ("r+b"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("r+b"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("r+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("r+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("r+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("r+b"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("r+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("r+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("r+b"))[3]); } } __result; }))) : strcmp (mode, "r+b")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("w+") && (__s1_len = strlen (mode), __s2_len = strlen ("w+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("w+") + 1) - (size_t)(const void *)("w+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("w+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("w+") && ((size_t)(const void *)(("w+") + 1) - (size_t)(const void *)("w+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("w+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("w+") && ((size_t)(const void *)(("w+") + 1) - (size_t)(const void *)("w+") == 1) && (__s2_len = strlen ("w+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("w+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("w+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("w+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("w+"))[3]); } } __result; }))) : strcmp (mode, "w+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("wb+") && (__s1_len = strlen (mode), __s2_len = strlen ("wb+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("wb+") + 1) - (size_t)(const void *)("wb+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("wb+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("wb+") && ((size_t)(const void *)(("wb+") + 1) - (size_t)(const void *)("wb+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("wb+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("wb+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("wb+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("wb+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("wb+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("wb+") && ((size_t)(const void *)(("wb+") + 1) - (size_t)(const void *)("wb+") == 1) && (__s2_len = strlen ("wb+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("wb+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("wb+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("wb+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("wb+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("wb+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("wb+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("wb+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("wb+"))[3]); } } __result; }))) : strcmp (mode, "wb+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("w+b") && (__s1_len = strlen (mode), __s2_len = strlen ("w+b"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("w+b") + 1) - (size_t)(const void *)("w+b") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("w+b"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("w+b") && ((size_t)(const void *)(("w+b") + 1) - (size_t)(const void *)("w+b") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w+b"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w+b"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w+b"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("w+b"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("w+b") && ((size_t)(const void *)(("w+b") + 1) - (size_t)(const void *)("w+b") == 1) && (__s2_len = strlen ("w+b"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("w+b"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("w+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("w+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("w+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("w+b"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("w+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("w+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("w+b"))[3]); } } __result; }))) : strcmp (mode, "w+b")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("a+") && (__s1_len = strlen (mode), __s2_len = strlen ("a+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("a+") + 1) - (size_t)(const void *)("a+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("a+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("a+") && ((size_t)(const void *)(("a+") + 1) - (size_t)(const void *)("a+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("a+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("a+") && ((size_t)(const void *)(("a+") + 1) - (size_t)(const void *)("a+") == 1) && (__s2_len = strlen ("a+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("a+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("a+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("a+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("a+"))[3]); } } __result; }))) : strcmp (mode, "a+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("ab+") && (__s1_len = strlen (mode), __s2_len = strlen ("ab+"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("ab+") + 1) - (size_t)(const void *)("ab+") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("ab+"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("ab+") && ((size_t)(const void *)(("ab+") + 1) - (size_t)(const void *)("ab+") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("ab+"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("ab+"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("ab+"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("ab+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("ab+"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("ab+") && ((size_t)(const void *)(("ab+") + 1) - (size_t)(const void *)("ab+") == 1) && (__s2_len = strlen ("ab+"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("ab+"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("ab+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("ab+"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("ab+"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("ab+"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("ab+"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("ab+"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("ab+"))[3]); } } __result; }))) : strcmp (mode, "ab+")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (mode) && __builtin_constant_p ("a+b") && (__s1_len = strlen (mode), __s2_len = strlen ("a+b"), (!((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("a+b") + 1) - (size_t)(const void *)("a+b") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (mode), (__const char *) ("a+b"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) && (__s1_len = strlen (mode), __s1_len < 4) ? (__builtin_constant_p ("a+b") && ((size_t)(const void *)(("a+b") + 1) - (size_t)(const void *)("a+b") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a+b"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a+b"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a+b"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("a+b"); register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("a+b") && ((size_t)(const void *)(("a+b") + 1) - (size_t)(const void *)("a+b") == 1) && (__s2_len = strlen ("a+b"), __s2_len < 4) ? (__builtin_constant_p (mode) && ((size_t)(const void *)((mode) + 1) - (size_t)(const void *)(mode) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (mode))[0] - ((__const unsigned char *) (__const char *)("a+b"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[1] - ((__const unsigned char *) (__const char *) ("a+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (mode))[2] - ((__const unsigned char *) (__const char *) ("a+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (mode))[3] - ((__const unsigned char *) (__const char *) ("a+b"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (mode); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("a+b"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("a+b"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("a+b"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("a+b"))[3]); } } __result; }))) : strcmp (mode, "a+b")))); }))
    return readwrite_unchecked_qual;
  return ((void *)0);
}
static qual int_mode_to_qual(known_cst cst)
{
  unsigned int mode;
  mode = cval_uint_value((cst)->cval);
  switch (mode & 0003)
    {
    case 00: return read_unchecked_qual;
    case 01: return write_unchecked_qual;
    case 02: return readwrite_unchecked_qual;
    default: return ((void *)0);
    }
}
static einfo analyze_magic_function_call(function_call fc, qtype fqt, store in,
                                         dd_list drinfolist)
{
  if (fc->arg1->kind == kind_identifier)
    {
      identifier id = ({ast_generic tEmPcast = (ast_generic)(fc->arg1); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_identifier && (tEmPcast)->kind <= postkind_identifier)", "flow.c", 2684, __PRETTY_FUNCTION__), 0))); (identifier)(tEmPcast); });
      if ((!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) && (__s2_len = strlen ("fopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fdopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fdopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fdopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fdopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) && (__s2_len = strlen ("fdopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fdopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fdopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("tmpfile") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("tmpfile"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("tmpfile"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("tmpfile") && ((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("tmpfile"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("tmpfile"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("tmpfile") && ((size_t)(const void *)(("tmpfile") + 1) - (size_t)(const void *)("tmpfile") == 1) && (__s2_len = strlen ("tmpfile"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("tmpfile"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("tmpfile"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("tmpfile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("tmpfile"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("tmpfile"))[3]); } } __result; }))) : strcmp (id->cstring.data, "tmpfile")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("safefopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("safefopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("safefopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("safefopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) && (__s2_len = strlen ("safefopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("safefopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "safefopen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) && (__s2_len = strlen ("popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "popen")))); }) ||
           !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("my_popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("my_popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("my_popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("my_popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) && (__s2_len = strlen ("my_popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("my_popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "my_popen")))); }))
          && have_stream_quals)
        {
          store cur_store;
          qual mode_qual;
          expression e;
          qtype ret_qtype;
          int num_args;
          cur_store = in;
          mode_qual = ((void *)0);
          num_args = 0;
          for (e = (fc->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 2704, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
            {
              einfo arg_info = analyze_expression(e, rpos, cur_store,
                                                  drinfolist);
              cur_store = arg_info.out;
              num_args++;
            }
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fopen") && ((size_t)(const void *)(("fopen") + 1) - (size_t)(const void *)("fopen") == 1) && (__s2_len = strlen ("fopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fopen")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("fdopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("fdopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("fdopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("fdopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("fdopen") && ((size_t)(const void *)(("fdopen") + 1) - (size_t)(const void *)("fdopen") == 1) && (__s2_len = strlen ("fdopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("fdopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("fdopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("fdopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("fdopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("fdopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "fdopen")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) && (__s2_len = strlen ("popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "popen")))); }) ||
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("my_popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("my_popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("my_popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("my_popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("my_popen") && ((size_t)(const void *)(("my_popen") + 1) - (size_t)(const void *)("my_popen") == 1) && (__s2_len = strlen ("my_popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("my_popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("my_popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("my_popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("my_popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("my_popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "my_popen")))); }))
            {
              if (num_args != 2)
                report_qerror(fc->loc, sev_err,
                              "wrong number of arguments to fopen");
              if (num_args >= 2 && fc->args->next->kind == kind_string)
                mode_qual =
                  string_mode_to_qual(string_to_charp(parse_region,
                                                ({ast_generic tEmPcast = (ast_generic)(fc->args->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_string && (tEmPcast)->kind <= postkind_string)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_string && (tEmPcast)->kind <= postkind_string)", "flow.c", 2725, __PRETTY_FUNCTION__), 0))); (string)(tEmPcast); })));
              if (!mode_qual)
                {
                  mode_qual = open_unchecked_qual;
                  report_qerror(fc->args->next->loc, sev_info,
                                "unable to determine mode for fopen");
                }
              else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("popen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("popen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("popen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("popen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("popen") && ((size_t)(const void *)(("popen") + 1) - (size_t)(const void *)("popen") == 1) && (__s2_len = strlen ("popen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("popen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("popen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("popen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("popen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("popen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "popen")))); }) &&
                       mode_qual == readwrite_unchecked_qual)
                report_qerror(fc->args->next->loc, sev_err,
                              "pipes can only be opened for reading or "
                              "writing, not both");
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("safefopen") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("safefopen"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("safefopen"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("safefopen"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("safefopen") && ((size_t)(const void *)(("safefopen") + 1) - (size_t)(const void *)("safefopen") == 1) && (__s2_len = strlen ("safefopen"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("safefopen"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("safefopen"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("safefopen"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("safefopen"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("safefopen"))[3]); } } __result; }))) : strcmp (id->cstring.data, "safefopen")))); }))
            {
              if (num_args != 4)
                report_qerror(fc->loc, sev_err,
                              "wrong number of arguments to safefopen");
              if (num_args >= 2 && ({ast_generic tEmPcast = (ast_generic)(fc->args->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 2745, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); })->cst)
                mode_qual =
                  int_mode_to_qual(({ast_generic tEmPcast = (ast_generic)(fc->args->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 2747, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); })->cst);
              if (!mode_qual)
                {
                  mode_qual = open_unchecked_qual;
                  report_qerror(fc->args->next->loc, sev_info,
                                "unable to determine mode for fopen");
                }
            }
          else
            __fail("flow.c", 2756, __FUNCTION__, "Unexpected id %s\n", id->cstring.data);
          ret_qtype = qtype_to_fs_qtype(fc->loc, fc->qtype);
          if (mode_qual != readwrite_qual)
            {
              if (qtype_pointer_loc(ret_qtype))
                {
                  aloc al;
                  qtype ret_ptqtype;
                  al = aloc_qtype(ret_qtype);
                  ret_ptqtype = copy_qtype(points_to_fs_qtype(ret_qtype,
                                                              cur_store),
                                           fc->loc);
                  ((void) ((!(mkleq_qual(fc->loc, &fs_qgate, mode_qual, qual_qtype(ret_ptqtype)))) ? 0 : (__assert_fail ("!(mkleq_qual(fc->loc, &fs_qgate, mode_qual, qual_qtype(ret_ptqtype)))", "flow.c", 2775, __PRETTY_FUNCTION__), 0)));
                  cur_store =
                    make_store_ref(fc->loc, cur_store, al);
                  cur_store =
                    make_store_assign(fc->loc,
                                      "strong update failure in fopen result",
                                      cur_store, al, ret_ptqtype, 1);
                }
              else
                report_qerror(fc->loc, sev_err,
                              "return type of fopen isn't a pointer type");
            }
          return mkeinfo(ret_qtype, cur_store, ((void *)0), ((void *)0), 0);
        }
      else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("malloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("malloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("malloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("malloc") && ((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("malloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("malloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("malloc") && ((size_t)(const void *)(("malloc") + 1) - (size_t)(const void *)("malloc") == 1) && (__s2_len = strlen ("malloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("malloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("malloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("malloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("malloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("malloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "malloc")))); }) ||
               !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("xmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("xmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("xmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("xmalloc") && ((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("xmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("xmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("xmalloc") && ((size_t)(const void *)(("xmalloc") + 1) - (size_t)(const void *)("xmalloc") == 1) && (__s2_len = strlen ("xmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("xmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("xmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("xmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("xmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("xmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "xmalloc")))); }) ||
               !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("vmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("vmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("vmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("vmalloc") && ((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("vmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("vmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("vmalloc") && ((size_t)(const void *)(("vmalloc") + 1) - (size_t)(const void *)("vmalloc") == 1) && (__s2_len = strlen ("vmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("vmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("vmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("vmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("vmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("vmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "vmalloc")))); }) ||
               !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) && (__s2_len = strlen ("kmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmalloc")))); }) ||
               !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("ioremap") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("ioremap"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("ioremap"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("ioremap") && ((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("ioremap"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("ioremap"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("ioremap") && ((size_t)(const void *)(("ioremap") + 1) - (size_t)(const void *)("ioremap") == 1) && (__s2_len = strlen ("ioremap"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("ioremap"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("ioremap"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("ioremap"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("ioremap"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("ioremap"))[3]); } } __result; }))) : strcmp (id->cstring.data, "ioremap")))); }) ||
               !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmem_cache_alloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmem_cache_alloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmem_cache_alloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmem_cache_alloc") && ((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmem_cache_alloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmem_cache_alloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmem_cache_alloc") && ((size_t)(const void *)(("kmem_cache_alloc") + 1) - (size_t)(const void *)("kmem_cache_alloc") == 1) && (__s2_len = strlen ("kmem_cache_alloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmem_cache_alloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmem_cache_alloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmem_cache_alloc")))); }))
        {
          qtype ret_qtype;
          store cur_store;
          expression e;
          int num_args = 0;
          cur_store = in;
          for (e = (fc->args); e; e = ({ast_generic tEmPcast = (ast_generic)(e->next); if (tEmPcast) ((void) ((((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)) ? 0 : (__assert_fail ("((tEmPcast)->kind >= kind_expression && (tEmPcast)->kind <= postkind_expression)", "flow.c", 2808, __PRETTY_FUNCTION__), 0))); (expression)(tEmPcast); }))
            {
              einfo arg_info = analyze_expression(e, rpos, cur_store,
                                                  drinfolist);
              cur_store = arg_info.out;
              num_args++;
            }
          ((void) ((qtype_pointer(fc->qtype)) ? 0 : (__assert_fail ("qtype_pointer(fc->qtype)", "flow.c", 2816, __PRETTY_FUNCTION__), 0)));
          cur_store = allocate_qtype(fc->loc, fc->qtype, cur_store);
          ret_qtype = qtype_to_fs_qtype(fc->loc, fc->qtype);
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (id->cstring.data) && __builtin_constant_p ("kmalloc") && (__s1_len = strlen (id->cstring.data), __s2_len = strlen ("kmalloc"), (!((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (id->cstring.data), (__const char *) ("kmalloc"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) && (__s1_len = strlen (id->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("kmalloc"); register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("kmalloc") && ((size_t)(const void *)(("kmalloc") + 1) - (size_t)(const void *)("kmalloc") == 1) && (__s2_len = strlen ("kmalloc"), __s2_len < 4) ? (__builtin_constant_p (id->cstring.data) && ((size_t)(const void *)((id->cstring.data) + 1) - (size_t)(const void *)(id->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[0] - ((__const unsigned char *) (__const char *)("kmalloc"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (id->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (id->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("kmalloc"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("kmalloc"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("kmalloc"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("kmalloc"))[3]); } } __result; }))) : strcmp (id->cstring.data, "kmalloc")))); }) && have_sleepy_quals)
            {
              if (num_args != 2)
                report_qerror(fc->loc, sev_err,
                              "wrong number of arguments to kmalloc");
              else if (fc->args->next->kind == kind_lexical_cst)
                {
                  lexical_cst cst = (lexical_cst)(fc->args->next);
                  if(__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (cst->cstring.data) && __builtin_constant_p ("0x20") && (__s1_len = strlen (cst->cstring.data), __s2_len = strlen ("0x20"), (!((size_t)(const void *)((cst->cstring.data) + 1) - (size_t)(const void *)(cst->cstring.data) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("0x20") + 1) - (size_t)(const void *)("0x20") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (cst->cstring.data), (__const char *) ("0x20"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (cst->cstring.data) && ((size_t)(const void *)((cst->cstring.data) + 1) - (size_t)(const void *)(cst->cstring.data) == 1) && (__s1_len = strlen (cst->cstring.data), __s1_len < 4) ? (__builtin_constant_p ("0x20") && ((size_t)(const void *)(("0x20") + 1) - (size_t)(const void *)("0x20") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[0] - ((__const unsigned char *) (__const char *)("0x20"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("0x20"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("0x20"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("0x20"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("0x20"); register int __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("0x20") && ((size_t)(const void *)(("0x20") + 1) - (size_t)(const void *)("0x20") == 1) && (__s2_len = strlen ("0x20"), __s2_len < 4) ? (__builtin_constant_p (cst->cstring.data) && ((size_t)(const void *)((cst->cstring.data) + 1) - (size_t)(const void *)(cst->cstring.data) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[0] - ((__const unsigned char *) (__const char *)("0x20"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[1] - ((__const unsigned char *) (__const char *) ("0x20"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[2] - ((__const unsigned char *) (__const char *) ("0x20"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (cst->cstring.data))[3] - ((__const unsigned char *) (__const char *) ("0x20"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (cst->cstring.data); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("0x20"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("0x20"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("0x20"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("0x20"))[3]); } } __result; }))) : strcmp (cst->cstring.data, "0x20")))); }))
                    {
                      aloc al;
                      qtype qt;
                      ((void) ((interrupt_status_qtype != ((void *)0)) ? 0 : (__assert_fail ("interrupt_status_qtype != ((void *)0)", "flow.c", 2834, __PRETTY_FUNCTION__), 0)));
                      al = aloc_qtype(interrupt_status_qtype);
                      qt = qtype_from_store(cur_store, al);
                      if (mkleq_qual(fc->loc, &fs_qgate, qual_qtype(qt),
                                     enabled_qual))
                        report_qerror(fc->loc, sev_err,
                                      "kmalloc (non-ATOMIC) called with "
                                      "interrupts enabled");
                    }
                }
            }
          return mkeinfo(ret_qtype, cur_store, ((void *)0), ((void *)0), 1);
        }
      else
        return mkeinfo(((void *)0), ((void *)0), ((void *)0), ((void *)0), 0);
    }
  else
    return mkeinfo(((void *)0), ((void *)0), ((void *)0), ((void *)0), 0);
}
bool file_pointer_fs_qtype(qtype qt, store s)
{
  if (qtype_pointer_loc(qt))
    {
      qtype points_to;
      points_to = points_to_fs_qtype(qt, s);
      return (qtype_struct(points_to) &&
              tag_name_qtype(points_to) &&
              !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (tag_name_qtype(points_to)) && __builtin_constant_p ("_IO_FILE") && (__s1_len = strlen (tag_name_qtype(points_to)), __s2_len = strlen ("_IO_FILE"), (!((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (tag_name_qtype(points_to)), (__const char *) ("_IO_FILE"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (tag_name_qtype(points_to)) && ((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) && (__s1_len = strlen (tag_name_qtype(points_to)), __s1_len < 4) ? (__builtin_constant_p ("_IO_FILE") && ((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - ((__const unsigned char *) (__const char *)("_IO_FILE"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("_IO_FILE"); register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("_IO_FILE") && ((size_t)(const void *)(("_IO_FILE") + 1) - (size_t)(const void *)("_IO_FILE") == 1) && (__s2_len = strlen ("_IO_FILE"), __s2_len < 4) ? (__builtin_constant_p (tag_name_qtype(points_to)) && ((size_t)(const void *)((tag_name_qtype(points_to)) + 1) - (size_t)(const void *)(tag_name_qtype(points_to)) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[0] - ((__const unsigned char *) (__const char *)("_IO_FILE"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (tag_name_qtype(points_to)))[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (tag_name_qtype(points_to)); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("_IO_FILE"))[3]); } } __result; }))) : strcmp (tag_name_qtype(points_to), "_IO_FILE")))); }));
    }
  return 0;
}
static einfo split_file_pointer_context(location loc, qtype qt, store s)
{
  aloc al;
  qtype old_qt, new_non_null_qt, new_null_qt;
  store non_null_store, null_store;
  ((void) ((have_stream_quals && file_pointer_fs_qtype(qt, s)) ? 0 : (__assert_fail ("have_stream_quals && file_pointer_fs_qtype(qt, s)", "flow.c", 2877, __PRETTY_FUNCTION__), 0)));
  al = aloc_qtype(qt);
  old_qt = qtype_from_store(s, al);
  new_non_null_qt = copy_qtype(old_qt, loc);
  new_null_qt = copy_qtype(old_qt, loc);
  ((void) ((!(cond_mkleq_qual(loc, &fs_qgate, read_unchecked_qual, qual_qtype(old_qt), &fs_qgate, read_qual, qual_qtype(new_non_null_qt)))) ? 0 : (__assert_fail ("!(cond_mkleq_qual(loc, &fs_qgate, read_unchecked_qual, qual_qtype(old_qt), &fs_qgate, read_qual, qual_qtype(new_non_null_qt)))", "flow.c", 2883, __PRETTY_FUNCTION__), 0)));
  ((void) ((!(cond_mkleq_qual(loc, &fs_qgate, write_unchecked_qual, qual_qtype(old_qt), &fs_qgate, write_qual, qual_qtype(new_non_null_qt)))) ? 0 : (__assert_fail ("!(cond_mkleq_qual(loc, &fs_qgate, write_unchecked_qual, qual_qtype(old_qt), &fs_qgate, write_qual, qual_qtype(new_non_null_qt)))", "flow.c", 2886, __PRETTY_FUNCTION__), 0)));
  ((void) ((!(mkleq_qual(loc, &fs_qgate, closed_qual, qual_qtype(new_null_qt)))) ? 0 : (__assert_fail ("!(mkleq_qual(loc, &fs_qgate, closed_qual, qual_qtype(new_null_qt)))", "flow.c", 2889, __PRETTY_FUNCTION__), 0)));
  non_null_store =
    make_store_assign(loc,
                      "strong update failure in fopen result test",
                      s, al, new_non_null_qt, 1);
  null_store =
    make_store_assign(loc,
                      "strong update failure in fopen result test",
                      s, al, new_null_qt, 1);
  return mkeinfo(qt, ((void *)0), non_null_store, null_store, 0);
}
typedef struct PrintQueue_elt {
  enum { kind_exp, kind_stmt } kind;
  union {
    statement stmt;
    expression expr;
  } u;
  store store;
  aloc al;
} * printqueue_elt;
static void print_queue(void)
{
  dd_list_pos cur;
  for (cur = dd_first((printqueue)); !(!(cur)->next); cur = ((cur)->next))
    {
      printqueue_elt elt = ((printqueue_elt)((cur)->data));
      if (elt->kind == kind_exp)
        {
          printf("\n Expression: ");
          prt_expression(elt->u.expr, 0);
        }
      else if (elt->kind == kind_stmt)
        {
          printf("\n Stmt: ");
          prt_statement(elt->u.stmt);
        }
      else
        __fail("flow.c", 2941, __FUNCTION__, "Unexpected print queue elt kind %d\n", elt->kind);
      printf("\n");
      print_aloc(printf, elt->al);
      printf("^");
      switch (lin_from_store(elt->store, elt->al))
        {
        case 0: printf("0"); break;
        case 1: printf("1"); break;
        case 2: printf("w"); break;
        default: printf("?");
        }
      printf("\n---------------------------------------------------\n");
    }
}
static void queue_print_stmt(statement stmt, store s, aloc al)
{
  printqueue_elt elt;
  elt = (sizeof(struct PrintQueue_elt) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct PrintQueue_elt)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct PrintQueue_elt), 0));
  elt->kind = kind_stmt;
  elt->u.stmt = stmt;
  elt->store = s;
  elt->al = al;
  dd_add_last(parse_region, printqueue, elt);
}
static void queue_print_exp(expression expr, store s, aloc al)
{
  printqueue_elt elt;
  elt = (sizeof(struct PrintQueue_elt) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct PrintQueue_elt)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct PrintQueue_elt), 0));
  elt->kind = kind_exp;
  elt->u.expr = expr;
  elt->store = s;
  elt->al = al;
  dd_add_last(parse_region, printqueue, elt);
}
void print_assert_type_failures(void)
{
  dd_list_pos cur;
  dd_list_pos cur_hax;
  qual locked_qual = find_qual("$locked");
  qual unlocked_qual = find_qual("$unlocked");
  int errors = 0;
  int total = 0;
  ((void) ((locked_qual && unlocked_qual) ? 0 : (__assert_fail ("locked_qual && unlocked_qual", "flow.c", 2996, __PRETTY_FUNCTION__), 0)));
  cur_hax = dd_first(assert_type_qtypes_hax);
  for (cur = dd_first((assert_type_qtypes)); !(!(cur)->next); cur = ((cur)->next))
    {
      qual top;
      qtype aqt = ((qtype)((cur)->data));
      location loc = ((location)((cur_hax)->data));
      top = qual_qtype(aqt);
      if ((leq_qual(top, locked_qual) &&
           leq_qual(unlocked_qual, top)) ||
          (leq_qual(top, unlocked_qual) &&
           leq_qual(locked_qual, top)))
        {
          printf("%s:%ld ", loc->filename, loc->lineno);
          printf("Assert type failure\n");
          errors++;
        }
      total++;
      cur_hax = ((cur_hax)->next);
    }
  printf("Assert type failures : %d/%d\n", errors, total);
}
const char CANON_IDENT_87e2832e1a5cd2e8d6124ba8357e188a[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/flow.c";
