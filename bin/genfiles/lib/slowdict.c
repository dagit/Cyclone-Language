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
struct Cyc_SlowDict_Dict* d); char Cyc_SlowDict_Absent[ 11u]="\000\000\000\000Absent";
char Cyc_SlowDict_Present[ 12u]="\000\000\000\000Present"; struct Cyc_SlowDict_Dict{
int(* reln)( void*, void*); void* tree; } ; struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty(
int(* comp)( void*, void*)){ void* t=( void*) Cyc_Splay_Leaf; return({ struct
Cyc_SlowDict_Dict* _temp0=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct
Cyc_SlowDict_Dict)); _temp0->reln= comp; _temp0->tree=( void*) t; _temp0;});}
int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d){ void* _temp1=( void*) d->tree;
struct Cyc_Splay_noderef* _temp7; _LL3: if( _temp1 ==( void*) Cyc_Splay_Leaf){
goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?*(( int*)
_temp1) == Cyc_Splay_Node: 0){ _LL8: _temp7=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp1)->f1; goto _LL6;} else{ goto _LL2;} _LL4:
return 1; _LL6: return 0; _LL2:;} int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict*
d, void* key){ return(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ void* newleft=( void*) Cyc_Splay_Leaf;
void* newright=( void*) Cyc_Splay_Leaf; if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp9=(
void*) d->tree; struct Cyc_Splay_noderef* _temp15; _LL11: if(( unsigned int)
_temp9 > 1u?*(( int*) _temp9) == Cyc_Splay_Node: 0){ _LL16: _temp15=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp9)->f1; goto _LL12;} else{ goto _LL13;}
_LL13: goto _LL14; _LL12: newleft=( void*)( _temp15->v)->left; newright=( void*)(
_temp15->v)->right; goto _LL10; _LL14: goto _LL10; _LL10:;} else{ void* _temp17=(
void*) d->tree; struct Cyc_Splay_noderef* _temp23; _LL19: if(( unsigned int)
_temp17 > 1u?*(( int*) _temp17) == Cyc_Splay_Node: 0){ _LL24: _temp23=( struct
Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp17)->f1; goto _LL20;}
else{ goto _LL21;} _LL21: if( _temp17 ==( void*) Cyc_Splay_Leaf){ goto _LL22;}
else{ goto _LL18;} _LL20: { struct Cyc_Splay_node* _temp25= _temp23->v; goto
_LL26; _LL26: if(( d->reln)( key,( void*) _temp25->key) < 0){ newleft=( void*)
_temp25->left; newright=( void*)({ struct Cyc_Splay_Node_struct* _temp27=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp27[ 0]=({ struct Cyc_Splay_Node_struct _temp28; _temp28.tag= Cyc_Splay_Node;
_temp28.f1=({ struct Cyc_Splay_noderef* _temp29=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp29->v=({ struct Cyc_Splay_node*
_temp30=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp30->key=( void*)(( void*) _temp25->key); _temp30->data=( void*)(( void*)
_temp25->data); _temp30->left=( void*)(( void*) Cyc_Splay_Leaf); _temp30->right=(
void*)(( void*) _temp25->right); _temp30;}); _temp29;}); _temp28;}); _temp27;});}
else{ newleft=( void*)({ struct Cyc_Splay_Node_struct* _temp31=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp31[ 0]=({ struct Cyc_Splay_Node_struct
_temp32; _temp32.tag= Cyc_Splay_Node; _temp32.f1=({ struct Cyc_Splay_noderef*
_temp33=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp33->v=({ struct Cyc_Splay_node* _temp34=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp34->key=( void*)(( void*) _temp25->key);
_temp34->data=( void*)(( void*) _temp25->data); _temp34->left=( void*)(( void*)
_temp25->left); _temp34->right=( void*)(( void*) Cyc_Splay_Leaf); _temp34;});
_temp33;}); _temp32;}); _temp31;}); newright=( void*) _temp25->right;} goto
_LL18;} _LL22: goto _LL18; _LL18:;} return({ struct Cyc_SlowDict_Dict* _temp35=(
struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp35->reln= d->reln; _temp35->tree=( void*)(( void*)({ struct Cyc_Splay_Node_struct*
_temp36=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp36[ 0]=({ struct Cyc_Splay_Node_struct _temp37; _temp37.tag= Cyc_Splay_Node;
_temp37.f1=({ struct Cyc_Splay_noderef* _temp38=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp38->v=({ struct Cyc_Splay_node*
_temp39=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp39->key=( void*) key; _temp39->data=( void*) data; _temp39->left=( void*)
newleft; _temp39->right=( void*) newright; _temp39;}); _temp38;}); _temp37;});
_temp36;})); _temp35;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if((( int(*)( int(* f)(
void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){(
void) _throw(( void*) Cyc_SlowDict_Present);} return(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)( d,
key, data);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict*
d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=(( struct Cyc_List_List*)
_check_null( kds))->tl){ d=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, void* key, void* data)) Cyc_SlowDict_insert)( d,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( kds))->hd)).f1,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( kds))->hd)).f2);} return d;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton( int(* comp)( void*, void*), void* key, void* data){
return({ struct Cyc_SlowDict_Dict* _temp40=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp40->reln= comp; _temp40->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp41=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp41[ 0]=({ struct Cyc_Splay_Node_struct
_temp42; _temp42.tag= Cyc_Splay_Node; _temp42.f1=({ struct Cyc_Splay_noderef*
_temp43=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp43->v=({ struct Cyc_Splay_node* _temp44=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp44->key=( void*) key; _temp44->data=( void*)
data; _temp44->left=( void*)(( void*) Cyc_Splay_Leaf); _temp44->right=( void*)((
void*) Cyc_Splay_Leaf); _temp44;}); _temp43;}); _temp42;}); _temp41;})); _temp40;});}
void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree)){ void* _temp45=( void*) d->tree; struct Cyc_Splay_noderef* _temp51;
_LL47: if(( unsigned int) _temp45 > 1u?*(( int*) _temp45) == Cyc_Splay_Node: 0){
_LL52: _temp51=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: if( _temp45 ==( void*) Cyc_Splay_Leaf){
goto _LL50;} else{ goto _LL46;} _LL48: return( void*)( _temp51->v)->data; _LL50:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp53=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp53[ 0]=({ struct
Cyc_Core_Impossible_struct _temp54; _temp54.tag= Cyc_Core_Impossible; _temp54.f1=(
struct _tagged_string)({ char* _temp55=( char*)"Dict::lookup"; struct
_tagged_string _temp56; _temp56.curr= _temp55; _temp56.base= _temp55; _temp56.last_plus_one=
_temp55 + 13; _temp56;}); _temp54;}); _temp53;})); _LL46:;}( void) _throw(( void*)
Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp57=( void*) d->tree; struct Cyc_Splay_noderef*
_temp63; _LL59: if(( unsigned int) _temp57 > 1u?*(( int*) _temp57) == Cyc_Splay_Node:
0){ _LL64: _temp63=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp57)->f1; goto _LL60;} else{ goto _LL61;} _LL61: if( _temp57 ==( void*) Cyc_Splay_Leaf){
goto _LL62;} else{ goto _LL58;} _LL60: return({ struct Cyc_Core_Opt* _temp65=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp65->v=(
void*)(( void*)( _temp63->v)->data); _temp65;}); _LL62:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp66=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp66[ 0]=({ struct
Cyc_Core_Impossible_struct _temp67; _temp67.tag= Cyc_Core_Impossible; _temp67.f1=(
struct _tagged_string)({ char* _temp68=( char*)"Dict::lookup"; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 13; _temp69;}); _temp67;}); _temp66;})); _LL58:;} return 0;} static
int Cyc_SlowDict_get_largest( void* x, void* y){ return 1;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(*
f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){
void* _temp70=( void*) d->tree; struct Cyc_Splay_noderef* _temp76; _LL72: if(
_temp70 ==( void*) Cyc_Splay_Leaf){ goto _LL73;} else{ goto _LL74;} _LL74: if((
unsigned int) _temp70 > 1u?*(( int*) _temp70) == Cyc_Splay_Node: 0){ _LL77:
_temp76=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp70)->f1;
goto _LL75;} else{ goto _LL71;} _LL73:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp78=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp78[ 0]=({ struct Cyc_Core_Impossible_struct _temp79; _temp79.tag= Cyc_Core_Impossible;
_temp79.f1=( struct _tagged_string)({ char* _temp80=( char*)"Dict::lookup";
struct _tagged_string _temp81; _temp81.curr= _temp80; _temp81.base= _temp80;
_temp81.last_plus_one= _temp80 + 13; _temp81;}); _temp79;}); _temp78;})); _LL75: {
struct Cyc_Splay_node* n= _temp76->v; void* _temp82=( void*) n->left; struct Cyc_Splay_noderef*
_temp88; _LL84: if( _temp82 ==( void*) Cyc_Splay_Leaf){ goto _LL85;} else{ goto
_LL86;} _LL86: if(( unsigned int) _temp82 > 1u?*(( int*) _temp82) == Cyc_Splay_Node:
0){ _LL89: _temp88=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp82)->f1; goto _LL87;} else{ goto _LL83;} _LL85: return({ struct Cyc_SlowDict_Dict*
_temp90=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp90->reln= d->reln; _temp90->tree=( void*)(( void*) n->right); _temp90;});
_LL87: { void* _temp91=( void*) n->right; struct Cyc_Splay_noderef* _temp97;
_LL93: if( _temp91 ==( void*) Cyc_Splay_Leaf){ goto _LL94;} else{ goto _LL95;}
_LL95: if(( unsigned int) _temp91 > 1u?*(( int*) _temp91) == Cyc_Splay_Node: 0){
_LL98: _temp97=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp91)->f1; goto _LL96;} else{ goto _LL92;} _LL94: return({ struct Cyc_SlowDict_Dict*
_temp99=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp99->reln= d->reln; _temp99->tree=( void*)(( void*) n->left); _temp99;});
_LL96:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp88->v; return({ struct Cyc_SlowDict_Dict* _temp100=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp100->reln= d->reln; _temp100->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp101=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp101[ 0]=({ struct Cyc_Splay_Node_struct
_temp102; _temp102.tag= Cyc_Splay_Node; _temp102.f1=({ struct Cyc_Splay_noderef*
_temp103=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp103->v=({ struct Cyc_Splay_node* _temp104=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp104->key=( void*)(( void*)
newtop->key); _temp104->data=( void*)(( void*) newtop->data); _temp104->left=(
void*)(( void*) newtop->left); _temp104->right=( void*)(( void*) n->right);
_temp104;}); _temp103;}); _temp102;}); _temp101;})); _temp100;});} _LL92:;}
_LL83:;} _LL71:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* _temp105=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); goto _LL106; _LL106: if( d == _temp105){( void) _throw(( void*) Cyc_SlowDict_Absent);}
return _temp105;} static void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*,
void*), void* t, void* accum){ void* _temp107= t; struct Cyc_Splay_noderef*
_temp113; _LL109: if( _temp107 ==( void*) Cyc_Splay_Leaf){ goto _LL110;} else{
goto _LL111;} _LL111: if(( unsigned int) _temp107 > 1u?*(( int*) _temp107) ==
Cyc_Splay_Node: 0){ _LL114: _temp113=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp107)->f1; goto _LL112;} else{ goto _LL108;} _LL110: return accum; _LL112: {
struct Cyc_Splay_node* n= _temp113->v; return f(( void*) n->key,( void*) n->data,((
void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL108:;} void*
Cyc_SlowDict_fold( void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d,
void* accum){ return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum){ void*
_temp115= t; struct Cyc_Splay_noderef* _temp121; _LL117: if( _temp115 ==( void*)
Cyc_Splay_Leaf){ goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp115 > 1u?*(( int*) _temp115) == Cyc_Splay_Node: 0){ _LL122: _temp121=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp115)->f1; goto
_LL120;} else{ goto _LL116;} _LL118: return accum; _LL120: { struct Cyc_Splay_node*
n= _temp121->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL116:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp123= t; struct Cyc_Splay_noderef*
_temp129; _LL125: if( _temp123 ==( void*) Cyc_Splay_Leaf){ goto _LL126;} else{
goto _LL127;} _LL127: if(( unsigned int) _temp123 > 1u?*(( int*) _temp123) ==
Cyc_Splay_Node: 0){ _LL130: _temp129=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp123)->f1; goto _LL128;} else{ goto _LL124;} _LL126: goto _LL124; _LL128: {
struct Cyc_Splay_node* _temp131= _temp129->v; goto _LL132; _LL132:(( void(*)(
void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) _temp131->left);
f(( void*) _temp131->key,( void*) _temp131->data);(( void(*)( void*(* f)( void*,
void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) _temp131->right); goto
_LL124;} _LL124:;} void Cyc_SlowDict_app( void*(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,(
void*) d->tree);} static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*),
void* t){ void* _temp133= t; struct Cyc_Splay_noderef* _temp139; _LL135: if(
_temp133 ==( void*) Cyc_Splay_Leaf){ goto _LL136;} else{ goto _LL137;} _LL137:
if(( unsigned int) _temp133 > 1u?*(( int*) _temp133) == Cyc_Splay_Node: 0){
_LL140: _temp139=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp133)->f1; goto _LL138;} else{ goto _LL134;} _LL136: goto _LL134; _LL138: {
struct Cyc_Splay_node* n= _temp139->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL134;} _LL134:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp141= t; struct
Cyc_Splay_noderef* _temp147; _LL143: if( _temp141 ==( void*) Cyc_Splay_Leaf){
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*((
int*) _temp141) == Cyc_Splay_Node: 0){ _LL148: _temp147=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp141)->f1; goto _LL146;} else{ goto _LL142;}
_LL144: goto _LL142; _LL146: { struct Cyc_Splay_node* n= _temp147->v;(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->right); goto _LL142;} _LL142:;} void Cyc_SlowDict_app_c( void*(*
f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp149= t; struct Cyc_Splay_noderef*
_temp155; _LL151: if( _temp149 ==( void*) Cyc_Splay_Leaf){ goto _LL152;} else{
goto _LL153;} _LL153: if(( unsigned int) _temp149 > 1u?*(( int*) _temp149) ==
Cyc_Splay_Node: 0){ _LL156: _temp155=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp149)->f1; goto _LL154;} else{ goto _LL150;} _LL152: goto _LL150; _LL154: {
struct Cyc_Splay_node* n= _temp155->v;(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->left); f( env,(
void*) n->key,( void*) n->data);(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->right); goto
_LL150;} _LL150:;} void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*,
void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) d->tree);}
static void* Cyc_SlowDict_map_tree( void*(* f)( void*), void* t){ void* _temp157=
t; struct Cyc_Splay_noderef* _temp163; _LL159: if( _temp157 ==( void*) Cyc_Splay_Leaf){
goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp157 > 1u?*((
int*) _temp157) == Cyc_Splay_Node: 0){ _LL164: _temp163=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp157)->f1; goto _LL162;} else{ goto _LL158;}
_LL160: return( void*) Cyc_Splay_Leaf; _LL162: { struct Cyc_Splay_node* _temp165=
_temp163->v; goto _LL166; _LL166: return( void*)({ struct Cyc_Splay_Node_struct*
_temp167=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp167[ 0]=({ struct Cyc_Splay_Node_struct _temp168; _temp168.tag= Cyc_Splay_Node;
_temp168.f1=({ struct Cyc_Splay_noderef* _temp169=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp169->v=({ struct Cyc_Splay_node*
_temp170=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp170->key=( void*)(( void*) _temp165->key); _temp170->data=( void*) f(( void*)
_temp165->data); _temp170->left=( void*)(( void*(*)( void*(* f)( void*), void* t))
Cyc_SlowDict_map_tree)( f,( void*) _temp165->left); _temp170->right=( void*)((
void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,( void*)
_temp165->right); _temp170;}); _temp169;}); _temp168;}); _temp167;});} _LL158:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp171=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp171->reln= d->reln; _temp171->tree=(
void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,(
void*) d->tree); _temp171;});} static void* Cyc_SlowDict_map_tree_c( void*(* f)(
void*, void*), void* env, void* t){ void* _temp172= t; struct Cyc_Splay_noderef*
_temp178; _LL174: if( _temp172 ==( void*) Cyc_Splay_Leaf){ goto _LL175;} else{
goto _LL176;} _LL176: if(( unsigned int) _temp172 > 1u?*(( int*) _temp172) ==
Cyc_Splay_Node: 0){ _LL179: _temp178=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp172)->f1; goto _LL177;} else{ goto _LL173;} _LL175: return( void*) Cyc_Splay_Leaf;
_LL177: { struct Cyc_Splay_node* n= _temp178->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp180=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp180[ 0]=({ struct Cyc_Splay_Node_struct _temp181; _temp181.tag= Cyc_Splay_Node;
_temp181.f1=({ struct Cyc_Splay_noderef* _temp182=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp182->v=({ struct Cyc_Splay_node*
_temp183=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp183->key=( void*)(( void*) n->key); _temp183->data=( void*) f( env,( void*)
n->data); _temp183->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp183->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp183;}); _temp182;}); _temp181;}); _temp180;});}
_LL173:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp184=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp184->reln= d->reln; _temp184->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp184;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp185=( void*) d->tree; struct Cyc_Splay_noderef* _temp191; _LL187: if(
_temp185 ==( void*) Cyc_Splay_Leaf){ goto _LL188;} else{ goto _LL189;} _LL189:
if(( unsigned int) _temp185 > 1u?*(( int*) _temp185) == Cyc_Splay_Node: 0){
_LL192: _temp191=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp185)->f1; goto _LL190;} else{ goto _LL186;} _LL188:( void) _throw(( void*)
Cyc_SlowDict_Absent); _LL190: return({ struct _tuple0* _temp193=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp193->f1=( void*)( _temp191->v)->key;
_temp193->f2=( void*)( _temp191->v)->data; _temp193;}); _LL186:;} struct Cyc_List_List*
Cyc_SlowDict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp194=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp194->hd=( void*)({ struct _tuple0* _temp195=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp195->f1= k; _temp195->f2=
v; _temp195;}); _temp194->tl= accum; _temp194;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}