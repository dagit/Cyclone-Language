 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
_tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; struct Cyc_Xarray_Xarray{
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
xarr->num_elmts){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp0=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp1; _temp1.tag= Cyc_Core_InvalidArg;
_temp1.f1=({ unsigned char* _temp2="Xarray::get: bad index"; struct
_tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 23; _temp3;}); _temp1;}); _temp0;}));} return*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts,
sizeof( void*), i);} void Cyc_Xarray_set( struct Cyc_Xarray_Xarray* xarr, int i,
void* a){ if( i < 0? 1: i >= xarr->num_elmts){( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp4=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp4[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp5; _temp5.tag= Cyc_Core_InvalidArg; _temp5.f1=({ unsigned char* _temp6="Xarray::set: bad index";
struct _tagged_string _temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one=
_temp6 + 23; _temp7;}); _temp5;}); _temp4;}));}*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts,
sizeof( void*), i)= a;} struct Cyc_Xarray_Xarray* Cyc_Xarray_create_empty(){
struct _tagged_ptr0 x=({ void** _temp10=({ void** _temp9=( void**) GC_malloc(
sizeof( void*) * 0); _temp9;}); struct _tagged_ptr0 _temp11; _temp11.curr=
_temp10; _temp11.base= _temp10; _temp11.last_plus_one= _temp10 + 0; _temp11;});
return({ struct Cyc_Xarray_Xarray* _temp8=( struct Cyc_Xarray_Xarray*) GC_malloc(
sizeof( struct Cyc_Xarray_Xarray)); _temp8->elmts= x; _temp8->num_elmts= 0;
_temp8;});} struct Cyc_Xarray_Xarray* Cyc_Xarray_create( int len, void* a){ if(
len < 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp12=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp12[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp13; _temp13.tag= Cyc_Core_InvalidArg;
_temp13.f1=({ unsigned char* _temp14="xarrays must have a non-negative size buffer";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 45; _temp15;}); _temp13;}); _temp12;}));}
return({ struct Cyc_Xarray_Xarray* _temp16=( struct Cyc_Xarray_Xarray*)
GC_malloc( sizeof( struct Cyc_Xarray_Xarray)); _temp16->elmts=({ unsigned int
_temp17=( unsigned int) len; void** _temp18=( void**) GC_malloc( sizeof( void*)
* _temp17); struct _tagged_ptr0 _temp20={ _temp18, _temp18, _temp18 + _temp17};{
unsigned int _temp19= _temp17; unsigned int i; for( i= 0; i < _temp19; i ++){
_temp18[ i]= a;}}; _temp20;}); _temp16->num_elmts= 0; _temp16;});} struct Cyc_Xarray_Xarray*
Cyc_Xarray_singleton( int len, void* a){ if( len < 1){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp21=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp21[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp22; _temp22.tag= Cyc_Core_InvalidArg; _temp22.f1=({
unsigned char* _temp23="singleton xarray must have size >=1"; struct
_tagged_string _temp24; _temp24.curr= _temp23; _temp24.base= _temp23; _temp24.last_plus_one=
_temp23 + 36; _temp24;}); _temp22;}); _temp21;}));}{ struct Cyc_Xarray_Xarray* x=((
struct Cyc_Xarray_Xarray*(*)( int len, void* a)) Cyc_Xarray_create)( len, a); x->num_elmts=
1; return x;}} void Cyc_Xarray_add( struct Cyc_Xarray_Xarray* xarr, void* a){
if( xarr->num_elmts ==({ struct _tagged_ptr0 _temp25= xarr->elmts;( unsigned int)(
_temp25.last_plus_one - _temp25.curr);})){ if( xarr->num_elmts == 0){ xarr->elmts=({
void** _temp29=({ unsigned int _temp26= 10u; void** _temp27=( void**) GC_malloc(
sizeof( void*) * _temp26);{ unsigned int _temp28= _temp26; unsigned int i; for(
i= 0; i < _temp28; i ++){ _temp27[ i]= a;}}; _temp27;}); struct _tagged_ptr0
_temp30; _temp30.curr= _temp29; _temp30.base= _temp29; _temp30.last_plus_one=
_temp29 + 10; _temp30;});} else{ struct _tagged_ptr0 newarr=({ unsigned int
_temp31=( unsigned int)( xarr->num_elmts * 2); void** _temp32=( void**)
GC_malloc( sizeof( void*) * _temp31); struct _tagged_ptr0 _temp34={ _temp32,
_temp32, _temp32 + _temp31};{ unsigned int _temp33= _temp31; unsigned int i;
for( i= 0; i < _temp33; i ++){ _temp32[ i]=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*), 0);}}; _temp34;});{ int i= 1; for( 0; i < xarr->num_elmts; ++ i){*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( newarr, sizeof( void*), i)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts,
sizeof( void*), i);}} xarr->elmts= newarr;}}*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*), xarr->num_elmts ++)= a;} int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*
xarr, void* a){(( void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)(
xarr, a); return xarr->num_elmts - 1;} struct _tagged_ptr0 Cyc_Xarray_to_array(
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return({ void**
_temp36=({ void** _temp35=( void**) GC_malloc( sizeof( void*) * 0); _temp35;});
struct _tagged_ptr0 _temp37; _temp37.curr= _temp36; _temp37.base= _temp36;
_temp37.last_plus_one= _temp36 + 0; _temp37;});}{ struct _tagged_ptr0 ans=({
unsigned int _temp38=( unsigned int) xarr->num_elmts; void** _temp39=( void**)
GC_malloc( sizeof( void*) * _temp38); struct _tagged_ptr0 _temp41={ _temp39,
_temp39, _temp39 + _temp38};{ unsigned int _temp40= _temp38; unsigned int i;
for( i= 0; i < _temp40; i ++){ _temp39[ i]=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*),( int) i);}}; _temp41;}); return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_from_array(
struct _tagged_ptr0 arr){ if(({ struct _tagged_ptr0 _temp42= arr;( unsigned int)(
_temp42.last_plus_one - _temp42.curr);}) == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp43=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp43->elmts=({ unsigned int _temp45=({ struct _tagged_ptr0 _temp47= arr;(
unsigned int)( _temp47.last_plus_one - _temp47.curr);}); void** _temp46=( void**)
GC_malloc( sizeof( void*) * _temp45); struct _tagged_ptr0 _temp49={ _temp46,
_temp46, _temp46 + _temp45};{ unsigned int _temp48= _temp45; unsigned int i;
for( i= 0; i < _temp48; i ++){ _temp46[ i]=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*),(
int) i);}}; _temp49;}); _temp43->num_elmts=( int)({ struct _tagged_ptr0 _temp44=
arr;( unsigned int)( _temp44.last_plus_one - _temp44.curr);}); _temp43;});
return ans;}} struct Cyc_Xarray_Xarray* Cyc_Xarray_append( struct Cyc_Xarray_Xarray*
xarr1, struct Cyc_Xarray_Xarray* xarr2){ int newsz=( int)(({ struct _tagged_ptr0
_temp56= xarr1->elmts;( unsigned int)( _temp56.last_plus_one - _temp56.curr);})
+({ struct _tagged_ptr0 _temp57= xarr2->elmts;( unsigned int)( _temp57.last_plus_one
- _temp57.curr);})); if( newsz == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ void* init=({ struct _tagged_ptr0 _temp55= xarr1->elmts;(
unsigned int)( _temp55.last_plus_one - _temp55.curr);}) == 0?*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
xarr2->elmts, sizeof( void*), 0):*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr1->elmts, sizeof(
void*), 0); struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray* _temp50=(
struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp50->elmts=({ unsigned int _temp51=( unsigned int) newsz; void** _temp52=(
void**) GC_malloc( sizeof( void*) * _temp51); struct _tagged_ptr0 _temp54={
_temp52, _temp52, _temp52 + _temp51};{ unsigned int _temp53= _temp51;
unsigned int i; for( i= 0; i < _temp53; i ++){ _temp52[ i]= init;}}; _temp54;});
_temp50->num_elmts= 0; _temp50;});{ int i= 0; for( 0; i < xarr1->num_elmts; ++ i){((
void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
xarr1->elmts, sizeof( void*), i));}}{ int i= 0; for( 0; i < xarr2->num_elmts; ++
i){(( void(*)( struct Cyc_Xarray_Xarray* xarr, void* a)) Cyc_Xarray_add)( ans,*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( xarr2->elmts, sizeof( void*), i));}} return ans;}}
void Cyc_Xarray_app( void*(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i=
0; for( 0; i < xarr->num_elmts; ++ i){ f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*), i));}} void Cyc_Xarray_app_c( void*(* f)( void*, void*), void* env,
struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts; ++ i){ f(
env,*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( xarr->elmts, sizeof( void*), i));}} void Cyc_Xarray_iter(
void(* f)( void*), struct Cyc_Xarray_Xarray* xarr){ int i= 0; for( 0; i < xarr->num_elmts;
++ i){ f(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( xarr->elmts, sizeof( void*), i));}} void Cyc_Xarray_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_Xarray_Xarray* xarr){ int i= 0;
for( 0; i < xarr->num_elmts; ++ i){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*), i));}} struct Cyc_Xarray_Xarray* Cyc_Xarray_map( void*(* f)( void*),
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp58=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp58->elmts=({ unsigned int _temp59=({ struct _tagged_ptr0 _temp61= xarr->elmts;(
unsigned int)( _temp61.last_plus_one - _temp61.curr);}); void** _temp60=( void**)
GC_malloc( sizeof( void*) * _temp59); struct _tagged_ptr0 _temp63={ _temp60,
_temp60, _temp60 + _temp59};{ unsigned int _temp62= _temp59; unsigned int i;
for( i= 0; i < _temp62; i ++){ _temp60[ i]= f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts, sizeof(
void*), 0));}}; _temp63;}); _temp58->num_elmts= xarr->num_elmts; _temp58;});{
int i= 1; for( 0; i < xarr->num_elmts; ++ i){*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( ans->elmts, sizeof( void*),
i)= f(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( xarr->elmts, sizeof( void*), i));}} return ans;}}
struct Cyc_Xarray_Xarray* Cyc_Xarray_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Xarray_Xarray* xarr){ if( xarr->num_elmts == 0){ return(( struct Cyc_Xarray_Xarray*(*)())
Cyc_Xarray_create_empty)();}{ struct Cyc_Xarray_Xarray* ans=({ struct Cyc_Xarray_Xarray*
_temp64=( struct Cyc_Xarray_Xarray*) GC_malloc( sizeof( struct Cyc_Xarray_Xarray));
_temp64->elmts=({ unsigned int _temp65=({ struct _tagged_ptr0 _temp67= xarr->elmts;(
unsigned int)( _temp67.last_plus_one - _temp67.curr);}); void** _temp66=( void**)
GC_malloc( sizeof( void*) * _temp65); struct _tagged_ptr0 _temp69={ _temp66,
_temp66, _temp66 + _temp65};{ unsigned int _temp68= _temp65; unsigned int i;
for( i= 0; i < _temp68; i ++){ _temp66[ i]= f( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( xarr->elmts,
sizeof( void*), 0));}}; _temp69;}); _temp64->num_elmts= xarr->num_elmts; _temp64;});{
int i= 1; for( 0; i < xarr->num_elmts; ++ i){*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( ans->elmts, sizeof( void*),
i)= f( env,*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( xarr->elmts, sizeof( void*), i));}} return ans;}}
void Cyc_Xarray_reuse( struct Cyc_Xarray_Xarray* xarr){ xarr->num_elmts= 0;}