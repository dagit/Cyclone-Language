 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Queue_Queue; extern int
Cyc_Queue_is_empty( struct Cyc_Queue_Queue*); extern unsigned char Cyc_Queue_Empty[
10u]; extern struct Cyc_Queue_Queue* Cyc_Queue_create(); extern void Cyc_Queue_add(
struct Cyc_Queue_Queue*, void* x); extern void Cyc_Queue_radd( struct
_RegionHandle*, struct Cyc_Queue_Queue*, void* x); extern void* Cyc_Queue_take(
struct Cyc_Queue_Queue*); extern void* Cyc_Queue_peek( struct Cyc_Queue_Queue*);
extern void Cyc_Queue_clear( struct Cyc_Queue_Queue*); extern int Cyc_Queue_length(
struct Cyc_Queue_Queue*); extern void Cyc_Queue_iter( void(* f)( void*), struct
Cyc_Queue_Queue*); extern void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue*);
struct Cyc_Queue_Queue{ struct Cyc_List_List* front; struct Cyc_List_List* rear;
} ; int Cyc_Queue_is_empty( struct Cyc_Queue_Queue* q){ return q->front ==  0;}
unsigned char Cyc_Queue_Empty[ 10u]="\000\000\000\000Empty"; struct Cyc_Queue_Queue*
Cyc_Queue_create(){ return({ struct Cyc_Queue_Queue* _temp0=( struct Cyc_Queue_Queue*)
GC_malloc( sizeof( struct Cyc_Queue_Queue)); _temp0->front= 0; _temp0->rear= 0;
_temp0;});} void Cyc_Queue_radd( struct _RegionHandle* r, struct Cyc_Queue_Queue*
q, void* x){ struct Cyc_List_List* cell=({ struct Cyc_List_List* _temp1=( struct
Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List)); _temp1->hd=(
void*) x; _temp1->tl= 0; _temp1;}); if( q->front ==  0){ q->front= cell; q->rear=
cell;} else{(( struct Cyc_List_List*) _check_null( q->rear))->tl= cell; q->rear=
cell;}} void Cyc_Queue_add( struct Cyc_Queue_Queue* q, void* x){ Cyc_Queue_radd(
Cyc_Core_heap_region, q, x);} void* Cyc_Queue_take( struct Cyc_Queue_Queue* q){
if( q->front ==  0){( int) _throw(( void*) Cyc_Queue_Empty);} else{ void* _temp2=(
void*)(( struct Cyc_List_List*) _check_null( q->front))->hd; q->front=(( struct
Cyc_List_List*) _check_null( q->front))->tl; if( q->front ==  0){ q->rear= 0;}
return _temp2;}} void* Cyc_Queue_peek( struct Cyc_Queue_Queue* q){ if( q->front
==  0){( int) _throw(( void*) Cyc_Queue_Empty);} else{ return( void*)(( struct
Cyc_List_List*) _check_null( q->front))->hd;}} void Cyc_Queue_clear( struct Cyc_Queue_Queue*
q){ q->front= 0; q->rear= 0;} int Cyc_Queue_length( struct Cyc_Queue_Queue* q){
return Cyc_List_length( q->front);} void Cyc_Queue_iter( void(* f)( void*),
struct Cyc_Queue_Queue* q){ struct Cyc_List_List* x= q->front; for( 0; x !=  0;
x=(( struct Cyc_List_List*) _check_null( x))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd);}} void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue*
q){ struct Cyc_List_List* x= q->front; for( 0; x !=  0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd);}}
