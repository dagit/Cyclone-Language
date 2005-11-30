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
typedef __ssize_t ssize_t;
typedef __intptr_t intptr_t;
typedef __socklen_t socklen_t;
extern int access (__const char *__name, int __type) ;
extern __off_t lseek (int __fd, __off_t __offset, int __whence) ;
extern int close (int __fd) ;
extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;
extern ssize_t write (int __fd, __const void *__buf, size_t __n) ;
extern int pipe (int __pipedes[2]) ;
extern unsigned int alarm (unsigned int __seconds) ;
extern unsigned int sleep (unsigned int __seconds) ;
extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
            ;
extern int usleep (__useconds_t __useconds) ;
extern int pause (void) ;
extern int chown (__const char *__file, __uid_t __owner, __gid_t __group)
            ;
extern int fchown (int __fd, __uid_t __owner, __gid_t __group) ;
extern int lchown (__const char *__file, __uid_t __owner, __gid_t __group)
            ;
extern int chdir (__const char *__path) ;
extern int fchdir (int __fd) ;
extern char *getcwd (char *__buf, size_t __size) ;
extern char *getwd (char *__buf) ;
extern int dup (int __fd) ;
extern int dup2 (int __fd, int __fd2) ;
extern char **__environ;
extern int execve (__const char *__path, char *__const __argv[],
                   char *__const __envp[]) ;
extern int execv (__const char *__path, char *__const __argv[]) ;
extern int execle (__const char *__path, __const char *__arg, ...) ;
extern int execl (__const char *__path, __const char *__arg, ...) ;
extern int execvp (__const char *__file, char *__const __argv[]) ;
extern int execlp (__const char *__file, __const char *__arg, ...) ;
extern int nice (int __inc) ;
extern void _exit (int __status) __attribute__ ((__noreturn__));
enum
  {
    _PC_LINK_MAX,
    _PC_MAX_CANON,
    _PC_MAX_INPUT,
    _PC_NAME_MAX,
    _PC_PATH_MAX,
    _PC_PIPE_BUF,
    _PC_CHOWN_RESTRICTED,
    _PC_NO_TRUNC,
    _PC_VDISABLE,
    _PC_SYNC_IO,
    _PC_ASYNC_IO,
    _PC_PRIO_IO,
    _PC_SOCK_MAXBUF,
    _PC_FILESIZEBITS,
    _PC_REC_INCR_XFER_SIZE,
    _PC_REC_MAX_XFER_SIZE,
    _PC_REC_MIN_XFER_SIZE,
    _PC_REC_XFER_ALIGN,
    _PC_ALLOC_SIZE_MIN,
    _PC_SYMLINK_MAX,
    _PC_2_SYMLINKS
  };
