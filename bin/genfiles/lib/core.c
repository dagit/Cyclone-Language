#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one; } ; struct _tuple0{ void* f1; void* f2; }
; struct _tuple1{ void* f1; void* f2; void* f3; } ; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); extern
struct _tagged_ptr0 Cyc_Core_std_args(); struct Cyc_Core_Opt{ void* v; } ;
typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct Cyc_Core_Opt* Cyc_Core_opt_map(
void*(* f)( void*), struct Cyc_Core_Opt* x); extern struct _tagged_string Cyc_Core_new_string(
int); extern int Cyc_Core_true_f( void*); extern int Cyc_Core_false_f( void*);
extern void* Cyc_Core_fst( struct _tuple0*); extern void* Cyc_Core_snd( struct
_tuple0*); extern void* Cyc_Core_third( struct _tuple1*); extern void* Cyc_Core_identity(
void*); extern int Cyc_Core_intcmp( int, int); extern int Cyc_Core_charcmp( char,
char); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_is_space( char);
extern int Cyc_Core_int_of_string( struct _tagged_string); extern struct
_tagged_string Cyc_Core_string_of_int( int); extern struct _tagged_string Cyc_Core_string_of_uint(
unsigned int); extern struct _tagged_string Cyc_Core_string_of_char( char);
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); extern int cyc_argc;
extern void start_args(); extern struct _tagged_string next_arg(); char Cyc_Core_InvalidArg_tag[
11u]="InvalidArg"; char Cyc_Core_SysError_tag[ 9u]="SysError"; struct Cyc_Core_SysError_struct{
char* tag; int f1; } ; struct _tagged_ptr0 Cyc_Core_std_args(){ struct
_tagged_ptr0 args=({ unsigned int _temp2=( unsigned int) cyc_argc; struct
_tagged_string* _temp3=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * _temp2); unsigned int i; struct _tagged_ptr0 _temp4={ _temp3,
_temp3, _temp3 + _temp2}; for( i= 0; i < _temp2; i ++){ _temp3[ i]=( struct
_tagged_string)({ char* _temp0=( char*)""; struct _tagged_string _temp1; _temp1.curr=
_temp0; _temp1.base= _temp0; _temp1.last_plus_one= _temp0 + 1; _temp1;});}
_temp4;}); start_args();{ int i= 0; for( 0; i < cyc_argc; i ++){({ struct
_tagged_ptr0 _temp5= args; struct _tagged_string* _temp7= _temp5.curr + i; if(
_temp7 < _temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7= next_arg();});}} return args;} char Cyc_Core_Failure_tag[ 8u]="Failure";
char Cyc_Core_Impossible_tag[ 11u]="Impossible"; char Cyc_Core_Not_found_tag[ 10u]="Not_found";
char Cyc_Core_Unreachable_tag[ 12u]="Unreachable"; struct Cyc_Core_Opt* Cyc_Core_opt_map(
void*(* f)( void*), struct Cyc_Core_Opt* o){ if( o == 0){ return 0;} return({
struct Cyc_Core_Opt* _temp8=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp8->v=( void*) f(( void*) o->v); _temp8;});} struct
_tagged_string Cyc_Core_new_string( int i){ return({ unsigned int _temp9=(
unsigned int) i; char* _temp10=( char*) GC_malloc_atomic( sizeof( char) * _temp9);
unsigned int j; struct _tagged_string _temp11={ _temp10, _temp10, _temp10 +
_temp9}; for( j= 0; j < _temp9; j ++){ _temp10[ j]='\000';} _temp11;});} int Cyc_Core_true_f(
void* x){ return 1;} int Cyc_Core_false_f( void* x){ return 0;} int Cyc_Core_intcmp(
int a, int b){ return a - b;} int Cyc_Core_charcmp( char a, char b){ return( int)
a -( int) b;} int Cyc_Core_ptrcmp( void** a, void** b){ if( a == b){ return 0;}
if( a > b){ return 1;} return - 1;} void* Cyc_Core_fst( struct _tuple0* pair){
return(* pair).f1;} void* Cyc_Core_snd( struct _tuple0* pair){ return(* pair).f2;}
void* Cyc_Core_third( struct _tuple1* triple){ return(* triple).f3;} void* Cyc_Core_identity(
void* x){ return x;} int Cyc_Core_is_space( char c){ switch(( int) c){ case 9:
_LL12: return 1; case 10: _LL13: return 1; case 11: _LL14: return 1; case 12:
_LL15: return 1; case 13: _LL16: return 1; case 32: _LL17: return 1; default:
_LL18: return 0;}} static int Cyc_Core_int_of_char( char c){ if(( int)'0' <=(
int) c?( int) c <=( int)'9': 0){ return( int) c -( int)'0';} else{ if(( int)'a'
<=( int) c?( int) c <=( int)'f': 0){ return( 10 +( int) c) -( int)'a';} else{
if(( int)'A' <=( int) c?( int) c <=( int)'F': 0){ return( 10 +( int) c) -( int)'A';}
else{( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp20=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp20[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp21; _temp21.tag= Cyc_Core_InvalidArg_tag;
_temp21.f1=( struct _tagged_string)({ char* _temp22=( char*)"string to integer conversion";
struct _tagged_string _temp23; _temp23.curr= _temp22; _temp23.base= _temp22;
_temp23.last_plus_one= _temp22 + 29; _temp23;}); _temp21;}); _temp20;}));}}}}
int Cyc_Core_int_of_string( struct _tagged_string s){ int n; int i; int base;
int sign= 1; for( i= 0;( unsigned int) i <({ struct _tagged_string _temp24= s;(
unsigned int)( _temp24.last_plus_one - _temp24.curr);})? Cyc_Core_is_space(({
struct _tagged_string _temp25= s; char* _temp27= _temp25.curr + i; if( _temp27 <
_temp25.base? 1: _temp27 >= _temp25.last_plus_one){ _throw( Null_Exception);}*
_temp27;})): 0; ++ i){;} while(( unsigned int) i <({ struct _tagged_string
_temp28= s;( unsigned int)( _temp28.last_plus_one - _temp28.curr);})?( int)({
struct _tagged_string _temp29= s; char* _temp31= _temp29.curr + i; if( _temp31 <
_temp29.base? 1: _temp31 >= _temp29.last_plus_one){ _throw( Null_Exception);}*
_temp31;}) ==( int)'-'? 1:( int)({ struct _tagged_string _temp32= s; char*
_temp34= _temp32.curr + i; if( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one){
_throw( Null_Exception);}* _temp34;}) ==( int)'+': 0) { if(( int)({ struct
_tagged_string _temp35= s; char* _temp37= _temp35.curr + i; if( _temp37 <
_temp35.base? 1: _temp37 >= _temp35.last_plus_one){ _throw( Null_Exception);}*
_temp37;}) ==( int)'-'){ sign= - sign;} i ++;} if(( unsigned int) i ==({ struct
_tagged_string _temp38= s;( unsigned int)( _temp38.last_plus_one - _temp38.curr);})){(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp39=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp39[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp40; _temp40.tag= Cyc_Core_InvalidArg_tag;
_temp40.f1=( struct _tagged_string)({ char* _temp41=( char*)"string to integer conversion";
struct _tagged_string _temp42; _temp42.curr= _temp41; _temp42.base= _temp41;
_temp42.last_plus_one= _temp41 + 29; _temp42;}); _temp40;}); _temp39;}));} if((
unsigned int)( i + 1) ==({ struct _tagged_string _temp43= s;( unsigned int)(
_temp43.last_plus_one - _temp43.curr);})? 1:( int)({ struct _tagged_string
_temp44= s; char* _temp46= _temp44.curr + i; if( _temp46 < _temp44.base? 1:
_temp46 >= _temp44.last_plus_one){ _throw( Null_Exception);}* _temp46;}) !=( int)'0'){
base= 10;} else{ switch(({ struct _tagged_string _temp47= s; char* _temp49=
_temp47.curr +( ++ i); if( _temp49 < _temp47.base? 1: _temp49 >= _temp47.last_plus_one){
_throw( Null_Exception);}* _temp49;})){ case 'x': _LL50: base= 16; ++ i; break;
case 'o': _LL51: base= 8; ++ i; break; case 'b': _LL52: base= 2; ++ i; break;
default: _LL53: base= 10; break;}} for( n= 0;( unsigned int) i <({ struct
_tagged_string _temp55= s;( unsigned int)( _temp55.last_plus_one - _temp55.curr);})?(
int)({ struct _tagged_string _temp56= s; char* _temp58= _temp56.curr + i; if(
_temp58 < _temp56.base? 1: _temp58 >= _temp56.last_plus_one){ _throw(
Null_Exception);}* _temp58;}) !=( int)'\000': 0; ++ i){ int digit= Cyc_Core_int_of_char(({
struct _tagged_string _temp59= s; char* _temp61= _temp59.curr + i; if( _temp61 <
_temp59.base? 1: _temp61 >= _temp59.last_plus_one){ _throw( Null_Exception);}*
_temp61;})); if( digit >= base){( void) _throw(( struct _xtunion_struct*)({
struct Cyc_Core_InvalidArg_struct* _temp62=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp62[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp63; _temp63.tag= Cyc_Core_InvalidArg_tag;
_temp63.f1=( struct _tagged_string)({ char* _temp64=( char*)"string to integer conversion";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 29; _temp65;}); _temp63;}); _temp62;}));} n= n
* base + digit;} return sign * n;} struct _tagged_string Cyc_Core_string_of_int_width(
int n, int minWidth){ int i; int len= 0; int negative= 0; if( n < 0){ negative=
1; ++ len; n= - n;}{ int m= n; do { ++ len;} while (( m /= 10) > 0); len= len >
minWidth? len: minWidth;{ struct _tagged_string ans= Cyc_Core_new_string( len +
1); for( i= len - 1; n > 0; -- i){({ struct _tagged_string _temp66= ans; char*
_temp68= _temp66.curr + i; if( _temp68 < _temp66.base? 1: _temp68 >= _temp66.last_plus_one){
_throw( Null_Exception);}* _temp68=( char)(( int)'0' + n % 10);}); n /= 10;}
for( 0; i >= 0; -- i){({ struct _tagged_string _temp69= ans; char* _temp71=
_temp69.curr + i; if( _temp71 < _temp69.base? 1: _temp71 >= _temp69.last_plus_one){
_throw( Null_Exception);}* _temp71='0';});} if( negative){({ struct
_tagged_string _temp72= ans; char* _temp74= _temp72.curr + 0; if( _temp74 <
_temp72.base? 1: _temp74 >= _temp72.last_plus_one){ _throw( Null_Exception);}*
_temp74='-';});} return ans;}}} struct _tagged_string Cyc_Core_string_of_int(
int n){ return Cyc_Core_string_of_int_width( n, 0);} struct _tagged_string Cyc_Core_string_of_uint(
unsigned int n){ int len= 0; unsigned int m= n; do { ++ len;} while (( m /= 10)
>( unsigned int) 0);{ struct _tagged_string ans= Cyc_Core_new_string( len + 1);{
int i= len - 1; for( 0; i >= 0; -- i){({ struct _tagged_string _temp75= ans;
char* _temp77= _temp75.curr + i; if( _temp77 < _temp75.base? 1: _temp77 >=
_temp75.last_plus_one){ _throw( Null_Exception);}* _temp77=( char)((
unsigned int)(( int)'0') + n %( unsigned int) 10);}); n /= 10;}} return ans;}}
struct _tagged_string Cyc_Core_string_of_char( char c){ struct _tagged_string
ans= Cyc_Core_new_string( 2);({ struct _tagged_string _temp78= ans; char*
_temp80= _temp78.curr + 0; if( _temp80 < _temp78.base? 1: _temp80 >= _temp78.last_plus_one){
_throw( Null_Exception);}* _temp80= c;}); return ans;}