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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
_temp2; _temp2.tag= Cyc_Core_InvalidArg; _temp2.f1=({ char* _temp3="Array::qsort";
struct _tagged_string _temp4; _temp4.curr= _temp3; _temp4.base= _temp3; _temp4.last_plus_one=
_temp3 + 13; _temp4;}); _temp2;}); _temp1;}));} limit_ofs= base_ofs + len;
sp_ofs= 0; for( 0; 1; 0){ if( limit_ofs - base_ofs > 3){ temp=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*),( limit_ofs - base_ofs) / 2 + base_ofs);*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*),( limit_ofs - base_ofs) / 2 + base_ofs)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), base_ofs);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), base_ofs)= temp;
i= base_ofs + 1; j= limit_ofs - 1; if( less_eq(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), i),*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j)) > 0){ temp=*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), i);*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), i)=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j);*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j)= temp;} if( less_eq(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*),
base_ofs),*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j)) > 0){ temp=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), base_ofs);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), base_ofs)=*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j);*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j)= temp;} if( less_eq(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), base_ofs)) > 0){ temp=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), i);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i)=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), base_ofs);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), base_ofs)= temp;}
for( 0; 1; 0){ do { i ++;} while ( less_eq(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), base_ofs)) < 0); do { j --;} while (
less_eq(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), base_ofs)) > 0); if( i > j){ break;} temp=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), i);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i)=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j)= temp;;} temp=*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), base_ofs);*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), base_ofs)=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j);*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j)= temp; if( j - base_ofs > limit_ofs - i){ Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs)]= base_ofs; Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs + 1)]= j; base_ofs= i;} else{ Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs)]= i; Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs + 1)]= limit_ofs; limit_ofs= j;}
sp_ofs += 2;} else{ for(( j= base_ofs, i= j + 1); i < limit_ofs;( j= i, i ++)){
for( 0; less_eq(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j + 1)) > 0; j --){ temp=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j);*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j + 1);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j + 1)= temp; if(
j == base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs= Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs)]; limit_ofs= Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs + 1)];} else{ break;}}}} void Cyc_Array_msort(
int(* less_eq)( void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){
if(( base_ofs < 0? 1: base_ofs + len >({ struct _tagged_ptr0 _temp5= arr;(
unsigned int)( _temp5.last_plus_one - _temp5.curr);}))? 1: len < 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp6=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp6[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp7; _temp7.tag= Cyc_Core_InvalidArg; _temp7.f1=({ char* _temp8="Array::msort";
struct _tagged_string _temp9; _temp9.curr= _temp8; _temp9.base= _temp8; _temp9.last_plus_one=
_temp8 + 13; _temp9;}); _temp7;}); _temp6;}));}{ int limit_ofs= base_ofs + len;
struct _tagged_ptr0 from=({ unsigned int _temp14=( unsigned int) len; void**
_temp15=( void**) GC_malloc( sizeof( void*) * _temp14); struct _tagged_ptr0
_temp17={ _temp15, _temp15, _temp15 + _temp14};{ unsigned int _temp16= _temp14;
unsigned int i; for( i= 0; i < _temp16; i ++){ _temp15[ i]=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), 0);}}; _temp17;}); struct _tagged_ptr0 to=({ unsigned int
_temp10=( unsigned int) len; void** _temp11=( void**) GC_malloc( sizeof( void*)
* _temp10); struct _tagged_ptr0 _temp13={ _temp11, _temp11, _temp11 + _temp10};{
unsigned int _temp12= _temp10; unsigned int i; for( i= 0; i < _temp12; i ++){
_temp11[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*),( int)( i + base_ofs));}};
_temp13;}); struct _tagged_ptr0 swap; int stepsize; int start; int lstart; int
lend; int rstart; int rend; int dest; for( stepsize= 1; stepsize < len; stepsize=
stepsize * 2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start <
limit_ofs; start= start + stepsize * 2){ lstart= start; rstart= start + stepsize
< limit_ofs? start + stepsize: limit_ofs; lend= rstart; rend= start + stepsize *
2 < limit_ofs? start + stepsize * 2: limit_ofs; while( lstart < lend? rstart <
rend: 0) { if( less_eq(*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( from, sizeof( void*), lstart),*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
from, sizeof( void*), rstart)) <= 0){*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( to, sizeof( void*), dest
++)=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( from, sizeof( void*), lstart ++);} else{*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( to,
sizeof( void*), dest ++)=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( from, sizeof( void*), rstart ++);}}
while( lstart < lend) {*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( to, sizeof( void*), dest ++)=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
from, sizeof( void*), lstart ++);} while( rstart < rend) {*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( to,
sizeof( void*), dest ++)=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( from, sizeof( void*), rstart ++);}}}{
int i= 0; for( 0; i < len; i ++){*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i +
base_ofs)=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( to, sizeof( void*), i);}}}} struct _tagged_ptr0 Cyc_Array_from_list(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List*
x)) Cyc_List_to_array)( x);} struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_ptr0 x){ return(( struct Cyc_List_List*(*)( struct _tagged_ptr0 arr))
Cyc_List_from_array)( x);} struct _tagged_ptr0 Cyc_Array_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp22= x;( unsigned int)(
_temp22.last_plus_one - _temp22.curr);}); return({ unsigned int _temp18=(
unsigned int) sx; void** _temp19=( void**) GC_malloc( sizeof( void*) * _temp18);
struct _tagged_ptr0 _temp21={ _temp19, _temp19, _temp19 + _temp18};{
unsigned int _temp20= _temp18; unsigned int i; for( i= 0; i < _temp20; i ++){
_temp19[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*),( int) i);}}; _temp21;});} struct
_tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=(
int)({ struct _tagged_ptr0 _temp27= x;( unsigned int)( _temp27.last_plus_one -
_temp27.curr);}); return({ unsigned int _temp23=( unsigned int) sx; void**
_temp24=( void**) GC_malloc( sizeof( void*) * _temp23); struct _tagged_ptr0
_temp26={ _temp24, _temp24, _temp24 + _temp23};{ unsigned int _temp25= _temp23;
unsigned int i; for( i= 0; i < _temp25; i ++){ _temp24[ i]= f(*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*),( int) i));}}; _temp26;});} struct _tagged_ptr0 Cyc_Array_map_c(
void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp32= x;( unsigned int)( _temp32.last_plus_one - _temp32.curr);});
return({ unsigned int _temp28=( unsigned int) sx; void** _temp29=( void**)
GC_malloc( sizeof( void*) * _temp28); struct _tagged_ptr0 _temp31={ _temp29,
_temp29, _temp29 + _temp28};{ unsigned int _temp30= _temp28; unsigned int i;
for( i= 0; i < _temp30; i ++){ _temp29[ i]= f( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*),( int) i));}}; _temp31;});} void Cyc_Array_imp_map( void*(* f)( void*),
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp33= x;(
unsigned int)( _temp33.last_plus_one - _temp33.curr);}); int i= 0; for( 0; i <
sx; i ++){*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)= f(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} void Cyc_Array_imp_map_c( void*(* f)( void*, void*), void* env,
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp34= x;(
unsigned int)( _temp34.last_plus_one - _temp34.curr);}); int i= 0; for( 0; i <
sx; i ++){*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)= f( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} char Cyc_Array_Array_mismatch[ 19u]="\000\000\000\000Array_mismatch";
struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp40= x;( unsigned int)( _temp40.last_plus_one - _temp40.curr);}); if( sx !=({
struct _tagged_ptr0 _temp35= y;( unsigned int)( _temp35.last_plus_one - _temp35.curr);})){(
void) _throw(( void*) Cyc_Array_Array_mismatch);} return({ unsigned int _temp36=(
unsigned int) sx; void** _temp37=( void**) GC_malloc( sizeof( void*) * _temp36);
struct _tagged_ptr0 _temp39={ _temp37, _temp37, _temp37 + _temp36};{
unsigned int _temp38= _temp36; unsigned int i; for( i= 0; i < _temp38; i ++){
_temp37[ i]= f(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*),( int) i),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( y, sizeof(
void*),( int) i));}}; _temp39;});} void Cyc_Array_app( void*(* f)( void*),
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp41= x;(
unsigned int)( _temp41.last_plus_one - _temp41.curr);}); int i= 0; for( 0; i <
sx; i ++){ f(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i));}} void Cyc_Array_app_c( void*(*
f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp42= x;( unsigned int)( _temp42.last_plus_one - _temp42.curr);});
int i= 0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i));}}
void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp43= x;( unsigned int)( _temp43.last_plus_one - _temp43.curr);});
int i= 0; for( 0; i < sx; i ++){ f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i));}}
void Cyc_Array_iter_c( void(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp44= x;( unsigned int)( _temp44.last_plus_one
- _temp44.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*), i));}} void Cyc_Array_app2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp46= x;( unsigned int)( _temp46.last_plus_one - _temp46.curr);}); if( sx !=({
struct _tagged_ptr0 _temp45= y;( unsigned int)( _temp45.last_plus_one - _temp45.curr);})){(
void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i= 0; for( 0; i < sx; i
++){ f(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( y, sizeof(
void*), i));}}} void Cyc_Array_app2_c( void*(* f)( void*, void*, void*), void*
env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp48= x;( unsigned int)( _temp48.last_plus_one - _temp48.curr);});
if( sx !=({ struct _tagged_ptr0 _temp47= y;( unsigned int)( _temp47.last_plus_one
- _temp47.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( y, sizeof( void*), i));}}} void Cyc_Array_iter2( void(*
f)( void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({
struct _tagged_ptr0 _temp50= x;( unsigned int)( _temp50.last_plus_one - _temp50.curr);});
if( sx !=({ struct _tagged_ptr0 _temp49= y;( unsigned int)( _temp49.last_plus_one
- _temp49.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f(*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( y,
sizeof( void*), i));}}} void Cyc_Array_iter2_c( void(* f)( void*, void*, void*),
void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp52= x;( unsigned int)( _temp52.last_plus_one - _temp52.curr);});
if( sx !=({ struct _tagged_ptr0 _temp51= y;( unsigned int)( _temp51.last_plus_one
- _temp51.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( y, sizeof( void*), i));}}} void* Cyc_Array_fold_left(
void*(* f)( void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp53= x;( unsigned int)( _temp53.last_plus_one - _temp53.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( accum,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp54= x;( unsigned int)( _temp54.last_plus_one - _temp54.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} return accum;} void* Cyc_Array_fold_right( void*(* f)( void*,
void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({ struct
_tagged_ptr0 _temp55= x;( unsigned int)( _temp55.last_plus_one - _temp55.curr);})
- 1); for( 0; i >= 0; i --){ accum= f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),
accum);}} return accum;} void* Cyc_Array_fold_right_c( void*(* f)( void*, void*,
void*), void* env, struct _tagged_ptr0 x, void* accum){{ int i=( int)(({ struct
_tagged_ptr0 _temp56= x;( unsigned int)( _temp56.last_plus_one - _temp56.curr);})
- 1); for( 0; i >= 0; i --){ accum= f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),
accum);}} return accum;} struct _tagged_ptr0 Cyc_Array_rev_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp61= x;( unsigned int)(
_temp61.last_plus_one - _temp61.curr);}); int n= sx - 1; return({ unsigned int
_temp57=( unsigned int) sx; void** _temp58=( void**) GC_malloc( sizeof( void*) *
_temp57); struct _tagged_ptr0 _temp60={ _temp58, _temp58, _temp58 + _temp57};{
unsigned int _temp59= _temp57; unsigned int i; for( i= 0; i < _temp59; i ++){
_temp58[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*),( int)( n - i));}}; _temp60;});}
void Cyc_Array_imp_rev( struct _tagged_ptr0 x){ void* temp; int i= 0; int j=(
int)(({ struct _tagged_ptr0 _temp62= x;( unsigned int)( _temp62.last_plus_one -
_temp62.curr);}) - 1); while( i < j) { temp=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i);*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), j);*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), j)= temp; i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp63= x;( unsigned int)( _temp63.last_plus_one - _temp63.curr);});{ int i= 0;
for( 0; i < sx; i ++){ if( ! pred(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i))){
return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*, void*), void*
env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp64= x;(
unsigned int)( _temp64.last_plus_one - _temp64.curr);});{ int i= 0; for( 0; i <
sx; i ++){ if( ! pred( env,*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i))){ return 0;}}}
return 1;} int Cyc_Array_exists( int(* pred)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp65= x;( unsigned int)( _temp65.last_plus_one
- _temp65.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred(*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*), i))){ return 1;}}} return 0;} int Cyc_Array_exists_c( int(* pred)(
void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp66= x;( unsigned int)( _temp66.last_plus_one - _temp66.curr);});{
int i= 0; for( 0; i < sx; i ++){ if( pred( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i))){ return 1;}}} return 0;} struct _tagged_ptr1 Cyc_Array_zip( struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp73= x;( unsigned int)( _temp73.last_plus_one - _temp73.curr);}); if( sx !=({
struct _tagged_ptr0 _temp67= y;( unsigned int)( _temp67.last_plus_one - _temp67.curr);})){(
void) _throw(( void*) Cyc_Array_Array_mismatch);} return({ unsigned int _temp68=(
unsigned int) sx; struct _tuple0* _temp69=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * _temp68); struct _tagged_ptr1 _temp72={ _temp69, _temp69,
_temp69 + _temp68};{ unsigned int _temp70= _temp68; unsigned int i; for( i= 0; i
< _temp70; i ++){ _temp69[ i]=({ struct _tuple0 _temp71; _temp71.f1=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*),( int) i); _temp71.f2=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( y, sizeof( void*),( int)
i); _temp71;});}}; _temp72;});} struct _tuple1 Cyc_Array_split( struct
_tagged_ptr1 x){ int sx=( int)({ struct _tagged_ptr1 _temp83= x;( unsigned int)(
_temp83.last_plus_one - _temp83.curr);}); return({ struct _tuple1 _temp74;
_temp74.f1=({ unsigned int _temp79=( unsigned int) sx; void** _temp80=( void**)
GC_malloc( sizeof( void*) * _temp79); struct _tagged_ptr0 _temp82={ _temp80,
_temp80, _temp80 + _temp79};{ unsigned int _temp81= _temp79; unsigned int i;
for( i= 0; i < _temp81; i ++){ _temp80[ i]=(*(( struct _tuple0*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
struct _tuple0),( int) i)).f1;}}; _temp82;}); _temp74.f2=({ unsigned int _temp75=(
unsigned int) sx; void** _temp76=( void**) GC_malloc( sizeof( void*) * _temp75);
struct _tagged_ptr0 _temp78={ _temp76, _temp76, _temp76 + _temp75};{
unsigned int _temp77= _temp75; unsigned int i; for( i= 0; i < _temp77; i ++){
_temp76[ i]=(*(( struct _tuple0*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( struct _tuple0),( int) i)).f2;}};
_temp78;}); _temp74;});} int Cyc_Array_memq( struct _tagged_ptr0 l, void* x){
int s=( int)({ struct _tagged_ptr0 _temp84= l;( unsigned int)( _temp84.last_plus_one
- _temp84.curr);});{ int i= 0; for( 0; i < s; i ++){ if(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( l, sizeof(
void*), i) == x){ return 1;}}} return 0;} int Cyc_Array_mem( int(* compare)(
void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({ struct
_tagged_ptr0 _temp85= l;( unsigned int)( _temp85.last_plus_one - _temp85.curr);});{
int i= 0; for( 0; i < s; i ++){ if( 0 == compare(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( l, sizeof(
void*), i), x)){ return 1;}}} return 0;} struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt){ int sx=( int)({ struct
_tagged_ptr0 _temp94= x;( unsigned int)( _temp94.last_plus_one - _temp94.curr);});
int n= n_opt == 0? sx - start:*(( int*) _check_null( n_opt)); if(( start < 0? 1:
n <= 0)? 1: start +( n_opt == 0? 0: n) > sx){( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp86=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp86[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp87; _temp87.tag= Cyc_Core_InvalidArg; _temp87.f1=({
char* _temp88="Array::extract"; struct _tagged_string _temp89; _temp89.curr=
_temp88; _temp89.base= _temp88; _temp89.last_plus_one= _temp88 + 15; _temp89;});
_temp87;}); _temp86;}));} return({ unsigned int _temp90=( unsigned int) n; void**
_temp91=( void**) GC_malloc( sizeof( void*) * _temp90); struct _tagged_ptr0
_temp93={ _temp91, _temp91, _temp91 + _temp90};{ unsigned int _temp92= _temp90;
unsigned int i; for( i= 0; i < _temp92; i ++){ _temp91[ i]=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*),( int)( start + i));}}; _temp93;});}