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
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
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
Cyc_List_List* Cyc_List_List_t; extern unsigned char Cyc_List_List_empty[ 15u];
extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern unsigned char Cyc_List_Nth[ 8u]; static const int Cyc_Fn_Fun= 0; struct
Cyc_Fn_Fun_struct{ int tag; void*(* f1)( void*, void*); void* f2; } ; typedef
void* Cyc_Fn_fn; extern void* Cyc_Fn_make_fn( void*(* f)( void*, void*), void* x);
extern void* Cyc_Fn_fp2fn( void*(* f)( void*)); extern void* Cyc_Fn_apply( void*
f, void* x); extern void* Cyc_Fn_compose( void* g, void* f); extern void* Cyc_Fn_curry(
void* f); extern void* Cyc_Fn_uncurry( void* f); extern struct Cyc_List_List*
Cyc_Fn_map_fn( void* f, struct Cyc_List_List* x); void* Cyc_Fn_make_fn( void*(*
f)( void*, void*), void* x){ return( void*)({ struct Cyc_Fn_Fun_struct* _temp0=(
struct Cyc_Fn_Fun_struct*) GC_malloc( sizeof( struct Cyc_Fn_Fun_struct)); _temp0[
0]=({ struct Cyc_Fn_Fun_struct _temp1; _temp1.tag= Cyc_Fn_Fun; _temp1.f1= f;
_temp1.f2=( void*) x; _temp1;}); _temp0;});} static void* Cyc_Fn_fp_apply( void*(*
f)( void*), void* x){ return f( x);} void* Cyc_Fn_fp2fn( void*(* f)( void*)){
return(( void*(*)( void*(* f)( void*(*)( void*), void*), void*(* x)( void*)))
Cyc_Fn_make_fn)(( void*(*)( void*(* f)( void*), void* x)) Cyc_Fn_fp_apply, f);}
void* Cyc_Fn_apply( void* f, void* x){ void* _temp4; void*(* _temp6)( void*,
void*); void* _temp2= f; if(*(( int*) _temp2) == Cyc_Fn_Fun){ _LL7: _temp6=((
struct Cyc_Fn_Fun_struct*) _temp2)->f1; goto _LL5; _LL5: _temp4=( void*)((
struct Cyc_Fn_Fun_struct*) _temp2)->f2; goto _LL3;} else{ goto _LL3;} _LL3:
return _temp6( _temp4, x);} struct _tuple0{ void* f1; void* f2; } ; static void*
Cyc_Fn_fn_compose( struct _tuple0* f_and_g, void* arg){ struct _tuple0 _temp10;
void* _temp11; void* _temp13; struct _tuple0* _temp8= f_and_g; _temp10=* _temp8;
_LL14: _temp13= _temp10.f1; goto _LL12; _LL12: _temp11= _temp10.f2; goto _LL9;
_LL9: return(( void*(*)( void* f, void* x)) Cyc_Fn_apply)( _temp13,(( void*(*)(
void* f, void* x)) Cyc_Fn_apply)( _temp11, arg));} void* Cyc_Fn_compose( void* g,
void* f){ return(( void*(*)( void*(* f)( struct _tuple0*, void*), struct _tuple0*
x)) Cyc_Fn_make_fn)(( void*(*)( struct _tuple0* f_and_g, void* arg)) Cyc_Fn_fn_compose,({
struct _tuple0* _temp15=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp15->f1= f; _temp15->f2= g; _temp15;}));} static void* Cyc_Fn_inner( struct
_tuple0* env, void* second){ return(( void*(*)( void* f, struct _tuple0* x)) Cyc_Fn_apply)((*
env).f1,({ struct _tuple0* _temp16=( struct _tuple0*) GC_malloc( sizeof( struct
_tuple0)); _temp16->f1=(* env).f2; _temp16->f2= second; _temp16;}));} static
void* Cyc_Fn_outer( void* f, void* first){ return(( void*(*)( void*(* f)( struct
_tuple0*, void*), struct _tuple0* x)) Cyc_Fn_make_fn)(( void*(*)( struct _tuple0*
env, void* second)) Cyc_Fn_inner,({ struct _tuple0* _temp17=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp17->f1= f; _temp17->f2= first; _temp17;}));}
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