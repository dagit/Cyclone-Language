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
int(* f)( void*, void*), void*, void*); static const int Cyc_Splay_LEFT= 0;
static const int Cyc_Splay_RIGHT= 1; static void Cyc_Splay_rotate_left( struct
Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp0= nr->v; void* _temp1=(
void*) _temp0->left; struct Cyc_Splay_noderef* _temp7; _LL3: if(( unsigned int)
_temp1 > 1u?*(( int*) _temp1) == Cyc_Splay_Node: 0){ _LL8: _temp7=(( struct Cyc_Splay_Node_struct*)
_temp1)->f1; goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_Splay_node*
_temp9= _temp7->v; struct Cyc_Splay_Node_struct* _temp10=({ struct Cyc_Splay_Node_struct*
_temp12=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp12[ 0]=({ struct Cyc_Splay_Node_struct _temp13; _temp13.tag= Cyc_Splay_Node;
_temp13.f1=({ struct Cyc_Splay_noderef* _temp14=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp14->v=({ struct Cyc_Splay_node*
_temp15=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp15->key=( void*)(( void*) _temp0->key); _temp15->data=( void*)(( void*)
_temp0->data); _temp15->left=( void*)(( void*) _temp9->right); _temp15->right=(
void*)(( void*) _temp0->right); _temp15;}); _temp14;}); _temp13;}); _temp12;});
nr->v=({ struct Cyc_Splay_node* _temp11=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp11->key=( void*)(( void*) _temp9->key);
_temp11->data=( void*)(( void*) _temp9->data); _temp11->left=( void*)(( void*)
_temp9->left); _temp11->right=( void*)(( void*) _temp10); _temp11;}); goto _LL2;}
_LL6:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp16=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp16[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp17; _temp17.tag= Cyc_Core_InvalidArg;
_temp17.f1= _tag_arr("Splay::rotate_left", sizeof( unsigned char), 19u); _temp17;});
_temp16;})); _LL2:;} static void Cyc_Splay_rotate_right( struct Cyc_Splay_noderef*
nr){ struct Cyc_Splay_node* _temp18= nr->v; void* _temp19=( void*) _temp18->right;
struct Cyc_Splay_noderef* _temp25; _LL21: if(( unsigned int) _temp19 > 1u?*((
int*) _temp19) == Cyc_Splay_Node: 0){ _LL26: _temp25=(( struct Cyc_Splay_Node_struct*)
_temp19)->f1; goto _LL22;} else{ goto _LL23;} _LL23: goto _LL24; _LL22: { struct
Cyc_Splay_node* _temp27= _temp25->v; struct Cyc_Splay_Node_struct* _temp28=({
struct Cyc_Splay_Node_struct* _temp30=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp30[ 0]=({ struct Cyc_Splay_Node_struct
_temp31; _temp31.tag= Cyc_Splay_Node; _temp31.f1=({ struct Cyc_Splay_noderef*
_temp32=( struct Cyc_Splay_noderef*) GC_malloc( sizeof( struct Cyc_Splay_noderef));
_temp32->v=({ struct Cyc_Splay_node* _temp33=( struct Cyc_Splay_node*) GC_malloc(
sizeof( struct Cyc_Splay_node)); _temp33->key=( void*)(( void*) _temp18->key);
_temp33->data=( void*)(( void*) _temp18->data); _temp33->left=( void*)(( void*)
_temp18->left); _temp33->right=( void*)(( void*) _temp27->left); _temp33;});
_temp32;}); _temp31;}); _temp30;}); nr->v=({ struct Cyc_Splay_node* _temp29=(
struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node)); _temp29->key=(
void*)(( void*) _temp27->key); _temp29->data=( void*)(( void*) _temp27->data);
_temp29->left=( void*)(( void*) _temp28); _temp29->right=( void*)(( void*)
_temp27->right); _temp29;}); goto _LL20;} _LL24:( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp34=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp34[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp35; _temp35.tag= Cyc_Core_InvalidArg; _temp35.f1=
_tag_arr("Splay::rotate_right", sizeof( unsigned char), 20u); _temp35;});
_temp34;})); _LL20:;} struct _tuple0{ void* f1; struct Cyc_Splay_noderef* f2; }
; static void Cyc_Splay_lift( struct Cyc_List_List* dnl){ while( dnl != 0) { if(((
struct Cyc_List_List*) _check_null( dnl))->tl == 0){{ struct _tuple0* _temp36=(
struct _tuple0*)(( struct Cyc_List_List*) _check_null( dnl))->hd; struct _tuple0
_temp42; struct Cyc_Splay_noderef* _temp43; void* _temp45; struct _tuple0
_temp47; struct Cyc_Splay_noderef* _temp48; void* _temp50; _LL38: _temp42=*
_temp36; _LL46: _temp45= _temp42.f1; if( _temp45 ==( void*) Cyc_Splay_LEFT){
goto _LL44;} else{ goto _LL40;} _LL44: _temp43= _temp42.f2; goto _LL39; _LL40:
_temp47=* _temp36; _LL51: _temp50= _temp47.f1; if( _temp50 ==( void*) Cyc_Splay_RIGHT){
goto _LL49;} else{ goto _LL37;} _LL49: _temp48= _temp47.f2; goto _LL41; _LL39:((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp43); goto
_LL37; _LL41:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp48); goto _LL37; _LL37:;} return;}{ struct _tuple0 _temp54; struct Cyc_Splay_noderef*
_temp55; void* _temp57; struct _tuple0* _temp52=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl))->hd; _temp54=* _temp52; _LL58: _temp57= _temp54.f1; goto
_LL56; _LL56: _temp55= _temp54.f2; goto _LL53; _LL53: { struct _tuple0 _temp61;
struct Cyc_Splay_noderef* _temp62; void* _temp64; struct _tuple0* _temp59=(
struct _tuple0*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( dnl))->tl))->hd; _temp61=* _temp59; _LL65: _temp64= _temp61.f1;
goto _LL63; _LL63: _temp62= _temp61.f2; goto _LL60; _LL60: dnl=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( dnl))->tl))->tl;{ void*
_temp66= _temp57; _LL68: if( _temp66 ==( void*) Cyc_Splay_LEFT){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp66 ==( void*) Cyc_Splay_RIGHT){ goto _LL71;}
else{ goto _LL67;} _LL69:{ void* _temp72= _temp64; _LL74: if( _temp72 ==( void*)
Cyc_Splay_LEFT){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp72 ==( void*)
Cyc_Splay_RIGHT){ goto _LL77;} else{ goto _LL73;} _LL75:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_left)( _temp62);(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_left)( _temp62); goto _LL73; _LL77:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_left)( _temp55);(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_right)( _temp62); goto _LL73; _LL73:;} goto _LL67; _LL71:{ void*
_temp78= _temp64; _LL80: if( _temp78 ==( void*) Cyc_Splay_LEFT){ goto _LL81;}
else{ goto _LL82;} _LL82: if( _temp78 ==( void*) Cyc_Splay_RIGHT){ goto _LL83;}
else{ goto _LL79;} _LL81:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp55);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp62); goto _LL79; _LL83:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp62);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp62); goto _LL79; _LL79:;} goto _LL67; _LL67:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp84= tree; struct Cyc_Splay_noderef* _temp90;
_LL86: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) == Cyc_Splay_Node: 0){
_LL91: _temp90=(( struct Cyc_Splay_Node_struct*) _temp84)->f1; goto _LL87;}
else{ goto _LL88;} _LL88: if( _temp84 ==( void*) Cyc_Splay_Leaf){ goto _LL89;}
else{ goto _LL85;} _LL87: { struct Cyc_Splay_node* _temp92= _temp90->v; int
_temp93= reln( reln_first_arg,( void*) _temp92->key); if( _temp93 == 0){(( void(*)(
struct Cyc_List_List* dnl)) Cyc_Splay_lift)( path); return 1;} else{ if( _temp93
< 0){ path=({ struct Cyc_List_List* _temp94=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp94->hd=( void*)({ struct _tuple0* _temp95=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp95->f1=( void*) Cyc_Splay_LEFT;
_temp95->f2= _temp90; _temp95;}); _temp94->tl= path; _temp94;}); tree=( void*)
_temp92->left;} else{ path=({ struct Cyc_List_List* _temp96=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp96->hd=( void*)({ struct _tuple0*
_temp97=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp97->f1=(
void*) Cyc_Splay_RIGHT; _temp97->f2= _temp90; _temp97;}); _temp96->tl= path;
_temp96;}); tree=( void*) _temp92->right;}} goto _LL85;} _LL89: if( path != 0){((
void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)((( struct Cyc_List_List*)
_check_null( path))->tl);} return 0; _LL85:;}}