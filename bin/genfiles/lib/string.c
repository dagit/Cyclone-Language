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
- _temp0.curr);}); i ++){ if(*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),(
int) i) =='\000'){ return i;}} return i;} static unsigned int Cyc_String_int_strleno(
struct _tagged_string s, int ofs, struct _tagged_string error){ if( ofs >({
struct _tagged_string _temp1= s;( unsigned int)( _temp1.last_plus_one - _temp1.curr);})?
1: ofs < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp2=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp3; _temp3.tag= Cyc_Core_InvalidArg;
_temp3.f1= error; _temp3;}); _temp2;}));}{ int i; for( i= ofs; i <({ struct
_tagged_string _temp4= s;( unsigned int)( _temp4.last_plus_one - _temp4.curr);});
i ++){ if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) =='\000'){
break;}} return( unsigned int)( i - ofs);}} static int Cyc_String_case_cmp(
unsigned char c1, unsigned char c2){ return c1 - c2;} static int Cyc_String_nocase_cmp(
unsigned char c1, unsigned char c2){ return Cyc_String_case_cmp(( unsigned char)
toupper(( int) c1),( unsigned char) toupper(( int) c2));} static int Cyc_String_cmp(
struct _tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2,
int ofs2, unsigned int len2, int(* f)( unsigned char, unsigned char)){
unsigned int min_length= len1 < len2? len1: len2; int i= - 1; while( ++ i <
min_length) { int diff= f(*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s1, sizeof(
unsigned char), i + ofs1),*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s2, sizeof(
unsigned char), i + ofs2)); if( diff != 0){ return diff;}} return( int) len1 -(
int) len2;} int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2){ if( s1.curr == s2.curr){ return 0;}{ unsigned int len1= Cyc_String_int_strleno(
s1, 0,({ unsigned char* _temp7=( unsigned char*)"String::strcmp"; struct
_tagged_string _temp8; _temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one=
_temp7 + 15; _temp8;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,({
unsigned char* _temp5=( unsigned char*)"String::strcmp"; struct _tagged_string
_temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one= _temp5 +
15; _temp6;})); return Cyc_String_cmp( s1, 0, len1, s2, 0, len2, Cyc_String_case_cmp);}}
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2){
return Cyc_String_strcmp(* s1,* s2);} static int Cyc_String_ncmp( struct
_tagged_string s1, int ofs1, unsigned int len1, struct _tagged_string s2, int
ofs2, unsigned int len2, unsigned int n, int(* f)( unsigned char, unsigned char)){
if( n <= 0){ return 0;}{ unsigned int min_len= len1 > len2? len2: len1;
unsigned int bound= min_len > n? n: min_len;{ int i= 0; for( 0; i < bound; i ++){
int retc; if(( retc= f(*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s1, sizeof(
unsigned char), i + ofs1),*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s2, sizeof(
unsigned char), i + ofs2))) != 0){ return retc;}}} return 0;}} int Cyc_String_strncmp(
struct _tagged_string s1, struct _tagged_string s2, unsigned int n){
unsigned int len1= Cyc_String_int_strleno( s1, 0,({ unsigned char* _temp11=(
unsigned char*)"String::strncmp"; struct _tagged_string _temp12; _temp12.curr=
_temp11; _temp12.base= _temp11; _temp12.last_plus_one= _temp11 + 16; _temp12;}));
unsigned int len2= Cyc_String_int_strleno( s2, 0,({ unsigned char* _temp9=(
unsigned char*)"String::strncmp"; struct _tagged_string _temp10; _temp10.curr=
_temp9; _temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 16; _temp10;}));
return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n, Cyc_String_case_cmp);} int
Cyc_String_strncasecmp( struct _tagged_string s1, struct _tagged_string s2,
unsigned int n){ unsigned int len1= Cyc_String_int_strleno( s1, 0,({
unsigned char* _temp15=( unsigned char*)"String::strncasecmp"; struct
_tagged_string _temp16; _temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one=
_temp15 + 20; _temp16;})); unsigned int len2= Cyc_String_int_strleno( s2, 0,({
unsigned char* _temp13=( unsigned char*)"String::strncasecmp"; struct
_tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one=
_temp13 + 20; _temp14;})); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n,
Cyc_String_nocase_cmp);} int Cyc_String_zstrcmp( struct _tagged_string a, struct
_tagged_string b){ if( a.curr == b.curr){ return 0;}{ unsigned int as=({ struct
_tagged_string _temp18= a;( unsigned int)( _temp18.last_plus_one - _temp18.curr);});
unsigned int bs=({ struct _tagged_string _temp17= b;( unsigned int)( _temp17.last_plus_one
- _temp17.curr);}); unsigned int min_length= as < bs? as: bs; int i= - 1; while(
++ i < min_length) { int diff=( int)*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( a,
sizeof( unsigned char), i) -( int)*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( b,
sizeof( unsigned char), i); if( diff != 0){ return diff;}} return( int) as -(
int) bs;}} int Cyc_String_zstrncmp( struct _tagged_string s1, struct
_tagged_string s2, unsigned int n){ if( n <= 0){ return 0;}{ unsigned int s1size=({
struct _tagged_string _temp20= s1;( unsigned int)( _temp20.last_plus_one -
_temp20.curr);}); unsigned int s2size=({ struct _tagged_string _temp19= s2;(
unsigned int)( _temp19.last_plus_one - _temp19.curr);}); unsigned int min_size=
s1size > s2size? s2size: s1size; unsigned int bound= min_size > n? n: min_size;{
int i= 0; for( 0; i < bound; i ++){ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s1,
sizeof( unsigned char), i) <*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s2, sizeof(
unsigned char), i)){ return - 1;} else{ if(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s2,
sizeof( unsigned char), i) <*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s1, sizeof(
unsigned char), i)){ return 1;}}}} if( min_size <= bound){ return 0;} if( s1size
< s2size){ return - 1;} else{ return 1;}}} int Cyc_String_zstrptrcmp( struct
_tagged_string* a, struct _tagged_string* b){ return Cyc_String_zstrcmp(* a,* b);}
static struct _tagged_string Cyc_String_int_strcato( struct _tagged_string dest,
struct _tagged_string src, int srcOfs, struct _tagged_string error){ int i;
unsigned int dsize; unsigned int slen; unsigned int dlen; dsize=({ struct
_tagged_string _temp21= dest;( unsigned int)( _temp21.last_plus_one - _temp21.curr);});
dlen= Cyc_String_strlen(( struct _tagged_string) dest); slen= Cyc_String_int_strleno(
src, srcOfs, error); if( slen + dlen <= dsize){ for( i= 0; i < slen; i ++){*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char),( int)( i + dlen))=*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( src, sizeof( unsigned char), i + srcOfs);} if( i !=
dsize){*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char),( int)( i + dlen))='\000';}}
else{( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp22=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp22[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp23; _temp23.tag= Cyc_Core_InvalidArg;
_temp23.f1= error; _temp23;}); _temp22;}));} return dest;} struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src){
return Cyc_String_int_strcato( dest, src, 0,({ unsigned char* _temp24=(
unsigned char*)"String::strcat"; struct _tagged_string _temp25; _temp25.curr=
_temp24; _temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 15; _temp25;}));}
struct _tagged_string Cyc_String_strconcat( struct _tagged_string a, struct
_tagged_string b){ unsigned int _temp26= Cyc_String_strlen( a); goto _LL27;
_LL27: { unsigned int _temp28= Cyc_String_strlen( b); goto _LL29; _LL29: {
struct _tagged_string ans= Cyc_Core_new_string(( int)( _temp26 + _temp28)); int
i; int j; for( i= 0; i < _temp26; ++ i){*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( ans,
sizeof( unsigned char), i)=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( a, sizeof( unsigned char),
i);} for( j= 0; j < _temp28; ++ j){*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( ans, sizeof(
unsigned char), i + j)=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( b, sizeof( unsigned char),
j);} return ans;}}} struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*
strs){ unsigned int len; unsigned int total_len= 0; struct Cyc_List_List* lens=({
struct Cyc_List_List* _temp33=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp33->hd=( void*)(( unsigned int) 0); _temp33->tl= 0;
_temp33;}); struct Cyc_List_List* end= lens;{ struct Cyc_List_List* p= strs;
for( 0; p != 0; p=(( struct Cyc_List_List*) _check_null( p))->tl){ len= Cyc_String_strlen(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( p))->hd));
total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List*
_temp30=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp30->hd=( void*) len; _temp30->tl= 0; _temp30;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl;}} lens=(( struct Cyc_List_List*) _check_null( lens))->tl;{
struct _tagged_string ans= Cyc_Core_new_string(( int) total_len); unsigned int i=
0; while( strs != 0) { struct _tagged_string _temp31=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( strs))->hd); goto _LL32; _LL32: len=(
unsigned int)(( struct Cyc_List_List*) _check_null( lens))->hd; Cyc_String_strncpy(
ans,( int) i, _temp31, 0, len); i += len; strs=(( struct Cyc_List_List*)
_check_null( strs))->tl; lens=(( struct Cyc_List_List*) _check_null( lens))->tl;}
return ans;}} struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*
strs, struct _tagged_string separator){ if( strs == 0){ return Cyc_Core_new_string(
0);} if((( struct Cyc_List_List*) _check_null( strs))->tl == 0){ return Cyc_String_strdup(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( strs))->hd));}{
struct Cyc_List_List* p= strs; struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp37=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp37->hd=( void*)(( unsigned int) 0); _temp37->tl= 0; _temp37;}); struct Cyc_List_List*
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
!= 0) { struct _tagged_string _temp35=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( strs))->hd); goto _LL36; _LL36: len=( unsigned int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_String_strncpy( ans,( int) i, _temp35, 0, len); i +=
len; Cyc_String_strncpy( ans,( int) i, separator, 0, seplen); i += seplen; strs=((
struct Cyc_List_List*) _check_null( strs))->tl; lens=(( struct Cyc_List_List*)
_check_null( lens))->tl;} Cyc_String_strncpy( ans,( int) i,*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( strs))->hd), 0,(
unsigned int)(( struct Cyc_List_List*) _check_null( lens))->hd); return ans;}}}}
struct _tagged_string Cyc_String_strncpy( struct _tagged_string dest, int
destOfs, struct _tagged_string src, int srcOfs, unsigned int n){ unsigned int
_temp39=({ struct _tagged_string _temp38= dest;( unsigned int)( _temp38.last_plus_one
- _temp38.curr);}); goto _LL40; _LL40: { unsigned int _temp42=({ struct
_tagged_string _temp41= src;( unsigned int)( _temp41.last_plus_one - _temp41.curr);});
goto _LL43; _LL43: if( destOfs < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp44=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp44[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp45; _temp45.tag= Cyc_Core_InvalidArg;
_temp45.f1=({ unsigned char* _temp46=( unsigned char*)"String::strncpy"; struct
_tagged_string _temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 16; _temp47;}); _temp45;}); _temp44;}));} if( srcOfs < 0? 1:( n > 0?
srcOfs >= _temp42: 0)){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp48=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp48[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp49; _temp49.tag= Cyc_Core_InvalidArg;
_temp49.f1=({ unsigned char* _temp50=( unsigned char*)"String::strncpy"; struct
_tagged_string _temp51; _temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one=
_temp50 + 16; _temp51;}); _temp49;}); _temp48;}));} if( n <= 0){ return dest;}
if( destOfs + n > _temp39? 1: srcOfs + n > _temp42){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp52=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp52[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp53; _temp53.tag= Cyc_Core_InvalidArg; _temp53.f1=({
unsigned char* _temp54=( unsigned char*)"String::strncpy"; struct _tagged_string
_temp55; _temp55.curr= _temp54; _temp55.base= _temp54; _temp55.last_plus_one=
_temp54 + 16; _temp55;}); _temp53;}); _temp52;}));}{ int i; for( i= 0; i < n; i
++){ unsigned char _temp56=*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( src, sizeof(
unsigned char), srcOfs + i); goto _LL57; _LL57: if( _temp56 =='\000'){ break;}*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char), destOfs + i)= _temp56;}
for( 0; i < n; i ++){*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( dest, sizeof( unsigned char), destOfs
+ i)='\000';} return dest;}}} struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string dest, int destOfs, struct _tagged_string src, int srcOfs,
unsigned int n){ unsigned int _temp59=({ struct _tagged_string _temp58= dest;(
unsigned int)( _temp58.last_plus_one - _temp58.curr);}); goto _LL60; _LL60: {
unsigned int _temp62=({ struct _tagged_string _temp61= src;( unsigned int)(
_temp61.last_plus_one - _temp61.curr);}); goto _LL63; _LL63: if( destOfs < 0){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp64=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp64[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp65; _temp65.tag= Cyc_Core_InvalidArg; _temp65.f1=({
unsigned char* _temp66=( unsigned char*)"String::zstrncpy"; struct
_tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66; _temp67.last_plus_one=
_temp66 + 17; _temp67;}); _temp65;}); _temp64;}));} if( srcOfs < 0? 1:( n > 0?
srcOfs >= _temp62: 0)){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp68=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp68[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp69; _temp69.tag= Cyc_Core_InvalidArg;
_temp69.f1=({ unsigned char* _temp70=( unsigned char*)"String::zstrncpy"; struct
_tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 17; _temp71;}); _temp69;}); _temp68;}));} if( n <= 0){ return dest;}
if( destOfs + n > _temp59? 1: srcOfs + n > _temp62){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp72=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp72[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp73; _temp73.tag= Cyc_Core_InvalidArg; _temp73.f1=({
unsigned char* _temp74=( unsigned char*)"String::zstrncpy"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 17; _temp75;}); _temp73;}); _temp72;}));}{ int i; for( i= 0; i < n; i
++){*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( dest, sizeof( unsigned char), destOfs + i)=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( src, sizeof( unsigned char), srcOfs + i);} return
dest;}}} struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src){ unsigned int len= Cyc_String_strlen( src); Cyc_String_strncpy(
dest, 0, src, 0, len); if( len !=({ struct _tagged_string _temp76= dest;(
unsigned int)( _temp76.last_plus_one - _temp76.curr);})){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
dest, sizeof( unsigned char),( int) len)='\000';} return dest;} struct
_tagged_string Cyc_String_strdup( struct _tagged_string src){ unsigned int len;
struct _tagged_string temp; len= Cyc_String_strlen( src); temp= Cyc_Core_new_string((
int) len); Cyc_String_strncpy( temp, 0, src, 0, len); return temp;} struct
_tagged_string Cyc_String_expand( struct _tagged_string s, unsigned int sz){
struct _tagged_string temp; unsigned int slen; slen= Cyc_String_strlen( s); sz=
sz > slen? sz: slen; temp= Cyc_Core_new_string(( int) sz); Cyc_String_strncpy(
temp, 0, s, 0, slen); if( slen !=({ struct _tagged_string _temp77= s;(
unsigned int)( _temp77.last_plus_one - _temp77.curr);})){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
temp, sizeof( unsigned char),( int) slen)='\000';} return temp;} struct
_tagged_string Cyc_String_realloc_str( struct _tagged_string str, unsigned int
sz){ unsigned int maxsizeP=({ struct _tagged_string _temp78= str;( unsigned int)(
_temp78.last_plus_one - _temp78.curr);}); if( maxsizeP == 0){ maxsizeP= 30 > sz?
30: sz; str= Cyc_Core_new_string(( int) maxsizeP);*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( str,
sizeof( unsigned char), 0)='\000';} else{ if( sz > maxsizeP){ maxsizeP= maxsizeP
* 2 >( sz * 5) / 4? maxsizeP * 2:( sz * 5) / 4; str= Cyc_String_expand(( struct
_tagged_string) str, maxsizeP);}} return str;} struct _tagged_string Cyc_String_substring(
struct _tagged_string s, int start, unsigned int amt){ if(( start < 0? 1: amt <
0)? 1:( start +( int) amt) - 1 >( int)({ struct _tagged_string _temp79= s;(
unsigned int)( _temp79.last_plus_one - _temp79.curr);})){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp80=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp80[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp81; _temp81.tag= Cyc_Core_InvalidArg; _temp81.f1=({
unsigned char* _temp82=( unsigned char*)"String::substring"; struct
_tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 18; _temp83;}); _temp81;}); _temp80;}));}{ struct _tagged_string ans=
Cyc_Core_new_string(( int)( amt + 1));{ unsigned int i= 0; for( 0; i < amt; ++ i){*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char),( int) i)=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char),( int)( start + i));}}*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char),( int) amt)='\000';
return ans;}} struct _tagged_string Cyc_String_replace_suffix( struct
_tagged_string src, struct _tagged_string curr_suffix, struct _tagged_string
new_suffix){ unsigned int m=({ struct _tagged_string _temp93= src;( unsigned int)(
_temp93.last_plus_one - _temp93.curr);}); unsigned int n=({ struct
_tagged_string _temp92= curr_suffix;( unsigned int)( _temp92.last_plus_one -
_temp92.curr);}); struct _tagged_string err=({ unsigned char* _temp90=(
unsigned char*)"String::replace_suffix"; struct _tagged_string _temp91; _temp91.curr=
_temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 23; _temp91;});
if( m < n){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp84=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp84[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp85; _temp85.tag= Cyc_Core_InvalidArg;
_temp85.f1= err; _temp85;}); _temp84;}));}{ unsigned int i= 1; for( 0; i <= n;
++ i){ if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( src, sizeof( unsigned char),( int)( m
- i)) !=*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( curr_suffix, sizeof( unsigned char),(
int)( n - i))){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp86=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp86[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp87; _temp87.tag= Cyc_Core_InvalidArg;
_temp87.f1= err; _temp87;}); _temp86;}));}}}{ struct _tagged_string ans= Cyc_Core_new_string((
int)(( m - n) +({ struct _tagged_string _temp89= new_suffix;( unsigned int)(
_temp89.last_plus_one - _temp89.curr);}))); Cyc_String_strncpy( ans, 0, src, 0,
m - n); Cyc_String_strncpy( ans,( int)( m - n), new_suffix, 0,({ struct
_tagged_string _temp88= new_suffix;( unsigned int)( _temp88.last_plus_one -
_temp88.curr);})); return ans;}} struct _tagged_string Cyc_String_strpbrk(
struct _tagged_string s, struct _tagged_string accept){ int len=( int) Cyc_String_int_strleno(
s, 0,({ unsigned char* _temp100=( unsigned char*)"String::strpbrk"; struct
_tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 16; _temp101;})); unsigned char c='\000';
for( 0; len > 0?( c=({ struct _tagged_string _temp94= s; const unsigned char*
_temp95= _temp94.curr; if( _temp94.base == 0? 1:( _temp95 < _temp94.base? 1:
_temp95 >= _temp94.last_plus_one)){ _throw( Null_Exception);}* _temp95;})) != 0:
0;( len --,({ struct _tagged_string* _temp96=& s; struct _tagged_string _temp97=*
_temp96; _temp96->curr ++; _temp97;}))){ int j= 0; for( 0; j <({ struct
_tagged_string _temp98= accept;( unsigned int)( _temp98.last_plus_one - _temp98.curr);});
j ++){ if( c ==*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( accept, sizeof( unsigned char), j)){
return s;}}} return( struct _tagged_string)({ struct _tagged_string _temp99={ 0,
0, 0}; _temp99;});} struct _tagged_string Cyc_String_strchr( struct
_tagged_string s, unsigned char c){ int len=( int) Cyc_String_int_strleno( s, 0,({
unsigned char* _temp107=( unsigned char*)"String::strchr"; struct _tagged_string
_temp108; _temp108.curr= _temp107; _temp108.base= _temp107; _temp108.last_plus_one=
_temp107 + 15; _temp108;})); unsigned char c2='\000'; for( 0; len > 0?( c2=({
struct _tagged_string _temp102= s; const unsigned char* _temp103= _temp102.curr;
if( _temp102.base == 0? 1:( _temp103 < _temp102.base? 1: _temp103 >= _temp102.last_plus_one)){
_throw( Null_Exception);}* _temp103;})) != 0: 0;( len --,({ struct
_tagged_string* _temp104=& s; struct _tagged_string _temp105=* _temp104;
_temp104->curr ++; _temp105;}))){ if( c2 == c){ return s;}} return( struct
_tagged_string)({ struct _tagged_string _temp106={ 0, 0, 0}; _temp106;});}
struct _tagged_string Cyc_String_strrchr( struct _tagged_string s, unsigned char
c){ int len=( int) Cyc_String_int_strleno( s, 0,({ unsigned char* _temp114=(
unsigned char*)"String::strrchr"; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 16;
_temp115;})); int i= len - 1; s.curr += i; for( 0; i >= 0;( i --,({ struct
_tagged_string* _temp109=& s; struct _tagged_string _temp110=* _temp109;
_temp109->curr --; _temp110;}))){ if(({ struct _tagged_string _temp111= s; const
unsigned char* _temp112= _temp111.curr; if( _temp111.base == 0? 1:( _temp112 <
_temp111.base? 1: _temp112 >= _temp111.last_plus_one)){ _throw( Null_Exception);}*
_temp112;}) == c){ return s;}} return( struct _tagged_string)({ struct
_tagged_string _temp113={ 0, 0, 0}; _temp113;});} unsigned int Cyc_String_strspn(
struct _tagged_string s, struct _tagged_string accept){ unsigned int len= Cyc_String_int_strleno(
s, 0,({ unsigned char* _temp118=( unsigned char*)"String::strspn"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 15; _temp119;}));{ unsigned int i= 0; for( 0;
i < len; i ++){ int j; for( j= 0; j <({ struct _tagged_string _temp116= accept;(
unsigned int)( _temp116.last_plus_one - _temp116.curr);}); j ++){ if(*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char),( int) i) ==*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( accept, sizeof( unsigned char), j)){ break;}} if( j
==({ struct _tagged_string _temp117= accept;( unsigned int)( _temp117.last_plus_one
- _temp117.curr);})){ return i;}}} return len;} struct Cyc_List_List* Cyc_String_explode(
struct _tagged_string s){ struct Cyc_List_List* result= 0;{ int i=( int)( Cyc_String_strlen(
s) - 1); for( 0; i >= 0; i --){ result=({ struct Cyc_List_List* _temp120=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp120->hd=(
void*)(( int)*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i));
_temp120->tl= result; _temp120;});}} return result;} struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* chars){ struct _tagged_string s= Cyc_Core_new_string(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( chars)); unsigned int i= 0;
while( chars != 0) {*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),( int) i ++)=(
unsigned char)(( int)(( struct Cyc_List_List*) _check_null( chars))->hd); chars=((
struct Cyc_List_List*) _check_null( chars))->tl;} return s;} int Cyc_String_to_int(
struct _tagged_string s, int* offset){ int ans= 0; int sn= 1; int digit_seen= 0;
int i= offset == 0? 0:*(( int*) _check_null( offset)); while( i <({ struct
_tagged_string _temp121= s;( unsigned int)( _temp121.last_plus_one - _temp121.curr);})?
isspace(( int)*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { i
++;} if( i <({ struct _tagged_string _temp122= s;( unsigned int)( _temp122.last_plus_one
- _temp122.curr);})?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i) =='-': 0){ i ++; sn= - 1;} while( i <({ struct _tagged_string _temp123= s;(
unsigned int)( _temp123.last_plus_one - _temp123.curr);})? isspace(( int)*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) { i ++;} while( i
<({ struct _tagged_string _temp124= s;( unsigned int)( _temp124.last_plus_one -
_temp124.curr);})? isdigit(( int)*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i)): 0) { digit_seen= 1; ans= ans * 10 +( int)(*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) -'0'); i ++;} ans= ans
* sn; if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp125=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp125[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp126; _temp126.tag= Cyc_Core_InvalidArg;
_temp126.f1=({ unsigned char* _temp127=( unsigned char*)"String::to_int"; struct
_tagged_string _temp128; _temp128.curr= _temp127; _temp128.base= _temp127;
_temp128.last_plus_one= _temp127 + 15; _temp128;}); _temp126;}); _temp125;}));}
if( offset != 0){*(( int*) _check_null( offset))= i;} return ans;} double Cyc_String_to_double(
struct _tagged_string s, int* offset){ double ans=( double) 0.0; int sn= 1; int
digit_seen= 0; int i= offset == 0? 0:*(( int*) _check_null( offset)); while( i <({
struct _tagged_string _temp129= s;( unsigned int)( _temp129.last_plus_one -
_temp129.curr);})? isspace(( int)*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i)): 0) { i ++;} if( i <({ struct _tagged_string
_temp130= s;( unsigned int)( _temp130.last_plus_one - _temp130.curr);})?*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='-': 0){ i ++; sn= -
1;} while( i <({ struct _tagged_string _temp131= s;( unsigned int)( _temp131.last_plus_one
- _temp131.curr);})? isdigit(( int)*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i)): 0) { digit_seen= 1; ans= ans * 10.0 +( double)(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) -'0'); i ++;} if( i <({
struct _tagged_string _temp132= s;( unsigned int)( _temp132.last_plus_one -
_temp132.curr);})?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i) =='.': 0){ i ++;{ double divisor=( double) 0.1; while( i <({ struct
_tagged_string _temp133= s;( unsigned int)( _temp133.last_plus_one - _temp133.curr);})?
isdigit(( int)*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) {
digit_seen= 1; ans= ans +( double)(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) -'0') * divisor; divisor= divisor / 10.0; i ++;}}}
if( ! digit_seen){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp134=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp134[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp135; _temp135.tag= Cyc_Core_InvalidArg;
_temp135.f1=({ unsigned char* _temp136=( unsigned char*)"String::to_double";
struct _tagged_string _temp137; _temp137.curr= _temp136; _temp137.base= _temp136;
_temp137.last_plus_one= _temp136 + 18; _temp137;}); _temp135;}); _temp134;}));}
if( i <({ struct _tagged_string _temp138= s;( unsigned int)( _temp138.last_plus_one
- _temp138.curr);})?*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i) =='e'? 1:*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i) =='E': 0){
i ++;{ int exponent= 0; int exp_sign= 1; digit_seen= 0; if( i <({ struct
_tagged_string _temp139= s;( unsigned int)( _temp139.last_plus_one - _temp139.curr);})?*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='-': 0){ i ++;
exp_sign= - 1;} else{ if( i <({ struct _tagged_string _temp140= s;( unsigned int)(
_temp140.last_plus_one - _temp140.curr);})?*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) =='+': 0){ i ++;}} while( i <({ struct _tagged_string
_temp141= s;( unsigned int)( _temp141.last_plus_one - _temp141.curr);})? isdigit((
int)*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), i)): 0) {
digit_seen= 1; exponent= exponent * 10 +( int)(*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) -'0'); i ++;} if( ! digit_seen){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp142=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp142[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp143; _temp143.tag= Cyc_Core_InvalidArg; _temp143.f1=({
unsigned char* _temp144=( unsigned char*)"String::to_double"; struct
_tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 18; _temp145;}); _temp143;}); _temp142;}));}
ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}} if( offset !=
0){*(( int*) _check_null( offset))= i;} return ans * sn;}