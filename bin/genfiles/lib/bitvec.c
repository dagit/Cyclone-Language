 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict*
d, void* key); extern int Cyc_Bitvec_get( struct _tagged_arr, int); extern void
Cyc_Bitvec_set( struct _tagged_arr, int); extern void Cyc_Bitvec_clear( struct
_tagged_arr, int); extern int Cyc_Bitvec_get_and_set( struct _tagged_arr, int);
extern void Cyc_Bitvec_union_two( struct _tagged_arr dest, struct _tagged_arr
src1, struct _tagged_arr src2); extern void Cyc_Bitvec_intersect_two( struct
_tagged_arr dest, struct _tagged_arr src1, struct _tagged_arr src2); extern void
Cyc_Bitvec_diff_two( struct _tagged_arr dest, struct _tagged_arr src1, struct
_tagged_arr src2); extern int Cyc_Bitvec_compare_two( struct _tagged_arr src1,
struct _tagged_arr src2); extern struct _tagged_arr Cyc_Bitvec_new_empty( int);
extern struct _tagged_arr Cyc_Bitvec_new_full( int); extern struct _tagged_arr
Cyc_Bitvec_new_copy( struct _tagged_arr); extern struct _tagged_arr Cyc_Bitvec_from_list(
struct Cyc_Dict_Dict* d, int(* f)( void*), int sz, struct Cyc_List_List*);
extern struct Cyc_List_List* Cyc_Bitvec_to_sorted_list( struct _tagged_arr bvec,
int sz); extern void Cyc_Bitvec_clear_all( struct _tagged_arr); extern void Cyc_Bitvec_set_all(
struct _tagged_arr); extern int Cyc_Bitvec_all_set( struct _tagged_arr bvec, int
sz); extern void Cyc_Bitvec_print_bvec( struct _tagged_arr bvec); struct Cyc_Stdio___sFILE;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_printf( struct _tagged_arr
fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa= 0; struct Cyc_Stdio_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct
Cyc_Stdio_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa=
3; struct Cyc_Stdio_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Stdio_StringPtr_sa= 4; struct Cyc_Stdio_StringPtr_sa_struct{ int
tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_DoublePtr_sa= 5;
struct Cyc_Stdio_DoublePtr_sa_struct{ int tag; double* f1; } ; static const int
Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{ int tag; float*
f1; } ; int Cyc_Bitvec_get( struct _tagged_arr bvec, int pos){ int word= pos >>
5; int offset= pos & 31; return(*(( int*) _check_unknown_subscript( bvec,
sizeof( int), word)) >> offset & 1) == 1;} void Cyc_Bitvec_set( struct
_tagged_arr bvec, int pos){ int word= pos >> 5; int offset= pos & 31;*(( int*)
_check_unknown_subscript( bvec, sizeof( int), word))=*(( int*)
_check_unknown_subscript( bvec, sizeof( int), word)) | 1 << offset;} void Cyc_Bitvec_clear(
struct _tagged_arr bvec, int pos){ int word= pos >> 5; int offset= pos & 31;*((
int*) _check_unknown_subscript( bvec, sizeof( int), word))=*(( int*)
_check_unknown_subscript( bvec, sizeof( int), word)) & ~( 1 << offset);} int Cyc_Bitvec_get_and_set(
struct _tagged_arr bvec, int pos){ int word= pos >> 5; int offset= pos & 31; int
slot=*(( int*) _check_unknown_subscript( bvec, sizeof( int), word)); int ans=(
slot >> offset & 1) == 1; if( ! ans){*(( int*) _check_unknown_subscript( bvec,
sizeof( int), word))= slot | 1 << offset;} return ans;} void Cyc_Bitvec_union_two(
struct _tagged_arr dest, struct _tagged_arr src1, struct _tagged_arr src2){ int
len=( int) _get_arr_size( dest, sizeof( int)); int i= 0; for( 0; i < len; ++ i){*((
int*) _check_unknown_subscript( dest, sizeof( int), i))=*(( int*)
_check_unknown_subscript( src1, sizeof( int), i)) |*(( int*)
_check_unknown_subscript( src2, sizeof( int), i));}} void Cyc_Bitvec_intersect_two(
struct _tagged_arr dest, struct _tagged_arr src1, struct _tagged_arr src2){ int
len=( int) _get_arr_size( dest, sizeof( int)); int i= 0; for( 0; i < len; ++ i){*((
int*) _check_unknown_subscript( dest, sizeof( int), i))=*(( int*)
_check_unknown_subscript( src1, sizeof( int), i)) &*(( int*)
_check_unknown_subscript( src2, sizeof( int), i));}} void Cyc_Bitvec_diff_two(
struct _tagged_arr dest, struct _tagged_arr src1, struct _tagged_arr src2){ int
len=( int) _get_arr_size( dest, sizeof( int)); int i= 0; for( 0; i < len; ++ i){*((
int*) _check_unknown_subscript( dest, sizeof( int), i))=*(( int*)
_check_unknown_subscript( src1, sizeof( int), i)) & ~(*(( int*)
_check_unknown_subscript( src2, sizeof( int), i)));}} int Cyc_Bitvec_compare_two(
struct _tagged_arr src1, struct _tagged_arr src2){ int len=( int) _get_arr_size(
src1, sizeof( int));{ int i= 0; for( 0; i < len; ++ i){ if(*(( int*)
_check_unknown_subscript( src1, sizeof( int), i)) !=*(( int*)
_check_unknown_subscript( src2, sizeof( int), i))){ return 0;}}} return 1;}
struct _tagged_arr Cyc_Bitvec_new_empty( int sz){ struct _tagged_arr ans=({
unsigned int _temp0=( unsigned int)( sz / 32 + 1); int* _temp1=( int*)
GC_malloc_atomic( sizeof( int) * _temp0); struct _tagged_arr _temp3= _tag_arr(
_temp1, sizeof( int),( unsigned int)( sz / 32 + 1));{ unsigned int _temp2=
_temp0; unsigned int i; for( i= 0; i < _temp2; i ++){ _temp1[ i]= 0;}}; _temp3;});
return ans;} struct _tagged_arr Cyc_Bitvec_new_full( int sz){ struct _tagged_arr
ans=({ unsigned int _temp4=( unsigned int)( sz / 32 + 1); int* _temp5=( int*)
GC_malloc_atomic( sizeof( int) * _temp4); struct _tagged_arr _temp7= _tag_arr(
_temp5, sizeof( int),( unsigned int)( sz / 32 + 1));{ unsigned int _temp6=
_temp4; unsigned int i; for( i= 0; i < _temp6; i ++){ _temp5[ i]= -1;}}; _temp7;});
return ans;} struct _tagged_arr Cyc_Bitvec_new_copy( struct _tagged_arr old){
struct _tagged_arr copy= Cyc_Bitvec_new_empty(( int) _get_arr_size( old, sizeof(
int))); Cyc_Bitvec_union_two( copy, copy, old); return copy;} struct _tagged_arr
Cyc_Bitvec_from_list( struct Cyc_Dict_Dict* d, int(* f)( void*), int sz, struct
Cyc_List_List* l){ struct _tagged_arr ans=({ unsigned int _temp8=( unsigned int)(
sz % 32 + 1); int* _temp9=( int*) GC_malloc_atomic( sizeof( int) * _temp8);
struct _tagged_arr _temp11= _tag_arr( _temp9, sizeof( int),( unsigned int)( sz %
32 + 1));{ unsigned int _temp10= _temp8; unsigned int i; for( i= 0; i < _temp10;
i ++){ _temp9[ i]= 0;}}; _temp11;}); for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ Cyc_Bitvec_set( ans, f( Cyc_Dict_lookup( d,( void*)((
struct Cyc_List_List*) _check_null( l))->hd)));} return ans;} struct Cyc_List_List*
Cyc_Bitvec_to_sorted_list( struct _tagged_arr bvec, int sz){ struct Cyc_List_List*
ans= 0;{ int pos= sz - 1; for( 0; pos >= 0; 0){ int word= pos >> 5; int bits=*((
int*) _check_unknown_subscript( bvec, sizeof( int), word)); int offset= pos & 31;
for( 0; offset >= 0;( -- offset, -- pos)){ if(( bits >> offset & 1) == 1){ ans=({
struct Cyc_List_List* _temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List) * 1); _temp12[ 0]=({ struct Cyc_List_List _temp13; _temp13.hd=(
void*) pos; _temp13.tl= ans; _temp13;}); _temp12;});}}}} return ans;} void Cyc_Bitvec_clear_all(
struct _tagged_arr bvec){ int len=( int) _get_arr_size( bvec, sizeof( int)); int
i= 0; for( 0; i < len; ++ i){*(( int*) _check_unknown_subscript( bvec, sizeof(
int), i))= 0;}} void Cyc_Bitvec_set_all( struct _tagged_arr bvec){ int len=( int)
_get_arr_size( bvec, sizeof( int)); int i= 0; for( 0; i < len; ++ i){*(( int*)
_check_unknown_subscript( bvec, sizeof( int), i))= -1;}} int Cyc_Bitvec_all_set(
struct _tagged_arr bvec, int sz){ int words= sz >> 5;{ int i= 0; for( 0; i <
words; i ++){ if(*(( int*) _check_unknown_subscript( bvec, sizeof( int), i)) !=
-1){ return 0;}}}{ int i= words * 32; for( 0; i < sz; i ++){ if( ! Cyc_Bitvec_get(
bvec, i)){ return 0;}}} return 1;} void Cyc_Bitvec_print_bvec( struct
_tagged_arr bvec){{ int i= 0; for( 0; i < 32 * _get_arr_size( bvec, sizeof( int));
++ i){({ int(* _temp14)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp18= _tag_arr("%d", sizeof( unsigned char), 3u); struct
Cyc_Stdio_Int_pa_struct _temp19; _temp19.tag= Cyc_Stdio_Int_pa; _temp19.f1=( int)((
unsigned int)( Cyc_Bitvec_get( bvec, i)? 1: 0));{ void* _temp17=( void*)&
_temp19; void* _temp15[ 1u]={ _temp17}; struct _tagged_arr _temp16={( void*)
_temp15,( void*) _temp15,( void*)( _temp15 + 1u)}; _temp14( _temp18, _temp16);}});}}({
int(* _temp20)( struct _tagged_arr fmt, struct _tagged_arr)= Cyc_Stdio_printf;
struct _tagged_arr _temp23= _tag_arr("\n", sizeof( unsigned char), 2u); void*
_temp21[ 0u]={}; struct _tagged_arr _temp22={( void*) _temp21,( void*) _temp21,(
void*)( _temp21 + 0u)}; _temp20( _temp23, _temp22);});}
