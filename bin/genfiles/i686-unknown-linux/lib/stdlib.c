 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); struct Cyc_std__Div{ int quot; int rem; } ; struct Cyc_std__Ldiv{
int quot; int rem; } ; extern int abs( int __x); extern int atexit( void(*
__func)()); extern struct Cyc_std__Div div( int __numer, int __denom); extern
struct Cyc_std__Ldiv ldiv( int __numer, int __denom); extern int random();
extern void srandom( unsigned int __seed); extern int rand(); extern void srand(
unsigned int __seed); extern int rand_r( unsigned int* __seed); extern int
grantpt( int __fd); extern int unlockpt( int __fd); extern double Cyc_std_atof(
struct _tagged_arr); extern int Cyc_std_atoi( struct _tagged_arr); extern int
Cyc_std_atol( struct _tagged_arr); extern struct _tagged_arr Cyc_std_getenv(
struct _tagged_arr); extern double Cyc_std_strtod( struct _tagged_arr n, struct
_tagged_arr* end); extern int Cyc_std_strtol( struct _tagged_arr n, struct
_tagged_arr* end, int base); extern unsigned int Cyc_std_strtoul( struct
_tagged_arr n, struct _tagged_arr* end, int base); extern double atof(
unsigned char*); extern int atoi( unsigned char*); extern int atol(
unsigned char*); extern unsigned char* getenv( unsigned char*); extern int
putenv( unsigned char*); extern double strtod( unsigned char*, unsigned char**);
extern int strtol( unsigned char*, unsigned char**, int); extern unsigned int
strtoul( unsigned char*, unsigned char**, int); double Cyc_std_atof( struct
_tagged_arr _nptr){ return atof( string_to_Cstring( _nptr));} int Cyc_std_atoi(
struct _tagged_arr _nptr){ return atoi( string_to_Cstring( _nptr));} int Cyc_std_atol(
struct _tagged_arr _nptr){ return atol( string_to_Cstring( _nptr));} struct
_tagged_arr Cyc_std_getenv( struct _tagged_arr name){ return Cstring_to_string(
getenv( string_to_Cstring( name)));} int Cyc_std_putenv( struct _tagged_arr s){
return putenv( string_to_Cstring( s));} static void Cyc_std_check_valid_cstring(
struct _tagged_arr s){ if( s.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp0=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp1; _temp1.tag= Cyc_Core_InvalidArg; _temp1.f1= _tag_arr("strtox NULL pointer",
sizeof( unsigned char), 20u); _temp1;}); _temp0;}));}{ int found_zero= 0;{ int i=(
int)( _get_arr_size( s, sizeof( unsigned char)) -  1); for( 0; i >=  0; i --){
if(*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)) == '\000'){ found_zero= 1; break;}}} if( ! found_zero){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp2=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct
_temp3; _temp3.tag= Cyc_Core_InvalidArg; _temp3.f1= _tag_arr("strtox: not a C string",
sizeof( unsigned char), 23u); _temp3;}); _temp2;}));}}} double Cyc_std_strtod(
struct _tagged_arr nptr, struct _tagged_arr* endptr){ Cyc_std_check_valid_cstring(
nptr);{ unsigned char* c= underlying_Cstring( nptr); unsigned char* e= endptr == 
0? 0: c; double d= strtod( c,( unsigned char**)& e); if( endptr !=  0){ int n=(
int)(( unsigned int) e - ( unsigned int) c);*(( struct _tagged_arr*) _check_null(
endptr))= _tagged_arr_plus( nptr, sizeof( unsigned char), n);} return d;}} int
Cyc_std_strtol( struct _tagged_arr n, struct _tagged_arr* endptr, int base){ Cyc_std_check_valid_cstring(
n);{ unsigned char* c= underlying_Cstring( n); unsigned char* e= endptr ==  0? 0:
c; int r= strtol( c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)((
unsigned int) e - ( unsigned int) c);*(( struct _tagged_arr*) _check_null(
endptr))= _tagged_arr_plus( n, sizeof( unsigned char), m);} return r;}}
unsigned int Cyc_std_strtoul( struct _tagged_arr n, struct _tagged_arr* endptr,
int base){ Cyc_std_check_valid_cstring( n);{ unsigned char* c=
underlying_Cstring( n); unsigned char* e= endptr ==  0? 0: c; unsigned int r=
strtoul( c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)((
unsigned int) e - ( unsigned int) c);*(( struct _tagged_arr*) _check_null(
endptr))= _tagged_arr_plus( n, sizeof( unsigned char), m);} return r;}}
