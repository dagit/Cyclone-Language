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
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern int Cyc_Stdio_putc(
int, struct Cyc_Stdio___sFILE*); extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_sprintf( struct
_tagged_arr s, struct _tagged_arr fmt, struct _tagged_arr); extern struct
_tagged_arr Cyc_Stdio_aprintf( struct _tagged_arr fmt, struct _tagged_arr);
extern struct _tagged_arr Cyc_Stdio_raprintf( struct _RegionHandle*, struct
_tagged_arr fmt, struct _tagged_arr ap); extern int Cyc_Stdio_vfprintf( struct
Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr ap); extern int
Cyc_Stdio_vprintf( struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_vsprintf(
struct _tagged_arr s, struct _tagged_arr fmt, struct _tagged_arr); extern struct
_tagged_arr Cyc_Stdio_vraprintf( struct _RegionHandle* r1, struct _tagged_arr
fmt, struct _tagged_arr ap); static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int isalnum( int __c)  __attribute__(( cdecl )) ;
extern int isalpha( int __c)  __attribute__(( cdecl )) ; extern int iscntrl( int
__c)  __attribute__(( cdecl )) ; extern int isdigit( int __c)  __attribute__((
cdecl )) ; extern int isgraph( int __c)  __attribute__(( cdecl )) ; extern int
islower( int __c)  __attribute__(( cdecl )) ; extern int isprint( int __c)
 __attribute__(( cdecl )) ; extern int ispunct( int __c)  __attribute__(( cdecl
)) ; extern int isspace( int __c)  __attribute__(( cdecl )) ; extern int isupper(
int __c)  __attribute__(( cdecl )) ; extern int isxdigit( int __c)
 __attribute__(( cdecl )) ; extern int tolower( int __c)  __attribute__(( cdecl
)) ; extern int toupper( int __c)  __attribute__(( cdecl )) ; extern int isascii(
int __c)  __attribute__(( cdecl )) ; extern int toascii( int __c)
 __attribute__(( cdecl )) ; extern int _tolower( int __c)  __attribute__(( cdecl
)) ; extern int _toupper( int __c)  __attribute__(( cdecl )) ; struct Cyc_Time_tm{
int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern unsigned int time( unsigned int*
t); struct Cyc___sFILE; struct Cyc__reent; struct Cyc__glue{ struct Cyc__glue*
_next; int _niobs; struct Cyc___sFILE* _iobs; } ; struct Cyc__Bigint{ struct Cyc__Bigint*
_next; int _k; int _maxwds; int _sign; int _wds; unsigned int _x[ 1u]; } ;
struct Cyc__atexit{ struct Cyc__atexit* _next; int _ind; void(* _fns[ 32u])(); }
; struct Cyc___sbuf{ unsigned char* _base; int _size; } ; struct Cyc___sFILE{
unsigned char* _p; int _r; int _w; short _flags; short _file; struct Cyc___sbuf
_bf; int _lbfsize; void* _cookie; int(* _read)( void* _cookie, unsigned char*
_buf, int _n)  __attribute__(( cdecl )) ; int(* _write)( void* _cookie, const
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
 __attribute__(( cdecl )) ; extern double atan( double); extern double cos(
double); extern double sin( double); extern double tan( double); extern double
tanh( double); extern double frexp( double, int*); extern double modf( double,
double*); extern double ceil( double); extern double fabs( double); extern
double floor( double); extern double acos( double); extern double asin( double);
extern double atan2( double, double); extern double cosh( double); extern double
sinh( double); extern double exp( double); extern double ldexp( double, int);
extern double log( double); extern double log10( double); extern double pow(
double, double); extern double sqrt( double); extern double fmod( double, double);
extern double infinity(); extern double nan(); extern int isnan( double); extern
int isinf( double); extern int finite( double); extern double copysign( double,
double); extern int ilogb( double); extern double asinh( double); extern double
cbrt( double); extern double nextafter( double, double); extern double rint(
double); extern double scalbn( double, int); extern double log1p( double);
extern double expm1( double); extern double acosh( double); extern double atanh(
double); extern double remainder( double, double); extern double gamma( double);
extern double gamma_r( double, int*); extern double lgamma( double); extern
double lgamma_r( double, int*); extern double erf( double); extern double erfc(
double); extern double y0( double); extern double y1( double); extern double yn(
int, double); extern double j0( double); extern double j1( double); extern
double jn( int, double); extern double hypot( double, double); extern double
cabs(); extern double drem( double, double); extern float atanf( float); extern
float cosf( float); extern float sinf( float); extern float tanf( float); extern
float tanhf( float); extern float frexpf( float, int*); extern float modff(
float, float*); extern float ceilf( float); extern float fabsf( float); extern
float floorf( float); extern float acosf( float); extern float asinf( float);
extern float atan2f( float, float); extern float coshf( float); extern float
sinhf( float); extern float expf( float); extern float ldexpf( float, int);
extern float logf( float); extern float log10f( float); extern float powf( float,
float); extern float sqrtf( float); extern float fmodf( float, float); extern
float infinityf(); extern float nanf(); extern int isnanf( float); extern int
isinff( float); extern int finitef( float); extern float copysignf( float, float);
extern int ilogbf( float); extern float asinhf( float); extern float cbrtf(
float); extern float nextafterf( float, float); extern float rintf( float);
extern float scalbnf( float, int); extern float log1pf( float); extern float
expm1f( float); extern float acoshf( float); extern float atanhf( float); extern
float remainderf( float, float); extern float gammaf( float); extern float
gammaf_r( float, int*); extern float lgammaf( float); extern float lgammaf_r(
float, int*); extern float erff( float); extern float erfcf( float); extern
float y0f( float); extern float y1f( float); extern float ynf( int, float);
extern float j0f( float); extern float j1f( float); extern float jnf( int, float);
extern float hypotf( float, float); extern float cabsf(); extern float dremf(
float, float); extern int signgam; struct Cyc_Math___exception{ int type;
unsigned char* name; double arg1; double arg2; double retval; int err; } ;
extern int matherr( struct Cyc_Math___exception* e); static struct _tagged_arr
Cyc_Stdio_parg2string( void* x){ void* _temp0= x; _LL2: if(*(( int*) _temp0) ==
Cyc_Stdio_String_pa){ goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0)
== Cyc_Stdio_Int_pa){ goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp0)
== Cyc_Stdio_Double_pa){ goto _LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp0)
== Cyc_Stdio_ShortPtr_pa){ goto _LL9;} else{ goto _LL10;} _LL10: if(*(( int*)
_temp0) == Cyc_Stdio_IntPtr_pa){ goto _LL11;} else{ goto _LL1;} _LL3: return
_tag_arr("string", sizeof( unsigned char), 7u); _LL5: return _tag_arr("int",
sizeof( unsigned char), 4u); _LL7: return _tag_arr("double", sizeof(
unsigned char), 7u); _LL9: return _tag_arr("short *", sizeof( unsigned char), 8u);
_LL11: return _tag_arr("unsigned long *", sizeof( unsigned char), 16u); _LL1:;}
static void* Cyc_Stdio_badarg( struct _tagged_arr s){({ struct Cyc_Stdio_String_pa_struct
_temp13; _temp13.tag= Cyc_Stdio_String_pa; _temp13.f1=( struct _tagged_arr) s;{
void* _temp12[ 1u]={& _temp13}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp12, sizeof( void*), 1u));}}); return(
void*) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp14=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp14[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp15; _temp15.tag= Cyc_Core_InvalidArg; _temp15.f1=
s; _temp15;}); _temp14;}));} static int Cyc_Stdio_va_arg_int( struct _tagged_arr
ap){ void* _temp16=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0u));
unsigned int _temp22; _LL18: if(*(( int*) _temp16) == Cyc_Stdio_Int_pa){ _LL23:
_temp22=(( struct Cyc_Stdio_Int_pa_struct*) _temp16)->f1; goto _LL19;} else{
goto _LL20;} _LL20: goto _LL21; _LL19: return( int) _temp22; _LL21: return(( int(*)(
struct _tagged_arr s)) Cyc_Stdio_badarg)( _tag_arr("printf expected int",
sizeof( unsigned char), 20u)); _LL17:;} static int Cyc_Stdio_va_arg_long( struct
_tagged_arr ap){ void* _temp24=*(( void**) _check_unknown_subscript( ap, sizeof(
void*), 0u)); unsigned int _temp30; _LL26: if(*(( int*) _temp24) == Cyc_Stdio_Int_pa){
_LL31: _temp30=(( struct Cyc_Stdio_Int_pa_struct*) _temp24)->f1; goto _LL27;}
else{ goto _LL28;} _LL28: goto _LL29; _LL27: return( int) _temp30; _LL29: return((
int(*)( struct _tagged_arr s)) Cyc_Stdio_badarg)( _tag_arr("printf expected int",
sizeof( unsigned char), 20u)); _LL25:;} static unsigned int Cyc_Stdio_va_arg_ulong(
struct _tagged_arr ap){ void* _temp32=*(( void**) _check_unknown_subscript( ap,
sizeof( void*), 0u)); unsigned int _temp38; _LL34: if(*(( int*) _temp32) == Cyc_Stdio_Int_pa){
_LL39: _temp38=(( struct Cyc_Stdio_Int_pa_struct*) _temp32)->f1; goto _LL35;}
else{ goto _LL36;} _LL36: goto _LL37; _LL35: return( unsigned int) _temp38;
_LL37: return(( unsigned int(*)( struct _tagged_arr s)) Cyc_Stdio_badarg)(
_tag_arr("printf expected int", sizeof( unsigned char), 20u)); _LL33:;} static
unsigned int Cyc_Stdio_va_arg_uint( struct _tagged_arr ap){ void* _temp40=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)); unsigned int _temp46;
_LL42: if(*(( int*) _temp40) == Cyc_Stdio_Int_pa){ _LL47: _temp46=(( struct Cyc_Stdio_Int_pa_struct*)
_temp40)->f1; goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL43: return(
unsigned int) _temp46; _LL45: return(( unsigned int(*)( struct _tagged_arr s))
Cyc_Stdio_badarg)( _tag_arr("printf expected int", sizeof( unsigned char), 20u));
_LL41:;} static double Cyc_Stdio_va_arg_double( struct _tagged_arr ap){ void*
_temp48=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0u)); double
_temp54; _LL50: if(*(( int*) _temp48) == Cyc_Stdio_Double_pa){ _LL55: _temp54=((
struct Cyc_Stdio_Double_pa_struct*) _temp48)->f1; goto _LL51;} else{ goto _LL52;}
_LL52: goto _LL53; _LL51: return _temp54; _LL53:( int) _throw( Cyc_Stdio_badarg((
struct _tagged_arr)({ struct Cyc_Stdio_String_pa_struct _temp57; _temp57.tag=
Cyc_Stdio_String_pa; _temp57.f1=( struct _tagged_arr) Cyc_Stdio_parg2string(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));{ void* _temp56[ 1u]={&
_temp57}; Cyc_Stdio_aprintf( _tag_arr("printf expected double but found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp56, sizeof( void*), 1u));}})));
_LL49:;} static short* Cyc_Stdio_va_arg_short_ptr( struct _tagged_arr ap){ void*
_temp58=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0u)); short*
_temp64; _LL60: if(*(( int*) _temp58) == Cyc_Stdio_ShortPtr_pa){ _LL65: _temp64=((
struct Cyc_Stdio_ShortPtr_pa_struct*) _temp58)->f1; goto _LL61;} else{ goto
_LL62;} _LL62: goto _LL63; _LL61: return _temp64; _LL63:( int) _throw( Cyc_Stdio_badarg(
_tag_arr("printf expected short pointer", sizeof( unsigned char), 30u))); _LL59:;}
static unsigned int* Cyc_Stdio_va_arg_int_ptr( struct _tagged_arr ap){ void*
_temp66=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0u));
unsigned int* _temp72; _LL68: if(*(( int*) _temp66) == Cyc_Stdio_IntPtr_pa){
_LL73: _temp72=(( struct Cyc_Stdio_IntPtr_pa_struct*) _temp66)->f1; goto _LL69;}
else{ goto _LL70;} _LL70: goto _LL71; _LL69: return _temp72; _LL71:( int) _throw(
Cyc_Stdio_badarg( _tag_arr("printf expected int pointer", sizeof( unsigned char),
28u))); _LL67:;} static struct _tagged_arr Cyc_Stdio_va_arg_string( struct
_tagged_arr ap){ void* _temp74=*(( void**) _check_unknown_subscript( ap, sizeof(
void*), 0u)); struct _tagged_arr _temp80; _LL76: if(*(( int*) _temp74) == Cyc_Stdio_String_pa){
_LL81: _temp80=(( struct Cyc_Stdio_String_pa_struct*) _temp74)->f1; goto _LL77;}
else{ goto _LL78;} _LL78: goto _LL79; _LL77: return _temp80; _LL79:( int) _throw(
Cyc_Stdio_badarg( _tag_arr("printf expected string", sizeof( unsigned char), 23u)));
_LL75:;} int Cyc_Stdio___cvt_double( double number, int prec, int flags, int*
signp, int fmtch, struct _tagged_arr startp, struct _tagged_arr endp); enum  Cyc_Stdio_BASE{
Cyc_Stdio_OCT  =  0u, Cyc_Stdio_DEC  =  1u, Cyc_Stdio_HEX  =  2u}; static int
Cyc_Stdio__IO_sputn( int(* ioputc)( int, void*), void* ioputc_env, struct
_tagged_arr s, int howmany){ int i= 0; while( i < howmany) { if( _get_arr_size(
s, sizeof( unsigned char)) == 0? 1: ioputc(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u)), ioputc_env) == - 1){
return i;} _tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1); i ++;}
return i;} static int Cyc_Stdio__IO_padn( int(* ioputc)( int, void*), void*
ioputc_env, unsigned char c, int howmany){ int i= 0; while( i < howmany) { if(
ioputc(( int) c, ioputc_env) == - 1){ return i;} i ++;} return i;} static struct
_tagged_arr Cyc_Stdio_memchr( struct _tagged_arr s, unsigned char c, int n){ int
sz=( int) _get_arr_size( s, sizeof( unsigned char)); n= n < sz? n: sz; for( 0; n
!= 0;( n --, _tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1))){
if(*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
0u)) == c){ return s;}} return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);} int
Cyc_Stdio__IO_vfprintf( int(* ioputc)( int, void*), void* ioputc_env, struct
_tagged_arr fmt0, struct _tagged_arr ap){ struct _tagged_arr fmt; int ch; int n;
struct _tagged_arr cp= _tag_arr( 0u, 0u, 0u); struct _tagged_arr cp2=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); int use_cp2; struct _tagged_arr fmark; int
flags; int ret; int width; int prec; unsigned char sign; int softsign= 0; double
_double; int fpprec; unsigned int _ulong; int dprec; int dpad; int fieldsz; int
size= 0; unsigned char buf[ 348u];{ unsigned int _temp130= 348u; unsigned int i;
for( i= 0; i < _temp130; i ++){ buf[ i]='\000';}}{ unsigned char ox[ 2u]={'\000','\000'};
enum  Cyc_Stdio_BASE base; fmt= fmt0; ret= 0; for( 0; 1; 0){ for( fmark= fmt;(
ch=( int)*(( const unsigned char*) _check_unknown_subscript( fmt, sizeof(
unsigned char), 0u))) !='\000'? ch !='%': 0; _tagged_arr_inplace_plus_post(& fmt,
sizeof( unsigned char), 1)){;} if(( n=( fmt.curr - fmark.curr) / sizeof(
unsigned char)) != 0){ do { if( Cyc_Stdio__IO_sputn( ioputc, ioputc_env, fmark,
n) != n){ goto error;}} while ( 0); ret += n;} if( ch =='\000'){ goto done;}
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1); flags= 0; dprec=
0; fpprec= 0; width= 0; prec= - 1; sign='\000'; rflag: ch=( int)*(( const
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u)); reswitch: use_cp2= 0;
switch( ch){ case ' ': _LL82: if( !(( int) sign)){ sign=' ';} goto rflag; case '#':
_LL83: flags |= 8; goto rflag; case '*': _LL84: width= Cyc_Stdio_va_arg_int( ap);
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); if( width >= 0){ goto
rflag;} width= - width; goto _LL85; case '-': _LL85: flags |= 16; flags &= ~ 32;
goto rflag; case '+': _LL86: sign='+'; goto rflag; case '.': _LL87: if(( ch=(
int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))) =='*'){ n= Cyc_Stdio_va_arg_int( ap);
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); prec= n < 0? - 1: n;
goto rflag;} n= 0; while(( unsigned int)( ch -'0') <= 9) { n= 10 * n +( ch -'0');
ch=( int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u));} prec= n < 0? - 1: n; goto reswitch; case '0': _LL88: if(
!( flags & 16)){ flags |= 32;} goto rflag; case '1': _LL89: goto _LL90; case '2':
_LL90: goto _LL91; case '3': _LL91: goto _LL92; case '4': _LL92: goto _LL93;
case '5': _LL93: goto _LL94; case '6': _LL94: goto _LL95; case '7': _LL95: goto
_LL96; case '8': _LL96: goto _LL97; case '9': _LL97: n= 0; do { n= 10 * n +( ch
-'0'); ch=( int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u));} while (( unsigned int)( ch -'0') <= 9); width= n; goto
reswitch; case 'L': _LL98: flags |= 2; goto rflag; case 'h': _LL99: flags |= 4;
goto rflag; case 'l': _LL100: flags |= 1; goto rflag; case 'c': _LL101: cp=
_tag_arr( buf, sizeof( unsigned char), 348u);*(( unsigned char*)
_check_unknown_subscript( cp, sizeof( unsigned char), 0u))=( unsigned char) Cyc_Stdio_va_arg_int(
ap); _tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); size= 1; sign='\000';
break; case 'D': _LL102: flags |= 1; goto _LL103; case 'd': _LL103: goto _LL104;
case 'i': _LL104: _ulong=( unsigned int)( flags & 1? Cyc_Stdio_va_arg_long( ap):(
flags & 4?( int)(( short) Cyc_Stdio_va_arg_int( ap)):( int) Cyc_Stdio_va_arg_int(
ap))); _tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); if(( int) _ulong
< 0){ _ulong= - _ulong; sign='-';} base= Cyc_Stdio_DEC; goto number; case 'e':
_LL105: goto _LL106; case 'E': _LL106: goto _LL107; case 'f': _LL107: goto
_LL108; case 'F': _LL108: goto _LL109; case 'g': _LL109: goto _LL110; case 'G':
_LL110: _double= Cyc_Stdio_va_arg_double( ap); _tagged_arr_inplace_plus_post(&
ap, sizeof( void*), 1); if( prec > 39){ if(( ch !='g'? ch !='G': 0)? 1: flags &
8){ fpprec= prec - 39;} prec= 39;} else{ if( prec == - 1){ prec= 6;}} cp=
_tag_arr( buf, sizeof( unsigned char), 348u);*(( unsigned char*)
_check_unknown_subscript( cp, sizeof( unsigned char), 0u))='\000'; size= Cyc_Stdio___cvt_double(
_double, prec, flags,& softsign, ch, cp, _tagged_arr_plus( _tag_arr( buf,
sizeof( unsigned char), 348u), sizeof( unsigned char),( int) sizeof( buf))); if(
softsign){ sign='-';} if(*(( unsigned char*) _check_unknown_subscript( cp,
sizeof( unsigned char), 0u)) =='\000'){ _tagged_arr_inplace_plus_post(& cp,
sizeof( unsigned char), 1);} break; case 'n': _LL111: if( flags & 1){* Cyc_Stdio_va_arg_int_ptr(
ap)=( unsigned int) ret;} else{ if( flags & 4){* Cyc_Stdio_va_arg_short_ptr( ap)=(
short) ret;} else{* Cyc_Stdio_va_arg_int_ptr( ap)=( unsigned int) ret;}}
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); continue; case 'O':
_LL112: flags |= 1; goto _LL113; case 'o': _LL113: _ulong= flags & 1? Cyc_Stdio_va_arg_ulong(
ap):( flags & 4?( unsigned int)(( unsigned short) Cyc_Stdio_va_arg_int( ap)):(
unsigned int) Cyc_Stdio_va_arg_uint( ap)); _tagged_arr_inplace_plus_post(& ap,
sizeof( void*), 1); base= Cyc_Stdio_OCT; goto nosign; case 'p': _LL114: _ulong=(
unsigned int) Cyc_Stdio_va_arg_long( ap); _tagged_arr_inplace_plus_post(& ap,
sizeof( void*), 1); base= Cyc_Stdio_HEX; flags |= 64; ch=( int)'x'; goto nosign;
case 's': _LL115: use_cp2= 1; cp2= Cyc_Stdio_va_arg_string( ap);
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); if( cp2.curr ==(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ cp2= _tag_arr("(null)", sizeof(
unsigned char), 7u);} if( prec >= 0){ struct _tagged_arr p= Cyc_Stdio_memchr(
cp2,'\000', prec); if( p.curr !=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
size=( p.curr - cp2.curr) / sizeof( unsigned char); if( size > prec){ size= prec;}}
else{ size= prec;}} else{ size= Cyc_String_strlen( cp2);} sign='\000'; break;
case 'U': _LL116: flags |= 1; goto _LL117; case 'u': _LL117: _ulong= flags & 1?
Cyc_Stdio_va_arg_ulong( ap):( flags & 4?( unsigned int)(( unsigned short) Cyc_Stdio_va_arg_int(
ap)):( unsigned int) Cyc_Stdio_va_arg_uint( ap)); _tagged_arr_inplace_plus_post(&
ap, sizeof( void*), 1); base= Cyc_Stdio_DEC; goto nosign; case 'X': _LL118: goto
_LL119; case 'x': _LL119: _ulong= flags & 1? Cyc_Stdio_va_arg_ulong( ap):( flags
& 4?( unsigned int)(( unsigned short) Cyc_Stdio_va_arg_int( ap)):( unsigned int)
Cyc_Stdio_va_arg_uint( ap)); _tagged_arr_inplace_plus_post(& ap, sizeof( void*),
1); base= Cyc_Stdio_HEX; if( flags & 8? _ulong != 0: 0){ flags |= 64;} nosign:
sign='\000'; number: if(( dprec= prec) >= 0){ flags &= ~ 32;} cp=
_tagged_arr_plus( _tag_arr( buf, sizeof( unsigned char), 348u), sizeof(
unsigned char),( 308 + 39) + 1); if( _ulong != 0? 1: prec != 0){ struct
_tagged_arr xdigs; enum  Cyc_Stdio_BASE _temp121= base; _LL123: if( _temp121 ==
Cyc_Stdio_OCT){ goto _LL124;} else{ goto _LL125;} _LL125: if( _temp121 == Cyc_Stdio_DEC){
goto _LL126;} else{ goto _LL127;} _LL127: if( _temp121 == Cyc_Stdio_HEX){ goto
_LL128;} else{ goto _LL122;} _LL124: do {*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus(& cp, sizeof( unsigned char),
-1), sizeof( unsigned char), 0u))=( unsigned char)(( _ulong & 7) +'0'); _ulong
>>= 3;} while (( int) _ulong); if( flags & 8?*(( unsigned char*)
_check_unknown_subscript( cp, sizeof( unsigned char), 0u)) !='0': 0){*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus(& cp, sizeof(
unsigned char), -1), sizeof( unsigned char), 0u))='0';} goto _LL122; _LL126:
while( _ulong >= 10) {*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& cp, sizeof( unsigned char), -1), sizeof(
unsigned char), 0u))=( unsigned char)( _ulong % 10 +'0'); _ulong /= 10;}*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus(& cp, sizeof(
unsigned char), -1), sizeof( unsigned char), 0u))=( unsigned char)( _ulong +'0');
goto _LL122; _LL128: if( ch =='X'){ xdigs= _tag_arr("0123456789ABCDEF", sizeof(
unsigned char), 17u);} else{ xdigs= _tag_arr("0123456789abcdef", sizeof(
unsigned char), 17u);} do {*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& cp, sizeof( unsigned char), -1), sizeof(
unsigned char), 0u))=*(( const unsigned char*) _check_unknown_subscript( xdigs,
sizeof( unsigned char),( int)( _ulong & 15))); _ulong >>= 4;} while (( int)
_ulong); goto _LL122; _LL122:;} size=(( _tagged_arr_plus( _tag_arr( buf, sizeof(
unsigned char), 348u), sizeof( unsigned char),( 308 + 39) + 1)).curr - cp.curr)
/ sizeof( unsigned char); skipsize: break; default: _LL120: if( ch =='\000'){
goto done;} cp= _tag_arr( buf, sizeof( unsigned char), 348u);*(( unsigned char*)
_check_unknown_subscript( cp, sizeof( unsigned char), 0u))=( unsigned char) ch;
size= 1; sign='\000'; break;} fieldsz= size + fpprec; dpad= dprec - size; if(
dpad < 0){ dpad= 0;} if(( int) sign){ fieldsz ++;} else{ if( flags & 64){
fieldsz += 2;}} fieldsz += dpad; if(( flags &( 16 | 32)) == 0){ if( Cyc_Stdio__IO_padn(
ioputc, ioputc_env,' ', width - fieldsz) < width - fieldsz){ goto error;}} if((
int) sign){ do { if( Cyc_Stdio__IO_sputn( ioputc, ioputc_env, _tag_arr(& sign,
sizeof( unsigned char), 1u), 1) != 1){ goto error;}} while ( 0);} else{ if(
flags & 64){ ox[ _check_known_subscript_notnull( 2u, 0)]='0'; ox[
_check_known_subscript_notnull( 2u, 1)]=( unsigned char) ch; do { if( Cyc_Stdio__IO_sputn(
ioputc, ioputc_env, _tag_arr( ox, sizeof( unsigned char), 2u), 2) != 2){ goto
error;}} while ( 0);}} if(( flags &( 16 | 32)) == 32){ if( Cyc_Stdio__IO_padn(
ioputc, ioputc_env,'0', width - fieldsz) < width - fieldsz){ goto error;}} if(
Cyc_Stdio__IO_padn( ioputc, ioputc_env,'0', dpad) < dpad){ goto error;} if(
use_cp2){ do { if( Cyc_Stdio__IO_sputn( ioputc, ioputc_env, cp2, size) != size){
goto error;}} while ( 0);} else{ do { if( Cyc_Stdio__IO_sputn( ioputc,
ioputc_env,( struct _tagged_arr) cp, size) != size){ goto error;}} while ( 0);}
if( Cyc_Stdio__IO_padn( ioputc, ioputc_env,'0', fpprec) < fpprec){ goto error;}
if( flags & 16){ if( Cyc_Stdio__IO_padn( ioputc, ioputc_env,' ', width - fieldsz)
< width - fieldsz){ goto error;}} ret += width > fieldsz? width: fieldsz;} done:
return ret; error: return - 1;}} static struct _tagged_arr Cyc_Stdio_exponent(
struct _tagged_arr p, int exp, int fmtch){ struct _tagged_arr t; unsigned char
expbuf[ 308u];{ unsigned int _temp131= 308u; unsigned int i; for( i= 0; i <
_temp131; i ++){ expbuf[ i]='0';}}*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))=( unsigned char) fmtch; if( exp < 0){ exp= - exp;*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& p,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))='-';} else{*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& p,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))='+';} t=
_tagged_arr_plus( _tag_arr( expbuf, sizeof( unsigned char), 308u), sizeof(
unsigned char), 308); if( exp > 9){ do {*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus(& t, sizeof( unsigned char),
-1), sizeof( unsigned char), 0u))=( unsigned char)( exp % 10 +'0');} while ((
exp /= 10) > 9);*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& t, sizeof( unsigned char), -1), sizeof( unsigned char),
0u))=( unsigned char)( exp +'0'); for( 0; t.curr <( _tagged_arr_plus( _tag_arr(
expbuf, sizeof( unsigned char), 308u), sizeof( unsigned char), 308)).curr;*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& p,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))=*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))){;}} else{*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& p, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))='0';*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& p, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char)( exp +'0');}
return p;} static struct _tagged_arr Cyc_Stdio_round( double fract, int* exp,
struct _tagged_arr start, struct _tagged_arr end, unsigned char ch, int* signp){
double tmp=( double) 0.0; if( fract != 0.0){ modf( fract * 10,( double*)& tmp);}
else{ tmp=( double)( ch -'0');} if( tmp > 4){ for( 0; 1;
_tagged_arr_inplace_plus(& end, sizeof( unsigned char), -1)){ if(*((
unsigned char*) _check_unknown_subscript( end, sizeof( unsigned char), 0u)) =='.'){
_tagged_arr_inplace_plus(& end, sizeof( unsigned char), -1);} if( ++(*((
unsigned char*) _check_unknown_subscript( end, sizeof( unsigned char), 0u))) <='9'){
break;}*(( unsigned char*) _check_unknown_subscript( end, sizeof( unsigned char),
0u))='0'; if( end.curr == start.curr){ if(( unsigned int) exp){*(( unsigned char*)
_check_unknown_subscript( end, sizeof( unsigned char), 0u))='1'; ++(*(( int*)
_check_null( exp)));} else{*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& end, sizeof( unsigned char), -1), sizeof(
unsigned char), 0u))='1'; _tagged_arr_inplace_plus(& start, sizeof(
unsigned char), -1);} break;}}} else{ if(* signp =='-'){ for( 0; 1;
_tagged_arr_inplace_plus(& end, sizeof( unsigned char), -1)){ if(*((
unsigned char*) _check_unknown_subscript( end, sizeof( unsigned char), 0u)) =='.'){
_tagged_arr_inplace_plus(& end, sizeof( unsigned char), -1);} if(*((
unsigned char*) _check_unknown_subscript( end, sizeof( unsigned char), 0u)) !='0'){
break;} if( end.curr == start.curr){* signp= 0;}}}} return start;} int Cyc_Stdio___cvt_double(
double number, int prec, int flags, int* signp, int fmtch, struct _tagged_arr
startp, struct _tagged_arr endp){ struct _tagged_arr p; struct _tagged_arr t;
double fract; int dotrim= 0; int expcnt; int gformat= 0; double integer=( double)
0.0; double tmp=( double) 0.0; expcnt= 0; if( number < 0){ number= - number;*
signp=( int)'-';} else{* signp= 0;} fract= modf( number,( double*)& integer); t=
_tagged_arr_inplace_plus(& startp, sizeof( unsigned char), 1); for( p=
_tagged_arr_plus( endp, sizeof( unsigned char), - 1); p.curr >= startp.curr?
integer != 0.0: 0; ++ expcnt){ tmp= modf( integer / 10,( double*)& integer);*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& p,
sizeof( unsigned char), -1), sizeof( unsigned char), 0u))=( unsigned char)(( int)((
tmp + .01) * 10) +'0');} switch( fmtch){ case 'f': _LL132: goto _LL133; case 'F':
_LL133: if( expcnt){ for( 0;( _tagged_arr_inplace_plus(& p, sizeof(
unsigned char), 1)).curr < endp.curr;*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u))){;}} else{*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& t,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))='0';} if( prec? 1:
flags & 8){*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='.';} if( fract != 0.0){ if( prec){ do { fract= modf( fract
* 10,( double*)& tmp);*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))=( unsigned char)(( int) tmp +'0');} while ( -- prec? fract
!= 0.0: 0);} if( fract != 0.0){ startp= Cyc_Stdio_round( fract, 0, startp,
_tagged_arr_plus( t, sizeof( unsigned char), - 1),( unsigned char) 0, signp);}}
for( 0; prec --;*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='0'){;} break; case 'e': _LL134: goto _LL135; case 'E':
_LL135: eformat: if( expcnt){*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))=*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), 1), sizeof( unsigned char),
0u)); if( prec? 1: flags & 8){*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='.';} for( 0; prec?( _tagged_arr_inplace_plus(& p, sizeof(
unsigned char), 1)).curr < endp.curr: 0; -- prec){*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u));} if( ! prec?(
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), 1)).curr < endp.curr: 0){
fract=( double) 0; startp= Cyc_Stdio_round(( double) 0,( int*)& expcnt, startp,
_tagged_arr_plus( t, sizeof( unsigned char), - 1),*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u)), signp);} -- expcnt;}
else{ if( fract != 0.0){ for( expcnt= - 1; 1; -- expcnt){ fract= modf( fract *
10,( double*)& tmp); if( tmp != 0.0){ break;}}*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char)(( int) tmp +'0');
if( prec? 1: flags & 8){*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='.';}} else{*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='0'; if( prec? 1: flags & 8){*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))='.';}}} if( fract != 0.0){ if(
prec){ do { fract= modf( fract * 10,( double*)& tmp);*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char)(( int) tmp +'0');} while (
-- prec? fract != 0.0: 0);} if( fract != 0.0){ startp= Cyc_Stdio_round( fract,(
int*)& expcnt, startp, _tagged_arr_plus( t, sizeof( unsigned char), - 1),(
unsigned char) 0, signp);}} for( 0; prec --;*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))='0'){;} if( gformat? !( flags &
8): 0){ while( t.curr > startp.curr?*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus(& t, sizeof( unsigned char), -1), sizeof( unsigned char),
0u)) =='0': 0) {;} if(*(( unsigned char*) _check_unknown_subscript( t, sizeof(
unsigned char), 0u)) =='.'){ _tagged_arr_inplace_plus(& t, sizeof( unsigned char),
-1);} _tagged_arr_inplace_plus(& t, sizeof( unsigned char), 1);} t= Cyc_Stdio_exponent(
t, expcnt, fmtch); break; case 'g': _LL136: goto _LL137; case 'G': _LL137: if( !
prec){ ++ prec;} if( expcnt > prec? 1:(( ! expcnt? fract != 0.0: 0)? fract <
.0001: 0)){ -- prec; fmtch -= 2; gformat= 1; goto eformat;} if( expcnt){ for( 0;(
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), 1)).curr < endp.curr;(*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& t,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))=*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u)), -- prec)){;}} else{*((
unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(& t,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u))='0';} if( prec? 1:
flags & 8){ dotrim= 1;*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))='.';} else{ dotrim= 0;} if( fract != 0.0){ if( prec){ do {
fract= modf( fract * 10,( double*)& tmp);*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& t, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char)(( int) tmp +'0');} while (
tmp == 0.0? ! expcnt: 0); while( -- prec? fract != 0.0: 0) { fract= modf( fract
* 10,( double*)& tmp);*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u))=( unsigned char)(( int) tmp +'0');}} if( fract != 0.0){
startp= Cyc_Stdio_round( fract, 0, startp, _tagged_arr_plus( t, sizeof(
unsigned char), - 1),( unsigned char) 0, signp);}} if( flags & 8){ for( 0; prec
--;*(( unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(&
t, sizeof( unsigned char), 1), sizeof( unsigned char), 0u))='0'){;}} else{ if(
dotrim){ while( t.curr > startp.curr?*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus(& t, sizeof( unsigned char),
-1), sizeof( unsigned char), 0u)) =='0': 0) {;} if(*(( unsigned char*)
_check_unknown_subscript( t, sizeof( unsigned char), 0u)) !='.'){
_tagged_arr_inplace_plus(& t, sizeof( unsigned char), 1);}}} break; default:
_LL138:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp140=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp140[ 0]=({ struct Cyc_Core_Impossible_struct _temp141; _temp141.tag= Cyc_Core_Impossible;
_temp141.f1= _tag_arr("stdio::__cvt_double", sizeof( unsigned char), 20u);
_temp141;}); _temp140;}));} return( t.curr - startp.curr) / sizeof(
unsigned char);} int Cyc_Stdio_vfprintf( struct Cyc_Stdio___sFILE* f, struct
_tagged_arr fmt, struct _tagged_arr ap){ int ans; ans=(( int(*)( int(* ioputc)(
int, struct Cyc_Stdio___sFILE*), struct Cyc_Stdio___sFILE* ioputc_env, struct
_tagged_arr fmt0, struct _tagged_arr ap)) Cyc_Stdio__IO_vfprintf)( Cyc_Stdio_putc,
f, fmt, ap); return ans;} int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE* f,
struct _tagged_arr fmt, struct _tagged_arr ap){ return Cyc_Stdio_vfprintf( f,
fmt, ap);} int Cyc_Stdio_vprintf( struct _tagged_arr fmt, struct _tagged_arr ap){
int ans; ans=(( int(*)( int(* ioputc)( int, struct Cyc_Stdio___sFILE*), struct
Cyc_Stdio___sFILE* ioputc_env, struct _tagged_arr fmt0, struct _tagged_arr ap))
Cyc_Stdio__IO_vfprintf)( Cyc_Stdio_putc, Cyc_Stdio_stdout, fmt, ap); return ans;}
int Cyc_Stdio_printf( struct _tagged_arr fmt, struct _tagged_arr ap){ int ans;
ans= Cyc_Stdio_vprintf( fmt, ap); return ans;} static int Cyc_Stdio_putc_string(
int c, struct _tagged_arr* sptr){ struct _tagged_arr s=* sptr; if( _get_arr_size(
s, sizeof( unsigned char)) == 0){ return - 1;}*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u))=( unsigned char) c;*
sptr= _tagged_arr_plus( s, sizeof( unsigned char), 1); return 1;} int Cyc_Stdio_vsprintf(
struct _tagged_arr s, struct _tagged_arr fmt, struct _tagged_arr ap){ int ans;
struct _tagged_arr _temp142= s; ans=(( int(*)( int(* ioputc)( int, struct
_tagged_arr*), struct _tagged_arr* ioputc_env, struct _tagged_arr fmt0, struct
_tagged_arr ap)) Cyc_Stdio__IO_vfprintf)( Cyc_Stdio_putc_string,& _temp142, fmt,
ap); if( 0 <= ans){*(( unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), ans))='\000';} return ans;} int Cyc_Stdio_sprintf( struct
_tagged_arr s, struct _tagged_arr fmt, struct _tagged_arr ap){ return Cyc_Stdio_vsprintf(
s, fmt, ap);} static int Cyc_Stdio_putc_void( int c, int dummy){ return 1;}
struct _tagged_arr Cyc_Stdio_vraprintf( struct _RegionHandle* r1, struct
_tagged_arr fmt, struct _tagged_arr ap){ int size=(( int(*)( int(* ioputc)( int,
int), int ioputc_env, struct _tagged_arr fmt0, struct _tagged_arr ap)) Cyc_Stdio__IO_vfprintf)(
Cyc_Stdio_putc_void, 0, fmt, ap) + 1; struct _tagged_arr s=({ unsigned int
_temp143=( unsigned int) size; unsigned char* _temp144=( unsigned char*)
_region_malloc( r1, sizeof( unsigned char) * _temp143); struct _tagged_arr
_temp146= _tag_arr( _temp144, sizeof( unsigned char),( unsigned int) size);{
unsigned int _temp145= _temp143; unsigned int i; for( i= 0; i < _temp145; i ++){
_temp144[ i]='\000';}}; _temp146;}); Cyc_Stdio_vsprintf( s, fmt, ap); return s;}
struct _tagged_arr Cyc_Stdio_raprintf( struct _RegionHandle* r1, struct
_tagged_arr fmt, struct _tagged_arr ap){ return Cyc_Stdio_vraprintf( r1, fmt, ap);}
struct _tagged_arr Cyc_Stdio_aprintf( struct _tagged_arr fmt, struct _tagged_arr
ap){ return Cyc_Stdio_vraprintf( Cyc_Core_heap_region, fmt, ap);}
