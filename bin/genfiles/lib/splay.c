 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Splay_node; struct Cyc_Splay_noderef{ struct Cyc_Splay_node*
v; } ; static const int Cyc_Splay_Leaf= 0; static const int Cyc_Splay_Node= 0;
struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef* f1; } ; struct
Cyc_Splay_node{ void* key; void* data; void* left; void* right; } ; extern int
Cyc_Splay_splay( int(* f)( void*, void*), void*, void*); static const int Cyc_Splay_LEFT=
0; static const int Cyc_Splay_RIGHT= 1; static void Cyc_Splay_rotate_left(
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp0= nr->v; goto _LL1;
_LL1: { void* _temp2=( void*) _temp0->left; struct Cyc_Splay_noderef* _temp8;
_LL4: if(( unsigned int) _temp2 > 1u?*(( int*) _temp2) == Cyc_Splay_Node: 0){
_LL9: _temp8=(( struct Cyc_Splay_Node_struct*) _temp2)->f1; goto _LL5;} else{
goto _LL6;} _LL6: goto _LL7; _LL5: { struct Cyc_Splay_node* _temp10= _temp8->v;
goto _LL11; _LL11: { struct Cyc_Splay_Node_struct* _temp16=({ struct Cyc_Splay_Node_struct*
_temp12=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp12[ 0]=({ struct Cyc_Splay_Node_struct _temp13; _temp13.tag= Cyc_Splay_Node;
_temp13.f1=({ struct Cyc_Splay_noderef* _temp14=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp14->v=({ struct Cyc_Splay_node*
_temp15=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp15->key=( void*)(( void*) _temp0->key); _temp15->data=( void*)(( void*)
_temp0->data); _temp15->left=( void*)(( void*) _temp10->right); _temp15->right=(
void*)(( void*) _temp0->right); _temp15;}); _temp14;}); _temp13;}); _temp12;});
goto _LL17; _LL17: nr->v=({ struct Cyc_Splay_node* _temp18=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp18->key=( void*)(( void*)
_temp10->key); _temp18->data=( void*)(( void*) _temp10->data); _temp18->left=(
void*)(( void*) _temp10->left); _temp18->right=( void*)(( void*) _temp16);
_temp18;}); goto _LL3;}} _LL7:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp19=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp19[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp20; _temp20.tag= Cyc_Core_InvalidArg;
_temp20.f1=({ unsigned char* _temp21="Splay::rotate_left"; struct _tagged_string
_temp22; _temp22.curr= _temp21; _temp22.base= _temp21; _temp22.last_plus_one=
_temp21 + 19; _temp22;}); _temp20;}); _temp19;})); _LL3:;}} static void Cyc_Splay_rotate_right(
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp23= nr->v; goto _LL24;
_LL24: { void* _temp25=( void*) _temp23->right; struct Cyc_Splay_noderef*
_temp31; _LL27: if(( unsigned int) _temp25 > 1u?*(( int*) _temp25) == Cyc_Splay_Node:
0){ _LL32: _temp31=(( struct Cyc_Splay_Node_struct*) _temp25)->f1; goto _LL28;}
else{ goto _LL29;} _LL29: goto _LL30; _LL28: { struct Cyc_Splay_node* _temp33=
_temp31->v; goto _LL34; _LL34: { struct Cyc_Splay_Node_struct* _temp39=({ struct
Cyc_Splay_Node_struct* _temp35=( struct Cyc_Splay_Node_struct*) GC_malloc(
sizeof( struct Cyc_Splay_Node_struct)); _temp35[ 0]=({ struct Cyc_Splay_Node_struct
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
Cyc_Core_InvalidArg_struct _temp43; _temp43.tag= Cyc_Core_InvalidArg; _temp43.f1=({
unsigned char* _temp44="Splay::rotate_right"; struct _tagged_string _temp45;
_temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one= _temp44 +
20; _temp45;}); _temp43;}); _temp42;})); _LL26:;}} struct _tuple0{ void* f1;
struct Cyc_Splay_noderef* f2; } ; static void Cyc_Splay_lift( struct Cyc_List_List*
dnl){ while( dnl != 0) { if((( struct Cyc_List_List*) _check_null( dnl))->tl ==
0){{ struct _tuple0* _temp46=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl))->hd; struct _tuple0 _temp52; struct Cyc_Splay_noderef*
_temp53; void* _temp55; struct _tuple0 _temp57; struct Cyc_Splay_noderef*
_temp58; void* _temp60; _LL48: _temp52=* _temp46; _LL56: _temp55= _temp52.f1;
if( _temp55 ==( void*) Cyc_Splay_LEFT){ goto _LL54;} else{ goto _LL50;} _LL54:
_temp53= _temp52.f2; goto _LL49; _LL50: _temp57=* _temp46; _LL61: _temp60=
_temp57.f1; if( _temp60 ==( void*) Cyc_Splay_RIGHT){ goto _LL59;} else{ goto
_LL47;} _LL59: _temp58= _temp57.f2; goto _LL51; _LL49:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_left)( _temp53); goto _LL47; _LL51:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_right)( _temp58); goto _LL47; _LL47:;} return;}{ struct
_tuple0 _temp64; struct Cyc_Splay_noderef* _temp65; void* _temp67; struct
_tuple0* _temp62=( struct _tuple0*)(( struct Cyc_List_List*) _check_null( dnl))->hd;
_temp64=* _temp62; _LL68: _temp67= _temp64.f1; goto _LL66; _LL66: _temp65=
_temp64.f2; goto _LL63; _LL63: { struct _tuple0 _temp71; struct Cyc_Splay_noderef*
_temp72; void* _temp74; struct _tuple0* _temp69=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( dnl))->tl))->hd; _temp71=*
_temp69; _LL75: _temp74= _temp71.f1; goto _LL73; _LL73: _temp72= _temp71.f2;
goto _LL70; _LL70: dnl=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( dnl))->tl))->tl;{ void* _temp76= _temp67; _LL78: if( _temp76 ==(
void*) Cyc_Splay_LEFT){ goto _LL79;} else{ goto _LL80;} _LL80: if( _temp76 ==(
void*) Cyc_Splay_RIGHT){ goto _LL81;} else{ goto _LL77;} _LL79:{ void* _temp82=
_temp74; _LL84: if( _temp82 ==( void*) Cyc_Splay_LEFT){ goto _LL85;} else{ goto
_LL86;} _LL86: if( _temp82 ==( void*) Cyc_Splay_RIGHT){ goto _LL87;} else{ goto
_LL83;} _LL85:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp72);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp72); goto _LL83; _LL87:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp65);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp72); goto _LL83; _LL83:;} goto _LL77; _LL81:{ void* _temp88= _temp74; _LL90:
if( _temp88 ==( void*) Cyc_Splay_LEFT){ goto _LL91;} else{ goto _LL92;} _LL92:
if( _temp88 ==( void*) Cyc_Splay_RIGHT){ goto _LL93;} else{ goto _LL89;} _LL91:((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)( _temp65);((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp72); goto
_LL89; _LL93:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp72);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp72); goto _LL89; _LL89:;} goto _LL77; _LL77:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp94= tree; struct Cyc_Splay_noderef* _temp100;
_LL96: if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Splay_Node: 0){
_LL101: _temp100=(( struct Cyc_Splay_Node_struct*) _temp94)->f1; goto _LL97;}
else{ goto _LL98;} _LL98: if( _temp94 ==( void*) Cyc_Splay_Leaf){ goto _LL99;}
else{ goto _LL95;} _LL97: { struct Cyc_Splay_node* _temp102= _temp100->v; goto
_LL103; _LL103: { int _temp104= reln( reln_first_arg,( void*) _temp102->key);
goto _LL105; _LL105: if( _temp104 == 0){(( void(*)( struct Cyc_List_List* dnl))
Cyc_Splay_lift)( path); return 1;} else{ if( _temp104 < 0){ path=({ struct Cyc_List_List*
_temp106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp106->hd=( void*)({ struct _tuple0* _temp107=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp107->f1=( void*) Cyc_Splay_LEFT; _temp107->f2=
_temp100; _temp107;}); _temp106->tl= path; _temp106;}); tree=( void*) _temp102->left;}
else{ path=({ struct Cyc_List_List* _temp108=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp108->hd=( void*)({ struct _tuple0* _temp109=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp109->f1=( void*) Cyc_Splay_RIGHT;
_temp109->f2= _temp100; _temp109;}); _temp108->tl= path; _temp108;}); tree=(
void*) _temp102->right;}} goto _LL95;}} _LL99: if( path != 0){(( void(*)( struct
Cyc_List_List* dnl)) Cyc_Splay_lift)((( struct Cyc_List_List*) _check_null( path))->tl);}
return 0; _LL95:;}}