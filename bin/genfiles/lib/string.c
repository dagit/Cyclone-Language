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
- _temp0.curr);}); i ++){ if(({ struct _tagged_string _temp1= s; char* _temp3=
_temp1.curr +( int) i; if( _temp1.base == 0? 1:( _temp3 < _temp1.base? 1: _temp3
>= _temp1.last_plus_one)){ _throw( Null_Exception);}* _temp3;}) =='\000'){
return i;}} return i;} static unsigned int Cyc_String_int_strleno( struct
_tagged_string s, int ofs, struct _tagged_string error){ if( ofs >({ struct
_tagged_string _temp4= s;( unsigned int)( _temp4.last_plus_one - _temp4.curr);})?
1: ofs < 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp5=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp5[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp6; _temp6.tag= Cyc_Core_InvalidArg_tag;
_temp6.f1= error; _temp6;}); _temp5;}));}{ int i; for( i= ofs; i <({ struct
_tagged_string _temp7= s;( unsigned int)( _temp7.last_plus_one - _temp7.curr);});
i ++){ if(({ struct _tagged_string _temp8= s; char* _temp10= _temp8.curr + i;
if( _temp8.base == 0? 1:( _temp10 < _temp8.base? 1: _temp10 >= _temp8.last_plus_one)){
_throw( Null_Exception);}* _temp10;}) =='\000'){ break;}} return( unsigned int)(
i - ofs);}} static int Cyc_String_case_cmp( char c1, char c2){ return c1 - c2;}
static int Cyc_String_nocase_cmp( char c1, char c2){ return Cyc_String_case_cmp(
Cyc_Character_toupper( c1), Cyc_Character_toupper( c2));} static int Cyc_String_cmp(
struct _tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2,
int ofs2, unsigned int len2, int(* f)( char, char)){ unsigned int min_length=
len1 < len2? len1: len2; int i= - 1; while(( ++ i) < min_length) { int diff= f(({
struct _tagged_string _temp11= s1; char* _temp13= _temp11.curr +( i + ofs1); if(
_temp11.base == 0? 1:( _temp13 < _temp11.base? 1: _temp13 >= _temp11.last_plus_one)){
_throw( Null_Exception);}* _temp13;}),({ struct _tagged_string _temp14= s2; char*
_temp16= _temp14.curr +( i + ofs2); if( _temp14.base == 0? 1:( _temp16 < _temp14.base?
1: _temp16 >= _temp14.last_plus_one)){ _throw( Null_Exception);}* _temp16;}));
if( diff != 0){ return diff;}} return( int) len1 -( int) len2;} int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2){ if( s1.curr == s2.curr){
return 0;}{ unsigned int len1= Cyc_String_int_strleno( s1, 0,( struct
_tagged_string)({ char* _temp19=( char*)"String::strcmp"; struct _tagged_string
_temp20; _temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one=
_temp19 + 15; _temp20;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,(
struct _tagged_string)({ char* _temp17=( char*)"String::strcmp"; struct
_tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one=
_temp17 + 15; _temp18;})); return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}}
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2){
return Cyc_String_strcmp(* s1,* s2);} static int Cyc_String_ncmp( struct
_tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2, int
ofs2, unsigned int len2, unsigned int n, int(* f)( char, char)){ if( n <= 0){
return 0;}{ unsigned int min_len= len1 > len2? len2: len1; unsigned int bound=
min_len > n? n: min_len;{ int i= 0; for( 0; i < bound; i ++){ int retc; if((
retc= f(({ struct _tagged_string _temp21= s1; char* _temp23= _temp21.curr +( i +
ofs1); if( _temp21.base == 0? 1:( _temp23 < _temp21.base? 1: _temp23 >= _temp21.last_plus_one)){
_throw( Null_Exception);}* _temp23;}),({ struct _tagged_string _temp24= s2; char*
_temp26= _temp24.curr +( i + ofs2); if( _temp24.base == 0? 1:( _temp26 < _temp24.base?
1: _temp26 >= _temp24.last_plus_one)){ _throw( Null_Exception);}* _temp26;})))
!= 0){ return retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_string
s1, struct _tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp29=( char*)"String::strncmp"; struct
_tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29; _temp30.last_plus_one=
_temp29 + 16; _temp30;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,(
struct _tagged_string)({ char* _temp27=( char*)"String::strncmp"; struct
_tagged_string _temp28; _temp28.curr= _temp27; _temp28.base= _temp27; _temp28.last_plus_one=
_temp27 + 16; _temp28;})); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n,
Cyc_String_case_cmp);} int Cyc_String_strncasecmp( struct _tagged_string s1,
struct _tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp33=( char*)"String::strncasecmp";
struct _tagged_string _temp34; _temp34.curr= _temp33; _temp34.base= _temp33;
_temp34.last_plus_one= _temp33 + 20; _temp34;})); unsigned int len2= Cyc_String_int_strleno(
s2, 0,( struct _tagged_string)({ char* _temp31=( char*)"String::strncasecmp";
struct _tagged_string _temp32; _temp32.curr= _temp31; _temp32.base= _temp31;
_temp32.last_plus_one= _temp31 + 20; _temp32;})); return Cyc_String_ncmp( s1, 0,
len1, s2, 0, len2, n, Cyc_String_nocase_cmp);} int Cyc_String_zstrcmp( struct
_tagged_string a, struct _tagged_string b){ if( a.curr == b.curr){ return 0;}{
unsigned int as=({ struct _tagged_string _temp42= a;( unsigned int)( _temp42.last_plus_one
- _temp42.curr);}); unsigned int bs=({ struct _tagged_string _temp41= b;(
unsigned int)( _temp41.last_plus_one - _temp41.curr);}); unsigned int min_length=
as < bs? as: bs; int i= - 1; while(( ++ i) < min_length) { int diff=( int)({
struct _tagged_string _temp35= a; char* _temp37= _temp35.curr + i; if( _temp35.base
== 0? 1:( _temp37 < _temp35.base? 1: _temp37 >= _temp35.last_plus_one)){ _throw(
Null_Exception);}* _temp37;}) -( int)({ struct _tagged_string _temp38= b; char*
_temp40= _temp38.curr + i; if( _temp38.base == 0? 1:( _temp40 < _temp38.base? 1:
_temp40 >= _temp38.last_plus_one)){ _throw( Null_Exception);}* _temp40;}); if(
diff != 0){ return diff;}} return( int) as -( int) bs;}} int Cyc_String_zstrncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int n){ if( n <= 0){
return 0;}{ unsigned int s1size=({ struct _tagged_string _temp56= s1;(
unsigned int)( _temp56.last_plus_one - _temp56.curr);}); unsigned int s2size=({
struct _tagged_string _temp55= s2;( unsigned int)( _temp55.last_plus_one -
_temp55.curr);}); unsigned int min_size= s1size > s2size? s2size: s1size;
unsigned int bound= min_size > n? n: min_size;{ int i= 0; for( 0; i < bound; i
++){ if(({ struct _tagged_string _temp43= s1; char* _temp45= _temp43.curr + i;
if( _temp43.base == 0? 1:( _temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one)){
_throw( Null_Exception);}* _temp45;}) <({ struct _tagged_string _temp46= s2;
char* _temp48= _temp46.curr + i; if( _temp46.base == 0? 1:( _temp48 < _temp46.base?
1: _temp48 >= _temp46.last_plus_one)){ _throw( Null_Exception);}* _temp48;})){
return - 1;} else{ if(({ struct _tagged_string _temp49= s2; char* _temp51=
_temp49.curr + i; if( _temp49.base == 0? 1:( _temp51 < _temp49.base? 1: _temp51
>= _temp49.last_plus_one)){ _throw( Null_Exception);}* _temp51;}) <({ struct
_tagged_string _temp52= s1; char* _temp54= _temp52.curr + i; if( _temp52.base ==
0? 1:( _temp54 < _temp52.base? 1: _temp54 >= _temp52.last_plus_one)){ _throw(
Null_Exception);}* _temp54;})){ return 1;}}}} if( min_size <= bound){ return 0;}
if( s1size < s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp(
struct _tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(*
a,* b);} static struct _tagged_string Cyc_String_int_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs, struct
_tagged_string error){ int i; unsigned int dsize; unsigned int slen;
unsigned int dlen; dsize=({ struct _tagged_string _temp57= dest;( unsigned int)(
_temp57.last_plus_one - _temp57.curr);}); dlen= Cyc_String_strlen( dest); slen=
Cyc_String_int_strleno( src, srcOfs, error); if( slen + dlen <= dsize){ for( i=
0; i < slen; i ++){({ struct _tagged_string _temp58= dest; char* _temp60=
_temp58.curr +( int)( i + dlen); if( _temp58.base == 0? 1:( _temp60 < _temp58.base?
1: _temp60 >= _temp58.last_plus_one)){ _throw( Null_Exception);}* _temp60=({
struct _tagged_string _temp61= src; char* _temp63= _temp61.curr +( i + srcOfs);
if( _temp61.base == 0? 1:( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one)){
_throw( Null_Exception);}* _temp63;});});} if( i != dsize){({ struct
_tagged_string _temp64= dest; char* _temp66= _temp64.curr +( int)( i + dlen);
if( _temp64.base == 0? 1:( _temp66 < _temp64.base? 1: _temp66 >= _temp64.last_plus_one)){
_throw( Null_Exception);}* _temp66='\000';});}} else{( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp67=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp67[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp68; _temp68.tag= Cyc_Core_InvalidArg_tag;
_temp68.f1= error; _temp68;}); _temp67;}));} return dest;} struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src){
return Cyc_String_int_strcato( dest, src, 0,( struct _tagged_string)({ char*
_temp69=( char*)"String::strcat"; struct _tagged_string _temp70; _temp70.curr=
_temp69; _temp70.base= _temp69; _temp70.last_plus_one= _temp69 + 15; _temp70;}));}
struct _tagged_string Cyc_String_strconcat( struct _tagged_string a, struct
_tagged_string b){ unsigned int alen= Cyc_String_strlen( a); unsigned int blen=
Cyc_String_strlen( b); struct _tagged_string ans= Cyc_Core_new_string(( int)(
alen + blen)); int i; int j; for( i= 0; i < alen; ++ i){({ struct _tagged_string
_temp71= ans; char* _temp73= _temp71.curr + i; if( _temp71.base == 0? 1:(
_temp73 < _temp71.base? 1: _temp73 >= _temp71.last_plus_one)){ _throw(
Null_Exception);}* _temp73=({ struct _tagged_string _temp74= a; char* _temp76=
_temp74.curr + i; if( _temp74.base == 0? 1:( _temp76 < _temp74.base? 1: _temp76
>= _temp74.last_plus_one)){ _throw( Null_Exception);}* _temp76;});});} for( j= 0;
j < blen; ++ j){({ struct _tagged_string _temp77= ans; char* _temp79= _temp77.curr
+( i + j); if( _temp77.base == 0? 1:( _temp79 < _temp77.base? 1: _temp79 >=
_temp77.last_plus_one)){ _throw( Null_Exception);}* _temp79=({ struct
_tagged_string _temp80= b; char* _temp82= _temp80.curr + j; if( _temp80.base ==
0? 1:( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one)){ _throw(
Null_Exception);}* _temp82;});});} return ans;} struct _tagged_string Cyc_String_strconcat_l(
struct Cyc_List_List* strs){ unsigned int len; unsigned int total_len=(
unsigned int) 0; struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp93=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp93->hd=(
void*)(( unsigned int) 0); _temp93->tl= 0; _temp93;}); struct Cyc_List_List* end=
lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p=({ struct Cyc_List_List*
_temp83= p; if( _temp83 == 0){ _throw( Null_Exception);} _temp83->tl;})){ len=
Cyc_String_strlen(*(( struct _tagged_string*)({ struct Cyc_List_List* _temp84= p;
if( _temp84 == 0){ _throw( Null_Exception);} _temp84->hd;}))); total_len += len;({
struct Cyc_List_List* _temp85= end; if( _temp85 == 0){ _throw( Null_Exception);}
_temp85->tl=({ struct Cyc_List_List* _temp86=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp86->hd=( void*) len; _temp86->tl= 0;
_temp86;});}); end=({ struct Cyc_List_List* _temp87= end; if( _temp87 == 0){
_throw( Null_Exception);} _temp87->tl;});}} lens=({ struct Cyc_List_List*
_temp88= lens; if( _temp88 == 0){ _throw( Null_Exception);} _temp88->tl;});{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*)({ struct Cyc_List_List* _temp92= strs; if( _temp92 == 0){
_throw( Null_Exception);} _temp92->hd;})); len=( unsigned int)({ struct Cyc_List_List*
_temp89= lens; if( _temp89 == 0){ _throw( Null_Exception);} _temp89->hd;}); Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs=({ struct Cyc_List_List* _temp90=
strs; if( _temp90 == 0){ _throw( Null_Exception);} _temp90->tl;}); lens=({
struct Cyc_List_List* _temp91= lens; if( _temp91 == 0){ _throw( Null_Exception);}
_temp91->tl;});} return ans;}} struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List* strs, struct _tagged_string separator){ if( strs == 0){
return( struct _tagged_string)({ char* _temp94=( char*)""; struct _tagged_string
_temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 1; _temp95;});} if(({ struct Cyc_List_List* _temp96= strs; if( _temp96
== 0){ _throw( Null_Exception);} _temp96->tl;}) == 0){ return Cyc_String_strdup(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp97= strs; if( _temp97 == 0){
_throw( Null_Exception);} _temp97->hd;})));}{ struct Cyc_List_List* p= strs;
struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp111->hd=( void*)(( unsigned int)
0); _temp111->tl= 0; _temp111;}); struct Cyc_List_List* end= lens; unsigned int
len=( unsigned int) 0; unsigned int total_len=( unsigned int) 0; unsigned int
list_len=( unsigned int) 0; for( 0; p != 0; p=({ struct Cyc_List_List* _temp98=
p; if( _temp98 == 0){ _throw( Null_Exception);} _temp98->tl;})){ len= Cyc_String_strlen(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp99= p; if( _temp99 == 0){
_throw( Null_Exception);} _temp99->hd;}))); total_len += len;({ struct Cyc_List_List*
_temp100= end; if( _temp100 == 0){ _throw( Null_Exception);} _temp100->tl=({
struct Cyc_List_List* _temp101=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp101->hd=( void*) len; _temp101->tl= 0; _temp101;});});
end=({ struct Cyc_List_List* _temp102= end; if( _temp102 == 0){ _throw(
Null_Exception);} _temp102->tl;}); ++ list_len;} lens=({ struct Cyc_List_List*
_temp103= lens; if( _temp103 == 0){ _throw( Null_Exception);} _temp103->tl;});{
unsigned int seplen= Cyc_String_strlen( separator); total_len +=( list_len - 1)
* seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int) total_len);
unsigned int i=( unsigned int) 0; while(({ struct Cyc_List_List* _temp104= strs;
if( _temp104 == 0){ _throw( Null_Exception);} _temp104->tl;}) != 0) { struct
_tagged_string next=*(( struct _tagged_string*)({ struct Cyc_List_List* _temp108=
strs; if( _temp108 == 0){ _throw( Null_Exception);} _temp108->hd;})); len=(
unsigned int)({ struct Cyc_List_List* _temp105= lens; if( _temp105 == 0){ _throw(
Null_Exception);} _temp105->hd;}); Cyc_String_strncpy( ans,( int) i, next, 0,
len); i += len; Cyc_String_strncpy( ans,( int) i, separator, 0, seplen); i +=
seplen; strs=({ struct Cyc_List_List* _temp106= strs; if( _temp106 == 0){ _throw(
Null_Exception);} _temp106->tl;}); lens=({ struct Cyc_List_List* _temp107= lens;
if( _temp107 == 0){ _throw( Null_Exception);} _temp107->tl;});} Cyc_String_strncpy(
ans,( int) i,*(( struct _tagged_string*)({ struct Cyc_List_List* _temp109= strs;
if( _temp109 == 0){ _throw( Null_Exception);} _temp109->hd;})), 0,( unsigned int)({
struct Cyc_List_List* _temp110= lens; if( _temp110 == 0){ _throw( Null_Exception);}
_temp110->hd;})); return ans;}}}} struct _tagged_string Cyc_String_strncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int destSz=({ struct _tagged_string _temp134= dest;(
unsigned int)( _temp134.last_plus_one - _temp134.curr);}); unsigned int srcSz=({
struct _tagged_string _temp133= src;( unsigned int)( _temp133.last_plus_one -
_temp133.curr);}); if( destOfs < 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp112=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp112[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp113; _temp113.tag= Cyc_Core_InvalidArg_tag;
_temp113.f1=( struct _tagged_string)({ char* _temp114=( char*)"String::strncpy";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 16; _temp115;}); _temp113;}); _temp112;}));}
if( srcOfs < 0? 1:( n > 0? srcOfs >= srcSz: 0)){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp116=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp116[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp117; _temp117.tag= Cyc_Core_InvalidArg_tag;
_temp117.f1=( struct _tagged_string)({ char* _temp118=( char*)"String::strncpy";
struct _tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 16; _temp119;}); _temp117;}); _temp116;}));}
if( n <= 0){ return dest;} if( destOfs + n > destSz? 1: srcOfs + n > srcSz){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp120=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp120[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp121; _temp121.tag= Cyc_Core_InvalidArg_tag;
_temp121.f1=( struct _tagged_string)({ char* _temp122=( char*)"String::strncpy";
struct _tagged_string _temp123; _temp123.curr= _temp122; _temp123.base= _temp122;
_temp123.last_plus_one= _temp122 + 16; _temp123;}); _temp121;}); _temp120;}));}{
int i; for( i= 0; i < n; i ++){ char srcChar=({ struct _tagged_string _temp127=
src; char* _temp129= _temp127.curr +( srcOfs + i); if( _temp127.base == 0? 1:(
_temp129 < _temp127.base? 1: _temp129 >= _temp127.last_plus_one)){ _throw(
Null_Exception);}* _temp129;}); if( srcChar =='\000'){ break;}({ struct
_tagged_string _temp124= dest; char* _temp126= _temp124.curr +( destOfs + i);
if( _temp124.base == 0? 1:( _temp126 < _temp124.base? 1: _temp126 >= _temp124.last_plus_one)){
_throw( Null_Exception);}* _temp126= srcChar;});} for( 0; i < n; i ++){({ struct
_tagged_string _temp130= dest; char* _temp132= _temp130.curr +( destOfs + i);
if( _temp130.base == 0? 1:( _temp132 < _temp130.base? 1: _temp132 >= _temp130.last_plus_one)){
_throw( Null_Exception);}* _temp132='\000';});} return dest;}} struct
_tagged_string Cyc_String_zstrncpy( struct _tagged_string dest, int destOfs,
struct _tagged_string src, int srcOfs, unsigned int n){ unsigned int destSz=({
struct _tagged_string _temp154= dest;( unsigned int)( _temp154.last_plus_one -
_temp154.curr);}); unsigned int srcSz=({ struct _tagged_string _temp153= src;(
unsigned int)( _temp153.last_plus_one - _temp153.curr);}); if( destOfs < 0){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp135=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp135[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp136; _temp136.tag= Cyc_Core_InvalidArg_tag;
_temp136.f1=( struct _tagged_string)({ char* _temp137=( char*)"String::zstrncpy";
struct _tagged_string _temp138; _temp138.curr= _temp137; _temp138.base= _temp137;
_temp138.last_plus_one= _temp137 + 17; _temp138;}); _temp136;}); _temp135;}));}
if( srcOfs < 0? 1:( n > 0? srcOfs >= srcSz: 0)){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp139=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp139[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp140; _temp140.tag= Cyc_Core_InvalidArg_tag;
_temp140.f1=( struct _tagged_string)({ char* _temp141=( char*)"String::zstrncpy";
struct _tagged_string _temp142; _temp142.curr= _temp141; _temp142.base= _temp141;
_temp142.last_plus_one= _temp141 + 17; _temp142;}); _temp140;}); _temp139;}));}
if( n <= 0){ return dest;} if( destOfs + n > destSz? 1: srcOfs + n > srcSz){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp143=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp143[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp144; _temp144.tag= Cyc_Core_InvalidArg_tag;
_temp144.f1=( struct _tagged_string)({ char* _temp145=( char*)"String::zstrncpy";
struct _tagged_string _temp146; _temp146.curr= _temp145; _temp146.base= _temp145;
_temp146.last_plus_one= _temp145 + 17; _temp146;}); _temp144;}); _temp143;}));}{
int i; for( i= 0; i < n; i ++){({ struct _tagged_string _temp147= dest; char*
_temp149= _temp147.curr +( destOfs + i); if( _temp147.base == 0? 1:( _temp149 <
_temp147.base? 1: _temp149 >= _temp147.last_plus_one)){ _throw( Null_Exception);}*
_temp149=({ struct _tagged_string _temp150= src; char* _temp152= _temp150.curr +(
srcOfs + i); if( _temp150.base == 0? 1:( _temp152 < _temp150.base? 1: _temp152
>= _temp150.last_plus_one)){ _throw( Null_Exception);}* _temp152;});});} return
dest;}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp155= dest;(
unsigned int)( _temp155.last_plus_one - _temp155.curr);})){({ struct
_tagged_string _temp156= dest; char* _temp158= _temp156.curr +( int) len; if(
_temp156.base == 0? 1:( _temp158 < _temp156.base? 1: _temp158 >= _temp156.last_plus_one)){
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
sz){ unsigned int maxsizeP=({ struct _tagged_string _temp166= str;( unsigned int)(
_temp166.last_plus_one - _temp166.curr);}); if( maxsizeP == 0){ maxsizeP= 30 >
sz? 30: sz; str= Cyc_Core_new_string(( int) maxsizeP);({ struct _tagged_string
_temp163= str; char* _temp165= _temp163.curr + 0; if( _temp163.base == 0? 1:(
_temp165 < _temp163.base? 1: _temp165 >= _temp163.last_plus_one)){ _throw(
Null_Exception);}* _temp165='\000';});} else{ if( sz > maxsizeP){ maxsizeP=
maxsizeP * 2 >( sz * 5) / 4? maxsizeP * 2:( sz * 5) / 4; str= Cyc_String_expand(
str, maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring(
struct _tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt <
0)? 1:( start +( int) amt) - 1 >( int)({ struct _tagged_string _temp167= s;(
unsigned int)( _temp167.last_plus_one - _temp167.curr);})){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp168=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp168[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp169; _temp169.tag= Cyc_Core_InvalidArg_tag;
_temp169.f1=( struct _tagged_string)({ char* _temp170=( char*)"String::substring";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 18; _temp171;}); _temp169;}); _temp168;}));}{
struct _tagged_string ans= Cyc_Core_new_string(( int)( amt + 1));{ unsigned int
i=( unsigned int) 0; for( 0; i < amt; ++ i){({ struct _tagged_string _temp172=
ans; char* _temp174= _temp172.curr +( int) i; if( _temp172.base == 0? 1:(
_temp174 < _temp172.base? 1: _temp174 >= _temp172.last_plus_one)){ _throw(
Null_Exception);}* _temp174=({ struct _tagged_string _temp175= s; char* _temp177=
_temp175.curr +( int)( start + i); if( _temp175.base == 0? 1:( _temp177 <
_temp175.base? 1: _temp177 >= _temp175.last_plus_one)){ _throw( Null_Exception);}*
_temp177;});});}}({ struct _tagged_string _temp178= ans; char* _temp180=
_temp178.curr +( int) amt; if( _temp178.base == 0? 1:( _temp180 < _temp178.base?
1: _temp180 >= _temp178.last_plus_one)){ _throw( Null_Exception);}* _temp180='\000';});
return ans;}} struct _tagged_string Cyc_String_replace_suffix( struct
_tagged_string src, struct _tagged_string curr_suffix, struct _tagged_string
new_suffix){ unsigned int m=({ struct _tagged_string _temp196= src;(
unsigned int)( _temp196.last_plus_one - _temp196.curr);}); unsigned int n=({
struct _tagged_string _temp195= curr_suffix;( unsigned int)( _temp195.last_plus_one
- _temp195.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp193=( char*)"String::replace_suffix"; struct _tagged_string _temp194;
_temp194.curr= _temp193; _temp194.base= _temp193; _temp194.last_plus_one=
_temp193 + 23; _temp194;}); if( m < n){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp181=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp181[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp182; _temp182.tag= Cyc_Core_InvalidArg_tag;
_temp182.f1= err; _temp182;}); _temp181;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(({ struct _tagged_string _temp183= src; char* _temp185=
_temp183.curr +( int)( m - i); if( _temp183.base == 0? 1:( _temp185 < _temp183.base?
1: _temp185 >= _temp183.last_plus_one)){ _throw( Null_Exception);}* _temp185;})
!=({ struct _tagged_string _temp186= curr_suffix; char* _temp188= _temp186.curr
+( int)( n - i); if( _temp186.base == 0? 1:( _temp188 < _temp186.base? 1:
_temp188 >= _temp186.last_plus_one)){ _throw( Null_Exception);}* _temp188;})){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp189=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp189[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp190; _temp190.tag= Cyc_Core_InvalidArg_tag;
_temp190.f1= err; _temp190;}); _temp189;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp192= new_suffix;( unsigned int)(
_temp192.last_plus_one - _temp192.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp191= new_suffix;( unsigned int)( _temp191.last_plus_one -
_temp191.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp206=( char*)"String::strpbrk"; struct
_tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 16; _temp207;})); char c='\000'; for( 0; len
> 0?( c=({ struct _tagged_string _temp197= s; char* _temp198= _temp197.curr; if(
_temp197.base == 0? 1:( _temp198 < _temp197.base? 1: _temp198 >= _temp197.last_plus_one)){
_throw( Null_Exception);}* _temp198;})) != 0: 0; len --,({ struct _tagged_string*
_temp199=& s; struct _tagged_string _temp200=* _temp199; _temp199->curr ++;
_temp200;})){ int j= 0; for( 0; j <({ struct _tagged_string _temp201= accept;(
unsigned int)( _temp201.last_plus_one - _temp201.curr);}); j ++){ if( c ==({
struct _tagged_string _temp202= accept; char* _temp204= _temp202.curr + j; if(
_temp202.base == 0? 1:( _temp204 < _temp202.base? 1: _temp204 >= _temp202.last_plus_one)){
_throw( Null_Exception);}* _temp204;})){ return s;}}} return({ struct
_tagged_string _temp205={ 0, 0, 0}; _temp205;});} struct _tagged_string Cyc_String_strchr(
struct _tagged_string s, char c){ int len=( int) Cyc_String_int_strleno( s, 0,(
struct _tagged_string)({ char* _temp213=( char*)"String::strchr"; struct
_tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 15; _temp214;})); char c2='\000'; for( 0; len
> 0?( c2=({ struct _tagged_string _temp208= s; char* _temp209= _temp208.curr;
if( _temp208.base == 0? 1:( _temp209 < _temp208.base? 1: _temp209 >= _temp208.last_plus_one)){
_throw( Null_Exception);}* _temp209;})) != 0: 0; len --,({ struct _tagged_string*
_temp210=& s; struct _tagged_string _temp211=* _temp210; _temp210->curr ++;
_temp211;})){ if( c2 == c){ return s;}} return({ struct _tagged_string _temp212={
0, 0, 0}; _temp212;});} struct _tagged_string Cyc_String_strrchr( struct
_tagged_string s, char c){ int len=( int) Cyc_String_int_strleno( s, 0,( struct
_tagged_string)({ char* _temp220=( char*)"String::strrchr"; struct
_tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 16; _temp221;})); int i= len - 1; s.curr += i;
for( 0; i >= 0; i --,({ struct _tagged_string* _temp215=& s; struct
_tagged_string _temp216=* _temp215; _temp215->curr --; _temp216;})){ if(({
struct _tagged_string _temp217= s; char* _temp218= _temp217.curr; if( _temp217.base
== 0? 1:( _temp218 < _temp217.base? 1: _temp218 >= _temp217.last_plus_one)){
_throw( Null_Exception);}* _temp218;}) == c){ return s;}} return({ struct
_tagged_string _temp219={ 0, 0, 0}; _temp219;});} unsigned int Cyc_String_strspn(
struct _tagged_string s, struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp230=( char*)"String::strspn"; struct
_tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 15; _temp231;}));{ unsigned int i=(
unsigned int) 0; for( 0; i < len; i ++){ int j; for( j= 0; j <({ struct
_tagged_string _temp222= accept;( unsigned int)( _temp222.last_plus_one -
_temp222.curr);}); j ++){ if(({ struct _tagged_string _temp223= s; char*
_temp225= _temp223.curr +( int) i; if( _temp223.base == 0? 1:( _temp225 <
_temp223.base? 1: _temp225 >= _temp223.last_plus_one)){ _throw( Null_Exception);}*
_temp225;}) ==({ struct _tagged_string _temp226= accept; char* _temp228=
_temp226.curr + j; if( _temp226.base == 0? 1:( _temp228 < _temp226.base? 1:
_temp228 >= _temp226.last_plus_one)){ _throw( Null_Exception);}* _temp228;})){
break;}} if( j ==({ struct _tagged_string _temp229= accept;( unsigned int)(
_temp229.last_plus_one - _temp229.curr);})){ return i;}}} return len;} struct
Cyc_List_List* Cyc_String_explode( struct _tagged_string s){ struct Cyc_List_List*
result= 0;{ int i=( int)( Cyc_String_strlen( s) - 1); for( 0; i >= 0; i --){
result=({ struct Cyc_List_List* _temp232=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp232->hd=( void*)(( int)({ struct
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
1; int digit_seen= 0; int i= offset == 0? 0:({ int* _temp265= offset; if(
_temp265 == 0){ _throw( Null_Exception);}* _temp265;}); while( i <({ struct
_tagged_string _temp241= s;( unsigned int)( _temp241.last_plus_one - _temp241.curr);})?
Cyc_Character_isspace(({ struct _tagged_string _temp242= s; char* _temp244=
_temp242.curr + i; if( _temp242.base == 0? 1:( _temp244 < _temp242.base? 1:
_temp244 >= _temp242.last_plus_one)){ _throw( Null_Exception);}* _temp244;})): 0) {
i ++;} if( i <({ struct _tagged_string _temp245= s;( unsigned int)( _temp245.last_plus_one
- _temp245.curr);})?({ struct _tagged_string _temp246= s; char* _temp248=
_temp246.curr + i; if( _temp246.base == 0? 1:( _temp248 < _temp246.base? 1:
_temp248 >= _temp246.last_plus_one)){ _throw( Null_Exception);}* _temp248;}) =='-':
0){ i ++; sn= - 1;} while( i <({ struct _tagged_string _temp249= s;(
unsigned int)( _temp249.last_plus_one - _temp249.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp250= s; char* _temp252= _temp250.curr + i; if(
_temp250.base == 0? 1:( _temp252 < _temp250.base? 1: _temp252 >= _temp250.last_plus_one)){
_throw( Null_Exception);}* _temp252;})): 0) { i ++;} while( i <({ struct
_tagged_string _temp253= s;( unsigned int)( _temp253.last_plus_one - _temp253.curr);})?
Cyc_Character_isdigit(({ struct _tagged_string _temp254= s; char* _temp256=
_temp254.curr + i; if( _temp254.base == 0? 1:( _temp256 < _temp254.base? 1:
_temp256 >= _temp254.last_plus_one)){ _throw( Null_Exception);}* _temp256;})): 0) {
digit_seen= 1; ans= ans * 10 +( int)(({ struct _tagged_string _temp257= s; char*
_temp259= _temp257.curr + i; if( _temp257.base == 0? 1:( _temp259 < _temp257.base?
1: _temp259 >= _temp257.last_plus_one)){ _throw( Null_Exception);}* _temp259;})
-'0'); i ++;} ans= ans * sn; if( ! digit_seen){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp260=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp260[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp261; _temp261.tag= Cyc_Core_InvalidArg_tag;
_temp261.f1=( struct _tagged_string)({ char* _temp262=( char*)"String::to_int";
struct _tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 15; _temp263;}); _temp261;}); _temp260;}));}
if( offset != 0){({ int* _temp264= offset; if( _temp264 == 0){ _throw(
Null_Exception);}* _temp264= i;});} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:({ int* _temp323= offset; if( _temp323 == 0){
_throw( Null_Exception);}* _temp323;}); while( i <({ struct _tagged_string
_temp266= s;( unsigned int)( _temp266.last_plus_one - _temp266.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp267= s; char* _temp269= _temp267.curr + i; if(
_temp267.base == 0? 1:( _temp269 < _temp267.base? 1: _temp269 >= _temp267.last_plus_one)){
_throw( Null_Exception);}* _temp269;})): 0) { i ++;} if( i <({ struct
_tagged_string _temp270= s;( unsigned int)( _temp270.last_plus_one - _temp270.curr);})?({
struct _tagged_string _temp271= s; char* _temp273= _temp271.curr + i; if(
_temp271.base == 0? 1:( _temp273 < _temp271.base? 1: _temp273 >= _temp271.last_plus_one)){
_throw( Null_Exception);}* _temp273;}) =='-': 0){ i ++; sn= - 1;} while( i <({
struct _tagged_string _temp274= s;( unsigned int)( _temp274.last_plus_one -
_temp274.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp275= s;
char* _temp277= _temp275.curr + i; if( _temp275.base == 0? 1:( _temp277 <
_temp275.base? 1: _temp277 >= _temp275.last_plus_one)){ _throw( Null_Exception);}*
_temp277;})): 0) { digit_seen= 1; ans= ans * 10.0 +( double)(({ struct
_tagged_string _temp278= s; char* _temp280= _temp278.curr + i; if( _temp278.base
== 0? 1:( _temp280 < _temp278.base? 1: _temp280 >= _temp278.last_plus_one)){
_throw( Null_Exception);}* _temp280;}) -'0'); i ++;} if( i <({ struct
_tagged_string _temp281= s;( unsigned int)( _temp281.last_plus_one - _temp281.curr);})?({
struct _tagged_string _temp282= s; char* _temp284= _temp282.curr + i; if(
_temp282.base == 0? 1:( _temp284 < _temp282.base? 1: _temp284 >= _temp282.last_plus_one)){
_throw( Null_Exception);}* _temp284;}) =='.': 0){ i ++;{ double divisor=( double)
0.1; while( i <({ struct _tagged_string _temp285= s;( unsigned int)( _temp285.last_plus_one
- _temp285.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp286= s;
char* _temp288= _temp286.curr + i; if( _temp286.base == 0? 1:( _temp288 <
_temp286.base? 1: _temp288 >= _temp286.last_plus_one)){ _throw( Null_Exception);}*
_temp288;})): 0) { digit_seen= 1; ans= ans +( double)(({ struct _tagged_string
_temp289= s; char* _temp291= _temp289.curr + i; if( _temp289.base == 0? 1:(
_temp291 < _temp289.base? 1: _temp291 >= _temp289.last_plus_one)){ _throw(
Null_Exception);}* _temp291;}) -'0') * divisor; divisor= divisor / 10.0; i ++;}}}
if( ! digit_seen){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp292=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp292[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp293; _temp293.tag= Cyc_Core_InvalidArg_tag;
_temp293.f1=( struct _tagged_string)({ char* _temp294=( char*)"String::to_double";
struct _tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 18; _temp295;}); _temp293;}); _temp292;}));}
if( i <({ struct _tagged_string _temp296= s;( unsigned int)( _temp296.last_plus_one
- _temp296.curr);})?({ struct _tagged_string _temp297= s; char* _temp299=
_temp297.curr + i; if( _temp297.base == 0? 1:( _temp299 < _temp297.base? 1:
_temp299 >= _temp297.last_plus_one)){ _throw( Null_Exception);}* _temp299;}) =='e'?
1:({ struct _tagged_string _temp300= s; char* _temp302= _temp300.curr + i; if(
_temp300.base == 0? 1:( _temp302 < _temp300.base? 1: _temp302 >= _temp300.last_plus_one)){
_throw( Null_Exception);}* _temp302;}) =='E': 0){ i ++;{ int exponent= 0; int
exp_sign= 1; digit_seen= 0; if( i <({ struct _tagged_string _temp303= s;(
unsigned int)( _temp303.last_plus_one - _temp303.curr);})?({ struct
_tagged_string _temp304= s; char* _temp306= _temp304.curr + i; if( _temp304.base
== 0? 1:( _temp306 < _temp304.base? 1: _temp306 >= _temp304.last_plus_one)){
_throw( Null_Exception);}* _temp306;}) =='-': 0){ i ++; exp_sign= - 1;} else{
if( i <({ struct _tagged_string _temp307= s;( unsigned int)( _temp307.last_plus_one
- _temp307.curr);})?({ struct _tagged_string _temp308= s; char* _temp310=
_temp308.curr + i; if( _temp308.base == 0? 1:( _temp310 < _temp308.base? 1:
_temp310 >= _temp308.last_plus_one)){ _throw( Null_Exception);}* _temp310;}) =='+':
0){ i ++;}} while( i <({ struct _tagged_string _temp311= s;( unsigned int)(
_temp311.last_plus_one - _temp311.curr);})? Cyc_Character_isdigit(({ struct
_tagged_string _temp312= s; char* _temp314= _temp312.curr + i; if( _temp312.base
== 0? 1:( _temp314 < _temp312.base? 1: _temp314 >= _temp312.last_plus_one)){
_throw( Null_Exception);}* _temp314;})): 0) { digit_seen= 1; exponent= exponent
* 10 +( int)(({ struct _tagged_string _temp315= s; char* _temp317= _temp315.curr
+ i; if( _temp315.base == 0? 1:( _temp317 < _temp315.base? 1: _temp317 >=
_temp315.last_plus_one)){ _throw( Null_Exception);}* _temp317;}) -'0'); i ++;}
if( ! digit_seen){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp318=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp318[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp319; _temp319.tag= Cyc_Core_InvalidArg_tag;
_temp319.f1=( struct _tagged_string)({ char* _temp320=( char*)"String::to_double";
struct _tagged_string _temp321; _temp321.curr= _temp320; _temp321.base= _temp320;
_temp321.last_plus_one= _temp320 + 18; _temp321;}); _temp319;}); _temp318;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){({ int* _temp322= offset; if( _temp322 == 0){ _throw( Null_Exception);}*
_temp322= i;});} return ans * sn;}