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
unsigned char*); extern int* __errno(); int Cyc_Execinfo_backtrace( struct
_tagged_arr, int); int Cyc_Execinfo_bt(); struct Cyc_Stdio___sFILE; extern
struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern int Cyc_Stdio_fclose( struct
Cyc_Stdio___sFILE*); extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_putc(
int, struct Cyc_Stdio___sFILE*); extern struct Cyc_Stdio___sFILE* Cyc_Stdio_fdopen(
int, struct _tagged_arr); extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct
Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int __mb_cur_max; extern int abs(
int); extern int atexit( void(* __func)()); extern struct Cyc_Stdlib__Div div(
int __numer, int __denom); extern struct Cyc_Stdlib__Ldiv ldiv( int __numer, int
__denom); extern int rand(); extern void srand( unsigned int __seed); extern int
rand_r( unsigned int* __seed); extern int wait( int* status); extern int waitpid(
int pid, int* status, int options); struct Cyc_Getopt_option{ struct _tagged_arr
name; int has_arg; int* flag; int val; } ; extern unsigned int alarm(
unsigned int seconds); extern int close( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int dup( int);
extern int dup2( int, int); extern unsigned short getuid(); extern int setuid(
unsigned short uid); extern unsigned short geteuid(); extern int seteuid(
unsigned short euid); extern unsigned short getgid(); extern int setgid(
unsigned short gid); extern unsigned short getegid(); extern int setegid(
unsigned short egid); extern int select( int n, struct Cyc__types_fd_set*
readfds, struct Cyc__types_fd_set* writefds, struct Cyc__types_fd_set* exceptfds,
struct Cyc_timeval* timeout); extern int pipe( int* filedes); extern int lseek(
int filedes, int offset, int whence); int Cyc_Unistd_chdir( struct _tagged_arr);
struct _tagged_arr Cyc_Unistd_getcwd( struct _tagged_arr buf, int size); int Cyc_Unistd_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_Unistd_execlp( struct _tagged_arr file, struct _tagged_arr arg0, struct
_tagged_arr argv); int Cyc_Unistd_execve( struct _tagged_arr filename, struct
_tagged_arr argv, struct _tagged_arr envp); int Cyc_Unistd_read( int fd, struct
_tagged_arr buf, int count); int Cyc_Unistd_write( int fd, struct _tagged_arr
buf, int count); int Cyc_Unistd_unlink( struct _tagged_arr pathname); void(* Cyc_Signal_signal(
int sig, void(* func)( int)))( int); extern int raise( int sig); extern int kill(
int, int); extern int backtrace( int*, int); int Cyc_Execinfo_backtrace( struct
_tagged_arr array, int size){ if( size >  _get_arr_size( array, sizeof( int))){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("backtrace: size > array.size",
sizeof( unsigned char), 29u); _temp1;}); _temp0;}));} return backtrace(( int*)
_check_null( _untag_arr( array, sizeof( int), 1u)), size);} int Cyc_Execinfo_bt(){
int bt[ 20u];{ unsigned int _temp10= 20u; unsigned int i; for( i= 0; i < 
_temp10; i ++){ bt[ i]= 0;}}{ int tochild[ 2u]={ 0, 0}; int fromchild[ 2u]={ 0,
0}; int pid; int self_pid; if( pipe( tochild)? 1: pipe( fromchild)){ return 1;}
self_pid= getpid(); if(( pid= fork()) ==  0){ if( dup2( tochild[
_check_known_subscript_notnull( 2u, 0)], 0) ==  - 1? 1: dup2( fromchild[
_check_known_subscript_notnull( 2u, 1)], 1) ==  - 1){ return 1;}({ struct
_tagged_arr _temp2[ 4u]; _temp2[ 3u]=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);
_temp2[ 2u]=( struct _tagged_arr)({ struct Cyc_Stdio_Int_pa_struct _temp4;
_temp4.tag= Cyc_Stdio_Int_pa; _temp4.f1=( int)(( unsigned int) self_pid);{ void*
_temp3[ 1u]={& _temp4}; Cyc_Stdio_aprintf( _tag_arr("/proc/%d/exe", sizeof(
unsigned char), 13u), _tag_arr( _temp3, sizeof( void*), 1u));}}); _temp2[ 1u]=
_tag_arr("-e", sizeof( unsigned char), 3u); _temp2[ 0u]= _tag_arr("--functions",
sizeof( unsigned char), 12u); Cyc_Unistd_execlp( _tag_arr("addr2line", sizeof(
unsigned char), 10u), _tag_arr("addr2line", sizeof( unsigned char), 10u),
_tag_arr( _temp2, sizeof( struct _tagged_arr), 4u));}); return 1;} else{ if( pid
<  0){ close( tochild[ _check_known_subscript_notnull( 2u, 0)]); close( tochild[
_check_known_subscript_notnull( 2u, 1)]); close( fromchild[
_check_known_subscript_notnull( 2u, 0)]); close( fromchild[
_check_known_subscript_notnull( 2u, 1)]); return 1;}}{ struct Cyc_Stdio___sFILE*
w=({ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_fdopen( tochild[
_check_known_subscript_notnull( 2u, 1)], _tag_arr("w", sizeof( unsigned char), 2u));
if( !(( unsigned int) f)){ return 1;}( struct Cyc_Stdio___sFILE*) _check_null( f);});
struct Cyc_Stdio___sFILE* r=({ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_fdopen(
fromchild[ _check_known_subscript_notnull( 2u, 0)], _tag_arr("r", sizeof(
unsigned char), 2u)); if( !(( unsigned int) f)){ Cyc_Stdio_fclose( w); return 1;}(
struct Cyc_Stdio___sFILE*) _check_null( f);}); int n= Cyc_Execinfo_backtrace(
_tag_arr( bt, sizeof( int), 20u),( int) 20u);{ int c= 0; for( 0; c <  n; c ++){({
struct Cyc_Stdio_Int_pa_struct _temp6; _temp6.tag= Cyc_Stdio_Int_pa; _temp6.f1=(
unsigned int) bt[ _check_known_subscript_notnull( 20u, c)];{ void* _temp5[ 1u]={&
_temp6}; Cyc_Stdio_fprintf( w, _tag_arr("%#x\n", sizeof( unsigned char), 5u),
_tag_arr( _temp5, sizeof( void*), 1u));}});}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
w);({ void* _temp7[ 0u]={}; Cyc_Stdio_printf( _tag_arr("Backtrace:\n  Function          Location\n  ----------------  --------------------------------\n",
sizeof( unsigned char), 94u), _tag_arr( _temp7, sizeof( void*), 0u));});{ int c=
0; for( 0; c <  n; c ++){ int d; int pos= 0;({ void* _temp8[ 0u]={}; Cyc_Stdio_printf(
_tag_arr("  ", sizeof( unsigned char), 3u), _tag_arr( _temp8, sizeof( void*), 0u));});
while(( d= Cyc_Stdio_getc( r)) != '\n') {( Cyc_Stdio_putc( d, Cyc_Stdio_stdout),
pos ++);} while( pos ++ <  16) { Cyc_Stdio_putc(( int)' ', Cyc_Stdio_stdout);}({
void* _temp9[ 0u]={}; Cyc_Stdio_printf( _tag_arr("  ", sizeof( unsigned char), 3u),
_tag_arr( _temp9, sizeof( void*), 0u));}); while(( d= Cyc_Stdio_getc( r)) != '\n') {
Cyc_Stdio_putc( d, Cyc_Stdio_stdout);} Cyc_Stdio_putc(( int)'\n', Cyc_Stdio_stdout);}}
Cyc_Stdio_fclose( r); Cyc_Stdio_fclose( w); kill( pid, 15); waitpid( pid, 0, 0);
return 0;}}}
