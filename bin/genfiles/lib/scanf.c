 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); extern int isalnum( int)
 __attribute__(( cdecl )) ; extern int isalpha( int)  __attribute__(( cdecl )) ;
extern int iscntrl( int)  __attribute__(( cdecl )) ; extern int isdigit( int)
 __attribute__(( cdecl )) ; extern int isgraph( int)  __attribute__(( cdecl )) ;
extern int islower( int)  __attribute__(( cdecl )) ; extern int isprint( int)
 __attribute__(( cdecl )) ; extern int ispunct( int)  __attribute__(( cdecl )) ;
extern int isspace( int)  __attribute__(( cdecl )) ; extern int isupper( int)
 __attribute__(( cdecl )) ; extern int isxdigit( int)  __attribute__(( cdecl )) ;
extern int tolower( int)  __attribute__(( cdecl )) ; extern int toupper( int)
 __attribute__(( cdecl )) ; extern int isascii( int)  __attribute__(( cdecl )) ;
extern int toascii( int)  __attribute__(( cdecl )) ; extern int _tolower( int)
 __attribute__(( cdecl )) ; extern int _toupper( int)  __attribute__(( cdecl )) ;
struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct Cyc_Stdlib__Ldiv{ int quot;
int rem; } ; extern int __mb_cur_max; extern int abs( int)  __attribute__((
cdecl )) ; extern int atexit( void(* __func)())  __attribute__(( cdecl )) ;
extern struct Cyc_Stdlib__Div div( int __numer, int __denom)  __attribute__((
cdecl )) ; extern struct Cyc_Stdlib__Ldiv ldiv( int __numer, int __denom)
 __attribute__(( cdecl )) ; extern int rand()  __attribute__(( cdecl )) ; extern
void srand( unsigned int __seed)  __attribute__(( cdecl )) ; extern int rand_r(
unsigned int* __seed)  __attribute__(( cdecl )) ; extern int random()
 __attribute__(( cdecl )) ; extern int srandom( unsigned int __seed)
 __attribute__(( cdecl )) ; extern int grantpt( int)  __attribute__(( cdecl )) ;
