#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u]; struct
_tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; extern struct
_tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_from_array( struct _tagged_ptr0 arr); extern void Cyc_Array_qsort( int(*
less_eq)( void*, void*), struct _tagged_ptr0 x, int ofs, int len); extern void
Cyc_Array_msort( int(* less_eq)( void*, void*), struct _tagged_ptr0 x, int ofs,
int len); extern struct _tagged_ptr0 Cyc_Array_from_list( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_Array_to_list( struct _tagged_ptr0 x);
extern struct _tagged_ptr0 Cyc_Array_copy( struct _tagged_ptr0 x); extern struct
_tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x); extern
struct _tagged_ptr0 Cyc_Array_map_c( void*(* f)( void*, void*), void* env,
struct _tagged_ptr0 x); extern void Cyc_Array_imp_map( void*(* f)( void*),
struct _tagged_ptr0 x); extern void Cyc_Array_imp_map_c( void*(* f)( void*, void*),
void* env, struct _tagged_ptr0 x); extern char Cyc_Array_Array_mismatch[ 19u];
extern struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_app( void*(* f)(
void*), struct _tagged_ptr0 x); extern void Cyc_Array_app_c( void*(* f)( void*,
void*), void* env, struct _tagged_ptr0 x); extern void Cyc_Array_iter( void(* f)(
void*), struct _tagged_ptr0 x); extern void Cyc_Array_iter_c( void(* f)( void*,
void*), void* env, struct _tagged_ptr0 x); extern void Cyc_Array_app2( void*(* f)(
void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y); extern void Cyc_Array_iter2( void(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y);
extern void* Cyc_Array_fold_left( void*(* f)( void*, void*), void* accum, struct
_tagged_ptr0 x); extern void* Cyc_Array_fold_left_c( void*(* f)( void*, void*,
void*), void* env, void* accum, struct _tagged_ptr0 x); extern void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum); extern void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum);
extern struct _tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x); extern
void Cyc_Array_imp_rev( struct _tagged_ptr0 x); extern int Cyc_Array_forall( int(*
pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_forall_c( int(* pred)(
void*, void*), void* env, struct _tagged_ptr0 x); extern int Cyc_Array_exists(
int(* pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_exists_c( int(*
pred)( void*, void*), void* env, struct _tagged_ptr0 x); struct _tuple0{ void*
f1; void* f2; } ; struct _tagged_ptr1{ struct _tuple0* curr; struct _tuple0*
base; struct _tuple0* last_plus_one; } ; extern struct _tagged_ptr1 Cyc_Array_zip(
struct _tagged_ptr0 x, struct _tagged_ptr0 y); struct _tuple1{ struct
_tagged_ptr0 f1; struct _tagged_ptr0 f2; } ; extern struct _tuple1 Cyc_Array_split(
struct _tagged_ptr1 x); extern int Cyc_Array_memq( struct _tagged_ptr0 l, void*
x); extern int Cyc_Array_mem( int(* compare)( void*, void*), struct _tagged_ptr0
l, void* x); extern struct _tagged_ptr0 Cyc_Array_extract( struct _tagged_ptr0 x,
int start, int* n_opt); static int Cyc_Array_sp[ 40u]={ 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0}; void Cyc_Array_qsort( int(* less_eq)( void*, void*), struct
_tagged_ptr0 arr, int base_ofs, int len){ void* temp; int sp_ofs; int i; int j;
int limit_ofs; if(( base_ofs < 0? 1: base_ofs + len >({ struct _tagged_ptr0
_temp0= arr;( unsigned int)( _temp0.last_plus_one - _temp0.curr);}))? 1: len < 0){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp1=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp1[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp2; _temp2.tag= Cyc_Core_InvalidArg; _temp2.f1=( struct _tagged_string)({
char* _temp3=( char*)"Array::qsort"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 13; _temp4;});
_temp2;}); _temp1;}));} limit_ofs= base_ofs + len; sp_ofs= 0; for( 0; 1; 0){ if(
limit_ofs - base_ofs > 3){ temp=({ struct _tagged_ptr0 _temp5= arr; void**
_temp7= _temp5.curr +(( limit_ofs - base_ofs) / 2 + base_ofs); if( _temp5.base
== 0? 1:( _temp7 < _temp5.base? 1: _temp7 >= _temp5.last_plus_one)){ _throw(
Null_Exception);}* _temp7;});({ struct _tagged_ptr0 _temp8= arr; void** _temp10=
_temp8.curr +(( limit_ofs - base_ofs) / 2 + base_ofs); if( _temp8.base == 0? 1:(
_temp10 < _temp8.base? 1: _temp10 >= _temp8.last_plus_one)){ _throw(
Null_Exception);}* _temp10=({ struct _tagged_ptr0 _temp11= arr; void** _temp13=
_temp11.curr + base_ofs; if( _temp11.base == 0? 1:( _temp13 < _temp11.base? 1:
_temp13 >= _temp11.last_plus_one)){ _throw( Null_Exception);}* _temp13;});});({
struct _tagged_ptr0 _temp14= arr; void** _temp16= _temp14.curr + base_ofs; if(
_temp14.base == 0? 1:( _temp16 < _temp14.base? 1: _temp16 >= _temp14.last_plus_one)){
_throw( Null_Exception);}* _temp16= temp;}); i= base_ofs + 1; j= limit_ofs - 1;
if( less_eq(({ struct _tagged_ptr0 _temp17= arr; void** _temp19= _temp17.curr +
i; if( _temp17.base == 0? 1:( _temp19 < _temp17.base? 1: _temp19 >= _temp17.last_plus_one)){
_throw( Null_Exception);}* _temp19;}),({ struct _tagged_ptr0 _temp20= arr; void**
_temp22= _temp20.curr + j; if( _temp20.base == 0? 1:( _temp22 < _temp20.base? 1:
_temp22 >= _temp20.last_plus_one)){ _throw( Null_Exception);}* _temp22;})) > 0){
temp=({ struct _tagged_ptr0 _temp23= arr; void** _temp25= _temp23.curr + i; if(
_temp23.base == 0? 1:( _temp25 < _temp23.base? 1: _temp25 >= _temp23.last_plus_one)){
_throw( Null_Exception);}* _temp25;});({ struct _tagged_ptr0 _temp26= arr; void**
_temp28= _temp26.curr + i; if( _temp26.base == 0? 1:( _temp28 < _temp26.base? 1:
_temp28 >= _temp26.last_plus_one)){ _throw( Null_Exception);}* _temp28=({ struct
_tagged_ptr0 _temp29= arr; void** _temp31= _temp29.curr + j; if( _temp29.base ==
0? 1:( _temp31 < _temp29.base? 1: _temp31 >= _temp29.last_plus_one)){ _throw(
Null_Exception);}* _temp31;});});({ struct _tagged_ptr0 _temp32= arr; void**
_temp34= _temp32.curr + j; if( _temp32.base == 0? 1:( _temp34 < _temp32.base? 1:
_temp34 >= _temp32.last_plus_one)){ _throw( Null_Exception);}* _temp34= temp;});}
if( less_eq(({ struct _tagged_ptr0 _temp35= arr; void** _temp37= _temp35.curr +
base_ofs; if( _temp35.base == 0? 1:( _temp37 < _temp35.base? 1: _temp37 >=
_temp35.last_plus_one)){ _throw( Null_Exception);}* _temp37;}),({ struct
_tagged_ptr0 _temp38= arr; void** _temp40= _temp38.curr + j; if( _temp38.base ==
0? 1:( _temp40 < _temp38.base? 1: _temp40 >= _temp38.last_plus_one)){ _throw(
Null_Exception);}* _temp40;})) > 0){ temp=({ struct _tagged_ptr0 _temp41= arr;
void** _temp43= _temp41.curr + base_ofs; if( _temp41.base == 0? 1:( _temp43 <
_temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw( Null_Exception);}*
_temp43;});({ struct _tagged_ptr0 _temp44= arr; void** _temp46= _temp44.curr +
base_ofs; if( _temp44.base == 0? 1:( _temp46 < _temp44.base? 1: _temp46 >=
_temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46=({ struct
_tagged_ptr0 _temp47= arr; void** _temp49= _temp47.curr + j; if( _temp47.base ==
0? 1:( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){ _throw(
Null_Exception);}* _temp49;});});({ struct _tagged_ptr0 _temp50= arr; void**
_temp52= _temp50.curr + j; if( _temp50.base == 0? 1:( _temp52 < _temp50.base? 1:
_temp52 >= _temp50.last_plus_one)){ _throw( Null_Exception);}* _temp52= temp;});}
if( less_eq(({ struct _tagged_ptr0 _temp53= arr; void** _temp55= _temp53.curr +
i; if( _temp53.base == 0? 1:( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one)){
_throw( Null_Exception);}* _temp55;}),({ struct _tagged_ptr0 _temp56= arr; void**
_temp58= _temp56.curr + base_ofs; if( _temp56.base == 0? 1:( _temp58 < _temp56.base?
1: _temp58 >= _temp56.last_plus_one)){ _throw( Null_Exception);}* _temp58;})) >
0){ temp=({ struct _tagged_ptr0 _temp59= arr; void** _temp61= _temp59.curr + i;
if( _temp59.base == 0? 1:( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one)){
_throw( Null_Exception);}* _temp61;});({ struct _tagged_ptr0 _temp62= arr; void**
_temp64= _temp62.curr + i; if( _temp62.base == 0? 1:( _temp64 < _temp62.base? 1:
_temp64 >= _temp62.last_plus_one)){ _throw( Null_Exception);}* _temp64=({ struct
_tagged_ptr0 _temp65= arr; void** _temp67= _temp65.curr + base_ofs; if( _temp65.base
== 0? 1:( _temp67 < _temp65.base? 1: _temp67 >= _temp65.last_plus_one)){ _throw(
Null_Exception);}* _temp67;});});({ struct _tagged_ptr0 _temp68= arr; void**
_temp70= _temp68.curr + base_ofs; if( _temp68.base == 0? 1:( _temp70 < _temp68.base?
1: _temp70 >= _temp68.last_plus_one)){ _throw( Null_Exception);}* _temp70= temp;});}
for( 0; 1; 0){ do { i ++;} while ( less_eq(({ struct _tagged_ptr0 _temp71= arr;
void** _temp73= _temp71.curr + i; if( _temp71.base == 0? 1:( _temp73 < _temp71.base?
1: _temp73 >= _temp71.last_plus_one)){ _throw( Null_Exception);}* _temp73;}),({
struct _tagged_ptr0 _temp74= arr; void** _temp76= _temp74.curr + base_ofs; if(
_temp74.base == 0? 1:( _temp76 < _temp74.base? 1: _temp76 >= _temp74.last_plus_one)){
_throw( Null_Exception);}* _temp76;})) < 0); do { j --;} while ( less_eq(({
struct _tagged_ptr0 _temp77= arr; void** _temp79= _temp77.curr + j; if( _temp77.base
== 0? 1:( _temp79 < _temp77.base? 1: _temp79 >= _temp77.last_plus_one)){ _throw(
Null_Exception);}* _temp79;}),({ struct _tagged_ptr0 _temp80= arr; void**
_temp82= _temp80.curr + base_ofs; if( _temp80.base == 0? 1:( _temp82 < _temp80.base?
1: _temp82 >= _temp80.last_plus_one)){ _throw( Null_Exception);}* _temp82;})) >
0); if( i > j){ break;} temp=({ struct _tagged_ptr0 _temp83= arr; void** _temp85=
_temp83.curr + i; if( _temp83.base == 0? 1:( _temp85 < _temp83.base? 1: _temp85
>= _temp83.last_plus_one)){ _throw( Null_Exception);}* _temp85;});({ struct
_tagged_ptr0 _temp86= arr; void** _temp88= _temp86.curr + i; if( _temp86.base ==
0? 1:( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one)){ _throw(
Null_Exception);}* _temp88=({ struct _tagged_ptr0 _temp89= arr; void** _temp91=
_temp89.curr + j; if( _temp89.base == 0? 1:( _temp91 < _temp89.base? 1: _temp91
>= _temp89.last_plus_one)){ _throw( Null_Exception);}* _temp91;});});({ struct
_tagged_ptr0 _temp92= arr; void** _temp94= _temp92.curr + j; if( _temp92.base ==
0? 1:( _temp94 < _temp92.base? 1: _temp94 >= _temp92.last_plus_one)){ _throw(
Null_Exception);}* _temp94= temp;});;} temp=({ struct _tagged_ptr0 _temp95= arr;
void** _temp97= _temp95.curr + base_ofs; if( _temp95.base == 0? 1:( _temp97 <
_temp95.base? 1: _temp97 >= _temp95.last_plus_one)){ _throw( Null_Exception);}*
_temp97;});({ struct _tagged_ptr0 _temp98= arr; void** _temp100= _temp98.curr +
base_ofs; if( _temp98.base == 0? 1:( _temp100 < _temp98.base? 1: _temp100 >=
_temp98.last_plus_one)){ _throw( Null_Exception);}* _temp100=({ struct
_tagged_ptr0 _temp101= arr; void** _temp103= _temp101.curr + j; if( _temp101.base
== 0? 1:( _temp103 < _temp101.base? 1: _temp103 >= _temp101.last_plus_one)){
_throw( Null_Exception);}* _temp103;});});({ struct _tagged_ptr0 _temp104= arr;
void** _temp106= _temp104.curr + j; if( _temp104.base == 0? 1:( _temp106 <
_temp104.base? 1: _temp106 >= _temp104.last_plus_one)){ _throw( Null_Exception);}*
_temp106= temp;}); if( j - base_ofs > limit_ofs - i){({ int* _temp107=( int*)
Cyc_Array_sp; unsigned int _temp108= sp_ofs; if( _temp108 >= 40u){ _throw(
Null_Exception);} _temp107[ _temp108]= base_ofs;});({ int* _temp109=( int*) Cyc_Array_sp;
unsigned int _temp110= sp_ofs + 1; if( _temp110 >= 40u){ _throw( Null_Exception);}
_temp109[ _temp110]= j;}); base_ofs= i;} else{({ int* _temp111=( int*) Cyc_Array_sp;
unsigned int _temp112= sp_ofs; if( _temp112 >= 40u){ _throw( Null_Exception);}
_temp111[ _temp112]= i;});({ int* _temp113=( int*) Cyc_Array_sp; unsigned int
_temp114= sp_ofs + 1; if( _temp114 >= 40u){ _throw( Null_Exception);} _temp113[
_temp114]= limit_ofs;}); limit_ofs= j;} sp_ofs += 2;} else{ for( j= base_ofs, i=
j + 1; i < limit_ofs; j= i, i ++){ for( 0; less_eq(({ struct _tagged_ptr0
_temp115= arr; void** _temp117= _temp115.curr + j; if( _temp115.base == 0? 1:(
_temp117 < _temp115.base? 1: _temp117 >= _temp115.last_plus_one)){ _throw(
Null_Exception);}* _temp117;}),({ struct _tagged_ptr0 _temp118= arr; void**
_temp120= _temp118.curr +( j + 1); if( _temp118.base == 0? 1:( _temp120 <
_temp118.base? 1: _temp120 >= _temp118.last_plus_one)){ _throw( Null_Exception);}*
_temp120;})) > 0; j --){ temp=({ struct _tagged_ptr0 _temp121= arr; void**
_temp123= _temp121.curr + j; if( _temp121.base == 0? 1:( _temp123 < _temp121.base?
1: _temp123 >= _temp121.last_plus_one)){ _throw( Null_Exception);}* _temp123;});({
struct _tagged_ptr0 _temp124= arr; void** _temp126= _temp124.curr + j; if(
_temp124.base == 0? 1:( _temp126 < _temp124.base? 1: _temp126 >= _temp124.last_plus_one)){
_throw( Null_Exception);}* _temp126=({ struct _tagged_ptr0 _temp127= arr; void**
_temp129= _temp127.curr +( j + 1); if( _temp127.base == 0? 1:( _temp129 <
_temp127.base? 1: _temp129 >= _temp127.last_plus_one)){ _throw( Null_Exception);}*
_temp129;});});({ struct _tagged_ptr0 _temp130= arr; void** _temp132= _temp130.curr
+( j + 1); if( _temp130.base == 0? 1:( _temp132 < _temp130.base? 1: _temp132 >=
_temp130.last_plus_one)){ _throw( Null_Exception);}* _temp132= temp;}); if( j ==
base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs=({ int* _temp133=(
int*) Cyc_Array_sp; unsigned int _temp134= sp_ofs; if( _temp134 >= 40u){ _throw(
Null_Exception);} _temp133[ _temp134];}); limit_ofs=({ int* _temp135=( int*) Cyc_Array_sp;
unsigned int _temp136= sp_ofs + 1; if( _temp136 >= 40u){ _throw( Null_Exception);}
_temp135[ _temp136];});} else{ break;}}}} void Cyc_Array_msort( int(* less_eq)(
void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){ if(( base_ofs <
0? 1: base_ofs + len >({ struct _tagged_ptr0 _temp137= arr;( unsigned int)(
_temp137.last_plus_one - _temp137.curr);}))? 1: len < 0){( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp138=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp138[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp139; _temp139.tag= Cyc_Core_InvalidArg; _temp139.f1=(
struct _tagged_string)({ char* _temp140=( char*)"Array::msort"; struct
_tagged_string _temp141; _temp141.curr= _temp140; _temp141.base= _temp140;
_temp141.last_plus_one= _temp140 + 13; _temp141;}); _temp139;}); _temp138;}));}{
int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({ unsigned int _temp186=(
unsigned int) len; void** _temp187=( void**) GC_malloc( sizeof( void*) *
_temp186); struct _tagged_ptr0 _temp193={ _temp187, _temp187, _temp187 +
_temp186};{ unsigned int _temp188= _temp186; unsigned int i; for( i= 0; i <
_temp188; i ++){ _temp187[ i]=({ struct _tagged_ptr0 _temp190= arr; void**
_temp192= _temp190.curr + 0; if( _temp190.base == 0? 1:( _temp192 < _temp190.base?
1: _temp192 >= _temp190.last_plus_one)){ _throw( Null_Exception);}* _temp192;});}};
_temp193;}); struct _tagged_ptr0 to=({ unsigned int _temp178=( unsigned int) len;
void** _temp179=( void**) GC_malloc( sizeof( void*) * _temp178); struct
_tagged_ptr0 _temp185={ _temp179, _temp179, _temp179 + _temp178};{ unsigned int
_temp180= _temp178; unsigned int i; for( i= 0; i < _temp180; i ++){ _temp179[ i]=({
struct _tagged_ptr0 _temp182= arr; void** _temp184= _temp182.curr +( int)( i +
base_ofs); if( _temp182.base == 0? 1:( _temp184 < _temp182.base? 1: _temp184 >=
_temp182.last_plus_one)){ _throw( Null_Exception);}* _temp184;});}}; _temp185;});
struct _tagged_ptr0 swap; int stepsize; int start; int lstart; int lend; int
rstart; int rend; int dest; for( stepsize= 1; stepsize < len; stepsize= stepsize
* 2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start < limit_ofs;
start= start + stepsize * 2){ lstart= start; rstart= start + stepsize <
limit_ofs? start + stepsize: limit_ofs; lend= rstart; rend= start + stepsize * 2
< limit_ofs? start + stepsize * 2: limit_ofs; while( lstart < lend? rstart <
rend: 0) { if( less_eq(({ struct _tagged_ptr0 _temp142= from; void** _temp144=
_temp142.curr + lstart; if( _temp142.base == 0? 1:( _temp144 < _temp142.base? 1:
_temp144 >= _temp142.last_plus_one)){ _throw( Null_Exception);}* _temp144;}),({
struct _tagged_ptr0 _temp145= from; void** _temp147= _temp145.curr + rstart; if(
_temp145.base == 0? 1:( _temp147 < _temp145.base? 1: _temp147 >= _temp145.last_plus_one)){
_throw( Null_Exception);}* _temp147;})) <= 0){({ struct _tagged_ptr0 _temp148=
to; void** _temp150= _temp148.curr +( dest ++); if( _temp148.base == 0? 1:(
_temp150 < _temp148.base? 1: _temp150 >= _temp148.last_plus_one)){ _throw(
Null_Exception);}* _temp150=({ struct _tagged_ptr0 _temp151= from; void**
_temp153= _temp151.curr +( lstart ++); if( _temp151.base == 0? 1:( _temp153 <
_temp151.base? 1: _temp153 >= _temp151.last_plus_one)){ _throw( Null_Exception);}*
_temp153;});});} else{({ struct _tagged_ptr0 _temp154= to; void** _temp156=
_temp154.curr +( dest ++); if( _temp154.base == 0? 1:( _temp156 < _temp154.base?
1: _temp156 >= _temp154.last_plus_one)){ _throw( Null_Exception);}* _temp156=({
struct _tagged_ptr0 _temp157= from; void** _temp159= _temp157.curr +( rstart ++);
if( _temp157.base == 0? 1:( _temp159 < _temp157.base? 1: _temp159 >= _temp157.last_plus_one)){
_throw( Null_Exception);}* _temp159;});});}} while( lstart < lend) {({ struct
_tagged_ptr0 _temp160= to; void** _temp162= _temp160.curr +( dest ++); if(
_temp160.base == 0? 1:( _temp162 < _temp160.base? 1: _temp162 >= _temp160.last_plus_one)){
_throw( Null_Exception);}* _temp162=({ struct _tagged_ptr0 _temp163= from; void**
_temp165= _temp163.curr +( lstart ++); if( _temp163.base == 0? 1:( _temp165 <
_temp163.base? 1: _temp165 >= _temp163.last_plus_one)){ _throw( Null_Exception);}*
_temp165;});});} while( rstart < rend) {({ struct _tagged_ptr0 _temp166= to;
void** _temp168= _temp166.curr +( dest ++); if( _temp166.base == 0? 1:( _temp168
< _temp166.base? 1: _temp168 >= _temp166.last_plus_one)){ _throw( Null_Exception);}*
_temp168=({ struct _tagged_ptr0 _temp169= from; void** _temp171= _temp169.curr +(
rstart ++); if( _temp169.base == 0? 1:( _temp171 < _temp169.base? 1: _temp171 >=
_temp169.last_plus_one)){ _throw( Null_Exception);}* _temp171;});});}}}{ int i=
0; for( 0; i < len; i ++){({ struct _tagged_ptr0 _temp172= arr; void** _temp174=
_temp172.curr +( i + base_ofs); if( _temp172.base == 0? 1:( _temp174 < _temp172.base?
1: _temp174 >= _temp172.last_plus_one)){ _throw( Null_Exception);}* _temp174=({
struct _tagged_ptr0 _temp175= to; void** _temp177= _temp175.curr + i; if(
_temp175.base == 0? 1:( _temp177 < _temp175.base? 1: _temp177 >= _temp175.last_plus_one)){
_throw( Null_Exception);}* _temp177;});});}}}} struct _tagged_ptr0 Cyc_Array_from_list(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List*
x)) Cyc_List_to_array)( x);} struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_ptr0 x){ return(( struct Cyc_List_List*(*)( struct _tagged_ptr0 arr))
Cyc_List_from_array)( x);} struct _tagged_ptr0 Cyc_Array_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp202= x;( unsigned int)(
_temp202.last_plus_one - _temp202.curr);}); return({ unsigned int _temp194=(
unsigned int) sx; void** _temp195=( void**) GC_malloc( sizeof( void*) * _temp194);
struct _tagged_ptr0 _temp201={ _temp195, _temp195, _temp195 + _temp194};{
unsigned int _temp196= _temp194; unsigned int i; for( i= 0; i < _temp196; i ++){
_temp195[ i]=({ struct _tagged_ptr0 _temp198= x; void** _temp200= _temp198.curr
+( int) i; if( _temp198.base == 0? 1:( _temp200 < _temp198.base? 1: _temp200 >=
_temp198.last_plus_one)){ _throw( Null_Exception);}* _temp200;});}}; _temp201;});}
struct _tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp211= x;( unsigned int)( _temp211.last_plus_one
- _temp211.curr);}); return({ unsigned int _temp203=( unsigned int) sx; void**
_temp204=( void**) GC_malloc( sizeof( void*) * _temp203); struct _tagged_ptr0
_temp210={ _temp204, _temp204, _temp204 + _temp203};{ unsigned int _temp205=
_temp203; unsigned int i; for( i= 0; i < _temp205; i ++){ _temp204[ i]= f(({
struct _tagged_ptr0 _temp207= x; void** _temp209= _temp207.curr +( int) i; if(
_temp207.base == 0? 1:( _temp209 < _temp207.base? 1: _temp209 >= _temp207.last_plus_one)){
_throw( Null_Exception);}* _temp209;}));}}; _temp210;});} struct _tagged_ptr0
Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp220= x;( unsigned int)( _temp220.last_plus_one
- _temp220.curr);}); return({ unsigned int _temp212=( unsigned int) sx; void**
_temp213=( void**) GC_malloc( sizeof( void*) * _temp212); struct _tagged_ptr0
_temp219={ _temp213, _temp213, _temp213 + _temp212};{ unsigned int _temp214=
_temp212; unsigned int i; for( i= 0; i < _temp214; i ++){ _temp213[ i]= f( env,({
struct _tagged_ptr0 _temp216= x; void** _temp218= _temp216.curr +( int) i; if(
_temp216.base == 0? 1:( _temp218 < _temp216.base? 1: _temp218 >= _temp216.last_plus_one)){
_throw( Null_Exception);}* _temp218;}));}}; _temp219;});} void Cyc_Array_imp_map(
void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp227= x;( unsigned int)( _temp227.last_plus_one - _temp227.curr);}); int i=
0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp221= x; void** _temp223=
_temp221.curr + i; if( _temp221.base == 0? 1:( _temp223 < _temp221.base? 1:
_temp223 >= _temp221.last_plus_one)){ _throw( Null_Exception);}* _temp223= f(({
struct _tagged_ptr0 _temp224= x; void** _temp226= _temp224.curr + i; if(
_temp224.base == 0? 1:( _temp226 < _temp224.base? 1: _temp226 >= _temp224.last_plus_one)){
_throw( Null_Exception);}* _temp226;}));});}} void Cyc_Array_imp_map_c( void*(*
f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp234= x;( unsigned int)( _temp234.last_plus_one - _temp234.curr);});
int i= 0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp228= x; void**
_temp230= _temp228.curr + i; if( _temp228.base == 0? 1:( _temp230 < _temp228.base?
1: _temp230 >= _temp228.last_plus_one)){ _throw( Null_Exception);}* _temp230= f(
env,({ struct _tagged_ptr0 _temp231= x; void** _temp233= _temp231.curr + i; if(
_temp231.base == 0? 1:( _temp233 < _temp231.base? 1: _temp233 >= _temp231.last_plus_one)){
_throw( Null_Exception);}* _temp233;}));});}} char Cyc_Array_Array_mismatch[ 19u];
struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp247= x;( unsigned int)( _temp247.last_plus_one - _temp247.curr);}); if( sx
!=({ struct _tagged_ptr0 _temp235= y;( unsigned int)( _temp235.last_plus_one -
_temp235.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);} return({
unsigned int _temp236=( unsigned int) sx; void** _temp237=( void**) GC_malloc(
sizeof( void*) * _temp236); struct _tagged_ptr0 _temp246={ _temp237, _temp237,
_temp237 + _temp236};{ unsigned int _temp238= _temp236; unsigned int i; for( i=
0; i < _temp238; i ++){ _temp237[ i]= f(({ struct _tagged_ptr0 _temp240= x; void**
_temp242= _temp240.curr +( int) i; if( _temp240.base == 0? 1:( _temp242 <
_temp240.base? 1: _temp242 >= _temp240.last_plus_one)){ _throw( Null_Exception);}*
_temp242;}),({ struct _tagged_ptr0 _temp243= y; void** _temp245= _temp243.curr +(
int) i; if( _temp243.base == 0? 1:( _temp245 < _temp243.base? 1: _temp245 >=
_temp243.last_plus_one)){ _throw( Null_Exception);}* _temp245;}));}}; _temp246;});}
void Cyc_Array_app( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp251= x;( unsigned int)( _temp251.last_plus_one -
_temp251.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp248= x; void** _temp250= _temp248.curr + i; if( _temp248.base == 0? 1:(
_temp250 < _temp248.base? 1: _temp250 >= _temp248.last_plus_one)){ _throw(
Null_Exception);}* _temp250;}));}} void Cyc_Array_app_c( void*(* f)( void*, void*),
void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp255=
x;( unsigned int)( _temp255.last_plus_one - _temp255.curr);}); int i= 0; for( 0;
i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp252= x; void** _temp254=
_temp252.curr + i; if( _temp252.base == 0? 1:( _temp254 < _temp252.base? 1:
_temp254 >= _temp252.last_plus_one)){ _throw( Null_Exception);}* _temp254;}));}}
void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp259= x;( unsigned int)( _temp259.last_plus_one -
_temp259.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp256= x; void** _temp258= _temp256.curr + i; if( _temp256.base == 0? 1:(
_temp258 < _temp256.base? 1: _temp258 >= _temp256.last_plus_one)){ _throw(
Null_Exception);}* _temp258;}));}} void Cyc_Array_iter_c( void(* f)( void*, void*),
void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp263=
x;( unsigned int)( _temp263.last_plus_one - _temp263.curr);}); int i= 0; for( 0;
i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp260= x; void** _temp262=
_temp260.curr + i; if( _temp260.base == 0? 1:( _temp262 < _temp260.base? 1:
_temp262 >= _temp260.last_plus_one)){ _throw( Null_Exception);}* _temp262;}));}}
void Cyc_Array_app2( void*(* f)( void*, void*), struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp271= x;( unsigned int)(
_temp271.last_plus_one - _temp271.curr);}); if( sx !=({ struct _tagged_ptr0
_temp264= y;( unsigned int)( _temp264.last_plus_one - _temp264.curr);})){( void)
_throw(( void*) Cyc_Array_Array_mismatch);}{ int i= 0; for( 0; i < sx; i ++){ f(({
struct _tagged_ptr0 _temp265= x; void** _temp267= _temp265.curr + i; if(
_temp265.base == 0? 1:( _temp267 < _temp265.base? 1: _temp267 >= _temp265.last_plus_one)){
_throw( Null_Exception);}* _temp267;}),({ struct _tagged_ptr0 _temp268= y; void**
_temp270= _temp268.curr + i; if( _temp268.base == 0? 1:( _temp270 < _temp268.base?
1: _temp270 >= _temp268.last_plus_one)){ _throw( Null_Exception);}* _temp270;}));}}}
void Cyc_Array_app2_c( void*(* f)( void*, void*, void*), void* env, struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp279= x;( unsigned int)( _temp279.last_plus_one - _temp279.curr);}); if( sx
!=({ struct _tagged_ptr0 _temp272= y;( unsigned int)( _temp272.last_plus_one -
_temp272.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i= 0;
for( 0; i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp273= x; void**
_temp275= _temp273.curr + i; if( _temp273.base == 0? 1:( _temp275 < _temp273.base?
1: _temp275 >= _temp273.last_plus_one)){ _throw( Null_Exception);}* _temp275;}),({
struct _tagged_ptr0 _temp276= y; void** _temp278= _temp276.curr + i; if(
_temp276.base == 0? 1:( _temp278 < _temp276.base? 1: _temp278 >= _temp276.last_plus_one)){
_throw( Null_Exception);}* _temp278;}));}}} void Cyc_Array_iter2( void(* f)(
void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({
struct _tagged_ptr0 _temp287= x;( unsigned int)( _temp287.last_plus_one -
_temp287.curr);}); if( sx !=({ struct _tagged_ptr0 _temp280= y;( unsigned int)(
_temp280.last_plus_one - _temp280.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{
int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0 _temp281= x; void**
_temp283= _temp281.curr + i; if( _temp281.base == 0? 1:( _temp283 < _temp281.base?
1: _temp283 >= _temp281.last_plus_one)){ _throw( Null_Exception);}* _temp283;}),({
struct _tagged_ptr0 _temp284= y; void** _temp286= _temp284.curr + i; if(
_temp284.base == 0? 1:( _temp286 < _temp284.base? 1: _temp286 >= _temp284.last_plus_one)){
_throw( Null_Exception);}* _temp286;}));}}} void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){
int sx=( int)({ struct _tagged_ptr0 _temp295= x;( unsigned int)( _temp295.last_plus_one
- _temp295.curr);}); if( sx !=({ struct _tagged_ptr0 _temp288= y;( unsigned int)(
_temp288.last_plus_one - _temp288.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{
int i= 0; for( 0; i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp289= x; void**
_temp291= _temp289.curr + i; if( _temp289.base == 0? 1:( _temp291 < _temp289.base?
1: _temp291 >= _temp289.last_plus_one)){ _throw( Null_Exception);}* _temp291;}),({
struct _tagged_ptr0 _temp292= y; void** _temp294= _temp292.curr + i; if(
_temp292.base == 0? 1:( _temp294 < _temp292.base? 1: _temp294 >= _temp292.last_plus_one)){
_throw( Null_Exception);}* _temp294;}));}}} void* Cyc_Array_fold_left( void*(* f)(
void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp299= x;( unsigned int)( _temp299.last_plus_one - _temp299.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( accum,({ struct _tagged_ptr0 _temp296=
x; void** _temp298= _temp296.curr + i; if( _temp296.base == 0? 1:( _temp298 <
_temp296.base? 1: _temp298 >= _temp296.last_plus_one)){ _throw( Null_Exception);}*
_temp298;}));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp303= x;( unsigned int)( _temp303.last_plus_one -
_temp303.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,({
struct _tagged_ptr0 _temp300= x; void** _temp302= _temp300.curr + i; if(
_temp300.base == 0? 1:( _temp302 < _temp300.base? 1: _temp302 >= _temp300.last_plus_one)){
_throw( Null_Exception);}* _temp302;}));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({
struct _tagged_ptr0 _temp307= x;( unsigned int)( _temp307.last_plus_one -
_temp307.curr);}) - 1); for( 0; i >= 0; i --){ accum= f(({ struct _tagged_ptr0
_temp304= x; void** _temp306= _temp304.curr + i; if( _temp304.base == 0? 1:(
_temp306 < _temp304.base? 1: _temp306 >= _temp304.last_plus_one)){ _throw(
Null_Exception);}* _temp306;}), accum);}} return accum;} void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum){{
int i=( int)(({ struct _tagged_ptr0 _temp311= x;( unsigned int)( _temp311.last_plus_one
- _temp311.curr);}) - 1); for( 0; i >= 0; i --){ accum= f( env,({ struct
_tagged_ptr0 _temp308= x; void** _temp310= _temp308.curr + i; if( _temp308.base
== 0? 1:( _temp310 < _temp308.base? 1: _temp310 >= _temp308.last_plus_one)){
_throw( Null_Exception);}* _temp310;}), accum);}} return accum;} struct
_tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp320= x;( unsigned int)( _temp320.last_plus_one - _temp320.curr);});
int n= sx - 1; return({ unsigned int _temp312=( unsigned int) sx; void**
_temp313=( void**) GC_malloc( sizeof( void*) * _temp312); struct _tagged_ptr0
_temp319={ _temp313, _temp313, _temp313 + _temp312};{ unsigned int _temp314=
_temp312; unsigned int i; for( i= 0; i < _temp314; i ++){ _temp313[ i]=({ struct
_tagged_ptr0 _temp316= x; void** _temp318= _temp316.curr +( int)( n - i); if(
_temp316.base == 0? 1:( _temp318 < _temp316.base? 1: _temp318 >= _temp316.last_plus_one)){
_throw( Null_Exception);}* _temp318;});}}; _temp319;});} void Cyc_Array_imp_rev(
struct _tagged_ptr0 x){ void* temp; int i= 0; int j=( int)(({ struct
_tagged_ptr0 _temp333= x;( unsigned int)( _temp333.last_plus_one - _temp333.curr);})
- 1); while( i < j) { temp=({ struct _tagged_ptr0 _temp321= x; void** _temp323=
_temp321.curr + i; if( _temp321.base == 0? 1:( _temp323 < _temp321.base? 1:
_temp323 >= _temp321.last_plus_one)){ _throw( Null_Exception);}* _temp323;});({
struct _tagged_ptr0 _temp324= x; void** _temp326= _temp324.curr + i; if(
_temp324.base == 0? 1:( _temp326 < _temp324.base? 1: _temp326 >= _temp324.last_plus_one)){
_throw( Null_Exception);}* _temp326=({ struct _tagged_ptr0 _temp327= x; void**
_temp329= _temp327.curr + j; if( _temp327.base == 0? 1:( _temp329 < _temp327.base?
1: _temp329 >= _temp327.last_plus_one)){ _throw( Null_Exception);}* _temp329;});});({
struct _tagged_ptr0 _temp330= x; void** _temp332= _temp330.curr + j; if(
_temp330.base == 0? 1:( _temp332 < _temp330.base? 1: _temp332 >= _temp330.last_plus_one)){
_throw( Null_Exception);}* _temp332= temp;}); i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp337= x;( unsigned int)( _temp337.last_plus_one - _temp337.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( ! pred(({ struct _tagged_ptr0 _temp334= x; void**
_temp336= _temp334.curr + i; if( _temp334.base == 0? 1:( _temp336 < _temp334.base?
1: _temp336 >= _temp334.last_plus_one)){ _throw( Null_Exception);}* _temp336;}))){
return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*, void*), void*
env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp341= x;(
unsigned int)( _temp341.last_plus_one - _temp341.curr);});{ int i= 0; for( 0; i
< sx; i ++){ if( ! pred( env,({ struct _tagged_ptr0 _temp338= x; void** _temp340=
_temp338.curr + i; if( _temp338.base == 0? 1:( _temp340 < _temp338.base? 1:
_temp340 >= _temp338.last_plus_one)){ _throw( Null_Exception);}* _temp340;}))){
return 0;}}} return 1;} int Cyc_Array_exists( int(* pred)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp345= x;( unsigned int)(
_temp345.last_plus_one - _temp345.curr);});{ int i= 0; for( 0; i < sx; i ++){
if( pred(({ struct _tagged_ptr0 _temp342= x; void** _temp344= _temp342.curr + i;
if( _temp342.base == 0? 1:( _temp344 < _temp342.base? 1: _temp344 >= _temp342.last_plus_one)){
_throw( Null_Exception);}* _temp344;}))){ return 1;}}} return 0;} int Cyc_Array_exists_c(
int(* pred)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp349= x;( unsigned int)( _temp349.last_plus_one -
_temp349.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred( env,({ struct
_tagged_ptr0 _temp346= x; void** _temp348= _temp346.curr + i; if( _temp346.base
== 0? 1:( _temp348 < _temp346.base? 1: _temp348 >= _temp346.last_plus_one)){
_throw( Null_Exception);}* _temp348;}))){ return 1;}}} return 0;} struct
_tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int
sx=( int)({ struct _tagged_ptr0 _temp363= x;( unsigned int)( _temp363.last_plus_one
- _temp363.curr);}); if( sx !=({ struct _tagged_ptr0 _temp350= y;( unsigned int)(
_temp350.last_plus_one - _temp350.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}
return({ unsigned int _temp351=( unsigned int) sx; struct _tuple0* _temp352=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0) * _temp351); struct
_tagged_ptr1 _temp362={ _temp352, _temp352, _temp352 + _temp351};{ unsigned int
_temp353= _temp351; unsigned int i; for( i= 0; i < _temp353; i ++){ _temp352[ i]=({
struct _tuple0 _temp355; _temp355.f1=({ struct _tagged_ptr0 _temp359= x; void**
_temp361= _temp359.curr +( int) i; if( _temp359.base == 0? 1:( _temp361 <
_temp359.base? 1: _temp361 >= _temp359.last_plus_one)){ _throw( Null_Exception);}*
_temp361;}); _temp355.f2=({ struct _tagged_ptr0 _temp356= y; void** _temp358=
_temp356.curr +( int) i; if( _temp356.base == 0? 1:( _temp358 < _temp356.base? 1:
_temp358 >= _temp356.last_plus_one)){ _throw( Null_Exception);}* _temp358;});
_temp355;});}}; _temp362;});} struct _tuple1 Cyc_Array_split( struct
_tagged_ptr1 x){ int sx=( int)({ struct _tagged_ptr1 _temp381= x;( unsigned int)(
_temp381.last_plus_one - _temp381.curr);}); return({ struct _tuple1 _temp364;
_temp364.f1=({ unsigned int _temp373=( unsigned int) sx; void** _temp374=( void**)
GC_malloc( sizeof( void*) * _temp373); struct _tagged_ptr0 _temp380={ _temp374,
_temp374, _temp374 + _temp373};{ unsigned int _temp375= _temp373; unsigned int i;
for( i= 0; i < _temp375; i ++){ _temp374[ i]=({ struct _tagged_ptr1 _temp377= x;
struct _tuple0* _temp379= _temp377.curr +( int) i; if( _temp377.base == 0? 1:(
_temp379 < _temp377.base? 1: _temp379 >= _temp377.last_plus_one)){ _throw(
Null_Exception);}* _temp379;}).f1;}}; _temp380;}); _temp364.f2=({ unsigned int
_temp365=( unsigned int) sx; void** _temp366=( void**) GC_malloc( sizeof( void*)
* _temp365); struct _tagged_ptr0 _temp372={ _temp366, _temp366, _temp366 +
_temp365};{ unsigned int _temp367= _temp365; unsigned int i; for( i= 0; i <
_temp367; i ++){ _temp366[ i]=({ struct _tagged_ptr1 _temp369= x; struct _tuple0*
_temp371= _temp369.curr +( int) i; if( _temp369.base == 0? 1:( _temp371 <
_temp369.base? 1: _temp371 >= _temp369.last_plus_one)){ _throw( Null_Exception);}*
_temp371;}).f2;}}; _temp372;}); _temp364;});} int Cyc_Array_memq( struct
_tagged_ptr0 l, void* x){ int s=( int)({ struct _tagged_ptr0 _temp385= l;(
unsigned int)( _temp385.last_plus_one - _temp385.curr);});{ int i= 0; for( 0; i
< s; i ++){ if(({ struct _tagged_ptr0 _temp382= l; void** _temp384= _temp382.curr
+ i; if( _temp382.base == 0? 1:( _temp384 < _temp382.base? 1: _temp384 >=
_temp382.last_plus_one)){ _throw( Null_Exception);}* _temp384;}) == x){ return 1;}}}
return 0;} int Cyc_Array_mem( int(* compare)( void*, void*), struct _tagged_ptr0
l, void* x){ int s=( int)({ struct _tagged_ptr0 _temp389= l;( unsigned int)(
_temp389.last_plus_one - _temp389.curr);});{ int i= 0; for( 0; i < s; i ++){ if(
0 == compare(({ struct _tagged_ptr0 _temp386= l; void** _temp388= _temp386.curr
+ i; if( _temp386.base == 0? 1:( _temp388 < _temp386.base? 1: _temp388 >=
_temp386.last_plus_one)){ _throw( Null_Exception);}* _temp388;}), x)){ return 1;}}}
return 0;} struct _tagged_ptr0 Cyc_Array_extract( struct _tagged_ptr0 x, int
start, int* n_opt){ int sx=( int)({ struct _tagged_ptr0 _temp403= x;(
unsigned int)( _temp403.last_plus_one - _temp403.curr);}); int n= n_opt == 0? sx
- start:({ int* _temp402= n_opt; if( _temp402 == 0){ _throw( Null_Exception);}*
_temp402;}); if(( start < 0? 1: n <= 0)? 1: start +( n_opt == 0? 0: n) > sx){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp390=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp390[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp391; _temp391.tag= Cyc_Core_InvalidArg; _temp391.f1=(
struct _tagged_string)({ char* _temp392=( char*)"Array::extract"; struct
_tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 15; _temp393;}); _temp391;}); _temp390;}));}
return({ unsigned int _temp394=( unsigned int) n; void** _temp395=( void**)
GC_malloc( sizeof( void*) * _temp394); struct _tagged_ptr0 _temp401={ _temp395,
_temp395, _temp395 + _temp394};{ unsigned int _temp396= _temp394; unsigned int i;
for( i= 0; i < _temp396; i ++){ _temp395[ i]=({ struct _tagged_ptr0 _temp398= x;
void** _temp400= _temp398.curr +( int)( start + i); if( _temp398.base == 0? 1:(
_temp400 < _temp398.base? 1: _temp400 >= _temp398.last_plus_one)){ _throw(
Null_Exception);}* _temp400;});}}; _temp401;});}