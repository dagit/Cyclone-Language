 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
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
unsigned char*); extern int* __errno(); struct Cyc_Getopt_option{ struct
_tagged_arr name; int has_arg; int* flag; int val; } ; extern int close( int);
extern int write( int, const unsigned char*, int); extern int getpid(); extern
int getppid(); extern int fork(); extern int fchdir( int); extern int dup( int);
extern int dup2( int, int); extern unsigned short getuid(); extern
unsigned short geteuid(); extern unsigned short getgid(); extern unsigned short
getegid(); int Cyc_Unistd_chdir( struct _tagged_arr); struct _tagged_arr Cyc_Unistd_getcwd(
struct _tagged_arr buf, int size); extern int chdir( unsigned char*); extern
unsigned char* getcwd( unsigned char* buf, int size); int Cyc_Unistd_chdir(
struct _tagged_arr path){ return chdir( string_to_Cstring( path));} struct
_tagged_arr Cyc_Unistd_getcwd( struct _tagged_arr buf, int size){ if( !((
unsigned int) buf.curr)? 1: _get_arr_size( buf, sizeof( unsigned char)) < size){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("getcwd: invalid buf argument",
sizeof( unsigned char), 29u); _temp1;}); _temp0;}));}{ unsigned char* response=
getcwd(( unsigned char*) _check_null( _untag_arr( buf, sizeof( unsigned char), 0u)),
size); return( unsigned int) response? buf: _tag_arr( 0u, sizeof( unsigned char),
0u);}}