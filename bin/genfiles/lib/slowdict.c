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
extern const unsigned int Cyc_Splay_Leaf; extern const int Cyc_Splay_Node_tag;
struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ; struct
Cyc_Splay_node{ void* key; void* data; void* left; void* right; } ; extern int
Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); struct Cyc_SlowDict_Dict;
typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_hdict_t; typedef struct Cyc_SlowDict_Dict*
Cyc_SlowDict_dict_t; extern char Cyc_SlowDict_Present_tag[ 8u]; struct Cyc_SlowDict_Present_struct{
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
Cyc_SlowDict_empty( int(* comp)( void*, void*)){ void* t=( void*) Cyc_Splay_Leaf;
return({ struct Cyc_SlowDict_Dict* _temp0=( struct Cyc_SlowDict_Dict*) GC_malloc(
sizeof( struct Cyc_SlowDict_Dict)); _temp0->reln= comp; _temp0->tree=( void*) t;
_temp0;});} int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d){ void*
_temp1=( void*) d->tree; struct Cyc_Splay_noderef* _temp7; _LL3: if(( int)
_temp1 == Cyc_Splay_Leaf){ goto _LL4;} else{ goto _LL5;} _LL5: if(( unsigned int)
_temp1 > 1u?(( struct _tunion_struct*) _temp1)->tag == Cyc_Splay_Node_tag: 0){
_LL8: _temp7=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp1)->f1; goto _LL6;} else{ goto _LL2;} _LL4: return 1; _LL6: return 0; _LL2:;}
int Cyc_SlowDict_member( struct Cyc_SlowDict_Dict* d, void* key){ return(( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree);} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict*
d, void* key, void* data){ void* newleft=( void*) Cyc_Splay_Leaf; void* newright=(
void*) Cyc_Splay_Leaf; if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp9=( void*) d->tree; struct Cyc_Splay_noderef*
_temp15; _LL11: if(( unsigned int) _temp9 > 1u?(( struct _tunion_struct*) _temp9)->tag
== Cyc_Splay_Node_tag: 0){ _LL16: _temp15=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp9)->f1; goto _LL12;} else{ goto _LL13;} _LL13: goto
_LL14; _LL12: newleft=( void*)( _temp15->v)->left; newright=( void*)( _temp15->v)->right;
goto _LL10; _LL14: goto _LL10; _LL10:;} else{ void* _temp17=( void*) d->tree;
struct Cyc_Splay_noderef* _temp23; _LL19: if(( unsigned int) _temp17 > 1u?((
struct _tunion_struct*) _temp17)->tag == Cyc_Splay_Node_tag: 0){ _LL24: _temp23=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp17)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(( int) _temp17 == Cyc_Splay_Leaf){ goto
_LL22;} else{ goto _LL18;} _LL20: { struct Cyc_Splay_node* n= _temp23->v; if(( d->reln)(
key,( void*) n->key) < 0){ newleft=( void*) n->left; newright=( void*)({ struct
Cyc_Splay_Node_struct* _temp25=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp25[ 0]=({ struct Cyc_Splay_Node_struct
_temp26; _temp26.tag= Cyc_Splay_Node_tag; _temp26.f1=({ struct Cyc_Splay_noderef*
_temp27=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp27->v=({ struct Cyc_Splay_node* _temp28=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp28->key=( void*)(( void*) n->key); _temp28->data=(
void*)(( void*) n->data); _temp28->left=( void*)(( void*) Cyc_Splay_Leaf);
_temp28->right=( void*)(( void*) n->right); _temp28;}); _temp27;}); _temp26;});
_temp25;});} else{ newleft=( void*)({ struct Cyc_Splay_Node_struct* _temp29=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp29[ 0]=({ struct Cyc_Splay_Node_struct _temp30; _temp30.tag= Cyc_Splay_Node_tag;
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
_temp34[ 0]=({ struct Cyc_Splay_Node_struct _temp35; _temp35.tag= Cyc_Splay_Node_tag;
_temp35.f1=({ struct Cyc_Splay_noderef* _temp36=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp36->v=({ struct Cyc_Splay_node*
_temp37=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp37->key=( void*) key; _temp37->data=( void*) data; _temp37->left=( void*)
newleft; _temp37->right=( void*) newright; _temp37;}); _temp36;}); _temp35;});
_temp34;})); _temp33;});} struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict* d, void* key, void* data){ if((( int(*)( int(* f)(
void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_SlowDict_Present_struct*
_temp38=( struct Cyc_SlowDict_Present_struct*) GC_malloc( sizeof( struct Cyc_SlowDict_Present_struct));
_temp38[ 0]=({ struct Cyc_SlowDict_Present_struct _temp39; _temp39.tag= Cyc_SlowDict_Present_tag;
_temp39;}); _temp38;}));} return(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, void* key, void* data)) Cyc_SlowDict_insert)( d, key, data);} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_inserts( struct Cyc_SlowDict_Dict* d, struct Cyc_List_List* kds){
for( 0; kds != 0; kds= kds->tl){ d=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, void* key, void* data)) Cyc_SlowDict_insert)( d,(*(( struct _tuple0*) kds->hd)).f1,(*((
struct _tuple0*) kds->hd)).f2);} return d;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_singleton(
int(* comp)( void*, void*), void* key, void* data){ return({ struct Cyc_SlowDict_Dict*
_temp40=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp40->reln= comp; _temp40->tree=( void*)(( void*)({ struct Cyc_Splay_Node_struct*
_temp41=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp41[ 0]=({ struct Cyc_Splay_Node_struct _temp42; _temp42.tag= Cyc_Splay_Node_tag;
_temp42.f1=({ struct Cyc_Splay_noderef* _temp43=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp43->v=({ struct Cyc_Splay_node*
_temp44=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp44->key=( void*) key; _temp44->data=( void*) data; _temp44->left=( void*)((
void*) Cyc_Splay_Leaf); _temp44->right=( void*)(( void*) Cyc_Splay_Leaf);
_temp44;}); _temp43;}); _temp42;}); _temp41;})); _temp40;});} void* Cyc_SlowDict_lookup(
struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp45=(
void*) d->tree; struct Cyc_Splay_noderef* _temp51; _LL47: if(( unsigned int)
_temp45 > 1u?(( struct _tunion_struct*) _temp45)->tag == Cyc_Splay_Node_tag: 0){
_LL52: _temp51=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp45)->f1; goto _LL48;} else{ goto _LL49;} _LL49: if(( int) _temp45 == Cyc_Splay_Leaf){
goto _LL50;} else{ goto _LL46;} _LL48: return( void*)( _temp51->v)->data; _LL50:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp53=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp53[ 0]=({ struct Cyc_Core_Impossible_struct _temp54; _temp54.tag= Cyc_Core_Impossible_tag;
_temp54.f1=( struct _tagged_string)({ char* _temp55=( char*)"Dict::lookup";
struct _tagged_string _temp56; _temp56.curr= _temp55; _temp56.base= _temp55;
_temp56.last_plus_one= _temp55 + 13; _temp56;}); _temp54;}); _temp53;})); _LL46:;}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct*
_temp57=( struct Cyc_SlowDict_Absent_struct*) GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct));
_temp57[ 0]=({ struct Cyc_SlowDict_Absent_struct _temp58; _temp58.tag= Cyc_SlowDict_Absent_tag;
_temp58;}); _temp57;}));} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct
Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*), void*,
void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp59=( void*)
d->tree; struct Cyc_Splay_noderef* _temp65; _LL61: if(( unsigned int) _temp59 >
1u?(( struct _tunion_struct*) _temp59)->tag == Cyc_Splay_Node_tag: 0){ _LL66:
_temp65=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp59)->f1;
goto _LL62;} else{ goto _LL63;} _LL63: if(( int) _temp59 == Cyc_Splay_Leaf){
goto _LL64;} else{ goto _LL60;} _LL62: return({ struct Cyc_Core_Opt* _temp67=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp67->v=(
void*)(( void*)( _temp65->v)->data); _temp67;}); _LL64:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp68=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp68[ 0]=({ struct
Cyc_Core_Impossible_struct _temp69; _temp69.tag= Cyc_Core_Impossible_tag;
_temp69.f1=( struct _tagged_string)({ char* _temp70=( char*)"Dict::lookup";
struct _tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70;
_temp71.last_plus_one= _temp70 + 13; _temp71;}); _temp69;}); _temp68;})); _LL60:;}
return 0;} static int Cyc_SlowDict_get_largest( void* x, void* y){ return 1;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void*
key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln,
key,( void*) d->tree)){ void* _temp72=( void*) d->tree; struct Cyc_Splay_noderef*
_temp78; _LL74: if(( int) _temp72 == Cyc_Splay_Leaf){ goto _LL75;} else{ goto
_LL76;} _LL76: if(( unsigned int) _temp72 > 1u?(( struct _tunion_struct*)
_temp72)->tag == Cyc_Splay_Node_tag: 0){ _LL79: _temp78=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp72)->f1; goto _LL77;} else{ goto _LL73;}
_LL75:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp80=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp80[ 0]=({ struct Cyc_Core_Impossible_struct _temp81; _temp81.tag= Cyc_Core_Impossible_tag;
_temp81.f1=( struct _tagged_string)({ char* _temp82=( char*)"Dict::lookup";
struct _tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82;
_temp83.last_plus_one= _temp82 + 13; _temp83;}); _temp81;}); _temp80;})); _LL77: {
struct Cyc_Splay_node* n= _temp78->v; void* _temp84=( void*) n->left; struct Cyc_Splay_noderef*
_temp90; _LL86: if(( int) _temp84 == Cyc_Splay_Leaf){ goto _LL87;} else{ goto
_LL88;} _LL88: if(( unsigned int) _temp84 > 1u?(( struct _tunion_struct*)
_temp84)->tag == Cyc_Splay_Node_tag: 0){ _LL91: _temp90=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp84)->f1; goto _LL89;} else{ goto _LL85;}
_LL87: return({ struct Cyc_SlowDict_Dict* _temp92=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp92->reln= d->reln; _temp92->tree=(
void*)(( void*) n->right); _temp92;}); _LL89: { void* _temp93=( void*) n->right;
struct Cyc_Splay_noderef* _temp99; _LL95: if(( int) _temp93 == Cyc_Splay_Leaf){
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp93 > 1u?(( struct
_tunion_struct*) _temp93)->tag == Cyc_Splay_Node_tag: 0){ _LL100: _temp99=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp93)->f1; goto
_LL98;} else{ goto _LL94;} _LL96: return({ struct Cyc_SlowDict_Dict* _temp101=(
struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp101->reln= d->reln; _temp101->tree=( void*)(( void*) n->left); _temp101;});
_LL98:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp90->v; return({ struct Cyc_SlowDict_Dict* _temp102=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp102->reln= d->reln; _temp102->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp103=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp103[ 0]=({ struct Cyc_Splay_Node_struct
_temp104; _temp104.tag= Cyc_Splay_Node_tag; _temp104.f1=({ struct Cyc_Splay_noderef*
_temp105=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp105->v=({ struct Cyc_Splay_node* _temp106=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp106->key=( void*)(( void*)
newtop->key); _temp106->data=( void*)(( void*) newtop->data); _temp106->left=(
void*)(( void*) newtop->left); _temp106->right=( void*)(( void*) n->right);
_temp106;}); _temp105;}); _temp104;}); _temp103;})); _temp102;});} _LL94:;}
_LL85:;} _LL73:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* d2=(( struct
Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); if( d == d2){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct*
_temp107=( struct Cyc_SlowDict_Absent_struct*) GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct));
_temp107[ 0]=({ struct Cyc_SlowDict_Absent_struct _temp108; _temp108.tag= Cyc_SlowDict_Absent_tag;
_temp108;}); _temp107;}));} return d2;} static void* Cyc_SlowDict_fold_tree(
void*(* f)( void*, void*, void*), void* t, void* accum){ void* _temp109= t;
struct Cyc_Splay_noderef* _temp115; _LL111: if(( int) _temp109 == Cyc_Splay_Leaf){
goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp109 > 1u?((
struct _tunion_struct*) _temp109)->tag == Cyc_Splay_Node_tag: 0){ _LL116:
_temp115=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp109)->f1;
goto _LL114;} else{ goto _LL110;} _LL112: return accum; _LL114: { struct Cyc_Splay_node*
n= _temp115->v; return f(( void*) n->key,( void*) n->data,(( void*(*)( void*(* f)(
void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)( f,( void*)
n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum))
Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL110:;} void* Cyc_SlowDict_fold(
void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum){
return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c( void*(* f)(
void*, void*, void*, void*), void* env, void* t, void* accum){ void* _temp117= t;
struct Cyc_Splay_noderef* _temp123; _LL119: if(( int) _temp117 == Cyc_Splay_Leaf){
goto _LL120;} else{ goto _LL121;} _LL121: if(( unsigned int) _temp117 > 1u?((
struct _tunion_struct*) _temp117)->tag == Cyc_Splay_Node_tag: 0){ _LL124:
_temp123=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp117)->f1;
goto _LL122;} else{ goto _LL118;} _LL120: return accum; _LL122: { struct Cyc_Splay_node*
n= _temp123->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL118:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp125= t; struct Cyc_Splay_noderef*
_temp131; _LL127: if(( int) _temp125 == Cyc_Splay_Leaf){ goto _LL128;} else{
goto _LL129;} _LL129: if(( unsigned int) _temp125 > 1u?(( struct _tunion_struct*)
_temp125)->tag == Cyc_Splay_Node_tag: 0){ _LL132: _temp131=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp125)->f1; goto _LL130;} else{ goto _LL126;}
_LL128: goto _LL126; _LL130: { struct Cyc_Splay_node* n= _temp131->v;(( void(*)(
void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) n->left);
f(( void*) n->key,( void*) n->data);(( void(*)( void*(* f)( void*, void*), void*
t)) Cyc_SlowDict_app_tree)( f,( void*) n->right); goto _LL126;} _LL126:;} void
Cyc_SlowDict_app( void*(* f)( void*, void*), struct Cyc_SlowDict_Dict* d){((
void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*)
d->tree);} static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*), void* t){
void* _temp133= t; struct Cyc_Splay_noderef* _temp139; _LL135: if(( int)
_temp133 == Cyc_Splay_Leaf){ goto _LL136;} else{ goto _LL137;} _LL137: if((
unsigned int) _temp133 > 1u?(( struct _tunion_struct*) _temp133)->tag == Cyc_Splay_Node_tag:
0){ _LL140: _temp139=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp133)->f1; goto _LL138;} else{ goto _LL134;} _LL136: goto _LL134; _LL138: {
struct Cyc_Splay_node* n= _temp139->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL134;} _LL134:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp141= t; struct
Cyc_Splay_noderef* _temp147; _LL143: if(( int) _temp141 == Cyc_Splay_Leaf){ goto
_LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?(( struct
_tunion_struct*) _temp141)->tag == Cyc_Splay_Node_tag: 0){ _LL148: _temp147=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp141)->f1; goto
_LL146;} else{ goto _LL142;} _LL144: goto _LL142; _LL146: { struct Cyc_Splay_node*
n= _temp147->v;(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->right); goto _LL142;} _LL142:;}
void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp149= t; struct Cyc_Splay_noderef*
_temp155; _LL151: if(( int) _temp149 == Cyc_Splay_Leaf){ goto _LL152;} else{
goto _LL153;} _LL153: if(( unsigned int) _temp149 > 1u?(( struct _tunion_struct*)
_temp149)->tag == Cyc_Splay_Node_tag: 0){ _LL156: _temp155=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp149)->f1; goto _LL154;} else{ goto _LL150;}
_LL152: goto _LL150; _LL154: { struct Cyc_Splay_node* n= _temp155->v;(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->right); goto _LL150;} _LL150:;} void Cyc_SlowDict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*),
void* t){ void* _temp157= t; struct Cyc_Splay_noderef* _temp163; _LL159: if((
int) _temp157 == Cyc_Splay_Leaf){ goto _LL160;} else{ goto _LL161;} _LL161: if((
unsigned int) _temp157 > 1u?(( struct _tunion_struct*) _temp157)->tag == Cyc_Splay_Node_tag:
0){ _LL164: _temp163=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp157)->f1; goto _LL162;} else{ goto _LL158;} _LL160: return( void*) Cyc_Splay_Leaf;
_LL162: { struct Cyc_Splay_node* n= _temp163->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp165=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp165[ 0]=({ struct Cyc_Splay_Node_struct _temp166; _temp166.tag= Cyc_Splay_Node_tag;
_temp166.f1=({ struct Cyc_Splay_noderef* _temp167=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp167->v=({ struct Cyc_Splay_node*
_temp168=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp168->key=( void*)(( void*) n->key); _temp168->data=( void*) f(( void*) n->data);
_temp168->left=( void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)(
f,( void*) n->left); _temp168->right=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) n->right); _temp168;}); _temp167;});
_temp166;}); _temp165;});} _LL158:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map(
void*(* f)( void*), struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp169=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp169->reln= d->reln; _temp169->tree=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) d->tree); _temp169;});} static void*
Cyc_SlowDict_map_tree_c( void*(* f)( void*, void*), void* env, void* t){ void*
_temp170= t; struct Cyc_Splay_noderef* _temp176; _LL172: if(( int) _temp170 ==
Cyc_Splay_Leaf){ goto _LL173;} else{ goto _LL174;} _LL174: if(( unsigned int)
_temp170 > 1u?(( struct _tunion_struct*) _temp170)->tag == Cyc_Splay_Node_tag: 0){
_LL177: _temp176=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp170)->f1; goto _LL175;} else{ goto _LL171;} _LL173: return( void*) Cyc_Splay_Leaf;
_LL175: { struct Cyc_Splay_node* n= _temp176->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp178=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp178[ 0]=({ struct Cyc_Splay_Node_struct _temp179; _temp179.tag= Cyc_Splay_Node_tag;
_temp179.f1=({ struct Cyc_Splay_noderef* _temp180=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp180->v=({ struct Cyc_Splay_node*
_temp181=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp181->key=( void*)(( void*) n->key); _temp181->data=( void*) f( env,( void*)
n->data); _temp181->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp181->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp181;}); _temp180;}); _temp179;}); _temp178;});}
_LL171:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp182=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp182->reln= d->reln; _temp182->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp182;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp183=( void*) d->tree; struct Cyc_Splay_noderef* _temp189; _LL185: if((
int) _temp183 == Cyc_Splay_Leaf){ goto _LL186;} else{ goto _LL187;} _LL187: if((
unsigned int) _temp183 > 1u?(( struct _tunion_struct*) _temp183)->tag == Cyc_Splay_Node_tag:
0){ _LL190: _temp189=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp183)->f1; goto _LL188;} else{ goto _LL184;} _LL186:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct* _temp191=( struct Cyc_SlowDict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct)); _temp191[ 0]=({ struct
Cyc_SlowDict_Absent_struct _temp192; _temp192.tag= Cyc_SlowDict_Absent_tag;
_temp192;}); _temp191;})); _LL188: return({ struct _tuple0* _temp193=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp193->f1=( void*)( _temp189->v)->key;
_temp193->f2=( void*)( _temp189->v)->data; _temp193;}); _LL184:;} struct Cyc_List_List*
Cyc_SlowDict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp194=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp194->hd=( void*)({ struct _tuple0* _temp195=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp195->f1= k; _temp195->f2=
v; _temp195;}); _temp194->tl= accum; _temp194;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}