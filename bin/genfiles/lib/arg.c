#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one;}; struct _tuple0{ struct _tagged_string f1;
void* f2; struct _tagged_string f3;}; typedef unsigned int Cyc_uint; typedef
char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); extern
struct _tagged_ptr0 Cyc_Core_std_args(); struct Cyc_Core_Opt{ void* v;}; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern int Cyc_Core_int_of_string( struct
_tagged_string); extern char* string_to_Cstring( struct _tagged_string); extern
char* underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char*
tag;}; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern char Cyc_Arg_Error_tag[ 6u]; struct Cyc_Arg_Error_struct{
char* tag;}; extern char Cyc_Arg_Bad_tag[ 4u]; struct Cyc_Arg_Bad_struct{ char*
tag; struct _tagged_string f1;}; typedef void* Cyc_Arg_Spec; extern const int
Cyc_Arg_Unit_spec_tag; struct Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)();};
extern const int Cyc_Arg_Set_spec_tag; struct Cyc_Arg_Set_spec_struct{ int tag;
int* f1;}; extern const int Cyc_Arg_Clear_spec_tag; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1;}; extern const int Cyc_Arg_String_spec_tag; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string);}; extern const int Cyc_Arg_Int_spec_tag;
struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int);}; extern const int
Cyc_Arg_Rest_spec_tag; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string);}; typedef void* Cyc_Arg_gspec_t; typedef void* Cyc_Arg_spec_t;
extern void Cyc_Arg_usage( struct Cyc_List_List*, struct _tagged_string); extern
int Cyc_Arg_current; extern void Cyc_Arg_parse( struct Cyc_List_List* specs,
void(* anonfun)( struct _tagged_string), struct _tagged_string errmsg); char Cyc_Arg_Error_tag[
6u]="Error"; char Cyc_Arg_Bad_tag[ 4u]="Bad"; const int Cyc_Arg_Unit_spec_tag= 0;
const int Cyc_Arg_Set_spec_tag= 1; const int Cyc_Arg_Clear_spec_tag= 2; const
int Cyc_Arg_String_spec_tag= 3; const int Cyc_Arg_Int_spec_tag= 4; const int Cyc_Arg_Rest_spec_tag=
5; typedef void* Cyc_Arg_error; static const int Cyc_Arg_Unknown_tag= 0; struct
Cyc_Arg_Unknown_struct{ int tag; struct _tagged_string f1;}; static const int
Cyc_Arg_Missing_tag= 1; struct Cyc_Arg_Missing_struct{ int tag; struct
_tagged_string f1;}; static const int Cyc_Arg_Message_tag= 2; struct Cyc_Arg_Message_struct{
int tag; struct _tagged_string f1;}; static const int Cyc_Arg_Wrong_tag= 3;
struct Cyc_Arg_Wrong_struct{ int tag; struct _tagged_string f1; struct
_tagged_string f2; struct _tagged_string f3;}; static void* Cyc_Arg_lookup(
struct Cyc_List_List* l, struct _tagged_string x){ while( l != 0) { if( Cyc_String_strcmp(
x,(*(( struct _tuple0*) l->hd)).f1) == 0){ return(*(( struct _tuple0*) l->hd)).f2;}
l= l->tl;}( void) _throw(({ struct Cyc_Core_Not_found_struct* _temp0=( struct
Cyc_Core_Not_found_struct*) GC_malloc_atomic( sizeof( struct Cyc_Core_Not_found_struct));*
_temp0=( struct Cyc_Core_Not_found_struct){.tag= Cyc_Core_Not_found_tag};(
struct _xenum_struct*) _temp0;}));} void Cyc_Arg_usage( struct Cyc_List_List*
speclist, struct _tagged_string errmsg){({ struct _tagged_string _temp1= errmsg;
fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp1.last_plus_one - _temp1.curr, _temp1.curr);});
while( speclist != 0) {({ struct _tagged_string _temp2=(*(( struct _tuple0*)
speclist->hd)).f1; struct _tagged_string _temp3=(*(( struct _tuple0*) speclist->hd)).f3;
fprintf( Cyc_Stdio_stderr," %.*s %.*s\n", _temp2.last_plus_one - _temp2.curr,
_temp2.curr, _temp3.last_plus_one - _temp3.curr, _temp3.curr);}); speclist=
speclist->tl;}} int Cyc_Arg_current= 0; static struct _tagged_ptr0 Cyc_Arg_args={
0, 0, 0}; static void Cyc_Arg_stop( int prog_pos, void* e, struct Cyc_List_List*
speclist, struct _tagged_string errmsg){ struct _tagged_string progname=(
unsigned int) prog_pos <({ struct _tagged_ptr0 _temp4= Cyc_Arg_args;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);})?({ struct _tagged_ptr0
_temp5= Cyc_Arg_args; struct _tagged_string* _temp7= _temp5.curr + prog_pos; if(
_temp7 < _temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7;}):( struct _tagged_string)({ char* _temp8=( char*)"(?)"; struct
_tagged_string _temp9; _temp9.curr= _temp8; _temp9.base= _temp8; _temp9.last_plus_one=
_temp8 + 4; _temp9;});{ void* _temp10= e; struct _tagged_string _temp20; struct
_tagged_string _temp22; struct _tagged_string _temp24; struct _tagged_string
_temp26; struct _tagged_string _temp28; struct _tagged_string _temp30; _LL12:
if((( struct _enum_struct*) _temp10)->tag == Cyc_Arg_Unknown_tag){ _LL21:
_temp20=( struct _tagged_string)(( struct Cyc_Arg_Unknown_struct*) _temp10)->f1;
goto _LL13;} else{ goto _LL14;} _LL14: if((( struct _enum_struct*) _temp10)->tag
== Cyc_Arg_Missing_tag){ _LL23: _temp22=( struct _tagged_string)(( struct Cyc_Arg_Missing_struct*)
_temp10)->f1; goto _LL15;} else{ goto _LL16;} _LL16: if((( struct _enum_struct*)
_temp10)->tag == Cyc_Arg_Wrong_tag){ _LL29: _temp28=( struct _tagged_string)((
struct Cyc_Arg_Wrong_struct*) _temp10)->f1; goto _LL27; _LL27: _temp26=( struct
_tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp10)->f2; goto _LL25; _LL25:
_temp24=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp10)->f3;
goto _LL17;} else{ goto _LL18;} _LL18: if((( struct _enum_struct*) _temp10)->tag
== Cyc_Arg_Message_tag){ _LL31: _temp30=( struct _tagged_string)(( struct Cyc_Arg_Message_struct*)
_temp10)->f1; goto _LL19;} else{ goto _LL11;} _LL13: if( Cyc_String_strcmp(
_temp20,( struct _tagged_string)({ char* _temp32=( char*)"-help"; struct
_tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one=
_temp32 + 6; _temp33;})) != 0){({ struct _tagged_string _temp34= progname;
struct _tagged_string _temp35= _temp20; fprintf( Cyc_Stdio_stderr,"%.*s: unknown option `%.*s'.\n",
_temp34.last_plus_one - _temp34.curr, _temp34.curr, _temp35.last_plus_one -
_temp35.curr, _temp35.curr);});} goto _LL11; _LL15:({ struct _tagged_string
_temp36= progname; struct _tagged_string _temp37= _temp22; fprintf( Cyc_Stdio_stderr,"%.*s: option `%.*s' needs an argument.\n",
_temp36.last_plus_one - _temp36.curr, _temp36.curr, _temp37.last_plus_one -
_temp37.curr, _temp37.curr);}); goto _LL11; _LL17:({ struct _tagged_string
_temp38= progname; struct _tagged_string _temp39= _temp26; struct _tagged_string
_temp40= _temp28; struct _tagged_string _temp41= _temp24; fprintf( Cyc_Stdio_stderr,"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp38.last_plus_one - _temp38.curr, _temp38.curr, _temp39.last_plus_one -
_temp39.curr, _temp39.curr, _temp40.last_plus_one - _temp40.curr, _temp40.curr,
_temp41.last_plus_one - _temp41.curr, _temp41.curr);}); goto _LL11; _LL19:({
struct _tagged_string _temp42= progname; struct _tagged_string _temp43= _temp30;
fprintf( Cyc_Stdio_stderr,"%.*s: %.*s.\n", _temp42.last_plus_one - _temp42.curr,
_temp42.curr, _temp43.last_plus_one - _temp43.curr, _temp43.curr);}); goto _LL11;
_LL11:;} Cyc_Arg_usage( speclist, errmsg);( void) _throw(({ struct Cyc_Arg_Error_struct*
_temp44=( struct Cyc_Arg_Error_struct*) GC_malloc_atomic( sizeof( struct Cyc_Arg_Error_struct));*
_temp44=( struct Cyc_Arg_Error_struct){.tag= Cyc_Arg_Error_tag};( struct
_xenum_struct*) _temp44;}));} void Cyc_Arg_parse( struct Cyc_List_List* speclist,
void(* anonfun)( struct _tagged_string), struct _tagged_string errmsg){ Cyc_Arg_args=
Cyc_Core_std_args();{ int initpos= Cyc_Arg_current; int l=( int)({ struct
_tagged_ptr0 _temp45= Cyc_Arg_args;( unsigned int)( _temp45.last_plus_one -
_temp45.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current < l) { struct
_tagged_string s=({ struct _tagged_ptr0 _temp46= Cyc_Arg_args; struct
_tagged_string* _temp48= _temp46.curr + Cyc_Arg_current; if( _temp48 < _temp46.base?
1: _temp48 >= _temp46.last_plus_one){ _throw( Null_Exception);}* _temp48;}); if(({
struct _tagged_string _temp49= s;( unsigned int)( _temp49.last_plus_one -
_temp49.curr);}) >=( unsigned int) 1?( int)({ struct _tagged_string _temp50= s;
char* _temp52= _temp50.curr + 0; if( _temp52 < _temp50.base? 1: _temp52 >=
_temp50.last_plus_one){ _throw( Null_Exception);}* _temp52;}) ==( int)'-': 0){
void* action;{ struct _handler_cons _temp53; _push_handler(& _temp53);{ struct
_xenum_struct* _temp54=( struct _xenum_struct*) setjmp( _temp53.handler); if( !
_temp54){ action= Cyc_Arg_lookup( speclist, s); _pop_handler();} else{ struct
_xenum_struct* _temp56= _temp54; _LL58: if( _temp56->tag == Cyc_Core_Not_found_tag){
goto _LL59;} else{ goto _LL60;} _LL60: goto _LL61; _LL59: Cyc_Arg_stop( initpos,({
struct Cyc_Arg_Unknown_struct* _temp62=( struct Cyc_Arg_Unknown_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct)); _temp62->tag= Cyc_Arg_Unknown_tag;
_temp62->f1= s;( void*) _temp62;}), speclist, errmsg); return; _LL61:( void)
_throw( _temp56); _LL57:;}}}{ struct _handler_cons _temp63; _push_handler(&
_temp63);{ struct _xenum_struct* _temp64=( struct _xenum_struct*) setjmp(
_temp63.handler); if( ! _temp64){{ void* _temp65= action; void(* _temp79)(); int*
_temp81; int* _temp83; void(* _temp85)( struct _tagged_string); void(* _temp87)(
int); void(* _temp89)( struct _tagged_string); _LL67: if((( struct _enum_struct*)
_temp65)->tag == Cyc_Arg_Unit_spec_tag){ _LL80: _temp79=( void(*)())(( struct
Cyc_Arg_Unit_spec_struct*) _temp65)->f1; goto _LL68;} else{ goto _LL69;} _LL69:
if((( struct _enum_struct*) _temp65)->tag == Cyc_Arg_Set_spec_tag){ _LL82:
_temp81=( int*)(( struct Cyc_Arg_Set_spec_struct*) _temp65)->f1; goto _LL70;}
else{ goto _LL71;} _LL71: if((( struct _enum_struct*) _temp65)->tag == Cyc_Arg_Clear_spec_tag){
_LL84: _temp83=( int*)(( struct Cyc_Arg_Clear_spec_struct*) _temp65)->f1; goto
_LL72;} else{ goto _LL73;} _LL73: if((( struct _enum_struct*) _temp65)->tag ==
Cyc_Arg_String_spec_tag){ _LL86: _temp85=( void(*)( struct _tagged_string))((
struct Cyc_Arg_String_spec_struct*) _temp65)->f1; goto _LL74;} else{ goto _LL75;}
_LL75: if((( struct _enum_struct*) _temp65)->tag == Cyc_Arg_Int_spec_tag){ _LL88:
_temp87=( void(*)( int))(( struct Cyc_Arg_Int_spec_struct*) _temp65)->f1; goto
_LL76;} else{ goto _LL77;} _LL77: if((( struct _enum_struct*) _temp65)->tag ==
Cyc_Arg_Rest_spec_tag){ _LL90: _temp89=( void(*)( struct _tagged_string))((
struct Cyc_Arg_Rest_spec_struct*) _temp65)->f1; goto _LL78;} else{ goto _LL66;}
_LL68: _temp79(); goto _LL66; _LL70:* _temp81= 1; goto _LL66; _LL72:* _temp83= 0;
goto _LL66; _LL74: if( Cyc_Arg_current + 1 < l){ _temp85(({ struct _tagged_ptr0
_temp91= Cyc_Arg_args; struct _tagged_string* _temp93= _temp91.curr +( Cyc_Arg_current
+ 1); if( _temp93 < _temp91.base? 1: _temp93 >= _temp91.last_plus_one){ _throw(
Null_Exception);}* _temp93;})); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,({
struct Cyc_Arg_Missing_struct* _temp94=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp94->tag= Cyc_Arg_Missing_tag;
_temp94->f1= s;( void*) _temp94;}), speclist, errmsg);} goto _LL66; _LL76: {
struct _tagged_string arg=({ struct _tagged_ptr0 _temp95= Cyc_Arg_args; struct
_tagged_string* _temp97= _temp95.curr +( Cyc_Arg_current + 1); if( _temp97 <
_temp95.base? 1: _temp97 >= _temp95.last_plus_one){ _throw( Null_Exception);}*
_temp97;}); int n;{ struct _handler_cons _temp98; _push_handler(& _temp98);{
struct _xenum_struct* _temp99=( struct _xenum_struct*) setjmp( _temp98.handler);
if( ! _temp99){ n= Cyc_Core_int_of_string( arg); _pop_handler();} else{ struct
_xenum_struct* _temp101= _temp99; struct _tagged_string _temp107; _LL103: if((*((
struct _xenum_struct*) _temp101)).tag == Cyc_Core_InvalidArg_tag){ _LL108:
_temp107=(( struct Cyc_Core_InvalidArg_struct*) _temp101)->f1; goto _LL104;}
else{ goto _LL105;} _LL105: goto _LL106; _LL104: Cyc_Arg_stop( initpos,({ struct
Cyc_Arg_Wrong_struct* _temp109=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp109->tag= Cyc_Arg_Wrong_tag;
_temp109->f1= s; _temp109->f2= arg; _temp109->f3=( struct _tagged_string)({ char*
_temp110=( char*)"an integer"; struct _tagged_string _temp111; _temp111.curr=
_temp110; _temp111.base= _temp110; _temp111.last_plus_one= _temp110 + 11;
_temp111;});( void*) _temp109;}), speclist, errmsg); _npop_handler( 0u); return;
_LL106:( void) _throw( _temp101); _LL102:;}}} _temp87( n); ++ Cyc_Arg_current;
goto _LL66;} _LL78: while( Cyc_Arg_current < l - 1) { _temp89(({ struct
_tagged_ptr0 _temp112= Cyc_Arg_args; struct _tagged_string* _temp114= _temp112.curr
+( Cyc_Arg_current + 1); if( _temp114 < _temp112.base? 1: _temp114 >= _temp112.last_plus_one){
_throw( Null_Exception);}* _temp114;})); ++ Cyc_Arg_current;} goto _LL66; _LL66:;}
_pop_handler();} else{ struct _xenum_struct* _temp116= _temp64; struct
_tagged_string _temp122; _LL118: if((*(( struct _xenum_struct*) _temp116)).tag
== Cyc_Arg_Bad_tag){ _LL123: _temp122=(( struct Cyc_Arg_Bad_struct*) _temp116)->f1;
goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119: Cyc_Arg_stop(
initpos,({ struct Cyc_Arg_Message_struct* _temp124=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp124->tag= Cyc_Arg_Message_tag;
_temp124->f1= _temp122;( void*) _temp124;}), speclist, errmsg); goto _LL117;
_LL121:( void) _throw( _temp116); _LL117:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp125; _push_handler(& _temp125);{ struct _xenum_struct*
_temp126=( struct _xenum_struct*) setjmp( _temp125.handler); if( ! _temp126){
anonfun( s); _pop_handler();} else{ struct _xenum_struct* _temp128= _temp126;
struct _tagged_string _temp134; _LL130: if((*(( struct _xenum_struct*) _temp128)).tag
== Cyc_Arg_Bad_tag){ _LL135: _temp134=(( struct Cyc_Arg_Bad_struct*) _temp128)->f1;
goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL131: Cyc_Arg_stop(
initpos,({ struct Cyc_Arg_Message_struct* _temp136=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp136->tag= Cyc_Arg_Message_tag;
_temp136->f1= _temp134;( void*) _temp136;}), speclist, errmsg); goto _LL129;
_LL133:( void) _throw( _temp128); _LL129:;}}} ++ Cyc_Arg_current;}}}}