typedef int __sig_atomic_t;
typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
extern __inline int __sigismember (__const __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return (__set->__val[__word] & __mask) ? 1 : 0; }
extern __inline int __sigaddset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] |= __mask), 0); }
extern __inline int __sigdelset ( __sigset_t *__set, int __sig) { unsigned long int __mask = (((unsigned long int) 1) << (((__sig) - 1) % (8 * sizeof (unsigned long int)))); unsigned long int __word = (((__sig) - 1) / (8 * sizeof (unsigned long int))); return ((__set->__val[__word] &= ~__mask), 0); }
typedef __sig_atomic_t sig_atomic_t;
typedef __sigset_t sigset_t;
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
typedef void (*__sighandler_t) (int);
extern __sighandler_t __sysv_signal (int __sig, __sighandler_t __handler)
            ;
extern __sighandler_t signal (int __sig, __sighandler_t __handler) ;
extern int kill (__pid_t __pid, int __sig) ;
extern int killpg (__pid_t __pgrp, int __sig) ;
extern int raise (int __sig) ;
extern __sighandler_t ssignal (int __sig, __sighandler_t __handler) ;
extern int gsignal (int __sig) ;
extern void psignal (int __sig, __const char *__s) ;
extern int __sigpause (int __sig_or_mask, int __is_sig) ;
extern int sigpause (int __mask) ;
extern int sigblock (int __mask) ;
extern int sigsetmask (int __mask) ;
extern int siggetmask (void) ;
typedef __sighandler_t sig_t;
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
typedef struct siginfo
  {
    int si_signo;
    int si_errno;
    int si_code;
    union
      {
        int _pad[((128 / sizeof (int)) - 3)];
        struct
          {
            __pid_t si_pid;
            __uid_t si_uid;
          } _kill;
        struct
          {
            int si_tid;
            int si_overrun;
            sigval_t si_sigval;
          } _timer;
        struct
          {
            __pid_t si_pid;
            __uid_t si_uid;
            sigval_t si_sigval;
          } _rt;
        struct
          {
            __pid_t si_pid;
            __uid_t si_uid;
            int si_status;
            __clock_t si_utime;
            __clock_t si_stime;
          } _sigchld;
        struct
          {
            void *si_addr;
          } _sigfault;
        struct
          {
            long int si_band;
            int si_fd;
          } _sigpoll;
      } _sifields;
  } siginfo_t;
enum
{
  SI_ASYNCNL = -6,
  SI_SIGIO,
  SI_ASYNCIO,
  SI_MESGQ,
  SI_TIMER,
  SI_QUEUE,
  SI_USER,
  SI_KERNEL = 0x80
};
enum
{
  ILL_ILLOPC = 1,
  ILL_ILLOPN,
  ILL_ILLADR,
  ILL_ILLTRP,
  ILL_PRVOPC,
  ILL_PRVREG,
  ILL_COPROC,
  ILL_BADSTK
};
enum
{
  FPE_INTDIV = 1,
  FPE_INTOVF,
  FPE_FLTDIV,
  FPE_FLTOVF,
  FPE_FLTUND,
  FPE_FLTRES,
  FPE_FLTINV,
  FPE_FLTSUB
};
enum
{
  SEGV_MAPERR = 1,
  SEGV_ACCERR
};
enum
{
  BUS_ADRALN = 1,
  BUS_ADRERR,
  BUS_OBJERR
};
enum
{
  TRAP_BRKPT = 1,
  TRAP_TRACE
};
enum
{
  CLD_EXITED = 1,
  CLD_KILLED,
  CLD_DUMPED,
  CLD_TRAPPED,
  CLD_STOPPED,
  CLD_CONTINUED
};
enum
{
  POLL_IN = 1,
  POLL_OUT,
  POLL_MSG,
  POLL_ERR,
  POLL_PRI,
  POLL_HUP
};
typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;
    union
      {
        int _pad[((64 / sizeof (int)) - 3)];
        __pid_t _tid;
        struct
          {
            void (*_function) (sigval_t);
            void *_attribute;
          } _sigev_thread;
      } _sigev_un;
  } sigevent_t;
enum
{
  SIGEV_SIGNAL = 0,
  SIGEV_NONE,
  SIGEV_THREAD,
  SIGEV_THREAD_ID = 4
};
extern int sigemptyset (sigset_t *__set) ;
extern int sigfillset (sigset_t *__set) ;
extern int sigaddset (sigset_t *__set, int __signo) ;
extern int sigdelset (sigset_t *__set, int __signo) ;
extern int sigismember (__const sigset_t *__set, int __signo) ;
struct sigaction
  {
    union
      {
        __sighandler_t sa_handler;
        void (*sa_sigaction) (int, siginfo_t *, void *);
      }
    __sigaction_handler;
    __sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer) (void);
  };
extern int sigprocmask (int __how, __const sigset_t *__restrict __set,
                        sigset_t *__restrict __oset) ;
extern int sigsuspend (__const sigset_t *__set) ;
extern int sigaction (int __sig, __const struct sigaction *__restrict __act,
                      struct sigaction *__restrict __oact) ;
extern int sigpending (sigset_t *__set) ;
extern int sigwait (__const sigset_t *__restrict __set, int *__restrict __sig)
            ;
extern int sigwaitinfo (__const sigset_t *__restrict __set,
                        siginfo_t *__restrict __info) ;
extern int sigtimedwait (__const sigset_t *__restrict __set,
                         siginfo_t *__restrict __info,
                         __const struct timespec *__restrict __timeout)
            ;
extern int sigqueue (__pid_t __pid, int __sig, __const union sigval __val)
            ;
extern __const char *__const _sys_siglist[64];
extern __const char *__const sys_siglist[64];
struct sigvec
  {
    __sighandler_t sv_handler;
    int sv_mask;
    int sv_flags;
  };
extern int sigvec (int __sig, __const struct sigvec *__vec,
                   struct sigvec *__ovec) ;
struct _fpreg {
        unsigned short significand[4];
        unsigned short exponent;
};
struct _fpxreg {
        unsigned short significand[4];
        unsigned short exponent;
        unsigned short padding[3];
};
struct _xmmreg {
        unsigned long element[4];
};
struct _fpstate {
        unsigned long cw;
        unsigned long sw;
        unsigned long tag;
        unsigned long ipoff;
        unsigned long cssel;
        unsigned long dataoff;
        unsigned long datasel;
        struct _fpreg _st[8];
        unsigned short status;
        unsigned short magic;
        unsigned long _fxsr_env[6];
        unsigned long mxcsr;
        unsigned long reserved;
        struct _fpxreg _fxsr_st[8];
        struct _xmmreg _xmm[8];
        unsigned long padding[56];
};
struct sigcontext {
        unsigned short gs, __gsh;
        unsigned short fs, __fsh;
        unsigned short es, __esh;
        unsigned short ds, __dsh;
        unsigned long edi;
        unsigned long esi;
        unsigned long ebp;
        unsigned long esp;
        unsigned long ebx;
        unsigned long edx;
        unsigned long ecx;
        unsigned long eax;
        unsigned long trapno;
        unsigned long err;
        unsigned long eip;
        unsigned short cs, __csh;
        unsigned long eflags;
        unsigned long esp_at_signal;
        unsigned short ss, __ssh;
        struct _fpstate * fpstate;
        unsigned long oldmask;
        unsigned long cr2;
};
extern int sigreturn (struct sigcontext *__scp) ;
extern int siginterrupt (int __sig, int __interrupt) ;
struct sigstack
  {
    void *ss_sp;
    int ss_onstack;
  };
enum
{
  SS_ONSTACK = 1,
  SS_DISABLE
};
typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
extern int sigstack (struct sigstack *__ss, struct sigstack *__oss) ;
extern int sigaltstack (__const struct sigaltstack *__restrict __ss,
                        struct sigaltstack *__restrict __oss) ;
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
extern int pthread_sigmask (int __how,
                            __const __sigset_t *__restrict __newmask,
                            __sigset_t *__restrict __oldmask);
extern int pthread_kill (pthread_t __threadid, int __signo) ;
extern int __libc_current_sigrtmin (void) ;
extern int __libc_current_sigrtmax (void) ;
struct stat
  {
    __dev_t st_dev;
    unsigned short int __pad1;
    __ino_t st_ino;
    __mode_t st_mode;
    __nlink_t st_nlink;
    __uid_t st_uid;
    __gid_t st_gid;
    __dev_t st_rdev;
    unsigned short int __pad2;
    __off_t st_size;
    __blksize_t st_blksize;
    __blkcnt_t st_blocks;
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
    unsigned long int __unused4;
    unsigned long int __unused5;
  };
extern int stat (__const char *__restrict __file,
                 struct stat *__restrict __buf) ;
extern int fstat (int __fd, struct stat *__buf) ;
extern int lstat (__const char *__restrict __file,
                  struct stat *__restrict __buf) ;
extern int chmod (__const char *__file, __mode_t __mode) ;
extern int lchmod (__const char *__file, __mode_t __mode) ;
extern int fchmod (int __fd, __mode_t __mode) ;
extern __mode_t umask (__mode_t __mask) ;
extern int mkdir (__const char *__path, __mode_t __mode) ;
extern int mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
            ;
extern int mkfifo (__const char *__path, __mode_t __mode) ;
extern int __fxstat (int __ver, int __fildes, struct stat *__stat_buf) ;
extern int __xstat (int __ver, __const char *__filename,
                    struct stat *__stat_buf) ;
extern int __lxstat (int __ver, __const char *__filename,
                     struct stat *__stat_buf) ;
extern int __xmknod (int __ver, __const char *__path, __mode_t __mode,
                     __dev_t *__dev) ;
extern __inline__ int stat (__const char *__path,
                            struct stat *__statbuf)
{
  return __xstat (3, __path, __statbuf);
}
extern __inline__ int lstat (__const char *__path,
                             struct stat *__statbuf)
{
  return __lxstat (3, __path, __statbuf);
}
extern __inline__ int fstat (int __fd, struct stat *__statbuf)
{
  return __fxstat (3, __fd, __statbuf);
}
extern __inline__ int mknod (__const char *__path, __mode_t __mode,
                             __dev_t __dev)
{
  return __xmknod (1, __path, __mode, &__dev);
}
typedef struct
  {
    size_t gl_pathc;
    char **gl_pathv;
    size_t gl_offs;
    int gl_flags;
    void (*gl_closedir) (void *);
    void *(*gl_readdir) (void *);
    void * (*gl_opendir) (__const char *);
    int (*gl_lstat) (__const char *__restrict, void *__restrict);
    int (*gl_stat) (__const char *__restrict, void *__restrict);
  } glob_t;
extern int glob (__const char *__restrict __pattern, int __flags, int (*__errfunc) (__const char *, int), glob_t *__restrict __pglob) ;
extern void globfree (glob_t *__pglob) ;
typedef __time_t time_t;
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
struct timezone
  {
    int tz_minuteswest;
    int tz_dsttime;
  };
typedef struct timezone *__restrict __timezone_ptr_t;
extern int gettimeofday (struct timeval *__restrict __tv,
                         __timezone_ptr_t __tz) ;
extern int settimeofday (__const struct timeval *__tv,
                         __const struct timezone *__tz) ;
extern int adjtime (__const struct timeval *__delta,
                    struct timeval *__olddelta) ;
enum __itimer_which
  {
    ITIMER_REAL = 0,
    ITIMER_VIRTUAL = 1,
    ITIMER_PROF = 2
  };
struct itimerval
  {
    struct timeval it_interval;
    struct timeval it_value;
  };
typedef int __itimer_which_t;
extern int getitimer (__itimer_which_t __which,
                      struct itimerval *__value) ;
extern int setitimer (__itimer_which_t __which,
                      __const struct itimerval *__restrict __new,
                      struct itimerval *__restrict __old) ;
extern int utimes (__const char *__file, __const struct timeval __tvp[2])
            ;
extern int lutimes (__const char *__file, __const struct timeval __tvp[2])
            ;
