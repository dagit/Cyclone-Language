 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
struct _tagged_arr Cyc_Core_new_string( int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
int Cyc_List_length( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_empty[
15u]; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; struct Cyc_Rope_Rope_node; extern struct Cyc_Rope_Rope_node*
Cyc_Rope_from_string( struct _tagged_arr); extern struct _tagged_arr Cyc_Rope_to_string(
struct Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node*
Cyc_Rope_concata( struct _tagged_arr); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concatl(
struct Cyc_List_List*); extern unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*);
extern int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*);
struct Cyc_Stdio___sFILE; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern struct _tagged_arr Cyc_String_strncpy( struct
_tagged_arr, int, struct _tagged_arr, int, int); static const int Cyc_Rope_String_rope=
0; struct Cyc_Rope_String_rope_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Rope_Array_rope= 1; struct Cyc_Rope_Array_rope_struct{ int
tag; struct _tagged_arr f1; } ; struct Cyc_Rope_Rope_node{ void* v; } ; struct
Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct _tagged_arr s){ return({ struct
Cyc_Rope_Rope_node* _temp0=( struct Cyc_Rope_Rope_node*) GC_malloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp0->v=( void*)(( void*)({ struct Cyc_Rope_String_rope_struct*
_temp1=( struct Cyc_Rope_String_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct));
_temp1[ 0]=({ struct Cyc_Rope_String_rope_struct _temp2; _temp2.tag= Cyc_Rope_String_rope;
_temp2.f1= s; _temp2;}); _temp1;})); _temp0;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concat(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return({ struct
Cyc_Rope_Rope_node* _temp3=( struct Cyc_Rope_Rope_node*) GC_malloc( sizeof(
struct Cyc_Rope_Rope_node)); _temp3->v=( void*)(( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp4=( struct Cyc_Rope_Array_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp4[ 0]=({ struct Cyc_Rope_Array_rope_struct _temp5; _temp5.tag= Cyc_Rope_Array_rope;
_temp5.f1= _tag_arr(({ struct Cyc_Rope_Rope_node** _temp6=( struct Cyc_Rope_Rope_node**)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node*) * 2); _temp6[ 0]= r1; _temp6[ 1]=
r2; _temp6;}), sizeof( struct Cyc_Rope_Rope_node*), 2u); _temp5;}); _temp4;}));
_temp3;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct _tagged_arr rs){
return({ struct Cyc_Rope_Rope_node* _temp7=( struct Cyc_Rope_Rope_node*)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp7->v=( void*)(( void*)({
struct Cyc_Rope_Array_rope_struct* _temp8=( struct Cyc_Rope_Array_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp8[ 0]=({ struct Cyc_Rope_Array_rope_struct
_temp9; _temp9.tag= Cyc_Rope_Array_rope; _temp9.f1= rs; _temp9;}); _temp8;}));
_temp7;});} struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct Cyc_List_List* l){
return({ struct Cyc_Rope_Rope_node* _temp10=( struct Cyc_Rope_Rope_node*)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp10->v=( void*)(( void*)({
struct Cyc_Rope_Array_rope_struct* _temp11=( struct Cyc_Rope_Array_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp11[ 0]=({ struct
Cyc_Rope_Array_rope_struct _temp12; _temp12.tag= Cyc_Rope_Array_rope; _temp12.f1=({
unsigned int _temp13=( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
l); struct Cyc_Rope_Rope_node** _temp14=( struct Cyc_Rope_Rope_node**) GC_malloc(
sizeof( struct Cyc_Rope_Rope_node*) * _temp13); struct _tagged_arr _temp16=
_tag_arr( _temp14, sizeof( struct Cyc_Rope_Rope_node*),( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( l));{ unsigned int _temp15= _temp13;
unsigned int i; for( i= 0; i < _temp15; i ++){ _temp14[ i]=({ struct Cyc_Rope_Rope_node*
r=( struct Cyc_Rope_Rope_node*)(( struct Cyc_List_List*) _check_null( l))->hd; l=((
struct Cyc_List_List*) _check_null( l))->tl; r;});}}; _temp16;}); _temp12;});
_temp11;})); _temp10;});} unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*
r){ void* _temp17=( void*) r->v; struct _tagged_arr _temp23; struct _tagged_arr
_temp25; _LL19: if(*(( int*) _temp17) == Cyc_Rope_String_rope){ _LL24: _temp23=((
struct Cyc_Rope_String_rope_struct*) _temp17)->f1; goto _LL20;} else{ goto _LL21;}
_LL21: if(*(( int*) _temp17) == Cyc_Rope_Array_rope){ _LL26: _temp25=(( struct
Cyc_Rope_Array_rope_struct*) _temp17)->f1; goto _LL22;} else{ goto _LL18;} _LL20:
return( unsigned int) Cyc_String_strlen( _temp23); _LL22: { unsigned int total=
0; unsigned int sz= _get_arr_size( _temp25, sizeof( struct Cyc_Rope_Rope_node*));{
unsigned int i= 0; for( 0; i < sz; i ++){ total += Cyc_Rope_length(*(( struct
Cyc_Rope_Rope_node**) _check_unknown_subscript( _temp25, sizeof( struct Cyc_Rope_Rope_node*),(
int) i)));}} return total;} _LL18:;} static unsigned int Cyc_Rope_flatten_it(
struct _tagged_arr s, unsigned int i, struct Cyc_Rope_Rope_node* r){ void*
_temp27=( void*) r->v; struct _tagged_arr _temp33; struct _tagged_arr _temp35;
_LL29: if(*(( int*) _temp27) == Cyc_Rope_String_rope){ _LL34: _temp33=(( struct
Cyc_Rope_String_rope_struct*) _temp27)->f1; goto _LL30;} else{ goto _LL31;}
_LL31: if(*(( int*) _temp27) == Cyc_Rope_Array_rope){ _LL36: _temp35=(( struct
Cyc_Rope_Array_rope_struct*) _temp27)->f1; goto _LL32;} else{ goto _LL28;} _LL30: {
int _temp37= Cyc_String_strlen( _temp33); goto _LL38; _LL38: Cyc_String_strncpy(
s,( int) i, _temp33, 0, _temp37); return i + _temp37;} _LL32: { unsigned int
_temp39= _get_arr_size( _temp35, sizeof( struct Cyc_Rope_Rope_node*)); goto
_LL40; _LL40:{ int j= 0; for( 0; j < _temp39; j ++){ i= Cyc_Rope_flatten_it( s,
i,*(( struct Cyc_Rope_Rope_node**) _check_unknown_subscript( _temp35, sizeof(
struct Cyc_Rope_Rope_node*), j)));}} return i;} _LL28:;} struct _tagged_arr Cyc_Rope_to_string(
struct Cyc_Rope_Rope_node* r){ struct _tagged_arr s= Cyc_Core_new_string(( int)
Cyc_Rope_length( r)); Cyc_Rope_flatten_it( s, 0, r);( void*)( r->v=( void*)((
void*)({ struct Cyc_Rope_String_rope_struct* _temp41=( struct Cyc_Rope_String_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp41[ 0]=({ struct
Cyc_Rope_String_rope_struct _temp42; _temp42.tag= Cyc_Rope_String_rope; _temp42.f1=(
struct _tagged_arr) s; _temp42;}); _temp41;}))); return s;} int Cyc_Rope_cmp(
struct Cyc_Rope_Rope_node* r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_String_strcmp((
struct _tagged_arr) Cyc_Rope_to_string( r1),( struct _tagged_arr) Cyc_Rope_to_string(
r2));}