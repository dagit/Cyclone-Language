#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
_throw(( void*) Cyc_Dict_Absent);} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key){ int(* rel)( void*, void*)= d->rel; struct
Cyc_Dict_T* t= d->t; while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T*
_temp15= t; if( _temp15 == 0){ _throw( Null_Exception);} _temp15->key_val;}).f1);
if( i < 0){ t=({ struct Cyc_Dict_T* _temp11= t; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T*
_temp12= t; if( _temp12 == 0){ _throw( Null_Exception);} _temp12->right;});}
else{ return({ struct Cyc_Core_Opt* _temp13=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp13->v=( void*)({ struct Cyc_Dict_T* _temp14=
t; if( _temp14 == 0){ _throw( Null_Exception);} _temp14->key_val;}).f2; _temp13;});}}}
return 0;} int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void**
ans_place){ int(* rel)( void*, void*)= d->rel; struct Cyc_Dict_T* t= d->t;
while( t != 0) { int i= rel( key,({ struct Cyc_Dict_T* _temp19= t; if( _temp19
== 0){ _throw( Null_Exception);} _temp19->key_val;}).f1); if( i < 0){ t=({
struct Cyc_Dict_T* _temp16= t; if( _temp16 == 0){ _throw( Null_Exception);}
_temp16->left;});} else{ if( i > 0){ t=({ struct Cyc_Dict_T* _temp17= t; if(
_temp17 == 0){ _throw( Null_Exception);} _temp17->right;});} else{* ans_place=({
struct Cyc_Dict_T* _temp18= t; if( _temp18 == 0){ _throw( Null_Exception);}
_temp18->key_val;}).f2; return 1;}}} return 0;} struct _tuple1{ void* f1; struct
Cyc_Dict_T* f2; struct Cyc_Dict_T* f3; struct _tuple0 f4; } ; static struct Cyc_Dict_T*
Cyc_Dict_balance( struct _tuple1 quad_t){ struct _tuple1 _temp20= quad_t; struct
_tuple0 _temp32; struct Cyc_Dict_T* _temp34; struct Cyc_Dict_T* _temp36; struct
Cyc_Dict_T _temp38; struct _tuple0 _temp39; struct Cyc_Dict_T* _temp41; struct
Cyc_Dict_T* _temp43; struct Cyc_Dict_T _temp45; struct _tuple0 _temp46; struct
Cyc_Dict_T* _temp48; struct Cyc_Dict_T* _temp50; void* _temp52; void* _temp54;
void* _temp56; struct _tuple0 _temp58; struct Cyc_Dict_T* _temp60; struct Cyc_Dict_T*
_temp62; struct Cyc_Dict_T _temp64; struct _tuple0 _temp65; struct Cyc_Dict_T*
_temp67; struct Cyc_Dict_T _temp69; struct _tuple0 _temp70; struct Cyc_Dict_T*
_temp72; struct Cyc_Dict_T* _temp74; void* _temp76; struct Cyc_Dict_T* _temp78;
void* _temp80; void* _temp82; struct _tuple0 _temp84; struct Cyc_Dict_T* _temp86;
struct Cyc_Dict_T _temp88; struct _tuple0 _temp89; struct Cyc_Dict_T* _temp91;
struct Cyc_Dict_T* _temp93; struct Cyc_Dict_T _temp95; struct _tuple0 _temp96;
struct Cyc_Dict_T* _temp98; struct Cyc_Dict_T* _temp100; void* _temp102; void*
_temp104; struct Cyc_Dict_T* _temp106; void* _temp108; struct _tuple0 _temp110;
struct Cyc_Dict_T* _temp112; struct Cyc_Dict_T _temp114; struct _tuple0 _temp115;
struct Cyc_Dict_T* _temp117; struct Cyc_Dict_T _temp119; struct _tuple0 _temp120;
struct Cyc_Dict_T* _temp122; struct Cyc_Dict_T* _temp124; void* _temp126; struct
Cyc_Dict_T* _temp128; void* _temp130; struct Cyc_Dict_T* _temp132; void*
_temp134; struct _tuple0 _temp136; struct Cyc_Dict_T* _temp138; struct Cyc_Dict_T*
_temp140; void* _temp142; _LL22: _LL57: _temp56= _temp20.f1; if( _temp56 ==(
void*) Cyc_Dict_B){ goto _LL37;} else{ goto _LL24;} _LL37: _temp36= _temp20.f2;
if( _temp36 == 0){ goto _LL24;} else{ _temp38=* _temp36; _LL55: _temp54=( void*)
_temp38.color; if( _temp54 ==( void*) Cyc_Dict_R){ goto _LL44;} else{ goto _LL24;}
_LL44: _temp43=( struct Cyc_Dict_T*) _temp38.left; if( _temp43 == 0){ goto _LL24;}
else{ _temp45=* _temp43; _LL53: _temp52=( void*) _temp45.color; if( _temp52 ==(
void*) Cyc_Dict_R){ goto _LL51;} else{ goto _LL24;} _LL51: _temp50=( struct Cyc_Dict_T*)
_temp45.left; goto _LL49; _LL49: _temp48=( struct Cyc_Dict_T*) _temp45.right;
goto _LL47; _LL47: _temp46=( struct _tuple0) _temp45.key_val; goto _LL42;} _LL42:
_temp41=( struct Cyc_Dict_T*) _temp38.right; goto _LL40; _LL40: _temp39=( struct
_tuple0) _temp38.key_val; goto _LL35;} _LL35: _temp34= _temp20.f3; goto _LL33;
_LL33: _temp32= _temp20.f4; goto _LL23; _LL24: _LL83: _temp82= _temp20.f1; if(
_temp82 ==( void*) Cyc_Dict_B){ goto _LL63;} else{ goto _LL26;} _LL63: _temp62=
_temp20.f2; if( _temp62 == 0){ goto _LL26;} else{ _temp64=* _temp62; _LL81:
_temp80=( void*) _temp64.color; if( _temp80 ==( void*) Cyc_Dict_R){ goto _LL79;}
else{ goto _LL26;} _LL79: _temp78=( struct Cyc_Dict_T*) _temp64.left; goto _LL68;
_LL68: _temp67=( struct Cyc_Dict_T*) _temp64.right; if( _temp67 == 0){ goto
_LL26;} else{ _temp69=* _temp67; _LL77: _temp76=( void*) _temp69.color; if(
_temp76 ==( void*) Cyc_Dict_R){ goto _LL75;} else{ goto _LL26;} _LL75: _temp74=(
struct Cyc_Dict_T*) _temp69.left; goto _LL73; _LL73: _temp72=( struct Cyc_Dict_T*)
_temp69.right; goto _LL71; _LL71: _temp70=( struct _tuple0) _temp69.key_val;
goto _LL66;} _LL66: _temp65=( struct _tuple0) _temp64.key_val; goto _LL61;}
_LL61: _temp60= _temp20.f3; goto _LL59; _LL59: _temp58= _temp20.f4; goto _LL25;
_LL26: _LL109: _temp108= _temp20.f1; if( _temp108 ==( void*) Cyc_Dict_B){ goto
_LL107;} else{ goto _LL28;} _LL107: _temp106= _temp20.f2; goto _LL87; _LL87:
_temp86= _temp20.f3; if( _temp86 == 0){ goto _LL28;} else{ _temp88=* _temp86;
_LL105: _temp104=( void*) _temp88.color; if( _temp104 ==( void*) Cyc_Dict_R){
goto _LL94;} else{ goto _LL28;} _LL94: _temp93=( struct Cyc_Dict_T*) _temp88.left;
if( _temp93 == 0){ goto _LL28;} else{ _temp95=* _temp93; _LL103: _temp102=( void*)
_temp95.color; if( _temp102 ==( void*) Cyc_Dict_R){ goto _LL101;} else{ goto
_LL28;} _LL101: _temp100=( struct Cyc_Dict_T*) _temp95.left; goto _LL99; _LL99:
_temp98=( struct Cyc_Dict_T*) _temp95.right; goto _LL97; _LL97: _temp96=( struct
_tuple0) _temp95.key_val; goto _LL92;} _LL92: _temp91=( struct Cyc_Dict_T*)
_temp88.right; goto _LL90; _LL90: _temp89=( struct _tuple0) _temp88.key_val;
goto _LL85;} _LL85: _temp84= _temp20.f4; goto _LL27; _LL28: _LL135: _temp134=
_temp20.f1; if( _temp134 ==( void*) Cyc_Dict_B){ goto _LL133;} else{ goto _LL30;}
_LL133: _temp132= _temp20.f2; goto _LL113; _LL113: _temp112= _temp20.f3; if(
_temp112 == 0){ goto _LL30;} else{ _temp114=* _temp112; _LL131: _temp130=( void*)
_temp114.color; if( _temp130 ==( void*) Cyc_Dict_R){ goto _LL129;} else{ goto
_LL30;} _LL129: _temp128=( struct Cyc_Dict_T*) _temp114.left; goto _LL118;
_LL118: _temp117=( struct Cyc_Dict_T*) _temp114.right; if( _temp117 == 0){ goto
_LL30;} else{ _temp119=* _temp117; _LL127: _temp126=( void*) _temp119.color; if(
_temp126 ==( void*) Cyc_Dict_R){ goto _LL125;} else{ goto _LL30;} _LL125:
_temp124=( struct Cyc_Dict_T*) _temp119.left; goto _LL123; _LL123: _temp122=(
struct Cyc_Dict_T*) _temp119.right; goto _LL121; _LL121: _temp120=( struct
_tuple0) _temp119.key_val; goto _LL116;} _LL116: _temp115=( struct _tuple0)
_temp114.key_val; goto _LL111;} _LL111: _temp110= _temp20.f4; goto _LL29; _LL30:
_LL143: _temp142= _temp20.f1; goto _LL141; _LL141: _temp140= _temp20.f2; goto
_LL139; _LL139: _temp138= _temp20.f3; goto _LL137; _LL137: _temp136= _temp20.f4;
goto _LL31; _LL23: return({ struct Cyc_Dict_T* _temp144=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp144->color=( void*)(( void*) Cyc_Dict_R);
_temp144->left=({ struct Cyc_Dict_T* _temp146=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp146->color=( void*)(( void*) Cyc_Dict_B);
_temp146->left= _temp50; _temp146->right= _temp48; _temp146->key_val= _temp46;
_temp146;}); _temp144->right=({ struct Cyc_Dict_T* _temp145=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp145->color=( void*)(( void*) Cyc_Dict_B);
_temp145->left= _temp41; _temp145->right= _temp34; _temp145->key_val= _temp32;
_temp145;}); _temp144->key_val= _temp39; _temp144;}); _LL25: return({ struct Cyc_Dict_T*
_temp147=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp147->color=(
void*)(( void*) Cyc_Dict_R); _temp147->left=({ struct Cyc_Dict_T* _temp149=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp149->color=(
void*)(( void*) Cyc_Dict_B); _temp149->left= _temp78; _temp149->right= _temp74;
_temp149->key_val= _temp65; _temp149;}); _temp147->right=({ struct Cyc_Dict_T*
_temp148=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp148->color=(
void*)(( void*) Cyc_Dict_B); _temp148->left= _temp72; _temp148->right= _temp60;
_temp148->key_val= _temp58; _temp148;}); _temp147->key_val= _temp70; _temp147;});
_LL27: return({ struct Cyc_Dict_T* _temp150=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp150->color=( void*)(( void*) Cyc_Dict_R);
_temp150->left=({ struct Cyc_Dict_T* _temp152=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp152->color=( void*)(( void*) Cyc_Dict_B);
_temp152->left= _temp106; _temp152->right= _temp100; _temp152->key_val= _temp84;
_temp152;}); _temp150->right=({ struct Cyc_Dict_T* _temp151=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp151->color=( void*)(( void*) Cyc_Dict_B);
_temp151->left= _temp98; _temp151->right= _temp91; _temp151->key_val= _temp89;
_temp151;}); _temp150->key_val= _temp96; _temp150;}); _LL29: return({ struct Cyc_Dict_T*
_temp153=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp153->color=(
void*)(( void*) Cyc_Dict_R); _temp153->left=({ struct Cyc_Dict_T* _temp155=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp155->color=(
void*)(( void*) Cyc_Dict_B); _temp155->left= _temp132; _temp155->right= _temp128;
_temp155->key_val= _temp110; _temp155;}); _temp153->right=({ struct Cyc_Dict_T*
_temp154=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp154->color=(
void*)(( void*) Cyc_Dict_B); _temp154->left= _temp124; _temp154->right= _temp122;
_temp154->key_val= _temp120; _temp154;}); _temp153->key_val= _temp115; _temp153;});
_LL31: return({ struct Cyc_Dict_T* _temp156=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp156->color=( void*) _temp142; _temp156->left=
_temp140; _temp156->right= _temp138; _temp156->key_val= _temp136; _temp156;});
_LL21:;} static struct Cyc_Dict_T* Cyc_Dict_ins( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t){ struct Cyc_Dict_T* _temp157= t;
struct Cyc_Dict_T _temp163; struct _tuple0 _temp164; struct Cyc_Dict_T* _temp166;
struct Cyc_Dict_T* _temp168; void* _temp170; _LL159: if( _temp157 == 0){ goto
_LL160;} else{ goto _LL161;} _LL161: if( _temp157 == 0){ goto _LL158;} else{
_temp163=* _temp157; _LL171: _temp170=( void*) _temp163.color; goto _LL169;
_LL169: _temp168=( struct Cyc_Dict_T*) _temp163.left; goto _LL167; _LL167:
_temp166=( struct Cyc_Dict_T*) _temp163.right; goto _LL165; _LL165: _temp164=(
struct _tuple0) _temp163.key_val; goto _LL162;} _LL160: return({ struct Cyc_Dict_T*
_temp172=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp172->color=(
void*)(( void*) Cyc_Dict_R); _temp172->left= 0; _temp172->right= 0; _temp172->key_val=
key_val; _temp172;}); _LL162: { int i= rel( key_val.f1, _temp164.f1); if( i < 0){
return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t)) Cyc_Dict_balance)(({
struct _tuple1 _temp173; _temp173.f1= _temp170; _temp173.f2=(( struct Cyc_Dict_T*(*)(
int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
rel, key_val, _temp168); _temp173.f3= _temp166; _temp173.f4= _temp164; _temp173;}));}
else{ if( i > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t)) Cyc_Dict_balance)(({
struct _tuple1 _temp174; _temp174.f1= _temp170; _temp174.f2= _temp168; _temp174.f3=((
struct Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct
Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val, _temp166); _temp174.f4= _temp164;
_temp174;}));} else{ return({ struct Cyc_Dict_T* _temp175=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp175->color=( void*) _temp170;
_temp175->left= _temp168; _temp175->right= _temp166; _temp175->key_val= key_val;
_temp175;});}}} _LL158:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data){ struct Cyc_Dict_T* ans=(( struct Cyc_Dict_T*(*)( int(*
rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
d->rel,({ struct _tuple0 _temp178; _temp178.f1= key; _temp178.f2= data; _temp178;}),
d->t);( void*)({ struct Cyc_Dict_T* _temp176= ans; if( _temp176 == 0){ _throw(
Null_Exception);} _temp176->color=( void*)(( void*) Cyc_Dict_B);}); return({
struct Cyc_Dict_Dict* _temp177=( struct Cyc_Dict_Dict*) GC_malloc( sizeof(
struct Cyc_Dict_Dict)); _temp177->rel= d->rel; _temp177->t= ans; _temp177;});}
struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key,
void* data){ if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)(
d, key)){( void) _throw(( void*) Cyc_Dict_Absent);} return(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d, key, data);}
struct Cyc_Dict_Dict* Cyc_Dict_inserts( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
kds){ for( 0; kds != 0; kds=({ struct Cyc_List_List* _temp179= kds; if( _temp179
== 0){ _throw( Null_Exception);} _temp179->tl;})){ d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( d,({ struct
_tuple0* _temp181=( struct _tuple0*)({ struct Cyc_List_List* _temp180= kds; if(
_temp180 == 0){ _throw( Null_Exception);} _temp180->hd;}); unsigned int _temp182=
0; if( _temp182 >= 1u){ _throw( Null_Exception);} _temp181[ _temp182];}).f1,({
struct _tuple0* _temp184=( struct _tuple0*)({ struct Cyc_List_List* _temp183=
kds; if( _temp183 == 0){ _throw( Null_Exception);} _temp183->hd;}); unsigned int
_temp185= 0; if( _temp185 >= 1u){ _throw( Null_Exception);} _temp184[ _temp185];}).f2);}
return d;} static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp188; struct _tuple0
_temp189; void* _temp191; void* _temp193; struct Cyc_Dict_T* _temp195; struct
Cyc_Dict_T* _temp197; void* _temp199; struct Cyc_Dict_T* _temp186= t; _temp188=*
_temp186; _LL200: _temp199=( void*) _temp188.color; goto _LL198; _LL198:
_temp197=( struct Cyc_Dict_T*) _temp188.left; goto _LL196; _LL196: _temp195=(
struct Cyc_Dict_T*) _temp188.right; goto _LL190; _LL190: _temp189=( struct
_tuple0) _temp188.key_val; _LL194: _temp193= _temp189.f1; goto _LL192; _LL192:
_temp191= _temp189.f2; goto _LL187; _LL187: if( _temp197 != 0){ accum=(( void*(*)(
void*(* f)( void*, void*, void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp201= _temp197; if( _temp201 ==
0){ _throw( Null_Exception);} _temp201;}), accum);} accum= f( _temp193, _temp191,
accum); if( _temp195 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*),
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp202= _temp195; if( _temp202 == 0){ _throw(
Null_Exception);} _temp202;}), accum);} return accum;} void* Cyc_Dict_fold( void*(*
f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){
return accum;} return(( void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp203= d->t; if( _temp203 == 0){ _throw( Null_Exception);} _temp203;}), accum);}
static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*, void*, void*), void*
env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp206; struct
_tuple0 _temp207; void* _temp209; void* _temp211; struct Cyc_Dict_T* _temp213;
struct Cyc_Dict_T* _temp215; void* _temp217; struct Cyc_Dict_T* _temp204= t;
_temp206=* _temp204; _LL218: _temp217=( void*) _temp206.color; goto _LL216;
_LL216: _temp215=( struct Cyc_Dict_T*) _temp206.left; goto _LL214; _LL214:
_temp213=( struct Cyc_Dict_T*) _temp206.right; goto _LL208; _LL208: _temp207=(
struct _tuple0) _temp206.key_val; _LL212: _temp211= _temp207.f1; goto _LL210;
_LL210: _temp209= _temp207.f2; goto _LL205; _LL205: if( _temp215 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct
Cyc_Dict_T* _temp219= _temp215; if( _temp219 == 0){ _throw( Null_Exception);}
_temp219;}), accum);} accum= f( env, _temp211, _temp209, accum); if( _temp213 !=
0){ accum=(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp220= _temp213; if( _temp220 == 0){ _throw(
Null_Exception);} _temp220;}), accum);} return accum;} void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp221= d->t; if( _temp221
== 0){ _throw( Null_Exception);} _temp221;}), accum);} static void Cyc_Dict_app_tree(
void*(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp224;
struct _tuple0 _temp225; void* _temp227; void* _temp229; struct Cyc_Dict_T*
_temp231; struct Cyc_Dict_T* _temp233; void* _temp235; struct Cyc_Dict_T*
_temp222= t; _temp224=* _temp222; _LL236: _temp235=( void*) _temp224.color; goto
_LL234; _LL234: _temp233=( struct Cyc_Dict_T*) _temp224.left; goto _LL232;
_LL232: _temp231=( struct Cyc_Dict_T*) _temp224.right; goto _LL226; _LL226:
_temp225=( struct _tuple0) _temp224.key_val; _LL230: _temp229= _temp225.f1; goto
_LL228; _LL228: _temp227= _temp225.f2; goto _LL223; _LL223: if( _temp233 != 0){((
void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp237= _temp233; if( _temp237 == 0){
_throw( Null_Exception);} _temp237;}));} f( _temp229, _temp227); if( _temp231 !=
0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp238= _temp231; if( _temp238 ==
0){ _throw( Null_Exception);} _temp238;}));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp239= d->t; if( _temp239 == 0){ _throw( Null_Exception);}
_temp239;}));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp242; struct _tuple0
_temp243; void* _temp245; void* _temp247; struct Cyc_Dict_T* _temp249; struct
Cyc_Dict_T* _temp251; void* _temp253; struct Cyc_Dict_T* _temp240= t; _temp242=*
_temp240; _LL254: _temp253=( void*) _temp242.color; goto _LL252; _LL252:
_temp251=( struct Cyc_Dict_T*) _temp242.left; goto _LL250; _LL250: _temp249=(
struct Cyc_Dict_T*) _temp242.right; goto _LL244; _LL244: _temp243=( struct
_tuple0) _temp242.key_val; _LL248: _temp247= _temp243.f1; goto _LL246; _LL246:
_temp245= _temp243.f2; goto _LL241; _LL241: if( _temp251 != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp255= _temp251; if(
_temp255 == 0){ _throw( Null_Exception);} _temp255;}));} f( env, _temp247,
_temp245); if( _temp249 != 0){(( void(*)( void*(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp256= _temp249; if( _temp256 == 0){ _throw(
Null_Exception);} _temp256;}));}} void Cyc_Dict_app_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp257= d->t; if( _temp257
== 0){ _throw( Null_Exception);} _temp257;}));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp260;
struct _tuple0 _temp261; void* _temp263; void* _temp265; struct Cyc_Dict_T*
_temp267; struct Cyc_Dict_T* _temp269; void* _temp271; struct Cyc_Dict_T*
_temp258= t; _temp260=* _temp258; _LL272: _temp271=( void*) _temp260.color; goto
_LL270; _LL270: _temp269=( struct Cyc_Dict_T*) _temp260.left; goto _LL268;
_LL268: _temp267=( struct Cyc_Dict_T*) _temp260.right; goto _LL262; _LL262:
_temp261=( struct _tuple0) _temp260.key_val; _LL266: _temp265= _temp261.f1; goto
_LL264; _LL264: _temp263= _temp261.f2; goto _LL259; _LL259: if( _temp269 != 0){((
void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp273= _temp269; if( _temp273 == 0){
_throw( Null_Exception);} _temp273;}));} f( _temp265, _temp263); if( _temp267 !=
0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp274= _temp267; if( _temp274 ==
0){ _throw( Null_Exception);} _temp274;}));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp275= d->t; if( _temp275 == 0){ _throw( Null_Exception);}
_temp275;}));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp278; struct _tuple0
_temp279; void* _temp281; void* _temp283; struct Cyc_Dict_T* _temp285; struct
Cyc_Dict_T* _temp287; void* _temp289; struct Cyc_Dict_T* _temp276= t; _temp278=*
_temp276; _LL290: _temp289=( void*) _temp278.color; goto _LL288; _LL288:
_temp287=( struct Cyc_Dict_T*) _temp278.left; goto _LL286; _LL286: _temp285=(
struct Cyc_Dict_T*) _temp278.right; goto _LL280; _LL280: _temp279=( struct
_tuple0) _temp278.key_val; _LL284: _temp283= _temp279.f1; goto _LL282; _LL282:
_temp281= _temp279.f2; goto _LL277; _LL277: if( _temp287 != 0){(( void(*)( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp291= _temp287; if(
_temp291 == 0){ _throw( Null_Exception);} _temp291;}));} f( env, _temp283,
_temp281); if( _temp285 != 0){(( void(*)( void(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp292= _temp285; if( _temp292 == 0){ _throw(
Null_Exception);} _temp292;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp293= d->t; if( _temp293 == 0){
_throw( Null_Exception);} _temp293;}));}} struct _tuple2{ void(* f1)( void*,
void*); struct Cyc_Dict_Dict* f2; } ; static void Cyc_Dict_iter2_f( struct
_tuple2* env, void* a, void* b1){ struct _tuple2 _temp296; struct Cyc_Dict_Dict*
_temp297; void(* _temp299)( void*, void*); struct _tuple2* _temp294= env;
_temp296=* _temp294; _LL300: _temp299= _temp296.f1; goto _LL298; _LL298:
_temp297= _temp296.f2; goto _LL295; _LL295: _temp299( b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp297, a));} void Cyc_Dict_iter2( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple2 env=({ struct _tuple2 _temp301; _temp301.f1= f; _temp301.f2= d2;
_temp301;});(( void(*)( void(* f)( struct _tuple2*, void*, void*), struct
_tuple2* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
_tuple2* env, void* a, void* b1)) Cyc_Dict_iter2_f,& env, d1);} struct _tuple3{
void(* f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void* f3; } ; static
void Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){ struct _tuple3
_temp304; void* _temp305; struct Cyc_Dict_Dict* _temp307; void(* _temp309)( void*,
void*, void*); struct _tuple3* _temp302= env; _temp304=* _temp302; _LL310:
_temp309= _temp304.f1; goto _LL308; _LL308: _temp307= _temp304.f2; goto _LL306;
_LL306: _temp305= _temp304.f3; goto _LL303; _LL303: _temp309( _temp305, b1,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp307, a));}
void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* inner_env, struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3 env=({ struct
_tuple3 _temp311; _temp311.f1= f; _temp311.f2= d2; _temp311.f3= inner_env;
_temp311;});(( void(*)( void(* f)( struct _tuple3*, void*, void*), struct
_tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct
_tuple3* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,& env, d1);} static struct
Cyc_Dict_T* Cyc_Dict_map_tree( void*(* f)( void*), struct Cyc_Dict_T* t){ struct
Cyc_Dict_T _temp314; struct _tuple0 _temp315; void* _temp317; void* _temp319;
struct Cyc_Dict_T* _temp321; struct Cyc_Dict_T* _temp323; void* _temp325; struct
Cyc_Dict_T* _temp312= t; _temp314=* _temp312; _LL326: _temp325=( void*) _temp314.color;
goto _LL324; _LL324: _temp323=( struct Cyc_Dict_T*) _temp314.left; goto _LL322;
_LL322: _temp321=( struct Cyc_Dict_T*) _temp314.right; goto _LL316; _LL316:
_temp315=( struct _tuple0) _temp314.key_val; _LL320: _temp319= _temp315.f1; goto
_LL318; _LL318: _temp317= _temp315.f2; goto _LL313; _LL313: { struct Cyc_Dict_T*
new_left= _temp323 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct
Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp330= _temp323; if( _temp330 == 0){ _throw( Null_Exception);} _temp330;}));
void* new_val= f( _temp317); struct Cyc_Dict_T* new_right= _temp321 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp329= _temp321; if( _temp329 ==
0){ _throw( Null_Exception);} _temp329;})); return({ struct Cyc_Dict_T* _temp327=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp327->color=(
void*) _temp325; _temp327->left= new_left; _temp327->right= new_right; _temp327->key_val=({
struct _tuple0 _temp328; _temp328.f1= _temp319; _temp328.f2= new_val; _temp328;});
_temp327;});}} struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct
Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict* _temp331=(
struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp331->rel=
d->rel; _temp331->t= 0; _temp331;});} return({ struct Cyc_Dict_Dict* _temp332=(
struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp332->rel=
d->rel; _temp332->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp333= d->t;
if( _temp333 == 0){ _throw( Null_Exception);} _temp333;})); _temp332;});} static
struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp336; struct _tuple0 _temp337; void*
_temp339; void* _temp341; struct Cyc_Dict_T* _temp343; struct Cyc_Dict_T*
_temp345; void* _temp347; struct Cyc_Dict_T* _temp334= t; _temp336=* _temp334;
_LL348: _temp347=( void*) _temp336.color; goto _LL346; _LL346: _temp345=( struct
Cyc_Dict_T*) _temp336.left; goto _LL344; _LL344: _temp343=( struct Cyc_Dict_T*)
_temp336.right; goto _LL338; _LL338: _temp337=( struct _tuple0) _temp336.key_val;
_LL342: _temp341= _temp337.f1; goto _LL340; _LL340: _temp339= _temp337.f2; goto
_LL335; _LL335: { struct Cyc_Dict_T* new_left= _temp345 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp352= _temp345; if(
_temp352 == 0){ _throw( Null_Exception);} _temp352;})); void* new_val= f( env,
_temp339); struct Cyc_Dict_T* new_right= _temp343 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp351= _temp343; if(
_temp351 == 0){ _throw( Null_Exception);} _temp351;})); return({ struct Cyc_Dict_T*
_temp349=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp349->color=(
void*) _temp347; _temp349->left= new_left; _temp349->right= new_right; _temp349->key_val=({
struct _tuple0 _temp350; _temp350.f1= _temp341; _temp350.f2= new_val; _temp350;});
_temp349;});}} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp353=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp353->rel= d->rel; _temp353->t= 0; _temp353;});} return({ struct Cyc_Dict_Dict*
_temp354=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp354->rel= d->rel; _temp354->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp355= d->t; if( _temp355 == 0){ _throw(
Null_Exception);} _temp355;})); _temp354;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
return({ struct _tuple0* _temp356=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp356->f1=({ struct Cyc_Dict_T* _temp358= d->t; if( _temp358 == 0){
_throw( Null_Exception);} _temp358->key_val;}).f1; _temp356->f2=({ struct Cyc_Dict_T*
_temp357= d->t; if( _temp357 == 0){ _throw( Null_Exception);} _temp357->key_val;}).f2;
_temp356;});} static int Cyc_Dict_forall_tree_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp361; struct _tuple0
_temp362; void* _temp364; void* _temp366; struct Cyc_Dict_T* _temp368; struct
Cyc_Dict_T* _temp370; void* _temp372; struct Cyc_Dict_T* _temp359= t; _temp361=*
_temp359; _LL373: _temp372=( void*) _temp361.color; goto _LL371; _LL371:
_temp370=( struct Cyc_Dict_T*) _temp361.left; goto _LL369; _LL369: _temp368=(
struct Cyc_Dict_T*) _temp361.right; goto _LL363; _LL363: _temp362=( struct
_tuple0) _temp361.key_val; _LL367: _temp366= _temp362.f1; goto _LL365; _LL365:
_temp364= _temp362.f2; goto _LL360; _LL360: return(( _temp370 == 0? 1:(( int(*)(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp374= _temp370; if(
_temp374 == 0){ _throw( Null_Exception);} _temp374;})))? f( env, _temp366,
_temp364): 0)? _temp368 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp375= _temp368; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375;})): 0;} int Cyc_Dict_forall_c( int(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return 1;} return((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp376= d->t; if( _temp376
== 0){ _throw( Null_Exception);} _temp376;}));} struct _tuple4{ int(* f1)( void*,
void*, void*); struct Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f(
struct _tuple4* env, void* a, void* b){ struct _tuple4 _temp379; struct Cyc_Dict_Dict*
_temp380; int(* _temp382)( void*, void*, void*); struct _tuple4* _temp377= env;
_temp379=* _temp377; _LL383: _temp382= _temp379.f1; goto _LL381; _LL381:
_temp380= _temp379.f2; goto _LL378; _LL378: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp380, a)){ return _temp382( a, b,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp380, a));} return 1;}
int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct _tuple4 env=({ struct _tuple4 _temp384;
_temp384.f1= f; _temp384.f2= d2; _temp384;}); return(( int(*)( int(* f)( struct
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
_tuple5 _temp387; struct Cyc_Dict_Dict* _temp388; struct Cyc_Dict_Dict**
_temp390; struct Cyc_Dict_Dict* _temp391; void*(* _temp393)( void*, void*);
struct _tuple5* _temp385= trip; _temp387=* _temp385; _LL394: _temp393= _temp387.f1;
goto _LL392; _LL392: _temp391= _temp387.f2; goto _LL389; _LL389: _temp388=
_temp387.f3; _temp390=&(* _temp385).f3; goto _LL386; _LL386: if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp391, a)){* _temp390=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(*
_temp390, a, _temp393((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp391, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct _tuple5 env=({ struct _tuple5 _temp395; _temp395.f1= f;
_temp395.f2= d1; _temp395.f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*,
void*))) Cyc_Dict_empty)( d1->rel); _temp395;});(( void(*)( void(* f)( struct
_tuple5*, void*, void*), struct _tuple5* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple5* trip, void* a, void* b)) Cyc_Dict_intersect_f,& env, d2);
return env.f3;}} struct _tuple6{ void*(* f1)( void*, void*, void*); void* f2;
struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static void Cyc_Dict_intersect_c_f(
struct _tuple6* quad_t, void* a, void* b){ struct _tuple6 _temp398; struct Cyc_Dict_Dict*
_temp399; struct Cyc_Dict_Dict** _temp401; struct Cyc_Dict_Dict* _temp402; void*
_temp404; void*(* _temp406)( void*, void*, void*); struct _tuple6* _temp396=
quad_t; _temp398=* _temp396; _LL407: _temp406= _temp398.f1; goto _LL405; _LL405:
_temp404= _temp398.f2; goto _LL403; _LL403: _temp402= _temp398.f3; goto _LL400;
_LL400: _temp399= _temp398.f4; _temp401=&(* _temp396).f4; goto _LL397; _LL397:
if((( int(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp402, a)){*
_temp401=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)(* _temp401, a, _temp406( _temp404,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp402, a), b));}} struct Cyc_Dict_Dict*
Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){ return d1;}{ struct _tuple6 env2=({
struct _tuple6 _temp408; _temp408.f1= f; _temp408.f2= env; _temp408.f3= d1;
_temp408.f4=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
d1->rel); _temp408;});(( void(*)( void(* f)( struct _tuple6*, void*, void*),
struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple6* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,& env2, d2);
return env2.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k, void*
v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp409=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp409->hd=(
void*)({ struct _tuple0* _temp410=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp410->f1= k; _temp410->f2= v; _temp410;}); _temp409->tl= accum;
_temp409;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
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
_tuple7 _temp413; void* _temp414; int(* _temp416)( void*, void*, void*); struct
_tuple7* _temp411= env; _temp413=* _temp411; _LL417: _temp416= _temp413.f1; goto
_LL415; _LL415: _temp414= _temp413.f2; goto _LL412; _LL412: return _temp416(
_temp414, x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct
_tuple7 env=({ struct _tuple7 _temp418; _temp418.f1= f; _temp418.f2= f_env;
_temp418;}); return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(* f)(
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
void* x, void* y){ struct _tuple8 _temp421; void* _temp422; int(* _temp424)(
void*, void*); struct _tuple8* _temp419= env; _temp421=* _temp419; _LL425:
_temp424= _temp421.f1; goto _LL423; _LL423: _temp422= _temp421.f2; goto _LL420;
_LL420: return _temp424( _temp422, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict* d, void* x){ if((( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup_opt)( d, x) == 0){ return d;}{ struct _tuple8 env=({
struct _tuple8 _temp426; _temp426.f1= d->rel; _temp426.f2= x; _temp426;});
return(( struct Cyc_Dict_Dict*(*)( int(* f)( struct _tuple8*, void*, void*),
struct _tuple8* f_env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(( int(*)(
struct _tuple8* env, void* x, void* y)) Cyc_Dict_delete_f,& env, d);}}