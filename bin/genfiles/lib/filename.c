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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
extern struct _tagged_string Cyc_Filename_concat( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string); extern struct _tagged_string Cyc_Filename_dirname(
struct _tagged_string); extern struct _tagged_string Cyc_Filename_basename(
struct _tagged_string); extern int Cyc_Filename_check_suffix( struct
_tagged_string, struct _tagged_string); extern struct _tagged_string Cyc_Filename_gnuify(
struct _tagged_string); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u];
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_String_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); struct _tagged_string Cyc_Filename_concat(
struct _tagged_string s1, struct _tagged_string s2){ return Cyc_String_strconcat_l(({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*)({ struct _tagged_string* _temp7=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp7[ 0]= s1;
_temp7;}); _temp0->tl=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_string* _temp4=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp4[ 0]=( struct _tagged_string)({ char* _temp5=( char*)"/";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 2; _temp6;}); _temp4;}); _temp1->tl=({ struct Cyc_List_List* _temp2=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2->hd=(
void*)({ struct _tagged_string* _temp3=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp3[ 0]= s2; _temp3;}); _temp2->tl= 0;
_temp2;}); _temp1;}); _temp0;}));} struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp12=
filename;( unsigned int)( _temp12.last_plus_one - _temp12.curr);}) - 1); while(
i >= 0?*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( char), i) !='.': 0) { -- i;} if( i
< 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp8=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp8[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp9; _temp9.tag= Cyc_Core_InvalidArg;
_temp9.f1=( struct _tagged_string)({ char* _temp10=( char*)"chop_extension";
struct _tagged_string _temp11; _temp11.curr= _temp10; _temp11.base= _temp10;
_temp11.last_plus_one= _temp10 + 15; _temp11;}); _temp9;}); _temp8;}));} return
Cyc_String_substring( filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_dirname(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp15=
filename;( unsigned int)( _temp15.last_plus_one - _temp15.curr);}) - 1); while(
i >= 0?*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( char), i) !='/': 0) { -- i;} if( i
< 0){ return( struct _tagged_string)({ char* _temp13=( char*)""; struct
_tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one=
_temp13 + 1; _temp14;});} return Cyc_String_substring( filename, 0,(
unsigned int) i);} struct _tagged_string Cyc_Filename_basename( struct
_tagged_string filename){ int i=( int)(({ struct _tagged_string _temp17=
filename;( unsigned int)( _temp17.last_plus_one - _temp17.curr);}) - 1); while(
i >= 0?*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( char), i) !='/': 0) { -- i;} return
Cyc_String_substring( filename, i + 1,({ struct _tagged_string _temp16= filename;(
unsigned int)( _temp16.last_plus_one - _temp16.curr);}) -( i + 1));} int Cyc_Filename_check_suffix(
struct _tagged_string filename, struct _tagged_string suffix){ int i=( int)(({
struct _tagged_string _temp19= filename;( unsigned int)( _temp19.last_plus_one -
_temp19.curr);}) - 1); int j=( int)(({ struct _tagged_string _temp18= suffix;(
unsigned int)( _temp18.last_plus_one - _temp18.curr);}) - 1); while( i >= 0? j
>= 0: 0) { if(*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( filename, sizeof( char), i --) !=*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( suffix,
sizeof( char), j --)){ return 0;}} if( j >= 0){ return 0;} else{ return 1;}}
struct _tagged_string Cyc_Filename_gnuify( struct _tagged_string filename){ int
has_drive_name=({ struct _tagged_string _temp30= filename;( unsigned int)(
_temp30.last_plus_one - _temp30.curr);}) > 1?*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( filename, sizeof( char),
1) ==':': 0; int i; int j; struct _tagged_string ans; int ans_sz; if(
has_drive_name){ ans_sz=( int)(({ struct _tagged_string _temp20= filename;(
unsigned int)( _temp20.last_plus_one - _temp20.curr);}) + 1); ans=({
unsigned int _temp21=( unsigned int) ans_sz; char* _temp22=( char*)
GC_malloc_atomic( sizeof( char) * _temp21); struct _tagged_string _temp24={
_temp22, _temp22, _temp22 + _temp21};{ unsigned int _temp23= _temp21;
unsigned int k; for( k= 0; k < _temp23; k ++){ _temp22[ k]='\000';}}; _temp24;});*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( char), 0)=(*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( ans,
sizeof( char), 1)='/');*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( ans, sizeof( char), 2)=*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
filename, sizeof( char), 0); i= 3; j= 2;} else{ ans_sz=( int)({ struct
_tagged_string _temp25= filename;( unsigned int)( _temp25.last_plus_one -
_temp25.curr);}); ans=({ unsigned int _temp26=( unsigned int) ans_sz; char*
_temp27=( char*) GC_malloc_atomic( sizeof( char) * _temp26); struct
_tagged_string _temp29={ _temp27, _temp27, _temp27 + _temp26};{ unsigned int
_temp28= _temp26; unsigned int k; for( k= 0; k < _temp28; k ++){ _temp27[ k]='\000';}};
_temp29;}); i= 0; j= 0;} while( i < ans_sz) { char c=*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( filename,
sizeof( char), j ++);*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( ans, sizeof( char), i ++)= c =='\\'?'/':
c;} return ans;}