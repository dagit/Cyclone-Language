 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_arr Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Stdio___sFILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_of_abs( struct _tagged_arr, int); extern void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename, struct Cyc_List_List* places); extern unsigned char
Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position( struct
_tagged_arr); extern void Cyc_Position_set_position_file( struct _tagged_arr);
extern struct _tagged_arr Cyc_Position_get_position_file(); struct Cyc_Position_Segment;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_arr Cyc_Position_string_of_loc(
int); extern struct _tagged_arr Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*, struct _tagged_arr); extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*, struct _tagged_arr);
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_print_context; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
extern int Cyc_Position_error_p(); extern unsigned int Cyc_String_strlen( struct
_tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern struct _tagged_arr Cyc_String_strncpy( struct
_tagged_arr, int, struct _tagged_arr, int, unsigned int); extern struct
_tagged_arr Cyc_String_substring( struct _tagged_arr, int ofs, unsigned int n);
unsigned char Cyc_Position_Exit[ 9u]="\000\000\000\000Exit"; static
unsigned char _temp0[ 1u]=""; static struct _tagged_arr Cyc_Position_source={
_temp0, _temp0, _temp0 + 1u}; struct Cyc_Position_Segment{ int start; int end; }
; struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int start, int end){
return({ struct Cyc_Position_Segment* _temp1=( struct Cyc_Position_Segment*)
GC_malloc( sizeof( struct Cyc_Position_Segment) * 1); _temp1[ 0]=({ struct Cyc_Position_Segment
_temp2; _temp2.start= start; _temp2.end= end; _temp2;}); _temp1;});} struct Cyc_Position_Segment*
Cyc_Position_segment_join( struct Cyc_Position_Segment* s1, struct Cyc_Position_Segment*
s2){ if( s1 == 0){ return s2;} if( s2 == 0){ return s1;} return({ struct Cyc_Position_Segment*
_temp3=( struct Cyc_Position_Segment*) GC_malloc( sizeof( struct Cyc_Position_Segment)
* 1); _temp3[ 0]=({ struct Cyc_Position_Segment _temp4; _temp4.start=(( struct
Cyc_Position_Segment*) _check_null( s1))->start; _temp4.end=(( struct Cyc_Position_Segment*)
_check_null( s2))->end; _temp4;}); _temp3;});} struct _tagged_arr Cyc_Position_string_of_loc(
int loc){ struct Cyc_Lineno_Pos* pos= Cyc_Lineno_pos_of_abs( Cyc_Position_source,
loc); return({ struct _tagged_arr _temp5= pos->logical_file; int _temp6= pos->line_no;
int _temp7= pos->col; xprintf("%.*s (%d:%d)", _temp5.last_plus_one - _temp5.curr,
_temp5.curr, _temp6, _temp7);});} static struct _tagged_arr Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos* pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_String_strcmp(
pos_s->logical_file, pos_e->logical_file) == 0){ return({ struct _tagged_arr
_temp8= pos_s->logical_file; int _temp9= pos_s->line_no; int _temp10= pos_s->col;
int _temp11= pos_e->line_no; int _temp12= pos_e->col; xprintf("%.*s(%d:%d-%d:%d)",
_temp8.last_plus_one - _temp8.curr, _temp8.curr, _temp9, _temp10, _temp11,
_temp12);});} else{ return({ struct _tagged_arr _temp13= pos_s->logical_file;
int _temp14= pos_s->line_no; int _temp15= pos_s->col; struct _tagged_arr _temp16=
pos_e->logical_file; int _temp17= pos_e->line_no; int _temp18= pos_e->col;
xprintf("%.*s(%d:%d)-%.*s(%d:%d)", _temp13.last_plus_one - _temp13.curr, _temp13.curr,
_temp14, _temp15, _temp16.last_plus_one - _temp16.curr, _temp16.curr, _temp17,
_temp18);});}} struct _tagged_arr Cyc_Position_string_of_segment( struct Cyc_Position_Segment*
s){ if( s == 0){ return({ struct _tagged_arr _temp19= Cyc_Position_source;
xprintf("%.*s", _temp19.last_plus_one - _temp19.curr, _temp19.curr);});}{ struct
Cyc_Lineno_Pos* pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,(( struct Cyc_Position_Segment*)
_check_null( s))->start); struct Cyc_Lineno_Pos* pos_e= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( s))->end);
return Cyc_Position_string_of_pos_pr( pos_s, pos_e);}} static struct Cyc_Lineno_Pos*
Cyc_Position_new_pos(){ return({ struct Cyc_Lineno_Pos* _temp20=( struct Cyc_Lineno_Pos*)
GC_malloc( sizeof( struct Cyc_Lineno_Pos) * 1); _temp20[ 0]=({ struct Cyc_Lineno_Pos
_temp21; _temp21.logical_file= _tag_arr("", sizeof( unsigned char), 1u); _temp21.line=
Cyc_Core_new_string( 0); _temp21.line_no= 0; _temp21.col= 0; _temp21;}); _temp20;});}
struct _tuple0{ int f1; struct Cyc_Lineno_Pos* f2; } ; struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List* segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List*
_temp22= segs; goto _LL23; _LL23: for( 0; _temp22 != 0; _temp22=(( struct Cyc_List_List*)
_check_null( _temp22))->tl){ if(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*)
_check_null( _temp22))->hd == 0){ continue;} places=({ struct Cyc_List_List*
_temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp24->hd=( void*)({ struct _tuple0* _temp27=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp27->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp22))->hd))->end; _temp27->f2= Cyc_Position_new_pos(); _temp27;}); _temp24->tl=({
struct Cyc_List_List* _temp25=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp25->hd=( void*)({ struct _tuple0* _temp26=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp26->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp22))->hd))->start; _temp26->f2= Cyc_Position_new_pos(); _temp26;}); _temp25->tl=
places; _temp25;}); _temp24;});}} Cyc_Lineno_poss_of_abss( Cyc_Position_source,
places);{ struct Cyc_List_List* ans= 0; places=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( places); for( 0; segs != 0; segs=((
struct Cyc_List_List*) _check_null( segs))->tl){ if(( struct Cyc_Position_Segment*)((
struct Cyc_List_List*) _check_null( segs))->hd == 0){ ans=({ struct Cyc_List_List*
_temp28=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp28->hd=( void*)({ struct _tagged_arr* _temp29=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp29[ 0]=({ struct _tagged_arr
_temp30= Cyc_Position_source; xprintf("%.*s(unknown)", _temp30.last_plus_one -
_temp30.curr, _temp30.curr);}); _temp29;}); _temp28->tl= ans; _temp28;});} else{
ans=({ struct Cyc_List_List* _temp31=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp31->hd=( void*)({ struct _tagged_arr* _temp32=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1); _temp32[ 0]=
Cyc_Position_string_of_pos_pr((*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f2,(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( places))->tl))->hd)).f2);
_temp32;}); _temp31->tl= ans; _temp31;}); places=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( places))->tl))->tl;}} return
ans;}} struct Cyc_Position_Error; struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp33=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp33->source= Cyc_Position_source; _temp33->seg= l; _temp33->kind=( void*)((
void*) Cyc_Position_Lex); _temp33->desc= desc; _temp33;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_arr
desc){ return({ struct Cyc_Position_Error* _temp34=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp34->source= Cyc_Position_source;
_temp34->seg= l; _temp34->kind=( void*)(( void*) Cyc_Position_Parse); _temp34->desc=
desc; _temp34;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp35=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp35->source= Cyc_Position_source; _temp35->seg= l; _temp35->kind=( void*)((
void*) Cyc_Position_Elab); _temp35->desc= desc; _temp35;});} unsigned char Cyc_Position_Nocontext[
14u]="\000\000\000\000Nocontext"; static struct _tagged_arr Cyc_Position_trunc(
int n, struct _tagged_arr s){ int len=( int) Cyc_String_strlen(( struct
_tagged_arr) s); if( len < n){ return s;}{ int len_one=( n - 3) / 2; int len_two=(
n - 3) - len_one; struct _tagged_arr ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy(
ans, 0,( struct _tagged_arr) s, 0,( unsigned int) len_one); Cyc_String_strncpy(
ans, len_one, _tag_arr("...", sizeof( unsigned char), 4u), 0, 3); Cyc_String_strncpy(
ans, len_one + 3,( struct _tagged_arr) s, len - len_two,( unsigned int) len_two);
return ans;}} static int Cyc_Position_line_length= 76; struct _tuple1{ struct
_tagged_arr f1; int f2; int f3; } ; static struct _tuple1* Cyc_Position_get_context(
struct Cyc_Position_Segment* seg){ if( seg == 0){( int) _throw(( void*) Cyc_Position_Nocontext);}{
struct Cyc_Lineno_Pos* pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct
_handler_cons _temp36; _push_handler(& _temp36);{ int _temp38= 0; if( setjmp(
_temp36.handler)){ _temp38= 1;} if( ! _temp38){ pos_s= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( seg))->start);
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,(( struct Cyc_Position_Segment*)
_check_null( seg))->end);; _pop_handler();} else{ void* _temp37=( void*)
_exn_thrown; void* _temp40= _temp37; _LL42: goto _LL43; _LL44: goto _LL45; _LL43:(
int) _throw(( void*) Cyc_Position_Nocontext); _LL45:( void) _throw( _temp40);
_LL41:;}}}{ struct Cyc_Lineno_Pos _temp48; int _temp49; int _temp51; struct
_tagged_arr _temp53; struct _tagged_arr _temp55; struct Cyc_Lineno_Pos* _temp46=
pos_s; _temp48=* _temp46; _LL56: _temp55= _temp48.logical_file; goto _LL54;
_LL54: _temp53= _temp48.line; goto _LL52; _LL52: _temp51= _temp48.line_no; goto
_LL50; _LL50: _temp49= _temp48.col; goto _LL47; _LL47: { struct Cyc_Lineno_Pos
_temp59; int _temp60; int _temp62; struct _tagged_arr _temp64; struct
_tagged_arr _temp66; struct Cyc_Lineno_Pos* _temp57= pos_e; _temp59=* _temp57;
_LL67: _temp66= _temp59.logical_file; goto _LL65; _LL65: _temp64= _temp59.line;
goto _LL63; _LL63: _temp62= _temp59.line_no; goto _LL61; _LL61: _temp60= _temp59.col;
goto _LL58; _LL58: if( _temp51 == _temp62){ int n= Cyc_Position_line_length / 3;
struct _tagged_arr sec_one= Cyc_Position_trunc( n, Cyc_String_substring(( struct
_tagged_arr) _temp53, 0,( unsigned int) _temp49)); struct _tagged_arr sec_two=
Cyc_Position_trunc( n, Cyc_String_substring(( struct _tagged_arr) _temp53,
_temp49,( unsigned int)( _temp60 - _temp49))); struct _tagged_arr sec_three= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp53, _temp49, Cyc_String_strlen((
struct _tagged_arr) _temp53) - _temp60)); return({ struct _tuple1* _temp68=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp68->f1=({ struct
_tagged_arr _temp69=( struct _tagged_arr) sec_one; struct _tagged_arr _temp70=(
struct _tagged_arr) sec_two; struct _tagged_arr _temp71=( struct _tagged_arr)
sec_three; xprintf("%.*s%.*s%.*s", _temp69.last_plus_one - _temp69.curr, _temp69.curr,
_temp70.last_plus_one - _temp70.curr, _temp70.curr, _temp71.last_plus_one -
_temp71.curr, _temp71.curr);}); _temp68->f2=( int) Cyc_String_strlen(( struct
_tagged_arr) sec_one); _temp68->f3=( int)( Cyc_String_strlen(( struct
_tagged_arr) sec_one) + Cyc_String_strlen(( struct _tagged_arr) sec_two));
_temp68;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct _tagged_arr
sec_one= Cyc_Position_trunc( n, Cyc_String_substring(( struct _tagged_arr)
_temp53, 0,( unsigned int) _temp49)); struct _tagged_arr sec_two= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp53, _temp49, Cyc_String_strlen((
struct _tagged_arr) _temp53) - _temp49)); struct _tagged_arr sec_three= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp64, 0,( unsigned int) _temp60));
struct _tagged_arr sec_four= Cyc_Position_trunc( n, Cyc_String_substring((
struct _tagged_arr) _temp64, _temp60, Cyc_String_strlen(( struct _tagged_arr)
_temp64) - _temp60)); return({ struct _tuple1* _temp72=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp72->f1=({ struct _tagged_arr _temp73=(
struct _tagged_arr) sec_one; struct _tagged_arr _temp74=( struct _tagged_arr)
sec_two; struct _tagged_arr _temp75=( struct _tagged_arr) sec_three; struct
_tagged_arr _temp76=( struct _tagged_arr) sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s",
_temp73.last_plus_one - _temp73.curr, _temp73.curr, _temp74.last_plus_one -
_temp74.curr, _temp74.curr, _temp75.last_plus_one - _temp75.curr, _temp75.curr,
_temp76.last_plus_one - _temp76.curr, _temp76.curr);}); _temp72->f2=( int) Cyc_String_strlen((
struct _tagged_arr) sec_one); _temp72->f3=( int)((( Cyc_String_strlen(( struct
_tagged_arr) sec_one) + Cyc_String_strlen(( struct _tagged_arr) sec_two)) + 3) +
Cyc_String_strlen(( struct _tagged_arr) sec_three)); _temp72;});}}}}} static int
Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
unsigned char Cyc_Position_Error[ 10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{
unsigned char* tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context=
0; int Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout);
if( Cyc_Position_first_error){ fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n");
Cyc_Position_first_error= 0;}({ struct _tagged_arr _temp77=( struct _tagged_arr)
Cyc_Position_string_of_segment( e->seg); struct _tagged_arr _temp78= e->desc;
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n", _temp77.last_plus_one
- _temp77.curr, _temp77.curr, _temp78.last_plus_one - _temp78.curr, _temp78.curr);});
if( Cyc_Position_print_context){ struct _handler_cons _temp79; _push_handler(&
_temp79);{ int _temp81= 0; if( setjmp( _temp79.handler)){ _temp81= 1;} if( !
_temp81){{ struct _tuple1* x= Cyc_Position_get_context( e->seg); struct
_tagged_arr marker_str=({ unsigned int _temp84=( unsigned int)((* x).f3 + 1);
unsigned char* _temp85=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
* _temp84); struct _tagged_arr _temp87= _tag_arr( _temp85, sizeof( unsigned char),(
unsigned int)((* x).f3 + 1));{ unsigned int _temp86= _temp84; unsigned int i;
for( i= 0; i < _temp86; i ++){ _temp85[ i]='\000';}}; _temp87;}); int i= - 1;
while( ++ i <(* x).f2) {*(( unsigned char*) _check_unknown_subscript( marker_str,
sizeof( unsigned char), i))=' ';} while( ++ i <(* x).f3) {*(( unsigned char*)
_check_unknown_subscript( marker_str, sizeof( unsigned char), i))='^';}({ struct
_tagged_arr _temp82=( struct _tagged_arr)(* x).f1; struct _tagged_arr _temp83=(
struct _tagged_arr) marker_str; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"  %.*s\n  %.*s\n",
_temp82.last_plus_one - _temp82.curr, _temp82.curr, _temp83.last_plus_one -
_temp83.curr, _temp83.curr);});}; _pop_handler();} else{ void* _temp80=( void*)
_exn_thrown; void* _temp89= _temp80; _LL91: if( _temp89 == Cyc_Position_Nocontext){
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL92: goto _LL90; _LL94:(
void) _throw( _temp89); _LL90:;}}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} void Cyc_Position_reset_position( struct _tagged_arr s){ Cyc_Position_source=
s; Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_arr s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_arr Cyc_Position_get_position_file(){ return Cyc_Position_source;}