#include "cyc_include.h"

 struct _tagged_ptr0{ void** curr; void** base; void** last_plus_one; } ; struct
_tuple0{ void* f1; void* f2; } ; struct _tagged_ptr1{ struct _tuple0* curr;
struct _tuple0* base; struct _tuple0* last_plus_one; } ; struct _tuple1{ struct
_tagged_ptr0 f1; struct _tagged_ptr0 f2; } ; typedef unsigned int Cyc_uint;
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
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; extern struct _tagged_ptr0 Cyc_List_to_array(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_from_array(
struct _tagged_ptr0 arr); extern void Cyc_Array_qsort( int(* less_eq)( void*,
void*), struct _tagged_ptr0 x, int ofs, int len); extern void Cyc_Array_msort(
int(* less_eq)( void*, void*), struct _tagged_ptr0 x, int ofs, int len); extern
struct _tagged_ptr0 Cyc_Array_from_list( struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_Array_to_list( struct _tagged_ptr0 x); extern struct
_tagged_ptr0 Cyc_Array_copy( struct _tagged_ptr0 x); extern struct _tagged_ptr0
Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x); extern struct
_tagged_ptr0 Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct
_tagged_ptr0 x); extern void Cyc_Array_imp_map( void*(* f)( void*), struct
_tagged_ptr0 x); extern void Cyc_Array_imp_map_c( void*(* f)( void*, void*),
void* env, struct _tagged_ptr0 x); extern char Cyc_Array_Array_mismatch_tag[ 15u];
struct Cyc_Array_Array_mismatch_struct{ char* tag; } ; extern struct
_tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct _tagged_ptr0 x,
struct _tagged_ptr0 y); extern void Cyc_Array_app( void*(* f)( void*), struct
_tagged_ptr0 x); extern void Cyc_Array_app_c( void*(* f)( void*, void*), void*
env, struct _tagged_ptr0 x); extern void Cyc_Array_iter( void(* f)( void*),
struct _tagged_ptr0 x); extern void Cyc_Array_iter_c( void(* f)( void*, void*),
void* env, struct _tagged_ptr0 x); extern void Cyc_Array_app2( void*(* f)( void*,
void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y); extern void Cyc_Array_iter2( void(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y); extern void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, struct _tagged_ptr0 y);
extern void* Cyc_Array_fold_left( void*(* f)( void*, void*), void* accum, struct
_tagged_ptr0 x); extern void* Cyc_Array_fold_left_c( void*(* f)( void*, void*,
void*), void* env, void* accum, struct _tagged_ptr0 x); extern void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum); extern void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum);
extern struct _tagged_ptr0 Cyc_Array_rev_copy( struct _tagged_ptr0 x); extern
void Cyc_Array_imp_rev( struct _tagged_ptr0 x); extern int Cyc_Array_forall( int(*
pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_forall_c( int(* pred)(
void*, void*), void* env, struct _tagged_ptr0 x); extern int Cyc_Array_exists(
int(* pred)( void*), struct _tagged_ptr0 x); extern int Cyc_Array_exists_c( int(*
pred)( void*, void*), void* env, struct _tagged_ptr0 x); extern struct
_tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct _tagged_ptr0 y);
extern struct _tuple1 Cyc_Array_split( struct _tagged_ptr1 x); extern int Cyc_Array_memq(
struct _tagged_ptr0 l, void* x); extern int Cyc_Array_mem( int(* compare)( void*,
void*), struct _tagged_ptr0 l, void* x); extern struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt); static int Cyc_Array_sp[ 40]={ 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; void Cyc_Array_qsort( int(* less_eq)( void*,
void*), struct _tagged_ptr0 arr, int base_ofs, int len){ void* temp; int sp_ofs;
int i; int j; int limit_ofs; if(( base_ofs < 0? 1:( unsigned int)( base_ofs +
len) >({ struct _tagged_ptr0 _temp0= arr;( unsigned int)( _temp0.last_plus_one -
_temp0.curr);}))? 1: len < 0){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp1=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp1=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp2=( char*)"Array::qsort"; struct
_tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 13; _temp3;})};( struct _xenum_struct*) _temp1;}));} limit_ofs=
base_ofs + len; sp_ofs= 0; for( 0; 1; 0){ if( limit_ofs - base_ofs > 3){ temp=({
struct _tagged_ptr0 _temp4= arr; void** _temp6= _temp4.curr +(( limit_ofs -
base_ofs) / 2 + base_ofs); if( _temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one){
_throw( Null_Exception);}* _temp6;});({ struct _tagged_ptr0 _temp7= arr; void**
_temp9= _temp7.curr +(( limit_ofs - base_ofs) / 2 + base_ofs); if( _temp9 <
_temp7.base? 1: _temp9 >= _temp7.last_plus_one){ _throw( Null_Exception);}*
_temp9=({ struct _tagged_ptr0 _temp10= arr; void** _temp12= _temp10.curr +
base_ofs; if( _temp12 < _temp10.base? 1: _temp12 >= _temp10.last_plus_one){
_throw( Null_Exception);}* _temp12;});});({ struct _tagged_ptr0 _temp13= arr;
void** _temp15= _temp13.curr + base_ofs; if( _temp15 < _temp13.base? 1: _temp15
>= _temp13.last_plus_one){ _throw( Null_Exception);}* _temp15= temp;}); i=
base_ofs + 1; j= limit_ofs - 1; if( less_eq(({ struct _tagged_ptr0 _temp16= arr;
void** _temp18= _temp16.curr + i; if( _temp18 < _temp16.base? 1: _temp18 >=
_temp16.last_plus_one){ _throw( Null_Exception);}* _temp18;}),({ struct
_tagged_ptr0 _temp19= arr; void** _temp21= _temp19.curr + j; if( _temp21 <
_temp19.base? 1: _temp21 >= _temp19.last_plus_one){ _throw( Null_Exception);}*
_temp21;})) > 0){ temp=({ struct _tagged_ptr0 _temp22= arr; void** _temp24=
_temp22.curr + i; if( _temp24 < _temp22.base? 1: _temp24 >= _temp22.last_plus_one){
_throw( Null_Exception);}* _temp24;});({ struct _tagged_ptr0 _temp25= arr; void**
_temp27= _temp25.curr + i; if( _temp27 < _temp25.base? 1: _temp27 >= _temp25.last_plus_one){
_throw( Null_Exception);}* _temp27=({ struct _tagged_ptr0 _temp28= arr; void**
_temp30= _temp28.curr + j; if( _temp30 < _temp28.base? 1: _temp30 >= _temp28.last_plus_one){
_throw( Null_Exception);}* _temp30;});});({ struct _tagged_ptr0 _temp31= arr;
void** _temp33= _temp31.curr + j; if( _temp33 < _temp31.base? 1: _temp33 >=
_temp31.last_plus_one){ _throw( Null_Exception);}* _temp33= temp;});} if(
less_eq(({ struct _tagged_ptr0 _temp34= arr; void** _temp36= _temp34.curr +
base_ofs; if( _temp36 < _temp34.base? 1: _temp36 >= _temp34.last_plus_one){
_throw( Null_Exception);}* _temp36;}),({ struct _tagged_ptr0 _temp37= arr; void**
_temp39= _temp37.curr + j; if( _temp39 < _temp37.base? 1: _temp39 >= _temp37.last_plus_one){
_throw( Null_Exception);}* _temp39;})) > 0){ temp=({ struct _tagged_ptr0 _temp40=
arr; void** _temp42= _temp40.curr + base_ofs; if( _temp42 < _temp40.base? 1:
_temp42 >= _temp40.last_plus_one){ _throw( Null_Exception);}* _temp42;});({
struct _tagged_ptr0 _temp43= arr; void** _temp45= _temp43.curr + base_ofs; if(
_temp45 < _temp43.base? 1: _temp45 >= _temp43.last_plus_one){ _throw(
Null_Exception);}* _temp45=({ struct _tagged_ptr0 _temp46= arr; void** _temp48=
_temp46.curr + j; if( _temp48 < _temp46.base? 1: _temp48 >= _temp46.last_plus_one){
_throw( Null_Exception);}* _temp48;});});({ struct _tagged_ptr0 _temp49= arr;
void** _temp51= _temp49.curr + j; if( _temp51 < _temp49.base? 1: _temp51 >=
_temp49.last_plus_one){ _throw( Null_Exception);}* _temp51= temp;});} if(
less_eq(({ struct _tagged_ptr0 _temp52= arr; void** _temp54= _temp52.curr + i;
if( _temp54 < _temp52.base? 1: _temp54 >= _temp52.last_plus_one){ _throw(
Null_Exception);}* _temp54;}),({ struct _tagged_ptr0 _temp55= arr; void**
_temp57= _temp55.curr + base_ofs; if( _temp57 < _temp55.base? 1: _temp57 >=
_temp55.last_plus_one){ _throw( Null_Exception);}* _temp57;})) > 0){ temp=({
struct _tagged_ptr0 _temp58= arr; void** _temp60= _temp58.curr + i; if( _temp60
< _temp58.base? 1: _temp60 >= _temp58.last_plus_one){ _throw( Null_Exception);}*
_temp60;});({ struct _tagged_ptr0 _temp61= arr; void** _temp63= _temp61.curr + i;
if( _temp63 < _temp61.base? 1: _temp63 >= _temp61.last_plus_one){ _throw(
Null_Exception);}* _temp63=({ struct _tagged_ptr0 _temp64= arr; void** _temp66=
_temp64.curr + base_ofs; if( _temp66 < _temp64.base? 1: _temp66 >= _temp64.last_plus_one){
_throw( Null_Exception);}* _temp66;});});({ struct _tagged_ptr0 _temp67= arr;
void** _temp69= _temp67.curr + base_ofs; if( _temp69 < _temp67.base? 1: _temp69
>= _temp67.last_plus_one){ _throw( Null_Exception);}* _temp69= temp;});} for( 0;
1; 0){ do { i ++;} while ( less_eq(({ struct _tagged_ptr0 _temp70= arr; void**
_temp72= _temp70.curr + i; if( _temp72 < _temp70.base? 1: _temp72 >= _temp70.last_plus_one){
_throw( Null_Exception);}* _temp72;}),({ struct _tagged_ptr0 _temp73= arr; void**
_temp75= _temp73.curr + base_ofs; if( _temp75 < _temp73.base? 1: _temp75 >=
_temp73.last_plus_one){ _throw( Null_Exception);}* _temp75;})) < 0); do { j --;} while (
less_eq(({ struct _tagged_ptr0 _temp76= arr; void** _temp78= _temp76.curr + j;
if( _temp78 < _temp76.base? 1: _temp78 >= _temp76.last_plus_one){ _throw(
Null_Exception);}* _temp78;}),({ struct _tagged_ptr0 _temp79= arr; void**
_temp81= _temp79.curr + base_ofs; if( _temp81 < _temp79.base? 1: _temp81 >=
_temp79.last_plus_one){ _throw( Null_Exception);}* _temp81;})) > 0); if( i > j){
break;} temp=({ struct _tagged_ptr0 _temp82= arr; void** _temp84= _temp82.curr +
i; if( _temp84 < _temp82.base? 1: _temp84 >= _temp82.last_plus_one){ _throw(
Null_Exception);}* _temp84;});({ struct _tagged_ptr0 _temp85= arr; void**
_temp87= _temp85.curr + i; if( _temp87 < _temp85.base? 1: _temp87 >= _temp85.last_plus_one){
_throw( Null_Exception);}* _temp87=({ struct _tagged_ptr0 _temp88= arr; void**
_temp90= _temp88.curr + j; if( _temp90 < _temp88.base? 1: _temp90 >= _temp88.last_plus_one){
_throw( Null_Exception);}* _temp90;});});({ struct _tagged_ptr0 _temp91= arr;
void** _temp93= _temp91.curr + j; if( _temp93 < _temp91.base? 1: _temp93 >=
_temp91.last_plus_one){ _throw( Null_Exception);}* _temp93= temp;});;} temp=({
struct _tagged_ptr0 _temp94= arr; void** _temp96= _temp94.curr + base_ofs; if(
_temp96 < _temp94.base? 1: _temp96 >= _temp94.last_plus_one){ _throw(
Null_Exception);}* _temp96;});({ struct _tagged_ptr0 _temp97= arr; void**
_temp99= _temp97.curr + base_ofs; if( _temp99 < _temp97.base? 1: _temp99 >=
_temp97.last_plus_one){ _throw( Null_Exception);}* _temp99=({ struct
_tagged_ptr0 _temp100= arr; void** _temp102= _temp100.curr + j; if( _temp102 <
_temp100.base? 1: _temp102 >= _temp100.last_plus_one){ _throw( Null_Exception);}*
_temp102;});});({ struct _tagged_ptr0 _temp103= arr; void** _temp105= _temp103.curr
+ j; if( _temp105 < _temp103.base? 1: _temp105 >= _temp103.last_plus_one){
_throw( Null_Exception);}* _temp105= temp;}); if( j - base_ofs > limit_ofs - i){({
int* _temp106=( int*) Cyc_Array_sp; unsigned int _temp107= sp_ofs; if( _temp107
>= 40u){ _throw( Null_Exception);} _temp106[ _temp107]= base_ofs;});({ int*
_temp108=( int*) Cyc_Array_sp; unsigned int _temp109= sp_ofs + 1; if( _temp109
>= 40u){ _throw( Null_Exception);} _temp108[ _temp109]= j;}); base_ofs= i;}
else{({ int* _temp110=( int*) Cyc_Array_sp; unsigned int _temp111= sp_ofs; if(
_temp111 >= 40u){ _throw( Null_Exception);} _temp110[ _temp111]= i;});({ int*
_temp112=( int*) Cyc_Array_sp; unsigned int _temp113= sp_ofs + 1; if( _temp113
>= 40u){ _throw( Null_Exception);} _temp112[ _temp113]= limit_ofs;}); limit_ofs=
j;} sp_ofs += 2;} else{ for( j= base_ofs, i= j + 1; i < limit_ofs; j= i, i ++){
for( 0; less_eq(({ struct _tagged_ptr0 _temp114= arr; void** _temp116= _temp114.curr
+ j; if( _temp116 < _temp114.base? 1: _temp116 >= _temp114.last_plus_one){
_throw( Null_Exception);}* _temp116;}),({ struct _tagged_ptr0 _temp117= arr;
void** _temp119= _temp117.curr +( j + 1); if( _temp119 < _temp117.base? 1:
_temp119 >= _temp117.last_plus_one){ _throw( Null_Exception);}* _temp119;})) > 0;
j --){ temp=({ struct _tagged_ptr0 _temp120= arr; void** _temp122= _temp120.curr
+ j; if( _temp122 < _temp120.base? 1: _temp122 >= _temp120.last_plus_one){
_throw( Null_Exception);}* _temp122;});({ struct _tagged_ptr0 _temp123= arr;
void** _temp125= _temp123.curr + j; if( _temp125 < _temp123.base? 1: _temp125 >=
_temp123.last_plus_one){ _throw( Null_Exception);}* _temp125=({ struct
_tagged_ptr0 _temp126= arr; void** _temp128= _temp126.curr +( j + 1); if(
_temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one){ _throw(
Null_Exception);}* _temp128;});});({ struct _tagged_ptr0 _temp129= arr; void**
_temp131= _temp129.curr +( j + 1); if( _temp131 < _temp129.base? 1: _temp131 >=
_temp129.last_plus_one){ _throw( Null_Exception);}* _temp131= temp;}); if( j ==
base_ofs){ break;}}} if( sp_ofs != 0){ sp_ofs -= 2; base_ofs=({ int* _temp132=(
int*) Cyc_Array_sp; unsigned int _temp133= sp_ofs; if( _temp133 >= 40u){ _throw(
Null_Exception);} _temp132[ _temp133];}); limit_ofs=({ int* _temp134=( int*) Cyc_Array_sp;
unsigned int _temp135= sp_ofs + 1; if( _temp135 >= 40u){ _throw( Null_Exception);}
_temp134[ _temp135];});} else{ break;}}}} void Cyc_Array_msort( int(* less_eq)(
void*, void*), struct _tagged_ptr0 arr, int base_ofs, int len){ if(( base_ofs <
0? 1:( unsigned int)( base_ofs + len) >({ struct _tagged_ptr0 _temp136= arr;(
unsigned int)( _temp136.last_plus_one - _temp136.curr);}))? 1: len < 0){( void)
_throw(({ struct Cyc_Core_InvalidArg_struct* _temp137=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp137=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp138=( char*)"Array::msort";
struct _tagged_string _temp139; _temp139.curr= _temp138; _temp139.base= _temp138;
_temp139.last_plus_one= _temp138 + 13; _temp139;})};( struct _xenum_struct*)
_temp137;}));}{ int limit_ofs= base_ofs + len; struct _tagged_ptr0 from=({
unsigned int _temp143=( unsigned int) len; void** _temp144=( void**) GC_malloc(
sizeof( void*) * _temp143); unsigned int i; struct _tagged_ptr0 _temp145={
_temp144, _temp144, _temp144 + _temp143}; for( i= 0; i < _temp143; i ++){
_temp144[ i]=({ struct _tagged_ptr0 _temp140= arr; void** _temp142= _temp140.curr
+ 0; if( _temp142 < _temp140.base? 1: _temp142 >= _temp140.last_plus_one){
_throw( Null_Exception);}* _temp142;});} _temp145;}); struct _tagged_ptr0 to=({
unsigned int _temp149=( unsigned int) len; void** _temp150=( void**) GC_malloc(
sizeof( void*) * _temp149); unsigned int i; struct _tagged_ptr0 _temp151={
_temp150, _temp150, _temp150 + _temp149}; for( i= 0; i < _temp149; i ++){
_temp150[ i]=({ struct _tagged_ptr0 _temp146= arr; void** _temp148= _temp146.curr
+( int)( i +( unsigned int) base_ofs); if( _temp148 < _temp146.base? 1: _temp148
>= _temp146.last_plus_one){ _throw( Null_Exception);}* _temp148;});} _temp151;});
struct _tagged_ptr0 swap; int stepsize; int start; int lstart; int lend; int
rstart; int rend; int dest; for( stepsize= 1; stepsize < len; stepsize= stepsize
* 2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start < limit_ofs;
start= start + stepsize * 2){ lstart= start; rstart= start + stepsize <
limit_ofs? start + stepsize: limit_ofs; lend= rstart; rend= start + stepsize * 2
< limit_ofs? start + stepsize * 2: limit_ofs; while( lstart < lend? rstart <
rend: 0) { if( less_eq(({ struct _tagged_ptr0 _temp152= from; void** _temp154=
_temp152.curr + lstart; if( _temp154 < _temp152.base? 1: _temp154 >= _temp152.last_plus_one){
_throw( Null_Exception);}* _temp154;}),({ struct _tagged_ptr0 _temp155= from;
void** _temp157= _temp155.curr + rstart; if( _temp157 < _temp155.base? 1:
_temp157 >= _temp155.last_plus_one){ _throw( Null_Exception);}* _temp157;})) <=
0){({ struct _tagged_ptr0 _temp158= to; void** _temp160= _temp158.curr +( dest
++); if( _temp160 < _temp158.base? 1: _temp160 >= _temp158.last_plus_one){
_throw( Null_Exception);}* _temp160=({ struct _tagged_ptr0 _temp161= from; void**
_temp163= _temp161.curr +( lstart ++); if( _temp163 < _temp161.base? 1: _temp163
>= _temp161.last_plus_one){ _throw( Null_Exception);}* _temp163;});});} else{({
struct _tagged_ptr0 _temp164= to; void** _temp166= _temp164.curr +( dest ++);
if( _temp166 < _temp164.base? 1: _temp166 >= _temp164.last_plus_one){ _throw(
Null_Exception);}* _temp166=({ struct _tagged_ptr0 _temp167= from; void**
_temp169= _temp167.curr +( rstart ++); if( _temp169 < _temp167.base? 1: _temp169
>= _temp167.last_plus_one){ _throw( Null_Exception);}* _temp169;});});}} while(
lstart < lend) {({ struct _tagged_ptr0 _temp170= to; void** _temp172= _temp170.curr
+( dest ++); if( _temp172 < _temp170.base? 1: _temp172 >= _temp170.last_plus_one){
_throw( Null_Exception);}* _temp172=({ struct _tagged_ptr0 _temp173= from; void**
_temp175= _temp173.curr +( lstart ++); if( _temp175 < _temp173.base? 1: _temp175
>= _temp173.last_plus_one){ _throw( Null_Exception);}* _temp175;});});} while(
rstart < rend) {({ struct _tagged_ptr0 _temp176= to; void** _temp178= _temp176.curr
+( dest ++); if( _temp178 < _temp176.base? 1: _temp178 >= _temp176.last_plus_one){
_throw( Null_Exception);}* _temp178=({ struct _tagged_ptr0 _temp179= from; void**
_temp181= _temp179.curr +( rstart ++); if( _temp181 < _temp179.base? 1: _temp181
>= _temp179.last_plus_one){ _throw( Null_Exception);}* _temp181;});});}}}{ int i=
0; for( 0; i < len; i ++){({ struct _tagged_ptr0 _temp182= arr; void** _temp184=
_temp182.curr +( i + base_ofs); if( _temp184 < _temp182.base? 1: _temp184 >=
_temp182.last_plus_one){ _throw( Null_Exception);}* _temp184=({ struct
_tagged_ptr0 _temp185= to; void** _temp187= _temp185.curr + i; if( _temp187 <
_temp185.base? 1: _temp187 >= _temp185.last_plus_one){ _throw( Null_Exception);}*
_temp187;});});}}}} struct _tagged_ptr0 Cyc_Array_from_list( struct Cyc_List_List*
x){ return(( struct _tagged_ptr0(*)( struct Cyc_List_List* x)) Cyc_List_to_array)(
x);} struct Cyc_List_List* Cyc_Array_to_list( struct _tagged_ptr0 x){ return((
struct Cyc_List_List*(*)( struct _tagged_ptr0 arr)) Cyc_List_from_array)( x);}
struct _tagged_ptr0 Cyc_Array_copy( struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp188= x;( unsigned int)( _temp188.last_plus_one -
_temp188.curr);}); return({ unsigned int _temp192=( unsigned int) sx; void**
_temp193=( void**) GC_malloc( sizeof( void*) * _temp192); unsigned int i; struct
_tagged_ptr0 _temp194={ _temp193, _temp193, _temp193 + _temp192}; for( i= 0; i <
_temp192; i ++){ _temp193[ i]=({ struct _tagged_ptr0 _temp189= x; void**
_temp191= _temp189.curr +( int) i; if( _temp191 < _temp189.base? 1: _temp191 >=
_temp189.last_plus_one){ _throw( Null_Exception);}* _temp191;});} _temp194;});}
struct _tagged_ptr0 Cyc_Array_map( void*(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp195= x;( unsigned int)( _temp195.last_plus_one
- _temp195.curr);}); return({ unsigned int _temp199=( unsigned int) sx; void**
_temp200=( void**) GC_malloc( sizeof( void*) * _temp199); unsigned int i; struct
_tagged_ptr0 _temp201={ _temp200, _temp200, _temp200 + _temp199}; for( i= 0; i <
_temp199; i ++){ _temp200[ i]= f(({ struct _tagged_ptr0 _temp196= x; void**
_temp198= _temp196.curr +( int) i; if( _temp198 < _temp196.base? 1: _temp198 >=
_temp196.last_plus_one){ _throw( Null_Exception);}* _temp198;}));} _temp201;});}
struct _tagged_ptr0 Cyc_Array_map_c( void*(* f)( void*, void*), void* env,
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp202= x;(
unsigned int)( _temp202.last_plus_one - _temp202.curr);}); return({ unsigned int
_temp206=( unsigned int) sx; void** _temp207=( void**) GC_malloc( sizeof( void*)
* _temp206); unsigned int i; struct _tagged_ptr0 _temp208={ _temp207, _temp207,
_temp207 + _temp206}; for( i= 0; i < _temp206; i ++){ _temp207[ i]= f( env,({
struct _tagged_ptr0 _temp203= x; void** _temp205= _temp203.curr +( int) i; if(
_temp205 < _temp203.base? 1: _temp205 >= _temp203.last_plus_one){ _throw(
Null_Exception);}* _temp205;}));} _temp208;});} void Cyc_Array_imp_map( void*(*
f)( void*), struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp209=
x;( unsigned int)( _temp209.last_plus_one - _temp209.curr);}); int i= 0; for( 0;
i < sx; i ++){({ struct _tagged_ptr0 _temp210= x; void** _temp212= _temp210.curr
+ i; if( _temp212 < _temp210.base? 1: _temp212 >= _temp210.last_plus_one){
_throw( Null_Exception);}* _temp212= f(({ struct _tagged_ptr0 _temp213= x; void**
_temp215= _temp213.curr + i; if( _temp215 < _temp213.base? 1: _temp215 >=
_temp213.last_plus_one){ _throw( Null_Exception);}* _temp215;}));});}} void Cyc_Array_imp_map_c(
void*(* f)( void*, void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp216= x;( unsigned int)( _temp216.last_plus_one -
_temp216.curr);}); int i= 0; for( 0; i < sx; i ++){({ struct _tagged_ptr0
_temp217= x; void** _temp219= _temp217.curr + i; if( _temp219 < _temp217.base? 1:
_temp219 >= _temp217.last_plus_one){ _throw( Null_Exception);}* _temp219= f( env,({
struct _tagged_ptr0 _temp220= x; void** _temp222= _temp220.curr + i; if(
_temp222 < _temp220.base? 1: _temp222 >= _temp220.last_plus_one){ _throw(
Null_Exception);}* _temp222;}));});}} char Cyc_Array_Array_mismatch_tag[ 15u]="Array_mismatch";
struct _tagged_ptr0 Cyc_Array_map2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp223= x;( unsigned int)( _temp223.last_plus_one - _temp223.curr);}); if((
unsigned int) sx !=({ struct _tagged_ptr0 _temp224= y;( unsigned int)( _temp224.last_plus_one
- _temp224.curr);})){( void) _throw(({ struct Cyc_Array_Array_mismatch_struct*
_temp225=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Array_Array_mismatch_struct));* _temp225=( struct Cyc_Array_Array_mismatch_struct){.tag=
Cyc_Array_Array_mismatch_tag};( struct _xenum_struct*) _temp225;}));} return({
unsigned int _temp232=( unsigned int) sx; void** _temp233=( void**) GC_malloc(
sizeof( void*) * _temp232); unsigned int i; struct _tagged_ptr0 _temp234={
_temp233, _temp233, _temp233 + _temp232}; for( i= 0; i < _temp232; i ++){
_temp233[ i]= f(({ struct _tagged_ptr0 _temp226= x; void** _temp228= _temp226.curr
+( int) i; if( _temp228 < _temp226.base? 1: _temp228 >= _temp226.last_plus_one){
_throw( Null_Exception);}* _temp228;}),({ struct _tagged_ptr0 _temp229= y; void**
_temp231= _temp229.curr +( int) i; if( _temp231 < _temp229.base? 1: _temp231 >=
_temp229.last_plus_one){ _throw( Null_Exception);}* _temp231;}));} _temp234;});}
void Cyc_Array_app( void*(* f)( void*), struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp235= x;( unsigned int)( _temp235.last_plus_one -
_temp235.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp236= x; void** _temp238= _temp236.curr + i; if( _temp238 < _temp236.base? 1:
_temp238 >= _temp236.last_plus_one){ _throw( Null_Exception);}* _temp238;}));}}
void Cyc_Array_app_c( void*(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp239= x;( unsigned int)( _temp239.last_plus_one
- _temp239.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,({ struct
_tagged_ptr0 _temp240= x; void** _temp242= _temp240.curr + i; if( _temp242 <
_temp240.base? 1: _temp242 >= _temp240.last_plus_one){ _throw( Null_Exception);}*
_temp242;}));}} void Cyc_Array_iter( void(* f)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp243= x;( unsigned int)( _temp243.last_plus_one
- _temp243.curr);}); int i= 0; for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0
_temp244= x; void** _temp246= _temp244.curr + i; if( _temp246 < _temp244.base? 1:
_temp246 >= _temp244.last_plus_one){ _throw( Null_Exception);}* _temp246;}));}}
void Cyc_Array_iter_c( void(* f)( void*, void*), void* env, struct _tagged_ptr0
x){ int sx=( int)({ struct _tagged_ptr0 _temp247= x;( unsigned int)( _temp247.last_plus_one
- _temp247.curr);}); int i= 0; for( 0; i < sx; i ++){ f( env,({ struct
_tagged_ptr0 _temp248= x; void** _temp250= _temp248.curr + i; if( _temp250 <
_temp248.base? 1: _temp250 >= _temp248.last_plus_one){ _throw( Null_Exception);}*
_temp250;}));}} void Cyc_Array_app2( void*(* f)( void*, void*), struct
_tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0
_temp251= x;( unsigned int)( _temp251.last_plus_one - _temp251.curr);}); if((
unsigned int) sx !=({ struct _tagged_ptr0 _temp252= y;( unsigned int)( _temp252.last_plus_one
- _temp252.curr);})){( void) _throw(({ struct Cyc_Array_Array_mismatch_struct*
_temp253=( struct Cyc_Array_Array_mismatch_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Array_Array_mismatch_struct));* _temp253=( struct Cyc_Array_Array_mismatch_struct){.tag=
Cyc_Array_Array_mismatch_tag};( struct _xenum_struct*) _temp253;}));}{ int i= 0;
for( 0; i < sx; i ++){ f(({ struct _tagged_ptr0 _temp254= x; void** _temp256=
_temp254.curr + i; if( _temp256 < _temp254.base? 1: _temp256 >= _temp254.last_plus_one){
_throw( Null_Exception);}* _temp256;}),({ struct _tagged_ptr0 _temp257= y; void**
_temp259= _temp257.curr + i; if( _temp259 < _temp257.base? 1: _temp259 >=
_temp257.last_plus_one){ _throw( Null_Exception);}* _temp259;}));}}} void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp260= x;( unsigned int)(
_temp260.last_plus_one - _temp260.curr);}); if(( unsigned int) sx !=({ struct
_tagged_ptr0 _temp261= y;( unsigned int)( _temp261.last_plus_one - _temp261.curr);})){(
void) _throw(({ struct Cyc_Array_Array_mismatch_struct* _temp262=( struct Cyc_Array_Array_mismatch_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Array_Array_mismatch_struct));* _temp262=(
struct Cyc_Array_Array_mismatch_struct){.tag= Cyc_Array_Array_mismatch_tag};(
struct _xenum_struct*) _temp262;}));}{ int i= 0; for( 0; i < sx; i ++){ f( env,({
struct _tagged_ptr0 _temp263= x; void** _temp265= _temp263.curr + i; if(
_temp265 < _temp263.base? 1: _temp265 >= _temp263.last_plus_one){ _throw(
Null_Exception);}* _temp265;}),({ struct _tagged_ptr0 _temp266= y; void**
_temp268= _temp266.curr + i; if( _temp268 < _temp266.base? 1: _temp268 >=
_temp266.last_plus_one){ _throw( Null_Exception);}* _temp268;}));}}} void Cyc_Array_iter2(
void(* f)( void*, void*), struct _tagged_ptr0 x, struct _tagged_ptr0 y){ int sx=(
int)({ struct _tagged_ptr0 _temp269= x;( unsigned int)( _temp269.last_plus_one -
_temp269.curr);}); if(( unsigned int) sx !=({ struct _tagged_ptr0 _temp270= y;(
unsigned int)( _temp270.last_plus_one - _temp270.curr);})){( void) _throw(({
struct Cyc_Array_Array_mismatch_struct* _temp271=( struct Cyc_Array_Array_mismatch_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Array_Array_mismatch_struct));* _temp271=(
struct Cyc_Array_Array_mismatch_struct){.tag= Cyc_Array_Array_mismatch_tag};(
struct _xenum_struct*) _temp271;}));}{ int i= 0; for( 0; i < sx; i ++){ f(({
struct _tagged_ptr0 _temp272= x; void** _temp274= _temp272.curr + i; if(
_temp274 < _temp272.base? 1: _temp274 >= _temp272.last_plus_one){ _throw(
Null_Exception);}* _temp274;}),({ struct _tagged_ptr0 _temp275= y; void**
_temp277= _temp275.curr + i; if( _temp277 < _temp275.base? 1: _temp277 >=
_temp275.last_plus_one){ _throw( Null_Exception);}* _temp277;}));}}} void Cyc_Array_iter2_c(
void(* f)( void*, void*, void*), void* env, struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp278= x;( unsigned int)(
_temp278.last_plus_one - _temp278.curr);}); if(( unsigned int) sx !=({ struct
_tagged_ptr0 _temp279= y;( unsigned int)( _temp279.last_plus_one - _temp279.curr);})){(
void) _throw(({ struct Cyc_Array_Array_mismatch_struct* _temp280=( struct Cyc_Array_Array_mismatch_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Array_Array_mismatch_struct));* _temp280=(
struct Cyc_Array_Array_mismatch_struct){.tag= Cyc_Array_Array_mismatch_tag};(
struct _xenum_struct*) _temp280;}));}{ int i= 0; for( 0; i < sx; i ++){ f( env,({
struct _tagged_ptr0 _temp281= x; void** _temp283= _temp281.curr + i; if(
_temp283 < _temp281.base? 1: _temp283 >= _temp281.last_plus_one){ _throw(
Null_Exception);}* _temp283;}),({ struct _tagged_ptr0 _temp284= y; void**
_temp286= _temp284.curr + i; if( _temp286 < _temp284.base? 1: _temp286 >=
_temp284.last_plus_one){ _throw( Null_Exception);}* _temp286;}));}}} void* Cyc_Array_fold_left(
void*(* f)( void*, void*), void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp287= x;( unsigned int)( _temp287.last_plus_one -
_temp287.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( accum,({ struct
_tagged_ptr0 _temp288= x; void** _temp290= _temp288.curr + i; if( _temp290 <
_temp288.base? 1: _temp290 >= _temp288.last_plus_one){ _throw( Null_Exception);}*
_temp290;}));}} return accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*,
void*, void*), void* env, void* accum, struct _tagged_ptr0 x){ int sx=( int)({
struct _tagged_ptr0 _temp291= x;( unsigned int)( _temp291.last_plus_one -
_temp291.curr);});{ int i= 0; for( 0; i < sx; i ++){ accum= f( env, accum,({
struct _tagged_ptr0 _temp292= x; void** _temp294= _temp292.curr + i; if(
_temp294 < _temp292.base? 1: _temp294 >= _temp292.last_plus_one){ _throw(
Null_Exception);}* _temp294;}));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_ptr0 x, void* accum){{ int i=( int)(({
struct _tagged_ptr0 _temp295= x;( unsigned int)( _temp295.last_plus_one -
_temp295.curr);}) -( unsigned int) 1); for( 0; i >= 0; i --){ accum= f(({ struct
_tagged_ptr0 _temp296= x; void** _temp298= _temp296.curr + i; if( _temp298 <
_temp296.base? 1: _temp298 >= _temp296.last_plus_one){ _throw( Null_Exception);}*
_temp298;}), accum);}} return accum;} void* Cyc_Array_fold_right_c( void*(* f)(
void*, void*, void*), void* env, struct _tagged_ptr0 x, void* accum){{ int i=(
int)(({ struct _tagged_ptr0 _temp299= x;( unsigned int)( _temp299.last_plus_one
- _temp299.curr);}) -( unsigned int) 1); for( 0; i >= 0; i --){ accum= f( env,({
struct _tagged_ptr0 _temp300= x; void** _temp302= _temp300.curr + i; if(
_temp302 < _temp300.base? 1: _temp302 >= _temp300.last_plus_one){ _throw(
Null_Exception);}* _temp302;}), accum);}} return accum;} struct _tagged_ptr0 Cyc_Array_rev_copy(
struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0 _temp303= x;(
unsigned int)( _temp303.last_plus_one - _temp303.curr);}); int n= sx - 1; return({
unsigned int _temp307=( unsigned int) sx; void** _temp308=( void**) GC_malloc(
sizeof( void*) * _temp307); unsigned int i; struct _tagged_ptr0 _temp309={
_temp308, _temp308, _temp308 + _temp307}; for( i= 0; i < _temp307; i ++){
_temp308[ i]=({ struct _tagged_ptr0 _temp304= x; void** _temp306= _temp304.curr
+( int)(( unsigned int) n - i); if( _temp306 < _temp304.base? 1: _temp306 >=
_temp304.last_plus_one){ _throw( Null_Exception);}* _temp306;});} _temp309;});}
void Cyc_Array_imp_rev( struct _tagged_ptr0 x){ void* temp; int i= 0; int j=(
int)(({ struct _tagged_ptr0 _temp310= x;( unsigned int)( _temp310.last_plus_one
- _temp310.curr);}) -( unsigned int) 1); while( i < j) { temp=({ struct
_tagged_ptr0 _temp311= x; void** _temp313= _temp311.curr + i; if( _temp313 <
_temp311.base? 1: _temp313 >= _temp311.last_plus_one){ _throw( Null_Exception);}*
_temp313;});({ struct _tagged_ptr0 _temp314= x; void** _temp316= _temp314.curr +
i; if( _temp316 < _temp314.base? 1: _temp316 >= _temp314.last_plus_one){ _throw(
Null_Exception);}* _temp316=({ struct _tagged_ptr0 _temp317= x; void** _temp319=
_temp317.curr + j; if( _temp319 < _temp317.base? 1: _temp319 >= _temp317.last_plus_one){
_throw( Null_Exception);}* _temp319;});});({ struct _tagged_ptr0 _temp320= x;
void** _temp322= _temp320.curr + j; if( _temp322 < _temp320.base? 1: _temp322 >=
_temp320.last_plus_one){ _throw( Null_Exception);}* _temp322= temp;}); i ++; j
--;}} int Cyc_Array_forall( int(* pred)( void*), struct _tagged_ptr0 x){ int sx=(
int)({ struct _tagged_ptr0 _temp323= x;( unsigned int)( _temp323.last_plus_one -
_temp323.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( ! pred(({ struct
_tagged_ptr0 _temp324= x; void** _temp326= _temp324.curr + i; if( _temp326 <
_temp324.base? 1: _temp326 >= _temp324.last_plus_one){ _throw( Null_Exception);}*
_temp326;}))){ return 0;}}} return 1;} int Cyc_Array_forall_c( int(* pred)( void*,
void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp327= x;( unsigned int)( _temp327.last_plus_one - _temp327.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( ! pred( env,({ struct _tagged_ptr0 _temp328= x;
void** _temp330= _temp328.curr + i; if( _temp330 < _temp328.base? 1: _temp330 >=
_temp328.last_plus_one){ _throw( Null_Exception);}* _temp330;}))){ return 0;}}}
return 1;} int Cyc_Array_exists( int(* pred)( void*), struct _tagged_ptr0 x){
int sx=( int)({ struct _tagged_ptr0 _temp331= x;( unsigned int)( _temp331.last_plus_one
- _temp331.curr);});{ int i= 0; for( 0; i < sx; i ++){ if( pred(({ struct
_tagged_ptr0 _temp332= x; void** _temp334= _temp332.curr + i; if( _temp334 <
_temp332.base? 1: _temp334 >= _temp332.last_plus_one){ _throw( Null_Exception);}*
_temp334;}))){ return 1;}}} return 0;} int Cyc_Array_exists_c( int(* pred)( void*,
void*), void* env, struct _tagged_ptr0 x){ int sx=( int)({ struct _tagged_ptr0
_temp335= x;( unsigned int)( _temp335.last_plus_one - _temp335.curr);});{ int i=
0; for( 0; i < sx; i ++){ if( pred( env,({ struct _tagged_ptr0 _temp336= x; void**
_temp338= _temp336.curr + i; if( _temp338 < _temp336.base? 1: _temp338 >=
_temp336.last_plus_one){ _throw( Null_Exception);}* _temp338;}))){ return 1;}}}
return 0;} struct _tagged_ptr1 Cyc_Array_zip( struct _tagged_ptr0 x, struct
_tagged_ptr0 y){ int sx=( int)({ struct _tagged_ptr0 _temp339= x;( unsigned int)(
_temp339.last_plus_one - _temp339.curr);}); if(( unsigned int) sx !=({ struct
_tagged_ptr0 _temp340= y;( unsigned int)( _temp340.last_plus_one - _temp340.curr);})){(
void) _throw(({ struct Cyc_Array_Array_mismatch_struct* _temp341=( struct Cyc_Array_Array_mismatch_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Array_Array_mismatch_struct));* _temp341=(
struct Cyc_Array_Array_mismatch_struct){.tag= Cyc_Array_Array_mismatch_tag};(
struct _xenum_struct*) _temp341;}));} return({ unsigned int _temp349=(
unsigned int) sx; struct _tuple0* _temp350=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * _temp349); unsigned int i; struct _tagged_ptr1 _temp351={
_temp350, _temp350, _temp350 + _temp349}; for( i= 0; i < _temp349; i ++){
_temp350[ i]=({ struct _tuple0 _temp342; _temp342.f1=({ struct _tagged_ptr0
_temp346= x; void** _temp348= _temp346.curr +( int) i; if( _temp348 < _temp346.base?
1: _temp348 >= _temp346.last_plus_one){ _throw( Null_Exception);}* _temp348;});
_temp342.f2=({ struct _tagged_ptr0 _temp343= y; void** _temp345= _temp343.curr +(
int) i; if( _temp345 < _temp343.base? 1: _temp345 >= _temp343.last_plus_one){
_throw( Null_Exception);}* _temp345;}); _temp342;});} _temp351;});} struct
_tuple1 Cyc_Array_split( struct _tagged_ptr1 x){ int sx=( int)({ struct
_tagged_ptr1 _temp352= x;( unsigned int)( _temp352.last_plus_one - _temp352.curr);});
return({ struct _tuple1 _temp353; _temp353.f1=({ unsigned int _temp363=(
unsigned int) sx; void** _temp364=( void**) GC_malloc( sizeof( void*) * _temp363);
unsigned int i; struct _tagged_ptr0 _temp365={ _temp364, _temp364, _temp364 +
_temp363}; for( i= 0; i < _temp363; i ++){ _temp364[ i]=({ struct _tagged_ptr1
_temp360= x; struct _tuple0* _temp362= _temp360.curr +( int) i; if( _temp362 <
_temp360.base? 1: _temp362 >= _temp360.last_plus_one){ _throw( Null_Exception);}*
_temp362;}).f1;} _temp365;}); _temp353.f2=({ unsigned int _temp357=(
unsigned int) sx; void** _temp358=( void**) GC_malloc( sizeof( void*) * _temp357);
unsigned int i; struct _tagged_ptr0 _temp359={ _temp358, _temp358, _temp358 +
_temp357}; for( i= 0; i < _temp357; i ++){ _temp358[ i]=({ struct _tagged_ptr1
_temp354= x; struct _tuple0* _temp356= _temp354.curr +( int) i; if( _temp356 <
_temp354.base? 1: _temp356 >= _temp354.last_plus_one){ _throw( Null_Exception);}*
_temp356;}).f2;} _temp359;}); _temp353;});} int Cyc_Array_memq( struct
_tagged_ptr0 l, void* x){ int s=( int)({ struct _tagged_ptr0 _temp366= l;(
unsigned int)( _temp366.last_plus_one - _temp366.curr);});{ int i= 0; for( 0; i
< s; i ++){ if(({ struct _tagged_ptr0 _temp367= l; void** _temp369= _temp367.curr
+ i; if( _temp369 < _temp367.base? 1: _temp369 >= _temp367.last_plus_one){
_throw( Null_Exception);}* _temp369;}) == x){ return 1;}}} return 0;} int Cyc_Array_mem(
int(* compare)( void*, void*), struct _tagged_ptr0 l, void* x){ int s=( int)({
struct _tagged_ptr0 _temp370= l;( unsigned int)( _temp370.last_plus_one -
_temp370.curr);});{ int i= 0; for( 0; i < s; i ++){ if( 0 == compare(({ struct
_tagged_ptr0 _temp371= l; void** _temp373= _temp371.curr + i; if( _temp373 <
_temp371.base? 1: _temp373 >= _temp371.last_plus_one){ _throw( Null_Exception);}*
_temp373;}), x)){ return 1;}}} return 0;} struct _tagged_ptr0 Cyc_Array_extract(
struct _tagged_ptr0 x, int start, int* n_opt){ int sx=( int)({ struct
_tagged_ptr0 _temp374= x;( unsigned int)( _temp374.last_plus_one - _temp374.curr);});
int n= n_opt == 0? sx - start:* n_opt; if(( start < 0? 1: n <= 0)? 1: start +(
n_opt == 0? 0: n) > sx){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp375=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp375=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp376=( char*)"Array::extract"; struct
_tagged_string _temp377; _temp377.curr= _temp376; _temp377.base= _temp376;
_temp377.last_plus_one= _temp376 + 15; _temp377;})};( struct _xenum_struct*)
_temp375;}));} return({ unsigned int _temp381=( unsigned int) n; void** _temp382=(
void**) GC_malloc( sizeof( void*) * _temp381); unsigned int i; struct
_tagged_ptr0 _temp383={ _temp382, _temp382, _temp382 + _temp381}; for( i= 0; i <
_temp381; i ++){ _temp382[ i]=({ struct _tagged_ptr0 _temp378= x; void**
_temp380= _temp378.curr +( int)(( unsigned int) start + i); if( _temp380 <
_temp378.base? 1: _temp380 >= _temp378.last_plus_one){ _throw( Null_Exception);}*
_temp380;});} _temp383;});}