#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_string); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); extern int
system( unsigned char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern unsigned char Cyc_Arg_Bad[
8u]; struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_string f1; }
; extern unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec=
0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int
Cyc_Arg_Set_spec= 1; struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ;
static const int Cyc_Arg_Clear_spec= 2; struct Cyc_Arg_Clear_spec_struct{ int
tag; int* f1; } ; static const int Cyc_Arg_String_spec= 3; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec=
4; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 5; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void* Cyc_Arg_spec_t;
extern void Cyc_Arg_usage( struct Cyc_List_List*, struct _tagged_string); extern
int Cyc_Arg_current; struct _tagged_ptr0{ struct _tagged_string* curr; struct
_tagged_string* base; struct _tagged_string* last_plus_one; } ; extern void Cyc_Arg_parse(
struct Cyc_List_List* specs, void(* anonfun)( struct _tagged_string), struct
_tagged_string errmsg, struct _tagged_ptr0 args); unsigned char Cyc_Arg_Bad[ 8u]="\000\000\000\000Bad";
unsigned char Cyc_Arg_Error[ 10u]="\000\000\000\000Error"; static const int Cyc_Arg_Unknown=
0; struct Cyc_Arg_Unknown_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Arg_Missing= 1; struct Cyc_Arg_Missing_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Arg_Message= 2; struct Cyc_Arg_Message_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Arg_Wrong= 3; struct
Cyc_Arg_Wrong_struct{ int tag; struct _tagged_string f1; struct _tagged_string
f2; struct _tagged_string f3; } ; struct _tuple0{ struct _tagged_string f1; void*
f2; struct _tagged_string f3; } ; static void* Cyc_Arg_lookup( struct Cyc_List_List*
l, struct _tagged_string x){ while( l != 0) { if( Cyc_String_strcmp( x,(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1) == 0){
return(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}
l=(( struct Cyc_List_List*) _check_null( l))->tl;}( void) _throw(( void*) Cyc_Core_Not_found);}
void Cyc_Arg_usage( struct Cyc_List_List* speclist, struct _tagged_string errmsg){({
struct _tagged_string _temp0= errmsg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); while( speclist != 0) {({
struct _tagged_string _temp1=(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( speclist))->hd)).f1; struct _tagged_string _temp2=(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f3; fprintf(
sfile_to_file( Cyc_Stdio_stderr)," %.*s %.*s\n", _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);}); speclist=((
struct Cyc_List_List*) _check_null( speclist))->tl;}} int Cyc_Arg_current= 0;
static struct _tagged_ptr0 Cyc_Arg_args={ 0, 0, 0}; static void Cyc_Arg_stop(
int prog_pos, void* e, struct Cyc_List_List* speclist, struct _tagged_string
errmsg){ struct _tagged_string progname= prog_pos <({ struct _tagged_ptr0
_temp38= Cyc_Arg_args;( unsigned int)( _temp38.last_plus_one - _temp38.curr);})?*((
struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Arg_args, sizeof( struct _tagged_string),
prog_pos):({ unsigned char* _temp39="(?)"; struct _tagged_string _temp40;
_temp40.curr= _temp39; _temp40.base= _temp39; _temp40.last_plus_one= _temp39 + 4;
_temp40;});{ void* _temp3= e; struct _tagged_string _temp13; struct
_tagged_string _temp15; struct _tagged_string _temp17; struct _tagged_string
_temp19; struct _tagged_string _temp21; struct _tagged_string _temp23; _LL5: if(*((
int*) _temp3) == Cyc_Arg_Unknown){ _LL14: _temp13=(( struct Cyc_Arg_Unknown_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: if(*(( int*) _temp3) == Cyc_Arg_Missing){
_LL16: _temp15=(( struct Cyc_Arg_Missing_struct*) _temp3)->f1; goto _LL8;} else{
goto _LL9;} _LL9: if(*(( int*) _temp3) == Cyc_Arg_Wrong){ _LL22: _temp21=((
struct Cyc_Arg_Wrong_struct*) _temp3)->f1; goto _LL20; _LL20: _temp19=(( struct
Cyc_Arg_Wrong_struct*) _temp3)->f2; goto _LL18; _LL18: _temp17=(( struct Cyc_Arg_Wrong_struct*)
_temp3)->f3; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp3) == Cyc_Arg_Message){
_LL24: _temp23=(( struct Cyc_Arg_Message_struct*) _temp3)->f1; goto _LL12;}
else{ goto _LL4;} _LL6: if( Cyc_String_strcmp( _temp13,({ unsigned char* _temp25="-help";
struct _tagged_string _temp26; _temp26.curr= _temp25; _temp26.base= _temp25;
_temp26.last_plus_one= _temp25 + 6; _temp26;})) != 0){({ struct _tagged_string
_temp27= progname; struct _tagged_string _temp28= _temp13; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: unknown option `%.*s'.\n", _temp27.last_plus_one
- _temp27.curr, _temp27.curr, _temp28.last_plus_one - _temp28.curr, _temp28.curr);});}
goto _LL4; _LL8:({ struct _tagged_string _temp29= progname; struct
_tagged_string _temp30= _temp15; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' needs an argument.\n",
_temp29.last_plus_one - _temp29.curr, _temp29.curr, _temp30.last_plus_one -
_temp30.curr, _temp30.curr);}); goto _LL4; _LL10:({ struct _tagged_string
_temp31= progname; struct _tagged_string _temp32= _temp19; struct _tagged_string
_temp33= _temp21; struct _tagged_string _temp34= _temp17; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp31.last_plus_one - _temp31.curr, _temp31.curr, _temp32.last_plus_one -
_temp32.curr, _temp32.curr, _temp33.last_plus_one - _temp33.curr, _temp33.curr,
_temp34.last_plus_one - _temp34.curr, _temp34.curr);}); goto _LL4; _LL12:({
struct _tagged_string _temp35= progname; struct _tagged_string _temp36= _temp23;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s.\n", _temp35.last_plus_one
- _temp35.curr, _temp35.curr, _temp36.last_plus_one - _temp36.curr, _temp36.curr);});
goto _LL4; _LL4:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)({
struct _tagged_ptr0 _temp37= Cyc_Arg_args;( unsigned int)( _temp37.last_plus_one
- _temp37.curr);});} void Cyc_Arg_parse( struct Cyc_List_List* speclist, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 orig_args){ Cyc_Arg_args= orig_args;{ int initpos= Cyc_Arg_current;
int l=( int)({ struct _tagged_ptr0 _temp126= Cyc_Arg_args;( unsigned int)(
_temp126.last_plus_one - _temp126.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args,
sizeof( struct _tagged_string), Cyc_Arg_current); if(({ struct _tagged_string
_temp41= s;( unsigned int)( _temp41.last_plus_one - _temp41.curr);}) >= 1?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), 0) =='-': 0){ void* action;{
struct _handler_cons _temp42; _push_handler(& _temp42);{ int _temp44= 0; if(
setjmp( _temp42.handler)){ _temp44= 1;} if( ! _temp44){ action= Cyc_Arg_lookup(
speclist, s);; _pop_handler();} else{ void* _temp43=( void*) _exn_thrown; void*
_temp46= _temp43; _LL48: if( _temp46 == Cyc_Core_Not_found){ goto _LL49;} else{
goto _LL50;} _LL50: goto _LL51; _LL49: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Unknown_struct* _temp52=( struct Cyc_Arg_Unknown_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Unknown_struct)); _temp52[ 0]=({ struct Cyc_Arg_Unknown_struct
_temp53; _temp53.tag= Cyc_Arg_Unknown; _temp53.f1= s; _temp53;}); _temp52;}),
speclist, errmsg); return; _LL51:( void) _throw( _temp46); _LL47:;}}}{ struct
_handler_cons _temp54; _push_handler(& _temp54);{ int _temp56= 0; if( setjmp(
_temp54.handler)){ _temp56= 1;} if( ! _temp56){{ void* _temp57= action; void(*
_temp71)(); int* _temp73; int* _temp75; void(* _temp77)( struct _tagged_string);
void(* _temp79)( int); void(* _temp81)( struct _tagged_string); _LL59: if(*((
int*) _temp57) == Cyc_Arg_Unit_spec){ _LL72: _temp71=(( struct Cyc_Arg_Unit_spec_struct*)
_temp57)->f1; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp57) ==
Cyc_Arg_Set_spec){ _LL74: _temp73=(( struct Cyc_Arg_Set_spec_struct*) _temp57)->f1;
goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp57) == Cyc_Arg_Clear_spec){
_LL76: _temp75=(( struct Cyc_Arg_Clear_spec_struct*) _temp57)->f1; goto _LL64;}
else{ goto _LL65;} _LL65: if(*(( int*) _temp57) == Cyc_Arg_String_spec){ _LL78:
_temp77=(( struct Cyc_Arg_String_spec_struct*) _temp57)->f1; goto _LL66;} else{
goto _LL67;} _LL67: if(*(( int*) _temp57) == Cyc_Arg_Int_spec){ _LL80: _temp79=((
struct Cyc_Arg_Int_spec_struct*) _temp57)->f1; goto _LL68;} else{ goto _LL69;}
_LL69: if(*(( int*) _temp57) == Cyc_Arg_Rest_spec){ _LL82: _temp81=(( struct Cyc_Arg_Rest_spec_struct*)
_temp57)->f1; goto _LL70;} else{ goto _LL58;} _LL60: _temp71(); goto _LL58;
_LL62:* _temp73= 1; goto _LL58; _LL64:* _temp75= 0; goto _LL58; _LL66: if( Cyc_Arg_current
+ 1 < l){ _temp77(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args, sizeof(
struct _tagged_string), Cyc_Arg_current + 1)); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp83=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp83[ 0]=({ struct Cyc_Arg_Missing_struct
_temp84; _temp84.tag= Cyc_Arg_Missing; _temp84.f1= s; _temp84;}); _temp83;}),
speclist, errmsg);} goto _LL58; _LL68: { struct _tagged_string arg=*(( struct
_tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Arg_args, sizeof( struct _tagged_string), Cyc_Arg_current
+ 1); int n;{ struct _handler_cons _temp85; _push_handler(& _temp85);{ int
_temp87= 0; if( setjmp( _temp85.handler)){ _temp87= 1;} if( ! _temp87){ n= Cyc_Core_int_of_string(
arg);; _pop_handler();} else{ void* _temp86=( void*) _exn_thrown; void* _temp89=
_temp86; struct _tagged_string _temp95; _LL91: if(*(( void**) _temp89) == Cyc_Core_InvalidArg){
_LL96: _temp95=(( struct Cyc_Core_InvalidArg_struct*) _temp89)->f1; goto _LL92;}
else{ goto _LL93;} _LL93: goto _LL94; _LL92: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Wrong_struct* _temp97=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp97[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp98; _temp98.tag= Cyc_Arg_Wrong; _temp98.f1= s; _temp98.f2= arg; _temp98.f3=({
unsigned char* _temp99="an integer"; struct _tagged_string _temp100; _temp100.curr=
_temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99 + 11; _temp100;});
_temp98;}); _temp97;}), speclist, errmsg); _npop_handler( 0u); return; _LL94:(
void) _throw( _temp89); _LL90:;}}} _temp79( n); ++ Cyc_Arg_current; goto _LL58;}
_LL70: while( Cyc_Arg_current < l - 1) { _temp81(*(( struct _tagged_string*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args,
sizeof( struct _tagged_string), Cyc_Arg_current + 1)); ++ Cyc_Arg_current;} goto
_LL58; _LL58:;}; _pop_handler();} else{ void* _temp55=( void*) _exn_thrown; void*
_temp102= _temp55; struct _tagged_string _temp108; _LL104: if(*(( void**)
_temp102) == Cyc_Arg_Bad){ _LL109: _temp108=(( struct Cyc_Arg_Bad_struct*)
_temp102)->f1; goto _LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL105:
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp110=(
struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp110[ 0]=({ struct Cyc_Arg_Message_struct _temp111; _temp111.tag= Cyc_Arg_Message;
_temp111.f1= _temp108; _temp111;}); _temp110;}), speclist, errmsg); goto _LL103;
_LL107:( void) _throw( _temp102); _LL103:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp112; _push_handler(& _temp112);{ int _temp114= 0; if( setjmp(
_temp112.handler)){ _temp114= 1;} if( ! _temp114){ anonfun( s);; _pop_handler();}
else{ void* _temp113=( void*) _exn_thrown; void* _temp116= _temp113; struct
_tagged_string _temp122; _LL118: if(*(( void**) _temp116) == Cyc_Arg_Bad){
_LL123: _temp122=(( struct Cyc_Arg_Bad_struct*) _temp116)->f1; goto _LL119;}
else{ goto _LL120;} _LL120: goto _LL121; _LL119: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Message_struct* _temp124=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp124[ 0]=({ struct Cyc_Arg_Message_struct
_temp125; _temp125.tag= Cyc_Arg_Message; _temp125.f1= _temp122; _temp125;});
_temp124;}), speclist, errmsg); goto _LL117; _LL121:( void) _throw( _temp116);
_LL117:;}}} ++ Cyc_Arg_current;}}}}