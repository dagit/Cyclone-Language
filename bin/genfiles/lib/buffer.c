#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_Buffer_t;
typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n); extern struct _tagged_string Cyc_Buffer_contents( struct Cyc_Buffer_t*);
extern unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t*); extern void Cyc_Buffer_clear(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_reset( struct Cyc_Buffer_t*);
extern void Cyc_Buffer_add_char( struct Cyc_Buffer_t*, char); extern void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*, struct _tagged_string, int offset, int len); extern void
Cyc_Buffer_add_string( struct Cyc_Buffer_t*, struct _tagged_string); extern void
Cyc_Buffer_add_buffer( struct Cyc_Buffer_t* buf_dest, struct Cyc_Buffer_t*
buf_source); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef
struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern struct _tagged_string Cyc_String_strncpy(
struct _tagged_string, int, struct _tagged_string, int, unsigned int); extern
struct _tagged_string Cyc_String_zstrncpy( struct _tagged_string, int, struct
_tagged_string, int, unsigned int); extern struct _tagged_string Cyc_String_substring(
struct _tagged_string, int ofs, unsigned int n); struct Cyc_Buffer_t{ struct
_tagged_string buffer; unsigned int position; unsigned int length; struct
_tagged_string initial_buffer;}; struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n){ if( n <( unsigned int) 1){ n= 1;}{ struct _tagged_string s= Cyc_Core_new_string((
int) n); return({ struct Cyc_Buffer_t* _temp0=( struct Cyc_Buffer_t*) GC_malloc(
sizeof( struct Cyc_Buffer_t)); _temp0->buffer= s; _temp0->position=(
unsigned int) 0; _temp0->length= n; _temp0->initial_buffer= s; _temp0;});}}
struct _tagged_string Cyc_Buffer_contents( struct Cyc_Buffer_t* b){ return Cyc_String_substring(
b->buffer, 0, b->position);} unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t*
b){ return b->position;} void Cyc_Buffer_clear( struct Cyc_Buffer_t* b){ b->position=
0; return;} void Cyc_Buffer_reset( struct Cyc_Buffer_t* b){ b->position= 0; b->buffer=
b->initial_buffer; b->length=({ struct _tagged_string _temp1= b->buffer;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); return;} static void Cyc_Buffer_resize(
struct Cyc_Buffer_t* b, unsigned int more){ unsigned int len= b->length;
unsigned int new_len= len; struct _tagged_string new_buffer; while( b->position
+ more > new_len) { new_len=( unsigned int) 2 * new_len;} new_buffer= Cyc_Core_new_string((
int) new_len); Cyc_String_strncpy( new_buffer, 0, b->buffer, 0, b->position); b->buffer=
new_buffer; b->length= new_len; return;} void Cyc_Buffer_add_char( struct Cyc_Buffer_t*
b, char c){ int pos=( int) b->position; if(( unsigned int) pos >= b->length){
Cyc_Buffer_resize( b,( unsigned int) 1);}({ struct _tagged_string _temp2= b->buffer;
char* _temp4= _temp2.curr + pos; if( _temp4 < _temp2.base? 1: _temp4 >= _temp2.last_plus_one){
_throw( Null_Exception);}* _temp4= c;}); b->position=( unsigned int)( pos + 1);
return;} void Cyc_Buffer_add_substring( struct Cyc_Buffer_t* b, struct
_tagged_string s, int offset, int len){ if(( offset < 0? 1: len < 0)? 1:(
unsigned int)( offset + len) >({ struct _tagged_string _temp5= s;( unsigned int)(
_temp5.last_plus_one - _temp5.curr);})){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp6=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp6=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp7=( char*)"Buffer::add_substring"; struct
_tagged_string _temp8; _temp8.curr= _temp7; _temp8.base= _temp7; _temp8.last_plus_one=
_temp7 + 22; _temp8;})};( struct _xenum_struct*) _temp6;}));}{ int new_position=(
int)( b->position +( unsigned int) len); if(( unsigned int) new_position > b->length){
Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_String_zstrncpy( b->buffer,( int)
b->position, s, offset,( unsigned int) len); b->position=( unsigned int)
new_position; return;}} void Cyc_Buffer_add_string( struct Cyc_Buffer_t* b,
struct _tagged_string s){ int len=( int) Cyc_String_strlen( s); int new_position=(
int)( b->position +( unsigned int) len); if(( unsigned int) new_position > b->length){
Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_String_strncpy( b->buffer,( int)
b->position, s, 0,( unsigned int) len); b->position=( unsigned int) new_position;
return;} void Cyc_Buffer_add_buffer( struct Cyc_Buffer_t* b, struct Cyc_Buffer_t*
bs){ Cyc_Buffer_add_substring( b, bs->buffer, 0,( int) bs->position); return;}