#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct Cyc_Dict_T*
f2; struct Cyc_Dict_T* f3; struct _tuple0 f4; } ; typedef int Cyc_ptrdiff_t;
typedef unsigned int Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef
unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short Cyc_u_short;
typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef
unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int
Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int
tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec
it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
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
Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T*
_temp4= t; if( _temp4 == 0){ _throw( Null_Exception);} _temp4->key_val;}).f1);
if( i < 0){ t=({ struct Cyc_Dict_T* _temp5= t; if( _temp5 == 0){ _throw(
Null_Exception);} _temp5->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T*
_temp6= t; if( _temp6 == 0){ _throw( Null_Exception);} _temp6->right;});} else{
return 1;}}} return 0;} void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void*
key){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t !=
0) { int i= rel( key,({ struct Cyc_Dict_T* _temp7= t; if( _temp7 == 0){ _throw(
Null_Exception);} _temp7->key_val;}).f1); if( i < 0){ t=({ struct Cyc_Dict_T*
_temp8= t; if( _temp8 == 0){ _throw( Null_Exception);} _temp8->left;});} else{
if( i > 0){ t=({ struct Cyc_Dict_T* _temp9= t; if( _temp9 == 0){ _throw(
Null_Exception);} _temp9->right;});} else{ return({ struct Cyc_Dict_T* _temp10=
t; if( _temp10 == 0){ _throw( Null_Exception);} _temp10->key_val;}).f2;}}}( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Dict_Absent_struct* _temp11=(
struct Cyc_Dict_Absent_struct*) GC_malloc( sizeof( struct Cyc_Dict_Absent_struct));
_temp11[ 0]=({ struct Cyc_Dict_Absent_struct _temp12; _temp12.tag= Cyc_Dict_Absent_tag;
_temp12;}); _temp11;}));} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* key){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t;
while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T* _temp13= t; if( _temp13
== 0){ _throw( Null_Exception);} _temp13->key_val;}).f1); if( i < 0){ t=({
struct Cyc_Dict_T* _temp14= t; if( _temp14 == 0){ _throw( Null_Exception);}
_temp14->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T* _temp15= t; if(
_temp15 == 0){ _throw( Null_Exception);} _temp15->right;});} else{ return({
struct Cyc_Core_Opt* _temp16=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp16->v=( void*)({ struct Cyc_Dict_T* _temp17= t; if( _temp17
== 0){ _throw( Null_Exception);} _temp17->key_val;}).f2; _temp16;});}}} return 0;}
int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void** ans_place){
int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t; while( t != 0) {
int i= rel( key,({ struct Cyc_Dict_T* _temp18= t; if( _temp18 == 0){ _throw(
Null_Exception);} _temp18->key_val;}).f1); if( i < 0){ t=({ struct Cyc_Dict_T*
_temp19= t; if( _temp19 == 0){ _throw( Null_Exception);} _temp19->left;});}
else{ if( i > 0){ t=({ struct Cyc_Dict_T* _temp20= t; if( _temp20 == 0){ _throw(
Null_Exception);} _temp20->right;});} else{* ans_place=({ struct Cyc_Dict_T*
_temp21= t; if( _temp21 == 0){ _throw( Null_Exception);} _temp21->key_val;}).f2;
return 1;}}} return 0;} static struct Cyc_Dict_T* Cyc_Dict_balance( struct
_tuple1 quad_t){ struct _tuple1 _temp22= quad_t; struct _tuple0 _temp34; struct
Cyc_Dict_T* _temp36; struct Cyc_Dict_T* _temp38; struct Cyc_Dict_T _temp40;
struct _tuple0 _temp41; struct Cyc_Dict_T* _temp43; struct Cyc_Dict_T* _temp45;
struct Cyc_Dict_T _temp47; struct _tuple0 _temp48; struct Cyc_Dict_T* _temp50;
struct Cyc_Dict_T* _temp52; void* _temp54; void* _temp56; void* _temp58; struct
_tuple0 _temp60; struct Cyc_Dict_T* _temp62; struct Cyc_Dict_T* _temp64; struct
Cyc_Dict_T _temp66; struct _tuple0 _temp67; struct Cyc_Dict_T* _temp69; struct
Cyc_Dict_T _temp71; struct _tuple0 _temp72; struct Cyc_Dict_T* _temp74; struct
Cyc_Dict_T* _temp76; void* _temp78; struct Cyc_Dict_T* _temp80; void* _temp82;
void* _temp84; struct _tuple0 _temp86; struct Cyc_Dict_T* _temp88; struct Cyc_Dict_T
_temp90; struct _tuple0 _temp91; struct Cyc_Dict_T* _temp93; struct Cyc_Dict_T*
_temp95; struct Cyc_Dict_T _temp97; struct _tuple0 _temp98; struct Cyc_Dict_T*
_temp100; struct Cyc_Dict_T* _temp102; void* _temp104; void* _temp106; struct
Cyc_Dict_T* _temp108; void* _temp110; struct _tuple0 _temp112; struct Cyc_Dict_T*
_temp114; struct Cyc_Dict_T _temp116; struct _tuple0 _temp117; struct Cyc_Dict_T*
_temp119; struct Cyc_Dict_T _temp121; struct _tuple0 _temp122; struct Cyc_Dict_T*
_temp124; struct Cyc_Dict_T* _temp126; void* _temp128; struct Cyc_Dict_T*
_temp130; void* _temp132; struct Cyc_Dict_T* _temp134; void* _temp136; struct
_tuple0 _temp138; struct Cyc_Dict_T* _temp140; struct Cyc_Dict_T* _temp142; void*
_temp144; _LL24: _LL59: _temp58= _temp22.f1; if(( int) _temp58 == Cyc_Dict_B){
goto _LL39;} else{ goto _LL26;} _LL39: _temp38= _temp22.f2; if( _temp38 == 0){
goto _LL26;} else{ _temp40=* _temp38; _LL57: _temp56=( void*) _temp40.color; if((
int) _temp56 == Cyc_Dict_R){ goto _LL46;} else{ goto _LL26;} _LL46: _temp45=(
struct Cyc_Dict_T*) _temp40.left; if( _temp45 == 0){ goto _LL26;} else{ _temp47=*
_temp45; _LL55: _temp54=( void*) _temp47.color; if(( int) _temp54 == Cyc_Dict_R){
goto _LL53;} else{ goto _LL26;} _LL53: _temp52=( struct Cyc_Dict_T*) _temp47.left;
goto _LL51; _LL51: _temp50=( struct Cyc_Dict_T*) _temp47.right; goto _LL49;
_LL49: _temp48=( struct _tuple0) _temp47.key_val; goto _LL44;} _LL44: _temp43=(
struct Cyc_Dict_T*) _temp40.right; goto _LL42; _LL42: _temp41=( struct _tuple0)
_temp40.key_val; goto _LL37;} _LL37: _temp36= _temp22.f3; goto _LL35; _LL35:
_temp34= _temp22.f4; goto _LL25; _LL26: _LL85: _temp84= _temp22.f1; if(( int)
_temp84 == Cyc_Dict_B){ goto _LL65;} else{ goto _LL28;} _LL65: _temp64= _temp22.f2;
if( _temp64 == 0){ goto _LL28;} else{ _temp66=* _temp64; _LL83: _temp82=( void*)
_temp66.color; if(( int) _temp82 == Cyc_Dict_R){ goto _LL81;} else{ goto _LL28;}
_LL81: _temp80=( struct Cyc_Dict_T*) _temp66.left; goto _LL70; _LL70: _temp69=(
struct Cyc_Dict_T*) _temp66.right; if( _temp69 == 0){ goto _LL28;} else{ _temp71=*
_temp69; _LL79: _temp78=( void*) _temp71.color; if(( int) _temp78 == Cyc_Dict_R){
goto _LL77;} else{ goto _LL28;} _LL77: _temp76=( struct Cyc_Dict_T*) _temp71.left;
goto _LL75; _LL75: _temp74=( struct Cyc_Dict_T*) _temp71.right; goto _LL73;
_LL73: _temp72=( struct _tuple0) _temp71.key_val; goto _LL68;} _LL68: _temp67=(
struct _tuple0) _temp66.key_val; goto _LL63;} _LL63: _temp62= _temp22.f3; goto
_LL61; _LL61: _temp60= _temp22.f4; goto _LL27; _LL28: _LL111: _temp110= _temp22.f1;
if(( int) _temp110 == Cyc_Dict_B){ goto _LL109;} else{ goto _LL30;} _LL109:
_temp108= _temp22.f2; goto _LL89; _LL89: _temp88= _temp22.f3; if( _temp88 == 0){
goto _LL30;} else{ _temp90=* _temp88; _LL107: _temp106=( void*) _temp90.color;
if(( int) _temp106 == Cyc_Dict_R){ goto _LL96;} else{ goto _LL30;} _LL96:
_temp95=( struct Cyc_Dict_T*) _temp90.left; if( _temp95 == 0){ goto _LL30;}
else{ _temp97=* _temp95; _LL105: _temp104=( void*) _temp97.color; if(( int)
_temp104 == Cyc_Dict_R){ goto _LL103;} else{ goto _LL30;} _LL103: _temp102=(
struct Cyc_Dict_T*) _temp97.left; goto _LL101; _LL101: _temp100=( struct Cyc_Dict_T*)
_temp97.right; goto _LL99; _LL99: _temp98=( struct _tuple0) _temp97.key_val;
goto _LL94;} _LL94: _temp93=( struct Cyc_Dict_T*) _temp90.right; goto _LL92;
_LL92: _temp91=( struct _tuple0) _temp90.key_val; goto _LL87;} _LL87: _temp86=
_temp22.f4; goto _LL29; _LL30: _LL137: _temp136= _temp22.f1; if(( int) _temp136
== Cyc_Dict_B){ goto _LL135;} else{ goto _LL32;} _LL135: _temp134= _temp22.f2;
goto _LL115; _LL115: _temp114= _temp22.f3; if( _temp114 == 0){ goto _LL32;}
else{ _temp116=* _temp114; _LL133: _temp132=( void*) _temp116.color; if(( int)
_temp132 == Cyc_Dict_R){ goto _LL131;} else{ goto _LL32;} _LL131: _temp130=(
struct Cyc_Dict_T*) _temp116.left; goto _LL120; _LL120: _temp119=( struct Cyc_Dict_T*)
_temp116.right; if( _temp119 == 0){ goto _LL32;} else{ _temp121=* _temp119;
_LL129: _temp128=( void*) _temp121.color; if(( int) _temp128 == Cyc_Dict_R){
goto _LL127;} else{ goto _LL32;} _LL127: _temp126=( struct Cyc_Dict_T*) _temp121.left;
goto _LL125; _LL125: _temp124=( struct Cyc_Dict_T*) _temp121.right; goto _LL123;
_LL123: _temp122=( struct _tuple0) _temp121.key_val; goto _LL118;} _LL118:
_temp117=( struct _tuple0) _temp116.key_val; goto _LL113;} _LL113: _temp112=
_temp22.f4; goto _LL31; _LL32: _LL145: _temp144= _temp22.f1; goto _LL143; _LL143:
_temp142= _temp22.f2; goto _LL141; _LL141: _temp140= _temp22.f3; goto _LL139;
_LL139: _temp138= _temp22.f4; goto _LL33; _LL25: return({ struct Cyc_Dict_T*
_temp146=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp146->color=(
void*)(( void*) Cyc_Dict_R); _temp146->left=({ struct Cyc_Dict_T* _temp148=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp148->color=(
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
d->rel,({ struct _tuple0 _temp178; _temp178.f1= key; _temp178.f2= data; _temp178;}),
d->t);( void*)({ struct Cyc_Dict_T* _temp179= ans; if( _temp179 == 0){ _throw(
Null_Exception);} _temp179->color=( void*)(( void*) Cyc_Dict_B);}); return({
struct Cyc_Dict_Dict* _temp180=( struct Cyc_Dict_Dict*) GC_malloc( sizeof(
struct Cyc_Dict_Dict)); _temp180->rel= d->rel; _temp180->t= ans; _temp180;});}
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
_temp195= _temp193.f2; goto _LL191; _LL191: accum= f( _temp197, _temp195, accum);
if( _temp201 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*), struct
Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp205= _temp201; if( _temp205 == 0){ _throw(
Null_Exception);} _temp205;}), accum);} if( _temp199 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp206= _temp199; if( _temp206 ==
0){ _throw( Null_Exception);} _temp206;}), accum);} return accum;} void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t
== 0){ return accum;} return(( void*(*)( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp207= d->t; if( _temp207 == 0){ _throw( Null_Exception);}
_temp207;}), accum);} static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*,
void*, void*), void* env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T
_temp210; struct _tuple0 _temp211; void* _temp213; void* _temp215; struct Cyc_Dict_T*
_temp217; struct Cyc_Dict_T* _temp219; void* _temp221; struct Cyc_Dict_T*
_temp208= t; _temp210=* _temp208; _LL222: _temp221=( void*) _temp210.color; goto
_LL220; _LL220: _temp219=( struct Cyc_Dict_T*) _temp210.left; goto _LL218;
_LL218: _temp217=( struct Cyc_Dict_T*) _temp210.right; goto _LL212; _LL212:
_temp211=( struct _tuple0) _temp210.key_val; _LL216: _temp215= _temp211.f1; goto
_LL214; _LL214: _temp213= _temp211.f2; goto _LL209; _LL209: accum= f( env,
_temp215, _temp213, accum); if( _temp219 != 0){ accum=(( void*(*)( void*(* f)(
void*, void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp223= _temp219; if(
_temp223 == 0){ _throw( Null_Exception);} _temp223;}), accum);} if( _temp217 !=
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
_LL232; _LL232: _temp231= _temp229.f2; goto _LL227; _LL227: f( _temp233,
_temp231); if( _temp237 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp241=
_temp237; if( _temp241 == 0){ _throw( Null_Exception);} _temp241;}));} if(
_temp235 != 0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
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
_temp249= _temp247.f2; goto _LL245; _LL245: f( env, _temp251, _temp249); if(
_temp255 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp259= _temp255; if( _temp259 == 0){ _throw( Null_Exception);} _temp259;}));}
if( _temp253 != 0){(( void(*)( void*(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
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
_LL268; _LL268: _temp267= _temp265.f2; goto _LL263; _LL263: f( _temp269,
_temp267); if( _temp273 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T*
t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp277=
_temp273; if( _temp277 == 0){ _throw( Null_Exception);} _temp277;}));} if(
_temp271 != 0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
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
_temp285= _temp283.f2; goto _LL281; _LL281: f( env, _temp287, _temp285); if(
_temp291 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp295= _temp291; if( _temp295 == 0){ _throw( Null_Exception);} _temp295;}));}
if( _temp289 != 0){(( void(*)( void(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp296= _temp289; if( _temp296 == 0){ _throw(
Null_Exception);} _temp296;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp297= d->t; if( _temp297 == 0){
_throw( Null_Exception);} _temp297;}));}} struct Cyc_Dict_Iter2Env{ void(* f)(
void*, void*, void*); void* inner_env; struct Cyc_Dict_Dict* d2; } ; typedef
struct Cyc_Dict_Iter2Env Cyc_Dict_iter2_env_t; static void Cyc_Dict_iter2_c_f(
struct Cyc_Dict_Iter2Env* env, void* a, void* b1){ struct Cyc_Dict_Iter2Env
_temp300; struct Cyc_Dict_Dict* _temp301; void* _temp303; void(* _temp305)( void*,
void*, void*); struct Cyc_Dict_Iter2Env* _temp298= env; _temp300=* _temp298;
_LL306: _temp305=( void(*)( void*, void*, void*)) _temp300.f; goto _LL304;
_LL304: _temp303=( void*) _temp300.inner_env; goto _LL302; _LL302: _temp301=(
struct Cyc_Dict_Dict*) _temp300.d2; goto _LL299; _LL299: _temp305( _temp303, b1,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp301, a));}
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* inner_env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct Cyc_Dict_Iter2Env env=({
struct Cyc_Dict_Iter2Env _temp307; _temp307.f= f; _temp307.inner_env=( void*)
inner_env; _temp307.d2= d2; _temp307;});(( void(*)( void(* f)( struct Cyc_Dict_Iter2Env*,
void*, void*), struct Cyc_Dict_Iter2Env* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct Cyc_Dict_Iter2Env* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,&
env, d1);} static struct Cyc_Dict_T* Cyc_Dict_map_tree( void*(* f)( void*),
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp310; struct _tuple0 _temp311; void*
_temp313; void* _temp315; struct Cyc_Dict_T* _temp317; struct Cyc_Dict_T*
_temp319; void* _temp321; struct Cyc_Dict_T* _temp308= t; _temp310=* _temp308;
_LL322: _temp321=( void*) _temp310.color; goto _LL320; _LL320: _temp319=( struct
Cyc_Dict_T*) _temp310.left; goto _LL318; _LL318: _temp317=( struct Cyc_Dict_T*)
_temp310.right; goto _LL312; _LL312: _temp311=( struct _tuple0) _temp310.key_val;
_LL316: _temp315= _temp311.f1; goto _LL314; _LL314: _temp313= _temp311.f2; goto
_LL309; _LL309: { struct Cyc_Dict_T* new_left= _temp319 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp323= _temp319; if( _temp323 == 0){ _throw(
Null_Exception);} _temp323;})); struct Cyc_Dict_T* new_right= _temp317 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp324= _temp317; if( _temp324 ==
0){ _throw( Null_Exception);} _temp324;})); return({ struct Cyc_Dict_T* _temp325=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp325->color=(
void*) _temp321; _temp325->left= new_left; _temp325->right= new_right; _temp325->key_val=({
struct _tuple0 _temp326; _temp326.f1= _temp315; _temp326.f2= f( _temp313);
_temp326;}); _temp325;});}} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*),
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp327=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp327->rel= d->rel; _temp327->t= 0; _temp327;});} return({ struct Cyc_Dict_Dict*
_temp328=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp328->rel= d->rel; _temp328->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*),
struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp329= d->t; if( _temp329 == 0){ _throw( Null_Exception);} _temp329;}));
_temp328;});} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp332; struct
_tuple0 _temp333; void* _temp335; void* _temp337; struct Cyc_Dict_T* _temp339;
struct Cyc_Dict_T* _temp341; void* _temp343; struct Cyc_Dict_T* _temp330= t;
_temp332=* _temp330; _LL344: _temp343=( void*) _temp332.color; goto _LL342;
_LL342: _temp341=( struct Cyc_Dict_T*) _temp332.left; goto _LL340; _LL340:
_temp339=( struct Cyc_Dict_T*) _temp332.right; goto _LL334; _LL334: _temp333=(
struct _tuple0) _temp332.key_val; _LL338: _temp337= _temp333.f1; goto _LL336;
_LL336: _temp335= _temp333.f2; goto _LL331; _LL331: { struct Cyc_Dict_T*
new_left= _temp341 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp345= _temp341; if( _temp345 == 0){ _throw(
Null_Exception);} _temp345;})); struct Cyc_Dict_T* new_right= _temp339 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp346= _temp339; if( _temp346 == 0){ _throw( Null_Exception);} _temp346;}));
return({ struct Cyc_Dict_T* _temp347=( struct Cyc_Dict_T*) GC_malloc( sizeof(
struct Cyc_Dict_T)); _temp347->color=( void*) _temp343; _temp347->left= new_left;
_temp347->right= new_right; _temp347->key_val=({ struct _tuple0 _temp348;
_temp348.f1= _temp337; _temp348.f2= f( env, _temp335); _temp348;}); _temp347;});}}
struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp349=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp349->rel= d->rel; _temp349->t= 0; _temp349;});} return({ struct Cyc_Dict_Dict*
_temp350=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp350->rel= d->rel; _temp350->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp351= d->t; if( _temp351 == 0){ _throw(
Null_Exception);} _temp351;})); _temp350;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Dict_Absent_struct* _temp352=( struct Cyc_Dict_Absent_struct*)
GC_malloc( sizeof( struct Cyc_Dict_Absent_struct)); _temp352[ 0]=({ struct Cyc_Dict_Absent_struct
_temp353; _temp353.tag= Cyc_Dict_Absent_tag; _temp353;}); _temp352;}));} return({
struct _tuple0* _temp354=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp354->f1=({ struct Cyc_Dict_T* _temp356= d->t; if( _temp356 == 0){ _throw(
Null_Exception);} _temp356->key_val;}).f1; _temp354->f2=({ struct Cyc_Dict_T*
_temp355= d->t; if( _temp355 == 0){ _throw( Null_Exception);} _temp355->key_val;}).f2;
_temp354;});} static int Cyc_Dict_forall_tree_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp359; struct _tuple0
_temp360; void* _temp362; void* _temp364; struct Cyc_Dict_T* _temp366; struct
Cyc_Dict_T* _temp368; void* _temp370; struct Cyc_Dict_T* _temp357= t; _temp359=*
_temp357; _LL371: _temp370=( void*) _temp359.color; goto _LL369; _LL369:
_temp368=( struct Cyc_Dict_T*) _temp359.left; goto _LL367; _LL367: _temp366=(
struct Cyc_Dict_T*) _temp359.right; goto _LL361; _LL361: _temp360=( struct
_tuple0) _temp359.key_val; _LL365: _temp364= _temp360.f1; goto _LL363; _LL363:
_temp362= _temp360.f2; goto _LL358; _LL358: return( f( env, _temp364, _temp362)?
_temp368 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void* env, struct
Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct
Cyc_Dict_T* _temp372= _temp368; if( _temp372 == 0){ _throw( Null_Exception);}
_temp372;})): 0)? _temp366 == 0? 1:(( int(*)( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp373= _temp366; if( _temp373 == 0){ _throw(
Null_Exception);} _temp373;})): 0;} int Cyc_Dict_forall_c( int(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return 1;} return((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp374= d->t; if( _temp374
== 0){ _throw( Null_Exception);} _temp374;}));} struct Cyc_Dict_ForallIntEnv{
int(* f)( void*, void*, void*); struct Cyc_Dict_Dict* d1; } ; typedef struct Cyc_Dict_ForallIntEnv
Cyc_Dict_forall_int_env_t; static int Cyc_Dict_forall_intersect_f( struct Cyc_Dict_ForallIntEnv*
env, void* a, void* b){ struct Cyc_Dict_ForallIntEnv _temp377; struct Cyc_Dict_Dict*
_temp378; int(* _temp380)( void*, void*, void*); struct Cyc_Dict_ForallIntEnv*
_temp375= env; _temp377=* _temp375; _LL381: _temp380=( int(*)( void*, void*,
void*)) _temp377.f; goto _LL379; _LL379: _temp378=( struct Cyc_Dict_Dict*)
_temp377.d1; goto _LL376; _LL376: if((( int(*)( struct Cyc_Dict_Dict* d, void*
key)) Cyc_Dict_member)( _temp378, a)){ return _temp380( a, b,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp378, a));} return 1;} int
Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct Cyc_Dict_ForallIntEnv env=({ struct Cyc_Dict_ForallIntEnv
_temp382; _temp382.f= f; _temp382.d1= d2; _temp382;}); return(( int(*)( int(* f)(
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
struct Cyc_Dict_IntArg _temp385; struct Cyc_Dict_Dict* _temp386; struct Cyc_Dict_Dict**
_temp388; struct Cyc_Dict_Dict* _temp389; void*(* _temp391)( void*, void*);
struct Cyc_Dict_IntArg* _temp383= trip; _temp385=* _temp383; _LL392: _temp391=(
void*(*)( void*, void*)) _temp385.f; goto _LL390; _LL390: _temp389=( struct Cyc_Dict_Dict*)
_temp385.d1; goto _LL387; _LL387: _temp386=( struct Cyc_Dict_Dict*) _temp385.d2;
_temp388=&(* _temp383).d2; goto _LL384; _LL384: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp389, a)){* _temp388=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp388, a,
_temp391((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp389, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct Cyc_Dict_IntArg env=({ struct Cyc_Dict_IntArg _temp393;
_temp393.f= f; _temp393.d1= d1; _temp393.d2=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( void*, void*))) Cyc_Dict_empty)( d1->rel); _temp393;});(( void(*)( void(*
f)( struct Cyc_Dict_IntArg*, void*, void*), struct Cyc_Dict_IntArg* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_Dict_IntArg* trip,
void* a, void* b)) Cyc_Dict_intersect_f,& env, d2); return env.d2;}} struct Cyc_Dict_IntArgC{
void*(* f)( void*, void*, void*); void* env; struct Cyc_Dict_Dict* d1; struct
Cyc_Dict_Dict* d2; } ; typedef struct Cyc_Dict_IntArgC Cyc_Dict_intargc_t;
static void Cyc_Dict_intersectc_f( struct Cyc_Dict_IntArgC* quad_t, void* a,
void* b){ struct Cyc_Dict_IntArgC _temp396; struct Cyc_Dict_Dict* _temp397;
struct Cyc_Dict_Dict** _temp399; struct Cyc_Dict_Dict* _temp400; void* _temp402;
void*(* _temp404)( void*, void*, void*); struct Cyc_Dict_IntArgC* _temp394=
quad_t; _temp396=* _temp394; _LL405: _temp404=( void*(*)( void*, void*, void*))
_temp396.f; goto _LL403; _LL403: _temp402=( void*) _temp396.env; goto _LL401;
_LL401: _temp400=( struct Cyc_Dict_Dict*) _temp396.d1; goto _LL398; _LL398:
_temp397=( struct Cyc_Dict_Dict*) _temp396.d2; _temp399=&(* _temp394).d2; goto
_LL395; _LL395: if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
_temp400, a)){* _temp399=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)(* _temp399, a, _temp404( _temp402,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp400, a), b));}}
struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct Cyc_Dict_IntArgC env2=({ struct Cyc_Dict_IntArgC _temp406;
_temp406.f= f; _temp406.env=( void*) env; _temp406.d1= d1; _temp406.d2=(( struct
Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)( d1->rel);
_temp406;});(( void(*)( void(* f)( struct Cyc_Dict_IntArgC*, void*, void*),
struct Cyc_Dict_IntArgC* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct Cyc_Dict_IntArgC* quad_t, void* a, void* b)) Cyc_Dict_intersectc_f,& env2,
d2); return env2.d2;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k,
void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp407=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp407->hd=(
void*)({ struct _tuple0* _temp408=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp408->f1= k; _temp408->f2= v; _temp408;}); _temp407->tl= accum;
_temp407;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
return(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( void*, void*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)(( struct Cyc_List_List*(*)( void* k, void* v, struct Cyc_List_List*
accum)) Cyc_Dict_to_list_f, d, 0);}