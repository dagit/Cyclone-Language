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
0) { int _temp9= _temp4( key,({ struct Cyc_Dict_T* _temp8= _temp6; if( _temp8 ==
0){ _throw( Null_Exception);} _temp8->key_val;}).f1); goto _LL10; _LL10: if(
_temp9 < 0){ _temp6=({ struct Cyc_Dict_T* _temp11= _temp6; if( _temp11 == 0){
_throw( Null_Exception);} _temp11->left;});} else{ if( _temp9 > 0){ _temp6=({
struct Cyc_Dict_T* _temp12= _temp6; if( _temp12 == 0){ _throw( Null_Exception);}
_temp12->right;});} else{ return 1;}}} return 0;}} void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* key){ int(* _temp13)( void*, void*)= d->rel; goto _LL14;
_LL14: { struct Cyc_Dict_T* _temp15= d->t; goto _LL16; _LL16: while( _temp15 !=
0) { int _temp18= _temp13( key,({ struct Cyc_Dict_T* _temp17= _temp15; if(
_temp17 == 0){ _throw( Null_Exception);} _temp17->key_val;}).f1); goto _LL19;
_LL19: if( _temp18 < 0){ _temp15=({ struct Cyc_Dict_T* _temp20= _temp15; if(
_temp20 == 0){ _throw( Null_Exception);} _temp20->left;});} else{ if( _temp18 >
0){ _temp15=({ struct Cyc_Dict_T* _temp21= _temp15; if( _temp21 == 0){ _throw(
Null_Exception);} _temp21->right;});} else{ return({ struct Cyc_Dict_T* _temp22=
_temp15; if( _temp22 == 0){ _throw( Null_Exception);} _temp22->key_val;}).f2;}}}(
void) _throw(( void*) Cyc_Dict_Absent);}} struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* key){ int(* _temp23)( void*, void*)= d->rel; goto
_LL24; _LL24: { struct Cyc_Dict_T* _temp25= d->t; goto _LL26; _LL26: while(
_temp25 != 0) { int _temp28= _temp23( key,({ struct Cyc_Dict_T* _temp27= _temp25;
if( _temp27 == 0){ _throw( Null_Exception);} _temp27->key_val;}).f1); goto _LL29;
_LL29: if( _temp28 < 0){ _temp25=({ struct Cyc_Dict_T* _temp30= _temp25; if(
_temp30 == 0){ _throw( Null_Exception);} _temp30->left;});} else{ if( _temp28 >
0){ _temp25=({ struct Cyc_Dict_T* _temp31= _temp25; if( _temp31 == 0){ _throw(
Null_Exception);} _temp31->right;});} else{ return({ struct Cyc_Core_Opt*
_temp32=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp32->v=( void*)({ struct Cyc_Dict_T* _temp33= _temp25; if( _temp33 == 0){
_throw( Null_Exception);} _temp33->key_val;}).f2; _temp32;});}}} return 0;}} int
Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d, void* key, void** ans_place){ int(*
_temp34)( void*, void*)= d->rel; goto _LL35; _LL35: { struct Cyc_Dict_T* _temp36=
d->t; goto _LL37; _LL37: while( _temp36 != 0) { int _temp39= _temp34( key,({
struct Cyc_Dict_T* _temp38= _temp36; if( _temp38 == 0){ _throw( Null_Exception);}
_temp38->key_val;}).f1); goto _LL40; _LL40: if( _temp39 < 0){ _temp36=({ struct
Cyc_Dict_T* _temp41= _temp36; if( _temp41 == 0){ _throw( Null_Exception);}
_temp41->left;});} else{ if( _temp39 > 0){ _temp36=({ struct Cyc_Dict_T* _temp42=
_temp36; if( _temp42 == 0){ _throw( Null_Exception);} _temp42->right;});} else{*
ans_place=({ struct Cyc_Dict_T* _temp43= _temp36; if( _temp43 == 0){ _throw(
Null_Exception);} _temp43->key_val;}).f2; return 1;}}} return 0;}} struct
_tuple1{ void* f1; struct Cyc_Dict_T* f2; struct Cyc_Dict_T* f3; struct _tuple0
f4; } ; static struct Cyc_Dict_T* Cyc_Dict_balance( struct _tuple1 quad_t){
struct _tuple1 _temp44= quad_t; struct _tuple0 _temp56; struct Cyc_Dict_T*
_temp58; struct Cyc_Dict_T* _temp60; struct Cyc_Dict_T _temp62; struct _tuple0
_temp63; struct Cyc_Dict_T* _temp65; struct Cyc_Dict_T* _temp67; struct Cyc_Dict_T
_temp69; struct _tuple0 _temp70; struct Cyc_Dict_T* _temp72; struct Cyc_Dict_T*
_temp74; void* _temp76; void* _temp78; void* _temp80; struct _tuple0 _temp82;
struct Cyc_Dict_T* _temp84; struct Cyc_Dict_T* _temp86; struct Cyc_Dict_T
_temp88; struct _tuple0 _temp89; struct Cyc_Dict_T* _temp91; struct Cyc_Dict_T
_temp93; struct _tuple0 _temp94; struct Cyc_Dict_T* _temp96; struct Cyc_Dict_T*
_temp98; void* _temp100; struct Cyc_Dict_T* _temp102; void* _temp104; void*
_temp106; struct _tuple0 _temp108; struct Cyc_Dict_T* _temp110; struct Cyc_Dict_T
_temp112; struct _tuple0 _temp113; struct Cyc_Dict_T* _temp115; struct Cyc_Dict_T*
_temp117; struct Cyc_Dict_T _temp119; struct _tuple0 _temp120; struct Cyc_Dict_T*
_temp122; struct Cyc_Dict_T* _temp124; void* _temp126; void* _temp128; struct
Cyc_Dict_T* _temp130; void* _temp132; struct _tuple0 _temp134; struct Cyc_Dict_T*
_temp136; struct Cyc_Dict_T _temp138; struct _tuple0 _temp139; struct Cyc_Dict_T*
_temp141; struct Cyc_Dict_T _temp143; struct _tuple0 _temp144; struct Cyc_Dict_T*
_temp146; struct Cyc_Dict_T* _temp148; void* _temp150; struct Cyc_Dict_T*
_temp152; void* _temp154; struct Cyc_Dict_T* _temp156; void* _temp158; struct
_tuple0 _temp160; struct Cyc_Dict_T* _temp162; struct Cyc_Dict_T* _temp164; void*
_temp166; _LL46: _LL81: _temp80= _temp44.f1; if( _temp80 ==( void*) Cyc_Dict_B){
goto _LL61;} else{ goto _LL48;} _LL61: _temp60= _temp44.f2; if( _temp60 == 0){
goto _LL48;} else{ _temp62=* _temp60; _LL79: _temp78=( void*) _temp62.color; if(
_temp78 ==( void*) Cyc_Dict_R){ goto _LL68;} else{ goto _LL48;} _LL68: _temp67=(
struct Cyc_Dict_T*) _temp62.left; if( _temp67 == 0){ goto _LL48;} else{ _temp69=*
_temp67; _LL77: _temp76=( void*) _temp69.color; if( _temp76 ==( void*) Cyc_Dict_R){
goto _LL75;} else{ goto _LL48;} _LL75: _temp74=( struct Cyc_Dict_T*) _temp69.left;
goto _LL73; _LL73: _temp72=( struct Cyc_Dict_T*) _temp69.right; goto _LL71;
_LL71: _temp70=( struct _tuple0) _temp69.key_val; goto _LL66;} _LL66: _temp65=(
struct Cyc_Dict_T*) _temp62.right; goto _LL64; _LL64: _temp63=( struct _tuple0)
_temp62.key_val; goto _LL59;} _LL59: _temp58= _temp44.f3; goto _LL57; _LL57:
_temp56= _temp44.f4; goto _LL47; _LL48: _LL107: _temp106= _temp44.f1; if(
_temp106 ==( void*) Cyc_Dict_B){ goto _LL87;} else{ goto _LL50;} _LL87: _temp86=
_temp44.f2; if( _temp86 == 0){ goto _LL50;} else{ _temp88=* _temp86; _LL105:
_temp104=( void*) _temp88.color; if( _temp104 ==( void*) Cyc_Dict_R){ goto
_LL103;} else{ goto _LL50;} _LL103: _temp102=( struct Cyc_Dict_T*) _temp88.left;
goto _LL92; _LL92: _temp91=( struct Cyc_Dict_T*) _temp88.right; if( _temp91 == 0){
goto _LL50;} else{ _temp93=* _temp91; _LL101: _temp100=( void*) _temp93.color;
if( _temp100 ==( void*) Cyc_Dict_R){ goto _LL99;} else{ goto _LL50;} _LL99:
_temp98=( struct Cyc_Dict_T*) _temp93.left; goto _LL97; _LL97: _temp96=( struct
Cyc_Dict_T*) _temp93.right; goto _LL95; _LL95: _temp94=( struct _tuple0) _temp93.key_val;
goto _LL90;} _LL90: _temp89=( struct _tuple0) _temp88.key_val; goto _LL85;}
_LL85: _temp84= _temp44.f3; goto _LL83; _LL83: _temp82= _temp44.f4; goto _LL49;
_LL50: _LL133: _temp132= _temp44.f1; if( _temp132 ==( void*) Cyc_Dict_B){ goto
_LL131;} else{ goto _LL52;} _LL131: _temp130= _temp44.f2; goto _LL111; _LL111:
_temp110= _temp44.f3; if( _temp110 == 0){ goto _LL52;} else{ _temp112=* _temp110;
_LL129: _temp128=( void*) _temp112.color; if( _temp128 ==( void*) Cyc_Dict_R){
goto _LL118;} else{ goto _LL52;} _LL118: _temp117=( struct Cyc_Dict_T*) _temp112.left;
if( _temp117 == 0){ goto _LL52;} else{ _temp119=* _temp117; _LL127: _temp126=(
void*) _temp119.color; if( _temp126 ==( void*) Cyc_Dict_R){ goto _LL125;} else{
goto _LL52;} _LL125: _temp124=( struct Cyc_Dict_T*) _temp119.left; goto _LL123;
_LL123: _temp122=( struct Cyc_Dict_T*) _temp119.right; goto _LL121; _LL121:
_temp120=( struct _tuple0) _temp119.key_val; goto _LL116;} _LL116: _temp115=(
struct Cyc_Dict_T*) _temp112.right; goto _LL114; _LL114: _temp113=( struct
_tuple0) _temp112.key_val; goto _LL109;} _LL109: _temp108= _temp44.f4; goto
_LL51; _LL52: _LL159: _temp158= _temp44.f1; if( _temp158 ==( void*) Cyc_Dict_B){
goto _LL157;} else{ goto _LL54;} _LL157: _temp156= _temp44.f2; goto _LL137;
_LL137: _temp136= _temp44.f3; if( _temp136 == 0){ goto _LL54;} else{ _temp138=*
_temp136; _LL155: _temp154=( void*) _temp138.color; if( _temp154 ==( void*) Cyc_Dict_R){
goto _LL153;} else{ goto _LL54;} _LL153: _temp152=( struct Cyc_Dict_T*) _temp138.left;
goto _LL142; _LL142: _temp141=( struct Cyc_Dict_T*) _temp138.right; if( _temp141
== 0){ goto _LL54;} else{ _temp143=* _temp141; _LL151: _temp150=( void*)
_temp143.color; if( _temp150 ==( void*) Cyc_Dict_R){ goto _LL149;} else{ goto
_LL54;} _LL149: _temp148=( struct Cyc_Dict_T*) _temp143.left; goto _LL147;
_LL147: _temp146=( struct Cyc_Dict_T*) _temp143.right; goto _LL145; _LL145:
_temp144=( struct _tuple0) _temp143.key_val; goto _LL140;} _LL140: _temp139=(
struct _tuple0) _temp138.key_val; goto _LL135;} _LL135: _temp134= _temp44.f4;
goto _LL53; _LL54: _LL167: _temp166= _temp44.f1; goto _LL165; _LL165: _temp164=
_temp44.f2; goto _LL163; _LL163: _temp162= _temp44.f3; goto _LL161; _LL161:
_temp160= _temp44.f4; goto _LL55; _LL47: return({ struct Cyc_Dict_T* _temp168=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp168->color=(
void*)(( void*) Cyc_Dict_R); _temp168->left=({ struct Cyc_Dict_T* _temp170=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp170->color=(
void*)(( void*) Cyc_Dict_B); _temp170->left= _temp74; _temp170->right= _temp72;
_temp170->key_val= _temp70; _temp170;}); _temp168->right=({ struct Cyc_Dict_T*
_temp169=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp169->color=(
void*)(( void*) Cyc_Dict_B); _temp169->left= _temp65; _temp169->right= _temp58;
_temp169->key_val= _temp56; _temp169;}); _temp168->key_val= _temp63; _temp168;});
_LL49: return({ struct Cyc_Dict_T* _temp171=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp171->color=( void*)(( void*) Cyc_Dict_R);
_temp171->left=({ struct Cyc_Dict_T* _temp173=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp173->color=( void*)(( void*) Cyc_Dict_B);
_temp173->left= _temp102; _temp173->right= _temp98; _temp173->key_val= _temp89;
_temp173;}); _temp171->right=({ struct Cyc_Dict_T* _temp172=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp172->color=( void*)(( void*) Cyc_Dict_B);
_temp172->left= _temp96; _temp172->right= _temp84; _temp172->key_val= _temp82;
_temp172;}); _temp171->key_val= _temp94; _temp171;}); _LL51: return({ struct Cyc_Dict_T*
_temp174=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp174->color=(
void*)(( void*) Cyc_Dict_R); _temp174->left=({ struct Cyc_Dict_T* _temp176=(
struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp176->color=(
void*)(( void*) Cyc_Dict_B); _temp176->left= _temp130; _temp176->right= _temp124;
_temp176->key_val= _temp108; _temp176;}); _temp174->right=({ struct Cyc_Dict_T*
_temp175=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp175->color=(
void*)(( void*) Cyc_Dict_B); _temp175->left= _temp122; _temp175->right= _temp115;
_temp175->key_val= _temp113; _temp175;}); _temp174->key_val= _temp120; _temp174;});
_LL53: return({ struct Cyc_Dict_T* _temp177=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp177->color=( void*)(( void*) Cyc_Dict_R);
_temp177->left=({ struct Cyc_Dict_T* _temp179=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp179->color=( void*)(( void*) Cyc_Dict_B);
_temp179->left= _temp156; _temp179->right= _temp152; _temp179->key_val= _temp134;
_temp179;}); _temp177->right=({ struct Cyc_Dict_T* _temp178=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp178->color=( void*)(( void*) Cyc_Dict_B);
_temp178->left= _temp148; _temp178->right= _temp146; _temp178->key_val= _temp144;
_temp178;}); _temp177->key_val= _temp139; _temp177;}); _LL55: return({ struct
Cyc_Dict_T* _temp180=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp180->color=( void*) _temp166; _temp180->left= _temp164; _temp180->right=
_temp162; _temp180->key_val= _temp160; _temp180;}); _LL45:;} static struct Cyc_Dict_T*
Cyc_Dict_ins( int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T*
t){ struct Cyc_Dict_T* _temp181= t; struct Cyc_Dict_T _temp187; struct _tuple0
_temp188; struct Cyc_Dict_T* _temp190; struct Cyc_Dict_T* _temp192; void*
_temp194; _LL183: if( _temp181 == 0){ goto _LL184;} else{ goto _LL185;} _LL185:
if( _temp181 == 0){ goto _LL182;} else{ _temp187=* _temp181; _LL195: _temp194=(
void*) _temp187.color; goto _LL193; _LL193: _temp192=( struct Cyc_Dict_T*)
_temp187.left; goto _LL191; _LL191: _temp190=( struct Cyc_Dict_T*) _temp187.right;
goto _LL189; _LL189: _temp188=( struct _tuple0) _temp187.key_val; goto _LL186;}
_LL184: return({ struct Cyc_Dict_T* _temp196=( struct Cyc_Dict_T*) GC_malloc(
sizeof( struct Cyc_Dict_T)); _temp196->color=( void*)(( void*) Cyc_Dict_R);
_temp196->left= 0; _temp196->right= 0; _temp196->key_val= key_val; _temp196;});
_LL186: { int _temp197= rel( key_val.f1, _temp188.f1); goto _LL198; _LL198: if(
_temp197 < 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t)) Cyc_Dict_balance)(({
struct _tuple1 _temp199; _temp199.f1= _temp194; _temp199.f2=(( struct Cyc_Dict_T*(*)(
int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)(
rel, key_val, _temp192); _temp199.f3= _temp190; _temp199.f4= _temp188; _temp199;}));}
else{ if( _temp197 > 0){ return(( struct Cyc_Dict_T*(*)( struct _tuple1 quad_t))
Cyc_Dict_balance)(({ struct _tuple1 _temp200; _temp200.f1= _temp194; _temp200.f2=
_temp192; _temp200.f3=(( struct Cyc_Dict_T*(*)( int(* rel)( void*, void*),
struct _tuple0 key_val, struct Cyc_Dict_T* t)) Cyc_Dict_ins)( rel, key_val,
_temp190); _temp200.f4= _temp188; _temp200;}));} else{ return({ struct Cyc_Dict_T*
_temp201=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T)); _temp201->color=(
void*) _temp194; _temp201->left= _temp192; _temp201->right= _temp190; _temp201->key_val=
key_val; _temp201;});}}} _LL182:;} struct Cyc_Dict_Dict* Cyc_Dict_insert( struct
Cyc_Dict_Dict* d, void* key, void* data){ struct Cyc_Dict_T* _temp203=(( struct
Cyc_Dict_T*(*)( int(* rel)( void*, void*), struct _tuple0 key_val, struct Cyc_Dict_T*
t)) Cyc_Dict_ins)( d->rel,({ struct _tuple0 _temp202; _temp202.f1= key; _temp202.f2=
data; _temp202;}), d->t); goto _LL204; _LL204:( void*)({ struct Cyc_Dict_T*
_temp205= _temp203; if( _temp205 == 0){ _throw( Null_Exception);} _temp205->color=(
void*)(( void*) Cyc_Dict_B);}); return({ struct Cyc_Dict_Dict* _temp206=( struct
Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict)); _temp206->rel= d->rel;
_temp206->t= _temp203; _temp206;});} struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* key, void* data){ if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, key)){( void) _throw(( void*) Cyc_Dict_Absent);}
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( d, key, data);} struct Cyc_Dict_Dict* Cyc_Dict_inserts(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* kds){ for( 0; kds != 0; kds=({
struct Cyc_List_List* _temp207= kds; if( _temp207 == 0){ _throw( Null_Exception);}
_temp207->tl;})){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void*
key, void* data)) Cyc_Dict_insert)( d,({ struct _tuple0* _temp209=( struct
_tuple0*)({ struct Cyc_List_List* _temp208= kds; if( _temp208 == 0){ _throw(
Null_Exception);} _temp208->hd;}); unsigned int _temp210= 0; if( _temp210 >= 1u){
_throw( Null_Exception);} _temp209[ _temp210];}).f1,({ struct _tuple0* _temp212=(
struct _tuple0*)({ struct Cyc_List_List* _temp211= kds; if( _temp211 == 0){
_throw( Null_Exception);} _temp211->hd;}); unsigned int _temp213= 0; if(
_temp213 >= 1u){ _throw( Null_Exception);} _temp212[ _temp213];}).f2);} return d;}
static void* Cyc_Dict_fold_tree( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum){ struct Cyc_Dict_T _temp216; struct _tuple0 _temp217; void*
_temp219; void* _temp221; struct Cyc_Dict_T* _temp223; struct Cyc_Dict_T*
_temp225; void* _temp227; struct Cyc_Dict_T* _temp214= t; _temp216=* _temp214;
_LL228: _temp227=( void*) _temp216.color; goto _LL226; _LL226: _temp225=( struct
Cyc_Dict_T*) _temp216.left; goto _LL224; _LL224: _temp223=( struct Cyc_Dict_T*)
_temp216.right; goto _LL218; _LL218: _temp217=( struct _tuple0) _temp216.key_val;
_LL222: _temp221= _temp217.f1; goto _LL220; _LL220: _temp219= _temp217.f2; goto
_LL215; _LL215: if( _temp225 != 0){ accum=(( void*(*)( void*(* f)( void*, void*,
void*), struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp229= _temp225; if( _temp229 == 0){ _throw(
Null_Exception);} _temp229;}), accum);} accum= f( _temp221, _temp219, accum);
if( _temp223 != 0){ accum=(( void*(*)( void*(* f)( void*, void*, void*), struct
Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp230= _temp223; if( _temp230 == 0){ _throw(
Null_Exception);} _temp230;}), accum);} return accum;} void* Cyc_Dict_fold( void*(*
f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum){ if( d->t == 0){
return accum;} return(( void*(*)( void*(* f)( void*, void*, void*), struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp231= d->t; if( _temp231 == 0){ _throw( Null_Exception);} _temp231;}), accum);}
static void* Cyc_Dict_fold_tree_c( void*(* f)( void*, void*, void*, void*), void*
env, struct Cyc_Dict_T* t, void* accum){ struct Cyc_Dict_T _temp234; struct
_tuple0 _temp235; void* _temp237; void* _temp239; struct Cyc_Dict_T* _temp241;
struct Cyc_Dict_T* _temp243; void* _temp245; struct Cyc_Dict_T* _temp232= t;
_temp234=* _temp232; _LL246: _temp245=( void*) _temp234.color; goto _LL244;
_LL244: _temp243=( struct Cyc_Dict_T*) _temp234.left; goto _LL242; _LL242:
_temp241=( struct Cyc_Dict_T*) _temp234.right; goto _LL236; _LL236: _temp235=(
struct _tuple0) _temp234.key_val; _LL240: _temp239= _temp235.f1; goto _LL238;
_LL238: _temp237= _temp235.f2; goto _LL233; _LL233: if( _temp243 != 0){ accum=((
void*(*)( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_T*
t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct
Cyc_Dict_T* _temp247= _temp243; if( _temp247 == 0){ _throw( Null_Exception);}
_temp247;}), accum);} accum= f( env, _temp239, _temp237, accum); if( _temp241 !=
0){ accum=(( void*(*)( void*(* f)( void*, void*, void*, void*), void* env,
struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp248= _temp241; if( _temp248 == 0){ _throw(
Null_Exception);} _temp248;}), accum);} return accum;} void* Cyc_Dict_fold_c(
void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict* d,
void* accum){ if( d->t == 0){ return accum;} return(( void*(*)( void*(* f)( void*,
void*, void*, void*), void* env, struct Cyc_Dict_T* t, void* accum)) Cyc_Dict_fold_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp249= d->t; if( _temp249
== 0){ _throw( Null_Exception);} _temp249;}), accum);} static void Cyc_Dict_app_tree(
void*(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp252;
struct _tuple0 _temp253; void* _temp255; void* _temp257; struct Cyc_Dict_T*
_temp259; struct Cyc_Dict_T* _temp261; void* _temp263; struct Cyc_Dict_T*
_temp250= t; _temp252=* _temp250; _LL264: _temp263=( void*) _temp252.color; goto
_LL262; _LL262: _temp261=( struct Cyc_Dict_T*) _temp252.left; goto _LL260;
_LL260: _temp259=( struct Cyc_Dict_T*) _temp252.right; goto _LL254; _LL254:
_temp253=( struct _tuple0) _temp252.key_val; _LL258: _temp257= _temp253.f1; goto
_LL256; _LL256: _temp255= _temp253.f2; goto _LL251; _LL251: if( _temp261 != 0){((
void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp265= _temp261; if( _temp265 == 0){
_throw( Null_Exception);} _temp265;}));} f( _temp257, _temp255); if( _temp259 !=
0){(( void(*)( void*(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp266= _temp259; if( _temp266 ==
0){ _throw( Null_Exception);} _temp266;}));}} void Cyc_Dict_app( void*(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_app_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp267= d->t; if( _temp267 == 0){ _throw( Null_Exception);}
_temp267;}));}} static void Cyc_Dict_app_tree_c( void*(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp270; struct _tuple0
_temp271; void* _temp273; void* _temp275; struct Cyc_Dict_T* _temp277; struct
Cyc_Dict_T* _temp279; void* _temp281; struct Cyc_Dict_T* _temp268= t; _temp270=*
_temp268; _LL282: _temp281=( void*) _temp270.color; goto _LL280; _LL280:
_temp279=( struct Cyc_Dict_T*) _temp270.left; goto _LL278; _LL278: _temp277=(
struct Cyc_Dict_T*) _temp270.right; goto _LL272; _LL272: _temp271=( struct
_tuple0) _temp270.key_val; _LL276: _temp275= _temp271.f1; goto _LL274; _LL274:
_temp273= _temp271.f2; goto _LL269; _LL269: if( _temp279 != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp283= _temp279; if(
_temp283 == 0){ _throw( Null_Exception);} _temp283;}));} f( env, _temp275,
_temp273); if( _temp277 != 0){(( void(*)( void*(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp284= _temp277; if( _temp284 == 0){ _throw(
Null_Exception);} _temp284;}));}} void Cyc_Dict_app_c( void*(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void*(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_app_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp285= d->t; if( _temp285
== 0){ _throw( Null_Exception);} _temp285;}));}} static void Cyc_Dict_iter_tree(
void(* f)( void*, void*), struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp288;
struct _tuple0 _temp289; void* _temp291; void* _temp293; struct Cyc_Dict_T*
_temp295; struct Cyc_Dict_T* _temp297; void* _temp299; struct Cyc_Dict_T*
_temp286= t; _temp288=* _temp286; _LL300: _temp299=( void*) _temp288.color; goto
_LL298; _LL298: _temp297=( struct Cyc_Dict_T*) _temp288.left; goto _LL296;
_LL296: _temp295=( struct Cyc_Dict_T*) _temp288.right; goto _LL290; _LL290:
_temp289=( struct _tuple0) _temp288.key_val; _LL294: _temp293= _temp289.f1; goto
_LL292; _LL292: _temp291= _temp289.f2; goto _LL287; _LL287: if( _temp297 != 0){((
void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,(
struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp301= _temp297; if( _temp301 == 0){
_throw( Null_Exception);} _temp301;}));} f( _temp293, _temp291); if( _temp295 !=
0){(( void(*)( void(* f)( void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp302= _temp295; if( _temp302 ==
0){ _throw( Null_Exception);} _temp302;}));}} void Cyc_Dict_iter( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*), struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp303= d->t; if( _temp303 == 0){ _throw( Null_Exception);}
_temp303;}));}} static void Cyc_Dict_iter_tree_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp306; struct _tuple0
_temp307; void* _temp309; void* _temp311; struct Cyc_Dict_T* _temp313; struct
Cyc_Dict_T* _temp315; void* _temp317; struct Cyc_Dict_T* _temp304= t; _temp306=*
_temp304; _LL318: _temp317=( void*) _temp306.color; goto _LL316; _LL316:
_temp315=( struct Cyc_Dict_T*) _temp306.left; goto _LL314; _LL314: _temp313=(
struct Cyc_Dict_T*) _temp306.right; goto _LL308; _LL308: _temp307=( struct
_tuple0) _temp306.key_val; _LL312: _temp311= _temp307.f1; goto _LL310; _LL310:
_temp309= _temp307.f2; goto _LL305; _LL305: if( _temp315 != 0){(( void(*)( void(*
f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp319= _temp315; if(
_temp319 == 0){ _throw( Null_Exception);} _temp319;}));} f( env, _temp311,
_temp309); if( _temp313 != 0){(( void(*)( void(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp320= _temp313; if( _temp320 == 0){ _throw(
Null_Exception);} _temp320;}));}} void Cyc_Dict_iter_c( void(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t != 0){(( void(*)( void(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_iter_tree_c)( f,
env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp321= d->t; if( _temp321 == 0){
_throw( Null_Exception);} _temp321;}));}} struct _tuple2{ void(* f1)( void*,
void*); struct Cyc_Dict_Dict* f2; } ; static void Cyc_Dict_iter2_f( struct
_tuple2* env, void* a, void* b1){ struct _tuple2 _temp324; struct Cyc_Dict_Dict*
_temp325; void(* _temp327)( void*, void*); struct _tuple2* _temp322= env;
_temp324=* _temp322; _LL328: _temp327= _temp324.f1; goto _LL326; _LL326:
_temp325= _temp324.f2; goto _LL323; _LL323: _temp327( b1,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( _temp325, a));} void Cyc_Dict_iter2( void(* f)(
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple2 _temp330=({ struct _tuple2 _temp329; _temp329.f1= f; _temp329.f2= d2;
_temp329;}); goto _LL331; _LL331:(( void(*)( void(* f)( struct _tuple2*, void*,
void*), struct _tuple2* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple2* env, void* a, void* b1)) Cyc_Dict_iter2_f,& _temp330, d1);}
struct _tuple3{ void(* f1)( void*, void*, void*); struct Cyc_Dict_Dict* f2; void*
f3; } ; static void Cyc_Dict_iter2_c_f( struct _tuple3* env, void* a, void* b1){
struct _tuple3 _temp334; void* _temp335; struct Cyc_Dict_Dict* _temp337; void(*
_temp339)( void*, void*, void*); struct _tuple3* _temp332= env; _temp334=*
_temp332; _LL340: _temp339= _temp334.f1; goto _LL338; _LL338: _temp337= _temp334.f2;
goto _LL336; _LL336: _temp335= _temp334.f3; goto _LL333; _LL333: _temp339(
_temp335, b1,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp337, a));} void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void*
inner_env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple3
_temp342=({ struct _tuple3 _temp341; _temp341.f1= f; _temp341.f2= d2; _temp341.f3=
inner_env; _temp341;}); goto _LL343; _LL343:(( void(*)( void(* f)( struct
_tuple3*, void*, void*), struct _tuple3* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple3* env, void* a, void* b1)) Cyc_Dict_iter2_c_f,& _temp342,
d1);} static struct Cyc_Dict_T* Cyc_Dict_map_tree( void*(* f)( void*), struct
Cyc_Dict_T* t){ struct Cyc_Dict_T _temp346; struct _tuple0 _temp347; void*
_temp349; void* _temp351; struct Cyc_Dict_T* _temp353; struct Cyc_Dict_T*
_temp355; void* _temp357; struct Cyc_Dict_T* _temp344= t; _temp346=* _temp344;
_LL358: _temp357=( void*) _temp346.color; goto _LL356; _LL356: _temp355=( struct
Cyc_Dict_T*) _temp346.left; goto _LL354; _LL354: _temp353=( struct Cyc_Dict_T*)
_temp346.right; goto _LL348; _LL348: _temp347=( struct _tuple0) _temp346.key_val;
_LL352: _temp351= _temp347.f1; goto _LL350; _LL350: _temp349= _temp347.f2; goto
_LL345; _LL345: { struct Cyc_Dict_T* _temp360= _temp355 == 0? 0:(( struct Cyc_Dict_T*(*)(
void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp359= _temp355; if( _temp359 == 0){ _throw(
Null_Exception);} _temp359;})); goto _LL361; _LL361: { void* _temp362= f(
_temp349); goto _LL363; _LL363: { struct Cyc_Dict_T* _temp365= _temp353 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*), struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)(
f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp364= _temp353; if( _temp364 ==
0){ _throw( Null_Exception);} _temp364;})); goto _LL366; _LL366: return({ struct
Cyc_Dict_T* _temp367=( struct Cyc_Dict_T*) GC_malloc( sizeof( struct Cyc_Dict_T));
_temp367->color=( void*) _temp357; _temp367->left= _temp360; _temp367->right=
_temp365; _temp367->key_val=({ struct _tuple0 _temp368; _temp368.f1= _temp351;
_temp368.f2= _temp362; _temp368;}); _temp367;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_map(
void*(* f)( void*), struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct
Cyc_Dict_Dict* _temp369=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp369->rel= d->rel; _temp369->t= 0; _temp369;});} return({ struct Cyc_Dict_Dict*
_temp370=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp370->rel= d->rel; _temp370->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*),
struct Cyc_Dict_T* t)) Cyc_Dict_map_tree)( f,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp371= d->t; if( _temp371 == 0){ _throw( Null_Exception);} _temp371;}));
_temp370;});} static struct Cyc_Dict_T* Cyc_Dict_map_tree_c( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp374; struct
_tuple0 _temp375; void* _temp377; void* _temp379; struct Cyc_Dict_T* _temp381;
struct Cyc_Dict_T* _temp383; void* _temp385; struct Cyc_Dict_T* _temp372= t;
_temp374=* _temp372; _LL386: _temp385=( void*) _temp374.color; goto _LL384;
_LL384: _temp383=( struct Cyc_Dict_T*) _temp374.left; goto _LL382; _LL382:
_temp381=( struct Cyc_Dict_T*) _temp374.right; goto _LL376; _LL376: _temp375=(
struct _tuple0) _temp374.key_val; _LL380: _temp379= _temp375.f1; goto _LL378;
_LL378: _temp377= _temp375.f2; goto _LL373; _LL373: { struct Cyc_Dict_T*
_temp388= _temp383 == 0? 0:(( struct Cyc_Dict_T*(*)( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp387= _temp383; if( _temp387 == 0){ _throw(
Null_Exception);} _temp387;})); goto _LL389; _LL389: { void* _temp390= f( env,
_temp377); goto _LL391; _LL391: { struct Cyc_Dict_T* _temp393= _temp381 == 0? 0:((
struct Cyc_Dict_T*(*)( void*(* f)( void*, void*), void* env, struct Cyc_Dict_T*
t)) Cyc_Dict_map_tree_c)( f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T*
_temp392= _temp381; if( _temp392 == 0){ _throw( Null_Exception);} _temp392;}));
goto _LL394; _LL394: return({ struct Cyc_Dict_T* _temp395=( struct Cyc_Dict_T*)
GC_malloc( sizeof( struct Cyc_Dict_T)); _temp395->color=( void*) _temp385;
_temp395->left= _temp388; _temp395->right= _temp393; _temp395->key_val=({ struct
_tuple0 _temp396; _temp396.f1= _temp379; _temp396.f2= _temp390; _temp396;});
_temp395;});}}}} struct Cyc_Dict_Dict* Cyc_Dict_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return({ struct Cyc_Dict_Dict*
_temp397=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp397->rel= d->rel; _temp397->t= 0; _temp397;});} return({ struct Cyc_Dict_Dict*
_temp398=( struct Cyc_Dict_Dict*) GC_malloc( sizeof( struct Cyc_Dict_Dict));
_temp398->rel= d->rel; _temp398->t=(( struct Cyc_Dict_T*(*)( void*(* f)( void*,
void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_map_tree_c)( f, env,( struct
Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp399= d->t; if( _temp399 == 0){ _throw(
Null_Exception);} _temp399;})); _temp398;});} struct _tuple0* Cyc_Dict_choose(
struct Cyc_Dict_Dict* d){ if( d->t == 0){( void) _throw(( void*) Cyc_Dict_Absent);}
return({ struct _tuple0* _temp400=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp400->f1=({ struct Cyc_Dict_T* _temp402= d->t; if( _temp402 == 0){
_throw( Null_Exception);} _temp402->key_val;}).f1; _temp400->f2=({ struct Cyc_Dict_T*
_temp401= d->t; if( _temp401 == 0){ _throw( Null_Exception);} _temp401->key_val;}).f2;
_temp400;});} static int Cyc_Dict_forall_tree_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_T* t){ struct Cyc_Dict_T _temp405; struct _tuple0
_temp406; void* _temp408; void* _temp410; struct Cyc_Dict_T* _temp412; struct
Cyc_Dict_T* _temp414; void* _temp416; struct Cyc_Dict_T* _temp403= t; _temp405=*
_temp403; _LL417: _temp416=( void*) _temp405.color; goto _LL415; _LL415:
_temp414=( struct Cyc_Dict_T*) _temp405.left; goto _LL413; _LL413: _temp412=(
struct Cyc_Dict_T*) _temp405.right; goto _LL407; _LL407: _temp406=( struct
_tuple0) _temp405.key_val; _LL411: _temp410= _temp406.f1; goto _LL409; _LL409:
_temp408= _temp406.f2; goto _LL404; _LL404: return(( _temp414 == 0? 1:(( int(*)(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp418= _temp414; if(
_temp418 == 0){ _throw( Null_Exception);} _temp418;})))? f( env, _temp410,
_temp408): 0)? _temp412 == 0? 1:(( int(*)( int(* f)( void*, void*, void*), void*
env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)( f, env,( struct Cyc_Dict_T*)({
struct Cyc_Dict_T* _temp419= _temp412; if( _temp419 == 0){ _throw(
Null_Exception);} _temp419;})): 0;} int Cyc_Dict_forall_c( int(* f)( void*, void*,
void*), void* env, struct Cyc_Dict_Dict* d){ if( d->t == 0){ return 1;} return((
int(*)( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_T* t)) Cyc_Dict_forall_tree_c)(
f, env,( struct Cyc_Dict_T*)({ struct Cyc_Dict_T* _temp420= d->t; if( _temp420
== 0){ _throw( Null_Exception);} _temp420;}));} struct _tuple4{ int(* f1)( void*,
void*, void*); struct Cyc_Dict_Dict* f2; } ; static int Cyc_Dict_forall_intersect_f(
struct _tuple4* env, void* a, void* b){ struct _tuple4 _temp423; struct Cyc_Dict_Dict*
_temp424; int(* _temp426)( void*, void*, void*); struct _tuple4* _temp421= env;
_temp423=* _temp421; _LL427: _temp426= _temp423.f1; goto _LL425; _LL425:
_temp424= _temp423.f2; goto _LL422; _LL422: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp424, a)){ return _temp426( a, b,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( _temp424, a));} return 1;}
int Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2){ struct _tuple4 _temp429=({ struct _tuple4
_temp428; _temp428.f1= f; _temp428.f2= d2; _temp428;}); goto _LL430; _LL430:
return(( int(*)( int(* f)( struct _tuple4*, void*, void*), struct _tuple4* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)( struct _tuple4* env, void*
a, void* b)) Cyc_Dict_forall_intersect_f,& _temp429, d1);} static struct Cyc_Dict_Dict*
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
struct _tuple5 _temp433; struct Cyc_Dict_Dict* _temp434; struct Cyc_Dict_Dict**
_temp436; struct Cyc_Dict_Dict* _temp437; void*(* _temp439)( void*, void*);
struct _tuple5* _temp431= trip; _temp433=* _temp431; _LL440: _temp439= _temp433.f1;
goto _LL438; _LL438: _temp437= _temp433.f2; goto _LL435; _LL435: _temp434=
_temp433.f3; _temp436=&(* _temp431).f3; goto _LL432; _LL432: if((( int(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_member)( _temp437, a)){* _temp436=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(*
_temp436, a, _temp439((( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp437, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect( void*(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ if( d1 == d2){
return d1;}{ struct _tuple5 _temp442=({ struct _tuple5 _temp441; _temp441.f1= f;
_temp441.f2= d1; _temp441.f3=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*,
void*))) Cyc_Dict_empty)( d1->rel); _temp441;}); goto _LL443; _LL443:(( void(*)(
void(* f)( struct _tuple5*, void*, void*), struct _tuple5* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple5* trip, void* a, void* b)) Cyc_Dict_intersect_f,&
_temp442, d2); return _temp442.f3;}} struct _tuple6{ void*(* f1)( void*, void*,
void*); void* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; } ; static
void Cyc_Dict_intersect_c_f( struct _tuple6* quad_t, void* a, void* b){ struct
_tuple6 _temp446; struct Cyc_Dict_Dict* _temp447; struct Cyc_Dict_Dict**
_temp449; struct Cyc_Dict_Dict* _temp450; void* _temp452; void*(* _temp454)(
void*, void*, void*); struct _tuple6* _temp444= quad_t; _temp446=* _temp444;
_LL455: _temp454= _temp446.f1; goto _LL453; _LL453: _temp452= _temp446.f2; goto
_LL451; _LL451: _temp450= _temp446.f3; goto _LL448; _LL448: _temp447= _temp446.f4;
_temp449=&(* _temp444).f4; goto _LL445; _LL445: if((( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( _temp450, a)){* _temp449=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(* _temp449, a,
_temp454( _temp452,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
_temp450, a), b));}} struct Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)(
void*, void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2){ if( d1 == d2){ return d1;}{ struct _tuple6 _temp457=({ struct _tuple6
_temp456; _temp456.f1= f; _temp456.f2= env; _temp456.f3= d1; _temp456.f4=((
struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)( d1->rel);
_temp456;}); goto _LL458; _LL458:(( void(*)( void(* f)( struct _tuple6*, void*,
void*), struct _tuple6* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple6* quad_t, void* a, void* b)) Cyc_Dict_intersect_c_f,& _temp457, d2);
return _temp457.f4;}} static struct Cyc_List_List* Cyc_Dict_to_list_f( void* k,
void* v, struct Cyc_List_List* accum){ return({ struct Cyc_List_List* _temp459=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp459->hd=(
void*)({ struct _tuple0* _temp460=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp460->f1= k; _temp460->f2= v; _temp460;}); _temp459->tl= accum;
_temp459;});} struct Cyc_List_List* Cyc_Dict_to_list( struct Cyc_Dict_Dict* d){
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
_tuple7 _temp463; void* _temp464; int(* _temp466)( void*, void*, void*); struct
_tuple7* _temp461= env; _temp463=* _temp461; _LL467: _temp466= _temp463.f1; goto
_LL465; _LL465: _temp464= _temp463.f2; goto _LL462; _LL462: return _temp466(
_temp464, x, y)?(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( acc, x, y): acc;} struct Cyc_Dict_Dict* Cyc_Dict_filter_c(
int(* f)( void*, void*, void*), void* f_env, struct Cyc_Dict_Dict* d){ struct
_tuple7 _temp469=({ struct _tuple7 _temp468; _temp468.f1= f; _temp468.f2= f_env;
_temp468;}); goto _LL470; _LL470: return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*(*
f)( struct _tuple7*, void*, void*, struct Cyc_Dict_Dict*), struct _tuple7* env,
struct Cyc_Dict_Dict* d, struct Cyc_Dict_Dict* accum)) Cyc_Dict_fold_c)(( struct
Cyc_Dict_Dict*(*)( struct _tuple7* env, void* x, void* y, struct Cyc_Dict_Dict*
acc)) Cyc_Dict_filter_c_f,& _temp469, d,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( d->rel));} static int Cyc_Dict_difference_f(
struct Cyc_Dict_Dict* d, void* x, void* y){ return !(( int(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_member)( d, x);} struct Cyc_Dict_Dict* Cyc_Dict_difference(
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ return(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Dict_Dict*, void*, void*), struct Cyc_Dict_Dict* f_env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)(( int(*)( struct Cyc_Dict_Dict* d,
void* x, void* y)) Cyc_Dict_difference_f, d2, d1);} struct _tuple8{ int(* f1)(
void*, void*); void* f2; } ; static int Cyc_Dict_delete_f( struct _tuple8* env,
void* x, void* y){ struct _tuple8 _temp473; void* _temp474; int(* _temp476)(
void*, void*); struct _tuple8* _temp471= env; _temp473=* _temp471; _LL477:
_temp476= _temp473.f1; goto _LL475; _LL475: _temp474= _temp473.f2; goto _LL472;
_LL472: return _temp476( _temp474, x) == 0;} struct Cyc_Dict_Dict* Cyc_Dict_delete(
struct Cyc_Dict_Dict* d, void* x){ if((( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup_opt)( d, x) == 0){ return d;}{ struct _tuple8
_temp479=({ struct _tuple8 _temp478; _temp478.f1= d->rel; _temp478.f2= x;
_temp478;}); goto _LL480; _LL480: return(( struct Cyc_Dict_Dict*(*)( int(* f)(
struct _tuple8*, void*, void*), struct _tuple8* f_env, struct Cyc_Dict_Dict* d))
Cyc_Dict_filter_c)(( int(*)( struct _tuple8* env, void* x, void* y)) Cyc_Dict_delete_f,&
_temp479, d);}}