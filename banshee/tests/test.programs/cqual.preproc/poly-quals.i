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
         const char *combine_colors_pam(const char *color1, const char *color2);
typedef unsigned char bool;
typedef struct region_ *region;
extern region permanent;
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
typedef __gnuc_va_list va_list;
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
extern void __assert_fail (__const char *__assertion, __const char *__file,
                           unsigned int __line, __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert_perror_fail (int __errnum, __const char *__file,
                                  unsigned int __line,
                                  __const char *__function)
             __attribute__ ((__noreturn__));
extern void __assert (const char *__assertion, const char *__file, int __line)
             __attribute__ ((__noreturn__));
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
typedef intptr_t stamp;
typedef void * term;
typedef struct term_hash *term_hash;
term_hash make_term_hash(region r);
term term_hash_find(term_hash h, stamp *st,int length);
bool term_hash_insert(term_hash h, term t, stamp *st, int length) ;
void term_hash_delete(term_hash h) ;
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
typedef struct timer
{
  struct timeval start;
  struct timeval elapsed;
} timer;
void start_timer(timer *t);
void end_timer(timer *t);
char *timer_to_ascii(timer *t);
int cmp_qual_byname (qual left, qual right);
unsigned long hash_qual(qual q);
static int cmp_qual_edge_semantic(qual_edge e1, qual_edge e2);
static unsigned long hash_qual_edge(qual_edge e);
typedef enum { d_bak, d_fwd } q_direction;
typedef enum { ek_open, ek_flow, ek_close } qedge_kind;
struct Qual_edge {
  qedge_kind kind:7;
  bool summary:1;
  qual q1;
  qual q2;
  int index;
  location loc;
  qual_gate qgate;
};
typedef struct { hash_table ht; } * qual_edge_hashset; typedef struct { hash_table_scanner hts; } qual_edge_hashset_scanner; static inline bool qual_edge_hashset_eqfn(qual_edge l, qual_edge r) { return cmp_qual_edge_semantic(l,r) == 0; } static inline qual_edge_hashset empty_qual_edge_hashset(region r) { return (qual_edge_hashset) make_hash_table(r, 16, (hash_fn) hash_qual_edge, (keyeq_fn) qual_edge_hashset_eqfn); } static inline qual_edge_hashset qual_edge_hashset_copy(region r, qual_edge_hashset s) { return (qual_edge_hashset) hash_table_copy(r, (hash_table) s); } static inline bool qual_edge_hashset_empty(qual_edge_hashset s) { return hash_table_size((hash_table) s) == 0; } static inline bool qual_edge_hashset_member(qual_edge_hashset s, qual_edge elt) { qual_edge dummy; return hash_table_lookup((hash_table) s, (hash_key) elt, (hash_data*) &dummy); } static inline bool qual_edge_hashset_hash_search(qual_edge_hashset s, bool (*eqfn)(qual_edge, qual_edge), qual_edge k, qual_edge *d) { return hash_table_hash_search((hash_table)s, (keyeq_fn)eqfn, (hash_key)k, (hash_data*)d); } static inline unsigned long qual_edge_hashset_size(qual_edge_hashset s) { return hash_table_size((hash_table) s); } static inline bool qual_edge_hashset_insert(region r, qual_edge_hashset *s, qual_edge elt) { return hash_table_insert((hash_table) *s, (hash_key) elt, (hash_data) elt); } static inline void qual_edge_hashset_remove(qual_edge_hashset *s, qual_edge elt) { hash_table_remove((hash_table) *s, (hash_key) elt); } static inline bool qual_edge_hashset_single(qual_edge_hashset s) { return qual_edge_hashset_size(s) == 1; } static inline void qual_edge_hashset_scan(qual_edge_hashset s, qual_edge_hashset_scanner *ss) { hash_table_scan((hash_table) s, &ss->hts); } static inline qual_edge qual_edge_hashset_next(qual_edge_hashset_scanner *ss) { qual_edge result; int dummy; if (hash_table_next(&ss->hts, (hash_key *) &result, (hash_data *) &dummy)) return result; return ((void *)0); } static inline qual_edge_hashset qual_edge_hashset_union(qual_edge_hashset s1, qual_edge_hashset s2) { qual_edge_hashset_scanner ss; qual_edge elt; qual_edge_hashset_scan (s2, &ss); while ((elt = qual_edge_hashset_next (&ss))) qual_edge_hashset_insert (((void *)0), &s1, elt); return s1; };
struct Qual_reason {
  enum { rk_none = 0, rk_edge = 1, rk_location = 2 } kind;
  union {
    location loc;
    qual_edge qe;
  } u;
  bool ci_bound;
};
typedef struct Qual_reason* qual_reason;
typedef struct { hash_table ht; } *qual_reason_map; typedef struct { hash_table_scanner hts; } qual_reason_map_scanner; typedef struct { hash_table_scanner_sorted htss; } qual_reason_map_scanner_sorted; static inline qual_reason_map make_qual_reason_map(region r, unsigned long size) { return (qual_reason_map) make_hash_table(r, size, (hash_fn) hash_qual, (keyeq_fn) eq_qual); } static inline void qual_reason_map_reset(qual_reason_map m) { hash_table_reset((hash_table) m); } static inline unsigned long qual_reason_map_size(qual_reason_map m) { return hash_table_size((hash_table) m); } static inline bool qual_reason_map_lookup(qual_reason_map m, qual k, qual_reason *d) { return hash_table_lookup((hash_table) m, (hash_key) k, (hash_data *) d); } static inline bool qual_reason_map_insert(qual_reason_map m, qual k, qual_reason d) { return hash_table_insert((hash_table) m, (hash_key) k, (hash_data) (intptr_t) d); } static inline bool qual_reason_map_remove(qual_reason_map m, qual k) { return hash_table_remove((hash_table) m, (hash_key) k); } static inline qual_reason_map qual_reason_map_copy(region r, qual_reason_map m) { return (qual_reason_map) hash_table_copy(r, (hash_table) m); } static inline qual_reason_map qual_reason_map_map(region r, qual_reason_map m, qual_reason (*f)(qual, qual_reason, void *), void *arg) { return (qual_reason_map) hash_table_map(r, (hash_table) m, (hash_map_fn) f, arg); } static inline void qual_reason_map_scan(qual_reason_map m, qual_reason_map_scanner *ms) { hash_table_scan((hash_table) m, &ms->hts); } static inline bool qual_reason_map_next(qual_reason_map_scanner *ms, qual *k, qual_reason *d) { return hash_table_next(&ms->hts, (hash_key *) k, (hash_data *) d); } static inline void qual_reason_map_scan_sorted(qual_reason_map m, int (*f)(qual, qual), qual_reason_map_scanner_sorted *mss) { hash_table_scan_sorted((hash_table) m, (keycmp_fn) f, &mss->htss); } static inline bool qual_reason_map_next_sorted(qual_reason_map_scanner_sorted *ms, qual *k, qual_reason *d) { return hash_table_next_sorted(&ms->htss, (hash_key *) k, (hash_data *) d); };
typedef struct Qual_error {
  qual lbc;
  qual ubc;
}* qual_error;
typedef dd_list qual_error_list; typedef dd_list_pos qual_error_list_scanner; typedef void (* qual_error_list_app_fn) (qual_error); typedef bool (* qual_error_list_eq_fn) (const qual_error); typedef int (* qual_error_list_comparator_fn)(const qual_error,const qual_error); static inline qual_error_list new_qual_error_list(region r) { return (qual_error_list)dd_new_list(r); } static inline int qual_error_list_length(qual_error_list a) { return dd_length((dd_list)a); } static inline qual_error_list qual_error_list_cons(region r, qual_error_list a, qual_error data) { dd_add_first(r, (dd_list)a, (void*)data); return a; }static inline qual_error_list qual_error_list_append(qual_error_list a, qual_error_list b) { dd_append((dd_list)a, (dd_list)b); return a; } static inline qual_error_list qual_error_list_append_elt(region r, qual_error_list a, qual_error b) { dd_add_last (r, (dd_list)a, (void*)b); return a; } static inline qual_error_list qual_error_list_app(qual_error_list a, qual_error_list_app_fn app) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { app (((qual_error)((elt)->data))); } return a; } static inline qual_error qual_error_list_find(qual_error_list a, qual_error_list_eq_fn eq) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (eq (((qual_error)((elt)->data)))) return ((qual_error)((elt)->data)); } return ((void *)0); } static inline qual_error_list qual_error_list_tail(qual_error_list a) { dd_remove (dd_first((dd_list)a)); return a; }static inline qual_error_list qual_error_list_front(qual_error_list a) { dd_remove (dd_last((dd_list)a)); return a; }static inline qual_error qual_error_list_head(qual_error_list a) { return ((qual_error)((dd_first((dd_list)a))->data)); } static inline qual_error qual_error_list_get_tail(qual_error_list a) { return ((qual_error)((dd_last((dd_list)a))->data)); } static inline qual_error_list qual_error_list_copy(region r, qual_error_list a) { return (qual_error_list)dd_copy(r,(dd_list) a); } static inline void qual_error_list_scan(qual_error_list a, qual_error_list_scanner *scan) { *scan = dd_first ((dd_list)a); }static inline bool qual_error_list_next(qual_error_list_scanner *scan, qual_error *data) { if (!(!((dd_list_pos)*scan)->next)) { *data = ((qual_error)(((dd_list_pos)*scan)->data)); *scan = (((dd_list_pos)*scan)->next); return 1; } return 0; } static inline bool qual_error_list_empty(qual_error_list a) { return ((!(dd_first(((dd_list)a)))->next)); } static inline bool qual_error_list_member(qual_error_list a, qual_error data) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (data == ((qual_error)(((dd_list_pos)a)->data))) return 1; } return 0; };
typedef struct Qual_bounds {
  qual_reason_map lbr;
  qual_reason_map ubr;
} *qual_bounds;
typedef struct QInstantiation {
  location loc;
  qual q1;
  qual q2;
  polarity p;
} *qinstantiation;
typedef dd_list qinstantiation_list; typedef dd_list_pos qinstantiation_list_scanner; typedef void (* qinstantiation_list_app_fn) (qinstantiation); typedef bool (* qinstantiation_list_eq_fn) (const qinstantiation); typedef int (* qinstantiation_list_comparator_fn)(const qinstantiation,const qinstantiation); static inline qinstantiation_list new_qinstantiation_list(region r) { return (qinstantiation_list)dd_new_list(r); } static inline int qinstantiation_list_length(qinstantiation_list a) { return dd_length((dd_list)a); } static inline qinstantiation_list qinstantiation_list_cons(region r, qinstantiation_list a, qinstantiation data) { dd_add_first(r, (dd_list)a, (void*)data); return a; }static inline qinstantiation_list qinstantiation_list_append(qinstantiation_list a, qinstantiation_list b) { dd_append((dd_list)a, (dd_list)b); return a; } static inline qinstantiation_list qinstantiation_list_append_elt(region r, qinstantiation_list a, qinstantiation b) { dd_add_last (r, (dd_list)a, (void*)b); return a; } static inline qinstantiation_list qinstantiation_list_app(qinstantiation_list a, qinstantiation_list_app_fn app) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { app (((qinstantiation)((elt)->data))); } return a; } static inline qinstantiation qinstantiation_list_find(qinstantiation_list a, qinstantiation_list_eq_fn eq) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (eq (((qinstantiation)((elt)->data)))) return ((qinstantiation)((elt)->data)); } return ((void *)0); } static inline qinstantiation_list qinstantiation_list_tail(qinstantiation_list a) { dd_remove (dd_first((dd_list)a)); return a; }static inline qinstantiation_list qinstantiation_list_front(qinstantiation_list a) { dd_remove (dd_last((dd_list)a)); return a; }static inline qinstantiation qinstantiation_list_head(qinstantiation_list a) { return ((qinstantiation)((dd_first((dd_list)a))->data)); } static inline qinstantiation qinstantiation_list_get_tail(qinstantiation_list a) { return ((qinstantiation)((dd_last((dd_list)a))->data)); } static inline qinstantiation_list qinstantiation_list_copy(region r, qinstantiation_list a) { return (qinstantiation_list)dd_copy(r,(dd_list) a); } static inline void qinstantiation_list_scan(qinstantiation_list a, qinstantiation_list_scanner *scan) { *scan = dd_first ((dd_list)a); }static inline bool qinstantiation_list_next(qinstantiation_list_scanner *scan, qinstantiation *data) { if (!(!((dd_list_pos)*scan)->next)) { *data = ((qinstantiation)(((dd_list_pos)*scan)->data)); *scan = (((dd_list_pos)*scan)->next); return 1; } return 0; } static inline bool qinstantiation_list_empty(qinstantiation_list a) { return ((!(dd_first(((dd_list)a)))->next)); } static inline bool qinstantiation_list_member(qinstantiation_list a, qinstantiation data) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (data == ((qinstantiation)(((dd_list_pos)a)->data))) return 1; } return 0; }
typedef enum cond_dir { c_leq_v, v_leq_c} cond_dir;
typedef struct Condition {
  cond_dir dir;
  qual c, left, right;
  qual_gate qgate;
  location loc;
  bool triggered;
} *cond;
typedef dd_list cond_set; typedef dd_list_pos cond_set_scanner; static inline cond_set empty_cond_set(region r) { return dd_new_list(r); } static inline cond_set cond_set_copy(region r, cond_set s) { if (s == ((void *)0)) return ((void *)0); return dd_copy(r, s); } static inline bool cond_set_empty(cond_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool cond_set_member(int (*cmp)(cond, cond), cond_set s, cond elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp, (void *) elt) != ((void *)0); } static inline int cond_set_size(cond_set s) { if (s == ((void *)0)) return 0; return dd_length(s); } static inline bool cond_set_insert(region r, cond_set *s, cond elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool cond_set_insert_last(region r, cond_set *s, cond elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline cond_set cond_set_union(cond_set s1, cond_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool cond_set_single(cond_set s) { return cond_set_size(s) == 1; } static inline void cond_set_sort(int (*cmp)(cond, cond), cond_set s) { if (s == ((void *)0)) return; dd_sort(s, (set_cmp_fn) cmp); } static inline void cond_set_remove_dups(int (*cmp)(cond, cond), cond_set s) { if (s == ((void *)0)) return; dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void cond_set_scan(cond_set s, cond_set_scanner *ss) { if (s == ((void *)0)) *ss = ((void *)0); else *ss = dd_first(s); } static inline cond cond_set_next(cond_set_scanner *ss) { cond result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((cond)((*ss)->data)); *ss = ((*ss)->next); return result; };
typedef struct po_info {
  qual_set qualifiers;
  bool flow_sensitive;
  bool nonprop;
  bool effect;
} *po_info;
struct type_qualifier {
  enum { qk_constant, qk_variable, qk_link} kind;
  union {
    struct {
      const char *name;
      const char *color;
      level_qual_t level;
      sign_qual_t sign;
      qual_set lbc, ubc;
      po_info po;
      int index;
    } elt;
    struct {
      const char *name;
      location loc;
      aloc aloc;
      store store;
      qual_edge_hashset lb, ub;
      cond_set cond_set;
      qual_bounds qbounds;
      bool interesting:1;
      bool preferred:1;
      bool param:1;
      bool passed:1;
      bool returned:1;
      bool ret:1;
      bool bak_seeded:1;
      bool fwd_seeded:1;
      bool bak_fs_seeded:1;
      bool fwd_fs_seeded:1;
      bool global:1;
      bool visited:1;
      bool anonymous:1;
    } var;
  } u;
    qual link;
    int num_equiv;
};
typedef struct { hash_table ht; } * qual_hashset; typedef struct { hash_table_scanner hts; } qual_hashset_scanner; static inline bool qual_hashset_eqfn(qual l, qual r) { return cmp_qual(l,r) == 0; } static inline qual_hashset empty_qual_hashset(region r) { return (qual_hashset) make_hash_table(r, 16, (hash_fn) hash_qual, (keyeq_fn) qual_hashset_eqfn); } static inline qual_hashset qual_hashset_copy(region r, qual_hashset s) { return (qual_hashset) hash_table_copy(r, (hash_table) s); } static inline bool qual_hashset_empty(qual_hashset s) { return hash_table_size((hash_table) s) == 0; } static inline bool qual_hashset_member(qual_hashset s, qual elt) { qual dummy; return hash_table_lookup((hash_table) s, (hash_key) elt, (hash_data*) &dummy); } static inline bool qual_hashset_hash_search(qual_hashset s, bool (*eqfn)(qual, qual), qual k, qual *d) { return hash_table_hash_search((hash_table)s, (keyeq_fn)eqfn, (hash_key)k, (hash_data*)d); } static inline unsigned long qual_hashset_size(qual_hashset s) { return hash_table_size((hash_table) s); } static inline bool qual_hashset_insert(region r, qual_hashset *s, qual elt) { return hash_table_insert((hash_table) *s, (hash_key) elt, (hash_data) elt); } static inline void qual_hashset_remove(qual_hashset *s, qual elt) { hash_table_remove((hash_table) *s, (hash_key) elt); } static inline bool qual_hashset_single(qual_hashset s) { return qual_hashset_size(s) == 1; } static inline void qual_hashset_scan(qual_hashset s, qual_hashset_scanner *ss) { hash_table_scan((hash_table) s, &ss->hts); } static inline qual qual_hashset_next(qual_hashset_scanner *ss) { qual result; int dummy; if (hash_table_next(&ss->hts, (hash_key *) &result, (hash_data *) &dummy)) return result; return ((void *)0); } static inline qual_hashset qual_hashset_union(qual_hashset s1, qual_hashset s2) { qual_hashset_scanner ss; qual elt; qual_hashset_scan (s2, &ss); while ((elt = qual_hashset_next (&ss))) qual_hashset_insert (((void *)0), &s1, elt); return s1; }
typedef dd_list qual_set_byname; typedef dd_list_pos qual_set_byname_scanner; static inline qual_set_byname empty_qual_set_byname (region r) { return dd_new_list(r); } static inline qual_set_byname qual_set_byname_copy(region r, qual_set_byname s) { if (s == ((void *)0)) return ((void *)0); else return dd_copy(r, s); } static inline bool qual_set_byname_empty(qual_set_byname s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool qual_set_byname_member(qual_set_byname s, qual elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp_qual_byname, (void *) elt) != ((void *)0); } static inline int qual_set_byname_size(qual_set_byname s) { if (s == ((void *)0)) return 0; else return dd_length(s); } static inline bool qual_set_byname_insert(region r, qual_set_byname *s, qual elt) { *s = dd_fix_null(r, *s); if (! qual_set_byname_member(*s, elt)) { dd_add_first(r, *s, (void *) elt); return 1; } return 0; } static inline bool qual_set_byname_insert_last(region r, qual_set_byname *s, qual elt) { *s = dd_fix_null(r, *s); if (! qual_set_byname_member(*s, elt)) { dd_add_last(r, *s, (void *) elt); return 1; } return 0; } static inline bool qual_set_byname_insert_nocheck(region r, qual_set_byname *s, qual elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool qual_set_byname_insert_last_nocheck(region r, qual_set_byname *s, qual elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline void qual_set_byname_remove(qual_set_byname *s, qual elt) { if (*s) dd_remove_all_matches_from(dd_first(*s), (dd_cmp_fn) cmp_qual_byname, (void *) elt); } static inline qual_set_byname qual_set_byname_union(qual_set_byname s1, qual_set_byname s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); dd_remove_dups(s1, (dd_cmp_fn)cmp_qual_byname); return s1; } static inline qual_set_byname qual_set_byname_union_nocheck(qual_set_byname s1, qual_set_byname s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool qual_set_byname_single(qual_set_byname s) { return qual_set_byname_size(s) == 1; } static inline void qual_set_byname_sort(qual_set_byname s) { if (s) dd_sort(s, (set_cmp_fn) cmp_qual_byname); } static inline void qual_set_byname_remove_dups(int (*cmp)(qual, qual), qual_set_byname s) { if (s) dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void qual_set_byname_scan(qual_set_byname s, qual_set_byname_scanner *ss) { if (s) *ss = dd_first(s); else *ss = ((void *)0); } static inline qual qual_set_byname_next(qual_set_byname_scanner *ss) { qual result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((qual)((*ss)->data)); *ss = ((*ss)->next); return result; }
typedef dd_list po_set; typedef dd_list_pos po_set_scanner; static inline po_set empty_po_set(region r) { return dd_new_list(r); } static inline po_set po_set_copy(region r, po_set s) { if (s == ((void *)0)) return ((void *)0); return dd_copy(r, s); } static inline bool po_set_empty(po_set s) { return s == ((void *)0) || ((!(dd_first((s)))->next)); } static inline bool po_set_member(int (*cmp)(po_info, po_info), po_set s, po_info elt) { return s != ((void *)0) && dd_search(s, (dd_cmp_fn) cmp, (void *) elt) != ((void *)0); } static inline int po_set_size(po_set s) { if (s == ((void *)0)) return 0; return dd_length(s); } static inline bool po_set_insert(region r, po_set *s, po_info elt) { *s = dd_fix_null(r, *s); dd_add_first(r, *s, (void *) elt); return 1; } static inline bool po_set_insert_last(region r, po_set *s, po_info elt) { *s = dd_fix_null(r, *s); dd_add_last(r, *s, (void *) elt); return 1; } static inline po_set po_set_union(po_set s1, po_set s2) { if (s1 == ((void *)0)) return s2; else if (s2 == ((void *)0)) return s1; dd_append(s1, s2); return s1; } static inline bool po_set_single(po_set s) { return po_set_size(s) == 1; } static inline void po_set_sort(int (*cmp)(po_info, po_info), po_set s) { if (s == ((void *)0)) return; dd_sort(s, (set_cmp_fn) cmp); } static inline void po_set_remove_dups(int (*cmp)(po_info, po_info), po_set s) { if (s == ((void *)0)) return; dd_remove_dups(s, (dd_cmp_fn)cmp); } static inline void po_set_scan(po_set s, po_set_scanner *ss) { if (s == ((void *)0)) *ss = ((void *)0); else *ss = dd_first(s); } static inline po_info po_set_next(po_set_scanner *ss) { po_info result; if (*ss == ((void *)0) || (!(*ss)->next)) return ((void *)0); result = ((po_info)((*ss)->data)); *ss = ((*ss)->next); return result; };
typedef dd_list qual_list; typedef dd_list_pos qual_list_scanner; typedef void (* qual_list_app_fn) (qual); typedef bool (* qual_list_eq_fn) (const qual); typedef int (* qual_list_comparator_fn)(const qual,const qual); static inline qual_list new_qual_list(region r) { return (qual_list)dd_new_list(r); } static inline int qual_list_length(qual_list a) { return dd_length((dd_list)a); } static inline qual_list qual_list_cons(region r, qual_list a, qual data) { dd_add_first(r, (dd_list)a, (void*)data); return a; }static inline qual_list qual_list_append(qual_list a, qual_list b) { dd_append((dd_list)a, (dd_list)b); return a; } static inline qual_list qual_list_append_elt(region r, qual_list a, qual b) { dd_add_last (r, (dd_list)a, (void*)b); return a; } static inline qual_list qual_list_app(qual_list a, qual_list_app_fn app) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { app (((qual)((elt)->data))); } return a; } static inline qual qual_list_find(qual_list a, qual_list_eq_fn eq) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (eq (((qual)((elt)->data)))) return ((qual)((elt)->data)); } return ((void *)0); } static inline qual_list qual_list_tail(qual_list a) { dd_remove (dd_first((dd_list)a)); return a; }static inline qual_list qual_list_front(qual_list a) { dd_remove (dd_last((dd_list)a)); return a; }static inline qual qual_list_head(qual_list a) { return ((qual)((dd_first((dd_list)a))->data)); } static inline qual qual_list_get_tail(qual_list a) { return ((qual)((dd_last((dd_list)a))->data)); } static inline qual_list qual_list_copy(region r, qual_list a) { return (qual_list)dd_copy(r,(dd_list) a); } static inline void qual_list_scan(qual_list a, qual_list_scanner *scan) { *scan = dd_first ((dd_list)a); }static inline bool qual_list_next(qual_list_scanner *scan, qual *data) { if (!(!((dd_list_pos)*scan)->next)) { *data = ((qual)(((dd_list_pos)*scan)->data)); *scan = (((dd_list_pos)*scan)->next); return 1; } return 0; } static inline bool qual_list_empty(qual_list a) { return ((!(dd_first(((dd_list)a)))->next)); } static inline bool qual_list_member(qual_list a, qual data) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (data == ((qual)(((dd_list_pos)a)->data))) return 1; } return 0; }
typedef dd_list qual_edge_list; typedef dd_list_pos qual_edge_list_scanner; typedef void (* qual_edge_list_app_fn) (qual_edge); typedef bool (* qual_edge_list_eq_fn) (const qual_edge); typedef int (* qual_edge_list_comparator_fn)(const qual_edge,const qual_edge); static inline qual_edge_list new_qual_edge_list(region r) { return (qual_edge_list)dd_new_list(r); } static inline int qual_edge_list_length(qual_edge_list a) { return dd_length((dd_list)a); } static inline qual_edge_list qual_edge_list_cons(region r, qual_edge_list a, qual_edge data) { dd_add_first(r, (dd_list)a, (void*)data); return a; }static inline qual_edge_list qual_edge_list_append(qual_edge_list a, qual_edge_list b) { dd_append((dd_list)a, (dd_list)b); return a; } static inline qual_edge_list qual_edge_list_append_elt(region r, qual_edge_list a, qual_edge b) { dd_add_last (r, (dd_list)a, (void*)b); return a; } static inline qual_edge_list qual_edge_list_app(qual_edge_list a, qual_edge_list_app_fn app) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { app (((qual_edge)((elt)->data))); } return a; } static inline qual_edge qual_edge_list_find(qual_edge_list a, qual_edge_list_eq_fn eq) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (eq (((qual_edge)((elt)->data)))) return ((qual_edge)((elt)->data)); } return ((void *)0); } static inline qual_edge_list qual_edge_list_tail(qual_edge_list a) { dd_remove (dd_first((dd_list)a)); return a; }static inline qual_edge_list qual_edge_list_front(qual_edge_list a) { dd_remove (dd_last((dd_list)a)); return a; }static inline qual_edge qual_edge_list_head(qual_edge_list a) { return ((qual_edge)((dd_first((dd_list)a))->data)); } static inline qual_edge qual_edge_list_get_tail(qual_edge_list a) { return ((qual_edge)((dd_last((dd_list)a))->data)); } static inline qual_edge_list qual_edge_list_copy(region r, qual_edge_list a) { return (qual_edge_list)dd_copy(r,(dd_list) a); } static inline void qual_edge_list_scan(qual_edge_list a, qual_edge_list_scanner *scan) { *scan = dd_first ((dd_list)a); }static inline bool qual_edge_list_next(qual_edge_list_scanner *scan, qual_edge *data) { if (!(!((dd_list_pos)*scan)->next)) { *data = ((qual_edge)(((dd_list_pos)*scan)->data)); *scan = (((dd_list_pos)*scan)->next); return 1; } return 0; } static inline bool qual_edge_list_empty(qual_edge_list a) { return ((!(dd_first(((dd_list)a)))->next)); } static inline bool qual_edge_list_member(qual_edge_list a, qual_edge data) { dd_list_pos elt; for (elt = dd_first(((dd_list)a)); !(!(elt)->next); elt = ((elt)->next)) { if (data == ((qual_edge)(((dd_list_pos)a)->data))) return 1; } return 0; }
enum {
  state_start = 0, state_init = 1, state_pos_defined = 2,
  state_finish = 3
} state = state_start;
bool exists_effect_qual = 0;
bool exists_ptrflow_qual = 0;
bool exists_fieldflow_qual = 0;
bool exists_fieldptrflow_qual = 0;
bool used_fs_qual = 0;
int flag_print_quals_graph = 0;
int flag_errors_only = 0;
int flag_context_summary = 0;
static int next_qual = 0;
static qual_set all_quals;
static qual_set all_vars;
static qual_set important_vars;
static po_set all_pos;
static po_info current_po;
static int num_constants = 0;
static qual_gate empty_qgate;
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
qual const_qual = ((void *)0);
qual nonconst_qual = ((void *)0);
qual volatile_qual = ((void *)0);
qual restrict_qual = ((void *)0);
qual noreturn_qual = ((void *)0);
qual init_qual = ((void *)0);
qual noninit_qual = ((void *)0);
int num_hotspots = 0;
qual *hotspots;
static region quals_region;
static region resolve_region;
static qinstantiation_list qinstantiations;
static bool can_unify_flow = 1;
static bool qual_gate_set_qual (qual_gate *qgate, qual q, bool allow);
static bool qual_gate_passes_qual (qual_gate *qgate, qual q, q_direction d);
static bool qual_gate_is_closed (qual_gate *qgate);
static qual ecr_qual(qual q)
{
  qual ecr, cur, temp;
  if (!q) return ((void *)0);
  if (!q->link)
      return q;
  ecr = q;
  while (ecr->link)
    ecr = ecr->link;
  cur = q;
  while (cur->link != ecr)
    {
      temp = cur->link;
      cur->link = ecr;
      cur = temp;
    }
  return ecr;
}
bool variable_qual(qual q)
{
  q = ecr_qual(q);
  return q->kind == qk_variable;
}
bool constant_qual(qual q)
{
  q = ecr_qual(q);
  return q->kind == qk_constant;
}
const char *name_qual(qual q)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant: return q->u.elt.name;
    case qk_variable: return q->u.var.name;
    default:
      __fail("poly-quals.c", 402, __FUNCTION__, "Unexpected kind %d for qualifier\n", q->kind);
    }
}
bool set_qual_name(qual q, const char *name)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant: return 0;
    case qk_variable: q->u.var.name = name; return 1;
    default:
      __fail("poly-quals.c", 414, __FUNCTION__, "Unexpected kind %d for qualifier\n", q->kind);
    }
  return 0;
}
const char *unique_name_qual(qual q)
{
  q = ecr_qual(q);
  return ptr_to_ascii(q);
}
int cmp_qual(qual left, qual right)
{
  left = ecr_qual(left);
  right = ecr_qual(right);
  if (left == right)
    return 0;
  else if (left < right)
    return -1;
  else
    return 1;
}
int cmp_qual_byname (qual left, qual right)
{
  left = ecr_qual (left);
  right = ecr_qual (right);
  return __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name_qual (left)) && __builtin_constant_p (name_qual (right)) && (__s1_len = strlen (name_qual (left)), __s2_len = strlen (name_qual (right)), (!((size_t)(const void *)((name_qual (left)) + 1) - (size_t)(const void *)(name_qual (left)) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name_qual (right)) + 1) - (size_t)(const void *)(name_qual (right)) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name_qual (left)), (__const char *) (name_qual (right)), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name_qual (left)) && ((size_t)(const void *)((name_qual (left)) + 1) - (size_t)(const void *)(name_qual (left)) == 1) && (__s1_len = strlen (name_qual (left)), __s1_len < 4) ? (__builtin_constant_p (name_qual (right)) && ((size_t)(const void *)((name_qual (right)) + 1) - (size_t)(const void *)(name_qual (right)) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[0] - ((__const unsigned char *) (__const char *)(name_qual (right)))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[1] - ((__const unsigned char *) (__const char *) (name_qual (right)))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[2] - ((__const unsigned char *) (__const char *) (name_qual (right)))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[3] - ((__const unsigned char *) (__const char *) (name_qual (right)))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name_qual (right)); register int __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name_qual (right)) && ((size_t)(const void *)((name_qual (right)) + 1) - (size_t)(const void *)(name_qual (right)) == 1) && (__s2_len = strlen (name_qual (right)), __s2_len < 4) ? (__builtin_constant_p (name_qual (left)) && ((size_t)(const void *)((name_qual (left)) + 1) - (size_t)(const void *)(name_qual (left)) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[0] - ((__const unsigned char *) (__const char *)(name_qual (right)))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[1] - ((__const unsigned char *) (__const char *) (name_qual (right)))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[2] - ((__const unsigned char *) (__const char *) (name_qual (right)))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name_qual (left)))[3] - ((__const unsigned char *) (__const char *) (name_qual (right)))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name_qual (left)); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name_qual (right)))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name_qual (right)))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name_qual (right)))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name_qual (right)))[3]); } } __result; }))) : strcmp (name_qual (left), name_qual (right))))); });
}
bool eq_qual(qual left, qual right)
{
  return (ecr_qual(left) == ecr_qual(right));
}
unsigned long hash_qual(qual q)
{
  q = ecr_qual(q);
  return (unsigned long) (intptr_t) q;
}
void begin_po_qual(void)
{
  if (current_po)
    __fail("poly-quals.c", 472, __FUNCTION__, "begin_po_qual called without previous end_po_qual\n");
  current_po = (sizeof(struct po_info) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct po_info)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct po_info), 0));
  current_po->qualifiers = empty_qual_set(quals_region);
  current_po->flow_sensitive = 0;
  current_po->nonprop = 0;
  current_po->effect = 0;
}
void end_po_qual(void)
{
  if (!current_po)
    __fail("poly-quals.c", 483, __FUNCTION__, "end_po_qual called without previous begin_po_qual\n");
  po_set_insert(quals_region, &all_pos, current_po);
  current_po = ((void *)0);
}
void set_po_flow_sensitive(void)
{
  current_po->flow_sensitive = 1;
  current_po->nonprop = 1;
}
void set_po_nonprop(void)
{
  current_po->nonprop = 1;
}
void set_po_effect(void)
{
  current_po->effect = 1;
  exists_effect_qual = 1;
}
qual add_qual(const char *name)
{
  qual new_qual;
  ((void) ((state == state_init) ? 0 : (__assert_fail ("state == state_init", "poly-quals.c", 514, __PRETTY_FUNCTION__), 0)));
  if (!current_po)
    __fail("poly-quals.c", 516, __FUNCTION__, "add_qual called without previous begin_po_qual\n");
  if ((new_qual = find_qual(name)))
    {
      if (new_qual->u.elt.po != current_po)
        __fail("poly-quals.c", 520, __FUNCTION__, "Qualifier %s in two different partial orders\n", new_qual->u.elt.name);
      return new_qual;
    }
  new_qual = (sizeof(struct type_qualifier) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct type_qualifier)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct type_qualifier), 0));
  new_qual->kind = qk_constant;
  new_qual->u.elt.name = (((void)0), __rc_rstrdup)(quals_region, name);
  new_qual->u.elt.color = ((void *)0);
  new_qual->u.elt.level = level_value;
  new_qual->u.elt.sign = sign_pos;
  new_qual->u.elt.lbc = empty_qual_set(quals_region);
  new_qual->u.elt.ubc = empty_qual_set(quals_region);
  new_qual->u.elt.po = current_po;
  new_qual->u.elt.index = num_constants++;
  ((void) ((num_constants <= 32) ? 0 : (__assert_fail ("num_constants <= 32", "poly-quals.c", 538, __PRETTY_FUNCTION__), 0)));
  qual_gate_set_qual (&open_qgate, new_qual, 1);
  if (current_po->flow_sensitive)
    qual_gate_set_qual (&fs_qgate, new_qual, 1);
  else
    qual_gate_set_qual (&fi_qgate, new_qual, 1);
  if (current_po->effect)
    qual_gate_set_qual (&effect_qgate, new_qual, 1);
  qual_set_insert(quals_region, &all_quals, new_qual);
  qual_set_insert(quals_region, &current_po->qualifiers, new_qual);
  if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("const") && (__s1_len = strlen (name), __s2_len = strlen ("const"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("const") + 1) - (size_t)(const void *)("const") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("const"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("const") && ((size_t)(const void *)(("const") + 1) - (size_t)(const void *)("const") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("const"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("const"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("const"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("const"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("const"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("const") && ((size_t)(const void *)(("const") + 1) - (size_t)(const void *)("const") == 1) && (__s2_len = strlen ("const"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("const"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("const"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("const"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("const"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("const"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("const"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("const"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("const"))[3]); } } __result; }))) : strcmp (name, "const")))); }))
    const_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("$nonconst") && (__s1_len = strlen (name), __s2_len = strlen ("$nonconst"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("$nonconst") + 1) - (size_t)(const void *)("$nonconst") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("$nonconst"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("$nonconst") && ((size_t)(const void *)(("$nonconst") + 1) - (size_t)(const void *)("$nonconst") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$nonconst"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$nonconst"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$nonconst"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$nonconst"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("$nonconst"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("$nonconst") && ((size_t)(const void *)(("$nonconst") + 1) - (size_t)(const void *)("$nonconst") == 1) && (__s2_len = strlen ("$nonconst"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$nonconst"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$nonconst"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$nonconst"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$nonconst"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("$nonconst"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("$nonconst"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("$nonconst"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("$nonconst"))[3]); } } __result; }))) : strcmp (name, "$nonconst")))); }))
    nonconst_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("volatile") && (__s1_len = strlen (name), __s2_len = strlen ("volatile"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("volatile") + 1) - (size_t)(const void *)("volatile") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("volatile"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("volatile") && ((size_t)(const void *)(("volatile") + 1) - (size_t)(const void *)("volatile") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("volatile"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("volatile"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("volatile"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("volatile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("volatile"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("volatile") && ((size_t)(const void *)(("volatile") + 1) - (size_t)(const void *)("volatile") == 1) && (__s2_len = strlen ("volatile"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("volatile"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("volatile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("volatile"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("volatile"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("volatile"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("volatile"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("volatile"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("volatile"))[3]); } } __result; }))) : strcmp (name, "volatile")))); }))
    volatile_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("restrict") && (__s1_len = strlen (name), __s2_len = strlen ("restrict"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("restrict") + 1) - (size_t)(const void *)("restrict") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("restrict"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("restrict") && ((size_t)(const void *)(("restrict") + 1) - (size_t)(const void *)("restrict") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("restrict"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("restrict"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("restrict"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("restrict"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("restrict"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("restrict") && ((size_t)(const void *)(("restrict") + 1) - (size_t)(const void *)("restrict") == 1) && (__s2_len = strlen ("restrict"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("restrict"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("restrict"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("restrict"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("restrict"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("restrict"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("restrict"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("restrict"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("restrict"))[3]); } } __result; }))) : strcmp (name, "restrict")))); }))
    restrict_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("noreturn") && (__s1_len = strlen (name), __s2_len = strlen ("noreturn"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("noreturn"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("noreturn") && ((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("noreturn"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("noreturn"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("noreturn") && ((size_t)(const void *)(("noreturn") + 1) - (size_t)(const void *)("noreturn") == 1) && (__s2_len = strlen ("noreturn"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("noreturn"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("noreturn"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("noreturn"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("noreturn"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("noreturn"))[3]); } } __result; }))) : strcmp (name, "noreturn")))); }))
    noreturn_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("$init") && (__s1_len = strlen (name), __s2_len = strlen ("$init"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("$init") + 1) - (size_t)(const void *)("$init") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("$init"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("$init") && ((size_t)(const void *)(("$init") + 1) - (size_t)(const void *)("$init") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$init"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$init"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$init"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("$init"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("$init") && ((size_t)(const void *)(("$init") + 1) - (size_t)(const void *)("$init") == 1) && (__s2_len = strlen ("$init"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$init"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$init"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$init"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("$init"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("$init"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("$init"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("$init"))[3]); } } __result; }))) : strcmp (name, "$init")))); }))
    init_qual = new_qual;
  else if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (name) && __builtin_constant_p ("$noninit") && (__s1_len = strlen (name), __s2_len = strlen ("$noninit"), (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)(("$noninit") + 1) - (size_t)(const void *)("$noninit") == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (name), (__const char *) ("$noninit"), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s1_len = strlen (name), __s1_len < 4) ? (__builtin_constant_p ("$noninit") && ((size_t)(const void *)(("$noninit") + 1) - (size_t)(const void *)("$noninit") == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$noninit"))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$noninit"))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$noninit"))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$noninit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) ("$noninit"); register int __result = (((__const unsigned char *) (__const char *) (name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p ("$noninit") && ((size_t)(const void *)(("$noninit") + 1) - (size_t)(const void *)("$noninit") == 1) && (__s2_len = strlen ("$noninit"), __s2_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (name))[0] - ((__const unsigned char *) (__const char *)("$noninit"))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[1] - ((__const unsigned char *) (__const char *) ("$noninit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (name))[2] - ((__const unsigned char *) (__const char *) ("$noninit"))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (name))[3] - ((__const unsigned char *) (__const char *) ("$noninit"))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) ("$noninit"))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) ("$noninit"))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) ("$noninit"))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) ("$noninit"))[3]); } } __result; }))) : strcmp (name, "$noninit")))); }))
    noninit_qual = new_qual;
  return new_qual;
}
void add_qual_lt(qual left, qual right)
{
  ((void) ((left->kind == qk_constant) ? 0 : (__assert_fail ("left->kind == qk_constant", "poly-quals.c", 569, __PRETTY_FUNCTION__), 0)));
  ((void) ((right->kind == qk_constant) ? 0 : (__assert_fail ("right->kind == qk_constant", "poly-quals.c", 570, __PRETTY_FUNCTION__), 0)));
  if (!qual_set_member(left->u.elt.ubc, right))
    {
      qual_set_scanner qs;
      qual q;
      qual_set_insert(quals_region, &left->u.elt.ubc, right);
      qual_set_insert(quals_region, &right->u.elt.lbc, left);
      for (qual_set_scan(right->u.elt.ubc, &qs), q = qual_set_next(&qs); q; q = qual_set_next(&qs))
        add_qual_lt(left, q);
      for (qual_set_scan(left->u.elt.lbc, &qs), q = qual_set_next(&qs); q; q = qual_set_next(&qs))
        add_qual_lt(q, right);
    }
  can_unify_flow = 0;
}
void add_color_qual(qual q, const char *color)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 601, __PRETTY_FUNCTION__), 0)));
  q->u.elt.color = (((void)0), __rc_rstrdup)(quals_region, color);
}
void add_level_qual(qual q, level_qual_t lev)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 607, __PRETTY_FUNCTION__), 0)));
  ((void) ((lev == level_value || lev == level_ref) ? 0 : (__assert_fail ("lev == level_value || lev == level_ref", "poly-quals.c", 608, __PRETTY_FUNCTION__), 0)));
  q->u.elt.level = lev;
}
void add_sign_qual(qual q, sign_qual_t sign)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 615, __PRETTY_FUNCTION__), 0)));
  ((void) ((sign == sign_pos || sign == sign_neg || sign == sign_eq) ? 0 : (__assert_fail ("sign == sign_pos || sign == sign_neg || sign == sign_eq", "poly-quals.c", 616, __PRETTY_FUNCTION__), 0)));
  q->u.elt.sign = sign;
}
void add_ptrflow_qual(qual q, flow_qual_t f)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 623, __PRETTY_FUNCTION__), 0)));
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_up_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_up_neg_qgate, q, 1);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_down_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_down_neg_qgate, q, 1);
    break;
  default:
    __fail("poly-quals.c", 638, __FUNCTION__, "unknown flow direction!\n");
  }
  exists_ptrflow_qual = 1;
}
void add_fieldflow_qual(qual q, flow_qual_t f)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 646, __PRETTY_FUNCTION__), 0)));
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_up_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_up_neg_qgate, q, 1);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_down_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_down_neg_qgate, q, 1);
    break;
  default:
    __fail("poly-quals.c", 661, __FUNCTION__, "unknown flow direction!\n");
  }
  exists_fieldflow_qual = 1;
}
void add_fieldptrflow_qual(qual q, flow_qual_t f)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 669, __PRETTY_FUNCTION__), 0)));
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_up_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_up_neg_qgate, q, 1);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_down_pos_qgate, q, 1);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_down_neg_qgate, q, 1);
    break;
  default:
    __fail("poly-quals.c", 684, __FUNCTION__, "unknown flow direction!\n");
  }
  exists_fieldptrflow_qual = 1;
}
qual_set get_constant_quals (void)
{
  return all_quals;
}
qual find_qual(const char *name)
{
  qual_set_scanner qs;
  qual q;
  for (qual_set_scan(all_quals, &qs), q = qual_set_next(&qs); q; q = qual_set_next(&qs))
    {
      ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 702, __PRETTY_FUNCTION__), 0)));
      if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (q->u.elt.name) && __builtin_constant_p (name) && (__s1_len = strlen (q->u.elt.name), __s2_len = strlen (name), (!((size_t)(const void *)((q->u.elt.name) + 1) - (size_t)(const void *)(q->u.elt.name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (q->u.elt.name), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (q->u.elt.name) && ((size_t)(const void *)((q->u.elt.name) + 1) - (size_t)(const void *)(q->u.elt.name) == 1) && (__s1_len = strlen (q->u.elt.name), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p (q->u.elt.name) && ((size_t)(const void *)((q->u.elt.name) + 1) - (size_t)(const void *)(q->u.elt.name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.elt.name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (q->u.elt.name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp (q->u.elt.name, name)))); }))
        {
          if (state >= state_pos_defined && q->u.elt.po->flow_sensitive)
            used_fs_qual = 1;
          return q;
        }
    }
  return ((void *)0);
}
void reset_flow_sensitive_quals(void)
{
  po_set_scanner ss;
  po_info po;
  for (po_set_scan(all_pos, &ss), po = po_set_next(&ss); po; po = po_set_next(&ss))
    if (po->flow_sensitive)
      po->nonprop = 0;
}
level_qual_t level_qual(qual q)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 726, __PRETTY_FUNCTION__), 0)));
  return q->u.elt.level;
}
sign_qual_t sign_qual(qual q)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 732, __PRETTY_FUNCTION__), 0)));
  return q->u.elt.sign;
}
bool flow_sensitive_qual(qual q)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 739, __PRETTY_FUNCTION__), 0)));
  return q->u.elt.po->flow_sensitive;
}
bool nonprop_qual(qual q)
{
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 746, __PRETTY_FUNCTION__), 0)));
  return q->u.elt.po->nonprop;
}
static int check_vars(qual_set vars)
{
  return 0;
}
qual make_qvar(const char *name, location loc, bool preferred, bool global)
{
  qual fresh;
  ((void) ((state >= state_pos_defined) ? 0 : (__assert_fail ("state >= state_pos_defined", "poly-quals.c", 813, __PRETTY_FUNCTION__), 0)));
  fresh = (sizeof(struct type_qualifier) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct type_qualifier)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct type_qualifier), 0));
  fresh->kind = qk_variable;
  fresh->u.var.name = name;
  fresh->u.var.loc = ((void *)0);
  fresh->u.var.store = ((void *)0);
  fresh->u.var.cond_set = ((void *)0);
  fresh->u.var.lb = empty_qual_edge_hashset(quals_region);
  fresh->u.var.ub = empty_qual_edge_hashset(quals_region);
  fresh->u.var.interesting = 0;
  fresh->u.var.preferred = preferred;
  fresh->u.var.param = 0;
  fresh->u.var.passed = 0;
  fresh->u.var.returned = 0;
  fresh->u.var.ret = 0;
  fresh->u.var.bak_seeded = 0;
  fresh->u.var.fwd_seeded = 0;
  fresh->u.var.visited = 0;
  fresh->u.var.anonymous = !preferred;
  fresh->u.var.global = global;
  fresh->u.var.loc = loc;
  fresh->u.var.qbounds = ((void *)0);
  fresh->link = ((void *)0);
  qual_set_insert_nocheck(quals_region,&all_vars,fresh);
  check_vars (all_vars);
  return fresh;
}
qual make_fresh_qvar(const char* base_name, location loc)
{
  const char *name;
  qual q;
  name = rstrcat(quals_region, base_name,
                 inttostr(quals_region, next_qual++));
  q = make_qvar(name, loc, 0,0);
  q->u.var.anonymous = 1;
  return q;
}
qual find_var(const char *name)
{
  qual_set_scanner qs;
  qual q;
  for (qual_set_scan(all_vars, &qs), q = qual_set_next(&qs); q; q = qual_set_next(&qs))
    {
      if (q->kind == qk_variable)
        {
          if (!__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (q->u.var.name) && __builtin_constant_p (name) && (__s1_len = strlen (q->u.var.name), __s2_len = strlen (name), (!((size_t)(const void *)((q->u.var.name) + 1) - (size_t)(const void *)(q->u.var.name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (q->u.var.name), (__const char *) (name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (q->u.var.name) && ((size_t)(const void *)((q->u.var.name) + 1) - (size_t)(const void *)(q->u.var.name) == 1) && (__s1_len = strlen (q->u.var.name), __s1_len < 4) ? (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (name); register int __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (name) && ((size_t)(const void *)((name) + 1) - (size_t)(const void *)(name) == 1) && (__s2_len = strlen (name), __s2_len < 4) ? (__builtin_constant_p (q->u.var.name) && ((size_t)(const void *)((q->u.var.name) + 1) - (size_t)(const void *)(q->u.var.name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[0] - ((__const unsigned char *) (__const char *)(name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (q->u.var.name))[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (q->u.var.name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (name))[3]); } } __result; }))) : strcmp (q->u.var.name, name)))); }))
            return q;
        }
    }
  return ((void *)0);
}
qual_set get_variable_quals (void)
{
  return all_vars;
}
void set_global_qual (qual q, bool global)
{
  q = ecr_qual (q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 885, __PRETTY_FUNCTION__), 0)));
  ((void) ((state < state_finish) ? 0 : (__assert_fail ("state < state_finish", "poly-quals.c", 887, __PRETTY_FUNCTION__), 0)));
  q->u.var.global = global;
}
void mk_no_qual_qual(location loc, qual q)
{
}
static bool is_ubounded_by_constant (qual q)
{
  q = ecr_qual (q);
  return q->u.var.qbounds && qual_reason_map_size (q->u.var.qbounds->ubr) > 0;
}
static bool is_lbounded_by_constant (qual q)
{
  q = ecr_qual (q);
  return q->u.var.qbounds && qual_reason_map_size (q->u.var.qbounds->lbr) > 0;
}
static bool is_bounded_by_constant (qual q)
{
  return is_ubounded_by_constant (q) && is_lbounded_by_constant (q);
}
bool store_aloc_qual(qual q, store s, aloc al)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 918, __PRETTY_FUNCTION__), 0)));
  ((void) ((!q->u.var.store && !q->u.var.aloc) ? 0 : (__assert_fail ("!q->u.var.store && !q->u.var.aloc", "poly-quals.c", 919, __PRETTY_FUNCTION__), 0)));
  q->u.var.store = s;
  q->u.var.aloc = al;
  if (is_lbounded_by_constant (q))
    propagate_store_cell_forward(s, al);
  if (is_ubounded_by_constant (q))
    propagate_store_cell_backward(s, al);
  if (! is_bounded_by_constant (q))
    return 0;
  else
    return 1;
}
bool global_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 936, __PRETTY_FUNCTION__), 0)));
  return q->u.var.global;
}
static bool param_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 943, __PRETTY_FUNCTION__), 0)));
  return q->u.var.param || global_qual(q);
}
static bool passed_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 950, __PRETTY_FUNCTION__), 0)));
  return q->u.var.passed || global_qual(q);
}
static bool ret_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 957, __PRETTY_FUNCTION__), 0)));
  return q->u.var.ret || global_qual(q);
}
static bool returned_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 964, __PRETTY_FUNCTION__), 0)));
  return q->u.var.returned || global_qual(q);
}
bool preferred_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 971, __PRETTY_FUNCTION__), 0)));
  return q->u.var.preferred;
}
location location_qual(qual q)
{
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 978, __PRETTY_FUNCTION__), 0)));
  return q->u.var.loc;
}
static qual_edge_set _lb_qual (region r, qual q, bool context_summary)
{
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;
  qual_edge_set result = empty_qual_edge_set(r);
  qual_edge qe;
  qual_edge_hashset_scanner qess;
  q = ecr_qual (q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 992, __PRETTY_FUNCTION__), 0)));
  if (q->u.var.qbounds)
    for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
      if (qr->ci_bound || context_summary)
        {
          qe = (sizeof(struct Qual_edge) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((r), sizeof(struct Qual_edge)) : (((void)0), __rc_typed_ralloc)((r), sizeof(struct Qual_edge), 0));
          qe->index = qr->kind == rk_location ? location_index (qr->u.loc) : 0;
          qe->q1 = c;
          qe->q2 = q;
          qe->kind = ek_flow;
          qe->loc = qr->kind == rk_location ? qr->u.loc : ((void *)0);
          qual_edge_set_insert (r, &result, qe);
        }
  for (qual_edge_hashset_scan(q->u.var.lb, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
    qual_edge_set_insert (r, &result, qe);
  return result;
}
qual_edge_set lb_qual (region r, qual q)
{
  return _lb_qual (r, q, 0);
}
qual_edge_set cs_lb_qual (region r, qual q)
{
  return _lb_qual (r, q, 1);
}
static qual_edge_set _ub_qual (region r, qual q, bool context_summary)
{
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;
  qual_edge_set result = empty_qual_edge_set(r);
  qual_edge qe;
  qual_edge_hashset_scanner qess;
  q = ecr_qual (q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 1032, __PRETTY_FUNCTION__), 0)));
  if (q->u.var.qbounds)
    for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
      if (qr->ci_bound || context_summary)
        {
          qe = (sizeof(struct Qual_edge) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((r), sizeof(struct Qual_edge)) : (((void)0), __rc_typed_ralloc)((r), sizeof(struct Qual_edge), 0));
          qe->index = qr->kind == rk_location ? location_index (qr->u.loc) : 0;
          qe->q1 = q;
          qe->q2 = c;
          qe->kind = ek_flow;
          qe->loc = qr->kind == rk_location ? qr->u.loc : ((void *)0);
          qual_edge_set_insert (r, &result, qe);
        }
  for (qual_edge_hashset_scan(q->u.var.ub, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
    qual_edge_set_insert (r, &result, qe);
  return result;
}
qual_edge_set ub_qual (region r, qual q)
{
  return _ub_qual (r, q, 0);
}
qual_edge_set cs_ub_qual (region r, qual q)
{
  return _ub_qual (r, q, 1);
}
int error_count_qual(qual q)
{
  return 0;
}
static bool _has_lb_qual(qual q, qual bound, bool context_summary)
{
  qual_edge_set_scanner qess;
  qual_edge qe;
  region scratch = newregion();
  bool result = 0;
  q = ecr_qual (q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 1083, __PRETTY_FUNCTION__), 0)));
  for (qual_edge_set_scan(_lb_qual(scratch, q, context_summary), &qess), qe = qual_edge_set_next(&qess); qe; qe = qual_edge_set_next(&qess))
    if (qe->kind == ek_flow)
      {
        if (eq_qual(bound, qe->q1))
          {
            result = 1;
            break;
          }
      }
  deleteregion (scratch);
  return result;
}
bool has_lb_qual(qual q, qual bound)
{
  return _has_lb_qual (q, bound, 0);
}
bool cs_has_lb_qual(qual q, qual bound)
{
  return _has_lb_qual (q, bound, 1);
}
static bool _has_ub_qual(qual q, qual bound, bool context_summary)
{
  qual_edge_set_scanner qess;
  qual_edge qe;
  region scratch = newregion();
  bool result = 0;
  q = ecr_qual (q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 1118, __PRETTY_FUNCTION__), 0)));
  for (qual_edge_set_scan(_ub_qual(scratch, q, context_summary), &qess), qe = qual_edge_set_next(&qess); qe; qe = qual_edge_set_next(&qess))
    if (qe->kind == ek_flow)
      {
        if (eq_qual(bound, qe->q1))
          {
            result = 1;
            break;
          }
      }
  deleteregion (scratch);
  return result;
}
bool has_ub_qual(qual q, qual bound)
{
  return _has_ub_qual (q, bound, 0);
}
bool cs_has_ub_qual(qual q, qual bound)
{
  return _has_ub_qual (q, bound, 1);
}
bool has_qual(qual q, qual bound)
{
  return has_lb_qual(q, bound) || has_ub_qual(q, bound);
}
bool cs_has_qual(qual q, qual bound)
{
  return cs_has_lb_qual(q, bound) || cs_has_ub_qual(q, bound);
}
bool has_fs_qual(qual q)
{
  qual qc;
  qual_reason qr;
  qual_reason_map_scanner qrms;
  q = ecr_qual (q);
  if (! q->u.var.qbounds)
    return 0;
  for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
    if ((qr->kind == rk_location || qr->kind == rk_none) &&
        qc->u.elt.po->flow_sensitive)
      return 1;
  for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
    if ((qr->kind == rk_location || qr->kind == rk_none) &&
        qc->u.elt.po->flow_sensitive)
      return 1;
  return 0;
}
static bool _leq_qual (qual q1, qual q2, bool context_summary)
{
  qual_reason qr;
  ((void) ((state == state_finish) ? 0 : (__assert_fail ("state == state_finish", "poly-quals.c", 1193, __PRETTY_FUNCTION__), 0)));
  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);
  if (eq_qual (q1, q2))
    return 1;
  if (constant_qual(q1) && variable_qual(q2))
    {
      qual c;
      qual_reason_map_scanner qrms;
      if (!q2->u.var.qbounds)
        return 0;
      for (qual_reason_map_scan(q2->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
        if (qr->ci_bound || context_summary)
          if (leq_qual (q1, c))
            return 1;
      return 0;
    }
  else if (variable_qual(q1) && constant_qual(q2))
    {
      qual c;
      qual_reason_map_scanner qrms;
      if (!q2->u.var.qbounds)
        return 0;
      for (qual_reason_map_scan(q2->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
        if (qr->ci_bound || context_summary)
          if (leq_qual (c, q2))
            return 1;
      return 0;
     }
  else if (constant_qual(q1) && constant_qual(q2))
    {
      return qual_set_member (q1->u.elt.ubc, q2);
    }
  else
    {
      ((void) ((0) ? 0 : (__assert_fail ("0", "poly-quals.c", 1237, __PRETTY_FUNCTION__), 0)));
    }
}
bool leq_qual (qual q1, qual q2)
{
  return _leq_qual (q1, q2, 0);
}
bool cs_leq_qual (qual q1, qual q2)
{
  return _leq_qual (q1, q2, 1);
}
static bool _may_leq_qual(qual left, qual right, bool context_summary,
                      qual_error_list* error_list, region r)
{
  qual_reason_map_scanner scan_lb, scan_ub;
  qual lbc, ubc;
  qual_reason lbr, ubr;
  bool retval = 1;
  left = ecr_qual(left);
  right = ecr_qual(right);
  if (constant_qual(left) && constant_qual(right))
    {
      ((void) ((state >= state_pos_defined) ? 0 : (__assert_fail ("state >= state_pos_defined", "poly-quals.c", 1272, __PRETTY_FUNCTION__), 0)));
      if (left == right)
        return 1;
      return (qual_set_member(left->u.elt.ubc, right) ||
              !qual_set_member(left->u.elt.po->qualifiers, right));
    }
  else if (variable_qual(left) && constant_qual(right))
    {
      if (! left->u.var.qbounds)
        return 1;
      for (qual_reason_map_scan(left->u.var.qbounds->lbr, &scan_lb); qual_reason_map_next(&scan_lb, &lbc, &lbr);)
        if (lbr->ci_bound || context_summary)
          if (! may_leq_qual (lbc, right))
            return 0;
      return 1;
    }
  else if (constant_qual(left) && variable_qual(right))
    {
      if (! right->u.var.qbounds)
        return 1;
      for (qual_reason_map_scan(right->u.var.qbounds->ubr, &scan_ub); qual_reason_map_next(&scan_ub, &ubc, &ubr);)
        if (ubr->ci_bound || context_summary)
          if (! may_leq_qual (left, ubc))
            return 0;
      return 1;
    }
  if (! left->u.var.qbounds)
    return 1;
  if (! right->u.var.qbounds)
    return 1;
  for (qual_reason_map_scan(left->u.var.qbounds->lbr, &scan_lb); qual_reason_map_next(&scan_lb, &lbc, &lbr);)
    if (lbr->ci_bound || context_summary)
      for (qual_reason_map_scan(right->u.var.qbounds->ubr, &scan_ub); qual_reason_map_next(&scan_ub, &ubc, &ubr);)
        if (ubr->ci_bound || context_summary)
          {
            ((void) ((constant_qual(lbc) && constant_qual(ubc)) ? 0 : (__assert_fail ("constant_qual(lbc) && constant_qual(ubc)", "poly-quals.c", 1329, __PRETTY_FUNCTION__), 0)));
            if (! may_leq_qual (lbc, ubc))
              {
                if (error_list != ((void *)0))
                  {
                    qual_error qe = (sizeof(struct Qual_error) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((r), sizeof(struct Qual_error)) : (((void)0), __rc_typed_ralloc)((r), sizeof(struct Qual_error), 0));
                    qe->lbc = lbc;
                    qe->ubc = ubc;
                    *error_list = qual_error_list_cons (r, *error_list, qe);
                    retval = 0;
                  }
                else
                  return 0;
              }
          }
  return retval;
}
bool may_leq_qual(qual left, qual right)
{
  return _may_leq_qual(left, right, 0, ((void *)0), ((void *)0));
}
bool cs_may_leq_qual(qual left, qual right)
{
  return _may_leq_qual(left, right, 1, ((void *)0), ((void *)0));
}
static void reset_visited (qual_set vars)
{
  qual_set_scanner qss;
  qual q;
  for (qual_set_scan(vars, &qss), q = qual_set_next(&qss); q; q = qual_set_next(&qss))
    if (q->kind == qk_variable)
      q->u.var.visited = 0;
}
int print_qual_raw(printf_func pf, qual q)
{
  if (!q)
    __fail("poly-quals.c", 1388, __FUNCTION__, "Null qualifier in print_qual_raw\n");
  if (q->link)
    return print_qual_raw(pf, q->link);
  switch(q->kind)
    {
    case qk_constant: return pf("%s", q->u.elt.name);
    case qk_variable: return pf("%s", q->u.var.name);
    default: __fail("poly-quals.c", 1395, __FUNCTION__, "Unexpected kind %d for qualifier\n", q->kind);
    }
}
static int _print_qual(printf_func pf, qual q, bool context_summary)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant:
      return print_qual_raw(pf,q);
    case qk_variable:
      if (q->u.var.qbounds)
        {
          qual_reason_map_scanner qrms;
          qual_reason qr;
          qual qc;
          region scratch;
          qual_set_byname bounds;
          qual_set_scanner qss;
          int printed = 0;
          bool first = 1;
          scratch = newregion();
          bounds = empty_qual_set(scratch);
          for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
            if ((qr->ci_bound || context_summary) &&
                (sign_qual(qc) == sign_pos || sign_qual(qc) == sign_eq))
              qual_set_byname_insert (scratch, &bounds, qc);
          for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
            if ((qr->ci_bound || context_summary) &&
                sign_qual(qc) == sign_neg)
              qual_set_byname_insert (scratch, &bounds, qc);
          qual_set_byname_remove_dups (cmp_qual_byname, bounds);
          qual_set_byname_sort (bounds);
          for (qual_set_byname_scan(bounds, &qss), qc = qual_set_byname_next(&qss); qc; qc = qual_set_byname_next(&qss))
            {
              if (!first)
                printed += pf (" ");
              printed += print_qual_raw (pf, qc);
              first = 0;
            }
          deleteregion (scratch);
          return printed;
        }
      return 0;
    default:
      ((void) ((0) ? 0 : (__assert_fail ("0", "poly-quals.c", 1449, __PRETTY_FUNCTION__), 0)));
    }
}
int print_qual(printf_func pf, qual q)
{
  return _print_qual (pf, q, 0);
}
int cs_print_qual(printf_func pf, qual q)
{
  return _print_qual (pf, q, 1);
}
void scan_qual_bounds(qual q, qual_traverse_fn f, void *arg)
{
  qual_reason_map_scanner qrms;
  qual p;
  qual_reason qr;
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 1473, __PRETTY_FUNCTION__), 0)));
  if (! q->u.var.qbounds)
     return;
  for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &p, &qr);)
    if ((sign_qual(p) == sign_pos) || (sign_qual(p) == sign_eq))
      f(p, arg);
  for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &p, &qr);)
    if (sign_qual(p) == sign_neg)
      f(p, arg);
}
const char *combine_colors_pam(const char *color1, const char *color2)
{
  if (!color1) return color2;
  if (!color2) return color1;
  if (__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (color1) && __builtin_constant_p (color2) && (__s1_len = strlen (color1), __s2_len = strlen (color2), (!((size_t)(const void *)((color1) + 1) - (size_t)(const void *)(color1) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((color2) + 1) - (size_t)(const void *)(color2) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (color1), (__const char *) (color2), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (color1) && ((size_t)(const void *)((color1) + 1) - (size_t)(const void *)(color1) == 1) && (__s1_len = strlen (color1), __s1_len < 4) ? (__builtin_constant_p (color2) && ((size_t)(const void *)((color2) + 1) - (size_t)(const void *)(color2) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (color1))[0] - ((__const unsigned char *) (__const char *)(color2))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[1] - ((__const unsigned char *) (__const char *) (color2))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[2] - ((__const unsigned char *) (__const char *) (color2))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (color1))[3] - ((__const unsigned char *) (__const char *) (color2))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (color2); register int __result = (((__const unsigned char *) (__const char *) (color1))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (color1))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (color2) && ((size_t)(const void *)((color2) + 1) - (size_t)(const void *)(color2) == 1) && (__s2_len = strlen (color2), __s2_len < 4) ? (__builtin_constant_p (color1) && ((size_t)(const void *)((color1) + 1) - (size_t)(const void *)(color1) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (color1))[0] - ((__const unsigned char *) (__const char *)(color2))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[1] - ((__const unsigned char *) (__const char *) (color2))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (color1))[2] - ((__const unsigned char *) (__const char *) (color2))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (color1))[3] - ((__const unsigned char *) (__const char *) (color2))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (color1); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (color2))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (color2))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (color2))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (color2))[3]); } } __result; }))) : strcmp (color1, color2)))); }) == 0)
    return color1;
  return "pam-color-5";
}
void color_qual_fn(qual q, void *arg)
{
  const char **color = (const char **) arg;
  *color = combine_colors_pam(*color, color_qual(q));
}
const char *color_qual(qual q)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant:
      return q->u.elt.color;
    case qk_variable:
      {
        const char *color = ((void *)0);
        scan_qual_bounds(q, color_qual_fn, &color);
        return color;
        break;
      }
    default:
      __fail("poly-quals.c", 1522, __FUNCTION__, "Unexpected kind %d for qualifier\n", q->kind);
    }
}
static void qual_gate_init (qual_gate * qgate, bool backward, bool forward)
{
  qgate->backward = backward;
  qgate->forward = forward;
  (__extension__ (__builtin_constant_p (sizeof (qgate->mask)) && (sizeof (qgate->mask)) <= 16 ? ((sizeof (qgate->mask)) == 1 ? ({ void *__s = (qgate->mask); *((__uint8_t *) __s) = (__uint8_t) 0; __s; }) : ({ void *__s = (qgate->mask); union { unsigned int __ui; unsigned short int __usi; unsigned char __uc; } *__u = __s; __uint8_t __c = (__uint8_t) (0); switch ((unsigned int) (sizeof (qgate->mask))) { case 15: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 11: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 7: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 3: __u->__usi = (unsigned short int) __c * 0x0101; __u = __extension__ ((void *) __u + 2); __u->__uc = (unsigned char) __c; break; case 14: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 10: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 6: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 2: __u->__usi = (unsigned short int) __c * 0x0101; break; case 13: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 9: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 5: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 1: __u->__uc = (unsigned char) __c; break; case 16: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 12: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 8: __u->__ui = __c * 0x01010101; __u = __extension__ ((void *) __u + 4); case 4: __u->__ui = __c * 0x01010101; case 0: break; } __s; })) : (__builtin_constant_p (0) && (0) == '\0' ? ({ void *__s = (qgate->mask); __builtin_memset (__s, '\0', sizeof (qgate->mask)); __s; }) : memset (qgate->mask, 0, sizeof (qgate->mask)))));
}
static bool qual_gate_get (gate_word *mask, unsigned int bitindex)
{
  int index;
  int getmask;
  index = bitindex / (sizeof (gate_word) * 8);
  getmask = 1 << (bitindex % (sizeof (gate_word) * 8));
  ((void) ((index < num_constants) ? 0 : (__assert_fail ("index < num_constants", "poly-quals.c", 1546, __PRETTY_FUNCTION__), 0)));
  return (mask[index] & getmask) != 0;
}
static bool qual_gate_set (gate_word *mask, unsigned int bitindex, bool val)
{
  int index;
  int setmask;
  bool old;
  index = bitindex / (sizeof (gate_word) * 8);
  setmask = 1 << (bitindex % (sizeof (gate_word) * 8));
  ((void) ((index < num_constants) ? 0 : (__assert_fail ("index < num_constants", "poly-quals.c", 1557, __PRETTY_FUNCTION__), 0)));
  old = (mask[index] & setmask) != 0;
  if (val)
    mask[index] |= setmask;
  else
    mask[index] &= (~setmask);
  return old;
}
static void qual_gate_set_all (gate_word *mask, bool val)
{
  gate_word newval = val ? -1 : 0;
  int i;
  for (i = 0; i < (((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8)); i++)
    mask[i] = newval & open_qgate.mask[i];
}
static bool qual_gate_and (gate_word *a, gate_word *b, gate_word *r)
{
  int i;
  bool rval = 1;
  for (i = 0; i < (((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8)); i++)
    {
      r[i] = a[i] & b[i] & open_qgate.mask[i];
      rval = rval || (r[i] != 0);
    }
  return rval;
}
static void qual_gate_or (gate_word *a, gate_word *b, gate_word *r)
{
  int i;
  for (i = 0; i < (((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8)); i++)
    {
      r[i] = (a[i] | b[i]) & open_qgate.mask[i];
    }
}
static bool qual_gate_set_qual (qual_gate *qgate, qual q, bool allow)
{
  q = ecr_qual (q);
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 1600, __PRETTY_FUNCTION__), 0)));
  return qual_gate_set (qgate->mask, q->u.elt.index, allow);
}
static bool qual_gate_get_qual (qual_gate *qgate, qual q)
{
  q = ecr_qual (q);
  ((void) ((q->kind == qk_constant) ? 0 : (__assert_fail ("q->kind == qk_constant", "poly-quals.c", 1640, __PRETTY_FUNCTION__), 0)));
  return qual_gate_get (qgate->mask, q->u.elt.index);
}
static bool qual_gate_passes_qual (qual_gate *qgate, qual q, q_direction d)
{
  switch (d)
    {
    case d_fwd:
      if (! qgate->forward)
        return 0;
      break;
    case d_bak:
      if (! qgate->backward)
        return 0;
      break;
    default:
      __fail("poly-quals.c", 1657, __FUNCTION__, "Unknown direction in qual_gate_passes\n");
    }
  return qual_gate_get_qual (qgate, q);
}
static bool qual_gate_is_closed (qual_gate *qgate)
{
  int i;
  if (!(qgate->backward || qgate->forward))
    return 1;
  for (i = 0; i < (((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8)); i++)
    if (qgate->mask[i] != 0)
      return 0;
  return 1;
}
static bool compose_qual_gates (qual_gate *a, qual_gate *b, qual_gate *r)
{
  bool forward = a->forward && b->forward;
  bool backward = a->backward && b->backward;
  gate_word rm[(((32) + (sizeof (gate_word) * 8) - 1) / (sizeof (gate_word) * 8))];
  if (! (forward || backward))
    return 0;
  r->forward = forward;
  r->backward = backward;
  if (!qual_gate_and (a->mask, b->mask, rm))
    return 0;
  memcpy (r->mask, rm, sizeof (r->mask));
  return 1;
}
static void qual_gate_to_string (qual_gate* qgate, char* buf, int len)
{
  static struct { qual_gate *qgate; char* name; } gates[] =
    {
      {&open_qgate, "ALL" },
      {&fi_qgate, "FI" },
      {&fs_qgate, "FS" },
      {&effect_qgate, "EFF" },
      {&ptrflow_down_pos_qgate, "PTR-D-P" },
      {&ptrflow_down_neg_qgate, "PTR-D-N" },
      {&ptrflow_up_pos_qgate, "PTR-U-P" },
      {&ptrflow_up_neg_qgate, "PTR-U-N" },
      {&fieldflow_down_pos_qgate, "FIELD-D-P" },
      {&fieldflow_down_neg_qgate, "FIELD-D-N" },
      {&fieldflow_up_pos_qgate, "FIELD-U-P" },
      {&fieldflow_up_neg_qgate, "FIELD-U-N" },
      {&fieldptrflow_down_pos_qgate, "FPTR-D-P" },
      {&fieldptrflow_down_neg_qgate, "FPTR-D-N" },
      {&fieldptrflow_up_pos_qgate, "FPTR-U-P" },
      {&fieldptrflow_up_neg_qgate, "FPTR-U-N" }
    };
  int i;
  for (i = 0; i < sizeof (gates) / sizeof (gates[0]); i++)
    if (!memcmp (qgate, gates[i].qgate, sizeof (*qgate)))
      {
        (__extension__ (__builtin_constant_p (gates[i].name) && __builtin_constant_p (len - 1) ? (strlen (gates[i].name) + 1 >= ((size_t) (len - 1)) ? (char *) memcpy (buf, gates[i].name, len - 1) : strncpy (buf, gates[i].name, len - 1)) : strncpy (buf, gates[i].name, len - 1)));
        buf[len-1] = '\0';
        return;
      }
  (__extension__ (__builtin_constant_p ("CUSTOM") && __builtin_constant_p (len - 1) ? (strlen ("CUSTOM") + 1 >= ((size_t) (len - 1)) ? (char *) memcpy (buf, "CUSTOM", len - 1) : strncpy (buf, "CUSTOM", len - 1)) : strncpy (buf, "CUSTOM", len - 1)));
  buf[len-1] = '\0';
}
qual qual_edge_other_end (qual_edge qe, qual q)
{
  if (eq_qual (qe->q1, q))
    return qe->q2;
  else
    return qe->q1;
}
location qual_edge_loc (qual_edge qe)
{
  return qe->loc;
}
unsigned long hash_qual_edge(qual_edge e)
{
  return e->index;
}
int cmp_qual_edge (qual_edge e1, qual_edge e2)
{
  int r;
  if ((r = cmp_qual (e1->q1, e2->q1)))
    return r;
  if ((r = cmp_qual (e1->q2, e2->q2)))
    return r;
  if ((r = __extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (e1->loc->filename) && __builtin_constant_p (e2->loc->filename) && (__s1_len = strlen (e1->loc->filename), __s2_len = strlen (e2->loc->filename), (!((size_t)(const void *)((e1->loc->filename) + 1) - (size_t)(const void *)(e1->loc->filename) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((e2->loc->filename) + 1) - (size_t)(const void *)(e2->loc->filename) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (e1->loc->filename), (__const char *) (e2->loc->filename), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (e1->loc->filename) && ((size_t)(const void *)((e1->loc->filename) + 1) - (size_t)(const void *)(e1->loc->filename) == 1) && (__s1_len = strlen (e1->loc->filename), __s1_len < 4) ? (__builtin_constant_p (e2->loc->filename) && ((size_t)(const void *)((e2->loc->filename) + 1) - (size_t)(const void *)(e2->loc->filename) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[0] - ((__const unsigned char *) (__const char *)(e2->loc->filename))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[1] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[2] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[3] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (e2->loc->filename); register int __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (e2->loc->filename) && ((size_t)(const void *)((e2->loc->filename) + 1) - (size_t)(const void *)(e2->loc->filename) == 1) && (__s2_len = strlen (e2->loc->filename), __s2_len < 4) ? (__builtin_constant_p (e1->loc->filename) && ((size_t)(const void *)((e1->loc->filename) + 1) - (size_t)(const void *)(e1->loc->filename) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[0] - ((__const unsigned char *) (__const char *)(e2->loc->filename))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[1] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[2] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (e1->loc->filename))[3] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (e1->loc->filename); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (e2->loc->filename))[3]); } } __result; }))) : strcmp (e1->loc->filename, e2->loc->filename)))); })))
    return r;
  if (((r = e1->loc->filepos - e2->loc->filepos)))
    return r;
  return e1->kind - e2->kind;
}
int cmp_qual_edge_exact (qual_edge e1, qual_edge e2)
{
  return memcmp (e1, e2, sizeof (*e1));
}
bool eq_qual_edge_exact (qual_edge e1, qual_edge e2)
{
  return cmp_qual_edge_exact(e1, e2) == 0;
}
int cmp_qual_edge_semantic(qual_edge e1, qual_edge e2)
{
  int r;
  if ((r = e1->kind - e2->kind))
    return r;
  if ((r = ecr_qual(e1->q1) - ecr_qual(e2->q1)))
    return r;
  if ((r = ecr_qual(e1->q2) - ecr_qual(e2->q2)))
    return r;
  if ((r = e1->index - e2->index))
    return r;
  if ((r = e1->qgate.backward - e2->qgate.backward))
    return r;
  if ((r = e1->qgate.forward - e2->qgate.forward))
    return r;
  return 0;
}
bool eq_qual_edge_semantic (qual_edge e1, qual_edge e2)
{
  return cmp_qual_edge_semantic(e1, e2) == 0;
}
static inline bool mkqual_edge(location loc, qedge_kind s, qual q1, qual q2,
                               qual_gate* qgate, qual_edge *ne)
{
  struct Qual_edge tqe;
  bool retval = 0;
  qual_edge result = ((void *)0);
  int l = !loc ? 0 : location_index(loc);
  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);
  if (! (qgate->backward || qgate->forward))
    return 0;
  if (qual_gate_is_closed (qgate))
    return 0;
  tqe.kind = s;
  tqe.summary = 0;
  tqe.q1 = q1;
  tqe.q2 = q2;
  tqe.index = l;
  tqe.loc = loc;
  tqe.qgate = *qgate;
  if (! qual_edge_hashset_hash_search(q1->u.var.ub, eq_qual_edge_semantic,
                                      &tqe, &result))
    {
      result = (sizeof(struct Qual_edge) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct Qual_edge)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct Qual_edge), 0));
      memcpy (result, &tqe, sizeof (*result));
      qual_edge_hashset_insert(quals_region,&q1->u.var.ub,result);
      qual_edge_hashset_insert(quals_region,&q2->u.var.lb,result);
      retval = 1;
    }
  else
    {
      qual_gate_or (result->qgate.mask, qgate->mask,
                    result->qgate.mask);
    }
  if (ne)
    *ne = result;
  return retval;
}
static polarity qedge_kind_to_polarity (qedge_kind ek)
{
  switch (ek)
    {
    case ek_close:
      return p_pos;
    case ek_open:
      return p_neg;
    case ek_flow:
      return p_non;
    default:
      printf ("Unknown edge_kind: %d\n", (int)ek);
    }
  return p_non;
}
static cond mkcond(cond_dir dir, qual_gate *qgate, qual c, qual left,
                   qual right, location loc)
{
  cond cond;
  cond = (sizeof(struct Condition) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct Condition)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct Condition), 0));
  cond->dir = dir;
  cond->c = c;
  cond->qgate = *qgate;
  cond->left = left;
  cond->right = right;
  cond->loc = loc;
  cond->triggered = 0;
  return cond;
}
bool cond_mkleq_qual(location loc, qual_gate *qgate1, qual l1, qual r1,
                     qual_gate *qgate2, qual l2, qual r2)
{
  ((void) ((constant_qual(l1) || constant_qual(r1)) ? 0 : (__assert_fail ("constant_qual(l1) || constant_qual(r1)", "poly-quals.c", 1946, __PRETTY_FUNCTION__), 0)));
  l1 = ecr_qual(l1);
  r1 = ecr_qual(r1);
  l2 = ecr_qual(l2);
  r2 = ecr_qual(r2);
  if (leq_qual(l1, r1))
    return mkleq_qual(loc, qgate2, l2, r2);
  else if (constant_qual(l1) && constant_qual(r1))
    return 0;
  else
    {
      qual v, c;
      cond_dir dir;
      if (constant_qual(l1))
        { c = l1; v = ecr_qual(r1); dir = c_leq_v; }
      else
        { c = r1; v = ecr_qual(l1); dir = v_leq_c; }
      ((void) ((constant_qual(c) && variable_qual(v)) ? 0 : (__assert_fail ("constant_qual(c) && variable_qual(v)", "poly-quals.c", 1978, __PRETTY_FUNCTION__), 0)));
      cond_set_insert(quals_region, &v->u.var.cond_set,
                      mkcond(dir, qgate2, c, l2, r2, loc));
      return 0;
    }
}
static bool find_flow_path (region r, qual current, qual target,
                            qual_hashset visited, qual_edge_list path)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;
  current = ecr_qual(current);
  ((void) ((variable_qual(current)) ? 0 : (__assert_fail ("variable_qual(current)", "poly-quals.c", 2127, __PRETTY_FUNCTION__), 0)));
  if (eq_qual (current, target))
    return 1;
  if (qual_hashset_member(visited,current))
    return 0;
  qual_hashset_insert(r, &visited, current);
  for (qual_edge_hashset_scan(current->u.var.ub, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
    if (qe->kind == ek_flow)
      {
        path = qual_edge_list_append_elt(r, path, qe);
        if (find_flow_path(r,qe->q2,target,visited,path))
          return 1;
        path = qual_edge_list_front(path);
      }
  return 0;
}
static qual_edge_list find_summary_path (region r, qual_edge e)
{
  qual_edge_list qel = ((void *)0);
  qual_edge_list tmp;
  qual_hashset visited;
  region scratch;
  qual q1;
  qual q2;
  qual_edge_hashset_scanner qess1;
  qual_edge_hashset_scanner qess2;
  qual_edge qe1;
  qual_edge qe2;
  ((void) ((e->summary) ? 0 : (__assert_fail ("e->summary", "poly-quals.c", 2165, __PRETTY_FUNCTION__), 0)));
  q1 = ecr_qual (e->q1);
  q2 = ecr_qual (e->q2);
  scratch = newregion();
  tmp = new_qual_edge_list (scratch);
  visited = empty_qual_hashset(scratch);
  if (global_qual(q1))
    if (global_qual(q2))
      if (find_flow_path(scratch, q1, q2, visited, tmp))
        {
          qel = qual_edge_list_copy(r, tmp);
          goto DONE;
        }
  if (global_qual(q1))
    for (qual_edge_hashset_scan(q2->u.var.lb, &qess2), qe2 = qual_edge_hashset_next(&qess2); qe2; qe2 = qual_edge_hashset_next(&qess2))
      if (qe2->kind == ek_close)
        if (find_flow_path(scratch, q1, ecr_qual(qe2->q1), visited, tmp))
          {
            qel = qual_edge_list_copy(r, tmp);
            qel = qual_edge_list_append_elt (r, qel, qe2);
            goto DONE;
          }
  if (global_qual(q2))
    for (qual_edge_hashset_scan(q1->u.var.ub, &qess1), qe1 = qual_edge_hashset_next(&qess1); qe1; qe1 = qual_edge_hashset_next(&qess1))
      if (qe1->kind == ek_open)
        if (find_flow_path(scratch, ecr_qual(qe1->q2), q2, visited, tmp))
          {
            qel = qual_edge_list_copy(r, tmp);
            qel = qual_edge_list_cons (r, qel, qe1);
            goto DONE;
          }
  for (qual_edge_hashset_scan(q1->u.var.ub, &qess1), qe1 = qual_edge_hashset_next(&qess1); qe1; qe1 = qual_edge_hashset_next(&qess1))
    if (qe1->kind == ek_open)
      for (qual_edge_hashset_scan(q2->u.var.lb, &qess2), qe2 = qual_edge_hashset_next(&qess2); qe2; qe2 = qual_edge_hashset_next(&qess2))
        if (qe2->kind == ek_close)
          if (find_flow_path(scratch, ecr_qual(qe1->q2), ecr_qual(qe2->q1),
                             visited, tmp))
            {
              qel = qual_edge_list_copy(r, tmp);
              qel = qual_edge_list_cons (r, qel, qe1);
              qel = qual_edge_list_append_elt (r, qel, qe2);
              goto DONE;
            }
 DONE:
  deleteregion(scratch);
  return qel;
}
bool traversing_head;
bool bidi;
edge_traverse_fn current_f;
void* current_arg;
static void traverse_error_path_edges_fn(qual_edge e);
static void summary_edge_app(qual_edge qe)
{
  region scratch;
  qual_edge_list qel;
  scratch = newregion();
  qel = find_summary_path(scratch, qe);
  ((void) ((qel) ? 0 : (__assert_fail ("qel", "poly-quals.c", 2234, __PRETTY_FUNCTION__), 0)));
  qual_edge_list_app(qel, traverse_error_path_edges_fn);
  deleteregion(scratch);
}
static void traverse_error_path_edges_fn(qual_edge e)
{
  qual q1 = ecr_qual(e->q1);
  qual q2 = ecr_qual(e->q2);
  ((void) ((current_f) ? 0 : (__assert_fail ("current_f", "poly-quals.c", 2245, __PRETTY_FUNCTION__), 0)));
  if (e->summary)
    summary_edge_app (e);
  else
    current_f(q1,q2,e->loc,qedge_kind_to_polarity(e->kind),
              traversing_head, bidi, current_arg);
  traversing_head = 0;
}
static void _traverse_reason_path_edges(qual q, qual b, bool upper)
{
  qual_bounds qbounds;
  qual_reason qr;
  qbounds = q->u.var.qbounds;
  if (upper)
    {
      qual_reason_map_lookup(qbounds->ubr, b, &qr);
      if (qr->kind == rk_location)
        {
          current_f (q, b, qr->u.loc, p_non,
                     traversing_head, bidi,
                     current_arg);
          return;
        }
    }
  else
    {
      qual_reason_map_lookup(qbounds->lbr, b, &qr);
      if (qr->kind == rk_location)
        {
          current_f (b, q, qr->u.loc, p_non,
                     traversing_head, bidi,
                     current_arg);
          return;
        }
    }
  if (qr->u.qe->summary)
    summary_edge_app (qr->u.qe);
  else
    current_f (qr->u.qe->q1, qr->u.qe->q2, qr->u.qe->loc,
               qedge_kind_to_polarity(qr->u.qe->kind),
               traversing_head, bidi, current_arg);
  if (eq_qual (q, qr->u.qe->q1))
    {
      q = ecr_qual(qr->u.qe->q2);
      upper = 1;
    }
  else
    {
      q = ecr_qual(qr->u.qe->q1);
      upper = 0;
    }
  traversing_head = !upper;
  _traverse_reason_path_edges(q, b, upper);
}
static void _traverse_error_path_edges(qual q, bool context_summary,
                                       edge_traverse_fn f, void *arg)
{
  qual_error_list qel;
  qual_error_list_scanner scan;
  qual_error next_error;
  region scratch;
  q = ecr_qual(q);
  ((void) ((q->kind == qk_variable) ? 0 : (__assert_fail ("q->kind == qk_variable", "poly-quals.c", 2318, __PRETTY_FUNCTION__), 0)));
  current_f = f;
  current_arg = arg;
  scratch = newregion();
  qel = new_qual_error_list (scratch);
  _may_leq_qual (q, q, context_summary, &qel, scratch);
  qual_error_list_scan(qel,&scan);
  while(qual_error_list_next(&scan,&next_error))
    {
      qual loffender = next_error->lbc;
      qual uoffender = next_error->ubc;
      if (loffender)
        {
          traversing_head = 1;
          bidi = loffender->u.elt.sign == sign_eq;
          _traverse_reason_path_edges(q,loffender,0);
        }
      if (uoffender)
        {
          traversing_head = 1;
          bidi = uoffender->u.elt.sign == sign_eq;
          _traverse_reason_path_edges(q,uoffender,1);
        }
    }
  deleteregion (scratch);
}
void traverse_error_path_edges(qual q, edge_traverse_fn f, void *arg)
{
  return _traverse_error_path_edges (q, 0, f, arg);
}
void cs_traverse_error_path_edges(qual q, edge_traverse_fn f, void *arg)
{
  return _traverse_error_path_edges (q, 1, f, arg);
}
static FILE* print_graph_file_target;
const char *polarity_to_string(polarity p)
{
  switch (p)
    {
    case p_neg:
      return "-";
    case p_pos:
      return "+";
    case p_non:
      return "T";
    case p_sub:
      return "S";
    default:
      fprintf (stderr, "unknown polarity %d.\n", p);
      abort ();
    }
  ((void) ((0) ? 0 : (__assert_fail ("0", "poly-quals.c", 2423, __PRETTY_FUNCTION__), 0)));
  return "ERROR";
}
static int print_graph_file(const char *fmt, ...)
{
  va_list args;
  __builtin_stdarg_start((args),fmt);
  return vfprintf(print_graph_file_target, fmt, args);
}
static void print_graph_name (FILE* f, const qual q)
{
  print_qual_raw(print_graph_file, q);
  if (flag_ugly)
    print_graph_file ("(%p)", q);
  if (q->kind == qk_variable && q->u.var.global)
    print_graph_file ("[g]");
}
static void add_graph_node (FILE* f, const qual q)
{
  print_graph_file_target = f;
  fprintf(f, "\"");
  print_graph_name (f, q);
  fprintf(f, "\"\n");
}
static void add_graph_edge(FILE* f, const qual left, const qual right,
                    const char* label, const bool dotted)
{
  print_graph_file_target = f;
  fprintf(f, "\"");
  print_graph_name(f, left);
  fprintf(f, "\"->\"");
  print_graph_name(f, right);
  fprintf(f, "\"");
  fprintf (f, "[");
  if (label)
    fprintf(f, "label=\"%s\",", label);
  fprintf(f, "style=%s", dotted ? "dotted" : "solid");
  fprintf (f, "]\n");
}
static char* location_label (location loc)
{
  static char buf[1024];
  snprintf (buf, sizeof(buf), "%s:%ld", loc->filename, loc->lineno);
  buf[sizeof(buf)-1] = '\0';
  return buf;
}
static char* edge_label(const qual_edge e)
{
  static char gate_buf[1024];
  static char buf[1024];
  qual_gate_to_string (&e->qgate, gate_buf, sizeof (gate_buf));
  snprintf (buf, 1000, "%d,%s,%s",
            location_index(e->loc),
            polarity_to_string(qedge_kind_to_polarity(e->kind)),
            gate_buf);
  buf[sizeof(buf)-1] = '\0';
  return buf;
}
static void dump_quals_graph_constant_bounds (FILE* f, qual q)
{
  if (q->u.var.qbounds)
    {
      qual_reason_map_scanner qrms;
      qual qc;
      qual_reason qr;
      for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
        {
          if (qr->ci_bound)
            {
              if (qr->kind == rk_location)
                add_graph_edge (f, qc, q, location_label (qr->u.loc), 0);
              else
                add_graph_edge (f, qc, q, "*", 0);
            }
        }
      for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &qc, &qr);)
        {
          if (qr->ci_bound)
            {
              if (qr->kind == rk_location)
                add_graph_edge (f, q, qc, location_label (qr->u.loc), 0);
              else
                add_graph_edge (f, q, qc, "*", 0);
            }
        }
    }
}
static void _dn_dfs (FILE* f, region scratch,
              qual root, int depth, qual_edge_set* printed_edges)
{
  qual_edge_hashset_scanner qess;
  qual_edge e;
  ((void) ((root->kind == qk_variable) ? 0 : (__assert_fail ("root->kind == qk_variable", "poly-quals.c", 2528, __PRETTY_FUNCTION__), 0)));
  if (root->u.var.visited)
    return;
  root->u.var.visited = 1;
  if (root->link)
    {
      add_graph_edge (f, root, ecr_qual (root), "+", 1);
      return _dn_dfs (f, scratch, ecr_qual (root), depth, printed_edges);
    }
  for (qual_edge_hashset_scan(root->u.var.lb, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
    {
      qual q = ecr_qual (e->q1);
      if (! q->link)
        {
          if ((depth > 0 || q->u.var.visited) &&
              ! qual_edge_set_member (cmp_qual_edge, *printed_edges, e))
            {
              add_graph_edge (f, q, root, edge_label (e), 0);
              qual_edge_set_insert (scratch, printed_edges, e);
            }
          if (depth > 0)
            _dn_dfs (f, scratch, q, depth-1, printed_edges);
        }
    }
  for (qual_edge_hashset_scan(root->u.var.ub, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
    {
      qual q = ecr_qual (e->q2);
      if (! q->link)
        {
          if ((depth > 0 || q->u.var.visited) &&
              ! qual_edge_set_member (cmp_qual_edge, *printed_edges, e))
            {
              add_graph_edge (f, root, q, edge_label (e), 0);
              qual_edge_set_insert (scratch, printed_edges, e);
            }
          if (depth > 0)
            _dn_dfs (f, scratch, q, depth-1, printed_edges);
        }
    }
  dump_quals_graph_constant_bounds (f, root);
}
static void _dump_neighborhood (FILE* f, qual_set vars, qual start, int size)
{
  qual_edge_set printed_edges;
  region scratch;
  reset_visited (vars);
  scratch = newregion();
  printed_edges = empty_qual_edge_set(scratch);
  _dn_dfs (f, scratch, start, size, &printed_edges);
  deleteregion (scratch);
}
static void dump_graph_init (FILE* f)
{
  fprintf(f, "digraph G {\n");
}
static void dump_graph_finish (FILE* f)
{
  fprintf (f, "}\n");
}
static void dump_neighborhood (FILE* f, qual_set vars, qual root, int depth)
{
  dump_graph_init (f);
  _dump_neighborhood (f, vars, root, depth);
  dump_graph_finish (f);
}
static void dump_quals_graph (FILE* f, qual_set vars)
{
  qual_set_scanner qss;
  qual q;
  qual_edge_hashset_scanner qess;
  qual_edge e;
  qual_edge_set printed_edges;
  region scratch;
  scratch = newregion();
  printed_edges = empty_qual_edge_set(scratch);
  dump_graph_init (f);
  for (qual_set_scan(vars, &qss), q = qual_set_next(&qss); q; q = qual_set_next(&qss))
    {
      q = ecr_qual(q);
      add_graph_node (f, q);
      for (qual_edge_hashset_scan(q->u.var.ub, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
        {
          if (!qual_edge_set_member (cmp_qual_edge, printed_edges, e))
            {
              add_graph_edge (f, q, ecr_qual(e->q2), edge_label (e), 0);
              qual_edge_set_insert (scratch, &printed_edges, e);
            }
        }
      for (qual_edge_hashset_scan(q->u.var.lb, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
        {
          if (!qual_edge_set_member (cmp_qual_edge, printed_edges, e))
            {
              add_graph_edge (f, ecr_qual(e->q1), q, edge_label (e), 0);
              qual_edge_set_insert (scratch, &printed_edges, e);
            }
        }
      dump_quals_graph_constant_bounds (f, q);
    }
  dump_graph_finish (f);
  deleteregion (scratch);
}
static inline bool insert_X_bound (location loc,
                                   qual constant, qual var,
                                   qual_edge qe,
                                   bool upper, bool ci_bound)
{
  bool result = 0;
  qual_bounds bounds;
  qual_reason qr;
  bool already_bounds;
  bool bound_upgrade;
  var = ecr_qual(var);
  ((void) ((constant->kind == qk_constant && var->kind == qk_variable) ? 0 : (__assert_fail ("constant->kind == qk_constant && var->kind == qk_variable", "poly-quals.c", 2760, __PRETTY_FUNCTION__), 0)));
  if (!(bounds = var->u.var.qbounds))
    {
      bounds = (sizeof(struct Qual_bounds) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct Qual_bounds)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct Qual_bounds), 0));
      bounds->lbr = make_qual_reason_map (quals_region, 2);
      bounds->ubr = make_qual_reason_map (quals_region, 2);
      var->u.var.qbounds = bounds;
    }
  var->u.var.interesting = 1;
  if (upper)
    already_bounds = qual_reason_map_lookup(bounds->ubr, constant, &qr);
  else
    already_bounds = qual_reason_map_lookup(bounds->lbr, constant, &qr);
  if (!already_bounds)
    {
      qr = (sizeof(struct Qual_reason) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((quals_region), sizeof(struct Qual_reason)) : (((void)0), __rc_typed_ralloc)((quals_region), sizeof(struct Qual_reason), 0));
      qr->kind = rk_none;
      qr->ci_bound = 0;
      if (upper)
        {
          qual_reason_map_insert (bounds->ubr, constant, qr);
        }
      else
        {
          qual_reason_map_insert (bounds->lbr, constant, qr);
        }
      result = 1;
    }
  bound_upgrade = !qr->ci_bound && ci_bound;
  if (bound_upgrade)
    result = 1;
  qr->ci_bound |= ci_bound;
  if (loc && (qr->kind < rk_location || bound_upgrade))
    {
      qr->kind = rk_location;
      qr->u.loc = loc;
      result = 1;
    }
  else if (qe && (qr->kind < rk_edge || bound_upgrade))
    {
      qr->kind = rk_edge;
      qr->u.qe = qe;
      result = 1;
    }
  if (!already_bounds && var->u.var.store)
    {
      if (upper)
        propagate_store_cell_backward(var->u.var.store, var->u.var.aloc);
      else
        propagate_store_cell_forward(var->u.var.store, var->u.var.aloc);
    }
  return result;
}
static inline bool insert_lower_bound(location loc, qual q1, qual q2,
                                      qual_edge qe,
                                      bool ci_bound)
{
  if (insert_X_bound (loc, q1, q2, qe, 0, ci_bound))
    return 1;
  else
    return 0;
}
static inline bool insert_upper_bound(location loc, qual q1, qual q2,
                                      qual_edge qe,
                                      bool ci_bound)
{
  if (insert_X_bound (loc, q2, q1, qe, 1, ci_bound))
    return 2;
  else
    return 0;
}
static bool _mkinst_qual(location loc, qual_gate *qgate, qual left, qual right,
                         polarity p)
{
  qinstantiation inst;
  qual q1 = ecr_qual(left);
  qual q2 = ecr_qual(right);
  if (!flag_poly)
    {
      if (p != p_non)
        {
          return mkleq_qual (loc, qgate, left, right);
        }
      else
        {
          return mkleq_qual (loc, qgate, left, right);
          return mkleq_qual (loc, qgate, right, left);
        }
    }
  ((void) ((q1->kind == qk_variable) ? 0 : (__assert_fail ("q1->kind == qk_variable", "poly-quals.c", 2929, __PRETTY_FUNCTION__), 0)));
  ((void) ((q2->kind == qk_variable) ? 0 : (__assert_fail ("q2->kind == qk_variable", "poly-quals.c", 2930, __PRETTY_FUNCTION__), 0)));
  inst = (sizeof(struct QInstantiation) < (1 << (13 - 3)) ? (((void)0), __rc_ralloc_small0)((resolve_region), sizeof(struct QInstantiation)) : (((void)0), __rc_typed_ralloc)((resolve_region), sizeof(struct QInstantiation), 0));
  inst->loc = loc;
  inst->p = p;
  inst->q1 = q1;
  inst->q2 = q2;
  qinstantiation_list_cons(resolve_region, qinstantiations, inst);
  if (p == p_pos)
    {
      mkqual_edge(loc,ek_close,q1,q2, qgate, ((void *)0));
      q1->u.var.ret = 1;
      q2->u.var.returned = 1;
    }
  else if (p == p_neg)
    {
      mkqual_edge(loc,ek_open,q1,q2, qgate, ((void *)0));
      q1->u.var.passed = 1;
      q2->u.var.param = 1;
    }
  else
    {
      mkqual_edge(loc,ek_flow,q1,q2, qgate, ((void *)0));
      mkqual_edge(loc,ek_flow,q2,q1, qgate, ((void *)0));
    }
  check_vars (all_vars);
  return 0;
}
bool mkinst_qual(location loc, qual_gate *qgate, qual left, qual right, polarity p)
{
  return _mkinst_qual(loc,qgate,left,right,p);
}
static int _mkleq_qual(location loc, qual left, qual right,
                       bool orig, bool ci_bound,
                       qual_gate* qgate,
                       qual_edge *new_edge, qual_edge causal_edge)
{
  bool result = 0;
  qual q1 = ecr_qual(left);
  qual q2 = ecr_qual(right);
  ((void) ((!q1->link) ? 0 : (__assert_fail ("!q1->link", "poly-quals.c", 2994, __PRETTY_FUNCTION__), 0)));
  ((void) ((!q2->link) ? 0 : (__assert_fail ("!q2->link", "poly-quals.c", 2995, __PRETTY_FUNCTION__), 0)));
  if (new_edge)
    *new_edge = ((void *)0);
  if (q1->kind == qk_constant && q2->kind == qk_constant)
    {
      __fail("poly-quals.c", 3015, __FUNCTION__, "mkleq_qual called on two qualifier constants");
    }
  else if (q1->kind == qk_constant && q2->kind == qk_variable)
    {
      result |= insert_lower_bound(loc, q1,q2, causal_edge, ci_bound);
      if (sign_qual(q1) == sign_eq || qual_set_empty (q1->u.elt.ubc))
        result |= insert_upper_bound(loc, q2,q1, causal_edge, ci_bound);
    }
  else if (q1->kind == qk_variable && q2->kind == qk_constant)
    {
      result |= insert_upper_bound(loc, q1,q2, causal_edge, ci_bound);
      if (sign_qual(q2) == sign_eq || qual_set_empty (q2->u.elt.lbc))
        result |= insert_lower_bound(loc, q2,q1, causal_edge, ci_bound);
    }
  else if (q1->kind == qk_variable && q2->kind == qk_variable)
    {
      mkqual_edge(loc,ek_flow,q1,q2, qgate, new_edge);
      result |= 4;
    }
  return result;
}
bool mkleq_qual(location loc, qual_gate *qgate, qual left, qual right)
{
  _mkleq_qual(loc,left,right,1,1,qgate,((void *)0),((void *)0));
  check_vars (all_vars);
  return 0;
}
bool mkeq_qual(location loc, qual_gate* qgate, qual left, qual right)
{
  bool result;
  result = mkleq_qual(loc, qgate, left, right);
  result = mkleq_qual(loc, qgate, right, left) || result;
  return result;
}
static void bounds_union(qual_bounds new, qual_bounds old)
{
  qual tq;
  qual_reason_map_scanner qrms;
  qual_reason tqr1;
  qual_reason tqr2;
  for (qual_reason_map_scan(old->lbr, &qrms); qual_reason_map_next(&qrms, &tq, &tqr1);)
    if (!qual_reason_map_lookup(new->lbr, tq, &tqr2))
      qual_reason_map_insert (new->lbr, tq, tqr1);
  for (qual_reason_map_scan(old->ubr, &qrms); qual_reason_map_next(&qrms, &tq, &tqr1);)
    if (!qual_reason_map_lookup(new->ubr, tq, &tqr2))
      qual_reason_map_insert (new->ubr, tq, tqr1);
}
bool unify_qual(location loc, qual left, qual right)
{
  bool result = 0;
  left = ecr_qual(left);
  right = ecr_qual(right);
  if (left == right)
    return 0;
  result = mkeq_qual(loc, &open_qgate, left, right);
  if (left->kind == qk_variable && right->kind == qk_variable)
    {
      qual new_ecr, new_link;
      if (right->u.var.preferred || left->u.var.anonymous ||
          (left->num_equiv <= right->num_equiv))
        {
          new_ecr = right;
          new_link = left;
        }
      else
        {
          new_ecr = left;
          new_link = right;
        }
      if (preferred_qual(left) && !preferred_qual(right))
        new_ecr->u.var.name = left->u.var.name;
      else if (!preferred_qual(left) && preferred_qual(right))
        new_ecr->u.var.name = right->u.var.name;
      else if (__extension__ ({ size_t __s1_len, __s2_len; (__builtin_constant_p (left->u.var.name) && __builtin_constant_p (right->u.var.name) && (__s1_len = strlen (left->u.var.name), __s2_len = strlen (right->u.var.name), (!((size_t)(const void *)((left->u.var.name) + 1) - (size_t)(const void *)(left->u.var.name) == 1) || __s1_len >= 4) && (!((size_t)(const void *)((right->u.var.name) + 1) - (size_t)(const void *)(right->u.var.name) == 1) || __s2_len >= 4)) ? memcmp ((__const char *) (left->u.var.name), (__const char *) (right->u.var.name), (__s1_len < __s2_len ? __s1_len : __s2_len) + 1) : (__builtin_constant_p (left->u.var.name) && ((size_t)(const void *)((left->u.var.name) + 1) - (size_t)(const void *)(left->u.var.name) == 1) && (__s1_len = strlen (left->u.var.name), __s1_len < 4) ? (__builtin_constant_p (right->u.var.name) && ((size_t)(const void *)((right->u.var.name) + 1) - (size_t)(const void *)(right->u.var.name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[0] - ((__const unsigned char *) (__const char *)(right->u.var.name))[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[1] - ((__const unsigned char *) (__const char *) (right->u.var.name))[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[2] - ((__const unsigned char *) (__const char *) (right->u.var.name))[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[3] - ((__const unsigned char *) (__const char *) (right->u.var.name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s2 = (__const unsigned char *) (__const char *) (right->u.var.name); register int __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[0] - __s2[0]); if (__s1_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[1] - __s2[1]); if (__s1_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[2] - __s2[2]); if (__s1_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[3] - __s2[3]); } } __result; }))) : (__builtin_constant_p (right->u.var.name) && ((size_t)(const void *)((right->u.var.name) + 1) - (size_t)(const void *)(right->u.var.name) == 1) && (__s2_len = strlen (right->u.var.name), __s2_len < 4) ? (__builtin_constant_p (left->u.var.name) && ((size_t)(const void *)((left->u.var.name) + 1) - (size_t)(const void *)(left->u.var.name) == 1) ? (__extension__ ({ register int __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[0] - ((__const unsigned char *) (__const char *)(right->u.var.name))[0]); if (__s2_len > 0 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[1] - ((__const unsigned char *) (__const char *) (right->u.var.name))[1]); if (__s2_len > 1 && __result == 0) { __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[2] - ((__const unsigned char *) (__const char *) (right->u.var.name))[2]); if (__s2_len > 2 && __result == 0) __result = (((__const unsigned char *) (__const char *) (left->u.var.name))[3] - ((__const unsigned char *) (__const char *) (right->u.var.name))[3]); } } __result; })) : (__extension__ ({ __const unsigned char *__s1 = (__const unsigned char *) (__const char *) (left->u.var.name); register int __result = __s1[0] - ((__const unsigned char *) (__const char *) (right->u.var.name))[0]; if (__s2_len > 0 && __result == 0) { __result = (__s1[1] - ((__const unsigned char *) (__const char *) (right->u.var.name))[1]); if (__s2_len > 1 && __result == 0) { __result = (__s1[2] - ((__const unsigned char *) (__const char *) (right->u.var.name))[2]); if (__s2_len > 2 && __result == 0) __result = (__s1[3] - ((__const unsigned char *) (__const char *) (right->u.var.name))[3]); } } __result; }))) : strcmp (left->u.var.name, right->u.var.name)))); }) > 0)
        new_ecr->u.var.name = right->u.var.name;
      else
        new_ecr->u.var.name = new_ecr->u.var.name;
      if (new_link->u.var.qbounds)
        {
          if (new_ecr->u.var.qbounds)
            {
              bounds_union(new_ecr->u.var.qbounds,new_link->u.var.qbounds);
            }
          else
            {
              new_ecr->u.var.qbounds = new_link->u.var.qbounds;
            }
        }
      new_ecr->num_equiv += new_link->num_equiv;
      new_ecr->u.var.lb = qual_edge_hashset_union(new_ecr->u.var.lb,
                                              new_link->u.var.lb);
      new_ecr->u.var.ub = qual_edge_hashset_union(new_ecr->u.var.ub,
                                              new_link->u.var.ub);
      new_ecr->u.var.interesting |= new_link->u.var.interesting ;
      new_ecr->u.var.preferred |= new_link->u.var.preferred;
      new_ecr->u.var.param |= new_link->u.var.param;
      new_ecr->u.var.passed |= new_link->u.var.passed;
      new_ecr->u.var.returned |= new_link->u.var.returned;
      new_ecr->u.var.ret |= new_link->u.var.ret;
      new_ecr->u.var.bak_seeded |= new_link->u.var.bak_seeded;
      new_ecr->u.var.fwd_seeded |= new_link->u.var.fwd_seeded;
      new_ecr->u.var.global |= new_link->u.var.global;
      new_ecr->u.var.anonymous &= new_link->u.var.anonymous;
      new_link->link = new_ecr;
      new_link->kind = qk_link;
    }
  check_vars (all_vars);
  return result;
}
bool copy_constraint (qual_edge e, qual q1, qual q2)
{
  if (constant_qual (q1) || constant_qual (q2))
    return mkleq_qual(e->loc, &e->qgate, q1, q2);
  mkqual_edge (e->loc, e->kind, q1, q2, &e->qgate, ((void *)0));
  return 0;
}
static bool _resolve_wellformed()
{
  region scratch = newregion();
  term_hash h = make_term_hash(scratch);
  qinstantiation_list_scanner scan;
  qinstantiation next_inst;
  bool result = 0;
  qinstantiation_list_scan(qinstantiations,&scan);
  while(qinstantiation_list_next(&scan,&next_inst))
    {
      qual q_prime;
      int index = location_index(next_inst->loc);
      qual q1 = ecr_qual(next_inst->q1);
      qual q2 = ecr_qual(next_inst->q2);
      stamp sig[2] = {index,(stamp)q1};
      if ( (q_prime = ecr_qual(term_hash_find(h,sig,2))) != ((void *)0))
        {
          if (!eq_qual(q_prime,q2))
            {
              unify_qual(next_inst->loc,q2,q_prime);
              result = 1;
            }
        }
      else
        term_hash_insert(h,(term)q2,sig,2);
    }
  term_hash_delete (h);
  deleteregion(scratch);
  return result;
}
static void resolve_wellformed()
{
  bool changed = 1;
  int iterations = 0;
  while(changed)
    {
      changed = _resolve_wellformed();
      iterations++;
    }
}
static bool is_bound_compatible (qual_edge qe, qual_reason r, qual c, q_direction d)
{
  return ! nonprop_qual (c) && qual_gate_passes_qual (&qe->qgate, c, d);
}
static bool new_summary (location loc, qual q1, qual q2,
                         qual_edge elb, qual_edge eub,
                         qual_gate *path_qgate)
{
  qual_edge se;
  bool result;
  qual_gate qgate = *path_qgate;
  if (elb && ! compose_qual_gates (&elb->qgate, &qgate, &qgate))
    return 0;
  if (eub && ! compose_qual_gates (&qgate, &eub->qgate, &qgate))
    return 0;
  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);
  result = mkqual_edge (loc, ek_flow, q1, q2, &qgate, &se);
  if (result)
    {
      se->summary = 1;
    }
  return result;
}
static bool new_summaries (qual q1, qual q2, qual_gate* qgate)
{
  qual_edge_hashset_scanner lbscan,ubscan;
  qual_edge elb, eub;
  bool result = 0;
  for (qual_edge_hashset_scan(q1->u.var.lb, &lbscan), elb = qual_edge_hashset_next(&lbscan); elb; elb = qual_edge_hashset_next(&lbscan))
    if (elb->kind == ek_open)
      for (qual_edge_hashset_scan(q2->u.var.ub, &ubscan), eub = qual_edge_hashset_next(&ubscan); eub; eub = qual_edge_hashset_next(&ubscan))
        if (eub->kind == ek_close && eub->index == elb->index)
          result |= new_summary (elb->loc, elb->q1, eub->q2, elb, eub, qgate);
  if (global_qual(q1))
    for (qual_edge_hashset_scan(q2->u.var.ub, &ubscan), eub = qual_edge_hashset_next(&ubscan); eub; eub = qual_edge_hashset_next(&ubscan))
      if (eub->kind == ek_close)
        result |= new_summary (q1->u.var.loc, q1, eub->q2, ((void *)0), eub, qgate);
  if (global_qual(q2))
    for (qual_edge_hashset_scan(q1->u.var.lb, &lbscan), elb = qual_edge_hashset_next(&lbscan); elb; elb = qual_edge_hashset_next(&lbscan))
      if (elb->kind == ek_open)
        result |= new_summary (elb->loc, elb->q1, q2, elb, ((void *)0), qgate);
  return result;
}
static bool find_bak_summaries (qual q);
static bool _find_bak_summaries (region r, qual origin, qual q,
                                 qual_gate* qgate, qual_hashset visited)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;
  bool retval = 0;
  bool repeat = 1;
  q = ecr_qual (q);
  if (qual_hashset_member (visited, q))
    return 0;
  qual_hashset_insert (r, &visited, q);
  if (param_qual (q))
    retval |= new_summaries (q, origin, qgate);
  while (repeat)
    {
      repeat = 0;
      for (qual_edge_hashset_scan(q->u.var.lb, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
        {
          qual_gate new_qgate;
          if (!compose_qual_gates (&qe->qgate, qgate, &new_qgate))
            continue;
          if (qe->kind == ek_flow)
            {
              retval |= _find_bak_summaries (r, origin, qe->q1,
                                             &new_qgate, visited);
            }
          if (qe->kind == ek_close)
            repeat |= find_bak_summaries (qe->q1);
        }
    }
  return retval;
}
static bool find_bak_summaries (qual q)
{
  region scratch;
  qual_hashset visited;
  qual_gate qgate;
  bool result;
  q = ecr_qual (q);
  if (q->u.var.bak_seeded)
    return 0;
  q->u.var.bak_seeded = 1;
  scratch = newregion();
  visited = empty_qual_hashset (scratch);
  qgate.backward = 1;
  qgate.forward = 1;
  qual_gate_set_all (qgate.mask, 1);
  result = _find_bak_summaries (scratch, q, q, &qgate, visited);
  deleteregion(scratch);
  return result;
}
static bool find_fwd_summaries (qual q);
static bool _find_fwd_summaries (region r, qual origin, qual q,
                                 qual_gate* qgate, qual_hashset visited)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;
  bool retval = 0;
  bool repeat = 1;
  q = ecr_qual (q);
  if (qual_hashset_member (visited, q))
    return 0;
  qual_hashset_insert (r, &visited, q);
  if (ret_qual (q))
    retval |= new_summaries (origin, q, qgate);
  while (repeat)
    {
      repeat = 0;
      for (qual_edge_hashset_scan(q->u.var.ub, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
        {
          qual_gate new_qgate;
          if (!compose_qual_gates (qgate, &qe->qgate, &new_qgate))
            continue;
          if (qe->kind == ek_flow)
            {
              retval |= _find_fwd_summaries (r, origin, qe->q2, &new_qgate,
                                             visited);
            }
          if (qe->kind == ek_open)
            repeat |= find_fwd_summaries (qe->q2);
        }
    }
  return retval;
}
static bool find_fwd_summaries (qual q)
{
  region scratch;
  qual_hashset visited;
  qual_gate qgate;
  bool result;
  q = ecr_qual (q);
  if (q->u.var.fwd_seeded)
    return 0;
  q->u.var.fwd_seeded = 1;
  scratch = newregion();
  visited = empty_qual_hashset (scratch);
  qgate.backward = 1;
  qgate.forward = 1;
  qual_gate_set_all (qgate.mask, 1);
  result = _find_fwd_summaries (scratch, q, q, &qgate, visited);
  deleteregion(scratch);
  return result;
}
static void cfl_flow (qual_set vars)
{
  region scratch;
  region newscratch;
  qual_hashset pf_worklist;
  qual_hashset newpf_worklist;
  qual_hashset_scanner qhss;
  qual_set_scanner qss;
  qual q;
  scratch = newregion();
  pf_worklist = empty_qual_hashset(scratch);
  for (qual_set_scan(vars, &qss), q = qual_set_next(&qss); q; q = qual_set_next(&qss))
    if (q->u.var.qbounds)
      qual_hashset_insert (scratch, &pf_worklist, ecr_qual (q));
  while (qual_hashset_size(pf_worklist) > 0)
    {
      newscratch = newregion();
      newpf_worklist = empty_qual_hashset(newscratch);
      for (qual_hashset_scan(pf_worklist, &qhss), q = qual_hashset_next(&qhss); q; q = qual_hashset_next(&qhss))
        {
          qual_edge qe;
          qual_edge_hashset_scanner qess;
          if (returned_qual (q))
            for (qual_edge_hashset_scan(q->u.var.lb, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
              if (qe->kind == ek_close)
                find_bak_summaries (qe->q1);
          for (qual_edge_hashset_scan(q->u.var.lb, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
            {
              qual_reason_map_scanner qrms;
              qual_reason qr;
              qual c;
              if (qe->kind == ek_close || flag_context_summary)
                continue;
              for (qual_reason_map_scan(q->u.var.qbounds->ubr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
                if (is_bound_compatible(qe, qr, c, d_bak))
                  {
                    if (qe->kind == ek_flow || qe->kind == ek_open)
                      {
                        int r;
                        r = _mkleq_qual (((void *)0), qe->q1, c, 0, qr->ci_bound,
                                         &qe->qgate,
                                         ((void *)0), qe);
                        if ((((r) & 1) != 0) || (((r) & 2) != 0))
                          qual_hashset_insert (newscratch, &newpf_worklist,
                                               ecr_qual (qe->q1));
                      }
                    else if (flag_context_summary)
                      {
                        int r;
                        r = _mkleq_qual (((void *)0), qe->q1, c, 0, 0,
                                         &qe->qgate,
                                         ((void *)0), qe);
                        if ((((r) & 1) != 0) || (((r) & 2) != 0))
                          qual_hashset_insert (newscratch, &newpf_worklist,
                                               ecr_qual (qe->q1));
                      }
                  }
            }
          if (passed_qual (q))
            for (qual_edge_hashset_scan(q->u.var.ub, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
              if (qe->kind == ek_open)
                find_fwd_summaries (qe->q2);
          for (qual_edge_hashset_scan(q->u.var.ub, &qess), qe = qual_edge_hashset_next(&qess); qe; qe = qual_edge_hashset_next(&qess))
            {
              qual_reason_map_scanner qrms;
              qual_reason qr;
              qual c;
              if (qe->kind == ek_open || flag_context_summary)
                continue;
              for (qual_reason_map_scan(q->u.var.qbounds->lbr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
                if (is_bound_compatible(qe, qr, c, d_fwd))
                  {
                    if (qe->kind == ek_flow || qe->kind == ek_close)
                      {
                        int r;
                        r = _mkleq_qual (((void *)0), c, qe->q2, 0, qr->ci_bound,
                                         &qe->qgate,
                                         ((void *)0), qe);
                        if ((((r) & 1) != 0) || (((r) & 2) != 0))
                          qual_hashset_insert (newscratch, &newpf_worklist,
                                               ecr_qual(qe->q2));
                      }
                    else if (flag_context_summary)
                      {
                        int r;
                        r = _mkleq_qual (((void *)0), c, qe->q2, 0, 0,
                                         &qe->qgate,
                                         ((void *)0), qe);
                        if ((((r) & 1) != 0) || (((r) & 2) != 0))
                          qual_hashset_insert (newscratch, &newpf_worklist,
                                               ecr_qual(qe->q2));
                      }
                  }
            }
        }
      pf_worklist = newpf_worklist;
      deleteregion (scratch);
      scratch = newscratch;
    }
  deleteregion(scratch);
}
static void fiv_dfs (qual q, qual_set* conn_comp, qual_set* cc_cbounds)
{
  qual_edge_hashset_scanner qess;
  qual_edge e;
  qual_bounds qb = ((void *)0);
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;
  q = ecr_qual (q);
  if (q->kind != qk_variable || q->u.var.visited)
    return;
  qual_set_insert_nocheck (quals_region, conn_comp, q);
  qb = q->u.var.qbounds;
  if (qb)
    {
      for (qual_reason_map_scan(qb->lbr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
        {
          if (qr->ci_bound)
            {
              ((void) ((c->kind == qk_constant) ? 0 : (__assert_fail ("c->kind == qk_constant", "poly-quals.c", 3673, __PRETTY_FUNCTION__), 0)));
              qual_set_insert (quals_region, cc_cbounds, c);
            }
        }
      for (qual_reason_map_scan(qb->ubr, &qrms); qual_reason_map_next(&qrms, &c, &qr);)
        {
          if (qr->ci_bound)
            {
              ((void) ((c->kind == qk_constant) ? 0 : (__assert_fail ("c->kind == qk_constant", "poly-quals.c", 3681, __PRETTY_FUNCTION__), 0)));
              qual_set_insert (quals_region, cc_cbounds, c);
            }
        }
    }
  q->u.var.visited = 1;
  for (qual_edge_hashset_scan(q->u.var.lb, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
    {
      fiv_dfs (e->q1, conn_comp, cc_cbounds);
    }
  for (qual_edge_hashset_scan(q->u.var.ub, &qess), e = qual_edge_hashset_next(&qess); e; e = qual_edge_hashset_next(&qess))
    {
      fiv_dfs (e->q2, conn_comp, cc_cbounds);
    }
}
static int fiv_could_have_type_error (qual_set cbounds)
{
  po_set_scanner pss;
  po_info po;
  qual_set_scanner qss;
  qual c;
  int count;
  for (po_set_scan(all_pos, &pss), po = po_set_next(&pss); po; po = po_set_next(&pss))
    {
      count = 0;
      for (qual_set_scan(cbounds, &qss), c = qual_set_next(&qss); c; c = qual_set_next(&qss))
        {
          ((void) ((c->kind == qk_constant) ? 0 : (__assert_fail ("c->kind == qk_constant", "poly-quals.c", 3716, __PRETTY_FUNCTION__), 0)));
          if (c->u.elt.po == po)
            {
              count++;
              if (count >= 2)
                return 1;
            }
        }
    }
  return 0;
}
static qual_set find_important_vars (qual_set vars)
{
  qual_set result;
  qual_set_scanner qss;
  qual start;
  qual_set conn_comp;
  qual_set cc_cbounds;
  result = empty_qual_set (quals_region);
  reset_visited (vars);
  for (qual_set_scan(vars, &qss), start = qual_set_next(&qss); start; start = qual_set_next(&qss))
    {
      if (start->kind == qk_variable &&
          start->u.var.visited == 0)
        {
          conn_comp = empty_qual_set(quals_region);
          cc_cbounds = empty_qual_set(quals_region);
          fiv_dfs (start, &conn_comp, &cc_cbounds);
          if (fiv_could_have_type_error (cc_cbounds))
            {
              result = qual_set_union_nocheck (result, conn_comp);
            }
        }
    }
  return result;
}
static qual_set expand_error_path_down (region r, qual start, qual lbc)
{
  qual_set result;
  qual_reason qr;
  result = empty_qual_set(r);
  start = ecr_qual (start);
  for (qual_reason_map_lookup (start->u.var.qbounds->lbr, lbc, &qr);
       qr->kind == rk_edge;
       qual_reason_map_lookup (start->u.var.qbounds->lbr, lbc, &qr))
    {
      qual_edge e = qr->u.qe;
      qual_set_insert (r, &result, ecr_qual(e->q1));
      qual_set_insert (r, &result, ecr_qual(e->q2));
      start = qual_edge_other_end (e, start);
      start = ecr_qual (start);
    }
  return result;
}
static qual_set expand_error_path_up (region r, qual start, qual ubc)
{
  qual_set result;
  qual_reason qr;
  result = empty_qual_set(r);
  start = ecr_qual (start);
  for (qual_reason_map_lookup (start->u.var.qbounds->ubr, ubc, &qr);
       qr->kind == rk_edge;
       qual_reason_map_lookup (start->u.var.qbounds->ubr, ubc, &qr))
    {
      qual_edge e = qr->u.qe;
      qual_set_insert (r, &result, ecr_qual(e->q1));
      qual_set_insert (r, &result, ecr_qual(e->q2));
      start = qual_edge_other_end (e, start);
      start = ecr_qual (start);
    }
  return result;
}
static bool is_derivative_error (qual e, qual lbc, qual ubc)
{
  qual_set lpath;
  qual_set upath;
  qual t;
  qual_set_scanner qss;
  region scratch;
  bool retval = 0;
  scratch = newregion();
  lpath = expand_error_path_down (scratch, e, lbc);
  upath = expand_error_path_up (scratch, e, ubc);
  for (qual_set_scan(lpath, &qss), t = qual_set_next(&qss); t; t = qual_set_next(&qss))
    {
      if (!t->u.var.anonymous && ! eq_qual (t, e) && qual_set_member (upath, t))
        {
          retval = 1;
          break;
        }
    }
  deleteregion (scratch);
  return retval;
}
static bool _report_errors (qual_set vars, bool print_anonymous)
{
  qual_set_scanner qss;
  qual q;
  bool found_error = 0;
  bool printed_error = 0;
  region scratch;
  scratch = newregion();
  for (qual_set_scan(vars, &qss), q = qual_set_next(&qss); q; q = qual_set_next(&qss))
    {
      if (! q->link && q->kind == qk_variable && q->u.var.interesting)
        {
          qual_error_list qel;
          qel = new_qual_error_list (scratch);
          if (! _may_leq_qual (q, q, 0, &qel, scratch))
            {
              qual_error_list_scanner qels;
              qual_error qe;
              found_error = 1;
              if (!q->u.var.anonymous || print_anonymous)
                {
                  qual_error_list_scan (qel, &qels);
                  while (qual_error_list_next (&qels, &qe))
                    {
                      if (!is_derivative_error (q, qe->lbc, qe->ubc))
                        {
                          report_qerror(q->u.var.loc, sev_err,
                                        "WARNING: %s treated as %s and %s\n",
                                        name_qual (q),
                                        name_qual (qe->lbc),
                                        name_qual (qe->ubc));
                          printed_error = 1;
                          break;
                        }
                    }
                }
            }
        }
    }
  deleteregion(scratch);
  return found_error && ! printed_error;
}
static void report_errors (qual_set vars)
{
  bool missed_errors;
  missed_errors = _report_errors (vars, 0);
  if (missed_errors)
    missed_errors = _report_errors (vars, 1);
  ((void) ((! missed_errors) ? 0 : (__assert_fail ("! missed_errors", "poly-quals.c", 3997, __PRETTY_FUNCTION__), 0)));
}
void init_quals(void)
{
  ((void) ((!quals_region) ? 0 : (__assert_fail ("!quals_region", "poly-quals.c", 4009, __PRETTY_FUNCTION__), 0)));
  quals_region = newregion();
  resolve_region = newregion();
  next_qual = 0;
  all_quals = empty_qual_set(quals_region);
  all_vars = empty_qual_set(quals_region);
  all_pos = empty_po_set(quals_region);
  current_po = ((void *)0);
  qinstantiations = new_qinstantiation_list(resolve_region);
  can_unify_flow = 1;
  qual_gate_init (&empty_qgate, 1, 1);
  qual_gate_init (&open_qgate, 1, 1);
  qual_gate_init (&fi_qgate, 1, 1);
  qual_gate_init (&fs_qgate, 1, 1);
  qual_gate_init (&effect_qgate, 1, 1);
  qual_gate_init (&ptrflow_down_pos_qgate, 0, 1);
  qual_gate_init (&ptrflow_up_pos_qgate, 0, 1);
  qual_gate_init (&ptrflow_down_neg_qgate, 1, 0);
  qual_gate_init (&ptrflow_up_neg_qgate, 1, 0);
  qual_gate_init (&fieldflow_down_pos_qgate, 0, 1);
  qual_gate_init (&fieldflow_up_pos_qgate, 0, 1);
  qual_gate_init (&fieldflow_down_neg_qgate, 1, 0);
  qual_gate_init (&fieldflow_up_neg_qgate, 1, 0);
  qual_gate_init (&fieldptrflow_down_pos_qgate, 0, 1);
  qual_gate_init (&fieldptrflow_up_pos_qgate, 0, 1);
  qual_gate_init (&fieldptrflow_down_neg_qgate, 1, 0);
  qual_gate_init (&fieldptrflow_up_neg_qgate, 1, 0);
  ((void) ((num_hotspots >= 0) ? 0 : (__assert_fail ("num_hotspots >= 0", "poly-quals.c", 4046, __PRETTY_FUNCTION__), 0)));
  if (num_hotspots != 0)
    {
      int i;
      hotspots = (((void)0), __rc_typed_rarrayalloc)((quals_region), (num_hotspots), sizeof(qual), 0);
      for (i = 0; i < num_hotspots; i++)
        hotspots[i] = ((void *)0);
    }
  else
    hotspots = ((void *)0);
  state = state_init;
}
void end_define_pos(void)
{
  ((void) ((state == state_init) ? 0 : (__assert_fail ("state == state_init", "poly-quals.c", 4064, __PRETTY_FUNCTION__), 0)));
  state = state_pos_defined;
}
void finish_quals(void)
{
  resolve_wellformed();
  check_vars(all_vars);
  check_vars(all_vars);
  check_vars(all_vars);
  if (flag_errors_only)
    important_vars = find_important_vars(all_vars);
  else
    important_vars = all_vars;
  check_vars(all_vars);
  if (flag_print_quals_graph)
    {
      char* filename = ((void *)0);
      char* rootname = ((void *)0);
      int depth;
      FILE* f = ((void *)0);
      qual root = ((void *)0);
      filename = getenv ("CQUAL_GRAPH_FILE");
      if (filename == ((void *)0))
        filename = "quals.dot";
      rootname = getenv ("CQUAL_GRAPH_ROOT");
      if (rootname)
        {
          root = find_var (rootname);
          if (root)
            printf ("found root\n");
          else
            printf ("couldn't find root\n");
        }
      if (rootname)
        {
          if (getenv ("CQUAL_GRAPH_DEPTH"))
            depth = atoi (getenv ("CQUAL_GRAPH_DEPTH"));
          else
            depth = 3;
        }
      else
        depth = 2147483647;
      f = fopen (filename, "w");
      if (f)
        {
          if (root)
            dump_neighborhood (f, all_vars, root, depth);
          else
            dump_quals_graph (f, important_vars);
          fclose (f);
        }
    }
  check_vars(all_vars);
  check_vars(all_vars);
  check_vars(all_vars);
  cfl_flow (important_vars);
  state = state_finish;
  if (flag_errors_only)
    report_errors (all_vars);
}
void dispose_quals(void)
{
  ((void) ((quals_region) ? 0 : (__assert_fail ("quals_region", "poly-quals.c", 4188, __PRETTY_FUNCTION__), 0)));
  deleteregion(quals_region);
  quals_region = 0;
  state = state_start;
}
const char CANON_IDENT_c12b74101b49f14c560a98ccb19cd9a2[] = "CANON_IDENT_/moa/sc1/jkodumal/work/banshee/experiments/cqual/src/poly-quals.c";
