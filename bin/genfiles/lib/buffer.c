#include "cyc_include.h"

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
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int
Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Buffer_t; typedef
struct Cyc_Buffer_t* Cyc_Buffer_T; extern struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n); extern struct _tagged_string Cyc_Buffer_contents( struct Cyc_Buffer_t*);
extern unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t*); extern void Cyc_Buffer_clear(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_reset( struct Cyc_Buffer_t*);
extern void Cyc_Buffer_add_char( struct Cyc_Buffer_t*, unsigned char); extern
void Cyc_Buffer_add_substring( struct Cyc_Buffer_t*, struct _tagged_string, int
offset, int len); extern void Cyc_Buffer_add_string( struct Cyc_Buffer_t*,
struct _tagged_string); extern void Cyc_Buffer_add_buffer( struct Cyc_Buffer_t*
buf_dest, struct Cyc_Buffer_t* buf_source); struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern struct _tagged_string Cyc_String_strncpy(
struct _tagged_string, int, struct _tagged_string, int, unsigned int); extern
struct _tagged_string Cyc_String_zstrncpy( struct _tagged_string, int, struct
_tagged_string, int, unsigned int); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); struct Cyc_Buffer_t{ struct
_tagged_string buffer; unsigned int position; unsigned int length; struct
_tagged_string initial_buffer; } ; struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n){ if( n < 1){ n= 1;}{ struct _tagged_string s= Cyc_Core_new_string((
int) n); return({ struct Cyc_Buffer_t* _temp0=( struct Cyc_Buffer_t*) GC_malloc(
sizeof( struct Cyc_Buffer_t)); _temp0->buffer= s; _temp0->position= 0; _temp0->length=
n; _temp0->initial_buffer= s; _temp0;});}} struct _tagged_string Cyc_Buffer_contents(
struct Cyc_Buffer_t* b){ return Cyc_String_substring( b->buffer, 0, b->position);}
unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t* b){ return b->position;}
void Cyc_Buffer_clear( struct Cyc_Buffer_t* b){ b->position= 0; return;} void
Cyc_Buffer_reset( struct Cyc_Buffer_t* b){ b->position= 0; b->buffer= b->initial_buffer;
b->length=({ struct _tagged_string _temp1= b->buffer;( unsigned int)( _temp1.last_plus_one
- _temp1.curr);}); return;} static void Cyc_Buffer_resize( struct Cyc_Buffer_t*
b, unsigned int more){ unsigned int len= b->length; unsigned int new_len= len;
struct _tagged_string new_buffer; while( b->position + more > new_len) { new_len=
2 * new_len;} new_buffer= Cyc_Core_new_string(( int) new_len); Cyc_String_strncpy(
new_buffer, 0, b->buffer, 0, b->position); b->buffer= new_buffer; b->length=
new_len; return;} void Cyc_Buffer_add_char( struct Cyc_Buffer_t* b,
unsigned char c){ int pos=( int) b->position; if( pos >= b->length){ Cyc_Buffer_resize(
b, 1);}*(( unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( b->buffer, sizeof( unsigned char), pos)= c; b->position=(
unsigned int)( pos + 1); return;} void Cyc_Buffer_add_substring( struct Cyc_Buffer_t*
b, struct _tagged_string s, int offset, int len){ if(( offset < 0? 1: len < 0)?
1: offset + len >({ struct _tagged_string _temp2= s;( unsigned int)( _temp2.last_plus_one
- _temp2.curr);})){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp3=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp3[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp4; _temp4.tag= Cyc_Core_InvalidArg;
_temp4.f1=({ unsigned char* _temp5="Buffer::add_substring"; struct
_tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 22; _temp6;}); _temp4;}); _temp3;}));}{ int new_position=( int)( b->position
+ len); if( new_position > b->length){ Cyc_Buffer_resize( b,( unsigned int) len);}
Cyc_String_zstrncpy( b->buffer,( int) b->position, s, offset,( unsigned int) len);
b->position=( unsigned int) new_position; return;}} void Cyc_Buffer_add_string(
struct Cyc_Buffer_t* b, struct _tagged_string s){ int len=( int) Cyc_String_strlen(
s); int new_position=( int)( b->position + len); if( new_position > b->length){
Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_String_strncpy( b->buffer,( int)
b->position, s, 0,( unsigned int) len); b->position=( unsigned int) new_position;
return;} void Cyc_Buffer_add_buffer( struct Cyc_Buffer_t* b, struct Cyc_Buffer_t*
bs){ Cyc_Buffer_add_substring( b, bs->buffer, 0,( int) bs->position); return;}