enum
  {
    _SC_ARG_MAX,
    _SC_CHILD_MAX,
    _SC_CLK_TCK,
    _SC_NGROUPS_MAX,
    _SC_OPEN_MAX,
    _SC_STREAM_MAX,
    _SC_TZNAME_MAX,
    _SC_JOB_CONTROL,
    _SC_SAVED_IDS,
    _SC_REALTIME_SIGNALS,
    _SC_PRIORITY_SCHEDULING,
    _SC_TIMERS,
    _SC_ASYNCHRONOUS_IO,
    _SC_PRIORITIZED_IO,
    _SC_SYNCHRONIZED_IO,
    _SC_FSYNC,
    _SC_MAPPED_FILES,
    _SC_MEMLOCK,
    _SC_MEMLOCK_RANGE,
    _SC_MEMORY_PROTECTION,
    _SC_MESSAGE_PASSING,
    _SC_SEMAPHORES,
    _SC_SHARED_MEMORY_OBJECTS,
    _SC_AIO_LISTIO_MAX,
    _SC_AIO_MAX,
    _SC_AIO_PRIO_DELTA_MAX,
    _SC_DELAYTIMER_MAX,
    _SC_MQ_OPEN_MAX,
    _SC_MQ_PRIO_MAX,
    _SC_VERSION,
    _SC_PAGESIZE,
    _SC_RTSIG_MAX,
    _SC_SEM_NSEMS_MAX,
    _SC_SEM_VALUE_MAX,
    _SC_SIGQUEUE_MAX,
    _SC_TIMER_MAX,
    _SC_BC_BASE_MAX,
    _SC_BC_DIM_MAX,
    _SC_BC_SCALE_MAX,
    _SC_BC_STRING_MAX,
    _SC_COLL_WEIGHTS_MAX,
    _SC_EQUIV_CLASS_MAX,
    _SC_EXPR_NEST_MAX,
    _SC_LINE_MAX,
    _SC_RE_DUP_MAX,
    _SC_CHARCLASS_NAME_MAX,
    _SC_2_VERSION,
    _SC_2_C_BIND,
    _SC_2_C_DEV,
    _SC_2_FORT_DEV,
    _SC_2_FORT_RUN,
    _SC_2_SW_DEV,
    _SC_2_LOCALEDEF,
    _SC_PII,
    _SC_PII_XTI,
    _SC_PII_SOCKET,
    _SC_PII_INTERNET,
    _SC_PII_OSI,
    _SC_POLL,
    _SC_SELECT,
    _SC_UIO_MAXIOV,
    _SC_IOV_MAX = _SC_UIO_MAXIOV,
    _SC_PII_INTERNET_STREAM,
    _SC_PII_INTERNET_DGRAM,
    _SC_PII_OSI_COTS,
    _SC_PII_OSI_CLTS,
    _SC_PII_OSI_M,
    _SC_T_IOV_MAX,
    _SC_THREADS,
    _SC_THREAD_SAFE_FUNCTIONS,
    _SC_GETGR_R_SIZE_MAX,
    _SC_GETPW_R_SIZE_MAX,
    _SC_LOGIN_NAME_MAX,
    _SC_TTY_NAME_MAX,
    _SC_THREAD_DESTRUCTOR_ITERATIONS,
    _SC_THREAD_KEYS_MAX,
    _SC_THREAD_STACK_MIN,
    _SC_THREAD_THREADS_MAX,
    _SC_THREAD_ATTR_STACKADDR,
    _SC_THREAD_ATTR_STACKSIZE,
    _SC_THREAD_PRIORITY_SCHEDULING,
    _SC_THREAD_PRIO_INHERIT,
    _SC_THREAD_PRIO_PROTECT,
    _SC_THREAD_PROCESS_SHARED,
    _SC_NPROCESSORS_CONF,
    _SC_NPROCESSORS_ONLN,
    _SC_PHYS_PAGES,
    _SC_AVPHYS_PAGES,
    _SC_ATEXIT_MAX,
    _SC_PASS_MAX,
    _SC_XOPEN_VERSION,
    _SC_XOPEN_XCU_VERSION,
    _SC_XOPEN_UNIX,
    _SC_XOPEN_CRYPT,
    _SC_XOPEN_ENH_I18N,
    _SC_XOPEN_SHM,
    _SC_2_CHAR_TERM,
    _SC_2_C_VERSION,
    _SC_2_UPE,
    _SC_XOPEN_XPG2,
    _SC_XOPEN_XPG3,
    _SC_XOPEN_XPG4,
    _SC_CHAR_BIT,
    _SC_CHAR_MAX,
    _SC_CHAR_MIN,
    _SC_INT_MAX,
    _SC_INT_MIN,
    _SC_LONG_BIT,
    _SC_WORD_BIT,
    _SC_MB_LEN_MAX,
    _SC_NZERO,
    _SC_SSIZE_MAX,
    _SC_SCHAR_MAX,
    _SC_SCHAR_MIN,
    _SC_SHRT_MAX,
    _SC_SHRT_MIN,
    _SC_UCHAR_MAX,
    _SC_UINT_MAX,
    _SC_ULONG_MAX,
    _SC_USHRT_MAX,
    _SC_NL_ARGMAX,
    _SC_NL_LANGMAX,
    _SC_NL_MSGMAX,
    _SC_NL_NMAX,
    _SC_NL_SETMAX,
    _SC_NL_TEXTMAX,
    _SC_XBS5_ILP32_OFF32,
    _SC_XBS5_ILP32_OFFBIG,
    _SC_XBS5_LP64_OFF64,
    _SC_XBS5_LPBIG_OFFBIG,
    _SC_XOPEN_LEGACY,
    _SC_XOPEN_REALTIME,
    _SC_XOPEN_REALTIME_THREADS,
    _SC_ADVISORY_INFO,
    _SC_BARRIERS,
    _SC_BASE,
    _SC_C_LANG_SUPPORT,
    _SC_C_LANG_SUPPORT_R,
    _SC_CLOCK_SELECTION,
    _SC_CPUTIME,
    _SC_THREAD_CPUTIME,
    _SC_DEVICE_IO,
    _SC_DEVICE_SPECIFIC,
    _SC_DEVICE_SPECIFIC_R,
    _SC_FD_MGMT,
    _SC_FIFO,
    _SC_PIPE,
    _SC_FILE_ATTRIBUTES,
    _SC_FILE_LOCKING,
    _SC_FILE_SYSTEM,
    _SC_MONOTONIC_CLOCK,
    _SC_MULTI_PROCESS,
    _SC_SINGLE_PROCESS,
    _SC_NETWORKING,
    _SC_READER_WRITER_LOCKS,
    _SC_SPIN_LOCKS,
    _SC_REGEXP,
    _SC_REGEX_VERSION,
    _SC_SHELL,
    _SC_SIGNALS,
    _SC_SPAWN,
    _SC_SPORADIC_SERVER,
    _SC_THREAD_SPORADIC_SERVER,
    _SC_SYSTEM_DATABASE,
    _SC_SYSTEM_DATABASE_R,
    _SC_TIMEOUTS,
    _SC_TYPED_MEMORY_OBJECTS,
    _SC_USER_GROUPS,
    _SC_USER_GROUPS_R,
    _SC_2_PBS,
    _SC_2_PBS_ACCOUNTING,
    _SC_2_PBS_LOCATE,
    _SC_2_PBS_MESSAGE,
    _SC_2_PBS_TRACK,
    _SC_SYMLOOP_MAX,
    _SC_STREAMS,
    _SC_2_PBS_CHECKPOINT,
    _SC_V6_ILP32_OFF32,
    _SC_V6_ILP32_OFFBIG,
    _SC_V6_LP64_OFF64,
    _SC_V6_LPBIG_OFFBIG,
    _SC_HOST_NAME_MAX,
    _SC_TRACE,
    _SC_TRACE_EVENT_FILTER,
    _SC_TRACE_INHERIT,
    _SC_TRACE_LOG
  };
