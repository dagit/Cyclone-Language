#include "cyc_include.h"

 typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern struct _tagged_string Cyc_Core_string_of_int( int); extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern struct _tagged_string Cyc_String_strconcat_l( struct Cyc_List_List*);
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
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5[ 0]=( struct
_tagged_string)({ char* _temp6=( char*)"$"; struct _tagged_string _temp7; _temp7.curr=
_temp6; _temp7.base= _temp6; _temp7.last_plus_one= _temp6 + 2; _temp7;}); _temp5;});
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