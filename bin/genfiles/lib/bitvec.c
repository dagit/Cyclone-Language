#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; typedef int
Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
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
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
typedef struct _tagged_ptr0 Cyc_Bitvec_gbitvec_t; typedef struct _tagged_ptr0
Cyc_Bitvec_bitvec_t; extern int Cyc_Bitvec_get( struct _tagged_ptr0, int);
extern void Cyc_Bitvec_set( struct _tagged_ptr0, int); extern void Cyc_Bitvec_clear(
struct _tagged_ptr0, int); extern int Cyc_Bitvec_get_and_set( struct
_tagged_ptr0, int); extern void Cyc_Bitvec_union_two( struct _tagged_ptr0 dest,
struct _tagged_ptr0 src1, struct _tagged_ptr0 src2); extern void Cyc_Bitvec_intersect_two(
struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2);
extern void Cyc_Bitvec_diff_two( struct _tagged_ptr0 dest, struct _tagged_ptr0
src1, struct _tagged_ptr0 src2); extern int Cyc_Bitvec_compare_two( struct
_tagged_ptr0 src1, struct _tagged_ptr0 src2); extern struct _tagged_ptr0 Cyc_Bitvec_new_empty(
int); extern struct _tagged_ptr0 Cyc_Bitvec_new_full( int); extern struct
_tagged_ptr0 Cyc_Bitvec_new_copy( struct _tagged_ptr0); extern struct
_tagged_ptr0 Cyc_Bitvec_from_list( struct Cyc_Dict_Dict* d, int(* f)( void*),
int sz, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Bitvec_to_sorted_list(
struct _tagged_ptr0 bvec, int sz); extern void Cyc_Bitvec_clear_all( struct
_tagged_ptr0); extern void Cyc_Bitvec_set_all( struct _tagged_ptr0); extern int
Cyc_Bitvec_all_set( struct _tagged_ptr0 bvec, int sz); extern void Cyc_Bitvec_print_bvec(
struct _tagged_ptr0 bvec); int Cyc_Bitvec_get( struct _tagged_ptr0 bvec, int pos){
int word= pos >> 5; int offset= pos & 31; return(({ struct _tagged_ptr0 _temp0=
bvec; int* _temp2= _temp0.curr + word; if( _temp0.base == 0? 1:( _temp2 < _temp0.base?
1: _temp2 >= _temp0.last_plus_one)){ _throw( Null_Exception);}* _temp2;}) >>
offset & 1) == 1;} void Cyc_Bitvec_set( struct _tagged_ptr0 bvec, int pos){ int
word= pos >> 5; int offset= pos & 31;({ struct _tagged_ptr0 _temp3= bvec; int*
_temp5= _temp3.curr + word; if( _temp3.base == 0? 1:( _temp5 < _temp3.base? 1:
_temp5 >= _temp3.last_plus_one)){ _throw( Null_Exception);}* _temp5=({ struct
_tagged_ptr0 _temp6= bvec; int* _temp8= _temp6.curr + word; if( _temp6.base == 0?
1:( _temp8 < _temp6.base? 1: _temp8 >= _temp6.last_plus_one)){ _throw(
Null_Exception);}* _temp8;}) | 1 << offset;});} void Cyc_Bitvec_clear( struct
_tagged_ptr0 bvec, int pos){ int word= pos >> 5; int offset= pos & 31;({ struct
_tagged_ptr0 _temp9= bvec; int* _temp11= _temp9.curr + word; if( _temp9.base ==
0? 1:( _temp11 < _temp9.base? 1: _temp11 >= _temp9.last_plus_one)){ _throw(
Null_Exception);}* _temp11=({ struct _tagged_ptr0 _temp12= bvec; int* _temp14=
_temp12.curr + word; if( _temp12.base == 0? 1:( _temp14 < _temp12.base? 1:
_temp14 >= _temp12.last_plus_one)){ _throw( Null_Exception);}* _temp14;}) & ~( 1
<< offset);});} int Cyc_Bitvec_get_and_set( struct _tagged_ptr0 bvec, int pos){
int word= pos >> 5; int offset= pos & 31; int slot=({ struct _tagged_ptr0
_temp15= bvec; int* _temp17= _temp15.curr + word; if( _temp15.base == 0? 1:(
_temp17 < _temp15.base? 1: _temp17 >= _temp15.last_plus_one)){ _throw(
Null_Exception);}* _temp17;}); int ans=( slot >> offset & 1) == 1; if( ! ans){({
struct _tagged_ptr0 _temp18= bvec; int* _temp20= _temp18.curr + word; if(
_temp18.base == 0? 1:( _temp20 < _temp18.base? 1: _temp20 >= _temp18.last_plus_one)){
_throw( Null_Exception);}* _temp20= slot | 1 << offset;});} return ans;} void
Cyc_Bitvec_union_two( struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct
_tagged_ptr0 src2){ int len=( int)({ struct _tagged_ptr0 _temp21= dest;(
unsigned int)( _temp21.last_plus_one - _temp21.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp22= dest; int* _temp24= _temp22.curr + i;
if( _temp22.base == 0? 1:( _temp24 < _temp22.base? 1: _temp24 >= _temp22.last_plus_one)){
_throw( Null_Exception);}* _temp24=({ struct _tagged_ptr0 _temp25= src1; int*
_temp27= _temp25.curr + i; if( _temp25.base == 0? 1:( _temp27 < _temp25.base? 1:
_temp27 >= _temp25.last_plus_one)){ _throw( Null_Exception);}* _temp27;}) |({
struct _tagged_ptr0 _temp28= src2; int* _temp30= _temp28.curr + i; if( _temp28.base
== 0? 1:( _temp30 < _temp28.base? 1: _temp30 >= _temp28.last_plus_one)){ _throw(
Null_Exception);}* _temp30;});});}} void Cyc_Bitvec_intersect_two( struct
_tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=(
int)({ struct _tagged_ptr0 _temp31= dest;( unsigned int)( _temp31.last_plus_one
- _temp31.curr);}); int i= 0; for( 0; i < len; ++ i){({ struct _tagged_ptr0
_temp32= dest; int* _temp34= _temp32.curr + i; if( _temp32.base == 0? 1:(
_temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one)){ _throw(
Null_Exception);}* _temp34=({ struct _tagged_ptr0 _temp35= src1; int* _temp37=
_temp35.curr + i; if( _temp35.base == 0? 1:( _temp37 < _temp35.base? 1: _temp37
>= _temp35.last_plus_one)){ _throw( Null_Exception);}* _temp37;}) &({ struct
_tagged_ptr0 _temp38= src2; int* _temp40= _temp38.curr + i; if( _temp38.base ==
0? 1:( _temp40 < _temp38.base? 1: _temp40 >= _temp38.last_plus_one)){ _throw(
Null_Exception);}* _temp40;});});}} void Cyc_Bitvec_diff_two( struct
_tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=(
int)({ struct _tagged_ptr0 _temp41= dest;( unsigned int)( _temp41.last_plus_one
- _temp41.curr);}); int i= 0; for( 0; i < len; ++ i){({ struct _tagged_ptr0
_temp42= dest; int* _temp44= _temp42.curr + i; if( _temp42.base == 0? 1:(
_temp44 < _temp42.base? 1: _temp44 >= _temp42.last_plus_one)){ _throw(
Null_Exception);}* _temp44=({ struct _tagged_ptr0 _temp45= src1; int* _temp47=
_temp45.curr + i; if( _temp45.base == 0? 1:( _temp47 < _temp45.base? 1: _temp47
>= _temp45.last_plus_one)){ _throw( Null_Exception);}* _temp47;}) & ~({ struct
_tagged_ptr0 _temp48= src2; int* _temp50= _temp48.curr + i; if( _temp48.base ==
0? 1:( _temp50 < _temp48.base? 1: _temp50 >= _temp48.last_plus_one)){ _throw(
Null_Exception);}* _temp50;});});}} int Cyc_Bitvec_compare_two( struct
_tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=( int)({ struct
_tagged_ptr0 _temp51= src1;( unsigned int)( _temp51.last_plus_one - _temp51.curr);});{
int i= 0; for( 0; i < len; ++ i){ if(({ struct _tagged_ptr0 _temp52= src1; int*
_temp54= _temp52.curr + i; if( _temp52.base == 0? 1:( _temp54 < _temp52.base? 1:
_temp54 >= _temp52.last_plus_one)){ _throw( Null_Exception);}* _temp54;}) !=({
struct _tagged_ptr0 _temp55= src2; int* _temp57= _temp55.curr + i; if( _temp55.base
== 0? 1:( _temp57 < _temp55.base? 1: _temp57 >= _temp55.last_plus_one)){ _throw(
Null_Exception);}* _temp57;})){ return 0;}}} return 1;} struct _tagged_ptr0 Cyc_Bitvec_new_empty(
int sz){ struct _tagged_ptr0 ans=({ unsigned int _temp58=( unsigned int)( sz /
32 + 1); int* _temp59=( int*) GC_malloc_atomic( sizeof( int) * _temp58); struct
_tagged_ptr0 _temp62={ _temp59, _temp59, _temp59 + _temp58};{ unsigned int
_temp60= _temp58; unsigned int i; for( i= 0; i < _temp60; i ++){ _temp59[ i]= 0;}};
_temp62;}); return ans;} struct _tagged_ptr0 Cyc_Bitvec_new_full( int sz){
struct _tagged_ptr0 ans=({ unsigned int _temp63=( unsigned int)( sz / 32 + 1);
int* _temp64=( int*) GC_malloc_atomic( sizeof( int) * _temp63); struct
_tagged_ptr0 _temp67={ _temp64, _temp64, _temp64 + _temp63};{ unsigned int
_temp65= _temp63; unsigned int i; for( i= 0; i < _temp65; i ++){ _temp64[ i]= -1;}};
_temp67;}); return ans;} struct _tagged_ptr0 Cyc_Bitvec_new_copy( struct
_tagged_ptr0 old){ struct _tagged_ptr0 copy= Cyc_Bitvec_new_empty(( int)({
struct _tagged_ptr0 _temp68= old;( unsigned int)( _temp68.last_plus_one -
_temp68.curr);})); Cyc_Bitvec_union_two( copy, copy, old); return copy;} struct
_tagged_ptr0 Cyc_Bitvec_from_list( struct Cyc_Dict_Dict* d, int(* f)( void*),
int sz, struct Cyc_List_List* l){ struct _tagged_ptr0 ans=({ unsigned int
_temp69=( unsigned int)( sz % 32 + 1); int* _temp70=( int*) GC_malloc_atomic(
sizeof( int) * _temp69); struct _tagged_ptr0 _temp73={ _temp70, _temp70, _temp70
+ _temp69};{ unsigned int _temp71= _temp69; unsigned int i; for( i= 0; i <
_temp71; i ++){ _temp70[ i]= 0;}}; _temp73;}); for( 0; l != 0; l=({ struct Cyc_List_List*
_temp74= l; if( _temp74 == 0){ _throw( Null_Exception);} _temp74->tl;})){ Cyc_Bitvec_set(
ans, f((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d,(
void*)({ struct Cyc_List_List* _temp75= l; if( _temp75 == 0){ _throw(
Null_Exception);} _temp75->hd;}))));} return ans;} struct Cyc_List_List* Cyc_Bitvec_to_sorted_list(
struct _tagged_ptr0 bvec, int sz){ struct Cyc_List_List* ans= 0;{ int pos= sz -
1; for( 0; pos >= 0; 0){ int word= pos >> 5; int bits=({ struct _tagged_ptr0
_temp76= bvec; int* _temp78= _temp76.curr + word; if( _temp76.base == 0? 1:(
_temp78 < _temp76.base? 1: _temp78 >= _temp76.last_plus_one)){ _throw(
Null_Exception);}* _temp78;}); int offset= pos & 31; for( 0; offset >= 0; --
offset, -- pos){ if(( bits >> offset & 1) == 1){ ans=({ struct Cyc_List_List*
_temp79=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp79[ 0]=({ struct Cyc_List_List _temp80; _temp80.hd=( void*) pos; _temp80.tl=
ans; _temp80;}); _temp79;});}}}} return ans;} void Cyc_Bitvec_clear_all( struct
_tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp81= bvec;(
unsigned int)( _temp81.last_plus_one - _temp81.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp82= bvec; int* _temp84= _temp82.curr + i;
if( _temp82.base == 0? 1:( _temp84 < _temp82.base? 1: _temp84 >= _temp82.last_plus_one)){
_throw( Null_Exception);}* _temp84= 0;});}} void Cyc_Bitvec_set_all( struct
_tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp85= bvec;(
unsigned int)( _temp85.last_plus_one - _temp85.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp86= bvec; int* _temp88= _temp86.curr + i;
if( _temp86.base == 0? 1:( _temp88 < _temp86.base? 1: _temp88 >= _temp86.last_plus_one)){
_throw( Null_Exception);}* _temp88= -1;});}} int Cyc_Bitvec_all_set( struct
_tagged_ptr0 bvec, int sz){ int words= sz >> 5;{ int i= 0; for( 0; i < words; i
++){ if(({ struct _tagged_ptr0 _temp89= bvec; int* _temp91= _temp89.curr + i;
if( _temp89.base == 0? 1:( _temp91 < _temp89.base? 1: _temp91 >= _temp89.last_plus_one)){
_throw( Null_Exception);}* _temp91;}) != -1){ return 0;}}}{ int i= words * 32;
for( 0; i < sz; i ++){ if( ! Cyc_Bitvec_get( bvec, i)){ return 0;}}} return 1;}
void Cyc_Bitvec_print_bvec( struct _tagged_ptr0 bvec){{ int i= 0; for( 0; i < 32
*({ struct _tagged_ptr0 _temp92= bvec;( unsigned int)( _temp92.last_plus_one -
_temp92.curr);}); ++ i){ printf("%d", Cyc_Bitvec_get( bvec, i)? 1: 0);}} printf("\n");}