enum
  {
    _CS_PATH,
    _CS_V6_WIDTH_RESTRICTED_ENVS,
    _CS_GNU_LIBC_VERSION,
    _CS_GNU_LIBPTHREAD_VERSION,
    _CS_LFS_CFLAGS = 1000,
    _CS_LFS_LDFLAGS,
    _CS_LFS_LIBS,
    _CS_LFS_LINTFLAGS,
    _CS_LFS64_CFLAGS,
    _CS_LFS64_LDFLAGS,
    _CS_LFS64_LIBS,
    _CS_LFS64_LINTFLAGS,
    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,
    _CS_XBS5_ILP32_OFF32_LDFLAGS,
    _CS_XBS5_ILP32_OFF32_LIBS,
    _CS_XBS5_ILP32_OFF32_LINTFLAGS,
    _CS_XBS5_ILP32_OFFBIG_CFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LIBS,
    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,
    _CS_XBS5_LP64_OFF64_CFLAGS,
    _CS_XBS5_LP64_OFF64_LDFLAGS,
    _CS_XBS5_LP64_OFF64_LIBS,
    _CS_XBS5_LP64_OFF64_LINTFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LIBS,
    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LIBS,
    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,
    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,
    _CS_POSIX_V6_LP64_OFF64_CFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LIBS,
    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS
  };
extern long int pathconf (__const char *__path, int __name) ;
extern long int fpathconf (int __fd, int __name) ;
extern long int sysconf (int __name) __attribute__ ((__const__));
extern size_t confstr (int __name, char *__buf, size_t __len) ;
extern __pid_t getpid (void) ;
extern __pid_t getppid (void) ;
extern __pid_t getpgrp (void) ;
extern __pid_t __getpgid (__pid_t __pid) ;
extern int setpgid (__pid_t __pid, __pid_t __pgid) ;
extern int setpgrp (void) ;
extern __pid_t setsid (void) ;
extern __uid_t getuid (void) ;
extern __uid_t geteuid (void) ;
extern __gid_t getgid (void) ;
extern __gid_t getegid (void) ;
extern int getgroups (int __size, __gid_t __list[]) ;
extern int setuid (__uid_t __uid) ;
extern int setreuid (__uid_t __ruid, __uid_t __euid) ;
extern int seteuid (__uid_t __uid) ;
extern int setgid (__gid_t __gid) ;
extern int setregid (__gid_t __rgid, __gid_t __egid) ;
extern int setegid (__gid_t __gid) ;
extern __pid_t fork (void) ;
extern __pid_t vfork (void) ;
extern char *ttyname (int __fd) ;
extern int ttyname_r (int __fd, char *__buf, size_t __buflen) ;
extern int isatty (int __fd) ;
extern int ttyslot (void) ;
extern int link (__const char *__from, __const char *__to) ;
extern int symlink (__const char *__from, __const char *__to) ;
extern int readlink (__const char *__restrict __path, char *__restrict __buf,
                     size_t __len) ;
extern int unlink (__const char *__name) ;
extern int rmdir (__const char *__path) ;
extern __pid_t tcgetpgrp (int __fd) ;
extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) ;
extern char *getlogin (void) ;
extern int setlogin (__const char *__name) ;
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts);
extern int gethostname (char *__name, size_t __len) ;
extern int sethostname (__const char *__name, size_t __len) ;
extern int sethostid (long int __id) ;
extern int getdomainname (char *__name, size_t __len) ;
extern int setdomainname (__const char *__name, size_t __len) ;
extern int vhangup (void) ;
extern int revoke (__const char *__file) ;
extern int profil (unsigned short int *__sample_buffer, size_t __size,
                   size_t __offset, unsigned int __scale) ;
extern int acct (__const char *__name) ;
extern char *getusershell (void) ;
extern void endusershell (void) ;
extern void setusershell (void) ;
extern int daemon (int __nochdir, int __noclose) ;
extern int chroot (__const char *__path) ;
extern char *getpass (__const char *__prompt) ;
extern int fsync (int __fd) ;
extern long int gethostid (void) ;
extern void sync (void) ;
extern int getpagesize (void) __attribute__ ((__const__));
extern int truncate (__const char *__file, __off_t __length) ;
extern int ftruncate (int __fd, __off_t __length) ;
extern int getdtablesize (void) ;
extern int brk (void *__addr) ;
extern void *sbrk (intptr_t __delta) ;
extern long int syscall (long int __sysno, ...) ;
extern int lockf (int __fd, int __cmd, __off_t __len) ;
extern int fdatasync (int __fildes) ;
extern int pthread_atfork (void (*__prepare) (void),
                           void (*__parent) (void),
                           void (*__child) (void)) ;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int lyyleng;
extern FILE *lyyin, *lyyout;
typedef unsigned int yy_size_t;
struct yy_buffer_state
        {
        FILE *yy_input_file;
        char *yy_ch_buf;
        char *yy_buf_pos;
        yy_size_t yy_buf_size;
        int yy_n_chars;
        int yy_is_our_buffer;
        int yy_is_interactive;
        int yy_at_bol;
        int yy_fill_buffer;
        int yy_buffer_status;
        };
