#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
i; x=({ struct Cyc_List_List* _temp0= x; if( _temp0 == 0){ _throw(
Null_Exception);} _temp0->tl;});} return i;} char Cyc_List_List_empty[ 15u]="\000\000\000\000List_empty";
void* Cyc_List_hd( struct Cyc_List_List* x){ if( x == 0){( void) _throw(( void*)
Cyc_List_List_empty);} return( void*)({ struct Cyc_List_List* _temp1= x; if(
_temp1 == 0){ _throw( Null_Exception);} _temp1->hd;});} struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x){ if( x == 0){( void) _throw(( void*) Cyc_List_List_empty);}
return({ struct Cyc_List_List* _temp2= x; if( _temp2 == 0){ _throw(
Null_Exception);} _temp2->tl;});} struct Cyc_List_List* Cyc_List_rcopy( struct
_RegionHandle* r2, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp3=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp3->hd=( void*)(( void*)({ struct Cyc_List_List* _temp4= x; if( _temp4 == 0){
_throw( Null_Exception);} _temp4->hd;})); _temp3->tl= 0; _temp3;}); prev= result;
for( x=({ struct Cyc_List_List* _temp5= x; if( _temp5 == 0){ _throw(
Null_Exception);} _temp5->tl;}); x != 0; x=({ struct Cyc_List_List* _temp6= x;
if( _temp6 == 0){ _throw( Null_Exception);} _temp6->tl;})){({ struct Cyc_List_List*
_temp7= prev; if( _temp7 == 0){ _throw( Null_Exception);} _temp7->tl=({ struct
Cyc_List_List* _temp8=( struct Cyc_List_List*) _region_malloc( r2, sizeof(
struct Cyc_List_List)); _temp8->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp9= x; if( _temp9 == 0){ _throw( Null_Exception);} _temp9->hd;})); _temp8->tl=
0; _temp8;});}); prev=({ struct Cyc_List_List* _temp10= prev; if( _temp10 == 0){
_throw( Null_Exception);} _temp10->tl;});} return result;} struct Cyc_List_List*
Cyc_List_copy( struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( Cyc_Core_heap_region,
x);} struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle* r2, void*(* f)(
void*), struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp11=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp11->hd=(
void*) f(( void*)({ struct Cyc_List_List* _temp12= x; if( _temp12 == 0){ _throw(
Null_Exception);} _temp12->hd;})); _temp11->tl= 0; _temp11;}); prev= result;
for( x=({ struct Cyc_List_List* _temp13= x; if( _temp13 == 0){ _throw(
Null_Exception);} _temp13->tl;}); x != 0; x=({ struct Cyc_List_List* _temp14= x;
if( _temp14 == 0){ _throw( Null_Exception);} _temp14->tl;})){ struct Cyc_List_List*
temp=({ struct Cyc_List_List* _temp16=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp16->hd=( void*) f(( void*)({ struct Cyc_List_List*
_temp17= x; if( _temp17 == 0){ _throw( Null_Exception);} _temp17->hd;}));
_temp16->tl= 0; _temp16;});({ struct Cyc_List_List* _temp15= prev; if( _temp15
== 0){ _throw( Null_Exception);} _temp15->tl= temp;}); prev= temp;} return
result;} struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, void*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_rmap)( Cyc_Core_heap_region, f, x);}
struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle* r2, void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp18=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp18->hd=( void*) f( env,( void*)({ struct Cyc_List_List* _temp19= x; if(
_temp19 == 0){ _throw( Null_Exception);} _temp19->hd;})); _temp18->tl= 0;
_temp18;}); prev= result; for( x=({ struct Cyc_List_List* _temp20= x; if(
_temp20 == 0){ _throw( Null_Exception);} _temp20->tl;}); x != 0; x=({ struct Cyc_List_List*
_temp21= x; if( _temp21 == 0){ _throw( Null_Exception);} _temp21->tl;})){({
struct Cyc_List_List* _temp22= prev; if( _temp22 == 0){ _throw( Null_Exception);}
_temp22->tl=({ struct Cyc_List_List* _temp23=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp23->hd=( void*) f( env,(
void*)({ struct Cyc_List_List* _temp24= x; if( _temp24 == 0){ _throw(
Null_Exception);} _temp24->hd;})); _temp23->tl= 0; _temp23;});}); prev=({ struct
Cyc_List_List* _temp25= prev; if( _temp25 == 0){ _throw( Null_Exception);}
_temp25->tl;});} return result;} struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( Cyc_Core_heap_region, f, env, x);} char Cyc_List_List_mismatch[
18u]="\000\000\000\000List_mismatch"; struct Cyc_List_List* Cyc_List_rmap2(
struct _RegionHandle* r3, void*(* f)( void*, void*), struct Cyc_List_List* x,
struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
void*) Cyc_List_List_mismatch);} result=({ struct Cyc_List_List* _temp26=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp26->hd=( void*) f(( void*)({ struct Cyc_List_List* _temp27= x; if( _temp27
== 0){ _throw( Null_Exception);} _temp27->hd;}),( void*)({ struct Cyc_List_List*
_temp28= y; if( _temp28 == 0){ _throw( Null_Exception);} _temp28->hd;}));
_temp26->tl= 0; _temp26;}); prev= result; x=({ struct Cyc_List_List* _temp29= x;
if( _temp29 == 0){ _throw( Null_Exception);} _temp29->tl;}); y=({ struct Cyc_List_List*
_temp30= y; if( _temp30 == 0){ _throw( Null_Exception);} _temp30->tl;}); while(
x != 0? y != 0: 0) {({ struct Cyc_List_List* _temp31= prev; if( _temp31 == 0){
_throw( Null_Exception);} _temp31->tl=({ struct Cyc_List_List* _temp32=( struct
Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List)); _temp32->hd=(
void*) f(( void*)({ struct Cyc_List_List* _temp33= x; if( _temp33 == 0){ _throw(
Null_Exception);} _temp33->hd;}),( void*)({ struct Cyc_List_List* _temp34= y;
if( _temp34 == 0){ _throw( Null_Exception);} _temp34->hd;})); _temp32->tl= 0;
_temp32;});}); prev=({ struct Cyc_List_List* _temp35= prev; if( _temp35 == 0){
_throw( Null_Exception);} _temp35->tl;}); x=({ struct Cyc_List_List* _temp36= x;
if( _temp36 == 0){ _throw( Null_Exception);} _temp36->tl;}); y=({ struct Cyc_List_List*
_temp37= y; if( _temp37 == 0){ _throw( Null_Exception);} _temp37->tl;});} if( x
!= 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);} return result;}
struct Cyc_List_List* Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List*
x, struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, void*(* f)( void*, void*), struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rmap2)( Cyc_Core_heap_region, f, x, y);} void Cyc_List_app(
void*(* f)( void*), struct Cyc_List_List* x){ while( x != 0) { f(( void*)({
struct Cyc_List_List* _temp38= x; if( _temp38 == 0){ _throw( Null_Exception);}
_temp38->hd;})); x=({ struct Cyc_List_List* _temp39= x; if( _temp39 == 0){
_throw( Null_Exception);} _temp39->tl;});}} void Cyc_List_app_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0) { f( env,(
void*)({ struct Cyc_List_List* _temp40= x; if( _temp40 == 0){ _throw(
Null_Exception);} _temp40->hd;})); x=({ struct Cyc_List_List* _temp41= x; if(
_temp41 == 0){ _throw( Null_Exception);} _temp41->tl;});}} void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x){ while( x != 0) { f(( void*)({
struct Cyc_List_List* _temp42= x; if( _temp42 == 0){ _throw( Null_Exception);}
_temp42->hd;})); x=({ struct Cyc_List_List* _temp43= x; if( _temp43 == 0){
_throw( Null_Exception);} _temp43->tl;});}} void Cyc_List_iter_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0) { f( env,(
void*)({ struct Cyc_List_List* _temp44= x; if( _temp44 == 0){ _throw(
Null_Exception);} _temp44->hd;})); x=({ struct Cyc_List_List* _temp45= x; if(
_temp45 == 0){ _throw( Null_Exception);} _temp45->tl;});}} void Cyc_List_app2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
while( x != 0? y != 0: 0) { f(( void*)({ struct Cyc_List_List* _temp46= x; if(
_temp46 == 0){ _throw( Null_Exception);} _temp46->hd;}),( void*)({ struct Cyc_List_List*
_temp47= y; if( _temp47 == 0){ _throw( Null_Exception);} _temp47->hd;})); x=({
struct Cyc_List_List* _temp48= x; if( _temp48 == 0){ _throw( Null_Exception);}
_temp48->tl;}); y=({ struct Cyc_List_List* _temp49= y; if( _temp49 == 0){ _throw(
Null_Exception);} _temp49->tl;});} if( x != 0? 1: y != 0){( void) _throw(( void*)
Cyc_List_List_mismatch);}} void Cyc_List_app2_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y
!= 0: 0) { f( env,( void*)({ struct Cyc_List_List* _temp50= x; if( _temp50 == 0){
_throw( Null_Exception);} _temp50->hd;}),( void*)({ struct Cyc_List_List*
_temp51= y; if( _temp51 == 0){ _throw( Null_Exception);} _temp51->hd;})); x=({
struct Cyc_List_List* _temp52= x; if( _temp52 == 0){ _throw( Null_Exception);}
_temp52->tl;}); y=({ struct Cyc_List_List* _temp53= y; if( _temp53 == 0){ _throw(
Null_Exception);} _temp53->tl;});} if( x != 0? 1: y != 0){( void) _throw(( void*)
Cyc_List_List_mismatch);}} void Cyc_List_iter2( void(* f)( void*, void*), struct
Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y != 0: 0) { f(( void*)({
struct Cyc_List_List* _temp54= x; if( _temp54 == 0){ _throw( Null_Exception);}
_temp54->hd;}),( void*)({ struct Cyc_List_List* _temp55= y; if( _temp55 == 0){
_throw( Null_Exception);} _temp55->hd;})); x=({ struct Cyc_List_List* _temp56= x;
if( _temp56 == 0){ _throw( Null_Exception);} _temp56->tl;}); y=({ struct Cyc_List_List*
_temp57= y; if( _temp57 == 0){ _throw( Null_Exception);} _temp57->tl;});} if( x
!= 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);}} void Cyc_List_iter2_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_List_List* x, struct Cyc_List_List*
y){ while( x != 0? y != 0: 0) { f( env,( void*)({ struct Cyc_List_List* _temp58=
x; if( _temp58 == 0){ _throw( Null_Exception);} _temp58->hd;}),( void*)({ struct
Cyc_List_List* _temp59= y; if( _temp59 == 0){ _throw( Null_Exception);} _temp59->hd;}));
x=({ struct Cyc_List_List* _temp60= x; if( _temp60 == 0){ _throw( Null_Exception);}
_temp60->tl;}); y=({ struct Cyc_List_List* _temp61= y; if( _temp61 == 0){ _throw(
Null_Exception);} _temp61->tl;});} if( x != 0? 1: y != 0){( void) _throw(( void*)
Cyc_List_List_mismatch);}} void* Cyc_List_fold_left( void*(* f)( void*, void*),
void* accum, struct Cyc_List_List* x){ while( x != 0) { accum= f( accum,( void*)({
struct Cyc_List_List* _temp62= x; if( _temp62 == 0){ _throw( Null_Exception);}
_temp62->hd;})); x=({ struct Cyc_List_List* _temp63= x; if( _temp63 == 0){
_throw( Null_Exception);} _temp63->tl;});} return accum;} void* Cyc_List_fold_left_c(
void*(* f)( void*, void*, void*), void* env, void* accum, struct Cyc_List_List*
x){ while( x != 0) { accum= f( env, accum,( void*)({ struct Cyc_List_List*
_temp64= x; if( _temp64 == 0){ _throw( Null_Exception);} _temp64->hd;})); x=({
struct Cyc_List_List* _temp65= x; if( _temp65 == 0){ _throw( Null_Exception);}
_temp65->tl;});} return accum;} void* Cyc_List_fold_right( void*(* f)( void*,
void*), struct Cyc_List_List* x, void* accum){ if( x == 0){ return accum;} else{
return f(( void*)({ struct Cyc_List_List* _temp66= x; if( _temp66 == 0){ _throw(
Null_Exception);} _temp66->hd;}),(( void*(*)( void*(* f)( void*, void*), struct
Cyc_List_List* x, void* accum)) Cyc_List_fold_right)( f,({ struct Cyc_List_List*
_temp67= x; if( _temp67 == 0){ _throw( Null_Exception);} _temp67->tl;}), accum));}}
void* Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void* env, struct
Cyc_List_List* x, void* accum){ if( x == 0){ return accum;} else{ return f( env,(
void*)({ struct Cyc_List_List* _temp68= x; if( _temp68 == 0){ _throw(
Null_Exception);} _temp68->hd;}),(( void*(*)( void*(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right_c)( f, env,({
struct Cyc_List_List* _temp69= x; if( _temp69 == 0){ _throw( Null_Exception);}
_temp69->tl;}), accum));}} struct Cyc_List_List* Cyc_List_rrevappend( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x
!= 0) { y=({ struct Cyc_List_List* _temp70=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp70->hd=( void*)(( void*)({
struct Cyc_List_List* _temp71= x; if( _temp71 == 0){ _throw( Null_Exception);}
_temp71->hd;})); _temp70->tl= y; _temp70;}); x=({ struct Cyc_List_List* _temp72=
x; if( _temp72 == 0){ _throw( Null_Exception);} _temp72->tl;});} return y;}
struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List* x, struct Cyc_List_List*
y){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rrevappend)( Cyc_Core_heap_region, x, y);}
struct Cyc_List_List* Cyc_List_rrev( struct _RegionHandle* r2, struct Cyc_List_List*
x){ if( x == 0){ return 0;} return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rrevappend)(
r2, x, 0);} struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x))
Cyc_List_rrev)( Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x){ if( x == 0){ return x;} else{ struct Cyc_List_List*
first= x; struct Cyc_List_List* second=({ struct Cyc_List_List* _temp76= x; if(
_temp76 == 0){ _throw( Null_Exception);} _temp76->tl;});({ struct Cyc_List_List*
_temp73= x; if( _temp73 == 0){ _throw( Null_Exception);} _temp73->tl= 0;});
while( second != 0) { struct Cyc_List_List* temp=({ struct Cyc_List_List*
_temp75= second; if( _temp75 == 0){ _throw( Null_Exception);} _temp75->tl;});({
struct Cyc_List_List* _temp74= second; if( _temp74 == 0){ _throw( Null_Exception);}
_temp74->tl= first;}); first= second; second= temp;} return first;}} struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return y;} if( y == 0){ return(( struct Cyc_List_List*(*)( struct _RegionHandle*
r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r2, x);} result=({ struct Cyc_List_List*
_temp77=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp77->hd=( void*)(( void*)({ struct Cyc_List_List* _temp78= x; if( _temp78 ==
0){ _throw( Null_Exception);} _temp78->hd;})); _temp77->tl= 0; _temp77;}); prev=
result; for( x=({ struct Cyc_List_List* _temp79= x; if( _temp79 == 0){ _throw(
Null_Exception);} _temp79->tl;}); x != 0; x=({ struct Cyc_List_List* _temp80= x;
if( _temp80 == 0){ _throw( Null_Exception);} _temp80->tl;})){({ struct Cyc_List_List*
_temp81= prev; if( _temp81 == 0){ _throw( Null_Exception);} _temp81->tl=({
struct Cyc_List_List* _temp82=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp82->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp83= x; if( _temp83 == 0){ _throw( Null_Exception);} _temp83->hd;}));
_temp82->tl= 0; _temp82;});}); prev=({ struct Cyc_List_List* _temp84= prev; if(
_temp84 == 0){ _throw( Null_Exception);} _temp84->tl;});}({ struct Cyc_List_List*
_temp85= prev; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl= y;});
return result;} struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
Cyc_Core_heap_region, x, y);} struct Cyc_List_List* Cyc_List_imp_append( struct
Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if( x == 0){
return y;} if( y == 0){ return x;} for( z= x;({ struct Cyc_List_List* _temp86= z;
if( _temp86 == 0){ _throw( Null_Exception);} _temp86->tl;}) != 0; z=({ struct
Cyc_List_List* _temp87= z; if( _temp87 == 0){ _throw( Null_Exception);} _temp87->tl;})){;}({
struct Cyc_List_List* _temp88= z; if( _temp88 == 0){ _throw( Null_Exception);}
_temp88->tl= y;}); return x;} struct Cyc_List_List* Cyc_List_rflatten( struct
_RegionHandle* r3, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _RegionHandle*, struct Cyc_List_List*, struct
Cyc_List_List*), struct _RegionHandle* env, struct Cyc_List_List* x, struct Cyc_List_List*
accum)) Cyc_List_fold_right_c)(( struct Cyc_List_List*(*)( struct _RegionHandle*
r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend, r3, x,
0);} struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct Cyc_List_List* x))
Cyc_List_rflatten)( Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_imp_merge(
int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b){
struct Cyc_List_List* c; struct Cyc_List_List* d; if( a == 0){ return b;} if( b
== 0){ return a;} if( less_eq(( void*)({ struct Cyc_List_List* _temp89= a; if(
_temp89 == 0){ _throw( Null_Exception);} _temp89->hd;}),( void*)({ struct Cyc_List_List*
_temp90= b; if( _temp90 == 0){ _throw( Null_Exception);} _temp90->hd;})) <= 0){
c= a; a=({ struct Cyc_List_List* _temp91= a; if( _temp91 == 0){ _throw(
Null_Exception);} _temp91->tl;});} else{ c= b; b=({ struct Cyc_List_List*
_temp92= b; if( _temp92 == 0){ _throw( Null_Exception);} _temp92->tl;});} d= c;
while( a != 0? b != 0: 0) { if( less_eq(( void*)({ struct Cyc_List_List* _temp93=
a; if( _temp93 == 0){ _throw( Null_Exception);} _temp93->hd;}),( void*)({ struct
Cyc_List_List* _temp94= b; if( _temp94 == 0){ _throw( Null_Exception);} _temp94->hd;}))
<= 0){({ struct Cyc_List_List* _temp95= c; if( _temp95 == 0){ _throw(
Null_Exception);} _temp95->tl= a;}); c= a; a=({ struct Cyc_List_List* _temp96= a;
if( _temp96 == 0){ _throw( Null_Exception);} _temp96->tl;});} else{({ struct Cyc_List_List*
_temp97= c; if( _temp97 == 0){ _throw( Null_Exception);} _temp97->tl= b;}); c= b;
b=({ struct Cyc_List_List* _temp98= b; if( _temp98 == 0){ _throw( Null_Exception);}
_temp98->tl;});}} if( a == 0){({ struct Cyc_List_List* _temp99= c; if( _temp99
== 0){ _throw( Null_Exception);} _temp99->tl= b;});} else{({ struct Cyc_List_List*
_temp100= c; if( _temp100 == 0){ _throw( Null_Exception);} _temp100->tl= a;});}
return d;} struct Cyc_List_List* Cyc_List_rimp_merge_sort( int(* less_eq)( void*,
void*), struct Cyc_List_List* x){ if( x == 0? 1:({ struct Cyc_List_List*
_temp101= x; if( _temp101 == 0){ _throw( Null_Exception);} _temp101->tl;}) == 0){
return x;}{ struct Cyc_List_List* a= x; struct Cyc_List_List* aptr= a; struct
Cyc_List_List* b=({ struct Cyc_List_List* _temp109= x; if( _temp109 == 0){
_throw( Null_Exception);} _temp109->tl;}); struct Cyc_List_List* bptr= b; x=({
struct Cyc_List_List* _temp102= b; if( _temp102 == 0){ _throw( Null_Exception);}
_temp102->tl;}); while( x != 0) {({ struct Cyc_List_List* _temp103= aptr; if(
_temp103 == 0){ _throw( Null_Exception);} _temp103->tl= x;}); aptr= x; x=({
struct Cyc_List_List* _temp104= x; if( _temp104 == 0){ _throw( Null_Exception);}
_temp104->tl;}); if( x != 0){({ struct Cyc_List_List* _temp105= bptr; if(
_temp105 == 0){ _throw( Null_Exception);} _temp105->tl= x;}); bptr= x; x=({
struct Cyc_List_List* _temp106= x; if( _temp106 == 0){ _throw( Null_Exception);}
_temp106->tl;});}}({ struct Cyc_List_List* _temp107= aptr; if( _temp107 == 0){
_throw( Null_Exception);} _temp107->tl= 0;});({ struct Cyc_List_List* _temp108=
bptr; if( _temp108 == 0){ _throw( Null_Exception);} _temp108->tl= 0;}); return((
struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b)) Cyc_List_imp_merge)( less_eq,(( struct Cyc_List_List*(*)(
int(* less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_rimp_merge_sort)(
less_eq, a),(( struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct
Cyc_List_List* x)) Cyc_List_rimp_merge_sort)( less_eq, b));}} struct Cyc_List_List*
Cyc_List_rmerge_sort( struct _RegionHandle* r2, int(* less_eq)( void*, void*),
struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( int(* less_eq)(
void*, void*), struct Cyc_List_List* x)) Cyc_List_rimp_merge_sort)( less_eq,((
struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x))
Cyc_List_rcopy)( r2, x));} struct Cyc_List_List* Cyc_List_rmerge( struct
_RegionHandle* r3, int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if(
a == 0){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);} if( b == 0){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);}
if( less_eq(( void*)({ struct Cyc_List_List* _temp110= a; if( _temp110 == 0){
_throw( Null_Exception);} _temp110->hd;}),( void*)({ struct Cyc_List_List*
_temp111= b; if( _temp111 == 0){ _throw( Null_Exception);} _temp111->hd;})) <= 0){
c=({ struct Cyc_List_List* _temp112=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp112->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp113= a; if( _temp113 == 0){ _throw( Null_Exception);} _temp113->hd;}));
_temp112->tl= 0; _temp112;}); a=({ struct Cyc_List_List* _temp114= a; if(
_temp114 == 0){ _throw( Null_Exception);} _temp114->tl;});} else{ c=({ struct
Cyc_List_List* _temp115=( struct Cyc_List_List*) _region_malloc( r3, sizeof(
struct Cyc_List_List)); _temp115->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp116= b; if( _temp116 == 0){ _throw( Null_Exception);} _temp116->hd;}));
_temp115->tl= 0; _temp115;}); b=({ struct Cyc_List_List* _temp117= b; if(
_temp117 == 0){ _throw( Null_Exception);} _temp117->tl;});} d= c; while( a != 0?
b != 0: 0) { if( less_eq(( void*)({ struct Cyc_List_List* _temp118= a; if(
_temp118 == 0){ _throw( Null_Exception);} _temp118->hd;}),( void*)({ struct Cyc_List_List*
_temp119= b; if( _temp119 == 0){ _throw( Null_Exception);} _temp119->hd;})) <= 0){({
struct Cyc_List_List* _temp120= c; if( _temp120 == 0){ _throw( Null_Exception);}
_temp120->tl=({ struct Cyc_List_List* _temp121=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp121->hd=( void*)(( void*)({
struct Cyc_List_List* _temp122= a; if( _temp122 == 0){ _throw( Null_Exception);}
_temp122->hd;})); _temp121->tl= 0; _temp121;});}); c=({ struct Cyc_List_List*
_temp123= c; if( _temp123 == 0){ _throw( Null_Exception);} _temp123->tl;}); a=({
struct Cyc_List_List* _temp124= a; if( _temp124 == 0){ _throw( Null_Exception);}
_temp124->tl;});} else{({ struct Cyc_List_List* _temp125= c; if( _temp125 == 0){
_throw( Null_Exception);} _temp125->tl=({ struct Cyc_List_List* _temp126=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp126->hd=( void*)(( void*)({ struct Cyc_List_List* _temp127= b; if( _temp127
== 0){ _throw( Null_Exception);} _temp127->hd;})); _temp126->tl= 0; _temp126;});});
c=({ struct Cyc_List_List* _temp128= c; if( _temp128 == 0){ _throw(
Null_Exception);} _temp128->tl;}); b=({ struct Cyc_List_List* _temp129= b; if(
_temp129 == 0){ _throw( Null_Exception);} _temp129->tl;});}} if( a == 0){({
struct Cyc_List_List* _temp130= c; if( _temp130 == 0){ _throw( Null_Exception);}
_temp130->tl=(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);});} else{({ struct Cyc_List_List* _temp131= c; if(
_temp131 == 0){ _throw( Null_Exception);} _temp131->tl=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);});}
return d;} struct Cyc_List_List* Cyc_List_merge_sort( int(* less_eq)( void*,
void*), struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_rmerge_sort)(
Cyc_Core_heap_region, less_eq, x);} struct Cyc_List_List* Cyc_List_merge( int(*
less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, int(* less_eq)(
void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b)) Cyc_List_rmerge)(
Cyc_Core_heap_region, less_eq, a, b);} char Cyc_List_Nth[ 8u]="\000\000\000\000Nth";
void* Cyc_List_nth( struct Cyc_List_List* x, int i){ while( i > 0? x != 0: 0) {
-- i; x=({ struct Cyc_List_List* _temp132= x; if( _temp132 == 0){ _throw(
Null_Exception);} _temp132->tl;});} if( i < 0? 1: x == 0){( void) _throw(( void*)
Cyc_List_Nth);} return( void*)({ struct Cyc_List_List* _temp133= x; if( _temp133
== 0){ _throw( Null_Exception);} _temp133->hd;});} int Cyc_List_forall( int(*
pred)( void*), struct Cyc_List_List* x){ while( x != 0? pred(( void*)({ struct
Cyc_List_List* _temp134= x; if( _temp134 == 0){ _throw( Null_Exception);}
_temp134->hd;})): 0) { x=({ struct Cyc_List_List* _temp135= x; if( _temp135 == 0){
_throw( Null_Exception);} _temp135->tl;});} return x == 0;} int Cyc_List_forall_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0?
pred( env,( void*)({ struct Cyc_List_List* _temp136= x; if( _temp136 == 0){
_throw( Null_Exception);} _temp136->hd;})): 0) { x=({ struct Cyc_List_List*
_temp137= x; if( _temp137 == 0){ _throw( Null_Exception);} _temp137->tl;});}
return x == 0;} int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List*
x){ while( x != 0? ! pred(( void*)({ struct Cyc_List_List* _temp138= x; if(
_temp138 == 0){ _throw( Null_Exception);} _temp138->hd;})): 0) { x=({ struct Cyc_List_List*
_temp139= x; if( _temp139 == 0){ _throw( Null_Exception);} _temp139->tl;});}
return x != 0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? ! pred( env,( void*)({ struct Cyc_List_List*
_temp140= x; if( _temp140 == 0){ _throw( Null_Exception);} _temp140->hd;})): 0) {
x=({ struct Cyc_List_List* _temp141= x; if( _temp141 == 0){ _throw(
Null_Exception);} _temp141->tl;});} return x != 0;} struct _tuple2{ void* f1;
void* f2; } ; struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3,
struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0? y == 0: 0){
return 0;} if( x == 0? 1: y == 0){( void) _throw(( void*) Cyc_List_List_mismatch);}
result=({ struct Cyc_List_List* _temp142=( struct Cyc_List_List*) _region_malloc(
r3, sizeof( struct Cyc_List_List)); _temp142->hd=( void*)({ struct _tuple2*
_temp143=( struct _tuple2*) _region_malloc( r4, sizeof( struct _tuple2));
_temp143->f1=( void*)({ struct Cyc_List_List* _temp145= x; if( _temp145 == 0){
_throw( Null_Exception);} _temp145->hd;}); _temp143->f2=( void*)({ struct Cyc_List_List*
_temp144= y; if( _temp144 == 0){ _throw( Null_Exception);} _temp144->hd;});
_temp143;}); _temp142->tl= 0; _temp142;}); prev= result; x=({ struct Cyc_List_List*
_temp146= x; if( _temp146 == 0){ _throw( Null_Exception);} _temp146->tl;}); y=({
struct Cyc_List_List* _temp147= y; if( _temp147 == 0){ _throw( Null_Exception);}
_temp147->tl;}); while( x != 0? y != 0: 0) {({ struct Cyc_List_List* _temp148=
prev; if( _temp148 == 0){ _throw( Null_Exception);} _temp148->tl=({ struct Cyc_List_List*
_temp149=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp149->hd=( void*)({ struct _tuple2* _temp150=( struct _tuple2*)
_region_malloc( r4, sizeof( struct _tuple2)); _temp150->f1=( void*)({ struct Cyc_List_List*
_temp152= x; if( _temp152 == 0){ _throw( Null_Exception);} _temp152->hd;});
_temp150->f2=( void*)({ struct Cyc_List_List* _temp151= y; if( _temp151 == 0){
_throw( Null_Exception);} _temp151->hd;}); _temp150;}); _temp149->tl= 0;
_temp149;});}); prev=({ struct Cyc_List_List* _temp153= prev; if( _temp153 == 0){
_throw( Null_Exception);} _temp153->tl;}); x=({ struct Cyc_List_List* _temp154=
x; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->tl;}); y=({ struct
Cyc_List_List* _temp155= y; if( _temp155 == 0){ _throw( Null_Exception);}
_temp155->tl;});} if( x != 0? 1: y != 0){( void) _throw(( void*) Cyc_List_List_mismatch);}
return result;} struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( Cyc_Core_heap_region, Cyc_Core_heap_region, x, y);} struct
_tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; if( x == 0){
return({ struct _tuple0 _temp156; _temp156.f1= 0; _temp156.f2= 0; _temp156;});}
prev1=( result1=({ struct Cyc_List_List* _temp157=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp157->hd=( void*)({
struct _tuple2* _temp159=( struct _tuple2*)({ struct Cyc_List_List* _temp158= x;
if( _temp158 == 0){ _throw( Null_Exception);} _temp158->hd;}); unsigned int
_temp160= 0; if( _temp160 >= 1u){ _throw( Null_Exception);} _temp159[ _temp160];}).f1;
_temp157->tl= 0; _temp157;})); prev2=( result2=({ struct Cyc_List_List* _temp161=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp161->hd=( void*)({ struct _tuple2* _temp163=( struct _tuple2*)({ struct Cyc_List_List*
_temp162= x; if( _temp162 == 0){ _throw( Null_Exception);} _temp162->hd;});
unsigned int _temp164= 0; if( _temp164 >= 1u){ _throw( Null_Exception);}
_temp163[ _temp164];}).f2; _temp161->tl= 0; _temp161;})); for( x=({ struct Cyc_List_List*
_temp165= x; if( _temp165 == 0){ _throw( Null_Exception);} _temp165->tl;}); x !=
0; x=({ struct Cyc_List_List* _temp166= x; if( _temp166 == 0){ _throw(
Null_Exception);} _temp166->tl;})){({ struct Cyc_List_List* _temp167= prev1; if(
_temp167 == 0){ _throw( Null_Exception);} _temp167->tl=({ struct Cyc_List_List*
_temp168=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp168->hd=( void*)({ struct _tuple2* _temp170=( struct _tuple2*)({ struct Cyc_List_List*
_temp169= x; if( _temp169 == 0){ _throw( Null_Exception);} _temp169->hd;});
unsigned int _temp171= 0; if( _temp171 >= 1u){ _throw( Null_Exception);}
_temp170[ _temp171];}).f1; _temp168->tl= 0; _temp168;});});({ struct Cyc_List_List*
_temp172= prev2; if( _temp172 == 0){ _throw( Null_Exception);} _temp172->tl=({
struct Cyc_List_List* _temp173=( struct Cyc_List_List*) _region_malloc( r4,
sizeof( struct Cyc_List_List)); _temp173->hd=( void*)({ struct _tuple2* _temp175=(
struct _tuple2*)({ struct Cyc_List_List* _temp174= x; if( _temp174 == 0){ _throw(
Null_Exception);} _temp174->hd;}); unsigned int _temp176= 0; if( _temp176 >= 1u){
_throw( Null_Exception);} _temp175[ _temp176];}).f2; _temp173->tl= 0; _temp173;});});
prev1=({ struct Cyc_List_List* _temp177= prev1; if( _temp177 == 0){ _throw(
Null_Exception);} _temp177->tl;}); prev2=({ struct Cyc_List_List* _temp178=
prev2; if( _temp178 == 0){ _throw( Null_Exception);} _temp178->tl;});} return({
struct _tuple0 _temp179; _temp179.f1= result1; _temp179.f2= result2; _temp179;});}
struct _tuple0 Cyc_List_split( struct Cyc_List_List* x){ return(( struct _tuple0(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x))
Cyc_List_rsplit)( Cyc_Core_heap_region, Cyc_Core_heap_region, x);} struct
_tuple3{ void* f1; void* f2; void* f3; } ; struct _tuple1 Cyc_List_rsplit3(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct _RegionHandle* r5,
struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; struct Cyc_List_List*
result3; struct Cyc_List_List* prev3; if( x == 0){ return({ struct _tuple1
_temp180; _temp180.f1= 0; _temp180.f2= 0; _temp180.f3= 0; _temp180;});} prev1=(
result1=({ struct Cyc_List_List* _temp181=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp181->hd=( void*)({
struct _tuple3* _temp183=( struct _tuple3*)({ struct Cyc_List_List* _temp182= x;
if( _temp182 == 0){ _throw( Null_Exception);} _temp182->hd;}); unsigned int
_temp184= 0; if( _temp184 >= 1u){ _throw( Null_Exception);} _temp183[ _temp184];}).f1;
_temp181->tl= 0; _temp181;})); prev2=( result2=({ struct Cyc_List_List* _temp185=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp185->hd=( void*)({ struct _tuple3* _temp187=( struct _tuple3*)({ struct Cyc_List_List*
_temp186= x; if( _temp186 == 0){ _throw( Null_Exception);} _temp186->hd;});
unsigned int _temp188= 0; if( _temp188 >= 1u){ _throw( Null_Exception);}
_temp187[ _temp188];}).f2; _temp185->tl= 0; _temp185;})); prev3=( result3=({
struct Cyc_List_List* _temp189=( struct Cyc_List_List*) _region_malloc( r5,
sizeof( struct Cyc_List_List)); _temp189->hd=( void*)({ struct _tuple3* _temp191=(
struct _tuple3*)({ struct Cyc_List_List* _temp190= x; if( _temp190 == 0){ _throw(
Null_Exception);} _temp190->hd;}); unsigned int _temp192= 0; if( _temp192 >= 1u){
_throw( Null_Exception);} _temp191[ _temp192];}).f3; _temp189->tl= 0; _temp189;}));
for( x=({ struct Cyc_List_List* _temp193= x; if( _temp193 == 0){ _throw(
Null_Exception);} _temp193->tl;}); x != 0; x=({ struct Cyc_List_List* _temp194=
x; if( _temp194 == 0){ _throw( Null_Exception);} _temp194->tl;})){({ struct Cyc_List_List*
_temp195= prev1; if( _temp195 == 0){ _throw( Null_Exception);} _temp195->tl=({
struct Cyc_List_List* _temp196=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp196->hd=( void*)({ struct _tuple3* _temp198=(
struct _tuple3*)({ struct Cyc_List_List* _temp197= x; if( _temp197 == 0){ _throw(
Null_Exception);} _temp197->hd;}); unsigned int _temp199= 0; if( _temp199 >= 1u){
_throw( Null_Exception);} _temp198[ _temp199];}).f1; _temp196->tl= 0; _temp196;});});({
struct Cyc_List_List* _temp200= prev2; if( _temp200 == 0){ _throw(
Null_Exception);} _temp200->tl=({ struct Cyc_List_List* _temp201=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp201->hd=( void*)({
struct _tuple3* _temp203=( struct _tuple3*)({ struct Cyc_List_List* _temp202= x;
if( _temp202 == 0){ _throw( Null_Exception);} _temp202->hd;}); unsigned int
_temp204= 0; if( _temp204 >= 1u){ _throw( Null_Exception);} _temp203[ _temp204];}).f2;
_temp201->tl= 0; _temp201;});});({ struct Cyc_List_List* _temp205= prev3; if(
_temp205 == 0){ _throw( Null_Exception);} _temp205->tl=({ struct Cyc_List_List*
_temp206=( struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp206->hd=( void*)({ struct _tuple3* _temp208=( struct _tuple3*)({ struct Cyc_List_List*
_temp207= x; if( _temp207 == 0){ _throw( Null_Exception);} _temp207->hd;});
unsigned int _temp209= 0; if( _temp209 >= 1u){ _throw( Null_Exception);}
_temp208[ _temp209];}).f3; _temp206->tl= 0; _temp206;});}); prev1=({ struct Cyc_List_List*
_temp210= prev1; if( _temp210 == 0){ _throw( Null_Exception);} _temp210->tl;});
prev2=({ struct Cyc_List_List* _temp211= prev2; if( _temp211 == 0){ _throw(
Null_Exception);} _temp211->tl;}); prev3=({ struct Cyc_List_List* _temp212=
prev3; if( _temp212 == 0){ _throw( Null_Exception);} _temp212->tl;});} return({
struct _tuple1 _temp213; _temp213.f1= result1; _temp213.f2= result2; _temp213.f3=
result3; _temp213;});} struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x){
return(( struct _tuple1(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct _RegionHandle* r5, struct Cyc_List_List* x)) Cyc_List_rsplit3)( Cyc_Core_heap_region,
Cyc_Core_heap_region, Cyc_Core_heap_region, x);} int Cyc_List_memq( struct Cyc_List_List*
l, void* x){ while( l != 0) { if(( void*)({ struct Cyc_List_List* _temp214= l;
if( _temp214 == 0){ _throw( Null_Exception);} _temp214->hd;}) == x){ return 1;}
l=({ struct Cyc_List_List* _temp215= l; if( _temp215 == 0){ _throw(
Null_Exception);} _temp215->tl;});} return 0;} int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare((
void*)({ struct Cyc_List_List* _temp216= l; if( _temp216 == 0){ _throw(
Null_Exception);} _temp216->hd;}), x) == 0){ return 1;} l=({ struct Cyc_List_List*
_temp217= l; if( _temp217 == 0){ _throw( Null_Exception);} _temp217->tl;});}
return 0;} void* Cyc_List_assoc( struct Cyc_List_List* l, void* x){ while( l !=
0) { if(({ struct _tuple2* _temp219=( struct _tuple2*)({ struct Cyc_List_List*
_temp218= l; if( _temp218 == 0){ _throw( Null_Exception);} _temp218->hd;});
unsigned int _temp220= 0; if( _temp220 >= 1u){ _throw( Null_Exception);}
_temp219[ _temp220];}).f1 == x){ return({ struct _tuple2* _temp222=( struct
_tuple2*)({ struct Cyc_List_List* _temp221= l; if( _temp221 == 0){ _throw(
Null_Exception);} _temp221->hd;}); unsigned int _temp223= 0; if( _temp223 >= 1u){
_throw( Null_Exception);} _temp222[ _temp223];}).f2;} l=({ struct Cyc_List_List*
_temp224= l; if( _temp224 == 0){ _throw( Null_Exception);} _temp224->tl;});}(
void) _throw(( void*) Cyc_Core_Not_found);} void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if(
compare( x,({ struct _tuple2* _temp226=( struct _tuple2*)({ struct Cyc_List_List*
_temp225= l; if( _temp225 == 0){ _throw( Null_Exception);} _temp225->hd;});
unsigned int _temp227= 0; if( _temp227 >= 1u){ _throw( Null_Exception);}
_temp226[ _temp227];}).f1) == 0){ return({ struct _tuple2* _temp229=( struct
_tuple2*)({ struct Cyc_List_List* _temp228= l; if( _temp228 == 0){ _throw(
Null_Exception);} _temp228->hd;}); unsigned int _temp230= 0; if( _temp230 >= 1u){
_throw( Null_Exception);} _temp229[ _temp230];}).f2;} l=({ struct Cyc_List_List*
_temp231= l; if( _temp231 == 0){ _throw( Null_Exception);} _temp231->tl;});}(
void) _throw(( void*) Cyc_Core_Not_found);} int Cyc_List_mem_assoc( struct Cyc_List_List*
l, void* x){ while( l != 0) { if(({ struct _tuple2* _temp233=( struct _tuple2*)({
struct Cyc_List_List* _temp232= l; if( _temp232 == 0){ _throw( Null_Exception);}
_temp232->hd;}); unsigned int _temp234= 0; if( _temp234 >= 1u){ _throw(
Null_Exception);} _temp233[ _temp234];}).f1 == x){ return 1;} l=({ struct Cyc_List_List*
_temp235= l; if( _temp235 == 0){ _throw( Null_Exception);} _temp235->tl;});}
return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*,
void*), struct Cyc_List_List* x){ while( x != 0) { if(({ struct Cyc_List_List*
_temp236= x; if( _temp236 == 0){ _throw( Null_Exception);} _temp236->tl;}) != 0){
if( compare(( void*)({ struct Cyc_List_List* _temp237= x; if( _temp237 == 0){
_throw( Null_Exception);} _temp237->hd;}),( void*)({ struct Cyc_List_List*
_temp239=({ struct Cyc_List_List* _temp238= x; if( _temp238 == 0){ _throw(
Null_Exception);} _temp238->tl;}); if( _temp239 == 0){ _throw( Null_Exception);}
_temp239->hd;})) == 0){ return({ struct Cyc_Core_Opt* _temp240=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp240->v=( void*)(( void*)({ struct
Cyc_List_List* _temp241= x; if( _temp241 == 0){ _throw( Null_Exception);}
_temp241->hd;})); _temp240;});}} x=({ struct Cyc_List_List* _temp242= x; if(
_temp242 == 0){ _throw( Null_Exception);} _temp242->tl;});} return 0;} struct
_tagged_ptr0 Cyc_List_rto_array( struct _RegionHandle* r2, struct Cyc_List_List*
x){ int s; struct _tagged_ptr0 arr; s=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
x); arr=({ unsigned int _temp243=( unsigned int) s; void** _temp244=( void**)
_region_malloc( r2, sizeof( void*) * _temp243); struct _tagged_ptr0 _temp248={
_temp244, _temp244, _temp244 + _temp243};{ unsigned int _temp245= _temp243;
unsigned int i; for( i= 0; i < _temp245; i ++){ _temp244[ i]=( void*)({ struct
Cyc_List_List* _temp247= x; if( _temp247 == 0){ _throw( Null_Exception);}
_temp247->hd;});}}; _temp248;});{ int i= 0; for( 0; i < s; ++ i, x=({ struct Cyc_List_List*
_temp249= x; if( _temp249 == 0){ _throw( Null_Exception);} _temp249->tl;})){({
struct _tagged_ptr0 _temp250= arr; void** _temp252= _temp250.curr + i; if(
_temp250.base == 0? 1:( _temp252 < _temp250.base? 1: _temp252 >= _temp250.last_plus_one)){
_throw( Null_Exception);}* _temp252=( void*)({ struct Cyc_List_List* _temp253= x;
if( _temp253 == 0){ _throw( Null_Exception);} _temp253->hd;});});}} return arr;}
struct _tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x){ return(( struct
_tagged_ptr0(*)( struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rto_array)(
Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_rfrom_array( struct
_RegionHandle* r2, struct _tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{
unsigned int i=({ struct _tagged_ptr0 _temp258= arr;( unsigned int)( _temp258.last_plus_one
- _temp258.curr);}) - 1; for( 0; i >= 0; -- i){ ans=({ struct Cyc_List_List*
_temp254=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp254->hd=( void*)({ struct _tagged_ptr0 _temp255= arr; void** _temp257=
_temp255.curr +( int) i; if( _temp255.base == 0? 1:( _temp257 < _temp255.base? 1:
_temp257 >= _temp255.last_plus_one)){ _throw( Null_Exception);}* _temp257;});
_temp254->tl= ans; _temp254;});}} return ans;} struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_ptr0 arr)) Cyc_List_rfrom_array)( Cyc_Core_heap_region,
arr);} struct Cyc_List_List* Cyc_List_rtabulate( struct _RegionHandle* r, int n,
void*(* f)( int)){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i < n; ++ i){
res=({ struct Cyc_List_List* _temp259=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp259->hd=( void*) f( i); _temp259->tl= res;
_temp259;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
res);} struct Cyc_List_List* Cyc_List_tabulate( int n, void*(* f)( int)){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(* f)( int))) Cyc_List_rtabulate)(
Cyc_Core_heap_region, n, f);} struct Cyc_List_List* Cyc_List_rtabulate_c( struct
_RegionHandle* r, int n, void*(* f)( void*, int), void* env){ struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp260=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp260->hd=( void*) f( env, i); _temp260->tl= res; _temp260;});}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( res);}
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(*
f)( void*, int), void* env)) Cyc_List_rtabulate_c)( Cyc_Core_heap_region, n, f,
env);} int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List*
_temp261= l1; if( _temp261 == 0){ _throw( Null_Exception);} _temp261->tl;}), l2=({
struct Cyc_List_List* _temp262= l2; if( _temp262 == 0){ _throw( Null_Exception);}
_temp262->tl;})){ if( l1 == l2){ return 0;}{ int i= cmp(( void*)({ struct Cyc_List_List*
_temp263= l1; if( _temp263 == 0){ _throw( Null_Exception);} _temp263->hd;}),(
void*)({ struct Cyc_List_List* _temp264= l2; if( _temp264 == 0){ _throw(
Null_Exception);} _temp264->hd;})); if( i != 0){ return i;}}} if( l1 != 0){
return 1;} if( l2 != 0){ return - 1;} return 0;} int Cyc_List_list_prefix( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){ for( 0;
l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List* _temp265= l1; if( _temp265 == 0){
_throw( Null_Exception);} _temp265->tl;}), l2=({ struct Cyc_List_List* _temp266=
l2; if( _temp266 == 0){ _throw( Null_Exception);} _temp266->tl;})){ if( l1 == l2){
return 1;}{ int i= cmp(( void*)({ struct Cyc_List_List* _temp267= l1; if(
_temp267 == 0){ _throw( Null_Exception);} _temp267->hd;}),( void*)({ struct Cyc_List_List*
_temp268= l2; if( _temp268 == 0){ _throw( Null_Exception);} _temp268->hd;}));
if( i != 0){ return 0;}}} return l1 == 0;} struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp276=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp276->hd=( void*)(( void*)({ struct Cyc_List_List* _temp277= l; if( _temp277
== 0){ _throw( Null_Exception);} _temp277->hd;})); _temp276->tl= 0; _temp276;});
struct Cyc_List_List* end= result; for( 0; l != 0; l=({ struct Cyc_List_List*
_temp269= l; if( _temp269 == 0){ _throw( Null_Exception);} _temp269->tl;})){ if(
f( env,( void*)({ struct Cyc_List_List* _temp270= l; if( _temp270 == 0){ _throw(
Null_Exception);} _temp270->hd;}))){({ struct Cyc_List_List* _temp271= end; if(
_temp271 == 0){ _throw( Null_Exception);} _temp271->tl=({ struct Cyc_List_List*
_temp272=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp272->hd=( void*)(( void*)({ struct Cyc_List_List* _temp273= l; if( _temp273
== 0){ _throw( Null_Exception);} _temp273->hd;})); _temp272->tl= 0; _temp272;});});
end=({ struct Cyc_List_List* _temp274= end; if( _temp274 == 0){ _throw(
Null_Exception);} _temp274->tl;});}} return({ struct Cyc_List_List* _temp275=
result; if( _temp275 == 0){ _throw( Null_Exception);} _temp275->tl;});}} struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* f)( void*,
void*), void* env, struct Cyc_List_List* l)) Cyc_List_rfilter_c)( Cyc_Core_heap_region,
f, env, l);} struct Cyc_List_List* Cyc_List_rfilter( struct _RegionHandle* r2,
int(* f)( void*), struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp285->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp286= l; if( _temp286 == 0){ _throw( Null_Exception);} _temp286->hd;}));
_temp285->tl= 0; _temp285;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l=({ struct Cyc_List_List* _temp278= l; if( _temp278 == 0){ _throw(
Null_Exception);} _temp278->tl;})){ if( f(( void*)({ struct Cyc_List_List*
_temp279= l; if( _temp279 == 0){ _throw( Null_Exception);} _temp279->hd;}))){({
struct Cyc_List_List* _temp280= end; if( _temp280 == 0){ _throw( Null_Exception);}
_temp280->tl=({ struct Cyc_List_List* _temp281=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp281->hd=( void*)(( void*)({
struct Cyc_List_List* _temp282= l; if( _temp282 == 0){ _throw( Null_Exception);}
_temp282->hd;})); _temp281->tl= 0; _temp281;});}); end=({ struct Cyc_List_List*
_temp283= end; if( _temp283 == 0){ _throw( Null_Exception);} _temp283->tl;});}}
return({ struct Cyc_List_List* _temp284= result; if( _temp284 == 0){ _throw(
Null_Exception);} _temp284->tl;});}} struct Cyc_List_List* Cyc_List_filter( int(*
f)( void*), struct Cyc_List_List* l){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* f)( void*), struct Cyc_List_List* l)) Cyc_List_rfilter)(
Cyc_Core_heap_region, f, l);}