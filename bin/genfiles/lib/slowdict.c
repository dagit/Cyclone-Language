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
goto _LL22;} else{ goto _LL18;} _LL20: { struct Cyc_Splay_node* _temp25= _temp23->v;
goto _LL26; _LL26: if(( d->reln)( key,( void*) _temp25->key) < 0){ newleft=(
void*) _temp25->left; newright=( void*)({ struct Cyc_Splay_Node_struct* _temp27=(
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
d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=({ struct Cyc_List_List*
_temp40= kds; if( _temp40 == 0){ _throw( Null_Exception);} _temp40->tl;})){ d=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key, void* data))
Cyc_SlowDict_insert)( d,(*(( struct _tuple0*)({ struct Cyc_List_List* _temp41=
kds; if( _temp41 == 0){ _throw( Null_Exception);} _temp41->hd;}))).f1,(*((
struct _tuple0*)({ struct Cyc_List_List* _temp42= kds; if( _temp42 == 0){ _throw(
Null_Exception);} _temp42->hd;}))).f2);} return d;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton( int(* comp)( void*, void*), void* key, void* data){
return({ struct Cyc_SlowDict_Dict* _temp43=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp43->reln= comp; _temp43->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp44=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp44[ 0]=({ struct Cyc_Splay_Node_struct
_temp45; _temp45.tag= Cyc_Splay_Node; _temp45.f1=({ struct Cyc_Splay_noderef*
_temp46=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp46->v=({ struct Cyc_Splay_node* _temp47=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp47->key=( void*) key; _temp47->data=( void*)
data; _temp47->left=( void*)(( void*) Cyc_Splay_Leaf); _temp47->right=( void*)((
void*) Cyc_Splay_Leaf); _temp47;}); _temp46;}); _temp45;}); _temp44;})); _temp43;});}
void* Cyc_SlowDict_lookup( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)(
int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*)
d->tree)){ void* _temp48=( void*) d->tree; struct Cyc_Splay_noderef* _temp54;
_LL50: if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Splay_Node: 0){
_LL55: _temp54=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp48)->f1; goto _LL51;} else{ goto _LL52;} _LL52: if( _temp48 ==( void*) Cyc_Splay_Leaf){
goto _LL53;} else{ goto _LL49;} _LL51: return( void*)( _temp54->v)->data; _LL53:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp56=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp56[ 0]=({ struct
Cyc_Core_Impossible_struct _temp57; _temp57.tag= Cyc_Core_Impossible; _temp57.f1=(
struct _tagged_string)({ char* _temp58=( char*)"Dict::lookup"; struct
_tagged_string _temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 13; _temp59;}); _temp57;}); _temp56;})); _LL49:;}( void) _throw(( void*)
Cyc_SlowDict_Absent);} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct Cyc_SlowDict_Dict*
d, void* key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(
d->reln, key,( void*) d->tree)){ void* _temp60=( void*) d->tree; struct Cyc_Splay_noderef*
_temp66; _LL62: if(( unsigned int) _temp60 > 1u?*(( int*) _temp60) == Cyc_Splay_Node:
0){ _LL67: _temp66=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp60)->f1; goto _LL63;} else{ goto _LL64;} _LL64: if( _temp60 ==( void*) Cyc_Splay_Leaf){
goto _LL65;} else{ goto _LL61;} _LL63: return({ struct Cyc_Core_Opt* _temp68=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp68->v=(
void*)(( void*)( _temp66->v)->data); _temp68;}); _LL65:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp69=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp69[ 0]=({ struct
Cyc_Core_Impossible_struct _temp70; _temp70.tag= Cyc_Core_Impossible; _temp70.f1=(
struct _tagged_string)({ char* _temp71=( char*)"Dict::lookup"; struct
_tagged_string _temp72; _temp72.curr= _temp71; _temp72.base= _temp71; _temp72.last_plus_one=
_temp71 + 13; _temp72;}); _temp70;}); _temp69;})); _LL61:;} return 0;} static
int Cyc_SlowDict_get_largest( void* x, void* y){ return 1;} struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(*
f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){
void* _temp73=( void*) d->tree; struct Cyc_Splay_noderef* _temp79; _LL75: if(
_temp73 ==( void*) Cyc_Splay_Leaf){ goto _LL76;} else{ goto _LL77;} _LL77: if((
unsigned int) _temp73 > 1u?*(( int*) _temp73) == Cyc_Splay_Node: 0){ _LL80:
_temp79=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp73)->f1;
goto _LL78;} else{ goto _LL74;} _LL76:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp81=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp81[ 0]=({ struct Cyc_Core_Impossible_struct _temp82; _temp82.tag= Cyc_Core_Impossible;
_temp82.f1=( struct _tagged_string)({ char* _temp83=( char*)"Dict::lookup";
struct _tagged_string _temp84; _temp84.curr= _temp83; _temp84.base= _temp83;
_temp84.last_plus_one= _temp83 + 13; _temp84;}); _temp82;}); _temp81;})); _LL78: {
struct Cyc_Splay_node* n= _temp79->v; void* _temp85=( void*) n->left; struct Cyc_Splay_noderef*
_temp91; _LL87: if( _temp85 ==( void*) Cyc_Splay_Leaf){ goto _LL88;} else{ goto
_LL89;} _LL89: if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Splay_Node:
0){ _LL92: _temp91=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp85)->f1; goto _LL90;} else{ goto _LL86;} _LL88: return({ struct Cyc_SlowDict_Dict*
_temp93=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp93->reln= d->reln; _temp93->tree=( void*)(( void*) n->right); _temp93;});
_LL90: { void* _temp94=( void*) n->right; struct Cyc_Splay_noderef* _temp100;
_LL96: if( _temp94 ==( void*) Cyc_Splay_Leaf){ goto _LL97;} else{ goto _LL98;}
_LL98: if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Splay_Node: 0){
_LL101: _temp100=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp94)->f1; goto _LL99;} else{ goto _LL95;} _LL97: return({ struct Cyc_SlowDict_Dict*
_temp102=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp102->reln= d->reln; _temp102->tree=( void*)(( void*) n->left); _temp102;});
_LL99:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp91->v; return({ struct Cyc_SlowDict_Dict* _temp103=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp103->reln= d->reln; _temp103->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp104=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp104[ 0]=({ struct Cyc_Splay_Node_struct
_temp105; _temp105.tag= Cyc_Splay_Node; _temp105.f1=({ struct Cyc_Splay_noderef*
_temp106=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp106->v=({ struct Cyc_Splay_node* _temp107=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp107->key=( void*)(( void*)
newtop->key); _temp107->data=( void*)(( void*) newtop->data); _temp107->left=(
void*)(( void*) newtop->left); _temp107->right=( void*)(( void*) n->right);
_temp107;}); _temp106;}); _temp105;}); _temp104;})); _temp103;});} _LL95:;}
_LL86:;} _LL74:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* _temp108=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); goto _LL109; _LL109: if( d == _temp108){( void) _throw(( void*) Cyc_SlowDict_Absent);}
return _temp108;} static void* Cyc_SlowDict_fold_tree( void*(* f)( void*, void*,
void*), void* t, void* accum){ void* _temp110= t; struct Cyc_Splay_noderef*
_temp116; _LL112: if( _temp110 ==( void*) Cyc_Splay_Leaf){ goto _LL113;} else{
goto _LL114;} _LL114: if(( unsigned int) _temp110 > 1u?*(( int*) _temp110) ==
Cyc_Splay_Node: 0){ _LL117: _temp116=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp110)->f1; goto _LL115;} else{ goto _LL111;} _LL113: return accum; _LL115: {
struct Cyc_Splay_node* n= _temp116->v; return f(( void*) n->key,( void*) n->data,((
void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL111:;} void*
Cyc_SlowDict_fold( void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d,
void* accum){ return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void*
accum)) Cyc_SlowDict_fold_tree)( f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum){ void*
_temp118= t; struct Cyc_Splay_noderef* _temp124; _LL120: if( _temp118 ==( void*)
Cyc_Splay_Leaf){ goto _LL121;} else{ goto _LL122;} _LL122: if(( unsigned int)
_temp118 > 1u?*(( int*) _temp118) == Cyc_Splay_Node: 0){ _LL125: _temp124=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp118)->f1; goto
_LL123;} else{ goto _LL119;} _LL121: return accum; _LL123: { struct Cyc_Splay_node*
n= _temp124->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL119:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp126= t; struct Cyc_Splay_noderef*
_temp132; _LL128: if( _temp126 ==( void*) Cyc_Splay_Leaf){ goto _LL129;} else{
goto _LL130;} _LL130: if(( unsigned int) _temp126 > 1u?*(( int*) _temp126) ==
Cyc_Splay_Node: 0){ _LL133: _temp132=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp126)->f1; goto _LL131;} else{ goto _LL127;} _LL129: goto _LL127; _LL131: {
struct Cyc_Splay_node* _temp134= _temp132->v; goto _LL135; _LL135:(( void(*)(
void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) _temp134->left);
f(( void*) _temp134->key,( void*) _temp134->data);(( void(*)( void*(* f)( void*,
void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) _temp134->right); goto
_LL127;} _LL127:;} void Cyc_SlowDict_app( void*(* f)( void*, void*), struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,(
void*) d->tree);} static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*),
void* t){ void* _temp136= t; struct Cyc_Splay_noderef* _temp142; _LL138: if(
_temp136 ==( void*) Cyc_Splay_Leaf){ goto _LL139;} else{ goto _LL140;} _LL140:
if(( unsigned int) _temp136 > 1u?*(( int*) _temp136) == Cyc_Splay_Node: 0){
_LL143: _temp142=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp136)->f1; goto _LL141;} else{ goto _LL137;} _LL139: goto _LL137; _LL141: {
struct Cyc_Splay_node* n= _temp142->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL137;} _LL137:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp144= t; struct
Cyc_Splay_noderef* _temp150; _LL146: if( _temp144 ==( void*) Cyc_Splay_Leaf){
goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp144 > 1u?*((
int*) _temp144) == Cyc_Splay_Node: 0){ _LL151: _temp150=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp144)->f1; goto _LL149;} else{ goto _LL145;}
_LL147: goto _LL145; _LL149: { struct Cyc_Splay_node* n= _temp150->v;(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) n->right); goto _LL145;} _LL145:;} void Cyc_SlowDict_app_c( void*(*
f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp152= t; struct Cyc_Splay_noderef*
_temp158; _LL154: if( _temp152 ==( void*) Cyc_Splay_Leaf){ goto _LL155;} else{
goto _LL156;} _LL156: if(( unsigned int) _temp152 > 1u?*(( int*) _temp152) ==
Cyc_Splay_Node: 0){ _LL159: _temp158=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp152)->f1; goto _LL157;} else{ goto _LL153;} _LL155: goto _LL153; _LL157: {
struct Cyc_Splay_node* n= _temp158->v;(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->left); f( env,(
void*) n->key,( void*) n->data);(( void(*)( void(* f)( void*, void*, void*),
void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) n->right); goto
_LL153;} _LL153:;} void Cyc_SlowDict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*,
void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)( f, env,( void*) d->tree);}
static void* Cyc_SlowDict_map_tree( void*(* f)( void*), void* t){ void* _temp160=
t; struct Cyc_Splay_noderef* _temp166; _LL162: if( _temp160 ==( void*) Cyc_Splay_Leaf){
goto _LL163;} else{ goto _LL164;} _LL164: if(( unsigned int) _temp160 > 1u?*((
int*) _temp160) == Cyc_Splay_Node: 0){ _LL167: _temp166=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp160)->f1; goto _LL165;} else{ goto _LL161;}
_LL163: return( void*) Cyc_Splay_Leaf; _LL165: { struct Cyc_Splay_node* _temp168=
_temp166->v; goto _LL169; _LL169: return( void*)({ struct Cyc_Splay_Node_struct*
_temp170=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp170[ 0]=({ struct Cyc_Splay_Node_struct _temp171; _temp171.tag= Cyc_Splay_Node;
_temp171.f1=({ struct Cyc_Splay_noderef* _temp172=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp172->v=({ struct Cyc_Splay_node*
_temp173=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp173->key=( void*)(( void*) _temp168->key); _temp173->data=( void*) f(( void*)
_temp168->data); _temp173->left=( void*)(( void*(*)( void*(* f)( void*), void* t))
Cyc_SlowDict_map_tree)( f,( void*) _temp168->left); _temp173->right=( void*)((
void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,( void*)
_temp168->right); _temp173;}); _temp172;}); _temp171;}); _temp170;});} _LL161:;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_map( void*(* f)( void*), struct Cyc_SlowDict_Dict*
d){ return({ struct Cyc_SlowDict_Dict* _temp174=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp174->reln= d->reln; _temp174->tree=(
void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)( f,(
void*) d->tree); _temp174;});} static void* Cyc_SlowDict_map_tree_c( void*(* f)(
void*, void*), void* env, void* t){ void* _temp175= t; struct Cyc_Splay_noderef*
_temp181; _LL177: if( _temp175 ==( void*) Cyc_Splay_Leaf){ goto _LL178;} else{
goto _LL179;} _LL179: if(( unsigned int) _temp175 > 1u?*(( int*) _temp175) ==
Cyc_Splay_Node: 0){ _LL182: _temp181=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp175)->f1; goto _LL180;} else{ goto _LL176;} _LL178: return( void*) Cyc_Splay_Leaf;
_LL180: { struct Cyc_Splay_node* n= _temp181->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp183=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp183[ 0]=({ struct Cyc_Splay_Node_struct _temp184; _temp184.tag= Cyc_Splay_Node;
_temp184.f1=({ struct Cyc_Splay_noderef* _temp185=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp185->v=({ struct Cyc_Splay_node*
_temp186=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp186->key=( void*)(( void*) n->key); _temp186->data=( void*) f( env,( void*)
n->data); _temp186->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp186->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp186;}); _temp185;}); _temp184;}); _temp183;});}
_LL176:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp187=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp187->reln= d->reln; _temp187->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp187;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp188=( void*) d->tree; struct Cyc_Splay_noderef* _temp194; _LL190: if(
_temp188 ==( void*) Cyc_Splay_Leaf){ goto _LL191;} else{ goto _LL192;} _LL192:
if(( unsigned int) _temp188 > 1u?*(( int*) _temp188) == Cyc_Splay_Node: 0){
_LL195: _temp194=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp188)->f1; goto _LL193;} else{ goto _LL189;} _LL191:( void) _throw(( void*)
Cyc_SlowDict_Absent); _LL193: return({ struct _tuple0* _temp196=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp196->f1=( void*)( _temp194->v)->key;
_temp196->f2=( void*)( _temp194->v)->data; _temp196;}); _LL189:;} struct Cyc_List_List*
Cyc_SlowDict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp197=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp197->hd=( void*)({ struct _tuple0* _temp198=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp198->f1= k; _temp198->f2=
v; _temp198;}); _temp197->tl= accum; _temp197;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}