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
segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List* _temp32= segs;
goto _LL33; _LL33: for( 0; _temp32 != 0; _temp32=({ struct Cyc_List_List*
_temp34= _temp32; if( _temp34 == 0){ _throw( Null_Exception);} _temp34->tl;})){
if(( struct Cyc_Position_Segment*)({ struct Cyc_List_List* _temp35= _temp32; if(
_temp35 == 0){ _throw( Null_Exception);} _temp35->hd;}) == 0){ continue;} places=({
struct Cyc_List_List* _temp36=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp36->hd=( void*)({ struct _tuple0* _temp41=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp41->f1=({ struct Cyc_Position_Segment*
_temp43=( struct Cyc_Position_Segment*)({ struct Cyc_List_List* _temp42= _temp32;
if( _temp42 == 0){ _throw( Null_Exception);} _temp42->hd;}); if( _temp43 == 0){
_throw( Null_Exception);} _temp43->end;}); _temp41->f2= Cyc_Position_new_pos();
_temp41;}); _temp36->tl=({ struct Cyc_List_List* _temp37=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp37->hd=( void*)({ struct _tuple0*
_temp38=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp38->f1=({
struct Cyc_Position_Segment* _temp40=( struct Cyc_Position_Segment*)({ struct
Cyc_List_List* _temp39= _temp32; if( _temp39 == 0){ _throw( Null_Exception);}
_temp39->hd;}); if( _temp40 == 0){ _throw( Null_Exception);} _temp40->start;});
_temp38->f2= Cyc_Position_new_pos(); _temp38;}); _temp37->tl= places; _temp37;});
_temp36;});}} Cyc_Lineno_poss_of_abss( Cyc_Position_source, places);{ struct Cyc_List_List*
ans= 0; places=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
places); for( 0; segs != 0; segs=({ struct Cyc_List_List* _temp44= segs; if(
_temp44 == 0){ _throw( Null_Exception);} _temp44->tl;})){ if(( struct Cyc_Position_Segment*)({
struct Cyc_List_List* _temp45= segs; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->hd;}) == 0){ ans=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp46->hd=( void*)({ struct
_tagged_string* _temp47=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp47[ 0]=({ struct _tagged_string _temp48= Cyc_Position_source;
xprintf("%.*s(unknown)", _temp48.last_plus_one - _temp48.curr, _temp48.curr);});
_temp47;}); _temp46->tl= ans; _temp46;});} else{ ans=({ struct Cyc_List_List*
_temp49=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp49->hd=( void*)({ struct _tagged_string* _temp50=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp50[ 0]= Cyc_Position_string_of_pos_pr((*((
struct _tuple0*)({ struct Cyc_List_List* _temp51= places; if( _temp51 == 0){
_throw( Null_Exception);} _temp51->hd;}))).f2,(*(( struct _tuple0*)({ struct Cyc_List_List*
_temp53=({ struct Cyc_List_List* _temp52= places; if( _temp52 == 0){ _throw(
Null_Exception);} _temp52->tl;}); if( _temp53 == 0){ _throw( Null_Exception);}
_temp53->hd;}))).f2); _temp50;}); _temp49->tl= ans; _temp49;}); places=({ struct
Cyc_List_List* _temp55=({ struct Cyc_List_List* _temp54= places; if( _temp54 ==
0){ _throw( Null_Exception);} _temp54->tl;}); if( _temp55 == 0){ _throw(
Null_Exception);} _temp55->tl;});}} return ans;}} struct Cyc_Position_Error;
struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*
l, struct _tagged_string desc){ return({ struct Cyc_Position_Error* _temp56=(
struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp56->source= Cyc_Position_source; _temp56->seg= l; _temp56->kind=( void*)((
void*) Cyc_Position_Lex); _temp56->desc= desc; _temp56;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_string
desc){ return({ struct Cyc_Position_Error* _temp57=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp57->source= Cyc_Position_source;
_temp57->seg= l; _temp57->kind=( void*)(( void*) Cyc_Position_Parse); _temp57->desc=
desc; _temp57;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_string desc){ return({ struct Cyc_Position_Error*
_temp58=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp58->source= Cyc_Position_source; _temp58->seg= l; _temp58->kind=( void*)((
void*) Cyc_Position_Elab); _temp58->desc= desc; _temp58;});} char Cyc_Position_Nocontext[
14u]; static struct _tagged_string Cyc_Position_trunc( int n, struct
_tagged_string s){ int len=( int) Cyc_String_strlen( s); if( len < n){ return s;}{
int len_one=( n - 3) / 2; int len_two=( n - 3) - len_one; struct _tagged_string
ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy( ans, 0, s, 0,(
unsigned int) len_one); Cyc_String_strncpy( ans, len_one,( struct _tagged_string)({
char* _temp59=( char*)"..."; struct _tagged_string _temp60; _temp60.curr=
_temp59; _temp60.base= _temp59; _temp60.last_plus_one= _temp59 + 4; _temp60;}),
0,( unsigned int) 3); Cyc_String_strncpy( ans, len_one + 3, s, len - len_two,(
unsigned int) len_two); return ans;}} static int Cyc_Position_line_length= 76;
struct _tuple1{ struct _tagged_string f1; int f2; int f3; } ; static struct
_tuple1* Cyc_Position_get_context( struct Cyc_Position_Segment* seg){ if( seg ==
0){( void) _throw(( void*) Cyc_Position_Nocontext);}{ struct Cyc_Lineno_Pos*
pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct _handler_cons _temp61;
_push_handler(& _temp61);{ int _temp63= 0; if( setjmp( _temp61.handler)){
_temp63= 1;} if( ! _temp63){ pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({
struct Cyc_Position_Segment* _temp64= seg; if( _temp64 == 0){ _throw(
Null_Exception);} _temp64->start;})); pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,({
struct Cyc_Position_Segment* _temp65= seg; if( _temp65 == 0){ _throw(
Null_Exception);} _temp65->end;}));; _pop_handler();} else{ void* _temp62=( void*)
_exn_thrown; void* _temp67= _temp62; _LL69: goto _LL70; _LL71: goto _LL72; _LL70:(
void) _throw(( void*) Cyc_Position_Nocontext); _LL72:( void) _throw( _temp67);
_LL68:;}}}{ struct Cyc_Lineno_Pos _temp75; int _temp76; int _temp78; struct
_tagged_string _temp80; struct _tagged_string _temp82; struct Cyc_Lineno_Pos*
_temp73= pos_s; _temp75=* _temp73; _LL83: _temp82=( struct _tagged_string)
_temp75.logical_file; goto _LL81; _LL81: _temp80=( struct _tagged_string)
_temp75.line; goto _LL79; _LL79: _temp78=( int) _temp75.line_no; goto _LL77;
_LL77: _temp76=( int) _temp75.col; goto _LL74; _LL74: { struct Cyc_Lineno_Pos
_temp86; int _temp87; int _temp89; struct _tagged_string _temp91; struct
_tagged_string _temp93; struct Cyc_Lineno_Pos* _temp84= pos_e; _temp86=* _temp84;
_LL94: _temp93=( struct _tagged_string) _temp86.logical_file; goto _LL92; _LL92:
_temp91=( struct _tagged_string) _temp86.line; goto _LL90; _LL90: _temp89=( int)
_temp86.line_no; goto _LL88; _LL88: _temp87=( int) _temp86.col; goto _LL85;
_LL85: if( _temp78 == _temp89){ int n= Cyc_Position_line_length / 3; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp80, 0,(
unsigned int) _temp76)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp80, _temp76,( unsigned int)( _temp87 - _temp76)));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp80, _temp76, Cyc_String_strlen( _temp80) - _temp87)); return({ struct
_tuple1* _temp95=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp95->f1=({
struct _tagged_string _temp96= sec_one; struct _tagged_string _temp97= sec_two;
struct _tagged_string _temp98= sec_three; xprintf("%.*s%.*s%.*s", _temp96.last_plus_one
- _temp96.curr, _temp96.curr, _temp97.last_plus_one - _temp97.curr, _temp97.curr,
_temp98.last_plus_one - _temp98.curr, _temp98.curr);}); _temp95->f2=( int) Cyc_String_strlen(
sec_one); _temp95->f3=( int)( Cyc_String_strlen( sec_one) + Cyc_String_strlen(
sec_two)); _temp95;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp80, 0,(
unsigned int) _temp76)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp80, _temp76, Cyc_String_strlen( _temp80) - _temp76));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp91, 0,( unsigned int) _temp87)); struct _tagged_string sec_four= Cyc_Position_trunc(
n, Cyc_String_substring( _temp91, _temp87, Cyc_String_strlen( _temp91) - _temp87));
return({ struct _tuple1* _temp99=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp99->f1=({ struct _tagged_string _temp100= sec_one; struct
_tagged_string _temp101= sec_two; struct _tagged_string _temp102= sec_three;
struct _tagged_string _temp103= sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s",
_temp100.last_plus_one - _temp100.curr, _temp100.curr, _temp101.last_plus_one -
_temp101.curr, _temp101.curr, _temp102.last_plus_one - _temp102.curr, _temp102.curr,
_temp103.last_plus_one - _temp103.curr, _temp103.curr);}); _temp99->f2=( int)
Cyc_String_strlen( sec_one); _temp99->f3=( int)((( Cyc_String_strlen( sec_one) +
Cyc_String_strlen( sec_two)) + 3) + Cyc_String_strlen( sec_three)); _temp99;});}}}}}
static int Cyc_Position_error_b= 0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;}
char Cyc_Position_Error[ 10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{
char* tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context= 0;
int Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout);
if( Cyc_Position_first_error){ fprintf( Cyc_Stdio_stderr,"\n"); Cyc_Position_first_error=
0;}({ struct _tagged_string _temp104= Cyc_Position_string_of_segment( e->seg);
struct _tagged_string _temp105= e->desc; fprintf( Cyc_Stdio_stderr,"%.*s: %.*s\n",
_temp104.last_plus_one - _temp104.curr, _temp104.curr, _temp105.last_plus_one -
_temp105.curr, _temp105.curr);}); if( Cyc_Position_print_context){ struct
_handler_cons _temp106; _push_handler(& _temp106);{ int _temp108= 0; if( setjmp(
_temp106.handler)){ _temp108= 1;} if( ! _temp108){{ struct _tuple1* x= Cyc_Position_get_context(
e->seg); struct _tagged_string marker_str=({ unsigned int _temp117=(
unsigned int)((* x).f3 + 1); char* _temp118=( char*) GC_malloc_atomic( sizeof(
char) * _temp117); struct _tagged_string _temp121={ _temp118, _temp118, _temp118
+ _temp117};{ unsigned int _temp119= _temp117; unsigned int i; for( i= 0; i <
_temp119; i ++){ _temp118[ i]='\000';}}; _temp121;}); int i= - 1; while(( ++ i)
<(* x).f2) {({ struct _tagged_string _temp109= marker_str; char* _temp111=
_temp109.curr + i; if( _temp109.base == 0? 1:( _temp111 < _temp109.base? 1:
_temp111 >= _temp109.last_plus_one)){ _throw( Null_Exception);}* _temp111=' ';});}
while(( ++ i) <(* x).f3) {({ struct _tagged_string _temp112= marker_str; char*
_temp114= _temp112.curr + i; if( _temp112.base == 0? 1:( _temp114 < _temp112.base?
1: _temp114 >= _temp112.last_plus_one)){ _throw( Null_Exception);}* _temp114='^';});}({
struct _tagged_string _temp115=(* x).f1; struct _tagged_string _temp116=
marker_str; fprintf( Cyc_Stdio_stderr,"  %.*s\n  %.*s\n", _temp115.last_plus_one
- _temp115.curr, _temp115.curr, _temp116.last_plus_one - _temp116.curr, _temp116.curr);});};
_pop_handler();} else{ void* _temp107=( void*) _exn_thrown; void* _temp123=
_temp107; _LL125: if( _temp123 == Cyc_Position_Nocontext){ goto _LL126;} else{
goto _LL127;} _LL127: goto _LL128; _LL126: goto _LL124; _LL128:( void) _throw(
_temp123); _LL124:;}}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);}
void Cyc_Position_reset_position( struct _tagged_string s){ Cyc_Position_source=
s; Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file( struct
_tagged_string s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;} struct
_tagged_string Cyc_Position_get_position_file(){ return Cyc_Position_source;}