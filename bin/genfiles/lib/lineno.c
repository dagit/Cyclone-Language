#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t;
typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t;
typedef unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
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
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_file_open( struct _tagged_string fname, struct _tagged_string mode);
extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*); extern char Cyc_Lexing_Error_tag[
6u]; struct Cyc_Lexing_Error_struct{ char* tag; struct _tagged_string f1; } ;
struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Stdio___sFILE*); extern struct _tagged_string Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
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
int lexstate)) Cyc_Lineno_token_rec)( lexbuf, 0);} struct Cyc_Lineno_Pos; struct
_tuple0{ struct _tagged_string f1; int f2; } ; static struct Cyc_Core_Opt* Cyc_Lineno_parse_linedef(
struct _tagged_string line){ struct _handler_cons _temp21; _push_handler(&
_temp21);{ struct _xtunion_struct* _temp22=( struct _xtunion_struct*) setjmp(
_temp21.handler); if( ! _temp22){{ int i= 0; while( i <({ struct _tagged_string
_temp23= line;( unsigned int)( _temp23.last_plus_one - _temp23.curr);})?({
struct _tagged_string _temp24= line; char* _temp26= _temp24.curr + i; if(
_temp24.base == 0? 1:( _temp26 < _temp24.base? 1: _temp26 >= _temp24.last_plus_one)){
_throw( Null_Exception);}* _temp26;}) <'0'? 1:({ struct _tagged_string _temp27=
line; char* _temp29= _temp27.curr + i; if( _temp27.base == 0? 1:( _temp29 <
_temp27.base? 1: _temp29 >= _temp27.last_plus_one)){ _throw( Null_Exception);}*
_temp29;}) >'9': 0) { ++ i;}{ int j= i; while(( j <({ struct _tagged_string
_temp30= line;( unsigned int)( _temp30.last_plus_one - _temp30.curr);})?({
struct _tagged_string _temp31= line; char* _temp33= _temp31.curr + j; if(
_temp31.base == 0? 1:( _temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one)){
_throw( Null_Exception);}* _temp33;}) >='0': 0)?({ struct _tagged_string _temp34=
line; char* _temp36= _temp34.curr + j; if( _temp34.base == 0? 1:( _temp36 <
_temp34.base? 1: _temp36 >= _temp34.last_plus_one)){ _throw( Null_Exception);}*
_temp36;}) <='9': 0) { ++ j;} if( i ==({ struct _tagged_string _temp37= line;(
unsigned int)( _temp37.last_plus_one - _temp37.curr);})){ struct Cyc_Core_Opt*
_temp38= 0; _npop_handler( 0u); return _temp38;}{ int number= Cyc_Core_int_of_string(
Cyc_String_substring( line, i,( unsigned int)( j - i))); while( j <({ struct
_tagged_string _temp39= line;( unsigned int)( _temp39.last_plus_one - _temp39.curr);})?({
struct _tagged_string _temp40= line; char* _temp42= _temp40.curr + j; if(
_temp40.base == 0? 1:( _temp42 < _temp40.base? 1: _temp42 >= _temp40.last_plus_one)){
_throw( Null_Exception);}* _temp42;}) !='"': 0) { ++ j;}{ int k= ++ j; while( k
<({ struct _tagged_string _temp43= line;( unsigned int)( _temp43.last_plus_one -
_temp43.curr);})?({ struct _tagged_string _temp44= line; char* _temp46= _temp44.curr
+ k; if( _temp44.base == 0? 1:( _temp46 < _temp44.base? 1: _temp46 >= _temp44.last_plus_one)){
_throw( Null_Exception);}* _temp46;}) !='"': 0) { ++ k;} if( j ==({ struct
_tagged_string _temp47= line;( unsigned int)( _temp47.last_plus_one - _temp47.curr);})?
1: k ==({ struct _tagged_string _temp48= line;( unsigned int)( _temp48.last_plus_one
- _temp48.curr);})){ struct Cyc_Core_Opt* _temp49= 0; _npop_handler( 0u); return
_temp49;}{ struct _tagged_string fname= Cyc_String_substring( line, j,(
unsigned int)( k - j)); struct Cyc_Core_Opt* _temp52=({ struct Cyc_Core_Opt*
_temp50=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp50->v=( void*)({ struct _tuple0* _temp51=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp51->f1= fname; _temp51->f2= number; _temp51;});
_temp50;}); _npop_handler( 0u); return _temp52;}}}}}; _pop_handler();} else{
struct _xtunion_struct* _temp54= _temp22; _LL56: goto _LL57; _LL58: goto _LL59;
_LL57: return 0; _LL59:( void) _throw( _temp54); _LL55:;}}} struct _tuple1{ int
f1; struct Cyc_Lineno_Pos* f2; } ; int Cyc_Lineno_place_cmp( struct _tuple1*
place1, struct _tuple1* place2){ return Cyc_Core_intcmp((* place1).f1,(* place2).f1);}
void Cyc_Lineno_poss_of_abss( struct _tagged_string filename, struct Cyc_List_List*
places){ places=(( struct Cyc_List_List*(*)( int(* less_eq)( struct _tuple1*,
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)( Cyc_Lineno_place_cmp,
places);{ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_file_open( filename,( struct
_tagged_string)({ char* _temp90=( char*)"r"; struct _tagged_string _temp91;
_temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 2;
_temp91;}));{ struct _handler_cons _temp60; _push_handler(& _temp60);{ struct
_xtunion_struct* _temp61=( struct _xtunion_struct*) setjmp( _temp60.handler);
if( ! _temp61){{ struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file( f);
struct _tagged_string source_file= filename; int line= 1; struct _tagged_string
this_line; int eol; int next; while( places != 0) { while( 1) { next=(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lbuf); if(
eol >=(*(( struct _tuple1*)({ struct Cyc_List_List* _temp62= places; if( _temp62
== 0){ _throw( Null_Exception);} _temp62->hd;}))).f1){ break;} switch( next){
case 3: _LL63:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp65=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp65[ 0]=({ struct Cyc_Core_Failure_struct _temp66; _temp66.tag= Cyc_Core_Failure_tag;
_temp66.f1=( struct _tagged_string)({ char* _temp67=( char*)"Lineno: Invalid file position";
struct _tagged_string _temp68; _temp68.curr= _temp67; _temp68.base= _temp67;
_temp68.last_plus_one= _temp67 + 30; _temp68;}); _temp66;}); _temp65;})); case 1:
_LL64: ++ line; break; case 2: _LL69: { struct Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef(
this_line); if( fno == 0){ ++ line;({ struct _tagged_string _temp71= this_line;
fprintf( Cyc_Stdio_stderr,"Unknown directive: %.*s\n", _temp71.last_plus_one -
_temp71.curr, _temp71.curr);});} else{ source_file=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp72= fno; if( _temp72 == 0){ _throw( Null_Exception);}
_temp72->v;}))).f1; line=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp73=
fno; if( _temp73 == 0){ _throw( Null_Exception);} _temp73->v;}))).f2;} break;}
default: _LL70:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct*
_temp75=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp75[ 0]=({ struct Cyc_Core_Failure_struct _temp76; _temp76.tag= Cyc_Core_Failure_tag;
_temp76.f1=( struct _tagged_string)({ char* _temp77=( char*)"Lineno: Impossible";
struct _tagged_string _temp78; _temp78.curr= _temp77; _temp78.base= _temp77;
_temp78.last_plus_one= _temp77 + 19; _temp78;}); _temp76;}); _temp75;}));}}
while( places != 0? eol >=(*(( struct _tuple1*)({ struct Cyc_List_List* _temp79=
places; if( _temp79 == 0){ _throw( Null_Exception);} _temp79->hd;}))).f1: 0) {
struct Cyc_Lineno_Pos* p=(*(( struct _tuple1*)({ struct Cyc_List_List* _temp82=
places; if( _temp82 == 0){ _throw( Null_Exception);} _temp82->hd;}))).f2; p->logical_file=
source_file; p->line= this_line; p->line_no= line; p->col=( int)( Cyc_String_strlen(
this_line) -( eol -(*(( struct _tuple1*)({ struct Cyc_List_List* _temp80= places;
if( _temp80 == 0){ _throw( Null_Exception);} _temp80->hd;}))).f1)); places=({
struct Cyc_List_List* _temp81= places; if( _temp81 == 0){ _throw( Null_Exception);}
_temp81->tl;});} ++ line;}}; _pop_handler();} else{ struct _xtunion_struct*
_temp84= _temp61; _LL86: goto _LL87; _LL88: goto _LL89; _LL87: Cyc_Stdio_file_close(
f);( void) _throw( _temp84); _LL89:( void) _throw( _temp84); _LL85:;}}} Cyc_Stdio_file_close(
f); return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct
_tagged_string filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct Cyc_Lineno_Pos*
_temp94=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos));
_temp94->logical_file=( struct _tagged_string)({ char* _temp97=( char*)"";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 1; _temp98;}); _temp94->line=( struct
_tagged_string)({ char* _temp95=( char*)""; struct _tagged_string _temp96;
_temp96.curr= _temp95; _temp96.base= _temp95; _temp96.last_plus_one= _temp95 + 1;
_temp96;}); _temp94->line_no= 0; _temp94->col= 0; _temp94;}); Cyc_Lineno_poss_of_abss(
filename,({ struct Cyc_List_List* _temp92=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp92->hd=( void*)({ struct _tuple1* _temp93=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp93->f1= abs; _temp93->f2=
ans; _temp93;}); _temp92->tl= 0; _temp92;})); return ans;}