static YY_BUFFER_STATE yy_current_buffer = 0;
static char yy_hold_char;
static int yy_n_chars;
int lyyleng;
static char *yy_c_buf_p = (char *) 0;
static int yy_init = 1;
static int yy_start = 0;
static int yy_did_buffer_switch_on_eof;
void lyyrestart ( FILE *input_file );
void lyy_switch_to_buffer ( YY_BUFFER_STATE new_buffer );
void lyy_load_buffer_state ( void );
YY_BUFFER_STATE lyy_create_buffer ( FILE *file, int size );
void lyy_delete_buffer ( YY_BUFFER_STATE b );
void lyy_init_buffer ( YY_BUFFER_STATE b, FILE *file );
void lyy_flush_buffer ( YY_BUFFER_STATE b );
YY_BUFFER_STATE lyy_scan_buffer ( char *base, yy_size_t size );
YY_BUFFER_STATE lyy_scan_string ( const char *yy_str );
YY_BUFFER_STATE lyy_scan_bytes ( const char *bytes, int len );
static void *yy_flex_alloc ( yy_size_t );
static void *yy_flex_realloc ( void *, yy_size_t );
static void yy_flex_free ( void * );
typedef unsigned char YY_CHAR;
FILE *lyyin = (FILE *) 0, *lyyout = (FILE *) 0;
typedef int yy_state_type;
extern char *lyytext;
static yy_state_type yy_get_previous_state ( void );
static yy_state_type yy_try_NUL_trans ( yy_state_type current_state );
static int yy_get_next_buffer ( void );
static void yy_fatal_error ( const char msg[] );
static const short int yy_accept[156] =
    { 0,
        0, 0, 37, 37, 42, 41, 1, 2, 41, 7,
       36, 41, 32, 33, 41, 41, 34, 35, 41, 41,
       41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
       29, 30, 37, 39, 38, 0, 31, 7, 3, 0,
        0, 0, 0, 0, 0, 18, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 19, 0, 0, 37, 38,
       38, 40, 0, 0, 21, 0, 0, 0, 0, 0,
        0, 0, 17, 0, 16, 0, 12, 0, 0, 0,
        0, 0, 0, 0, 0, 20, 0, 0, 0, 0,
       22, 0, 0, 0, 15, 0, 0, 28, 0, 8,
        4, 0, 0, 0, 14, 0, 0, 0, 13, 0,
        0, 26, 0, 0, 0, 0, 0, 0, 0, 0,
       27, 0, 0, 0, 0, 11, 23, 0, 0, 0,
        0, 0, 0, 6, 5, 24, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 25, 0, 0, 0, 0,
        0, 10, 0, 9, 0
    } ;
static const int yy_ec[256] =
    { 0,
        1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 1, 4, 1, 5, 1, 1, 1, 1,
        1, 6, 1, 7, 8, 1, 9, 10, 10, 10,
       10, 10, 10, 10, 10, 10, 10, 1, 1, 11,
       12, 1, 1, 1, 13, 10, 10, 14, 15, 10,
       10, 10, 16, 10, 10, 17, 10, 10, 18, 19,
       10, 20, 10, 21, 10, 10, 10, 10, 10, 10,
       22, 1, 23, 1, 10, 1, 24, 10, 25, 26,
       27, 28, 29, 10, 30, 10, 10, 31, 10, 32,
       33, 34, 35, 36, 37, 38, 39, 40, 41, 10,
       10, 10, 42, 1, 43, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1
    } ;
static const int yy_meta[44] =
    { 0,
        1, 1, 2, 1, 1, 3, 1, 1, 1, 4,
        1, 1, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 1, 1, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 1, 1
    } ;
static const short int yy_base[161] =
    { 0,
        0, 0, 41, 42, 193, 194, 194, 194, 188, 0,
      194, 185, 194, 194, 26, 36, 194, 194, 159, 156,
      155, 22, 21, 160, 26, 41, 159, 155, 150, 31,
      194, 194, 0, 194, 52, 179, 194, 0, 194, 42,
       46, 151, 38, 140, 152, 194, 152, 145, 137, 147,
      143, 137, 137, 35, 143, 194, 140, 139, 0, 67,
       69, 194, 56, 46, 194, 136, 131, 135, 139, 134,
      123, 136, 194, 53, 194, 134, 194, 123, 128, 120,
      134, 57, 65, 121, 118, 194, 130, 128, 145, 121,
      194, 115, 119, 113, 194, 121, 109, 194, 72, 194,
      194, 108, 58, 60, 194, 112, 111, 113, 194, 112,
       71, 194, 110, 102, 107, 111, 103, 95, 110, 103,
      194, 100, 96, 94, 98, 194, 194, 91, 101, 86,
       98, 98, 87, 194, 194, 194, 92, 90, 91, 87,
       82, 78, 72, 82, 78, 194, 63, 60, 69, 71,
       54, 194, 64, 194, 194, 102, 106, 85, 110, 114
    } ;
static const short int yy_def[161] =
    { 0,
      155, 1, 156, 156, 155, 155, 155, 155, 157, 158,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 159, 155, 160, 157, 155, 158, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 159, 160,
      160, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 0, 155, 155, 155, 155, 155
    } ;
