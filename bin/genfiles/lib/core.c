#include "cyc_include.h"

 struct _tagged_ptr0{ struct _tagged_string* curr; struct _tagged_string* base;
struct _tagged_string* last_plus_one;}; struct _tuple0{ void* f1; void* f2;};
struct _tuple1{ void* f1; void* f2; void* f3;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); extern
struct _tagged_ptr0 Cyc_Core_std_args(); struct Cyc_Core_Opt{ void* v;}; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct Cyc_Core_Opt* Cyc_Core_opt_map(
void*(* f)( void*), struct Cyc_Core_Opt* x); extern struct _tagged_string Cyc_Core_new_string(
int); extern int Cyc_Core_true_f( void*); extern int Cyc_Core_false_f( void*);
extern void* Cyc_Core_fst( struct _tuple0*); extern void* Cyc_Core_snd( struct
_tuple0*); extern void* Cyc_Core_third( struct _tuple1*); extern void* Cyc_Core_identity(
void*); extern int Cyc_Core_intcmp( int, int); extern int Cyc_Core_charcmp( char,
char); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern int Cyc_Core_is_space( char);
extern int Cyc_Core_int_of_string( struct _tagged_string); extern struct
_tagged_string Cyc_Core_string_of_int( int); extern struct _tagged_string Cyc_Core_string_of_uint(
unsigned int); extern struct _tagged_string Cyc_Core_string_of_char( char);
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); extern int cyc_argc;
extern void start_args(); extern struct _tagged_string next_arg(); char Cyc_Core_InvalidArg_tag[
11u]="InvalidArg"; char Cyc_Core_SysError_tag[ 9u]="SysError"; struct Cyc_Core_SysError_struct{
char* tag; int f1;}; struct _tagged_ptr0 Cyc_Core_std_args(){ struct
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
else{( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp20=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp20=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp21=( char*)"string to integer conversion";
struct _tagged_string _temp22; _temp22.curr= _temp21; _temp22.base= _temp21;
_temp22.last_plus_one= _temp21 + 29; _temp22;})};( struct _xenum_struct*)
_temp20;}));}}}} int Cyc_Core_int_of_string( struct _tagged_string s){ int n;
int i; int base; int sign= 1; for( i= 0;( unsigned int) i <({ struct
_tagged_string _temp23= s;( unsigned int)( _temp23.last_plus_one - _temp23.curr);})?
Cyc_Core_is_space(({ struct _tagged_string _temp24= s; char* _temp26= _temp24.curr
+ i; if( _temp26 < _temp24.base? 1: _temp26 >= _temp24.last_plus_one){ _throw(
Null_Exception);}* _temp26;})): 0; ++ i){;} while(( unsigned int) i <({ struct
_tagged_string _temp27= s;( unsigned int)( _temp27.last_plus_one - _temp27.curr);})?(
int)({ struct _tagged_string _temp28= s; char* _temp30= _temp28.curr + i; if(
_temp30 < _temp28.base? 1: _temp30 >= _temp28.last_plus_one){ _throw(
Null_Exception);}* _temp30;}) ==( int)'-'? 1:( int)({ struct _tagged_string
_temp31= s; char* _temp33= _temp31.curr + i; if( _temp33 < _temp31.base? 1:
_temp33 >= _temp31.last_plus_one){ _throw( Null_Exception);}* _temp33;}) ==( int)'+':
0) { if(( int)({ struct _tagged_string _temp34= s; char* _temp36= _temp34.curr +
i; if( _temp36 < _temp34.base? 1: _temp36 >= _temp34.last_plus_one){ _throw(
Null_Exception);}* _temp36;}) ==( int)'-'){ sign= - sign;} i ++;} if((
unsigned int) i ==({ struct _tagged_string _temp37= s;( unsigned int)( _temp37.last_plus_one
- _temp37.curr);})){( void) _throw(({ struct Cyc_Core_InvalidArg_struct* _temp38=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp38=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp39=( char*)"string to integer conversion";
struct _tagged_string _temp40; _temp40.curr= _temp39; _temp40.base= _temp39;
_temp40.last_plus_one= _temp39 + 29; _temp40;})};( struct _xenum_struct*)
_temp38;}));} if(( unsigned int)( i + 1) ==({ struct _tagged_string _temp41= s;(
unsigned int)( _temp41.last_plus_one - _temp41.curr);})? 1:( int)({ struct
_tagged_string _temp42= s; char* _temp44= _temp42.curr + i; if( _temp44 <
_temp42.base? 1: _temp44 >= _temp42.last_plus_one){ _throw( Null_Exception);}*
_temp44;}) !=( int)'0'){ base= 10;} else{ switch(({ struct _tagged_string
_temp45= s; char* _temp47= _temp45.curr +( ++ i); if( _temp47 < _temp45.base? 1:
_temp47 >= _temp45.last_plus_one){ _throw( Null_Exception);}* _temp47;})){ case
'x': _LL48: base= 16; ++ i; break; case 'o': _LL49: base= 8; ++ i; break; case 'b':
_LL50: base= 2; ++ i; break; default: _LL51: base= 10; break;}} for( n= 0;(
unsigned int) i <({ struct _tagged_string _temp53= s;( unsigned int)( _temp53.last_plus_one
- _temp53.curr);})?( int)({ struct _tagged_string _temp54= s; char* _temp56=
_temp54.curr + i; if( _temp56 < _temp54.base? 1: _temp56 >= _temp54.last_plus_one){
_throw( Null_Exception);}* _temp56;}) !=( int)'\000': 0; ++ i){ int digit= Cyc_Core_int_of_char(({
struct _tagged_string _temp57= s; char* _temp59= _temp57.curr + i; if( _temp59 <
_temp57.base? 1: _temp59 >= _temp57.last_plus_one){ _throw( Null_Exception);}*
_temp59;})); if( digit >= base){( void) _throw(({ struct Cyc_Core_InvalidArg_struct*
_temp60=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));*
_temp60=( struct Cyc_Core_InvalidArg_struct){.tag= Cyc_Core_InvalidArg_tag,.f1=(
struct _tagged_string)({ char* _temp61=( char*)"string to integer conversion";
struct _tagged_string _temp62; _temp62.curr= _temp61; _temp62.base= _temp61;
_temp62.last_plus_one= _temp61 + 29; _temp62;})};( struct _xenum_struct*)
_temp60;}));} n= n * base + digit;} return sign * n;} struct _tagged_string Cyc_Core_string_of_int_width(
int n, int minWidth){ int i; int len= 0; int negative= 0; if( n < 0){ negative=
1; ++ len; n= - n;}{ int m= n; do { ++ len;} while (( m /= 10) > 0); len= len >
minWidth? len: minWidth;{ struct _tagged_string ans= Cyc_Core_new_string( len +
1); for( i= len - 1; n > 0; -- i){({ struct _tagged_string _temp63= ans; char*
_temp65= _temp63.curr + i; if( _temp65 < _temp63.base? 1: _temp65 >= _temp63.last_plus_one){
_throw( Null_Exception);}* _temp65=( char)(( int)'0' + n % 10);}); n /= 10;}
for( 0; i >= 0; -- i){({ struct _tagged_string _temp66= ans; char* _temp68=
_temp66.curr + i; if( _temp68 < _temp66.base? 1: _temp68 >= _temp66.last_plus_one){
_throw( Null_Exception);}* _temp68='0';});} if( negative){({ struct
_tagged_string _temp69= ans; char* _temp71= _temp69.curr + 0; if( _temp71 <
_temp69.base? 1: _temp71 >= _temp69.last_plus_one){ _throw( Null_Exception);}*
_temp71='-';});} return ans;}}} struct _tagged_string Cyc_Core_string_of_int(
int n){ return Cyc_Core_string_of_int_width( n, 0);} struct _tagged_string Cyc_Core_string_of_uint(
unsigned int n){ int len= 0; unsigned int m= n; do { ++ len;} while (( m /= 10)
>( unsigned int) 0);{ struct _tagged_string ans= Cyc_Core_new_string( len + 1);{
int i= len - 1; for( 0; i >= 0; -- i){({ struct _tagged_string _temp72= ans;
char* _temp74= _temp72.curr + i; if( _temp74 < _temp72.base? 1: _temp74 >=
_temp72.last_plus_one){ _throw( Null_Exception);}* _temp74=( char)((
unsigned int)(( int)'0') + n %( unsigned int) 10);}); n /= 10;}} return ans;}}
struct _tagged_string Cyc_Core_string_of_char( char c){ struct _tagged_string
ans= Cyc_Core_new_string( 2);({ struct _tagged_string _temp75= ans; char*
_temp77= _temp75.curr + 0; if( _temp77 < _temp75.base? 1: _temp77 >= _temp75.last_plus_one){
_throw( Null_Exception);}* _temp77= c;}); return ans;}