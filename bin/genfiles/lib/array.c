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
int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({ unsigned int _temp186=(
unsigned int) len; void** _temp187=( void**) GC_malloc( sizeof( void*) *
_temp186); struct _tagged_ptr0 _temp193={ _temp187, _temp187, _temp187 +
_temp186};{ unsigned int _temp188= _temp186; unsigned int i; for( i= 0; i <
_temp188; i ++){ _temp187[ i]=({ struct _tagged_ptr0 _temp190= arr; void**
_temp192= _temp190.curr + 0; if( _temp190.base == 0? 1:( _temp192 < _temp190.base?
1: _temp192 >= _temp190.last_plus_one)){ _throw( Null_Exception);}* _temp192;});}};
_temp193;}); struct _tagged_ptr0 to=({ unsigned int _temp178=( unsigned int) len;
void** _temp179=( void**) GC_malloc( sizeof( void*) * _temp178); struct
_tagged_ptr0 _temp185={ _temp179, _temp179, _temp179 + _temp178};{ unsigned int
_temp180= _temp178; unsigned int i; for( i= 0; i < _temp180; i ++){ _temp179[ i]=({
struct _tagged_ptr0 _temp182= arr; void** _temp184= _temp182.curr +( int)( i +
base_ofs); if( _temp182.base == 0? 1:( _temp184 < _temp182.base? 1: _temp184 >=
_temp182.last_plus_one)){ _throw( Null_Exception);}* _temp184;});}}; _temp185;});
struct _tagged_ptr0 swap; int stepsize; int start; int lstart; int lend; int
rstart; int rend; int dest; for( stepsize= 1; stepsize < len; stepsize= stepsize
* 2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start < limit_ofs;
start= start + stepsize * 2){ lstart= start; rstart= start + stepsize <
limit_ofs? start + stepsize: limit_ofs; lend= rstart; rend= start + stepsize * 2
< limit_ofs? start + stepsize * 2: limit_ofs; while( lstart < lend? rstart <
rend: 0) { if( less_eq(({ struct _tagged_ptr0 _temp142= from; void** _temp144=
_temp142.curr + lstart; if( _temp142.base == 0? 1:( _temp144 < _temp142.base? 1:
_temp144 >= _temp142.last_plus_one)){ _throw( Null_Exception);}* _temp144;}),({
struct _tagged_ptr0 _temp145= from; void** _temp147= _temp145.curr + rstart; if(
_temp145.base == 0? 1:( _temp147 < _temp145.base? 1: _temp147 >= _temp145.last_plus_one)){
_throw( Null_Exception);}* _temp147;})) <= 0){({ struct _tagged_ptr0 _temp148=
to; void** _temp150= _temp148.curr +( dest ++); if( _temp148.base == 0? 1:(
_temp150 < _temp148.base? 1: _temp150 >= _temp148.last_plus_one)){ _throw(
Null_Exception);}* _temp150=({ struct _tagged_ptr0 _temp151= from; void**
_temp153= _temp151.curr +( lstart ++); if( _temp151.base == 0? 1:( _temp153 <
_temp151.base? 1: _temp153 >= _temp151.last_plus_one)){ _throw( Null_Exception);}*
_temp153;});});} else{({ struct _tagged_ptr0 _temp154= to; void** _temp156=
_temp154.curr +( dest ++); if( _temp154.base == 0? 1:( _temp156 < _temp154.base?
1: _temp156 >= _temp154.last_plus_one)){ _throw( Null_Exception);}* _temp156=({
struct _tagged_ptr0 _temp157= from; void** _temp159= _temp157.curr +( rstart ++);
if( _temp157.base == 0? 1:( _temp159 < _temp157.base? 1: _temp159 >= _temp157.last_plus_one)){
_throw( Null_Exception);}* _temp159;});});}} while( lstart < lend) {({ struct
_tagged_ptr0 _temp160= to; void** _temp162= _temp160.curr +( dest ++); if(
_temp160.base == 0? 1:( _temp162 < _temp160.base? 1: _temp162 >= _temp160.last_plus_one)){
_throw( Null_Exception);}* _temp162=({ struct _tagged_ptr0 _temp163= from; void**
_temp165= _temp163.curr +( lstart ++); if( _temp163.base == 0? 1:( _temp165 <
_temp163.base? 1: _temp165 >= _temp163.last_plus_one)){ _throw( Null_Exception);}*
_temp165;});});} while( rstart < rend) {({ struct _tagged_ptr0 _temp166= to;
void** _temp168= _temp166.curr +( dest ++); if( _temp166.base == 0? 1:( _temp168
< _temp166.base? 1: _temp168 >= _temp166.last_plus_one)){ _throw( Null_Exception);}*
_temp168=({ struct _tagged_ptr0 _temp169= from; void** _temp171= _temp169.curr +(
rstart ++); if( _temp169.base == 0? 1:( _temp171 < _temp169.base? 1: _temp171 >=
_temp169.last_plus_one)){ _throw( Null_Exception);}* _temp171;});});}}}{ int i=
0; for( 0; i < len; i ++){({ struct _tagged_ptr0 _temp172= arr; void** _temp174=
_temp172.curr +( i + base_ofs); if( _temp172.base == 0? 1:( _temp174 < _temp172.base?
1: _temp174 >= _temp172.last_plus_one)){ _throw( Null_Exception);}* _temp174=({
struct _tagged_ptr0 _temp175= to; void** _temp177= _temp175.curr + i; if(
_temp175.base == 0? 1:( _temp177 < _temp175.base? 1: _temp177 >= _temp175.last_plus_one)){
_throw( Null_Exception);}* _temp177;});});}}}} struct _tagged_ptr0 Cyc_Array_from_list(
struct Cyc_List_List* x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List*
x)) Cyc_List_to_array)( x);} struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_ptr0 x){ return(( struct Cyc_List_List*(*)( struct _tagged_ptr0 arr))
Cyc_List_from_array)( x);} struct _tagged_ptr0 Cyc_Array_copy( struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp202= x;( unsigned int)(
_temp202.last_plus_one - _temp202.curr);}); return({ unsigned int _temp194=(
unsigned int) sx; void** _temp195=( void**) GC_malloc( sizeof( void*) * _temp194);
struct _tagged_ptr0 _temp201={ _temp195, _temp195, _temp195 + _temp194};{
unsigned int _temp196= _temp194; unsigned int i; for( i= 0; i < _temp196; i ++){
_temp195[ i]=({ struct _tagged_ptr0 _temp198= x; void** _temp200= _temp198.curr
+( int) i; if( _temp198.base == 0? 1:( _temp200 < _temp198.base? 1: _temp200 >=
_temp198.last_plus_one)){ _throw( Null_Exception);}* _temp200;});}}; _temp201;});}
struct _tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp211= x;( unsigned int)( _temp211.last_plus_one
- _temp211.curr);}); return({ unsigned int _temp203=( unsigned int) sx; void**
_temp204=( void**) GC_malloc( sizeof( void*) * _temp203); struct _tagged_ptr0
_temp210={ _temp204, _temp204, _temp204 + _temp203};{ unsigned int _temp205=
_temp203; unsigned int i; for( i= 0; i < _temp205; i ++){ _temp204[ i]= f(({
struct _tagged_ptr0 _temp207= x; void** _temp209= _temp207.curr +( int) i; if(
_temp207.base == 0? 1:( _temp209 < _temp207.base? 1: _temp209 >= _temp207.last_plus_one)){
_throw( Null_Exception);}* _temp209;}));}}; _temp210;});} struct _tagged_ptr0
Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp220= x;( unsigned int)( _temp220.last_plus_one
- _temp220.curr);}); return({ unsigned int _temp212=( unsigned int) sx; void**
_temp213=( void**) GC_malloc( sizeof( void*) * _temp212); struct _tagged_ptr0
_temp219={ _temp213, _temp213, _temp213 + _temp212};{ unsigned int _temp214=
_temp212; unsigned int i; for( i= 0; i < _temp214; i ++){ _temp213[ i]= f( env,({
struct _tagged_ptr0 _temp216= x; void** _temp218= _temp216.curr +( int) i; if(
_temp216.base == 0? 1:( _temp218 < _temp216.base? 1: _temp218 >= _temp216.last_plus_one)){
_throw( Null_Exception);}* _temp218;}));}}; _temp219;});} void Cyc_Array_imp_map(
void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp227= x;( unsigned int)( _temp227.last_plus_one - _temp227.curr);}); int i=
0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp221= x; void** _temp223=
_temp221.curr + i; if( _temp221.base == 0? 1:( _temp223 < _temp221.base? 1:
_temp223 >= _temp221.last_plus_one)){ _throw( Null_Exception);}* _temp223= f(({
struct _tagged_ptr0 _temp224= x; void** _temp226= _temp224.curr + i; if(
_temp224.base == 0? 1:( _temp226 < _temp224.base? 1: _temp226 >= _temp224.last_plus_one)){
_throw( Null_Exception);}* _temp226;}));});}} void Cyc_Array_imp_map_c( void*(*
f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp234= x;( unsigned int)( _temp234.last_plus_one - _temp234.curr);});
int i= 0; for( 0; i < sx; i ++){({ struct _tagged_ptr0 _temp228= x; void**
_temp230= _temp228.curr + i; if( _temp228.base == 0? 1:( _temp230 < _temp228.base?
1: _temp230 >= _temp228.last_plus_one)){ _throw( Null_Exception);}* _temp230= f(
env,({ struct _tagged_ptr0 _temp231= x; void** _temp233= _temp231.curr + i; if(
_temp231.base == 0? 1:( _temp233 < _temp231.base? 1: _temp233 >= _temp231.last_plus_one)){
_throw( Null_Exception);}* _temp233;}));});}} char Cyc_Array_Array_mismatch_tag[
15u]="Array_mismatch"; struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*,
void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct
_tagged_ptr0 _temp249= x;( unsigned int)( _temp249.last_plus_one - _temp249.curr);});
if( sx !=({ struct _tagged_ptr0 _temp235= y;( unsigned int)( _temp235.last_plus_one
- _temp235.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp236=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp236[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp237; _temp237.tag= Cyc_Array_Array_mismatch_tag; _temp237;}); _temp236;}));}
return({ unsigned int _temp238=( unsigned int) sx; void** _temp239=( void**)
GC_malloc( sizeof( void*) * _temp238); struct _tagged_ptr0 _temp248={ _temp239,
_temp239, _temp239 + _temp238};{ unsigned int _temp240= _temp238; unsigned int i;
for( i= 0; i < _temp240; i ++){ _temp239[ i]= f(({ struct _tagged_ptr0 _temp242=
x; void** _temp244= _temp242.curr +( int) i; if( _temp242.base == 0? 1:(
_temp244 < _temp242.base? 1: _temp244 >= _temp242.last_plus_one)){ _throw(
Null_Exception);}* _temp244;}),({ struct _tagged_ptr0 _temp245= y; void**
_temp247= _temp245.curr +( int) i; if( _temp245.base == 0? 1:( _temp247 <
_temp245.base? 1: _temp247 >= _temp245.last_plus_one)){ _throw( Null_Exception);}*
_temp247;}));}}; _temp248;});} void Cyc_Array_app( void*(* f)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp253= x;( unsigned int)(
_temp253.last_plus_one - _temp253.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({
struct _tagged_ptr0 _temp250= x; void** _temp252= _temp250.curr + i; if(
_temp250.base == 0? 1:( _temp252 < _temp250.base? 1: _temp252 >= _temp250.last_plus_one)){
_throw( Null_Exception);}* _temp252;}));}} void Cyc_Array_app_c( void*(* f)(
void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp257= x;( unsigned int)( _temp257.last_plus_one - _temp257.curr);});
int i= 0; for( 0; i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp254= x; void**
_temp256= _temp254.curr + i; if( _temp254.base == 0? 1:( _temp256 < _temp254.base?
1: _temp256 >= _temp254.last_plus_one)){ _throw( Null_Exception);}* _temp256;}));}}
void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp261= x;( unsigned int)( _temp261.last_plus_one -
_temp261.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp258= x; void** _temp260= _temp258.curr + i; if( _temp258.base == 0? 1:(
_temp260 < _temp258.base? 1: _temp260 >= _temp258.last_plus_one)){ _throw(
Null_Exception);}* _temp260;}));}} void Cyc_Array_iter_c( void(* f)( void*, void*),
void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp265=
x;( unsigned int)( _temp265.last_plus_one - _temp265.curr);}); int i= 0; for( 0;
i < sx; i ++){ f( env,({ struct _tagged_ptr0 _temp262= x; void** _temp264=
_temp262.curr + i; if( _temp262.base == 0? 1:( _temp264 < _temp262.base? 1:
_temp264 >= _temp262.last_plus_one)){ _throw( Null_Exception);}* _temp264;}));}}
void Cyc_Array_app2( void*(* f)( void*, void*), struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp275= x;( unsigned int)(
_temp275.last_plus_one - _temp275.curr);}); if( sx !=({ struct _tagged_ptr0
_temp266= y;( unsigned int)( _temp266.last_plus_one - _temp266.curr);})){( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct*
_temp267=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct
Cyc_Array_Array_mismatch_struct)); _temp267[ 0]=({ struct Cyc_Array_Array_mismatch_struct
_temp268; _temp268.tag= Cyc_Array_Array_mismatch_tag; _temp268;}); _temp267;}));}{
int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0 _temp269= x; void**
_temp271= _temp269.curr + i; if( _temp269.base == 0? 1:( _temp271 < _temp269.base?
1: _temp271 >= _temp269.last_plus_one)){ _throw( Null_Exception);}* _temp271;}),({
struct _tagged_ptr0 _temp272= y; void** _temp274= _temp272.curr + i; if(
_temp272.base == 0? 1:( _temp274 < _temp272.base? 1: _temp274 >= _temp272.last_plus_one)){
_throw( Null_Exception);}* _temp274;}));}}} void Cyc_Array_app2_c( void*(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){
int sx=( int)({ struct _tagged_ptr0 _temp285= x;( unsigned int)( _temp285.last_plus_one
- _temp285.curr);}); if( sx !=({ struct _tagged_ptr0 _temp276= y;( unsigned int)(
_temp276.last_plus_one - _temp276.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp277=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp277[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp278; _temp278.tag=
Cyc_Array_Array_mismatch_tag; _temp278;}); _temp277;}));}{ int i= 0; for( 0; i <
sx; i ++){ f( env,({ struct _tagged_ptr0 _temp279= x; void** _temp281= _temp279.curr
+ i; if( _temp279.base == 0? 1:( _temp281 < _temp279.base? 1: _temp281 >=
_temp279.last_plus_one)){ _throw( Null_Exception);}* _temp281;}),({ struct
_tagged_ptr0 _temp282= y; void** _temp284= _temp282.curr + i; if( _temp282.base
== 0? 1:( _temp284 < _temp282.base? 1: _temp284 >= _temp282.last_plus_one)){
_throw( Null_Exception);}* _temp284;}));}}} void Cyc_Array_iter2( void(* f)(
void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({
struct _tagged_ptr0 _temp295= x;( unsigned int)( _temp295.last_plus_one -
_temp295.curr);}); if( sx !=({ struct _tagged_ptr0 _temp286= y;( unsigned int)(
_temp286.last_plus_one - _temp286.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp287=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp287[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp288; _temp288.tag=
Cyc_Array_Array_mismatch_tag; _temp288;}); _temp287;}));}{ int i= 0; for( 0; i <
sx; i ++){ f(({ struct _tagged_ptr0 _temp289= x; void** _temp291= _temp289.curr
+ i; if( _temp289.base == 0? 1:( _temp291 < _temp289.base? 1: _temp291 >=
_temp289.last_plus_one)){ _throw( Null_Exception);}* _temp291;}),({ struct
_tagged_ptr0 _temp292= y; void** _temp294= _temp292.curr + i; if( _temp292.base
== 0? 1:( _temp294 < _temp292.base? 1: _temp294 >= _temp292.last_plus_one)){
_throw( Null_Exception);}* _temp294;}));}}} void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y){
int sx=( int)({ struct _tagged_ptr0 _temp305= x;( unsigned int)( _temp305.last_plus_one
- _temp305.curr);}); if( sx !=({ struct _tagged_ptr0 _temp296= y;( unsigned int)(
_temp296.last_plus_one - _temp296.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp297=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp297[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp298; _temp298.tag=
Cyc_Array_Array_mismatch_tag; _temp298;}); _temp297;}));}{ int i= 0; for( 0; i <
sx; i ++){ f( env,({ struct _tagged_ptr0 _temp299= x; void** _temp301= _temp299.curr
+ i; if( _temp299.base == 0? 1:( _temp301 < _temp299.base? 1: _temp301 >=
_temp299.last_plus_one)){ _throw( Null_Exception);}* _temp301;}),({ struct
_tagged_ptr0 _temp302= y; void** _temp304= _temp302.curr + i; if( _temp302.base
== 0? 1:( _temp304 < _temp302.base? 1: _temp304 >= _temp302.last_plus_one)){
_throw( Null_Exception);}* _temp304;}));}}} void* Cyc_Array_fold_left( void*(* f)(
void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp309= x;( unsigned int)( _temp309.last_plus_one - _temp309.curr);});{
int i= 0; for( 0; i < sx; i ++){ accum= f( accum,({ struct _tagged_ptr0 _temp306=
x; void** _temp308= _temp306.curr + i; if( _temp306.base == 0? 1:( _temp308 <
_temp306.base? 1: _temp308 >= _temp306.last_plus_one)){ _throw( Null_Exception);}*
_temp308;}));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp313= x;( unsigned int)( _temp313.last_plus_one -
_temp313.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,({
struct _tagged_ptr0 _temp310= x; void** _temp312= _temp310.curr + i; if(
_temp310.base == 0? 1:( _temp312 < _temp310.base? 1: _temp312 >= _temp310.last_plus_one)){
_throw( Null_Exception);}* _temp312;}));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({
struct _tagged_ptr0 _temp317= x;( unsigned int)( _temp317.last_plus_one -
_temp317.curr);}) - 1); for( 0; i >= 0; i --){ accum= f(({ struct _tagged_ptr0
_temp314= x; void** _temp316= _temp314.curr + i; if( _temp314.base == 0? 1:(
_temp316 < _temp314.base? 1: _temp316 >= _temp314.last_plus_one)){ _throw(
Null_Exception);}* _temp316;}), accum);}} return accum;} void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum){{
int i=( int)(({ struct _tagged_ptr0 _temp321= x;( unsigned int)( _temp321.last_plus_one
- _temp321.curr);}) - 1); for( 0; i >= 0; i --){ accum= f( env,({ struct
_tagged_ptr0 _temp318= x; void** _temp320= _temp318.curr + i; if( _temp318.base
== 0? 1:( _temp320 < _temp318.base? 1: _temp320 >= _temp318.last_plus_one)){
_throw( Null_Exception);}* _temp320;}), accum);}} return accum;} struct
_tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x){ int sx=( int)({ struct
_tagged_ptr0 _temp330= x;( unsigned int)( _temp330.last_plus_one - _temp330.curr);});
int n= sx - 1; return({ unsigned int _temp322=( unsigned int) sx; void**
_temp323=( void**) GC_malloc( sizeof( void*) * _temp322); struct _tagged_ptr0
_temp329={ _temp323, _temp323, _temp323 + _temp322};{ unsigned int _temp324=
_temp322; unsigned int i; for( i= 0; i < _temp324; i ++){ _temp323[ i]=({ struct
_tagged_ptr0 _temp326= x; void** _temp328= _temp326.curr +( int)( n - i); if(
_temp326.base == 0? 1:( _temp328 < _temp326.base? 1: _temp328 >= _temp326.last_plus_one)){
_throw( Null_Exception);}* _temp328;});}}; _temp329;});} void Cyc_Array_imp_rev(
struct _tagged_ptr0 x){ void* temp; int i= 0; int j=( int)(({ struct
_tagged_ptr0 _temp343= x;( unsigned int)( _temp343.last_plus_one - _temp343.curr);})
- 1); while( i < j) { temp=({ struct _tagged_ptr0 _temp331= x; void** _temp333=
_temp331.curr + i; if( _temp331.base == 0? 1:( _temp333 < _temp331.base? 1:
_temp333 >= _temp331.last_plus_one)){ _throw( Null_Exception);}* _temp333;});({
struct _tagged_ptr0 _temp334= x; void** _temp336= _temp334.curr + i; if(
_temp334.base == 0? 1:( _temp336 < _temp334.base? 1: _temp336 >= _temp334.last_plus_one)){
_throw( Null_Exception);}* _temp336=({ struct _tagged_ptr0 _temp337= x; void**
_temp339= _temp337.curr + j; if( _temp337.base == 0? 1:( _temp339 < _temp337.base?
1: _temp339 >= _temp337.last_plus_one)){ _throw( Null_Exception);}* _temp339;});});({
struct _tagged_ptr0 _temp340= x; void** _temp342= _temp340.curr + j; if(
_temp340.base == 0? 1:( _temp342 < _temp340.base? 1: _temp342 >= _temp340.last_plus_one)){
_throw( Null_Exception);}* _temp342= temp;}); i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp347= x;( unsigned int)( _temp347.last_plus_one - _temp347.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( ! pred(({ struct _tagged_ptr0 _temp344= x; void**
_temp346= _temp344.curr + i; if( _temp344.base == 0? 1:( _temp346 < _temp344.base?
1: _temp346 >= _temp344.last_plus_one)){ _throw( Null_Exception);}* _temp346;}))){
return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*, void*), void*
env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp351= x;(
unsigned int)( _temp351.last_plus_one - _temp351.curr);});{ int i= 0; for( 0; i
< sx; i ++){ if( ! pred( env,({ struct _tagged_ptr0 _temp348= x; void** _temp350=
_temp348.curr + i; if( _temp348.base == 0? 1:( _temp350 < _temp348.base? 1:
_temp350 >= _temp348.last_plus_one)){ _throw( Null_Exception);}* _temp350;}))){
return 0;}}} return 1;} int Cyc_Array_exists( int(* pred)( void*), struct
_tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp355= x;( unsigned int)(
_temp355.last_plus_one - _temp355.curr);});{ int i= 0; for( 0; i < sx; i ++){
if( pred(({ struct _tagged_ptr0 _temp352= x; void** _temp354= _temp352.curr + i;
if( _temp352.base == 0? 1:( _temp354 < _temp352.base? 1: _temp354 >= _temp352.last_plus_one)){
_throw( Null_Exception);}* _temp354;}))){ return 1;}}} return 0;} int Cyc_Array_exists_c(
int(* pred)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp359= x;( unsigned int)( _temp359.last_plus_one -
_temp359.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred( env,({ struct
_tagged_ptr0 _temp356= x; void** _temp358= _temp356.curr + i; if( _temp356.base
== 0? 1:( _temp358 < _temp356.base? 1: _temp358 >= _temp356.last_plus_one)){
_throw( Null_Exception);}* _temp358;}))){ return 1;}}} return 0;} struct
_tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int
sx=( int)({ struct _tagged_ptr0 _temp375= x;( unsigned int)( _temp375.last_plus_one
- _temp375.curr);}); if( sx !=({ struct _tagged_ptr0 _temp360= y;( unsigned int)(
_temp360.last_plus_one - _temp360.curr);})){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Array_Array_mismatch_struct* _temp361=( struct
Cyc_Array_Array_mismatch_struct*) GC_malloc( sizeof( struct Cyc_Array_Array_mismatch_struct));
_temp361[ 0]=({ struct Cyc_Array_Array_mismatch_struct _temp362; _temp362.tag=
Cyc_Array_Array_mismatch_tag; _temp362;}); _temp361;}));} return({ unsigned int
_temp363=( unsigned int) sx; struct _tuple0* _temp364=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0) * _temp363); struct _tagged_ptr1 _temp374={
_temp364, _temp364, _temp364 + _temp363};{ unsigned int _temp365= _temp363;
unsigned int i; for( i= 0; i < _temp365; i ++){ _temp364[ i]=({ struct _tuple0
_temp367; _temp367.f1=({ struct _tagged_ptr0 _temp371= x; void** _temp373=
_temp371.curr +( int) i; if( _temp371.base == 0? 1:( _temp373 < _temp371.base? 1:
_temp373 >= _temp371.last_plus_one)){ _throw( Null_Exception);}* _temp373;});
_temp367.f2=({ struct _tagged_ptr0 _temp368= y; void** _temp370= _temp368.curr +(
int) i; if( _temp368.base == 0? 1:( _temp370 < _temp368.base? 1: _temp370 >=
_temp368.last_plus_one)){ _throw( Null_Exception);}* _temp370;}); _temp367;});}};
_temp374;});} struct _tuple1 Cyc_Array_split( struct _tagged_ptr1 x){ int sx=(
int)({ struct _tagged_ptr1 _temp393= x;( unsigned int)( _temp393.last_plus_one -
_temp393.curr);}); return({ struct _tuple1 _temp376; _temp376.f1=({ unsigned int
_temp385=( unsigned int) sx; void** _temp386=( void**) GC_malloc( sizeof( void*)
* _temp385); struct _tagged_ptr0 _temp392={ _temp386, _temp386, _temp386 +
_temp385};{ unsigned int _temp387= _temp385; unsigned int i; for( i= 0; i <
_temp387; i ++){ _temp386[ i]=({ struct _tagged_ptr1 _temp389= x; struct _tuple0*
_temp391= _temp389.curr +( int) i; if( _temp389.base == 0? 1:( _temp391 <
_temp389.base? 1: _temp391 >= _temp389.last_plus_one)){ _throw( Null_Exception);}*
_temp391;}).f1;}}; _temp392;}); _temp376.f2=({ unsigned int _temp377=(
unsigned int) sx; void** _temp378=( void**) GC_malloc( sizeof( void*) * _temp377);
struct _tagged_ptr0 _temp384={ _temp378, _temp378, _temp378 + _temp377};{
unsigned int _temp379= _temp377; unsigned int i; for( i= 0; i < _temp379; i ++){
_temp378[ i]=({ struct _tagged_ptr1 _temp381= x; struct _tuple0* _temp383=
_temp381.curr +( int) i; if( _temp381.base == 0? 1:( _temp383 < _temp381.base? 1:
_temp383 >= _temp381.last_plus_one)){ _throw( Null_Exception);}* _temp383;}).f2;}};
_temp384;}); _temp376;});} int Cyc_Array_memq( struct _tagged_ptr0 l, void* x){
int s=( int)({ struct _tagged_ptr0 _temp397= l;( unsigned int)( _temp397.last_plus_one
- _temp397.curr);});{ int i= 0; for( 0; i < s; i ++){ if(({ struct _tagged_ptr0
_temp394= l; void** _temp396= _temp394.curr + i; if( _temp394.base == 0? 1:(
_temp396 < _temp394.base? 1: _temp396 >= _temp394.last_plus_one)){ _throw(
Null_Exception);}* _temp396;}) == x){ return 1;}}} return 0;} int Cyc_Array_mem(
int(* compare)( void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({
struct _tagged_ptr0 _temp401= l;( unsigned int)( _temp401.last_plus_one -
_temp401.curr);});{ int i= 0; for( 0; i < s; i ++){ if( 0 == compare(({ struct
_tagged_ptr0 _temp398= l; void** _temp400= _temp398.curr + i; if( _temp398.base
== 0? 1:( _temp400 < _temp398.base? 1: _temp400 >= _temp398.last_plus_one)){
_throw( Null_Exception);}* _temp400;}), x)){ return 1;}}} return 0;} struct
_tagged_ptr0 Cyc_Array_extract( struct _tagged_ptr0 x, int start, int* n_opt){
int sx=( int)({ struct _tagged_ptr0 _temp415= x;( unsigned int)( _temp415.last_plus_one
- _temp415.curr);}); int n= n_opt == 0? sx - start:({ int* _temp414= n_opt; if(
_temp414 == 0){ _throw( Null_Exception);}* _temp414;}); if(( start < 0? 1: n <=
0)? 1: start +( n_opt == 0? 0: n) > sx){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp402=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp402[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp403; _temp403.tag= Cyc_Core_InvalidArg_tag;
_temp403.f1=( struct _tagged_string)({ char* _temp404=( char*)"Array::extract";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 15; _temp405;}); _temp403;}); _temp402;}));}
return({ unsigned int _temp406=( unsigned int) n; void** _temp407=( void**)
GC_malloc( sizeof( void*) * _temp406); struct _tagged_ptr0 _temp413={ _temp407,
_temp407, _temp407 + _temp406};{ unsigned int _temp408= _temp406; unsigned int i;
for( i= 0; i < _temp408; i ++){ _temp407[ i]=({ struct _tagged_ptr0 _temp410= x;
void** _temp412= _temp410.curr +( int)( start + i); if( _temp410.base == 0? 1:(
_temp412 < _temp410.base? 1: _temp412 >= _temp410.last_plus_one)){ _throw(
Null_Exception);}* _temp412;});}}; _temp413;});}