static const short int yy_nxt[238] =
    { 0,
        6, 7, 8, 9, 10, 6, 11, 6, 12, 6,
       13, 14, 6, 6, 6, 6, 6, 15, 16, 6,
        6, 17, 18, 19, 20, 21, 22, 23, 6, 6,
       24, 25, 15, 26, 6, 27, 28, 6, 29, 30,
        6, 31, 32, 34, 34, 40, 35, 35, 41, 45,
       47, 48, 50, 41, 57, 63, 46, 61, 51, 41,
       62, 40, 77, 58, 41, 64, 83, 63, 66, 67,
       82, 78, 155, 52, 61, 155, 98, 62, 53, 91,
       99, 64, 82, 83, 111, 113, 92, 121, 38, 115,
      154, 114, 98, 153, 99, 111, 116, 152, 151, 150,
      149, 121, 33, 33, 33, 33, 36, 148, 36, 36,
       59, 147, 146, 59, 60, 145, 60, 60, 144, 143,
      142, 141, 140, 139, 138, 137, 136, 135, 134, 133,
      132, 131, 130, 129, 128, 127, 126, 125, 124, 123,
      122, 120, 119, 118, 117, 112, 110, 109, 108, 107,
      106, 105, 104, 103, 102, 101, 100, 97, 96, 95,
       94, 93, 90, 89, 88, 87, 86, 85, 84, 81,
       80, 79, 76, 75, 74, 73, 72, 71, 70, 69,
       68, 65, 37, 56, 55, 54, 49, 44, 43, 42,
       39, 37, 155, 5, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155
    } ;
static const short int yy_chk[238] =
    { 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 3, 4, 15, 3, 4, 16, 22,
       23, 23, 25, 26, 30, 40, 22, 35, 25, 16,
       35, 15, 54, 30, 26, 41, 64, 40, 43, 43,
       63, 54, 60, 26, 61, 60, 82, 61, 26, 74,
       83, 41, 63, 64, 99, 103, 74, 111, 158, 104,
      153, 103, 82, 151, 83, 99, 104, 150, 149, 148,
      147, 111, 156, 156, 156, 156, 157, 145, 157, 157,
      159, 144, 143, 159, 160, 142, 160, 160, 141, 140,
      139, 138, 137, 133, 132, 131, 130, 129, 128, 125,
      124, 123, 122, 120, 119, 118, 117, 116, 115, 114,
      113, 110, 108, 107, 106, 102, 97, 96, 94, 93,
       92, 90, 89, 88, 87, 85, 84, 81, 80, 79,
       78, 76, 72, 71, 70, 69, 68, 67, 66, 58,
       57, 55, 53, 52, 51, 50, 49, 48, 47, 45,
       44, 42, 36, 29, 28, 27, 24, 21, 20, 19,
       12, 9, 5, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
      155, 155, 155, 155, 155, 155, 155
    } ;
static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;
char *lyytext;
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
extern void *malloc (size_t __size) __attribute__ ((__malloc__));
extern void *calloc (size_t __nmemb, size_t __size)
             __attribute__ ((__malloc__));
extern char *__strdup (__const char *__string) __attribute__ ((__malloc__));
extern char *__strndup (__const char *__string, size_t __n)
             __attribute__ ((__malloc__));
typedef union {
  char *str;
} yystype;
extern yystype lyylval;
  extern int lattice_line;
static int input ( void );
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
int lyylex ( void )
        {
        register yy_state_type yy_current_state;
        register char *yy_cp = ((void *)0), *yy_bp = ((void *)0);
        register int yy_act;
        if ( yy_init )
                {
                yy_init = 0;
                if ( ! yy_start )
                        yy_start = 1;
                if ( ! lyyin )
                        lyyin = stdin;
                if ( ! lyyout )
                        lyyout = stdout;
                if ( ! yy_current_buffer )
                        yy_current_buffer =
                                lyy_create_buffer( lyyin, 16384 );
                lyy_load_buffer_state();
                }
        while ( 1 )
                {
                yy_cp = yy_c_buf_p;
                *yy_cp = yy_hold_char;
                yy_bp = yy_cp;
                yy_current_state = yy_start;
yy_match:
                do
                        {
                        register YY_CHAR yy_c = yy_ec[((unsigned int) (unsigned char) *yy_cp)];
                        if ( yy_accept[yy_current_state] )
                                {
                                yy_last_accepting_state = yy_current_state;
                                yy_last_accepting_cpos = yy_cp;
                                }
                        while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
                                {
                                yy_current_state = (int) yy_def[yy_current_state];
                                if ( yy_current_state >= 156 )
                                        yy_c = yy_meta[(unsigned int) yy_c];
                                }
                        yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
                        ++yy_cp;
                        }
                while ( yy_base[yy_current_state] != 194 );
yy_find_action:
                yy_act = yy_accept[yy_current_state];
                if ( yy_act == 0 )
                        {
                        yy_cp = yy_last_accepting_cpos;
                        yy_current_state = yy_last_accepting_state;
                        yy_act = yy_accept[yy_current_state];
                        }
                lyytext = yy_bp; lyyleng = (int) (yy_cp - yy_bp); yy_hold_char = *yy_cp; *yy_cp = '\0'; yy_c_buf_p = yy_cp;;
do_action:
                switch ( yy_act )
        {
                        case 0:
                        *yy_cp = yy_hold_char;
                        yy_cp = yy_last_accepting_cpos;
                        yy_current_state = yy_last_accepting_state;
                        goto yy_find_action;
case 1:
        break;
case 2:
{ lattice_line++; }
        break;
case 3:
{ yy_start = 1 + 2 *(1); }
        break;
case 4:
{ lyylval.str = (__extension__ (__builtin_constant_p (lyytext) && ((size_t)(const void *)((lyytext) + 1) - (size_t)(const void *)(lyytext) == 1) ? (((__const char *) (lyytext))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (lyytext) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, lyytext, __len); __retval; })) : __strdup (lyytext))); return 257; }
        break;
