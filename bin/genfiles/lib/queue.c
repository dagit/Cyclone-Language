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
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; struct Cyc_Queue_Queue; typedef struct Cyc_Queue_Queue* Cyc_Queue_gqueue_t;
typedef struct Cyc_Queue_Queue* Cyc_Queue_queue_t; extern int Cyc_Queue_is_empty(
struct Cyc_Queue_Queue*); extern char Cyc_Queue_Empty_tag[ 6u]; struct Cyc_Queue_Empty_struct{
char* tag; } ; extern struct Cyc_Queue_Queue* Cyc_Queue_create(); extern void
Cyc_Queue_add( struct Cyc_Queue_Queue*, void* x); extern void* Cyc_Queue_take(
struct Cyc_Queue_Queue*); extern void* Cyc_Queue_peek( struct Cyc_Queue_Queue*);
extern void Cyc_Queue_clear( struct Cyc_Queue_Queue*); extern int Cyc_Queue_length(
struct Cyc_Queue_Queue*); extern void Cyc_Queue_iter( void(* f)( void*), struct
Cyc_Queue_Queue*); extern void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue*);
struct Cyc_Queue_Queue{ struct Cyc_List_List* front; struct Cyc_List_List* rear;
} ; int Cyc_Queue_is_empty( struct Cyc_Queue_Queue* q){ return q->front == 0;}
char Cyc_Queue_Empty_tag[ 6u]="Empty"; struct Cyc_Queue_Queue* Cyc_Queue_create(){
return({ struct Cyc_Queue_Queue* _temp0=( struct Cyc_Queue_Queue*) GC_malloc(
sizeof( struct Cyc_Queue_Queue)); _temp0->front= 0; _temp0->rear= 0; _temp0;});}
void Cyc_Queue_add( struct Cyc_Queue_Queue* q, void* x){ struct Cyc_List_List*
cell=({ struct Cyc_List_List* _temp2=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2->hd=( void*) x; _temp2->tl= 0; _temp2;}); if( q->front
== 0){ q->front= cell; q->rear= cell;} else{({ struct Cyc_List_List* _temp1= q->rear;
if( _temp1 == 0){ _throw( Null_Exception);} _temp1->tl= cell;}); q->rear= cell;}}
void* Cyc_Queue_take( struct Cyc_Queue_Queue* q){ if( q->front == 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Queue_Empty_struct* _temp3=(
struct Cyc_Queue_Empty_struct*) GC_malloc( sizeof( struct Cyc_Queue_Empty_struct));
_temp3[ 0]=({ struct Cyc_Queue_Empty_struct _temp4; _temp4.tag= Cyc_Queue_Empty_tag;
_temp4;}); _temp3;}));} else{ void* x=( void*)({ struct Cyc_List_List* _temp6= q->front;
if( _temp6 == 0){ _throw( Null_Exception);} _temp6->hd;}); q->front=({ struct
Cyc_List_List* _temp5= q->front; if( _temp5 == 0){ _throw( Null_Exception);}
_temp5->tl;}); if( q->front == 0){ q->rear= 0;} return x;}} void* Cyc_Queue_peek(
struct Cyc_Queue_Queue* q){ if( q->front == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Queue_Empty_struct* _temp7=( struct Cyc_Queue_Empty_struct*)
GC_malloc( sizeof( struct Cyc_Queue_Empty_struct)); _temp7[ 0]=({ struct Cyc_Queue_Empty_struct
_temp8; _temp8.tag= Cyc_Queue_Empty_tag; _temp8;}); _temp7;}));} else{ return(
void*)({ struct Cyc_List_List* _temp9= q->front; if( _temp9 == 0){ _throw(
Null_Exception);} _temp9->hd;});}} void Cyc_Queue_clear( struct Cyc_Queue_Queue*
q){ q->front= 0; q->rear= 0;} int Cyc_Queue_length( struct Cyc_Queue_Queue* q){
return(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( q->front);} void
Cyc_Queue_iter( void(* f)( void*), struct Cyc_Queue_Queue* q){ struct Cyc_List_List*
x= q->front; for( 0; x != 0; x=({ struct Cyc_List_List* _temp10= x; if( _temp10
== 0){ _throw( Null_Exception);} _temp10->tl;})){ f(( void*)({ struct Cyc_List_List*
_temp11= x; if( _temp11 == 0){ _throw( Null_Exception);} _temp11->hd;}));}} void
Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue* q){ struct Cyc_List_List*
x= q->front; for( 0; x != 0; x=({ struct Cyc_List_List* _temp12= x; if( _temp12
== 0){ _throw( Null_Exception);} _temp12->tl;})){ f(( void*)({ struct Cyc_List_List*
_temp13= x; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->hd;}));}}