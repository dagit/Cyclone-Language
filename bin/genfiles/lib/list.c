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
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rimp_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b); extern struct Cyc_List_List* Cyc_List_rmerge( struct
_RegionHandle*, int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b); extern struct Cyc_List_List* Cyc_List_imp_merge( int(*
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
Cyc_List_List* Cyc_List_imp_merge( int(* less_eq)( void*, void*), struct Cyc_List_List*
a, struct Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d;
if( a == 0){ return b;} if( b == 0){ return a;} if( less_eq(( void*)({ struct
Cyc_List_List* _temp105= a; if( _temp105 == 0){ _throw( Null_Exception);}
_temp105->hd;}),( void*)({ struct Cyc_List_List* _temp106= b; if( _temp106 == 0){
_throw( Null_Exception);} _temp106->hd;})) <= 0){ c= a; a=({ struct Cyc_List_List*
_temp107= a; if( _temp107 == 0){ _throw( Null_Exception);} _temp107->tl;});}
else{ c= b; b=({ struct Cyc_List_List* _temp108= b; if( _temp108 == 0){ _throw(
Null_Exception);} _temp108->tl;});} d= c; while( a != 0? b != 0: 0) { if(
less_eq(( void*)({ struct Cyc_List_List* _temp109= a; if( _temp109 == 0){ _throw(
Null_Exception);} _temp109->hd;}),( void*)({ struct Cyc_List_List* _temp110= b;
if( _temp110 == 0){ _throw( Null_Exception);} _temp110->hd;})) <= 0){({ struct
Cyc_List_List* _temp111= c; if( _temp111 == 0){ _throw( Null_Exception);}
_temp111->tl= a;}); c= a; a=({ struct Cyc_List_List* _temp112= a; if( _temp112
== 0){ _throw( Null_Exception);} _temp112->tl;});} else{({ struct Cyc_List_List*
_temp113= c; if( _temp113 == 0){ _throw( Null_Exception);} _temp113->tl= b;}); c=
b; b=({ struct Cyc_List_List* _temp114= b; if( _temp114 == 0){ _throw(
Null_Exception);} _temp114->tl;});}} if( a == 0){({ struct Cyc_List_List*
_temp115= c; if( _temp115 == 0){ _throw( Null_Exception);} _temp115->tl= b;});}
else{({ struct Cyc_List_List* _temp116= c; if( _temp116 == 0){ _throw(
Null_Exception);} _temp116->tl= a;});} return d;} struct Cyc_List_List* Cyc_List_rimp_merge_sort(
int(* less_eq)( void*, void*), struct Cyc_List_List* x){ if( x == 0? 1:({ struct
Cyc_List_List* _temp117= x; if( _temp117 == 0){ _throw( Null_Exception);}
_temp117->tl;}) == 0){ return x;}{ struct Cyc_List_List* a= x; struct Cyc_List_List*
aptr= a; struct Cyc_List_List* b=({ struct Cyc_List_List* _temp118= x; if(
_temp118 == 0){ _throw( Null_Exception);} _temp118->tl;}); struct Cyc_List_List*
bptr= b; x=({ struct Cyc_List_List* _temp119= b; if( _temp119 == 0){ _throw(
Null_Exception);} _temp119->tl;}); while( x != 0) {({ struct Cyc_List_List*
_temp120= aptr; if( _temp120 == 0){ _throw( Null_Exception);} _temp120->tl= x;});
aptr= x; x=({ struct Cyc_List_List* _temp121= x; if( _temp121 == 0){ _throw(
Null_Exception);} _temp121->tl;}); if( x != 0){({ struct Cyc_List_List* _temp122=
bptr; if( _temp122 == 0){ _throw( Null_Exception);} _temp122->tl= x;}); bptr= x;
x=({ struct Cyc_List_List* _temp123= x; if( _temp123 == 0){ _throw(
Null_Exception);} _temp123->tl;});}}({ struct Cyc_List_List* _temp124= aptr; if(
_temp124 == 0){ _throw( Null_Exception);} _temp124->tl= 0;});({ struct Cyc_List_List*
_temp125= bptr; if( _temp125 == 0){ _throw( Null_Exception);} _temp125->tl= 0;});
return(( struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List*
a, struct Cyc_List_List* b)) Cyc_List_imp_merge)( less_eq,(( struct Cyc_List_List*(*)(
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
if( less_eq(( void*)({ struct Cyc_List_List* _temp126= a; if( _temp126 == 0){
_throw( Null_Exception);} _temp126->hd;}),( void*)({ struct Cyc_List_List*
_temp127= b; if( _temp127 == 0){ _throw( Null_Exception);} _temp127->hd;})) <= 0){
c=({ struct Cyc_List_List* _temp128=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp128->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp129= a; if( _temp129 == 0){ _throw( Null_Exception);} _temp129->hd;}));
_temp128->tl= 0; _temp128;}); a=({ struct Cyc_List_List* _temp130= a; if(
_temp130 == 0){ _throw( Null_Exception);} _temp130->tl;});} else{ c=({ struct
Cyc_List_List* _temp131=( struct Cyc_List_List*) _region_malloc( r3, sizeof(
struct Cyc_List_List)); _temp131->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp132= b; if( _temp132 == 0){ _throw( Null_Exception);} _temp132->hd;}));
_temp131->tl= 0; _temp131;}); b=({ struct Cyc_List_List* _temp133= b; if(
_temp133 == 0){ _throw( Null_Exception);} _temp133->tl;});} d= c; while( a != 0?
b != 0: 0) { if( less_eq(( void*)({ struct Cyc_List_List* _temp134= a; if(
_temp134 == 0){ _throw( Null_Exception);} _temp134->hd;}),( void*)({ struct Cyc_List_List*
_temp135= b; if( _temp135 == 0){ _throw( Null_Exception);} _temp135->hd;})) <= 0){({
struct Cyc_List_List* _temp136= c; if( _temp136 == 0){ _throw( Null_Exception);}
_temp136->tl=({ struct Cyc_List_List* _temp137=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp137->hd=( void*)(( void*)({
struct Cyc_List_List* _temp138= a; if( _temp138 == 0){ _throw( Null_Exception);}
_temp138->hd;})); _temp137->tl= 0; _temp137;});}); c=({ struct Cyc_List_List*
_temp139= c; if( _temp139 == 0){ _throw( Null_Exception);} _temp139->tl;}); a=({
struct Cyc_List_List* _temp140= a; if( _temp140 == 0){ _throw( Null_Exception);}
_temp140->tl;});} else{({ struct Cyc_List_List* _temp141= c; if( _temp141 == 0){
_throw( Null_Exception);} _temp141->tl=({ struct Cyc_List_List* _temp142=(
struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp142->hd=( void*)(( void*)({ struct Cyc_List_List* _temp143= b; if( _temp143
== 0){ _throw( Null_Exception);} _temp143->hd;})); _temp142->tl= 0; _temp142;});});
c=({ struct Cyc_List_List* _temp144= c; if( _temp144 == 0){ _throw(
Null_Exception);} _temp144->tl;}); b=({ struct Cyc_List_List* _temp145= b; if(
_temp145 == 0){ _throw( Null_Exception);} _temp145->tl;});}} if( a == 0){({
struct Cyc_List_List* _temp146= c; if( _temp146 == 0){ _throw( Null_Exception);}
_temp146->tl=(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, struct Cyc_List_List*
x)) Cyc_List_rcopy)( r3, b);});} else{({ struct Cyc_List_List* _temp147= c; if(
_temp147 == 0){ _throw( Null_Exception);} _temp147->tl=(( struct Cyc_List_List*(*)(
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
x=({ struct Cyc_List_List* _temp148= x; if( _temp148 == 0){ _throw(
Null_Exception);} _temp148->tl;});} if( i < 0? 1: x == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_Nth_struct* _temp149=( struct Cyc_List_Nth_struct*)
GC_malloc( sizeof( struct Cyc_List_Nth_struct)); _temp149[ 0]=({ struct Cyc_List_Nth_struct
_temp150; _temp150.tag= Cyc_List_Nth_tag; _temp150;}); _temp149;}));} return(
void*)({ struct Cyc_List_List* _temp151= x; if( _temp151 == 0){ _throw(
Null_Exception);} _temp151->hd;});} int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x){ while( x != 0? pred(( void*)({ struct Cyc_List_List*
_temp152= x; if( _temp152 == 0){ _throw( Null_Exception);} _temp152->hd;})): 0) {
x=({ struct Cyc_List_List* _temp153= x; if( _temp153 == 0){ _throw(
Null_Exception);} _temp153->tl;});} return x == 0;} int Cyc_List_forall_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0? pred(
env,( void*)({ struct Cyc_List_List* _temp154= x; if( _temp154 == 0){ _throw(
Null_Exception);} _temp154->hd;})): 0) { x=({ struct Cyc_List_List* _temp155= x;
if( _temp155 == 0){ _throw( Null_Exception);} _temp155->tl;});} return x == 0;}
int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x){ while( x !=
0? ! pred(( void*)({ struct Cyc_List_List* _temp156= x; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->hd;})): 0) { x=({ struct Cyc_List_List*
_temp157= x; if( _temp157 == 0){ _throw( Null_Exception);} _temp157->tl;});}
return x != 0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? ! pred( env,( void*)({ struct Cyc_List_List*
_temp158= x; if( _temp158 == 0){ _throw( Null_Exception);} _temp158->hd;})): 0) {
x=({ struct Cyc_List_List* _temp159= x; if( _temp159 == 0){ _throw(
Null_Exception);} _temp159->tl;});} return x != 0;} struct Cyc_List_List* Cyc_List_rzip(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y){ struct Cyc_List_List* result; struct Cyc_List_List*
prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct* _temp160=(
struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp160[ 0]=({ struct Cyc_List_List_mismatch_struct _temp161; _temp161.tag= Cyc_List_List_mismatch_tag;
_temp161;}); _temp160;}));} result=({ struct Cyc_List_List* _temp162=( struct
Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List)); _temp162->hd=(
void*)({ struct _tuple2* _temp163=( struct _tuple2*) _region_malloc( r4, sizeof(
struct _tuple2)); _temp163->f1=( void*)({ struct Cyc_List_List* _temp165= x; if(
_temp165 == 0){ _throw( Null_Exception);} _temp165->hd;}); _temp163->f2=( void*)({
struct Cyc_List_List* _temp164= y; if( _temp164 == 0){ _throw( Null_Exception);}
_temp164->hd;}); _temp163;}); _temp162->tl= 0; _temp162;}); prev= result; x=({
struct Cyc_List_List* _temp166= x; if( _temp166 == 0){ _throw( Null_Exception);}
_temp166->tl;}); y=({ struct Cyc_List_List* _temp167= y; if( _temp167 == 0){
_throw( Null_Exception);} _temp167->tl;}); while( x != 0? y != 0: 0) {({ struct
Cyc_List_List* _temp168= prev; if( _temp168 == 0){ _throw( Null_Exception);}
_temp168->tl=({ struct Cyc_List_List* _temp169=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp169->hd=( void*)({
struct _tuple2* _temp170=( struct _tuple2*) _region_malloc( r4, sizeof( struct
_tuple2)); _temp170->f1=( void*)({ struct Cyc_List_List* _temp172= x; if(
_temp172 == 0){ _throw( Null_Exception);} _temp172->hd;}); _temp170->f2=( void*)({
struct Cyc_List_List* _temp171= y; if( _temp171 == 0){ _throw( Null_Exception);}
_temp171->hd;}); _temp170;}); _temp169->tl= 0; _temp169;});}); prev=({ struct
Cyc_List_List* _temp173= prev; if( _temp173 == 0){ _throw( Null_Exception);}
_temp173->tl;}); x=({ struct Cyc_List_List* _temp174= x; if( _temp174 == 0){
_throw( Null_Exception);} _temp174->tl;}); y=({ struct Cyc_List_List* _temp175=
y; if( _temp175 == 0){ _throw( Null_Exception);} _temp175->tl;});} if( x != 0? 1:
y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp176=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp176[ 0]=({ struct Cyc_List_List_mismatch_struct _temp177; _temp177.tag= Cyc_List_List_mismatch_tag;
_temp177;}); _temp176;}));} return result;} struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y){ return(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( Cyc_Core_heap_region, Cyc_Core_heap_region,
x, y);} struct _tuple0 Cyc_List_rsplit( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x){ struct Cyc_List_List* result1;
struct Cyc_List_List* prev1; struct Cyc_List_List* result2; struct Cyc_List_List*
prev2; if( x == 0){ return({ struct _tuple0 _temp178; _temp178.f1= 0; _temp178.f2=
0; _temp178;});} prev1=( result1=({ struct Cyc_List_List* _temp179=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp179->hd=( void*)({
struct _tuple2* _temp181=( struct _tuple2*)({ struct Cyc_List_List* _temp180= x;
if( _temp180 == 0){ _throw( Null_Exception);} _temp180->hd;}); unsigned int
_temp182= 0; if( _temp182 >= 1u){ _throw( Null_Exception);} _temp181[ _temp182];}).f1;
_temp179->tl= 0; _temp179;})); prev2=( result2=({ struct Cyc_List_List* _temp183=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp183->hd=( void*)({ struct _tuple2* _temp185=( struct _tuple2*)({ struct Cyc_List_List*
_temp184= x; if( _temp184 == 0){ _throw( Null_Exception);} _temp184->hd;});
unsigned int _temp186= 0; if( _temp186 >= 1u){ _throw( Null_Exception);}
_temp185[ _temp186];}).f2; _temp183->tl= 0; _temp183;})); for( x=({ struct Cyc_List_List*
_temp187= x; if( _temp187 == 0){ _throw( Null_Exception);} _temp187->tl;}); x !=
0; x=({ struct Cyc_List_List* _temp188= x; if( _temp188 == 0){ _throw(
Null_Exception);} _temp188->tl;})){({ struct Cyc_List_List* _temp189= prev1; if(
_temp189 == 0){ _throw( Null_Exception);} _temp189->tl=({ struct Cyc_List_List*
_temp190=( struct Cyc_List_List*) _region_malloc( r3, sizeof( struct Cyc_List_List));
_temp190->hd=( void*)({ struct _tuple2* _temp192=( struct _tuple2*)({ struct Cyc_List_List*
_temp191= x; if( _temp191 == 0){ _throw( Null_Exception);} _temp191->hd;});
unsigned int _temp193= 0; if( _temp193 >= 1u){ _throw( Null_Exception);}
_temp192[ _temp193];}).f1; _temp190->tl= 0; _temp190;});});({ struct Cyc_List_List*
_temp194= prev2; if( _temp194 == 0){ _throw( Null_Exception);} _temp194->tl=({
struct Cyc_List_List* _temp195=( struct Cyc_List_List*) _region_malloc( r4,
sizeof( struct Cyc_List_List)); _temp195->hd=( void*)({ struct _tuple2* _temp197=(
struct _tuple2*)({ struct Cyc_List_List* _temp196= x; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->hd;}); unsigned int _temp198= 0; if( _temp198 >= 1u){
_throw( Null_Exception);} _temp197[ _temp198];}).f2; _temp195->tl= 0; _temp195;});});
prev1=({ struct Cyc_List_List* _temp199= prev1; if( _temp199 == 0){ _throw(
Null_Exception);} _temp199->tl;}); prev2=({ struct Cyc_List_List* _temp200=
prev2; if( _temp200 == 0){ _throw( Null_Exception);} _temp200->tl;});} return({
struct _tuple0 _temp201; _temp201.f1= result1; _temp201.f2= result2; _temp201;});}
struct _tuple0 Cyc_List_split( struct Cyc_List_List* x){ return(( struct _tuple0(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x))
Cyc_List_rsplit)( Cyc_Core_heap_region, Cyc_Core_heap_region, x);} struct
_tuple1 Cyc_List_rsplit3( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct _RegionHandle* r5, struct Cyc_List_List* x){ struct Cyc_List_List*
result1; struct Cyc_List_List* prev1; struct Cyc_List_List* result2; struct Cyc_List_List*
prev2; struct Cyc_List_List* result3; struct Cyc_List_List* prev3; if( x == 0){
return({ struct _tuple1 _temp202; _temp202.f1= 0; _temp202.f2= 0; _temp202.f3= 0;
_temp202;});} prev1=( result1=({ struct Cyc_List_List* _temp203=( struct Cyc_List_List*)
_region_malloc( r3, sizeof( struct Cyc_List_List)); _temp203->hd=( void*)({
struct _tuple3* _temp205=( struct _tuple3*)({ struct Cyc_List_List* _temp204= x;
if( _temp204 == 0){ _throw( Null_Exception);} _temp204->hd;}); unsigned int
_temp206= 0; if( _temp206 >= 1u){ _throw( Null_Exception);} _temp205[ _temp206];}).f1;
_temp203->tl= 0; _temp203;})); prev2=( result2=({ struct Cyc_List_List* _temp207=(
struct Cyc_List_List*) _region_malloc( r4, sizeof( struct Cyc_List_List));
_temp207->hd=( void*)({ struct _tuple3* _temp209=( struct _tuple3*)({ struct Cyc_List_List*
_temp208= x; if( _temp208 == 0){ _throw( Null_Exception);} _temp208->hd;});
unsigned int _temp210= 0; if( _temp210 >= 1u){ _throw( Null_Exception);}
_temp209[ _temp210];}).f2; _temp207->tl= 0; _temp207;})); prev3=( result3=({
struct Cyc_List_List* _temp211=( struct Cyc_List_List*) _region_malloc( r5,
sizeof( struct Cyc_List_List)); _temp211->hd=( void*)({ struct _tuple3* _temp213=(
struct _tuple3*)({ struct Cyc_List_List* _temp212= x; if( _temp212 == 0){ _throw(
Null_Exception);} _temp212->hd;}); unsigned int _temp214= 0; if( _temp214 >= 1u){
_throw( Null_Exception);} _temp213[ _temp214];}).f3; _temp211->tl= 0; _temp211;}));
for( x=({ struct Cyc_List_List* _temp215= x; if( _temp215 == 0){ _throw(
Null_Exception);} _temp215->tl;}); x != 0; x=({ struct Cyc_List_List* _temp216=
x; if( _temp216 == 0){ _throw( Null_Exception);} _temp216->tl;})){({ struct Cyc_List_List*
_temp217= prev1; if( _temp217 == 0){ _throw( Null_Exception);} _temp217->tl=({
struct Cyc_List_List* _temp218=( struct Cyc_List_List*) _region_malloc( r3,
sizeof( struct Cyc_List_List)); _temp218->hd=( void*)({ struct _tuple3* _temp220=(
struct _tuple3*)({ struct Cyc_List_List* _temp219= x; if( _temp219 == 0){ _throw(
Null_Exception);} _temp219->hd;}); unsigned int _temp221= 0; if( _temp221 >= 1u){
_throw( Null_Exception);} _temp220[ _temp221];}).f1; _temp218->tl= 0; _temp218;});});({
struct Cyc_List_List* _temp222= prev2; if( _temp222 == 0){ _throw(
Null_Exception);} _temp222->tl=({ struct Cyc_List_List* _temp223=( struct Cyc_List_List*)
_region_malloc( r4, sizeof( struct Cyc_List_List)); _temp223->hd=( void*)({
struct _tuple3* _temp225=( struct _tuple3*)({ struct Cyc_List_List* _temp224= x;
if( _temp224 == 0){ _throw( Null_Exception);} _temp224->hd;}); unsigned int
_temp226= 0; if( _temp226 >= 1u){ _throw( Null_Exception);} _temp225[ _temp226];}).f2;
_temp223->tl= 0; _temp223;});});({ struct Cyc_List_List* _temp227= prev3; if(
_temp227 == 0){ _throw( Null_Exception);} _temp227->tl=({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) _region_malloc( r5, sizeof( struct Cyc_List_List));
_temp228->hd=( void*)({ struct _tuple3* _temp230=( struct _tuple3*)({ struct Cyc_List_List*
_temp229= x; if( _temp229 == 0){ _throw( Null_Exception);} _temp229->hd;});
unsigned int _temp231= 0; if( _temp231 >= 1u){ _throw( Null_Exception);}
_temp230[ _temp231];}).f3; _temp228->tl= 0; _temp228;});}); prev1=({ struct Cyc_List_List*
_temp232= prev1; if( _temp232 == 0){ _throw( Null_Exception);} _temp232->tl;});
prev2=({ struct Cyc_List_List* _temp233= prev2; if( _temp233 == 0){ _throw(
Null_Exception);} _temp233->tl;}); prev3=({ struct Cyc_List_List* _temp234=
prev3; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->tl;});} return({
struct _tuple1 _temp235; _temp235.f1= result1; _temp235.f2= result2; _temp235.f3=
result3; _temp235;});} struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x){
return(( struct _tuple1(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct _RegionHandle* r5, struct Cyc_List_List* x)) Cyc_List_rsplit3)( Cyc_Core_heap_region,
Cyc_Core_heap_region, Cyc_Core_heap_region, x);} int Cyc_List_memq( struct Cyc_List_List*
l, void* x){ while( l != 0) { if(( void*)({ struct Cyc_List_List* _temp236= l;
if( _temp236 == 0){ _throw( Null_Exception);} _temp236->hd;}) == x){ return 1;}
l=({ struct Cyc_List_List* _temp237= l; if( _temp237 == 0){ _throw(
Null_Exception);} _temp237->tl;});} return 0;} int Cyc_List_mem( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare((
void*)({ struct Cyc_List_List* _temp238= l; if( _temp238 == 0){ _throw(
Null_Exception);} _temp238->hd;}), x) == 0){ return 1;} l=({ struct Cyc_List_List*
_temp239= l; if( _temp239 == 0){ _throw( Null_Exception);} _temp239->tl;});}
return 0;} void* Cyc_List_assoc( struct Cyc_List_List* l, void* x){ while( l !=
0) { if(({ struct _tuple2* _temp241=( struct _tuple2*)({ struct Cyc_List_List*
_temp240= l; if( _temp240 == 0){ _throw( Null_Exception);} _temp240->hd;});
unsigned int _temp242= 0; if( _temp242 >= 1u){ _throw( Null_Exception);}
_temp241[ _temp242];}).f1 == x){ return({ struct _tuple2* _temp244=( struct
_tuple2*)({ struct Cyc_List_List* _temp243= l; if( _temp243 == 0){ _throw(
Null_Exception);} _temp243->hd;}); unsigned int _temp245= 0; if( _temp245 >= 1u){
_throw( Null_Exception);} _temp244[ _temp245];}).f2;} l=({ struct Cyc_List_List*
_temp246= l; if( _temp246 == 0){ _throw( Null_Exception);} _temp246->tl;});}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Not_found_struct*
_temp247=( struct Cyc_Core_Not_found_struct*) GC_malloc( sizeof( struct Cyc_Core_Not_found_struct));
_temp247[ 0]=({ struct Cyc_Core_Not_found_struct _temp248; _temp248.tag= Cyc_Core_Not_found_tag;
_temp248;}); _temp247;}));} void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare( x,({
struct _tuple2* _temp250=( struct _tuple2*)({ struct Cyc_List_List* _temp249= l;
if( _temp249 == 0){ _throw( Null_Exception);} _temp249->hd;}); unsigned int
_temp251= 0; if( _temp251 >= 1u){ _throw( Null_Exception);} _temp250[ _temp251];}).f1)
== 0){ return({ struct _tuple2* _temp253=( struct _tuple2*)({ struct Cyc_List_List*
_temp252= l; if( _temp252 == 0){ _throw( Null_Exception);} _temp252->hd;});
unsigned int _temp254= 0; if( _temp254 >= 1u){ _throw( Null_Exception);}
_temp253[ _temp254];}).f2;} l=({ struct Cyc_List_List* _temp255= l; if( _temp255
== 0){ _throw( Null_Exception);} _temp255->tl;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp256=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp256[ 0]=({ struct
Cyc_Core_Not_found_struct _temp257; _temp257.tag= Cyc_Core_Not_found_tag;
_temp257;}); _temp256;}));} int Cyc_List_mem_assoc( struct Cyc_List_List* l,
void* x){ while( l != 0) { if(({ struct _tuple2* _temp259=( struct _tuple2*)({
struct Cyc_List_List* _temp258= l; if( _temp258 == 0){ _throw( Null_Exception);}
_temp258->hd;}); unsigned int _temp260= 0; if( _temp260 >= 1u){ _throw(
Null_Exception);} _temp259[ _temp260];}).f1 == x){ return 1;} l=({ struct Cyc_List_List*
_temp261= l; if( _temp261 == 0){ _throw( Null_Exception);} _temp261->tl;});}
return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*,
void*), struct Cyc_List_List* x){ while( x != 0) { if(({ struct Cyc_List_List*
_temp262= x; if( _temp262 == 0){ _throw( Null_Exception);} _temp262->tl;}) != 0){
if( compare(( void*)({ struct Cyc_List_List* _temp263= x; if( _temp263 == 0){
_throw( Null_Exception);} _temp263->hd;}),( void*)({ struct Cyc_List_List*
_temp265=({ struct Cyc_List_List* _temp264= x; if( _temp264 == 0){ _throw(
Null_Exception);} _temp264->tl;}); if( _temp265 == 0){ _throw( Null_Exception);}
_temp265->hd;})) == 0){ return({ struct Cyc_Core_Opt* _temp266=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp266->v=( void*)(( void*)({ struct
Cyc_List_List* _temp267= x; if( _temp267 == 0){ _throw( Null_Exception);}
_temp267->hd;})); _temp266;});}} x=({ struct Cyc_List_List* _temp268= x; if(
_temp268 == 0){ _throw( Null_Exception);} _temp268->tl;});} return 0;} struct
_tagged_ptr0 Cyc_List_rto_array( struct _RegionHandle* r2, struct Cyc_List_List*
x){ int s; struct _tagged_ptr0 arr; s=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
x); arr=({ unsigned int _temp269=( unsigned int) s; void** _temp270=( void**)
_region_malloc( r2, sizeof( void*) * _temp269); struct _tagged_ptr0 _temp274={
_temp270, _temp270, _temp270 + _temp269};{ unsigned int _temp271= _temp269;
unsigned int i; for( i= 0; i < _temp271; i ++){ _temp270[ i]=( void*)({ struct
Cyc_List_List* _temp273= x; if( _temp273 == 0){ _throw( Null_Exception);}
_temp273->hd;});}}; _temp274;});{ int i= 0; for( 0; i < s; ++ i, x=({ struct Cyc_List_List*
_temp275= x; if( _temp275 == 0){ _throw( Null_Exception);} _temp275->tl;})){({
struct _tagged_ptr0 _temp276= arr; void** _temp278= _temp276.curr + i; if(
_temp276.base == 0? 1:( _temp278 < _temp276.base? 1: _temp278 >= _temp276.last_plus_one)){
_throw( Null_Exception);}* _temp278=( void*)({ struct Cyc_List_List* _temp279= x;
if( _temp279 == 0){ _throw( Null_Exception);} _temp279->hd;});});}} return arr;}
struct _tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x){ return(( struct
_tagged_ptr0(*)( struct _RegionHandle* r2, struct Cyc_List_List* x)) Cyc_List_rto_array)(
Cyc_Core_heap_region, x);} struct Cyc_List_List* Cyc_List_rfrom_array( struct
_RegionHandle* r2, struct _tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{
unsigned int i=({ struct _tagged_ptr0 _temp280= arr;( unsigned int)( _temp280.last_plus_one
- _temp280.curr);}) - 1; for( 0; i >= 0; -- i){ ans=({ struct Cyc_List_List*
_temp281=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp281->hd=( void*)({ struct _tagged_ptr0 _temp282= arr; void** _temp284=
_temp282.curr +( int) i; if( _temp282.base == 0? 1:( _temp284 < _temp282.base? 1:
_temp284 >= _temp282.last_plus_one)){ _throw( Null_Exception);}* _temp284;});
_temp281->tl= ans; _temp281;});}} return ans;} struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, struct _tagged_ptr0 arr)) Cyc_List_rfrom_array)( Cyc_Core_heap_region,
arr);} struct Cyc_List_List* Cyc_List_rtabulate( struct _RegionHandle* r, int n,
void*(* f)( int)){ struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i < n; ++ i){
res=({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp285->hd=( void*) f( i); _temp285->tl= res;
_temp285;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
res);} struct Cyc_List_List* Cyc_List_tabulate( int n, void*(* f)( int)){ return((
struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(* f)( int))) Cyc_List_rtabulate)(
Cyc_Core_heap_region, n, f);} struct Cyc_List_List* Cyc_List_rtabulate_c( struct
_RegionHandle* r, int n, void*(* f)( void*, int), void* env){ struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp286=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp286->hd=( void*) f( env, i); _temp286->tl= res; _temp286;});}} return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( res);}
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r, int n, void*(*
f)( void*, int), void* env)) Cyc_List_rtabulate_c)( Cyc_Core_heap_region, n, f,
env);} int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List*
_temp287= l1; if( _temp287 == 0){ _throw( Null_Exception);} _temp287->tl;}), l2=({
struct Cyc_List_List* _temp288= l2; if( _temp288 == 0){ _throw( Null_Exception);}
_temp288->tl;})){ if( l1 == l2){ return 0;}{ int i= cmp(( void*)({ struct Cyc_List_List*
_temp289= l1; if( _temp289 == 0){ _throw( Null_Exception);} _temp289->hd;}),(
void*)({ struct Cyc_List_List* _temp290= l2; if( _temp290 == 0){ _throw(
Null_Exception);} _temp290->hd;})); if( i != 0){ return i;}}} if( l1 != 0){
return 1;} if( l2 != 0){ return - 1;} return 0;} int Cyc_List_list_prefix( int(*
cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){ for( 0;
l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List* _temp291= l1; if( _temp291 == 0){
_throw( Null_Exception);} _temp291->tl;}), l2=({ struct Cyc_List_List* _temp292=
l2; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->tl;})){ if( l1 == l2){
return 1;}{ int i= cmp(( void*)({ struct Cyc_List_List* _temp293= l1; if(
_temp293 == 0){ _throw( Null_Exception);} _temp293->hd;}),( void*)({ struct Cyc_List_List*
_temp294= l2; if( _temp294 == 0){ _throw( Null_Exception);} _temp294->hd;}));
if( i != 0){ return 0;}}} return l1 == 0;} struct Cyc_List_List* Cyc_List_rfilter_c(
struct _RegionHandle* r2, int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({ struct Cyc_List_List*
_temp295=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp295->hd=( void*)(( void*)({ struct Cyc_List_List* _temp296= l; if( _temp296
== 0){ _throw( Null_Exception);} _temp296->hd;})); _temp295->tl= 0; _temp295;});
struct Cyc_List_List* end= result; for( 0; l != 0; l=({ struct Cyc_List_List*
_temp297= l; if( _temp297 == 0){ _throw( Null_Exception);} _temp297->tl;})){ if(
f( env,( void*)({ struct Cyc_List_List* _temp298= l; if( _temp298 == 0){ _throw(
Null_Exception);} _temp298->hd;}))){({ struct Cyc_List_List* _temp299= end; if(
_temp299 == 0){ _throw( Null_Exception);} _temp299->tl=({ struct Cyc_List_List*
_temp300=( struct Cyc_List_List*) _region_malloc( r2, sizeof( struct Cyc_List_List));
_temp300->hd=( void*)(( void*)({ struct Cyc_List_List* _temp301= l; if( _temp301
== 0){ _throw( Null_Exception);} _temp301->hd;})); _temp300->tl= 0; _temp300;});});
end=({ struct Cyc_List_List* _temp302= end; if( _temp302 == 0){ _throw(
Null_Exception);} _temp302->tl;});}} return({ struct Cyc_List_List* _temp303=
result; if( _temp303 == 0){ _throw( Null_Exception);} _temp303->tl;});}} struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l){ return(( struct Cyc_List_List*(*)( struct _RegionHandle* r2, int(* f)( void*,
void*), void* env, struct Cyc_List_List* l)) Cyc_List_rfilter_c)( Cyc_Core_heap_region,
f, env, l);} struct Cyc_List_List* Cyc_List_rfilter( struct _RegionHandle* r2,
int(* f)( void*), struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp304=( struct Cyc_List_List*) _region_malloc(
r2, sizeof( struct Cyc_List_List)); _temp304->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp305= l; if( _temp305 == 0){ _throw( Null_Exception);} _temp305->hd;}));
_temp304->tl= 0; _temp304;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l=({ struct Cyc_List_List* _temp306= l; if( _temp306 == 0){ _throw(
Null_Exception);} _temp306->tl;})){ if( f(( void*)({ struct Cyc_List_List*
_temp307= l; if( _temp307 == 0){ _throw( Null_Exception);} _temp307->hd;}))){({
struct Cyc_List_List* _temp308= end; if( _temp308 == 0){ _throw( Null_Exception);}
_temp308->tl=({ struct Cyc_List_List* _temp309=( struct Cyc_List_List*)
_region_malloc( r2, sizeof( struct Cyc_List_List)); _temp309->hd=( void*)(( void*)({
struct Cyc_List_List* _temp310= l; if( _temp310 == 0){ _throw( Null_Exception);}
_temp310->hd;})); _temp309->tl= 0; _temp309;});}); end=({ struct Cyc_List_List*
_temp311= end; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->tl;});}}
return({ struct Cyc_List_List* _temp312= result; if( _temp312 == 0){ _throw(
Null_Exception);} _temp312->tl;});}} struct Cyc_List_List* Cyc_List_filter( int(*
f)( void*), struct Cyc_List_List* l){ return(( struct Cyc_List_List*(*)( struct
_RegionHandle* r2, int(* f)( void*), struct Cyc_List_List* l)) Cyc_List_rfilter)(
Cyc_Core_heap_region, f, l);}