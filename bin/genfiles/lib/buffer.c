 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct _tagged_string Cyc_Core_new_string( int); extern
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
Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Buffer_t; extern
struct Cyc_Buffer_t* Cyc_Buffer_create( unsigned int n); extern struct
_tagged_string Cyc_Buffer_contents( struct Cyc_Buffer_t*); extern unsigned int
Cyc_Buffer_length( struct Cyc_Buffer_t*); extern void Cyc_Buffer_clear( struct
Cyc_Buffer_t*); extern void Cyc_Buffer_reset( struct Cyc_Buffer_t*); extern void
Cyc_Buffer_add_char( struct Cyc_Buffer_t*, unsigned char); extern void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*, struct _tagged_string, int offset, int len); extern void
Cyc_Buffer_add_string( struct Cyc_Buffer_t*, struct _tagged_string); extern void
Cyc_Buffer_add_buffer( struct Cyc_Buffer_t* buf_dest, struct Cyc_Buffer_t*
buf_source); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
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
struct Cyc_Buffer_t* b){ return Cyc_String_substring(( struct _tagged_string) b->buffer,
0, b->position);} unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t* b){
return b->position;} void Cyc_Buffer_clear( struct Cyc_Buffer_t* b){ b->position=
0; return;} void Cyc_Buffer_reset( struct Cyc_Buffer_t* b){ b->position= 0; b->buffer=
b->initial_buffer; b->length=({ struct _tagged_string _temp1= b->buffer;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); return;} static void Cyc_Buffer_resize(
struct Cyc_Buffer_t* b, unsigned int more){ unsigned int len= b->length;
unsigned int new_len= len; struct _tagged_string new_buffer; while( b->position
+ more > new_len) { new_len= 2 * new_len;} new_buffer= Cyc_Core_new_string(( int)
new_len); Cyc_String_strncpy( new_buffer, 0,( struct _tagged_string) b->buffer,
0, b->position); b->buffer= new_buffer; b->length= new_len; return;} void Cyc_Buffer_add_char(
struct Cyc_Buffer_t* b, unsigned char c){ int pos=( int) b->position; if( pos >=
b->length){ Cyc_Buffer_resize( b, 1);}*(( unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( b->buffer,
sizeof( unsigned char), pos)= c; b->position=( unsigned int)( pos + 1); return;}
void Cyc_Buffer_add_substring( struct Cyc_Buffer_t* b, struct _tagged_string s,
int offset, int len){ if(( offset < 0? 1: len < 0)? 1: offset + len >({ struct
_tagged_string _temp2= s;( unsigned int)( _temp2.last_plus_one - _temp2.curr);})){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp3=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp3[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp4; _temp4.tag= Cyc_Core_InvalidArg; _temp4.f1=({ unsigned char* _temp5=(
unsigned char*)"Buffer::add_substring"; struct _tagged_string _temp6; _temp6.curr=
_temp5; _temp6.base= _temp5; _temp6.last_plus_one= _temp5 + 22; _temp6;});
_temp4;}); _temp3;}));}{ int new_position=( int)( b->position + len); if(
new_position > b->length){ Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_String_zstrncpy(
b->buffer,( int) b->position, s, offset,( unsigned int) len); b->position=(
unsigned int) new_position; return;}} void Cyc_Buffer_add_string( struct Cyc_Buffer_t*
b, struct _tagged_string s){ int len=( int) Cyc_String_strlen( s); int
new_position=( int)( b->position + len); if( new_position > b->length){ Cyc_Buffer_resize(
b,( unsigned int) len);} Cyc_String_strncpy( b->buffer,( int) b->position, s, 0,(
unsigned int) len); b->position=( unsigned int) new_position; return;} void Cyc_Buffer_add_buffer(
struct Cyc_Buffer_t* b, struct Cyc_Buffer_t* bs){ Cyc_Buffer_add_substring( b,(
struct _tagged_string) bs->buffer, 0,( int) bs->position); return;}