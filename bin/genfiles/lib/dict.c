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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
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
Cyc_Dict_delete( struct Cyc_Dict_Dict*, void*); char Cyc_Dict_Absent_tag[ 7u]="Absent";
char Cyc_Dict_Present_tag[ 8u]="Present"; typedef void* Cyc_Dict_Color; static
const unsigned int Cyc_Dict_R= 0; static const unsigned int Cyc_Dict_B= 1;
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
Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T*
_temp6= t; if( _temp6 == 0){ _throw( Null_Exception);} _temp6->key_val;}).f1);
if( i < 0){ t=({ struct Cyc_Dict_T* _temp4= t; if( _temp4 == 0){ _throw(
Null_Exception);} _temp4->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T*
_temp5= t; if( _temp5 == 0){ _throw( Null_Exception);} _temp5->right;});} else{
return 1;}}} return 0;} void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void*
key){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t !=
0) { int i= rel( key,({ struct Cyc_Dict_T* _temp10= t; if( _temp10 == 0){ _throw(
Null_Exception);} _temp10->key_val;}).f1); if( i < 0){ t=({ struct Cyc_Dict_T*
_temp7= t; if( _temp7 == 0){ _throw( Null_Exception);} _temp7->left;});} else{
if( i > 0){ t=({ struct Cyc_Dict_T* _temp8= t; if( _temp8 == 0){ _throw(
Null_Exception);} _temp8->right;});} else{ return({ struct Cyc_Dict_T* _temp9= t;
if( _temp9 == 0){ _throw( Null_Exception);} _temp9->key_val;}).f2;}}}( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp11=(
struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp11[ 0]=({ struct Cyc_Dict_Absent_struct _temp12; _temp12.tag= Cyc_Dict_Absent_tag;
_temp12;}); _temp11;}));} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t;
while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T* _temp17= t; if( _temp17
== 0){ _throw( Null_Exception);} _temp17->key_val;}).f1); if( i < 0){ t=({
struct Cyc_Dict_T* _temp13= t; if( _temp13 == 0){ _throw( Null_Exception);}
_temp13->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T* _temp14= t; if(
_temp14 == 0){ _throw( Null_Exception);} _temp14->right;});} else{ return({
struct Cyc_Core_Opt* _temp15=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp15->v=( void*)({ struct Cyc_Dict_T* _temp16= t; if( _temp16
== 0){ _throw( Null_Exception);} _temp16->key_val;}).f2; _temp15;});}}} return 0;}
int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void** ans_place){
int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) {
int i= rel( key,({ struct Cyc_Dict_T* _temp21= t; if( _temp21 == 0){ _throw(
Null_Exception);} _temp21->key_val;}).f1); if( i < 0){ t=({ struct Cyc_Dict_T*
_temp18= t; if( _temp18 == 0){ _throw( Null_Exception);} _temp18->left;});}
else{ if( i > 0){ t=({ struct Cyc_Dict_T* _temp19= t; if( _temp19 == 0){ _throw(
Null_Exception);} _temp19->right;});} else{* ans_place=({ struct Cyc_Dict_T*
_temp20= t; if( _temp20 == 0){ _throw( Null_Exception);} _temp20->key_val;}).f2;
return 1;}}} return 0;} struct _tuple1{ void* f1; struct Cyc_Dict_T* f2; struct
Cyc_Dict_T* f3; struct _tuple0 f4; } ; static struct Cyc_Dict_T* Cyc_Dict_balance(
struct _tuple1 quad_t){ struct _tuple1 _temp22= quad_t; struct _tuple0 _temp34;
struct Cyc_Dict_T* _temp36; struct Cyc_Dict_T* _temp38; struct Cyc_Dict_T
_temp40; struct _tuple0 _temp41; struct Cyc_Dict_T* _temp43; struct Cyc_Dict_T*
_temp45; struct Cyc_Dict_T _temp47; struct _tuple0 _temp48; struct Cyc_Dict_T*
_temp50; struct Cyc_Dict_T* _temp52; void* _temp54; void* _temp56; void* _temp58;
struct _tuple0 _temp60; struct Cyc_Dict_T* _temp62; struct Cyc_Dict_T* _temp64;
struct Cyc_Dict_T _temp66; struct _tuple0 _temp67; struct Cyc_Dict_T* _temp69;
struct Cyc_Dict_T _temp71; struct _tuple0 _temp72; struct Cyc_Dict_T* _temp74;
struct Cyc_Dict_T* _temp76; void* _temp78; struct Cyc_Dict_T* _temp80; void*
_temp82; void* _temp84; struct _tuple0 _temp86; struct Cyc_Dict_T* _temp88;
struct Cyc_Dict_T _temp90; struct _tuple0 _temp91; struct Cyc_Dict_T* _temp93;
struct Cyc_Dict_T* _temp95; struct Cyc_Dict_T _temp97; struct _tuple0 _temp98;
struct Cyc_Dict_T* _temp100; struct Cyc_Dict_T* _temp102; void* _temp104; void*
_temp106; struct Cyc_Dict_T* _temp108; void* _temp110; struct _tuple0 _temp112;
struct Cyc_Dict_T* _temp114; struct Cyc_Dict_T _temp116; struct _tuple0 _temp117;
struct Cyc_Dict_T* _temp119; struct Cyc_Dict_T _temp121; struct _tuple0 _temp122;
struct Cyc_Dict_T* _temp124; struct Cyc_Dict_T* _temp126; void* _temp128; struct
Cyc_Dict_T* _temp130; void* _temp132; struct Cyc_Dict_T* _temp134; void*
_temp136; struct _tuple0 _temp138; struct Cyc_Dict_T* _temp140; struct Cyc_Dict_T*
_temp142; void* _temp144; _LL24: _LL59: _temp58= _temp22.f1; if(( int) _temp58
== Cyc_Dict_B){ goto _LL39;} else{ goto _LL26;} _LL39: _temp38= _temp22.f2; if(
_temp38 == 0){ goto _LL26;} else{ _temp40=* _temp38; _LL57: _temp56=( void*)
_temp40.color; if(( int) _temp56 == Cyc_Dict_R){ goto _LL46;} else{ goto _LL26;}
_LL46: _temp45=( struct Cyc_Dict_T*) _temp40.left; if( _temp45 == 0){ goto _LL26;}
else{ _temp47=* _temp45; _LL55: _temp54=( void*) _temp47.color; if(( int)
_temp54 == Cyc_Dict_R){ goto _LL53;} else{ goto _LL26;} _LL53: _temp52=( struct
Cyc_Dict_T*) _temp47.left; goto _LL51; _LL51: _temp50=( struct Cyc_Dict_T*)
_temp47.right; goto _LL49; _LL49: _temp48=( struct _tuple0) _temp47.key_val;
goto _LL44;} _LL44: _temp43=( struct Cyc_Dict_T*) _temp40.right; goto _LL42;
_LL42: _temp41=( struct _tuple0) _temp40.key_val; goto _LL37;} _LL37: _temp36=
_temp22.f3; goto _LL35; _LL35: _temp34= _temp22.f4; goto _LL25; _LL26: _LL85:
_temp84= _temp22.f1; if(( int) _temp84 == Cyc_Dict_B){ goto _LL65;} else{ goto
_LL28;} _LL65: _temp64= _temp22.f2; if( _temp64 == 0){ goto _LL28;} else{
_temp66=* _temp64; _LL83: _temp82=( void*) _temp66.color; if(( int) _temp82 ==
Cyc_Dict_R){ goto _LL81;} else{ goto _LL28;} _LL81: _temp80=( struct Cyc_Dict_T*)
_temp66.left; goto _LL70; _LL70: _temp69=( struct Cyc_Dict_T*) _temp66.right;
if( _temp69 == 0){ goto _LL28;} else{ _temp71=* _temp69; _LL79: _temp78=( void*)
_temp71.color; if(( int) _temp78 == Cyc_Dict_R){ goto _LL77;} else{ goto _LL28;}
_LL77: _temp76=( struct Cyc_Dict_T*) _temp71.left; goto _LL75; _LL75: _temp74=(
struct Cyc_Dict_T*) _temp71.right; goto _LL73; _LL73: _temp72=( struct _tuple0)
_temp71.key_val; goto _LL68;} _LL68: _temp67=( struct _tuple0) _temp66.key_val;
goto _LL63;} _LL63: _temp62= _temp22.f3; goto _LL61; _LL61: _temp60= _temp22.f4;
goto _LL27; _LL28: _LL111: _temp110= _temp22.f1; if(( int) _temp110 == Cyc_Dict_B){
goto _LL109;} else{ goto _LL30;} _LL109: _temp108= _temp22.f2; goto _LL89; _LL89:
_temp88= _temp22.f3; if( _temp88 == 0){ goto _LL30;} else{ _temp90=* _temp88;
_LL107: _temp106=( void*) _temp90.color; if(( int) _temp106 == Cyc_Dict_R){ goto
_LL96;} else{ goto _LL30;} _LL96: _temp95=( struct Cyc_Dict_T*) _temp90.left;
if( _temp95 == 0){ goto _LL30;} else{ _temp97=* _temp95; _LL105: _temp104=( void*)
_temp97.color; if(( int) _temp104 == Cyc_Dict_R){ goto _LL103;} else{ goto _LL30;}
_LL103: _temp102=( struct Cyc_Dict_T*) _temp97.left; goto _LL101; _LL101:
_temp100=( struct Cyc_Dict_T*) _temp97.right; goto _LL99; _LL99: _temp98=(
struct _tuple0) _temp97.key_val; goto _LL94;} _LL94: _temp93=( struct Cyc_Dict_T*)
_temp90.right; goto _LL92; _LL92: _temp91=( struct _tuple0) _temp90.key_val;
goto _LL87;} _LL87: _temp86= _temp22.f4; goto _LL29; _LL30: _LL137: _temp136=
_temp22.f1; if(( int) _temp136 == Cyc_Dict_B){ goto _LL135;} else{ goto _LL32;}
_LL135: _temp134= _temp22.f2; goto _LL115; _LL115: _temp114= _temp22.f3; if(
_temp114 == 0){ goto _LL32;} else{ _temp116=* _temp114; _LL133: _temp132=( void*)
_temp116.color; if(( int) _temp132 == Cyc_Dict_R){ goto _LL131;} else{ goto
_LL32;} _LL131: _temp130=( struct Cyc_Dict_T*) _temp116.left; goto _LL120;
_LL120: _temp119=( struct Cyc_Dict_T*) _temp116.right; if( _temp119 == 0){ goto
_LL32;} else{ _temp121=* _temp119; _LL129: _temp128=( void*) _temp121.color; if((
int) _temp128 == Cyc_Dict_R){ goto _LL127;} else{ goto _LL32;} _LL127: _temp126=(
struct Cyc_Dict_T*) _temp121.left; goto _LL125; _LL125: _temp124=( struct Cyc_Dict_T*)
_temp121.right; goto _LL123; _LL123: _temp122=( struct _tuple0) _temp121.key_val;
goto _LL118;} _LL118: _temp117=( struct _tuple0) _temp116.key_val; goto _LL113;}
_LL113: _temp112= _temp22.f4; goto _LL31; _LL32: _LL145: _temp144= _temp22.f1;
goto _LL143; _LL143: _temp142= _temp22.f2; goto _LL141; _LL141: _temp140=
_temp22.f3; goto _LL139; _LL139: _temp138= _temp22.f4; goto _LL33; _LL25: return({
struct Cyc_Dict_T* _temp146=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp146->color=( void*)(( void*) Cyc_Dict_R); _temp146->left=({ struct Cyc_Dict_T*
_temp148=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp148->color=(
void*)(( void*) Cyc_Dict_B); _temp148->left= _temp52; _temp148->right= _temp50;
_temp148->key_val= _temp48; _temp148;}); _temp146->right=({ struct Cyc_Dict_T*
_temp147=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp147->color=(
void*)(( void*) Cyc_Dict_B); _temp147->left= _temp43; _temp147->right= _temp36;
_temp147->key_val= _temp34; _temp147;}); _temp146->key_val= _temp41; _temp146;});
_LL27: return({ struct Cyc_Dict_T* _temp149=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp149->color=( void*)(( void*) Cyc_Dict_R);
_temp149->left=({ struct Cyc_Dict_T* _temp151=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp151->color=( void*)(( void*) Cyc_Dict_B);
_temp151->left= _temp80; _temp151->right= _temp76; _temp151->key_val= _temp67;
_temp151;}); _temp149->right=({ struct Cyc_Dict_T* _temp150=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp150->color=( void*)(( void*) Cyc_Dict_B);
_temp150->left= _temp74; _temp150->right= _temp62; _temp150->key_val= _temp60;
_temp150;}); _temp149->key_val= _temp72; _temp149;}); _LL29: return({ struct Cyc_Dict_T*
_temp152=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp152->color=(
void*)(( void*) Cyc_Dict_R); _temp152->left=({ struct Cyc_Dict_T* _temp154=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp154->color=(
void*)(( void*) Cyc_Dict_B); _temp154->left= _temp108; _temp154->right= _temp102;
_temp154->key_val= _temp86; _temp154;}); _temp152->right=({ struct Cyc_Dict_T*
_temp153=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp153->color=(
void*)(( void*) Cyc_Dict_B); _temp153->left= _temp100; _temp153->right= _temp93;
_temp153->key_val= _temp91; _temp153;}); _temp152->key_val= _temp98; _temp152;});
_LL31: return({ struct Cyc_Dict_T* _temp155=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp155->color=( void*)(( void*) Cyc_Dict_R);
_temp155->left=({ struct Cyc_Dict_T* _temp157=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp157->color=( void*)(( void*) Cyc_Dict_B);
_temp157->left= _temp134; _temp157->right= _temp130; _temp157->key_val= _temp112;
_temp157;}); _temp155->right=({ struct Cyc_Dict_T* _temp156=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp156->color=( void*)(( void*) Cyc_Dict_B);
_temp156->left= _temp126; _temp156->right= _temp124; _temp156->key_val= _temp122;
_temp156;}); _temp155->key_val= _temp117; _temp155;}); _LL33: return({ struct
Cyc_Dict_T* _temp158=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp158->color=( void*) _temp144; _temp158->left= _temp142; _temp158->right=
_temp140; _temp158->key_val= _temp138; _temp158;}); _LL23:;} static struct Cyc_Dict_T*
Cyc_Dict_ins( int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T* _temp159= t; struct Cyc_Dict_T _temp165; struct _tuple0
_temp166; struct Cyc_Dict_T* _temp168; struct Cyc_Dict_T* _temp170; void*
_temp172; _LL161: if( _temp159 == 0){ goto _LL162;} else{ goto _LL163;} _LL163:
if( _temp159 == 0){ goto _LL160;} else{ _temp165=* _temp159; _LL173: _temp172=(
void*) _temp165.color; goto _LL171; _LL171: _temp170=( struct Cyc_Dict_T*)
_temp165.left; goto _LL169; _LL169: _temp168=( struct Cyc_Dict_T*) _temp165.right;
goto _LL167; _LL167: _temp166=( struct _tuple0) _temp165.key_val; goto _LL164;}
_LL162: return({ struct Cyc_Dict_T* _temp174=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp174->color=( void*)(( void*) Cyc_Dict_R);
_temp174->left= 0; _temp174->right= 0; _temp174->key_val= key_val; _temp174;});
_LL164: { int i= rel( key_val.f1, _temp166.f1); if( i < 0){ return(( struct Cyc_Dict_T*(*)(
struct _tuple1 quad_t)) Cyc_Dict_balance)(({ struct _tuple1 _temp175; _temp175.f1=
_temp172; _temp175.f2=(( struct Cyc_Dict_T*(*)( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val,
_temp170); _temp175.f3= _temp168; _temp175.f4= _temp166; _temp175;}));} else{
if( i > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t)) Cyc_Dict_balance)(({
struct _tuple1 _temp176; _temp176.f1= _temp172; _temp176.f2= _temp170; _temp176.f3=((
struct Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct
Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val, _temp168); _temp176.f4= _temp166;
_temp176;}));} else{ return({ struct Cyc_Dict_T* _temp177=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp177->color=( void*) _temp172;
_temp177->left= _temp170; _temp177->right= _temp168; _temp177->key_val= key_val;
_temp177;});}}} _LL160:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data){ struct Cyc_Dict_T* ans=(( struct Cyc_Dict_T*(*)( int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
d->rel,({ struct _tuple0 _temp180; _temp180.f1= key; _temp180.f2= data; _temp180;}),
d->t);( void*)({ struct Cyc_Dict_T* _temp178= ans; if( _temp178 == 0){ _throw(
Null_Exception);} _temp178->color=( void*)(( void*) Cyc_Dict_B);}); return({
struct Cyc_Dict_Dict* _temp179=( struct Cyc_Dict_Dict*) GC_malloc( sizeof(
struct Cyc_Dict_Dict)); _temp179->rel= d->rel; _temp179->t= ans; _temp179;});}
struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key,
void* data){ if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
d, key)){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct*
_temp181=( struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp181[ 0]=({ struct Cyc_Dict_Absent_struct _temp182; _temp182.tag= Cyc_Dict_Absent_tag;
_temp182;}); _temp181;}));} return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( d, key, data);} struct Cyc_Dict_Dict*
Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List* kds){ for( 0;
kds != 0; kds=({ struct Cyc_List_List* _temp183= kds; if( _temp183 == 0){ _throw(
Null_Exception);} _temp183->tl;})){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( d,({ struct _tuple0* _temp185=(
struct _tuple0*)({ struct Cyc_List_List* _temp184= kds; if( _temp184 == 0){
_throw( Null_Exception);} _temp184->hd;}); unsigned int _temp186= 0; if(
_temp186 >= 1u){ _throw( Null_Exception);} _temp185[ _temp186];}).f1,({ struct
_tuple0* _temp188=( struct _tuple0*)({ struct Cyc_List_List* _temp187= kds; if(
_temp187 == 0){ _throw( Null_Exception);} _temp187->hd;}); unsigned int _temp189=
0; if( _temp189 >= 1u){ _throw( Null_Exception);} _temp188[ _temp189];}).f2);}
return d;} static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp192; struct _tuple0
_temp193; void* _temp195; void* _temp197; struct Cyc_Dict_T* _temp199; struct
Cyc_Dict_T* _temp201; void* _temp203; struct Cyc_Dict_T* _temp190= t; _temp192=*
_temp190; _LL204: _temp203=( void*) _temp192.color; goto _LL202; _LL202:
_temp201=( struct Cyc_Dict_T*) _temp192.left; goto _LL200; _LL200: _temp199=(
struct Cyc_Dict_T*) _temp192.right; goto _LL194; _LL194: _temp193=( struct
_tuple0) _temp192.key_val; _LL198: _temp197= _temp193.f1; goto _LL196; _LL196:
_temp195= _temp193.f2; goto _LL191; _LL191: if( _temp201 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp205= _temp201; if( _temp205 ==
0){ _throw( Null_Exception);} _temp205;}), accum);} accum= f( _temp197, _temp195,
accum); if( _temp199 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp206= _temp199; if( _temp206 == 0){ _throw(
Null_Exception);} _temp206;}), accum);} return accum;} void* Cyc_Dict_fold( void*(*
f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){
return accum;} return(( void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp207= d->t; if( _temp207 == 0){ _throw( Null_Exception);} _temp207;}), accum);}
static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*, void*, void*), void*
env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp210; struct
_tuple0 _temp211; void* _temp213; void* _temp215; struct Cyc_Dict_T* _temp217;
struct Cyc_Dict_T* _temp219; void* _temp221; struct Cyc_Dict_T* _temp208= t;
_temp210=* _temp208; _LL222: _temp221=( void*) _temp210.color; goto _LL220;
_LL220: _temp219=( struct Cyc_Dict_T*) _temp210.left; goto _LL218; _LL218:
_temp217=( struct Cyc_Dict_T*) _temp210.right; goto _LL212; _LL212: _temp211=(
struct _tuple0) _temp210.key_val; _LL216: _temp215= _temp211.f1; goto _LL214;
_LL214: _temp213= _temp211.f2; goto _LL209; _LL209: if( _temp219 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct
Cyc_Dict_T* _temp223= _temp219; if( _temp223 == 0){ _throw( Null_Exception);}
_temp223;}), accum);} accum= f( env, _temp215, _temp213, accum); if( _temp217 !=
0){ accum=(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp224= _temp217; if( _temp224 == 0){ _throw(
Null_Exception);} _temp224;}), accum);} return accum;} void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp225= d->t; if( _temp225
== 0){ _throw( Null_Exception);} _temp225;}), accum);} static void Cyc_Dict_app_tree(
void*(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp228;
struct _tuple0 _temp229; void* _temp231; void* _temp233; struct Cyc_Dict_T*
_temp235; struct Cyc_Dict_T* _temp237; void* _temp239; struct Cyc_Dict_T*
_temp226= t; _temp228=* _temp226; _LL240: _temp239=( void*) _temp228.color; goto
_LL238; _LL238: _temp237=( struct Cyc_Dict_T*) _temp228.left; goto _LL236;
_LL236: _temp235=( struct Cyc_Dict_T*) _temp228.right; goto _LL230; _LL230:
_temp229=( struct _tuple0) _temp228.key_val; _LL234: _temp233= _temp229.f1; goto
_LL232; _LL232: _temp231= _temp229.f2; goto _LL227; _LL227: if( _temp237 != 0){((
void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp241= _temp237; if( _temp241 == 0){
_throw( Null_Exception);} _temp241;}));} f( _temp233, _temp231); if( _temp235 !=
0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp242= _temp235; if( _temp242 ==
0){ _throw( Null_Exception);} _temp242;}));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp243= d->t; if( _temp243 == 0){ _throw( Null_Exception);}
_temp243;}));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp246; struct _tuple0
_temp247; void* _temp249; void* _temp251; struct Cyc_Dict_T* _temp253; struct
Cyc_Dict_T* _temp255; void* _temp257; struct Cyc_Dict_T* _temp244= t; _temp246=*
_temp244; _LL258: _temp257=( void*) _temp246.color; goto _LL256; _LL256:
_temp255=( struct Cyc_Dict_T*) _temp246.left; goto _LL254; _LL254: _temp253=(
struct Cyc_Dict_T*) _temp246.right; goto _LL248; _LL248: _temp247=( struct
_tuple0) _temp246.key_val; _LL252: _temp251= _temp247.f1; goto _LL250; _LL250:
_temp249= _temp247.f2; goto _LL245; _LL245: if( _temp255 != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp259= _temp255; if(
_temp259 == 0){ _throw( Null_Exception);} _temp259;}));} f( env, _temp251,
_temp249); if( _temp253 != 0){(( void(*)( void*(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp260= _temp253; if( _temp260 == 0){ _throw(
Null_Exception);} _temp260;}));}} void Cyc_Dict_app_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp261= d->t; if( _temp261
== 0){ _throw( Null_Exception);} _temp261;}));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp264;
struct _tuple0 _temp265; void* _temp267; void* _temp269; struct Cyc_Dict_T*
_temp271; struct Cyc_Dict_T* _temp273; void* _temp275; struct Cyc_Dict_T*
_temp262= t; _temp264=* _temp262; _LL276: _temp275=( void*) _temp264.color; goto
_LL274; _LL274: _temp273=( struct Cyc_Dict_T*) _temp264.left; goto _LL272;
_LL272: _temp271=( struct Cyc_Dict_T*) _temp264.right; goto _LL266; _LL266:
_temp265=( struct _tuple0) _temp264.key_val; _LL270: _temp269= _temp265.f1; goto
_LL268; _LL268: _temp267= _temp265.f2; goto _LL263; _LL263: if( _temp273 != 0){((
void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp277= _temp273; if( _temp277 == 0){
_throw( Null_Exception);} _temp277;}));} f( _temp269, _temp267); if( _temp271 !=
0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp278= _temp271; if( _temp278 ==
0){ _throw( Null_Exception);} _temp278;}));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp279= d->t; if( _temp279 == 0){ _throw( Null_Exception);}
_temp279;}));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp282; struct _tuple0
_temp283; void* _temp285; void* _temp287; struct Cyc_Dict_T* _temp289; struct
Cyc_Dict_T* _temp291; void* _temp293; struct Cyc_Dict_T* _temp280= t; _temp282=*
_temp280; _LL294: _temp293=( void*) _temp282.color; goto _LL292; _LL292:
_temp291=( struct Cyc_Dict_T*) _temp282.left; goto _LL290; _LL290: _temp289=(
struct Cyc_Dict_T*) _temp282.right; goto _LL284; _LL284: _temp283=( struct
_tuple0) _temp282.key_val; _LL288: _temp287= _temp283.f1; goto _LL286; _LL286:
_temp285= _temp283.f2; goto _LL281; _LL281: if( _temp291 != 0){(( void(*)( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp295= _temp291; if(
_temp295 == 0){ _throw( Null_Exception);} _temp295;}));} f( env, _temp287,
_temp285); if( _temp289 != 0){(( void(*)( void(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp296= _temp289; if( _temp296 == 0){ _throw(
Null_Exception);} _temp296;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp297= d->t; if( _temp297 == 0){
_throw( Null_Exception);} _temp297;}));}} struct _tuple2{ void(* f1)( void*,
void*); struct Cyc_Dict_Dict* f2; } ; static void Cyc_Dict_iter2_f( struct
_tuple2* env, void* a, void* b1){ struct _tuple2 _temp300; struct Cyc_Dict_Dict*
_temp301; void(* _temp303)( void*, void*); struct _tuple2* _temp298= env;
_temp300=* _temp298; _LL304: _temp303= _temp300.f1; goto _LL302; _LL302:
_temp301= _temp300.f2; goto _LL299; _LL299: _temp303( b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp301, a));} void Cyc_Dict_iter2( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple2 env=({ struct _tuple2 _temp305; _temp305.f1= f; _temp305.f2= d2;
_temp305;});(( void(*)( void(* f)( struct _tuple2*, void*, void*), struct
_tuple2* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
_tuple2* env, void* a, void* b1)) Cyc_Dict_iter2_f,& env, d1);} struct _tuple3{
void(* f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void* f3; } ; static
void Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){ struct _tuple3
_temp308; void* _temp309; struct Cyc_Dict_Dict* _temp311; void(* _temp313)( void*,
void*, void*); struct _tuple3* _temp306= env; _temp308=* _temp306; _LL314:
_temp313= _temp308.f1; goto _LL312; _LL312: _temp311= _temp308.f2; goto _LL310;
_LL310: _temp309= _temp308.f3; goto _LL307; _LL307: _temp313( _temp309, b1,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp311, a));}
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* inner_env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3 env=({ struct
_tuple3 _temp315; _temp315.f1= f; _temp315.f2= d2; _temp315.f3= inner_env;
_temp315;});(( void(*)( void(* f)( struct _tuple3*, void*, void*), struct
_tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
_tuple3* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,& env, d1);} static struct
Cyc_Dict_T* Cyc_Dict_map_tree( void*(* f)( void*), struct Cyc_Dict_T* t){ struct
Cyc_Dict_T _temp318; struct _tuple0 _temp319; void* _temp321; void* _temp323;
struct Cyc_Dict_T* _temp325; struct Cyc_Dict_T* _temp327; void* _temp329; struct
Cyc_Dict_T* _temp316= t; _temp318=* _temp316; _LL330: _temp329=( void*) _temp318.color;
goto _LL328; _LL328: _temp327=( struct Cyc_Dict_T*) _temp318.left; goto _LL326;
_LL326: _temp325=( struct Cyc_Dict_T*) _temp318.right; goto _LL320; _LL320:
_temp319=( struct _tuple0) _temp318.key_val; _LL324: _temp323= _temp319.f1; goto
_LL322; _LL322: _temp321= _temp319.f2; goto _LL317; _LL317: { struct Cyc_Dict_T*
new_left= _temp327 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct
Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp334= _temp327; if( _temp334 == 0){ _throw( Null_Exception);} _temp334;}));
void* new_val= f( _temp321); struct Cyc_Dict_T* new_right= _temp325 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp333= _temp325; if( _temp333 ==
0){ _throw( Null_Exception);} _temp333;})); return({ struct Cyc_Dict_T* _temp331=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp331->color=(
void*) _temp329; _temp331->left= new_left; _temp331->right= new_right; _temp331->key_val=({
struct _tuple0 _temp332; _temp332.f1= _temp323; _temp332.f2= new_val; _temp332;});
_temp331;});}} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct
Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict* _temp335=(
struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp335->rel=
d->rel; _temp335->t= 0; _temp335;});} return({ struct Cyc_Dict_Dict* _temp336=(
struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp336->rel=
d->rel; _temp336->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp337= d->t;
if( _temp337 == 0){ _throw( Null_Exception);} _temp337;})); _temp336;});} static
struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp340; struct _tuple0 _temp341; void*
_temp343; void* _temp345; struct Cyc_Dict_T* _temp347; struct Cyc_Dict_T*
_temp349; void* _temp351; struct Cyc_Dict_T* _temp338= t; _temp340=* _temp338;
_LL352: _temp351=( void*) _temp340.color; goto _LL350; _LL350: _temp349=( struct
Cyc_Dict_T*) _temp340.left; goto _LL348; _LL348: _temp347=( struct Cyc_Dict_T*)
_temp340.right; goto _LL342; _LL342: _temp341=( struct _tuple0) _temp340.key_val;
_LL346: _temp345= _temp341.f1; goto _LL344; _LL344: _temp343= _temp341.f2; goto
_LL339; _LL339: { struct Cyc_Dict_T* new_left= _temp349 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp356= _temp349; if(
_temp356 == 0){ _throw( Null_Exception);} _temp356;})); void* new_val= f( env,
_temp343); struct Cyc_Dict_T* new_right= _temp347 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp355= _temp347; if(
_temp355 == 0){ _throw( Null_Exception);} _temp355;})); return({ struct Cyc_Dict_T*
_temp353=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp353->color=(
void*) _temp351; _temp353->left= new_left; _temp353->right= new_right; _temp353->key_val=({
struct _tuple0 _temp354; _temp354.f1= _temp345; _temp354.f2= new_val; _temp354;});
_temp353;});}} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp357=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp357->rel= d->rel; _temp357->t= 0; _temp357;});} return({ struct Cyc_Dict_Dict*
_temp358=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp358->rel= d->rel; _temp358->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp359= d->t; if( _temp359 == 0){ _throw(
Null_Exception);} _temp359;})); _temp358;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Dict_Absent_struct* _temp360=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp360[ 0]=({ struct Cyc_Dict_Absent_struct
_temp361; _temp361.tag= Cyc_Dict_Absent_tag; _temp361;}); _temp360;}));} return({
struct _tuple0* _temp362=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp362->f1=({ struct Cyc_Dict_T* _temp364= d->t; if( _temp364 == 0){ _throw(
Null_Exception);} _temp364->key_val;}).f1; _temp362->f2=({ struct Cyc_Dict_T*
_temp363= d->t; if( _temp363 == 0){ _throw( Null_Exception);} _temp363->key_val;}).f2;
_temp362;});} static int Cyc_Dict_forall_tree_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp367; struct _tuple0
_temp368; void* _temp370; void* _temp372; struct Cyc_Dict_T* _temp374; struct
Cyc_Dict_T* _temp376; void* _temp378; struct Cyc_Dict_T* _temp365= t; _temp367=*
_temp365; _LL379: _temp378=( void*) _temp367.color; goto _LL377; _LL377:
_temp376=( struct Cyc_Dict_T*) _temp367.left; goto _LL375; _LL375: _temp374=(
struct Cyc_Dict_T*) _temp367.right; goto _LL369; _LL369: _temp368=( struct
_tuple0) _temp367.key_val; _LL373: _temp372= _temp368.f1; goto _LL371; _LL371:
_temp370= _temp368.f2; goto _LL366; _LL366: return(( _temp376 == 0? 1:(( int(*)(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp380= _temp376; if(
_temp380 == 0){ _throw( Null_Exception);} _temp380;})))? f( env, _temp372,
_temp370): 0)? _temp374 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp381= _temp374; if( _temp381 == 0){ _throw(
Null_Exception);} _temp381;})): 0;} int Cyc_Dict_forall_c( int(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return 1;} return((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp382= d->t; if( _temp382
== 0){ _throw( Null_Exception);} _temp382;}));} struct _tuple4{ int(* f1)( void*,
void*, void*); struct Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f(
struct _tuple4* env, void* a, void* b){ struct _tuple4 _temp385; struct Cyc_Dict_Dict*
_temp386; int(* _temp388)( void*, void*, void*); struct _tuple4* _temp383= env;
_temp385=* _temp383; _LL389: _temp388= _temp385.f1; goto _LL387; _LL387:
_temp386= _temp385.f2; goto _LL384; _LL384: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp386, a)){ return _temp388( a, b,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp386, a));} return 1;}
int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct _tuple4 env=({ struct _tuple4 _temp390;
_temp390.f1= f; _temp390.f2= d2; _temp390;}); return(( int(*)( int(* f)( struct
_tuple4*, void*, void*), struct _tuple4* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)((
int(*)( struct _tuple4* env, void* a, void* b)) Cyc_Dict_forall_intersect_f,&
env, d1);} static struct Cyc_Dict_Dict* Cyc_Dict_union_f( void*(* f)( void*,
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
Cyc_Dict_Dict* d1)) Cyc_Dict_union_f, f, d2, d1);} struct _tuple5{ void*(* f1)(
void*, void*); struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; } ; static
void Cyc_Dict_intersect_f( struct _tuple5* trip, void* a, void* b){ struct
_tuple5 _temp393; struct Cyc_Dict_Dict* _temp394; struct Cyc_Dict_Dict**
_temp396; struct Cyc_Dict_Dict* _temp397; void*(* _temp399)( void*, void*);
struct _tuple5* _temp391= trip; _temp393=* _temp391; _LL400: _temp399= _temp393.f1;
goto _LL398; _LL398: _temp397= _temp393.f2; goto _LL395; _LL395: _temp394=
_temp393.f3; _temp396=&(* _temp391).f3; goto _LL392; _LL392: if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp397, a)){* _temp396=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(*
_temp396, a, _temp399((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp397, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct _tuple5 env=({ struct _tuple5 _temp401; _temp401.f1= f;
_temp401.f2= d1; _temp401.f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*,
void*))) Cyc_Dict_empty)( d1->rel); _temp401;});(( void(*)( void(* f)( struct
_tuple5*, void*, void*), struct _tuple5* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple5* trip, void* a, void* b)) Cyc_Dict_intersect_f,& env, d2);
return env.f3;}} struct _tuple6{ void*(* f1)( void*, void*, void*); void* f2;
struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static void Cyc_Dict_intersect_c_f(
struct _tuple6* quad_t, void* a, void* b){ struct _tuple6 _temp404; struct Cyc_Dict_Dict*
_temp405; struct Cyc_Dict_Dict** _temp407; struct Cyc_Dict_Dict* _temp408; void*
_temp410; void*(* _temp412)( void*, void*, void*); struct _tuple6* _temp402=
quad_t; _temp404=* _temp402; _LL413: _temp412= _temp404.f1; goto _LL411; _LL411:
_temp410= _temp404.f2; goto _LL409; _LL409: _temp408= _temp404.f3; goto _LL406;
_LL406: _temp405= _temp404.f4; _temp407=&(* _temp402).f4; goto _LL403; _LL403:
if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp408, a)){*
_temp407=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)(* _temp407, a, _temp412( _temp410,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp408, a), b));}} struct Cyc_Dict_Dict*
Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct _tuple6 env2=({
struct _tuple6 _temp414; _temp414.f1= f; _temp414.f2= env; _temp414.f3= d1;
_temp414.f4=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
d1->rel); _temp414;});(( void(*)( void(* f)( struct _tuple6*, void*, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple6* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,& env2, d2);
return env2.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k, void*
v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp415=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp415->hd=(
void*)({ struct _tuple0* _temp416=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp416->f1= k; _temp416->f2= v; _temp416;}); _temp415->tl= accum;
_temp415;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
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
_tuple7 _temp419; void* _temp420; int(* _temp422)( void*, void*, void*); struct
_tuple7* _temp417= env; _temp419=* _temp417; _LL423: _temp422= _temp419.f1; goto
_LL421; _LL421: _temp420= _temp419.f2; goto _LL418; _LL418: return _temp422(
_temp420, x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct
_tuple7 env=({ struct _tuple7 _temp424; _temp424.f1= f; _temp424.f2= f_env;
_temp424;}); return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)(
struct _tuple7*, void*, void*, struct Cyc_Dict_Dict*), struct _tuple7* env,
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_c_f,& env, d,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( d->rel));} static int Cyc_Dict_difference_f(
struct Cyc_Dict_Dict* d, void* x, void* y){ return !(( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, x);} struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Dict_Dict*, void*, void*), struct Cyc_Dict_Dict* f_env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(( int(*)( struct Cyc_Dict_Dict* d,
void* x, void* y)) Cyc_Dict_difference_f, d2, d1);} struct _tuple8{ int(* f1)(
void*, void*); void* f2; } ; static int Cyc_Dict_delete_f( struct _tuple8* env,
void* x, void* y){ struct _tuple8 _temp427; void* _temp428; int(* _temp430)(
void*, void*); struct _tuple8* _temp425= env; _temp427=* _temp425; _LL431:
_temp430= _temp427.f1; goto _LL429; _LL429: _temp428= _temp427.f2; goto _LL426;
_LL426: return _temp430( _temp428, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict* d, void* x){ if((( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup_opt)( d, x) == 0){ return d;}{ struct _tuple8 env=({
struct _tuple8 _temp432; _temp432.f1= d->rel; _temp432.f2= x; _temp432;});
return(( struct Cyc_Dict_Dict*(*)( int(* f)( struct _tuple8*, void*, void*),
struct _tuple8* f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(( int(*)(
struct _tuple8* env, void* x, void* y)) Cyc_Dict_delete_f,& env, d);}}