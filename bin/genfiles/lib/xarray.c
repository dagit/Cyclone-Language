#include "cyc_include.h"

 struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one;}; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Xarray_Xarray{ struct _tagged_ptr0 elmts; int num_elmts;}; typedef
struct Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*); extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*,
int); extern void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*, int, void*); extern
struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int, void*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create_empty(); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int, void*); extern void Cyc_Xarray_add( struct Cyc_Xarray_Xarray*, void*);
extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*, void*); extern struct
_tagged_ptr0 Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_from_array( struct _tagged_ptr0 arr); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_append( struct Cyc_Xarray_Xarray*, struct Cyc_Xarray_Xarray*); extern
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray*); extern void
Cyc_Xarray_app_c( void*(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray*);
extern void Cyc_Xarray_iter_c( void(* f)( void*, void*), void*, struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map( void*(* f)( void*), struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*),
void*, struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray*
xarr); int Cyc_Xarray_length( struct Cyc_Xarray_Xarray* xarr){ return xarr->num_elmts;}
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray* xarr, int i){ if( i < 0? 1: i >=
xarr->num_elmts){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp0=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp0=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp1=( char*)"Xarray::get: bad index"; struct
_tagged_string _temp2; _temp2.curr= _temp1; _temp2.base= _temp1; _temp2.last_plus_one=
_temp1 + 23; _temp2;})};( struct _xenum_struct*) _temp0;}));} return({ struct
_tagged_ptr0 _temp3= xarr->elmts; void** _temp5= _temp3.curr + i; if( _temp5 <
_temp3.base? 1: _temp5 >= _temp3.last_plus_one){ _throw( Null_Exception);}*
_temp5;});} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray* xarr, int i, void* a){
if( i < 0? 1: i >= xarr->num_elmts){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp6=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp6=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp7=( char*)"Xarray::set: bad index"; struct
_tagged_string _temp8; _temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one=
_temp7 + 23; _temp8;})};( struct _xenum_struct*) _temp6;}));}({ struct
_tagged_ptr0 _temp9= xarr->elmts; void** _temp11= _temp9.curr + i; if( _temp11 <
_temp9.base? 1: _temp11 >= _temp9.last_plus_one){ _throw( Null_Exception);}*
_temp11= a;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(){ struct
_tagged_ptr0 x=( struct _tagged_ptr0)({ void** _temp13=( void**)({ void**
_temp12=( void**) GC_malloc( sizeof( void*) * 0); _temp12;}); struct
_tagged_ptr0 _temp14; _temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one=
_temp13 + 0; _temp14;}); return({ struct Cyc_Xarray_Xarray* _temp15=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp15->elmts= x; _temp15->num_elmts=
0; _temp15;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int len, void* a){
if( len < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp16=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp16=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp17=( char*)"xarrays must have a non-negative size buffer";
struct _tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17;
_temp18.last_plus_one= _temp17 + 45; _temp18;})};( struct _xenum_struct*)
_temp16;}));} return({ struct Cyc_Xarray_Xarray* _temp19=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp19->elmts=({ unsigned int
_temp20=( unsigned int) len; void** _temp21=( void**) GC_malloc( sizeof( void*)
* _temp20); unsigned int i; struct _tagged_ptr0 _temp22={ _temp21, _temp21,
_temp21 + _temp20}; for( i= 0; i < _temp20; i ++){ _temp21[ i]= a;} _temp22;});
_temp19->num_elmts= 0; _temp19;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int len, void* a){ if( len < 1){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp23=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp23=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp24=( char*)"singleton xarray must have size >=1";
struct _tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24;
_temp25.last_plus_one= _temp24 + 36; _temp25;})};( struct _xenum_struct*)
_temp23;}));}{ struct Cyc_Xarray_Xarray* x=(( struct Cyc_Xarray_Xarray*(*)( int
len, void* a)) Cyc_Xarray_create)( len, a); x->num_elmts= 1; return x;}} void
Cyc_Xarray_add( struct Cyc_Xarray_Xarray* xarr, void* a){ if(( unsigned int)
xarr->num_elmts ==({ struct _tagged_ptr0 _temp26= xarr->elmts;( unsigned int)(
_temp26.last_plus_one - _temp26.curr);})){ if( xarr->num_elmts == 0){ xarr->elmts=({
unsigned int _temp27=( unsigned int) 10; void** _temp28=( void**) GC_malloc(
sizeof( void*) * _temp27); unsigned int i; struct _tagged_ptr0 _temp29={ _temp28,
_temp28, _temp28 + _temp27}; for( i= 0; i < _temp27; i ++){ _temp28[ i]= a;}
_temp29;});} else{ struct _tagged_ptr0 newarr=({ unsigned int _temp33=(
unsigned int)( xarr->num_elmts * 2); void** _temp34=( void**) GC_malloc( sizeof(
void*) * _temp33); unsigned int i; struct _tagged_ptr0 _temp35={ _temp34,
_temp34, _temp34 + _temp33}; for( i= 0; i < _temp33; i ++){ _temp34[ i]=({
struct _tagged_ptr0 _temp30= xarr->elmts; void** _temp32= _temp30.curr + 0; if(
_temp32 < _temp30.base? 1: _temp32 >= _temp30.last_plus_one){ _throw(
Null_Exception);}* _temp32;});} _temp35;});{ int i= 1; for( 0; i < xarr->num_elmts;
++ i){({ struct _tagged_ptr0 _temp36= newarr; void** _temp38= _temp36.curr + i;
if( _temp38 < _temp36.base? 1: _temp38 >= _temp36.last_plus_one){ _throw(
Null_Exception);}* _temp38=({ struct _tagged_ptr0 _temp39= xarr->elmts; void**
_temp41= _temp39.curr + i; if( _temp41 < _temp39.base? 1: _temp41 >= _temp39.last_plus_one){
_throw( Null_Exception);}* _temp41;});});}} xarr->elmts= newarr;}}({ struct
_tagged_ptr0 _temp42= xarr->elmts; void** _temp44= _temp42.curr +( xarr->num_elmts
++); if( _temp44 < _temp42.base? 1: _temp44 >= _temp42.last_plus_one){ _throw(
Null_Exception);}* _temp44= a;});} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*
xarr, void* a){(( void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)(
xarr, a); return xarr->num_elmts - 1;} struct _tagged_ptr0 Cyc_Xarray_to_array(
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return( struct
_tagged_ptr0)({ void** _temp46=( void**)({ void** _temp45=( void**) GC_malloc(
sizeof( void*) * 0); _temp45;}); struct _tagged_ptr0 _temp47; _temp47.curr=
_temp46; _temp47.base= _temp46; _temp47.last_plus_one= _temp46 + 0; _temp47;});}{
struct _tagged_ptr0 ans=({ unsigned int _temp51=( unsigned int) xarr->num_elmts;
void** _temp52=( void**) GC_malloc( sizeof( void*) * _temp51); unsigned int i;
struct _tagged_ptr0 _temp53={ _temp52, _temp52, _temp52 + _temp51}; for( i= 0; i
< _temp51; i ++){ _temp52[ i]=({ struct _tagged_ptr0 _temp48= xarr->elmts; void**
_temp50= _temp48.curr +( int) i; if( _temp50 < _temp48.base? 1: _temp50 >=
_temp48.last_plus_one){ _throw( Null_Exception);}* _temp50;});} _temp53;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array( struct
_tagged_ptr0 arr){ if(({ struct _tagged_ptr0 _temp54= arr;( unsigned int)(
_temp54.last_plus_one - _temp54.curr);}) ==( unsigned int) 0){ return(( struct
Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp55=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp55->elmts=({ unsigned int _temp61=({
struct _tagged_ptr0 _temp57= arr;( unsigned int)( _temp57.last_plus_one -
_temp57.curr);}); void** _temp62=( void**) GC_malloc( sizeof( void*) * _temp61);
unsigned int i; struct _tagged_ptr0 _temp63={ _temp62, _temp62, _temp62 +
_temp61}; for( i= 0; i < _temp61; i ++){ _temp62[ i]=({ struct _tagged_ptr0
_temp58= arr; void** _temp60= _temp58.curr +( int) i; if( _temp60 < _temp58.base?
1: _temp60 >= _temp58.last_plus_one){ _throw( Null_Exception);}* _temp60;});}
_temp63;}); _temp55->num_elmts=( int)({ struct _tagged_ptr0 _temp56= arr;(
unsigned int)( _temp56.last_plus_one - _temp56.curr);}); _temp55;}); return ans;}}
struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray* xarr1,
struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)(({ struct _tagged_ptr0
_temp64= xarr1->elmts;( unsigned int)( _temp64.last_plus_one - _temp64.curr);})
+({ struct _tagged_ptr0 _temp65= xarr2->elmts;( unsigned int)( _temp65.last_plus_one
- _temp65.curr);})); if( newsz == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ void* init=({ struct _tagged_ptr0 _temp66= xarr1->elmts;(
unsigned int)( _temp66.last_plus_one - _temp66.curr);}) ==( unsigned int) 0?({
struct _tagged_ptr0 _temp67= xarr2->elmts; void** _temp69= _temp67.curr + 0; if(
_temp69 < _temp67.base? 1: _temp69 >= _temp67.last_plus_one){ _throw(
Null_Exception);}* _temp69;}):({ struct _tagged_ptr0 _temp70= xarr1->elmts; void**
_temp72= _temp70.curr + 0; if( _temp72 < _temp70.base? 1: _temp72 >= _temp70.last_plus_one){
_throw( Null_Exception);}* _temp72;}); struct Cyc_Xarray_Xarray* ans=({ struct
Cyc_Xarray_Xarray* _temp73=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof(
struct Cyc_Xarray_Xarray)); _temp73->elmts=({ unsigned int _temp74=(
unsigned int) newsz; void** _temp75=( void**) GC_malloc( sizeof( void*) *
_temp74); unsigned int i; struct _tagged_ptr0 _temp76={ _temp75, _temp75,
_temp75 + _temp74}; for( i= 0; i < _temp74; i ++){ _temp75[ i]= init;} _temp76;});
_temp73->num_elmts= 0; _temp73;});{ int i= 0; for( 0; i < xarr1->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp77= xarr1->elmts; void** _temp79= _temp77.curr + i; if(
_temp79 < _temp77.base? 1: _temp79 >= _temp77.last_plus_one){ _throw(
Null_Exception);}* _temp79;}));}}{ int i= 0; for( 0; i < xarr2->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp80= xarr2->elmts; void** _temp82= _temp80.curr + i; if(
_temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){ _throw(
Null_Exception);}* _temp82;}));}} return ans;}} void Cyc_Xarray_app( void*(* f)(
void*), struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f(({ struct _tagged_ptr0 _temp83= xarr->elmts; void** _temp85= _temp83.curr
+ i; if( _temp85 < _temp83.base? 1: _temp85 >= _temp83.last_plus_one){ _throw(
Null_Exception);}* _temp85;}));}} void Cyc_Xarray_app_c( void*(* f)( void*, void*),
void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f( env,({ struct _tagged_ptr0 _temp86= xarr->elmts; void** _temp88=
_temp86.curr + i; if( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one){
_throw( Null_Exception);}* _temp88;}));}} void Cyc_Xarray_iter( void(* f)( void*),
struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f(({
struct _tagged_ptr0 _temp89= xarr->elmts; void** _temp91= _temp89.curr + i; if(
_temp91 < _temp89.base? 1: _temp91 >= _temp89.last_plus_one){ _throw(
Null_Exception);}* _temp91;}));}} void Cyc_Xarray_iter_c( void(* f)( void*, void*),
void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f( env,({ struct _tagged_ptr0 _temp92= xarr->elmts; void** _temp94=
_temp92.curr + i; if( _temp94 < _temp92.base? 1: _temp94 >= _temp92.last_plus_one){
_throw( Null_Exception);}* _temp94;}));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map(
void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){
return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct
Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp95=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp95->elmts=({ unsigned int
_temp100=({ struct _tagged_ptr0 _temp96= xarr->elmts;( unsigned int)( _temp96.last_plus_one
- _temp96.curr);}); void** _temp101=( void**) GC_malloc( sizeof( void*) *
_temp100); unsigned int i; struct _tagged_ptr0 _temp102={ _temp101, _temp101,
_temp101 + _temp100}; for( i= 0; i < _temp100; i ++){ _temp101[ i]= f(({ struct
_tagged_ptr0 _temp97= xarr->elmts; void** _temp99= _temp97.curr + 0; if( _temp99
< _temp97.base? 1: _temp99 >= _temp97.last_plus_one){ _throw( Null_Exception);}*
_temp99;}));} _temp102;}); _temp95->num_elmts= xarr->num_elmts; _temp95;});{ int
i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct _tagged_ptr0 _temp103= ans->elmts;
void** _temp105= _temp103.curr + i; if( _temp105 < _temp103.base? 1: _temp105 >=
_temp103.last_plus_one){ _throw( Null_Exception);}* _temp105= f(({ struct
_tagged_ptr0 _temp106= xarr->elmts; void** _temp108= _temp106.curr + i; if(
_temp108 < _temp106.base? 1: _temp108 >= _temp106.last_plus_one){ _throw(
Null_Exception);}* _temp108;}));});}} return ans;}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray*
xarr){ if( xarr->num_elmts == 0){ return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp109=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp109->elmts=({ unsigned int
_temp114=({ struct _tagged_ptr0 _temp110= xarr->elmts;( unsigned int)( _temp110.last_plus_one
- _temp110.curr);}); void** _temp115=( void**) GC_malloc( sizeof( void*) *
_temp114); unsigned int i; struct _tagged_ptr0 _temp116={ _temp115, _temp115,
_temp115 + _temp114}; for( i= 0; i < _temp114; i ++){ _temp115[ i]= f( env,({
struct _tagged_ptr0 _temp111= xarr->elmts; void** _temp113= _temp111.curr + 0;
if( _temp113 < _temp111.base? 1: _temp113 >= _temp111.last_plus_one){ _throw(
Null_Exception);}* _temp113;}));} _temp116;}); _temp109->num_elmts= xarr->num_elmts;
_temp109;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct
_tagged_ptr0 _temp117= ans->elmts; void** _temp119= _temp117.curr + i; if(
_temp119 < _temp117.base? 1: _temp119 >= _temp117.last_plus_one){ _throw(
Null_Exception);}* _temp119= f( env,({ struct _tagged_ptr0 _temp120= xarr->elmts;
void** _temp122= _temp120.curr + i; if( _temp122 < _temp120.base? 1: _temp122 >=
_temp120.last_plus_one){ _throw( Null_Exception);}* _temp122;}));});}} return
ans;}} void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts=
0;}