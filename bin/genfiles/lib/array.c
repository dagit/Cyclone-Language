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
int i; int j; int limit_ofs; if(( base_ofs < 0? 1: base_ofs + len >({ struct
_tagged_ptr0 _temp0= arr;( unsigned int)( _temp0.last_plus_one - _temp0.curr);}))?
1: len < 0){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp1=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp1[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp2; _temp2.tag= Cyc_Core_InvalidArg_tag;
_temp2.f1=( struct _tagged_string)({ char* _temp3=( char*)"Array::qsort"; struct
_tagged_string _temp4; _temp4.curr= _temp3; _temp4.base= _temp3; _temp4.last_plus_one=
_temp3 + 13; _temp4;}); _temp2;}); _temp1;}));} limit_ofs= base_ofs + len;
sp_ofs= 0; for( 0; 1; 0){ if( limit_ofs - base_ofs > 3){ temp=({ struct
_tagged_ptr0 _temp5= arr; void** _temp7= _temp5.curr +(( limit_ofs - base_ofs) /
2 + base_ofs); if( _temp5.base == 0? 1:( _temp7 < _temp5.base? 1: _temp7 >=
_temp5.last_plus_one)){ _throw( Null_Exception);}* _temp7;});({ struct
_tagged_ptr0 _temp8= arr; void** _temp10= _temp8.curr +(( limit_ofs - base_ofs)
/ 2 + base_ofs); if( _temp8.base == 0? 1:( _temp10 < _temp8.base? 1: _temp10 >=
_temp8.last_plus_one)){ _throw( Null_Exception);}* _temp10=({ struct
_tagged_ptr0 _temp11= arr; void** _temp13= _temp11.curr + base_ofs; if( _temp11.base
== 0? 1:( _temp13 < _temp11.base? 1: _temp13 >= _temp11.last_plus_one)){ _throw(
Null_Exception);}* _temp13;});});({ struct _tagged_ptr0 _temp14= arr; void**
_temp16= _temp14.curr + base_ofs; if( _temp14.base == 0? 1:( _temp16 < _temp14.base?
1: _temp16 >= _temp14.last_plus_one)){ _throw( Null_Exception);}* _temp16= temp;});
i= base_ofs + 1; j= limit_ofs - 1; if( less_eq(({ struct _tagged_ptr0 _temp17=
arr; void** _temp19= _temp17.curr + i; if( _temp17.base == 0? 1:( _temp19 <
_temp17.base? 1: _temp19 >= _temp17.last_plus_one)){ _throw( Null_Exception);}*
_temp19;}),({ struct _tagged_ptr0 _temp20= arr; void** _temp22= _temp20.curr + j;
if( _temp20.base == 0? 1:( _temp22 < _temp20.base? 1: _temp22 >= _temp20.last_plus_one)){
_throw( Null_Exception);}* _temp22;})) > 0){ temp=({ struct _tagged_ptr0 _temp23=
arr; void** _temp25= _temp23.curr + i; if( _temp23.base == 0? 1:( _temp25 <
_temp23.base? 1: _temp25 >= _temp23.last_plus_one)){ _throw( Null_Exception);}*
_temp25;});({ struct _tagged_ptr0 _temp26= arr; void** _temp28= _temp26.curr + i;
if( _temp26.base == 0? 1:( _temp28 < _temp26.base? 1: _temp28 >= _temp26.last_plus_one)){
_throw( Null_Exception);}* _temp28=({ struct _tagged_ptr0 _temp29= arr; void**
_temp31= _temp29.curr + j; if( _temp29.base == 0? 1:( _temp31 < _temp29.base? 1:
_temp31 >= _temp29.last_plus_one)){ _throw( Null_Exception);}* _temp31;});});({
struct _tagged_ptr0 _temp32= arr; void** _temp34= _temp32.curr + j; if( _temp32.base
== 0? 1:( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one)){ _throw(
Null_Exception);}* _temp34= temp;});} if( less_eq(({ struct _tagged_ptr0 _temp35=
arr; void** _temp37= _temp35.curr + base_ofs; if( _temp35.base == 0? 1:( _temp37
< _temp35.base? 1: _temp37 >= _temp35.last_plus_one)){ _throw( Null_Exception);}*
_temp37;}),({ struct _tagged_ptr0 _temp38= arr; void** _temp40= _temp38.curr + j;
if( _temp38.base == 0? 1:( _temp40 < _temp38.base? 1: _temp40 >= _temp38.last_plus_one)){
_throw( Null_Exception);}* _temp40;})) > 0){ temp=({ struct _tagged_ptr0 _temp41=
arr; void** _temp43= _temp41.curr + base_ofs; if( _temp41.base == 0? 1:( _temp43
< _temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw( Null_Exception);}*
_temp43;});({ struct _tagged_ptr0 _temp44= arr; void** _temp46= _temp44.curr +
base_ofs; if( _temp44.base == 0? 1:( _temp46 < _temp44.base? 1: _temp46 >=
_temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46=({ struct
_tagged_ptr0 _temp47= arr; void** _temp49= _temp47.curr + j; if( _temp47.base ==
0? 1:( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){ _throw(
Null_Exception);}* _temp49;});});({ struct _tagged_ptr0 _temp50= arr; void**
_temp52= _temp50.curr + j; if( _temp50.base == 0? 1:( _temp52 < _temp50.base? 1:
_temp52 >= _temp50.last_plus_one)){ _throw( Null_Exception);}* _temp52= temp;});}
if( less_eq(({ struct _tagged_ptr0 _temp53= arr; void** _temp55= _temp53.curr +
i; if( _temp53.base == 0? 1:( _temp55 < _temp53.base? 1: _temp55 >= _temp53.last_plus_one)){
_throw( Null_Exception);}* _temp55;}),({ struct _tagged_ptr0 _temp56= arr; void**
_temp58= _temp56.curr + base_ofs; if( _temp56.base == 0? 1:( _temp58 < _temp56.base?
1: _temp58 >= _temp56.last_plus_one)){ _throw( Null_Exception);}* _temp58;})) >
0){ temp=({ struct _tagged_ptr0 _temp59= arr; void** _temp61= _temp59.curr + i;
if( _temp59.base == 0? 1:( _temp61 < _temp59.base? 1: _temp61 >= _temp59.last_plus_one)){
_throw( Null_Exception);}* _temp61;});({ struct _tagged_ptr0 _temp62= arr; void**
_temp64= _temp62.curr + i; if( _temp62.base == 0? 1:( _temp64 < _temp62.base? 1:
_temp64 >= _temp62.last_plus_one)){ _throw( Null_Exception);}* _temp64=({ struct
_tagged_ptr0 _temp65= arr; void** _temp67= _temp65.curr + base_ofs; if( _temp65.base
== 0? 1:( _temp67 < _temp65.base? 1: _temp67 >= _temp65.last_plus_one)){ _throw(
Null_Exception);}* _temp67;});});({ struct _tagged_ptr0 _temp68= arr; void**
_temp70= _temp68.curr + base_ofs; if( _temp68.base == 0? 1:( _temp70 < _temp68.base?
1: _temp70 >= _temp68.last_plus_one)){ _throw( Null_Exception);}* _temp70= temp;});}
for( 0; 1; 0){ do { i ++;} while ( less_eq(({ struct _tagged_ptr0 _temp71= arr;
void** _temp73= _temp71.curr + i; if( _temp71.base == 0? 1:( _temp73 < _temp71.base?
1: _temp73 >= _temp71.last_plus_one)){ _throw( Null_Exception);}* _temp73;}),({
struct _tagged_ptr0 _temp74= arr; void** _temp76= _temp74.curr + base_ofs; if(
_temp74.base == 0? 1:( _temp76 < _temp74.base? 1: _temp76 >= _temp74.last_plus_one)){
_throw( Null_Exception);}* _temp76;})) < 0); do { j --;} while ( less_eq(({
struct _tagged_ptr0 _temp77= arr; void** _temp79= _temp77.curr + j; if( _temp77.base
== 0? 1:( _temp79 < _temp77.base? 1: _temp79 >= _temp77.last_plus_one)){ _throw(
Null_Exception);}* _temp79;}),({ struct _tagged_ptr0 _temp80= arr; void**
_temp82= _temp80.curr + base_ofs; if( _temp80.base == 0? 1:( _temp82 < _temp80.base?
1: _temp82 >= _temp80.last_plus_one)){ _throw( Null_Exception);}* _temp82;})) >
0); if( i > j){ break;} temp=({ struct _tagged_ptr0 _temp83= arr; void** _temp85=
_temp83.curr + i; if( _temp83.base == 0? 1:( _temp85 < _temp83.base? 1: _temp85
>= _temp83.last_plus_one)){ _throw( Null_Exception);}* _temp85;});({ struct
_tagged_ptr0 _temp86= arr; void** _temp88= _temp86.curr + i; if( _temp86.base ==
0? 1:( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one)){ _throw(
Null_Exception);}* _temp88=({ struct _tagged_ptr0 _temp89= arr; void** _temp91=
_temp89.curr + j; if( _temp89.base == 0? 1:( _temp91 < _temp89.base? 1: _temp91
>= _temp89.last_plus_one)){ _throw( Null_Exception);}* _temp91;});});({ struct
_tagged_ptr0 _temp92= arr; void** _temp94= _temp92.curr + j; if( _temp92.base ==
0? 1:( _temp94 < _temp92.base? 1: _temp94 >= _temp92.last_plus_one)){ _throw(
Null_Exception);}* _temp94= temp;});;} temp=({ struct _tagged_ptr0 _temp95= arr;
void** _temp97= _temp95.curr + base_ofs; if( _temp95.base == 0? 1:( _temp97 <
_temp95.base? 1: _temp97 >= _temp95.last_plus_one)){ _throw( Null_Exception);}*
_temp97;});({ struct _tagged_ptr0 _temp98= arr; void** _temp100= _temp98.curr +
base_ofs; if( _temp98.base == 0? 1:( _temp100 < _temp98.base? 1: _temp100 >=
_temp98.last_plus_one)){ _throw( Null_Exception);}* _temp100=({ struct
_tagged_ptr0 _temp101= arr; void** _temp103= _temp101.curr + j; if( _temp101.base
== 0? 1:( _temp103 < _temp101.base? 1: _temp103 >= _temp101.last_plus_one)){
_throw( Null_Exception);}* _temp103;});});({ struct _tagged_ptr0 _temp104= arr;
void** _temp106= _temp104.curr + j; if( _temp104.base == 0? 1:( _temp106 <
_temp104.base? 1: _temp106 >= _temp104.last_plus_one)){ _throw( Null_Exception);}*
_temp106= temp;}); if( j - base_ofs > limit_ofs - i){({ int* _temp107=( int*)
Cyc_Array_sp; unsigned int _temp108= sp_ofs; if( _temp108 >= 40u){ _throw(
Null_Exception);} _temp107[ _temp108]= base_ofs;});({ int* _temp109=( int*) Cyc_Array_sp;
unsigned int _temp110= sp_ofs + 1; if( _temp110 >= 40u){ _throw( Null_Exception);}
_temp109[ _temp110]= j;}); base_ofs= i;} else{({ int* _temp111=( int*) Cyc_Array_sp;
unsigned int _temp112= sp_ofs; if( _temp112 >= 40u){ _throw( Null_Exception);}
_temp111[ _temp112]= i;});({ int* _temp113=( int*) Cyc_Array_sp; unsigned int
_temp114= sp_ofs + 1; if( _temp114 >= 40u){ _throw( Null_Exception);} _temp113[
_temp114]= limit_ofs;}); limit_ofs= j;} sp_ofs += 2;} else{ for( j= base_ofs, i=
j + 1; i < limit_ofs; j= i, i ++){ for( 0; less_eq(({ struct _tagged_ptr0
_temp115= arr; void** _temp117= _temp115.curr + j; if( _temp115.base == 0? 1:(
_temp117 < _temp115.base? 1: _temp117 >= _temp115.last_plus_one)){ _throw(
Null_Exception);}* _temp117;}),({ struct _tagged_ptr0 _temp118= arr; void**
_temp120= _temp118.curr +( j + 1); if( _temp118.base == 0? 1:( _temp120 <
_temp118.base? 1: _temp120 >= _temp118.last_plus_one)){ _throw( Null_Exception);}*
_temp120;})) > 0; j --){ temp=({ struct _tagged_ptr0 _temp121= arr; void**
_temp123= _temp121.curr + j; if( _temp121.base == 0? 1:( _temp123 < _temp121.base?
1: _temp123 >= _temp121.last_plus_one)){ _throw( Null_Exception);}* _temp123;});({
struct _tagged_ptr0 _temp124= arr; void** _temp126= _temp124.curr + j; if(
_temp124.base == 0? 1:( _temp126 < _temp124.base? 1: _temp126 >= _temp124.last_plus_one)){
_throw( Null_Exception);}* _temp126=({ struct _tagged_ptr0 _temp127= arr; void**
_temp129= _temp127.curr +( j + 1); if( _temp127.base == 0? 1:( _temp129 <
_temp127.base? 1: _temp129 >= _temp127.last_plus_one)){ _throw( Null_Exception);}*
_temp129;});});({ struct _tagged_ptr0 _temp130= arr; void** _temp132= _temp130.curr
+( j + 1); if( _temp130.base == 0? 1:( _temp132 < _temp130.base? 1: _temp132 >=
_temp130.last_plus_one)){ _throw( Null_Exception);}* _temp132= temp;}); if( j ==
base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs=({ int* _temp133=(
int*) Cyc_Array_sp; unsigned int _temp134= sp_ofs; if( _temp134 >= 40u){ _throw(
Null_Exception);} _temp133[ _temp134];}); limit_ofs=({ int* _temp135=( int*) Cyc_Array_sp;
unsigned int _temp136= sp_ofs + 1; if( _temp136 >= 40u){ _throw( Null_Exception);}
_temp135[ _temp136];});} else{ break;}}}} void Cyc_Array_msort( int(* less_eq)(
void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){ if(( base_ofs <
0? 1: base_ofs + len >({ struct _tagged_ptr0 _temp137= arr;( unsigned int)(
_temp137.last_plus_one - _temp137.curr);}))? 1: len < 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp138=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp138[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp139; _temp139.tag= Cyc_Core_InvalidArg_tag;
_temp139.f1=( struct _tagged_string)({ char* _temp140=( char*)"Array::msort";
struct _tagged_string _temp141; _temp141.curr= _temp140; _temp141.base= _temp140;
_temp141.last_plus_one= _temp140 + 13; _temp141;}); _temp139;}); _temp138;}));}{
int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({ unsigned int _temp142=(
unsigned int) len; void** _temp143=( void**) GC_malloc( sizeof( void*) *
_temp142); struct _tagged_ptr0 _temp149={ _temp143, _temp143, _temp143 +
_temp142};{ unsigned int _temp144= _temp142; unsigned int i; for( i= 0; i <
_temp144; i ++){ _temp143[ i]=({ struct _tagged_ptr0 _temp146= arr; void**
_temp148= _temp146.curr + 0; if( _temp146.base == 0? 1:( _temp148 < _temp146.base?
1: _temp148 >= _temp146.last_plus_one)){ _throw( Null_Exception);}* _temp148;});}};
_temp149;}); struct _tagged_ptr0 to=({ unsigned int _temp150=( unsigned int) len;
void** _temp151=( void**) GC_malloc( sizeof( void*) * _temp150); struct
_tagged_ptr0 _temp157={ _temp151, _temp151, _temp151 + _temp150};{ unsigned int
_temp152= _temp150; unsigned int i; for( i= 0; i < _temp152; i ++){ _temp151[ i]=({
struct _tagged_ptr0 _temp154= arr; void** _temp156= _temp154.curr +( int)( i +
base_ofs); if( _temp154.base == 0? 1:( _temp156 < _temp154.base? 1: _temp156 >=
_temp154.last_plus_one)){ _throw( Null_Exception);}* _temp156;});}}; _temp157;});
struct _tagged_ptr0 swap; int stepsize; int start; int lstart; int lend; int
rstart; int rend; int dest; for( stepsize= 1; stepsize < len; stepsize= stepsize
* 2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start < limit_ofs;
start= start + stepsize * 2){ lstart= start; rstart= start + stepsize <
limit_ofs? start + stepsize: limit_ofs; lend= rstart; rend= start + stepsize * 2
< limit_ofs? start + stepsize * 2: limit_ofs; while( lstart < lend? rstart <
rend: 0) { if( less_eq(({ struct _tagged_ptr0 _temp158= from; void** _temp160=
_temp158.curr + lstart; if( _temp158.base == 0? 1:( _temp160 < _temp158.base? 1:
_temp160 >= _temp158.last_plus_one)){ _throw( Null_Exception);}* _temp160;}),({
struct _tagged_ptr0 _temp161= from; void** _temp163= _temp161.curr + rstart; if(
_temp161.base == 0? 1:( _temp163 < _temp161.base? 1: _temp163 >= _temp161.last_plus_one)){
_throw( Null_Exception);}* _temp163;})) <= 0){({ struct _tagged_ptr0 _temp164=
to; void** _temp166= _temp164.curr +( dest ++); if( _temp164.base == 0? 1:(
_temp166 < _temp164.base? 1: _temp166 >= _temp164.last_plus_one)){ _throw(
Null_Exception);}* _temp166=({ struct _tagged_ptr0 _temp167= from; void**
_temp169= _temp167.curr +( lstart ++); if( _temp167.base == 0? 1:( _temp169 <
_temp167.base? 1: _temp169 >= _temp167.last_plus_one)){ _throw( Null_Exception);}*
_temp169;});});} else{({ struct _tagged_ptr0 _temp170= to; void** _temp172=
_temp170.curr +( dest ++); if( _temp170.base == 0? 1:( _temp172 < _temp170.base?
1: _temp172 >= _temp170.last_plus_one)){ _throw( Null_Exception);}* _temp172=({
struct _tagged_ptr0 _temp173= from; void** _temp175= _temp173.curr +( rstart ++);
if( _temp173.base == 0? 1:( _temp175 < _temp173.base? 1: _temp175 >= _temp173.last_plus_one)){
_throw( Null_Exception);}* _temp175;});});}} while( lstart < lend) {({ struct
_tagged_ptr0 _temp176= to; void** _temp178= _temp176.curr +( dest ++); if(
_temp176.base == 0? 1:( _temp178 < _temp176.base? 1: _temp178 >= _temp176.last_plus_one)){
_throw( Null_Exception);}* _temp178=({ struct _tagged_ptr0 _temp179= from; void**
_temp181= _temp179.curr +( lstart ++); if( _temp179.base == 0? 1:( _temp181 <
_temp179.base? 1: _temp181 >= _temp179.last_plus_one)){ _throw( Null_Exception);}*
_temp181;});});} while( rstart < rend) {({ struct _tagged_ptr0 _temp182= to;
void** _temp184= _temp182.curr +( dest ++); if( _temp182.base == 0? 1:( _temp184
< _temp182.base? 1: _temp184 >= _temp182.last_plus_one)){ _throw( Null_Exception);}*
_temp184=({ struct _tagged_ptr0 _temp185= from; void** _temp187= _temp185.curr +(
rstart ++); if( _temp185.base == 0? 1:( _temp187 < _temp185.base? 1: _temp187 >=
_temp185.last_plus_one)){ _throw( Null_Exception);}* _temp187;});});}}}{ int i=
0; for( 0; i < len; i ++){({ struct _tagged_ptr0 _temp188= arr; void** _temp190=
_temp188.curr +( i + base_ofs); if( _temp188.base == 0? 1:( _temp190 < _temp188.base?
1: _temp190 >= _temp188.last_plus_one)){ _throw( Null_Exception);}* _temp190=({
struct _tagged_ptr0 _temp191= to; void** _temp193= _temp191.curr + i; if(
_temp191.base == 0? 1:( _temp193 < _temp191.base? 1: _temp193 >= _temp191.last_plus_one)){
_throw( Null_Exception);}* _temp193;});});}}}} struct _tagged_ptr0 Cyc_Array_from_list(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List*
x)) Cyc_List_to_array)( x);} struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_ptr0 x){ return(( struct Cyc_List_List*(*)( struct _tagged_ptr0 arr))
Cyc_List_from_array)( x);} struct _tagged_ptr0 Cyc_Array_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp194= x;( unsigned int)(
_temp194.last_plus_one - _temp194.curr);}); return({ unsigned int _temp195=(
unsigned int) sx; void** _temp196=( void**) GC_malloc( sizeof( void*) * _temp195);
struct _tagged_ptr0 _temp202={ _temp196, _temp196, _temp196 + _temp195};{
unsigned int _temp197= _temp195; unsigned int i; for( i= 0; i < _temp197; i ++){
_temp196[ i]=({ struct _tagged_ptr0 _temp199= x; void** _temp201= _temp199.curr
+( int) i; if( _temp199.base == 0? 1:( _temp201 < _temp199.base? 1: _temp201 >=
_temp199.last_plus_one)){ _throw( Null_Exception);}* _temp201;});}}; _temp202;});}
struct _tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp203= x;( unsigned int)( _temp203.last_plus_one
- _temp203.curr);}); return({ unsigned int _temp204=( unsigned int) sx; void**
_temp205=( void**) GC_malloc( sizeof( void*) * _temp204); struct _tagged_ptr0
_temp211={ _temp205, _temp205, _temp205 + _temp204};{ unsigned int _temp206=
_temp204; unsigned int i; for( i= 0; i < _temp206; i ++){ _temp205[ i]= f(({
struct _tagged_ptr0 _temp208= x; void** _temp210= _temp208.curr +( int) i; if(
_temp208.base == 0? 1:( _temp210 < _temp208.base? 1: _temp210 >= _temp208.last_plus_one)){
_throw( Null_Exception);}* _temp210;}));}}; _temp211;});} struct _tagged_ptr0
Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp212= x;( unsigned int)( _temp212.last_plus_one
- _temp212.curr);}); return({ unsigned int _temp213=( unsigned int) sx; void**
_temp214=( void**) GC_malloc( sizeof( void*) * _temp213); struct _tagged_ptr0
_temp220={ _temp214, _temp214, _temp214 + _temp213};{ unsigned int _temp215=
_temp213; unsigned int i; for( i= 0; i < _temp215; i ++){ _temp214[ i]= f( env,({
struct _tagged_ptr0 _temp217= x; void** _temp219= _temp217.curr +( int) i; if(
_temp217.base == 0? 1:( _temp219 < _temp217.base? 1: _temp219 >= _temp217.last_plus_one)){
_throw( Null_Exception);}* _temp219;}));}}; _temp220;});} void Cyc_Array_imp_map(
void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp221= x;( unsigned int)( _temp221.last_plus_one - _temp221.curr);}); int i=
0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp222= x; void** _temp224=
_temp222.curr + i; if( _temp222.base == 0? 1:( _temp224 < _temp222.base? 1:
_temp224 >= _temp222.last_plus_one)){ _throw( Null_Exception);}* _temp224= f(({
struct _tagged_ptr0 _temp225= x; void** _temp227= _temp225.curr + i; if(
_temp225.base == 0? 1:( _temp227 < _temp225.base? 1: _temp227 >= _temp225.last_plus_one)){
_throw( Null_Exception);}* _temp227;}));});}} void Cyc_Array_imp_map_c( void*(*
f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp228= x;( unsigned int)( _temp228.last_plus_one - _temp228.curr);});
int i= 0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp229= x; void**
_temp231= _temp229.curr + i; if( _temp229.base == 0? 1:( _temp231 < _temp229.base?
1: _temp231 >= _temp229.last_plus_one)){ _throw( Null_Exception);}* _temp231= f(
env,({ struct _tagged_ptr0 _temp232= x; void** _temp234= _temp232.curr + i; if(
_temp232.base == 0? 1:( _temp234 < _temp232.base? 1: _temp234 >= _temp232.last_plus_one)){
_throw( Null_Exception);}* _temp234;}));});}} char Cyc_Array_Array_mismatch_tag[
15u]="Array_mismatch"; struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*,
void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp235= x;( unsigned int)( _temp235.last_plus_one - _temp235.curr);});
if( sx !=({ struct _tagged_ptr0 _temp236= y;( unsigned int)( _temp236.last_plus_one
- _temp236.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp237=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp237[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp238; _temp238.tag= Cyc_Array_Array_mismatch_tag; _temp238;}); _temp237;}));}
return({ unsigned int _temp239=( unsigned int) sx; void** _temp240=( void**)
GC_malloc( sizeof( void*) * _temp239); struct _tagged_ptr0 _temp249={ _temp240,
_temp240, _temp240 + _temp239};{ unsigned int _temp241= _temp239; unsigned int i;
for( i= 0; i < _temp241; i ++){ _temp240[ i]= f(({ struct _tagged_ptr0 _temp243=
x; void** _temp245= _temp243.curr +( int) i; if( _temp243.base == 0? 1:(
_temp245 < _temp243.base? 1: _temp245 >= _temp243.last_plus_one)){ _throw(
Null_Exception);}* _temp245;}),({ struct _tagged_ptr0 _temp246= y; void**
_temp248= _temp246.curr +( int) i; if( _temp246.base == 0? 1:( _temp248 <
_temp246.base? 1: _temp248 >= _temp246.last_plus_one)){ _throw( Null_Exception);}*
_temp248;}));}}; _temp249;});} void Cyc_Array_app( void*(* f)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp250= x;( unsigned int)(
_temp250.last_plus_one - _temp250.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({
struct _tagged_ptr0 _temp251= x; void** _temp253= _temp251.curr + i; if(
_temp251.base == 0? 1:( _temp253 < _temp251.base? 1: _temp253 >= _temp251.last_plus_one)){
_throw( Null_Exception);}* _temp253;}));}} void Cyc_Array_app_c( void*(* f)(
void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp254= x;( unsigned int)( _temp254.last_plus_one - _temp254.curr);});
int i= 0; for( 0; i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp255= x; void**
_temp257= _temp255.curr + i; if( _temp255.base == 0? 1:( _temp257 < _temp255.base?
1: _temp257 >= _temp255.last_plus_one)){ _throw( Null_Exception);}* _temp257;}));}}
void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp258= x;( unsigned int)( _temp258.last_plus_one -
_temp258.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp259= x; void** _temp261= _temp259.curr + i; if( _temp259.base == 0? 1:(
_temp261 < _temp259.base? 1: _temp261 >= _temp259.last_plus_one)){ _throw(
Null_Exception);}* _temp261;}));}} void Cyc_Array_iter_c( void(* f)( void*, void*),
void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp262=
x;( unsigned int)( _temp262.last_plus_one - _temp262.curr);}); int i= 0; for( 0;
i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp263= x; void** _temp265=
_temp263.curr + i; if( _temp263.base == 0? 1:( _temp265 < _temp263.base? 1:
_temp265 >= _temp263.last_plus_one)){ _throw( Null_Exception);}* _temp265;}));}}
void Cyc_Array_app2( void*(* f)( void*, void*), struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp266= x;( unsigned int)(
_temp266.last_plus_one - _temp266.curr);}); if( sx !=({ struct _tagged_ptr0
_temp267= y;( unsigned int)( _temp267.last_plus_one - _temp267.curr);})){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp268=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp268[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp269; _temp269.tag= Cyc_Array_Array_mismatch_tag; _temp269;}); _temp268;}));}{
int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0 _temp270= x; void**
_temp272= _temp270.curr + i; if( _temp270.base == 0? 1:( _temp272 < _temp270.base?
1: _temp272 >= _temp270.last_plus_one)){ _throw( Null_Exception);}* _temp272;}),({
struct _tagged_ptr0 _temp273= y; void** _temp275= _temp273.curr + i; if(
_temp273.base == 0? 1:( _temp275 < _temp273.base? 1: _temp275 >= _temp273.last_plus_one)){
_throw( Null_Exception);}* _temp275;}));}}} void Cyc_Array_app2_c( void*(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){
int sx=( int)({ struct _tagged_ptr0 _temp276= x;( unsigned int)( _temp276.last_plus_one
- _temp276.curr);}); if( sx !=({ struct _tagged_ptr0 _temp277= y;( unsigned int)(
_temp277.last_plus_one - _temp277.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp278=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp278[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp279; _temp279.tag=
Cyc_Array_Array_mismatch_tag; _temp279;}); _temp278;}));}{ int i= 0; for( 0; i <
sx; i ++){ f( env,({ struct _tagged_ptr0 _temp280= x; void** _temp282= _temp280.curr
+ i; if( _temp280.base == 0? 1:( _temp282 < _temp280.base? 1: _temp282 >=
_temp280.last_plus_one)){ _throw( Null_Exception);}* _temp282;}),({ struct
_tagged_ptr0 _temp283= y; void** _temp285= _temp283.curr + i; if( _temp283.base
== 0? 1:( _temp285 < _temp283.base? 1: _temp285 >= _temp283.last_plus_one)){
_throw( Null_Exception);}* _temp285;}));}}} void Cyc_Array_iter2( void(* f)(
void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({
struct _tagged_ptr0 _temp286= x;( unsigned int)( _temp286.last_plus_one -
_temp286.curr);}); if( sx !=({ struct _tagged_ptr0 _temp287= y;( unsigned int)(
_temp287.last_plus_one - _temp287.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp288=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp288[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp289; _temp289.tag=
Cyc_Array_Array_mismatch_tag; _temp289;}); _temp288;}));}{ int i= 0; for( 0; i <
sx; i ++){ f(({ struct _tagged_ptr0 _temp290= x; void** _temp292= _temp290.curr
+ i; if( _temp290.base == 0? 1:( _temp292 < _temp290.base? 1: _temp292 >=
_temp290.last_plus_one)){ _throw( Null_Exception);}* _temp292;}),({ struct
_tagged_ptr0 _temp293= y; void** _temp295= _temp293.curr + i; if( _temp293.base
== 0? 1:( _temp295 < _temp293.base? 1: _temp295 >= _temp293.last_plus_one)){
_throw( Null_Exception);}* _temp295;}));}}} void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){
int sx=( int)({ struct _tagged_ptr0 _temp296= x;( unsigned int)( _temp296.last_plus_one
- _temp296.curr);}); if( sx !=({ struct _tagged_ptr0 _temp297= y;( unsigned int)(
_temp297.last_plus_one - _temp297.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp298=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp298[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp299; _temp299.tag=
Cyc_Array_Array_mismatch_tag; _temp299;}); _temp298;}));}{ int i= 0; for( 0; i <
sx; i ++){ f( env,({ struct _tagged_ptr0 _temp300= x; void** _temp302= _temp300.curr
+ i; if( _temp300.base == 0? 1:( _temp302 < _temp300.base? 1: _temp302 >=
_temp300.last_plus_one)){ _throw( Null_Exception);}* _temp302;}),({ struct
_tagged_ptr0 _temp303= y; void** _temp305= _temp303.curr + i; if( _temp303.base
== 0? 1:( _temp305 < _temp303.base? 1: _temp305 >= _temp303.last_plus_one)){
_throw( Null_Exception);}* _temp305;}));}}} void* Cyc_Array_fold_left( void*(* f)(
void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp306= x;( unsigned int)( _temp306.last_plus_one - _temp306.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( accum,({ struct _tagged_ptr0 _temp307=
x; void** _temp309= _temp307.curr + i; if( _temp307.base == 0? 1:( _temp309 <
_temp307.base? 1: _temp309 >= _temp307.last_plus_one)){ _throw( Null_Exception);}*
_temp309;}));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp310= x;( unsigned int)( _temp310.last_plus_one -
_temp310.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,({
struct _tagged_ptr0 _temp311= x; void** _temp313= _temp311.curr + i; if(
_temp311.base == 0? 1:( _temp313 < _temp311.base? 1: _temp313 >= _temp311.last_plus_one)){
_throw( Null_Exception);}* _temp313;}));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({
struct _tagged_ptr0 _temp314= x;( unsigned int)( _temp314.last_plus_one -
_temp314.curr);}) - 1); for( 0; i >= 0; i --){ accum= f(({ struct _tagged_ptr0
_temp315= x; void** _temp317= _temp315.curr + i; if( _temp315.base == 0? 1:(
_temp317 < _temp315.base? 1: _temp317 >= _temp315.last_plus_one)){ _throw(
Null_Exception);}* _temp317;}), accum);}} return accum;} void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum){{
int i=( int)(({ struct _tagged_ptr0 _temp318= x;( unsigned int)( _temp318.last_plus_one
- _temp318.curr);}) - 1); for( 0; i >= 0; i --){ accum= f( env,({ struct
_tagged_ptr0 _temp319= x; void** _temp321= _temp319.curr + i; if( _temp319.base
== 0? 1:( _temp321 < _temp319.base? 1: _temp321 >= _temp319.last_plus_one)){
_throw( Null_Exception);}* _temp321;}), accum);}} return accum;} struct
_tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp322= x;( unsigned int)( _temp322.last_plus_one - _temp322.curr);});
int n= sx - 1; return({ unsigned int _temp323=( unsigned int) sx; void**
_temp324=( void**) GC_malloc( sizeof( void*) * _temp323); struct _tagged_ptr0
_temp330={ _temp324, _temp324, _temp324 + _temp323};{ unsigned int _temp325=
_temp323; unsigned int i; for( i= 0; i < _temp325; i ++){ _temp324[ i]=({ struct
_tagged_ptr0 _temp327= x; void** _temp329= _temp327.curr +( int)( n - i); if(
_temp327.base == 0? 1:( _temp329 < _temp327.base? 1: _temp329 >= _temp327.last_plus_one)){
_throw( Null_Exception);}* _temp329;});}}; _temp330;});} void Cyc_Array_imp_rev(
struct _tagged_ptr0 x){ void* temp; int i= 0; int j=( int)(({ struct
_tagged_ptr0 _temp331= x;( unsigned int)( _temp331.last_plus_one - _temp331.curr);})
- 1); while( i < j) { temp=({ struct _tagged_ptr0 _temp332= x; void** _temp334=
_temp332.curr + i; if( _temp332.base == 0? 1:( _temp334 < _temp332.base? 1:
_temp334 >= _temp332.last_plus_one)){ _throw( Null_Exception);}* _temp334;});({
struct _tagged_ptr0 _temp335= x; void** _temp337= _temp335.curr + i; if(
_temp335.base == 0? 1:( _temp337 < _temp335.base? 1: _temp337 >= _temp335.last_plus_one)){
_throw( Null_Exception);}* _temp337=({ struct _tagged_ptr0 _temp338= x; void**
_temp340= _temp338.curr + j; if( _temp338.base == 0? 1:( _temp340 < _temp338.base?
1: _temp340 >= _temp338.last_plus_one)){ _throw( Null_Exception);}* _temp340;});});({
struct _tagged_ptr0 _temp341= x; void** _temp343= _temp341.curr + j; if(
_temp341.base == 0? 1:( _temp343 < _temp341.base? 1: _temp343 >= _temp341.last_plus_one)){
_throw( Null_Exception);}* _temp343= temp;}); i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp344= x;( unsigned int)( _temp344.last_plus_one - _temp344.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( ! pred(({ struct _tagged_ptr0 _temp345= x; void**
_temp347= _temp345.curr + i; if( _temp345.base == 0? 1:( _temp347 < _temp345.base?
1: _temp347 >= _temp345.last_plus_one)){ _throw( Null_Exception);}* _temp347;}))){
return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*, void*), void*
env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp348= x;(
unsigned int)( _temp348.last_plus_one - _temp348.curr);});{ int i= 0; for( 0; i
< sx; i ++){ if( ! pred( env,({ struct _tagged_ptr0 _temp349= x; void** _temp351=
_temp349.curr + i; if( _temp349.base == 0? 1:( _temp351 < _temp349.base? 1:
_temp351 >= _temp349.last_plus_one)){ _throw( Null_Exception);}* _temp351;}))){
return 0;}}} return 1;} int Cyc_Array_exists( int(* pred)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp352= x;( unsigned int)(
_temp352.last_plus_one - _temp352.curr);});{ int i= 0; for( 0; i < sx; i ++){
if( pred(({ struct _tagged_ptr0 _temp353= x; void** _temp355= _temp353.curr + i;
if( _temp353.base == 0? 1:( _temp355 < _temp353.base? 1: _temp355 >= _temp353.last_plus_one)){
_throw( Null_Exception);}* _temp355;}))){ return 1;}}} return 0;} int Cyc_Array_exists_c(
int(* pred)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp356= x;( unsigned int)( _temp356.last_plus_one -
_temp356.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred( env,({ struct
_tagged_ptr0 _temp357= x; void** _temp359= _temp357.curr + i; if( _temp357.base
== 0? 1:( _temp359 < _temp357.base? 1: _temp359 >= _temp357.last_plus_one)){
_throw( Null_Exception);}* _temp359;}))){ return 1;}}} return 0;} struct
_tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int
sx=( int)({ struct _tagged_ptr0 _temp360= x;( unsigned int)( _temp360.last_plus_one
- _temp360.curr);}); if( sx !=({ struct _tagged_ptr0 _temp361= y;( unsigned int)(
_temp361.last_plus_one - _temp361.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp362=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp362[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp363; _temp363.tag=
Cyc_Array_Array_mismatch_tag; _temp363;}); _temp362;}));} return({ unsigned int
_temp364=( unsigned int) sx; struct _tuple0* _temp365=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0) * _temp364); struct _tagged_ptr1 _temp375={
_temp365, _temp365, _temp365 + _temp364};{ unsigned int _temp366= _temp364;
unsigned int i; for( i= 0; i < _temp366; i ++){ _temp365[ i]=({ struct _tuple0
_temp368; _temp368.f1=({ struct _tagged_ptr0 _temp372= x; void** _temp374=
_temp372.curr +( int) i; if( _temp372.base == 0? 1:( _temp374 < _temp372.base? 1:
_temp374 >= _temp372.last_plus_one)){ _throw( Null_Exception);}* _temp374;});
_temp368.f2=({ struct _tagged_ptr0 _temp369= y; void** _temp371= _temp369.curr +(
int) i; if( _temp369.base == 0? 1:( _temp371 < _temp369.base? 1: _temp371 >=
_temp369.last_plus_one)){ _throw( Null_Exception);}* _temp371;}); _temp368;});}};
_temp375;});} struct _tuple1 Cyc_Array_split( struct _tagged_ptr1 x){ int sx=(
int)({ struct _tagged_ptr1 _temp376= x;( unsigned int)( _temp376.last_plus_one -
_temp376.curr);}); return({ struct _tuple1 _temp377; _temp377.f1=({ unsigned int
_temp386=( unsigned int) sx; void** _temp387=( void**) GC_malloc( sizeof( void*)
* _temp386); struct _tagged_ptr0 _temp393={ _temp387, _temp387, _temp387 +
_temp386};{ unsigned int _temp388= _temp386; unsigned int i; for( i= 0; i <
_temp388; i ++){ _temp387[ i]=({ struct _tagged_ptr1 _temp390= x; struct _tuple0*
_temp392= _temp390.curr +( int) i; if( _temp390.base == 0? 1:( _temp392 <
_temp390.base? 1: _temp392 >= _temp390.last_plus_one)){ _throw( Null_Exception);}*
_temp392;}).f1;}}; _temp393;}); _temp377.f2=({ unsigned int _temp378=(
unsigned int) sx; void** _temp379=( void**) GC_malloc( sizeof( void*) * _temp378);
struct _tagged_ptr0 _temp385={ _temp379, _temp379, _temp379 + _temp378};{
unsigned int _temp380= _temp378; unsigned int i; for( i= 0; i < _temp380; i ++){
_temp379[ i]=({ struct _tagged_ptr1 _temp382= x; struct _tuple0* _temp384=
_temp382.curr +( int) i; if( _temp382.base == 0? 1:( _temp384 < _temp382.base? 1:
_temp384 >= _temp382.last_plus_one)){ _throw( Null_Exception);}* _temp384;}).f2;}};
_temp385;}); _temp377;});} int Cyc_Array_memq( struct _tagged_ptr0 l, void* x){
int s=( int)({ struct _tagged_ptr0 _temp394= l;( unsigned int)( _temp394.last_plus_one
- _temp394.curr);});{ int i= 0; for( 0; i < s; i ++){ if(({ struct _tagged_ptr0
_temp395= l; void** _temp397= _temp395.curr + i; if( _temp395.base == 0? 1:(
_temp397 < _temp395.base? 1: _temp397 >= _temp395.last_plus_one)){ _throw(
Null_Exception);}* _temp397;}) == x){ return 1;}}} return 0;} int Cyc_Array_mem(
int(* compare)( void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({
struct _tagged_ptr0 _temp398= l;( unsigned int)( _temp398.last_plus_one -
_temp398.curr);});{ int i= 0; for( 0; i < s; i ++){ if( 0 == compare(({ struct
_tagged_ptr0 _temp399= l; void** _temp401= _temp399.curr + i; if( _temp399.base
== 0? 1:( _temp401 < _temp399.base? 1: _temp401 >= _temp399.last_plus_one)){
_throw( Null_Exception);}* _temp401;}), x)){ return 1;}}} return 0;} struct
_tagged_ptr0 Cyc_Array_extract( struct _tagged_ptr0 x, int start, int* n_opt){
int sx=( int)({ struct _tagged_ptr0 _temp402= x;( unsigned int)( _temp402.last_plus_one
- _temp402.curr);}); int n= n_opt == 0? sx - start:({ int* _temp403= n_opt; if(
_temp403 == 0){ _throw( Null_Exception);}* _temp403;}); if(( start < 0? 1: n <=
0)? 1: start +( n_opt == 0? 0: n) > sx){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp404=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp404[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp405; _temp405.tag= Cyc_Core_InvalidArg_tag;
_temp405.f1=( struct _tagged_string)({ char* _temp406=( char*)"Array::extract";
struct _tagged_string _temp407; _temp407.curr= _temp406; _temp407.base= _temp406;
_temp407.last_plus_one= _temp406 + 15; _temp407;}); _temp405;}); _temp404;}));}
return({ unsigned int _temp408=( unsigned int) n; void** _temp409=( void**)
GC_malloc( sizeof( void*) * _temp408); struct _tagged_ptr0 _temp415={ _temp409,
_temp409, _temp409 + _temp408};{ unsigned int _temp410= _temp408; unsigned int i;
for( i= 0; i < _temp410; i ++){ _temp409[ i]=({ struct _tagged_ptr0 _temp412= x;
void** _temp414= _temp412.curr +( int)( start + i); if( _temp412.base == 0? 1:(
_temp414 < _temp412.base? 1: _temp414 >= _temp412.last_plus_one)){ _throw(
Null_Exception);}* _temp414;});}}; _temp415;});}