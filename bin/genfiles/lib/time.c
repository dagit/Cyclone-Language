#include "cyc_include.h"

 typedef int Cyc___int32_t; typedef unsigned int Cyc___uint32_t; typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int
Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t;
typedef unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef
unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
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
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_Time_tm{ int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int
tm_year; int tm_wday; int tm_yday; int tm_isdst; } ; extern unsigned int clock()
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
extern struct _tagged_string Cyc_Time_asctime( const struct Cyc_Time_tm* _tblock);
extern struct _tagged_string Cyc_Time_ctime( const int* _time); extern
unsigned int Cyc_Time_strftime( struct _tagged_string _s, unsigned int _maxsize,
struct _tagged_string _fmt, const struct Cyc_Time_tm* _t); extern struct
_tagged_string Cyc_Time_asctime_r( const struct Cyc_Time_tm*, struct
_tagged_string); extern struct _tagged_string Cyc_Time_ctime_r( const int*,
struct _tagged_string); extern struct _tagged_string Cyc_Time_timezone(); extern
unsigned char* asctime( const struct Cyc_Time_tm* _tblock); extern unsigned char*
ctime( const int* _time); extern unsigned int strftime( unsigned char* _s,
unsigned int _maxsize, unsigned char* _fmt, const struct Cyc_Time_tm* _t);
extern unsigned char* asctime_r( const struct Cyc_Time_tm*, unsigned char*);
extern unsigned char* ctime_r( const int*, unsigned char*); extern unsigned char*
timezone(); struct _tagged_string Cyc_Time_asctime( const struct Cyc_Time_tm*
_tblock){ return Cstring_to_string( asctime( _tblock));} struct _tagged_string
Cyc_Time_ctime( const int* _time){ return Cstring_to_string( ctime( _time));}
unsigned int Cyc_Time_strftime( struct _tagged_string _s, unsigned int _maxsize,
struct _tagged_string _fmt, const struct Cyc_Time_tm* _t){ unsigned int m=({
struct _tagged_string _temp0= _s;( unsigned int)( _temp0.last_plus_one - _temp0.curr);})
< _maxsize?({ struct _tagged_string _temp1= _s;( unsigned int)( _temp1.last_plus_one
- _temp1.curr);}): _maxsize; return strftime( underlying_Cstring( _s), m,
underlying_Cstring( _fmt), _t);} struct _tagged_string Cyc_Time_asctime_r( const
struct Cyc_Time_tm* t, struct _tagged_string s){ if(({ struct _tagged_string
_temp2= s;( unsigned int)( _temp2.last_plus_one - _temp2.curr);}) < 50){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp3=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp3[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp4; _temp4.tag= Cyc_Core_InvalidArg; _temp4.f1=({ unsigned char* _temp5="Time::asctime_r: string too small (< 50)";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 41; _temp6;}); _temp4;}); _temp3;}));} return Cstring_to_string(
asctime_r( t, underlying_Cstring( s)));} struct _tagged_string Cyc_Time_ctime_r(
const int* t, struct _tagged_string s){ if(({ struct _tagged_string _temp7= s;(
unsigned int)( _temp7.last_plus_one - _temp7.curr);}) < 50){( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp8=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp8[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp9; _temp9.tag= Cyc_Core_InvalidArg; _temp9.f1=({ unsigned char* _temp10="Time::ctime_r: string too small (< 50)";
struct _tagged_string _temp11; _temp11.curr= _temp10; _temp11.base= _temp10;
_temp11.last_plus_one= _temp10 + 39; _temp11;}); _temp9;}); _temp8;}));} return
Cstring_to_string( ctime_r( t, underlying_Cstring( s)));} struct _tagged_string
Cyc_Time_timezone(){ return Cstring_to_string( timezone());}