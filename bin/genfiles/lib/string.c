#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t;
typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t;
typedef unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_Character_isdigit(
char c); extern int Cyc_Character_isspace( char c); extern char Cyc_Character_toupper(
char c); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern unsigned int Cyc_String_strlen( struct _tagged_string s);
extern int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
_tagged_string* s2); extern int Cyc_String_strncmp( struct _tagged_string s1,
struct _tagged_string s2, unsigned int len); extern int Cyc_String_strncasecmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int len); extern
int Cyc_String_zstrcmp( struct _tagged_string, struct _tagged_string); extern
int Cyc_String_zstrncmp( struct _tagged_string s1, struct _tagged_string s2,
unsigned int n); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strcat( struct
_tagged_string dest, struct _tagged_string src); extern struct _tagged_string
Cyc_String_strconcat( struct _tagged_string, struct _tagged_string); extern
struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*); extern
struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern struct _tagged_string Cyc_String_strcpy( struct
_tagged_string dest, struct _tagged_string src); extern struct _tagged_string
Cyc_String_strncpy( struct _tagged_string, int, struct _tagged_string, int,
unsigned int); extern struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string, int, struct _tagged_string, int, unsigned int); extern struct
_tagged_string Cyc_String_expand( struct _tagged_string s, unsigned int sz);
extern struct _tagged_string Cyc_String_realloc_str( struct _tagged_string str,
unsigned int sz); extern struct _tagged_string Cyc_String_strdup( struct
_tagged_string src); extern struct _tagged_string Cyc_String_substring( struct
_tagged_string, int ofs, unsigned int n); extern struct _tagged_string Cyc_String_replace_suffix(
struct _tagged_string, struct _tagged_string, struct _tagged_string); extern
struct _tagged_string Cyc_String_strchr( struct _tagged_string s, char c);
extern struct _tagged_string Cyc_String_strrchr( struct _tagged_string s, char c);
extern struct _tagged_string Cyc_String_strpbrk( struct _tagged_string s, struct
_tagged_string accept); extern unsigned int Cyc_String_strspn( struct
_tagged_string s, struct _tagged_string accept); extern struct Cyc_List_List*
Cyc_String_explode( struct _tagged_string s); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); extern int Cyc_String_to_int( struct _tagged_string s,
int* offset); extern double Cyc_String_to_double( struct _tagged_string s, int*
offset); extern double atan( double); extern double cos( double); extern double
sin( double); extern double tan( double); extern double tanh( double); extern
double frexp( double, int*); extern double modf( double, double*); extern double
ceil( double); extern double fabs( double); extern double floor( double); extern
double acos( double); extern double asin( double); extern double atan2( double,
double); extern double cosh( double); extern double sinh( double); extern double
exp( double); extern double ldexp( double, int); extern double log( double);
extern double log10( double); extern double pow( double, double); extern double
sqrt( double); extern double fmod( double, double); extern double infinity();
extern double nan(); extern int isnan( double); extern int isinf( double);
extern int finite( double); extern double copysign( double, double); extern int
ilogb( double); extern double asinh( double); extern double cbrt( double);
extern double nextafter( double, double); extern double rint( double); extern
double scalbn( double, int); extern double log1p( double); extern double expm1(
double); extern double acosh( double); extern double atanh( double); extern
double remainder( double, double); extern double gamma( double); extern double
gamma_r( double, int*); extern double lgamma( double); extern double lgamma_r(
double, int*); extern double erf( double); extern double erfc( double); extern
double y0( double); extern double y1( double); extern double yn( int, double);
extern double j0( double); extern double j1( double); extern double jn( int,
double); extern double hypot( double, double); extern double cabs(); extern
double drem( double, double); extern float atanf( float); extern float cosf(
float); extern float sinf( float); extern float tanf( float); extern float tanhf(
float); extern float frexpf( float, int*); extern float modff( float, float*);
extern float ceilf( float); extern float fabsf( float); extern float floorf(
float); extern float acosf( float); extern float asinf( float); extern float
atan2f( float, float); extern float coshf( float); extern float sinhf( float);
extern float expf( float); extern float ldexpf( float, int); extern float logf(
float); extern float log10f( float); extern float powf( float, float); extern
float sqrtf( float); extern float fmodf( float, float); extern float infinityf();
extern float nanf(); extern int isnanf( float); extern int isinff( float);
extern int finitef( float); extern float copysignf( float, float); extern int
ilogbf( float); extern float asinhf( float); extern float cbrtf( float); extern
float nextafterf( float, float); extern float rintf( float); extern float
scalbnf( float, int); extern float log1pf( float); extern float expm1f( float);
extern float acoshf( float); extern float atanhf( float); extern float
remainderf( float, float); extern float gammaf( float); extern float gammaf_r(
float, int*); extern float lgammaf( float); extern float lgammaf_r( float, int*);
extern float erff( float); extern float erfcf( float); extern float y0f( float);
extern float y1f( float); extern float ynf( int, float); extern float j0f( float);
extern float j1f( float); extern float jnf( int, float); extern float hypotf(
float, float); extern float cabsf(); extern float dremf( float, float); extern
int signgam; struct Cyc_Math___exception{ int type; char* name; double arg1;
double arg2; double retval; int err; } ; extern int matherr( struct Cyc_Math___exception*
e); unsigned int Cyc_String_strlen( struct _tagged_string s){ unsigned int i;
for( i= 0; i <({ struct _tagged_string _temp0= s;( unsigned int)( _temp0.last_plus_one
- _temp0.curr);}); i ++){ if(( int)({ struct _tagged_string _temp1= s; char*
_temp3= _temp1.curr +( int) i; if( _temp1.base == 0? 1:( _temp3 < _temp1.base? 1:
_temp3 >= _temp1.last_plus_one)){ _throw( Null_Exception);}* _temp3;}) ==( int)'\000'){
return i;}} return i;} static unsigned int Cyc_String_int_strleno( struct
_tagged_string s, int ofs, struct _tagged_string error){ if(( unsigned int) ofs
>({ struct _tagged_string _temp4= s;( unsigned int)( _temp4.last_plus_one -
_temp4.curr);})? 1: ofs < 0){( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_InvalidArg_struct* _temp5=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp5[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp6; _temp6.tag= Cyc_Core_InvalidArg_tag; _temp6.f1= error; _temp6;}); _temp5;}));}{
int i; for( i= ofs;( unsigned int) i <({ struct _tagged_string _temp7= s;(
unsigned int)( _temp7.last_plus_one - _temp7.curr);}); i ++){ if(( int)({ struct
_tagged_string _temp8= s; char* _temp10= _temp8.curr + i; if( _temp8.base == 0?
1:( _temp10 < _temp8.base? 1: _temp10 >= _temp8.last_plus_one)){ _throw(
Null_Exception);}* _temp10;}) ==( int)'\000'){ break;}} return( unsigned int)( i
- ofs);}} static int Cyc_String_case_cmp( char c1, char c2){ return( int) c1 -(
int) c2;} static int Cyc_String_nocase_cmp( char c1, char c2){ return Cyc_String_case_cmp(
Cyc_Character_toupper( c1), Cyc_Character_toupper( c2));} static int Cyc_String_cmp(
struct _tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2,
int ofs2, unsigned int len2, int(* f)( char, char)){ unsigned int min_length=
len1 < len2? len1: len2; int i= - 1; while(( unsigned int)( ++ i) < min_length) {
int diff= f(({ struct _tagged_string _temp11= s1; char* _temp13= _temp11.curr +(
i + ofs1); if( _temp11.base == 0? 1:( _temp13 < _temp11.base? 1: _temp13 >=
_temp11.last_plus_one)){ _throw( Null_Exception);}* _temp13;}),({ struct
_tagged_string _temp14= s2; char* _temp16= _temp14.curr +( i + ofs2); if(
_temp14.base == 0? 1:( _temp16 < _temp14.base? 1: _temp16 >= _temp14.last_plus_one)){
_throw( Null_Exception);}* _temp16;})); if( diff != 0){ return diff;}} return(
int) len1 -( int) len2;} int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp17=( char*)"String::strcmp"; struct
_tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one=
_temp17 + 15; _temp18;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,(
struct _tagged_string)({ char* _temp19=( char*)"String::strcmp"; struct
_tagged_string _temp20; _temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one=
_temp19 + 15; _temp20;})); return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}}
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2){
return Cyc_String_strcmp(* s1,* s2);} static int Cyc_String_ncmp( struct
_tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2, int
ofs2, unsigned int len2, unsigned int n, int(* f)( char, char)){ if( n <=(
unsigned int) 0){ return 0;}{ unsigned int min_len= len1 > len2? len2: len1;
unsigned int bound= min_len > n? n: min_len;{ int i= 0; for( 0;( unsigned int) i
< bound; i ++){ int retc; if(( retc= f(({ struct _tagged_string _temp21= s1;
char* _temp23= _temp21.curr +( i + ofs1); if( _temp21.base == 0? 1:( _temp23 <
_temp21.base? 1: _temp23 >= _temp21.last_plus_one)){ _throw( Null_Exception);}*
_temp23;}),({ struct _tagged_string _temp24= s2; char* _temp26= _temp24.curr +(
i + ofs2); if( _temp24.base == 0? 1:( _temp26 < _temp24.base? 1: _temp26 >=
_temp24.last_plus_one)){ _throw( Null_Exception);}* _temp26;}))) != 0){ return
retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp27=( char*)"String::strncmp"; struct
_tagged_string _temp28; _temp28.curr= _temp27; _temp28.base= _temp27; _temp28.last_plus_one=
_temp27 + 16; _temp28;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,(
struct _tagged_string)({ char* _temp29=( char*)"String::strncmp"; struct
_tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29; _temp30.last_plus_one=
_temp29 + 16; _temp30;})); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n,
Cyc_String_case_cmp);} int Cyc_String_strncasecmp( struct _tagged_string s1,
struct _tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp31=( char*)"String::strncasecmp";
struct _tagged_string _temp32; _temp32.curr= _temp31; _temp32.base= _temp31;
_temp32.last_plus_one= _temp31 + 20; _temp32;})); unsigned int len2= Cyc_String_int_strleno(
s2, 0,( struct _tagged_string)({ char* _temp33=( char*)"String::strncasecmp";
struct _tagged_string _temp34; _temp34.curr= _temp33; _temp34.base= _temp33;
_temp34.last_plus_one= _temp33 + 20; _temp34;})); return Cyc_String_ncmp( s1, 0,
len1, s2, 0, len2, n, Cyc_String_nocase_cmp);} int Cyc_String_zstrcmp( struct
_tagged_string a, struct _tagged_string b){ if( a.curr == b.curr){ return 0;}{
unsigned int as=({ struct _tagged_string _temp35= a;( unsigned int)( _temp35.last_plus_one
- _temp35.curr);}); unsigned int bs=({ struct _tagged_string _temp36= b;(
unsigned int)( _temp36.last_plus_one - _temp36.curr);}); unsigned int min_length=
as < bs? as: bs; int i= - 1; while(( unsigned int)( ++ i) < min_length) { int
diff=( int)({ struct _tagged_string _temp37= a; char* _temp39= _temp37.curr + i;
if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one)){
_throw( Null_Exception);}* _temp39;}) -( int)({ struct _tagged_string _temp40= b;
char* _temp42= _temp40.curr + i; if( _temp40.base == 0? 1:( _temp42 < _temp40.base?
1: _temp42 >= _temp40.last_plus_one)){ _throw( Null_Exception);}* _temp42;});
if( diff != 0){ return diff;}} return( int) as -( int) bs;}} int Cyc_String_zstrncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int n){ if( n <=(
unsigned int) 0){ return 0;}{ unsigned int s1size=({ struct _tagged_string
_temp43= s1;( unsigned int)( _temp43.last_plus_one - _temp43.curr);});
unsigned int s2size=({ struct _tagged_string _temp44= s2;( unsigned int)(
_temp44.last_plus_one - _temp44.curr);}); unsigned int min_size= s1size > s2size?
s2size: s1size; unsigned int bound= min_size > n? n: min_size;{ int i= 0; for( 0;(
unsigned int) i < bound; i ++){ if(( int)({ struct _tagged_string _temp45= s1;
char* _temp47= _temp45.curr + i; if( _temp45.base == 0? 1:( _temp47 < _temp45.base?
1: _temp47 >= _temp45.last_plus_one)){ _throw( Null_Exception);}* _temp47;}) <(
int)({ struct _tagged_string _temp48= s2; char* _temp50= _temp48.curr + i; if(
_temp48.base == 0? 1:( _temp50 < _temp48.base? 1: _temp50 >= _temp48.last_plus_one)){
_throw( Null_Exception);}* _temp50;})){ return - 1;} else{ if(( int)({ struct
_tagged_string _temp51= s2; char* _temp53= _temp51.curr + i; if( _temp51.base ==
0? 1:( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one)){ _throw(
Null_Exception);}* _temp53;}) <( int)({ struct _tagged_string _temp54= s1; char*
_temp56= _temp54.curr + i; if( _temp54.base == 0? 1:( _temp56 < _temp54.base? 1:
_temp56 >= _temp54.last_plus_one)){ _throw( Null_Exception);}* _temp56;})){
return 1;}}}} if( min_size <= bound){ return 0;} if( s1size < s2size){ return -
1;} else{ return 1;}}} int Cyc_String_zstrptrcmp( struct _tagged_string* a,
struct _tagged_string* b){ return Cyc_String_zstrcmp(* a,* b);} static struct
_tagged_string Cyc_String_int_strcato( struct _tagged_string dest, struct
_tagged_string src, int srcOfs, struct _tagged_string error){ int i;
unsigned int dsize; unsigned int slen; unsigned int dlen; dsize=({ struct
_tagged_string _temp57= dest;( unsigned int)( _temp57.last_plus_one - _temp57.curr);});
dlen= Cyc_String_strlen( dest); slen= Cyc_String_int_strleno( src, srcOfs, error);
if( slen + dlen <= dsize){ for( i= 0;( unsigned int) i < slen; i ++){({ struct
_tagged_string _temp58= dest; char* _temp60= _temp58.curr +( int)(( unsigned int)
i + dlen); if( _temp58.base == 0? 1:( _temp60 < _temp58.base? 1: _temp60 >=
_temp58.last_plus_one)){ _throw( Null_Exception);}* _temp60=({ struct
_tagged_string _temp61= src; char* _temp63= _temp61.curr +( i + srcOfs); if(
_temp61.base == 0? 1:( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one)){
_throw( Null_Exception);}* _temp63;});});} if(( unsigned int) i != dsize){({
struct _tagged_string _temp64= dest; char* _temp66= _temp64.curr +( int)((
unsigned int) i + dlen); if( _temp64.base == 0? 1:( _temp66 < _temp64.base? 1:
_temp66 >= _temp64.last_plus_one)){ _throw( Null_Exception);}* _temp66='\000';});}}
else{( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp67=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp67[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp68; _temp68.tag= Cyc_Core_InvalidArg_tag;
_temp68.f1= error; _temp68;}); _temp67;}));} return dest;} struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src){
return Cyc_String_int_strcato( dest, src, 0,( struct _tagged_string)({ char*
_temp69=( char*)"String::strcat"; struct _tagged_string _temp70; _temp70.curr=
_temp69; _temp70.base= _temp69; _temp70.last_plus_one= _temp69 + 15; _temp70;}));}
struct _tagged_string Cyc_String_strconcat( struct _tagged_string a, struct
_tagged_string b){ unsigned int alen= Cyc_String_strlen( a); unsigned int blen=
Cyc_String_strlen( b); struct _tagged_string ans= Cyc_Core_new_string(( int)(
alen + blen)); int i; int j; for( i= 0;( unsigned int) i < alen; ++ i){({ struct
_tagged_string _temp71= ans; char* _temp73= _temp71.curr + i; if( _temp71.base
== 0? 1:( _temp73 < _temp71.base? 1: _temp73 >= _temp71.last_plus_one)){ _throw(
Null_Exception);}* _temp73=({ struct _tagged_string _temp74= a; char* _temp76=
_temp74.curr + i; if( _temp74.base == 0? 1:( _temp76 < _temp74.base? 1: _temp76
>= _temp74.last_plus_one)){ _throw( Null_Exception);}* _temp76;});});} for( j= 0;(
unsigned int) j < blen; ++ j){({ struct _tagged_string _temp77= ans; char*
_temp79= _temp77.curr +( i + j); if( _temp77.base == 0? 1:( _temp79 < _temp77.base?
1: _temp79 >= _temp77.last_plus_one)){ _throw( Null_Exception);}* _temp79=({
struct _tagged_string _temp80= b; char* _temp82= _temp80.curr + j; if( _temp80.base
== 0? 1:( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one)){ _throw(
Null_Exception);}* _temp82;});});} return ans;} struct _tagged_string Cyc_String_strconcat_l(
struct Cyc_List_List* strs){ unsigned int len; unsigned int total_len=(
unsigned int) 0; struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp83=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp83->hd=(
void*)(( unsigned int) 0); _temp83->tl= 0; _temp83;}); struct Cyc_List_List* end=
lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p=({ struct Cyc_List_List*
_temp84= p; if( _temp84 == 0){ _throw( Null_Exception);} _temp84->tl;})){ len=
Cyc_String_strlen(*(( struct _tagged_string*)({ struct Cyc_List_List* _temp85= p;
if( _temp85 == 0){ _throw( Null_Exception);} _temp85->hd;}))); total_len += len;({
struct Cyc_List_List* _temp86= end; if( _temp86 == 0){ _throw( Null_Exception);}
_temp86->tl=({ struct Cyc_List_List* _temp87=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp87->hd=( void*) len; _temp87->tl= 0;
_temp87;});}); end=({ struct Cyc_List_List* _temp88= end; if( _temp88 == 0){
_throw( Null_Exception);} _temp88->tl;});}} lens=({ struct Cyc_List_List*
_temp89= lens; if( _temp89 == 0){ _throw( Null_Exception);} _temp89->tl;});{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*)({ struct Cyc_List_List* _temp90= strs; if( _temp90 == 0){
_throw( Null_Exception);} _temp90->hd;})); len=( unsigned int)({ struct Cyc_List_List*
_temp91= lens; if( _temp91 == 0){ _throw( Null_Exception);} _temp91->hd;}); Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs=({ struct Cyc_List_List* _temp92=
strs; if( _temp92 == 0){ _throw( Null_Exception);} _temp92->tl;}); lens=({
struct Cyc_List_List* _temp93= lens; if( _temp93 == 0){ _throw( Null_Exception);}
_temp93->tl;});} return ans;}} struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List* strs, struct _tagged_string separator){ if( strs == 0){
return( struct _tagged_string)({ char* _temp94=( char*)""; struct _tagged_string
_temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 1; _temp95;});} if(({ struct Cyc_List_List* _temp96= strs; if( _temp96
== 0){ _throw( Null_Exception);} _temp96->tl;}) == 0){ return Cyc_String_strdup(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp97= strs; if( _temp97 == 0){
_throw( Null_Exception);} _temp97->hd;})));}{ struct Cyc_List_List* p= strs;
struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp98->hd=( void*)(( unsigned int)
0); _temp98->tl= 0; _temp98;}); struct Cyc_List_List* end= lens; unsigned int
len=( unsigned int) 0; unsigned int total_len=( unsigned int) 0; unsigned int
list_len=( unsigned int) 0; for( 0; p != 0; p=({ struct Cyc_List_List* _temp99=
p; if( _temp99 == 0){ _throw( Null_Exception);} _temp99->tl;})){ len= Cyc_String_strlen(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp100= p; if( _temp100 == 0){
_throw( Null_Exception);} _temp100->hd;}))); total_len += len;({ struct Cyc_List_List*
_temp101= end; if( _temp101 == 0){ _throw( Null_Exception);} _temp101->tl=({
struct Cyc_List_List* _temp102=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp102->hd=( void*) len; _temp102->tl= 0; _temp102;});});
end=({ struct Cyc_List_List* _temp103= end; if( _temp103 == 0){ _throw(
Null_Exception);} _temp103->tl;}); ++ list_len;} lens=({ struct Cyc_List_List*
_temp104= lens; if( _temp104 == 0){ _throw( Null_Exception);} _temp104->tl;});{
unsigned int seplen= Cyc_String_strlen( separator); total_len +=( list_len -(
unsigned int) 1) * seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int)
total_len); unsigned int i=( unsigned int) 0; while(({ struct Cyc_List_List*
_temp105= strs; if( _temp105 == 0){ _throw( Null_Exception);} _temp105->tl;}) !=
0) { struct _tagged_string next=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp106= strs; if( _temp106 == 0){ _throw( Null_Exception);} _temp106->hd;}));
len=( unsigned int)({ struct Cyc_List_List* _temp107= lens; if( _temp107 == 0){
_throw( Null_Exception);} _temp107->hd;}); Cyc_String_strncpy( ans,( int) i,
next, 0, len); i += len; Cyc_String_strncpy( ans,( int) i, separator, 0, seplen);
i += seplen; strs=({ struct Cyc_List_List* _temp108= strs; if( _temp108 == 0){
_throw( Null_Exception);} _temp108->tl;}); lens=({ struct Cyc_List_List*
_temp109= lens; if( _temp109 == 0){ _throw( Null_Exception);} _temp109->tl;});}
Cyc_String_strncpy( ans,( int) i,*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp110= strs; if( _temp110 == 0){ _throw( Null_Exception);} _temp110->hd;})),
0,( unsigned int)({ struct Cyc_List_List* _temp111= lens; if( _temp111 == 0){
_throw( Null_Exception);} _temp111->hd;})); return ans;}}}} struct
_tagged_string Cyc_String_strncpy( struct _tagged_string dest, int destOfs,
struct _tagged_string src, int srcOfs, unsigned int n){ unsigned int destSz=({
struct _tagged_string _temp112= dest;( unsigned int)( _temp112.last_plus_one -
_temp112.curr);}); unsigned int srcSz=({ struct _tagged_string _temp113= src;(
unsigned int)( _temp113.last_plus_one - _temp113.curr);}); if( destOfs < 0){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp114=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp114[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp115; _temp115.tag= Cyc_Core_InvalidArg_tag;
_temp115.f1=( struct _tagged_string)({ char* _temp116=( char*)"String::strncpy";
struct _tagged_string _temp117; _temp117.curr= _temp116; _temp117.base= _temp116;
_temp117.last_plus_one= _temp116 + 16; _temp117;}); _temp115;}); _temp114;}));}
if( srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp118=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp118[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp119; _temp119.tag= Cyc_Core_InvalidArg_tag;
_temp119.f1=( struct _tagged_string)({ char* _temp120=( char*)"String::strncpy";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 16; _temp121;}); _temp119;}); _temp118;}));}
if( n <=( unsigned int) 0){ return dest;} if(( unsigned int) destOfs + n >
destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp122=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp122[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp123; _temp123.tag= Cyc_Core_InvalidArg_tag;
_temp123.f1=( struct _tagged_string)({ char* _temp124=( char*)"String::strncpy";
struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 16; _temp125;}); _temp123;}); _temp122;}));}{
int i; for( i= 0;( unsigned int) i < n; i ++){ char srcChar=({ struct
_tagged_string _temp126= src; char* _temp128= _temp126.curr +( srcOfs + i); if(
_temp126.base == 0? 1:( _temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one)){
_throw( Null_Exception);}* _temp128;}); if(( int) srcChar ==( int)'\000'){
break;}({ struct _tagged_string _temp129= dest; char* _temp131= _temp129.curr +(
destOfs + i); if( _temp129.base == 0? 1:( _temp131 < _temp129.base? 1: _temp131
>= _temp129.last_plus_one)){ _throw( Null_Exception);}* _temp131= srcChar;});}
for( 0;( unsigned int) i < n; i ++){({ struct _tagged_string _temp132= dest;
char* _temp134= _temp132.curr +( destOfs + i); if( _temp132.base == 0? 1:(
_temp134 < _temp132.base? 1: _temp134 >= _temp132.last_plus_one)){ _throw(
Null_Exception);}* _temp134='\000';});} return dest;}} struct _tagged_string Cyc_String_zstrncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int destSz=({ struct _tagged_string _temp135= dest;(
unsigned int)( _temp135.last_plus_one - _temp135.curr);}); unsigned int srcSz=({
struct _tagged_string _temp136= src;( unsigned int)( _temp136.last_plus_one -
_temp136.curr);}); if( destOfs < 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp137=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp137[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp138; _temp138.tag= Cyc_Core_InvalidArg_tag;
_temp138.f1=( struct _tagged_string)({ char* _temp139=( char*)"String::zstrncpy";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 17; _temp140;}); _temp138;}); _temp137;}));}
if( srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp141=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp141[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp142; _temp142.tag= Cyc_Core_InvalidArg_tag;
_temp142.f1=( struct _tagged_string)({ char* _temp143=( char*)"String::zstrncpy";
struct _tagged_string _temp144; _temp144.curr= _temp143; _temp144.base= _temp143;
_temp144.last_plus_one= _temp143 + 17; _temp144;}); _temp142;}); _temp141;}));}
if( n <=( unsigned int) 0){ return dest;} if(( unsigned int) destOfs + n >
destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp145=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp145[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp146; _temp146.tag= Cyc_Core_InvalidArg_tag;
_temp146.f1=( struct _tagged_string)({ char* _temp147=( char*)"String::zstrncpy";
struct _tagged_string _temp148; _temp148.curr= _temp147; _temp148.base= _temp147;
_temp148.last_plus_one= _temp147 + 17; _temp148;}); _temp146;}); _temp145;}));}{
int i; for( i= 0;( unsigned int) i < n; i ++){({ struct _tagged_string _temp149=
dest; char* _temp151= _temp149.curr +( destOfs + i); if( _temp149.base == 0? 1:(
_temp151 < _temp149.base? 1: _temp151 >= _temp149.last_plus_one)){ _throw(
Null_Exception);}* _temp151=({ struct _tagged_string _temp152= src; char*
_temp154= _temp152.curr +( srcOfs + i); if( _temp152.base == 0? 1:( _temp154 <
_temp152.base? 1: _temp154 >= _temp152.last_plus_one)){ _throw( Null_Exception);}*
_temp154;});});} return dest;}} struct _tagged_string Cyc_String_strcpy( struct
_tagged_string dest, struct _tagged_string src){ unsigned int len= Cyc_String_strlen(
src); Cyc_String_strncpy( dest, 0, src, 0, len); if( len !=({ struct
_tagged_string _temp155= dest;( unsigned int)( _temp155.last_plus_one - _temp155.curr);})){({
struct _tagged_string _temp156= dest; char* _temp158= _temp156.curr +( int) len;
if( _temp156.base == 0? 1:( _temp158 < _temp156.base? 1: _temp158 >= _temp156.last_plus_one)){
_throw( Null_Exception);}* _temp158='\000';});} return dest;} struct
_tagged_string Cyc_String_strdup( struct _tagged_string src){ unsigned int len;
struct _tagged_string temp; len= Cyc_String_strlen( src); temp= Cyc_Core_new_string((
int) len); Cyc_String_strncpy( temp, 0, src, 0, len); return temp;} struct
_tagged_string Cyc_String_expand( struct _tagged_string s, unsigned int sz){
struct _tagged_string temp; unsigned int slen; slen= Cyc_String_strlen( s); sz=
sz > slen? sz: slen; temp= Cyc_Core_new_string(( int) sz); Cyc_String_strncpy(
temp, 0, s, 0, slen); if( slen !=({ struct _tagged_string _temp159= s;(
unsigned int)( _temp159.last_plus_one - _temp159.curr);})){({ struct
_tagged_string _temp160= temp; char* _temp162= _temp160.curr +( int) slen; if(
_temp160.base == 0? 1:( _temp162 < _temp160.base? 1: _temp162 >= _temp160.last_plus_one)){
_throw( Null_Exception);}* _temp162='\000';});} return temp;} struct
_tagged_string Cyc_String_realloc_str( struct _tagged_string str, unsigned int
sz){ unsigned int maxsizeP=({ struct _tagged_string _temp163= str;( unsigned int)(
_temp163.last_plus_one - _temp163.curr);}); if( maxsizeP ==( unsigned int) 0){
maxsizeP=( unsigned int) 30 > sz? 30: sz; str= Cyc_Core_new_string(( int)
maxsizeP);({ struct _tagged_string _temp164= str; char* _temp166= _temp164.curr
+ 0; if( _temp164.base == 0? 1:( _temp166 < _temp164.base? 1: _temp166 >=
_temp164.last_plus_one)){ _throw( Null_Exception);}* _temp166='\000';});} else{
if( sz > maxsizeP){ maxsizeP= maxsizeP *( unsigned int) 2 >( sz *( unsigned int)
5) /( unsigned int) 4? maxsizeP *( unsigned int) 2:( sz *( unsigned int) 5) /(
unsigned int) 4; str= Cyc_String_expand( str, maxsizeP);}} return str;} struct
_tagged_string Cyc_String_substring( struct _tagged_string s, int start,
unsigned int amt){ if(( start < 0? 1: amt <( unsigned int) 0)? 1:( start +( int)
amt) - 1 >( int)({ struct _tagged_string _temp167= s;( unsigned int)( _temp167.last_plus_one
- _temp167.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp168=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp168[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp169; _temp169.tag= Cyc_Core_InvalidArg_tag;
_temp169.f1=( struct _tagged_string)({ char* _temp170=( char*)"String::substring";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 18; _temp171;}); _temp169;}); _temp168;}));}{
struct _tagged_string ans= Cyc_Core_new_string(( int)( amt +( unsigned int) 1));{
unsigned int i=( unsigned int) 0; for( 0; i < amt; ++ i){({ struct
_tagged_string _temp172= ans; char* _temp174= _temp172.curr +( int) i; if(
_temp172.base == 0? 1:( _temp174 < _temp172.base? 1: _temp174 >= _temp172.last_plus_one)){
_throw( Null_Exception);}* _temp174=({ struct _tagged_string _temp175= s; char*
_temp177= _temp175.curr +( int)(( unsigned int) start + i); if( _temp175.base ==
0? 1:( _temp177 < _temp175.base? 1: _temp177 >= _temp175.last_plus_one)){ _throw(
Null_Exception);}* _temp177;});});}}({ struct _tagged_string _temp178= ans; char*
_temp180= _temp178.curr +( int) amt; if( _temp178.base == 0? 1:( _temp180 <
_temp178.base? 1: _temp180 >= _temp178.last_plus_one)){ _throw( Null_Exception);}*
_temp180='\000';}); return ans;}} struct _tagged_string Cyc_String_replace_suffix(
struct _tagged_string src, struct _tagged_string curr_suffix, struct
_tagged_string new_suffix){ unsigned int m=({ struct _tagged_string _temp181=
src;( unsigned int)( _temp181.last_plus_one - _temp181.curr);}); unsigned int n=({
struct _tagged_string _temp182= curr_suffix;( unsigned int)( _temp182.last_plus_one
- _temp182.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp183=( char*)"String::replace_suffix"; struct _tagged_string _temp184;
_temp184.curr= _temp183; _temp184.base= _temp183; _temp184.last_plus_one=
_temp183 + 23; _temp184;}); if( m < n){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp185=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp185[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp186; _temp186.tag= Cyc_Core_InvalidArg_tag;
_temp186.f1= err; _temp186;}); _temp185;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(( int)({ struct _tagged_string _temp187= src; char*
_temp189= _temp187.curr +( int)( m - i); if( _temp187.base == 0? 1:( _temp189 <
_temp187.base? 1: _temp189 >= _temp187.last_plus_one)){ _throw( Null_Exception);}*
_temp189;}) !=( int)({ struct _tagged_string _temp190= curr_suffix; char*
_temp192= _temp190.curr +( int)( n - i); if( _temp190.base == 0? 1:( _temp192 <
_temp190.base? 1: _temp192 >= _temp190.last_plus_one)){ _throw( Null_Exception);}*
_temp192;})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp193=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp193[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp194; _temp194.tag= Cyc_Core_InvalidArg_tag;
_temp194.f1= err; _temp194;}); _temp193;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp195= new_suffix;( unsigned int)(
_temp195.last_plus_one - _temp195.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp196= new_suffix;( unsigned int)( _temp196.last_plus_one -
_temp196.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp197=( char*)"String::strpbrk"; struct
_tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 16; _temp198;})); char c='\000'; for( 0; len
> 0?( int)( c=({ struct _tagged_string _temp199= s; char* _temp200= _temp199.curr;
if( _temp199.base == 0? 1:( _temp200 < _temp199.base? 1: _temp200 >= _temp199.last_plus_one)){
_throw( Null_Exception);}* _temp200;})) != 0: 0; len --,({ struct _tagged_string*
_temp201=& s; struct _tagged_string _temp202=* _temp201; _temp201->curr ++;
_temp202;})){ int j= 0; for( 0;( unsigned int) j <({ struct _tagged_string
_temp203= accept;( unsigned int)( _temp203.last_plus_one - _temp203.curr);}); j
++){ if(( int) c ==( int)({ struct _tagged_string _temp204= accept; char*
_temp206= _temp204.curr + j; if( _temp204.base == 0? 1:( _temp206 < _temp204.base?
1: _temp206 >= _temp204.last_plus_one)){ _throw( Null_Exception);}* _temp206;})){
return s;}}} return({ struct _tagged_string _temp207={ 0, 0, 0}; _temp207;});}
struct _tagged_string Cyc_String_strchr( struct _tagged_string s, char c){ int
len=( int) Cyc_String_int_strleno( s, 0,( struct _tagged_string)({ char*
_temp208=( char*)"String::strchr"; struct _tagged_string _temp209; _temp209.curr=
_temp208; _temp209.base= _temp208; _temp209.last_plus_one= _temp208 + 15;
_temp209;})); char c2='\000'; for( 0; len > 0?( int)( c2=({ struct
_tagged_string _temp210= s; char* _temp211= _temp210.curr; if( _temp210.base ==
0? 1:( _temp211 < _temp210.base? 1: _temp211 >= _temp210.last_plus_one)){ _throw(
Null_Exception);}* _temp211;})) != 0: 0; len --,({ struct _tagged_string*
_temp212=& s; struct _tagged_string _temp213=* _temp212; _temp212->curr ++;
_temp213;})){ if(( int) c2 ==( int) c){ return s;}} return({ struct
_tagged_string _temp214={ 0, 0, 0}; _temp214;});} struct _tagged_string Cyc_String_strrchr(
struct _tagged_string s, char c){ int len=( int) Cyc_String_int_strleno( s, 0,(
struct _tagged_string)({ char* _temp215=( char*)"String::strrchr"; struct
_tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 16; _temp216;})); int i= len - 1; s.curr += i;
for( 0; i >= 0; i --,({ struct _tagged_string* _temp217=& s; struct
_tagged_string _temp218=* _temp217; _temp217->curr --; _temp218;})){ if(( int)({
struct _tagged_string _temp219= s; char* _temp220= _temp219.curr; if( _temp219.base
== 0? 1:( _temp220 < _temp219.base? 1: _temp220 >= _temp219.last_plus_one)){
_throw( Null_Exception);}* _temp220;}) ==( int) c){ return s;}} return({ struct
_tagged_string _temp221={ 0, 0, 0}; _temp221;});} unsigned int Cyc_String_strspn(
struct _tagged_string s, struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp222=( char*)"String::strspn"; struct
_tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 15; _temp223;}));{ unsigned int i=(
unsigned int) 0; for( 0; i < len; i ++){ int j; for( j= 0;( unsigned int) j <({
struct _tagged_string _temp224= accept;( unsigned int)( _temp224.last_plus_one -
_temp224.curr);}); j ++){ if(( int)({ struct _tagged_string _temp225= s; char*
_temp227= _temp225.curr +( int) i; if( _temp225.base == 0? 1:( _temp227 <
_temp225.base? 1: _temp227 >= _temp225.last_plus_one)){ _throw( Null_Exception);}*
_temp227;}) ==( int)({ struct _tagged_string _temp228= accept; char* _temp230=
_temp228.curr + j; if( _temp228.base == 0? 1:( _temp230 < _temp228.base? 1:
_temp230 >= _temp228.last_plus_one)){ _throw( Null_Exception);}* _temp230;})){
break;}} if(( unsigned int) j ==({ struct _tagged_string _temp231= accept;(
unsigned int)( _temp231.last_plus_one - _temp231.curr);})){ return i;}}} return
len;} struct Cyc_List_List* Cyc_String_explode( struct _tagged_string s){ struct
Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen( s) -( unsigned int)
1); for( 0; i >= 0; i --){ result=({ struct Cyc_List_List* _temp232=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp232->hd=( void*)(( int)({ struct
_tagged_string _temp233= s; char* _temp235= _temp233.curr + i; if( _temp233.base
== 0? 1:( _temp235 < _temp233.base? 1: _temp235 >= _temp233.last_plus_one)){
_throw( Null_Exception);}* _temp235;})); _temp232->tl= result; _temp232;});}}
return result;} struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
chars){ struct _tagged_string s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); unsigned int i=( unsigned int) 0; while( chars !=
0) {({ struct _tagged_string _temp236= s; char* _temp238= _temp236.curr +( int)(
i ++); if( _temp236.base == 0? 1:( _temp238 < _temp236.base? 1: _temp238 >=
_temp236.last_plus_one)){ _throw( Null_Exception);}* _temp238=( char)(( int)({
struct Cyc_List_List* _temp239= chars; if( _temp239 == 0){ _throw(
Null_Exception);} _temp239->hd;}));}); chars=({ struct Cyc_List_List* _temp240=
chars; if( _temp240 == 0){ _throw( Null_Exception);} _temp240->tl;});} return s;}
int Cyc_String_to_int( struct _tagged_string s, int* offset){ int ans= 0; int sn=
1; int digit_seen= 0; int i= offset == 0? 0:({ int* _temp241= offset; if(
_temp241 == 0){ _throw( Null_Exception);}* _temp241;}); while(( unsigned int) i
<({ struct _tagged_string _temp242= s;( unsigned int)( _temp242.last_plus_one -
_temp242.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp243= s;
char* _temp245= _temp243.curr + i; if( _temp243.base == 0? 1:( _temp245 <
_temp243.base? 1: _temp245 >= _temp243.last_plus_one)){ _throw( Null_Exception);}*
_temp245;})): 0) { i ++;} if(( unsigned int) i <({ struct _tagged_string
_temp246= s;( unsigned int)( _temp246.last_plus_one - _temp246.curr);})?( int)({
struct _tagged_string _temp247= s; char* _temp249= _temp247.curr + i; if(
_temp247.base == 0? 1:( _temp249 < _temp247.base? 1: _temp249 >= _temp247.last_plus_one)){
_throw( Null_Exception);}* _temp249;}) ==( int)'-': 0){ i ++; sn= - 1;} while((
unsigned int) i <({ struct _tagged_string _temp250= s;( unsigned int)( _temp250.last_plus_one
- _temp250.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp251= s;
char* _temp253= _temp251.curr + i; if( _temp251.base == 0? 1:( _temp253 <
_temp251.base? 1: _temp253 >= _temp251.last_plus_one)){ _throw( Null_Exception);}*
_temp253;})): 0) { i ++;} while(( unsigned int) i <({ struct _tagged_string
_temp254= s;( unsigned int)( _temp254.last_plus_one - _temp254.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp255= s; char* _temp257= _temp255.curr + i; if(
_temp255.base == 0? 1:( _temp257 < _temp255.base? 1: _temp257 >= _temp255.last_plus_one)){
_throw( Null_Exception);}* _temp257;})): 0) { digit_seen= 1; ans= ans * 10 +(
int)(( int)({ struct _tagged_string _temp258= s; char* _temp260= _temp258.curr +
i; if( _temp258.base == 0? 1:( _temp260 < _temp258.base? 1: _temp260 >= _temp258.last_plus_one)){
_throw( Null_Exception);}* _temp260;}) -( int)'0'); i ++;} ans= ans * sn; if( !
digit_seen){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp261=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp261[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp262; _temp262.tag= Cyc_Core_InvalidArg_tag;
_temp262.f1=( struct _tagged_string)({ char* _temp263=( char*)"String::to_int";
struct _tagged_string _temp264; _temp264.curr= _temp263; _temp264.base= _temp263;
_temp264.last_plus_one= _temp263 + 15; _temp264;}); _temp262;}); _temp261;}));}
if( offset != 0){({ int* _temp265= offset; if( _temp265 == 0){ _throw(
Null_Exception);}* _temp265= i;});} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:({ int* _temp266= offset; if( _temp266 == 0){
_throw( Null_Exception);}* _temp266;}); while(( unsigned int) i <({ struct
_tagged_string _temp267= s;( unsigned int)( _temp267.last_plus_one - _temp267.curr);})?
Cyc_Character_isspace(({ struct _tagged_string _temp268= s; char* _temp270=
_temp268.curr + i; if( _temp268.base == 0? 1:( _temp270 < _temp268.base? 1:
_temp270 >= _temp268.last_plus_one)){ _throw( Null_Exception);}* _temp270;})): 0) {
i ++;} if(( unsigned int) i <({ struct _tagged_string _temp271= s;( unsigned int)(
_temp271.last_plus_one - _temp271.curr);})?( int)({ struct _tagged_string
_temp272= s; char* _temp274= _temp272.curr + i; if( _temp272.base == 0? 1:(
_temp274 < _temp272.base? 1: _temp274 >= _temp272.last_plus_one)){ _throw(
Null_Exception);}* _temp274;}) ==( int)'-': 0){ i ++; sn= - 1;} while((
unsigned int) i <({ struct _tagged_string _temp275= s;( unsigned int)( _temp275.last_plus_one
- _temp275.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp276= s;
char* _temp278= _temp276.curr + i; if( _temp276.base == 0? 1:( _temp278 <
_temp276.base? 1: _temp278 >= _temp276.last_plus_one)){ _throw( Null_Exception);}*
_temp278;})): 0) { digit_seen= 1; ans= ans *( double) 10.0 +( double)(( int)({
struct _tagged_string _temp279= s; char* _temp281= _temp279.curr + i; if(
_temp279.base == 0? 1:( _temp281 < _temp279.base? 1: _temp281 >= _temp279.last_plus_one)){
_throw( Null_Exception);}* _temp281;}) -( int)'0'); i ++;} if(( unsigned int) i
<({ struct _tagged_string _temp282= s;( unsigned int)( _temp282.last_plus_one -
_temp282.curr);})?( int)({ struct _tagged_string _temp283= s; char* _temp285=
_temp283.curr + i; if( _temp283.base == 0? 1:( _temp285 < _temp283.base? 1:
_temp285 >= _temp283.last_plus_one)){ _throw( Null_Exception);}* _temp285;}) ==(
int)'.': 0){ i ++;{ double divisor=( double) 0.1; while(( unsigned int) i <({
struct _tagged_string _temp286= s;( unsigned int)( _temp286.last_plus_one -
_temp286.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp287= s;
char* _temp289= _temp287.curr + i; if( _temp287.base == 0? 1:( _temp289 <
_temp287.base? 1: _temp289 >= _temp287.last_plus_one)){ _throw( Null_Exception);}*
_temp289;})): 0) { digit_seen= 1; ans= ans +( double)(( int)({ struct
_tagged_string _temp290= s; char* _temp292= _temp290.curr + i; if( _temp290.base
== 0? 1:( _temp292 < _temp290.base? 1: _temp292 >= _temp290.last_plus_one)){
_throw( Null_Exception);}* _temp292;}) -( int)'0') * divisor; divisor= divisor /(
double) 10.0; i ++;}}} if( ! digit_seen){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp293=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp293[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp294; _temp294.tag= Cyc_Core_InvalidArg_tag;
_temp294.f1=( struct _tagged_string)({ char* _temp295=( char*)"String::to_double";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 18; _temp296;}); _temp294;}); _temp293;}));}
if(( unsigned int) i <({ struct _tagged_string _temp297= s;( unsigned int)(
_temp297.last_plus_one - _temp297.curr);})?( int)({ struct _tagged_string
_temp298= s; char* _temp300= _temp298.curr + i; if( _temp298.base == 0? 1:(
_temp300 < _temp298.base? 1: _temp300 >= _temp298.last_plus_one)){ _throw(
Null_Exception);}* _temp300;}) ==( int)'e'? 1:( int)({ struct _tagged_string
_temp301= s; char* _temp303= _temp301.curr + i; if( _temp301.base == 0? 1:(
_temp303 < _temp301.base? 1: _temp303 >= _temp301.last_plus_one)){ _throw(
Null_Exception);}* _temp303;}) ==( int)'E': 0){ i ++;{ int exponent= 0; int
exp_sign= 1; digit_seen= 0; if(( unsigned int) i <({ struct _tagged_string
_temp304= s;( unsigned int)( _temp304.last_plus_one - _temp304.curr);})?( int)({
struct _tagged_string _temp305= s; char* _temp307= _temp305.curr + i; if(
_temp305.base == 0? 1:( _temp307 < _temp305.base? 1: _temp307 >= _temp305.last_plus_one)){
_throw( Null_Exception);}* _temp307;}) ==( int)'-': 0){ i ++; exp_sign= - 1;}
else{ if(( unsigned int) i <({ struct _tagged_string _temp308= s;( unsigned int)(
_temp308.last_plus_one - _temp308.curr);})?( int)({ struct _tagged_string
_temp309= s; char* _temp311= _temp309.curr + i; if( _temp309.base == 0? 1:(
_temp311 < _temp309.base? 1: _temp311 >= _temp309.last_plus_one)){ _throw(
Null_Exception);}* _temp311;}) ==( int)'+': 0){ i ++;}} while(( unsigned int) i
<({ struct _tagged_string _temp312= s;( unsigned int)( _temp312.last_plus_one -
_temp312.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp313= s;
char* _temp315= _temp313.curr + i; if( _temp313.base == 0? 1:( _temp315 <
_temp313.base? 1: _temp315 >= _temp313.last_plus_one)){ _throw( Null_Exception);}*
_temp315;})): 0) { digit_seen= 1; exponent= exponent * 10 +( int)(( int)({
struct _tagged_string _temp316= s; char* _temp318= _temp316.curr + i; if(
_temp316.base == 0? 1:( _temp318 < _temp316.base? 1: _temp318 >= _temp316.last_plus_one)){
_throw( Null_Exception);}* _temp318;}) -( int)'0'); i ++;} if( ! digit_seen){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp319=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp319[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp320; _temp320.tag= Cyc_Core_InvalidArg_tag;
_temp320.f1=( struct _tagged_string)({ char* _temp321=( char*)"String::to_double";
struct _tagged_string _temp322; _temp322.curr= _temp321; _temp322.base= _temp321;
_temp322.last_plus_one= _temp321 + 18; _temp322;}); _temp320;}); _temp319;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){({ int* _temp323= offset; if( _temp323 == 0){ _throw( Null_Exception);}*
_temp323= i;});} return ans *( double) sn;}