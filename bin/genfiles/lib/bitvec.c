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
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); struct
_tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; typedef struct
_tagged_ptr0 Cyc_Bitvec_gbitvec_t; typedef struct _tagged_ptr0 Cyc_Bitvec_bitvec_t;
extern int Cyc_Bitvec_get( struct _tagged_ptr0, int); extern void Cyc_Bitvec_set(
struct _tagged_ptr0, int); extern void Cyc_Bitvec_clear( struct _tagged_ptr0,
int); extern int Cyc_Bitvec_get_and_set( struct _tagged_ptr0, int); extern void
Cyc_Bitvec_union_two( struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct
_tagged_ptr0 src2); extern void Cyc_Bitvec_intersect_two( struct _tagged_ptr0
dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2); extern void Cyc_Bitvec_diff_two(
struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2);
extern int Cyc_Bitvec_compare_two( struct _tagged_ptr0 src1, struct _tagged_ptr0
src2); extern struct _tagged_ptr0 Cyc_Bitvec_new_empty( int); extern struct
_tagged_ptr0 Cyc_Bitvec_new_full( int); extern struct _tagged_ptr0 Cyc_Bitvec_new_copy(
struct _tagged_ptr0); extern struct _tagged_ptr0 Cyc_Bitvec_from_list( struct
Cyc_Dict_Dict* d, int(* f)( void*), int sz, struct Cyc_List_List*); extern
struct Cyc_List_List* Cyc_Bitvec_to_sorted_list( struct _tagged_ptr0 bvec, int
sz); extern void Cyc_Bitvec_clear_all( struct _tagged_ptr0); extern void Cyc_Bitvec_set_all(
struct _tagged_ptr0); extern int Cyc_Bitvec_all_set( struct _tagged_ptr0 bvec,
int sz); extern void Cyc_Bitvec_print_bvec( struct _tagged_ptr0 bvec); int Cyc_Bitvec_get(
struct _tagged_ptr0 bvec, int pos){ int word= pos >> 5; int offset= pos & 31;
return(({ struct _tagged_ptr0 _temp0= bvec; int* _temp2= _temp0.curr + word; if(
_temp0.base == 0? 1:( _temp2 < _temp0.base? 1: _temp2 >= _temp0.last_plus_one)){
_throw( Null_Exception);}* _temp2;}) >> offset & 1) == 1;} void Cyc_Bitvec_set(
struct _tagged_ptr0 bvec, int pos){ int word= pos >> 5; int offset= pos & 31;({
struct _tagged_ptr0 _temp3= bvec; int* _temp5= _temp3.curr + word; if( _temp3.base
== 0? 1:( _temp5 < _temp3.base? 1: _temp5 >= _temp3.last_plus_one)){ _throw(
Null_Exception);}* _temp5=({ struct _tagged_ptr0 _temp6= bvec; int* _temp8=
_temp6.curr + word; if( _temp6.base == 0? 1:( _temp8 < _temp6.base? 1: _temp8 >=
_temp6.last_plus_one)){ _throw( Null_Exception);}* _temp8;}) | 1 << offset;});}
void Cyc_Bitvec_clear( struct _tagged_ptr0 bvec, int pos){ int word= pos >> 5;
int offset= pos & 31;({ struct _tagged_ptr0 _temp9= bvec; int* _temp11= _temp9.curr
+ word; if( _temp9.base == 0? 1:( _temp11 < _temp9.base? 1: _temp11 >= _temp9.last_plus_one)){
_throw( Null_Exception);}* _temp11=({ struct _tagged_ptr0 _temp12= bvec; int*
_temp14= _temp12.curr + word; if( _temp12.base == 0? 1:( _temp14 < _temp12.base?
1: _temp14 >= _temp12.last_plus_one)){ _throw( Null_Exception);}* _temp14;}) & ~(
1 << offset);});} int Cyc_Bitvec_get_and_set( struct _tagged_ptr0 bvec, int pos){
int word= pos >> 5; int offset= pos & 31; int slot=({ struct _tagged_ptr0
_temp18= bvec; int* _temp20= _temp18.curr + word; if( _temp18.base == 0? 1:(
_temp20 < _temp18.base? 1: _temp20 >= _temp18.last_plus_one)){ _throw(
Null_Exception);}* _temp20;}); int ans=( slot >> offset & 1) == 1; if( ! ans){({
struct _tagged_ptr0 _temp15= bvec; int* _temp17= _temp15.curr + word; if(
_temp15.base == 0? 1:( _temp17 < _temp15.base? 1: _temp17 >= _temp15.last_plus_one)){
_throw( Null_Exception);}* _temp17= slot | 1 << offset;});} return ans;} void
Cyc_Bitvec_union_two( struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct
_tagged_ptr0 src2){ int len=( int)({ struct _tagged_ptr0 _temp30= dest;(
unsigned int)( _temp30.last_plus_one - _temp30.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp21= dest; int* _temp23= _temp21.curr + i;
if( _temp21.base == 0? 1:( _temp23 < _temp21.base? 1: _temp23 >= _temp21.last_plus_one)){
_throw( Null_Exception);}* _temp23=({ struct _tagged_ptr0 _temp24= src1; int*
_temp26= _temp24.curr + i; if( _temp24.base == 0? 1:( _temp26 < _temp24.base? 1:
_temp26 >= _temp24.last_plus_one)){ _throw( Null_Exception);}* _temp26;}) |({
struct _tagged_ptr0 _temp27= src2; int* _temp29= _temp27.curr + i; if( _temp27.base
== 0? 1:( _temp29 < _temp27.base? 1: _temp29 >= _temp27.last_plus_one)){ _throw(
Null_Exception);}* _temp29;});});}} void Cyc_Bitvec_intersect_two( struct
_tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=(
int)({ struct _tagged_ptr0 _temp40= dest;( unsigned int)( _temp40.last_plus_one
- _temp40.curr);}); int i= 0; for( 0; i < len; ++ i){({ struct _tagged_ptr0
_temp31= dest; int* _temp33= _temp31.curr + i; if( _temp31.base == 0? 1:(
_temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one)){ _throw(
Null_Exception);}* _temp33=({ struct _tagged_ptr0 _temp34= src1; int* _temp36=
_temp34.curr + i; if( _temp34.base == 0? 1:( _temp36 < _temp34.base? 1: _temp36
>= _temp34.last_plus_one)){ _throw( Null_Exception);}* _temp36;}) &({ struct
_tagged_ptr0 _temp37= src2; int* _temp39= _temp37.curr + i; if( _temp37.base ==
0? 1:( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one)){ _throw(
Null_Exception);}* _temp39;});});}} void Cyc_Bitvec_diff_two( struct
_tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=(
int)({ struct _tagged_ptr0 _temp50= dest;( unsigned int)( _temp50.last_plus_one
- _temp50.curr);}); int i= 0; for( 0; i < len; ++ i){({ struct _tagged_ptr0
_temp41= dest; int* _temp43= _temp41.curr + i; if( _temp41.base == 0? 1:(
_temp43 < _temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw(
Null_Exception);}* _temp43=({ struct _tagged_ptr0 _temp44= src1; int* _temp46=
_temp44.curr + i; if( _temp44.base == 0? 1:( _temp46 < _temp44.base? 1: _temp46
>= _temp44.last_plus_one)){ _throw( Null_Exception);}* _temp46;}) & ~({ struct
_tagged_ptr0 _temp47= src2; int* _temp49= _temp47.curr + i; if( _temp47.base ==
0? 1:( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one)){ _throw(
Null_Exception);}* _temp49;});});}} int Cyc_Bitvec_compare_two( struct
_tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=( int)({ struct
_tagged_ptr0 _temp57= src1;( unsigned int)( _temp57.last_plus_one - _temp57.curr);});{
int i= 0; for( 0; i < len; ++ i){ if(({ struct _tagged_ptr0 _temp51= src1; int*
_temp53= _temp51.curr + i; if( _temp51.base == 0? 1:( _temp53 < _temp51.base? 1:
_temp53 >= _temp51.last_plus_one)){ _throw( Null_Exception);}* _temp53;}) !=({
struct _tagged_ptr0 _temp54= src2; int* _temp56= _temp54.curr + i; if( _temp54.base
== 0? 1:( _temp56 < _temp54.base? 1: _temp56 >= _temp54.last_plus_one)){ _throw(
Null_Exception);}* _temp56;})){ return 0;}}} return 1;} struct _tagged_ptr0 Cyc_Bitvec_new_empty(
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
_temp71=( unsigned int)( sz % 32 + 1); int* _temp72=( int*) GC_malloc_atomic(
sizeof( int) * _temp71); struct _tagged_ptr0 _temp75={ _temp72, _temp72, _temp72
+ _temp71};{ unsigned int _temp73= _temp71; unsigned int i; for( i= 0; i <
_temp73; i ++){ _temp72[ i]= 0;}}; _temp75;}); for( 0; l != 0; l=({ struct Cyc_List_List*
_temp69= l; if( _temp69 == 0){ _throw( Null_Exception);} _temp69->tl;})){ Cyc_Bitvec_set(
ans, f((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d,(
void*)({ struct Cyc_List_List* _temp70= l; if( _temp70 == 0){ _throw(
Null_Exception);} _temp70->hd;}))));} return ans;} struct Cyc_List_List* Cyc_Bitvec_to_sorted_list(
struct _tagged_ptr0 bvec, int sz){ struct Cyc_List_List* ans= 0;{ int pos= sz -
1; for( 0; pos >= 0; 0){ int word= pos >> 5; int bits=({ struct _tagged_ptr0
_temp78= bvec; int* _temp80= _temp78.curr + word; if( _temp78.base == 0? 1:(
_temp80 < _temp78.base? 1: _temp80 >= _temp78.last_plus_one)){ _throw(
Null_Exception);}* _temp80;}); int offset= pos & 31; for( 0; offset >= 0; --
offset, -- pos){ if(( bits >> offset & 1) == 1){ ans=({ struct Cyc_List_List*
_temp76=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp76[ 0]=({ struct Cyc_List_List _temp77; _temp77.hd=( void*) pos; _temp77.tl=
ans; _temp77;}); _temp76;});}}}} return ans;} void Cyc_Bitvec_clear_all( struct
_tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp84= bvec;(
unsigned int)( _temp84.last_plus_one - _temp84.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp81= bvec; int* _temp83= _temp81.curr + i;
if( _temp81.base == 0? 1:( _temp83 < _temp81.base? 1: _temp83 >= _temp81.last_plus_one)){
_throw( Null_Exception);}* _temp83= 0;});}} void Cyc_Bitvec_set_all( struct
_tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp88= bvec;(
unsigned int)( _temp88.last_plus_one - _temp88.curr);}); int i= 0; for( 0; i <
len; ++ i){({ struct _tagged_ptr0 _temp85= bvec; int* _temp87= _temp85.curr + i;
if( _temp85.base == 0? 1:( _temp87 < _temp85.base? 1: _temp87 >= _temp85.last_plus_one)){
_throw( Null_Exception);}* _temp87= -1;});}} int Cyc_Bitvec_all_set( struct
_tagged_ptr0 bvec, int sz){ int words= sz >> 5;{ int i= 0; for( 0; i < words; i
++){ if(({ struct _tagged_ptr0 _temp89= bvec; int* _temp91= _temp89.curr + i;
if( _temp89.base == 0? 1:( _temp91 < _temp89.base? 1: _temp91 >= _temp89.last_plus_one)){
_throw( Null_Exception);}* _temp91;}) != -1){ return 0;}}}{ int i= words * 32;
for( 0; i < sz; i ++){ if( ! Cyc_Bitvec_get( bvec, i)){ return 0;}}} return 1;}
void Cyc_Bitvec_print_bvec( struct _tagged_ptr0 bvec){{ int i= 0; for( 0; i < 32
*({ struct _tagged_ptr0 _temp92= bvec;( unsigned int)( _temp92.last_plus_one -
_temp92.curr);}); ++ i){ printf("%d", Cyc_Bitvec_get( bvec, i)? 1: 0);}} printf("\n");}