/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef _CYCBOOT_H_
#define _CYCBOOT_H_

/***** <sys/type.h> *****/
#ifndef _size_t_def_
#define _size_t_def_
// Careful with this one, it might be arch specific
typedef unsigned long size_t; // correct on powerpc at least
#endif

/***** <fcntl.h> *****/
// Best get rid of this one.  It's used by buildlib which uses open; also mkdir
#ifndef _mode_t_def_
#define _mode_t_def_
typedef unsigned short mode_t; // correct on powerpc
#endif
#ifndef O_RDONLY
#define O_RDONLY 0x0000 
#endif
  extern int open(const char *, int, ... mode_t);

/***** <math.h> *****/
  extern "C" double modf(double, double @);

/***** <stdio.h> *****/
#define EOF (-1) 
  extern struct __cycFILE;  
  // Be sure to use _def_ so we don't interfere with libc.cys
#ifndef _FILE_def_
#define _FILE_def_
  typedef struct __cycFILE FILE;
#endif
  extern FILE @stdout;
  extern FILE @stdin;
  extern FILE @stderr;

  // vararg for printf functions.  The functions themselves are defined
  // in printf.cyc.
  extern datatype PrintArg<`r::R> {
    String_pa(const char ? @nozeroterm`r);
    Int_pa(unsigned long);
    Double_pa(double);
    LongDouble_pa(long double);
    ShortPtr_pa(short @`r);
    IntPtr_pa(unsigned long @`r);
  };
  // Be sure to use _def_ so we don't interfere with cycboot.h
#ifndef _parg_t_def_
#define _parg_t_def_
  typedef datatype PrintArg<`r> @`r parg_t<`r>;
#endif

  // Cyclone specific.
  // Similar to sprintf but allocates a result of the right size on the heap.
  extern char ?aprintf(const char ?, ... inject parg_t)
    __attribute__((format(printf,1,2)))
    ;

    //  extern void clearerr(FILE @);

  extern int fclose(FILE @);

  // FIX: second arg allowed to be NULL?
    //  extern FILE *fdopen(int, const char *);

  extern int feof(FILE @);

    //  extern int ferror(FILE @);

  extern int fflush(FILE *);

  extern int fgetc(FILE @);

    //  extern int fgetpos(FILE @, fpos_t @);

    //  extern char ? @nozeroterm`r fgets(char ? @nozeroterm`r, int, FILE @);

    //  extern int fileno(FILE @);

  extern FILE *fopen(const char @, const char @);

  extern int fprintf(FILE @,const char ?, ... inject parg_t)
    __attribute__((format(printf,2,3)))
    ;

  extern int fputc(int, FILE @);

  extern int fputs(const char @, FILE @);

  extern size_t fread(char ? @nozeroterm, size_t, size_t, FILE @);

    //  extern FILE *`r freopen(const char *, const char @, FILE @`r);

  // vararg for scanf functions.  The functions themselves are defined
  // in scanf.cyc.
  extern datatype ScanfArg<`r::R> {
    ShortPtr_sa(short @`r);
    UShortPtr_sa(unsigned short @`r);
    IntPtr_sa(int @`r);
    UIntPtr_sa(unsigned int @`r);
    StringPtr_sa(char ?`r);
    DoublePtr_sa(double @`r);
    FloatPtr_sa(float @`r);
    CharPtr_sa(char ? @nozeroterm`r)
  };
  // Be sure to use _def_ so we don't interfere with cycboot.h
#ifndef _sarg_t_def_
#define _sarg_t_def_
  typedef datatype ScanfArg<`r1> @`r2 sarg_t<`r1,`r2>;
#endif
//
//  extern int fscanf(FILE @, const char ?, ... inject sarg_t)
//    __attribute__((format(scanf,2,3)))
//    ;

//  extern int fseek(FILE @, long, int);

//  extern int fsetpos(FILE @, const fpos_t @);

//  extern long ftell(FILE @);

extern size_t fwrite(const char ? @nozeroterm, size_t, size_t, FILE @);

extern int getc(FILE @);

//  extern int getchar(void);

  // FIX: we don't support gets because the C version is completely
  // unsafe.  We could support it safely by writing it in Cyclone.
  //  extern char *gets(char *);

//  extern int pclose(FILE @);

  // FIX: worry about char * being NULL?
//  extern "C" void perror(const char *);

//  extern FILE *popen(const char @, const char @);

  extern int printf(const char ?, ... inject parg_t)
    __attribute__((format(printf,1,2)))
    ;

  extern int putc(int, FILE @);

    //  extern "C" int putchar(int);

  extern "C" int puts(const char @);

  extern "C" int remove(const char @);

    //  extern "C" int rename(const char @, const char @);

    //  extern void rewind(FILE @);

  // Cyclone specific.
  extern char ?`r rprintf(region_t<`r>, const char ?, ... inject parg_t)
    __attribute__((format(printf,2,3)))
    ;

    //  extern int scanf(const char ?, ... inject sarg_t)
    //    __attribute__((format(scanf,1,2)))
    //    ;

  // FIX: should the second arg be const?
    //  extern int setvbuf(FILE @, char ? @nozeroterm`H, int, size_t);
  // The second arg of setvbuf must be heap-allocated because
  // it will be used by the IO library after setvbuf returns.

    //  extern void setbuf(FILE @, char ? @nozeroterm`H);

    //  extern int snprintf(char ? @nozeroterm, size_t, const char ?, ... inject parg_t)
    //    __attribute__((format(printf,3,4)))
    //    ;

    //  extern int sprintf(char ? @nozeroterm, const char ?, ... inject parg_t)
    //    __attribute__((format(printf,2,3)))
    //    ;

  extern int sscanf(const char ?, const char ?, ... inject sarg_t)
    __attribute__((format(scanf,2,3)))
    ;

  // FIX: tempnam is not yet supported.  The C version does a heap
  // allocation (with malloc).  We need to write a C stub to free
  // this.
  // Note, tempnam is considered insecure because in between the call
  // of tempnam and open, a file can be created with the supposedly
  // fresh name.
  //  extern char *tempnam(const char *, const char *);

    //  extern FILE *tmpfile(void);

  // Note, tmpnam is considered insecure because in between the call
  // of tmpnam and open, a file can be created with the supposedly
  // fresh name.
  // Note, we require the argument of tmpnam to be heap-allocated.
  // This is because if tmpnam is called with NULL, it is supposed
  // to return a pointer to a static (heap allocated) buffer, while if
  // tmpnam is called with a non-NULL argument, it returns that
  // argument.  So the only way to type tmpnam is to require the arg
  // to be heap allocated.
//  extern char ? tmpnam(char ?`H);
//
  extern int ungetc(int, FILE @);

  extern int vfprintf(FILE @, const char ?, parg_t ?)
    __attribute__((format(printf,2,0)))
    ;

  extern int vprintf(const char ?, parg_t ?)
    __attribute__((format(printf,1,0)))
    ;

  extern char ?`r vrprintf(region_t<`r>, const char ?, parg_t ?)
     __attribute__((format(printf,2,0)))
     ;

  // WE NEED TO ADD THIS TO SCANF.CYC
  //  extern int vscanf(const char *restrict, va_list);

  extern int vsnprintf(char ? @nozeroterm, size_t, const char ?, parg_t ?) 
    __attribute__((format(printf,3,0)))
    ;

  extern int vsprintf(char ? @nozeroterm, const char ?, parg_t ?)
    __attribute__((format(printf,2,0)))
    ;

  extern int vsscanf(const char ?, const char ?, sarg_t ?)
    __attribute__((format(scanf,2,0)))
    ;

  // getw is a glibc function but does not appear in posix; remove ??
  extern int getw(FILE @);
