#include "cyc_include.h"

 struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ;
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Xarray_Xarray{ struct _tagged_ptr0 elmts; int num_elmts; } ; typedef
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
xarr->num_elmts){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp0=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp1; _temp1.tag= Cyc_Core_InvalidArg_tag;
_temp1.f1=( struct _tagged_string)({ char* _temp2=( char*)"Xarray::get: bad index";
struct _tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 23; _temp3;}); _temp1;}); _temp0;}));} return({ struct _tagged_ptr0
_temp4= xarr->elmts; void** _temp6= _temp4.curr + i; if( _temp4.base == 0? 1:(
_temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one)){ _throw(
Null_Exception);}* _temp6;});} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray*
xarr, int i, void* a){ if( i < 0? 1: i >= xarr->num_elmts){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp7=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp7[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp8; _temp8.tag= Cyc_Core_InvalidArg_tag;
_temp8.f1=( struct _tagged_string)({ char* _temp9=( char*)"Xarray::set: bad index";
struct _tagged_string _temp10; _temp10.curr= _temp9; _temp10.base= _temp9;
_temp10.last_plus_one= _temp9 + 23; _temp10;}); _temp8;}); _temp7;}));}({ struct
_tagged_ptr0 _temp11= xarr->elmts; void** _temp13= _temp11.curr + i; if( _temp11.base
== 0? 1:( _temp13 < _temp11.base? 1: _temp13 >= _temp11.last_plus_one)){ _throw(
Null_Exception);}* _temp13= a;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(){
struct _tagged_ptr0 x=( struct _tagged_ptr0)({ void** _temp15=( void**)({ void**
_temp14=( void**) GC_malloc( sizeof( void*) * 0); _temp14;}); struct
_tagged_ptr0 _temp16; _temp16.curr= _temp15; _temp16.base= _temp15; _temp16.last_plus_one=
_temp15 + 0; _temp16;}); return({ struct Cyc_Xarray_Xarray* _temp17=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp17->elmts= x; _temp17->num_elmts=
0; _temp17;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int len, void* a){
if( len < 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp18=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp18[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp19; _temp19.tag= Cyc_Core_InvalidArg_tag;
_temp19.f1=( struct _tagged_string)({ char* _temp20=( char*)"xarrays must have a non-negative size buffer";
struct _tagged_string _temp21; _temp21.curr= _temp20; _temp21.base= _temp20;
_temp21.last_plus_one= _temp20 + 45; _temp21;}); _temp19;}); _temp18;}));}
return({ struct Cyc_Xarray_Xarray* _temp22=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp22->elmts=({ unsigned int
_temp23=( unsigned int) len; void** _temp24=( void**) GC_malloc( sizeof( void*)
* _temp23); unsigned int i; struct _tagged_ptr0 _temp25={ _temp24, _temp24,
_temp24 + _temp23}; for( i= 0; i < _temp23; i ++){ _temp24[ i]= a;} _temp25;});
_temp22->num_elmts= 0; _temp22;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_singleton(
int len, void* a){ if( len < 1){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp26=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp26[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp27; _temp27.tag= Cyc_Core_InvalidArg_tag;
_temp27.f1=( struct _tagged_string)({ char* _temp28=( char*)"singleton xarray must have size >=1";
struct _tagged_string _temp29; _temp29.curr= _temp28; _temp29.base= _temp28;
_temp29.last_plus_one= _temp28 + 36; _temp29;}); _temp27;}); _temp26;}));}{
struct Cyc_Xarray_Xarray* x=(( struct Cyc_Xarray_Xarray*(*)( int len, void* a))
Cyc_Xarray_create)( len, a); x->num_elmts= 1; return x;}} void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray* xarr, void* a){ if(( unsigned int) xarr->num_elmts ==({
struct _tagged_ptr0 _temp30= xarr->elmts;( unsigned int)( _temp30.last_plus_one
- _temp30.curr);})){ if( xarr->num_elmts == 0){ xarr->elmts=( struct
_tagged_ptr0)({ void** _temp33=( void**)({ unsigned int _temp31= 10u; void**
_temp32=( void**) GC_malloc( sizeof( void*) * _temp31); unsigned int i; for( i=
0; i < _temp31; i ++){ _temp32[ i]= a;} _temp32;}); struct _tagged_ptr0 _temp34;
_temp34.curr= _temp33; _temp34.base= _temp33; _temp34.last_plus_one= _temp33 +
10; _temp34;});} else{ struct _tagged_ptr0 newarr=({ unsigned int _temp38=(
unsigned int)( xarr->num_elmts * 2); void** _temp39=( void**) GC_malloc( sizeof(
void*) * _temp38); unsigned int i; struct _tagged_ptr0 _temp40={ _temp39,
_temp39, _temp39 + _temp38}; for( i= 0; i < _temp38; i ++){ _temp39[ i]=({
struct _tagged_ptr0 _temp35= xarr->elmts; void** _temp37= _temp35.curr + 0; if(
_temp35.base == 0? 1:( _temp37 < _temp35.base? 1: _temp37 >= _temp35.last_plus_one)){
_throw( Null_Exception);}* _temp37;});} _temp40;});{ int i= 1; for( 0; i < xarr->num_elmts;
++ i){({ struct _tagged_ptr0 _temp41= newarr; void** _temp43= _temp41.curr + i;
if( _temp41.base == 0? 1:( _temp43 < _temp41.base? 1: _temp43 >= _temp41.last_plus_one)){
_throw( Null_Exception);}* _temp43=({ struct _tagged_ptr0 _temp44= xarr->elmts;
void** _temp46= _temp44.curr + i; if( _temp44.base == 0? 1:( _temp46 < _temp44.base?
1: _temp46 >= _temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46;});});}}
xarr->elmts= newarr;}}({ struct _tagged_ptr0 _temp47= xarr->elmts; void**
_temp49= _temp47.curr +( xarr->num_elmts ++); if( _temp47.base == 0? 1:( _temp49
< _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){ _throw( Null_Exception);}*
_temp49= a;});} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray* xarr, void* a){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( xarr, a);
return xarr->num_elmts - 1;} struct _tagged_ptr0 Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*
xarr){ if( xarr->num_elmts == 0){ return( struct _tagged_ptr0)({ void** _temp51=(
void**)({ void** _temp50=( void**) GC_malloc( sizeof( void*) * 0); _temp50;});
struct _tagged_ptr0 _temp52; _temp52.curr= _temp51; _temp52.base= _temp51;
_temp52.last_plus_one= _temp51 + 0; _temp52;});}{ struct _tagged_ptr0 ans=({
unsigned int _temp56=( unsigned int) xarr->num_elmts; void** _temp57=( void**)
GC_malloc( sizeof( void*) * _temp56); unsigned int i; struct _tagged_ptr0
_temp58={ _temp57, _temp57, _temp57 + _temp56}; for( i= 0; i < _temp56; i ++){
_temp57[ i]=({ struct _tagged_ptr0 _temp53= xarr->elmts; void** _temp55= _temp53.curr
+( int) i; if( _temp53.base == 0? 1:( _temp55 < _temp53.base? 1: _temp55 >=
_temp53.last_plus_one)){ _throw( Null_Exception);}* _temp55;});} _temp58;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array( struct
_tagged_ptr0 arr){ if(({ struct _tagged_ptr0 _temp59= arr;( unsigned int)(
_temp59.last_plus_one - _temp59.curr);}) ==( unsigned int) 0){ return(( struct
Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp60=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp60->elmts=({ unsigned int _temp66=({
struct _tagged_ptr0 _temp62= arr;( unsigned int)( _temp62.last_plus_one -
_temp62.curr);}); void** _temp67=( void**) GC_malloc( sizeof( void*) * _temp66);
unsigned int i; struct _tagged_ptr0 _temp68={ _temp67, _temp67, _temp67 +
_temp66}; for( i= 0; i < _temp66; i ++){ _temp67[ i]=({ struct _tagged_ptr0
_temp63= arr; void** _temp65= _temp63.curr +( int) i; if( _temp63.base == 0? 1:(
_temp65 < _temp63.base? 1: _temp65 >= _temp63.last_plus_one)){ _throw(
Null_Exception);}* _temp65;});} _temp68;}); _temp60->num_elmts=( int)({ struct
_tagged_ptr0 _temp61= arr;( unsigned int)( _temp61.last_plus_one - _temp61.curr);});
_temp60;}); return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct
Cyc_Xarray_Xarray* xarr1, struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)(({
struct _tagged_ptr0 _temp69= xarr1->elmts;( unsigned int)( _temp69.last_plus_one
- _temp69.curr);}) +({ struct _tagged_ptr0 _temp70= xarr2->elmts;( unsigned int)(
_temp70.last_plus_one - _temp70.curr);})); if( newsz == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ void* init=({ struct _tagged_ptr0 _temp71= xarr1->elmts;(
unsigned int)( _temp71.last_plus_one - _temp71.curr);}) ==( unsigned int) 0?({
struct _tagged_ptr0 _temp72= xarr2->elmts; void** _temp74= _temp72.curr + 0; if(
_temp72.base == 0? 1:( _temp74 < _temp72.base? 1: _temp74 >= _temp72.last_plus_one)){
_throw( Null_Exception);}* _temp74;}):({ struct _tagged_ptr0 _temp75= xarr1->elmts;
void** _temp77= _temp75.curr + 0; if( _temp75.base == 0? 1:( _temp77 < _temp75.base?
1: _temp77 >= _temp75.last_plus_one)){ _throw( Null_Exception);}* _temp77;});
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp78=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp78->elmts=({ unsigned int
_temp79=( unsigned int) newsz; void** _temp80=( void**) GC_malloc( sizeof( void*)
* _temp79); unsigned int i; struct _tagged_ptr0 _temp81={ _temp80, _temp80,
_temp80 + _temp79}; for( i= 0; i < _temp79; i ++){ _temp80[ i]= init;} _temp81;});
_temp78->num_elmts= 0; _temp78;});{ int i= 0; for( 0; i < xarr1->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp82= xarr1->elmts; void** _temp84= _temp82.curr + i; if(
_temp82.base == 0? 1:( _temp84 < _temp82.base? 1: _temp84 >= _temp82.last_plus_one)){
_throw( Null_Exception);}* _temp84;}));}}{ int i= 0; for( 0; i < xarr2->num_elmts;
++ i){(( void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,({
struct _tagged_ptr0 _temp85= xarr2->elmts; void** _temp87= _temp85.curr + i; if(
_temp85.base == 0? 1:( _temp87 < _temp85.base? 1: _temp87 >= _temp85.last_plus_one)){
_throw( Null_Exception);}* _temp87;}));}} return ans;}} void Cyc_Xarray_app(
void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f(({ struct _tagged_ptr0 _temp88= xarr->elmts; void** _temp90= _temp88.curr
+ i; if( _temp88.base == 0? 1:( _temp90 < _temp88.base? 1: _temp90 >= _temp88.last_plus_one)){
_throw( Null_Exception);}* _temp90;}));}} void Cyc_Xarray_app_c( void*(* f)(
void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i <
xarr->num_elmts; ++ i){ f( env,({ struct _tagged_ptr0 _temp91= xarr->elmts; void**
_temp93= _temp91.curr + i; if( _temp91.base == 0? 1:( _temp93 < _temp91.base? 1:
_temp93 >= _temp91.last_plus_one)){ _throw( Null_Exception);}* _temp93;}));}}
void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i=
0; for( 0; i < xarr->num_elmts; ++ i){ f(({ struct _tagged_ptr0 _temp94= xarr->elmts;
void** _temp96= _temp94.curr + i; if( _temp94.base == 0? 1:( _temp96 < _temp94.base?
1: _temp96 >= _temp94.last_plus_one)){ _throw( Null_Exception);}* _temp96;}));}}
void Cyc_Xarray_iter_c( void(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray*
xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f( env,({ struct
_tagged_ptr0 _temp97= xarr->elmts; void** _temp99= _temp97.curr + i; if( _temp97.base
== 0? 1:( _temp99 < _temp97.base? 1: _temp99 >= _temp97.last_plus_one)){ _throw(
Null_Exception);}* _temp99;}));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map( void*(*
f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return((
struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray*
ans=({ struct Cyc_Xarray_Xarray* _temp100=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp100->elmts=({ unsigned int _temp105=({
struct _tagged_ptr0 _temp101= xarr->elmts;( unsigned int)( _temp101.last_plus_one
- _temp101.curr);}); void** _temp106=( void**) GC_malloc( sizeof( void*) *
_temp105); unsigned int i; struct _tagged_ptr0 _temp107={ _temp106, _temp106,
_temp106 + _temp105}; for( i= 0; i < _temp105; i ++){ _temp106[ i]= f(({ struct
_tagged_ptr0 _temp102= xarr->elmts; void** _temp104= _temp102.curr + 0; if(
_temp102.base == 0? 1:( _temp104 < _temp102.base? 1: _temp104 >= _temp102.last_plus_one)){
_throw( Null_Exception);}* _temp104;}));} _temp107;}); _temp100->num_elmts= xarr->num_elmts;
_temp100;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct
_tagged_ptr0 _temp108= ans->elmts; void** _temp110= _temp108.curr + i; if(
_temp108.base == 0? 1:( _temp110 < _temp108.base? 1: _temp110 >= _temp108.last_plus_one)){
_throw( Null_Exception);}* _temp110= f(({ struct _tagged_ptr0 _temp111= xarr->elmts;
void** _temp113= _temp111.curr + i; if( _temp111.base == 0? 1:( _temp113 <
_temp111.base? 1: _temp113 >= _temp111.last_plus_one)){ _throw( Null_Exception);}*
_temp113;}));});}} return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts
== 0){ return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp114=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp114->elmts=({ unsigned int
_temp119=({ struct _tagged_ptr0 _temp115= xarr->elmts;( unsigned int)( _temp115.last_plus_one
- _temp115.curr);}); void** _temp120=( void**) GC_malloc( sizeof( void*) *
_temp119); unsigned int i; struct _tagged_ptr0 _temp121={ _temp120, _temp120,
_temp120 + _temp119}; for( i= 0; i < _temp119; i ++){ _temp120[ i]= f( env,({
struct _tagged_ptr0 _temp116= xarr->elmts; void** _temp118= _temp116.curr + 0;
if( _temp116.base == 0? 1:( _temp118 < _temp116.base? 1: _temp118 >= _temp116.last_plus_one)){
_throw( Null_Exception);}* _temp118;}));} _temp121;}); _temp114->num_elmts= xarr->num_elmts;
_temp114;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct
_tagged_ptr0 _temp122= ans->elmts; void** _temp124= _temp122.curr + i; if(
_temp122.base == 0? 1:( _temp124 < _temp122.base? 1: _temp124 >= _temp122.last_plus_one)){
_throw( Null_Exception);}* _temp124= f( env,({ struct _tagged_ptr0 _temp125=
xarr->elmts; void** _temp127= _temp125.curr + i; if( _temp125.base == 0? 1:(
_temp127 < _temp125.base? 1: _temp127 >= _temp125.last_plus_one)){ _throw(
Null_Exception);}* _temp127;}));});}} return ans;}} void Cyc_Xarray_reuse(
struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts= 0;}