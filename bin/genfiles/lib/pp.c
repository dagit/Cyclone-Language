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
_tagged_string Cyc_Core_new_string( int); extern void* Cyc_Core_identity( void*);
extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_string); struct Cyc_Hashtable_Table;
typedef struct Cyc_Hashtable_Table* Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_string* s); static
const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*,
void*); void* f2; } ; typedef void* Cyc_Fn_fn; extern void* Cyc_Fn_make_fn( void*(*
f)( void*, void*), void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*));
extern void* Cyc_Fn_apply( void* f, void* x); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int
w); struct _tuple0{ struct _tagged_string f1; struct Cyc_List_List* f2; } ;
extern struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w);
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s); extern struct
Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_blank_doc();
extern struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt, struct
_tagged_string full); extern struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct
Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc*
Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2); extern struct Cyc_PP_Doc*
Cyc_PP_concats( struct Cyc_List_List* doclist); struct _tagged_ptr0{ struct Cyc_PP_Doc**
curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc** last_plus_one; } ; extern
struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct _tagged_ptr0 docs); extern struct
Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2);
extern struct Cyc_PP_Doc* Cyc_PP_oline_doc(); extern struct Cyc_PP_Doc* Cyc_PP_tab(
struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0);
extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_string sep, struct Cyc_List_List* l0); struct
_tagged_ptr1{ void** curr; void** base; void** last_plus_one; } ; struct Cyc_Xarray_Xarray{
struct _tagged_ptr1 elmts; int num_elmts; } ; typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*, int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
static const int Cyc_PP_Empty= 0; static const int Cyc_PP_Single= 0; struct Cyc_PP_Single_struct{
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
Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray* xarr=(( struct Cyc_Xarray_Xarray*(*)(
int, void*)) Cyc_Xarray_create)( 16, al);(( void(*)( struct Cyc_Xarray_Xarray*,
void*)) Cyc_Xarray_add)( xarr, al);{ int last= 0; while( last >= 0) { void*
_temp17=(( void*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)( xarr,
last); struct _tagged_string* _temp25; void* _temp27; void* _temp29; _LL19: if(
_temp17 ==( void*) Cyc_PP_Empty){ goto _LL20;} else{ goto _LL21;} _LL21: if((
unsigned int) _temp17 > 1u?*(( int*) _temp17) == Cyc_PP_Single: 0){ _LL26:
_temp25=( struct _tagged_string*)(( struct Cyc_PP_Single_struct*) _temp17)->f1;
goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp17 > 1u?*(( int*)
_temp17) == Cyc_PP_Append: 0){ _LL30: _temp29=( void*)(( struct Cyc_PP_Append_struct*)
_temp17)->f1; goto _LL28; _LL28: _temp27=( void*)(( struct Cyc_PP_Append_struct*)
_temp17)->f2; goto _LL24;} else{ goto _LL18;} _LL20: -- last; goto _LL18; _LL22:
-- last;({ struct _tagged_string _temp31=* _temp25; fprintf( f,"%.*s", _temp31.last_plus_one
- _temp31.curr, _temp31.curr);}); goto _LL18; _LL24:(( void(*)( struct Cyc_Xarray_Xarray*,
int, void*)) Cyc_Xarray_set)( xarr, last, _temp27); if( last ==(( int(*)( struct
Cyc_Xarray_Xarray*)) Cyc_Xarray_length)( xarr) - 1){(( void(*)( struct Cyc_Xarray_Xarray*,
void*)) Cyc_Xarray_add)( xarr, _temp29);} else{(( void(*)( struct Cyc_Xarray_Xarray*,
int, void*)) Cyc_Xarray_set)( xarr, last + 1, _temp29);} ++ last; goto _LL18;
_LL18:;}}} void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp32=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp32->ci= 0; _temp32->cc=
0; _temp32->cl= 1; _temp32->pw= w; _temp32->epw= w; _temp32;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp35=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp35->ci= 0; _temp35->cc=
0; _temp35->cl= 1; _temp35->pw= w; _temp35->epw= w; _temp35;})); return Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*) o->ppout),(
struct _tagged_string)({ char* _temp33=( char*)""; struct _tagged_string _temp34;
_temp34.curr= _temp33; _temp34.base= _temp33; _temp34.last_plus_one= _temp33 + 1;
_temp34;}));} struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int
w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp39=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp39->ci= 0; _temp39->cc=
0; _temp39->cl= 1; _temp39->pw= w; _temp39->epw= w; _temp39;})); return({ struct
_tuple0* _temp36=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp36->f1=
Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout),( struct _tagged_string)({ char* _temp37=( char*)""; struct
_tagged_string _temp38; _temp38.curr= _temp37; _temp38.base= _temp37; _temp38.last_plus_one=
_temp37 + 1; _temp38;})); _temp36->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->links); _temp36;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0;
struct _tagged_string Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp40=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp40->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp40;});} if( i < 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp41=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp41[ 0]=({ struct Cyc_Core_Failure_struct
_temp42; _temp42.tag= Cyc_Core_Failure; _temp42.f1=( struct _tagged_string)({
char* _temp43=( char*)"nlblanks"; struct _tagged_string _temp44; _temp44.curr=
_temp43; _temp44.base= _temp43; _temp44.last_plus_one= _temp43 + 9; _temp44;});
_temp42;}); _temp41;}));}{ struct _handler_cons _temp45; _push_handler(& _temp45);{
int _temp47= 0; if( setjmp( _temp45.handler)){ _temp47= 1;} if( ! _temp47){{
struct _tagged_string _temp49=*(( struct _tagged_string*(*)( struct Cyc_Hashtable_Table*
t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt*
_temp48= Cyc_PP_bhashtbl; if( _temp48 == 0){ _throw( Null_Exception);} _temp48->v;}),
i); _npop_handler( 0u); return _temp49;}; _pop_handler();} else{ void* _temp46=(
void*) _exn_thrown; void* _temp51= _temp46; _LL53: if( _temp51 == Cyc_Core_Not_found){
goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL54: { int num_tabs= i / 8;
int num_spaces= i % 8; int total=( 1 + num_tabs) + num_spaces; struct
_tagged_string nlb= Cyc_Core_new_string( total);({ struct _tagged_string _temp57=
nlb; char* _temp59= _temp57.curr + 0; if( _temp57.base == 0? 1:( _temp59 <
_temp57.base? 1: _temp59 >= _temp57.last_plus_one)){ _throw( Null_Exception);}*
_temp59='\n';});{ int j= 0; for( 0; j < num_tabs; j ++){({ struct _tagged_string
_temp60= nlb; char* _temp62= _temp60.curr +( j + 1); if( _temp60.base == 0? 1:(
_temp62 < _temp60.base? 1: _temp62 >= _temp60.last_plus_one)){ _throw(
Null_Exception);}* _temp62='\t';});}}{ int j= 0; for( 0; j < num_spaces; j ++){({
struct _tagged_string _temp63= nlb; char* _temp65= _temp63.curr +(( j + 1) +
num_tabs); if( _temp63.base == 0? 1:( _temp65 < _temp63.base? 1: _temp65 >=
_temp63.last_plus_one)){ _throw( Null_Exception);}* _temp65=' ';});}}(( void(*)(
struct Cyc_Hashtable_Table* t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt* _temp66= Cyc_PP_bhashtbl;
if( _temp66 == 0){ _throw( Null_Exception);} _temp66->v;}), i,({ struct
_tagged_string* _temp67=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp67[ 0]= nlb; _temp67;})); return nlb;} _LL56:( void)
_throw( _temp51); _LL52:;}}}} static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0;
int Cyc_PP_infinity= 9999999; struct _tuple1{ int f1; struct _tagged_string f2;
} ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f( struct _tuple1* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple1 _temp70; struct _tagged_string _temp71; int _temp73; struct
_tuple1* _temp68= clo; _temp70=* _temp68; _LL74: _temp73= _temp70.f1; goto _LL72;
_LL72: _temp71= _temp70.f2; goto _LL69; _LL69: return({ struct Cyc_PP_Out*
_temp75=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp75->newcc=
st->cc + _temp73; _temp75->newcl= st->cl; _temp75->ppout=( void*)(( void*)({
struct Cyc_PP_Single_struct* _temp76=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp76[ 0]=({ struct Cyc_PP_Single_struct
_temp77; _temp77.tag= Cyc_PP_Single; _temp77.f1=( void*)({ struct _tagged_string*
_temp78=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp78[ 0]= _temp71; _temp78;}); _temp77;}); _temp76;})); _temp75->links=( void*)((
void*) Cyc_PP_Empty); _temp75;});} static struct Cyc_PP_Doc* Cyc_PP_text_doc(
struct _tagged_string s){ int slen=( int) Cyc_String_strlen( s); return({ struct
Cyc_PP_Doc* _temp79=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp79->mwo= slen; _temp79->mw= Cyc_PP_infinity; _temp79->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*), struct
_tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1* _temp80=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp80->f1= slen; _temp80->f2=
s; _temp80;})); _temp79;});} struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl == 0){
t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_string*,
struct _tagged_string*), int(* hash)( struct _tagged_string*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp81=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp81->v=( void*) t; _temp81;});} else{ t=( struct Cyc_Hashtable_Table*)({
struct Cyc_Core_Opt* _temp82= Cyc_PP_str_hashtbl; if( _temp82 == 0){ _throw(
Null_Exception);} _temp82->v;});}{ struct _handler_cons _temp83; _push_handler(&
_temp83);{ int _temp85= 0; if( setjmp( _temp83.handler)){ _temp85= 1;} if( !
_temp85){{ struct Cyc_PP_Doc* _temp87=(( struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_string* key)) Cyc_Hashtable_lookup)( t,({ struct
_tagged_string* _temp86=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp86[ 0]= s; _temp86;})); _npop_handler( 0u); return
_temp87;}; _pop_handler();} else{ void* _temp84=( void*) _exn_thrown; void*
_temp89= _temp84; _LL91: if( _temp89 == Cyc_Core_Not_found){ goto _LL92;} else{
goto _LL93;} _LL93: goto _LL94; _LL92: { struct Cyc_PP_Doc* d= Cyc_PP_text_doc(
s);(( void(*)( struct Cyc_Hashtable_Table* t, struct _tagged_string* key, struct
Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct _tagged_string* _temp95=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp95[
0]= s; _temp95;}), d); return d;} _LL94:( void) _throw( _temp89); _LL90:;}}}}
struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt= 0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){
if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({ struct Cyc_Core_Opt* _temp96=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp96->v=(
void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp97=( char*)""; struct
_tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97; _temp98.last_plus_one=
_temp97 + 1; _temp98;})); _temp96;});} return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt*
_temp99= Cyc_PP_nil_doc_opt; if( _temp99 == 0){ _throw( Null_Exception);}
_temp99->v;});} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt
== 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt* _temp100=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp100->v=( void*) Cyc_PP_text((
struct _tagged_string)({ char* _temp101=( char*)""; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 1; _temp102;})); _temp100;});} return( struct Cyc_PP_Doc*)({ struct
Cyc_Core_Opt* _temp103= Cyc_PP_blank_doc_opt; if( _temp103 == 0){ _throw(
Null_Exception);} _temp103->v;});} struct _tuple2{ int f1; struct _tagged_string
f2; struct _tagged_string f3; } ; struct _tuple3{ int f1; int f2; int f3; struct
_tagged_string f4; } ; static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct
_tuple2* clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp106; struct
_tagged_string _temp107; struct _tagged_string _temp109; int _temp111; struct
_tuple2* _temp104= clo; _temp106=* _temp104; _LL112: _temp111= _temp106.f1; goto
_LL110; _LL110: _temp109= _temp106.f2; goto _LL108; _LL108: _temp107= _temp106.f3;
goto _LL105; _LL105: return({ struct Cyc_PP_Out* _temp113=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp113->newcc= st->cc + _temp111;
_temp113->newcl= st->cl; _temp113->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp117=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp117[ 0]=({ struct Cyc_PP_Single_struct _temp118; _temp118.tag= Cyc_PP_Single;
_temp118.f1=( void*)({ struct _tagged_string* _temp119=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp119[ 0]= _temp109; _temp119;});
_temp118;}); _temp117;})); _temp113->links=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp114=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp114[ 0]=({ struct Cyc_PP_Single_struct _temp115; _temp115.tag= Cyc_PP_Single;
_temp115.f1=( void*)({ struct _tuple3* _temp116=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp116->f1= st->cl; _temp116->f2= st->cc; _temp116->f3=
_temp111; _temp116->f4= _temp107; _temp116;}); _temp115;}); _temp114;}));
_temp113;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt,
struct _tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({
struct Cyc_PP_Doc* _temp120=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp120->mwo= slen; _temp120->mw= Cyc_PP_infinity; _temp120->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp121=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp121->f1= slen;
_temp121->f2= shrt; _temp121->f3= full; _temp121;})); _temp120;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp122=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp122->newcc= st->ci; _temp122->newcl= st->cl + 1; _temp122->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp123=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp123[ 0]=({ struct Cyc_PP_Single_struct
_temp124; _temp124.tag= Cyc_PP_Single; _temp124.f1=( void*)({ struct
_tagged_string* _temp125=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp125[ 0]= Cyc_PP_nlblanks( st->ci); _temp125;});
_temp124;}); _temp123;})); _temp122->links=( void*)(( void*) Cyc_PP_Empty);
_temp122;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp126=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp126->v=( void*)({ struct Cyc_PP_Doc*
_temp127=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp127->mwo=
0; _temp127->mw= 0; _temp127->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp127;}); _temp126;});}
return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt* _temp128= Cyc_PP_line_doc_opt;
if( _temp128 == 0){ _throw( Null_Exception);} _temp128->v;});} struct _tuple4{
int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp131; struct
Cyc_PP_Doc* _temp132; int _temp134; struct _tuple4* _temp129= clo; _temp131=*
_temp129; _LL135: _temp134= _temp131.f1; goto _LL133; _LL133: _temp132= _temp131.f2;
goto _LL130; _LL130: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp136=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp136->ci= st->ci + _temp134; _temp136->cc= st->cc; _temp136->cl= st->cl;
_temp136->pw= st->pw; _temp136->epw= st->epw; _temp136;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp132->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp137=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp137->mwo= d->mwo; _temp137->mw= d->mw; _temp137->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp138=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp138->f1= k; _temp138->f2= d;
_temp138;})); _temp137;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp141; struct Cyc_PP_Doc* _temp142; struct Cyc_PP_Doc*
_temp144; struct _tuple5* _temp139= clo; _temp141=* _temp139; _LL145: _temp144=
_temp141.f1; goto _LL143; _LL143: _temp142= _temp141.f2; goto _LL140; _LL140: {
int epw2= Cyc_PP_max( st->pw - _temp142->mw, st->epw - _temp144->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp148=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp148->ci= st->ci;
_temp148->cc= st->cc; _temp148->cl= st->cl; _temp148->pw= st->pw; _temp148->epw=
epw2; _temp148;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp144->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp147=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp147->ci= st->ci;
_temp147->cc= o1->newcc; _temp147->cl= o1->newcl; _temp147->pw= st->pw; _temp147->epw=
epw2; _temp147;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp142->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp146=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp146->newcc= o2->newcc; _temp146->newcl= o2->newcl;
_temp146->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp146->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp146;});
return o3;}} struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return({ struct Cyc_PP_Doc* _temp149=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp149->mwo= Cyc_PP_min( d1->mw, d1->mwo + d2->mwo);
_temp149->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp149->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp150=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp150->f1= d1; _temp150->f2=
d2; _temp150;})); _temp149;});} static struct Cyc_PP_Out* Cyc_PP_long_concats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp151= ds; if( _temp151 == 0){ _throw( Null_Exception);} _temp151->tl;})){
struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp154= ds;
if( _temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}); struct Cyc_PP_Out*
o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp152=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp152->ci= st->ci;
_temp152->cc= o->newcc; _temp152->cl= o->newcl; _temp152->pw= st->pw; _temp152->epw=
st->epw - d->mwo; _temp152;}); os=({ struct Cyc_List_List* _temp153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp153->hd=( void*) o; _temp153->tl=
os; _temp153;});}}{ int newcc=(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp160= os; if( _temp160 == 0){ _throw( Null_Exception);} _temp160->hd;}))->newcc;
int newcl=(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp159= os; if(
_temp159 == 0){ _throw( Null_Exception);} _temp159->hd;}))->newcl; void* s=(
void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=({
struct Cyc_List_List* _temp155= os; if( _temp155 == 0){ _throw( Null_Exception);}
_temp155->tl;})){ s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)((
struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp156= os; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->hd;}))->ppout, s); links=(( void*(*)( void*
a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp157= os; if( _temp157 == 0){ _throw( Null_Exception);} _temp157->hd;}))->links,
links);} return({ struct Cyc_PP_Out* _temp158=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp158->newcc= newcc; _temp158->newcl= newcl;
_temp158->ppout=( void*) s; _temp158->links=( void*) links; _temp158;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_concats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp166= doclist; if( _temp166 == 0){ _throw(
Null_Exception);} _temp166->hd;}); doclist=({ struct Cyc_List_List* _temp161=
doclist; if( _temp161 == 0){ _throw( Null_Exception);} _temp161->tl;});{ int mw=
d->mw; int mwo= d->mw;{ struct Cyc_List_List* ds= doclist; for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp162= ds; if( _temp162 == 0){ _throw( Null_Exception);}
_temp162->tl;})){ int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({ struct
Cyc_List_List* _temp164= ds; if( _temp164 == 0){ _throw( Null_Exception);}
_temp164->hd;}))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp163= ds; if( _temp163 == 0){ _throw( Null_Exception);}
_temp163->hd;}))->mw); mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc*
_temp165=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp165->mwo=
mw; _temp165->mw= mwo; _temp165->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_List_List*, struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)(
Cyc_PP_long_concats_f, orig); _temp165;});}} struct Cyc_PP_Doc* Cyc_PP_concats(
struct Cyc_List_List* doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();}
else{ if(({ struct Cyc_List_List* _temp167= doclist; if( _temp167 == 0){ _throw(
Null_Exception);} _temp167->tl;}) == 0){ return( struct Cyc_PP_Doc*)({ struct
Cyc_List_List* _temp168= doclist; if( _temp168 == 0){ _throw( Null_Exception);}
_temp168->hd;});} else{ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
doclist) > 30){ return Cyc_PP_long_concats( doclist);} else{ return Cyc_PP_concat((
struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp169= doclist; if( _temp169 == 0){
_throw( Null_Exception);} _temp169->hd;}), Cyc_PP_concats(({ struct Cyc_List_List*
_temp170= doclist; if( _temp170 == 0){ _throw( Null_Exception);} _temp170->tl;})));}}}}
static struct Cyc_PP_Out* Cyc_PP_concats_arr_f( struct _tagged_ptr0* docs_ptr,
struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0; struct _tagged_ptr0
docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0 _temp182= docs;(
unsigned int)( _temp182.last_plus_one - _temp182.curr);});{ int i= 0; for( 0; i
< sz; ++ i){ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp173= docs; struct
Cyc_PP_Doc** _temp175= _temp173.curr + i; if( _temp173.base == 0? 1:( _temp175 <
_temp173.base? 1: _temp175 >= _temp173.last_plus_one)){ _throw( Null_Exception);}*
_temp175;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp171=(
struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp171->ci= st->ci; _temp171->cc= o->newcc; _temp171->cl= o->newcl; _temp171->pw=
st->pw; _temp171->epw= st->epw - d->mwo; _temp171;}); os=({ struct Cyc_List_List*
_temp172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp172->hd=( void*) o; _temp172->tl= os; _temp172;});}}{ int newcc=(( struct
Cyc_PP_Out*)({ struct Cyc_List_List* _temp181= os; if( _temp181 == 0){ _throw(
Null_Exception);} _temp181->hd;}))->newcc; int newcl=(( struct Cyc_PP_Out*)({
struct Cyc_List_List* _temp180= os; if( _temp180 == 0){ _throw( Null_Exception);}
_temp180->hd;}))->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty;
for( 0; os != 0; os=({ struct Cyc_List_List* _temp176= os; if( _temp176 == 0){
_throw( Null_Exception);} _temp176->tl;})){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp177=
os; if( _temp177 == 0){ _throw( Null_Exception);} _temp177->hd;}))->ppout, s);
links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({
struct Cyc_List_List* _temp178= os; if( _temp178 == 0){ _throw( Null_Exception);}
_temp178->hd;}))->links, links);} return({ struct Cyc_PP_Out* _temp179=( struct
Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp179->newcc= newcc;
_temp179->newcl= newcl; _temp179->ppout=( void*) s; _temp179->links=( void*)
links; _temp179;});}} struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct _tagged_ptr0
docs){ int sz=( int)({ struct _tagged_ptr0 _temp198= docs;( unsigned int)(
_temp198.last_plus_one - _temp198.curr);}); if( sz == 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp183=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp183[ 0]=({ struct Cyc_Core_Failure_struct
_temp184; _temp184.tag= Cyc_Core_Failure; _temp184.f1=( struct _tagged_string)({
char* _temp185=( char*)"concats_arr -- size zero array"; struct _tagged_string
_temp186; _temp186.curr= _temp185; _temp186.base= _temp185; _temp186.last_plus_one=
_temp185 + 31; _temp186;}); _temp184;}); _temp183;}));}{ struct Cyc_PP_Doc* d=({
struct _tagged_ptr0 _temp195= docs; struct Cyc_PP_Doc** _temp197= _temp195.curr
+ 0; if( _temp195.base == 0? 1:( _temp197 < _temp195.base? 1: _temp197 >=
_temp195.last_plus_one)){ _throw( Null_Exception);}* _temp197;}); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo +({
struct _tagged_ptr0 _temp190= docs; struct Cyc_PP_Doc** _temp192= _temp190.curr
+ i; if( _temp190.base == 0? 1:( _temp192 < _temp190.base? 1: _temp192 >=
_temp190.last_plus_one)){ _throw( Null_Exception);}* _temp192;})->mwo); int mwo2=
Cyc_PP_min( mw, mwo +({ struct _tagged_ptr0 _temp187= docs; struct Cyc_PP_Doc**
_temp189= _temp187.curr + i; if( _temp187.base == 0? 1:( _temp189 < _temp187.base?
1: _temp189 >= _temp187.last_plus_one)){ _throw( Null_Exception);}* _temp189;})->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp193=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp193->mwo= mw; _temp193->mw= mwo;
_temp193->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_ptr0*,
struct Cyc_PP_Ppstate*), struct _tagged_ptr0* x)) Cyc_Fn_make_fn)( Cyc_PP_concats_arr_f,({
struct _tagged_ptr0* _temp194=( struct _tagged_ptr0*) GC_malloc( sizeof( struct
_tagged_ptr0) * 1); _temp194[ 0]= docs; _temp194;})); _temp193;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp201; struct Cyc_PP_Doc* _temp202; struct Cyc_PP_Doc*
_temp204; struct _tuple5* _temp199= clo; _temp201=* _temp199; _LL205: _temp204=
_temp201.f1; goto _LL203; _LL203: _temp202= _temp201.f2; goto _LL200; _LL200: {
int dfits= st->cc + _temp204->mwo <= st->epw? 1: st->cc + _temp204->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp204->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp202->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp206=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp206->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp206->mw=
Cyc_PP_min( d->mw, d2->mw); _temp206->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp207=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp207->f1= d; _temp207->f2= d2; _temp207;}));
_temp206;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp208=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp208->ci= st->cc; _temp208->cc= st->cc;
_temp208->cl= st->cl; _temp208->pw= st->pw; _temp208->epw= st->epw; _temp208;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp209=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp209->mwo= d->mwo; _temp209->mw= d->mw;
_temp209->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp209;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List* _temp210= l; if(
_temp210 == 0){ _throw( Null_Exception);} _temp210->tl;}) == 0){ return pp((
void*)({ struct Cyc_List_List* _temp211= l; if( _temp211 == 0){ _throw(
Null_Exception);} _temp211->hd;}));} else{ return Cyc_PP_concats(({ struct Cyc_List_List*
_temp212=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp212->hd=( void*) pp(( void*)({ struct Cyc_List_List* _temp217= l; if(
_temp217 == 0){ _throw( Null_Exception);} _temp217->hd;})); _temp212->tl=({
struct Cyc_List_List* _temp213=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp213->hd=( void*) Cyc_PP_text( sep); _temp213->tl=({
struct Cyc_List_List* _temp214=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp214->hd=( void*) Cyc_PP_oline_doc(); _temp214->tl=({
struct Cyc_List_List* _temp215=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp215->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseq_f)( pp, sep,({ struct Cyc_List_List* _temp216= l; if( _temp216 == 0){
_throw( Null_Exception);} _temp216->tl;})); _temp215->tl= 0; _temp215;});
_temp214;}); _temp213;}); _temp212;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseq(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep,
l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List*
_temp218= l; if( _temp218 == 0){ _throw( Null_Exception);} _temp218->tl;}) == 0){
return( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp219= l; if( _temp219 ==
0){ _throw( Null_Exception);} _temp219->hd;});} else{ struct Cyc_PP_Doc* sep2=
Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List*
x= l; while(({ struct Cyc_List_List* _temp220= x; if( _temp220 == 0){ _throw(
Null_Exception);} _temp220->tl;}) != 0) { struct Cyc_List_List* temp=({ struct
Cyc_List_List* _temp224= x; if( _temp224 == 0){ _throw( Null_Exception);}
_temp224->tl;});({ struct Cyc_List_List* _temp221= x; if( _temp221 == 0){ _throw(
Null_Exception);} _temp221->tl=({ struct Cyc_List_List* _temp222=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp222->hd=( void*) sep2; _temp222->tl=({
struct Cyc_List_List* _temp223=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp223->hd=( void*) oline; _temp223->tl= temp;
_temp223;}); _temp222;});}); x= temp;} return Cyc_PP_concats( l);}}} struct Cyc_PP_Doc*
Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(
Cyc_PP_seq_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List* _temp225= l; if(
_temp225 == 0){ _throw( Null_Exception);} _temp225->tl;}) == 0){ return pp((
void*)({ struct Cyc_List_List* _temp226= l; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->hd;}));} else{ return Cyc_PP_concats(({ struct Cyc_List_List*
_temp227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp227->hd=( void*) pp(( void*)({ struct Cyc_List_List* _temp232= l; if(
_temp232 == 0){ _throw( Null_Exception);} _temp232->hd;})); _temp227->tl=({
struct Cyc_List_List* _temp228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp228->hd=( void*) Cyc_PP_text( sep); _temp228->tl=({
struct Cyc_List_List* _temp229=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp229->hd=( void*) Cyc_PP_line_doc(); _temp229->tl=({
struct Cyc_List_List* _temp230=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp230->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseql_f)( pp, sep,({ struct Cyc_List_List* _temp231= l; if( _temp231 ==
0){ _throw( Null_Exception);} _temp231->tl;})); _temp230->tl= 0; _temp230;});
_temp229;}); _temp228;}); _temp227;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseql(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep,
l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_string sep,
struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({
struct Cyc_List_List* _temp233= l; if( _temp233 == 0){ _throw( Null_Exception);}
_temp233->tl;}) == 0){ return( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp234= l; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->hd;});}
else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp235->hd=( void*)(( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp240= l; if( _temp240 == 0){ _throw( Null_Exception);}
_temp240->hd;})); _temp235->tl=({ struct Cyc_List_List* _temp236=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp236->hd=( void*) Cyc_PP_text(
sep); _temp236->tl=({ struct Cyc_List_List* _temp237=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp237->hd=( void*) Cyc_PP_line_doc();
_temp237->tl=({ struct Cyc_List_List* _temp238=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp238->hd=( void*) Cyc_PP_seql_f(
sep,({ struct Cyc_List_List* _temp239= l; if( _temp239 == 0){ _throw(
Null_Exception);} _temp239->tl;})); _temp238->tl= 0; _temp238;}); _temp237;});
_temp236;}); _temp235;}));}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));}