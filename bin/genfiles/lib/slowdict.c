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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u]; struct Cyc_Splay_node;
struct Cyc_Splay_noderef{ struct Cyc_Splay_node* v; } ; static const int Cyc_Splay_Leaf=
0; static const int Cyc_Splay_Node= 0; struct Cyc_Splay_Node_struct{ int tag;
struct Cyc_Splay_noderef* f1; } ; struct Cyc_Splay_node{ void* key; void* data;
void* left; void* right; } ; extern int Cyc_Splay_splay( int(* f)( void*, void*),
void*, void*); struct Cyc_SlowDict_Dict; typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_hdict_t;
typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_dict_t; extern char Cyc_SlowDict_Present[
12u]; extern char Cyc_SlowDict_Absent[ 11u]; extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty( int(* comp)( void*, void*)); extern int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict* d); extern int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict*
d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict*
d, void* key, void* data); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data); extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict* d, struct Cyc_List_List* kds);
extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_singleton( int(* comp)( void*,
void*), void* key, void* data); extern void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict*
d, void* key); extern struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict*
d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key); extern void* Cyc_SlowDict_fold( void*(*
f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum); extern void*
Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*, void*), void* env, struct
Cyc_SlowDict_Dict* dict, void* accum); extern void Cyc_SlowDict_app( void*(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d); extern void Cyc_SlowDict_app_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d);
extern void Cyc_SlowDict_iter( void(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d); extern void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*), void* env,
struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_map(
void*(* f)( void*), struct Cyc_SlowDict_Dict* d); extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_map_c( void*(* f)( void*, void*), void* env, struct Cyc_SlowDict_Dict*
d); struct _tuple0{ void* f1; void* f2; } ; extern struct _tuple0* Cyc_SlowDict_choose(
struct Cyc_SlowDict_Dict* d); extern struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d); char Cyc_SlowDict_Absent[ 11u]; char Cyc_SlowDict_Present[
12u]; struct Cyc_SlowDict_Dict{ int(* reln)( void*, void*); void* tree; } ;
struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)( void*, void*)){ void*
t=( void*) Cyc_Splay_Leaf; return({ struct Cyc_SlowDict_Dict* _temp0=( struct
Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp0->reln=
comp; _temp0->tree=( void*) t; _temp0;});} int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict*
d){ void* _temp1=( void*) d->tree; struct Cyc_Splay_noderef* _temp7; _LL3: if(
_temp1 ==( void*) Cyc_Splay_Leaf){ goto _LL4;} else{ goto _LL5;} _LL5: if((
unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Splay_Node: 0){ _LL8: _temp7=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp1)->f1; goto
_LL6;} else{ goto _LL2;} _LL4: return 1; _LL6: return 0; _LL2:;} int Cyc_SlowDict_member(
struct Cyc_SlowDict_Dict* d, void* key){ return(( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree);} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void* key, void* data){ void*
newleft=( void*) Cyc_Splay_Leaf; void* newright=( void*) Cyc_Splay_Leaf; if(((
int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,(
void*) d->tree)){ void* _temp9=( void*) d->tree; struct Cyc_Splay_noderef*
_temp15; _LL11: if(( unsigned int) _temp9 > 1u?*(( int*) _temp9) == Cyc_Splay_Node:
0){ _LL16: _temp15=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp9)->f1; goto _LL12;} else{ goto _LL13;} _LL13: goto _LL14; _LL12: newleft=(
void*)( _temp15->v)->left; newright=( void*)( _temp15->v)->right; goto _LL10;
_LL14: goto _LL10; _LL10:;} else{ void* _temp17=( void*) d->tree; struct Cyc_Splay_noderef*
_temp23; _LL19: if(( unsigned int) _temp17 > 1u?*(( int*) _temp17) == Cyc_Splay_Node:
0){ _LL24: _temp23=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp17)->f1; goto _LL20;} else{ goto _LL21;} _LL21: if( _temp17 ==( void*) Cyc_Splay_Leaf){
goto _LL22;} else{ goto _LL18;} _LL20: { struct Cyc_Splay_node* n= _temp23->v;
if(( d->reln)( key,( void*) n->key) < 0){ newleft=( void*) n->left; newright=(
void*)({ struct Cyc_Splay_Node_struct* _temp25=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp25[ 0]=({ struct Cyc_Splay_Node_struct
_temp26; _temp26.tag= Cyc_Splay_Node; _temp26.f1=({ struct Cyc_Splay_noderef*
_temp27=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp27->v=({ struct Cyc_Splay_node* _temp28=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp28->key=( void*)(( void*) n->key); _temp28->data=(
void*)(( void*) n->data); _temp28->left=( void*)(( void*) Cyc_Splay_Leaf);
_temp28->right=( void*)(( void*) n->right); _temp28;}); _temp27;}); _temp26;});
_temp25;});} else{ newleft=( void*)({ struct Cyc_Splay_Node_struct* _temp29=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp29[ 0]=({ struct Cyc_Splay_Node_struct _temp30; _temp30.tag= Cyc_Splay_Node;
_temp30.f1=({ struct Cyc_Splay_noderef* _temp31=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp31->v=({ struct Cyc_Splay_node*
_temp32=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp32->key=( void*)(( void*) n->key); _temp32->data=( void*)(( void*) n->data);
_temp32->left=( void*)(( void*) n->left); _temp32->right=( void*)(( void*) Cyc_Splay_Leaf);
_temp32;}); _temp31;}); _temp30;}); _temp29;}); newright=( void*) n->right;}
goto _LL18;} _LL22: goto _LL18; _LL18:;} return({ struct Cyc_SlowDict_Dict*
_temp33=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp33->reln= d->reln; _temp33->tree=( void*)(( void*)({ struct Cyc_Splay_Node_struct*
_temp34=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp34[ 0]=({ struct Cyc_Splay_Node_struct _temp35; _temp35.tag= Cyc_Splay_Node;
_temp35.f1=({ struct Cyc_Splay_noderef* _temp36=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp36->v=({ struct Cyc_Splay_node*
_temp37=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp37->key=( void*) key; _temp37->data=( void*) data; _temp37->left=( void*)
newleft; _temp37->right=( void*) newright; _temp37;}); _temp36;}); _temp35;});
_temp34;})); _temp33;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if((( int(*)( int(* f)(
void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){(
void) _throw(( void*) Cyc_SlowDict_Present);} return(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)( d,
key, data);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict*
d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=({ struct Cyc_List_List*
_temp38= kds; if( _temp38 == 0){ _throw( Null_Exception);} _temp38->tl;})){ d=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key, void* data))
Cyc_SlowDict_insert)( d,(*(( struct _tuple0*)({ struct Cyc_List_List* _temp39=
kds; if( _temp39 == 0){ _throw( Null_Exception);} _temp39->hd;}))).f1,(*((
struct _tuple0*)({ struct Cyc_List_List* _temp40= kds; if( _temp40 == 0){ _throw(
Null_Exception);} _temp40->hd;}))).f2);} return d;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton( int(* comp)( void*, void*), void* key, void* data){
return({ struct Cyc_SlowDict_Dict* _temp41=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp41->reln= comp; _temp41->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp42=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp42[ 0]=({ struct Cyc_Splay_Node_struct
_temp43; _temp43.tag= Cyc_Splay_Node; _temp43.f1=({ struct Cyc_Splay_noderef*
_temp44=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp44->v=({ struct Cyc_Splay_node* _temp45=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp45->key=( void*) key; _temp45->data=( void*)
data; _temp45->left=( void*)(( void*) Cyc_Splay_Leaf); _temp45->right=( void*)((
void*) Cyc_Splay_Leaf); _temp45;}); _temp44;}); _temp43;}); _temp42;})); _temp41;});}
void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree)){ void* _temp46=( void*) d->tree; struct Cyc_Splay_noderef* _temp52;
_LL48: if(( unsigned int) _temp46 > 1u?*(( int*) _temp46) == Cyc_Splay_Node: 0){
_LL53: _temp52=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp46)->f1; goto _LL49;} else{ goto _LL50;} _LL50: if( _temp46 ==( void*) Cyc_Splay_Leaf){
goto _LL51;} else{ goto _LL47;} _LL49: return( void*)( _temp52->v)->data; _LL51:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp54=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp54[ 0]=({ struct
Cyc_Core_Impossible_struct _temp55; _temp55.tag= Cyc_Core_Impossible; _temp55.f1=(
struct _tagged_string)({ char* _temp56=( char*)"Dict::lookup"; struct
_tagged_string _temp57; _temp57.curr= _temp56; _temp57.base= _temp56; _temp57.last_plus_one=
_temp56 + 13; _temp57;}); _temp55;}); _temp54;})); _LL47:;}( void) _throw(( void*)
Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp58=( void*) d->tree; struct Cyc_Splay_noderef*
_temp64; _LL60: if(( unsigned int) _temp58 > 1u?*(( int*) _temp58) == Cyc_Splay_Node:
0){ _LL65: _temp64=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp58)->f1; goto _LL61;} else{ goto _LL62;} _LL62: if( _temp58 ==( void*) Cyc_Splay_Leaf){
goto _LL63;} else{ goto _LL59;} _LL61: return({ struct Cyc_Core_Opt* _temp66=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp66->v=(
void*)(( void*)( _temp64->v)->data); _temp66;}); _LL63:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp67=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp67[ 0]=({ struct
Cyc_Core_Impossible_struct _temp68; _temp68.tag= Cyc_Core_Impossible; _temp68.f1=(
struct _tagged_string)({ char* _temp69=( char*)"Dict::lookup"; struct
_tagged_string _temp70; _temp70.curr= _temp69; _temp70.base= _temp69; _temp70.last_plus_one=
_temp69 + 13; _temp70;}); _temp68;}); _temp67;})); _LL59:;} return 0;} static
int Cyc_SlowDict_get_largest( void* x, void* y){ return 1;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(*
f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){
void* _temp71=( void*) d->tree; struct Cyc_Splay_noderef* _temp77; _LL73: if(
_temp71 ==( void*) Cyc_Splay_Leaf){ goto _LL74;} else{ goto _LL75;} _LL75: if((
unsigned int) _temp71 > 1u?*(( int*) _temp71) == Cyc_Splay_Node: 0){ _LL78:
_temp77=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp71)->f1;
goto _LL76;} else{ goto _LL72;} _LL74:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp79=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp79[ 0]=({ struct Cyc_Core_Impossible_struct _temp80; _temp80.tag= Cyc_Core_Impossible;
_temp80.f1=( struct _tagged_string)({ char* _temp81=( char*)"Dict::lookup";
struct _tagged_string _temp82; _temp82.curr= _temp81; _temp82.base= _temp81;
_temp82.last_plus_one= _temp81 + 13; _temp82;}); _temp80;}); _temp79;})); _LL76: {
struct Cyc_Splay_node* n= _temp77->v; void* _temp83=( void*) n->left; struct Cyc_Splay_noderef*
_temp89; _LL85: if( _temp83 ==( void*) Cyc_Splay_Leaf){ goto _LL86;} else{ goto
_LL87;} _LL87: if(( unsigned int) _temp83 > 1u?*(( int*) _temp83) == Cyc_Splay_Node:
0){ _LL90: _temp89=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp83)->f1; goto _LL88;} else{ goto _LL84;} _LL86: return({ struct Cyc_SlowDict_Dict*
_temp91=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp91->reln= d->reln; _temp91->tree=( void*)(( void*) n->right); _temp91;});
_LL88: { void* _temp92=( void*) n->right; struct Cyc_Splay_noderef* _temp98;
_LL94: if( _temp92 ==( void*) Cyc_Splay_Leaf){ goto _LL95;} else{ goto _LL96;}
_LL96: if(( unsigned int) _temp92 > 1u?*(( int*) _temp92) == Cyc_Splay_Node: 0){
_LL99: _temp98=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp92)->f1; goto _LL97;} else{ goto _LL93;} _LL95: return({ struct Cyc_SlowDict_Dict*
_temp100=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp100->reln= d->reln; _temp100->tree=( void*)(( void*) n->left); _temp100;});
_LL97:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp89->v; return({ struct Cyc_SlowDict_Dict* _temp101=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp101->reln= d->reln; _temp101->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp102=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp102[ 0]=({ struct Cyc_Splay_Node_struct
_temp103; _temp103.tag= Cyc_Splay_Node; _temp103.f1=({ struct Cyc_Splay_noderef*
_temp104=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp104->v=({ struct Cyc_Splay_node* _temp105=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp105->key=( void*)(( void*)
newtop->key); _temp105->data=( void*)(( void*) newtop->data); _temp105->left=(
void*)(( void*) newtop->left); _temp105->right=( void*)(( void*) n->right);
_temp105;}); _temp104;}); _temp103;}); _temp102;})); _temp101;});} _LL93:;}
_LL84:;} _LL72:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* d2=(( struct
Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); if( d == d2){( void) _throw(( void*) Cyc_SlowDict_Absent);} return d2;}
static void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*, void*), void* t,
void* accum){ void* _temp106= t; struct Cyc_Splay_noderef* _temp112; _LL108: if(
_temp106 ==( void*) Cyc_Splay_Leaf){ goto _LL109;} else{ goto _LL110;} _LL110:
if(( unsigned int) _temp106 > 1u?*(( int*) _temp106) == Cyc_Splay_Node: 0){
_LL113: _temp112=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp106)->f1; goto _LL111;} else{ goto _LL107;} _LL109: return accum; _LL111: {
struct Cyc_Splay_node* n= _temp112->v; return f(( void*) n->key,( void*) n->data,((
void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL107:;} void*
Cyc_SlowDict_fold( void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d,
void* accum){ return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum){ void*
_temp114= t; struct Cyc_Splay_noderef* _temp120; _LL116: if( _temp114 ==( void*)
Cyc_Splay_Leaf){ goto _LL117;} else{ goto _LL118;} _LL118: if(( unsigned int)
_temp114 > 1u?*(( int*) _temp114) == Cyc_Splay_Node: 0){ _LL121: _temp120=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp114)->f1; goto
_LL119;} else{ goto _LL115;} _LL117: return accum; _LL119: { struct Cyc_Splay_node*
n= _temp120->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL115:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp122= t; struct Cyc_Splay_noderef*
_temp128; _LL124: if( _temp122 ==( void*) Cyc_Splay_Leaf){ goto _LL125;} else{
goto _LL126;} _LL126: if(( unsigned int) _temp122 > 1u?*(( int*) _temp122) ==
Cyc_Splay_Node: 0){ _LL129: _temp128=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp122)->f1; goto _LL127;} else{ goto _LL123;} _LL125: goto _LL123; _LL127: {
struct Cyc_Splay_node* n= _temp128->v;(( void(*)( void*(* f)( void*, void*),
void* t)) Cyc_SlowDict_app_tree)( f,( void*) n->left); f(( void*) n->key,( void*)
n->data);(( void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)(
f,( void*) n->right); goto _LL123;} _LL123:;} void Cyc_SlowDict_app( void*(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void*(* f)( void*, void*),
void* t)) Cyc_SlowDict_app_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_iter_tree(
void(* f)( void*, void*), void* t){ void* _temp130= t; struct Cyc_Splay_noderef*
_temp136; _LL132: if( _temp130 ==( void*) Cyc_Splay_Leaf){ goto _LL133;} else{
goto _LL134;} _LL134: if(( unsigned int) _temp130 > 1u?*(( int*) _temp130) ==
Cyc_Splay_Node: 0){ _LL137: _temp136=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp130)->f1; goto _LL135;} else{ goto _LL131;} _LL133: goto _LL131; _LL135: {
struct Cyc_Splay_node* n= _temp136->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL131;} _LL131:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp138= t; struct
Cyc_Splay_noderef* _temp144; _LL140: if( _temp138 ==( void*) Cyc_Splay_Leaf){
goto _LL141;} else{ goto _LL142;} _LL142: if(( unsigned int) _temp138 > 1u?*((
int*) _temp138) == Cyc_Splay_Node: 0){ _LL145: _temp144=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp138)->f1; goto _LL143;} else{ goto _LL139;}
_LL141: goto _LL139; _LL143: { struct Cyc_Splay_node* n= _temp144->v;(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->right); goto _LL139;} _LL139:;} void Cyc_SlowDict_app_c( void*(*
f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp146= t; struct Cyc_Splay_noderef*
_temp152; _LL148: if( _temp146 ==( void*) Cyc_Splay_Leaf){ goto _LL149;} else{
goto _LL150;} _LL150: if(( unsigned int) _temp146 > 1u?*(( int*) _temp146) ==
Cyc_Splay_Node: 0){ _LL153: _temp152=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp146)->f1; goto _LL151;} else{ goto _LL147;} _LL149: goto _LL147; _LL151: {
struct Cyc_Splay_node* n= _temp152->v;(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->left); f( env,(
void*) n->key,( void*) n->data);(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->right); goto
_LL147;} _LL147:;} void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*,
void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) d->tree);}
static void* Cyc_SlowDict_map_tree( void*(* f)( void*), void* t){ void* _temp154=
t; struct Cyc_Splay_noderef* _temp160; _LL156: if( _temp154 ==( void*) Cyc_Splay_Leaf){
goto _LL157;} else{ goto _LL158;} _LL158: if(( unsigned int) _temp154 > 1u?*((
int*) _temp154) == Cyc_Splay_Node: 0){ _LL161: _temp160=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp154)->f1; goto _LL159;} else{ goto _LL155;}
_LL157: return( void*) Cyc_Splay_Leaf; _LL159: { struct Cyc_Splay_node* n=
_temp160->v; return( void*)({ struct Cyc_Splay_Node_struct* _temp162=( struct
Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp162[ 0]=({ struct Cyc_Splay_Node_struct _temp163; _temp163.tag= Cyc_Splay_Node;
_temp163.f1=({ struct Cyc_Splay_noderef* _temp164=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp164->v=({ struct Cyc_Splay_node*
_temp165=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp165->key=( void*)(( void*) n->key); _temp165->data=( void*) f(( void*) n->data);
_temp165->left=( void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)(
f,( void*) n->left); _temp165->right=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) n->right); _temp165;}); _temp164;});
_temp163;}); _temp162;});} _LL155:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map(
void*(* f)( void*), struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp166=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp166->reln= d->reln; _temp166->tree=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) d->tree); _temp166;});} static void*
Cyc_SlowDict_map_tree_c( void*(* f)( void*, void*), void* env, void* t){ void*
_temp167= t; struct Cyc_Splay_noderef* _temp173; _LL169: if( _temp167 ==( void*)
Cyc_Splay_Leaf){ goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp167 > 1u?*(( int*) _temp167) == Cyc_Splay_Node: 0){ _LL174: _temp173=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp167)->f1; goto
_LL172;} else{ goto _LL168;} _LL170: return( void*) Cyc_Splay_Leaf; _LL172: {
struct Cyc_Splay_node* n= _temp173->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp175=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp175[ 0]=({ struct Cyc_Splay_Node_struct _temp176; _temp176.tag= Cyc_Splay_Node;
_temp176.f1=({ struct Cyc_Splay_noderef* _temp177=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp177->v=({ struct Cyc_Splay_node*
_temp178=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp178->key=( void*)(( void*) n->key); _temp178->data=( void*) f( env,( void*)
n->data); _temp178->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp178->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp178;}); _temp177;}); _temp176;}); _temp175;});}
_LL168:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp179=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp179->reln= d->reln; _temp179->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp179;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp180=( void*) d->tree; struct Cyc_Splay_noderef* _temp186; _LL182: if(
_temp180 ==( void*) Cyc_Splay_Leaf){ goto _LL183;} else{ goto _LL184;} _LL184:
if(( unsigned int) _temp180 > 1u?*(( int*) _temp180) == Cyc_Splay_Node: 0){
_LL187: _temp186=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp180)->f1; goto _LL185;} else{ goto _LL181;} _LL183:( void) _throw(( void*)
Cyc_SlowDict_Absent); _LL185: return({ struct _tuple0* _temp188=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp188->f1=( void*)( _temp186->v)->key;
_temp188->f2=( void*)( _temp186->v)->data; _temp188;}); _LL181:;} struct Cyc_List_List*
Cyc_SlowDict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp189=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp189->hd=( void*)({ struct _tuple0* _temp190=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp190->f1= k; _temp190->f2=
v; _temp190;}); _temp189->tl= accum; _temp189;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}