case 5:
{ lyylval.str = (__extension__ (__builtin_constant_p (lyytext) && ((size_t)(const void *)((lyytext) + 1) - (size_t)(const void *)(lyytext) == 1) ? (((__const char *) (lyytext))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (lyytext) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, lyytext, __len); __retval; })) : __strdup (lyytext))); return 257; }
        break;
case 6:
{ lyylval.str = (__extension__ (__builtin_constant_p (lyytext) && ((size_t)(const void *)((lyytext) + 1) - (size_t)(const void *)(lyytext) == 1) ? (((__const char *) (lyytext))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (lyytext) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, lyytext, __len); __retval; })) : __strdup (lyytext))); return 257; }
        break;
case 7:
{ lyylval.str = (__extension__ (__builtin_constant_p (lyytext) && ((size_t)(const void *)((lyytext) + 1) - (size_t)(const void *)(lyytext) == 1) ? (((__const char *) (lyytext))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (lyytext) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, lyytext, __len); __retval; })) : __strdup (lyytext))); return 257; }
        break;
case 8:
{ return 259; }
        break;
case 9:
{ return 261; }
        break;
case 10:
{ return 262; }
        break;
case 11:
{ return 266; }
        break;
case 12:
{ return 270; }
        break;
case 13:
{ return 272; }
        break;
case 14:
{ return 264; }
        break;
case 15:
{ return 271; }
        break;
case 16:
{ return 269; }
        break;
case 17:
{ return 265; }
        break;
case 18:
{ return 263; }
        break;
case 19:
{ return 276; }
        break;
case 20:
{ return 277; }
        break;
case 21:
{ return 278; }
        break;
case 22:
{ return 279; }
        break;
case 23:
{ return 273; }
        break;
case 24:
{ return 274; }
        break;
case 25:
{ return 275; }
        break;
case 26:
{ return 260; }
        break;
case 27:
{ return 268; }
        break;
case 28:
{ return 267; }
        break;
case 29:
{ return '{'; }
        break;
case 30:
{ return '}'; }
        break;
case 31:
{ lyylval.str = (__extension__ (__builtin_constant_p (lyytext+1) && ((size_t)(const void *)((lyytext+1) + 1) - (size_t)(const void *)(lyytext+1) == 1) ? (((__const char *) (lyytext+1))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (lyytext+1) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, lyytext+1, __len); __retval; })) : __strdup (lyytext+1)));
              lyylval.str[strlen(lyylval.str)-1] = '\0';
              return 258; }
        break;
case 32:
{ return '<'; }
        break;
case 33:
{ return '='; }
        break;
case 34:
{ return '['; }
        break;
case 35:
{ return ']'; }
        break;
case 36:
{ return ','; }
        break;
case 37:
        break;
case 38: break;
case 39:
{ lattice_line++; }
        break;
case 40:
yy_start = 1 + 2 *(0);
        break;
case 41:(void) fwrite( lyytext, lyyleng, 1, lyyout );
        break;
case (42 + 0 + 1):
case (42 + 1 + 1):
        return 0;
        case 42:
                {
                int yy_amount_of_matched_text = (int) (yy_cp - lyytext) - 1;
                *yy_cp = yy_hold_char;
                if ( yy_current_buffer->yy_buffer_status == 0 )
                        {
                        yy_n_chars = yy_current_buffer->yy_n_chars;
                        yy_current_buffer->yy_input_file = lyyin;
                        yy_current_buffer->yy_buffer_status = 1;
                        }
                if ( yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars] )
                        {
                        yy_state_type yy_next_state;
                        yy_c_buf_p = lyytext + yy_amount_of_matched_text;
                        yy_current_state = yy_get_previous_state();
                        yy_next_state = yy_try_NUL_trans( yy_current_state );
                        yy_bp = lyytext + 0;
                        if ( yy_next_state )
                                {
                                yy_cp = ++yy_c_buf_p;
                                yy_current_state = yy_next_state;
                                goto yy_match;
                                }
                        else
                                {
                                yy_cp = yy_c_buf_p;
                                goto yy_find_action;
                                }
                        }
                else switch ( yy_get_next_buffer() )
                        {
                        case 1:
                                {
                                yy_did_buffer_switch_on_eof = 0;
                                if ( 1 )
                                        {
                                        yy_c_buf_p = lyytext + 0;
                                        yy_act = (42 + ((yy_start - 1) / 2) + 1);
                                        goto do_action;
                                        }
                                else
                                        {
                                        if ( ! yy_did_buffer_switch_on_eof )
                                                lyyrestart( lyyin );
                                        }
                                break;
                                }
                        case 0:
                                yy_c_buf_p =
                                        lyytext + yy_amount_of_matched_text;
                                yy_current_state = yy_get_previous_state();
                                yy_cp = yy_c_buf_p;
                                yy_bp = lyytext + 0;
                                goto yy_match;
                        case 2:
                                yy_c_buf_p =
                                &yy_current_buffer->yy_ch_buf[yy_n_chars];
                                yy_current_state = yy_get_previous_state();
                                yy_cp = yy_c_buf_p;
                                yy_bp = lyytext + 0;
                                goto yy_find_action;
                        }
                break;
                }
        default:
                yy_fatal_error( "fatal flex scanner internal error--no action found" );
        }
                }
        }
