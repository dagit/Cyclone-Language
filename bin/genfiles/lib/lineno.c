 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp( int,
int); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
_tagged_arr f1; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_arr fname, struct _tagged_arr mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
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
int tag; float* f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*);
extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern
int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen( struct
_tagged_arr s); extern struct _tagged_arr Cyc_String_substring( struct
_tagged_arr, int ofs, int n); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern struct
Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr, int); extern void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places); const int Cyc_Lineno_lex_base[
8u]=( const int[ 8u]){ 0, 1, - 2, 2, 5, - 3, - 1, 6}; const int Cyc_Lineno_lex_backtrk[
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
state; int base; int backtrk; int c; state= start_state; if( state >= 0){ lbuf->lex_last_pos=(
lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{
state=( - state) - 1;} while( 1) { base= Cyc_Lineno_lex_base[
_check_known_subscript_notnull( 8u, state)]; if( base < 0){ return( - base) - 1;}
backtrk= Cyc_Lineno_lex_backtrk[ _check_known_subscript_notnull( 8u, state)];
if( backtrk >= 0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >= lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) - 1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c == - 1){ c= 256;}} if( Cyc_Lineno_lex_check[
_check_known_subscript_notnull( 263u, base + c)] == state){ state= Cyc_Lineno_lex_trans[
_check_known_subscript_notnull( 263u, base + c)];} else{ state= Cyc_Lineno_lex_default[
_check_known_subscript_notnull( 8u, state)];} if( state < 0){ lbuf->lex_curr_pos=
lbuf->lex_last_pos; if( lbuf->lex_last_action == - 1){( int) _throw(( void*)({
struct Cyc_Lexing_Error_struct* _temp0=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp0[ 0]=({ struct Cyc_Lexing_Error_struct
_temp1; _temp1.tag= Cyc_Lexing_Error; _temp1.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp1;}); _temp0;}));} else{ return lbuf->lex_last_action;}}
else{ if( c == 256){ lbuf->lex_eof_reached= 0;}}}} int Cyc_Lineno_token_rec(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ lexstate= Cyc_Lineno_lex_engine(
lexstate, lexbuf); switch( lexstate){ case 0: _LL2: return 2; case 1: _LL3:
return 1; case 2: _LL4: return 3; default: _LL5:( lexbuf->refill_buff)( lexbuf);
return Cyc_Lineno_token_rec( lexbuf, lexstate);}( int) _throw(( void*)({ struct
Cyc_Lexing_Error_struct* _temp7=( struct Cyc_Lexing_Error_struct*) GC_malloc(
sizeof( struct Cyc_Lexing_Error_struct)); _temp7[ 0]=({ struct Cyc_Lexing_Error_struct
_temp8; _temp8.tag= Cyc_Lexing_Error; _temp8.f1= _tag_arr("some action didn't return!",
sizeof( unsigned char), 27u); _temp8;}); _temp7;}));} int Cyc_Lineno_token(
struct Cyc_Lexing_lexbuf* lexbuf){ return Cyc_Lineno_token_rec( lexbuf, 0);}
struct Cyc_Lineno_Pos; struct _tuple0{ struct _tagged_arr f1; int f2; } ; static
struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef( struct _tagged_arr line){ struct
_handler_cons _temp9; _push_handler(& _temp9);{ int _temp11= 0; if( setjmp(
_temp9.handler)){ _temp11= 1;} if( ! _temp11){{ int i= 0; while( i <
_get_arr_size( line, sizeof( unsigned char))?*(( unsigned char*)
_check_unknown_subscript( line, sizeof( unsigned char), i)) <'0'? 1:*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), i)) >'9':
0) { ++ i;}{ int j= i; while(( j < _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) >='0':
0)?*(( unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j))
<='9': 0) { ++ j;} if( i == _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp12= 0; _npop_handler( 0u); return _temp12;}{ int
number= Cyc_Core_int_of_string(( struct _tagged_arr) Cyc_String_substring((
struct _tagged_arr) line, i, j - i)); while( j < _get_arr_size( line, sizeof(
unsigned char))?*(( unsigned char*) _check_unknown_subscript( line, sizeof(
unsigned char), j)) !='"': 0) { ++ j;}{ int k= ++ j; while( k < _get_arr_size(
line, sizeof( unsigned char))?*(( unsigned char*) _check_unknown_subscript( line,
sizeof( unsigned char), k)) !='"': 0) { ++ k;} if( j == _get_arr_size( line,
sizeof( unsigned char))? 1: k == _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp13= 0; _npop_handler( 0u); return _temp13;}{ struct
_tagged_arr fname= Cyc_String_substring(( struct _tagged_arr) line, j, k - j);
struct Cyc_Core_Opt* _temp16=({ struct Cyc_Core_Opt* _temp14=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp14->v=( void*)({ struct _tuple0*
_temp15=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp15->f1=
fname; _temp15->f2= number; _temp15;}); _temp14;}); _npop_handler( 0u); return
_temp16;}}}}}; _pop_handler();} else{ void* _temp10=( void*) _exn_thrown; void*
_temp18= _temp10; _LL20: goto _LL21; _LL22: goto _LL23; _LL21: return 0; _LL23:(
void) _throw( _temp18); _LL19:;}}} struct _tuple1{ int f1; struct Cyc_Lineno_Pos*
f2; } ; int Cyc_Lineno_place_cmp( struct _tuple1* place1, struct _tuple1* place2){
return Cyc_Core_intcmp((* place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places){ places=(( struct Cyc_List_List*(*)(
int(* less_eq)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp, places);{ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u));{ struct _handler_cons
_temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp( _temp24.handler)){
_temp26= 1;} if( ! _temp26){{ struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file(
f); struct _tagged_arr source_file= filename; int line= 1; struct _tagged_arr
this_line; int eol; int next; while( places != 0) { while( 1) { next=(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct
_tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lbuf); if( eol
>=(*(( struct _tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f1){
break;} switch( next){ case 3: _LL27:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp29=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp29[ 0]=({ struct Cyc_Core_Failure_struct _temp30; _temp30.tag= Cyc_Core_Failure;
_temp30.f1= _tag_arr("Lineno: Invalid file position", sizeof( unsigned char), 30u);
_temp30;}); _temp29;})); case 1: _LL28: ++ line; break; case 2: _LL31: { struct
Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef( this_line); if( fno == 0){ ++ line;({
struct Cyc_Stdio_String_pa_struct _temp34; _temp34.tag= Cyc_Stdio_String_pa;
_temp34.f1=( struct _tagged_arr) this_line;{ void* _temp33[ 1u]={& _temp34}; Cyc_Stdio_fprintf(
Cyc_Stdio_stderr, _tag_arr("Unknown directive: %s\n", sizeof( unsigned char), 23u),
_tag_arr( _temp33, sizeof( void*), 1u));}});} else{ source_file=( struct
_tagged_arr)(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f1;
line=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f2;}
break;} default: _LL32:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp36=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp36[ 0]=({ struct Cyc_Core_Failure_struct _temp37; _temp37.tag= Cyc_Core_Failure;
_temp37.f1= _tag_arr("Lineno: Impossible", sizeof( unsigned char), 19u); _temp37;});
_temp36;}));}} while( places != 0? eol >=(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1: 0) { struct Cyc_Lineno_Pos* _temp38=(*(( struct
_tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2; _temp38->logical_file=
source_file; _temp38->line= this_line; _temp38->line_no= line; _temp38->col= Cyc_String_strlen((
struct _tagged_arr) this_line) -( eol -(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1); places=(( struct Cyc_List_List*) _check_null(
places))->tl;} ++ line;}}; _pop_handler();} else{ void* _temp25=( void*)
_exn_thrown; void* _temp40= _temp25; _LL42: goto _LL43; _LL44: goto _LL45; _LL43:
Cyc_Stdio_file_close( f);( int) _throw( _temp40); _LL45:( void) _throw( _temp40);
_LL41:;}}} Cyc_Stdio_file_close( f); return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs(
struct _tagged_arr filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct Cyc_Lineno_Pos*
_temp48=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos));
_temp48->logical_file= _tag_arr("", sizeof( unsigned char), 1u); _temp48->line=
Cyc_Core_new_string( 0); _temp48->line_no= 0; _temp48->col= 0; _temp48;}); Cyc_Lineno_poss_of_abss(
filename,({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)({ struct _tuple1* _temp47=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp47->f1= abs; _temp47->f2=
ans; _temp47;}); _temp46->tl= 0; _temp46;})); return ans;}
