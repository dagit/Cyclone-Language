#include "cyc_include.h"

 struct _tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct
_tuple1{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ;
struct _tuple2{ void* f1; void* f2; } ; struct _tuple3{ void* f1; void* f2; void*
f3; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
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
char* tag; } ; extern void* Cyc_List_hd( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_copy( struct Cyc_List_List* src); extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern void Cyc_List_app( void*(* f)( void*), struct Cyc_List_List* x);
extern void Cyc_List_app_c( void*(* f)( void*, void*), void*, struct Cyc_List_List*
x); extern void Cyc_List_app2( void*(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern void Cyc_List_app2_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List* y);
extern void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern
void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern void Cyc_List_iter2( void(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y); extern void Cyc_List_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List* y);
extern void* Cyc_List_fold_left( void*(* f)( void*, void*), void* accum, struct
Cyc_List_List* x); extern void* Cyc_List_fold_left_c( void*(* f)( void*, void*,
void*), void*, void* accum, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern void*
Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void*, struct Cyc_List_List*
x, void* accum); extern struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_flatten( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern void* Cyc_List_nth( struct Cyc_List_List* x, int i);
extern int Cyc_List_forall( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_List_forall_c( int(* pred)( void*, void*), void* env, struct Cyc_List_List*
x); extern int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x);
extern int Cyc_List_exists_c( int(* pred)( void*, void*), void* env, struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct _tuple0 Cyc_List_split( struct Cyc_List_List* x);
extern struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x); extern int Cyc_List_memq(
struct Cyc_List_List* l, void* x); extern int Cyc_List_mem( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); extern void* Cyc_List_assoc( struct
Cyc_List_List* l, void* x); extern void* Cyc_List_assoc_cmp( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_mem_assoc(
struct Cyc_List_List* l, void* x); extern struct Cyc_Core_Opt* Cyc_List_check_unique(
int(* compare)( void*, void*), struct Cyc_List_List* x); extern struct
_tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_from_array( struct _tagged_ptr0 arr); extern struct Cyc_List_List* Cyc_List_tabulate(
int n, void*(* f)( int)); extern struct Cyc_List_List* Cyc_List_tabulate_c( int
n, void*(* f)( void*, int), void* env); extern int Cyc_List_list_cmp( int(* cmp)(
void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); extern int
Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2); extern struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); extern struct Cyc_List_List*
Cyc_List_filter( int(* f)( void*), struct Cyc_List_List* l); int Cyc_List_length(
struct Cyc_List_List* x){ int i= 0; while( x != 0) { ++ i; x= x->tl;} return i;}
char Cyc_List_List_empty_tag[ 11u]="List_empty"; void* Cyc_List_hd( struct Cyc_List_List*
x){ if( x == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_empty_struct*
_temp0=( struct Cyc_List_List_empty_struct*) GC_malloc( sizeof( struct Cyc_List_List_empty_struct));
_temp0[ 0]=({ struct Cyc_List_List_empty_struct _temp1; _temp1.tag= Cyc_List_List_empty_tag;
_temp1;}); _temp0;}));} return( void*) x->hd;} struct Cyc_List_List* Cyc_List_tl(
struct Cyc_List_List* x){ if( x == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_List_List_empty_struct* _temp2=( struct Cyc_List_List_empty_struct*)
GC_malloc( sizeof( struct Cyc_List_List_empty_struct)); _temp2[ 0]=({ struct Cyc_List_List_empty_struct
_temp3; _temp3.tag= Cyc_List_List_empty_tag; _temp3;}); _temp2;}));} return x->tl;}
struct Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* x){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp4=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4->hd=( void*)(( void*) x->hd); _temp4->tl= 0; _temp4;}); prev= result;
for( x= x->tl; x != 0; x= x->tl){ prev->tl=({ struct Cyc_List_List* _temp5=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5->hd=(
void*)(( void*) x->hd); _temp5->tl= 0; _temp5;}); prev= prev->tl;} return result;}
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp6=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp6->hd=( void*) f(( void*) x->hd); _temp6->tl=
0; _temp6;}); prev= result; for( x= x->tl; x != 0; x= x->tl){ struct Cyc_List_List*
temp=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp7->hd=( void*) f(( void*) x->hd); _temp7->tl= 0;
_temp7;}); prev->tl= temp; prev= temp;} return result;} struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp8->hd=( void*) f( env,( void*) x->hd);
_temp8->tl= 0; _temp8;}); prev= result; for( x= x->tl; x != 0; x= x->tl){ prev->tl=({
struct Cyc_List_List* _temp9=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp9->hd=( void*) f( env,( void*) x->hd); _temp9->tl= 0;
_temp9;}); prev= prev->tl;} return result;} char Cyc_List_List_mismatch_tag[ 14u]="List_mismatch";
struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp10=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp10[ 0]=({ struct Cyc_List_List_mismatch_struct _temp11; _temp11.tag= Cyc_List_List_mismatch_tag;
_temp11;}); _temp10;}));} result=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp12->hd=( void*) f(( void*) x->hd,(
void*) y->hd); _temp12->tl= 0; _temp12;}); prev= result; x= x->tl; y= y->tl;
while( x != 0? y != 0: 0) { prev->tl=({ struct Cyc_List_List* _temp13=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp13->hd=( void*)
f(( void*) x->hd,( void*) y->hd); _temp13->tl= 0; _temp13;}); prev= prev->tl; x=
x->tl; y= y->tl;} if( x != 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_List_List_mismatch_struct* _temp14=( struct Cyc_List_List_mismatch_struct*)
GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct)); _temp14[ 0]=({ struct
Cyc_List_List_mismatch_struct _temp15; _temp15.tag= Cyc_List_List_mismatch_tag;
_temp15;}); _temp14;}));} return result;} void Cyc_List_app( void*(* f)( void*),
struct Cyc_List_List* x){ while( x != 0) { f(( void*) x->hd); x= x->tl;}} void
Cyc_List_app_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){
while( x != 0) { f( env,( void*) x->hd); x= x->tl;}} void Cyc_List_iter( void(*
f)( void*), struct Cyc_List_List* x){ while( x != 0) { f(( void*) x->hd); x= x->tl;}}
void Cyc_List_iter_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
x){ while( x != 0) { f( env,( void*) x->hd); x= x->tl;}} void Cyc_List_app2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
while( x != 0? y != 0: 0) { f(( void*) x->hd,( void*) y->hd); x= x->tl; y= y->tl;}
if( x != 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp16=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp16[ 0]=({ struct Cyc_List_List_mismatch_struct _temp17; _temp17.tag= Cyc_List_List_mismatch_tag;
_temp17;}); _temp16;}));}} void Cyc_List_app2_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y
!= 0: 0) { f( env,( void*) x->hd,( void*) y->hd); x= x->tl; y= y->tl;} if( x !=
0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp18=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp18[ 0]=({ struct Cyc_List_List_mismatch_struct _temp19; _temp19.tag= Cyc_List_List_mismatch_tag;
_temp19;}); _temp18;}));}} void Cyc_List_iter2( void(* f)( void*, void*), struct
Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y != 0: 0) { f(( void*)
x->hd,( void*) y->hd); x= x->tl; y= y->tl;} if( x != 0? 1: y != 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp20=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp20[ 0]=({ struct Cyc_List_List_mismatch_struct _temp21; _temp21.tag= Cyc_List_List_mismatch_tag;
_temp21;}); _temp20;}));}} void Cyc_List_iter2_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y
!= 0: 0) { f( env,( void*) x->hd,( void*) y->hd); x= x->tl; y= y->tl;} if( x !=
0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp22=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp22[ 0]=({ struct Cyc_List_List_mismatch_struct _temp23; _temp23.tag= Cyc_List_List_mismatch_tag;
_temp23;}); _temp22;}));}} void* Cyc_List_fold_left( void*(* f)( void*, void*),
void* accum, struct Cyc_List_List* x){ while( x != 0) { accum= f( accum,( void*)
x->hd); x= x->tl;} return accum;} void* Cyc_List_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct Cyc_List_List* x){ while( x != 0) {
accum= f( env, accum,( void*) x->hd); x= x->tl;} return accum;} void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum){ if( x == 0){
return accum;} else{ return f(( void*) x->hd,(( void*(*)( void*(* f)( void*,
void*), struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right)( f, x->tl,
accum));}} void* Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void*
env, struct Cyc_List_List* x, void* accum){ if( x == 0){ return accum;} else{
return f( env,( void*) x->hd,(( void*(*)( void*(* f)( void*, void*, void*), void*
env, struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right_c)( f, env, x->tl,
accum));}} struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x,
struct Cyc_List_List* y){ while( x != 0) { y=({ struct Cyc_List_List* _temp24=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp24->hd=(
void*)(( void*) x->hd); _temp24->tl= y; _temp24;}); x= x->tl;} return y;} struct
Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x){ if( x == 0){ return 0;}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_revappend)( x, 0);} struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x){ if( x == 0){ return x;} else{ struct Cyc_List_List* first= x;
struct Cyc_List_List* second= x->tl; x->tl= 0; while( second != 0) { struct Cyc_List_List*
temp= second->tl; second->tl= first; first= second; second= temp;} return first;}}
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return y;} if( y == 0){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_copy)( x);} result=({ struct Cyc_List_List* _temp25=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp25->hd=( void*)(( void*) x->hd);
_temp25->tl= 0; _temp25;}); prev= result; for( x= x->tl; x != 0; x= x->tl){ prev->tl=({
struct Cyc_List_List* _temp26=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp26->hd=( void*)(( void*) x->hd); _temp26->tl= 0; _temp26;});
prev= prev->tl;} prev->tl= y; return result;} struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if(
x == 0){ return y;} if( y == 0){ return x;} for( z= x; z->tl != 0; z= z->tl){;}
z->tl= y; return x;} struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List*
x){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct Cyc_List_List*,
struct Cyc_List_List*), struct Cyc_List_List* x, struct Cyc_List_List* accum))
Cyc_List_fold_right)(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append, x, 0);} struct Cyc_List_List* Cyc_List_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x){ struct Cyc_List_List* a=
0; struct Cyc_List_List* b= 0; if( x == 0? 1: x->tl == 0){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_copy)( x);} while( x != 0) { a=({ struct Cyc_List_List*
_temp27=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp27->hd=( void*)(( void*) x->hd); _temp27->tl= a; _temp27;}); x= x->tl; if(
x != 0){ b=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp28->hd=( void*)(( void*) x->hd); _temp28->tl=
b; _temp28;}); x= x->tl;}} a=(( struct Cyc_List_List*(*)( int(* less_eq)( void*,
void*), struct Cyc_List_List* x)) Cyc_List_merge_sort)( less_eq, a); b=(( struct
Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_merge_sort)(
less_eq, b); return(( struct Cyc_List_List*(*)( int(* less_eq)( void*, void*),
struct Cyc_List_List* a, struct Cyc_List_List* b)) Cyc_List_merge)( less_eq, a,
b);} struct Cyc_List_List* Cyc_List_merge( int(* less_eq)( void*, void*), struct
Cyc_List_List* a, struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List*
d; if( a == 0){ return b;} if( b == 0){ return a;} if( less_eq(( void*) a->hd,(
void*) b->hd) <= 0){ c= a; d= a; a= a->tl;} else{ c= b; d= b; b= b->tl;} while(
a != 0? b != 0: 0) { if( less_eq(( void*) a->hd,( void*) b->hd) <= 0){ c->tl= a;
c= a; a= a->tl;} else{ c->tl= b; c= b; b= b->tl;}} if( a == 0){ c->tl= b;} else{
c->tl= a;} return d;} char Cyc_List_Nth_tag[ 4u]="Nth"; void* Cyc_List_nth(
struct Cyc_List_List* x, int i){ while( i > 0? x != 0: 0) { -- i; x= x->tl;} if(
i < 0? 1: x == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_Nth_struct*
_temp29=( struct Cyc_List_Nth_struct*) GC_malloc( sizeof( struct Cyc_List_Nth_struct));
_temp29[ 0]=({ struct Cyc_List_Nth_struct _temp30; _temp30.tag= Cyc_List_Nth_tag;
_temp30;}); _temp29;}));} return( void*) x->hd;} int Cyc_List_forall( int(* pred)(
void*), struct Cyc_List_List* x){ while( x != 0? pred(( void*) x->hd): 0) { x= x->tl;}
return x == 0;} int Cyc_List_forall_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? pred( env,( void*) x->hd): 0) { x= x->tl;}
return x == 0;} int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x){ while( x != 0? ! pred(( void*) x->hd): 0) { x= x->tl;} return x != 0;} int
Cyc_List_exists_c( int(* pred)( void*, void*), void* env, struct Cyc_List_List*
x){ while( x != 0? ! pred( env,( void*) x->hd): 0) { x= x->tl;} return x != 0;}
struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List*
y){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0? y == 0:
0){ return 0;} if( x == 0? 1: y == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_List_List_mismatch_struct* _temp31=( struct Cyc_List_List_mismatch_struct*)
GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct)); _temp31[ 0]=({ struct
Cyc_List_List_mismatch_struct _temp32; _temp32.tag= Cyc_List_List_mismatch_tag;
_temp32;}); _temp31;}));} result=({ struct Cyc_List_List* _temp33=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp33->hd=( void*)({ struct _tuple2*
_temp34=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp34->f1=(
void*) x->hd; _temp34->f2=( void*) y->hd; _temp34;}); _temp33->tl= 0; _temp33;});
prev= result; x= x->tl; y= y->tl; while( x != 0? y != 0: 0) { prev->tl=({ struct
Cyc_List_List* _temp35=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp35->hd=( void*)({ struct _tuple2* _temp36=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp36->f1=( void*) x->hd; _temp36->f2=( void*) y->hd;
_temp36;}); _temp35->tl= 0; _temp35;}); prev= prev->tl; x= x->tl; y= y->tl;} if(
x != 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp37=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp37[ 0]=({ struct Cyc_List_List_mismatch_struct _temp38; _temp38.tag= Cyc_List_List_mismatch_tag;
_temp38;}); _temp37;}));} return result;} struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List* prev1;
struct Cyc_List_List* result2; struct Cyc_List_List* prev2; if( x == 0){ return({
struct _tuple0 _temp39; _temp39.f1= 0; _temp39.f2= 0; _temp39;});} prev1=(
result1=({ struct Cyc_List_List* _temp40=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp40->hd=( void*)({ struct _tuple2* _temp41=(
struct _tuple2*) x->hd; unsigned int _temp42= 0; if( _temp42 >= 1u){ _throw(
Null_Exception);} _temp41[ _temp42];}).f1; _temp40->tl= 0; _temp40;})); prev2=(
result2=({ struct Cyc_List_List* _temp43=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp43->hd=( void*)({ struct _tuple2* _temp44=(
struct _tuple2*) x->hd; unsigned int _temp45= 0; if( _temp45 >= 1u){ _throw(
Null_Exception);} _temp44[ _temp45];}).f2; _temp43->tl= 0; _temp43;})); for( x=
x->tl; x != 0; x= x->tl){ prev1->tl=({ struct Cyc_List_List* _temp46=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp46->hd=( void*)({
struct _tuple2* _temp47=( struct _tuple2*) x->hd; unsigned int _temp48= 0; if(
_temp48 >= 1u){ _throw( Null_Exception);} _temp47[ _temp48];}).f1; _temp46->tl=
0; _temp46;}); prev2->tl=({ struct Cyc_List_List* _temp49=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp49->hd=( void*)({ struct _tuple2*
_temp50=( struct _tuple2*) x->hd; unsigned int _temp51= 0; if( _temp51 >= 1u){
_throw( Null_Exception);} _temp50[ _temp51];}).f2; _temp49->tl= 0; _temp49;});
prev1= prev1->tl; prev2= prev2->tl;} return({ struct _tuple0 _temp52; _temp52.f1=
result1; _temp52.f2= result2; _temp52;});} struct _tuple1 Cyc_List_split3(
struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; struct Cyc_List_List*
result3; struct Cyc_List_List* prev3; if( x == 0){ return({ struct _tuple1
_temp53; _temp53.f1= 0; _temp53.f2= 0; _temp53.f3= 0; _temp53;});} prev1=(
result1=({ struct Cyc_List_List* _temp54=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp54->hd=( void*)({ struct _tuple3* _temp55=(
struct _tuple3*) x->hd; unsigned int _temp56= 0; if( _temp56 >= 1u){ _throw(
Null_Exception);} _temp55[ _temp56];}).f1; _temp54->tl= 0; _temp54;})); prev2=(
result2=({ struct Cyc_List_List* _temp57=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp57->hd=( void*)({ struct _tuple3* _temp58=(
struct _tuple3*) x->hd; unsigned int _temp59= 0; if( _temp59 >= 1u){ _throw(
Null_Exception);} _temp58[ _temp59];}).f2; _temp57->tl= 0; _temp57;})); prev3=(
result3=({ struct Cyc_List_List* _temp60=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp60->hd=( void*)({ struct _tuple3* _temp61=(
struct _tuple3*) x->hd; unsigned int _temp62= 0; if( _temp62 >= 1u){ _throw(
Null_Exception);} _temp61[ _temp62];}).f3; _temp60->tl= 0; _temp60;})); for( x=
x->tl; x != 0; x= x->tl){ prev1->tl=({ struct Cyc_List_List* _temp63=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp63->hd=( void*)({
struct _tuple3* _temp64=( struct _tuple3*) x->hd; unsigned int _temp65= 0; if(
_temp65 >= 1u){ _throw( Null_Exception);} _temp64[ _temp65];}).f1; _temp63->tl=
0; _temp63;}); prev2->tl=({ struct Cyc_List_List* _temp66=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp66->hd=( void*)({ struct _tuple3*
_temp67=( struct _tuple3*) x->hd; unsigned int _temp68= 0; if( _temp68 >= 1u){
_throw( Null_Exception);} _temp67[ _temp68];}).f2; _temp66->tl= 0; _temp66;});
prev3->tl=({ struct Cyc_List_List* _temp69=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp69->hd=( void*)({ struct _tuple3* _temp70=(
struct _tuple3*) x->hd; unsigned int _temp71= 0; if( _temp71 >= 1u){ _throw(
Null_Exception);} _temp70[ _temp71];}).f3; _temp69->tl= 0; _temp69;}); prev1=
prev1->tl; prev2= prev2->tl; prev3= prev3->tl;} return({ struct _tuple1 _temp72;
_temp72.f1= result1; _temp72.f2= result2; _temp72.f3= result3; _temp72;});} int
Cyc_List_memq( struct Cyc_List_List* l, void* x){ while( l != 0) { if(( void*) l->hd
== x){ return 1;} l= l->tl;} return 0;} int Cyc_List_mem( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare(( void*)
l->hd, x) == 0){ return 1;} l= l->tl;} return 0;} void* Cyc_List_assoc( struct
Cyc_List_List* l, void* x){ while( l != 0) { if(({ struct _tuple2* _temp73=(
struct _tuple2*) l->hd; unsigned int _temp74= 0; if( _temp74 >= 1u){ _throw(
Null_Exception);} _temp73[ _temp74];}).f1 == x){ return({ struct _tuple2*
_temp75=( struct _tuple2*) l->hd; unsigned int _temp76= 0; if( _temp76 >= 1u){
_throw( Null_Exception);} _temp75[ _temp76];}).f2;} l= l->tl;}( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp77=( struct
Cyc_Core_Not_found_struct*) GC_malloc( sizeof( struct Cyc_Core_Not_found_struct));
_temp77[ 0]=({ struct Cyc_Core_Not_found_struct _temp78; _temp78.tag= Cyc_Core_Not_found_tag;
_temp78;}); _temp77;}));} void* Cyc_List_assoc_cmp( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare( x,({ struct
_tuple2* _temp79=( struct _tuple2*) l->hd; unsigned int _temp80= 0; if( _temp80
>= 1u){ _throw( Null_Exception);} _temp79[ _temp80];}).f1) == 0){ return({
struct _tuple2* _temp81=( struct _tuple2*) l->hd; unsigned int _temp82= 0; if(
_temp82 >= 1u){ _throw( Null_Exception);} _temp81[ _temp82];}).f2;} l= l->tl;}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Not_found_struct*
_temp83=( struct Cyc_Core_Not_found_struct*) GC_malloc( sizeof( struct Cyc_Core_Not_found_struct));
_temp83[ 0]=({ struct Cyc_Core_Not_found_struct _temp84; _temp84.tag= Cyc_Core_Not_found_tag;
_temp84;}); _temp83;}));} int Cyc_List_mem_assoc( struct Cyc_List_List* l, void*
x){ while( l != 0) { if(({ struct _tuple2* _temp85=( struct _tuple2*) l->hd;
unsigned int _temp86= 0; if( _temp86 >= 1u){ _throw( Null_Exception);} _temp85[
_temp86];}).f1 == x){ return 1;} l= l->tl;} return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique(
int(* compare)( void*, void*), struct Cyc_List_List* x){ while( x != 0) { if( x->tl
!= 0){ if( compare(( void*) x->hd,( void*)( x->tl)->hd) == 0){ return({ struct
Cyc_Core_Opt* _temp87=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp87->v=( void*)(( void*) x->hd); _temp87;});}} x= x->tl;} return 0;} struct
_tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x){ int s; struct
_tagged_ptr0 arr; s=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( x);
arr=({ unsigned int _temp88=( unsigned int) s; void** _temp89=( void**)
GC_malloc( sizeof( void*) * _temp88); unsigned int i; struct _tagged_ptr0
_temp90={ _temp89, _temp89, _temp89 + _temp88}; for( i= 0; i < _temp88; i ++){
_temp89[ i]=( void*) x->hd;} _temp90;});{ int i= 0; for( 0; i < s; ++ i, x= x->tl){({
struct _tagged_ptr0 _temp91= arr; void** _temp93= _temp91.curr + i; if( _temp93
< _temp91.base? 1: _temp93 >= _temp91.last_plus_one){ _throw( Null_Exception);}*
_temp93=( void*) x->hd;});}} return arr;} struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{ unsigned int i=({
struct _tagged_ptr0 _temp94= arr;( unsigned int)( _temp94.last_plus_one -
_temp94.curr);}) -( unsigned int) 1; for( 0; i >=( unsigned int) 0; -- i){ ans=({
struct Cyc_List_List* _temp95=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp95->hd=( void*)({ struct _tagged_ptr0 _temp96= arr; void**
_temp98= _temp96.curr +( int) i; if( _temp98 < _temp96.base? 1: _temp98 >=
_temp96.last_plus_one){ _throw( Null_Exception);}* _temp98;}); _temp95->tl= ans;
_temp95;});}} return ans;} struct Cyc_List_List* Cyc_List_tabulate( int n, void*(*
f)( int)){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({
struct Cyc_List_List* _temp99=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp99->hd=( void*) f( i); _temp99->tl= res; _temp99;});}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
res);} struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int),
void* env){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({
struct Cyc_List_List* _temp100=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp100->hd=( void*) f( env, i); _temp100->tl= res;
_temp100;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
res);} int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1= l1->tl, l2= l2->tl){
if( l1 == l2){ return 0;}{ int i= cmp(( void*) l1->hd,( void*) l2->hd); if( i !=
0){ return i;}}} if( l1 != 0){ return 1;} if( l2 != 0){ return - 1;} return 0;}
int Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1= l1->tl, l2= l2->tl){
if( l1 == l2){ return 1;}{ int i= cmp(( void*) l1->hd,( void*) l2->hd); if( i !=
0){ return 0;}}} return l1 == 0;} struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l){ if( l == 0){ return 0;}{
struct Cyc_List_List* result=({ struct Cyc_List_List* _temp101=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp101->hd=( void*)(( void*) l->hd);
_temp101->tl= 0; _temp101;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l= l->tl){ if( f( env,( void*) l->hd)){ end->tl=({ struct Cyc_List_List*
_temp102=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp102->hd=( void*)(( void*) l->hd); _temp102->tl= 0; _temp102;}); end= end->tl;}}
return result->tl;}} struct Cyc_List_List* Cyc_List_filter( int(* f)( void*),
struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({
struct Cyc_List_List* _temp103=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp103->hd=( void*)(( void*) l->hd); _temp103->tl= 0;
_temp103;}); struct Cyc_List_List* end= result; for( 0; l != 0; l= l->tl){ if( f((
void*) l->hd)){ end->tl=({ struct Cyc_List_List* _temp104=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp104->hd=( void*)(( void*) l->hd);
_temp104->tl= 0; _temp104;}); end= end->tl;}} return result->tl;}}