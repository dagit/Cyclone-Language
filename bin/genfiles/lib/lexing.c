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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_Stdio_file_string_read( struct
Cyc_Stdio___sFILE* fd, struct _tagged_string dest, int dest_offset, int
max_count); extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function(
int(* read_fun)( struct _tagged_string, int, void*), void*); extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_string( struct _tagged_string); extern struct _tagged_string Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*); extern char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*,
int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int
Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[ 18u];
extern char Cyc_List_Nth[ 8u]; extern struct _tagged_string Cyc_String_zstrncpy(
struct _tagged_string, int, struct _tagged_string, int, unsigned int); extern
struct _tagged_string Cyc_String_strdup( struct _tagged_string src); char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_lexbuf; struct Cyc_Lexing_function_lexbuf_state; struct
Cyc_Lexing_lex_tables; static char _temp2[ 1u]=""; static struct _tagged_string
Cyc_Lexing_aux_buffer=( struct _tagged_string){ _temp2, _temp2, _temp2 + 1u};
void Cyc_Lexing_lex_refill( struct Cyc_Lexing_lexbuf* lexbuf){ if(({ struct
_tagged_string _temp3= Cyc_Lexing_aux_buffer;( unsigned int)( _temp3.last_plus_one
- _temp3.curr);}) == 1){ Cyc_Lexing_aux_buffer= Cyc_Core_new_string( 4096);}{
int read=((( struct Cyc_Lexing_function_lexbuf_state*) lexbuf->refill_state)->read_fun)(
Cyc_Lexing_aux_buffer,( int)({ struct _tagged_string _temp4= Cyc_Lexing_aux_buffer;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}),( void*)(( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun_state); int n= read > 0? read:( lexbuf->lex_eof_reached=
1, 0); if( lexbuf->lex_start_pos < n){ int oldlen= lexbuf->lex_buffer_len; int
newlen= oldlen * 2; struct _tagged_string newbuf= Cyc_Core_new_string( newlen +
1); Cyc_String_zstrncpy( newbuf, oldlen, lexbuf->lex_buffer, 0,( unsigned int)
oldlen); lexbuf->lex_buffer= newbuf; lexbuf->lex_buffer_len= newlen; lexbuf->lex_abs_pos=
lexbuf->lex_abs_pos - oldlen; lexbuf->lex_curr_pos= lexbuf->lex_curr_pos +
oldlen; lexbuf->lex_start_pos= lexbuf->lex_start_pos + oldlen; lexbuf->lex_last_pos=
lexbuf->lex_last_pos + oldlen;} Cyc_String_zstrncpy( lexbuf->lex_buffer, 0,
lexbuf->lex_buffer, n,( unsigned int)( lexbuf->lex_buffer_len - n)); Cyc_String_zstrncpy(
lexbuf->lex_buffer, lexbuf->lex_buffer_len - n, Cyc_Lexing_aux_buffer, 0,(
unsigned int) n); lexbuf->lex_abs_pos= lexbuf->lex_abs_pos + n; lexbuf->lex_curr_pos=
lexbuf->lex_curr_pos - n; lexbuf->lex_start_pos= lexbuf->lex_start_pos - n;
lexbuf->lex_last_pos= lexbuf->lex_last_pos - n;}} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function(
int(* read_fun)( struct _tagged_string, int, void*), void* read_fun_state){
return({ struct Cyc_Lexing_lexbuf* _temp5=( struct Cyc_Lexing_lexbuf*) GC_malloc(
sizeof( struct Cyc_Lexing_lexbuf)); _temp5->refill_buff=( void(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_Lexing_lex_refill; _temp5->refill_state=( void*)({ struct Cyc_Lexing_function_lexbuf_state*
_temp6=( struct Cyc_Lexing_function_lexbuf_state*) GC_malloc( sizeof( struct Cyc_Lexing_function_lexbuf_state));
_temp6->read_fun= read_fun; _temp6->read_fun_state=( void*) read_fun_state;
_temp6;}); _temp5->lex_buffer= Cyc_Core_new_string( 8192); _temp5->lex_buffer_len=
8192; _temp5->lex_abs_pos= - 8192; _temp5->lex_start_pos= 8192; _temp5->lex_curr_pos=
8192; _temp5->lex_last_pos= 8192; _temp5->lex_last_action= 0; _temp5->lex_eof_reached=
0; _temp5;});} int Cyc_Lexing_read_from_file( struct _tagged_string aux, int n,
struct Cyc_Stdio___sFILE* f){ return Cyc_Stdio_file_string_read( f, aux, 0, n);}
struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE* f){
return(( struct Cyc_Lexing_lexbuf*(*)( int(* read_fun)( struct _tagged_string,
int, struct Cyc_Stdio___sFILE*), struct Cyc_Stdio___sFILE* read_fun_state)) Cyc_Lexing_from_function)(
Cyc_Lexing_read_from_file, f);} static void Cyc_Lexing_set_eof( struct Cyc_Lexing_lexbuf*
lbuf){ lbuf->lex_eof_reached= 1;} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string(
struct _tagged_string s){ return({ struct Cyc_Lexing_lexbuf* _temp7=( struct Cyc_Lexing_lexbuf*)
GC_malloc( sizeof( struct Cyc_Lexing_lexbuf)); _temp7->refill_buff= Cyc_Lexing_set_eof;
_temp7->refill_state=( void*) 0; _temp7->lex_buffer= Cyc_String_strdup( s);
_temp7->lex_buffer_len=( int)({ struct _tagged_string _temp8= s;( unsigned int)(
_temp8.last_plus_one - _temp8.curr);}); _temp7->lex_abs_pos= 0; _temp7->lex_start_pos=
0; _temp7->lex_curr_pos= 0; _temp7->lex_last_pos= 0; _temp7->lex_last_action= 0;
_temp7->lex_eof_reached= 1; _temp7;});} struct _tagged_string Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf* lbuf){ int len= lbuf->lex_curr_pos - lbuf->lex_start_pos;
struct _tagged_string s= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy( s,
0, lbuf->lex_buffer, lbuf->lex_start_pos,( unsigned int) len);*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), len)='\000'; return s;} char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*
lbuf, int i){ return*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( lbuf->lex_buffer, sizeof( char), lbuf->lex_start_pos
+ i);} int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf* lbuf){ return lbuf->lex_abs_pos
+ lbuf->lex_start_pos;} int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*
lbuf){ return lbuf->lex_abs_pos + lbuf->lex_curr_pos;} int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables* tbl, int start_state, struct Cyc_Lexing_lexbuf*
lbuf){ int state; int base; int backtrk; int c; state= start_state; if( state >=
0){ lbuf->lex_last_pos=( lbuf->lex_start_pos= lbuf->lex_curr_pos); lbuf->lex_last_action=
- 1;} else{ state=( - state) - 1;} while( 1) { base=*(( int*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( tbl->lex_base,
sizeof( int), state); if( base < 0){ return( - base) - 1;} backtrk=*(( int*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( tbl->lex_backtrk,
sizeof( int), state); if( backtrk >= 0){ lbuf->lex_last_pos= lbuf->lex_curr_pos;
lbuf->lex_last_action= backtrk;} if( lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if( ! lbuf->lex_eof_reached){ return( - state) - 1;} else{ c= 256;}} else{ c=(
int)*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( lbuf->lex_buffer, sizeof( char),( lbuf->lex_curr_pos
++)); if( c ==( char)( - 1)){ c= 256;}} if(*(( int*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tbl->lex_check, sizeof(
int), base + c) == state){ state=*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( tbl->lex_trans, sizeof( int), base + c);}
else{ state=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tbl->lex_default, sizeof( int), state);} if( state <
0){ lbuf->lex_curr_pos= lbuf->lex_last_pos; if( lbuf->lex_last_action == - 1){(
void) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp9=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp9[ 0]=({ struct Cyc_Lexing_Error_struct
_temp10; _temp10.tag= Cyc_Lexing_Error; _temp10.f1=( struct _tagged_string)({
char* _temp11=( char*)"empty token"; struct _tagged_string _temp12; _temp12.curr=
_temp11; _temp12.base= _temp11; _temp12.last_plus_one= _temp11 + 12; _temp12;});
_temp10;}); _temp9;}));} else{ return lbuf->lex_last_action;}} else{ if( c ==
256){ lbuf->lex_eof_reached= 0;}}}}