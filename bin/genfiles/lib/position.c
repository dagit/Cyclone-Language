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
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern unsigned char
Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u];
struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs(
struct _tagged_arr, int); extern void Cyc_Lineno_poss_of_abss( struct
_tagged_arr filename, struct Cyc_List_List* places); extern unsigned char Cyc_Position_Exit[
9u]; extern void Cyc_Position_reset_position( struct _tagged_arr); extern void
Cyc_Position_set_position_file( struct _tagged_arr); extern struct _tagged_arr
Cyc_Position_get_position_file(); struct Cyc_Position_Segment; extern struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs( int, int); extern struct Cyc_Position_Segment* Cyc_Position_segment_join(
struct Cyc_Position_Segment*, struct Cyc_Position_Segment*); extern struct
_tagged_arr Cyc_Position_string_of_loc( int); extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); extern struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List*); static const int Cyc_Position_Lex= 0; static const int
Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{
struct _tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*, struct _tagged_arr); extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*, struct _tagged_arr);
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_arr); extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_print_context; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
extern int Cyc_Position_error_p(); extern int Cyc_String_strlen( struct
_tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern struct _tagged_arr Cyc_String_strncpy( struct
_tagged_arr, struct _tagged_arr, int); extern struct _tagged_arr Cyc_String_substring(
struct _tagged_arr, int ofs, int n); unsigned char Cyc_Position_Exit[ 9u]="\000\000\000\000Exit";
static unsigned char _temp0[ 1u]=""; static struct _tagged_arr Cyc_Position_source={
_temp0, _temp0, _temp0 + 1u}; struct Cyc_Position_Segment{ int start; int end; }
; struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int start, int end){
return({ struct Cyc_Position_Segment* _temp1=( struct Cyc_Position_Segment*)
GC_malloc_atomic( sizeof( struct Cyc_Position_Segment)); _temp1->start= start;
_temp1->end= end; _temp1;});} struct Cyc_Position_Segment* Cyc_Position_segment_join(
struct Cyc_Position_Segment* s1, struct Cyc_Position_Segment* s2){ if( s1 == 0){
return s2;} if( s2 == 0){ return s1;} return({ struct Cyc_Position_Segment*
_temp2=( struct Cyc_Position_Segment*) GC_malloc_atomic( sizeof( struct Cyc_Position_Segment));
_temp2->start=(( struct Cyc_Position_Segment*) _check_null( s1))->start; _temp2->end=((
struct Cyc_Position_Segment*) _check_null( s2))->end; _temp2;});} struct
_tagged_arr Cyc_Position_string_of_loc( int loc){ struct Cyc_Lineno_Pos* pos=
Cyc_Lineno_pos_of_abs( Cyc_Position_source, loc); return({ struct _tagged_arr
_temp3= pos->logical_file; int _temp4= pos->line_no; int _temp5= pos->col;
xprintf("%.*s (%d:%d)", _get_arr_size( _temp3, 1u), _temp3.curr, _temp4, _temp5);});}
static struct _tagged_arr Cyc_Position_string_of_pos_pr( struct Cyc_Lineno_Pos*
pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_String_strcmp( pos_s->logical_file,
pos_e->logical_file) == 0){ return({ struct _tagged_arr _temp6= pos_s->logical_file;
int _temp7= pos_s->line_no; int _temp8= pos_s->col; int _temp9= pos_e->line_no;
int _temp10= pos_e->col; xprintf("%.*s(%d:%d-%d:%d)", _get_arr_size( _temp6, 1u),
_temp6.curr, _temp7, _temp8, _temp9, _temp10);});} else{ return({ struct
_tagged_arr _temp11= pos_s->logical_file; int _temp12= pos_s->line_no; int
_temp13= pos_s->col; struct _tagged_arr _temp14= pos_e->logical_file; int
_temp15= pos_e->line_no; int _temp16= pos_e->col; xprintf("%.*s(%d:%d)-%.*s(%d:%d)",
_get_arr_size( _temp11, 1u), _temp11.curr, _temp12, _temp13, _get_arr_size(
_temp14, 1u), _temp14.curr, _temp15, _temp16);});}} struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment* s){ if( s == 0){ return({ struct _tagged_arr
_temp17= Cyc_Position_source; xprintf("%.*s", _get_arr_size( _temp17, 1u),
_temp17.curr);});}{ struct Cyc_Lineno_Pos* pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,((
struct Cyc_Position_Segment*) _check_null( s))->start); struct Cyc_Lineno_Pos*
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,(( struct Cyc_Position_Segment*)
_check_null( s))->end); return Cyc_Position_string_of_pos_pr( pos_s, pos_e);}}
static struct Cyc_Lineno_Pos* Cyc_Position_new_pos(){ return({ struct Cyc_Lineno_Pos*
_temp18=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos));
_temp18->logical_file= _tag_arr("", sizeof( unsigned char), 1u); _temp18->line=
Cyc_Core_new_string( 0); _temp18->line_no= 0; _temp18->col= 0; _temp18;});}
struct _tuple0{ int f1; struct Cyc_Lineno_Pos* f2; } ; struct Cyc_List_List* Cyc_Position_strings_of_segments(
struct Cyc_List_List* segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List*
_temp19= segs; for( 0; _temp19 != 0; _temp19=(( struct Cyc_List_List*)
_check_null( _temp19))->tl){ if(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*)
_check_null( _temp19))->hd == 0){ continue;} places=({ struct Cyc_List_List*
_temp20=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp20->hd=( void*)({ struct _tuple0* _temp23=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp23->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp19))->hd))->end; _temp23->f2= Cyc_Position_new_pos(); _temp23;}); _temp20->tl=({
struct Cyc_List_List* _temp21=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp21->hd=( void*)({ struct _tuple0* _temp22=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp22->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp19))->hd))->start; _temp22->f2= Cyc_Position_new_pos(); _temp22;}); _temp21->tl=
places; _temp21;}); _temp20;});}} Cyc_Lineno_poss_of_abss( Cyc_Position_source,
places);{ struct Cyc_List_List* ans= 0; places=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( places); for( 0; segs != 0; segs=((
struct Cyc_List_List*) _check_null( segs))->tl){ if(( struct Cyc_Position_Segment*)((
struct Cyc_List_List*) _check_null( segs))->hd == 0){ ans=({ struct Cyc_List_List*
_temp24=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp24->hd=( void*)({ struct _tagged_arr* _temp25=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp25[ 0]=({ struct _tagged_arr
_temp26= Cyc_Position_source; xprintf("%.*s(unknown)", _get_arr_size( _temp26, 1u),
_temp26.curr);}); _temp25;}); _temp24->tl= ans; _temp24;});} else{ ans=({ struct
Cyc_List_List* _temp27=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp27->hd=( void*)({ struct _tagged_arr* _temp28=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp28[ 0]= Cyc_Position_string_of_pos_pr((*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2,(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( places))->tl))->hd)).f2); _temp28;}); _temp27->tl= ans; _temp27;});
places=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( places))->tl))->tl;}} return ans;}} struct Cyc_Position_Error;
struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*
l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error* _temp29=(
struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp29->source= Cyc_Position_source; _temp29->seg= l; _temp29->kind=( void*)((
void*) Cyc_Position_Lex); _temp29->desc= desc; _temp29;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_arr
desc){ return({ struct Cyc_Position_Error* _temp30=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp30->source= Cyc_Position_source;
_temp30->seg= l; _temp30->kind=( void*)(( void*) Cyc_Position_Parse); _temp30->desc=
desc; _temp30;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp31=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp31->source= Cyc_Position_source; _temp31->seg= l; _temp31->kind=( void*)((
void*) Cyc_Position_Elab); _temp31->desc= desc; _temp31;});} unsigned char Cyc_Position_Nocontext[
14u]="\000\000\000\000Nocontext"; static struct _tagged_arr Cyc_Position_trunc(
int n, struct _tagged_arr s){ int len= Cyc_String_strlen(( struct _tagged_arr) s);
if( len < n){ return s;}{ int len_one=( n - 3) / 2; int len_two=( n - 3) -
len_one; struct _tagged_arr ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy(
ans,( struct _tagged_arr) s, len_one); Cyc_String_strncpy( _tagged_arr_plus( ans,
sizeof( unsigned char), len_one), _tag_arr("...", sizeof( unsigned char), 4u), 3);
Cyc_String_strncpy( _tagged_arr_plus( _tagged_arr_plus( ans, sizeof(
unsigned char), len_one), sizeof( unsigned char), 3),( struct _tagged_arr)
_tagged_arr_plus( _tagged_arr_plus( s, sizeof( unsigned char), len), sizeof(
unsigned char), - len_two), len_two); return ans;}} static int Cyc_Position_line_length=
76; struct _tuple1{ struct _tagged_arr f1; int f2; int f3; } ; static struct
_tuple1* Cyc_Position_get_context( struct Cyc_Position_Segment* seg){ if( seg ==
0){( int) _throw(( void*) Cyc_Position_Nocontext);}{ struct Cyc_Lineno_Pos*
pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons _temp32;
_push_handler(& _temp32);{ int _temp34= 0; if( setjmp( _temp32.handler)){
_temp34= 1;} if( ! _temp34){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,((
struct Cyc_Position_Segment*) _check_null( seg))->start); pos_e= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( seg))->end);;
_pop_handler();} else{ void* _temp33=( void*) _exn_thrown; void* _temp36=
_temp33; _LL38: goto _LL39; _LL40: goto _LL41; _LL39:( int) _throw(( void*) Cyc_Position_Nocontext);
_LL41:( void) _throw( _temp36); _LL37:;}}}{ struct Cyc_Lineno_Pos _temp44; int
_temp45; int _temp47; struct _tagged_arr _temp49; struct Cyc_Lineno_Pos* _temp42=
pos_s; _temp44=* _temp42; _LL50: _temp49= _temp44.line; goto _LL48; _LL48:
_temp47= _temp44.line_no; goto _LL46; _LL46: _temp45= _temp44.col; goto _LL43;
_LL43: { struct Cyc_Lineno_Pos _temp53; int _temp54; int _temp56; struct
_tagged_arr _temp58; struct Cyc_Lineno_Pos* _temp51= pos_e; _temp53=* _temp51;
_LL59: _temp58= _temp53.line; goto _LL57; _LL57: _temp56= _temp53.line_no; goto
_LL55; _LL55: _temp54= _temp53.col; goto _LL52; _LL52: if( _temp47 == _temp56){
int n= Cyc_Position_line_length / 3; struct _tagged_arr sec_one= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp49, 0, _temp45)); struct
_tagged_arr sec_two= Cyc_Position_trunc( n, Cyc_String_substring(( struct
_tagged_arr) _temp49, _temp45, _temp54 - _temp45)); struct _tagged_arr sec_three=
Cyc_Position_trunc( n, Cyc_String_substring(( struct _tagged_arr) _temp49,
_temp45, Cyc_String_strlen(( struct _tagged_arr) _temp49) - _temp54)); return({
struct _tuple1* _temp60=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp60->f1=({ struct _tagged_arr _temp61=( struct _tagged_arr) sec_one; struct
_tagged_arr _temp62=( struct _tagged_arr) sec_two; struct _tagged_arr _temp63=(
struct _tagged_arr) sec_three; xprintf("%.*s%.*s%.*s", _get_arr_size( _temp61, 1u),
_temp61.curr, _get_arr_size( _temp62, 1u), _temp62.curr, _get_arr_size( _temp63,
1u), _temp63.curr);}); _temp60->f2=( int) Cyc_String_strlen(( struct _tagged_arr)
sec_one); _temp60->f3=( int)( Cyc_String_strlen(( struct _tagged_arr) sec_one) +
Cyc_String_strlen(( struct _tagged_arr) sec_two)); _temp60;});} else{ int n=(
Cyc_Position_line_length - 3) / 4; struct _tagged_arr sec_one= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp49, 0, _temp45)); struct
_tagged_arr sec_two= Cyc_Position_trunc( n, Cyc_String_substring(( struct
_tagged_arr) _temp49, _temp45, Cyc_String_strlen(( struct _tagged_arr) _temp49)
- _temp45)); struct _tagged_arr sec_three= Cyc_Position_trunc( n, Cyc_String_substring((
struct _tagged_arr) _temp58, 0, _temp54)); struct _tagged_arr sec_four= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp58, _temp54, Cyc_String_strlen((
struct _tagged_arr) _temp58) - _temp54)); return({ struct _tuple1* _temp64=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp64->f1=({ struct
_tagged_arr _temp65=( struct _tagged_arr) sec_one; struct _tagged_arr _temp66=(
struct _tagged_arr) sec_two; struct _tagged_arr _temp67=( struct _tagged_arr)
sec_three; struct _tagged_arr _temp68=( struct _tagged_arr) sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s",
_get_arr_size( _temp65, 1u), _temp65.curr, _get_arr_size( _temp66, 1u), _temp66.curr,
_get_arr_size( _temp67, 1u), _temp67.curr, _get_arr_size( _temp68, 1u), _temp68.curr);});
_temp64->f2=( int) Cyc_String_strlen(( struct _tagged_arr) sec_one); _temp64->f3=(
int)((( Cyc_String_strlen(( struct _tagged_arr) sec_one) + Cyc_String_strlen((
struct _tagged_arr) sec_two)) + 3) + Cyc_String_strlen(( struct _tagged_arr)
sec_three)); _temp64;});}}}}} static int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){
return Cyc_Position_error_b;} unsigned char Cyc_Position_Error[ 10u]="\000\000\000\000Error";
struct Cyc_Position_Error_struct{ unsigned char* tag; struct Cyc_Position_Error*
f1; } ; int Cyc_Position_print_context= 0; int Cyc_Position_first_error= 1; void
Cyc_Position_post_error( struct Cyc_Position_Error* e){ Cyc_Position_error_b= 1;
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout); if( Cyc_Position_first_error){
fprintf( _sfile_to_file( Cyc_Stdio_stderr),"\n"); Cyc_Position_first_error= 0;}({
struct _tagged_arr _temp69=( struct _tagged_arr) Cyc_Position_string_of_segment(
e->seg); struct _tagged_arr _temp70= e->desc; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n",
_get_arr_size( _temp69, 1u), _temp69.curr, _get_arr_size( _temp70, 1u), _temp70.curr);});
if( Cyc_Position_print_context){ struct _handler_cons _temp71; _push_handler(&
_temp71);{ int _temp73= 0; if( setjmp( _temp71.handler)){ _temp73= 1;} if( !
_temp73){{ struct _tuple1* x= Cyc_Position_get_context( e->seg); struct
_tagged_arr marker_str=({ unsigned int _temp76=( unsigned int)((* x).f3 + 1);
unsigned char* _temp77=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
* _temp76); struct _tagged_arr _temp79= _tag_arr( _temp77, sizeof( unsigned char),(
unsigned int)((* x).f3 + 1));{ unsigned int _temp78= _temp76; unsigned int i;
for( i= 0; i < _temp78; i ++){ _temp77[ i]='\000';}}; _temp79;}); int i= - 1;
while( ++ i <(* x).f2) {*(( unsigned char*) _check_unknown_subscript( marker_str,
sizeof( unsigned char), i))=' ';} while( ++ i <(* x).f3) {*(( unsigned char*)
_check_unknown_subscript( marker_str, sizeof( unsigned char), i))='^';}({ struct
_tagged_arr _temp74=( struct _tagged_arr)(* x).f1; struct _tagged_arr _temp75=(
struct _tagged_arr) marker_str; fprintf( _sfile_to_file( Cyc_Stdio_stderr),"  %.*s\n  %.*s\n",
_get_arr_size( _temp74, 1u), _temp74.curr, _get_arr_size( _temp75, 1u), _temp75.curr);});};
_pop_handler();} else{ void* _temp72=( void*) _exn_thrown; void* _temp81=
_temp72; _LL83: if( _temp81 == Cyc_Position_Nocontext){ goto _LL84;} else{ goto
_LL85;} _LL85: goto _LL86; _LL84: goto _LL82; _LL86:( void) _throw( _temp81);
_LL82:;}}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}
void Cyc_Position_reset_position( struct _tagged_arr s){ Cyc_Position_source= s;
Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_arr s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_arr Cyc_Position_get_position_file(){ return Cyc_Position_source;}