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
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Queue_Queue; typedef
struct Cyc_Queue_Queue* Cyc_Queue_gqueue_t; typedef struct Cyc_Queue_Queue* Cyc_Queue_queue_t;
extern int Cyc_Queue_is_empty( struct Cyc_Queue_Queue*); extern char Cyc_Queue_Empty_tag[
6u]; struct Cyc_Queue_Empty_struct{ char* tag; } ; extern struct Cyc_Queue_Queue*
Cyc_Queue_create(); extern void Cyc_Queue_add( struct Cyc_Queue_Queue*, void* x);
extern void* Cyc_Queue_take( struct Cyc_Queue_Queue*); extern void* Cyc_Queue_peek(
struct Cyc_Queue_Queue*); extern void Cyc_Queue_clear( struct Cyc_Queue_Queue*);
extern int Cyc_Queue_length( struct Cyc_Queue_Queue*); extern void Cyc_Queue_iter(
void(* f)( void*), struct Cyc_Queue_Queue*); extern void Cyc_Queue_app( void*(*
f)( void*), struct Cyc_Queue_Queue*); struct Cyc_Queue_Queue{ struct Cyc_List_List*
front; struct Cyc_List_List* rear; } ; int Cyc_Queue_is_empty( struct Cyc_Queue_Queue*
q){ return q->front == 0;} char Cyc_Queue_Empty_tag[ 6u]="Empty"; struct Cyc_Queue_Queue*
Cyc_Queue_create(){ return({ struct Cyc_Queue_Queue* _temp0=( struct Cyc_Queue_Queue*)
GC_malloc( sizeof( struct Cyc_Queue_Queue)); _temp0->front= 0; _temp0->rear= 0;
_temp0;});} void Cyc_Queue_add( struct Cyc_Queue_Queue* q, void* x){ struct Cyc_List_List*
cell=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1->hd=( void*) x; _temp1->tl= 0; _temp1;}); if( q->front
== 0){ q->front= cell; q->rear= cell;} else{( q->rear)->tl= cell; q->rear= cell;}}
void* Cyc_Queue_take( struct Cyc_Queue_Queue* q){ if( q->front == 0){( void)
_throw(({ struct Cyc_Queue_Empty_struct* _temp2=( struct Cyc_Queue_Empty_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Queue_Empty_struct)); _temp2->tag= Cyc_Queue_Empty_tag;(
struct _xenum_struct*) _temp2;}));} else{ void* x=( void*)( q->front)->hd; q->front=(
q->front)->tl; if( q->front == 0){ q->rear= 0;} return x;}} void* Cyc_Queue_peek(
struct Cyc_Queue_Queue* q){ if( q->front == 0){( void) _throw(({ struct Cyc_Queue_Empty_struct*
_temp3=( struct Cyc_Queue_Empty_struct*) GC_malloc_atomic( sizeof( struct Cyc_Queue_Empty_struct));
_temp3->tag= Cyc_Queue_Empty_tag;( struct _xenum_struct*) _temp3;}));} else{
return( void*)( q->front)->hd;}} void Cyc_Queue_clear( struct Cyc_Queue_Queue* q){
q->front= 0; q->rear= 0;} int Cyc_Queue_length( struct Cyc_Queue_Queue* q){
return(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( q->front);} void
Cyc_Queue_iter( void(* f)( void*), struct Cyc_Queue_Queue* q){ struct Cyc_List_List*
x= q->front; for( 0; x != 0; x= x->tl){ f(( void*) x->hd);}} void Cyc_Queue_app(
void*(* f)( void*), struct Cyc_Queue_Queue* q){ struct Cyc_List_List* x= q->front;
for( 0; x != 0; x= x->tl){ f(( void*) x->hd);}}