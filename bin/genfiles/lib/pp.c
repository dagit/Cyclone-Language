#include "cyc_include.h"

 struct _tuple0{ struct _tagged_string f1; struct Cyc_List_List* f2;}; struct
_tagged_ptr0{ struct Cyc_PP_Doc** curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc**
last_plus_one;}; struct _tagged_ptr1{ void** curr; void** base; void**
last_plus_one;}; struct _tuple1{ int f1; struct _tagged_string f2;}; struct
_tuple2{ int f1; struct _tagged_string f2; struct _tagged_string f3;}; struct
_tuple3{ int f1; int f2; int f3; struct _tagged_string f4;}; struct _tuple4{ int
f1; struct Cyc_PP_Doc* f2;}; struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc*
f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
void* Cyc_Core_identity( void*); extern int Cyc_Core_intcmp( int, int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); extern struct _tagged_string Cyc_String_str_sepstr(
struct Cyc_List_List*, struct _tagged_string); struct Cyc_Hashtable_Table;
typedef struct Cyc_Hashtable_Table* Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_string* s);
typedef void* Cyc_Fn_Function; extern const int Cyc_Fn_Fun_tag; struct Cyc_Fn_Fun_struct{
int tag; void*(* f1)( void*, void*); void* f2;}; typedef void* Cyc_Fn_fn; extern
void* Cyc_Fn_make_fn( void*(* f)( void*, void*), void* x); extern void* Cyc_Fn_fp2fn(
void*(* f)( void*)); extern void* Cyc_Fn_apply( void* f, void* x); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); extern struct _tuple0* Cyc_PP_string_and_links(
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
struct _tagged_ptr1 elmts; int num_elmts;}; typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*, int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
typedef void* Cyc_PP_Alist; void* Cyc_PP_Empty=( void*) 0; const int Cyc_PP_Single_tag=
0; struct Cyc_PP_Single_struct{ int tag; void* f1;}; const int Cyc_PP_Append_tag=
1; struct Cyc_PP_Append_struct{ int tag; void* f1; void* f2;}; typedef void* Cyc_PP_alist_t;
void* Cyc_PP_append( void* a1, void* a2){ if( a1 == Cyc_PP_Empty){ return a2;}
else{ if( a2 == Cyc_PP_Empty){ return a1;} else{ return({ struct Cyc_PP_Append_struct*
_temp0=( struct Cyc_PP_Append_struct*) GC_malloc( sizeof( struct Cyc_PP_Append_struct));
_temp0->tag= Cyc_PP_Append_tag; _temp0->f1=( void*) a1; _temp0->f2=( void*) a2;(
void*) _temp0;});}}} struct Cyc_List_List* Cyc_PP_list_of_alist_f( void* y,
struct Cyc_List_List* l){ void* _temp1= y; void* _temp9; void* _temp11; void*
_temp13; _LL3: if( _temp1 == Cyc_PP_Empty){ goto _LL4;} else{ goto _LL5;} _LL5:
if(( unsigned int) _temp1 > 1u?(( struct _enum_struct*) _temp1)->tag == Cyc_PP_Single_tag:
0){ _LL10: _temp9=( void*)(( struct Cyc_PP_Single_struct*) _temp1)->f1; goto
_LL6;} else{ goto _LL7;} _LL7: if(( unsigned int) _temp1 > 1u?(( struct
_enum_struct*) _temp1)->tag == Cyc_PP_Append_tag: 0){ _LL14: _temp13=( void*)((
struct Cyc_PP_Append_struct*) _temp1)->f1; goto _LL12; _LL12: _temp11=( void*)((
struct Cyc_PP_Append_struct*) _temp1)->f2; goto _LL8;} else{ goto _LL2;} _LL4:
return l; _LL6: return({ struct Cyc_List_List* _temp15=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp15->hd=( void*) _temp9; _temp15->tl=
l; _temp15;}); _LL8: return(( struct Cyc_List_List*(*)( void* y, struct Cyc_List_List*
l)) Cyc_PP_list_of_alist_f)( _temp13,(( struct Cyc_List_List*(*)( void* y,
struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)( _temp11, l)); _LL2:;} struct
Cyc_List_List* Cyc_PP_list_of_alist( void* x){ return(( struct Cyc_List_List*(*)(
void* y, struct Cyc_List_List* l)) Cyc_PP_list_of_alist_f)( x, 0);} struct Cyc_PP_Ppstate{
int ci; int cc; int cl; int pw; int epw;}; struct Cyc_PP_Out{ int newcc; int
newcl; void* ppout; void* links;}; struct Cyc_PP_Doc{ int mwo; int mw; void* f;};
static void Cyc_PP_dump_out( struct Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray*
xarr=(( struct Cyc_Xarray_Xarray*(*)( int, void*)) Cyc_Xarray_create)( 16, al);((
void(*)( struct Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( xarr, al);{ int
last= 0; while( last >= 0) { void* _temp16=(( void*(*)( struct Cyc_Xarray_Xarray*,
int)) Cyc_Xarray_get)( xarr, last); struct _tagged_string* _temp24; void*
_temp26; void* _temp28; _LL18: if( _temp16 == Cyc_PP_Empty){ goto _LL19;} else{
goto _LL20;} _LL20: if(( unsigned int) _temp16 > 1u?(( struct _enum_struct*)
_temp16)->tag == Cyc_PP_Single_tag: 0){ _LL25: _temp24=( struct _tagged_string*)((
struct Cyc_PP_Single_struct*) _temp16)->f1; goto _LL21;} else{ goto _LL22;}
_LL22: if(( unsigned int) _temp16 > 1u?(( struct _enum_struct*) _temp16)->tag ==
Cyc_PP_Append_tag: 0){ _LL29: _temp28=( void*)(( struct Cyc_PP_Append_struct*)
_temp16)->f1; goto _LL27; _LL27: _temp26=( void*)(( struct Cyc_PP_Append_struct*)
_temp16)->f2; goto _LL23;} else{ goto _LL17;} _LL19: -- last; goto _LL17; _LL21:
-- last;({ struct _tagged_string _temp30=* _temp24; fprintf( f,"%.*s", _temp30.last_plus_one
- _temp30.curr, _temp30.curr);}); goto _LL17; _LL23:(( void(*)( struct Cyc_Xarray_Xarray*,
int, void*)) Cyc_Xarray_set)( xarr, last, _temp26); if( last ==(( int(*)( struct
Cyc_Xarray_Xarray*)) Cyc_Xarray_length)( xarr) - 1){(( void(*)( struct Cyc_Xarray_Xarray*,
void*)) Cyc_Xarray_add)( xarr, _temp28);} else{(( void(*)( struct Cyc_Xarray_Xarray*,
int, void*)) Cyc_Xarray_set)( xarr, last + 1, _temp28);} ++ last; goto _LL17;
_LL17:;}}} void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp31=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp31->ci= 0; _temp31->cc=
0; _temp31->cl= 1; _temp31->pw= w; _temp31->epw= w; _temp31;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp32=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp32->ci= 0; _temp32->cc=
0; _temp32->cl= 1; _temp32->pw= w; _temp32->epw= w; _temp32;})); return Cyc_String_str_sepstr(((
struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*) o->ppout),(
struct _tagged_string)({ char* _temp33=( char*)""; struct _tagged_string _temp34;
_temp34.curr= _temp33; _temp34.base= _temp33; _temp34.last_plus_one= _temp33 + 1;
_temp34;}));} struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int
w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp35=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp35->ci= 0; _temp35->cc=
0; _temp35->cl= 1; _temp35->pw= w; _temp35->epw= w; _temp35;})); return({ struct
_tuple0* _temp36=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp36->f1=
Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout),( struct _tagged_string)({ char* _temp37=( char*)""; struct
_tagged_string _temp38; _temp38.curr= _temp37; _temp38.base= _temp37; _temp38.last_plus_one=
_temp37 + 1; _temp38;})); _temp36->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->links); _temp36;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0;
struct _tagged_string Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp39=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp39->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp39;});} if( i < 0){( void) _throw(({
struct Cyc_Core_Failure_struct* _temp40=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp40=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp41=( char*)"nlblanks";
struct _tagged_string _temp42; _temp42.curr= _temp41; _temp42.base= _temp41;
_temp42.last_plus_one= _temp41 + 9; _temp42;})};( struct _xenum_struct*) _temp40;}));}{
struct _handler_cons _temp43; _push_handler(& _temp43);{ struct _xenum_struct*
_temp44=( struct _xenum_struct*) setjmp( _temp43.handler); if( ! _temp44){{
struct _tagged_string _temp45=*(( struct _tagged_string*(*)( struct Cyc_Hashtable_Table*
t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*) Cyc_PP_bhashtbl->v,
i); _npop_handler( 0u); return _temp45;} _pop_handler();} else{ struct
_xenum_struct* _temp47= _temp44; _LL49: if( _temp47->tag == Cyc_Core_Not_found_tag){
goto _LL50;} else{ goto _LL51;} _LL51: goto _LL52; _LL50: { int num_tabs= i / 8;
int num_spaces= i % 8; int total=( 1 + num_tabs) + num_spaces; struct
_tagged_string nlb= Cyc_Core_new_string( total);({ struct _tagged_string _temp53=
nlb; char* _temp55= _temp53.curr + 0; if( _temp55 < _temp53.base? 1: _temp55 >=
_temp53.last_plus_one){ _throw( Null_Exception);}* _temp55='\n';});{ int j= 0;
for( 0; j < num_tabs; j ++){({ struct _tagged_string _temp56= nlb; char* _temp58=
_temp56.curr +( j + 1); if( _temp58 < _temp56.base? 1: _temp58 >= _temp56.last_plus_one){
_throw( Null_Exception);}* _temp58='\t';});}}{ int j= 0; for( 0; j < num_spaces;
j ++){({ struct _tagged_string _temp59= nlb; char* _temp61= _temp59.curr +(( j +
1) + num_tabs); if( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one){
_throw( Null_Exception);}* _temp61=' ';});}}(( void(*)( struct Cyc_Hashtable_Table*
t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)(( struct Cyc_Hashtable_Table*)
Cyc_PP_bhashtbl->v, i,({ struct _tagged_string* _temp62=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp62[ 0]= nlb; _temp62;}));
return nlb;} _LL52:( void) _throw( _temp47); _LL48:;}}}} static struct Cyc_Core_Opt*
Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999; static struct Cyc_PP_Out*
Cyc_PP_text_doc_f( struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct
_tuple1 _temp65; struct _tagged_string _temp66; int _temp68; struct _tuple1*
_temp63= clo; _temp65=* _temp63; _LL69: _temp68= _temp65.f1; goto _LL67; _LL67:
_temp66= _temp65.f2; goto _LL64; _LL64: return({ struct Cyc_PP_Out* _temp70=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp70->newcc= st->cc
+ _temp68; _temp70->newcl= st->cl; _temp70->ppout=( void*)({ struct Cyc_PP_Single_struct*
_temp71=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp71->tag= Cyc_PP_Single_tag; _temp71->f1=( void*)({ struct _tagged_string*
_temp72=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp72[ 0]= _temp66; _temp72;});( void*) _temp71;}); _temp70->links=( void*)
Cyc_PP_Empty; _temp70;});} static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct
_tagged_string s){ int slen=( int) Cyc_String_strlen( s); return({ struct Cyc_PP_Doc*
_temp73=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp73->mwo=
slen; _temp73->mw= Cyc_PP_infinity; _temp73->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple1*, struct Cyc_PP_Ppstate*), struct _tuple1* x)) Cyc_Fn_make_fn)(
Cyc_PP_text_doc_f,({ struct _tuple1* _temp74=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp74->f1= slen; _temp74->f2= s; _temp74;}));
_temp73;});} struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s){ struct
Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl == 0){ t=(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct _tagged_string*, struct _tagged_string*), int(* hash)(
struct _tagged_string*))) Cyc_Hashtable_create)( 101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({ struct Cyc_Core_Opt* _temp75=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp75->v=( void*) t; _temp75;});}
else{ t=( struct Cyc_Hashtable_Table*) Cyc_PP_str_hashtbl->v;}{ struct
_handler_cons _temp76; _push_handler(& _temp76);{ struct _xenum_struct* _temp77=(
struct _xenum_struct*) setjmp( _temp76.handler); if( ! _temp77){{ struct Cyc_PP_Doc*
_temp79=(( struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_string* key)) Cyc_Hashtable_lookup)( t,({ struct _tagged_string* _temp78=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp78[
0]= s; _temp78;})); _npop_handler( 0u); return _temp79;} _pop_handler();} else{
struct _xenum_struct* _temp81= _temp77; _LL83: if( _temp81->tag == Cyc_Core_Not_found_tag){
goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86; _LL84: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_string* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_string* _temp87=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp87[ 0]= s; _temp87;}), d); return d;} _LL86:( void)
_throw( _temp81); _LL82:;}}}} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct
Cyc_Core_Opt* Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt=
0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt* _temp88=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp88->v=( void*) Cyc_PP_text(( struct _tagged_string)({ char*
_temp89=( char*)""; struct _tagged_string _temp90; _temp90.curr= _temp89;
_temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 1; _temp90;})); _temp88;});}
return( struct Cyc_PP_Doc*) Cyc_PP_nil_doc_opt->v;} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){
if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt*
_temp91=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp91->v=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp92=( char*)"";
struct _tagged_string _temp93; _temp93.curr= _temp92; _temp93.base= _temp92;
_temp93.last_plus_one= _temp92 + 1; _temp93;})); _temp91;});} return( struct Cyc_PP_Doc*)
Cyc_PP_blank_doc_opt->v;} static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct
_tuple2* clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp96; struct
_tagged_string _temp97; struct _tagged_string _temp99; int _temp101; struct
_tuple2* _temp94= clo; _temp96=* _temp94; _LL102: _temp101= _temp96.f1; goto
_LL100; _LL100: _temp99= _temp96.f2; goto _LL98; _LL98: _temp97= _temp96.f3;
goto _LL95; _LL95: return({ struct Cyc_PP_Out* _temp103=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp103->newcc= st->cc + _temp101;
_temp103->newcl= st->cl; _temp103->ppout=( void*)({ struct Cyc_PP_Single_struct*
_temp106=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp106->tag= Cyc_PP_Single_tag; _temp106->f1=( void*)({ struct _tagged_string*
_temp107=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp107[ 0]= _temp99; _temp107;});( void*) _temp106;}); _temp103->links=( void*)({
struct Cyc_PP_Single_struct* _temp104=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp104->tag= Cyc_PP_Single_tag;
_temp104->f1=( void*)({ struct _tuple3* _temp105=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp105->f1= st->cl; _temp105->f2= st->cc; _temp105->f3=
_temp101; _temp105->f4= _temp97; _temp105;});( void*) _temp104;}); _temp103;});}
struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt, struct
_tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({ struct
Cyc_PP_Doc* _temp108=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp108->mwo= slen; _temp108->mw= Cyc_PP_infinity; _temp108->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp109=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp109->f1= slen;
_temp109->f2= shrt; _temp109->f3= full; _temp109;})); _temp108;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp110=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp110->newcc= st->ci; _temp110->newcl= st->cl + 1; _temp110->ppout=( void*)({
struct Cyc_PP_Single_struct* _temp111=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp111->tag= Cyc_PP_Single_tag;
_temp111->f1=( void*)({ struct _tagged_string* _temp112=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp112[ 0]= Cyc_PP_nlblanks(
st->ci); _temp112;});( void*) _temp111;}); _temp110->links=( void*) Cyc_PP_Empty;
_temp110;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp113=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp113->v=( void*)({ struct Cyc_PP_Doc*
_temp114=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp114->mwo=
0; _temp114->mw= 0; _temp114->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp114;}); _temp113;});}
return( struct Cyc_PP_Doc*) Cyc_PP_line_doc_opt->v;} static struct Cyc_PP_Out*
Cyc_PP_nest_f( struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4
_temp117; struct Cyc_PP_Doc* _temp118; int _temp120; struct _tuple4* _temp115=
clo; _temp117=* _temp115; _LL121: _temp120= _temp117.f1; goto _LL119; _LL119:
_temp118= _temp117.f2; goto _LL116; _LL116: { struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp122=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp122->ci= st->ci + _temp120; _temp122->cc=
st->cc; _temp122->cl= st->cl; _temp122->pw= st->pw; _temp122->epw= st->epw;
_temp122;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp118->f, st2);}} struct Cyc_PP_Doc* Cyc_PP_nest( int
k, struct Cyc_PP_Doc* d){ return({ struct Cyc_PP_Doc* _temp123=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp123->mwo= d->mwo; _temp123->mw= d->mw;
_temp123->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple4*,
struct Cyc_PP_Ppstate*), struct _tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({
struct _tuple4* _temp124=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp124->f1= k; _temp124->f2= d; _temp124;})); _temp123;});} int Cyc_PP_min(
int x, int y){ if( x <= y){ return x;} else{ return y;}} int Cyc_PP_max( int x,
int y){ if( x >= y){ return x;} else{ return y;}} static struct Cyc_PP_Out* Cyc_PP_concat_f(
struct _tuple5* clo, struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp127; struct
Cyc_PP_Doc* _temp128; struct Cyc_PP_Doc* _temp130; struct _tuple5* _temp125= clo;
_temp127=* _temp125; _LL131: _temp130= _temp127.f1; goto _LL129; _LL129:
_temp128= _temp127.f2; goto _LL126; _LL126: { int epw2= Cyc_PP_max( st->pw -
_temp128->mw, st->epw - _temp130->mwo); struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp132=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp132->ci= st->ci; _temp132->cc= st->cc; _temp132->cl= st->cl; _temp132->pw=
st->pw; _temp132->epw= epw2; _temp132;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp130->f, st2);
struct Cyc_PP_Ppstate* st3=({ struct Cyc_PP_Ppstate* _temp133=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp133->ci= st->ci;
_temp133->cc= o1->newcc; _temp133->cl= o1->newcl; _temp133->pw= st->pw; _temp133->epw=
epw2; _temp133;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp128->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp134=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp134->newcc= o2->newcc; _temp134->newcl= o2->newcl;
_temp134->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp134->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp134;});
return o3;}} struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return({ struct Cyc_PP_Doc* _temp135=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp135->mwo= Cyc_PP_min( d1->mw, d1->mwo + d2->mwo);
_temp135->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp135->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp136=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp136->f1= d1; _temp136->f2=
d2; _temp136;})); _temp135;});} static struct Cyc_PP_Out* Cyc_PP_long_concats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_PP_Doc*
d=( struct Cyc_PP_Doc*) ds->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp137=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp137->ci= st->ci; _temp137->cc= o->newcc;
_temp137->cl= o->newcl; _temp137->pw= st->pw; _temp137->epw= st->epw - d->mwo;
_temp137;}); os=({ struct Cyc_List_List* _temp138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp138->hd=( void*) o; _temp138->tl=
os; _temp138;});}}{ int newcc=(( struct Cyc_PP_Out*) os->hd)->newcc; int newcl=((
struct Cyc_PP_Out*) os->hd)->newcl; void* s= Cyc_PP_Empty; void* links= Cyc_PP_Empty;
for( 0; os != 0; os= os->tl){ s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*) os->hd)->ppout, s); links=(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*) os->hd)->links, links);}
return({ struct Cyc_PP_Out* _temp139=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp139->newcc= newcc; _temp139->newcl= newcl; _temp139->ppout=(
void*) s; _temp139->links=( void*) links; _temp139;});}} static struct Cyc_PP_Doc*
Cyc_PP_long_concats( struct Cyc_List_List* doclist){ struct Cyc_List_List* orig=
doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*) doclist->hd; doclist=
doclist->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List* ds= doclist;
for( 0; ds != 0; ds= ds->tl){ int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)
ds->hd)->mwo); int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*) ds->hd)->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp140=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp140->mwo= mw; _temp140->mw= mwo;
_temp140->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*,
struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_concats_f,
orig); _temp140;});}} struct Cyc_PP_Doc* Cyc_PP_concats( struct Cyc_List_List*
doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();} else{ if( doclist->tl ==
0){ return( struct Cyc_PP_Doc*) doclist->hd;} else{ if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( doclist) > 30){ return Cyc_PP_long_concats( doclist);}
else{ return Cyc_PP_concat(( struct Cyc_PP_Doc*) doclist->hd, Cyc_PP_concats(
doclist->tl));}}}} static struct Cyc_PP_Out* Cyc_PP_concats_arr_f( struct
_tagged_ptr0* docs_ptr, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0;
struct _tagged_ptr0 docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0
_temp141= docs;( unsigned int)( _temp141.last_plus_one - _temp141.curr);});{ int
i= 0; for( 0; i < sz; ++ i){ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0
_temp142= docs; struct Cyc_PP_Doc** _temp144= _temp142.curr + i; if( _temp144 <
_temp142.base? 1: _temp144 >= _temp142.last_plus_one){ _throw( Null_Exception);}*
_temp144;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp145=(
struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp145->ci= st->ci; _temp145->cc= o->newcc; _temp145->cl= o->newcl; _temp145->pw=
st->pw; _temp145->epw= st->epw - d->mwo; _temp145;}); os=({ struct Cyc_List_List*
_temp146=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp146->hd=( void*) o; _temp146->tl= os; _temp146;});}}{ int newcc=(( struct
Cyc_PP_Out*) os->hd)->newcc; int newcl=(( struct Cyc_PP_Out*) os->hd)->newcl;
void* s= Cyc_PP_Empty; void* links= Cyc_PP_Empty; for( 0; os != 0; os= os->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)
os->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*) os->hd)->links, links);} return({ struct Cyc_PP_Out*
_temp147=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp147->newcc=
newcc; _temp147->newcl= newcl; _temp147->ppout=( void*) s; _temp147->links=(
void*) links; _temp147;});}} struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct
_tagged_ptr0 docs){ int sz=( int)({ struct _tagged_ptr0 _temp148= docs;(
unsigned int)( _temp148.last_plus_one - _temp148.curr);}); if( sz == 0){( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp149=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp149=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp150=( char*)"concats_arr -- size zero array";
struct _tagged_string _temp151; _temp151.curr= _temp150; _temp151.base= _temp150;
_temp151.last_plus_one= _temp150 + 31; _temp151;})};( struct _xenum_struct*)
_temp149;}));}{ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp152= docs;
struct Cyc_PP_Doc** _temp154= _temp152.curr + 0; if( _temp154 < _temp152.base? 1:
_temp154 >= _temp152.last_plus_one){ _throw( Null_Exception);}* _temp154;}); int
mw= d->mw; int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min(
mw, mwo +({ struct _tagged_ptr0 _temp155= docs; struct Cyc_PP_Doc** _temp157=
_temp155.curr + i; if( _temp157 < _temp155.base? 1: _temp157 >= _temp155.last_plus_one){
_throw( Null_Exception);}* _temp157;})->mwo); int mwo2= Cyc_PP_min( mw, mwo +({
struct _tagged_ptr0 _temp158= docs; struct Cyc_PP_Doc** _temp160= _temp158.curr
+ i; if( _temp160 < _temp158.base? 1: _temp160 >= _temp158.last_plus_one){
_throw( Null_Exception);}* _temp160;})->mw); mw= mw2; mwo= mwo2;}} return({
struct Cyc_PP_Doc* _temp161=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp161->mwo= mw; _temp161->mw= mwo; _temp161->f=( void*)(( void*(*)( struct
Cyc_PP_Out*(* f)( struct _tagged_ptr0*, struct Cyc_PP_Ppstate*), struct
_tagged_ptr0* x)) Cyc_Fn_make_fn)( Cyc_PP_concats_arr_f,({ struct _tagged_ptr0*
_temp162=( struct _tagged_ptr0*) GC_malloc( sizeof( struct _tagged_ptr0) * 1);
_temp162[ 0]= docs; _temp162;})); _temp161;});}} static struct Cyc_PP_Out* Cyc_PP_doc_union_f(
struct _tuple5* clo, struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp165; struct
Cyc_PP_Doc* _temp166; struct Cyc_PP_Doc* _temp168; struct _tuple5* _temp163= clo;
_temp165=* _temp163; _LL169: _temp168= _temp165.f1; goto _LL167; _LL167:
_temp166= _temp165.f2; goto _LL164; _LL164: { int dfits= st->cc + _temp168->mwo
<= st->epw? 1: st->cc + _temp168->mw <= st->pw; if( dfits){ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp168->f, st);}
else{ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) _temp166->f, st);}}} struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc*
d, struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp170=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp170->mwo= Cyc_PP_min( d->mwo, d2->mwo);
_temp170->mw= Cyc_PP_min( d->mw, d2->mw); _temp170->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_doc_union_f,({ struct _tuple5* _temp171=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp171->f1= d; _temp171->f2=
d2; _temp171;})); _temp170;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return
Cyc_PP_doc_union( Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out*
Cyc_PP_tab_f( struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate*
st2=({ struct Cyc_PP_Ppstate* _temp172=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp172->ci= st->cc;
_temp172->cc= st->cc; _temp172->cl= st->cl; _temp172->pw= st->pw; _temp172->epw=
st->epw; _temp172;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct
Cyc_PP_Doc* d){ struct Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp173=( struct
Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp173->mwo= d->mwo;
_temp173->mw= d->mw; _temp173->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Doc*, struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)(
Cyc_PP_tab_f, d); _temp173;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if( l->tl == 0){ return pp((
void*) l->hd);} else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp174=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp174->hd=(
void*) pp(( void*) l->hd); _temp174->tl=({ struct Cyc_List_List* _temp175=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp175->hd=(
void*) Cyc_PP_text( sep); _temp175->tl=({ struct Cyc_List_List* _temp176=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp176->hd=(
void*) Cyc_PP_oline_doc(); _temp176->tl=({ struct Cyc_List_List* _temp177=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp177->hd=(
void*)(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep, l->tl);
_temp177->tl= 0; _temp177;}); _temp176;}); _temp175;}); _temp174;}));}}} struct
Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq_f)( pp, sep, l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct
_tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if( l->tl == 0){ return( struct Cyc_PP_Doc*) l->hd;} else{ struct Cyc_PP_Doc*
sep2= Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct
Cyc_List_List* x= l; while( x->tl != 0) { struct Cyc_List_List* temp= x->tl; x->tl=({
struct Cyc_List_List* _temp178=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp178->hd=( void*) sep2; _temp178->tl=({ struct Cyc_List_List*
_temp179=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp179->hd=( void*) oline; _temp179->tl= temp; _temp179;}); _temp178;}); x=
temp;} return Cyc_PP_concats( l);}}} struct Cyc_PP_Doc* Cyc_PP_seq( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seq_f(
sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if( l->tl == 0){ return pp(( void*) l->hd);} else{ return Cyc_PP_concats(({
struct Cyc_List_List* _temp180=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp180->hd=( void*) pp(( void*) l->hd); _temp180->tl=({
struct Cyc_List_List* _temp181=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp181->hd=( void*) Cyc_PP_text( sep); _temp181->tl=({
struct Cyc_List_List* _temp182=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp182->hd=( void*) Cyc_PP_line_doc(); _temp182->tl=({
struct Cyc_List_List* _temp183=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp183->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseql_f)( pp, sep, l->tl); _temp183->tl= 0; _temp183;}); _temp182;});
_temp181;}); _temp180;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string
sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep, l0));} static struct
Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_string sep, struct Cyc_List_List* l){
if( l == 0){ return Cyc_PP_nil_doc();} else{ if( l->tl == 0){ return( struct Cyc_PP_Doc*)
l->hd;} else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp184=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp184->hd=( void*)((
struct Cyc_PP_Doc*) l->hd); _temp184->tl=({ struct Cyc_List_List* _temp185=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=(
void*) Cyc_PP_text( sep); _temp185->tl=({ struct Cyc_List_List* _temp186=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp186->hd=(
void*) Cyc_PP_line_doc(); _temp186->tl=({ struct Cyc_List_List* _temp187=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp187->hd=(
void*) Cyc_PP_seql_f( sep, l->tl); _temp187->tl= 0; _temp187;}); _temp186;});
_temp185;}); _temp184;}));}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));}