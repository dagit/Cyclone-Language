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
int); extern void* Cyc_Core_identity( void*); extern int Cyc_Core_intcmp( int,
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
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
-- last;({ struct _tagged_string _temp33=* _temp27; fprintf( f,"%.*s", _temp33.last_plus_one
- _temp33.curr, _temp33.curr);}); goto _LL20; _LL26:(( void(*)( struct Cyc_Xarray_Xarray*,
int, void*)) Cyc_Xarray_set)( _temp17, last, _temp29); if( last ==(( int(*)(
struct Cyc_Xarray_Xarray*)) Cyc_Xarray_length)( _temp17) - 1){(( void(*)( struct
Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( _temp17, _temp31);} else{(( void(*)(
struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)( _temp17, last + 1,
_temp31);} ++ last; goto _LL20; _LL20:;}}} void Cyc_PP_file_of_doc( struct Cyc_PP_Doc*
d, int w, struct Cyc_Stdio___sFILE* f){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate*
_temp34=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp34->ci= 0; _temp34->cc= 0; _temp34->cl= 1; _temp34->pw= w; _temp34->epw= w;
_temp34;})); Cyc_PP_dump_out( f,( void*) o->ppout);} struct _tagged_string Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate*
_temp37=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp37->ci= 0; _temp37->cc= 0; _temp37->cl= 1; _temp37->pw= w; _temp37->epw= w;
_temp37;})); return Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x))
Cyc_PP_list_of_alist)(( void*) o->ppout),( struct _tagged_string)({ char*
_temp35=( char*)""; struct _tagged_string _temp36; _temp36.curr= _temp35;
_temp36.base= _temp35; _temp36.last_plus_one= _temp35 + 1; _temp36;}));} struct
_tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out*
o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f,({ struct Cyc_PP_Ppstate* _temp41=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp41->ci= 0; _temp41->cc=
0; _temp41->cl= 1; _temp41->pw= w; _temp41->epw= w; _temp41;})); return({ struct
_tuple0* _temp38=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp38->f1=
Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout),( struct _tagged_string)({ char* _temp39=( char*)""; struct
_tagged_string _temp40; _temp40.curr= _temp39; _temp40.base= _temp39; _temp40.last_plus_one=
_temp39 + 1; _temp40;})); _temp38->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->links); _temp38;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0;
struct _tagged_string Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp42=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp42->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp42;});} if( i < 0){( void) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp43=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp43[ 0]=({ struct Cyc_Core_Failure_struct
_temp44; _temp44.tag= Cyc_Core_Failure; _temp44.f1=( struct _tagged_string)({
char* _temp45=( char*)"nlblanks"; struct _tagged_string _temp46; _temp46.curr=
_temp45; _temp46.base= _temp45; _temp46.last_plus_one= _temp45 + 9; _temp46;});
_temp44;}); _temp43;}));}{ struct _handler_cons _temp47; _push_handler(& _temp47);{
int _temp49= 0; if( setjmp( _temp47.handler)){ _temp49= 1;} if( ! _temp49){{
struct _tagged_string _temp51=*(( struct _tagged_string*(*)( struct Cyc_Hashtable_Table*
t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt*
_temp50= Cyc_PP_bhashtbl; if( _temp50 == 0){ _throw( Null_Exception);} _temp50->v;}),
i); _npop_handler( 0u); return _temp51;}; _pop_handler();} else{ void* _temp48=(
void*) _exn_thrown; void* _temp53= _temp48; _LL55: if( _temp53 == Cyc_Core_Not_found){
goto _LL56;} else{ goto _LL57;} _LL57: goto _LL58; _LL56: { int _temp59= i / 8;
goto _LL60; _LL60: { int _temp61= i % 8; goto _LL62; _LL62: { int _temp63=( 1 +
_temp59) + _temp61; goto _LL64; _LL64: { struct _tagged_string nlb= Cyc_Core_new_string(
_temp63);({ struct _tagged_string _temp65= nlb; char* _temp67= _temp65.curr + 0;
if( _temp65.base == 0? 1:( _temp67 < _temp65.base? 1: _temp67 >= _temp65.last_plus_one)){
_throw( Null_Exception);}* _temp67='\n';});{ int j= 0; for( 0; j < _temp59; j ++){({
struct _tagged_string _temp68= nlb; char* _temp70= _temp68.curr +( j + 1); if(
_temp68.base == 0? 1:( _temp70 < _temp68.base? 1: _temp70 >= _temp68.last_plus_one)){
_throw( Null_Exception);}* _temp70='\t';});}}{ int j= 0; for( 0; j < _temp61; j
++){({ struct _tagged_string _temp71= nlb; char* _temp73= _temp71.curr +(( j + 1)
+ _temp59); if( _temp71.base == 0? 1:( _temp73 < _temp71.base? 1: _temp73 >=
_temp71.last_plus_one)){ _throw( Null_Exception);}* _temp73=' ';});}}(( void(*)(
struct Cyc_Hashtable_Table* t, int key, struct _tagged_string* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)({ struct Cyc_Core_Opt* _temp74= Cyc_PP_bhashtbl;
if( _temp74 == 0){ _throw( Null_Exception);} _temp74->v;}), i,({ struct
_tagged_string* _temp75=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp75[ 0]= nlb; _temp75;})); return nlb;}}}} _LL58:(
void) _throw( _temp53); _LL54:;}}}} static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl=
0; int Cyc_PP_infinity= 9999999; struct _tuple1{ int f1; struct _tagged_string
f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f( struct _tuple1* clo, struct
Cyc_PP_Ppstate* st){ struct _tuple1 _temp78; struct _tagged_string _temp79; int
_temp81; struct _tuple1* _temp76= clo; _temp78=* _temp76; _LL82: _temp81=
_temp78.f1; goto _LL80; _LL80: _temp79= _temp78.f2; goto _LL77; _LL77: return({
struct Cyc_PP_Out* _temp83=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp83->newcc= st->cc + _temp81; _temp83->newcl= st->cl; _temp83->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp84=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp84[ 0]=({ struct Cyc_PP_Single_struct
_temp85; _temp85.tag= Cyc_PP_Single; _temp85.f1=( void*)({ struct _tagged_string*
_temp86=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp86[ 0]= _temp79; _temp86;}); _temp85;}); _temp84;})); _temp83->links=( void*)((
void*) Cyc_PP_Empty); _temp83;});} static struct Cyc_PP_Doc* Cyc_PP_text_doc(
struct _tagged_string s){ int slen=( int) Cyc_String_strlen( s); return({ struct
Cyc_PP_Doc* _temp87=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp87->mwo= slen; _temp87->mw= Cyc_PP_infinity; _temp87->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*), struct
_tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1* _temp88=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp88->f1= slen; _temp88->f2=
s; _temp88;})); _temp87;});} struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_string s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl == 0){
t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_string*,
struct _tagged_string*), int(* hash)( struct _tagged_string*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp89=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp89->v=( void*) t; _temp89;});} else{ t=( struct Cyc_Hashtable_Table*)({
struct Cyc_Core_Opt* _temp90= Cyc_PP_str_hashtbl; if( _temp90 == 0){ _throw(
Null_Exception);} _temp90->v;});}{ struct _handler_cons _temp91; _push_handler(&
_temp91);{ int _temp93= 0; if( setjmp( _temp91.handler)){ _temp93= 1;} if( !
_temp93){{ struct Cyc_PP_Doc* _temp95=(( struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table*
t, struct _tagged_string* key)) Cyc_Hashtable_lookup)( t,({ struct
_tagged_string* _temp94=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp94[ 0]= s; _temp94;})); _npop_handler( 0u); return
_temp95;}; _pop_handler();} else{ void* _temp92=( void*) _exn_thrown; void*
_temp97= _temp92; _LL99: if( _temp97 == Cyc_Core_Not_found){ goto _LL100;} else{
goto _LL101;} _LL101: goto _LL102; _LL100: { struct Cyc_PP_Doc* d= Cyc_PP_text_doc(
s);(( void(*)( struct Cyc_Hashtable_Table* t, struct _tagged_string* key, struct
Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct _tagged_string* _temp103=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp103[
0]= s; _temp103;}), d); return d;} _LL102:( void) _throw( _temp97); _LL98:;}}}}
struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt= 0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){
if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({ struct Cyc_Core_Opt*
_temp104=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp104->v=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp105=(
char*)""; struct _tagged_string _temp106; _temp106.curr= _temp105; _temp106.base=
_temp105; _temp106.last_plus_one= _temp105 + 1; _temp106;})); _temp104;});}
return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt* _temp107= Cyc_PP_nil_doc_opt;
if( _temp107 == 0){ _throw( Null_Exception);} _temp107->v;});} struct Cyc_PP_Doc*
Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({
struct Cyc_Core_Opt* _temp108=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp108->v=( void*) Cyc_PP_text(( struct _tagged_string)({ char*
_temp109=( char*)""; struct _tagged_string _temp110; _temp110.curr= _temp109;
_temp110.base= _temp109; _temp110.last_plus_one= _temp109 + 1; _temp110;}));
_temp108;});} return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt* _temp111= Cyc_PP_blank_doc_opt;
if( _temp111 == 0){ _throw( Null_Exception);} _temp111->v;});} struct _tuple2{
int f1; struct _tagged_string f2; struct _tagged_string f3; } ; struct _tuple3{
int f1; int f2; int f3; struct _tagged_string f4; } ; static struct Cyc_PP_Out*
Cyc_PP_hyperlink_f( struct _tuple2* clo, struct Cyc_PP_Ppstate* st){ struct
_tuple2 _temp114; struct _tagged_string _temp115; struct _tagged_string _temp117;
int _temp119; struct _tuple2* _temp112= clo; _temp114=* _temp112; _LL120:
_temp119= _temp114.f1; goto _LL118; _LL118: _temp117= _temp114.f2; goto _LL116;
_LL116: _temp115= _temp114.f3; goto _LL113; _LL113: return({ struct Cyc_PP_Out*
_temp121=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp121->newcc=
st->cc + _temp119; _temp121->newcl= st->cl; _temp121->ppout=( void*)(( void*)({
struct Cyc_PP_Single_struct* _temp125=( struct Cyc_PP_Single_struct*) GC_malloc(
sizeof( struct Cyc_PP_Single_struct)); _temp125[ 0]=({ struct Cyc_PP_Single_struct
_temp126; _temp126.tag= Cyc_PP_Single; _temp126.f1=( void*)({ struct
_tagged_string* _temp127=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp127[ 0]= _temp117; _temp127;}); _temp126;}); _temp125;}));
_temp121->links=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp122=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp122[ 0]=({ struct Cyc_PP_Single_struct _temp123; _temp123.tag= Cyc_PP_Single;
_temp123.f1=( void*)({ struct _tuple3* _temp124=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp124->f1= st->cl; _temp124->f2= st->cc; _temp124->f3=
_temp119; _temp124->f4= _temp115; _temp124;}); _temp123;}); _temp122;}));
_temp121;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_string shrt,
struct _tagged_string full){ int slen=( int) Cyc_String_strlen( shrt); return({
struct Cyc_PP_Doc* _temp128=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp128->mwo= slen; _temp128->mw= Cyc_PP_infinity; _temp128->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct
_tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({ struct _tuple2* _temp129=(
struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp129->f1= slen;
_temp129->f2= shrt; _temp129->f3= full; _temp129;})); _temp128;});} static
struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){ return({ struct
Cyc_PP_Out* _temp130=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp130->newcc= st->ci; _temp130->newcl= st->cl + 1; _temp130->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp131=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp131[ 0]=({ struct Cyc_PP_Single_struct
_temp132; _temp132.tag= Cyc_PP_Single; _temp132.f1=( void*)({ struct
_tagged_string* _temp133=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp133[ 0]= Cyc_PP_nlblanks( st->ci); _temp133;});
_temp132;}); _temp131;})); _temp130->links=( void*)(( void*) Cyc_PP_Empty);
_temp130;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp134=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp134->v=( void*)({ struct Cyc_PP_Doc*
_temp135=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp135->mwo=
0; _temp135->mw= 0; _temp135->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp135;}); _temp134;});}
return( struct Cyc_PP_Doc*)({ struct Cyc_Core_Opt* _temp136= Cyc_PP_line_doc_opt;
if( _temp136 == 0){ _throw( Null_Exception);} _temp136->v;});} struct _tuple4{
int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp139; struct
Cyc_PP_Doc* _temp140; int _temp142; struct _tuple4* _temp137= clo; _temp139=*
_temp137; _LL143: _temp142= _temp139.f1; goto _LL141; _LL141: _temp140= _temp139.f2;
goto _LL138; _LL138: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp144=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp144->ci= st->ci + _temp142; _temp144->cc= st->cc; _temp144->cl= st->cl;
_temp144->pw= st->pw; _temp144->epw= st->epw; _temp144;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp140->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp145=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp145->mwo= d->mwo; _temp145->mw= d->mw; _temp145->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp146=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp146->f1= k; _temp146->f2= d;
_temp146;})); _temp145;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp149; struct Cyc_PP_Doc* _temp150; struct Cyc_PP_Doc*
_temp152; struct _tuple5* _temp147= clo; _temp149=* _temp147; _LL153: _temp152=
_temp149.f1; goto _LL151; _LL151: _temp150= _temp149.f2; goto _LL148; _LL148: {
int epw2= Cyc_PP_max( st->pw - _temp150->mw, st->epw - _temp152->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp156=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp156->ci= st->ci;
_temp156->cc= st->cc; _temp156->cl= st->cl; _temp156->pw= st->pw; _temp156->epw=
epw2; _temp156;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp152->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp155=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp155->ci= st->ci;
_temp155->cc= o1->newcc; _temp155->cl= o1->newcl; _temp155->pw= st->pw; _temp155->epw=
epw2; _temp155;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp150->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp154=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp154->newcc= o2->newcc; _temp154->newcl= o2->newcl;
_temp154->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp154->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp154;});
return o3;}} struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2){ return({ struct Cyc_PP_Doc* _temp157=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp157->mwo= Cyc_PP_min( d1->mw, d1->mwo + d2->mwo);
_temp157->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp157->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct
_tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp158=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp158->f1= d1; _temp158->f2=
d2; _temp158;})); _temp157;});} static struct Cyc_PP_Out* Cyc_PP_long_concats_f(
struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os=
0;{ struct Cyc_List_List* _temp159= ds0; goto _LL160; _LL160: for( 0; _temp159
!= 0; _temp159=({ struct Cyc_List_List* _temp161= _temp159; if( _temp161 == 0){
_throw( Null_Exception);} _temp161->tl;})){ struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp164= _temp159; if( _temp164 == 0){ _throw(
Null_Exception);} _temp164->hd;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp162=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp162->ci= st->ci; _temp162->cc= o->newcc;
_temp162->cl= o->newcl; _temp162->pw= st->pw; _temp162->epw= st->epw - d->mwo;
_temp162;}); os=({ struct Cyc_List_List* _temp163=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp163->hd=( void*) o; _temp163->tl=
os; _temp163;});}}{ int newcc=(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp170= os; if( _temp170 == 0){ _throw( Null_Exception);} _temp170->hd;}))->newcc;
int newcl=(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp169= os; if(
_temp169 == 0){ _throw( Null_Exception);} _temp169->hd;}))->newcl; void* s=(
void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=({
struct Cyc_List_List* _temp165= os; if( _temp165 == 0){ _throw( Null_Exception);}
_temp165->tl;})){ s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)((
struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp166= os; if( _temp166 == 0){
_throw( Null_Exception);} _temp166->hd;}))->ppout, s); links=(( void*(*)( void*
a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp167= os; if( _temp167 == 0){ _throw( Null_Exception);} _temp167->hd;}))->links,
links);} return({ struct Cyc_PP_Out* _temp168=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp168->newcc= newcc; _temp168->newcl= newcl;
_temp168->ppout=( void*) s; _temp168->links=( void*) links; _temp168;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_concats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp178= doclist; if( _temp178 == 0){ _throw(
Null_Exception);} _temp178->hd;}); doclist=({ struct Cyc_List_List* _temp171=
doclist; if( _temp171 == 0){ _throw( Null_Exception);} _temp171->tl;});{ int mw=
d->mw; int mwo= d->mw;{ struct Cyc_List_List* _temp172= doclist; goto _LL173;
_LL173: for( 0; _temp172 != 0; _temp172=({ struct Cyc_List_List* _temp174=
_temp172; if( _temp174 == 0){ _throw( Null_Exception);} _temp174->tl;})){ int
mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp176= _temp172; if( _temp176 == 0){ _throw( Null_Exception);} _temp176->hd;}))->mwo);
int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp175= _temp172; if( _temp175 == 0){ _throw( Null_Exception);} _temp175->hd;}))->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp177=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp177->mwo= mw; _temp177->mw= mwo;
_temp177->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*,
struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_concats_f,
orig); _temp177;});}} struct Cyc_PP_Doc* Cyc_PP_concats( struct Cyc_List_List*
doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List*
_temp179= doclist; if( _temp179 == 0){ _throw( Null_Exception);} _temp179->tl;})
== 0){ return( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp180= doclist;
if( _temp180 == 0){ _throw( Null_Exception);} _temp180->hd;});} else{ if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( doclist) > 30){ return Cyc_PP_long_concats(
doclist);} else{ return Cyc_PP_concat(( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp181= doclist; if( _temp181 == 0){ _throw( Null_Exception);} _temp181->hd;}),
Cyc_PP_concats(({ struct Cyc_List_List* _temp182= doclist; if( _temp182 == 0){
_throw( Null_Exception);} _temp182->tl;})));}}}} static struct Cyc_PP_Out* Cyc_PP_concats_arr_f(
struct _tagged_ptr0* docs_ptr, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List*
os= 0; struct _tagged_ptr0 docs=* docs_ptr; int sz=( int)({ struct _tagged_ptr0
_temp194= docs;( unsigned int)( _temp194.last_plus_one - _temp194.curr);});{ int
i= 0; for( 0; i < sz; ++ i){ struct Cyc_PP_Doc* d=({ struct _tagged_ptr0
_temp185= docs; struct Cyc_PP_Doc** _temp187= _temp185.curr + i; if( _temp185.base
== 0? 1:( _temp187 < _temp185.base? 1: _temp187 >= _temp185.last_plus_one)){
_throw( Null_Exception);}* _temp187;}); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp183=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp183->ci= st->ci; _temp183->cc= o->newcc;
_temp183->cl= o->newcl; _temp183->pw= st->pw; _temp183->epw= st->epw - d->mwo;
_temp183;}); os=({ struct Cyc_List_List* _temp184=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp184->hd=( void*) o; _temp184->tl=
os; _temp184;});}}{ int newcc=(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp193= os; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->hd;}))->newcc;
int newcl=(( struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp192= os; if(
_temp192 == 0){ _throw( Null_Exception);} _temp192->hd;}))->newcl; void* s=(
void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=({
struct Cyc_List_List* _temp188= os; if( _temp188 == 0){ _throw( Null_Exception);}
_temp188->tl;})){ s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)((
struct Cyc_PP_Out*)({ struct Cyc_List_List* _temp189= os; if( _temp189 == 0){
_throw( Null_Exception);} _temp189->hd;}))->ppout, s); links=(( void*(*)( void*
a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)({ struct Cyc_List_List*
_temp190= os; if( _temp190 == 0){ _throw( Null_Exception);} _temp190->hd;}))->links,
links);} return({ struct Cyc_PP_Out* _temp191=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp191->newcc= newcc; _temp191->newcl= newcl;
_temp191->ppout=( void*) s; _temp191->links=( void*) links; _temp191;});}}
struct Cyc_PP_Doc* Cyc_PP_concats_arr( struct _tagged_ptr0 docs){ int sz=( int)({
struct _tagged_ptr0 _temp210= docs;( unsigned int)( _temp210.last_plus_one -
_temp210.curr);}); if( sz == 0){( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp195=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp195[ 0]=({ struct Cyc_Core_Failure_struct _temp196; _temp196.tag= Cyc_Core_Failure;
_temp196.f1=( struct _tagged_string)({ char* _temp197=( char*)"concats_arr -- size zero array";
struct _tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 31; _temp198;}); _temp196;}); _temp195;}));}{
struct Cyc_PP_Doc* d=({ struct _tagged_ptr0 _temp207= docs; struct Cyc_PP_Doc**
_temp209= _temp207.curr + 0; if( _temp207.base == 0? 1:( _temp209 < _temp207.base?
1: _temp209 >= _temp207.last_plus_one)){ _throw( Null_Exception);}* _temp209;});
int mw= d->mw; int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min(
mw, mwo +({ struct _tagged_ptr0 _temp202= docs; struct Cyc_PP_Doc** _temp204=
_temp202.curr + i; if( _temp202.base == 0? 1:( _temp204 < _temp202.base? 1:
_temp204 >= _temp202.last_plus_one)){ _throw( Null_Exception);}* _temp204;})->mwo);
int mwo2= Cyc_PP_min( mw, mwo +({ struct _tagged_ptr0 _temp199= docs; struct Cyc_PP_Doc**
_temp201= _temp199.curr + i; if( _temp199.base == 0? 1:( _temp201 < _temp199.base?
1: _temp201 >= _temp199.last_plus_one)){ _throw( Null_Exception);}* _temp201;})->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp205=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp205->mwo= mw; _temp205->mw= mwo;
_temp205->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_ptr0*,
struct Cyc_PP_Ppstate*), struct _tagged_ptr0* x)) Cyc_Fn_make_fn)( Cyc_PP_concats_arr_f,({
struct _tagged_ptr0* _temp206=( struct _tagged_ptr0*) GC_malloc( sizeof( struct
_tagged_ptr0) * 1); _temp206[ 0]= docs; _temp206;})); _temp205;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp213; struct Cyc_PP_Doc* _temp214; struct Cyc_PP_Doc*
_temp216; struct _tuple5* _temp211= clo; _temp213=* _temp211; _LL217: _temp216=
_temp213.f1; goto _LL215; _LL215: _temp214= _temp213.f2; goto _LL212; _LL212: {
int dfits= st->cc + _temp216->mwo <= st->epw? 1: st->cc + _temp216->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp216->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp214->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp218=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp218->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp218->mw=
Cyc_PP_min( d->mw, d2->mw); _temp218->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp219=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp219->f1= d; _temp219->f2= d2; _temp219;}));
_temp218;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp220=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp220->ci= st->cc; _temp220->cc= st->cc;
_temp220->cl= st->cl; _temp220->pw= st->pw; _temp220->epw= st->epw; _temp220;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp221=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp221->mwo= d->mwo; _temp221->mw= d->mw;
_temp221->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp221;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List* _temp222= l; if(
_temp222 == 0){ _throw( Null_Exception);} _temp222->tl;}) == 0){ return pp((
void*)({ struct Cyc_List_List* _temp223= l; if( _temp223 == 0){ _throw(
Null_Exception);} _temp223->hd;}));} else{ return Cyc_PP_concats(({ struct Cyc_List_List*
_temp224=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp224->hd=( void*) pp(( void*)({ struct Cyc_List_List* _temp229= l; if(
_temp229 == 0){ _throw( Null_Exception);} _temp229->hd;})); _temp224->tl=({
struct Cyc_List_List* _temp225=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp225->hd=( void*) Cyc_PP_text( sep); _temp225->tl=({
struct Cyc_List_List* _temp226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp226->hd=( void*) Cyc_PP_oline_doc(); _temp226->tl=({
struct Cyc_List_List* _temp227=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp227->hd=( void*)(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List* l))
Cyc_PP_ppseq_f)( pp, sep,({ struct Cyc_List_List* _temp228= l; if( _temp228 == 0){
_throw( Null_Exception);} _temp228->tl;})); _temp227->tl= 0; _temp227;});
_temp226;}); _temp225;}); _temp224;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseq(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp, sep,
l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List*
_temp230= l; if( _temp230 == 0){ _throw( Null_Exception);} _temp230->tl;}) == 0){
return( struct Cyc_PP_Doc*)({ struct Cyc_List_List* _temp231= l; if( _temp231 ==
0){ _throw( Null_Exception);} _temp231->hd;});} else{ struct Cyc_PP_Doc* sep2=
Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List*
_temp232= l; goto _LL233; _LL233: while(({ struct Cyc_List_List* _temp234=
_temp232; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->tl;}) != 0) {
struct Cyc_List_List* _temp236=({ struct Cyc_List_List* _temp235= _temp232; if(
_temp235 == 0){ _throw( Null_Exception);} _temp235->tl;}); goto _LL237; _LL237:({
struct Cyc_List_List* _temp238= _temp232; if( _temp238 == 0){ _throw(
Null_Exception);} _temp238->tl=({ struct Cyc_List_List* _temp239=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp239->hd=( void*) sep2; _temp239->tl=({
struct Cyc_List_List* _temp240=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp240->hd=( void*) oline; _temp240->tl= _temp236;
_temp240;}); _temp239;});}); _temp232= _temp236;} return Cyc_PP_concats( l);}}}
struct Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab( Cyc_PP_seq_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({ struct Cyc_List_List*
_temp241= l; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->tl;}) == 0){
return pp(( void*)({ struct Cyc_List_List* _temp242= l; if( _temp242 == 0){
_throw( Null_Exception);} _temp242->hd;}));} else{ return Cyc_PP_concats(({
struct Cyc_List_List* _temp243=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp243->hd=( void*) pp(( void*)({ struct Cyc_List_List*
_temp248= l; if( _temp248 == 0){ _throw( Null_Exception);} _temp248->hd;}));
_temp243->tl=({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp244->hd=( void*) Cyc_PP_text(
sep); _temp244->tl=({ struct Cyc_List_List* _temp245=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp245->hd=( void*) Cyc_PP_line_doc();
_temp245->tl=({ struct Cyc_List_List* _temp246=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp246->hd=( void*)(( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql_f)( pp, sep,({ struct Cyc_List_List* _temp247= l; if( _temp247
== 0){ _throw( Null_Exception);} _temp247->tl;})); _temp246->tl= 0; _temp246;});
_temp245;}); _temp244;}); _temp243;}));}}} struct Cyc_PP_Doc* Cyc_PP_ppseql(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_string sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep,
l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_string sep,
struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if(({
struct Cyc_List_List* _temp249= l; if( _temp249 == 0){ _throw( Null_Exception);}
_temp249->tl;}) == 0){ return( struct Cyc_PP_Doc*)({ struct Cyc_List_List*
_temp250= l; if( _temp250 == 0){ _throw( Null_Exception);} _temp250->hd;});}
else{ return Cyc_PP_concats(({ struct Cyc_List_List* _temp251=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp251->hd=( void*)(( struct Cyc_PP_Doc*)({
struct Cyc_List_List* _temp256= l; if( _temp256 == 0){ _throw( Null_Exception);}
_temp256->hd;})); _temp251->tl=({ struct Cyc_List_List* _temp252=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp252->hd=( void*) Cyc_PP_text(
sep); _temp252->tl=({ struct Cyc_List_List* _temp253=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp253->hd=( void*) Cyc_PP_line_doc();
_temp253->tl=({ struct Cyc_List_List* _temp254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp254->hd=( void*) Cyc_PP_seql_f(
sep,({ struct Cyc_List_List* _temp255= l; if( _temp255 == 0){ _throw(
Null_Exception);} _temp255->tl;})); _temp254->tl= 0; _temp254;}); _temp253;});
_temp252;}); _temp251;}));}}} struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seql_f(
sep, l0));}