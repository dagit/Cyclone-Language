#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
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
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern struct
_tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*); extern struct
_tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
unsigned int n); struct _tagged_string Cyc_Filename_concat( struct
_tagged_string s1, struct _tagged_string s2){ return Cyc_String_strconcat_l(({
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
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp8=
filename;( unsigned int)( _temp8.last_plus_one - _temp8.curr);}) - 1); while( i
>= 0?({ struct _tagged_string _temp9= filename; char* _temp11= _temp9.curr + i;
if( _temp9.base == 0? 1:( _temp11 < _temp9.base? 1: _temp11 >= _temp9.last_plus_one)){
_throw( Null_Exception);}* _temp11;}) !='.': 0) { -- i;} if( i < 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp12=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp12[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp13; _temp13.tag= Cyc_Core_InvalidArg_tag;
_temp13.f1=( struct _tagged_string)({ char* _temp14=( char*)"chop_extension";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 15; _temp15;}); _temp13;}); _temp12;}));}
return Cyc_String_substring( filename, 0,( unsigned int) i);} struct
_tagged_string Cyc_Filename_dirname( struct _tagged_string filename){ int i=(
int)(({ struct _tagged_string _temp16= filename;( unsigned int)( _temp16.last_plus_one
- _temp16.curr);}) - 1); while( i >= 0?({ struct _tagged_string _temp17=
filename; char* _temp19= _temp17.curr + i; if( _temp17.base == 0? 1:( _temp19 <
_temp17.base? 1: _temp19 >= _temp17.last_plus_one)){ _throw( Null_Exception);}*
_temp19;}) !='/': 0) { -- i;} if( i < 0){ return( struct _tagged_string)({ char*
_temp20=( char*)""; struct _tagged_string _temp21; _temp21.curr= _temp20;
_temp21.base= _temp20; _temp21.last_plus_one= _temp20 + 1; _temp21;});} return
Cyc_String_substring( filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_basename(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp22=
filename;( unsigned int)( _temp22.last_plus_one - _temp22.curr);}) - 1); while(
i >= 0?({ struct _tagged_string _temp23= filename; char* _temp25= _temp23.curr +
i; if( _temp23.base == 0? 1:( _temp25 < _temp23.base? 1: _temp25 >= _temp23.last_plus_one)){
_throw( Null_Exception);}* _temp25;}) !='/': 0) { -- i;} return Cyc_String_substring(
filename, i + 1,({ struct _tagged_string _temp26= filename;( unsigned int)(
_temp26.last_plus_one - _temp26.curr);}) -( i + 1));} int Cyc_Filename_check_suffix(
struct _tagged_string filename, struct _tagged_string suffix){ int i=( int)(({
struct _tagged_string _temp27= filename;( unsigned int)( _temp27.last_plus_one -
_temp27.curr);}) - 1); int j=( int)(({ struct _tagged_string _temp28= suffix;(
unsigned int)( _temp28.last_plus_one - _temp28.curr);}) - 1); while( i >= 0? j
>= 0: 0) { if(({ struct _tagged_string _temp29= filename; char* _temp31= _temp29.curr
+( i --); if( _temp29.base == 0? 1:( _temp31 < _temp29.base? 1: _temp31 >=
_temp29.last_plus_one)){ _throw( Null_Exception);}* _temp31;}) !=({ struct
_tagged_string _temp32= suffix; char* _temp34= _temp32.curr +( j --); if(
_temp32.base == 0? 1:( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one)){
_throw( Null_Exception);}* _temp34;})){ return 0;}} if( j >= 0){ return 0;}
else{ return 1;}} struct _tagged_string Cyc_Filename_gnuify( struct
_tagged_string filename){ int has_drive_name=({ struct _tagged_string _temp35=
filename;( unsigned int)( _temp35.last_plus_one - _temp35.curr);}) > 1?({ struct
_tagged_string _temp36= filename; char* _temp38= _temp36.curr + 1; if( _temp36.base
== 0? 1:( _temp38 < _temp36.base? 1: _temp38 >= _temp36.last_plus_one)){ _throw(
Null_Exception);}* _temp38;}) ==':': 0; int i; int j; struct _tagged_string ans;
int ans_sz; if( has_drive_name){ ans_sz=( int)(({ struct _tagged_string _temp39=
filename;( unsigned int)( _temp39.last_plus_one - _temp39.curr);}) + 1); ans=({
unsigned int _temp40=( unsigned int) ans_sz; char* _temp41=( char*)
GC_malloc_atomic( sizeof( char) * _temp40); unsigned int k; struct
_tagged_string _temp42={ _temp41, _temp41, _temp41 + _temp40}; for( k= 0; k <
_temp40; k ++){ _temp41[ k]='\000';} _temp42;});({ struct _tagged_string _temp43=
ans; char* _temp45= _temp43.curr + 0; if( _temp43.base == 0? 1:( _temp45 <
_temp43.base? 1: _temp45 >= _temp43.last_plus_one)){ _throw( Null_Exception);}*
_temp45=({ struct _tagged_string _temp46= ans; char* _temp48= _temp46.curr + 1;
if( _temp46.base == 0? 1:( _temp48 < _temp46.base? 1: _temp48 >= _temp46.last_plus_one)){
_throw( Null_Exception);}* _temp48='/';});});({ struct _tagged_string _temp49=
ans; char* _temp51= _temp49.curr + 2; if( _temp49.base == 0? 1:( _temp51 <
_temp49.base? 1: _temp51 >= _temp49.last_plus_one)){ _throw( Null_Exception);}*
_temp51=({ struct _tagged_string _temp52= filename; char* _temp54= _temp52.curr
+ 0; if( _temp52.base == 0? 1:( _temp54 < _temp52.base? 1: _temp54 >= _temp52.last_plus_one)){
_throw( Null_Exception);}* _temp54;});}); i= 3; j= 2;} else{ ans_sz=( int)({
struct _tagged_string _temp55= filename;( unsigned int)( _temp55.last_plus_one -
_temp55.curr);}); ans=({ unsigned int _temp56=( unsigned int) ans_sz; char*
_temp57=( char*) GC_malloc_atomic( sizeof( char) * _temp56); unsigned int k;
struct _tagged_string _temp58={ _temp57, _temp57, _temp57 + _temp56}; for( k= 0;
k < _temp56; k ++){ _temp57[ k]='\000';} _temp58;}); i= 0; j= 0;} while( i <
ans_sz) { char c=({ struct _tagged_string _temp59= filename; char* _temp61=
_temp59.curr +( j ++); if( _temp59.base == 0? 1:( _temp61 < _temp59.base? 1:
_temp61 >= _temp59.last_plus_one)){ _throw( Null_Exception);}* _temp61;});({
struct _tagged_string _temp62= ans; char* _temp64= _temp62.curr +( i ++); if(
_temp62.base == 0? 1:( _temp64 < _temp62.base? 1: _temp64 >= _temp62.last_plus_one)){
_throw( Null_Exception);}* _temp64= c =='\\'?'/': c;});} return ans;}