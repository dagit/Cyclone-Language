 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_Stdio_file_string_read(
struct Cyc_Stdio___sFILE* fd, struct _tagged_arr dest, int dest_offset, int
max_count); extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function( int(* read_fun)(
struct _tagged_arr, int, void*), void*); extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Stdio___sFILE*); extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string(
struct _tagged_arr); extern struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*);
extern unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*, int);
extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern struct _tagged_arr Cyc_String_zstrncpy(
struct _tagged_arr, struct _tagged_arr, int); extern struct _tagged_arr Cyc_String_strdup(
struct _tagged_arr src); unsigned char Cyc_Lexing_Error[ 10u]="\000\000\000\000Error";
struct Cyc_Lexing_lexbuf; struct Cyc_Lexing_function_lexbuf_state; struct Cyc_Lexing_lex_tables;
static unsigned char Cyc_Lexing_aux_buffer_v[ 1u]={'\000'}; static struct
_tagged_arr Cyc_Lexing_aux_buffer={( void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v),(
void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v),( void*)(( unsigned char*) Cyc_Lexing_aux_buffer_v)
+ 1u}; void Cyc_Lexing_lex_refill( struct Cyc_Lexing_lexbuf* lexbuf){ if(
_get_arr_size( Cyc_Lexing_aux_buffer, sizeof( unsigned char)) == 1){ Cyc_Lexing_aux_buffer=
Cyc_Core_new_string( 4096);}{ int read=((( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun)( Cyc_Lexing_aux_buffer,( int) _get_arr_size(
Cyc_Lexing_aux_buffer, sizeof( unsigned char)),( void*)(( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun_state); int n= read > 0? read:(( lexbuf->lex_eof_reached=
1, 0)); if( lexbuf->lex_start_pos < n){ int oldlen= lexbuf->lex_buffer_len; int
newlen= oldlen * 2; struct _tagged_arr newbuf= Cyc_Core_new_string( newlen + 1);
Cyc_String_zstrncpy( _tagged_arr_plus( newbuf, sizeof( unsigned char), oldlen),(
struct _tagged_arr) lexbuf->lex_buffer, oldlen); lexbuf->lex_buffer= newbuf;
lexbuf->lex_buffer_len= newlen; lexbuf->lex_abs_pos= lexbuf->lex_abs_pos -
oldlen; lexbuf->lex_curr_pos= lexbuf->lex_curr_pos + oldlen; lexbuf->lex_start_pos=
lexbuf->lex_start_pos + oldlen; lexbuf->lex_last_pos= lexbuf->lex_last_pos +
oldlen;} Cyc_String_zstrncpy( lexbuf->lex_buffer,( struct _tagged_arr)
_tagged_arr_plus( lexbuf->lex_buffer, sizeof( unsigned char), n), lexbuf->lex_buffer_len
- n); Cyc_String_zstrncpy( _tagged_arr_plus( _tagged_arr_plus( lexbuf->lex_buffer,
sizeof( unsigned char), lexbuf->lex_buffer_len), sizeof( unsigned char), - n),(
struct _tagged_arr) Cyc_Lexing_aux_buffer, n); lexbuf->lex_abs_pos= lexbuf->lex_abs_pos
+ n; lexbuf->lex_curr_pos= lexbuf->lex_curr_pos - n; lexbuf->lex_start_pos=
lexbuf->lex_start_pos - n; lexbuf->lex_last_pos= lexbuf->lex_last_pos - n;}}
struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function( int(* read_fun)( struct
_tagged_arr, int, void*), void* read_fun_state){ return({ struct Cyc_Lexing_lexbuf*
_temp0=( struct Cyc_Lexing_lexbuf*) GC_malloc( sizeof( struct Cyc_Lexing_lexbuf));
_temp0->refill_buff=( void(*)( struct Cyc_Lexing_lexbuf* lexbuf)) Cyc_Lexing_lex_refill;
_temp0->refill_state=( void*)({ struct Cyc_Lexing_function_lexbuf_state* _temp1=(
struct Cyc_Lexing_function_lexbuf_state*) GC_malloc( sizeof( struct Cyc_Lexing_function_lexbuf_state));
_temp1->read_fun= read_fun; _temp1->read_fun_state=( void*) read_fun_state;
_temp1;}); _temp0->lex_buffer= Cyc_Core_new_string( 8192); _temp0->lex_buffer_len=
8192; _temp0->lex_abs_pos= - 8192; _temp0->lex_start_pos= 8192; _temp0->lex_curr_pos=
8192; _temp0->lex_last_pos= 8192; _temp0->lex_last_action= 0; _temp0->lex_eof_reached=
0; _temp0;});} int Cyc_Lexing_read_from_file( struct _tagged_arr aux, int n,
struct Cyc_Stdio___sFILE* f){ return Cyc_Stdio_file_string_read( f, aux, 0, n);}
struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE* f){
return(( struct Cyc_Lexing_lexbuf*(*)( int(* read_fun)( struct _tagged_arr, int,
struct Cyc_Stdio___sFILE*), struct Cyc_Stdio___sFILE* read_fun_state)) Cyc_Lexing_from_function)(
Cyc_Lexing_read_from_file, f);} static void Cyc_Lexing_set_eof( struct Cyc_Lexing_lexbuf*
lbuf){ lbuf->lex_eof_reached= 1;} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string(
struct _tagged_arr s){ return({ struct Cyc_Lexing_lexbuf* _temp2=( struct Cyc_Lexing_lexbuf*)
GC_malloc( sizeof( struct Cyc_Lexing_lexbuf)); _temp2->refill_buff= Cyc_Lexing_set_eof;
_temp2->refill_state=( void*) 0; _temp2->lex_buffer= Cyc_String_strdup(( struct
_tagged_arr) s); _temp2->lex_buffer_len=( int) _get_arr_size( s, sizeof(
unsigned char)); _temp2->lex_abs_pos= 0; _temp2->lex_start_pos= 0; _temp2->lex_curr_pos=
0; _temp2->lex_last_pos= 0; _temp2->lex_last_action= 0; _temp2->lex_eof_reached=
1; _temp2;});} struct _tagged_arr Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*
lbuf){ int len= lbuf->lex_curr_pos - lbuf->lex_start_pos; struct _tagged_arr s=
Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy( s,( struct _tagged_arr)
_tagged_arr_plus( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_start_pos),
len);*(( unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
len))='\000'; return s;} unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*
lbuf, int i){ return*(( unsigned char*) _check_unknown_subscript( lbuf->lex_buffer,
sizeof( unsigned char), lbuf->lex_start_pos + i));} int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf* lbuf){ return lbuf->lex_abs_pos + lbuf->lex_start_pos;}
int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf* lbuf){ return lbuf->lex_abs_pos
+ lbuf->lex_curr_pos;} int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*
tbl, int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state; int base; int
backtrk; int c; state= start_state; if( state >= 0){ lbuf->lex_last_pos=( lbuf->lex_start_pos=
lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{ state=( - state) - 1;}
while( 1) { base=*(( int*) _check_unknown_subscript( tbl->lex_base, sizeof( int),
state)); if( base < 0){ return( - base) - 1;} backtrk=*(( int*)
_check_unknown_subscript( tbl->lex_backtrk, sizeof( int), state)); if( backtrk
>= 0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action= backtrk;}
if( lbuf->lex_curr_pos >= lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) - 1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*)
_check_unknown_subscript( lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos
++)); if( c == - 1){ c= 256;}} if(*(( int*) _check_unknown_subscript( tbl->lex_check,
sizeof( int), base + c)) == state){ state=*(( int*) _check_unknown_subscript(
tbl->lex_trans, sizeof( int), base + c));} else{ state=*(( int*)
_check_unknown_subscript( tbl->lex_default, sizeof( int), state));} if( state <
0){ lbuf->lex_curr_pos= lbuf->lex_last_pos; if( lbuf->lex_last_action == - 1){(
int) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp3=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp3[ 0]=({ struct Cyc_Lexing_Error_struct
_temp4; _temp4.tag= Cyc_Lexing_Error; _temp4.f1= _tag_arr("empty token", sizeof(
unsigned char), 12u); _temp4;}); _temp3;}));} else{ return lbuf->lex_last_action;}}
else{ if( c == 256){ lbuf->lex_eof_reached= 0;}}}}