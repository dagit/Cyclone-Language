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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Time_tm{ int tm_sec; int tm_min; int tm_hour;
int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int tm_isdst; }
; extern unsigned int time( unsigned int* t); struct Cyc___sFILE; struct Cyc__reent;
struct Cyc__glue{ struct Cyc__glue* _next; int _niobs; struct Cyc___sFILE* _iobs;
} ; struct Cyc__Bigint{ struct Cyc__Bigint* _next; int _k; int _maxwds; int
_sign; int _wds; unsigned int _x[ 1u]; } ; struct Cyc__atexit{ struct Cyc__atexit*
_next; int _ind; void(* _fns[ 32u])(); } ; struct Cyc___sbuf{ unsigned char*
_base; int _size; } ; struct Cyc___sFILE{ unsigned char* _p; int _r; int _w;
short _flags; short _file; struct Cyc___sbuf _bf; int _lbfsize; void* _cookie;
int(* _read)( void* _cookie, unsigned char* _buf, int _n)  __attribute__(( cdecl
)) ; int(* _write)( void* _cookie, const unsigned char* _buf, int _n)
 __attribute__(( cdecl )) ; int(* _seek)( void* _cookie, int _offset, int
_whence)  __attribute__(( cdecl )) ; int(* _close)( void* _cookie)
 __attribute__(( cdecl )) ; struct Cyc___sbuf _ub; unsigned char* _up; int _ur;
unsigned char _ubuf[ 3u]; unsigned char _nbuf[ 1u]; struct Cyc___sbuf _lb; int
_blksize; int _offset; struct Cyc__reent* _data; } ; struct Cyc__reent_u1{
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
struct Cyc__reent*); struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct Cyc_Stdlib__Ldiv{
int quot; int rem; } ; extern int __mb_cur_max  __attribute__(( dllimport )) ;
extern int abs( int)  __attribute__(( cdecl )) ; extern int atexit( void(*
__func)())  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Div div( int
__numer, int __denom)  __attribute__(( cdecl )) ; extern struct Cyc_Stdlib__Ldiv
ldiv( int __numer, int __denom)  __attribute__(( cdecl )) ; extern int rand()
 __attribute__(( cdecl )) ; extern void srand( unsigned int __seed)
 __attribute__(( cdecl )) ; extern int rand_r( unsigned int* __seed)
 __attribute__(( cdecl )) ; extern int random()  __attribute__(( cdecl )) ;
extern int srandom( unsigned int __seed)  __attribute__(( cdecl )) ; extern int
grantpt( int)  __attribute__(( cdecl )) ; extern int unlockpt( int)
 __attribute__(( cdecl )) ; extern struct _tagged_arr Cyc_Stdlib_getenv( struct
_tagged_arr); extern struct _tagged_arr Cyc_Getopt_optarg; extern int Cyc_Getopt_optind;
extern int Cyc_Getopt_opterr; extern int Cyc_Getopt_optopt; struct Cyc_Getopt_option{
struct _tagged_arr name; int has_arg; int* flag; int val; } ; extern int Cyc_Getopt_getopt(
int argc, struct _tagged_arr argv, struct _tagged_arr shortopts); extern int Cyc_Getopt__getopt_internal(
int argc, struct _tagged_arr argv, struct _tagged_arr shortopts, struct
_tagged_arr longopts, int* longind, int long_only); struct _tagged_arr Cyc_Getopt_optarg={(
void*) 0u,( void*) 0u,( void*) 0u + 0u}; int Cyc_Getopt_optind= 0; static struct
_tagged_arr Cyc_Getopt_nextchar; int Cyc_Getopt_opterr= 1; int Cyc_Getopt_optopt=(
int)'?'; enum  Cyc_Getopt_ordering_tag{ Cyc_Getopt_REQUIRE_ORDER  =  0u, Cyc_Getopt_PERMUTE
 =  1u, Cyc_Getopt_RETURN_IN_ORDER  =  2u}; static enum  Cyc_Getopt_ordering_tag
