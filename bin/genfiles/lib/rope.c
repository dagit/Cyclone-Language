 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Rope_Rope_node; typedef
struct Cyc_Rope_Rope_node* Cyc_Rope_rope_t; extern struct Cyc_Rope_Rope_node*
Cyc_Rope_from_string( struct _tagged_string); extern struct _tagged_string Cyc_Rope_to_string(
struct Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*); struct _tagged_ptr0{
struct Cyc_Rope_Rope_node** curr; struct Cyc_Rope_Rope_node** base; struct Cyc_Rope_Rope_node**
last_plus_one; } ; extern struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct
_tagged_ptr0); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct Cyc_List_List*);
extern unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*); extern int Cyc_Rope_cmp(
struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned int Cyc_String_strlen( struct
_tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern struct _tagged_string Cyc_String_strncpy(
struct _tagged_string, int, struct _tagged_string, int, unsigned int); static
const int Cyc_Rope_String_rope= 0; struct Cyc_Rope_String_rope_struct{ int tag;
struct _tagged_string f1; } ; static const int Cyc_Rope_Array_rope= 1; struct
Cyc_Rope_Array_rope_struct{ int tag; struct _tagged_ptr0 f1; } ; struct Cyc_Rope_Rope_node{
void* v; } ; struct Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct
_tagged_string s){ return({ struct Cyc_Rope_Rope_node* _temp0=( struct Cyc_Rope_Rope_node*)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp0->v=( void*)(( void*)({
struct Cyc_Rope_String_rope_struct* _temp1=( struct Cyc_Rope_String_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp1[ 0]=({ struct
Cyc_Rope_String_rope_struct _temp2; _temp2.tag= Cyc_Rope_String_rope; _temp2.f1=
s; _temp2;}); _temp1;})); _temp0;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return({ struct
Cyc_Rope_Rope_node* _temp3=( struct Cyc_Rope_Rope_node*) GC_malloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp3->v=( void*)(( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp4=( struct Cyc_Rope_Array_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp4[ 0]=({ struct Cyc_Rope_Array_rope_struct _temp5; _temp5.tag= Cyc_Rope_Array_rope;
_temp5.f1=({ struct Cyc_Rope_Rope_node** _temp7=({ struct Cyc_Rope_Rope_node**
_temp6=( struct Cyc_Rope_Rope_node**) GC_malloc( sizeof( struct Cyc_Rope_Rope_node*)
* 2); _temp6[ 0]= r1; _temp6[ 1]= r2; _temp6;}); struct _tagged_ptr0 _temp8;
_temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 + 2;
_temp8;}); _temp5;}); _temp4;})); _temp3;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concata(
struct _tagged_ptr0 rs){ return({ struct Cyc_Rope_Rope_node* _temp9=( struct Cyc_Rope_Rope_node*)
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
sizeof( struct Cyc_Rope_Rope_node*) * _temp15); struct _tagged_ptr0 _temp18={
_temp16, _temp16, _temp16 + _temp15};{ unsigned int _temp17= _temp15;
unsigned int i; for( i= 0; i < _temp17; i ++){ _temp16[ i]=({ struct Cyc_Rope_Rope_node*
r=( struct Cyc_Rope_Rope_node*)(( struct Cyc_List_List*) _check_null( l))->hd; l=((
struct Cyc_List_List*) _check_null( l))->tl; r;});}}; _temp18;}); _temp14;});
_temp13;})); _temp12;});} unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*
r){ void* _temp19=( void*) r->v; struct _tagged_string _temp25; struct
_tagged_ptr0 _temp27; _LL21: if(*(( int*) _temp19) == Cyc_Rope_String_rope){
_LL26: _temp25=(( struct Cyc_Rope_String_rope_struct*) _temp19)->f1; goto _LL22;}
else{ goto _LL23;} _LL23: if(*(( int*) _temp19) == Cyc_Rope_Array_rope){ _LL28:
_temp27=(( struct Cyc_Rope_Array_rope_struct*) _temp19)->f1; goto _LL24;} else{
goto _LL20;} _LL22: return Cyc_String_strlen( _temp25); _LL24: { unsigned int
total= 0; unsigned int sz=({ struct _tagged_ptr0 _temp29= _temp27;( unsigned int)(
_temp29.last_plus_one - _temp29.curr);});{ unsigned int i= 0; for( 0; i < sz; i
++){ total += Cyc_Rope_length(*(( struct Cyc_Rope_Rope_node**(*)( struct
_tagged_ptr0, unsigned int, unsigned int)) _check_unknown_subscript)( _temp27,
sizeof( struct Cyc_Rope_Rope_node*),( int) i));}} return total;} _LL20:;} static
unsigned int Cyc_Rope_flatten_it( struct _tagged_string s, unsigned int i,
struct Cyc_Rope_Rope_node* r){ void* _temp30=( void*) r->v; struct
_tagged_string _temp36; struct _tagged_ptr0 _temp38; _LL32: if(*(( int*) _temp30)
== Cyc_Rope_String_rope){ _LL37: _temp36=(( struct Cyc_Rope_String_rope_struct*)
_temp30)->f1; goto _LL33;} else{ goto _LL34;} _LL34: if(*(( int*) _temp30) ==
Cyc_Rope_Array_rope){ _LL39: _temp38=(( struct Cyc_Rope_Array_rope_struct*)
_temp30)->f1; goto _LL35;} else{ goto _LL31;} _LL33: { unsigned int _temp40= Cyc_String_strlen(
_temp36); goto _LL41; _LL41: Cyc_String_strncpy( s,( int) i, _temp36, 0, _temp40);
return i + _temp40;} _LL35: { unsigned int _temp43=({ struct _tagged_ptr0
_temp42= _temp38;( unsigned int)( _temp42.last_plus_one - _temp42.curr);}); goto
_LL44; _LL44:{ int j= 0; for( 0; j < _temp43; j ++){ i= Cyc_Rope_flatten_it( s,
i,*(( struct Cyc_Rope_Rope_node**(*)( struct _tagged_ptr0, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp38, sizeof( struct Cyc_Rope_Rope_node*),
j));}} return i;} _LL31:;} struct _tagged_string Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*
r){ struct _tagged_string s= Cyc_Core_new_string(( int) Cyc_Rope_length( r));
Cyc_Rope_flatten_it( s, 0, r);( void*)( r->v=( void*)(( void*)({ struct Cyc_Rope_String_rope_struct*
_temp45=( struct Cyc_Rope_String_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct));
_temp45[ 0]=({ struct Cyc_Rope_String_rope_struct _temp46; _temp46.tag= Cyc_Rope_String_rope;
_temp46.f1= s; _temp46;}); _temp45;}))); return s;} int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*
r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_String_strcmp( Cyc_Rope_to_string(
r1), Cyc_Rope_to_string( r2));}