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
struct Cyc_List_List* x){ int i= 0; while( x != 0) { ++ i; x=({ struct Cyc_List_List*
_temp0= x; if( _temp0 == 0){ _throw( Null_Exception);} _temp0->tl;});} return i;}
char Cyc_List_List_empty_tag[ 11u]="List_empty"; void* Cyc_List_hd( struct Cyc_List_List*
x){ if( x == 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_empty_struct*
_temp1=( struct Cyc_List_List_empty_struct*) GC_malloc( sizeof( struct Cyc_List_List_empty_struct));
_temp1[ 0]=({ struct Cyc_List_List_empty_struct _temp2; _temp2.tag= Cyc_List_List_empty_tag;
_temp2;}); _temp1;}));} return( void*)({ struct Cyc_List_List* _temp3= x; if(
_temp3 == 0){ _throw( Null_Exception);} _temp3->hd;});} struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x){ if( x == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_List_List_empty_struct* _temp4=( struct Cyc_List_List_empty_struct*)
GC_malloc( sizeof( struct Cyc_List_List_empty_struct)); _temp4[ 0]=({ struct Cyc_List_List_empty_struct
_temp5; _temp5.tag= Cyc_List_List_empty_tag; _temp5;}); _temp4;}));} return({
struct Cyc_List_List* _temp6= x; if( _temp6 == 0){ _throw( Null_Exception);}
_temp6->tl;});} struct Cyc_List_List* Cyc_List_copy( struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp7->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp8= x; if( _temp8 == 0){ _throw( Null_Exception);} _temp8->hd;})); _temp7->tl=
0; _temp7;}); prev= result; for( x=({ struct Cyc_List_List* _temp9= x; if(
_temp9 == 0){ _throw( Null_Exception);} _temp9->tl;}); x != 0; x=({ struct Cyc_List_List*
_temp10= x; if( _temp10 == 0){ _throw( Null_Exception);} _temp10->tl;})){({
struct Cyc_List_List* _temp11= prev; if( _temp11 == 0){ _throw( Null_Exception);}
_temp11->tl=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp12->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp13= x; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->hd;}));
_temp12->tl= 0; _temp12;});}); prev=({ struct Cyc_List_List* _temp14= prev; if(
_temp14 == 0){ _throw( Null_Exception);} _temp14->tl;});} return result;} struct
Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0){ return 0;}
result=({ struct Cyc_List_List* _temp15=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp15->hd=( void*) f(( void*)({ struct Cyc_List_List*
_temp16= x; if( _temp16 == 0){ _throw( Null_Exception);} _temp16->hd;}));
_temp15->tl= 0; _temp15;}); prev= result; for( x=({ struct Cyc_List_List*
_temp17= x; if( _temp17 == 0){ _throw( Null_Exception);} _temp17->tl;}); x != 0;
x=({ struct Cyc_List_List* _temp18= x; if( _temp18 == 0){ _throw( Null_Exception);}
_temp18->tl;})){ struct Cyc_List_List* temp=({ struct Cyc_List_List* _temp19=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp19->hd=(
void*) f(( void*)({ struct Cyc_List_List* _temp20= x; if( _temp20 == 0){ _throw(
Null_Exception);} _temp20->hd;})); _temp19->tl= 0; _temp19;});({ struct Cyc_List_List*
_temp21= prev; if( _temp21 == 0){ _throw( Null_Exception);} _temp21->tl= temp;});
prev= temp;} return result;} struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0){ return 0;} result=({ struct Cyc_List_List*
_temp22=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp22->hd=( void*) f( env,( void*)({ struct Cyc_List_List* _temp23= x; if(
_temp23 == 0){ _throw( Null_Exception);} _temp23->hd;})); _temp22->tl= 0;
_temp22;}); prev= result; for( x=({ struct Cyc_List_List* _temp24= x; if(
_temp24 == 0){ _throw( Null_Exception);} _temp24->tl;}); x != 0; x=({ struct Cyc_List_List*
_temp25= x; if( _temp25 == 0){ _throw( Null_Exception);} _temp25->tl;})){({
struct Cyc_List_List* _temp26= prev; if( _temp26 == 0){ _throw( Null_Exception);}
_temp26->tl=({ struct Cyc_List_List* _temp27=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp27->hd=( void*) f( env,( void*)({ struct
Cyc_List_List* _temp28= x; if( _temp28 == 0){ _throw( Null_Exception);} _temp28->hd;}));
_temp27->tl= 0; _temp27;});}); prev=({ struct Cyc_List_List* _temp29= prev; if(
_temp29 == 0){ _throw( Null_Exception);} _temp29->tl;});} return result;} char
Cyc_List_List_mismatch_tag[ 14u]="List_mismatch"; struct Cyc_List_List* Cyc_List_map2(
void*(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y){
struct Cyc_List_List* result; struct Cyc_List_List* prev; if( x == 0? y == 0: 0){
return 0;} if( x == 0? 1: y == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_List_List_mismatch_struct* _temp30=( struct Cyc_List_List_mismatch_struct*)
GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct)); _temp30[ 0]=({ struct
Cyc_List_List_mismatch_struct _temp31; _temp31.tag= Cyc_List_List_mismatch_tag;
_temp31;}); _temp30;}));} result=({ struct Cyc_List_List* _temp32=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp32->hd=( void*) f(( void*)({
struct Cyc_List_List* _temp33= x; if( _temp33 == 0){ _throw( Null_Exception);}
_temp33->hd;}),( void*)({ struct Cyc_List_List* _temp34= y; if( _temp34 == 0){
_throw( Null_Exception);} _temp34->hd;})); _temp32->tl= 0; _temp32;}); prev=
result; x=({ struct Cyc_List_List* _temp35= x; if( _temp35 == 0){ _throw(
Null_Exception);} _temp35->tl;}); y=({ struct Cyc_List_List* _temp36= y; if(
_temp36 == 0){ _throw( Null_Exception);} _temp36->tl;}); while( x != 0? y != 0:
0) {({ struct Cyc_List_List* _temp37= prev; if( _temp37 == 0){ _throw(
Null_Exception);} _temp37->tl=({ struct Cyc_List_List* _temp38=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp38->hd=( void*) f(( void*)({
struct Cyc_List_List* _temp39= x; if( _temp39 == 0){ _throw( Null_Exception);}
_temp39->hd;}),( void*)({ struct Cyc_List_List* _temp40= y; if( _temp40 == 0){
_throw( Null_Exception);} _temp40->hd;})); _temp38->tl= 0; _temp38;});}); prev=({
struct Cyc_List_List* _temp41= prev; if( _temp41 == 0){ _throw( Null_Exception);}
_temp41->tl;}); x=({ struct Cyc_List_List* _temp42= x; if( _temp42 == 0){ _throw(
Null_Exception);} _temp42->tl;}); y=({ struct Cyc_List_List* _temp43= y; if(
_temp43 == 0){ _throw( Null_Exception);} _temp43->tl;});} if( x != 0? 1: y != 0){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp44=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp44[ 0]=({ struct Cyc_List_List_mismatch_struct _temp45; _temp45.tag= Cyc_List_List_mismatch_tag;
_temp45;}); _temp44;}));} return result;} void Cyc_List_app( void*(* f)( void*),
struct Cyc_List_List* x){ while( x != 0) { f(( void*)({ struct Cyc_List_List*
_temp46= x; if( _temp46 == 0){ _throw( Null_Exception);} _temp46->hd;})); x=({
struct Cyc_List_List* _temp47= x; if( _temp47 == 0){ _throw( Null_Exception);}
_temp47->tl;});}} void Cyc_List_app_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0) { f( env,( void*)({ struct Cyc_List_List*
_temp48= x; if( _temp48 == 0){ _throw( Null_Exception);} _temp48->hd;})); x=({
struct Cyc_List_List* _temp49= x; if( _temp49 == 0){ _throw( Null_Exception);}
_temp49->tl;});}} void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x){
while( x != 0) { f(( void*)({ struct Cyc_List_List* _temp50= x; if( _temp50 == 0){
_throw( Null_Exception);} _temp50->hd;})); x=({ struct Cyc_List_List* _temp51= x;
if( _temp51 == 0){ _throw( Null_Exception);} _temp51->tl;});}} void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0) {
f( env,( void*)({ struct Cyc_List_List* _temp52= x; if( _temp52 == 0){ _throw(
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
_temp85->tl;}), accum));}} struct Cyc_List_List* Cyc_List_revappend( struct Cyc_List_List*
x, struct Cyc_List_List* y){ while( x != 0) { y=({ struct Cyc_List_List* _temp86=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp86->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp87= x; if( _temp87 == 0){ _throw(
Null_Exception);} _temp87->hd;})); _temp86->tl= y; _temp86;}); x=({ struct Cyc_List_List*
_temp88= x; if( _temp88 == 0){ _throw( Null_Exception);} _temp88->tl;});} return
y;} struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x){ if( x == 0){
return 0;} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_revappend)( x, 0);} struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x){ if( x == 0){ return x;} else{ struct Cyc_List_List*
first= x; struct Cyc_List_List* second=({ struct Cyc_List_List* _temp89= x; if(
_temp89 == 0){ _throw( Null_Exception);} _temp89->tl;});({ struct Cyc_List_List*
_temp90= x; if( _temp90 == 0){ _throw( Null_Exception);} _temp90->tl= 0;});
while( second != 0) { struct Cyc_List_List* temp=({ struct Cyc_List_List*
_temp91= second; if( _temp91 == 0){ _throw( Null_Exception);} _temp91->tl;});({
struct Cyc_List_List* _temp92= second; if( _temp92 == 0){ _throw( Null_Exception);}
_temp92->tl= first;}); first= second; second= temp;} return first;}} struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List*
result; struct Cyc_List_List* prev; if( x == 0){ return y;} if( y == 0){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_copy)( x);} result=({
struct Cyc_List_List* _temp93=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp93->hd=( void*)(( void*)({ struct Cyc_List_List* _temp94=
x; if( _temp94 == 0){ _throw( Null_Exception);} _temp94->hd;})); _temp93->tl= 0;
_temp93;}); prev= result; for( x=({ struct Cyc_List_List* _temp95= x; if(
_temp95 == 0){ _throw( Null_Exception);} _temp95->tl;}); x != 0; x=({ struct Cyc_List_List*
_temp96= x; if( _temp96 == 0){ _throw( Null_Exception);} _temp96->tl;})){({
struct Cyc_List_List* _temp97= prev; if( _temp97 == 0){ _throw( Null_Exception);}
_temp97->tl=({ struct Cyc_List_List* _temp98=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp98->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp99= x; if( _temp99 == 0){ _throw( Null_Exception);} _temp99->hd;}));
_temp98->tl= 0; _temp98;});}); prev=({ struct Cyc_List_List* _temp100= prev; if(
_temp100 == 0){ _throw( Null_Exception);} _temp100->tl;});}({ struct Cyc_List_List*
_temp101= prev; if( _temp101 == 0){ _throw( Null_Exception);} _temp101->tl= y;});
return result;} struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List*
x, struct Cyc_List_List* y){ struct Cyc_List_List* z; if( x == 0){ return y;}
if( y == 0){ return x;} for( z= x;({ struct Cyc_List_List* _temp102= z; if(
_temp102 == 0){ _throw( Null_Exception);} _temp102->tl;}) != 0; z=({ struct Cyc_List_List*
_temp103= z; if( _temp103 == 0){ _throw( Null_Exception);} _temp103->tl;})){;}({
struct Cyc_List_List* _temp104= z; if( _temp104 == 0){ _throw( Null_Exception);}
_temp104->tl= y;}); return x;} struct Cyc_List_List* Cyc_List_flatten( struct
Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct Cyc_List_List*, struct Cyc_List_List*), struct Cyc_List_List* x, struct
Cyc_List_List* accum)) Cyc_List_fold_right)(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append, x, 0);} struct Cyc_List_List*
Cyc_List_merge_sort( int(* less_eq)( void*, void*), struct Cyc_List_List* x){
struct Cyc_List_List* a= 0; struct Cyc_List_List* b= 0; if( x == 0? 1:({ struct
Cyc_List_List* _temp105= x; if( _temp105 == 0){ _throw( Null_Exception);}
_temp105->tl;}) == 0){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_copy)( x);} while( x != 0) { a=({ struct Cyc_List_List* _temp106=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp106->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp107= x; if( _temp107 == 0){ _throw(
Null_Exception);} _temp107->hd;})); _temp106->tl= a; _temp106;}); x=({ struct
Cyc_List_List* _temp108= x; if( _temp108 == 0){ _throw( Null_Exception);}
_temp108->tl;}); if( x != 0){ b=({ struct Cyc_List_List* _temp109=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp109->hd=( void*)(( void*)({
struct Cyc_List_List* _temp110= x; if( _temp110 == 0){ _throw( Null_Exception);}
_temp110->hd;})); _temp109->tl= b; _temp109;}); x=({ struct Cyc_List_List*
_temp111= x; if( _temp111 == 0){ _throw( Null_Exception);} _temp111->tl;});}} a=((
struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List* x))
Cyc_List_merge_sort)( less_eq, a); b=(( struct Cyc_List_List*(*)( int(* less_eq)(
void*, void*), struct Cyc_List_List* x)) Cyc_List_merge_sort)( less_eq, b);
return(( struct Cyc_List_List*(*)( int(* less_eq)( void*, void*), struct Cyc_List_List*
a, struct Cyc_List_List* b)) Cyc_List_merge)( less_eq, a, b);} struct Cyc_List_List*
Cyc_List_merge( int(* less_eq)( void*, void*), struct Cyc_List_List* a, struct
Cyc_List_List* b){ struct Cyc_List_List* c; struct Cyc_List_List* d; if( a == 0){
return b;} if( b == 0){ return a;} if( less_eq(( void*)({ struct Cyc_List_List*
_temp112= a; if( _temp112 == 0){ _throw( Null_Exception);} _temp112->hd;}),(
void*)({ struct Cyc_List_List* _temp113= b; if( _temp113 == 0){ _throw(
Null_Exception);} _temp113->hd;})) <= 0){ c= a; d= a; a=({ struct Cyc_List_List*
_temp114= a; if( _temp114 == 0){ _throw( Null_Exception);} _temp114->tl;});}
else{ c= b; d= b; b=({ struct Cyc_List_List* _temp115= b; if( _temp115 == 0){
_throw( Null_Exception);} _temp115->tl;});} while( a != 0? b != 0: 0) { if(
less_eq(( void*)({ struct Cyc_List_List* _temp116= a; if( _temp116 == 0){ _throw(
Null_Exception);} _temp116->hd;}),( void*)({ struct Cyc_List_List* _temp117= b;
if( _temp117 == 0){ _throw( Null_Exception);} _temp117->hd;})) <= 0){({ struct
Cyc_List_List* _temp118= c; if( _temp118 == 0){ _throw( Null_Exception);}
_temp118->tl= a;}); c= a; a=({ struct Cyc_List_List* _temp119= a; if( _temp119
== 0){ _throw( Null_Exception);} _temp119->tl;});} else{({ struct Cyc_List_List*
_temp120= c; if( _temp120 == 0){ _throw( Null_Exception);} _temp120->tl= b;}); c=
b; b=({ struct Cyc_List_List* _temp121= b; if( _temp121 == 0){ _throw(
Null_Exception);} _temp121->tl;});}} if( a == 0){({ struct Cyc_List_List*
_temp122= c; if( _temp122 == 0){ _throw( Null_Exception);} _temp122->tl= b;});}
else{({ struct Cyc_List_List* _temp123= c; if( _temp123 == 0){ _throw(
Null_Exception);} _temp123->tl= a;});} return d;} char Cyc_List_Nth_tag[ 4u]="Nth";
void* Cyc_List_nth( struct Cyc_List_List* x, int i){ while( i > 0? x != 0: 0) {
-- i; x=({ struct Cyc_List_List* _temp124= x; if( _temp124 == 0){ _throw(
Null_Exception);} _temp124->tl;});} if( i < 0? 1: x == 0){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_List_Nth_struct* _temp125=( struct Cyc_List_Nth_struct*)
GC_malloc( sizeof( struct Cyc_List_Nth_struct)); _temp125[ 0]=({ struct Cyc_List_Nth_struct
_temp126; _temp126.tag= Cyc_List_Nth_tag; _temp126;}); _temp125;}));} return(
void*)({ struct Cyc_List_List* _temp127= x; if( _temp127 == 0){ _throw(
Null_Exception);} _temp127->hd;});} int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x){ while( x != 0? pred(( void*)({ struct Cyc_List_List*
_temp128= x; if( _temp128 == 0){ _throw( Null_Exception);} _temp128->hd;})): 0) {
x=({ struct Cyc_List_List* _temp129= x; if( _temp129 == 0){ _throw(
Null_Exception);} _temp129->tl;});} return x == 0;} int Cyc_List_forall_c( int(*
pred)( void*, void*), void* env, struct Cyc_List_List* x){ while( x != 0? pred(
env,( void*)({ struct Cyc_List_List* _temp130= x; if( _temp130 == 0){ _throw(
Null_Exception);} _temp130->hd;})): 0) { x=({ struct Cyc_List_List* _temp131= x;
if( _temp131 == 0){ _throw( Null_Exception);} _temp131->tl;});} return x == 0;}
int Cyc_List_exists( int(* pred)( void*), struct Cyc_List_List* x){ while( x !=
0? ! pred(( void*)({ struct Cyc_List_List* _temp132= x; if( _temp132 == 0){
_throw( Null_Exception);} _temp132->hd;})): 0) { x=({ struct Cyc_List_List*
_temp133= x; if( _temp133 == 0){ _throw( Null_Exception);} _temp133->tl;});}
return x != 0;} int Cyc_List_exists_c( int(* pred)( void*, void*), void* env,
struct Cyc_List_List* x){ while( x != 0? ! pred( env,( void*)({ struct Cyc_List_List*
_temp134= x; if( _temp134 == 0){ _throw( Null_Exception);} _temp134->hd;})): 0) {
x=({ struct Cyc_List_List* _temp135= x; if( _temp135 == 0){ _throw(
Null_Exception);} _temp135->tl;});} return x != 0;} struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y){ struct Cyc_List_List* result;
struct Cyc_List_List* prev; if( x == 0? y == 0: 0){ return 0;} if( x == 0? 1: y
== 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp136=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp136[ 0]=({ struct Cyc_List_List_mismatch_struct _temp137; _temp137.tag= Cyc_List_List_mismatch_tag;
_temp137;}); _temp136;}));} result=({ struct Cyc_List_List* _temp138=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp138->hd=( void*)({
struct _tuple2* _temp139=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp139->f1=( void*)({ struct Cyc_List_List* _temp141= x; if( _temp141 == 0){
_throw( Null_Exception);} _temp141->hd;}); _temp139->f2=( void*)({ struct Cyc_List_List*
_temp140= y; if( _temp140 == 0){ _throw( Null_Exception);} _temp140->hd;});
_temp139;}); _temp138->tl= 0; _temp138;}); prev= result; x=({ struct Cyc_List_List*
_temp142= x; if( _temp142 == 0){ _throw( Null_Exception);} _temp142->tl;}); y=({
struct Cyc_List_List* _temp143= y; if( _temp143 == 0){ _throw( Null_Exception);}
_temp143->tl;}); while( x != 0? y != 0: 0) {({ struct Cyc_List_List* _temp144=
prev; if( _temp144 == 0){ _throw( Null_Exception);} _temp144->tl=({ struct Cyc_List_List*
_temp145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp145->hd=( void*)({ struct _tuple2* _temp146=( struct _tuple2*) GC_malloc(
sizeof( struct _tuple2)); _temp146->f1=( void*)({ struct Cyc_List_List* _temp148=
x; if( _temp148 == 0){ _throw( Null_Exception);} _temp148->hd;}); _temp146->f2=(
void*)({ struct Cyc_List_List* _temp147= y; if( _temp147 == 0){ _throw(
Null_Exception);} _temp147->hd;}); _temp146;}); _temp145->tl= 0; _temp145;});});
prev=({ struct Cyc_List_List* _temp149= prev; if( _temp149 == 0){ _throw(
Null_Exception);} _temp149->tl;}); x=({ struct Cyc_List_List* _temp150= x; if(
_temp150 == 0){ _throw( Null_Exception);} _temp150->tl;}); y=({ struct Cyc_List_List*
_temp151= y; if( _temp151 == 0){ _throw( Null_Exception);} _temp151->tl;});} if(
x != 0? 1: y != 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_List_List_mismatch_struct*
_temp152=( struct Cyc_List_List_mismatch_struct*) GC_malloc( sizeof( struct Cyc_List_List_mismatch_struct));
_temp152[ 0]=({ struct Cyc_List_List_mismatch_struct _temp153; _temp153.tag= Cyc_List_List_mismatch_tag;
_temp153;}); _temp152;}));} return result;} struct _tuple0 Cyc_List_split(
struct Cyc_List_List* x){ struct Cyc_List_List* result1; struct Cyc_List_List*
prev1; struct Cyc_List_List* result2; struct Cyc_List_List* prev2; if( x == 0){
return({ struct _tuple0 _temp154; _temp154.f1= 0; _temp154.f2= 0; _temp154;});}
prev1=( result1=({ struct Cyc_List_List* _temp155=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp155->hd=( void*)({ struct
_tuple2* _temp157=( struct _tuple2*)({ struct Cyc_List_List* _temp156= x; if(
_temp156 == 0){ _throw( Null_Exception);} _temp156->hd;}); unsigned int _temp158=
0; if( _temp158 >= 1u){ _throw( Null_Exception);} _temp157[ _temp158];}).f1;
_temp155->tl= 0; _temp155;})); prev2=( result2=({ struct Cyc_List_List* _temp159=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp159->hd=(
void*)({ struct _tuple2* _temp161=( struct _tuple2*)({ struct Cyc_List_List*
_temp160= x; if( _temp160 == 0){ _throw( Null_Exception);} _temp160->hd;});
unsigned int _temp162= 0; if( _temp162 >= 1u){ _throw( Null_Exception);}
_temp161[ _temp162];}).f2; _temp159->tl= 0; _temp159;})); for( x=({ struct Cyc_List_List*
_temp163= x; if( _temp163 == 0){ _throw( Null_Exception);} _temp163->tl;}); x !=
0; x=({ struct Cyc_List_List* _temp164= x; if( _temp164 == 0){ _throw(
Null_Exception);} _temp164->tl;})){({ struct Cyc_List_List* _temp165= prev1; if(
_temp165 == 0){ _throw( Null_Exception);} _temp165->tl=({ struct Cyc_List_List*
_temp166=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp166->hd=( void*)({ struct _tuple2* _temp168=( struct _tuple2*)({ struct Cyc_List_List*
_temp167= x; if( _temp167 == 0){ _throw( Null_Exception);} _temp167->hd;});
unsigned int _temp169= 0; if( _temp169 >= 1u){ _throw( Null_Exception);}
_temp168[ _temp169];}).f1; _temp166->tl= 0; _temp166;});});({ struct Cyc_List_List*
_temp170= prev2; if( _temp170 == 0){ _throw( Null_Exception);} _temp170->tl=({
struct Cyc_List_List* _temp171=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp171->hd=( void*)({ struct _tuple2* _temp173=(
struct _tuple2*)({ struct Cyc_List_List* _temp172= x; if( _temp172 == 0){ _throw(
Null_Exception);} _temp172->hd;}); unsigned int _temp174= 0; if( _temp174 >= 1u){
_throw( Null_Exception);} _temp173[ _temp174];}).f2; _temp171->tl= 0; _temp171;});});
prev1=({ struct Cyc_List_List* _temp175= prev1; if( _temp175 == 0){ _throw(
Null_Exception);} _temp175->tl;}); prev2=({ struct Cyc_List_List* _temp176=
prev2; if( _temp176 == 0){ _throw( Null_Exception);} _temp176->tl;});} return({
struct _tuple0 _temp177; _temp177.f1= result1; _temp177.f2= result2; _temp177;});}
struct _tuple1 Cyc_List_split3( struct Cyc_List_List* x){ struct Cyc_List_List*
result1; struct Cyc_List_List* prev1; struct Cyc_List_List* result2; struct Cyc_List_List*
prev2; struct Cyc_List_List* result3; struct Cyc_List_List* prev3; if( x == 0){
return({ struct _tuple1 _temp178; _temp178.f1= 0; _temp178.f2= 0; _temp178.f3= 0;
_temp178;});} prev1=( result1=({ struct Cyc_List_List* _temp179=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp179->hd=( void*)({ struct
_tuple3* _temp181=( struct _tuple3*)({ struct Cyc_List_List* _temp180= x; if(
_temp180 == 0){ _throw( Null_Exception);} _temp180->hd;}); unsigned int _temp182=
0; if( _temp182 >= 1u){ _throw( Null_Exception);} _temp181[ _temp182];}).f1;
_temp179->tl= 0; _temp179;})); prev2=( result2=({ struct Cyc_List_List* _temp183=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp183->hd=(
void*)({ struct _tuple3* _temp185=( struct _tuple3*)({ struct Cyc_List_List*
_temp184= x; if( _temp184 == 0){ _throw( Null_Exception);} _temp184->hd;});
unsigned int _temp186= 0; if( _temp186 >= 1u){ _throw( Null_Exception);}
_temp185[ _temp186];}).f2; _temp183->tl= 0; _temp183;})); prev3=( result3=({
struct Cyc_List_List* _temp187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp187->hd=( void*)({ struct _tuple3* _temp189=(
struct _tuple3*)({ struct Cyc_List_List* _temp188= x; if( _temp188 == 0){ _throw(
Null_Exception);} _temp188->hd;}); unsigned int _temp190= 0; if( _temp190 >= 1u){
_throw( Null_Exception);} _temp189[ _temp190];}).f3; _temp187->tl= 0; _temp187;}));
for( x=({ struct Cyc_List_List* _temp191= x; if( _temp191 == 0){ _throw(
Null_Exception);} _temp191->tl;}); x != 0; x=({ struct Cyc_List_List* _temp192=
x; if( _temp192 == 0){ _throw( Null_Exception);} _temp192->tl;})){({ struct Cyc_List_List*
_temp193= prev1; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->tl=({
struct Cyc_List_List* _temp194=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp194->hd=( void*)({ struct _tuple3* _temp196=(
struct _tuple3*)({ struct Cyc_List_List* _temp195= x; if( _temp195 == 0){ _throw(
Null_Exception);} _temp195->hd;}); unsigned int _temp197= 0; if( _temp197 >= 1u){
_throw( Null_Exception);} _temp196[ _temp197];}).f1; _temp194->tl= 0; _temp194;});});({
struct Cyc_List_List* _temp198= prev2; if( _temp198 == 0){ _throw(
Null_Exception);} _temp198->tl=({ struct Cyc_List_List* _temp199=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp199->hd=( void*)({ struct
_tuple3* _temp201=( struct _tuple3*)({ struct Cyc_List_List* _temp200= x; if(
_temp200 == 0){ _throw( Null_Exception);} _temp200->hd;}); unsigned int _temp202=
0; if( _temp202 >= 1u){ _throw( Null_Exception);} _temp201[ _temp202];}).f2;
_temp199->tl= 0; _temp199;});});({ struct Cyc_List_List* _temp203= prev3; if(
_temp203 == 0){ _throw( Null_Exception);} _temp203->tl=({ struct Cyc_List_List*
_temp204=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp204->hd=( void*)({ struct _tuple3* _temp206=( struct _tuple3*)({ struct Cyc_List_List*
_temp205= x; if( _temp205 == 0){ _throw( Null_Exception);} _temp205->hd;});
unsigned int _temp207= 0; if( _temp207 >= 1u){ _throw( Null_Exception);}
_temp206[ _temp207];}).f3; _temp204->tl= 0; _temp204;});}); prev1=({ struct Cyc_List_List*
_temp208= prev1; if( _temp208 == 0){ _throw( Null_Exception);} _temp208->tl;});
prev2=({ struct Cyc_List_List* _temp209= prev2; if( _temp209 == 0){ _throw(
Null_Exception);} _temp209->tl;}); prev3=({ struct Cyc_List_List* _temp210=
prev3; if( _temp210 == 0){ _throw( Null_Exception);} _temp210->tl;});} return({
struct _tuple1 _temp211; _temp211.f1= result1; _temp211.f2= result2; _temp211.f3=
result3; _temp211;});} int Cyc_List_memq( struct Cyc_List_List* l, void* x){
while( l != 0) { if(( void*)({ struct Cyc_List_List* _temp212= l; if( _temp212
== 0){ _throw( Null_Exception);} _temp212->hd;}) == x){ return 1;} l=({ struct
Cyc_List_List* _temp213= l; if( _temp213 == 0){ _throw( Null_Exception);}
_temp213->tl;});} return 0;} int Cyc_List_mem( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare(( void*)({
struct Cyc_List_List* _temp214= l; if( _temp214 == 0){ _throw( Null_Exception);}
_temp214->hd;}), x) == 0){ return 1;} l=({ struct Cyc_List_List* _temp215= l;
if( _temp215 == 0){ _throw( Null_Exception);} _temp215->tl;});} return 0;} void*
Cyc_List_assoc( struct Cyc_List_List* l, void* x){ while( l != 0) { if(({ struct
_tuple2* _temp217=( struct _tuple2*)({ struct Cyc_List_List* _temp216= l; if(
_temp216 == 0){ _throw( Null_Exception);} _temp216->hd;}); unsigned int _temp218=
0; if( _temp218 >= 1u){ _throw( Null_Exception);} _temp217[ _temp218];}).f1 == x){
return({ struct _tuple2* _temp220=( struct _tuple2*)({ struct Cyc_List_List*
_temp219= l; if( _temp219 == 0){ _throw( Null_Exception);} _temp219->hd;});
unsigned int _temp221= 0; if( _temp221 >= 1u){ _throw( Null_Exception);}
_temp220[ _temp221];}).f2;} l=({ struct Cyc_List_List* _temp222= l; if( _temp222
== 0){ _throw( Null_Exception);} _temp222->tl;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp223=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp223[ 0]=({ struct
Cyc_Core_Not_found_struct _temp224; _temp224.tag= Cyc_Core_Not_found_tag;
_temp224;}); _temp223;}));} void* Cyc_List_assoc_cmp( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x){ while( l != 0) { if( compare( x,({
struct _tuple2* _temp226=( struct _tuple2*)({ struct Cyc_List_List* _temp225= l;
if( _temp225 == 0){ _throw( Null_Exception);} _temp225->hd;}); unsigned int
_temp227= 0; if( _temp227 >= 1u){ _throw( Null_Exception);} _temp226[ _temp227];}).f1)
== 0){ return({ struct _tuple2* _temp229=( struct _tuple2*)({ struct Cyc_List_List*
_temp228= l; if( _temp228 == 0){ _throw( Null_Exception);} _temp228->hd;});
unsigned int _temp230= 0; if( _temp230 >= 1u){ _throw( Null_Exception);}
_temp229[ _temp230];}).f2;} l=({ struct Cyc_List_List* _temp231= l; if( _temp231
== 0){ _throw( Null_Exception);} _temp231->tl;});}( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Not_found_struct* _temp232=( struct Cyc_Core_Not_found_struct*)
GC_malloc( sizeof( struct Cyc_Core_Not_found_struct)); _temp232[ 0]=({ struct
Cyc_Core_Not_found_struct _temp233; _temp233.tag= Cyc_Core_Not_found_tag;
_temp233;}); _temp232;}));} int Cyc_List_mem_assoc( struct Cyc_List_List* l,
void* x){ while( l != 0) { if(({ struct _tuple2* _temp235=( struct _tuple2*)({
struct Cyc_List_List* _temp234= l; if( _temp234 == 0){ _throw( Null_Exception);}
_temp234->hd;}); unsigned int _temp236= 0; if( _temp236 >= 1u){ _throw(
Null_Exception);} _temp235[ _temp236];}).f1 == x){ return 1;} l=({ struct Cyc_List_List*
_temp237= l; if( _temp237 == 0){ _throw( Null_Exception);} _temp237->tl;});}
return 0;} struct Cyc_Core_Opt* Cyc_List_check_unique( int(* compare)( void*,
void*), struct Cyc_List_List* x){ while( x != 0) { if(({ struct Cyc_List_List*
_temp238= x; if( _temp238 == 0){ _throw( Null_Exception);} _temp238->tl;}) != 0){
if( compare(( void*)({ struct Cyc_List_List* _temp239= x; if( _temp239 == 0){
_throw( Null_Exception);} _temp239->hd;}),( void*)({ struct Cyc_List_List*
_temp241=({ struct Cyc_List_List* _temp240= x; if( _temp240 == 0){ _throw(
Null_Exception);} _temp240->tl;}); if( _temp241 == 0){ _throw( Null_Exception);}
_temp241->hd;})) == 0){ return({ struct Cyc_Core_Opt* _temp242=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp242->v=( void*)(( void*)({ struct
Cyc_List_List* _temp243= x; if( _temp243 == 0){ _throw( Null_Exception);}
_temp243->hd;})); _temp242;});}} x=({ struct Cyc_List_List* _temp244= x; if(
_temp244 == 0){ _throw( Null_Exception);} _temp244->tl;});} return 0;} struct
_tagged_ptr0 Cyc_List_to_array( struct Cyc_List_List* x){ int s; struct
_tagged_ptr0 arr; s=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( x);
arr=({ unsigned int _temp246=( unsigned int) s; void** _temp247=( void**)
GC_malloc( sizeof( void*) * _temp246); unsigned int i; struct _tagged_ptr0
_temp248={ _temp247, _temp247, _temp247 + _temp246}; for( i= 0; i < _temp246; i
++){ _temp247[ i]=( void*)({ struct Cyc_List_List* _temp245= x; if( _temp245 ==
0){ _throw( Null_Exception);} _temp245->hd;});} _temp248;});{ int i= 0; for( 0;
i < s; ++ i, x=({ struct Cyc_List_List* _temp249= x; if( _temp249 == 0){ _throw(
Null_Exception);} _temp249->tl;})){({ struct _tagged_ptr0 _temp250= arr; void**
_temp252= _temp250.curr + i; if( _temp250.base == 0? 1:( _temp252 < _temp250.base?
1: _temp252 >= _temp250.last_plus_one)){ _throw( Null_Exception);}* _temp252=(
void*)({ struct Cyc_List_List* _temp253= x; if( _temp253 == 0){ _throw(
Null_Exception);} _temp253->hd;});});}} return arr;} struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr){ struct Cyc_List_List* ans= 0;{ unsigned int i=({
struct _tagged_ptr0 _temp254= arr;( unsigned int)( _temp254.last_plus_one -
_temp254.curr);}) -( unsigned int) 1; for( 0; i >=( unsigned int) 0; -- i){ ans=({
struct Cyc_List_List* _temp255=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp255->hd=( void*)({ struct _tagged_ptr0 _temp256=
arr; void** _temp258= _temp256.curr +( int) i; if( _temp256.base == 0? 1:(
_temp258 < _temp256.base? 1: _temp258 >= _temp256.last_plus_one)){ _throw(
Null_Exception);}* _temp258;}); _temp255->tl= ans; _temp255;});}} return ans;}
struct Cyc_List_List* Cyc_List_tabulate( int n, void*(* f)( int)){ struct Cyc_List_List*
res= 0;{ int i= 0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp259=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp259->hd=(
void*) f( i); _temp259->tl= res; _temp259;});}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( res);} struct Cyc_List_List* Cyc_List_tabulate_c(
int n, void*(* f)( void*, int), void* env){ struct Cyc_List_List* res= 0;{ int i=
0; for( 0; i < n; ++ i){ res=({ struct Cyc_List_List* _temp260=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp260->hd=( void*) f( env, i);
_temp260->tl= res; _temp260;});}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( res);} int Cyc_List_list_cmp( int(* cmp)( void*, void*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0;
l1=({ struct Cyc_List_List* _temp261= l1; if( _temp261 == 0){ _throw(
Null_Exception);} _temp261->tl;}), l2=({ struct Cyc_List_List* _temp262= l2; if(
_temp262 == 0){ _throw( Null_Exception);} _temp262->tl;})){ if( l1 == l2){
return 0;}{ int i= cmp(( void*)({ struct Cyc_List_List* _temp263= l1; if(
_temp263 == 0){ _throw( Null_Exception);} _temp263->hd;}),( void*)({ struct Cyc_List_List*
_temp264= l2; if( _temp264 == 0){ _throw( Null_Exception);} _temp264->hd;}));
if( i != 0){ return i;}}} if( l1 != 0){ return 1;} if( l2 != 0){ return - 1;}
return 0;} int Cyc_List_list_prefix( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2){ for( 0; l1 != 0? l2 != 0: 0; l1=({ struct Cyc_List_List*
_temp265= l1; if( _temp265 == 0){ _throw( Null_Exception);} _temp265->tl;}), l2=({
struct Cyc_List_List* _temp266= l2; if( _temp266 == 0){ _throw( Null_Exception);}
_temp266->tl;})){ if( l1 == l2){ return 1;}{ int i= cmp(( void*)({ struct Cyc_List_List*
_temp267= l1; if( _temp267 == 0){ _throw( Null_Exception);} _temp267->hd;}),(
void*)({ struct Cyc_List_List* _temp268= l2; if( _temp268 == 0){ _throw(
Null_Exception);} _temp268->hd;})); if( i != 0){ return 0;}}} return l1 == 0;}
struct Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env,
struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List* result=({
struct Cyc_List_List* _temp269=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp269->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp270= l; if( _temp270 == 0){ _throw( Null_Exception);} _temp270->hd;}));
_temp269->tl= 0; _temp269;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l=({ struct Cyc_List_List* _temp271= l; if( _temp271 == 0){ _throw(
Null_Exception);} _temp271->tl;})){ if( f( env,( void*)({ struct Cyc_List_List*
_temp272= l; if( _temp272 == 0){ _throw( Null_Exception);} _temp272->hd;}))){({
struct Cyc_List_List* _temp273= end; if( _temp273 == 0){ _throw( Null_Exception);}
_temp273->tl=({ struct Cyc_List_List* _temp274=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp274->hd=( void*)(( void*)({
struct Cyc_List_List* _temp275= l; if( _temp275 == 0){ _throw( Null_Exception);}
_temp275->hd;})); _temp274->tl= 0; _temp274;});}); end=({ struct Cyc_List_List*
_temp276= end; if( _temp276 == 0){ _throw( Null_Exception);} _temp276->tl;});}}
return({ struct Cyc_List_List* _temp277= result; if( _temp277 == 0){ _throw(
Null_Exception);} _temp277->tl;});}} struct Cyc_List_List* Cyc_List_filter( int(*
f)( void*), struct Cyc_List_List* l){ if( l == 0){ return 0;}{ struct Cyc_List_List*
result=({ struct Cyc_List_List* _temp278=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp278->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp279= l; if( _temp279 == 0){ _throw( Null_Exception);} _temp279->hd;}));
_temp278->tl= 0; _temp278;}); struct Cyc_List_List* end= result; for( 0; l != 0;
l=({ struct Cyc_List_List* _temp280= l; if( _temp280 == 0){ _throw(
Null_Exception);} _temp280->tl;})){ if( f(( void*)({ struct Cyc_List_List*
_temp281= l; if( _temp281 == 0){ _throw( Null_Exception);} _temp281->hd;}))){({
struct Cyc_List_List* _temp282= end; if( _temp282 == 0){ _throw( Null_Exception);}
_temp282->tl=({ struct Cyc_List_List* _temp283=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp283->hd=( void*)(( void*)({
struct Cyc_List_List* _temp284= l; if( _temp284 == 0){ _throw( Null_Exception);}
_temp284->hd;})); _temp283->tl= 0; _temp283;});}); end=({ struct Cyc_List_List*
_temp285= end; if( _temp285 == 0){ _throw( Null_Exception);} _temp285->tl;});}}
return({ struct Cyc_List_List* _temp286= result; if( _temp286 == 0){ _throw(
Null_Exception);} _temp286->tl;});}}