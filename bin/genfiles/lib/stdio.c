 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern int Cyc_Stdio_remove(
struct _tagged_arr); extern int Cyc_Stdio_rename( struct _tagged_arr, struct
_tagged_arr); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_tmpfile(); extern
struct _tagged_arr Cyc_Stdio_tmpnam( struct _tagged_arr); extern int Cyc_Stdio_fclose(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen( struct _tagged_arr __filename,
struct _tagged_arr __modes); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen(
struct _tagged_arr, struct _tagged_arr, struct Cyc_Stdio___sFILE*); extern void
Cyc_Stdio_setbuf( struct Cyc_Stdio___sFILE* __stream, struct _tagged_arr __buf);
extern int Cyc_Stdio_setvbuf( struct Cyc_Stdio___sFILE* __stream, struct
_tagged_arr __buf, int __modes, int __n); extern int Cyc_Stdio_fgetc( struct Cyc_Stdio___sFILE*
__stream); extern int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE* __stream);
extern struct _tagged_arr Cyc_Stdio_fgets( struct _tagged_arr __s, int __n,
struct Cyc_Stdio___sFILE* __stream); extern int Cyc_Stdio_fputc( int __c, struct
Cyc_Stdio___sFILE* __stream); extern int Cyc_Stdio_putc( int __c, struct Cyc_Stdio___sFILE*
__stream); extern int Cyc_Stdio_fputs( struct _tagged_arr __s, struct Cyc_Stdio___sFILE*
__stream); extern int Cyc_Stdio_puts( struct _tagged_arr __s); extern int Cyc_Stdio_ungetc(
int __c, struct Cyc_Stdio___sFILE* __stream); extern int Cyc_Stdio_fread( struct
_tagged_arr __ptr, int __size, int __n, struct Cyc_Stdio___sFILE* __stream);
extern int Cyc_Stdio_fwrite( struct _tagged_arr __ptr, int __size, int __n,
struct Cyc_Stdio___sFILE* __s); extern int Cyc_Stdio_fseek( struct Cyc_Stdio___sFILE*
__stream, int __off, int __whence); extern int Cyc_Stdio_ftell( struct Cyc_Stdio___sFILE*
__stream); extern void Cyc_Stdio_rewind( struct Cyc_Stdio___sFILE* __stream);
extern int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE* __stream, int* __pos);
extern int Cyc_Stdio_fsetpos( struct Cyc_Stdio___sFILE* __stream, int* __pos);
extern void Cyc_Stdio_clearerr( struct Cyc_Stdio___sFILE* __stream); extern int
Cyc_Stdio_feof( struct Cyc_Stdio___sFILE* __stream); extern int Cyc_Stdio_ferror(
struct Cyc_Stdio___sFILE* __stream); extern void Cyc_Stdio_perror( struct
_tagged_arr __s); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fdopen( int __fd,
struct _tagged_arr __modes); extern int Cyc_Stdio_fileno( struct Cyc_Stdio___sFILE*
__stream); extern int Cyc_Stdio_getw( struct Cyc_Stdio___sFILE* __stream);
extern int Cyc_Stdio_putw( int __w, struct Cyc_Stdio___sFILE* __stream); extern
void Cyc_Stdio_setbuffer( struct Cyc_Stdio___sFILE* __stream, struct _tagged_arr
__buf, int __size); extern void Cyc_Stdio_setlinebuf( struct Cyc_Stdio___sFILE*
__stream); extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_file_open( struct _tagged_arr fname, struct _tagged_arr mode); extern
void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*); static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern struct _tagged_arr Cyc_String_strcpy(
struct _tagged_arr dest, struct _tagged_arr src); struct Cyc_Cstdio___sFILE;
extern int remove( unsigned char*); extern int rename( unsigned char*,
unsigned char*); extern struct Cyc_Cstdio___sFILE* tmpfile(); extern
unsigned char* tmpnam( unsigned char*); extern int fclose( struct Cyc_Cstdio___sFILE*);
extern int fflush( struct Cyc_Cstdio___sFILE*); extern struct Cyc_Cstdio___sFILE*
freopen( unsigned char*, unsigned char*, struct Cyc_Cstdio___sFILE*); extern int
setvbuf( struct Cyc_Cstdio___sFILE*, unsigned char*, int, int); extern int fgetc(
struct Cyc_Cstdio___sFILE*); extern unsigned char* fgets( unsigned char*, int,
struct Cyc_Cstdio___sFILE*); extern int fputc( int, struct Cyc_Cstdio___sFILE*);
extern int fputs( unsigned char*, struct Cyc_Cstdio___sFILE*); extern int getc(
struct Cyc_Cstdio___sFILE*); extern unsigned char* gets( unsigned char*); extern
int putc( int, struct Cyc_Cstdio___sFILE*); extern int puts( unsigned char*);
extern int ungetc( int, struct Cyc_Cstdio___sFILE*); extern int fread(
unsigned char*, int _size, int _n, struct Cyc_Cstdio___sFILE*); extern int
fwrite( unsigned char*, int _size, int _n, struct Cyc_Cstdio___sFILE*); extern
int fgetpos( struct Cyc_Cstdio___sFILE*, int*); extern int fseek( struct Cyc_Cstdio___sFILE*,
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
Cyc_Cstdio___sFILE* file; } ; int Cyc_Stdio_remove( struct _tagged_arr filename){
return remove( string_to_Cstring( filename));} int Cyc_Stdio_rename( struct
_tagged_arr old_filename, struct _tagged_arr new_filename){ return rename(
string_to_Cstring( old_filename), string_to_Cstring( new_filename));} struct Cyc_Stdio___sFILE*
Cyc_Stdio_tmpfile(){ struct Cyc_Cstdio___sFILE* cf= tmpfile(); return(
unsigned int) cf?({ struct Cyc_Stdio___sFILE* _temp0=( struct Cyc_Stdio___sFILE*)
GC_malloc( sizeof( struct Cyc_Stdio___sFILE)); _temp0->file= cf; _temp0;}): 0;}
struct _tagged_arr Cyc_Stdio_tmpnam( struct _tagged_arr s){ struct _tagged_arr
name= Cstring_to_string( tmpnam( 0)); return( unsigned int) s.curr? Cyc_String_strcpy(
s,( struct _tagged_arr) name): name;} int Cyc_Stdio_fclose( struct Cyc_Stdio___sFILE*
f){ int r= fclose(( struct Cyc_Cstdio___sFILE*) _check_null( f->file)); if( r == 
0){ f->file= 0;} return r;} int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE* f){
return( unsigned int) f? fflush((( struct Cyc_Stdio___sFILE*) _check_null( f))->file):
fflush( 0);} struct Cyc_Stdio___sFILE* Cyc_Stdio_freopen( struct _tagged_arr x,
struct _tagged_arr y, struct Cyc_Stdio___sFILE* f){ struct Cyc_Cstdio___sFILE*
cf= freopen( string_to_Cstring( x), string_to_Cstring( y),( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); return( unsigned int) cf?({ struct Cyc_Stdio___sFILE*
_temp1=( struct Cyc_Stdio___sFILE*) GC_malloc( sizeof( struct Cyc_Stdio___sFILE));
_temp1->file= cf; _temp1;}): 0;} void Cyc_Stdio_setbuf( struct Cyc_Stdio___sFILE*
f, struct _tagged_arr buf){ Cyc_Stdio_setvbuf( f, buf,( unsigned int) buf.curr?
0: 2, 1024);} void Cyc_Stdio_setbuffer( struct Cyc_Stdio___sFILE* f, struct
_tagged_arr buf, int size){ Cyc_Stdio_setvbuf( f, buf,( unsigned int) buf.curr?
0: 2, size);} void Cyc_Stdio_setlinebuf( struct Cyc_Stdio___sFILE* f){ Cyc_Stdio_setvbuf(
f, _tag_arr( 0u, 0u, 0u), 1, 0);} int Cyc_Stdio_setvbuf( struct Cyc_Stdio___sFILE*
f, struct _tagged_arr buf, int mode, int size){ if( _get_arr_size( buf, sizeof(
unsigned char)) <  size){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp2=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp2[ 0]=({ struct Cyc_Core_Failure_struct _temp3; _temp3.tag= Cyc_Core_Failure;
_temp3.f1= _tag_arr("setvbuf: buffer insufficient", sizeof( unsigned char), 29u);
_temp3;}); _temp2;}));} return setvbuf(( struct Cyc_Cstdio___sFILE*) _check_null(
f->file), underlying_Cstring(( struct _tagged_arr) buf), mode, size);} int Cyc_Stdio_fgetc(
struct Cyc_Stdio___sFILE* f){ return fgetc(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} struct _tagged_arr Cyc_Stdio_fgets( struct _tagged_arr
s, int n, struct Cyc_Stdio___sFILE* f){ unsigned char* result; unsigned char*
buffer= underlying_Cstring(( struct _tagged_arr) s); unsigned int len=
_get_arr_size( s, sizeof( unsigned char)); n=( int)( len <  n? len:(
unsigned int) n); result= fgets( buffer, n,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); if( result ==  0){ return _tag_arr( 0u, 0u, 0u);} else{
return s;}} int Cyc_Stdio_fputc( int i, struct Cyc_Stdio___sFILE* f){ return
fputc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_fputs(
struct _tagged_arr s, struct Cyc_Stdio___sFILE* f){ return fputs(
string_to_Cstring( s),( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int
Cyc_Stdio_getc( struct Cyc_Stdio___sFILE* f){ return getc(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_putc( int i, struct Cyc_Stdio___sFILE* f){
return putc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_puts(
struct _tagged_arr s){ return puts( string_to_Cstring( s));} int Cyc_Stdio_ungetc(
int i, struct Cyc_Stdio___sFILE* f){ return ungetc( i,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_fread( struct _tagged_arr ptr, int size,
int nmemb, struct Cyc_Stdio___sFILE* f){ if( size *  nmemb >  _get_arr_size( ptr,
sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp4=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp4[ 0]=({ struct Cyc_Core_Failure_struct _temp5; _temp5.tag= Cyc_Core_Failure;
_temp5.f1= _tag_arr("fread: buffer insufficient", sizeof( unsigned char), 27u);
_temp5;}); _temp4;}));} return fread( underlying_Cstring(( struct _tagged_arr)
ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_fwrite(
struct _tagged_arr ptr, int size, int nmemb, struct Cyc_Stdio___sFILE* f){ if(
size *  nmemb >  _get_arr_size( ptr, sizeof( unsigned char))){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp6=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp6[ 0]=({ struct Cyc_Core_Failure_struct
_temp7; _temp7.tag= Cyc_Core_Failure; _temp7.f1= _tag_arr("fwrite: buffer insufficient",
sizeof( unsigned char), 28u); _temp7;}); _temp6;}));} return fwrite(
underlying_Cstring( ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Stdio_fgetpos( struct Cyc_Stdio___sFILE* f, int* x){ return
fgetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file), x);} int Cyc_Stdio_fseek(
struct Cyc_Stdio___sFILE* f, int offset, int whence){ return fseek(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file), offset, whence);} int Cyc_Stdio_fsetpos( struct Cyc_Stdio___sFILE*
f, int* x){ return fsetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file),
x);} int Cyc_Stdio_ftell( struct Cyc_Stdio___sFILE* f){ return ftell(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file));} void Cyc_Stdio_rewind( struct Cyc_Stdio___sFILE*
f){ rewind(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void Cyc_Stdio_clearerr(
struct Cyc_Stdio___sFILE* f){ clearerr(( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Stdio_feof( struct Cyc_Stdio___sFILE* f){ return feof((
struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_ferror(
struct Cyc_Stdio___sFILE* f){ return ferror(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} void Cyc_Stdio_perror( struct _tagged_arr s){ perror(
string_to_Cstring( s));} struct Cyc_Stdio___sFILE* Cyc_Stdio_fopen( struct
_tagged_arr name, struct _tagged_arr type){ struct Cyc_Cstdio___sFILE* cf= fopen(
string_to_Cstring( name), string_to_Cstring( type)); return( unsigned int) cf?({
struct Cyc_Stdio___sFILE* _temp8=( struct Cyc_Stdio___sFILE*) GC_malloc( sizeof(
struct Cyc_Stdio___sFILE)); _temp8->file= cf; _temp8;}): 0;} struct Cyc_Stdio___sFILE*
Cyc_Stdio_fdopen( int i, struct _tagged_arr s){ struct Cyc_Cstdio___sFILE* cf=
fdopen( i, string_to_Cstring( s)); return( unsigned int) cf?({ struct Cyc_Stdio___sFILE*
_temp9=( struct Cyc_Stdio___sFILE*) GC_malloc( sizeof( struct Cyc_Stdio___sFILE));
_temp9->file= cf; _temp9;}): 0;} int Cyc_Stdio_fileno( struct Cyc_Stdio___sFILE*
f){ return fileno(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Stdio_getw(
struct Cyc_Stdio___sFILE* f){ return getw(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Stdio_putw( int i, struct Cyc_Stdio___sFILE* f){
return putw( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));}
unsigned char Cyc_Stdio_FileCloseError[ 19u]="\000\000\000\000FileCloseError";
unsigned char Cyc_Stdio_FileOpenError[ 18u]="\000\000\000\000FileOpenError";
struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct _tagged_arr fname, struct
_tagged_arr mode){ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_fopen( fname, mode);
if( f ==  0){ struct _tagged_arr fn=({ unsigned int _temp12= _get_arr_size(
fname, sizeof( unsigned char)); unsigned char* _temp13=( unsigned char*)
GC_malloc_atomic( sizeof( unsigned char) *  _temp12); struct _tagged_arr _temp15=
_tag_arr( _temp13, sizeof( unsigned char), _get_arr_size( fname, sizeof(
unsigned char)));{ unsigned int _temp14= _temp12; unsigned int i; for( i= 0; i < 
_temp14; i ++){ _temp13[ i]=*(( const unsigned char*) _check_unknown_subscript(
fname, sizeof( unsigned char),( int) i));}}; _temp15;});( int) _throw(( void*)({
struct Cyc_Stdio_FileOpenError_struct* _temp10=( struct Cyc_Stdio_FileOpenError_struct*)
GC_malloc( sizeof( struct Cyc_Stdio_FileOpenError_struct)); _temp10[ 0]=({
struct Cyc_Stdio_FileOpenError_struct _temp11; _temp11.tag= Cyc_Stdio_FileOpenError;
_temp11.f1= fn; _temp11;}); _temp10;}));} return( struct Cyc_Stdio___sFILE*)
_check_null( f);} void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE* f){ if(
Cyc_Stdio_fclose( f) !=  0){( int) _throw(( void*) Cyc_Stdio_FileCloseError);}}
