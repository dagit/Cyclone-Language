 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern
struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct _tagged_arr);
extern struct _tagged_arr Cyc_Filename_chop_extension( struct _tagged_arr);
extern struct _tagged_arr Cyc_Filename_dirname( struct _tagged_arr); extern
struct _tagged_arr Cyc_Filename_basename( struct _tagged_arr); extern int Cyc_Filename_check_suffix(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_Filename_gnuify(
struct _tagged_arr); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern struct _tagged_arr Cyc_std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct _tagged_arr Cyc_Filename_concat(
struct _tagged_arr s1, struct _tagged_arr s2){ return Cyc_std_strconcat( s1,(
struct _tagged_arr) Cyc_std_strconcat( _tag_arr("/", sizeof( unsigned char), 2u),
s2));} struct _tagged_arr Cyc_Filename_chop_extension( struct _tagged_arr
filename){ int i=( int)( _get_arr_size( filename, sizeof( unsigned char)) -  1);
while( i >=  0?*(( const unsigned char*) _check_unknown_subscript( filename,
sizeof( unsigned char), i)) != '.': 0) { -- i;} if( i <  0){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp0=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp1; _temp1.tag= Cyc_Core_InvalidArg; _temp1.f1= _tag_arr("chop_extension",
sizeof( unsigned char), 15u); _temp1;}); _temp0;}));} return Cyc_std_substring(
filename, 0,( unsigned int) i);} struct _tagged_arr Cyc_Filename_dirname( struct
_tagged_arr filename){ int i=( int)( _get_arr_size( filename, sizeof(
unsigned char)) -  1); while( i >=  0?*(( const unsigned char*)
_check_unknown_subscript( filename, sizeof( unsigned char), i)) != '/': 0) { --
i;} if( i <  0){ return Cyc_Core_new_string( 0);} return Cyc_std_substring(
filename, 0,( unsigned int) i);} struct _tagged_arr Cyc_Filename_basename(
struct _tagged_arr filename){ int i=( int)( _get_arr_size( filename, sizeof(
unsigned char)) -  1); while( i >=  0?*(( const unsigned char*)
_check_unknown_subscript( filename, sizeof( unsigned char), i)) != '/': 0) { --
i;} return Cyc_std_substring( filename, i +  1, _get_arr_size( filename, sizeof(
unsigned char)) - ( i +  1));} int Cyc_Filename_check_suffix( struct _tagged_arr
filename, struct _tagged_arr suffix){ int i=( int)( _get_arr_size( filename,
sizeof( unsigned char)) -  1); int j=( int)( _get_arr_size( suffix, sizeof(
unsigned char)) -  1); while( i >=  0? j >=  0: 0) { if(*(( const unsigned char*)
_check_unknown_subscript( filename, sizeof( unsigned char), i --)) != *(( const
unsigned char*) _check_unknown_subscript( suffix, sizeof( unsigned char), j --))){
return 0;}} if( j >=  0){ return 0;} else{ return 1;}} struct _tagged_arr Cyc_Filename_gnuify(
struct _tagged_arr filename){ int has_drive_name= _get_arr_size( filename,
sizeof( unsigned char)) >  1?*(( const unsigned char*) _check_unknown_subscript(
filename, sizeof( unsigned char), 1)) == ':': 0; int i; int j; struct
_tagged_arr ans; int ans_sz; if( has_drive_name){ ans_sz=( int)( _get_arr_size(
filename, sizeof( unsigned char)) +  1); ans=({ unsigned int _temp2=(
unsigned int) ans_sz; unsigned char* _temp3=( unsigned char*) GC_malloc_atomic(
sizeof( unsigned char) *  _temp2); struct _tagged_arr _temp5= _tag_arr( _temp3,
sizeof( unsigned char),( unsigned int) ans_sz);{ unsigned int _temp4= _temp2;
unsigned int k; for( k= 0; k <  _temp4; k ++){ _temp3[ k]='\000';}}; _temp5;});*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), 0))=(*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), 1))='/');*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), 2))=*((
const unsigned char*) _check_unknown_subscript( filename, sizeof( unsigned char),
0)); i= 3; j= 2;} else{ ans_sz=( int) _get_arr_size( filename, sizeof(
unsigned char)); ans=({ unsigned int _temp6=( unsigned int) ans_sz;
unsigned char* _temp7=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
*  _temp6); struct _tagged_arr _temp9= _tag_arr( _temp7, sizeof( unsigned char),(
unsigned int) ans_sz);{ unsigned int _temp8= _temp6; unsigned int k; for( k= 0;
k <  _temp8; k ++){ _temp7[ k]='\000';}}; _temp9;}); i= 0; j= 0;} while( i < 
ans_sz) { unsigned char c=*(( const unsigned char*) _check_unknown_subscript(
filename, sizeof( unsigned char), j ++));*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char), i ++))= c == '\\'?'/': c;}
return ans;}
