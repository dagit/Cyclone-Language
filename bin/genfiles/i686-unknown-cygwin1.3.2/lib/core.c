 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct Cyc_Core_Opt* Cyc_Core_opt_map( void*(* f)( void*), struct Cyc_Core_Opt*
x); extern struct _tagged_arr Cyc_Core_new_string( int); extern struct
_tagged_arr Cyc_Core_rnew_string( struct _RegionHandle*, int); extern int Cyc_Core_true_f(
void*); extern int Cyc_Core_false_f( void*); struct _tuple0{ void* f1; void* f2;
} ; extern void* Cyc_Core_fst( struct _tuple0*); extern void* Cyc_Core_snd(
struct _tuple0*); struct _tuple1{ void* f1; void* f2; void* f3; } ; extern void*
Cyc_Core_third( struct _tuple1*); extern void* Cyc_Core_identity( void*); extern
int Cyc_Core_intcmp( int, int); extern int Cyc_Core_charcmp( unsigned char,
unsigned char); extern int Cyc_Core_ptrcmp( void**, void**); extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); unsigned char Cyc_Core_InvalidArg[
15u]="\000\000\000\000InvalidArg"; unsigned char Cyc_Core_SysError[ 13u]="\000\000\000\000SysError";
struct Cyc_Core_SysError_struct{ unsigned char* tag; int f1; } ; unsigned char
Cyc_Core_Failure[ 12u]="\000\000\000\000Failure"; unsigned char Cyc_Core_Impossible[
15u]="\000\000\000\000Impossible"; unsigned char Cyc_Core_Not_found[ 14u]="\000\000\000\000Not_found";
unsigned char Cyc_Core_Unreachable[ 16u]="\000\000\000\000Unreachable"; struct
Cyc_Core_Opt; struct Cyc_Core_Opt* Cyc_Core_opt_map( void*(* f)( void*), struct
Cyc_Core_Opt* o){ if( o ==  0){ return 0;} return({ struct Cyc_Core_Opt* _temp0=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=( void*)
f(( void*)(( struct Cyc_Core_Opt*) _check_null( o))->v); _temp0;});} struct
_tagged_arr Cyc_Core_new_string( int i){ return({ unsigned int _temp1=(
unsigned int) i; unsigned char* _temp2=( unsigned char*) GC_malloc_atomic(
sizeof( unsigned char) *  _temp1); struct _tagged_arr _temp4= _tag_arr( _temp2,
sizeof( unsigned char),( unsigned int) i);{ unsigned int _temp3= _temp1;
unsigned int j; for( j= 0; j <  _temp3; j ++){ _temp2[ j]='\000';}}; _temp4;});}
struct _tagged_arr Cyc_Core_rnew_string( struct _RegionHandle* r, int i){ return({
unsigned int _temp5=( unsigned int) i; unsigned char* _temp6=( unsigned char*)
_region_malloc( r, sizeof( unsigned char) *  _temp5); struct _tagged_arr _temp8=
_tag_arr( _temp6, sizeof( unsigned char),( unsigned int) i);{ unsigned int
_temp7= _temp5; unsigned int j; for( j= 0; j <  _temp7; j ++){ _temp6[ j]='\000';}};
_temp8;});} int Cyc_Core_true_f( void* x){ return 1;} int Cyc_Core_false_f( void*
x){ return 0;} int Cyc_Core_intcmp( int a, int b){ return a -  b;} int Cyc_Core_charcmp(
unsigned char a, unsigned char b){ return( int) a - ( int) b;} int Cyc_Core_ptrcmp(
void** a, void** b){ if( a ==  b){ return 0;} if( a >  b){ return 1;} return - 1;}
void* Cyc_Core_fst( struct _tuple0* pair){ return(* pair).f1;} void* Cyc_Core_snd(
struct _tuple0* pair){ return(* pair).f2;} void* Cyc_Core_third( struct _tuple1*
triple){ return(* triple).f3;} void* Cyc_Core_identity( void* x){ return x;}
