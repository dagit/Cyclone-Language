#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_Character_isdigit( char c); extern int Cyc_Character_isspace(
char c); extern char Cyc_Character_toupper( char c); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
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
void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp5=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp5=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1= error};( struct _xenum_struct*) _temp5;}));}{ int i;
for( i= ofs;( unsigned int) i <({ struct _tagged_string _temp6= s;( unsigned int)(
_temp6.last_plus_one - _temp6.curr);}); i ++){ if(( int)({ struct _tagged_string
_temp7= s; char* _temp9= _temp7.curr + i; if( _temp9 < _temp7.base? 1: _temp9 >=
_temp7.last_plus_one){ _throw( Null_Exception);}* _temp9;}) ==( int)'\000'){
break;}} return( unsigned int)( i - ofs);}} static int Cyc_String_case_cmp( char
c1, char c2){ return( int) c1 -( int) c2;} static int Cyc_String_nocase_cmp(
char c1, char c2){ return Cyc_String_case_cmp( Cyc_Character_toupper( c1), Cyc_Character_toupper(
c2));} static int Cyc_String_cmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2, int(*
f)( char, char)){ unsigned int min_length= len1 < len2? len1: len2; int i= - 1;
while(( unsigned int)( ++ i) < min_length) { int diff= f(({ struct
_tagged_string _temp10= s1; char* _temp12= _temp10.curr +( i + ofs1); if(
_temp12 < _temp10.base? 1: _temp12 >= _temp10.last_plus_one){ _throw(
Null_Exception);}* _temp12;}),({ struct _tagged_string _temp13= s2; char*
_temp15= _temp13.curr +( i + ofs2); if( _temp15 < _temp13.base? 1: _temp15 >=
_temp13.last_plus_one){ _throw( Null_Exception);}* _temp15;})); if( diff != 0){
return diff;}} return( int) len1 -( int) len2;} int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{
unsigned int len1= Cyc_String_int_strleno( s1, 0,( struct _tagged_string)({ char*
_temp16=( char*)"String::strcmp"; struct _tagged_string _temp17; _temp17.curr=
_temp16; _temp17.base= _temp16; _temp17.last_plus_one= _temp16 + 15; _temp17;}));
unsigned int len2= Cyc_String_int_strleno( s2, 0,( struct _tagged_string)({ char*
_temp18=( char*)"String::strcmp"; struct _tagged_string _temp19; _temp19.curr=
_temp18; _temp19.base= _temp18; _temp19.last_plus_one= _temp18 + 15; _temp19;}));
return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}} int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2){ return Cyc_String_strcmp(*
s1,* s2);} static int Cyc_String_ncmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2,
unsigned int n, int(* f)( char, char)){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int min_len= len1 > len2? len2: len1; unsigned int bound= min_len > n?
n: min_len;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){ int retc; if((
retc= f(({ struct _tagged_string _temp20= s1; char* _temp22= _temp20.curr +( i +
ofs1); if( _temp22 < _temp20.base? 1: _temp22 >= _temp20.last_plus_one){ _throw(
Null_Exception);}* _temp22;}),({ struct _tagged_string _temp23= s2; char*
_temp25= _temp23.curr +( i + ofs2); if( _temp25 < _temp23.base? 1: _temp25 >=
_temp23.last_plus_one){ _throw( Null_Exception);}* _temp25;}))) != 0){ return
retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp26=( char*)"String::strncmp"; struct
_tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one=
_temp26 + 16; _temp27;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,(
struct _tagged_string)({ char* _temp28=( char*)"String::strncmp"; struct
_tagged_string _temp29; _temp29.curr= _temp28; _temp29.base= _temp28; _temp29.last_plus_one=
_temp28 + 16; _temp29;})); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n,
Cyc_String_case_cmp);} int Cyc_String_strncasecmp( struct _tagged_string s1,
struct _tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,( struct _tagged_string)({ char* _temp30=( char*)"String::strncasecmp";
struct _tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30;
_temp31.last_plus_one= _temp30 + 20; _temp31;})); unsigned int len2= Cyc_String_int_strleno(
s2, 0,( struct _tagged_string)({ char* _temp32=( char*)"String::strncasecmp";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 20; _temp33;})); return Cyc_String_ncmp( s1, 0,
len1, s2, 0, len2, n, Cyc_String_nocase_cmp);} int Cyc_String_zstrcmp( struct
_tagged_string a, struct _tagged_string b){ if( a.curr == b.curr){ return 0;}{
unsigned int as=({ struct _tagged_string _temp34= a;( unsigned int)( _temp34.last_plus_one
- _temp34.curr);}); unsigned int bs=({ struct _tagged_string _temp35= b;(
unsigned int)( _temp35.last_plus_one - _temp35.curr);}); unsigned int min_length=
as < bs? as: bs; int i= - 1; while(( unsigned int)( ++ i) < min_length) { int
diff=( int)({ struct _tagged_string _temp36= a; char* _temp38= _temp36.curr + i;
if( _temp38 < _temp36.base? 1: _temp38 >= _temp36.last_plus_one){ _throw(
Null_Exception);}* _temp38;}) -( int)({ struct _tagged_string _temp39= b; char*
_temp41= _temp39.curr + i; if( _temp41 < _temp39.base? 1: _temp41 >= _temp39.last_plus_one){
_throw( Null_Exception);}* _temp41;}); if( diff != 0){ return diff;}} return(
int) as -( int) bs;}} int Cyc_String_zstrncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ if( n <=( unsigned int) 0){ return 0;}{
unsigned int s1size=({ struct _tagged_string _temp42= s1;( unsigned int)(
_temp42.last_plus_one - _temp42.curr);}); unsigned int s2size=({ struct
_tagged_string _temp43= s2;( unsigned int)( _temp43.last_plus_one - _temp43.curr);});
unsigned int min_size= s1size > s2size? s2size: s1size; unsigned int bound=
min_size > n? n: min_size;{ int i= 0; for( 0;( unsigned int) i < bound; i ++){
if(( int)({ struct _tagged_string _temp44= s1; char* _temp46= _temp44.curr + i;
if( _temp46 < _temp44.base? 1: _temp46 >= _temp44.last_plus_one){ _throw(
Null_Exception);}* _temp46;}) <( int)({ struct _tagged_string _temp47= s2; char*
_temp49= _temp47.curr + i; if( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one){
_throw( Null_Exception);}* _temp49;})){ return - 1;} else{ if(( int)({ struct
_tagged_string _temp50= s2; char* _temp52= _temp50.curr + i; if( _temp52 <
_temp50.base? 1: _temp52 >= _temp50.last_plus_one){ _throw( Null_Exception);}*
_temp52;}) <( int)({ struct _tagged_string _temp53= s1; char* _temp55= _temp53.curr
+ i; if( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one){ _throw(
Null_Exception);}* _temp55;})){ return 1;}}}} if( min_size <= bound){ return 0;}
if( s1size < s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp(
struct _tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(*
a,* b);} static struct _tagged_string Cyc_String_int_strcato( struct
_tagged_string dest, struct _tagged_string src, int srcOfs, struct
_tagged_string error){ int i; unsigned int dsize; unsigned int slen;
unsigned int dlen; dsize=({ struct _tagged_string _temp56= dest;( unsigned int)(
_temp56.last_plus_one - _temp56.curr);}); dlen= Cyc_String_strlen( dest); slen=
Cyc_String_int_strleno( src, srcOfs, error); if( slen + dlen <= dsize){ for( i=
0;( unsigned int) i < slen; i ++){({ struct _tagged_string _temp57= dest; char*
_temp59= _temp57.curr +( int)(( unsigned int) i + dlen); if( _temp59 < _temp57.base?
1: _temp59 >= _temp57.last_plus_one){ _throw( Null_Exception);}* _temp59=({
struct _tagged_string _temp60= src; char* _temp62= _temp60.curr +( i + srcOfs);
if( _temp62 < _temp60.base? 1: _temp62 >= _temp60.last_plus_one){ _throw(
Null_Exception);}* _temp62;});});} if(( unsigned int) i != dsize){({ struct
_tagged_string _temp63= dest; char* _temp65= _temp63.curr +( int)(( unsigned int)
i + dlen); if( _temp65 < _temp63.base? 1: _temp65 >= _temp63.last_plus_one){
_throw( Null_Exception);}* _temp65='\000';});}} else{( void) _throw(({ struct
Cyc_Core_InvalidArg_struct* _temp66=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp66=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1= error};( struct _xenum_struct*) _temp66;}));}
return dest;} struct _tagged_string Cyc_String_strcat( struct _tagged_string
dest, struct _tagged_string src){ return Cyc_String_int_strcato( dest, src, 0,(
struct _tagged_string)({ char* _temp67=( char*)"String::strcat"; struct
_tagged_string _temp68; _temp68.curr= _temp67; _temp68.base= _temp67; _temp68.last_plus_one=
_temp67 + 15; _temp68;}));} struct _tagged_string Cyc_String_strconcat( struct
_tagged_string a, struct _tagged_string b){ unsigned int alen= Cyc_String_strlen(
a); unsigned int blen= Cyc_String_strlen( b); struct _tagged_string ans= Cyc_Core_new_string((
int)( alen + blen)); int i; int j; for( i= 0;( unsigned int) i < alen; ++ i){({
struct _tagged_string _temp69= ans; char* _temp71= _temp69.curr + i; if( _temp71
< _temp69.base? 1: _temp71 >= _temp69.last_plus_one){ _throw( Null_Exception);}*
_temp71=({ struct _tagged_string _temp72= a; char* _temp74= _temp72.curr + i;
if( _temp74 < _temp72.base? 1: _temp74 >= _temp72.last_plus_one){ _throw(
Null_Exception);}* _temp74;});});} for( j= 0;( unsigned int) j < blen; ++ j){({
struct _tagged_string _temp75= ans; char* _temp77= _temp75.curr +( i + j); if(
_temp77 < _temp75.base? 1: _temp77 >= _temp75.last_plus_one){ _throw(
Null_Exception);}* _temp77=({ struct _tagged_string _temp78= b; char* _temp80=
_temp78.curr + j; if( _temp80 < _temp78.base? 1: _temp80 >= _temp78.last_plus_one){
_throw( Null_Exception);}* _temp80;});});} return ans;} struct _tagged_string
Cyc_String_strconcat_l( struct Cyc_List_List* strs){ unsigned int len;
unsigned int total_len=( unsigned int) 0; struct Cyc_List_List* lens=({ struct
Cyc_List_List* _temp81=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp81->hd=( void*)(( unsigned int) 0); _temp81->tl= 0; _temp81;}); struct Cyc_List_List*
end= lens;{ struct Cyc_List_List* p= strs; for( 0; p != 0; p= p->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*) p->hd)); total_len += len; end->tl=({ struct Cyc_List_List*
_temp82=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp82->hd=( void*) len; _temp82->tl= 0; _temp82;}); end= end->tl;}} lens= lens->tl;{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=(
unsigned int) 0; while( strs != 0) { struct _tagged_string next=*(( struct
_tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; strs= strs->tl; lens= lens->tl;} return
ans;}} struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List* strs,
struct _tagged_string separator){ if( strs == 0){ return( struct _tagged_string)({
char* _temp83=( char*)""; struct _tagged_string _temp84; _temp84.curr= _temp83;
_temp84.base= _temp83; _temp84.last_plus_one= _temp83 + 1; _temp84;});} if( strs->tl
== 0){ return Cyc_String_strdup(*(( struct _tagged_string*) strs->hd));}{ struct
Cyc_List_List* p= strs; struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp85=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp85->hd=( void*)(( unsigned int) 0); _temp85->tl= 0; _temp85;}); struct Cyc_List_List*
end= lens; unsigned int len=( unsigned int) 0; unsigned int total_len=(
unsigned int) 0; unsigned int list_len=( unsigned int) 0; for( 0; p != 0; p= p->tl){
len= Cyc_String_strlen(*(( struct _tagged_string*) p->hd)); total_len += len;
end->tl=({ struct Cyc_List_List* _temp86=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp86->hd=( void*) len; _temp86->tl= 0;
_temp86;}); end= end->tl; ++ list_len;} lens= lens->tl;{ unsigned int seplen=
Cyc_String_strlen( separator); total_len +=( list_len -( unsigned int) 1) *
seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int) total_len);
unsigned int i=( unsigned int) 0; while( strs->tl != 0) { struct _tagged_string
next=*(( struct _tagged_string*) strs->hd); len=( unsigned int) lens->hd; Cyc_String_strncpy(
ans,( int) i, next, 0, len); i += len; Cyc_String_strncpy( ans,( int) i,
separator, 0, seplen); i += seplen; strs= strs->tl; lens= lens->tl;} Cyc_String_strncpy(
ans,( int) i,*(( struct _tagged_string*) strs->hd), 0,( unsigned int) lens->hd);
return ans;}}}} struct _tagged_string Cyc_String_strncpy( struct _tagged_string
dest, int destOfs, struct _tagged_string src, int srcOfs, unsigned int n){
unsigned int destSz=({ struct _tagged_string _temp87= dest;( unsigned int)(
_temp87.last_plus_one - _temp87.curr);}); unsigned int srcSz=({ struct
_tagged_string _temp88= src;( unsigned int)( _temp88.last_plus_one - _temp88.curr);});
if( destOfs < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp89=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp89=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp90=( char*)"String::strncpy"; struct
_tagged_string _temp91; _temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one=
_temp90 + 16; _temp91;})};( struct _xenum_struct*) _temp89;}));} if( srcOfs < 0?
1:( n >( unsigned int) 0?( unsigned int) srcOfs >= srcSz: 0)){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp92=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp92=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp93=( char*)"String::strncpy";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 16; _temp94;})};( struct _xenum_struct*)
_temp92;}));} if( n <=( unsigned int) 0){ return dest;} if(( unsigned int)
destOfs + n > destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp95=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp95=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp96=( char*)"String::strncpy";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 16; _temp97;})};( struct _xenum_struct*)
_temp95;}));}{ int i; for( i= 0;( unsigned int) i < n; i ++){ char srcChar=({
struct _tagged_string _temp98= src; char* _temp100= _temp98.curr +( srcOfs + i);
if( _temp100 < _temp98.base? 1: _temp100 >= _temp98.last_plus_one){ _throw(
Null_Exception);}* _temp100;}); if(( int) srcChar ==( int)'\000'){ break;}({
struct _tagged_string _temp101= dest; char* _temp103= _temp101.curr +( destOfs +
i); if( _temp103 < _temp101.base? 1: _temp103 >= _temp101.last_plus_one){ _throw(
Null_Exception);}* _temp103= srcChar;});} for( 0;( unsigned int) i < n; i ++){({
struct _tagged_string _temp104= dest; char* _temp106= _temp104.curr +( destOfs +
i); if( _temp106 < _temp104.base? 1: _temp106 >= _temp104.last_plus_one){ _throw(
Null_Exception);}* _temp106='\000';});} return dest;}} struct _tagged_string Cyc_String_zstrncpy(
struct _tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int destSz=({ struct _tagged_string _temp107= dest;(
unsigned int)( _temp107.last_plus_one - _temp107.curr);}); unsigned int srcSz=({
struct _tagged_string _temp108= src;( unsigned int)( _temp108.last_plus_one -
_temp108.curr);}); if( destOfs < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp109=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp109=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp110=( char*)"String::zstrncpy"; struct
_tagged_string _temp111; _temp111.curr= _temp110; _temp111.base= _temp110;
_temp111.last_plus_one= _temp110 + 17; _temp111;})};( struct _xenum_struct*)
_temp109;}));} if( srcOfs < 0? 1:( n >( unsigned int) 0?( unsigned int) srcOfs
>= srcSz: 0)){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp112=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp112=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp113=( char*)"String::zstrncpy"; struct
_tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 17; _temp114;})};( struct _xenum_struct*)
_temp112;}));} if( n <=( unsigned int) 0){ return dest;} if(( unsigned int)
destOfs + n > destSz? 1:( unsigned int) srcOfs + n > srcSz){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp115=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp115=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp116=( char*)"String::zstrncpy";
struct _tagged_string _temp117; _temp117.curr= _temp116; _temp117.base= _temp116;
_temp117.last_plus_one= _temp116 + 17; _temp117;})};( struct _xenum_struct*)
_temp115;}));}{ int i; for( i= 0;( unsigned int) i < n; i ++){({ struct
_tagged_string _temp118= dest; char* _temp120= _temp118.curr +( destOfs + i);
if( _temp120 < _temp118.base? 1: _temp120 >= _temp118.last_plus_one){ _throw(
Null_Exception);}* _temp120=({ struct _tagged_string _temp121= src; char*
_temp123= _temp121.curr +( srcOfs + i); if( _temp123 < _temp121.base? 1:
_temp123 >= _temp121.last_plus_one){ _throw( Null_Exception);}* _temp123;});});}
return dest;}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string
dest, struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp124= dest;(
unsigned int)( _temp124.last_plus_one - _temp124.curr);})){({ struct
_tagged_string _temp125= dest; char* _temp127= _temp125.curr +( int) len; if(
_temp127 < _temp125.base? 1: _temp127 >= _temp125.last_plus_one){ _throw(
Null_Exception);}* _temp127='\000';});} return dest;} struct _tagged_string Cyc_String_strdup(
struct _tagged_string src){ unsigned int len; struct _tagged_string temp; len=
Cyc_String_strlen( src); temp= Cyc_Core_new_string(( int) len); Cyc_String_strncpy(
temp, 0, src, 0, len); return temp;} struct _tagged_string Cyc_String_expand(
struct _tagged_string s, unsigned int sz){ struct _tagged_string temp;
unsigned int slen; slen= Cyc_String_strlen( s); sz= sz > slen? sz: slen; temp=
Cyc_Core_new_string(( int) sz); Cyc_String_strncpy( temp, 0, s, 0, slen); if(
slen !=({ struct _tagged_string _temp128= s;( unsigned int)( _temp128.last_plus_one
- _temp128.curr);})){({ struct _tagged_string _temp129= temp; char* _temp131=
_temp129.curr +( int) slen; if( _temp131 < _temp129.base? 1: _temp131 >=
_temp129.last_plus_one){ _throw( Null_Exception);}* _temp131='\000';});} return
temp;} struct _tagged_string Cyc_String_realloc_str( struct _tagged_string str,
unsigned int sz){ unsigned int maxsizeP=({ struct _tagged_string _temp132= str;(
unsigned int)( _temp132.last_plus_one - _temp132.curr);}); if( maxsizeP ==(
unsigned int) 0){ maxsizeP=( unsigned int) 30 > sz? 30: sz; str= Cyc_Core_new_string((
int) maxsizeP);({ struct _tagged_string _temp133= str; char* _temp135= _temp133.curr
+ 0; if( _temp135 < _temp133.base? 1: _temp135 >= _temp133.last_plus_one){
_throw( Null_Exception);}* _temp135='\000';});} else{ if( sz > maxsizeP){
maxsizeP= maxsizeP *( unsigned int) 2 >( sz *( unsigned int) 5) /( unsigned int)
4? maxsizeP *( unsigned int) 2:( sz *( unsigned int) 5) /( unsigned int) 4; str=
Cyc_String_expand( str, maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring(
struct _tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt <(
unsigned int) 0)? 1:( start +( int) amt) - 1 >( int)({ struct _tagged_string
_temp136= s;( unsigned int)( _temp136.last_plus_one - _temp136.curr);})){( void)
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp137=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp137=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp138=( char*)"String::substring";
struct _tagged_string _temp139; _temp139.curr= _temp138; _temp139.base= _temp138;
_temp139.last_plus_one= _temp138 + 18; _temp139;})};( struct _xenum_struct*)
_temp137;}));}{ struct _tagged_string ans= Cyc_Core_new_string(( int)( amt +(
unsigned int) 1));{ unsigned int i=( unsigned int) 0; for( 0; i < amt; ++ i){({
struct _tagged_string _temp140= ans; char* _temp142= _temp140.curr +( int) i;
if( _temp142 < _temp140.base? 1: _temp142 >= _temp140.last_plus_one){ _throw(
Null_Exception);}* _temp142=({ struct _tagged_string _temp143= s; char* _temp145=
_temp143.curr +( int)(( unsigned int) start + i); if( _temp145 < _temp143.base?
1: _temp145 >= _temp143.last_plus_one){ _throw( Null_Exception);}* _temp145;});});}}({
struct _tagged_string _temp146= ans; char* _temp148= _temp146.curr +( int) amt;
if( _temp148 < _temp146.base? 1: _temp148 >= _temp146.last_plus_one){ _throw(
Null_Exception);}* _temp148='\000';}); return ans;}} struct _tagged_string Cyc_String_replace_suffix(
struct _tagged_string src, struct _tagged_string curr_suffix, struct
_tagged_string new_suffix){ unsigned int m=({ struct _tagged_string _temp149=
src;( unsigned int)( _temp149.last_plus_one - _temp149.curr);}); unsigned int n=({
struct _tagged_string _temp150= curr_suffix;( unsigned int)( _temp150.last_plus_one
- _temp150.curr);}); struct _tagged_string err=( struct _tagged_string)({ char*
_temp151=( char*)"String::replace_suffix"; struct _tagged_string _temp152;
_temp152.curr= _temp151; _temp152.base= _temp151; _temp152.last_plus_one=
_temp151 + 23; _temp152;}); if( m < n){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp153=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp153=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=
err};( struct _xenum_struct*) _temp153;}));}{ unsigned int i=( unsigned int) 1;
for( 0; i <= n; ++ i){ if(( int)({ struct _tagged_string _temp154= src; char*
_temp156= _temp154.curr +( int)( m - i); if( _temp156 < _temp154.base? 1:
_temp156 >= _temp154.last_plus_one){ _throw( Null_Exception);}* _temp156;}) !=(
int)({ struct _tagged_string _temp157= curr_suffix; char* _temp159= _temp157.curr
+( int)( n - i); if( _temp159 < _temp157.base? 1: _temp159 >= _temp157.last_plus_one){
_throw( Null_Exception);}* _temp159;})){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp160=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp160=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=
err};( struct _xenum_struct*) _temp160;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp161= new_suffix;( unsigned int)(
_temp161.last_plus_one - _temp161.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp162= new_suffix;( unsigned int)( _temp162.last_plus_one -
_temp162.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp163=( char*)"String::strpbrk"; struct
_tagged_string _temp164; _temp164.curr= _temp163; _temp164.base= _temp163;
_temp164.last_plus_one= _temp163 + 16; _temp164;})); char c='\000'; for( 0; len
> 0?( int)( c=({ struct _tagged_string _temp165= s; char* _temp166= _temp165.curr;
if( _temp166 < _temp165.base? 1: _temp166 >= _temp165.last_plus_one){ _throw(
Null_Exception);}* _temp166;})) != 0: 0; len --,({ struct _tagged_string*
_temp167=& s; struct _tagged_string _temp168=* _temp167; _temp167->curr ++;
_temp168;})){ int j= 0; for( 0;( unsigned int) j <({ struct _tagged_string
_temp169= accept;( unsigned int)( _temp169.last_plus_one - _temp169.curr);}); j
++){ if(( int) c ==( int)({ struct _tagged_string _temp170= accept; char*
_temp172= _temp170.curr + j; if( _temp172 < _temp170.base? 1: _temp172 >=
_temp170.last_plus_one){ _throw( Null_Exception);}* _temp172;})){ return s;}}}
return({ struct _tagged_string _temp173={ 0, 0, 0}; _temp173;});} struct
_tagged_string Cyc_String_strchr( struct _tagged_string s, char c){ int len=(
int) Cyc_String_int_strleno( s, 0,( struct _tagged_string)({ char* _temp174=(
char*)"String::strchr"; struct _tagged_string _temp175; _temp175.curr= _temp174;
_temp175.base= _temp174; _temp175.last_plus_one= _temp174 + 15; _temp175;}));
char c2='\000'; for( 0; len > 0?( int)( c2=({ struct _tagged_string _temp176= s;
char* _temp177= _temp176.curr; if( _temp177 < _temp176.base? 1: _temp177 >=
_temp176.last_plus_one){ _throw( Null_Exception);}* _temp177;})) != 0: 0; len --,({
struct _tagged_string* _temp178=& s; struct _tagged_string _temp179=* _temp178;
_temp178->curr ++; _temp179;})){ if(( int) c2 ==( int) c){ return s;}} return({
struct _tagged_string _temp180={ 0, 0, 0}; _temp180;});} struct _tagged_string
Cyc_String_strrchr( struct _tagged_string s, char c){ int len=( int) Cyc_String_int_strleno(
s, 0,( struct _tagged_string)({ char* _temp181=( char*)"String::strrchr"; struct
_tagged_string _temp182; _temp182.curr= _temp181; _temp182.base= _temp181;
_temp182.last_plus_one= _temp181 + 16; _temp182;})); int i= len - 1; s.curr += i;
for( 0; i >= 0; i --,({ struct _tagged_string* _temp183=& s; struct
_tagged_string _temp184=* _temp183; _temp183->curr --; _temp184;})){ if(( int)({
struct _tagged_string _temp185= s; char* _temp186= _temp185.curr; if( _temp186 <
_temp185.base? 1: _temp186 >= _temp185.last_plus_one){ _throw( Null_Exception);}*
_temp186;}) ==( int) c){ return s;}} return({ struct _tagged_string _temp187={ 0,
0, 0}; _temp187;});} unsigned int Cyc_String_strspn( struct _tagged_string s,
struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno( s, 0,(
struct _tagged_string)({ char* _temp188=( char*)"String::strspn"; struct
_tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 15; _temp189;}));{ unsigned int i=(
unsigned int) 0; for( 0; i < len; i ++){ int j; for( j= 0;( unsigned int) j <({
struct _tagged_string _temp190= accept;( unsigned int)( _temp190.last_plus_one -
_temp190.curr);}); j ++){ if(( int)({ struct _tagged_string _temp191= s; char*
_temp193= _temp191.curr +( int) i; if( _temp193 < _temp191.base? 1: _temp193 >=
_temp191.last_plus_one){ _throw( Null_Exception);}* _temp193;}) ==( int)({
struct _tagged_string _temp194= accept; char* _temp196= _temp194.curr + j; if(
_temp196 < _temp194.base? 1: _temp196 >= _temp194.last_plus_one){ _throw(
Null_Exception);}* _temp196;})){ break;}} if(( unsigned int) j ==({ struct
_tagged_string _temp197= accept;( unsigned int)( _temp197.last_plus_one -
_temp197.curr);})){ return i;}}} return len;} struct Cyc_List_List* Cyc_String_explode(
struct _tagged_string s){ struct Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen(
s) -( unsigned int) 1); for( 0; i >= 0; i --){ result=({ struct Cyc_List_List*
_temp198=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp198->hd=( void*)(( int)({ struct _tagged_string _temp199= s; char* _temp201=
_temp199.curr + i; if( _temp201 < _temp199.base? 1: _temp201 >= _temp199.last_plus_one){
_throw( Null_Exception);}* _temp201;})); _temp198->tl= result; _temp198;});}}
return result;} struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
chars){ struct _tagged_string s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); unsigned int i=( unsigned int) 0; while( chars !=
0) {({ struct _tagged_string _temp202= s; char* _temp204= _temp202.curr +( int)(
i ++); if( _temp204 < _temp202.base? 1: _temp204 >= _temp202.last_plus_one){
_throw( Null_Exception);}* _temp204=( char)(( int) chars->hd);}); chars= chars->tl;}
return s;} int Cyc_String_to_int( struct _tagged_string s, int* offset){ int ans=
0; int sn= 1; int digit_seen= 0; int i= offset == 0? 0:* offset; while((
unsigned int) i <({ struct _tagged_string _temp205= s;( unsigned int)( _temp205.last_plus_one
- _temp205.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp206= s;
char* _temp208= _temp206.curr + i; if( _temp208 < _temp206.base? 1: _temp208 >=
_temp206.last_plus_one){ _throw( Null_Exception);}* _temp208;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp209= s;( unsigned int)(
_temp209.last_plus_one - _temp209.curr);})?( int)({ struct _tagged_string
_temp210= s; char* _temp212= _temp210.curr + i; if( _temp212 < _temp210.base? 1:
_temp212 >= _temp210.last_plus_one){ _throw( Null_Exception);}* _temp212;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp213= s;( unsigned int)( _temp213.last_plus_one - _temp213.curr);})? Cyc_Character_isspace(({
struct _tagged_string _temp214= s; char* _temp216= _temp214.curr + i; if(
_temp216 < _temp214.base? 1: _temp216 >= _temp214.last_plus_one){ _throw(
Null_Exception);}* _temp216;})): 0) { i ++;} while(( unsigned int) i <({ struct
_tagged_string _temp217= s;( unsigned int)( _temp217.last_plus_one - _temp217.curr);})?
Cyc_Character_isdigit(({ struct _tagged_string _temp218= s; char* _temp220=
_temp218.curr + i; if( _temp220 < _temp218.base? 1: _temp220 >= _temp218.last_plus_one){
_throw( Null_Exception);}* _temp220;})): 0) { digit_seen= 1; ans= ans * 10 +(
int)(( int)({ struct _tagged_string _temp221= s; char* _temp223= _temp221.curr +
i; if( _temp223 < _temp221.base? 1: _temp223 >= _temp221.last_plus_one){ _throw(
Null_Exception);}* _temp223;}) -( int)'0'); i ++;} ans= ans * sn; if( !
digit_seen){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp224=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp224=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp225=( char*)"String::to_int"; struct
_tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 15; _temp226;})};( struct _xenum_struct*)
_temp224;}));} if( offset != 0){* offset= i;} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:* offset; while(( unsigned int) i <({
struct _tagged_string _temp227= s;( unsigned int)( _temp227.last_plus_one -
_temp227.curr);})? Cyc_Character_isspace(({ struct _tagged_string _temp228= s;
char* _temp230= _temp228.curr + i; if( _temp230 < _temp228.base? 1: _temp230 >=
_temp228.last_plus_one){ _throw( Null_Exception);}* _temp230;})): 0) { i ++;}
if(( unsigned int) i <({ struct _tagged_string _temp231= s;( unsigned int)(
_temp231.last_plus_one - _temp231.curr);})?( int)({ struct _tagged_string
_temp232= s; char* _temp234= _temp232.curr + i; if( _temp234 < _temp232.base? 1:
_temp234 >= _temp232.last_plus_one){ _throw( Null_Exception);}* _temp234;}) ==(
int)'-': 0){ i ++; sn= - 1;} while(( unsigned int) i <({ struct _tagged_string
_temp235= s;( unsigned int)( _temp235.last_plus_one - _temp235.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp236= s; char* _temp238= _temp236.curr + i; if(
_temp238 < _temp236.base? 1: _temp238 >= _temp236.last_plus_one){ _throw(
Null_Exception);}* _temp238;})): 0) { digit_seen= 1; ans= ans *( double) 10.0 +(
double)(( int)({ struct _tagged_string _temp239= s; char* _temp241= _temp239.curr
+ i; if( _temp241 < _temp239.base? 1: _temp241 >= _temp239.last_plus_one){
_throw( Null_Exception);}* _temp241;}) -( int)'0'); i ++;} if(( unsigned int) i
<({ struct _tagged_string _temp242= s;( unsigned int)( _temp242.last_plus_one -
_temp242.curr);})?( int)({ struct _tagged_string _temp243= s; char* _temp245=
_temp243.curr + i; if( _temp245 < _temp243.base? 1: _temp245 >= _temp243.last_plus_one){
_throw( Null_Exception);}* _temp245;}) ==( int)'.': 0){ i ++;{ double divisor=(
double) 0.1; while(( unsigned int) i <({ struct _tagged_string _temp246= s;(
unsigned int)( _temp246.last_plus_one - _temp246.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp247= s; char* _temp249= _temp247.curr + i; if(
_temp249 < _temp247.base? 1: _temp249 >= _temp247.last_plus_one){ _throw(
Null_Exception);}* _temp249;})): 0) { digit_seen= 1; ans= ans +( double)(( int)({
struct _tagged_string _temp250= s; char* _temp252= _temp250.curr + i; if(
_temp252 < _temp250.base? 1: _temp252 >= _temp250.last_plus_one){ _throw(
Null_Exception);}* _temp252;}) -( int)'0') * divisor; divisor= divisor /( double)
10.0; i ++;}}} if( ! digit_seen){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp253=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp253=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp254=( char*)"String::to_double"; struct
_tagged_string _temp255; _temp255.curr= _temp254; _temp255.base= _temp254;
_temp255.last_plus_one= _temp254 + 18; _temp255;})};( struct _xenum_struct*)
_temp253;}));} if(( unsigned int) i <({ struct _tagged_string _temp256= s;(
unsigned int)( _temp256.last_plus_one - _temp256.curr);})?( int)({ struct
_tagged_string _temp257= s; char* _temp259= _temp257.curr + i; if( _temp259 <
_temp257.base? 1: _temp259 >= _temp257.last_plus_one){ _throw( Null_Exception);}*
_temp259;}) ==( int)'e'? 1:( int)({ struct _tagged_string _temp260= s; char*
_temp262= _temp260.curr + i; if( _temp262 < _temp260.base? 1: _temp262 >=
_temp260.last_plus_one){ _throw( Null_Exception);}* _temp262;}) ==( int)'E': 0){
i ++;{ int exponent= 0; int exp_sign= 1; digit_seen= 0; if(( unsigned int) i <({
struct _tagged_string _temp263= s;( unsigned int)( _temp263.last_plus_one -
_temp263.curr);})?( int)({ struct _tagged_string _temp264= s; char* _temp266=
_temp264.curr + i; if( _temp266 < _temp264.base? 1: _temp266 >= _temp264.last_plus_one){
_throw( Null_Exception);}* _temp266;}) ==( int)'-': 0){ i ++; exp_sign= - 1;}
else{ if(( unsigned int) i <({ struct _tagged_string _temp267= s;( unsigned int)(
_temp267.last_plus_one - _temp267.curr);})?( int)({ struct _tagged_string
_temp268= s; char* _temp270= _temp268.curr + i; if( _temp270 < _temp268.base? 1:
_temp270 >= _temp268.last_plus_one){ _throw( Null_Exception);}* _temp270;}) ==(
int)'+': 0){ i ++;}} while(( unsigned int) i <({ struct _tagged_string _temp271=
s;( unsigned int)( _temp271.last_plus_one - _temp271.curr);})? Cyc_Character_isdigit(({
struct _tagged_string _temp272= s; char* _temp274= _temp272.curr + i; if(
_temp274 < _temp272.base? 1: _temp274 >= _temp272.last_plus_one){ _throw(
Null_Exception);}* _temp274;})): 0) { digit_seen= 1; exponent= exponent * 10 +(
int)(( int)({ struct _tagged_string _temp275= s; char* _temp277= _temp275.curr +
i; if( _temp277 < _temp275.base? 1: _temp277 >= _temp275.last_plus_one){ _throw(
Null_Exception);}* _temp277;}) -( int)'0'); i ++;} if( ! digit_seen){( void)
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp278=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp278=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp279=( char*)"String::to_double";
struct _tagged_string _temp280; _temp280.curr= _temp279; _temp280.base= _temp279;
_temp280.last_plus_one= _temp279 + 18; _temp280;})};( struct _xenum_struct*)
_temp278;}));} ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}}
if( offset != 0){* offset= i;} return ans *( double) sn;}