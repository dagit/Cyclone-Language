 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_arr Cyc_Core_new_string( int); extern void*
Cyc_Core_identity( void*); extern int Cyc_Core_intcmp( int, int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; }
; extern int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen( struct _tagged_arr s); extern
int Cyc_String_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); extern
struct _tagged_arr Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table* Cyc_Hashtable_create(
int sz, int(* cmp)( void*, void*), int(* hash)( void*)); extern void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table* t, void* key, void* val); extern void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key); extern int Cyc_Hashtable_hash_stringptr(
struct _tagged_arr* s); static const int Cyc_Fn_Fun= 0; struct Cyc_Fn_Fun_struct{
int tag; void*(* f1)( void*, void*); void* f2; } ; extern void* Cyc_Fn_make_fn(
void*(* f)( void*, void*), void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*));
extern void* Cyc_Fn_apply( void* f, void* x); struct Cyc_PP_Ppstate; struct Cyc_PP_Out;
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
int, void*)) Cyc_Xarray_create)( 16, al);(( void(*)( struct Cyc_Xarray_Xarray*,
void*)) Cyc_Xarray_add)( _temp17, al);{ int last= 0; while( last >= 0) { void*
_temp18=(( void*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)( _temp17,
last); struct _tagged_arr* _temp26; void* _temp28; void* _temp30; _LL20: if(
_temp18 ==( void*) Cyc_PP_Empty){ goto _LL21;} else{ goto _LL22;} _LL22: if((
unsigned int) _temp18 > 1u?*(( int*) _temp18) == Cyc_PP_Single: 0){ _LL27:
_temp26=( struct _tagged_arr*)(( struct Cyc_PP_Single_struct*) _temp18)->f1;
goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp18 > 1u?*(( int*)
_temp18) == Cyc_PP_Append: 0){ _LL31: _temp30=( void*)(( struct Cyc_PP_Append_struct*)
_temp18)->f1; goto _LL29; _LL29: _temp28=( void*)(( struct Cyc_PP_Append_struct*)
_temp18)->f2; goto _LL25;} else{ goto _LL19;} _LL21: -- last; goto _LL19; _LL23:
-- last;({ struct _tagged_arr _temp32=* _temp26; fprintf( _sfile_to_file( f),"%.*s",
_get_arr_size( _temp32, 1u), _temp32.curr);}); goto _LL19; _LL25:(( void(*)(
struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)( _temp17, last, _temp28);
if( last ==(( int(*)( struct Cyc_Xarray_Xarray*)) Cyc_Xarray_length)( _temp17) -
1){(( void(*)( struct Cyc_Xarray_Xarray*, void*)) Cyc_Xarray_add)( _temp17,
_temp30);} else{(( void(*)( struct Cyc_Xarray_Xarray*, int, void*)) Cyc_Xarray_set)(
_temp17, last + 1, _temp30);} ++ last; goto _LL19; _LL19:;}}} void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE* f){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp33=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp33->ci= 0; _temp33->cc=
0; _temp33->cl= 1; _temp33->pw= w; _temp33->epw= w; _temp33;})); Cyc_PP_dump_out(
f,( void*) o->ppout);} struct _tagged_arr Cyc_PP_string_of_doc( struct Cyc_PP_Doc*
d, int w){ struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate*
x)) Cyc_Fn_apply)(( void*) d->f,({ struct Cyc_PP_Ppstate* _temp34=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp34->ci= 0; _temp34->cc=
0; _temp34->cl= 1; _temp34->pw= w; _temp34->epw= w; _temp34;})); return( struct
_tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)((
void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));} struct _tuple0*
Cyc_PP_string_and_links( struct Cyc_PP_Doc* d, int w){ struct Cyc_PP_Out* o=((
struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*)
d->f,({ struct Cyc_PP_Ppstate* _temp36=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp36->ci= 0; _temp36->cc=
0; _temp36->cl= 1; _temp36->pw= w; _temp36->epw= w; _temp36;})); return({ struct
_tuple0* _temp35=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp35->f1=(
struct _tagged_arr) Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( void* x))
Cyc_PP_list_of_alist)(( void*) o->ppout), _tag_arr("", sizeof( unsigned char), 1u));
_temp35->f2=(( struct Cyc_List_List*(*)( void* x)) Cyc_PP_list_of_alist)(( void*)
o->links); _temp35;});} static struct Cyc_Core_Opt* Cyc_PP_bhashtbl= 0; struct
_tagged_arr Cyc_PP_nlblanks( int i){ if( Cyc_PP_bhashtbl == 0){ Cyc_PP_bhashtbl=({
struct Cyc_Core_Opt* _temp37=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp37->v=( void*)(( struct Cyc_Hashtable_Table*(*)( int sz,
int(* cmp)( int, int), int(* hash)( int))) Cyc_Hashtable_create)( 61, Cyc_Core_intcmp,(
int(*)( int)) Cyc_Core_identity); _temp37;});} if( i < 0){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp38=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp38[ 0]=({ struct Cyc_Core_Failure_struct
_temp39; _temp39.tag= Cyc_Core_Failure; _temp39.f1= _tag_arr("nlblanks", sizeof(
unsigned char), 9u); _temp39;}); _temp38;}));}{ struct _handler_cons _temp40;
_push_handler(& _temp40);{ int _temp42= 0; if( setjmp( _temp40.handler)){
_temp42= 1;} if( ! _temp42){{ struct _tagged_arr _temp43=*(( struct _tagged_arr*(*)(
struct Cyc_Hashtable_Table* t, int key)) Cyc_Hashtable_lookup)(( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v, i); _npop_handler( 0u);
return _temp43;}; _pop_handler();} else{ void* _temp41=( void*) _exn_thrown;
void* _temp45= _temp41; _LL47: if( _temp45 == Cyc_Core_Not_found){ goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { int _temp51= i / 8; int _temp52=
i % 8; int _temp53=( 1 + _temp51) + _temp52; struct _tagged_arr nlb= Cyc_Core_new_string(
_temp53);*(( unsigned char*) _check_unknown_subscript( nlb, sizeof(
unsigned char), 0))='\n';{ int j= 0; for( 0; j < _temp51; j ++){*((
unsigned char*) _check_unknown_subscript( nlb, sizeof( unsigned char), j + 1))='\t';}}{
int j= 0; for( 0; j < _temp52; j ++){*(( unsigned char*)
_check_unknown_subscript( nlb, sizeof( unsigned char),( j + 1) + _temp51))=' ';}}((
void(*)( struct Cyc_Hashtable_Table* t, int key, struct _tagged_arr* val)) Cyc_Hashtable_insert)((
struct Cyc_Hashtable_Table*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_bhashtbl))->v,
i,({ struct _tagged_arr* _temp54=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr) * 1); _temp54[ 0]=( struct _tagged_arr) nlb; _temp54;}));
return( struct _tagged_arr) nlb;} _LL50:( void) _throw( _temp45); _LL46:;}}}}
static struct Cyc_Core_Opt* Cyc_PP_str_hashtbl= 0; int Cyc_PP_infinity= 9999999;
struct _tuple1{ int f1; struct _tagged_arr f2; } ; static struct Cyc_PP_Out* Cyc_PP_text_doc_f(
struct _tuple1* clo, struct Cyc_PP_Ppstate* st){ struct _tuple1 _temp57; struct
_tagged_arr _temp58; int _temp60; struct _tuple1* _temp55= clo; _temp57=*
_temp55; _LL61: _temp60= _temp57.f1; goto _LL59; _LL59: _temp58= _temp57.f2;
goto _LL56; _LL56: return({ struct Cyc_PP_Out* _temp62=( struct Cyc_PP_Out*)
GC_malloc( sizeof( struct Cyc_PP_Out)); _temp62->newcc= st->cc + _temp60;
_temp62->newcl= st->cl; _temp62->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct*
_temp63=( struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp63[ 0]=({ struct Cyc_PP_Single_struct _temp64; _temp64.tag= Cyc_PP_Single;
_temp64.f1=( void*)({ struct _tagged_arr* _temp65=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp65[ 0]= _temp58; _temp65;});
_temp64;}); _temp63;})); _temp62->links=( void*)(( void*) Cyc_PP_Empty); _temp62;});}
static struct Cyc_PP_Doc* Cyc_PP_text_doc( struct _tagged_arr s){ int slen= Cyc_String_strlen(
s); return({ struct Cyc_PP_Doc* _temp66=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp66->mwo= slen; _temp66->mw= Cyc_PP_infinity; _temp66->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple1*, struct Cyc_PP_Ppstate*),
struct _tuple1* x)) Cyc_Fn_make_fn)( Cyc_PP_text_doc_f,({ struct _tuple1*
_temp67=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp67->f1=
slen; _temp67->f2= s; _temp67;})); _temp66;});} struct Cyc_PP_Doc* Cyc_PP_text(
struct _tagged_arr s){ struct Cyc_Hashtable_Table* t; if( Cyc_PP_str_hashtbl ==
0){ t=(( struct Cyc_Hashtable_Table*(*)( int sz, int(* cmp)( struct _tagged_arr*,
struct _tagged_arr*), int(* hash)( struct _tagged_arr*))) Cyc_Hashtable_create)(
101, Cyc_String_zstrptrcmp, Cyc_Hashtable_hash_stringptr); Cyc_PP_str_hashtbl=({
struct Cyc_Core_Opt* _temp68=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp68->v=( void*) t; _temp68;});} else{ t=( struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*) _check_null( Cyc_PP_str_hashtbl))->v;}{ struct
_handler_cons _temp69; _push_handler(& _temp69);{ int _temp71= 0; if( setjmp(
_temp69.handler)){ _temp71= 1;} if( ! _temp71){{ struct Cyc_PP_Doc* _temp73=((
struct Cyc_PP_Doc*(*)( struct Cyc_Hashtable_Table* t, struct _tagged_arr* key))
Cyc_Hashtable_lookup)( t,({ struct _tagged_arr* _temp72=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp72[ 0]= s; _temp72;}));
_npop_handler( 0u); return _temp73;}; _pop_handler();} else{ void* _temp70=(
void*) _exn_thrown; void* _temp75= _temp70; _LL77: if( _temp75 == Cyc_Core_Not_found){
goto _LL78;} else{ goto _LL79;} _LL79: goto _LL80; _LL78: { struct Cyc_PP_Doc* d=
Cyc_PP_text_doc( s);(( void(*)( struct Cyc_Hashtable_Table* t, struct
_tagged_arr* key, struct Cyc_PP_Doc* val)) Cyc_Hashtable_insert)( t,({ struct
_tagged_arr* _temp81=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp81[ 0]= s; _temp81;}), d); return d;} _LL80:( void)
_throw( _temp75); _LL76:;}}}} struct Cyc_PP_Doc* Cyc_PP_textptr( struct
_tagged_arr* s){ return Cyc_PP_text(* s);} struct Cyc_Core_Opt* Cyc_PP_nil_doc_opt=
0; struct Cyc_Core_Opt* Cyc_PP_blank_doc_opt= 0; struct Cyc_Core_Opt* Cyc_PP_line_doc_opt=
0; struct Cyc_PP_Doc* Cyc_PP_nil_doc(){ if( Cyc_PP_nil_doc_opt == 0){ Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt* _temp82=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp82->v=( void*) Cyc_PP_text( _tag_arr("", sizeof(
unsigned char), 1u)); _temp82;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_PP_nil_doc_opt))->v;} struct Cyc_PP_Doc* Cyc_PP_blank_doc(){
if( Cyc_PP_blank_doc_opt == 0){ Cyc_PP_blank_doc_opt=({ struct Cyc_Core_Opt*
_temp83=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp83->v=( void*) Cyc_PP_text( _tag_arr("", sizeof( unsigned char), 1u));
_temp83;});} return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_PP_blank_doc_opt))->v;} struct _tuple2{ int f1; struct _tagged_arr f2;
struct _tagged_arr f3; } ; struct _tuple3{ int f1; int f2; int f3; struct
_tagged_arr f4; } ; static struct Cyc_PP_Out* Cyc_PP_hyperlink_f( struct _tuple2*
clo, struct Cyc_PP_Ppstate* st){ struct _tuple2 _temp86; struct _tagged_arr
_temp87; struct _tagged_arr _temp89; int _temp91; struct _tuple2* _temp84= clo;
_temp86=* _temp84; _LL92: _temp91= _temp86.f1; goto _LL90; _LL90: _temp89=
_temp86.f2; goto _LL88; _LL88: _temp87= _temp86.f3; goto _LL85; _LL85: return({
struct Cyc_PP_Out* _temp93=( struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out));
_temp93->newcc= st->cc + _temp91; _temp93->newcl= st->cl; _temp93->ppout=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp97=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp97[ 0]=({ struct Cyc_PP_Single_struct
_temp98; _temp98.tag= Cyc_PP_Single; _temp98.f1=( void*)({ struct _tagged_arr*
_temp99=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr) * 1);
_temp99[ 0]= _temp89; _temp99;}); _temp98;}); _temp97;})); _temp93->links=( void*)((
void*)({ struct Cyc_PP_Single_struct* _temp94=( struct Cyc_PP_Single_struct*)
GC_malloc( sizeof( struct Cyc_PP_Single_struct)); _temp94[ 0]=({ struct Cyc_PP_Single_struct
_temp95; _temp95.tag= Cyc_PP_Single; _temp95.f1=( void*)({ struct _tuple3*
_temp96=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp96->f1= st->cl;
_temp96->f2= st->cc; _temp96->f3= _temp91; _temp96->f4= _temp87; _temp96;});
_temp95;}); _temp94;})); _temp93;});} struct Cyc_PP_Doc* Cyc_PP_hyperlink(
struct _tagged_arr shrt, struct _tagged_arr full){ int slen= Cyc_String_strlen(
shrt); return({ struct Cyc_PP_Doc* _temp100=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp100->mwo= slen; _temp100->mw= Cyc_PP_infinity;
_temp100->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple2*,
struct Cyc_PP_Ppstate*), struct _tuple2* x)) Cyc_Fn_make_fn)( Cyc_PP_hyperlink_f,({
struct _tuple2* _temp101=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp101->f1= slen; _temp101->f2= shrt; _temp101->f3= full; _temp101;}));
_temp100;});} static struct Cyc_PP_Out* Cyc_PP_line_f( struct Cyc_PP_Ppstate* st){
return({ struct Cyc_PP_Out* _temp102=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp102->newcc= st->ci; _temp102->newcl= st->cl + 1;
_temp102->ppout=( void*)(( void*)({ struct Cyc_PP_Single_struct* _temp103=(
struct Cyc_PP_Single_struct*) GC_malloc( sizeof( struct Cyc_PP_Single_struct));
_temp103[ 0]=({ struct Cyc_PP_Single_struct _temp104; _temp104.tag= Cyc_PP_Single;
_temp104.f1=( void*)({ struct _tagged_arr* _temp105=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) * 1); _temp105[ 0]= Cyc_PP_nlblanks( st->ci);
_temp105;}); _temp104;}); _temp103;})); _temp102->links=( void*)(( void*) Cyc_PP_Empty);
_temp102;});} struct Cyc_PP_Doc* Cyc_PP_line_doc(){ if( Cyc_PP_line_doc_opt == 0){
Cyc_PP_line_doc_opt=({ struct Cyc_Core_Opt* _temp106=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp106->v=( void*)({ struct Cyc_PP_Doc*
_temp107=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp107->mwo=
0; _temp107->mw= 0; _temp107->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)(
struct Cyc_PP_Ppstate*))) Cyc_Fn_fp2fn)( Cyc_PP_line_f); _temp107;}); _temp106;});}
return( struct Cyc_PP_Doc*)(( struct Cyc_Core_Opt*) _check_null( Cyc_PP_line_doc_opt))->v;}
struct _tuple4{ int f1; struct Cyc_PP_Doc* f2; } ; static struct Cyc_PP_Out* Cyc_PP_nest_f(
struct _tuple4* clo, struct Cyc_PP_Ppstate* st){ struct _tuple4 _temp110; struct
Cyc_PP_Doc* _temp111; int _temp113; struct _tuple4* _temp108= clo; _temp110=*
_temp108; _LL114: _temp113= _temp110.f1; goto _LL112; _LL112: _temp111= _temp110.f2;
goto _LL109; _LL109: { struct Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate*
_temp115=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp115->ci= st->ci + _temp113; _temp115->cc= st->cc; _temp115->cl= st->cl;
_temp115->pw= st->pw; _temp115->epw= st->epw; _temp115;}); return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp111->f, st2);}}
struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d){ return({ struct
Cyc_PP_Doc* _temp116=( struct Cyc_PP_Doc*) GC_malloc( sizeof( struct Cyc_PP_Doc));
_temp116->mwo= d->mwo; _temp116->mw= d->mw; _temp116->f=( void*)(( void*(*)(
struct Cyc_PP_Out*(* f)( struct _tuple4*, struct Cyc_PP_Ppstate*), struct
_tuple4* x)) Cyc_Fn_make_fn)( Cyc_PP_nest_f,({ struct _tuple4* _temp117=( struct
_tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp117->f1= k; _temp117->f2= d;
_temp117;})); _temp116;});} int Cyc_PP_min( int x, int y){ if( x <= y){ return x;}
else{ return y;}} int Cyc_PP_max( int x, int y){ if( x >= y){ return x;} else{
return y;}} struct _tuple5{ struct Cyc_PP_Doc* f1; struct Cyc_PP_Doc* f2; } ;
static struct Cyc_PP_Out* Cyc_PP_concat_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp120; struct Cyc_PP_Doc* _temp121; struct Cyc_PP_Doc*
_temp123; struct _tuple5* _temp118= clo; _temp120=* _temp118; _LL124: _temp123=
_temp120.f1; goto _LL122; _LL122: _temp121= _temp120.f2; goto _LL119; _LL119: {
int epw2= Cyc_PP_max( st->pw - _temp121->mw, st->epw - _temp123->mwo); struct
Cyc_PP_Ppstate* st2=({ struct Cyc_PP_Ppstate* _temp127=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp127->ci= st->ci;
_temp127->cc= st->cc; _temp127->cl= st->cl; _temp127->pw= st->pw; _temp127->epw=
epw2; _temp127;}); struct Cyc_PP_Out* o1=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp123->f, st2); struct Cyc_PP_Ppstate*
st3=({ struct Cyc_PP_Ppstate* _temp126=( struct Cyc_PP_Ppstate*)
GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate)); _temp126->ci= st->ci;
_temp126->cc= o1->newcc; _temp126->cl= o1->newcl; _temp126->pw= st->pw; _temp126->epw=
epw2; _temp126;}); struct Cyc_PP_Out* o2=(( struct Cyc_PP_Out*(*)( void* f,
struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp121->f, st3); struct Cyc_PP_Out*
o3=({ struct Cyc_PP_Out* _temp125=( struct Cyc_PP_Out*) GC_malloc( sizeof(
struct Cyc_PP_Out)); _temp125->newcc= o2->newcc; _temp125->newcl= o2->newcl;
_temp125->ppout=( void*)(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)
o1->ppout,( void*) o2->ppout); _temp125->links=( void*)(( void*(*)( void* a1,
void* a2)) Cyc_PP_append)(( void*) o1->links,( void*) o2->links); _temp125;});
return o3;}} static struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2){ return({ struct Cyc_PP_Doc* _temp128=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp128->mwo= Cyc_PP_min( d1->mw, d1->mwo
+ d2->mwo); _temp128->mw= Cyc_PP_min( d1->mw, d1->mwo + d2->mw); _temp128->f=(
void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tuple5*, struct Cyc_PP_Ppstate*),
struct _tuple5* x)) Cyc_Fn_make_fn)( Cyc_PP_concat_f,({ struct _tuple5* _temp129=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp129->f1= d1; _temp129->f2=
d2; _temp129;})); _temp128;});} struct Cyc_PP_Doc* Cyc_PP_cat( struct
_tagged_arr l){ struct Cyc_PP_Doc* _temp130= Cyc_PP_nil_doc();{ int i=( int)(
_get_arr_size( l, sizeof( struct Cyc_PP_Doc*)) - 1); for( 0; i >= 0; i --){
_temp130= Cyc_PP_concat(*(( struct Cyc_PP_Doc**) _check_unknown_subscript( l,
sizeof( struct Cyc_PP_Doc*), i)), _temp130);}} return _temp130;} static struct
Cyc_PP_Out* Cyc_PP_long_cats_f( struct Cyc_List_List* ds0, struct Cyc_PP_Ppstate*
st){ struct Cyc_List_List* os= 0;{ struct Cyc_List_List* _temp131= ds0; for( 0;
_temp131 != 0; _temp131=(( struct Cyc_List_List*) _check_null( _temp131))->tl){
struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null(
_temp131))->hd; struct Cyc_PP_Out* o=(( struct Cyc_PP_Out*(*)( void* f, struct
Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) d->f, st); st=({ struct Cyc_PP_Ppstate*
_temp132=( struct Cyc_PP_Ppstate*) GC_malloc_atomic( sizeof( struct Cyc_PP_Ppstate));
_temp132->ci= st->ci; _temp132->cc= o->newcc; _temp132->cl= o->newcl; _temp132->pw=
st->pw; _temp132->epw= st->epw - d->mwo; _temp132;}); os=({ struct Cyc_List_List*
_temp133=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp133->hd=( void*) o; _temp133->tl= os; _temp133;});}}{ int newcc=(( struct
Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcc; int newcl=((
struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->newcl; void*
s=( void*) Cyc_PP_Empty; void* links=( void*) Cyc_PP_Empty; for( 0; os != 0; os=((
struct Cyc_List_List*) _check_null( os))->tl){ s=(( void*(*)( void* a1, void* a2))
Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->ppout, s); links=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)((
void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*) _check_null( os))->hd)->links,
links);} return({ struct Cyc_PP_Out* _temp134=( struct Cyc_PP_Out*) GC_malloc(
sizeof( struct Cyc_PP_Out)); _temp134->newcc= newcc; _temp134->newcl= newcl;
_temp134->ppout=( void*) s; _temp134->links=( void*) links; _temp134;});}}
static struct Cyc_PP_Doc* Cyc_PP_long_cats( struct Cyc_List_List* doclist){
struct Cyc_List_List* orig= doclist; struct Cyc_PP_Doc* d=( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( doclist))->hd; doclist=(( struct Cyc_List_List*)
_check_null( doclist))->tl;{ int mw= d->mw; int mwo= d->mw;{ struct Cyc_List_List*
_temp135= doclist; for( 0; _temp135 != 0; _temp135=(( struct Cyc_List_List*)
_check_null( _temp135))->tl){ int mw2= Cyc_PP_min( mw, mwo +(( struct Cyc_PP_Doc*)((
struct Cyc_List_List*) _check_null( _temp135))->hd)->mwo); int mwo2= Cyc_PP_min(
mw, mwo +(( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( _temp135))->hd)->mw);
mw= mw2; mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp136=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp136->mwo= mw; _temp136->mw= mwo;
_temp136->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_List_List*,
struct Cyc_PP_Ppstate*), struct Cyc_List_List* x)) Cyc_Fn_make_fn)( Cyc_PP_long_cats_f,
orig); _temp136;});}} struct Cyc_PP_Doc* Cyc_PP_cats( struct Cyc_List_List*
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
struct Cyc_PP_Ppstate* _temp137=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp137->ci= st->ci; _temp137->cc= o->newcc;
_temp137->cl= o->newcl; _temp137->pw= st->pw; _temp137->epw= st->epw - d->mwo;
_temp137;}); os=({ struct Cyc_List_List* _temp138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp138->hd=( void*) o; _temp138->tl=
os; _temp138;});}}{ int newcc=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcc; int newcl=(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->newcl; void* s=( void*) Cyc_PP_Empty; void* links=( void*)
Cyc_PP_Empty; for( 0; os != 0; os=(( struct Cyc_List_List*) _check_null( os))->tl){
s=(( void*(*)( void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)((
struct Cyc_List_List*) _check_null( os))->hd)->ppout, s); links=(( void*(*)(
void* a1, void* a2)) Cyc_PP_append)(( void*)(( struct Cyc_PP_Out*)(( struct Cyc_List_List*)
_check_null( os))->hd)->links, links);} return({ struct Cyc_PP_Out* _temp139=(
struct Cyc_PP_Out*) GC_malloc( sizeof( struct Cyc_PP_Out)); _temp139->newcc=
newcc; _temp139->newcl= newcl; _temp139->ppout=( void*) s; _temp139->links=(
void*) links; _temp139;});}} struct Cyc_PP_Doc* Cyc_PP_cats_arr( struct
_tagged_arr docs){ int sz=( int) _get_arr_size( docs, sizeof( struct Cyc_PP_Doc*));
if( sz == 0){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp140=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp140[ 0]=({ struct Cyc_Core_Failure_struct _temp141; _temp141.tag= Cyc_Core_Failure;
_temp141.f1= _tag_arr("cats_arr -- size zero array", sizeof( unsigned char), 28u);
_temp141;}); _temp140;}));}{ struct Cyc_PP_Doc* d=*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), 0)); int mw= d->mw;
int mwo= d->mw;{ int i= 1; for( 0; i < sz; ++ i){ int mw2= Cyc_PP_min( mw, mwo +(*((
struct Cyc_PP_Doc**) _check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*),
i)))->mwo); int mwo2= Cyc_PP_min( mw, mwo +(*(( struct Cyc_PP_Doc**)
_check_unknown_subscript( docs, sizeof( struct Cyc_PP_Doc*), i)))->mw); mw= mw2;
mwo= mwo2;}} return({ struct Cyc_PP_Doc* _temp142=( struct Cyc_PP_Doc*)
GC_malloc( sizeof( struct Cyc_PP_Doc)); _temp142->mwo= mw; _temp142->mw= mwo;
_temp142->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct _tagged_arr*,
struct Cyc_PP_Ppstate*), struct _tagged_arr* x)) Cyc_Fn_make_fn)( Cyc_PP_cats_arr_f,({
struct _tagged_arr* _temp143=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr) * 1); _temp143[ 0]= docs; _temp143;})); _temp142;});}} static
struct Cyc_PP_Out* Cyc_PP_doc_union_f( struct _tuple5* clo, struct Cyc_PP_Ppstate*
st){ struct _tuple5 _temp146; struct Cyc_PP_Doc* _temp147; struct Cyc_PP_Doc*
_temp149; struct _tuple5* _temp144= clo; _temp146=* _temp144; _LL150: _temp149=
_temp146.f1; goto _LL148; _LL148: _temp147= _temp146.f2; goto _LL145; _LL145: {
int dfits= st->cc + _temp149->mwo <= st->epw? 1: st->cc + _temp149->mw <= st->pw;
if( dfits){ return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x))
Cyc_Fn_apply)(( void*) _temp149->f, st);} else{ return(( struct Cyc_PP_Out*(*)(
void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)(( void*) _temp147->f, st);}}}
struct Cyc_PP_Doc* Cyc_PP_doc_union( struct Cyc_PP_Doc* d, struct Cyc_PP_Doc* d2){
return({ struct Cyc_PP_Doc* _temp151=( struct Cyc_PP_Doc*) GC_malloc( sizeof(
struct Cyc_PP_Doc)); _temp151->mwo= Cyc_PP_min( d->mwo, d2->mwo); _temp151->mw=
Cyc_PP_min( d->mw, d2->mw); _temp151->f=( void*)(( void*(*)( struct Cyc_PP_Out*(*
f)( struct _tuple5*, struct Cyc_PP_Ppstate*), struct _tuple5* x)) Cyc_Fn_make_fn)(
Cyc_PP_doc_union_f,({ struct _tuple5* _temp152=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp152->f1= d; _temp152->f2= d2; _temp152;}));
_temp151;});} struct Cyc_PP_Doc* Cyc_PP_oline_doc(){ return Cyc_PP_doc_union(
Cyc_PP_nil_doc(), Cyc_PP_line_doc());} static struct Cyc_PP_Out* Cyc_PP_tab_f(
struct Cyc_PP_Doc* d, struct Cyc_PP_Ppstate* st){ struct Cyc_PP_Ppstate* st2=({
struct Cyc_PP_Ppstate* _temp153=( struct Cyc_PP_Ppstate*) GC_malloc_atomic(
sizeof( struct Cyc_PP_Ppstate)); _temp153->ci= st->cc; _temp153->cc= st->cc;
_temp153->cl= st->cl; _temp153->pw= st->pw; _temp153->epw= st->epw; _temp153;});
return(( struct Cyc_PP_Out*(*)( void* f, struct Cyc_PP_Ppstate* x)) Cyc_Fn_apply)((
void*) d->f, st2);} struct Cyc_PP_Doc* Cyc_PP_tab( struct Cyc_PP_Doc* d){ struct
Cyc_PP_Doc* d2=({ struct Cyc_PP_Doc* _temp154=( struct Cyc_PP_Doc*) GC_malloc(
sizeof( struct Cyc_PP_Doc)); _temp154->mwo= d->mwo; _temp154->mw= d->mw;
_temp154->f=( void*)(( void*(*)( struct Cyc_PP_Out*(* f)( struct Cyc_PP_Doc*,
struct Cyc_PP_Ppstate*), struct Cyc_PP_Doc* x)) Cyc_Fn_make_fn)( Cyc_PP_tab_f, d);
_temp154;}); return d2;} static struct Cyc_PP_Doc* Cyc_PP_ppseq_f( struct Cyc_PP_Doc*(*
pp)( void*), struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){
return Cyc_PP_nil_doc();} else{ if((( struct Cyc_List_List*) _check_null( l))->tl
== 0){ return pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);} else{
return({ struct Cyc_PP_Doc*(* _temp155)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp158= pp(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
struct Cyc_PP_Doc* _temp159= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp160= Cyc_PP_oline_doc();
struct Cyc_PP_Doc* _temp161=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseq_f)( pp,
sep,(( struct Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp156[
4u]={ _temp158, _temp159, _temp160, _temp161}; struct _tagged_arr _temp157={(
void*) _temp156,( void*) _temp156,( void*)( _temp156 + 4u)}; _temp155( _temp157);});}}}
struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseq_f)( pp, sep, l0));} struct Cyc_PP_Doc* Cyc_PP_seq_f( struct
_tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;} else{ struct Cyc_PP_Doc*
sep2= Cyc_PP_text( sep); struct Cyc_PP_Doc* oline= Cyc_PP_oline_doc(); struct
Cyc_List_List* _temp162= l; while((( struct Cyc_List_List*) _check_null(
_temp162))->tl != 0) { struct Cyc_List_List* _temp163=(( struct Cyc_List_List*)
_check_null( _temp162))->tl;(( struct Cyc_List_List*) _check_null( _temp162))->tl=({
struct Cyc_List_List* _temp164=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp164->hd=( void*) sep2; _temp164->tl=({ struct Cyc_List_List*
_temp165=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp165->hd=( void*) oline; _temp165->tl= _temp163; _temp165;}); _temp164;});
_temp162= _temp163;} return Cyc_PP_cats( l);}}} struct Cyc_PP_Doc* Cyc_PP_seq(
struct _tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab( Cyc_PP_seq_f(
sep, l0));} struct Cyc_PP_Doc* Cyc_PP_ppseql_f( struct Cyc_PP_Doc*(* pp)( void*),
struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return pp(( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} else{ return({ struct Cyc_PP_Doc*(*
_temp166)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp169= pp((
void*)(( struct Cyc_List_List*) _check_null( l))->hd); struct Cyc_PP_Doc*
_temp170= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp171= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp172=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)(
void*), struct _tagged_arr sep, struct Cyc_List_List* l)) Cyc_PP_ppseql_f)( pp,
sep,(( struct Cyc_List_List*) _check_null( l))->tl); struct Cyc_PP_Doc* _temp167[
4u]={ _temp169, _temp170, _temp171, _temp172}; struct _tagged_arr _temp168={(
void*) _temp167,( void*) _temp167,( void*)( _temp167 + 4u)}; _temp166( _temp168);});}}}
struct Cyc_PP_Doc* Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab((( struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_arr sep, struct Cyc_List_List*
l)) Cyc_PP_ppseql_f)( pp, sep, l0));} static struct Cyc_PP_Doc* Cyc_PP_seql_f(
struct _tagged_arr sep, struct Cyc_List_List* l){ if( l == 0){ return Cyc_PP_nil_doc();}
else{ if((( struct Cyc_List_List*) _check_null( l))->tl == 0){ return( struct
Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;} else{ return({
struct Cyc_PP_Doc*(* _temp173)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc*
_temp176=( struct Cyc_PP_Doc*)(( struct Cyc_List_List*) _check_null( l))->hd;
struct Cyc_PP_Doc* _temp177= Cyc_PP_text( sep); struct Cyc_PP_Doc* _temp178= Cyc_PP_line_doc();
struct Cyc_PP_Doc* _temp179= Cyc_PP_seql_f( sep,(( struct Cyc_List_List*)
_check_null( l))->tl); struct Cyc_PP_Doc* _temp174[ 4u]={ _temp176, _temp177,
_temp178, _temp179}; struct _tagged_arr _temp175={( void*) _temp174,( void*)
_temp174,( void*)( _temp174 + 4u)}; _temp173( _temp175);});}}} struct Cyc_PP_Doc*
Cyc_PP_seql( struct _tagged_arr sep, struct Cyc_List_List* l0){ return Cyc_PP_tab(
Cyc_PP_seql_f( sep, l0));} struct Cyc_PP_Doc* Cyc_PP_group( struct _tagged_arr
start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss){
return({ struct Cyc_PP_Doc*(* _temp180)( struct _tagged_arr l)= Cyc_PP_cat;
struct Cyc_PP_Doc* _temp183= Cyc_PP_text( start); struct Cyc_PP_Doc* _temp184=
Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc* _temp185= Cyc_PP_text( stop); struct
Cyc_PP_Doc* _temp181[ 3u]={ _temp183, _temp184, _temp185}; struct _tagged_arr
_temp182={( void*) _temp181,( void*) _temp181,( void*)( _temp181 + 3u)};
_temp180( _temp182);});} struct Cyc_PP_Doc* Cyc_PP_egroup( struct _tagged_arr
start, struct _tagged_arr stop, struct _tagged_arr sep, struct Cyc_List_List* ss){
if( ss == 0){ return Cyc_PP_nil_doc();} else{ return({ struct Cyc_PP_Doc*(*
_temp186)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp189= Cyc_PP_text(
start); struct Cyc_PP_Doc* _temp190= Cyc_PP_seq( sep, ss); struct Cyc_PP_Doc*
_temp191= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp187[ 3u]={ _temp189,
_temp190, _temp191}; struct _tagged_arr _temp188={( void*) _temp187,( void*)
_temp187,( void*)( _temp187 + 3u)}; _temp186( _temp188);});}} struct Cyc_PP_Doc*
Cyc_PP_groupl( struct _tagged_arr start, struct _tagged_arr stop, struct
_tagged_arr sep, struct Cyc_List_List* ss){ return({ struct Cyc_PP_Doc*(*
_temp192)( struct _tagged_arr l)= Cyc_PP_cat; struct Cyc_PP_Doc* _temp195= Cyc_PP_text(
start); struct Cyc_PP_Doc* _temp196= Cyc_PP_seql( sep, ss); struct Cyc_PP_Doc*
_temp197= Cyc_PP_text( stop); struct Cyc_PP_Doc* _temp193[ 3u]={ _temp195,
_temp196, _temp197}; struct _tagged_arr _temp194={( void*) _temp193,( void*)
_temp193,( void*)( _temp193 + 3u)}; _temp192( _temp194);});}