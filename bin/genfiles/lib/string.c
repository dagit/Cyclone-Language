 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern struct _tagged_arr Cyc_Core_rnew_string(
struct _RegionHandle*, int); extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int isalnum( int __c);
extern int isalpha( int __c); extern int iscntrl( int __c); extern int isdigit(
int __c); extern int isgraph( int __c); extern int islower( int __c); extern int
isprint( int __c); extern int ispunct( int __c); extern int isspace( int __c);
extern int isupper( int __c); extern int isxdigit( int __c); extern int tolower(
int __c); extern int toupper( int __c); extern int isascii( int __c); extern int
toascii( int __c); extern int _tolower( int __c); extern int _toupper( int __c);
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern struct _tagged_arr Cyc_String_strerror( int);
extern int Cyc_String_strlen( struct _tagged_arr s); extern int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_String_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_String_strncmp(
struct _tagged_arr s1, struct _tagged_arr s2, int len); extern int Cyc_String_zstrcmp(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_String_zstrncmp( struct
_tagged_arr s1, struct _tagged_arr s2, int n); extern int Cyc_String_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_String_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_String_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_String_rstrconcat(
struct _RegionHandle*, struct _tagged_arr, struct _tagged_arr); extern struct
_tagged_arr Cyc_String_strconcat_l( struct Cyc_List_List*); extern struct
_tagged_arr Cyc_String_rstrconcat_l( struct _RegionHandle*, struct Cyc_List_List*);
extern struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern struct _tagged_arr Cyc_String_rstr_sepstr( struct
_RegionHandle*, struct Cyc_List_List*, struct _tagged_arr); extern struct
_tagged_arr Cyc_String_strcpy( struct _tagged_arr dest, struct _tagged_arr src);
extern struct _tagged_arr Cyc_String_strncpy( struct _tagged_arr, struct
_tagged_arr, int); extern struct _tagged_arr Cyc_String_zstrncpy( struct
_tagged_arr, struct _tagged_arr, int); extern struct _tagged_arr Cyc_String_expand(
struct _tagged_arr s, int sz); extern struct _tagged_arr Cyc_String_rexpand(
struct _RegionHandle*, struct _tagged_arr s, int sz); extern struct _tagged_arr
Cyc_String_realloc_str( struct _tagged_arr str, int sz); extern struct
_tagged_arr Cyc_String_rrealloc_str( struct _RegionHandle* r, struct _tagged_arr
str, int sz); extern struct _tagged_arr Cyc_String_strdup( struct _tagged_arr
src); extern struct _tagged_arr Cyc_String_rstrdup( struct _RegionHandle*,
struct _tagged_arr src); extern struct _tagged_arr Cyc_String_substring( struct
_tagged_arr, int ofs, int n); extern struct _tagged_arr Cyc_String_rsubstring(
struct _RegionHandle*, struct _tagged_arr, int ofs, int n); extern struct
_tagged_arr Cyc_String_replace_suffix( struct _tagged_arr, struct _tagged_arr,
struct _tagged_arr); extern struct _tagged_arr Cyc_String_rreplace_suffix(
struct _RegionHandle* r, struct _tagged_arr src, struct _tagged_arr curr_suffix,
struct _tagged_arr new_suffix); extern struct _tagged_arr Cyc_String_strchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_String_strrchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_String_strpbrk(
struct _tagged_arr s, struct _tagged_arr accept); extern int Cyc_String_strspn(
struct _tagged_arr s, struct _tagged_arr accept); extern struct Cyc_List_List*
Cyc_String_explode( struct _tagged_arr s); extern struct Cyc_List_List* Cyc_String_rexplode(
struct _RegionHandle*, struct _tagged_arr s); extern struct _tagged_arr Cyc_String_implode(
struct Cyc_List_List* c); extern int Cyc_String_to_int( struct _tagged_arr s,
int* offset); extern double Cyc_String_to_double( struct _tagged_arr s, int*
offset); extern int Cyc_Strings_strcasecmp( struct _tagged_arr, struct
_tagged_arr); extern int Cyc_Strings_strncasecmp( struct _tagged_arr, struct
_tagged_arr, int); extern double atan( double); extern double cos( double);
extern double sin( double); extern double tan( double); extern double tanh(
double); extern double frexp( double, int*); extern double modf( double, double*);
extern double ceil( double); extern double fabs( double); extern double floor(
double); extern double acos( double); extern double asin( double); extern double
atan2( double, double); extern double cosh( double); extern double sinh( double);
extern double exp( double); extern double ldexp( double, int); extern double log(
double); extern double log10( double); extern double pow( double, double);
extern double sqrt( double); extern double fmod( double, double); extern double
infinity(); extern double nan(); extern int isnan( double); extern int isinf(
double); extern int finite( double); extern double copysign( double, double);
extern int ilogb( double); extern double asinh( double); extern double cbrt(
double); extern double nextafter( double, double); extern double rint( double);
extern double scalbn( double, int); extern double log1p( double); extern double
expm1( double); extern double acosh( double); extern double atanh( double);
extern double remainder( double, double); extern double gamma( double); extern
double gamma_r( double, int*); extern double lgamma( double); extern double
lgamma_r( double, int*); extern double erf( double); extern double erfc( double);
extern double y0( double); extern double y1( double); extern double yn( int,
double); extern double j0( double); extern double j1( double); extern double jn(
int, double); extern double hypot( double, double); extern double cabs(); extern
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
e); extern unsigned char* strerror( int errnum); struct _tagged_arr Cyc_String_strerror(
int errnum){ return Cstring_to_string( strerror( errnum));} int Cyc_String_strlen(
struct _tagged_arr s){ int i; for( i= 0; i < _get_arr_size( s, sizeof(
unsigned char)); i ++){ if(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) =='\000'){ return i;}} return i;} static int Cyc_String_int_strleno(
struct _tagged_arr s, int ofs, struct _tagged_arr error){ if( ofs >
_get_arr_size( s, sizeof( unsigned char))? 1: ofs < 0){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp0=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp1; _temp1.tag= Cyc_Core_InvalidArg; _temp1.f1= error; _temp1;}); _temp0;}));}{
int i; for( i= ofs; i < _get_arr_size( s, sizeof( unsigned char)); i ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
=='\000'){ break;}} return( int)( i - ofs);}} static int Cyc_String_case_cmp(
unsigned char c1, unsigned char c2){ return c1 - c2;} static int Cyc_String_cmp(
struct _tagged_arr s1, int ofs1, int len1, struct _tagged_arr s2, int ofs2, int
len2, int(* f)( unsigned char, unsigned char)){ int min_length= len1 < len2?
len1: len2; int i= - 1; while( ++ i < min_length) { int diff= f(*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i + ofs1)),*((
const unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i +
ofs2))); if( diff != 0){ return diff;}} return( int) len1 -( int) len2;} int Cyc_String_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr == s2.curr){ return 0;}{
int len1= Cyc_String_int_strleno( s1, 0, _tag_arr("String::strcmp", sizeof(
unsigned char), 15u)); int len2= Cyc_String_int_strleno( s2, 0, _tag_arr("String::strcmp",
sizeof( unsigned char), 15u)); return Cyc_String_cmp( s1, 0, len1, s2, 0, len2,
Cyc_String_case_cmp);}} int Cyc_String_strptrcmp( struct _tagged_arr* s1, struct
_tagged_arr* s2){ return Cyc_String_strcmp(* s1,* s2);} static int Cyc_String_ncmp(
struct _tagged_arr s1, int ofs1, int len1, struct _tagged_arr s2, int ofs2, int
len2, int n, int(* f)( unsigned char, unsigned char)){ if( n <= 0){ return 0;}{
int min_len= len1 > len2? len2: len1; int bound= min_len > n? n: min_len;{ int i=
0; for( 0; i < bound; i ++){ int retc; if(( retc= f(*(( const unsigned char*)
_check_unknown_subscript( s1, sizeof( unsigned char), i + ofs1)),*(( const
unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i + ofs2))))
!= 0){ return retc;}}} return 0;}} int Cyc_String_strncmp( struct _tagged_arr s1,
struct _tagged_arr s2, int n){ int len1= Cyc_String_int_strleno( s1, 0, _tag_arr("String::strncmp",
sizeof( unsigned char), 16u)); int len2= Cyc_String_int_strleno( s2, 0, _tag_arr("String::strncmp",
sizeof( unsigned char), 16u)); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2,
n, Cyc_String_case_cmp);} int Cyc_String_zstrcmp( struct _tagged_arr a, struct
_tagged_arr b){ if( a.curr == b.curr){ return 0;}{ int as=( int) _get_arr_size(
a, sizeof( unsigned char)); int bs=( int) _get_arr_size( b, sizeof(
unsigned char)); int min_length= as < bs? as: bs; int i= - 1; while( ++ i <
min_length) { int diff=( int)*(( const unsigned char*) _check_unknown_subscript(
a, sizeof( unsigned char), i)) -( int)*(( const unsigned char*)
_check_unknown_subscript( b, sizeof( unsigned char), i)); if( diff != 0){ return
diff;}} return( int) as -( int) bs;}} int Cyc_String_zstrncmp( struct
_tagged_arr s1, struct _tagged_arr s2, int n){ if( n <= 0){ return 0;}{ int
s1size=( int) _get_arr_size( s1, sizeof( unsigned char)); int s2size=( int)
_get_arr_size( s2, sizeof( unsigned char)); int min_size= s1size > s2size?
s2size: s1size; int bound= min_size > n? n: min_size;{ int i= 0; for( 0; i <
bound; i ++){ if(*(( const unsigned char*) _check_unknown_subscript( s1, sizeof(
unsigned char), i)) <*(( const unsigned char*) _check_unknown_subscript( s2,
sizeof( unsigned char), i))){ return - 1;} else{ if(*(( const unsigned char*)
_check_unknown_subscript( s2, sizeof( unsigned char), i)) <*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i))){
return 1;}}}} if( min_size <= bound){ return 0;} if( s1size < s2size){ return -
1;} else{ return 1;}}} int Cyc_String_zstrptrcmp( struct _tagged_arr* a, struct
_tagged_arr* b){ return Cyc_String_zstrcmp(* a,* b);} static struct _tagged_arr
Cyc_String_int_strcato( struct _tagged_arr dest, struct _tagged_arr src, int
srcOfs, struct _tagged_arr error){ int i; int dsize; int slen; int dlen; dsize=(
int) _get_arr_size( dest, sizeof( unsigned char)); dlen= Cyc_String_strlen((
struct _tagged_arr) dest); slen= Cyc_String_int_strleno( src, srcOfs, error);
if( slen + dlen <= dsize){ for( i= 0; i < slen; i ++){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char), i + dlen))=*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), i +
srcOfs));} if( i != dsize){*(( unsigned char*) _check_unknown_subscript( dest,
sizeof( unsigned char), i + dlen))='\000';}} else{( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp2=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp3; _temp3.tag= Cyc_Core_InvalidArg; _temp3.f1= error; _temp3;}); _temp2;}));}
return dest;} struct _tagged_arr Cyc_String_strcat( struct _tagged_arr dest,
struct _tagged_arr src){ return Cyc_String_int_strcato( dest, src, 0, _tag_arr("String::strcat",
sizeof( unsigned char), 15u));} struct _tagged_arr Cyc_String_rstrconcat( struct
_RegionHandle* r, struct _tagged_arr a, struct _tagged_arr b){ int _temp4= Cyc_String_strlen(
a); int _temp5= Cyc_String_strlen( b); struct _tagged_arr ans= Cyc_Core_rnew_string(
r,( _temp4 + _temp5) + 1); int i; int j; for( i= 0; i < _temp4; ++ i){*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i))=*((
const unsigned char*) _check_unknown_subscript( a, sizeof( unsigned char), i));}
for( j= 0; j < _temp5; ++ j){*(( unsigned char*) _check_unknown_subscript( ans,
sizeof( unsigned char), i + j))=*(( const unsigned char*)
_check_unknown_subscript( b, sizeof( unsigned char), j));} return ans;} struct
_tagged_arr Cyc_String_strconcat( struct _tagged_arr a, struct _tagged_arr b){
return Cyc_String_rstrconcat( Cyc_Core_heap_region, a, b);} struct _tagged_arr
Cyc_String_rstrconcat_l( struct _RegionHandle* r, struct Cyc_List_List* strs){
int len; int total_len= 0; struct _tagged_arr ans;{ struct _RegionHandle _temp6=
_new_region(); struct _RegionHandle* temp=& _temp6; _push_region( temp);{ struct
Cyc_List_List* lens=({ struct Cyc_List_List* _temp9=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp9->hd=( void*)(( int)
0); _temp9->tl= 0; _temp9;}); struct Cyc_List_List* end= lens;{ struct Cyc_List_List*
p= strs; for( 0; p != 0; p=(( struct Cyc_List_List*) _check_null( p))->tl){ len=
Cyc_String_strlen(*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
p))->hd)); total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({
struct Cyc_List_List* _temp7=( struct Cyc_List_List*) _region_malloc( temp,
sizeof( struct Cyc_List_List)); _temp7->hd=( void*) len; _temp7->tl= 0; _temp7;});
end=(( struct Cyc_List_List*) _check_null( end))->tl;}} lens=(( struct Cyc_List_List*)
_check_null( lens))->tl; ans= Cyc_Core_rnew_string( r, total_len + 1);{ int i= 0;
while( strs != 0) { struct _tagged_arr _temp8=*(( struct _tagged_arr*)(( struct
Cyc_List_List*) _check_null( strs))->hd); len=( int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_String_strncpy( _tagged_arr_plus( ans, sizeof(
unsigned char), i), _temp8, len); i += len; strs=(( struct Cyc_List_List*)
_check_null( strs))->tl; lens=(( struct Cyc_List_List*) _check_null( lens))->tl;}}};
_pop_region( temp);} return ans;} struct _tagged_arr Cyc_String_strconcat_l(
struct Cyc_List_List* strs){ return Cyc_String_rstrconcat_l( Cyc_Core_heap_region,
strs);} struct _tagged_arr Cyc_String_rstr_sepstr( struct _RegionHandle* r,
struct Cyc_List_List* strs, struct _tagged_arr separator){ if( strs == 0){
return Cyc_Core_rnew_string( r, 0);} if((( struct Cyc_List_List*) _check_null(
strs))->tl == 0){ return Cyc_String_rstrdup( r,*(( struct _tagged_arr*)(( struct
Cyc_List_List*) _check_null( strs))->hd));}{ struct Cyc_List_List* _temp10= strs;
struct _RegionHandle _temp11= _new_region(); struct _RegionHandle* temp=&
_temp11; _push_region( temp);{ struct Cyc_List_List* lens=({ struct Cyc_List_List*
_temp15=( struct Cyc_List_List*) _region_malloc( temp, sizeof( struct Cyc_List_List));
_temp15->hd=( void*)(( int) 0); _temp15->tl= 0; _temp15;}); struct Cyc_List_List*
end= lens; int len= 0; int total_len= 0; int list_len= 0; for( 0; _temp10 != 0;
_temp10=(( struct Cyc_List_List*) _check_null( _temp10))->tl){ len= Cyc_String_strlen(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp10))->hd));
total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) _region_malloc( temp, sizeof( struct Cyc_List_List));
_temp12->hd=( void*) len; _temp12->tl= 0; _temp12;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl; ++ list_len;} lens=(( struct Cyc_List_List*) _check_null(
lens))->tl;{ int seplen= Cyc_String_strlen( separator); total_len +=( list_len -
1) * seplen;{ struct _tagged_arr ans= Cyc_Core_rnew_string( r, total_len); int i=
0; while((( struct Cyc_List_List*) _check_null( strs))->tl != 0) { struct
_tagged_arr _temp13=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( strs))->hd); len=( int)(( struct Cyc_List_List*) _check_null( lens))->hd;
Cyc_String_strncpy( _tagged_arr_plus( ans, sizeof( unsigned char), i), _temp13,
len); i += len; Cyc_String_strncpy( _tagged_arr_plus( ans, sizeof( unsigned char),
i), separator, seplen); i += seplen; strs=(( struct Cyc_List_List*) _check_null(
strs))->tl; lens=(( struct Cyc_List_List*) _check_null( lens))->tl;} Cyc_String_strncpy(
_tagged_arr_plus( ans, sizeof( unsigned char), i),*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( strs))->hd),( int)(( struct Cyc_List_List*)
_check_null( lens))->hd);{ struct _tagged_arr _temp14= ans; _npop_handler( 0u);
return _temp14;}}}}; _pop_region( temp);}} struct _tagged_arr Cyc_String_str_sepstr(
struct Cyc_List_List* strs, struct _tagged_arr separator){ return Cyc_String_rstr_sepstr(
Cyc_Core_heap_region, strs, separator);} struct _tagged_arr Cyc_String_strncpy(
struct _tagged_arr dest, struct _tagged_arr src, int n){ unsigned int _temp16=
_get_arr_size( dest, sizeof( unsigned char)); unsigned int _temp17=
_get_arr_size( src, sizeof( unsigned char)); if( n <= 0){ return dest;} if( n >
_temp16? 1: n > _temp17){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp18=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp18[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp19; _temp19.tag= Cyc_Core_InvalidArg;
_temp19.f1= _tag_arr("String::strncpy", sizeof( unsigned char), 16u); _temp19;});
_temp18;}));}{ int i; for( i= 0; i < n; i ++){ unsigned char _temp20=*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), i)); if(
_temp20 =='\000'){ break;}*(( unsigned char*) _check_unknown_subscript( dest,
sizeof( unsigned char), i))= _temp20;} for( 0; i < n; i ++){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char), i))='\000';} return dest;}}
struct _tagged_arr Cyc_String_zstrncpy( struct _tagged_arr dest, struct
_tagged_arr src, int n){ unsigned int _temp21= _get_arr_size( dest, sizeof(
unsigned char)); unsigned int _temp22= _get_arr_size( src, sizeof( unsigned char));
if( n <= 0){ return dest;} if( n > _temp21? 1: n > _temp22){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp23=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp23[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp24; _temp24.tag= Cyc_Core_InvalidArg; _temp24.f1=
_tag_arr("String::zstrncpy", sizeof( unsigned char), 17u); _temp24;}); _temp23;}));}{
int i; for( i= 0; i < n; i ++){*(( unsigned char*) _check_unknown_subscript(
dest, sizeof( unsigned char), i))=*(( const unsigned char*)
_check_unknown_subscript( src, sizeof( unsigned char), i));} return dest;}}
struct _tagged_arr Cyc_String_strcpy( struct _tagged_arr dest, struct
_tagged_arr src){ int len= Cyc_String_strlen( src); Cyc_String_strncpy( dest,
src, len); if( len != _get_arr_size( dest, sizeof( unsigned char))){*((
unsigned char*) _check_unknown_subscript( dest, sizeof( unsigned char), len))='\000';}
return dest;} struct _tagged_arr Cyc_String_rstrdup( struct _RegionHandle* r,
struct _tagged_arr src){ int len; struct _tagged_arr temp; len= Cyc_String_strlen(
src); temp= Cyc_Core_rnew_string( r, len + 1); Cyc_String_strncpy( temp, src,
len); return temp;} struct _tagged_arr Cyc_String_strdup( struct _tagged_arr src){
return Cyc_String_rstrdup( Cyc_Core_heap_region, src);} struct _tagged_arr Cyc_String_rexpand(
struct _RegionHandle* r, struct _tagged_arr s, int sz){ struct _tagged_arr temp;
int slen; slen= Cyc_String_strlen( s); sz= sz > slen? sz: slen; temp= Cyc_Core_rnew_string(
r, sz); Cyc_String_strncpy( temp, s, slen); if( slen != _get_arr_size( s,
sizeof( unsigned char))){*(( unsigned char*) _check_unknown_subscript( temp,
sizeof( unsigned char), slen))='\000';} return temp;} struct _tagged_arr Cyc_String_expand(
struct _tagged_arr s, int sz){ return Cyc_String_rexpand( Cyc_Core_heap_region,
s, sz);} struct _tagged_arr Cyc_String_rrealloc_str( struct _RegionHandle* r,
struct _tagged_arr str, int sz){ int maxsizeP=( int) _get_arr_size( str, sizeof(
unsigned char)); if( maxsizeP == 0){ maxsizeP= 30 > sz? 30: sz; str= Cyc_Core_rnew_string(
r, maxsizeP);*(( unsigned char*) _check_unknown_subscript( str, sizeof(
unsigned char), 0))='\000';} else{ if( sz > maxsizeP){ maxsizeP= maxsizeP * 2 >(
sz * 5) / 4? maxsizeP * 2:( sz * 5) / 4; str= Cyc_String_rexpand( r,( struct
_tagged_arr) str, maxsizeP);}} return str;} struct _tagged_arr Cyc_String_realloc_str(
struct _tagged_arr str, int sz){ return Cyc_String_rrealloc_str( Cyc_Core_heap_region,
str, sz);} struct _tagged_arr Cyc_String_rsubstring( struct _RegionHandle* r,
struct _tagged_arr s, int start, int amt){ if(( start < 0? 1: amt < 0)? 1:(
start +( int) amt) - 1 >( int) _get_arr_size( s, sizeof( unsigned char))){( int)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp25=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp25[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp26; _temp26.tag= Cyc_Core_InvalidArg; _temp26.f1=
_tag_arr("String::substring", sizeof( unsigned char), 18u); _temp26;}); _temp25;}));}{
struct _tagged_arr ans= Cyc_Core_rnew_string( r, amt + 1);{ int i= 0; for( 0; i
< amt; ++ i){*(( unsigned char*) _check_unknown_subscript( ans, sizeof(
unsigned char), i))=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), start + i));}}*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), amt))='\000'; return ans;}}
struct _tagged_arr Cyc_String_substring( struct _tagged_arr s, int start, int
amt){ return Cyc_String_rsubstring( Cyc_Core_heap_region, s, start, amt);}
struct _tagged_arr Cyc_String_rreplace_suffix( struct _RegionHandle* r, struct
_tagged_arr src, struct _tagged_arr curr_suffix, struct _tagged_arr new_suffix){
int m=( int) _get_arr_size( src, sizeof( unsigned char)); int n=( int)
_get_arr_size( curr_suffix, sizeof( unsigned char)); struct _tagged_arr err=
_tag_arr("String::replace_suffix", sizeof( unsigned char), 23u); if( m < n){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp27=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp27[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp28; _temp28.tag= Cyc_Core_InvalidArg; _temp28.f1=
err; _temp28;}); _temp27;}));}{ int i= 1; for( 0; i <= n; ++ i){ if(*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), m - i))
!=*(( const unsigned char*) _check_unknown_subscript( curr_suffix, sizeof(
unsigned char), n - i))){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp29=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp29[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp30; _temp30.tag= Cyc_Core_InvalidArg;
_temp30.f1= err; _temp30;}); _temp29;}));}}}{ struct _tagged_arr ans= Cyc_Core_rnew_string(
r,( int)(( m - n) + _get_arr_size( new_suffix, sizeof( unsigned char)))); Cyc_String_strncpy(
ans, src, m - n); Cyc_String_strncpy( _tagged_arr_plus( _tagged_arr_plus( ans,
sizeof( unsigned char), m), sizeof( unsigned char), - n), new_suffix,( int)
_get_arr_size( new_suffix, sizeof( unsigned char))); return ans;}} struct
_tagged_arr Cyc_String_replace_suffix( struct _tagged_arr src, struct
_tagged_arr curr_suffix, struct _tagged_arr new_suffix){ return Cyc_String_rreplace_suffix(
Cyc_Core_heap_region, src, curr_suffix, new_suffix);} struct _tagged_arr Cyc_String_strpbrk(
struct _tagged_arr s, struct _tagged_arr accept){ int len= Cyc_String_int_strleno((
struct _tagged_arr) s, 0, _tag_arr("String::strpbrk", sizeof( unsigned char), 16u));
unsigned char c='\000'; for( 0; len > 0?( c=*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u))) != 0: 0;( len --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1))){ int j= 0; for(
0; j < _get_arr_size( accept, sizeof( unsigned char)); j ++){ if( c ==*(( const
unsigned char*) _check_unknown_subscript( accept, sizeof( unsigned char), j))){
return s;}}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_String_strchr(
struct _tagged_arr s, unsigned char c){ int len= Cyc_String_int_strleno( s, 0,
_tag_arr("String::strchr", sizeof( unsigned char), 15u)); unsigned char c2='\000';
for( 0; len > 0?( c2=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0u))) != 0: 0;( len --, _tagged_arr_inplace_plus_post(&
s, sizeof( unsigned char), 1))){ if( c2 == c){ return s;}} return( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_String_strrchr(
struct _tagged_arr s, unsigned char c){ int len= Cyc_String_int_strleno(( struct
_tagged_arr) s, 0, _tag_arr("String::strrchr", sizeof( unsigned char), 16u));
int i= len - 1; _tagged_arr_inplace_plus(& s, sizeof( unsigned char), i); for( 0;
i >= 0;( i --, _tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), -1))){
if(*(( unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0u))
== c){ return s;}} return _tag_arr( 0u, 0u, 0u);} int Cyc_String_strspn( struct
_tagged_arr s, struct _tagged_arr accept){ int len= Cyc_String_int_strleno( s, 0,
_tag_arr("String::strspn", sizeof( unsigned char), 15u));{ int i= 0; for( 0; i <
len; i ++){ int j; for( j= 0; j < _get_arr_size( accept, sizeof( unsigned char));
j ++){ if(*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) ==*(( const unsigned char*) _check_unknown_subscript( accept,
sizeof( unsigned char), j))){ break;}} if( j == _get_arr_size( accept, sizeof(
unsigned char))){ return i;}}} return len;} struct Cyc_List_List* Cyc_String_rexplode(
struct _RegionHandle* r, struct _tagged_arr s){ struct Cyc_List_List* result= 0;{
int i= Cyc_String_strlen( s) - 1; for( 0; i >= 0; i --){ result=({ struct Cyc_List_List*
_temp31=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp31->hd=( void*)(( int)*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i))); _temp31->tl= result; _temp31;});}} return result;}
struct Cyc_List_List* Cyc_String_explode( struct _tagged_arr s){ return Cyc_String_rexplode(
Cyc_Core_heap_region, s);} struct _tagged_arr Cyc_String_implode( struct Cyc_List_List*
chars){ struct _tagged_arr s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); int i= 0; while( chars != 0) {*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i ++))=( unsigned char)((
int)(( struct Cyc_List_List*) _check_null( chars))->hd); chars=(( struct Cyc_List_List*)
_check_null( chars))->tl;} return s;} int Cyc_String_to_int( struct _tagged_arr
s, int* offset){ int ans= 0; int sn= 1; int digit_seen= 0; int i= offset == 0? 0:*((
int*) _check_null( offset)); while( i < _get_arr_size( s, sizeof( unsigned char))?
isspace(( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i))): 0) { i ++;} if( i < _get_arr_size( s, sizeof(
unsigned char))?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) =='-': 0){ i ++; sn= - 1;} while( i < _get_arr_size( s,
sizeof( unsigned char))? isspace(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { i ++;} while( i
< _get_arr_size( s, sizeof( unsigned char))? isdigit(( int)*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))): 0) {
digit_seen= 1; ans= ans * 10 +( int)(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) -'0'); i ++;} ans= ans
* sn; if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp32=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp32[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp33; _temp33.tag= Cyc_Core_InvalidArg;
_temp33.f1= _tag_arr("String::to_int", sizeof( unsigned char), 15u); _temp33;});
_temp32;}));} if( offset != 0){*(( int*) _check_null( offset))= i;} return ans;}
double Cyc_String_to_double( struct _tagged_arr s, int* offset){ double ans=(
double) 0.0; int sn= 1; int digit_seen= 0; int i= offset == 0? 0:*(( int*)
_check_null( offset)); while( i < _get_arr_size( s, sizeof( unsigned char))?
isspace(( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i))): 0) { i ++;} if( i < _get_arr_size( s, sizeof(
unsigned char))?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) =='-': 0){ i ++; sn= - 1;} while( i < _get_arr_size( s,
sizeof( unsigned char))? isdigit(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { digit_seen= 1;
ans= ans * 10.0 +( double)(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) -'0'); i ++;} if( i < _get_arr_size( s, sizeof(
unsigned char))?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) =='.': 0){ i ++;{ double divisor=( double) 0.1; while( i <
_get_arr_size( s, sizeof( unsigned char))? isdigit(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { digit_seen= 1;
ans= ans +( double)(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) -'0') * divisor; divisor= divisor / 10.0; i ++;}}}
if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp34=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp34[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp35; _temp35.tag= Cyc_Core_InvalidArg;
_temp35.f1= _tag_arr("String::to_double", sizeof( unsigned char), 18u); _temp35;});
_temp34;}));} if( i < _get_arr_size( s, sizeof( unsigned char))?*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) =='e'?
1:*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)) =='E': 0){ i ++;{ int exponent= 0; int exp_sign= 1; digit_seen= 0; if( i <
_get_arr_size( s, sizeof( unsigned char))?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) =='-': 0){ i ++;
exp_sign= - 1;} else{ if( i < _get_arr_size( s, sizeof( unsigned char))?*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
=='+': 0){ i ++;}} while( i < _get_arr_size( s, sizeof( unsigned char))? isdigit((
int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i))): 0) { digit_seen= 1; exponent= exponent * 10 +( int)(*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) -'0');
i ++;} if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp36=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp36[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp37; _temp37.tag= Cyc_Core_InvalidArg;
_temp37.f1= _tag_arr("String::to_double", sizeof( unsigned char), 18u); _temp37;});
_temp36;}));} ans= ans * pow(( double) 10.0,( double)( exponent * exp_sign));}}
if( offset != 0){*(( int*) _check_null( offset))= i;} return ans * sn;} int Cyc_Strings_strcasecmp(
struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr == s2.curr){ return 0;}{
int len1= Cyc_String_int_strleno( s1, 0, _tag_arr("Strings::strcasecmp", sizeof(
unsigned char), 20u)); int len2= Cyc_String_int_strleno( s2, 0, _tag_arr("Strings::strcasecmp",
sizeof( unsigned char), 20u)); return Cyc_String_cmp( s1, 0, len1, s2, 0, len2,
Cyc_String_case_cmp);}} static int Cyc_Strings_nocase_cmp( unsigned char c1,
unsigned char c2){ return Cyc_String_case_cmp(( unsigned char) toupper(( int) c1),(
unsigned char) toupper(( int) c2));} int Cyc_Strings_strncasecmp( struct
_tagged_arr s1, struct _tagged_arr s2, int n){ int len1= Cyc_String_int_strleno(
s1, 0, _tag_arr("Strings::strncasecmp", sizeof( unsigned char), 21u)); int len2=
Cyc_String_int_strleno( s2, 0, _tag_arr("Strings::strncasecmp", sizeof(
unsigned char), 21u)); return Cyc_String_ncmp( s1, 0, len1, s2, 0, len2, n, Cyc_Strings_nocase_cmp);}
