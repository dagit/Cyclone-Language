 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern struct Cyc_Core_Opt* Cyc_Core_opt_map( void*(* f)( void*),
struct Cyc_Core_Opt* x); extern struct _tagged_string Cyc_Core_new_string( int);
extern struct _tagged_string Cyc_Core_rnew_string( struct _RegionHandle*, int);
extern int Cyc_Core_true_f( void*); extern int Cyc_Core_false_f( void*); struct
_tuple0{ void* f1; void* f2; } ; extern void* Cyc_Core_fst( struct _tuple0*);
extern void* Cyc_Core_snd( struct _tuple0*); struct _tuple1{ void* f1; void* f2;
void* f3; } ; extern void* Cyc_Core_third( struct _tuple1*); extern void* Cyc_Core_identity(
void*); extern int Cyc_Core_intcmp( int, int); extern int Cyc_Core_charcmp(
unsigned char, unsigned char); extern int Cyc_Core_ptrcmp( void**, void**);
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_Core_is_space(
unsigned char); extern int Cyc_Core_int_of_string( struct _tagged_string);
extern struct _tagged_string Cyc_Core_string_of_int( int); extern struct
_tagged_string Cyc_Core_string_of_uint( unsigned int); extern struct
_tagged_string Cyc_Core_string_of_char( unsigned char); extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); unsigned char Cyc_Core_InvalidArg[ 15u]="\000\000\000\000InvalidArg";
unsigned char Cyc_Core_SysError[ 13u]="\000\000\000\000SysError"; struct Cyc_Core_SysError_struct{
unsigned char* tag; int f1; } ; unsigned char Cyc_Core_Failure[ 12u]="\000\000\000\000Failure";
unsigned char Cyc_Core_Impossible[ 15u]="\000\000\000\000Impossible";
unsigned char Cyc_Core_Not_found[ 14u]="\000\000\000\000Not_found";
unsigned char Cyc_Core_Unreachable[ 16u]="\000\000\000\000Unreachable"; struct
Cyc_Core_Opt* Cyc_Core_opt_map( void*(* f)( void*), struct Cyc_Core_Opt* o){ if(
o == 0){ return 0;} return({ struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp0->v=( void*) f(( void*)(( struct
Cyc_Core_Opt*) _check_null( o))->v); _temp0;});} struct _tagged_string Cyc_Core_new_string(
int i){ return({ unsigned int _temp1=( unsigned int) i; unsigned char* _temp2=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp1); struct
_tagged_string _temp4={ _temp2, _temp2, _temp2 + _temp1};{ unsigned int _temp3=
_temp1; unsigned int j; for( j= 0; j < _temp3; j ++){ _temp2[ j]='\000';}};
_temp4;});} struct _tagged_string Cyc_Core_rnew_string( struct _RegionHandle* r,
int i){ return({ unsigned int _temp5=( unsigned int) i; unsigned char* _temp6=(
unsigned char*) _region_malloc( r, sizeof( unsigned char) * _temp5); struct
_tagged_string _temp8={ _temp6, _temp6, _temp6 + _temp5};{ unsigned int _temp7=
_temp5; unsigned int j; for( j= 0; j < _temp7; j ++){ _temp6[ j]='\000';}};
_temp8;});} int Cyc_Core_true_f( void* x){ return 1;} int Cyc_Core_false_f( void*
x){ return 0;} int Cyc_Core_intcmp( int a, int b){ return a - b;} int Cyc_Core_charcmp(
unsigned char a, unsigned char b){ return( int) a -( int) b;} int Cyc_Core_ptrcmp(
void** a, void** b){ if( a == b){ return 0;} if( a > b){ return 1;} return - 1;}
void* Cyc_Core_fst( struct _tuple0* pair){ return(* pair).f1;} void* Cyc_Core_snd(
struct _tuple0* pair){ return(* pair).f2;} void* Cyc_Core_third( struct _tuple1*
triple){ return(* triple).f3;} void* Cyc_Core_identity( void* x){ return x;} int
Cyc_Core_is_space( unsigned char c){ switch(( int) c){ case 9: _LL9: return 1;
case 10: _LL10: return 1; case 11: _LL11: return 1; case 12: _LL12: return 1;
case 13: _LL13: return 1; case 32: _LL14: return 1; default: _LL15: return 0;}}
static int Cyc_Core_int_of_char( unsigned char c){ if('0' <= c? c <='9': 0){
return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';} else{ if('A'
<= c? c <='F': 0){ return( 10 + c) -'A';} else{( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp17=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp17[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp18; _temp18.tag= Cyc_Core_InvalidArg; _temp18.f1=({
unsigned char* _temp19=( unsigned char*)"string to integer conversion"; struct
_tagged_string _temp20; _temp20.curr= _temp19; _temp20.base= _temp19; _temp20.last_plus_one=
_temp19 + 29; _temp20;}); _temp18;}); _temp17;}));}}}} int Cyc_Core_int_of_string(
struct _tagged_string s){ int n; int i; int base; int sign= 1; for( i= 0; i <({
struct _tagged_string _temp21= s;( unsigned int)( _temp21.last_plus_one -
_temp21.curr);})? Cyc_Core_is_space(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i)): 0; ++ i){;} while( i <({ struct _tagged_string
_temp22= s;( unsigned int)( _temp22.last_plus_one - _temp22.curr);})?*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='-'? 1:*(( const
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='+': 0) { if(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i) =='-'){ sign= - sign;}
i ++;} if( i ==({ struct _tagged_string _temp23= s;( unsigned int)( _temp23.last_plus_one
- _temp23.curr);})){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp24=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp24[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp25; _temp25.tag= Cyc_Core_InvalidArg;
_temp25.f1=({ unsigned char* _temp26=( unsigned char*)"string to integer conversion";
struct _tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26;
_temp27.last_plus_one= _temp26 + 29; _temp27;}); _temp25;}); _temp24;}));} if( i
+ 1 ==({ struct _tagged_string _temp28= s;( unsigned int)( _temp28.last_plus_one
- _temp28.curr);})? 1:*(( const unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
i) !='0'){ base= 10;} else{ switch(*(( const unsigned char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), ++ i)){ case 'x': _LL29: base= 16; ++ i; break; case 'o':
_LL30: base= 8; ++ i; break; case 'b': _LL31: base= 2; ++ i; break; default:
_LL32: base= 10; break;}} for( n= 0; i <({ struct _tagged_string _temp34= s;(
unsigned int)( _temp34.last_plus_one - _temp34.curr);})?*(( const unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( unsigned char), i) !='\000': 0; ++ i){ int digit= Cyc_Core_int_of_char(*((
const unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( unsigned char), i)); if( digit >= base){(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp35=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp35[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp36; _temp36.tag= Cyc_Core_InvalidArg; _temp36.f1=({
unsigned char* _temp37=( unsigned char*)"string to integer conversion"; struct
_tagged_string _temp38; _temp38.curr= _temp37; _temp38.base= _temp37; _temp38.last_plus_one=
_temp37 + 29; _temp38;}); _temp36;}); _temp35;}));} n= n * base + digit;} return
sign * n;} struct _tagged_string Cyc_Core_string_of_int_width( int n, int
minWidth){ int i; int len= 0; int negative= 0; if( n < 0){ negative= 1; ++ len;
n= - n;}{ int m= n; do { ++ len;} while (( m /= 10) > 0); len= len > minWidth?
len: minWidth;{ struct _tagged_string ans= Cyc_Core_new_string( len + 1); for( i=
len - 1; n > 0; -- i){*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( ans, sizeof( unsigned char), i)=(
unsigned char)('0' + n % 10); n /= 10;} for( 0; i >= 0; -- i){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
ans, sizeof( unsigned char), i)='0';} if( negative){*(( unsigned char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
ans, sizeof( unsigned char), 0)='-';} return ans;}}} struct _tagged_string Cyc_Core_string_of_int(
int n){ return Cyc_Core_string_of_int_width( n, 0);} struct _tagged_string Cyc_Core_string_of_uint(
unsigned int n){ int len= 0; unsigned int m= n; do { ++ len;} while (( m /= 10)
> 0);{ struct _tagged_string ans= Cyc_Core_new_string( len + 1);{ int i= len - 1;
for( 0; i >= 0; -- i){*(( unsigned char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( ans, sizeof( unsigned char), i)=(
unsigned char)('0' + n % 10); n /= 10;}} return ans;}} struct _tagged_string Cyc_Core_string_of_char(
unsigned char c){ struct _tagged_string ans= Cyc_Core_new_string( 2);*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( ans, sizeof( unsigned char), 0)= c; return ans;}