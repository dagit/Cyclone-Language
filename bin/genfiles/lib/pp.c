#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
void* Cyc_Core_identity( void*); extern int Cyc_Core_intcmp( int, int); extern
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
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int
Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern unsigned int Cyc_String_strlen( struct _tagged_string
s); extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); struct Cyc_Hashtable_Table; typedef struct Cyc_Hashtable_Table*
Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table* Cyc_Hashtable_create(
int sz, int(* cmp)( void*, void*), int(* hash)( void*)); extern void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table* t, void* key, void* val); extern void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key); extern int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s); static const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{
int tag; void*(* f1)( void*, void*); void* f2; } ; typedef void* Cyc_Fn_fn;
extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*), void* x); extern void*
Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply( void* f, void* x);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc*
d, int w, struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); struct _tuple0{ struct _tagged_string f1; struct
Cyc_List_List* f2; } ; extern struct _tuple0* Cyc_PP_string_and_links( struct
Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct
Cyc_PP_Doc* Cyc_PP_blank_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_oline_doc(); extern struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_string* s); extern struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct
_tagged_string shrt, struct _tagged_string full); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); struct _tagged_ptr0{ struct Cyc_PP_Doc** curr;
struct Cyc_PP_Doc** base; struct Cyc_PP_Doc** last_plus_one; } ; extern struct
Cyc_PP_Doc* Cyc_PP_cat( struct _tagged_ptr0); extern struct Cyc_PP_Doc* Cyc_PP_cats(
struct Cyc_List_List* doclist); extern struct Cyc_PP_Doc* Cyc_PP_cats_arr(
struct _tagged_ptr0 docs); extern struct Cyc_PP_Doc* Cyc_PP_doc_union( struct
Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2); extern struct Cyc_PP_Doc* Cyc_PP_tab(
struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0);
extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_group( struct _tagged_string start, struct _tagged_string
stop, struct _tagged_string sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc*
Cyc_PP_groupl( struct _tagged_string start, struct _tagged_string stop, struct
_tagged_string sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc* Cyc_PP_egroup(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss); struct _tagged_ptr1{ void** curr; void** base;
void** last_plus_one; } ; struct Cyc_Xarray_Xarray{ struct _tagged_ptr1 elmts;
int num_elmts; } ; typedef struct Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern
int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*); extern void* Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*,
int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*);
extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*); static const int
Cyc_PP_Empty= 0; static const int Cyc_PP_Single= 0; struct Cyc_PP_Single_struct{
int tag; void* f1; } ; static const int Cyc_PP_Append= 1; struct Cyc_PP_Append_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_PP_alist_t; void* Cyc_PP_append(
void* a1, void* a2){ if( a1 ==( void*) Cyc_PP_Empty){ return a2;} else{ if( a2
==( void*) Cyc_PP_Empty){ return a1;} else{ return( void*)({ struct Cyc_PP_Append_struct*
_temp0=( struct Cyc_PP_Append_struct*) GC_malloc( sizeof( struct Cyc_PP_Append_struct));
_temp0[ 0]=({ struct Cyc_PP_Append_struct _temp1; _temp1.tag= Cyc_PP_Append;
_temp1.f1=( void*) a1; _temp1.f2=( void*) a2; _temp1;}); _temp0;});}}} struct
Cyc_List_List* Cyc_PP_list_of_alist_f( void* y, struct Cyc_List_List* l){ void*
_temp2= y; void* _temp10; void* _temp12; void* _temp14; _LL4: if( _temp2 ==(
void*) Cyc_PP_Empty){ goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int)
_temp2 > 1u?*(( int*) _temp2) == Cyc_PP_Single: 0){ _LL11: _temp10=( void*)((
struct Cyc_PP_Single_struct*) _temp2)->f1; goto _LL7;} else{ goto _LL8;} _LL8:
if(( unsigned int) _temp2 > 1u?*(( int*) _temp2) == Cyc_PP_Append: 0){ _LL15:
_temp14=( void*)(( struct Cyc_PP_Append_struct*) _temp2)->f1; goto _LL13; _LL13:
_temp12=( void*)(( struct Cyc_PP_Append_struct*) _temp2)->f2; goto _LL9;} else{
goto _LL3;} _LL5: return l; _LL7: return({ struct Cyc_List_List* _temp16=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp16->hd=(
void*) _temp10; _temp16->tl= l; _temp16;}); _LL9: return(( struct Cyc_List_List*(*)(
void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)( _temp14,(( struct
Cyc_List_List*(*)( void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)(
_temp12, l)); _LL3:;} struct Cyc_List_List* Cyc_PP_list_of_alist( void* x){
return(( struct Cyc_List_List*(*)( void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)(
x, 0);} struct Cyc_PP_Ppstate{ int ci; int cc; int cl; int pw; int epw; } ;
struct Cyc_PP_Out{ int newcc; int newcl; void* ppout; void* links; } ; struct
Cyc_PP_Doc{ int mwo; int mw; void* f; } ; static void Cyc_PP_dump_out( struct
Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray* _temp17=(( struct Cyc_Xarray_Xarray*(*)(
int, void*)) Cyc_Xarray_create)( 16, al); goto _LL18; _LL18:(( void(*)( struct
Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( _temp17, al);{ int last= 0; while(
last >= 0) { void* _temp19=(( void*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)(
_temp17, last); struct _tagged_string* _temp27; void* _temp29; void* _temp31;
_LL21: if( _temp19 ==( void*) Cyc_PP_Empty){ goto _LL22;} else{ goto _LL23;}
_LL23: if(( unsigned int) _temp19 > 1u?*(( int*) _temp19) == Cyc_PP_Single: 0){
_LL28: _temp27=( struct _tagged_string*)(( struct Cyc_PP_Single_struct*) _temp19)->f1;
goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp19 > 1u?*(( int*)
_temp19) == Cyc_PP_Append: 0){ _LL32: _temp31=( void*)(( struct Cyc_PP_Append_struct*)
_temp19)->f1; goto _LL30; _LL30: _temp29=( void*)(( struct Cyc_PP_Append_struct*)
_temp19)->f2; goto _LL26;} else{ goto _LL20;} _LL22: -- last; goto _LL20; _LL24:
-- last;({ struct _tagged_string _temp33=* _temp27; fprintf( sfile_to_file( f),"%.*s",
_temp33.last_plus_one - _temp33.curr, _temp33.curr);}); goto _LL20; _LL26:((
void(*)( struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)( _temp17, last,
_temp29); if( last ==(( int(*)( struct Cyc_Xarray_Xarray*)) Cyc_Xarray_length)(
_temp17) - 1){(( void(*)( struct Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)(
_temp17, _temp31);} else{(( void(*)( struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)(
_temp17, last + 1, _temp31);} ++ last; goto _LL20; _LL20:;}}} void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE* f){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp34=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp34->ci= 0; _temp34->cc=
0; _temp34->cl= 1; _temp34->pw= w; _temp34->epw= w; _temp34;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp37=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp37->ci= 0; _temp37->cc=
0; _temp37->cl= 1; _temp37->pw= w; _temp37->epw= w; _temp37;})); return Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*) o->ppout),({
unsigned char* _temp35=""; struct _tagged_string _temp36; _temp36.curr= _temp35;
_temp36.base= _temp35; _temp36.last_plus_one= _temp35 + 1; _temp36;}));} struct
_tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out*
o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f,({ struct Cyc_PP_Ppstate* _temp41=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp41->ci= 0; _temp41->cc=
0; _temp41->cl= 1; _temp41->pw= w; _temp41->epw= w; _temp41;})); return({ struct
_tuple0* _temp38=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp38->f1=
Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout),({ unsigned char* _temp39=""; struct _tagged_string _temp40;
_temp40.curr= _temp39; _temp40.base= _temp39; _temp40.last_plus_one= _temp39 + 1;
_temp40;})); _temp38->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->links); _temp38;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0;
struct _tagged_string Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp42=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp42->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp42;});} if( i < 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp43=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp43[ 0]=({ struct Cyc_Core_Failure_struct
_temp44; _temp44.tag= Cyc_Core_Failure; _temp44.f1=({ unsigned char* _temp45="nlblanks";
struct _tagged_string _temp46; _temp46.curr= _temp45; _temp46.base= _temp45;
_temp46.last_plus_one= _temp45 + 9; _temp46;}); _temp44;}); _temp43;}));}{
struct _handler_cons _temp47; _push_handler(& _temp47);{ int _temp49= 0; if(
setjmp( _temp47.handler)){ _temp49= 1;} if( ! _temp49){{ struct _tagged_string
_temp50=*(( struct _tagged_string*(*)( struct Cyc_Hashtable_Table* t, int key))
Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_PP_bhashtbl))->v, i); _npop_handler( 0u); return _temp50;};
_pop_handler();} else{ void* _temp48=( void*) _exn_thrown; void* _temp52=
_temp48; _LL54: if( _temp52 == Cyc_Core_Not_found){ goto _LL55;} else{ goto
_LL56;} _LL56: goto _LL57; _LL55: { int _temp58= i / 8; goto _LL59; _LL59: { int
_temp60= i % 8; goto _LL61; _LL61: { int _temp62=( 1 + _temp58) + _temp60; goto
_LL63; _LL63: { struct _tagged_string nlb= Cyc_Core_new_string( _temp62);*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( nlb, sizeof( unsigned char), 0)='\n';{ int j= 0; for(
0; j < _temp58; j ++){*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( nlb, sizeof( unsigned char), j + 1)='\t';}}{
int j= 0; for( 0; j < _temp60; j ++){*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( nlb,
sizeof( unsigned char),( j + 1) + _temp58)=' ';}}(( void(*)( struct Cyc_Hashtable_Table*
t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)(( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v, i,({ struct
_tagged_string* _temp64=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp64[ 0]= nlb; _temp64;})); return nlb;}}}} _LL57:(
void) _throw( _temp52); _LL53:;}}}} static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl=
0; int Cyc_PP_infinity= 9999999; struct _tuple1{ int f1; struct _tagged_string
f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f( struct _tuple1* clo, struct
Cyc_PP_Ppstate* st){ struct _tuple1 _temp67; struct _tagged_string _temp68; int
_temp70; struct _tuple1* _temp65= clo; _temp67=* _temp65; _LL71: _temp70=
_temp67.f1; goto _LL69; _LL69: _temp68= _temp67.f2; goto _LL66; _LL66: return({
struct Cyc_PP_Out* _temp72=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp72->newcc= st->cc + _temp70; _temp72->newcl= st->cl; _temp72->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp73=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp73[ 0]=({ struct Cyc_PP_Single_struct
_temp74; _temp74.tag= Cyc_PP_Single; _temp74.f1=( void*)({ struct _tagged_string*
_temp75=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp75[ 0]= _temp68; _temp75;}); _temp74;}); _temp73;})); _temp72->links=( void*)((
void*) Cyc_PP_Empty); _temp72;});} static struct Cyc_PP_Doc* Cyc_PP_text_doc(
struct _tagged_string s){ int slen=( int) Cyc_String_strlen( s); return({ struct
Cyc_PP_Doc* _temp76=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp76->mwo= slen; _temp76->mw= Cyc_PP_infinity; _temp76->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*), struct
_tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1* _temp77=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp77->f1= slen; _temp77->f2=
s; _temp77;})); _temp76;});} struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl == 0){
t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_string*,
struct _tagged_string*), int(* hash)( struct _tagged_string*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp78=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp78->v=( void*) t; _temp78;});} else{ t=( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_str_hashtbl))->v;}{ struct
_handler_cons _temp79; _push_handler(& _temp79);{ int _temp81= 0; if( setjmp(
_temp79.handler)){ _temp81= 1;} if( ! _temp81){{ struct Cyc_PP_Doc* _temp83=((
struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct _tagged_string* key))
Cyc_Hashtable_lookup)( t,({ struct _tagged_string* _temp82=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp82[ 0]= s;
_temp82;})); _npop_handler( 0u); return _temp83;}; _pop_handler();} else{ void*
_temp80=( void*) _exn_thrown; void* _temp85= _temp80; _LL87: if( _temp85 == Cyc_Core_Not_found){
goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL88: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_string* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_string* _temp91=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp91[ 0]= s; _temp91;}), d); return d;} _LL90:( void)
_throw( _temp85); _LL86:;}}}} struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_string* s){ return Cyc_PP_text(* s);} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt=
0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt* _temp92=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp92->v=( void*) Cyc_PP_text(({ unsigned char* _temp93="";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 1; _temp94;})); _temp92;});} return( struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_nil_doc_opt))->v;} struct Cyc_PP_Doc*
Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({
struct Cyc_Core_Opt* _temp95=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp95->v=( void*) Cyc_PP_text(({ unsigned char* _temp96="";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 1; _temp97;})); _temp95;});} return( struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_blank_doc_opt))->v;} struct _tuple2{
int f1; struct _tagged_string f2; struct _tagged_string f3; } ; struct _tuple3{
int f1; int f2; int f3; struct _tagged_string f4; } ; static struct Cyc_PP_Out*
Cyc_PP_hyperlink_f( struct _tuple2* clo, struct Cyc_PP_Ppstate* st){ struct
_tuple2 _temp100; struct _tagged_string _temp101; struct _tagged_string _temp103;
int _temp105; struct _tuple2* _temp98= clo; _temp100=* _temp98; _LL106: _temp105=
_temp100.f1; goto _LL104; _LL104: _temp103= _temp100.f2; goto _LL102; _LL102:
_temp101= _temp100.f3; goto _LL99; _LL99: return({ struct Cyc_PP_Out* _temp107=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp107->newcc= st->cc
+ _temp105; _temp107->newcl= st->cl; _temp107->ppout=( void*)(( void*)({ struct
Cyc_PP_Single_struct* _temp111=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp111[ 0]=({ struct Cyc_PP_Single_struct
_temp112; _temp112.tag= Cyc_PP_Single; _temp112.f1=( void*)({ struct
_tagged_string* _temp113=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp113[ 0]= _temp103; _temp113;}); _temp112;}); _temp111;}));
_temp107->links=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp108=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp108[ 0]=({ struct Cyc_PP_Single_struct _temp109; _temp109.tag= Cyc_PP_Single;
_temp109.f1=( void*)({ struct _tuple3* _temp110=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp110->f1= st->cl; _temp110->f2= st->cc; _temp110->f3=
_temp105; _temp110->f4= _temp101; _temp110;}); _temp109;}); _temp108;}));
_temp107;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt,
struct _tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({
struct Cyc_PP_Doc* _temp114=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp114->mwo= slen; _temp114->mw= Cyc_PP_infinity; _temp114->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp115=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp115->f1= slen;
_temp115->f2= shrt; _temp115->f3= full; _temp115;})); _temp114;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp116=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp116->newcc= st->ci; _temp116->newcl= st->cl + 1; _temp116->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp117=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp117[ 0]=({ struct Cyc_PP_Single_struct
_temp118; _temp118.tag= Cyc_PP_Single; _temp118.f1=( void*)({ struct
_tagged_string* _temp119=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp119[ 0]= Cyc_PP_nlblanks( st->ci); _temp119;});
_temp118;}); _temp117;})); _temp116->links=( void*)(( void*) Cyc_PP_Empty);
_temp116;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp120=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp120->v=( void*)({ struct Cyc_PP_Doc*
_temp121=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp121->mwo=
0; _temp121->mw= 0; _temp121->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp121;}); _temp120;});}
return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_line_doc_opt))->v;}
struct _tuple4{ int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp124; struct
Cyc_PP_Doc* _temp125; int _temp127; struct _tuple4* _temp122= clo; _temp124=*
_temp122; _LL128: _temp127= _temp124.f1; goto _LL126; _LL126: _temp125= _temp124.f2;
goto _LL123; _LL123: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp129=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp129->ci= st->ci + _temp127; _temp129->cc= st->cc; _temp129->cl= st->cl;
_temp129->pw= st->pw; _temp129->epw= st->epw; _temp129;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp125->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp130=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp130->mwo= d->mwo; _temp130->mw= d->mw; _temp130->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp131=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp131->f1= k; _temp131->f2= d;
_temp131;})); _temp130;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp134; struct Cyc_PP_Doc* _temp135; struct Cyc_PP_Doc*
_temp137; struct _tuple5* _temp132= clo; _temp134=* _temp132; _LL138: _temp137=
_temp134.f1; goto _LL136; _LL136: _temp135= _temp134.f2; goto _LL133; _LL133: {
int epw2= Cyc_PP_max( st->pw - _temp135->mw, st->epw - _temp137->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp141=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp141->ci= st->ci;
_temp141->cc= st->cc; _temp141->cl= st->cl; _temp141->pw= st->pw; _temp141->epw=
epw2; _temp141;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp137->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp140=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp140->ci= st->ci;
_temp140->cc= o1->newcc; _temp140->cl= o1->newcl; _temp140->pw= st->pw; _temp140->epw=
epw2; _temp140;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp135->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp139=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp139->newcc= o2->newcc; _temp139->newcl= o2->newcl;
_temp139->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp139->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp139;});
return o3;}} static struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp142=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp142->mwo= Cyc_PP_min( d1->mw, d1->mwo
+ d2->mwo); _temp142->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp142->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*),
struct _tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp143=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp143->f1= d1; _temp143->f2=
d2; _temp143;})); _temp142;});} struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_ptr0 l){ struct Cyc_PP_Doc* _temp144= Cyc_PP_nil_doc(); goto _LL145;
_LL145:{ int i=( int)(({ struct _tagged_ptr0 _temp146= l;( unsigned int)(
_temp146.last_plus_one - _temp146.curr);}) - 1); for( 0; i >= 0; i --){ _temp144=
Cyc_PP_concat(*(( struct Cyc_PP_Doc**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( l, sizeof( struct Cyc_PP_Doc*), i),
_temp144);}} return _temp144;} static struct Cyc_PP_Out* Cyc_PP_long_cats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* _temp147= ds0; goto _LL148; _LL148: for( 0; _temp147
!= 0; _temp147=(( struct Cyc_List_List*) _check_null( _temp147))->tl){ struct
Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(
_temp147))->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct
Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate*
_temp149=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp149->ci= st->ci; _temp149->cc= o->newcc; _temp149->cl= o->newcl; _temp149->pw=
st->pw; _temp149->epw= st->epw - d->mwo; _temp149;}); os=({ struct Cyc_List_List*
_temp150=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp150->hd=( void*) o; _temp150->tl= os; _temp150;});}}{ int newcc=(( struct
Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcc; int newcl=((
struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcl; void*
s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=((
struct Cyc_List_List*) _check_null( os))->tl){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->links,
links);} return({ struct Cyc_PP_Out* _temp151=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp151->newcc= newcc; _temp151->newcl= newcl;
_temp151->ppout=( void*) s; _temp151->links=( void*) links; _temp151;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_cats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( doclist))->hd; doclist=(( struct Cyc_List_List*)
_check_null( doclist))->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List*
_temp152= doclist; goto _LL153; _LL153: for( 0; _temp152 != 0; _temp152=((
struct Cyc_List_List*) _check_null( _temp152))->tl){ int mw2= Cyc_PP_min( mw,
mwo +(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( _temp152))->hd)->mwo);
int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( _temp152))->hd)->mw); mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc*
_temp154=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp154->mwo=
mw; _temp154->mw= mwo; _temp154->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_List_List*, struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)(
Cyc_PP_long_cats_f, orig); _temp154;});}} struct Cyc_PP_Doc* Cyc_PP_cats( struct
Cyc_List_List* doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();} else{ if(((
struct Cyc_List_List*) _check_null( doclist))->tl == 0){ return( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( doclist))->hd;} else{ if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( doclist) > 30){ return Cyc_PP_long_cats(
doclist);} else{ return Cyc_PP_concat(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( doclist))->hd, Cyc_PP_cats((( struct Cyc_List_List*) _check_null(
doclist))->tl));}}}} static struct Cyc_PP_Out* Cyc_PP_cats_arr_f( struct
_tagged_ptr0* docs_ptr, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0;
struct _tagged_ptr0 docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0
_temp158= docs;( unsigned int)( _temp158.last_plus_one - _temp158.curr);});{ int
i= 0; for( 0; i < sz; ++ i){ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
docs, sizeof( struct Cyc_PP_Doc*), i); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp155=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp155->ci= st->ci; _temp155->cc= o->newcc;
_temp155->cl= o->newcl; _temp155->pw= st->pw; _temp155->epw= st->epw - d->mwo;
_temp155;}); os=({ struct Cyc_List_List* _temp156=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp156->hd=( void*) o; _temp156->tl=
os; _temp156;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os != 0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp157=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp157->newcc=
newcc; _temp157->newcl= newcl; _temp157->ppout=( void*) s; _temp157->links=(
void*) links; _temp157;});}} struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct
_tagged_ptr0 docs){ int sz=( int)({ struct _tagged_ptr0 _temp165= docs;(
unsigned int)( _temp165.last_plus_one - _temp165.curr);}); if( sz == 0){( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp159=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp159[ 0]=({ struct Cyc_Core_Failure_struct
_temp160; _temp160.tag= Cyc_Core_Failure; _temp160.f1=({ unsigned char* _temp161="cats_arr -- size zero array";
struct _tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 28; _temp162;}); _temp160;}); _temp159;}));}{
struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( docs, sizeof( struct Cyc_PP_Doc*),
0); int mw= d->mw; int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2=
Cyc_PP_min( mw, mwo +(*(( struct Cyc_PP_Doc**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( docs, sizeof( struct Cyc_PP_Doc*),
i))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(*(( struct Cyc_PP_Doc**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( docs,
sizeof( struct Cyc_PP_Doc*), i))->mw); mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc*
_temp163=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp163->mwo=
mw; _temp163->mw= mwo; _temp163->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct _tagged_ptr0*, struct Cyc_PP_Ppstate*), struct _tagged_ptr0* x)) Cyc_Fn_make_fn)(
Cyc_PP_cats_arr_f,({ struct _tagged_ptr0* _temp164=( struct _tagged_ptr0*)
GC_malloc( sizeof( struct _tagged_ptr0) * 1); _temp164[ 0]= docs; _temp164;}));
_temp163;});}} static struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo,
struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp168; struct Cyc_PP_Doc* _temp169;
struct Cyc_PP_Doc* _temp171; struct _tuple5* _temp166= clo; _temp168=* _temp166;
_LL172: _temp171= _temp168.f1; goto _LL170; _LL170: _temp169= _temp168.f2; goto
_LL167; _LL167: { int dfits= st->cc + _temp171->mwo <= st->epw? 1: st->cc +
_temp171->mw <= st->pw; if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp171->f, st);} else{
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) _temp169->f, st);}}} struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc*
d, struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp173=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp173->mwo= Cyc_PP_min( d->mwo, d2->mwo);
_temp173->mw= Cyc_PP_min( d->mw, d2->mw); _temp173->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_doc_union_f,({ struct _tuple5* _temp174=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp174->f1= d; _temp174->f2=
d2; _temp174;})); _temp173;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return
Cyc_PP_doc_union( Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out*
Cyc_PP_tab_f( struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate*
st2=({ struct Cyc_PP_Ppstate* _temp175=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp175->ci= st->cc;
_temp175->cc= st->cc; _temp175->cl= st->cl; _temp175->pw= st->pw; _temp175->epw=
st->epw; _temp175;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct
Cyc_PP_Doc* d){ struct Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp176=( struct
Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp176->mwo= d->mwo;
_temp176->mw= d->mw; _temp176->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Doc*, struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)(
Cyc_PP_tab_f, d); _temp176;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl == 0){ return pp(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*(* _temp177)( struct
_tagged_ptr0 l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp180= pp(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd); struct Cyc_PP_Doc* _temp181= Cyc_PP_text(
sep); struct Cyc_PP_Doc* _temp182= Cyc_PP_oline_doc(); struct Cyc_PP_Doc*
_temp183=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep,(( struct
Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp178[ 4u]={
_temp180, _temp181, _temp182, _temp183}; struct _tagged_ptr0 _temp179={ _temp178,
_temp178, _temp178 + 4u}; _temp177( _temp179);});}}} struct Cyc_PP_Doc* Cyc_PP_ppseq(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep,
l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl == 0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( l))->hd;} else{ struct Cyc_PP_Doc* sep2= Cyc_PP_text( sep); struct
Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List* _temp184= l; goto
_LL185; _LL185: while((( struct Cyc_List_List*) _check_null( _temp184))->tl != 0) {
struct Cyc_List_List* _temp186=(( struct Cyc_List_List*) _check_null( _temp184))->tl;
goto _LL187; _LL187:(( struct Cyc_List_List*) _check_null( _temp184))->tl=({
struct Cyc_List_List* _temp188=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp188->hd=( void*) sep2; _temp188->tl=({ struct Cyc_List_List*
_temp189=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp189->hd=( void*) oline; _temp189->tl= _temp186; _temp189;}); _temp188;});
_temp184= _temp186;} return Cyc_PP_cats( l);}}} struct Cyc_PP_Doc* Cyc_PP_seq(
struct _tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seq_f(
sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return pp(( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*(*
_temp190)( struct _tagged_ptr0 l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp193= pp((
void*)(( struct Cyc_List_List*) _check_null( l))->hd); struct Cyc_PP_Doc*
_temp194= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp195= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp196=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)(
pp, sep,(( struct Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc*
_temp191[ 4u]={ _temp193, _temp194, _temp195, _temp196}; struct _tagged_ptr0
_temp192={ _temp191, _temp191, _temp191 + 4u}; _temp190( _temp192);});}}} struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql_f)( pp, sep, l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f(
struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;} else{ return({
struct Cyc_PP_Doc*(* _temp197)( struct _tagged_ptr0 l)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp200=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;
struct Cyc_PP_Doc* _temp201= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp202= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp203= Cyc_PP_seql_f( sep,(( struct Cyc_List_List*)
_check_null( l))->tl); struct Cyc_PP_Doc* _temp198[ 4u]={ _temp200, _temp201,
_temp202, _temp203}; struct _tagged_ptr0 _temp199={ _temp198, _temp198, _temp198
+ 4u}; _temp197( _temp199);});}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));} struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return({ struct Cyc_PP_Doc*(* _temp204)( struct _tagged_ptr0 l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp207= Cyc_PP_text( start); struct Cyc_PP_Doc* _temp208=
Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc* _temp209= Cyc_PP_text( stop); struct
Cyc_PP_Doc* _temp205[ 3u]={ _temp207, _temp208, _temp209}; struct _tagged_ptr0
_temp206={ _temp205, _temp205, _temp205 + 3u}; _temp204( _temp206);});} struct
Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_string start, struct _tagged_string
stop, struct _tagged_string sep, struct Cyc_List_List* ss){ if( ss == 0){ return
Cyc_PP_nil_doc();} else{ return({ struct Cyc_PP_Doc*(* _temp210)( struct
_tagged_ptr0 l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp213= Cyc_PP_text( start);
struct Cyc_PP_Doc* _temp214= Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc* _temp215=
Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp211[ 3u]={ _temp213, _temp214,
_temp215}; struct _tagged_ptr0 _temp212={ _temp211, _temp211, _temp211 + 3u};
_temp210( _temp212);});}} struct Cyc_PP_Doc* Cyc_PP_groupl( struct
_tagged_string start, struct _tagged_string stop, struct _tagged_string sep,
struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*(* _temp216)( struct
_tagged_ptr0 l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp219= Cyc_PP_text( start);
struct Cyc_PP_Doc* _temp220= Cyc_PP_seql( sep, ss); struct Cyc_PP_Doc* _temp221=
Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp217[ 3u]={ _temp219, _temp220,
_temp221}; struct _tagged_ptr0 _temp218={ _temp217, _temp217, _temp217 + 3u};
_temp216( _temp218);});}