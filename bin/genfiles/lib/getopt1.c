 struct Cyc_Getopt_option{ struct _tagged_string name; int has_arg; int* flag;
int val; } ; struct _tagged_ptr0{ struct _tagged_string* curr; struct
_tagged_string* base; struct _tagged_string* last_plus_one; } ; struct
_tagged_ptr1{ struct Cyc_Getopt_option* curr; struct Cyc_Getopt_option* base;
struct Cyc_Getopt_option* last_plus_one; } ; extern int Cyc_Getopt_getopt_long(
int argc, struct _tagged_ptr0 argv, struct _tagged_string shortopts, struct
_tagged_ptr1 longopts, int* longind); extern int Cyc_Getopt_getopt_long_only(
int argc, struct _tagged_ptr0 argv, struct _tagged_string shortopts, struct
_tagged_ptr1 longopts, int* longind); extern int Cyc_Getopt__getopt_internal(
int argc, struct _tagged_ptr0 argv, struct _tagged_string shortopts, struct
_tagged_ptr1 longopts, int* longind, int long_only); struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int fds_bits[ 2u]; }
; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; }
; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Getopt_Time_tm{ int
tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern unsigned int clock(); extern
double difftime( int _time2, int _time1); extern int mktime( struct Cyc_Getopt_Time_tm*
_timeptr); extern int time( int* _timer); extern struct Cyc_Getopt_Time_tm*
gmtime( const int* _timer); extern struct Cyc_Getopt_Time_tm* localtime( const
int* _timer); extern struct Cyc_Getopt_Time_tm* gmtime_r( const int*, struct Cyc_Getopt_Time_tm*);
extern struct Cyc_Getopt_Time_tm* localtime_r( const int*, struct Cyc_Getopt_Time_tm*);
struct Cyc_Getopt___sFILE; struct Cyc_Getopt__reent; struct Cyc_Getopt__glue{
struct Cyc_Getopt__glue* _next; int _niobs; struct Cyc_Getopt___sFILE* _iobs; }
; struct Cyc_Getopt__Bigint{ struct Cyc_Getopt__Bigint* _next; int _k; int
_maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc_Getopt__atexit{
struct Cyc_Getopt__atexit* _next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc_Getopt___sbuf{
unsigned char* _base; int _size; } ; struct Cyc_Getopt___sFILE{ unsigned char*
_p; int _r; int _w; short _flags; short _file; struct Cyc_Getopt___sbuf _bf; int
_lbfsize; void* _cookie; int(* _read)( void* _cookie, unsigned char* _buf, int
_n); int(* _write)( void* _cookie, const unsigned char* _buf, int _n); int(*
_seek)( void* _cookie, int _offset, int _whence); int(* _close)( void* _cookie);
struct Cyc_Getopt___sbuf _ub; unsigned char* _up; int _ur; unsigned char _ubuf[
3u]; unsigned char _nbuf[ 1u]; struct Cyc_Getopt___sbuf _lb; int _blksize; int
_offset; struct Cyc_Getopt__reent* _data; } ; struct Cyc_Getopt__reent_u1{
unsigned int _unused_rand; int _strtok_last; unsigned char _asctime_buf[ 26u];
struct Cyc_Getopt_Time_tm _localtime_buf; int _gamma_signgam; unsigned long long
_rand_next; } ; struct Cyc_Getopt__reent_u2{ unsigned int _nextf[ 30u];
unsigned int _nmalloc[ 30u]; } ; union Cyc_Getopt__reent_union{ struct Cyc_Getopt__reent_u1
_reent; struct Cyc_Getopt__reent_u2 _unused; } ; struct Cyc_Getopt__reent{ int
_errno; struct Cyc_Getopt___sFILE* _stdin; struct Cyc_Getopt___sFILE* _stdout;
struct Cyc_Getopt___sFILE* _stderr; int _inc; unsigned char _emergency[ 25u];
int _current_category; const unsigned char* _current_locale; int __sdidinit;
void(* __cleanup)( struct Cyc_Getopt__reent*); struct Cyc_Getopt__Bigint*
_result; int _result_k; struct Cyc_Getopt__Bigint* _p5s; struct Cyc_Getopt__Bigint**
_freelist; int _cvtlen; unsigned char* _cvtbuf; union Cyc_Getopt__reent_union
_new; struct Cyc_Getopt__atexit* _atexit; struct Cyc_Getopt__atexit _atexit0;
void(** _sig_func)( int); struct Cyc_Getopt__glue __sglue; struct Cyc_Getopt___sFILE
__sf[ 3u]; } ; extern struct Cyc_Getopt__reent* _impure_ptr; extern void
_reclaim_reent( struct Cyc_Getopt__reent*); struct Cyc_Getopt_Stdlib__Div{ int
quot; int rem; } ; struct Cyc_Getopt_Stdlib__Ldiv{ int quot; int rem; } ; extern
int __mb_cur_max; extern int abs( int); extern int atexit( void(* __func)());
extern struct Cyc_Getopt_Stdlib__Div div( int __numer, int __denom); extern
struct Cyc_Getopt_Stdlib__Ldiv ldiv( int __numer, int __denom); extern int rand();
extern void srand( unsigned int __seed); extern int rand_r( unsigned int* __seed);
int Cyc_Getopt_getopt_long( int argc, struct _tagged_ptr0 argv, struct
_tagged_string options, struct _tagged_ptr1 long_options, int* opt_index){
return Cyc_Getopt__getopt_internal( argc, argv, options, long_options, opt_index,
0);} int Cyc_Getopt_getopt_long_only( int argc, struct _tagged_ptr0 argv, struct
_tagged_string options, struct _tagged_ptr1 long_options, int* opt_index){
return Cyc_Getopt__getopt_internal( argc, argv, options, long_options, opt_index,
1);}