extern int unlockpt( int)  __attribute__(( cdecl )) ; extern double Cyc_Stdlib_atof(
struct _tagged_arr); extern int Cyc_Stdlib_strtol( struct _tagged_arr n, struct
_tagged_arr* end, int base); extern unsigned int Cyc_Stdlib_strtoul( struct
_tagged_arr n, struct _tagged_arr* end, int base); struct Cyc_Stdio___sFILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stdin; extern int Cyc_Stdio_fgetc(
struct Cyc_Stdio___sFILE*); extern int Cyc_Stdio_getc( struct Cyc_Stdio___sFILE*);
extern int Cyc_Stdio_ungetc( int, struct Cyc_Stdio___sFILE*); extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int Cyc_Stdio_scanf( struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Stdio_fscanf( struct Cyc_Stdio___sFILE*
stream, struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Stdio_sscanf(
struct _tagged_arr src, struct _tagged_arr fmt, struct _tagged_arr); extern int
Cyc_Stdio_vfscanf( struct Cyc_Stdio___sFILE* stream, struct _tagged_arr fmt,
struct _tagged_arr); extern int Cyc_Stdio_vsscanf( struct _tagged_arr src,
struct _tagged_arr fmt, struct _tagged_arr); static struct _tagged_arr Cyc_Stdio___sccl(
struct _tagged_arr tab, struct _tagged_arr fmt); static short* Cyc_Stdio_va_arg_short_ptr(
void* a){ void* _temp0= a; short* _temp8; unsigned short* _temp10; _LL2: if(*((
int*) _temp0) ==  Cyc_Stdio_ShortPtr_sa){ _LL9: _temp8=(( struct Cyc_Stdio_ShortPtr_sa_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) ==  Cyc_Stdio_UShortPtr_sa){
_LL11: _temp10=(( struct Cyc_Stdio_UShortPtr_sa_struct*) _temp0)->f1; goto _LL5;}
else{ goto _LL6;} _LL6: goto _LL7; _LL3: return _temp8; _LL5: return( short*)
_temp10; _LL7:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp12=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp12[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp13; _temp13.tag= Cyc_Core_InvalidArg;
_temp13.f1= _tag_arr("scan expects short pointer", sizeof( unsigned char), 27u);
_temp13;}); _temp12;})); _LL1:;} static int* Cyc_Stdio_va_arg_int_ptr( void* a){
void* _temp14= a; int* _temp22; unsigned int* _temp24; _LL16: if(*(( int*)
_temp14) ==  Cyc_Stdio_IntPtr_sa){ _LL23: _temp22=(( struct Cyc_Stdio_IntPtr_sa_struct*)
_temp14)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(*(( int*) _temp14) == 
Cyc_Stdio_UIntPtr_sa){ _LL25: _temp24=(( struct Cyc_Stdio_UIntPtr_sa_struct*)
_temp14)->f1; goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL17: return
_temp22; _LL19: return( int*) _temp24; _LL21:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp26=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp26[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp27; _temp27.tag= Cyc_Core_InvalidArg;
_temp27.f1= _tag_arr("scan expects int pointer", sizeof( unsigned char), 25u);
_temp27;}); _temp26;})); _LL15:;} static struct _tagged_arr Cyc_Stdio_va_arg_string_ptr(
void* a){ void* _temp28= a; struct _tagged_arr _temp34; _LL30: if(*(( int*)
_temp28) ==  Cyc_Stdio_StringPtr_sa){ _LL35: _temp34=(( struct Cyc_Stdio_StringPtr_sa_struct*)
_temp28)->f1; goto _LL31;} else{ goto _LL32;} _LL32: goto _LL33; _LL31: return
_temp34; _LL33:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp36=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp36[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp37; _temp37.tag= Cyc_Core_InvalidArg;
_temp37.f1= _tag_arr("scan expects char pointer", sizeof( unsigned char), 26u);
_temp37;}); _temp36;})); _LL29:;} static double* Cyc_Stdio_va_arg_double_ptr(
void* a){ void* _temp38= a; double* _temp44; _LL40: if(*(( int*) _temp38) == 
Cyc_Stdio_DoublePtr_sa){ _LL45: _temp44=(( struct Cyc_Stdio_DoublePtr_sa_struct*)
_temp38)->f1; goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43; _LL41: return
_temp44; _LL43:( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp46=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp46[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp47; _temp47.tag= Cyc_Core_InvalidArg;
_temp47.f1= _tag_arr("scan expects double pointer", sizeof( unsigned char), 28u);
_temp47;}); _temp46;})); _LL39:;} static float* Cyc_Stdio_va_arg_float_ptr( void*
a){ void* _temp48= a; float* _temp54; _LL50: if(*(( int*) _temp48) ==  Cyc_Stdio_FloatPtr_sa){
_LL55: _temp54=(( struct Cyc_Stdio_FloatPtr_sa_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: goto _LL53; _LL51: return _temp54; _LL53:( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp56=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp56[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp57; _temp57.tag= Cyc_Core_InvalidArg; _temp57.f1=
_tag_arr("scan expects float pointer", sizeof( unsigned char), 27u); _temp57;});
_temp56;})); _LL49:;} int Cyc_Stdio__IO_vfscanf( int(* _IO_getc)( void*), int(*
_IO_ungetc)( int, void*), int(* _IO_peekc)( void*), void* fp, struct _tagged_arr
fmt0, struct _tagged_arr ap, int* errp){ struct _tagged_arr fmt= fmt0; int c;
long long width; struct _tagged_arr p= _tag_arr( 0u, 0u, 0u); int n; int flags=
0; struct _tagged_arr p0= _tag_arr( 0u, 0u, 0u); int nassigned; int nread; int
base= 0; int use_strtoul= 0; unsigned char ccltab[ 256u];{ unsigned int _temp154=
256u; unsigned int i; for( i= 0; i <  _temp154; i ++){ ccltab[ i]='0';}}{
unsigned char buf[ 350u];{ unsigned int _temp153= 350u; unsigned int i; for( i=
0; i <  _temp153; i ++){ buf[ i]='0';}}{ int seen_eof= 0; static short basefix[
17u]={ 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; nassigned= 0;
nread= 0; for( 0; 1; 0){ c=( int)*(( const unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u)); if( c ==  0){ goto done;} if(
isspace( c)){ for( 0; 1; 0){ c= _IO_getc( fp); if( c ==  - 1){ seen_eof ++;
break;} if( ! isspace( c)){ _IO_ungetc( c, fp); break;} nread ++;} continue;}
if( c != '%'){ goto literal;} width=( long long) 0; flags= 0; again: c=( int)*((
const unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(&
fmt, sizeof( unsigned char), 1), sizeof( unsigned char), 0u)); switch( c){ case
'%': _LL58: literal: n= _IO_getc( fp); if( n ==  - 1){ goto eof_failure;} if( n
!=  c){ _IO_ungetc( n, fp); goto match_failure;} nread ++; continue; case '*':
_LL59: if( flags){ goto control_failure;} flags= 8; goto again; case 'l': _LL60:
if( flags &  ~( 8 |  64)){ goto control_failure;} flags |= 1; goto again; case 'L':
_LL61: if( flags &  ~( 8 |  64)){ goto control_failure;} flags |= 2; goto again;
case 'h': _LL62: if( flags &  ~( 8 |  64)){ goto control_failure;} flags |= 4;
goto again; case '0': _LL63: goto _LL64; case '1': _LL64: goto _LL65; case '2':
_LL65: goto _LL66; case '3': _LL66: goto _LL67; case '4': _LL67: goto _LL68;
case '5': _LL68: goto _LL69; case '6': _LL69: goto _LL70; case '7': _LL70: goto
_LL71; case '8': _LL71: goto _LL72; case '9': _LL72: if( flags &  ~( 8 |  64)){
goto control_failure;} flags |= 64; width=( width *  10 +  c) - '0'; goto again;
case 'D': _LL73: flags |= 1; goto _LL74; case 'd': _LL74: c= 3; use_strtoul= 0;
base= 10; break; case 'i': _LL75: c= 3; use_strtoul= 0; base= 0; break; case 'O':
_LL76: flags |= 1; goto _LL77; case 'o': _LL77: c= 3; use_strtoul= 1; base= 8;
break; case 'u': _LL78: c= 3; use_strtoul= 1; base= 10; break; case 'X': _LL79:
goto _LL80; case 'x': _LL80: flags |= 256; c= 3; use_strtoul= 1; base= 16;
break; case 'E': _LL81: goto _LL82; case 'F': _LL82: goto _LL83; case 'e': _LL83:
goto _LL84; case 'f': _LL84: goto _LL85; case 'g': _LL85: c= 4; break; case 's':
_LL86: c= 2; break; case '[': _LL87: fmt= Cyc_Stdio___sccl( _tag_arr( ccltab,
sizeof( unsigned char), 256u), fmt); flags |= 32; c= 1; break; case 'c': _LL88:
flags |= 32; c= 0; break; case 'p': _LL89: flags |= 16 |  256; c= 3; use_strtoul=
1; base= 16; break; case 'n': _LL90: if( flags &  8){ continue;} if( flags &  4){*
Cyc_Stdio_va_arg_short_ptr(*(( void**) _check_unknown_subscript( ap, sizeof(
void*), 0u)))=( short) nread;} else{ if( flags &  1){* Cyc_Stdio_va_arg_int_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) nread;} else{*
Cyc_Stdio_va_arg_int_ptr(*(( void**) _check_unknown_subscript( ap, sizeof( void*),
0u)))=( int) nread;}} _tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);
continue; case '\000': _LL91: nassigned= - 1; goto done; default: _LL92: if(
isupper( c)){ flags |= 1;} c= 3; use_strtoul= 0; base= 10; break;} if( _IO_peekc(
fp) ==  - 1){ goto eof_failure;} if(( flags &  32) ==  0){ n= _IO_peekc( fp);
while( isspace( n)) { n= _IO_getc( fp); nread ++; n= _IO_peekc( fp); if( n ==  -
1){ goto eof_failure;}}} switch( c){ case 0: _LL94: if( width ==  0){ width=(
long long) 1;} if( flags &  8){ long long sum=( long long) 0; for( 0; 1; 0){ n=
_IO_getc( fp); if( n ==  - 1? width !=  0: 0){ goto eof_failure;} else{ if( n == 
- 1){ seen_eof ++; break;}} sum ++; width --;} nread += sum;} else{ long long
sum=( long long) 0; struct _tagged_arr _temp96= Cyc_Stdio_va_arg_string_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); for( 0; 1; 0){ n=
_IO_getc( fp); if( n ==  - 1? width !=  0: 0){ goto eof_failure;} else{ if( n == 
- 1){ seen_eof ++; break;}} if( _get_arr_size( _temp96, sizeof( unsigned char))
==  0){ goto eof_failure;}*(( unsigned char*) _check_unknown_subscript( _temp96,
sizeof( unsigned char), 0u))=( unsigned char) n; sum ++; width --;
_tagged_arr_inplace_plus_post(& _temp96, sizeof( unsigned char), 1);} nread +=
sum; if( _get_arr_size( _temp96, sizeof( unsigned char)) !=  0){*((
unsigned char*) _check_unknown_subscript( _temp96, sizeof( unsigned char), 0u))='\000';}
nassigned ++;} break; case 1: _LL95: if( width ==  0){ width=( long long) ~ 0;}
if( flags &  8){ n= 0;{ int c= _IO_peekc( fp); while(( int) ccltab[
_check_known_subscript_notnull( 256u,( int)(( unsigned char) c))]) { n ++;
_IO_getc( fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){
if( n ==  0){ goto eof_failure;} seen_eof ++; break;}} if( n ==  0){ goto
match_failure;}}} else{ struct _tagged_arr _temp98= Cyc_Stdio_va_arg_string_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);{ struct _tagged_arr
_temp99= _temp98; int c= _IO_peekc( fp); while(( int) ccltab[
_check_known_subscript_notnull( 256u,( int)(( unsigned char) c))]) { if(
_get_arr_size( _temp99, sizeof( unsigned char)) ==  0){ goto eof_failure;}*((
unsigned char*) _check_unknown_subscript( _temp99, sizeof( unsigned char), 0u))=(
unsigned char) c; _tagged_arr_inplace_plus_post(& _temp99, sizeof( unsigned char),
1); _IO_getc( fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){
if( _temp99.curr ==  p0.curr){ goto eof_failure;} seen_eof ++; break;}} n=(
_temp99.curr -  _temp98.curr) /  sizeof( unsigned char); if( n ==  0){ goto
match_failure;} if( _get_arr_size( _temp99, sizeof( unsigned char)) ==  0){ goto
eof_failure;}*(( unsigned char*) _check_unknown_subscript( _temp99, sizeof(
unsigned char), 0u))='\000'; nassigned ++;}} nread += n; break; case 2: _LL97:
if( width ==  0){ width=( long long) ~ 0;} if( flags &  8){ n= 0;{ int c=
_IO_peekc( fp); while( ! isspace(( int)(( unsigned char) c))) { n ++; _IO_getc(
fp); if( -- width ==  0){ break;} if(( c= _IO_peekc( fp)) ==  - 1){ seen_eof ++;
break;}} nread += n;}} else{ struct _tagged_arr _temp101= Cyc_Stdio_va_arg_string_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)));
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1);{ struct _tagged_arr
_temp102= _temp101; int c= _IO_peekc( fp); while( ! isspace(( int)((
unsigned char) c))) { if( _get_arr_size( _temp102, sizeof( unsigned char)) ==  0){
goto eof_failure;}*(( unsigned char*) _check_unknown_subscript( _temp102,
sizeof( unsigned char), 0u))=( unsigned char) c; _tagged_arr_inplace_plus_post(&
_temp102, sizeof( unsigned char), 1); if( -- width ==  0){ break;} if(( c=
_IO_peekc( fp)) ==  - 1){ seen_eof ++; break;}} if( _get_arr_size( _temp102,
sizeof( unsigned char)) ==  0){ goto eof_failure;}*(( unsigned char*)
_check_unknown_subscript( _temp102, sizeof( unsigned char), 0u))='\000'; nread +=(
_temp102.curr -  _temp101.curr) /  sizeof( unsigned char); nassigned ++;}}
continue; case 3: _LL100: if( width ==  0? 1: width >  sizeof( buf) -  1){ width=(
long long)( sizeof( buf) -  1);} flags |=( 64 |  128) |  512; for( p= _tag_arr(
buf, sizeof( unsigned char), 350u); width !=  0; width --){ c=( int)((
unsigned char) _IO_peekc( fp)); switch( c){ case '0': _LL104: if( base ==  0){
base= 8; flags |= 256;} if( flags &  512){ flags &= ~(( 64 |  512) |  128);}
else{ flags &= ~(( 64 |  256) |  128);} goto ok; case '1': _LL105: goto _LL106;
case '2': _LL106: goto _LL107; case '3': _LL107: goto _LL108; case '4': _LL108:
goto _LL109; case '5': _LL109: goto _LL110; case '6': _LL110: goto _LL111; case
'7': _LL111: base=( int) basefix[ _check_known_subscript_notnull( 17u, base)];
flags &= ~(( 64 |  256) |  128); goto ok; case '8': _LL112: goto _LL113; case '9':
_LL113: base=( int) basefix[ _check_known_subscript_notnull( 17u, base)]; if(
base <=  8){ break;} flags &= ~(( 64 |  256) |  128); goto ok; case 'A': _LL114:
goto _LL115; case 'B': _LL115: goto _LL116; case 'C': _LL116: goto _LL117; case
'D': _LL117: goto _LL118; case 'E': _LL118: goto _LL119; case 'F': _LL119: goto
_LL120; case 'a': _LL120: goto _LL121; case 'b': _LL121: goto _LL122; case 'c':
_LL122: goto _LL123; case 'd': _LL123: goto _LL124; case 'e': _LL124: goto
_LL125; case 'f': _LL125: if( base <=  10){ break;} flags &= ~(( 64 |  256) | 
128); goto ok; case '+': _LL126: goto _LL127; case '-': _LL127: if( flags &  64){
flags &= ~ 64; goto ok;} break; case 'x': _LL128: goto _LL129; case 'X': _LL129:
if( flags &  256? p.curr == ( _tagged_arr_plus( _tag_arr( buf, sizeof(
unsigned char), 350u), sizeof( unsigned char), 1)).curr: 0){ base= 16; flags &=
~ 256; goto ok;} break; default: _LL130: break;} break; ok:*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& p, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char) c; _IO_getc(
fp); if( _IO_peekc( fp) ==  - 1){ seen_eof ++; break;}} if( flags &  128){ if( p.curr
> ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr){
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1); _IO_ungetc(( int)*((
unsigned char*) _check_unknown_subscript( p, sizeof( unsigned char), 0u)), fp);}
goto match_failure;} c=( int)*(( unsigned char*) _check_unknown_subscript( p,
sizeof( unsigned char), -1)); if( c == 'x'? 1: c == 'X'){
_tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1); _IO_ungetc( c, fp);}
if(( flags &  8) ==  0){ unsigned int res;*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u))='\000'; if(
use_strtoul){ res= Cyc_Stdlib_strtoul( _tag_arr( buf, sizeof( unsigned char),
350u), 0, base);} else{ res=( unsigned int) Cyc_Stdlib_strtol( _tag_arr( buf,
sizeof( unsigned char), 350u), 0, base);} if( flags &  16){* Cyc_Stdio_va_arg_int_ptr(*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) res;} else{
if( flags &  4){* Cyc_Stdio_va_arg_short_ptr(*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0u)))=( short) res;} else{ if(
flags &  1){* Cyc_Stdio_va_arg_int_ptr(*(( void**) _check_unknown_subscript( ap,
sizeof( void*), 0u)))=( int) res;} else{* Cyc_Stdio_va_arg_int_ptr(*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0u)))=( int) res;}}}
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); nassigned ++;} nread +=(
p.curr - ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr) /  sizeof(
unsigned char); break; case 4: _LL103: if( width ==  0? 1: width >  sizeof( buf)
-  1){ width=( long long)( sizeof( buf) -  1);} flags |=(( 64 |  128) |  256) | 
512; for( p= _tag_arr( buf, sizeof( unsigned char), 350u); width !=  0; width --){
c= _IO_peekc( fp); switch( c){ case '0': _LL133: goto _LL134; case '1': _LL134:
goto _LL135; case '2': _LL135: goto _LL136; case '3': _LL136: goto _LL137; case
'4': _LL137: goto _LL138; case '5': _LL138: goto _LL139; case '6': _LL139: goto
_LL140; case '7': _LL140: goto _LL141; case '8': _LL141: goto _LL142; case '9':
_LL142: flags &= ~( 64 |  128); goto fok; case '+': _LL143: goto _LL144; case '-':
_LL144: if( flags &  64){ flags &= ~ 64; goto fok;} break; case '.': _LL145: if(
flags &  256){ flags &= ~( 64 |  256); goto fok;} break; case 'e': _LL146: goto
_LL147; case 'E': _LL147: if(( flags & ( 128 |  512)) ==  512){ flags=( flags & 
~( 512 |  256) |  64) |  128; goto fok;} break; default: _LL148: break;} break;
fok:*(( unsigned char*) _check_unknown_subscript( _tagged_arr_inplace_plus_post(&
p, sizeof( unsigned char), 1), sizeof( unsigned char), 0u))=( unsigned char) c;
_IO_getc( fp); if( _IO_peekc( fp) ==  - 1){ seen_eof ++; break;}} if( flags & 
128){ if( flags &  512){ while( p.curr > ( _tag_arr( buf, sizeof( unsigned char),
350u)).curr) { _tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1);
_IO_ungetc(( int)*(( unsigned char*) _check_unknown_subscript( p, sizeof(
unsigned char), 0u)), fp);} goto match_failure;} _tagged_arr_inplace_plus(& p,
sizeof( unsigned char), -1); c=( int)*(( unsigned char*)
_check_unknown_subscript( p, sizeof( unsigned char), 0u)); if( c != 'e'? c != 'E':
0){ _IO_ungetc( c, fp); _tagged_arr_inplace_plus(& p, sizeof( unsigned char), -1);
c=( int)*(( unsigned char*) _check_unknown_subscript( p, sizeof( unsigned char),
0u));} _IO_ungetc( c, fp);} if(( flags &  8) ==  0){ double res;*((
unsigned char*) _check_unknown_subscript( p, sizeof( unsigned char), 0u))='\000';
res= Cyc_Stdlib_atof( _tag_arr( buf, sizeof( unsigned char), 350u)); if( flags & 
1){* Cyc_Stdio_va_arg_double_ptr(*(( void**) _check_unknown_subscript( ap,
sizeof( void*), 0u)))= res;} else{* Cyc_Stdio_va_arg_float_ptr(*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0u)))=( float) res;}
_tagged_arr_inplace_plus_post(& ap, sizeof( void*), 1); nassigned ++;} nread +=(
p.curr - ( _tag_arr( buf, sizeof( unsigned char), 350u)).curr) /  sizeof(
unsigned char); break; default: _LL132:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp151=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp151[ 0]=({ struct Cyc_Core_Impossible_struct _temp152; _temp152.tag= Cyc_Core_Impossible;
_temp152.f1= _tag_arr("scanf3", sizeof( unsigned char), 7u); _temp152;});
_temp151;}));}} eof_failure: seen_eof ++; input_failure: if( nassigned ==  0){
nassigned= - 1;} control_failure: match_failure: if(( unsigned int) errp){*((
int*) _check_null( errp)) |= 2;} done: if(( unsigned int) errp? seen_eof: 0){*((
int*) _check_null( errp)) |= 1;} return nassigned;}}} static struct _tagged_arr
Cyc_Stdio___sccl( struct _tagged_arr tab, struct _tagged_arr fmt){ int c; int n;
int v; c=( int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u)); if( c == '^'){ v= 1; c=( int)*(( const unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1), sizeof( unsigned char), 0u));} else{ v= 0;} for( n= 0; n < 
256; n ++){*(( unsigned char*) _check_unknown_subscript( tab, sizeof(
unsigned char), n))=( unsigned char) v;} if( c ==  0){ return _tagged_arr_plus(
fmt, sizeof( unsigned char), - 1);} v= 1 -  v; for( 0; 1; 0){*(( unsigned char*)
_check_unknown_subscript( tab, sizeof( unsigned char), c))=( unsigned char) v;
doswitch: n=( int)*(( const unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt, sizeof( unsigned char), 1), sizeof(
unsigned char), 0u)); switch( n){ case 0: _LL155: return _tagged_arr_plus( fmt,
sizeof( unsigned char), - 1); case '-': _LL156: n=( int)*(( const unsigned char*)
_check_unknown_subscript( fmt, sizeof( unsigned char), 0u)); if( n == ']'? 1: n
<  c){ c=( int)'-'; break;} _tagged_arr_inplace_plus_post(& fmt, sizeof(
unsigned char), 1); do {*(( unsigned char*) _check_unknown_subscript( tab,
sizeof( unsigned char), ++ c))=( unsigned char) v;} while ( c <  n); goto
doswitch; break; case ']': _LL157: return fmt; default: _LL158: c= n; break;}}}
static int Cyc_Stdio_string_getc( struct _tagged_arr* sptr){ unsigned char c;
struct _tagged_arr s=* sptr; if(( s.curr == (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr? 1: _get_arr_size( s, sizeof( unsigned char)) ==  0)? 1:( c=*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0)))
== '\000'){ return - 1;}* sptr= _tagged_arr_plus( s, sizeof( unsigned char), 1);
return( int) c;} static int Cyc_Stdio_string_ungetc( int ignore, struct
_tagged_arr* sptr){* sptr= _tagged_arr_plus(* sptr, sizeof( unsigned char), - 1);
return 0;} static int Cyc_Stdio_string_peekc( struct _tagged_arr* sptr){
unsigned char c; struct _tagged_arr s=* sptr; if(( s.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: _get_arr_size( s, sizeof(
unsigned char)) ==  0)? 1:( c=*(( const unsigned char*) _check_unknown_subscript(
s, sizeof( unsigned char), 0))) == '\000'){ return - 1;} return( int) c;} int
Cyc_Stdio_vsscanf( struct _tagged_arr src, struct _tagged_arr fmt, struct
_tagged_arr ap){ int err= 0; int(* sgc)( struct _tagged_arr*)= Cyc_Stdio_string_getc;
return(( int(*)( int(* _IO_getc)( struct _tagged_arr*), int(* _IO_ungetc)( int,
struct _tagged_arr*), int(* _IO_peekc)( struct _tagged_arr*), struct _tagged_arr*
fp, struct _tagged_arr fmt0, struct _tagged_arr ap, int* errp)) Cyc_Stdio__IO_vfscanf)(
Cyc_Stdio_string_getc, Cyc_Stdio_string_ungetc, Cyc_Stdio_string_peekc,& src,
fmt, ap,( int*)& err);} int Cyc_Stdio_sscanf( struct _tagged_arr src, struct
_tagged_arr fmt, struct _tagged_arr ap){ return Cyc_Stdio_vsscanf( src, fmt, ap);}
int Cyc_Stdio_peekc( struct Cyc_Stdio___sFILE* stream){ int c= Cyc_Stdio_fgetc(
stream); Cyc_Stdio_ungetc( c, stream); return c;} int Cyc_Stdio_vfscanf( struct
Cyc_Stdio___sFILE* stream, struct _tagged_arr fmt, struct _tagged_arr ap){ int
err= 0; return(( int(*)( int(* _IO_getc)( struct Cyc_Stdio___sFILE*), int(*
_IO_ungetc)( int, struct Cyc_Stdio___sFILE*), int(* _IO_peekc)( struct Cyc_Stdio___sFILE*),
struct Cyc_Stdio___sFILE* fp, struct _tagged_arr fmt0, struct _tagged_arr ap,
int* errp)) Cyc_Stdio__IO_vfscanf)( Cyc_Stdio_getc, Cyc_Stdio_ungetc, Cyc_Stdio_peekc,
stream, fmt, ap,( int*)& err);} int Cyc_Stdio_fscanf( struct Cyc_Stdio___sFILE*
stream, struct _tagged_arr fmt, struct _tagged_arr ap){ return Cyc_Stdio_vfscanf(
stream, fmt, ap);} int Cyc_Stdio_scanf( struct _tagged_arr fmt, struct
_tagged_arr ap){ return Cyc_Stdio_vfscanf( Cyc_Stdio_stdin, fmt, ap);}
