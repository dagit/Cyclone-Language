 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern void* Cyc_Core_identity(
void*); extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_String_str_sepstr( struct
Cyc_List_List*, struct _tagged_arr); struct Cyc_Hashtable_Table; extern struct
Cyc_Hashtable_Table* Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*),
int(* hash)( void*)); extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table*
t, void* key, void* val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table*
t, void* key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_arr* s);
static const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)(
void*, void*); void* f2; } ; extern void* Cyc_Fn_make_fn( void*(* f)( void*,
void*), void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void*
Cyc_Fn_apply( void* f, void* x); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
struct Cyc_PP_Doc; extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f); extern struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w); struct _tuple0{ struct _tagged_arr f1; struct Cyc_List_List*
f2; } ; extern struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d,
int w); extern struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc*
Cyc_PP_blank_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct
Cyc_PP_Doc* Cyc_PP_oline_doc(); extern struct Cyc_PP_Doc* Cyc_PP_text( struct
_tagged_arr s); extern struct Cyc_PP_Doc* Cyc_PP_textptr( struct _tagged_arr* s);
extern struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_arr shrt, struct
_tagged_arr full); extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct _tagged_arr); extern struct Cyc_PP_Doc*
Cyc_PP_cats( struct Cyc_List_List* doclist); extern struct Cyc_PP_Doc* Cyc_PP_cats_arr(
struct _tagged_arr docs); extern struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc*
d, struct Cyc_PP_Doc* d2); extern struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc*
d); extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seq(
struct _tagged_arr sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_ppseql(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l0); extern struct Cyc_PP_Doc* Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List*
l0); extern struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_arr start, struct
_tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss); extern
struct Cyc_PP_Doc* Cyc_PP_groupl( struct _tagged_arr start, struct _tagged_arr
stop, struct _tagged_arr sep, struct Cyc_List_List* ss); extern struct Cyc_PP_Doc*
Cyc_PP_egroup( struct _tagged_arr start, struct _tagged_arr stop, struct
_tagged_arr sep, struct Cyc_List_List* ss); struct Cyc_Xarray_Xarray{ struct
_tagged_arr elmts; int num_elmts; } ; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*, int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
static const int Cyc_PP_Empty= 0; static const int Cyc_PP_Single= 0; struct Cyc_PP_Single_struct{
int tag; void* f1; } ; static const int Cyc_PP_Append= 1; struct Cyc_PP_Append_struct{
int tag; void* f1; void* f2; } ; void* Cyc_PP_append( void* a1, void* a2){ if(
a1 ==( void*) Cyc_PP_Empty){ return a2;} else{ if( a2 ==( void*) Cyc_PP_Empty){
return a1;} else{ return( void*)({ struct Cyc_PP_Append_struct* _temp0=( struct
Cyc_PP_Append_struct*) GC_malloc( sizeof( struct Cyc_PP_Append_struct)); _temp0[
0]=({ struct Cyc_PP_Append_struct _temp1; _temp1.tag= Cyc_PP_Append; _temp1.f1=(
void*) a1; _temp1.f2=( void*) a2; _temp1;}); _temp0;});}}} struct Cyc_List_List*
Cyc_PP_list_of_alist_f( void* y, struct Cyc_List_List* l){ void* _temp2= y; void*
_temp10; void* _temp12; void* _temp14; _LL4: if( _temp2 ==( void*) Cyc_PP_Empty){
goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp2 > 1u?*(( int*)
_temp2) == Cyc_PP_Single: 0){ _LL11: _temp10=( void*)(( struct Cyc_PP_Single_struct*)
_temp2)->f1; goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp2 > 1u?*((
int*) _temp2) == Cyc_PP_Append: 0){ _LL15: _temp14=( void*)(( struct Cyc_PP_Append_struct*)
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
Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray* _temp17=(( struct Cyc_Xarray_Xarray*(*)(
int, void*)) Cyc_Xarray_create)( 16, al); goto _LL18; _LL18:(( void(*)( struct
Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( _temp17, al);{ int last= 0; while(
last >= 0) { void* _temp19=(( void*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)(
_temp17, last); struct _tagged_arr* _temp27; void* _temp29; void* _temp31; _LL21:
if( _temp19 ==( void*) Cyc_PP_Empty){ goto _LL22;} else{ goto _LL23;} _LL23: if((
unsigned int) _temp19 > 1u?*(( int*) _temp19) == Cyc_PP_Single: 0){ _LL28:
_temp27=( struct _tagged_arr*)(( struct Cyc_PP_Single_struct*) _temp19)->f1;
goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp19 > 1u?*(( int*)
_temp19) == Cyc_PP_Append: 0){ _LL32: _temp31=( void*)(( struct Cyc_PP_Append_struct*)
_temp19)->f1; goto _LL30; _LL30: _temp29=( void*)(( struct Cyc_PP_Append_struct*)
_temp19)->f2; goto _LL26;} else{ goto _LL20;} _LL22: -- last; goto _LL20; _LL24:
-- last;({ struct _tagged_arr _temp33=* _temp27; fprintf( _sfile_to_file( f),"%.*s",
_get_arr_size( _temp33, 1u), _temp33.curr);}); goto _LL20; _LL26:(( void(*)(
struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)( _temp17, last, _temp29);
if( last ==(( int(*)( struct Cyc_Xarray_Xarray*)) Cyc_Xarray_length)( _temp17) -
1){(( void(*)( struct Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( _temp17,
_temp31);} else{(( void(*)( struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)(
_temp17, last + 1, _temp31);} ++ last; goto _LL20; _LL20:;}}} void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE* f){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp34=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp34->ci= 0; _temp34->cc=
0; _temp34->cl= 1; _temp34->pw= w; _temp34->epw= w; _temp34;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp35=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp35->ci= 0; _temp35->cc=
0; _temp35->cl= 1; _temp35->pw= w; _temp35->epw= w; _temp35;})); return( struct
_tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));} struct _tuple0*
Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp37=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp37->ci= 0; _temp37->cc=
0; _temp37->cl= 1; _temp37->pw= w; _temp37->epw= w; _temp37;})); return({ struct
_tuple0* _temp36=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp36->f1=(
struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x))
Cyc_PP_list_of_alist)(( void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));
_temp36->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*)
o->links); _temp36;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0; struct
_tagged_arr Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp38=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp38->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp38;});} if( i < 0){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp39=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp39[ 0]=({ struct Cyc_Core_Failure_struct
_temp40; _temp40.tag= Cyc_Core_Failure; _temp40.f1= _tag_arr("nlblanks", sizeof(
unsigned char), 9u); _temp40;}); _temp39;}));}{ struct _handler_cons _temp41;
_push_handler(& _temp41);{ int _temp43= 0; if( setjmp( _temp41.handler)){
_temp43= 1;} if( ! _temp43){{ struct _tagged_arr _temp44=*(( struct _tagged_arr*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v, i); _npop_handler( 0u);
return _temp44;}; _pop_handler();} else{ void* _temp42=( void*) _exn_thrown;
void* _temp46= _temp42; _LL48: if( _temp46 == Cyc_Core_Not_found){ goto _LL49;}
else{ goto _LL50;} _LL50: goto _LL51; _LL49: { int _temp52= i / 8; goto _LL53;
_LL53: { int _temp54= i % 8; goto _LL55; _LL55: { int _temp56=( 1 + _temp52) +
_temp54; goto _LL57; _LL57: { struct _tagged_arr nlb= Cyc_Core_new_string(
_temp56);*(( unsigned char*) _check_unknown_subscript( nlb, sizeof(
unsigned char), 0))='\n';{ int j= 0; for( 0; j < _temp52; j ++){*((
unsigned char*) _check_unknown_subscript( nlb, sizeof( unsigned char), j + 1))='\t';}}{
int j= 0; for( 0; j < _temp54; j ++){*(( unsigned char*)
_check_unknown_subscript( nlb, sizeof( unsigned char),( j + 1) + _temp52))=' ';}}((
void(*)( struct Cyc_Hashtable_Table* t, int key, struct _tagged_arr* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v,
i,({ struct _tagged_arr* _temp58=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp58[ 0]=( struct _tagged_arr) nlb; _temp58;}));
return( struct _tagged_arr) nlb;}}}} _LL51:( void) _throw( _temp46); _LL47:;}}}}
static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999;
struct _tuple1{ int f1; struct _tagged_arr f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f(
struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct _tuple1 _temp61; struct
_tagged_arr _temp62; int _temp64; struct _tuple1* _temp59= clo; _temp61=*
_temp59; _LL65: _temp64= _temp61.f1; goto _LL63; _LL63: _temp62= _temp61.f2;
goto _LL60; _LL60: return({ struct Cyc_PP_Out* _temp66=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp66->newcc= st->cc + _temp64;
_temp66->newcl= st->cl; _temp66->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp67=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp67[ 0]=({ struct Cyc_PP_Single_struct _temp68; _temp68.tag= Cyc_PP_Single;
_temp68.f1=( void*)({ struct _tagged_arr* _temp69=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp69[ 0]= _temp62; _temp69;});
_temp68;}); _temp67;})); _temp66->links=( void*)(( void*) Cyc_PP_Empty); _temp66;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_arr s){ int slen= Cyc_String_strlen(
s); return({ struct Cyc_PP_Doc* _temp70=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp70->mwo= slen; _temp70->mw= Cyc_PP_infinity; _temp70->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*),
struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1*
_temp71=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp71->f1=
slen; _temp71->f2= s; _temp71;})); _temp70;});} struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_arr s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl ==
0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*), int(* hash)( struct _tagged_arr*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp72=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp72->v=( void*) t; _temp72;});} else{ t=( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_str_hashtbl))->v;}{ struct
_handler_cons _temp73; _push_handler(& _temp73);{ int _temp75= 0; if( setjmp(
_temp73.handler)){ _temp75= 1;} if( ! _temp75){{ struct Cyc_PP_Doc* _temp77=((
struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct _tagged_arr* key))
Cyc_Hashtable_lookup)( t,({ struct _tagged_arr* _temp76=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp76[ 0]= s; _temp76;}));
_npop_handler( 0u); return _temp77;}; _pop_handler();} else{ void* _temp74=(
void*) _exn_thrown; void* _temp79= _temp74; _LL81: if( _temp79 == Cyc_Core_Not_found){
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL82: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_arr* _temp85=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp85[ 0]= s; _temp85;}), d); return d;} _LL84:( void)
_throw( _temp79); _LL80:;}}}} struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_arr* s){ return Cyc_PP_text(* s);} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt=
0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt* _temp86=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp86->v=( void*) Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)); _temp86;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_PP_nil_doc_opt))->v;} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){
if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt*
_temp87=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp87->v=( void*) Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u));
_temp87;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_PP_blank_doc_opt))->v;} struct _tuple2{ int f1; struct _tagged_arr f2;
struct _tagged_arr f3; } ; struct _tuple3{ int f1; int f2; int f3; struct
_tagged_arr f4; } ; static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct _tuple2*
clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp90; struct _tagged_arr
_temp91; struct _tagged_arr _temp93; int _temp95; struct _tuple2* _temp88= clo;
_temp90=* _temp88; _LL96: _temp95= _temp90.f1; goto _LL94; _LL94: _temp93=
_temp90.f2; goto _LL92; _LL92: _temp91= _temp90.f3; goto _LL89; _LL89: return({
struct Cyc_PP_Out* _temp97=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp97->newcc= st->cc + _temp95; _temp97->newcl= st->cl; _temp97->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp101=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp101[ 0]=({ struct Cyc_PP_Single_struct
_temp102; _temp102.tag= Cyc_PP_Single; _temp102.f1=( void*)({ struct _tagged_arr*
_temp103=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp103[ 0]= _temp93; _temp103;}); _temp102;}); _temp101;})); _temp97->links=(
void*)(( void*)({ struct Cyc_PP_Single_struct* _temp98=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp98[ 0]=({ struct Cyc_PP_Single_struct
_temp99; _temp99.tag= Cyc_PP_Single; _temp99.f1=( void*)({ struct _tuple3*
_temp100=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp100->f1=
st->cl; _temp100->f2= st->cc; _temp100->f3= _temp95; _temp100->f4= _temp91;
_temp100;}); _temp99;}); _temp98;})); _temp97;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink(
struct _tagged_arr shrt, struct _tagged_arr full){ int slen= Cyc_String_strlen(
shrt); return({ struct Cyc_PP_Doc* _temp104=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp104->mwo= slen; _temp104->mw= Cyc_PP_infinity;
_temp104->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple2*,
struct Cyc_PP_Ppstate*), struct _tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({
struct _tuple2* _temp105=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp105->f1= slen; _temp105->f2= shrt; _temp105->f3= full; _temp105;}));
_temp104;});} static struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){
return({ struct Cyc_PP_Out* _temp106=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp106->newcc= st->ci; _temp106->newcl= st->cl + 1;
_temp106->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp107=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp107[ 0]=({ struct Cyc_PP_Single_struct _temp108; _temp108.tag= Cyc_PP_Single;
_temp108.f1=( void*)({ struct _tagged_arr* _temp109=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp109[ 0]= Cyc_PP_nlblanks( st->ci);
_temp109;}); _temp108;}); _temp107;})); _temp106->links=( void*)(( void*) Cyc_PP_Empty);
_temp106;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp110=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp110->v=( void*)({ struct Cyc_PP_Doc*
_temp111=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp111->mwo=
0; _temp111->mw= 0; _temp111->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp111;}); _temp110;});}
return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_line_doc_opt))->v;}
struct _tuple4{ int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp114; struct
Cyc_PP_Doc* _temp115; int _temp117; struct _tuple4* _temp112= clo; _temp114=*
_temp112; _LL118: _temp117= _temp114.f1; goto _LL116; _LL116: _temp115= _temp114.f2;
goto _LL113; _LL113: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp119=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp119->ci= st->ci + _temp117; _temp119->cc= st->cc; _temp119->cl= st->cl;
_temp119->pw= st->pw; _temp119->epw= st->epw; _temp119;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp115->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp120=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp120->mwo= d->mwo; _temp120->mw= d->mw; _temp120->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp121=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp121->f1= k; _temp121->f2= d;
_temp121;})); _temp120;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp124; struct Cyc_PP_Doc* _temp125; struct Cyc_PP_Doc*
_temp127; struct _tuple5* _temp122= clo; _temp124=* _temp122; _LL128: _temp127=
_temp124.f1; goto _LL126; _LL126: _temp125= _temp124.f2; goto _LL123; _LL123: {
int epw2= Cyc_PP_max( st->pw - _temp125->mw, st->epw - _temp127->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp131=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp131->ci= st->ci;
_temp131->cc= st->cc; _temp131->cl= st->cl; _temp131->pw= st->pw; _temp131->epw=
epw2; _temp131;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp127->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp130=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp130->ci= st->ci;
_temp130->cc= o1->newcc; _temp130->cl= o1->newcl; _temp130->pw= st->pw; _temp130->epw=
epw2; _temp130;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp125->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp129=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp129->newcc= o2->newcc; _temp129->newcl= o2->newcl;
_temp129->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp129->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp129;});
return o3;}} static struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp132=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp132->mwo= Cyc_PP_min( d1->mw, d1->mwo
+ d2->mwo); _temp132->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp132->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*),
struct _tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp133=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp133->f1= d1; _temp133->f2=
d2; _temp133;})); _temp132;});} struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr l){ struct Cyc_PP_Doc* _temp134= Cyc_PP_nil_doc(); goto _LL135;
_LL135:{ int i=( int)( _get_arr_size( l, sizeof( struct Cyc_PP_Doc*)) - 1); for(
0; i >= 0; i --){ _temp134= Cyc_PP_concat(*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( l, sizeof( struct Cyc_PP_Doc*), i)), _temp134);}}
return _temp134;} static struct Cyc_PP_Out* Cyc_PP_long_cats_f( struct Cyc_List_List*
ds0, struct Cyc_PP_Ppstate* st){ struct Cyc_List_List* os= 0;{ struct Cyc_List_List*
_temp136= ds0; goto _LL137; _LL137: for( 0; _temp136 != 0; _temp136=(( struct
Cyc_List_List*) _check_null( _temp136))->tl){ struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( _temp136))->hd; struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f, st); st=({ struct Cyc_PP_Ppstate* _temp138=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp138->ci= st->ci;
_temp138->cc= o->newcc; _temp138->cl= o->newcl; _temp138->pw= st->pw; _temp138->epw=
st->epw - d->mwo; _temp138;}); os=({ struct Cyc_List_List* _temp139=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp139->hd=( void*) o; _temp139->tl=
os; _temp139;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os != 0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp140=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp140->newcc=
newcc; _temp140->newcl= newcl; _temp140->ppout=( void*) s; _temp140->links=(
void*) links; _temp140;});}} static struct Cyc_PP_Doc* Cyc_PP_long_cats( struct
Cyc_List_List* doclist){ struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc*
d=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( doclist))->hd;
doclist=(( struct Cyc_List_List*) _check_null( doclist))->tl;{ int mw= d->mw;
int mwo= d->mw;{ struct Cyc_List_List* _temp141= doclist; goto _LL142; _LL142:
for( 0; _temp141 != 0; _temp141=(( struct Cyc_List_List*) _check_null( _temp141))->tl){
int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( _temp141))->hd)->mwo); int mwo2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( _temp141))->hd)->mw); mw= mw2; mwo= mwo2;}}
return({ struct Cyc_PP_Doc* _temp143=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp143->mwo= mw; _temp143->mw= mwo; _temp143->f=( void*)((
void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*, struct Cyc_PP_Ppstate*),
struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_cats_f, orig); _temp143;});}}
struct Cyc_PP_Doc* Cyc_PP_cats( struct Cyc_List_List* doclist){ if( doclist == 0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null(
doclist))->tl == 0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( doclist))->hd;} else{ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
doclist) > 30){ return Cyc_PP_long_cats( doclist);} else{ return Cyc_PP_concat((
struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( doclist))->hd, Cyc_PP_cats(((
struct Cyc_List_List*) _check_null( doclist))->tl));}}}} static struct Cyc_PP_Out*
Cyc_PP_cats_arr_f( struct _tagged_arr* docs_ptr, struct Cyc_PP_Ppstate* st){
struct Cyc_List_List* os= 0; struct _tagged_arr docs=* docs_ptr; int sz=( int)
_get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));{ int i= 0; for( 0; i < sz; ++
i){ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**) _check_unknown_subscript( docs,
sizeof( struct Cyc_PP_Doc*), i)); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp144=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp144->ci= st->ci; _temp144->cc= o->newcc;
_temp144->cl= o->newcl; _temp144->pw= st->pw; _temp144->epw= st->epw - d->mwo;
_temp144;}); os=({ struct Cyc_List_List* _temp145=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp145->hd=( void*) o; _temp145->tl=
os; _temp145;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os != 0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp146=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp146->newcc=
newcc; _temp146->newcl= newcl; _temp146->ppout=( void*) s; _temp146->links=(
void*) links; _temp146;});}} struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct
_tagged_arr docs){ int sz=( int) _get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));
if( sz == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp147=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp147[ 0]=({ struct Cyc_Core_Failure_struct _temp148; _temp148.tag= Cyc_Core_Failure;
_temp148.f1= _tag_arr("cats_arr -- size zero array", sizeof( unsigned char), 28u);
_temp148;}); _temp147;}));}{ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), 0)); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo +(*((
struct Cyc_PP_Doc**) _check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*),
i)))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), i)))->mw); mw= mw2;
mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp149=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp149->mwo= mw; _temp149->mw= mwo;
_temp149->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_arr*,
struct Cyc_PP_Ppstate*), struct _tagged_arr* x)) Cyc_Fn_make_fn)( Cyc_PP_cats_arr_f,({
struct _tagged_arr* _temp150=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp150[ 0]= docs; _temp150;})); _temp149;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp153; struct Cyc_PP_Doc* _temp154; struct Cyc_PP_Doc*
_temp156; struct _tuple5* _temp151= clo; _temp153=* _temp151; _LL157: _temp156=
_temp153.f1; goto _LL155; _LL155: _temp154= _temp153.f2; goto _LL152; _LL152: {
int dfits= st->cc + _temp156->mwo <= st->epw? 1: st->cc + _temp156->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp156->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp154->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp158=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp158->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp158->mw=
Cyc_PP_min( d->mw, d2->mw); _temp158->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp159=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp159->f1= d; _temp159->f2= d2; _temp159;}));
_temp158;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp160=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp160->ci= st->cc; _temp160->cc= st->cc;
_temp160->cl= st->cl; _temp160->pw= st->pw; _temp160->epw= st->epw; _temp160;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp161=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp161->mwo= d->mwo; _temp161->mw= d->mw;
_temp161->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp161;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null( l))->tl
== 0){ return pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);} else{
return({ struct Cyc_PP_Doc*(* _temp162)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp165= pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
struct Cyc_PP_Doc* _temp166= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp167= Cyc_PP_oline_doc();
struct Cyc_PP_Doc* _temp168=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp,
sep,(( struct Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp163[
4u]={ _temp165, _temp166, _temp167, _temp168}; struct _tagged_arr _temp164={(
void*) _temp163,( void*) _temp163,( void*)( _temp163 + 4u)}; _temp162( _temp164);});}}}
struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq_f)( pp, sep, l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct
_tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;} else{ struct Cyc_PP_Doc*
sep2= Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct
Cyc_List_List* _temp169= l; goto _LL170; _LL170: while((( struct Cyc_List_List*)
_check_null( _temp169))->tl != 0) { struct Cyc_List_List* _temp171=(( struct Cyc_List_List*)
_check_null( _temp169))->tl; goto _LL172; _LL172:(( struct Cyc_List_List*)
_check_null( _temp169))->tl=({ struct Cyc_List_List* _temp173=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp173->hd=( void*) sep2; _temp173->tl=({
struct Cyc_List_List* _temp174=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp174->hd=( void*) oline; _temp174->tl= _temp171;
_temp174;}); _temp173;}); _temp169= _temp171;} return Cyc_PP_cats( l);}}} struct
Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_arr sep, struct Cyc_List_List* l0){
return Cyc_PP_tab( Cyc_PP_seq_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl == 0){ return pp(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*(* _temp175)( struct
_tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp178= pp(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd); struct Cyc_PP_Doc* _temp179= Cyc_PP_text(
sep); struct Cyc_PP_Doc* _temp180= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp181=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp, sep,(( struct
Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp176[ 4u]={
_temp178, _temp179, _temp180, _temp181}; struct _tagged_arr _temp177={( void*)
_temp176,( void*) _temp176,( void*)( _temp176 + 4u)}; _temp175( _temp177);});}}}
struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql_f)( pp, sep, l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f(
struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;} else{ return({
struct Cyc_PP_Doc*(* _temp182)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp185=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;
struct Cyc_PP_Doc* _temp186= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp187= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp188= Cyc_PP_seql_f( sep,(( struct Cyc_List_List*)
_check_null( l))->tl); struct Cyc_PP_Doc* _temp183[ 4u]={ _temp185, _temp186,
_temp187, _temp188}; struct _tagged_arr _temp184={( void*) _temp183,( void*)
_temp183,( void*)( _temp183 + 4u)}; _temp182( _temp184);});}}} struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(
Cyc_PP_seql_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_arr
start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss){
return({ struct Cyc_PP_Doc*(* _temp189)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp192= Cyc_PP_text( start); struct Cyc_PP_Doc* _temp193=
Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc* _temp194= Cyc_PP_text( stop); struct
Cyc_PP_Doc* _temp190[ 3u]={ _temp192, _temp193, _temp194}; struct _tagged_arr
_temp191={( void*) _temp190,( void*) _temp190,( void*)( _temp190 + 3u)};
_temp189( _temp191);});} struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr
start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss){
if( ss == 0){ return Cyc_PP_nil_doc();} else{ return({ struct Cyc_PP_Doc*(*
_temp195)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp198= Cyc_PP_text(
start); struct Cyc_PP_Doc* _temp199= Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc*
_temp200= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp196[ 3u]={ _temp198,
_temp199, _temp200}; struct _tagged_arr _temp197={( void*) _temp196,( void*)
_temp196,( void*)( _temp196 + 3u)}; _temp195( _temp197);});}} struct Cyc_PP_Doc*
Cyc_PP_groupl( struct _tagged_arr start, struct _tagged_arr stop, struct
_tagged_arr sep, struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*(*
_temp201)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp204= Cyc_PP_text(
start); struct Cyc_PP_Doc* _temp205= Cyc_PP_seql( sep, ss); struct Cyc_PP_Doc*
_temp206= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp202[ 3u]={ _temp204,
_temp205, _temp206}; struct _tagged_arr _temp203={( void*) _temp202,( void*)
_temp202,( void*)( _temp202 + 3u)}; _temp201( _temp203);});}