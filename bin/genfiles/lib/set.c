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
tag; struct _tagged_string f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_app( void*(*
f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter( void(* f)( void*),
struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x); extern char Cyc_List_Nth[
8u]; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_rempty( struct _RegionHandle* rgn, int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_singleton( int(* comp)( void*, void*),
void* x); extern struct Cyc_Set_Set* Cyc_Set_from_list( int(* comp)( void*, void*),
struct Cyc_List_List* x); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle*
rgn, struct Cyc_Set_Set* s, void* elt); extern struct Cyc_Set_Set* Cyc_Set_union_two(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_intersect(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_diff(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_delete(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_is_empty( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_subset( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2); extern int Cyc_Set_compare( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern int Cyc_Set_equals( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern struct Cyc_List_List* Cyc_Set_elements( struct
Cyc_Set_Set* s); extern void* Cyc_Set_fold( void*(* f)( void*, void*), struct
Cyc_Set_Set* s, void* accum); extern void* Cyc_Set_fold_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_Set_Set* s, void* accum); extern void Cyc_Set_app(
void*(* f)( void*), struct Cyc_Set_Set* s); extern void Cyc_Set_iter( void(* f)(
void*), struct Cyc_Set_Set* s); extern void Cyc_Set_iter_c( void(* f)( void*,
void*), void* env, struct Cyc_Set_Set* s); extern char Cyc_Set_Absent[ 11u];
extern void* Cyc_Set_choose( struct Cyc_Set_Set* s); struct Cyc_Set_Set{ int(*
compare)( void*, void*); int cardinality; struct Cyc_List_List* nodes; } ;
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)){ return({ struct
Cyc_Set_Set* _temp0=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set));
_temp0->compare= comp; _temp0->cardinality= 0; _temp0->nodes= 0; _temp0;});}
struct Cyc_Set_Set* Cyc_Set_rempty( struct _RegionHandle* rgn, int(* comp)( void*,
void*)){ return({ struct Cyc_Set_Set* _temp1=( struct Cyc_Set_Set*)
_region_malloc( rgn, sizeof( struct Cyc_Set_Set)); _temp1->compare= comp; _temp1->cardinality=
0; _temp1->nodes= 0; _temp1;});} struct Cyc_Set_Set* Cyc_Set_singleton( int(*
comp)( void*, void*), void* x){ return({ struct Cyc_Set_Set* _temp2=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp2->compare= comp; _temp2->cardinality=
1; _temp2->nodes=({ struct Cyc_List_List* _temp3=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3->hd=( void*) x; _temp3->tl= 0;
_temp3;}); _temp2;});} int Cyc_Set_cardinality( struct Cyc_Set_Set* s){ return s->cardinality;}
int Cyc_Set_is_empty( struct Cyc_Set_Set* s){ return s->cardinality == 0;}
static int Cyc_Set_member_b( int(* compare)( void*, void*), struct Cyc_List_List*
n, void* elt){ while( n != 0) { int i= compare( elt,( void*)(( struct Cyc_List_List*)
_check_null( n))->hd); if( i == 0){ return 1;} else{ if( i < 0){ return 0;}
else{ n=(( struct Cyc_List_List*) _check_null( n))->tl;}}} return 0;} int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt){ return(( int(*)( int(* compare)( void*, void*),
struct Cyc_List_List* n, void* elt)) Cyc_Set_member_b)( s->compare, s->nodes,
elt);} static struct Cyc_List_List* Cyc_Set_insert_b( struct _RegionHandle* rgn,
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt){ if( n == 0){
return({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp4->hd=( void*) elt; _temp4->tl= 0;
_temp4;});} else{ int i= compare( elt,( void*)(( struct Cyc_List_List*)
_check_null( n))->hd); if( i < 0){ return({ struct Cyc_List_List* _temp5=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp5->hd=( void*) elt; _temp5->tl= n; _temp5;});} else{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*) _region_malloc(
rgn, sizeof( struct Cyc_List_List)); _temp8->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( n))->hd); _temp8->tl= 0; _temp8;}); struct Cyc_List_List* prev=
result; n=(( struct Cyc_List_List*) _check_null( n))->tl; while( n != 0?( i=
compare(( void*)(( struct Cyc_List_List*) _check_null( n))->hd, elt)) < 0: 0) {((
struct Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp6=(
struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp6->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( n))->hd);
_temp6->tl= 0; _temp6;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl;
n=(( struct Cyc_List_List*) _check_null( n))->tl;}(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp7->hd=( void*) elt;
_temp7->tl= n; _temp7;}); return result;}}} struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set* s, void*
elt)) Cyc_Set_member)( s, elt)){ return s;} else{ return({ struct Cyc_Set_Set*
_temp9=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp9->compare=
s->compare; _temp9->cardinality= s->cardinality + 1; _temp9->nodes=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, int(* compare)( void*, void*), struct Cyc_List_List*
n, void* elt)) Cyc_Set_insert_b)( Cyc_Core_heap_region, s->compare, s->nodes,
elt); _temp9;});}} struct Cyc_Set_Set* Cyc_Set_rinsert( struct _RegionHandle*
rgn, struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set* s,
void* elt)) Cyc_Set_member)( s, elt)){ return s;} else{ return({ struct Cyc_Set_Set*
_temp10=( struct Cyc_Set_Set*) _region_malloc( rgn, sizeof( struct Cyc_Set_Set));
_temp10->compare= s->compare; _temp10->cardinality= s->cardinality + 1; _temp10->nodes=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, int(* compare)( void*, void*),
struct Cyc_List_List* n, void* elt)) Cyc_Set_insert_b)( rgn, s->compare, s->nodes,
elt); _temp10;});}} struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2){ if( s1 == s2){ return s1;} if( s1->cardinality == 0){
return s2;} if( s2->cardinality == 0){ return s1;}{ struct Cyc_List_List* nodes=
0; int cardinality= 0; int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List*
x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; struct Cyc_List_List* curr=
0; while( x1 != 0? x2 != 0: 0) { int i= comp(( void*)(( struct Cyc_List_List*)
_check_null( x1))->hd,( void*)(( struct Cyc_List_List*) _check_null( x2))->hd);
if( i == 0){ x2=(( struct Cyc_List_List*) _check_null( x2))->tl;} else{ if( i <
0){ if( curr == 0){ nodes=({ struct Cyc_List_List* _temp11=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp11->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( x1))->hd); _temp11->tl= 0; _temp11;}); curr=
nodes;} else{(( struct Cyc_List_List*) _check_null( curr))->tl=({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp12->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( x1))->hd);
_temp12->tl= 0; _temp12;}); curr=(( struct Cyc_List_List*) _check_null( curr))->tl;}
x1=(( struct Cyc_List_List*) _check_null( x1))->tl; ++ cardinality;} else{ if(
curr == 0){ nodes=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( x2))->hd); _temp13->tl= 0; _temp13;}); curr=
nodes;} else{(( struct Cyc_List_List*) _check_null( curr))->tl=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( x2))->hd);
_temp14->tl= 0; _temp14;}); curr=(( struct Cyc_List_List*) _check_null( curr))->tl;}
x2=(( struct Cyc_List_List*) _check_null( x2))->tl; ++ cardinality;}}} if( x1 !=
0){ if( curr == 0){ nodes= x1;} else{(( struct Cyc_List_List*) _check_null( curr))->tl=
x1;} cardinality +=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( x1);}
else{ if( x2 != 0){ if( curr == 0){ nodes= x2;} else{(( struct Cyc_List_List*)
_check_null( curr))->tl= x2;} cardinality +=(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( x2);}} return({ struct Cyc_Set_Set* _temp15=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp15->compare= comp; _temp15->cardinality=
cardinality; _temp15->nodes= nodes; _temp15;});}} static struct Cyc_List_List*
Cyc_Set_delete_b( int(* compare)( void*, void*), struct Cyc_List_List* n, void*
elt){ if( compare(( void*)(( struct Cyc_List_List*) _check_null( n))->hd, elt)
== 0){ return(( struct Cyc_List_List*) _check_null( n))->tl;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp17=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp17->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( n))->hd); _temp17->tl= 0; _temp17;}); struct Cyc_List_List* prev=
result; n=(( struct Cyc_List_List*) _check_null( n))->tl; while( n != 0? compare((
void*)(( struct Cyc_List_List*) _check_null( n))->hd, elt) != 0: 0) {(( struct
Cyc_List_List*) _check_null( prev))->tl=({ struct Cyc_List_List* _temp16=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp16->hd=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( n))->hd); _temp16->tl= 0;
_temp16;}); prev=(( struct Cyc_List_List*) _check_null( prev))->tl; n=(( struct
Cyc_List_List*) _check_null( n))->tl;}(( struct Cyc_List_List*) _check_null(
prev))->tl=(( struct Cyc_List_List*) _check_null( n))->tl; return result;}}
struct Cyc_Set_Set* Cyc_Set_delete( struct Cyc_Set_Set* s, void* elt){ if((( int(*)(
struct Cyc_Set_Set* s, void* elt)) Cyc_Set_member)( s, elt)){ return({ struct
Cyc_Set_Set* _temp18=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set));
_temp18->compare= s->compare; _temp18->cardinality= s->cardinality - 1; _temp18->nodes=((
struct Cyc_List_List*(*)( int(* compare)( void*, void*), struct Cyc_List_List* n,
void* elt)) Cyc_Set_delete_b)( s->compare, s->nodes, elt); _temp18;});} else{
return s;}} struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set* s){
return s->nodes;} void* Cyc_Set_fold( void*(* f)( void*, void*), struct Cyc_Set_Set*
s, void* accum){ struct Cyc_List_List* n= s->nodes; while( n != 0) { accum= f((
void*)(( struct Cyc_List_List*) _check_null( n))->hd, accum); n=(( struct Cyc_List_List*)
_check_null( n))->tl;} return accum;} void* Cyc_Set_fold_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List*
n= s->nodes; while( n != 0) { accum= f( env,( void*)(( struct Cyc_List_List*)
_check_null( n))->hd, accum); n=(( struct Cyc_List_List*) _check_null( n))->tl;}
return accum;} void Cyc_Set_app( void*(* f)( void*), struct Cyc_Set_Set* s){((
void(*)( void*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_app)( f, s->nodes);}
void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s){(( void(*)( void(*
f)( void*), struct Cyc_List_List* x)) Cyc_List_iter)( f, s->nodes);} void Cyc_Set_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_Set_Set* s){(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( f, env, s->nodes);}
struct Cyc_Set_Set* Cyc_Set_intersect( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2){ if( s1 == s2){ return s1;}{ int(* comp)( void*, void*)= s1->compare; struct
Cyc_List_List* x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; struct Cyc_List_List*
result= 0; struct Cyc_List_List* prev= 0; int card= 0; if( x1 == 0){ return s1;}
if( x2 == 0){ return s2;} while( x1 != 0? x2 != 0: 0) { int i= comp(( void*)((
struct Cyc_List_List*) _check_null( x1))->hd,( void*)(( struct Cyc_List_List*)
_check_null( x2))->hd); if( i == 0){ if( result == 0){ result=({ struct Cyc_List_List*
_temp19=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp19->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( x1))->hd);
_temp19->tl= 0; _temp19;}); prev= result;} else{(( struct Cyc_List_List*)
_check_null( prev))->tl=({ struct Cyc_List_List* _temp20=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp20->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( x1))->hd); _temp20->tl= 0; _temp20;}); prev=((
struct Cyc_List_List*) _check_null( prev))->tl;} ++ card; x1=(( struct Cyc_List_List*)
_check_null( x1))->tl; x2=(( struct Cyc_List_List*) _check_null( x2))->tl;}
else{ if( i < 0){ x1=(( struct Cyc_List_List*) _check_null( x1))->tl;} else{ x2=((
struct Cyc_List_List*) _check_null( x2))->tl;}}} return({ struct Cyc_Set_Set*
_temp21=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp21->compare=
comp; _temp21->cardinality= card; _temp21->nodes= result; _temp21;});}} struct
Cyc_Set_Set* Cyc_Set_from_list( int(* comp)( void*, void*), struct Cyc_List_List*
x){ struct Cyc_List_List* z=(( struct Cyc_List_List*(*)( int(* less_eq)( void*,
void*), struct Cyc_List_List* x)) Cyc_List_merge_sort)( comp, x);{ struct Cyc_List_List*
y= z; for( 0; y != 0; y=(( struct Cyc_List_List*) _check_null( y))->tl){ if(((
struct Cyc_List_List*) _check_null( y))->tl != 0? comp(( void*)(( struct Cyc_List_List*)
_check_null( y))->hd,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( y))->tl))->hd) == 0: 0){(( struct Cyc_List_List*) _check_null( y))->tl=((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( y))->tl))->tl;}}}
return({ struct Cyc_Set_Set* _temp22=( struct Cyc_Set_Set*) GC_malloc( sizeof(
struct Cyc_Set_Set)); _temp22->compare= comp; _temp22->cardinality=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( z); _temp22->nodes= z; _temp22;});}
int Cyc_Set_subset( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ int(* comp)(
void*, void*)= s1->compare; struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List*
x2= s2->nodes; while( 1) { if( x1 == 0){ return 1;} if( x2 == 0){ return 0;}{
int i= comp(( void*)(( struct Cyc_List_List*) _check_null( x1))->hd,( void*)((
struct Cyc_List_List*) _check_null( x2))->hd); if( i == 0){ x1=(( struct Cyc_List_List*)
_check_null( x1))->tl; x2=(( struct Cyc_List_List*) _check_null( x2))->tl;}
else{ if( i > 0){ x2=(( struct Cyc_List_List*) _check_null( x2))->tl;} else{
return 0;}}}} return 1;} struct Cyc_Set_Set* Cyc_Set_diff( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2){ int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List*
x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; int card= s1->cardinality;
if( x2 == 0){ return s1;} while( x2 != 0) { void* elt=( void*)(( struct Cyc_List_List*)
_check_null( x2))->hd; if((( int(*)( int(* compare)( void*, void*), struct Cyc_List_List*
n, void* elt)) Cyc_Set_member_b)( comp, x1, elt)){ -- card; x1=(( struct Cyc_List_List*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_delete_b)(
comp, x1, elt);} x2=(( struct Cyc_List_List*) _check_null( x2))->tl;} return({
struct Cyc_Set_Set* _temp23=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set));
_temp23->compare= comp; _temp23->cardinality= card; _temp23->nodes= x1; _temp23;});}
int Cyc_Set_compare( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ if( s1->cardinality
!= s2->cardinality){ return s1->cardinality - s2->cardinality;}{ struct Cyc_List_List*
x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; int(* comp)( void*, void*)=
s1->compare; while( x1 != 0) { int diff= comp(( void*)(( struct Cyc_List_List*)
_check_null( x1))->hd,( void*)(( struct Cyc_List_List*) _check_null( x2))->hd);
if( diff != 0){ return diff;} x1=(( struct Cyc_List_List*) _check_null( x1))->tl;
x2=(( struct Cyc_List_List*) _check_null( x2))->tl;} return 0;}} int Cyc_Set_equals(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ return(( int(*)( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2)) Cyc_Set_compare)( s1, s2) == 0;} char Cyc_Set_Absent[
11u]="\000\000\000\000Absent"; void* Cyc_Set_choose( struct Cyc_Set_Set* s){ if(
s->nodes == 0){( void) _throw(( void*) Cyc_Set_Absent);} return( void*)(( struct
Cyc_List_List*) _check_null( s->nodes))->hd;}