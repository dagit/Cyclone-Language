#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; typedef unsigned int Cyc_uint; typedef
char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Splay_node; struct
Cyc_Splay_noderef{ struct Cyc_Splay_node* v; } ; typedef void* Cyc_Splay_tree;
extern void* Cyc_Splay_Leaf; extern const int Cyc_Splay_Node_tag; struct Cyc_Splay_Node_struct{
int tag; struct Cyc_Splay_noderef* f1; } ; struct Cyc_Splay_node{ void* key;
void* data; void* left; void* right; } ; extern int Cyc_Splay_splay( int(* f)(
void*, void*), void*, void*); struct Cyc_SlowDict_Dict; typedef struct Cyc_SlowDict_Dict*
Cyc_SlowDict_hdict_t; typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_dict_t;
extern char Cyc_SlowDict_Present_tag[ 8u]; struct Cyc_SlowDict_Present_struct{
char* tag; } ; extern char Cyc_SlowDict_Absent_tag[ 7u]; struct Cyc_SlowDict_Absent_struct{
char* tag; } ; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)(
void*, void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d);
extern int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict* d, void* key); extern
struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void*
key, void* data); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
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
d); extern struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d);
extern struct Cyc_List_List* Cyc_SlowDict_to_list( struct Cyc_SlowDict_Dict* d);
char Cyc_SlowDict_Absent_tag[ 7u]="Absent"; char Cyc_SlowDict_Present_tag[ 8u]="Present";
struct Cyc_SlowDict_Dict{ int(* reln)( void*, void*); void* tree; } ; struct Cyc_SlowDict_Dict*
Cyc_SlowDict_empty( int(* comp)( void*, void*)){ void* t= Cyc_Splay_Leaf; return({
struct Cyc_SlowDict_Dict* _temp0=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof(
struct Cyc_SlowDict_Dict)); _temp0->reln= comp; _temp0->tree=( void*) t; _temp0;});}
int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d){ void* _temp1=( void*) d->tree;
struct Cyc_Splay_noderef* _temp7; _LL3: if( _temp1 == Cyc_Splay_Leaf){ goto _LL4;}
else{ goto _LL5;} _LL5: if(( unsigned int) _temp1 > 1u?(( struct _enum_struct*)
_temp1)->tag == Cyc_Splay_Node_tag: 0){ _LL8: _temp7=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp1)->f1; goto _LL6;} else{ goto _LL2;} _LL4:
return 1; _LL6: return 0; _LL2:;} int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict*
d, void* key){ return(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ void* newleft= Cyc_Splay_Leaf;
void* newright= Cyc_Splay_Leaf; if((( int(*)( int(* f)( void*, void*), void*,
void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp9=( void*)
d->tree; struct Cyc_Splay_noderef* _temp15; _LL11: if(( unsigned int) _temp9 > 1u?((
struct _enum_struct*) _temp9)->tag == Cyc_Splay_Node_tag: 0){ _LL16: _temp15=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp9)->f1; goto
_LL12;} else{ goto _LL13;} _LL13: goto _LL14; _LL12: newleft=( void*)( _temp15->v)->left;
newright=( void*)( _temp15->v)->right; goto _LL10; _LL14: goto _LL10; _LL10:;}
else{ void* _temp17=( void*) d->tree; struct Cyc_Splay_noderef* _temp23; _LL19:
if(( unsigned int) _temp17 > 1u?(( struct _enum_struct*) _temp17)->tag == Cyc_Splay_Node_tag:
0){ _LL24: _temp23=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp17)->f1; goto _LL20;} else{ goto _LL21;} _LL21: if( _temp17 == Cyc_Splay_Leaf){
goto _LL22;} else{ goto _LL18;} _LL20: { struct Cyc_Splay_node* n= _temp23->v;
if(( d->reln)( key,( void*) n->key) < 0){ newleft=( void*) n->left; newright=({
struct Cyc_Splay_Node_struct* _temp25=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp25->tag= Cyc_Splay_Node_tag;
_temp25->f1=({ struct Cyc_Splay_noderef* _temp26=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp26->v=({ struct Cyc_Splay_node*
_temp27=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp27->key=( void*)(( void*) n->key); _temp27->data=( void*)(( void*) n->data);
_temp27->left=( void*) Cyc_Splay_Leaf; _temp27->right=( void*)(( void*) n->right);
_temp27;}); _temp26;});( void*) _temp25;});} else{ newleft=({ struct Cyc_Splay_Node_struct*
_temp28=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp28->tag= Cyc_Splay_Node_tag; _temp28->f1=({ struct Cyc_Splay_noderef*
_temp29=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp29->v=({ struct Cyc_Splay_node* _temp30=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp30->key=( void*)(( void*) n->key); _temp30->data=(
void*)(( void*) n->data); _temp30->left=( void*)(( void*) n->left); _temp30->right=(
void*) Cyc_Splay_Leaf; _temp30;}); _temp29;});( void*) _temp28;}); newright=(
void*) n->right;} goto _LL18;} _LL22: goto _LL18; _LL18:;} return({ struct Cyc_SlowDict_Dict*
_temp31=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp31->reln= d->reln; _temp31->tree=( void*)({ struct Cyc_Splay_Node_struct*
_temp32=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp32->tag= Cyc_Splay_Node_tag; _temp32->f1=({ struct Cyc_Splay_noderef*
_temp33=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp33->v=({ struct Cyc_Splay_node* _temp34=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp34->key=( void*) key; _temp34->data=( void*)
data; _temp34->left=( void*) newleft; _temp34->right=( void*) newright; _temp34;});
_temp33;});( void*) _temp32;}); _temp31;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if((( int(*)( int(* f)(
void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){(
void) _throw(({ struct Cyc_SlowDict_Present_struct* _temp35=( struct Cyc_SlowDict_Present_struct*)
GC_malloc_atomic( sizeof( struct Cyc_SlowDict_Present_struct)); _temp35->tag=
Cyc_SlowDict_Present_tag;( struct _xenum_struct*) _temp35;}));} return(( struct
Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)(
d, key, data);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict*
d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds= kds->tl){ d=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)( d,(*((
struct _tuple0*) kds->hd)).f1,(*(( struct _tuple0*) kds->hd)).f2);} return d;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_singleton( int(* comp)( void*, void*),
void* key, void* data){ return({ struct Cyc_SlowDict_Dict* _temp36=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp36->reln= comp; _temp36->tree=(
void*)({ struct Cyc_Splay_Node_struct* _temp37=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp37->tag= Cyc_Splay_Node_tag;
_temp37->f1=({ struct Cyc_Splay_noderef* _temp38=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp38->v=({ struct Cyc_Splay_node*
_temp39=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp39->key=( void*) key; _temp39->data=( void*) data; _temp39->left=( void*)
Cyc_Splay_Leaf; _temp39->right=( void*) Cyc_Splay_Leaf; _temp39;}); _temp38;});(
void*) _temp37;}); _temp36;});} void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp40=( void*) d->tree; struct Cyc_Splay_noderef*
_temp46; _LL42: if(( unsigned int) _temp40 > 1u?(( struct _enum_struct*) _temp40)->tag
== Cyc_Splay_Node_tag: 0){ _LL47: _temp46=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp40)->f1; goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp40 == Cyc_Splay_Leaf){ goto _LL45;} else{ goto _LL41;} _LL43: return( void*)(
_temp46->v)->data; _LL45:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp48=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp48->tag= Cyc_Core_Impossible_tag; _temp48->f1=( struct _tagged_string)({
char* _temp49=( char*)"Dict::lookup"; struct _tagged_string _temp50; _temp50.curr=
_temp49; _temp50.base= _temp49; _temp50.last_plus_one= _temp49 + 13; _temp50;});(
struct _xenum_struct*) _temp48;})); _LL41:;}( void) _throw(({ struct Cyc_SlowDict_Absent_struct*
_temp51=( struct Cyc_SlowDict_Absent_struct*) GC_malloc_atomic( sizeof( struct
Cyc_SlowDict_Absent_struct)); _temp51->tag= Cyc_SlowDict_Absent_tag;( struct
_xenum_struct*) _temp51;}));} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp52=(
void*) d->tree; struct Cyc_Splay_noderef* _temp58; _LL54: if(( unsigned int)
_temp52 > 1u?(( struct _enum_struct*) _temp52)->tag == Cyc_Splay_Node_tag: 0){
_LL59: _temp58=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp52)->f1; goto _LL55;} else{ goto _LL56;} _LL56: if( _temp52 == Cyc_Splay_Leaf){
goto _LL57;} else{ goto _LL53;} _LL55: return({ struct Cyc_Core_Opt* _temp60=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp60->v=(
void*)(( void*)( _temp58->v)->data); _temp60;}); _LL57:( void) _throw(({ struct
Cyc_Core_Impossible_struct* _temp61=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp61->tag= Cyc_Core_Impossible_tag;
_temp61->f1=( struct _tagged_string)({ char* _temp62=( char*)"Dict::lookup";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 13; _temp63;});( struct _xenum_struct*) _temp61;}));
_LL53:;} return 0;} static int Cyc_SlowDict_get_largest( void* x, void* y){
return 1;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp64=( void*) d->tree; struct Cyc_Splay_noderef*
_temp70; _LL66: if( _temp64 == Cyc_Splay_Leaf){ goto _LL67;} else{ goto _LL68;}
_LL68: if(( unsigned int) _temp64 > 1u?(( struct _enum_struct*) _temp64)->tag ==
Cyc_Splay_Node_tag: 0){ _LL71: _temp70=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp64)->f1; goto _LL69;} else{ goto _LL65;} _LL67:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp72=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp72->tag= Cyc_Core_Impossible_tag; _temp72->f1=( struct _tagged_string)({
char* _temp73=( char*)"Dict::lookup"; struct _tagged_string _temp74; _temp74.curr=
_temp73; _temp74.base= _temp73; _temp74.last_plus_one= _temp73 + 13; _temp74;});(
struct _xenum_struct*) _temp72;})); _LL69: { struct Cyc_Splay_node* n= _temp70->v;
void* _temp75=( void*) n->left; struct Cyc_Splay_noderef* _temp81; _LL77: if(
_temp75 == Cyc_Splay_Leaf){ goto _LL78;} else{ goto _LL79;} _LL79: if((
unsigned int) _temp75 > 1u?(( struct _enum_struct*) _temp75)->tag == Cyc_Splay_Node_tag:
0){ _LL82: _temp81=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp75)->f1; goto _LL80;} else{ goto _LL76;} _LL78: return({ struct Cyc_SlowDict_Dict*
_temp83=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp83->reln= d->reln; _temp83->tree=( void*)(( void*) n->right); _temp83;});
_LL80: { void* _temp84=( void*) n->right; struct Cyc_Splay_noderef* _temp90;
_LL86: if( _temp84 == Cyc_Splay_Leaf){ goto _LL87;} else{ goto _LL88;} _LL88:
if(( unsigned int) _temp84 > 1u?(( struct _enum_struct*) _temp84)->tag == Cyc_Splay_Node_tag:
0){ _LL91: _temp90=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp84)->f1; goto _LL89;} else{ goto _LL85;} _LL87: return({ struct Cyc_SlowDict_Dict*
_temp92=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp92->reln= d->reln; _temp92->tree=( void*)(( void*) n->left); _temp92;});
_LL89:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp81->v; return({ struct Cyc_SlowDict_Dict* _temp93=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp93->reln= d->reln; _temp93->tree=(
void*)({ struct Cyc_Splay_Node_struct* _temp94=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp94->tag= Cyc_Splay_Node_tag;
_temp94->f1=({ struct Cyc_Splay_noderef* _temp95=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp95->v=({ struct Cyc_Splay_node*
_temp96=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp96->key=( void*)(( void*) newtop->key); _temp96->data=( void*)(( void*)
newtop->data); _temp96->left=( void*)(( void*) newtop->left); _temp96->right=(
void*)(( void*) n->right); _temp96;}); _temp95;});( void*) _temp94;}); _temp93;});}
_LL85:;} _LL76:;} _LL65:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* d2=(( struct
Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); if( d == d2){( void) _throw(({ struct Cyc_SlowDict_Absent_struct*
_temp97=( struct Cyc_SlowDict_Absent_struct*) GC_malloc_atomic( sizeof( struct
Cyc_SlowDict_Absent_struct)); _temp97->tag= Cyc_SlowDict_Absent_tag;( struct
_xenum_struct*) _temp97;}));} return d2;} static void* Cyc_SlowDict_fold_tree(
void*(* f)( void*, void*, void*), void* t, void* accum){ void* _temp98= t;
struct Cyc_Splay_noderef* _temp104; _LL100: if( _temp98 == Cyc_Splay_Leaf){ goto
_LL101;} else{ goto _LL102;} _LL102: if(( unsigned int) _temp98 > 1u?(( struct
_enum_struct*) _temp98)->tag == Cyc_Splay_Node_tag: 0){ _LL105: _temp104=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp98)->f1; goto
_LL103;} else{ goto _LL99;} _LL101: return accum; _LL103: { struct Cyc_Splay_node*
n= _temp104->v; return f(( void*) n->key,( void*) n->data,(( void*(*)( void*(* f)(
void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)( f,( void*)
n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum))
Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL99:;} void* Cyc_SlowDict_fold(
void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum){
return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c( void*(* f)(
void*, void*, void*, void*), void* env, void* t, void* accum){ void* _temp106= t;
struct Cyc_Splay_noderef* _temp112; _LL108: if( _temp106 == Cyc_Splay_Leaf){
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp106 > 1u?((
struct _enum_struct*) _temp106)->tag == Cyc_Splay_Node_tag: 0){ _LL113: _temp112=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp106)->f1; goto
_LL111;} else{ goto _LL107;} _LL109: return accum; _LL111: { struct Cyc_Splay_node*
n= _temp112->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL107:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp114= t; struct Cyc_Splay_noderef*
_temp120; _LL116: if( _temp114 == Cyc_Splay_Leaf){ goto _LL117;} else{ goto
_LL118;} _LL118: if(( unsigned int) _temp114 > 1u?(( struct _enum_struct*)
_temp114)->tag == Cyc_Splay_Node_tag: 0){ _LL121: _temp120=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp114)->f1; goto _LL119;} else{ goto _LL115;}
_LL117: goto _LL115; _LL119: { struct Cyc_Splay_node* n= _temp120->v;(( void(*)(
void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) n->left);
f(( void*) n->key,( void*) n->data);(( void(*)( void*(* f)( void*, void*), void*
t)) Cyc_SlowDict_app_tree)( f,( void*) n->right); goto _LL115;} _LL115:;} void
Cyc_SlowDict_app( void*(* f)( void*, void*), struct Cyc_SlowDict_Dict* d){((
void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*)
d->tree);} static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*), void* t){
void* _temp122= t; struct Cyc_Splay_noderef* _temp128; _LL124: if( _temp122 ==
Cyc_Splay_Leaf){ goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp122 > 1u?(( struct _enum_struct*) _temp122)->tag == Cyc_Splay_Node_tag: 0){
_LL129: _temp128=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp122)->f1; goto _LL127;} else{ goto _LL123;} _LL125: goto _LL123; _LL127: {
struct Cyc_Splay_node* n= _temp128->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL123;} _LL123:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp130= t; struct
Cyc_Splay_noderef* _temp136; _LL132: if( _temp130 == Cyc_Splay_Leaf){ goto
_LL133;} else{ goto _LL134;} _LL134: if(( unsigned int) _temp130 > 1u?(( struct
_enum_struct*) _temp130)->tag == Cyc_Splay_Node_tag: 0){ _LL137: _temp136=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp130)->f1; goto
_LL135;} else{ goto _LL131;} _LL133: goto _LL131; _LL135: { struct Cyc_Splay_node*
n= _temp136->v;(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->right); goto _LL131;} _LL131:;}
void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp138= t; struct Cyc_Splay_noderef*
_temp144; _LL140: if( _temp138 == Cyc_Splay_Leaf){ goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp138 > 1u?(( struct _enum_struct*)
_temp138)->tag == Cyc_Splay_Node_tag: 0){ _LL145: _temp144=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp138)->f1; goto _LL143;} else{ goto _LL139;}
_LL141: goto _LL139; _LL143: { struct Cyc_Splay_node* n= _temp144->v;(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->right); goto _LL139;} _LL139:;} void Cyc_SlowDict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*),
void* t){ void* _temp146= t; struct Cyc_Splay_noderef* _temp152; _LL148: if(
_temp146 == Cyc_Splay_Leaf){ goto _LL149;} else{ goto _LL150;} _LL150: if((
unsigned int) _temp146 > 1u?(( struct _enum_struct*) _temp146)->tag == Cyc_Splay_Node_tag:
0){ _LL153: _temp152=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp146)->f1; goto _LL151;} else{ goto _LL147;} _LL149: return Cyc_Splay_Leaf;
_LL151: { struct Cyc_Splay_node* n= _temp152->v; return({ struct Cyc_Splay_Node_struct*
_temp154=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp154->tag= Cyc_Splay_Node_tag; _temp154->f1=({ struct Cyc_Splay_noderef*
_temp155=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp155->v=({ struct Cyc_Splay_node* _temp156=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp156->key=( void*)(( void*) n->key);
_temp156->data=( void*) f(( void*) n->data); _temp156->left=( void*)(( void*(*)(
void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,( void*) n->left);
_temp156->right=( void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)(
f,( void*) n->right); _temp156;}); _temp155;});( void*) _temp154;});} _LL147:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp157=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp157->reln= d->reln; _temp157->tree=(
void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,(
void*) d->tree); _temp157;});} static void* Cyc_SlowDict_map_tree_c( void*(* f)(
void*, void*), void* env, void* t){ void* _temp158= t; struct Cyc_Splay_noderef*
_temp164; _LL160: if( _temp158 == Cyc_Splay_Leaf){ goto _LL161;} else{ goto
_LL162;} _LL162: if(( unsigned int) _temp158 > 1u?(( struct _enum_struct*)
_temp158)->tag == Cyc_Splay_Node_tag: 0){ _LL165: _temp164=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp158)->f1; goto _LL163;} else{ goto _LL159;}
_LL161: return Cyc_Splay_Leaf; _LL163: { struct Cyc_Splay_node* n= _temp164->v;
return({ struct Cyc_Splay_Node_struct* _temp166=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp166->tag= Cyc_Splay_Node_tag;
_temp166->f1=({ struct Cyc_Splay_noderef* _temp167=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp167->v=({ struct Cyc_Splay_node*
_temp168=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp168->key=( void*)(( void*) n->key); _temp168->data=( void*) f( env,( void*)
n->data); _temp168->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp168->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp168;}); _temp167;});( void*) _temp166;});}
_LL159:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp169=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp169->reln= d->reln; _temp169->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp169;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp170=( void*) d->tree; struct Cyc_Splay_noderef* _temp176; _LL172: if(
_temp170 == Cyc_Splay_Leaf){ goto _LL173;} else{ goto _LL174;} _LL174: if((
unsigned int) _temp170 > 1u?(( struct _enum_struct*) _temp170)->tag == Cyc_Splay_Node_tag:
0){ _LL177: _temp176=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp170)->f1; goto _LL175;} else{ goto _LL171;} _LL173:( void) _throw(({ struct
Cyc_SlowDict_Absent_struct* _temp178=( struct Cyc_SlowDict_Absent_struct*)
GC_malloc_atomic( sizeof( struct Cyc_SlowDict_Absent_struct)); _temp178->tag=
Cyc_SlowDict_Absent_tag;( struct _xenum_struct*) _temp178;})); _LL175: return({
struct _tuple0* _temp179=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp179->f1=( void*)( _temp176->v)->key; _temp179->f2=( void*)( _temp176->v)->data;
_temp179;}); _LL171:;} struct Cyc_List_List* Cyc_SlowDict_to_list_f( void* k,
void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp180=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp180->hd=(
void*)({ struct _tuple0* _temp181=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp181->f1= k; _temp181->f2= v; _temp181;}); _temp180->tl= accum;
_temp180;});} struct Cyc_List_List* Cyc_SlowDict_to_list( struct Cyc_SlowDict_Dict*
d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( void*, void*,
struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct Cyc_List_List* accum))
Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k, void* v, struct Cyc_List_List*
accum)) Cyc_SlowDict_to_list_f, d, 0);}