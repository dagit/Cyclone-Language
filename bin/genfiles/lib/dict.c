#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct Cyc_Dict_T*
f2; struct Cyc_Dict_T* f3; struct _tuple0 f4; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char*
tag; } ; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data); extern
struct Cyc_Dict_Dict* Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
kds); extern struct Cyc_Dict_Dict* Cyc_Dict_singleton( int(* comp)( void*, void*),
void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void*
key, void** ans_place); extern void* Cyc_Dict_fold( void*(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d, void* accum); extern void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* dict,
void* accum); extern void Cyc_Dict_app( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern void Cyc_Dict_app_c( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter( void(* f)( void*, void*),
struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter2_c( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_union_two(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c( int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); extern struct _tuple0* Cyc_Dict_choose( struct Cyc_Dict_Dict* d); extern
struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d); char Cyc_Dict_Absent_tag[
7u]="Absent"; char Cyc_Dict_Present_tag[ 8u]="Present"; typedef void* Cyc_Dict_Color;
static const unsigned int Cyc_Dict_R= 0; static const unsigned int Cyc_Dict_B= 1;
struct Cyc_Dict_T{ void* color; struct Cyc_Dict_T* left; struct Cyc_Dict_T*
right; struct _tuple0 key_val; } ; typedef struct Cyc_Dict_T* Cyc_Dict_tree;
struct Cyc_Dict_Dict{ int(* rel)( void*, void*); struct Cyc_Dict_T* t; } ;
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)){ return({
struct Cyc_Dict_Dict* _temp0=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct
Cyc_Dict_Dict)); _temp0->rel= comp; _temp0->t= 0; _temp0;});} struct Cyc_Dict_Dict*
Cyc_Dict_singleton( int(* comp)( void*, void*), void* key, void* data){ return({
struct Cyc_Dict_Dict* _temp1=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct
Cyc_Dict_Dict)); _temp1->rel= comp; _temp1->t=({ struct Cyc_Dict_T* _temp2=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp2->color=( void*)((
void*) Cyc_Dict_B); _temp2->left= 0; _temp2->right= 0; _temp2->key_val=({ struct
_tuple0 _temp3; _temp3.f1= key; _temp3.f2= data; _temp3;}); _temp2;}); _temp1;});}
int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d){ return d->t == 0;} int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*, void*)= d->rel; struct
Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1); if( i <
0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{ return 1;}}} return 0;}
void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*,
void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1);
if( i < 0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{ return( t->key_val).f2;}}}(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp4=(
struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp4[ 0]=({ struct Cyc_Dict_Absent_struct _temp5; _temp5.tag= Cyc_Dict_Absent_tag;
_temp5;}); _temp4;}));} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t;
while( t != 0) { int i= rel( key,( t->key_val).f1); if( i < 0){ t= t->left;}
else{ if( i > 0){ t= t->right;} else{ return({ struct Cyc_Core_Opt* _temp6=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6->v=( void*)(
t->key_val).f2; _temp6;});}}} return 0;} int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T*
t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1); if( i < 0){ t= t->left;}
else{ if( i > 0){ t= t->right;} else{* ans_place=( t->key_val).f2; return 1;}}}
return 0;} static struct Cyc_Dict_T* Cyc_Dict_balance( struct _tuple1 quad){
struct _tuple1 _temp7= quad; struct _tuple0 _temp19; struct Cyc_Dict_T* _temp21;
struct Cyc_Dict_T* _temp23; struct Cyc_Dict_T _temp25; struct _tuple0 _temp26;
struct Cyc_Dict_T* _temp28; struct Cyc_Dict_T* _temp30; struct Cyc_Dict_T
_temp32; struct _tuple0 _temp33; struct Cyc_Dict_T* _temp35; struct Cyc_Dict_T*
_temp37; void* _temp39; void* _temp41; void* _temp43; struct _tuple0 _temp45;
struct Cyc_Dict_T* _temp47; struct Cyc_Dict_T* _temp49; struct Cyc_Dict_T
_temp51; struct _tuple0 _temp52; struct Cyc_Dict_T* _temp54; struct Cyc_Dict_T
_temp56; struct _tuple0 _temp57; struct Cyc_Dict_T* _temp59; struct Cyc_Dict_T*
_temp61; void* _temp63; struct Cyc_Dict_T* _temp65; void* _temp67; void* _temp69;
struct _tuple0 _temp71; struct Cyc_Dict_T* _temp73; struct Cyc_Dict_T _temp75;
struct _tuple0 _temp76; struct Cyc_Dict_T* _temp78; struct Cyc_Dict_T* _temp80;
struct Cyc_Dict_T _temp82; struct _tuple0 _temp83; struct Cyc_Dict_T* _temp85;
struct Cyc_Dict_T* _temp87; void* _temp89; void* _temp91; struct Cyc_Dict_T*
_temp93; void* _temp95; struct _tuple0 _temp97; struct Cyc_Dict_T* _temp99;
struct Cyc_Dict_T _temp101; struct _tuple0 _temp102; struct Cyc_Dict_T* _temp104;
struct Cyc_Dict_T _temp106; struct _tuple0 _temp107; struct Cyc_Dict_T* _temp109;
struct Cyc_Dict_T* _temp111; void* _temp113; struct Cyc_Dict_T* _temp115; void*
_temp117; struct Cyc_Dict_T* _temp119; void* _temp121; struct _tuple0 _temp123;
struct Cyc_Dict_T* _temp125; struct Cyc_Dict_T* _temp127; void* _temp129; _LL9:
_LL44: _temp43= _temp7.f1; if(( int) _temp43 == Cyc_Dict_B){ goto _LL24;} else{
goto _LL11;} _LL24: _temp23= _temp7.f2; if( _temp23 == 0){ goto _LL11;} else{
_temp25=* _temp23; _LL42: _temp41=( void*) _temp25.color; if(( int) _temp41 ==
Cyc_Dict_R){ goto _LL31;} else{ goto _LL11;} _LL31: _temp30=( struct Cyc_Dict_T*)
_temp25.left; if( _temp30 == 0){ goto _LL11;} else{ _temp32=* _temp30; _LL40:
_temp39=( void*) _temp32.color; if(( int) _temp39 == Cyc_Dict_R){ goto _LL38;}
else{ goto _LL11;} _LL38: _temp37=( struct Cyc_Dict_T*) _temp32.left; goto _LL36;
_LL36: _temp35=( struct Cyc_Dict_T*) _temp32.right; goto _LL34; _LL34: _temp33=(
struct _tuple0) _temp32.key_val; goto _LL29;} _LL29: _temp28=( struct Cyc_Dict_T*)
_temp25.right; goto _LL27; _LL27: _temp26=( struct _tuple0) _temp25.key_val;
goto _LL22;} _LL22: _temp21= _temp7.f3; goto _LL20; _LL20: _temp19= _temp7.f4;
goto _LL10; _LL11: _LL70: _temp69= _temp7.f1; if(( int) _temp69 == Cyc_Dict_B){
goto _LL50;} else{ goto _LL13;} _LL50: _temp49= _temp7.f2; if( _temp49 == 0){
goto _LL13;} else{ _temp51=* _temp49; _LL68: _temp67=( void*) _temp51.color; if((
int) _temp67 == Cyc_Dict_R){ goto _LL66;} else{ goto _LL13;} _LL66: _temp65=(
struct Cyc_Dict_T*) _temp51.left; goto _LL55; _LL55: _temp54=( struct Cyc_Dict_T*)
_temp51.right; if( _temp54 == 0){ goto _LL13;} else{ _temp56=* _temp54; _LL64:
_temp63=( void*) _temp56.color; if(( int) _temp63 == Cyc_Dict_R){ goto _LL62;}
else{ goto _LL13;} _LL62: _temp61=( struct Cyc_Dict_T*) _temp56.left; goto _LL60;
_LL60: _temp59=( struct Cyc_Dict_T*) _temp56.right; goto _LL58; _LL58: _temp57=(
struct _tuple0) _temp56.key_val; goto _LL53;} _LL53: _temp52=( struct _tuple0)
_temp51.key_val; goto _LL48;} _LL48: _temp47= _temp7.f3; goto _LL46; _LL46:
_temp45= _temp7.f4; goto _LL12; _LL13: _LL96: _temp95= _temp7.f1; if(( int)
_temp95 == Cyc_Dict_B){ goto _LL94;} else{ goto _LL15;} _LL94: _temp93= _temp7.f2;
goto _LL74; _LL74: _temp73= _temp7.f3; if( _temp73 == 0){ goto _LL15;} else{
_temp75=* _temp73; _LL92: _temp91=( void*) _temp75.color; if(( int) _temp91 ==
Cyc_Dict_R){ goto _LL81;} else{ goto _LL15;} _LL81: _temp80=( struct Cyc_Dict_T*)
_temp75.left; if( _temp80 == 0){ goto _LL15;} else{ _temp82=* _temp80; _LL90:
_temp89=( void*) _temp82.color; if(( int) _temp89 == Cyc_Dict_R){ goto _LL88;}
else{ goto _LL15;} _LL88: _temp87=( struct Cyc_Dict_T*) _temp82.left; goto _LL86;
_LL86: _temp85=( struct Cyc_Dict_T*) _temp82.right; goto _LL84; _LL84: _temp83=(
struct _tuple0) _temp82.key_val; goto _LL79;} _LL79: _temp78=( struct Cyc_Dict_T*)
_temp75.right; goto _LL77; _LL77: _temp76=( struct _tuple0) _temp75.key_val;
goto _LL72;} _LL72: _temp71= _temp7.f4; goto _LL14; _LL15: _LL122: _temp121=
_temp7.f1; if(( int) _temp121 == Cyc_Dict_B){ goto _LL120;} else{ goto _LL17;}
_LL120: _temp119= _temp7.f2; goto _LL100; _LL100: _temp99= _temp7.f3; if(
_temp99 == 0){ goto _LL17;} else{ _temp101=* _temp99; _LL118: _temp117=( void*)
_temp101.color; if(( int) _temp117 == Cyc_Dict_R){ goto _LL116;} else{ goto
_LL17;} _LL116: _temp115=( struct Cyc_Dict_T*) _temp101.left; goto _LL105;
_LL105: _temp104=( struct Cyc_Dict_T*) _temp101.right; if( _temp104 == 0){ goto
_LL17;} else{ _temp106=* _temp104; _LL114: _temp113=( void*) _temp106.color; if((
int) _temp113 == Cyc_Dict_R){ goto _LL112;} else{ goto _LL17;} _LL112: _temp111=(
struct Cyc_Dict_T*) _temp106.left; goto _LL110; _LL110: _temp109=( struct Cyc_Dict_T*)
_temp106.right; goto _LL108; _LL108: _temp107=( struct _tuple0) _temp106.key_val;
goto _LL103;} _LL103: _temp102=( struct _tuple0) _temp101.key_val; goto _LL98;}
_LL98: _temp97= _temp7.f4; goto _LL16; _LL17: _LL130: _temp129= _temp7.f1; goto
_LL128; _LL128: _temp127= _temp7.f2; goto _LL126; _LL126: _temp125= _temp7.f3;
goto _LL124; _LL124: _temp123= _temp7.f4; goto _LL18; _LL10: return({ struct Cyc_Dict_T*
_temp131=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp131->color=(
void*)(( void*) Cyc_Dict_R); _temp131->left=({ struct Cyc_Dict_T* _temp133=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp133->color=(
void*)(( void*) Cyc_Dict_B); _temp133->left= _temp37; _temp133->right= _temp35;
_temp133->key_val= _temp33; _temp133;}); _temp131->right=({ struct Cyc_Dict_T*
_temp132=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp132->color=(
void*)(( void*) Cyc_Dict_B); _temp132->left= _temp28; _temp132->right= _temp21;
_temp132->key_val= _temp19; _temp132;}); _temp131->key_val= _temp26; _temp131;});
_LL12: return({ struct Cyc_Dict_T* _temp134=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp134->color=( void*)(( void*) Cyc_Dict_R);
_temp134->left=({ struct Cyc_Dict_T* _temp136=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp136->color=( void*)(( void*) Cyc_Dict_B);
_temp136->left= _temp65; _temp136->right= _temp61; _temp136->key_val= _temp52;
_temp136;}); _temp134->right=({ struct Cyc_Dict_T* _temp135=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp135->color=( void*)(( void*) Cyc_Dict_B);
_temp135->left= _temp59; _temp135->right= _temp47; _temp135->key_val= _temp45;
_temp135;}); _temp134->key_val= _temp57; _temp134;}); _LL14: return({ struct Cyc_Dict_T*
_temp137=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp137->color=(
void*)(( void*) Cyc_Dict_R); _temp137->left=({ struct Cyc_Dict_T* _temp139=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp139->color=(
void*)(( void*) Cyc_Dict_B); _temp139->left= _temp93; _temp139->right= _temp87;
_temp139->key_val= _temp71; _temp139;}); _temp137->right=({ struct Cyc_Dict_T*
_temp138=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp138->color=(
void*)(( void*) Cyc_Dict_B); _temp138->left= _temp85; _temp138->right= _temp78;
_temp138->key_val= _temp76; _temp138;}); _temp137->key_val= _temp83; _temp137;});
_LL16: return({ struct Cyc_Dict_T* _temp140=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp140->color=( void*)(( void*) Cyc_Dict_R);
_temp140->left=({ struct Cyc_Dict_T* _temp142=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp142->color=( void*)(( void*) Cyc_Dict_B);
_temp142->left= _temp119; _temp142->right= _temp115; _temp142->key_val= _temp97;
_temp142;}); _temp140->right=({ struct Cyc_Dict_T* _temp141=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp141->color=( void*)(( void*) Cyc_Dict_B);
_temp141->left= _temp111; _temp141->right= _temp109; _temp141->key_val= _temp107;
_temp141;}); _temp140->key_val= _temp102; _temp140;}); _LL18: return({ struct
Cyc_Dict_T* _temp143=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp143->color=( void*) _temp129; _temp143->left= _temp127; _temp143->right=
_temp125; _temp143->key_val= _temp123; _temp143;}); _LL8:;} static struct Cyc_Dict_T*
Cyc_Dict_ins( int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T* _temp144= t; struct Cyc_Dict_T _temp150; struct _tuple0
_temp151; struct Cyc_Dict_T* _temp153; struct Cyc_Dict_T* _temp155; void*
_temp157; _LL146: if( _temp144 == 0){ goto _LL147;} else{ goto _LL148;} _LL148:
if( _temp144 == 0){ goto _LL145;} else{ _temp150=* _temp144; _LL158: _temp157=(
void*) _temp150.color; goto _LL156; _LL156: _temp155=( struct Cyc_Dict_T*)
_temp150.left; goto _LL154; _LL154: _temp153=( struct Cyc_Dict_T*) _temp150.right;
goto _LL152; _LL152: _temp151=( struct _tuple0) _temp150.key_val; goto _LL149;}
_LL147: return({ struct Cyc_Dict_T* _temp159=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp159->color=( void*)(( void*) Cyc_Dict_R);
_temp159->left= 0; _temp159->right= 0; _temp159->key_val= key_val; _temp159;});
_LL149: { int i= rel( key_val.f1, _temp151.f1); if( i < 0){ return(( struct Cyc_Dict_T*(*)(
struct _tuple1 quad)) Cyc_Dict_balance)(({ struct _tuple1 _temp160; _temp160.f1=
_temp157; _temp160.f2=(( struct Cyc_Dict_T*(*)( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val,
_temp155); _temp160.f3= _temp153; _temp160.f4= _temp151; _temp160;}));} else{
if( i > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad)) Cyc_Dict_balance)(({
struct _tuple1 _temp161; _temp161.f1= _temp157; _temp161.f2= _temp155; _temp161.f3=((
struct Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct
Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val, _temp153); _temp161.f4= _temp151;
_temp161;}));} else{ return({ struct Cyc_Dict_T* _temp162=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp162->color=( void*) _temp157;
_temp162->left= _temp155; _temp162->right= _temp153; _temp162->key_val= key_val;
_temp162;});}}} _LL145:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data){ struct Cyc_Dict_T* ans=(( struct Cyc_Dict_T*(*)( int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
d->rel,({ struct _tuple0 _temp163; _temp163.f1= key; _temp163.f2= data; _temp163;}),
d->t);( void*)( ans->color=( void*)(( void*) Cyc_Dict_B)); return({ struct Cyc_Dict_Dict*
_temp164=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp164->rel= d->rel; _temp164->t= ans; _temp164;});} struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* key, void* data){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, key)){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp165=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp165[ 0]=({ struct Cyc_Dict_Absent_struct
_temp166; _temp166.tag= Cyc_Dict_Absent_tag; _temp166;}); _temp165;}));} return((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(
d, key, data);} struct Cyc_Dict_Dict* Cyc_Dict_inserts( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* kds){ for( 0; kds != 0; kds= kds->tl){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d,({ struct
_tuple0* _temp167=( struct _tuple0*) kds->hd; unsigned int _temp168= 0; if(
_temp168 >= 1u){ _throw( Null_Exception);} _temp167[ _temp168];}).f1,({ struct
_tuple0* _temp169=( struct _tuple0*) kds->hd; unsigned int _temp170= 0; if(
_temp170 >= 1u){ _throw( Null_Exception);} _temp169[ _temp170];}).f2);} return d;}
static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum){ struct Cyc_Dict_T _temp173; struct _tuple0 _temp174; void*
_temp176; void* _temp178; struct Cyc_Dict_T* _temp180; struct Cyc_Dict_T*
_temp182; void* _temp184; struct Cyc_Dict_T* _temp171= t; _temp173=* _temp171;
_LL185: _temp184=( void*) _temp173.color; goto _LL183; _LL183: _temp182=( struct
Cyc_Dict_T*) _temp173.left; goto _LL181; _LL181: _temp180=( struct Cyc_Dict_T*)
_temp173.right; goto _LL175; _LL175: _temp174=( struct _tuple0) _temp173.key_val;
_LL179: _temp178= _temp174.f1; goto _LL177; _LL177: _temp176= _temp174.f2; goto
_LL172; _LL172: accum= f( _temp178, _temp176, accum); if( _temp182 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum))
Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp186=
_temp182; if( _temp186 == 0){ _throw( Null_Exception);} _temp186;}), accum);}
if( _temp180 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*), struct
Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp187= _temp180; if( _temp187 == 0){ _throw(
Null_Exception);} _temp187;}), accum);} return accum;} void* Cyc_Dict_fold( void*(*
f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){
return accum;} return(( void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp188= d->t; if( _temp188 == 0){ _throw( Null_Exception);} _temp188;}), accum);}
static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*, void*, void*), void*
env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp191; struct
_tuple0 _temp192; void* _temp194; void* _temp196; struct Cyc_Dict_T* _temp198;
struct Cyc_Dict_T* _temp200; void* _temp202; struct Cyc_Dict_T* _temp189= t;
_temp191=* _temp189; _LL203: _temp202=( void*) _temp191.color; goto _LL201;
_LL201: _temp200=( struct Cyc_Dict_T*) _temp191.left; goto _LL199; _LL199:
_temp198=( struct Cyc_Dict_T*) _temp191.right; goto _LL193; _LL193: _temp192=(
struct _tuple0) _temp191.key_val; _LL197: _temp196= _temp192.f1; goto _LL195;
_LL195: _temp194= _temp192.f2; goto _LL190; _LL190: accum= f( env, _temp196,
_temp194, accum); if( _temp200 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp204= _temp200; if(
_temp204 == 0){ _throw( Null_Exception);} _temp204;}), accum);} if( _temp198 !=
0){ accum=(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp205= _temp198; if( _temp205 == 0){ _throw(
Null_Exception);} _temp205;}), accum);} return accum;} void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp206= d->t; if( _temp206
== 0){ _throw( Null_Exception);} _temp206;}), accum);} static void Cyc_Dict_app_tree(
void*(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp209;
struct _tuple0 _temp210; void* _temp212; void* _temp214; struct Cyc_Dict_T*
_temp216; struct Cyc_Dict_T* _temp218; void* _temp220; struct Cyc_Dict_T*
_temp207= t; _temp209=* _temp207; _LL221: _temp220=( void*) _temp209.color; goto
_LL219; _LL219: _temp218=( struct Cyc_Dict_T*) _temp209.left; goto _LL217;
_LL217: _temp216=( struct Cyc_Dict_T*) _temp209.right; goto _LL211; _LL211:
_temp210=( struct _tuple0) _temp209.key_val; _LL215: _temp214= _temp210.f1; goto
_LL213; _LL213: _temp212= _temp210.f2; goto _LL208; _LL208: f( _temp214,
_temp212); if( _temp218 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp222=
_temp218; if( _temp222 == 0){ _throw( Null_Exception);} _temp222;}));} if(
_temp216 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp223= _temp216; if( _temp223 ==
0){ _throw( Null_Exception);} _temp223;}));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp224= d->t; if( _temp224 == 0){ _throw( Null_Exception);}
_temp224;}));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp227; struct _tuple0
_temp228; void* _temp230; void* _temp232; struct Cyc_Dict_T* _temp234; struct
Cyc_Dict_T* _temp236; void* _temp238; struct Cyc_Dict_T* _temp225= t; _temp227=*
_temp225; _LL239: _temp238=( void*) _temp227.color; goto _LL237; _LL237:
_temp236=( struct Cyc_Dict_T*) _temp227.left; goto _LL235; _LL235: _temp234=(
struct Cyc_Dict_T*) _temp227.right; goto _LL229; _LL229: _temp228=( struct
_tuple0) _temp227.key_val; _LL233: _temp232= _temp228.f1; goto _LL231; _LL231:
_temp230= _temp228.f2; goto _LL226; _LL226: f( env, _temp232, _temp230); if(
_temp236 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp240= _temp236; if( _temp240 == 0){ _throw( Null_Exception);} _temp240;}));}
if( _temp234 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp241= _temp234; if( _temp241 == 0){ _throw(
Null_Exception);} _temp241;}));}} void Cyc_Dict_app_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp242= d->t; if( _temp242
== 0){ _throw( Null_Exception);} _temp242;}));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp245;
struct _tuple0 _temp246; void* _temp248; void* _temp250; struct Cyc_Dict_T*
_temp252; struct Cyc_Dict_T* _temp254; void* _temp256; struct Cyc_Dict_T*
_temp243= t; _temp245=* _temp243; _LL257: _temp256=( void*) _temp245.color; goto
_LL255; _LL255: _temp254=( struct Cyc_Dict_T*) _temp245.left; goto _LL253;
_LL253: _temp252=( struct Cyc_Dict_T*) _temp245.right; goto _LL247; _LL247:
_temp246=( struct _tuple0) _temp245.key_val; _LL251: _temp250= _temp246.f1; goto
_LL249; _LL249: _temp248= _temp246.f2; goto _LL244; _LL244: f( _temp250,
_temp248); if( _temp254 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp258=
_temp254; if( _temp258 == 0){ _throw( Null_Exception);} _temp258;}));} if(
_temp252 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp259= _temp252; if( _temp259 ==
0){ _throw( Null_Exception);} _temp259;}));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp260= d->t; if( _temp260 == 0){ _throw( Null_Exception);}
_temp260;}));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp263; struct _tuple0
_temp264; void* _temp266; void* _temp268; struct Cyc_Dict_T* _temp270; struct
Cyc_Dict_T* _temp272; void* _temp274; struct Cyc_Dict_T* _temp261= t; _temp263=*
_temp261; _LL275: _temp274=( void*) _temp263.color; goto _LL273; _LL273:
_temp272=( struct Cyc_Dict_T*) _temp263.left; goto _LL271; _LL271: _temp270=(
struct Cyc_Dict_T*) _temp263.right; goto _LL265; _LL265: _temp264=( struct
_tuple0) _temp263.key_val; _LL269: _temp268= _temp264.f1; goto _LL267; _LL267:
_temp266= _temp264.f2; goto _LL262; _LL262: f( env, _temp268, _temp266); if(
_temp272 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp276= _temp272; if( _temp276 == 0){ _throw( Null_Exception);} _temp276;}));}
if( _temp270 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp277= _temp270; if( _temp277 == 0){ _throw(
Null_Exception);} _temp277;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp278= d->t; if( _temp278 == 0){
_throw( Null_Exception);} _temp278;}));}} struct Cyc_Dict_Iter2Env{ void(* f)(
void*, void*, void*); void* inner_env; struct Cyc_Dict_Dict* d2; } ; typedef
struct Cyc_Dict_Iter2Env Cyc_Dict_iter2_env_t; static void Cyc_Dict_iter2_c_f(
struct Cyc_Dict_Iter2Env* env, void* a, void* b1){ struct Cyc_Dict_Iter2Env
_temp281; struct Cyc_Dict_Dict* _temp282; void* _temp284; void(* _temp286)( void*,
void*, void*); struct Cyc_Dict_Iter2Env* _temp279= env; _temp281=* _temp279;
_LL287: _temp286=( void(*)( void*, void*, void*)) _temp281.f; goto _LL285;
_LL285: _temp284=( void*) _temp281.inner_env; goto _LL283; _LL283: _temp282=(
struct Cyc_Dict_Dict*) _temp281.d2; goto _LL280; _LL280: _temp286( _temp284, b1,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp282, a));}
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* inner_env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct Cyc_Dict_Iter2Env env=({
struct Cyc_Dict_Iter2Env _temp288; _temp288.f= f; _temp288.inner_env=( void*)
inner_env; _temp288.d2= d2; _temp288;});(( void(*)( void(* f)( struct Cyc_Dict_Iter2Env*,
void*, void*), struct Cyc_Dict_Iter2Env* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct Cyc_Dict_Iter2Env* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,&
env, d1);} static struct Cyc_Dict_T* Cyc_Dict_map_tree( void*(* f)( void*),
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp291; struct _tuple0 _temp292; void*
_temp294; void* _temp296; struct Cyc_Dict_T* _temp298; struct Cyc_Dict_T*
_temp300; void* _temp302; struct Cyc_Dict_T* _temp289= t; _temp291=* _temp289;
_LL303: _temp302=( void*) _temp291.color; goto _LL301; _LL301: _temp300=( struct
Cyc_Dict_T*) _temp291.left; goto _LL299; _LL299: _temp298=( struct Cyc_Dict_T*)
_temp291.right; goto _LL293; _LL293: _temp292=( struct _tuple0) _temp291.key_val;
_LL297: _temp296= _temp292.f1; goto _LL295; _LL295: _temp294= _temp292.f2; goto
_LL290; _LL290: { struct Cyc_Dict_T* new_left= _temp300 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp304= _temp300; if( _temp304 == 0){ _throw(
Null_Exception);} _temp304;})); struct Cyc_Dict_T* new_right= _temp298 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp305= _temp298; if( _temp305 ==
0){ _throw( Null_Exception);} _temp305;})); return({ struct Cyc_Dict_T* _temp306=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp306->color=(
void*) _temp302; _temp306->left= new_left; _temp306->right= new_right; _temp306->key_val=({
struct _tuple0 _temp307; _temp307.f1= _temp296; _temp307.f2= f( _temp294);
_temp307;}); _temp306;});}} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*),
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp308=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp308->rel= d->rel; _temp308->t= 0; _temp308;});} return({ struct Cyc_Dict_Dict*
_temp309=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp309->rel= d->rel; _temp309->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*),
struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp310= d->t; if( _temp310 == 0){ _throw( Null_Exception);} _temp310;}));
_temp309;});} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp313; struct
_tuple0 _temp314; void* _temp316; void* _temp318; struct Cyc_Dict_T* _temp320;
struct Cyc_Dict_T* _temp322; void* _temp324; struct Cyc_Dict_T* _temp311= t;
_temp313=* _temp311; _LL325: _temp324=( void*) _temp313.color; goto _LL323;
_LL323: _temp322=( struct Cyc_Dict_T*) _temp313.left; goto _LL321; _LL321:
_temp320=( struct Cyc_Dict_T*) _temp313.right; goto _LL315; _LL315: _temp314=(
struct _tuple0) _temp313.key_val; _LL319: _temp318= _temp314.f1; goto _LL317;
_LL317: _temp316= _temp314.f2; goto _LL312; _LL312: { struct Cyc_Dict_T*
new_left= _temp322 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp326= _temp322; if( _temp326 == 0){ _throw(
Null_Exception);} _temp326;})); struct Cyc_Dict_T* new_right= _temp320 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp327= _temp320; if( _temp327 == 0){ _throw( Null_Exception);} _temp327;}));
return({ struct Cyc_Dict_T* _temp328=( struct Cyc_Dict_T*) GC_malloc( sizeof(
struct Cyc_Dict_T)); _temp328->color=( void*) _temp324; _temp328->left= new_left;
_temp328->right= new_right; _temp328->key_val=({ struct _tuple0 _temp329;
_temp329.f1= _temp318; _temp329.f2= f( env, _temp316); _temp329;}); _temp328;});}}
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp330=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp330->rel= d->rel; _temp330->t= 0; _temp330;});} return({ struct Cyc_Dict_Dict*
_temp331=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp331->rel= d->rel; _temp331->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp332= d->t; if( _temp332 == 0){ _throw(
Null_Exception);} _temp332;})); _temp331;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Dict_Absent_struct* _temp333=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp333[ 0]=({ struct Cyc_Dict_Absent_struct
_temp334; _temp334.tag= Cyc_Dict_Absent_tag; _temp334;}); _temp333;}));} return({
struct _tuple0* _temp335=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp335->f1=(( d->t)->key_val).f1; _temp335->f2=(( d->t)->key_val).f2; _temp335;});}
static int Cyc_Dict_forall_tree_c( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp338; struct _tuple0 _temp339; void*
_temp341; void* _temp343; struct Cyc_Dict_T* _temp345; struct Cyc_Dict_T*
_temp347; void* _temp349; struct Cyc_Dict_T* _temp336= t; _temp338=* _temp336;
_LL350: _temp349=( void*) _temp338.color; goto _LL348; _LL348: _temp347=( struct
Cyc_Dict_T*) _temp338.left; goto _LL346; _LL346: _temp345=( struct Cyc_Dict_T*)
_temp338.right; goto _LL340; _LL340: _temp339=( struct _tuple0) _temp338.key_val;
_LL344: _temp343= _temp339.f1; goto _LL342; _LL342: _temp341= _temp339.f2; goto
_LL337; _LL337: return( f( env, _temp343, _temp341)? _temp347 == 0? 1:(( int(*)(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp351= _temp347; if(
_temp351 == 0){ _throw( Null_Exception);} _temp351;})): 0)? _temp345 == 0? 1:((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp352= _temp345; if(
_temp352 == 0){ _throw( Null_Exception);} _temp352;})): 0;} int Cyc_Dict_forall_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t ==
0){ return 1;} return(( int(*)( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp353= d->t; if( _temp353 == 0){ _throw( Null_Exception);}
_temp353;}));} struct Cyc_Dict_ForallIntEnv{ int(* f)( void*, void*, void*);
struct Cyc_Dict_Dict* d1; } ; typedef struct Cyc_Dict_ForallIntEnv Cyc_Dict_forall_int_env_t;
static int Cyc_Dict_forall_intersect_f( struct Cyc_Dict_ForallIntEnv* env, void*
a, void* b){ struct Cyc_Dict_ForallIntEnv _temp356; struct Cyc_Dict_Dict*
_temp357; int(* _temp359)( void*, void*, void*); struct Cyc_Dict_ForallIntEnv*
_temp354= env; _temp356=* _temp354; _LL360: _temp359=( int(*)( void*, void*,
void*)) _temp356.f; goto _LL358; _LL358: _temp357=( struct Cyc_Dict_Dict*)
_temp356.d1; goto _LL355; _LL355: if((( int(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_member)( _temp357, a)){ return _temp359( a, b,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp357, a));} return 1;} int
Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct Cyc_Dict_ForallIntEnv env=({ struct Cyc_Dict_ForallIntEnv
_temp361; _temp361.f= f; _temp361.d1= d2; _temp361;}); return(( int(*)( int(* f)(
struct Cyc_Dict_ForallIntEnv*, void*, void*), struct Cyc_Dict_ForallIntEnv* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)( struct Cyc_Dict_ForallIntEnv*
env, void* a, void* b)) Cyc_Dict_forall_intersect_f,& env, d1);} static struct
Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*, void*), void* a, void* b,
struct Cyc_Dict_Dict* d1){ if((( int(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_member)( d1, a)){ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( d1, a, f((( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( d1, a), b));} else{ return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d1, a, b);}}
struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;} return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*(* f)( void*(*)( void*, void*), void*, void*, struct Cyc_Dict_Dict*),
void*(* env)( void*, void*), struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold_c)(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*),
void* a, void* b, struct Cyc_Dict_Dict* d1)) Cyc_Dict_union_f, f, d2, d1);}
struct Cyc_Dict_IntArg{ void*(* f)( void*, void*); struct Cyc_Dict_Dict* d1;
struct Cyc_Dict_Dict* d2; } ; typedef struct Cyc_Dict_IntArg Cyc_Dict_intarg_t;
static void Cyc_Dict_intersect_f( struct Cyc_Dict_IntArg* trip, void* a, void* b){
struct Cyc_Dict_IntArg _temp364; struct Cyc_Dict_Dict* _temp365; struct Cyc_Dict_Dict**
_temp367; struct Cyc_Dict_Dict* _temp368; void*(* _temp370)( void*, void*);
struct Cyc_Dict_IntArg* _temp362= trip; _temp364=* _temp362; _LL371: _temp370=(
void*(*)( void*, void*)) _temp364.f; goto _LL369; _LL369: _temp368=( struct Cyc_Dict_Dict*)
_temp364.d1; goto _LL366; _LL366: _temp365=( struct Cyc_Dict_Dict*) _temp364.d2;
_temp367=&(* _temp362).d2; goto _LL363; _LL363: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp368, a)){* _temp367=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp367, a,
_temp370((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp368, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct Cyc_Dict_IntArg env=({ struct Cyc_Dict_IntArg _temp372;
_temp372.f= f; _temp372.d1= d1; _temp372.d2=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( void*, void*))) Cyc_Dict_empty)( d1->rel); _temp372;});(( void(*)( void(*
f)( struct Cyc_Dict_IntArg*, void*, void*), struct Cyc_Dict_IntArg* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Dict_IntArg* trip,
void* a, void* b)) Cyc_Dict_intersect_f,& env, d2); return env.d2;}} struct Cyc_Dict_IntArgC{
void*(* f)( void*, void*, void*); void* env; struct Cyc_Dict_Dict* d1; struct
Cyc_Dict_Dict* d2; } ; typedef struct Cyc_Dict_IntArgC Cyc_Dict_intargc_t;
static void Cyc_Dict_intersectc_f( struct Cyc_Dict_IntArgC* quad, void* a, void*
b){ struct Cyc_Dict_IntArgC _temp375; struct Cyc_Dict_Dict* _temp376; struct Cyc_Dict_Dict**
_temp378; struct Cyc_Dict_Dict* _temp379; void* _temp381; void*(* _temp383)(
void*, void*, void*); struct Cyc_Dict_IntArgC* _temp373= quad; _temp375=*
_temp373; _LL384: _temp383=( void*(*)( void*, void*, void*)) _temp375.f; goto
_LL382; _LL382: _temp381=( void*) _temp375.env; goto _LL380; _LL380: _temp379=(
struct Cyc_Dict_Dict*) _temp375.d1; goto _LL377; _LL377: _temp376=( struct Cyc_Dict_Dict*)
_temp375.d2; _temp378=&(* _temp373).d2; goto _LL374; _LL374: if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp379, a)){* _temp378=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(*
_temp378, a, _temp383( _temp381,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( _temp379, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct Cyc_Dict_IntArgC env2=({
struct Cyc_Dict_IntArgC _temp385; _temp385.f= f; _temp385.env=( void*) env;
_temp385.d1= d1; _temp385.d2=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*,
void*))) Cyc_Dict_empty)( d1->rel); _temp385;});(( void(*)( void(* f)( struct
Cyc_Dict_IntArgC*, void*, void*), struct Cyc_Dict_IntArgC* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Dict_IntArgC* quad, void* a, void* b))
Cyc_Dict_intersectc_f,& env2, d2); return env2.d2;}} static struct Cyc_List_List*
Cyc_Dict_to_list_f( void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp386=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp386->hd=( void*)({ struct _tuple0* _temp387=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp387->f1= k; _temp387->f2=
v; _temp387;}); _temp386->tl= accum; _temp386;});} struct Cyc_List_List* Cyc_Dict_to_list(
struct Cyc_Dict_Dict* d){ return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( void*, void*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List*
accum)) Cyc_Dict_fold)(( struct Cyc_List_List*(*)( void* k, void* v, struct Cyc_List_List*
accum)) Cyc_Dict_to_list_f, d, 0);}