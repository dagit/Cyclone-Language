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
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern struct _tagged_string Cyc_String_strconcat_l(
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
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp15=
filename;( unsigned int)( _temp15.last_plus_one - _temp15.curr);}) - 1); while(
i >= 0?({ struct _tagged_string _temp8= filename; char* _temp10= _temp8.curr + i;
if( _temp8.base == 0? 1:( _temp10 < _temp8.base? 1: _temp10 >= _temp8.last_plus_one)){
_throw( Null_Exception);}* _temp10;}) !='.': 0) { -- i;} if( i < 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp11=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp11[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp12; _temp12.tag= Cyc_Core_InvalidArg; _temp12.f1=(
struct _tagged_string)({ char* _temp13=( char*)"chop_extension"; struct
_tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one=
_temp13 + 15; _temp14;}); _temp12;}); _temp11;}));} return Cyc_String_substring(
filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_dirname(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp21=
filename;( unsigned int)( _temp21.last_plus_one - _temp21.curr);}) - 1); while(
i >= 0?({ struct _tagged_string _temp16= filename; char* _temp18= _temp16.curr +
i; if( _temp16.base == 0? 1:( _temp18 < _temp16.base? 1: _temp18 >= _temp16.last_plus_one)){
_throw( Null_Exception);}* _temp18;}) !='/': 0) { -- i;} if( i < 0){ return(
struct _tagged_string)({ char* _temp19=( char*)""; struct _tagged_string _temp20;
_temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one= _temp19 + 1;
_temp20;});} return Cyc_String_substring( filename, 0,( unsigned int) i);}
struct _tagged_string Cyc_Filename_basename( struct _tagged_string filename){
int i=( int)(({ struct _tagged_string _temp26= filename;( unsigned int)( _temp26.last_plus_one
- _temp26.curr);}) - 1); while( i >= 0?({ struct _tagged_string _temp22=
filename; char* _temp24= _temp22.curr + i; if( _temp22.base == 0? 1:( _temp24 <
_temp22.base? 1: _temp24 >= _temp22.last_plus_one)){ _throw( Null_Exception);}*
_temp24;}) !='/': 0) { -- i;} return Cyc_String_substring( filename, i + 1,({
struct _tagged_string _temp25= filename;( unsigned int)( _temp25.last_plus_one -
_temp25.curr);}) -( i + 1));} int Cyc_Filename_check_suffix( struct
_tagged_string filename, struct _tagged_string suffix){ int i=( int)(({ struct
_tagged_string _temp34= filename;( unsigned int)( _temp34.last_plus_one -
_temp34.curr);}) - 1); int j=( int)(({ struct _tagged_string _temp33= suffix;(
unsigned int)( _temp33.last_plus_one - _temp33.curr);}) - 1); while( i >= 0? j
>= 0: 0) { if(({ struct _tagged_string _temp27= filename; char* _temp29= _temp27.curr
+( i --); if( _temp27.base == 0? 1:( _temp29 < _temp27.base? 1: _temp29 >=
_temp27.last_plus_one)){ _throw( Null_Exception);}* _temp29;}) !=({ struct
_tagged_string _temp30= suffix; char* _temp32= _temp30.curr +( j --); if(
_temp30.base == 0? 1:( _temp32 < _temp30.base? 1: _temp32 >= _temp30.last_plus_one)){
_throw( Null_Exception);}* _temp32;})){ return 0;}} if( j >= 0){ return 0;}
else{ return 1;}} struct _tagged_string Cyc_Filename_gnuify( struct
_tagged_string filename){ int has_drive_name=({ struct _tagged_string _temp65=
filename;( unsigned int)( _temp65.last_plus_one - _temp65.curr);}) > 1?({ struct
_tagged_string _temp66= filename; char* _temp68= _temp66.curr + 1; if( _temp66.base
== 0? 1:( _temp68 < _temp66.base? 1: _temp68 >= _temp66.last_plus_one)){ _throw(
Null_Exception);}* _temp68;}) ==':': 0; int i; int j; struct _tagged_string ans;
int ans_sz; if( has_drive_name){ ans_sz=( int)(({ struct _tagged_string _temp35=
filename;( unsigned int)( _temp35.last_plus_one - _temp35.curr);}) + 1); ans=({
unsigned int _temp36=( unsigned int) ans_sz; char* _temp37=( char*)
GC_malloc_atomic( sizeof( char) * _temp36); struct _tagged_string _temp40={
_temp37, _temp37, _temp37 + _temp36};{ unsigned int _temp38= _temp36;
unsigned int k; for( k= 0; k < _temp38; k ++){ _temp37[ k]='\000';}}; _temp40;});({
struct _tagged_string _temp41= ans; char* _temp43= _temp41.curr + 0; if( _temp41.base
== 0? 1:( _temp43 < _temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw(
Null_Exception);}* _temp43=({ struct _tagged_string _temp44= ans; char* _temp46=
_temp44.curr + 1; if( _temp44.base == 0? 1:( _temp46 < _temp44.base? 1: _temp46
>= _temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46='/';});});({
struct _tagged_string _temp47= ans; char* _temp49= _temp47.curr + 2; if( _temp47.base
== 0? 1:( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){ _throw(
Null_Exception);}* _temp49=({ struct _tagged_string _temp50= filename; char*
_temp52= _temp50.curr + 0; if( _temp50.base == 0? 1:( _temp52 < _temp50.base? 1:
_temp52 >= _temp50.last_plus_one)){ _throw( Null_Exception);}* _temp52;});}); i=
3; j= 2;} else{ ans_sz=( int)({ struct _tagged_string _temp53= filename;(
unsigned int)( _temp53.last_plus_one - _temp53.curr);}); ans=({ unsigned int
_temp54=( unsigned int) ans_sz; char* _temp55=( char*) GC_malloc_atomic( sizeof(
char) * _temp54); struct _tagged_string _temp58={ _temp55, _temp55, _temp55 +
_temp54};{ unsigned int _temp56= _temp54; unsigned int k; for( k= 0; k < _temp56;
k ++){ _temp55[ k]='\000';}}; _temp58;}); i= 0; j= 0;} while( i < ans_sz) { char
c=({ struct _tagged_string _temp62= filename; char* _temp64= _temp62.curr +( j
++); if( _temp62.base == 0? 1:( _temp64 < _temp62.base? 1: _temp64 >= _temp62.last_plus_one)){
_throw( Null_Exception);}* _temp64;});({ struct _tagged_string _temp59= ans;
char* _temp61= _temp59.curr +( i ++); if( _temp59.base == 0? 1:( _temp61 <
_temp59.base? 1: _temp61 >= _temp59.last_plus_one)){ _throw( Null_Exception);}*
_temp61= c =='\\'?'/': c;});} return ans;}