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
unsigned char*); extern int* __errno(); struct Cyc_Stdlib__Div{ int quot; int
rem; } ; struct Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int __mb_cur_max;
extern int abs( int); extern int atexit( void(* __func)()); extern struct Cyc_Stdlib__Div
div( int __numer, int __denom); extern struct Cyc_Stdlib__Ldiv ldiv( int __numer,
int __denom); extern int rand(); extern void srand( unsigned int __seed); extern
int rand_r( unsigned int* __seed); extern double Cyc_Stdlib_atof( struct
_tagged_arr); extern int Cyc_Stdlib_atoi( struct _tagged_arr); extern int Cyc_Stdlib_atol(
struct _tagged_arr); extern struct _tagged_arr Cyc_Stdlib_getenv( struct
_tagged_arr); extern double Cyc_Stdlib_strtod( struct _tagged_arr n, struct
_tagged_arr* end); extern int Cyc_Stdlib_strtol( struct _tagged_arr n, struct
_tagged_arr* end, int base); extern unsigned int Cyc_Stdlib_strtoul( struct
_tagged_arr n, struct _tagged_arr* end, int base); extern double atof(
unsigned char* _nptr); extern int atoi( unsigned char* _nptr); extern int atol(
unsigned char* _nptr); extern unsigned char* getenv( unsigned char* __string);
extern unsigned char* _findenv( unsigned char*, int*); extern int putenv(
unsigned char* __string); extern double strtod( unsigned char*, unsigned char**);
extern int strtol( unsigned char*, unsigned char**, int base); extern
unsigned int strtoul( unsigned char*, unsigned char**, int base); double Cyc_Stdlib_atof(
struct _tagged_arr _nptr){ return atof( string_to_Cstring( _nptr));} int Cyc_Stdlib_atoi(
struct _tagged_arr _nptr){ return atoi( string_to_Cstring( _nptr));} int Cyc_Stdlib_atol(
struct _tagged_arr _nptr){ return atol( string_to_Cstring( _nptr));} struct
_tagged_arr Cyc_Stdlib_getenv( struct _tagged_arr name){ return
Cstring_to_string( getenv( string_to_Cstring( name)));} int Cyc_Stdlib_putenv(
struct _tagged_arr s){ return putenv( string_to_Cstring( s));} static void Cyc_Stdlib_check_valid_cstring(
struct _tagged_arr s){ if( s.curr ==(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp0=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp1; _temp1.tag= Cyc_Core_InvalidArg; _temp1.f1= _tag_arr("strtox null pointer",
sizeof( unsigned char), 20u); _temp1;}); _temp0;}));}{ int found_zero= 0;{ int i=(
int)( _get_arr_size( s, sizeof( unsigned char)) - 1); for( 0; i >= 0; i --){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
=='\000'){ found_zero= 1; break;}}} if( ! found_zero){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp2=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp3; _temp3.tag= Cyc_Core_InvalidArg; _temp3.f1= _tag_arr("strtox: not a C string",
sizeof( unsigned char), 23u); _temp3;}); _temp2;}));}}} double Cyc_Stdlib_strtod(
struct _tagged_arr nptr, struct _tagged_arr* endptr){ Cyc_Stdlib_check_valid_cstring(
nptr);{ unsigned char* c= underlying_Cstring( nptr); unsigned char* e= endptr ==
0? 0: c; double d= strtod( c,( unsigned char**)& e); if( endptr != 0){ int n=(
int)(( unsigned int) e -( unsigned int) c);*(( struct _tagged_arr*) _check_null(
endptr))= _tagged_arr_plus( nptr, sizeof( unsigned char), n);} return d;}} int
Cyc_Stdlib_strtol( struct _tagged_arr n, struct _tagged_arr* endptr, int base){
Cyc_Stdlib_check_valid_cstring( n);{ unsigned char* c= underlying_Cstring( n);
unsigned char* e= endptr == 0? 0: c; int r= strtol( c,( unsigned char**)& e,
base); if( endptr != 0){ int m=( int)(( unsigned int) e -( unsigned int) c);*((
struct _tagged_arr*) _check_null( endptr))= _tagged_arr_plus( n, sizeof(
unsigned char), m);} return r;}} unsigned int Cyc_Stdlib_strtoul( struct
_tagged_arr n, struct _tagged_arr* endptr, int base){ Cyc_Stdlib_check_valid_cstring(
n);{ unsigned char* c= underlying_Cstring( n); unsigned char* e= endptr == 0? 0:
c; unsigned int r= strtoul( c,( unsigned char**)& e, base); if( endptr != 0){
int m=( int)(( unsigned int) e -( unsigned int) c);*(( struct _tagged_arr*)
_check_null( endptr))= _tagged_arr_plus( n, sizeof( unsigned char), m);} return
r;}}
