#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; void* f2;
void* f3; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t;
typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int;
typedef unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef
unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t;
struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t;
typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int
Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t;
typedef char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t;
typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t; typedef long long
Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t;
typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t;
typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t;
typedef int Cyc_ssize_t; typedef char* Cyc_addr_t; typedef int Cyc_mode_t;
typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct Cyc_Core_Opt*
Cyc_Core_opt_map( void*(* f)( void*), struct Cyc_Core_Opt* x); extern struct
_tagged_string Cyc_Core_new_string( int); extern int Cyc_Core_true_f( void*);
extern int Cyc_Core_false_f( void*); extern void* Cyc_Core_fst( struct _tuple0*);
extern void* Cyc_Core_snd( struct _tuple0*); extern void* Cyc_Core_third( struct
_tuple1*); extern void* Cyc_Core_identity( void*); extern int Cyc_Core_intcmp(
int, int); extern int Cyc_Core_charcmp( char, char); extern int Cyc_Core_ptrcmp(
void**, void**); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern int Cyc_Core_is_space( char); extern int Cyc_Core_int_of_string( struct
_tagged_string); extern struct _tagged_string Cyc_Core_string_of_int( int);
extern struct _tagged_string Cyc_Core_string_of_uint( unsigned int); extern
struct _tagged_string Cyc_Core_string_of_char( char); extern char*
string_to_Cstring( struct _tagged_string); extern char* underlying_Cstring(
struct _tagged_string); extern struct _tagged_string Cstring_to_string( char*);
extern int system( char*); char Cyc_Core_InvalidArg_tag[ 11u]="InvalidArg"; char
Cyc_Core_SysError_tag[ 9u]="SysError"; struct Cyc_Core_SysError_struct{ char*
tag; int f1; } ; char Cyc_Core_Failure_tag[ 8u]="Failure"; char Cyc_Core_Impossible_tag[
11u]="Impossible"; char Cyc_Core_Not_found_tag[ 10u]="Not_found"; char Cyc_Core_Unreachable_tag[
12u]="Unreachable"; struct Cyc_Core_Opt* Cyc_Core_opt_map( void*(* f)( void*),
struct Cyc_Core_Opt* o){ if( o == 0){ return 0;} return({ struct Cyc_Core_Opt*
_temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=(
void*) f(( void*)({ struct Cyc_Core_Opt* _temp1= o; if( _temp1 == 0){ _throw(
Null_Exception);} _temp1->v;})); _temp0;});} struct _tagged_string Cyc_Core_new_string(
int i){ return({ unsigned int _temp2=( unsigned int) i; char* _temp3=( char*)
GC_malloc_atomic( sizeof( char) * _temp2); struct _tagged_string _temp6={ _temp3,
_temp3, _temp3 + _temp2};{ unsigned int _temp4= _temp2; unsigned int j; for( j=
0; j < _temp4; j ++){ _temp3[ j]='\000';}}; _temp6;});} int Cyc_Core_true_f(
void* x){ return 1;} int Cyc_Core_false_f( void* x){ return 0;} int Cyc_Core_intcmp(
int a, int b){ return a - b;} int Cyc_Core_charcmp( char a, char b){ return( int)
a -( int) b;} int Cyc_Core_ptrcmp( void** a, void** b){ if( a == b){ return 0;}
if( a > b){ return 1;} return - 1;} void* Cyc_Core_fst( struct _tuple0* pair){
return(* pair).f1;} void* Cyc_Core_snd( struct _tuple0* pair){ return(* pair).f2;}
void* Cyc_Core_third( struct _tuple1* triple){ return(* triple).f3;} void* Cyc_Core_identity(
void* x){ return x;} int Cyc_Core_is_space( char c){ switch(( int) c){ case 9:
_LL7: return 1; case 10: _LL8: return 1; case 11: _LL9: return 1; case 12: _LL10:
return 1; case 13: _LL11: return 1; case 32: _LL12: return 1; default: _LL13:
return 0;}} static int Cyc_Core_int_of_char( char c){ if('0' <= c? c <='9': 0){
return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';} else{ if('A'
<= c? c <='F': 0){ return( 10 + c) -'A';} else{( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp15=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp15[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp16; _temp16.tag= Cyc_Core_InvalidArg_tag;
_temp16.f1=( struct _tagged_string)({ char* _temp17=( char*)"string to integer conversion";
struct _tagged_string _temp18; _temp18.curr= _temp17; _temp18.base= _temp17;
_temp18.last_plus_one= _temp17 + 29; _temp18;}); _temp16;}); _temp15;}));}}}}
int Cyc_Core_int_of_string( struct _tagged_string s){ int n; int i; int base;
int sign= 1; for( i= 0; i <({ struct _tagged_string _temp19= s;( unsigned int)(
_temp19.last_plus_one - _temp19.curr);})? Cyc_Core_is_space(({ struct
_tagged_string _temp20= s; char* _temp22= _temp20.curr + i; if( _temp20.base ==
0? 1:( _temp22 < _temp20.base? 1: _temp22 >= _temp20.last_plus_one)){ _throw(
Null_Exception);}* _temp22;})): 0; ++ i){;} while( i <({ struct _tagged_string
_temp23= s;( unsigned int)( _temp23.last_plus_one - _temp23.curr);})?({ struct
_tagged_string _temp24= s; char* _temp26= _temp24.curr + i; if( _temp24.base ==
0? 1:( _temp26 < _temp24.base? 1: _temp26 >= _temp24.last_plus_one)){ _throw(
Null_Exception);}* _temp26;}) =='-'? 1:({ struct _tagged_string _temp27= s; char*
_temp29= _temp27.curr + i; if( _temp27.base == 0? 1:( _temp29 < _temp27.base? 1:
_temp29 >= _temp27.last_plus_one)){ _throw( Null_Exception);}* _temp29;}) =='+':
0) { if(({ struct _tagged_string _temp30= s; char* _temp32= _temp30.curr + i;
if( _temp30.base == 0? 1:( _temp32 < _temp30.base? 1: _temp32 >= _temp30.last_plus_one)){
_throw( Null_Exception);}* _temp32;}) =='-'){ sign= - sign;} i ++;} if( i ==({
struct _tagged_string _temp33= s;( unsigned int)( _temp33.last_plus_one -
_temp33.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp34=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp34[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp35; _temp35.tag= Cyc_Core_InvalidArg_tag;
_temp35.f1=( struct _tagged_string)({ char* _temp36=( char*)"string to integer conversion";
struct _tagged_string _temp37; _temp37.curr= _temp36; _temp37.base= _temp36;
_temp37.last_plus_one= _temp36 + 29; _temp37;}); _temp35;}); _temp34;}));} if( i
+ 1 ==({ struct _tagged_string _temp38= s;( unsigned int)( _temp38.last_plus_one
- _temp38.curr);})? 1:({ struct _tagged_string _temp39= s; char* _temp41=
_temp39.curr + i; if( _temp39.base == 0? 1:( _temp41 < _temp39.base? 1: _temp41
>= _temp39.last_plus_one)){ _throw( Null_Exception);}* _temp41;}) !='0'){ base=
10;} else{ switch(({ struct _tagged_string _temp42= s; char* _temp44= _temp42.curr
+( ++ i); if( _temp42.base == 0? 1:( _temp44 < _temp42.base? 1: _temp44 >=
_temp42.last_plus_one)){ _throw( Null_Exception);}* _temp44;})){ case 'x': _LL45:
base= 16; ++ i; break; case 'o': _LL46: base= 8; ++ i; break; case 'b': _LL47:
base= 2; ++ i; break; default: _LL48: base= 10; break;}} for( n= 0; i <({ struct
_tagged_string _temp50= s;( unsigned int)( _temp50.last_plus_one - _temp50.curr);})?({
struct _tagged_string _temp51= s; char* _temp53= _temp51.curr + i; if( _temp51.base
== 0? 1:( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one)){ _throw(
Null_Exception);}* _temp53;}) !='\000': 0; ++ i){ int digit= Cyc_Core_int_of_char(({
struct _tagged_string _temp54= s; char* _temp56= _temp54.curr + i; if( _temp54.base
== 0? 1:( _temp56 < _temp54.base? 1: _temp56 >= _temp54.last_plus_one)){ _throw(
Null_Exception);}* _temp56;})); if( digit >= base){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp57=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp57[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp58; _temp58.tag= Cyc_Core_InvalidArg_tag;
_temp58.f1=( struct _tagged_string)({ char* _temp59=( char*)"string to integer conversion";
struct _tagged_string _temp60; _temp60.curr= _temp59; _temp60.base= _temp59;
_temp60.last_plus_one= _temp59 + 29; _temp60;}); _temp58;}); _temp57;}));} n= n
* base + digit;} return sign * n;} struct _tagged_string Cyc_Core_string_of_int_width(
int n, int minWidth){ int i; int len= 0; int negative= 0; if( n < 0){ negative=
1; ++ len; n= - n;}{ int m= n; do { ++ len;} while (( m /= 10) > 0); len= len >
minWidth? len: minWidth;{ struct _tagged_string ans= Cyc_Core_new_string( len +
1); for( i= len - 1; n > 0; -- i){({ struct _tagged_string _temp61= ans; char*
_temp63= _temp61.curr + i; if( _temp61.base == 0? 1:( _temp63 < _temp61.base? 1:
_temp63 >= _temp61.last_plus_one)){ _throw( Null_Exception);}* _temp63=( char)('0'
+ n % 10);}); n /= 10;} for( 0; i >= 0; -- i){({ struct _tagged_string _temp64=
ans; char* _temp66= _temp64.curr + i; if( _temp64.base == 0? 1:( _temp66 <
_temp64.base? 1: _temp66 >= _temp64.last_plus_one)){ _throw( Null_Exception);}*
_temp66='0';});} if( negative){({ struct _tagged_string _temp67= ans; char*
_temp69= _temp67.curr + 0; if( _temp67.base == 0? 1:( _temp69 < _temp67.base? 1:
_temp69 >= _temp67.last_plus_one)){ _throw( Null_Exception);}* _temp69='-';});}
return ans;}}} struct _tagged_string Cyc_Core_string_of_int( int n){ return Cyc_Core_string_of_int_width(
n, 0);} struct _tagged_string Cyc_Core_string_of_uint( unsigned int n){ int len=
0; unsigned int m= n; do { ++ len;} while (( m /= 10) > 0);{ struct
_tagged_string ans= Cyc_Core_new_string( len + 1);{ int i= len - 1; for( 0; i >=
0; -- i){({ struct _tagged_string _temp70= ans; char* _temp72= _temp70.curr + i;
if( _temp70.base == 0? 1:( _temp72 < _temp70.base? 1: _temp72 >= _temp70.last_plus_one)){
_throw( Null_Exception);}* _temp72=( char)('0' + n % 10);}); n /= 10;}} return
ans;}} struct _tagged_string Cyc_Core_string_of_char( char c){ struct
_tagged_string ans= Cyc_Core_new_string( 2);({ struct _tagged_string _temp73=
ans; char* _temp75= _temp73.curr + 0; if( _temp73.base == 0? 1:( _temp75 <
_temp73.base? 1: _temp75 >= _temp73.last_plus_one)){ _throw( Null_Exception);}*
_temp75= c;}); return ans;}