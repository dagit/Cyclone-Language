 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[ 18u];
extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen( struct
_tagged_arr s); extern int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct
_tagged_arr*); extern struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_arr* s); static
const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*,
void*); void* f2; } ; extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*),
void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply(
void* f, void* x); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int w);
struct _tuple0{ struct _tagged_arr f1; struct Cyc_List_List* f2; } ; extern
struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w); extern
struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_blank_doc();
extern struct Cyc_PP_Doc* Cyc_PP_line_doc(); extern struct Cyc_PP_Doc* Cyc_PP_oline_doc();
extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_arr s); extern struct Cyc_PP_Doc*
Cyc_PP_textptr( struct _tagged_arr* s); extern struct Cyc_PP_Doc* Cyc_PP_hyperlink(
struct _tagged_arr shrt, struct _tagged_arr full); extern struct Cyc_PP_Doc* Cyc_PP_nest(
int k, struct Cyc_PP_Doc* d); extern struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr); extern struct Cyc_PP_Doc* Cyc_PP_cats( struct Cyc_List_List*
doclist); extern struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct _tagged_arr docs);
extern struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc*
d2); extern struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d); extern struct
Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr
sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seq( struct
_tagged_arr sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_ppseql(
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
_temp16->hd=( void*) _temp10; _temp16->tl= l; _temp16;}); _LL9: return Cyc_PP_list_of_alist_f(
_temp14, Cyc_PP_list_of_alist_f( _temp12, l)); _LL3:;} struct Cyc_List_List* Cyc_PP_list_of_alist(
void* x){ return Cyc_PP_list_of_alist_f( x, 0);} struct Cyc_PP_Ppstate{ int ci;
int cc; int cl; int pw; int epw; } ; struct Cyc_PP_Out{ int newcc; int newcl;
void* ppout; void* links; } ; struct Cyc_PP_Doc{ int mwo; int mw; void* f; } ;
static void Cyc_PP_dump_out( struct Cyc_Stdio___sFILE* f, void* al){ struct Cyc_Xarray_Xarray*
_temp17= Cyc_Xarray_create( 16, al); Cyc_Xarray_add( _temp17, al);{ int last= 0;
while( last >= 0) { void* _temp18= Cyc_Xarray_get( _temp17, last); struct
_tagged_arr* _temp26; void* _temp28; void* _temp30; _LL20: if( _temp18 ==( void*)
Cyc_PP_Empty){ goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp18
> 1u?*(( int*) _temp18) == Cyc_PP_Single: 0){ _LL27: _temp26=( struct
_tagged_arr*)(( struct Cyc_PP_Single_struct*) _temp18)->f1; goto _LL23;} else{
goto _LL24;} _LL24: if(( unsigned int) _temp18 > 1u?*(( int*) _temp18) == Cyc_PP_Append:
0){ _LL31: _temp30=( void*)(( struct Cyc_PP_Append_struct*) _temp18)->f1; goto
_LL29; _LL29: _temp28=( void*)(( struct Cyc_PP_Append_struct*) _temp18)->f2;
goto _LL25;} else{ goto _LL19;} _LL21: -- last; goto _LL19; _LL23: -- last;({
int(* _temp32)( struct Cyc_Stdio___sFILE*, struct _tagged_arr fmt, struct
_tagged_arr)= Cyc_Stdio_fprintf; struct Cyc_Stdio___sFILE* _temp36= f; struct
_tagged_arr _temp37= _tag_arr("%s", sizeof( unsigned char), 3u); struct Cyc_Stdio_String_pa_struct
_temp38; _temp38.tag= Cyc_Stdio_String_pa; _temp38.f1=( struct _tagged_arr)*
_temp26;{ void* _temp35=( void*)& _temp38; void* _temp33[ 1u]={ _temp35}; struct
_tagged_arr _temp34={( void*) _temp33,( void*) _temp33,( void*)( _temp33 + 1u)};
_temp32( _temp36, _temp37, _temp34);}}); goto _LL19; _LL25: Cyc_Xarray_set(
_temp17, last, _temp28); if( last == Cyc_Xarray_length( _temp17) - 1){ Cyc_Xarray_add(
_temp17, _temp30);} else{ Cyc_Xarray_set( _temp17, last + 1, _temp30);} ++ last;
goto _LL19; _LL19:;}}} void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w,
struct Cyc_Stdio___sFILE* f){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate*
_temp39=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp39->ci= 0; _temp39->cc= 0; _temp39->cl= 1; _temp39->pw= w; _temp39->epw= w;
_temp39;})); Cyc_PP_dump_out( f,( void*) o->ppout);} struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate*
_temp40=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp40->ci= 0; _temp40->cc= 0; _temp40->cl= 1; _temp40->pw= w; _temp40->epw= w;
_temp40;})); return( struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)(
void* x)) Cyc_PP_list_of_alist)(( void*) o->ppout), _tag_arr("", sizeof(
unsigned char), 1u));} struct _tuple0* Cyc_PP_string_and_links( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp42=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp42->ci= 0; _temp42->cc=
0; _temp42->cl= 1; _temp42->pw= w; _temp42->epw= w; _temp42;})); return({ struct
_tuple0* _temp41=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp41->f1=(
struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x))
Cyc_PP_list_of_alist)(( void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));
_temp41->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*)
o->links); _temp41;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0; struct
_tagged_arr Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp43=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp43->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp43;});} if( i < 0){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp44=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp44[ 0]=({ struct Cyc_Core_Failure_struct
_temp45; _temp45.tag= Cyc_Core_Failure; _temp45.f1= _tag_arr("nlblanks", sizeof(
unsigned char), 9u); _temp45;}); _temp44;}));}{ struct _handler_cons _temp46;
_push_handler(& _temp46);{ int _temp48= 0; if( setjmp( _temp46.handler)){
_temp48= 1;} if( ! _temp48){{ struct _tagged_arr _temp49=*(( struct _tagged_arr*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v, i); _npop_handler( 0u);
return _temp49;}; _pop_handler();} else{ void* _temp47=( void*) _exn_thrown;
void* _temp51= _temp47; _LL53: if( _temp51 == Cyc_Core_Not_found){ goto _LL54;}
else{ goto _LL55;} _LL55: goto _LL56; _LL54: { int _temp57= i / 8; int _temp58=
i % 8; int _temp59=( 1 + _temp57) + _temp58; struct _tagged_arr nlb= Cyc_Core_new_string(
_temp59);*(( unsigned char*) _check_unknown_subscript( nlb, sizeof(
unsigned char), 0))='\n';{ int j= 0; for( 0; j < _temp57; j ++){*((
unsigned char*) _check_unknown_subscript( nlb, sizeof( unsigned char), j + 1))='\t';}}{
int j= 0; for( 0; j < _temp58; j ++){*(( unsigned char*)
_check_unknown_subscript( nlb, sizeof( unsigned char),( j + 1) + _temp57))=' ';}}((
void(*)( struct Cyc_Hashtable_Table* t, int key, struct _tagged_arr* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v,
i,({ struct _tagged_arr* _temp60=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp60[ 0]=( struct _tagged_arr) nlb; _temp60;}));
return( struct _tagged_arr) nlb;} _LL56:( void) _throw( _temp51); _LL52:;}}}}
static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999;
struct _tuple1{ int f1; struct _tagged_arr f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f(
struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct _tuple1 _temp63; struct
_tagged_arr _temp64; int _temp66; struct _tuple1* _temp61= clo; _temp63=*
_temp61; _LL67: _temp66= _temp63.f1; goto _LL65; _LL65: _temp64= _temp63.f2;
goto _LL62; _LL62: return({ struct Cyc_PP_Out* _temp68=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp68->newcc= st->cc + _temp66;
_temp68->newcl= st->cl; _temp68->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp69=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp69[ 0]=({ struct Cyc_PP_Single_struct _temp70; _temp70.tag= Cyc_PP_Single;
_temp70.f1=( void*)({ struct _tagged_arr* _temp71=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp71[ 0]= _temp64; _temp71;});
_temp70;}); _temp69;})); _temp68->links=( void*)(( void*) Cyc_PP_Empty); _temp68;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_arr s){ int slen= Cyc_String_strlen(
s); return({ struct Cyc_PP_Doc* _temp72=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp72->mwo= slen; _temp72->mw= Cyc_PP_infinity; _temp72->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*),
struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1*
_temp73=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp73->f1=
slen; _temp73->f2= s; _temp73;})); _temp72;});} struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_arr s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl ==
0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*), int(* hash)( struct _tagged_arr*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp74=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp74->v=( void*) t; _temp74;});} else{ t=( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_str_hashtbl))->v;}{ struct
_handler_cons _temp75; _push_handler(& _temp75);{ int _temp77= 0; if( setjmp(
_temp75.handler)){ _temp77= 1;} if( ! _temp77){{ struct Cyc_PP_Doc* _temp79=((
struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct _tagged_arr* key))
Cyc_Hashtable_lookup)( t,({ struct _tagged_arr* _temp78=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp78[ 0]= s; _temp78;}));
_npop_handler( 0u); return _temp79;}; _pop_handler();} else{ void* _temp76=(
void*) _exn_thrown; void* _temp81= _temp76; _LL83: if( _temp81 == Cyc_Core_Not_found){
goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86; _LL84: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_arr* _temp87=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp87[ 0]= s; _temp87;}), d); return d;} _LL86:( void)
_throw( _temp81); _LL82:;}}}} struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_arr* s){ return Cyc_PP_text(* s);} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt=
0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt* _temp88=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp88->v=( void*) Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)); _temp88;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_PP_nil_doc_opt))->v;} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){
if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt*
_temp89=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp89->v=( void*) Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u));
_temp89;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_PP_blank_doc_opt))->v;} struct _tuple2{ int f1; struct _tagged_arr f2;
struct _tagged_arr f3; } ; struct _tuple3{ int f1; int f2; int f3; struct
_tagged_arr f4; } ; static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct _tuple2*
clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp92; struct _tagged_arr
_temp93; struct _tagged_arr _temp95; int _temp97; struct _tuple2* _temp90= clo;
_temp92=* _temp90; _LL98: _temp97= _temp92.f1; goto _LL96; _LL96: _temp95=
_temp92.f2; goto _LL94; _LL94: _temp93= _temp92.f3; goto _LL91; _LL91: return({
struct Cyc_PP_Out* _temp99=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp99->newcc= st->cc + _temp97; _temp99->newcl= st->cl; _temp99->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp103=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp103[ 0]=({ struct Cyc_PP_Single_struct
_temp104; _temp104.tag= Cyc_PP_Single; _temp104.f1=( void*)({ struct _tagged_arr*
_temp105=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp105[ 0]= _temp95; _temp105;}); _temp104;}); _temp103;})); _temp99->links=(
void*)(( void*)({ struct Cyc_PP_Single_struct* _temp100=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp100[ 0]=({ struct Cyc_PP_Single_struct
_temp101; _temp101.tag= Cyc_PP_Single; _temp101.f1=( void*)({ struct _tuple3*
_temp102=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp102->f1=
st->cl; _temp102->f2= st->cc; _temp102->f3= _temp97; _temp102->f4= _temp93;
_temp102;}); _temp101;}); _temp100;})); _temp99;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink(
struct _tagged_arr shrt, struct _tagged_arr full){ int slen= Cyc_String_strlen(
shrt); return({ struct Cyc_PP_Doc* _temp106=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp106->mwo= slen; _temp106->mw= Cyc_PP_infinity;
_temp106->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple2*,
struct Cyc_PP_Ppstate*), struct _tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({
struct _tuple2* _temp107=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp107->f1= slen; _temp107->f2= shrt; _temp107->f3= full; _temp107;}));
_temp106;});} static struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){
return({ struct Cyc_PP_Out* _temp108=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp108->newcc= st->ci; _temp108->newcl= st->cl + 1;
_temp108->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp109=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp109[ 0]=({ struct Cyc_PP_Single_struct _temp110; _temp110.tag= Cyc_PP_Single;
_temp110.f1=( void*)({ struct _tagged_arr* _temp111=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp111[ 0]= Cyc_PP_nlblanks( st->ci);
_temp111;}); _temp110;}); _temp109;})); _temp108->links=( void*)(( void*) Cyc_PP_Empty);
_temp108;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp112=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp112->v=( void*)({ struct Cyc_PP_Doc*
_temp113=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp113->mwo=
0; _temp113->mw= 0; _temp113->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp113;}); _temp112;});}
return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_line_doc_opt))->v;}
struct _tuple4{ int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp116; struct
Cyc_PP_Doc* _temp117; int _temp119; struct _tuple4* _temp114= clo; _temp116=*
_temp114; _LL120: _temp119= _temp116.f1; goto _LL118; _LL118: _temp117= _temp116.f2;
goto _LL115; _LL115: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp121=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp121->ci= st->ci + _temp119; _temp121->cc= st->cc; _temp121->cl= st->cl;
_temp121->pw= st->pw; _temp121->epw= st->epw; _temp121;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp117->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp122=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp122->mwo= d->mwo; _temp122->mw= d->mw; _temp122->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp123=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp123->f1= k; _temp123->f2= d;
_temp123;})); _temp122;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp126; struct Cyc_PP_Doc* _temp127; struct Cyc_PP_Doc*
_temp129; struct _tuple5* _temp124= clo; _temp126=* _temp124; _LL130: _temp129=
_temp126.f1; goto _LL128; _LL128: _temp127= _temp126.f2; goto _LL125; _LL125: {
int epw2= Cyc_PP_max( st->pw - _temp127->mw, st->epw - _temp129->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp133=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp133->ci= st->ci;
_temp133->cc= st->cc; _temp133->cl= st->cl; _temp133->pw= st->pw; _temp133->epw=
epw2; _temp133;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp129->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp132=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp132->ci= st->ci;
_temp132->cc= o1->newcc; _temp132->cl= o1->newcl; _temp132->pw= st->pw; _temp132->epw=
epw2; _temp132;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp127->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp131=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp131->newcc= o2->newcc; _temp131->newcl= o2->newcl;
_temp131->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp131->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp131;});
return o3;}} static struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp134=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp134->mwo= Cyc_PP_min( d1->mw, d1->mwo
+ d2->mwo); _temp134->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp134->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*),
struct _tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp135=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp135->f1= d1; _temp135->f2=
d2; _temp135;})); _temp134;});} struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr l){ struct Cyc_PP_Doc* _temp136= Cyc_PP_nil_doc();{ int i=( int)(
_get_arr_size( l, sizeof( struct Cyc_PP_Doc*)) - 1); for( 0; i >= 0; i --){
_temp136= Cyc_PP_concat(*(( struct Cyc_PP_Doc**) _check_unknown_subscript( l,
sizeof( struct Cyc_PP_Doc*), i)), _temp136);}} return _temp136;} static struct
Cyc_PP_Out* Cyc_PP_long_cats_f( struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate*
st){ struct Cyc_List_List* os= 0;{ struct Cyc_List_List* _temp137= ds0; for( 0;
_temp137 != 0; _temp137=(( struct Cyc_List_List*) _check_null( _temp137))->tl){
struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(
_temp137))->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct
Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate*
_temp138=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp138->ci= st->ci; _temp138->cc= o->newcc; _temp138->cl= o->newcl; _temp138->pw=
st->pw; _temp138->epw= st->epw - d->mwo; _temp138;}); os=({ struct Cyc_List_List*
_temp139=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp139->hd=( void*) o; _temp139->tl= os; _temp139;});}}{ int newcc=(( struct
Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcc; int newcl=((
struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcl; void*
s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=((
struct Cyc_List_List*) _check_null( os))->tl){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->links,
links);} return({ struct Cyc_PP_Out* _temp140=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp140->newcc= newcc; _temp140->newcl= newcl;
_temp140->ppout=( void*) s; _temp140->links=( void*) links; _temp140;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_cats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( doclist))->hd; doclist=(( struct Cyc_List_List*)
_check_null( doclist))->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List*
_temp141= doclist; for( 0; _temp141 != 0; _temp141=(( struct Cyc_List_List*)
_check_null( _temp141))->tl){ int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( _temp141))->hd)->mwo); int mwo2= Cyc_PP_min(
mw, mwo +(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( _temp141))->hd)->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp142=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp142->mwo= mw; _temp142->mw= mwo;
_temp142->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*,
struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_cats_f,
orig); _temp142;});}} struct Cyc_PP_Doc* Cyc_PP_cats( struct Cyc_List_List*
doclist){ if( doclist == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( doclist))->tl == 0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
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
struct Cyc_PP_Ppstate* _temp143=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp143->ci= st->ci; _temp143->cc= o->newcc;
_temp143->cl= o->newcl; _temp143->pw= st->pw; _temp143->epw= st->epw - d->mwo;
_temp143;}); os=({ struct Cyc_List_List* _temp144=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp144->hd=( void*) o; _temp144->tl=
os; _temp144;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os != 0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp145=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp145->newcc=
newcc; _temp145->newcl= newcl; _temp145->ppout=( void*) s; _temp145->links=(
void*) links; _temp145;});}} struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct
_tagged_arr docs){ int sz=( int) _get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));
if( sz == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp146=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp146[ 0]=({ struct Cyc_Core_Failure_struct _temp147; _temp147.tag= Cyc_Core_Failure;
_temp147.f1= _tag_arr("cats_arr -- size zero array", sizeof( unsigned char), 28u);
_temp147;}); _temp146;}));}{ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), 0)); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo +(*((
struct Cyc_PP_Doc**) _check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*),
i)))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), i)))->mw); mw= mw2;
mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp148=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp148->mwo= mw; _temp148->mw= mwo;
_temp148->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_arr*,
struct Cyc_PP_Ppstate*), struct _tagged_arr* x)) Cyc_Fn_make_fn)( Cyc_PP_cats_arr_f,({
struct _tagged_arr* _temp149=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp149[ 0]= docs; _temp149;})); _temp148;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp152; struct Cyc_PP_Doc* _temp153; struct Cyc_PP_Doc*
_temp155; struct _tuple5* _temp150= clo; _temp152=* _temp150; _LL156: _temp155=
_temp152.f1; goto _LL154; _LL154: _temp153= _temp152.f2; goto _LL151; _LL151: {
int dfits= st->cc + _temp155->mwo <= st->epw? 1: st->cc + _temp155->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp155->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp153->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp157=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp157->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp157->mw=
Cyc_PP_min( d->mw, d2->mw); _temp157->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp158=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp158->f1= d; _temp158->f2= d2; _temp158;}));
_temp157;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp159=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp159->ci= st->cc; _temp159->cc= st->cc;
_temp159->cl= st->cl; _temp159->pw= st->pw; _temp159->epw= st->epw; _temp159;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp160=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp160->mwo= d->mwo; _temp160->mw= d->mw;
_temp160->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp160;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null( l))->tl
== 0){ return pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);} else{
return({ struct Cyc_PP_Doc*(* _temp161)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp164= pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
struct Cyc_PP_Doc* _temp165= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp166= Cyc_PP_oline_doc();
struct Cyc_PP_Doc* _temp167= Cyc_PP_ppseq_f( pp, sep,(( struct Cyc_List_List*)
_check_null( l))->tl); struct Cyc_PP_Doc* _temp162[ 4u]={ _temp164, _temp165,
_temp166, _temp167}; struct _tagged_arr _temp163={( void*) _temp162,( void*)
_temp162,( void*)( _temp162 + 4u)}; _temp161( _temp163);});}}} struct Cyc_PP_Doc*
Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct
Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_ppseq_f( pp, sep, l0));} struct
Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_arr sep, struct Cyc_List_List* l){ if(
l == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl == 0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( l))->hd;} else{ struct Cyc_PP_Doc* sep2= Cyc_PP_text( sep); struct
Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List* _temp168= l; while(((
struct Cyc_List_List*) _check_null( _temp168))->tl != 0) { struct Cyc_List_List*
_temp169=(( struct Cyc_List_List*) _check_null( _temp168))->tl;(( struct Cyc_List_List*)
_check_null( _temp168))->tl=({ struct Cyc_List_List* _temp170=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp170->hd=( void*) sep2; _temp170->tl=({
struct Cyc_List_List* _temp171=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp171->hd=( void*) oline; _temp171->tl= _temp169;
_temp171;}); _temp170;}); _temp168= _temp169;} return Cyc_PP_cats( l);}}} struct
Cyc_PP_Doc* Cyc_PP_seq( struct _tagged_arr sep, struct Cyc_List_List* l0){
return Cyc_PP_tab( Cyc_PP_seq_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l){ if( l == 0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl == 0){ return pp(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*(* _temp172)( struct
_tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp175= pp(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd); struct Cyc_PP_Doc* _temp176= Cyc_PP_text(
sep); struct Cyc_PP_Doc* _temp177= Cyc_PP_line_doc(); struct Cyc_PP_Doc*
_temp178= Cyc_PP_ppseql_f( pp, sep,(( struct Cyc_List_List*) _check_null( l))->tl);
struct Cyc_PP_Doc* _temp173[ 4u]={ _temp175, _temp176, _temp177, _temp178};
struct _tagged_arr _temp174={( void*) _temp173,( void*) _temp173,( void*)(
_temp173 + 4u)}; _temp172( _temp174);});}}} struct Cyc_PP_Doc* Cyc_PP_ppseql(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l0){ return Cyc_PP_tab( Cyc_PP_ppseql_f( pp, sep, l0));} static struct Cyc_PP_Doc*
Cyc_PP_seql_f( struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null( l))->tl
== 0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;}
else{ return({ struct Cyc_PP_Doc*(* _temp179)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp182=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( l))->hd; struct Cyc_PP_Doc* _temp183= Cyc_PP_text( sep); struct Cyc_PP_Doc*
_temp184= Cyc_PP_line_doc(); struct Cyc_PP_Doc* _temp185= Cyc_PP_seql_f( sep,((
struct Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp180[ 4u]={
_temp182, _temp183, _temp184, _temp185}; struct _tagged_arr _temp181={( void*)
_temp180,( void*) _temp180,( void*)( _temp180 + 4u)}; _temp179( _temp181);});}}}
struct Cyc_PP_Doc* Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0){
return Cyc_PP_tab( Cyc_PP_seql_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_group(
struct _tagged_arr start, struct _tagged_arr stop, struct _tagged_arr sep,
struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*(* _temp186)( struct
_tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp189= Cyc_PP_text( start);
struct Cyc_PP_Doc* _temp190= Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc* _temp191=
Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp187[ 3u]={ _temp189, _temp190,
_temp191}; struct _tagged_arr _temp188={( void*) _temp187,( void*) _temp187,(
void*)( _temp187 + 3u)}; _temp186( _temp188);});} struct Cyc_PP_Doc* Cyc_PP_egroup(
struct _tagged_arr start, struct _tagged_arr stop, struct _tagged_arr sep,
struct Cyc_List_List* ss){ if( ss == 0){ return Cyc_PP_nil_doc();} else{ return({
struct Cyc_PP_Doc*(* _temp192)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp195= Cyc_PP_text( start); struct Cyc_PP_Doc* _temp196= Cyc_PP_seq( sep, ss);
struct Cyc_PP_Doc* _temp197= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp193[ 3u]={
_temp195, _temp196, _temp197}; struct _tagged_arr _temp194={( void*) _temp193,(
void*) _temp193,( void*)( _temp193 + 3u)}; _temp192( _temp194);});}} struct Cyc_PP_Doc*
Cyc_PP_groupl( struct _tagged_arr start, struct _tagged_arr stop, struct
_tagged_arr sep, struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*(*
_temp198)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp201= Cyc_PP_text(
start); struct Cyc_PP_Doc* _temp202= Cyc_PP_seql( sep, ss); struct Cyc_PP_Doc*
_temp203= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp199[ 3u]={ _temp201,
_temp202, _temp203}; struct _tagged_arr _temp200={( void*) _temp199,( void*)
_temp199,( void*)( _temp199 + 3u)}; _temp198( _temp200);});}