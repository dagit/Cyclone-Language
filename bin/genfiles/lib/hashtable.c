#include "cyc_include.h"

 struct _tagged_ptr0{ struct Cyc_List_List** curr; struct Cyc_List_List** base;
struct Cyc_List_List** last_plus_one; } ; struct _tuple0{ void* f1; void* f2; }
; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
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
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
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
Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp1);
unsigned int i; struct _tagged_ptr0 _temp3={ _temp2, _temp2, _temp2 + _temp1};
for( i= 0; i < _temp1; i ++){ _temp2[ i]= mt;} _temp3;}); _temp0;});} void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table* t, void* key, void* val){ struct _tagged_ptr0 tab= t->tab;
int bucket=( int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp4= tab;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}));({ struct _tagged_ptr0
_temp5= tab; struct Cyc_List_List** _temp7= _temp5.curr + bucket; if( _temp5.base
== 0? 1:( _temp7 < _temp5.base? 1: _temp7 >= _temp5.last_plus_one)){ _throw(
Null_Exception);}* _temp7=({ struct Cyc_List_List* _temp8=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp8->hd=( void*)({ struct _tuple0*
_temp12=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp12->f1= key;
_temp12->f2= val; _temp12;}); _temp8->tl=({ struct _tagged_ptr0 _temp9= tab;
struct Cyc_List_List** _temp11= _temp9.curr + bucket; if( _temp9.base == 0? 1:(
_temp11 < _temp9.base? 1: _temp11 >= _temp9.last_plus_one)){ _throw(
Null_Exception);}* _temp11;}); _temp8;});}); if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(({ struct _tagged_ptr0 _temp13= tab; struct Cyc_List_List**
_temp15= _temp13.curr + bucket; if( _temp13.base == 0? 1:( _temp15 < _temp13.base?
1: _temp15 >= _temp13.last_plus_one)){ _throw( Null_Exception);}* _temp15;})) >
t->max_len){(( void(*)( struct Cyc_Hashtable_Table* t)) Cyc_Hashtable_resize)( t);}}
void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void* key){ struct
_tagged_ptr0 tab= t->tab; struct Cyc_List_List* l=({ struct _tagged_ptr0 _temp17=
tab; struct Cyc_List_List** _temp19= _temp17.curr +( int)((* t->hash)( key) %({
struct _tagged_ptr0 _temp16= tab;( unsigned int)( _temp16.last_plus_one -
_temp16.curr);})); if( _temp17.base == 0? 1:( _temp19 < _temp17.base? 1: _temp19
>= _temp17.last_plus_one)){ _throw( Null_Exception);}* _temp19;}); return(( void*(*)(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x)) Cyc_List_assoc_cmp)(
t->cmp, l, key);} void Cyc_Hashtable_remove( struct Cyc_Hashtable_Table* t, void*
key){ struct _tagged_ptr0 tab= t->tab; int(* cmp)( void*, void*)= t->cmp; int
bucket=( int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp20= tab;(
unsigned int)( _temp20.last_plus_one - _temp20.curr);})); struct Cyc_List_List*
l=({ struct _tagged_ptr0 _temp21= tab; struct Cyc_List_List** _temp23= _temp21.curr
+ bucket; if( _temp21.base == 0? 1:( _temp23 < _temp21.base? 1: _temp23 >=
_temp21.last_plus_one)){ _throw( Null_Exception);}* _temp23;}); if( l == 0){
return;} if((* cmp)( key,({ struct _tuple0* _temp25=( struct _tuple0*)({ struct
Cyc_List_List* _temp24= l; if( _temp24 == 0){ _throw( Null_Exception);} _temp24->hd;});
unsigned int _temp26= 0; if( _temp26 >= 1u){ _throw( Null_Exception);} _temp25[
_temp26];}).f1) == 0){({ struct _tagged_ptr0 _temp27= tab; struct Cyc_List_List**
_temp29= _temp27.curr + bucket; if( _temp27.base == 0? 1:( _temp29 < _temp27.base?
1: _temp29 >= _temp27.last_plus_one)){ _throw( Null_Exception);}* _temp29=({
struct Cyc_List_List* _temp30= l; if( _temp30 == 0){ _throw( Null_Exception);}
_temp30->tl;});}); return;}{ struct Cyc_List_List* prev= l; l=({ struct Cyc_List_List*
_temp31= l; if( _temp31 == 0){ _throw( Null_Exception);} _temp31->tl;}); for( 0;({
struct Cyc_List_List* _temp32= l; if( _temp32 == 0){ _throw( Null_Exception);}
_temp32->tl;}) != 0; prev= l, l=({ struct Cyc_List_List* _temp33= l; if( _temp33
== 0){ _throw( Null_Exception);} _temp33->tl;})){ if((* cmp)( key,({ struct
_tuple0* _temp35=( struct _tuple0*)({ struct Cyc_List_List* _temp34= l; if(
_temp34 == 0){ _throw( Null_Exception);} _temp34->hd;}); unsigned int _temp36= 0;
if( _temp36 >= 1u){ _throw( Null_Exception);} _temp35[ _temp36];}).f1) == 0){({
struct Cyc_List_List* _temp37= prev; if( _temp37 == 0){ _throw( Null_Exception);}
_temp37->tl=({ struct Cyc_List_List* _temp38= l; if( _temp38 == 0){ _throw(
Null_Exception);} _temp38->tl;});}); return;}}}} int Cyc_Hashtable_hash_string(
struct _tagged_string s){ int ans= 0; int sz=( int)({ struct _tagged_string
_temp39= s;( unsigned int)( _temp39.last_plus_one - _temp39.curr);}); int shift=
0;{ int i= 0; for( 0; i < sz; ++ i){ ans= ans ^({ struct _tagged_string _temp40=
s; char* _temp42= _temp40.curr + i; if( _temp40.base == 0? 1:( _temp42 < _temp40.base?
1: _temp42 >= _temp40.last_plus_one)){ _throw( Null_Exception);}* _temp42;}) <<
shift; shift += 8; if( shift == 32){ shift= 0;}}} return ans;} int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s){ return Cyc_Hashtable_hash_string(* s);} void Cyc_Hashtable_insert_bucket(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems){ if(
elems == 0){ return;}(( void(*)( struct _tagged_ptr0 tab, int(* hash)( void*),
struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)( tab, hash,({ struct
Cyc_List_List* _temp43= elems; if( _temp43 == 0){ _throw( Null_Exception);}
_temp43->tl;}));{ void* key=({ struct _tuple0* _temp45=( struct _tuple0*)({
struct Cyc_List_List* _temp44= elems; if( _temp44 == 0){ _throw( Null_Exception);}
_temp44->hd;}); unsigned int _temp46= 0; if( _temp46 >= 1u){ _throw(
Null_Exception);} _temp45[ _temp46];}).f1; void* val=({ struct _tuple0* _temp48=(
struct _tuple0*)({ struct Cyc_List_List* _temp47= elems; if( _temp47 == 0){
_throw( Null_Exception);} _temp47->hd;}); unsigned int _temp49= 0; if( _temp49
>= 1u){ _throw( Null_Exception);} _temp48[ _temp49];}).f2; int nidx=( int)((*
hash)( key) %({ struct _tagged_ptr0 _temp50= tab;( unsigned int)( _temp50.last_plus_one
- _temp50.curr);}));({ struct _tagged_ptr0 _temp51= tab; struct Cyc_List_List**
_temp53= _temp51.curr + nidx; if( _temp51.base == 0? 1:( _temp53 < _temp51.base?
1: _temp53 >= _temp51.last_plus_one)){ _throw( Null_Exception);}* _temp53=({
struct Cyc_List_List* _temp54=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp54->hd=( void*)({ struct _tuple0* _temp58=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp58->f1= key; _temp58->f2= val; _temp58;});
_temp54->tl=({ struct _tagged_ptr0 _temp55= tab; struct Cyc_List_List** _temp57=
_temp55.curr + nidx; if( _temp55.base == 0? 1:( _temp57 < _temp55.base? 1:
_temp57 >= _temp55.last_plus_one)){ _throw( Null_Exception);}* _temp57;});
_temp54;});});}} void Cyc_Hashtable_resize( struct Cyc_Hashtable_Table* t){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp59= odata;( unsigned int)( _temp59.last_plus_one - _temp59.curr);}); int
nsize= 2 * osize + 1; struct Cyc_List_List* mt= 0; struct _tagged_ptr0 ndata=({
unsigned int _temp60=( unsigned int) nsize; struct Cyc_List_List** _temp61=(
struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp60);
unsigned int i; struct _tagged_ptr0 _temp62={ _temp61, _temp61, _temp61 +
_temp60}; for( i= 0; i < _temp60; i ++){ _temp61[ i]= mt;} _temp62;});{ int i= 0;
for( 0; i < osize; i ++){(( void(*)( struct _tagged_ptr0 tab, int(* hash)( void*),
struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)( ndata, t->hash,({
struct _tagged_ptr0 _temp63= odata; struct Cyc_List_List** _temp65= _temp63.curr
+ i; if( _temp63.base == 0? 1:( _temp65 < _temp63.base? 1: _temp65 >= _temp63.last_plus_one)){
_throw( Null_Exception);}* _temp65;}));}} t->tab= ndata; t->max_len= 2 * t->max_len;}
void Cyc_Hashtable_iter( void(* f)( void*, void*), struct Cyc_Hashtable_Table* t){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp66= odata;( unsigned int)( _temp66.last_plus_one - _temp66.curr);}); int i=
0; for( 0; i < osize; i ++){ struct Cyc_List_List* iter=({ struct _tagged_ptr0
_temp67= odata; struct Cyc_List_List** _temp69= _temp67.curr + i; if( _temp67.base
== 0? 1:( _temp69 < _temp67.base? 1: _temp69 >= _temp67.last_plus_one)){ _throw(
Null_Exception);}* _temp69;}); for( 0; iter != 0; iter=({ struct Cyc_List_List*
_temp70= iter; if( _temp70 == 0){ _throw( Null_Exception);} _temp70->tl;})){ f(({
struct _tuple0* _temp72=( struct _tuple0*)({ struct Cyc_List_List* _temp71= iter;
if( _temp71 == 0){ _throw( Null_Exception);} _temp71->hd;}); unsigned int
_temp73= 0; if( _temp73 >= 1u){ _throw( Null_Exception);} _temp72[ _temp73];}).f1,({
struct _tuple0* _temp75=( struct _tuple0*)({ struct Cyc_List_List* _temp74= iter;
if( _temp74 == 0){ _throw( Null_Exception);} _temp74->hd;}); unsigned int
_temp76= 0; if( _temp76 >= 1u){ _throw( Null_Exception);} _temp75[ _temp76];}).f2);}}}
void Cyc_Hashtable_print_table_map( struct Cyc_Hashtable_Table* t, void(*
prn_key)( void*), void(* prn_val)( void*)){ struct _tagged_ptr0 odata= t->tab;
int osize=( int)({ struct _tagged_ptr0 _temp77= odata;( unsigned int)( _temp77.last_plus_one
- _temp77.curr);}); int i= 0; for( 0; i < osize; i ++){ printf("%d: ", i);{
struct Cyc_List_List* iter=({ struct _tagged_ptr0 _temp78= odata; struct Cyc_List_List**
_temp80= _temp78.curr + i; if( _temp78.base == 0? 1:( _temp80 < _temp78.base? 1:
_temp80 >= _temp78.last_plus_one)){ _throw( Null_Exception);}* _temp80;}); for(
0; iter != 0; iter=({ struct Cyc_List_List* _temp81= iter; if( _temp81 == 0){
_throw( Null_Exception);} _temp81->tl;})){ printf("("); prn_key(({ struct
_tuple0* _temp83=( struct _tuple0*)({ struct Cyc_List_List* _temp82= iter; if(
_temp82 == 0){ _throw( Null_Exception);} _temp82->hd;}); unsigned int _temp84= 0;
if( _temp84 >= 1u){ _throw( Null_Exception);} _temp83[ _temp84];}).f1); printf(",");
prn_val(({ struct _tuple0* _temp86=( struct _tuple0*)({ struct Cyc_List_List*
_temp85= iter; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->hd;});
unsigned int _temp87= 0; if( _temp87 >= 1u){ _throw( Null_Exception);} _temp86[
_temp87];}).f2); printf(") ");}} printf("\n");}}