 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp(
int, int); extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern struct Cyc_Std___sFILE* Cyc_Std_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Std_file_close(
struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Std_sscanf( struct _tagged_arr src,
struct _tagged_arr fmt, struct _tagged_arr); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{
int(* read_fun)( struct _tagged_arr, int, void*); void* read_fun_state; } ;
struct Cyc_Lexing_lex_tables{ struct _tagged_arr lex_base; struct _tagged_arr
lex_backtrk; struct _tagged_arr lex_default; struct _tagged_arr lex_trans;
struct _tagged_arr lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Std___sFILE*); extern struct _tagged_arr Cyc_Lexing_lexeme( struct
Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_merge_sort( int(* less_eq)( void*,
void*), struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; extern
unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern struct _tagged_arr
Cyc_Std_strdup( struct _tagged_arr src); extern struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr, int);
extern void Cyc_Lineno_poss_of_abss( struct _tagged_arr filename, struct Cyc_List_List*
places); static const int Cyc_Lineno_NEWLINE= 0; static const int Cyc_Lineno_LINEDEF=
1; static const int Cyc_Lineno_END= 2; const int Cyc_Lineno_lex_base[ 8u]=(
const int[ 8u]){ 0, 1, - 2, 2, 5, - 3, - 1, 6}; const int Cyc_Lineno_lex_backtrk[
8u]=( const int[ 8u]){ - 1, - 1, - 1, 1, - 1, - 1, - 1, 0}; const int Cyc_Lineno_lex_default[
8u]=( const int[ 8u]){ 1, 1, 0, - 1, 4, 0, 0, - 1}; const int Cyc_Lineno_lex_trans[
263u]=( const int[ 263u]){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 3, 6, 6, 0,
7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0,
5, 0}; const int Cyc_Lineno_lex_check[ 263u]=( const int[ 263u]){ - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, 3, 0, 1, 4, 7, - 1, 4, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, - 1, - 1, - 1, 4, - 1}; int
Cyc_Lineno_lex_engine( int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int
state; int base; int backtrk; int c; state= start_state; if( state >=  0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) -  1;} while( 1) { base= Cyc_Lineno_lex_base[
_check_known_subscript_notnull( 8u, state)]; if( base <  0){ return( - base) - 
1;} backtrk= Cyc_Lineno_lex_backtrk[ _check_known_subscript_notnull( 8u, state)];
if( backtrk >=  0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >=  lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) -  1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c ==  - 1){ c= 256;}} if( Cyc_Lineno_lex_check[
_check_known_subscript_notnull( 263u, base +  c)] ==  state){ state= Cyc_Lineno_lex_trans[
_check_known_subscript_notnull( 263u, base +  c)];} else{ state= Cyc_Lineno_lex_default[
_check_known_subscript_notnull( 8u, state)];} if( state <  0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action ==  - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp0=( struct Cyc_Lexing_Error_struct*)
_cycalloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp0[ 0]=({ struct Cyc_Lexing_Error_struct
_temp1; _temp1.tag= Cyc_Lexing_Error; _temp1.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp1;}); _temp0;}));} else{ return lbuf->lex_last_action;}}
else{ if( c ==  256){ lbuf->lex_eof_reached= 0;}}}} void* Cyc_Lineno_token_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_Lineno_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL2: return( void*) Cyc_Lineno_LINEDEF;
case 1: _LL3: return( void*) Cyc_Lineno_NEWLINE; case 2: _LL4: return( void*)
Cyc_Lineno_END; default: _LL5:( lexbuf->refill_buff)( lexbuf); return Cyc_Lineno_token_rec(
lexbuf, lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp7=( struct Cyc_Lexing_Error_struct*) _cycalloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp7[ 0]=({ struct Cyc_Lexing_Error_struct _temp8; _temp8.tag= Cyc_Lexing_Error;
_temp8.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp8;}); _temp7;}));} void* Cyc_Lineno_token( struct Cyc_Lexing_lexbuf* lexbuf){
return Cyc_Lineno_token_rec( lexbuf, 0);} struct Cyc_Lineno_Pos; struct _tuple0{
struct _tagged_arr f1; int f2; } ; static struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef(
struct _tagged_arr line){ struct _handler_cons _temp9; _push_handler(& _temp9);{
int _temp11= 0; if( setjmp( _temp9.handler)){ _temp11= 1;} if( ! _temp11){{ int
i= 0; while( i <  _get_arr_size( line, sizeof( unsigned char))?*(( unsigned char*)
_check_unknown_subscript( line, sizeof( unsigned char), i)) < '0'? 1:*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), i)) > '9':
0) { ++ i;}{ int j= i; while(( j <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) >= '0':
0)?*(( unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j))
<= '9': 0) { ++ j;} if( i ==  _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp12= 0; _npop_handler( 0u); return _temp12;}{ int
number= 0; if(({ struct Cyc_Std_IntPtr_sa_struct _temp14; _temp14.tag= Cyc_Std_IntPtr_sa;
_temp14.f1=& number;{ void* _temp13[ 1u]={& _temp14}; Cyc_Std_sscanf(( struct
_tagged_arr) Cyc_Std_substring(( struct _tagged_arr) line, i,( unsigned int)( j
-  i)), _tag_arr("%d", sizeof( unsigned char), 3u), _tag_arr( _temp13, sizeof(
void*), 1u));}}) !=  1){ struct Cyc_Core_Opt* _temp15= 0; _npop_handler( 0u);
return _temp15;} while( j <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) != '"':
0) { ++ j;}{ int k= ++ j; while( k <  _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), k)) != '"':
0) { ++ k;} if( j ==  _get_arr_size( line, sizeof( unsigned char))? 1: k == 
_get_arr_size( line, sizeof( unsigned char))){ struct Cyc_Core_Opt* _temp16= 0;
_npop_handler( 0u); return _temp16;}{ struct _tagged_arr fname= Cyc_Std_substring((
struct _tagged_arr) line, j,( unsigned int)( k -  j)); struct Cyc_Core_Opt*
_temp19=({ struct Cyc_Core_Opt* _temp17=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp17->v=( void*)({ struct _tuple0* _temp18=(
struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp18->f1= fname;
_temp18->f2= number; _temp18;}); _temp17;}); _npop_handler( 0u); return _temp19;}}}}};
_pop_handler();} else{ void* _temp10=( void*) _exn_thrown; void* _temp21=
_temp10; _LL23: goto _LL24; _LL25: goto _LL26; _LL24: return 0; _LL26:( void)
_throw( _temp21); _LL22:;}}} struct _tuple1{ int f1; struct Cyc_Lineno_Pos* f2;
} ; int Cyc_Lineno_place_cmp( struct _tuple1* place1, struct _tuple1* place2){
return Cyc_Core_intcmp((* place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places){ places=(( struct Cyc_List_List*(*)(
int(* less_eq)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp, places);{ struct Cyc_Std___sFILE* f= Cyc_Std_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u));{ struct _handler_cons
_temp27; _push_handler(& _temp27);{ int _temp29= 0; if( setjmp( _temp27.handler)){
_temp29= 1;} if( ! _temp29){{ struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file(
f); struct _tagged_arr _temp30= filename; int _temp31= 1; struct _tagged_arr
this_line; int eol; void* next; while( places !=  0) { while( 1) { next=(( void*(*)(
struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct
_tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lbuf); if( next
== ( void*) Cyc_Lineno_END? 1: eol > (*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1){ break;} if( next == ( void*) Cyc_Lineno_NEWLINE){
++ _temp31;} else{ struct Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef( this_line);
if( fno ==  0){ ++ _temp31;} else{ _temp30=( struct _tagged_arr)(*(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f1; _temp31=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f2;}}} while(
places !=  0? next == ( void*) Cyc_Lineno_END? 1: eol > (*(( struct _tuple1*)((
struct Cyc_List_List*) _check_null( places))->hd)).f1: 0) { struct Cyc_Lineno_Pos*
_temp32=(*(( struct _tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2;
_temp32->logical_file=( struct _tagged_arr) Cyc_Std_strdup( _temp30); _temp32->line=
this_line; _temp32->line_no= _temp31; _temp32->col=( int)( Cyc_Std_strlen((
struct _tagged_arr) this_line) - ( eol - (*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1)); if( _temp32->col <  0){ _temp32->col= 0;}
places=(( struct Cyc_List_List*) _check_null( places))->tl;} ++ _temp31;}};
_pop_handler();} else{ void* _temp28=( void*) _exn_thrown; void* _temp34=
_temp28; _LL36: goto _LL37; _LL38: goto _LL39; _LL37: Cyc_Std_file_close( f);(
int) _throw( _temp34); _LL39:( void) _throw( _temp34); _LL35:;}}} Cyc_Std_file_close(
f); return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr
filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct Cyc_Lineno_Pos* _temp42=(
struct Cyc_Lineno_Pos*) _cycalloc( sizeof( struct Cyc_Lineno_Pos)); _temp42->logical_file=
_tag_arr("", sizeof( unsigned char), 1u); _temp42->line= Cyc_Core_new_string( 0);
_temp42->line_no= 0; _temp42->col= 0; _temp42;}); Cyc_Lineno_poss_of_abss(
filename,({ struct Cyc_List_List* _temp40=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp40->hd=( void*)({ struct _tuple1* _temp41=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp41->f1= abs; _temp41->f2=
ans; _temp41;}); _temp40->tl= 0; _temp40;})); return ans;}
