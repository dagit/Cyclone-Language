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
Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp0= nr->v; goto _LL1; _LL1: {
void* _temp2=( void*) _temp0->left; struct Cyc_Splay_noderef* _temp8; _LL4: if((
unsigned int) _temp2 > 1u?*(( int*) _temp2) == Cyc_Splay_Node: 0){ _LL9: _temp8=((
struct Cyc_Splay_Node_struct*) _temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6:
goto _LL7; _LL5: { struct Cyc_Splay_node* _temp10= _temp8->v; goto _LL11; _LL11: {
struct Cyc_Splay_Node_struct* _temp16=({ struct Cyc_Splay_Node_struct* _temp12=(
struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
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
_temp18;}); goto _LL3;}} _LL7:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp19=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp19[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp20; _temp20.tag= Cyc_Core_InvalidArg;
_temp20.f1= _tag_arr("Splay::rotate_left", sizeof( unsigned char), 19u); _temp20;});
_temp19;})); _LL3:;}} static void Cyc_Splay_rotate_right( struct Cyc_Splay_noderef*
nr){ struct Cyc_Splay_node* _temp21= nr->v; goto _LL22; _LL22: { void* _temp23=(
void*) _temp21->right; struct Cyc_Splay_noderef* _temp29; _LL25: if((
unsigned int) _temp23 > 1u?*(( int*) _temp23) == Cyc_Splay_Node: 0){ _LL30:
_temp29=(( struct Cyc_Splay_Node_struct*) _temp23)->f1; goto _LL26;} else{ goto
_LL27;} _LL27: goto _LL28; _LL26: { struct Cyc_Splay_node* _temp31= _temp29->v;
goto _LL32; _LL32: { struct Cyc_Splay_Node_struct* _temp37=({ struct Cyc_Splay_Node_struct*
_temp33=( struct Cyc_Splay_Node_struct*) GC_malloc( sizeof( struct Cyc_Splay_Node_struct));
_temp33[ 0]=({ struct Cyc_Splay_Node_struct _temp34; _temp34.tag= Cyc_Splay_Node;
_temp34.f1=({ struct Cyc_Splay_noderef* _temp35=( struct Cyc_Splay_noderef*)
GC_malloc( sizeof( struct Cyc_Splay_noderef)); _temp35->v=({ struct Cyc_Splay_node*
_temp36=( struct Cyc_Splay_node*) GC_malloc( sizeof( struct Cyc_Splay_node));
_temp36->key=( void*)(( void*) _temp21->key); _temp36->data=( void*)(( void*)
_temp21->data); _temp36->left=( void*)(( void*) _temp21->left); _temp36->right=(
void*)(( void*) _temp31->left); _temp36;}); _temp35;}); _temp34;}); _temp33;});
goto _LL38; _LL38: nr->v=({ struct Cyc_Splay_node* _temp39=( struct Cyc_Splay_node*)
GC_malloc( sizeof( struct Cyc_Splay_node)); _temp39->key=( void*)(( void*)
_temp31->key); _temp39->data=( void*)(( void*) _temp31->data); _temp39->left=(
void*)(( void*) _temp37); _temp39->right=( void*)(( void*) _temp31->right);
_temp39;}); goto _LL24;}} _LL28:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp40=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp40[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp41; _temp41.tag= Cyc_Core_InvalidArg;
_temp41.f1= _tag_arr("Splay::rotate_right", sizeof( unsigned char), 20u);
_temp41;}); _temp40;})); _LL24:;}} struct _tuple0{ void* f1; struct Cyc_Splay_noderef*
f2; } ; static void Cyc_Splay_lift( struct Cyc_List_List* dnl){ while( dnl != 0) {
if((( struct Cyc_List_List*) _check_null( dnl))->tl == 0){{ struct _tuple0*
_temp42=( struct _tuple0*)(( struct Cyc_List_List*) _check_null( dnl))->hd;
struct _tuple0 _temp48; struct Cyc_Splay_noderef* _temp49; void* _temp51; struct
_tuple0 _temp53; struct Cyc_Splay_noderef* _temp54; void* _temp56; _LL44:
_temp48=* _temp42; _LL52: _temp51= _temp48.f1; if( _temp51 ==( void*) Cyc_Splay_LEFT){
goto _LL50;} else{ goto _LL46;} _LL50: _temp49= _temp48.f2; goto _LL45; _LL46:
_temp53=* _temp42; _LL57: _temp56= _temp53.f1; if( _temp56 ==( void*) Cyc_Splay_RIGHT){
goto _LL55;} else{ goto _LL43;} _LL55: _temp54= _temp53.f2; goto _LL47; _LL45:((
void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)( _temp49); goto
_LL43; _LL47:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp54); goto _LL43; _LL43:;} return;}{ struct _tuple0 _temp60; struct Cyc_Splay_noderef*
_temp61; void* _temp63; struct _tuple0* _temp58=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl))->hd; _temp60=* _temp58; _LL64: _temp63= _temp60.f1; goto
_LL62; _LL62: _temp61= _temp60.f2; goto _LL59; _LL59: { struct _tuple0 _temp67;
struct Cyc_Splay_noderef* _temp68; void* _temp70; struct _tuple0* _temp65=(
struct _tuple0*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( dnl))->tl))->hd; _temp67=* _temp65; _LL71: _temp70= _temp67.f1;
goto _LL69; _LL69: _temp68= _temp67.f2; goto _LL66; _LL66: dnl=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( dnl))->tl))->tl;{ void*
_temp72= _temp63; _LL74: if( _temp72 ==( void*) Cyc_Splay_LEFT){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp72 ==( void*) Cyc_Splay_RIGHT){ goto _LL77;}
else{ goto _LL73;} _LL75:{ void* _temp78= _temp70; _LL80: if( _temp78 ==( void*)
Cyc_Splay_LEFT){ goto _LL81;} else{ goto _LL82;} _LL82: if( _temp78 ==( void*)
Cyc_Splay_RIGHT){ goto _LL83;} else{ goto _LL79;} _LL81:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_left)( _temp68);(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_left)( _temp68); goto _LL79; _LL83:(( void(*)( struct Cyc_Splay_noderef*
nr)) Cyc_Splay_rotate_left)( _temp61);(( void(*)( struct Cyc_Splay_noderef* nr))
Cyc_Splay_rotate_right)( _temp68); goto _LL79; _LL79:;} goto _LL73; _LL77:{ void*
_temp84= _temp70; _LL86: if( _temp84 ==( void*) Cyc_Splay_LEFT){ goto _LL87;}
else{ goto _LL88;} _LL88: if( _temp84 ==( void*) Cyc_Splay_RIGHT){ goto _LL89;}
else{ goto _LL85;} _LL87:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp61);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_left)(
_temp68); goto _LL85; _LL89:(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp68);(( void(*)( struct Cyc_Splay_noderef* nr)) Cyc_Splay_rotate_right)(
_temp68); goto _LL85; _LL85:;} goto _LL73; _LL73:;}}}}} int Cyc_Splay_splay( int(*
reln)( void*, void*), void* reln_first_arg, void* tree){ struct Cyc_List_List*
path= 0; while( 1) { void* _temp90= tree; struct Cyc_Splay_noderef* _temp96;
_LL92: if(( unsigned int) _temp90 > 1u?*(( int*) _temp90) == Cyc_Splay_Node: 0){
_LL97: _temp96=(( struct Cyc_Splay_Node_struct*) _temp90)->f1; goto _LL93;}
else{ goto _LL94;} _LL94: if( _temp90 ==( void*) Cyc_Splay_Leaf){ goto _LL95;}
else{ goto _LL91;} _LL93: { struct Cyc_Splay_node* _temp98= _temp96->v; goto
_LL99; _LL99: { int _temp100= reln( reln_first_arg,( void*) _temp98->key); goto
_LL101; _LL101: if( _temp100 == 0){(( void(*)( struct Cyc_List_List* dnl)) Cyc_Splay_lift)(
path); return 1;} else{ if( _temp100 < 0){ path=({ struct Cyc_List_List*
_temp102=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp102->hd=( void*)({ struct _tuple0* _temp103=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp103->f1=( void*) Cyc_Splay_LEFT; _temp103->f2=
_temp96; _temp103;}); _temp102->tl= path; _temp102;}); tree=( void*) _temp98->left;}
else{ path=({ struct Cyc_List_List* _temp104=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp104->hd=( void*)({ struct _tuple0* _temp105=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp105->f1=( void*) Cyc_Splay_RIGHT;
_temp105->f2= _temp96; _temp105;}); _temp104->tl= path; _temp104;}); tree=( void*)
_temp98->right;}} goto _LL91;}} _LL95: if( path != 0){(( void(*)( struct Cyc_List_List*
dnl)) Cyc_Splay_lift)((( struct Cyc_List_List*) _check_null( path))->tl);}
return 0; _LL91:;}}