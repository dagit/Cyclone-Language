 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern void* Cyc_Core_identity(
void*); extern int Cyc_Core_intcmp( int, int); extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE; extern unsigned char
Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u];
struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char
Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[ 18u];
extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern struct _tagged_arr Cyc_Std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); extern int Cyc_Hashtable_hash_stringptr( struct _tagged_arr* s); static
const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*,
void*); void* f2; } ; extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*),
void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply(
void* f, void* x); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
extern void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Std___sFILE*
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
a1 == ( void*) Cyc_PP_Empty){ return a2;} else{ if( a2 == ( void*) Cyc_PP_Empty){
return a1;} else{ return( void*)({ struct Cyc_PP_Append_struct* _temp0=( struct
Cyc_PP_Append_struct*) GC_malloc( sizeof( struct Cyc_PP_Append_struct)); _temp0[
0]=({ struct Cyc_PP_Append_struct _temp1; _temp1.tag= Cyc_PP_Append; _temp1.f1=(
void*) a1; _temp1.f2=( void*) a2; _temp1;}); _temp0;});}}} struct Cyc_List_List*
Cyc_PP_list_of_alist_f( void* y, struct Cyc_List_List* l){ void* _temp2= y; void*
_temp10; void* _temp12; void* _temp14; _LL4: if( _temp2 == ( void*) Cyc_PP_Empty){
goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp2 >  1u?*(( int*)
_temp2) ==  Cyc_PP_Single: 0){ _LL11: _temp10=( void*)(( struct Cyc_PP_Single_struct*)
_temp2)->f1; goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp2 >  1u?*((
int*) _temp2) ==  Cyc_PP_Append: 0){ _LL15: _temp14=( void*)(( struct Cyc_PP_Append_struct*)
_temp2)->f1; goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_PP_Append_struct*)
_temp2)->f2; goto _LL9;} else{ goto _LL3;} _LL5: return l; _LL7: return({ struct
Cyc_List_List* _temp16=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp16->hd=( void*) _temp10; _temp16->tl= l; _temp16;}); _LL9: return Cyc_PP_list_of_alist_f(
_temp14, Cyc_PP_list_of_alist_f( _temp12, l)); _LL3:;} struct Cyc_List_List* Cyc_PP_list_of_alist(
void* x){ return Cyc_PP_list_of_alist_f( x, 0);} struct Cyc_PP_Ppstate{ int ci;
int cc; int cl; int pw; int epw; } ; struct Cyc_PP_Out{ int newcc; int newcl;
void* ppout; void* links; } ; struct Cyc_PP_Doc{ int mwo; int mw; void* f; } ;
static void Cyc_PP_dump_out( struct Cyc_Std___sFILE* f, void* al){ struct Cyc_Xarray_Xarray*
_temp17= Cyc_Xarray_create( 16, al); Cyc_Xarray_add( _temp17, al);{ int last= 0;
while( last >=  0) { void* _temp18= Cyc_Xarray_get( _temp17, last); struct
_tagged_arr* _temp26; void* _temp28; void* _temp30; _LL20: if( _temp18 == ( void*)
Cyc_PP_Empty){ goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp18
>  1u?*(( int*) _temp18) ==  Cyc_PP_Single: 0){ _LL27: _temp26=( struct
_tagged_arr*)(( struct Cyc_PP_Single_struct*) _temp18)->f1; goto _LL23;} else{
goto _LL24;} _LL24: if(( unsigned int) _temp18 >  1u?*(( int*) _temp18) ==  Cyc_PP_Append:
0){ _LL31: _temp30=( void*)(( struct Cyc_PP_Append_struct*) _temp18)->f1; goto
_LL29; _LL29: _temp28=( void*)(( struct Cyc_PP_Append_struct*) _temp18)->f2;
goto _LL25;} else{ goto _LL19;} _LL21: -- last; goto _LL19; _LL23: -- last;({
struct Cyc_Std_String_pa_struct _temp33; _temp33.tag= Cyc_Std_String_pa; _temp33.f1=(
struct _tagged_arr)* _temp26;{ void* _temp32[ 1u]={& _temp33}; Cyc_Std_fprintf(
f, _tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp32, sizeof( void*),
1u));}}); goto _LL19; _LL25: Cyc_Xarray_set( _temp17, last, _temp28); if( last
==  Cyc_Xarray_length( _temp17) -  1){ Cyc_Xarray_add( _temp17, _temp30);} else{
Cyc_Xarray_set( _temp17, last +  1, _temp30);} ++ last; goto _LL19; _LL19:;}}}
void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Std___sFILE* f){
struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp34=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp34->ci= 0; _temp34->cc=
0; _temp34->cl= 1; _temp34->pw= w; _temp34->epw= w; _temp34;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp35=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp35->ci= 0; _temp35->cc=
0; _temp35->cl= 1; _temp35->pw= w; _temp35->epw= w; _temp35;})); return( struct
_tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));} struct _tuple0*
Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp37=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp37->ci= 0; _temp37->cc=
0; _temp37->cl= 1; _temp37->pw= w; _temp37->epw= w; _temp37;})); return({ struct
_tuple0* _temp36=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp36->f1=(
struct _tagged_arr) Cyc_Std_str_sepstr((( struct Cyc_List_List*(*)( void* x))
Cyc_PP_list_of_alist)(( void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));
_temp36->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*)
o->links); _temp36;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0; struct
_tagged_arr Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl ==  0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp38=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp38->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp38;});} if( i <  0){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp39=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp39[ 0]=({ struct Cyc_Core_Failure_struct
_temp40; _temp40.tag= Cyc_Core_Failure; _temp40.f1= _tag_arr("nlblanks", sizeof(
unsigned char), 9u); _temp40;}); _temp39;}));}{ struct _handler_cons _temp41;
_push_handler(& _temp41);{ int _temp43= 0; if( setjmp( _temp41.handler)){
_temp43= 1;} if( ! _temp43){{ struct _tagged_arr _temp44=*(( struct _tagged_arr*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v, i); _npop_handler( 0u);
return _temp44;}; _pop_handler();} else{ void* _temp42=( void*) _exn_thrown;
void* _temp46= _temp42; _LL48: if( _temp46 ==  Cyc_Core_Not_found){ goto _LL49;}
else{ goto _LL50;} _LL50: goto _LL51; _LL49: { int _temp52= i /  8; int _temp53=
i %  8; int _temp54=( 1 +  _temp52) +  _temp53; struct _tagged_arr nlb= Cyc_Core_new_string(
_temp54);*(( unsigned char*) _check_unknown_subscript( nlb, sizeof(
unsigned char), 0))='\n';{ int j= 0; for( 0; j <  _temp52; j ++){*((
unsigned char*) _check_unknown_subscript( nlb, sizeof( unsigned char), j +  1))='\t';}}{
int j= 0; for( 0; j <  _temp53; j ++){*(( unsigned char*)
_check_unknown_subscript( nlb, sizeof( unsigned char),( j +  1) +  _temp52))=' ';}}((
void(*)( struct Cyc_Hashtable_Table* t, int key, struct _tagged_arr* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v,
i,({ struct _tagged_arr* _temp55=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) *  1); _temp55[ 0]=( struct _tagged_arr) nlb; _temp55;}));
return( struct _tagged_arr) nlb;} _LL51:( void) _throw( _temp46); _LL47:;}}}}
static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999;
struct _tuple1{ int f1; struct _tagged_arr f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f(
struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct _tuple1 _temp58; struct
_tagged_arr _temp59; int _temp61; struct _tuple1* _temp56= clo; _temp58=*
_temp56; _LL62: _temp61= _temp58.f1; goto _LL60; _LL60: _temp59= _temp58.f2;
goto _LL57; _LL57: return({ struct Cyc_PP_Out* _temp63=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp63->newcc= st->cc +  _temp61;
_temp63->newcl= st->cl; _temp63->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp64=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp64[ 0]=({ struct Cyc_PP_Single_struct _temp65; _temp65.tag= Cyc_PP_Single;
_temp65.f1=( void*)({ struct _tagged_arr* _temp66=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) *  1); _temp66[ 0]= _temp59; _temp66;});
_temp65;}); _temp64;})); _temp63->links=( void*)(( void*) Cyc_PP_Empty); _temp63;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_arr s){ int slen=( int)
Cyc_Std_strlen( s); return({ struct Cyc_PP_Doc* _temp67=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp67->mwo= slen; _temp67->mw= Cyc_PP_infinity;
_temp67->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct
Cyc_PP_Ppstate*), struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({
struct _tuple1* _temp68=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp68->f1= slen; _temp68->f2= s; _temp68;})); _temp67;});} struct Cyc_PP_Doc*
Cyc_PP_text( struct _tagged_arr s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl
==  0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*), int(* hash)( struct _tagged_arr*))) Cyc_Hashtable_create)(
101, Cyc_Std_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp69=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp69->v=( void*) t; _temp69;});} else{ t=( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_str_hashtbl))->v;}{ struct
_handler_cons _temp70; _push_handler(& _temp70);{ int _temp72= 0; if( setjmp(
_temp70.handler)){ _temp72= 1;} if( ! _temp72){{ struct Cyc_PP_Doc* _temp74=((
struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct _tagged_arr* key))
Cyc_Hashtable_lookup)( t,({ struct _tagged_arr* _temp73=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp73[ 0]= s; _temp73;}));
_npop_handler( 0u); return _temp74;}; _pop_handler();} else{ void* _temp71=(
void*) _exn_thrown; void* _temp76= _temp71; _LL78: if( _temp76 ==  Cyc_Core_Not_found){
goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL79: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_arr* _temp82=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp82[ 0]= s; _temp82;}), d); return d;} _LL81:( void) _throw(
_temp76); _LL77:;}}}} struct Cyc_PP_Doc* Cyc_PP_textptr( struct _tagged_arr* s){
return Cyc_PP_text(* s);} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt= 0; struct Cyc_Core_Opt*
Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt= 0; struct Cyc_PP_Doc*
Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt ==  0){ Cyc_PP_nil_doc_opt=({ struct
Cyc_Core_Opt* _temp83=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp83->v=( void*) Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u));
_temp83;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_PP_nil_doc_opt))->v;} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){ if( Cyc_PP_blank_doc_opt
==  0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt* _temp84=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp84->v=( void*) Cyc_PP_text(
_tag_arr("", sizeof( unsigned char), 1u)); _temp84;});} return( struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_blank_doc_opt))->v;} struct _tuple2{
int f1; struct _tagged_arr f2; struct _tagged_arr f3; } ; struct _tuple3{ int f1;
int f2; int f3; struct _tagged_arr f4; } ; static struct Cyc_PP_Out* Cyc_PP_hyperlink_f(
struct _tuple2* clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp87; struct
_tagged_arr _temp88; struct _tagged_arr _temp90; int _temp92; struct _tuple2*
_temp85= clo; _temp87=* _temp85; _LL93: _temp92= _temp87.f1; goto _LL91; _LL91:
_temp90= _temp87.f2; goto _LL89; _LL89: _temp88= _temp87.f3; goto _LL86; _LL86:
return({ struct Cyc_PP_Out* _temp94=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp94->newcc= st->cc +  _temp92; _temp94->newcl= st->cl;
_temp94->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp98=( struct
Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp98[
0]=({ struct Cyc_PP_Single_struct _temp99; _temp99.tag= Cyc_PP_Single; _temp99.f1=(
void*)({ struct _tagged_arr* _temp100=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) *  1); _temp100[ 0]= _temp90; _temp100;}); _temp99;});
_temp98;})); _temp94->links=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp95=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp95[ 0]=({ struct Cyc_PP_Single_struct _temp96; _temp96.tag= Cyc_PP_Single;
_temp96.f1=( void*)({ struct _tuple3* _temp97=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp97->f1= st->cl; _temp97->f2= st->cc; _temp97->f3=
_temp92; _temp97->f4= _temp88; _temp97;}); _temp96;}); _temp95;})); _temp94;});}
struct Cyc_PP_Doc* Cyc_PP_hyperlink( struct _tagged_arr shrt, struct _tagged_arr
full){ int slen=( int) Cyc_Std_strlen( shrt); return({ struct Cyc_PP_Doc*
_temp101=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp101->mwo=
slen; _temp101->mw= Cyc_PP_infinity; _temp101->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple2*, struct Cyc_PP_Ppstate*), struct _tuple2* x)) Cyc_Fn_make_fn)(
Cyc_PP_hyperlink_f,({ struct _tuple2* _temp102=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp102->f1= slen; _temp102->f2= shrt; _temp102->f3=
full; _temp102;})); _temp101;});} static struct Cyc_PP_Out* Cyc_PP_line_f(
struct Cyc_PP_Ppstate* st){ return({ struct Cyc_PP_Out* _temp103=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp103->newcc= st->ci; _temp103->newcl=
st->cl +  1; _temp103->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp104=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp104[ 0]=({ struct Cyc_PP_Single_struct _temp105; _temp105.tag= Cyc_PP_Single;
_temp105.f1=( void*)({ struct _tagged_arr* _temp106=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) *  1); _temp106[ 0]= Cyc_PP_nlblanks( st->ci);
_temp106;}); _temp105;}); _temp104;})); _temp103->links=( void*)(( void*) Cyc_PP_Empty);
_temp103;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 
0){ Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp107=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp107->v=( void*)({ struct Cyc_PP_Doc*
_temp108=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp108->mwo=
0; _temp108->mw= 0; _temp108->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp108;}); _temp107;});}
return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_line_doc_opt))->v;}
struct _tuple4{ int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp111; struct
Cyc_PP_Doc* _temp112; int _temp114; struct _tuple4* _temp109= clo; _temp111=*
_temp109; _LL115: _temp114= _temp111.f1; goto _LL113; _LL113: _temp112= _temp111.f2;
goto _LL110; _LL110: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp116=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp116->ci= st->ci +  _temp114; _temp116->cc= st->cc; _temp116->cl= st->cl;
_temp116->pw= st->pw; _temp116->epw= st->epw; _temp116;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp112->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp117=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp117->mwo= d->mwo; _temp117->mw= d->mw; _temp117->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp118=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp118->f1= k; _temp118->f2= d;
_temp118;})); _temp117;});} int Cyc_PP_min( int x, int y){ if( x <=  y){ return
x;} else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >=  y){ return x;}
else{ return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2;
} ; static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp121; struct Cyc_PP_Doc* _temp122; struct Cyc_PP_Doc*
_temp124; struct _tuple5* _temp119= clo; _temp121=* _temp119; _LL125: _temp124=
_temp121.f1; goto _LL123; _LL123: _temp122= _temp121.f2; goto _LL120; _LL120: {
int epw2= Cyc_PP_max( st->pw -  _temp122->mw, st->epw -  _temp124->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp128=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp128->ci= st->ci;
_temp128->cc= st->cc; _temp128->cl= st->cl; _temp128->pw= st->pw; _temp128->epw=
epw2; _temp128;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp124->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp127=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp127->ci= st->ci;
_temp127->cc= o1->newcc; _temp127->cl= o1->newcl; _temp127->pw= st->pw; _temp127->epw=
epw2; _temp127;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp122->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp126=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp126->newcc= o2->newcc; _temp126->newcl= o2->newcl;
_temp126->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp126->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp126;});
return o3;}} static struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp129=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp129->mwo= Cyc_PP_min( d1->mw, d1->mwo
+  d2->mwo); _temp129->mw= Cyc_PP_min( d1->mw, d1->mwo +  d2->mw); _temp129->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*),
struct _tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp130=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp130->f1= d1; _temp130->f2=
d2; _temp130;})); _temp129;});} struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr l){ struct Cyc_PP_Doc* _temp131= Cyc_PP_nil_doc();{ int i=( int)(
_get_arr_size( l, sizeof( struct Cyc_PP_Doc*)) -  1); for( 0; i >=  0; i --){
_temp131= Cyc_PP_concat(*(( struct Cyc_PP_Doc**) _check_unknown_subscript( l,
sizeof( struct Cyc_PP_Doc*), i)), _temp131);}} return _temp131;} static struct
Cyc_PP_Out* Cyc_PP_long_cats_f( struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate*
st){ struct Cyc_List_List* os= 0;{ struct Cyc_List_List* _temp132= ds0; for( 0;
_temp132 !=  0; _temp132=(( struct Cyc_List_List*) _check_null( _temp132))->tl){
struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(
_temp132))->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct
Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate*
_temp133=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp133->ci= st->ci; _temp133->cc= o->newcc; _temp133->cl= o->newcl; _temp133->pw=
st->pw; _temp133->epw= st->epw -  d->mwo; _temp133;}); os=({ struct Cyc_List_List*
_temp134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp134->hd=( void*) o; _temp134->tl= os; _temp134;});}}{ int newcc=(( struct
Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcc; int newcl=((
struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcl; void*
s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os !=  0; os=((
struct Cyc_List_List*) _check_null( os))->tl){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->links,
links);} return({ struct Cyc_PP_Out* _temp135=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp135->newcc= newcc; _temp135->newcl= newcl;
_temp135->ppout=( void*) s; _temp135->links=( void*) links; _temp135;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_cats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( doclist))->hd; doclist=(( struct Cyc_List_List*)
_check_null( doclist))->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List*
_temp136= doclist; for( 0; _temp136 !=  0; _temp136=(( struct Cyc_List_List*)
_check_null( _temp136))->tl){ int mw2= Cyc_PP_min( mw, mwo + (( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( _temp136))->hd)->mwo); int mwo2= Cyc_PP_min(
mw, mwo + (( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( _temp136))->hd)->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp137=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp137->mwo= mw; _temp137->mw= mwo;
_temp137->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*,
struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_cats_f,
orig); _temp137;});}} struct Cyc_PP_Doc* Cyc_PP_cats( struct Cyc_List_List*
doclist){ if( doclist ==  0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( doclist))->tl ==  0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( doclist))->hd;} else{ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
doclist) >  30){ return Cyc_PP_long_cats( doclist);} else{ return Cyc_PP_concat((
struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( doclist))->hd, Cyc_PP_cats(((
struct Cyc_List_List*) _check_null( doclist))->tl));}}}} static struct Cyc_PP_Out*
Cyc_PP_cats_arr_f( struct _tagged_arr* docs_ptr, struct Cyc_PP_Ppstate* st){
struct Cyc_List_List* os= 0; struct _tagged_arr docs=* docs_ptr; int sz=( int)
_get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));{ int i= 0; for( 0; i <  sz;
++ i){ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**) _check_unknown_subscript(
docs, sizeof( struct Cyc_PP_Doc*), i)); struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({
struct Cyc_PP_Ppstate* _temp138=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp138->ci= st->ci; _temp138->cc= o->newcc;
_temp138->cl= o->newcl; _temp138->pw= st->pw; _temp138->epw= st->epw -  d->mwo;
_temp138;}); os=({ struct Cyc_List_List* _temp139=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp139->hd=( void*) o; _temp139->tl=
os; _temp139;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os !=  0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp140=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp140->newcc=
newcc; _temp140->newcl= newcl; _temp140->ppout=( void*) s; _temp140->links=(
void*) links; _temp140;});}} struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct
_tagged_arr docs){ int sz=( int) _get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));
if( sz ==  0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp141=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp141[ 0]=({ struct Cyc_Core_Failure_struct _temp142; _temp142.tag= Cyc_Core_Failure;
_temp142.f1= _tag_arr("cats_arr -- size zero array", sizeof( unsigned char), 28u);
_temp142;}); _temp141;}));}{ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), 0)); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i <  sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo
+ (*(( struct Cyc_PP_Doc**) _check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*),
i)))->mwo); int mwo2= Cyc_PP_min( mw, mwo + (*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), i)))->mw); mw= mw2;
mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp143=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp143->mwo= mw; _temp143->mw= mwo;
_temp143->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_arr*,
struct Cyc_PP_Ppstate*), struct _tagged_arr* x)) Cyc_Fn_make_fn)( Cyc_PP_cats_arr_f,({
struct _tagged_arr* _temp144=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) *  1); _temp144[ 0]= docs; _temp144;})); _temp143;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp147; struct Cyc_PP_Doc* _temp148; struct Cyc_PP_Doc*
_temp150; struct _tuple5* _temp145= clo; _temp147=* _temp145; _LL151: _temp150=
_temp147.f1; goto _LL149; _LL149: _temp148= _temp147.f2; goto _LL146; _LL146: {
int dfits= st->cc +  _temp150->mwo <=  st->epw? 1: st->cc +  _temp150->mw <=  st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp150->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp148->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp152=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp152->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp152->mw=
Cyc_PP_min( d->mw, d2->mw); _temp152->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp153=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp153->f1= d; _temp153->f2= d2; _temp153;}));
_temp152;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp154=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp154->ci= st->cc; _temp154->cc= st->cc;
_temp154->cl= st->cl; _temp154->pw= st->pw; _temp154->epw= st->epw; _temp154;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp155=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp155->mwo= d->mwo; _temp155->mw= d->mw;
_temp155->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp155;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_arr sep, struct Cyc_List_List* l){ if( l ==  0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null( l))->tl
==  0){ return pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);}
else{ return({ struct Cyc_PP_Doc* _temp156[ 4u]; _temp156[ 3u]= Cyc_PP_ppseq_f(
pp, sep,(( struct Cyc_List_List*) _check_null( l))->tl); _temp156[ 2u]= Cyc_PP_oline_doc();
_temp156[ 1u]= Cyc_PP_text( sep); _temp156[ 0u]= pp(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_PP_cat( _tag_arr( _temp156, sizeof( struct Cyc_PP_Doc*),
4u));});}}} struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_ppseq_f(
pp, sep, l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct _tagged_arr sep, struct
Cyc_List_List* l){ if( l ==  0){ return Cyc_PP_nil_doc();} else{ if((( struct
Cyc_List_List*) _check_null( l))->tl ==  0){ return( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( l))->hd;} else{ struct Cyc_PP_Doc* sep2= Cyc_PP_text(
sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct Cyc_List_List*
_temp157= l; while((( struct Cyc_List_List*) _check_null( _temp157))->tl !=  0) {
struct Cyc_List_List* _temp158=(( struct Cyc_List_List*) _check_null( _temp157))->tl;((
struct Cyc_List_List*) _check_null( _temp157))->tl=({ struct Cyc_List_List*
_temp159=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp159->hd=( void*) sep2; _temp159->tl=({ struct Cyc_List_List* _temp160=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp160->hd=(
void*) oline; _temp160->tl= _temp158; _temp160;}); _temp159;}); _temp157=
_temp158;} return Cyc_PP_cats( l);}}} struct Cyc_PP_Doc* Cyc_PP_seq( struct
_tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seq_f( sep,
l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_arr sep, struct Cyc_List_List* l){ if( l ==  0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl ==  0){ return pp(( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*
_temp161[ 4u]; _temp161[ 3u]= Cyc_PP_ppseql_f( pp, sep,(( struct Cyc_List_List*)
_check_null( l))->tl); _temp161[ 2u]= Cyc_PP_line_doc(); _temp161[ 1u]= Cyc_PP_text(
sep); _temp161[ 0u]= pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
Cyc_PP_cat( _tag_arr( _temp161, sizeof( struct Cyc_PP_Doc*), 4u));});}}} struct
Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr
sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_ppseql_f( pp, sep, l0));}
static struct Cyc_PP_Doc* Cyc_PP_seql_f( struct _tagged_arr sep, struct Cyc_List_List*
l){ if( l ==  0){ return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*)
_check_null( l))->tl ==  0){ return( struct Cyc_PP_Doc*)(( struct Cyc_List_List*)
_check_null( l))->hd;} else{ return({ struct Cyc_PP_Doc* _temp162[ 4u]; _temp162[
3u]= Cyc_PP_seql_f( sep,(( struct Cyc_List_List*) _check_null( l))->tl);
_temp162[ 2u]= Cyc_PP_line_doc(); _temp162[ 1u]= Cyc_PP_text( sep); _temp162[ 0u]=(
struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd; Cyc_PP_cat(
_tag_arr( _temp162, sizeof( struct Cyc_PP_Doc*), 4u));});}}} struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(
Cyc_PP_seql_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_arr
start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss){
return({ struct Cyc_PP_Doc* _temp163[ 3u]; _temp163[ 2u]= Cyc_PP_text( stop);
_temp163[ 1u]= Cyc_PP_seq( sep, ss); _temp163[ 0u]= Cyc_PP_text( start); Cyc_PP_cat(
_tag_arr( _temp163, sizeof( struct Cyc_PP_Doc*), 3u));});} struct Cyc_PP_Doc*
Cyc_PP_egroup( struct _tagged_arr start, struct _tagged_arr stop, struct
_tagged_arr sep, struct Cyc_List_List* ss){ if( ss ==  0){ return Cyc_PP_nil_doc();}
else{ return({ struct Cyc_PP_Doc* _temp164[ 3u]; _temp164[ 2u]= Cyc_PP_text(
stop); _temp164[ 1u]= Cyc_PP_seq( sep, ss); _temp164[ 0u]= Cyc_PP_text( start);
Cyc_PP_cat( _tag_arr( _temp164, sizeof( struct Cyc_PP_Doc*), 3u));});}} struct
Cyc_PP_Doc* Cyc_PP_groupl( struct _tagged_arr start, struct _tagged_arr stop,
struct _tagged_arr sep, struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*
_temp165[ 3u]; _temp165[ 2u]= Cyc_PP_text( stop); _temp165[ 1u]= Cyc_PP_seql(
sep, ss); _temp165[ 0u]= Cyc_PP_text( start); Cyc_PP_cat( _tag_arr( _temp165,
sizeof( struct Cyc_PP_Doc*), 3u));});}
