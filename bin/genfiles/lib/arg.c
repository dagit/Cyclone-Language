 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_string); extern unsigned char* string_to_Cstring( struct
_tagged_string); extern unsigned char* underlying_Cstring( struct _tagged_string);
extern struct _tagged_string Cstring_to_string( unsigned char*); struct
_tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base; struct
_tagged_string* last_plus_one; } ; extern struct _tagged_ptr0 ntCsl_to_ntsl(
unsigned char**); extern int system( unsigned char*); extern int* __errno();
struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern int Cyc_String_strncmp( struct _tagged_string
s1, struct _tagged_string s2, unsigned int len); extern unsigned char Cyc_Arg_Bad[
8u]; struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_string f1; }
; extern unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec=
0; struct Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int
Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; static const int Cyc_Arg_FlagString_spec= 2; struct
Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct _tagged_string,
struct _tagged_string); } ; static const int Cyc_Arg_Set_spec= 3; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec= 4; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_String_spec= 5; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec=
6; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; extern void Cyc_Arg_usage( struct Cyc_List_List*,
struct _tagged_string); extern int Cyc_Arg_current; extern void Cyc_Arg_parse(
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
l, struct _tagged_string x){ while( l != 0) { struct _tagged_string _temp0=(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1; goto _LL1;
_LL1: { unsigned int _temp2= Cyc_String_strlen( _temp0); goto _LL3; _LL3: if(
_temp2 > 0?*(( const unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp0, sizeof( unsigned char),( int)(
_temp2 - 1)) =='*': 0){ if( Cyc_String_strncmp( x,(*(( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( l))->hd)).f1, _temp2 - 1) == 0){ return(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}} else{ if( Cyc_String_strcmp(
x,(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1) == 0){
return(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}}
l=(( struct Cyc_List_List*) _check_null( l))->tl;}}( void) _throw(( void*) Cyc_Core_Not_found);}
void Cyc_Arg_usage( struct Cyc_List_List* speclist, struct _tagged_string errmsg){({
struct _tagged_string _temp4= errmsg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_temp4.last_plus_one - _temp4.curr, _temp4.curr);}); while( speclist != 0) {({
struct _tagged_string _temp5=(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( speclist))->hd)).f1; struct _tagged_string _temp6=(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f3; fprintf(
sfile_to_file( Cyc_Stdio_stderr)," %.*s %.*s\n", _temp5.last_plus_one - _temp5.curr,
_temp5.curr, _temp6.last_plus_one - _temp6.curr, _temp6.curr);}); speclist=((
struct Cyc_List_List*) _check_null( speclist))->tl;}} int Cyc_Arg_current= 0;
static struct _tagged_ptr0 Cyc_Arg_args={ 0, 0, 0}; static void Cyc_Arg_stop(
int prog_pos, void* e, struct Cyc_List_List* speclist, struct _tagged_string
errmsg){ struct _tagged_string progname= prog_pos <({ struct _tagged_ptr0
_temp42= Cyc_Arg_args;( unsigned int)( _temp42.last_plus_one - _temp42.curr);})?*((
struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Arg_args, sizeof( struct _tagged_string),
prog_pos):({ unsigned char* _temp43=( unsigned char*)"(?)"; struct
_tagged_string _temp44; _temp44.curr= _temp43; _temp44.base= _temp43; _temp44.last_plus_one=
_temp43 + 4; _temp44;});{ void* _temp7= e; struct _tagged_string _temp17; struct
_tagged_string _temp19; struct _tagged_string _temp21; struct _tagged_string
_temp23; struct _tagged_string _temp25; struct _tagged_string _temp27; _LL9: if(*((
int*) _temp7) == Cyc_Arg_Unknown){ _LL18: _temp17=(( struct Cyc_Arg_Unknown_struct*)
_temp7)->f1; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp7) == Cyc_Arg_Missing){
_LL20: _temp19=(( struct Cyc_Arg_Missing_struct*) _temp7)->f1; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp7) == Cyc_Arg_Wrong){ _LL26: _temp25=((
struct Cyc_Arg_Wrong_struct*) _temp7)->f1; goto _LL24; _LL24: _temp23=(( struct
Cyc_Arg_Wrong_struct*) _temp7)->f2; goto _LL22; _LL22: _temp21=(( struct Cyc_Arg_Wrong_struct*)
_temp7)->f3; goto _LL14;} else{ goto _LL15;} _LL15: if(*(( int*) _temp7) == Cyc_Arg_Message){
_LL28: _temp27=(( struct Cyc_Arg_Message_struct*) _temp7)->f1; goto _LL16;}
else{ goto _LL8;} _LL10: if( Cyc_String_strcmp( _temp17,({ unsigned char*
_temp29=( unsigned char*)"-help"; struct _tagged_string _temp30; _temp30.curr=
_temp29; _temp30.base= _temp29; _temp30.last_plus_one= _temp29 + 6; _temp30;}))
!= 0){({ struct _tagged_string _temp31= progname; struct _tagged_string _temp32=
_temp17; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: unknown option `%.*s'.\n",
_temp31.last_plus_one - _temp31.curr, _temp31.curr, _temp32.last_plus_one -
_temp32.curr, _temp32.curr);});} goto _LL8; _LL12:({ struct _tagged_string
_temp33= progname; struct _tagged_string _temp34= _temp19; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' needs an argument.\n",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr);}); goto _LL8; _LL14:({ struct _tagged_string
_temp35= progname; struct _tagged_string _temp36= _temp23; struct _tagged_string
_temp37= _temp25; struct _tagged_string _temp38= _temp21; fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp35.last_plus_one - _temp35.curr, _temp35.curr, _temp36.last_plus_one -
_temp36.curr, _temp36.curr, _temp37.last_plus_one - _temp37.curr, _temp37.curr,
_temp38.last_plus_one - _temp38.curr, _temp38.curr);}); goto _LL8; _LL16:({
struct _tagged_string _temp39= progname; struct _tagged_string _temp40= _temp27;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s.\n", _temp39.last_plus_one
- _temp39.curr, _temp39.curr, _temp40.last_plus_one - _temp40.curr, _temp40.curr);});
goto _LL8; _LL8:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)({
struct _tagged_ptr0 _temp41= Cyc_Arg_args;( unsigned int)( _temp41.last_plus_one
- _temp41.curr);});} void Cyc_Arg_parse( struct Cyc_List_List* speclist, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 orig_args){ Cyc_Arg_args= orig_args;{ int initpos= Cyc_Arg_current;
int l=( int)({ struct _tagged_ptr0 _temp140= Cyc_Arg_args;( unsigned int)(
_temp140.last_plus_one - _temp140.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=*(( struct _tagged_string*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args,
sizeof( struct _tagged_string), Cyc_Arg_current); if(({ struct _tagged_string
_temp45= s;( unsigned int)( _temp45.last_plus_one - _temp45.curr);}) >= 1?*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), 0) =='-': 0){ void* action;{
struct _handler_cons _temp46; _push_handler(& _temp46);{ int _temp48= 0; if(
setjmp( _temp46.handler)){ _temp48= 1;} if( ! _temp48){ action= Cyc_Arg_lookup(
speclist, s);; _pop_handler();} else{ void* _temp47=( void*) _exn_thrown; void*
_temp50= _temp47; _LL52: if( _temp50 == Cyc_Core_Not_found){ goto _LL53;} else{
goto _LL54;} _LL54: goto _LL55; _LL53: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Unknown_struct* _temp56=( struct Cyc_Arg_Unknown_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Unknown_struct)); _temp56[ 0]=({ struct Cyc_Arg_Unknown_struct
_temp57; _temp57.tag= Cyc_Arg_Unknown; _temp57.f1= s; _temp57;}); _temp56;}),
speclist, errmsg); return; _LL55:( void) _throw( _temp50); _LL51:;}}}{ struct
_handler_cons _temp58; _push_handler(& _temp58);{ int _temp60= 0; if( setjmp(
_temp58.handler)){ _temp60= 1;} if( ! _temp60){{ void* _temp61= action; void(*
_temp79)(); void(* _temp81)( struct _tagged_string); int* _temp83; int* _temp85;
void(* _temp87)( struct _tagged_string, struct _tagged_string); void(* _temp89)(
struct _tagged_string); void(* _temp91)( int); void(* _temp93)( struct
_tagged_string); _LL63: if(*(( int*) _temp61) == Cyc_Arg_Unit_spec){ _LL80:
_temp79=(( struct Cyc_Arg_Unit_spec_struct*) _temp61)->f1; goto _LL64;} else{
goto _LL65;} _LL65: if(*(( int*) _temp61) == Cyc_Arg_Flag_spec){ _LL82: _temp81=((
struct Cyc_Arg_Flag_spec_struct*) _temp61)->f1; goto _LL66;} else{ goto _LL67;}
_LL67: if(*(( int*) _temp61) == Cyc_Arg_Set_spec){ _LL84: _temp83=(( struct Cyc_Arg_Set_spec_struct*)
_temp61)->f1; goto _LL68;} else{ goto _LL69;} _LL69: if(*(( int*) _temp61) ==
Cyc_Arg_Clear_spec){ _LL86: _temp85=(( struct Cyc_Arg_Clear_spec_struct*)
_temp61)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp61) ==
Cyc_Arg_FlagString_spec){ _LL88: _temp87=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp61)->f1; goto _LL72;} else{ goto _LL73;} _LL73: if(*(( int*) _temp61) ==
Cyc_Arg_String_spec){ _LL90: _temp89=(( struct Cyc_Arg_String_spec_struct*)
_temp61)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if(*(( int*) _temp61) ==
Cyc_Arg_Int_spec){ _LL92: _temp91=(( struct Cyc_Arg_Int_spec_struct*) _temp61)->f1;
goto _LL76;} else{ goto _LL77;} _LL77: if(*(( int*) _temp61) == Cyc_Arg_Rest_spec){
_LL94: _temp93=(( struct Cyc_Arg_Rest_spec_struct*) _temp61)->f1; goto _LL78;}
else{ goto _LL62;} _LL64: _temp79(); goto _LL62; _LL66: _temp81( s); goto _LL62;
_LL68:* _temp83= 1; goto _LL62; _LL70:* _temp85= 0; goto _LL62; _LL72: if( Cyc_Arg_current
+ 1 < l){ _temp87( s,*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args, sizeof(
struct _tagged_string), Cyc_Arg_current + 1)); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp95=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp95[ 0]=({ struct Cyc_Arg_Missing_struct
_temp96; _temp96.tag= Cyc_Arg_Missing; _temp96.f1= s; _temp96;}); _temp95;}),
speclist, errmsg);} goto _LL62; _LL74: if( Cyc_Arg_current + 1 < l){ _temp89(*((
struct _tagged_string*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Arg_args, sizeof( struct _tagged_string), Cyc_Arg_current
+ 1)); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp97=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp97[ 0]=({ struct Cyc_Arg_Missing_struct _temp98; _temp98.tag= Cyc_Arg_Missing;
_temp98.f1= s; _temp98;}); _temp97;}), speclist, errmsg);} goto _LL62; _LL76: {
struct _tagged_string arg=*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args, sizeof(
struct _tagged_string), Cyc_Arg_current + 1); int n;{ struct _handler_cons
_temp99; _push_handler(& _temp99);{ int _temp101= 0; if( setjmp( _temp99.handler)){
_temp101= 1;} if( ! _temp101){ n= Cyc_Core_int_of_string( arg);; _pop_handler();}
else{ void* _temp100=( void*) _exn_thrown; void* _temp103= _temp100; struct
_tagged_string _temp109; _LL105: if(*(( void**) _temp103) == Cyc_Core_InvalidArg){
_LL110: _temp109=(( struct Cyc_Core_InvalidArg_struct*) _temp103)->f1; goto
_LL106;} else{ goto _LL107;} _LL107: goto _LL108; _LL106: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Wrong_struct* _temp111=( struct Cyc_Arg_Wrong_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct)); _temp111[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp112; _temp112.tag= Cyc_Arg_Wrong; _temp112.f1= s; _temp112.f2= arg;
_temp112.f3=({ unsigned char* _temp113=( unsigned char*)"an integer"; struct
_tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 11; _temp114;}); _temp112;}); _temp111;}),
speclist, errmsg); _npop_handler( 0u); return; _LL108:( void) _throw( _temp103);
_LL104:;}}} _temp91( n); ++ Cyc_Arg_current; goto _LL62;} _LL78: while( Cyc_Arg_current
< l - 1) { _temp93(*(( struct _tagged_string*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Arg_args, sizeof(
struct _tagged_string), Cyc_Arg_current + 1)); ++ Cyc_Arg_current;} goto _LL62;
_LL62:;}; _pop_handler();} else{ void* _temp59=( void*) _exn_thrown; void*
_temp116= _temp59; struct _tagged_string _temp122; _LL118: if(*(( void**)
_temp116) == Cyc_Arg_Bad){ _LL123: _temp122=(( struct Cyc_Arg_Bad_struct*)
_temp116)->f1; goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119:
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp124=(
struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp124[ 0]=({ struct Cyc_Arg_Message_struct _temp125; _temp125.tag= Cyc_Arg_Message;
_temp125.f1= _temp122; _temp125;}); _temp124;}), speclist, errmsg); goto _LL117;
_LL121:( void) _throw( _temp116); _LL117:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp126; _push_handler(& _temp126);{ int _temp128= 0; if( setjmp(
_temp126.handler)){ _temp128= 1;} if( ! _temp128){ anonfun( s);; _pop_handler();}
else{ void* _temp127=( void*) _exn_thrown; void* _temp130= _temp127; struct
_tagged_string _temp136; _LL132: if(*(( void**) _temp130) == Cyc_Arg_Bad){
_LL137: _temp136=(( struct Cyc_Arg_Bad_struct*) _temp130)->f1; goto _LL133;}
else{ goto _LL134;} _LL134: goto _LL135; _LL133: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Message_struct* _temp138=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp138[ 0]=({ struct Cyc_Arg_Message_struct
_temp139; _temp139.tag= Cyc_Arg_Message; _temp139.f1= _temp136; _temp139;});
_temp138;}), speclist, errmsg); goto _LL131; _LL135:( void) _throw( _temp130);
_LL131:;}}} ++ Cyc_Arg_current;}}}}