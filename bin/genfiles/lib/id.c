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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct
Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u];
extern int Cyc_String_zstrcmp( struct _tagged_string, struct _tagged_string);
extern struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*);
extern struct _tagged_string Cyc_String_strdup( struct _tagged_string src);
struct Cyc_Id_id; typedef struct Cyc_Id_id* Cyc_Id_Id; extern struct Cyc_Id_id*
Cyc_Id_id_make( struct _tagged_string, int); extern struct Cyc_Id_id* Cyc_Id_id_new(
struct _tagged_string); extern struct Cyc_Id_id* Cyc_Id_id_renew( struct Cyc_Id_id*);
extern struct Cyc_Id_id* Cyc_Id_id_of_string( struct _tagged_string); extern
struct Cyc_Id_id* Cyc_Id_id_unique( struct _tagged_string); extern struct
_tagged_string Cyc_Id_id_to_string( struct Cyc_Id_id*); extern void Cyc_Id_id_prn(
struct Cyc_Stdio___sFILE*, struct Cyc_Id_id*); extern struct _tagged_string Cyc_Id_id_to_source(
struct Cyc_Id_id*); extern int Cyc_Id_id_compare( struct Cyc_Id_id*, struct Cyc_Id_id*);
struct Cyc_Id_id{ struct _tagged_string s; int i; } ; struct Cyc_Id_id* Cyc_Id_id_make(
struct _tagged_string s, int i){ return({ struct Cyc_Id_id* _temp0=( struct Cyc_Id_id*)
GC_malloc( sizeof( struct Cyc_Id_id)); _temp0->s= s; _temp0->i= i; _temp0;});}
static int Cyc_Id_counter= 0; struct Cyc_Id_id* Cyc_Id_id_new( struct
_tagged_string s){ ++ Cyc_Id_counter; return Cyc_Id_id_make( s, Cyc_Id_counter);}
struct Cyc_Id_id* Cyc_Id_id_renew( struct Cyc_Id_id* i){ ++ Cyc_Id_counter;
return Cyc_Id_id_make( i->s, Cyc_Id_counter);} struct Cyc_Id_id* Cyc_Id_id_of_string(
struct _tagged_string s){ return Cyc_Id_id_make( s, - 1);} struct Cyc_Id_id* Cyc_Id_id_unique(
struct _tagged_string s){ return Cyc_Id_id_new( s);} struct _tagged_string Cyc_Id_id_to_string(
struct Cyc_Id_id* x){ if( x->i == - 1){ return Cyc_String_strdup( x->s);} return
Cyc_String_strconcat_l(({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_string* _temp8=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp8[ 0]= x->s; _temp8;}); _temp1->tl=({ struct Cyc_List_List*
_temp2=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2->hd=( void*)({ struct _tagged_string* _temp5=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp5[ 0]=( struct _tagged_string)({
char* _temp6=( char*)"$"; struct _tagged_string _temp7; _temp7.curr= _temp6;
_temp7.base= _temp6; _temp7.last_plus_one= _temp6 + 2; _temp7;}); _temp5;});
_temp2->tl=({ struct Cyc_List_List* _temp3=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp3->hd=( void*)({ struct _tagged_string*
_temp4=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp4[ 0]= Cyc_Core_string_of_int( x->i); _temp4;}); _temp3->tl= 0; _temp3;});
_temp2;}); _temp1;}));} void Cyc_Id_id_prn( struct Cyc_Stdio___sFILE* f, struct
Cyc_Id_id* x){({ struct _tagged_string _temp9= Cyc_Id_id_to_string( x); fprintf(
f,"%.*s", _temp9.last_plus_one - _temp9.curr, _temp9.curr);});} struct
_tagged_string Cyc_Id_id_to_source( struct Cyc_Id_id* x){ return x->s;} int Cyc_Id_id_compare(
struct Cyc_Id_id* x1, struct Cyc_Id_id* x2){ if( x1->i == x2->i){ return Cyc_String_zstrcmp(
x1->s, x2->s);} else{ if( x1->i < x2->i){ return - 1;} else{ return 1;}}}