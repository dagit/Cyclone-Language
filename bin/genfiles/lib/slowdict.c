#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; typedef int Cyc_ptrdiff_t; typedef
unsigned int Cyc_size_t; typedef int Cyc_wchar_t; typedef unsigned int Cyc_wint_t;
typedef char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t;
typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t;
typedef int Cyc_ssize_t; typedef unsigned int Cyc_mode_t; typedef unsigned short
Cyc_nlink_t; typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u];
} ; typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Splay_node; struct
Cyc_Splay_noderef{ struct Cyc_Splay_node* v; } ; typedef void* Cyc_Splay_tree;
static const unsigned int Cyc_Splay_Leaf= 0; static const int Cyc_Splay_Node_tag=
0; struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ;
struct Cyc_Splay_node{ void* key; void* data; void* left; void* right; } ;
extern int Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); struct Cyc_SlowDict_Dict;
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
for( 0; kds != 0; kds=({ struct Cyc_List_List* _temp40= kds; if( _temp40 == 0){
_throw( Null_Exception);} _temp40->tl;})){ d=(( struct Cyc_SlowDict_Dict*(*)(
struct Cyc_SlowDict_Dict* d, void* key, void* data)) Cyc_SlowDict_insert)( d,(*((
struct _tuple0*)({ struct Cyc_List_List* _temp41= kds; if( _temp41 == 0){ _throw(
Null_Exception);} _temp41->hd;}))).f1,(*(( struct _tuple0*)({ struct Cyc_List_List*
_temp42= kds; if( _temp42 == 0){ _throw( Null_Exception);} _temp42->hd;}))).f2);}
return d;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_singleton( int(* comp)( void*,
void*), void* key, void* data){ return({ struct Cyc_SlowDict_Dict* _temp43=(
struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp43->reln= comp; _temp43->tree=( void*)(( void*)({ struct Cyc_Splay_Node_struct*
_temp44=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp44[ 0]=({ struct Cyc_Splay_Node_struct _temp45; _temp45.tag= Cyc_Splay_Node_tag;
_temp45.f1=({ struct Cyc_Splay_noderef* _temp46=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp46->v=({ struct Cyc_Splay_node*
_temp47=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp47->key=( void*) key; _temp47->data=( void*) data; _temp47->left=( void*)((
void*) Cyc_Splay_Leaf); _temp47->right=( void*)(( void*) Cyc_Splay_Leaf);
_temp47;}); _temp46;}); _temp45;}); _temp44;})); _temp43;});} void* Cyc_SlowDict_lookup(
struct Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*),
void*, void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp48=(
void*) d->tree; struct Cyc_Splay_noderef* _temp54; _LL50: if(( unsigned int)
_temp48 > 1u?(( struct _tunion_struct*) _temp48)->tag == Cyc_Splay_Node_tag: 0){
_LL55: _temp54=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp48)->f1; goto _LL51;} else{ goto _LL52;} _LL52: if(( int) _temp48 == Cyc_Splay_Leaf){
goto _LL53;} else{ goto _LL49;} _LL51: return( void*)( _temp54->v)->data; _LL53:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp56=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp56[ 0]=({ struct Cyc_Core_Impossible_struct _temp57; _temp57.tag= Cyc_Core_Impossible_tag;
_temp57.f1=( struct _tagged_string)({ char* _temp58=( char*)"Dict::lookup";
struct _tagged_string _temp59; _temp59.curr= _temp58; _temp59.base= _temp58;
_temp59.last_plus_one= _temp58 + 13; _temp59;}); _temp57;}); _temp56;})); _LL49:;}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct*
_temp60=( struct Cyc_SlowDict_Absent_struct*) GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct));
_temp60[ 0]=({ struct Cyc_SlowDict_Absent_struct _temp61; _temp61.tag= Cyc_SlowDict_Absent_tag;
_temp61;}); _temp60;}));} struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct
Cyc_SlowDict_Dict* d, void* key){ if((( int(*)( int(* f)( void*, void*), void*,
void*)) Cyc_Splay_splay)( d->reln, key,( void*) d->tree)){ void* _temp62=( void*)
d->tree; struct Cyc_Splay_noderef* _temp68; _LL64: if(( unsigned int) _temp62 >
1u?(( struct _tunion_struct*) _temp62)->tag == Cyc_Splay_Node_tag: 0){ _LL69:
_temp68=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp62)->f1;
goto _LL65;} else{ goto _LL66;} _LL66: if(( int) _temp62 == Cyc_Splay_Leaf){
goto _LL67;} else{ goto _LL63;} _LL65: return({ struct Cyc_Core_Opt* _temp70=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp70->v=(
void*)(( void*)( _temp68->v)->data); _temp70;}); _LL67:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp71=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp71[ 0]=({ struct
Cyc_Core_Impossible_struct _temp72; _temp72.tag= Cyc_Core_Impossible_tag;
_temp72.f1=( struct _tagged_string)({ char* _temp73=( char*)"Dict::lookup";
struct _tagged_string _temp74; _temp74.curr= _temp73; _temp74.base= _temp73;
_temp74.last_plus_one= _temp73 + 13; _temp74;}); _temp72;}); _temp71;})); _LL63:;}
return 0;} static int Cyc_SlowDict_get_largest( void* x, void* y){ return 1;}
struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete( struct Cyc_SlowDict_Dict* d, void*
key){ if((( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)( d->reln,
key,( void*) d->tree)){ void* _temp75=( void*) d->tree; struct Cyc_Splay_noderef*
_temp81; _LL77: if(( int) _temp75 == Cyc_Splay_Leaf){ goto _LL78;} else{ goto
_LL79;} _LL79: if(( unsigned int) _temp75 > 1u?(( struct _tunion_struct*)
_temp75)->tag == Cyc_Splay_Node_tag: 0){ _LL82: _temp81=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp75)->f1; goto _LL80;} else{ goto _LL76;}
_LL78:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp83=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp83[ 0]=({ struct Cyc_Core_Impossible_struct _temp84; _temp84.tag= Cyc_Core_Impossible_tag;
_temp84.f1=( struct _tagged_string)({ char* _temp85=( char*)"Dict::lookup";
struct _tagged_string _temp86; _temp86.curr= _temp85; _temp86.base= _temp85;
_temp86.last_plus_one= _temp85 + 13; _temp86;}); _temp84;}); _temp83;})); _LL80: {
struct Cyc_Splay_node* n= _temp81->v; void* _temp87=( void*) n->left; struct Cyc_Splay_noderef*
_temp93; _LL89: if(( int) _temp87 == Cyc_Splay_Leaf){ goto _LL90;} else{ goto
_LL91;} _LL91: if(( unsigned int) _temp87 > 1u?(( struct _tunion_struct*)
_temp87)->tag == Cyc_Splay_Node_tag: 0){ _LL94: _temp93=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp87)->f1; goto _LL92;} else{ goto _LL88;}
_LL90: return({ struct Cyc_SlowDict_Dict* _temp95=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp95->reln= d->reln; _temp95->tree=(
void*)(( void*) n->right); _temp95;}); _LL92: { void* _temp96=( void*) n->right;
struct Cyc_Splay_noderef* _temp102; _LL98: if(( int) _temp96 == Cyc_Splay_Leaf){
goto _LL99;} else{ goto _LL100;} _LL100: if(( unsigned int) _temp96 > 1u?((
struct _tunion_struct*) _temp96)->tag == Cyc_Splay_Node_tag: 0){ _LL103:
_temp102=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp96)->f1;
goto _LL101;} else{ goto _LL97;} _LL99: return({ struct Cyc_SlowDict_Dict*
_temp104=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp104->reln= d->reln; _temp104->tree=( void*)(( void*) n->left); _temp104;});
_LL101:(( int(*)( int(* f)( void*, void*), void*, void*)) Cyc_Splay_splay)(( int(*)(
void* x, void* y)) Cyc_SlowDict_get_largest, key,( void*) n->left);{ struct Cyc_Splay_node*
newtop= _temp93->v; return({ struct Cyc_SlowDict_Dict* _temp105=( struct Cyc_SlowDict_Dict*)
GC_malloc( sizeof( struct Cyc_SlowDict_Dict)); _temp105->reln= d->reln; _temp105->tree=(
void*)(( void*)({ struct Cyc_Splay_Node_struct* _temp106=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp106[ 0]=({ struct Cyc_Splay_Node_struct
_temp107; _temp107.tag= Cyc_Splay_Node_tag; _temp107.f1=({ struct Cyc_Splay_noderef*
_temp108=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp108->v=({ struct Cyc_Splay_node* _temp109=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp109->key=( void*)(( void*)
newtop->key); _temp109->data=( void*)(( void*) newtop->data); _temp109->left=(
void*)(( void*) newtop->left); _temp109->right=( void*)(( void*) n->right);
_temp109;}); _temp108;}); _temp107;}); _temp106;})); _temp105;});} _LL97:;}
_LL88:;} _LL76:;} else{ return d;}} struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete_present(
struct Cyc_SlowDict_Dict* d, void* key){ struct Cyc_SlowDict_Dict* d2=(( struct
Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, void* key)) Cyc_SlowDict_delete)(
d, key); if( d == d2){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct*
_temp110=( struct Cyc_SlowDict_Absent_struct*) GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct));
_temp110[ 0]=({ struct Cyc_SlowDict_Absent_struct _temp111; _temp111.tag= Cyc_SlowDict_Absent_tag;
_temp111;}); _temp110;}));} return d2;} static void* Cyc_SlowDict_fold_tree(
void*(* f)( void*, void*, void*), void* t, void* accum){ void* _temp112= t;
struct Cyc_Splay_noderef* _temp118; _LL114: if(( int) _temp112 == Cyc_Splay_Leaf){
goto _LL115;} else{ goto _LL116;} _LL116: if(( unsigned int) _temp112 > 1u?((
struct _tunion_struct*) _temp112)->tag == Cyc_Splay_Node_tag: 0){ _LL119:
_temp118=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp112)->f1;
goto _LL117;} else{ goto _LL113;} _LL115: return accum; _LL117: { struct Cyc_Splay_node*
n= _temp118->v; return f(( void*) n->key,( void*) n->data,(( void*(*)( void*(* f)(
void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)( f,( void*)
n->left,(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum))
Cyc_SlowDict_fold_tree)( f,( void*) n->right, accum)));} _LL113:;} void* Cyc_SlowDict_fold(
void*(* f)( void*, void*, void*), struct Cyc_SlowDict_Dict* d, void* accum){
return(( void*(*)( void*(* f)( void*, void*, void*), void* t, void* accum)) Cyc_SlowDict_fold_tree)(
f,( void*) d->tree, accum);} static void* Cyc_SlowDict_fold_tree_c( void*(* f)(
void*, void*, void*, void*), void* env, void* t, void* accum){ void* _temp120= t;
struct Cyc_Splay_noderef* _temp126; _LL122: if(( int) _temp120 == Cyc_Splay_Leaf){
goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int) _temp120 > 1u?((
struct _tunion_struct*) _temp120)->tag == Cyc_Splay_Node_tag: 0){ _LL127:
_temp126=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp120)->f1;
goto _LL125;} else{ goto _LL121;} _LL123: return accum; _LL125: { struct Cyc_Splay_node*
n= _temp126->v; return f( env,( void*) n->key,( void*) n->data,(( void*(*)( void*(*
f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) n->left,(( void*(*)( void*(* f)( void*, void*, void*, void*),
void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)( f, env,( void*) n->right,
accum)));} _LL121:;} void* Cyc_SlowDict_fold_c( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_SlowDict_Dict* dict, void* accum){ return(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, void* t, void* accum)) Cyc_SlowDict_fold_tree_c)(
f, env,( void*) dict->tree, accum);} static void Cyc_SlowDict_app_tree( void*(*
f)( void*, void*), void* t){ void* _temp128= t; struct Cyc_Splay_noderef*
_temp134; _LL130: if(( int) _temp128 == Cyc_Splay_Leaf){ goto _LL131;} else{
goto _LL132;} _LL132: if(( unsigned int) _temp128 > 1u?(( struct _tunion_struct*)
_temp128)->tag == Cyc_Splay_Node_tag: 0){ _LL135: _temp134=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp128)->f1; goto _LL133;} else{ goto _LL129;}
_LL131: goto _LL129; _LL133: { struct Cyc_Splay_node* n= _temp134->v;(( void(*)(
void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*) n->left);
f(( void*) n->key,( void*) n->data);(( void(*)( void*(* f)( void*, void*), void*
t)) Cyc_SlowDict_app_tree)( f,( void*) n->right); goto _LL129;} _LL129:;} void
Cyc_SlowDict_app( void*(* f)( void*, void*), struct Cyc_SlowDict_Dict* d){((
void(*)( void*(* f)( void*, void*), void* t)) Cyc_SlowDict_app_tree)( f,( void*)
d->tree);} static void Cyc_SlowDict_iter_tree( void(* f)( void*, void*), void* t){
void* _temp136= t; struct Cyc_Splay_noderef* _temp142; _LL138: if(( int)
_temp136 == Cyc_Splay_Leaf){ goto _LL139;} else{ goto _LL140;} _LL140: if((
unsigned int) _temp136 > 1u?(( struct _tunion_struct*) _temp136)->tag == Cyc_Splay_Node_tag:
0){ _LL143: _temp142=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp136)->f1; goto _LL141;} else{ goto _LL137;} _LL139: goto _LL137; _LL141: {
struct Cyc_Splay_node* n= _temp142->v;(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) n->left); f(( void*) n->key,( void*) n->data);((
void(*)( void(* f)( void*, void*), void* t)) Cyc_SlowDict_iter_tree)( f,( void*)
n->right); goto _LL137;} _LL137:;} void Cyc_SlowDict_iter( void(* f)( void*,
void*), struct Cyc_SlowDict_Dict* d){(( void(*)( void(* f)( void*, void*), void*
t)) Cyc_SlowDict_iter_tree)( f,( void*) d->tree);} static void Cyc_SlowDict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, void* t){ void* _temp144= t; struct
Cyc_Splay_noderef* _temp150; _LL146: if(( int) _temp144 == Cyc_Splay_Leaf){ goto
_LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp144 > 1u?(( struct
_tunion_struct*) _temp144)->tag == Cyc_Splay_Node_tag: 0){ _LL151: _temp150=(
struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*) _temp144)->f1; goto
_LL149;} else{ goto _LL145;} _LL147: goto _LL145; _LL149: { struct Cyc_Splay_node*
n= _temp150->v;(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->left); f( env,( void*) n->key,(
void*) n->data);(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t))
Cyc_SlowDict_app_tree_c)( f, env,( void*) n->right); goto _LL145;} _LL145:;}
void Cyc_SlowDict_app_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict*
d){(( void(*)( void*(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_app_tree_c)(
f, env,( void*) d->tree);} static void Cyc_SlowDict_iter_tree_c( void(* f)( void*,
void*, void*), void* env, void* t){ void* _temp152= t; struct Cyc_Splay_noderef*
_temp158; _LL154: if(( int) _temp152 == Cyc_Splay_Leaf){ goto _LL155;} else{
goto _LL156;} _LL156: if(( unsigned int) _temp152 > 1u?(( struct _tunion_struct*)
_temp152)->tag == Cyc_Splay_Node_tag: 0){ _LL159: _temp158=( struct Cyc_Splay_noderef*)((
struct Cyc_Splay_Node_struct*) _temp152)->f1; goto _LL157;} else{ goto _LL153;}
_LL155: goto _LL153; _LL157: { struct Cyc_Splay_node* n= _temp158->v;(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->left); f( env,( void*) n->key,( void*) n->data);(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) n->right); goto _LL153;} _LL153:;} void Cyc_SlowDict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_SlowDict_Dict* d){(( void(*)(
void(* f)( void*, void*, void*), void* env, void* t)) Cyc_SlowDict_iter_tree_c)(
f, env,( void*) d->tree);} static void* Cyc_SlowDict_map_tree( void*(* f)( void*),
void* t){ void* _temp160= t; struct Cyc_Splay_noderef* _temp166; _LL162: if((
int) _temp160 == Cyc_Splay_Leaf){ goto _LL163;} else{ goto _LL164;} _LL164: if((
unsigned int) _temp160 > 1u?(( struct _tunion_struct*) _temp160)->tag == Cyc_Splay_Node_tag:
0){ _LL167: _temp166=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp160)->f1; goto _LL165;} else{ goto _LL161;} _LL163: return( void*) Cyc_Splay_Leaf;
_LL165: { struct Cyc_Splay_node* n= _temp166->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp168=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp168[ 0]=({ struct Cyc_Splay_Node_struct _temp169; _temp169.tag= Cyc_Splay_Node_tag;
_temp169.f1=({ struct Cyc_Splay_noderef* _temp170=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp170->v=({ struct Cyc_Splay_node*
_temp171=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp171->key=( void*)(( void*) n->key); _temp171->data=( void*) f(( void*) n->data);
_temp171->left=( void*)(( void*(*)( void*(* f)( void*), void* t)) Cyc_SlowDict_map_tree)(
f,( void*) n->left); _temp171->right=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) n->right); _temp171;}); _temp170;});
_temp169;}); _temp168;});} _LL161:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map(
void*(* f)( void*), struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp172=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp172->reln= d->reln; _temp172->tree=( void*)(( void*(*)( void*(* f)( void*),
void* t)) Cyc_SlowDict_map_tree)( f,( void*) d->tree); _temp172;});} static void*
Cyc_SlowDict_map_tree_c( void*(* f)( void*, void*), void* env, void* t){ void*
_temp173= t; struct Cyc_Splay_noderef* _temp179; _LL175: if(( int) _temp173 ==
Cyc_Splay_Leaf){ goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp173 > 1u?(( struct _tunion_struct*) _temp173)->tag == Cyc_Splay_Node_tag: 0){
_LL180: _temp179=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp173)->f1; goto _LL178;} else{ goto _LL174;} _LL176: return( void*) Cyc_Splay_Leaf;
_LL178: { struct Cyc_Splay_node* n= _temp179->v; return( void*)({ struct Cyc_Splay_Node_struct*
_temp181=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp181[ 0]=({ struct Cyc_Splay_Node_struct _temp182; _temp182.tag= Cyc_Splay_Node_tag;
_temp182.f1=({ struct Cyc_Splay_noderef* _temp183=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp183->v=({ struct Cyc_Splay_node*
_temp184=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp184->key=( void*)(( void*) n->key); _temp184->data=( void*) f( env,( void*)
n->data); _temp184->left=( void*)(( void*(*)( void*(* f)( void*, void*), void*
env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) n->left); _temp184->right=(
void*)(( void*(*)( void*(* f)( void*, void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)(
f, env,( void*) n->right); _temp184;}); _temp183;}); _temp182;}); _temp181;});}
_LL174:;} struct Cyc_SlowDict_Dict* Cyc_SlowDict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_SlowDict_Dict* d){ return({ struct Cyc_SlowDict_Dict*
_temp185=( struct Cyc_SlowDict_Dict*) GC_malloc( sizeof( struct Cyc_SlowDict_Dict));
_temp185->reln= d->reln; _temp185->tree=( void*)(( void*(*)( void*(* f)( void*,
void*), void* env, void* t)) Cyc_SlowDict_map_tree_c)( f, env,( void*) d->tree);
_temp185;});} struct _tuple0* Cyc_SlowDict_choose( struct Cyc_SlowDict_Dict* d){
void* _temp186=( void*) d->tree; struct Cyc_Splay_noderef* _temp192; _LL188: if((
int) _temp186 == Cyc_Splay_Leaf){ goto _LL189;} else{ goto _LL190;} _LL190: if((
unsigned int) _temp186 > 1u?(( struct _tunion_struct*) _temp186)->tag == Cyc_Splay_Node_tag:
0){ _LL193: _temp192=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp186)->f1; goto _LL191;} else{ goto _LL187;} _LL189:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_SlowDict_Absent_struct* _temp194=( struct Cyc_SlowDict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_SlowDict_Absent_struct)); _temp194[ 0]=({ struct
Cyc_SlowDict_Absent_struct _temp195; _temp195.tag= Cyc_SlowDict_Absent_tag;
_temp195;}); _temp194;})); _LL191: return({ struct _tuple0* _temp196=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp196->f1=( void*)( _temp192->v)->key;
_temp196->f2=( void*)( _temp192->v)->data; _temp196;}); _LL187:;} struct Cyc_List_List*
Cyc_SlowDict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp197=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp197->hd=( void*)({ struct _tuple0* _temp198=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp198->f1= k; _temp198->f2=
v; _temp198;}); _temp197->tl= accum; _temp197;});} struct Cyc_List_List* Cyc_SlowDict_to_list(
struct Cyc_SlowDict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_SlowDict_Dict* d, struct
Cyc_List_List* accum)) Cyc_SlowDict_fold)(( struct Cyc_List_List*(*)( void* k,
void* v, struct Cyc_List_List* accum)) Cyc_SlowDict_to_list_f, d, 0);}