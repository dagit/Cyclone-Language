#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct
_tagged_string Cyc_Core_new_string( int); extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
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
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
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
struct _tagged_string, int ofs, unsigned int n); char Cyc_Position_Exit[ 9u]="\000\000\000\000Exit";
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
14u]="\000\000\000\000Nocontext"; static struct _tagged_string Cyc_Position_trunc(
int n, struct _tagged_string s){ int len=( int) Cyc_String_strlen( s); if( len <
n){ return s;}{ int len_one=( n - 3) / 2; int len_two=( n - 3) - len_one; struct
_tagged_string ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy( ans, 0, s,
0,( unsigned int) len_one); Cyc_String_strncpy( ans, len_one,( struct
_tagged_string)({ char* _temp57=( char*)"..."; struct _tagged_string _temp58;
_temp58.curr= _temp57; _temp58.base= _temp57; _temp58.last_plus_one= _temp57 + 4;
_temp58;}), 0,( unsigned int) 3); Cyc_String_strncpy( ans, len_one + 3, s, len -
len_two,( unsigned int) len_two); return ans;}} static int Cyc_Position_line_length=
76; struct _tuple1{ struct _tagged_string f1; int f2; int f3; } ; static struct
_tuple1* Cyc_Position_get_context( struct Cyc_Position_Segment* seg){ if( seg ==
0){( void) _throw(( void*) Cyc_Position_Nocontext);}{ struct Cyc_Lineno_Pos*
pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons _temp59;
_push_handler(& _temp59);{ int _temp61= 0; if( setjmp( _temp59.handler)){
_temp61= 1;} if( ! _temp61){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({
struct Cyc_Position_Segment* _temp62= seg; if( _temp62 == 0){ _throw(
Null_Exception);} _temp62->start;})); pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({
struct Cyc_Position_Segment* _temp63= seg; if( _temp63 == 0){ _throw(
Null_Exception);} _temp63->end;}));; _pop_handler();} else{ void* _temp60=( void*)
_exn_thrown; void* _temp65= _temp60; _LL67: goto _LL68; _LL69: goto _LL70; _LL68:(
void) _throw(( void*) Cyc_Position_Nocontext); _LL70:( void) _throw( _temp65);
_LL66:;}}}{ struct Cyc_Lineno_Pos _temp73; int _temp74; int _temp76; struct
_tagged_string _temp78; struct _tagged_string _temp80; struct Cyc_Lineno_Pos*
_temp71= pos_s; _temp73=* _temp71; _LL81: _temp80=( struct _tagged_string)
_temp73.logical_file; goto _LL79; _LL79: _temp78=( struct _tagged_string)
_temp73.line; goto _LL77; _LL77: _temp76=( int) _temp73.line_no; goto _LL75;
_LL75: _temp74=( int) _temp73.col; goto _LL72; _LL72: { struct Cyc_Lineno_Pos
_temp84; int _temp85; int _temp87; struct _tagged_string _temp89; struct
_tagged_string _temp91; struct Cyc_Lineno_Pos* _temp82= pos_e; _temp84=* _temp82;
_LL92: _temp91=( struct _tagged_string) _temp84.logical_file; goto _LL90; _LL90:
_temp89=( struct _tagged_string) _temp84.line; goto _LL88; _LL88: _temp87=( int)
_temp84.line_no; goto _LL86; _LL86: _temp85=( int) _temp84.col; goto _LL83;
_LL83: if( _temp76 == _temp87){ int n= Cyc_Position_line_length / 3; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp78, 0,(
unsigned int) _temp74)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp78, _temp74,( unsigned int)( _temp85 - _temp74)));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp78, _temp74, Cyc_String_strlen( _temp78) - _temp85)); return({ struct
_tuple1* _temp93=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp93->f1=({
struct _tagged_string _temp94= sec_one; struct _tagged_string _temp95= sec_two;
struct _tagged_string _temp96= sec_three; xprintf("%.*s%.*s%.*s", _temp94.last_plus_one
- _temp94.curr, _temp94.curr, _temp95.last_plus_one - _temp95.curr, _temp95.curr,
_temp96.last_plus_one - _temp96.curr, _temp96.curr);}); _temp93->f2=( int) Cyc_String_strlen(
sec_one); _temp93->f3=( int)( Cyc_String_strlen( sec_one) + Cyc_String_strlen(
sec_two)); _temp93;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp78, 0,(
unsigned int) _temp74)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp78, _temp74, Cyc_String_strlen( _temp78) - _temp74));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp89, 0,( unsigned int) _temp85)); struct _tagged_string sec_four= Cyc_Position_trunc(
n, Cyc_String_substring( _temp89, _temp85, Cyc_String_strlen( _temp89) - _temp85));
return({ struct _tuple1* _temp97=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp97->f1=({ struct _tagged_string _temp98= sec_one; struct
_tagged_string _temp99= sec_two; struct _tagged_string _temp100= sec_three;
struct _tagged_string _temp101= sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s",
_temp98.last_plus_one - _temp98.curr, _temp98.curr, _temp99.last_plus_one -
_temp99.curr, _temp99.curr, _temp100.last_plus_one - _temp100.curr, _temp100.curr,
_temp101.last_plus_one - _temp101.curr, _temp101.curr);}); _temp97->f2=( int)
Cyc_String_strlen( sec_one); _temp97->f3=( int)((( Cyc_String_strlen( sec_one) +
Cyc_String_strlen( sec_two)) + 3) + Cyc_String_strlen( sec_three)); _temp97;});}}}}}
static int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
char Cyc_Position_Error[ 10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{
char* tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context= 0;
int Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout);
if( Cyc_Position_first_error){ fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Position_first_error=
0;}({ struct _tagged_string _temp102= Cyc_Position_string_of_segment( e->seg);
struct _tagged_string _temp103= e->desc; fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp102.last_plus_one - _temp102.curr, _temp102.curr, _temp103.last_plus_one -
_temp103.curr, _temp103.curr);}); if( Cyc_Position_print_context){ struct
_handler_cons _temp104; _push_handler(& _temp104);{ int _temp106= 0; if( setjmp(
_temp104.handler)){ _temp106= 1;} if( ! _temp106){{ struct _tuple1* x= Cyc_Position_get_context(
e->seg); struct _tagged_string marker_str=({ unsigned int _temp115=(
unsigned int)((* x).f3 + 1); char* _temp116=( char*) GC_malloc_atomic( sizeof(
char) * _temp115); struct _tagged_string _temp119={ _temp116, _temp116, _temp116
+ _temp115};{ unsigned int _temp117= _temp115; unsigned int i; for( i= 0; i <
_temp117; i ++){ _temp116[ i]='\000';}}; _temp119;}); int i= - 1; while(( ++ i)
<(* x).f2) {({ struct _tagged_string _temp107= marker_str; char* _temp109=
_temp107.curr + i; if( _temp107.base == 0? 1:( _temp109 < _temp107.base? 1:
_temp109 >= _temp107.last_plus_one)){ _throw( Null_Exception);}* _temp109=' ';});}
while(( ++ i) <(* x).f3) {({ struct _tagged_string _temp110= marker_str; char*
_temp112= _temp110.curr + i; if( _temp110.base == 0? 1:( _temp112 < _temp110.base?
1: _temp112 >= _temp110.last_plus_one)){ _throw( Null_Exception);}* _temp112='^';});}({
struct _tagged_string _temp113=(* x).f1; struct _tagged_string _temp114=
marker_str; fprintf( Cyc_Stdio_stderr,"  %.*s\n  %.*s\n", _temp113.last_plus_one
- _temp113.curr, _temp113.curr, _temp114.last_plus_one - _temp114.curr, _temp114.curr);});};
_pop_handler();} else{ void* _temp105=( void*) _exn_thrown; void* _temp121=
_temp105; _LL123: if( _temp121 == Cyc_Position_Nocontext){ goto _LL124;} else{
goto _LL125;} _LL125: goto _LL126; _LL124: goto _LL122; _LL126:( void) _throw(
_temp121); _LL122:;}}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}
void Cyc_Position_reset_position( struct _tagged_string s){ Cyc_Position_source=
s; Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_string s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_string Cyc_Position_get_position_file(){ return Cyc_Position_source;}