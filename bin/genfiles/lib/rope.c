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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern char Cyc_List_Nth[ 8u]; struct Cyc_Rope_Rope_node; typedef struct
Cyc_Rope_Rope_node* Cyc_Rope_rope_t; extern struct Cyc_Rope_Rope_node* Cyc_Rope_from_string(
struct _tagged_string); extern struct _tagged_string Cyc_Rope_to_string( struct
Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concat( struct
Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*); struct _tagged_ptr0{ struct
Cyc_Rope_Rope_node** curr; struct Cyc_Rope_Rope_node** base; struct Cyc_Rope_Rope_node**
last_plus_one; } ; extern struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct
_tagged_ptr0); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct Cyc_List_List*);
extern unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*); extern int Cyc_Rope_cmp(
struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern int
Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2); extern
struct _tagged_string Cyc_String_strncpy( struct _tagged_string, int, struct
_tagged_string, int, unsigned int); static const int Cyc_Rope_String_rope= 0;
struct Cyc_Rope_String_rope_struct{ int tag; struct _tagged_string f1; } ;
static const int Cyc_Rope_Array_rope= 1; struct Cyc_Rope_Array_rope_struct{ int
tag; struct _tagged_ptr0 f1; } ; struct Cyc_Rope_Rope_node{ void* v; } ; struct
Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct _tagged_string s){ return({
struct Cyc_Rope_Rope_node* _temp0=( struct Cyc_Rope_Rope_node*) GC_malloc(
sizeof( struct Cyc_Rope_Rope_node)); _temp0->v=( void*)(( void*)({ struct Cyc_Rope_String_rope_struct*
_temp1=( struct Cyc_Rope_String_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct));
_temp1[ 0]=({ struct Cyc_Rope_String_rope_struct _temp2; _temp2.tag= Cyc_Rope_String_rope;
_temp2.f1= s; _temp2;}); _temp1;})); _temp0;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return({ struct
Cyc_Rope_Rope_node* _temp3=( struct Cyc_Rope_Rope_node*) GC_malloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp3->v=( void*)(( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp4=( struct Cyc_Rope_Array_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp4[ 0]=({ struct Cyc_Rope_Array_rope_struct _temp5; _temp5.tag= Cyc_Rope_Array_rope;
_temp5.f1=( struct _tagged_ptr0)({ struct Cyc_Rope_Rope_node** _temp7=( struct
Cyc_Rope_Rope_node**)({ struct Cyc_Rope_Rope_node** _temp6=( struct Cyc_Rope_Rope_node**)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node*) * 2); _temp6[ 0]= r1; _temp6[ 1]=
r2; _temp6;}); struct _tagged_ptr0 _temp8; _temp8.curr= _temp7; _temp8.base=
_temp7; _temp8.last_plus_one= _temp7 + 2; _temp8;}); _temp5;}); _temp4;}));
_temp3;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct _tagged_ptr0 rs){
return({ struct Cyc_Rope_Rope_node* _temp9=( struct Cyc_Rope_Rope_node*)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp9->v=( void*)(( void*)({
struct Cyc_Rope_Array_rope_struct* _temp10=( struct Cyc_Rope_Array_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp10[ 0]=({ struct
Cyc_Rope_Array_rope_struct _temp11; _temp11.tag= Cyc_Rope_Array_rope; _temp11.f1=
rs; _temp11;}); _temp10;})); _temp9;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concatl(
struct Cyc_List_List* l){ return({ struct Cyc_Rope_Rope_node* _temp12=( struct
Cyc_Rope_Rope_node*) GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp12->v=(
void*)(( void*)({ struct Cyc_Rope_Array_rope_struct* _temp13=( struct Cyc_Rope_Array_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp13[ 0]=({ struct
Cyc_Rope_Array_rope_struct _temp14; _temp14.tag= Cyc_Rope_Array_rope; _temp14.f1=({
unsigned int _temp15=( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
l); struct Cyc_Rope_Rope_node** _temp16=( struct Cyc_Rope_Rope_node**) GC_malloc(
sizeof( struct Cyc_Rope_Rope_node*) * _temp15); struct _tagged_ptr0 _temp19={
_temp16, _temp16, _temp16 + _temp15};{ unsigned int _temp17= _temp15;
unsigned int i; for( i= 0; i < _temp17; i ++){ _temp16[ i]=({ struct Cyc_Rope_Rope_node*
r=( struct Cyc_Rope_Rope_node*)(( struct Cyc_List_List*) _check_null( l))->hd; l=((
struct Cyc_List_List*) _check_null( l))->tl; r;});}}; _temp19;}); _temp14;});
_temp13;})); _temp12;});} unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*
r){ void* _temp20=( void*) r->v; struct _tagged_string _temp26; struct
_tagged_ptr0 _temp28; _LL22: if(*(( int*) _temp20) == Cyc_Rope_String_rope){
_LL27: _temp26=( struct _tagged_string)(( struct Cyc_Rope_String_rope_struct*)
_temp20)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp20) ==
Cyc_Rope_Array_rope){ _LL29: _temp28=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp20)->f1; goto _LL25;} else{ goto _LL21;} _LL23: return Cyc_String_strlen(
_temp26); _LL25: { unsigned int total=( unsigned int) 0; unsigned int sz=({
struct _tagged_ptr0 _temp30= _temp28;( unsigned int)( _temp30.last_plus_one -
_temp30.curr);});{ unsigned int i=( unsigned int) 0; for( 0; i < sz; i ++){
total += Cyc_Rope_length(*(( struct Cyc_Rope_Rope_node**(*)( struct _tagged_ptr0,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp28, sizeof( struct
Cyc_Rope_Rope_node*),( int) i));}} return total;} _LL21:;} static unsigned int
Cyc_Rope_flatten_it( struct _tagged_string s, unsigned int i, struct Cyc_Rope_Rope_node*
r){ void* _temp31=( void*) r->v; struct _tagged_string _temp37; struct
_tagged_ptr0 _temp39; _LL33: if(*(( int*) _temp31) == Cyc_Rope_String_rope){
_LL38: _temp37=( struct _tagged_string)(( struct Cyc_Rope_String_rope_struct*)
_temp31)->f1; goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp31) ==
Cyc_Rope_Array_rope){ _LL40: _temp39=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp31)->f1; goto _LL36;} else{ goto _LL32;} _LL34: { unsigned int _temp41= Cyc_String_strlen(
_temp37); goto _LL42; _LL42: Cyc_String_strncpy( s,( int) i, _temp37, 0, _temp41);
return i + _temp41;} _LL36: { unsigned int _temp44=({ struct _tagged_ptr0
_temp43= _temp39;( unsigned int)( _temp43.last_plus_one - _temp43.curr);}); goto
_LL45; _LL45:{ int j= 0; for( 0; j < _temp44; j ++){ i= Cyc_Rope_flatten_it( s,
i,*(( struct Cyc_Rope_Rope_node**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp39, sizeof( struct Cyc_Rope_Rope_node*),
j));}} return i;} _LL32:;} struct _tagged_string Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*
r){ struct _tagged_string s= Cyc_Core_new_string(( int) Cyc_Rope_length( r));
Cyc_Rope_flatten_it( s,( unsigned int) 0, r);( void*)( r->v=( void*)(( void*)({
struct Cyc_Rope_String_rope_struct* _temp46=( struct Cyc_Rope_String_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp46[ 0]=({ struct
Cyc_Rope_String_rope_struct _temp47; _temp47.tag= Cyc_Rope_String_rope; _temp47.f1=
s; _temp47;}); _temp46;}))); return s;} int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*
r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_String_strcmp( Cyc_Rope_to_string(
r1), Cyc_Rope_to_string( r2));}