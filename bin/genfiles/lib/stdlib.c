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
unsigned char*); extern int* __errno(); struct Cyc_Time_tm{ int tm_sec; int
tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int
tm_yday; int tm_isdst; } ; extern unsigned int time( unsigned int* t); struct
Cyc___sFILE; struct Cyc__reent; struct Cyc__glue{ struct Cyc__glue* _next; int
_niobs; struct Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{ struct Cyc__Bigint*
_next; int _k; int _maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ;
struct Cyc__atexit{ struct Cyc__atexit* _next; int _ind; void(* _fns[ 32u])(); }
; struct Cyc___sbuf{ unsigned char* _base; int _size; } ; struct Cyc___sFILE{
unsigned char* _p; int _r; int _w; short _flags; short _file; struct Cyc___sbuf
_bf; int _lbfsize; void* _cookie; int(* _read)( void* _cookie, unsigned char*
_buf, int _n); int(* _write)( void* _cookie, const unsigned char* _buf, int _n);
int(* _seek)( void* _cookie, int _offset, int _whence); int(* _close)( void*
_cookie); struct Cyc___sbuf _ub; unsigned char* _up; int _ur; unsigned char
_ubuf[ 3u]; unsigned char _nbuf[ 1u]; struct Cyc___sbuf _lb; int _blksize; int
_offset; struct Cyc__reent* _data; } ; struct Cyc__reent_u1{ unsigned int
_unused_rand; int _strtok_last; unsigned char _asctime_buf[ 26u]; struct Cyc_Time_tm
_localtime_buf; int _gamma_signgam; unsigned long long _rand_next; } ; struct
Cyc__reent_u2{ unsigned int _nextf[ 30u]; unsigned int _nmalloc[ 30u]; } ; union
Cyc__reent_union{ struct Cyc__reent_u1 _reent; struct Cyc__reent_u2 _unused; } ;
struct Cyc__reent{ int _errno; struct Cyc___sFILE* _stdin; struct Cyc___sFILE*
_stdout; struct Cyc___sFILE* _stderr; int _inc; unsigned char _emergency[ 25u];
int _current_category; const unsigned char* _current_locale; int __sdidinit;
void(* __cleanup)( struct Cyc__reent*); struct Cyc__Bigint* _result; int
_result_k; struct Cyc__Bigint* _p5s; struct Cyc__Bigint** _freelist; int _cvtlen;
unsigned char* _cvtbuf; union Cyc__reent_union _new; struct Cyc__atexit* _atexit;
struct Cyc__atexit _atexit0; void(** _sig_func)( int); struct Cyc__glue __sglue;
struct Cyc___sFILE __sf[ 3u]; } ; extern struct Cyc__reent* _impure_ptr; extern
void _reclaim_reent( struct Cyc__reent*); struct Cyc_Stdlib__Div{ int quot; int
rem; } ; struct Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int __mb_cur_max;
extern int abs( int); extern int atexit( void(* __func)()); extern struct Cyc_Stdlib__Div
div( int __numer, int __denom); extern struct Cyc_Stdlib__Ldiv ldiv( int __numer,
int __denom); extern int rand(); extern void srand( unsigned int __seed); extern
int rand_r( unsigned int* __seed); extern double Cyc_Stdlib_atof( struct
_tagged_arr); extern int Cyc_Stdlib_atoi( struct _tagged_arr); extern int Cyc_Stdlib_atol(
struct _tagged_arr); extern struct _tagged_arr Cyc_Stdlib_getenv( struct
_tagged_arr); extern double atof( unsigned char* _nptr); extern int atoi(
unsigned char* _nptr); extern int atol( unsigned char* _nptr); extern
unsigned char* getenv( unsigned char* __string); extern unsigned char* _getenv_r(
struct Cyc__reent*, unsigned char* __string); extern unsigned char* _findenv(
unsigned char*, int*); extern unsigned char* _findenv_r( struct Cyc__reent*,
unsigned char*, int*); extern int putenv( unsigned char* __string); extern int
_putenv_r( struct Cyc__reent*, unsigned char* __string); extern int setenv(
unsigned char* __string, unsigned char* __value, int __overwrite); extern int
_setenv_r( struct Cyc__reent*, unsigned char* __string, unsigned char* __value,
int __overwrite); double Cyc_Stdlib_atof( struct _tagged_arr _nptr){ return atof(
string_to_Cstring( _nptr));} int Cyc_Stdlib_atoi( struct _tagged_arr _nptr){
return atoi( string_to_Cstring( _nptr));} int Cyc_Stdlib_atol( struct
_tagged_arr _nptr){ return atol( string_to_Cstring( _nptr));} struct _tagged_arr
Cyc_Stdlib_getenv( struct _tagged_arr name){ return Cstring_to_string( getenv(
string_to_Cstring( name)));} int Cyc_Stdlib_putenv( struct _tagged_arr s){
return putenv( string_to_Cstring( s));} int Cyc_Stdlib_setenv( struct
_tagged_arr s, struct _tagged_arr v, int overwrite){ return setenv(
string_to_Cstring( s), string_to_Cstring( v), overwrite);}