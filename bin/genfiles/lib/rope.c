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
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern
int Cyc_String_strcmp( struct _tagged_string s1, struct _tagged_string s2);
extern struct _tagged_string Cyc_String_strncpy( struct _tagged_string, int,
struct _tagged_string, int, unsigned int); static const int Cyc_Rope_String_rope=
0; struct Cyc_Rope_String_rope_struct{ int tag; struct _tagged_string f1; } ;
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
sizeof( struct Cyc_Rope_Rope_node*) * _temp15); struct _tagged_ptr0 _temp21={
_temp16, _temp16, _temp16 + _temp15};{ unsigned int _temp17= _temp15;
unsigned int i; for( i= 0; i < _temp17; i ++){ _temp16[ i]=({ struct Cyc_Rope_Rope_node*
r=( struct Cyc_Rope_Rope_node*)({ struct Cyc_List_List* _temp20= l; if( _temp20
== 0){ _throw( Null_Exception);} _temp20->hd;}); l=({ struct Cyc_List_List*
_temp19= l; if( _temp19 == 0){ _throw( Null_Exception);} _temp19->tl;}); r;});}};
_temp21;}); _temp14;}); _temp13;})); _temp12;});} unsigned int Cyc_Rope_length(
struct Cyc_Rope_Rope_node* r){ void* _temp22=( void*) r->v; struct
_tagged_string _temp28; struct _tagged_ptr0 _temp30; _LL24: if((( struct
_tunion_struct*) _temp22)->tag == Cyc_Rope_String_rope){ _LL29: _temp28=( struct
_tagged_string)(( struct Cyc_Rope_String_rope_struct*) _temp22)->f1; goto _LL25;}
else{ goto _LL26;} _LL26: if((( struct _tunion_struct*) _temp22)->tag == Cyc_Rope_Array_rope){
_LL31: _temp30=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp22)->f1; goto _LL27;} else{ goto _LL23;} _LL25: return Cyc_String_strlen(
_temp28); _LL27: { unsigned int total=( unsigned int) 0; unsigned int sz=({
struct _tagged_ptr0 _temp35= _temp30;( unsigned int)( _temp35.last_plus_one -
_temp35.curr);});{ unsigned int i=( unsigned int) 0; for( 0; i < sz; i ++){
total += Cyc_Rope_length(({ struct _tagged_ptr0 _temp32= _temp30; struct Cyc_Rope_Rope_node**
_temp34= _temp32.curr +( int) i; if( _temp32.base == 0? 1:( _temp34 < _temp32.base?
1: _temp34 >= _temp32.last_plus_one)){ _throw( Null_Exception);}* _temp34;}));}}
return total;} _LL23:;} static unsigned int Cyc_Rope_flatten_it( struct
_tagged_string s, unsigned int i, struct Cyc_Rope_Rope_node* r){ void* _temp36=(
void*) r->v; struct _tagged_string _temp42; struct _tagged_ptr0 _temp44; _LL38:
if((( struct _tunion_struct*) _temp36)->tag == Cyc_Rope_String_rope){ _LL43:
_temp42=( struct _tagged_string)(( struct Cyc_Rope_String_rope_struct*) _temp36)->f1;
goto _LL39;} else{ goto _LL40;} _LL40: if((( struct _tunion_struct*) _temp36)->tag
== Cyc_Rope_Array_rope){ _LL45: _temp44=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp36)->f1; goto _LL41;} else{ goto _LL37;} _LL39: { unsigned int len= Cyc_String_strlen(
_temp42); Cyc_String_strncpy( s,( int) i, _temp42, 0, len); return i + len;}
_LL41: { unsigned int len=({ struct _tagged_ptr0 _temp49= _temp44;( unsigned int)(
_temp49.last_plus_one - _temp49.curr);});{ int j= 0; for( 0; j < len; j ++){ i=
Cyc_Rope_flatten_it( s, i,({ struct _tagged_ptr0 _temp46= _temp44; struct Cyc_Rope_Rope_node**
_temp48= _temp46.curr + j; if( _temp46.base == 0? 1:( _temp48 < _temp46.base? 1:
_temp48 >= _temp46.last_plus_one)){ _throw( Null_Exception);}* _temp48;}));}}
return i;} _LL37:;} struct _tagged_string Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*
r){ struct _tagged_string s= Cyc_Core_new_string(( int) Cyc_Rope_length( r));
Cyc_Rope_flatten_it( s,( unsigned int) 0, r);( void*)( r->v=( void*)(( void*)({
struct Cyc_Rope_String_rope_struct* _temp50=( struct Cyc_Rope_String_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp50[ 0]=({ struct
Cyc_Rope_String_rope_struct _temp51; _temp51.tag= Cyc_Rope_String_rope; _temp51.f1=
s; _temp51;}); _temp50;}))); return s;} int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*
r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_String_strcmp( Cyc_Rope_to_string(
r1), Cyc_Rope_to_string( r2));}