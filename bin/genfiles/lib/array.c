#include "cyc_include.h"

 struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; struct
_tuple0{ void* f1; void* f2; } ; struct _tagged_ptr1{ struct _tuple0* curr;
struct _tuple0* base; struct _tuple0* last_plus_one; } ; struct _tuple1{ struct
_tagged_ptr0 f1; struct _tagged_ptr0 f2; } ; typedef int Cyc_ptrdiff_t; typedef
unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern struct _tagged_ptr0 Cyc_List_to_array(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr); extern void Cyc_Array_qsort( int(* less_eq)( void*,
void*), struct _tagged_ptr0 x, int ofs, int len); extern void Cyc_Array_msort(
int(* less_eq)( void*, void*), struct _tagged_ptr0 x, int ofs, int len); extern
struct _tagged_ptr0 Cyc_Array_from_list( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_Array_to_list( struct _tagged_ptr0 x); extern struct
_tagged_ptr0 Cyc_Array_copy( struct _tagged_ptr0 x); extern struct _tagged_ptr0
Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x); extern struct
_tagged_ptr0 Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct
_tagged_ptr0 x); extern void Cyc_Array_imp_map( void*(* f)( void*), struct
_tagged_ptr0 x); extern void Cyc_Array_imp_map_c( void*(* f)( void*, void*),
void* env, struct _tagged_ptr0 x); extern char Cyc_Array_Array_mismatch_tag[ 15u];
struct Cyc_Array_Array_mismatch_struct{ char* tag; } ; extern struct
_tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct _tagged_ptr0 x,
struct _tagged_ptr0 y); extern void Cyc_Array_app( void*(* f)( void*), struct
_tagged_ptr0 x); extern void Cyc_Array_app_c( void*(* f)( void*, void*), void*
env, struct _tagged_ptr0 x); extern void Cyc_Array_iter( void(* f)( void*),
struct _tagged_ptr0 x); extern void Cyc_Array_iter_c( void(* f)( void*, void*),
void* env, struct _tagged_ptr0 x); extern void Cyc_Array_app2( void*(* f)( void*,
void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y); extern void Cyc_Array_iter2( void(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y);
extern void* Cyc_Array_fold_left( void*(* f)( void*, void*), void* accum, struct
_tagged_ptr0 x); extern void* Cyc_Array_fold_left_c( void*(* f)( void*, void*,
void*), void* env, void* accum, struct _tagged_ptr0 x); extern void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum); extern void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum);
extern struct _tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x); extern
void Cyc_Array_imp_rev( struct _tagged_ptr0 x); extern int Cyc_Array_forall( int(*
pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_forall_c( int(* pred)(
void*, void*), void* env, struct _tagged_ptr0 x); extern int Cyc_Array_exists(
int(* pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_exists_c( int(*
pred)( void*, void*), void* env, struct _tagged_ptr0 x); extern struct
_tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct _tagged_ptr0 y);
extern struct _tuple1 Cyc_Array_split( struct _tagged_ptr1 x); extern int Cyc_Array_memq(
struct _tagged_ptr0 l, void* x); extern int Cyc_Array_mem( int(* compare)( void*,
void*), struct _tagged_ptr0 l, void* x); extern struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt); static int Cyc_Array_sp[ 40u]={ 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; void Cyc_Array_qsort( int(* less_eq)( void*,
void*), struct _tagged_ptr0 arr, int base_ofs, int len){ void* temp; int sp_ofs;
int i; int j; int limit_ofs; if(( base_ofs < 0? 1:( unsigned int)( base_ofs +
len) >({ struct _tagged_ptr0 _temp0= arr;( unsigned int)( _temp0.last_plus_one -
_temp0.curr);}))? 1: len < 0){( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_InvalidArg_struct* _temp1=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp1[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp2; _temp2.tag= Cyc_Core_InvalidArg_tag; _temp2.f1=( struct _tagged_string)({
char* _temp3=( char*)"Array::qsort"; struct _tagged_string _temp4; _temp4.curr=
_temp3; _temp4.base= _temp3; _temp4.last_plus_one= _temp3 + 13; _temp4;});
_temp2;}); _temp1;}));} limit_ofs= base_ofs + len; sp_ofs= 0; for( 0; 1; 0){ if(
limit_ofs - base_ofs > 3){ temp=({ struct _tagged_ptr0 _temp5= arr; void**
_temp7= _temp5.curr +(( limit_ofs - base_ofs) / 2 + base_ofs); if( _temp7 <
_temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7;});({ struct _tagged_ptr0 _temp8= arr; void** _temp10= _temp8.curr +((
limit_ofs - base_ofs) / 2 + base_ofs); if( _temp10 < _temp8.base? 1: _temp10 >=
_temp8.last_plus_one){ _throw( Null_Exception);}* _temp10=({ struct _tagged_ptr0
_temp11= arr; void** _temp13= _temp11.curr + base_ofs; if( _temp13 < _temp11.base?
1: _temp13 >= _temp11.last_plus_one){ _throw( Null_Exception);}* _temp13;});});({
struct _tagged_ptr0 _temp14= arr; void** _temp16= _temp14.curr + base_ofs; if(
_temp16 < _temp14.base? 1: _temp16 >= _temp14.last_plus_one){ _throw(
Null_Exception);}* _temp16= temp;}); i= base_ofs + 1; j= limit_ofs - 1; if(
less_eq(({ struct _tagged_ptr0 _temp17= arr; void** _temp19= _temp17.curr + i;
if( _temp19 < _temp17.base? 1: _temp19 >= _temp17.last_plus_one){ _throw(
Null_Exception);}* _temp19;}),({ struct _tagged_ptr0 _temp20= arr; void**
_temp22= _temp20.curr + j; if( _temp22 < _temp20.base? 1: _temp22 >= _temp20.last_plus_one){
_throw( Null_Exception);}* _temp22;})) > 0){ temp=({ struct _tagged_ptr0 _temp23=
arr; void** _temp25= _temp23.curr + i; if( _temp25 < _temp23.base? 1: _temp25 >=
_temp23.last_plus_one){ _throw( Null_Exception);}* _temp25;});({ struct
_tagged_ptr0 _temp26= arr; void** _temp28= _temp26.curr + i; if( _temp28 <
_temp26.base? 1: _temp28 >= _temp26.last_plus_one){ _throw( Null_Exception);}*
_temp28=({ struct _tagged_ptr0 _temp29= arr; void** _temp31= _temp29.curr + j;
if( _temp31 < _temp29.base? 1: _temp31 >= _temp29.last_plus_one){ _throw(
Null_Exception);}* _temp31;});});({ struct _tagged_ptr0 _temp32= arr; void**
_temp34= _temp32.curr + j; if( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one){
_throw( Null_Exception);}* _temp34= temp;});} if( less_eq(({ struct _tagged_ptr0
_temp35= arr; void** _temp37= _temp35.curr + base_ofs; if( _temp37 < _temp35.base?
1: _temp37 >= _temp35.last_plus_one){ _throw( Null_Exception);}* _temp37;}),({
struct _tagged_ptr0 _temp38= arr; void** _temp40= _temp38.curr + j; if( _temp40
< _temp38.base? 1: _temp40 >= _temp38.last_plus_one){ _throw( Null_Exception);}*
_temp40;})) > 0){ temp=({ struct _tagged_ptr0 _temp41= arr; void** _temp43=
_temp41.curr + base_ofs; if( _temp43 < _temp41.base? 1: _temp43 >= _temp41.last_plus_one){
_throw( Null_Exception);}* _temp43;});({ struct _tagged_ptr0 _temp44= arr; void**
_temp46= _temp44.curr + base_ofs; if( _temp46 < _temp44.base? 1: _temp46 >=
_temp44.last_plus_one){ _throw( Null_Exception);}* _temp46=({ struct
_tagged_ptr0 _temp47= arr; void** _temp49= _temp47.curr + j; if( _temp49 <
_temp47.base? 1: _temp49 >= _temp47.last_plus_one){ _throw( Null_Exception);}*
_temp49;});});({ struct _tagged_ptr0 _temp50= arr; void** _temp52= _temp50.curr
+ j; if( _temp52 < _temp50.base? 1: _temp52 >= _temp50.last_plus_one){ _throw(
Null_Exception);}* _temp52= temp;});} if( less_eq(({ struct _tagged_ptr0 _temp53=
arr; void** _temp55= _temp53.curr + i; if( _temp55 < _temp53.base? 1: _temp55 >=
_temp53.last_plus_one){ _throw( Null_Exception);}* _temp55;}),({ struct
_tagged_ptr0 _temp56= arr; void** _temp58= _temp56.curr + base_ofs; if( _temp58
< _temp56.base? 1: _temp58 >= _temp56.last_plus_one){ _throw( Null_Exception);}*
_temp58;})) > 0){ temp=({ struct _tagged_ptr0 _temp59= arr; void** _temp61=
_temp59.curr + i; if( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one){
_throw( Null_Exception);}* _temp61;});({ struct _tagged_ptr0 _temp62= arr; void**
_temp64= _temp62.curr + i; if( _temp64 < _temp62.base? 1: _temp64 >= _temp62.last_plus_one){
_throw( Null_Exception);}* _temp64=({ struct _tagged_ptr0 _temp65= arr; void**
_temp67= _temp65.curr + base_ofs; if( _temp67 < _temp65.base? 1: _temp67 >=
_temp65.last_plus_one){ _throw( Null_Exception);}* _temp67;});});({ struct
_tagged_ptr0 _temp68= arr; void** _temp70= _temp68.curr + base_ofs; if( _temp70
< _temp68.base? 1: _temp70 >= _temp68.last_plus_one){ _throw( Null_Exception);}*
_temp70= temp;});} for( 0; 1; 0){ do { i ++;} while ( less_eq(({ struct
_tagged_ptr0 _temp71= arr; void** _temp73= _temp71.curr + i; if( _temp73 <
_temp71.base? 1: _temp73 >= _temp71.last_plus_one){ _throw( Null_Exception);}*
_temp73;}),({ struct _tagged_ptr0 _temp74= arr; void** _temp76= _temp74.curr +
base_ofs; if( _temp76 < _temp74.base? 1: _temp76 >= _temp74.last_plus_one){
_throw( Null_Exception);}* _temp76;})) < 0); do { j --;} while ( less_eq(({
struct _tagged_ptr0 _temp77= arr; void** _temp79= _temp77.curr + j; if( _temp79
< _temp77.base? 1: _temp79 >= _temp77.last_plus_one){ _throw( Null_Exception);}*
_temp79;}),({ struct _tagged_ptr0 _temp80= arr; void** _temp82= _temp80.curr +
base_ofs; if( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){
_throw( Null_Exception);}* _temp82;})) > 0); if( i > j){ break;} temp=({ struct
_tagged_ptr0 _temp83= arr; void** _temp85= _temp83.curr + i; if( _temp85 <
_temp83.base? 1: _temp85 >= _temp83.last_plus_one){ _throw( Null_Exception);}*
_temp85;});({ struct _tagged_ptr0 _temp86= arr; void** _temp88= _temp86.curr + i;
if( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one){ _throw(
Null_Exception);}* _temp88=({ struct _tagged_ptr0 _temp89= arr; void** _temp91=
_temp89.curr + j; if( _temp91 < _temp89.base? 1: _temp91 >= _temp89.last_plus_one){
_throw( Null_Exception);}* _temp91;});});({ struct _tagged_ptr0 _temp92= arr;
void** _temp94= _temp92.curr + j; if( _temp94 < _temp92.base? 1: _temp94 >=
_temp92.last_plus_one){ _throw( Null_Exception);}* _temp94= temp;});;} temp=({
struct _tagged_ptr0 _temp95= arr; void** _temp97= _temp95.curr + base_ofs; if(
_temp97 < _temp95.base? 1: _temp97 >= _temp95.last_plus_one){ _throw(
Null_Exception);}* _temp97;});({ struct _tagged_ptr0 _temp98= arr; void**
_temp100= _temp98.curr + base_ofs; if( _temp100 < _temp98.base? 1: _temp100 >=
_temp98.last_plus_one){ _throw( Null_Exception);}* _temp100=({ struct
_tagged_ptr0 _temp101= arr; void** _temp103= _temp101.curr + j; if( _temp103 <
_temp101.base? 1: _temp103 >= _temp101.last_plus_one){ _throw( Null_Exception);}*
_temp103;});});({ struct _tagged_ptr0 _temp104= arr; void** _temp106= _temp104.curr
+ j; if( _temp106 < _temp104.base? 1: _temp106 >= _temp104.last_plus_one){
_throw( Null_Exception);}* _temp106= temp;}); if( j - base_ofs > limit_ofs - i){({
int* _temp107=( int*) Cyc_Array_sp; unsigned int _temp108= sp_ofs; if( _temp108
>= 40u){ _throw( Null_Exception);} _temp107[ _temp108]= base_ofs;});({ int*
_temp109=( int*) Cyc_Array_sp; unsigned int _temp110= sp_ofs + 1; if( _temp110
>= 40u){ _throw( Null_Exception);} _temp109[ _temp110]= j;}); base_ofs= i;}
else{({ int* _temp111=( int*) Cyc_Array_sp; unsigned int _temp112= sp_ofs; if(
_temp112 >= 40u){ _throw( Null_Exception);} _temp111[ _temp112]= i;});({ int*
_temp113=( int*) Cyc_Array_sp; unsigned int _temp114= sp_ofs + 1; if( _temp114
>= 40u){ _throw( Null_Exception);} _temp113[ _temp114]= limit_ofs;}); limit_ofs=
j;} sp_ofs += 2;} else{ for( j= base_ofs, i= j + 1; i < limit_ofs; j= i, i ++){
for( 0; less_eq(({ struct _tagged_ptr0 _temp115= arr; void** _temp117= _temp115.curr
+ j; if( _temp117 < _temp115.base? 1: _temp117 >= _temp115.last_plus_one){
_throw( Null_Exception);}* _temp117;}),({ struct _tagged_ptr0 _temp118= arr;
void** _temp120= _temp118.curr +( j + 1); if( _temp120 < _temp118.base? 1:
_temp120 >= _temp118.last_plus_one){ _throw( Null_Exception);}* _temp120;})) > 0;
j --){ temp=({ struct _tagged_ptr0 _temp121= arr; void** _temp123= _temp121.curr
+ j; if( _temp123 < _temp121.base? 1: _temp123 >= _temp121.last_plus_one){
_throw( Null_Exception);}* _temp123;});({ struct _tagged_ptr0 _temp124= arr;
void** _temp126= _temp124.curr + j; if( _temp126 < _temp124.base? 1: _temp126 >=
_temp124.last_plus_one){ _throw( Null_Exception);}* _temp126=({ struct
_tagged_ptr0 _temp127= arr; void** _temp129= _temp127.curr +( j + 1); if(
_temp129 < _temp127.base? 1: _temp129 >= _temp127.last_plus_one){ _throw(
Null_Exception);}* _temp129;});});({ struct _tagged_ptr0 _temp130= arr; void**
_temp132= _temp130.curr +( j + 1); if( _temp132 < _temp130.base? 1: _temp132 >=
_temp130.last_plus_one){ _throw( Null_Exception);}* _temp132= temp;}); if( j ==
base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs=({ int* _temp133=(
int*) Cyc_Array_sp; unsigned int _temp134= sp_ofs; if( _temp134 >= 40u){ _throw(
Null_Exception);} _temp133[ _temp134];}); limit_ofs=({ int* _temp135=( int*) Cyc_Array_sp;
unsigned int _temp136= sp_ofs + 1; if( _temp136 >= 40u){ _throw( Null_Exception);}
_temp135[ _temp136];});} else{ break;}}}} void Cyc_Array_msort( int(* less_eq)(
void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){ if(( base_ofs <
0? 1:( unsigned int)( base_ofs + len) >({ struct _tagged_ptr0 _temp137= arr;(
unsigned int)( _temp137.last_plus_one - _temp137.curr);}))? 1: len < 0){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp138=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp138[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp139; _temp139.tag= Cyc_Core_InvalidArg_tag;
_temp139.f1=( struct _tagged_string)({ char* _temp140=( char*)"Array::msort";
struct _tagged_string _temp141; _temp141.curr= _temp140; _temp141.base= _temp140;
_temp141.last_plus_one= _temp140 + 13; _temp141;}); _temp139;}); _temp138;}));}{
int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({ unsigned int _temp145=(
unsigned int) len; void** _temp146=( void**) GC_malloc( sizeof( void*) *
_temp145); unsigned int i; struct _tagged_ptr0 _temp147={ _temp146, _temp146,
_temp146 + _temp145}; for( i= 0; i < _temp145; i ++){ _temp146[ i]=({ struct
_tagged_ptr0 _temp142= arr; void** _temp144= _temp142.curr + 0; if( _temp144 <
_temp142.base? 1: _temp144 >= _temp142.last_plus_one){ _throw( Null_Exception);}*
_temp144;});} _temp147;}); struct _tagged_ptr0 to=({ unsigned int _temp151=(
unsigned int) len; void** _temp152=( void**) GC_malloc( sizeof( void*) *
_temp151); unsigned int i; struct _tagged_ptr0 _temp153={ _temp152, _temp152,
_temp152 + _temp151}; for( i= 0; i < _temp151; i ++){ _temp152[ i]=({ struct
_tagged_ptr0 _temp148= arr; void** _temp150= _temp148.curr +( int)( i +(
unsigned int) base_ofs); if( _temp150 < _temp148.base? 1: _temp150 >= _temp148.last_plus_one){
_throw( Null_Exception);}* _temp150;});} _temp153;}); struct _tagged_ptr0 swap;
int stepsize; int start; int lstart; int lend; int rstart; int rend; int dest;
for( stepsize= 1; stepsize < len; stepsize= stepsize * 2){ swap= from; from= to;
to= swap; dest= 0; for( start= 0; start < limit_ofs; start= start + stepsize * 2){
lstart= start; rstart= start + stepsize < limit_ofs? start + stepsize: limit_ofs;
lend= rstart; rend= start + stepsize * 2 < limit_ofs? start + stepsize * 2:
limit_ofs; while( lstart < lend? rstart < rend: 0) { if( less_eq(({ struct
_tagged_ptr0 _temp154= from; void** _temp156= _temp154.curr + lstart; if(
_temp156 < _temp154.base? 1: _temp156 >= _temp154.last_plus_one){ _throw(
Null_Exception);}* _temp156;}),({ struct _tagged_ptr0 _temp157= from; void**
_temp159= _temp157.curr + rstart; if( _temp159 < _temp157.base? 1: _temp159 >=
_temp157.last_plus_one){ _throw( Null_Exception);}* _temp159;})) <= 0){({ struct
_tagged_ptr0 _temp160= to; void** _temp162= _temp160.curr +( dest ++); if(
_temp162 < _temp160.base? 1: _temp162 >= _temp160.last_plus_one){ _throw(
Null_Exception);}* _temp162=({ struct _tagged_ptr0 _temp163= from; void**
_temp165= _temp163.curr +( lstart ++); if( _temp165 < _temp163.base? 1: _temp165
>= _temp163.last_plus_one){ _throw( Null_Exception);}* _temp165;});});} else{({
struct _tagged_ptr0 _temp166= to; void** _temp168= _temp166.curr +( dest ++);
if( _temp168 < _temp166.base? 1: _temp168 >= _temp166.last_plus_one){ _throw(
Null_Exception);}* _temp168=({ struct _tagged_ptr0 _temp169= from; void**
_temp171= _temp169.curr +( rstart ++); if( _temp171 < _temp169.base? 1: _temp171
>= _temp169.last_plus_one){ _throw( Null_Exception);}* _temp171;});});}} while(
lstart < lend) {({ struct _tagged_ptr0 _temp172= to; void** _temp174= _temp172.curr
+( dest ++); if( _temp174 < _temp172.base? 1: _temp174 >= _temp172.last_plus_one){
_throw( Null_Exception);}* _temp174=({ struct _tagged_ptr0 _temp175= from; void**
_temp177= _temp175.curr +( lstart ++); if( _temp177 < _temp175.base? 1: _temp177
>= _temp175.last_plus_one){ _throw( Null_Exception);}* _temp177;});});} while(
rstart < rend) {({ struct _tagged_ptr0 _temp178= to; void** _temp180= _temp178.curr
+( dest ++); if( _temp180 < _temp178.base? 1: _temp180 >= _temp178.last_plus_one){
_throw( Null_Exception);}* _temp180=({ struct _tagged_ptr0 _temp181= from; void**
_temp183= _temp181.curr +( rstart ++); if( _temp183 < _temp181.base? 1: _temp183
>= _temp181.last_plus_one){ _throw( Null_Exception);}* _temp183;});});}}}{ int i=
0; for( 0; i < len; i ++){({ struct _tagged_ptr0 _temp184= arr; void** _temp186=
_temp184.curr +( i + base_ofs); if( _temp186 < _temp184.base? 1: _temp186 >=
_temp184.last_plus_one){ _throw( Null_Exception);}* _temp186=({ struct
_tagged_ptr0 _temp187= to; void** _temp189= _temp187.curr + i; if( _temp189 <
_temp187.base? 1: _temp189 >= _temp187.last_plus_one){ _throw( Null_Exception);}*
_temp189;});});}}}} struct _tagged_ptr0 Cyc_Array_from_list( struct Cyc_List_List*
x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List* x)) Cyc_List_to_array)(
x);} struct Cyc_List_List* Cyc_Array_to_list( struct _tagged_ptr0 x){ return((
struct Cyc_List_List*(*)( struct _tagged_ptr0 arr)) Cyc_List_from_array)( x);}
struct _tagged_ptr0 Cyc_Array_copy( struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp190= x;( unsigned int)( _temp190.last_plus_one -
_temp190.curr);}); return({ unsigned int _temp194=( unsigned int) sx; void**
_temp195=( void**) GC_malloc( sizeof( void*) * _temp194); unsigned int i; struct
_tagged_ptr0 _temp196={ _temp195, _temp195, _temp195 + _temp194}; for( i= 0; i <
_temp194; i ++){ _temp195[ i]=({ struct _tagged_ptr0 _temp191= x; void**
_temp193= _temp191.curr +( int) i; if( _temp193 < _temp191.base? 1: _temp193 >=
_temp191.last_plus_one){ _throw( Null_Exception);}* _temp193;});} _temp196;});}
struct _tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp197= x;( unsigned int)( _temp197.last_plus_one
- _temp197.curr);}); return({ unsigned int _temp201=( unsigned int) sx; void**
_temp202=( void**) GC_malloc( sizeof( void*) * _temp201); unsigned int i; struct
_tagged_ptr0 _temp203={ _temp202, _temp202, _temp202 + _temp201}; for( i= 0; i <
_temp201; i ++){ _temp202[ i]= f(({ struct _tagged_ptr0 _temp198= x; void**
_temp200= _temp198.curr +( int) i; if( _temp200 < _temp198.base? 1: _temp200 >=
_temp198.last_plus_one){ _throw( Null_Exception);}* _temp200;}));} _temp203;});}
struct _tagged_ptr0 Cyc_Array_map_c( void*(* f)( void*, void*), void* env,
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp204= x;(
unsigned int)( _temp204.last_plus_one - _temp204.curr);}); return({ unsigned int
_temp208=( unsigned int) sx; void** _temp209=( void**) GC_malloc( sizeof( void*)
* _temp208); unsigned int i; struct _tagged_ptr0 _temp210={ _temp209, _temp209,
_temp209 + _temp208}; for( i= 0; i < _temp208; i ++){ _temp209[ i]= f( env,({
struct _tagged_ptr0 _temp205= x; void** _temp207= _temp205.curr +( int) i; if(
_temp207 < _temp205.base? 1: _temp207 >= _temp205.last_plus_one){ _throw(
Null_Exception);}* _temp207;}));} _temp210;});} void Cyc_Array_imp_map( void*(*
f)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp211=
x;( unsigned int)( _temp211.last_plus_one - _temp211.curr);}); int i= 0; for( 0;
i < sx; i ++){({ struct _tagged_ptr0 _temp212= x; void** _temp214= _temp212.curr
+ i; if( _temp214 < _temp212.base? 1: _temp214 >= _temp212.last_plus_one){
_throw( Null_Exception);}* _temp214= f(({ struct _tagged_ptr0 _temp215= x; void**
_temp217= _temp215.curr + i; if( _temp217 < _temp215.base? 1: _temp217 >=
_temp215.last_plus_one){ _throw( Null_Exception);}* _temp217;}));});}} void Cyc_Array_imp_map_c(
void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp218= x;( unsigned int)( _temp218.last_plus_one -
_temp218.curr);}); int i= 0; for( 0; i < sx; i ++){({ struct _tagged_ptr0
_temp219= x; void** _temp221= _temp219.curr + i; if( _temp221 < _temp219.base? 1:
_temp221 >= _temp219.last_plus_one){ _throw( Null_Exception);}* _temp221= f( env,({
struct _tagged_ptr0 _temp222= x; void** _temp224= _temp222.curr + i; if(
_temp224 < _temp222.base? 1: _temp224 >= _temp222.last_plus_one){ _throw(
Null_Exception);}* _temp224;}));});}} char Cyc_Array_Array_mismatch_tag[ 15u]="Array_mismatch";
struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp225= x;( unsigned int)( _temp225.last_plus_one - _temp225.curr);}); if((
unsigned int) sx !=({ struct _tagged_ptr0 _temp226= y;( unsigned int)( _temp226.last_plus_one
- _temp226.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp227=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp227[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp228; _temp228.tag= Cyc_Array_Array_mismatch_tag; _temp228;}); _temp227;}));}
return({ unsigned int _temp235=( unsigned int) sx; void** _temp236=( void**)
GC_malloc( sizeof( void*) * _temp235); unsigned int i; struct _tagged_ptr0
_temp237={ _temp236, _temp236, _temp236 + _temp235}; for( i= 0; i < _temp235; i
++){ _temp236[ i]= f(({ struct _tagged_ptr0 _temp229= x; void** _temp231=
_temp229.curr +( int) i; if( _temp231 < _temp229.base? 1: _temp231 >= _temp229.last_plus_one){
_throw( Null_Exception);}* _temp231;}),({ struct _tagged_ptr0 _temp232= y; void**
_temp234= _temp232.curr +( int) i; if( _temp234 < _temp232.base? 1: _temp234 >=
_temp232.last_plus_one){ _throw( Null_Exception);}* _temp234;}));} _temp237;});}
void Cyc_Array_app( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp238= x;( unsigned int)( _temp238.last_plus_one -
_temp238.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp239= x; void** _temp241= _temp239.curr + i; if( _temp241 < _temp239.base? 1:
_temp241 >= _temp239.last_plus_one){ _throw( Null_Exception);}* _temp241;}));}}
void Cyc_Array_app_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp242= x;( unsigned int)( _temp242.last_plus_one
- _temp242.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,({ struct
_tagged_ptr0 _temp243= x; void** _temp245= _temp243.curr + i; if( _temp245 <
_temp243.base? 1: _temp245 >= _temp243.last_plus_one){ _throw( Null_Exception);}*
_temp245;}));}} void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp246= x;( unsigned int)( _temp246.last_plus_one
- _temp246.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp247= x; void** _temp249= _temp247.curr + i; if( _temp249 < _temp247.base? 1:
_temp249 >= _temp247.last_plus_one){ _throw( Null_Exception);}* _temp249;}));}}
void Cyc_Array_iter_c( void(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp250= x;( unsigned int)( _temp250.last_plus_one
- _temp250.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,({ struct
_tagged_ptr0 _temp251= x; void** _temp253= _temp251.curr + i; if( _temp253 <
_temp251.base? 1: _temp253 >= _temp251.last_plus_one){ _throw( Null_Exception);}*
_temp253;}));}} void Cyc_Array_app2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp254= x;( unsigned int)( _temp254.last_plus_one - _temp254.curr);}); if((
unsigned int) sx !=({ struct _tagged_ptr0 _temp255= y;( unsigned int)( _temp255.last_plus_one
- _temp255.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp256=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp256[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp257; _temp257.tag= Cyc_Array_Array_mismatch_tag; _temp257;}); _temp256;}));}{
int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0 _temp258= x; void**
_temp260= _temp258.curr + i; if( _temp260 < _temp258.base? 1: _temp260 >=
_temp258.last_plus_one){ _throw( Null_Exception);}* _temp260;}),({ struct
_tagged_ptr0 _temp261= y; void** _temp263= _temp261.curr + i; if( _temp263 <
_temp261.base? 1: _temp263 >= _temp261.last_plus_one){ _throw( Null_Exception);}*
_temp263;}));}}} void Cyc_Array_app2_c( void*(* f)( void*, void*, void*), void*
env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp264= x;( unsigned int)( _temp264.last_plus_one - _temp264.curr);});
if(( unsigned int) sx !=({ struct _tagged_ptr0 _temp265= y;( unsigned int)(
_temp265.last_plus_one - _temp265.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp266=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp266[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp267; _temp267.tag=
Cyc_Array_Array_mismatch_tag; _temp267;}); _temp266;}));}{ int i= 0; for( 0; i <
sx; i ++){ f( env,({ struct _tagged_ptr0 _temp268= x; void** _temp270= _temp268.curr
+ i; if( _temp270 < _temp268.base? 1: _temp270 >= _temp268.last_plus_one){
_throw( Null_Exception);}* _temp270;}),({ struct _tagged_ptr0 _temp271= y; void**
_temp273= _temp271.curr + i; if( _temp273 < _temp271.base? 1: _temp273 >=
_temp271.last_plus_one){ _throw( Null_Exception);}* _temp273;}));}}} void Cyc_Array_iter2(
void(* f)( void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=(
int)({ struct _tagged_ptr0 _temp274= x;( unsigned int)( _temp274.last_plus_one -
_temp274.curr);}); if(( unsigned int) sx !=({ struct _tagged_ptr0 _temp275= y;(
unsigned int)( _temp275.last_plus_one - _temp275.curr);})){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp276=(
struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp276[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp277; _temp277.tag=
Cyc_Array_Array_mismatch_tag; _temp277;}); _temp276;}));}{ int i= 0; for( 0; i <
sx; i ++){ f(({ struct _tagged_ptr0 _temp278= x; void** _temp280= _temp278.curr
+ i; if( _temp280 < _temp278.base? 1: _temp280 >= _temp278.last_plus_one){
_throw( Null_Exception);}* _temp280;}),({ struct _tagged_ptr0 _temp281= y; void**
_temp283= _temp281.curr + i; if( _temp283 < _temp281.base? 1: _temp283 >=
_temp281.last_plus_one){ _throw( Null_Exception);}* _temp283;}));}}} void Cyc_Array_iter2_c(
void(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp284= x;( unsigned int)(
_temp284.last_plus_one - _temp284.curr);}); if(( unsigned int) sx !=({ struct
_tagged_ptr0 _temp285= y;( unsigned int)( _temp285.last_plus_one - _temp285.curr);})){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp286=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp286[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp287; _temp287.tag= Cyc_Array_Array_mismatch_tag; _temp287;}); _temp286;}));}{
int i= 0; for( 0; i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp288= x; void**
_temp290= _temp288.curr + i; if( _temp290 < _temp288.base? 1: _temp290 >=
_temp288.last_plus_one){ _throw( Null_Exception);}* _temp290;}),({ struct
_tagged_ptr0 _temp291= y; void** _temp293= _temp291.curr + i; if( _temp293 <
_temp291.base? 1: _temp293 >= _temp291.last_plus_one){ _throw( Null_Exception);}*
_temp293;}));}}} void* Cyc_Array_fold_left( void*(* f)( void*, void*), void*
accum, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp294= x;(
unsigned int)( _temp294.last_plus_one - _temp294.curr);});{ int i= 0; for( 0; i
< sx; i ++){ accum= f( accum,({ struct _tagged_ptr0 _temp295= x; void** _temp297=
_temp295.curr + i; if( _temp297 < _temp295.base? 1: _temp297 >= _temp295.last_plus_one){
_throw( Null_Exception);}* _temp297;}));}} return accum;} void* Cyc_Array_fold_left_c(
void*(* f)( void*, void*, void*), void* env, void* accum, struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp298= x;( unsigned int)( _temp298.last_plus_one
- _temp298.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,({
struct _tagged_ptr0 _temp299= x; void** _temp301= _temp299.curr + i; if(
_temp301 < _temp299.base? 1: _temp301 >= _temp299.last_plus_one){ _throw(
Null_Exception);}* _temp301;}));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({
struct _tagged_ptr0 _temp302= x;( unsigned int)( _temp302.last_plus_one -
_temp302.curr);}) -( unsigned int) 1); for( 0; i >= 0; i --){ accum= f(({ struct
_tagged_ptr0 _temp303= x; void** _temp305= _temp303.curr + i; if( _temp305 <
_temp303.base? 1: _temp305 >= _temp303.last_plus_one){ _throw( Null_Exception);}*
_temp305;}), accum);}} return accum;} void* Cyc_Array_fold_right_c( void*(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum){{ int i=(
int)(({ struct _tagged_ptr0 _temp306= x;( unsigned int)( _temp306.last_plus_one
- _temp306.curr);}) -( unsigned int) 1); for( 0; i >= 0; i --){ accum= f( env,({
struct _tagged_ptr0 _temp307= x; void** _temp309= _temp307.curr + i; if(
_temp309 < _temp307.base? 1: _temp309 >= _temp307.last_plus_one){ _throw(
Null_Exception);}* _temp309;}), accum);}} return accum;} struct _tagged_ptr0 Cyc_Array_rev_copy(
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp310= x;(
unsigned int)( _temp310.last_plus_one - _temp310.curr);}); int n= sx - 1; return({
unsigned int _temp314=( unsigned int) sx; void** _temp315=( void**) GC_malloc(
sizeof( void*) * _temp314); unsigned int i; struct _tagged_ptr0 _temp316={
_temp315, _temp315, _temp315 + _temp314}; for( i= 0; i < _temp314; i ++){
_temp315[ i]=({ struct _tagged_ptr0 _temp311= x; void** _temp313= _temp311.curr
+( int)(( unsigned int) n - i); if( _temp313 < _temp311.base? 1: _temp313 >=
_temp311.last_plus_one){ _throw( Null_Exception);}* _temp313;});} _temp316;});}
void Cyc_Array_imp_rev( struct _tagged_ptr0 x){ void* temp; int i= 0; int j=(
int)(({ struct _tagged_ptr0 _temp317= x;( unsigned int)( _temp317.last_plus_one
- _temp317.curr);}) -( unsigned int) 1); while( i < j) { temp=({ struct
_tagged_ptr0 _temp318= x; void** _temp320= _temp318.curr + i; if( _temp320 <
_temp318.base? 1: _temp320 >= _temp318.last_plus_one){ _throw( Null_Exception);}*
_temp320;});({ struct _tagged_ptr0 _temp321= x; void** _temp323= _temp321.curr +
i; if( _temp323 < _temp321.base? 1: _temp323 >= _temp321.last_plus_one){ _throw(
Null_Exception);}* _temp323=({ struct _tagged_ptr0 _temp324= x; void** _temp326=
_temp324.curr + j; if( _temp326 < _temp324.base? 1: _temp326 >= _temp324.last_plus_one){
_throw( Null_Exception);}* _temp326;});});({ struct _tagged_ptr0 _temp327= x;
void** _temp329= _temp327.curr + j; if( _temp329 < _temp327.base? 1: _temp329 >=
_temp327.last_plus_one){ _throw( Null_Exception);}* _temp329= temp;}); i ++; j
--;}} int Cyc_Array_forall( int(* pred)( void*), struct _tagged_ptr0 x){ int sx=(
int)({ struct _tagged_ptr0 _temp330= x;( unsigned int)( _temp330.last_plus_one -
_temp330.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( ! pred(({ struct
_tagged_ptr0 _temp331= x; void** _temp333= _temp331.curr + i; if( _temp333 <
_temp331.base? 1: _temp333 >= _temp331.last_plus_one){ _throw( Null_Exception);}*
_temp333;}))){ return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*,
void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp334= x;( unsigned int)( _temp334.last_plus_one - _temp334.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( ! pred( env,({ struct _tagged_ptr0 _temp335= x;
void** _temp337= _temp335.curr + i; if( _temp337 < _temp335.base? 1: _temp337 >=
_temp335.last_plus_one){ _throw( Null_Exception);}* _temp337;}))){ return 0;}}}
return 1;} int Cyc_Array_exists( int(* pred)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp338= x;( unsigned int)( _temp338.last_plus_one
- _temp338.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred(({ struct
_tagged_ptr0 _temp339= x; void** _temp341= _temp339.curr + i; if( _temp341 <
_temp339.base? 1: _temp341 >= _temp339.last_plus_one){ _throw( Null_Exception);}*
_temp341;}))){ return 1;}}} return 0;} int Cyc_Array_exists_c( int(* pred)( void*,
void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp342= x;( unsigned int)( _temp342.last_plus_one - _temp342.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( pred( env,({ struct _tagged_ptr0 _temp343= x; void**
_temp345= _temp343.curr + i; if( _temp345 < _temp343.base? 1: _temp345 >=
_temp343.last_plus_one){ _throw( Null_Exception);}* _temp345;}))){ return 1;}}}
return 0;} struct _tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp346= x;( unsigned int)(
_temp346.last_plus_one - _temp346.curr);}); if(( unsigned int) sx !=({ struct
_tagged_ptr0 _temp347= y;( unsigned int)( _temp347.last_plus_one - _temp347.curr);})){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp348=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp348[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp349; _temp349.tag= Cyc_Array_Array_mismatch_tag; _temp349;}); _temp348;}));}
return({ unsigned int _temp357=( unsigned int) sx; struct _tuple0* _temp358=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0) * _temp357); unsigned int i;
struct _tagged_ptr1 _temp359={ _temp358, _temp358, _temp358 + _temp357}; for( i=
0; i < _temp357; i ++){ _temp358[ i]=({ struct _tuple0 _temp350; _temp350.f1=({
struct _tagged_ptr0 _temp354= x; void** _temp356= _temp354.curr +( int) i; if(
_temp356 < _temp354.base? 1: _temp356 >= _temp354.last_plus_one){ _throw(
Null_Exception);}* _temp356;}); _temp350.f2=({ struct _tagged_ptr0 _temp351= y;
void** _temp353= _temp351.curr +( int) i; if( _temp353 < _temp351.base? 1:
_temp353 >= _temp351.last_plus_one){ _throw( Null_Exception);}* _temp353;});
_temp350;});} _temp359;});} struct _tuple1 Cyc_Array_split( struct _tagged_ptr1
x){ int sx=( int)({ struct _tagged_ptr1 _temp360= x;( unsigned int)( _temp360.last_plus_one
- _temp360.curr);}); return({ struct _tuple1 _temp361; _temp361.f1=({
unsigned int _temp371=( unsigned int) sx; void** _temp372=( void**) GC_malloc(
sizeof( void*) * _temp371); unsigned int i; struct _tagged_ptr0 _temp373={
_temp372, _temp372, _temp372 + _temp371}; for( i= 0; i < _temp371; i ++){
_temp372[ i]=({ struct _tagged_ptr1 _temp368= x; struct _tuple0* _temp370=
_temp368.curr +( int) i; if( _temp370 < _temp368.base? 1: _temp370 >= _temp368.last_plus_one){
_throw( Null_Exception);}* _temp370;}).f1;} _temp373;}); _temp361.f2=({
unsigned int _temp365=( unsigned int) sx; void** _temp366=( void**) GC_malloc(
sizeof( void*) * _temp365); unsigned int i; struct _tagged_ptr0 _temp367={
_temp366, _temp366, _temp366 + _temp365}; for( i= 0; i < _temp365; i ++){
_temp366[ i]=({ struct _tagged_ptr1 _temp362= x; struct _tuple0* _temp364=
_temp362.curr +( int) i; if( _temp364 < _temp362.base? 1: _temp364 >= _temp362.last_plus_one){
_throw( Null_Exception);}* _temp364;}).f2;} _temp367;}); _temp361;});} int Cyc_Array_memq(
struct _tagged_ptr0 l, void* x){ int s=( int)({ struct _tagged_ptr0 _temp374= l;(
unsigned int)( _temp374.last_plus_one - _temp374.curr);});{ int i= 0; for( 0; i
< s; i ++){ if(({ struct _tagged_ptr0 _temp375= l; void** _temp377= _temp375.curr
+ i; if( _temp377 < _temp375.base? 1: _temp377 >= _temp375.last_plus_one){
_throw( Null_Exception);}* _temp377;}) == x){ return 1;}}} return 0;} int Cyc_Array_mem(
int(* compare)( void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({
struct _tagged_ptr0 _temp378= l;( unsigned int)( _temp378.last_plus_one -
_temp378.curr);});{ int i= 0; for( 0; i < s; i ++){ if( 0 == compare(({ struct
_tagged_ptr0 _temp379= l; void** _temp381= _temp379.curr + i; if( _temp381 <
_temp379.base? 1: _temp381 >= _temp379.last_plus_one){ _throw( Null_Exception);}*
_temp381;}), x)){ return 1;}}} return 0;} struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt){ int sx=( int)({ struct
_tagged_ptr0 _temp382= x;( unsigned int)( _temp382.last_plus_one - _temp382.curr);});
int n= n_opt == 0? sx - start:* n_opt; if(( start < 0? 1: n <= 0)? 1: start +(
n_opt == 0? 0: n) > sx){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp383=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp383[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp384; _temp384.tag= Cyc_Core_InvalidArg_tag;
_temp384.f1=( struct _tagged_string)({ char* _temp385=( char*)"Array::extract";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 15; _temp386;}); _temp384;}); _temp383;}));}
return({ unsigned int _temp390=( unsigned int) n; void** _temp391=( void**)
GC_malloc( sizeof( void*) * _temp390); unsigned int i; struct _tagged_ptr0
_temp392={ _temp391, _temp391, _temp391 + _temp390}; for( i= 0; i < _temp390; i
++){ _temp391[ i]=({ struct _tagged_ptr0 _temp387= x; void** _temp389= _temp387.curr
+( int)(( unsigned int) start + i); if( _temp389 < _temp387.base? 1: _temp389 >=
_temp387.last_plus_one){ _throw( Null_Exception);}* _temp389;});} _temp392;});}