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
struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; struct
Cyc_Xarray_Xarray{ struct _tagged_ptr0 elmts; int num_elmts; } ; typedef struct
Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern int Cyc_Xarray_length( struct Cyc_Xarray_Xarray*);
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
struct _tagged_ptr0 x=( struct _tagged_ptr0)({ void** _temp16=( void**)({ void**
_temp15=( void**) GC_malloc( sizeof( void*) * 0); _temp15;}); struct
_tagged_ptr0 _temp17; _temp17.curr= _temp16; _temp17.base= _temp16; _temp17.last_plus_one=
_temp16 + 0; _temp17;}); return({ struct Cyc_Xarray_Xarray* _temp14=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp14->elmts= x; _temp14->num_elmts=
0; _temp14;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int len, void* a){
if( len < 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp18=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp18[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp19; _temp19.tag= Cyc_Core_InvalidArg_tag;
_temp19.f1=( struct _tagged_string)({ char* _temp20=( char*)"xarrays must have a non-negative size buffer";
struct _tagged_string _temp21; _temp21.curr= _temp20; _temp21.base= _temp20;
_temp21.last_plus_one= _temp20 + 45; _temp21;}); _temp19;}); _temp18;}));}
return({ struct Cyc_Xarray_Xarray* _temp22=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp22->elmts=({ unsigned int
_temp23=( unsigned int) len; void** _temp24=( void**) GC_malloc( sizeof( void*)
* _temp23); struct _tagged_ptr0 _temp27={ _temp24, _temp24, _temp24 + _temp23};{
unsigned int _temp25= _temp23; unsigned int i; for( i= 0; i < _temp25; i ++){
_temp24[ i]= a;}}; _temp27;}); _temp22->num_elmts= 0; _temp22;});} struct Cyc_Xarray_Xarray*
Cyc_Xarray_singleton( int len, void* a){ if( len < 1){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp28=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp28[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp29; _temp29.tag= Cyc_Core_InvalidArg_tag;
_temp29.f1=( struct _tagged_string)({ char* _temp30=( char*)"singleton xarray must have size >=1";
struct _tagged_string _temp31; _temp31.curr= _temp30; _temp31.base= _temp30;
_temp31.last_plus_one= _temp30 + 36; _temp31;}); _temp29;}); _temp28;}));}{
struct Cyc_Xarray_Xarray* x=(( struct Cyc_Xarray_Xarray*(*)( int len, void* a))
Cyc_Xarray_create)( len, a); x->num_elmts= 1; return x;}} void Cyc_Xarray_add(
struct Cyc_Xarray_Xarray* xarr, void* a){ if( xarr->num_elmts ==({ struct
_tagged_ptr0 _temp32= xarr->elmts;( unsigned int)( _temp32.last_plus_one -
_temp32.curr);})){ if( xarr->num_elmts == 0){ xarr->elmts=( struct _tagged_ptr0)({
void** _temp37=( void**)({ unsigned int _temp33= 10u; void** _temp34=( void**)
GC_malloc( sizeof( void*) * _temp33);{ unsigned int _temp35= _temp33;
unsigned int i; for( i= 0; i < _temp35; i ++){ _temp34[ i]= a;}}; _temp34;});
struct _tagged_ptr0 _temp38; _temp38.curr= _temp37; _temp38.base= _temp37;
_temp38.last_plus_one= _temp37 + 10; _temp38;});} else{ struct _tagged_ptr0
newarr=({ unsigned int _temp45=( unsigned int)( xarr->num_elmts * 2); void**
_temp46=( void**) GC_malloc( sizeof( void*) * _temp45); struct _tagged_ptr0
_temp52={ _temp46, _temp46, _temp46 + _temp45};{ unsigned int _temp47= _temp45;
unsigned int i; for( i= 0; i < _temp47; i ++){ _temp46[ i]=({ struct
_tagged_ptr0 _temp49= xarr->elmts; void** _temp51= _temp49.curr + 0; if( _temp49.base
== 0? 1:( _temp51 < _temp49.base? 1: _temp51 >= _temp49.last_plus_one)){ _throw(
Null_Exception);}* _temp51;});}}; _temp52;});{ int i= 1; for( 0; i < xarr->num_elmts;
++ i){({ struct _tagged_ptr0 _temp39= newarr; void** _temp41= _temp39.curr + i;
if( _temp39.base == 0? 1:( _temp41 < _temp39.base? 1: _temp41 >= _temp39.last_plus_one)){
_throw( Null_Exception);}* _temp41=({ struct _tagged_ptr0 _temp42= xarr->elmts;
void** _temp44= _temp42.curr + i; if( _temp42.base == 0? 1:( _temp44 < _temp42.base?
1: _temp44 >= _temp42.last_plus_one)){ _throw( Null_Exception);}* _temp44;});});}}
xarr->elmts= newarr;}}({ struct _tagged_ptr0 _temp53= xarr->elmts; void**
_temp55= _temp53.curr +( xarr->num_elmts ++); if( _temp53.base == 0? 1:( _temp55
< _temp53.base? 1: _temp55 >= _temp53.last_plus_one)){ _throw( Null_Exception);}*
_temp55= a;});} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray* xarr, void* a){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( xarr, a);
return xarr->num_elmts - 1;} struct _tagged_ptr0 Cyc_Xarray_to_array( struct Cyc_Xarray_Xarray*
xarr){ if( xarr->num_elmts == 0){ return( struct _tagged_ptr0)({ void** _temp57=(
void**)({ void** _temp56=( void**) GC_malloc( sizeof( void*) * 0); _temp56;});
struct _tagged_ptr0 _temp58; _temp58.curr= _temp57; _temp58.base= _temp57;
_temp58.last_plus_one= _temp57 + 0; _temp58;});}{ struct _tagged_ptr0 ans=({
unsigned int _temp59=( unsigned int) xarr->num_elmts; void** _temp60=( void**)
GC_malloc( sizeof( void*) * _temp59); struct _tagged_ptr0 _temp66={ _temp60,
_temp60, _temp60 + _temp59};{ unsigned int _temp61= _temp59; unsigned int i;
for( i= 0; i < _temp61; i ++){ _temp60[ i]=({ struct _tagged_ptr0 _temp63= xarr->elmts;
void** _temp65= _temp63.curr +( int) i; if( _temp63.base == 0? 1:( _temp65 <
_temp63.base? 1: _temp65 >= _temp63.last_plus_one)){ _throw( Null_Exception);}*
_temp65;});}}; _temp66;}); return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array(
struct _tagged_ptr0 arr){ if(({ struct _tagged_ptr0 _temp67= arr;( unsigned int)(
_temp67.last_plus_one - _temp67.curr);}) == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp68=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp68->elmts=({ unsigned int _temp70=({ struct _tagged_ptr0 _temp72= arr;(
unsigned int)( _temp72.last_plus_one - _temp72.curr);}); void** _temp71=( void**)
GC_malloc( sizeof( void*) * _temp70); struct _tagged_ptr0 _temp78={ _temp71,
_temp71, _temp71 + _temp70};{ unsigned int _temp73= _temp70; unsigned int i;
for( i= 0; i < _temp73; i ++){ _temp71[ i]=({ struct _tagged_ptr0 _temp75= arr;
void** _temp77= _temp75.curr +( int) i; if( _temp75.base == 0? 1:( _temp77 <
_temp75.base? 1: _temp77 >= _temp75.last_plus_one)){ _throw( Null_Exception);}*
_temp77;});}}; _temp78;}); _temp68->num_elmts=( int)({ struct _tagged_ptr0
_temp69= arr;( unsigned int)( _temp69.last_plus_one - _temp69.curr);}); _temp68;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray*
xarr1, struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)(({ struct _tagged_ptr0
_temp98= xarr1->elmts;( unsigned int)( _temp98.last_plus_one - _temp98.curr);})
+({ struct _tagged_ptr0 _temp99= xarr2->elmts;( unsigned int)( _temp99.last_plus_one
- _temp99.curr);})); if( newsz == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ void* init=({ struct _tagged_ptr0 _temp91= xarr1->elmts;(
unsigned int)( _temp91.last_plus_one - _temp91.curr);}) == 0?({ struct
_tagged_ptr0 _temp92= xarr2->elmts; void** _temp94= _temp92.curr + 0; if(
_temp92.base == 0? 1:( _temp94 < _temp92.base? 1: _temp94 >= _temp92.last_plus_one)){
_throw( Null_Exception);}* _temp94;}):({ struct _tagged_ptr0 _temp95= xarr1->elmts;
void** _temp97= _temp95.curr + 0; if( _temp95.base == 0? 1:( _temp97 < _temp95.base?
1: _temp97 >= _temp95.last_plus_one)){ _throw( Null_Exception);}* _temp97;});
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp85=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp85->elmts=({ unsigned int
_temp86=( unsigned int) newsz; void** _temp87=( void**) GC_malloc( sizeof( void*)
* _temp86); struct _tagged_ptr0 _temp90={ _temp87, _temp87, _temp87 + _temp86};{
unsigned int _temp88= _temp86; unsigned int i; for( i= 0; i < _temp88; i ++){
_temp87[ i]= init;}}; _temp90;}); _temp85->num_elmts= 0; _temp85;});{ int i= 0;
for( 0; i < xarr1->num_elmts; ++ i){(( void(*)( struct Cyc_Xarray_Xarray* xarr,
void* a)) Cyc_Xarray_add)( ans,({ struct _tagged_ptr0 _temp79= xarr1->elmts;
void** _temp81= _temp79.curr + i; if( _temp79.base == 0? 1:( _temp81 < _temp79.base?
1: _temp81 >= _temp79.last_plus_one)){ _throw( Null_Exception);}* _temp81;}));}}{
int i= 0; for( 0; i < xarr2->num_elmts; ++ i){(( void(*)( struct Cyc_Xarray_Xarray*
xarr, void* a)) Cyc_Xarray_add)( ans,({ struct _tagged_ptr0 _temp82= xarr2->elmts;
void** _temp84= _temp82.curr + i; if( _temp82.base == 0? 1:( _temp84 < _temp82.base?
1: _temp84 >= _temp82.last_plus_one)){ _throw( Null_Exception);}* _temp84;}));}}
return ans;}} void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray*
xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f(({ struct _tagged_ptr0
_temp100= xarr->elmts; void** _temp102= _temp100.curr + i; if( _temp100.base ==
0? 1:( _temp102 < _temp100.base? 1: _temp102 >= _temp100.last_plus_one)){ _throw(
Null_Exception);}* _temp102;}));}} void Cyc_Xarray_app_c( void*(* f)( void*,
void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f( env,({ struct _tagged_ptr0 _temp103= xarr->elmts; void** _temp105=
_temp103.curr + i; if( _temp103.base == 0? 1:( _temp105 < _temp103.base? 1:
_temp105 >= _temp103.last_plus_one)){ _throw( Null_Exception);}* _temp105;}));}}
void Cyc_Xarray_iter( void(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i=
0; for( 0; i < xarr->num_elmts; ++ i){ f(({ struct _tagged_ptr0 _temp106= xarr->elmts;
void** _temp108= _temp106.curr + i; if( _temp106.base == 0? 1:( _temp108 <
_temp106.base? 1: _temp108 >= _temp106.last_plus_one)){ _throw( Null_Exception);}*
_temp108;}));}} void Cyc_Xarray_iter_c( void(* f)( void*, void*), void* env,
struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f(
env,({ struct _tagged_ptr0 _temp109= xarr->elmts; void** _temp111= _temp109.curr
+ i; if( _temp109.base == 0? 1:( _temp111 < _temp109.base? 1: _temp111 >=
_temp109.last_plus_one)){ _throw( Null_Exception);}* _temp111;}));}} struct Cyc_Xarray_Xarray*
Cyc_Xarray_map( void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts
== 0){ return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp118=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp118->elmts=({ unsigned int
_temp119=({ struct _tagged_ptr0 _temp121= xarr->elmts;( unsigned int)( _temp121.last_plus_one
- _temp121.curr);}); void** _temp120=( void**) GC_malloc( sizeof( void*) *
_temp119); struct _tagged_ptr0 _temp127={ _temp120, _temp120, _temp120 +
_temp119};{ unsigned int _temp122= _temp119; unsigned int i; for( i= 0; i <
_temp122; i ++){ _temp120[ i]= f(({ struct _tagged_ptr0 _temp124= xarr->elmts;
void** _temp126= _temp124.curr + 0; if( _temp124.base == 0? 1:( _temp126 <
_temp124.base? 1: _temp126 >= _temp124.last_plus_one)){ _throw( Null_Exception);}*
_temp126;}));}}; _temp127;}); _temp118->num_elmts= xarr->num_elmts; _temp118;});{
int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct _tagged_ptr0 _temp112=
ans->elmts; void** _temp114= _temp112.curr + i; if( _temp112.base == 0? 1:(
_temp114 < _temp112.base? 1: _temp114 >= _temp112.last_plus_one)){ _throw(
Null_Exception);}* _temp114= f(({ struct _tagged_ptr0 _temp115= xarr->elmts;
void** _temp117= _temp115.curr + i; if( _temp115.base == 0? 1:( _temp117 <
_temp115.base? 1: _temp117 >= _temp115.last_plus_one)){ _throw( Null_Exception);}*
_temp117;}));});}} return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts
== 0){ return(( struct Cyc_Xarray_Xarray*(*)()) Cyc_Xarray_create_empty)();}{
struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp134=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp134->elmts=({ unsigned int
_temp135=({ struct _tagged_ptr0 _temp137= xarr->elmts;( unsigned int)( _temp137.last_plus_one
- _temp137.curr);}); void** _temp136=( void**) GC_malloc( sizeof( void*) *
_temp135); struct _tagged_ptr0 _temp143={ _temp136, _temp136, _temp136 +
_temp135};{ unsigned int _temp138= _temp135; unsigned int i; for( i= 0; i <
_temp138; i ++){ _temp136[ i]= f( env,({ struct _tagged_ptr0 _temp140= xarr->elmts;
void** _temp142= _temp140.curr + 0; if( _temp140.base == 0? 1:( _temp142 <
_temp140.base? 1: _temp142 >= _temp140.last_plus_one)){ _throw( Null_Exception);}*
_temp142;}));}}; _temp143;}); _temp134->num_elmts= xarr->num_elmts; _temp134;});{
int i= 1; for( 0; i < xarr->num_elmts; ++ i){({ struct _tagged_ptr0 _temp128=
ans->elmts; void** _temp130= _temp128.curr + i; if( _temp128.base == 0? 1:(
_temp130 < _temp128.base? 1: _temp130 >= _temp128.last_plus_one)){ _throw(
Null_Exception);}* _temp130= f( env,({ struct _tagged_ptr0 _temp131= xarr->elmts;
void** _temp133= _temp131.curr + i; if( _temp131.base == 0? 1:( _temp133 <
_temp131.base? 1: _temp133 >= _temp131.last_plus_one)){ _throw( Null_Exception);}*
_temp133;}));});}} return ans;}} void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray*
xarr){ xarr->num_elmts= 0;}