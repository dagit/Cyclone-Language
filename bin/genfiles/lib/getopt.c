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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern struct
Cyc_Stdio___sFILE* Cyc_Stdio_stderr; extern unsigned char Cyc_Stdio_FileCloseError[
19u]; extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_String_pa=
0; struct Cyc_Stdio_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_Int_pa= 1; struct Cyc_Stdio_Int_pa_struct{ int tag;
unsigned int f1; } ; static const int Cyc_Stdio_Double_pa= 2; struct Cyc_Stdio_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Stdio_fprintf( struct Cyc_Stdio___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Stdio_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Stdio_ShortPtr_sa=
0; struct Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Stdio_UShortPtr_sa= 1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Stdlib__Div{ int quot; int rem; } ; struct
Cyc_Stdlib__Ldiv{ int quot; int rem; } ; extern int __mb_cur_max; extern int abs(
int); extern int atexit( void(* __func)()); extern struct Cyc_Stdlib__Div div(
int __numer, int __denom); extern struct Cyc_Stdlib__Ldiv ldiv( int __numer, int
__denom); extern int rand(); extern void srand( unsigned int __seed); extern int
rand_r( unsigned int* __seed); extern struct _tagged_arr Cyc_Stdlib_getenv(
struct _tagged_arr); extern struct _tagged_arr Cyc_Getopt_optarg; extern int Cyc_Getopt_optind;
extern int Cyc_Getopt_opterr; extern int Cyc_Getopt_optopt; struct Cyc_Getopt_option{
struct _tagged_arr name; int has_arg; int* flag; int val; } ; extern int Cyc_Getopt_getopt(
int argc, struct _tagged_arr argv, struct _tagged_arr shortopts); extern int Cyc_Getopt__getopt_internal(
int argc, struct _tagged_arr argv, struct _tagged_arr shortopts, struct
_tagged_arr longopts, int* longind, int long_only); struct _tagged_arr Cyc_Getopt_optarg={(
void*) 0u,( void*) 0u,( void*) 0u +  0u}; int Cyc_Getopt_optind= 0; static
struct _tagged_arr Cyc_Getopt_nextchar; int Cyc_Getopt_opterr= 1; int Cyc_Getopt_optopt=(
int)'?'; enum  Cyc_Getopt_ordering_tag{ Cyc_Getopt_REQUIRE_ORDER  =  0u, Cyc_Getopt_PERMUTE
 =  1u, Cyc_Getopt_RETURN_IN_ORDER  =  2u}; static enum  Cyc_Getopt_ordering_tag
Cyc_Getopt_ordering; static struct _tagged_arr Cyc_Getopt_posixly_correct;
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_String_strlen(
struct _tagged_arr s); extern int Cyc_String_strcmp( struct _tagged_arr s1,
struct _tagged_arr s2); extern int Cyc_String_strncmp( struct _tagged_arr s1,
struct _tagged_arr s2, int len); extern struct _tagged_arr Cyc_String_strchr(
struct _tagged_arr s, unsigned char c); static int Cyc_Getopt_first_nonopt;
static int Cyc_Getopt_last_nonopt; static struct _tagged_arr Cyc_Getopt_nonoption_flags;
static int Cyc_Getopt_nonoption_flags_len; static void Cyc_Getopt_exchange(
struct _tagged_arr); static void Cyc_Getopt_exchange( struct _tagged_arr argv){
int bottom= Cyc_Getopt_first_nonopt; int middle= Cyc_Getopt_last_nonopt; int top=
Cyc_Getopt_optind; struct _tagged_arr tem; while( top >  middle? middle > 
bottom: 0) { if( top -  middle >  middle -  bottom){ int len= middle -  bottom;
int i; for( i= 0; i <  len; i ++){ tem=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), bottom +  i));*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
bottom +  i))=*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr),( top - ( middle -  bottom)) +  i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr),( top - ( middle - 
bottom)) +  i))= tem;} top -= len;} else{ int len= top -  middle; int i; for( i=
0; i <  len; i ++){ tem=*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), bottom +  i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), bottom +  i))=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
middle +  i));*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), middle +  i))= tem;} bottom += len;}} Cyc_Getopt_first_nonopt
+= Cyc_Getopt_optind -  Cyc_Getopt_last_nonopt; Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;}
static struct _tagged_arr Cyc_Getopt__getopt_initialize( struct _tagged_arr);
static struct _tagged_arr Cyc_Getopt__getopt_initialize( struct _tagged_arr
optstring){ Cyc_Getopt_first_nonopt=( Cyc_Getopt_last_nonopt=( Cyc_Getopt_optind=
1)); Cyc_Getopt_nextchar= _tag_arr( 0u, 0u, 0u); Cyc_Getopt_posixly_correct= Cyc_Stdlib_getenv(
_tag_arr("POSIXLY_CORRECT", sizeof( unsigned char), 16u)); if(*(( const
unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char), 0))
== '-'){ Cyc_Getopt_ordering= Cyc_Getopt_RETURN_IN_ORDER;
_tagged_arr_inplace_plus(& optstring, sizeof( unsigned char), 1);} else{ if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == '+'){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;
_tagged_arr_inplace_plus(& optstring, sizeof( unsigned char), 1);} else{ if( Cyc_Getopt_posixly_correct.curr
!= ( _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Getopt_ordering= Cyc_Getopt_REQUIRE_ORDER;}
else{ Cyc_Getopt_ordering= Cyc_Getopt_PERMUTE;}}} if( Cyc_Getopt_posixly_correct.curr
== ( _tag_arr( 0u, 0u, 0u)).curr){ struct _tagged_arr var; var=({ struct Cyc_Stdio_Int_pa_struct
_temp1; _temp1.tag= Cyc_Stdio_Int_pa; _temp1.f1=( int)(( unsigned int) 10);{
void* _temp0[ 1u]={& _temp1}; Cyc_Stdio_aprintf( _tag_arr("_%d_GNU_nonoption_argv_flags_",
sizeof( unsigned char), 30u), _tag_arr( _temp0, sizeof( void*), 1u));}}); Cyc_Getopt_nonoption_flags=
Cyc_Stdlib_getenv(( struct _tagged_arr) var); if( Cyc_Getopt_nonoption_flags.curr
== ( _tag_arr( 0u, 0u, 0u)).curr){ Cyc_Getopt_nonoption_flags_len= 0;} else{ Cyc_Getopt_nonoption_flags_len=
Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nonoption_flags);}} return
optstring;} int Cyc_Getopt__getopt_internal( int argc, struct _tagged_arr argv,
struct _tagged_arr optstring, struct _tagged_arr longopts, int* longind, int
long_only){ Cyc_Getopt_optarg= _tag_arr( 0u, 0u, 0u); if( Cyc_Getopt_optind == 
0){ optstring= Cyc_Getopt__getopt_initialize( optstring); Cyc_Getopt_optind= 1;}
if( Cyc_Getopt_nextchar.curr == ( _tag_arr( 0u, 0u, 0u)).curr? 1:*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nextchar, sizeof(
unsigned char), 0u)) == '\000'){ if( Cyc_Getopt_last_nonopt >  Cyc_Getopt_optind){
Cyc_Getopt_last_nonopt= Cyc_Getopt_optind;} if( Cyc_Getopt_first_nonopt >  Cyc_Getopt_optind){
Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;} if( Cyc_Getopt_ordering ==  Cyc_Getopt_PERMUTE){
if( Cyc_Getopt_first_nonopt !=  Cyc_Getopt_last_nonopt? Cyc_Getopt_last_nonopt
!=  Cyc_Getopt_optind: 0){ Cyc_Getopt_exchange(( struct _tagged_arr) argv);}
else{ if( Cyc_Getopt_last_nonopt !=  Cyc_Getopt_optind){ Cyc_Getopt_first_nonopt=
Cyc_Getopt_optind;}} while( Cyc_Getopt_optind <  argc?(*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 0)) != '-'?
1:*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) == '\000')? 1:( Cyc_Getopt_optind <  Cyc_Getopt_nonoption_flags_len?*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nonoption_flags, sizeof(
unsigned char), Cyc_Getopt_optind)) == '1': 0): 0) { Cyc_Getopt_optind ++;} Cyc_Getopt_last_nonopt=
Cyc_Getopt_optind;} if( Cyc_Getopt_optind !=  argc? ! Cyc_String_strcmp(( struct
_tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), Cyc_Getopt_optind)), _tag_arr("--", sizeof( unsigned char),
3u)): 0){ Cyc_Getopt_optind ++; if( Cyc_Getopt_first_nonopt !=  Cyc_Getopt_last_nonopt?
Cyc_Getopt_last_nonopt !=  Cyc_Getopt_optind: 0){ Cyc_Getopt_exchange(( struct
_tagged_arr) argv);} else{ if( Cyc_Getopt_first_nonopt ==  Cyc_Getopt_last_nonopt){
Cyc_Getopt_first_nonopt= Cyc_Getopt_optind;}} Cyc_Getopt_last_nonopt= argc; Cyc_Getopt_optind=
argc;} if( Cyc_Getopt_optind ==  argc){ if( Cyc_Getopt_first_nonopt !=  Cyc_Getopt_last_nonopt){
Cyc_Getopt_optind= Cyc_Getopt_first_nonopt;} return - 1;} if((*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 0)) != '-'?
1:*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) == '\000')? 1:( Cyc_Getopt_optind <  Cyc_Getopt_nonoption_flags_len?*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nonoption_flags, sizeof(
unsigned char), Cyc_Getopt_optind)) == '1': 0)){ if( Cyc_Getopt_ordering ==  Cyc_Getopt_REQUIRE_ORDER){
return - 1;} Cyc_Getopt_optarg=*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind ++)); return 1;} Cyc_Getopt_nextchar=
_tagged_arr_plus( _tagged_arr_plus(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1), sizeof( unsigned char),( longopts.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 1)) == '-':
0)? 1: 0);} if( longopts.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) == '-'? 1:( long_only?( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 2))? 1:
!(( unsigned int)( Cyc_String_strchr( optstring,*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Getopt_optind)), sizeof( unsigned char), 1)))).curr):
0): 0){ struct _tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr
pfound=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0;
int indfound= 0; int option_index; for( nameend= Cyc_Getopt_nextchar;( int)*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))?*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))
!= '=': 0; _tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;}
for(( p= longopts, option_index= 0);( unsigned int)((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_Getopt_option), 1),
option_index ++)){ if( ! Cyc_String_strncmp((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name,(
struct _tagged_arr) Cyc_Getopt_nextchar,( nameend.curr -  Cyc_Getopt_nextchar.curr)
/  sizeof( unsigned char))){ if(( nameend.curr -  Cyc_Getopt_nextchar.curr) / 
sizeof( unsigned char) ==  Cyc_String_strlen((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Getopt_option), 0u))->name)){
pfound= p; indfound= option_index; exact= 1; break;} else{ if( pfound.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= p; indfound=
option_index;} else{ ambig= 1;}}}} if( ambig? ! exact: 0){ if( Cyc_Getopt_opterr){({
struct Cyc_Stdio_String_pa_struct _temp4; _temp4.tag= Cyc_Stdio_String_pa;
_temp4.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind));{ struct Cyc_Stdio_String_pa_struct
_temp3; _temp3.tag= Cyc_Stdio_String_pa; _temp3.f1=( struct _tagged_arr)*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
0));{ void* _temp2[ 2u]={& _temp3,& _temp4}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: option `%s' is ambiguous\n", sizeof( unsigned char), 30u),
_tag_arr( _temp2, sizeof( void*), 2u));}}});} _tagged_arr_inplace_plus(& Cyc_Getopt_nextchar,
sizeof( unsigned char), Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nextchar));
Cyc_Getopt_optind ++; Cyc_Getopt_optopt= 0; return( int)'?';} if( pfound.curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ option_index= indfound; Cyc_Getopt_optind
++; if(( int)*(( unsigned char*) _check_unknown_subscript( nameend, sizeof(
unsigned char), 0u))){ if((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->has_arg){
Cyc_Getopt_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char), 1);} else{
if( Cyc_Getopt_opterr){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Getopt_optind -  1)), sizeof( unsigned char), 1)) == '-'){({ struct Cyc_Stdio_String_pa_struct
_temp7; _temp7.tag= Cyc_Stdio_String_pa; _temp7.f1=( struct _tagged_arr)(( const
struct Cyc_Getopt_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option),
0u))->name;{ struct Cyc_Stdio_String_pa_struct _temp6; _temp6.tag= Cyc_Stdio_String_pa;
_temp6.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), 0));{ void* _temp5[ 2u]={& _temp6,& _temp7};
Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: option `--%s' doesn't allow an argument\n",
sizeof( unsigned char), 45u), _tag_arr( _temp5, sizeof( void*), 2u));}}});}
else{({ struct Cyc_Stdio_String_pa_struct _temp11; _temp11.tag= Cyc_Stdio_String_pa;
_temp11.f1=( struct _tagged_arr)(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->name;{
struct Cyc_Stdio_Int_pa_struct _temp10; _temp10.tag= Cyc_Stdio_Int_pa; _temp10.f1=(
int)(( unsigned int)(( int)*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Getopt_optind -  1)), sizeof( unsigned char), 0))));{ struct Cyc_Stdio_String_pa_struct
_temp9; _temp9.tag= Cyc_Stdio_String_pa; _temp9.f1=( struct _tagged_arr)*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
0));{ void* _temp8[ 3u]={& _temp9,& _temp10,& _temp11}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: option `%c%s' doesn't allow an argument\n", sizeof( unsigned char),
45u), _tag_arr( _temp8, sizeof( void*), 3u));}}}});}} _tagged_arr_inplace_plus(&
Cyc_Getopt_nextchar, sizeof( unsigned char), Cyc_String_strlen(( struct
_tagged_arr) Cyc_Getopt_nextchar)); Cyc_Getopt_optopt=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)'?';}} else{ if((( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->has_arg
==  1){ if( Cyc_Getopt_optind <  argc){ Cyc_Getopt_optarg=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind
++));} else{ if( Cyc_Getopt_opterr){({ struct Cyc_Stdio_String_pa_struct _temp14;
_temp14.tag= Cyc_Stdio_String_pa; _temp14.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind
-  1));{ struct Cyc_Stdio_String_pa_struct _temp13; _temp13.tag= Cyc_Stdio_String_pa;
_temp13.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp12[
2u]={& _temp13,& _temp14}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: option `%s' requires an argument\n",
sizeof( unsigned char), 38u), _tag_arr( _temp12, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_Getopt_nextchar, sizeof( unsigned char), Cyc_String_strlen((
struct _tagged_arr) Cyc_Getopt_nextchar)); Cyc_Getopt_optopt=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return( int)(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'?':':'?');}}} _tagged_arr_inplace_plus(& Cyc_Getopt_nextchar,
sizeof( unsigned char), Cyc_String_strlen(( struct _tagged_arr) Cyc_Getopt_nextchar));
if( longind !=  0){*(( int*) _check_null( longind))= option_index;} if((
unsigned int)(( const struct Cyc_Getopt_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->flag){*(( int*) _check_null(((
const struct Cyc_Getopt_option*) _check_unknown_subscript( pfound, sizeof(
struct Cyc_Getopt_option), 0u))->flag))=(( const struct Cyc_Getopt_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;
return 0;} return(( const struct Cyc_Getopt_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Getopt_option), 0u))->val;} if(( ! long_only? 1:*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 1)) == '-')? 1:( Cyc_String_strchr( optstring,*((
unsigned char*) _check_unknown_subscript( Cyc_Getopt_nextchar, sizeof(
unsigned char), 0u)))).curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( Cyc_Getopt_opterr){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Getopt_optind)), sizeof( unsigned char), 1)) == '-'){({ struct Cyc_Stdio_String_pa_struct
_temp17; _temp17.tag= Cyc_Stdio_String_pa; _temp17.f1=( struct _tagged_arr) Cyc_Getopt_nextchar;{
struct Cyc_Stdio_String_pa_struct _temp16; _temp16.tag= Cyc_Stdio_String_pa;
_temp16.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp15[
2u]={& _temp16,& _temp17}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: unrecognized option `--%s'\n",
sizeof( unsigned char), 32u), _tag_arr( _temp15, sizeof( void*), 2u));}}});}
else{({ struct Cyc_Stdio_String_pa_struct _temp21; _temp21.tag= Cyc_Stdio_String_pa;
_temp21.f1=( struct _tagged_arr) Cyc_Getopt_nextchar;{ struct Cyc_Stdio_Int_pa_struct
_temp20; _temp20.tag= Cyc_Stdio_Int_pa; _temp20.f1=( int)(( unsigned int)(( int)*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind)),
sizeof( unsigned char), 0))));{ struct Cyc_Stdio_String_pa_struct _temp19;
_temp19.tag= Cyc_Stdio_String_pa; _temp19.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{
void* _temp18[ 3u]={& _temp19,& _temp20,& _temp21}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: unrecognized option `%c%s'\n", sizeof( unsigned char), 32u),
_tag_arr( _temp18, sizeof( void*), 3u));}}}});}} Cyc_Getopt_nextchar= _tag_arr(({
unsigned char* _temp22=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char));
_temp22[ 0]='\000'; _temp22;}), sizeof( unsigned char), 1u); Cyc_Getopt_optind
++; Cyc_Getopt_optopt= 0; return( int)'?';}}{ unsigned char c=*(( unsigned char*)
_check_unknown_subscript( _tagged_arr_inplace_plus_post(& Cyc_Getopt_nextchar,
sizeof( unsigned char), 1), sizeof( unsigned char), 0u)); struct _tagged_arr
temp= Cyc_String_strchr( optstring, c); if(*(( unsigned char*)
_check_unknown_subscript( Cyc_Getopt_nextchar, sizeof( unsigned char), 0u)) == '\000'){
++ Cyc_Getopt_optind;} if( temp.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u,
0u)).curr? 1: c == ':'){ if( Cyc_Getopt_opterr){ if(( unsigned int) Cyc_Getopt_posixly_correct.curr){({
struct Cyc_Stdio_Int_pa_struct _temp25; _temp25.tag= Cyc_Stdio_Int_pa; _temp25.f1=(
int)(( unsigned int)(( int) c));{ struct Cyc_Stdio_String_pa_struct _temp24;
_temp24.tag= Cyc_Stdio_String_pa; _temp24.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{
void* _temp23[ 2u]={& _temp24,& _temp25}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: illegal option -- %c\n", sizeof( unsigned char), 26u), _tag_arr(
_temp23, sizeof( void*), 2u));}}});} else{({ struct Cyc_Stdio_Int_pa_struct
_temp28; _temp28.tag= Cyc_Stdio_Int_pa; _temp28.f1=( int)(( unsigned int)(( int)
c));{ struct Cyc_Stdio_String_pa_struct _temp27; _temp27.tag= Cyc_Stdio_String_pa;
_temp27.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp26[
2u]={& _temp27,& _temp28}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr, _tag_arr("%s: invalid option -- %c\n",
sizeof( unsigned char), 26u), _tag_arr( _temp26, sizeof( void*), 2u));}}});}}
Cyc_Getopt_optopt=( int) c; return( int)'?';} if(*(( const unsigned char*)
_check_unknown_subscript( temp, sizeof( unsigned char), 1)) == ':'){ if(*((
const unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char), 2))
== ':'){ if(*(( unsigned char*) _check_unknown_subscript( Cyc_Getopt_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_Getopt_optarg= Cyc_Getopt_nextchar;
Cyc_Getopt_optind ++;} else{ Cyc_Getopt_optarg= _tag_arr( 0u, 0u, 0u);} Cyc_Getopt_nextchar=
_tag_arr( 0u, 0u, 0u);} else{ if(*(( unsigned char*) _check_unknown_subscript(
Cyc_Getopt_nextchar, sizeof( unsigned char), 0u)) != '\000'){ Cyc_Getopt_optarg=
Cyc_Getopt_nextchar; Cyc_Getopt_optind ++;} else{ if( Cyc_Getopt_optind ==  argc){
if( Cyc_Getopt_opterr){({ struct Cyc_Stdio_Int_pa_struct _temp31; _temp31.tag=
Cyc_Stdio_Int_pa; _temp31.f1=( int)(( unsigned int)(( int) c));{ struct Cyc_Stdio_String_pa_struct
_temp30; _temp30.tag= Cyc_Stdio_String_pa; _temp30.f1=( struct _tagged_arr)*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
0));{ void* _temp29[ 2u]={& _temp30,& _temp31}; Cyc_Stdio_fprintf( Cyc_Stdio_stderr,
_tag_arr("%s: option requires an argument -- %c\n", sizeof( unsigned char), 39u),
_tag_arr( _temp29, sizeof( void*), 2u));}}});} Cyc_Getopt_optopt=( int) c; if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == ':'){ c=':';} else{ c='?';}} else{ Cyc_Getopt_optarg=*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Getopt_optind
++));}} Cyc_Getopt_nextchar= _tag_arr( 0u, 0u, 0u);}} return( int) c;}} int Cyc_Getopt_getopt(
int argc, struct _tagged_arr argv, struct _tagged_arr optstring){ return Cyc_Getopt__getopt_internal(
argc, argv, optstring,( struct _tagged_arr) _tag_arr( 0u, 0u, 0u),( int*) 0, 0);}
