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
char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
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
void*), void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter2( void(*
f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(*
f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict* Cyc_Dict_intersect(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
extern struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); struct _tuple0{ void* f1; void* f2;
} ; extern struct _tuple0* Cyc_Dict_choose( struct Cyc_Dict_Dict* d); extern
struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter( int(* f)(
void*, void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_delete( struct Cyc_Dict_Dict*, void*); char Cyc_Dict_Absent[ 11u]; char
Cyc_Dict_Present[ 12u]; static const int Cyc_Dict_R= 0; static const int Cyc_Dict_B=
1; struct Cyc_Dict_T{ void* color; struct Cyc_Dict_T* left; struct Cyc_Dict_T*
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
struct Cyc_Dict_Dict* d, void* key){ int(* _temp4)( void*, void*)= d->rel; goto
_LL5; _LL5: { struct Cyc_Dict_T* _temp6= d->t; goto _LL7; _LL7: while( _temp6 !=
0) { int _temp8= _temp4( key,((( struct Cyc_Dict_T*) _check_null( _temp6))->key_val).f1);
goto _LL9; _LL9: if( _temp8 < 0){ _temp6=(( struct Cyc_Dict_T*) _check_null(
_temp6))->left;} else{ if( _temp8 > 0){ _temp6=(( struct Cyc_Dict_T*)
_check_null( _temp6))->right;} else{ return 1;}}} return 0;}} void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key){ int(* _temp10)( void*, void*)= d->rel; goto
_LL11; _LL11: { struct Cyc_Dict_T* _temp12= d->t; goto _LL13; _LL13: while(
_temp12 != 0) { int _temp14= _temp10( key,((( struct Cyc_Dict_T*) _check_null(
_temp12))->key_val).f1); goto _LL15; _LL15: if( _temp14 < 0){ _temp12=(( struct
Cyc_Dict_T*) _check_null( _temp12))->left;} else{ if( _temp14 > 0){ _temp12=((
struct Cyc_Dict_T*) _check_null( _temp12))->right;} else{ return((( struct Cyc_Dict_T*)
_check_null( _temp12))->key_val).f2;}}}( void) _throw(( void*) Cyc_Dict_Absent);}}
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key){
int(* _temp16)( void*, void*)= d->rel; goto _LL17; _LL17: { struct Cyc_Dict_T*
_temp18= d->t; goto _LL19; _LL19: while( _temp18 != 0) { int _temp20= _temp16(
key,((( struct Cyc_Dict_T*) _check_null( _temp18))->key_val).f1); goto _LL21;
_LL21: if( _temp20 < 0){ _temp18=(( struct Cyc_Dict_T*) _check_null( _temp18))->left;}
else{ if( _temp20 > 0){ _temp18=(( struct Cyc_Dict_T*) _check_null( _temp18))->right;}
else{ return({ struct Cyc_Core_Opt* _temp22=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp22->v=( void*)((( struct Cyc_Dict_T*)
_check_null( _temp18))->key_val).f2; _temp22;});}}} return 0;}} int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict* d, void* key, void** ans_place){ int(* _temp23)( void*,
void*)= d->rel; goto _LL24; _LL24: { struct Cyc_Dict_T* _temp25= d->t; goto
_LL26; _LL26: while( _temp25 != 0) { int _temp27= _temp23( key,((( struct Cyc_Dict_T*)
_check_null( _temp25))->key_val).f1); goto _LL28; _LL28: if( _temp27 < 0){
_temp25=(( struct Cyc_Dict_T*) _check_null( _temp25))->left;} else{ if( _temp27
> 0){ _temp25=(( struct Cyc_Dict_T*) _check_null( _temp25))->right;} else{*
ans_place=((( struct Cyc_Dict_T*) _check_null( _temp25))->key_val).f2; return 1;}}}
return 0;}} struct _tuple1{ void* f1; struct Cyc_Dict_T* f2; struct Cyc_Dict_T*
f3; struct _tuple0 f4; } ; static struct Cyc_Dict_T* Cyc_Dict_balance( struct
_tuple1 quad_t){ struct _tuple1 _temp29= quad_t; struct _tuple0 _temp41; struct
Cyc_Dict_T* _temp43; struct Cyc_Dict_T* _temp45; struct Cyc_Dict_T _temp47;
struct _tuple0 _temp48; struct Cyc_Dict_T* _temp50; struct Cyc_Dict_T* _temp52;
struct Cyc_Dict_T _temp54; struct _tuple0 _temp55; struct Cyc_Dict_T* _temp57;
struct Cyc_Dict_T* _temp59; void* _temp61; void* _temp63; void* _temp65; struct
_tuple0 _temp67; struct Cyc_Dict_T* _temp69; struct Cyc_Dict_T* _temp71; struct
Cyc_Dict_T _temp73; struct _tuple0 _temp74; struct Cyc_Dict_T* _temp76; struct
Cyc_Dict_T _temp78; struct _tuple0 _temp79; struct Cyc_Dict_T* _temp81; struct
Cyc_Dict_T* _temp83; void* _temp85; struct Cyc_Dict_T* _temp87; void* _temp89;
void* _temp91; struct _tuple0 _temp93; struct Cyc_Dict_T* _temp95; struct Cyc_Dict_T
_temp97; struct _tuple0 _temp98; struct Cyc_Dict_T* _temp100; struct Cyc_Dict_T*
_temp102; struct Cyc_Dict_T _temp104; struct _tuple0 _temp105; struct Cyc_Dict_T*
_temp107; struct Cyc_Dict_T* _temp109; void* _temp111; void* _temp113; struct
Cyc_Dict_T* _temp115; void* _temp117; struct _tuple0 _temp119; struct Cyc_Dict_T*
_temp121; struct Cyc_Dict_T _temp123; struct _tuple0 _temp124; struct Cyc_Dict_T*
_temp126; struct Cyc_Dict_T _temp128; struct _tuple0 _temp129; struct Cyc_Dict_T*
_temp131; struct Cyc_Dict_T* _temp133; void* _temp135; struct Cyc_Dict_T*
_temp137; void* _temp139; struct Cyc_Dict_T* _temp141; void* _temp143; struct
_tuple0 _temp145; struct Cyc_Dict_T* _temp147; struct Cyc_Dict_T* _temp149; void*
_temp151; _LL31: _LL66: _temp65= _temp29.f1; if( _temp65 ==( void*) Cyc_Dict_B){
goto _LL46;} else{ goto _LL33;} _LL46: _temp45= _temp29.f2; if( _temp45 == 0){
goto _LL33;} else{ _temp47=* _temp45; _LL64: _temp63=( void*) _temp47.color; if(
_temp63 ==( void*) Cyc_Dict_R){ goto _LL53;} else{ goto _LL33;} _LL53: _temp52=(
struct Cyc_Dict_T*) _temp47.left; if( _temp52 == 0){ goto _LL33;} else{ _temp54=*
_temp52; _LL62: _temp61=( void*) _temp54.color; if( _temp61 ==( void*) Cyc_Dict_R){
goto _LL60;} else{ goto _LL33;} _LL60: _temp59=( struct Cyc_Dict_T*) _temp54.left;
goto _LL58; _LL58: _temp57=( struct Cyc_Dict_T*) _temp54.right; goto _LL56;
_LL56: _temp55=( struct _tuple0) _temp54.key_val; goto _LL51;} _LL51: _temp50=(
struct Cyc_Dict_T*) _temp47.right; goto _LL49; _LL49: _temp48=( struct _tuple0)
_temp47.key_val; goto _LL44;} _LL44: _temp43= _temp29.f3; goto _LL42; _LL42:
_temp41= _temp29.f4; goto _LL32; _LL33: _LL92: _temp91= _temp29.f1; if( _temp91
==( void*) Cyc_Dict_B){ goto _LL72;} else{ goto _LL35;} _LL72: _temp71= _temp29.f2;
if( _temp71 == 0){ goto _LL35;} else{ _temp73=* _temp71; _LL90: _temp89=( void*)
_temp73.color; if( _temp89 ==( void*) Cyc_Dict_R){ goto _LL88;} else{ goto _LL35;}
_LL88: _temp87=( struct Cyc_Dict_T*) _temp73.left; goto _LL77; _LL77: _temp76=(
struct Cyc_Dict_T*) _temp73.right; if( _temp76 == 0){ goto _LL35;} else{ _temp78=*
_temp76; _LL86: _temp85=( void*) _temp78.color; if( _temp85 ==( void*) Cyc_Dict_R){
goto _LL84;} else{ goto _LL35;} _LL84: _temp83=( struct Cyc_Dict_T*) _temp78.left;
goto _LL82; _LL82: _temp81=( struct Cyc_Dict_T*) _temp78.right; goto _LL80;
_LL80: _temp79=( struct _tuple0) _temp78.key_val; goto _LL75;} _LL75: _temp74=(
struct _tuple0) _temp73.key_val; goto _LL70;} _LL70: _temp69= _temp29.f3; goto
_LL68; _LL68: _temp67= _temp29.f4; goto _LL34; _LL35: _LL118: _temp117= _temp29.f1;
if( _temp117 ==( void*) Cyc_Dict_B){ goto _LL116;} else{ goto _LL37;} _LL116:
_temp115= _temp29.f2; goto _LL96; _LL96: _temp95= _temp29.f3; if( _temp95 == 0){
goto _LL37;} else{ _temp97=* _temp95; _LL114: _temp113=( void*) _temp97.color;
if( _temp113 ==( void*) Cyc_Dict_R){ goto _LL103;} else{ goto _LL37;} _LL103:
_temp102=( struct Cyc_Dict_T*) _temp97.left; if( _temp102 == 0){ goto _LL37;}
else{ _temp104=* _temp102; _LL112: _temp111=( void*) _temp104.color; if(
_temp111 ==( void*) Cyc_Dict_R){ goto _LL110;} else{ goto _LL37;} _LL110:
_temp109=( struct Cyc_Dict_T*) _temp104.left; goto _LL108; _LL108: _temp107=(
struct Cyc_Dict_T*) _temp104.right; goto _LL106; _LL106: _temp105=( struct
_tuple0) _temp104.key_val; goto _LL101;} _LL101: _temp100=( struct Cyc_Dict_T*)
_temp97.right; goto _LL99; _LL99: _temp98=( struct _tuple0) _temp97.key_val;
goto _LL94;} _LL94: _temp93= _temp29.f4; goto _LL36; _LL37: _LL144: _temp143=
_temp29.f1; if( _temp143 ==( void*) Cyc_Dict_B){ goto _LL142;} else{ goto _LL39;}
_LL142: _temp141= _temp29.f2; goto _LL122; _LL122: _temp121= _temp29.f3; if(
_temp121 == 0){ goto _LL39;} else{ _temp123=* _temp121; _LL140: _temp139=( void*)
_temp123.color; if( _temp139 ==( void*) Cyc_Dict_R){ goto _LL138;} else{ goto
_LL39;} _LL138: _temp137=( struct Cyc_Dict_T*) _temp123.left; goto _LL127;
_LL127: _temp126=( struct Cyc_Dict_T*) _temp123.right; if( _temp126 == 0){ goto
_LL39;} else{ _temp128=* _temp126; _LL136: _temp135=( void*) _temp128.color; if(
_temp135 ==( void*) Cyc_Dict_R){ goto _LL134;} else{ goto _LL39;} _LL134:
_temp133=( struct Cyc_Dict_T*) _temp128.left; goto _LL132; _LL132: _temp131=(
struct Cyc_Dict_T*) _temp128.right; goto _LL130; _LL130: _temp129=( struct
_tuple0) _temp128.key_val; goto _LL125;} _LL125: _temp124=( struct _tuple0)
_temp123.key_val; goto _LL120;} _LL120: _temp119= _temp29.f4; goto _LL38; _LL39:
_LL152: _temp151= _temp29.f1; goto _LL150; _LL150: _temp149= _temp29.f2; goto
_LL148; _LL148: _temp147= _temp29.f3; goto _LL146; _LL146: _temp145= _temp29.f4;
goto _LL40; _LL32: return({ struct Cyc_Dict_T* _temp153=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp153->color=( void*)(( void*) Cyc_Dict_R);
_temp153->left=({ struct Cyc_Dict_T* _temp155=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp155->color=( void*)(( void*) Cyc_Dict_B);
_temp155->left= _temp59; _temp155->right= _temp57; _temp155->key_val= _temp55;
_temp155;}); _temp153->right=({ struct Cyc_Dict_T* _temp154=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp154->color=( void*)(( void*) Cyc_Dict_B);
_temp154->left= _temp50; _temp154->right= _temp43; _temp154->key_val= _temp41;
_temp154;}); _temp153->key_val= _temp48; _temp153;}); _LL34: return({ struct Cyc_Dict_T*
_temp156=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp156->color=(
void*)(( void*) Cyc_Dict_R); _temp156->left=({ struct Cyc_Dict_T* _temp158=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp158->color=(
void*)(( void*) Cyc_Dict_B); _temp158->left= _temp87; _temp158->right= _temp83;
_temp158->key_val= _temp74; _temp158;}); _temp156->right=({ struct Cyc_Dict_T*
_temp157=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp157->color=(
void*)(( void*) Cyc_Dict_B); _temp157->left= _temp81; _temp157->right= _temp69;
_temp157->key_val= _temp67; _temp157;}); _temp156->key_val= _temp79; _temp156;});
_LL36: return({ struct Cyc_Dict_T* _temp159=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp159->color=( void*)(( void*) Cyc_Dict_R);
_temp159->left=({ struct Cyc_Dict_T* _temp161=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp161->color=( void*)(( void*) Cyc_Dict_B);
_temp161->left= _temp115; _temp161->right= _temp109; _temp161->key_val= _temp93;
_temp161;}); _temp159->right=({ struct Cyc_Dict_T* _temp160=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp160->color=( void*)(( void*) Cyc_Dict_B);
_temp160->left= _temp107; _temp160->right= _temp100; _temp160->key_val= _temp98;
_temp160;}); _temp159->key_val= _temp105; _temp159;}); _LL38: return({ struct
Cyc_Dict_T* _temp162=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp162->color=( void*)(( void*) Cyc_Dict_R); _temp162->left=({ struct Cyc_Dict_T*
_temp164=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp164->color=(
void*)(( void*) Cyc_Dict_B); _temp164->left= _temp141; _temp164->right= _temp137;
_temp164->key_val= _temp119; _temp164;}); _temp162->right=({ struct Cyc_Dict_T*
_temp163=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp163->color=(
void*)(( void*) Cyc_Dict_B); _temp163->left= _temp133; _temp163->right= _temp131;
_temp163->key_val= _temp129; _temp163;}); _temp162->key_val= _temp124; _temp162;});
_LL40: return({ struct Cyc_Dict_T* _temp165=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp165->color=( void*) _temp151; _temp165->left=
_temp149; _temp165->right= _temp147; _temp165->key_val= _temp145; _temp165;});
_LL30:;} static struct Cyc_Dict_T* Cyc_Dict_ins( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t){ struct Cyc_Dict_T* _temp166= t;
struct Cyc_Dict_T _temp172; struct _tuple0 _temp173; struct Cyc_Dict_T* _temp175;
struct Cyc_Dict_T* _temp177; void* _temp179; _LL168: if( _temp166 == 0){ goto
_LL169;} else{ goto _LL170;} _LL170: if( _temp166 == 0){ goto _LL167;} else{
_temp172=* _temp166; _LL180: _temp179=( void*) _temp172.color; goto _LL178;
_LL178: _temp177=( struct Cyc_Dict_T*) _temp172.left; goto _LL176; _LL176:
_temp175=( struct Cyc_Dict_T*) _temp172.right; goto _LL174; _LL174: _temp173=(
struct _tuple0) _temp172.key_val; goto _LL171;} _LL169: return({ struct Cyc_Dict_T*
_temp181=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp181->color=(
void*)(( void*) Cyc_Dict_R); _temp181->left= 0; _temp181->right= 0; _temp181->key_val=
key_val; _temp181;}); _LL171: { int _temp182= rel( key_val.f1, _temp173.f1);
goto _LL183; _LL183: if( _temp182 < 0){ return(( struct Cyc_Dict_T*(*)( struct
_tuple1 quad_t)) Cyc_Dict_balance)(({ struct _tuple1 _temp184; _temp184.f1=
_temp179; _temp184.f2=(( struct Cyc_Dict_T*(*)( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val,
_temp177); _temp184.f3= _temp175; _temp184.f4= _temp173; _temp184;}));} else{
if( _temp182 > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t)) Cyc_Dict_balance)(({
struct _tuple1 _temp185; _temp185.f1= _temp179; _temp185.f2= _temp177; _temp185.f3=((
struct Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct
Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val, _temp175); _temp185.f4= _temp173;
_temp185;}));} else{ return({ struct Cyc_Dict_T* _temp186=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp186->color=( void*) _temp179;
_temp186->left= _temp177; _temp186->right= _temp175; _temp186->key_val= key_val;
_temp186;});}}} _LL167:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data){ struct Cyc_Dict_T* _temp188=(( struct Cyc_Dict_T*(*)(
int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
d->rel,({ struct _tuple0 _temp187; _temp187.f1= key; _temp187.f2= data; _temp187;}),
d->t); goto _LL189; _LL189:( void*)((( struct Cyc_Dict_T*) _check_null( _temp188))->color=(
void*)(( void*) Cyc_Dict_B)); return({ struct Cyc_Dict_Dict* _temp190=( struct
Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp190->rel= d->rel;
_temp190->t= _temp188; _temp190;});} struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* key, void* data){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, key)){( void) _throw(( void*) Cyc_Dict_Absent);}
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( d, key, data);} struct Cyc_Dict_Dict* Cyc_Dict_inserts(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=((
struct Cyc_List_List*) _check_null( kds))->tl){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d,((( struct
_tuple0*)(( struct Cyc_List_List*) _check_null( kds))->hd)[
_check_known_subscript_notnull( 1u, 0)]).f1,((( struct _tuple0*)(( struct Cyc_List_List*)
_check_null( kds))->hd)[ _check_known_subscript_notnull( 1u, 0)]).f2);} return d;}
static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum){ struct Cyc_Dict_T _temp193; struct _tuple0 _temp194; void*
_temp196; void* _temp198; struct Cyc_Dict_T* _temp200; struct Cyc_Dict_T*
_temp202; void* _temp204; struct Cyc_Dict_T* _temp191= t; _temp193=* _temp191;
_LL205: _temp204=( void*) _temp193.color; goto _LL203; _LL203: _temp202=( struct
Cyc_Dict_T*) _temp193.left; goto _LL201; _LL201: _temp200=( struct Cyc_Dict_T*)
_temp193.right; goto _LL195; _LL195: _temp194=( struct _tuple0) _temp193.key_val;
_LL199: _temp198= _temp194.f1; goto _LL197; _LL197: _temp196= _temp194.f2; goto
_LL192; _LL192: if( _temp202 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)((
struct Cyc_Dict_T*) _check_null( _temp202)), accum);} accum= f( _temp198,
_temp196, accum); if( _temp200 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)((
struct Cyc_Dict_T*) _check_null( _temp200)), accum);} return accum;} void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t
== 0){ return accum;} return(( void*(*)( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)((
struct Cyc_Dict_T*) _check_null( d->t)), accum);} static void* Cyc_Dict_fold_tree_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void*
accum){ struct Cyc_Dict_T _temp208; struct _tuple0 _temp209; void* _temp211;
void* _temp213; struct Cyc_Dict_T* _temp215; struct Cyc_Dict_T* _temp217; void*
_temp219; struct Cyc_Dict_T* _temp206= t; _temp208=* _temp206; _LL220: _temp219=(
void*) _temp208.color; goto _LL218; _LL218: _temp217=( struct Cyc_Dict_T*)
_temp208.left; goto _LL216; _LL216: _temp215=( struct Cyc_Dict_T*) _temp208.right;
goto _LL210; _LL210: _temp209=( struct _tuple0) _temp208.key_val; _LL214:
_temp213= _temp209.f1; goto _LL212; _LL212: _temp211= _temp209.f2; goto _LL207;
_LL207: if( _temp217 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f,
env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp217)), accum);}
accum= f( env, _temp213, _temp211, accum); if( _temp215 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void*
accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( _temp215)), accum);} return accum;} void* Cyc_Dict_fold_c( void*(*
f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d, void* accum){
if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*, void*, void*,
void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f,
env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( d->t)), accum);}
static void Cyc_Dict_app_tree( void*(* f)( void*, void*), struct Cyc_Dict_T* t){
struct Cyc_Dict_T _temp223; struct _tuple0 _temp224; void* _temp226; void*
_temp228; struct Cyc_Dict_T* _temp230; struct Cyc_Dict_T* _temp232; void*
_temp234; struct Cyc_Dict_T* _temp221= t; _temp223=* _temp221; _LL235: _temp234=(
void*) _temp223.color; goto _LL233; _LL233: _temp232=( struct Cyc_Dict_T*)
_temp223.left; goto _LL231; _LL231: _temp230=( struct Cyc_Dict_T*) _temp223.right;
goto _LL225; _LL225: _temp224=( struct _tuple0) _temp223.key_val; _LL229:
_temp228= _temp224.f1; goto _LL227; _LL227: _temp226= _temp224.f2; goto _LL222;
_LL222: if( _temp232 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( _temp232)));} f( _temp228, _temp226); if( _temp230 != 0){(( void(*)(
void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp230)));}} void Cyc_Dict_app(
void*(* f)( void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( d->t)));}} static void Cyc_Dict_app_tree_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp238; struct _tuple0 _temp239; void* _temp241; void* _temp243; struct Cyc_Dict_T*
_temp245; struct Cyc_Dict_T* _temp247; void* _temp249; struct Cyc_Dict_T*
_temp236= t; _temp238=* _temp236; _LL250: _temp249=( void*) _temp238.color; goto
_LL248; _LL248: _temp247=( struct Cyc_Dict_T*) _temp238.left; goto _LL246;
_LL246: _temp245=( struct Cyc_Dict_T*) _temp238.right; goto _LL240; _LL240:
_temp239=( struct _tuple0) _temp238.key_val; _LL244: _temp243= _temp239.f1; goto
_LL242; _LL242: _temp241= _temp239.f2; goto _LL237; _LL237: if( _temp247 != 0){((
void(*)( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp247)));} f(
env, _temp243, _temp241); if( _temp245 != 0){(( void(*)( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp245)));}} void Cyc_Dict_app_c(
void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t
!= 0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( d->t)));}} static void Cyc_Dict_iter_tree( void(* f)( void*, void*),
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp253; struct _tuple0 _temp254; void*
_temp256; void* _temp258; struct Cyc_Dict_T* _temp260; struct Cyc_Dict_T*
_temp262; void* _temp264; struct Cyc_Dict_T* _temp251= t; _temp253=* _temp251;
_LL265: _temp264=( void*) _temp253.color; goto _LL263; _LL263: _temp262=( struct
Cyc_Dict_T*) _temp253.left; goto _LL261; _LL261: _temp260=( struct Cyc_Dict_T*)
_temp253.right; goto _LL255; _LL255: _temp254=( struct _tuple0) _temp253.key_val;
_LL259: _temp258= _temp254.f1; goto _LL257; _LL257: _temp256= _temp254.f2; goto
_LL252; _LL252: if( _temp262 != 0){(( void(*)( void(* f)( void*, void*), struct
Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( _temp262)));} f( _temp258, _temp256); if( _temp260 != 0){(( void(*)(
void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp260)));}} void Cyc_Dict_iter(
void(* f)( void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)(
void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( d->t)));}} static void Cyc_Dict_iter_tree_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp268; struct _tuple0 _temp269; void* _temp271; void* _temp273; struct Cyc_Dict_T*
_temp275; struct Cyc_Dict_T* _temp277; void* _temp279; struct Cyc_Dict_T*
_temp266= t; _temp268=* _temp266; _LL280: _temp279=( void*) _temp268.color; goto
_LL278; _LL278: _temp277=( struct Cyc_Dict_T*) _temp268.left; goto _LL276;
_LL276: _temp275=( struct Cyc_Dict_T*) _temp268.right; goto _LL270; _LL270:
_temp269=( struct _tuple0) _temp268.key_val; _LL274: _temp273= _temp269.f1; goto
_LL272; _LL272: _temp271= _temp269.f2; goto _LL267; _LL267: if( _temp277 != 0){((
void(*)( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp277)));} f(
env, _temp273, _temp271); if( _temp275 != 0){(( void(*)( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp275)));}} void Cyc_Dict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t
!= 0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( d->t)));}} struct _tuple2{ void(* f1)( void*, void*); struct Cyc_Dict_Dict*
f2; } ; static void Cyc_Dict_iter2_f( struct _tuple2* env, void* a, void* b1){
struct _tuple2 _temp283; struct Cyc_Dict_Dict* _temp284; void(* _temp286)( void*,
void*); struct _tuple2* _temp281= env; _temp283=* _temp281; _LL287: _temp286=
_temp283.f1; goto _LL285; _LL285: _temp284= _temp283.f2; goto _LL282; _LL282:
_temp286( b1,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp284, a));} void Cyc_Dict_iter2( void(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct _tuple2 _temp289=({ struct _tuple2
_temp288; _temp288.f1= f; _temp288.f2= d2; _temp288;}); goto _LL290; _LL290:((
void(*)( void(* f)( struct _tuple2*, void*, void*), struct _tuple2* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple2* env, void* a,
void* b1)) Cyc_Dict_iter2_f,& _temp289, d1);} struct _tuple3{ void(* f1)( void*,
void*, void*); struct Cyc_Dict_Dict* f2; void* f3; } ; static void Cyc_Dict_iter2_c_f(
struct _tuple3* env, void* a, void* b1){ struct _tuple3 _temp293; void* _temp294;
struct Cyc_Dict_Dict* _temp296; void(* _temp298)( void*, void*, void*); struct
_tuple3* _temp291= env; _temp293=* _temp291; _LL299: _temp298= _temp293.f1; goto
_LL297; _LL297: _temp296= _temp293.f2; goto _LL295; _LL295: _temp294= _temp293.f3;
goto _LL292; _LL292: _temp298( _temp294, b1,(( void*(*)( struct Cyc_Dict_Dict* d,
void* key)) Cyc_Dict_lookup)( _temp296, a));} void Cyc_Dict_iter2_c( void(* f)(
void*, void*, void*), void* inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ struct _tuple3 _temp301=({ struct _tuple3 _temp300; _temp300.f1= f;
_temp300.f2= d2; _temp300.f3= inner_env; _temp300;}); goto _LL302; _LL302:((
void(*)( void(* f)( struct _tuple3*, void*, void*), struct _tuple3* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple3* env, void* a,
void* b1)) Cyc_Dict_iter2_c_f,& _temp301, d1);} static struct Cyc_Dict_T* Cyc_Dict_map_tree(
void*(* f)( void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp305; struct
_tuple0 _temp306; void* _temp308; void* _temp310; struct Cyc_Dict_T* _temp312;
struct Cyc_Dict_T* _temp314; void* _temp316; struct Cyc_Dict_T* _temp303= t;
_temp305=* _temp303; _LL317: _temp316=( void*) _temp305.color; goto _LL315;
_LL315: _temp314=( struct Cyc_Dict_T*) _temp305.left; goto _LL313; _LL313:
_temp312=( struct Cyc_Dict_T*) _temp305.right; goto _LL307; _LL307: _temp306=(
struct _tuple0) _temp305.key_val; _LL311: _temp310= _temp306.f1; goto _LL309;
_LL309: _temp308= _temp306.f2; goto _LL304; _LL304: { struct Cyc_Dict_T*
_temp318= _temp314 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct
Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( _temp314))); goto _LL319; _LL319: { void* _temp320= f( _temp308);
goto _LL321; _LL321: { struct Cyc_Dict_T* _temp322= _temp312 == 0? 0:(( struct
Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,(
struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp312))); goto _LL323;
_LL323: return({ struct Cyc_Dict_T* _temp324=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp324->color=( void*) _temp316; _temp324->left=
_temp318; _temp324->right= _temp322; _temp324->key_val=({ struct _tuple0
_temp325; _temp325.f1= _temp310; _temp325.f2= _temp320; _temp325;}); _temp324;});}}}}
struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d){
if( d->t == 0){ return({ struct Cyc_Dict_Dict* _temp326=( struct Cyc_Dict_Dict*)
GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp326->rel= d->rel; _temp326->t= 0;
_temp326;});} return({ struct Cyc_Dict_Dict* _temp327=( struct Cyc_Dict_Dict*)
GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp327->rel= d->rel; _temp327->t=((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( d->t))); _temp327;});}
static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*, void*), void*
env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp330; struct _tuple0 _temp331;
void* _temp333; void* _temp335; struct Cyc_Dict_T* _temp337; struct Cyc_Dict_T*
_temp339; void* _temp341; struct Cyc_Dict_T* _temp328= t; _temp330=* _temp328;
_LL342: _temp341=( void*) _temp330.color; goto _LL340; _LL340: _temp339=( struct
Cyc_Dict_T*) _temp330.left; goto _LL338; _LL338: _temp337=( struct Cyc_Dict_T*)
_temp330.right; goto _LL332; _LL332: _temp331=( struct _tuple0) _temp330.key_val;
_LL336: _temp335= _temp331.f1; goto _LL334; _LL334: _temp333= _temp331.f2; goto
_LL329; _LL329: { struct Cyc_Dict_T* _temp343= _temp339 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)(
f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp339)));
goto _LL344; _LL344: { void* _temp345= f( env, _temp333); goto _LL346; _LL346: {
struct Cyc_Dict_T* _temp347= _temp337 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(*
f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f,
env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp337))); goto
_LL348; _LL348: return({ struct Cyc_Dict_T* _temp349=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp349->color=( void*) _temp341;
_temp349->left= _temp343; _temp349->right= _temp347; _temp349->key_val=({ struct
_tuple0 _temp350; _temp350.f1= _temp335; _temp350.f2= _temp345; _temp350;});
_temp349;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp351=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp351->rel= d->rel; _temp351->t= 0; _temp351;});} return({ struct Cyc_Dict_Dict*
_temp352=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp352->rel= d->rel; _temp352->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( d->t))); _temp352;});} struct
_tuple0* Cyc_Dict_choose( struct Cyc_Dict_Dict* d){ if( d->t == 0){( void)
_throw(( void*) Cyc_Dict_Absent);} return({ struct _tuple0* _temp353=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp353->f1=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f1; _temp353->f2=((( struct Cyc_Dict_T*)
_check_null( d->t))->key_val).f2; _temp353;});} static int Cyc_Dict_forall_tree_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T
_temp356; struct _tuple0 _temp357; void* _temp359; void* _temp361; struct Cyc_Dict_T*
_temp363; struct Cyc_Dict_T* _temp365; void* _temp367; struct Cyc_Dict_T*
_temp354= t; _temp356=* _temp354; _LL368: _temp367=( void*) _temp356.color; goto
_LL366; _LL366: _temp365=( struct Cyc_Dict_T*) _temp356.left; goto _LL364;
_LL364: _temp363=( struct Cyc_Dict_T*) _temp356.right; goto _LL358; _LL358:
_temp357=( struct _tuple0) _temp356.key_val; _LL362: _temp361= _temp357.f1; goto
_LL360; _LL360: _temp359= _temp357.f2; goto _LL355; _LL355: return(( _temp365 ==
0? 1:(( int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t))
Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*)
_check_null( _temp365))))? f( env, _temp361, _temp359): 0)? _temp363 == 0? 1:((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)(( struct Cyc_Dict_T*) _check_null( _temp363))): 0;}
int Cyc_Dict_forall_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d){ if( d->t == 0){ return 1;} return(( int(*)( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)((
struct Cyc_Dict_T*) _check_null( d->t)));} struct _tuple4{ int(* f1)( void*,
void*, void*); struct Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f(
struct _tuple4* env, void* a, void* b){ struct _tuple4 _temp371; struct Cyc_Dict_Dict*
_temp372; int(* _temp374)( void*, void*, void*); struct _tuple4* _temp369= env;
_temp371=* _temp369; _LL375: _temp374= _temp371.f1; goto _LL373; _LL373:
_temp372= _temp371.f2; goto _LL370; _LL370: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp372, a)){ return _temp374( a, b,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp372, a));} return 1;}
int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct _tuple4 _temp377=({ struct _tuple4
_temp376; _temp376.f1= f; _temp376.f2= d2; _temp376;}); goto _LL378; _LL378:
return(( int(*)( int(* f)( struct _tuple4*, void*, void*), struct _tuple4* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)( struct _tuple4* env, void*
a, void* b)) Cyc_Dict_forall_intersect_f,& _temp377, d1);} static struct Cyc_Dict_Dict*
Cyc_Dict_union_f( void*(* f)( void*, void*), void* a, void* b, struct Cyc_Dict_Dict*
d1){ if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( d1, a)){
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( d1, a, f((( void*(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_lookup)( d1, a), b));} else{ return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d1, a, b);}}
struct Cyc_Dict_Dict* Cyc_Dict_union_two( void*(* f)( void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;} return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*(* f)( void*(*)( void*, void*), void*, void*, struct Cyc_Dict_Dict*),
void*(* env)( void*, void*), struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict*
accum)) Cyc_Dict_fold_c)(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*),
void* a, void* b, struct Cyc_Dict_Dict* d1)) Cyc_Dict_union_f, f, d2, d1);}
struct _tuple5{ void*(* f1)( void*, void*); struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; } ; static void Cyc_Dict_intersect_f( struct _tuple5* trip, void* a, void* b){
struct _tuple5 _temp381; struct Cyc_Dict_Dict* _temp382; struct Cyc_Dict_Dict**
_temp384; struct Cyc_Dict_Dict* _temp385; void*(* _temp387)( void*, void*);
struct _tuple5* _temp379= trip; _temp381=* _temp379; _LL388: _temp387= _temp381.f1;
goto _LL386; _LL386: _temp385= _temp381.f2; goto _LL383; _LL383: _temp382=
_temp381.f3; _temp384=&(* _temp379).f3; goto _LL380; _LL380: if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp385, a)){* _temp384=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(*
_temp384, a, _temp387((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp385, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct _tuple5 _temp390=({ struct _tuple5 _temp389; _temp389.f1= f;
_temp389.f2= d1; _temp389.f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*,
void*))) Cyc_Dict_empty)( d1->rel); _temp389;}); goto _LL391; _LL391:(( void(*)(
void(* f)( struct _tuple5*, void*, void*), struct _tuple5* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple5* trip, void* a, void* b)) Cyc_Dict_intersect_f,&
_temp390, d2); return _temp390.f3;}} struct _tuple6{ void*(* f1)( void*, void*,
void*); void* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static
void Cyc_Dict_intersect_c_f( struct _tuple6* quad_t, void* a, void* b){ struct
_tuple6 _temp394; struct Cyc_Dict_Dict* _temp395; struct Cyc_Dict_Dict**
_temp397; struct Cyc_Dict_Dict* _temp398; void* _temp400; void*(* _temp402)(
void*, void*, void*); struct _tuple6* _temp392= quad_t; _temp394=* _temp392;
_LL403: _temp402= _temp394.f1; goto _LL401; _LL401: _temp400= _temp394.f2; goto
_LL399; _LL399: _temp398= _temp394.f3; goto _LL396; _LL396: _temp395= _temp394.f4;
_temp397=&(* _temp392).f4; goto _LL393; _LL393: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp398, a)){* _temp397=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp397, a,
_temp402( _temp400,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp398, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ if( d1 == d2){ return d1;}{ struct _tuple6 _temp405=({ struct _tuple6
_temp404; _temp404.f1= f; _temp404.f2= env; _temp404.f3= d1; _temp404.f4=((
struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)( d1->rel);
_temp404;}); goto _LL406; _LL406:(( void(*)( void(* f)( struct _tuple6*, void*,
void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple6* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,& _temp405, d2);
return _temp405.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k,
void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp407=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp407->hd=(
void*)({ struct _tuple0* _temp408=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp408->f1= k; _temp408->f2= v; _temp408;}); _temp407->tl= accum;
_temp407;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( void*, void*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)(( struct Cyc_List_List*(*)( void* k, void* v, struct Cyc_List_List*
accum)) Cyc_Dict_to_list_f, d, 0);} static struct Cyc_Dict_Dict* Cyc_Dict_filter_f(
int(* f)( void*, void*), void* x, void* y, struct Cyc_Dict_Dict* acc){ return f(
x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_filter(
int(* f)( void*, void*), struct Cyc_Dict_Dict* d){ return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*(* f)( int(*)( void*, void*), void*, void*, struct Cyc_Dict_Dict*),
int(* env)( void*, void*), struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum))
Cyc_Dict_fold_c)(( struct Cyc_Dict_Dict*(*)( int(* f)( void*, void*), void* x,
void* y, struct Cyc_Dict_Dict* acc)) Cyc_Dict_filter_f, f, d,(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( d->rel));} struct _tuple7{ int(*
f1)( void*, void*, void*); void* f2; } ; static struct Cyc_Dict_Dict* Cyc_Dict_filter_c_f(
struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict* acc){ struct
_tuple7 _temp411; void* _temp412; int(* _temp414)( void*, void*, void*); struct
_tuple7* _temp409= env; _temp411=* _temp409; _LL415: _temp414= _temp411.f1; goto
_LL413; _LL413: _temp412= _temp411.f2; goto _LL410; _LL410: return _temp414(
_temp412, x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct
_tuple7 _temp417=({ struct _tuple7 _temp416; _temp416.f1= f; _temp416.f2= f_env;
_temp416;}); goto _LL418; _LL418: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(*
f)( struct _tuple7*, void*, void*, struct Cyc_Dict_Dict*), struct _tuple7* env,
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_c_f,& _temp417, d,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( d->rel));} static int Cyc_Dict_difference_f(
struct Cyc_Dict_Dict* d, void* x, void* y){ return !(( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, x);} struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Dict_Dict*, void*, void*), struct Cyc_Dict_Dict* f_env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(( int(*)( struct Cyc_Dict_Dict* d,
void* x, void* y)) Cyc_Dict_difference_f, d2, d1);} struct _tuple8{ int(* f1)(
void*, void*); void* f2; } ; static int Cyc_Dict_delete_f( struct _tuple8* env,
void* x, void* y){ struct _tuple8 _temp421; void* _temp422; int(* _temp424)(
void*, void*); struct _tuple8* _temp419= env; _temp421=* _temp419; _LL425:
_temp424= _temp421.f1; goto _LL423; _LL423: _temp422= _temp421.f2; goto _LL420;
_LL420: return _temp424( _temp422, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict* d, void* x){ if((( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup_opt)( d, x) == 0){ return d;}{ struct _tuple8
_temp427=({ struct _tuple8 _temp426; _temp426.f1= d->rel; _temp426.f2= x;
_temp426;}); goto _LL428; _LL428: return(( struct Cyc_Dict_Dict*(*)( int(* f)(
struct _tuple8*, void*, void*), struct _tuple8* f_env, struct Cyc_Dict_Dict* d))
Cyc_Dict_filter_c)(( int(*)( struct _tuple8* env, void* x, void* y)) Cyc_Dict_delete_f,&
_temp427, d);}}