#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; void* f2;
void* f3; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t;
typedef int Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
GC_malloc_atomic( sizeof( char) * _temp2); unsigned int j; struct _tagged_string
_temp4={ _temp3, _temp3, _temp3 + _temp2}; for( j= 0; j < _temp2; j ++){ _temp3[
j]='\000';} _temp4;});} int Cyc_Core_true_f( void* x){ return 1;} int Cyc_Core_false_f(
void* x){ return 0;} int Cyc_Core_intcmp( int a, int b){ return a - b;} int Cyc_Core_charcmp(
char a, char b){ return( int) a -( int) b;} int Cyc_Core_ptrcmp( void** a, void**
b){ if( a == b){ return 0;} if( a > b){ return 1;} return - 1;} void* Cyc_Core_fst(
struct _tuple0* pair){ return(* pair).f1;} void* Cyc_Core_snd( struct _tuple0*
pair){ return(* pair).f2;} void* Cyc_Core_third( struct _tuple1* triple){ return(*
triple).f3;} void* Cyc_Core_identity( void* x){ return x;} int Cyc_Core_is_space(
char c){ switch(( int) c){ case 9: _LL5: return 1; case 10: _LL6: return 1; case
11: _LL7: return 1; case 12: _LL8: return 1; case 13: _LL9: return 1; case 32:
_LL10: return 1; default: _LL11: return 0;}} static int Cyc_Core_int_of_char(
char c){ if('0' <= c? c <='9': 0){ return c -'0';} else{ if('a' <= c? c <='f': 0){
return( 10 + c) -'a';} else{ if('A' <= c? c <='F': 0){ return( 10 + c) -'A';}
else{( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp13=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp13[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp14; _temp14.tag= Cyc_Core_InvalidArg_tag;
_temp14.f1=( struct _tagged_string)({ char* _temp15=( char*)"string to integer conversion";
struct _tagged_string _temp16; _temp16.curr= _temp15; _temp16.base= _temp15;
_temp16.last_plus_one= _temp15 + 29; _temp16;}); _temp14;}); _temp13;}));}}}}
int Cyc_Core_int_of_string( struct _tagged_string s){ int n; int i; int base;
int sign= 1; for( i= 0; i <({ struct _tagged_string _temp17= s;( unsigned int)(
_temp17.last_plus_one - _temp17.curr);})? Cyc_Core_is_space(({ struct
_tagged_string _temp18= s; char* _temp20= _temp18.curr + i; if( _temp18.base ==
0? 1:( _temp20 < _temp18.base? 1: _temp20 >= _temp18.last_plus_one)){ _throw(
Null_Exception);}* _temp20;})): 0; ++ i){;} while( i <({ struct _tagged_string
_temp21= s;( unsigned int)( _temp21.last_plus_one - _temp21.curr);})?({ struct
_tagged_string _temp22= s; char* _temp24= _temp22.curr + i; if( _temp22.base ==
0? 1:( _temp24 < _temp22.base? 1: _temp24 >= _temp22.last_plus_one)){ _throw(
Null_Exception);}* _temp24;}) =='-'? 1:({ struct _tagged_string _temp25= s; char*
_temp27= _temp25.curr + i; if( _temp25.base == 0? 1:( _temp27 < _temp25.base? 1:
_temp27 >= _temp25.last_plus_one)){ _throw( Null_Exception);}* _temp27;}) =='+':
0) { if(({ struct _tagged_string _temp28= s; char* _temp30= _temp28.curr + i;
if( _temp28.base == 0? 1:( _temp30 < _temp28.base? 1: _temp30 >= _temp28.last_plus_one)){
_throw( Null_Exception);}* _temp30;}) =='-'){ sign= - sign;} i ++;} if( i ==({
struct _tagged_string _temp31= s;( unsigned int)( _temp31.last_plus_one -
_temp31.curr);})){( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct*
_temp32=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp32[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp33; _temp33.tag= Cyc_Core_InvalidArg_tag;
_temp33.f1=( struct _tagged_string)({ char* _temp34=( char*)"string to integer conversion";
struct _tagged_string _temp35; _temp35.curr= _temp34; _temp35.base= _temp34;
_temp35.last_plus_one= _temp34 + 29; _temp35;}); _temp33;}); _temp32;}));} if( i
+ 1 ==({ struct _tagged_string _temp36= s;( unsigned int)( _temp36.last_plus_one
- _temp36.curr);})? 1:({ struct _tagged_string _temp37= s; char* _temp39=
_temp37.curr + i; if( _temp37.base == 0? 1:( _temp39 < _temp37.base? 1: _temp39
>= _temp37.last_plus_one)){ _throw( Null_Exception);}* _temp39;}) !='0'){ base=
10;} else{ switch(({ struct _tagged_string _temp40= s; char* _temp42= _temp40.curr
+( ++ i); if( _temp40.base == 0? 1:( _temp42 < _temp40.base? 1: _temp42 >=
_temp40.last_plus_one)){ _throw( Null_Exception);}* _temp42;})){ case 'x': _LL43:
base= 16; ++ i; break; case 'o': _LL44: base= 8; ++ i; break; case 'b': _LL45:
base= 2; ++ i; break; default: _LL46: base= 10; break;}} for( n= 0; i <({ struct
_tagged_string _temp48= s;( unsigned int)( _temp48.last_plus_one - _temp48.curr);})?({
struct _tagged_string _temp49= s; char* _temp51= _temp49.curr + i; if( _temp49.base
== 0? 1:( _temp51 < _temp49.base? 1: _temp51 >= _temp49.last_plus_one)){ _throw(
Null_Exception);}* _temp51;}) !='\000': 0; ++ i){ int digit= Cyc_Core_int_of_char(({
struct _tagged_string _temp52= s; char* _temp54= _temp52.curr + i; if( _temp52.base
== 0? 1:( _temp54 < _temp52.base? 1: _temp54 >= _temp52.last_plus_one)){ _throw(
Null_Exception);}* _temp54;})); if( digit >= base){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp55=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp55[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp56; _temp56.tag= Cyc_Core_InvalidArg_tag;
_temp56.f1=( struct _tagged_string)({ char* _temp57=( char*)"string to integer conversion";
struct _tagged_string _temp58; _temp58.curr= _temp57; _temp58.base= _temp57;
_temp58.last_plus_one= _temp57 + 29; _temp58;}); _temp56;}); _temp55;}));} n= n
* base + digit;} return sign * n;} struct _tagged_string Cyc_Core_string_of_int_width(
int n, int minWidth){ int i; int len= 0; int negative= 0; if( n < 0){ negative=
1; ++ len; n= - n;}{ int m= n; do { ++ len;} while (( m /= 10) > 0); len= len >
minWidth? len: minWidth;{ struct _tagged_string ans= Cyc_Core_new_string( len +
1); for( i= len - 1; n > 0; -- i){({ struct _tagged_string _temp59= ans; char*
_temp61= _temp59.curr + i; if( _temp59.base == 0? 1:( _temp61 < _temp59.base? 1:
_temp61 >= _temp59.last_plus_one)){ _throw( Null_Exception);}* _temp61=( char)('0'
+ n % 10);}); n /= 10;} for( 0; i >= 0; -- i){({ struct _tagged_string _temp62=
ans; char* _temp64= _temp62.curr + i; if( _temp62.base == 0? 1:( _temp64 <
_temp62.base? 1: _temp64 >= _temp62.last_plus_one)){ _throw( Null_Exception);}*
_temp64='0';});} if( negative){({ struct _tagged_string _temp65= ans; char*
_temp67= _temp65.curr + 0; if( _temp65.base == 0? 1:( _temp67 < _temp65.base? 1:
_temp67 >= _temp65.last_plus_one)){ _throw( Null_Exception);}* _temp67='-';});}
return ans;}}} struct _tagged_string Cyc_Core_string_of_int( int n){ return Cyc_Core_string_of_int_width(
n, 0);} struct _tagged_string Cyc_Core_string_of_uint( unsigned int n){ int len=
0; unsigned int m= n; do { ++ len;} while (( m /= 10) > 0);{ struct
_tagged_string ans= Cyc_Core_new_string( len + 1);{ int i= len - 1; for( 0; i >=
0; -- i){({ struct _tagged_string _temp68= ans; char* _temp70= _temp68.curr + i;
if( _temp68.base == 0? 1:( _temp70 < _temp68.base? 1: _temp70 >= _temp68.last_plus_one)){
_throw( Null_Exception);}* _temp70=( char)('0' + n % 10);}); n /= 10;}} return
ans;}} struct _tagged_string Cyc_Core_string_of_char( char c){ struct
_tagged_string ans= Cyc_Core_new_string( 2);({ struct _tagged_string _temp71=
ans; char* _temp73= _temp71.curr + 0; if( _temp71.base == 0? 1:( _temp73 <
_temp71.base? 1: _temp73 >= _temp71.last_plus_one)){ _throw( Null_Exception);}*
_temp73= c;}); return ans;}