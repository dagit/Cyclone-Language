 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_string Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_Stdio_file_string_read(
struct Cyc_Stdio___sFILE* fd, struct _tagged_string dest, int dest_offset, int
max_count); extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; struct
_tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct Cyc_Lexing_lex_tables{
struct _tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct
_tagged_ptr0 lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0
lex_check; } ; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function( int(*
read_fun)( struct _tagged_string, int, void*), void*); extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_string( struct _tagged_string); extern struct _tagged_string Cyc_Lexing_lexeme(
struct Cyc_Lexing_lexbuf*); extern unsigned char Cyc_Lexing_lexeme_char( struct
Cyc_Lexing_lexbuf*, int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string, int, struct _tagged_string, int, unsigned int); extern struct
_tagged_string Cyc_String_strdup( struct _tagged_string src); unsigned char Cyc_Lexing_Error[
10u]="\000\000\000\000Error"; struct Cyc_Lexing_lexbuf; struct Cyc_Lexing_function_lexbuf_state;
struct Cyc_Lexing_lex_tables; static unsigned char Cyc_Lexing_aux_buffer_v[ 1u]={'\000'};
static struct _tagged_string Cyc_Lexing_aux_buffer={( unsigned char*) Cyc_Lexing_aux_buffer_v,(
unsigned char*) Cyc_Lexing_aux_buffer_v,( unsigned char*) Cyc_Lexing_aux_buffer_v
+ 1u}; void Cyc_Lexing_lex_refill( struct Cyc_Lexing_lexbuf* lexbuf){ if(({
struct _tagged_string _temp2= Cyc_Lexing_aux_buffer;( unsigned int)( _temp2.last_plus_one
- _temp2.curr);}) == 1){ Cyc_Lexing_aux_buffer= Cyc_Core_new_string( 4096);}{
int read=((( struct Cyc_Lexing_function_lexbuf_state*) lexbuf->refill_state)->read_fun)(
Cyc_Lexing_aux_buffer,( int)({ struct _tagged_string _temp3= Cyc_Lexing_aux_buffer;(
unsigned int)( _temp3.last_plus_one - _temp3.curr);}),( void*)(( struct Cyc_Lexing_function_lexbuf_state*)
lexbuf->refill_state)->read_fun_state); int n= read > 0? read:(( lexbuf->lex_eof_reached=
1, 0)); if( lexbuf->lex_start_pos < n){ int oldlen= lexbuf->lex_buffer_len; int
newlen= oldlen * 2; struct _tagged_string newbuf= Cyc_Core_new_string( newlen +
1); Cyc_String_zstrncpy( newbuf, oldlen,( struct _tagged_string) lexbuf->lex_buffer,
0,( unsigned int) oldlen); lexbuf->lex_buffer= newbuf; lexbuf->lex_buffer_len=
newlen; lexbuf->lex_abs_pos= lexbuf->lex_abs_pos - oldlen; lexbuf->lex_curr_pos=
lexbuf->lex_curr_pos + oldlen; lexbuf->lex_start_pos= lexbuf->lex_start_pos +
oldlen; lexbuf->lex_last_pos= lexbuf->lex_last_pos + oldlen;} Cyc_String_zstrncpy(
lexbuf->lex_buffer, 0,( struct _tagged_string) lexbuf->lex_buffer, n,(
unsigned int)( lexbuf->lex_buffer_len - n)); Cyc_String_zstrncpy( lexbuf->lex_buffer,
lexbuf->lex_buffer_len - n,( struct _tagged_string) Cyc_Lexing_aux_buffer, 0,(
unsigned int) n); lexbuf->lex_abs_pos= lexbuf->lex_abs_pos + n; lexbuf->lex_curr_pos=
lexbuf->lex_curr_pos - n; lexbuf->lex_start_pos= lexbuf->lex_start_pos - n;
lexbuf->lex_last_pos= lexbuf->lex_last_pos - n;}} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_function(
int(* read_fun)( struct _tagged_string, int, void*), void* read_fun_state){
return({ struct Cyc_Lexing_lexbuf* _temp4=( struct Cyc_Lexing_lexbuf*) GC_malloc(
sizeof( struct Cyc_Lexing_lexbuf)); _temp4->refill_buff=( void(*)( struct Cyc_Lexing_lexbuf*
lexbuf)) Cyc_Lexing_lex_refill; _temp4->refill_state=( void*)({ struct Cyc_Lexing_function_lexbuf_state*
_temp5=( struct Cyc_Lexing_function_lexbuf_state*) GC_malloc( sizeof( struct Cyc_Lexing_function_lexbuf_state));
_temp5->read_fun= read_fun; _temp5->read_fun_state=( void*) read_fun_state;
_temp5;}); _temp4->lex_buffer= Cyc_Core_new_string( 8192); _temp4->lex_buffer_len=
8192; _temp4->lex_abs_pos= - 8192; _temp4->lex_start_pos= 8192; _temp4->lex_curr_pos=
8192; _temp4->lex_last_pos= 8192; _temp4->lex_last_action= 0; _temp4->lex_eof_reached=
0; _temp4;});} int Cyc_Lexing_read_from_file( struct _tagged_string aux, int n,
struct Cyc_Stdio___sFILE* f){ return Cyc_Stdio_file_string_read( f, aux, 0, n);}
struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE* f){
return(( struct Cyc_Lexing_lexbuf*(*)( int(* read_fun)( struct _tagged_string,
int, struct Cyc_Stdio___sFILE*), struct Cyc_Stdio___sFILE* read_fun_state)) Cyc_Lexing_from_function)(
Cyc_Lexing_read_from_file, f);} static void Cyc_Lexing_set_eof( struct Cyc_Lexing_lexbuf*
lbuf){ lbuf->lex_eof_reached= 1;} struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_string(
struct _tagged_string s){ return({ struct Cyc_Lexing_lexbuf* _temp6=( struct Cyc_Lexing_lexbuf*)
GC_malloc( sizeof( struct Cyc_Lexing_lexbuf)); _temp6->refill_buff= Cyc_Lexing_set_eof;
_temp6->refill_state=( void*) 0; _temp6->lex_buffer= Cyc_String_strdup(( struct
_tagged_string) s); _temp6->lex_buffer_len=( int)({ struct _tagged_string _temp7=
s;( unsigned int)( _temp7.last_plus_one - _temp7.curr);}); _temp6->lex_abs_pos=
0; _temp6->lex_start_pos= 0; _temp6->lex_curr_pos= 0; _temp6->lex_last_pos= 0;
_temp6->lex_last_action= 0; _temp6->lex_eof_reached= 1; _temp6;});} struct
_tagged_string Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf* lbuf){ int len= lbuf->lex_curr_pos
- lbuf->lex_start_pos; struct _tagged_string s= Cyc_Core_new_string( len + 1);
Cyc_String_zstrncpy( s, 0,( struct _tagged_string) lbuf->lex_buffer, lbuf->lex_start_pos,(
unsigned int) len);*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char), len)='\000';
return s;} unsigned char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf* lbuf,
int i){ return*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( lbuf->lex_buffer, sizeof(
unsigned char), lbuf->lex_start_pos + i);} int Cyc_Lexing_lexeme_start( struct
Cyc_Lexing_lexbuf* lbuf){ return lbuf->lex_abs_pos + lbuf->lex_start_pos;} int
Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf* lbuf){ return lbuf->lex_abs_pos
+ lbuf->lex_curr_pos;} int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*
tbl, int start_state, struct Cyc_Lexing_lexbuf* lbuf){ int state; int base; int
backtrk; int c; state= start_state; if( state >= 0){ lbuf->lex_last_pos=( lbuf->lex_start_pos=
lbuf->lex_curr_pos); lbuf->lex_last_action= - 1;} else{ state=( - state) - 1;}
while( 1) { base=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tbl->lex_base, sizeof( int), state); if( base < 0){
return( - base) - 1;} backtrk=*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( tbl->lex_backtrk, sizeof( int), state);
if( backtrk >= 0){ lbuf->lex_last_pos= lbuf->lex_curr_pos; lbuf->lex_last_action=
backtrk;} if( lbuf->lex_curr_pos >= lbuf->lex_buffer_len){ if( ! lbuf->lex_eof_reached){
return( - state) - 1;} else{ c= 256;}} else{ c=( int)*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
lbuf->lex_buffer, sizeof( unsigned char), lbuf->lex_curr_pos ++); if( c == - 1){
c= 256;}} if(*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tbl->lex_check, sizeof( int), base + c) == state){
state=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tbl->lex_trans, sizeof( int), base + c);} else{ state=*((
int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tbl->lex_default, sizeof( int), state);} if( state <
0){ lbuf->lex_curr_pos= lbuf->lex_last_pos; if( lbuf->lex_last_action == - 1){(
void) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp8=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp8[ 0]=({ struct Cyc_Lexing_Error_struct
_temp9; _temp9.tag= Cyc_Lexing_Error; _temp9.f1=({ unsigned char* _temp10=(
unsigned char*)"empty token"; struct _tagged_string _temp11; _temp11.curr=
_temp10; _temp11.base= _temp10; _temp11.last_plus_one= _temp10 + 12; _temp11;});
_temp9;}); _temp8;}));} else{ return lbuf->lex_last_action;}} else{ if( c == 256){
lbuf->lex_eof_reached= 0;}}}}