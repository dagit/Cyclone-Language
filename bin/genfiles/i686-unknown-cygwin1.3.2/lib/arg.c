 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_Core_int_of_string(
struct _tagged_arr); extern unsigned char* string_to_Cstring( struct _tagged_arr);
extern unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_std___sFILE;
extern struct Cyc_std___sFILE* Cyc_std_stderr; extern unsigned char Cyc_std_FileCloseError[
19u]; extern unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa=
0; struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_std_strlen(
struct _tagged_arr s); extern int Cyc_std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_std_strncmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int len); extern struct _tagged_arr Cyc_std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern unsigned char Cyc_Arg_Bad[ 8u];
struct Cyc_Arg_Bad_struct{ unsigned char* tag; struct _tagged_arr f1; } ; extern
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
_tagged_arr errmsg, struct _tagged_arr args); struct Cyc_Buffer_t; extern struct
Cyc_Buffer_t* Cyc_Buffer_create( unsigned int n); extern struct _tagged_arr Cyc_Buffer_contents(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_add_substring( struct Cyc_Buffer_t*,
struct _tagged_arr, int offset, int len); extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*, struct _tagged_arr); extern int isalnum( int); extern int
isalpha( int); extern int isascii( int); extern int iscntrl( int); extern int
isdigit( int); extern int isgraph( int); extern int islower( int); extern int
isprint( int); extern int ispunct( int); extern int isspace( int); extern int
isupper( int); extern int isxdigit( int); extern int toascii( int); extern int
tolower( int); extern int toupper( int); extern int _tolower( int); extern int
_toupper( int); unsigned char Cyc_Arg_Bad[ 8u]="\000\000\000\000Bad";
unsigned char Cyc_Arg_Error[ 10u]="\000\000\000\000Error"; static const int Cyc_Arg_Prefix=
0; struct Cyc_Arg_Prefix_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Arg_Exact= 1; struct Cyc_Arg_Exact_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Arg_Unknown= 0; struct Cyc_Arg_Unknown_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Arg_Missing= 1; struct
Cyc_Arg_Missing_struct{ int tag; struct _tagged_arr f1; } ; static const int Cyc_Arg_Message=
2; struct Cyc_Arg_Message_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Arg_Wrong= 3; struct Cyc_Arg_Wrong_struct{ int tag; struct
_tagged_arr f1; struct _tagged_arr f2; struct _tagged_arr f3; } ; struct _tuple0{
struct _tagged_arr f1; int f2; struct _tagged_arr f3; void* f4; struct
_tagged_arr f5; } ; static void* Cyc_Arg_lookup( struct Cyc_List_List* l, struct
_tagged_arr x){ while( l !=  0) { struct _tagged_arr _temp0=(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( l))->hd)).f1; unsigned int _temp1= Cyc_std_strlen(
_temp0); if( _temp1 >  0?(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( l))->hd)).f2: 0){ if( Cyc_std_strncmp( x,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( l))->hd)).f1, _temp1) ==  0){ return(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f4;}} else{ if(
Cyc_std_strcmp( x,(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f1)
==  0){ return(*(( struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)).f4;}}
l=(( struct Cyc_List_List*) _check_null( l))->tl;}( int) _throw(( void*) Cyc_Core_Not_found);}
static struct _tagged_arr Cyc_Arg_Justify_break_line( struct Cyc_Buffer_t* b,
int howmuch, struct _tagged_arr s){ if( s.curr == (( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u)).curr){ return( struct _tagged_arr) _tag_arr( 0u, 0u, 0u);}
if( howmuch <  0){ howmuch= 0;}{ unsigned int _temp2= Cyc_std_strlen( s); if(
howmuch >  _temp2){ Cyc_Buffer_add_string( b, s); return( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u);}{ int i; for( i= howmuch -  1; i >=  0? ! isspace(( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))):
0; i --){;} if( i <  0){ for( i= howmuch? howmuch -  1: 0;( i <  _temp2?( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)):
0)? ! isspace(( int)*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i))): 0; i ++){;}} Cyc_Buffer_add_substring( b, s, 0, i);{
struct _tagged_arr whatsleft=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); for( 0;(
i <  _temp2?( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)): 0)? isspace(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0; i ++){;} if( i < 
_temp2?( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)): 0){ whatsleft= _tagged_arr_plus( s, sizeof( unsigned char),
i);} return whatsleft;}}}} void Cyc_Arg_Justify_justify_b( struct Cyc_Buffer_t*
b, int indent, int margin, struct _tagged_arr item, struct _tagged_arr desc){
if( item.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Buffer_add_string(
b, item);} if( desc.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
return;} if( indent <  0){ indent= 0;} if( margin <  0){ margin= 0;}{ struct
_tagged_arr indentstr=({ unsigned int _temp8=( unsigned int)( indent +  1);
unsigned char* _temp9=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
*  _temp8); struct _tagged_arr _temp11= _tag_arr( _temp9, sizeof( unsigned char),(
unsigned int)( indent +  1));{ unsigned int _temp10= _temp8; unsigned int i;
for( i= 0; i <  _temp10; i ++){ _temp9[ i]= i ==  0?'\n':' ';}}; _temp11;});
unsigned int _temp3= Cyc_std_strlen( item); struct _tagged_arr itemsep; if( Cyc_std_strlen(
desc) >  0){ if( _temp3 +  1 >  indent){ itemsep= indentstr;} else{ itemsep=({
unsigned int _temp4= indent -  _temp3; unsigned char* _temp5=( unsigned char*)
GC_malloc_atomic( sizeof( unsigned char) *  _temp4); struct _tagged_arr _temp7=
_tag_arr( _temp5, sizeof( unsigned char), indent -  _temp3);{ unsigned int
_temp6= _temp4; unsigned int i; for( i= 0; i <  _temp6; i ++){ _temp5[ i]=' ';}};
_temp7;});}} else{ return;} Cyc_Buffer_add_string( b, itemsep); while( desc.curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr) { desc= Cyc_Arg_Justify_break_line(
b, margin -  indent, desc); if( desc.curr != (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr){ Cyc_Buffer_add_string( b, indentstr);} else{ Cyc_Buffer_add_string(
b, _tag_arr("\n", sizeof( unsigned char), 2u));}} return;}} void Cyc_Arg_usage(
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){({ struct Cyc_std_String_pa_struct
_temp13; _temp13.tag= Cyc_std_String_pa; _temp13.f1=( struct _tagged_arr) errmsg;{
void* _temp12[ 1u]={& _temp13}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s\n",
sizeof( unsigned char), 4u), _tag_arr( _temp12, sizeof( void*), 1u));}});{
struct Cyc_Buffer_t* _temp14= Cyc_Buffer_create( 1024); while( speclist !=  0) {
Cyc_Arg_Justify_justify_b( _temp14, 12, 72,( struct _tagged_arr) Cyc_std_strconcat((*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f1,(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f3),(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( speclist))->hd)).f5);
speclist=(( struct Cyc_List_List*) _check_null( speclist))->tl;}({ struct Cyc_std_String_pa_struct
_temp16; _temp16.tag= Cyc_std_String_pa; _temp16.f1=( struct _tagged_arr) Cyc_Buffer_contents(
_temp14);{ void* _temp15[ 1u]={& _temp16}; Cyc_std_fprintf( Cyc_std_stderr,
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp15, sizeof( void*), 1u));}});}}
int Cyc_Arg_current= 0; static struct _tagged_arr Cyc_Arg_args={( void*) 0u,(
void*) 0u,( void*) 0u +  0u}; static void Cyc_Arg_stop( int prog_pos, void* e,
struct Cyc_List_List* speclist, struct _tagged_arr errmsg){ struct _tagged_arr
progname= prog_pos <  _get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr))?*((
struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args, sizeof( struct
_tagged_arr), prog_pos)): _tag_arr("(?)", sizeof( unsigned char), 4u);{ void*
_temp17= e; struct _tagged_arr _temp27; struct _tagged_arr _temp29; struct
_tagged_arr _temp31; struct _tagged_arr _temp33; struct _tagged_arr _temp35;
struct _tagged_arr _temp37; _LL19: if(*(( int*) _temp17) ==  Cyc_Arg_Unknown){
_LL28: _temp27=(( struct Cyc_Arg_Unknown_struct*) _temp17)->f1; goto _LL20;}
else{ goto _LL21;} _LL21: if(*(( int*) _temp17) ==  Cyc_Arg_Missing){ _LL30:
_temp29=(( struct Cyc_Arg_Missing_struct*) _temp17)->f1; goto _LL22;} else{ goto
_LL23;} _LL23: if(*(( int*) _temp17) ==  Cyc_Arg_Wrong){ _LL36: _temp35=((
struct Cyc_Arg_Wrong_struct*) _temp17)->f1; goto _LL34; _LL34: _temp33=(( struct
Cyc_Arg_Wrong_struct*) _temp17)->f2; goto _LL32; _LL32: _temp31=(( struct Cyc_Arg_Wrong_struct*)
_temp17)->f3; goto _LL24;} else{ goto _LL25;} _LL25: if(*(( int*) _temp17) == 
Cyc_Arg_Message){ _LL38: _temp37=(( struct Cyc_Arg_Message_struct*) _temp17)->f1;
goto _LL26;} else{ goto _LL18;} _LL20: if( Cyc_std_strcmp( _temp27, _tag_arr("-help",
sizeof( unsigned char), 6u)) !=  0){({ struct Cyc_std_String_pa_struct _temp41;
_temp41.tag= Cyc_std_String_pa; _temp41.f1=( struct _tagged_arr) _temp27;{
struct Cyc_std_String_pa_struct _temp40; _temp40.tag= Cyc_std_String_pa; _temp40.f1=(
struct _tagged_arr) progname;{ void* _temp39[ 2u]={& _temp40,& _temp41}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: unknown option `%s'.\n", sizeof( unsigned char),
26u), _tag_arr( _temp39, sizeof( void*), 2u));}}});} goto _LL18; _LL22:({ struct
Cyc_std_String_pa_struct _temp44; _temp44.tag= Cyc_std_String_pa; _temp44.f1=(
struct _tagged_arr) _temp29;{ struct Cyc_std_String_pa_struct _temp43; _temp43.tag=
Cyc_std_String_pa; _temp43.f1=( struct _tagged_arr) progname;{ void* _temp42[ 2u]={&
_temp43,& _temp44}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s: option `%s' needs an argument.\n",
sizeof( unsigned char), 36u), _tag_arr( _temp42, sizeof( void*), 2u));}}}); goto
_LL18; _LL24:({ struct Cyc_std_String_pa_struct _temp49; _temp49.tag= Cyc_std_String_pa;
_temp49.f1=( struct _tagged_arr) _temp31;{ struct Cyc_std_String_pa_struct
_temp48; _temp48.tag= Cyc_std_String_pa; _temp48.f1=( struct _tagged_arr)
_temp35;{ struct Cyc_std_String_pa_struct _temp47; _temp47.tag= Cyc_std_String_pa;
_temp47.f1=( struct _tagged_arr) _temp33;{ struct Cyc_std_String_pa_struct
_temp46; _temp46.tag= Cyc_std_String_pa; _temp46.f1=( struct _tagged_arr)
progname;{ void* _temp45[ 4u]={& _temp46,& _temp47,& _temp48,& _temp49}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof( unsigned char), 50u), _tag_arr( _temp45, sizeof( void*), 4u));}}}}});
goto _LL18; _LL26:({ struct Cyc_std_String_pa_struct _temp52; _temp52.tag= Cyc_std_String_pa;
_temp52.f1=( struct _tagged_arr) _temp37;{ struct Cyc_std_String_pa_struct
_temp51; _temp51.tag= Cyc_std_String_pa; _temp51.f1=( struct _tagged_arr)
progname;{ void* _temp50[ 2u]={& _temp51,& _temp52}; Cyc_std_fprintf( Cyc_std_stderr,
_tag_arr("%s: %s.\n", sizeof( unsigned char), 9u), _tag_arr( _temp50, sizeof(
void*), 2u));}}}); goto _LL18; _LL18:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=(
int) _get_arr_size( Cyc_Arg_args, sizeof( struct _tagged_arr));} void Cyc_Arg_parse(
struct Cyc_List_List* speclist, void(* anonfun)( struct _tagged_arr), struct
_tagged_arr errmsg, struct _tagged_arr orig_args){ Cyc_Arg_args= orig_args;{ int
initpos= Cyc_Arg_current; int l=( int) _get_arr_size( Cyc_Arg_args, sizeof(
struct _tagged_arr)); ++ Cyc_Arg_current; while( Cyc_Arg_current <  l) { struct
_tagged_arr s=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current)); if( _get_arr_size( s, sizeof(
unsigned char)) >=  1?*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0)) == '-': 0){ void* action;{ struct _handler_cons
_temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp( _temp53.handler)){
_temp55= 1;} if( ! _temp55){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();}
else{ void* _temp54=( void*) _exn_thrown; void* _temp57= _temp54; _LL59: if(
_temp57 ==  Cyc_Core_Not_found){ goto _LL60;} else{ goto _LL61;} _LL61: goto
_LL62; _LL60: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Unknown_struct*
_temp63=( struct Cyc_Arg_Unknown_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct));
_temp63[ 0]=({ struct Cyc_Arg_Unknown_struct _temp64; _temp64.tag= Cyc_Arg_Unknown;
_temp64.f1= s; _temp64;}); _temp63;}), speclist, errmsg); return; _LL62:( void)
_throw( _temp57); _LL58:;}}}{ struct _handler_cons _temp65; _push_handler(&
_temp65);{ int _temp67= 0; if( setjmp( _temp65.handler)){ _temp67= 1;} if( !
_temp67){{ void* _temp68= action; void(* _temp86)(); void(* _temp88)( struct
_tagged_arr); int* _temp90; int* _temp92; void(* _temp94)( struct _tagged_arr,
struct _tagged_arr); void(* _temp96)( struct _tagged_arr); void(* _temp98)( int);
void(* _temp100)( struct _tagged_arr); _LL70: if(*(( int*) _temp68) ==  Cyc_Arg_Unit_spec){
_LL87: _temp86=(( struct Cyc_Arg_Unit_spec_struct*) _temp68)->f1; goto _LL71;}
else{ goto _LL72;} _LL72: if(*(( int*) _temp68) ==  Cyc_Arg_Flag_spec){ _LL89:
_temp88=(( struct Cyc_Arg_Flag_spec_struct*) _temp68)->f1; goto _LL73;} else{
goto _LL74;} _LL74: if(*(( int*) _temp68) ==  Cyc_Arg_Set_spec){ _LL91: _temp90=((
struct Cyc_Arg_Set_spec_struct*) _temp68)->f1; goto _LL75;} else{ goto _LL76;}
_LL76: if(*(( int*) _temp68) ==  Cyc_Arg_Clear_spec){ _LL93: _temp92=(( struct
Cyc_Arg_Clear_spec_struct*) _temp68)->f1; goto _LL77;} else{ goto _LL78;} _LL78:
if(*(( int*) _temp68) ==  Cyc_Arg_FlagString_spec){ _LL95: _temp94=(( struct Cyc_Arg_FlagString_spec_struct*)
_temp68)->f1; goto _LL79;} else{ goto _LL80;} _LL80: if(*(( int*) _temp68) == 
Cyc_Arg_String_spec){ _LL97: _temp96=(( struct Cyc_Arg_String_spec_struct*)
_temp68)->f1; goto _LL81;} else{ goto _LL82;} _LL82: if(*(( int*) _temp68) == 
Cyc_Arg_Int_spec){ _LL99: _temp98=(( struct Cyc_Arg_Int_spec_struct*) _temp68)->f1;
goto _LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp68) ==  Cyc_Arg_Rest_spec){
_LL101: _temp100=(( struct Cyc_Arg_Rest_spec_struct*) _temp68)->f1; goto _LL85;}
else{ goto _LL69;} _LL71: _temp86(); goto _LL69; _LL73: _temp88( s); goto _LL69;
_LL75:* _temp90= 1; goto _LL69; _LL77:* _temp92= 0; goto _LL69; _LL79: if( Cyc_Arg_current
+  1 <  l){ _temp94( s,*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current +  1))); ++ Cyc_Arg_current;} else{
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct* _temp102=(
struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp102[ 0]=({ struct Cyc_Arg_Missing_struct _temp103; _temp103.tag= Cyc_Arg_Missing;
_temp103.f1= s; _temp103;}); _temp102;}), speclist, errmsg);} goto _LL69; _LL81:
if( Cyc_Arg_current +  1 <  l){ _temp96(*(( struct _tagged_arr*)
_check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr), Cyc_Arg_current
+  1))); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Missing_struct*
_temp104=( struct Cyc_Arg_Missing_struct*) GC_malloc( sizeof( struct Cyc_Arg_Missing_struct));
_temp104[ 0]=({ struct Cyc_Arg_Missing_struct _temp105; _temp105.tag= Cyc_Arg_Missing;
_temp105.f1= s; _temp105;}); _temp104;}), speclist, errmsg);} goto _LL69; _LL83: {
struct _tagged_arr arg=*(( struct _tagged_arr*) _check_unknown_subscript( Cyc_Arg_args,
sizeof( struct _tagged_arr), Cyc_Arg_current +  1)); int n;{ struct
_handler_cons _temp106; _push_handler(& _temp106);{ int _temp108= 0; if( setjmp(
_temp106.handler)){ _temp108= 1;} if( ! _temp108){ n= Cyc_Core_int_of_string(
arg);; _pop_handler();} else{ void* _temp107=( void*) _exn_thrown; void*
_temp110= _temp107; struct _tagged_arr _temp116; _LL112: if(*(( void**) _temp110)
==  Cyc_Core_InvalidArg){ _LL117: _temp116=(( struct Cyc_Core_InvalidArg_struct*)
_temp110)->f1; goto _LL113;} else{ goto _LL114;} _LL114: goto _LL115; _LL113:
Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Wrong_struct* _temp118=( struct
Cyc_Arg_Wrong_struct*) GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct));
_temp118[ 0]=({ struct Cyc_Arg_Wrong_struct _temp119; _temp119.tag= Cyc_Arg_Wrong;
_temp119.f1= s; _temp119.f2= arg; _temp119.f3= _tag_arr("an integer", sizeof(
unsigned char), 11u); _temp119;}); _temp118;}), speclist, errmsg); _npop_handler(
0u); return; _LL115:( void) _throw( _temp110); _LL111:;}}} _temp98( n); ++ Cyc_Arg_current;
goto _LL69;} _LL85: while( Cyc_Arg_current <  l -  1) { _temp100(*(( struct
_tagged_arr*) _check_unknown_subscript( Cyc_Arg_args, sizeof( struct _tagged_arr),
Cyc_Arg_current +  1))); ++ Cyc_Arg_current;} goto _LL69; _LL69:;}; _pop_handler();}
else{ void* _temp66=( void*) _exn_thrown; void* _temp121= _temp66; struct
_tagged_arr _temp127; _LL123: if(*(( void**) _temp121) ==  Cyc_Arg_Bad){ _LL128:
_temp127=(( struct Cyc_Arg_Bad_struct*) _temp121)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: goto _LL126; _LL124: Cyc_Arg_stop( initpos,( void*)({ struct
Cyc_Arg_Message_struct* _temp129=( struct Cyc_Arg_Message_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Message_struct)); _temp129[ 0]=({ struct Cyc_Arg_Message_struct
_temp130; _temp130.tag= Cyc_Arg_Message; _temp130.f1= _temp127; _temp130;});
_temp129;}), speclist, errmsg); goto _LL122; _LL126:( void) _throw( _temp121);
_LL122:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp131;
_push_handler(& _temp131);{ int _temp133= 0; if( setjmp( _temp131.handler)){
_temp133= 1;} if( ! _temp133){ anonfun( s);; _pop_handler();} else{ void*
_temp132=( void*) _exn_thrown; void* _temp135= _temp132; struct _tagged_arr
_temp141; _LL137: if(*(( void**) _temp135) ==  Cyc_Arg_Bad){ _LL142: _temp141=((
struct Cyc_Arg_Bad_struct*) _temp135)->f1; goto _LL138;} else{ goto _LL139;}
_LL139: goto _LL140; _LL138: Cyc_Arg_stop( initpos,( void*)({ struct Cyc_Arg_Message_struct*
_temp143=( struct Cyc_Arg_Message_struct*) GC_malloc( sizeof( struct Cyc_Arg_Message_struct));
_temp143[ 0]=({ struct Cyc_Arg_Message_struct _temp144; _temp144.tag= Cyc_Arg_Message;
_temp144.f1= _temp141; _temp144;}); _temp143;}), speclist, errmsg); goto _LL136;
_LL140:( void) _throw( _temp135); _LL136:;}}} ++ Cyc_Arg_current;}}}}
