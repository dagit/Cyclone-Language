// This is a Cyclone include file, but the actual definitions are
// made in C code.  So, this file is used to automatically produce
// a C include file, for use with the C definitions.

#ifndef PRECORE_H
#define PRECORE_H

// Core.cyc uses some additional C routines, but they are not
// declared here because they are internal to Core, whereas the
// routines below can be used by any Cyclone program.

// These two type definitions are actually made in stdio.h and
// should be removed from precore_c.h
struct __sFILE; // This is what gcc uses
typedef struct __sFILE FILE;

extern FILE @cyc_stdout;
extern FILE @cyc_stdin;
extern FILE @cyc_stderr;

typedef unsigned int uint;
//typedef unsigned int size_t;
typedef char Cstring[];
// a boxed and tagged string: struct {uint sz; Cstring *contents;}@
typedef char string[?];
extern Cstring string_to_Cstring(string);
extern int system(Cstring);

extern int f_string_read  (FILE *,string,int,int);
extern int f_string_write (FILE *,string,int,int);
extern int f_seek         (FILE *,int);
extern int fflush         (FILE *);
extern int fgetc          (FILE *);

#endif
