#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
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
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern unsigned char Cyc_List_List_empty[ 15u];
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdout; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fflush(
struct Cyc_Stdio___sFILE*); extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_of_abs( struct _tagged_string, int); extern void Cyc_Lineno_poss_of_abss(
struct _tagged_string filename, struct Cyc_List_List* places); extern
unsigned char Cyc_Position_Exit[ 9u]; extern void Cyc_Position_reset_position(
struct _tagged_string); extern void Cyc_Position_set_position_file( struct
_tagged_string); extern struct _tagged_string Cyc_Position_get_position_file();
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
struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[ 14u];
extern int Cyc_Position_print_context; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); extern int Cyc_Position_error_p(); extern
unsigned int Cyc_String_strlen( struct _tagged_string s); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern struct
_tagged_string Cyc_String_strncpy( struct _tagged_string, int, struct
_tagged_string, int, unsigned int); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); unsigned char Cyc_Position_Exit[
9u]="\000\000\000\000Exit"; static unsigned char _temp2[ 1u]=""; static struct
_tagged_string Cyc_Position_source={ _temp2, _temp2, _temp2 + 1u}; struct Cyc_Position_Segment{
int start; int end; } ; struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int start, int end){ return({ struct Cyc_Position_Segment* _temp3=( struct Cyc_Position_Segment*)
GC_malloc( sizeof( struct Cyc_Position_Segment) * 1); _temp3[ 0]=({ struct Cyc_Position_Segment
_temp4; _temp4.start= start; _temp4.end= end; _temp4;}); _temp3;});} struct Cyc_Position_Segment*
Cyc_Position_segment_join( struct Cyc_Position_Segment* s1, struct Cyc_Position_Segment*
s2){ if( s1 == 0){ return s2;} if( s2 == 0){ return s1;} return({ struct Cyc_Position_Segment*
_temp5=( struct Cyc_Position_Segment*) GC_malloc( sizeof( struct Cyc_Position_Segment)
* 1); _temp5[ 0]=({ struct Cyc_Position_Segment _temp6; _temp6.start=(( struct
Cyc_Position_Segment*) _check_null( s1))->start; _temp6.end=(( struct Cyc_Position_Segment*)
_check_null( s2))->end; _temp6;}); _temp5;});} struct _tagged_string Cyc_Position_string_of_loc(
int loc){ struct Cyc_Lineno_Pos* pos= Cyc_Lineno_pos_of_abs( Cyc_Position_source,
loc); return({ struct _tagged_string _temp7= pos->logical_file; int _temp8= pos->line_no;
int _temp9= pos->col; xprintf("%.*s (%d:%d)", _temp7.last_plus_one - _temp7.curr,
_temp7.curr, _temp8, _temp9);});} static struct _tagged_string Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos* pos_s, struct Cyc_Lineno_Pos* pos_e){ if( Cyc_String_strcmp(
pos_s->logical_file, pos_e->logical_file) == 0){ return({ struct _tagged_string
_temp10= pos_s->logical_file; int _temp11= pos_s->line_no; int _temp12= pos_s->col;
int _temp13= pos_e->line_no; int _temp14= pos_e->col; xprintf("%.*s(%d:%d-%d:%d)",
_temp10.last_plus_one - _temp10.curr, _temp10.curr, _temp11, _temp12, _temp13,
_temp14);});} else{ return({ struct _tagged_string _temp15= pos_s->logical_file;
int _temp16= pos_s->line_no; int _temp17= pos_s->col; struct _tagged_string
_temp18= pos_e->logical_file; int _temp19= pos_e->line_no; int _temp20= pos_e->col;
xprintf("%.*s(%d:%d)-%.*s(%d:%d)", _temp15.last_plus_one - _temp15.curr, _temp15.curr,
_temp16, _temp17, _temp18.last_plus_one - _temp18.curr, _temp18.curr, _temp19,
_temp20);});}} struct _tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*
s){ if( s == 0){ return({ struct _tagged_string _temp21= Cyc_Position_source;
xprintf("%.*s(unknown)", _temp21.last_plus_one - _temp21.curr, _temp21.curr);});}{
struct Cyc_Lineno_Pos* pos_s= Cyc_Lineno_pos_of_abs( Cyc_Position_source,((
struct Cyc_Position_Segment*) _check_null( s))->start); struct Cyc_Lineno_Pos*
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,(( struct Cyc_Position_Segment*)
_check_null( s))->end); return Cyc_Position_string_of_pos_pr( pos_s, pos_e);}}
static struct Cyc_Lineno_Pos* Cyc_Position_new_pos(){ return({ struct Cyc_Lineno_Pos*
_temp22=( struct Cyc_Lineno_Pos*) GC_malloc( sizeof( struct Cyc_Lineno_Pos) * 1);
_temp22[ 0]=({ struct Cyc_Lineno_Pos _temp23; _temp23.logical_file=({
unsigned char* _temp26=""; struct _tagged_string _temp27; _temp27.curr= _temp26;
_temp27.base= _temp26; _temp27.last_plus_one= _temp26 + 1; _temp27;}); _temp23.line=({
unsigned char* _temp24=""; struct _tagged_string _temp25; _temp25.curr= _temp24;
_temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 1; _temp25;}); _temp23.line_no=
0; _temp23.col= 0; _temp23;}); _temp22;});} struct _tuple0{ int f1; struct Cyc_Lineno_Pos*
f2; } ; struct Cyc_List_List* Cyc_Position_strings_of_segments( struct Cyc_List_List*
segs){ struct Cyc_List_List* places= 0;{ struct Cyc_List_List* _temp28= segs;
goto _LL29; _LL29: for( 0; _temp28 != 0; _temp28=(( struct Cyc_List_List*)
_check_null( _temp28))->tl){ if(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*)
_check_null( _temp28))->hd == 0){ continue;} places=({ struct Cyc_List_List*
_temp30=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp30->hd=( void*)({ struct _tuple0* _temp33=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp33->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp28))->hd))->end; _temp33->f2= Cyc_Position_new_pos(); _temp33;}); _temp30->tl=({
struct Cyc_List_List* _temp31=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp31->hd=( void*)({ struct _tuple0* _temp32=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp32->f1=(( struct Cyc_Position_Segment*)
_check_null(( struct Cyc_Position_Segment*)(( struct Cyc_List_List*) _check_null(
_temp28))->hd))->start; _temp32->f2= Cyc_Position_new_pos(); _temp32;}); _temp31->tl=
places; _temp31;}); _temp30;});}} Cyc_Lineno_poss_of_abss( Cyc_Position_source,
places);{ struct Cyc_List_List* ans= 0; places=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( places); for( 0; segs != 0; segs=((
struct Cyc_List_List*) _check_null( segs))->tl){ if(( struct Cyc_Position_Segment*)((
struct Cyc_List_List*) _check_null( segs))->hd == 0){ ans=({ struct Cyc_List_List*
_temp34=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp34->hd=( void*)({ struct _tagged_string* _temp35=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp35[ 0]=({ struct
_tagged_string _temp36= Cyc_Position_source; xprintf("%.*s(unknown)", _temp36.last_plus_one
- _temp36.curr, _temp36.curr);}); _temp35;}); _temp34->tl= ans; _temp34;});}
else{ ans=({ struct Cyc_List_List* _temp37=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp37->hd=( void*)({ struct _tagged_string*
_temp38=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp38[ 0]= Cyc_Position_string_of_pos_pr((*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( places))->hd)).f2,(*(( struct _tuple0*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( places))->tl))->hd)).f2);
_temp38;}); _temp37->tl= ans; _temp37;}); places=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( places))->tl))->tl;}} return
ans;}} struct Cyc_Position_Error; struct Cyc_Position_Error* Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment* l, struct _tagged_string desc){ return({ struct Cyc_Position_Error*
_temp39=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp39->source= Cyc_Position_source; _temp39->seg= l; _temp39->kind=( void*)((
void*) Cyc_Position_Lex); _temp39->desc= desc; _temp39;});} struct Cyc_Position_Error*
Cyc_Position_mk_err_parse( struct Cyc_Position_Segment* l, struct _tagged_string
desc){ return({ struct Cyc_Position_Error* _temp40=( struct Cyc_Position_Error*)
GC_malloc( sizeof( struct Cyc_Position_Error)); _temp40->source= Cyc_Position_source;
_temp40->seg= l; _temp40->kind=( void*)(( void*) Cyc_Position_Parse); _temp40->desc=
desc; _temp40;});} struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct
Cyc_Position_Segment* l, struct _tagged_string desc){ return({ struct Cyc_Position_Error*
_temp41=( struct Cyc_Position_Error*) GC_malloc( sizeof( struct Cyc_Position_Error));
_temp41->source= Cyc_Position_source; _temp41->seg= l; _temp41->kind=( void*)((
void*) Cyc_Position_Elab); _temp41->desc= desc; _temp41;});} unsigned char Cyc_Position_Nocontext[
14u]="\000\000\000\000Nocontext"; static struct _tagged_string Cyc_Position_trunc(
int n, struct _tagged_string s){ int len=( int) Cyc_String_strlen( s); if( len <
n){ return s;}{ int len_one=( n - 3) / 2; int len_two=( n - 3) - len_one; struct
_tagged_string ans= Cyc_Core_new_string( n + 1); Cyc_String_strncpy( ans, 0, s,
0,( unsigned int) len_one); Cyc_String_strncpy( ans, len_one,({ unsigned char*
_temp42="..."; struct _tagged_string _temp43; _temp43.curr= _temp42; _temp43.base=
_temp42; _temp43.last_plus_one= _temp42 + 4; _temp43;}), 0, 3); Cyc_String_strncpy(
ans, len_one + 3, s, len - len_two,( unsigned int) len_two); return ans;}}
static int Cyc_Position_line_length= 76; struct _tuple1{ struct _tagged_string
f1; int f2; int f3; } ; static struct _tuple1* Cyc_Position_get_context( struct
Cyc_Position_Segment* seg){ if( seg == 0){( void) _throw(( void*) Cyc_Position_Nocontext);}{
struct Cyc_Lineno_Pos* pos_s; struct Cyc_Lineno_Pos* pos_e;{ struct
_handler_cons _temp44; _push_handler(& _temp44);{ int _temp46= 0; if( setjmp(
_temp44.handler)){ _temp46= 1;} if( ! _temp46){ pos_s= Cyc_Lineno_pos_of_abs(
Cyc_Position_source,(( struct Cyc_Position_Segment*) _check_null( seg))->start);
pos_e= Cyc_Lineno_pos_of_abs( Cyc_Position_source,(( struct Cyc_Position_Segment*)
_check_null( seg))->end);; _pop_handler();} else{ void* _temp45=( void*)
_exn_thrown; void* _temp48= _temp45; _LL50: goto _LL51; _LL52: goto _LL53; _LL51:(
void) _throw(( void*) Cyc_Position_Nocontext); _LL53:( void) _throw( _temp48);
_LL49:;}}}{ struct Cyc_Lineno_Pos _temp56; int _temp57; int _temp59; struct
_tagged_string _temp61; struct _tagged_string _temp63; struct Cyc_Lineno_Pos*
_temp54= pos_s; _temp56=* _temp54; _LL64: _temp63= _temp56.logical_file; goto
_LL62; _LL62: _temp61= _temp56.line; goto _LL60; _LL60: _temp59= _temp56.line_no;
goto _LL58; _LL58: _temp57= _temp56.col; goto _LL55; _LL55: { struct Cyc_Lineno_Pos
_temp67; int _temp68; int _temp70; struct _tagged_string _temp72; struct
_tagged_string _temp74; struct Cyc_Lineno_Pos* _temp65= pos_e; _temp67=* _temp65;
_LL75: _temp74= _temp67.logical_file; goto _LL73; _LL73: _temp72= _temp67.line;
goto _LL71; _LL71: _temp70= _temp67.line_no; goto _LL69; _LL69: _temp68= _temp67.col;
goto _LL66; _LL66: if( _temp59 == _temp70){ int n= Cyc_Position_line_length / 3;
struct _tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring(
_temp61, 0,( unsigned int) _temp57)); struct _tagged_string sec_two= Cyc_Position_trunc(
n, Cyc_String_substring( _temp61, _temp57,( unsigned int)( _temp68 - _temp57)));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp61, _temp57, Cyc_String_strlen( _temp61) - _temp68)); return({ struct
_tuple1* _temp76=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp76->f1=({
struct _tagged_string _temp77= sec_one; struct _tagged_string _temp78= sec_two;
struct _tagged_string _temp79= sec_three; xprintf("%.*s%.*s%.*s", _temp77.last_plus_one
- _temp77.curr, _temp77.curr, _temp78.last_plus_one - _temp78.curr, _temp78.curr,
_temp79.last_plus_one - _temp79.curr, _temp79.curr);}); _temp76->f2=( int) Cyc_String_strlen(
sec_one); _temp76->f3=( int)( Cyc_String_strlen( sec_one) + Cyc_String_strlen(
sec_two)); _temp76;});} else{ int n=( Cyc_Position_line_length - 3) / 4; struct
_tagged_string sec_one= Cyc_Position_trunc( n, Cyc_String_substring( _temp61, 0,(
unsigned int) _temp57)); struct _tagged_string sec_two= Cyc_Position_trunc( n,
Cyc_String_substring( _temp61, _temp57, Cyc_String_strlen( _temp61) - _temp57));
struct _tagged_string sec_three= Cyc_Position_trunc( n, Cyc_String_substring(
_temp72, 0,( unsigned int) _temp68)); struct _tagged_string sec_four= Cyc_Position_trunc(
n, Cyc_String_substring( _temp72, _temp68, Cyc_String_strlen( _temp72) - _temp68));
return({ struct _tuple1* _temp80=( struct _tuple1*) GC_malloc( sizeof( struct
_tuple1)); _temp80->f1=({ struct _tagged_string _temp81= sec_one; struct
_tagged_string _temp82= sec_two; struct _tagged_string _temp83= sec_three;
struct _tagged_string _temp84= sec_four; xprintf("%.*s%.*s.\\.%.*s%.*s", _temp81.last_plus_one
- _temp81.curr, _temp81.curr, _temp82.last_plus_one - _temp82.curr, _temp82.curr,
_temp83.last_plus_one - _temp83.curr, _temp83.curr, _temp84.last_plus_one -
_temp84.curr, _temp84.curr);}); _temp80->f2=( int) Cyc_String_strlen( sec_one);
_temp80->f3=( int)((( Cyc_String_strlen( sec_one) + Cyc_String_strlen( sec_two))
+ 3) + Cyc_String_strlen( sec_three)); _temp80;});}}}}} static int Cyc_Position_error_b=
0; int Cyc_Position_error_p(){ return Cyc_Position_error_b;} unsigned char Cyc_Position_Error[
10u]="\000\000\000\000Error"; struct Cyc_Position_Error_struct{ unsigned char*
tag; struct Cyc_Position_Error* f1; } ; int Cyc_Position_print_context= 0; int
Cyc_Position_first_error= 1; void Cyc_Position_post_error( struct Cyc_Position_Error*
e){ Cyc_Position_error_b= 1; Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*) Cyc_Stdio_stdout);
if( Cyc_Position_first_error){ fprintf( sfile_to_file( Cyc_Stdio_stderr),"\n");
Cyc_Position_first_error= 0;}({ struct _tagged_string _temp85= Cyc_Position_string_of_segment(
e->seg); struct _tagged_string _temp86= e->desc; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: %.*s\n",
_temp85.last_plus_one - _temp85.curr, _temp85.curr, _temp86.last_plus_one -
_temp86.curr, _temp86.curr);}); if( Cyc_Position_print_context){ struct
_handler_cons _temp87; _push_handler(& _temp87);{ int _temp89= 0; if( setjmp(
_temp87.handler)){ _temp89= 1;} if( ! _temp89){{ struct _tuple1* x= Cyc_Position_get_context(
e->seg); struct _tagged_string marker_str=({ unsigned int _temp92=( unsigned int)((*
x).f3 + 1); unsigned char* _temp93=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char) * _temp92); struct _tagged_string _temp95={ _temp93, _temp93,
_temp93 + _temp92};{ unsigned int _temp94= _temp92; unsigned int i; for( i= 0; i
< _temp94; i ++){ _temp93[ i]='\000';}}; _temp95;}); int i= - 1; while( ++ i <(*
x).f2) {*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( marker_str, sizeof( unsigned char), i)=' ';} while(
++ i <(* x).f3) {*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( marker_str, sizeof( unsigned char), i)='^';}({
struct _tagged_string _temp90=(* x).f1; struct _tagged_string _temp91=
marker_str; fprintf( sfile_to_file( Cyc_Stdio_stderr),"  %.*s\n  %.*s\n",
_temp90.last_plus_one - _temp90.curr, _temp90.curr, _temp91.last_plus_one -
_temp91.curr, _temp91.curr);});}; _pop_handler();} else{ void* _temp88=( void*)
_exn_thrown; void* _temp97= _temp88; _LL99: if( _temp97 == Cyc_Position_Nocontext){
goto _LL100;} else{ goto _LL101;} _LL101: goto _LL102; _LL100: goto _LL98;
_LL102:( void) _throw( _temp97); _LL98:;}}} Cyc_Stdio_fflush(( struct Cyc_Stdio___sFILE*)
Cyc_Stdio_stderr);} void Cyc_Position_reset_position( struct _tagged_string s){
Cyc_Position_source= s; Cyc_Position_error_b= 0;} void Cyc_Position_set_position_file(
struct _tagged_string s){ Cyc_Position_source= s; Cyc_Position_error_b= 0;}
struct _tagged_string Cyc_Position_get_position_file(){ return Cyc_Position_source;}