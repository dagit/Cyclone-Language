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
_tagged_string progname= prog_pos <({ struct _tagged_ptr0 _temp46= Cyc_Arg_args;(
unsigned int)( _temp46.last_plus_one - _temp46.curr);})?({ struct _tagged_ptr0
_temp47= Cyc_Arg_args; struct _tagged_string* _temp49= _temp47.curr + prog_pos;
if( _temp47.base == 0? 1:( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){
_throw( Null_Exception);}* _temp49;}):( struct _tagged_string)({ char* _temp50=(
char*)"(?)"; struct _tagged_string _temp51; _temp51.curr= _temp50; _temp51.base=
_temp50; _temp51.last_plus_one= _temp50 + 4; _temp51;});{ void* _temp11= e;
struct _tagged_string _temp21; struct _tagged_string _temp23; struct
_tagged_string _temp25; struct _tagged_string _temp27; struct _tagged_string
_temp29; struct _tagged_string _temp31; _LL13: if((( struct _tunion_struct*)
_temp11)->tag == Cyc_Arg_Unknown_tag){ _LL22: _temp21=( struct _tagged_string)((
struct Cyc_Arg_Unknown_struct*) _temp11)->f1; goto _LL14;} else{ goto _LL15;}
_LL15: if((( struct _tunion_struct*) _temp11)->tag == Cyc_Arg_Missing_tag){
_LL24: _temp23=( struct _tagged_string)(( struct Cyc_Arg_Missing_struct*)
_temp11)->f1; goto _LL16;} else{ goto _LL17;} _LL17: if((( struct _tunion_struct*)
_temp11)->tag == Cyc_Arg_Wrong_tag){ _LL30: _temp29=( struct _tagged_string)((
struct Cyc_Arg_Wrong_struct*) _temp11)->f1; goto _LL28; _LL28: _temp27=( struct
_tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp11)->f2; goto _LL26; _LL26:
_temp25=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp11)->f3;
goto _LL18;} else{ goto _LL19;} _LL19: if((( struct _tunion_struct*) _temp11)->tag
== Cyc_Arg_Message_tag){ _LL32: _temp31=( struct _tagged_string)(( struct Cyc_Arg_Message_struct*)
_temp11)->f1; goto _LL20;} else{ goto _LL12;} _LL14: if( Cyc_String_strcmp(
_temp21,( struct _tagged_string)({ char* _temp33=( char*)"-help"; struct
_tagged_string _temp34; _temp34.curr= _temp33; _temp34.base= _temp33; _temp34.last_plus_one=
_temp33 + 6; _temp34;})) != 0){({ struct _tagged_string _temp35= progname;
struct _tagged_string _temp36= _temp21; fprintf( Cyc_Stdio_stderr,"%.*s: unknown option `%.*s'.\n",
_temp35.last_plus_one - _temp35.curr, _temp35.curr, _temp36.last_plus_one -
_temp36.curr, _temp36.curr);});} goto _LL12; _LL16:({ struct _tagged_string
_temp37= progname; struct _tagged_string _temp38= _temp23; fprintf( Cyc_Stdio_stderr,"%.*s: option `%.*s' needs an argument.\n",
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
int l=( int)({ struct _tagged_ptr0 _temp148= Cyc_Arg_args;( unsigned int)(
_temp148.last_plus_one - _temp148.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=({ struct _tagged_ptr0 _temp145= Cyc_Arg_args;
struct _tagged_string* _temp147= _temp145.curr + Cyc_Arg_current; if( _temp145.base
== 0? 1:( _temp147 < _temp145.base? 1: _temp147 >= _temp145.last_plus_one)){
_throw( Null_Exception);}* _temp147;}); if(({ struct _tagged_string _temp52= s;(
unsigned int)( _temp52.last_plus_one - _temp52.curr);}) >= 1?({ struct
_tagged_string _temp53= s; char* _temp55= _temp53.curr + 0; if( _temp53.base ==
0? 1:( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one)){ _throw(
Null_Exception);}* _temp55;}) =='-': 0){ void* action;{ struct _handler_cons
_temp56; _push_handler(& _temp56);{ struct _xtunion_struct* _temp57=( struct
_xtunion_struct*) setjmp( _temp56.handler); if( ! _temp57){ action= Cyc_Arg_lookup(
speclist, s);; _pop_handler();} else{ struct _xtunion_struct* _temp59= _temp57;
_LL61: if( _temp59->tag == Cyc_Core_Not_found_tag){ goto _LL62;} else{ goto
_LL63;} _LL63: goto _LL64; _LL62: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp65=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp65[ 0]=({ struct Cyc_Arg_Unknown_struct _temp66; _temp66.tag= Cyc_Arg_Unknown_tag;
_temp66.f1= s; _temp66;}); _temp65;}), speclist, errmsg); return; _LL64:( void)
_throw( _temp59); _LL60:;}}}{ struct _handler_cons _temp67; _push_handler(&
_temp67);{ struct _xtunion_struct* _temp68=( struct _xtunion_struct*) setjmp(
_temp67.handler); if( ! _temp68){{ void* _temp69= action; void(* _temp83)(); int*
_temp85; int* _temp87; void(* _temp89)( struct _tagged_string); void(* _temp91)(
int); void(* _temp93)( struct _tagged_string); _LL71: if((( struct
_tunion_struct*) _temp69)->tag == Cyc_Arg_Unit_spec_tag){ _LL84: _temp83=( void(*)())((
struct Cyc_Arg_Unit_spec_struct*) _temp69)->f1; goto _LL72;} else{ goto _LL73;}
_LL73: if((( struct _tunion_struct*) _temp69)->tag == Cyc_Arg_Set_spec_tag){
_LL86: _temp85=( int*)(( struct Cyc_Arg_Set_spec_struct*) _temp69)->f1; goto
_LL74;} else{ goto _LL75;} _LL75: if((( struct _tunion_struct*) _temp69)->tag ==
Cyc_Arg_Clear_spec_tag){ _LL88: _temp87=( int*)(( struct Cyc_Arg_Clear_spec_struct*)
_temp69)->f1; goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _tunion_struct*)
_temp69)->tag == Cyc_Arg_String_spec_tag){ _LL90: _temp89=( void(*)( struct
_tagged_string))(( struct Cyc_Arg_String_spec_struct*) _temp69)->f1; goto _LL78;}
else{ goto _LL79;} _LL79: if((( struct _tunion_struct*) _temp69)->tag == Cyc_Arg_Int_spec_tag){
_LL92: _temp91=( void(*)( int))(( struct Cyc_Arg_Int_spec_struct*) _temp69)->f1;
goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _tunion_struct*) _temp69)->tag
== Cyc_Arg_Rest_spec_tag){ _LL94: _temp93=( void(*)( struct _tagged_string))((
struct Cyc_Arg_Rest_spec_struct*) _temp69)->f1; goto _LL82;} else{ goto _LL70;}
_LL72: _temp83(); goto _LL70; _LL74:* _temp85= 1; goto _LL70; _LL76:* _temp87= 0;
goto _LL70; _LL78: if( Cyc_Arg_current + 1 < l){ _temp89(({ struct _tagged_ptr0
_temp95= Cyc_Arg_args; struct _tagged_string* _temp97= _temp95.curr +( Cyc_Arg_current
+ 1); if( _temp95.base == 0? 1:( _temp97 < _temp95.base? 1: _temp97 >= _temp95.last_plus_one)){
_throw( Null_Exception);}* _temp97;})); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp98=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp98[ 0]=({ struct Cyc_Arg_Missing_struct
_temp99; _temp99.tag= Cyc_Arg_Missing_tag; _temp99.f1= s; _temp99;}); _temp98;}),
speclist, errmsg);} goto _LL70; _LL80: { struct _tagged_string arg=({ struct
_tagged_ptr0 _temp115= Cyc_Arg_args; struct _tagged_string* _temp117= _temp115.curr
+( Cyc_Arg_current + 1); if( _temp115.base == 0? 1:( _temp117 < _temp115.base? 1:
_temp117 >= _temp115.last_plus_one)){ _throw( Null_Exception);}* _temp117;});
int n;{ struct _handler_cons _temp100; _push_handler(& _temp100);{ struct
_xtunion_struct* _temp101=( struct _xtunion_struct*) setjmp( _temp100.handler);
if( ! _temp101){ n= Cyc_Core_int_of_string( arg);; _pop_handler();} else{ struct
_xtunion_struct* _temp103= _temp101; struct _tagged_string _temp109; _LL105: if((*((
struct _xtunion_struct*) _temp103)).tag == Cyc_Core_InvalidArg_tag){ _LL110:
_temp109=(( struct Cyc_Core_InvalidArg_struct*) _temp103)->f1; goto _LL106;}
else{ goto _LL107;} _LL107: goto _LL108; _LL106: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Wrong_struct* _temp111=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp111[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp112; _temp112.tag= Cyc_Arg_Wrong_tag; _temp112.f1= s; _temp112.f2= arg;
_temp112.f3=( struct _tagged_string)({ char* _temp113=( char*)"an integer";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 11; _temp114;}); _temp112;}); _temp111;}),
speclist, errmsg); _npop_handler( 0u); return; _LL108:( void) _throw( _temp103);
_LL104:;}}} _temp91( n); ++ Cyc_Arg_current; goto _LL70;} _LL82: while( Cyc_Arg_current
< l - 1) { _temp93(({ struct _tagged_ptr0 _temp118= Cyc_Arg_args; struct
_tagged_string* _temp120= _temp118.curr +( Cyc_Arg_current + 1); if( _temp118.base
== 0? 1:( _temp120 < _temp118.base? 1: _temp120 >= _temp118.last_plus_one)){
_throw( Null_Exception);}* _temp120;})); ++ Cyc_Arg_current;} goto _LL70; _LL70:;};
_pop_handler();} else{ struct _xtunion_struct* _temp122= _temp68; struct
_tagged_string _temp128; _LL124: if((*(( struct _xtunion_struct*) _temp122)).tag
== Cyc_Arg_Bad_tag){ _LL129: _temp128=(( struct Cyc_Arg_Bad_struct*) _temp122)->f1;
goto _LL125;} else{ goto _LL126;} _LL126: goto _LL127; _LL125: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp130=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp130[ 0]=({ struct Cyc_Arg_Message_struct
_temp131; _temp131.tag= Cyc_Arg_Message_tag; _temp131.f1= _temp128; _temp131;});
_temp130;}), speclist, errmsg); goto _LL123; _LL127:( void) _throw( _temp122);
_LL123:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp132;
_push_handler(& _temp132);{ struct _xtunion_struct* _temp133=( struct
_xtunion_struct*) setjmp( _temp132.handler); if( ! _temp133){ anonfun( s);;
_pop_handler();} else{ struct _xtunion_struct* _temp135= _temp133; struct
_tagged_string _temp141; _LL137: if((*(( struct _xtunion_struct*) _temp135)).tag
== Cyc_Arg_Bad_tag){ _LL142: _temp141=(( struct Cyc_Arg_Bad_struct*) _temp135)->f1;
goto _LL138;} else{ goto _LL139;} _LL139: goto _LL140; _LL138: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp143=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp143[ 0]=({ struct Cyc_Arg_Message_struct
_temp144; _temp144.tag= Cyc_Arg_Message_tag; _temp144.f1= _temp141; _temp144;});
_temp143;}), speclist, errmsg); goto _LL136; _LL140:( void) _throw( _temp135);
_LL136:;}}} ++ Cyc_Arg_current;}}}}