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
tag; struct _tagged_string f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty[
15u]; extern void* Cyc_List_hd( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_copy(
struct Cyc_List_List* src); extern struct Cyc_List_List* Cyc_List_rcopy( struct
_RegionHandle*, struct Cyc_List_List* src); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_map2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rmap2( struct _RegionHandle*, void*(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern void
Cyc_List_app( void*(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_app_c(
void*(* f)( void*, void*), void*, struct Cyc_List_List* x); extern void Cyc_List_app2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y);
extern void Cyc_List_app2_c( void*(* f)( void*, void*, void*), void* env, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern void Cyc_List_iter( void(* f)(
void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)( void*,
void*), void* env, struct Cyc_List_List* x); extern void Cyc_List_iter2( void(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern
void Cyc_List_iter2_c( void(* f)( void*, void*, void*), void* env, struct Cyc_List_List*
x, struct Cyc_List_List* y); extern void* Cyc_List_fold_left( void*(* f)( void*,
void*), void* accum, struct Cyc_List_List* x); extern void* Cyc_List_fold_left_c(
void*(* f)( void*, void*, void*), void*, void* accum, struct Cyc_List_List* x);
extern void* Cyc_List_fold_right( void*(* f)( void*, void*), struct Cyc_List_List*
x, void* accum); extern void* Cyc_List_fold_right_c( void*(* f)( void*, void*,
void*), void*, struct Cyc_List_List* x, void* accum); extern struct Cyc_List_List*
Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_rrevappend( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rrev( struct
_RegionHandle*, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_rflatten( struct _RegionHandle*, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_merge_sort( int(*
less_eq)( void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rmerge_sort( struct _RegionHandle*, int(* less_eq)( void*, void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rimp_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b); extern struct Cyc_List_List* Cyc_List_rmerge( struct
_RegionHandle*, int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b); extern struct Cyc_List_List* Cyc_List_imp_merge( int(*
less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b);
extern char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_List_forall_c( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x); extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x); extern int Cyc_List_exists_c( int(* pred)( void*, void*), void* env, struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple0 Cyc_List_split( struct Cyc_List_List* x); struct _tuple1{
struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; extern struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x); extern struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x); extern struct _tuple1 Cyc_List_rsplit3( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct _RegionHandle* r5, struct
Cyc_List_List* x); extern int Cyc_List_memq( struct Cyc_List_List* l, void* x);
extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern void* Cyc_List_assoc( struct Cyc_List_List* l, void* x); extern
void* Cyc_List_assoc_cmp( int(* compare)( void*, void*), struct Cyc_List_List* l,
void* x); extern int Cyc_List_mem_assoc( struct Cyc_List_List* l, void* x);
extern struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*, void*),
struct Cyc_List_List* x); struct _tagged_ptr0{ void** curr; void** base; void**
last_plus_one; } ; extern struct _tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List*
x); extern struct _tagged_ptr0 Cyc_List_rto_array( struct _RegionHandle* r2,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr); extern struct Cyc_List_List* Cyc_List_rfrom_array(
struct _RegionHandle* r2, struct _tagged_ptr0 arr); extern struct Cyc_List_List*
Cyc_List_tabulate( int n, void*(* f)( int)); extern struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env); extern struct Cyc_List_List* Cyc_List_rtabulate(
struct _RegionHandle* r, int n, void*(* f)( int)); extern struct Cyc_List_List*
Cyc_List_rtabulate_c( struct _RegionHandle* r, int n, void*(* f)( void*, int),
void* env); extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); extern int Cyc_List_list_prefix( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern struct Cyc_List_List*
Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List* l);
extern struct Cyc_List_List* Cyc_List_rfilter_c( struct _RegionHandle* r2, int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); extern struct Cyc_List_List*
Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l); extern struct Cyc_List_List*
Cyc_List_rfilter( struct _RegionHandle* r2, int(* f)( void*), struct Cyc_List_List*
l); int Cyc_List_length( struct Cyc_List_List* x){ int i= 0; while( x != 0) { ++
i; x=(( struct Cyc_List_List*) _check_null( x))->tl;} return i;} char Cyc_List_List_empty[
15u]="\000\000\000\000List_empty"; void* Cyc_List_hd( struct Cyc_List_List* x){
if( x == 0){( void) _throw(( void*) Cyc_List_List_empty);} return( void*)((
struct Cyc_List_List*) _check_null( x))->hd;} struct Cyc_List_List* Cyc_List_tl(
struct Cyc_List_List* x){ if( x == 0){( void) _throw(( void*) Cyc_List_List_empty);}
return(( struct Cyc_List_List*) _check_null( x))->tl;} struct Cyc_List_List* Cyc_List_rcopy(
struct _RegionHandle* r2, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp0=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp0->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
_temp0->tl= 0; _temp0;}); prev= result; for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp1=(
struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp1->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
_temp1->tl= 0; _temp1;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;}
return result;} struct Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* x){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle* r2, void*(* f)( void*), struct Cyc_List_List* x){ struct
Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp2=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp2->hd=( void*) f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); _temp2->tl= 0; _temp2;}); prev= result; for( x=(( struct
Cyc_List_List*) _check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_List_List* temp=({ struct Cyc_List_List*
_temp3=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp3->hd=( void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
_temp3->tl= 0; _temp3;});(( struct Cyc_List_List*) _check_null( prev))->tl= temp;
prev= temp;} return result;} struct Cyc_List_List* Cyc_List_map( void*(* f)(
void*), struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, void*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_rmap)(
Cyc_Core_heap_region, f, x);} struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp4->hd=( void*) f( env,( void*)(( struct
Cyc_List_List*) _check_null( x))->hd); _temp4->tl= 0; _temp4;}); prev= result;
for( x=(( struct Cyc_List_List*) _check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp5=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp5->hd=( void*) f( env,( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); _temp5->tl= 0; _temp5;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;} return result;} struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ return(( struct
Cyc_List_List*(*)( struct _RegionHandle* r2, void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( Cyc_Core_heap_region, f, env, x);}
char Cyc_List_List_mismatch[ 18u]="\000\000\000\000List_mismatch"; struct Cyc_List_List*
Cyc_List_rmap2( struct _RegionHandle* r3, void*(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
void*) Cyc_List_List_mismatch);} result=({ struct Cyc_List_List* _temp6=( struct
Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List)); _temp6->hd=(
void*) f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,( void*)((
struct Cyc_List_List*) _check_null( y))->hd); _temp6->tl= 0; _temp6;}); prev=
result; x=(( struct Cyc_List_List*) _check_null( x))->tl; y=(( struct Cyc_List_List*)
_check_null( y))->tl; while( x != 0? y != 0: 0) {(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp7->hd=( void*) f(( void*)((
struct Cyc_List_List*) _check_null( x))->hd,( void*)(( struct Cyc_List_List*)
_check_null( y))->hd); _temp7->tl= 0; _temp7;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl; x=(( struct Cyc_List_List*) _check_null( x))->tl; y=((
struct Cyc_List_List*) _check_null( y))->tl;} if( x != 0? 1: y != 0){( void)
_throw(( void*) Cyc_List_List_mismatch);} return result;} struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, void*(* f)(
void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rmap2)(
Cyc_Core_heap_region, f, x, y);} void Cyc_List_app( void*(* f)( void*), struct
Cyc_List_List* x){ while( x != 0) { f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); x=(( struct Cyc_List_List*) _check_null( x))->tl;}} void
Cyc_List_app_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
while( x != 0) { f( env,( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
x=(( struct Cyc_List_List*) _check_null( x))->tl;}} void Cyc_List_iter( void(* f)(
void*), struct Cyc_List_List* x){ while( x != 0) { f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); x=(( struct Cyc_List_List*) _check_null( x))->tl;}} void
Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct Cyc_List_List* x){
while( x != 0) { f( env,( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
x=(( struct Cyc_List_List*) _check_null( x))->tl;}} void Cyc_List_app2( void*(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x
!= 0? y != 0: 0) { f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,(
void*)(( struct Cyc_List_List*) _check_null( y))->hd); x=(( struct Cyc_List_List*)
_check_null( x))->tl; y=(( struct Cyc_List_List*) _check_null( y))->tl;} if( x
!= 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);}} void Cyc_List_app2_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List*
y){ while( x != 0? y != 0: 0) { f( env,( void*)(( struct Cyc_List_List*)
_check_null( x))->hd,( void*)(( struct Cyc_List_List*) _check_null( y))->hd); x=((
struct Cyc_List_List*) _check_null( x))->tl; y=(( struct Cyc_List_List*)
_check_null( y))->tl;} if( x != 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);}}
void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y){ while( x != 0? y != 0: 0) { f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd,( void*)(( struct Cyc_List_List*) _check_null( y))->hd); x=((
struct Cyc_List_List*) _check_null( x))->tl; y=(( struct Cyc_List_List*)
_check_null( y))->tl;} if( x != 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);}}
void Cyc_List_iter2_c( void(* f)( void*, void*, void*), void* env, struct Cyc_List_List*
x, struct Cyc_List_List* y){ while( x != 0? y != 0: 0) { f( env,( void*)((
struct Cyc_List_List*) _check_null( x))->hd,( void*)(( struct Cyc_List_List*)
_check_null( y))->hd); x=(( struct Cyc_List_List*) _check_null( x))->tl; y=((
struct Cyc_List_List*) _check_null( y))->tl;} if( x != 0? 1: y != 0){( void)
_throw(( void*) Cyc_List_List_mismatch);}} void* Cyc_List_fold_left( void*(* f)(
void*, void*), void* accum, struct Cyc_List_List* x){ while( x != 0) { accum= f(
accum,( void*)(( struct Cyc_List_List*) _check_null( x))->hd); x=(( struct Cyc_List_List*)
_check_null( x))->tl;} return accum;} void* Cyc_List_fold_left_c( void*(* f)(
void*, void*, void*), void* env, void* accum, struct Cyc_List_List* x){ while( x
!= 0) { accum= f( env, accum,( void*)(( struct Cyc_List_List*) _check_null( x))->hd);
x=(( struct Cyc_List_List*) _check_null( x))->tl;} return accum;} void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum){ if( x == 0){
return accum;} else{ return f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,((
void*(*)( void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right)(
f,(( struct Cyc_List_List*) _check_null( x))->tl, accum));}} void* Cyc_List_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_List_List* x, void*
accum){ if( x == 0){ return accum;} else{ return f( env,( void*)(( struct Cyc_List_List*)
_check_null( x))->hd,(( void*(*)( void*(* f)( void*, void*, void*), void* env,
struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right_c)( f, env,(( struct
Cyc_List_List*) _check_null( x))->tl, accum));}} struct Cyc_List_List* Cyc_List_rrevappend(
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y){
while( x != 0) { y=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp8->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( x))->hd); _temp8->tl= y; _temp8;}); x=((
struct Cyc_List_List*) _check_null( x))->tl;} return y;} struct Cyc_List_List*
Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List* y){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rrevappend)( Cyc_Core_heap_region, x, y);}
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle* r2, struct Cyc_List_List*
x){ if( x == 0){ return 0;} return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)(
r2, x, 0);} struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x))
Cyc_List_rrev)( Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x){ if( x == 0){ return x;} else{ struct Cyc_List_List*
first= x; struct Cyc_List_List* second=(( struct Cyc_List_List*) _check_null( x))->tl;((
struct Cyc_List_List*) _check_null( x))->tl= 0; while( second != 0) { struct Cyc_List_List*
temp=(( struct Cyc_List_List*) _check_null( second))->tl;(( struct Cyc_List_List*)
_check_null( second))->tl= first; first= second; second= temp;} return first;}}
struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle* r2, struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return y;} if( y == 0){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r2, x);}
result=({ struct Cyc_List_List* _temp9=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp9->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); _temp9->tl= 0; _temp9;}); prev= result; for( x=(( struct
Cyc_List_List*) _check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev))->tl=({
struct Cyc_List_List* _temp10=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp10->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); _temp10->tl= 0; _temp10;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl;}(( struct Cyc_List_List*) _check_null( prev))->tl= y;
return result;} struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
Cyc_Core_heap_region, x, y);} struct Cyc_List_List* Cyc_List_imp_append( struct
Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if( x == 0){
return y;} if( y == 0){ return x;} for( z= x;(( struct Cyc_List_List*)
_check_null( z))->tl != 0; z=(( struct Cyc_List_List*) _check_null( z))->tl){;}((
struct Cyc_List_List*) _check_null( z))->tl= y; return x;} struct Cyc_List_List*
Cyc_List_rflatten( struct _RegionHandle* r3, struct Cyc_List_List* x){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _RegionHandle*,
struct Cyc_List_List*, struct Cyc_List_List*), struct _RegionHandle* env, struct
Cyc_List_List* x, struct Cyc_List_List* accum)) Cyc_List_fold_right_c)(( struct
Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rappend, r3, x, 0);} struct Cyc_List_List* Cyc_List_flatten( struct
Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct Cyc_List_List* x)) Cyc_List_rflatten)( Cyc_Core_heap_region, x);} struct
Cyc_List_List* Cyc_List_imp_merge( int(* less_eq)( void*, void*), struct Cyc_List_List*
a, struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d;
if( a == 0){ return b;} if( b == 0){ return a;} if( less_eq(( void*)(( struct
Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( b))->hd) <= 0){ c= a; a=(( struct Cyc_List_List*) _check_null( a))->tl;}
else{ c= b; b=(( struct Cyc_List_List*) _check_null( b))->tl;} d= c; while( a !=
0? b != 0: 0) { if( less_eq(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,(
void*)(( struct Cyc_List_List*) _check_null( b))->hd) <= 0){(( struct Cyc_List_List*)
_check_null( c))->tl= a; c= a; a=(( struct Cyc_List_List*) _check_null( a))->tl;}
else{(( struct Cyc_List_List*) _check_null( c))->tl= b; c= b; b=(( struct Cyc_List_List*)
_check_null( b))->tl;}} if( a == 0){(( struct Cyc_List_List*) _check_null( c))->tl=
b;} else{(( struct Cyc_List_List*) _check_null( c))->tl= a;} return d;} struct
Cyc_List_List* Cyc_List_rimp_merge_sort( int(* less_eq)( void*, void*), struct
Cyc_List_List* x){ if( x == 0? 1:(( struct Cyc_List_List*) _check_null( x))->tl
== 0){ return x;}{ struct Cyc_List_List* a= x; struct Cyc_List_List* aptr= a;
struct Cyc_List_List* b=(( struct Cyc_List_List*) _check_null( x))->tl; struct
Cyc_List_List* bptr= b; x=(( struct Cyc_List_List*) _check_null( b))->tl; while(
x != 0) {(( struct Cyc_List_List*) _check_null( aptr))->tl= x; aptr= x; x=((
struct Cyc_List_List*) _check_null( x))->tl; if( x != 0){(( struct Cyc_List_List*)
_check_null( bptr))->tl= x; bptr= x; x=(( struct Cyc_List_List*) _check_null( x))->tl;}}((
struct Cyc_List_List*) _check_null( aptr))->tl= 0;(( struct Cyc_List_List*)
_check_null( bptr))->tl= 0; return(( struct Cyc_List_List*(*)( int(* less_eq)(
void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b)) Cyc_List_imp_merge)(
less_eq,(( struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List*
x)) Cyc_List_rimp_merge_sort)( less_eq, a),(( struct Cyc_List_List*(*)( int(*
less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_rimp_merge_sort)(
less_eq, b));}} struct Cyc_List_List* Cyc_List_rmerge_sort( struct _RegionHandle*
r2, int(* less_eq)( void*, void*), struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
int(* less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_rimp_merge_sort)(
less_eq,(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r2, x));} struct Cyc_List_List* Cyc_List_rmerge( struct
_RegionHandle* r3, int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if(
a == 0){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);} if( b == 0){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);}
if( less_eq(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( b))->hd) <= 0){ c=({ struct Cyc_List_List*
_temp11=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp11->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( a))->hd);
_temp11->tl= 0; _temp11;}); a=(( struct Cyc_List_List*) _check_null( a))->tl;}
else{ c=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*) _region_malloc(
r3, sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( b))->hd); _temp12->tl= 0; _temp12;}); b=(( struct Cyc_List_List*)
_check_null( b))->tl;} d= c; while( a != 0? b != 0: 0) { if( less_eq(( void*)((
struct Cyc_List_List*) _check_null( a))->hd,( void*)(( struct Cyc_List_List*)
_check_null( b))->hd) <= 0){(( struct Cyc_List_List*) _check_null( c))->tl=({
struct Cyc_List_List* _temp13=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( a))->hd); _temp13->tl= 0; _temp13;}); c=(( struct Cyc_List_List*)
_check_null( c))->tl; a=(( struct Cyc_List_List*) _check_null( a))->tl;} else{((
struct Cyc_List_List*) _check_null( c))->tl=({ struct Cyc_List_List* _temp14=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp14->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( b))->hd);
_temp14->tl= 0; _temp14;}); c=(( struct Cyc_List_List*) _check_null( c))->tl; b=((
struct Cyc_List_List*) _check_null( b))->tl;}} if( a == 0){(( struct Cyc_List_List*)
_check_null( c))->tl=(( struct Cyc_List_List*(*)( struct _RegionHandle* r2,
struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, b);} else{(( struct Cyc_List_List*)
_check_null( c))->tl=(( struct Cyc_List_List*(*)( struct _RegionHandle* r2,
struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);} return d;} struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* less_eq)(
void*, void*), struct Cyc_List_List* x)) Cyc_List_rmerge_sort)( Cyc_Core_heap_region,
less_eq, x);} struct Cyc_List_List* Cyc_List_merge( int(* less_eq)( void*, void*),
struct Cyc_List_List* a, struct Cyc_List_List* b){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b)) Cyc_List_rmerge)( Cyc_Core_heap_region, less_eq, a, b);}
char Cyc_List_Nth[ 8u]="\000\000\000\000Nth"; void* Cyc_List_nth( struct Cyc_List_List*
x, int i){ while( i > 0? x != 0: 0) { -- i; x=(( struct Cyc_List_List*)
_check_null( x))->tl;} if( i < 0? 1: x == 0){( void) _throw(( void*) Cyc_List_Nth);}
return( void*)(( struct Cyc_List_List*) _check_null( x))->hd;} int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x){ while( x != 0? pred(( void*)((
struct Cyc_List_List*) _check_null( x))->hd): 0) { x=(( struct Cyc_List_List*)
_check_null( x))->tl;} return x == 0;} int Cyc_List_forall_c( int(* pred)( void*,
void*), void* env, struct Cyc_List_List* x){ while( x != 0? pred( env,( void*)((
struct Cyc_List_List*) _check_null( x))->hd): 0) { x=(( struct Cyc_List_List*)
_check_null( x))->tl;} return x == 0;} int Cyc_List_exists( int(* pred)( void*),
struct Cyc_List_List* x){ while( x != 0? ! pred(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd): 0) { x=(( struct Cyc_List_List*) _check_null( x))->tl;}
return x != 0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? ! pred( env,( void*)(( struct Cyc_List_List*)
_check_null( x))->hd): 0) { x=(( struct Cyc_List_List*) _check_null( x))->tl;}
return x != 0;} struct _tuple2{ void* f1; void* f2; } ; struct Cyc_List_List*
Cyc_List_rzip( struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
void*) Cyc_List_List_mismatch);} result=({ struct Cyc_List_List* _temp15=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp15->hd=( void*)({ struct _tuple2* _temp16=( struct _tuple2*) _region_malloc(
r4, sizeof( struct _tuple2)); _temp16->f1=( void*)(( struct Cyc_List_List*)
_check_null( x))->hd; _temp16->f2=( void*)(( struct Cyc_List_List*) _check_null(
y))->hd; _temp16;}); _temp15->tl= 0; _temp15;}); prev= result; x=(( struct Cyc_List_List*)
_check_null( x))->tl; y=(( struct Cyc_List_List*) _check_null( y))->tl; while( x
!= 0? y != 0: 0) {(( struct Cyc_List_List*) _check_null( prev))->tl=({ struct
Cyc_List_List* _temp17=( struct Cyc_List_List*) _region_malloc( r3, sizeof(
struct Cyc_List_List)); _temp17->hd=( void*)({ struct _tuple2* _temp18=( struct
_tuple2*) _region_malloc( r4, sizeof( struct _tuple2)); _temp18->f1=( void*)((
struct Cyc_List_List*) _check_null( x))->hd; _temp18->f2=( void*)(( struct Cyc_List_List*)
_check_null( y))->hd; _temp18;}); _temp17->tl= 0; _temp17;}); prev=(( struct Cyc_List_List*)
_check_null( prev))->tl; x=(( struct Cyc_List_List*) _check_null( x))->tl; y=((
struct Cyc_List_List*) _check_null( y))->tl;} if( x != 0? 1: y != 0){( void)
_throw(( void*) Cyc_List_List_mismatch);} return result;} struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y){ return(( struct
Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4, struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( Cyc_Core_heap_region,
Cyc_Core_heap_region, x, y);} struct _tuple0 Cyc_List_rsplit( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x){ struct
Cyc_List_List* result1; struct Cyc_List_List* prev1; struct Cyc_List_List*
result2; struct Cyc_List_List* prev2; if( x == 0){ return({ struct _tuple0
_temp19; _temp19.f1= 0; _temp19.f2= 0; _temp19;});} prev1=( result1=({ struct
Cyc_List_List* _temp20=( struct Cyc_List_List*) _region_malloc( r3, sizeof(
struct Cyc_List_List)); _temp20->hd=( void*)((( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( x))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1; _temp20->tl=
0; _temp20;})); prev2=( result2=({ struct Cyc_List_List* _temp21=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp21->hd=( void*)(((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( x))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2; _temp21->tl= 0; _temp21;})); for( x=((
struct Cyc_List_List*) _check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){(( struct Cyc_List_List*) _check_null( prev1))->tl=({
struct Cyc_List_List* _temp22=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp22->hd=( void*)((( struct _tuple2*)((
struct Cyc_List_List*) _check_null( x))->hd)[ _check_known_subscript_notnull( 1u,
0)]).f1; _temp22->tl= 0; _temp22;});(( struct Cyc_List_List*) _check_null( prev2))->tl=({
struct Cyc_List_List* _temp23=( struct Cyc_List_List*) _region_malloc( r4,
sizeof( struct Cyc_List_List)); _temp23->hd=( void*)((( struct _tuple2*)((
struct Cyc_List_List*) _check_null( x))->hd)[ _check_known_subscript_notnull( 1u,
0)]).f2; _temp23->tl= 0; _temp23;}); prev1=(( struct Cyc_List_List*) _check_null(
prev1))->tl; prev2=(( struct Cyc_List_List*) _check_null( prev2))->tl;} return({
struct _tuple0 _temp24; _temp24.f1= result1; _temp24.f2= result2; _temp24;});}
struct _tuple0 Cyc_List_split( struct Cyc_List_List* x){ return(( struct _tuple0(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x))
Cyc_List_rsplit)( Cyc_Core_heap_region, Cyc_Core_heap_region, x);} struct
_tuple3{ void* f1; void* f2; void* f3; } ; struct _tuple1 Cyc_List_rsplit3(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct _RegionHandle* r5,
struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; struct Cyc_List_List*
result3; struct Cyc_List_List* prev3; if( x == 0){ return({ struct _tuple1
_temp25; _temp25.f1= 0; _temp25.f2= 0; _temp25.f3= 0; _temp25;});} prev1=(
result1=({ struct Cyc_List_List* _temp26=( struct Cyc_List_List*) _region_malloc(
r3, sizeof( struct Cyc_List_List)); _temp26->hd=( void*)((( struct _tuple3*)((
struct Cyc_List_List*) _check_null( x))->hd)[ _check_known_subscript_notnull( 1u,
0)]).f1; _temp26->tl= 0; _temp26;})); prev2=( result2=({ struct Cyc_List_List*
_temp27=( struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp27->hd=( void*)((( struct _tuple3*)(( struct Cyc_List_List*) _check_null( x))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2; _temp27->tl= 0; _temp27;})); prev3=(
result3=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*) _region_malloc(
r5, sizeof( struct Cyc_List_List)); _temp28->hd=( void*)((( struct _tuple3*)((
struct Cyc_List_List*) _check_null( x))->hd)[ _check_known_subscript_notnull( 1u,
0)]).f3; _temp28->tl= 0; _temp28;})); for( x=(( struct Cyc_List_List*)
_check_null( x))->tl; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){((
struct Cyc_List_List*) _check_null( prev1))->tl=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp29->hd=( void*)((( struct _tuple3*)(( struct Cyc_List_List*) _check_null( x))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1; _temp29->tl= 0; _temp29;});((
struct Cyc_List_List*) _check_null( prev2))->tl=({ struct Cyc_List_List* _temp30=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp30->hd=( void*)((( struct _tuple3*)(( struct Cyc_List_List*) _check_null( x))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2; _temp30->tl= 0; _temp30;});((
struct Cyc_List_List*) _check_null( prev3))->tl=({ struct Cyc_List_List* _temp31=(
struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp31->hd=( void*)((( struct _tuple3*)(( struct Cyc_List_List*) _check_null( x))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f3; _temp31->tl= 0; _temp31;}); prev1=((
struct Cyc_List_List*) _check_null( prev1))->tl; prev2=(( struct Cyc_List_List*)
_check_null( prev2))->tl; prev3=(( struct Cyc_List_List*) _check_null( prev3))->tl;}
return({ struct _tuple1 _temp32; _temp32.f1= result1; _temp32.f2= result2;
_temp32.f3= result3; _temp32;});} struct _tuple1 Cyc_List_split3( struct Cyc_List_List*
x){ return(( struct _tuple1(*)( struct _RegionHandle* r3, struct _RegionHandle*
r4, struct _RegionHandle* r5, struct Cyc_List_List* x)) Cyc_List_rsplit3)( Cyc_Core_heap_region,
Cyc_Core_heap_region, Cyc_Core_heap_region, x);} int Cyc_List_memq( struct Cyc_List_List*
l, void* x){ while( l != 0) { if(( void*)(( struct Cyc_List_List*) _check_null(
l))->hd == x){ return 1;} l=(( struct Cyc_List_List*) _check_null( l))->tl;}
return 0;} int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x){ while( l != 0) { if( compare(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd, x) == 0){ return 1;} l=(( struct Cyc_List_List*)
_check_null( l))->tl;} return 0;} void* Cyc_List_assoc( struct Cyc_List_List* l,
void* x){ while( l != 0) { if(((( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( l))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1 == x){ return(((
struct _tuple2*)(( struct Cyc_List_List*) _check_null( l))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2;} l=(( struct Cyc_List_List*)
_check_null( l))->tl;}( void) _throw(( void*) Cyc_Core_Not_found);} void* Cyc_List_assoc_cmp(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x){ while( l != 0) {
if( compare( x,((( struct _tuple2*)(( struct Cyc_List_List*) _check_null( l))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1) == 0){ return((( struct _tuple2*)((
struct Cyc_List_List*) _check_null( l))->hd)[ _check_known_subscript_notnull( 1u,
0)]).f2;} l=(( struct Cyc_List_List*) _check_null( l))->tl;}( void) _throw((
void*) Cyc_Core_Not_found);} int Cyc_List_mem_assoc( struct Cyc_List_List* l,
void* x){ while( l != 0) { if(((( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( l))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1 == x){ return
1;} l=(( struct Cyc_List_List*) _check_null( l))->tl;} return 0;} struct Cyc_Core_Opt*
Cyc_List_check_unique( int(* compare)( void*, void*), struct Cyc_List_List* x){
while( x != 0) { if((( struct Cyc_List_List*) _check_null( x))->tl != 0){ if(
compare(( void*)(( struct Cyc_List_List*) _check_null( x))->hd,( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( x))->tl))->hd)
== 0){ return({ struct Cyc_Core_Opt* _temp33=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp33->v=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd); _temp33;});}} x=(( struct Cyc_List_List*) _check_null( x))->tl;}
return 0;} struct _tagged_ptr0 Cyc_List_rto_array( struct _RegionHandle* r2,
struct Cyc_List_List* x){ int s; struct _tagged_ptr0 arr; s=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( x); arr=({ unsigned int _temp34=(
unsigned int) s; void** _temp35=( void**) _region_malloc( r2, sizeof( void*) *
_temp34); struct _tagged_ptr0 _temp37={ _temp35, _temp35, _temp35 + _temp34};{
unsigned int _temp36= _temp34; unsigned int i; for( i= 0; i < _temp36; i ++){
_temp35[ i]=( void*)(( struct Cyc_List_List*) _check_null( x))->hd;}}; _temp37;});{
int i= 0; for( 0; i < s;( ++ i, x=(( struct Cyc_List_List*) _check_null( x))->tl)){*((
void**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( arr, sizeof( void*), i)=( void*)(( struct Cyc_List_List*)
_check_null( x))->hd;}} return arr;} struct _tagged_ptr0 Cyc_List_to_array(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct _RegionHandle*
r2, struct Cyc_List_List* x)) Cyc_List_rto_array)( Cyc_Core_heap_region, x);}
struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2, struct
_tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{ unsigned int i=({ struct
_tagged_ptr0 _temp39= arr;( unsigned int)( _temp39.last_plus_one - _temp39.curr);})
- 1; for( 0; i >= 0; -- i){ ans=({ struct Cyc_List_List* _temp38=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp38->hd=( void*)*(( void**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( arr,
sizeof( void*),( int) i); _temp38->tl= ans; _temp38;});}} return ans;} struct
Cyc_List_List* Cyc_List_from_array( struct _tagged_ptr0 arr){ return(( struct
Cyc_List_List*(*)( struct _RegionHandle* r2, struct _tagged_ptr0 arr)) Cyc_List_rfrom_array)(
Cyc_Core_heap_region, arr);} struct Cyc_List_List* Cyc_List_rtabulate( struct
_RegionHandle* r, int n, void*(* f)( int)){ struct Cyc_List_List* res= 0;{ int i=
0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp40=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp40->hd=( void*) f( i);
_temp40->tl= res; _temp40;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( res);} struct Cyc_List_List* Cyc_List_tabulate( int n,
void*(* f)( int)){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r,
int n, void*(* f)( int))) Cyc_List_rtabulate)( Cyc_Core_heap_region, n, f);}
struct Cyc_List_List* Cyc_List_rtabulate_c( struct _RegionHandle* r, int n, void*(*
f)( void*, int), void* env){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i
< n; ++ i){ res=({ struct Cyc_List_List* _temp41=( struct Cyc_List_List*)
_region_malloc( r, sizeof( struct Cyc_List_List)); _temp41->hd=( void*) f( env,
i); _temp41->tl= res; _temp41;});}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( res);} struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r, int n, void*(* f)( void*, int), void* env)) Cyc_List_rtabulate_c)(
Cyc_Core_heap_region, n, f, env);} int Cyc_List_list_cmp( int(* cmp)( void*,
void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2
!= 0: 0;( l1=(( struct Cyc_List_List*) _check_null( l1))->tl, l2=(( struct Cyc_List_List*)
_check_null( l2))->tl)){ if( l1 == l2){ return 0;}{ int _temp42= cmp(( void*)((
struct Cyc_List_List*) _check_null( l1))->hd,( void*)(( struct Cyc_List_List*)
_check_null( l2))->hd); goto _LL43; _LL43: if( _temp42 != 0){ return _temp42;}}}
if( l1 != 0){ return 1;} if( l2 != 0){ return - 1;} return 0;} int Cyc_List_list_prefix(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){
for( 0; l1 != 0? l2 != 0: 0;( l1=(( struct Cyc_List_List*) _check_null( l1))->tl,
l2=(( struct Cyc_List_List*) _check_null( l2))->tl)){ if( l1 == l2){ return 1;}{
int _temp44= cmp(( void*)(( struct Cyc_List_List*) _check_null( l1))->hd,( void*)((
struct Cyc_List_List*) _check_null( l2))->hd); goto _LL45; _LL45: if( _temp44 !=
0){ return 0;}}} return l1 == 0;} struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp47=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp47->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
_temp47->tl= 0; _temp47;}); struct Cyc_List_List* end= result; for( 0; l != 0; l=((
struct Cyc_List_List*) _check_null( l))->tl){ if( f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd)){(( struct Cyc_List_List*) _check_null( end))->tl=({
struct Cyc_List_List* _temp46=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); _temp46->tl= 0; _temp46;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl;}} return(( struct Cyc_List_List*) _check_null( result))->tl;}}
struct Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env,
struct Cyc_List_List* l){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List* l))
Cyc_List_rfilter_c)( Cyc_Core_heap_region, f, env, l);} struct Cyc_List_List*
Cyc_List_rfilter( struct _RegionHandle* r2, int(* f)( void*), struct Cyc_List_List*
l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp49=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp49->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( l))->hd);
_temp49->tl= 0; _temp49;}); struct Cyc_List_List* end= result; for( 0; l != 0; l=((
struct Cyc_List_List*) _check_null( l))->tl){ if( f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd)){(( struct Cyc_List_List*) _check_null( end))->tl=({
struct Cyc_List_List* _temp48=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp48->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); _temp48->tl= 0; _temp48;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl;}} return(( struct Cyc_List_List*) _check_null( result))->tl;}}
struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* f)( void*),
struct Cyc_List_List* l)) Cyc_List_rfilter)( Cyc_Core_heap_region, f, l);}