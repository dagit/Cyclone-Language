#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one; } ; struct _tuple0{ struct _tagged_string
f1; void* f2; struct _tagged_string f3; } ; typedef int Cyc_ptrdiff_t; typedef
unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_string); extern char* string_to_Cstring( struct _tagged_string);
extern char* underlying_Cstring( struct _tagged_string); extern struct
_tagged_string Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern char Cyc_Arg_Error_tag[
6u]; struct Cyc_Arg_Error_struct{ char* tag; } ; extern char Cyc_Arg_Bad_tag[ 4u];
struct Cyc_Arg_Bad_struct{ char* tag; struct _tagged_string f1; } ; typedef void*
Cyc_Arg_Spec; static const int Cyc_Arg_Unit_spec_tag= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Set_spec_tag= 1; struct Cyc_Arg_Set_spec_struct{
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
while( l != 0) { if( Cyc_String_strcmp( x,(*(( struct _tuple0*)({ struct Cyc_List_List*
_temp0= l; if( _temp0 == 0){ _throw( Null_Exception);} _temp0->hd;}))).f1) == 0){
return(*(( struct _tuple0*)({ struct Cyc_List_List* _temp1= l; if( _temp1 == 0){
_throw( Null_Exception);} _temp1->hd;}))).f2;} l=({ struct Cyc_List_List* _temp2=
l; if( _temp2 == 0){ _throw( Null_Exception);} _temp2->tl;});}( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp3=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp3[ 0]=({ struct Cyc_Core_Not_found_struct
_temp4; _temp4.tag= Cyc_Core_Not_found_tag; _temp4;}); _temp3;}));} void Cyc_Arg_usage(
struct Cyc_List_List* speclist, struct _tagged_string errmsg){({ struct
_tagged_string _temp5= errmsg; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp5.last_plus_one
- _temp5.curr, _temp5.curr);}); while( speclist != 0) {({ struct _tagged_string
_temp8=(*(( struct _tuple0*)({ struct Cyc_List_List* _temp6= speclist; if(
_temp6 == 0){ _throw( Null_Exception);} _temp6->hd;}))).f1; struct
_tagged_string _temp9=(*(( struct _tuple0*)({ struct Cyc_List_List* _temp7=
speclist; if( _temp7 == 0){ _throw( Null_Exception);} _temp7->hd;}))).f3;
fprintf( Cyc_Stdio_stderr," %.*s %.*s\n", _temp8.last_plus_one - _temp8.curr,
_temp8.curr, _temp9.last_plus_one - _temp9.curr, _temp9.curr);}); speclist=({
struct Cyc_List_List* _temp10= speclist; if( _temp10 == 0){ _throw(
Null_Exception);} _temp10->tl;});}} int Cyc_Arg_current= 0; static struct
_tagged_ptr0 Cyc_Arg_args={ 0, 0, 0}; static void Cyc_Arg_stop( int prog_pos,
void* e, struct Cyc_List_List* speclist, struct _tagged_string errmsg){ struct
_tagged_string progname=( unsigned int) prog_pos <({ struct _tagged_ptr0 _temp11=
Cyc_Arg_args;( unsigned int)( _temp11.last_plus_one - _temp11.curr);})?({ struct
_tagged_ptr0 _temp12= Cyc_Arg_args; struct _tagged_string* _temp14= _temp12.curr
+ prog_pos; if( _temp12.base == 0? 1:( _temp14 < _temp12.base? 1: _temp14 >=
_temp12.last_plus_one)){ _throw( Null_Exception);}* _temp14;}):( struct
_tagged_string)({ char* _temp15=( char*)"(?)"; struct _tagged_string _temp16;
_temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one= _temp15 + 4;
_temp16;});{ void* _temp17= e; struct _tagged_string _temp27; struct
_tagged_string _temp29; struct _tagged_string _temp31; struct _tagged_string
_temp33; struct _tagged_string _temp35; struct _tagged_string _temp37; _LL19:
if((( struct _tunion_struct*) _temp17)->tag == Cyc_Arg_Unknown_tag){ _LL28:
_temp27=( struct _tagged_string)(( struct Cyc_Arg_Unknown_struct*) _temp17)->f1;
goto _LL20;} else{ goto _LL21;} _LL21: if((( struct _tunion_struct*) _temp17)->tag
== Cyc_Arg_Missing_tag){ _LL30: _temp29=( struct _tagged_string)(( struct Cyc_Arg_Missing_struct*)
_temp17)->f1; goto _LL22;} else{ goto _LL23;} _LL23: if((( struct _tunion_struct*)
_temp17)->tag == Cyc_Arg_Wrong_tag){ _LL36: _temp35=( struct _tagged_string)((
struct Cyc_Arg_Wrong_struct*) _temp17)->f1; goto _LL34; _LL34: _temp33=( struct
_tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp17)->f2; goto _LL32; _LL32:
_temp31=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp17)->f3;
goto _LL24;} else{ goto _LL25;} _LL25: if((( struct _tunion_struct*) _temp17)->tag
== Cyc_Arg_Message_tag){ _LL38: _temp37=( struct _tagged_string)(( struct Cyc_Arg_Message_struct*)
_temp17)->f1; goto _LL26;} else{ goto _LL18;} _LL20: if( Cyc_String_strcmp(
_temp27,( struct _tagged_string)({ char* _temp39=( char*)"-help"; struct
_tagged_string _temp40; _temp40.curr= _temp39; _temp40.base= _temp39; _temp40.last_plus_one=
_temp39 + 6; _temp40;})) != 0){({ struct _tagged_string _temp41= progname;
struct _tagged_string _temp42= _temp27; fprintf( Cyc_Stdio_stderr,"%.*s: unknown option `%.*s'.\n",
_temp41.last_plus_one - _temp41.curr, _temp41.curr, _temp42.last_plus_one -
_temp42.curr, _temp42.curr);});} goto _LL18; _LL22:({ struct _tagged_string
_temp43= progname; struct _tagged_string _temp44= _temp29; fprintf( Cyc_Stdio_stderr,"%.*s: option `%.*s' needs an argument.\n",
_temp43.last_plus_one - _temp43.curr, _temp43.curr, _temp44.last_plus_one -
_temp44.curr, _temp44.curr);}); goto _LL18; _LL24:({ struct _tagged_string
_temp45= progname; struct _tagged_string _temp46= _temp33; struct _tagged_string
_temp47= _temp35; struct _tagged_string _temp48= _temp31; fprintf( Cyc_Stdio_stderr,"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp45.last_plus_one - _temp45.curr, _temp45.curr, _temp46.last_plus_one -
_temp46.curr, _temp46.curr, _temp47.last_plus_one - _temp47.curr, _temp47.curr,
_temp48.last_plus_one - _temp48.curr, _temp48.curr);}); goto _LL18; _LL26:({
struct _tagged_string _temp49= progname; struct _tagged_string _temp50= _temp37;
fprintf( Cyc_Stdio_stderr,"%.*s: %.*s.\n", _temp49.last_plus_one - _temp49.curr,
_temp49.curr, _temp50.last_plus_one - _temp50.curr, _temp50.curr);}); goto _LL18;
_LL18:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)({ struct
_tagged_ptr0 _temp51= Cyc_Arg_args;( unsigned int)( _temp51.last_plus_one -
_temp51.curr);});} void Cyc_Arg_parse( struct Cyc_List_List* speclist, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 orig_args){ Cyc_Arg_args= orig_args;{ int initpos= Cyc_Arg_current;
int l=( int)({ struct _tagged_ptr0 _temp52= Cyc_Arg_args;( unsigned int)(
_temp52.last_plus_one - _temp52.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=({ struct _tagged_ptr0 _temp53= Cyc_Arg_args;
struct _tagged_string* _temp55= _temp53.curr + Cyc_Arg_current; if( _temp53.base
== 0? 1:( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one)){ _throw(
Null_Exception);}* _temp55;}); if(({ struct _tagged_string _temp56= s;(
unsigned int)( _temp56.last_plus_one - _temp56.curr);}) >=( unsigned int) 1?(
int)({ struct _tagged_string _temp57= s; char* _temp59= _temp57.curr + 0; if(
_temp57.base == 0? 1:( _temp59 < _temp57.base? 1: _temp59 >= _temp57.last_plus_one)){
_throw( Null_Exception);}* _temp59;}) ==( int)'-': 0){ void* action;{ struct
_handler_cons _temp60; _push_handler(& _temp60);{ struct _xtunion_struct*
_temp61=( struct _xtunion_struct*) setjmp( _temp60.handler); if( ! _temp61){
action= Cyc_Arg_lookup( speclist, s);; _pop_handler();} else{ struct
_xtunion_struct* _temp63= _temp61; _LL65: if( _temp63->tag == Cyc_Core_Not_found_tag){
goto _LL66;} else{ goto _LL67;} _LL67: goto _LL68; _LL66: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Unknown_struct* _temp69=( struct Cyc_Arg_Unknown_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct)); _temp69[ 0]=({ struct Cyc_Arg_Unknown_struct
_temp70; _temp70.tag= Cyc_Arg_Unknown_tag; _temp70.f1= s; _temp70;}); _temp69;}),
speclist, errmsg); return; _LL68:( void) _throw( _temp63); _LL64:;}}}{ struct
_handler_cons _temp71; _push_handler(& _temp71);{ struct _xtunion_struct*
_temp72=( struct _xtunion_struct*) setjmp( _temp71.handler); if( ! _temp72){{
void* _temp73= action; void(* _temp87)(); int* _temp89; int* _temp91; void(*
_temp93)( struct _tagged_string); void(* _temp95)( int); void(* _temp97)( struct
_tagged_string); _LL75: if((( struct _tunion_struct*) _temp73)->tag == Cyc_Arg_Unit_spec_tag){
_LL88: _temp87=( void(*)())(( struct Cyc_Arg_Unit_spec_struct*) _temp73)->f1;
goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _tunion_struct*) _temp73)->tag
== Cyc_Arg_Set_spec_tag){ _LL90: _temp89=( int*)(( struct Cyc_Arg_Set_spec_struct*)
_temp73)->f1; goto _LL78;} else{ goto _LL79;} _LL79: if((( struct _tunion_struct*)
_temp73)->tag == Cyc_Arg_Clear_spec_tag){ _LL92: _temp91=( int*)(( struct Cyc_Arg_Clear_spec_struct*)
_temp73)->f1; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*)
_temp73)->tag == Cyc_Arg_String_spec_tag){ _LL94: _temp93=( void(*)( struct
_tagged_string))(( struct Cyc_Arg_String_spec_struct*) _temp73)->f1; goto _LL82;}
else{ goto _LL83;} _LL83: if((( struct _tunion_struct*) _temp73)->tag == Cyc_Arg_Int_spec_tag){
_LL96: _temp95=( void(*)( int))(( struct Cyc_Arg_Int_spec_struct*) _temp73)->f1;
goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _tunion_struct*) _temp73)->tag
== Cyc_Arg_Rest_spec_tag){ _LL98: _temp97=( void(*)( struct _tagged_string))((
struct Cyc_Arg_Rest_spec_struct*) _temp73)->f1; goto _LL86;} else{ goto _LL74;}
_LL76: _temp87(); goto _LL74; _LL78:* _temp89= 1; goto _LL74; _LL80:* _temp91= 0;
goto _LL74; _LL82: if( Cyc_Arg_current + 1 < l){ _temp93(({ struct _tagged_ptr0
_temp99= Cyc_Arg_args; struct _tagged_string* _temp101= _temp99.curr +( Cyc_Arg_current
+ 1); if( _temp99.base == 0? 1:( _temp101 < _temp99.base? 1: _temp101 >= _temp99.last_plus_one)){
_throw( Null_Exception);}* _temp101;})); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp102=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp102[ 0]=({ struct Cyc_Arg_Missing_struct
_temp103; _temp103.tag= Cyc_Arg_Missing_tag; _temp103.f1= s; _temp103;});
_temp102;}), speclist, errmsg);} goto _LL74; _LL84: { struct _tagged_string arg=({
struct _tagged_ptr0 _temp104= Cyc_Arg_args; struct _tagged_string* _temp106=
_temp104.curr +( Cyc_Arg_current + 1); if( _temp104.base == 0? 1:( _temp106 <
_temp104.base? 1: _temp106 >= _temp104.last_plus_one)){ _throw( Null_Exception);}*
_temp106;}); int n;{ struct _handler_cons _temp107; _push_handler(& _temp107);{
struct _xtunion_struct* _temp108=( struct _xtunion_struct*) setjmp( _temp107.handler);
if( ! _temp108){ n= Cyc_Core_int_of_string( arg);; _pop_handler();} else{ struct
_xtunion_struct* _temp110= _temp108; struct _tagged_string _temp116; _LL112: if((*((
struct _xtunion_struct*) _temp110)).tag == Cyc_Core_InvalidArg_tag){ _LL117:
_temp116=(( struct Cyc_Core_InvalidArg_struct*) _temp110)->f1; goto _LL113;}
else{ goto _LL114;} _LL114: goto _LL115; _LL113: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Wrong_struct* _temp118=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp118[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp119; _temp119.tag= Cyc_Arg_Wrong_tag; _temp119.f1= s; _temp119.f2= arg;
_temp119.f3=( struct _tagged_string)({ char* _temp120=( char*)"an integer";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 11; _temp121;}); _temp119;}); _temp118;}),
speclist, errmsg); _npop_handler( 0u); return; _LL115:( void) _throw( _temp110);
_LL111:;}}} _temp95( n); ++ Cyc_Arg_current; goto _LL74;} _LL86: while( Cyc_Arg_current
< l - 1) { _temp97(({ struct _tagged_ptr0 _temp122= Cyc_Arg_args; struct
_tagged_string* _temp124= _temp122.curr +( Cyc_Arg_current + 1); if( _temp122.base
== 0? 1:( _temp124 < _temp122.base? 1: _temp124 >= _temp122.last_plus_one)){
_throw( Null_Exception);}* _temp124;})); ++ Cyc_Arg_current;} goto _LL74; _LL74:;};
_pop_handler();} else{ struct _xtunion_struct* _temp126= _temp72; struct
_tagged_string _temp132; _LL128: if((*(( struct _xtunion_struct*) _temp126)).tag
== Cyc_Arg_Bad_tag){ _LL133: _temp132=(( struct Cyc_Arg_Bad_struct*) _temp126)->f1;
goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp134=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp134[ 0]=({ struct Cyc_Arg_Message_struct
_temp135; _temp135.tag= Cyc_Arg_Message_tag; _temp135.f1= _temp132; _temp135;});
_temp134;}), speclist, errmsg); goto _LL127; _LL131:( void) _throw( _temp126);
_LL127:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp136;
_push_handler(& _temp136);{ struct _xtunion_struct* _temp137=( struct
_xtunion_struct*) setjmp( _temp136.handler); if( ! _temp137){ anonfun( s);;
_pop_handler();} else{ struct _xtunion_struct* _temp139= _temp137; struct
_tagged_string _temp145; _LL141: if((*(( struct _xtunion_struct*) _temp139)).tag
== Cyc_Arg_Bad_tag){ _LL146: _temp145=(( struct Cyc_Arg_Bad_struct*) _temp139)->f1;
goto _LL142;} else{ goto _LL143;} _LL143: goto _LL144; _LL142: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp147=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp147[ 0]=({ struct Cyc_Arg_Message_struct
_temp148; _temp148.tag= Cyc_Arg_Message_tag; _temp148.f1= _temp145; _temp148;});
_temp147;}), speclist, errmsg); goto _LL140; _LL144:( void) _throw( _temp139);
_LL140:;}}} ++ Cyc_Arg_current;}}}}