extern int futimes (int fd, __const struct timeval __tvp[2]) ;
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
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
typedef __key_t key_t;
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
typedef __blkcnt_t blkcnt_t;
typedef __fsblkcnt_t fsblkcnt_t;
typedef __fsfilcnt_t fsfilcnt_t;
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
void unparse(FILE *to, declaration program) ;
void unparse_start(FILE *to);
void unparse_end(void) ;
struct Location output_location(void);
void output(char *format, ...) __attribute__((format (printf, 1, 2)));
void outputln(char *format, ...) __attribute__((format (printf, 1, 2)));
const char *binary_op_name(ast_kind kind);
void prt_toplevel_declaration(declaration d);
void prt_nelements(expression array);
void prt_expression(expression e, int context_priority);
void prt_regionof_name(const char *name);
void prt_regionof(expression e);
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
void enter_pam_mode(void);
void pam_add_file(const char *name);
void pam_add_error(const char *func, location loc, severity s,
                   const char *fmt, va_list args);
void pam_add_overlay_file(location loc, const char *idname, qtype qt);
void pam_add_overlay_anchor(location loc);
void pam_add_overlay_lin(location loc, store store, aloc al);
void pam_add_overlay_flow_sensitive(location loc, qtype qt, store s);
void pam_add_overlay_file_rinf(location loc, const char *idname, rinf_const c);
void print_results(void);
void init_pam(void);
typedef struct timer
{
  struct timeval start;
  struct timeval elapsed;
} timer;
void start_timer(timer *t);
void end_timer(timer *t);
char *timer_to_ascii(timer *t);
long get_memusage(void);
void rinf_init_analyze(void);
void rinf_analyze(declaration program);
void rinf_finish_analyze(void);
void report_rinf(location loc, severity sev, const char *format, ...);
void rinf_traverse_globals(traverse_global_fn f, void *arg);
void rinf_traverse_globals_sorted(traverse_global_fn f, void *arg);
char *progname;
int copy_argc;
char **copy_argv;
bool in_prelude;
extern int num_hotspots;
dd_list parsed_files;
static struct { char *string; int *variable; int on_value;} f_options[] =
{
  {"volatile", &flag_volatile, 1},
  {"volatile-global", &flag_volatile_global, 1},
  {"syntax-only", &flag_syntax_only, 1},
  {"parse-only", &flag_parse_only, 1},
  {"pack-struct", &flag_pack_struct, 1},
  {"pam-mode", &flag_pam_mode, 1},
  {"pam-html", &flag_pam_html, 1},
  {"print-quals-graph", &flag_print_quals_graph, 1},
  {"strict-const", &flag_strict_const, 1},
  {"print-results", &flag_print_results, 1},
  {"casts-preserve", &flag_casts_preserve, 1},
  {"use-const-subtyping", &flag_const_subtyping, 1},
  {"flow-sensitive", &flag_flow_sensitive, 1},
  {"force-flow-sensitive", &flag_force_flow_sensitive, 1},
  {"print-lin", &flag_print_lin, 1},
  {"driver", &flag_driver, 1},
  {"casts-warn", &flag_casts_warn, 1},
  {"ugly", &flag_ugly, 1},
  {"stats", &flag_statistics, 1},
  {"print-stores", &flag_print_stores, 1},
};
static char *lang_options[] =
{
  "-ansi",
  "-fallow-single-precision",
  "-fsigned-bitfields",
  "-funsigned-bitfields",
  "-fno-signed-bitfields",
  "-fno-unsigned-bitfields",
  "-fsigned-char",
  "-funsigned-char",
  "-fno-signed-char",
  "-fno-unsigned-char",
  "-ftraditional",
  "-traditional",
  "-fnotraditional",
  "-fno-traditional",
  "-fasm",
  "-fno-asm",
  "-fbuiltin",
  "-fno-builtin",
  "-fhosted",
  "-fno-hosted",
  "-ffreestanding",
  "-fno-freestanding",
  "-fcond-mismatch",
  "-fno-cond-mismatch",
  "-fdollars-in-identifiers",
  "-fno-dollars-in-identifiers",
  "-fident",
  "-fno-ident",
  "-fshort-double",
  "-fno-short-double",
  "-fshort-enums",
  "-fno-short-enums",
  "-Wall",
  "-Wbad-function-cast",
  "-Wno-bad-function-cast",
  "-Wcast-qual",
  "-Wno-cast-qual",
  "-Wchar-subscripts",
  "-Wno-char-subscripts",
  "-Wcomment",
  "-Wno-comment",
  "-Wcomments",
  "-Wno-comments",
  "-Wconversion",
  "-Wno-conversion",
  "-Wformat",
  "-Wno-format",
  "-Wimport",
  "-Wno-import",
  "-Wimplicit-function-declaration",
  "-Wno-implicit-function-declaration",
  "-Werror-implicit-function-declaration",
  "-Wimplicit-int",
  "-Wno-implicit-int",
  "-Wimplicit",
  "-Wno-implicit",
  "-Wmain",
  "-Wno-main",
  "-Wmissing-braces",
  "-Wno-missing-braces",
  "-Wmissing-declarations",
  "-Wno-missing-declarations",
  "-Wmissing-prototypes",
  "-Wno-missing-prototypes",
  "-Wnested-externs",
  "-Wno-nested-externs",
  "-Wparentheses",
  "-Wno-parentheses",
  "-Wpointer-arith",
  "-Wno-pointer-arith",
  "-Wredundant-decls",
  "-Wno-redundant-decls",
  "-Wsign-compare",
  "-Wno-sign-compare",
  "-Wstrict-prototypes",
  "-Wno-strict-prototypes",
  "-Wtraditional",
  "-Wno-traditional",
  "-Wtrigraphs",
  "-Wno-trigraphs",
  "-Wundef",
  "-Wno-undef",
  "-Wwrite-strings",
  "-Wno-write-strings",
  0
};
static struct { char *string; int *variable; int on_value;} W_options[] =
{
  {"unused", &warn_unused, 1},
  {"error", &warnings_are_errors, 1},
  {"shadow", &warn_shadow, 1},
  {"switch", &warn_switch, 1},
  {"aggregate-return", &warn_aggregate_return, 1},
  {"cast-align", &warn_cast_align, 1},
  {"uninitialized", &warn_uninitialized, 1},
  {"inline", &warn_inline, 1}
};
typedef struct parsed_file
{
  const char *name;
  declaration program;
} *parsed_file;
static timer total_time;
static long memusage;
static void pipe_closed (int signo)
{
  fatal("output pipe has been closed");
}
void outofmemory(void)
{
  fprintf(stderr, "Out of memory - exiting\n");
  exit(33);
}
static void add_program_files(const char *file_list, dd_list files)
{
  FILE *list = fopen(file_list, "r");
  if(!list)
    {
      error("unable to open %s", file_list);
      return;
    }
  while(!ferror(list) && !feof(list))
    {
      char *newline = ((void *)0);
      char name[4096] = {0};
      if(!fgets(name, 4096, list))
        break;
      newline = (__extension__ (__builtin_constant_p ((int)'\n') && ((int)'\n') == '\0' ? (char *) __rawmemchr (name, (int)'\n') : strchr (name, (int)'\n')));
      if (newline)
        *newline = '\0';
      dd_add_last(parse_region, files, (((void)0), __rc_rstrdup)(parse_region, name));
    }
}
static void compile_file(char *name)
{
  set_nomem_handler(outofmemory);
  if (parse(name) != 0)
    {
      if (errorcount == 0)
        fprintf (stderr, "Errors detected in input file (your bison.simple is out of date)");
    }
  if (errorcount == 0)
    {
      if (the_program)
        {
          parsed_file pf;
          pf = (sizeof(struct parsed_file) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((parse_region), sizeof(struct parsed_file)) : (((void)0), __rc_typed_ralloc)((parse_region), sizeof(struct parsed_file), 0));
          pf->name = name;
          pf->program = the_program;
          dd_add_last(parse_region, parsed_files, pf);
          rinf_analyze(the_program);
        }
    }
}
static void c_decode_option(char *p)
{
  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-ftraditional") && (__s1_len = strlen (p), __s2_len = strlen ("-ftraditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-ftraditional") + 1) - (size_t)(const void *)("-ftraditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-ftraditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-ftraditional") && ((size_t)(const void *)(("-ftraditional") + 1) - (size_t)(const void *)("-ftraditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-ftraditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-ftraditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-ftraditional") && ((size_t)(const void *)(("-ftraditional") + 1) - (size_t)(const void *)("-ftraditional") == 1) && (__s2_len = strlen ("-ftraditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-ftraditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-ftraditional"))[3]); } } __result; }))) : strcmp (p, "-ftraditional")))); }) || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-traditional") && (__s1_len = strlen (p), __s2_len = strlen ("-traditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-traditional") + 1) - (size_t)(const void *)("-traditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-traditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-traditional") && ((size_t)(const void *)(("-traditional") + 1) - (size_t)(const void *)("-traditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-traditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-traditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-traditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-traditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-traditional") && ((size_t)(const void *)(("-traditional") + 1) - (size_t)(const void *)("-traditional") == 1) && (__s2_len = strlen ("-traditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-traditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-traditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-traditional"))[3]); } } __result; }))) : strcmp (p, "-traditional")))); }))
    {
      flag_traditional = 1;
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fallow-single-precision") && (__s1_len = strlen (p), __s2_len = strlen ("-fallow-single-precision"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fallow-single-precision") + 1) - (size_t)(const void *)("-fallow-single-precision") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fallow-single-precision"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fallow-single-precision") && ((size_t)(const void *)(("-fallow-single-precision") + 1) - (size_t)(const void *)("-fallow-single-precision") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fallow-single-precision"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fallow-single-precision"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fallow-single-precision") && ((size_t)(const void *)(("-fallow-single-precision") + 1) - (size_t)(const void *)("-fallow-single-precision") == 1) && (__s2_len = strlen ("-fallow-single-precision"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fallow-single-precision"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fallow-single-precision"))[3]); } } __result; }))) : strcmp (p, "-fallow-single-precision")))); }))
    flag_allow_single_precision = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fnotraditional") && (__s1_len = strlen (p), __s2_len = strlen ("-fnotraditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fnotraditional") + 1) - (size_t)(const void *)("-fnotraditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fnotraditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fnotraditional") && ((size_t)(const void *)(("-fnotraditional") + 1) - (size_t)(const void *)("-fnotraditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fnotraditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fnotraditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fnotraditional") && ((size_t)(const void *)(("-fnotraditional") + 1) - (size_t)(const void *)("-fnotraditional") == 1) && (__s2_len = strlen ("-fnotraditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fnotraditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fnotraditional"))[3]); } } __result; }))) : strcmp (p, "-fnotraditional")))); }) || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-traditional") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-traditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-traditional") + 1) - (size_t)(const void *)("-fno-traditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-traditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-traditional") && ((size_t)(const void *)(("-fno-traditional") + 1) - (size_t)(const void *)("-fno-traditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-traditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-traditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-traditional") && ((size_t)(const void *)(("-fno-traditional") + 1) - (size_t)(const void *)("-fno-traditional") == 1) && (__s2_len = strlen ("-fno-traditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-traditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-traditional"))[3]); } } __result; }))) : strcmp (p, "-fno-traditional")))); }))
    {
      flag_traditional = 0;
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fdollars-in-identifiers") && (__s1_len = strlen (p), __s2_len = strlen ("-fdollars-in-identifiers"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fdollars-in-identifiers") + 1) - (size_t)(const void *)("-fdollars-in-identifiers") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fdollars-in-identifiers"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fdollars-in-identifiers") && ((size_t)(const void *)(("-fdollars-in-identifiers") + 1) - (size_t)(const void *)("-fdollars-in-identifiers") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fdollars-in-identifiers"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fdollars-in-identifiers") && ((size_t)(const void *)(("-fdollars-in-identifiers") + 1) - (size_t)(const void *)("-fdollars-in-identifiers") == 1) && (__s2_len = strlen ("-fdollars-in-identifiers"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fdollars-in-identifiers"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fdollars-in-identifiers"))[3]); } } __result; }))) : strcmp (p, "-fdollars-in-identifiers")))); }))
    dollars_in_ident = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-dollars-in-identifiers") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-dollars-in-identifiers"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-dollars-in-identifiers") + 1) - (size_t)(const void *)("-fno-dollars-in-identifiers") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-dollars-in-identifiers"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-dollars-in-identifiers") && ((size_t)(const void *)(("-fno-dollars-in-identifiers") + 1) - (size_t)(const void *)("-fno-dollars-in-identifiers") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-dollars-in-identifiers"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-dollars-in-identifiers") && ((size_t)(const void *)(("-fno-dollars-in-identifiers") + 1) - (size_t)(const void *)("-fno-dollars-in-identifiers") == 1) && (__s2_len = strlen ("-fno-dollars-in-identifiers"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-dollars-in-identifiers"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-dollars-in-identifiers"))[3]); } } __result; }))) : strcmp (p, "-fno-dollars-in-identifiers")))); }))
    dollars_in_ident = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fsigned-char") && (__s1_len = strlen (p), __s2_len = strlen ("-fsigned-char"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fsigned-char") + 1) - (size_t)(const void *)("-fsigned-char") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fsigned-char"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fsigned-char") && ((size_t)(const void *)(("-fsigned-char") + 1) - (size_t)(const void *)("-fsigned-char") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fsigned-char"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fsigned-char"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fsigned-char") && ((size_t)(const void *)(("-fsigned-char") + 1) - (size_t)(const void *)("-fsigned-char") == 1) && (__s2_len = strlen ("-fsigned-char"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fsigned-char"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fsigned-char"))[3]); } } __result; }))) : strcmp (p, "-fsigned-char")))); }))
    flag_signed_char = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-funsigned-char") && (__s1_len = strlen (p), __s2_len = strlen ("-funsigned-char"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-funsigned-char") + 1) - (size_t)(const void *)("-funsigned-char") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-funsigned-char"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-funsigned-char") && ((size_t)(const void *)(("-funsigned-char") + 1) - (size_t)(const void *)("-funsigned-char") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-funsigned-char"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-funsigned-char"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-funsigned-char") && ((size_t)(const void *)(("-funsigned-char") + 1) - (size_t)(const void *)("-funsigned-char") == 1) && (__s2_len = strlen ("-funsigned-char"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-funsigned-char"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-funsigned-char"))[3]); } } __result; }))) : strcmp (p, "-funsigned-char")))); }))
    flag_signed_char = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-signed-char") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-signed-char"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-signed-char") + 1) - (size_t)(const void *)("-fno-signed-char") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-signed-char"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-signed-char") && ((size_t)(const void *)(("-fno-signed-char") + 1) - (size_t)(const void *)("-fno-signed-char") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-signed-char"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-signed-char"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-signed-char") && ((size_t)(const void *)(("-fno-signed-char") + 1) - (size_t)(const void *)("-fno-signed-char") == 1) && (__s2_len = strlen ("-fno-signed-char"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-signed-char"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-char"))[3]); } } __result; }))) : strcmp (p, "-fno-signed-char")))); }))
    flag_signed_char = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-unsigned-char") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-unsigned-char"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-unsigned-char") + 1) - (size_t)(const void *)("-fno-unsigned-char") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-unsigned-char"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-unsigned-char") && ((size_t)(const void *)(("-fno-unsigned-char") + 1) - (size_t)(const void *)("-fno-unsigned-char") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-unsigned-char"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-unsigned-char"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-unsigned-char") && ((size_t)(const void *)(("-fno-unsigned-char") + 1) - (size_t)(const void *)("-fno-unsigned-char") == 1) && (__s2_len = strlen ("-fno-unsigned-char"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-unsigned-char"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-char"))[3]); } } __result; }))) : strcmp (p, "-fno-unsigned-char")))); }))
    flag_signed_char = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fsigned-bitfields") && (__s1_len = strlen (p), __s2_len = strlen ("-fsigned-bitfields"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fsigned-bitfields") + 1) - (size_t)(const void *)("-fsigned-bitfields") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fsigned-bitfields"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fsigned-bitfields") && ((size_t)(const void *)(("-fsigned-bitfields") + 1) - (size_t)(const void *)("-fsigned-bitfields") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fsigned-bitfields"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fsigned-bitfields"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fsigned-bitfields") && ((size_t)(const void *)(("-fsigned-bitfields") + 1) - (size_t)(const void *)("-fsigned-bitfields") == 1) && (__s2_len = strlen ("-fsigned-bitfields"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fsigned-bitfields"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fsigned-bitfields"))[3]); } } __result; }))) : strcmp (p, "-fsigned-bitfields")))); })
           || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-unsigned-bitfields") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-unsigned-bitfields"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-unsigned-bitfields") + 1) - (size_t)(const void *)("-fno-unsigned-bitfields") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-unsigned-bitfields"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-unsigned-bitfields") && ((size_t)(const void *)(("-fno-unsigned-bitfields") + 1) - (size_t)(const void *)("-fno-unsigned-bitfields") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-unsigned-bitfields"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-unsigned-bitfields") && ((size_t)(const void *)(("-fno-unsigned-bitfields") + 1) - (size_t)(const void *)("-fno-unsigned-bitfields") == 1) && (__s2_len = strlen ("-fno-unsigned-bitfields"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-unsigned-bitfields"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-unsigned-bitfields"))[3]); } } __result; }))) : strcmp (p, "-fno-unsigned-bitfields")))); }))
    {
      flag_signed_bitfields = 1;
      explicit_flag_signed_bitfields = 1;
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-funsigned-bitfields") && (__s1_len = strlen (p), __s2_len = strlen ("-funsigned-bitfields"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-funsigned-bitfields") + 1) - (size_t)(const void *)("-funsigned-bitfields") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-funsigned-bitfields"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-funsigned-bitfields") && ((size_t)(const void *)(("-funsigned-bitfields") + 1) - (size_t)(const void *)("-funsigned-bitfields") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-funsigned-bitfields"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-funsigned-bitfields"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-funsigned-bitfields") && ((size_t)(const void *)(("-funsigned-bitfields") + 1) - (size_t)(const void *)("-funsigned-bitfields") == 1) && (__s2_len = strlen ("-funsigned-bitfields"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-funsigned-bitfields"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-funsigned-bitfields"))[3]); } } __result; }))) : strcmp (p, "-funsigned-bitfields")))); })
           || !__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-signed-bitfields") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-signed-bitfields"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-signed-bitfields") + 1) - (size_t)(const void *)("-fno-signed-bitfields") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-signed-bitfields"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-signed-bitfields") && ((size_t)(const void *)(("-fno-signed-bitfields") + 1) - (size_t)(const void *)("-fno-signed-bitfields") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-signed-bitfields"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-signed-bitfields"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-signed-bitfields") && ((size_t)(const void *)(("-fno-signed-bitfields") + 1) - (size_t)(const void *)("-fno-signed-bitfields") == 1) && (__s2_len = strlen ("-fno-signed-bitfields"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-signed-bitfields"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-signed-bitfields"))[3]); } } __result; }))) : strcmp (p, "-fno-signed-bitfields")))); }))
    {
      flag_signed_bitfields = 0;
      explicit_flag_signed_bitfields = 1;
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fshort-enums") && (__s1_len = strlen (p), __s2_len = strlen ("-fshort-enums"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fshort-enums") + 1) - (size_t)(const void *)("-fshort-enums") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fshort-enums"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fshort-enums") && ((size_t)(const void *)(("-fshort-enums") + 1) - (size_t)(const void *)("-fshort-enums") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fshort-enums"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fshort-enums"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fshort-enums") && ((size_t)(const void *)(("-fshort-enums") + 1) - (size_t)(const void *)("-fshort-enums") == 1) && (__s2_len = strlen ("-fshort-enums"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fshort-enums"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fshort-enums"))[3]); } } __result; }))) : strcmp (p, "-fshort-enums")))); }))
    flag_short_enums = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-short-enums") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-short-enums"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-short-enums") + 1) - (size_t)(const void *)("-fno-short-enums") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-short-enums"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-short-enums") && ((size_t)(const void *)(("-fno-short-enums") + 1) - (size_t)(const void *)("-fno-short-enums") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-short-enums"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-short-enums"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-short-enums") && ((size_t)(const void *)(("-fno-short-enums") + 1) - (size_t)(const void *)("-fno-short-enums") == 1) && (__s2_len = strlen ("-fno-short-enums"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-short-enums"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-short-enums"))[3]); } } __result; }))) : strcmp (p, "-fno-short-enums")))); }))
    flag_short_enums = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fcond-mismatch") && (__s1_len = strlen (p), __s2_len = strlen ("-fcond-mismatch"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fcond-mismatch") + 1) - (size_t)(const void *)("-fcond-mismatch") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fcond-mismatch"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fcond-mismatch") && ((size_t)(const void *)(("-fcond-mismatch") + 1) - (size_t)(const void *)("-fcond-mismatch") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fcond-mismatch"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fcond-mismatch"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fcond-mismatch") && ((size_t)(const void *)(("-fcond-mismatch") + 1) - (size_t)(const void *)("-fcond-mismatch") == 1) && (__s2_len = strlen ("-fcond-mismatch"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fcond-mismatch"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fcond-mismatch"))[3]); } } __result; }))) : strcmp (p, "-fcond-mismatch")))); }))
    flag_cond_mismatch = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-cond-mismatch") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-cond-mismatch"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-cond-mismatch") + 1) - (size_t)(const void *)("-fno-cond-mismatch") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-cond-mismatch"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-cond-mismatch") && ((size_t)(const void *)(("-fno-cond-mismatch") + 1) - (size_t)(const void *)("-fno-cond-mismatch") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-cond-mismatch"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-cond-mismatch"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-cond-mismatch") && ((size_t)(const void *)(("-fno-cond-mismatch") + 1) - (size_t)(const void *)("-fno-cond-mismatch") == 1) && (__s2_len = strlen ("-fno-cond-mismatch"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-cond-mismatch"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-cond-mismatch"))[3]); } } __result; }))) : strcmp (p, "-fno-cond-mismatch")))); }))
    flag_cond_mismatch = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fasm") && (__s1_len = strlen (p), __s2_len = strlen ("-fasm"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fasm") + 1) - (size_t)(const void *)("-fasm") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fasm"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fasm") && ((size_t)(const void *)(("-fasm") + 1) - (size_t)(const void *)("-fasm") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fasm"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fasm"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fasm"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fasm"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fasm"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fasm") && ((size_t)(const void *)(("-fasm") + 1) - (size_t)(const void *)("-fasm") == 1) && (__s2_len = strlen ("-fasm"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fasm"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fasm"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fasm"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fasm"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fasm"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fasm"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fasm"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fasm"))[3]); } } __result; }))) : strcmp (p, "-fasm")))); }))
    flag_no_asm = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-fno-asm") && (__s1_len = strlen (p), __s2_len = strlen ("-fno-asm"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-fno-asm") + 1) - (size_t)(const void *)("-fno-asm") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-fno-asm"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-fno-asm") && ((size_t)(const void *)(("-fno-asm") + 1) - (size_t)(const void *)("-fno-asm") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-asm"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-fno-asm"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-fno-asm") && ((size_t)(const void *)(("-fno-asm") + 1) - (size_t)(const void *)("-fno-asm") == 1) && (__s2_len = strlen ("-fno-asm"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-fno-asm"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-fno-asm"))[3]); } } __result; }))) : strcmp (p, "-fno-asm")))); }))
    flag_no_asm = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-ansi") && (__s1_len = strlen (p), __s2_len = strlen ("-ansi"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-ansi") + 1) - (size_t)(const void *)("-ansi") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-ansi"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-ansi") && ((size_t)(const void *)(("-ansi") + 1) - (size_t)(const void *)("-ansi") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-ansi"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-ansi"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-ansi"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-ansi"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-ansi"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-ansi") && ((size_t)(const void *)(("-ansi") + 1) - (size_t)(const void *)("-ansi") == 1) && (__s2_len = strlen ("-ansi"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-ansi"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-ansi"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-ansi"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-ansi"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-ansi"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-ansi"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-ansi"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-ansi"))[3]); } } __result; }))) : strcmp (p, "-ansi")))); }))
    flag_no_asm = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Werror-implicit-function-declaration") && (__s1_len = strlen (p), __s2_len = strlen ("-Werror-implicit-function-declaration"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Werror-implicit-function-declaration") + 1) - (size_t)(const void *)("-Werror-implicit-function-declaration") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Werror-implicit-function-declaration"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Werror-implicit-function-declaration") && ((size_t)(const void *)(("-Werror-implicit-function-declaration") + 1) - (size_t)(const void *)("-Werror-implicit-function-declaration") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Werror-implicit-function-declaration"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Werror-implicit-function-declaration") && ((size_t)(const void *)(("-Werror-implicit-function-declaration") + 1) - (size_t)(const void *)("-Werror-implicit-function-declaration") == 1) && (__s2_len = strlen ("-Werror-implicit-function-declaration"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Werror-implicit-function-declaration"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Werror-implicit-function-declaration"))[3]); } } __result; }))) : strcmp (p, "-Werror-implicit-function-declaration")))); }))
    mesg_implicit_function_declaration = 2;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wimplicit-function-declaration") && (__s1_len = strlen (p), __s2_len = strlen ("-Wimplicit-function-declaration"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wimplicit-function-declaration") + 1) - (size_t)(const void *)("-Wimplicit-function-declaration") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wimplicit-function-declaration"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wimplicit-function-declaration") && ((size_t)(const void *)(("-Wimplicit-function-declaration") + 1) - (size_t)(const void *)("-Wimplicit-function-declaration") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit-function-declaration"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wimplicit-function-declaration") && ((size_t)(const void *)(("-Wimplicit-function-declaration") + 1) - (size_t)(const void *)("-Wimplicit-function-declaration") == 1) && (__s2_len = strlen ("-Wimplicit-function-declaration"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit-function-declaration"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-function-declaration"))[3]); } } __result; }))) : strcmp (p, "-Wimplicit-function-declaration")))); }))
    mesg_implicit_function_declaration = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-implicit-function-declaration") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-implicit-function-declaration"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-implicit-function-declaration") + 1) - (size_t)(const void *)("-Wno-implicit-function-declaration") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-implicit-function-declaration"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-implicit-function-declaration") && ((size_t)(const void *)(("-Wno-implicit-function-declaration") + 1) - (size_t)(const void *)("-Wno-implicit-function-declaration") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit-function-declaration"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-implicit-function-declaration") && ((size_t)(const void *)(("-Wno-implicit-function-declaration") + 1) - (size_t)(const void *)("-Wno-implicit-function-declaration") == 1) && (__s2_len = strlen ("-Wno-implicit-function-declaration"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit-function-declaration"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-function-declaration"))[3]); } } __result; }))) : strcmp (p, "-Wno-implicit-function-declaration")))); }))
    mesg_implicit_function_declaration = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wimplicit-int") && (__s1_len = strlen (p), __s2_len = strlen ("-Wimplicit-int"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wimplicit-int") + 1) - (size_t)(const void *)("-Wimplicit-int") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wimplicit-int"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wimplicit-int") && ((size_t)(const void *)(("-Wimplicit-int") + 1) - (size_t)(const void *)("-Wimplicit-int") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit-int"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wimplicit-int"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wimplicit-int") && ((size_t)(const void *)(("-Wimplicit-int") + 1) - (size_t)(const void *)("-Wimplicit-int") == 1) && (__s2_len = strlen ("-Wimplicit-int"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit-int"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit-int"))[3]); } } __result; }))) : strcmp (p, "-Wimplicit-int")))); }))
    warn_implicit_int = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-implicit-int") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-implicit-int"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-implicit-int") + 1) - (size_t)(const void *)("-Wno-implicit-int") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-implicit-int"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-implicit-int") && ((size_t)(const void *)(("-Wno-implicit-int") + 1) - (size_t)(const void *)("-Wno-implicit-int") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit-int"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-implicit-int"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-implicit-int") && ((size_t)(const void *)(("-Wno-implicit-int") + 1) - (size_t)(const void *)("-Wno-implicit-int") == 1) && (__s2_len = strlen ("-Wno-implicit-int"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit-int"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit-int"))[3]); } } __result; }))) : strcmp (p, "-Wno-implicit-int")))); }))
    warn_implicit_int = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wimplicit") && (__s1_len = strlen (p), __s2_len = strlen ("-Wimplicit"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wimplicit") + 1) - (size_t)(const void *)("-Wimplicit") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wimplicit"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wimplicit") && ((size_t)(const void *)(("-Wimplicit") + 1) - (size_t)(const void *)("-Wimplicit") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wimplicit"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wimplicit") && ((size_t)(const void *)(("-Wimplicit") + 1) - (size_t)(const void *)("-Wimplicit") == 1) && (__s2_len = strlen ("-Wimplicit"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wimplicit"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wimplicit"))[3]); } } __result; }))) : strcmp (p, "-Wimplicit")))); }))
    {
      warn_implicit_int = 1;
      if (mesg_implicit_function_declaration != 2)
        mesg_implicit_function_declaration = 1;
    }
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-implicit") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-implicit"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-implicit") + 1) - (size_t)(const void *)("-Wno-implicit") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-implicit"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-implicit") && ((size_t)(const void *)(("-Wno-implicit") + 1) - (size_t)(const void *)("-Wno-implicit") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-implicit"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-implicit") && ((size_t)(const void *)(("-Wno-implicit") + 1) - (size_t)(const void *)("-Wno-implicit") == 1) && (__s2_len = strlen ("-Wno-implicit"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-implicit"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-implicit"))[3]); } } __result; }))) : strcmp (p, "-Wno-implicit")))); }))
    warn_implicit_int = 0, mesg_implicit_function_declaration = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wwrite-strings") && (__s1_len = strlen (p), __s2_len = strlen ("-Wwrite-strings"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wwrite-strings") + 1) - (size_t)(const void *)("-Wwrite-strings") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wwrite-strings"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wwrite-strings") && ((size_t)(const void *)(("-Wwrite-strings") + 1) - (size_t)(const void *)("-Wwrite-strings") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wwrite-strings"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wwrite-strings"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wwrite-strings") && ((size_t)(const void *)(("-Wwrite-strings") + 1) - (size_t)(const void *)("-Wwrite-strings") == 1) && (__s2_len = strlen ("-Wwrite-strings"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wwrite-strings"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wwrite-strings"))[3]); } } __result; }))) : strcmp (p, "-Wwrite-strings")))); }))
    warn_write_strings = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-write-strings") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-write-strings"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-write-strings") + 1) - (size_t)(const void *)("-Wno-write-strings") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-write-strings"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-write-strings") && ((size_t)(const void *)(("-Wno-write-strings") + 1) - (size_t)(const void *)("-Wno-write-strings") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-write-strings"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-write-strings"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-write-strings") && ((size_t)(const void *)(("-Wno-write-strings") + 1) - (size_t)(const void *)("-Wno-write-strings") == 1) && (__s2_len = strlen ("-Wno-write-strings"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-write-strings"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-write-strings"))[3]); } } __result; }))) : strcmp (p, "-Wno-write-strings")))); }))
    warn_write_strings = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wcast-qual") && (__s1_len = strlen (p), __s2_len = strlen ("-Wcast-qual"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wcast-qual") + 1) - (size_t)(const void *)("-Wcast-qual") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wcast-qual"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wcast-qual") && ((size_t)(const void *)(("-Wcast-qual") + 1) - (size_t)(const void *)("-Wcast-qual") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wcast-qual"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wcast-qual"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wcast-qual") && ((size_t)(const void *)(("-Wcast-qual") + 1) - (size_t)(const void *)("-Wcast-qual") == 1) && (__s2_len = strlen ("-Wcast-qual"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wcast-qual"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wcast-qual"))[3]); } } __result; }))) : strcmp (p, "-Wcast-qual")))); }))
    warn_cast_qual = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-cast-qual") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-cast-qual"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-cast-qual") + 1) - (size_t)(const void *)("-Wno-cast-qual") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-cast-qual"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-cast-qual") && ((size_t)(const void *)(("-Wno-cast-qual") + 1) - (size_t)(const void *)("-Wno-cast-qual") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-cast-qual"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-cast-qual"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-cast-qual") && ((size_t)(const void *)(("-Wno-cast-qual") + 1) - (size_t)(const void *)("-Wno-cast-qual") == 1) && (__s2_len = strlen ("-Wno-cast-qual"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-cast-qual"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-cast-qual"))[3]); } } __result; }))) : strcmp (p, "-Wno-cast-qual")))); }))
    warn_cast_qual = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wbad-function-cast") && (__s1_len = strlen (p), __s2_len = strlen ("-Wbad-function-cast"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wbad-function-cast") + 1) - (size_t)(const void *)("-Wbad-function-cast") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wbad-function-cast"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wbad-function-cast") && ((size_t)(const void *)(("-Wbad-function-cast") + 1) - (size_t)(const void *)("-Wbad-function-cast") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wbad-function-cast"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wbad-function-cast"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wbad-function-cast") && ((size_t)(const void *)(("-Wbad-function-cast") + 1) - (size_t)(const void *)("-Wbad-function-cast") == 1) && (__s2_len = strlen ("-Wbad-function-cast"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wbad-function-cast"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wbad-function-cast"))[3]); } } __result; }))) : strcmp (p, "-Wbad-function-cast")))); }))
    warn_bad_function_cast = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-bad-function-cast") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-bad-function-cast"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-bad-function-cast") + 1) - (size_t)(const void *)("-Wno-bad-function-cast") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-bad-function-cast"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-bad-function-cast") && ((size_t)(const void *)(("-Wno-bad-function-cast") + 1) - (size_t)(const void *)("-Wno-bad-function-cast") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-bad-function-cast"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-bad-function-cast") && ((size_t)(const void *)(("-Wno-bad-function-cast") + 1) - (size_t)(const void *)("-Wno-bad-function-cast") == 1) && (__s2_len = strlen ("-Wno-bad-function-cast"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-bad-function-cast"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-bad-function-cast"))[3]); } } __result; }))) : strcmp (p, "-Wno-bad-function-cast")))); }))
    warn_bad_function_cast = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wpointer-arith") && (__s1_len = strlen (p), __s2_len = strlen ("-Wpointer-arith"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wpointer-arith") + 1) - (size_t)(const void *)("-Wpointer-arith") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wpointer-arith"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wpointer-arith") && ((size_t)(const void *)(("-Wpointer-arith") + 1) - (size_t)(const void *)("-Wpointer-arith") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wpointer-arith"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wpointer-arith"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wpointer-arith") && ((size_t)(const void *)(("-Wpointer-arith") + 1) - (size_t)(const void *)("-Wpointer-arith") == 1) && (__s2_len = strlen ("-Wpointer-arith"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wpointer-arith"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wpointer-arith"))[3]); } } __result; }))) : strcmp (p, "-Wpointer-arith")))); }))
    warn_pointer_arith = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-pointer-arith") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-pointer-arith"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-pointer-arith") + 1) - (size_t)(const void *)("-Wno-pointer-arith") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-pointer-arith"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-pointer-arith") && ((size_t)(const void *)(("-Wno-pointer-arith") + 1) - (size_t)(const void *)("-Wno-pointer-arith") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-pointer-arith"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-pointer-arith"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-pointer-arith") && ((size_t)(const void *)(("-Wno-pointer-arith") + 1) - (size_t)(const void *)("-Wno-pointer-arith") == 1) && (__s2_len = strlen ("-Wno-pointer-arith"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-pointer-arith"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-pointer-arith"))[3]); } } __result; }))) : strcmp (p, "-Wno-pointer-arith")))); }))
    warn_pointer_arith = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wstrict-prototypes") && (__s1_len = strlen (p), __s2_len = strlen ("-Wstrict-prototypes"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wstrict-prototypes") + 1) - (size_t)(const void *)("-Wstrict-prototypes") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wstrict-prototypes"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wstrict-prototypes") && ((size_t)(const void *)(("-Wstrict-prototypes") + 1) - (size_t)(const void *)("-Wstrict-prototypes") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wstrict-prototypes"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wstrict-prototypes"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wstrict-prototypes") && ((size_t)(const void *)(("-Wstrict-prototypes") + 1) - (size_t)(const void *)("-Wstrict-prototypes") == 1) && (__s2_len = strlen ("-Wstrict-prototypes"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wstrict-prototypes"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wstrict-prototypes"))[3]); } } __result; }))) : strcmp (p, "-Wstrict-prototypes")))); }))
    warn_strict_prototypes = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-strict-prototypes") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-strict-prototypes"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-strict-prototypes") + 1) - (size_t)(const void *)("-Wno-strict-prototypes") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-strict-prototypes"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-strict-prototypes") && ((size_t)(const void *)(("-Wno-strict-prototypes") + 1) - (size_t)(const void *)("-Wno-strict-prototypes") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-strict-prototypes"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-strict-prototypes") && ((size_t)(const void *)(("-Wno-strict-prototypes") + 1) - (size_t)(const void *)("-Wno-strict-prototypes") == 1) && (__s2_len = strlen ("-Wno-strict-prototypes"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-strict-prototypes"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-strict-prototypes"))[3]); } } __result; }))) : strcmp (p, "-Wno-strict-prototypes")))); }))
    warn_strict_prototypes = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wmissing-prototypes") && (__s1_len = strlen (p), __s2_len = strlen ("-Wmissing-prototypes"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wmissing-prototypes") + 1) - (size_t)(const void *)("-Wmissing-prototypes") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wmissing-prototypes"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wmissing-prototypes") && ((size_t)(const void *)(("-Wmissing-prototypes") + 1) - (size_t)(const void *)("-Wmissing-prototypes") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-prototypes"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wmissing-prototypes"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wmissing-prototypes") && ((size_t)(const void *)(("-Wmissing-prototypes") + 1) - (size_t)(const void *)("-Wmissing-prototypes") == 1) && (__s2_len = strlen ("-Wmissing-prototypes"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-prototypes"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-prototypes"))[3]); } } __result; }))) : strcmp (p, "-Wmissing-prototypes")))); }))
    warn_missing_prototypes = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-missing-prototypes") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-missing-prototypes"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-missing-prototypes") + 1) - (size_t)(const void *)("-Wno-missing-prototypes") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-missing-prototypes"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-missing-prototypes") && ((size_t)(const void *)(("-Wno-missing-prototypes") + 1) - (size_t)(const void *)("-Wno-missing-prototypes") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-prototypes"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-missing-prototypes") && ((size_t)(const void *)(("-Wno-missing-prototypes") + 1) - (size_t)(const void *)("-Wno-missing-prototypes") == 1) && (__s2_len = strlen ("-Wno-missing-prototypes"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-prototypes"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-prototypes"))[3]); } } __result; }))) : strcmp (p, "-Wno-missing-prototypes")))); }))
    warn_missing_prototypes = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wmissing-declarations") && (__s1_len = strlen (p), __s2_len = strlen ("-Wmissing-declarations"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wmissing-declarations") + 1) - (size_t)(const void *)("-Wmissing-declarations") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wmissing-declarations"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wmissing-declarations") && ((size_t)(const void *)(("-Wmissing-declarations") + 1) - (size_t)(const void *)("-Wmissing-declarations") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-declarations"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wmissing-declarations"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wmissing-declarations") && ((size_t)(const void *)(("-Wmissing-declarations") + 1) - (size_t)(const void *)("-Wmissing-declarations") == 1) && (__s2_len = strlen ("-Wmissing-declarations"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-declarations"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-declarations"))[3]); } } __result; }))) : strcmp (p, "-Wmissing-declarations")))); }))
    warn_missing_declarations = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-missing-declarations") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-missing-declarations"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-missing-declarations") + 1) - (size_t)(const void *)("-Wno-missing-declarations") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-missing-declarations"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-missing-declarations") && ((size_t)(const void *)(("-Wno-missing-declarations") + 1) - (size_t)(const void *)("-Wno-missing-declarations") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-declarations"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-missing-declarations"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-missing-declarations") && ((size_t)(const void *)(("-Wno-missing-declarations") + 1) - (size_t)(const void *)("-Wno-missing-declarations") == 1) && (__s2_len = strlen ("-Wno-missing-declarations"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-declarations"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-declarations"))[3]); } } __result; }))) : strcmp (p, "-Wno-missing-declarations")))); }))
    warn_missing_declarations = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wredundant-decls") && (__s1_len = strlen (p), __s2_len = strlen ("-Wredundant-decls"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wredundant-decls") + 1) - (size_t)(const void *)("-Wredundant-decls") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wredundant-decls"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wredundant-decls") && ((size_t)(const void *)(("-Wredundant-decls") + 1) - (size_t)(const void *)("-Wredundant-decls") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wredundant-decls"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wredundant-decls"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wredundant-decls") && ((size_t)(const void *)(("-Wredundant-decls") + 1) - (size_t)(const void *)("-Wredundant-decls") == 1) && (__s2_len = strlen ("-Wredundant-decls"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wredundant-decls"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wredundant-decls"))[3]); } } __result; }))) : strcmp (p, "-Wredundant-decls")))); }))
    warn_redundant_decls = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-redundant-decls") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-redundant-decls"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-redundant-decls") + 1) - (size_t)(const void *)("-Wno-redundant-decls") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-redundant-decls"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-redundant-decls") && ((size_t)(const void *)(("-Wno-redundant-decls") + 1) - (size_t)(const void *)("-Wno-redundant-decls") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-redundant-decls"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-redundant-decls"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-redundant-decls") && ((size_t)(const void *)(("-Wno-redundant-decls") + 1) - (size_t)(const void *)("-Wno-redundant-decls") == 1) && (__s2_len = strlen ("-Wno-redundant-decls"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-redundant-decls"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-redundant-decls"))[3]); } } __result; }))) : strcmp (p, "-Wno-redundant-decls")))); }))
    warn_redundant_decls = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wnested-externs") && (__s1_len = strlen (p), __s2_len = strlen ("-Wnested-externs"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wnested-externs") + 1) - (size_t)(const void *)("-Wnested-externs") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wnested-externs"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wnested-externs") && ((size_t)(const void *)(("-Wnested-externs") + 1) - (size_t)(const void *)("-Wnested-externs") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wnested-externs"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wnested-externs"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wnested-externs") && ((size_t)(const void *)(("-Wnested-externs") + 1) - (size_t)(const void *)("-Wnested-externs") == 1) && (__s2_len = strlen ("-Wnested-externs"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wnested-externs"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wnested-externs"))[3]); } } __result; }))) : strcmp (p, "-Wnested-externs")))); }))
    warn_nested_externs = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-nested-externs") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-nested-externs"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-nested-externs") + 1) - (size_t)(const void *)("-Wno-nested-externs") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-nested-externs"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-nested-externs") && ((size_t)(const void *)(("-Wno-nested-externs") + 1) - (size_t)(const void *)("-Wno-nested-externs") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-nested-externs"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-nested-externs"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-nested-externs") && ((size_t)(const void *)(("-Wno-nested-externs") + 1) - (size_t)(const void *)("-Wno-nested-externs") == 1) && (__s2_len = strlen ("-Wno-nested-externs"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-nested-externs"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-nested-externs"))[3]); } } __result; }))) : strcmp (p, "-Wno-nested-externs")))); }))
    warn_nested_externs = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wtraditional") && (__s1_len = strlen (p), __s2_len = strlen ("-Wtraditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wtraditional") + 1) - (size_t)(const void *)("-Wtraditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wtraditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wtraditional") && ((size_t)(const void *)(("-Wtraditional") + 1) - (size_t)(const void *)("-Wtraditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wtraditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wtraditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wtraditional") && ((size_t)(const void *)(("-Wtraditional") + 1) - (size_t)(const void *)("-Wtraditional") == 1) && (__s2_len = strlen ("-Wtraditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wtraditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wtraditional"))[3]); } } __result; }))) : strcmp (p, "-Wtraditional")))); }))
    warn_traditional = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-traditional") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-traditional"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-traditional") + 1) - (size_t)(const void *)("-Wno-traditional") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-traditional"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-traditional") && ((size_t)(const void *)(("-Wno-traditional") + 1) - (size_t)(const void *)("-Wno-traditional") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-traditional"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-traditional"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-traditional") && ((size_t)(const void *)(("-Wno-traditional") + 1) - (size_t)(const void *)("-Wno-traditional") == 1) && (__s2_len = strlen ("-Wno-traditional"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-traditional"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-traditional"))[3]); } } __result; }))) : strcmp (p, "-Wno-traditional")))); }))
    warn_traditional = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wformat") && (__s1_len = strlen (p), __s2_len = strlen ("-Wformat"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wformat") + 1) - (size_t)(const void *)("-Wformat") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wformat"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wformat") && ((size_t)(const void *)(("-Wformat") + 1) - (size_t)(const void *)("-Wformat") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wformat"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wformat"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wformat"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wformat"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wformat"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wformat") && ((size_t)(const void *)(("-Wformat") + 1) - (size_t)(const void *)("-Wformat") == 1) && (__s2_len = strlen ("-Wformat"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wformat"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wformat"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wformat"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wformat"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wformat"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wformat"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wformat"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wformat"))[3]); } } __result; }))) : strcmp (p, "-Wformat")))); }))
    warn_format = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-format") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-format"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-format") + 1) - (size_t)(const void *)("-Wno-format") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-format"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-format") && ((size_t)(const void *)(("-Wno-format") + 1) - (size_t)(const void *)("-Wno-format") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-format"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-format"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-format") && ((size_t)(const void *)(("-Wno-format") + 1) - (size_t)(const void *)("-Wno-format") == 1) && (__s2_len = strlen ("-Wno-format"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-format"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-format"))[3]); } } __result; }))) : strcmp (p, "-Wno-format")))); }))
    warn_format = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wchar-subscripts") && (__s1_len = strlen (p), __s2_len = strlen ("-Wchar-subscripts"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wchar-subscripts") + 1) - (size_t)(const void *)("-Wchar-subscripts") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wchar-subscripts"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wchar-subscripts") && ((size_t)(const void *)(("-Wchar-subscripts") + 1) - (size_t)(const void *)("-Wchar-subscripts") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wchar-subscripts"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wchar-subscripts"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wchar-subscripts") && ((size_t)(const void *)(("-Wchar-subscripts") + 1) - (size_t)(const void *)("-Wchar-subscripts") == 1) && (__s2_len = strlen ("-Wchar-subscripts"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wchar-subscripts"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wchar-subscripts"))[3]); } } __result; }))) : strcmp (p, "-Wchar-subscripts")))); }))
    warn_char_subscripts = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-char-subscripts") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-char-subscripts"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-char-subscripts") + 1) - (size_t)(const void *)("-Wno-char-subscripts") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-char-subscripts"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-char-subscripts") && ((size_t)(const void *)(("-Wno-char-subscripts") + 1) - (size_t)(const void *)("-Wno-char-subscripts") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-char-subscripts"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-char-subscripts"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-char-subscripts") && ((size_t)(const void *)(("-Wno-char-subscripts") + 1) - (size_t)(const void *)("-Wno-char-subscripts") == 1) && (__s2_len = strlen ("-Wno-char-subscripts"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-char-subscripts"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-char-subscripts"))[3]); } } __result; }))) : strcmp (p, "-Wno-char-subscripts")))); }))
    warn_char_subscripts = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wconversion") && (__s1_len = strlen (p), __s2_len = strlen ("-Wconversion"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wconversion") + 1) - (size_t)(const void *)("-Wconversion") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wconversion"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wconversion") && ((size_t)(const void *)(("-Wconversion") + 1) - (size_t)(const void *)("-Wconversion") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wconversion"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wconversion"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wconversion") && ((size_t)(const void *)(("-Wconversion") + 1) - (size_t)(const void *)("-Wconversion") == 1) && (__s2_len = strlen ("-Wconversion"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wconversion"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wconversion"))[3]); } } __result; }))) : strcmp (p, "-Wconversion")))); }))
    warn_conversion = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-conversion") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-conversion"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-conversion") + 1) - (size_t)(const void *)("-Wno-conversion") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-conversion"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-conversion") && ((size_t)(const void *)(("-Wno-conversion") + 1) - (size_t)(const void *)("-Wno-conversion") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-conversion"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-conversion"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-conversion") && ((size_t)(const void *)(("-Wno-conversion") + 1) - (size_t)(const void *)("-Wno-conversion") == 1) && (__s2_len = strlen ("-Wno-conversion"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-conversion"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-conversion"))[3]); } } __result; }))) : strcmp (p, "-Wno-conversion")))); }))
    warn_conversion = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wparentheses") && (__s1_len = strlen (p), __s2_len = strlen ("-Wparentheses"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wparentheses") + 1) - (size_t)(const void *)("-Wparentheses") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wparentheses"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wparentheses") && ((size_t)(const void *)(("-Wparentheses") + 1) - (size_t)(const void *)("-Wparentheses") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wparentheses"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wparentheses"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wparentheses") && ((size_t)(const void *)(("-Wparentheses") + 1) - (size_t)(const void *)("-Wparentheses") == 1) && (__s2_len = strlen ("-Wparentheses"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wparentheses"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wparentheses"))[3]); } } __result; }))) : strcmp (p, "-Wparentheses")))); }))
    warn_parentheses = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-parentheses") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-parentheses"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-parentheses") + 1) - (size_t)(const void *)("-Wno-parentheses") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-parentheses"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-parentheses") && ((size_t)(const void *)(("-Wno-parentheses") + 1) - (size_t)(const void *)("-Wno-parentheses") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-parentheses"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-parentheses"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-parentheses") && ((size_t)(const void *)(("-Wno-parentheses") + 1) - (size_t)(const void *)("-Wno-parentheses") == 1) && (__s2_len = strlen ("-Wno-parentheses"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-parentheses"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-parentheses"))[3]); } } __result; }))) : strcmp (p, "-Wno-parentheses")))); }))
    warn_parentheses = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wreturn-type") && (__s1_len = strlen (p), __s2_len = strlen ("-Wreturn-type"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wreturn-type") + 1) - (size_t)(const void *)("-Wreturn-type") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wreturn-type"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wreturn-type") && ((size_t)(const void *)(("-Wreturn-type") + 1) - (size_t)(const void *)("-Wreturn-type") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wreturn-type"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wreturn-type"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wreturn-type") && ((size_t)(const void *)(("-Wreturn-type") + 1) - (size_t)(const void *)("-Wreturn-type") == 1) && (__s2_len = strlen ("-Wreturn-type"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wreturn-type"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wreturn-type"))[3]); } } __result; }))) : strcmp (p, "-Wreturn-type")))); }))
    warn_return_type = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-return-type") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-return-type"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-return-type") + 1) - (size_t)(const void *)("-Wno-return-type") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-return-type"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-return-type") && ((size_t)(const void *)(("-Wno-return-type") + 1) - (size_t)(const void *)("-Wno-return-type") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-return-type"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-return-type"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-return-type") && ((size_t)(const void *)(("-Wno-return-type") + 1) - (size_t)(const void *)("-Wno-return-type") == 1) && (__s2_len = strlen ("-Wno-return-type"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-return-type"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-return-type"))[3]); } } __result; }))) : strcmp (p, "-Wno-return-type")))); }))
    warn_return_type = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wmissing-braces") && (__s1_len = strlen (p), __s2_len = strlen ("-Wmissing-braces"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wmissing-braces") + 1) - (size_t)(const void *)("-Wmissing-braces") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wmissing-braces"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wmissing-braces") && ((size_t)(const void *)(("-Wmissing-braces") + 1) - (size_t)(const void *)("-Wmissing-braces") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-braces"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wmissing-braces"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wmissing-braces") && ((size_t)(const void *)(("-Wmissing-braces") + 1) - (size_t)(const void *)("-Wmissing-braces") == 1) && (__s2_len = strlen ("-Wmissing-braces"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmissing-braces"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wmissing-braces"))[3]); } } __result; }))) : strcmp (p, "-Wmissing-braces")))); }))
    warn_missing_braces = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-missing-braces") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-missing-braces"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-missing-braces") + 1) - (size_t)(const void *)("-Wno-missing-braces") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-missing-braces"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-missing-braces") && ((size_t)(const void *)(("-Wno-missing-braces") + 1) - (size_t)(const void *)("-Wno-missing-braces") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-braces"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-missing-braces"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-missing-braces") && ((size_t)(const void *)(("-Wno-missing-braces") + 1) - (size_t)(const void *)("-Wno-missing-braces") == 1) && (__s2_len = strlen ("-Wno-missing-braces"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-missing-braces"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-missing-braces"))[3]); } } __result; }))) : strcmp (p, "-Wno-missing-braces")))); }))
    warn_missing_braces = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wmain") && (__s1_len = strlen (p), __s2_len = strlen ("-Wmain"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wmain") + 1) - (size_t)(const void *)("-Wmain") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wmain"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wmain") && ((size_t)(const void *)(("-Wmain") + 1) - (size_t)(const void *)("-Wmain") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmain"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmain"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmain"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmain"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wmain"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wmain") && ((size_t)(const void *)(("-Wmain") + 1) - (size_t)(const void *)("-Wmain") == 1) && (__s2_len = strlen ("-Wmain"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmain"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmain"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmain"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmain"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wmain"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wmain"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wmain"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wmain"))[3]); } } __result; }))) : strcmp (p, "-Wmain")))); }))
    warn_main = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-main") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-main"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-main") + 1) - (size_t)(const void *)("-Wno-main") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-main"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-main") && ((size_t)(const void *)(("-Wno-main") + 1) - (size_t)(const void *)("-Wno-main") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-main"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-main"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-main") && ((size_t)(const void *)(("-Wno-main") + 1) - (size_t)(const void *)("-Wno-main") == 1) && (__s2_len = strlen ("-Wno-main"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-main"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-main"))[3]); } } __result; }))) : strcmp (p, "-Wno-main")))); }))
    warn_main = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wsign-compare") && (__s1_len = strlen (p), __s2_len = strlen ("-Wsign-compare"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wsign-compare") + 1) - (size_t)(const void *)("-Wsign-compare") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wsign-compare"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wsign-compare") && ((size_t)(const void *)(("-Wsign-compare") + 1) - (size_t)(const void *)("-Wsign-compare") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wsign-compare"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wsign-compare"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wsign-compare") && ((size_t)(const void *)(("-Wsign-compare") + 1) - (size_t)(const void *)("-Wsign-compare") == 1) && (__s2_len = strlen ("-Wsign-compare"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wsign-compare"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wsign-compare"))[3]); } } __result; }))) : strcmp (p, "-Wsign-compare")))); }))
    warn_sign_compare = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-sign-compare") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-sign-compare"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-sign-compare") + 1) - (size_t)(const void *)("-Wno-sign-compare") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-sign-compare"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-sign-compare") && ((size_t)(const void *)(("-Wno-sign-compare") + 1) - (size_t)(const void *)("-Wno-sign-compare") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-sign-compare"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-sign-compare"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-sign-compare") && ((size_t)(const void *)(("-Wno-sign-compare") + 1) - (size_t)(const void *)("-Wno-sign-compare") == 1) && (__s2_len = strlen ("-Wno-sign-compare"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-sign-compare"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-sign-compare"))[3]); } } __result; }))) : strcmp (p, "-Wno-sign-compare")))); }))
    warn_sign_compare = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wmultichar") && (__s1_len = strlen (p), __s2_len = strlen ("-Wmultichar"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wmultichar") + 1) - (size_t)(const void *)("-Wmultichar") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wmultichar"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wmultichar") && ((size_t)(const void *)(("-Wmultichar") + 1) - (size_t)(const void *)("-Wmultichar") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmultichar"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wmultichar"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wmultichar") && ((size_t)(const void *)(("-Wmultichar") + 1) - (size_t)(const void *)("-Wmultichar") == 1) && (__s2_len = strlen ("-Wmultichar"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wmultichar"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wmultichar"))[3]); } } __result; }))) : strcmp (p, "-Wmultichar")))); }))
    warn_multichar = 1;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wno-multichar") && (__s1_len = strlen (p), __s2_len = strlen ("-Wno-multichar"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wno-multichar") + 1) - (size_t)(const void *)("-Wno-multichar") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wno-multichar"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wno-multichar") && ((size_t)(const void *)(("-Wno-multichar") + 1) - (size_t)(const void *)("-Wno-multichar") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-multichar"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wno-multichar"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wno-multichar") && ((size_t)(const void *)(("-Wno-multichar") + 1) - (size_t)(const void *)("-Wno-multichar") == 1) && (__s2_len = strlen ("-Wno-multichar"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wno-multichar"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wno-multichar"))[3]); } } __result; }))) : strcmp (p, "-Wno-multichar")))); }))
    warn_multichar = 0;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("-Wall") && (__s1_len = strlen (p), __s2_len = strlen ("-Wall"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("-Wall") + 1) - (size_t)(const void *)("-Wall") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("-Wall"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("-Wall") && ((size_t)(const void *)(("-Wall") + 1) - (size_t)(const void *)("-Wall") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wall"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wall"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wall"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wall"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("-Wall"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("-Wall") && ((size_t)(const void *)(("-Wall") + 1) - (size_t)(const void *)("-Wall") == 1) && (__s2_len = strlen ("-Wall"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("-Wall"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("-Wall"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("-Wall"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("-Wall"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("-Wall"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("-Wall"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("-Wall"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("-Wall"))[3]); } } __result; }))) : strcmp (p, "-Wall")))); }))
    {
      if (warn_uninitialized != 1)
        warn_uninitialized = 2;
      warn_implicit_int = 1;
      mesg_implicit_function_declaration = 1;
      warn_return_type = 1;
      warn_unused = 1;
      warn_switch = 1;
      warn_format = 1;
      warn_char_subscripts = 1;
      warn_parentheses = 1;
      warn_missing_braces = 1;
      warn_main = 2;
    }
}
int main(int argc, char **argv)
{
  register int i;
  dd_list files, preludes;
  dd_list_pos cur;
  int version_flag = 0;
  char *p;
  char *config_file = ((void *)0);
  start_timer(&total_time);
  region_init();
  parse_region = newregion();
  in_prelude = 0;
  num_hotspots = 0;
  parsed_files = dd_new_list(parse_region);
  copy_argc = 0;
  copy_argv = xmalloc((argc + 1) * sizeof(*copy_argv));
  files = dd_new_list(parse_region);
  preludes = dd_new_list(parse_region);
  p = argv[0] + strlen (argv[0]);
  while (p != argv[0] && p[-1] != '/'
         )
    --p;
  progname = p;
  signal (13, pipe_closed);
  copy_argv[0] = argv[0];
  copy_argc = 1;
  for (i = 1; i < argc; i++)
    {
      int j;
      bool copy_arg = 1;
      for (j = 0; lang_options[j] != 0; j++)
        if (!(__extension__ (__builtin_constant_p (strlen (lang_options[j])) && ((__builtin_constant_p (argv[i]) && strlen (argv[i]) < ((size_t) (strlen (lang_options[j])))) || (__builtin_constant_p (lang_options[j]) && strlen (lang_options[j]) < ((size_t) (strlen (lang_options[j]))))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (argv[i]) && __builtin_constant_p (lang_options[j]) && (__s1_len = strlen (argv[i]), __s2_len = strlen (lang_options[j]), (!((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((lang_options[j]) + 1) - (size_t)(const void *)(lang_options[j]) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (argv[i]), (__const char *) (lang_options[j]), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) && (__s1_len = strlen (argv[i]), __s1_len < 4) ? (__builtin_constant_p (lang_options[j]) && ((size_t)(const void *)((lang_options[j]) + 1) - (size_t)(const void *)(lang_options[j]) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)(lang_options[j]))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) (lang_options[j]))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) (lang_options[j]))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) (lang_options[j]))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (lang_options[j]); register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (lang_options[j]) && ((size_t)(const void *)((lang_options[j]) + 1) - (size_t)(const void *)(lang_options[j]) == 1) && (__s2_len = strlen (lang_options[j]), __s2_len < 4) ? (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (argv[i]))[0] - ((__const unsigned char *) (__const char *)(lang_options[j]))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[1] - ((__const unsigned char *) (__const char *) (lang_options[j]))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (argv[i]))[2] - ((__const unsigned char *) (__const char *) (lang_options[j]))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (argv[i]))[3] - ((__const unsigned char *) (__const char *) (lang_options[j]))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (argv[i]); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (lang_options[j]))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (lang_options[j]))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (lang_options[j]))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (lang_options[j]))[3]); } } __result; }))) : strcmp (argv[i], lang_options[j])))); }) : strncmp (argv[i], lang_options[j], strlen (lang_options[j])))))
          break;
      if (lang_options[j] != 0)
        c_decode_option(argv[i]);
      else if (argv[i][0] == '-' && argv[i][1] != 0)
        {
          register char *str = argv[i] + 1;
          if (str[0] == 'Y')
            str++;
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("dumpbase") && (__s1_len = strlen (str), __s2_len = strlen ("dumpbase"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("dumpbase") + 1) - (size_t)(const void *)("dumpbase") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("dumpbase"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("dumpbase") && ((size_t)(const void *)(("dumpbase") + 1) - (size_t)(const void *)("dumpbase") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("dumpbase"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("dumpbase"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("dumpbase"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("dumpbase"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("dumpbase"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("dumpbase") && ((size_t)(const void *)(("dumpbase") + 1) - (size_t)(const void *)("dumpbase") == 1) && (__s2_len = strlen ("dumpbase"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("dumpbase"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("dumpbase"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("dumpbase"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("dumpbase"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("dumpbase"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("dumpbase"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("dumpbase"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("dumpbase"))[3]); } } __result; }))) : strcmp (str, "dumpbase")))); }))
            copy_argv[copy_argc++] = argv[i++];
          else if (str[0] == 'f')
            {
              register char *p = &str[1];
              int found = 0;
              for (j = 0;
                   !found && j < sizeof (f_options) / sizeof (f_options[0]);
                   j++)
                {
                  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p (f_options[j].string) && (__s1_len = strlen (p), __s2_len = strlen (f_options[j].string), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) (f_options[j].string), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p (f_options[j].string) && ((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)(f_options[j].string))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (f_options[j].string); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (f_options[j].string) && ((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) && (__s2_len = strlen (f_options[j].string), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)(f_options[j].string))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (f_options[j].string))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; }))) : strcmp (p, f_options[j].string)))); }))
                    {
                      *f_options[j].variable = f_options[j].on_value;
                      found = 1;
                    }
                  if (p[0] == 'n' && p[1] == 'o' && p[2] == '-'
                      && ! __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p+3) && __builtin_constant_p (f_options[j].string) && (__s1_len = strlen (p+3), __s2_len = strlen (f_options[j].string), (!((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p+3), (__const char *) (f_options[j].string), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p+3) && ((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) && (__s1_len = strlen (p+3), __s1_len < 4) ? (__builtin_constant_p (f_options[j].string) && ((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - ((__const unsigned char *) (__const char *)(f_options[j].string))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (f_options[j].string); register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (f_options[j].string) && ((size_t)(const void *)((f_options[j].string) + 1) - (size_t)(const void *)(f_options[j].string) == 1) && (__s2_len = strlen (f_options[j].string), __s2_len < 4) ? (__builtin_constant_p (p+3) && ((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - ((__const unsigned char *) (__const char *)(f_options[j].string))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p+3); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (f_options[j].string))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (f_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (f_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (f_options[j].string))[3]); } } __result; }))) : strcmp (p+3, f_options[j].string)))); }))
                    {
                      *f_options[j].variable = ! f_options[j].on_value;
                      found = 1;
                    }
                }
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("pedantic") && (__s1_len = strlen (str), __s2_len = strlen ("pedantic"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("pedantic") + 1) - (size_t)(const void *)("pedantic") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("pedantic"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("pedantic") && ((size_t)(const void *)(("pedantic") + 1) - (size_t)(const void *)("pedantic") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("pedantic"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("pedantic"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("pedantic"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("pedantic"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("pedantic"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("pedantic") && ((size_t)(const void *)(("pedantic") + 1) - (size_t)(const void *)("pedantic") == 1) && (__s2_len = strlen ("pedantic"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("pedantic"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("pedantic"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("pedantic"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("pedantic"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("pedantic"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("pedantic"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("pedantic"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("pedantic"))[3]); } } __result; }))) : strcmp (str, "pedantic")))); }))
            pedantic = 1;
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("pedantic-errors") && (__s1_len = strlen (str), __s2_len = strlen ("pedantic-errors"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("pedantic-errors") + 1) - (size_t)(const void *)("pedantic-errors") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("pedantic-errors"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("pedantic-errors") && ((size_t)(const void *)(("pedantic-errors") + 1) - (size_t)(const void *)("pedantic-errors") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("pedantic-errors"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("pedantic-errors"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("pedantic-errors") && ((size_t)(const void *)(("pedantic-errors") + 1) - (size_t)(const void *)("pedantic-errors") == 1) && (__s2_len = strlen ("pedantic-errors"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("pedantic-errors"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("pedantic-errors"))[3]); } } __result; }))) : strcmp (str, "pedantic-errors")))); }))
            flag_pedantic_errors = pedantic = 1;
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("quiet") && (__s1_len = strlen (str), __s2_len = strlen ("quiet"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("quiet") + 1) - (size_t)(const void *)("quiet") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("quiet"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("quiet") && ((size_t)(const void *)(("quiet") + 1) - (size_t)(const void *)("quiet") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("quiet"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("quiet"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("quiet"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("quiet"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("quiet"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("quiet") && ((size_t)(const void *)(("quiet") + 1) - (size_t)(const void *)("quiet") == 1) && (__s2_len = strlen ("quiet"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("quiet"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("quiet"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("quiet"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("quiet"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("quiet"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("quiet"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("quiet"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("quiet"))[3]); } } __result; }))) : strcmp (str, "quiet")))); }))
            quiet_flag = 1;
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("version") && (__s1_len = strlen (str), __s2_len = strlen ("version"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("version") + 1) - (size_t)(const void *)("version") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("version"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("version") && ((size_t)(const void *)(("version") + 1) - (size_t)(const void *)("version") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("version"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("version"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("version"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("version"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("version"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("version") && ((size_t)(const void *)(("version") + 1) - (size_t)(const void *)("version") == 1) && (__s2_len = strlen ("version"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("version"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("version"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("version"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("version"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("version"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("version"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("version"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("version"))[3]); } } __result; }))) : strcmp (str, "version")))); }))
            version_flag = 1;
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("w") && (__s1_len = strlen (str), __s2_len = strlen ("w"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("w"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("w") && ((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("w"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("w"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("w") && ((size_t)(const void *)(("w") + 1) - (size_t)(const void *)("w") == 1) && (__s2_len = strlen ("w"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("w"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("w"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("w"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("w"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("w"))[3]); } } __result; }))) : strcmp (str, "w")))); }))
            inhibit_warnings = 1;
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("W") && (__s1_len = strlen (str), __s2_len = strlen ("W"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("W") + 1) - (size_t)(const void *)("W") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("W"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("W") && ((size_t)(const void *)(("W") + 1) - (size_t)(const void *)("W") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("W"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("W"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("W"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("W"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("W"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("W") && ((size_t)(const void *)(("W") + 1) - (size_t)(const void *)("W") == 1) && (__s2_len = strlen ("W"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("W"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("W"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("W"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("W"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("W"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("W"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("W"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("W"))[3]); } } __result; }))) : strcmp (str, "W")))); }))
            {
              extra_warnings = 1;
              if (warn_uninitialized != 1)
                warn_uninitialized = 2;
            }
          else if (str[0] == 'W')
            {
              register char *p = &str[1];
              int found = 0;
              for (j = 0;
                   !found && j < sizeof (W_options) / sizeof (W_options[0]);
                   j++)
                {
                  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p (W_options[j].string) && (__s1_len = strlen (p), __s2_len = strlen (W_options[j].string), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) (W_options[j].string), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p (W_options[j].string) && ((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)(W_options[j].string))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (W_options[j].string); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (W_options[j].string) && ((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) && (__s2_len = strlen (W_options[j].string), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)(W_options[j].string))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (W_options[j].string))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; }))) : strcmp (p, W_options[j].string)))); }))
                    {
                      *W_options[j].variable = W_options[j].on_value;
                      found = 1;
                    }
                  if (p[0] == 'n' && p[1] == 'o' && p[2] == '-'
                      && ! __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p+3) && __builtin_constant_p (W_options[j].string) && (__s1_len = strlen (p+3), __s2_len = strlen (W_options[j].string), (!((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p+3), (__const char *) (W_options[j].string), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p+3) && ((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) && (__s1_len = strlen (p+3), __s1_len < 4) ? (__builtin_constant_p (W_options[j].string) && ((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - ((__const unsigned char *) (__const char *)(W_options[j].string))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (W_options[j].string); register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (W_options[j].string) && ((size_t)(const void *)((W_options[j].string) + 1) - (size_t)(const void *)(W_options[j].string) == 1) && (__s2_len = strlen (W_options[j].string), __s2_len < 4) ? (__builtin_constant_p (p+3) && ((size_t)(const void *)((p+3) + 1) - (size_t)(const void *)(p+3) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p+3))[0] - ((__const unsigned char *) (__const char *)(W_options[j].string))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p+3))[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p+3))[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p+3); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (W_options[j].string))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (W_options[j].string))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (W_options[j].string))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (W_options[j].string))[3]); } } __result; }))) : strcmp (p+3, W_options[j].string)))); }))
                    {
                      *W_options[j].variable = ! W_options[j].on_value;
                      found = 1;
                    }
                }
              if (found)
                ;
              else if (!(__extension__ (__builtin_constant_p (9) && ((__builtin_constant_p (p) && strlen (p) < ((size_t) (9))) || (__builtin_constant_p ("id-clash-") && strlen ("id-clash-") < ((size_t) (9)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("id-clash-") && (__s1_len = strlen (p), __s2_len = strlen ("id-clash-"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("id-clash-") + 1) - (size_t)(const void *)("id-clash-") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("id-clash-"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("id-clash-") && ((size_t)(const void *)(("id-clash-") + 1) - (size_t)(const void *)("id-clash-") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("id-clash-"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("id-clash-"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("id-clash-"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("id-clash-"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("id-clash-"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("id-clash-") && ((size_t)(const void *)(("id-clash-") + 1) - (size_t)(const void *)("id-clash-") == 1) && (__s2_len = strlen ("id-clash-"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("id-clash-"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("id-clash-"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("id-clash-"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("id-clash-"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("id-clash-"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("id-clash-"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("id-clash-"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("id-clash-"))[3]); } } __result; }))) : strcmp (p, "id-clash-")))); }) : strncmp (p, "id-clash-", 9))))
                {
                  char *endp = p + 9;
                  while (*endp)
                    {
                      if (*endp >= '0' && *endp <= '9')
                        endp++;
                      else
                        {
                          error ("Invalid option `%s'", argv[i]);
                          goto id_clash_lose;
                        }
                    }
                  warn_id_clash = 1;
                  id_clash_len = atoi (str + 10);
                id_clash_lose: ;
                }
              else if (!(__extension__ (__builtin_constant_p (12) && ((__builtin_constant_p (p) && strlen (p) < ((size_t) (12))) || (__builtin_constant_p ("larger-than-") && strlen ("larger-than-") < ((size_t) (12)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (p) && __builtin_constant_p ("larger-than-") && (__s1_len = strlen (p), __s2_len = strlen ("larger-than-"), (!((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("larger-than-") + 1) - (size_t)(const void *)("larger-than-") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (p), (__const char *) ("larger-than-"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) && (__s1_len = strlen (p), __s1_len < 4) ? (__builtin_constant_p ("larger-than-") && ((size_t)(const void *)(("larger-than-") + 1) - (size_t)(const void *)("larger-than-") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("larger-than-"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("larger-than-"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("larger-than-"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("larger-than-"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("larger-than-"); register int __result = (((__const unsigned char *) (__const char *) (p))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("larger-than-") && ((size_t)(const void *)(("larger-than-") + 1) - (size_t)(const void *)("larger-than-") == 1) && (__s2_len = strlen ("larger-than-"), __s2_len < 4) ? (__builtin_constant_p (p) && ((size_t)(const void *)((p) + 1) - (size_t)(const void *)(p) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (p))[0] - ((__const unsigned char *) (__const char *)("larger-than-"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[1] - ((__const unsigned char *) (__const char *) ("larger-than-"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (p))[2] - ((__const unsigned char *) (__const char *) ("larger-than-"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (p))[3] - ((__const unsigned char *) (__const char *) ("larger-than-"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (p); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("larger-than-"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("larger-than-"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("larger-than-"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("larger-than-"))[3]); } } __result; }))) : strcmp (p, "larger-than-")))); }) : strncmp (p, "larger-than-", 12))))
                {
                  char *endp = p + 12;
                  while (*endp)
                    {
                      if (*endp >= '0' && *endp <= '9')
                        endp++;
                      else
                        {
                          error ("Invalid option `%s'", argv[i]);
                          goto larger_than_lose;
                        }
                    }
                  warn_larger_than = 1;
                  larger_than_size = atoi (str + 13);
                larger_than_lose: ;
                }
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("o") && (__s1_len = strlen (str), __s2_len = strlen ("o"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("o") + 1) - (size_t)(const void *)("o") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("o"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("o") && ((size_t)(const void *)(("o") + 1) - (size_t)(const void *)("o") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("o"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("o"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("o"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("o"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("o"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("o") && ((size_t)(const void *)(("o") + 1) - (size_t)(const void *)("o") == 1) && (__s2_len = strlen ("o"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("o"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("o"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("o"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("o"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("o"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("o"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("o"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("o"))[3]); } } __result; }))) : strcmp (str, "o")))); }))
            copy_argv[copy_argc++] = argv[i++];
          else if (str[0] == 'G')
            {
              if (str[1] == '\0')
                copy_argv[copy_argc++] = argv[i++];
            }
          else if (!(__extension__ (__builtin_constant_p (8) && ((__builtin_constant_p (str) && strlen (str) < ((size_t) (8))) || (__builtin_constant_p ("aux-info") && strlen ("aux-info") < ((size_t) (8)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("aux-info") && (__s1_len = strlen (str), __s2_len = strlen ("aux-info"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("aux-info") + 1) - (size_t)(const void *)("aux-info") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("aux-info"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("aux-info") && ((size_t)(const void *)(("aux-info") + 1) - (size_t)(const void *)("aux-info") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("aux-info"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("aux-info"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("aux-info"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("aux-info"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("aux-info"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("aux-info") && ((size_t)(const void *)(("aux-info") + 1) - (size_t)(const void *)("aux-info") == 1) && (__s2_len = strlen ("aux-info"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("aux-info"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("aux-info"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("aux-info"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("aux-info"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("aux-info"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("aux-info"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("aux-info"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("aux-info"))[3]); } } __result; }))) : strcmp (str, "aux-info")))); }) : strncmp (str, "aux-info", 8))))
            {
              if (str[8] == '\0')
                copy_argv[copy_argc++] = argv[i++];
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("config") && (__s1_len = strlen (str), __s2_len = strlen ("config"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("config") + 1) - (size_t)(const void *)("config") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("config"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("config") && ((size_t)(const void *)(("config") + 1) - (size_t)(const void *)("config") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("config"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("config"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("config"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("config"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("config"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("config") && ((size_t)(const void *)(("config") + 1) - (size_t)(const void *)("config") == 1) && (__s2_len = strlen ("config"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("config"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("config"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("config"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("config"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("config"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("config"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("config"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("config"))[3]); } } __result; }))) : strcmp (str, "config")))); }))
            {
              if (i < argc - 1)
                {
                  i++;
                  config_file = (__extension__ (__builtin_constant_p (argv[i]) && ((size_t)(const void *)((argv[i]) + 1) - (size_t)(const void *)(argv[i]) == 1) ? (((__const char *) (argv[i]))[0] == '\0' ? (char *) calloc (1, 1) : ({ size_t __len = strlen (argv[i]) + 1; char *__retval = (char *) malloc (__len); if (__retval != ((void *)0)) __retval = (char *) memcpy (__retval, argv[i], __len); __retval; })) : __strdup (argv[i])));
                }
              else
                error ("Missing -config file");
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("prelude") && (__s1_len = strlen (str), __s2_len = strlen ("prelude"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("prelude") + 1) - (size_t)(const void *)("prelude") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("prelude"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("prelude") && ((size_t)(const void *)(("prelude") + 1) - (size_t)(const void *)("prelude") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("prelude"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("prelude"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("prelude"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("prelude"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("prelude"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("prelude") && ((size_t)(const void *)(("prelude") + 1) - (size_t)(const void *)("prelude") == 1) && (__s2_len = strlen ("prelude"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("prelude"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("prelude"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("prelude"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("prelude"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("prelude"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("prelude"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("prelude"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("prelude"))[3]); } } __result; }))) : strcmp (str, "prelude")))); }))
            {
              if (i < argc - 1)
                {
                  i++;
                  dd_add_last(parse_region, preludes,
                              (((void)0), __rc_rstrdup)(parse_region, argv[i]));
                }
              else
                error("Missing -prelude file");
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("hotspots") && (__s1_len = strlen (str), __s2_len = strlen ("hotspots"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("hotspots") + 1) - (size_t)(const void *)("hotspots") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("hotspots"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("hotspots") && ((size_t)(const void *)(("hotspots") + 1) - (size_t)(const void *)("hotspots") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("hotspots"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("hotspots"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("hotspots"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("hotspots"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("hotspots"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("hotspots") && ((size_t)(const void *)(("hotspots") + 1) - (size_t)(const void *)("hotspots") == 1) && (__s2_len = strlen ("hotspots"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("hotspots"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("hotspots"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("hotspots"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("hotspots"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("hotspots"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("hotspots"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("hotspots"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("hotspots"))[3]); } } __result; }))) : strcmp (str, "hotspots")))); }))
            {
              if (i < argc - 1)
                {
                  i++;
                  num_hotspots = atoi(argv[i]);
                  if (num_hotspots < 0)
                    error("Negative value for -hotspots count");
                }
              else
                error("Missing -hotspots count");
            }
          else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (str) && __builtin_constant_p ("program-files") && (__s1_len = strlen (str), __s2_len = strlen ("program-files"), (!((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("program-files") + 1) - (size_t)(const void *)("program-files") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (str), (__const char *) ("program-files"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) && (__s1_len = strlen (str), __s1_len < 4) ? (__builtin_constant_p ("program-files") && ((size_t)(const void *)(("program-files") + 1) - (size_t)(const void *)("program-files") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("program-files"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("program-files"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("program-files"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("program-files"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("program-files"); register int __result = (((__const unsigned char *) (__const char *) (str))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("program-files") && ((size_t)(const void *)(("program-files") + 1) - (size_t)(const void *)("program-files") == 1) && (__s2_len = strlen ("program-files"), __s2_len < 4) ? (__builtin_constant_p (str) && ((size_t)(const void *)((str) + 1) - (size_t)(const void *)(str) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (str))[0] - ((__const unsigned char *) (__const char *)("program-files"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[1] - ((__const unsigned char *) (__const char *) ("program-files"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (str))[2] - ((__const unsigned char *) (__const char *) ("program-files"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (str))[3] - ((__const unsigned char *) (__const char *) ("program-files"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (str); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("program-files"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("program-files"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("program-files"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("program-files"))[3]); } } __result; }))) : strcmp (str, "program-files")))); }))
            {
              if (i < argc - 1)
                {
                  i++;
                  add_program_files(argv[i], files);
                }
              else
                error("Missing -program-files file");
            }
        }
      else if (argv[i][0] == '+')
        ;
      else
        {
          glob_t globbuf;
          char **cur;
          if (glob(argv[i], 0, ((void *)0), &globbuf))
            {
              fprintf(stderr, "%s: file not found\n", argv[i]);
              exit(33);
            }
          else
            for (cur = globbuf.gl_pathv; *cur; cur++)
              {
                if ( strlen(*cur) >= 9 &&
                    !(__extension__ (__builtin_constant_p (9) && ((__builtin_constant_p ("prelude.i") && strlen ("prelude.i") < ((size_t) (9))) || (__builtin_constant_p (*cur + strlen(*cur) - 9) && strlen (*cur + strlen(*cur) - 9) < ((size_t) (9)))) ? __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p ("prelude.i") && __builtin_constant_p (*cur + strlen(*cur) - 9) && (__s1_len = strlen ("prelude.i"), __s2_len = strlen (*cur + strlen(*cur) - 9), (!((size_t)(const void *)(("prelude.i") + 1) - (size_t)(const void *)("prelude.i") == 1) || __s1_len >= 4) && (!((size_t)(const void *)((*cur + strlen(*cur) - 9) + 1) - (size_t)(const void *)(*cur + strlen(*cur) - 9) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) ("prelude.i"), (__const char *) (*cur + strlen(*cur) - 9), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p ("prelude.i") && ((size_t)(const void *)(("prelude.i") + 1) - (size_t)(const void *)("prelude.i") == 1) && (__s1_len = strlen ("prelude.i"), __s1_len < 4) ? (__builtin_constant_p (*cur + strlen(*cur) - 9) && ((size_t)(const void *)((*cur + strlen(*cur) - 9) + 1) - (size_t)(const void *)(*cur + strlen(*cur) - 9) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[0] - ((__const unsigned char *) (__const char *)(*cur + strlen(*cur) - 9))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[1] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[2] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[3] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9); register int __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (*cur + strlen(*cur) - 9) && ((size_t)(const void *)((*cur + strlen(*cur) - 9) + 1) - (size_t)(const void *)(*cur + strlen(*cur) - 9) == 1) && (__s2_len = strlen (*cur + strlen(*cur) - 9), __s2_len < 4) ? (__builtin_constant_p ("prelude.i") && ((size_t)(const void *)(("prelude.i") + 1) - (size_t)(const void *)("prelude.i") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[0] - ((__const unsigned char *) (__const char *)(*cur + strlen(*cur) - 9))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[1] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[2] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) ("prelude.i"))[3] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) ("prelude.i"); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (*cur + strlen(*cur) - 9))[3]); } } __result; }))) : strcmp ("prelude.i", *cur + strlen(*cur) - 9)))); }) : strncmp ("prelude.i", *cur + strlen(*cur) - 9, 9))))
                  dd_add_last(parse_region, preludes,
                              (((void)0), __rc_rstrdup)(parse_region, *cur));
                else
                  dd_add_last(parse_region, files,
                              (((void)0), __rc_rstrdup)(parse_region, *cur));
              }
          copy_arg = 0;
        }
      if (copy_arg)
        copy_argv[copy_argc++] = argv[i];
    }
  copy_argv[copy_argc] = ((void *)0);
  init_types();
  cval_init();
  init_effects();
  init_qtype();
  init_quals();
  init_qerror();
  init_store();
  init_aloctypes();
  if (config_file)
    load_config_file_quals(config_file);
  if (!const_qual)
    {
      begin_po_qual();
      const_qual = add_qual("const");
      add_level_qual(const_qual, level_ref);
      set_po_nonprop();
      end_po_qual();
    }
  if (!volatile_qual)
    {
      begin_po_qual();
      volatile_qual = add_qual("volatile");
      add_level_qual(volatile_qual, level_ref);
      add_sign_qual(volatile_qual, sign_eq);
      set_po_nonprop();
      end_po_qual();
    }
  if (!noreturn_qual)
    {
      begin_po_qual();
      noreturn_qual = add_qual("noreturn");
      add_level_qual(noreturn_qual, level_value);
      add_sign_qual(noreturn_qual, sign_eq);
      set_po_nonprop();
      end_po_qual();
    }
  end_define_pos();
  init_pam();
  rinf_init_analyze();
  in_prelude = 1;
  for (cur = dd_first((preludes)); !(!(cur)->next); cur = ((cur)->next))
    {
      char *file;
      file = ((char *)((cur)->data));
      fprintf(stderr, "Analyzing prelude %s\n", file);
      compile_file(file);
    }
  in_prelude = 0;
  if (((!(dd_first((files)))->next)))
    compile_file(0);
  else
    for (cur = dd_first((files)); !(!(cur)->next); cur = ((cur)->next))
    {
      char *file;
      file = ((char *)((cur)->data));
      fprintf(stderr, "Analyzing %s\n", file);
      compile_file(file);
    }
  if (errorcount)
    exit (33);
  rinf_finish_analyze();
  if (flag_flow_sensitive)
    {
      dd_list_pos cur;
      init_flow_sensitive();
      for (cur = dd_first((parsed_files)); !(!(cur)->next); cur = ((cur)->next))
        {
          parsed_file pf;
          pf = ((parsed_file)((cur)->data));
          fprintf(stderr, "Flow-sensitive analysis of %s\n", pf->name);
          analyze_flow_sensitive(pf->program);
        }
      finish_flow_sensitive();
    }
  finish_quals();
  end_timer(&total_time);
  if (flag_statistics)
    memusage = get_memusage();
  if (flag_print_results)
    print_results();
  if (flag_pam_mode)
    enter_pam_mode();
  if (flag_statistics)
    {
      fflush(((void *)0));
      fprintf(stdout, "VMSize: %ldK\n", memusage / 1024);
      fprintf(stdout, "Elapsed time: %ss\n", timer_to_ascii(&total_time));
      fprintf(stdout, "Analysis complete\n");
    }
  fflush(((void *)0));
  exit (0);
  return 0;
}
const char CANON_IDENT_9e6dd87f45ca61505c48aaa229494d37[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/rinf_main.c";