Cyc_Getopt_ordering; static struct _tagged_arr Cyc_Getopt_posixly_correct;
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern int Cyc_String_strncmp( struct _tagged_arr s1,
struct _tagged_arr s2, int len); extern struct _tagged_arr Cyc_String_strchr(
struct _tagged_arr s, unsigned char c); static int Cyc_Getopt_first_nonopt;
static int Cyc_Getopt_last_nonopt; static struct _tagged_arr Cyc_Getopt_nonoption_flags;
static int Cyc_Getopt_nonoption_flags_len; static void Cyc_Getopt_exchange(
struct _tagged_arr); static void Cyc_Getopt_exchange( struct _tagged_arr argv){
int bottom= Cyc_Getopt_first_nonopt; int middle= Cyc_Getopt_last_nonopt; int top=
Cyc_Getopt_optind; struct _tagged_arr tem; while( top > middle? middle > bottom:
0) { if( top - middle > middle - bottom){ int len= middle - bottom; int i; for(
i= 0; i < len; i ++){ tem=*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), bottom + i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), bottom + i))=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
top -( middle - bottom)) + i));*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr),( top -( middle - bottom)) + i))= tem;} top -=
len;} else{ int len= top - middle; int i; for( i= 0; i < len; i ++){ tem=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
bottom + i));*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), bottom + i))=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), middle + i));*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
middle + i))= tem;} bottom += len;}} Cyc_Getopt_first_nonopt += Cyc_Getopt_optind
- Cyc_Getopt_last_nonopt; Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;} static
struct _tagged_arr Cyc_Getopt__getopt_initialize( struct _tagged_arr); static
struct _tagged_arr Cyc_Getopt__getopt_initialize( struct _tagged_arr optstring){
Cyc_Getopt_first_nonopt=( Cyc_Getopt_last_nonopt=( Cyc_Getopt_optind= 1)); Cyc_Getopt_nextchar=
_tag_arr( 0u, 0u, 0u); Cyc_Getopt_posixly_correct= Cyc_Stdlib_getenv( _tag_arr("POSIXLY_CORRECT",
sizeof( unsigned char), 16u)); if(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) =='-'){ Cyc_Getopt_ordering=
Cyc_Getopt_RETURN_IN_ORDER; _tagged_arr_inplace_plus(& optstring, sizeof(
unsigned char), 1);} else{ if(*(( const unsigned char*) _check_unknown_subscript(
optstring, sizeof( unsigned char), 0)) =='+'){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;
_tagged_arr_inplace_plus(& optstring, sizeof( unsigned char), 1);} else{ if( Cyc_Getopt_posixly_correct.curr
!=( _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;}
else{ Cyc_Getopt_ordering= Cyc_Getopt_PERMUTE;}}} if( Cyc_Getopt_posixly_correct.curr
==( _tag_arr( 0u, 0u, 0u)).curr){ struct _tagged_arr var; var=({ struct
_tagged_arr(* _temp0)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp4= _tag_arr("_%d_GNU_nonoption_argv_flags_", sizeof(
unsigned char), 30u); struct Cyc_Stdio_Int_pa_struct _temp5; _temp5.tag= Cyc_Stdio_Int_pa;
_temp5.f1=( int)(( unsigned int) 10);{ void* _temp3=( void*)& _temp5; void*
_temp1[ 1u]={ _temp3}; struct _tagged_arr _temp2={( void*) _temp1,( void*)
_temp1,( void*)( _temp1 + 1u)}; _temp0( _temp4, _temp2);}}); Cyc_Getopt_nonoption_flags=
Cyc_Stdlib_getenv(( struct _tagged_arr) var); if( Cyc_Getopt_nonoption_flags.curr
==( _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Getopt_nonoption_flags_len= 0;} else{ Cyc_Getopt_nonoption_flags_len=
Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nonoption_flags);}} return
optstring;} int Cyc_Getopt__getopt_internal( int argc, struct _tagged_arr argv,
struct _tagged_arr optstring, struct _tagged_arr longopts, int* longind, int
long_only){ Cyc_Getopt_optarg= _tag_arr( 0u, 0u, 0u); if( Cyc_Getopt_optind == 0){
optstring= Cyc_Getopt__getopt_initialize( optstring); Cyc_Getopt_optind= 1;} if(
Cyc_Getopt_nextchar.curr ==( _tag_arr( 0u, 0u, 0u)).curr? 1:*(( unsigned char*)
_check_unknown_subscript( Cyc_Getopt_nextchar, sizeof( unsigned char), 0u)) =='\000'){
if( Cyc_Getopt_last_nonopt > Cyc_Getopt_optind){ Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;}
if( Cyc_Getopt_first_nonopt > Cyc_Getopt_optind){ Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}
if( Cyc_Getopt_ordering == Cyc_Getopt_PERMUTE){ if( Cyc_Getopt_first_nonopt !=
Cyc_Getopt_last_nonopt? Cyc_Getopt_last_nonopt != Cyc_Getopt_optind: 0){ Cyc_Getopt_exchange((
struct _tagged_arr) argv);} else{ if( Cyc_Getopt_last_nonopt != Cyc_Getopt_optind){
Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}} while( Cyc_Getopt_optind < argc?(*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 0)) !='-'? 1:*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 1)) =='\000')?
1:( Cyc_Getopt_optind < Cyc_Getopt_nonoption_flags_len?*(( unsigned char*)
_check_unknown_subscript( Cyc_Getopt_nonoption_flags, sizeof( unsigned char),
Cyc_Getopt_optind)) =='1': 0): 0) { Cyc_Getopt_optind ++;} Cyc_Getopt_last_nonopt=
Cyc_Getopt_optind;} if( Cyc_Getopt_optind != argc? ! Cyc_String_strcmp(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), Cyc_Getopt_optind)), _tag_arr("--", sizeof( unsigned char),
3u)): 0){ Cyc_Getopt_optind ++; if( Cyc_Getopt_first_nonopt != Cyc_Getopt_last_nonopt?
Cyc_Getopt_last_nonopt != Cyc_Getopt_optind: 0){ Cyc_Getopt_exchange(( struct
_tagged_arr) argv);} else{ if( Cyc_Getopt_first_nonopt == Cyc_Getopt_last_nonopt){
Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}} Cyc_Getopt_last_nonopt= argc; Cyc_Getopt_optind=
argc;} if( Cyc_Getopt_optind == argc){ if( Cyc_Getopt_first_nonopt != Cyc_Getopt_last_nonopt){
Cyc_Getopt_optind= Cyc_Getopt_first_nonopt;} return - 1;} if((*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 0)) !='-'?
1:*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) =='\000')? 1:( Cyc_Getopt_optind < Cyc_Getopt_nonoption_flags_len?*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nonoption_flags, sizeof(
unsigned char), Cyc_Getopt_optind)) =='1': 0)){ if( Cyc_Getopt_ordering == Cyc_Getopt_REQUIRE_ORDER){
return - 1;} Cyc_Getopt_optarg=*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind ++)); return 1;} Cyc_Getopt_nextchar=
_tagged_arr_plus( _tagged_arr_plus(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1), sizeof( unsigned char),( longopts.curr !=(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 1)) =='-':
0)? 1: 0);} if( longopts.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) =='-'? 1:( long_only?( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 2))? 1:
!(( unsigned int)( Cyc_String_strchr( optstring,*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 1)))).curr):
0): 0){ struct _tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr
pfound=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0;
int indfound= 0; int option_index; for( nameend= Cyc_Getopt_nextchar;( int)*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))?*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))
!='=': 0; _tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;}
for(( p= longopts, option_index= 0);( unsigned int)((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_Getopt_option), 1),
option_index ++)){ if( ! Cyc_String_strncmp((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name,(
struct _tagged_arr) Cyc_Getopt_nextchar,( nameend.curr - Cyc_Getopt_nextchar.curr)
/ sizeof( unsigned char))){ if(( nameend.curr - Cyc_Getopt_nextchar.curr) /
sizeof( unsigned char) == Cyc_String_strlen((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name)){
pfound= p; indfound= option_index; exact= 1; break;} else{ if( pfound.curr ==((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= p; indfound=
option_index;} else{ ambig= 1;}}}} if( ambig? ! exact: 0){ if( Cyc_Getopt_opterr){({
int(* _temp6)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp11= Cyc_Stdio_stderr;
struct _tagged_arr _temp12= _tag_arr("%s: option `%s' is ambiguous\n", sizeof(
unsigned char), 30u); struct Cyc_Stdio_String_pa_struct _temp14; _temp14.tag=
Cyc_Stdio_String_pa; _temp14.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp9=(
void*)& _temp14; struct Cyc_Stdio_String_pa_struct _temp13; _temp13.tag= Cyc_Stdio_String_pa;
_temp13.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind));{
void* _temp10=( void*)& _temp13; void* _temp7[ 2u]={ _temp9, _temp10}; struct
_tagged_arr _temp8={( void*) _temp7,( void*) _temp7,( void*)( _temp7 + 2u)};
_temp6( _temp11, _temp12, _temp8);}}});} _tagged_arr_inplace_plus(& Cyc_Getopt_nextchar,
sizeof( unsigned char), Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nextchar));
Cyc_Getopt_optind ++; Cyc_Getopt_optopt= 0; return( int)'?';} if( pfound.curr !=((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ option_index= indfound; Cyc_Getopt_optind
++; if(( int)*(( unsigned char*) _check_unknown_subscript( nameend, sizeof(
unsigned char), 0u))){ if((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->has_arg){
Cyc_Getopt_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char), 1);} else{
if( Cyc_Getopt_opterr){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Getopt_optind - 1)), sizeof( unsigned char), 1)) =='-'){({ int(* _temp15)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp20= Cyc_Stdio_stderr; struct _tagged_arr _temp21=
_tag_arr("%s: option `--%s' doesn't allow an argument\n", sizeof( unsigned char),
45u); struct Cyc_Stdio_String_pa_struct _temp23; _temp23.tag= Cyc_Stdio_String_pa;
_temp23.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp18=(
void*)& _temp23; struct Cyc_Stdio_String_pa_struct _temp22; _temp22.tag= Cyc_Stdio_String_pa;
_temp22.f1=( struct _tagged_arr)(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->name;{
void* _temp19=( void*)& _temp22; void* _temp16[ 2u]={ _temp18, _temp19}; struct
_tagged_arr _temp17={( void*) _temp16,( void*) _temp16,( void*)( _temp16 + 2u)};
_temp15( _temp20, _temp21, _temp17);}}});} else{({ int(* _temp24)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp30= Cyc_Stdio_stderr; struct _tagged_arr _temp31= _tag_arr("%s: option `%c%s' doesn't allow an argument\n",
sizeof( unsigned char), 45u); struct Cyc_Stdio_String_pa_struct _temp34; _temp34.tag=
Cyc_Stdio_String_pa; _temp34.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp27=(
void*)& _temp34; struct Cyc_Stdio_Int_pa_struct _temp33; _temp33.tag= Cyc_Stdio_Int_pa;
_temp33.f1=( int)(( unsigned int)(( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind - 1)), sizeof( unsigned char), 0))));{
void* _temp28=( void*)& _temp33; struct Cyc_Stdio_String_pa_struct _temp32;
_temp32.tag= Cyc_Stdio_String_pa; _temp32.f1=( struct _tagged_arr)(( const
struct Cyc_Getopt_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option),
0u))->name;{ void* _temp29=( void*)& _temp32; void* _temp25[ 3u]={ _temp27,
_temp28, _temp29}; struct _tagged_arr _temp26={( void*) _temp25,( void*) _temp25,(
void*)( _temp25 + 3u)}; _temp24( _temp30, _temp31, _temp26);}}}});}}
_tagged_arr_inplace_plus(& Cyc_Getopt_nextchar, sizeof( unsigned char), Cyc_String_strlen((
struct _tagged_arr) Cyc_Getopt_nextchar)); Cyc_Getopt_optopt=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)'?';}} else{ if((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->has_arg
== 1){ if( Cyc_Getopt_optind < argc){ Cyc_Getopt_optarg=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind
++));} else{ if( Cyc_Getopt_opterr){({ int(* _temp35)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp40= Cyc_Stdio_stderr; struct _tagged_arr _temp41= _tag_arr("%s: option `%s' requires an argument\n",
sizeof( unsigned char), 38u); struct Cyc_Stdio_String_pa_struct _temp43; _temp43.tag=
Cyc_Stdio_String_pa; _temp43.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp38=(
void*)& _temp43; struct Cyc_Stdio_String_pa_struct _temp42; _temp42.tag= Cyc_Stdio_String_pa;
_temp42.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind -
1));{ void* _temp39=( void*)& _temp42; void* _temp36[ 2u]={ _temp38, _temp39};
struct _tagged_arr _temp37={( void*) _temp36,( void*) _temp36,( void*)( _temp36
+ 2u)}; _temp35( _temp40, _temp41, _temp37);}}});} _tagged_arr_inplace_plus(&
Cyc_Getopt_nextchar, sizeof( unsigned char), Cyc_String_strlen(( struct
_tagged_arr) Cyc_Getopt_nextchar)); Cyc_Getopt_optopt=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) ==':'?':':'?');}}} _tagged_arr_inplace_plus(& Cyc_Getopt_nextchar,
sizeof( unsigned char), Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nextchar));
if( longind != 0){*(( int*) _check_null( longind))= option_index;} if((
unsigned int)(( const struct Cyc_Getopt_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->flag){*(( int*) _check_null(((
const struct Cyc_Getopt_option*) _check_unknown_subscript( pfound, sizeof(
struct Cyc_Getopt_option), 0u))->flag))=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return 0;} return(( const struct Cyc_Getopt_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;} if(( ! long_only? 1:*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) =='-')? 1:( Cyc_String_strchr( optstring,*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nextchar, sizeof(
unsigned char), 0u)))).curr ==(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( Cyc_Getopt_opterr){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Getopt_optind)), sizeof( unsigned char), 1)) =='-'){({ int(* _temp44)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp49= Cyc_Stdio_stderr; struct _tagged_arr _temp50=
_tag_arr("%s: unrecognized option `--%s'\n", sizeof( unsigned char), 32u);
struct Cyc_Stdio_String_pa_struct _temp52; _temp52.tag= Cyc_Stdio_String_pa;
_temp52.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp47=(
void*)& _temp52; struct Cyc_Stdio_String_pa_struct _temp51; _temp51.tag= Cyc_Stdio_String_pa;
_temp51.f1=( struct _tagged_arr) Cyc_Getopt_nextchar;{ void* _temp48=( void*)&
_temp51; void* _temp45[ 2u]={ _temp47, _temp48}; struct _tagged_arr _temp46={(
void*) _temp45,( void*) _temp45,( void*)( _temp45 + 2u)}; _temp44( _temp49,
_temp50, _temp46);}}});} else{({ int(* _temp53)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp59= Cyc_Stdio_stderr; struct _tagged_arr _temp60= _tag_arr("%s: unrecognized option `%c%s'\n",
sizeof( unsigned char), 32u); struct Cyc_Stdio_String_pa_struct _temp63; _temp63.tag=
Cyc_Stdio_String_pa; _temp63.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp56=(
void*)& _temp63; struct Cyc_Stdio_Int_pa_struct _temp62; _temp62.tag= Cyc_Stdio_Int_pa;
_temp62.f1=( int)(( unsigned int)(( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 0))));{
void* _temp57=( void*)& _temp62; struct Cyc_Stdio_String_pa_struct _temp61;
_temp61.tag= Cyc_Stdio_String_pa; _temp61.f1=( struct _tagged_arr) Cyc_Getopt_nextchar;{
void* _temp58=( void*)& _temp61; void* _temp54[ 3u]={ _temp56, _temp57, _temp58};
struct _tagged_arr _temp55={( void*) _temp54,( void*) _temp54,( void*)( _temp54
+ 3u)}; _temp53( _temp59, _temp60, _temp55);}}}});}} Cyc_Getopt_nextchar=
_tag_arr(({ unsigned char* _temp64=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char)); _temp64[ 0]='\000'; _temp64;}), sizeof( unsigned char), 1u);
Cyc_Getopt_optind ++; Cyc_Getopt_optopt= 0; return( int)'?';}}{ unsigned char c=*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& Cyc_Getopt_nextchar,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u)); struct _tagged_arr
temp= Cyc_String_strchr( optstring, c); if(*(( unsigned char*)
_check_unknown_subscript( Cyc_Getopt_nextchar, sizeof( unsigned char), 0u)) =='\000'){
++ Cyc_Getopt_optind;} if( temp.curr ==(( struct _tagged_arr) _tag_arr( 0u, 0u,
0u)).curr? 1: c ==':'){ if( Cyc_Getopt_opterr){ if(( unsigned int) Cyc_Getopt_posixly_correct.curr){({
int(* _temp65)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp70= Cyc_Stdio_stderr;
struct _tagged_arr _temp71= _tag_arr("%s: illegal option -- %c\n", sizeof(
unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp73; _temp73.tag=
Cyc_Stdio_String_pa; _temp73.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp68=(
void*)& _temp73; struct Cyc_Stdio_Int_pa_struct _temp72; _temp72.tag= Cyc_Stdio_Int_pa;
_temp72.f1=( int)(( unsigned int)(( int) c));{ void* _temp69=( void*)& _temp72;
void* _temp66[ 2u]={ _temp68, _temp69}; struct _tagged_arr _temp67={( void*)
_temp66,( void*) _temp66,( void*)( _temp66 + 2u)}; _temp65( _temp70, _temp71,
_temp67);}}});} else{({ int(* _temp74)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp79= Cyc_Stdio_stderr; struct _tagged_arr _temp80= _tag_arr("%s: invalid option -- %c\n",
sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp82; _temp82.tag=
Cyc_Stdio_String_pa; _temp82.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp77=(
void*)& _temp82; struct Cyc_Stdio_Int_pa_struct _temp81; _temp81.tag= Cyc_Stdio_Int_pa;
_temp81.f1=( int)(( unsigned int)(( int) c));{ void* _temp78=( void*)& _temp81;
void* _temp75[ 2u]={ _temp77, _temp78}; struct _tagged_arr _temp76={( void*)
_temp75,( void*) _temp75,( void*)( _temp75 + 2u)}; _temp74( _temp79, _temp80,
_temp76);}}});}} Cyc_Getopt_optopt=( int) c; return( int)'?';} if(*(( const
unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char), 1)) ==':'){
if(*(( const unsigned char*) _check_unknown_subscript( temp, sizeof(
unsigned char), 2)) ==':'){ if(*(( unsigned char*) _check_unknown_subscript( Cyc_Getopt_nextchar,
sizeof( unsigned char), 0u)) !='\000'){ Cyc_Getopt_optarg= Cyc_Getopt_nextchar;
Cyc_Getopt_optind ++;} else{ Cyc_Getopt_optarg= _tag_arr( 0u, 0u, 0u);} Cyc_Getopt_nextchar=
_tag_arr( 0u, 0u, 0u);} else{ if(*(( unsigned char*) _check_unknown_subscript(
Cyc_Getopt_nextchar, sizeof( unsigned char), 0u)) !='\000'){ Cyc_Getopt_optarg=
Cyc_Getopt_nextchar; Cyc_Getopt_optind ++;} else{ if( Cyc_Getopt_optind == argc){
if( Cyc_Getopt_opterr){({ int(* _temp83)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp88= Cyc_Stdio_stderr; struct _tagged_arr _temp89= _tag_arr("%s: option requires an argument -- %c\n",
sizeof( unsigned char), 39u); struct Cyc_Stdio_String_pa_struct _temp91; _temp91.tag=
Cyc_Stdio_String_pa; _temp91.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp86=(
void*)& _temp91; struct Cyc_Stdio_Int_pa_struct _temp90; _temp90.tag= Cyc_Stdio_Int_pa;
_temp90.f1=( int)(( unsigned int)(( int) c));{ void* _temp87=( void*)& _temp90;
void* _temp84[ 2u]={ _temp86, _temp87}; struct _tagged_arr _temp85={( void*)
_temp84,( void*) _temp84,( void*)( _temp84 + 2u)}; _temp83( _temp88, _temp89,
_temp85);}}});} Cyc_Getopt_optopt=( int) c; if(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) ==':'){ c=':';}
else{ c='?';}} else{ Cyc_Getopt_optarg=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind
++));}} Cyc_Getopt_nextchar= _tag_arr( 0u, 0u, 0u);}} return( int) c;}} int Cyc_Getopt_getopt(
int argc, struct _tagged_arr argv, struct _tagged_arr optstring){ return Cyc_Getopt__getopt_internal(
argc, argv, optstring,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u),( int*) 0, 0);}
