#include "cyc_include.h"

 struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ;
typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Xarray_Xarray{
struct _tagged_ptr0 elmts; int num_elmts; } ; typedef struct Cyc_Xarray_Xarray*
Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
extern void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*, int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create(
int, void*); extern struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(); extern
struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton( int, void*); extern void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray*, void*); extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*,
void*); extern struct _tagged_ptr0 Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array( struct _tagged_ptr0 arr);
extern struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray*,
struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_app( void*(* f)( void*),
struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_app_c( void*(* f)( void*,
void*), void*, struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_iter( void(* f)(
void*), struct Cyc_Xarray_Xarray*); extern void Cyc_Xarray_iter_c( void(* f)(
void*, void*), void*, struct Cyc_Xarray_Xarray*); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_map( void*(* f)( void*), struct Cyc_Xarray_Xarray*); extern struct
Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*), void*, struct
Cyc_Xarray_Xarray*); extern void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray*
xarr); int Cyc_Xarray_length( struct Cyc_Xarray_Xarray* xarr){ return xarr->num_elmts;}
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray* xarr, int i){ if( i < 0? 1: i >=
xarr->num_elmts){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp0=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0->tag= Cyc_Core_InvalidArg_tag; _temp0->f1=( struct _tagged_string)({ char*
_temp1=( char*)"Xarray::get: bad index"; struct _tagged_string _temp2; _temp2.curr=
_temp1; _temp2.base= _temp1; _temp2.last_plus_one= _temp1 + 23; _temp2;});(
struct _xenum_struct*) _temp0;}));} return({ struct _tagged_ptr0 _temp3= xarr->elmts;
void** _temp5= _temp3.curr + i; if( _temp5 < _temp3.base? 1: _temp5 >= _temp3.last_plus_one){
_throw( Null_Exception);}* _temp5;});} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*
xarr, int i, void* a){ if( i < 0? 1: i >= xarr->num_elmts){( void) _throw(({
struct Cyc_Core_InvalidArg_struct* _temp6=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp6->tag= Cyc_Core_InvalidArg_tag;
_temp6->f1=( struct _tagged_string)({ char* _temp7=( char*)"Xarray::set: bad index";
struct _tagged_string _temp8; _temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one=
_temp7 + 23; _temp8;});( struct _xenum_struct*) _temp6;}));}({ struct
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
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp16->tag= Cyc_Core_InvalidArg_tag; _temp16->f1=( struct _tagged_string)({
char* _temp17=( char*)"xarrays must have a non-negative size buffer"; struct
_tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17; _temp18.last_plus_one=
_temp17 + 45; _temp18;});( struct _xenum_struct*) _temp16;}));} return({ struct
Cyc_Xarray_Xarray* _temp19=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof(
struct Cyc_Xarray_Xarray)); _temp19->elmts=({ unsigned int _temp20=(
unsigned int) len; void** _temp21=( void**) GC_malloc( sizeof( void*) * _temp20);
unsigned int i; struct _tagged_ptr0 _temp22={ _temp21, _temp21, _temp21 +
_temp20}; for( i= 0; i < _temp20; i ++){ _temp21[ i]= a;} _temp22;}); _temp19->num_elmts=
0; _temp19;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton( int len, void* a){
if( len < 1){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp23=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp23->tag= Cyc_Core_InvalidArg_tag; _temp23->f1=( struct _tagged_string)({
char* _temp24=( char*)"singleton xarray must have size >=1"; struct
_tagged_string _temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 36; _temp25;});( struct _xenum_struct*) _temp23;}));}{ struct Cyc_Xarray_Xarray*
x=(( struct Cyc_Xarray_Xarray*(*)( int len, void* a)) Cyc_Xarray_create)( len, a);
x->num_elmts= 1; return x;}} void Cyc_Xarray_add( struct Cyc_Xarray_Xarray* xarr,
void* a){ if(( unsigned int) xarr->num_elmts ==({ struct _tagged_ptr0 _temp26=
xarr->elmts;( unsigned int)( _temp26.last_plus_one - _temp26.curr);})){ if( xarr->num_elmts
== 0){ xarr->elmts=( struct _tagged_ptr0)({ void** _temp29=( void**)({
unsigned int _temp27= 10u; void** _temp28=( void**) GC_malloc( sizeof( void*) *
_temp27); unsigned int i; for( i= 0; i < _temp27; i ++){ _temp28[ i]= a;}
_temp28;}); struct _tagged_ptr0 _temp30; _temp30.curr= _temp29; _temp30.base=
_temp29; _temp30.last_plus_one= _temp29 + 10; _temp30;});} else{ struct
_tagged_ptr0 newarr=({ unsigned int _temp34=( unsigned int)( xarr->num_elmts * 2);
void** _temp35=( void**) GC_malloc( sizeof( void*) * _temp34); unsigned int i;
struct _tagged_ptr0 _temp36={ _temp35, _temp35, _temp35 + _temp34}; for( i= 0; i
< _temp34; i ++){ _temp35[ i]=({ struct _tagged_ptr0 _temp31= xarr->elmts; void**
_temp33= _temp31.curr + 0; if( _temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one){
_throw( Null_Exception);}* _temp33;});} _temp36;});{ int i= 1; for( 0; i < xarr->num_elmts;
++ i){({ struct _tagged_ptr0 _temp37= newarr; void** _temp39= _temp37.curr + i;
if( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one){ _throw(
Null_Exception);}* _temp39=({ struct _tagged_ptr0 _temp40= xarr->elmts; void**
_temp42= _temp40.curr + i; if( _temp42 < _temp40.base? 1: _temp42 >= _temp40.last_plus_one){
_throw( Null_Exception);}* _temp42;});});}} xarr->elmts= newarr;}}({ struct
_tagged_ptr0 _temp43= xarr->elmts; void** _temp45= _temp43.curr +( xarr->num_elmts
++); if( _temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one){ _throw(
Null_Exception);}* _temp45= a;});} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*
xarr, void* a){(( void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)(
xarr, a); return xarr->num_elmts - 1;} struct _tagged_ptr0 Cyc_Xarray_to_array(
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return( struct
_tagged_ptr0)({ void** _temp47=( void**)({ void** _temp46=( void**) GC_malloc(
sizeof( void*) * 0); _temp46;}); struct _tagged_ptr0 _temp48; _temp48.curr=
_temp47; _temp48.base= _temp47; _temp48.last_plus_one= _temp47 + 0; _temp48;});}{
struct _tagged_ptr0 ans=({ unsigned int _temp52=( unsigned int) xarr->num_elmts;
void** _temp53=( void**) GC_malloc( sizeof( void*) * _temp52); unsigned int i;
struct _tagged_ptr0 _temp54={ _temp53, _temp53, _temp53 + _temp52}; for( i= 0; i
< _temp52; i ++){ _temp53[ i]=({ struct _tagged_ptr0 _temp49= xarr->elmts; void**
_temp51= _temp49.curr +( int) i; if( _temp51 < _temp49.base? 1: _temp51 >=
_temp49.last_plus_one){ _throw( Null_Exception);}* _temp51;});} _temp54;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array( struct
_tagged_ptr0 arr){ if(({ struct _tagged_ptr0 _temp55= arr;( unsigned int)(
_temp55.last_plus_one - _temp55.curr);}) ==( unsigned int) 0){ return(( struct
Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp56=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp56->elmts=({ unsigned int _temp62=({
struct _tagged_ptr0 _temp58= arr;( unsigned int)( _temp58.last_plus_one -
_temp58.curr);}); void** _temp63=( void**) GC_malloc( sizeof( void*) * _temp62);
unsigned int i; struct _tagged_ptr0 _temp64={ _temp63, _temp63, _temp63 +
_temp62}; for( i= 0; i < _temp62; i ++){ _temp63[ i]=({ struct _tagged_ptr0
_temp59= arr; void** _temp61= _temp59.curr +( int) i; if( _temp61 < _temp59.base?
1: _temp61 >= _temp59.last_plus_one){ _throw( Null_Exception);}* _temp61;});}
_temp64;}); _temp56->num_elmts=( int)({ struct _tagged_ptr0 _temp57= arr;(
unsigned int)( _temp57.last_plus_one - _temp57.curr);}); _temp56;}); return ans;}}
struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray* xarr1,
struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)(({ struct _tagged_ptr0
_temp65= xarr1->elmts;( unsigned int)( _temp65.last_plus_one - _temp65.curr);})
+({ struct _tagged_ptr0 _temp66= xarr2->elmts;( unsigned int)( _temp66.last_plus_one
- _temp66.curr);})); if( newsz == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ void* init=({ struct _tagged_ptr0 _temp67= xarr1->elmts;(
unsigned int)( _temp67.last_plus_one - _temp67.curr);}) ==( unsigned int) 0?({
struct _tagged_ptr0 _temp68= xarr2->elmts; void** _temp70= _temp68.curr + 0; if(
_temp70 < _temp68.base? 1: _temp70 >= _temp68.last_plus_one){ _throw(
Null_Exception);}* _temp70;}):({ struct _tagged_ptr0 _temp71= xarr1->elmts; void**
_temp73= _temp71.curr + 0; if( _temp73 < _temp71.base? 1: _temp73 >= _temp71.last_plus_one){
_throw( Null_Exception);}* _temp73;}); struct Cyc_Xarray_Xarray* ans=({ struct
Cyc_Xarray_Xarray* _temp74=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof(
struct Cyc_Xarray_Xarray)); _temp74->elmts=({ unsigned int _temp75=(
unsigned int) newsz; void** _temp76=( void**) GC_malloc( sizeof( void*) *
_temp75); unsigned int i; struct _tagged_ptr0 _temp77={ _temp76, _temp76,
_temp76 + _temp75}; for( i= 0; i < _temp75; i ++){ _temp76[ i]= init;} _temp77;});
_temp74->num_elmts= 0; _temp74;});{ int i= 0; for( 0; i < xarr1->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp78= xarr1->elmts; void** _temp80= _temp78.curr + i; if(
_temp80 < _temp78.base? 1: _temp80 >= _temp78.last_plus_one){ _throw(
Null_Exception);}* _temp80;}));}}{ int i= 0; for( 0; i < xarr2->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp81= xarr2->elmts; void** _temp83= _temp81.curr + i; if(
_temp83 < _temp81.base? 1: _temp83 >= _temp81.last_plus_one){ _throw(
Null_Exception);}* _temp83;}));}} return ans;}} void Cyc_Xarray_app( void*(* f)(
void*), struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f(({ struct _tagged_ptr0 _temp84= xarr->elmts; void** _temp86= _temp84.curr
+ i; if( _temp86 < _temp84.base? 1: _temp86 >= _temp84.last_plus_one){ _throw(
Null_Exception);}* _temp86;}));}} void Cyc_Xarray_app_c( void*(* f)( void*, void*),
void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f( env,({ struct _tagged_ptr0 _temp87= xarr->elmts; void** _temp89=
_temp87.curr + i; if( _temp89 < _temp87.base? 1: _temp89 >= _temp87.last_plus_one){
_throw( Null_Exception);}* _temp89;}));}} void Cyc_Xarray_iter( void(* f)( void*),
struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f(({
struct _tagged_ptr0 _temp90= xarr->elmts; void** _temp92= _temp90.curr + i; if(
_temp92 < _temp90.base? 1: _temp92 >= _temp90.last_plus_one){ _throw(
Null_Exception);}* _temp92;}));}} void Cyc_Xarray_iter_c( void(* f)( void*, void*),
void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f( env,({ struct _tagged_ptr0 _temp93= xarr->elmts; void** _temp95=
_temp93.curr + i; if( _temp95 < _temp93.base? 1: _temp95 >= _temp93.last_plus_one){
_throw( Null_Exception);}* _temp95;}));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map(
void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){
return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct
Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp96=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp96->elmts=({ unsigned int
_temp101=({ struct _tagged_ptr0 _temp97= xarr->elmts;( unsigned int)( _temp97.last_plus_one
- _temp97.curr);}); void** _temp102=( void**) GC_malloc( sizeof( void*) *
_temp101); unsigned int i; struct _tagged_ptr0 _temp103={ _temp102, _temp102,
_temp102 + _temp101}; for( i= 0; i < _temp101; i ++){ _temp102[ i]= f(({ struct
_tagged_ptr0 _temp98= xarr->elmts; void** _temp100= _temp98.curr + 0; if(
_temp100 < _temp98.base? 1: _temp100 >= _temp98.last_plus_one){ _throw(
Null_Exception);}* _temp100;}));} _temp103;}); _temp96->num_elmts= xarr->num_elmts;
_temp96;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct _tagged_ptr0
_temp104= ans->elmts; void** _temp106= _temp104.curr + i; if( _temp106 <
_temp104.base? 1: _temp106 >= _temp104.last_plus_one){ _throw( Null_Exception);}*
_temp106= f(({ struct _tagged_ptr0 _temp107= xarr->elmts; void** _temp109=
_temp107.curr + i; if( _temp109 < _temp107.base? 1: _temp109 >= _temp107.last_plus_one){
_throw( Null_Exception);}* _temp109;}));});}} return ans;}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_map_c( void*(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray*
xarr){ if( xarr->num_elmts == 0){ return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp110=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp110->elmts=({ unsigned int
_temp115=({ struct _tagged_ptr0 _temp111= xarr->elmts;( unsigned int)( _temp111.last_plus_one
- _temp111.curr);}); void** _temp116=( void**) GC_malloc( sizeof( void*) *
_temp115); unsigned int i; struct _tagged_ptr0 _temp117={ _temp116, _temp116,
_temp116 + _temp115}; for( i= 0; i < _temp115; i ++){ _temp116[ i]= f( env,({
struct _tagged_ptr0 _temp112= xarr->elmts; void** _temp114= _temp112.curr + 0;
if( _temp114 < _temp112.base? 1: _temp114 >= _temp112.last_plus_one){ _throw(
Null_Exception);}* _temp114;}));} _temp117;}); _temp110->num_elmts= xarr->num_elmts;
_temp110;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct
_tagged_ptr0 _temp118= ans->elmts; void** _temp120= _temp118.curr + i; if(
_temp120 < _temp118.base? 1: _temp120 >= _temp118.last_plus_one){ _throw(
Null_Exception);}* _temp120= f( env,({ struct _tagged_ptr0 _temp121= xarr->elmts;
void** _temp123= _temp121.curr + i; if( _temp123 < _temp121.base? 1: _temp123 >=
_temp121.last_plus_one){ _throw( Null_Exception);}* _temp123;}));});}} return
ans;}} void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts=
0;}