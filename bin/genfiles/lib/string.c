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
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; extern int Cyc_Character_isdigit( char c);
extern int Cyc_Character_isspace( char c); extern char Cyc_Character_toupper(
char c); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern unsigned int Cyc_String_strlen( struct
_tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern int Cyc_String_strptrcmp( struct
_tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_strncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int len); extern
int Cyc_String_strncasecmp( struct _tagged_string s1, struct _tagged_string s2,
unsigned int len); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrncmp( struct _tagged_string s1,
struct _tagged_string s2, unsigned int n); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src);
extern struct _tagged_string Cyc_String_strconcat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*);
extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); extern struct _tagged_string Cyc_String_strcpy( struct
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
1: ofs < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp5=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp5[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp6; _temp6.tag= Cyc_Core_InvalidArg;
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
_throw( Null_Exception);}* _temp66='\000';});}} else{( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp67=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp67[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp68; _temp68.tag= Cyc_Core_InvalidArg; _temp68.f1=
error; _temp68;}); _temp67;}));} return dest;} struct _tagged_string Cyc_String_strcat(
struct _tagged_string dest, struct _tagged_string src){ return Cyc_String_int_strcato(
dest, src, 0,( struct _tagged_string)({ char* _temp69=( char*)"String::strcat";
struct _tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69;
_temp70.last_plus_one= _temp69 + 15; _temp70;}));} struct _tagged_string Cyc_String_strconcat(
struct _tagged_string a, struct _tagged_string b){ unsigned int _temp71= Cyc_String_strlen(
a); goto _LL72; _LL72: { unsigned int _temp73= Cyc_String_strlen( b); goto _LL74;
_LL74: { struct _tagged_string ans= Cyc_Core_new_string(( int)( _temp71 +
_temp73)); int i; int j; for( i= 0; i < _temp71; ++ i){({ struct _tagged_string
_temp75= ans; char* _temp77= _temp75.curr + i; if( _temp75.base == 0? 1:(
_temp77 < _temp75.base? 1: _temp77 >= _temp75.last_plus_one)){ _throw(
Null_Exception);}* _temp77=({ struct _tagged_string _temp78= a; char* _temp80=
_temp78.curr + i; if( _temp78.base == 0? 1:( _temp80 < _temp78.base? 1: _temp80
>= _temp78.last_plus_one)){ _throw( Null_Exception);}* _temp80;});});} for( j= 0;
j < _temp73; ++ j){({ struct _tagged_string _temp81= ans; char* _temp83= _temp81.curr
+( i + j); if( _temp81.base == 0? 1:( _temp83 < _temp81.base? 1: _temp83 >=
_temp81.last_plus_one)){ _throw( Null_Exception);}* _temp83=({ struct
_tagged_string _temp84= b; char* _temp86= _temp84.curr + j; if( _temp84.base ==
0? 1:( _temp86 < _temp84.base? 1: _temp86 >= _temp84.last_plus_one)){ _throw(
Null_Exception);}* _temp86;});});} return ans;}}} struct _tagged_string Cyc_String_strconcat_l(
struct Cyc_List_List* strs){ unsigned int len; unsigned int total_len=(
unsigned int) 0; struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp97=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=(
void*)(( unsigned int) 0); _temp97->tl= 0; _temp97;}); struct Cyc_List_List* end=
lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p=({ struct Cyc_List_List*
_temp87= p; if( _temp87 == 0){ _throw( Null_Exception);} _temp87->tl;})){ len=
Cyc_String_strlen(*(( struct _tagged_string*)({ struct Cyc_List_List* _temp88= p;
if( _temp88 == 0){ _throw( Null_Exception);} _temp88->hd;}))); total_len += len;({
struct Cyc_List_List* _temp89= end; if( _temp89 == 0){ _throw( Null_Exception);}
_temp89->tl=({ struct Cyc_List_List* _temp90=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp90->hd=( void*) len; _temp90->tl= 0;
_temp90;});}); end=({ struct Cyc_List_List* _temp91= end; if( _temp91 == 0){
_throw( Null_Exception);} _temp91->tl;});}} lens=({ struct Cyc_List_List*
_temp92= lens; if( _temp92 == 0){ _throw( Null_Exception);} _temp92->tl;});{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*)({ struct Cyc_List_List* _temp96= strs; if( _temp96 == 0){
_throw( Null_Exception);} _temp96->hd;})); len=( unsigned int)({ struct Cyc_List_List*
_temp93= lens; if( _temp93 == 0){ _throw( Null_Exception);} _temp93->hd;}); Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs=({ struct Cyc_List_List* _temp94=
strs; if( _temp94 == 0){ _throw( Null_Exception);} _temp94->tl;}); lens=({
struct Cyc_List_List* _temp95= lens; if( _temp95 == 0){ _throw( Null_Exception);}
_temp95->tl;});} return ans;}} struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List* strs, struct _tagged_string separator){ if( strs == 0){
return( struct _tagged_string)({ char* _temp98=( char*)""; struct _tagged_string
_temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 1; _temp99;});} if(({ struct Cyc_List_List* _temp100= strs; if(
_temp100 == 0){ _throw( Null_Exception);} _temp100->tl;}) == 0){ return Cyc_String_strdup(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp101= strs; if( _temp101 ==
0){ _throw( Null_Exception);} _temp101->hd;})));}{ struct Cyc_List_List* p= strs;
struct Cyc_List_List* lens=({ struct Cyc_List_List* _temp115=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp115->hd=( void*)(( unsigned int)
0); _temp115->tl= 0; _temp115;}); struct Cyc_List_List* end= lens; unsigned int
len=( unsigned int) 0; unsigned int total_len=( unsigned int) 0; unsigned int
list_len=( unsigned int) 0; for( 0; p != 0; p=({ struct Cyc_List_List* _temp102=
p; if( _temp102 == 0){ _throw( Null_Exception);} _temp102->tl;})){ len= Cyc_String_strlen(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp103= p; if( _temp103 == 0){
_throw( Null_Exception);} _temp103->hd;}))); total_len += len;({ struct Cyc_List_List*
_temp104= end; if( _temp104 == 0){ _throw( Null_Exception);} _temp104->tl=({
struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp105->hd=( void*) len; _temp105->tl= 0; _temp105;});});
end=({ struct Cyc_List_List* _temp106= end; if( _temp106 == 0){ _throw(
Null_Exception);} _temp106->tl;}); ++ list_len;} lens=({ struct Cyc_List_List*
_temp107= lens; if( _temp107 == 0){ _throw( Null_Exception);} _temp107->tl;});{
unsigned int seplen= Cyc_String_strlen( separator); total_len +=( list_len - 1)
* seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int) total_len);
unsigned int i=( unsigned int) 0; while(({ struct Cyc_List_List* _temp108= strs;
if( _temp108 == 0){ _throw( Null_Exception);} _temp108->tl;}) != 0) { struct
_tagged_string next=*(( struct _tagged_string*)({ struct Cyc_List_List* _temp112=
strs; if( _temp112 == 0){ _throw( Null_Exception);} _temp112->hd;})); len=(
unsigned int)({ struct Cyc_List_List* _temp109= lens; if( _temp109 == 0){ _throw(
Null_Exception);} _temp109->hd;}); Cyc_String_strncpy( ans,( int) i, next, 0,
len); i += len; Cyc_String_strncpy( ans,( int) i, separator, 0, seplen); i +=
seplen; strs=({ struct Cyc_List_List* _temp110= strs; if( _temp110 == 0){ _throw(
Null_Exception);} _temp110->tl;}); lens=({ struct Cyc_List_List* _temp111= lens;
if( _temp111 == 0){ _throw( Null_Exception);} _temp111->tl;});} Cyc_String_strncpy(
ans,( int) i,*(( struct _tagged_string*)({ struct Cyc_List_List* _temp113= strs;
if( _temp113 == 0){ _throw( Null_Exception);} _temp113->hd;})), 0,( unsigned int)({
struct Cyc_List_List* _temp114= lens; if( _temp114 == 0){ _throw( Null_Exception);}
_temp114->hd;})); return ans;}}}} struct _tagged_string Cyc_String_strncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int _temp117=({ struct _tagged_string _temp116= dest;(
unsigned int)( _temp116.last_plus_one - _temp116.curr);}); goto _LL118; _LL118: {
unsigned int _temp120=({ struct _tagged_string _temp119= src;( unsigned int)(
_temp119.last_plus_one - _temp119.curr);}); goto _LL121; _LL121: if( destOfs < 0){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp122=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp122[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp123; _temp123.tag= Cyc_Core_InvalidArg; _temp123.f1=(
struct _tagged_string)({ char* _temp124=( char*)"String::strncpy"; struct
_tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 16; _temp125;}); _temp123;}); _temp122;}));}
if( srcOfs < 0? 1:( n > 0? srcOfs >= _temp120: 0)){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp126=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp126[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp127; _temp127.tag= Cyc_Core_InvalidArg; _temp127.f1=(
struct _tagged_string)({ char* _temp128=( char*)"String::strncpy"; struct
_tagged_string _temp129; _temp129.curr= _temp128; _temp129.base= _temp128;
_temp129.last_plus_one= _temp128 + 16; _temp129;}); _temp127;}); _temp126;}));}
if( n <= 0){ return dest;} if( destOfs + n > _temp117? 1: srcOfs + n > _temp120){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp130=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp130[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp131; _temp131.tag= Cyc_Core_InvalidArg; _temp131.f1=(
struct _tagged_string)({ char* _temp132=( char*)"String::strncpy"; struct
_tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 16; _temp133;}); _temp131;}); _temp130;}));}{
int i; for( i= 0; i < n; i ++){ char _temp137=({ struct _tagged_string _temp134=
src; char* _temp136= _temp134.curr +( srcOfs + i); if( _temp134.base == 0? 1:(
_temp136 < _temp134.base? 1: _temp136 >= _temp134.last_plus_one)){ _throw(
Null_Exception);}* _temp136;}); goto _LL138; _LL138: if( _temp137 =='\000'){
break;}({ struct _tagged_string _temp139= dest; char* _temp141= _temp139.curr +(
destOfs + i); if( _temp139.base == 0? 1:( _temp141 < _temp139.base? 1: _temp141
>= _temp139.last_plus_one)){ _throw( Null_Exception);}* _temp141= _temp137;});}
for( 0; i < n; i ++){({ struct _tagged_string _temp142= dest; char* _temp144=
_temp142.curr +( destOfs + i); if( _temp142.base == 0? 1:( _temp144 < _temp142.base?
1: _temp144 >= _temp142.last_plus_one)){ _throw( Null_Exception);}* _temp144='\000';});}
return dest;}}} struct _tagged_string Cyc_String_zstrncpy( struct _tagged_string
dest, int destOfs, struct _tagged_string src, int srcOfs, unsigned int n){
unsigned int _temp146=({ struct _tagged_string _temp145= dest;( unsigned int)(
_temp145.last_plus_one - _temp145.curr);}); goto _LL147; _LL147: { unsigned int
_temp149=({ struct _tagged_string _temp148= src;( unsigned int)( _temp148.last_plus_one
- _temp148.curr);}); goto _LL150; _LL150: if( destOfs < 0){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp151=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp151[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp152; _temp152.tag= Cyc_Core_InvalidArg; _temp152.f1=(
struct _tagged_string)({ char* _temp153=( char*)"String::zstrncpy"; struct
_tagged_string _temp154; _temp154.curr= _temp153; _temp154.base= _temp153;
_temp154.last_plus_one= _temp153 + 17; _temp154;}); _temp152;}); _temp151;}));}
if( srcOfs < 0? 1:( n > 0? srcOfs >= _temp149: 0)){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp155=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp155[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp156; _temp156.tag= Cyc_Core_InvalidArg; _temp156.f1=(
struct _tagged_string)({ char* _temp157=( char*)"String::zstrncpy"; struct
_tagged_string _temp158; _temp158.curr= _temp157; _temp158.base= _temp157;
_temp158.last_plus_one= _temp157 + 17; _temp158;}); _temp156;}); _temp155;}));}
if( n <= 0){ return dest;} if( destOfs + n > _temp146? 1: srcOfs + n > _temp149){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp159=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp159[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp160; _temp160.tag= Cyc_Core_InvalidArg; _temp160.f1=(
struct _tagged_string)({ char* _temp161=( char*)"String::zstrncpy"; struct
_tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 17; _temp162;}); _temp160;}); _temp159;}));}{
int i; for( i= 0; i < n; i ++){({ struct _tagged_string _temp163= dest; char*
_temp165= _temp163.curr +( destOfs + i); if( _temp163.base == 0? 1:( _temp165 <
_temp163.base? 1: _temp165 >= _temp163.last_plus_one)){ _throw( Null_Exception);}*
_temp165=({ struct _tagged_string _temp166= src; char* _temp168= _temp166.curr +(
srcOfs + i); if( _temp166.base == 0? 1:( _temp168 < _temp166.base? 1: _temp168
>= _temp166.last_plus_one)){ _throw( Null_Exception);}* _temp168;});});} return
dest;}}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp169= dest;(
unsigned int)( _temp169.last_plus_one - _temp169.curr);})){({ struct
_tagged_string _temp170= dest; char* _temp172= _temp170.curr +( int) len; if(
_temp170.base == 0? 1:( _temp172 < _temp170.base? 1: _temp172 >= _temp170.last_plus_one)){
_throw( Null_Exception);}* _temp172='\000';});} return dest;} struct
_tagged_string Cyc_String_strdup( struct _tagged_string src){ unsigned int len;
struct _tagged_string temp; len= Cyc_String_strlen( src); temp= Cyc_Core_new_string((
int) len); Cyc_String_strncpy( temp, 0, src, 0, len); return temp;} struct
_tagged_string Cyc_String_expand( struct _tagged_string s, unsigned int sz){
struct _tagged_string temp; unsigned int slen; slen= Cyc_String_strlen( s); sz=
sz > slen? sz: slen; temp= Cyc_Core_new_string(( int) sz); Cyc_String_strncpy(
temp, 0, s, 0, slen); if( slen !=({ struct _tagged_string _temp173= s;(
unsigned int)( _temp173.last_plus_one - _temp173.curr);})){({ struct
_tagged_string _temp174= temp; char* _temp176= _temp174.curr +( int) slen; if(
_temp174.base == 0? 1:( _temp176 < _temp174.base? 1: _temp176 >= _temp174.last_plus_one)){
_throw( Null_Exception);}* _temp176='\000';});} return temp;} struct
_tagged_string Cyc_String_realloc_str( struct _tagged_string str, unsigned int
sz){ unsigned int maxsizeP=({ struct _tagged_string _temp180= str;( unsigned int)(
_temp180.last_plus_one - _temp180.curr);}); if( maxsizeP == 0){ maxsizeP= 30 >
sz? 30: sz; str= Cyc_Core_new_string(( int) maxsizeP);({ struct _tagged_string
_temp177= str; char* _temp179= _temp177.curr + 0; if( _temp177.base == 0? 1:(
_temp179 < _temp177.base? 1: _temp179 >= _temp177.last_plus_one)){ _throw(
Null_Exception);}* _temp179='\000';});} else{ if( sz > maxsizeP){ maxsizeP=
maxsizeP * 2 >( sz * 5) / 4? maxsizeP * 2:( sz * 5) / 4; str= Cyc_String_expand(
str, maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring(
struct _tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt <
0)? 1:( start +( int) amt) - 1 >( int)({ struct _tagged_string _temp181= s;(
unsigned int)( _temp181.last_plus_one - _temp181.curr);})){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp182=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp182[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp183; _temp183.tag= Cyc_Core_InvalidArg; _temp183.f1=(
struct _tagged_string)({ char* _temp184=( char*)"String::substring"; struct
_tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 18; _temp185;}); _temp183;}); _temp182;}));}{
struct _tagged_string ans= Cyc_Core_new_string(( int)( amt + 1));{ unsigned int
i=( unsigned int) 0; for( 0; i < amt; ++ i){({ struct _tagged_string _temp186=
ans; char* _temp188= _temp186.curr +( int) i; if( _temp186.base == 0? 1:(
_temp188 < _temp186.base? 1: _temp188 >= _temp186.last_plus_one)){ _throw(
Null_Exception);}* _temp188=({ struct _tagged_string _temp189= s; char* _temp191=
_temp189.curr +( int)( start + i); if( _temp189.base == 0? 1:( _temp191 <
_temp189.base? 1: _temp191 >= _temp189.last_plus_one)){ _throw( Null_Exception);}*
_temp191;});});}}({ struct _tagged_string _temp192= ans; char* _temp194=
_temp192.curr +( int) amt; if( _temp192.base == 0? 1:( _temp194 < _temp192.base?
1: _temp194 >= _temp192.last_plus_one)){ _throw( Null_Exception);}* _temp194='\000';});
return ans;}} struct _tagged_string Cyc_String_replace_suffix( struct
_tagged_string src, struct _tagged_string curr_suffix, struct _tagged_string
new_suffix){ unsigned int m=({ struct _tagged_string _temp210= src;(
unsigned int)( _temp210.last_plus_one - _temp210.curr);}); unsigned int n=({
struct _tagged_string _temp209= curr_suffix;( unsigned int)( _temp209.last_plus_one
- _temp209.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp207=( char*)"String::replace_suffix"; struct _tagged_string _temp208;
_temp208.curr= _temp207; _temp208.base= _temp207; _temp208.last_plus_one=
_temp207 + 23; _temp208;}); if( m < n){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp195=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp195[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp196; _temp196.tag= Cyc_Core_InvalidArg;
_temp196.f1= err; _temp196;}); _temp195;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(({ struct _tagged_string _temp197= src; char* _temp199=
_temp197.curr +( int)( m - i); if( _temp197.base == 0? 1:( _temp199 < _temp197.base?
1: _temp199 >= _temp197.last_plus_one)){ _throw( Null_Exception);}* _temp199;})
!=({ struct _tagged_string _temp200= curr_suffix; char* _temp202= _temp200.curr
+( int)( n - i); if( _temp200.base == 0? 1:( _temp202 < _temp200.base? 1:
_temp202 >= _temp200.last_plus_one)){ _throw( Null_Exception);}* _temp202;})){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp203=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp203[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp204; _temp204.tag= Cyc_Core_InvalidArg; _temp204.f1=
err; _temp204;}); _temp203;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp206= new_suffix;( unsigned int)(
_temp206.last_plus_one - _temp206.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp205= new_suffix;( unsigned int)( _temp205.last_plus_one -
_temp205.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp220=( char*)"String::strpbrk"; struct
_tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 16; _temp221;})); char c='\000'; for( 0; len
> 0?( c=({ struct _tagged_string _temp211= s; char* _temp212= _temp211.curr; if(
_temp211.base == 0? 1:( _temp212 < _temp211.base? 1: _temp212 >= _temp211.last_plus_one)){
_throw( Null_Exception);}* _temp212;})) != 0: 0; len --,({ struct _tagged_string*
_temp213=& s; struct _tagged_string _temp214=* _temp213; _temp213->curr ++;
_temp214;})){ int j= 0; for( 0; j <({ struct _tagged_string _temp215= accept;(
unsigned int)( _temp215.last_plus_one - _temp215.curr);}); j ++){ if( c ==({
struct _tagged_string _temp216= accept; char* _temp218= _temp216.curr + j; if(
_temp216.base == 0? 1:( _temp218 < _temp216.base? 1: _temp218 >= _temp216.last_plus_one)){
_throw( Null_Exception);}* _temp218;})){ return s;}}} return({ struct
_tagged_string _temp219={ 0, 0, 0}; _temp219;});} struct _tagged_string Cyc_String_strchr(
struct _tagged_string s, char c){ int len=( int) Cyc_String_int_strleno( s, 0,(
struct _tagged_string)({ char* _temp227=( char*)"String::strchr"; struct
_tagged_string _temp228; _temp228.curr= _temp227; _temp228.base= _temp227;
_temp228.last_plus_one= _temp227 + 15; _temp228;})); char c2='\000'; for( 0; len
> 0?( c2=({ struct _tagged_string _temp222= s; char* _temp223= _temp222.curr;
if( _temp222.base == 0? 1:( _temp223 < _temp222.base? 1: _temp223 >= _temp222.last_plus_one)){
_throw( Null_Exception);}* _temp223;})) != 0: 0; len --,({ struct _tagged_string*
_temp224=& s; struct _tagged_string _temp225=* _temp224; _temp224->curr ++;
_temp225;})){ if( c2 == c){ return s;}} return({ struct _tagged_string _temp226={
0, 0, 0}; _temp226;});} struct _tagged_string Cyc_String_strrchr( struct
_tagged_string s, char c){ int len=( int) Cyc_String_int_strleno( s, 0,( struct
_tagged_string)({ char* _temp234=( char*)"String::strrchr"; struct
_tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 16; _temp235;})); int i= len - 1; s.curr += i;
for( 0; i >= 0; i --,({ struct _tagged_string* _temp229=& s; struct
_tagged_string _temp230=* _temp229; _temp229->curr --; _temp230;})){ if(({
struct _tagged_string _temp231= s; char* _temp232= _temp231.curr; if( _temp231.base
== 0? 1:( _temp232 < _temp231.base? 1: _temp232 >= _temp231.last_plus_one)){
_throw( Null_Exception);}* _temp232;}) == c){ return s;}} return({ struct
_tagged_string _temp233={ 0, 0, 0}; _temp233;});} unsigned int Cyc_String_strspn(
struct _tagged_string s, struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp244=( char*)"String::strspn"; struct
_tagged_string _temp245; _temp245.curr= _temp244; _temp245.base= _temp244;
_temp245.last_plus_one= _temp244 + 15; _temp245;}));{ unsigned int i=(
unsigned int) 0; for( 0; i < len; i ++){ int j; for( j= 0; j <({ struct
_tagged_string _temp236= accept;( unsigned int)( _temp236.last_plus_one -
_temp236.curr);}); j ++){ if(({ struct _tagged_string _temp237= s; char*
_temp239= _temp237.curr +( int) i; if( _temp237.base == 0? 1:( _temp239 <
_temp237.base? 1: _temp239 >= _temp237.last_plus_one)){ _throw( Null_Exception);}*
_temp239;}) ==({ struct _tagged_string _temp240= accept; char* _temp242=
_temp240.curr + j; if( _temp240.base == 0? 1:( _temp242 < _temp240.base? 1:
_temp242 >= _temp240.last_plus_one)){ _throw( Null_Exception);}* _temp242;})){
break;}} if( j ==({ struct _tagged_string _temp243= accept;( unsigned int)(
_temp243.last_plus_one - _temp243.curr);})){ return i;}}} return len;} struct
Cyc_List_List* Cyc_String_explode( struct _tagged_string s){ struct Cyc_List_List*
result= 0;{ int i=( int)( Cyc_String_strlen( s) - 1); for( 0; i >= 0; i --){
result=({ struct Cyc_List_List* _temp246=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp246->hd=( void*)(( int)({ struct
_tagged_string _temp247= s; char* _temp249= _temp247.curr + i; if( _temp247.base
== 0? 1:( _temp249 < _temp247.base? 1: _temp249 >= _temp247.last_plus_one)){
_throw( Null_Exception);}* _temp249;})); _temp246->tl= result; _temp246;});}}
return result;} struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
chars){ struct _tagged_string s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); unsigned int i=( unsigned int) 0; while( chars !=
0) {({ struct _tagged_string _temp250= s; char* _temp252= _temp250.curr +( int)(
i ++); if( _temp250.base == 0? 1:( _temp252 < _temp250.base? 1: _temp252 >=
_temp250.last_plus_one)){ _throw( Null_Exception);}* _temp252=( char)(( int)({
struct Cyc_List_List* _temp253= chars; if( _temp253 == 0){ _throw(
Null_Exception);} _temp253->hd;}));}); chars=({ struct Cyc_List_List* _temp254=
chars; if( _temp254 == 0){ _throw( Null_Exception);} _temp254->tl;});} return s;}
int Cyc_String_to_int( struct _tagged_string s, int* offset){ int ans= 0; int sn=
1; int digit_seen= 0; int i= offset == 0? 0:({ int* _temp279= offset; if(
_temp279 == 0){ _throw( Null_Exception);}* _temp279;}); while( i <({ struct
_tagged_string _temp255= s;( unsigned int)( _temp255.last_plus_one - _temp255.curr);})?
Cyc_Character_isspace(({ struct _tagged_string _temp256= s; char* _temp258=
_temp256.curr + i; if( _temp256.base == 0? 1:( _temp258 < _temp256.base? 1:
_temp258 >= _temp256.last_plus_one)){ _throw( Null_Exception);}* _temp258;})): 0) {
i ++;} if( i <({ struct _tagged_string _temp259= s;( unsigned int)( _temp259.last_plus_one
- _temp259.curr);})?({ struct _tagged_string _temp260= s; char* _temp262=
_temp260.curr + i; if( _temp260.base == 0? 1:( _temp262 < _temp260.base? 1:
_temp262 >= _temp260.last_plus_one)){ _throw( Null_Exception);}* _temp262;}) =='-':
0){ i ++; sn= - 1;} while( i <({ struct _tagged_string _temp263= s;(
unsigned int)( _temp263.last_plus_one - _temp263.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp264= s; char* _temp266= _temp264.curr + i; if(
_temp264.base == 0? 1:( _temp266 < _temp264.base? 1: _temp266 >= _temp264.last_plus_one)){
_throw( Null_Exception);}* _temp266;})): 0) { i ++;} while( i <({ struct
_tagged_string _temp267= s;( unsigned int)( _temp267.last_plus_one - _temp267.curr);})?
Cyc_Character_isdigit(({ struct _tagged_string _temp268= s; char* _temp270=
_temp268.curr + i; if( _temp268.base == 0? 1:( _temp270 < _temp268.base? 1:
_temp270 >= _temp268.last_plus_one)){ _throw( Null_Exception);}* _temp270;})): 0) {
digit_seen= 1; ans= ans * 10 +( int)(({ struct _tagged_string _temp271= s; char*
_temp273= _temp271.curr + i; if( _temp271.base == 0? 1:( _temp273 < _temp271.base?
1: _temp273 >= _temp271.last_plus_one)){ _throw( Null_Exception);}* _temp273;})
-'0'); i ++;} ans= ans * sn; if( ! digit_seen){( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp274=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp274[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp275; _temp275.tag= Cyc_Core_InvalidArg; _temp275.f1=(
struct _tagged_string)({ char* _temp276=( char*)"String::to_int"; struct
_tagged_string _temp277; _temp277.curr= _temp276; _temp277.base= _temp276;
_temp277.last_plus_one= _temp276 + 15; _temp277;}); _temp275;}); _temp274;}));}
if( offset != 0){({ int* _temp278= offset; if( _temp278 == 0){ _throw(
Null_Exception);}* _temp278= i;});} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:({ int* _temp337= offset; if( _temp337 == 0){
_throw( Null_Exception);}* _temp337;}); while( i <({ struct _tagged_string
_temp280= s;( unsigned int)( _temp280.last_plus_one - _temp280.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp281= s; char* _temp283= _temp281.curr + i; if(
_temp281.base == 0? 1:( _temp283 < _temp281.base? 1: _temp283 >= _temp281.last_plus_one)){
_throw( Null_Exception);}* _temp283;})): 0) { i ++;} if( i <({ struct
_tagged_string _temp284= s;( unsigned int)( _temp284.last_plus_one - _temp284.curr);})?({
struct _tagged_string _temp285= s; char* _temp287= _temp285.curr + i; if(
_temp285.base == 0? 1:( _temp287 < _temp285.base? 1: _temp287 >= _temp285.last_plus_one)){
_throw( Null_Exception);}* _temp287;}) =='-': 0){ i ++; sn= - 1;} while( i <({
struct _tagged_string _temp288= s;( unsigned int)( _temp288.last_plus_one -
_temp288.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp289= s;
char* _temp291= _temp289.curr + i; if( _temp289.base == 0? 1:( _temp291 <
_temp289.base? 1: _temp291 >= _temp289.last_plus_one)){ _throw( Null_Exception);}*
_temp291;})): 0) { digit_seen= 1; ans= ans * 10.0 +( double)(({ struct
_tagged_string _temp292= s; char* _temp294= _temp292.curr + i; if( _temp292.base
== 0? 1:( _temp294 < _temp292.base? 1: _temp294 >= _temp292.last_plus_one)){
_throw( Null_Exception);}* _temp294;}) -'0'); i ++;} if( i <({ struct
_tagged_string _temp295= s;( unsigned int)( _temp295.last_plus_one - _temp295.curr);})?({
struct _tagged_string _temp296= s; char* _temp298= _temp296.curr + i; if(
_temp296.base == 0? 1:( _temp298 < _temp296.base? 1: _temp298 >= _temp296.last_plus_one)){
_throw( Null_Exception);}* _temp298;}) =='.': 0){ i ++;{ double divisor=( double)
0.1; while( i <({ struct _tagged_string _temp299= s;( unsigned int)( _temp299.last_plus_one
- _temp299.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp300= s;
char* _temp302= _temp300.curr + i; if( _temp300.base == 0? 1:( _temp302 <
_temp300.base? 1: _temp302 >= _temp300.last_plus_one)){ _throw( Null_Exception);}*
_temp302;})): 0) { digit_seen= 1; ans= ans +( double)(({ struct _tagged_string
_temp303= s; char* _temp305= _temp303.curr + i; if( _temp303.base == 0? 1:(
_temp305 < _temp303.base? 1: _temp305 >= _temp303.last_plus_one)){ _throw(
Null_Exception);}* _temp305;}) -'0') * divisor; divisor= divisor / 10.0; i ++;}}}
if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp306=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp306[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp307; _temp307.tag= Cyc_Core_InvalidArg;
_temp307.f1=( struct _tagged_string)({ char* _temp308=( char*)"String::to_double";
struct _tagged_string _temp309; _temp309.curr= _temp308; _temp309.base= _temp308;
_temp309.last_plus_one= _temp308 + 18; _temp309;}); _temp307;}); _temp306;}));}
if( i <({ struct _tagged_string _temp310= s;( unsigned int)( _temp310.last_plus_one
- _temp310.curr);})?({ struct _tagged_string _temp311= s; char* _temp313=
_temp311.curr + i; if( _temp311.base == 0? 1:( _temp313 < _temp311.base? 1:
_temp313 >= _temp311.last_plus_one)){ _throw( Null_Exception);}* _temp313;}) =='e'?
1:({ struct _tagged_string _temp314= s; char* _temp316= _temp314.curr + i; if(
_temp314.base == 0? 1:( _temp316 < _temp314.base? 1: _temp316 >= _temp314.last_plus_one)){
_throw( Null_Exception);}* _temp316;}) =='E': 0){ i ++;{ int exponent= 0; int
exp_sign= 1; digit_seen= 0; if( i <({ struct _tagged_string _temp317= s;(
unsigned int)( _temp317.last_plus_one - _temp317.curr);})?({ struct
_tagged_string _temp318= s; char* _temp320= _temp318.curr + i; if( _temp318.base
== 0? 1:( _temp320 < _temp318.base? 1: _temp320 >= _temp318.last_plus_one)){
_throw( Null_Exception);}* _temp320;}) =='-': 0){ i ++; exp_sign= - 1;} else{
if( i <({ struct _tagged_string _temp321= s;( unsigned int)( _temp321.last_plus_one
- _temp321.curr);})?({ struct _tagged_string _temp322= s; char* _temp324=
_temp322.curr + i; if( _temp322.base == 0? 1:( _temp324 < _temp322.base? 1:
_temp324 >= _temp322.last_plus_one)){ _throw( Null_Exception);}* _temp324;}) =='+':
0){ i ++;}} while( i <({ struct _tagged_string _temp325= s;( unsigned int)(
_temp325.last_plus_one - _temp325.curr);})? Cyc_Character_isdigit(({ struct
_tagged_string _temp326= s; char* _temp328= _temp326.curr + i; if( _temp326.base
== 0? 1:( _temp328 < _temp326.base? 1: _temp328 >= _temp326.last_plus_one)){
_throw( Null_Exception);}* _temp328;})): 0) { digit_seen= 1; exponent= exponent
* 10 +( int)(({ struct _tagged_string _temp329= s; char* _temp331= _temp329.curr
+ i; if( _temp329.base == 0? 1:( _temp331 < _temp329.base? 1: _temp331 >=
_temp329.last_plus_one)){ _throw( Null_Exception);}* _temp331;}) -'0'); i ++;}
if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp332=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp332[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp333; _temp333.tag= Cyc_Core_InvalidArg;
_temp333.f1=( struct _tagged_string)({ char* _temp334=( char*)"String::to_double";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 18; _temp335;}); _temp333;}); _temp332;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){({ int* _temp336= offset; if( _temp336 == 0){ _throw( Null_Exception);}*
_temp336= i;});} return ans * sn;}