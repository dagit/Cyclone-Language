#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); typedef
int Cyc___int32_t; typedef unsigned int Cyc___uint32_t; struct Cyc_Time_tm{ int
tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern unsigned int clock()
 __attribute__(( cdecl )) ; extern double difftime( int _time2, int _time1)
 __attribute__(( cdecl )) ; extern int mktime( struct Cyc_Time_tm* _timeptr)
 __attribute__(( cdecl )) ; extern int time( int* _timer)  __attribute__(( cdecl
)) ; extern struct Cyc_Time_tm* gmtime( const int* _timer)  __attribute__((
cdecl )) ; extern struct Cyc_Time_tm* localtime( const int* _timer)
 __attribute__(( cdecl )) ; extern struct Cyc_Time_tm* gmtime_r( const int*,
struct Cyc_Time_tm*)  __attribute__(( cdecl )) ; extern struct Cyc_Time_tm*
localtime_r( const int*, struct Cyc_Time_tm*)  __attribute__(( cdecl )) ; extern
int _timezone  __attribute__(( dllimport )) ; extern int _daylight
 __attribute__(( dllimport )) ; extern unsigned char* _tzname[ 2u]
 __attribute__(( dllimport )) ; extern void tzset()  __attribute__(( cdecl )) ;
struct Cyc___sFILE; struct Cyc__reent; typedef unsigned int Cyc___ULong; struct
Cyc__glue{ struct Cyc__glue* _next; int _niobs; struct Cyc___sFILE* _iobs; } ;
struct Cyc__Bigint{ struct Cyc__Bigint* _next; int _k; int _maxwds; int _sign;
int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{ struct Cyc__atexit*
_next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{ unsigned char*
_base; int _size; } ; typedef int Cyc__fpos_t; struct Cyc___sFILE{ unsigned char*
_p; int _r; int _w; short _flags; short _file; struct Cyc___sbuf _bf; int
_lbfsize; void* _cookie; int(* _read)( void* _cookie, unsigned char* _buf, int
_n)  __attribute__(( cdecl )) ; int(* _write)( void* _cookie, const
unsigned char* _buf, int _n)  __attribute__(( cdecl )) ; int(* _seek)( void*
_cookie, int _offset, int _whence)  __attribute__(( cdecl )) ; int(* _close)(
void* _cookie)  __attribute__(( cdecl )) ; struct Cyc___sbuf _ub; unsigned char*
_up; int _ur; unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u]; struct Cyc___sbuf
_lb; int _blksize; int _offset; struct Cyc__reent* _data; } ; struct Cyc__reent_u1{
unsigned int _unused_rand; int _strtok_last; unsigned char _asctime_buf[ 26u];
struct Cyc_Time_tm _localtime_buf; int _gamma_signgam; unsigned long long
_rand_next; } ; struct Cyc__reent_u2{ unsigned int _nextf[ 30u]; unsigned int
_nmalloc[ 30u]; } ; union Cyc__reent_union{ struct Cyc__reent_u1 _reent; struct
Cyc__reent_u2 _unused; } ; struct Cyc__reent{ int _errno; struct Cyc___sFILE*
_stdin; struct Cyc___sFILE* _stdout; struct Cyc___sFILE* _stderr; int _inc;
unsigned char _emergency[ 25u]; int _current_category; const unsigned char*
_current_locale; int __sdidinit; void(* __cleanup)( struct Cyc__reent*)
 __attribute__(( cdecl )) ; struct Cyc__Bigint* _result; int _result_k; struct
Cyc__Bigint* _p5s; struct Cyc__Bigint** _freelist; int _cvtlen; unsigned char*
_cvtbuf; union Cyc__reent_union _new; struct Cyc__atexit* _atexit; struct Cyc__atexit
_atexit0; void(** _sig_func)( int); struct Cyc__glue __sglue; struct Cyc___sFILE
__sf[ 3u]; } ; extern struct Cyc__reent* _impure_ptr; extern void _reclaim_reent(
struct Cyc__reent*); struct Cyc_Stdlib__Div{ int quot; int rem; } ; typedef
struct Cyc_Stdlib__Div Cyc_Stdlib_div_t; struct Cyc_Stdlib__Ldiv{ int quot; int
rem; } ; typedef struct Cyc_Stdlib__Ldiv Cyc_Stdlib_ldiv_t; extern int
__mb_cur_max  __attribute__(( dllimport )) ; extern int abs( int)
 __attribute__(( cdecl )) ; extern int atexit( void(* __func)())
 __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Div div( int __numer, int
__denom)  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Ldiv ldiv( int
__numer, int __denom)  __attribute__(( cdecl )) ; extern int rand()
 __attribute__(( cdecl )) ; extern void srand( unsigned int __seed)
 __attribute__(( cdecl )) ; extern int rand_r( unsigned int* __seed)
 __attribute__(( cdecl )) ; extern int random()  __attribute__(( cdecl )) ;
extern int srandom( unsigned int __seed)  __attribute__(( cdecl )) ; extern int
grantpt( int)  __attribute__(( cdecl )) ; extern int unlockpt( int)
 __attribute__(( cdecl )) ; extern double Cyc_Stdlib_atof( struct _tagged_string);
extern float Cyc_Stdlib_atoff( struct _tagged_string); extern int Cyc_Stdlib_atoi(
struct _tagged_string); extern int Cyc_Stdlib_atol( struct _tagged_string);
extern struct _tagged_string Cyc_Stdlib_getenv( struct _tagged_string); extern
struct _tagged_string Cyc_Stdlib__getenv_r( struct Cyc__reent*, struct
_tagged_string); extern double atof( unsigned char* _nptr)  __attribute__((
cdecl )) ; extern float atoff( unsigned char* _nptr)  __attribute__(( cdecl )) ;
extern int atoi( unsigned char* _nptr)  __attribute__(( cdecl )) ; extern int
atol( unsigned char* _nptr)  __attribute__(( cdecl )) ; extern unsigned char*
getenv( unsigned char* __string)  __attribute__(( cdecl )) ; extern
unsigned char* _getenv_r( struct Cyc__reent*, unsigned char* __string)
 __attribute__(( cdecl )) ; extern unsigned char* _findenv( unsigned char*, int*)
 __attribute__(( cdecl )) ; extern unsigned char* _findenv_r( struct Cyc__reent*,
unsigned char*, int*)  __attribute__(( cdecl )) ; extern int putenv(
unsigned char* __string)  __attribute__(( cdecl )) ; extern int _putenv_r(
struct Cyc__reent*, unsigned char* __string)  __attribute__(( cdecl )) ; extern
int setenv( unsigned char* __string, unsigned char* __value, int __overwrite)
 __attribute__(( cdecl )) ; extern int _setenv_r( struct Cyc__reent*,
unsigned char* __string, unsigned char* __value, int __overwrite)
 __attribute__(( cdecl )) ; extern void unsetenv( unsigned char* __string)
 __attribute__(( cdecl )) ; extern void _unsetenv_r( struct Cyc__reent*,
unsigned char* __string)  __attribute__(( cdecl )) ; double Cyc_Stdlib_atof(
struct _tagged_string _nptr){ return atof( string_to_Cstring( _nptr));} float
Cyc_Stdlib_atoff( struct _tagged_string _nptr){ return atoff( string_to_Cstring(
_nptr));} int Cyc_Stdlib_atoi( struct _tagged_string _nptr){ return atoi(
string_to_Cstring( _nptr));} int Cyc_Stdlib_atol( struct _tagged_string _nptr){
return atol( string_to_Cstring( _nptr));} struct _tagged_string Cyc_Stdlib_getenv(
struct _tagged_string name){ return Cstring_to_string( getenv( string_to_Cstring(
name)));} struct _tagged_string Cyc_Stdlib__getenv_r( struct Cyc__reent* r,
struct _tagged_string name){ return Cstring_to_string( _getenv_r( r,
string_to_Cstring( name)));} struct _tagged_string Cyc_Stdlib__findenv( struct
_tagged_string n, int* i){ return Cstring_to_string( _findenv( string_to_Cstring(
n), i));} struct _tagged_string Cyc_Stdlib__findenv_r( struct Cyc__reent* r,
struct _tagged_string n, int* i){ return Cstring_to_string( _findenv_r(( struct
Cyc__reent*) r, string_to_Cstring( n), i));} int Cyc_Stdlib_putenv( struct
_tagged_string s){ return putenv( string_to_Cstring( s));} int Cyc_Stdlib__putenv_r(
struct Cyc__reent* r, struct _tagged_string s){ return _putenv_r( r,
string_to_Cstring( s));} int Cyc_Stdlib_setenv( struct _tagged_string s, struct
_tagged_string v, int overwrite){ return setenv( string_to_Cstring( s),
string_to_Cstring( v), overwrite);} int Cyc_Stdlib__setenv_r( struct Cyc__reent*
r, struct _tagged_string s, struct _tagged_string v, int overwrite){ return
_setenv_r( r, string_to_Cstring( s), string_to_Cstring( v), overwrite);} void
Cyc_Stdlib_unsetenv( struct _tagged_string s){ unsetenv( string_to_Cstring( s));}
void Cyc_Stdlib__unsetenv_r( struct Cyc__reent* r, struct _tagged_string s){
_unsetenv_r( r, string_to_Cstring( s));}