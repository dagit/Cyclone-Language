 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Xarray_Xarray{
struct _tagged_arr elmts; int num_elmts; } ; extern int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*); extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*, int, void*); extern
struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty(); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*, void*); extern struct
_tagged_arr Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_from_array( struct _tagged_arr arr); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_append( struct Cyc_Xarray_Xarray*, struct Cyc_Xarray_Xarray*); extern
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray*); extern void
Cyc_Xarray_app_c( void*(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter_c( void(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map( void*(* f)( void*), struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*),
void*, struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray*
xarr); int Cyc_Xarray_length( struct Cyc_Xarray_Xarray* xarr){ return xarr->num_elmts;}
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray* xarr, int i){ if( i <  0? 1: i
>=  xarr->num_elmts){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp0=( struct Cyc_Core_Invalid_argument_struct*) GC_malloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp0[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp1; _temp1.tag= Cyc_Core_Invalid_argument;
_temp1.f1= _tag_arr("Xarray::get: bad index", sizeof( unsigned char), 23u);
_temp1;}); _temp0;}));} return*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), i));} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray* xarr, int i,
void* a){ if( i <  0? 1: i >=  xarr->num_elmts){( int) _throw(( void*)({ struct
Cyc_Core_Invalid_argument_struct* _temp2=( struct Cyc_Core_Invalid_argument_struct*)
GC_malloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp2[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp3; _temp3.tag= Cyc_Core_Invalid_argument;
_temp3.f1= _tag_arr("Xarray::set: bad index", sizeof( unsigned char), 23u);
_temp3;}); _temp2;}));}*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), i))= a;} struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(){
struct _tagged_arr x= _tag_arr(({ void** _temp5=( void**) GC_malloc( sizeof(
void*) *  0); _temp5;}), sizeof( void*), 0u); return({ struct Cyc_Xarray_Xarray*
_temp4=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp4->elmts= x; _temp4->num_elmts= 0; _temp4;});} struct Cyc_Xarray_Xarray*
Cyc_Xarray_create( int len, void* a){ if( len <  0){( int) _throw(( void*)({
struct Cyc_Core_Invalid_argument_struct* _temp6=( struct Cyc_Core_Invalid_argument_struct*)
GC_malloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp6[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp7; _temp7.tag= Cyc_Core_Invalid_argument;
_temp7.f1= _tag_arr("xarrays must have a non-negative size buffer", sizeof(
unsigned char), 45u); _temp7;}); _temp6;}));} return({ struct Cyc_Xarray_Xarray*
_temp8=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp8->elmts=({ unsigned int _temp9=( unsigned int) len; void** _temp10=( void**)
GC_malloc( sizeof( void*) *  _temp9); struct _tagged_arr _temp12= _tag_arr(
_temp10, sizeof( void*),( unsigned int) len);{ unsigned int _temp11= _temp9;
unsigned int i; for( i= 0; i <  _temp11; i ++){ _temp10[ i]= a;}}; _temp12;});
_temp8->num_elmts= 0; _temp8;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int len, void* a){ if( len <  1){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp13=( struct Cyc_Core_Invalid_argument_struct*) GC_malloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp13[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp14; _temp14.tag= Cyc_Core_Invalid_argument; _temp14.f1= _tag_arr("singleton xarray must have size >=1",
sizeof( unsigned char), 36u); _temp14;}); _temp13;}));}{ struct Cyc_Xarray_Xarray*
x= Cyc_Xarray_create( len, a); x->num_elmts= 1; return x;}} void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray* xarr, void* a){ if( xarr->num_elmts ==  _get_arr_size(
xarr->elmts, sizeof( void*))){ if( xarr->num_elmts ==  0){ xarr->elmts= _tag_arr(({
unsigned int _temp15=( unsigned int) 10; void** _temp16=( void**) GC_malloc(
sizeof( void*) *  _temp15);{ unsigned int _temp17= _temp15; unsigned int i; for(
i= 0; i <  _temp17; i ++){ _temp16[ i]= a;}}; _temp16;}), sizeof( void*),(
unsigned int) 10);} else{ struct _tagged_arr newarr=({ unsigned int _temp18=(
unsigned int)( xarr->num_elmts *  2); void** _temp19=( void**) GC_malloc(
sizeof( void*) *  _temp18); struct _tagged_arr _temp21= _tag_arr( _temp19,
sizeof( void*),( unsigned int)( xarr->num_elmts *  2));{ unsigned int _temp20=
_temp18; unsigned int i; for( i= 0; i <  _temp20; i ++){ _temp19[ i]=*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), 0));}}; _temp21;});{ int
i= 1; for( 0; i <  xarr->num_elmts; ++ i){*(( void**) _check_unknown_subscript(
newarr, sizeof( void*), i))=*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), i));}} xarr->elmts= newarr;}}*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), xarr->num_elmts ++))= a;}
int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray* xarr, void* a){ Cyc_Xarray_add(
xarr, a); return xarr->num_elmts -  1;} struct _tagged_arr Cyc_Xarray_to_array(
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){ return _tag_arr(({
void** _temp22=( void**) GC_malloc( sizeof( void*) *  0); _temp22;}), sizeof(
void*), 0u);}{ struct _tagged_arr ans=({ unsigned int _temp23=( unsigned int)
xarr->num_elmts; void** _temp24=( void**) GC_malloc( sizeof( void*) *  _temp23);
struct _tagged_arr _temp26= _tag_arr( _temp24, sizeof( void*),( unsigned int)
xarr->num_elmts);{ unsigned int _temp25= _temp23; unsigned int i; for( i= 0; i < 
_temp25; i ++){ _temp24[ i]=*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*),( int) i));}}; _temp26;}); return ans;}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_from_array( struct _tagged_arr arr){ if( _get_arr_size( arr, sizeof(
void*)) ==  0){ return Cyc_Xarray_create_empty();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp27=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp27->elmts=({ unsigned int _temp28=
_get_arr_size( arr, sizeof( void*)); void** _temp29=( void**) GC_malloc( sizeof(
void*) *  _temp28); struct _tagged_arr _temp31= _tag_arr( _temp29, sizeof( void*),
_get_arr_size( arr, sizeof( void*)));{ unsigned int _temp30= _temp28;
unsigned int i; for( i= 0; i <  _temp30; i ++){ _temp29[ i]=*(( void**)
_check_unknown_subscript( arr, sizeof( void*),( int) i));}}; _temp31;}); _temp27->num_elmts=(
int) _get_arr_size( arr, sizeof( void*)); _temp27;}); return ans;}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_append( struct Cyc_Xarray_Xarray* xarr1, struct Cyc_Xarray_Xarray*
xarr2){ int newsz=( int)( _get_arr_size( xarr1->elmts, sizeof( void*)) + 
_get_arr_size( xarr2->elmts, sizeof( void*))); if( newsz ==  0){ return Cyc_Xarray_create_empty();}{
void* init= _get_arr_size( xarr1->elmts, sizeof( void*)) ==  0?*(( void**)
_check_unknown_subscript( xarr2->elmts, sizeof( void*), 0)):*(( void**)
_check_unknown_subscript( xarr1->elmts, sizeof( void*), 0)); struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp32=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp32->elmts=({ unsigned int _temp33=(
unsigned int) newsz; void** _temp34=( void**) GC_malloc( sizeof( void*) * 
_temp33); struct _tagged_arr _temp36= _tag_arr( _temp34, sizeof( void*),(
unsigned int) newsz);{ unsigned int _temp35= _temp33; unsigned int i; for( i= 0;
i <  _temp35; i ++){ _temp34[ i]= init;}}; _temp36;}); _temp32->num_elmts= 0;
_temp32;});{ int i= 0; for( 0; i <  xarr1->num_elmts; ++ i){ Cyc_Xarray_add( ans,*((
void**) _check_unknown_subscript( xarr1->elmts, sizeof( void*), i)));}}{ int i=
0; for( 0; i <  xarr2->num_elmts; ++ i){ Cyc_Xarray_add( ans,*(( void**)
_check_unknown_subscript( xarr2->elmts, sizeof( void*), i)));}} return ans;}}
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i=
0; for( 0; i <  xarr->num_elmts; ++ i){ f(*(( void**) _check_unknown_subscript(
xarr->elmts, sizeof( void*), i)));}} void Cyc_Xarray_app_c( void*(* f)( void*,
void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i <  xarr->num_elmts;
++ i){ f( env,*(( void**) _check_unknown_subscript( xarr->elmts, sizeof( void*),
i)));}} void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray* xarr){
int i= 0; for( 0; i <  xarr->num_elmts; ++ i){ f(*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}} void Cyc_Xarray_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0;
for( 0; i <  xarr->num_elmts; ++ i){ f( env,*(( void**) _check_unknown_subscript(
xarr->elmts, sizeof( void*), i)));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map(
void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){
return Cyc_Xarray_create_empty();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp37=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp37->elmts=({ unsigned int _temp38= _get_arr_size( xarr->elmts, sizeof( void*));
void** _temp39=( void**) GC_malloc( sizeof( void*) *  _temp38); struct
_tagged_arr _temp41= _tag_arr( _temp39, sizeof( void*), _get_arr_size( xarr->elmts,
sizeof( void*)));{ unsigned int _temp40= _temp38; unsigned int i; for( i= 0; i < 
_temp40; i ++){ _temp39[ i]= f(*(( void**) _check_unknown_subscript( xarr->elmts,
sizeof( void*), 0)));}}; _temp41;}); _temp37->num_elmts= xarr->num_elmts;
_temp37;});{ int i= 1; for( 0; i <  xarr->num_elmts; ++ i){*(( void**)
_check_unknown_subscript( ans->elmts, sizeof( void*), i))= f(*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}} return ans;}}
struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts ==  0){ return Cyc_Xarray_create_empty();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp42=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp42->elmts=({ unsigned int
_temp43= _get_arr_size( xarr->elmts, sizeof( void*)); void** _temp44=( void**)
GC_malloc( sizeof( void*) *  _temp43); struct _tagged_arr _temp46= _tag_arr(
_temp44, sizeof( void*), _get_arr_size( xarr->elmts, sizeof( void*)));{
unsigned int _temp45= _temp43; unsigned int i; for( i= 0; i <  _temp45; i ++){
_temp44[ i]= f( env,*(( void**) _check_unknown_subscript( xarr->elmts, sizeof(
void*), 0)));}}; _temp46;}); _temp42->num_elmts= xarr->num_elmts; _temp42;});{
int i= 1; for( 0; i <  xarr->num_elmts; ++ i){*(( void**)
_check_unknown_subscript( ans->elmts, sizeof( void*), i))= f( env,*(( void**)
_check_unknown_subscript( xarr->elmts, sizeof( void*), i)));}} return ans;}}
void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts= 0;}
