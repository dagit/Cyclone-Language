#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct
_tuple0{ struct _tagged_string f1; int f2; } ; struct _tuple1{ int f1; struct
Cyc_Lineno_Pos* f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_intcmp( int, int);
extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern int Cyc_Core_int_of_string( struct _tagged_string); extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef
unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct
_tagged_string fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close(
struct Cyc_Stdio___sFILE*); extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); extern struct _tagged_string
Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ;
extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern struct
_tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
unsigned int n); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct
_tagged_string, int); extern void Cyc_Lineno_poss_of_abss( struct _tagged_string
filename, struct Cyc_List_List* places); struct Cyc_Lexing_lex_tables* Cyc_Lineno_lt=
0; int Cyc_Lineno_lbase[ 8u]={ 0, 1, - 2, 2, 5, - 3, - 1, 6}; int Cyc_Lineno_lbacktrk[
8u]={ - 1, - 1, - 1, 1, - 1, - 1, - 1, 0}; int Cyc_Lineno_ldefault[ 8u]={ 1, 1,
0, - 1, 4, 0, 0, - 1}; int Cyc_Lineno_ltrans[ 263u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 2, 2, 2, 3, 3, 6, 6, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 5, 5, 0, 0, 0, 5, 0}; int Cyc_Lineno_lcheck[ 263u]={ - 1, - 1, - 1,
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
Cyc_Lineno_token_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lineno_lt
== 0){ Cyc_Lineno_lt=({ struct Cyc_Lexing_lex_tables* _temp0=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp0->lex_base=( struct
_tagged_ptr0)({ int* _temp9=( int*)(( int*) Cyc_Lineno_lbase); struct
_tagged_ptr0 _temp10; _temp10.curr= _temp9; _temp10.base= _temp9; _temp10.last_plus_one=
_temp9 + 8; _temp10;}); _temp0->lex_backtrk=( struct _tagged_ptr0)({ int* _temp7=(
int*)(( int*) Cyc_Lineno_lbacktrk); struct _tagged_ptr0 _temp8; _temp8.curr=
_temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 + 8; _temp8;}); _temp0->lex_default=(
struct _tagged_ptr0)({ int* _temp5=( int*)(( int*) Cyc_Lineno_ldefault); struct
_tagged_ptr0 _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 8; _temp6;}); _temp0->lex_trans=( struct _tagged_ptr0)({ int* _temp3=(
int*)(( int*) Cyc_Lineno_ltrans); struct _tagged_ptr0 _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 263; _temp4;});
_temp0->lex_check=( struct _tagged_ptr0)({ int* _temp1=( int*)(( int*) Cyc_Lineno_lcheck);
struct _tagged_ptr0 _temp2; _temp2.curr= _temp1; _temp2.base= _temp1; _temp2.last_plus_one=
_temp1 + 263; _temp2;}); _temp0;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)({
struct Cyc_Lexing_lex_tables* _temp11= Cyc_Lineno_lt; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11;}), lexstate, lexbuf); switch( lexstate){ case 0:
_LL12: return 2; case 1: _LL13: return 1; case 2: _LL14: return 3; default:
_LL15:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lineno_token_rec)( lexbuf, lexstate);}( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct* _temp17=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp17[ 0]=({ struct Cyc_Lexing_Error_struct
_temp18; _temp18.tag= Cyc_Lexing_Error_tag; _temp18.f1=( struct _tagged_string)({
char* _temp19=( char*)"some action didn't return!"; struct _tagged_string
_temp20; _temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one=
_temp19 + 27; _temp20;}); _temp18;}); _temp17;}));} int Cyc_Lineno_token( struct
Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf,
int lexstate)) Cyc_Lineno_token_rec)( lexbuf, 0);} struct Cyc_Lineno_Pos; static
struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef( struct _tagged_string line){
struct _handler_cons _temp21; _push_handler(& _temp21);{ struct _xtunion_struct*
_temp22=( struct _xtunion_struct*) setjmp( _temp21.handler); if( ! _temp22){{
int i= 0; while(( unsigned int) i <({ struct _tagged_string _temp23= line;(
unsigned int)( _temp23.last_plus_one - _temp23.curr);})?( int)({ struct
_tagged_string _temp24= line; char* _temp26= _temp24.curr + i; if( _temp26 <
_temp24.base? 1: _temp26 >= _temp24.last_plus_one){ _throw( Null_Exception);}*
_temp26;}) <( int)'0'? 1:( int)({ struct _tagged_string _temp27= line; char*
_temp29= _temp27.curr + i; if( _temp29 < _temp27.base? 1: _temp29 >= _temp27.last_plus_one){
_throw( Null_Exception);}* _temp29;}) >( int)'9': 0) { ++ i;}{ int j= i; while(((
unsigned int) j <({ struct _tagged_string _temp30= line;( unsigned int)( _temp30.last_plus_one
- _temp30.curr);})?( int)({ struct _tagged_string _temp31= line; char* _temp33=
_temp31.curr + j; if( _temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one){
_throw( Null_Exception);}* _temp33;}) >=( int)'0': 0)?( int)({ struct
_tagged_string _temp34= line; char* _temp36= _temp34.curr + j; if( _temp36 <
_temp34.base? 1: _temp36 >= _temp34.last_plus_one){ _throw( Null_Exception);}*
_temp36;}) <=( int)'9': 0) { ++ j;} if(( unsigned int) i ==({ struct
_tagged_string _temp37= line;( unsigned int)( _temp37.last_plus_one - _temp37.curr);})){
struct Cyc_Core_Opt* _temp38= 0; _npop_handler( 0u); return _temp38;}{ int
number= Cyc_Core_int_of_string( Cyc_String_substring( line, i,( unsigned int)( j
- i))); while(( unsigned int) j <({ struct _tagged_string _temp39= line;(
unsigned int)( _temp39.last_plus_one - _temp39.curr);})?( int)({ struct
_tagged_string _temp40= line; char* _temp42= _temp40.curr + j; if( _temp42 <
_temp40.base? 1: _temp42 >= _temp40.last_plus_one){ _throw( Null_Exception);}*
_temp42;}) !=( int)'"': 0) { ++ j;}{ int k= ++ j; while(( unsigned int) k <({
struct _tagged_string _temp43= line;( unsigned int)( _temp43.last_plus_one -
_temp43.curr);})?( int)({ struct _tagged_string _temp44= line; char* _temp46=
_temp44.curr + k; if( _temp46 < _temp44.base? 1: _temp46 >= _temp44.last_plus_one){
_throw( Null_Exception);}* _temp46;}) !=( int)'"': 0) { ++ k;} if(( unsigned int)
j ==({ struct _tagged_string _temp47= line;( unsigned int)( _temp47.last_plus_one
- _temp47.curr);})? 1:( unsigned int) k ==({ struct _tagged_string _temp48= line;(
unsigned int)( _temp48.last_plus_one - _temp48.curr);})){ struct Cyc_Core_Opt*
_temp49= 0; _npop_handler( 0u); return _temp49;}{ struct _tagged_string fname=
Cyc_String_substring( line, j,( unsigned int)( k - j)); struct Cyc_Core_Opt*
_temp52=({ struct Cyc_Core_Opt* _temp50=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp50->v=( void*)({ struct _tuple0* _temp51=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp51->f1= fname;
_temp51->f2= number; _temp51;}); _temp50;}); _npop_handler( 0u); return _temp52;}}}}};
_pop_handler();} else{ struct _xtunion_struct* _temp54= _temp22; _LL56: goto
_LL57; _LL58: goto _LL59; _LL57: return 0; _LL59:( void) _throw( _temp54); _LL55:;}}}
int Cyc_Lineno_place_cmp( struct _tuple1* place1, struct _tuple1* place2){
return Cyc_Core_intcmp((* place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss(
struct _tagged_string filename, struct Cyc_List_List* places){ places=(( struct
Cyc_List_List*(*)( int(* less_eq)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_merge_sort)( Cyc_Lineno_place_cmp, places);{ struct Cyc_Stdio___sFILE*
f= Cyc_Stdio_file_open( filename,( struct _tagged_string)({ char* _temp60=( char*)"r";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 2; _temp61;}));{ struct _handler_cons _temp62;
_push_handler(& _temp62);{ struct _xtunion_struct* _temp63=( struct
_xtunion_struct*) setjmp( _temp62.handler); if( ! _temp63){{ struct Cyc_Lexing_lexbuf*
lbuf= Cyc_Lexing_from_file( f); struct _tagged_string source_file= filename; int
line= 1; struct _tagged_string this_line; int eol; int next; while( places != 0) {
while( 1) { next=(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)(
lbuf); eol=(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf);
this_line=(( struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)(
lbuf); if( eol >=(*(( struct _tuple1*) places->hd)).f1){ break;} switch( next){
case 3: _LL64:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp66=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp66[ 0]=({ struct Cyc_Core_Failure_struct _temp67; _temp67.tag= Cyc_Core_Failure_tag;
_temp67.f1=( struct _tagged_string)({ char* _temp68=( char*)"Lineno: Invalid file position";
struct _tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68;
_temp69.last_plus_one= _temp68 + 30; _temp69;}); _temp67;}); _temp66;})); case 1:
_LL65: ++ line; break; case 2: _LL70: { struct Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef(
this_line); if( fno == 0){ ++ line;({ struct _tagged_string _temp72= this_line;
fprintf( Cyc_Stdio_stderr,"Unknown directive: %.*s\n", _temp72.last_plus_one -
_temp72.curr, _temp72.curr);});} else{ source_file=(*(( struct _tuple0*) fno->v)).f1;
line=(*(( struct _tuple0*) fno->v)).f2;} break;} default: _LL71:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp74=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp74[ 0]=({ struct Cyc_Core_Failure_struct
_temp75; _temp75.tag= Cyc_Core_Failure_tag; _temp75.f1=( struct _tagged_string)({
char* _temp76=( char*)"Lineno: Impossible"; struct _tagged_string _temp77;
_temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one= _temp76 +
19; _temp77;}); _temp75;}); _temp74;}));}} while( places != 0? eol >=(*(( struct
_tuple1*) places->hd)).f1: 0) { struct Cyc_Lineno_Pos* p=(*(( struct _tuple1*)
places->hd)).f2; p->logical_file= source_file; p->line= this_line; p->line_no=
line; p->col=( int)( Cyc_String_strlen( this_line) -( unsigned int)( eol -(*((
struct _tuple1*) places->hd)).f1)); places= places->tl;} ++ line;}};
_pop_handler();} else{ struct _xtunion_struct* _temp79= _temp63; _LL81: goto
_LL82; _LL83: goto _LL84; _LL82: Cyc_Stdio_file_close( f);( void) _throw(
_temp79); _LL84:( void) _throw( _temp79); _LL80:;}}} Cyc_Stdio_file_close( f);
return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct _tagged_string
filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct Cyc_Lineno_Pos* _temp85=(
struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos)); _temp85->logical_file=(
struct _tagged_string)({ char* _temp88=( char*)""; struct _tagged_string _temp89;
_temp89.curr= _temp88; _temp89.base= _temp88; _temp89.last_plus_one= _temp88 + 1;
_temp89;}); _temp85->line=( struct _tagged_string)({ char* _temp86=( char*)"";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 1; _temp87;}); _temp85->line_no= 0; _temp85->col=
0; _temp85;}); Cyc_Lineno_poss_of_abss( filename,({ struct Cyc_List_List*
_temp90=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*)({ struct _tuple1* _temp91=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp91->f1= abs; _temp91->f2= ans; _temp91;});
_temp90->tl= 0; _temp90;})); return ans;}