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
} ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
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
Cyc_Lineno_pos_of_abs( Cyc_Position_source, loc); return({ struct _tagged_arr(*
_temp3)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp9= _tag_arr("%s (%d:%d)", sizeof( unsigned char), 11u); struct
Cyc_Stdio_String_pa_struct _temp12; _temp12.tag= Cyc_Stdio_String_pa; _temp12.f1=(
struct _tagged_arr) pos->logical_file;{ void* _temp6=( void*)& _temp12; struct
Cyc_Stdio_Int_pa_struct _temp11; _temp11.tag= Cyc_Stdio_Int_pa; _temp11.f1=( int)((
unsigned int) pos->line_no);{ void* _temp7=( void*)& _temp11; struct Cyc_Stdio_Int_pa_struct
_temp10; _temp10.tag= Cyc_Stdio_Int_pa; _temp10.f1=( int)(( unsigned int) pos->col);{
void* _temp8=( void*)& _temp10; void* _temp4[ 3u]={ _temp6, _temp7, _temp8};
struct _tagged_arr _temp5={( void*) _temp4,( void*) _temp4,( void*)( _temp4 + 3u)};
_temp3( _temp9, _temp5);}}}});} static struct _tagged_arr Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos* pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_String_strcmp(
pos_s->logical_file, pos_e->logical_file) == 0){ return({ struct _tagged_arr(*
_temp13)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp21= _tag_arr("%s(%d:%d-%d:%d)", sizeof( unsigned char), 16u);
struct Cyc_Stdio_String_pa_struct _temp26; _temp26.tag= Cyc_Stdio_String_pa;
_temp26.f1=( struct _tagged_arr) pos_s->logical_file;{ void* _temp16=( void*)&
_temp26; struct Cyc_Stdio_Int_pa_struct _temp25; _temp25.tag= Cyc_Stdio_Int_pa;
_temp25.f1=( int)(( unsigned int) pos_s->line_no);{ void* _temp17=( void*)&
_temp25; struct Cyc_Stdio_Int_pa_struct _temp24; _temp24.tag= Cyc_Stdio_Int_pa;
_temp24.f1=( int)(( unsigned int) pos_s->col);{ void* _temp18=( void*)& _temp24;
struct Cyc_Stdio_Int_pa_struct _temp23; _temp23.tag= Cyc_Stdio_Int_pa; _temp23.f1=(
int)(( unsigned int) pos_e->line_no);{ void* _temp19=( void*)& _temp23; struct
Cyc_Stdio_Int_pa_struct _temp22; _temp22.tag= Cyc_Stdio_Int_pa; _temp22.f1=( int)((
unsigned int) pos_e->col);{ void* _temp20=( void*)& _temp22; void* _temp14[ 5u]={
_temp16, _temp17, _temp18, _temp19, _temp20}; struct _tagged_arr _temp15={( void*)
_temp14,( void*) _temp14,( void*)( _temp14 + 5u)}; _temp13( _temp21, _temp15);}}}}}});}
else{ return({ struct _tagged_arr(* _temp27)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp36= _tag_arr("%s(%d:%d)-%s(%d:%d)",
sizeof( unsigned char), 20u); struct Cyc_Stdio_String_pa_struct _temp42; _temp42.tag=
Cyc_Stdio_String_pa; _temp42.f1=( struct _tagged_arr) pos_s->logical_file;{ void*
_temp30=( void*)& _temp42; struct Cyc_Stdio_Int_pa_struct _temp41; _temp41.tag=
Cyc_Stdio_Int_pa; _temp41.f1=( int)(( unsigned int) pos_s->line_no);{ void*
_temp31=( void*)& _temp41; struct Cyc_Stdio_Int_pa_struct _temp40; _temp40.tag=
Cyc_Stdio_Int_pa; _temp40.f1=( int)(( unsigned int) pos_s->col);{ void* _temp32=(
void*)& _temp40; struct Cyc_Stdio_String_pa_struct _temp39; _temp39.tag= Cyc_Stdio_String_pa;
_temp39.f1=( struct _tagged_arr) pos_e->logical_file;{ void* _temp33=( void*)&
_temp39; struct Cyc_Stdio_Int_pa_struct _temp38; _temp38.tag= Cyc_Stdio_Int_pa;
_temp38.f1=( int)(( unsigned int) pos_e->line_no);{ void* _temp34=( void*)&
_temp38; struct Cyc_Stdio_Int_pa_struct _temp37; _temp37.tag= Cyc_Stdio_Int_pa;
_temp37.f1=( int)(( unsigned int) pos_e->col);{ void* _temp35=( void*)& _temp37;
void* _temp28[ 6u]={ _temp30, _temp31, _temp32, _temp33, _temp34, _temp35};
struct _tagged_arr _temp29={( void*) _temp28,( void*) _temp28,( void*)( _temp28
+ 6u)}; _temp27( _temp36, _temp29);}}}}}}});}} struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment* s){ if( s == 0){ return({ struct _tagged_arr(*
_temp43)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf; struct
_tagged_arr _temp47= _tag_arr("%s", sizeof( unsigned char), 3u); struct Cyc_Stdio_String_pa_struct
_temp48; _temp48.tag= Cyc_Stdio_String_pa; _temp48.f1=( struct _tagged_arr) Cyc_Position_source;{
void* _temp46=( void*)& _temp48; void* _temp44[ 1u]={ _temp46}; struct
_tagged_arr _temp45={( void*) _temp44,( void*) _temp44,( void*)( _temp44 + 1u)};
_temp43( _temp47, _temp45);}});}{ struct Cyc_Lineno_Pos* pos_s= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( s))->start);
struct Cyc_Lineno_Pos* pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,((
struct Cyc_Position_Segment*) _check_null( s))->end); return Cyc_Position_string_of_pos_pr(
pos_s, pos_e);}} static struct Cyc_Lineno_Pos* Cyc_Position_new_pos(){ return({
struct Cyc_Lineno_Pos* _temp49=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof(
struct Cyc_Lineno_Pos)); _temp49->logical_file= _tag_arr("", sizeof(
unsigned char), 1u); _temp49->line= Cyc_Core_new_string( 0); _temp49->line_no= 0;
_temp49->col= 0; _temp49;});} struct _tuple0{ int f1; struct Cyc_Lineno_Pos* f2;
} ; struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*
segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List* _temp50= segs;
for( 0; _temp50 != 0; _temp50=(( struct Cyc_List_List*) _check_null( _temp50))->tl){
if(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null( _temp50))->hd
== 0){ continue;} places=({ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp51->hd=( void*)({ struct _tuple0*
_temp54=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp54->f1=((
struct Cyc_Position_Segment*) _check_null(( struct Cyc_Position_Segment*)((
struct Cyc_List_List*) _check_null( _temp50))->hd))->end; _temp54->f2= Cyc_Position_new_pos();
_temp54;}); _temp51->tl=({ struct Cyc_List_List* _temp52=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp52->hd=( void*)({ struct _tuple0*
_temp53=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp53->f1=((
struct Cyc_Position_Segment*) _check_null(( struct Cyc_Position_Segment*)((
struct Cyc_List_List*) _check_null( _temp50))->hd))->start; _temp53->f2= Cyc_Position_new_pos();
_temp53;}); _temp52->tl= places; _temp52;}); _temp51;});}} Cyc_Lineno_poss_of_abss(
Cyc_Position_source, places);{ struct Cyc_List_List* ans= 0; places=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( places); for( 0;
segs != 0; segs=(( struct Cyc_List_List*) _check_null( segs))->tl){ if(( struct
Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null( segs))->hd == 0){
ans=({ struct Cyc_List_List* _temp55=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp55->hd=( void*)({ struct _tagged_arr* _temp56=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp56[ 0]=({
struct _tagged_arr(* _temp57)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp61= _tag_arr("%s(unknown)", sizeof( unsigned char), 12u);
struct Cyc_Stdio_String_pa_struct _temp62; _temp62.tag= Cyc_Stdio_String_pa;
_temp62.f1=( struct _tagged_arr) Cyc_Position_source;{ void* _temp60=( void*)&
_temp62; void* _temp58[ 1u]={ _temp60}; struct _tagged_arr _temp59={( void*)
_temp58,( void*) _temp58,( void*)( _temp58 + 1u)}; _temp57( _temp61, _temp59);}});
_temp56;}); _temp55->tl= ans; _temp55;});} else{ ans=({ struct Cyc_List_List*
_temp63=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp63->hd=( void*)({ struct _tagged_arr* _temp64=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp64[ 0]= Cyc_Position_string_of_pos_pr((*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( places))->hd)).f2,(*((
struct _tuple0*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( places))->tl))->hd)).f2); _temp64;}); _temp63->tl= ans; _temp63;});
places=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( places))->tl))->tl;}} return ans;}} struct Cyc_Position_Error;
struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*
l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error* _temp65=(
struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp65->source= Cyc_Position_source; _temp65->seg= l; _temp65->kind=( void*)((
void*) Cyc_Position_Lex); _temp65->desc= desc; _temp65;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_arr
desc){ return({ struct Cyc_Position_Error* _temp66=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp66->source= Cyc_Position_source;
_temp66->seg= l; _temp66->kind=( void*)(( void*) Cyc_Position_Parse); _temp66->desc=
desc; _temp66;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_arr desc){ return({ struct Cyc_Position_Error*
_temp67=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp67->source= Cyc_Position_source; _temp67->seg= l; _temp67->kind=( void*)((
void*) Cyc_Position_Elab); _temp67->desc= desc; _temp67;});} unsigned char Cyc_Position_Nocontext[
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
pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons _temp68;
_push_handler(& _temp68);{ int _temp70= 0; if( setjmp( _temp68.handler)){
_temp70= 1;} if( ! _temp70){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,((
struct Cyc_Position_Segment*) _check_null( seg))->start); pos_e= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( seg))->end);;
_pop_handler();} else{ void* _temp69=( void*) _exn_thrown; void* _temp72=
_temp69; _LL74: goto _LL75; _LL76: goto _LL77; _LL75:( int) _throw(( void*) Cyc_Position_Nocontext);
_LL77:( void) _throw( _temp72); _LL73:;}}}{ struct Cyc_Lineno_Pos _temp80; int
_temp81; int _temp83; struct _tagged_arr _temp85; struct Cyc_Lineno_Pos* _temp78=
pos_s; _temp80=* _temp78; _LL86: _temp85= _temp80.line; goto _LL84; _LL84:
_temp83= _temp80.line_no; goto _LL82; _LL82: _temp81= _temp80.col; goto _LL79;
_LL79: { struct Cyc_Lineno_Pos _temp89; int _temp90; int _temp92; struct
_tagged_arr _temp94; struct Cyc_Lineno_Pos* _temp87= pos_e; _temp89=* _temp87;
_LL95: _temp94= _temp89.line; goto _LL93; _LL93: _temp92= _temp89.line_no; goto
_LL91; _LL91: _temp90= _temp89.col; goto _LL88; _LL88: if( _temp83 == _temp92){
int n= Cyc_Position_line_length / 3; struct _tagged_arr sec_one= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp85, 0, _temp81)); struct
_tagged_arr sec_two= Cyc_Position_trunc( n, Cyc_String_substring(( struct
_tagged_arr) _temp85, _temp81, _temp90 - _temp81)); struct _tagged_arr sec_three=
Cyc_Position_trunc( n, Cyc_String_substring(( struct _tagged_arr) _temp85,
_temp81, Cyc_String_strlen(( struct _tagged_arr) _temp85) - _temp90)); return({
struct _tuple1* _temp96=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp96->f1=({ struct _tagged_arr(* _temp97)( struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_aprintf; struct _tagged_arr _temp103= _tag_arr("%s%s%s",
sizeof( unsigned char), 7u); struct Cyc_Stdio_String_pa_struct _temp106;
_temp106.tag= Cyc_Stdio_String_pa; _temp106.f1=( struct _tagged_arr) sec_one;{
void* _temp100=( void*)& _temp106; struct Cyc_Stdio_String_pa_struct _temp105;
_temp105.tag= Cyc_Stdio_String_pa; _temp105.f1=( struct _tagged_arr) sec_two;{
void* _temp101=( void*)& _temp105; struct Cyc_Stdio_String_pa_struct _temp104;
_temp104.tag= Cyc_Stdio_String_pa; _temp104.f1=( struct _tagged_arr) sec_three;{
void* _temp102=( void*)& _temp104; void* _temp98[ 3u]={ _temp100, _temp101,
_temp102}; struct _tagged_arr _temp99={( void*) _temp98,( void*) _temp98,( void*)(
_temp98 + 3u)}; _temp97( _temp103, _temp99);}}}}); _temp96->f2=( int) Cyc_String_strlen((
struct _tagged_arr) sec_one); _temp96->f3=( int)( Cyc_String_strlen(( struct
_tagged_arr) sec_one) + Cyc_String_strlen(( struct _tagged_arr) sec_two));
_temp96;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct _tagged_arr
sec_one= Cyc_Position_trunc( n, Cyc_String_substring(( struct _tagged_arr)
_temp85, 0, _temp81)); struct _tagged_arr sec_two= Cyc_Position_trunc( n, Cyc_String_substring((
struct _tagged_arr) _temp85, _temp81, Cyc_String_strlen(( struct _tagged_arr)
_temp85) - _temp81)); struct _tagged_arr sec_three= Cyc_Position_trunc( n, Cyc_String_substring((
struct _tagged_arr) _temp94, 0, _temp90)); struct _tagged_arr sec_four= Cyc_Position_trunc(
n, Cyc_String_substring(( struct _tagged_arr) _temp94, _temp90, Cyc_String_strlen((
struct _tagged_arr) _temp94) - _temp90)); return({ struct _tuple1* _temp107=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp107->f1=({ struct
_tagged_arr(* _temp108)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_aprintf;
struct _tagged_arr _temp115= _tag_arr("%s%s.\\.%s%s", sizeof( unsigned char), 12u);
struct Cyc_Stdio_String_pa_struct _temp119; _temp119.tag= Cyc_Stdio_String_pa;
_temp119.f1=( struct _tagged_arr) sec_one;{ void* _temp111=( void*)& _temp119;
struct Cyc_Stdio_String_pa_struct _temp118; _temp118.tag= Cyc_Stdio_String_pa;
_temp118.f1=( struct _tagged_arr) sec_two;{ void* _temp112=( void*)& _temp118;
struct Cyc_Stdio_String_pa_struct _temp117; _temp117.tag= Cyc_Stdio_String_pa;
_temp117.f1=( struct _tagged_arr) sec_three;{ void* _temp113=( void*)& _temp117;
struct Cyc_Stdio_String_pa_struct _temp116; _temp116.tag= Cyc_Stdio_String_pa;
_temp116.f1=( struct _tagged_arr) sec_four;{ void* _temp114=( void*)& _temp116;
void* _temp109[ 4u]={ _temp111, _temp112, _temp113, _temp114}; struct
_tagged_arr _temp110={( void*) _temp109,( void*) _temp109,( void*)( _temp109 + 4u)};
_temp108( _temp115, _temp110);}}}}}); _temp107->f2=( int) Cyc_String_strlen((
struct _tagged_arr) sec_one); _temp107->f3=( int)((( Cyc_String_strlen(( struct
_tagged_arr) sec_one) + Cyc_String_strlen(( struct _tagged_arr) sec_two)) + 3) +
Cyc_String_strlen(( struct _tagged_arr) sec_three)); _temp107;});}}}}} static
int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
unsigned char Cyc_Position_Error[ 10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{
unsigned char* tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context=
0; int Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout);
if( Cyc_Position_first_error){({ int(* _temp120)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp123= Cyc_Stdio_stderr; struct _tagged_arr _temp124= _tag_arr("\n", sizeof(
unsigned char), 2u); void* _temp121[ 0u]={}; struct _tagged_arr _temp122={( void*)
_temp121,( void*) _temp121,( void*)( _temp121 + 0u)}; _temp120( _temp123,
_temp124, _temp122);}); Cyc_Position_first_error= 0;}({ int(* _temp125)( struct
Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf;
struct Cyc_Stdio___sFILE* _temp130= Cyc_Stdio_stderr; struct _tagged_arr
_temp131= _tag_arr("%s: %s\n", sizeof( unsigned char), 8u); struct Cyc_Stdio_String_pa_struct
_temp133; _temp133.tag= Cyc_Stdio_String_pa; _temp133.f1=( struct _tagged_arr)
Cyc_Position_string_of_segment( e->seg);{ void* _temp128=( void*)& _temp133;
struct Cyc_Stdio_String_pa_struct _temp132; _temp132.tag= Cyc_Stdio_String_pa;
_temp132.f1=( struct _tagged_arr) e->desc;{ void* _temp129=( void*)& _temp132;
void* _temp126[ 2u]={ _temp128, _temp129}; struct _tagged_arr _temp127={( void*)
_temp126,( void*) _temp126,( void*)( _temp126 + 2u)}; _temp125( _temp130,
_temp131, _temp127);}}}); if( Cyc_Position_print_context){ struct _handler_cons
_temp134; _push_handler(& _temp134);{ int _temp136= 0; if( setjmp( _temp134.handler)){
_temp136= 1;} if( ! _temp136){{ struct _tuple1* x= Cyc_Position_get_context( e->seg);
struct _tagged_arr marker_str=({ unsigned int _temp146=( unsigned int)((* x).f3
+ 1); unsigned char* _temp147=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char) * _temp146); struct _tagged_arr _temp149= _tag_arr( _temp147,
sizeof( unsigned char),( unsigned int)((* x).f3 + 1));{ unsigned int _temp148=
_temp146; unsigned int i; for( i= 0; i < _temp148; i ++){ _temp147[ i]='\000';}};
_temp149;}); int i= - 1; while( ++ i <(* x).f2) {*(( unsigned char*)
_check_unknown_subscript( marker_str, sizeof( unsigned char), i))=' ';} while(
++ i <(* x).f3) {*(( unsigned char*) _check_unknown_subscript( marker_str,
sizeof( unsigned char), i))='^';}({ int(* _temp137)( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE*
_temp142= Cyc_Stdio_stderr; struct _tagged_arr _temp143= _tag_arr("  %s\n  %s\n",
sizeof( unsigned char), 11u); struct Cyc_Stdio_String_pa_struct _temp145;
_temp145.tag= Cyc_Stdio_String_pa; _temp145.f1=( struct _tagged_arr)(* x).f1;{
void* _temp140=( void*)& _temp145; struct Cyc_Stdio_String_pa_struct _temp144;
_temp144.tag= Cyc_Stdio_String_pa; _temp144.f1=( struct _tagged_arr) marker_str;{
void* _temp141=( void*)& _temp144; void* _temp138[ 2u]={ _temp140, _temp141};
struct _tagged_arr _temp139={( void*) _temp138,( void*) _temp138,( void*)(
_temp138 + 2u)}; _temp137( _temp142, _temp143, _temp139);}}});}; _pop_handler();}
else{ void* _temp135=( void*) _exn_thrown; void* _temp151= _temp135; _LL153: if(
_temp151 == Cyc_Position_Nocontext){ goto _LL154;} else{ goto _LL155;} _LL155:
goto _LL156; _LL154: goto _LL152; _LL156:( void) _throw( _temp151); _LL152:;}}}
Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);} void Cyc_Position_reset_position(
struct _tagged_arr s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} void
Cyc_Position_set_position_file( struct _tagged_arr s){ Cyc_Position_source= s;
Cyc_Position_error_b= 0;} struct _tagged_arr Cyc_Position_get_position_file(){
return Cyc_Position_source;}