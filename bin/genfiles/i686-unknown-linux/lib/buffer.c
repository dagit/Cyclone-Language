 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); struct Cyc_Buffer_t;
extern struct Cyc_Buffer_t* Cyc_Buffer_create( unsigned int n); extern struct
_tagged_arr Cyc_Buffer_contents( struct Cyc_Buffer_t*); extern unsigned int Cyc_Buffer_length(
struct Cyc_Buffer_t*); extern void Cyc_Buffer_clear( struct Cyc_Buffer_t*);
extern void Cyc_Buffer_reset( struct Cyc_Buffer_t*); extern void Cyc_Buffer_add_char(
struct Cyc_Buffer_t*, unsigned char); extern void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*, struct _tagged_arr, int offset, int len); extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*, struct _tagged_arr); extern void Cyc_Buffer_add_buffer(
struct Cyc_Buffer_t* buf_dest, struct Cyc_Buffer_t* buf_source); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern unsigned int Cyc_std_strlen( struct _tagged_arr s);
extern struct _tagged_arr Cyc_std_strncpy( struct _tagged_arr, struct
_tagged_arr, unsigned int); extern struct _tagged_arr Cyc_std_zstrncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_std_substring(
struct _tagged_arr, int ofs, unsigned int n); struct Cyc_Buffer_t{ struct
_tagged_arr buffer; unsigned int position; unsigned int length; struct
_tagged_arr initial_buffer; } ; struct Cyc_Buffer_t* Cyc_Buffer_create(
unsigned int n){ if( n <  1){ n= 1;}{ struct _tagged_arr s= Cyc_Core_new_string((
int) n); return({ struct Cyc_Buffer_t* _temp0=( struct Cyc_Buffer_t*) GC_malloc(
sizeof( struct Cyc_Buffer_t)); _temp0->buffer= s; _temp0->position= 0; _temp0->length=
n; _temp0->initial_buffer= s; _temp0;});}} struct _tagged_arr Cyc_Buffer_contents(
struct Cyc_Buffer_t* b){ return Cyc_std_substring(( struct _tagged_arr) b->buffer,
0, b->position);} unsigned int Cyc_Buffer_length( struct Cyc_Buffer_t* b){
return( unsigned int) b->position;} void Cyc_Buffer_clear( struct Cyc_Buffer_t*
b){ b->position= 0; return;} void Cyc_Buffer_reset( struct Cyc_Buffer_t* b){ b->position=
0; b->buffer= b->initial_buffer; b->length= _get_arr_size( b->buffer, sizeof(
unsigned char)); return;} static void Cyc_Buffer_resize( struct Cyc_Buffer_t* b,
unsigned int more){ unsigned int len= b->length; unsigned int new_len= len;
struct _tagged_arr new_buffer; while( b->position +  more >  new_len) { new_len=
2 *  new_len;} new_buffer= Cyc_Core_new_string(( int) new_len); Cyc_std_strncpy(
new_buffer,( struct _tagged_arr) b->buffer, b->position); b->buffer= new_buffer;
b->length= new_len; return;} void Cyc_Buffer_add_char( struct Cyc_Buffer_t* b,
unsigned char c){ int pos=( int) b->position; if( pos >=  b->length){ Cyc_Buffer_resize(
b, 1);}*(( unsigned char*) _check_unknown_subscript( b->buffer, sizeof(
unsigned char), pos))= c; b->position=( unsigned int)( pos +  1); return;} void
Cyc_Buffer_add_substring( struct Cyc_Buffer_t* b, struct _tagged_arr s, int
offset, int len){ if(( offset <  0? 1: len <  0)? 1: offset +  len > 
_get_arr_size( s, sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp1=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp1[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp2; _temp2.tag= Cyc_Core_InvalidArg;
_temp2.f1= _tag_arr("Buffer::add_substring", sizeof( unsigned char), 22u);
_temp2;}); _temp1;}));}{ int new_position=( int)( b->position +  len); if(
new_position >  b->length){ Cyc_Buffer_resize( b,( unsigned int) len);} Cyc_std_zstrncpy(
_tagged_arr_plus( b->buffer, sizeof( unsigned char),( int) b->position),
_tagged_arr_plus( s, sizeof( unsigned char), offset),( unsigned int) len); b->position=(
unsigned int) new_position; return;}} void Cyc_Buffer_add_string( struct Cyc_Buffer_t*
b, struct _tagged_arr s){ int len=( int) Cyc_std_strlen( s); int new_position=(
int)( b->position +  len); if( new_position >  b->length){ Cyc_Buffer_resize( b,(
unsigned int) len);} Cyc_std_strncpy( _tagged_arr_plus( b->buffer, sizeof(
unsigned char),( int) b->position), s,( unsigned int) len); b->position=(
unsigned int) new_position; return;} void Cyc_Buffer_add_buffer( struct Cyc_Buffer_t*
b, struct Cyc_Buffer_t* bs){ Cyc_Buffer_add_substring( b,( struct _tagged_arr)
bs->buffer, 0,( int) bs->position); return;}
