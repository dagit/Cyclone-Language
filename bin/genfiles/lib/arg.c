 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_arr); extern unsigned char* string_to_Cstring( struct _tagged_arr);
extern unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern int Cyc_String_strncmp( struct _tagged_arr s1,
struct _tagged_arr s2, int len); extern unsigned char Cyc_Arg_Bad[ 8u]; struct
Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_arr f1; } ; extern
unsigned char Cyc_Arg_Error[ 10u]; static const int Cyc_Arg_Unit_spec= 0; struct
Cyc_Arg_Unit_spec_struct{ int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec=
1; struct Cyc_Arg_Flag_spec_struct{ int tag; void(* f1)( struct _tagged_arr); }
; static const int Cyc_Arg_FlagString_spec= 2; struct Cyc_Arg_FlagString_spec_struct{
int tag; void(* f1)( struct _tagged_arr, struct _tagged_arr); } ; static const
int Cyc_Arg_Set_spec= 3; struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ;
static const int Cyc_Arg_Clear_spec= 4; struct Cyc_Arg_Clear_spec_struct{ int
tag; int* f1; } ; static const int Cyc_Arg_String_spec= 5; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_Int_spec=
6; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_arr); } ; extern void Cyc_Arg_usage( struct Cyc_List_List*,
struct _tagged_arr); extern int Cyc_Arg_current; extern void Cyc_Arg_parse(
struct Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr args); unsigned char Cyc_Arg_Bad[ 8u]="\000\000\000\000Bad";
unsigned char Cyc_Arg_Error[ 10u]="\000\000\000\000Error"; static const int Cyc_Arg_Unknown=
0; struct Cyc_Arg_Unknown_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Arg_Missing= 1; struct Cyc_Arg_Missing_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Arg_Message= 2; struct Cyc_Arg_Message_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Arg_Wrong= 3; struct
Cyc_Arg_Wrong_struct{ int tag; struct _tagged_arr f1; struct _tagged_arr f2;
struct _tagged_arr f3; } ; struct _tuple0{ struct _tagged_arr f1; void* f2;
struct _tagged_arr f3; } ; static void* Cyc_Arg_lookup( struct Cyc_List_List* l,
struct _tagged_arr x){ while( l != 0) { struct _tagged_arr _temp0=(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1; int _temp1= Cyc_String_strlen(
_temp0); if( _temp1 > 0?*(( const unsigned char*) _check_unknown_subscript(
_temp0, sizeof( unsigned char), _temp1 - 1)) =='*': 0){ if( Cyc_String_strncmp(
x,(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1,
_temp1 - 1) == 0){ return(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( l))->hd)).f2;}} else{ if( Cyc_String_strcmp( x,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( l))->hd)).f1) == 0){ return(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}} l=(( struct Cyc_List_List*)
_check_null( l))->tl;}( int) _throw(( void*) Cyc_Core_Not_found);} void Cyc_Arg_usage(
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){({ struct _tagged_arr
_temp2= errmsg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_get_arr_size( _temp2, 1u), _temp2.curr);}); while( speclist != 0) {({ struct
_tagged_arr _temp3=(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null(
speclist))->hd)).f1; struct _tagged_arr _temp4=(*(( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( speclist))->hd)).f3; fprintf( _sfile_to_file( Cyc_Stdio_stderr)," %.*s %.*s\n",
_get_arr_size( _temp3, 1u), _temp3.curr, _get_arr_size( _temp4, 1u), _temp4.curr);});
speclist=(( struct Cyc_List_List*) _check_null( speclist))->tl;}} int Cyc_Arg_current=
0; static struct _tagged_arr Cyc_Arg_args={( void*) 0u,( void*) 0u,( void*) 0u +
0u}; static void Cyc_Arg_stop( int prog_pos, void* e, struct Cyc_List_List*
speclist, struct _tagged_arr errmsg){ struct _tagged_arr progname= prog_pos <
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr))?*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), prog_pos)):
_tag_arr("(?)", sizeof( unsigned char), 4u);{ void* _temp5= e; struct
_tagged_arr _temp15; struct _tagged_arr _temp17; struct _tagged_arr _temp19;
struct _tagged_arr _temp21; struct _tagged_arr _temp23; struct _tagged_arr
_temp25; _LL7: if(*(( int*) _temp5) == Cyc_Arg_Unknown){ _LL16: _temp15=((
struct Cyc_Arg_Unknown_struct*) _temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9:
if(*(( int*) _temp5) == Cyc_Arg_Missing){ _LL18: _temp17=(( struct Cyc_Arg_Missing_struct*)
_temp5)->f1; goto _LL10;} else{ goto _LL11;} _LL11: if(*(( int*) _temp5) == Cyc_Arg_Wrong){
_LL24: _temp23=(( struct Cyc_Arg_Wrong_struct*) _temp5)->f1; goto _LL22; _LL22:
_temp21=(( struct Cyc_Arg_Wrong_struct*) _temp5)->f2; goto _LL20; _LL20: _temp19=((
struct Cyc_Arg_Wrong_struct*) _temp5)->f3; goto _LL12;} else{ goto _LL13;} _LL13:
if(*(( int*) _temp5) == Cyc_Arg_Message){ _LL26: _temp25=(( struct Cyc_Arg_Message_struct*)
_temp5)->f1; goto _LL14;} else{ goto _LL6;} _LL8: if( Cyc_String_strcmp( _temp15,
_tag_arr("-help", sizeof( unsigned char), 6u)) != 0){({ struct _tagged_arr
_temp27= progname; struct _tagged_arr _temp28= _temp15; fprintf( _sfile_to_file(
Cyc_Stdio_stderr),"%.*s: unknown option `%.*s'.\n", _get_arr_size( _temp27, 1u),
_temp27.curr, _get_arr_size( _temp28, 1u), _temp28.curr);});} goto _LL6; _LL10:({
struct _tagged_arr _temp29= progname; struct _tagged_arr _temp30= _temp17;
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' needs an argument.\n",
_get_arr_size( _temp29, 1u), _temp29.curr, _get_arr_size( _temp30, 1u), _temp30.curr);});
goto _LL6; _LL12:({ struct _tagged_arr _temp31= progname; struct _tagged_arr
_temp32= _temp21; struct _tagged_arr _temp33= _temp23; struct _tagged_arr
_temp34= _temp19; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_get_arr_size( _temp31, 1u), _temp31.curr, _get_arr_size( _temp32, 1u), _temp32.curr,
_get_arr_size( _temp33, 1u), _temp33.curr, _get_arr_size( _temp34, 1u), _temp34.curr);});
goto _LL6; _LL14:({ struct _tagged_arr _temp35= progname; struct _tagged_arr
_temp36= _temp25; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s.\n",
_get_arr_size( _temp35, 1u), _temp35.curr, _get_arr_size( _temp36, 1u), _temp36.curr);});
goto _LL6; _LL6:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current < l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >= 1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) =='-': 0){ void* action;{ struct _handler_cons
_temp37; _push_handler(& _temp37);{ int _temp39= 0; if( setjmp( _temp37.handler)){
_temp39= 1;} if( ! _temp39){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp38=( void*) _exn_thrown; void* _temp41= _temp38; _LL43: if(
_temp41 == Cyc_Core_Not_found){ goto _LL44;} else{ goto _LL45;} _LL45: goto
_LL46; _LL44: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp47=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp47[ 0]=({ struct Cyc_Arg_Unknown_struct _temp48; _temp48.tag= Cyc_Arg_Unknown;
_temp48.f1= s; _temp48;}); _temp47;}), speclist, errmsg); return; _LL46:( void)
_throw( _temp41); _LL42:;}}}{ struct _handler_cons _temp49; _push_handler(&
_temp49);{ int _temp51= 0; if( setjmp( _temp49.handler)){ _temp51= 1;} if( !
_temp51){{ void* _temp52= action; void(* _temp70)(); void(* _temp72)( struct
_tagged_arr); int* _temp74; int* _temp76; void(* _temp78)( struct _tagged_arr,
struct _tagged_arr); void(* _temp80)( struct _tagged_arr); void(* _temp82)( int);
void(* _temp84)( struct _tagged_arr); _LL54: if(*(( int*) _temp52) == Cyc_Arg_Unit_spec){
_LL71: _temp70=(( struct Cyc_Arg_Unit_spec_struct*) _temp52)->f1; goto _LL55;}
else{ goto _LL56;} _LL56: if(*(( int*) _temp52) == Cyc_Arg_Flag_spec){ _LL73:
_temp72=(( struct Cyc_Arg_Flag_spec_struct*) _temp52)->f1; goto _LL57;} else{
goto _LL58;} _LL58: if(*(( int*) _temp52) == Cyc_Arg_Set_spec){ _LL75: _temp74=((
struct Cyc_Arg_Set_spec_struct*) _temp52)->f1; goto _LL59;} else{ goto _LL60;}
_LL60: if(*(( int*) _temp52) == Cyc_Arg_Clear_spec){ _LL77: _temp76=(( struct
Cyc_Arg_Clear_spec_struct*) _temp52)->f1; goto _LL61;} else{ goto _LL62;} _LL62:
if(*(( int*) _temp52) == Cyc_Arg_FlagString_spec){ _LL79: _temp78=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp52)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp52) ==
Cyc_Arg_String_spec){ _LL81: _temp80=(( struct Cyc_Arg_String_spec_struct*)
_temp52)->f1; goto _LL65;} else{ goto _LL66;} _LL66: if(*(( int*) _temp52) ==
Cyc_Arg_Int_spec){ _LL83: _temp82=(( struct Cyc_Arg_Int_spec_struct*) _temp52)->f1;
goto _LL67;} else{ goto _LL68;} _LL68: if(*(( int*) _temp52) == Cyc_Arg_Rest_spec){
_LL85: _temp84=(( struct Cyc_Arg_Rest_spec_struct*) _temp52)->f1; goto _LL69;}
else{ goto _LL53;} _LL55: _temp70(); goto _LL53; _LL57: _temp72( s); goto _LL53;
_LL59:* _temp74= 1; goto _LL53; _LL61:* _temp76= 0; goto _LL53; _LL63: if( Cyc_Arg_current
+ 1 < l){ _temp78( s,*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1))); ++ Cyc_Arg_current;} else{
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp86=( struct
Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp86[ 0]=({ struct Cyc_Arg_Missing_struct _temp87; _temp87.tag= Cyc_Arg_Missing;
_temp87.f1= s; _temp87;}); _temp86;}), speclist, errmsg);} goto _LL53; _LL65:
if( Cyc_Arg_current + 1 < l){ _temp80(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp88=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp88[ 0]=({ struct Cyc_Arg_Missing_struct _temp89; _temp89.tag= Cyc_Arg_Missing;
_temp89.f1= s; _temp89;}); _temp88;}), speclist, errmsg);} goto _LL53; _LL67: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1)); int n;{ struct _handler_cons
_temp90; _push_handler(& _temp90);{ int _temp92= 0; if( setjmp( _temp90.handler)){
_temp92= 1;} if( ! _temp92){ n= Cyc_Core_int_of_string( arg);; _pop_handler();}
else{ void* _temp91=( void*) _exn_thrown; void* _temp94= _temp91; struct
_tagged_arr _temp100; _LL96: if(*(( void**) _temp94) == Cyc_Core_InvalidArg){
_LL101: _temp100=(( struct Cyc_Core_InvalidArg_struct*) _temp94)->f1; goto _LL97;}
else{ goto _LL98;} _LL98: goto _LL99; _LL97: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Wrong_struct* _temp102=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp102[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp103; _temp103.tag= Cyc_Arg_Wrong; _temp103.f1= s; _temp103.f2= arg;
_temp103.f3= _tag_arr("an integer", sizeof( unsigned char), 11u); _temp103;});
_temp102;}), speclist, errmsg); _npop_handler( 0u); return; _LL99:( void) _throw(
_temp94); _LL95:;}}} _temp82( n); ++ Cyc_Arg_current; goto _LL53;} _LL69: while(
Cyc_Arg_current < l - 1) { _temp84(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} goto _LL53; _LL53:;}; _pop_handler();} else{ void*
_temp50=( void*) _exn_thrown; void* _temp105= _temp50; struct _tagged_arr
_temp111; _LL107: if(*(( void**) _temp105) == Cyc_Arg_Bad){ _LL112: _temp111=((
struct Cyc_Arg_Bad_struct*) _temp105)->f1; goto _LL108;} else{ goto _LL109;}
_LL109: goto _LL110; _LL108: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp113=( struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp113[ 0]=({ struct Cyc_Arg_Message_struct _temp114; _temp114.tag= Cyc_Arg_Message;
_temp114.f1= _temp111; _temp114;}); _temp113;}), speclist, errmsg); goto _LL106;
_LL110:( void) _throw( _temp105); _LL106:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp115; _push_handler(& _temp115);{ int _temp117= 0; if( setjmp(
_temp115.handler)){ _temp117= 1;} if( ! _temp117){ anonfun( s);; _pop_handler();}
else{ void* _temp116=( void*) _exn_thrown; void* _temp119= _temp116; struct
_tagged_arr _temp125; _LL121: if(*(( void**) _temp119) == Cyc_Arg_Bad){ _LL126:
_temp125=(( struct Cyc_Arg_Bad_struct*) _temp119)->f1; goto _LL122;} else{ goto
_LL123;} _LL123: goto _LL124; _LL122: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp127=( struct Cyc_Arg_Message_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp127[ 0]=({ struct Cyc_Arg_Message_struct
_temp128; _temp128.tag= Cyc_Arg_Message; _temp128.f1= _temp125; _temp128;});
_temp127;}), speclist, errmsg); goto _LL120; _LL124:( void) _throw( _temp119);
_LL120:;}}} ++ Cyc_Arg_current;}}}}