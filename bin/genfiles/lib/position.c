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
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int
Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*);
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern struct Cyc_Lineno_Pos* Cyc_Lineno_pos_of_abs( struct
_tagged_string, int); extern void Cyc_Lineno_poss_of_abss( struct _tagged_string
filename, struct Cyc_List_List* places); extern char Cyc_Position_Exit[ 9u];
extern void Cyc_Position_reset_position( struct _tagged_string); extern void Cyc_Position_set_position_file(
struct _tagged_string); extern struct _tagged_string Cyc_Position_get_position_file();
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Position_string_of_loc(
int); extern struct _tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
extern struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*);
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*, struct _tagged_string); extern struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*, struct _tagged_string);
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext[ 14u]; extern int Cyc_Position_print_context;
extern void Cyc_Position_post_error( struct Cyc_Position_Error*); extern int Cyc_Position_error_p();
extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern struct
_tagged_string Cyc_String_strncpy( struct _tagged_string, int, struct
_tagged_string, int, unsigned int); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); char Cyc_Position_Exit[ 9u];
static char _temp2[ 1u]=""; static struct _tagged_string Cyc_Position_source=(
struct _tagged_string){ _temp2, _temp2, _temp2 + 1u}; struct Cyc_Position_Segment{
int start; int end; } ; struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int start, int end){ return({ struct Cyc_Position_Segment* _temp3=( struct Cyc_Position_Segment*)
GC_malloc( sizeof( struct Cyc_Position_Segment) * 1); _temp3[ 0]=({ struct Cyc_Position_Segment
_temp4; _temp4.start= start; _temp4.end= end; _temp4;}); _temp3;});} struct Cyc_Position_Segment*
Cyc_Position_segment_join( struct Cyc_Position_Segment* s1, struct Cyc_Position_Segment*
s2){ if( s1 == 0){ return s2;} if( s2 == 0){ return s1;} return({ struct Cyc_Position_Segment*
_temp5=( struct Cyc_Position_Segment*) GC_malloc( sizeof( struct Cyc_Position_Segment)
* 1); _temp5[ 0]=({ struct Cyc_Position_Segment _temp6; _temp6.start=({ struct
Cyc_Position_Segment* _temp8= s1; if( _temp8 == 0){ _throw( Null_Exception);}
_temp8->start;}); _temp6.end=({ struct Cyc_Position_Segment* _temp7= s2; if(
_temp7 == 0){ _throw( Null_Exception);} _temp7->end;}); _temp6;}); _temp5;});}
struct _tagged_string Cyc_Position_string_of_loc( int loc){ struct Cyc_Lineno_Pos*
pos= Cyc_Lineno_pos_of_abs( Cyc_Position_source, loc); return({ struct
_tagged_string _temp9= pos->logical_file; int _temp10= pos->line_no; int _temp11=
pos->col; xprintf("%.*s (%d:%d)", _temp9.last_plus_one - _temp9.curr, _temp9.curr,
_temp10, _temp11);});} static struct _tagged_string Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos* pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_String_strcmp(
pos_s->logical_file, pos_e->logical_file) == 0){ return({ struct _tagged_string
_temp12= pos_s->logical_file; int _temp13= pos_s->line_no; int _temp14= pos_s->col;
int _temp15= pos_e->line_no; int _temp16= pos_e->col; xprintf("%.*s(%d:%d-%d:%d)",
_temp12.last_plus_one - _temp12.curr, _temp12.curr, _temp13, _temp14, _temp15,
_temp16);});} else{ return({ struct _tagged_string _temp17= pos_s->logical_file;
int _temp18= pos_s->line_no; int _temp19= pos_s->col; struct _tagged_string
_temp20= pos_e->logical_file; int _temp21= pos_e->line_no; int _temp22= pos_e->col;
xprintf("%.*s(%d:%d)-%.*s(%d:%d)", _temp17.last_plus_one - _temp17.curr, _temp17.curr,
_temp18, _temp19, _temp20.last_plus_one - _temp20.curr, _temp20.curr, _temp21,
_temp22);});}} struct _tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*
s){ if( s == 0){ return({ struct _tagged_string _temp23= Cyc_Position_source;
xprintf("%.*s(unknown)", _temp23.last_plus_one - _temp23.curr, _temp23.curr);});}{
struct Cyc_Lineno_Pos* pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({
struct Cyc_Position_Segment* _temp25= s; if( _temp25 == 0){ _throw(
Null_Exception);} _temp25->start;})); struct Cyc_Lineno_Pos* pos_e= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,({ struct Cyc_Position_Segment* _temp24= s; if( _temp24 == 0){
_throw( Null_Exception);} _temp24->end;})); return Cyc_Position_string_of_pos_pr(
pos_s, pos_e);}} static struct Cyc_Lineno_Pos* Cyc_Position_new_pos(){ return({
struct Cyc_Lineno_Pos* _temp26=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof(
struct Cyc_Lineno_Pos) * 1); _temp26[ 0]=({ struct Cyc_Lineno_Pos _temp27;
_temp27.logical_file=( struct _tagged_string)({ char* _temp30=( char*)""; struct
_tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 1; _temp31;}); _temp27.line=( struct _tagged_string)({ char* _temp28=(
char*)""; struct _tagged_string _temp29; _temp29.curr= _temp28; _temp29.base=
_temp28; _temp29.last_plus_one= _temp28 + 1; _temp29;}); _temp27.line_no= 0;
_temp27.col= 0; _temp27;}); _temp26;});} struct _tuple0{ int f1; struct Cyc_Lineno_Pos*
f2; } ; struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*
segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List* segs2= segs; for(
0; segs2 != 0; segs2=({ struct Cyc_List_List* _temp32= segs2; if( _temp32 == 0){
_throw( Null_Exception);} _temp32->tl;})){ if(( struct Cyc_Position_Segment*)({
struct Cyc_List_List* _temp33= segs2; if( _temp33 == 0){ _throw( Null_Exception);}
_temp33->hd;}) == 0){ continue;} places=({ struct Cyc_List_List* _temp34=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp34->hd=(
void*)({ struct _tuple0* _temp39=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp39->f1=({ struct Cyc_Position_Segment* _temp41=( struct Cyc_Position_Segment*)({
struct Cyc_List_List* _temp40= segs2; if( _temp40 == 0){ _throw( Null_Exception);}
_temp40->hd;}); if( _temp41 == 0){ _throw( Null_Exception);} _temp41->end;});
_temp39->f2= Cyc_Position_new_pos(); _temp39;}); _temp34->tl=({ struct Cyc_List_List*
_temp35=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp35->hd=( void*)({ struct _tuple0* _temp36=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp36->f1=({ struct Cyc_Position_Segment* _temp38=(
struct Cyc_Position_Segment*)({ struct Cyc_List_List* _temp37= segs2; if(
_temp37 == 0){ _throw( Null_Exception);} _temp37->hd;}); if( _temp38 == 0){
_throw( Null_Exception);} _temp38->start;}); _temp36->f2= Cyc_Position_new_pos();
_temp36;}); _temp35->tl= places; _temp35;}); _temp34;});}} Cyc_Lineno_poss_of_abss(
Cyc_Position_source, places);{ struct Cyc_List_List* ans= 0; places=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( places); for( 0;
segs != 0; segs=({ struct Cyc_List_List* _temp42= segs; if( _temp42 == 0){
_throw( Null_Exception);} _temp42->tl;})){ if(( struct Cyc_Position_Segment*)({
struct Cyc_List_List* _temp43= segs; if( _temp43 == 0){ _throw( Null_Exception);}
_temp43->hd;}) == 0){ ans=({ struct Cyc_List_List* _temp44=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp44->hd=( void*)({ struct
_tagged_string* _temp45=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp45[ 0]=({ struct _tagged_string _temp46= Cyc_Position_source;
xprintf("%.*s(unknown)", _temp46.last_plus_one - _temp46.curr, _temp46.curr);});
_temp45;}); _temp44->tl= ans; _temp44;});} else{ ans=({ struct Cyc_List_List*
_temp47=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp47->hd=( void*)({ struct _tagged_string* _temp48=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp48[ 0]= Cyc_Position_string_of_pos_pr((*((
struct _tuple0*)({ struct Cyc_List_List* _temp49= places; if( _temp49 == 0){
_throw( Null_Exception);} _temp49->hd;}))).f2,(*(( struct _tuple0*)({ struct Cyc_List_List*
_temp51=({ struct Cyc_List_List* _temp50= places; if( _temp50 == 0){ _throw(
Null_Exception);} _temp50->tl;}); if( _temp51 == 0){ _throw( Null_Exception);}
_temp51->hd;}))).f2); _temp48;}); _temp47->tl= ans; _temp47;}); places=({ struct
Cyc_List_List* _temp53=({ struct Cyc_List_List* _temp52= places; if( _temp52 ==
0){ _throw( Null_Exception);} _temp52->tl;}); if( _temp53 == 0){ _throw(
Null_Exception);} _temp53->tl;});}} return ans;}} struct Cyc_Position_Error;
struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*
l, struct _tagged_string desc){ return({ struct Cyc_Position_Error* _temp54=(
struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp54->source= Cyc_Position_source; _temp54->seg= l; _temp54->kind=( void*)((
void*) Cyc_Position_Lex); _temp54->desc= desc; _temp54;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_string
desc){ return({ struct Cyc_Position_Error* _temp55=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp55->source= Cyc_Position_source;
_temp55->seg= l; _temp55->kind=( void*)(( void*) Cyc_Position_Parse); _temp55->desc=
desc; _temp55;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_string desc){ return({ struct Cyc_Position_Error*
_temp56=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp56->source= Cyc_Position_source; _temp56->seg= l; _temp56->kind=( void*)((
void*) Cyc_Position_Elab); _temp56->desc= desc; _temp56;});} char Cyc_Position_Nocontext[
14u]; static struct _tagged_string Cyc_Position_trunc( int n, struct
_tagged_string s){ int len=( int) Cyc_String_strlen( s); if( len < n){ return s;}{
int len_one=( n - 3) / 2; int len_two=( n - 3) - len_one; struct _tagged_string
ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy( ans, 0, s, 0,(
unsigned int) len_one); Cyc_String_strncpy( ans, len_one,( struct _tagged_string)({
char* _temp57=( char*)"..."; struct _tagged_string _temp58; _temp58.curr=
_temp57; _temp58.base= _temp57; _temp58.last_plus_one= _temp57 + 4; _temp58;}),
0,( unsigned int) 3); Cyc_String_strncpy( ans, len_one + 3, s, len - len_two,(
unsigned int) len_two); return ans;}} static int Cyc_Position_line_length= 76;
struct _tuple1{ struct _tagged_string f1; int f2; int f3; } ; static struct
_tuple1* Cyc_Position_get_context( struct Cyc_Position_Segment* seg){ if( seg ==
0){( void) _throw(( void*) Cyc_Position_Nocontext);}{ struct Cyc_Lineno_Pos*
pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons _temp59;
_push_handler(& _temp59);{ void* _temp60=( void*) setjmp( _temp59.handler); if(
! _temp60){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({ struct Cyc_Position_Segment*
_temp61= seg; if( _temp61 == 0){ _throw( Null_Exception);} _temp61->start;}));
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({ struct Cyc_Position_Segment*
_temp62= seg; if( _temp62 == 0){ _throw( Null_Exception);} _temp62->end;}));;
_pop_handler();} else{ void* _temp64= _temp60; _LL66: goto _LL67; _LL68: goto
_LL69; _LL67:( void) _throw(( void*) Cyc_Position_Nocontext); _LL69:( void)
_throw( _temp64); _LL65:;}}}{ struct Cyc_Lineno_Pos _temp72; int _temp73; int
_temp75; struct _tagged_string _temp77; struct _tagged_string _temp79; struct
Cyc_Lineno_Pos* _temp70= pos_s; _temp72=* _temp70; _LL80: _temp79=( struct
_tagged_string) _temp72.logical_file; goto _LL78; _LL78: _temp77=( struct
_tagged_string) _temp72.line; goto _LL76; _LL76: _temp75=( int) _temp72.line_no;
goto _LL74; _LL74: _temp73=( int) _temp72.col; goto _LL71; _LL71: { struct Cyc_Lineno_Pos
_temp83; int _temp84; int _temp86; struct _tagged_string _temp88; struct
_tagged_string _temp90; struct Cyc_Lineno_Pos* _temp81= pos_e; _temp83=* _temp81;
_LL91: _temp90=( struct _tagged_string) _temp83.logical_file; goto _LL89; _LL89:
_temp88=( struct _tagged_string) _temp83.line; goto _LL87; _LL87: _temp86=( int)
_temp83.line_no; goto _LL85; _LL85: _temp84=( int) _temp83.col; goto _LL82;
_LL82: if( _temp75 == _temp86){ int n= Cyc_Position_line_length / 3; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp77, 0,(
unsigned int) _temp73)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp77, _temp73,( unsigned int)( _temp84 - _temp73)));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp77, _temp73, Cyc_String_strlen( _temp77) - _temp84)); return({ struct
_tuple1* _temp92=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp92->f1=({
struct _tagged_string _temp93= sec_one; struct _tagged_string _temp94= sec_two;
struct _tagged_string _temp95= sec_three; xprintf("%.*s%.*s%.*s", _temp93.last_plus_one
- _temp93.curr, _temp93.curr, _temp94.last_plus_one - _temp94.curr, _temp94.curr,
_temp95.last_plus_one - _temp95.curr, _temp95.curr);}); _temp92->f2=( int) Cyc_String_strlen(
sec_one); _temp92->f3=( int)( Cyc_String_strlen( sec_one) + Cyc_String_strlen(
sec_two)); _temp92;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp77, 0,(
unsigned int) _temp73)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp77, _temp73, Cyc_String_strlen( _temp77) - _temp73));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp88, 0,( unsigned int) _temp84)); struct _tagged_string sec_four= Cyc_Position_trunc(
n, Cyc_String_substring( _temp88, _temp84, Cyc_String_strlen( _temp88) - _temp84));
return({ struct _tuple1* _temp96=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp96->f1=({ struct _tagged_string _temp97= sec_one; struct
_tagged_string _temp98= sec_two; struct _tagged_string _temp99= sec_three;
struct _tagged_string _temp100= sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s",
_temp97.last_plus_one - _temp97.curr, _temp97.curr, _temp98.last_plus_one -
_temp98.curr, _temp98.curr, _temp99.last_plus_one - _temp99.curr, _temp99.curr,
_temp100.last_plus_one - _temp100.curr, _temp100.curr);}); _temp96->f2=( int)
Cyc_String_strlen( sec_one); _temp96->f3=( int)((( Cyc_String_strlen( sec_one) +
Cyc_String_strlen( sec_two)) + 3) + Cyc_String_strlen( sec_three)); _temp96;});}}}}}
static int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
char Cyc_Position_Error[ 10u]="Error"; struct Cyc_Position_Error_struct{ char*
tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context= 0; int
Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush( Cyc_Stdio_stdout); if( Cyc_Position_first_error){
fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Position_first_error= 0;}({ struct
_tagged_string _temp101= Cyc_Position_string_of_segment( e->seg); struct
_tagged_string _temp102= e->desc; fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp101.last_plus_one - _temp101.curr, _temp101.curr, _temp102.last_plus_one -
_temp102.curr, _temp102.curr);}); if( Cyc_Position_print_context){ struct
_handler_cons _temp103; _push_handler(& _temp103);{ void* _temp104=( void*)
setjmp( _temp103.handler); if( ! _temp104){{ struct _tuple1* x= Cyc_Position_get_context(
e->seg); struct _tagged_string marker_str=({ unsigned int _temp113=(
unsigned int)((* x).f3 + 1); char* _temp114=( char*) GC_malloc_atomic( sizeof(
char) * _temp113); struct _tagged_string _temp117={ _temp114, _temp114, _temp114
+ _temp113};{ unsigned int _temp115= _temp113; unsigned int i; for( i= 0; i <
_temp115; i ++){ _temp114[ i]='\000';}}; _temp117;}); int i= - 1; while(( ++ i)
<(* x).f2) {({ struct _tagged_string _temp105= marker_str; char* _temp107=
_temp105.curr + i; if( _temp105.base == 0? 1:( _temp107 < _temp105.base? 1:
_temp107 >= _temp105.last_plus_one)){ _throw( Null_Exception);}* _temp107=' ';});}
while(( ++ i) <(* x).f3) {({ struct _tagged_string _temp108= marker_str; char*
_temp110= _temp108.curr + i; if( _temp108.base == 0? 1:( _temp110 < _temp108.base?
1: _temp110 >= _temp108.last_plus_one)){ _throw( Null_Exception);}* _temp110='^';});}({
struct _tagged_string _temp111=(* x).f1; struct _tagged_string _temp112=
marker_str; fprintf( Cyc_Stdio_stderr,"  %.*s\n  %.*s\n", _temp111.last_plus_one
- _temp111.curr, _temp111.curr, _temp112.last_plus_one - _temp112.curr, _temp112.curr);});};
_pop_handler();} else{ void* _temp119= _temp104; _LL121: if( _temp119 == Cyc_Position_Nocontext){
goto _LL122;} else{ goto _LL123;} _LL123: goto _LL124; _LL122: goto _LL120;
_LL124:( void) _throw( _temp119); _LL120:;}}} Cyc_Stdio_fflush( Cyc_Stdio_stderr);}
void Cyc_Position_reset_position( struct _tagged_string s){ Cyc_Position_source=
s; Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_string s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_string Cyc_Position_get_position_file(){ return Cyc_Position_source;}