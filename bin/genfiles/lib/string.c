 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_string Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern int isalnum( int __c)  __attribute__(( cdecl )) ; extern int isalpha( int
__c)  __attribute__(( cdecl )) ; extern int iscntrl( int __c)  __attribute__((
cdecl )) ; extern int isdigit( int __c)  __attribute__(( cdecl )) ; extern int
isgraph( int __c)  __attribute__(( cdecl )) ; extern int islower( int __c)
 __attribute__(( cdecl )) ; extern int isprint( int __c)  __attribute__(( cdecl
)) ; extern int ispunct( int __c)  __attribute__(( cdecl )) ; extern int isspace(
int __c)  __attribute__(( cdecl )) ; extern int isupper( int __c)
 __attribute__(( cdecl )) ; extern int isxdigit( int __c)  __attribute__(( cdecl
)) ; extern int tolower( int __c)  __attribute__(( cdecl )) ; extern int toupper(
int __c)  __attribute__(( cdecl )) ; extern int isascii( int __c)
 __attribute__(( cdecl )) ; extern int toascii( int __c)  __attribute__(( cdecl
)) ; extern int _tolower( int __c)  __attribute__(( cdecl )) ; extern int
_toupper( int __c)  __attribute__(( cdecl )) ; struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned int Cyc_String_strlen( struct
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
struct _tagged_string Cyc_String_strchr( struct _tagged_string s, unsigned char
c); extern struct _tagged_string Cyc_String_strrchr( struct _tagged_string s,
unsigned char c); extern struct _tagged_string Cyc_String_strpbrk( struct
_tagged_string s, struct _tagged_string accept); extern unsigned int Cyc_String_strspn(
struct _tagged_string s, struct _tagged_string accept); extern struct Cyc_List_List*
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
int signgam; struct Cyc_Math___exception{ int type; unsigned char* name; double
arg1; double arg2; double retval; int err; } ; extern int matherr( struct Cyc_Math___exception*
e); unsigned int Cyc_String_strlen( struct _tagged_string s){ unsigned int i;
for( i= 0; i <({ struct _tagged_string _temp0= s;( unsigned int)( _temp0.last_plus_one
- _temp0.curr);}); i ++){ if(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),(
int) i) =='\000'){ return i;}} return i;} static unsigned int Cyc_String_int_strleno(
struct _tagged_string s, int ofs, struct _tagged_string error){ if( ofs >({
struct _tagged_string _temp1= s;( unsigned int)( _temp1.last_plus_one - _temp1.curr);})?
1: ofs < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp2=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp3; _temp3.tag= Cyc_Core_InvalidArg;
_temp3.f1= error; _temp3;}); _temp2;}));}{ int i; for( i= ofs; i <({ struct
_tagged_string _temp4= s;( unsigned int)( _temp4.last_plus_one - _temp4.curr);});
i ++){ if(*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) =='\000'){
break;}} return( unsigned int)( i - ofs);}} static int Cyc_String_case_cmp(
unsigned char c1, unsigned char c2){ return c1 - c2;} static int Cyc_String_nocase_cmp(
unsigned char c1, unsigned char c2){ return Cyc_String_case_cmp(( unsigned char)
toupper(( int) c1),( unsigned char) toupper(( int) c2));} static int Cyc_String_cmp(
struct _tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2,
int ofs2, unsigned int len2, int(* f)( unsigned char, unsigned char)){
unsigned int min_length= len1 < len2? len1: len2; int i= - 1; while( ++ i <
min_length) { int diff= f(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s1, sizeof(
unsigned char), i + ofs1),*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s2, sizeof(
unsigned char), i + ofs2)); if( diff != 0){ return diff;}} return( int) len1 -(
int) len2;} int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{ unsigned int len1= Cyc_String_int_strleno(
s1, 0,({ unsigned char* _temp7="String::strcmp"; struct _tagged_string _temp8;
_temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 + 15;
_temp8;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,({ unsigned char*
_temp5="String::strcmp"; struct _tagged_string _temp6; _temp6.curr= _temp5;
_temp6.base= _temp5; _temp6.last_plus_one= _temp5 + 15; _temp6;})); return Cyc_String_cmp(
s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}} int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2){ return Cyc_String_strcmp(*
s1,* s2);} static int Cyc_String_ncmp( struct _tagged_string s1, int ofs1,
unsigned int len1, struct _tagged_string s2, int ofs2, unsigned int len2,
unsigned int n, int(* f)( unsigned char, unsigned char)){ if( n <= 0){ return 0;}{
unsigned int min_len= len1 > len2? len2: len1; unsigned int bound= min_len > n?
n: min_len;{ int i= 0; for( 0; i < bound; i ++){ int retc; if(( retc= f(*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s1, sizeof( unsigned char), i + ofs1),*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s2, sizeof( unsigned char), i + ofs2))) != 0){ return
retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ unsigned int len1= Cyc_String_int_strleno(
s1, 0,({ unsigned char* _temp11="String::strncmp"; struct _tagged_string _temp12;
_temp12.curr= _temp11; _temp12.base= _temp11; _temp12.last_plus_one= _temp11 +
16; _temp12;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,({
unsigned char* _temp9="String::strncmp"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 16; _temp10;}));
return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n, Cyc_String_case_cmp);} int
Cyc_String_strncasecmp( struct _tagged_string s1, struct _tagged_string s2,
unsigned int n){ unsigned int len1= Cyc_String_int_strleno( s1, 0,({
unsigned char* _temp15="String::strncasecmp"; struct _tagged_string _temp16;
_temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one= _temp15 +
20; _temp16;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,({
unsigned char* _temp13="String::strncasecmp"; struct _tagged_string _temp14;
_temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one= _temp13 +
20; _temp14;})); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n, Cyc_String_nocase_cmp);}
int Cyc_String_zstrcmp( struct _tagged_string a, struct _tagged_string b){ if( a.curr
== b.curr){ return 0;}{ unsigned int as=({ struct _tagged_string _temp18= a;(
unsigned int)( _temp18.last_plus_one - _temp18.curr);}); unsigned int bs=({
struct _tagged_string _temp17= b;( unsigned int)( _temp17.last_plus_one -
_temp17.curr);}); unsigned int min_length= as < bs? as: bs; int i= - 1; while(
++ i < min_length) { int diff=( int)*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( a, sizeof( unsigned char),
i) -( int)*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( b, sizeof( unsigned char), i); if(
diff != 0){ return diff;}} return( int) as -( int) bs;}} int Cyc_String_zstrncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int n){ if( n <= 0){
return 0;}{ unsigned int s1size=({ struct _tagged_string _temp20= s1;(
unsigned int)( _temp20.last_plus_one - _temp20.curr);}); unsigned int s2size=({
struct _tagged_string _temp19= s2;( unsigned int)( _temp19.last_plus_one -
_temp19.curr);}); unsigned int min_size= s1size > s2size? s2size: s1size;
unsigned int bound= min_size > n? n: min_size;{ int i= 0; for( 0; i < bound; i
++){ if(*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s1, sizeof( unsigned char), i) <*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
s2, sizeof( unsigned char), i)){ return - 1;} else{ if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
s2, sizeof( unsigned char), i) <*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s1, sizeof(
unsigned char), i)){ return 1;}}}} if( min_size <= bound){ return 0;} if( s1size
< s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp( struct
_tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(* a,* b);}
static struct _tagged_string Cyc_String_int_strcato( struct _tagged_string dest,
struct _tagged_string src, int srcOfs, struct _tagged_string error){ int i;
unsigned int dsize; unsigned int slen; unsigned int dlen; dsize=({ struct
_tagged_string _temp21= dest;( unsigned int)( _temp21.last_plus_one - _temp21.curr);});
dlen= Cyc_String_strlen( dest); slen= Cyc_String_int_strleno( src, srcOfs, error);
if( slen + dlen <= dsize){ for( i= 0; i < slen; i ++){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
dest, sizeof( unsigned char),( int)( i + dlen))=*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( src,
sizeof( unsigned char), i + srcOfs);} if( i != dsize){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
dest, sizeof( unsigned char),( int)( i + dlen))='\000';}} else{( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp22=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp22[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp23; _temp23.tag= Cyc_Core_InvalidArg; _temp23.f1=
error; _temp23;}); _temp22;}));} return dest;} struct _tagged_string Cyc_String_strcat(
struct _tagged_string dest, struct _tagged_string src){ return Cyc_String_int_strcato(
dest, src, 0,({ unsigned char* _temp24="String::strcat"; struct _tagged_string
_temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 15; _temp25;}));} struct _tagged_string Cyc_String_strconcat( struct
_tagged_string a, struct _tagged_string b){ unsigned int _temp26= Cyc_String_strlen(
a); goto _LL27; _LL27: { unsigned int _temp28= Cyc_String_strlen( b); goto _LL29;
_LL29: { struct _tagged_string ans= Cyc_Core_new_string(( int)( _temp26 +
_temp28)); int i; int j; for( i= 0; i < _temp26; ++ i){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
ans, sizeof( unsigned char), i)=*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( a, sizeof( unsigned char),
i);} for( j= 0; j < _temp28; ++ j){*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( ans, sizeof(
unsigned char), i + j)=*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( b, sizeof( unsigned char),
j);} return ans;}}} struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*
strs){ unsigned int len; unsigned int total_len= 0; struct Cyc_List_List* lens=({
struct Cyc_List_List* _temp31=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp31->hd=( void*)(( unsigned int) 0); _temp31->tl= 0;
_temp31;}); struct Cyc_List_List* end= lens;{ struct Cyc_List_List* p= strs;
for( 0; p != 0; p=(( struct Cyc_List_List*) _check_null( p))->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( p))->hd));
total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List*
_temp30=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp30->hd=( void*) len; _temp30->tl= 0; _temp30;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl;}} lens=(( struct Cyc_List_List*) _check_null( lens))->tl;{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=
0; while( strs != 0) { struct _tagged_string next=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( strs))->hd); len=( unsigned int)(( struct
Cyc_List_List*) _check_null( lens))->hd; Cyc_String_strncpy( ans,( int) i, next,
0, len); i += len; strs=(( struct Cyc_List_List*) _check_null( strs))->tl; lens=((
struct Cyc_List_List*) _check_null( lens))->tl;} return ans;}} struct
_tagged_string Cyc_String_str_sepstr( struct Cyc_List_List* strs, struct
_tagged_string separator){ if( strs == 0){ return({ unsigned char* _temp32="";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 1; _temp33;});} if((( struct Cyc_List_List*)
_check_null( strs))->tl == 0){ return Cyc_String_strdup(*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( strs))->hd));}{ struct
Cyc_List_List* p= strs; struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp35=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp35->hd=( void*)(( unsigned int) 0); _temp35->tl= 0; _temp35;}); struct Cyc_List_List*
end= lens; unsigned int len= 0; unsigned int total_len= 0; unsigned int list_len=
0; for( 0; p != 0; p=(( struct Cyc_List_List*) _check_null( p))->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( p))->hd));
total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List*
_temp34=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp34->hd=( void*) len; _temp34->tl= 0; _temp34;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl; ++ list_len;} lens=(( struct Cyc_List_List*) _check_null(
lens))->tl;{ unsigned int seplen= Cyc_String_strlen( separator); total_len +=(
list_len - 1) * seplen;{ struct _tagged_string ans= Cyc_Core_new_string(( int)
total_len); unsigned int i= 0; while((( struct Cyc_List_List*) _check_null( strs))->tl
!= 0) { struct _tagged_string next=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( strs))->hd); len=( unsigned int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_String_strncpy( ans,( int) i, next, 0, len); i +=
len; Cyc_String_strncpy( ans,( int) i, separator, 0, seplen); i += seplen; strs=((
struct Cyc_List_List*) _check_null( strs))->tl; lens=(( struct Cyc_List_List*)
_check_null( lens))->tl;} Cyc_String_strncpy( ans,( int) i,*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( strs))->hd), 0,(
unsigned int)(( struct Cyc_List_List*) _check_null( lens))->hd); return ans;}}}}
struct _tagged_string Cyc_String_strncpy( struct _tagged_string dest, int
destOfs, struct _tagged_string src, int srcOfs, unsigned int n){ unsigned int
_temp37=({ struct _tagged_string _temp36= dest;( unsigned int)( _temp36.last_plus_one
- _temp36.curr);}); goto _LL38; _LL38: { unsigned int _temp40=({ struct
_tagged_string _temp39= src;( unsigned int)( _temp39.last_plus_one - _temp39.curr);});
goto _LL41; _LL41: if( destOfs < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp42=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp42[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp43; _temp43.tag= Cyc_Core_InvalidArg;
_temp43.f1=({ unsigned char* _temp44="String::strncpy"; struct _tagged_string
_temp45; _temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one=
_temp44 + 16; _temp45;}); _temp43;}); _temp42;}));} if( srcOfs < 0? 1:( n > 0?
srcOfs >= _temp40: 0)){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp46=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp46[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp47; _temp47.tag= Cyc_Core_InvalidArg;
_temp47.f1=({ unsigned char* _temp48="String::strncpy"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 16; _temp49;}); _temp47;}); _temp46;}));} if( n <= 0){ return dest;}
if( destOfs + n > _temp37? 1: srcOfs + n > _temp40){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp50=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp50[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp51; _temp51.tag= Cyc_Core_InvalidArg; _temp51.f1=({
unsigned char* _temp52="String::strncpy"; struct _tagged_string _temp53; _temp53.curr=
_temp52; _temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 16; _temp53;});
_temp51;}); _temp50;}));}{ int i; for( i= 0; i < n; i ++){ unsigned char _temp54=*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( src, sizeof( unsigned char), srcOfs + i); goto _LL55;
_LL55: if( _temp54 =='\000'){ break;}*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( dest,
sizeof( unsigned char), destOfs + i)= _temp54;} for( 0; i < n; i ++){*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char), destOfs + i)='\000';}
return dest;}}} struct _tagged_string Cyc_String_zstrncpy( struct _tagged_string
dest, int destOfs, struct _tagged_string src, int srcOfs, unsigned int n){
unsigned int _temp57=({ struct _tagged_string _temp56= dest;( unsigned int)(
_temp56.last_plus_one - _temp56.curr);}); goto _LL58; _LL58: { unsigned int
_temp60=({ struct _tagged_string _temp59= src;( unsigned int)( _temp59.last_plus_one
- _temp59.curr);}); goto _LL61; _LL61: if( destOfs < 0){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp62=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp62[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp63; _temp63.tag= Cyc_Core_InvalidArg; _temp63.f1=({
unsigned char* _temp64="String::zstrncpy"; struct _tagged_string _temp65;
_temp65.curr= _temp64; _temp65.base= _temp64; _temp65.last_plus_one= _temp64 +
17; _temp65;}); _temp63;}); _temp62;}));} if( srcOfs < 0? 1:( n > 0? srcOfs >=
_temp60: 0)){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp66=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp66[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp67; _temp67.tag= Cyc_Core_InvalidArg;
_temp67.f1=({ unsigned char* _temp68="String::zstrncpy"; struct _tagged_string
_temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 17; _temp69;}); _temp67;}); _temp66;}));} if( n <= 0){ return dest;}
if( destOfs + n > _temp57? 1: srcOfs + n > _temp60){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp70=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp70[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp71; _temp71.tag= Cyc_Core_InvalidArg; _temp71.f1=({
unsigned char* _temp72="String::zstrncpy"; struct _tagged_string _temp73;
_temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one= _temp72 +
17; _temp73;}); _temp71;}); _temp70;}));}{ int i; for( i= 0; i < n; i ++){*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char), destOfs + i)=*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( src, sizeof( unsigned char), srcOfs + i);} return
dest;}}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp74= dest;(
unsigned int)( _temp74.last_plus_one - _temp74.curr);})){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
dest, sizeof( unsigned char),( int) len)='\000';} return dest;} struct
_tagged_string Cyc_String_strdup( struct _tagged_string src){ unsigned int len;
struct _tagged_string temp; len= Cyc_String_strlen( src); temp= Cyc_Core_new_string((
int) len); Cyc_String_strncpy( temp, 0, src, 0, len); return temp;} struct
_tagged_string Cyc_String_expand( struct _tagged_string s, unsigned int sz){
struct _tagged_string temp; unsigned int slen; slen= Cyc_String_strlen( s); sz=
sz > slen? sz: slen; temp= Cyc_Core_new_string(( int) sz); Cyc_String_strncpy(
temp, 0, s, 0, slen); if( slen !=({ struct _tagged_string _temp75= s;(
unsigned int)( _temp75.last_plus_one - _temp75.curr);})){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
temp, sizeof( unsigned char),( int) slen)='\000';} return temp;} struct
_tagged_string Cyc_String_realloc_str( struct _tagged_string str, unsigned int
sz){ unsigned int maxsizeP=({ struct _tagged_string _temp76= str;( unsigned int)(
_temp76.last_plus_one - _temp76.curr);}); if( maxsizeP == 0){ maxsizeP= 30 > sz?
30: sz; str= Cyc_Core_new_string(( int) maxsizeP);*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( str,
sizeof( unsigned char), 0)='\000';} else{ if( sz > maxsizeP){ maxsizeP= maxsizeP
* 2 >( sz * 5) / 4? maxsizeP * 2:( sz * 5) / 4; str= Cyc_String_expand( str,
maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring( struct
_tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt < 0)? 1:(
start +( int) amt) - 1 >( int)({ struct _tagged_string _temp77= s;( unsigned int)(
_temp77.last_plus_one - _temp77.curr);})){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp78=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp78[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp79; _temp79.tag= Cyc_Core_InvalidArg;
_temp79.f1=({ unsigned char* _temp80="String::substring"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 18; _temp81;}); _temp79;}); _temp78;}));}{ struct _tagged_string ans=
Cyc_Core_new_string(( int)( amt + 1));{ unsigned int i= 0; for( 0; i < amt; ++ i){*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char),( int) i)=*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char),( int)( start + i));}}*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char),( int) amt)='\000';
return ans;}} struct _tagged_string Cyc_String_replace_suffix( struct
_tagged_string src, struct _tagged_string curr_suffix, struct _tagged_string
new_suffix){ unsigned int m=({ struct _tagged_string _temp91= src;( unsigned int)(
_temp91.last_plus_one - _temp91.curr);}); unsigned int n=({ struct
_tagged_string _temp90= curr_suffix;( unsigned int)( _temp90.last_plus_one -
_temp90.curr);}); struct _tagged_string err=({ unsigned char* _temp88="String::replace_suffix";
struct _tagged_string _temp89; _temp89.curr= _temp88; _temp89.base= _temp88;
_temp89.last_plus_one= _temp88 + 23; _temp89;}); if( m < n){( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp82=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp82[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp83; _temp83.tag= Cyc_Core_InvalidArg; _temp83.f1=
err; _temp83;}); _temp82;}));}{ unsigned int i= 1; for( 0; i <= n; ++ i){ if(*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( src, sizeof( unsigned char),( int)( m - i)) !=*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( curr_suffix, sizeof( unsigned char),( int)( n - i))){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp84=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp84[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp85; _temp85.tag= Cyc_Core_InvalidArg; _temp85.f1=
err; _temp85;}); _temp84;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp87= new_suffix;( unsigned int)(
_temp87.last_plus_one - _temp87.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp86= new_suffix;( unsigned int)( _temp86.last_plus_one -
_temp86.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,({ unsigned char* _temp98="String::strpbrk"; struct _tagged_string _temp99;
_temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one= _temp98 +
16; _temp99;})); unsigned char c='\000'; for( 0; len > 0?( c=({ struct
_tagged_string _temp92= s; unsigned char* _temp93= _temp92.curr; if( _temp92.base
== 0? 1:( _temp93 < _temp92.base? 1: _temp93 >= _temp92.last_plus_one)){ _throw(
Null_Exception);}* _temp93;})) != 0: 0;( len --,({ struct _tagged_string*
_temp94=& s; struct _tagged_string _temp95=* _temp94; _temp94->curr ++; _temp95;}))){
int j= 0; for( 0; j <({ struct _tagged_string _temp96= accept;( unsigned int)(
_temp96.last_plus_one - _temp96.curr);}); j ++){ if( c ==*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
accept, sizeof( unsigned char), j)){ return s;}}} return({ struct _tagged_string
_temp97={ 0, 0, 0}; _temp97;});} struct _tagged_string Cyc_String_strchr( struct
_tagged_string s, unsigned char c){ int len=( int) Cyc_String_int_strleno( s, 0,({
unsigned char* _temp105="String::strchr"; struct _tagged_string _temp106;
_temp106.curr= _temp105; _temp106.base= _temp105; _temp106.last_plus_one=
_temp105 + 15; _temp106;})); unsigned char c2='\000'; for( 0; len > 0?( c2=({
struct _tagged_string _temp100= s; unsigned char* _temp101= _temp100.curr; if(
_temp100.base == 0? 1:( _temp101 < _temp100.base? 1: _temp101 >= _temp100.last_plus_one)){
_throw( Null_Exception);}* _temp101;})) != 0: 0;( len --,({ struct
_tagged_string* _temp102=& s; struct _tagged_string _temp103=* _temp102;
_temp102->curr ++; _temp103;}))){ if( c2 == c){ return s;}} return({ struct
_tagged_string _temp104={ 0, 0, 0}; _temp104;});} struct _tagged_string Cyc_String_strrchr(
struct _tagged_string s, unsigned char c){ int len=( int) Cyc_String_int_strleno(
s, 0,({ unsigned char* _temp112="String::strrchr"; struct _tagged_string
_temp113; _temp113.curr= _temp112; _temp113.base= _temp112; _temp113.last_plus_one=
_temp112 + 16; _temp113;})); int i= len - 1; s.curr += i; for( 0; i >= 0;( i --,({
struct _tagged_string* _temp107=& s; struct _tagged_string _temp108=* _temp107;
_temp107->curr --; _temp108;}))){ if(({ struct _tagged_string _temp109= s;
unsigned char* _temp110= _temp109.curr; if( _temp109.base == 0? 1:( _temp110 <
_temp109.base? 1: _temp110 >= _temp109.last_plus_one)){ _throw( Null_Exception);}*
_temp110;}) == c){ return s;}} return({ struct _tagged_string _temp111={ 0, 0, 0};
_temp111;});} unsigned int Cyc_String_strspn( struct _tagged_string s, struct
_tagged_string accept){ unsigned int len= Cyc_String_int_strleno( s, 0,({
unsigned char* _temp116="String::strspn"; struct _tagged_string _temp117;
_temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 15; _temp117;}));{ unsigned int i= 0; for( 0; i < len; i ++){ int j;
for( j= 0; j <({ struct _tagged_string _temp114= accept;( unsigned int)(
_temp114.last_plus_one - _temp114.curr);}); j ++){ if(*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char),( int) i) ==*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( accept, sizeof(
unsigned char), j)){ break;}} if( j ==({ struct _tagged_string _temp115= accept;(
unsigned int)( _temp115.last_plus_one - _temp115.curr);})){ return i;}}} return
len;} struct Cyc_List_List* Cyc_String_explode( struct _tagged_string s){ struct
Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen( s) - 1); for( 0; i
>= 0; i --){ result=({ struct Cyc_List_List* _temp118=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp118->hd=( void*)(( int)*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)); _temp118->tl= result;
_temp118;});}} return result;} struct _tagged_string Cyc_String_implode( struct
Cyc_List_List* chars){ struct _tagged_string s= Cyc_Core_new_string((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( chars)); unsigned int i= 0; while(
chars != 0) {*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),( int) i ++)=(
unsigned char)(( int)(( struct Cyc_List_List*) _check_null( chars))->hd); chars=((
struct Cyc_List_List*) _check_null( chars))->tl;} return s;} int Cyc_String_to_int(
struct _tagged_string s, int* offset){ int ans= 0; int sn= 1; int digit_seen= 0;
int i= offset == 0? 0:*(( int*) _check_null( offset)); while( i <({ struct
_tagged_string _temp119= s;( unsigned int)( _temp119.last_plus_one - _temp119.curr);})?
isspace(( int)*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { i
++;} if( i <({ struct _tagged_string _temp120= s;( unsigned int)( _temp120.last_plus_one
- _temp120.curr);})?*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) =='-': 0){
i ++; sn= - 1;} while( i <({ struct _tagged_string _temp121= s;( unsigned int)(
_temp121.last_plus_one - _temp121.curr);})? isspace(( int)*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i)): 0) { i ++;} while( i <({ struct _tagged_string
_temp122= s;( unsigned int)( _temp122.last_plus_one - _temp122.curr);})? isdigit((
int)*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { digit_seen= 1;
ans= ans * 10 +( int)(*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) -'0'); i
++;} ans= ans * sn; if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp123=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp123[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp124; _temp124.tag= Cyc_Core_InvalidArg;
_temp124.f1=({ unsigned char* _temp125="String::to_int"; struct _tagged_string
_temp126; _temp126.curr= _temp125; _temp126.base= _temp125; _temp126.last_plus_one=
_temp125 + 15; _temp126;}); _temp124;}); _temp123;}));} if( offset != 0){*(( int*)
_check_null( offset))= i;} return ans;} double Cyc_String_to_double( struct
_tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:*(( int*) _check_null( offset)); while( i <({
struct _tagged_string _temp127= s;( unsigned int)( _temp127.last_plus_one -
_temp127.curr);})? isspace(( int)*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i)): 0) { i ++;} if( i <({ struct _tagged_string _temp128= s;( unsigned int)(
_temp128.last_plus_one - _temp128.curr);})?*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) =='-': 0){ i ++; sn= - 1;} while( i <({ struct
_tagged_string _temp129= s;( unsigned int)( _temp129.last_plus_one - _temp129.curr);})?
isdigit(( int)*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) {
digit_seen= 1; ans= ans * 10.0 +( double)(*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) -'0'); i ++;} if( i <({ struct _tagged_string
_temp130= s;( unsigned int)( _temp130.last_plus_one - _temp130.curr);})?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='.': 0){ i ++;{
double divisor=( double) 0.1; while( i <({ struct _tagged_string _temp131= s;(
unsigned int)( _temp131.last_plus_one - _temp131.curr);})? isdigit(( int)*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { digit_seen= 1;
ans= ans +( double)(*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) -'0') *
divisor; divisor= divisor / 10.0; i ++;}}} if( ! digit_seen){( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp132=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp132[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp133; _temp133.tag= Cyc_Core_InvalidArg; _temp133.f1=({
unsigned char* _temp134="String::to_double"; struct _tagged_string _temp135;
_temp135.curr= _temp134; _temp135.base= _temp134; _temp135.last_plus_one=
_temp134 + 18; _temp135;}); _temp133;}); _temp132;}));} if( i <({ struct
_tagged_string _temp136= s;( unsigned int)( _temp136.last_plus_one - _temp136.curr);})?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='e'? 1:*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='E': 0){ i ++;{ int
exponent= 0; int exp_sign= 1; digit_seen= 0; if( i <({ struct _tagged_string
_temp137= s;( unsigned int)( _temp137.last_plus_one - _temp137.curr);})?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='-': 0){ i ++;
exp_sign= - 1;} else{ if( i <({ struct _tagged_string _temp138= s;( unsigned int)(
_temp138.last_plus_one - _temp138.curr);})?*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) =='+': 0){ i ++;}} while( i <({ struct _tagged_string
_temp139= s;( unsigned int)( _temp139.last_plus_one - _temp139.curr);})? isdigit((
int)*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { digit_seen= 1;
exponent= exponent * 10 +( int)(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i) -'0'); i ++;} if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp140=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp140[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp141; _temp141.tag= Cyc_Core_InvalidArg;
_temp141.f1=({ unsigned char* _temp142="String::to_double"; struct
_tagged_string _temp143; _temp143.curr= _temp142; _temp143.base= _temp142;
_temp143.last_plus_one= _temp142 + 18; _temp143;}); _temp141;}); _temp140;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){*(( int*) _check_null( offset))= i;} return ans * sn;}