static int yy_get_next_buffer()
        {
        register char *dest = yy_current_buffer->yy_ch_buf;
        register char *source = lyytext;
        register int number_to_move, i;
        int ret_val;
        if ( yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1] )
                yy_fatal_error( "fatal flex scanner internal error--end of buffer missed" );
        if ( yy_current_buffer->yy_fill_buffer == 0 )
                {
                if ( yy_c_buf_p - lyytext - 0 == 1 )
                        {
                        return 1;
                        }
                else
                        {
                        return 2;
                        }
                }
        number_to_move = (int) (yy_c_buf_p - lyytext) - 1;
        for ( i = 0; i < number_to_move; ++i )
                *(dest++) = *(source++);
        if ( yy_current_buffer->yy_buffer_status == 2 )
                yy_current_buffer->yy_n_chars = yy_n_chars = 0;
        else
                {
                int num_to_read =
                        yy_current_buffer->yy_buf_size - number_to_move - 1;
                while ( num_to_read <= 0 )
                        {
                        YY_BUFFER_STATE b = yy_current_buffer;
                        int yy_c_buf_p_offset =
                                (int) (yy_c_buf_p - b->yy_ch_buf);
                        if ( b->yy_is_our_buffer )
                                {
                                int new_size = b->yy_buf_size * 2;
                                if ( new_size <= 0 )
                                        b->yy_buf_size += b->yy_buf_size / 8;
                                else
                                        b->yy_buf_size *= 2;
                                b->yy_ch_buf = (char *)
                                        yy_flex_realloc( (void *) b->yy_ch_buf,
                                                         b->yy_buf_size + 2 );
                                }
                        else
                                b->yy_ch_buf = 0;
                        if ( ! b->yy_ch_buf )
                                yy_fatal_error( "fatal error - scanner input buffer overflow" );
                        yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];
                        num_to_read = yy_current_buffer->yy_buf_size -
                                                number_to_move - 1;
                        }
                if ( num_to_read > 8192 )
                        num_to_read = 8192;
                if ( yy_current_buffer->yy_is_interactive ) { int c = '*', n; for ( n = 0; n < num_to_read && (c = _IO_getc (lyyin)) != (-1) && c != '\n'; ++n ) (&yy_current_buffer->yy_ch_buf[number_to_move])[n] = (char) c; if ( c == '\n' ) (&yy_current_buffer->yy_ch_buf[number_to_move])[n++] = (char) c; if ( c == (-1) && ferror( lyyin ) ) yy_fatal_error( "input in flex scanner failed" ); yy_n_chars = n; } else if ( ((yy_n_chars = fread( (&yy_current_buffer->yy_ch_buf[number_to_move]), 1, num_to_read, lyyin )) == 0) && ferror( lyyin ) ) yy_fatal_error( "input in flex scanner failed" );;
                yy_current_buffer->yy_n_chars = yy_n_chars;
                }
        if ( yy_n_chars == 0 )
                {
                if ( number_to_move == 0 )
                        {
                        ret_val = 1;
                        lyyrestart( lyyin );
                        }
                else
                        {
                        ret_val = 2;
                        yy_current_buffer->yy_buffer_status =
                                2;
                        }
                }
        else
                ret_val = 0;
        yy_n_chars += number_to_move;
        yy_current_buffer->yy_ch_buf[yy_n_chars] = 0;
        yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = 0;
        lyytext = &yy_current_buffer->yy_ch_buf[0];
        return ret_val;
        }
static yy_state_type yy_get_previous_state()
        {
        register yy_state_type yy_current_state;
        register char *yy_cp;
        yy_current_state = yy_start;
        for ( yy_cp = lyytext + 0; yy_cp < yy_c_buf_p; ++yy_cp )
                {
                register YY_CHAR yy_c = (*yy_cp ? yy_ec[((unsigned int) (unsigned char) *yy_cp)] : 1);
                if ( yy_accept[yy_current_state] )
                        {
                        yy_last_accepting_state = yy_current_state;
                        yy_last_accepting_cpos = yy_cp;
                        }
                while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
                        {
                        yy_current_state = (int) yy_def[yy_current_state];
                        if ( yy_current_state >= 156 )
                                yy_c = yy_meta[(unsigned int) yy_c];
                        }
                yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
                }
        return yy_current_state;
        }
static yy_state_type yy_try_NUL_trans( yy_state_type yy_current_state )
        {
        register int yy_is_jam;
        register char *yy_cp = yy_c_buf_p;
        register YY_CHAR yy_c = 1;
        if ( yy_accept[yy_current_state] )
                {
                yy_last_accepting_state = yy_current_state;
                yy_last_accepting_cpos = yy_cp;
                }
        while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
                {
                yy_current_state = (int) yy_def[yy_current_state];
                if ( yy_current_state >= 156 )
                        yy_c = yy_meta[(unsigned int) yy_c];
                }
        yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
        yy_is_jam = (yy_current_state == 155);
        return yy_is_jam ? 0 : yy_current_state;
        }
static int input()
        {
        int c;
        *yy_c_buf_p = yy_hold_char;
        if ( *yy_c_buf_p == 0 )
                {
                if ( yy_c_buf_p < &yy_current_buffer->yy_ch_buf[yy_n_chars] )
                        *yy_c_buf_p = '\0';
                else
                        {
                        int offset = yy_c_buf_p - lyytext;
                        ++yy_c_buf_p;
                        switch ( yy_get_next_buffer() )
                                {
                                case 2:
                                        lyyrestart( lyyin );
                                case 1:
                                        {
                                        if ( 1 )
                                                return (-1);
                                        if ( ! yy_did_buffer_switch_on_eof )
                                                lyyrestart( lyyin );
                                        return input();
                                        }
                                case 0:
                                        yy_c_buf_p = lyytext + offset;
                                        break;
                                }
                        }
                }
        c = *(unsigned char *) yy_c_buf_p;
        *yy_c_buf_p = '\0';
        yy_hold_char = *++yy_c_buf_p;
        return c;
        }
