 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Time_tm{ int tm_sec;
int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int
tm_yday; int tm_isdst; } ; extern unsigned int clock(); extern double difftime(
int _time2, int _time1); extern int mktime( struct Cyc_Time_tm* _timeptr);
extern int time( int* _timer); extern struct Cyc_Time_tm* gmtime( const int*
_timer); extern struct Cyc_Time_tm* localtime( const int* _timer); extern struct
Cyc_Time_tm* gmtime_r( const int*, struct Cyc_Time_tm*); extern struct Cyc_Time_tm*
localtime_r( const int*, struct Cyc_Time_tm*); struct Cyc___sFILE; struct Cyc__reent;
struct Cyc__glue{ struct Cyc__glue* _next; int _niobs; struct Cyc___sFILE* _iobs;
} ; struct Cyc__Bigint{ struct Cyc__Bigint* _next; int _k; int _maxwds; int
_sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{ struct Cyc__atexit*
_next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{ unsigned char*
_base; int _size; } ; struct Cyc___sFILE{ unsigned char* _p; int _r; int _w;
short _flags; short _file; struct Cyc___sbuf _bf; int _lbfsize; void* _cookie;
int(* _read)( void* _cookie, unsigned char* _buf, int _n); int(* _write)( void*
_cookie, const unsigned char* _buf, int _n); int(* _seek)( void* _cookie, int
_offset, int _whence); int(* _close)( void* _cookie); struct Cyc___sbuf _ub;
unsigned char* _up; int _ur; unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u];
struct Cyc___sbuf _lb; int _blksize; int _offset; struct Cyc__reent* _data; } ;
struct Cyc__reent_u1{ unsigned int _unused_rand; int _strtok_last; unsigned char
_asctime_buf[ 26u]; struct Cyc_Time_tm _localtime_buf; int _gamma_signgam;
unsigned long long _rand_next; } ; struct Cyc__reent_u2{ unsigned int _nextf[ 30u];
unsigned int _nmalloc[ 30u]; } ; union Cyc__reent_union{ struct Cyc__reent_u1
_reent; struct Cyc__reent_u2 _unused; } ; struct Cyc__reent{ int _errno; struct
Cyc___sFILE* _stdin; struct Cyc___sFILE* _stdout; struct Cyc___sFILE* _stderr;
int _inc; unsigned char _emergency[ 25u]; int _current_category; const
unsigned char* _current_locale; int __sdidinit; void(* __cleanup)( struct Cyc__reent*);
struct Cyc__Bigint* _result; int _result_k; struct Cyc__Bigint* _p5s; struct Cyc__Bigint**
_freelist; int _cvtlen; unsigned char* _cvtbuf; union Cyc__reent_union _new;
struct Cyc__atexit* _atexit; struct Cyc__atexit _atexit0; void(** _sig_func)(
int); struct Cyc__glue __sglue; struct Cyc___sFILE __sf[ 3u]; } ; extern struct
Cyc__reent* _impure_ptr; extern void _reclaim_reent( struct Cyc__reent*); struct
Cyc_Stdlib__Div{ int quot; int rem; } ; struct Cyc_Stdlib__Ldiv{ int quot; int
rem; } ; extern int __mb_cur_max; extern int abs( int); extern int atexit( void(*
__func)()); extern struct Cyc_Stdlib__Div div( int __numer, int __denom); extern
struct Cyc_Stdlib__Ldiv ldiv( int __numer, int __denom); extern int rand();
extern void srand( unsigned int __seed); extern int rand_r( unsigned int* __seed);
extern struct _tagged_string Cyc_Stdlib_getenv( struct _tagged_string); extern
struct _tagged_string Cyc_Getopt_optarg; extern int Cyc_Getopt_optind; extern
int Cyc_Getopt_opterr; extern int Cyc_Getopt_optopt; struct Cyc_Getopt_option{
struct _tagged_string name; int has_arg; int* flag; int val; } ; extern int Cyc_Getopt_getopt(
int argc, struct _tagged_ptr0 argv, struct _tagged_string shortopts); struct
_tagged_ptr1{ struct Cyc_Getopt_option* curr; struct Cyc_Getopt_option* base;
struct Cyc_Getopt_option* last_plus_one; } ; extern int Cyc_Getopt__getopt_internal(
int argc, struct _tagged_ptr0 argv, struct _tagged_string shortopts, struct
_tagged_ptr1 longopts, int* longind, int long_only); struct _tagged_string Cyc_Getopt_optarg={
0, 0, 0}; int Cyc_Getopt_optind= 0; static struct _tagged_string Cyc_Getopt_nextchar;
int Cyc_Getopt_opterr= 1; int Cyc_Getopt_optopt=( int)'?'; enum  Cyc_Getopt_ordering_tag{
Cyc_Getopt_REQUIRE_ORDER  =  0u, Cyc_Getopt_PERMUTE  =  1u, Cyc_Getopt_RETURN_IN_ORDER
 =  2u}; static enum  Cyc_Getopt_ordering_tag Cyc_Getopt_ordering; static struct
_tagged_string Cyc_Getopt_posixly_correct; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u];
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern int
Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2); extern
int Cyc_String_strncmp( struct _tagged_string s1, struct _tagged_string s2,
unsigned int len); extern struct _tagged_string Cyc_String_strchr( struct
_tagged_string s, unsigned char c); static int Cyc_Getopt_first_nonopt; static
int Cyc_Getopt_last_nonopt; static struct _tagged_string Cyc_Getopt_nonoption_flags;
static int Cyc_Getopt_nonoption_flags_len; static void Cyc_Getopt_exchange(
struct _tagged_ptr0); static void Cyc_Getopt_exchange( struct _tagged_ptr0 argv){
int bottom= Cyc_Getopt_first_nonopt; int middle= Cyc_Getopt_last_nonopt; int top=
Cyc_Getopt_optind; struct _tagged_string tem; while( top > middle? middle >
bottom: 0) { if( top - middle > middle - bottom){ int len= middle - bottom; int
i; for( i= 0; i < len; i ++){ tem=*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), bottom + i);*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), bottom + i)=*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string),( top -( middle - bottom)) + i);*(( struct
_tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string),( top -( middle
- bottom)) + i)= tem;} top -= len;} else{ int len= top - middle; int i; for( i=
0; i < len; i ++){ tem=*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), bottom + i);*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), bottom + i)=*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), middle + i);*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), middle + i)= tem;} bottom += len;}} Cyc_Getopt_first_nonopt +=
Cyc_Getopt_optind - Cyc_Getopt_last_nonopt; Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;}
static struct _tagged_string Cyc_Getopt__getopt_initialize( struct
_tagged_string); static struct _tagged_string Cyc_Getopt__getopt_initialize(
struct _tagged_string optstring){ Cyc_Getopt_first_nonopt=( Cyc_Getopt_last_nonopt=(
Cyc_Getopt_optind= 1)); Cyc_Getopt_nextchar=({ struct _tagged_string _temp0={ 0,
0, 0}; _temp0;}); Cyc_Getopt_posixly_correct= Cyc_Stdlib_getenv(({ unsigned char*
_temp1=( unsigned char*)"POSIXLY_CORRECT"; struct _tagged_string _temp2; _temp2.curr=
_temp1; _temp2.base= _temp1; _temp2.last_plus_one= _temp1 + 16; _temp2;})); if(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( optstring, sizeof( unsigned char), 0) =='-'){ Cyc_Getopt_ordering=
Cyc_Getopt_RETURN_IN_ORDER;({ struct _tagged_string* _temp3=& optstring; ++
_temp3->curr;* _temp3;});} else{ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
optstring, sizeof( unsigned char), 0) =='+'){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;({
struct _tagged_string* _temp4=& optstring; ++ _temp4->curr;* _temp4;});} else{
if( Cyc_Getopt_posixly_correct.curr !=({ struct _tagged_string _temp5={ 0, 0, 0};
_temp5;}).curr){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;} else{ Cyc_Getopt_ordering=
Cyc_Getopt_PERMUTE;}}} if( Cyc_Getopt_posixly_correct.curr ==({ struct
_tagged_string _temp6={ 0, 0, 0}; _temp6;}).curr){ struct _tagged_string var;
var= xprintf("_%d_GNU_nonoption_argv_flags_", 10); Cyc_Getopt_nonoption_flags=
Cyc_Stdlib_getenv(( struct _tagged_string) var); if( Cyc_Getopt_nonoption_flags.curr
==({ struct _tagged_string _temp7={ 0, 0, 0}; _temp7;}).curr){ Cyc_Getopt_nonoption_flags_len=
0;} else{ Cyc_Getopt_nonoption_flags_len=( int) Cyc_String_strlen(( struct
_tagged_string) Cyc_Getopt_nonoption_flags);}} return optstring;} int Cyc_Getopt__getopt_internal(
int argc, struct _tagged_ptr0 argv, struct _tagged_string optstring, struct
_tagged_ptr1 longopts, int* longind, int long_only){ Cyc_Getopt_optarg=({ struct
_tagged_string _temp8={ 0, 0, 0}; _temp8;}); if( Cyc_Getopt_optind == 0){
optstring= Cyc_Getopt__getopt_initialize( optstring); Cyc_Getopt_optind= 1;} if(
Cyc_Getopt_nextchar.curr ==({ struct _tagged_string _temp9={ 0, 0, 0}; _temp9;}).curr?
1:({ struct _tagged_string _temp10= Cyc_Getopt_nextchar; unsigned char* _temp11=
_temp10.curr; if( _temp10.base == 0? 1:( _temp11 < _temp10.base? 1: _temp11 >=
_temp10.last_plus_one)){ _throw( Cyc_Null_Exception);}* _temp11;}) =='\000'){
if( Cyc_Getopt_last_nonopt > Cyc_Getopt_optind){ Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;}
if( Cyc_Getopt_first_nonopt > Cyc_Getopt_optind){ Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}
if( Cyc_Getopt_ordering == Cyc_Getopt_PERMUTE){ if( Cyc_Getopt_first_nonopt !=
Cyc_Getopt_last_nonopt? Cyc_Getopt_last_nonopt != Cyc_Getopt_optind: 0){ Cyc_Getopt_exchange((
struct _tagged_ptr0) argv);} else{ if( Cyc_Getopt_last_nonopt != Cyc_Getopt_optind){
Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}} while( Cyc_Getopt_optind < argc?(*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 0) !='-'? 1:*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 1) =='\000')? 1:(
Cyc_Getopt_optind < Cyc_Getopt_nonoption_flags_len?*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Getopt_nonoption_flags,
sizeof( unsigned char), Cyc_Getopt_optind) =='1': 0): 0) { Cyc_Getopt_optind ++;}
Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;} if( Cyc_Getopt_optind != argc? ! Cyc_String_strcmp((
struct _tagged_string)*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind),({ unsigned char* _temp12=( unsigned char*)"--";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 3; _temp13;})): 0){ Cyc_Getopt_optind ++; if(
Cyc_Getopt_first_nonopt != Cyc_Getopt_last_nonopt? Cyc_Getopt_last_nonopt != Cyc_Getopt_optind:
0){ Cyc_Getopt_exchange(( struct _tagged_ptr0) argv);} else{ if( Cyc_Getopt_first_nonopt
== Cyc_Getopt_last_nonopt){ Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}} Cyc_Getopt_last_nonopt=
argc; Cyc_Getopt_optind= argc;} if( Cyc_Getopt_optind == argc){ if( Cyc_Getopt_first_nonopt
!= Cyc_Getopt_last_nonopt){ Cyc_Getopt_optind= Cyc_Getopt_first_nonopt;} return
- 1;} if((*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)(*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 0)
!='-'? 1:*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)(*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 1)
=='\000')? 1:( Cyc_Getopt_optind < Cyc_Getopt_nonoption_flags_len?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Getopt_nonoption_flags, sizeof( unsigned char),
Cyc_Getopt_optind) =='1': 0)){ if( Cyc_Getopt_ordering == Cyc_Getopt_REQUIRE_ORDER){
return - 1;} Cyc_Getopt_optarg=*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), Cyc_Getopt_optind ++); return 1;} Cyc_Getopt_nextchar=({
struct _tagged_string _temp16=({ struct _tagged_string _temp14=*(( struct
_tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string), Cyc_Getopt_optind);
_temp14.curr += 1; _temp14;}); _temp16.curr +=( longopts.curr !=(( struct
_tagged_ptr1)({ struct _tagged_ptr1 _temp15={ 0, 0, 0}; _temp15;})).curr?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 1) =='-': 0)? 1: 0;
_temp16;});} if( longopts.curr !=(( struct _tagged_ptr1)({ struct _tagged_ptr1
_temp17={ 0, 0, 0}; _temp17;})).curr?*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(*((
struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string), Cyc_Getopt_optind),
sizeof( unsigned char), 1) =='-'? 1:( long_only?( int)*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(*((
struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string), Cyc_Getopt_optind),
sizeof( unsigned char), 2)? 1: !(( unsigned int)( Cyc_String_strchr( optstring,*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 1))).curr): 0): 0){
struct _tagged_string nameend; struct _tagged_ptr1 p; struct _tagged_ptr1 pfound=(
struct _tagged_ptr1)({ struct _tagged_ptr1 _temp51={ 0, 0, 0}; _temp51;}); int
exact= 0; int ambig= 0; int indfound= 0; int option_index; for( nameend= Cyc_Getopt_nextchar;(
int)({ struct _tagged_string _temp18= nameend; unsigned char* _temp19= _temp18.curr;
if( _temp18.base == 0? 1:( _temp19 < _temp18.base? 1: _temp19 >= _temp18.last_plus_one)){
_throw( Cyc_Null_Exception);}* _temp19;})?({ struct _tagged_string _temp20=
nameend; unsigned char* _temp21= _temp20.curr; if( _temp20.base == 0? 1:(
_temp21 < _temp20.base? 1: _temp21 >= _temp20.last_plus_one)){ _throw( Cyc_Null_Exception);}*
_temp21;}) !='=': 0;({ struct _tagged_string* _temp22=& nameend; struct
_tagged_string _temp23=* _temp22; _temp22->curr ++; _temp23;})){;} for(( p=
longopts, option_index= 0);( unsigned int)(((( const struct Cyc_Getopt_option*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( p,
sizeof( struct Cyc_Getopt_option), 0u))->name).curr;(({ struct _tagged_ptr1*
_temp24=& p; struct _tagged_ptr1 _temp25=* _temp24; _temp24->curr ++; _temp25;}),
option_index ++)){ if( ! Cyc_String_strncmp(((( const struct Cyc_Getopt_option*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( p,
sizeof( struct Cyc_Getopt_option), 0u))->name,( struct _tagged_string) Cyc_Getopt_nextchar,(
unsigned int)( nameend.curr - Cyc_Getopt_nextchar.curr))){ if( nameend.curr -
Cyc_Getopt_nextchar.curr == Cyc_String_strlen(((( const struct Cyc_Getopt_option*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( p,
sizeof( struct Cyc_Getopt_option), 0u))->name)){ pfound= p; indfound=
option_index; exact= 1; break;} else{ if( pfound.curr ==(( struct _tagged_ptr1)({
struct _tagged_ptr1 _temp26={ 0, 0, 0}; _temp26;})).curr){ pfound= p; indfound=
option_index;} else{ ambig= 1;}}}} if( ambig? ! exact: 0){ if( Cyc_Getopt_opterr){({
struct _tagged_string _temp27=( struct _tagged_string)*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), 0); struct _tagged_string _temp28=( struct
_tagged_string)*(( struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( argv, sizeof( struct _tagged_string),
Cyc_Getopt_optind); fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' is ambiguous\n",
_temp27.last_plus_one - _temp27.curr, _temp27.curr, _temp28.last_plus_one -
_temp28.curr, _temp28.curr);});} Cyc_Getopt_nextchar.curr +=( int) Cyc_String_strlen((
struct _tagged_string) Cyc_Getopt_nextchar); Cyc_Getopt_optind ++; Cyc_Getopt_optopt=
0; return( int)'?';} if( pfound.curr !=(( struct _tagged_ptr1)({ struct
_tagged_ptr1 _temp29={ 0, 0, 0}; _temp29;})).curr){ option_index= indfound; Cyc_Getopt_optind
++; if(( int)({ struct _tagged_string _temp30= nameend; unsigned char* _temp31=
_temp30.curr; if( _temp30.base == 0? 1:( _temp31 < _temp30.base? 1: _temp31 >=
_temp30.last_plus_one)){ _throw( Cyc_Null_Exception);}* _temp31;})){ if((((
const struct Cyc_Getopt_option*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( pfound, sizeof( struct Cyc_Getopt_option),
0u))->has_arg){ Cyc_Getopt_optarg=({ struct _tagged_string _temp32= nameend;
_temp32.curr += 1; _temp32;});} else{ if( Cyc_Getopt_opterr){ if(*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind - 1), sizeof( unsigned char), 1) =='-'){({
struct _tagged_string _temp33=( struct _tagged_string)*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), 0); struct _tagged_string _temp34=(((
const struct Cyc_Getopt_option*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( pfound, sizeof( struct Cyc_Getopt_option),
0u))->name; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `--%.*s' doesn't allow an argument\n",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr);});} else{({ struct _tagged_string _temp35=( struct
_tagged_string)*(( struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( argv, sizeof( struct _tagged_string),
0); int _temp36=( int)*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)(*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), Cyc_Getopt_optind - 1), sizeof( unsigned char),
0); struct _tagged_string _temp37=((( const struct Cyc_Getopt_option*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( pfound,
sizeof( struct Cyc_Getopt_option), 0u))->name; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%c%.*s' doesn't allow an argument\n",
_temp35.last_plus_one - _temp35.curr, _temp35.curr, _temp36, _temp37.last_plus_one
- _temp37.curr, _temp37.curr);});}} Cyc_Getopt_nextchar.curr +=( int) Cyc_String_strlen((
struct _tagged_string) Cyc_Getopt_nextchar); Cyc_Getopt_optopt=((( const struct
Cyc_Getopt_option*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)'?';}} else{ if(((( const struct Cyc_Getopt_option*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( pfound,
sizeof( struct Cyc_Getopt_option), 0u))->has_arg == 1){ if( Cyc_Getopt_optind <
argc){ Cyc_Getopt_optarg=*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind ++);} else{ if( Cyc_Getopt_opterr){({ struct
_tagged_string _temp38=( struct _tagged_string)*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), 0); struct _tagged_string _temp39=( struct
_tagged_string)*(( struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( argv, sizeof( struct _tagged_string),
Cyc_Getopt_optind - 1); fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' requires an argument\n",
_temp38.last_plus_one - _temp38.curr, _temp38.curr, _temp39.last_plus_one -
_temp39.curr, _temp39.curr);});} Cyc_Getopt_nextchar.curr +=( int) Cyc_String_strlen((
struct _tagged_string) Cyc_Getopt_nextchar); Cyc_Getopt_optopt=((( const struct
Cyc_Getopt_option*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( optstring, sizeof( unsigned char), 0)
==':'?':':'?');}}} Cyc_Getopt_nextchar.curr +=( int) Cyc_String_strlen(( struct
_tagged_string) Cyc_Getopt_nextchar); if( longind != 0){*(( int*) _check_null(
longind))= option_index;} if(( unsigned int)((( const struct Cyc_Getopt_option*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->flag){*(( int*) _check_null((((
const struct Cyc_Getopt_option*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( pfound, sizeof( struct Cyc_Getopt_option),
0u))->flag))=((( const struct Cyc_Getopt_option*(*)( struct _tagged_ptr1,
unsigned int, unsigned int)) _check_unknown_subscript)( pfound, sizeof( struct
Cyc_Getopt_option), 0u))->val; return 0;} return((( const struct Cyc_Getopt_option*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;} if(( ! long_only? 1:*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 1) =='-')? 1:( Cyc_String_strchr(
optstring,({ struct _tagged_string _temp40= Cyc_Getopt_nextchar; unsigned char*
_temp41= _temp40.curr; if( _temp40.base == 0? 1:( _temp41 < _temp40.base? 1:
_temp41 >= _temp40.last_plus_one)){ _throw( Cyc_Null_Exception);}* _temp41;}))).curr
==(( struct _tagged_string)({ struct _tagged_string _temp42={ 0, 0, 0}; _temp42;})).curr){
if( Cyc_Getopt_opterr){ if(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)(*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), Cyc_Getopt_optind), sizeof( unsigned char),
1) =='-'){({ struct _tagged_string _temp43=( struct _tagged_string)*(( struct
_tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string), 0); struct
_tagged_string _temp44=( struct _tagged_string) Cyc_Getopt_nextchar; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: unrecognized option `--%.*s'\n", _temp43.last_plus_one
- _temp43.curr, _temp43.curr, _temp44.last_plus_one - _temp44.curr, _temp44.curr);});}
else{({ struct _tagged_string _temp45=( struct _tagged_string)*(( struct
_tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( argv, sizeof( struct _tagged_string), 0); int _temp46=(
int)*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), Cyc_Getopt_optind), sizeof( unsigned char), 0); struct
_tagged_string _temp47=( struct _tagged_string) Cyc_Getopt_nextchar; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: unrecognized option `%c%.*s'\n", _temp45.last_plus_one
- _temp45.curr, _temp45.curr, _temp46, _temp47.last_plus_one - _temp47.curr,
_temp47.curr);});}} Cyc_Getopt_nextchar=({ unsigned char* _temp49=({
unsigned char* _temp48=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char));
_temp48[ 0]='\000'; _temp48;}); struct _tagged_string _temp50; _temp50.curr=
_temp49; _temp50.base= _temp49; _temp50.last_plus_one= _temp49 + 1; _temp50;});
Cyc_Getopt_optind ++; Cyc_Getopt_optopt= 0; return( int)'?';}}{ unsigned char c=({
struct _tagged_string _temp70=({ struct _tagged_string* _temp68=& Cyc_Getopt_nextchar;
struct _tagged_string _temp69=* _temp68; _temp68->curr ++; _temp69;});
unsigned char* _temp71= _temp70.curr; if( _temp70.base == 0? 1:( _temp71 <
_temp70.base? 1: _temp71 >= _temp70.last_plus_one)){ _throw( Cyc_Null_Exception);}*
_temp71;}); struct _tagged_string temp= Cyc_String_strchr( optstring, c); if(({
struct _tagged_string _temp52= Cyc_Getopt_nextchar; unsigned char* _temp53=
_temp52.curr; if( _temp52.base == 0? 1:( _temp53 < _temp52.base? 1: _temp53 >=
_temp52.last_plus_one)){ _throw( Cyc_Null_Exception);}* _temp53;}) =='\000'){ ++
Cyc_Getopt_optind;} if( temp.curr ==(( struct _tagged_string)({ struct
_tagged_string _temp54={ 0, 0, 0}; _temp54;})).curr? 1: c ==':'){ if( Cyc_Getopt_opterr){
if(( unsigned int) Cyc_Getopt_posixly_correct.curr){({ struct _tagged_string
_temp55=( struct _tagged_string)*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( argv,
sizeof( struct _tagged_string), 0); int _temp56=( int) c; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s: illegal option -- %c\n", _temp55.last_plus_one -
_temp55.curr, _temp55.curr, _temp56);});} else{({ struct _tagged_string _temp57=(
struct _tagged_string)*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( argv, sizeof( struct
_tagged_string), 0); int _temp58=( int) c; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: invalid option -- %c\n",
_temp57.last_plus_one - _temp57.curr, _temp57.curr, _temp58);});}} Cyc_Getopt_optopt=(
int) c; return( int)'?';} if(*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( temp, sizeof(
unsigned char), 1) ==':'){ if(*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( temp, sizeof(
unsigned char), 2) ==':'){ if(({ struct _tagged_string _temp59= Cyc_Getopt_nextchar;
unsigned char* _temp60= _temp59.curr; if( _temp59.base == 0? 1:( _temp60 <
_temp59.base? 1: _temp60 >= _temp59.last_plus_one)){ _throw( Cyc_Null_Exception);}*
_temp60;}) !='\000'){ Cyc_Getopt_optarg= Cyc_Getopt_nextchar; Cyc_Getopt_optind
++;} else{ Cyc_Getopt_optarg=({ struct _tagged_string _temp61={ 0, 0, 0};
_temp61;});} Cyc_Getopt_nextchar=({ struct _tagged_string _temp62={ 0, 0, 0};
_temp62;});} else{ if(({ struct _tagged_string _temp63= Cyc_Getopt_nextchar;
unsigned char* _temp64= _temp63.curr; if( _temp63.base == 0? 1:( _temp64 <
_temp63.base? 1: _temp64 >= _temp63.last_plus_one)){ _throw( Cyc_Null_Exception);}*
_temp64;}) !='\000'){ Cyc_Getopt_optarg= Cyc_Getopt_nextchar; Cyc_Getopt_optind
++;} else{ if( Cyc_Getopt_optind == argc){ if( Cyc_Getopt_opterr){({ struct
_tagged_string _temp65=( struct _tagged_string)*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), 0); int _temp66=( int) c; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: option requires an argument -- %c\n",
_temp65.last_plus_one - _temp65.curr, _temp65.curr, _temp66);});} Cyc_Getopt_optopt=(
int) c; if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( optstring, sizeof( unsigned char), 0)
==':'){ c=':';} else{ c='?';}} else{ Cyc_Getopt_optarg=*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
argv, sizeof( struct _tagged_string), Cyc_Getopt_optind ++);}} Cyc_Getopt_nextchar=({
struct _tagged_string _temp67={ 0, 0, 0}; _temp67;});}} return( int) c;}} int
Cyc_Getopt_getopt( int argc, struct _tagged_ptr0 argv, struct _tagged_string
optstring){ return Cyc_Getopt__getopt_internal( argc, argv, optstring,( struct
_tagged_ptr1)({ struct _tagged_ptr1 _temp72={ 0, 0, 0}; _temp72;}),( int*) 0, 0);}