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
char* tag; struct _tagged_string f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern void* Cyc_List_hd(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* src);
extern struct Cyc_List_List* Cyc_List_rcopy( struct _RegionHandle*, struct Cyc_List_List*
src); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u];
struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map2( void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rmap2( struct _RegionHandle*, void*(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern
void Cyc_List_app( void*(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_app_c(
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
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_merge( int(*
less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b);
extern struct Cyc_List_List* Cyc_List_rmerge( struct _RegionHandle*, int(*
less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ;
extern void* Cyc_List_nth( struct Cyc_List_List* x, int i); extern int Cyc_List_forall(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_forall_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x); extern int Cyc_List_exists(
int(* pred)( void*), struct Cyc_List_List* x); extern int Cyc_List_exists_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct
Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct _tuple0 Cyc_List_split(
struct Cyc_List_List* x); extern struct _tuple1 Cyc_List_split3( struct Cyc_List_List*
x); extern struct _tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x); extern struct _tuple1 Cyc_List_rsplit3(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct _RegionHandle* r5,
struct Cyc_List_List* x); extern int Cyc_List_memq( struct Cyc_List_List* l,
void* x); extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); extern void* Cyc_List_assoc( struct Cyc_List_List* l, void* x);
extern void* Cyc_List_assoc_cmp( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); extern int Cyc_List_mem_assoc( struct Cyc_List_List* l, void* x);
extern struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*, void*),
struct Cyc_List_List* x); extern struct _tagged_ptr0 Cyc_List_to_array( struct
Cyc_List_List* x); extern struct _tagged_ptr0 Cyc_List_rto_array( struct
_RegionHandle* r2, struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_from_array(
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
Null_Exception);} _temp0->tl;});} return i;} char Cyc_List_List_empty_tag[ 11u]="List_empty";
void* Cyc_List_hd( struct Cyc_List_List* x){ if( x == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_List_List_empty_struct* _temp1=( struct Cyc_List_List_empty_struct*)
GC_malloc( sizeof( struct Cyc_List_List_empty_struct)); _temp1[ 0]=({ struct Cyc_List_List_empty_struct
_temp2; _temp2.tag= Cyc_List_List_empty_tag; _temp2;}); _temp1;}));} return(
void*)({ struct Cyc_List_List* _temp3= x; if( _temp3 == 0){ _throw(
Null_Exception);} _temp3->hd;});} struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List*
x){ if( x == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_empty_struct*
_temp4=( struct Cyc_List_List_empty_struct*) GC_malloc( sizeof( struct Cyc_List_List_empty_struct));
_temp4[ 0]=({ struct Cyc_List_List_empty_struct _temp5; _temp5.tag= Cyc_List_List_empty_tag;
_temp5;}); _temp4;}));} return({ struct Cyc_List_List* _temp6= x; if( _temp6 ==
0){ _throw( Null_Exception);} _temp6->tl;});} struct Cyc_List_List* Cyc_List_rcopy(
struct _RegionHandle* r2, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp7=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp7->hd=( void*)(( void*)({ struct Cyc_List_List* _temp8= x; if( _temp8 == 0){
_throw( Null_Exception);} _temp8->hd;})); _temp7->tl= 0; _temp7;}); prev= result;
for( x=({ struct Cyc_List_List* _temp9= x; if( _temp9 == 0){ _throw(
Null_Exception);} _temp9->tl;}); x != 0; x=({ struct Cyc_List_List* _temp10= x;
if( _temp10 == 0){ _throw( Null_Exception);} _temp10->tl;})){({ struct Cyc_List_List*
_temp11= prev; if( _temp11 == 0){ _throw( Null_Exception);} _temp11->tl=({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp13= x; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->hd;}));
_temp12->tl= 0; _temp12;});}); prev=({ struct Cyc_List_List* _temp14= prev; if(
_temp14 == 0){ _throw( Null_Exception);} _temp14->tl;});} return result;} struct
Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( Cyc_Core_heap_region,
x);} struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle* r2, void*(* f)(
void*), struct Cyc_List_List* x){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List* _temp15=( struct
Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List)); _temp15->hd=(
void*) f(( void*)({ struct Cyc_List_List* _temp16= x; if( _temp16 == 0){ _throw(
Null_Exception);} _temp16->hd;})); _temp15->tl= 0; _temp15;}); prev= result;
for( x=({ struct Cyc_List_List* _temp17= x; if( _temp17 == 0){ _throw(
Null_Exception);} _temp17->tl;}); x != 0; x=({ struct Cyc_List_List* _temp18= x;
if( _temp18 == 0){ _throw( Null_Exception);} _temp18->tl;})){ struct Cyc_List_List*
temp=({ struct Cyc_List_List* _temp19=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp19->hd=( void*) f(( void*)({ struct Cyc_List_List*
_temp20= x; if( _temp20 == 0){ _throw( Null_Exception);} _temp20->hd;}));
_temp19->tl= 0; _temp19;});({ struct Cyc_List_List* _temp21= prev; if( _temp21
== 0){ _throw( Null_Exception);} _temp21->tl= temp;}); prev= temp;} return
result;} struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, void*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_rmap)( Cyc_Core_heap_region, f, x);}
struct Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle* r2, void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp22=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp22->hd=( void*) f( env,( void*)({ struct Cyc_List_List* _temp23= x; if(
_temp23 == 0){ _throw( Null_Exception);} _temp23->hd;})); _temp22->tl= 0;
_temp22;}); prev= result; for( x=({ struct Cyc_List_List* _temp24= x; if(
_temp24 == 0){ _throw( Null_Exception);} _temp24->tl;}); x != 0; x=({ struct Cyc_List_List*
_temp25= x; if( _temp25 == 0){ _throw( Null_Exception);} _temp25->tl;})){({
struct Cyc_List_List* _temp26= prev; if( _temp26 == 0){ _throw( Null_Exception);}
_temp26->tl=({ struct Cyc_List_List* _temp27=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp27->hd=( void*) f( env,(
void*)({ struct Cyc_List_List* _temp28= x; if( _temp28 == 0){ _throw(
Null_Exception);} _temp28->hd;})); _temp27->tl= 0; _temp27;});}); prev=({ struct
Cyc_List_List* _temp29= prev; if( _temp29 == 0){ _throw( Null_Exception);}
_temp29->tl;});} return result;} struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( Cyc_Core_heap_region, f, env, x);} char Cyc_List_List_mismatch_tag[
14u]="List_mismatch"; struct Cyc_List_List* Cyc_List_rmap2( struct _RegionHandle*
r3, void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0? y == 0: 0){
return 0;} if( x == 0? 1: y == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_List_List_mismatch_struct* _temp30=( struct Cyc_List_List_mismatch_struct*)
GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct)); _temp30[ 0]=({ struct
Cyc_List_List_mismatch_struct _temp31; _temp31.tag= Cyc_List_List_mismatch_tag;
_temp31;}); _temp30;}));} result=({ struct Cyc_List_List* _temp32=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp32->hd=( void*) f((
void*)({ struct Cyc_List_List* _temp33= x; if( _temp33 == 0){ _throw(
Null_Exception);} _temp33->hd;}),( void*)({ struct Cyc_List_List* _temp34= y;
if( _temp34 == 0){ _throw( Null_Exception);} _temp34->hd;})); _temp32->tl= 0;
_temp32;}); prev= result; x=({ struct Cyc_List_List* _temp35= x; if( _temp35 ==
0){ _throw( Null_Exception);} _temp35->tl;}); y=({ struct Cyc_List_List* _temp36=
y; if( _temp36 == 0){ _throw( Null_Exception);} _temp36->tl;}); while( x != 0? y
!= 0: 0) {({ struct Cyc_List_List* _temp37= prev; if( _temp37 == 0){ _throw(
Null_Exception);} _temp37->tl=({ struct Cyc_List_List* _temp38=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp38->hd=( void*) f((
void*)({ struct Cyc_List_List* _temp39= x; if( _temp39 == 0){ _throw(
Null_Exception);} _temp39->hd;}),( void*)({ struct Cyc_List_List* _temp40= y;
if( _temp40 == 0){ _throw( Null_Exception);} _temp40->hd;})); _temp38->tl= 0;
_temp38;});}); prev=({ struct Cyc_List_List* _temp41= prev; if( _temp41 == 0){
_throw( Null_Exception);} _temp41->tl;}); x=({ struct Cyc_List_List* _temp42= x;
if( _temp42 == 0){ _throw( Null_Exception);} _temp42->tl;}); y=({ struct Cyc_List_List*
_temp43= y; if( _temp43 == 0){ _throw( Null_Exception);} _temp43->tl;});} if( x
!= 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp44=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp44[ 0]=({ struct Cyc_List_List_mismatch_struct _temp45; _temp45.tag= Cyc_List_List_mismatch_tag;
_temp45;}); _temp44;}));} return result;} struct Cyc_List_List* Cyc_List_map2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, void*(* f)( void*,
void*), struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rmap2)( Cyc_Core_heap_region,
f, x, y);} void Cyc_List_app( void*(* f)( void*), struct Cyc_List_List* x){
while( x != 0) { f(( void*)({ struct Cyc_List_List* _temp46= x; if( _temp46 == 0){
_throw( Null_Exception);} _temp46->hd;})); x=({ struct Cyc_List_List* _temp47= x;
if( _temp47 == 0){ _throw( Null_Exception);} _temp47->tl;});}} void Cyc_List_app_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0) {
f( env,( void*)({ struct Cyc_List_List* _temp48= x; if( _temp48 == 0){ _throw(
Null_Exception);} _temp48->hd;})); x=({ struct Cyc_List_List* _temp49= x; if(
_temp49 == 0){ _throw( Null_Exception);} _temp49->tl;});}} void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x){ while( x != 0) { f(( void*)({
struct Cyc_List_List* _temp50= x; if( _temp50 == 0){ _throw( Null_Exception);}
_temp50->hd;})); x=({ struct Cyc_List_List* _temp51= x; if( _temp51 == 0){
_throw( Null_Exception);} _temp51->tl;});}} void Cyc_List_iter_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0) { f( env,(
void*)({ struct Cyc_List_List* _temp52= x; if( _temp52 == 0){ _throw(
Null_Exception);} _temp52->hd;})); x=({ struct Cyc_List_List* _temp53= x; if(
_temp53 == 0){ _throw( Null_Exception);} _temp53->tl;});}} void Cyc_List_app2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
while( x != 0? y != 0: 0) { f(( void*)({ struct Cyc_List_List* _temp54= x; if(
_temp54 == 0){ _throw( Null_Exception);} _temp54->hd;}),( void*)({ struct Cyc_List_List*
_temp55= y; if( _temp55 == 0){ _throw( Null_Exception);} _temp55->hd;})); x=({
struct Cyc_List_List* _temp56= x; if( _temp56 == 0){ _throw( Null_Exception);}
_temp56->tl;}); y=({ struct Cyc_List_List* _temp57= y; if( _temp57 == 0){ _throw(
Null_Exception);} _temp57->tl;});} if( x != 0? 1: y != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp58=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp58[ 0]=({ struct Cyc_List_List_mismatch_struct _temp59; _temp59.tag= Cyc_List_List_mismatch_tag;
_temp59;}); _temp58;}));}} void Cyc_List_app2_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y
!= 0: 0) { f( env,( void*)({ struct Cyc_List_List* _temp60= x; if( _temp60 == 0){
_throw( Null_Exception);} _temp60->hd;}),( void*)({ struct Cyc_List_List*
_temp61= y; if( _temp61 == 0){ _throw( Null_Exception);} _temp61->hd;})); x=({
struct Cyc_List_List* _temp62= x; if( _temp62 == 0){ _throw( Null_Exception);}
_temp62->tl;}); y=({ struct Cyc_List_List* _temp63= y; if( _temp63 == 0){ _throw(
Null_Exception);} _temp63->tl;});} if( x != 0? 1: y != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp64=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp64[ 0]=({ struct Cyc_List_List_mismatch_struct _temp65; _temp65.tag= Cyc_List_List_mismatch_tag;
_temp65;}); _temp64;}));}} void Cyc_List_iter2( void(* f)( void*, void*), struct
Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y != 0: 0) { f(( void*)({
struct Cyc_List_List* _temp66= x; if( _temp66 == 0){ _throw( Null_Exception);}
_temp66->hd;}),( void*)({ struct Cyc_List_List* _temp67= y; if( _temp67 == 0){
_throw( Null_Exception);} _temp67->hd;})); x=({ struct Cyc_List_List* _temp68= x;
if( _temp68 == 0){ _throw( Null_Exception);} _temp68->tl;}); y=({ struct Cyc_List_List*
_temp69= y; if( _temp69 == 0){ _throw( Null_Exception);} _temp69->tl;});} if( x
!= 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp70=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp70[ 0]=({ struct Cyc_List_List_mismatch_struct _temp71; _temp71.tag= Cyc_List_List_mismatch_tag;
_temp71;}); _temp70;}));}} void Cyc_List_iter2_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x != 0? y
!= 0: 0) { f( env,( void*)({ struct Cyc_List_List* _temp72= x; if( _temp72 == 0){
_throw( Null_Exception);} _temp72->hd;}),( void*)({ struct Cyc_List_List*
_temp73= y; if( _temp73 == 0){ _throw( Null_Exception);} _temp73->hd;})); x=({
struct Cyc_List_List* _temp74= x; if( _temp74 == 0){ _throw( Null_Exception);}
_temp74->tl;}); y=({ struct Cyc_List_List* _temp75= y; if( _temp75 == 0){ _throw(
Null_Exception);} _temp75->tl;});} if( x != 0? 1: y != 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp76=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp76[ 0]=({ struct Cyc_List_List_mismatch_struct _temp77; _temp77.tag= Cyc_List_List_mismatch_tag;
_temp77;}); _temp76;}));}} void* Cyc_List_fold_left( void*(* f)( void*, void*),
void* accum, struct Cyc_List_List* x){ while( x != 0) { accum= f( accum,( void*)({
struct Cyc_List_List* _temp78= x; if( _temp78 == 0){ _throw( Null_Exception);}
_temp78->hd;})); x=({ struct Cyc_List_List* _temp79= x; if( _temp79 == 0){
_throw( Null_Exception);} _temp79->tl;});} return accum;} void* Cyc_List_fold_left_c(
void*(* f)( void*, void*, void*), void* env, void* accum, struct Cyc_List_List*
x){ while( x != 0) { accum= f( env, accum,( void*)({ struct Cyc_List_List*
_temp80= x; if( _temp80 == 0){ _throw( Null_Exception);} _temp80->hd;})); x=({
struct Cyc_List_List* _temp81= x; if( _temp81 == 0){ _throw( Null_Exception);}
_temp81->tl;});} return accum;} void* Cyc_List_fold_right( void*(* f)( void*,
void*), struct Cyc_List_List* x, void* accum){ if( x == 0){ return accum;} else{
return f(( void*)({ struct Cyc_List_List* _temp82= x; if( _temp82 == 0){ _throw(
Null_Exception);} _temp82->hd;}),(( void*(*)( void*(* f)( void*, void*), struct
Cyc_List_List* x, void* accum)) Cyc_List_fold_right)( f,({ struct Cyc_List_List*
_temp83= x; if( _temp83 == 0){ _throw( Null_Exception);} _temp83->tl;}), accum));}}
void* Cyc_List_fold_right_c( void*(* f)( void*, void*, void*), void* env, struct
Cyc_List_List* x, void* accum){ if( x == 0){ return accum;} else{ return f( env,(
void*)({ struct Cyc_List_List* _temp84= x; if( _temp84 == 0){ _throw(
Null_Exception);} _temp84->hd;}),(( void*(*)( void*(* f)( void*, void*, void*),
void* env, struct Cyc_List_List* x, void* accum)) Cyc_List_fold_right_c)( f, env,({
struct Cyc_List_List* _temp85= x; if( _temp85 == 0){ _throw( Null_Exception);}
_temp85->tl;}), accum));}} struct Cyc_List_List* Cyc_List_rrevappend( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y){ while( x
!= 0) { y=({ struct Cyc_List_List* _temp86=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp86->hd=( void*)(( void*)({
struct Cyc_List_List* _temp87= x; if( _temp87 == 0){ _throw( Null_Exception);}
_temp87->hd;})); _temp86->tl= y; _temp86;}); x=({ struct Cyc_List_List* _temp88=
x; if( _temp88 == 0){ _throw( Null_Exception);} _temp88->tl;});} return y;}
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
first= x; struct Cyc_List_List* second=({ struct Cyc_List_List* _temp89= x; if(
_temp89 == 0){ _throw( Null_Exception);} _temp89->tl;});({ struct Cyc_List_List*
_temp90= x; if( _temp90 == 0){ _throw( Null_Exception);} _temp90->tl= 0;});
while( second != 0) { struct Cyc_List_List* temp=({ struct Cyc_List_List*
_temp91= second; if( _temp91 == 0){ _throw( Null_Exception);} _temp91->tl;});({
struct Cyc_List_List* _temp92= second; if( _temp92 == 0){ _throw( Null_Exception);}
_temp92->tl= first;}); first= second; second= temp;} return first;}} struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y){ struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){
return y;} if( y == 0){ return(( struct Cyc_List_List*(*)( struct _RegionHandle*
r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r2, x);} result=({ struct Cyc_List_List*
_temp93=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp93->hd=( void*)(( void*)({ struct Cyc_List_List* _temp94= x; if( _temp94 ==
0){ _throw( Null_Exception);} _temp94->hd;})); _temp93->tl= 0; _temp93;}); prev=
result; for( x=({ struct Cyc_List_List* _temp95= x; if( _temp95 == 0){ _throw(
Null_Exception);} _temp95->tl;}); x != 0; x=({ struct Cyc_List_List* _temp96= x;
if( _temp96 == 0){ _throw( Null_Exception);} _temp96->tl;})){({ struct Cyc_List_List*
_temp97= prev; if( _temp97 == 0){ _throw( Null_Exception);} _temp97->tl=({
struct Cyc_List_List* _temp98=( struct Cyc_List_List*) _region_malloc( r2,
sizeof( struct Cyc_List_List)); _temp98->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp99= x; if( _temp99 == 0){ _throw( Null_Exception);} _temp99->hd;}));
_temp98->tl= 0; _temp98;});}); prev=({ struct Cyc_List_List* _temp100= prev; if(
_temp100 == 0){ _throw( Null_Exception);} _temp100->tl;});}({ struct Cyc_List_List*
_temp101= prev; if( _temp101 == 0){ _throw( Null_Exception);} _temp101->tl= y;});
return result;} struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
Cyc_Core_heap_region, x, y);} struct Cyc_List_List* Cyc_List_imp_append( struct
Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if( x == 0){
return y;} if( y == 0){ return x;} for( z= x;({ struct Cyc_List_List* _temp102=
z; if( _temp102 == 0){ _throw( Null_Exception);} _temp102->tl;}) != 0; z=({
struct Cyc_List_List* _temp103= z; if( _temp103 == 0){ _throw( Null_Exception);}
_temp103->tl;})){;}({ struct Cyc_List_List* _temp104= z; if( _temp104 == 0){
_throw( Null_Exception);} _temp104->tl= y;}); return x;} struct Cyc_List_List*
Cyc_List_rflatten( struct _RegionHandle* r3, struct Cyc_List_List* x){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _RegionHandle*,
struct Cyc_List_List*, struct Cyc_List_List*), struct _RegionHandle* env, struct
Cyc_List_List* x, struct Cyc_List_List* accum)) Cyc_List_fold_right_c)(( struct
Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rappend, r3, x, 0);} struct Cyc_List_List* Cyc_List_flatten( struct
Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3,
struct Cyc_List_List* x)) Cyc_List_rflatten)( Cyc_Core_heap_region, x);} struct
Cyc_List_List* Cyc_List_rmerge_sort( struct _RegionHandle* r2, int(* less_eq)(
void*, void*), struct Cyc_List_List* x){ if( x == 0? 1:({ struct Cyc_List_List*
_temp105= x; if( _temp105 == 0){ _throw( Null_Exception);} _temp105->tl;}) == 0){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r2, x);}{ struct Cyc_List_List* result;{ struct
_RegionHandle _temp106= _new_region(); struct _RegionHandle* r3=& _temp106;
struct _handler_cons _temp107; _push_handler(& _temp107);{ struct
_xtunion_struct* _temp108=( struct _xtunion_struct*) setjmp( _temp107.handler);
if( ! _temp108){{ struct Cyc_List_List* a= 0; struct Cyc_List_List* b= 0; while(
x != 0) { a=({ struct Cyc_List_List* _temp109=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp109->hd=( void*)(( void*)({
struct Cyc_List_List* _temp110= x; if( _temp110 == 0){ _throw( Null_Exception);}
_temp110->hd;})); _temp109->tl= a; _temp109;}); x=({ struct Cyc_List_List*
_temp111= x; if( _temp111 == 0){ _throw( Null_Exception);} _temp111->tl;}); if(
x != 0){ b=({ struct Cyc_List_List* _temp112=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp112->hd=( void*)(( void*)({
struct Cyc_List_List* _temp113= x; if( _temp113 == 0){ _throw( Null_Exception);}
_temp113->hd;})); _temp112->tl= b; _temp112;}); x=({ struct Cyc_List_List*
_temp114= x; if( _temp114 == 0){ _throw( Null_Exception);} _temp114->tl;});}} a=((
struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* less_eq)( void*, void*),
struct Cyc_List_List* x)) Cyc_List_rmerge_sort)( r3, less_eq, a); b=(( struct
Cyc_List_List*(*)( struct _RegionHandle* r2, int(* less_eq)( void*, void*),
struct Cyc_List_List* x)) Cyc_List_rmerge_sort)( r3, less_eq, b); result=((
struct Cyc_List_List*(*)( struct _RegionHandle*, int(* less_eq)( void*, void*),
struct Cyc_List_List* a, struct Cyc_List_List* b)) Cyc_List_rmerge)( r2, less_eq,
a, b);} _free_region(& _temp106); _pop_handler();} else{ _free_region(& _temp106);
_throw( _temp108);}}} return result;}} struct Cyc_List_List* Cyc_List_rmerge(
struct _RegionHandle* r3, int(* less_eq)( void*, void*), struct Cyc_List_List* a,
struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if(
a == 0){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);} if( b == 0){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);}
if( less_eq(( void*)({ struct Cyc_List_List* _temp115= a; if( _temp115 == 0){
_throw( Null_Exception);} _temp115->hd;}),( void*)({ struct Cyc_List_List*
_temp116= b; if( _temp116 == 0){ _throw( Null_Exception);} _temp116->hd;})) <= 0){
c=({ struct Cyc_List_List* _temp117=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp117->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp118= a; if( _temp118 == 0){ _throw( Null_Exception);} _temp118->hd;}));
_temp117->tl= 0; _temp117;}); a=({ struct Cyc_List_List* _temp119= a; if(
_temp119 == 0){ _throw( Null_Exception);} _temp119->tl;});} else{ c=({ struct
Cyc_List_List* _temp120=( struct Cyc_List_List*) _region_malloc( r3, sizeof(
struct Cyc_List_List)); _temp120->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp121= b; if( _temp121 == 0){ _throw( Null_Exception);} _temp121->hd;}));
_temp120->tl= 0; _temp120;}); b=({ struct Cyc_List_List* _temp122= b; if(
_temp122 == 0){ _throw( Null_Exception);} _temp122->tl;});} d= c; while( a != 0?
b != 0: 0) { if( less_eq(( void*)({ struct Cyc_List_List* _temp123= a; if(
_temp123 == 0){ _throw( Null_Exception);} _temp123->hd;}),( void*)({ struct Cyc_List_List*
_temp124= b; if( _temp124 == 0){ _throw( Null_Exception);} _temp124->hd;})) <= 0){({
struct Cyc_List_List* _temp125= c; if( _temp125 == 0){ _throw( Null_Exception);}
_temp125->tl=({ struct Cyc_List_List* _temp126=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp126->hd=( void*)(( void*)({
struct Cyc_List_List* _temp127= a; if( _temp127 == 0){ _throw( Null_Exception);}
_temp127->hd;})); _temp126->tl= 0; _temp126;});}); c=({ struct Cyc_List_List*
_temp128= c; if( _temp128 == 0){ _throw( Null_Exception);} _temp128->tl;}); a=({
struct Cyc_List_List* _temp129= a; if( _temp129 == 0){ _throw( Null_Exception);}
_temp129->tl;});} else{({ struct Cyc_List_List* _temp130= c; if( _temp130 == 0){
_throw( Null_Exception);} _temp130->tl=({ struct Cyc_List_List* _temp131=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp131->hd=( void*)(( void*)({ struct Cyc_List_List* _temp132= b; if( _temp132
== 0){ _throw( Null_Exception);} _temp132->hd;})); _temp131->tl= 0; _temp131;});});
c=({ struct Cyc_List_List* _temp133= c; if( _temp133 == 0){ _throw(
Null_Exception);} _temp133->tl;}); b=({ struct Cyc_List_List* _temp134= b; if(
_temp134 == 0){ _throw( Null_Exception);} _temp134->tl;});}} if( a == 0){({
struct Cyc_List_List* _temp135= c; if( _temp135 == 0){ _throw( Null_Exception);}
_temp135->tl=(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);});} else{({ struct Cyc_List_List* _temp136= c; if(
_temp136 == 0){ _throw( Null_Exception);} _temp136->tl=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rcopy)( r3, a);});}
return d;} struct Cyc_List_List* Cyc_List_merge_sort( int(* less_eq)( void*,
void*), struct Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* less_eq)( void*, void*), struct Cyc_List_List* x)) Cyc_List_rmerge_sort)(
Cyc_Core_heap_region, less_eq, x);} struct Cyc_List_List* Cyc_List_merge( int(*
less_eq)( void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, int(* less_eq)(
void*, void*), struct Cyc_List_List* a, struct Cyc_List_List* b)) Cyc_List_rmerge)(
Cyc_Core_heap_region, less_eq, a, b);} char Cyc_List_Nth_tag[ 4u]="Nth"; void*
Cyc_List_nth( struct Cyc_List_List* x, int i){ while( i > 0? x != 0: 0) { -- i;
x=({ struct Cyc_List_List* _temp137= x; if( _temp137 == 0){ _throw(
Null_Exception);} _temp137->tl;});} if( i < 0? 1: x == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_Nth_struct* _temp138=( struct Cyc_List_Nth_struct*)
GC_malloc( sizeof( struct Cyc_List_Nth_struct)); _temp138[ 0]=({ struct Cyc_List_Nth_struct
_temp139; _temp139.tag= Cyc_List_Nth_tag; _temp139;}); _temp138;}));} return(
void*)({ struct Cyc_List_List* _temp140= x; if( _temp140 == 0){ _throw(
Null_Exception);} _temp140->hd;});} int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x){ while( x != 0? pred(( void*)({ struct Cyc_List_List*
_temp141= x; if( _temp141 == 0){ _throw( Null_Exception);} _temp141->hd;})): 0) {
x=({ struct Cyc_List_List* _temp142= x; if( _temp142 == 0){ _throw(
Null_Exception);} _temp142->tl;});} return x == 0;} int Cyc_List_forall_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0? pred(
env,( void*)({ struct Cyc_List_List* _temp143= x; if( _temp143 == 0){ _throw(
Null_Exception);} _temp143->hd;})): 0) { x=({ struct Cyc_List_List* _temp144= x;
if( _temp144 == 0){ _throw( Null_Exception);} _temp144->tl;});} return x == 0;}
int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x){ while( x !=
0? ! pred(( void*)({ struct Cyc_List_List* _temp145= x; if( _temp145 == 0){
_throw( Null_Exception);} _temp145->hd;})): 0) { x=({ struct Cyc_List_List*
_temp146= x; if( _temp146 == 0){ _throw( Null_Exception);} _temp146->tl;});}
return x != 0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? ! pred( env,( void*)({ struct Cyc_List_List*
_temp147= x; if( _temp147 == 0){ _throw( Null_Exception);} _temp147->hd;})): 0) {
x=({ struct Cyc_List_List* _temp148= x; if( _temp148 == 0){ _throw(
Null_Exception);} _temp148->tl;});} return x != 0;} struct Cyc_List_List* Cyc_List_rzip(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp149=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp149[ 0]=({ struct Cyc_List_List_mismatch_struct _temp150; _temp150.tag= Cyc_List_List_mismatch_tag;
_temp150;}); _temp149;}));} result=({ struct Cyc_List_List* _temp151=( struct
Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List)); _temp151->hd=(
void*)({ struct _tuple2* _temp152=( struct _tuple2*) _region_malloc( r4, sizeof(
struct _tuple2)); _temp152->f1=( void*)({ struct Cyc_List_List* _temp154= x; if(
_temp154 == 0){ _throw( Null_Exception);} _temp154->hd;}); _temp152->f2=( void*)({
struct Cyc_List_List* _temp153= y; if( _temp153 == 0){ _throw( Null_Exception);}
_temp153->hd;}); _temp152;}); _temp151->tl= 0; _temp151;}); prev= result; x=({
struct Cyc_List_List* _temp155= x; if( _temp155 == 0){ _throw( Null_Exception);}
_temp155->tl;}); y=({ struct Cyc_List_List* _temp156= y; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->tl;}); while( x != 0? y != 0: 0) {({ struct
Cyc_List_List* _temp157= prev; if( _temp157 == 0){ _throw( Null_Exception);}
_temp157->tl=({ struct Cyc_List_List* _temp158=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp158->hd=( void*)({
struct _tuple2* _temp159=( struct _tuple2*) _region_malloc( r4, sizeof( struct
_tuple2)); _temp159->f1=( void*)({ struct Cyc_List_List* _temp161= x; if(
_temp161 == 0){ _throw( Null_Exception);} _temp161->hd;}); _temp159->f2=( void*)({
struct Cyc_List_List* _temp160= y; if( _temp160 == 0){ _throw( Null_Exception);}
_temp160->hd;}); _temp159;}); _temp158->tl= 0; _temp158;});}); prev=({ struct
Cyc_List_List* _temp162= prev; if( _temp162 == 0){ _throw( Null_Exception);}
_temp162->tl;}); x=({ struct Cyc_List_List* _temp163= x; if( _temp163 == 0){
_throw( Null_Exception);} _temp163->tl;}); y=({ struct Cyc_List_List* _temp164=
y; if( _temp164 == 0){ _throw( Null_Exception);} _temp164->tl;});} if( x != 0? 1:
y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp165=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp165[ 0]=({ struct Cyc_List_List_mismatch_struct _temp166; _temp166.tag= Cyc_List_List_mismatch_tag;
_temp166;}); _temp165;}));} return result;} struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( Cyc_Core_heap_region, Cyc_Core_heap_region,
x, y);} struct _tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x){ struct Cyc_List_List* result1;
struct Cyc_List_List* prev1; struct Cyc_List_List* result2; struct Cyc_List_List*
prev2; if( x == 0){ return({ struct _tuple0 _temp167; _temp167.f1= 0; _temp167.f2=
0; _temp167;});} prev1=( result1=({ struct Cyc_List_List* _temp168=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp168->hd=( void*)({
struct _tuple2* _temp170=( struct _tuple2*)({ struct Cyc_List_List* _temp169= x;
if( _temp169 == 0){ _throw( Null_Exception);} _temp169->hd;}); unsigned int
_temp171= 0; if( _temp171 >= 1u){ _throw( Null_Exception);} _temp170[ _temp171];}).f1;
_temp168->tl= 0; _temp168;})); prev2=( result2=({ struct Cyc_List_List* _temp172=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp172->hd=( void*)({ struct _tuple2* _temp174=( struct _tuple2*)({ struct Cyc_List_List*
_temp173= x; if( _temp173 == 0){ _throw( Null_Exception);} _temp173->hd;});
unsigned int _temp175= 0; if( _temp175 >= 1u){ _throw( Null_Exception);}
_temp174[ _temp175];}).f2; _temp172->tl= 0; _temp172;})); for( x=({ struct Cyc_List_List*
_temp176= x; if( _temp176 == 0){ _throw( Null_Exception);} _temp176->tl;}); x !=
0; x=({ struct Cyc_List_List* _temp177= x; if( _temp177 == 0){ _throw(
Null_Exception);} _temp177->tl;})){({ struct Cyc_List_List* _temp178= prev1; if(
_temp178 == 0){ _throw( Null_Exception);} _temp178->tl=({ struct Cyc_List_List*
_temp179=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp179->hd=( void*)({ struct _tuple2* _temp181=( struct _tuple2*)({ struct Cyc_List_List*
_temp180= x; if( _temp180 == 0){ _throw( Null_Exception);} _temp180->hd;});
unsigned int _temp182= 0; if( _temp182 >= 1u){ _throw( Null_Exception);}
_temp181[ _temp182];}).f1; _temp179->tl= 0; _temp179;});});({ struct Cyc_List_List*
_temp183= prev2; if( _temp183 == 0){ _throw( Null_Exception);} _temp183->tl=({
struct Cyc_List_List* _temp184=( struct Cyc_List_List*) _region_malloc( r4,
sizeof( struct Cyc_List_List)); _temp184->hd=( void*)({ struct _tuple2* _temp186=(
struct _tuple2*)({ struct Cyc_List_List* _temp185= x; if( _temp185 == 0){ _throw(
Null_Exception);} _temp185->hd;}); unsigned int _temp187= 0; if( _temp187 >= 1u){
_throw( Null_Exception);} _temp186[ _temp187];}).f2; _temp184->tl= 0; _temp184;});});
prev1=({ struct Cyc_List_List* _temp188= prev1; if( _temp188 == 0){ _throw(
Null_Exception);} _temp188->tl;}); prev2=({ struct Cyc_List_List* _temp189=
prev2; if( _temp189 == 0){ _throw( Null_Exception);} _temp189->tl;});} return({
struct _tuple0 _temp190; _temp190.f1= result1; _temp190.f2= result2; _temp190;});}
struct _tuple0 Cyc_List_split( struct Cyc_List_List* x){ return(( struct _tuple0(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x))
Cyc_List_rsplit)( Cyc_Core_heap_region, Cyc_Core_heap_region, x);} struct
_tuple1 Cyc_List_rsplit3( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct _RegionHandle* r5, struct Cyc_List_List* x){ struct Cyc_List_List*
result1; struct Cyc_List_List* prev1; struct Cyc_List_List* result2; struct Cyc_List_List*
prev2; struct Cyc_List_List* result3; struct Cyc_List_List* prev3; if( x == 0){
return({ struct _tuple1 _temp191; _temp191.f1= 0; _temp191.f2= 0; _temp191.f3= 0;
_temp191;});} prev1=( result1=({ struct Cyc_List_List* _temp192=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp192->hd=( void*)({
struct _tuple3* _temp194=( struct _tuple3*)({ struct Cyc_List_List* _temp193= x;
if( _temp193 == 0){ _throw( Null_Exception);} _temp193->hd;}); unsigned int
_temp195= 0; if( _temp195 >= 1u){ _throw( Null_Exception);} _temp194[ _temp195];}).f1;
_temp192->tl= 0; _temp192;})); prev2=( result2=({ struct Cyc_List_List* _temp196=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp196->hd=( void*)({ struct _tuple3* _temp198=( struct _tuple3*)({ struct Cyc_List_List*
_temp197= x; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->hd;});
unsigned int _temp199= 0; if( _temp199 >= 1u){ _throw( Null_Exception);}
_temp198[ _temp199];}).f2; _temp196->tl= 0; _temp196;})); prev3=( result3=({
struct Cyc_List_List* _temp200=( struct Cyc_List_List*) _region_malloc( r5,
sizeof( struct Cyc_List_List)); _temp200->hd=( void*)({ struct _tuple3* _temp202=(
struct _tuple3*)({ struct Cyc_List_List* _temp201= x; if( _temp201 == 0){ _throw(
Null_Exception);} _temp201->hd;}); unsigned int _temp203= 0; if( _temp203 >= 1u){
_throw( Null_Exception);} _temp202[ _temp203];}).f3; _temp200->tl= 0; _temp200;}));
for( x=({ struct Cyc_List_List* _temp204= x; if( _temp204 == 0){ _throw(
Null_Exception);} _temp204->tl;}); x != 0; x=({ struct Cyc_List_List* _temp205=
x; if( _temp205 == 0){ _throw( Null_Exception);} _temp205->tl;})){({ struct Cyc_List_List*
_temp206= prev1; if( _temp206 == 0){ _throw( Null_Exception);} _temp206->tl=({
struct Cyc_List_List* _temp207=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp207->hd=( void*)({ struct _tuple3* _temp209=(
struct _tuple3*)({ struct Cyc_List_List* _temp208= x; if( _temp208 == 0){ _throw(
Null_Exception);} _temp208->hd;}); unsigned int _temp210= 0; if( _temp210 >= 1u){
_throw( Null_Exception);} _temp209[ _temp210];}).f1; _temp207->tl= 0; _temp207;});});({
struct Cyc_List_List* _temp211= prev2; if( _temp211 == 0){ _throw(
Null_Exception);} _temp211->tl=({ struct Cyc_List_List* _temp212=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp212->hd=( void*)({
struct _tuple3* _temp214=( struct _tuple3*)({ struct Cyc_List_List* _temp213= x;
if( _temp213 == 0){ _throw( Null_Exception);} _temp213->hd;}); unsigned int
_temp215= 0; if( _temp215 >= 1u){ _throw( Null_Exception);} _temp214[ _temp215];}).f2;
_temp212->tl= 0; _temp212;});});({ struct Cyc_List_List* _temp216= prev3; if(
_temp216 == 0){ _throw( Null_Exception);} _temp216->tl=({ struct Cyc_List_List*
_temp217=( struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp217->hd=( void*)({ struct _tuple3* _temp219=( struct _tuple3*)({ struct Cyc_List_List*
_temp218= x; if( _temp218 == 0){ _throw( Null_Exception);} _temp218->hd;});
unsigned int _temp220= 0; if( _temp220 >= 1u){ _throw( Null_Exception);}
_temp219[ _temp220];}).f3; _temp217->tl= 0; _temp217;});}); prev1=({ struct Cyc_List_List*
_temp221= prev1; if( _temp221 == 0){ _throw( Null_Exception);} _temp221->tl;});
prev2=({ struct Cyc_List_List* _temp222= prev2; if( _temp222 == 0){ _throw(
Null_Exception);} _temp222->tl;}); prev3=({ struct Cyc_List_List* _temp223=
prev3; if( _temp223 == 0){ _throw( Null_Exception);} _temp223->tl;});} return({
struct _tuple1 _temp224; _temp224.f1= result1; _temp224.f2= result2; _temp224.f3=
result3; _temp224;});} struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x){
return(( struct _tuple1(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct _RegionHandle* r5, struct Cyc_List_List* x)) Cyc_List_rsplit3)( Cyc_Core_heap_region,
Cyc_Core_heap_region, Cyc_Core_heap_region, x);} int Cyc_List_memq( struct Cyc_List_List*
l, void* x){ while( l != 0) { if(( void*)({ struct Cyc_List_List* _temp225= l;
if( _temp225 == 0){ _throw( Null_Exception);} _temp225->hd;}) == x){ return 1;}
l=({ struct Cyc_List_List* _temp226= l; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->tl;});} return 0;} int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare((
void*)({ struct Cyc_List_List* _temp227= l; if( _temp227 == 0){ _throw(
Null_Exception);} _temp227->hd;}), x) == 0){ return 1;} l=({ struct Cyc_List_List*
_temp228= l; if( _temp228 == 0){ _throw( Null_Exception);} _temp228->tl;});}
return 0;} void* Cyc_List_assoc( struct Cyc_List_List* l, void* x){ while( l !=
0) { if(({ struct _tuple2* _temp230=( struct _tuple2*)({ struct Cyc_List_List*
_temp229= l; if( _temp229 == 0){ _throw( Null_Exception);} _temp229->hd;});
unsigned int _temp231= 0; if( _temp231 >= 1u){ _throw( Null_Exception);}
_temp230[ _temp231];}).f1 == x){ return({ struct _tuple2* _temp233=( struct
_tuple2*)({ struct Cyc_List_List* _temp232= l; if( _temp232 == 0){ _throw(
Null_Exception);} _temp232->hd;}); unsigned int _temp234= 0; if( _temp234 >= 1u){
_throw( Null_Exception);} _temp233[ _temp234];}).f2;} l=({ struct Cyc_List_List*
_temp235= l; if( _temp235 == 0){ _throw( Null_Exception);} _temp235->tl;});}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Not_found_struct*
_temp236=( struct Cyc_Core_Not_found_struct*) GC_malloc( sizeof( struct Cyc_Core_Not_found_struct));
_temp236[ 0]=({ struct Cyc_Core_Not_found_struct _temp237; _temp237.tag= Cyc_Core_Not_found_tag;
_temp237;}); _temp236;}));} void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare( x,({
struct _tuple2* _temp239=( struct _tuple2*)({ struct Cyc_List_List* _temp238= l;
if( _temp238 == 0){ _throw( Null_Exception);} _temp238->hd;}); unsigned int
_temp240= 0; if( _temp240 >= 1u){ _throw( Null_Exception);} _temp239[ _temp240];}).f1)
== 0){ return({ struct _tuple2* _temp242=( struct _tuple2*)({ struct Cyc_List_List*
_temp241= l; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->hd;});
unsigned int _temp243= 0; if( _temp243 >= 1u){ _throw( Null_Exception);}
_temp242[ _temp243];}).f2;} l=({ struct Cyc_List_List* _temp244= l; if( _temp244
== 0){ _throw( Null_Exception);} _temp244->tl;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp245=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp245[ 0]=({ struct
Cyc_Core_Not_found_struct _temp246; _temp246.tag= Cyc_Core_Not_found_tag;
_temp246;}); _temp245;}));} int Cyc_List_mem_assoc( struct Cyc_List_List* l,
void* x){ while( l != 0) { if(({ struct _tuple2* _temp248=( struct _tuple2*)({
struct Cyc_List_List* _temp247= l; if( _temp247 == 0){ _throw( Null_Exception);}
_temp247->hd;}); unsigned int _temp249= 0; if( _temp249 >= 1u){ _throw(
Null_Exception);} _temp248[ _temp249];}).f1 == x){ return 1;} l=({ struct Cyc_List_List*
_temp250= l; if( _temp250 == 0){ _throw( Null_Exception);} _temp250->tl;});}
return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*,
void*), struct Cyc_List_List* x){ while( x != 0) { if(({ struct Cyc_List_List*
_temp251= x; if( _temp251 == 0){ _throw( Null_Exception);} _temp251->tl;}) != 0){
if( compare(( void*)({ struct Cyc_List_List* _temp252= x; if( _temp252 == 0){
_throw( Null_Exception);} _temp252->hd;}),( void*)({ struct Cyc_List_List*
_temp254=({ struct Cyc_List_List* _temp253= x; if( _temp253 == 0){ _throw(
Null_Exception);} _temp253->tl;}); if( _temp254 == 0){ _throw( Null_Exception);}
_temp254->hd;})) == 0){ return({ struct Cyc_Core_Opt* _temp255=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*)(( void*)({ struct
Cyc_List_List* _temp256= x; if( _temp256 == 0){ _throw( Null_Exception);}
_temp256->hd;})); _temp255;});}} x=({ struct Cyc_List_List* _temp257= x; if(
_temp257 == 0){ _throw( Null_Exception);} _temp257->tl;});} return 0;} struct
_tagged_ptr0 Cyc_List_rto_array( struct _RegionHandle* r2, struct Cyc_List_List*
x){ int s; struct _tagged_ptr0 arr; s=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
x); arr=({ unsigned int _temp259=( unsigned int) s; void** _temp260=( void**)
_region_malloc( r2, sizeof( void*) * _temp259); unsigned int i; struct
_tagged_ptr0 _temp261={ _temp260, _temp260, _temp260 + _temp259}; for( i= 0; i <
_temp259; i ++){ _temp260[ i]=( void*)({ struct Cyc_List_List* _temp258= x; if(
_temp258 == 0){ _throw( Null_Exception);} _temp258->hd;});} _temp261;});{ int i=
0; for( 0; i < s; ++ i, x=({ struct Cyc_List_List* _temp262= x; if( _temp262 ==
0){ _throw( Null_Exception);} _temp262->tl;})){({ struct _tagged_ptr0 _temp263=
arr; void** _temp265= _temp263.curr + i; if( _temp263.base == 0? 1:( _temp265 <
_temp263.base? 1: _temp265 >= _temp263.last_plus_one)){ _throw( Null_Exception);}*
_temp265=( void*)({ struct Cyc_List_List* _temp266= x; if( _temp266 == 0){
_throw( Null_Exception);} _temp266->hd;});});}} return arr;} struct _tagged_ptr0
Cyc_List_to_array( struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)(
struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rto_array)( Cyc_Core_heap_region,
x);} struct Cyc_List_List* Cyc_List_rfrom_array( struct _RegionHandle* r2,
struct _tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{ unsigned int i=({
struct _tagged_ptr0 _temp267= arr;( unsigned int)( _temp267.last_plus_one -
_temp267.curr);}) - 1; for( 0; i >= 0; -- i){ ans=({ struct Cyc_List_List*
_temp268=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp268->hd=( void*)({ struct _tagged_ptr0 _temp269= arr; void** _temp271=
_temp269.curr +( int) i; if( _temp269.base == 0? 1:( _temp271 < _temp269.base? 1:
_temp271 >= _temp269.last_plus_one)){ _throw( Null_Exception);}* _temp271;});
_temp268->tl= ans; _temp268;});}} return ans;} struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_ptr0 arr)) Cyc_List_rfrom_array)( Cyc_Core_heap_region,
arr);} struct Cyc_List_List* Cyc_List_rtabulate( struct _RegionHandle* r, int n,
void*(* f)( int)){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i < n; ++ i){
res=({ struct Cyc_List_List* _temp272=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp272->hd=( void*) f( i); _temp272->tl= res;
_temp272;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
res);} struct Cyc_List_List* Cyc_List_tabulate( int n, void*(* f)( int)){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(* f)( int))) Cyc_List_rtabulate)(
Cyc_Core_heap_region, n, f);} struct Cyc_List_List* Cyc_List_rtabulate_c( struct
_RegionHandle* r, int n, void*(* f)( void*, int), void* env){ struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp273=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp273->hd=( void*) f( env, i); _temp273->tl= res; _temp273;});}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( res);}
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(*
f)( void*, int), void* env)) Cyc_List_rtabulate_c)( Cyc_Core_heap_region, n, f,
env);} int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List*
_temp274= l1; if( _temp274 == 0){ _throw( Null_Exception);} _temp274->tl;}), l2=({
struct Cyc_List_List* _temp275= l2; if( _temp275 == 0){ _throw( Null_Exception);}
_temp275->tl;})){ if( l1 == l2){ return 0;}{ int i= cmp(( void*)({ struct Cyc_List_List*
_temp276= l1; if( _temp276 == 0){ _throw( Null_Exception);} _temp276->hd;}),(
void*)({ struct Cyc_List_List* _temp277= l2; if( _temp277 == 0){ _throw(
Null_Exception);} _temp277->hd;})); if( i != 0){ return i;}}} if( l1 != 0){
return 1;} if( l2 != 0){ return - 1;} return 0;} int Cyc_List_list_prefix( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){ for( 0;
l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List* _temp278= l1; if( _temp278 == 0){
_throw( Null_Exception);} _temp278->tl;}), l2=({ struct Cyc_List_List* _temp279=
l2; if( _temp279 == 0){ _throw( Null_Exception);} _temp279->tl;})){ if( l1 == l2){
return 1;}{ int i= cmp(( void*)({ struct Cyc_List_List* _temp280= l1; if(
_temp280 == 0){ _throw( Null_Exception);} _temp280->hd;}),( void*)({ struct Cyc_List_List*
_temp281= l2; if( _temp281 == 0){ _throw( Null_Exception);} _temp281->hd;}));
if( i != 0){ return 0;}}} return l1 == 0;} struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp282=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp282->hd=( void*)(( void*)({ struct Cyc_List_List* _temp283= l; if( _temp283
== 0){ _throw( Null_Exception);} _temp283->hd;})); _temp282->tl= 0; _temp282;});
struct Cyc_List_List* end= result; for( 0; l != 0; l=({ struct Cyc_List_List*
_temp284= l; if( _temp284 == 0){ _throw( Null_Exception);} _temp284->tl;})){ if(
f( env,( void*)({ struct Cyc_List_List* _temp285= l; if( _temp285 == 0){ _throw(
Null_Exception);} _temp285->hd;}))){({ struct Cyc_List_List* _temp286= end; if(
_temp286 == 0){ _throw( Null_Exception);} _temp286->tl=({ struct Cyc_List_List*
_temp287=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp287->hd=( void*)(( void*)({ struct Cyc_List_List* _temp288= l; if( _temp288
== 0){ _throw( Null_Exception);} _temp288->hd;})); _temp287->tl= 0; _temp287;});});
end=({ struct Cyc_List_List* _temp289= end; if( _temp289 == 0){ _throw(
Null_Exception);} _temp289->tl;});}} return({ struct Cyc_List_List* _temp290=
result; if( _temp290 == 0){ _throw( Null_Exception);} _temp290->tl;});}} struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* f)( void*,
void*), void* env, struct Cyc_List_List* l)) Cyc_List_rfilter_c)( Cyc_Core_heap_region,
f, env, l);} struct Cyc_List_List* Cyc_List_rfilter( struct _RegionHandle* r2,
int(* f)( void*), struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp291=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp291->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp292= l; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->hd;}));
_temp291->tl= 0; _temp291;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l=({ struct Cyc_List_List* _temp293= l; if( _temp293 == 0){ _throw(
Null_Exception);} _temp293->tl;})){ if( f(( void*)({ struct Cyc_List_List*
_temp294= l; if( _temp294 == 0){ _throw( Null_Exception);} _temp294->hd;}))){({
struct Cyc_List_List* _temp295= end; if( _temp295 == 0){ _throw( Null_Exception);}
_temp295->tl=({ struct Cyc_List_List* _temp296=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp296->hd=( void*)(( void*)({
struct Cyc_List_List* _temp297= l; if( _temp297 == 0){ _throw( Null_Exception);}
_temp297->hd;})); _temp296->tl= 0; _temp296;});}); end=({ struct Cyc_List_List*
_temp298= end; if( _temp298 == 0){ _throw( Null_Exception);} _temp298->tl;});}}
return({ struct Cyc_List_List* _temp299= result; if( _temp299 == 0){ _throw(
Null_Exception);} _temp299->tl;});}} struct Cyc_List_List* Cyc_List_filter( int(*
f)( void*), struct Cyc_List_List* l){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* f)( void*), struct Cyc_List_List* l)) Cyc_List_rfilter)(
Cyc_Core_heap_region, f, l);}