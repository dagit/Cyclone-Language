 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_arr Cyc_Core_new_string( int); extern int Cyc_Core_intcmp(
int, int); extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
struct Cyc_Stdio___sFILE*); extern unsigned char Cyc_Lexing_Error[ 10u]; struct
Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_arr f1; } ; struct
Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_arr lex_buffer; int lex_buffer_len; int lex_abs_pos;
int lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_arr, int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*);
extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern
int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen( struct
_tagged_arr s); extern struct _tagged_arr Cyc_String_substring( struct
_tagged_arr, int ofs, int n); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern struct
Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr, int); extern void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places); struct Cyc_Lexing_lex_tables*
Cyc_Lineno_lt= 0; int Cyc_Lineno_lbase[ 8u]={ 0, 1, - 2, 2, 5, - 3, - 1, 6}; int
Cyc_Lineno_lbacktrk[ 8u]={ - 1, - 1, - 1, 1, - 1, - 1, - 1, 0}; int Cyc_Lineno_ldefault[
8u]={ 1, 1, 0, - 1, 4, 0, 0, - 1}; int Cyc_Lineno_ltrans[ 263u]={ 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 2, 2, 3, 3, 6, 6, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 5, 0}; int Cyc_Lineno_lcheck[ 263u]={ - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, 3, 0, 1, 4, 7, - 1, 4, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1,
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
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 1, - 1, - 1, - 1, 4, -
1}; int Cyc_Lineno_token_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){
if( Cyc_Lineno_lt == 0){ Cyc_Lineno_lt=({ struct Cyc_Lexing_lex_tables* _temp0=(
struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp0->lex_base= _tag_arr(( int*) Cyc_Lineno_lbase, sizeof( int), 8u); _temp0->lex_backtrk=
_tag_arr(( int*) Cyc_Lineno_lbacktrk, sizeof( int), 8u); _temp0->lex_default=
_tag_arr(( int*) Cyc_Lineno_ldefault, sizeof( int), 8u); _temp0->lex_trans=
_tag_arr(( int*) Cyc_Lineno_ltrans, sizeof( int), 263u); _temp0->lex_check=
_tag_arr(( int*) Cyc_Lineno_lcheck, sizeof( int), 263u); _temp0;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*) _check_null( Cyc_Lineno_lt), lexstate, lexbuf);
switch( lexstate){ case 0: _LL1: return 2; case 1: _LL2: return 1; case 2: _LL3:
return 3; default: _LL4:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lineno_token_rec)( lexbuf,
lexstate);}( int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp6=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp6[ 0]=({ struct Cyc_Lexing_Error_struct _temp7; _temp7.tag= Cyc_Lexing_Error;
_temp7.f1= _tag_arr("some action didn't return!", sizeof( unsigned char), 27u);
_temp7;}); _temp6;}));} int Cyc_Lineno_token( struct Cyc_Lexing_lexbuf* lexbuf){
return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lineno_token_rec)(
lexbuf, 0);} struct Cyc_Lineno_Pos; struct _tuple0{ struct _tagged_arr f1; int
f2; } ; static struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef( struct _tagged_arr
line){ struct _handler_cons _temp8; _push_handler(& _temp8);{ int _temp10= 0;
if( setjmp( _temp8.handler)){ _temp10= 1;} if( ! _temp10){{ int i= 0; while( i <
_get_arr_size( line, sizeof( unsigned char))?*(( unsigned char*)
_check_unknown_subscript( line, sizeof( unsigned char), i)) <'0'? 1:*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), i)) >'9':
0) { ++ i;}{ int j= i; while(( j < _get_arr_size( line, sizeof( unsigned char))?*((
unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j)) >='0':
0)?*(( unsigned char*) _check_unknown_subscript( line, sizeof( unsigned char), j))
<='9': 0) { ++ j;} if( i == _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp11= 0; _npop_handler( 0u); return _temp11;}{ int
number= Cyc_Core_int_of_string(( struct _tagged_arr) Cyc_String_substring((
struct _tagged_arr) line, i, j - i)); while( j < _get_arr_size( line, sizeof(
unsigned char))?*(( unsigned char*) _check_unknown_subscript( line, sizeof(
unsigned char), j)) !='"': 0) { ++ j;}{ int k= ++ j; while( k < _get_arr_size(
line, sizeof( unsigned char))?*(( unsigned char*) _check_unknown_subscript( line,
sizeof( unsigned char), k)) !='"': 0) { ++ k;} if( j == _get_arr_size( line,
sizeof( unsigned char))? 1: k == _get_arr_size( line, sizeof( unsigned char))){
struct Cyc_Core_Opt* _temp12= 0; _npop_handler( 0u); return _temp12;}{ struct
_tagged_arr fname= Cyc_String_substring(( struct _tagged_arr) line, j, k - j);
struct Cyc_Core_Opt* _temp15=({ struct Cyc_Core_Opt* _temp13=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp13->v=( void*)({ struct _tuple0*
_temp14=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp14->f1=
fname; _temp14->f2= number; _temp14;}); _temp13;}); _npop_handler( 0u); return
_temp15;}}}}}; _pop_handler();} else{ void* _temp9=( void*) _exn_thrown; void*
_temp17= _temp9; _LL19: goto _LL20; _LL21: goto _LL22; _LL20: return 0; _LL22:(
void) _throw( _temp17); _LL18:;}}} struct _tuple1{ int f1; struct Cyc_Lineno_Pos*
f2; } ; int Cyc_Lineno_place_cmp( struct _tuple1* place1, struct _tuple1* place2){
return Cyc_Core_intcmp((* place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places){ places=(( struct Cyc_List_List*(*)(
int(* less_eq)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp, places);{ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u));{ struct _handler_cons
_temp23; _push_handler(& _temp23);{ int _temp25= 0; if( setjmp( _temp23.handler)){
_temp25= 1;} if( ! _temp25){{ struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file(
f); struct _tagged_arr source_file= filename; int line= 1; struct _tagged_arr
this_line; int eol; int next; while( places != 0) { while( 1) { next=(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct
_tagged_arr(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lbuf); if( eol
>=(*(( struct _tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f1){
break;} switch( next){ case 3: _LL26:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp28=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp28[ 0]=({ struct Cyc_Core_Failure_struct _temp29; _temp29.tag= Cyc_Core_Failure;
_temp29.f1= _tag_arr("Lineno: Invalid file position", sizeof( unsigned char), 30u);
_temp29;}); _temp28;})); case 1: _LL27: ++ line; break; case 2: _LL30: { struct
Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef( this_line); if( fno == 0){ ++ line;({
struct _tagged_arr _temp32=( struct _tagged_arr) this_line; fprintf(
_sfile_to_file( Cyc_Stdio_stderr),"Unknown directive: %.*s\n", _get_arr_size(
_temp32, 1u), _temp32.curr);});} else{ source_file=( struct _tagged_arr)(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f1; line=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( fno))->v)).f2;} break;}
default: _LL31:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp34=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp34[ 0]=({ struct Cyc_Core_Failure_struct _temp35; _temp35.tag= Cyc_Core_Failure;
_temp35.f1= _tag_arr("Lineno: Impossible", sizeof( unsigned char), 19u); _temp35;});
_temp34;}));}} while( places != 0? eol >=(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1: 0) { struct Cyc_Lineno_Pos* _temp36=(*(( struct
_tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2; goto _LL37;
_LL37: _temp36->logical_file= source_file; _temp36->line= this_line; _temp36->line_no=
line; _temp36->col= Cyc_String_strlen(( struct _tagged_arr) this_line) -( eol -(*((
struct _tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f1);
places=(( struct Cyc_List_List*) _check_null( places))->tl;} ++ line;}};
_pop_handler();} else{ void* _temp24=( void*) _exn_thrown; void* _temp39=
_temp24; _LL41: goto _LL42; _LL43: goto _LL44; _LL42: Cyc_Stdio_file_close( f);(
int) _throw( _temp39); _LL44:( void) _throw( _temp39); _LL40:;}}} Cyc_Stdio_file_close(
f); return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_arr
filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct Cyc_Lineno_Pos* _temp47=(
struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos)); _temp47->logical_file=
_tag_arr("", sizeof( unsigned char), 1u); _temp47->line= Cyc_Core_new_string( 0);
_temp47->line_no= 0; _temp47->col= 0; _temp47;}); Cyc_Lineno_poss_of_abss(
filename,({ struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp45->hd=( void*)({ struct _tuple1* _temp46=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp46->f1= abs; _temp46->f2=
ans; _temp46;}); _temp45->tl= 0; _temp45;})); return ans;}