 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_rempty( struct _RegionHandle*, int(* comp)( void*,
void*)); extern int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data); extern
struct Cyc_Dict_Dict* Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
kds); extern struct Cyc_Dict_Dict* Cyc_Dict_singleton( int(* comp)( void*, void*),
void* key, void* data); extern struct Cyc_Dict_Dict* Cyc_Dict_rsingleton( struct
_RegionHandle*, int(* comp)( void*, void*), void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict* d, void* key, void** ans_place); extern void** Cyc_Dict_rlookup_opt(
struct _RegionHandle*, struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern
void* Cyc_Dict_fold_c( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_Dict* dict, void* accum); extern void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d); extern void Cyc_Dict_app_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter(
void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern
void Cyc_Dict_iter2( void(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2); extern void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct
Cyc_Dict_Dict* Cyc_Dict_rcopy( struct _RegionHandle*, struct Cyc_Dict_Dict*);
extern struct Cyc_Dict_Dict* Cyc_Dict_copy( struct Cyc_Dict_Dict*); extern
struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_union_two(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c( int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); struct _tuple0{ void* f1; void* f2; } ; extern struct _tuple0* Cyc_Dict_rchoose(
struct _RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_List_List*
Cyc_Dict_to_list( struct Cyc_Dict_Dict* d); extern struct Cyc_List_List* Cyc_Dict_rto_list(
struct _RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle*, int(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter( int(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_rfilter( struct _RegionHandle*, int(* f)( void*,
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_rdifference( struct _RegionHandle*, struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_delete( struct Cyc_Dict_Dict*,
void*); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete( struct _RegionHandle*,
struct Cyc_Dict_Dict*, void*); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict*, void*); unsigned char Cyc_Dict_Absent[ 11u]="\000\000\000\000Absent";
unsigned char Cyc_Dict_Present[ 12u]="\000\000\000\000Present"; static const int
Cyc_Dict_R= 0; static const int Cyc_Dict_B= 1; struct Cyc_Dict_T{ void* color;
struct Cyc_Dict_T* left; struct Cyc_Dict_T* right; struct _tuple0 key_val; } ;
struct Cyc_Dict_Dict{ int(* rel)( void*, void*); struct _RegionHandle* r; struct
Cyc_Dict_T* t; } ; struct Cyc_Dict_Dict* Cyc_Dict_rempty( struct _RegionHandle*
r, int(* comp)( void*, void*)){ return({ struct Cyc_Dict_Dict* _temp0=( struct
Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp0->rel=
comp; _temp0->r= r; _temp0->t= 0; _temp0;});} struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, int(* comp)( void*, void*))) Cyc_Dict_rempty)( Cyc_Core_heap_region,
comp);} struct Cyc_Dict_Dict* Cyc_Dict_rsingleton( struct _RegionHandle* r, int(*
comp)( void*, void*), void* key, void* data){ return({ struct Cyc_Dict_Dict*
_temp1=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict));
_temp1->rel= comp; _temp1->r= r; _temp1->t=({ struct Cyc_Dict_T* _temp2=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp2->color=(
void*)(( void*) Cyc_Dict_B); _temp2->left= 0; _temp2->right= 0; _temp2->key_val=({
struct _tuple0 _temp3; _temp3.f1= key; _temp3.f2= data; _temp3;}); _temp2;});
_temp1;});} struct Cyc_Dict_Dict* Cyc_Dict_singleton( int(* comp)( void*, void*),
void* key, void* data){ return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle*
r, int(* comp)( void*, void*), void* key, void* data)) Cyc_Dict_rsingleton)( Cyc_Core_heap_region,
comp, key, data);} int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d){ return d->t
== 0;} int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key){ int(* _temp4)(
void*, void*)= d->rel; goto _LL5; _LL5: { struct Cyc_Dict_T* _temp6= d->t; goto
_LL7; _LL7: while( _temp6 != 0) { int _temp8= _temp4( key,((( struct Cyc_Dict_T*)
_check_null( _temp6))->key_val).f1); goto _LL9; _LL9: if( _temp8 < 0){ _temp6=((
struct Cyc_Dict_T*) _check_null( _temp6))->left;} else{ if( _temp8 > 0){ _temp6=((
struct Cyc_Dict_T*) _check_null( _temp6))->right;} else{ return 1;}}} return 0;}}
void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key){ int(* _temp10)( void*,
void*)= d->rel; goto _LL11; _LL11: { struct Cyc_Dict_T* _temp12= d->t; goto
_LL13; _LL13: while( _temp12 != 0) { int _temp14= _temp10( key,((( struct Cyc_Dict_T*)
_check_null( _temp12))->key_val).f1); goto _LL15; _LL15: if( _temp14 < 0){
_temp12=(( struct Cyc_Dict_T*) _check_null( _temp12))->left;} else{ if( _temp14
> 0){ _temp12=(( struct Cyc_Dict_T*) _check_null( _temp12))->right;} else{
return((( struct Cyc_Dict_T*) _check_null( _temp12))->key_val).f2;}}}( int)
_throw(( void*) Cyc_Dict_Absent);}} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key){ int(* _temp16)( void*, void*)= d->rel; goto
_LL17; _LL17: { struct Cyc_Dict_T* _temp18= d->t; goto _LL19; _LL19: while(
_temp18 != 0) { int _temp20= _temp16( key,((( struct Cyc_Dict_T*) _check_null(
_temp18))->key_val).f1); goto _LL21; _LL21: if( _temp20 < 0){ _temp18=(( struct
Cyc_Dict_T*) _check_null( _temp18))->left;} else{ if( _temp20 > 0){ _temp18=((
struct Cyc_Dict_T*) _check_null( _temp18))->right;} else{ return({ struct Cyc_Core_Opt*
_temp22=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp22->v=( void*)((( struct Cyc_Dict_T*) _check_null( _temp18))->key_val).f2;
_temp22;});}}} return 0;}} void** Cyc_Dict_rlookup_opt( struct _RegionHandle* r,
struct Cyc_Dict_Dict* d, void* key){ int(* _temp23)( void*, void*)= d->rel; goto
_LL24; _LL24: { struct Cyc_Dict_T* _temp25= d->t; goto _LL26; _LL26: while(
_temp25 != 0) { int _temp27= _temp23( key,((( struct Cyc_Dict_T*) _check_null(
_temp25))->key_val).f1); goto _LL28; _LL28: if( _temp27 < 0){ _temp25=(( struct
Cyc_Dict_T*) _check_null( _temp25))->left;} else{ if( _temp27 > 0){ _temp25=((
struct Cyc_Dict_T*) _check_null( _temp25))->right;} else{ return({ void**
_temp29=( void**) _region_malloc( r, sizeof( void*)); _temp29[ 0]=((( struct Cyc_Dict_T*)
_check_null( _temp25))->key_val).f2; _temp29;});}}} return 0;}} int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict* d, void* key, void** ans_place){ int(* _temp30)( void*,
void*)= d->rel; goto _LL31; _LL31: { struct Cyc_Dict_T* _temp32= d->t; goto
_LL33; _LL33: while( _temp32 != 0) { int _temp34= _temp30( key,((( struct Cyc_Dict_T*)
_check_null( _temp32))->key_val).f1); goto _LL35; _LL35: if( _temp34 < 0){
_temp32=(( struct Cyc_Dict_T*) _check_null( _temp32))->left;} else{ if( _temp34
> 0){ _temp32=(( struct Cyc_Dict_T*) _check_null( _temp32))->right;} else{*
ans_place=((( struct Cyc_Dict_T*) _check_null( _temp32))->key_val).f2; return 1;}}}
return 0;}} struct _tuple1{ void* f1; struct Cyc_Dict_T* f2; struct Cyc_Dict_T*
f3; struct _tuple0 f4; } ; static struct Cyc_Dict_T* Cyc_Dict_balance( struct
_RegionHandle* r, struct _tuple1 quad_t){ struct _tuple1 _temp36= quad_t; struct
_tuple0 _temp48; struct Cyc_Dict_T* _temp50; struct Cyc_Dict_T* _temp52; struct
Cyc_Dict_T _temp54; struct _tuple0 _temp55; struct Cyc_Dict_T* _temp57; struct
Cyc_Dict_T* _temp59; struct Cyc_Dict_T _temp61; struct _tuple0 _temp62; struct
Cyc_Dict_T* _temp64; struct Cyc_Dict_T* _temp66; void* _temp68; void* _temp70;
void* _temp72; struct _tuple0 _temp74; struct Cyc_Dict_T* _temp76; struct Cyc_Dict_T*
_temp78; struct Cyc_Dict_T _temp80; struct _tuple0 _temp81; struct Cyc_Dict_T*
_temp83; struct Cyc_Dict_T _temp85; struct _tuple0 _temp86; struct Cyc_Dict_T*
_temp88; struct Cyc_Dict_T* _temp90; void* _temp92; struct Cyc_Dict_T* _temp94;
void* _temp96; void* _temp98; struct _tuple0 _temp100; struct Cyc_Dict_T*
_temp102; struct Cyc_Dict_T _temp104; struct _tuple0 _temp105; struct Cyc_Dict_T*
_temp107; struct Cyc_Dict_T* _temp109; struct Cyc_Dict_T _temp111; struct
_tuple0 _temp112; struct Cyc_Dict_T* _temp114; struct Cyc_Dict_T* _temp116; void*
_temp118; void* _temp120; struct Cyc_Dict_T* _temp122; void* _temp124; struct
_tuple0 _temp126; struct Cyc_Dict_T* _temp128; struct Cyc_Dict_T _temp130;
struct _tuple0 _temp131; struct Cyc_Dict_T* _temp133; struct Cyc_Dict_T _temp135;
struct _tuple0 _temp136; struct Cyc_Dict_T* _temp138; struct Cyc_Dict_T*
_temp140; void* _temp142; struct Cyc_Dict_T* _temp144; void* _temp146; struct
Cyc_Dict_T* _temp148; void* _temp150; struct _tuple0 _temp152; struct Cyc_Dict_T*
_temp154; struct Cyc_Dict_T* _temp156; void* _temp158; _LL38: _LL73: _temp72=
_temp36.f1; if( _temp72 ==( void*) Cyc_Dict_B){ goto _LL53;} else{ goto _LL40;}
_LL53: _temp52= _temp36.f2; if( _temp52 == 0){ goto _LL40;} else{ _temp54=*
_temp52; _LL71: _temp70=( void*) _temp54.color; if( _temp70 ==( void*) Cyc_Dict_R){
goto _LL60;} else{ goto _LL40;} _LL60: _temp59= _temp54.left; if( _temp59 == 0){
goto _LL40;} else{ _temp61=* _temp59; _LL69: _temp68=( void*) _temp61.color; if(
_temp68 ==( void*) Cyc_Dict_R){ goto _LL67;} else{ goto _LL40;} _LL67: _temp66=
_temp61.left; goto _LL65; _LL65: _temp64= _temp61.right; goto _LL63; _LL63:
_temp62= _temp61.key_val; goto _LL58;} _LL58: _temp57= _temp54.right; goto _LL56;
_LL56: _temp55= _temp54.key_val; goto _LL51;} _LL51: _temp50= _temp36.f3; goto
_LL49; _LL49: _temp48= _temp36.f4; goto _LL39; _LL40: _LL99: _temp98= _temp36.f1;
if( _temp98 ==( void*) Cyc_Dict_B){ goto _LL79;} else{ goto _LL42;} _LL79:
_temp78= _temp36.f2; if( _temp78 == 0){ goto _LL42;} else{ _temp80=* _temp78;
_LL97: _temp96=( void*) _temp80.color; if( _temp96 ==( void*) Cyc_Dict_R){ goto
_LL95;} else{ goto _LL42;} _LL95: _temp94= _temp80.left; goto _LL84; _LL84:
_temp83= _temp80.right; if( _temp83 == 0){ goto _LL42;} else{ _temp85=* _temp83;
_LL93: _temp92=( void*) _temp85.color; if( _temp92 ==( void*) Cyc_Dict_R){ goto
_LL91;} else{ goto _LL42;} _LL91: _temp90= _temp85.left; goto _LL89; _LL89:
_temp88= _temp85.right; goto _LL87; _LL87: _temp86= _temp85.key_val; goto _LL82;}
_LL82: _temp81= _temp80.key_val; goto _LL77;} _LL77: _temp76= _temp36.f3; goto
_LL75; _LL75: _temp74= _temp36.f4; goto _LL41; _LL42: _LL125: _temp124= _temp36.f1;
if( _temp124 ==( void*) Cyc_Dict_B){ goto _LL123;} else{ goto _LL44;} _LL123:
_temp122= _temp36.f2; goto _LL103; _LL103: _temp102= _temp36.f3; if( _temp102 ==
0){ goto _LL44;} else{ _temp104=* _temp102; _LL121: _temp120=( void*) _temp104.color;
if( _temp120 ==( void*) Cyc_Dict_R){ goto _LL110;} else{ goto _LL44;} _LL110:
_temp109= _temp104.left; if( _temp109 == 0){ goto _LL44;} else{ _temp111=*
_temp109; _LL119: _temp118=( void*) _temp111.color; if( _temp118 ==( void*) Cyc_Dict_R){
goto _LL117;} else{ goto _LL44;} _LL117: _temp116= _temp111.left; goto _LL115;
_LL115: _temp114= _temp111.right; goto _LL113; _LL113: _temp112= _temp111.key_val;
goto _LL108;} _LL108: _temp107= _temp104.right; goto _LL106; _LL106: _temp105=
_temp104.key_val; goto _LL101;} _LL101: _temp100= _temp36.f4; goto _LL43; _LL44:
_LL151: _temp150= _temp36.f1; if( _temp150 ==( void*) Cyc_Dict_B){ goto _LL149;}
else{ goto _LL46;} _LL149: _temp148= _temp36.f2; goto _LL129; _LL129: _temp128=
_temp36.f3; if( _temp128 == 0){ goto _LL46;} else{ _temp130=* _temp128; _LL147:
_temp146=( void*) _temp130.color; if( _temp146 ==( void*) Cyc_Dict_R){ goto
_LL145;} else{ goto _LL46;} _LL145: _temp144= _temp130.left; goto _LL134; _LL134:
_temp133= _temp130.right; if( _temp133 == 0){ goto _LL46;} else{ _temp135=*
_temp133; _LL143: _temp142=( void*) _temp135.color; if( _temp142 ==( void*) Cyc_Dict_R){
goto _LL141;} else{ goto _LL46;} _LL141: _temp140= _temp135.left; goto _LL139;
_LL139: _temp138= _temp135.right; goto _LL137; _LL137: _temp136= _temp135.key_val;
goto _LL132;} _LL132: _temp131= _temp130.key_val; goto _LL127;} _LL127: _temp126=
_temp36.f4; goto _LL45; _LL46: _LL159: _temp158= _temp36.f1; goto _LL157; _LL157:
_temp156= _temp36.f2; goto _LL155; _LL155: _temp154= _temp36.f3; goto _LL153;
_LL153: _temp152= _temp36.f4; goto _LL47; _LL39: return({ struct Cyc_Dict_T*
_temp160=( struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T));
_temp160->color=( void*)(( void*) Cyc_Dict_R); _temp160->left=({ struct Cyc_Dict_T*
_temp162=( struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T));
_temp162->color=( void*)(( void*) Cyc_Dict_B); _temp162->left= _temp66; _temp162->right=
_temp64; _temp162->key_val= _temp62; _temp162;}); _temp160->right=({ struct Cyc_Dict_T*
_temp161=( struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T));
_temp161->color=( void*)(( void*) Cyc_Dict_B); _temp161->left= _temp57; _temp161->right=
_temp50; _temp161->key_val= _temp48; _temp161;}); _temp160->key_val= _temp55;
_temp160;}); _LL41: return({ struct Cyc_Dict_T* _temp163=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp163->color=( void*)(( void*)
Cyc_Dict_R); _temp163->left=({ struct Cyc_Dict_T* _temp165=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp165->color=( void*)(( void*)
Cyc_Dict_B); _temp165->left= _temp94; _temp165->right= _temp90; _temp165->key_val=
_temp81; _temp165;}); _temp163->right=({ struct Cyc_Dict_T* _temp164=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp164->color=(
void*)(( void*) Cyc_Dict_B); _temp164->left= _temp88; _temp164->right= _temp76;
_temp164->key_val= _temp74; _temp164;}); _temp163->key_val= _temp86; _temp163;});
_LL43: return({ struct Cyc_Dict_T* _temp166=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp166->color=( void*)(( void*) Cyc_Dict_R);
_temp166->left=({ struct Cyc_Dict_T* _temp168=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp168->color=( void*)(( void*)
Cyc_Dict_B); _temp168->left= _temp122; _temp168->right= _temp116; _temp168->key_val=
_temp100; _temp168;}); _temp166->right=({ struct Cyc_Dict_T* _temp167=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp167->color=(
void*)(( void*) Cyc_Dict_B); _temp167->left= _temp114; _temp167->right= _temp107;
_temp167->key_val= _temp105; _temp167;}); _temp166->key_val= _temp112; _temp166;});
_LL45: return({ struct Cyc_Dict_T* _temp169=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp169->color=( void*)(( void*) Cyc_Dict_R);
_temp169->left=({ struct Cyc_Dict_T* _temp171=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp171->color=( void*)(( void*)
Cyc_Dict_B); _temp171->left= _temp148; _temp171->right= _temp144; _temp171->key_val=
_temp126; _temp171;}); _temp169->right=({ struct Cyc_Dict_T* _temp170=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp170->color=(
void*)(( void*) Cyc_Dict_B); _temp170->left= _temp140; _temp170->right= _temp138;
_temp170->key_val= _temp136; _temp170;}); _temp169->key_val= _temp131; _temp169;});
_LL47: return({ struct Cyc_Dict_T* _temp172=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp172->color=( void*) _temp158; _temp172->left=
_temp156; _temp172->right= _temp154; _temp172->key_val= _temp152; _temp172;});
_LL37:;} static struct Cyc_Dict_T* Cyc_Dict_ins( struct _RegionHandle* r, int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t){ struct Cyc_Dict_T*
_temp173= t; struct Cyc_Dict_T _temp179; struct _tuple0 _temp180; struct Cyc_Dict_T*
_temp182; struct Cyc_Dict_T* _temp184; void* _temp186; _LL175: if( _temp173 == 0){
goto _LL176;} else{ goto _LL177;} _LL177: if( _temp173 == 0){ goto _LL174;}
else{ _temp179=* _temp173; _LL187: _temp186=( void*) _temp179.color; goto _LL185;
_LL185: _temp184= _temp179.left; goto _LL183; _LL183: _temp182= _temp179.right;
goto _LL181; _LL181: _temp180= _temp179.key_val; goto _LL178;} _LL176: return({
struct Cyc_Dict_T* _temp188=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp188->color=( void*)(( void*) Cyc_Dict_R); _temp188->left=
0; _temp188->right= 0; _temp188->key_val= key_val; _temp188;}); _LL178: { int
_temp189= rel( key_val.f1, _temp180.f1); goto _LL190; _LL190: if( _temp189 < 0){
return(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r, struct _tuple1 quad_t))
Cyc_Dict_balance)( r,({ struct _tuple1 _temp191; _temp191.f1= _temp186; _temp191.f2=((
struct Cyc_Dict_T*(*)( struct _RegionHandle* r, int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( r, rel, key_val,
_temp184); _temp191.f3= _temp182; _temp191.f4= _temp180; _temp191;}));} else{
if( _temp189 > 0){ return(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
struct _tuple1 quad_t)) Cyc_Dict_balance)( r,({ struct _tuple1 _temp192;
_temp192.f1= _temp186; _temp192.f2= _temp184; _temp192.f3=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r, int(* rel)( void*, void*), struct _tuple0 key_val,
struct Cyc_Dict_T* t)) Cyc_Dict_ins)( r, rel, key_val, _temp182); _temp192.f4=
_temp180; _temp192;}));} else{ return({ struct Cyc_Dict_T* _temp193=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp193->color=( void*)
_temp186; _temp193->left= _temp184; _temp193->right= _temp182; _temp193->key_val=
key_val; _temp193;});}}} _LL174:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct
Cyc_Dict_Dict* d, void* key, void* data){ struct Cyc_Dict_T* _temp195=(( struct
Cyc_Dict_T*(*)( struct _RegionHandle* r, int(* rel)( void*, void*), struct
_tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( d->r, d->rel,({ struct
_tuple0 _temp194; _temp194.f1= key; _temp194.f2= data; _temp194;}), d->t); goto
_LL196; _LL196:( void*)((( struct Cyc_Dict_T*) _check_null( _temp195))->color=(
void*)(( void*) Cyc_Dict_B)); return({ struct Cyc_Dict_Dict* _temp197=( struct
Cyc_Dict_Dict*) _region_malloc( d->r, sizeof( struct Cyc_Dict_Dict)); _temp197->rel=
d->rel; _temp197->r= d->r; _temp197->t= _temp195; _temp197;});} struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data){ if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d, key)){( int) _throw((
void*) Cyc_Dict_Absent);} return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( d, key, data);} struct Cyc_Dict_Dict*
Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List* kds){ for( 0;
kds != 0; kds=(( struct Cyc_List_List*) _check_null( kds))->tl){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d,((( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( kds))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1,((( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( kds))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2);} return d;}
static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum){ struct Cyc_Dict_T _temp200; struct _tuple0 _temp201; void*
_temp203; void* _temp205; struct Cyc_Dict_T* _temp207; struct Cyc_Dict_T*
_temp209; void* _temp211; struct Cyc_Dict_T* _temp198= t; _temp200=* _temp198;
_LL212: _temp211=( void*) _temp200.color; goto _LL210; _LL210: _temp209=
_temp200.left; goto _LL208; _LL208: _temp207= _temp200.right; goto _LL202;
_LL202: _temp201= _temp200.key_val; _LL206: _temp205= _temp201.f1; goto _LL204;
_LL204: _temp203= _temp201.f2; goto _LL199; _LL199: if( _temp209 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum))
Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*) _check_null( _temp209), accum);}
accum= f( _temp205, _temp203, accum); if( _temp207 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp207), accum);} return accum;} void*
Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void*
accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,(
struct Cyc_Dict_T*) _check_null( d->t), accum);} static void* Cyc_Dict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void*
accum){ struct Cyc_Dict_T _temp215; struct _tuple0 _temp216; void* _temp218;
void* _temp220; struct Cyc_Dict_T* _temp222; struct Cyc_Dict_T* _temp224; void*
_temp226; struct Cyc_Dict_T* _temp213= t; _temp215=* _temp213; _LL227: _temp226=(
void*) _temp215.color; goto _LL225; _LL225: _temp224= _temp215.left; goto _LL223;
_LL223: _temp222= _temp215.right; goto _LL217; _LL217: _temp216= _temp215.key_val;
_LL221: _temp220= _temp216.f1; goto _LL219; _LL219: _temp218= _temp216.f2; goto
_LL214; _LL214: if( _temp224 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp224), accum);} accum= f( env,
_temp220, _temp218, accum); if( _temp222 != 0){ accum=(( void*(*)( void*(* f)(
void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp222), accum);} return accum;}
void* Cyc_Dict_fold_c( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){ return accum;} return((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null(
d->t), accum);} static void Cyc_Dict_app_tree( void*(* f)( void*, void*), struct
Cyc_Dict_T* t){ struct Cyc_Dict_T _temp230; struct _tuple0 _temp231; void*
_temp233; void* _temp235; struct Cyc_Dict_T* _temp237; struct Cyc_Dict_T*
_temp239; void* _temp241; struct Cyc_Dict_T* _temp228= t; _temp230=* _temp228;
_LL242: _temp241=( void*) _temp230.color; goto _LL240; _LL240: _temp239=
_temp230.left; goto _LL238; _LL238: _temp237= _temp230.right; goto _LL232;
_LL232: _temp231= _temp230.key_val; _LL236: _temp235= _temp231.f1; goto _LL234;
_LL234: _temp233= _temp231.f2; goto _LL229; _LL229: if( _temp239 != 0){(( void(*)(
void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct
Cyc_Dict_T*) _check_null( _temp239));} f( _temp235, _temp233); if( _temp237 != 0){((
void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,(
struct Cyc_Dict_T*) _check_null( _temp237));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)
_check_null( d->t));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp245; struct
_tuple0 _temp246; void* _temp248; void* _temp250; struct Cyc_Dict_T* _temp252;
struct Cyc_Dict_T* _temp254; void* _temp256; struct Cyc_Dict_T* _temp243= t;
_temp245=* _temp243; _LL257: _temp256=( void*) _temp245.color; goto _LL255;
_LL255: _temp254= _temp245.left; goto _LL253; _LL253: _temp252= _temp245.right;
goto _LL247; _LL247: _temp246= _temp245.key_val; _LL251: _temp250= _temp246.f1;
goto _LL249; _LL249: _temp248= _temp246.f2; goto _LL244; _LL244: if( _temp254 !=
0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null( _temp254));} f(
env, _temp250, _temp248); if( _temp252 != 0){(( void(*)( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct
Cyc_Dict_T*) _check_null( _temp252));}} void Cyc_Dict_app_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( d->t));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp260;
struct _tuple0 _temp261; void* _temp263; void* _temp265; struct Cyc_Dict_T*
_temp267; struct Cyc_Dict_T* _temp269; void* _temp271; struct Cyc_Dict_T*
_temp258= t; _temp260=* _temp258; _LL272: _temp271=( void*) _temp260.color; goto
_LL270; _LL270: _temp269= _temp260.left; goto _LL268; _LL268: _temp267= _temp260.right;
goto _LL262; _LL262: _temp261= _temp260.key_val; _LL266: _temp265= _temp261.f1;
goto _LL264; _LL264: _temp263= _temp261.f2; goto _LL259; _LL259: if( _temp269 !=
0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp269));} f( _temp265, _temp263); if(
_temp267 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp267));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)
_check_null( d->t));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp275; struct
_tuple0 _temp276; void* _temp278; void* _temp280; struct Cyc_Dict_T* _temp282;
struct Cyc_Dict_T* _temp284; void* _temp286; struct Cyc_Dict_T* _temp273= t;
_temp275=* _temp273; _LL287: _temp286=( void*) _temp275.color; goto _LL285;
_LL285: _temp284= _temp275.left; goto _LL283; _LL283: _temp282= _temp275.right;
goto _LL277; _LL277: _temp276= _temp275.key_val; _LL281: _temp280= _temp276.f1;
goto _LL279; _LL279: _temp278= _temp276.f2; goto _LL274; _LL274: if( _temp284 !=
0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null( _temp284));} f(
env, _temp280, _temp278); if( _temp282 != 0){(( void(*)( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct
Cyc_Dict_T*) _check_null( _temp282));}} void Cyc_Dict_iter_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( d->t));}} struct _tuple2{ void(* f1)(
void*, void*); struct Cyc_Dict_Dict* f2; } ; static void Cyc_Dict_iter2_f(
struct _tuple2* env, void* a, void* b1){ struct _tuple2 _temp290; struct Cyc_Dict_Dict*
_temp291; void(* _temp293)( void*, void*); struct _tuple2* _temp288= env;
_temp290=* _temp288; _LL294: _temp293= _temp290.f1; goto _LL292; _LL292:
_temp291= _temp290.f2; goto _LL289; _LL289: _temp293( b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp291, a));} void Cyc_Dict_iter2( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple2 _temp296=({ struct _tuple2 _temp295; _temp295.f1= f; _temp295.f2= d2;
_temp295;}); goto _LL297; _LL297:(( void(*)( void(* f)( struct _tuple2*, void*,
void*), struct _tuple2* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple2* env, void* a, void* b1)) Cyc_Dict_iter2_f,& _temp296, d1);}
struct _tuple3{ void(* f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void*
f3; } ; static void Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){
struct _tuple3 _temp300; void* _temp301; struct Cyc_Dict_Dict* _temp303; void(*
_temp305)( void*, void*, void*); struct _tuple3* _temp298= env; _temp300=*
_temp298; _LL306: _temp305= _temp300.f1; goto _LL304; _LL304: _temp303= _temp300.f2;
goto _LL302; _LL302: _temp301= _temp300.f3; goto _LL299; _LL299: _temp305(
_temp301, b1,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp303, a));} void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void*
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3
_temp308=({ struct _tuple3 _temp307; _temp307.f1= f; _temp307.f2= d2; _temp307.f3=
inner_env; _temp307;}); goto _LL309; _LL309:(( void(*)( void(* f)( struct
_tuple3*, void*, void*), struct _tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple3* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,& _temp308,
d1);} static struct Cyc_Dict_T* Cyc_Dict_copy_tree( struct _RegionHandle* r2,
struct Cyc_Dict_T* t){ if( t == 0){ return 0;} else{ struct _tuple0 _temp312;
struct Cyc_Dict_T* _temp314; struct Cyc_Dict_T* _temp316; void* _temp318; struct
Cyc_Dict_T _temp310=*(( struct Cyc_Dict_T*) _check_null( t)); _LL319: _temp318=(
void*) _temp310.color; goto _LL317; _LL317: _temp316= _temp310.left; goto _LL315;
_LL315: _temp314= _temp310.right; goto _LL313; _LL313: _temp312= _temp310.key_val;
goto _LL311; _LL311: { struct Cyc_Dict_T* _temp320=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r2, struct Cyc_Dict_T* t)) Cyc_Dict_copy_tree)( r2,
_temp316); goto _LL321; _LL321: { struct Cyc_Dict_T* _temp322=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r2, struct Cyc_Dict_T* t)) Cyc_Dict_copy_tree)( r2,
_temp314); goto _LL323; _LL323: return({ struct Cyc_Dict_T* _temp324=( struct
Cyc_Dict_T*) _region_malloc( r2, sizeof( struct Cyc_Dict_T)); _temp324->color=(
void*) _temp318; _temp324->left= _temp320; _temp324->right= _temp322; _temp324->key_val=
_temp312; _temp324;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_rcopy( struct
_RegionHandle* r2, struct Cyc_Dict_Dict* d){ return({ struct Cyc_Dict_Dict*
_temp325=( struct Cyc_Dict_Dict*) _region_malloc( r2, sizeof( struct Cyc_Dict_Dict));
_temp325->rel= d->rel; _temp325->r= r2; _temp325->t=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r2, struct Cyc_Dict_T* t)) Cyc_Dict_copy_tree)( r2, d->t);
_temp325;});} struct Cyc_Dict_Dict* Cyc_Dict_copy( struct Cyc_Dict_Dict* d){
return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rcopy)( Cyc_Core_heap_region, d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree(
struct _RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp328; struct _tuple0 _temp329; void* _temp331; void* _temp333; struct Cyc_Dict_T*
_temp335; struct Cyc_Dict_T* _temp337; void* _temp339; struct Cyc_Dict_T*
_temp326= t; _temp328=* _temp326; _LL340: _temp339=( void*) _temp328.color; goto
_LL338; _LL338: _temp337= _temp328.left; goto _LL336; _LL336: _temp335= _temp328.right;
goto _LL330; _LL330: _temp329= _temp328.key_val; _LL334: _temp333= _temp329.f1;
goto _LL332; _LL332: _temp331= _temp329.f2; goto _LL327; _LL327: { struct Cyc_Dict_T*
_temp341= _temp337 == 0? 0:(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*)
_check_null( _temp337)); goto _LL342; _LL342: { void* _temp343= f( _temp331);
goto _LL344; _LL344: { struct Cyc_Dict_T* _temp345= _temp335 == 0? 0:(( struct
Cyc_Dict_T*(*)( struct _RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*) _check_null( _temp335)); goto
_LL346; _LL346: return({ struct Cyc_Dict_T* _temp347=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp347->color=( void*)
_temp339; _temp347->left= _temp341; _temp347->right= _temp345; _temp347->key_val=({
struct _tuple0 _temp348; _temp348.f1= _temp333; _temp348.f2= _temp343; _temp348;});
_temp347;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_rmap( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct
Cyc_Dict_Dict* _temp349=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof(
struct Cyc_Dict_Dict)); _temp349->rel= d->rel; _temp349->r= r; _temp349->t= 0;
_temp349;});} return({ struct Cyc_Dict_Dict* _temp350=( struct Cyc_Dict_Dict*)
_region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp350->rel= d->rel;
_temp350->r= r; _temp350->t=(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*)
_check_null( d->t)); _temp350;});} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)(
void*), struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_rmap)(
Cyc_Core_heap_region, f, d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c(
struct _RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T _temp353; struct _tuple0 _temp354; void* _temp356; void*
_temp358; struct Cyc_Dict_T* _temp360; struct Cyc_Dict_T* _temp362; void*
_temp364; struct Cyc_Dict_T* _temp351= t; _temp353=* _temp351; _LL365: _temp364=(
void*) _temp353.color; goto _LL363; _LL363: _temp362= _temp353.left; goto _LL361;
_LL361: _temp360= _temp353.right; goto _LL355; _LL355: _temp354= _temp353.key_val;
_LL359: _temp358= _temp354.f1; goto _LL357; _LL357: _temp356= _temp354.f2; goto
_LL352; _LL352: { struct Cyc_Dict_T* _temp366= _temp362 == 0? 0:(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*) _check_null( _temp362));
goto _LL367; _LL367: { void* _temp368= f( env, _temp356); goto _LL369; _LL369: {
struct Cyc_Dict_T* _temp370= _temp360 == 0? 0:(( struct Cyc_Dict_T*(*)( struct
_RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*) _check_null( _temp360));
goto _LL371; _LL371: return({ struct Cyc_Dict_T* _temp372=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp372->color=( void*)
_temp364; _temp372->left= _temp366; _temp372->right= _temp370; _temp372->key_val=({
struct _tuple0 _temp373; _temp373.f1= _temp358; _temp373.f2= _temp368; _temp373;});
_temp372;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct _RegionHandle* r,
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){
return({ struct Cyc_Dict_Dict* _temp374=( struct Cyc_Dict_Dict*) _region_malloc(
r, sizeof( struct Cyc_Dict_Dict)); _temp374->rel= d->rel; _temp374->r= r;
_temp374->t= 0; _temp374;});} return({ struct Cyc_Dict_Dict* _temp375=( struct
Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp375->rel=
d->rel; _temp375->r= r; _temp375->t=(( struct Cyc_Dict_T*(*)( struct
_RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*) _check_null( d->t));
_temp375;});} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_rmap_c)( Cyc_Core_heap_region, f, env, d);} struct _tuple0* Cyc_Dict_rchoose(
struct _RegionHandle* r, struct Cyc_Dict_Dict* d){ if( d->t == 0){( int) _throw((
void*) Cyc_Dict_Absent);} return({ struct _tuple0* _temp376=( struct _tuple0*)
_region_malloc( r, sizeof( struct _tuple0)); _temp376->f1=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f1; _temp376->f2=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f2; _temp376;});} static int Cyc_Dict_forall_tree_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp379; struct _tuple0 _temp380; void* _temp382; void* _temp384; struct Cyc_Dict_T*
_temp386; struct Cyc_Dict_T* _temp388; void* _temp390; struct Cyc_Dict_T*
_temp377= t; _temp379=* _temp377; _LL391: _temp390=( void*) _temp379.color; goto
_LL389; _LL389: _temp388= _temp379.left; goto _LL387; _LL387: _temp386= _temp379.right;
goto _LL381; _LL381: _temp380= _temp379.key_val; _LL385: _temp384= _temp380.f1;
goto _LL383; _LL383: _temp382= _temp380.f2; goto _LL378; _LL378: return((
_temp388 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)
_check_null( _temp388)))? f( env, _temp384, _temp382): 0)? _temp386 == 0? 1:((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp386)): 0;} int Cyc_Dict_forall_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t ==
0){ return 1;} return(( int(*)( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)
_check_null( d->t));} struct _tuple4{ int(* f1)( void*, void*, void*); struct
Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f( struct _tuple4*
env, void* a, void* b){ struct _tuple4 _temp394; struct Cyc_Dict_Dict* _temp395;
int(* _temp397)( void*, void*, void*); struct _tuple4* _temp392= env; _temp394=*
_temp392; _LL398: _temp397= _temp394.f1; goto _LL396; _LL396: _temp395= _temp394.f2;
goto _LL393; _LL393: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp395, a)){ return _temp397( a, b,(( void*(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_lookup)( _temp395, a));} return 1;} int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ struct _tuple4 _temp400=({ struct _tuple4 _temp399; _temp399.f1= f;
_temp399.f2= d2; _temp399;}); goto _LL401; _LL401: return(( int(*)( int(* f)(
struct _tuple4*, void*, void*), struct _tuple4* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_forall_c)(( int(*)( struct _tuple4* env, void* a, void* b)) Cyc_Dict_forall_intersect_f,&
_temp400, d1);} static struct Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*,
void*), void* a, void* b, struct Cyc_Dict_Dict* d1){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d1, a)){ return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d1, a, f(((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( d1, a), b));}
else{ return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( d1, a, b);}} struct Cyc_Dict_Dict* Cyc_Dict_union_two(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( void*(*)( void*,
void*), void*, void*, struct Cyc_Dict_Dict*), void*(* env)( void*, void*),
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* a, void* b, struct Cyc_Dict_Dict*
d1)) Cyc_Dict_union_f, f, d1, d2);} struct _tuple5{ void*(* f1)( void*, void*);
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; } ; static void Cyc_Dict_intersect_f(
struct _tuple5* trip, void* a, void* b){ struct _tuple5 _temp404; struct Cyc_Dict_Dict*
_temp405; struct Cyc_Dict_Dict** _temp407; struct Cyc_Dict_Dict* _temp408; void*(*
_temp410)( void*, void*); struct _tuple5* _temp402= trip; _temp404=* _temp402;
_LL411: _temp410= _temp404.f1; goto _LL409; _LL409: _temp408= _temp404.f2; goto
_LL406; _LL406: _temp405= _temp404.f3; _temp407=&(* _temp402).f3; goto _LL403;
_LL403: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp408, a)){* _temp407=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)(* _temp407, a, _temp410((( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp408, a), b));}}
struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct _tuple5
_temp413=({ struct _tuple5 _temp412; _temp412.f1= f; _temp412.f2= d1; _temp412.f3=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r, int(* comp)( void*, void*)))
Cyc_Dict_rempty)( d1->r, d1->rel); _temp412;}); goto _LL414; _LL414:(( void(*)(
void(* f)( struct _tuple5*, void*, void*), struct _tuple5* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple5* trip, void* a, void* b)) Cyc_Dict_intersect_f,&
_temp413, d2); return _temp413.f3;}} struct _tuple6{ void*(* f1)( void*, void*,
void*); void* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static
void Cyc_Dict_intersect_c_f( struct _tuple6* quad_t, void* a, void* b){ struct
_tuple6 _temp417; struct Cyc_Dict_Dict* _temp418; struct Cyc_Dict_Dict**
_temp420; struct Cyc_Dict_Dict* _temp421; void* _temp423; void*(* _temp425)(
void*, void*, void*); struct _tuple6* _temp415= quad_t; _temp417=* _temp415;
_LL426: _temp425= _temp417.f1; goto _LL424; _LL424: _temp423= _temp417.f2; goto
_LL422; _LL422: _temp421= _temp417.f3; goto _LL419; _LL419: _temp418= _temp417.f4;
_temp420=&(* _temp415).f4; goto _LL416; _LL416: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp421, a)){* _temp420=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp420, a,
_temp425( _temp423,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp421, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ if( d1 == d2){ return d1;}{ struct _tuple6 _temp428=({ struct _tuple6
_temp427; _temp427.f1= f; _temp427.f2= env; _temp427.f3= d1; _temp427.f4=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r, int(* comp)( void*, void*)))
Cyc_Dict_rempty)( d1->r, d1->rel); _temp427;}); goto _LL429; _LL429:(( void(*)(
void(* f)( struct _tuple6*, void*, void*), struct _tuple6* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple6* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,&
_temp428, d2); return _temp428.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f(
struct _RegionHandle* r, void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp430=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp430->hd=( void*)({ struct _tuple0* _temp431=(
struct _tuple0*) _region_malloc( r, sizeof( struct _tuple0)); _temp431->f1= k;
_temp431->f2= v; _temp431;}); _temp430->tl= accum; _temp430;});} struct Cyc_List_List*
Cyc_Dict_rto_list( struct _RegionHandle* r, struct Cyc_Dict_Dict* d){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _RegionHandle*,
void*, void*, struct Cyc_List_List*), struct _RegionHandle* env, struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold_c)(( struct Cyc_List_List*(*)(
struct _RegionHandle* r, void* k, void* v, struct Cyc_List_List* accum)) Cyc_Dict_to_list_f,
r, d, 0);} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rto_list)( Cyc_Core_heap_region, d);} struct _tuple7{ int(* f1)(
void*, void*); struct _RegionHandle* f2; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_f(
struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict* acc){ struct
_tuple7 _temp434; struct _RegionHandle* _temp435; int(* _temp437)( void*, void*);
struct _tuple7* _temp432= env; _temp434=* _temp432; _LL438: _temp437= _temp434.f1;
goto _LL436; _LL436: _temp435= _temp434.f2; goto _LL433; _LL433: return _temp437(
x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter(
struct _RegionHandle* r2, int(* f)( void*, void*), struct Cyc_Dict_Dict* d){
struct _tuple7 _temp440=({ struct _tuple7 _temp439; _temp439.f1= f; _temp439.f2=
r2; _temp439;}); goto _LL441; _LL441: return(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict*(* f)( struct _tuple7*, void*, void*, struct Cyc_Dict_Dict*),
struct _tuple7* env, struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)((
struct Cyc_Dict_Dict*(*)( struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_f,& _temp440, d,(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, int(* comp)( void*, void*))) Cyc_Dict_rempty)( r2, d->rel));}
struct Cyc_Dict_Dict* Cyc_Dict_filter( int(* f)( void*, void*), struct Cyc_Dict_Dict*
d){ return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, int(* f)( void*,
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter)( Cyc_Core_heap_region, f, d);}
struct _tuple8{ int(* f1)( void*, void*, void*); void* f2; struct _RegionHandle*
f3; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_c_f( struct _tuple8* env,
void* x, void* y, struct Cyc_Dict_Dict* acc){ struct _tuple8 _temp444; struct
_RegionHandle* _temp445; void* _temp447; int(* _temp449)( void*, void*, void*);
struct _tuple8* _temp442= env; _temp444=* _temp442; _LL450: _temp449= _temp444.f1;
goto _LL448; _LL448: _temp447= _temp444.f2; goto _LL446; _LL446: _temp445=
_temp444.f3; goto _LL443; _LL443: return _temp449( _temp447, x, y)?(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( acc, x, y):
acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle* r2, int(*
f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct _tuple8
_temp452=({ struct _tuple8 _temp451; _temp451.f1= f; _temp451.f2= f_env;
_temp451.f3= r2; _temp451;}); goto _LL453; _LL453: return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*(* f)( struct _tuple8*, void*, void*, struct Cyc_Dict_Dict*),
struct _tuple8* env, struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)((
struct Cyc_Dict_Dict*(*)( struct _tuple8* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_c_f,& _temp452, d,(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, int(* comp)( void*, void*))) Cyc_Dict_rempty)( r2, d->rel));}
struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void*
f_env, struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r2, int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rfilter_c)( Cyc_Core_heap_region, f, f_env, d);} static int Cyc_Dict_difference_f(
struct Cyc_Dict_Dict* d, void* x, void* y){ return !(( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, x);} struct Cyc_Dict_Dict* Cyc_Dict_rdifference(
struct _RegionHandle* r2, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){
return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, int(* f)( struct
Cyc_Dict_Dict*, void*, void*), struct Cyc_Dict_Dict* f_env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rfilter_c)( r2,( int(*)( struct Cyc_Dict_Dict* d, void* x, void* y))
Cyc_Dict_difference_f, d2, d1);} struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle* r2, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_rdifference)( Cyc_Core_heap_region, d1, d2);} struct _tuple9{ int(* f1)(
void*, void*); void* f2; } ; static int Cyc_Dict_delete_f( struct _tuple9* env,
void* x, void* y){ struct _tuple9 _temp456; void* _temp457; int(* _temp459)(
void*, void*); struct _tuple9* _temp454= env; _temp456=* _temp454; _LL460:
_temp459= _temp456.f1; goto _LL458; _LL458: _temp457= _temp456.f2; goto _LL455;
_LL455: return _temp459( _temp457, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_rdelete(
struct _RegionHandle* r2, struct Cyc_Dict_Dict* d, void* x){ if( !(( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d, x)){ return(( struct
Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, struct Cyc_Dict_Dict* d)) Cyc_Dict_rcopy)(
r2, d);}{ struct _tuple9 _temp462=({ struct _tuple9 _temp461; _temp461.f1= d->rel;
_temp461.f2= x; _temp461;}); goto _LL463; _LL463: return(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle* r2, int(* f)( struct _tuple9*, void*, void*), struct
_tuple9* f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter_c)( r2,( int(*)(
struct _tuple9* env, void* x, void* y)) Cyc_Dict_delete_f,& _temp462, d);}}
struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict* d, void* x){
if( !(( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d, x)){
return d;}{ struct _tuple9 _temp465=({ struct _tuple9 _temp464; _temp464.f1= d->rel;
_temp464.f2= x; _temp464;}); goto _LL466; _LL466: return(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle* r2, int(* f)( struct _tuple9*, void*, void*), struct
_tuple9* f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter_c)( d->r,( int(*)(
struct _tuple9* env, void* x, void* y)) Cyc_Dict_delete_f,& _temp465, d);}}
struct Cyc_Dict_Dict* Cyc_Dict_delete( struct Cyc_Dict_Dict* d, void* x){ return((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, struct Cyc_Dict_Dict* d,
void* x)) Cyc_Dict_rdelete)( Cyc_Core_heap_region, d, x);}