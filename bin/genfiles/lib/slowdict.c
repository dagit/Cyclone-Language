 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
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
_LL18;} _LL20: { struct Cyc_Splay_node* _temp25= _temp23->v; goto _LL26; _LL26:
if(( d->reln)( key,( void*) _temp25->key) < 0){ newleft=( void*) _temp25->left;
newright=( void*)({ struct Cyc_Splay_Node_struct* _temp27=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp27[ 0]=({ struct Cyc_Splay_Node_struct
_temp28; _temp28.tag= Cyc_Splay_Node; _temp28.f1=({ struct Cyc_Splay_noderef*
_temp29=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp29->v=({ struct Cyc_Splay_node* _temp30=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp30->key=( void*)(( void*) _temp25->key);
_temp30->data=( void*)(( void*) _temp25->data); _temp30->left=( void*)(( void*)
Cyc_Splay_Leaf); _temp30->right=( void*)(( void*) _temp25->right); _temp30;});
_temp29;}); _temp28;}); _temp27;});} else{ newleft=( void*)({ struct Cyc_Splay_Node_struct*
_temp31=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp31[ 0]=({ struct Cyc_Splay_Node_struct _temp32; _temp32.tag= Cyc_Splay_Node;
_temp32.f1=({ struct Cyc_Splay_noderef* _temp33=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp33->v=({ struct Cyc_Splay_node*
_temp34=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp34->key=( void*)(( void*) _temp25->key); _temp34->data=( void*)(( void*)
_temp25->data); _temp34->left=( void*)(( void*) _temp25->left); _temp34->right=(
void*)(( void*) Cyc_Splay_Leaf); _temp34;}); _temp33;}); _temp32;}); _temp31;});
newright=( void*) _temp25->right;} goto _LL18;} _LL22: goto _LL18; _LL18:;}
return({ struct Cyc_SlowDict_Dict* _temp35=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp35->reln= d->reln; _temp35->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp36=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp36[ 0]=({ struct Cyc_Splay_Node_struct
_temp37; _temp37.tag= Cyc_Splay_Node; _temp37.f1=({ struct Cyc_Splay_noderef*
_temp38=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp38->v=({ struct Cyc_Splay_node* _temp39=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp39->key=( void*) key; _temp39->data=( void*)
data; _temp39->left=( void*) newleft; _temp39->right=( void*) newright; _temp39;});
_temp38;}); _temp37;}); _temp36;})); _temp35;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
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
_LL52: _temp51=(( struct Cyc_Splay_Node_struct*) _temp45)->f1; goto _LL48;}
else{ goto _LL49;} _LL49: if( _temp45 ==( void*) Cyc_Splay_Leaf){ goto _LL50;}
else{ goto _LL46;} _LL48: return( void*)( _temp51->v)->data; _LL50:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp53=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp53[ 0]=({ struct
Cyc_Core_Impossible_struct _temp54; _temp54.tag= Cyc_Core_Impossible; _temp54.f1=
_tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp54;}); _temp53;}));
_LL46:;}( int) _throw(( void*) Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp55=(
void*) d->tree; struct Cyc_Splay_noderef* _temp61; _LL57: if(( unsigned int)
_temp55 > 1u?*(( int*) _temp55) == Cyc_Splay_Node: 0){ _LL62: _temp61=(( struct
Cyc_Splay_Node_struct*) _temp55)->f1; goto _LL58;} else{ goto _LL59;} _LL59: if(
_temp55 ==( void*) Cyc_Splay_Leaf){ goto _LL60;} else{ goto _LL56;} _LL58:
return({ struct Cyc_Core_Opt* _temp63=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp63->v=( void*)(( void*)( _temp61->v)->data); _temp63;});
_LL60:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp64=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp64[ 0]=({ struct Cyc_Core_Impossible_struct _temp65; _temp65.tag= Cyc_Core_Impossible;
_temp65.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp65;});
_temp64;})); _LL56:;} return 0;} static int Cyc_SlowDict_get_largest( void* x,
void* y){ return 1;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp66=( void*) d->tree; struct Cyc_Splay_noderef*
_temp72; _LL68: if( _temp66 ==( void*) Cyc_Splay_Leaf){ goto _LL69;} else{ goto
_LL70;} _LL70: if(( unsigned int) _temp66 > 1u?*(( int*) _temp66) == Cyc_Splay_Node:
0){ _LL73: _temp72=(( struct Cyc_Splay_Node_struct*) _temp66)->f1; goto _LL71;}
else{ goto _LL67;} _LL69:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp74=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp74[ 0]=({ struct Cyc_Core_Impossible_struct _temp75; _temp75.tag= Cyc_Core_Impossible;
_temp75.f1= _tag_arr("Dict::lookup", sizeof( unsigned char), 13u); _temp75;});
_temp74;})); _LL71: { struct Cyc_Splay_node* n= _temp72->v; void* _temp76=( void*)
n->left; struct Cyc_Splay_noderef* _temp82; _LL78: if( _temp76 ==( void*) Cyc_Splay_Leaf){
goto _LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp76 > 1u?*(( int*)
_temp76) == Cyc_Splay_Node: 0){ _LL83: _temp82=(( struct Cyc_Splay_Node_struct*)
_temp76)->f1; goto _LL81;} else{ goto _LL77;} _LL79: return({ struct Cyc_SlowDict_Dict*
_temp84=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp84->reln= d->reln; _temp84->tree=( void*)(( void*) n->right); _temp84;});
_LL81: { void* _temp85=( void*) n->right; struct Cyc_Splay_noderef* _temp91;
_LL87: if( _temp85 ==( void*) Cyc_Splay_Leaf){ goto _LL88;} else{ goto _LL89;}
_LL89: if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Splay_Node: 0){
_LL92: _temp91=(( struct Cyc_Splay_Node_struct*) _temp85)->f1; goto _LL90;}
else{ goto _LL86;} _LL88: return({ struct Cyc_SlowDict_Dict* _temp93=( struct
Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp93->reln=
d->reln; _temp93->tree=( void*)(( void*) n->left); _temp93;}); _LL90:(( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)( void* x, void*
y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp82->v; return({ struct Cyc_SlowDict_Dict* _temp94=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp94->reln= d->reln; _temp94->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp95=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp95[ 0]=({ struct Cyc_Splay_Node_struct
_temp96; _temp96.tag= Cyc_Splay_Node; _temp96.f1=({ struct Cyc_Splay_noderef*
_temp97=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp97->v=({ struct Cyc_Splay_node* _temp98=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp98->key=( void*)(( void*) newtop->key);
_temp98->data=( void*)(( void*) newtop->data); _temp98->left=( void*)(( void*)
newtop->left); _temp98->right=( void*)(( void*) n->right); _temp98;}); _temp97;});
_temp96;}); _temp95;})); _temp94;});} _LL86:;} _LL77:;} _LL67:;} else{ return d;}}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present( struct Cyc_SlowDict_Dict*
d, void* key){ struct Cyc_SlowDict_Dict* _temp99=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)( d, key); goto
_LL100; _LL100: if( d == _temp99){( int) _throw(( void*) Cyc_SlowDict_Absent);}
return _temp99;} static void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*,
void*), void* t, void* accum){ void* _temp101= t; struct Cyc_Splay_noderef*
_temp107; _LL103: if( _temp101 ==( void*) Cyc_Splay_Leaf){ goto _LL104;} else{
goto _LL105;} _LL105: if(( unsigned int) _temp101 > 1u?*(( int*) _temp101) ==
Cyc_Splay_Node: 0){ _LL108: _temp107=(( struct Cyc_Splay_Node_struct*) _temp101)->f1;
goto _LL106;} else{ goto _LL102;} _LL104: return accum; _LL106: { struct Cyc_Splay_node*
n= _temp107->v; return f(( void*) n->key,( void*) n->data,(( void*(*)( void*(* f)(
void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)( f,( void*)
n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum))
Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL102:;} void* Cyc_SlowDict_fold(
void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum){
return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c( void*(* f)(
void*, void*, void*, void*), void* env, void* t, void* accum){ void* _temp109= t;
struct Cyc_Splay_noderef* _temp115; _LL111: if( _temp109 ==( void*) Cyc_Splay_Leaf){
goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp109 > 1u?*((
int*) _temp109) == Cyc_Splay_Node: 0){ _LL116: _temp115=(( struct Cyc_Splay_Node_struct*)
_temp109)->f1; goto _LL114;} else{ goto _LL110;} _LL112: return accum; _LL114: {
struct Cyc_Splay_node* n= _temp115->v; return f( env,( void*) n->key,( void*) n->data,((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, void* t, void*
accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->left,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->right, accum)));} _LL110:;} void* Cyc_SlowDict_fold_c( void*(*
f)( void*, void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* dict, void*
accum){ return(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) dict->tree,
accum);} static void Cyc_SlowDict_app_tree( void*(* f)( void*, void*), void* t){
void* _temp117= t; struct Cyc_Splay_noderef* _temp123; _LL119: if( _temp117 ==(
void*) Cyc_Splay_Leaf){ goto _LL120;} else{ goto _LL121;} _LL121: if((
unsigned int) _temp117 > 1u?*(( int*) _temp117) == Cyc_Splay_Node: 0){ _LL124:
_temp123=(( struct Cyc_Splay_Node_struct*) _temp117)->f1; goto _LL122;} else{
goto _LL118;} _LL120: goto _LL118; _LL122: { struct Cyc_Splay_node* _temp125=
_temp123->v; goto _LL126; _LL126:(( void(*)( void*(* f)( void*, void*), void* t))
Cyc_SlowDict_app_tree)( f,( void*) _temp125->left); f(( void*) _temp125->key,(
void*) _temp125->data);(( void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)(
f,( void*) _temp125->right); goto _LL118;} _LL118:;} void Cyc_SlowDict_app( void*(*
f)( void*, void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void*(* f)( void*,
void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_iter_tree(
void(* f)( void*, void*), void* t){ void* _temp127= t; struct Cyc_Splay_noderef*
_temp133; _LL129: if( _temp127 ==( void*) Cyc_Splay_Leaf){ goto _LL130;} else{
goto _LL131;} _LL131: if(( unsigned int) _temp127 > 1u?*(( int*) _temp127) ==
Cyc_Splay_Node: 0){ _LL134: _temp133=(( struct Cyc_Splay_Node_struct*) _temp127)->f1;
goto _LL132;} else{ goto _LL128;} _LL130: goto _LL128; _LL132: { struct Cyc_Splay_node*
n= _temp133->v;(( void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)(
f,( void*) n->left); f(( void*) n->key,( void*) n->data);(( void(*)( void(* f)(
void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*) n->right); goto
_LL128;} _LL128:;} void Cyc_SlowDict_iter( void(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d){(( void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,(
void*) d->tree);} static void Cyc_SlowDict_app_tree_c( void*(* f)( void*, void*,
void*), void* env, void* t){ void* _temp135= t; struct Cyc_Splay_noderef*
_temp141; _LL137: if( _temp135 ==( void*) Cyc_Splay_Leaf){ goto _LL138;} else{
goto _LL139;} _LL139: if(( unsigned int) _temp135 > 1u?*(( int*) _temp135) ==
Cyc_Splay_Node: 0){ _LL142: _temp141=(( struct Cyc_Splay_Node_struct*) _temp135)->f1;
goto _LL140;} else{ goto _LL136;} _LL138: goto _LL136; _LL140: { struct Cyc_Splay_node*
n= _temp141->v;(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->right); goto _LL136;} _LL136:;}
void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp143= t; struct Cyc_Splay_noderef*
_temp149; _LL145: if( _temp143 ==( void*) Cyc_Splay_Leaf){ goto _LL146;} else{
goto _LL147;} _LL147: if(( unsigned int) _temp143 > 1u?*(( int*) _temp143) ==
Cyc_Splay_Node: 0){ _LL150: _temp149=(( struct Cyc_Splay_Node_struct*) _temp143)->f1;
goto _LL148;} else{ goto _LL144;} _LL146: goto _LL144; _LL148: { struct Cyc_Splay_node*
n= _temp149->v;(( void(*)( void(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->right); goto _LL144;} _LL144:;}
void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*),
void* t){ void* _temp151= t; struct Cyc_Splay_noderef* _temp157; _LL153: if(
_temp151 ==( void*) Cyc_Splay_Leaf){ goto _LL154;} else{ goto _LL155;} _LL155:
if(( unsigned int) _temp151 > 1u?*(( int*) _temp151) == Cyc_Splay_Node: 0){
_LL158: _temp157=(( struct Cyc_Splay_Node_struct*) _temp151)->f1; goto _LL156;}
else{ goto _LL152;} _LL154: return( void*) Cyc_Splay_Leaf; _LL156: { struct Cyc_Splay_node*
_temp159= _temp157->v; goto _LL160; _LL160: return( void*)({ struct Cyc_Splay_Node_struct*
_temp161=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp161[ 0]=({ struct Cyc_Splay_Node_struct _temp162; _temp162.tag= Cyc_Splay_Node;
_temp162.f1=({ struct Cyc_Splay_noderef* _temp163=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp163->v=({ struct Cyc_Splay_node*
_temp164=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp164->key=( void*)(( void*) _temp159->key); _temp164->data=( void*) f(( void*)
_temp159->data); _temp164->left=( void*)(( void*(*)( void*(* f)( void*), void* t))
Cyc_SlowDict_map_tree)( f,( void*) _temp159->left); _temp164->right=( void*)((
void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,( void*)
_temp159->right); _temp164;}); _temp163;}); _temp162;}); _temp161;});} _LL152:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp165=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp165->reln= d->reln; _temp165->tree=(
void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,(
void*) d->tree); _temp165;});} static void* Cyc_SlowDict_map_tree_c( void*(* f)(
void*, void*), void* env, void* t){ void* _temp166= t; struct Cyc_Splay_noderef*
_temp172; _LL168: if( _temp166 ==( void*) Cyc_Splay_Leaf){ goto _LL169;} else{
goto _LL170;} _LL170: if(( unsigned int) _temp166 > 1u?*(( int*) _temp166) ==
Cyc_Splay_Node: 0){ _LL173: _temp172=(( struct Cyc_Splay_Node_struct*) _temp166)->f1;
goto _LL171;} else{ goto _LL167;} _LL169: return( void*) Cyc_Splay_Leaf; _LL171: {
struct Cyc_Splay_node* n= _temp172->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp174=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp174[ 0]=({ struct Cyc_Splay_Node_struct _temp175; _temp175.tag= Cyc_Splay_Node;
_temp175.f1=({ struct Cyc_Splay_noderef* _temp176=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp176->v=({ struct Cyc_Splay_node*
_temp177=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp177->key=( void*)(( void*) n->key); _temp177->data=( void*) f( env,( void*)
n->data); _temp177->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp177->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp177;}); _temp176;}); _temp175;}); _temp174;});}
_LL167:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp178=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp178->reln= d->reln; _temp178->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp178;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp179=( void*) d->tree; struct Cyc_Splay_noderef* _temp185; _LL181: if(
_temp179 ==( void*) Cyc_Splay_Leaf){ goto _LL182;} else{ goto _LL183;} _LL183:
if(( unsigned int) _temp179 > 1u?*(( int*) _temp179) == Cyc_Splay_Node: 0){
_LL186: _temp185=(( struct Cyc_Splay_Node_struct*) _temp179)->f1; goto _LL184;}
else{ goto _LL180;} _LL182:( int) _throw(( void*) Cyc_SlowDict_Absent); _LL184:
return({ struct _tuple0* _temp187=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp187->f1=( void*)( _temp185->v)->key; _temp187->f2=( void*)(
_temp185->v)->data; _temp187;}); _LL180:;} struct Cyc_List_List* Cyc_SlowDict_to_list_f(
void* k, void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List*
_temp188=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp188->hd=( void*)({ struct _tuple0* _temp189=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp189->f1= k; _temp189->f2= v; _temp189;});
_temp188->tl= accum; _temp188;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}