#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char*
tag;}; extern int Cyc_Character_isdigit( char c); extern int Cyc_Character_isspace(
char c); extern char Cyc_Character_toupper( char c); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; extern unsigned int Cyc_String_strlen( struct _tagged_string s);
extern unsigned int Cyc_String_strleno( struct _tagged_string s, int ofs);
extern int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
_tagged_string* s2); extern int Cyc_String_strcmpo( struct _tagged_string s1,
int ofs1, struct _tagged_string s2, int ofs2); extern int Cyc_String_strncmp(
struct _tagged_string s1, int ofs1, struct _tagged_string s2, int ofs2,
unsigned int len); extern int Cyc_String_strncasecmp( struct _tagged_string s1,
int ofs1, struct _tagged_string s2, int ofs2, unsigned int len); extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int n); extern int
Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*); extern
struct _tagged_string Cyc_String_strcat( struct _tagged_string dest, struct
_tagged_string src); extern struct _tagged_string Cyc_String_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs); extern struct
_tagged_string Cyc_String_strconcat( struct _tagged_string, struct
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
struct _tagged_string, struct _tagged_string, struct _tagged_string); extern int
Cyc_String_strchr( struct _tagged_string s, int ofs, char c); extern int Cyc_String_strrchr(
struct _tagged_string s, int ofs, char c); extern int Cyc_String_strpbrk( struct
_tagged_string s, int ofs, struct _tagged_string accept); extern int Cyc_String_strspn(
struct _tagged_string s, int ofs, struct _tagged_string accept); extern struct
Cyc_List_List* Cyc_String_explode( struct _tagged_string s); extern struct
_tagged_string Cyc_String_implode( struct Cyc_List_List* c); extern int Cyc_String_to_int(
struct _tagged_string s, int* offset); extern double Cyc_String_to_double(
struct _tagged_string s, int* offset); extern double atan( double); extern
double cos( double); extern double sin( double); extern double tan( double);
extern double tanh( double); extern double frexp( double, int*); extern double
modf( double, double*); extern double ceil( double); extern double fabs( double);
extern double floor( double); extern double acos( double); extern double asin(
double); extern double atan2( double, double); extern double cosh( double);
extern double sinh( double); extern double exp( double); extern double ldexp(
double, int); extern double log( double); extern double log10( double); extern
double pow( double, double); extern double sqrt( double); extern double fmod(
double, double); extern double infinity(); extern double nan(); extern int isnan(
double); extern int isinf( double); extern int finite( double); extern double
copysign( double, double); extern int ilogb( double); extern double asinh(
double); extern double cbrt( double); extern double nextafter( double, double);
extern double rint( double); extern double scalbn( double, int); extern double
log1p( double); extern double expm1( double); extern double acosh( double);
extern double atanh( double); extern double remainder( double, double); extern
double gamma( double); extern double gamma_r( double, int*); extern double
lgamma( double); extern double lgamma_r( double, int*); extern double erf(
double); extern double erfc( double); extern double y0( double); extern double
y1( double); extern double yn( int, double); extern double j0( double); extern
double j1( double); extern double jn( int, double); extern double hypot( double,
double); extern double cabs(); extern double drem( double, double); extern float
atanf( float); extern float cosf( float); extern float sinf( float); extern
float tanf( float); extern float tanhf( float); extern float frexpf( float, int*);
extern float modff( float, float*); extern float ceilf( float); extern float
fabsf( float); extern float floorf( float); extern float acosf( float); extern
float asinf( float); extern float atan2f( float, float); extern float coshf(
float); extern float sinhf( float); extern float expf( float); extern float
ldexpf( float, int); extern float logf( float); extern float log10f( float);
extern float powf( float, float); extern float sqrtf( float); extern float fmodf(
float, float); extern float infinityf(); extern float nanf(); extern int isnanf(
float); extern int isinff( float); extern int finitef( float); extern float
copysignf( float, float); extern int ilogbf( float); extern float asinhf( float);
extern float cbrtf( float); extern float nextafterf( float, float); extern float
rintf( float); extern float scalbnf( float, int); extern float log1pf( float);
extern float expm1f( float); extern float acoshf( float); extern float atanhf(
float); extern float remainderf( float, float); extern float gammaf( float);
extern float gammaf_r( float, int*); extern float lgammaf( float); extern float
lgammaf_r( float, int*); extern float erff( float); extern float erfcf( float);
extern float y0f( float); extern float y1f( float); extern float ynf( int, float);
extern float j0f( float); extern float j1f( float); extern float jnf( int, float);
extern float hypotf( float, float); extern float cabsf(); extern float dremf(
float, float); extern int signgam; struct Cyc_Math___exception{ int type; char*
name; double arg1; double arg2; double retval; int err;}; extern int matherr(
struct Cyc_Math___exception* e); unsigned int Cyc_String_strlen( struct
_tagged_string s){ unsigned int i; for( i= 0; i <({ struct _tagged_string _temp0=
s;( unsigned int)( _temp0.last_plus_one - _temp0.curr);}); i ++){ if(( int)({
struct _tagged_string _temp1= s; char* _temp3= _temp1.curr +( int) i; if( _temp3
< _temp1.base? 1: _temp3 >= _temp1.last_plus_one){ _throw( Null_Exception);}*
_temp3;}) ==( int)'\000'){ return i;}} return i;} static unsigned int Cyc_String_int_strleno(
struct _tagged_string s, int ofs, struct _tagged_string error){ if((
unsigned int) ofs >({ struct _tagged_string _temp4= s;( unsigned int)( _temp4.last_plus_one
- _temp4.curr);})? 1: ofs < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp5=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp5=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=
error};( struct _xenum_struct*) _temp5;}));}{ int i; for( i= ofs;( unsigned int)
i <({ struct _tagged_string _temp6= s;( unsigned int)( _temp6.last_plus_one -
_temp6.curr);}); i ++){ if(( int)({ struct _tagged_string _temp7= s; char*
_temp9= _temp7.curr + i; if( _temp9 < _temp7.base? 1: _temp9 >= _temp7.last_plus_one){
_throw( Null_Exception);}* _temp9;}) ==( int)'\000'){ break;}} return(
unsigned int)( i - ofs);}} unsigned int Cyc_String_strleno( struct
_tagged_string s, int ofs){ return Cyc_String_int_strleno( s, ofs,( struct
_tagged_string)({ char* _temp10=( char*)"String::strleno"; struct _tagged_string
_temp11; _temp11.curr= _temp10; _temp11.base= _temp10; _temp11.last_plus_one=
_temp10 + 16; _temp11;}));} static int Cyc_String_case_cmp( char c1, char c2){
return( int) c1 -( int) c2;} static int Cyc_String_nocase_cmp( char c1, char c2){
return Cyc_String_case_cmp( Cyc_Character_toupper( c1), Cyc_Character_toupper(
c2));} static int Cyc_String_cmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2, int(*
f)( char, char)){ unsigned int min_length= len1 < len2? len1: len2; int i= - 1;
while(( unsigned int)( ++ i) < min_length) { int diff= f(({ struct
_tagged_string _temp12= s1; char* _temp14= _temp12.curr +( i + ofs1); if(
_temp14 < _temp12.base? 1: _temp14 >= _temp12.last_plus_one){ _throw(
Null_Exception);}* _temp14;}),({ struct _tagged_string _temp15= s2; char*
_temp17= _temp15.curr +( i + ofs2); if( _temp17 < _temp15.base? 1: _temp17 >=
_temp15.last_plus_one){ _throw( Null_Exception);}* _temp17;})); if( diff != 0){
return diff;}} return( int) len1 -( int) len2;} int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{
unsigned int len1= Cyc_String_int_strleno( s1, 0,( struct _tagged_string)({ char*
_temp18=( char*)"String::strcmp"; struct _tagged_string _temp19; _temp19.curr=
_temp18; _temp19.base= _temp18; _temp19.last_plus_one= _temp18 + 15; _temp19;}));
unsigned int len2= Cyc_String_int_strleno( s2, 0,( struct _tagged_string)({ char*
_temp20=( char*)"String::strcmp"; struct _tagged_string _temp21; _temp21.curr=
_temp20; _temp21.base= _temp20; _temp21.last_plus_one= _temp20 + 15; _temp21;}));
return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}} int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2){ return Cyc_String_strcmp(*
s1,* s2);} int Cyc_String_strcmpo( struct _tagged_string s1, int ofs1, struct
_tagged_string s2, int ofs2){ unsigned int len1= Cyc_String_int_strleno( s1,
ofs1,( struct _tagged_string)({ char* _temp22=( char*)"String::strcmp"; struct
_tagged_string _temp23; _temp23.curr= _temp22; _temp23.base= _temp22; _temp23.last_plus_one=
_temp22 + 15; _temp23;})); unsigned int len2= Cyc_String_int_strleno( s2, ofs2,(
struct _tagged_string)({ char* _temp24=( char*)"String::strcmp"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 15; _temp25;})); return Cyc_String_cmp( s1, ofs1, len1, s2, ofs2, len2,
Cyc_String_case_cmp);} static int Cyc_String_ncmp( struct _tagged_string s1, int
ofs1, unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2,
unsigned int n, int(* f)( char, char)){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int min_len= len1 > len2? len2: len1; unsigned int bound= min_len > n?
n: min_len;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){ int retc; if((
retc= f(({ struct _tagged_string _temp26= s1; char* _temp28= _temp26.curr +( i +
ofs1); if( _temp28 < _temp26.base? 1: _temp28 >= _temp26.last_plus_one){ _throw(
Null_Exception);}* _temp28;}),({ struct _tagged_string _temp29= s2; char*
_temp31= _temp29.curr +( i + ofs2); if( _temp31 < _temp29.base? 1: _temp31 >=
_temp29.last_plus_one){ _throw( Null_Exception);}* _temp31;}))) != 0){ return
retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_string s1, int ofs1,
struct _tagged_string s2, int ofs2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, ofs1,( struct _tagged_string)({ char* _temp32=( char*)"String::strncmp";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 16; _temp33;})); unsigned int len2= Cyc_String_int_strleno(
s2, ofs2,( struct _tagged_string)({ char* _temp34=( char*)"String::strncmp";
struct _tagged_string _temp35; _temp35.curr= _temp34; _temp35.base= _temp34;
_temp35.last_plus_one= _temp34 + 16; _temp35;})); return Cyc_String_ncmp( s1,
ofs1, len1, s2, ofs2, len2, n, Cyc_String_case_cmp);} int Cyc_String_strncasecmp(
struct _tagged_string s1, int ofs1, struct _tagged_string s2, int ofs2,
unsigned int n){ unsigned int len1= Cyc_String_int_strleno( s1, ofs1,( struct
_tagged_string)({ char* _temp36=( char*)"String::strncasecmp"; struct
_tagged_string _temp37; _temp37.curr= _temp36; _temp37.base= _temp36; _temp37.last_plus_one=
_temp36 + 20; _temp37;})); unsigned int len2= Cyc_String_int_strleno( s2, ofs2,(
struct _tagged_string)({ char* _temp38=( char*)"String::strncasecmp"; struct
_tagged_string _temp39; _temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one=
_temp38 + 20; _temp39;})); return Cyc_String_ncmp( s1, ofs1, len1, s2, ofs2,
len2, n, Cyc_String_nocase_cmp);} int Cyc_String_zstrcmp( struct _tagged_string
a, struct _tagged_string b){ if( a.curr == b.curr){ return 0;}{ unsigned int as=({
struct _tagged_string _temp40= a;( unsigned int)( _temp40.last_plus_one -
_temp40.curr);}); unsigned int bs=({ struct _tagged_string _temp41= b;(
unsigned int)( _temp41.last_plus_one - _temp41.curr);}); unsigned int min_length=
as < bs? as: bs; int i= - 1; while(( unsigned int)( ++ i) < min_length) { int
diff=( int)({ struct _tagged_string _temp42= a; char* _temp44= _temp42.curr + i;
if( _temp44 < _temp42.base? 1: _temp44 >= _temp42.last_plus_one){ _throw(
Null_Exception);}* _temp44;}) -( int)({ struct _tagged_string _temp45= b; char*
_temp47= _temp45.curr + i; if( _temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one){
_throw( Null_Exception);}* _temp47;}); if( diff != 0){ return diff;}} return(
int) as -( int) bs;}} int Cyc_String_zstrncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int s1size=({ struct _tagged_string _temp48= s1;( unsigned int)(
_temp48.last_plus_one - _temp48.curr);}); unsigned int s2size=({ struct
_tagged_string _temp49= s2;( unsigned int)( _temp49.last_plus_one - _temp49.curr);});
unsigned int min_size= s1size > s2size? s2size: s1size; unsigned int bound=
min_size > n? n: min_size;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){
if(( int)({ struct _tagged_string _temp50= s1; char* _temp52= _temp50.curr + i;
if( _temp52 < _temp50.base? 1: _temp52 >= _temp50.last_plus_one){ _throw(
Null_Exception);}* _temp52;}) <( int)({ struct _tagged_string _temp53= s2; char*
_temp55= _temp53.curr + i; if( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one){
_throw( Null_Exception);}* _temp55;})){ return - 1;} else{ if(( int)({ struct
_tagged_string _temp56= s2; char* _temp58= _temp56.curr + i; if( _temp58 <
_temp56.base? 1: _temp58 >= _temp56.last_plus_one){ _throw( Null_Exception);}*
_temp58;}) <( int)({ struct _tagged_string _temp59= s1; char* _temp61= _temp59.curr
+ i; if( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one){ _throw(
Null_Exception);}* _temp61;})){ return 1;}}}} if( min_size <= bound){ return 0;}
if( s1size < s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp(
struct _tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(*
a,* b);} static struct _tagged_string Cyc_String_int_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs, struct
_tagged_string error){ int i; unsigned int dsize; unsigned int slen;
unsigned int dlen; dsize=({ struct _tagged_string _temp62= dest;( unsigned int)(
_temp62.last_plus_one - _temp62.curr);}); dlen= Cyc_String_strlen( dest); slen=
Cyc_String_int_strleno( src, srcOfs, error); if( slen + dlen <= dsize){ for( i=
0;( unsigned int) i < slen; i ++){({ struct _tagged_string _temp63= dest; char*
_temp65= _temp63.curr +( int)(( unsigned int) i + dlen); if( _temp65 < _temp63.base?
1: _temp65 >= _temp63.last_plus_one){ _throw( Null_Exception);}* _temp65=({
struct _tagged_string _temp66= src; char* _temp68= _temp66.curr +( i + srcOfs);
if( _temp68 < _temp66.base? 1: _temp68 >= _temp66.last_plus_one){ _throw(
Null_Exception);}* _temp68;});});} if(( unsigned int) i != dsize){({ struct
_tagged_string _temp69= dest; char* _temp71= _temp69.curr +( int)(( unsigned int)
i + dlen); if( _temp71 < _temp69.base? 1: _temp71 >= _temp69.last_plus_one){
_throw( Null_Exception);}* _temp71='\000';});}} else{( void) _throw(({ struct
Cyc_Core_InvalidArg_struct* _temp72=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp72=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1= error};( struct _xenum_struct*) _temp72;}));}
return dest;} struct _tagged_string Cyc_String_strcat( struct _tagged_string
dest, struct _tagged_string src){ return Cyc_String_int_strcato( dest, src, 0,(
struct _tagged_string)({ char* _temp73=( char*)"String::strcat"; struct
_tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73; _temp74.last_plus_one=
_temp73 + 15; _temp74;}));} struct _tagged_string Cyc_String_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs){ return Cyc_String_int_strcato(
dest, src, srcOfs,( struct _tagged_string)({ char* _temp75=( char*)"String::strcato";
struct _tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75;
_temp76.last_plus_one= _temp75 + 16; _temp76;}));} struct _tagged_string Cyc_String_strconcat(
struct _tagged_string a, struct _tagged_string b){ unsigned int alen= Cyc_String_strlen(
a); unsigned int blen= Cyc_String_strlen( b); struct _tagged_string ans= Cyc_Core_new_string((
int)( alen + blen)); int i; int j; for( i= 0;( unsigned int) i < alen; ++ i){({
struct _tagged_string _temp77= ans; char* _temp79= _temp77.curr + i; if( _temp79
< _temp77.base? 1: _temp79 >= _temp77.last_plus_one){ _throw( Null_Exception);}*
_temp79=({ struct _tagged_string _temp80= a; char* _temp82= _temp80.curr + i;
if( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){ _throw(
Null_Exception);}* _temp82;});});} for( j= 0;( unsigned int) j < blen; ++ j){({
struct _tagged_string _temp83= ans; char* _temp85= _temp83.curr +( i + j); if(
_temp85 < _temp83.base? 1: _temp85 >= _temp83.last_plus_one){ _throw(
Null_Exception);}* _temp85=({ struct _tagged_string _temp86= b; char* _temp88=
_temp86.curr + j; if( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one){
_throw( Null_Exception);}* _temp88;});});} return ans;} struct _tagged_string
Cyc_String_strconcat_l( struct Cyc_List_List* strs){ unsigned int len;
unsigned int total_len=( unsigned int) 0; struct Cyc_List_List* lens=({ struct
Cyc_List_List* _temp89=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp89->hd=( void*)(( unsigned int) 0); _temp89->tl= 0; _temp89;}); struct Cyc_List_List*
end= lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p= p->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*) p->hd)); total_len += len; end->tl=({ struct Cyc_List_List*
_temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*) len; _temp90->tl= 0; _temp90;}); end= end->tl;}} lens= lens->tl;{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs= strs->tl; lens= lens->tl;} return
ans;}} struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List* strs,
struct _tagged_string separator){ if( strs == 0){ return( struct _tagged_string)({
char* _temp91=( char*)""; struct _tagged_string _temp92; _temp92.curr= _temp91;
_temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 1; _temp92;});} if( strs->tl
== 0){ return Cyc_String_strdup(*(( struct _tagged_string*) strs->hd));}{ struct
Cyc_List_List* p= strs; struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp93->hd=( void*)(( unsigned int) 0); _temp93->tl= 0; _temp93;}); struct Cyc_List_List*
end= lens; unsigned int len=( unsigned int) 0; unsigned int total_len=(
unsigned int) 0; unsigned int list_len=( unsigned int) 0; for( 0; p != 0; p= p->tl){
len= Cyc_String_strlen(*(( struct _tagged_string*) p->hd)); total_len += len;
end->tl=({ struct Cyc_List_List* _temp94=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp94->hd=( void*) len; _temp94->tl= 0;
_temp94;}); end= end->tl; ++ list_len;} lens= lens->tl;{ unsigned int seplen=
Cyc_String_strlen( separator); total_len +=( list_len -( unsigned int) 1) *
seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int) total_len);
unsigned int i=( unsigned int) 0; while( strs->tl != 0) { struct _tagged_string
next=*(( struct _tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; Cyc_String_strncpy( ans,( int) i,
separator, 0, seplen); i += seplen; strs= strs->tl; lens= lens->tl;} Cyc_String_strncpy(
ans,( int) i,*(( struct _tagged_string*) strs->hd), 0,( unsigned int) lens->hd);
return ans;}}}} struct _tagged_string Cyc_String_strncpy( struct _tagged_string
dest, int destOfs, struct _tagged_string src, int srcOfs, unsigned int n){
unsigned int destSz=({ struct _tagged_string _temp95= dest;( unsigned int)(
_temp95.last_plus_one - _temp95.curr);}); unsigned int srcSz=({ struct
_tagged_string _temp96= src;( unsigned int)( _temp96.last_plus_one - _temp96.curr);});
if( destOfs < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp97=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp97=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp98=( char*)"String::strncpy"; struct
_tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 16; _temp99;})};( struct _xenum_struct*) _temp97;}));} if( srcOfs < 0?
1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp100=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp100=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp101=( char*)"String::strncpy";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 16; _temp102;})};( struct _xenum_struct*)
_temp100;}));} if( n <=( unsigned int) 0){ return dest;} if(( unsigned int)
destOfs + n > destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp103=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp103=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp104=( char*)"String::strncpy";
struct _tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 16; _temp105;})};( struct _xenum_struct*)
_temp103;}));}{ int i; for( i= 0;( unsigned int) i < n; i ++){ char srcChar=({
struct _tagged_string _temp106= src; char* _temp108= _temp106.curr +( srcOfs + i);
if( _temp108 < _temp106.base? 1: _temp108 >= _temp106.last_plus_one){ _throw(
Null_Exception);}* _temp108;}); if(( int) srcChar ==( int)'\000'){ break;}({
struct _tagged_string _temp109= dest; char* _temp111= _temp109.curr +( destOfs +
i); if( _temp111 < _temp109.base? 1: _temp111 >= _temp109.last_plus_one){ _throw(
Null_Exception);}* _temp111= srcChar;});} for( 0;( unsigned int) i < n; i ++){({
struct _tagged_string _temp112= dest; char* _temp114= _temp112.curr +( destOfs +
i); if( _temp114 < _temp112.base? 1: _temp114 >= _temp112.last_plus_one){ _throw(
Null_Exception);}* _temp114='\000';});} return dest;}} struct _tagged_string Cyc_String_zstrncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int destSz=({ struct _tagged_string _temp115= dest;(
unsigned int)( _temp115.last_plus_one - _temp115.curr);}); unsigned int srcSz=({
struct _tagged_string _temp116= src;( unsigned int)( _temp116.last_plus_one -
_temp116.curr);}); if( destOfs < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp117=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp117=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp118=( char*)"String::zstrncpy"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 17; _temp119;})};( struct _xenum_struct*)
_temp117;}));} if( srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs
>= srcSz: 0)){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp120=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp120=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp121=( char*)"String::zstrncpy"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 17; _temp122;})};( struct _xenum_struct*)
_temp120;}));} if( n <=( unsigned int) 0){ return dest;} if(( unsigned int)
destOfs + n > destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp123=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp123=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp124=( char*)"String::zstrncpy";
struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 17; _temp125;})};( struct _xenum_struct*)
_temp123;}));}{ int i; for( i= 0;( unsigned int) i < n; i ++){({ struct
_tagged_string _temp126= dest; char* _temp128= _temp126.curr +( destOfs + i);
if( _temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one){ _throw(
Null_Exception);}* _temp128=({ struct _tagged_string _temp129= src; char*
_temp131= _temp129.curr +( srcOfs + i); if( _temp131 < _temp129.base? 1:
_temp131 >= _temp129.last_plus_one){ _throw( Null_Exception);}* _temp131;});});}
return dest;}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string
dest, struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp132= dest;(
unsigned int)( _temp132.last_plus_one - _temp132.curr);})){({ struct
_tagged_string _temp133= dest; char* _temp135= _temp133.curr +( int) len; if(
_temp135 < _temp133.base? 1: _temp135 >= _temp133.last_plus_one){ _throw(
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
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp145=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp145=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp146=( char*)"String::substring";
struct _tagged_string _temp147; _temp147.curr= _temp146; _temp147.base= _temp146;
_temp147.last_plus_one= _temp146 + 18; _temp147;})};( struct _xenum_struct*)
_temp145;}));}{ struct _tagged_string ans= Cyc_Core_new_string(( int)( amt +(
unsigned int) 1));{ unsigned int i=( unsigned int) 0; for( 0; i < amt; ++ i){({
struct _tagged_string _temp148= ans; char* _temp150= _temp148.curr +( int) i;
if( _temp150 < _temp148.base? 1: _temp150 >= _temp148.last_plus_one){ _throw(
Null_Exception);}* _temp150=({ struct _tagged_string _temp151= s; char* _temp153=
_temp151.curr +( int)(( unsigned int) start + i); if( _temp153 < _temp151.base?
1: _temp153 >= _temp151.last_plus_one){ _throw( Null_Exception);}* _temp153;});});}}({
struct _tagged_string _temp154= ans; char* _temp156= _temp154.curr +( int) amt;
if( _temp156 < _temp154.base? 1: _temp156 >= _temp154.last_plus_one){ _throw(
Null_Exception);}* _temp156='\000';}); return ans;}} struct _tagged_string Cyc_String_replace_suffix(
struct _tagged_string src, struct _tagged_string curr_suffix, struct
_tagged_string new_suffix){ unsigned int m=({ struct _tagged_string _temp157=
src;( unsigned int)( _temp157.last_plus_one - _temp157.curr);}); unsigned int n=({
struct _tagged_string _temp158= curr_suffix;( unsigned int)( _temp158.last_plus_one
- _temp158.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp159=( char*)"String::replace_suffix"; struct _tagged_string _temp160;
_temp160.curr= _temp159; _temp160.base= _temp159; _temp160.last_plus_one=
_temp159 + 23; _temp160;}); if( m < n){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp161=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp161=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=
err};( struct _xenum_struct*) _temp161;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(( int)({ struct _tagged_string _temp162= src; char*
_temp164= _temp162.curr +( int)( m - i); if( _temp164 < _temp162.base? 1:
_temp164 >= _temp162.last_plus_one){ _throw( Null_Exception);}* _temp164;}) !=(
int)({ struct _tagged_string _temp165= curr_suffix; char* _temp167= _temp165.curr
+( int)( n - i); if( _temp167 < _temp165.base? 1: _temp167 >= _temp165.last_plus_one){
_throw( Null_Exception);}* _temp167;})){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp168=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp168=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=
err};( struct _xenum_struct*) _temp168;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp169= new_suffix;( unsigned int)(
_temp169.last_plus_one - _temp169.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp170= new_suffix;( unsigned int)( _temp170.last_plus_one -
_temp170.curr);})); return ans;}} int Cyc_String_strpbrk( struct _tagged_string
s, int ofs, struct _tagged_string accept){ int endOfs=( int)( Cyc_String_int_strleno(
s, ofs,( struct _tagged_string)({ char* _temp171=( char*)"String::strpbrk";
struct _tagged_string _temp172; _temp172.curr= _temp171; _temp172.base= _temp171;
_temp172.last_plus_one= _temp171 + 16; _temp172;})) +( unsigned int) ofs);{ int
i= ofs; for( 0; i < endOfs; i ++){ int j= 0; for( 0;( unsigned int) j <({ struct
_tagged_string _temp173= accept;( unsigned int)( _temp173.last_plus_one -
_temp173.curr);}); j ++){ if(( int)({ struct _tagged_string _temp174= s; char*
_temp176= _temp174.curr + i; if( _temp176 < _temp174.base? 1: _temp176 >=
_temp174.last_plus_one){ _throw( Null_Exception);}* _temp176;}) ==( int)({
struct _tagged_string _temp177= accept; char* _temp179= _temp177.curr + j; if(
_temp179 < _temp177.base? 1: _temp179 >= _temp177.last_plus_one){ _throw(
Null_Exception);}* _temp179;})){ return i;}}}} return - 1;} int Cyc_String_strchr(
struct _tagged_string s, int ofs, char c){ int endOfs=( int)( Cyc_String_int_strleno(
s, ofs,( struct _tagged_string)({ char* _temp180=( char*)"String::strchr";
struct _tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 15; _temp181;})) +( unsigned int) ofs);{ int
i= ofs; for( 0; i < endOfs; i ++){ if(( int)({ struct _tagged_string _temp182= s;
char* _temp184= _temp182.curr + i; if( _temp184 < _temp182.base? 1: _temp184 >=
_temp182.last_plus_one){ _throw( Null_Exception);}* _temp184;}) ==( int) c){
return i;}}} return - 1;} int Cyc_String_strrchr( struct _tagged_string s, int
ofs, char c){ int endOfs=( int)( Cyc_String_int_strleno( s, ofs,( struct
_tagged_string)({ char* _temp185=( char*)"String::strchr"; struct _tagged_string
_temp186; _temp186.curr= _temp185; _temp186.base= _temp185; _temp186.last_plus_one=
_temp185 + 15; _temp186;})) +( unsigned int) ofs);{ int i= endOfs - 1; for( 0; i
>= ofs; i --){ if(( int)({ struct _tagged_string _temp187= s; char* _temp189=
_temp187.curr + i; if( _temp189 < _temp187.base? 1: _temp189 >= _temp187.last_plus_one){
_throw( Null_Exception);}* _temp189;}) ==( int) c){ return i;}}} return - 1;}
int Cyc_String_strspn( struct _tagged_string s, int ofs, struct _tagged_string
accept){ int endOfs=( int)( Cyc_String_int_strleno( s, ofs,( struct
_tagged_string)({ char* _temp190=( char*)"String::strspn"; struct _tagged_string
_temp191; _temp191.curr= _temp190; _temp191.base= _temp190; _temp191.last_plus_one=
_temp190 + 15; _temp191;})) +( unsigned int) ofs);{ int i= ofs; for( 0; i <
endOfs; i ++){ int j; for( j= 0;( unsigned int) j <({ struct _tagged_string
_temp192= accept;( unsigned int)( _temp192.last_plus_one - _temp192.curr);}); j
++){ if(( int)({ struct _tagged_string _temp193= s; char* _temp195= _temp193.curr
+ i; if( _temp195 < _temp193.base? 1: _temp195 >= _temp193.last_plus_one){
_throw( Null_Exception);}* _temp195;}) ==( int)({ struct _tagged_string _temp196=
accept; char* _temp198= _temp196.curr + j; if( _temp198 < _temp196.base? 1:
_temp198 >= _temp196.last_plus_one){ _throw( Null_Exception);}* _temp198;})){
break;}} if(( unsigned int) j ==({ struct _tagged_string _temp199= accept;(
unsigned int)( _temp199.last_plus_one - _temp199.curr);})){ return i;}}} return
- 1;} struct Cyc_List_List* Cyc_String_explode( struct _tagged_string s){ struct
Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen( s) -( unsigned int)
1); for( 0; i >= 0; i --){ result=({ struct Cyc_List_List* _temp200=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp200->hd=( void*)(( int)({ struct
_tagged_string _temp201= s; char* _temp203= _temp201.curr + i; if( _temp203 <
_temp201.base? 1: _temp203 >= _temp201.last_plus_one){ _throw( Null_Exception);}*
_temp203;})); _temp200->tl= result; _temp200;});}} return result;} struct
_tagged_string Cyc_String_implode( struct Cyc_List_List* chars){ struct
_tagged_string s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
chars)); unsigned int i=( unsigned int) 0; while( chars != 0) {({ struct
_tagged_string _temp204= s; char* _temp206= _temp204.curr +( int)( i ++); if(
_temp206 < _temp204.base? 1: _temp206 >= _temp204.last_plus_one){ _throw(
Null_Exception);}* _temp206=( char)(( int) chars->hd);}); chars= chars->tl;}
return s;} int Cyc_String_to_int( struct _tagged_string s, int* offset){ int ans=
0; int sn= 1; int digit_seen= 0; int i= offset == 0? 0:* offset; while((
unsigned int) i <({ struct _tagged_string _temp207= s;( unsigned int)( _temp207.last_plus_one
- _temp207.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp208= s;
char* _temp210= _temp208.curr + i; if( _temp210 < _temp208.base? 1: _temp210 >=
_temp208.last_plus_one){ _throw( Null_Exception);}* _temp210;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp211= s;( unsigned int)(
_temp211.last_plus_one - _temp211.curr);})?( int)({ struct _tagged_string
_temp212= s; char* _temp214= _temp212.curr + i; if( _temp214 < _temp212.base? 1:
_temp214 >= _temp212.last_plus_one){ _throw( Null_Exception);}* _temp214;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp215= s;( unsigned int)( _temp215.last_plus_one - _temp215.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp216= s; char* _temp218= _temp216.curr + i; if(
_temp218 < _temp216.base? 1: _temp218 >= _temp216.last_plus_one){ _throw(
Null_Exception);}* _temp218;})): 0) { i ++;} while(( unsigned int) i <({ struct
_tagged_string _temp219= s;( unsigned int)( _temp219.last_plus_one - _temp219.curr);})?
Cyc_Character_isdigit(({ struct _tagged_string _temp220= s; char* _temp222=
_temp220.curr + i; if( _temp222 < _temp220.base? 1: _temp222 >= _temp220.last_plus_one){
_throw( Null_Exception);}* _temp222;})): 0) { digit_seen= 1; ans= ans * 10 +(
int)(( int)({ struct _tagged_string _temp223= s; char* _temp225= _temp223.curr +
i; if( _temp225 < _temp223.base? 1: _temp225 >= _temp223.last_plus_one){ _throw(
Null_Exception);}* _temp225;}) -( int)'0'); i ++;} ans= ans * sn; if( !
digit_seen){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp226=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp226=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp227=( char*)"String::to_int"; struct
_tagged_string _temp228; _temp228.curr= _temp227; _temp228.base= _temp227;
_temp228.last_plus_one= _temp227 + 15; _temp228;})};( struct _xenum_struct*)
_temp226;}));} if( offset != 0){* offset= i;} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:* offset; while(( unsigned int) i <({
struct _tagged_string _temp229= s;( unsigned int)( _temp229.last_plus_one -
_temp229.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp230= s;
char* _temp232= _temp230.curr + i; if( _temp232 < _temp230.base? 1: _temp232 >=
_temp230.last_plus_one){ _throw( Null_Exception);}* _temp232;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp233= s;( unsigned int)(
_temp233.last_plus_one - _temp233.curr);})?( int)({ struct _tagged_string
_temp234= s; char* _temp236= _temp234.curr + i; if( _temp236 < _temp234.base? 1:
_temp236 >= _temp234.last_plus_one){ _throw( Null_Exception);}* _temp236;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp237= s;( unsigned int)( _temp237.last_plus_one - _temp237.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp238= s; char* _temp240= _temp238.curr + i; if(
_temp240 < _temp238.base? 1: _temp240 >= _temp238.last_plus_one){ _throw(
Null_Exception);}* _temp240;})): 0) { digit_seen= 1; ans= ans *( double) 10.0 +(
double)(( int)({ struct _tagged_string _temp241= s; char* _temp243= _temp241.curr
+ i; if( _temp243 < _temp241.base? 1: _temp243 >= _temp241.last_plus_one){
_throw( Null_Exception);}* _temp243;}) -( int)'0'); i ++;} if(( unsigned int) i
<({ struct _tagged_string _temp244= s;( unsigned int)( _temp244.last_plus_one -
_temp244.curr);})?( int)({ struct _tagged_string _temp245= s; char* _temp247=
_temp245.curr + i; if( _temp247 < _temp245.base? 1: _temp247 >= _temp245.last_plus_one){
_throw( Null_Exception);}* _temp247;}) ==( int)'.': 0){ i ++;{ double divisor=(
double) 0.1; while(( unsigned int) i <({ struct _tagged_string _temp248= s;(
unsigned int)( _temp248.last_plus_one - _temp248.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp249= s; char* _temp251= _temp249.curr + i; if(
_temp251 < _temp249.base? 1: _temp251 >= _temp249.last_plus_one){ _throw(
Null_Exception);}* _temp251;})): 0) { digit_seen= 1; ans= ans +( double)(( int)({
struct _tagged_string _temp252= s; char* _temp254= _temp252.curr + i; if(
_temp254 < _temp252.base? 1: _temp254 >= _temp252.last_plus_one){ _throw(
Null_Exception);}* _temp254;}) -( int)'0') * divisor; divisor= divisor /( double)
10.0; i ++;}}} if( ! digit_seen){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp255=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp255=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp256=( char*)"String::to_double"; struct
_tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 18; _temp257;})};( struct _xenum_struct*)
_temp255;}));} if(( unsigned int) i <({ struct _tagged_string _temp258= s;(
unsigned int)( _temp258.last_plus_one - _temp258.curr);})?( int)({ struct
_tagged_string _temp259= s; char* _temp261= _temp259.curr + i; if( _temp261 <
_temp259.base? 1: _temp261 >= _temp259.last_plus_one){ _throw( Null_Exception);}*
_temp261;}) ==( int)'e'? 1:( int)({ struct _tagged_string _temp262= s; char*
_temp264= _temp262.curr + i; if( _temp264 < _temp262.base? 1: _temp264 >=
_temp262.last_plus_one){ _throw( Null_Exception);}* _temp264;}) ==( int)'E': 0){
i ++;{ int exponent= 0; int exp_sign= 1; digit_seen= 0; if(( unsigned int) i <({
struct _tagged_string _temp265= s;( unsigned int)( _temp265.last_plus_one -
_temp265.curr);})?( int)({ struct _tagged_string _temp266= s; char* _temp268=
_temp266.curr + i; if( _temp268 < _temp266.base? 1: _temp268 >= _temp266.last_plus_one){
_throw( Null_Exception);}* _temp268;}) ==( int)'-': 0){ i ++; exp_sign= - 1;}
else{ if(( unsigned int) i <({ struct _tagged_string _temp269= s;( unsigned int)(
_temp269.last_plus_one - _temp269.curr);})?( int)({ struct _tagged_string
_temp270= s; char* _temp272= _temp270.curr + i; if( _temp272 < _temp270.base? 1:
_temp272 >= _temp270.last_plus_one){ _throw( Null_Exception);}* _temp272;}) ==(
int)'+': 0){ i ++;}} while(( unsigned int) i <({ struct _tagged_string _temp273=
s;( unsigned int)( _temp273.last_plus_one - _temp273.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp274= s; char* _temp276= _temp274.curr + i; if(
_temp276 < _temp274.base? 1: _temp276 >= _temp274.last_plus_one){ _throw(
Null_Exception);}* _temp276;})): 0) { digit_seen= 1; exponent= exponent * 10 +(
int)(( int)({ struct _tagged_string _temp277= s; char* _temp279= _temp277.curr +
i; if( _temp279 < _temp277.base? 1: _temp279 >= _temp277.last_plus_one){ _throw(
Null_Exception);}* _temp279;}) -( int)'0'); i ++;} if( ! digit_seen){( void)
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp280=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp280=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp281=( char*)"String::to_double";
struct _tagged_string _temp282; _temp282.curr= _temp281; _temp282.base= _temp281;
_temp282.last_plus_one= _temp281 + 18; _temp282;})};( struct _xenum_struct*)
_temp280;}));} ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}}
if( offset != 0){* offset= i;} return ans *( double) sn;}