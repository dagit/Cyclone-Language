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
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct
Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int __mb_cur_max
 __attribute__(( dllimport )) ; extern int abs( int)  __attribute__(( cdecl )) ;
extern int atexit( void(* __func)())  __attribute__(( cdecl )) ; extern struct
Cyc_Stdlib__Div div( int __numer, int __denom)  __attribute__(( cdecl )) ;
extern struct Cyc_Stdlib__Ldiv ldiv( int __numer, int __denom)  __attribute__((
cdecl )) ; extern int rand()  __attribute__(( cdecl )) ; extern void srand(
unsigned int __seed)  __attribute__(( cdecl )) ; extern int rand_r( unsigned int*
__seed)  __attribute__(( cdecl )) ; extern int random()  __attribute__(( cdecl
)) ; extern int srandom( unsigned int __seed)  __attribute__(( cdecl )) ; extern
int grantpt( int)  __attribute__(( cdecl )) ; extern int unlockpt( int)
 __attribute__(( cdecl )) ; extern int wait( int* status); extern int waitpid(
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
int, int); int Cyc_Execinfo_backtrace( struct _tagged_arr array, int size){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("backtrace: unsupported except under Linux",
sizeof( unsigned char), 42u); _temp1;}); _temp0;})); return 0;} int Cyc_Execinfo_bt(){
return 1;}
