 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_rempty( struct _RegionHandle*, int(* cmp)( void*,
void*)); extern int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* k, void* v); extern struct Cyc_Dict_Dict* Cyc_Dict_inserts(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* l); extern struct Cyc_Dict_Dict*
Cyc_Dict_singleton( int(* cmp)( void*, void*), void* k, void* v); extern struct
Cyc_Dict_Dict* Cyc_Dict_rsingleton( struct _RegionHandle*, int(* cmp)( void*,
void*), void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* k); extern void** Cyc_Dict_rlookup_opt( struct _RegionHandle*, struct
Cyc_Dict_Dict* d, void* k); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* k, void** ans); extern void* Cyc_Dict_fold( void*(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d, void* accum); extern void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum); extern void Cyc_Dict_app( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern void Cyc_Dict_app_c( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter( void(* f)( void*, void*),
struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter2( void(*
f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern void* Cyc_Dict_fold2_c( void*(* f)( void*,
void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2, void* accum); extern struct Cyc_Dict_Dict* Cyc_Dict_rcopy( struct
_RegionHandle*, struct Cyc_Dict_Dict*); extern struct Cyc_Dict_Dict* Cyc_Dict_copy(
struct Cyc_Dict_Dict*); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)(
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c( int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); struct _tuple0{ void* f1; void* f2; } ; extern struct _tuple0* Cyc_Dict_rchoose(
struct _RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_List_List*
Cyc_Dict_to_list( struct Cyc_Dict_Dict* d); extern struct Cyc_List_List* Cyc_Dict_rto_list(
struct _RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter( int(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_rfilter( struct _RegionHandle*, int(* f)( void*,
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle*, int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_difference( struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
struct Cyc_Dict_Dict* Cyc_Dict_rdifference( struct _RegionHandle*, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict*, void*); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete(
struct _RegionHandle*, struct Cyc_Dict_Dict*, void*); extern struct Cyc_Dict_Dict*
Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict*, void*); unsigned char Cyc_Dict_Absent[
11u]="\000\000\000\000Absent"; unsigned char Cyc_Dict_Present[ 12u]="\000\000\000\000Present";
static const int Cyc_Dict_R= 0; static const int Cyc_Dict_B= 1; struct Cyc_Dict_T{
void* color; struct Cyc_Dict_T* left; struct Cyc_Dict_T* right; struct _tuple0
key_val; } ; struct Cyc_Dict_Dict{ int(* rel)( void*, void*); struct
_RegionHandle* r; struct Cyc_Dict_T* t; } ; struct Cyc_Dict_Dict* Cyc_Dict_rempty(
struct _RegionHandle* r, int(* comp)( void*, void*)){ return({ struct Cyc_Dict_Dict*
_temp0=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict));
_temp0->rel= comp; _temp0->r= r; _temp0->t= 0; _temp0;});} struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)){ return Cyc_Dict_rempty( Cyc_Core_heap_region,
comp);} struct Cyc_Dict_Dict* Cyc_Dict_rsingleton( struct _RegionHandle* r, int(*
comp)( void*, void*), void* key, void* data){ return({ struct Cyc_Dict_Dict*
_temp1=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict));
_temp1->rel= comp; _temp1->r= r; _temp1->t=({ struct Cyc_Dict_T* _temp2=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp2->color=(
void*)(( void*) Cyc_Dict_B); _temp2->left= 0; _temp2->right= 0; _temp2->key_val=({
struct _tuple0 _temp3; _temp3.f1= key; _temp3.f2= data; _temp3;}); _temp2;});
_temp1;});} struct Cyc_Dict_Dict* Cyc_Dict_singleton( int(* comp)( void*, void*),
void* key, void* data){ return Cyc_Dict_rsingleton( Cyc_Core_heap_region, comp,
key, data);} int Cyc_Dict_is_empty( struct Cyc_Dict_Dict* d){ return d->t ==  0;}
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key){ int(* _temp4)( void*,
void*)= d->rel; struct Cyc_Dict_T* _temp5= d->t; while( _temp5 !=  0) { int
_temp6= _temp4( key,((( struct Cyc_Dict_T*) _check_null( _temp5))->key_val).f1);
if( _temp6 <  0){ _temp5=(( struct Cyc_Dict_T*) _check_null( _temp5))->left;}
else{ if( _temp6 >  0){ _temp5=(( struct Cyc_Dict_T*) _check_null( _temp5))->right;}
else{ return 1;}}} return 0;} void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key){ int(* _temp7)( void*, void*)= d->rel; struct Cyc_Dict_T* _temp8= d->t;
while( _temp8 !=  0) { int _temp9= _temp7( key,((( struct Cyc_Dict_T*)
_check_null( _temp8))->key_val).f1); if( _temp9 <  0){ _temp8=(( struct Cyc_Dict_T*)
_check_null( _temp8))->left;} else{ if( _temp9 >  0){ _temp8=(( struct Cyc_Dict_T*)
_check_null( _temp8))->right;} else{ return((( struct Cyc_Dict_T*) _check_null(
_temp8))->key_val).f2;}}}( int) _throw(( void*) Cyc_Dict_Absent);} struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key){ int(* _temp10)( void*,
void*)= d->rel; struct Cyc_Dict_T* _temp11= d->t; while( _temp11 !=  0) { int
_temp12= _temp10( key,((( struct Cyc_Dict_T*) _check_null( _temp11))->key_val).f1);
if( _temp12 <  0){ _temp11=(( struct Cyc_Dict_T*) _check_null( _temp11))->left;}
else{ if( _temp12 >  0){ _temp11=(( struct Cyc_Dict_T*) _check_null( _temp11))->right;}
else{ return({ struct Cyc_Core_Opt* _temp13=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp13->v=( void*)((( struct Cyc_Dict_T*)
_check_null( _temp11))->key_val).f2; _temp13;});}}} return 0;} void** Cyc_Dict_rlookup_opt(
struct _RegionHandle* r, struct Cyc_Dict_Dict* d, void* key){ int(* _temp14)(
void*, void*)= d->rel; struct Cyc_Dict_T* _temp15= d->t; while( _temp15 !=  0) {
int _temp16= _temp14( key,((( struct Cyc_Dict_T*) _check_null( _temp15))->key_val).f1);
if( _temp16 <  0){ _temp15=(( struct Cyc_Dict_T*) _check_null( _temp15))->left;}
else{ if( _temp16 >  0){ _temp15=(( struct Cyc_Dict_T*) _check_null( _temp15))->right;}
else{ return({ void** _temp17=( void**) _region_malloc( r, sizeof( void*));
_temp17[ 0]=((( struct Cyc_Dict_T*) _check_null( _temp15))->key_val).f2; _temp17;});}}}
return 0;} int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void**
ans_place){ int(* _temp18)( void*, void*)= d->rel; struct Cyc_Dict_T* _temp19= d->t;
while( _temp19 !=  0) { int _temp20= _temp18( key,((( struct Cyc_Dict_T*)
_check_null( _temp19))->key_val).f1); if( _temp20 <  0){ _temp19=(( struct Cyc_Dict_T*)
_check_null( _temp19))->left;} else{ if( _temp20 >  0){ _temp19=(( struct Cyc_Dict_T*)
_check_null( _temp19))->right;} else{* ans_place=((( struct Cyc_Dict_T*)
_check_null( _temp19))->key_val).f2; return 1;}}} return 0;} struct _tuple1{
void* f1; struct Cyc_Dict_T* f2; struct Cyc_Dict_T* f3; struct _tuple0 f4; } ;
static struct Cyc_Dict_T* Cyc_Dict_balance( struct _RegionHandle* r, struct
_tuple1 quad){ struct _tuple1 _temp21= quad; struct _tuple0 _temp33; struct Cyc_Dict_T*
_temp35; struct Cyc_Dict_T* _temp37; struct Cyc_Dict_T _temp39; struct _tuple0
_temp40; struct Cyc_Dict_T* _temp42; struct Cyc_Dict_T* _temp44; struct Cyc_Dict_T
_temp46; struct _tuple0 _temp47; struct Cyc_Dict_T* _temp49; struct Cyc_Dict_T*
_temp51; void* _temp53; void* _temp55; void* _temp57; struct _tuple0 _temp59;
struct Cyc_Dict_T* _temp61; struct Cyc_Dict_T* _temp63; struct Cyc_Dict_T
_temp65; struct _tuple0 _temp66; struct Cyc_Dict_T* _temp68; struct Cyc_Dict_T
_temp70; struct _tuple0 _temp71; struct Cyc_Dict_T* _temp73; struct Cyc_Dict_T*
_temp75; void* _temp77; struct Cyc_Dict_T* _temp79; void* _temp81; void* _temp83;
struct _tuple0 _temp85; struct Cyc_Dict_T* _temp87; struct Cyc_Dict_T _temp89;
struct _tuple0 _temp90; struct Cyc_Dict_T* _temp92; struct Cyc_Dict_T* _temp94;
struct Cyc_Dict_T _temp96; struct _tuple0 _temp97; struct Cyc_Dict_T* _temp99;
struct Cyc_Dict_T* _temp101; void* _temp103; void* _temp105; struct Cyc_Dict_T*
_temp107; void* _temp109; struct _tuple0 _temp111; struct Cyc_Dict_T* _temp113;
struct Cyc_Dict_T _temp115; struct _tuple0 _temp116; struct Cyc_Dict_T* _temp118;
struct Cyc_Dict_T _temp120; struct _tuple0 _temp121; struct Cyc_Dict_T* _temp123;
struct Cyc_Dict_T* _temp125; void* _temp127; struct Cyc_Dict_T* _temp129; void*
_temp131; struct Cyc_Dict_T* _temp133; void* _temp135; struct _tuple0 _temp137;
struct Cyc_Dict_T* _temp139; struct Cyc_Dict_T* _temp141; void* _temp143; _LL23:
_LL58: _temp57= _temp21.f1; if( _temp57 == ( void*) Cyc_Dict_B){ goto _LL38;}
else{ goto _LL25;} _LL38: _temp37= _temp21.f2; if( _temp37 ==  0){ goto _LL25;}
else{ _temp39=* _temp37; _LL56: _temp55=( void*) _temp39.color; if( _temp55 == (
void*) Cyc_Dict_R){ goto _LL45;} else{ goto _LL25;} _LL45: _temp44= _temp39.left;
if( _temp44 ==  0){ goto _LL25;} else{ _temp46=* _temp44; _LL54: _temp53=( void*)
_temp46.color; if( _temp53 == ( void*) Cyc_Dict_R){ goto _LL52;} else{ goto
_LL25;} _LL52: _temp51= _temp46.left; goto _LL50; _LL50: _temp49= _temp46.right;
goto _LL48; _LL48: _temp47= _temp46.key_val; goto _LL43;} _LL43: _temp42=
_temp39.right; goto _LL41; _LL41: _temp40= _temp39.key_val; goto _LL36;} _LL36:
_temp35= _temp21.f3; goto _LL34; _LL34: _temp33= _temp21.f4; goto _LL24; _LL25:
_LL84: _temp83= _temp21.f1; if( _temp83 == ( void*) Cyc_Dict_B){ goto _LL64;}
else{ goto _LL27;} _LL64: _temp63= _temp21.f2; if( _temp63 ==  0){ goto _LL27;}
else{ _temp65=* _temp63; _LL82: _temp81=( void*) _temp65.color; if( _temp81 == (
void*) Cyc_Dict_R){ goto _LL80;} else{ goto _LL27;} _LL80: _temp79= _temp65.left;
goto _LL69; _LL69: _temp68= _temp65.right; if( _temp68 ==  0){ goto _LL27;}
else{ _temp70=* _temp68; _LL78: _temp77=( void*) _temp70.color; if( _temp77 == (
void*) Cyc_Dict_R){ goto _LL76;} else{ goto _LL27;} _LL76: _temp75= _temp70.left;
goto _LL74; _LL74: _temp73= _temp70.right; goto _LL72; _LL72: _temp71= _temp70.key_val;
goto _LL67;} _LL67: _temp66= _temp65.key_val; goto _LL62;} _LL62: _temp61=
_temp21.f3; goto _LL60; _LL60: _temp59= _temp21.f4; goto _LL26; _LL27: _LL110:
_temp109= _temp21.f1; if( _temp109 == ( void*) Cyc_Dict_B){ goto _LL108;} else{
goto _LL29;} _LL108: _temp107= _temp21.f2; goto _LL88; _LL88: _temp87= _temp21.f3;
if( _temp87 ==  0){ goto _LL29;} else{ _temp89=* _temp87; _LL106: _temp105=(
void*) _temp89.color; if( _temp105 == ( void*) Cyc_Dict_R){ goto _LL95;} else{
goto _LL29;} _LL95: _temp94= _temp89.left; if( _temp94 ==  0){ goto _LL29;}
else{ _temp96=* _temp94; _LL104: _temp103=( void*) _temp96.color; if( _temp103
== ( void*) Cyc_Dict_R){ goto _LL102;} else{ goto _LL29;} _LL102: _temp101=
_temp96.left; goto _LL100; _LL100: _temp99= _temp96.right; goto _LL98; _LL98:
_temp97= _temp96.key_val; goto _LL93;} _LL93: _temp92= _temp89.right; goto _LL91;
_LL91: _temp90= _temp89.key_val; goto _LL86;} _LL86: _temp85= _temp21.f4; goto
_LL28; _LL29: _LL136: _temp135= _temp21.f1; if( _temp135 == ( void*) Cyc_Dict_B){
goto _LL134;} else{ goto _LL31;} _LL134: _temp133= _temp21.f2; goto _LL114;
_LL114: _temp113= _temp21.f3; if( _temp113 ==  0){ goto _LL31;} else{ _temp115=*
_temp113; _LL132: _temp131=( void*) _temp115.color; if( _temp131 == ( void*) Cyc_Dict_R){
goto _LL130;} else{ goto _LL31;} _LL130: _temp129= _temp115.left; goto _LL119;
_LL119: _temp118= _temp115.right; if( _temp118 ==  0){ goto _LL31;} else{
_temp120=* _temp118; _LL128: _temp127=( void*) _temp120.color; if( _temp127 == (
void*) Cyc_Dict_R){ goto _LL126;} else{ goto _LL31;} _LL126: _temp125= _temp120.left;
goto _LL124; _LL124: _temp123= _temp120.right; goto _LL122; _LL122: _temp121=
_temp120.key_val; goto _LL117;} _LL117: _temp116= _temp115.key_val; goto _LL112;}
_LL112: _temp111= _temp21.f4; goto _LL30; _LL31: _LL144: _temp143= _temp21.f1;
goto _LL142; _LL142: _temp141= _temp21.f2; goto _LL140; _LL140: _temp139=
_temp21.f3; goto _LL138; _LL138: _temp137= _temp21.f4; goto _LL32; _LL24: return({
struct Cyc_Dict_T* _temp145=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp145->color=( void*)(( void*) Cyc_Dict_R); _temp145->left=({
struct Cyc_Dict_T* _temp147=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp147->color=( void*)(( void*) Cyc_Dict_B); _temp147->left=
_temp51; _temp147->right= _temp49; _temp147->key_val= _temp47; _temp147;});
_temp145->right=({ struct Cyc_Dict_T* _temp146=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp146->color=( void*)(( void*)
Cyc_Dict_B); _temp146->left= _temp42; _temp146->right= _temp35; _temp146->key_val=
_temp33; _temp146;}); _temp145->key_val= _temp40; _temp145;}); _LL26: return({
struct Cyc_Dict_T* _temp148=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp148->color=( void*)(( void*) Cyc_Dict_R); _temp148->left=({
struct Cyc_Dict_T* _temp150=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp150->color=( void*)(( void*) Cyc_Dict_B); _temp150->left=
_temp79; _temp150->right= _temp75; _temp150->key_val= _temp66; _temp150;});
_temp148->right=({ struct Cyc_Dict_T* _temp149=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp149->color=( void*)(( void*)
Cyc_Dict_B); _temp149->left= _temp73; _temp149->right= _temp61; _temp149->key_val=
_temp59; _temp149;}); _temp148->key_val= _temp71; _temp148;}); _LL28: return({
struct Cyc_Dict_T* _temp151=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp151->color=( void*)(( void*) Cyc_Dict_R); _temp151->left=({
struct Cyc_Dict_T* _temp153=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp153->color=( void*)(( void*) Cyc_Dict_B); _temp153->left=
_temp107; _temp153->right= _temp101; _temp153->key_val= _temp85; _temp153;});
_temp151->right=({ struct Cyc_Dict_T* _temp152=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp152->color=( void*)(( void*)
Cyc_Dict_B); _temp152->left= _temp99; _temp152->right= _temp92; _temp152->key_val=
_temp90; _temp152;}); _temp151->key_val= _temp97; _temp151;}); _LL30: return({
struct Cyc_Dict_T* _temp154=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp154->color=( void*)(( void*) Cyc_Dict_R); _temp154->left=({
struct Cyc_Dict_T* _temp156=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp156->color=( void*)(( void*) Cyc_Dict_B); _temp156->left=
_temp133; _temp156->right= _temp129; _temp156->key_val= _temp111; _temp156;});
_temp154->right=({ struct Cyc_Dict_T* _temp155=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp155->color=( void*)(( void*)
Cyc_Dict_B); _temp155->left= _temp125; _temp155->right= _temp123; _temp155->key_val=
_temp121; _temp155;}); _temp154->key_val= _temp116; _temp154;}); _LL32: return({
struct Cyc_Dict_T* _temp157=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp157->color=( void*) _temp143; _temp157->left= _temp141;
_temp157->right= _temp139; _temp157->key_val= _temp137; _temp157;}); _LL22:;}
static struct Cyc_Dict_T* Cyc_Dict_ins( struct _RegionHandle* r, int(* rel)(
void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t){ struct Cyc_Dict_T*
_temp158= t; struct Cyc_Dict_T _temp164; struct _tuple0 _temp165; struct Cyc_Dict_T*
_temp167; struct Cyc_Dict_T* _temp169; void* _temp171; _LL160: if( _temp158 == 
0){ goto _LL161;} else{ goto _LL162;} _LL162: if( _temp158 ==  0){ goto _LL159;}
else{ _temp164=* _temp158; _LL172: _temp171=( void*) _temp164.color; goto _LL170;
_LL170: _temp169= _temp164.left; goto _LL168; _LL168: _temp167= _temp164.right;
goto _LL166; _LL166: _temp165= _temp164.key_val; goto _LL163;} _LL161: return({
struct Cyc_Dict_T* _temp173=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp173->color=( void*)(( void*) Cyc_Dict_R); _temp173->left=
0; _temp173->right= 0; _temp173->key_val= key_val; _temp173;}); _LL163: { int
_temp174= rel( key_val.f1, _temp165.f1); if( _temp174 <  0){ return Cyc_Dict_balance(
r,({ struct _tuple1 _temp175; _temp175.f1= _temp171; _temp175.f2= Cyc_Dict_ins(
r, rel, key_val, _temp169); _temp175.f3= _temp167; _temp175.f4= _temp165;
_temp175;}));} else{ if( _temp174 >  0){ return Cyc_Dict_balance( r,({ struct
_tuple1 _temp176; _temp176.f1= _temp171; _temp176.f2= _temp169; _temp176.f3= Cyc_Dict_ins(
r, rel, key_val, _temp167); _temp176.f4= _temp165; _temp176;}));} else{ return({
struct Cyc_Dict_T* _temp177=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp177->color=( void*) _temp171; _temp177->left= _temp169;
_temp177->right= _temp167; _temp177->key_val= key_val; _temp177;});}}} _LL159:;}
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data){ struct Cyc_Dict_T* _temp178= Cyc_Dict_ins( d->r, d->rel,({ struct _tuple0
_temp180; _temp180.f1= key; _temp180.f2= data; _temp180;}), d->t);( void*)(((
struct Cyc_Dict_T*) _check_null( _temp178))->color=( void*)(( void*) Cyc_Dict_B));
return({ struct Cyc_Dict_Dict* _temp179=( struct Cyc_Dict_Dict*) _region_malloc(
d->r, sizeof( struct Cyc_Dict_Dict)); _temp179->rel= d->rel; _temp179->r= d->r;
_temp179->t= _temp178; _temp179;});} struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* key, void* data){ if( Cyc_Dict_member( d, key)){(
int) _throw(( void*) Cyc_Dict_Absent);} return Cyc_Dict_insert( d, key, data);}
struct Cyc_Dict_Dict* Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
kds){ for( 0; kds !=  0; kds=(( struct Cyc_List_List*) _check_null( kds))->tl){
d= Cyc_Dict_insert( d,((( struct _tuple0*)(( struct Cyc_List_List*) _check_null(
kds))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f1,((( struct _tuple0*)((
struct Cyc_List_List*) _check_null( kds))->hd)[ _check_known_subscript_notnull(
1u, 0)]).f2);} return d;} static void* Cyc_Dict_fold_tree( void*(* f)( void*,
void*, void*), struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp183;
struct _tuple0 _temp184; void* _temp186; void* _temp188; struct Cyc_Dict_T*
_temp190; struct Cyc_Dict_T* _temp192; struct Cyc_Dict_T* _temp181= t; _temp183=*
_temp181; _LL193: _temp192= _temp183.left; goto _LL191; _LL191: _temp190=
_temp183.right; goto _LL185; _LL185: _temp184= _temp183.key_val; _LL189:
_temp188= _temp184.f1; goto _LL187; _LL187: _temp186= _temp184.f2; goto _LL182;
_LL182: if( _temp192 !=  0){ accum= Cyc_Dict_fold_tree( f,( struct Cyc_Dict_T*)
_check_null( _temp192), accum);} accum= f( _temp188, _temp186, accum); if(
_temp190 !=  0){ accum= Cyc_Dict_fold_tree( f,( struct Cyc_Dict_T*) _check_null(
_temp190), accum);} return accum;} void* Cyc_Dict_fold( void*(* f)( void*, void*,
void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t ==  0){ return accum;}
return Cyc_Dict_fold_tree( f,( struct Cyc_Dict_T*) _check_null( d->t), accum);}
static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*, void*, void*), void*
env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp196; struct
_tuple0 _temp197; void* _temp199; void* _temp201; struct Cyc_Dict_T* _temp203;
struct Cyc_Dict_T* _temp205; struct Cyc_Dict_T* _temp194= t; _temp196=* _temp194;
_LL206: _temp205= _temp196.left; goto _LL204; _LL204: _temp203= _temp196.right;
goto _LL198; _LL198: _temp197= _temp196.key_val; _LL202: _temp201= _temp197.f1;
goto _LL200; _LL200: _temp199= _temp197.f2; goto _LL195; _LL195: if( _temp205 != 
0){ accum= Cyc_Dict_fold_tree_c( f, env,( struct Cyc_Dict_T*) _check_null(
_temp205), accum);} accum= f( env, _temp201, _temp199, accum); if( _temp203 != 
0){ accum= Cyc_Dict_fold_tree_c( f, env,( struct Cyc_Dict_T*) _check_null(
_temp203), accum);} return accum;} void* Cyc_Dict_fold_c( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d, void* accum){ if( d->t
==  0){ return accum;} return Cyc_Dict_fold_tree_c( f, env,( struct Cyc_Dict_T*)
_check_null( d->t), accum);} static void Cyc_Dict_app_tree( void*(* f)( void*,
void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp209; struct _tuple0
_temp210; void* _temp212; void* _temp214; struct Cyc_Dict_T* _temp216; struct
Cyc_Dict_T* _temp218; struct Cyc_Dict_T* _temp207= t; _temp209=* _temp207;
_LL219: _temp218= _temp209.left; goto _LL217; _LL217: _temp216= _temp209.right;
goto _LL211; _LL211: _temp210= _temp209.key_val; _LL215: _temp214= _temp210.f1;
goto _LL213; _LL213: _temp212= _temp210.f2; goto _LL208; _LL208: if( _temp218 != 
0){ Cyc_Dict_app_tree( f,( struct Cyc_Dict_T*) _check_null( _temp218));} f(
_temp214, _temp212); if( _temp216 !=  0){ Cyc_Dict_app_tree( f,( struct Cyc_Dict_T*)
_check_null( _temp216));}} void Cyc_Dict_app( void*(* f)( void*, void*), struct
Cyc_Dict_Dict* d){ if( d->t !=  0){ Cyc_Dict_app_tree( f,( struct Cyc_Dict_T*)
_check_null( d->t));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp222; struct
_tuple0 _temp223; void* _temp225; void* _temp227; struct Cyc_Dict_T* _temp229;
struct Cyc_Dict_T* _temp231; struct Cyc_Dict_T* _temp220= t; _temp222=* _temp220;
_LL232: _temp231= _temp222.left; goto _LL230; _LL230: _temp229= _temp222.right;
goto _LL224; _LL224: _temp223= _temp222.key_val; _LL228: _temp227= _temp223.f1;
goto _LL226; _LL226: _temp225= _temp223.f2; goto _LL221; _LL221: if( _temp231 != 
0){ Cyc_Dict_app_tree_c( f, env,( struct Cyc_Dict_T*) _check_null( _temp231));}
f( env, _temp227, _temp225); if( _temp229 !=  0){ Cyc_Dict_app_tree_c( f, env,(
struct Cyc_Dict_T*) _check_null( _temp229));}} void Cyc_Dict_app_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t !=  0){ Cyc_Dict_app_tree_c(
f, env,( struct Cyc_Dict_T*) _check_null( d->t));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp235;
struct _tuple0 _temp236; void* _temp238; void* _temp240; struct Cyc_Dict_T*
_temp242; struct Cyc_Dict_T* _temp244; struct Cyc_Dict_T* _temp233= t; _temp235=*
_temp233; _LL245: _temp244= _temp235.left; goto _LL243; _LL243: _temp242=
_temp235.right; goto _LL237; _LL237: _temp236= _temp235.key_val; _LL241:
_temp240= _temp236.f1; goto _LL239; _LL239: _temp238= _temp236.f2; goto _LL234;
_LL234: if( _temp244 !=  0){ Cyc_Dict_iter_tree( f,( struct Cyc_Dict_T*)
_check_null( _temp244));} f( _temp240, _temp238); if( _temp242 !=  0){ Cyc_Dict_iter_tree(
f,( struct Cyc_Dict_T*) _check_null( _temp242));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t !=  0){ Cyc_Dict_iter_tree( f,(
struct Cyc_Dict_T*) _check_null( d->t));}} static void Cyc_Dict_iter_tree_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp248; struct _tuple0 _temp249; void* _temp251; void* _temp253; struct Cyc_Dict_T*
_temp255; struct Cyc_Dict_T* _temp257; struct Cyc_Dict_T* _temp246= t; _temp248=*
_temp246; _LL258: _temp257= _temp248.left; goto _LL256; _LL256: _temp255=
_temp248.right; goto _LL250; _LL250: _temp249= _temp248.key_val; _LL254:
_temp253= _temp249.f1; goto _LL252; _LL252: _temp251= _temp249.f2; goto _LL247;
_LL247: if( _temp257 !=  0){ Cyc_Dict_iter_tree_c( f, env,( struct Cyc_Dict_T*)
_check_null( _temp257));} f( env, _temp253, _temp251); if( _temp255 !=  0){ Cyc_Dict_iter_tree_c(
f, env,( struct Cyc_Dict_T*) _check_null( _temp255));}} void Cyc_Dict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t
!=  0){ Cyc_Dict_iter_tree_c( f, env,( struct Cyc_Dict_T*) _check_null( d->t));}}
struct _tuple2{ void(* f1)( void*, void*); struct Cyc_Dict_Dict* f2; } ; static
void Cyc_Dict_iter2_f( struct _tuple2* env, void* a, void* b1){ struct _tuple2
_temp261; struct Cyc_Dict_Dict* _temp262; void(* _temp264)( void*, void*);
struct _tuple2* _temp259= env; _temp261=* _temp259; _LL265: _temp264= _temp261.f1;
goto _LL263; _LL263: _temp262= _temp261.f2; goto _LL260; _LL260: _temp264( b1,
Cyc_Dict_lookup( _temp262, a));} void Cyc_Dict_iter2( void(* f)( void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple2 _temp266=({
struct _tuple2 _temp267; _temp267.f1= f; _temp267.f2= d2; _temp267;});(( void(*)(
void(* f)( struct _tuple2*, void*, void*), struct _tuple2* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Dict_iter2_f,& _temp266, d1);} struct _tuple3{ void(*
f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void* f3; } ; static void
Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){ struct _tuple3
_temp270; void* _temp271; struct Cyc_Dict_Dict* _temp273; void(* _temp275)( void*,
void*, void*); struct _tuple3* _temp268= env; _temp270=* _temp268; _LL276:
_temp275= _temp270.f1; goto _LL274; _LL274: _temp273= _temp270.f2; goto _LL272;
_LL272: _temp271= _temp270.f3; goto _LL269; _LL269: _temp275( _temp271, b1, Cyc_Dict_lookup(
_temp273, a));} void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void*
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3
_temp277=({ struct _tuple3 _temp278; _temp278.f1= f; _temp278.f2= d2; _temp278.f3=
inner_env; _temp278;});(( void(*)( void(* f)( struct _tuple3*, void*, void*),
struct _tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Dict_iter2_c_f,&
_temp277, d1);} struct _tuple4{ void*(* f1)( void*, void*, void*, void*, void*);
struct Cyc_Dict_Dict* f2; void* f3; } ; static void* Cyc_Dict_fold2_c_f( struct
_tuple4* env, void* a, void* b1, void* accum){ struct _tuple4 _temp281; void*
_temp282; struct Cyc_Dict_Dict* _temp284; void*(* _temp286)( void*, void*, void*,
void*, void*); struct _tuple4* _temp279= env; _temp281=* _temp279; _LL287:
_temp286= _temp281.f1; goto _LL285; _LL285: _temp284= _temp281.f2; goto _LL283;
_LL283: _temp282= _temp281.f3; goto _LL280; _LL280: return _temp286( _temp282, a,
b1, Cyc_Dict_lookup( _temp284, a), accum);} void* Cyc_Dict_fold2_c( void*(* f)(
void*, void*, void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2, void* accum){ struct _tuple4 _temp288=({ struct
_tuple4 _temp289; _temp289.f1= f; _temp289.f2= d2; _temp289.f3= inner_env;
_temp289;}); return(( void*(*)( void*(* f)( struct _tuple4*, void*, void*, void*),
struct _tuple4* env, struct Cyc_Dict_Dict* d, void* accum)) Cyc_Dict_fold_c)(
Cyc_Dict_fold2_c_f,& _temp288, d1, accum);} static struct Cyc_Dict_T* Cyc_Dict_copy_tree(
struct _RegionHandle* r2, struct Cyc_Dict_T* t){ if( t ==  0){ return 0;} else{
struct _tuple0 _temp292; struct Cyc_Dict_T* _temp294; struct Cyc_Dict_T*
_temp296; void* _temp298; struct Cyc_Dict_T _temp290=*(( struct Cyc_Dict_T*)
_check_null( t)); _LL299: _temp298=( void*) _temp290.color; goto _LL297; _LL297:
_temp296= _temp290.left; goto _LL295; _LL295: _temp294= _temp290.right; goto
_LL293; _LL293: _temp292= _temp290.key_val; goto _LL291; _LL291: { struct Cyc_Dict_T*
_temp300= Cyc_Dict_copy_tree( r2, _temp296); struct Cyc_Dict_T* _temp301= Cyc_Dict_copy_tree(
r2, _temp294); return({ struct Cyc_Dict_T* _temp302=( struct Cyc_Dict_T*)
_region_malloc( r2, sizeof( struct Cyc_Dict_T)); _temp302->color=( void*)
_temp298; _temp302->left= _temp300; _temp302->right= _temp301; _temp302->key_val=
_temp292; _temp302;});}}} struct Cyc_Dict_Dict* Cyc_Dict_rcopy( struct
_RegionHandle* r2, struct Cyc_Dict_Dict* d){ return({ struct Cyc_Dict_Dict*
_temp303=( struct Cyc_Dict_Dict*) _region_malloc( r2, sizeof( struct Cyc_Dict_Dict));
_temp303->rel= d->rel; _temp303->r= r2; _temp303->t= Cyc_Dict_copy_tree( r2, d->t);
_temp303;});} struct Cyc_Dict_Dict* Cyc_Dict_copy( struct Cyc_Dict_Dict* d){
return Cyc_Dict_rcopy( Cyc_Core_heap_region, d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree(
struct _RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp306; struct _tuple0 _temp307; void* _temp309; void* _temp311; struct Cyc_Dict_T*
_temp313; struct Cyc_Dict_T* _temp315; void* _temp317; struct Cyc_Dict_T*
_temp304= t; _temp306=* _temp304; _LL318: _temp317=( void*) _temp306.color; goto
_LL316; _LL316: _temp315= _temp306.left; goto _LL314; _LL314: _temp313= _temp306.right;
goto _LL308; _LL308: _temp307= _temp306.key_val; _LL312: _temp311= _temp307.f1;
goto _LL310; _LL310: _temp309= _temp307.f2; goto _LL305; _LL305: { struct Cyc_Dict_T*
_temp319= _temp315 ==  0? 0: Cyc_Dict_map_tree( r, f,( struct Cyc_Dict_T*)
_check_null( _temp315)); void* _temp320= f( _temp309); struct Cyc_Dict_T*
_temp321= _temp313 ==  0? 0: Cyc_Dict_map_tree( r, f,( struct Cyc_Dict_T*)
_check_null( _temp313)); return({ struct Cyc_Dict_T* _temp322=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp322->color=( void*)
_temp317; _temp322->left= _temp319; _temp322->right= _temp321; _temp322->key_val=({
struct _tuple0 _temp323; _temp323.f1= _temp311; _temp323.f2= _temp320; _temp323;});
_temp322;});}} struct Cyc_Dict_Dict* Cyc_Dict_rmap( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_Dict* d){ if( d->t ==  0){ return({ struct
Cyc_Dict_Dict* _temp324=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof(
struct Cyc_Dict_Dict)); _temp324->rel= d->rel; _temp324->r= r; _temp324->t= 0;
_temp324;});} return({ struct Cyc_Dict_Dict* _temp325=( struct Cyc_Dict_Dict*)
_region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp325->rel= d->rel;
_temp325->r= r; _temp325->t= Cyc_Dict_map_tree( r, f,( struct Cyc_Dict_T*)
_check_null( d->t)); _temp325;});} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)(
void*), struct Cyc_Dict_Dict* d){ return Cyc_Dict_rmap( Cyc_Core_heap_region, f,
d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( struct _RegionHandle* r,
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp328; struct _tuple0 _temp329; void* _temp331; void* _temp333; struct Cyc_Dict_T*
_temp335; struct Cyc_Dict_T* _temp337; void* _temp339; struct Cyc_Dict_T*
_temp326= t; _temp328=* _temp326; _LL340: _temp339=( void*) _temp328.color; goto
_LL338; _LL338: _temp337= _temp328.left; goto _LL336; _LL336: _temp335= _temp328.right;
goto _LL330; _LL330: _temp329= _temp328.key_val; _LL334: _temp333= _temp329.f1;
goto _LL332; _LL332: _temp331= _temp329.f2; goto _LL327; _LL327: { struct Cyc_Dict_T*
_temp341= _temp337 ==  0? 0: Cyc_Dict_map_tree_c( r, f, env,( struct Cyc_Dict_T*)
_check_null( _temp337)); void* _temp342= f( env, _temp331); struct Cyc_Dict_T*
_temp343= _temp335 ==  0? 0: Cyc_Dict_map_tree_c( r, f, env,( struct Cyc_Dict_T*)
_check_null( _temp335)); return({ struct Cyc_Dict_T* _temp344=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp344->color=( void*)
_temp339; _temp344->left= _temp341; _temp344->right= _temp343; _temp344->key_val=({
struct _tuple0 _temp345; _temp345.f1= _temp333; _temp345.f2= _temp342; _temp345;});
_temp344;});}} struct Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct _RegionHandle* r,
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t ==  0){
return({ struct Cyc_Dict_Dict* _temp346=( struct Cyc_Dict_Dict*) _region_malloc(
r, sizeof( struct Cyc_Dict_Dict)); _temp346->rel= d->rel; _temp346->r= r;
_temp346->t= 0; _temp346;});} return({ struct Cyc_Dict_Dict* _temp347=( struct
Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp347->rel=
d->rel; _temp347->r= r; _temp347->t= Cyc_Dict_map_tree_c( r, f, env,( struct Cyc_Dict_T*)
_check_null( d->t)); _temp347;});} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(*
f)( void*, void*), void* env, struct Cyc_Dict_Dict* d){ return Cyc_Dict_rmap_c(
Cyc_Core_heap_region, f, env, d);} struct _tuple0* Cyc_Dict_rchoose( struct
_RegionHandle* r, struct Cyc_Dict_Dict* d){ if( d->t ==  0){( int) _throw(( void*)
Cyc_Dict_Absent);} return({ struct _tuple0* _temp348=( struct _tuple0*)
_region_malloc( r, sizeof( struct _tuple0)); _temp348->f1=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f1; _temp348->f2=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f2; _temp348;});} static int Cyc_Dict_forall_tree_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp351; struct _tuple0 _temp352; void* _temp354; void* _temp356; struct Cyc_Dict_T*
_temp358; struct Cyc_Dict_T* _temp360; struct Cyc_Dict_T* _temp349= t; _temp351=*
_temp349; _LL361: _temp360= _temp351.left; goto _LL359; _LL359: _temp358=
_temp351.right; goto _LL353; _LL353: _temp352= _temp351.key_val; _LL357:
_temp356= _temp352.f1; goto _LL355; _LL355: _temp354= _temp352.f2; goto _LL350;
_LL350: return(( _temp360 ==  0? 1: Cyc_Dict_forall_tree_c( f, env,( struct Cyc_Dict_T*)
_check_null( _temp360)))? f( env, _temp356, _temp354): 0)? _temp358 ==  0? 1:
Cyc_Dict_forall_tree_c( f, env,( struct Cyc_Dict_T*) _check_null( _temp358)): 0;}
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d){ if( d->t ==  0){ return 1;} return Cyc_Dict_forall_tree_c( f, env,( struct
Cyc_Dict_T*) _check_null( d->t));} struct _tuple5{ int(* f1)( void*, void*, void*);
struct Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f( struct
_tuple5* env, void* a, void* b){ struct _tuple5 _temp364; struct Cyc_Dict_Dict*
_temp365; int(* _temp367)( void*, void*, void*); struct _tuple5* _temp362= env;
_temp364=* _temp362; _LL368: _temp367= _temp364.f1; goto _LL366; _LL366:
_temp365= _temp364.f2; goto _LL363; _LL363: if( Cyc_Dict_member( _temp365, a)){
return _temp367( a, b, Cyc_Dict_lookup( _temp365, a));} return 1;} int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ struct _tuple5 _temp369=({ struct _tuple5 _temp370; _temp370.f1= f;
_temp370.f2= d2; _temp370;}); return(( int(*)( int(* f)( struct _tuple5*, void*,
void*), struct _tuple5* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)( Cyc_Dict_forall_intersect_f,&
_temp369, d1);} static struct Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*,
void*), void* a, void* b, struct Cyc_Dict_Dict* d1){ if( Cyc_Dict_member( d1, a)){
return Cyc_Dict_insert( d1, a, f( Cyc_Dict_lookup( d1, a), b));} else{ return
Cyc_Dict_insert( d1, a, b);}} struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(*
f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( void*(*)( void*, void*),
void*, void*, struct Cyc_Dict_Dict*), void*(* env)( void*, void*), struct Cyc_Dict_Dict*
d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)( Cyc_Dict_union_f, f, d1, d2);}
struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 ==  d2? 1:
d2->t ==  0){ return d2;}{ struct _tagged_arr queue= _tag_arr(({ unsigned int
_temp391=( unsigned int) 16; struct Cyc_Dict_T** _temp392=( struct Cyc_Dict_T**)
_cycalloc( _check_times( sizeof( struct Cyc_Dict_T*), _temp391));{ unsigned int
_temp393= _temp391; unsigned int i; for( i= 0; i <  _temp393; i ++){ _temp392[ i]=(
struct Cyc_Dict_T*) _check_null( d2->t);}}; _temp392;}), sizeof( struct Cyc_Dict_T*),(
unsigned int) 16); int ind= 0; struct Cyc_Dict_T* _temp371= 0; while( ind !=  -
1) { struct Cyc_Dict_T _temp374; struct _tuple0 _temp375; void* _temp377; void*
_temp379; struct Cyc_Dict_T* _temp381; struct Cyc_Dict_T* _temp383; struct Cyc_Dict_T*
_temp372=*(( struct Cyc_Dict_T**) _check_unknown_subscript( queue, sizeof(
struct Cyc_Dict_T*), ind --)); _temp374=* _temp372; _LL384: _temp383= _temp374.left;
goto _LL382; _LL382: _temp381= _temp374.right; goto _LL376; _LL376: _temp375=
_temp374.key_val; _LL380: _temp379= _temp375.f1; goto _LL378; _LL378: _temp377=
_temp375.f2; goto _LL373; _LL373: if( ind +  2 >=  _get_arr_size( queue, sizeof(
struct Cyc_Dict_T*))){ queue=({ unsigned int _temp385= _get_arr_size( queue,
sizeof( struct Cyc_Dict_T*)) *  2; struct Cyc_Dict_T** _temp386=( struct Cyc_Dict_T**)
_cycalloc( _check_times( sizeof( struct Cyc_Dict_T*), _temp385)); struct
_tagged_arr _temp388= _tag_arr( _temp386, sizeof( struct Cyc_Dict_T*),
_get_arr_size( queue, sizeof( struct Cyc_Dict_T*)) *  2);{ unsigned int _temp387=
_temp385; unsigned int i; for( i= 0; i <  _temp387; i ++){ _temp386[ i]= i < 
_get_arr_size( queue, sizeof( struct Cyc_Dict_T*))?*(( struct Cyc_Dict_T**)
_check_unknown_subscript( queue, sizeof( struct Cyc_Dict_T*),( int) i)):( struct
Cyc_Dict_T*) _check_null( d2->t);}}; _temp388;});} if( _temp383 !=  0){*((
struct Cyc_Dict_T**) _check_unknown_subscript( queue, sizeof( struct Cyc_Dict_T*),
++ ind))=( struct Cyc_Dict_T*) _check_null( _temp383);} if( _temp381 !=  0){*((
struct Cyc_Dict_T**) _check_unknown_subscript( queue, sizeof( struct Cyc_Dict_T*),
++ ind))=( struct Cyc_Dict_T*) _check_null( _temp381);} if( Cyc_Dict_member( d1,
_temp379)){ _temp371= Cyc_Dict_ins( d2->r, d2->rel,({ struct _tuple0 _temp389;
_temp389.f1= _temp379; _temp389.f2= f( env, Cyc_Dict_lookup( d1, _temp379),
_temp377); _temp389;}), _temp371);}} return({ struct Cyc_Dict_Dict* _temp390=(
struct Cyc_Dict_Dict*) _region_malloc( d2->r, sizeof( struct Cyc_Dict_Dict));
_temp390->rel= d2->rel; _temp390->r= d2->r; _temp390->t= _temp371; _temp390;});}}
static struct Cyc_List_List* Cyc_Dict_to_list_f( struct _RegionHandle* r, void*
k, void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List*
_temp394=( struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp394->hd=( void*)({ struct _tuple0* _temp395=( struct _tuple0*)
_region_malloc( r, sizeof( struct _tuple0)); _temp395->f1= k; _temp395->f2= v;
_temp395;}); _temp394->tl= accum; _temp394;});} struct Cyc_List_List* Cyc_Dict_rto_list(
struct _RegionHandle* r, struct Cyc_Dict_Dict* d){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _RegionHandle*, void*, void*, struct Cyc_List_List*),
struct _RegionHandle* env, struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold_c)( Cyc_Dict_to_list_f, r, d, 0);} struct Cyc_List_List* Cyc_Dict_to_list(
struct Cyc_Dict_Dict* d){ return Cyc_Dict_rto_list( Cyc_Core_heap_region, d);}
struct _tuple6{ int(* f1)( void*, void*); struct _RegionHandle* f2; } ; static
struct Cyc_Dict_Dict* Cyc_Dict_filter_f( struct _tuple6* env, void* x, void* y,
struct Cyc_Dict_Dict* acc){ struct _tuple6 _temp398; struct _RegionHandle*
_temp399; int(* _temp401)( void*, void*); struct _tuple6* _temp396= env;
_temp398=* _temp396; _LL402: _temp401= _temp398.f1; goto _LL400; _LL400:
_temp399= _temp398.f2; goto _LL397; _LL397: return _temp401( x, y)? Cyc_Dict_insert(
acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter( struct _RegionHandle*
r2, int(* f)( void*, void*), struct Cyc_Dict_Dict* d){ struct _tuple6 _temp403=({
struct _tuple6 _temp404; _temp404.f1= f; _temp404.f2= r2; _temp404;}); struct
Cyc_Dict_Dict*(* ff)( struct _tuple6*, void*, void*, struct Cyc_Dict_Dict*)= Cyc_Dict_filter_f;
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)( struct _tuple6*,
void*, void*, struct Cyc_Dict_Dict*), struct _tuple6* env, struct Cyc_Dict_Dict*
d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)( ff,& _temp403, d, Cyc_Dict_rempty(
r2, d->rel));} struct Cyc_Dict_Dict* Cyc_Dict_filter( int(* f)( void*, void*),
struct Cyc_Dict_Dict* d){ return Cyc_Dict_rfilter( Cyc_Core_heap_region, f, d);}
struct _tuple7{ int(* f1)( void*, void*, void*); void* f2; struct _RegionHandle*
f3; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_c_f( struct _tuple7* env,
void* x, void* y, struct Cyc_Dict_Dict* acc){ struct _tuple7 _temp407; struct
_RegionHandle* _temp408; void* _temp410; int(* _temp412)( void*, void*, void*);
struct _tuple7* _temp405= env; _temp407=* _temp405; _LL413: _temp412= _temp407.f1;
goto _LL411; _LL411: _temp410= _temp407.f2; goto _LL409; _LL409: _temp408=
_temp407.f3; goto _LL406; _LL406: return _temp412( _temp410, x, y)? Cyc_Dict_insert(
acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle*
r2, int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){
struct _tuple7 _temp414=({ struct _tuple7 _temp415; _temp415.f1= f; _temp415.f2=
f_env; _temp415.f3= r2; _temp415;}); return(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict*(* f)( struct _tuple7*, void*, void*, struct Cyc_Dict_Dict*),
struct _tuple7* env, struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(
Cyc_Dict_filter_c_f,& _temp414, d, Cyc_Dict_rempty( r2, d->rel));} struct Cyc_Dict_Dict*
Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict*
d){ return Cyc_Dict_rfilter_c( Cyc_Core_heap_region, f, f_env, d);} static int
Cyc_Dict_difference_f( struct Cyc_Dict_Dict* d, void* x, void* y){ return ! Cyc_Dict_member(
d, x);} struct Cyc_Dict_Dict* Cyc_Dict_rdifference( struct _RegionHandle* r2,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return(( struct Cyc_Dict_Dict*(*)(
struct _RegionHandle* r2, int(* f)( struct Cyc_Dict_Dict*, void*, void*), struct
Cyc_Dict_Dict* f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter_c)( r2, Cyc_Dict_difference_f,
d2, d1);} struct Cyc_Dict_Dict* Cyc_Dict_difference( struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2){ return Cyc_Dict_rdifference( Cyc_Core_heap_region, d1,
d2);} struct _tuple8{ int(* f1)( void*, void*); void* f2; } ; static int Cyc_Dict_delete_f(
struct _tuple8* env, void* x, void* y){ struct _tuple8 _temp418; void* _temp419;
int(* _temp421)( void*, void*); struct _tuple8* _temp416= env; _temp418=*
_temp416; _LL422: _temp421= _temp418.f1; goto _LL420; _LL420: _temp419= _temp418.f2;
goto _LL417; _LL417: return _temp421( _temp419, x) ==  0;} struct Cyc_Dict_Dict*
Cyc_Dict_rdelete( struct _RegionHandle* r2, struct Cyc_Dict_Dict* d, void* x){
if( ! Cyc_Dict_member( d, x)){ return Cyc_Dict_rcopy( r2, d);}{ struct _tuple8
_temp423=({ struct _tuple8 _temp424; _temp424.f1= d->rel; _temp424.f2= x;
_temp424;}); return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, int(*
f)( struct _tuple8*, void*, void*), struct _tuple8* f_env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rfilter_c)( r2, Cyc_Dict_delete_f,& _temp423, d);}} struct Cyc_Dict_Dict*
Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict* d, void* x){ if( ! Cyc_Dict_member(
d, x)){ return d;}{ struct _tuple8 _temp425=({ struct _tuple8 _temp426; _temp426.f1=
d->rel; _temp426.f2= x; _temp426;}); return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r2, int(* f)( struct _tuple8*, void*, void*), struct _tuple8*
f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter_c)( d->r, Cyc_Dict_delete_f,&
_temp425, d);}} struct Cyc_Dict_Dict* Cyc_Dict_delete( struct Cyc_Dict_Dict* d,
void* x){ return Cyc_Dict_rdelete( Cyc_Core_heap_region, d, x);}