void lyyrestart( FILE *input_file )
        {
        if ( ! yy_current_buffer )
                yy_current_buffer = lyy_create_buffer( lyyin, 16384 );
        lyy_init_buffer( yy_current_buffer, input_file );
        lyy_load_buffer_state();
        }
void lyy_switch_to_buffer( YY_BUFFER_STATE new_buffer )
        {
        if ( yy_current_buffer == new_buffer )
                return;
        if ( yy_current_buffer )
                {
                *yy_c_buf_p = yy_hold_char;
                yy_current_buffer->yy_buf_pos = yy_c_buf_p;
                yy_current_buffer->yy_n_chars = yy_n_chars;
                }
        yy_current_buffer = new_buffer;
        lyy_load_buffer_state();
        yy_did_buffer_switch_on_eof = 1;
        }
void lyy_load_buffer_state( void )
        {
        yy_n_chars = yy_current_buffer->yy_n_chars;
        lyytext = yy_c_buf_p = yy_current_buffer->yy_buf_pos;
        lyyin = yy_current_buffer->yy_input_file;
        yy_hold_char = *yy_c_buf_p;
        }
YY_BUFFER_STATE lyy_create_buffer( FILE *file, int size )
        {
        YY_BUFFER_STATE b;
        b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
        if ( ! b )
                yy_fatal_error( "out of dynamic memory in yy_create_buffer()" );
        b->yy_buf_size = size;
        b->yy_ch_buf = (char *) yy_flex_alloc( b->yy_buf_size + 2 );
        if ( ! b->yy_ch_buf )
                yy_fatal_error( "out of dynamic memory in yy_create_buffer()" );
        b->yy_is_our_buffer = 1;
        lyy_init_buffer( b, file );
        return b;
        }
void lyy_delete_buffer( YY_BUFFER_STATE b )
        {
        if ( ! b )
                return;
        if ( b == yy_current_buffer )
                yy_current_buffer = (YY_BUFFER_STATE) 0;
        if ( b->yy_is_our_buffer )
                yy_flex_free( (void *) b->yy_ch_buf );
        yy_flex_free( (void *) b );
        }
void lyy_init_buffer( YY_BUFFER_STATE b, FILE *file )
        {
        lyy_flush_buffer( b );
        b->yy_input_file = file;
        b->yy_fill_buffer = 1;
        b->yy_is_interactive = file ? (isatty( fileno(file) ) > 0) : 0;
        }
void lyy_flush_buffer( YY_BUFFER_STATE b )
        {
        if ( ! b )
                return;
        b->yy_n_chars = 0;
        b->yy_ch_buf[0] = 0;
        b->yy_ch_buf[1] = 0;
        b->yy_buf_pos = &b->yy_ch_buf[0];
        b->yy_at_bol = 1;
        b->yy_buffer_status = 0;
        if ( b == yy_current_buffer )
                lyy_load_buffer_state();
        }
YY_BUFFER_STATE lyy_scan_buffer( char *base, yy_size_t size )
        {
        YY_BUFFER_STATE b;
        if ( size < 2 ||
             base[size-2] != 0 ||
             base[size-1] != 0 )
                return 0;
        b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
        if ( ! b )
                yy_fatal_error( "out of dynamic memory in yy_scan_buffer()" );
        b->yy_buf_size = size - 2;
        b->yy_buf_pos = b->yy_ch_buf = base;
        b->yy_is_our_buffer = 0;
        b->yy_input_file = 0;
        b->yy_n_chars = b->yy_buf_size;
        b->yy_is_interactive = 0;
        b->yy_at_bol = 1;
        b->yy_fill_buffer = 0;
        b->yy_buffer_status = 0;
        lyy_switch_to_buffer( b );
        return b;
        }
YY_BUFFER_STATE lyy_scan_string( const char *yy_str )
        {
        int len;
        for ( len = 0; yy_str[len]; ++len )
                ;
        return lyy_scan_bytes( yy_str, len );
        }
YY_BUFFER_STATE lyy_scan_bytes( const char *bytes, int len )
        {
        YY_BUFFER_STATE b;
        char *buf;
        yy_size_t n;
        int i;
        n = len + 2;
        buf = (char *) yy_flex_alloc( n );
        if ( ! buf )
                yy_fatal_error( "out of dynamic memory in yy_scan_bytes()" );
        for ( i = 0; i < len; ++i )
                buf[i] = bytes[i];
        buf[len] = buf[len+1] = 0;
        b = lyy_scan_buffer( buf, n );
        if ( ! b )
                yy_fatal_error( "bad buffer in yy_scan_bytes()" );
        b->yy_is_our_buffer = 1;
        return b;
        }
static void yy_fatal_error( const char msg[] )
        {
        (void) fprintf( stderr, "%s\n", msg );
        exit( 2 );
        }
static void *yy_flex_alloc( yy_size_t size )
        {
        return (void *) malloc( size );
        }
static void *yy_flex_realloc( void *ptr, yy_size_t size )
        {
        return (void *) realloc( (char *) ptr, size );
        }
static void yy_flex_free( void *ptr )
        {
        free( ptr );
        }
const char CANON_IDENT_a1f71d5a2929938bd12ece9931f77509[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/lattice-lex.c";
