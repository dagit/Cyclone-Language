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
_temp3= _temp1.curr +( int) i; if( _temp3 < _temp1.base? 1: _temp3 >= _temp1.last_plus_one){
_throw( Null_Exception);}* _temp3;}) ==( int)'\000'){ return i;}} return i;}
static unsigned int Cyc_String_int_strleno( struct _tagged_string s, int ofs,
struct _tagged_string error){ if(( unsigned int) ofs >({ struct _tagged_string
_temp4= s;( unsigned int)( _temp4.last_plus_one - _temp4.curr);})? 1: ofs < 0){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp5=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp5[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp6; _temp6.tag= Cyc_Core_InvalidArg_tag;
_temp6.f1= error; _temp6;}); _temp5;}));}{ int i; for( i= ofs;( unsigned int) i
<({ struct _tagged_string _temp7= s;( unsigned int)( _temp7.last_plus_one -
_temp7.curr);}); i ++){ if(( int)({ struct _tagged_string _temp8= s; char*
_temp10= _temp8.curr + i; if( _temp10 < _temp8.base? 1: _temp10 >= _temp8.last_plus_one){
_throw( Null_Exception);}* _temp10;}) ==( int)'\000'){ break;}} return(
unsigned int)( i - ofs);}} static int Cyc_String_case_cmp( char c1, char c2){
return( int) c1 -( int) c2;} static int Cyc_String_nocase_cmp( char c1, char c2){
return Cyc_String_case_cmp( Cyc_Character_toupper( c1), Cyc_Character_toupper(
c2));} static int Cyc_String_cmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2, int(*
f)( char, char)){ unsigned int min_length= len1 < len2? len1: len2; int i= - 1;
while(( unsigned int)( ++ i) < min_length) { int diff= f(({ struct
_tagged_string _temp11= s1; char* _temp13= _temp11.curr +( i + ofs1); if(
_temp13 < _temp11.base? 1: _temp13 >= _temp11.last_plus_one){ _throw(
Null_Exception);}* _temp13;}),({ struct _tagged_string _temp14= s2; char*
_temp16= _temp14.curr +( i + ofs2); if( _temp16 < _temp14.base? 1: _temp16 >=
_temp14.last_plus_one){ _throw( Null_Exception);}* _temp16;})); if( diff != 0){
return diff;}} return( int) len1 -( int) len2;} int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{
unsigned int len1= Cyc_String_int_strleno( s1, 0,( struct _tagged_string)({ char*
_temp17=( char*)"String::strcmp"; struct _tagged_string _temp18; _temp18.curr=
_temp17; _temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 15; _temp18;}));
unsigned int len2= Cyc_String_int_strleno( s2, 0,( struct _tagged_string)({ char*
_temp19=( char*)"String::strcmp"; struct _tagged_string _temp20; _temp20.curr=
_temp19; _temp20.base= _temp19; _temp20.last_plus_one= _temp19 + 15; _temp20;}));
return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}} int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2){ return Cyc_String_strcmp(*
s1,* s2);} static int Cyc_String_ncmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2,
unsigned int n, int(* f)( char, char)){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int min_len= len1 > len2? len2: len1; unsigned int bound= min_len > n?
n: min_len;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){ int retc; if((
retc= f(({ struct _tagged_string _temp21= s1; char* _temp23= _temp21.curr +( i +
ofs1); if( _temp23 < _temp21.base? 1: _temp23 >= _temp21.last_plus_one){ _throw(
Null_Exception);}* _temp23;}),({ struct _tagged_string _temp24= s2; char*
_temp26= _temp24.curr +( i + ofs2); if( _temp26 < _temp24.base? 1: _temp26 >=
_temp24.last_plus_one){ _throw( Null_Exception);}* _temp26;}))) != 0){ return
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
if( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one){ _throw(
Null_Exception);}* _temp39;}) -( int)({ struct _tagged_string _temp40= b; char*
_temp42= _temp40.curr + i; if( _temp42 < _temp40.base? 1: _temp42 >= _temp40.last_plus_one){
_throw( Null_Exception);}* _temp42;}); if( diff != 0){ return diff;}} return(
int) as -( int) bs;}} int Cyc_String_zstrncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int s1size=({ struct _tagged_string _temp43= s1;( unsigned int)(
_temp43.last_plus_one - _temp43.curr);}); unsigned int s2size=({ struct
_tagged_string _temp44= s2;( unsigned int)( _temp44.last_plus_one - _temp44.curr);});
unsigned int min_size= s1size > s2size? s2size: s1size; unsigned int bound=
min_size > n? n: min_size;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){
if(( int)({ struct _tagged_string _temp45= s1; char* _temp47= _temp45.curr + i;
if( _temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one){ _throw(
Null_Exception);}* _temp47;}) <( int)({ struct _tagged_string _temp48= s2; char*
_temp50= _temp48.curr + i; if( _temp50 < _temp48.base? 1: _temp50 >= _temp48.last_plus_one){
_throw( Null_Exception);}* _temp50;})){ return - 1;} else{ if(( int)({ struct
_tagged_string _temp51= s2; char* _temp53= _temp51.curr + i; if( _temp53 <
_temp51.base? 1: _temp53 >= _temp51.last_plus_one){ _throw( Null_Exception);}*
_temp53;}) <( int)({ struct _tagged_string _temp54= s1; char* _temp56= _temp54.curr
+ i; if( _temp56 < _temp54.base? 1: _temp56 >= _temp54.last_plus_one){ _throw(
Null_Exception);}* _temp56;})){ return 1;}}}} if( min_size <= bound){ return 0;}
if( s1size < s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp(
struct _tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(*
a,* b);} static struct _tagged_string Cyc_String_int_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs, struct
_tagged_string error){ int i; unsigned int dsize; unsigned int slen;
unsigned int dlen; dsize=({ struct _tagged_string _temp57= dest;( unsigned int)(
_temp57.last_plus_one - _temp57.curr);}); dlen= Cyc_String_strlen( dest); slen=
Cyc_String_int_strleno( src, srcOfs, error); if( slen + dlen <= dsize){ for( i=
0;( unsigned int) i < slen; i ++){({ struct _tagged_string _temp58= dest; char*
_temp60= _temp58.curr +( int)(( unsigned int) i + dlen); if( _temp60 < _temp58.base?
1: _temp60 >= _temp58.last_plus_one){ _throw( Null_Exception);}* _temp60=({
struct _tagged_string _temp61= src; char* _temp63= _temp61.curr +( i + srcOfs);
if( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one){ _throw(
Null_Exception);}* _temp63;});});} if(( unsigned int) i != dsize){({ struct
_tagged_string _temp64= dest; char* _temp66= _temp64.curr +( int)(( unsigned int)
i + dlen); if( _temp66 < _temp64.base? 1: _temp66 >= _temp64.last_plus_one){
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
alen + blen)); int i; int j; for( i= 0;( unsigned int) i < alen; ++ i){({ struct
_tagged_string _temp71= ans; char* _temp73= _temp71.curr + i; if( _temp73 <
_temp71.base? 1: _temp73 >= _temp71.last_plus_one){ _throw( Null_Exception);}*
_temp73=({ struct _tagged_string _temp74= a; char* _temp76= _temp74.curr + i;
if( _temp76 < _temp74.base? 1: _temp76 >= _temp74.last_plus_one){ _throw(
Null_Exception);}* _temp76;});});} for( j= 0;( unsigned int) j < blen; ++ j){({
struct _tagged_string _temp77= ans; char* _temp79= _temp77.curr +( i + j); if(
_temp79 < _temp77.base? 1: _temp79 >= _temp77.last_plus_one){ _throw(
Null_Exception);}* _temp79=({ struct _tagged_string _temp80= b; char* _temp82=
_temp80.curr + j; if( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){
_throw( Null_Exception);}* _temp82;});});} return ans;} struct _tagged_string
Cyc_String_strconcat_l( struct Cyc_List_List* strs){ unsigned int len;
unsigned int total_len=( unsigned int) 0; struct Cyc_List_List* lens=({ struct
Cyc_List_List* _temp83=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp83->hd=( void*)(( unsigned int) 0); _temp83->tl= 0; _temp83;}); struct Cyc_List_List*
end= lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p= p->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*) p->hd)); total_len += len; end->tl=({ struct Cyc_List_List*
_temp84=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp84->hd=( void*) len; _temp84->tl= 0; _temp84;}); end= end->tl;}} lens= lens->tl;{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs= strs->tl; lens= lens->tl;} return
ans;}} struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List* strs,
struct _tagged_string separator){ if( strs == 0){ return( struct _tagged_string)({
char* _temp85=( char*)""; struct _tagged_string _temp86; _temp86.curr= _temp85;
_temp86.base= _temp85; _temp86.last_plus_one= _temp85 + 1; _temp86;});} if( strs->tl
== 0){ return Cyc_String_strdup(*(( struct _tagged_string*) strs->hd));}{ struct
Cyc_List_List* p= strs; struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp87=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp87->hd=( void*)(( unsigned int) 0); _temp87->tl= 0; _temp87;}); struct Cyc_List_List*
end= lens; unsigned int len=( unsigned int) 0; unsigned int total_len=(
unsigned int) 0; unsigned int list_len=( unsigned int) 0; for( 0; p != 0; p= p->tl){
len= Cyc_String_strlen(*(( struct _tagged_string*) p->hd)); total_len += len;
end->tl=({ struct Cyc_List_List* _temp88=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp88->hd=( void*) len; _temp88->tl= 0;
_temp88;}); end= end->tl; ++ list_len;} lens= lens->tl;{ unsigned int seplen=
Cyc_String_strlen( separator); total_len +=( list_len -( unsigned int) 1) *
seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int) total_len);
unsigned int i=( unsigned int) 0; while( strs->tl != 0) { struct _tagged_string
next=*(( struct _tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; Cyc_String_strncpy( ans,( int) i,
separator, 0, seplen); i += seplen; strs= strs->tl; lens= lens->tl;} Cyc_String_strncpy(
ans,( int) i,*(( struct _tagged_string*) strs->hd), 0,( unsigned int) lens->hd);
return ans;}}}} struct _tagged_string Cyc_String_strncpy( struct _tagged_string
dest, int destOfs, struct _tagged_string src, int srcOfs, unsigned int n){
unsigned int destSz=({ struct _tagged_string _temp89= dest;( unsigned int)(
_temp89.last_plus_one - _temp89.curr);}); unsigned int srcSz=({ struct
_tagged_string _temp90= src;( unsigned int)( _temp90.last_plus_one - _temp90.curr);});
if( destOfs < 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp91=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp91[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp92; _temp92.tag= Cyc_Core_InvalidArg_tag;
_temp92.f1=( struct _tagged_string)({ char* _temp93=( char*)"String::strncpy";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 16; _temp94;}); _temp92;}); _temp91;}));} if(
srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp95=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp95[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp96; _temp96.tag= Cyc_Core_InvalidArg_tag;
_temp96.f1=( struct _tagged_string)({ char* _temp97=( char*)"String::strncpy";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 16; _temp98;}); _temp96;}); _temp95;}));} if( n
<=( unsigned int) 0){ return dest;} if(( unsigned int) destOfs + n > destSz? 1:(
unsigned int) srcOfs + n > srcSz){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp99=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp99[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp100; _temp100.tag= Cyc_Core_InvalidArg_tag;
_temp100.f1=( struct _tagged_string)({ char* _temp101=( char*)"String::strncpy";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 16; _temp102;}); _temp100;}); _temp99;}));}{
int i; for( i= 0;( unsigned int) i < n; i ++){ char srcChar=({ struct
_tagged_string _temp103= src; char* _temp105= _temp103.curr +( srcOfs + i); if(
_temp105 < _temp103.base? 1: _temp105 >= _temp103.last_plus_one){ _throw(
Null_Exception);}* _temp105;}); if(( int) srcChar ==( int)'\000'){ break;}({
struct _tagged_string _temp106= dest; char* _temp108= _temp106.curr +( destOfs +
i); if( _temp108 < _temp106.base? 1: _temp108 >= _temp106.last_plus_one){ _throw(
Null_Exception);}* _temp108= srcChar;});} for( 0;( unsigned int) i < n; i ++){({
struct _tagged_string _temp109= dest; char* _temp111= _temp109.curr +( destOfs +
i); if( _temp111 < _temp109.base? 1: _temp111 >= _temp109.last_plus_one){ _throw(
Null_Exception);}* _temp111='\000';});} return dest;}} struct _tagged_string Cyc_String_zstrncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int destSz=({ struct _tagged_string _temp112= dest;(
unsigned int)( _temp112.last_plus_one - _temp112.curr);}); unsigned int srcSz=({
struct _tagged_string _temp113= src;( unsigned int)( _temp113.last_plus_one -
_temp113.curr);}); if( destOfs < 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp114=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp114[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp115; _temp115.tag= Cyc_Core_InvalidArg_tag;
_temp115.f1=( struct _tagged_string)({ char* _temp116=( char*)"String::zstrncpy";
struct _tagged_string _temp117; _temp117.curr= _temp116; _temp117.base= _temp116;
_temp117.last_plus_one= _temp116 + 17; _temp117;}); _temp115;}); _temp114;}));}
if( srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp118=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp118[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp119; _temp119.tag= Cyc_Core_InvalidArg_tag;
_temp119.f1=( struct _tagged_string)({ char* _temp120=( char*)"String::zstrncpy";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 17; _temp121;}); _temp119;}); _temp118;}));}
if( n <=( unsigned int) 0){ return dest;} if(( unsigned int) destOfs + n >
destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp122=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp122[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp123; _temp123.tag= Cyc_Core_InvalidArg_tag;
_temp123.f1=( struct _tagged_string)({ char* _temp124=( char*)"String::zstrncpy";
struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 17; _temp125;}); _temp123;}); _temp122;}));}{
int i; for( i= 0;( unsigned int) i < n; i ++){({ struct _tagged_string _temp126=
dest; char* _temp128= _temp126.curr +( destOfs + i); if( _temp128 < _temp126.base?
1: _temp128 >= _temp126.last_plus_one){ _throw( Null_Exception);}* _temp128=({
struct _tagged_string _temp129= src; char* _temp131= _temp129.curr +( srcOfs + i);
if( _temp131 < _temp129.base? 1: _temp131 >= _temp129.last_plus_one){ _throw(
Null_Exception);}* _temp131;});});} return dest;}} struct _tagged_string Cyc_String_strcpy(
struct _tagged_string dest, struct _tagged_string src){ unsigned int len= Cyc_String_strlen(
src); Cyc_String_strncpy( dest, 0, src, 0, len); if( len !=({ struct
_tagged_string _temp132= dest;( unsigned int)( _temp132.last_plus_one - _temp132.curr);})){({
struct _tagged_string _temp133= dest; char* _temp135= _temp133.curr +( int) len;
if( _temp135 < _temp133.base? 1: _temp135 >= _temp133.last_plus_one){ _throw(
Null_Exception);}* _temp135='\000';});} return dest;} struct _tagged_string Cyc_String_strdup(
struct _tagged_string src){ unsigned int len; struct _tagged_string temp; len=
Cyc_String_strlen( src); temp= Cyc_Core_new_string(( int) len); Cyc_String_strncpy(
temp, 0, src, 0, len); return temp;} struct _tagged_string Cyc_String_expand(
struct _tagged_string s, unsigned int sz){ struct _tagged_string temp;
unsigned int slen; slen= Cyc_String_strlen( s); sz= sz > slen? sz: slen; temp=
Cyc_Core_new_string(( int) sz); Cyc_String_strncpy( temp, 0, s, 0, slen); if(
slen !=({ struct _tagged_string _temp136= s;( unsigned int)( _temp136.last_plus_one
- _temp136.curr);})){({ struct _tagged_string _temp137= temp; char* _temp139=
_temp137.curr +( int) slen; if( _temp139 < _temp137.base? 1: _temp139 >=
_temp137.last_plus_one){ _throw( Null_Exception);}* _temp139='\000';});} return
temp;} struct _tagged_string Cyc_String_realloc_str( struct _tagged_string str,
unsigned int sz){ unsigned int maxsizeP=({ struct _tagged_string _temp140= str;(
unsigned int)( _temp140.last_plus_one - _temp140.curr);}); if( maxsizeP ==(
unsigned int) 0){ maxsizeP=( unsigned int) 30 > sz? 30: sz; str= Cyc_Core_new_string((
int) maxsizeP);({ struct _tagged_string _temp141= str; char* _temp143= _temp141.curr
+ 0; if( _temp143 < _temp141.base? 1: _temp143 >= _temp141.last_plus_one){
_throw( Null_Exception);}* _temp143='\000';});} else{ if( sz > maxsizeP){
maxsizeP= maxsizeP *( unsigned int) 2 >( sz *( unsigned int) 5) /( unsigned int)
4? maxsizeP *( unsigned int) 2:( sz *( unsigned int) 5) /( unsigned int) 4; str=
Cyc_String_expand( str, maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring(
struct _tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt <(
unsigned int) 0)? 1:( start +( int) amt) - 1 >( int)({ struct _tagged_string
_temp144= s;( unsigned int)( _temp144.last_plus_one - _temp144.curr);})){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp145=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp145[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp146; _temp146.tag= Cyc_Core_InvalidArg_tag;
_temp146.f1=( struct _tagged_string)({ char* _temp147=( char*)"String::substring";
struct _tagged_string _temp148; _temp148.curr= _temp147; _temp148.base= _temp147;
_temp148.last_plus_one= _temp147 + 18; _temp148;}); _temp146;}); _temp145;}));}{
struct _tagged_string ans= Cyc_Core_new_string(( int)( amt +( unsigned int) 1));{
unsigned int i=( unsigned int) 0; for( 0; i < amt; ++ i){({ struct
_tagged_string _temp149= ans; char* _temp151= _temp149.curr +( int) i; if(
_temp151 < _temp149.base? 1: _temp151 >= _temp149.last_plus_one){ _throw(
Null_Exception);}* _temp151=({ struct _tagged_string _temp152= s; char* _temp154=
_temp152.curr +( int)(( unsigned int) start + i); if( _temp154 < _temp152.base?
1: _temp154 >= _temp152.last_plus_one){ _throw( Null_Exception);}* _temp154;});});}}({
struct _tagged_string _temp155= ans; char* _temp157= _temp155.curr +( int) amt;
if( _temp157 < _temp155.base? 1: _temp157 >= _temp155.last_plus_one){ _throw(
Null_Exception);}* _temp157='\000';}); return ans;}} struct _tagged_string Cyc_String_replace_suffix(
struct _tagged_string src, struct _tagged_string curr_suffix, struct
_tagged_string new_suffix){ unsigned int m=({ struct _tagged_string _temp158=
src;( unsigned int)( _temp158.last_plus_one - _temp158.curr);}); unsigned int n=({
struct _tagged_string _temp159= curr_suffix;( unsigned int)( _temp159.last_plus_one
- _temp159.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp160=( char*)"String::replace_suffix"; struct _tagged_string _temp161;
_temp161.curr= _temp160; _temp161.base= _temp160; _temp161.last_plus_one=
_temp160 + 23; _temp161;}); if( m < n){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp162=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp162[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp163; _temp163.tag= Cyc_Core_InvalidArg_tag;
_temp163.f1= err; _temp163;}); _temp162;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(( int)({ struct _tagged_string _temp164= src; char*
_temp166= _temp164.curr +( int)( m - i); if( _temp166 < _temp164.base? 1:
_temp166 >= _temp164.last_plus_one){ _throw( Null_Exception);}* _temp166;}) !=(
int)({ struct _tagged_string _temp167= curr_suffix; char* _temp169= _temp167.curr
+( int)( n - i); if( _temp169 < _temp167.base? 1: _temp169 >= _temp167.last_plus_one){
_throw( Null_Exception);}* _temp169;})){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp170=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp170[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp171; _temp171.tag= Cyc_Core_InvalidArg_tag;
_temp171.f1= err; _temp171;}); _temp170;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp172= new_suffix;( unsigned int)(
_temp172.last_plus_one - _temp172.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp173= new_suffix;( unsigned int)( _temp173.last_plus_one -
_temp173.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp174=( char*)"String::strpbrk"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 16; _temp175;})); char c='\000'; for( 0; len
> 0?( int)( c=({ struct _tagged_string _temp176= s; char* _temp177= _temp176.curr;
if( _temp177 < _temp176.base? 1: _temp177 >= _temp176.last_plus_one){ _throw(
Null_Exception);}* _temp177;})) != 0: 0; len --,({ struct _tagged_string*
_temp178=& s; struct _tagged_string _temp179=* _temp178; _temp178->curr ++;
_temp179;})){ int j= 0; for( 0;( unsigned int) j <({ struct _tagged_string
_temp180= accept;( unsigned int)( _temp180.last_plus_one - _temp180.curr);}); j
++){ if(( int) c ==( int)({ struct _tagged_string _temp181= accept; char*
_temp183= _temp181.curr + j; if( _temp183 < _temp181.base? 1: _temp183 >=
_temp181.last_plus_one){ _throw( Null_Exception);}* _temp183;})){ return s;}}}
return({ struct _tagged_string _temp184={ 0, 0, 0}; _temp184;});} struct
_tagged_string Cyc_String_strchr( struct _tagged_string s, char c){ int len=(
int) Cyc_String_int_strleno( s, 0,( struct _tagged_string)({ char* _temp185=(
char*)"String::strchr"; struct _tagged_string _temp186; _temp186.curr= _temp185;
_temp186.base= _temp185; _temp186.last_plus_one= _temp185 + 15; _temp186;}));
char c2='\000'; for( 0; len > 0?( int)( c2=({ struct _tagged_string _temp187= s;
char* _temp188= _temp187.curr; if( _temp188 < _temp187.base? 1: _temp188 >=
_temp187.last_plus_one){ _throw( Null_Exception);}* _temp188;})) != 0: 0; len --,({
struct _tagged_string* _temp189=& s; struct _tagged_string _temp190=* _temp189;
_temp189->curr ++; _temp190;})){ if(( int) c2 ==( int) c){ return s;}} return({
struct _tagged_string _temp191={ 0, 0, 0}; _temp191;});} struct _tagged_string
Cyc_String_strrchr( struct _tagged_string s, char c){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp192=( char*)"String::strrchr"; struct
_tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 16; _temp193;})); int i= len - 1; s.curr += i;
for( 0; i >= 0; i --,({ struct _tagged_string* _temp194=& s; struct
_tagged_string _temp195=* _temp194; _temp194->curr --; _temp195;})){ if(( int)({
struct _tagged_string _temp196= s; char* _temp197= _temp196.curr; if( _temp197 <
_temp196.base? 1: _temp197 >= _temp196.last_plus_one){ _throw( Null_Exception);}*
_temp197;}) ==( int) c){ return s;}} return({ struct _tagged_string _temp198={ 0,
0, 0}; _temp198;});} unsigned int Cyc_String_strspn( struct _tagged_string s,
struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno( s, 0,(
struct _tagged_string)({ char* _temp199=( char*)"String::strspn"; struct
_tagged_string _temp200; _temp200.curr= _temp199; _temp200.base= _temp199;
_temp200.last_plus_one= _temp199 + 15; _temp200;}));{ unsigned int i=(
unsigned int) 0; for( 0; i < len; i ++){ int j; for( j= 0;( unsigned int) j <({
struct _tagged_string _temp201= accept;( unsigned int)( _temp201.last_plus_one -
_temp201.curr);}); j ++){ if(( int)({ struct _tagged_string _temp202= s; char*
_temp204= _temp202.curr +( int) i; if( _temp204 < _temp202.base? 1: _temp204 >=
_temp202.last_plus_one){ _throw( Null_Exception);}* _temp204;}) ==( int)({
struct _tagged_string _temp205= accept; char* _temp207= _temp205.curr + j; if(
_temp207 < _temp205.base? 1: _temp207 >= _temp205.last_plus_one){ _throw(
Null_Exception);}* _temp207;})){ break;}} if(( unsigned int) j ==({ struct
_tagged_string _temp208= accept;( unsigned int)( _temp208.last_plus_one -
_temp208.curr);})){ return i;}}} return len;} struct Cyc_List_List* Cyc_String_explode(
struct _tagged_string s){ struct Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen(
s) -( unsigned int) 1); for( 0; i >= 0; i --){ result=({ struct Cyc_List_List*
_temp209=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp209->hd=( void*)(( int)({ struct _tagged_string _temp210= s; char* _temp212=
_temp210.curr + i; if( _temp212 < _temp210.base? 1: _temp212 >= _temp210.last_plus_one){
_throw( Null_Exception);}* _temp212;})); _temp209->tl= result; _temp209;});}}
return result;} struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
chars){ struct _tagged_string s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); unsigned int i=( unsigned int) 0; while( chars !=
0) {({ struct _tagged_string _temp213= s; char* _temp215= _temp213.curr +( int)(
i ++); if( _temp215 < _temp213.base? 1: _temp215 >= _temp213.last_plus_one){
_throw( Null_Exception);}* _temp215=( char)(( int) chars->hd);}); chars= chars->tl;}
return s;} int Cyc_String_to_int( struct _tagged_string s, int* offset){ int ans=
0; int sn= 1; int digit_seen= 0; int i= offset == 0? 0:* offset; while((
unsigned int) i <({ struct _tagged_string _temp216= s;( unsigned int)( _temp216.last_plus_one
- _temp216.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp217= s;
char* _temp219= _temp217.curr + i; if( _temp219 < _temp217.base? 1: _temp219 >=
_temp217.last_plus_one){ _throw( Null_Exception);}* _temp219;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp220= s;( unsigned int)(
_temp220.last_plus_one - _temp220.curr);})?( int)({ struct _tagged_string
_temp221= s; char* _temp223= _temp221.curr + i; if( _temp223 < _temp221.base? 1:
_temp223 >= _temp221.last_plus_one){ _throw( Null_Exception);}* _temp223;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp224= s;( unsigned int)( _temp224.last_plus_one - _temp224.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp225= s; char* _temp227= _temp225.curr + i; if(
_temp227 < _temp225.base? 1: _temp227 >= _temp225.last_plus_one){ _throw(
Null_Exception);}* _temp227;})): 0) { i ++;} while(( unsigned int) i <({ struct
_tagged_string _temp228= s;( unsigned int)( _temp228.last_plus_one - _temp228.curr);})?
Cyc_Character_isdigit(({ struct _tagged_string _temp229= s; char* _temp231=
_temp229.curr + i; if( _temp231 < _temp229.base? 1: _temp231 >= _temp229.last_plus_one){
_throw( Null_Exception);}* _temp231;})): 0) { digit_seen= 1; ans= ans * 10 +(
int)(( int)({ struct _tagged_string _temp232= s; char* _temp234= _temp232.curr +
i; if( _temp234 < _temp232.base? 1: _temp234 >= _temp232.last_plus_one){ _throw(
Null_Exception);}* _temp234;}) -( int)'0'); i ++;} ans= ans * sn; if( !
digit_seen){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp235=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp235[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp236; _temp236.tag= Cyc_Core_InvalidArg_tag;
_temp236.f1=( struct _tagged_string)({ char* _temp237=( char*)"String::to_int";
struct _tagged_string _temp238; _temp238.curr= _temp237; _temp238.base= _temp237;
_temp238.last_plus_one= _temp237 + 15; _temp238;}); _temp236;}); _temp235;}));}
if( offset != 0){* offset= i;} return ans;} double Cyc_String_to_double( struct
_tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:* offset; while(( unsigned int) i <({
struct _tagged_string _temp239= s;( unsigned int)( _temp239.last_plus_one -
_temp239.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp240= s;
char* _temp242= _temp240.curr + i; if( _temp242 < _temp240.base? 1: _temp242 >=
_temp240.last_plus_one){ _throw( Null_Exception);}* _temp242;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp243= s;( unsigned int)(
_temp243.last_plus_one - _temp243.curr);})?( int)({ struct _tagged_string
_temp244= s; char* _temp246= _temp244.curr + i; if( _temp246 < _temp244.base? 1:
_temp246 >= _temp244.last_plus_one){ _throw( Null_Exception);}* _temp246;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp247= s;( unsigned int)( _temp247.last_plus_one - _temp247.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp248= s; char* _temp250= _temp248.curr + i; if(
_temp250 < _temp248.base? 1: _temp250 >= _temp248.last_plus_one){ _throw(
Null_Exception);}* _temp250;})): 0) { digit_seen= 1; ans= ans *( double) 10.0 +(
double)(( int)({ struct _tagged_string _temp251= s; char* _temp253= _temp251.curr
+ i; if( _temp253 < _temp251.base? 1: _temp253 >= _temp251.last_plus_one){
_throw( Null_Exception);}* _temp253;}) -( int)'0'); i ++;} if(( unsigned int) i
<({ struct _tagged_string _temp254= s;( unsigned int)( _temp254.last_plus_one -
_temp254.curr);})?( int)({ struct _tagged_string _temp255= s; char* _temp257=
_temp255.curr + i; if( _temp257 < _temp255.base? 1: _temp257 >= _temp255.last_plus_one){
_throw( Null_Exception);}* _temp257;}) ==( int)'.': 0){ i ++;{ double divisor=(
double) 0.1; while(( unsigned int) i <({ struct _tagged_string _temp258= s;(
unsigned int)( _temp258.last_plus_one - _temp258.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp259= s; char* _temp261= _temp259.curr + i; if(
_temp261 < _temp259.base? 1: _temp261 >= _temp259.last_plus_one){ _throw(
Null_Exception);}* _temp261;})): 0) { digit_seen= 1; ans= ans +( double)(( int)({
struct _tagged_string _temp262= s; char* _temp264= _temp262.curr + i; if(
_temp264 < _temp262.base? 1: _temp264 >= _temp262.last_plus_one){ _throw(
Null_Exception);}* _temp264;}) -( int)'0') * divisor; divisor= divisor /( double)
10.0; i ++;}}} if( ! digit_seen){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp265=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp265[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp266; _temp266.tag= Cyc_Core_InvalidArg_tag;
_temp266.f1=( struct _tagged_string)({ char* _temp267=( char*)"String::to_double";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 18; _temp268;}); _temp266;}); _temp265;}));}
if(( unsigned int) i <({ struct _tagged_string _temp269= s;( unsigned int)(
_temp269.last_plus_one - _temp269.curr);})?( int)({ struct _tagged_string
_temp270= s; char* _temp272= _temp270.curr + i; if( _temp272 < _temp270.base? 1:
_temp272 >= _temp270.last_plus_one){ _throw( Null_Exception);}* _temp272;}) ==(
int)'e'? 1:( int)({ struct _tagged_string _temp273= s; char* _temp275= _temp273.curr
+ i; if( _temp275 < _temp273.base? 1: _temp275 >= _temp273.last_plus_one){
_throw( Null_Exception);}* _temp275;}) ==( int)'E': 0){ i ++;{ int exponent= 0;
int exp_sign= 1; digit_seen= 0; if(( unsigned int) i <({ struct _tagged_string
_temp276= s;( unsigned int)( _temp276.last_plus_one - _temp276.curr);})?( int)({
struct _tagged_string _temp277= s; char* _temp279= _temp277.curr + i; if(
_temp279 < _temp277.base? 1: _temp279 >= _temp277.last_plus_one){ _throw(
Null_Exception);}* _temp279;}) ==( int)'-': 0){ i ++; exp_sign= - 1;} else{ if((
unsigned int) i <({ struct _tagged_string _temp280= s;( unsigned int)( _temp280.last_plus_one
- _temp280.curr);})?( int)({ struct _tagged_string _temp281= s; char* _temp283=
_temp281.curr + i; if( _temp283 < _temp281.base? 1: _temp283 >= _temp281.last_plus_one){
_throw( Null_Exception);}* _temp283;}) ==( int)'+': 0){ i ++;}} while((
unsigned int) i <({ struct _tagged_string _temp284= s;( unsigned int)( _temp284.last_plus_one
- _temp284.curr);})? Cyc_Character_isdigit(({ struct _tagged_string _temp285= s;
char* _temp287= _temp285.curr + i; if( _temp287 < _temp285.base? 1: _temp287 >=
_temp285.last_plus_one){ _throw( Null_Exception);}* _temp287;})): 0) {
digit_seen= 1; exponent= exponent * 10 +( int)(( int)({ struct _tagged_string
_temp288= s; char* _temp290= _temp288.curr + i; if( _temp290 < _temp288.base? 1:
_temp290 >= _temp288.last_plus_one){ _throw( Null_Exception);}* _temp290;}) -(
int)'0'); i ++;} if( ! digit_seen){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp291=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp291[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp292; _temp292.tag= Cyc_Core_InvalidArg_tag;
_temp292.f1=( struct _tagged_string)({ char* _temp293=( char*)"String::to_double";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 18; _temp294;}); _temp292;}); _temp291;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){* offset= i;} return ans *( double) sn;}