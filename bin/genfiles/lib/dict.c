#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct Cyc_Dict_T*
f2; struct Cyc_Dict_T* f3; struct _tuple0 f4;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag;}; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char* tag;};
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d); extern int Cyc_Dict_member(
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
void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map(
void*(* f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_intersect(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
extern int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_Dict* d); extern struct _tuple0* Cyc_Dict_choose( struct Cyc_Dict_Dict*
d); extern struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d);
char Cyc_Dict_Absent_tag[ 7u]="Absent"; char Cyc_Dict_Present_tag[ 8u]="Present";
typedef void* Cyc_Dict_Color; static void* Cyc_Dict_R=( void*) 0; static void*
Cyc_Dict_B=( void*) 1; struct Cyc_Dict_T{ void* color; struct Cyc_Dict_T* left;
struct Cyc_Dict_T* right; struct _tuple0 key_val;}; typedef struct Cyc_Dict_T*
Cyc_Dict_tree; struct Cyc_Dict_Dict{ int(* rel)( void*, void*); struct Cyc_Dict_T*
t;}; struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)){ return({
struct Cyc_Dict_Dict* _temp0=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct
Cyc_Dict_Dict)); _temp0->rel= comp; _temp0->t= 0; _temp0;});} struct Cyc_Dict_Dict*
Cyc_Dict_singleton( int(* comp)( void*, void*), void* key, void* data){ return({
struct Cyc_Dict_Dict* _temp1=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct
Cyc_Dict_Dict)); _temp1->rel= comp; _temp1->t=({ struct Cyc_Dict_T* _temp2=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp2->color=( void*)
Cyc_Dict_B; _temp2->left= 0; _temp2->right= 0; _temp2->key_val=({ struct _tuple0
_temp3; _temp3.f1= key; _temp3.f2= data; _temp3;}); _temp2;}); _temp1;});} int
Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d){ return d->t == 0;} int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*, void*)= d->rel; struct
Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1); if( i <
0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{ return 1;}}} return 0;}
void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*,
void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1);
if( i < 0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{ return( t->key_val).f2;}}}(
void) _throw(({ struct Cyc_Dict_Absent_struct* _temp4=( struct Cyc_Dict_Absent_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));* _temp4=( struct Cyc_Dict_Absent_struct){.tag=
Cyc_Dict_Absent_tag};( struct _xenum_struct*) _temp4;}));} struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*,
void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1);
if( i < 0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{ return({ struct
Cyc_Core_Opt* _temp5=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5->v=( void*)( t->key_val).f2; _temp5;});}}} return 0;} int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict* d, void* key, void** ans_place){ int(* rel)( void*, void*)=
d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,( t->key_val).f1);
if( i < 0){ t= t->left;} else{ if( i > 0){ t= t->right;} else{* ans_place=( t->key_val).f2;
return 1;}}} return 0;} static struct Cyc_Dict_T* Cyc_Dict_balance( struct
_tuple1 quad){ struct _tuple1 _temp6= quad; struct _tuple0 _temp18; struct Cyc_Dict_T*
_temp20; struct Cyc_Dict_T* _temp22; struct Cyc_Dict_T _temp24; struct _tuple0
_temp25; struct Cyc_Dict_T* _temp27; struct Cyc_Dict_T* _temp29; struct Cyc_Dict_T
_temp31; struct _tuple0 _temp32; struct Cyc_Dict_T* _temp34; struct Cyc_Dict_T*
_temp36; void* _temp38; void* _temp40; void* _temp42; struct _tuple0 _temp44;
struct Cyc_Dict_T* _temp46; struct Cyc_Dict_T* _temp48; struct Cyc_Dict_T
_temp50; struct _tuple0 _temp51; struct Cyc_Dict_T* _temp53; struct Cyc_Dict_T
_temp55; struct _tuple0 _temp56; struct Cyc_Dict_T* _temp58; struct Cyc_Dict_T*
_temp60; void* _temp62; struct Cyc_Dict_T* _temp64; void* _temp66; void* _temp68;
struct _tuple0 _temp70; struct Cyc_Dict_T* _temp72; struct Cyc_Dict_T _temp74;
struct _tuple0 _temp75; struct Cyc_Dict_T* _temp77; struct Cyc_Dict_T* _temp79;
struct Cyc_Dict_T _temp81; struct _tuple0 _temp82; struct Cyc_Dict_T* _temp84;
struct Cyc_Dict_T* _temp86; void* _temp88; void* _temp90; struct Cyc_Dict_T*
_temp92; void* _temp94; struct _tuple0 _temp96; struct Cyc_Dict_T* _temp98;
struct Cyc_Dict_T _temp100; struct _tuple0 _temp101; struct Cyc_Dict_T* _temp103;
struct Cyc_Dict_T _temp105; struct _tuple0 _temp106; struct Cyc_Dict_T* _temp108;
struct Cyc_Dict_T* _temp110; void* _temp112; struct Cyc_Dict_T* _temp114; void*
_temp116; struct Cyc_Dict_T* _temp118; void* _temp120; struct _tuple0 _temp122;
struct Cyc_Dict_T* _temp124; struct Cyc_Dict_T* _temp126; void* _temp128; _LL8:
_LL43: _temp42= _temp6.f1; if( _temp42 == Cyc_Dict_B){ goto _LL23;} else{ goto
_LL10;} _LL23: _temp22= _temp6.f2; if( _temp22 == 0){ goto _LL10;} else{ _temp24=*
_temp22; _LL41: _temp40=( void*) _temp24.color; if( _temp40 == Cyc_Dict_R){ goto
_LL30;} else{ goto _LL10;} _LL30: _temp29=( struct Cyc_Dict_T*) _temp24.left;
if( _temp29 == 0){ goto _LL10;} else{ _temp31=* _temp29; _LL39: _temp38=( void*)
_temp31.color; if( _temp38 == Cyc_Dict_R){ goto _LL37;} else{ goto _LL10;} _LL37:
_temp36=( struct Cyc_Dict_T*) _temp31.left; goto _LL35; _LL35: _temp34=( struct
Cyc_Dict_T*) _temp31.right; goto _LL33; _LL33: _temp32=( struct _tuple0) _temp31.key_val;
goto _LL28;} _LL28: _temp27=( struct Cyc_Dict_T*) _temp24.right; goto _LL26;
_LL26: _temp25=( struct _tuple0) _temp24.key_val; goto _LL21;} _LL21: _temp20=
_temp6.f3; goto _LL19; _LL19: _temp18= _temp6.f4; goto _LL9; _LL10: _LL69:
_temp68= _temp6.f1; if( _temp68 == Cyc_Dict_B){ goto _LL49;} else{ goto _LL12;}
_LL49: _temp48= _temp6.f2; if( _temp48 == 0){ goto _LL12;} else{ _temp50=*
_temp48; _LL67: _temp66=( void*) _temp50.color; if( _temp66 == Cyc_Dict_R){ goto
_LL65;} else{ goto _LL12;} _LL65: _temp64=( struct Cyc_Dict_T*) _temp50.left;
goto _LL54; _LL54: _temp53=( struct Cyc_Dict_T*) _temp50.right; if( _temp53 == 0){
goto _LL12;} else{ _temp55=* _temp53; _LL63: _temp62=( void*) _temp55.color; if(
_temp62 == Cyc_Dict_R){ goto _LL61;} else{ goto _LL12;} _LL61: _temp60=( struct
Cyc_Dict_T*) _temp55.left; goto _LL59; _LL59: _temp58=( struct Cyc_Dict_T*)
_temp55.right; goto _LL57; _LL57: _temp56=( struct _tuple0) _temp55.key_val;
goto _LL52;} _LL52: _temp51=( struct _tuple0) _temp50.key_val; goto _LL47;}
_LL47: _temp46= _temp6.f3; goto _LL45; _LL45: _temp44= _temp6.f4; goto _LL11;
_LL12: _LL95: _temp94= _temp6.f1; if( _temp94 == Cyc_Dict_B){ goto _LL93;} else{
goto _LL14;} _LL93: _temp92= _temp6.f2; goto _LL73; _LL73: _temp72= _temp6.f3;
if( _temp72 == 0){ goto _LL14;} else{ _temp74=* _temp72; _LL91: _temp90=( void*)
_temp74.color; if( _temp90 == Cyc_Dict_R){ goto _LL80;} else{ goto _LL14;} _LL80:
_temp79=( struct Cyc_Dict_T*) _temp74.left; if( _temp79 == 0){ goto _LL14;}
else{ _temp81=* _temp79; _LL89: _temp88=( void*) _temp81.color; if( _temp88 ==
Cyc_Dict_R){ goto _LL87;} else{ goto _LL14;} _LL87: _temp86=( struct Cyc_Dict_T*)
_temp81.left; goto _LL85; _LL85: _temp84=( struct Cyc_Dict_T*) _temp81.right;
goto _LL83; _LL83: _temp82=( struct _tuple0) _temp81.key_val; goto _LL78;} _LL78:
_temp77=( struct Cyc_Dict_T*) _temp74.right; goto _LL76; _LL76: _temp75=( struct
_tuple0) _temp74.key_val; goto _LL71;} _LL71: _temp70= _temp6.f4; goto _LL13;
_LL14: _LL121: _temp120= _temp6.f1; if( _temp120 == Cyc_Dict_B){ goto _LL119;}
else{ goto _LL16;} _LL119: _temp118= _temp6.f2; goto _LL99; _LL99: _temp98=
_temp6.f3; if( _temp98 == 0){ goto _LL16;} else{ _temp100=* _temp98; _LL117:
_temp116=( void*) _temp100.color; if( _temp116 == Cyc_Dict_R){ goto _LL115;}
else{ goto _LL16;} _LL115: _temp114=( struct Cyc_Dict_T*) _temp100.left; goto
_LL104; _LL104: _temp103=( struct Cyc_Dict_T*) _temp100.right; if( _temp103 == 0){
goto _LL16;} else{ _temp105=* _temp103; _LL113: _temp112=( void*) _temp105.color;
if( _temp112 == Cyc_Dict_R){ goto _LL111;} else{ goto _LL16;} _LL111: _temp110=(
struct Cyc_Dict_T*) _temp105.left; goto _LL109; _LL109: _temp108=( struct Cyc_Dict_T*)
_temp105.right; goto _LL107; _LL107: _temp106=( struct _tuple0) _temp105.key_val;
goto _LL102;} _LL102: _temp101=( struct _tuple0) _temp100.key_val; goto _LL97;}
_LL97: _temp96= _temp6.f4; goto _LL15; _LL16: _LL129: _temp128= _temp6.f1; goto
_LL127; _LL127: _temp126= _temp6.f2; goto _LL125; _LL125: _temp124= _temp6.f3;
goto _LL123; _LL123: _temp122= _temp6.f4; goto _LL17; _LL9: return({ struct Cyc_Dict_T*
_temp130=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp130->color=(
void*) Cyc_Dict_R; _temp130->left=({ struct Cyc_Dict_T* _temp132=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp132->color=( void*) Cyc_Dict_B;
_temp132->left= _temp36; _temp132->right= _temp34; _temp132->key_val= _temp32;
_temp132;}); _temp130->right=({ struct Cyc_Dict_T* _temp131=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp131->color=( void*) Cyc_Dict_B;
_temp131->left= _temp27; _temp131->right= _temp20; _temp131->key_val= _temp18;
_temp131;}); _temp130->key_val= _temp25; _temp130;}); _LL11: return({ struct Cyc_Dict_T*
_temp133=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp133->color=(
void*) Cyc_Dict_R; _temp133->left=({ struct Cyc_Dict_T* _temp135=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp135->color=( void*) Cyc_Dict_B;
_temp135->left= _temp64; _temp135->right= _temp60; _temp135->key_val= _temp51;
_temp135;}); _temp133->right=({ struct Cyc_Dict_T* _temp134=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp134->color=( void*) Cyc_Dict_B;
_temp134->left= _temp58; _temp134->right= _temp46; _temp134->key_val= _temp44;
_temp134;}); _temp133->key_val= _temp56; _temp133;}); _LL13: return({ struct Cyc_Dict_T*
_temp136=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp136->color=(
void*) Cyc_Dict_R; _temp136->left=({ struct Cyc_Dict_T* _temp138=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp138->color=( void*) Cyc_Dict_B;
_temp138->left= _temp92; _temp138->right= _temp86; _temp138->key_val= _temp70;
_temp138;}); _temp136->right=({ struct Cyc_Dict_T* _temp137=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp137->color=( void*) Cyc_Dict_B;
_temp137->left= _temp84; _temp137->right= _temp77; _temp137->key_val= _temp75;
_temp137;}); _temp136->key_val= _temp82; _temp136;}); _LL15: return({ struct Cyc_Dict_T*
_temp139=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp139->color=(
void*) Cyc_Dict_R; _temp139->left=({ struct Cyc_Dict_T* _temp141=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp141->color=( void*) Cyc_Dict_B;
_temp141->left= _temp118; _temp141->right= _temp114; _temp141->key_val= _temp96;
_temp141;}); _temp139->right=({ struct Cyc_Dict_T* _temp140=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp140->color=( void*) Cyc_Dict_B;
_temp140->left= _temp110; _temp140->right= _temp108; _temp140->key_val= _temp106;
_temp140;}); _temp139->key_val= _temp101; _temp139;}); _LL17: return({ struct
Cyc_Dict_T* _temp142=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp142->color=( void*) _temp128; _temp142->left= _temp126; _temp142->right=
_temp124; _temp142->key_val= _temp122; _temp142;}); _LL7:;} static struct Cyc_Dict_T*
Cyc_Dict_ins( int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T* _temp143= t; struct Cyc_Dict_T _temp149; struct _tuple0
_temp150; struct Cyc_Dict_T* _temp152; struct Cyc_Dict_T* _temp154; void*
_temp156; _LL145: if( _temp143 == 0){ goto _LL146;} else{ goto _LL147;} _LL147:
if( _temp143 == 0){ goto _LL144;} else{ _temp149=* _temp143; _LL157: _temp156=(
void*) _temp149.color; goto _LL155; _LL155: _temp154=( struct Cyc_Dict_T*)
_temp149.left; goto _LL153; _LL153: _temp152=( struct Cyc_Dict_T*) _temp149.right;
goto _LL151; _LL151: _temp150=( struct _tuple0) _temp149.key_val; goto _LL148;}
_LL146: return({ struct Cyc_Dict_T* _temp158=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp158->color=( void*) Cyc_Dict_R; _temp158->left=
0; _temp158->right= 0; _temp158->key_val= key_val; _temp158;}); _LL148: { int i=
rel( key_val.f1, _temp150.f1); if( i < 0){ return(( struct Cyc_Dict_T*(*)(
struct _tuple1 quad)) Cyc_Dict_balance)(({ struct _tuple1 _temp159; _temp159.f1=
_temp156; _temp159.f2=(( struct Cyc_Dict_T*(*)( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val,
_temp154); _temp159.f3= _temp152; _temp159.f4= _temp150; _temp159;}));} else{
if( i > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad)) Cyc_Dict_balance)(({
struct _tuple1 _temp160; _temp160.f1= _temp156; _temp160.f2= _temp154; _temp160.f3=((
struct Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct
Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val, _temp152); _temp160.f4= _temp150;
_temp160;}));} else{ return({ struct Cyc_Dict_T* _temp161=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp161->color=( void*) _temp156;
_temp161->left= _temp154; _temp161->right= _temp152; _temp161->key_val= key_val;
_temp161;});}}} _LL144:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data){ struct Cyc_Dict_T* ans=(( struct Cyc_Dict_T*(*)( int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
d->rel,({ struct _tuple0 _temp162; _temp162.f1= key; _temp162.f2= data; _temp162;}),
d->t);( void*)( ans->color=( void*) Cyc_Dict_B); return({ struct Cyc_Dict_Dict*
_temp163=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp163->rel= d->rel; _temp163->t= ans; _temp163;});} struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* key, void* data){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, key)){( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp164=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));*
_temp164=( struct Cyc_Dict_Absent_struct){.tag= Cyc_Dict_Absent_tag};( struct
_xenum_struct*) _temp164;}));} return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( d, key, data);} struct Cyc_Dict_Dict*
Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List* kds){ for( 0;
kds != 0; kds= kds->tl){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( d,({ struct _tuple0* _temp165=( struct
_tuple0*) kds->hd; unsigned int _temp166= 0; if( _temp166 >= 1u){ _throw(
Null_Exception);} _temp165[ _temp166];}).f1,({ struct _tuple0* _temp167=( struct
_tuple0*) kds->hd; unsigned int _temp168= 0; if( _temp168 >= 1u){ _throw(
Null_Exception);} _temp167[ _temp168];}).f2);} return d;} static void* Cyc_Dict_fold_tree(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T
_temp171; struct _tuple0 _temp172; void* _temp174; void* _temp176; struct Cyc_Dict_T*
_temp178; struct Cyc_Dict_T* _temp180; void* _temp182; struct Cyc_Dict_T*
_temp169= t; _temp171=* _temp169; _LL183: _temp182=( void*) _temp171.color; goto
_LL181; _LL181: _temp180=( struct Cyc_Dict_T*) _temp171.left; goto _LL179;
_LL179: _temp178=( struct Cyc_Dict_T*) _temp171.right; goto _LL173; _LL173:
_temp172=( struct _tuple0) _temp171.key_val; _LL177: _temp176= _temp172.f1; goto
_LL175; _LL175: _temp174= _temp172.f2; goto _LL170; _LL170: accum= f( _temp176,
_temp174, accum); if( _temp180 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp184= _temp180; if( _temp184 == 0){ _throw(
Null_Exception);} _temp184;}), accum);} if( _temp178 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp185= _temp178; if( _temp185 ==
0){ _throw( Null_Exception);} _temp185;}), accum);} return accum;} void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t
== 0){ return accum;} return(( void*(*)( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp186= d->t; if( _temp186 == 0){ _throw( Null_Exception);}
_temp186;}), accum);} static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*,
void*, void*), void* env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T
_temp189; struct _tuple0 _temp190; void* _temp192; void* _temp194; struct Cyc_Dict_T*
_temp196; struct Cyc_Dict_T* _temp198; void* _temp200; struct Cyc_Dict_T*
_temp187= t; _temp189=* _temp187; _LL201: _temp200=( void*) _temp189.color; goto
_LL199; _LL199: _temp198=( struct Cyc_Dict_T*) _temp189.left; goto _LL197;
_LL197: _temp196=( struct Cyc_Dict_T*) _temp189.right; goto _LL191; _LL191:
_temp190=( struct _tuple0) _temp189.key_val; _LL195: _temp194= _temp190.f1; goto
_LL193; _LL193: _temp192= _temp190.f2; goto _LL188; _LL188: accum= f( env,
_temp194, _temp192, accum); if( _temp198 != 0){ accum=(( void*(*)( void*(* f)(
void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp202= _temp198; if(
_temp202 == 0){ _throw( Null_Exception);} _temp202;}), accum);} if( _temp196 !=
0){ accum=(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp203= _temp196; if( _temp203 == 0){ _throw(
Null_Exception);} _temp203;}), accum);} return accum;} void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp204= d->t; if( _temp204
== 0){ _throw( Null_Exception);} _temp204;}), accum);} static void Cyc_Dict_app_tree(
void*(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp207;
struct _tuple0 _temp208; void* _temp210; void* _temp212; struct Cyc_Dict_T*
_temp214; struct Cyc_Dict_T* _temp216; void* _temp218; struct Cyc_Dict_T*
_temp205= t; _temp207=* _temp205; _LL219: _temp218=( void*) _temp207.color; goto
_LL217; _LL217: _temp216=( struct Cyc_Dict_T*) _temp207.left; goto _LL215;
_LL215: _temp214=( struct Cyc_Dict_T*) _temp207.right; goto _LL209; _LL209:
_temp208=( struct _tuple0) _temp207.key_val; _LL213: _temp212= _temp208.f1; goto
_LL211; _LL211: _temp210= _temp208.f2; goto _LL206; _LL206: f( _temp212,
_temp210); if( _temp216 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp220=
_temp216; if( _temp220 == 0){ _throw( Null_Exception);} _temp220;}));} if(
_temp214 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp221= _temp214; if( _temp221 ==
0){ _throw( Null_Exception);} _temp221;}));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp222= d->t; if( _temp222 == 0){ _throw( Null_Exception);}
_temp222;}));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp225; struct _tuple0
_temp226; void* _temp228; void* _temp230; struct Cyc_Dict_T* _temp232; struct
Cyc_Dict_T* _temp234; void* _temp236; struct Cyc_Dict_T* _temp223= t; _temp225=*
_temp223; _LL237: _temp236=( void*) _temp225.color; goto _LL235; _LL235:
_temp234=( struct Cyc_Dict_T*) _temp225.left; goto _LL233; _LL233: _temp232=(
struct Cyc_Dict_T*) _temp225.right; goto _LL227; _LL227: _temp226=( struct
_tuple0) _temp225.key_val; _LL231: _temp230= _temp226.f1; goto _LL229; _LL229:
_temp228= _temp226.f2; goto _LL224; _LL224: f( env, _temp230, _temp228); if(
_temp234 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp238= _temp234; if( _temp238 == 0){ _throw( Null_Exception);} _temp238;}));}
if( _temp232 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp239= _temp232; if( _temp239 == 0){ _throw(
Null_Exception);} _temp239;}));}} void Cyc_Dict_app_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp240= d->t; if( _temp240
== 0){ _throw( Null_Exception);} _temp240;}));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp243;
struct _tuple0 _temp244; void* _temp246; void* _temp248; struct Cyc_Dict_T*
_temp250; struct Cyc_Dict_T* _temp252; void* _temp254; struct Cyc_Dict_T*
_temp241= t; _temp243=* _temp241; _LL255: _temp254=( void*) _temp243.color; goto
_LL253; _LL253: _temp252=( struct Cyc_Dict_T*) _temp243.left; goto _LL251;
_LL251: _temp250=( struct Cyc_Dict_T*) _temp243.right; goto _LL245; _LL245:
_temp244=( struct _tuple0) _temp243.key_val; _LL249: _temp248= _temp244.f1; goto
_LL247; _LL247: _temp246= _temp244.f2; goto _LL242; _LL242: f( _temp248,
_temp246); if( _temp252 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp256=
_temp252; if( _temp256 == 0){ _throw( Null_Exception);} _temp256;}));} if(
_temp250 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp257= _temp250; if( _temp257 ==
0){ _throw( Null_Exception);} _temp257;}));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp258= d->t; if( _temp258 == 0){ _throw( Null_Exception);}
_temp258;}));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp261; struct _tuple0
_temp262; void* _temp264; void* _temp266; struct Cyc_Dict_T* _temp268; struct
Cyc_Dict_T* _temp270; void* _temp272; struct Cyc_Dict_T* _temp259= t; _temp261=*
_temp259; _LL273: _temp272=( void*) _temp261.color; goto _LL271; _LL271:
_temp270=( struct Cyc_Dict_T*) _temp261.left; goto _LL269; _LL269: _temp268=(
struct Cyc_Dict_T*) _temp261.right; goto _LL263; _LL263: _temp262=( struct
_tuple0) _temp261.key_val; _LL267: _temp266= _temp262.f1; goto _LL265; _LL265:
_temp264= _temp262.f2; goto _LL260; _LL260: f( env, _temp266, _temp264); if(
_temp270 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp274= _temp270; if( _temp274 == 0){ _throw( Null_Exception);} _temp274;}));}
if( _temp268 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp275= _temp268; if( _temp275 == 0){ _throw(
Null_Exception);} _temp275;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp276= d->t; if( _temp276 == 0){
_throw( Null_Exception);} _temp276;}));}} static struct Cyc_Dict_T* Cyc_Dict_map_tree(
void*(* f)( void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp279; struct
_tuple0 _temp280; void* _temp282; void* _temp284; struct Cyc_Dict_T* _temp286;
struct Cyc_Dict_T* _temp288; void* _temp290; struct Cyc_Dict_T* _temp277= t;
_temp279=* _temp277; _LL291: _temp290=( void*) _temp279.color; goto _LL289;
_LL289: _temp288=( struct Cyc_Dict_T*) _temp279.left; goto _LL287; _LL287:
_temp286=( struct Cyc_Dict_T*) _temp279.right; goto _LL281; _LL281: _temp280=(
struct _tuple0) _temp279.key_val; _LL285: _temp284= _temp280.f1; goto _LL283;
_LL283: _temp282= _temp280.f2; goto _LL278; _LL278: { struct Cyc_Dict_T*
new_left= _temp288 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct
Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp292= _temp288; if( _temp292 == 0){ _throw( Null_Exception);} _temp292;}));
struct Cyc_Dict_T* new_right= _temp286 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(*
f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp293= _temp286; if( _temp293 == 0){ _throw(
Null_Exception);} _temp293;})); return({ struct Cyc_Dict_T* _temp294=( struct
Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp294->color=( void*)
_temp290; _temp294->left= new_left; _temp294->right= new_right; _temp294->key_val=({
struct _tuple0 _temp295; _temp295.f1= _temp284; _temp295.f2= f( _temp282);
_temp295;}); _temp294;});}} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*),
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp296=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp296->rel= d->rel; _temp296->t= 0; _temp296;});} return({ struct Cyc_Dict_Dict*
_temp297=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp297->rel= d->rel; _temp297->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*),
struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp298= d->t; if( _temp298 == 0){ _throw( Null_Exception);} _temp298;}));
_temp297;});} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp301; struct
_tuple0 _temp302; void* _temp304; void* _temp306; struct Cyc_Dict_T* _temp308;
struct Cyc_Dict_T* _temp310; void* _temp312; struct Cyc_Dict_T* _temp299= t;
_temp301=* _temp299; _LL313: _temp312=( void*) _temp301.color; goto _LL311;
_LL311: _temp310=( struct Cyc_Dict_T*) _temp301.left; goto _LL309; _LL309:
_temp308=( struct Cyc_Dict_T*) _temp301.right; goto _LL303; _LL303: _temp302=(
struct _tuple0) _temp301.key_val; _LL307: _temp306= _temp302.f1; goto _LL305;
_LL305: _temp304= _temp302.f2; goto _LL300; _LL300: { struct Cyc_Dict_T*
new_left= _temp310 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp314= _temp310; if( _temp314 == 0){ _throw(
Null_Exception);} _temp314;})); struct Cyc_Dict_T* new_right= _temp308 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp315= _temp308; if( _temp315 == 0){ _throw( Null_Exception);} _temp315;}));
return({ struct Cyc_Dict_T* _temp316=( struct Cyc_Dict_T*) GC_malloc( sizeof(
struct Cyc_Dict_T)); _temp316->color=( void*) _temp312; _temp316->left= new_left;
_temp316->right= new_right; _temp316->key_val=({ struct _tuple0 _temp317;
_temp317.f1= _temp306; _temp317.f2= f( env, _temp304); _temp317;}); _temp316;});}}
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp318=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp318->rel= d->rel; _temp318->t= 0; _temp318;});} return({ struct Cyc_Dict_Dict*
_temp319=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp319->rel= d->rel; _temp319->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp320= d->t; if( _temp320 == 0){ _throw(
Null_Exception);} _temp320;})); _temp319;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp321=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));*
_temp321=( struct Cyc_Dict_Absent_struct){.tag= Cyc_Dict_Absent_tag};( struct
_xenum_struct*) _temp321;}));} return({ struct _tuple0* _temp322=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp322->f1=(( d->t)->key_val).f1;
_temp322->f2=(( d->t)->key_val).f2; _temp322;});} static int Cyc_Dict_forall_tree_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp325; struct _tuple0 _temp326; void* _temp328; void* _temp330; struct Cyc_Dict_T*
_temp332; struct Cyc_Dict_T* _temp334; void* _temp336; struct Cyc_Dict_T*
_temp323= t; _temp325=* _temp323; _LL337: _temp336=( void*) _temp325.color; goto
_LL335; _LL335: _temp334=( struct Cyc_Dict_T*) _temp325.left; goto _LL333;
_LL333: _temp332=( struct Cyc_Dict_T*) _temp325.right; goto _LL327; _LL327:
_temp326=( struct _tuple0) _temp325.key_val; _LL331: _temp330= _temp326.f1; goto
_LL329; _LL329: _temp328= _temp326.f2; goto _LL324; _LL324: return( f( env,
_temp330, _temp328)? _temp334 == 0? 1:(( int(*)( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp338= _temp334; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338;})): 0)? _temp332 == 0? 1:(( int(*)( int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp339= _temp332; if( _temp339 == 0){
_throw( Null_Exception);} _temp339;})): 0;} int Cyc_Dict_forall_c( int(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){
return 1;} return(( int(*)( int(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct
Cyc_Dict_T* _temp340= d->t; if( _temp340 == 0){ _throw( Null_Exception);}
_temp340;}));} static struct Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*,
void*), void* a, void* b, struct Cyc_Dict_Dict* d1){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d1, a)){ return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d1, a, f(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d1, a), b));}
else{ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( d1, a, b);}} struct Cyc_Dict_Dict* Cyc_Dict_union_two(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){
if( d1 == d2){ return d1;} return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(*
f)( void*(*)( void*, void*), void*, void*, struct Cyc_Dict_Dict*), void*(* env)(
void*, void*), struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)((
struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* a, void* b, struct
Cyc_Dict_Dict* d1)) Cyc_Dict_union_f, f, d2, d1);} struct Cyc_Dict_IntArg{ void*(*
f)( void*, void*); struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2;}; typedef
struct Cyc_Dict_IntArg Cyc_Dict_intarg_t; static void Cyc_Dict_intersect_f(
struct Cyc_Dict_IntArg* trip, void* a, void* b){ struct Cyc_Dict_IntArg _temp343;
struct Cyc_Dict_Dict* _temp344; struct Cyc_Dict_Dict** _temp346; struct Cyc_Dict_Dict*
_temp347; void*(* _temp349)( void*, void*); struct Cyc_Dict_IntArg* _temp341=
trip; _temp343=* _temp341; _LL350: _temp349=( void*(*)( void*, void*)) _temp343.f;
goto _LL348; _LL348: _temp347=( struct Cyc_Dict_Dict*) _temp343.d1; goto _LL345;
_LL345: _temp344=( struct Cyc_Dict_Dict*) _temp343.d2; _temp346=&(* _temp341).d2;
goto _LL342; _LL342: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp347, a)){* _temp346=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)(* _temp346, a, _temp349((( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp347, a), b));}}
struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct Cyc_Dict_IntArg
env=({ struct Cyc_Dict_IntArg _temp351; _temp351.f= f; _temp351.d1= d1; _temp351.d2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)( d1->rel);
_temp351;});(( void(*)( void(* f)( struct Cyc_Dict_IntArg*, void*, void*),
struct Cyc_Dict_IntArg* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct Cyc_Dict_IntArg* trip, void* a, void* b)) Cyc_Dict_intersect_f,& env, d2);
return env.d2;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k, void*
v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp352=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp352->hd=(
void*)({ struct _tuple0* _temp353=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp353->f1= k; _temp353->f2= v; _temp353;}); _temp352->tl= accum;
_temp352;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( void*, void*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)(( struct Cyc_List_List*(*)( void* k, void* v, struct Cyc_List_List*
accum)) Cyc_Dict_to_list_f, d, 0);}