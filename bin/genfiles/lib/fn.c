#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; typedef unsigned int Cyc_uint; typedef
char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; typedef void* Cyc_Fn_Function; extern const int Cyc_Fn_Fun_tag;
struct Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*, void*); void* f2; } ;
typedef void* Cyc_Fn_fn; extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*),
void* x); extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply(
void* f, void* x); extern void* Cyc_Fn_compose( void* g, void* f); extern void*
Cyc_Fn_curry( void* f); extern void* Cyc_Fn_uncurry( void* f); extern struct Cyc_List_List*
Cyc_Fn_map_fn( void* f, struct Cyc_List_List* x); const int Cyc_Fn_Fun_tag= 0;
void* Cyc_Fn_make_fn( void*(* f)( void*, void*), void* x){ return({ struct Cyc_Fn_Fun_struct*
_temp0=( struct Cyc_Fn_Fun_struct*) GC_malloc( sizeof( struct Cyc_Fn_Fun_struct));
_temp0->tag= Cyc_Fn_Fun_tag; _temp0->f1= f; _temp0->f2=( void*) x;( void*)
_temp0;});} static void* Cyc_Fn_fp_apply( void*(* f)( void*), void* x){ return f(
x);} void* Cyc_Fn_fp2fn( void*(* f)( void*)){ return(( void*(*)( void*(* f)(
void*(*)( void*), void*), void*(* x)( void*))) Cyc_Fn_make_fn)(( void*(*)( void*(*
f)( void*), void* x)) Cyc_Fn_fp_apply, f);} void* Cyc_Fn_apply( void* f, void* x){
void* _temp1= f; void* _temp5; void*(* _temp7)( void*, void*); _LL3: if(((
struct _enum_struct*) _temp1)->tag == Cyc_Fn_Fun_tag){ _LL8: _temp7=( void*(*)(
void*, void*))(( struct Cyc_Fn_Fun_struct*) _temp1)->f1; goto _LL6; _LL6: _temp5=(
void*)(( struct Cyc_Fn_Fun_struct*) _temp1)->f2; goto _LL4;} else{ goto _LL2;}
_LL4: return _temp7( _temp5, x); _LL2:;} static void* Cyc_Fn_fn_compose( struct
_tuple0* f_and_g, void* arg){ struct _tuple0 _temp11; void* _temp12; void*
_temp14; struct _tuple0* _temp9= f_and_g; _temp11=* _temp9; _LL15: _temp14=
_temp11.f1; goto _LL13; _LL13: _temp12= _temp11.f2; goto _LL10; _LL10: return((
void*(*)( void* f, void* x)) Cyc_Fn_apply)( _temp14,(( void*(*)( void* f, void*
x)) Cyc_Fn_apply)( _temp12, arg));} void* Cyc_Fn_compose( void* g, void* f){
return(( void*(*)( void*(* f)( struct _tuple0*, void*), struct _tuple0* x)) Cyc_Fn_make_fn)((
void*(*)( struct _tuple0* f_and_g, void* arg)) Cyc_Fn_fn_compose,({ struct
_tuple0* _temp16=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp16->f1=
f; _temp16->f2= g; _temp16;}));} static void* Cyc_Fn_inner( struct _tuple0* env,
void* second){ return(( void*(*)( void* f, struct _tuple0* x)) Cyc_Fn_apply)((*
env).f1,({ struct _tuple0* _temp17=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp17->f1=(* env).f2; _temp17->f2= second; _temp17;}));} static
void* Cyc_Fn_outer( void* f, void* first){ return(( void*(*)( void*(* f)( struct
_tuple0*, void*), struct _tuple0* x)) Cyc_Fn_make_fn)(( void*(*)( struct _tuple0*
env, void* second)) Cyc_Fn_inner,({ struct _tuple0* _temp18=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp18->f1= f; _temp18->f2= first; _temp18;}));}
void* Cyc_Fn_curry( void* f){ return(( void*(*)( void*(* f)( void*, void*), void*
x)) Cyc_Fn_make_fn)(( void*(*)( void* f, void* first)) Cyc_Fn_outer, f);} static
void* Cyc_Fn_lambda( void* f, struct _tuple0* arg){ return(( void*(*)( void* f,
void* x)) Cyc_Fn_apply)((( void*(*)( void* f, void* x)) Cyc_Fn_apply)( f,(* arg).f1),(*
arg).f2);} void* Cyc_Fn_uncurry( void* f){ return(( void*(*)( void*(* f)( void*,
struct _tuple0*), void* x)) Cyc_Fn_make_fn)(( void*(*)( void* f, struct _tuple0*
arg)) Cyc_Fn_lambda, f);} struct Cyc_List_List* Cyc_Fn_map_fn( void* f, struct
Cyc_List_List* x){ return(( struct Cyc_List_List*(*)( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x)) Cyc_List_map_c)(( void*(*)( void* f, void*
x)) Cyc_Fn_apply, f, x);}