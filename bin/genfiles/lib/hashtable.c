 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length(
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_assoc_cmp( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table* Cyc_Hashtable_create(
int sz, int(* cmp)( void*, void*), int(* hash)( void*)); extern void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table* t, void* key, void* val); extern void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key); extern void Cyc_Hashtable_resize(
struct Cyc_Hashtable_Table* t); extern void Cyc_Hashtable_remove( struct Cyc_Hashtable_Table*
t, void* key); extern int Cyc_Hashtable_hash_string( struct _tagged_string s);
extern int Cyc_Hashtable_hash_stringptr( struct _tagged_string* s); extern void
Cyc_Hashtable_iter( void(* f)( void*, void*), struct Cyc_Hashtable_Table* t);
extern void Cyc_Hashtable_print_table_map( struct Cyc_Hashtable_Table* t, void(*
prn_key)( void*), void(* prn_val)( void*)); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct _tagged_ptr0{ struct Cyc_List_List** curr; struct
Cyc_List_List** base; struct Cyc_List_List** last_plus_one; } ; struct Cyc_Hashtable_Table{
int(* cmp)( void*, void*); int(* hash)( void*); int max_len; struct _tagged_ptr0
tab; } ; struct Cyc_Hashtable_Table* Cyc_Hashtable_create( int sz, int(* cmp)(
void*, void*), int(* hash)( void*)){ struct Cyc_List_List* mt= 0; return({
struct Cyc_Hashtable_Table* _temp0=( struct Cyc_Hashtable_Table*) GC_malloc(
sizeof( struct Cyc_Hashtable_Table)); _temp0->cmp= cmp; _temp0->hash= hash;
_temp0->max_len= 3; _temp0->tab=({ unsigned int _temp1=( unsigned int) sz;
struct Cyc_List_List** _temp2=( struct Cyc_List_List**) GC_malloc( sizeof(
struct Cyc_List_List*) * _temp1); struct _tagged_ptr0 _temp4={ _temp2, _temp2,
_temp2 + _temp1};{ unsigned int _temp3= _temp1; unsigned int i; for( i= 0; i <
_temp3; i ++){ _temp2[ i]= mt;}}; _temp4;}); _temp0;});} struct _tuple0{ void*
f1; void* f2; } ; void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void*
key, void* val){ struct _tagged_ptr0 tab= t->tab; int bucket=( int)((* t->hash)(
key) %({ struct _tagged_ptr0 _temp7= tab;( unsigned int)( _temp7.last_plus_one -
_temp7.curr);}));*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*),
bucket)=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp5->hd=( void*)({ struct _tuple0* _temp6=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp6->f1= key; _temp6->f2=
val; _temp6;}); _temp5->tl=*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*),
bucket); _temp5;}); if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(*((
struct Cyc_List_List**(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*), bucket)) > t->max_len){((
void(*)( struct Cyc_Hashtable_Table* t)) Cyc_Hashtable_resize)( t);}} void* Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table* t, void* key){ struct _tagged_ptr0 tab= t->tab;
struct Cyc_List_List* l=*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*),(
int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp8= tab;( unsigned int)(
_temp8.last_plus_one - _temp8.curr);}))); return(( void*(*)( int(* compare)(
void*, void*), struct Cyc_List_List* l, void* x)) Cyc_List_assoc_cmp)( t->cmp, l,
key);} void Cyc_Hashtable_remove( struct Cyc_Hashtable_Table* t, void* key){
struct _tagged_ptr0 tab= t->tab; int(* cmp)( void*, void*)= t->cmp; int bucket=(
int)((* t->hash)( key) %({ struct _tagged_ptr0 _temp9= tab;( unsigned int)(
_temp9.last_plus_one - _temp9.curr);})); struct Cyc_List_List* l=*(( struct Cyc_List_List**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( tab,
sizeof( struct Cyc_List_List*), bucket); if( l == 0){ return;} if((* cmp)( key,(((
struct _tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1) == 0){*(( struct Cyc_List_List**(*)(
struct _tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( tab,
sizeof( struct Cyc_List_List*), bucket)=(( struct Cyc_List_List*) _check_null( l))->tl;
return;}{ struct Cyc_List_List* prev= l; l=(( struct Cyc_List_List*) _check_null(
l))->tl; for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0;( prev= l, l=((
struct Cyc_List_List*) _check_null( l))->tl)){ if((* cmp)( key,((( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( l))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1) == 0){(( struct Cyc_List_List*)
_check_null( prev))->tl=(( struct Cyc_List_List*) _check_null( l))->tl; return;}}}}
int Cyc_Hashtable_hash_string( struct _tagged_string s){ int ans= 0; int sz=(
int)({ struct _tagged_string _temp10= s;( unsigned int)( _temp10.last_plus_one -
_temp10.curr);}); int shift= 0;{ int i= 0; for( 0; i < sz; ++ i){ ans= ans ^*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) << shift; shift += 8;
if( shift == 32){ shift= 0;}}} return ans;} int Cyc_Hashtable_hash_stringptr(
struct _tagged_string* s){ return Cyc_Hashtable_hash_string(* s);} void Cyc_Hashtable_insert_bucket(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems){ if(
elems == 0){ return;}(( void(*)( struct _tagged_ptr0 tab, int(* hash)( void*),
struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)( tab, hash,(( struct
Cyc_List_List*) _check_null( elems))->tl);{ void* key=((( struct _tuple0*)((
struct Cyc_List_List*) _check_null( elems))->hd)[ _check_known_subscript_notnull(
1u, 0)]).f1; void* val=((( struct _tuple0*)(( struct Cyc_List_List*) _check_null(
elems))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2; int nidx=( int)((*
hash)( key) %({ struct _tagged_ptr0 _temp13= tab;( unsigned int)( _temp13.last_plus_one
- _temp13.curr);}));*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*),
nidx)=({ struct Cyc_List_List* _temp11=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp11->hd=( void*)({ struct _tuple0* _temp12=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp12->f1= key; _temp12->f2=
val; _temp12;}); _temp11->tl=*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( tab, sizeof( struct Cyc_List_List*),
nidx); _temp11;});}} void Cyc_Hashtable_resize( struct Cyc_Hashtable_Table* t){
struct _tagged_ptr0 odata= t->tab; int osize=( int)({ struct _tagged_ptr0
_temp18= odata;( unsigned int)( _temp18.last_plus_one - _temp18.curr);}); int
nsize= 2 * osize + 1; struct Cyc_List_List* mt= 0; struct _tagged_ptr0 ndata=({
unsigned int _temp14=( unsigned int) nsize; struct Cyc_List_List** _temp15=(
struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*) * _temp14);
struct _tagged_ptr0 _temp17={ _temp15, _temp15, _temp15 + _temp14};{
unsigned int _temp16= _temp14; unsigned int i; for( i= 0; i < _temp16; i ++){
_temp15[ i]= mt;}}; _temp17;});{ int i= 0; for( 0; i < osize; i ++){(( void(*)(
struct _tagged_ptr0 tab, int(* hash)( void*), struct Cyc_List_List* elems)) Cyc_Hashtable_insert_bucket)(
ndata, t->hash,*(( struct Cyc_List_List**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( odata, sizeof( struct Cyc_List_List*),
i));}} t->tab= ndata; t->max_len= 2 * t->max_len;} void Cyc_Hashtable_iter( void(*
f)( void*, void*), struct Cyc_Hashtable_Table* t){ struct _tagged_ptr0 odata= t->tab;
int osize=( int)({ struct _tagged_ptr0 _temp19= odata;( unsigned int)( _temp19.last_plus_one
- _temp19.curr);}); int i= 0; for( 0; i < osize; i ++){ struct Cyc_List_List*
iter=*(( struct Cyc_List_List**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( odata, sizeof( struct Cyc_List_List*),
i); for( 0; iter != 0; iter=(( struct Cyc_List_List*) _check_null( iter))->tl){
f(((( struct _tuple0*)(( struct Cyc_List_List*) _check_null( iter))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1,((( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( iter))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2);}}} void
Cyc_Hashtable_print_table_map( struct Cyc_Hashtable_Table* t, void(* prn_key)(
void*), void(* prn_val)( void*)){ struct _tagged_ptr0 odata= t->tab; int osize=(
int)({ struct _tagged_ptr0 _temp20= odata;( unsigned int)( _temp20.last_plus_one
- _temp20.curr);}); int i= 0; for( 0; i < osize; i ++){ printf("%d: ", i);{
struct Cyc_List_List* iter=*(( struct Cyc_List_List**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( odata, sizeof( struct
Cyc_List_List*), i); for( 0; iter != 0; iter=(( struct Cyc_List_List*)
_check_null( iter))->tl){ printf("("); prn_key(((( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( iter))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1); printf(",");
prn_val(((( struct _tuple0*)(( struct Cyc_List_List*) _check_null( iter))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f2); printf(") ");}} printf("\n");}}