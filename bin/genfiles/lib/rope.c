#include "cyc_include.h"

 struct _tagged_ptr0{ struct Cyc_Rope_Rope_node** curr; struct Cyc_Rope_Rope_node**
base; struct Cyc_Rope_Rope_node** last_plus_one;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct
_tagged_string Cyc_Core_new_string( int); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char*
tag;}; struct Cyc_Rope_Rope_node; typedef struct Cyc_Rope_Rope_node* Cyc_Rope_rope_t;
extern struct Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct _tagged_string);
extern struct _tagged_string Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*);
extern struct Cyc_Rope_Rope_node* Cyc_Rope_concat( struct Cyc_Rope_Rope_node*,
struct Cyc_Rope_Rope_node*); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concata(
struct _tagged_ptr0); extern struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct
Cyc_List_List*); extern unsigned int Cyc_Rope_length( struct Cyc_Rope_Rope_node*);
extern int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*, struct Cyc_Rope_Rope_node*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern struct _tagged_string Cyc_String_strncpy(
struct _tagged_string, int, struct _tagged_string, int, unsigned int); typedef
void* Cyc_Rope_R; static const int Cyc_Rope_String_rope_tag= 0; struct Cyc_Rope_String_rope_struct{
int tag; struct _tagged_string f1;}; static const int Cyc_Rope_Array_rope_tag= 1;
struct Cyc_Rope_Array_rope_struct{ int tag; struct _tagged_ptr0 f1;}; struct Cyc_Rope_Rope_node{
void* v;}; struct Cyc_Rope_Rope_node* Cyc_Rope_from_string( struct
_tagged_string s){ return({ struct Cyc_Rope_Rope_node* _temp0=( struct Cyc_Rope_Rope_node*)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node)); _temp0->v=( void*)({ struct Cyc_Rope_String_rope_struct*
_temp1=( struct Cyc_Rope_String_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct));
_temp1->tag= Cyc_Rope_String_rope_tag; _temp1->f1= s;( void*) _temp1;}); _temp0;});}
struct Cyc_Rope_Rope_node* Cyc_Rope_concat( struct Cyc_Rope_Rope_node* r1,
struct Cyc_Rope_Rope_node* r2){ return({ struct Cyc_Rope_Rope_node* _temp2=(
struct Cyc_Rope_Rope_node*) GC_malloc( sizeof( struct Cyc_Rope_Rope_node));
_temp2->v=( void*)({ struct Cyc_Rope_Array_rope_struct* _temp3=( struct Cyc_Rope_Array_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct)); _temp3->tag= Cyc_Rope_Array_rope_tag;
_temp3->f1=( struct _tagged_ptr0)({ struct Cyc_Rope_Rope_node** _temp5=( struct
Cyc_Rope_Rope_node**)({ struct Cyc_Rope_Rope_node** _temp4=( struct Cyc_Rope_Rope_node**)
GC_malloc( sizeof( struct Cyc_Rope_Rope_node*) * 2); _temp4[ 0]= r1; _temp4[ 1]=
r2; _temp4;}); struct _tagged_ptr0 _temp6; _temp6.curr= _temp5; _temp6.base=
_temp5; _temp6.last_plus_one= _temp5 + 2; _temp6;});( void*) _temp3;}); _temp2;});}
struct Cyc_Rope_Rope_node* Cyc_Rope_concata( struct _tagged_ptr0 rs){ return({
struct Cyc_Rope_Rope_node* _temp7=( struct Cyc_Rope_Rope_node*) GC_malloc(
sizeof( struct Cyc_Rope_Rope_node)); _temp7->v=( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp8=( struct Cyc_Rope_Array_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp8->tag= Cyc_Rope_Array_rope_tag; _temp8->f1= rs;( void*) _temp8;}); _temp7;});}
struct Cyc_Rope_Rope_node* Cyc_Rope_concatl( struct Cyc_List_List* l){ return({
struct Cyc_Rope_Rope_node* _temp9=( struct Cyc_Rope_Rope_node*) GC_malloc(
sizeof( struct Cyc_Rope_Rope_node)); _temp9->v=( void*)({ struct Cyc_Rope_Array_rope_struct*
_temp10=( struct Cyc_Rope_Array_rope_struct*) GC_malloc( sizeof( struct Cyc_Rope_Array_rope_struct));
_temp10->tag= Cyc_Rope_Array_rope_tag; _temp10->f1=({ unsigned int _temp11=(
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( l); struct
Cyc_Rope_Rope_node** _temp12=( struct Cyc_Rope_Rope_node**) GC_malloc( sizeof(
struct Cyc_Rope_Rope_node*) * _temp11); unsigned int i; struct _tagged_ptr0
_temp13={ _temp12, _temp12, _temp12 + _temp11}; for( i= 0; i < _temp11; i ++){
_temp12[ i]=({ struct Cyc_Rope_Rope_node* r=( struct Cyc_Rope_Rope_node*) l->hd;
l= l->tl; r;});} _temp13;});( void*) _temp10;}); _temp9;});} unsigned int Cyc_Rope_length(
struct Cyc_Rope_Rope_node* r){ void* _temp14=( void*) r->v; struct
_tagged_string _temp20; struct _tagged_ptr0 _temp22; _LL16: if((( struct
_enum_struct*) _temp14)->tag == Cyc_Rope_String_rope_tag){ _LL21: _temp20=(
struct _tagged_string)(( struct Cyc_Rope_String_rope_struct*) _temp14)->f1; goto
_LL17;} else{ goto _LL18;} _LL18: if((( struct _enum_struct*) _temp14)->tag ==
Cyc_Rope_Array_rope_tag){ _LL23: _temp22=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp14)->f1; goto _LL19;} else{ goto _LL15;} _LL17: return Cyc_String_strlen(
_temp20); _LL19: { unsigned int total=( unsigned int) 0; unsigned int sz=({
struct _tagged_ptr0 _temp24= _temp22;( unsigned int)( _temp24.last_plus_one -
_temp24.curr);});{ unsigned int i=( unsigned int) 0; for( 0; i < sz; i ++){
total += Cyc_Rope_length(({ struct _tagged_ptr0 _temp25= _temp22; struct Cyc_Rope_Rope_node**
_temp27= _temp25.curr +( int) i; if( _temp27 < _temp25.base? 1: _temp27 >=
_temp25.last_plus_one){ _throw( Null_Exception);}* _temp27;}));}} return total;}
_LL15:;} static unsigned int Cyc_Rope_flatten_it( struct _tagged_string s,
unsigned int i, struct Cyc_Rope_Rope_node* r){ void* _temp28=( void*) r->v;
struct _tagged_string _temp34; struct _tagged_ptr0 _temp36; _LL30: if((( struct
_enum_struct*) _temp28)->tag == Cyc_Rope_String_rope_tag){ _LL35: _temp34=(
struct _tagged_string)(( struct Cyc_Rope_String_rope_struct*) _temp28)->f1; goto
_LL31;} else{ goto _LL32;} _LL32: if((( struct _enum_struct*) _temp28)->tag ==
Cyc_Rope_Array_rope_tag){ _LL37: _temp36=( struct _tagged_ptr0)(( struct Cyc_Rope_Array_rope_struct*)
_temp28)->f1; goto _LL33;} else{ goto _LL29;} _LL31: { unsigned int len= Cyc_String_strlen(
_temp34); Cyc_String_strncpy( s,( int) i, _temp34, 0, len); return i + len;}
_LL33: { unsigned int len=({ struct _tagged_ptr0 _temp38= _temp36;( unsigned int)(
_temp38.last_plus_one - _temp38.curr);});{ int j= 0; for( 0;( unsigned int) j <
len; j ++){ i= Cyc_Rope_flatten_it( s, i,({ struct _tagged_ptr0 _temp39= _temp36;
struct Cyc_Rope_Rope_node** _temp41= _temp39.curr + j; if( _temp41 < _temp39.base?
1: _temp41 >= _temp39.last_plus_one){ _throw( Null_Exception);}* _temp41;}));}}
return i;} _LL29:;} struct _tagged_string Cyc_Rope_to_string( struct Cyc_Rope_Rope_node*
r){ struct _tagged_string s= Cyc_Core_new_string(( int) Cyc_Rope_length( r));
Cyc_Rope_flatten_it( s,( unsigned int) 0, r);( void*)( r->v=( void*)({ struct
Cyc_Rope_String_rope_struct* _temp42=( struct Cyc_Rope_String_rope_struct*)
GC_malloc( sizeof( struct Cyc_Rope_String_rope_struct)); _temp42->tag= Cyc_Rope_String_rope_tag;
_temp42->f1= s;( void*) _temp42;})); return s;} int Cyc_Rope_cmp( struct Cyc_Rope_Rope_node*
r1, struct Cyc_Rope_Rope_node* r2){ return Cyc_String_strcmp( Cyc_Rope_to_string(
r1), Cyc_Rope_to_string( r2));}