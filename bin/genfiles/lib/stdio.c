 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern int Cyc_Stdio_remove(
struct _tagged_string); extern int Cyc_Stdio_rename( struct _tagged_string,
struct _tagged_string); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_tmpfile();
extern int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen(
struct _tagged_string, struct _tagged_string, struct Cyc_Stdio___sFILE*); extern
int Cyc_Stdio_fgetc( struct Cyc_Stdio___sFILE*); extern struct _tagged_string
Cyc_Stdio_fgets( struct _tagged_string, int n, struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fputs(
struct _tagged_string, struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_getc(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_putc( int, struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_puts( struct _tagged_string); extern int Cyc_Stdio_ungetc(
int, struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE*,
int*); extern int Cyc_Stdio_fseek( struct Cyc_Stdio___sFILE*, int, int); extern
int Cyc_Stdio_fsetpos( struct Cyc_Stdio___sFILE*, int*); extern int Cyc_Stdio_ftell(
struct Cyc_Stdio___sFILE*); extern void Cyc_Stdio_rewind( struct Cyc_Stdio___sFILE*);
extern void Cyc_Stdio_clearerr( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_feof(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_ferror( struct Cyc_Stdio___sFILE*);
extern void Cyc_Stdio_perror( struct _tagged_string); extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_fopen( struct _tagged_string _name, struct _tagged_string _type);
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fdopen( int, struct _tagged_string);
extern int Cyc_Stdio_fileno( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_getw(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_putw( int, struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open(
struct _tagged_string fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); struct Cyc_Cstdio___sFILE; extern int remove(
unsigned char*); extern int rename( unsigned char*, unsigned char*); extern
struct Cyc_Cstdio___sFILE* tmpfile(); extern unsigned char* tmpnam(
unsigned char*); extern int fclose( struct Cyc_Cstdio___sFILE*); extern int
fflush( struct Cyc_Cstdio___sFILE*); extern struct Cyc_Cstdio___sFILE* freopen(
unsigned char*, unsigned char*, struct Cyc_Cstdio___sFILE*); extern void setbuf(
struct Cyc_Cstdio___sFILE*, unsigned char*); extern int setvbuf( struct Cyc_Cstdio___sFILE*,
unsigned char*, int, unsigned int); extern int fgetc( struct Cyc_Cstdio___sFILE*);
extern unsigned char* fgets( unsigned char*, int, struct Cyc_Cstdio___sFILE*);
extern int fputc( int, struct Cyc_Cstdio___sFILE*); extern int fputs(
unsigned char*, struct Cyc_Cstdio___sFILE*); extern int getc( struct Cyc_Cstdio___sFILE*);
extern unsigned char* gets( unsigned char*); extern int putc( int, struct Cyc_Cstdio___sFILE*);
extern int puts( unsigned char*); extern int ungetc( int, struct Cyc_Cstdio___sFILE*);
extern unsigned int fread( unsigned char*, unsigned int _size, unsigned int _n,
struct Cyc_Cstdio___sFILE*); extern unsigned int fwrite( unsigned char*,
unsigned int _size, unsigned int _n, struct Cyc_Cstdio___sFILE*); extern int
fgetpos( struct Cyc_Cstdio___sFILE*, int*); extern int fseek( struct Cyc_Cstdio___sFILE*,
int, int); extern int fsetpos( struct Cyc_Cstdio___sFILE*, int*); extern int
ftell( struct Cyc_Cstdio___sFILE*); extern void rewind( struct Cyc_Cstdio___sFILE*);
extern void clearerr( struct Cyc_Cstdio___sFILE*); extern int feof( struct Cyc_Cstdio___sFILE*);
extern int ferror( struct Cyc_Cstdio___sFILE*); extern void perror(
unsigned char*); extern struct Cyc_Cstdio___sFILE* fopen( unsigned char* _name,
unsigned char* _type); extern struct Cyc_Cstdio___sFILE* fdopen( int,
unsigned char*); extern int fileno( struct Cyc_Cstdio___sFILE*); extern int getw(
struct Cyc_Cstdio___sFILE*); extern int putw( int, struct Cyc_Cstdio___sFILE*);
extern void setbuffer( struct Cyc_Cstdio___sFILE*, unsigned char*, int); extern
int setlinebuf( struct Cyc_Cstdio___sFILE*); struct Cyc_Stdio___sFILE{ struct
Cyc_Cstdio___sFILE* file; } ; int Cyc_Stdio_remove( struct _tagged_string
filename){ return remove( string_to_Cstring( filename));} int Cyc_Stdio_rename(
struct _tagged_string old_filename, struct _tagged_string new_filename){ return
rename( string_to_Cstring( old_filename), string_to_Cstring( new_filename));}
struct Cyc_Stdio___sFILE* Cyc_Stdio_tmpfile(){ struct Cyc_Cstdio___sFILE* cf=
tmpfile(); return( unsigned int) cf?({ struct Cyc_Stdio___sFILE* _temp0=( struct
Cyc_Stdio___sFILE*) GC_malloc( sizeof( struct Cyc_Stdio___sFILE)); _temp0->file=
cf; _temp0;}): 0;} int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE* f){ int r=
fclose(( struct Cyc_Cstdio___sFILE*) _check_null( f->file)); if( r == 0){ f->file=
0;} return r;} int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE* f){ return(
unsigned int) f? fflush((( struct Cyc_Stdio___sFILE*) _check_null( f))->file):
fflush( 0);} struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen( struct _tagged_string
x, struct _tagged_string y, struct Cyc_Stdio___sFILE* f){ struct Cyc_Cstdio___sFILE*
cf= freopen( string_to_Cstring( x), string_to_Cstring( y),( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); return( unsigned int) cf?({ struct Cyc_Stdio___sFILE*
_temp1=( struct Cyc_Stdio___sFILE*) GC_malloc( sizeof( struct Cyc_Stdio___sFILE));
_temp1->file= cf; _temp1;}): 0;} int Cyc_Stdio_fgetc( struct Cyc_Stdio___sFILE*
f){ return fgetc(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} struct
_tagged_string Cyc_Stdio_fgets( struct _tagged_string s, int n, struct Cyc_Stdio___sFILE*
f){ unsigned char* result; unsigned char* buffer= underlying_Cstring(( struct
_tagged_string) s); unsigned int len=({ struct _tagged_string _temp3= s;(
unsigned int)( _temp3.last_plus_one - _temp3.curr);}); n=( int)( len < n? len:(
unsigned int) n); result= fgets( buffer, n,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); if( result == 0){ return({ struct _tagged_string _temp2={
0, 0, 0}; _temp2;});} else{ return s;}} int Cyc_Stdio_fputc( int i, struct Cyc_Stdio___sFILE*
f){ return fputc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int
Cyc_Stdio_fputs( struct _tagged_string s, struct Cyc_Stdio___sFILE* f){ return
fputs( string_to_Cstring( s),( struct Cyc_Cstdio___sFILE*) _check_null( f->file));}
int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE* f){ return getc(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_putc( int i, struct Cyc_Stdio___sFILE* f){
return putc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_puts(
struct _tagged_string s){ return puts( string_to_Cstring( s));} int Cyc_Stdio_ungetc(
int i, struct Cyc_Stdio___sFILE* f){ return ungetc( i,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE* f, int*
x){ return fgetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file), x);} int
Cyc_Stdio_fseek( struct Cyc_Stdio___sFILE* f, int offset, int whence){ return
fseek(( struct Cyc_Cstdio___sFILE*) _check_null( f->file), offset, whence);} int
Cyc_Stdio_fsetpos( struct Cyc_Stdio___sFILE* f, int* x){ return fsetpos(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file), x);} int Cyc_Stdio_ftell( struct Cyc_Stdio___sFILE*
f){ return ftell(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void Cyc_Stdio_rewind(
struct Cyc_Stdio___sFILE* f){ rewind(( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} void Cyc_Stdio_clearerr( struct Cyc_Stdio___sFILE* f){ clearerr((
struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_feof( struct
Cyc_Stdio___sFILE* f){ return feof(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));}
int Cyc_Stdio_ferror( struct Cyc_Stdio___sFILE* f){ return ferror(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} void Cyc_Stdio_perror( struct _tagged_string s){ perror(
string_to_Cstring( s));} struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen( struct
_tagged_string name, struct _tagged_string type){ struct Cyc_Cstdio___sFILE* cf=
fopen( string_to_Cstring( name), string_to_Cstring( type)); return( unsigned int)
cf?({ struct Cyc_Stdio___sFILE* _temp4=( struct Cyc_Stdio___sFILE*) GC_malloc(
sizeof( struct Cyc_Stdio___sFILE)); _temp4->file= cf; _temp4;}): 0;} struct Cyc_Stdio___sFILE*
Cyc_Stdio_fdopen( int i, struct _tagged_string s){ struct Cyc_Cstdio___sFILE* cf=
fdopen( i, string_to_Cstring( s)); return( unsigned int) cf?({ struct Cyc_Stdio___sFILE*
_temp5=( struct Cyc_Stdio___sFILE*) GC_malloc( sizeof( struct Cyc_Stdio___sFILE));
_temp5->file= cf; _temp5;}): 0;} int Cyc_Stdio_fileno( struct Cyc_Stdio___sFILE*
f){ return fileno(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_getw(
struct Cyc_Stdio___sFILE* f){ return getw(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_putw( int i, struct Cyc_Stdio___sFILE* f){
return putw( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));}
unsigned char Cyc_Stdio_FileCloseError[ 19u]="\000\000\000\000FileCloseError";
unsigned char Cyc_Stdio_FileOpenError[ 18u]="\000\000\000\000FileOpenError";
struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct _tagged_string fname,
struct _tagged_string mode){ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_fopen( fname,
mode); if( f == 0){( void) _throw(( void*)({ struct Cyc_Stdio_FileOpenError_struct*
_temp6=( struct Cyc_Stdio_FileOpenError_struct*) GC_malloc( sizeof( struct Cyc_Stdio_FileOpenError_struct));
_temp6[ 0]=({ struct Cyc_Stdio_FileOpenError_struct _temp7; _temp7.tag= Cyc_Stdio_FileOpenError;
_temp7.f1= fname; _temp7;}); _temp6;}));} return( struct Cyc_Stdio___sFILE*)
_check_null( f);} void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE* f){ if(
Cyc_Stdio_fclose( f) != 0){( void) _throw(( void*) Cyc_Stdio_FileCloseError);}}