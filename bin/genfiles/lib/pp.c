#include "cyc_include.h"

 struct _tuple0{ struct _tagged_string f1; struct Cyc_List_List* f2; } ; struct
_tagged_ptr0{ struct Cyc_PP_Doc** curr; struct Cyc_PP_Doc** base; struct Cyc_PP_Doc**
last_plus_one; } ; struct _tagged_ptr1{ void** curr; void** base; void**
last_plus_one; } ; struct _tuple1{ int f1; struct _tagged_string f2; } ; struct
_tuple2{ int f1; struct _tagged_string f2; struct _tagged_string f3; } ; struct
_tuple3{ int f1; int f2; int f3; struct _tagged_string f4; } ; struct _tuple4{
int f1; struct Cyc_PP_Doc* f2; } ; struct _tuple5{ struct Cyc_PP_Doc* f1; struct
Cyc_PP_Doc* f2; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t;
typedef int Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct
_tagged_string Cyc_Core_new_string( int); extern void* Cyc_Core_identity( void*);
extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
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
if( ! _temp46){{ struct _tagged_string _temp48=*(( struct _tagged_string*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)({
struct Cyc_Core_Opt* _temp47= Cyc_PP_bhashtbl; if( _temp47 == 0){ _throw(
Null_Exception);} _temp47->v;}), i); _npop_handler( 0u); return _temp48;};
_pop_handler();} else{ struct _xtunion_struct* _temp50= _temp46; _LL52: if(
_temp50->tag == Cyc_Core_Not_found_tag){ goto _LL53;} else{ goto _LL54;} _LL54:
goto _LL55; _LL53: { int num_tabs= i / 8; int num_spaces= i % 8; int total=( 1 +
num_tabs) + num_spaces; struct _tagged_string nlb= Cyc_Core_new_string( total);({
struct _tagged_string _temp56= nlb; char* _temp58= _temp56.curr + 0; if( _temp56.base
== 0? 1:( _temp58 < _temp56.base? 1: _temp58 >= _temp56.last_plus_one)){ _throw(
Null_Exception);}* _temp58='\n';});{ int j= 0; for( 0; j < num_tabs; j ++){({
struct _tagged_string _temp59= nlb; char* _temp61= _temp59.curr +( j + 1); if(
_temp59.base == 0? 1:( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one)){
_throw( Null_Exception);}* _temp61='\t';});}}{ int j= 0; for( 0; j < num_spaces;
j ++){({ struct _tagged_string _temp62= nlb; char* _temp64= _temp62.curr +(( j +
1) + num_tabs); if( _temp62.base == 0? 1:( _temp64 < _temp62.base? 1: _temp64 >=
_temp62.last_plus_one)){ _throw( Null_Exception);}* _temp64=' ';});}}(( void(*)(
struct Cyc_Hashtable_Table* t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt* _temp65= Cyc_PP_bhashtbl;
if( _temp65 == 0){ _throw( Null_Exception);} _temp65->v;}), i,({ struct
_tagged_string* _temp66=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp66[ 0]= nlb; _temp66;})); return nlb;} _LL55:( void)
_throw( _temp50); _LL51:;}}}} static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0;
int Cyc_PP_infinity= 9999999; static struct Cyc_PP_Out* Cyc_PP_text_doc_f(
struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct _tuple1 _temp69; struct
_tagged_string _temp70; int _temp72; struct _tuple1* _temp67= clo; _temp69=*
_temp67; _LL73: _temp72= _temp69.f1; goto _LL71; _LL71: _temp70= _temp69.f2;
goto _LL68; _LL68: return({ struct Cyc_PP_Out* _temp74=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp74->newcc= st->cc + _temp72;
_temp74->newcl= st->cl; _temp74->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp75=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp75[ 0]=({ struct Cyc_PP_Single_struct _temp76; _temp76.tag= Cyc_PP_Single_tag;
_temp76.f1=( void*)({ struct _tagged_string* _temp77=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp77[ 0]= _temp70; _temp77;});
_temp76;}); _temp75;})); _temp74->links=( void*)(( void*) Cyc_PP_Empty); _temp74;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_string s){ int slen=(
int) Cyc_String_strlen( s); return({ struct Cyc_PP_Doc* _temp78=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp78->mwo= slen; _temp78->mw= Cyc_PP_infinity;
_temp78->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct
Cyc_PP_Ppstate*), struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({
struct _tuple1* _temp79=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp79->f1= slen; _temp79->f2= s; _temp79;})); _temp78;});} struct Cyc_PP_Doc*
Cyc_PP_text( struct _tagged_string s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl
== 0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct
_tagged_string*, struct _tagged_string*), int(* hash)( struct _tagged_string*)))
Cyc_Hashtable_create)( 101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({ struct Cyc_Core_Opt* _temp80=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp80->v=( void*) t; _temp80;});}
else{ t=( struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt* _temp81= Cyc_PP_str_hashtbl;
if( _temp81 == 0){ _throw( Null_Exception);} _temp81->v;});}{ struct
_handler_cons _temp82; _push_handler(& _temp82);{ struct _xtunion_struct*
_temp83=( struct _xtunion_struct*) setjmp( _temp82.handler); if( ! _temp83){{
struct Cyc_PP_Doc* _temp85=(( struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_string* key)) Cyc_Hashtable_lookup)( t,({ struct
_tagged_string* _temp84=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp84[ 0]= s; _temp84;})); _npop_handler( 0u); return
_temp85;}; _pop_handler();} else{ struct _xtunion_struct* _temp87= _temp83;
_LL89: if( _temp87->tag == Cyc_Core_Not_found_tag){ goto _LL90;} else{ goto
_LL91;} _LL91: goto _LL92; _LL90: { struct Cyc_PP_Doc* d= Cyc_PP_text_doc( s);((
void(*)( struct Cyc_Hashtable_Table* t, struct _tagged_string* key, struct Cyc_PP_Doc*
val)) Cyc_Hashtable_insert)( t,({ struct _tagged_string* _temp93=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp93[ 0]= s;
_temp93;}), d); return d;} _LL92:( void) _throw( _temp87); _LL88:;}}}} struct
Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt= 0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){
if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({ struct Cyc_Core_Opt* _temp94=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp94->v=(
void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp95=( char*)""; struct
_tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95; _temp96.last_plus_one=
_temp95 + 1; _temp96;})); _temp94;});} return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt*
_temp97= Cyc_PP_nil_doc_opt; if( _temp97 == 0){ _throw( Null_Exception);}
_temp97->v;});} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt
== 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt* _temp98=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp98->v=( void*) Cyc_PP_text((
struct _tagged_string)({ char* _temp99=( char*)""; struct _tagged_string
_temp100; _temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one=
_temp99 + 1; _temp100;})); _temp98;});} return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt*
_temp101= Cyc_PP_blank_doc_opt; if( _temp101 == 0){ _throw( Null_Exception);}
_temp101->v;});} static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct _tuple2*
clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp104; struct _tagged_string
_temp105; struct _tagged_string _temp107; int _temp109; struct _tuple2* _temp102=
clo; _temp104=* _temp102; _LL110: _temp109= _temp104.f1; goto _LL108; _LL108:
_temp107= _temp104.f2; goto _LL106; _LL106: _temp105= _temp104.f3; goto _LL103;
_LL103: return({ struct Cyc_PP_Out* _temp111=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp111->newcc= st->cc + _temp109; _temp111->newcl=
st->cl; _temp111->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp115=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp115[ 0]=({ struct Cyc_PP_Single_struct _temp116; _temp116.tag= Cyc_PP_Single_tag;
_temp116.f1=( void*)({ struct _tagged_string* _temp117=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp117[ 0]= _temp107; _temp117;});
_temp116;}); _temp115;})); _temp111->links=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp112=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp112[ 0]=({ struct Cyc_PP_Single_struct _temp113; _temp113.tag= Cyc_PP_Single_tag;
_temp113.f1=( void*)({ struct _tuple3* _temp114=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp114->f1= st->cl; _temp114->f2= st->cc; _temp114->f3=
_temp109; _temp114->f4= _temp105; _temp114;}); _temp113;}); _temp112;}));
_temp111;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt,
struct _tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({
struct Cyc_PP_Doc* _temp118=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp118->mwo= slen; _temp118->mw= Cyc_PP_infinity; _temp118->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp119=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp119->f1= slen;
_temp119->f2= shrt; _temp119->f3= full; _temp119;})); _temp118;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp120=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp120->newcc= st->ci; _temp120->newcl= st->cl + 1; _temp120->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp121=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp121[ 0]=({ struct Cyc_PP_Single_struct
_temp122; _temp122.tag= Cyc_PP_Single_tag; _temp122.f1=( void*)({ struct
_tagged_string* _temp123=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp123[ 0]= Cyc_PP_nlblanks( st->ci); _temp123;});
_temp122;}); _temp121;})); _temp120->links=( void*)(( void*) Cyc_PP_Empty);
_temp120;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp124=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp124->v=( void*)({ struct Cyc_PP_Doc*
_temp125=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp125->mwo=
0; _temp125->mw= 0; _temp125->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp125;}); _temp124;});}
return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt* _temp126= Cyc_PP_line_doc_opt;
if( _temp126 == 0){ _throw( Null_Exception);} _temp126->v;});} static struct Cyc_PP_Out*
Cyc_PP_nest_f( struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4
_temp129; struct Cyc_PP_Doc* _temp130; int _temp132; struct _tuple4* _temp127=
clo; _temp129=* _temp127; _LL133: _temp132= _temp129.f1; goto _LL131; _LL131:
_temp130= _temp129.f2; goto _LL128; _LL128: { struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp134=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp134->ci= st->ci + _temp132; _temp134->cc=
st->cc; _temp134->cl= st->cl; _temp134->pw= st->pw; _temp134->epw= st->epw;
_temp134;}); return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp130->f, st2);}} struct Cyc_PP_Doc* Cyc_PP_nest( int
k, struct Cyc_PP_Doc* d){ return({ struct Cyc_PP_Doc* _temp135=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp135->mwo= d->mwo; _temp135->mw= d->mw;
_temp135->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple4*,
struct Cyc_PP_Ppstate*), struct _tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({
struct _tuple4* _temp136=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp136->f1= k; _temp136->f2= d; _temp136;})); _temp135;});} int Cyc_PP_min(
int x, int y){ if( x <= y){ return x;} else{ return y;}} int Cyc_PP_max( int x,
int y){ if( x >= y){ return x;} else{ return y;}} static struct Cyc_PP_Out* Cyc_PP_concat_f(
struct _tuple5* clo, struct Cyc_PP_Ppstate* st){ struct _tuple5 _temp139; struct
Cyc_PP_Doc* _temp140; struct Cyc_PP_Doc* _temp142; struct _tuple5* _temp137= clo;
_temp139=* _temp137; _LL143: _temp142= _temp139.f1; goto _LL141; _LL141:
_temp140= _temp139.f2; goto _LL138; _LL138: { int epw2= Cyc_PP_max( st->pw -
_temp140->mw, st->epw - _temp142->mwo); struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp144=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp144->ci= st->ci; _temp144->cc= st->cc; _temp144->cl= st->cl; _temp144->pw=
st->pw; _temp144->epw= epw2; _temp144;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp142->f, st2);
struct Cyc_PP_Ppstate* st3=({ struct Cyc_PP_Ppstate* _temp145=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp145->ci= st->ci;
_temp145->cc= o1->newcc; _temp145->cl= o1->newcl; _temp145->pw= st->pw; _temp145->epw=
epw2; _temp145;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp140->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp146=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp146->newcc= o2->newcc; _temp146->newcl= o2->newcl;
_temp146->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp146->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp146;});
return o3;}} struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return({ struct Cyc_PP_Doc* _temp147=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp147->mwo= Cyc_PP_min( d1->mw, d1->mwo + d2->mwo);
_temp147->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp147->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp148=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp148->f1= d1; _temp148->f2=
d2; _temp148;})); _temp147;});} static struct Cyc_PP_Out* Cyc_PP_long_concats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp149= ds; if( _temp149 == 0){ _throw( Null_Exception);} _temp149->tl;})){
struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp150= ds;
if( _temp150 == 0){ _throw( Null_Exception);} _temp150->hd;}); struct Cyc_PP_Out*
o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp151=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp151->ci= st->ci;
_temp151->cc= o->newcc; _temp151->cl= o->newcl; _temp151->pw= st->pw; _temp151->epw=
st->epw - d->mwo; _temp151;}); os=({ struct Cyc_List_List* _temp152=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp152->hd=( void*) o; _temp152->tl=
os; _temp152;});}}{ int newcc=(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp153= os; if( _temp153 == 0){ _throw( Null_Exception);} _temp153->hd;}))->newcc;
int newcl=(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp154= os; if(
_temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}))->newcl; void* s=(
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
struct Cyc_List_List* _temp159= doclist; if( _temp159 == 0){ _throw(
Null_Exception);} _temp159->hd;}); doclist=({ struct Cyc_List_List* _temp160=
doclist; if( _temp160 == 0){ _throw( Null_Exception);} _temp160->tl;});{ int mw=
d->mw; int mwo= d->mw;{ struct Cyc_List_List* ds= doclist; for( 0; ds != 0; ds=({
struct Cyc_List_List* _temp161= ds; if( _temp161 == 0){ _throw( Null_Exception);}
_temp161->tl;})){ int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({ struct
Cyc_List_List* _temp162= ds; if( _temp162 == 0){ _throw( Null_Exception);}
_temp162->hd;}))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp163= ds; if( _temp163 == 0){ _throw( Null_Exception);}
_temp163->hd;}))->mw); mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc*
_temp164=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp164->mwo=
mw; _temp164->mw= mwo; _temp164->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_List_List*, struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)(
Cyc_PP_long_concats_f, orig); _temp164;});}} struct Cyc_PP_Doc* Cyc_PP_concats(
struct Cyc_List_List* doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();}
else{ if(({ struct Cyc_List_List* _temp165= doclist; if( _temp165 == 0){ _throw(
Null_Exception);} _temp165->tl;}) == 0){ return( struct Cyc_PP_Doc*)({ struct
Cyc_List_List* _temp166= doclist; if( _temp166 == 0){ _throw( Null_Exception);}
_temp166->hd;});} else{ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
doclist) > 30){ return Cyc_PP_long_concats( doclist);} else{ return Cyc_PP_concat((
struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp167= doclist; if( _temp167 == 0){
_throw( Null_Exception);} _temp167->hd;}), Cyc_PP_concats(({ struct Cyc_List_List*
_temp168= doclist; if( _temp168 == 0){ _throw( Null_Exception);} _temp168->tl;})));}}}}
static struct Cyc_PP_Out* Cyc_PP_concats_arr_f( struct _tagged_ptr0* docs_ptr,
struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0; struct _tagged_ptr0
docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0 _temp169= docs;(
unsigned int)( _temp169.last_plus_one - _temp169.curr);});{ int i= 0; for( 0; i
< sz; ++ i){ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp170= docs; struct
Cyc_PP_Doc** _temp172= _temp170.curr + i; if( _temp170.base == 0? 1:( _temp172 <
_temp170.base? 1: _temp172 >= _temp170.last_plus_one)){ _throw( Null_Exception);}*
_temp172;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate* _temp173=(
struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp173->ci= st->ci; _temp173->cc= o->newcc; _temp173->cl= o->newcl; _temp173->pw=
st->pw; _temp173->epw= st->epw - d->mwo; _temp173;}); os=({ struct Cyc_List_List*
_temp174=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp174->hd=( void*) o; _temp174->tl= os; _temp174;});}}{ int newcc=(( struct
Cyc_PP_Out*)({ struct Cyc_List_List* _temp175= os; if( _temp175 == 0){ _throw(
Null_Exception);} _temp175->hd;}))->newcc; int newcl=(( struct Cyc_PP_Out*)({
struct Cyc_List_List* _temp176= os; if( _temp176 == 0){ _throw( Null_Exception);}
_temp176->hd;}))->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty;
for( 0; os != 0; os=({ struct Cyc_List_List* _temp177= os; if( _temp177 == 0){
_throw( Null_Exception);} _temp177->tl;})){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp178=
os; if( _temp178 == 0){ _throw( Null_Exception);} _temp178->hd;}))->ppout, s);
links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({
struct Cyc_List_List* _temp179= os; if( _temp179 == 0){ _throw( Null_Exception);}
_temp179->hd;}))->links, links);} return({ struct Cyc_PP_Out* _temp180=( struct
Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp180->newcc= newcc;
_temp180->newcl= newcl; _temp180->ppout=( void*) s; _temp180->links=( void*)
links; _temp180;});}} struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct _tagged_ptr0
docs){ int sz=( int)({ struct _tagged_ptr0 _temp181= docs;( unsigned int)(
_temp181.last_plus_one - _temp181.curr);}); if( sz == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp182=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp182[ 0]=({ struct Cyc_Core_Failure_struct
_temp183; _temp183.tag= Cyc_Core_Failure_tag; _temp183.f1=( struct
_tagged_string)({ char* _temp184=( char*)"concats_arr -- size zero array";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 31; _temp185;}); _temp183;}); _temp182;}));}{
struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp186= docs; struct Cyc_PP_Doc**
_temp188= _temp186.curr + 0; if( _temp186.base == 0? 1:( _temp188 < _temp186.base?
1: _temp188 >= _temp186.last_plus_one)){ _throw( Null_Exception);}* _temp188;});
int mw= d->mw; int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min(
mw, mwo +({ struct _tagged_ptr0 _temp189= docs; struct Cyc_PP_Doc** _temp191=
_temp189.curr + i; if( _temp189.base == 0? 1:( _temp191 < _temp189.base? 1:
_temp191 >= _temp189.last_plus_one)){ _throw( Null_Exception);}* _temp191;})->mwo);
int mwo2= Cyc_PP_min( mw, mwo +({ struct _tagged_ptr0 _temp192= docs; struct Cyc_PP_Doc**
_temp194= _temp192.curr + i; if( _temp192.base == 0? 1:( _temp194 < _temp192.base?
1: _temp194 >= _temp192.last_plus_one)){ _throw( Null_Exception);}* _temp194;})->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp195=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp195->mwo= mw; _temp195->mw= mwo;
_temp195->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_ptr0*,
struct Cyc_PP_Ppstate*), struct _tagged_ptr0* x)) Cyc_Fn_make_fn)( Cyc_PP_concats_arr_f,({
struct _tagged_ptr0* _temp196=( struct _tagged_ptr0*) GC_malloc( sizeof( struct
_tagged_ptr0) * 1); _temp196[ 0]= docs; _temp196;})); _temp195;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp199; struct Cyc_PP_Doc* _temp200; struct Cyc_PP_Doc*
_temp202; struct _tuple5* _temp197= clo; _temp199=* _temp197; _LL203: _temp202=
_temp199.f1; goto _LL201; _LL201: _temp200= _temp199.f2; goto _LL198; _LL198: {
int dfits= st->cc + _temp202->mwo <= st->epw? 1: st->cc + _temp202->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp202->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp200->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp204=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp204->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp204->mw=
Cyc_PP_min( d->mw, d2->mw); _temp204->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp205=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp205->f1= d; _temp205->f2= d2; _temp205;}));
_temp204;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp206=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp206->ci= st->cc; _temp206->cc= st->cc;
_temp206->cl= st->cl; _temp206->pw= st->pw; _temp206->epw= st->epw; _temp206;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp207=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp207->mwo= d->mwo; _temp207->mw= d->mw;
_temp207->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp207;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List* _temp208= l; if(
_temp208 == 0){ _throw( Null_Exception);} _temp208->tl;}) == 0){ return pp((
void*)({ struct Cyc_List_List* _temp209= l; if( _temp209 == 0){ _throw(
Null_Exception);} _temp209->hd;}));} else{ return Cyc_PP_concats(({ struct Cyc_List_List*
_temp210=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp210->hd=( void*) pp(( void*)({ struct Cyc_List_List* _temp215= l; if(
_temp215 == 0){ _throw( Null_Exception);} _temp215->hd;})); _temp210->tl=({
struct Cyc_List_List* _temp211=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp211->hd=( void*) Cyc_PP_text( sep); _temp211->tl=({
struct Cyc_List_List* _temp212=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp212->hd=( void*) Cyc_PP_oline_doc(); _temp212->tl=({
struct Cyc_List_List* _temp213=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp213->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseq_f)( pp, sep,({ struct Cyc_List_List* _temp214= l; if( _temp214 == 0){
_throw( Null_Exception);} _temp214->tl;})); _temp213->tl= 0; _temp213;});
_temp212;}); _temp211;}); _temp210;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseq(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep,
l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List*
_temp216= l; if( _temp216 == 0){ _throw( Null_Exception);} _temp216->tl;}) == 0){
return( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp217= l; if( _temp217 ==
0){ _throw( Null_Exception);} _temp217->hd;});} else{ struct Cyc_PP_Doc* sep2=
Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List*
x= l; while(({ struct Cyc_List_List* _temp218= x; if( _temp218 == 0){ _throw(
Null_Exception);} _temp218->tl;}) != 0) { struct Cyc_List_List* temp=({ struct
Cyc_List_List* _temp219= x; if( _temp219 == 0){ _throw( Null_Exception);}
_temp219->tl;});({ struct Cyc_List_List* _temp220= x; if( _temp220 == 0){ _throw(
Null_Exception);} _temp220->tl=({ struct Cyc_List_List* _temp221=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp221->hd=( void*) sep2; _temp221->tl=({
struct Cyc_List_List* _temp222=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp222->hd=( void*) oline; _temp222->tl= temp;
_temp222;}); _temp221;});}); x= temp;} return Cyc_PP_concats( l);}}} struct Cyc_PP_Doc*
Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(
Cyc_PP_seq_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List* _temp223= l; if(
_temp223 == 0){ _throw( Null_Exception);} _temp223->tl;}) == 0){ return pp((
void*)({ struct Cyc_List_List* _temp224= l; if( _temp224 == 0){ _throw(
Null_Exception);} _temp224->hd;}));} else{ return Cyc_PP_concats(({ struct Cyc_List_List*
_temp225=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp225->hd=( void*) pp(( void*)({ struct Cyc_List_List* _temp230= l; if(
_temp230 == 0){ _throw( Null_Exception);} _temp230->hd;})); _temp225->tl=({
struct Cyc_List_List* _temp226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp226->hd=( void*) Cyc_PP_text( sep); _temp226->tl=({
struct Cyc_List_List* _temp227=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp227->hd=( void*) Cyc_PP_line_doc(); _temp227->tl=({
struct Cyc_List_List* _temp228=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp228->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseql_f)( pp, sep,({ struct Cyc_List_List* _temp229= l; if( _temp229 ==
0){ _throw( Null_Exception);} _temp229->tl;})); _temp228->tl= 0; _temp228;});
_temp227;}); _temp226;}); _temp225;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseql(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep,
l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_string sep,
struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({
struct Cyc_List_List* _temp231= l; if( _temp231 == 0){ _throw( Null_Exception);}
_temp231->tl;}) == 0){ return( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp232= l; if( _temp232 == 0){ _throw( Null_Exception);} _temp232->hd;});}
else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp233=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp233->hd=( void*)(( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp238= l; if( _temp238 == 0){ _throw( Null_Exception);}
_temp238->hd;})); _temp233->tl=({ struct Cyc_List_List* _temp234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp234->hd=( void*) Cyc_PP_text(
sep); _temp234->tl=({ struct Cyc_List_List* _temp235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp235->hd=( void*) Cyc_PP_line_doc();
_temp235->tl=({ struct Cyc_List_List* _temp236=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp236->hd=( void*) Cyc_PP_seql_f(
sep,({ struct Cyc_List_List* _temp237= l; if( _temp237 == 0){ _throw(
Null_Exception);} _temp237->tl;})); _temp236->tl= 0; _temp236;}); _temp235;});
_temp234;}); _temp233;}));}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));}