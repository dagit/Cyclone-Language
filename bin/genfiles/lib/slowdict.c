 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
struct Cyc_Splay_node; struct Cyc_Splay_noderef{ struct Cyc_Splay_node* v; } ;
static const int Cyc_Splay_Leaf= 0; static const int Cyc_Splay_Node= 0; struct
Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ; struct Cyc_Splay_node{
void* key; void* data; void* left; void* right; } ; extern int Cyc_Splay_splay(
int(* f)( void*, void*), void*, void*); struct Cyc_SlowDict_Dict; extern
unsigned char Cyc_SlowDict_Present[ 12u]; extern unsigned char Cyc_SlowDict_Absent[
11u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict* d, void* key); extern struct
Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void* key,
void* data); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new( struct
Cyc_SlowDict_Dict* d, void* key, void* data); extern struct Cyc_SlowDict_Dict*
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
struct Cyc_SlowDict_Dict* d); unsigned char Cyc_SlowDict_Absent[ 11u]="\000\000\000\000Absent";
unsigned char Cyc_SlowDict_Present[ 12u]="\000\000\000\000Present"; struct Cyc_SlowDict_Dict{
int(* reln)( void*, void*); void* tree; } ; struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty(
int(* comp)( void*, void*)){ void* t=( void*) Cyc_Splay_Leaf; return({ struct
Cyc_SlowDict_Dict* _temp0=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct
Cyc_SlowDict_Dict)); _temp0->reln= comp; _temp0->tree=( void*) t; _temp0;});}
int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d){ void* _temp1=( void*) d->tree;
struct Cyc_Splay_noderef* _temp7; _LL3: if( _temp1 ==( void*) Cyc_Splay_Leaf){
goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?*(( int*)
_temp1) == Cyc_Splay_Node: 0){ _LL8: _temp7=(( struct Cyc_Splay_Node_struct*)
_temp1)->f1; goto _LL6;} else{ goto _LL2;} _LL4: return 1; _LL6: return 0; _LL2:;}
int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict* d, void* key){ return(( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict*
d, void* key, void* data){ void* newleft=( void*) Cyc_Splay_Leaf; void* newright=(
void*) Cyc_Splay_Leaf; if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp9=( void*) d->tree; struct Cyc_Splay_noderef*
_temp15; _LL11: if(( unsigned int) _temp9 > 1u?*(( int*) _temp9) == Cyc_Splay_Node:
0){ _LL16: _temp15=(( struct Cyc_Splay_Node_struct*) _temp9)->f1; goto _LL12;}
else{ goto _LL13;} _LL13: goto _LL14; _LL12: newleft=( void*)( _temp15->v)->left;
newright=( void*)( _temp15->v)->right; goto _LL10; _LL14: goto _LL10; _LL10:;}
else{ void* _temp17=( void*) d->tree; struct Cyc_Splay_noderef* _temp23; _LL19:
if(( unsigned int) _temp17 > 1u?*(( int*) _temp17) == Cyc_Splay_Node: 0){ _LL24:
_temp23=(( struct Cyc_Splay_Node_struct*) _temp17)->f1; goto _LL20;} else{ goto
_LL21;} _LL21: if( _temp17 ==( void*) Cyc_Splay_Leaf){ goto _LL22;} else{ goto
_LL18;} _LL20: { struct Cyc_Splay_node* _temp25= _temp23->v; if(( d->reln)( key,(
void*) _temp25->key) < 0){ newleft=( void*) _temp25->left; newright=( void*)({
struct Cyc_Splay_Node_struct* _temp26=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp26[ 0]=({ struct Cyc_Splay_Node_struct
_temp27; _temp27.tag= Cyc_Splay_Node; _temp27.f1=({ struct Cyc_Splay_noderef*
_temp28=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp28->v=({ struct Cyc_Splay_node* _temp29=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp29->key=( void*)(( void*) _temp25->key);
_temp29->data=( void*)(( void*) _temp25->data); _temp29->left=( void*)(( void*)
Cyc_Splay_Leaf); _temp29->right=( void*)(( void*) _temp25->right); _temp29;});
_temp28;}); _temp27;}); _temp26;});} else{ newleft=( void*)({ struct Cyc_Splay_Node_struct*
_temp30=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp30[ 0]=({ struct Cyc_Splay_Node_struct _temp31; _temp31.tag= Cyc_Splay_Node;
_temp31.f1=({ struct Cyc_Splay_noderef* _temp32=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp32->v=({ struct Cyc_Splay_node*
_temp33=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp33->key=( void*)(( void*) _temp25->key); _temp33->data=( void*)(( void*)
_temp25->data); _temp33->left=( void*)(( void*) _temp25->left); _temp33->right=(
void*)(( void*) Cyc_Splay_Leaf); _temp33;}); _temp32;}); _temp31;}); _temp30;});
newright=( void*) _temp25->right;} goto _LL18;} _LL22: goto _LL18; _LL18:;}
return({ struct Cyc_SlowDict_Dict* _temp34=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp34->reln= d->reln; _temp34->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp35=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp35[ 0]=({ struct Cyc_Splay_Node_struct
_temp36; _temp36.tag= Cyc_Splay_Node; _temp36.f1=({ struct Cyc_Splay_noderef*
_temp37=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp37->v=({ struct Cyc_Splay_node* _temp38=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp38->key=( void*) key; _temp38->data=( void*)
data; _temp38->left=( void*) newleft; _temp38->right=( void*) newright; _temp38;});
_temp37;}); _temp36;}); _temp35;})); _temp34;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if((( int(*)( int(* f)(
void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){(
int) _throw(( void*) Cyc_SlowDict_Present);} return(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)( d,
key, data);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict*
d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=(( struct Cyc_List_List*)
_check_null( kds))->tl){ d=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, void* key, void* data)) Cyc_SlowDict_insert)( d,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( kds))->hd)).f1,(*(( struct _tuple0*)((
struct Cyc_List_List*) _check_null( kds))->hd)).f2);} return d;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton( int(* comp)( void*, void*), void* key, void* data){
return({ struct Cyc_SlowDict_Dict* _temp39=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp39->reln= comp; _temp39->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp40=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp40[ 0]=({ struct Cyc_Splay_Node_struct
_temp41; _temp41.tag= Cyc_Splay_Node; _temp41.f1=({ struct Cyc_Splay_noderef*
_temp42=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp42->v=({ struct Cyc_Splay_node* _temp43=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp43->key=( void*) key; _temp43->data=( void*)
data; _temp43->left=( void*)(( void*) Cyc_Splay_Leaf); _temp43->right=( void*)((
void*) Cyc_Splay_Leaf); _temp43;}); _temp42;}); _temp41;}); _temp40;})); _temp39;});}
void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree)){ void* _temp44=( void*) d->tree; struct Cyc_Splay_noderef* _temp50;
_LL46: if(( unsigned int) _temp44 > 1u?*(( int*) _temp44) == Cyc_Splay_Node: 0){
_LL51: _temp50=(( struct Cyc_Splay_Node_struct*) _temp44)->f1; goto _LL47;}
else{ goto _LL48;} _LL48: if( _temp44 ==( void*) Cyc_Splay_Leaf){ goto _LL49;}
else{ goto _LL45;} _LL47: return( void*)( _temp50->v)->data; _LL49:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp52=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp52[ 0]=({ struct
Cyc_Core_Impossible_struct _temp53; _temp53.tag= Cyc_Core_Impossible; _temp53.f1=
_tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp53;}); _temp52;}));
_LL45:;}( int) _throw(( void*) Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp54=(
void*) d->tree; struct Cyc_Splay_noderef* _temp60; _LL56: if(( unsigned int)
_temp54 > 1u?*(( int*) _temp54) == Cyc_Splay_Node: 0){ _LL61: _temp60=(( struct
Cyc_Splay_Node_struct*) _temp54)->f1; goto _LL57;} else{ goto _LL58;} _LL58: if(
_temp54 ==( void*) Cyc_Splay_Leaf){ goto _LL59;} else{ goto _LL55;} _LL57:
return({ struct Cyc_Core_Opt* _temp62=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp62->v=( void*)(( void*)( _temp60->v)->data); _temp62;});
_LL59:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp63=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp63[ 0]=({ struct Cyc_Core_Impossible_struct _temp64; _temp64.tag= Cyc_Core_Impossible;
_temp64.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp64;});
_temp63;})); _LL55:;} return 0;} static int Cyc_SlowDict_get_largest( void* x,
void* y){ return 1;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp65=( void*) d->tree; struct Cyc_Splay_noderef*
_temp71; _LL67: if( _temp65 ==( void*) Cyc_Splay_Leaf){ goto _LL68;} else{ goto
_LL69;} _LL69: if(( unsigned int) _temp65 > 1u?*(( int*) _temp65) == Cyc_Splay_Node:
0){ _LL72: _temp71=(( struct Cyc_Splay_Node_struct*) _temp65)->f1; goto _LL70;}
else{ goto _LL66;} _LL68:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp73=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp73[ 0]=({ struct Cyc_Core_Impossible_struct _temp74; _temp74.tag= Cyc_Core_Impossible;
_temp74.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp74;});
_temp73;})); _LL70: { struct Cyc_Splay_node* n= _temp71->v; void* _temp75=( void*)
n->left; struct Cyc_Splay_noderef* _temp81; _LL77: if( _temp75 ==( void*) Cyc_Splay_Leaf){
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp75 > 1u?*(( int*)
_temp75) == Cyc_Splay_Node: 0){ _LL82: _temp81=(( struct Cyc_Splay_Node_struct*)
_temp75)->f1; goto _LL80;} else{ goto _LL76;} _LL78: return({ struct Cyc_SlowDict_Dict*
_temp83=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp83->reln= d->reln; _temp83->tree=( void*)(( void*) n->right); _temp83;});
_LL80: { void* _temp84=( void*) n->right; struct Cyc_Splay_noderef* _temp90;
_LL86: if( _temp84 ==( void*) Cyc_Splay_Leaf){ goto _LL87;} else{ goto _LL88;}
_LL88: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) == Cyc_Splay_Node: 0){
_LL91: _temp90=(( struct Cyc_Splay_Node_struct*) _temp84)->f1; goto _LL89;}
else{ goto _LL85;} _LL87: return({ struct Cyc_SlowDict_Dict* _temp92=( struct
Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp92->reln=
d->reln; _temp92->tree=( void*)(( void*) n->left); _temp92;}); _LL89:(( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)( void* x, void*
y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp81->v; return({ struct Cyc_SlowDict_Dict* _temp93=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp93->reln= d->reln; _temp93->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp94=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp94[ 0]=({ struct Cyc_Splay_Node_struct
_temp95; _temp95.tag= Cyc_Splay_Node; _temp95.f1=({ struct Cyc_Splay_noderef*
_temp96=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp96->v=({ struct Cyc_Splay_node* _temp97=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp97->key=( void*)(( void*) newtop->key);
_temp97->data=( void*)(( void*) newtop->data); _temp97->left=( void*)(( void*)
newtop->left); _temp97->right=( void*)(( void*) n->right); _temp97;}); _temp96;});
_temp95;}); _temp94;})); _temp93;});} _LL85:;} _LL76:;} _LL66:;} else{ return d;}}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present( struct Cyc_SlowDict_Dict*
d, void* key){ struct Cyc_SlowDict_Dict* _temp98=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)( d, key); if( d ==
_temp98){( int) _throw(( void*) Cyc_SlowDict_Absent);} return _temp98;} static
void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*, void*), void* t, void*
accum){ void* _temp99= t; struct Cyc_Splay_noderef* _temp105; _LL101: if(
_temp99 ==( void*) Cyc_Splay_Leaf){ goto _LL102;} else{ goto _LL103;} _LL103:
if(( unsigned int) _temp99 > 1u?*(( int*) _temp99) == Cyc_Splay_Node: 0){ _LL106:
_temp105=(( struct Cyc_Splay_Node_struct*) _temp99)->f1; goto _LL104;} else{
goto _LL100;} _LL102: return accum; _LL104: { struct Cyc_Splay_node* n= _temp105->v;
return f(( void*) n->key,( void*) n->data,(( void*(*)( void*(* f)( void*, void*,
void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)( f,( void*) n->left,((
void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) n->right, accum)));} _LL100:;} void* Cyc_SlowDict_fold( void*(* f)(
void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c( void*(* f)(
void*, void*, void*, void*), void* env, void* t, void* accum){ void* _temp107= t;
struct Cyc_Splay_noderef* _temp113; _LL109: if( _temp107 ==( void*) Cyc_Splay_Leaf){
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 > 1u?*((
int*) _temp107) == Cyc_Splay_Node: 0){ _LL114: _temp113=(( struct Cyc_Splay_Node_struct*)
_temp107)->f1; goto _LL112;} else{ goto _LL108;} _LL110: return accum; _LL112: {
struct Cyc_Splay_node* n= _temp113->v; return f( env,( void*) n->key,( void*) n->data,((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, void* t, void*
accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->left,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->right, accum)));} _LL108:;} void* Cyc_SlowDict_fold_c( void*(*
f)( void*, void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* dict, void*
accum){ return(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) dict->tree,
accum);} static void Cyc_SlowDict_app_tree( void*(* f)( void*, void*), void* t){
void* _temp115= t; struct Cyc_Splay_noderef* _temp121; _LL117: if( _temp115 ==(
void*) Cyc_Splay_Leaf){ goto _LL118;} else{ goto _LL119;} _LL119: if((
unsigned int) _temp115 > 1u?*(( int*) _temp115) == Cyc_Splay_Node: 0){ _LL122:
_temp121=(( struct Cyc_Splay_Node_struct*) _temp115)->f1; goto _LL120;} else{
goto _LL116;} _LL118: goto _LL116; _LL120: { struct Cyc_Splay_node* _temp123=
_temp121->v;(( void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)(
f,( void*) _temp123->left); f(( void*) _temp123->key,( void*) _temp123->data);((
void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*)
_temp123->right); goto _LL116;} _LL116:;} void Cyc_SlowDict_app( void*(* f)(
void*, void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void*(* f)( void*, void*),
void* t)) Cyc_SlowDict_app_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_iter_tree(
void(* f)( void*, void*), void* t){ void* _temp124= t; struct Cyc_Splay_noderef*
_temp130; _LL126: if( _temp124 ==( void*) Cyc_Splay_Leaf){ goto _LL127;} else{
goto _LL128;} _LL128: if(( unsigned int) _temp124 > 1u?*(( int*) _temp124) ==
Cyc_Splay_Node: 0){ _LL131: _temp130=(( struct Cyc_Splay_Node_struct*) _temp124)->f1;
goto _LL129;} else{ goto _LL125;} _LL127: goto _LL125; _LL129: { struct Cyc_Splay_node*
n= _temp130->v;(( void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)(
f,( void*) n->left); f(( void*) n->key,( void*) n->data);(( void(*)( void(* f)(
void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*) n->right); goto
_LL125;} _LL125:;} void Cyc_SlowDict_iter( void(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d){(( void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,(
void*) d->tree);} static void Cyc_SlowDict_app_tree_c( void*(* f)( void*, void*,
void*), void* env, void* t){ void* _temp132= t; struct Cyc_Splay_noderef*
_temp138; _LL134: if( _temp132 ==( void*) Cyc_Splay_Leaf){ goto _LL135;} else{
goto _LL136;} _LL136: if(( unsigned int) _temp132 > 1u?*(( int*) _temp132) ==
Cyc_Splay_Node: 0){ _LL139: _temp138=(( struct Cyc_Splay_Node_struct*) _temp132)->f1;
goto _LL137;} else{ goto _LL133;} _LL135: goto _LL133; _LL137: { struct Cyc_Splay_node*
n= _temp138->v;(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->right); goto _LL133;} _LL133:;}
void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp140= t; struct Cyc_Splay_noderef*
_temp146; _LL142: if( _temp140 ==( void*) Cyc_Splay_Leaf){ goto _LL143;} else{
goto _LL144;} _LL144: if(( unsigned int) _temp140 > 1u?*(( int*) _temp140) ==
Cyc_Splay_Node: 0){ _LL147: _temp146=(( struct Cyc_Splay_Node_struct*) _temp140)->f1;
goto _LL145;} else{ goto _LL141;} _LL143: goto _LL141; _LL145: { struct Cyc_Splay_node*
n= _temp146->v;(( void(*)( void(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->right); goto _LL141;} _LL141:;}
void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*),
void* t){ void* _temp148= t; struct Cyc_Splay_noderef* _temp154; _LL150: if(
_temp148 ==( void*) Cyc_Splay_Leaf){ goto _LL151;} else{ goto _LL152;} _LL152:
if(( unsigned int) _temp148 > 1u?*(( int*) _temp148) == Cyc_Splay_Node: 0){
_LL155: _temp154=(( struct Cyc_Splay_Node_struct*) _temp148)->f1; goto _LL153;}
else{ goto _LL149;} _LL151: return( void*) Cyc_Splay_Leaf; _LL153: { struct Cyc_Splay_node*
_temp156= _temp154->v; return( void*)({ struct Cyc_Splay_Node_struct* _temp157=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp157[ 0]=({ struct Cyc_Splay_Node_struct _temp158; _temp158.tag= Cyc_Splay_Node;
_temp158.f1=({ struct Cyc_Splay_noderef* _temp159=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp159->v=({ struct Cyc_Splay_node*
_temp160=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp160->key=( void*)(( void*) _temp156->key); _temp160->data=( void*) f(( void*)
_temp156->data); _temp160->left=( void*)(( void*(*)( void*(* f)( void*), void* t))
Cyc_SlowDict_map_tree)( f,( void*) _temp156->left); _temp160->right=( void*)((
void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,( void*)
_temp156->right); _temp160;}); _temp159;}); _temp158;}); _temp157;});} _LL149:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp161=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp161->reln= d->reln; _temp161->tree=(
void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,(
void*) d->tree); _temp161;});} static void* Cyc_SlowDict_map_tree_c( void*(* f)(
void*, void*), void* env, void* t){ void* _temp162= t; struct Cyc_Splay_noderef*
_temp168; _LL164: if( _temp162 ==( void*) Cyc_Splay_Leaf){ goto _LL165;} else{
goto _LL166;} _LL166: if(( unsigned int) _temp162 > 1u?*(( int*) _temp162) ==
Cyc_Splay_Node: 0){ _LL169: _temp168=(( struct Cyc_Splay_Node_struct*) _temp162)->f1;
goto _LL167;} else{ goto _LL163;} _LL165: return( void*) Cyc_Splay_Leaf; _LL167: {
struct Cyc_Splay_node* n= _temp168->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp170=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp170[ 0]=({ struct Cyc_Splay_Node_struct _temp171; _temp171.tag= Cyc_Splay_Node;
_temp171.f1=({ struct Cyc_Splay_noderef* _temp172=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp172->v=({ struct Cyc_Splay_node*
_temp173=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp173->key=( void*)(( void*) n->key); _temp173->data=( void*) f( env,( void*)
n->data); _temp173->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp173->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp173;}); _temp172;}); _temp171;}); _temp170;});}
_LL163:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp174=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp174->reln= d->reln; _temp174->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp174;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp175=( void*) d->tree; struct Cyc_Splay_noderef* _temp181; _LL177: if(
_temp175 ==( void*) Cyc_Splay_Leaf){ goto _LL178;} else{ goto _LL179;} _LL179:
if(( unsigned int) _temp175 > 1u?*(( int*) _temp175) == Cyc_Splay_Node: 0){
_LL182: _temp181=(( struct Cyc_Splay_Node_struct*) _temp175)->f1; goto _LL180;}
else{ goto _LL176;} _LL178:( int) _throw(( void*) Cyc_SlowDict_Absent); _LL180:
return({ struct _tuple0* _temp183=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp183->f1=( void*)( _temp181->v)->key; _temp183->f2=( void*)(
_temp181->v)->data; _temp183;}); _LL176:;} struct Cyc_List_List* Cyc_SlowDict_to_list_f(
void* k, void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List*
_temp184=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp184->hd=( void*)({ struct _tuple0* _temp185=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp185->f1= k; _temp185->f2= v; _temp185;});
_temp184->tl= accum; _temp184;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}