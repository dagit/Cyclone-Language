 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Splay_node; struct Cyc_Splay_noderef{
struct Cyc_Splay_node* v; } ; static const int Cyc_Splay_Leaf= 0; static const
int Cyc_Splay_Node= 0; struct Cyc_Splay_Node_struct{ int tag; struct Cyc_Splay_noderef*
f1; } ; struct Cyc_Splay_node{ void* key; void* data; void* left; void* right; }
; extern int Cyc_Splay_rsplay( struct _RegionHandle*, int(* f)( void*, void*),
void*, void*); extern int Cyc_Splay_splay( int(* f)( void*, void*), void*, void*);
static const int Cyc_Splay_LEFT= 0; static const int Cyc_Splay_RIGHT= 1; static
void Cyc_Splay_rotate_left( struct _RegionHandle* r, struct Cyc_Splay_noderef*
nr){ struct Cyc_Splay_node* _temp0= nr->v; void* _temp1=( void*) _temp0->left;
struct Cyc_Splay_noderef* _temp7; _LL3: if(( unsigned int) _temp1 >  1u?*(( int*)
_temp1) ==  Cyc_Splay_Node: 0){ _LL8: _temp7=(( struct Cyc_Splay_Node_struct*)
_temp1)->f1; goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_Splay_node*
_temp9= _temp7->v; struct Cyc_Splay_Node_struct* _temp10=({ struct Cyc_Splay_Node_struct*
_temp12=( struct Cyc_Splay_Node_struct*) _region_malloc( r, sizeof( struct Cyc_Splay_Node_struct));
_temp12[ 0]=({ struct Cyc_Splay_Node_struct _temp13; _temp13.tag= Cyc_Splay_Node;
_temp13.f1=({ struct Cyc_Splay_noderef* _temp14=( struct Cyc_Splay_noderef*)
_region_malloc( r, sizeof( struct Cyc_Splay_noderef)); _temp14->v=({ struct Cyc_Splay_node*
_temp15=( struct Cyc_Splay_node*) _region_malloc( r, sizeof( struct Cyc_Splay_node));
_temp15->key=( void*)(( void*) _temp0->key); _temp15->data=( void*)(( void*)
_temp0->data); _temp15->left=( void*)(( void*) _temp9->right); _temp15->right=(
void*)(( void*) _temp0->right); _temp15;}); _temp14;}); _temp13;}); _temp12;});
nr->v=({ struct Cyc_Splay_node* _temp11=( struct Cyc_Splay_node*) _region_malloc(
r, sizeof( struct Cyc_Splay_node)); _temp11->key=( void*)(( void*) _temp9->key);
_temp11->data=( void*)(( void*) _temp9->data); _temp11->left=( void*)(( void*)
_temp9->left); _temp11->right=( void*)(( void*) _temp10); _temp11;}); goto _LL2;}
_LL6:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp16=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp16[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp17; _temp17.tag= Cyc_Core_InvalidArg;
_temp17.f1= _tag_arr("Splay::rotate_left", sizeof( unsigned char), 19u); _temp17;});
_temp16;})); _LL2:;} static void Cyc_Splay_rotate_right( struct _RegionHandle* r,
struct Cyc_Splay_noderef* nr){ struct Cyc_Splay_node* _temp18= nr->v; void*
_temp19=( void*) _temp18->right; struct Cyc_Splay_noderef* _temp25; _LL21: if((
unsigned int) _temp19 >  1u?*(( int*) _temp19) ==  Cyc_Splay_Node: 0){ _LL26:
_temp25=(( struct Cyc_Splay_Node_struct*) _temp19)->f1; goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL22: { struct Cyc_Splay_node* _temp27= _temp25->v;
struct Cyc_Splay_Node_struct* _temp28=({ struct Cyc_Splay_Node_struct* _temp30=(
struct Cyc_Splay_Node_struct*) _region_malloc( r, sizeof( struct Cyc_Splay_Node_struct));
_temp30[ 0]=({ struct Cyc_Splay_Node_struct _temp31; _temp31.tag= Cyc_Splay_Node;
_temp31.f1=({ struct Cyc_Splay_noderef* _temp32=( struct Cyc_Splay_noderef*)
_region_malloc( r, sizeof( struct Cyc_Splay_noderef)); _temp32->v=({ struct Cyc_Splay_node*
_temp33=( struct Cyc_Splay_node*) _region_malloc( r, sizeof( struct Cyc_Splay_node));
_temp33->key=( void*)(( void*) _temp18->key); _temp33->data=( void*)(( void*)
_temp18->data); _temp33->left=( void*)(( void*) _temp18->left); _temp33->right=(
void*)(( void*) _temp27->left); _temp33;}); _temp32;}); _temp31;}); _temp30;});
nr->v=({ struct Cyc_Splay_node* _temp29=( struct Cyc_Splay_node*) _region_malloc(
r, sizeof( struct Cyc_Splay_node)); _temp29->key=( void*)(( void*) _temp27->key);
_temp29->data=( void*)(( void*) _temp27->data); _temp29->left=( void*)(( void*)
_temp28); _temp29->right=( void*)(( void*) _temp27->right); _temp29;}); goto
_LL20;} _LL24:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp34=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp34[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp35; _temp35.tag= Cyc_Core_InvalidArg;
_temp35.f1= _tag_arr("Splay::rotate_right", sizeof( unsigned char), 20u);
_temp35;}); _temp34;})); _LL20:;} struct _tuple0{ void* f1; struct Cyc_Splay_noderef*
f2; } ; static void Cyc_Splay_lift( struct _RegionHandle* r, struct Cyc_List_List*
dnl){ while( dnl !=  0) { if((( struct Cyc_List_List*) _check_null( dnl))->tl == 
0){{ struct _tuple0* _temp36=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl))->hd; struct _tuple0 _temp42; struct Cyc_Splay_noderef*
_temp43; void* _temp45; struct _tuple0 _temp47; struct Cyc_Splay_noderef*
_temp48; void* _temp50; _LL38: _temp42=* _temp36; _LL46: _temp45= _temp42.f1;
if( _temp45 == ( void*) Cyc_Splay_LEFT){ goto _LL44;} else{ goto _LL40;} _LL44:
_temp43= _temp42.f2; goto _LL39; _LL40: _temp47=* _temp36; _LL51: _temp50=
_temp47.f1; if( _temp50 == ( void*) Cyc_Splay_RIGHT){ goto _LL49;} else{ goto
_LL37;} _LL49: _temp48= _temp47.f2; goto _LL41; _LL39: Cyc_Splay_rotate_left( r,
_temp43); goto _LL37; _LL41: Cyc_Splay_rotate_right( r, _temp48); goto _LL37;
_LL37:;} return;}{ struct _tuple0 _temp54; struct Cyc_Splay_noderef* _temp55;
void* _temp57; struct _tuple0* _temp52=( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( dnl))->hd; _temp54=* _temp52; _LL58: _temp57= _temp54.f1; goto
_LL56; _LL56: _temp55= _temp54.f2; goto _LL53; _LL53: { struct _tuple0 _temp61;
struct Cyc_Splay_noderef* _temp62; void* _temp64; struct _tuple0* _temp59=(
struct _tuple0*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( dnl))->tl))->hd; _temp61=* _temp59; _LL65: _temp64= _temp61.f1;
goto _LL63; _LL63: _temp62= _temp61.f2; goto _LL60; _LL60: dnl=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( dnl))->tl))->tl;{ void*
_temp66= _temp57; _LL68: if( _temp66 == ( void*) Cyc_Splay_LEFT){ goto _LL69;}
else{ goto _LL70;} _LL70: if( _temp66 == ( void*) Cyc_Splay_RIGHT){ goto _LL71;}
else{ goto _LL67;} _LL69:{ void* _temp72= _temp64; _LL74: if( _temp72 == ( void*)
Cyc_Splay_LEFT){ goto _LL75;} else{ goto _LL76;} _LL76: if( _temp72 == ( void*)
Cyc_Splay_RIGHT){ goto _LL77;} else{ goto _LL73;} _LL75: Cyc_Splay_rotate_left(
r, _temp62); Cyc_Splay_rotate_left( r, _temp62); goto _LL73; _LL77: Cyc_Splay_rotate_left(
r, _temp55); Cyc_Splay_rotate_right( r, _temp62); goto _LL73; _LL73:;} goto
_LL67; _LL71:{ void* _temp78= _temp64; _LL80: if( _temp78 == ( void*) Cyc_Splay_LEFT){
goto _LL81;} else{ goto _LL82;} _LL82: if( _temp78 == ( void*) Cyc_Splay_RIGHT){
goto _LL83;} else{ goto _LL79;} _LL81: Cyc_Splay_rotate_right( r, _temp55); Cyc_Splay_rotate_left(
r, _temp62); goto _LL79; _LL83: Cyc_Splay_rotate_right( r, _temp62); Cyc_Splay_rotate_right(
r, _temp62); goto _LL79; _LL79:;} goto _LL67; _LL67:;}}}}} int Cyc_Splay_rsplay(
struct _RegionHandle* r, int(* reln)( void*, void*), void* reln_first_arg, void*
tree){ struct _RegionHandle _temp84= _new_region(); struct _RegionHandle* temp=&
_temp84; _push_region( temp);{ struct Cyc_List_List* path= 0; while( 1) { void*
_temp85= tree; struct Cyc_Splay_noderef* _temp91; _LL87: if(( unsigned int)
_temp85 >  1u?*(( int*) _temp85) ==  Cyc_Splay_Node: 0){ _LL92: _temp91=((
struct Cyc_Splay_Node_struct*) _temp85)->f1; goto _LL88;} else{ goto _LL89;}
_LL89: if( _temp85 == ( void*) Cyc_Splay_Leaf){ goto _LL90;} else{ goto _LL86;}
_LL88: { struct Cyc_Splay_node* _temp93= _temp91->v; int _temp94= reln(
reln_first_arg,( void*) _temp93->key); if( _temp94 ==  0){ Cyc_Splay_lift( r,
path);{ int _temp95= 1; _npop_handler( 0u); return _temp95;}} else{ if( _temp94
<  0){ path=({ struct Cyc_List_List* _temp96=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp96->hd=( void*)({
struct _tuple0* _temp97=( struct _tuple0*) _region_malloc( temp, sizeof( struct
_tuple0)); _temp97->f1=( void*) Cyc_Splay_LEFT; _temp97->f2= _temp91; _temp97;});
_temp96->tl= path; _temp96;}); tree=( void*) _temp93->left;} else{ path=({
struct Cyc_List_List* _temp98=( struct Cyc_List_List*) _region_malloc( temp,
sizeof( struct Cyc_List_List)); _temp98->hd=( void*)({ struct _tuple0* _temp99=(
struct _tuple0*) _region_malloc( temp, sizeof( struct _tuple0)); _temp99->f1=(
void*) Cyc_Splay_RIGHT; _temp99->f2= _temp91; _temp99;}); _temp98->tl= path;
_temp98;}); tree=( void*) _temp93->right;}} goto _LL86;} _LL90: if( path !=  0){
Cyc_Splay_lift( r,(( struct Cyc_List_List*) _check_null( path))->tl);}{ int
_temp100= 0; _npop_handler( 0u); return _temp100;} _LL86:;}}; _pop_region( temp);}
int Cyc_Splay_splay( int(* reln)( void*, void*), void* reln_first_arg, void*
tree){ return Cyc_Splay_rsplay( Cyc_Core_heap_region, reln, reln_first_arg, tree);}
