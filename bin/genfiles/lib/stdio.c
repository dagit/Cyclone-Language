#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_remove( struct _tagged_string);
extern int Cyc_Stdio_rename( struct _tagged_string, struct _tagged_string);
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_tmpfile(); extern int Cyc_Stdio_fclose(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen( struct _tagged_string,
struct _tagged_string, struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fgetc(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fgets( struct _tagged_string,
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_fputs( struct _tagged_string, struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_putc(
int, struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_puts( struct
_tagged_string); extern int Cyc_Stdio_ungetc( int, struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE*, int*); extern int Cyc_Stdio_fseek(
struct Cyc_Stdio___sFILE*, int, int); extern int Cyc_Stdio_fsetpos( struct Cyc_Stdio___sFILE*,
int*); extern int Cyc_Stdio_ftell( struct Cyc_Stdio___sFILE*); extern void Cyc_Stdio_rewind(
struct Cyc_Stdio___sFILE*); extern void Cyc_Stdio_clearerr( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_feof( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_ferror(
struct Cyc_Stdio___sFILE*); extern void Cyc_Stdio_perror( struct _tagged_string);
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen( struct _tagged_string _name,
struct _tagged_string _type); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fdopen(
int, struct _tagged_string); extern int Cyc_Stdio_fileno( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_getw( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_pclose(
struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_popen(
struct _tagged_string, struct _tagged_string); extern int Cyc_Stdio_putw( int,
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_string fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); extern int remove( char*); extern int rename( char*,
char*); extern struct Cyc_Stdio___sFILE* tmpfile(); extern char* tmpnam( char*);
extern int fclose( struct Cyc_Stdio___sFILE*); extern int fflush( struct Cyc_Stdio___sFILE*);
extern struct Cyc_Stdio___sFILE* freopen( char*, char*, struct Cyc_Stdio___sFILE*);
extern void setbuf( struct Cyc_Stdio___sFILE*, char*); extern int setvbuf(
struct Cyc_Stdio___sFILE*, char*, int, unsigned int); extern int fgetc( struct
Cyc_Stdio___sFILE*); extern char* fgets( char*, int, struct Cyc_Stdio___sFILE*);
extern int fputc( int, struct Cyc_Stdio___sFILE*); extern int fputs( char*,
struct Cyc_Stdio___sFILE*); extern int getc( struct Cyc_Stdio___sFILE*); extern
char* gets( char*); extern int putc( int, struct Cyc_Stdio___sFILE*); extern int
puts( char*); extern int ungetc( int, struct Cyc_Stdio___sFILE*); extern
unsigned int fread( char*, unsigned int _size, unsigned int _n, struct Cyc_Stdio___sFILE*);
extern unsigned int fwrite( char*, unsigned int _size, unsigned int _n, struct
Cyc_Stdio___sFILE*); extern int fgetpos( struct Cyc_Stdio___sFILE*, int*);
extern int fseek( struct Cyc_Stdio___sFILE*, int, int); extern int fsetpos(
struct Cyc_Stdio___sFILE*, int*); extern int ftell( struct Cyc_Stdio___sFILE*);
extern void rewind( struct Cyc_Stdio___sFILE*); extern void clearerr( struct Cyc_Stdio___sFILE*);
extern int feof( struct Cyc_Stdio___sFILE*); extern int ferror( struct Cyc_Stdio___sFILE*);
extern void perror( char*); extern struct Cyc_Stdio___sFILE* fopen( char* _name,
char* _type); extern struct Cyc_Stdio___sFILE* fdopen( int, char*); extern int
fileno( struct Cyc_Stdio___sFILE*); extern int getw( struct Cyc_Stdio___sFILE*);
extern int pclose( struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE*
popen( char*, char*); extern int putw( int, struct Cyc_Stdio___sFILE*); extern
void setbuffer( struct Cyc_Stdio___sFILE*, char*, int); extern int setlinebuf(
struct Cyc_Stdio___sFILE*); int Cyc_Stdio_remove( struct _tagged_string filename){
return remove( string_to_Cstring( filename));} int Cyc_Stdio_rename( struct
_tagged_string old_filename, struct _tagged_string new_filename){ return rename(
string_to_Cstring( old_filename), string_to_Cstring( new_filename));} struct Cyc_Stdio___sFILE*
Cyc_Stdio_tmpfile(){ return tmpfile();} int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*
f){ return fclose( f);} int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE* f){
return fflush( f);} struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen( struct
_tagged_string x, struct _tagged_string y, struct Cyc_Stdio___sFILE* f){ return
freopen( string_to_Cstring( x), string_to_Cstring( y), f);} int Cyc_Stdio_fgetc(
struct Cyc_Stdio___sFILE* f){ return fgetc( f);} int Cyc_Stdio_fgets( struct
_tagged_string s, struct Cyc_Stdio___sFILE* f){ char* buffer= underlying_Cstring(
s); unsigned int len=({ struct _tagged_string _temp0= s;( unsigned int)( _temp0.last_plus_one
- _temp0.curr);}); return fgets( buffer,( int) len, f) == 0;} int Cyc_Stdio_fputc(
int i, struct Cyc_Stdio___sFILE* f){ return fputc( i, f);} int Cyc_Stdio_fputs(
struct _tagged_string s, struct Cyc_Stdio___sFILE* f){ return fputs(
string_to_Cstring( s), f);} int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE* f){
return getc( f);} int Cyc_Stdio_putc( int i, struct Cyc_Stdio___sFILE* f){
return putc( i, f);} int Cyc_Stdio_puts( struct _tagged_string s){ return puts(
string_to_Cstring( s));} int Cyc_Stdio_ungetc( int i, struct Cyc_Stdio___sFILE*
f){ return ungetc( i, f);} int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE* f,
int* x){ return fgetpos( f, x);} int Cyc_Stdio_fseek( struct Cyc_Stdio___sFILE*
f, int offset, int whence){ return fseek( f, offset, whence);} int Cyc_Stdio_fsetpos(
struct Cyc_Stdio___sFILE* f, int* x){ return fsetpos( f, x);} int Cyc_Stdio_ftell(
struct Cyc_Stdio___sFILE* f){ return ftell( f);} void Cyc_Stdio_rewind( struct
Cyc_Stdio___sFILE* f){ rewind( f);} void Cyc_Stdio_clearerr( struct Cyc_Stdio___sFILE*
f){ clearerr( f);} int Cyc_Stdio_feof( struct Cyc_Stdio___sFILE* f){ return feof(
f);} int Cyc_Stdio_ferror( struct Cyc_Stdio___sFILE* f){ return ferror( f);}
void Cyc_Stdio_perror( struct _tagged_string s){ perror( string_to_Cstring( s));}
struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen( struct _tagged_string name, struct
_tagged_string type){ return fopen( string_to_Cstring( name), string_to_Cstring(
type));} struct Cyc_Stdio___sFILE* Cyc_Stdio_fdopen( int i, struct
_tagged_string s){ return fdopen( i, string_to_Cstring( s));} int Cyc_Stdio_fileno(
struct Cyc_Stdio___sFILE* f){ return fileno( f);} int Cyc_Stdio_getw( struct Cyc_Stdio___sFILE*
f){ return getw( f);} int Cyc_Stdio_pclose( struct Cyc_Stdio___sFILE* f){ return
pclose( f);} struct Cyc_Stdio___sFILE* Cyc_Stdio_popen( struct _tagged_string s,
struct _tagged_string m){ return popen( string_to_Cstring( s), string_to_Cstring(
m));} int Cyc_Stdio_putw( int i, struct Cyc_Stdio___sFILE* f){ return putw( i, f);}
char Cyc_Stdio_FileOpenError_tag[ 14u]="FileOpenError"; char Cyc_Stdio_FileCloseError_tag[
15u]="FileCloseError"; struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_string fname, struct _tagged_string mode){ struct Cyc_Stdio___sFILE* f=
Cyc_Stdio_fopen( fname, mode); if( f == 0){( void) _throw(({ struct Cyc_Stdio_FileOpenError_struct*
_temp1=( struct Cyc_Stdio_FileOpenError_struct*) GC_malloc( sizeof( struct Cyc_Stdio_FileOpenError_struct));
_temp1->tag= Cyc_Stdio_FileOpenError_tag; _temp1->f1= fname;( struct
_xenum_struct*) _temp1;}));} return( struct Cyc_Stdio___sFILE*)({ struct Cyc_Stdio___sFILE*
_temp2= f; if( _temp2 == 0){ _throw( Null_Exception);} _temp2;});} void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE* f){ if( Cyc_Stdio_fclose(( struct Cyc_Stdio___sFILE*)
f) != 0){( void) _throw(({ struct Cyc_Stdio_FileCloseError_struct* _temp3=(
struct Cyc_Stdio_FileCloseError_struct*) GC_malloc_atomic( sizeof( struct Cyc_Stdio_FileCloseError_struct));
_temp3->tag= Cyc_Stdio_FileCloseError_tag;( struct _xenum_struct*) _temp3;}));}}