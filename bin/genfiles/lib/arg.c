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
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
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
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){({ int(* _temp2)(
struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp6= Cyc_Stdio_stderr; struct _tagged_arr _temp7=
_tag_arr("%s\n", sizeof( unsigned char), 4u); struct Cyc_Stdio_String_pa_struct
_temp8; _temp8.tag= Cyc_Stdio_String_pa; _temp8.f1=( struct _tagged_arr) errmsg;{
void* _temp5=( void*)& _temp8; void* _temp3[ 1u]={ _temp5}; struct _tagged_arr
_temp4={( void*) _temp3,( void*) _temp3,( void*)( _temp3 + 1u)}; _temp2( _temp6,
_temp7, _temp4);}}); while( speclist != 0) {({ int(* _temp9)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp14= Cyc_Stdio_stderr; struct _tagged_arr _temp15= _tag_arr(" %s %s\n",
sizeof( unsigned char), 8u); struct Cyc_Stdio_String_pa_struct _temp17; _temp17.tag=
Cyc_Stdio_String_pa; _temp17.f1=( struct _tagged_arr)(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( speclist))->hd)).f1;{ void* _temp12=( void*)&
_temp17; struct Cyc_Stdio_String_pa_struct _temp16; _temp16.tag= Cyc_Stdio_String_pa;
_temp16.f1=( struct _tagged_arr)(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( speclist))->hd)).f3;{ void* _temp13=( void*)& _temp16; void*
_temp10[ 2u]={ _temp12, _temp13}; struct _tagged_arr _temp11={( void*) _temp10,(
void*) _temp10,( void*)( _temp10 + 2u)}; _temp9( _temp14, _temp15, _temp11);}}});
speclist=(( struct Cyc_List_List*) _check_null( speclist))->tl;}} int Cyc_Arg_current=
0; static struct _tagged_arr Cyc_Arg_args={( void*) 0u,( void*) 0u,( void*) 0u +
0u}; static void Cyc_Arg_stop( int prog_pos, void* e, struct Cyc_List_List*
speclist, struct _tagged_arr errmsg){ struct _tagged_arr progname= prog_pos <
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr))?*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), prog_pos)):
_tag_arr("(?)", sizeof( unsigned char), 4u);{ void* _temp18= e; struct
_tagged_arr _temp28; struct _tagged_arr _temp30; struct _tagged_arr _temp32;
struct _tagged_arr _temp34; struct _tagged_arr _temp36; struct _tagged_arr
_temp38; _LL20: if(*(( int*) _temp18) == Cyc_Arg_Unknown){ _LL29: _temp28=((
struct Cyc_Arg_Unknown_struct*) _temp18)->f1; goto _LL21;} else{ goto _LL22;}
_LL22: if(*(( int*) _temp18) == Cyc_Arg_Missing){ _LL31: _temp30=(( struct Cyc_Arg_Missing_struct*)
_temp18)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp18) ==
Cyc_Arg_Wrong){ _LL37: _temp36=(( struct Cyc_Arg_Wrong_struct*) _temp18)->f1;
goto _LL35; _LL35: _temp34=(( struct Cyc_Arg_Wrong_struct*) _temp18)->f2; goto
_LL33; _LL33: _temp32=(( struct Cyc_Arg_Wrong_struct*) _temp18)->f3; goto _LL25;}
else{ goto _LL26;} _LL26: if(*(( int*) _temp18) == Cyc_Arg_Message){ _LL39:
_temp38=(( struct Cyc_Arg_Message_struct*) _temp18)->f1; goto _LL27;} else{ goto
_LL19;} _LL21: if( Cyc_String_strcmp( _temp28, _tag_arr("-help", sizeof(
unsigned char), 6u)) != 0){({ int(* _temp40)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp45= Cyc_Stdio_stderr; struct _tagged_arr _temp46= _tag_arr("%s: unknown option `%s'.\n",
sizeof( unsigned char), 26u); struct Cyc_Stdio_String_pa_struct _temp48; _temp48.tag=
Cyc_Stdio_String_pa; _temp48.f1=( struct _tagged_arr) progname;{ void* _temp43=(
void*)& _temp48; struct Cyc_Stdio_String_pa_struct _temp47; _temp47.tag= Cyc_Stdio_String_pa;
_temp47.f1=( struct _tagged_arr) _temp28;{ void* _temp44=( void*)& _temp47; void*
_temp41[ 2u]={ _temp43, _temp44}; struct _tagged_arr _temp42={( void*) _temp41,(
void*) _temp41,( void*)( _temp41 + 2u)}; _temp40( _temp45, _temp46, _temp42);}}});}
goto _LL19; _LL23:({ int(* _temp49)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp54= Cyc_Stdio_stderr; struct _tagged_arr _temp55= _tag_arr("%s: option `%s' needs an argument.\n",
sizeof( unsigned char), 36u); struct Cyc_Stdio_String_pa_struct _temp57; _temp57.tag=
Cyc_Stdio_String_pa; _temp57.f1=( struct _tagged_arr) progname;{ void* _temp52=(
void*)& _temp57; struct Cyc_Stdio_String_pa_struct _temp56; _temp56.tag= Cyc_Stdio_String_pa;
_temp56.f1=( struct _tagged_arr) _temp30;{ void* _temp53=( void*)& _temp56; void*
_temp50[ 2u]={ _temp52, _temp53}; struct _tagged_arr _temp51={( void*) _temp50,(
void*) _temp50,( void*)( _temp50 + 2u)}; _temp49( _temp54, _temp55, _temp51);}}});
goto _LL19; _LL25:({ int(* _temp58)( struct Cyc_Stdio___sFILE*, struct
_tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp65= Cyc_Stdio_stderr; struct _tagged_arr _temp66= _tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof( unsigned char), 50u); struct Cyc_Stdio_String_pa_struct _temp70; _temp70.tag=
Cyc_Stdio_String_pa; _temp70.f1=( struct _tagged_arr) progname;{ void* _temp61=(
void*)& _temp70; struct Cyc_Stdio_String_pa_struct _temp69; _temp69.tag= Cyc_Stdio_String_pa;
_temp69.f1=( struct _tagged_arr) _temp34;{ void* _temp62=( void*)& _temp69;
struct Cyc_Stdio_String_pa_struct _temp68; _temp68.tag= Cyc_Stdio_String_pa;
_temp68.f1=( struct _tagged_arr) _temp36;{ void* _temp63=( void*)& _temp68;
struct Cyc_Stdio_String_pa_struct _temp67; _temp67.tag= Cyc_Stdio_String_pa;
_temp67.f1=( struct _tagged_arr) _temp32;{ void* _temp64=( void*)& _temp67; void*
_temp59[ 4u]={ _temp61, _temp62, _temp63, _temp64}; struct _tagged_arr _temp60={(
void*) _temp59,( void*) _temp59,( void*)( _temp59 + 4u)}; _temp58( _temp65,
_temp66, _temp60);}}}}}); goto _LL19; _LL27:({ int(* _temp71)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp76= Cyc_Stdio_stderr; struct _tagged_arr _temp77= _tag_arr("%s: %s.\n",
sizeof( unsigned char), 9u); struct Cyc_Stdio_String_pa_struct _temp79; _temp79.tag=
Cyc_Stdio_String_pa; _temp79.f1=( struct _tagged_arr) progname;{ void* _temp74=(
void*)& _temp79; struct Cyc_Stdio_String_pa_struct _temp78; _temp78.tag= Cyc_Stdio_String_pa;
_temp78.f1=( struct _tagged_arr) _temp38;{ void* _temp75=( void*)& _temp78; void*
_temp72[ 2u]={ _temp74, _temp75}; struct _tagged_arr _temp73={( void*) _temp72,(
void*) _temp72,( void*)( _temp72 + 2u)}; _temp71( _temp76, _temp77, _temp73);}}});
goto _LL19; _LL19:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)
_get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current < l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >= 1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) =='-': 0){ void* action;{ struct _handler_cons
_temp80; _push_handler(& _temp80);{ int _temp82= 0; if( setjmp( _temp80.handler)){
_temp82= 1;} if( ! _temp82){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp81=( void*) _exn_thrown; void* _temp84= _temp81; _LL86: if(
_temp84 == Cyc_Core_Not_found){ goto _LL87;} else{ goto _LL88;} _LL88: goto
_LL89; _LL87: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp90=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp90[ 0]=({ struct Cyc_Arg_Unknown_struct _temp91; _temp91.tag= Cyc_Arg_Unknown;
_temp91.f1= s; _temp91;}); _temp90;}), speclist, errmsg); return; _LL89:( void)
_throw( _temp84); _LL85:;}}}{ struct _handler_cons _temp92; _push_handler(&
_temp92);{ int _temp94= 0; if( setjmp( _temp92.handler)){ _temp94= 1;} if( !
_temp94){{ void* _temp95= action; void(* _temp113)(); void(* _temp115)( struct
_tagged_arr); int* _temp117; int* _temp119; void(* _temp121)( struct _tagged_arr,
struct _tagged_arr); void(* _temp123)( struct _tagged_arr); void(* _temp125)(
int); void(* _temp127)( struct _tagged_arr); _LL97: if(*(( int*) _temp95) == Cyc_Arg_Unit_spec){
_LL114: _temp113=(( struct Cyc_Arg_Unit_spec_struct*) _temp95)->f1; goto _LL98;}
else{ goto _LL99;} _LL99: if(*(( int*) _temp95) == Cyc_Arg_Flag_spec){ _LL116:
_temp115=(( struct Cyc_Arg_Flag_spec_struct*) _temp95)->f1; goto _LL100;} else{
goto _LL101;} _LL101: if(*(( int*) _temp95) == Cyc_Arg_Set_spec){ _LL118:
_temp117=(( struct Cyc_Arg_Set_spec_struct*) _temp95)->f1; goto _LL102;} else{
goto _LL103;} _LL103: if(*(( int*) _temp95) == Cyc_Arg_Clear_spec){ _LL120:
_temp119=(( struct Cyc_Arg_Clear_spec_struct*) _temp95)->f1; goto _LL104;} else{
goto _LL105;} _LL105: if(*(( int*) _temp95) == Cyc_Arg_FlagString_spec){ _LL122:
_temp121=(( struct Cyc_Arg_FlagString_spec_struct*) _temp95)->f1; goto _LL106;}
else{ goto _LL107;} _LL107: if(*(( int*) _temp95) == Cyc_Arg_String_spec){
_LL124: _temp123=(( struct Cyc_Arg_String_spec_struct*) _temp95)->f1; goto
_LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp95) == Cyc_Arg_Int_spec){
_LL126: _temp125=(( struct Cyc_Arg_Int_spec_struct*) _temp95)->f1; goto _LL110;}
else{ goto _LL111;} _LL111: if(*(( int*) _temp95) == Cyc_Arg_Rest_spec){ _LL128:
_temp127=(( struct Cyc_Arg_Rest_spec_struct*) _temp95)->f1; goto _LL112;} else{
goto _LL96;} _LL98: _temp113(); goto _LL96; _LL100: _temp115( s); goto _LL96;
_LL102:* _temp117= 1; goto _LL96; _LL104:* _temp119= 0; goto _LL96; _LL106: if(
Cyc_Arg_current + 1 < l){ _temp121( s,*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp129=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp129[ 0]=({ struct Cyc_Arg_Missing_struct _temp130; _temp130.tag= Cyc_Arg_Missing;
_temp130.f1= s; _temp130;}); _temp129;}), speclist, errmsg);} goto _LL96; _LL108:
if( Cyc_Arg_current + 1 < l){ _temp123(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp131=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp131[ 0]=({ struct Cyc_Arg_Missing_struct _temp132; _temp132.tag= Cyc_Arg_Missing;
_temp132.f1= s; _temp132;}); _temp131;}), speclist, errmsg);} goto _LL96; _LL110: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1)); int n;{ struct _handler_cons
_temp133; _push_handler(& _temp133);{ int _temp135= 0; if( setjmp( _temp133.handler)){
_temp135= 1;} if( ! _temp135){ n= Cyc_Core_int_of_string( arg);; _pop_handler();}
else{ void* _temp134=( void*) _exn_thrown; void* _temp137= _temp134; struct
_tagged_arr _temp143; _LL139: if(*(( void**) _temp137) == Cyc_Core_InvalidArg){
_LL144: _temp143=(( struct Cyc_Core_InvalidArg_struct*) _temp137)->f1; goto
_LL140;} else{ goto _LL141;} _LL141: goto _LL142; _LL140: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Wrong_struct* _temp145=( struct Cyc_Arg_Wrong_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct)); _temp145[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp146; _temp146.tag= Cyc_Arg_Wrong; _temp146.f1= s; _temp146.f2= arg;
_temp146.f3= _tag_arr("an integer", sizeof( unsigned char), 11u); _temp146;});
_temp145;}), speclist, errmsg); _npop_handler( 0u); return; _LL142:( void)
_throw( _temp137); _LL138:;}}} _temp125( n); ++ Cyc_Arg_current; goto _LL96;}
_LL112: while( Cyc_Arg_current < l - 1) { _temp127(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} goto _LL96; _LL96:;}; _pop_handler();} else{ void*
_temp93=( void*) _exn_thrown; void* _temp148= _temp93; struct _tagged_arr
_temp154; _LL150: if(*(( void**) _temp148) == Cyc_Arg_Bad){ _LL155: _temp154=((
struct Cyc_Arg_Bad_struct*) _temp148)->f1; goto _LL151;} else{ goto _LL152;}
_LL152: goto _LL153; _LL151: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp156=( struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp156[ 0]=({ struct Cyc_Arg_Message_struct _temp157; _temp157.tag= Cyc_Arg_Message;
_temp157.f1= _temp154; _temp157;}); _temp156;}), speclist, errmsg); goto _LL149;
_LL153:( void) _throw( _temp148); _LL149:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp158; _push_handler(& _temp158);{ int _temp160= 0; if( setjmp(
_temp158.handler)){ _temp160= 1;} if( ! _temp160){ anonfun( s);; _pop_handler();}
else{ void* _temp159=( void*) _exn_thrown; void* _temp162= _temp159; struct
_tagged_arr _temp168; _LL164: if(*(( void**) _temp162) == Cyc_Arg_Bad){ _LL169:
_temp168=(( struct Cyc_Arg_Bad_struct*) _temp162)->f1; goto _LL165;} else{ goto
_LL166;} _LL166: goto _LL167; _LL165: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp170=( struct Cyc_Arg_Message_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp170[ 0]=({ struct Cyc_Arg_Message_struct
_temp171; _temp171.tag= Cyc_Arg_Message; _temp171.f1= _temp168; _temp171;});
_temp170;}), speclist, errmsg); goto _LL163; _LL167:( void) _throw( _temp162);
_LL163:;}}} ++ Cyc_Arg_current;}}}}