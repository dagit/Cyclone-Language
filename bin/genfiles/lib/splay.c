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
void*, void*); static const int Cyc_Splay_LEFT= 0; static const int Cyc_Splay_RIGHT=
1; static void Cyc_Splay_rotate_left( struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node*
_temp0= nr->v; goto _LL1; _LL1: { void* _temp2=( void*) _temp0->left; struct Cyc_Splay_noderef*
_temp8; _LL4: if(( unsigned int) _temp2 > 1u?*(( int*) _temp2) == Cyc_Splay_Node:
0){ _LL9: _temp8=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL5: { struct Cyc_Splay_node*
_temp10= _temp8->v; goto _LL11; _LL11: { struct Cyc_Splay_Node_struct* _temp16=({
struct Cyc_Splay_Node_struct* _temp12=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp12[ 0]=({ struct Cyc_Splay_Node_struct
_temp13; _temp13.tag= Cyc_Splay_Node; _temp13.f1=({ struct Cyc_Splay_noderef*
_temp14=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp14->v=({ struct Cyc_Splay_node* _temp15=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp15->key=( void*)(( void*) _temp0->key);
_temp15->data=( void*)(( void*) _temp0->data); _temp15->left=( void*)(( void*)
_temp10->right); _temp15->right=( void*)(( void*) _temp0->right); _temp15;});
_temp14;}); _temp13;}); _temp12;}); goto _LL17; _LL17: nr->v=({ struct Cyc_Splay_node*
_temp18=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp18->key=( void*)(( void*) _temp10->key); _temp18->data=( void*)(( void*)
_temp10->data); _temp18->left=( void*)(( void*) _temp10->left); _temp18->right=(
void*)(( void*) _temp16); _temp18;}); goto _LL3;}} _LL7:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp19=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp19[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp20; _temp20.tag= Cyc_Core_InvalidArg; _temp20.f1=(
struct _tagged_string)({ char* _temp21=( char*)"Splay::rotate_left"; struct
_tagged_string _temp22; _temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one=
_temp21 + 19; _temp22;}); _temp20;}); _temp19;})); _LL3:;}} static void Cyc_Splay_rotate_right(
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp23= nr->v; goto _LL24;
_LL24: { void* _temp25=( void*) _temp23->right; struct Cyc_Splay_noderef*
_temp31; _LL27: if(( unsigned int) _temp25 > 1u?*(( int*) _temp25) == Cyc_Splay_Node:
0){ _LL32: _temp31=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp25)->f1; goto _LL28;} else{ goto _LL29;} _LL29: goto _LL30; _LL28: { struct
Cyc_Splay_node* _temp33= _temp31->v; goto _LL34; _LL34: { struct Cyc_Splay_Node_struct*
_temp39=({ struct Cyc_Splay_Node_struct* _temp35=( struct Cyc_Splay_Node_struct*)
GC_malloc( sizeof( struct Cyc_Splay_Node_struct)); _temp35[ 0]=({ struct Cyc_Splay_Node_struct
_temp36; _temp36.tag= Cyc_Splay_Node; _temp36.f1=({ struct Cyc_Splay_noderef*
_temp37=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp37->v=({ struct Cyc_Splay_node* _temp38=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp38->key=( void*)(( void*) _temp23->key);
_temp38->data=( void*)(( void*) _temp23->data); _temp38->left=( void*)(( void*)
_temp23->left); _temp38->right=( void*)(( void*) _temp33->left); _temp38;});
_temp37;}); _temp36;}); _temp35;}); goto _LL40; _LL40: nr->v=({ struct Cyc_Splay_node*
_temp41=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp41->key=( void*)(( void*) _temp33->key); _temp41->data=( void*)(( void*)
_temp33->data); _temp41->left=( void*)(( void*) _temp39); _temp41->right=( void*)((
void*) _temp33->right); _temp41;}); goto _LL26;}} _LL30:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp42=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp42[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp43; _temp43.tag= Cyc_Core_InvalidArg; _temp43.f1=(
struct _tagged_string)({ char* _temp44=( char*)"Splay::rotate_right"; struct
_tagged_string _temp45; _temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one=
_temp44 + 20; _temp45;}); _temp43;}); _temp42;})); _LL26:;}} struct _tuple0{
void* f1; struct Cyc_Splay_noderef* f2; } ; static void Cyc_Splay_lift( struct
Cyc_List_List* dnl){ while( dnl != 0) { if(({ struct Cyc_List_List* _temp46= dnl;
if( _temp46 == 0){ _throw( Null_Exception);} _temp46->tl;}) == 0){{ struct
_tuple0* _temp48=( struct _tuple0*)({ struct Cyc_List_List* _temp47= dnl; if(
_temp47 == 0){ _throw( Null_Exception);} _temp47->hd;}); struct _tuple0 _temp54;
struct Cyc_Splay_noderef* _temp55; void* _temp57; struct _tuple0 _temp59; struct
Cyc_Splay_noderef* _temp60; void* _temp62; _LL50: _temp54=* _temp48; _LL58:
_temp57= _temp54.f1; if( _temp57 ==( void*) Cyc_Splay_LEFT){ goto _LL56;} else{
goto _LL52;} _LL56: _temp55= _temp54.f2; goto _LL51; _LL52: _temp59=* _temp48;
_LL63: _temp62= _temp59.f1; if( _temp62 ==( void*) Cyc_Splay_RIGHT){ goto _LL61;}
else{ goto _LL49;} _LL61: _temp60= _temp59.f2; goto _LL53; _LL51:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp55); goto _LL49;
_LL53:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp60); goto _LL49; _LL49:;} return;}{ struct _tuple0 _temp67; struct Cyc_Splay_noderef*
_temp68; void* _temp70; struct _tuple0* _temp65=( struct _tuple0*)({ struct Cyc_List_List*
_temp64= dnl; if( _temp64 == 0){ _throw( Null_Exception);} _temp64->hd;});
_temp67=* _temp65; _LL71: _temp70= _temp67.f1; goto _LL69; _LL69: _temp68=
_temp67.f2; goto _LL66; _LL66: { struct _tuple0 _temp76; struct Cyc_Splay_noderef*
_temp77; void* _temp79; struct _tuple0* _temp74=( struct _tuple0*)({ struct Cyc_List_List*
_temp73=({ struct Cyc_List_List* _temp72= dnl; if( _temp72 == 0){ _throw(
Null_Exception);} _temp72->tl;}); if( _temp73 == 0){ _throw( Null_Exception);}
_temp73->hd;}); _temp76=* _temp74; _LL80: _temp79= _temp76.f1; goto _LL78; _LL78:
_temp77= _temp76.f2; goto _LL75; _LL75: dnl=({ struct Cyc_List_List* _temp82=({
struct Cyc_List_List* _temp81= dnl; if( _temp81 == 0){ _throw( Null_Exception);}
_temp81->tl;}); if( _temp82 == 0){ _throw( Null_Exception);} _temp82->tl;});{
void* _temp83= _temp70; _LL85: if( _temp83 ==( void*) Cyc_Splay_LEFT){ goto
_LL86;} else{ goto _LL87;} _LL87: if( _temp83 ==( void*) Cyc_Splay_RIGHT){ goto
_LL88;} else{ goto _LL84;} _LL86:{ void* _temp89= _temp79; _LL91: if( _temp89 ==(
void*) Cyc_Splay_LEFT){ goto _LL92;} else{ goto _LL93;} _LL93: if( _temp89 ==(
void*) Cyc_Splay_RIGHT){ goto _LL94;} else{ goto _LL90;} _LL92:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp77);(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp77); goto _LL90;
_LL94:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp68);((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp77); goto
_LL90; _LL90:;} goto _LL84; _LL88:{ void* _temp95= _temp79; _LL97: if( _temp95
==( void*) Cyc_Splay_LEFT){ goto _LL98;} else{ goto _LL99;} _LL99: if( _temp95
==( void*) Cyc_Splay_RIGHT){ goto _LL100;} else{ goto _LL96;} _LL98:(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp68);(( void(*)(
struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp77); goto _LL96;
_LL100:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp77);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp77); goto _LL96; _LL96:;} goto _LL84; _LL84:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp101= tree; struct Cyc_Splay_noderef* _temp107;
_LL103: if(( unsigned int) _temp101 > 1u?*(( int*) _temp101) == Cyc_Splay_Node:
0){ _LL108: _temp107=( struct Cyc_Splay_noderef*)(( struct Cyc_Splay_Node_struct*)
_temp101)->f1; goto _LL104;} else{ goto _LL105;} _LL105: if( _temp101 ==( void*)
Cyc_Splay_Leaf){ goto _LL106;} else{ goto _LL102;} _LL104: { struct Cyc_Splay_node*
_temp109= _temp107->v; goto _LL110; _LL110: { int _temp111= reln( reln_first_arg,(
void*) _temp109->key); goto _LL112; _LL112: if( _temp111 == 0){(( void(*)(
struct Cyc_List_List* dnl)) Cyc_Splay_lift)( path); return 1;} else{ if(
_temp111 < 0){ path=({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp113->hd=( void*)({ struct
_tuple0* _temp114=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp114->f1=( void*) Cyc_Splay_LEFT; _temp114->f2= _temp107; _temp114;});
_temp113->tl= path; _temp113;}); tree=( void*) _temp109->left;} else{ path=({
struct Cyc_List_List* _temp115=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp115->hd=( void*)({ struct _tuple0* _temp116=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp116->f1=( void*) Cyc_Splay_RIGHT;
_temp116->f2= _temp107; _temp116;}); _temp115->tl= path; _temp115;}); tree=(
void*) _temp109->right;}} goto _LL102;}} _LL106: if( path != 0){(( void(*)(
struct Cyc_List_List* dnl)) Cyc_Splay_lift)(({ struct Cyc_List_List* _temp117=
path; if( _temp117 == 0){ _throw( Null_Exception);} _temp117->tl;}));} return 0;
_LL102:;}}