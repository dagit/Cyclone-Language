#include "cyc_include.h"

 struct _tagged_ptr0{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple0{ void* f1; void* f2; }
; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
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
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern void* Cyc_List_assoc_cmp( int(* compare)( void*, void*),
struct Cyc_List_List* l, void* x); struct Cyc_Hashtable_Table; typedef struct
Cyc_Hashtable_Table* Cyc_Hashtable_table_t; extern struct Cyc_Hashtable_Table*
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
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_Hashtable_Table{ int(* cmp)( void*, void*); int(* hash)(
void*); int max_len; struct _tagged_ptr0 tab; } ; struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*)){
struct Cyc_List_List* mt= 0; return({ struct Cyc_Hashtable_Table* _temp0=(
struct Cyc_Hashtable_Table*) GC_malloc( sizeof( struct Cyc_Hashtable_Table));
_temp0->cmp= cmp; _temp0->hash= hash; _temp0->max_len= 3; _temp0->tab=({
unsigned int _temp1=( unsigned int) sz; struct Cyc_List_List** _temp2=( struct
Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp1); struct
_tagged_ptr0 _temp5={ _temp2, _temp2, _temp2 + _temp1};{ unsigned int _temp3=
_temp1; unsigned int i; for( i= 0; i < _temp3; i ++){ _temp2[ i]= mt;}}; _temp5;});
_temp0;});} void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key,
void* val){ struct _tagged_ptr0 tab= t->tab; int bucket=( int)((* t->hash)( key)
%({ struct _tagged_ptr0 _temp6= tab;( unsigned int)( _temp6.last_plus_one -
_temp6.curr);}));({ struct _tagged_ptr0 _temp7= tab; struct Cyc_List_List**
_temp9= _temp7.curr + bucket; if( _temp7.base == 0? 1:( _temp9 < _temp7.base? 1:
_temp9 >= _temp7.last_plus_one)){ _throw( Null_Exception);}* _temp9=({ struct
Cyc_List_List* _temp10=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp10->hd=( void*)({ struct _tuple0* _temp14=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp14->f1= key; _temp14->f2= val; _temp14;});
_temp10->tl=({ struct _tagged_ptr0 _temp11= tab; struct Cyc_List_List** _temp13=
_temp11.curr + bucket; if( _temp11.base == 0? 1:( _temp13 < _temp11.base? 1:
_temp13 >= _temp11.last_plus_one)){ _throw( Null_Exception);}* _temp13;});
_temp10;});}); if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(({
struct _tagged_ptr0 _temp15= tab; struct Cyc_List_List** _temp17= _temp15.curr +
bucket; if( _temp15.base == 0? 1:( _temp17 < _temp15.base? 1: _temp17 >= _temp15.last_plus_one)){
_throw( Null_Exception);}* _temp17;})) > t->max_len){(( void(*)( struct Cyc_Hashtable_Table*
t)) Cyc_Hashtable_resize)( t);}} void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table*
t, void* key){ struct _tagged_ptr0 tab= t->tab; struct Cyc_List_List* l=({
struct _tagged_ptr0 _temp19= tab; struct Cyc_List_List** _temp21= _temp19.curr +(
int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp18= tab;( unsigned int)(
_temp18.last_plus_one - _temp18.curr);})); if( _temp19.base == 0? 1:( _temp21 <
_temp19.base? 1: _temp21 >= _temp19.last_plus_one)){ _throw( Null_Exception);}*
_temp21;}); return(( void*(*)( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x)) Cyc_List_assoc_cmp)( t->cmp, l, key);} void Cyc_Hashtable_remove(
struct Cyc_Hashtable_Table* t, void* key){ struct _tagged_ptr0 tab= t->tab; int(*
cmp)( void*, void*)= t->cmp; int bucket=( int)((* t->hash)( key) %({ struct
_tagged_ptr0 _temp22= tab;( unsigned int)( _temp22.last_plus_one - _temp22.curr);}));
struct Cyc_List_List* l=({ struct _tagged_ptr0 _temp23= tab; struct Cyc_List_List**
_temp25= _temp23.curr + bucket; if( _temp23.base == 0? 1:( _temp25 < _temp23.base?
1: _temp25 >= _temp23.last_plus_one)){ _throw( Null_Exception);}* _temp25;});
if( l == 0){ return;} if((* cmp)( key,({ struct _tuple0* _temp27=( struct
_tuple0*)({ struct Cyc_List_List* _temp26= l; if( _temp26 == 0){ _throw(
Null_Exception);} _temp26->hd;}); unsigned int _temp28= 0; if( _temp28 >= 1u){
_throw( Null_Exception);} _temp27[ _temp28];}).f1) == 0){({ struct _tagged_ptr0
_temp29= tab; struct Cyc_List_List** _temp31= _temp29.curr + bucket; if( _temp29.base
== 0? 1:( _temp31 < _temp29.base? 1: _temp31 >= _temp29.last_plus_one)){ _throw(
Null_Exception);}* _temp31=({ struct Cyc_List_List* _temp32= l; if( _temp32 == 0){
_throw( Null_Exception);} _temp32->tl;});}); return;}{ struct Cyc_List_List*
prev= l; l=({ struct Cyc_List_List* _temp33= l; if( _temp33 == 0){ _throw(
Null_Exception);} _temp33->tl;}); for( 0;({ struct Cyc_List_List* _temp34= l;
if( _temp34 == 0){ _throw( Null_Exception);} _temp34->tl;}) != 0; prev= l, l=({
struct Cyc_List_List* _temp35= l; if( _temp35 == 0){ _throw( Null_Exception);}
_temp35->tl;})){ if((* cmp)( key,({ struct _tuple0* _temp37=( struct _tuple0*)({
struct Cyc_List_List* _temp36= l; if( _temp36 == 0){ _throw( Null_Exception);}
_temp36->hd;}); unsigned int _temp38= 0; if( _temp38 >= 1u){ _throw(
Null_Exception);} _temp37[ _temp38];}).f1) == 0){({ struct Cyc_List_List*
_temp39= prev; if( _temp39 == 0){ _throw( Null_Exception);} _temp39->tl=({
struct Cyc_List_List* _temp40= l; if( _temp40 == 0){ _throw( Null_Exception);}
_temp40->tl;});}); return;}}}} int Cyc_Hashtable_hash_string( struct
_tagged_string s){ int ans= 0; int sz=( int)({ struct _tagged_string _temp41= s;(
unsigned int)( _temp41.last_plus_one - _temp41.curr);}); int shift= 0;{ int i= 0;
for( 0; i < sz; ++ i){ ans= ans ^({ struct _tagged_string _temp42= s; char*
_temp44= _temp42.curr + i; if( _temp42.base == 0? 1:( _temp44 < _temp42.base? 1:
_temp44 >= _temp42.last_plus_one)){ _throw( Null_Exception);}* _temp44;}) <<
shift; shift += 8; if( shift == 32){ shift= 0;}}} return ans;} int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s){ return Cyc_Hashtable_hash_string(* s);} void Cyc_Hashtable_insert_bucket(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems){ if(
elems == 0){ return;}(( void(*)( struct _tagged_ptr0 tab, int(* hash)( void*),
struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)( tab, hash,({ struct
Cyc_List_List* _temp45= elems; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->tl;}));{ void* key=({ struct _tuple0* _temp47=( struct _tuple0*)({
struct Cyc_List_List* _temp46= elems; if( _temp46 == 0){ _throw( Null_Exception);}
_temp46->hd;}); unsigned int _temp48= 0; if( _temp48 >= 1u){ _throw(
Null_Exception);} _temp47[ _temp48];}).f1; void* val=({ struct _tuple0* _temp50=(
struct _tuple0*)({ struct Cyc_List_List* _temp49= elems; if( _temp49 == 0){
_throw( Null_Exception);} _temp49->hd;}); unsigned int _temp51= 0; if( _temp51
>= 1u){ _throw( Null_Exception);} _temp50[ _temp51];}).f2; int nidx=( int)((*
hash)( key) %({ struct _tagged_ptr0 _temp52= tab;( unsigned int)( _temp52.last_plus_one
- _temp52.curr);}));({ struct _tagged_ptr0 _temp53= tab; struct Cyc_List_List**
_temp55= _temp53.curr + nidx; if( _temp53.base == 0? 1:( _temp55 < _temp53.base?
1: _temp55 >= _temp53.last_plus_one)){ _throw( Null_Exception);}* _temp55=({
struct Cyc_List_List* _temp56=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp56->hd=( void*)({ struct _tuple0* _temp60=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp60->f1= key; _temp60->f2= val; _temp60;});
_temp56->tl=({ struct _tagged_ptr0 _temp57= tab; struct Cyc_List_List** _temp59=
_temp57.curr + nidx; if( _temp57.base == 0? 1:( _temp59 < _temp57.base? 1:
_temp59 >= _temp57.last_plus_one)){ _throw( Null_Exception);}* _temp59;});
_temp56;});});}} void Cyc_Hashtable_resize( struct Cyc_Hashtable_Table* t){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp61= odata;( unsigned int)( _temp61.last_plus_one - _temp61.curr);}); int
nsize= 2 * osize + 1; struct Cyc_List_List* mt= 0; struct _tagged_ptr0 ndata=({
unsigned int _temp62=( unsigned int) nsize; struct Cyc_List_List** _temp63=(
struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp62);
struct _tagged_ptr0 _temp66={ _temp63, _temp63, _temp63 + _temp62};{
unsigned int _temp64= _temp62; unsigned int i; for( i= 0; i < _temp64; i ++){
_temp63[ i]= mt;}}; _temp66;});{ int i= 0; for( 0; i < osize; i ++){(( void(*)(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)(
ndata, t->hash,({ struct _tagged_ptr0 _temp67= odata; struct Cyc_List_List**
_temp69= _temp67.curr + i; if( _temp67.base == 0? 1:( _temp69 < _temp67.base? 1:
_temp69 >= _temp67.last_plus_one)){ _throw( Null_Exception);}* _temp69;}));}} t->tab=
ndata; t->max_len= 2 * t->max_len;} void Cyc_Hashtable_iter( void(* f)( void*,
void*), struct Cyc_Hashtable_Table* t){ struct _tagged_ptr0 odata= t->tab; int
osize=( int)({ struct _tagged_ptr0 _temp70= odata;( unsigned int)( _temp70.last_plus_one
- _temp70.curr);}); int i= 0; for( 0; i < osize; i ++){ struct Cyc_List_List*
iter=({ struct _tagged_ptr0 _temp71= odata; struct Cyc_List_List** _temp73=
_temp71.curr + i; if( _temp71.base == 0? 1:( _temp73 < _temp71.base? 1: _temp73
>= _temp71.last_plus_one)){ _throw( Null_Exception);}* _temp73;}); for( 0; iter
!= 0; iter=({ struct Cyc_List_List* _temp74= iter; if( _temp74 == 0){ _throw(
Null_Exception);} _temp74->tl;})){ f(({ struct _tuple0* _temp76=( struct _tuple0*)({
struct Cyc_List_List* _temp75= iter; if( _temp75 == 0){ _throw( Null_Exception);}
_temp75->hd;}); unsigned int _temp77= 0; if( _temp77 >= 1u){ _throw(
Null_Exception);} _temp76[ _temp77];}).f1,({ struct _tuple0* _temp79=( struct
_tuple0*)({ struct Cyc_List_List* _temp78= iter; if( _temp78 == 0){ _throw(
Null_Exception);} _temp78->hd;}); unsigned int _temp80= 0; if( _temp80 >= 1u){
_throw( Null_Exception);} _temp79[ _temp80];}).f2);}}} void Cyc_Hashtable_print_table_map(
struct Cyc_Hashtable_Table* t, void(* prn_key)( void*), void(* prn_val)( void*)){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp81= odata;( unsigned int)( _temp81.last_plus_one - _temp81.curr);}); int i=
0; for( 0; i < osize; i ++){ printf("%d: ", i);{ struct Cyc_List_List* iter=({
struct _tagged_ptr0 _temp82= odata; struct Cyc_List_List** _temp84= _temp82.curr
+ i; if( _temp82.base == 0? 1:( _temp84 < _temp82.base? 1: _temp84 >= _temp82.last_plus_one)){
_throw( Null_Exception);}* _temp84;}); for( 0; iter != 0; iter=({ struct Cyc_List_List*
_temp85= iter; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl;})){
printf("("); prn_key(({ struct _tuple0* _temp87=( struct _tuple0*)({ struct Cyc_List_List*
_temp86= iter; if( _temp86 == 0){ _throw( Null_Exception);} _temp86->hd;});
unsigned int _temp88= 0; if( _temp88 >= 1u){ _throw( Null_Exception);} _temp87[
_temp88];}).f1); printf(","); prn_val(({ struct _tuple0* _temp90=( struct
_tuple0*)({ struct Cyc_List_List* _temp89= iter; if( _temp89 == 0){ _throw(
Null_Exception);} _temp89->hd;}); unsigned int _temp91= 0; if( _temp91 >= 1u){
_throw( Null_Exception);} _temp90[ _temp91];}).f2); printf(") ");}} printf("\n");}}