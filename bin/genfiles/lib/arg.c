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
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){({ struct Cyc_Stdio_String_pa_struct
_temp3; _temp3.tag= Cyc_Stdio_String_pa; _temp3.f1=( struct _tagged_arr) errmsg;{
void* _temp2[ 1u]={& _temp3}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp2, sizeof( void*), 1u));}}); while(
speclist != 0) {({ struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag= Cyc_Stdio_String_pa;
_temp6.f1=( struct _tagged_arr)(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( speclist))->hd)).f3;{ struct Cyc_Stdio_String_pa_struct _temp5;
_temp5.tag= Cyc_Stdio_String_pa; _temp5.f1=( struct _tagged_arr)(*(( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f1;{ void*
_temp4[ 2u]={& _temp5,& _temp6}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr(" %s %s\n",
sizeof( unsigned char), 8u), _tag_arr( _temp4, sizeof( void*), 2u));}}});
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
_temp17, _tag_arr("-help", sizeof( unsigned char), 6u)) != 0){({ struct Cyc_Stdio_String_pa_struct
_temp31; _temp31.tag= Cyc_Stdio_String_pa; _temp31.f1=( struct _tagged_arr)
_temp17;{ struct Cyc_Stdio_String_pa_struct _temp30; _temp30.tag= Cyc_Stdio_String_pa;
_temp30.f1=( struct _tagged_arr) progname;{ void* _temp29[ 2u]={& _temp30,&
_temp31}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: unknown option `%s'.\n",
sizeof( unsigned char), 26u), _tag_arr( _temp29, sizeof( void*), 2u));}}});}
goto _LL8; _LL12:({ struct Cyc_Stdio_String_pa_struct _temp34; _temp34.tag= Cyc_Stdio_String_pa;
_temp34.f1=( struct _tagged_arr) _temp19;{ struct Cyc_Stdio_String_pa_struct
_temp33; _temp33.tag= Cyc_Stdio_String_pa; _temp33.f1=( struct _tagged_arr)
progname;{ void* _temp32[ 2u]={& _temp33,& _temp34}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: option `%s' needs an argument.\n", sizeof( unsigned char), 36u),
_tag_arr( _temp32, sizeof( void*), 2u));}}}); goto _LL8; _LL14:({ struct Cyc_Stdio_String_pa_struct
_temp39; _temp39.tag= Cyc_Stdio_String_pa; _temp39.f1=( struct _tagged_arr)
_temp21;{ struct Cyc_Stdio_String_pa_struct _temp38; _temp38.tag= Cyc_Stdio_String_pa;
_temp38.f1=( struct _tagged_arr) _temp25;{ struct Cyc_Stdio_String_pa_struct
_temp37; _temp37.tag= Cyc_Stdio_String_pa; _temp37.f1=( struct _tagged_arr)
_temp23;{ struct Cyc_Stdio_String_pa_struct _temp36; _temp36.tag= Cyc_Stdio_String_pa;
_temp36.f1=( struct _tagged_arr) progname;{ void* _temp35[ 4u]={& _temp36,&
_temp37,& _temp38,& _temp39}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof( unsigned char), 50u), _tag_arr( _temp35, sizeof( void*), 4u));}}}}});
goto _LL8; _LL16:({ struct Cyc_Stdio_String_pa_struct _temp42; _temp42.tag= Cyc_Stdio_String_pa;
_temp42.f1=( struct _tagged_arr) _temp27;{ struct Cyc_Stdio_String_pa_struct
_temp41; _temp41.tag= Cyc_Stdio_String_pa; _temp41.f1=( struct _tagged_arr)
progname;{ void* _temp40[ 2u]={& _temp41,& _temp42}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: %s.\n", sizeof( unsigned char), 9u), _tag_arr( _temp40, sizeof(
void*), 2u));}}}); goto _LL8; _LL8:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=(
int) _get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current < l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >= 1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) =='-': 0){ void* action;{ struct _handler_cons
_temp43; _push_handler(& _temp43);{ int _temp45= 0; if( setjmp( _temp43.handler)){
_temp45= 1;} if( ! _temp45){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp44=( void*) _exn_thrown; void* _temp47= _temp44; _LL49: if(
_temp47 == Cyc_Core_Not_found){ goto _LL50;} else{ goto _LL51;} _LL51: goto
_LL52; _LL50: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp53=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp53[ 0]=({ struct Cyc_Arg_Unknown_struct _temp54; _temp54.tag= Cyc_Arg_Unknown;
_temp54.f1= s; _temp54;}); _temp53;}), speclist, errmsg); return; _LL52:( void)
_throw( _temp47); _LL48:;}}}{ struct _handler_cons _temp55; _push_handler(&
_temp55);{ int _temp57= 0; if( setjmp( _temp55.handler)){ _temp57= 1;} if( !
_temp57){{ void* _temp58= action; void(* _temp76)(); void(* _temp78)( struct
_tagged_arr); int* _temp80; int* _temp82; void(* _temp84)( struct _tagged_arr,
struct _tagged_arr); void(* _temp86)( struct _tagged_arr); void(* _temp88)( int);
void(* _temp90)( struct _tagged_arr); _LL60: if(*(( int*) _temp58) == Cyc_Arg_Unit_spec){
_LL77: _temp76=(( struct Cyc_Arg_Unit_spec_struct*) _temp58)->f1; goto _LL61;}
else{ goto _LL62;} _LL62: if(*(( int*) _temp58) == Cyc_Arg_Flag_spec){ _LL79:
_temp78=(( struct Cyc_Arg_Flag_spec_struct*) _temp58)->f1; goto _LL63;} else{
goto _LL64;} _LL64: if(*(( int*) _temp58) == Cyc_Arg_Set_spec){ _LL81: _temp80=((
struct Cyc_Arg_Set_spec_struct*) _temp58)->f1; goto _LL65;} else{ goto _LL66;}
_LL66: if(*(( int*) _temp58) == Cyc_Arg_Clear_spec){ _LL83: _temp82=(( struct
Cyc_Arg_Clear_spec_struct*) _temp58)->f1; goto _LL67;} else{ goto _LL68;} _LL68:
if(*(( int*) _temp58) == Cyc_Arg_FlagString_spec){ _LL85: _temp84=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp58)->f1; goto _LL69;} else{ goto _LL70;} _LL70: if(*(( int*) _temp58) ==
Cyc_Arg_String_spec){ _LL87: _temp86=(( struct Cyc_Arg_String_spec_struct*)
_temp58)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if(*(( int*) _temp58) ==
Cyc_Arg_Int_spec){ _LL89: _temp88=(( struct Cyc_Arg_Int_spec_struct*) _temp58)->f1;
goto _LL73;} else{ goto _LL74;} _LL74: if(*(( int*) _temp58) == Cyc_Arg_Rest_spec){
_LL91: _temp90=(( struct Cyc_Arg_Rest_spec_struct*) _temp58)->f1; goto _LL75;}
else{ goto _LL59;} _LL61: _temp76(); goto _LL59; _LL63: _temp78( s); goto _LL59;
_LL65:* _temp80= 1; goto _LL59; _LL67:* _temp82= 0; goto _LL59; _LL69: if( Cyc_Arg_current
+ 1 < l){ _temp84( s,*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1))); ++ Cyc_Arg_current;} else{
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp92=( struct
Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp92[ 0]=({ struct Cyc_Arg_Missing_struct _temp93; _temp93.tag= Cyc_Arg_Missing;
_temp93.f1= s; _temp93;}); _temp92;}), speclist, errmsg);} goto _LL59; _LL71:
if( Cyc_Arg_current + 1 < l){ _temp86(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp94=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp94[ 0]=({ struct Cyc_Arg_Missing_struct _temp95; _temp95.tag= Cyc_Arg_Missing;
_temp95.f1= s; _temp95;}); _temp94;}), speclist, errmsg);} goto _LL59; _LL73: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current + 1)); int n;{ struct _handler_cons
_temp96; _push_handler(& _temp96);{ int _temp98= 0; if( setjmp( _temp96.handler)){
_temp98= 1;} if( ! _temp98){ n= Cyc_Core_int_of_string( arg);; _pop_handler();}
else{ void* _temp97=( void*) _exn_thrown; void* _temp100= _temp97; struct
_tagged_arr _temp106; _LL102: if(*(( void**) _temp100) == Cyc_Core_InvalidArg){
_LL107: _temp106=(( struct Cyc_Core_InvalidArg_struct*) _temp100)->f1; goto
_LL103;} else{ goto _LL104;} _LL104: goto _LL105; _LL103: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Wrong_struct* _temp108=( struct Cyc_Arg_Wrong_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct)); _temp108[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp109; _temp109.tag= Cyc_Arg_Wrong; _temp109.f1= s; _temp109.f2= arg;
_temp109.f3= _tag_arr("an integer", sizeof( unsigned char), 11u); _temp109;});
_temp108;}), speclist, errmsg); _npop_handler( 0u); return; _LL105:( void)
_throw( _temp100); _LL101:;}}} _temp88( n); ++ Cyc_Arg_current; goto _LL59;}
_LL75: while( Cyc_Arg_current < l - 1) { _temp90(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+ 1))); ++ Cyc_Arg_current;} goto _LL59; _LL59:;}; _pop_handler();} else{ void*
_temp56=( void*) _exn_thrown; void* _temp111= _temp56; struct _tagged_arr
_temp117; _LL113: if(*(( void**) _temp111) == Cyc_Arg_Bad){ _LL118: _temp117=((
struct Cyc_Arg_Bad_struct*) _temp111)->f1; goto _LL114;} else{ goto _LL115;}
_LL115: goto _LL116; _LL114: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp119=( struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp119[ 0]=({ struct Cyc_Arg_Message_struct _temp120; _temp120.tag= Cyc_Arg_Message;
_temp120.f1= _temp117; _temp120;}); _temp119;}), speclist, errmsg); goto _LL112;
_LL116:( void) _throw( _temp111); _LL112:;}}} ++ Cyc_Arg_current;} else{{ struct
_handler_cons _temp121; _push_handler(& _temp121);{ int _temp123= 0; if( setjmp(
_temp121.handler)){ _temp123= 1;} if( ! _temp123){ anonfun( s);; _pop_handler();}
else{ void* _temp122=( void*) _exn_thrown; void* _temp125= _temp122; struct
_tagged_arr _temp131; _LL127: if(*(( void**) _temp125) == Cyc_Arg_Bad){ _LL132:
_temp131=(( struct Cyc_Arg_Bad_struct*) _temp125)->f1; goto _LL128;} else{ goto
_LL129;} _LL129: goto _LL130; _LL128: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp133=( struct Cyc_Arg_Message_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp133[ 0]=({ struct Cyc_Arg_Message_struct
_temp134; _temp134.tag= Cyc_Arg_Message; _temp134.f1= _temp131; _temp134;});
_temp133;}), speclist, errmsg); goto _LL126; _LL130:( void) _throw( _temp125);
_LL126:;}}} ++ Cyc_Arg_current;}}}}
