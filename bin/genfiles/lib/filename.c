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
Cstring_to_string( unsigned char*); extern int system( unsigned char*); extern
struct _tagged_string Cyc_Filename_concat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string); extern struct _tagged_string Cyc_Filename_dirname(
struct _tagged_string); extern struct _tagged_string Cyc_Filename_basename(
struct _tagged_string); extern int Cyc_Filename_check_suffix( struct
_tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_Filename_gnuify(
struct _tagged_string); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern struct _tagged_string Cyc_String_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); struct _tagged_string Cyc_Filename_concat(
struct _tagged_string s1, struct _tagged_string s2){ return Cyc_String_strconcat_l(({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*)({ struct _tagged_string* _temp7=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp7[ 0]= s1;
_temp7;}); _temp0->tl=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_string* _temp4=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp4[ 0]=({ unsigned char* _temp5=( unsigned char*)"/";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 2; _temp6;}); _temp4;}); _temp1->tl=({ struct Cyc_List_List* _temp2=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2->hd=(
void*)({ struct _tagged_string* _temp3=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp3[ 0]= s2; _temp3;}); _temp2->tl= 0;
_temp2;}); _temp1;}); _temp0;}));} struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp12=
filename;( unsigned int)( _temp12.last_plus_one - _temp12.curr);}) - 1); while(
i >= 0?*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( filename, sizeof( unsigned char), i)
!='.': 0) { -- i;} if( i < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp8=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp8[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp9; _temp9.tag= Cyc_Core_InvalidArg;
_temp9.f1=({ unsigned char* _temp10=( unsigned char*)"chop_extension"; struct
_tagged_string _temp11; _temp11.curr= _temp10; _temp11.base= _temp10; _temp11.last_plus_one=
_temp10 + 15; _temp11;}); _temp9;}); _temp8;}));} return Cyc_String_substring(
filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_dirname(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp13=
filename;( unsigned int)( _temp13.last_plus_one - _temp13.curr);}) - 1); while(
i >= 0?*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( filename, sizeof( unsigned char), i)
!='/': 0) { -- i;} if( i < 0){ return Cyc_Core_new_string( 0);} return Cyc_String_substring(
filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_basename(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp15=
filename;( unsigned int)( _temp15.last_plus_one - _temp15.curr);}) - 1); while(
i >= 0?*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( filename, sizeof( unsigned char), i)
!='/': 0) { -- i;} return Cyc_String_substring( filename, i + 1,({ struct
_tagged_string _temp14= filename;( unsigned int)( _temp14.last_plus_one -
_temp14.curr);}) -( i + 1));} int Cyc_Filename_check_suffix( struct
_tagged_string filename, struct _tagged_string suffix){ int i=( int)(({ struct
_tagged_string _temp17= filename;( unsigned int)( _temp17.last_plus_one -
_temp17.curr);}) - 1); int j=( int)(({ struct _tagged_string _temp16= suffix;(
unsigned int)( _temp16.last_plus_one - _temp16.curr);}) - 1); while( i >= 0? j
>= 0: 0) { if(*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( filename, sizeof( unsigned char), i --)
!=*(( const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( suffix, sizeof( unsigned char), j --)){ return 0;}}
if( j >= 0){ return 0;} else{ return 1;}} struct _tagged_string Cyc_Filename_gnuify(
struct _tagged_string filename){ int has_drive_name=({ struct _tagged_string
_temp28= filename;( unsigned int)( _temp28.last_plus_one - _temp28.curr);}) > 1?*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( unsigned char), 1) ==':': 0; int i;
int j; struct _tagged_string ans; int ans_sz; if( has_drive_name){ ans_sz=( int)(({
struct _tagged_string _temp18= filename;( unsigned int)( _temp18.last_plus_one -
_temp18.curr);}) + 1); ans=({ unsigned int _temp19=( unsigned int) ans_sz;
unsigned char* _temp20=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
* _temp19); struct _tagged_string _temp22={ _temp20, _temp20, _temp20 + _temp19};{
unsigned int _temp21= _temp19; unsigned int k; for( k= 0; k < _temp21; k ++){
_temp20[ k]='\000';}}; _temp22;});*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( ans, sizeof(
unsigned char), 0)=(*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( ans, sizeof( unsigned char), 1)='/');*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char), 2)=*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( unsigned char), 0); i= 3; j= 2;}
else{ ans_sz=( int)({ struct _tagged_string _temp23= filename;( unsigned int)(
_temp23.last_plus_one - _temp23.curr);}); ans=({ unsigned int _temp24=(
unsigned int) ans_sz; unsigned char* _temp25=( unsigned char*) GC_malloc_atomic(
sizeof( unsigned char) * _temp24); struct _tagged_string _temp27={ _temp25,
_temp25, _temp25 + _temp24};{ unsigned int _temp26= _temp24; unsigned int k;
for( k= 0; k < _temp26; k ++){ _temp25[ k]='\000';}}; _temp27;}); i= 0; j= 0;}
while( i < ans_sz) { unsigned char c=*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( filename,
sizeof( unsigned char), j ++);*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( ans, sizeof(
unsigned char), i ++)= c =='\\'?'/': c;} return ans;}