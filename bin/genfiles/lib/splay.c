#include "cyc_include.h"

 struct _tuple0{ void* f1; struct Cyc_Splay_noderef* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; struct Cyc_Splay_node; struct Cyc_Splay_noderef{ struct Cyc_Splay_node*
v; } ; typedef void* Cyc_Splay_tree; extern const unsigned int Cyc_Splay_Leaf;
extern const int Cyc_Splay_Node_tag; struct Cyc_Splay_Node_struct{ int tag;
struct Cyc_Splay_noderef* f1; } ; struct Cyc_Splay_node{ void* key; void* data;
void* left; void* right; } ; extern int Cyc_Splay_splay( int(* f)( void*, void*),
void*, void*); const unsigned int Cyc_Splay_Leaf= 0; const int Cyc_Splay_Node_tag=
0; typedef void* Cyc_Splay_direction; static const unsigned int Cyc_Splay_LEFT=
0; static const unsigned int Cyc_Splay_RIGHT= 1; static void Cyc_Splay_rotate_left(
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* n= nr->v; void* _temp0=(
void*) n->left; struct Cyc_Splay_noderef* _temp6; _LL2: if(( unsigned int)
_temp0 > 1u?(( struct _tunion_struct*) _temp0)->tag == Cyc_Splay_Node_tag: 0){
_LL7: _temp6=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: goto _LL5; _LL3: { struct Cyc_Splay_node*
n2= _temp6->v; struct Cyc_Splay_Node_struct* t=({ struct Cyc_Splay_Node_struct*
_temp8=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp8[ 0]=({ struct Cyc_Splay_Node_struct _temp9; _temp9.tag= Cyc_Splay_Node_tag;
_temp9.f1=({ struct Cyc_Splay_noderef* _temp10=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp10->v=({ struct Cyc_Splay_node*
_temp11=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp11->key=( void*)(( void*) n->key); _temp11->data=( void*)(( void*) n->data);
_temp11->left=( void*)(( void*) n2->right); _temp11->right=( void*)(( void*) n->right);
_temp11;}); _temp10;}); _temp9;}); _temp8;}); nr->v=({ struct Cyc_Splay_node*
_temp12=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp12->key=( void*)(( void*) n2->key); _temp12->data=( void*)(( void*) n2->data);
_temp12->left=( void*)(( void*) n2->left); _temp12->right=( void*)(( void*) t);
_temp12;}); goto _LL1;} _LL5:( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Core_InvalidArg_struct* _temp13=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp13[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp14; _temp14.tag= Cyc_Core_InvalidArg_tag;
_temp14.f1=( struct _tagged_string)({ char* _temp15=( char*)"Splay::rotate_left";
struct _tagged_string _temp16; _temp16.curr= _temp15; _temp16.base= _temp15;
_temp16.last_plus_one= _temp15 + 19; _temp16;}); _temp14;}); _temp13;})); _LL1:;}
static void Cyc_Splay_rotate_right( struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node*
n= nr->v; void* _temp17=( void*) n->right; struct Cyc_Splay_noderef* _temp23;
_LL19: if(( unsigned int) _temp17 > 1u?(( struct _tunion_struct*) _temp17)->tag
== Cyc_Splay_Node_tag: 0){ _LL24: _temp23=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp17)->f1; goto _LL20;} else{ goto _LL21;} _LL21:
goto _LL22; _LL20: { struct Cyc_Splay_node* n2= _temp23->v; struct Cyc_Splay_Node_struct*
t=({ struct Cyc_Splay_Node_struct* _temp25=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp25[ 0]=({ struct Cyc_Splay_Node_struct
_temp26; _temp26.tag= Cyc_Splay_Node_tag; _temp26.f1=({ struct Cyc_Splay_noderef*
_temp27=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp27->v=({ struct Cyc_Splay_node* _temp28=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp28->key=( void*)(( void*) n->key); _temp28->data=(
void*)(( void*) n->data); _temp28->left=( void*)(( void*) n->left); _temp28->right=(
void*)(( void*) n2->left); _temp28;}); _temp27;}); _temp26;}); _temp25;}); nr->v=({
struct Cyc_Splay_node* _temp29=( struct Cyc_Splay_node*) GC_malloc( sizeof(
struct Cyc_Splay_node)); _temp29->key=( void*)(( void*) n2->key); _temp29->data=(
void*)(( void*) n2->data); _temp29->left=( void*)(( void*) t); _temp29->right=(
void*)(( void*) n2->right); _temp29;}); goto _LL18;} _LL22:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp30=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp30[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp31; _temp31.tag= Cyc_Core_InvalidArg_tag;
_temp31.f1=( struct _tagged_string)({ char* _temp32=( char*)"Splay::rotate_right";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 20; _temp33;}); _temp31;}); _temp30;})); _LL18:;}
static void Cyc_Splay_lift( struct Cyc_List_List* dnl){ while( dnl != 0) { if(
dnl->tl == 0){{ struct _tuple0* _temp34=( struct _tuple0*) dnl->hd; struct
_tuple0 _temp40; struct Cyc_Splay_noderef* _temp41; void* _temp43; struct
_tuple0 _temp45; struct Cyc_Splay_noderef* _temp46; void* _temp48; _LL36:
_temp40=* _temp34; _LL44: _temp43= _temp40.f1; if(( int) _temp43 == Cyc_Splay_LEFT){
goto _LL42;} else{ goto _LL38;} _LL42: _temp41= _temp40.f2; goto _LL37; _LL38:
_temp45=* _temp34; _LL49: _temp48= _temp45.f1; if(( int) _temp48 == Cyc_Splay_RIGHT){
goto _LL47;} else{ goto _LL35;} _LL47: _temp46= _temp45.f2; goto _LL39; _LL37:((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp41); goto
_LL35; _LL39:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp46); goto _LL35; _LL35:;} return;}{ struct _tuple0 _temp52; struct Cyc_Splay_noderef*
_temp53; void* _temp55; struct _tuple0* _temp50=( struct _tuple0*) dnl->hd;
_temp52=* _temp50; _LL56: _temp55= _temp52.f1; goto _LL54; _LL54: _temp53=
_temp52.f2; goto _LL51; _LL51: { struct _tuple0 _temp59; struct Cyc_Splay_noderef*
_temp60; void* _temp62; struct _tuple0* _temp57=( struct _tuple0*)( dnl->tl)->hd;
_temp59=* _temp57; _LL63: _temp62= _temp59.f1; goto _LL61; _LL61: _temp60=
_temp59.f2; goto _LL58; _LL58: dnl=( dnl->tl)->tl;{ void* _temp64= _temp55;
_LL66: if(( int) _temp64 == Cyc_Splay_LEFT){ goto _LL67;} else{ goto _LL68;}
_LL68: if(( int) _temp64 == Cyc_Splay_RIGHT){ goto _LL69;} else{ goto _LL65;}
_LL67:{ void* _temp70= _temp62; _LL72: if(( int) _temp70 == Cyc_Splay_LEFT){
goto _LL73;} else{ goto _LL74;} _LL74: if(( int) _temp70 == Cyc_Splay_RIGHT){
goto _LL75;} else{ goto _LL71;} _LL73:(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_left)( _temp60);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp60); goto _LL71; _LL75:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp53);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp60); goto _LL71; _LL71:;} goto _LL65; _LL69:{ void* _temp76= _temp62; _LL78:
if(( int) _temp76 == Cyc_Splay_LEFT){ goto _LL79;} else{ goto _LL80;} _LL80: if((
int) _temp76 == Cyc_Splay_RIGHT){ goto _LL81;} else{ goto _LL77;} _LL79:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp53);(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp60); goto _LL77;
_LL81:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp60);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp60); goto _LL77; _LL77:;} goto _LL65; _LL65:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp82= tree; struct Cyc_Splay_noderef* _temp88;
_LL84: if(( unsigned int) _temp82 > 1u?(( struct _tunion_struct*) _temp82)->tag
== Cyc_Splay_Node_tag: 0){ _LL89: _temp88=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp82)->f1; goto _LL85;} else{ goto _LL86;} _LL86: if((
int) _temp82 == Cyc_Splay_Leaf){ goto _LL87;} else{ goto _LL83;} _LL85: { struct
Cyc_Splay_node* n= _temp88->v; int comp= reln( reln_first_arg,( void*) n->key);
if( comp == 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)( path);
return 1;} else{ if( comp < 0){ path=({ struct Cyc_List_List* _temp90=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp90->hd=( void*)({
struct _tuple0* _temp91=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp91->f1=( void*) Cyc_Splay_LEFT; _temp91->f2= _temp88; _temp91;}); _temp90->tl=
path; _temp90;}); tree=( void*) n->left;} else{ path=({ struct Cyc_List_List*
_temp92=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp92->hd=( void*)({ struct _tuple0* _temp93=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp93->f1=( void*) Cyc_Splay_RIGHT; _temp93->f2=
_temp88; _temp93;}); _temp92->tl= path; _temp92;}); tree=( void*) n->right;}}
goto _LL83;} _LL87: if( path != 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)(
path->tl);} return 0; _LL83:;}}