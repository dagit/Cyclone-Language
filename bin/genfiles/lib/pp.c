#include "cyc_include.h"

 struct _tuple0{ struct _tagged_string f1; struct Cyc_List_List* f2; } ; struct
_tagged_ptr0{ struct Cyc_PP_Doc** curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc**
last_plus_one; } ; struct _tagged_ptr1{ void** curr; void** base; void**
last_plus_one; } ; struct _tuple1{ int f1; struct _tagged_string f2; } ; struct
_tuple2{ int f1; struct _tagged_string f2; struct _tagged_string f3; } ; struct
_tuple3{ int f1; int f2; int f3; struct _tagged_string f4; } ; struct _tuple4{
int f1; struct Cyc_PP_Doc* f2; } ; struct _tuple5{ struct Cyc_PP_Doc* f1; struct
Cyc_PP_Doc* f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern void* Cyc_Core_identity( void*); extern int Cyc_Core_intcmp( int,
int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern unsigned int Cyc_String_strlen( struct _tagged_string s);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); struct Cyc_Hashtable_Table; typedef struct Cyc_Hashtable_Table*
Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table* Cyc_Hashtable_create(
int sz, int(* cmp)( void*, void*), int(* hash)( void*)); extern void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table* t, void* key, void* val); extern void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key); extern int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s); typedef void* Cyc_Fn_Function; static const int Cyc_Fn_Fun_tag=
0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*, void*); void* f2; } ;
typedef void* Cyc_Fn_fn; extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*),
void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply(
void* f, void* x); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct
Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE* f); extern struct _tagged_string
Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w); extern struct _tuple0* Cyc_PP_string_and_links(
struct Cyc_PP_Doc* d, int w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc*
Cyc_PP_blank_doc(); extern struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct
_tagged_string shrt, struct _tagged_string full); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); extern
struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2);
extern struct Cyc_PP_Doc* Cyc_PP_concats( struct Cyc_List_List* doclist); extern
struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct _tagged_ptr0 docs); extern struct
Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2);
extern struct Cyc_PP_Doc* Cyc_PP_oline_doc(); extern struct Cyc_PP_Doc* Cyc_PP_tab(
struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0); extern struct
Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0);
extern struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Xarray_Xarray{
struct _tagged_ptr1 elmts; int num_elmts; } ; typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*, int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
typedef void* Cyc_PP_Alist; static const unsigned int Cyc_PP_Empty= 0; static
const int Cyc_PP_Single_tag= 0; struct Cyc_PP_Single_struct{ int tag; void* f1;
} ; static const int Cyc_PP_Append_tag= 1; struct Cyc_PP_Append_struct{ int tag;
void* f1; void* f2; } ; typedef void* Cyc_PP_alist_t; void* Cyc_PP_append( void*
a1, void* a2){ if( a1 ==( void*) Cyc_PP_Empty){ return a2;} else{ if( a2 ==(
void*) Cyc_PP_Empty){ return a1;} else{ return( void*)({ struct Cyc_PP_Append_struct*
_temp0=( struct Cyc_PP_Append_struct*) GC_malloc( sizeof( struct Cyc_PP_Append_struct));
_temp0[ 0]=({ struct Cyc_PP_Append_struct _temp1; _temp1.tag= Cyc_PP_Append_tag;
_temp1.f1=( void*) a1; _temp1.f2=( void*) a2; _temp1;}); _temp0;});}}} struct
Cyc_List_List* Cyc_PP_list_of_alist_f( void* y, struct Cyc_List_List* l){ void*
_temp2= y; void* _temp10; void* _temp12; void* _temp14; _LL4: if(( int) _temp2
== Cyc_PP_Empty){ goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp2
> 1u?(( struct _tunion_struct*) _temp2)->tag == Cyc_PP_Single_tag: 0){ _LL11:
_temp10=( void*)(( struct Cyc_PP_Single_struct*) _temp2)->f1; goto _LL7;} else{
goto _LL8;} _LL8: if(( unsigned int) _temp2 > 1u?(( struct _tunion_struct*)
_temp2)->tag == Cyc_PP_Append_tag: 0){ _LL15: _temp14=( void*)(( struct Cyc_PP_Append_struct*)
_temp2)->f1; goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_PP_Append_struct*)
_temp2)->f2; goto _LL9;} else{ goto _LL3;} _LL5: return l; _LL7: return({ struct
Cyc_List_List* _temp16=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp16->hd=( void*) _temp10; _temp16->tl= l; _temp16;}); _LL9: return(( struct
Cyc_List_List*(*)( void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)(
_temp14,(( struct Cyc_List_List*(*)( void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)(
_temp12, l)); _LL3:;} struct Cyc_List_List* Cyc_PP_list_of_alist( void* x){
return(( struct Cyc_List_List*(*)( void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)(
x, 0);} struct Cyc_PP_Ppstate{ int ci; int cc; int cl; int pw; int epw; } ;
struct Cyc_PP_Out{ int newcc; int newcl; void* ppout; void* links; } ; struct
Cyc_PP_Doc{ int mwo; int mw; void* f; } ; static void Cyc_PP_dump_out( struct
Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray* xarr=(( struct Cyc_Xarray_Xarray*(*)(
int, void*)) Cyc_Xarray_create)( 16, al);(( void(*)( struct Cyc_Xarray_Xarray*,
void*)) Cyc_Xarray_add)( xarr, al);{ int last= 0; while( last >= 0) { void*
_temp17=(( void*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)( xarr,
last); struct _tagged_string* _temp25; void* _temp27; void* _temp29; _LL19: if((
int) _temp17 == Cyc_PP_Empty){ goto _LL20;} else{ goto _LL21;} _LL21: if((
unsigned int) _temp17 > 1u?(( struct _tunion_struct*) _temp17)->tag == Cyc_PP_Single_tag:
0){ _LL26: _temp25=( struct _tagged_string*)(( struct Cyc_PP_Single_struct*)
_temp17)->f1; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp17
> 1u?(( struct _tunion_struct*) _temp17)->tag == Cyc_PP_Append_tag: 0){ _LL30:
_temp29=( void*)(( struct Cyc_PP_Append_struct*) _temp17)->f1; goto _LL28; _LL28:
_temp27=( void*)(( struct Cyc_PP_Append_struct*) _temp17)->f2; goto _LL24;}
else{ goto _LL18;} _LL20: -- last; goto _LL18; _LL22: -- last;({ struct
_tagged_string _temp31=* _temp25; fprintf( f,"%.*s", _temp31.last_plus_one -
_temp31.curr, _temp31.curr);}); goto _LL18; _LL24:(( void(*)( struct Cyc_Xarray_Xarray*,
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
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp33=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp33->ci= 0; _temp33->cc=
0; _temp33->cl= 1; _temp33->pw= w; _temp33->epw= w; _temp33;})); return Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*) o->ppout),(
struct _tagged_string)({ char* _temp34=( char*)""; struct _tagged_string _temp35;
_temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one= _temp34 + 1;
_temp35;}));} struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int
w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp36=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp36->ci= 0; _temp36->cc=
0; _temp36->cl= 1; _temp36->pw= w; _temp36->epw= w; _temp36;})); return({ struct
_tuple0* _temp37=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp37->f1=
Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout),( struct _tagged_string)({ char* _temp38=( char*)""; struct
_tagged_string _temp39; _temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one=
_temp38 + 1; _temp39;})); _temp37->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->links); _temp37;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0;
struct _tagged_string Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp40=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp40->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp40;});} if( i < 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp41=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp41[ 0]=({ struct Cyc_Core_Failure_struct
_temp42; _temp42.tag= Cyc_Core_Failure_tag; _temp42.f1=( struct _tagged_string)({
char* _temp43=( char*)"nlblanks"; struct _tagged_string _temp44; _temp44.curr=
_temp43; _temp44.base= _temp43; _temp44.last_plus_one= _temp43 + 9; _temp44;});
_temp42;}); _temp41;}));}{ struct _handler_cons _temp45; _push_handler(& _temp45);{
struct _xtunion_struct* _temp46=( struct _xtunion_struct*) setjmp( _temp45.handler);
if( ! _temp46){{ struct _tagged_string _temp47=*(( struct _tagged_string*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)
Cyc_PP_bhashtbl->v, i); _npop_handler( 0u); return _temp47;}; _pop_handler();}
else{ struct _xtunion_struct* _temp49= _temp46; _LL51: if( _temp49->tag == Cyc_Core_Not_found_tag){
goto _LL52;} else{ goto _LL53;} _LL53: goto _LL54; _LL52: { int num_tabs= i / 8;
int num_spaces= i % 8; int total=( 1 + num_tabs) + num_spaces; struct
_tagged_string nlb= Cyc_Core_new_string( total);({ struct _tagged_string _temp55=
nlb; char* _temp57= _temp55.curr + 0; if( _temp57 < _temp55.base? 1: _temp57 >=
_temp55.last_plus_one){ _throw( Null_Exception);}* _temp57='\n';});{ int j= 0;
for( 0; j < num_tabs; j ++){({ struct _tagged_string _temp58= nlb; char* _temp60=
_temp58.curr +( j + 1); if( _temp60 < _temp58.base? 1: _temp60 >= _temp58.last_plus_one){
_throw( Null_Exception);}* _temp60='\t';});}}{ int j= 0; for( 0; j < num_spaces;
j ++){({ struct _tagged_string _temp61= nlb; char* _temp63= _temp61.curr +(( j +
1) + num_tabs); if( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one){
_throw( Null_Exception);}* _temp63=' ';});}}(( void(*)( struct Cyc_Hashtable_Table*
t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)(( struct Cyc_Hashtable_Table*)
Cyc_PP_bhashtbl->v, i,({ struct _tagged_string* _temp64=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp64[ 0]= nlb; _temp64;}));
return nlb;} _LL54:( void) _throw( _temp49); _LL50:;}}}} static struct Cyc_Core_Opt*
Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999; static struct Cyc_PP_Out*
Cyc_PP_text_doc_f( struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct
_tuple1 _temp67; struct _tagged_string _temp68; int _temp70; struct _tuple1*
_temp65= clo; _temp67=* _temp65; _LL71: _temp70= _temp67.f1; goto _LL69; _LL69:
_temp68= _temp67.f2; goto _LL66; _LL66: return({ struct Cyc_PP_Out* _temp72=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp72->newcc= st->cc
+ _temp70; _temp72->newcl= st->cl; _temp72->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp73=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp73[ 0]=({ struct Cyc_PP_Single_struct _temp74; _temp74.tag= Cyc_PP_Single_tag;
_temp74.f1=( void*)({ struct _tagged_string* _temp75=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp75[ 0]= _temp68; _temp75;});
_temp74;}); _temp73;})); _temp72->links=( void*)(( void*) Cyc_PP_Empty); _temp72;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_string s){ int slen=(
int) Cyc_String_strlen( s); return({ struct Cyc_PP_Doc* _temp76=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp76->mwo= slen; _temp76->mw= Cyc_PP_infinity;
_temp76->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct
Cyc_PP_Ppstate*), struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({
struct _tuple1* _temp77=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp77->f1= slen; _temp77->f2= s; _temp77;})); _temp76;});} struct Cyc_PP_Doc*
Cyc_PP_text( struct _tagged_string s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl
== 0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct
_tagged_string*, struct _tagged_string*), int(* hash)( struct _tagged_string*)))
Cyc_Hashtable_create)( 101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({ struct Cyc_Core_Opt* _temp78=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp78->v=( void*) t; _temp78;});}
else{ t=( struct Cyc_Hashtable_Table*) Cyc_PP_str_hashtbl->v;}{ struct
_handler_cons _temp79; _push_handler(& _temp79);{ struct _xtunion_struct*
_temp80=( struct _xtunion_struct*) setjmp( _temp79.handler); if( ! _temp80){{
struct Cyc_PP_Doc* _temp82=(( struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_string* key)) Cyc_Hashtable_lookup)( t,({ struct
_tagged_string* _temp81=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp81[ 0]= s; _temp81;})); _npop_handler( 0u); return
_temp82;}; _pop_handler();} else{ struct _xtunion_struct* _temp84= _temp80;
_LL86: if( _temp84->tag == Cyc_Core_Not_found_tag){ goto _LL87;} else{ goto
_LL88;} _LL88: goto _LL89; _LL87: { struct Cyc_PP_Doc* d= Cyc_PP_text_doc( s);((
void(*)( struct Cyc_Hashtable_Table* t, struct _tagged_string* key, struct Cyc_PP_Doc*
val)) Cyc_Hashtable_insert)( t,({ struct _tagged_string* _temp90=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp90[ 0]= s;
_temp90;}), d); return d;} _LL89:( void) _throw( _temp84); _LL85:;}}}} struct
Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt= 0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){
if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({ struct Cyc_Core_Opt* _temp91=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp91->v=(
void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp92=( char*)""; struct
_tagged_string _temp93; _temp93.curr= _temp92; _temp93.base= _temp92; _temp93.last_plus_one=
_temp92 + 1; _temp93;})); _temp91;});} return( struct Cyc_PP_Doc*) Cyc_PP_nil_doc_opt->v;}
struct Cyc_PP_Doc* Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({
struct Cyc_Core_Opt* _temp94=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp94->v=( void*) Cyc_PP_text(( struct _tagged_string)({ char*
_temp95=( char*)""; struct _tagged_string _temp96; _temp96.curr= _temp95;
_temp96.base= _temp95; _temp96.last_plus_one= _temp95 + 1; _temp96;})); _temp94;});}
return( struct Cyc_PP_Doc*) Cyc_PP_blank_doc_opt->v;} static struct Cyc_PP_Out*
Cyc_PP_hyperlink_f( struct _tuple2* clo, struct Cyc_PP_Ppstate* st){ struct
_tuple2 _temp99; struct _tagged_string _temp100; struct _tagged_string _temp102;
int _temp104; struct _tuple2* _temp97= clo; _temp99=* _temp97; _LL105: _temp104=
_temp99.f1; goto _LL103; _LL103: _temp102= _temp99.f2; goto _LL101; _LL101:
_temp100= _temp99.f3; goto _LL98; _LL98: return({ struct Cyc_PP_Out* _temp106=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp106->newcc= st->cc
+ _temp104; _temp106->newcl= st->cl; _temp106->ppout=( void*)(( void*)({ struct
Cyc_PP_Single_struct* _temp110=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp110[ 0]=({ struct Cyc_PP_Single_struct
_temp111; _temp111.tag= Cyc_PP_Single_tag; _temp111.f1=( void*)({ struct
_tagged_string* _temp112=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp112[ 0]= _temp102; _temp112;}); _temp111;}); _temp110;}));
_temp106->links=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp107=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp107[ 0]=({ struct Cyc_PP_Single_struct _temp108; _temp108.tag= Cyc_PP_Single_tag;
_temp108.f1=( void*)({ struct _tuple3* _temp109=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp109->f1= st->cl; _temp109->f2= st->cc; _temp109->f3=
_temp104; _temp109->f4= _temp100; _temp109;}); _temp108;}); _temp107;}));
_temp106;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt,
struct _tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({
struct Cyc_PP_Doc* _temp113=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp113->mwo= slen; _temp113->mw= Cyc_PP_infinity; _temp113->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp114=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp114->f1= slen;
_temp114->f2= shrt; _temp114->f3= full; _temp114;})); _temp113;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp115=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp115->newcc= st->ci; _temp115->newcl= st->cl + 1; _temp115->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp116=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp116[ 0]=({ struct Cyc_PP_Single_struct
_temp117; _temp117.tag= Cyc_PP_Single_tag; _temp117.f1=( void*)({ struct
_tagged_string* _temp118=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp118[ 0]= Cyc_PP_nlblanks( st->ci); _temp118;});
_temp117;}); _temp116;})); _temp115->links=( void*)(( void*) Cyc_PP_Empty);
_temp115;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp119=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp119->v=( void*)({ struct Cyc_PP_Doc*
_temp120=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp120->mwo=
0; _temp120->mw= 0; _temp120->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp120;}); _temp119;});}
return( struct Cyc_PP_Doc*) Cyc_PP_line_doc_opt->v;} static struct Cyc_PP_Out*
Cyc_PP_nest_f( struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4
_temp123; struct Cyc_PP_Doc* _temp124; int _temp126; struct _tuple4* _temp121=
clo; _temp123=* _temp121; _LL127: _temp126= _temp123.f1; goto _LL125; _LL125:
_temp124= _temp123.f2; goto _LL122; _LL122: { struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp128=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp128->ci= st->ci + _temp126; _temp128->cc=
st->cc; _temp128->cl= st->cl; _temp128->pw= st->pw; _temp128->epw= st->epw;
_temp128;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp124->f, st2);}} struct Cyc_PP_Doc* Cyc_PP_nest( int
k, struct Cyc_PP_Doc* d){ return({ struct Cyc_PP_Doc* _temp129=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp129->mwo= d->mwo; _temp129->mw= d->mw;
_temp129->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple4*,
struct Cyc_PP_Ppstate*), struct _tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({
struct _tuple4* _temp130=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp130->f1= k; _temp130->f2= d; _temp130;})); _temp129;});} int Cyc_PP_min(
int x, int y){ if( x <= y){ return x;} else{ return y;}} int Cyc_PP_max( int x,
int y){ if( x >= y){ return x;} else{ return y;}} static struct Cyc_PP_Out* Cyc_PP_concat_f(
struct _tuple5* clo, struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp133; struct
Cyc_PP_Doc* _temp134; struct Cyc_PP_Doc* _temp136; struct _tuple5* _temp131= clo;
_temp133=* _temp131; _LL137: _temp136= _temp133.f1; goto _LL135; _LL135:
_temp134= _temp133.f2; goto _LL132; _LL132: { int epw2= Cyc_PP_max( st->pw -
_temp134->mw, st->epw - _temp136->mwo); struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp138=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp138->ci= st->ci; _temp138->cc= st->cc; _temp138->cl= st->cl; _temp138->pw=
st->pw; _temp138->epw= epw2; _temp138;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp136->f, st2);
struct Cyc_PP_Ppstate* st3=({ struct Cyc_PP_Ppstate* _temp139=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp139->ci= st->ci;
_temp139->cc= o1->newcc; _temp139->cl= o1->newcl; _temp139->pw= st->pw; _temp139->epw=
epw2; _temp139;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp134->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp140=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp140->newcc= o2->newcc; _temp140->newcl= o2->newcl;
_temp140->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp140->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp140;});
return o3;}} struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return({ struct Cyc_PP_Doc* _temp141=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp141->mwo= Cyc_PP_min( d1->mw, d1->mwo + d2->mwo);
_temp141->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp141->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp142=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp142->f1= d1; _temp142->f2=
d2; _temp142;})); _temp141;});} static struct Cyc_PP_Out* Cyc_PP_long_concats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_PP_Doc*
d=( struct Cyc_PP_Doc*) ds->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp143=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp143->ci= st->ci; _temp143->cc= o->newcc;
_temp143->cl= o->newcl; _temp143->pw= st->pw; _temp143->epw= st->epw - d->mwo;
_temp143;}); os=({ struct Cyc_List_List* _temp144=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp144->hd=( void*) o; _temp144->tl=
os; _temp144;});}}{ int newcc=(( struct Cyc_PP_Out*) os->hd)->newcc; int newcl=((
struct Cyc_PP_Out*) os->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=(
void*) Cyc_PP_Empty; for( 0; os != 0; os= os->tl){ s=(( void*(*)( void* a1, void*
a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*) os->hd)->ppout, s); links=((
void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*) os->hd)->links,
links);} return({ struct Cyc_PP_Out* _temp145=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp145->newcc= newcc; _temp145->newcl= newcl;
_temp145->ppout=( void*) s; _temp145->links=( void*) links; _temp145;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_concats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)
doclist->hd; doclist= doclist->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List*
ds= doclist; for( 0; ds != 0; ds= ds->tl){ int mw2= Cyc_PP_min( mw, mwo +((
struct Cyc_PP_Doc*) ds->hd)->mwo); int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)
ds->hd)->mw); mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp146=(
struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp146->mwo= mw;
_temp146->mw= mwo; _temp146->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_List_List*, struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)(
Cyc_PP_long_concats_f, orig); _temp146;});}} struct Cyc_PP_Doc* Cyc_PP_concats(
struct Cyc_List_List* doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();}
else{ if( doclist->tl == 0){ return( struct Cyc_PP_Doc*) doclist->hd;} else{ if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( doclist) > 30){ return Cyc_PP_long_concats(
doclist);} else{ return Cyc_PP_concat(( struct Cyc_PP_Doc*) doclist->hd, Cyc_PP_concats(
doclist->tl));}}}} static struct Cyc_PP_Out* Cyc_PP_concats_arr_f( struct
_tagged_ptr0* docs_ptr, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0;
struct _tagged_ptr0 docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0
_temp147= docs;( unsigned int)( _temp147.last_plus_one - _temp147.curr);});{ int
i= 0; for( 0; i < sz; ++ i){ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0
_temp148= docs; struct Cyc_PP_Doc** _temp150= _temp148.curr + i; if( _temp150 <
_temp148.base? 1: _temp150 >= _temp148.last_plus_one){ _throw( Null_Exception);}*
_temp150;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp151=(
struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp151->ci= st->ci; _temp151->cc= o->newcc; _temp151->cl= o->newcl; _temp151->pw=
st->pw; _temp151->epw= st->epw - d->mwo; _temp151;}); os=({ struct Cyc_List_List*
_temp152=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp152->hd=( void*) o; _temp152->tl= os; _temp152;});}}{ int newcc=(( struct
Cyc_PP_Out*) os->hd)->newcc; int newcl=(( struct Cyc_PP_Out*) os->hd)->newcl;
void* s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os !=
0; os= os->tl){ s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)((
struct Cyc_PP_Out*) os->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*) os->hd)->links, links);} return({
struct Cyc_PP_Out* _temp153=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp153->newcc= newcc; _temp153->newcl= newcl; _temp153->ppout=( void*) s;
_temp153->links=( void*) links; _temp153;});}} struct Cyc_PP_Doc* Cyc_PP_concats_arr(
struct _tagged_ptr0 docs){ int sz=( int)({ struct _tagged_ptr0 _temp154= docs;(
unsigned int)( _temp154.last_plus_one - _temp154.curr);}); if( sz == 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp155=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp155[ 0]=({ struct Cyc_Core_Failure_struct _temp156; _temp156.tag= Cyc_Core_Failure_tag;
_temp156.f1=( struct _tagged_string)({ char* _temp157=( char*)"concats_arr -- size zero array";
struct _tagged_string _temp158; _temp158.curr= _temp157; _temp158.base= _temp157;
_temp158.last_plus_one= _temp157 + 31; _temp158;}); _temp156;}); _temp155;}));}{
struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp159= docs; struct Cyc_PP_Doc**
_temp161= _temp159.curr + 0; if( _temp161 < _temp159.base? 1: _temp161 >=
_temp159.last_plus_one){ _throw( Null_Exception);}* _temp161;}); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo +({
struct _tagged_ptr0 _temp162= docs; struct Cyc_PP_Doc** _temp164= _temp162.curr
+ i; if( _temp164 < _temp162.base? 1: _temp164 >= _temp162.last_plus_one){
_throw( Null_Exception);}* _temp164;})->mwo); int mwo2= Cyc_PP_min( mw, mwo +({
struct _tagged_ptr0 _temp165= docs; struct Cyc_PP_Doc** _temp167= _temp165.curr
+ i; if( _temp167 < _temp165.base? 1: _temp167 >= _temp165.last_plus_one){
_throw( Null_Exception);}* _temp167;})->mw); mw= mw2; mwo= mwo2;}} return({
struct Cyc_PP_Doc* _temp168=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp168->mwo= mw; _temp168->mw= mwo; _temp168->f=( void*)(( void*(*)( struct
Cyc_PP_Out*(* f)( struct _tagged_ptr0*, struct Cyc_PP_Ppstate*), struct
_tagged_ptr0* x)) Cyc_Fn_make_fn)( Cyc_PP_concats_arr_f,({ struct _tagged_ptr0*
_temp169=( struct _tagged_ptr0*) GC_malloc( sizeof( struct _tagged_ptr0) * 1);
_temp169[ 0]= docs; _temp169;})); _temp168;});}} static struct Cyc_PP_Out* Cyc_PP_doc_union_f(
struct _tuple5* clo, struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp172; struct
Cyc_PP_Doc* _temp173; struct Cyc_PP_Doc* _temp175; struct _tuple5* _temp170= clo;
_temp172=* _temp170; _LL176: _temp175= _temp172.f1; goto _LL174; _LL174:
_temp173= _temp172.f2; goto _LL171; _LL171: { int dfits= st->cc + _temp175->mwo
<= st->epw? 1: st->cc + _temp175->mw <= st->pw; if( dfits){ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp175->f, st);}
else{ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) _temp173->f, st);}}} struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc*
d, struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp177=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp177->mwo= Cyc_PP_min( d->mwo, d2->mwo);
_temp177->mw= Cyc_PP_min( d->mw, d2->mw); _temp177->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_doc_union_f,({ struct _tuple5* _temp178=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp178->f1= d; _temp178->f2=
d2; _temp178;})); _temp177;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return
Cyc_PP_doc_union( Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out*
Cyc_PP_tab_f( struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate*
st2=({ struct Cyc_PP_Ppstate* _temp179=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp179->ci= st->cc;
_temp179->cc= st->cc; _temp179->cl= st->cl; _temp179->pw= st->pw; _temp179->epw=
st->epw; _temp179;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct
Cyc_PP_Doc* d){ struct Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp180=( struct
Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp180->mwo= d->mwo;
_temp180->mw= d->mw; _temp180->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Doc*, struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)(
Cyc_PP_tab_f, d); _temp180;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if( l->tl == 0){ return pp((
void*) l->hd);} else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp181=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp181->hd=(
void*) pp(( void*) l->hd); _temp181->tl=({ struct Cyc_List_List* _temp182=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp182->hd=(
void*) Cyc_PP_text( sep); _temp182->tl=({ struct Cyc_List_List* _temp183=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp183->hd=(
void*) Cyc_PP_oline_doc(); _temp183->tl=({ struct Cyc_List_List* _temp184=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp184->hd=(
void*)(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep, l->tl);
_temp184->tl= 0; _temp184;}); _temp183;}); _temp182;}); _temp181;}));}}} struct
Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq_f)( pp, sep, l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct
_tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if( l->tl == 0){ return( struct Cyc_PP_Doc*) l->hd;} else{ struct Cyc_PP_Doc*
sep2= Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct
Cyc_List_List* x= l; while( x->tl != 0) { struct Cyc_List_List* temp= x->tl; x->tl=({
struct Cyc_List_List* _temp185=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp185->hd=( void*) sep2; _temp185->tl=({ struct Cyc_List_List*
_temp186=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp186->hd=( void*) oline; _temp186->tl= temp; _temp186;}); _temp185;}); x=
temp;} return Cyc_PP_concats( l);}}} struct Cyc_PP_Doc* Cyc_PP_seq( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seq_f(
sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if( l->tl == 0){ return pp(( void*) l->hd);} else{ return Cyc_PP_concats(({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*) pp(( void*) l->hd); _temp187->tl=({
struct Cyc_List_List* _temp188=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp188->hd=( void*) Cyc_PP_text( sep); _temp188->tl=({
struct Cyc_List_List* _temp189=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp189->hd=( void*) Cyc_PP_line_doc(); _temp189->tl=({
struct Cyc_List_List* _temp190=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp190->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseql_f)( pp, sep, l->tl); _temp190->tl= 0; _temp190;}); _temp189;});
_temp188;}); _temp187;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string
sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep, l0));} static struct
Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_string sep, struct Cyc_List_List* l){
if( l == 0){ return Cyc_PP_nil_doc();} else{ if( l->tl == 0){ return( struct Cyc_PP_Doc*)
l->hd;} else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp191=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp191->hd=( void*)((
struct Cyc_PP_Doc*) l->hd); _temp191->tl=({ struct Cyc_List_List* _temp192=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp192->hd=(
void*) Cyc_PP_text( sep); _temp192->tl=({ struct Cyc_List_List* _temp193=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp193->hd=(
void*) Cyc_PP_line_doc(); _temp193->tl=({ struct Cyc_List_List* _temp194=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp194->hd=(
void*) Cyc_PP_seql_f( sep, l->tl); _temp194->tl= 0; _temp194;}); _temp193;});
_temp192;}); _temp191;}));}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));}