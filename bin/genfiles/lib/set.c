#include "cyc_include.h"

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
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_app(
void*(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter( void(*
f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ;
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_singleton( int(* comp)( void*, void*), void* x); extern
struct Cyc_Set_Set* Cyc_Set_from_list( int(* comp)( void*, void*), struct Cyc_List_List*
x); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt);
extern struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2); extern struct Cyc_Set_Set* Cyc_Set_intersect( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_diff( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_delete( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
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
void*), void* env, struct Cyc_Set_Set* s); extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; extern void* Cyc_Set_choose( struct
Cyc_Set_Set* s); struct Cyc_Set_Set{ int(* compare)( void*, void*); int
cardinality; struct Cyc_List_List* nodes; } ; struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)){ return({ struct Cyc_Set_Set* _temp0=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp0->compare= comp; _temp0->cardinality=
0; _temp0->nodes= 0; _temp0;});} struct Cyc_Set_Set* Cyc_Set_singleton( int(*
comp)( void*, void*), void* x){ return({ struct Cyc_Set_Set* _temp1=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp1->compare= comp; _temp1->cardinality=
1; _temp1->nodes=({ struct Cyc_List_List* _temp2=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2->hd=( void*) x; _temp2->tl= 0;
_temp2;}); _temp1;});} int Cyc_Set_cardinality( struct Cyc_Set_Set* s){ return s->cardinality;}
int Cyc_Set_is_empty( struct Cyc_Set_Set* s){ return s->cardinality == 0;}
static int Cyc_Set_member_b( int(* compare)( void*, void*), struct Cyc_List_List*
n, void* elt){ while( n != 0) { int i= compare( elt,( void*) n->hd); if( i == 0){
return 1;} else{ if( i < 0){ return 0;} else{ n= n->tl;}}} return 0;} int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt){ return(( int(*)( int(* compare)( void*, void*),
struct Cyc_List_List* n, void* elt)) Cyc_Set_member_b)( s->compare, s->nodes,
elt);} static struct Cyc_List_List* Cyc_Set_insert_b( int(* compare)( void*,
void*), struct Cyc_List_List* n, void* elt){ if( n == 0){ return({ struct Cyc_List_List*
_temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3->hd=( void*) elt; _temp3->tl= 0; _temp3;});} else{ int i= compare( elt,(
void*) n->hd); if( i < 0){ return({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*) elt; _temp4->tl=
n; _temp4;});} else{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp5=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5->hd=( void*)(( void*) n->hd); _temp5->tl= 0; _temp5;}); struct Cyc_List_List*
prev= result; n= n->tl; while( n != 0?( i= compare(( void*) n->hd, elt)) < 0: 0) {
prev->tl=({ struct Cyc_List_List* _temp6=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp6->hd=( void*)(( void*) n->hd); _temp6->tl=
0; _temp6;}); prev= prev->tl; n= n->tl;} prev->tl=({ struct Cyc_List_List*
_temp7=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7->hd=( void*) elt; _temp7->tl= n; _temp7;}); return result;}}} struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set*
s, void* elt)) Cyc_Set_member)( s, elt)){ return s;} else{ return({ struct Cyc_Set_Set*
_temp8=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp8->compare=
s->compare; _temp8->cardinality= s->cardinality + 1; _temp8->nodes=(( struct Cyc_List_List*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_insert_b)(
s->compare, s->nodes, elt); _temp8;});}} struct Cyc_Set_Set* Cyc_Set_union_two(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ if( s1 == s2){ return s1;} if(
s1->cardinality == 0){ return s2;} if( s2->cardinality == 0){ return s1;}{
struct Cyc_List_List* nodes= 0; int cardinality= 0; int(* comp)( void*, void*)=
s1->compare; struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes;
struct Cyc_List_List* curr= 0; while( x1 != 0? x2 != 0: 0) { int i= comp(( void*)
x1->hd,( void*) x2->hd); if( i == 0){ x2= x2->tl;} else{ if( i < 0){ if( curr ==
0){ nodes=({ struct Cyc_List_List* _temp9=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp9->hd=( void*)(( void*) x1->hd); _temp9->tl=
0; _temp9;}); curr= nodes;} else{ curr->tl=({ struct Cyc_List_List* _temp10=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp10->hd=(
void*)(( void*) x1->hd); _temp10->tl= 0; _temp10;}); curr= curr->tl;} x1= x1->tl;
++ cardinality;} else{ if( curr == 0){ nodes=({ struct Cyc_List_List* _temp11=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp11->hd=(
void*)(( void*) x2->hd); _temp11->tl= 0; _temp11;}); curr= nodes;} else{ curr->tl=({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp12->hd=( void*)(( void*) x2->hd); _temp12->tl= 0; _temp12;});
curr= curr->tl;} x2= x2->tl; ++ cardinality;}}} if( x1 != 0){ if( curr == 0){
nodes= x1;} else{ curr->tl= x1;} cardinality +=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( x1);} else{ if( x2 != 0){ if( curr == 0){ nodes= x2;}
else{ curr->tl= x2;} cardinality +=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
x2);}} return({ struct Cyc_Set_Set* _temp13=( struct Cyc_Set_Set*) GC_malloc(
sizeof( struct Cyc_Set_Set)); _temp13->compare= comp; _temp13->cardinality=
cardinality; _temp13->nodes= nodes; _temp13;});}} static struct Cyc_List_List*
Cyc_Set_delete_b( int(* compare)( void*, void*), struct Cyc_List_List* n, void*
elt){ if( compare(( void*) n->hd, elt) == 0){ return n->tl;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp14=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp14->hd=( void*)(( void*) n->hd); _temp14->tl=
0; _temp14;}); struct Cyc_List_List* prev= result; n= n->tl; while( n != 0?
compare(( void*) n->hd, elt) != 0: 0) { prev->tl=({ struct Cyc_List_List*
_temp15=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp15->hd=( void*)(( void*) n->hd); _temp15->tl= 0; _temp15;}); prev= prev->tl;
n= n->tl;} prev->tl= n->tl; return result;}} struct Cyc_Set_Set* Cyc_Set_delete(
struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set* s, void*
elt)) Cyc_Set_member)( s, elt)){ return({ struct Cyc_Set_Set* _temp16=( struct
Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp16->compare= s->compare;
_temp16->cardinality= s->cardinality - 1; _temp16->nodes=(( struct Cyc_List_List*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_delete_b)(
s->compare, s->nodes, elt); _temp16;});} else{ return s;}} struct Cyc_List_List*
Cyc_Set_elements( struct Cyc_Set_Set* s){ return s->nodes;} void* Cyc_Set_fold(
void*(* f)( void*, void*), struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List*
n= s->nodes; while( n != 0) { accum= f(( void*) n->hd, accum); n= n->tl;} return
accum;} void* Cyc_Set_fold_c( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List* n= s->nodes; while( n
!= 0) { accum= f( env,( void*) n->hd, accum); n= n->tl;} return accum;} void Cyc_Set_app(
void*(* f)( void*), struct Cyc_Set_Set* s){(( void(*)( void*(* f)( void*),
struct Cyc_List_List* x)) Cyc_List_app)( f, s->nodes);} void Cyc_Set_iter( void(*
f)( void*), struct Cyc_Set_Set* s){(( void(*)( void(* f)( void*), struct Cyc_List_List*
x)) Cyc_List_iter)( f, s->nodes);} void Cyc_Set_iter_c( void(* f)( void*, void*),
void* env, struct Cyc_Set_Set* s){(( void(*)( void(* f)( void*, void*), void*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( f, env, s->nodes);} struct Cyc_Set_Set*
Cyc_Set_intersect( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ if( s1 == s2){
return s1;}{ int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List* x1=
s1->nodes; struct Cyc_List_List* x2= s2->nodes; struct Cyc_List_List* result= 0;
struct Cyc_List_List* prev= 0; int card= 0; if( x1 == 0){ return s1;} if( x2 ==
0){ return s2;} while( x1 != 0? x2 != 0: 0) { int i= comp(( void*) x1->hd,( void*)
x2->hd); if( i == 0){ if( result == 0){ result=({ struct Cyc_List_List* _temp17=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp17->hd=(
void*)(( void*) x1->hd); _temp17->tl= 0; _temp17;}); prev= result;} else{ prev->tl=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)(( void*) x1->hd); _temp18->tl= 0; _temp18;});
prev= prev->tl;} ++ card; x1= x1->tl; x2= x2->tl;} else{ if( i < 0){ x1= x1->tl;}
else{ x2= x2->tl;}}} return({ struct Cyc_Set_Set* _temp19=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp19->compare= comp; _temp19->cardinality=
card; _temp19->nodes= result; _temp19;});}} struct Cyc_Set_Set* Cyc_Set_from_list(
int(* comp)( void*, void*), struct Cyc_List_List* x){ struct Cyc_List_List* z=((
struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List* x))
Cyc_List_merge_sort)( comp, x);{ struct Cyc_List_List* y= z; for( 0; y != 0; y=
y->tl){ if( y->tl != 0? comp(( void*) y->hd,( void*)( y->tl)->hd) == 0: 0){ y->tl=(
y->tl)->tl;}}} return({ struct Cyc_Set_Set* _temp20=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp20->compare= comp; _temp20->cardinality=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( z); _temp20->nodes= z;
_temp20;});} int Cyc_Set_subset( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List* x1= s1->nodes;
struct Cyc_List_List* x2= s2->nodes; while( 1) { if( x1 == 0){ return 1;} if( x2
== 0){ return 0;}{ int i= comp(( void*) x1->hd,( void*) x2->hd); if( i == 0){ x1=
x1->tl; x2= x2->tl;} else{ if( i > 0){ x2= x2->tl;} else{ return 0;}}}} return 1;}
struct Cyc_Set_Set* Cyc_Set_diff( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List* x1= s1->nodes;
struct Cyc_List_List* x2= s2->nodes; int card= s1->cardinality; if( x2 == 0){
return s1;} while( x2 != 0) { void* elt=( void*) x2->hd; if((( int(*)( int(*
compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_member_b)(
comp, x1, elt)){ -- card; x1=(( struct Cyc_List_List*(*)( int(* compare)( void*,
void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_delete_b)( comp, x1, elt);}
x2= x2->tl;} return({ struct Cyc_Set_Set* _temp21=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp21->compare= comp; _temp21->cardinality=
card; _temp21->nodes= x1; _temp21;});} int Cyc_Set_compare( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2){ if( s1->cardinality != s2->cardinality){ return s1->cardinality
- s2->cardinality;}{ struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List*
x2= s2->nodes; int(* comp)( void*, void*)= s1->compare; while( x1 != 0) { int
diff= comp(( void*) x1->hd,( void*) x2->hd); if( diff != 0){ return diff;} x1=
x1->tl; x2= x2->tl;} return 0;}} int Cyc_Set_equals( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2){ return(( int(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_compare)( s1, s2) == 0;} char Cyc_Set_Absent_tag[ 7u]="Absent";
void* Cyc_Set_choose( struct Cyc_Set_Set* s){ if( s->nodes == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Set_Absent_struct* _temp22=( struct Cyc_Set_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Set_Absent_struct)); _temp22[ 0]=({ struct Cyc_Set_Absent_struct
_temp23; _temp23.tag= Cyc_Set_Absent_tag; _temp23;}); _temp22;}));} return( void*)(
s->nodes)->hd;}