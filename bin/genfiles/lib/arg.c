#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one; } ; struct _tuple0{ struct _tagged_string
f1; void* f2; struct _tagged_string f3; } ; typedef unsigned int Cyc_uint;
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
char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_string); extern char* string_to_Cstring( struct _tagged_string);
extern char* underlying_Cstring( struct _tagged_string); extern struct
_tagged_string Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern char Cyc_Arg_Error_tag[ 6u]; struct Cyc_Arg_Error_struct{
char* tag; } ; extern char Cyc_Arg_Bad_tag[ 4u]; struct Cyc_Arg_Bad_struct{ char*
tag; struct _tagged_string f1; } ; typedef void* Cyc_Arg_Spec; static const int
Cyc_Arg_Unit_spec_tag= 0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)();
} ; static const int Cyc_Arg_Set_spec_tag= 1; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec_tag= 2; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_String_spec_tag= 3; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec_tag=
4; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec_tag= 5; struct Cyc_Arg_Rest_spec_struct{ int tag; void(*
f1)( struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void*
Cyc_Arg_spec_t; extern void Cyc_Arg_usage( struct Cyc_List_List*, struct
_tagged_string); extern int Cyc_Arg_current; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_string), struct
_tagged_string errmsg, struct _tagged_ptr0 args); char Cyc_Arg_Error_tag[ 6u]="Error";
char Cyc_Arg_Bad_tag[ 4u]="Bad"; typedef void* Cyc_Arg_error; static const int
Cyc_Arg_Unknown_tag= 0; struct Cyc_Arg_Unknown_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Arg_Missing_tag= 1; struct Cyc_Arg_Missing_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Arg_Message_tag= 2;
struct Cyc_Arg_Message_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Arg_Wrong_tag= 3; struct Cyc_Arg_Wrong_struct{ int tag; struct
_tagged_string f1; struct _tagged_string f2; struct _tagged_string f3; } ;
static void* Cyc_Arg_lookup( struct Cyc_List_List* l, struct _tagged_string x){
while( l != 0) { if( Cyc_String_strcmp( x,(*(( struct _tuple0*) l->hd)).f1) == 0){
return(*(( struct _tuple0*) l->hd)).f2;} l= l->tl;}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp0=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp0[ 0]=({ struct Cyc_Core_Not_found_struct
_temp1; _temp1.tag= Cyc_Core_Not_found_tag; _temp1;}); _temp0;}));} void Cyc_Arg_usage(
struct Cyc_List_List* speclist, struct _tagged_string errmsg){({ struct
_tagged_string _temp2= errmsg; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp2.last_plus_one
- _temp2.curr, _temp2.curr);}); while( speclist != 0) {({ struct _tagged_string
_temp3=(*(( struct _tuple0*) speclist->hd)).f1; struct _tagged_string _temp4=(*((
struct _tuple0*) speclist->hd)).f3; fprintf( Cyc_Stdio_stderr," %.*s %.*s\n",
_temp3.last_plus_one - _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr,
_temp4.curr);}); speclist= speclist->tl;}} int Cyc_Arg_current= 0; static struct
_tagged_ptr0 Cyc_Arg_args={ 0, 0, 0}; static void Cyc_Arg_stop( int prog_pos,
void* e, struct Cyc_List_List* speclist, struct _tagged_string errmsg){ struct
_tagged_string progname=( unsigned int) prog_pos <({ struct _tagged_ptr0 _temp5=
Cyc_Arg_args;( unsigned int)( _temp5.last_plus_one - _temp5.curr);})?({ struct
_tagged_ptr0 _temp6= Cyc_Arg_args; struct _tagged_string* _temp8= _temp6.curr +
prog_pos; if( _temp8 < _temp6.base? 1: _temp8 >= _temp6.last_plus_one){ _throw(
Null_Exception);}* _temp8;}):( struct _tagged_string)({ char* _temp9=( char*)"(?)";
struct _tagged_string _temp10; _temp10.curr= _temp9; _temp10.base= _temp9;
_temp10.last_plus_one= _temp9 + 4; _temp10;});{ void* _temp11= e; struct
_tagged_string _temp21; struct _tagged_string _temp23; struct _tagged_string
_temp25; struct _tagged_string _temp27; struct _tagged_string _temp29; struct
_tagged_string _temp31; _LL13: if((( struct _tunion_struct*) _temp11)->tag ==
Cyc_Arg_Unknown_tag){ _LL22: _temp21=( struct _tagged_string)(( struct Cyc_Arg_Unknown_struct*)
_temp11)->f1; goto _LL14;} else{ goto _LL15;} _LL15: if((( struct _tunion_struct*)
_temp11)->tag == Cyc_Arg_Missing_tag){ _LL24: _temp23=( struct _tagged_string)((
struct Cyc_Arg_Missing_struct*) _temp11)->f1; goto _LL16;} else{ goto _LL17;}
_LL17: if((( struct _tunion_struct*) _temp11)->tag == Cyc_Arg_Wrong_tag){ _LL30:
_temp29=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp11)->f1;
goto _LL28; _LL28: _temp27=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*)
_temp11)->f2; goto _LL26; _LL26: _temp25=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*)
_temp11)->f3; goto _LL18;} else{ goto _LL19;} _LL19: if((( struct _tunion_struct*)
_temp11)->tag == Cyc_Arg_Message_tag){ _LL32: _temp31=( struct _tagged_string)((
struct Cyc_Arg_Message_struct*) _temp11)->f1; goto _LL20;} else{ goto _LL12;}
_LL14: if( Cyc_String_strcmp( _temp21,( struct _tagged_string)({ char* _temp33=(
char*)"-help"; struct _tagged_string _temp34; _temp34.curr= _temp33; _temp34.base=
_temp33; _temp34.last_plus_one= _temp33 + 6; _temp34;})) != 0){({ struct
_tagged_string _temp35= progname; struct _tagged_string _temp36= _temp21;
fprintf( Cyc_Stdio_stderr,"%.*s: unknown option `%.*s'.\n", _temp35.last_plus_one
- _temp35.curr, _temp35.curr, _temp36.last_plus_one - _temp36.curr, _temp36.curr);});}
goto _LL12; _LL16:({ struct _tagged_string _temp37= progname; struct
_tagged_string _temp38= _temp23; fprintf( Cyc_Stdio_stderr,"%.*s: option `%.*s' needs an argument.\n",
_temp37.last_plus_one - _temp37.curr, _temp37.curr, _temp38.last_plus_one -
_temp38.curr, _temp38.curr);}); goto _LL12; _LL18:({ struct _tagged_string
_temp39= progname; struct _tagged_string _temp40= _temp27; struct _tagged_string
_temp41= _temp29; struct _tagged_string _temp42= _temp25; fprintf( Cyc_Stdio_stderr,"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp39.last_plus_one - _temp39.curr, _temp39.curr, _temp40.last_plus_one -
_temp40.curr, _temp40.curr, _temp41.last_plus_one - _temp41.curr, _temp41.curr,
_temp42.last_plus_one - _temp42.curr, _temp42.curr);}); goto _LL12; _LL20:({
struct _tagged_string _temp43= progname; struct _tagged_string _temp44= _temp31;
fprintf( Cyc_Stdio_stderr,"%.*s: %.*s.\n", _temp43.last_plus_one - _temp43.curr,
_temp43.curr, _temp44.last_plus_one - _temp44.curr, _temp44.curr);}); goto _LL12;
_LL12:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)({ struct
_tagged_ptr0 _temp45= Cyc_Arg_args;( unsigned int)( _temp45.last_plus_one -
_temp45.curr);});} void Cyc_Arg_parse( struct Cyc_List_List* speclist, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 orig_args){ Cyc_Arg_args= orig_args;{ int initpos= Cyc_Arg_current;
int l=( int)({ struct _tagged_ptr0 _temp46= Cyc_Arg_args;( unsigned int)(
_temp46.last_plus_one - _temp46.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=({ struct _tagged_ptr0 _temp47= Cyc_Arg_args;
struct _tagged_string* _temp49= _temp47.curr + Cyc_Arg_current; if( _temp49 <
_temp47.base? 1: _temp49 >= _temp47.last_plus_one){ _throw( Null_Exception);}*
_temp49;}); if(({ struct _tagged_string _temp50= s;( unsigned int)( _temp50.last_plus_one
- _temp50.curr);}) >=( unsigned int) 1?( int)({ struct _tagged_string _temp51= s;
char* _temp53= _temp51.curr + 0; if( _temp53 < _temp51.base? 1: _temp53 >=
_temp51.last_plus_one){ _throw( Null_Exception);}* _temp53;}) ==( int)'-': 0){
void* action;{ struct _handler_cons _temp54; _push_handler(& _temp54);{ struct
_xtunion_struct* _temp55=( struct _xtunion_struct*) setjmp( _temp54.handler);
if( ! _temp55){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();} else{
struct _xtunion_struct* _temp57= _temp55; _LL59: if( _temp57->tag == Cyc_Core_Not_found_tag){
goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL60: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Unknown_struct* _temp63=( struct Cyc_Arg_Unknown_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct)); _temp63[ 0]=({ struct Cyc_Arg_Unknown_struct
_temp64; _temp64.tag= Cyc_Arg_Unknown_tag; _temp64.f1= s; _temp64;}); _temp63;}),
speclist, errmsg); return; _LL62:( void) _throw( _temp57); _LL58:;}}}{ struct
_handler_cons _temp65; _push_handler(& _temp65);{ struct _xtunion_struct*
_temp66=( struct _xtunion_struct*) setjmp( _temp65.handler); if( ! _temp66){{
void* _temp67= action; void(* _temp81)(); int* _temp83; int* _temp85; void(*
_temp87)( struct _tagged_string); void(* _temp89)( int); void(* _temp91)( struct
_tagged_string); _LL69: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Arg_Unit_spec_tag){
_LL82: _temp81=( void(*)())(( struct Cyc_Arg_Unit_spec_struct*) _temp67)->f1;
goto _LL70;} else{ goto _LL71;} _LL71: if((( struct _tunion_struct*) _temp67)->tag
== Cyc_Arg_Set_spec_tag){ _LL84: _temp83=( int*)(( struct Cyc_Arg_Set_spec_struct*)
_temp67)->f1; goto _LL72;} else{ goto _LL73;} _LL73: if((( struct _tunion_struct*)
_temp67)->tag == Cyc_Arg_Clear_spec_tag){ _LL86: _temp85=( int*)(( struct Cyc_Arg_Clear_spec_struct*)
_temp67)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if((( struct _tunion_struct*)
_temp67)->tag == Cyc_Arg_String_spec_tag){ _LL88: _temp87=( void(*)( struct
_tagged_string))(( struct Cyc_Arg_String_spec_struct*) _temp67)->f1; goto _LL76;}
else{ goto _LL77;} _LL77: if((( struct _tunion_struct*) _temp67)->tag == Cyc_Arg_Int_spec_tag){
_LL90: _temp89=( void(*)( int))(( struct Cyc_Arg_Int_spec_struct*) _temp67)->f1;
goto _LL78;} else{ goto _LL79;} _LL79: if((( struct _tunion_struct*) _temp67)->tag
== Cyc_Arg_Rest_spec_tag){ _LL92: _temp91=( void(*)( struct _tagged_string))((
struct Cyc_Arg_Rest_spec_struct*) _temp67)->f1; goto _LL80;} else{ goto _LL68;}
_LL70: _temp81(); goto _LL68; _LL72:* _temp83= 1; goto _LL68; _LL74:* _temp85= 0;
goto _LL68; _LL76: if( Cyc_Arg_current + 1 < l){ _temp87(({ struct _tagged_ptr0
_temp93= Cyc_Arg_args; struct _tagged_string* _temp95= _temp93.curr +( Cyc_Arg_current
+ 1); if( _temp95 < _temp93.base? 1: _temp95 >= _temp93.last_plus_one){ _throw(
Null_Exception);}* _temp95;})); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Missing_struct* _temp96=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp96[ 0]=({ struct Cyc_Arg_Missing_struct
_temp97; _temp97.tag= Cyc_Arg_Missing_tag; _temp97.f1= s; _temp97;}); _temp96;}),
speclist, errmsg);} goto _LL68; _LL78: { struct _tagged_string arg=({ struct
_tagged_ptr0 _temp98= Cyc_Arg_args; struct _tagged_string* _temp100= _temp98.curr
+( Cyc_Arg_current + 1); if( _temp100 < _temp98.base? 1: _temp100 >= _temp98.last_plus_one){
_throw( Null_Exception);}* _temp100;}); int n;{ struct _handler_cons _temp101;
_push_handler(& _temp101);{ struct _xtunion_struct* _temp102=( struct
_xtunion_struct*) setjmp( _temp101.handler); if( ! _temp102){ n= Cyc_Core_int_of_string(
arg);; _pop_handler();} else{ struct _xtunion_struct* _temp104= _temp102; struct
_tagged_string _temp110; _LL106: if((*(( struct _xtunion_struct*) _temp104)).tag
== Cyc_Core_InvalidArg_tag){ _LL111: _temp110=(( struct Cyc_Core_InvalidArg_struct*)
_temp104)->f1; goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL107:
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Wrong_struct* _temp112=( struct
Cyc_Arg_Wrong_struct*) GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct));
_temp112[ 0]=({ struct Cyc_Arg_Wrong_struct _temp113; _temp113.tag= Cyc_Arg_Wrong_tag;
_temp113.f1= s; _temp113.f2= arg; _temp113.f3=( struct _tagged_string)({ char*
_temp114=( char*)"an integer"; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 11;
_temp115;}); _temp113;}); _temp112;}), speclist, errmsg); _npop_handler( 0u);
return; _LL109:( void) _throw( _temp104); _LL105:;}}} _temp89( n); ++ Cyc_Arg_current;
goto _LL68;} _LL80: while( Cyc_Arg_current < l - 1) { _temp91(({ struct
_tagged_ptr0 _temp116= Cyc_Arg_args; struct _tagged_string* _temp118= _temp116.curr
+( Cyc_Arg_current + 1); if( _temp118 < _temp116.base? 1: _temp118 >= _temp116.last_plus_one){
_throw( Null_Exception);}* _temp118;})); ++ Cyc_Arg_current;} goto _LL68; _LL68:;};
_pop_handler();} else{ struct _xtunion_struct* _temp120= _temp66; struct
_tagged_string _temp126; _LL122: if((*(( struct _xtunion_struct*) _temp120)).tag
== Cyc_Arg_Bad_tag){ _LL127: _temp126=(( struct Cyc_Arg_Bad_struct*) _temp120)->f1;
goto _LL123;} else{ goto _LL124;} _LL124: goto _LL125; _LL123: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp128=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp128[ 0]=({ struct Cyc_Arg_Message_struct
_temp129; _temp129.tag= Cyc_Arg_Message_tag; _temp129.f1= _temp126; _temp129;});
_temp128;}), speclist, errmsg); goto _LL121; _LL125:( void) _throw( _temp120);
_LL121:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp130;
_push_handler(& _temp130);{ struct _xtunion_struct* _temp131=( struct
_xtunion_struct*) setjmp( _temp130.handler); if( ! _temp131){ anonfun( s);;
_pop_handler();} else{ struct _xtunion_struct* _temp133= _temp131; struct
_tagged_string _temp139; _LL135: if((*(( struct _xtunion_struct*) _temp133)).tag
== Cyc_Arg_Bad_tag){ _LL140: _temp139=(( struct Cyc_Arg_Bad_struct*) _temp133)->f1;
goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL136: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp141=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp141[ 0]=({ struct Cyc_Arg_Message_struct
_temp142; _temp142.tag= Cyc_Arg_Message_tag; _temp142.f1= _temp139; _temp142;});
_temp141;}), speclist, errmsg); goto _LL134; _LL138:( void) _throw( _temp133);
_LL134:;}}} ++ Cyc_Arg_current;}}}}