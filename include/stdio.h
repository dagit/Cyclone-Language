//
// Copyright (c) 1990 The Regents of the University of California.
// All rights reserved.
//
// Redistribution and use in source and binary forms are permitted
// provided that the above copyright notice and this paragraph are
// duplicated in all such forms and that any documentation,
// advertising materials, and other materials related to such
// distribution and use acknowledge that the software was developed
// by the University of California, Berkeley.  The name of the
// University may not be used to endorse or promote products derived
// from this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
//	@(#)stdio.h	5.3 (Berkeley) 3/15/86
//
#ifndef _STDIO_H_
#define _STDIO_H_
#include "core.h"
namespace Stdio {
using Core;
extern struct __sFILE;  
typedef struct __sFILE FILE;
extern FILE @stdout;
extern FILE @stdin;
extern FILE @stderr;
typedef long fpos_t;
#define	__SLBF	0x0001		// line buffered 
#define	__SNBF	0x0002		// unbuffered 
#define	__SRD	0x0004		// OK to read 
#define	__SWR	0x0008		// OK to write 
	// RD and WR are never simultaneously asserted 
#define	__SRW	0x0010		// open for reading & writing 
#define	__SEOF	0x0020		// found EOF 
#define	__SERR	0x0040		// found error 
#define	__SMBF	0x0080		// _buf is from malloc 
#define	__SAPP	0x0100		// fdopen()ed in append mode - so must  write to end 
#define	__SSTR	0x0200		// this is an sprintf/snprintf string 
#define	__SOPT	0x0400		// do fseek() optimisation 
#define	__SNPT	0x0800		// do not do fseek() optimisation 
#define	__SOFF	0x1000		// set iff _offset is in fact correct 
#define	__SMOD	0x2000		// true => fgetline modified _p text 
#define __SCLE        0x4000          // convert line endings CR/LF <-> NL 

#define	_IOFBF	0		// setvbuf should set fully buffered 
#define	_IOLBF	1		// setvbuf should set line buffered 
#define	_IONBF	2		// setvbuf should set unbuffered 

#ifndef NULL
#define	NULL	null
#endif

#define	BUFSIZ	1024
#define	EOF	((char)(-1))

#define	FOPEN_MAX	20	// must be <= OPEN_MAX <sys/syslimits.h> 
#define	FILENAME_MAX	1024	// must be <= PATH_MAX <sys/syslimits.h> 
#define	L_tmpnam	1024	// XXX must be == PATH_MAX 

#ifndef SEEK_SET
#define	SEEK_SET	0	// set file offset to offset 
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	// set file offset to current plus offset 
#endif
#ifndef SEEK_END
#define	SEEK_END	2	// set file offset to EOF plus offset 
#endif

#define	TMP_MAX		26

//
// Functions defined in ANSI C standard.
//
extern int	remove(string_t);
extern int	rename(string_t, string_t);
extern FILE *	tmpfile();
// extern string	tmpnam(string);
extern int	fclose(FILE @);
extern int	fflush(FILE *);
extern FILE *	freopen(string_t, string_t, FILE @);
//extern void	setbuf(FILE *, string);
//extern int	setvbuf(FILE *, string, int, size_t);
extern int	fgetc(FILE @);
extern mstring_t fgets(mstring_t, int n, FILE @);
extern int	fputc(int, FILE @);
extern int	fputs(string_t, FILE @);
extern int	getc(FILE @);
//extern int	getchar();
// extern string     gets(string); // unsafe!
extern int	putc(int, FILE @);
  //extern int	putchar(int);
extern int	puts(string_t);
extern int	ungetc(int, FILE @);
  //extern size_t	fread(string,unsigned int _offset,size_t _size,size_t _n,FILE *);
  //extern size_t	fwrite(string,unsigned int _offset,size_t _size,size_t _n,FILE *);
extern int	fgetpos(FILE @, fpos_t @);
extern int	fseek(FILE @, long, int);
extern int	fsetpos(FILE @, fpos_t @);
extern long	ftell(FILE @);
extern void	rewind(FILE @);
extern void	clearerr(FILE @);
extern int	feof(FILE @);
extern int	ferror(FILE @);
extern void     perror(string_t);
extern FILE *	fopen(string_t _name , string_t _type);

//
// Routines in POSIX 1003.1.
//

extern FILE *	fdopen(int, string_t);
extern int	fileno(FILE @);
extern int	getw(FILE @);
  //extern int	pclose(FILE @);
  //extern FILE *   popen(string, string);
extern int	putw(int, FILE @);
extern void     setbuffer(FILE @, mstring_t, int);
extern int	setlinebuf(FILE @);

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x, stdout)

#define	L_cuserid	9		// posix says it goes in stdio.h :( 
#define L_ctermid       16

// 
// Routines added for Cyclone
//
extern xtunion exn {
  extern FileOpenError(string_t);
  extern FileCloseError;
};

extern FILE @file_open(string_t fname, string_t mode);
extern void file_close(FILE @);
extern void file_delete(string_t);
extern void file_length(string_t);
// these two provided in cyc_runtime.c
extern int file_string_read(FILE @fd, mstring_t dest, int dest_offset, 
			    int max_count);
extern int file_string_write(FILE @fd, string_t src, int src_offset, 
			     int max_count);
}
#endif
