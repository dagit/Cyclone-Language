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
v; } ; typedef void* Cyc_Splay_tree; extern void* Cyc_Splay_Leaf; extern const
int Cyc_Splay_Node_tag; struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef*
f1; } ; struct Cyc_Splay_node{ void* key; void* data; void* left; void* right; }
; extern int Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); void* Cyc_Splay_Leaf=(
void*) 0; const int Cyc_Splay_Node_tag= 0; typedef void* Cyc_Splay_direction;
static void* Cyc_Splay_LEFT=( void*) 0; static void* Cyc_Splay_RIGHT=( void*) 1;
static void Cyc_Splay_rotate_left( struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node*
n= nr->v; void* _temp0=( void*) n->left; struct Cyc_Splay_noderef* _temp6; _LL2:
if(( unsigned int) _temp0 > 1u?(( struct _enum_struct*) _temp0)->tag == Cyc_Splay_Node_tag:
0){ _LL7: _temp6=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: goto _LL5; _LL3: { struct Cyc_Splay_node*
n2= _temp6->v; void* t=({ struct Cyc_Splay_Node_struct* _temp8=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp8->tag= Cyc_Splay_Node_tag;
_temp8->f1=({ struct Cyc_Splay_noderef* _temp9=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp9->v=({ struct Cyc_Splay_node*
_temp10=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp10->key=( void*)(( void*) n->key); _temp10->data=( void*)(( void*) n->data);
_temp10->left=( void*)(( void*) n2->right); _temp10->right=( void*)(( void*) n->right);
_temp10;}); _temp9;});( void*) _temp8;}); nr->v=({ struct Cyc_Splay_node*
_temp11=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp11->key=( void*)(( void*) n2->key); _temp11->data=( void*)(( void*) n2->data);
_temp11->left=( void*)(( void*) n2->left); _temp11->right=( void*) t; _temp11;});
goto _LL1;} _LL5:( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp12=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp12->tag= Cyc_Core_InvalidArg_tag; _temp12->f1=( struct _tagged_string)({
char* _temp13=( char*)"Splay::rotate_left"; struct _tagged_string _temp14;
_temp14.curr= _temp13; _temp14.base= _temp13; _temp14.last_plus_one= _temp13 +
19; _temp14;});( struct _xenum_struct*) _temp12;})); _LL1:;} static void Cyc_Splay_rotate_right(
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* n= nr->v; void* _temp15=(
void*) n->right; struct Cyc_Splay_noderef* _temp21; _LL17: if(( unsigned int)
_temp15 > 1u?(( struct _enum_struct*) _temp15)->tag == Cyc_Splay_Node_tag: 0){
_LL22: _temp21=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp15)->f1; goto _LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL18: { struct
Cyc_Splay_node* n2= _temp21->v; void* t=({ struct Cyc_Splay_Node_struct* _temp23=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp23->tag= Cyc_Splay_Node_tag; _temp23->f1=({ struct Cyc_Splay_noderef*
_temp24=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp24->v=({ struct Cyc_Splay_node* _temp25=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp25->key=( void*)(( void*) n->key); _temp25->data=(
void*)(( void*) n->data); _temp25->left=( void*)(( void*) n->left); _temp25->right=(
void*)(( void*) n2->left); _temp25;}); _temp24;});( void*) _temp23;}); nr->v=({
struct Cyc_Splay_node* _temp26=( struct Cyc_Splay_node*) GC_malloc( sizeof(
struct Cyc_Splay_node)); _temp26->key=( void*)(( void*) n2->key); _temp26->data=(
void*)(( void*) n2->data); _temp26->left=( void*) t; _temp26->right=( void*)((
void*) n2->right); _temp26;}); goto _LL16;} _LL20:( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp27=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp27->tag= Cyc_Core_InvalidArg_tag; _temp27->f1=( struct _tagged_string)({
char* _temp28=( char*)"Splay::rotate_right"; struct _tagged_string _temp29;
_temp29.curr= _temp28; _temp29.base= _temp28; _temp29.last_plus_one= _temp28 +
20; _temp29;});( struct _xenum_struct*) _temp27;})); _LL16:;} static void Cyc_Splay_lift(
struct Cyc_List_List* dnl){ while( dnl != 0) { if( dnl->tl == 0){{ struct
_tuple0* _temp30=( struct _tuple0*) dnl->hd; struct _tuple0 _temp36; struct Cyc_Splay_noderef*
_temp37; void* _temp39; struct _tuple0 _temp41; struct Cyc_Splay_noderef*
_temp42; void* _temp44; _LL32: _temp36=* _temp30; _LL40: _temp39= _temp36.f1;
if( _temp39 == Cyc_Splay_LEFT){ goto _LL38;} else{ goto _LL34;} _LL38: _temp37=
_temp36.f2; goto _LL33; _LL34: _temp41=* _temp30; _LL45: _temp44= _temp41.f1;
if( _temp44 == Cyc_Splay_RIGHT){ goto _LL43;} else{ goto _LL31;} _LL43: _temp42=
_temp41.f2; goto _LL35; _LL33:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp37); goto _LL31; _LL35:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp42); goto _LL31; _LL31:;} return;}{ struct _tuple0 _temp48; struct Cyc_Splay_noderef*
_temp49; void* _temp51; struct _tuple0* _temp46=( struct _tuple0*) dnl->hd;
_temp48=* _temp46; _LL52: _temp51= _temp48.f1; goto _LL50; _LL50: _temp49=
_temp48.f2; goto _LL47; _LL47: { struct _tuple0 _temp55; struct Cyc_Splay_noderef*
_temp56; void* _temp58; struct _tuple0* _temp53=( struct _tuple0*)( dnl->tl)->hd;
_temp55=* _temp53; _LL59: _temp58= _temp55.f1; goto _LL57; _LL57: _temp56=
_temp55.f2; goto _LL54; _LL54: dnl=( dnl->tl)->tl;{ void* _temp60= _temp51;
_LL62: if( _temp60 == Cyc_Splay_LEFT){ goto _LL63;} else{ goto _LL64;} _LL64:
if( _temp60 == Cyc_Splay_RIGHT){ goto _LL65;} else{ goto _LL61;} _LL63:{ void*
_temp66= _temp58; _LL68: if( _temp66 == Cyc_Splay_LEFT){ goto _LL69;} else{ goto
_LL70;} _LL70: if( _temp66 == Cyc_Splay_RIGHT){ goto _LL71;} else{ goto _LL67;}
_LL69:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp56);((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp56); goto
_LL67; _LL71:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp49);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp56); goto _LL67; _LL67:;} goto _LL61; _LL65:{ void* _temp72= _temp58; _LL74:
if( _temp72 == Cyc_Splay_LEFT){ goto _LL75;} else{ goto _LL76;} _LL76: if(
_temp72 == Cyc_Splay_RIGHT){ goto _LL77;} else{ goto _LL73;} _LL75:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp49);(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp56); goto _LL73;
_LL77:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp56);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp56); goto _LL73; _LL73:;} goto _LL61; _LL61:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp78= tree; struct Cyc_Splay_noderef* _temp84;
_LL80: if(( unsigned int) _temp78 > 1u?(( struct _enum_struct*) _temp78)->tag ==
Cyc_Splay_Node_tag: 0){ _LL85: _temp84=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp78)->f1; goto _LL81;} else{ goto _LL82;} _LL82: if( _temp78 == Cyc_Splay_Leaf){
goto _LL83;} else{ goto _LL79;} _LL81: { struct Cyc_Splay_node* n= _temp84->v;
int comp= reln( reln_first_arg,( void*) n->key); if( comp == 0){(( void(*)(
struct Cyc_List_List* dnl)) Cyc_Splay_lift)( path); return 1;} else{ if( comp <
0){ path=({ struct Cyc_List_List* _temp86=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp86->hd=( void*)({ struct _tuple0* _temp87=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp87->f1= Cyc_Splay_LEFT;
_temp87->f2= _temp84; _temp87;}); _temp86->tl= path; _temp86;}); tree=( void*) n->left;}
else{ path=({ struct Cyc_List_List* _temp88=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp88->hd=( void*)({ struct _tuple0* _temp89=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp89->f1= Cyc_Splay_RIGHT;
_temp89->f2= _temp84; _temp89;}); _temp88->tl= path; _temp88;}); tree=( void*) n->right;}}
goto _LL79;} _LL83: if( path != 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)(
path->tl);} return 0; _LL79:;}}