//
  // putw is a glibc function but does not appear in posix; remove ??
  extern int putw(int, FILE @);
//
//  // These functions appear in glibc but are not part of the posix
//  // standard.  They are omitted for now.
//  //  extern void setbuffer (FILE @, char ?, size_t);
//  //  extern void setlinebuf (FILE @);
//
//  // FIX: These functions are are not part of POSIX, they
//  // are Cyclone-specific.  We should get rid of them or move
//  // them elsewhere.
//
// FIX: Not sure if these should use NULL-terminated strings or not
extern datatype exn {
  extern FileOpenError(const char ?);
  extern FileCloseError;
};
extern FILE @file_open(const char ?, const char ?);
extern void file_close(FILE @);
//
//
// these two provided in c stubs
extern int file_string_read(FILE @, char ?dest, int dest_offset, 
                            int max_count);
extern int file_string_write(FILE @, const char ?src, int src_offset, 
                             int max_count);

/***** <ctype.h> *****/
  extern "C" int isalnum(int);

  extern "C" int isalpha(int);

  extern "C" int isascii(int);

  extern "C" int iscntrl(int);

  extern "C" int isdigit(int);

  extern "C" int isgraph(int);

  extern "C" int islower(int);

  extern "C" int isprint(int);

  extern "C" int ispunct(int);

  extern "C" int isspace(int);

  extern "C" int isupper(int);

  extern "C" int isxdigit(int);

  extern "C" int toascii(int);

  extern "C" int tolower(int);

  extern "C" int toupper(int);

/***** <stdlib.h> *****/
  extern "C" char * getenv(const char @);
  extern "C" double atof(const char @);
  extern "C" long strtol(char @`r, char @`r *, int);
  // FIX: should be const?
  extern "C" unsigned long strtoul(char @`r, char @`r *, int);
  extern "C" int system(const char @);
  extern "C" void exit(int) __attribute__((noreturn)) ;
  extern "C" `a abort() __attribute__((noreturn));

/***** <sys/stat.h> *****/
  extern "C" int mkdir(const char @ pathname, mode_t mode);

/***** <unistd.h> *****/
  extern "C" int close(int);
  extern "C" int chdir(const char @);
  extern char ?`r getcwd(char ?`r buf, size_t size);

/***** <limits.h> -- needed for ap.cyc *****/
  extern const long long_max, long_min;

#endif
