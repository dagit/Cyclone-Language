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
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern void Cyc_List_app( void*(* f)( void*), struct Cyc_List_List*
x); extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x);
extern void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_merge_sort( int(* less_eq)( void*,
void*), struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_singleton( int(* comp)( void*, void*), void* x);
extern struct Cyc_Set_Set* Cyc_Set_from_list( int(* comp)( void*, void*), struct
Cyc_List_List* x); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set*
s, void* elt); extern struct Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set*
s1, struct Cyc_Set_Set* s2); extern struct Cyc_Set_Set* Cyc_Set_intersect(
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
n, void* elt){ while( n != 0) { int i= compare( elt,( void*)({ struct Cyc_List_List*
_temp4= n; if( _temp4 == 0){ _throw( Null_Exception);} _temp4->hd;})); if( i ==
0){ return 1;} else{ if( i < 0){ return 0;} else{ n=({ struct Cyc_List_List*
_temp3= n; if( _temp3 == 0){ _throw( Null_Exception);} _temp3->tl;});}}} return
0;} int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt){ return(( int(*)( int(*
compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_member_b)(
s->compare, s->nodes, elt);} static struct Cyc_List_List* Cyc_Set_insert_b( int(*
compare)( void*, void*), struct Cyc_List_List* n, void* elt){ if( n == 0){
return({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp5->hd=( void*) elt; _temp5->tl= 0; _temp5;});}
else{ int i= compare( elt,( void*)({ struct Cyc_List_List* _temp18= n; if(
_temp18 == 0){ _throw( Null_Exception);} _temp18->hd;})); if( i < 0){ return({
struct Cyc_List_List* _temp6=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp6->hd=( void*) elt; _temp6->tl= n; _temp6;});} else{
struct Cyc_List_List* result=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp16->hd=( void*)(( void*)({
struct Cyc_List_List* _temp17= n; if( _temp17 == 0){ _throw( Null_Exception);}
_temp17->hd;})); _temp16->tl= 0; _temp16;}); struct Cyc_List_List* prev= result;
n=({ struct Cyc_List_List* _temp7= n; if( _temp7 == 0){ _throw( Null_Exception);}
_temp7->tl;}); while( n != 0?( i= compare(( void*)({ struct Cyc_List_List*
_temp8= n; if( _temp8 == 0){ _throw( Null_Exception);} _temp8->hd;}), elt)) < 0:
0) {({ struct Cyc_List_List* _temp9= prev; if( _temp9 == 0){ _throw(
Null_Exception);} _temp9->tl=({ struct Cyc_List_List* _temp10=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp10->hd=( void*)(( void*)({
struct Cyc_List_List* _temp11= n; if( _temp11 == 0){ _throw( Null_Exception);}
_temp11->hd;})); _temp10->tl= 0; _temp10;});}); prev=({ struct Cyc_List_List*
_temp12= prev; if( _temp12 == 0){ _throw( Null_Exception);} _temp12->tl;}); n=({
struct Cyc_List_List* _temp13= n; if( _temp13 == 0){ _throw( Null_Exception);}
_temp13->tl;});}({ struct Cyc_List_List* _temp14= prev; if( _temp14 == 0){
_throw( Null_Exception);} _temp14->tl=({ struct Cyc_List_List* _temp15=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp15->hd=( void*)
elt; _temp15->tl= n; _temp15;});}); return result;}}} struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set* s, void*
elt)) Cyc_Set_member)( s, elt)){ return s;} else{ return({ struct Cyc_Set_Set*
_temp19=( struct Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp19->compare=
s->compare; _temp19->cardinality= s->cardinality + 1; _temp19->nodes=(( struct
Cyc_List_List*(*)( int(* compare)( void*, void*), struct Cyc_List_List* n, void*
elt)) Cyc_Set_insert_b)( s->compare, s->nodes, elt); _temp19;});}} struct Cyc_Set_Set*
Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ if( s1 == s2){
return s1;} if( s1->cardinality == 0){ return s2;} if( s2->cardinality == 0){
return s1;}{ struct Cyc_List_List* nodes= 0; int cardinality= 0; int(* comp)(
void*, void*)= s1->compare; struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List*
x2= s2->nodes; struct Cyc_List_List* curr= 0; while( x1 != 0? x2 != 0: 0) { int
i= comp(( void*)({ struct Cyc_List_List* _temp35= x1; if( _temp35 == 0){ _throw(
Null_Exception);} _temp35->hd;}),( void*)({ struct Cyc_List_List* _temp36= x2;
if( _temp36 == 0){ _throw( Null_Exception);} _temp36->hd;})); if( i == 0){ x2=({
struct Cyc_List_List* _temp20= x2; if( _temp20 == 0){ _throw( Null_Exception);}
_temp20->tl;});} else{ if( i < 0){ if( curr == 0){ nodes=({ struct Cyc_List_List*
_temp21=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp21->hd=( void*)(( void*)({ struct Cyc_List_List* _temp22= x1; if( _temp22
== 0){ _throw( Null_Exception);} _temp22->hd;})); _temp21->tl= 0; _temp21;});
curr= nodes;} else{({ struct Cyc_List_List* _temp23= curr; if( _temp23 == 0){
_throw( Null_Exception);} _temp23->tl=({ struct Cyc_List_List* _temp24=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp24->hd=( void*)((
void*)({ struct Cyc_List_List* _temp25= x1; if( _temp25 == 0){ _throw(
Null_Exception);} _temp25->hd;})); _temp24->tl= 0; _temp24;});}); curr=({ struct
Cyc_List_List* _temp26= curr; if( _temp26 == 0){ _throw( Null_Exception);}
_temp26->tl;});} x1=({ struct Cyc_List_List* _temp27= x1; if( _temp27 == 0){
_throw( Null_Exception);} _temp27->tl;}); ++ cardinality;} else{ if( curr == 0){
nodes=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp28->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp29= x2; if( _temp29 == 0){ _throw( Null_Exception);} _temp29->hd;}));
_temp28->tl= 0; _temp28;}); curr= nodes;} else{({ struct Cyc_List_List* _temp30=
curr; if( _temp30 == 0){ _throw( Null_Exception);} _temp30->tl=({ struct Cyc_List_List*
_temp31=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp31->hd=( void*)(( void*)({ struct Cyc_List_List* _temp32= x2; if( _temp32
== 0){ _throw( Null_Exception);} _temp32->hd;})); _temp31->tl= 0; _temp31;});});
curr=({ struct Cyc_List_List* _temp33= curr; if( _temp33 == 0){ _throw(
Null_Exception);} _temp33->tl;});} x2=({ struct Cyc_List_List* _temp34= x2; if(
_temp34 == 0){ _throw( Null_Exception);} _temp34->tl;}); ++ cardinality;}}} if(
x1 != 0){ if( curr == 0){ nodes= x1;} else{({ struct Cyc_List_List* _temp37=
curr; if( _temp37 == 0){ _throw( Null_Exception);} _temp37->tl= x1;});}
cardinality +=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( x1);} else{
if( x2 != 0){ if( curr == 0){ nodes= x2;} else{({ struct Cyc_List_List* _temp38=
curr; if( _temp38 == 0){ _throw( Null_Exception);} _temp38->tl= x2;});}
cardinality +=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( x2);}}
return({ struct Cyc_Set_Set* _temp39=( struct Cyc_Set_Set*) GC_malloc( sizeof(
struct Cyc_Set_Set)); _temp39->compare= comp; _temp39->cardinality= cardinality;
_temp39->nodes= nodes; _temp39;});}} static struct Cyc_List_List* Cyc_Set_delete_b(
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt){ if( compare((
void*)({ struct Cyc_List_List* _temp40= n; if( _temp40 == 0){ _throw(
Null_Exception);} _temp40->hd;}), elt) == 0){ return({ struct Cyc_List_List*
_temp41= n; if( _temp41 == 0){ _throw( Null_Exception);} _temp41->tl;});}{
struct Cyc_List_List* result=({ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp51->hd=( void*)(( void*)({
struct Cyc_List_List* _temp52= n; if( _temp52 == 0){ _throw( Null_Exception);}
_temp52->hd;})); _temp51->tl= 0; _temp51;}); struct Cyc_List_List* prev= result;
n=({ struct Cyc_List_List* _temp42= n; if( _temp42 == 0){ _throw( Null_Exception);}
_temp42->tl;}); while( n != 0? compare(( void*)({ struct Cyc_List_List* _temp43=
n; if( _temp43 == 0){ _throw( Null_Exception);} _temp43->hd;}), elt) != 0: 0) {({
struct Cyc_List_List* _temp44= prev; if( _temp44 == 0){ _throw( Null_Exception);}
_temp44->tl=({ struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp45->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp46= n; if( _temp46 == 0){ _throw( Null_Exception);} _temp46->hd;}));
_temp45->tl= 0; _temp45;});}); prev=({ struct Cyc_List_List* _temp47= prev; if(
_temp47 == 0){ _throw( Null_Exception);} _temp47->tl;}); n=({ struct Cyc_List_List*
_temp48= n; if( _temp48 == 0){ _throw( Null_Exception);} _temp48->tl;});}({
struct Cyc_List_List* _temp49= prev; if( _temp49 == 0){ _throw( Null_Exception);}
_temp49->tl=({ struct Cyc_List_List* _temp50= n; if( _temp50 == 0){ _throw(
Null_Exception);} _temp50->tl;});}); return result;}} struct Cyc_Set_Set* Cyc_Set_delete(
struct Cyc_Set_Set* s, void* elt){ if((( int(*)( struct Cyc_Set_Set* s, void*
elt)) Cyc_Set_member)( s, elt)){ return({ struct Cyc_Set_Set* _temp53=( struct
Cyc_Set_Set*) GC_malloc( sizeof( struct Cyc_Set_Set)); _temp53->compare= s->compare;
_temp53->cardinality= s->cardinality - 1; _temp53->nodes=(( struct Cyc_List_List*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* n, void* elt)) Cyc_Set_delete_b)(
s->compare, s->nodes, elt); _temp53;});} else{ return s;}} struct Cyc_List_List*
Cyc_Set_elements( struct Cyc_Set_Set* s){ return s->nodes;} void* Cyc_Set_fold(
void*(* f)( void*, void*), struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List*
n= s->nodes; while( n != 0) { accum= f(( void*)({ struct Cyc_List_List* _temp54=
n; if( _temp54 == 0){ _throw( Null_Exception);} _temp54->hd;}), accum); n=({
struct Cyc_List_List* _temp55= n; if( _temp55 == 0){ _throw( Null_Exception);}
_temp55->tl;});} return accum;} void* Cyc_Set_fold_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Set_Set* s, void* accum){ struct Cyc_List_List* n=
s->nodes; while( n != 0) { accum= f( env,( void*)({ struct Cyc_List_List*
_temp56= n; if( _temp56 == 0){ _throw( Null_Exception);} _temp56->hd;}), accum);
n=({ struct Cyc_List_List* _temp57= n; if( _temp57 == 0){ _throw( Null_Exception);}
_temp57->tl;});} return accum;} void Cyc_Set_app( void*(* f)( void*), struct Cyc_Set_Set*
s){(( void(*)( void*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_app)( f, s->nodes);}
void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s){(( void(*)( void(*
f)( void*), struct Cyc_List_List* x)) Cyc_List_iter)( f, s->nodes);} void Cyc_Set_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_Set_Set* s){(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x)) Cyc_List_iter_c)( f, env, s->nodes);}
struct Cyc_Set_Set* Cyc_Set_intersect( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2){ if( s1 == s2){ return s1;}{ int(* comp)( void*, void*)= s1->compare; struct
Cyc_List_List* x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; struct Cyc_List_List*
result= 0; struct Cyc_List_List* prev= 0; int card= 0; if( x1 == 0){ return s1;}
if( x2 == 0){ return s2;} while( x1 != 0? x2 != 0: 0) { int i= comp(( void*)({
struct Cyc_List_List* _temp68= x1; if( _temp68 == 0){ _throw( Null_Exception);}
_temp68->hd;}),( void*)({ struct Cyc_List_List* _temp69= x2; if( _temp69 == 0){
_throw( Null_Exception);} _temp69->hd;})); if( i == 0){ if( result == 0){ result=({
struct Cyc_List_List* _temp58=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp58->hd=( void*)(( void*)({ struct Cyc_List_List* _temp59=
x1; if( _temp59 == 0){ _throw( Null_Exception);} _temp59->hd;})); _temp58->tl= 0;
_temp58;}); prev= result;} else{({ struct Cyc_List_List* _temp60= prev; if(
_temp60 == 0){ _throw( Null_Exception);} _temp60->tl=({ struct Cyc_List_List*
_temp61=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp61->hd=( void*)(( void*)({ struct Cyc_List_List* _temp62= x1; if( _temp62
== 0){ _throw( Null_Exception);} _temp62->hd;})); _temp61->tl= 0; _temp61;});});
prev=({ struct Cyc_List_List* _temp63= prev; if( _temp63 == 0){ _throw(
Null_Exception);} _temp63->tl;});} ++ card; x1=({ struct Cyc_List_List* _temp64=
x1; if( _temp64 == 0){ _throw( Null_Exception);} _temp64->tl;}); x2=({ struct
Cyc_List_List* _temp65= x2; if( _temp65 == 0){ _throw( Null_Exception);} _temp65->tl;});}
else{ if( i < 0){ x1=({ struct Cyc_List_List* _temp66= x1; if( _temp66 == 0){
_throw( Null_Exception);} _temp66->tl;});} else{ x2=({ struct Cyc_List_List*
_temp67= x2; if( _temp67 == 0){ _throw( Null_Exception);} _temp67->tl;});}}}
return({ struct Cyc_Set_Set* _temp70=( struct Cyc_Set_Set*) GC_malloc( sizeof(
struct Cyc_Set_Set)); _temp70->compare= comp; _temp70->cardinality= card;
_temp70->nodes= result; _temp70;});}} struct Cyc_Set_Set* Cyc_Set_from_list( int(*
comp)( void*, void*), struct Cyc_List_List* x){ struct Cyc_List_List* z=((
struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List* x))
Cyc_List_merge_sort)( comp, x);{ struct Cyc_List_List* y= z; for( 0; y != 0; y=({
struct Cyc_List_List* _temp71= y; if( _temp71 == 0){ _throw( Null_Exception);}
_temp71->tl;})){ if(({ struct Cyc_List_List* _temp72= y; if( _temp72 == 0){
_throw( Null_Exception);} _temp72->tl;}) != 0? comp(( void*)({ struct Cyc_List_List*
_temp73= y; if( _temp73 == 0){ _throw( Null_Exception);} _temp73->hd;}),( void*)({
struct Cyc_List_List* _temp75=({ struct Cyc_List_List* _temp74= y; if( _temp74
== 0){ _throw( Null_Exception);} _temp74->tl;}); if( _temp75 == 0){ _throw(
Null_Exception);} _temp75->hd;})) == 0: 0){({ struct Cyc_List_List* _temp76= y;
if( _temp76 == 0){ _throw( Null_Exception);} _temp76->tl=({ struct Cyc_List_List*
_temp78=({ struct Cyc_List_List* _temp77= y; if( _temp77 == 0){ _throw(
Null_Exception);} _temp77->tl;}); if( _temp78 == 0){ _throw( Null_Exception);}
_temp78->tl;});});}}} return({ struct Cyc_Set_Set* _temp79=( struct Cyc_Set_Set*)
GC_malloc( sizeof( struct Cyc_Set_Set)); _temp79->compare= comp; _temp79->cardinality=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( z); _temp79->nodes= z;
_temp79;});} int Cyc_Set_subset( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
int(* comp)( void*, void*)= s1->compare; struct Cyc_List_List* x1= s1->nodes;
struct Cyc_List_List* x2= s2->nodes; while( 1) { if( x1 == 0){ return 1;} if( x2
== 0){ return 0;}{ int i= comp(( void*)({ struct Cyc_List_List* _temp83= x1; if(
_temp83 == 0){ _throw( Null_Exception);} _temp83->hd;}),( void*)({ struct Cyc_List_List*
_temp84= x2; if( _temp84 == 0){ _throw( Null_Exception);} _temp84->hd;})); if( i
== 0){ x1=({ struct Cyc_List_List* _temp80= x1; if( _temp80 == 0){ _throw(
Null_Exception);} _temp80->tl;}); x2=({ struct Cyc_List_List* _temp81= x2; if(
_temp81 == 0){ _throw( Null_Exception);} _temp81->tl;});} else{ if( i > 0){ x2=({
struct Cyc_List_List* _temp82= x2; if( _temp82 == 0){ _throw( Null_Exception);}
_temp82->tl;});} else{ return 0;}}}} return 1;} struct Cyc_Set_Set* Cyc_Set_diff(
struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){ int(* comp)( void*, void*)= s1->compare;
struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List* x2= s2->nodes; int
card= s1->cardinality; if( x2 == 0){ return s1;} while( x2 != 0) { void* elt=(
void*)({ struct Cyc_List_List* _temp86= x2; if( _temp86 == 0){ _throw(
Null_Exception);} _temp86->hd;}); if((( int(*)( int(* compare)( void*, void*),
struct Cyc_List_List* n, void* elt)) Cyc_Set_member_b)( comp, x1, elt)){ -- card;
x1=(( struct Cyc_List_List*(*)( int(* compare)( void*, void*), struct Cyc_List_List*
n, void* elt)) Cyc_Set_delete_b)( comp, x1, elt);} x2=({ struct Cyc_List_List*
_temp85= x2; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl;});}
return({ struct Cyc_Set_Set* _temp87=( struct Cyc_Set_Set*) GC_malloc( sizeof(
struct Cyc_Set_Set)); _temp87->compare= comp; _temp87->cardinality= card;
_temp87->nodes= x1; _temp87;});} int Cyc_Set_compare( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2){ if( s1->cardinality != s2->cardinality){ return s1->cardinality
- s2->cardinality;}{ struct Cyc_List_List* x1= s1->nodes; struct Cyc_List_List*
x2= s2->nodes; int(* comp)( void*, void*)= s1->compare; while( x1 != 0) { int
diff= comp(( void*)({ struct Cyc_List_List* _temp90= x1; if( _temp90 == 0){
_throw( Null_Exception);} _temp90->hd;}),( void*)({ struct Cyc_List_List*
_temp91= x2; if( _temp91 == 0){ _throw( Null_Exception);} _temp91->hd;})); if(
diff != 0){ return diff;} x1=({ struct Cyc_List_List* _temp88= x1; if( _temp88
== 0){ _throw( Null_Exception);} _temp88->tl;}); x2=({ struct Cyc_List_List*
_temp89= x2; if( _temp89 == 0){ _throw( Null_Exception);} _temp89->tl;});}
return 0;}} int Cyc_Set_equals( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2){
return(( int(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_compare)(
s1, s2) == 0;} char Cyc_Set_Absent_tag[ 7u]="Absent"; void* Cyc_Set_choose(
struct Cyc_Set_Set* s){ if( s->nodes == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Set_Absent_struct* _temp92=( struct Cyc_Set_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Set_Absent_struct)); _temp92[ 0]=({ struct Cyc_Set_Absent_struct
_temp93; _temp93.tag= Cyc_Set_Absent_tag; _temp93;}); _temp92;}));} return( void*)({
struct Cyc_List_List* _temp94= s->nodes; if( _temp94 == 0){ _throw(
Null_Exception);} _temp94->hd;});}