 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
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
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern void* Cyc_Dict_fold2_c(
void*(* f)( void*, void*, void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2, void* accum); extern struct Cyc_Dict_Dict* Cyc_Dict_rcopy(
struct _RegionHandle*, struct Cyc_Dict_Dict*); extern struct Cyc_Dict_Dict* Cyc_Dict_copy(
struct Cyc_Dict_Dict*); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)(
void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_intersect( void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct
Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2); extern int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern int Cyc_Dict_forall_intersect( int(* f)( void*,
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct
_tuple0{ void* f1; void* f2; } ; extern struct _tuple0* Cyc_Dict_rchoose( struct
_RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_List_List* Cyc_Dict_to_list(
struct Cyc_Dict_Dict* d); extern struct Cyc_List_List* Cyc_Dict_rto_list( struct
_RegionHandle*, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle*, int(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
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
void*, void*)= d->rel; struct Cyc_Dict_T* _temp5= d->t; while( _temp5 != 0) {
int _temp6= _temp4( key,((( struct Cyc_Dict_T*) _check_null( _temp5))->key_val).f1);
if( _temp6 < 0){ _temp5=(( struct Cyc_Dict_T*) _check_null( _temp5))->left;}
else{ if( _temp6 > 0){ _temp5=(( struct Cyc_Dict_T*) _check_null( _temp5))->right;}
else{ return 1;}}} return 0;} void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key){ int(* _temp7)( void*, void*)= d->rel; struct Cyc_Dict_T* _temp8= d->t;
while( _temp8 != 0) { int _temp9= _temp7( key,((( struct Cyc_Dict_T*)
_check_null( _temp8))->key_val).f1); if( _temp9 < 0){ _temp8=(( struct Cyc_Dict_T*)
_check_null( _temp8))->left;} else{ if( _temp9 > 0){ _temp8=(( struct Cyc_Dict_T*)
_check_null( _temp8))->right;} else{ return((( struct Cyc_Dict_T*) _check_null(
_temp8))->key_val).f2;}}}( int) _throw(( void*) Cyc_Dict_Absent);} struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key){ int(* _temp10)( void*,
void*)= d->rel; struct Cyc_Dict_T* _temp11= d->t; while( _temp11 != 0) { int
_temp12= _temp10( key,((( struct Cyc_Dict_T*) _check_null( _temp11))->key_val).f1);
if( _temp12 < 0){ _temp11=(( struct Cyc_Dict_T*) _check_null( _temp11))->left;}
else{ if( _temp12 > 0){ _temp11=(( struct Cyc_Dict_T*) _check_null( _temp11))->right;}
else{ return({ struct Cyc_Core_Opt* _temp13=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp13->v=( void*)((( struct Cyc_Dict_T*)
_check_null( _temp11))->key_val).f2; _temp13;});}}} return 0;} void** Cyc_Dict_rlookup_opt(
struct _RegionHandle* r, struct Cyc_Dict_Dict* d, void* key){ int(* _temp14)(
void*, void*)= d->rel; struct Cyc_Dict_T* _temp15= d->t; while( _temp15 != 0) {
int _temp16= _temp14( key,((( struct Cyc_Dict_T*) _check_null( _temp15))->key_val).f1);
if( _temp16 < 0){ _temp15=(( struct Cyc_Dict_T*) _check_null( _temp15))->left;}
else{ if( _temp16 > 0){ _temp15=(( struct Cyc_Dict_T*) _check_null( _temp15))->right;}
else{ return({ void** _temp17=( void**) _region_malloc( r, sizeof( void*));
_temp17[ 0]=((( struct Cyc_Dict_T*) _check_null( _temp15))->key_val).f2; _temp17;});}}}
return 0;} int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void**
ans_place){ int(* _temp18)( void*, void*)= d->rel; struct Cyc_Dict_T* _temp19= d->t;
while( _temp19 != 0) { int _temp20= _temp18( key,((( struct Cyc_Dict_T*)
_check_null( _temp19))->key_val).f1); if( _temp20 < 0){ _temp19=(( struct Cyc_Dict_T*)
_check_null( _temp19))->left;} else{ if( _temp20 > 0){ _temp19=(( struct Cyc_Dict_T*)
_check_null( _temp19))->right;} else{* ans_place=((( struct Cyc_Dict_T*)
_check_null( _temp19))->key_val).f2; return 1;}}} return 0;} struct _tuple1{
void* f1; struct Cyc_Dict_T* f2; struct Cyc_Dict_T* f3; struct _tuple0 f4; } ;
static struct Cyc_Dict_T* Cyc_Dict_balance( struct _RegionHandle* r, struct
_tuple1 quad_t){ struct _tuple1 _temp21= quad_t; struct _tuple0 _temp33; struct
Cyc_Dict_T* _temp35; struct Cyc_Dict_T* _temp37; struct Cyc_Dict_T _temp39;
struct _tuple0 _temp40; struct Cyc_Dict_T* _temp42; struct Cyc_Dict_T* _temp44;
struct Cyc_Dict_T _temp46; struct _tuple0 _temp47; struct Cyc_Dict_T* _temp49;
struct Cyc_Dict_T* _temp51; void* _temp53; void* _temp55; void* _temp57; struct
_tuple0 _temp59; struct Cyc_Dict_T* _temp61; struct Cyc_Dict_T* _temp63; struct
Cyc_Dict_T _temp65; struct _tuple0 _temp66; struct Cyc_Dict_T* _temp68; struct
Cyc_Dict_T _temp70; struct _tuple0 _temp71; struct Cyc_Dict_T* _temp73; struct
Cyc_Dict_T* _temp75; void* _temp77; struct Cyc_Dict_T* _temp79; void* _temp81;
void* _temp83; struct _tuple0 _temp85; struct Cyc_Dict_T* _temp87; struct Cyc_Dict_T
_temp89; struct _tuple0 _temp90; struct Cyc_Dict_T* _temp92; struct Cyc_Dict_T*
_temp94; struct Cyc_Dict_T _temp96; struct _tuple0 _temp97; struct Cyc_Dict_T*
_temp99; struct Cyc_Dict_T* _temp101; void* _temp103; void* _temp105; struct Cyc_Dict_T*
_temp107; void* _temp109; struct _tuple0 _temp111; struct Cyc_Dict_T* _temp113;
struct Cyc_Dict_T _temp115; struct _tuple0 _temp116; struct Cyc_Dict_T* _temp118;
struct Cyc_Dict_T _temp120; struct _tuple0 _temp121; struct Cyc_Dict_T* _temp123;
struct Cyc_Dict_T* _temp125; void* _temp127; struct Cyc_Dict_T* _temp129; void*
_temp131; struct Cyc_Dict_T* _temp133; void* _temp135; struct _tuple0 _temp137;
struct Cyc_Dict_T* _temp139; struct Cyc_Dict_T* _temp141; void* _temp143; _LL23:
_LL58: _temp57= _temp21.f1; if( _temp57 ==( void*) Cyc_Dict_B){ goto _LL38;}
else{ goto _LL25;} _LL38: _temp37= _temp21.f2; if( _temp37 == 0){ goto _LL25;}
else{ _temp39=* _temp37; _LL56: _temp55=( void*) _temp39.color; if( _temp55 ==(
void*) Cyc_Dict_R){ goto _LL45;} else{ goto _LL25;} _LL45: _temp44= _temp39.left;
if( _temp44 == 0){ goto _LL25;} else{ _temp46=* _temp44; _LL54: _temp53=( void*)
_temp46.color; if( _temp53 ==( void*) Cyc_Dict_R){ goto _LL52;} else{ goto _LL25;}
_LL52: _temp51= _temp46.left; goto _LL50; _LL50: _temp49= _temp46.right; goto
_LL48; _LL48: _temp47= _temp46.key_val; goto _LL43;} _LL43: _temp42= _temp39.right;
goto _LL41; _LL41: _temp40= _temp39.key_val; goto _LL36;} _LL36: _temp35=
_temp21.f3; goto _LL34; _LL34: _temp33= _temp21.f4; goto _LL24; _LL25: _LL84:
_temp83= _temp21.f1; if( _temp83 ==( void*) Cyc_Dict_B){ goto _LL64;} else{ goto
_LL27;} _LL64: _temp63= _temp21.f2; if( _temp63 == 0){ goto _LL27;} else{
_temp65=* _temp63; _LL82: _temp81=( void*) _temp65.color; if( _temp81 ==( void*)
Cyc_Dict_R){ goto _LL80;} else{ goto _LL27;} _LL80: _temp79= _temp65.left; goto
_LL69; _LL69: _temp68= _temp65.right; if( _temp68 == 0){ goto _LL27;} else{
_temp70=* _temp68; _LL78: _temp77=( void*) _temp70.color; if( _temp77 ==( void*)
Cyc_Dict_R){ goto _LL76;} else{ goto _LL27;} _LL76: _temp75= _temp70.left; goto
_LL74; _LL74: _temp73= _temp70.right; goto _LL72; _LL72: _temp71= _temp70.key_val;
goto _LL67;} _LL67: _temp66= _temp65.key_val; goto _LL62;} _LL62: _temp61=
_temp21.f3; goto _LL60; _LL60: _temp59= _temp21.f4; goto _LL26; _LL27: _LL110:
_temp109= _temp21.f1; if( _temp109 ==( void*) Cyc_Dict_B){ goto _LL108;} else{
goto _LL29;} _LL108: _temp107= _temp21.f2; goto _LL88; _LL88: _temp87= _temp21.f3;
if( _temp87 == 0){ goto _LL29;} else{ _temp89=* _temp87; _LL106: _temp105=( void*)
_temp89.color; if( _temp105 ==( void*) Cyc_Dict_R){ goto _LL95;} else{ goto
_LL29;} _LL95: _temp94= _temp89.left; if( _temp94 == 0){ goto _LL29;} else{
_temp96=* _temp94; _LL104: _temp103=( void*) _temp96.color; if( _temp103 ==(
void*) Cyc_Dict_R){ goto _LL102;} else{ goto _LL29;} _LL102: _temp101= _temp96.left;
goto _LL100; _LL100: _temp99= _temp96.right; goto _LL98; _LL98: _temp97= _temp96.key_val;
goto _LL93;} _LL93: _temp92= _temp89.right; goto _LL91; _LL91: _temp90= _temp89.key_val;
goto _LL86;} _LL86: _temp85= _temp21.f4; goto _LL28; _LL29: _LL136: _temp135=
_temp21.f1; if( _temp135 ==( void*) Cyc_Dict_B){ goto _LL134;} else{ goto _LL31;}
_LL134: _temp133= _temp21.f2; goto _LL114; _LL114: _temp113= _temp21.f3; if(
_temp113 == 0){ goto _LL31;} else{ _temp115=* _temp113; _LL132: _temp131=( void*)
_temp115.color; if( _temp131 ==( void*) Cyc_Dict_R){ goto _LL130;} else{ goto
_LL31;} _LL130: _temp129= _temp115.left; goto _LL119; _LL119: _temp118= _temp115.right;
if( _temp118 == 0){ goto _LL31;} else{ _temp120=* _temp118; _LL128: _temp127=(
void*) _temp120.color; if( _temp127 ==( void*) Cyc_Dict_R){ goto _LL126;} else{
goto _LL31;} _LL126: _temp125= _temp120.left; goto _LL124; _LL124: _temp123=
_temp120.right; goto _LL122; _LL122: _temp121= _temp120.key_val; goto _LL117;}
_LL117: _temp116= _temp115.key_val; goto _LL112;} _LL112: _temp111= _temp21.f4;
goto _LL30; _LL31: _LL144: _temp143= _temp21.f1; goto _LL142; _LL142: _temp141=
_temp21.f2; goto _LL140; _LL140: _temp139= _temp21.f3; goto _LL138; _LL138:
_temp137= _temp21.f4; goto _LL32; _LL24: return({ struct Cyc_Dict_T* _temp145=(
struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp145->color=(
void*)(( void*) Cyc_Dict_R); _temp145->left=({ struct Cyc_Dict_T* _temp147=(
struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp147->color=(
void*)(( void*) Cyc_Dict_B); _temp147->left= _temp51; _temp147->right= _temp49;
_temp147->key_val= _temp47; _temp147;}); _temp145->right=({ struct Cyc_Dict_T*
_temp146=( struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T));
_temp146->color=( void*)(( void*) Cyc_Dict_B); _temp146->left= _temp42; _temp146->right=
_temp35; _temp146->key_val= _temp33; _temp146;}); _temp145->key_val= _temp40;
_temp145;}); _LL26: return({ struct Cyc_Dict_T* _temp148=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp148->color=( void*)(( void*)
Cyc_Dict_R); _temp148->left=({ struct Cyc_Dict_T* _temp150=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp150->color=( void*)(( void*)
Cyc_Dict_B); _temp150->left= _temp79; _temp150->right= _temp75; _temp150->key_val=
_temp66; _temp150;}); _temp148->right=({ struct Cyc_Dict_T* _temp149=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp149->color=(
void*)(( void*) Cyc_Dict_B); _temp149->left= _temp73; _temp149->right= _temp61;
_temp149->key_val= _temp59; _temp149;}); _temp148->key_val= _temp71; _temp148;});
_LL28: return({ struct Cyc_Dict_T* _temp151=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp151->color=( void*)(( void*) Cyc_Dict_R);
_temp151->left=({ struct Cyc_Dict_T* _temp153=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp153->color=( void*)(( void*)
Cyc_Dict_B); _temp153->left= _temp107; _temp153->right= _temp101; _temp153->key_val=
_temp85; _temp153;}); _temp151->right=({ struct Cyc_Dict_T* _temp152=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp152->color=(
void*)(( void*) Cyc_Dict_B); _temp152->left= _temp99; _temp152->right= _temp92;
_temp152->key_val= _temp90; _temp152;}); _temp151->key_val= _temp97; _temp151;});
_LL30: return({ struct Cyc_Dict_T* _temp154=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp154->color=( void*)(( void*) Cyc_Dict_R);
_temp154->left=({ struct Cyc_Dict_T* _temp156=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp156->color=( void*)(( void*)
Cyc_Dict_B); _temp156->left= _temp133; _temp156->right= _temp129; _temp156->key_val=
_temp111; _temp156;}); _temp154->right=({ struct Cyc_Dict_T* _temp155=( struct
Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp155->color=(
void*)(( void*) Cyc_Dict_B); _temp155->left= _temp125; _temp155->right= _temp123;
_temp155->key_val= _temp121; _temp155;}); _temp154->key_val= _temp116; _temp154;});
_LL32: return({ struct Cyc_Dict_T* _temp157=( struct Cyc_Dict_T*) _region_malloc(
r, sizeof( struct Cyc_Dict_T)); _temp157->color=( void*) _temp143; _temp157->left=
_temp141; _temp157->right= _temp139; _temp157->key_val= _temp137; _temp157;});
_LL22:;} static struct Cyc_Dict_T* Cyc_Dict_ins( struct _RegionHandle* r, int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t){ struct Cyc_Dict_T*
_temp158= t; struct Cyc_Dict_T _temp164; struct _tuple0 _temp165; struct Cyc_Dict_T*
_temp167; struct Cyc_Dict_T* _temp169; void* _temp171; _LL160: if( _temp158 == 0){
goto _LL161;} else{ goto _LL162;} _LL162: if( _temp158 == 0){ goto _LL159;}
else{ _temp164=* _temp158; _LL172: _temp171=( void*) _temp164.color; goto _LL170;
_LL170: _temp169= _temp164.left; goto _LL168; _LL168: _temp167= _temp164.right;
goto _LL166; _LL166: _temp165= _temp164.key_val; goto _LL163;} _LL161: return({
struct Cyc_Dict_T* _temp173=( struct Cyc_Dict_T*) _region_malloc( r, sizeof(
struct Cyc_Dict_T)); _temp173->color=( void*)(( void*) Cyc_Dict_R); _temp173->left=
0; _temp173->right= 0; _temp173->key_val= key_val; _temp173;}); _LL163: { int
_temp174= rel( key_val.f1, _temp165.f1); if( _temp174 < 0){ return(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r, struct _tuple1 quad_t)) Cyc_Dict_balance)( r,({ struct
_tuple1 _temp175; _temp175.f1= _temp171; _temp175.f2=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r, int(* rel)( void*, void*), struct _tuple0 key_val,
struct Cyc_Dict_T* t)) Cyc_Dict_ins)( r, rel, key_val, _temp169); _temp175.f3=
_temp167; _temp175.f4= _temp165; _temp175;}));} else{ if( _temp174 > 0){ return((
struct Cyc_Dict_T*(*)( struct _RegionHandle* r, struct _tuple1 quad_t)) Cyc_Dict_balance)(
r,({ struct _tuple1 _temp176; _temp176.f1= _temp171; _temp176.f2= _temp169;
_temp176.f3=(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r, int(* rel)( void*,
void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( r, rel,
key_val, _temp167); _temp176.f4= _temp165; _temp176;}));} else{ return({ struct
Cyc_Dict_T* _temp177=( struct Cyc_Dict_T*) _region_malloc( r, sizeof( struct Cyc_Dict_T));
_temp177->color=( void*) _temp171; _temp177->left= _temp169; _temp177->right=
_temp167; _temp177->key_val= key_val; _temp177;});}}} _LL159:;} struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data){ struct Cyc_Dict_T*
_temp178=(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r, int(* rel)( void*,
void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( d->r, d->rel,({
struct _tuple0 _temp180; _temp180.f1= key; _temp180.f2= data; _temp180;}), d->t);(
void*)((( struct Cyc_Dict_T*) _check_null( _temp178))->color=( void*)(( void*)
Cyc_Dict_B)); return({ struct Cyc_Dict_Dict* _temp179=( struct Cyc_Dict_Dict*)
_region_malloc( d->r, sizeof( struct Cyc_Dict_Dict)); _temp179->rel= d->rel;
_temp179->r= d->r; _temp179->t= _temp178; _temp179;});} struct Cyc_Dict_Dict*
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
t, void* accum){ struct Cyc_Dict_T _temp183; struct _tuple0 _temp184; void*
_temp186; void* _temp188; struct Cyc_Dict_T* _temp190; struct Cyc_Dict_T*
_temp192; void* _temp194; struct Cyc_Dict_T* _temp181= t; _temp183=* _temp181;
_LL195: _temp194=( void*) _temp183.color; goto _LL193; _LL193: _temp192=
_temp183.left; goto _LL191; _LL191: _temp190= _temp183.right; goto _LL185;
_LL185: _temp184= _temp183.key_val; _LL189: _temp188= _temp184.f1; goto _LL187;
_LL187: _temp186= _temp184.f2; goto _LL182; _LL182: if( _temp192 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum))
Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*) _check_null( _temp192), accum);}
accum= f( _temp188, _temp186, accum); if( _temp190 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp190), accum);} return accum;} void*
Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void*
accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,(
struct Cyc_Dict_T*) _check_null( d->t), accum);} static void* Cyc_Dict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void*
accum){ struct Cyc_Dict_T _temp198; struct _tuple0 _temp199; void* _temp201;
void* _temp203; struct Cyc_Dict_T* _temp205; struct Cyc_Dict_T* _temp207; void*
_temp209; struct Cyc_Dict_T* _temp196= t; _temp198=* _temp196; _LL210: _temp209=(
void*) _temp198.color; goto _LL208; _LL208: _temp207= _temp198.left; goto _LL206;
_LL206: _temp205= _temp198.right; goto _LL200; _LL200: _temp199= _temp198.key_val;
_LL204: _temp203= _temp199.f1; goto _LL202; _LL202: _temp201= _temp199.f2; goto
_LL197; _LL197: if( _temp207 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp207), accum);} accum= f( env,
_temp203, _temp201, accum); if( _temp205 != 0){ accum=(( void*(*)( void*(* f)(
void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp205), accum);} return accum;}
void* Cyc_Dict_fold_c( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){ return accum;} return((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null(
d->t), accum);} static void Cyc_Dict_app_tree( void*(* f)( void*, void*), struct
Cyc_Dict_T* t){ struct Cyc_Dict_T _temp213; struct _tuple0 _temp214; void*
_temp216; void* _temp218; struct Cyc_Dict_T* _temp220; struct Cyc_Dict_T*
_temp222; void* _temp224; struct Cyc_Dict_T* _temp211= t; _temp213=* _temp211;
_LL225: _temp224=( void*) _temp213.color; goto _LL223; _LL223: _temp222=
_temp213.left; goto _LL221; _LL221: _temp220= _temp213.right; goto _LL215;
_LL215: _temp214= _temp213.key_val; _LL219: _temp218= _temp214.f1; goto _LL217;
_LL217: _temp216= _temp214.f2; goto _LL212; _LL212: if( _temp222 != 0){(( void(*)(
void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct
Cyc_Dict_T*) _check_null( _temp222));} f( _temp218, _temp216); if( _temp220 != 0){((
void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,(
struct Cyc_Dict_T*) _check_null( _temp220));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)
_check_null( d->t));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp228; struct
_tuple0 _temp229; void* _temp231; void* _temp233; struct Cyc_Dict_T* _temp235;
struct Cyc_Dict_T* _temp237; void* _temp239; struct Cyc_Dict_T* _temp226= t;
_temp228=* _temp226; _LL240: _temp239=( void*) _temp228.color; goto _LL238;
_LL238: _temp237= _temp228.left; goto _LL236; _LL236: _temp235= _temp228.right;
goto _LL230; _LL230: _temp229= _temp228.key_val; _LL234: _temp233= _temp229.f1;
goto _LL232; _LL232: _temp231= _temp229.f2; goto _LL227; _LL227: if( _temp237 !=
0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null( _temp237));} f(
env, _temp233, _temp231); if( _temp235 != 0){(( void(*)( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct
Cyc_Dict_T*) _check_null( _temp235));}} void Cyc_Dict_app_c( void*(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( d->t));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp243;
struct _tuple0 _temp244; void* _temp246; void* _temp248; struct Cyc_Dict_T*
_temp250; struct Cyc_Dict_T* _temp252; void* _temp254; struct Cyc_Dict_T*
_temp241= t; _temp243=* _temp241; _LL255: _temp254=( void*) _temp243.color; goto
_LL253; _LL253: _temp252= _temp243.left; goto _LL251; _LL251: _temp250= _temp243.right;
goto _LL245; _LL245: _temp244= _temp243.key_val; _LL249: _temp248= _temp244.f1;
goto _LL247; _LL247: _temp246= _temp244.f2; goto _LL242; _LL242: if( _temp252 !=
0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp252));} f( _temp248, _temp246); if(
_temp250 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*) _check_null( _temp250));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)
_check_null( d->t));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp258; struct
_tuple0 _temp259; void* _temp261; void* _temp263; struct Cyc_Dict_T* _temp265;
struct Cyc_Dict_T* _temp267; void* _temp269; struct Cyc_Dict_T* _temp256= t;
_temp258=* _temp256; _LL270: _temp269=( void*) _temp258.color; goto _LL268;
_LL268: _temp267= _temp258.left; goto _LL266; _LL266: _temp265= _temp258.right;
goto _LL260; _LL260: _temp259= _temp258.key_val; _LL264: _temp263= _temp259.f1;
goto _LL262; _LL262: _temp261= _temp259.f2; goto _LL257; _LL257: if( _temp267 !=
0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*) _check_null( _temp267));} f(
env, _temp263, _temp261); if( _temp265 != 0){(( void(*)( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct
Cyc_Dict_T*) _check_null( _temp265));}} void Cyc_Dict_iter_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( d->t));}} struct _tuple2{ void(* f1)(
void*, void*); struct Cyc_Dict_Dict* f2; } ; static void Cyc_Dict_iter2_f(
struct _tuple2* env, void* a, void* b1){ struct _tuple2 _temp273; struct Cyc_Dict_Dict*
_temp274; void(* _temp276)( void*, void*); struct _tuple2* _temp271= env;
_temp273=* _temp271; _LL277: _temp276= _temp273.f1; goto _LL275; _LL275:
_temp274= _temp273.f2; goto _LL272; _LL272: _temp276( b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp274, a));} void Cyc_Dict_iter2( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple2 _temp278=({ struct _tuple2 _temp279; _temp279.f1= f; _temp279.f2= d2;
_temp279;});(( void(*)( void(* f)( struct _tuple2*, void*, void*), struct
_tuple2* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
_tuple2* env, void* a, void* b1)) Cyc_Dict_iter2_f,& _temp278, d1);} struct
_tuple3{ void(* f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void* f3; }
; static void Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){
struct _tuple3 _temp282; void* _temp283; struct Cyc_Dict_Dict* _temp285; void(*
_temp287)( void*, void*, void*); struct _tuple3* _temp280= env; _temp282=*
_temp280; _LL288: _temp287= _temp282.f1; goto _LL286; _LL286: _temp285= _temp282.f2;
goto _LL284; _LL284: _temp283= _temp282.f3; goto _LL281; _LL281: _temp287(
_temp283, b1,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp285, a));} void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void*
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3
_temp289=({ struct _tuple3 _temp290; _temp290.f1= f; _temp290.f2= d2; _temp290.f3=
inner_env; _temp290;});(( void(*)( void(* f)( struct _tuple3*, void*, void*),
struct _tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple3* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,& _temp289, d1);}
struct _tuple4{ void*(* f1)( void*, void*, void*, void*, void*); struct Cyc_Dict_Dict*
f2; void* f3; } ; static void* Cyc_Dict_fold2_c_f( struct _tuple4* env, void* a,
void* b1, void* accum){ struct _tuple4 _temp293; void* _temp294; struct Cyc_Dict_Dict*
_temp296; void*(* _temp298)( void*, void*, void*, void*, void*); struct _tuple4*
_temp291= env; _temp293=* _temp291; _LL299: _temp298= _temp293.f1; goto _LL297;
_LL297: _temp296= _temp293.f2; goto _LL295; _LL295: _temp294= _temp293.f3; goto
_LL292; _LL292: return _temp298( _temp294, a, b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp296, a), accum);} void* Cyc_Dict_fold2_c(
void*(* f)( void*, void*, void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2, void* accum){ struct _tuple4 _temp300=({ struct
_tuple4 _temp301; _temp301.f1= f; _temp301.f2= d2; _temp301.f3= inner_env;
_temp301;}); return(( void*(*)( void*(* f)( struct _tuple4*, void*, void*, void*),
struct _tuple4* env, struct Cyc_Dict_Dict* d, void* accum)) Cyc_Dict_fold_c)((
void*(*)( struct _tuple4* env, void* a, void* b1, void* accum)) Cyc_Dict_fold2_c_f,&
_temp300, d1, accum);} static struct Cyc_Dict_T* Cyc_Dict_copy_tree( struct
_RegionHandle* r2, struct Cyc_Dict_T* t){ if( t == 0){ return 0;} else{ struct
_tuple0 _temp304; struct Cyc_Dict_T* _temp306; struct Cyc_Dict_T* _temp308; void*
_temp310; struct Cyc_Dict_T _temp302=*(( struct Cyc_Dict_T*) _check_null( t));
_LL311: _temp310=( void*) _temp302.color; goto _LL309; _LL309: _temp308=
_temp302.left; goto _LL307; _LL307: _temp306= _temp302.right; goto _LL305;
_LL305: _temp304= _temp302.key_val; goto _LL303; _LL303: { struct Cyc_Dict_T*
_temp312=(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r2, struct Cyc_Dict_T*
t)) Cyc_Dict_copy_tree)( r2, _temp308); struct Cyc_Dict_T* _temp313=(( struct
Cyc_Dict_T*(*)( struct _RegionHandle* r2, struct Cyc_Dict_T* t)) Cyc_Dict_copy_tree)(
r2, _temp306); return({ struct Cyc_Dict_T* _temp314=( struct Cyc_Dict_T*)
_region_malloc( r2, sizeof( struct Cyc_Dict_T)); _temp314->color=( void*)
_temp310; _temp314->left= _temp312; _temp314->right= _temp313; _temp314->key_val=
_temp304; _temp314;});}}} struct Cyc_Dict_Dict* Cyc_Dict_rcopy( struct
_RegionHandle* r2, struct Cyc_Dict_Dict* d){ return({ struct Cyc_Dict_Dict*
_temp315=( struct Cyc_Dict_Dict*) _region_malloc( r2, sizeof( struct Cyc_Dict_Dict));
_temp315->rel= d->rel; _temp315->r= r2; _temp315->t=(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r2, struct Cyc_Dict_T* t)) Cyc_Dict_copy_tree)( r2, d->t);
_temp315;});} struct Cyc_Dict_Dict* Cyc_Dict_copy( struct Cyc_Dict_Dict* d){
return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rcopy)( Cyc_Core_heap_region, d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree(
struct _RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp318; struct _tuple0 _temp319; void* _temp321; void* _temp323; struct Cyc_Dict_T*
_temp325; struct Cyc_Dict_T* _temp327; void* _temp329; struct Cyc_Dict_T*
_temp316= t; _temp318=* _temp316; _LL330: _temp329=( void*) _temp318.color; goto
_LL328; _LL328: _temp327= _temp318.left; goto _LL326; _LL326: _temp325= _temp318.right;
goto _LL320; _LL320: _temp319= _temp318.key_val; _LL324: _temp323= _temp319.f1;
goto _LL322; _LL322: _temp321= _temp319.f2; goto _LL317; _LL317: { struct Cyc_Dict_T*
_temp331= _temp327 == 0? 0:(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*)
_check_null( _temp327)); void* _temp332= f( _temp321); struct Cyc_Dict_T*
_temp333= _temp325 == 0? 0:(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*)
_check_null( _temp325)); return({ struct Cyc_Dict_T* _temp334=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp334->color=( void*)
_temp329; _temp334->left= _temp331; _temp334->right= _temp333; _temp334->key_val=({
struct _tuple0 _temp335; _temp335.f1= _temp323; _temp335.f2= _temp332; _temp335;});
_temp334;});}} struct Cyc_Dict_Dict* Cyc_Dict_rmap( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct
Cyc_Dict_Dict* _temp336=( struct Cyc_Dict_Dict*) _region_malloc( r, sizeof(
struct Cyc_Dict_Dict)); _temp336->rel= d->rel; _temp336->r= r; _temp336->t= 0;
_temp336;});} return({ struct Cyc_Dict_Dict* _temp337=( struct Cyc_Dict_Dict*)
_region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp337->rel= d->rel;
_temp337->r= r; _temp337->t=(( struct Cyc_Dict_T*(*)( struct _RegionHandle* r,
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( r, f,( struct Cyc_Dict_T*)
_check_null( d->t)); _temp337;});} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)(
void*), struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, void*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_rmap)(
Cyc_Core_heap_region, f, d);} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c(
struct _RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T _temp340; struct _tuple0 _temp341; void* _temp343; void*
_temp345; struct Cyc_Dict_T* _temp347; struct Cyc_Dict_T* _temp349; void*
_temp351; struct Cyc_Dict_T* _temp338= t; _temp340=* _temp338; _LL352: _temp351=(
void*) _temp340.color; goto _LL350; _LL350: _temp349= _temp340.left; goto _LL348;
_LL348: _temp347= _temp340.right; goto _LL342; _LL342: _temp341= _temp340.key_val;
_LL346: _temp345= _temp341.f1; goto _LL344; _LL344: _temp343= _temp341.f2; goto
_LL339; _LL339: { struct Cyc_Dict_T* _temp353= _temp349 == 0? 0:(( struct Cyc_Dict_T*(*)(
struct _RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*) _check_null( _temp349));
void* _temp354= f( env, _temp343); struct Cyc_Dict_T* _temp355= _temp347 == 0? 0:((
struct Cyc_Dict_T*(*)( struct _RegionHandle* r, void*(* f)( void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*)
_check_null( _temp347)); return({ struct Cyc_Dict_T* _temp356=( struct Cyc_Dict_T*)
_region_malloc( r, sizeof( struct Cyc_Dict_T)); _temp356->color=( void*)
_temp351; _temp356->left= _temp353; _temp356->right= _temp355; _temp356->key_val=({
struct _tuple0 _temp357; _temp357.f1= _temp345; _temp357.f2= _temp354; _temp357;});
_temp356;});}} struct Cyc_Dict_Dict* Cyc_Dict_rmap_c( struct _RegionHandle* r,
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){
return({ struct Cyc_Dict_Dict* _temp358=( struct Cyc_Dict_Dict*) _region_malloc(
r, sizeof( struct Cyc_Dict_Dict)); _temp358->rel= d->rel; _temp358->r= r;
_temp358->t= 0; _temp358;});} return({ struct Cyc_Dict_Dict* _temp359=( struct
Cyc_Dict_Dict*) _region_malloc( r, sizeof( struct Cyc_Dict_Dict)); _temp359->rel=
d->rel; _temp359->r= r; _temp359->t=(( struct Cyc_Dict_T*(*)( struct
_RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_map_tree_c)( r, f, env,( struct Cyc_Dict_T*) _check_null( d->t));
_temp359;});} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_rmap_c)( Cyc_Core_heap_region, f, env, d);} struct _tuple0* Cyc_Dict_rchoose(
struct _RegionHandle* r, struct Cyc_Dict_Dict* d){ if( d->t == 0){( int) _throw((
void*) Cyc_Dict_Absent);} return({ struct _tuple0* _temp360=( struct _tuple0*)
_region_malloc( r, sizeof( struct _tuple0)); _temp360->f1=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f1; _temp360->f2=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f2; _temp360;});} static int Cyc_Dict_forall_tree_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp363; struct _tuple0 _temp364; void* _temp366; void* _temp368; struct Cyc_Dict_T*
_temp370; struct Cyc_Dict_T* _temp372; void* _temp374; struct Cyc_Dict_T*
_temp361= t; _temp363=* _temp361; _LL375: _temp374=( void*) _temp363.color; goto
_LL373; _LL373: _temp372= _temp363.left; goto _LL371; _LL371: _temp370= _temp363.right;
goto _LL365; _LL365: _temp364= _temp363.key_val; _LL369: _temp368= _temp364.f1;
goto _LL367; _LL367: _temp366= _temp364.f2; goto _LL362; _LL362: return((
_temp372 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)
_check_null( _temp372)))? f( env, _temp368, _temp366): 0)? _temp370 == 0? 1:((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*) _check_null( _temp370)): 0;} int Cyc_Dict_forall_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t ==
0){ return 1;} return(( int(*)( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)
_check_null( d->t));} struct _tuple5{ int(* f1)( void*, void*, void*); struct
Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f( struct _tuple5*
env, void* a, void* b){ struct _tuple5 _temp378; struct Cyc_Dict_Dict* _temp379;
int(* _temp381)( void*, void*, void*); struct _tuple5* _temp376= env; _temp378=*
_temp376; _LL382: _temp381= _temp378.f1; goto _LL380; _LL380: _temp379= _temp378.f2;
goto _LL377; _LL377: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp379, a)){ return _temp381( a, b,(( void*(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_lookup)( _temp379, a));} return 1;} int Cyc_Dict_forall_intersect(
int(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ struct _tuple5 _temp383=({ struct _tuple5 _temp384; _temp384.f1= f;
_temp384.f2= d2; _temp384;}); return(( int(*)( int(* f)( struct _tuple5*, void*,
void*), struct _tuple5* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)(
struct _tuple5* env, void* a, void* b)) Cyc_Dict_forall_intersect_f,& _temp383,
d1);} static struct Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*, void*),
void* a, void* b, struct Cyc_Dict_Dict* d1){ if((( int(*)( struct Cyc_Dict_Dict*
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
d1)) Cyc_Dict_union_f, f, d1, d2);} struct _tuple6{ void*(* f1)( void*, void*);
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; } ; static void Cyc_Dict_intersect_f(
struct _tuple6* trip, void* a, void* b){ struct _tuple6 _temp387; struct Cyc_Dict_Dict*
_temp388; struct Cyc_Dict_Dict** _temp390; struct Cyc_Dict_Dict* _temp391; void*(*
_temp393)( void*, void*); struct _tuple6* _temp385= trip; _temp387=* _temp385;
_LL394: _temp393= _temp387.f1; goto _LL392; _LL392: _temp391= _temp387.f2; goto
_LL389; _LL389: _temp388= _temp387.f3; _temp390=&(* _temp385).f3; goto _LL386;
_LL386: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp391, a)){* _temp390=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)(* _temp390, a, _temp393((( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp391, a), b));}}
struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct _tuple6
_temp395=({ struct _tuple6 _temp396; _temp396.f1= f; _temp396.f2= d1; _temp396.f3=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r, int(* comp)( void*, void*)))
Cyc_Dict_rempty)( d1->r, d1->rel); _temp396;});(( void(*)( void(* f)( struct
_tuple6*, void*, void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple6* trip, void* a, void* b)) Cyc_Dict_intersect_f,&
_temp395, d2); return _temp395.f3;}} struct _tuple7{ void*(* f1)( void*, void*,
void*); void* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static
void Cyc_Dict_intersect_c_f( struct _tuple7* quad_t, void* a, void* b){ struct
_tuple7 _temp399; struct Cyc_Dict_Dict* _temp400; struct Cyc_Dict_Dict**
_temp402; struct Cyc_Dict_Dict* _temp403; void* _temp405; void*(* _temp407)(
void*, void*, void*); struct _tuple7* _temp397= quad_t; _temp399=* _temp397;
_LL408: _temp407= _temp399.f1; goto _LL406; _LL406: _temp405= _temp399.f2; goto
_LL404; _LL404: _temp403= _temp399.f3; goto _LL401; _LL401: _temp400= _temp399.f4;
_temp402=&(* _temp397).f4; goto _LL398; _LL398: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp403, a)){* _temp402=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp402, a,
_temp407( _temp405,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp403, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ if( d1 == d2){ return d1;}{ struct _tuple7 _temp409=({ struct _tuple7
_temp410; _temp410.f1= f; _temp410.f2= env; _temp410.f3= d1; _temp410.f4=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r, int(* comp)( void*, void*)))
Cyc_Dict_rempty)( d1->r, d1->rel); _temp410;});(( void(*)( void(* f)( struct
_tuple7*, void*, void*), struct _tuple7* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple7* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,&
_temp409, d2); return _temp409.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f(
struct _RegionHandle* r, void* k, void* v, struct Cyc_List_List* accum){ return({
struct Cyc_List_List* _temp411=( struct Cyc_List_List*) _region_malloc( r,
sizeof( struct Cyc_List_List)); _temp411->hd=( void*)({ struct _tuple0* _temp412=(
struct _tuple0*) _region_malloc( r, sizeof( struct _tuple0)); _temp412->f1= k;
_temp412->f2= v; _temp412;}); _temp411->tl= accum; _temp411;});} struct Cyc_List_List*
Cyc_Dict_rto_list( struct _RegionHandle* r, struct Cyc_Dict_Dict* d){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _RegionHandle*,
void*, void*, struct Cyc_List_List*), struct _RegionHandle* env, struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold_c)(( struct Cyc_List_List*(*)(
struct _RegionHandle* r, void* k, void* v, struct Cyc_List_List* accum)) Cyc_Dict_to_list_f,
r, d, 0);} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
return(( struct Cyc_List_List*(*)( struct _RegionHandle* r, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rto_list)( Cyc_Core_heap_region, d);} struct _tuple8{ int(* f1)(
void*, void*); struct _RegionHandle* f2; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_f(
struct _tuple8* env, void* x, void* y, struct Cyc_Dict_Dict* acc){ struct
_tuple8 _temp415; struct _RegionHandle* _temp416; int(* _temp418)( void*, void*);
struct _tuple8* _temp413= env; _temp415=* _temp413; _LL419: _temp418= _temp415.f1;
goto _LL417; _LL417: _temp416= _temp415.f2; goto _LL414; _LL414: return _temp418(
x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter(
struct _RegionHandle* r2, int(* f)( void*, void*), struct Cyc_Dict_Dict* d){
struct _tuple8 _temp420=({ struct _tuple8 _temp421; _temp421.f1= f; _temp421.f2=
r2; _temp421;}); return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)(
struct _tuple8*, void*, void*, struct Cyc_Dict_Dict*), struct _tuple8* env,
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( struct _tuple8* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_f,& _temp420, d,(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r, int(* comp)( void*, void*))) Cyc_Dict_rempty)( r2, d->rel));}
struct Cyc_Dict_Dict* Cyc_Dict_filter( int(* f)( void*, void*), struct Cyc_Dict_Dict*
d){ return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, int(* f)( void*,
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter)( Cyc_Core_heap_region, f, d);}
struct _tuple9{ int(* f1)( void*, void*, void*); void* f2; struct _RegionHandle*
f3; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_c_f( struct _tuple9* env,
void* x, void* y, struct Cyc_Dict_Dict* acc){ struct _tuple9 _temp424; struct
_RegionHandle* _temp425; void* _temp427; int(* _temp429)( void*, void*, void*);
struct _tuple9* _temp422= env; _temp424=* _temp422; _LL430: _temp429= _temp424.f1;
goto _LL428; _LL428: _temp427= _temp424.f2; goto _LL426; _LL426: _temp425=
_temp424.f3; goto _LL423; _LL423: return _temp429( _temp427, x, y)?(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( acc, x, y):
acc;} struct Cyc_Dict_Dict* Cyc_Dict_rfilter_c( struct _RegionHandle* r2, int(*
f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct _tuple9
_temp431=({ struct _tuple9 _temp432; _temp432.f1= f; _temp432.f2= f_env;
_temp432.f3= r2; _temp432;}); return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(*
f)( struct _tuple9*, void*, void*, struct Cyc_Dict_Dict*), struct _tuple9* env,
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( struct _tuple9* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_c_f,& _temp431, d,(( struct Cyc_Dict_Dict*(*)( struct
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
Cyc_Dict_rdifference)( Cyc_Core_heap_region, d1, d2);} struct _tuple10{ int(* f1)(
void*, void*); void* f2; } ; static int Cyc_Dict_delete_f( struct _tuple10* env,
void* x, void* y){ struct _tuple10 _temp435; void* _temp436; int(* _temp438)(
void*, void*); struct _tuple10* _temp433= env; _temp435=* _temp433; _LL439:
_temp438= _temp435.f1; goto _LL437; _LL437: _temp436= _temp435.f2; goto _LL434;
_LL434: return _temp438( _temp436, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_rdelete(
struct _RegionHandle* r2, struct Cyc_Dict_Dict* d, void* x){ if( !(( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d, x)){ return(( struct
Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, struct Cyc_Dict_Dict* d)) Cyc_Dict_rcopy)(
r2, d);}{ struct _tuple10 _temp440=({ struct _tuple10 _temp441; _temp441.f1= d->rel;
_temp441.f2= x; _temp441;}); return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r2, int(* f)( struct _tuple10*, void*, void*), struct _tuple10*
f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_rfilter_c)( r2,( int(*)( struct
_tuple10* env, void* x, void* y)) Cyc_Dict_delete_f,& _temp440, d);}} struct Cyc_Dict_Dict*
Cyc_Dict_rdelete_same( struct Cyc_Dict_Dict* d, void* x){ if( !(( int(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d, x)){ return d;}{ struct
_tuple10 _temp442=({ struct _tuple10 _temp443; _temp443.f1= d->rel; _temp443.f2=
x; _temp443;}); return(( struct Cyc_Dict_Dict*(*)( struct _RegionHandle* r2, int(*
f)( struct _tuple10*, void*, void*), struct _tuple10* f_env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_rfilter_c)( d->r,( int(*)( struct _tuple10* env, void* x, void* y))
Cyc_Dict_delete_f,& _temp442, d);}} struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict* d, void* x){ return(( struct Cyc_Dict_Dict*(*)( struct
_RegionHandle* r2, struct Cyc_Dict_Dict* d, void* x)) Cyc_Dict_rdelete)( Cyc_Core_heap_region,
d, x);}