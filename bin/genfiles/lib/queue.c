#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Queue_Queue; typedef
struct Cyc_Queue_Queue* Cyc_Queue_gqueue_t; typedef struct Cyc_Queue_Queue* Cyc_Queue_queue_t;
extern int Cyc_Queue_is_empty( struct Cyc_Queue_Queue*); extern unsigned char
Cyc_Queue_Empty[ 10u]; extern struct Cyc_Queue_Queue* Cyc_Queue_create(); extern
void Cyc_Queue_add( struct Cyc_Queue_Queue*, void* x); extern void* Cyc_Queue_take(
struct Cyc_Queue_Queue*); extern void* Cyc_Queue_peek( struct Cyc_Queue_Queue*);
extern void Cyc_Queue_clear( struct Cyc_Queue_Queue*); extern int Cyc_Queue_length(
struct Cyc_Queue_Queue*); extern void Cyc_Queue_iter( void(* f)( void*), struct
Cyc_Queue_Queue*); extern void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue*);
struct Cyc_Queue_Queue{ struct Cyc_List_List* front; struct Cyc_List_List* rear;
} ; int Cyc_Queue_is_empty( struct Cyc_Queue_Queue* q){ return q->front == 0;}
unsigned char Cyc_Queue_Empty[ 10u]="\000\000\000\000Empty"; struct Cyc_Queue_Queue*
Cyc_Queue_create(){ return({ struct Cyc_Queue_Queue* _temp0=( struct Cyc_Queue_Queue*)
GC_malloc( sizeof( struct Cyc_Queue_Queue)); _temp0->front= 0; _temp0->rear= 0;
_temp0;});} void Cyc_Queue_add( struct Cyc_Queue_Queue* q, void* x){ struct Cyc_List_List*
cell=({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1->hd=( void*) x; _temp1->tl= 0; _temp1;}); if( q->front
== 0){ q->front= cell; q->rear= cell;} else{(( struct Cyc_List_List*)
_check_null( q->rear))->tl= cell; q->rear= cell;}} void* Cyc_Queue_take( struct
Cyc_Queue_Queue* q){ if( q->front == 0){( void) _throw(( void*) Cyc_Queue_Empty);}
else{ void* _temp2=( void*)(( struct Cyc_List_List*) _check_null( q->front))->hd;
goto _LL3; _LL3: q->front=(( struct Cyc_List_List*) _check_null( q->front))->tl;
if( q->front == 0){ q->rear= 0;} return _temp2;}} void* Cyc_Queue_peek( struct
Cyc_Queue_Queue* q){ if( q->front == 0){( void) _throw(( void*) Cyc_Queue_Empty);}
else{ return( void*)(( struct Cyc_List_List*) _check_null( q->front))->hd;}}
void Cyc_Queue_clear( struct Cyc_Queue_Queue* q){ q->front= 0; q->rear= 0;} int
Cyc_Queue_length( struct Cyc_Queue_Queue* q){ return(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( q->front);} void Cyc_Queue_iter( void(* f)( void*), struct
Cyc_Queue_Queue* q){ struct Cyc_List_List* x= q->front; for( 0; x != 0; x=((
struct Cyc_List_List*) _check_null( x))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( x))->hd);}} void Cyc_Queue_app( void*(* f)( void*), struct Cyc_Queue_Queue*
q){ struct Cyc_List_List* x= q->front; for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ f(( void*)(( struct Cyc_List_List*) _check_null( x))->hd);}}