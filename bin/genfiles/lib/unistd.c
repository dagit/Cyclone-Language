 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
unsigned char*); extern int* __errno(); struct Cyc_Getopt_option{ struct
_tagged_arr name; int has_arg; int* flag; int val; } ; extern int close( int);
extern int write( int, const unsigned char*, int); extern int getpid(); extern
int getppid(); extern int fork(); extern int fchdir( int); extern int dup( int);
extern int dup2( int, int); extern unsigned short getuid(); extern
unsigned short geteuid(); extern unsigned short getgid(); extern unsigned short
getegid(); extern int select( int n, struct Cyc__types_fd_set* readfds, struct
Cyc__types_fd_set* writefds, struct Cyc__types_fd_set* exceptfds, struct Cyc_timeval*
timeout); int Cyc_Unistd_chdir( struct _tagged_arr); struct _tagged_arr Cyc_Unistd_getcwd(
struct _tagged_arr buf, int size); int Cyc_Unistd_execl( struct _tagged_arr path,
struct _tagged_arr arg0, struct _tagged_arr argv); extern int chdir(
unsigned char*); extern unsigned char* getcwd( unsigned char* buf, int size);
extern int execv( unsigned char* path, unsigned char** argv); int Cyc_Unistd_chdir(
struct _tagged_arr path){ return chdir( string_to_Cstring( path));} struct
_tagged_arr Cyc_Unistd_getcwd( struct _tagged_arr buf, int size){ if( !((
unsigned int) buf.curr)? 1: _get_arr_size( buf, sizeof( unsigned char)) < size){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("getcwd: invalid buf argument",
sizeof( unsigned char), 29u); _temp1;}); _temp0;}));}{ unsigned char* response=
getcwd(( unsigned char*) _check_null( _untag_arr( buf, sizeof( unsigned char), 0u)),
size); return( unsigned int) response? buf: _tag_arr( 0u, 0u, 0u);}} int Cyc_Unistd_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv){ if((*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) - 1)))).curr !=(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp2=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp2[ 0]=({ struct Cyc_Core_Failure_struct _temp3; _temp3.tag= Cyc_Core_Failure;
_temp3.f1= _tag_arr("execl: arg list must be null-terminated", sizeof(
unsigned char), 40u); _temp3;}); _temp2;}));}{ struct _tagged_arr newargs=({
unsigned int _temp4= 1 + _get_arr_size( argv, sizeof( struct _tagged_arr));
unsigned char** _temp5=( unsigned char**) GC_malloc( sizeof( unsigned char*) *
_temp4); struct _tagged_arr _temp7= _tag_arr( _temp5, sizeof( unsigned char*), 1
+ _get_arr_size( argv, sizeof( struct _tagged_arr)));{ unsigned int _temp6=
_temp4; unsigned int i; for( i= 0; i < _temp6; i ++){ _temp5[ i]= 0;}}; _temp7;});*((
unsigned char**) _check_unknown_subscript( newargs, sizeof( unsigned char*), 0))=
string_to_Cstring( arg0);{ int i= 0; for( 0; i < _get_arr_size( argv, sizeof(
struct _tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), i + 1))= string_to_Cstring(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), i)));}}
return execv( string_to_Cstring( path),( unsigned char**) _check_null(
_untag_arr( newargs, sizeof( unsigned char*), 1u)));}}