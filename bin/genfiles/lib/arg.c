 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1; goto _LL1; _LL1: {
int _temp2= Cyc_String_strlen( _temp0); goto _LL3; _LL3: if( _temp2 > 0?*((
const unsigned char*) _check_unknown_subscript( _temp0, sizeof( unsigned char),
_temp2 - 1)) =='*': 0){ if( Cyc_String_strncmp( x,(*(( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( l))->hd)).f1, _temp2 - 1) == 0){ return(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}} else{ if( Cyc_String_strcmp(
x,(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1) == 0){
return(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f2;}}
l=(( struct Cyc_List_List*) _check_null( l))->tl;}}( int) _throw(( void*) Cyc_Core_Not_found);}
void Cyc_Arg_usage( struct Cyc_List_List* speclist, struct _tagged_arr errmsg){({
struct _tagged_arr _temp4= errmsg; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s\n",
_get_arr_size( _temp4, 1u), _temp4.curr);}); while( speclist != 0) {({ struct
_tagged_arr _temp5=(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null(
speclist))->hd)).f1; struct _tagged_arr _temp6=(*(( struct _tuple0*)(( struct
Cyc_List_List*) _check_null( speclist))->hd)).f3; fprintf( _sfile_to_file( Cyc_Stdio_stderr)," %.*s %.*s\n",
_get_arr_size( _temp5, 1u), _temp5.curr, _get_arr_size( _temp6, 1u), _temp6.curr);});
speclist=(( struct Cyc_List_List*) _check_null( speclist))->tl;}} int Cyc_Arg_current=
0; static struct _tagged_arr Cyc_Arg_args={( void*) 0u,( void*) 0u,( void*) 0u +
0u}; static void Cyc_Arg_stop( int prog_pos, void* e, struct Cyc_List_List*
speclist, struct _tagged_arr errmsg){ struct _tagged_arr progname= prog_pos <
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr))?*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), prog_pos)):
_tag_arr("(?)", sizeof( unsigned char), 4u);{ void* _temp7= e; struct
_tagged_arr _temp17; struct _tagged_arr _temp19; struct _tagged_arr _temp21;
struct _tagged_arr _temp23; struct _tagged_arr _temp25; struct _tagged_arr
_temp27; _LL9: if(*(( int*) _temp7) == Cyc_Arg_Unknown){ _LL18: _temp17=((
struct Cyc_Arg_Unknown_struct*) _temp7)->f1; goto _LL10;} else{ goto _LL11;}
_LL11: if(*(( int*) _temp7) == Cyc_Arg_Missing){ _LL20: _temp19=(( struct Cyc_Arg_Missing_struct*)
_temp7)->f1; goto _LL12;} else{ goto _LL13;} _LL13: if(*(( int*) _temp7) == Cyc_Arg_Wrong){
_LL26: _temp25=(( struct Cyc_Arg_Wrong_struct*) _temp7)->f1; goto _LL24; _LL24:
_temp23=(( struct Cyc_Arg_Wrong_struct*) _temp7)->f2; goto _LL22; _LL22: _temp21=((
struct Cyc_Arg_Wrong_struct*) _temp7)->f3; goto _LL14;} else{ goto _LL15;} _LL15:
if(*(( int*) _temp7) == Cyc_Arg_Message){ _LL28: _temp27=(( struct Cyc_Arg_Message_struct*)
_temp7)->f1; goto _LL16;} else{ goto _LL8;} _LL10: if( Cyc_String_strcmp(
_temp17, _tag_arr("-help", sizeof( unsigned char), 6u)) != 0){({ struct
_tagged_arr _temp29= progname; struct _tagged_arr _temp30= _temp17; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"%.*s: unknown option `%.*s'.\n",
_get_arr_size( _temp29, 1u), _temp29.curr, _get_arr_size( _temp30, 1u), _temp30.curr);});}
goto _LL8; _LL12:({ struct _tagged_arr _temp31= progname; struct _tagged_arr
_temp32= _temp19; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: option `%.*s' needs an argument.\n",
_get_arr_size( _temp31, 1u), _temp31.curr, _get_arr_size( _temp32, 1u), _temp32.curr);});
goto _LL8; _LL14:({ struct _tagged_arr _temp33= progname; struct _tagged_arr
_temp34= _temp23; struct _tagged_arr _temp35= _temp25; struct _tagged_arr
_temp36= _temp21; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_get_arr_size( _temp33, 1u), _temp33.curr, _get_arr_size( _temp34, 1u), _temp34.curr,
_get_arr_size( _temp35, 1u), _temp35.curr, _get_arr_size( _temp36, 1u), _temp36.curr);});
goto _LL8; _LL16:({ struct _tagged_arr _temp37= progname; struct _tagged_arr
_temp38= _temp27; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s.\n",
_get_arr_size( _temp37, 1u), _temp37.curr, _get_arr_size( _temp38, 1u), _temp38.curr);});
goto _LL8; _LL8:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current < l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >= 1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) =='-': 0){ void* action;{ struct _handler_cons
_temp39; _push_handler(& _temp39);{ int _temp41= 0; if( setjmp( _temp39.handler)){
_temp41= 1;} if( ! _temp41){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp40=( void*) _exn_thrown; void* _temp43= _temp40; _LL45: if(
_temp43 == Cyc_Core_Not_found){ goto _LL46;} else{ goto _LL47;} _LL47: goto
_LL48; _LL46: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp49=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp49[ 0]=({ struct Cyc_Arg_Unknown_struct _temp50; _temp50.tag= Cyc_Arg_Unknown;
_temp50.f1= s; _temp50;}); _temp49;}), speclist, errmsg); return; _LL48:( void)
_throw( _temp43); _LL44:;}}}{ struct _handler_cons _temp51; _push_handler(&
_temp51);{ int _temp53= 0; if( setjmp( _temp51.handler)){ _temp53= 1;} if( !
_temp53){{ void* _temp54= action; void(* _temp72)(); void(* _temp74)( struct
_tagged_arr); int* _temp76; int* _temp78; void(* _temp80)( struct _tagged_arr,
struct _tagged_arr); void(* _temp82)( struct _tagged_arr); void(* _temp84)( int);
void(* _temp86)( struct _tagged_arr); _LL56: if(*(( int*) _temp54) == Cyc_Arg_Unit_spec){
_LL73: _temp72=(( struct Cyc_Arg_Unit_spec_struct*) _temp54)->f1; goto _LL57;}
else{ goto _LL58;} _LL58: if(*(( int*) _temp54) == Cyc_Arg_Flag_spec){ _LL75:
_temp74=(( struct Cyc_Arg_Flag_spec_struct*) _temp54)->f1; goto _LL59;} else{
goto _LL60;} _LL60: if(*(( int*) _temp54) == Cyc_Arg_Set_spec){ _LL77: _temp76=((
struct Cyc_Arg_Set_spec_struct*) _temp54)->f1; goto _LL61;} else{ goto _LL62;}
_LL62: if(*(( int*) _temp54) == Cyc_Arg_Clear_spec){ _LL79: _temp78=(( struct
Cyc_Arg_Clear_spec_struct*) _temp54)->f1; goto _LL63;} else{ goto _LL64;} _LL64:
if(*(( int*) _temp54) == Cyc_Arg_FlagString_spec){ _LL81: _temp80=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp54)->f1; goto _LL65;} else{ goto _LL66;} _LL66: if(*(( int*) _temp54) ==
Cyc_Arg_String_spec){ _LL83: _temp82=(( struct Cyc_Arg_String_spec_struct*)
_temp54)->f1; goto _LL67;} else{ goto _LL68;} _LL68: if(*(( int*) _temp54) ==
Cyc_Arg_Int_spec){ _LL85: _temp84=(( struct Cyc_Arg_Int_spec_struct*) _temp54)->f1;
goto _LL69;} else{ goto _LL70;} _LL70: if(*(( int*) _temp54) == Cyc_Arg_Rest_spec){
_LL87: _temp86=(( struct Cyc_Arg_Rest_spec_struct*) _temp54)->f1; goto _LL71;}
else{ goto _LL55;} _LL57: _temp72(); goto _LL55; _LL59: _temp74( s); goto _LL55;
_LL61:* _temp76= 1; goto _LL55; _LL63:* _temp78= 0; goto _LL55; _LL65: if( Cyc_Arg_current
+ 1 < l){ _temp80( s,*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1))); ++ Cyc_Arg_current;} else{
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp88=( struct
Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp88[ 0]=({ struct Cyc_Arg_Missing_struct _temp89; _temp89.tag= Cyc_Arg_Missing;
_temp89.f1= s; _temp89;}); _temp88;}), speclist, errmsg);} goto _LL55; _LL67:
if( Cyc_Arg_current + 1 < l){ _temp82(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp90=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp90[ 0]=({ struct Cyc_Arg_Missing_struct _temp91; _temp91.tag= Cyc_Arg_Missing;
_temp91.f1= s; _temp91;}); _temp90;}), speclist, errmsg);} goto _LL55; _LL69: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1)); int n;{ struct _handler_cons
_temp92; _push_handler(& _temp92);{ int _temp94= 0; if( setjmp( _temp92.handler)){
_temp94= 1;} if( ! _temp94){ n= Cyc_Core_int_of_string( arg);; _pop_handler();}
else{ void* _temp93=( void*) _exn_thrown; void* _temp96= _temp93; struct
_tagged_arr _temp102; _LL98: if(*(( void**) _temp96) == Cyc_Core_InvalidArg){
_LL103: _temp102=(( struct Cyc_Core_InvalidArg_struct*) _temp96)->f1; goto _LL99;}
else{ goto _LL100;} _LL100: goto _LL101; _LL99: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Wrong_struct* _temp104=( struct Cyc_Arg_Wrong_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Wrong_struct)); _temp104[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp105; _temp105.tag= Cyc_Arg_Wrong; _temp105.f1= s; _temp105.f2= arg;
_temp105.f3= _tag_arr("an integer", sizeof( unsigned char), 11u); _temp105;});
_temp104;}), speclist, errmsg); _npop_handler( 0u); return; _LL101:( void)
_throw( _temp96); _LL97:;}}} _temp84( n); ++ Cyc_Arg_current; goto _LL55;} _LL71:
while( Cyc_Arg_current < l - 1) { _temp86(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} goto _LL55; _LL55:;}; _pop_handler();} else{ void*
_temp52=( void*) _exn_thrown; void* _temp107= _temp52; struct _tagged_arr
_temp113; _LL109: if(*(( void**) _temp107) == Cyc_Arg_Bad){ _LL114: _temp113=((
struct Cyc_Arg_Bad_struct*) _temp107)->f1; goto _LL110;} else{ goto _LL111;}
_LL111: goto _LL112; _LL110: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp115=( struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp115[ 0]=({ struct Cyc_Arg_Message_struct _temp116; _temp116.tag= Cyc_Arg_Message;
_temp116.f1= _temp113; _temp116;}); _temp115;}), speclist, errmsg); goto _LL108;
_LL112:( void) _throw( _temp107); _LL108:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp117; _push_handler(& _temp117);{ int _temp119= 0; if( setjmp(
_temp117.handler)){ _temp119= 1;} if( ! _temp119){ anonfun( s);; _pop_handler();}
else{ void* _temp118=( void*) _exn_thrown; void* _temp121= _temp118; struct
_tagged_arr _temp127; _LL123: if(*(( void**) _temp121) == Cyc_Arg_Bad){ _LL128:
_temp127=(( struct Cyc_Arg_Bad_struct*) _temp121)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: goto _LL126; _LL124: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp129=( struct Cyc_Arg_Message_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp129[ 0]=({ struct Cyc_Arg_Message_struct
_temp130; _temp130.tag= Cyc_Arg_Message; _temp130.f1= _temp127; _temp130;});
_temp129;}), speclist, errmsg); goto _LL122; _LL126:( void) _throw( _temp121);
_LL122:;}}} ++ Cyc_Arg_current;}}}}