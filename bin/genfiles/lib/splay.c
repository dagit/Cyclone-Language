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
extern int Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); typedef void*
Cyc_Splay_direction; static const unsigned int Cyc_Splay_LEFT= 0; static const
unsigned int Cyc_Splay_RIGHT= 1; static void Cyc_Splay_rotate_left( struct Cyc_Splay_noderef*
nr){ struct Cyc_Splay_node* n= nr->v; void* _temp0=( void*) n->left; struct Cyc_Splay_noderef*
_temp6; _LL2: if(( unsigned int) _temp0 > 1u?(( struct _tunion_struct*) _temp0)->tag
== Cyc_Splay_Node_tag: 0){ _LL7: _temp6=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: goto
_LL5; _LL3: { struct Cyc_Splay_node* n2= _temp6->v; struct Cyc_Splay_Node_struct*
t=({ struct Cyc_Splay_Node_struct* _temp9=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp9[ 0]=({ struct Cyc_Splay_Node_struct
_temp10; _temp10.tag= Cyc_Splay_Node_tag; _temp10.f1=({ struct Cyc_Splay_noderef*
_temp11=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp11->v=({ struct Cyc_Splay_node* _temp12=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp12->key=( void*)(( void*) n->key); _temp12->data=(
void*)(( void*) n->data); _temp12->left=( void*)(( void*) n2->right); _temp12->right=(
void*)(( void*) n->right); _temp12;}); _temp11;}); _temp10;}); _temp9;}); nr->v=({
struct Cyc_Splay_node* _temp8=( struct Cyc_Splay_node*) GC_malloc( sizeof(
struct Cyc_Splay_node)); _temp8->key=( void*)(( void*) n2->key); _temp8->data=(
void*)(( void*) n2->data); _temp8->left=( void*)(( void*) n2->left); _temp8->right=(
void*)(( void*) t); _temp8;}); goto _LL1;} _LL5:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp13=( struct Cyc_Core_InvalidArg_struct*)
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
t=({ struct Cyc_Splay_Node_struct* _temp26=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp26[ 0]=({ struct Cyc_Splay_Node_struct
_temp27; _temp27.tag= Cyc_Splay_Node_tag; _temp27.f1=({ struct Cyc_Splay_noderef*
_temp28=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp28->v=({ struct Cyc_Splay_node* _temp29=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp29->key=( void*)(( void*) n->key); _temp29->data=(
void*)(( void*) n->data); _temp29->left=( void*)(( void*) n->left); _temp29->right=(
void*)(( void*) n2->left); _temp29;}); _temp28;}); _temp27;}); _temp26;}); nr->v=({
struct Cyc_Splay_node* _temp25=( struct Cyc_Splay_node*) GC_malloc( sizeof(
struct Cyc_Splay_node)); _temp25->key=( void*)(( void*) n2->key); _temp25->data=(
void*)(( void*) n2->data); _temp25->left=( void*)(( void*) t); _temp25->right=(
void*)(( void*) n2->right); _temp25;}); goto _LL18;} _LL22:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp30=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp30[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp31; _temp31.tag= Cyc_Core_InvalidArg_tag;
_temp31.f1=( struct _tagged_string)({ char* _temp32=( char*)"Splay::rotate_right";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 20; _temp33;}); _temp31;}); _temp30;})); _LL18:;}
struct _tuple0{ void* f1; struct Cyc_Splay_noderef* f2; } ; static void Cyc_Splay_lift(
struct Cyc_List_List* dnl){ while( dnl != 0) { if(({ struct Cyc_List_List*
_temp34= dnl; if( _temp34 == 0){ _throw( Null_Exception);} _temp34->tl;}) == 0){{
struct _tuple0* _temp36=( struct _tuple0*)({ struct Cyc_List_List* _temp35= dnl;
if( _temp35 == 0){ _throw( Null_Exception);} _temp35->hd;}); struct _tuple0
_temp42; struct Cyc_Splay_noderef* _temp43; void* _temp45; struct _tuple0
_temp47; struct Cyc_Splay_noderef* _temp48; void* _temp50; _LL38: _temp42=*
_temp36; _LL46: _temp45= _temp42.f1; if(( int) _temp45 == Cyc_Splay_LEFT){ goto
_LL44;} else{ goto _LL40;} _LL44: _temp43= _temp42.f2; goto _LL39; _LL40:
_temp47=* _temp36; _LL51: _temp50= _temp47.f1; if(( int) _temp50 == Cyc_Splay_RIGHT){
goto _LL49;} else{ goto _LL37;} _LL49: _temp48= _temp47.f2; goto _LL41; _LL39:((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp43); goto
_LL37; _LL41:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp48); goto _LL37; _LL37:;} return;}{ struct _tuple0 _temp55; struct Cyc_Splay_noderef*
_temp56; void* _temp58; struct _tuple0* _temp53=( struct _tuple0*)({ struct Cyc_List_List*
_temp52= dnl; if( _temp52 == 0){ _throw( Null_Exception);} _temp52->hd;});
_temp55=* _temp53; _LL59: _temp58= _temp55.f1; goto _LL57; _LL57: _temp56=
_temp55.f2; goto _LL54; _LL54: { struct _tuple0 _temp64; struct Cyc_Splay_noderef*
_temp65; void* _temp67; struct _tuple0* _temp62=( struct _tuple0*)({ struct Cyc_List_List*
_temp61=({ struct Cyc_List_List* _temp60= dnl; if( _temp60 == 0){ _throw(
Null_Exception);} _temp60->tl;}); if( _temp61 == 0){ _throw( Null_Exception);}
_temp61->hd;}); _temp64=* _temp62; _LL68: _temp67= _temp64.f1; goto _LL66; _LL66:
_temp65= _temp64.f2; goto _LL63; _LL63: dnl=({ struct Cyc_List_List* _temp70=({
struct Cyc_List_List* _temp69= dnl; if( _temp69 == 0){ _throw( Null_Exception);}
_temp69->tl;}); if( _temp70 == 0){ _throw( Null_Exception);} _temp70->tl;});{
void* _temp71= _temp58; _LL73: if(( int) _temp71 == Cyc_Splay_LEFT){ goto _LL74;}
else{ goto _LL75;} _LL75: if(( int) _temp71 == Cyc_Splay_RIGHT){ goto _LL76;}
else{ goto _LL72;} _LL74:{ void* _temp77= _temp67; _LL79: if(( int) _temp77 ==
Cyc_Splay_LEFT){ goto _LL80;} else{ goto _LL81;} _LL81: if(( int) _temp77 == Cyc_Splay_RIGHT){
goto _LL82;} else{ goto _LL78;} _LL80:(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_left)( _temp65);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp65); goto _LL78; _LL82:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp56);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp65); goto _LL78; _LL78:;} goto _LL72; _LL76:{ void* _temp83= _temp67; _LL85:
if(( int) _temp83 == Cyc_Splay_LEFT){ goto _LL86;} else{ goto _LL87;} _LL87: if((
int) _temp83 == Cyc_Splay_RIGHT){ goto _LL88;} else{ goto _LL84;} _LL86:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp56);(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp65); goto _LL84;
_LL88:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp65);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp65); goto _LL84; _LL84:;} goto _LL72; _LL72:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp89= tree; struct Cyc_Splay_noderef* _temp95;
_LL91: if(( unsigned int) _temp89 > 1u?(( struct _tunion_struct*) _temp89)->tag
== Cyc_Splay_Node_tag: 0){ _LL96: _temp95=( struct Cyc_Splay_noderef*)(( struct
Cyc_Splay_Node_struct*) _temp89)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if((
int) _temp89 == Cyc_Splay_Leaf){ goto _LL94;} else{ goto _LL90;} _LL92: { struct
Cyc_Splay_node* n= _temp95->v; int comp= reln( reln_first_arg,( void*) n->key);
if( comp == 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)( path);
return 1;} else{ if( comp < 0){ path=({ struct Cyc_List_List* _temp97=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp97->hd=( void*)({
struct _tuple0* _temp98=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp98->f1=( void*) Cyc_Splay_LEFT; _temp98->f2= _temp95; _temp98;}); _temp97->tl=
path; _temp97;}); tree=( void*) n->left;} else{ path=({ struct Cyc_List_List*
_temp99=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp99->hd=( void*)({ struct _tuple0* _temp100=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp100->f1=( void*) Cyc_Splay_RIGHT; _temp100->f2=
_temp95; _temp100;}); _temp99->tl= path; _temp99;}); tree=( void*) n->right;}}
goto _LL90;} _LL94: if( path != 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)(({
struct Cyc_List_List* _temp101= path; if( _temp101 == 0){ _throw( Null_Exception);}
_temp101->tl;}));} return 0; _LL90:;}}