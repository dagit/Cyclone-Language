#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* key); struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; typedef struct _tagged_ptr0 Cyc_Bitvec_gbitvec_t; typedef
struct _tagged_ptr0 Cyc_Bitvec_bitvec_t; extern int Cyc_Bitvec_get( struct
_tagged_ptr0, int); extern void Cyc_Bitvec_set( struct _tagged_ptr0, int);
extern void Cyc_Bitvec_clear( struct _tagged_ptr0, int); extern int Cyc_Bitvec_get_and_set(
struct _tagged_ptr0, int); extern void Cyc_Bitvec_union_two( struct _tagged_ptr0
dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2); extern void Cyc_Bitvec_intersect_two(
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
int word= pos >> 5; int offset= pos & 31; return(*(( int*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( bvec,
sizeof( int), word) >> offset & 1) == 1;} void Cyc_Bitvec_set( struct
_tagged_ptr0 bvec, int pos){ int word= pos >> 5; int offset= pos & 31;*(( int*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
bvec, sizeof( int), word)=*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( bvec, sizeof( int), word) | 1 <<
offset;} void Cyc_Bitvec_clear( struct _tagged_ptr0 bvec, int pos){ int word=
pos >> 5; int offset= pos & 31;*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( bvec, sizeof( int), word)=*(( int*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
bvec, sizeof( int), word) & ~( 1 << offset);} int Cyc_Bitvec_get_and_set( struct
_tagged_ptr0 bvec, int pos){ int word= pos >> 5; int offset= pos & 31; int slot=*((
int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), word); int ans=( slot >> offset &
1) == 1; if( ! ans){*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), word)= slot | 1 << offset;}
return ans;} void Cyc_Bitvec_union_two( struct _tagged_ptr0 dest, struct
_tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=( int)({ struct
_tagged_ptr0 _temp0= dest;( unsigned int)( _temp0.last_plus_one - _temp0.curr);});
int i= 0; for( 0; i < len; ++ i){*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( dest, sizeof( int), i)=*(( int*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
src1, sizeof( int), i) |*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( src2, sizeof( int), i);}} void Cyc_Bitvec_intersect_two(
struct _tagged_ptr0 dest, struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){
int len=( int)({ struct _tagged_ptr0 _temp1= dest;( unsigned int)( _temp1.last_plus_one
- _temp1.curr);}); int i= 0; for( 0; i < len; ++ i){*(( int*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( dest,
sizeof( int), i)=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( src1, sizeof( int), i) &*(( int*(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( src2,
sizeof( int), i);}} void Cyc_Bitvec_diff_two( struct _tagged_ptr0 dest, struct
_tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=( int)({ struct
_tagged_ptr0 _temp2= dest;( unsigned int)( _temp2.last_plus_one - _temp2.curr);});
int i= 0; for( 0; i < len; ++ i){*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( dest, sizeof( int), i)=*(( int*(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)(
src1, sizeof( int), i) & ~(*(( int*(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( src2, sizeof( int), i));}} int Cyc_Bitvec_compare_two(
struct _tagged_ptr0 src1, struct _tagged_ptr0 src2){ int len=( int)({ struct
_tagged_ptr0 _temp3= src1;( unsigned int)( _temp3.last_plus_one - _temp3.curr);});{
int i= 0; for( 0; i < len; ++ i){ if(*(( int*(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( src1, sizeof( int), i)
!=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( src2, sizeof( int), i)){ return 0;}}} return 1;}
struct _tagged_ptr0 Cyc_Bitvec_new_empty( int sz){ struct _tagged_ptr0 ans=({
unsigned int _temp4=( unsigned int)( sz / 32 + 1); int* _temp5=( int*)
GC_malloc_atomic( sizeof( int) * _temp4); struct _tagged_ptr0 _temp7={ _temp5,
_temp5, _temp5 + _temp4};{ unsigned int _temp6= _temp4; unsigned int i; for( i=
0; i < _temp6; i ++){ _temp5[ i]= 0;}}; _temp7;}); return ans;} struct
_tagged_ptr0 Cyc_Bitvec_new_full( int sz){ struct _tagged_ptr0 ans=({
unsigned int _temp8=( unsigned int)( sz / 32 + 1); int* _temp9=( int*)
GC_malloc_atomic( sizeof( int) * _temp8); struct _tagged_ptr0 _temp11={ _temp9,
_temp9, _temp9 + _temp8};{ unsigned int _temp10= _temp8; unsigned int i; for( i=
0; i < _temp10; i ++){ _temp9[ i]= -1;}}; _temp11;}); return ans;} struct
_tagged_ptr0 Cyc_Bitvec_new_copy( struct _tagged_ptr0 old){ struct _tagged_ptr0
copy= Cyc_Bitvec_new_empty(( int)({ struct _tagged_ptr0 _temp12= old;(
unsigned int)( _temp12.last_plus_one - _temp12.curr);})); Cyc_Bitvec_union_two(
copy, copy, old); return copy;} struct _tagged_ptr0 Cyc_Bitvec_from_list( struct
Cyc_Dict_Dict* d, int(* f)( void*), int sz, struct Cyc_List_List* l){ struct
_tagged_ptr0 ans=({ unsigned int _temp13=( unsigned int)( sz % 32 + 1); int*
_temp14=( int*) GC_malloc_atomic( sizeof( int) * _temp13); struct _tagged_ptr0
_temp16={ _temp14, _temp14, _temp14 + _temp13};{ unsigned int _temp15= _temp13;
unsigned int i; for( i= 0; i < _temp15; i ++){ _temp14[ i]= 0;}}; _temp16;});
for( 0; l != 0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ Cyc_Bitvec_set(
ans, f((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d,(
void*)(( struct Cyc_List_List*) _check_null( l))->hd)));} return ans;} struct
Cyc_List_List* Cyc_Bitvec_to_sorted_list( struct _tagged_ptr0 bvec, int sz){
struct Cyc_List_List* ans= 0;{ int pos= sz - 1; for( 0; pos >= 0; 0){ int word=
pos >> 5; int bits=*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), word); int offset= pos & 31; for(
0; offset >= 0;( -- offset, -- pos)){ if(( bits >> offset & 1) == 1){ ans=({
struct Cyc_List_List* _temp17=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List) * 1); _temp17[ 0]=({ struct Cyc_List_List _temp18; _temp18.hd=(
void*) pos; _temp18.tl= ans; _temp18;}); _temp17;});}}}} return ans;} void Cyc_Bitvec_clear_all(
struct _tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp19= bvec;(
unsigned int)( _temp19.last_plus_one - _temp19.curr);}); int i= 0; for( 0; i <
len; ++ i){*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), i)= 0;}} void Cyc_Bitvec_set_all(
struct _tagged_ptr0 bvec){ int len=( int)({ struct _tagged_ptr0 _temp20= bvec;(
unsigned int)( _temp20.last_plus_one - _temp20.curr);}); int i= 0; for( 0; i <
len; ++ i){*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), i)= -1;}} int Cyc_Bitvec_all_set(
struct _tagged_ptr0 bvec, int sz){ int words= sz >> 5;{ int i= 0; for( 0; i <
words; i ++){ if(*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( bvec, sizeof( int), i) != -1){ return 0;}}}{ int i=
words * 32; for( 0; i < sz; i ++){ if( ! Cyc_Bitvec_get( bvec, i)){ return 0;}}}
return 1;} void Cyc_Bitvec_print_bvec( struct _tagged_ptr0 bvec){{ int i= 0;
for( 0; i < 32 *({ struct _tagged_ptr0 _temp21= bvec;( unsigned int)( _temp21.last_plus_one
- _temp21.curr);}); ++ i){ printf("%d", Cyc_Bitvec_get( bvec, i)? 1: 0);}}
printf("\n");}