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
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); extern struct
_tagged_string Cyc_Errno_sys_err( int); extern int* __errno()  __attribute__((
dllimport )) ; extern int _sys_nerr  __attribute__(( dllimport )) ; extern char*
_sys_errlist[ 135]  __attribute__(( dllimport )) ; struct _tagged_string Cyc_Errno_sys_err(
int i){ if( i < 0? 1: i > _sys_nerr){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp0=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0->tag= Cyc_Core_InvalidArg_tag; _temp0->f1=( struct _tagged_string)({ char*
_temp1=( char*)"sys_err: integer argument out of range"; struct _tagged_string
_temp2; _temp2.curr= _temp1; _temp2.base= _temp1; _temp2.last_plus_one= _temp1 +
39; _temp2;});( struct _xenum_struct*) _temp0;}));} return Cstring_to_string(({
char** _temp3=( char**) _sys_errlist; unsigned int _temp4= i; if( _temp4 >= 135u){
_throw( Null_Exception);} _temp3[ _temp4];}));}