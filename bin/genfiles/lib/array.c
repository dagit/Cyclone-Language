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
_temp2; _temp2.tag= Cyc_Core_InvalidArg; _temp2.f1=( struct _tagged_string)({
char* _temp3=( char*)"Array::qsort"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 13; _temp4;});
_temp2;}); _temp1;}));} limit_ofs= base_ofs + len; sp_ofs= 0; for( 0; 1; 0){ if(
limit_ofs - base_ofs > 3){ temp=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*),( limit_ofs -
base_ofs) / 2 + base_ofs);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*),( limit_ofs -
base_ofs) / 2 + base_ofs)=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), base_ofs);*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), base_ofs)= temp; i= base_ofs + 1;
j= limit_ofs - 1; if( less_eq(*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i),*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j)) > 0){ temp=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i);*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
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
sizeof( void*), j)= temp; if( j - base_ofs > limit_ofs - i){(( int*) Cyc_Array_sp)[
_check_known_subscript_notnull( 40u, sp_ofs)]= base_ofs;(( int*) Cyc_Array_sp)[
_check_known_subscript_notnull( 40u, sp_ofs + 1)]= j; base_ofs= i;} else{(( int*)
Cyc_Array_sp)[ _check_known_subscript_notnull( 40u, sp_ofs)]= i;(( int*) Cyc_Array_sp)[
_check_known_subscript_notnull( 40u, sp_ofs + 1)]= limit_ofs; limit_ofs= j;}
sp_ofs += 2;} else{ for( j= base_ofs, i= j + 1; i < limit_ofs; j= i, i ++){ for(
0; less_eq(*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j + 1)) > 0; j --){ temp=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j);*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), j)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*), j + 1);*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), j + 1)= temp; if(
j == base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs=(( int*) Cyc_Array_sp)[
_check_known_subscript_notnull( 40u, sp_ofs)]; limit_ofs=(( int*) Cyc_Array_sp)[
_check_known_subscript_notnull( 40u, sp_ofs + 1)];} else{ break;}}}} void Cyc_Array_msort(
int(* less_eq)( void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){
if(( base_ofs < 0? 1: base_ofs + len >({ struct _tagged_ptr0 _temp5= arr;(
unsigned int)( _temp5.last_plus_one - _temp5.curr);}))? 1: len < 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp6=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp6[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp7; _temp7.tag= Cyc_Core_InvalidArg; _temp7.f1=( struct _tagged_string)({
char* _temp8=( char*)"Array::msort"; struct _tagged_string _temp9; _temp9.curr=
_temp8; _temp9.base= _temp8; _temp9.last_plus_one= _temp8 + 13; _temp9;});
_temp7;}); _temp6;}));}{ int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({
unsigned int _temp15=( unsigned int) len; void** _temp16=( void**) GC_malloc(
sizeof( void*) * _temp15); struct _tagged_ptr0 _temp19={ _temp16, _temp16,
_temp16 + _temp15};{ unsigned int _temp17= _temp15; unsigned int i; for( i= 0; i
< _temp17; i ++){ _temp16[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), 0);}}; _temp19;});
struct _tagged_ptr0 to=({ unsigned int _temp10=( unsigned int) len; void**
_temp11=( void**) GC_malloc( sizeof( void*) * _temp10); struct _tagged_ptr0
_temp14={ _temp11, _temp11, _temp11 + _temp10};{ unsigned int _temp12= _temp10;
unsigned int i; for( i= 0; i < _temp12; i ++){ _temp11[ i]=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*),( int)( i + base_ofs));}}; _temp14;}); struct _tagged_ptr0 swap;
int stepsize; int start; int lstart; int lend; int rstart; int rend; int dest;
for( stepsize= 1; stepsize < len; stepsize= stepsize * 2){ swap= from; from= to;
to= swap; dest= 0; for( start= 0; start < limit_ofs; start= start + stepsize * 2){
lstart= start; rstart= start + stepsize < limit_ofs? start + stepsize: limit_ofs;
lend= rstart; rend= start + stepsize * 2 < limit_ofs? start + stepsize * 2:
limit_ofs; while( lstart < lend? rstart < rend: 0) { if( less_eq(*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
from, sizeof( void*), lstart),*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( from, sizeof( void*), rstart)) <= 0){*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( to, sizeof( void*),( dest ++))=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( from,
sizeof( void*),( lstart ++));} else{*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( to, sizeof( void*),(
dest ++))=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( from, sizeof( void*),( rstart ++));}} while( lstart <
lend) {*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( to, sizeof( void*),( dest ++))=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( from,
sizeof( void*),( lstart ++));} while( rstart < rend) {*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( to,
sizeof( void*),( dest ++))=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( from, sizeof( void*),( rstart ++));}}}{
int i= 0; for( 0; i < len; i ++){*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( arr, sizeof( void*), i +
base_ofs)=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( to, sizeof( void*), i);}}}} struct _tagged_ptr0 Cyc_Array_from_list(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List*
x)) Cyc_List_to_array)( x);} struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_ptr0 x){ return(( struct Cyc_List_List*(*)( struct _tagged_ptr0 arr))
Cyc_List_from_array)( x);} struct _tagged_ptr0 Cyc_Array_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp25= x;( unsigned int)(
_temp25.last_plus_one - _temp25.curr);}); return({ unsigned int _temp20=(
unsigned int) sx; void** _temp21=( void**) GC_malloc( sizeof( void*) * _temp20);
struct _tagged_ptr0 _temp24={ _temp21, _temp21, _temp21 + _temp20};{
unsigned int _temp22= _temp20; unsigned int i; for( i= 0; i < _temp22; i ++){
_temp21[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*),( int) i);}}; _temp24;});} struct
_tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=(
int)({ struct _tagged_ptr0 _temp31= x;( unsigned int)( _temp31.last_plus_one -
_temp31.curr);}); return({ unsigned int _temp26=( unsigned int) sx; void**
_temp27=( void**) GC_malloc( sizeof( void*) * _temp26); struct _tagged_ptr0
_temp30={ _temp27, _temp27, _temp27 + _temp26};{ unsigned int _temp28= _temp26;
unsigned int i; for( i= 0; i < _temp28; i ++){ _temp27[ i]= f(*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*),( int) i));}}; _temp30;});} struct _tagged_ptr0 Cyc_Array_map_c(
void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp37= x;( unsigned int)( _temp37.last_plus_one - _temp37.curr);});
return({ unsigned int _temp32=( unsigned int) sx; void** _temp33=( void**)
GC_malloc( sizeof( void*) * _temp32); struct _tagged_ptr0 _temp36={ _temp33,
_temp33, _temp33 + _temp32};{ unsigned int _temp34= _temp32; unsigned int i;
for( i= 0; i < _temp34; i ++){ _temp33[ i]= f( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*),( int) i));}}; _temp36;});} void Cyc_Array_imp_map( void*(* f)( void*),
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp38= x;(
unsigned int)( _temp38.last_plus_one - _temp38.curr);}); int i= 0; for( 0; i <
sx; i ++){*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)= f(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} void Cyc_Array_imp_map_c( void*(* f)( void*, void*), void* env,
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp39= x;(
unsigned int)( _temp39.last_plus_one - _temp39.curr);}); int i= 0; for( 0; i <
sx; i ++){*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)= f( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} char Cyc_Array_Array_mismatch[ 19u]; struct _tagged_ptr0 Cyc_Array_map2(
void*(* f)( void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=(
int)({ struct _tagged_ptr0 _temp46= x;( unsigned int)( _temp46.last_plus_one -
_temp46.curr);}); if( sx !=({ struct _tagged_ptr0 _temp40= y;( unsigned int)(
_temp40.last_plus_one - _temp40.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}
return({ unsigned int _temp41=( unsigned int) sx; void** _temp42=( void**)
GC_malloc( sizeof( void*) * _temp41); struct _tagged_ptr0 _temp45={ _temp42,
_temp42, _temp42 + _temp41};{ unsigned int _temp43= _temp41; unsigned int i;
for( i= 0; i < _temp43; i ++){ _temp42[ i]= f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*),( int)
i),*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( y, sizeof( void*),( int) i));}}; _temp45;});} void
Cyc_Array_app( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp47= x;( unsigned int)( _temp47.last_plus_one - _temp47.curr);});
int i= 0; for( 0; i < sx; i ++){ f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i));}}
void Cyc_Array_app_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp48= x;( unsigned int)( _temp48.last_plus_one
- _temp48.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*), i));}} void Cyc_Array_iter( void(* f)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp49= x;( unsigned int)(
_temp49.last_plus_one - _temp49.curr);}); int i= 0; for( 0; i < sx; i ++){ f(*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i));}} void Cyc_Array_iter_c( void(*
f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp50= x;( unsigned int)( _temp50.last_plus_one - _temp50.curr);});
int i= 0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i));}}
void Cyc_Array_app2( void*(* f)( void*, void*), struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp52= x;( unsigned int)(
_temp52.last_plus_one - _temp52.curr);}); if( sx !=({ struct _tagged_ptr0
_temp51= y;( unsigned int)( _temp51.last_plus_one - _temp51.curr);})){( void)
_throw(( void*) Cyc_Array_Array_mismatch);}{ int i= 0; for( 0; i < sx; i ++){ f(*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i),*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( y, sizeof(
void*), i));}}} void Cyc_Array_app2_c( void*(* f)( void*, void*, void*), void*
env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp54= x;( unsigned int)( _temp54.last_plus_one - _temp54.curr);});
if( sx !=({ struct _tagged_ptr0 _temp53= y;( unsigned int)( _temp53.last_plus_one
- _temp53.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( y, sizeof( void*), i));}}} void Cyc_Array_iter2( void(*
f)( void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({
struct _tagged_ptr0 _temp56= x;( unsigned int)( _temp56.last_plus_one - _temp56.curr);});
if( sx !=({ struct _tagged_ptr0 _temp55= y;( unsigned int)( _temp55.last_plus_one
- _temp55.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f(*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( y,
sizeof( void*), i));}}} void Cyc_Array_iter2_c( void(* f)( void*, void*, void*),
void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp58= x;( unsigned int)( _temp58.last_plus_one - _temp58.curr);});
if( sx !=({ struct _tagged_ptr0 _temp57= y;( unsigned int)( _temp57.last_plus_one
- _temp57.curr);})){( void) _throw(( void*) Cyc_Array_Array_mismatch);}{ int i=
0; for( 0; i < sx; i ++){ f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( y, sizeof( void*), i));}}} void* Cyc_Array_fold_left(
void*(* f)( void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp59= x;( unsigned int)( _temp59.last_plus_one - _temp59.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( accum,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp60= x;( unsigned int)( _temp60.last_plus_one - _temp60.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i));}} return accum;} void* Cyc_Array_fold_right( void*(* f)( void*,
void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({ struct
_tagged_ptr0 _temp61= x;( unsigned int)( _temp61.last_plus_one - _temp61.curr);})
- 1); for( 0; i >= 0; i --){ accum= f(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),
accum);}} return accum;} void* Cyc_Array_fold_right_c( void*(* f)( void*, void*,
void*), void* env, struct _tagged_ptr0 x, void* accum){{ int i=( int)(({ struct
_tagged_ptr0 _temp62= x;( unsigned int)( _temp62.last_plus_one - _temp62.curr);})
- 1); for( 0; i >= 0; i --){ accum= f( env,*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i),
accum);}} return accum;} struct _tagged_ptr0 Cyc_Array_rev_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp68= x;( unsigned int)(
_temp68.last_plus_one - _temp68.curr);}); int n= sx - 1; return({ unsigned int
_temp63=( unsigned int) sx; void** _temp64=( void**) GC_malloc( sizeof( void*) *
_temp63); struct _tagged_ptr0 _temp67={ _temp64, _temp64, _temp64 + _temp63};{
unsigned int _temp65= _temp63; unsigned int i; for( i= 0; i < _temp65; i ++){
_temp64[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*),( int)( n - i));}}; _temp67;});}
void Cyc_Array_imp_rev( struct _tagged_ptr0 x){ void* temp; int i= 0; int j=(
int)(({ struct _tagged_ptr0 _temp69= x;( unsigned int)( _temp69.last_plus_one -
_temp69.curr);}) - 1); while( i < j) { temp=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i);*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), i)=*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), j);*(( void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( x, sizeof( void*), j)= temp; i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp70= x;( unsigned int)( _temp70.last_plus_one - _temp70.curr);});{ int i= 0;
for( 0; i < sx; i ++){ if( ! pred(*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i))){
return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*, void*), void*
env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp71= x;(
unsigned int)( _temp71.last_plus_one - _temp71.curr);});{ int i= 0; for( 0; i <
sx; i ++){ if( ! pred( env,*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( void*), i))){ return 0;}}}
return 1;} int Cyc_Array_exists( int(* pred)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp72= x;( unsigned int)( _temp72.last_plus_one
- _temp72.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred(*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*), i))){ return 1;}}} return 0;} int Cyc_Array_exists_c( int(* pred)(
void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp73= x;( unsigned int)( _temp73.last_plus_one - _temp73.curr);});{
int i= 0; for( 0; i < sx; i ++){ if( pred( env,*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
void*), i))){ return 1;}}} return 0;} struct _tagged_ptr1 Cyc_Array_zip( struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp81= x;( unsigned int)( _temp81.last_plus_one - _temp81.curr);}); if( sx !=({
struct _tagged_ptr0 _temp74= y;( unsigned int)( _temp74.last_plus_one - _temp74.curr);})){(
void) _throw(( void*) Cyc_Array_Array_mismatch);} return({ unsigned int _temp75=(
unsigned int) sx; struct _tuple0* _temp76=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * _temp75); struct _tagged_ptr1 _temp80={ _temp76, _temp76,
_temp76 + _temp75};{ unsigned int _temp77= _temp75; unsigned int i; for( i= 0; i
< _temp77; i ++){ _temp76[ i]=({ struct _tuple0 _temp79; _temp79.f1=*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( x,
sizeof( void*),( int) i); _temp79.f2=*(( void**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( y, sizeof( void*),( int)
i); _temp79;});}}; _temp80;});} struct _tuple1 Cyc_Array_split( struct
_tagged_ptr1 x){ int sx=( int)({ struct _tagged_ptr1 _temp93= x;( unsigned int)(
_temp93.last_plus_one - _temp93.curr);}); return({ struct _tuple1 _temp82;
_temp82.f1=({ unsigned int _temp88=( unsigned int) sx; void** _temp89=( void**)
GC_malloc( sizeof( void*) * _temp88); struct _tagged_ptr0 _temp92={ _temp89,
_temp89, _temp89 + _temp88};{ unsigned int _temp90= _temp88; unsigned int i;
for( i= 0; i < _temp90; i ++){ _temp89[ i]=(*(( struct _tuple0*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( x, sizeof(
struct _tuple0),( int) i)).f1;}}; _temp92;}); _temp82.f2=({ unsigned int _temp83=(
unsigned int) sx; void** _temp84=( void**) GC_malloc( sizeof( void*) * _temp83);
struct _tagged_ptr0 _temp87={ _temp84, _temp84, _temp84 + _temp83};{
unsigned int _temp85= _temp83; unsigned int i; for( i= 0; i < _temp85; i ++){
_temp84[ i]=(*(( struct _tuple0*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( struct _tuple0),( int) i)).f2;}};
_temp87;}); _temp82;});} int Cyc_Array_memq( struct _tagged_ptr0 l, void* x){
int s=( int)({ struct _tagged_ptr0 _temp94= l;( unsigned int)( _temp94.last_plus_one
- _temp94.curr);});{ int i= 0; for( 0; i < s; i ++){ if(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( l, sizeof(
void*), i) == x){ return 1;}}} return 0;} int Cyc_Array_mem( int(* compare)(
void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({ struct
_tagged_ptr0 _temp95= l;( unsigned int)( _temp95.last_plus_one - _temp95.curr);});{
int i= 0; for( 0; i < s; i ++){ if( 0 == compare(*(( void**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( l, sizeof(
void*), i), x)){ return 1;}}} return 0;} struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt){ int sx=( int)({ struct
_tagged_ptr0 _temp105= x;( unsigned int)( _temp105.last_plus_one - _temp105.curr);});
int n= n_opt == 0? sx - start:*(( int*) _check_null( n_opt)); if(( start < 0? 1:
n <= 0)? 1: start +( n_opt == 0? 0: n) > sx){( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp96=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp96[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp97; _temp97.tag= Cyc_Core_InvalidArg; _temp97.f1=(
struct _tagged_string)({ char* _temp98=( char*)"Array::extract"; struct
_tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 15; _temp99;}); _temp97;}); _temp96;}));} return({ unsigned int
_temp100=( unsigned int) n; void** _temp101=( void**) GC_malloc( sizeof( void*)
* _temp100); struct _tagged_ptr0 _temp104={ _temp101, _temp101, _temp101 +
_temp100};{ unsigned int _temp102= _temp100; unsigned int i; for( i= 0; i <
_temp102; i ++){ _temp101[ i]=*(( void**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( x, sizeof( void*),( int)( start + i));}};
_temp104;});}