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
extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u]; struct
Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag; struct
_tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern int Cyc_Core_int_of_string( struct _tagged_string); extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_file_open( struct _tagged_string
fname, struct _tagged_string mode); extern void Cyc_Stdio_file_close( struct Cyc_Stdio___sFILE*);
extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{ char* tag;
struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)(
struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_string lex_buffer;
int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int lex_curr_pos; int
lex_last_pos; int lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf*
Cyc_Lexing_Lexbuf; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x); extern char Cyc_List_Nth[
8u]; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern
struct _tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
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
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)((
struct Cyc_Lexing_lex_tables*) _check_null( Cyc_Lineno_lt)), lexstate, lexbuf);
switch( lexstate){ case 0: _LL11: return 2; case 1: _LL12: return 1; case 2:
_LL13: return 3; default: _LL14:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lineno_token_rec)( lexbuf,
lexstate);}( void) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp16=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp16[ 0]=({ struct Cyc_Lexing_Error_struct _temp17; _temp17.tag= Cyc_Lexing_Error;
_temp17.f1=( struct _tagged_string)({ char* _temp18=( char*)"some action didn't return!";
struct _tagged_string _temp19; _temp19.curr= _temp18; _temp19.base= _temp18;
_temp19.last_plus_one= _temp18 + 27; _temp19;}); _temp17;}); _temp16;}));} int
Cyc_Lineno_token( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lineno_token_rec)( lexbuf, 0);} struct Cyc_Lineno_Pos;
struct _tuple0{ struct _tagged_string f1; int f2; } ; static struct Cyc_Core_Opt*
Cyc_Lineno_parse_linedef( struct _tagged_string line){ struct _handler_cons
_temp20; _push_handler(& _temp20);{ int _temp22= 0; if( setjmp( _temp20.handler)){
_temp22= 1;} if( ! _temp22){{ int i= 0; while( i <({ struct _tagged_string
_temp23= line;( unsigned int)( _temp23.last_plus_one - _temp23.curr);})?*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
line, sizeof( char), i) <'0'? 1:*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( line, sizeof( char), i)
>'9': 0) { ++ i;}{ int j= i; while(( j <({ struct _tagged_string _temp24= line;(
unsigned int)( _temp24.last_plus_one - _temp24.curr);})?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( line,
sizeof( char), j) >='0': 0)?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( line, sizeof( char), j) <='9': 0) { ++
j;} if( i ==({ struct _tagged_string _temp25= line;( unsigned int)( _temp25.last_plus_one
- _temp25.curr);})){ struct Cyc_Core_Opt* _temp26= 0; _npop_handler( 0u); return
_temp26;}{ int number= Cyc_Core_int_of_string( Cyc_String_substring( line, i,(
unsigned int)( j - i))); while( j <({ struct _tagged_string _temp27= line;(
unsigned int)( _temp27.last_plus_one - _temp27.curr);})?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( line,
sizeof( char), j) !='"': 0) { ++ j;}{ int k= ++ j; while( k <({ struct
_tagged_string _temp28= line;( unsigned int)( _temp28.last_plus_one - _temp28.curr);})?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( line, sizeof( char), k) !='"': 0) { ++ k;} if( j ==({
struct _tagged_string _temp29= line;( unsigned int)( _temp29.last_plus_one -
_temp29.curr);})? 1: k ==({ struct _tagged_string _temp30= line;( unsigned int)(
_temp30.last_plus_one - _temp30.curr);})){ struct Cyc_Core_Opt* _temp31= 0;
_npop_handler( 0u); return _temp31;}{ struct _tagged_string fname= Cyc_String_substring(
line, j,( unsigned int)( k - j)); struct Cyc_Core_Opt* _temp34=({ struct Cyc_Core_Opt*
_temp32=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp32->v=( void*)({ struct _tuple0* _temp33=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp33->f1= fname; _temp33->f2= number; _temp33;});
_temp32;}); _npop_handler( 0u); return _temp34;}}}}}; _pop_handler();} else{
void* _temp21=( void*) _exn_thrown; void* _temp36= _temp21; _LL38: goto _LL39;
_LL40: goto _LL41; _LL39: return 0; _LL41:( void) _throw( _temp36); _LL37:;}}}
struct _tuple1{ int f1; struct Cyc_Lineno_Pos* f2; } ; int Cyc_Lineno_place_cmp(
struct _tuple1* place1, struct _tuple1* place2){ return Cyc_Core_intcmp((*
place1).f1,(* place2).f1);} void Cyc_Lineno_poss_of_abss( struct _tagged_string
filename, struct Cyc_List_List* places){ places=(( struct Cyc_List_List*(*)( int(*
less_eq)( struct _tuple1*, struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp, places);{ struct Cyc_Stdio___sFILE* f= Cyc_Stdio_file_open(
filename,( struct _tagged_string)({ char* _temp68=( char*)"r"; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 2; _temp69;}));{ struct _handler_cons _temp42; _push_handler(& _temp42);{
int _temp44= 0; if( setjmp( _temp42.handler)){ _temp44= 1;} if( ! _temp44){{
struct Cyc_Lexing_lexbuf* lbuf= Cyc_Lexing_from_file( f); struct _tagged_string
source_file= filename; int line= 1; struct _tagged_string this_line; int eol;
int next; while( places != 0) { while( 1) { next=(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_Lineno_token)( lbuf); eol=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)( lbuf); this_line=(( struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme)( lbuf); if( eol >=(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1){ break;} switch( next){ case 3: _LL45:( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp47=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp47[ 0]=({ struct Cyc_Core_Failure_struct
_temp48; _temp48.tag= Cyc_Core_Failure; _temp48.f1=( struct _tagged_string)({
char* _temp49=( char*)"Lineno: Invalid file position"; struct _tagged_string
_temp50; _temp50.curr= _temp49; _temp50.base= _temp49; _temp50.last_plus_one=
_temp49 + 30; _temp50;}); _temp48;}); _temp47;})); case 1: _LL46: ++ line;
break; case 2: _LL51: { struct Cyc_Core_Opt* fno= Cyc_Lineno_parse_linedef(
this_line); if( fno == 0){ ++ line;({ struct _tagged_string _temp53= this_line;
fprintf( Cyc_Stdio_stderr,"Unknown directive: %.*s\n", _temp53.last_plus_one -
_temp53.curr, _temp53.curr);});} else{ source_file=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( fno))->v)).f1; line=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( fno))->v)).f2;} break;} default: _LL52:( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp55=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp55[ 0]=({ struct Cyc_Core_Failure_struct
_temp56; _temp56.tag= Cyc_Core_Failure; _temp56.f1=( struct _tagged_string)({
char* _temp57=( char*)"Lineno: Impossible"; struct _tagged_string _temp58;
_temp58.curr= _temp57; _temp58.base= _temp57; _temp58.last_plus_one= _temp57 +
19; _temp58;}); _temp56;}); _temp55;}));}} while( places != 0? eol >=(*(( struct
_tuple1*)(( struct Cyc_List_List*) _check_null( places))->hd)).f1: 0) { struct
Cyc_Lineno_Pos* _temp59=(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f2; goto _LL60; _LL60: _temp59->logical_file=
source_file; _temp59->line= this_line; _temp59->line_no= line; _temp59->col=(
int)( Cyc_String_strlen( this_line) -( eol -(*(( struct _tuple1*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f1)); places=(( struct Cyc_List_List*) _check_null(
places))->tl;} ++ line;}}; _pop_handler();} else{ void* _temp43=( void*)
_exn_thrown; void* _temp62= _temp43; _LL64: goto _LL65; _LL66: goto _LL67; _LL65:
Cyc_Stdio_file_close( f);( void) _throw( _temp62); _LL67:( void) _throw( _temp62);
_LL63:;}}} Cyc_Stdio_file_close( f); return;}} struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs(
struct _tagged_string filename, int abs){ struct Cyc_Lineno_Pos* ans=({ struct
Cyc_Lineno_Pos* _temp72=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos));
_temp72->logical_file=( struct _tagged_string)({ char* _temp75=( char*)"";
struct _tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75;
_temp76.last_plus_one= _temp75 + 1; _temp76;}); _temp72->line=( struct
_tagged_string)({ char* _temp73=( char*)""; struct _tagged_string _temp74;
_temp74.curr= _temp73; _temp74.base= _temp73; _temp74.last_plus_one= _temp73 + 1;
_temp74;}); _temp72->line_no= 0; _temp72->col= 0; _temp72;}); Cyc_Lineno_poss_of_abss(
filename,({ struct Cyc_List_List* _temp70=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp70->hd=( void*)({ struct _tuple1* _temp71=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp71->f1= abs; _temp71->f2=
ans; _temp71;}); _temp70->tl= 0; _temp70;})); return ans;}