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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; extern void* Cyc_List_assoc_cmp( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); struct Cyc_Hashtable_Table;
typedef struct Cyc_Hashtable_Table* Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); extern void Cyc_Hashtable_resize( struct Cyc_Hashtable_Table* t); extern
void Cyc_Hashtable_remove( struct Cyc_Hashtable_Table* t, void* key); extern int
Cyc_Hashtable_hash_string( struct _tagged_string s); extern int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s); extern void Cyc_Hashtable_iter( void(* f)( void*,
void*), struct Cyc_Hashtable_Table* t); extern void Cyc_Hashtable_print_table_map(
struct Cyc_Hashtable_Table* t, void(* prn_key)( void*), void(* prn_val)( void*));
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; struct _tagged_ptr0{ struct Cyc_List_List**
curr; struct Cyc_List_List** base; struct Cyc_List_List** last_plus_one; } ;
struct Cyc_Hashtable_Table{ int(* cmp)( void*, void*); int(* hash)( void*); int
max_len; struct _tagged_ptr0 tab; } ; struct Cyc_Hashtable_Table* Cyc_Hashtable_create(
int sz, int(* cmp)( void*, void*), int(* hash)( void*)){ struct Cyc_List_List*
mt= 0; return({ struct Cyc_Hashtable_Table* _temp0=( struct Cyc_Hashtable_Table*)
GC_malloc( sizeof( struct Cyc_Hashtable_Table)); _temp0->cmp= cmp; _temp0->hash=
hash; _temp0->max_len= 3; _temp0->tab=({ unsigned int _temp1=( unsigned int) sz;
struct Cyc_List_List** _temp2=( struct Cyc_List_List**) GC_malloc( sizeof(
struct Cyc_List_List*) * _temp1); struct _tagged_ptr0 _temp5={ _temp2, _temp2,
_temp2 + _temp1};{ unsigned int _temp3= _temp1; unsigned int i; for( i= 0; i <
_temp3; i ++){ _temp2[ i]= mt;}}; _temp5;}); _temp0;});} struct _tuple0{ void*
f1; void* f2; } ; void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void*
key, void* val){ struct _tagged_ptr0 tab= t->tab; int bucket=( int)((* t->hash)(
key) %({ struct _tagged_ptr0 _temp17= tab;( unsigned int)( _temp17.last_plus_one
- _temp17.curr);}));({ struct _tagged_ptr0 _temp6= tab; struct Cyc_List_List**
_temp8= _temp6.curr + bucket; if( _temp6.base == 0? 1:( _temp8 < _temp6.base? 1:
_temp8 >= _temp6.last_plus_one)){ _throw( Null_Exception);}* _temp8=({ struct
Cyc_List_List* _temp9=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp9->hd=( void*)({ struct _tuple0* _temp13=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp13->f1= key; _temp13->f2= val; _temp13;}); _temp9->tl=({
struct _tagged_ptr0 _temp10= tab; struct Cyc_List_List** _temp12= _temp10.curr +
bucket; if( _temp10.base == 0? 1:( _temp12 < _temp10.base? 1: _temp12 >= _temp10.last_plus_one)){
_throw( Null_Exception);}* _temp12;}); _temp9;});}); if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(({ struct _tagged_ptr0 _temp14= tab; struct Cyc_List_List**
_temp16= _temp14.curr + bucket; if( _temp14.base == 0? 1:( _temp16 < _temp14.base?
1: _temp16 >= _temp14.last_plus_one)){ _throw( Null_Exception);}* _temp16;})) >
t->max_len){(( void(*)( struct Cyc_Hashtable_Table* t)) Cyc_Hashtable_resize)( t);}}
void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void* key){ struct
_tagged_ptr0 tab= t->tab; struct Cyc_List_List* l=({ struct _tagged_ptr0 _temp19=
tab; struct Cyc_List_List** _temp21= _temp19.curr +( int)((* t->hash)( key) %({
struct _tagged_ptr0 _temp18= tab;( unsigned int)( _temp18.last_plus_one -
_temp18.curr);})); if( _temp19.base == 0? 1:( _temp21 < _temp19.base? 1: _temp21
>= _temp19.last_plus_one)){ _throw( Null_Exception);}* _temp21;}); return(( void*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x)) Cyc_List_assoc_cmp)(
t->cmp, l, key);} void Cyc_Hashtable_remove( struct Cyc_Hashtable_Table* t, void*
key){ struct _tagged_ptr0 tab= t->tab; int(* cmp)( void*, void*)= t->cmp; int
bucket=( int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp40= tab;(
unsigned int)( _temp40.last_plus_one - _temp40.curr);})); struct Cyc_List_List*
l=({ struct _tagged_ptr0 _temp37= tab; struct Cyc_List_List** _temp39= _temp37.curr
+ bucket; if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1: _temp39 >=
_temp37.last_plus_one)){ _throw( Null_Exception);}* _temp39;}); if( l == 0){
return;} if((* cmp)( key,({ struct _tuple0* _temp23=( struct _tuple0*)({ struct
Cyc_List_List* _temp22= l; if( _temp22 == 0){ _throw( Null_Exception);} _temp22->hd;});
unsigned int _temp24= 0; if( _temp24 >= 1u){ _throw( Null_Exception);} _temp23[
_temp24];}).f1) == 0){({ struct _tagged_ptr0 _temp25= tab; struct Cyc_List_List**
_temp27= _temp25.curr + bucket; if( _temp25.base == 0? 1:( _temp27 < _temp25.base?
1: _temp27 >= _temp25.last_plus_one)){ _throw( Null_Exception);}* _temp27=({
struct Cyc_List_List* _temp28= l; if( _temp28 == 0){ _throw( Null_Exception);}
_temp28->tl;});}); return;}{ struct Cyc_List_List* prev= l; l=({ struct Cyc_List_List*
_temp29= l; if( _temp29 == 0){ _throw( Null_Exception);} _temp29->tl;}); for( 0;({
struct Cyc_List_List* _temp30= l; if( _temp30 == 0){ _throw( Null_Exception);}
_temp30->tl;}) != 0; prev= l, l=({ struct Cyc_List_List* _temp31= l; if( _temp31
== 0){ _throw( Null_Exception);} _temp31->tl;})){ if((* cmp)( key,({ struct
_tuple0* _temp33=( struct _tuple0*)({ struct Cyc_List_List* _temp32= l; if(
_temp32 == 0){ _throw( Null_Exception);} _temp32->hd;}); unsigned int _temp34= 0;
if( _temp34 >= 1u){ _throw( Null_Exception);} _temp33[ _temp34];}).f1) == 0){({
struct Cyc_List_List* _temp35= prev; if( _temp35 == 0){ _throw( Null_Exception);}
_temp35->tl=({ struct Cyc_List_List* _temp36= l; if( _temp36 == 0){ _throw(
Null_Exception);} _temp36->tl;});}); return;}}}} int Cyc_Hashtable_hash_string(
struct _tagged_string s){ int ans= 0; int sz=( int)({ struct _tagged_string
_temp44= s;( unsigned int)( _temp44.last_plus_one - _temp44.curr);}); int shift=
0;{ int i= 0; for( 0; i < sz; ++ i){ ans= ans ^({ struct _tagged_string _temp41=
s; char* _temp43= _temp41.curr + i; if( _temp41.base == 0? 1:( _temp43 < _temp41.base?
1: _temp43 >= _temp41.last_plus_one)){ _throw( Null_Exception);}* _temp43;}) <<
shift; shift += 8; if( shift == 32){ shift= 0;}}} return ans;} int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s){ return Cyc_Hashtable_hash_string(* s);} void Cyc_Hashtable_insert_bucket(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems){ if(
elems == 0){ return;}(( void(*)( struct _tagged_ptr0 tab, int(* hash)( void*),
struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)( tab, hash,({ struct
Cyc_List_List* _temp45= elems; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->tl;}));{ void* key=({ struct _tuple0* _temp59=( struct _tuple0*)({
struct Cyc_List_List* _temp58= elems; if( _temp58 == 0){ _throw( Null_Exception);}
_temp58->hd;}); unsigned int _temp60= 0; if( _temp60 >= 1u){ _throw(
Null_Exception);} _temp59[ _temp60];}).f1; void* val=({ struct _tuple0* _temp56=(
struct _tuple0*)({ struct Cyc_List_List* _temp55= elems; if( _temp55 == 0){
_throw( Null_Exception);} _temp55->hd;}); unsigned int _temp57= 0; if( _temp57
>= 1u){ _throw( Null_Exception);} _temp56[ _temp57];}).f2; int nidx=( int)((*
hash)( key) %({ struct _tagged_ptr0 _temp54= tab;( unsigned int)( _temp54.last_plus_one
- _temp54.curr);}));({ struct _tagged_ptr0 _temp46= tab; struct Cyc_List_List**
_temp48= _temp46.curr + nidx; if( _temp46.base == 0? 1:( _temp48 < _temp46.base?
1: _temp48 >= _temp46.last_plus_one)){ _throw( Null_Exception);}* _temp48=({
struct Cyc_List_List* _temp49=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp49->hd=( void*)({ struct _tuple0* _temp53=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp53->f1= key; _temp53->f2= val; _temp53;});
_temp49->tl=({ struct _tagged_ptr0 _temp50= tab; struct Cyc_List_List** _temp52=
_temp50.curr + nidx; if( _temp50.base == 0? 1:( _temp52 < _temp50.base? 1:
_temp52 >= _temp50.last_plus_one)){ _throw( Null_Exception);}* _temp52;});
_temp49;});});}} void Cyc_Hashtable_resize( struct Cyc_Hashtable_Table* t){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp69= odata;( unsigned int)( _temp69.last_plus_one - _temp69.curr);}); int
nsize= 2 * osize + 1; struct Cyc_List_List* mt= 0; struct _tagged_ptr0 ndata=({
unsigned int _temp64=( unsigned int) nsize; struct Cyc_List_List** _temp65=(
struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp64);
struct _tagged_ptr0 _temp68={ _temp65, _temp65, _temp65 + _temp64};{
unsigned int _temp66= _temp64; unsigned int i; for( i= 0; i < _temp66; i ++){
_temp65[ i]= mt;}}; _temp68;});{ int i= 0; for( 0; i < osize; i ++){(( void(*)(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)(
ndata, t->hash,({ struct _tagged_ptr0 _temp61= odata; struct Cyc_List_List**
_temp63= _temp61.curr + i; if( _temp61.base == 0? 1:( _temp63 < _temp61.base? 1:
_temp63 >= _temp61.last_plus_one)){ _throw( Null_Exception);}* _temp63;}));}} t->tab=
ndata; t->max_len= 2 * t->max_len;} void Cyc_Hashtable_iter( void(* f)( void*,
void*), struct Cyc_Hashtable_Table* t){ struct _tagged_ptr0 odata= t->tab; int
osize=( int)({ struct _tagged_ptr0 _temp80= odata;( unsigned int)( _temp80.last_plus_one
- _temp80.curr);}); int i= 0; for( 0; i < osize; i ++){ struct Cyc_List_List*
iter=({ struct _tagged_ptr0 _temp77= odata; struct Cyc_List_List** _temp79=
_temp77.curr + i; if( _temp77.base == 0? 1:( _temp79 < _temp77.base? 1: _temp79
>= _temp77.last_plus_one)){ _throw( Null_Exception);}* _temp79;}); for( 0; iter
!= 0; iter=({ struct Cyc_List_List* _temp70= iter; if( _temp70 == 0){ _throw(
Null_Exception);} _temp70->tl;})){ f(({ struct _tuple0* _temp72=( struct _tuple0*)({
struct Cyc_List_List* _temp71= iter; if( _temp71 == 0){ _throw( Null_Exception);}
_temp71->hd;}); unsigned int _temp73= 0; if( _temp73 >= 1u){ _throw(
Null_Exception);} _temp72[ _temp73];}).f1,({ struct _tuple0* _temp75=( struct
_tuple0*)({ struct Cyc_List_List* _temp74= iter; if( _temp74 == 0){ _throw(
Null_Exception);} _temp74->hd;}); unsigned int _temp76= 0; if( _temp76 >= 1u){
_throw( Null_Exception);} _temp75[ _temp76];}).f2);}}} void Cyc_Hashtable_print_table_map(
struct Cyc_Hashtable_Table* t, void(* prn_key)( void*), void(* prn_val)( void*)){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp91= odata;( unsigned int)( _temp91.last_plus_one - _temp91.curr);}); int i=
0; for( 0; i < osize; i ++){ printf("%d: ", i);{ struct Cyc_List_List* iter=({
struct _tagged_ptr0 _temp88= odata; struct Cyc_List_List** _temp90= _temp88.curr
+ i; if( _temp88.base == 0? 1:( _temp90 < _temp88.base? 1: _temp90 >= _temp88.last_plus_one)){
_throw( Null_Exception);}* _temp90;}); for( 0; iter != 0; iter=({ struct Cyc_List_List*
_temp81= iter; if( _temp81 == 0){ _throw( Null_Exception);} _temp81->tl;})){
printf("("); prn_key(({ struct _tuple0* _temp83=( struct _tuple0*)({ struct Cyc_List_List*
_temp82= iter; if( _temp82 == 0){ _throw( Null_Exception);} _temp82->hd;});
unsigned int _temp84= 0; if( _temp84 >= 1u){ _throw( Null_Exception);} _temp83[
_temp84];}).f1); printf(","); prn_val(({ struct _tuple0* _temp86=( struct
_tuple0*)({ struct Cyc_List_List* _temp85= iter; if( _temp85 == 0){ _throw(
Null_Exception);} _temp85->hd;}); unsigned int _temp87= 0; if( _temp87 >= 1u){
_throw( Null_Exception);} _temp86[ _temp87];}).f2); printf(") ");}} printf("\n");}}