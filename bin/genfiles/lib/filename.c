#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); extern struct
_tagged_string Cyc_Filename_concat( struct _tagged_string, struct _tagged_string);
extern struct _tagged_string Cyc_Filename_chop_extension( struct _tagged_string);
extern struct _tagged_string Cyc_Filename_dirname( struct _tagged_string);
extern struct _tagged_string Cyc_Filename_basename( struct _tagged_string);
extern int Cyc_Filename_check_suffix( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_Filename_gnuify( struct
_tagged_string); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*);
extern struct _tagged_string Cyc_String_substring( struct _tagged_string, int
ofs, unsigned int n); struct _tagged_string Cyc_Filename_concat( struct
_tagged_string s1, struct _tagged_string s2){ return Cyc_String_strconcat_l(({
struct Cyc_List_List* _temp0=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp0->hd=( void*)({ struct _tagged_string* _temp7=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7[ 0]= s1;
_temp7;}); _temp0->tl=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_string* _temp4=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4[ 0]=( struct _tagged_string)({ char* _temp5=( char*)"/";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 2; _temp6;}); _temp4;}); _temp1->tl=({ struct Cyc_List_List* _temp2=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2->hd=(
void*)({ struct _tagged_string* _temp3=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string) * 1); _temp3[ 0]= s2; _temp3;}); _temp2->tl= 0;
_temp2;}); _temp1;}); _temp0;}));} struct _tagged_string Cyc_Filename_chop_extension(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp8=
filename;( unsigned int)( _temp8.last_plus_one - _temp8.curr);}) -( unsigned int)
1); while( i >= 0?( int)({ struct _tagged_string _temp9= filename; char* _temp11=
_temp9.curr + i; if( _temp11 < _temp9.base? 1: _temp11 >= _temp9.last_plus_one){
_throw( Null_Exception);}* _temp11;}) !=( int)'.': 0) { -- i;} if( i < 0){( void)
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp12=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp12=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp13=( char*)"chop_extension";
struct _tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13;
_temp14.last_plus_one= _temp13 + 15; _temp14;})};( struct _xenum_struct*)
_temp12;}));} return Cyc_String_substring( filename, 0,( unsigned int) i);}
struct _tagged_string Cyc_Filename_dirname( struct _tagged_string filename){ int
i=( int)(({ struct _tagged_string _temp15= filename;( unsigned int)( _temp15.last_plus_one
- _temp15.curr);}) -( unsigned int) 1); while( i >= 0?( int)({ struct
_tagged_string _temp16= filename; char* _temp18= _temp16.curr + i; if( _temp18 <
_temp16.base? 1: _temp18 >= _temp16.last_plus_one){ _throw( Null_Exception);}*
_temp18;}) !=( int)'/': 0) { -- i;} if( i < 0){ return( struct _tagged_string)({
char* _temp19=( char*)""; struct _tagged_string _temp20; _temp20.curr= _temp19;
_temp20.base= _temp19; _temp20.last_plus_one= _temp19 + 1; _temp20;});} return
Cyc_String_substring( filename, 0,( unsigned int) i);} struct _tagged_string Cyc_Filename_basename(
struct _tagged_string filename){ int i=( int)(({ struct _tagged_string _temp21=
filename;( unsigned int)( _temp21.last_plus_one - _temp21.curr);}) -(
unsigned int) 1); while( i >= 0?( int)({ struct _tagged_string _temp22= filename;
char* _temp24= _temp22.curr + i; if( _temp24 < _temp22.base? 1: _temp24 >=
_temp22.last_plus_one){ _throw( Null_Exception);}* _temp24;}) !=( int)'/': 0) {
-- i;} return Cyc_String_substring( filename, i + 1,({ struct _tagged_string
_temp25= filename;( unsigned int)( _temp25.last_plus_one - _temp25.curr);}) -(
unsigned int)( i + 1));} int Cyc_Filename_check_suffix( struct _tagged_string
filename, struct _tagged_string suffix){ int i=( int)(({ struct _tagged_string
_temp26= filename;( unsigned int)( _temp26.last_plus_one - _temp26.curr);}) -(
unsigned int) 1); int j=( int)(({ struct _tagged_string _temp27= suffix;(
unsigned int)( _temp27.last_plus_one - _temp27.curr);}) -( unsigned int) 1);
while( i >= 0? j >= 0: 0) { if(( int)({ struct _tagged_string _temp28= filename;
char* _temp30= _temp28.curr +( i --); if( _temp30 < _temp28.base? 1: _temp30 >=
_temp28.last_plus_one){ _throw( Null_Exception);}* _temp30;}) !=( int)({ struct
_tagged_string _temp31= suffix; char* _temp33= _temp31.curr +( j --); if(
_temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one){ _throw(
Null_Exception);}* _temp33;})){ return 0;}} if( j >= 0){ return 0;} else{ return
1;}} struct _tagged_string Cyc_Filename_gnuify( struct _tagged_string filename){
int has_drive_name=({ struct _tagged_string _temp34= filename;( unsigned int)(
_temp34.last_plus_one - _temp34.curr);}) >( unsigned int) 1?( int)({ struct
_tagged_string _temp35= filename; char* _temp37= _temp35.curr + 1; if( _temp37 <
_temp35.base? 1: _temp37 >= _temp35.last_plus_one){ _throw( Null_Exception);}*
_temp37;}) ==( int)':': 0; int i; int j; struct _tagged_string ans; int ans_sz;
if( has_drive_name){ ans_sz=( int)(({ struct _tagged_string _temp38= filename;(
unsigned int)( _temp38.last_plus_one - _temp38.curr);}) +( unsigned int) 1); ans=({
unsigned int _temp39=( unsigned int) ans_sz; char* _temp40=( char*)
GC_malloc_atomic( sizeof( char) * _temp39); unsigned int k; struct
_tagged_string _temp41={ _temp40, _temp40, _temp40 + _temp39}; for( k= 0; k <
_temp39; k ++){ _temp40[ k]='\000';} _temp41;});({ struct _tagged_string _temp42=
ans; char* _temp44= _temp42.curr + 0; if( _temp44 < _temp42.base? 1: _temp44 >=
_temp42.last_plus_one){ _throw( Null_Exception);}* _temp44=({ struct
_tagged_string _temp45= ans; char* _temp47= _temp45.curr + 1; if( _temp47 <
_temp45.base? 1: _temp47 >= _temp45.last_plus_one){ _throw( Null_Exception);}*
_temp47='/';});});({ struct _tagged_string _temp48= ans; char* _temp50= _temp48.curr
+ 2; if( _temp50 < _temp48.base? 1: _temp50 >= _temp48.last_plus_one){ _throw(
Null_Exception);}* _temp50=({ struct _tagged_string _temp51= filename; char*
_temp53= _temp51.curr + 0; if( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one){
_throw( Null_Exception);}* _temp53;});}); i= 3; j= 2;} else{ ans_sz=( int)({
struct _tagged_string _temp54= filename;( unsigned int)( _temp54.last_plus_one -
_temp54.curr);}); ans=({ unsigned int _temp55=( unsigned int) ans_sz; char*
_temp56=( char*) GC_malloc_atomic( sizeof( char) * _temp55); unsigned int k;
struct _tagged_string _temp57={ _temp56, _temp56, _temp56 + _temp55}; for( k= 0;
k < _temp55; k ++){ _temp56[ k]='\000';} _temp57;}); i= 0; j= 0;} while( i <
ans_sz) { char c=({ struct _tagged_string _temp58= filename; char* _temp60=
_temp58.curr +( j ++); if( _temp60 < _temp58.base? 1: _temp60 >= _temp58.last_plus_one){
_throw( Null_Exception);}* _temp60;});({ struct _tagged_string _temp61= ans;
char* _temp63= _temp61.curr +( i ++); if( _temp63 < _temp61.base? 1: _temp63 >=
_temp61.last_plus_one){ _throw( Null_Exception);}* _temp63=( int) c ==( int)'\\'?'/':
c;});} return ans;}