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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_std___sFILE;
extern struct Cyc_std___sFILE* Cyc_std_stderr; extern unsigned char Cyc_std_FileCloseError[
19u]; extern unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa=
0; struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int system( unsigned char*); struct Cyc_std__Div{
int quot; int rem; } ; struct Cyc_std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_std__Div
div( int __numer, int __denom); extern struct Cyc_std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern struct _tagged_arr Cyc_std_getenv( struct _tagged_arr); struct Cyc_std_timeval{
int tv_sec; int tv_usec; } ; extern struct _tagged_arr Cyc_std_optarg; extern
int Cyc_std_optind; extern int Cyc_std_opterr; extern int Cyc_std_optopt; struct
Cyc_std_option{ struct _tagged_arr name; int has_arg; int* flag; int val; } ;
extern int Cyc_std_getopt( int __argc, struct _tagged_arr __argv, struct
_tagged_arr __shortopts); extern int Cyc_std__getopt_internal( int __argc,
struct _tagged_arr __argv, struct _tagged_arr __shortopts, struct _tagged_arr
__longopts, int* __longind, int __long_only); extern unsigned int alarm(
unsigned int seconds); extern int close( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int dup( int);
extern int dup2( int, int); extern unsigned int getuid(); extern int setuid(
unsigned int uid); extern unsigned int geteuid(); extern int seteuid(
unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int pipe( int* filedes); extern int lseek( int
filedes, int offset, int whence); int Cyc_std_chdir( struct _tagged_arr); struct
_tagged_arr Cyc_std_getcwd( struct _tagged_arr buf, unsigned int size); int Cyc_std_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_std_execlp( struct _tagged_arr file, struct _tagged_arr arg0, struct
_tagged_arr argv); int Cyc_std_execve( struct _tagged_arr filename, struct
_tagged_arr argv, struct _tagged_arr envp); int Cyc_std_read( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_std_write( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_std_unlink( struct _tagged_arr
pathname); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_std_strlen(
struct _tagged_arr s); extern int Cyc_std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_std_strncmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int len); extern struct _tagged_arr Cyc_std_strchr(
struct _tagged_arr s, unsigned char c); struct _tagged_arr Cyc_std_optarg; int
Cyc_std_optind= 1; int Cyc_std___getopt_initialized; static struct _tagged_arr
Cyc_std_nextchar; int Cyc_std_opterr= 1; int Cyc_std_optopt=( int)'?'; enum  Cyc_std_ordering_tag{
Cyc_std_REQUIRE_ORDER  =  0u, Cyc_std_PERMUTE  =  1u, Cyc_std_RETURN_IN_ORDER
 =  2u}; static enum  Cyc_std_ordering_tag Cyc_std_ordering; static struct
_tagged_arr Cyc_std_posixly_correct; static int Cyc_std_first_nonopt; static int
Cyc_std_last_nonopt; static int Cyc_std_nonoption_flags_max_len; static int Cyc_std_nonoption_flags_len;
static int Cyc_std_original_argc; static int Cyc_std_original_argv; static void
 __attribute__(( unused ))  Cyc_std_store_args_and_env( int argc, struct
_tagged_arr argv){ Cyc_std_original_argc= argc; Cyc_std_original_argv=( int)
argv.curr;} static void Cyc_std_exchange( struct _tagged_arr argv){ int bottom=
Cyc_std_first_nonopt; int middle= Cyc_std_last_nonopt; int top= Cyc_std_optind;
struct _tagged_arr tem; while( top >  middle? middle >  bottom: 0) { if( top - 
middle >  middle -  bottom){ int len= middle -  bottom; int i; for( i= 0; i < 
len; i ++){ tem=*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), bottom +  i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), bottom +  i))=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
top - ( middle -  bottom)) +  i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr),( top - ( middle - 
bottom)) +  i))= tem;;} top -= len;} else{ int len= top -  middle; int i; for( i=
0; i <  len; i ++){ tem=*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), bottom +  i));*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), bottom +  i))=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
middle +  i));*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof(
struct _tagged_arr), middle +  i))= tem;;} bottom += len;}} Cyc_std_first_nonopt
+= Cyc_std_optind -  Cyc_std_last_nonopt; Cyc_std_last_nonopt= Cyc_std_optind;}
static struct _tagged_arr Cyc_std__getopt_initialize( int argc, struct
_tagged_arr argv, struct _tagged_arr optstring){ Cyc_std_first_nonopt=( Cyc_std_last_nonopt=
Cyc_std_optind); Cyc_std_nextchar= _tag_arr( 0u, 0u, 0u); Cyc_std_posixly_correct=
Cyc_std_getenv( _tag_arr("POSIXLY_CORRECT", sizeof( unsigned char), 16u)); if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == '-'){ Cyc_std_ordering= Cyc_std_RETURN_IN_ORDER; _tagged_arr_inplace_plus(&
optstring, sizeof( unsigned char), 1);} else{ if(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) == '+'){ Cyc_std_ordering=
Cyc_std_REQUIRE_ORDER; _tagged_arr_inplace_plus(& optstring, sizeof(
unsigned char), 1);} else{ if( Cyc_std_posixly_correct.curr != ( _tag_arr( 0u, 0u,
0u)).curr){ Cyc_std_ordering= Cyc_std_REQUIRE_ORDER;} else{ Cyc_std_ordering=
Cyc_std_PERMUTE;}}} return optstring;} int Cyc_std__getopt_internal( int argc,
struct _tagged_arr argv, struct _tagged_arr optstring, struct _tagged_arr
longopts, int* longind, int long_only){ int print_errors= Cyc_std_opterr; if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == ':'){ print_errors= 0;} if( argc <  1){ return - 1;} Cyc_std_optarg=
_tag_arr( 0u, 0u, 0u); if( Cyc_std_optind ==  0? 1: ! Cyc_std___getopt_initialized){
if( Cyc_std_optind ==  0){ Cyc_std_optind= 1;} optstring= Cyc_std__getopt_initialize(
argc, argv, optstring); Cyc_std___getopt_initialized= 1;} if( Cyc_std_nextchar.curr
== ( _tag_arr( 0u, 0u, 0u)).curr? 1:*(( unsigned char*) _check_unknown_subscript(
Cyc_std_nextchar, sizeof( unsigned char), 0u)) == '\000'){ if( Cyc_std_last_nonopt
>  Cyc_std_optind){ Cyc_std_last_nonopt= Cyc_std_optind;} if( Cyc_std_first_nonopt
>  Cyc_std_optind){ Cyc_std_first_nonopt= Cyc_std_optind;} if( Cyc_std_ordering
==  Cyc_std_PERMUTE){ if( Cyc_std_first_nonopt !=  Cyc_std_last_nonopt? Cyc_std_last_nonopt
!=  Cyc_std_optind: 0){ Cyc_std_exchange( argv);} else{ if( Cyc_std_last_nonopt
!=  Cyc_std_optind){ Cyc_std_first_nonopt= Cyc_std_optind;}} while( Cyc_std_optind
<  argc?*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 0)) != '-'? 1:*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind)), sizeof( unsigned char), 1)) == '\000':
0) { Cyc_std_optind ++;} Cyc_std_last_nonopt= Cyc_std_optind;} if( Cyc_std_optind
!=  argc? ! Cyc_std_strcmp(( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
_tag_arr("--", sizeof( unsigned char), 3u)): 0){ Cyc_std_optind ++; if( Cyc_std_first_nonopt
!=  Cyc_std_last_nonopt? Cyc_std_last_nonopt !=  Cyc_std_optind: 0){ Cyc_std_exchange(
argv);} else{ if( Cyc_std_first_nonopt ==  Cyc_std_last_nonopt){ Cyc_std_first_nonopt=
Cyc_std_optind;}} Cyc_std_last_nonopt= argc; Cyc_std_optind= argc;} if( Cyc_std_optind
==  argc){ if( Cyc_std_first_nonopt !=  Cyc_std_last_nonopt){ Cyc_std_optind=
Cyc_std_first_nonopt;} return - 1;} if(*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind)), sizeof( unsigned char), 0)) != '-'?
1:*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 1)) == '\000'){ if( Cyc_std_ordering ==  Cyc_std_REQUIRE_ORDER){
return - 1;} Cyc_std_optarg=*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), Cyc_std_optind ++)); return 1;} Cyc_std_nextchar=
_tagged_arr_plus( _tagged_arr_plus(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 1), sizeof( unsigned char), longopts.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind)), sizeof( unsigned char), 1)) == '-':
0);} if( longopts.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 1)) == '-'? 1:( long_only?( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind)), sizeof( unsigned char), 2))? 1: !((
unsigned int)( Cyc_std_strchr( optstring,*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind)), sizeof( unsigned char), 1)))).curr):
0): 0){ struct _tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr
pfound=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0;
int indfound= - 1; int option_index; for( nameend= Cyc_std_nextchar;( int)*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))?*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))
!= '=': 0; _tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;}
for(( p= longopts, option_index= 0);( unsigned int)((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_std_option), 1),
option_index ++)){ if( ! Cyc_std_strncmp((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name,( struct
_tagged_arr) Cyc_std_nextchar,( unsigned int)(( nameend.curr -  Cyc_std_nextchar.curr)
/  sizeof( unsigned char)))){ if(( unsigned int)(( nameend.curr -  Cyc_std_nextchar.curr)
/  sizeof( unsigned char)) == ( unsigned int) Cyc_std_strlen((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name)){
pfound= p; indfound= option_index; exact= 1; break;} else{ if( pfound.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= p; indfound=
option_index;} else{ if((( long_only? 1:(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->has_arg
!= (( const struct Cyc_std_option*) _check_unknown_subscript( p, sizeof( struct
Cyc_std_option), 0u))->has_arg)? 1:(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->flag != ((
const struct Cyc_std_option*) _check_unknown_subscript( p, sizeof( struct Cyc_std_option),
0u))->flag)? 1:(( const struct Cyc_std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_std_option), 0u))->val != (( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->val){ ambig=
1;}}}}} if( ambig? ! exact: 0){ if( print_errors){({ struct Cyc_std_String_pa_struct
_temp2; _temp2.tag= Cyc_std_String_pa; _temp2.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind));{
struct Cyc_std_String_pa_struct _temp1; _temp1.tag= Cyc_std_String_pa; _temp1.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp0[ 2u]={& _temp1,& _temp2}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option `%s' is ambiguous\n", sizeof( unsigned char),
30u), _tag_arr( _temp0, sizeof( void*), 2u));}}});} _tagged_arr_inplace_plus(&
Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen(( struct
_tagged_arr) Cyc_std_nextchar)); Cyc_std_optind ++; Cyc_std_optopt= 0; return(
int)'?';} if( pfound.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
option_index= indfound; Cyc_std_optind ++; if(( int)*(( unsigned char*)
_check_unknown_subscript( nameend, sizeof( unsigned char), 0u))){ if((( const
struct Cyc_std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_std_option),
0u))->has_arg){ Cyc_std_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char),
1);} else{ if( print_errors){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_std_optind -  1)), sizeof( unsigned char), 1)) == '-'){({ struct Cyc_std_String_pa_struct
_temp5; _temp5.tag= Cyc_std_String_pa; _temp5.f1=( struct _tagged_arr)(( const
struct Cyc_std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_std_option),
0u))->name;{ struct Cyc_std_String_pa_struct _temp4; _temp4.tag= Cyc_std_String_pa;
_temp4.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), 0));{ void* _temp3[ 2u]={& _temp4,& _temp5};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s: option `--%s' doesn't allow an argument\n",
sizeof( unsigned char), 45u), _tag_arr( _temp3, sizeof( void*), 2u));}}});}
else{({ struct Cyc_std_String_pa_struct _temp9; _temp9.tag= Cyc_std_String_pa;
_temp9.f1=( struct _tagged_arr)(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->name;{
struct Cyc_std_Int_pa_struct _temp8; _temp8.tag= Cyc_std_Int_pa; _temp8.f1=( int)((
unsigned int)(( int)*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind
-  1)), sizeof( unsigned char), 0))));{ struct Cyc_std_String_pa_struct _temp7;
_temp7.tag= Cyc_std_String_pa; _temp7.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{
void* _temp6[ 3u]={& _temp7,& _temp8,& _temp9}; Cyc_std_fprintf( Cyc_std_stderr,
_tag_arr("%s: option `%c%s' doesn't allow an argument\n", sizeof( unsigned char),
45u), _tag_arr( _temp6, sizeof( void*), 3u));}}}});}} _tagged_arr_inplace_plus(&
Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen(( struct
_tagged_arr) Cyc_std_nextchar)); Cyc_std_optopt=(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->val;
return( int)'?';}} else{ if((( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->has_arg
==  1){ if( Cyc_std_optind <  argc){ Cyc_std_optarg=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind ++));}
else{ if( print_errors){({ struct Cyc_std_String_pa_struct _temp12; _temp12.tag=
Cyc_std_String_pa; _temp12.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind -  1));{
struct Cyc_std_String_pa_struct _temp11; _temp11.tag= Cyc_std_String_pa; _temp11.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp10[ 2u]={& _temp11,& _temp12}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option `%s' requires an argument\n", sizeof(
unsigned char), 38u), _tag_arr( _temp10, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen((
struct _tagged_arr) Cyc_std_nextchar)); Cyc_std_optopt=(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->val;
return( int)(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'?':':'?');}}} _tagged_arr_inplace_plus(& Cyc_std_nextchar,
sizeof( unsigned char),( int) Cyc_std_strlen(( struct _tagged_arr) Cyc_std_nextchar));
if( longind !=  0){*(( int*) _check_null( longind))= option_index;} if((
unsigned int)(( const struct Cyc_std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_std_option), 0u))->flag){*(( int*) _check_null((( const
struct Cyc_std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_std_option),
0u))->flag))=(( const struct Cyc_std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_std_option), 0u))->val; return 0;} return(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->val;}
if(( ! long_only? 1:*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 1)) == '-')? 1:( Cyc_std_strchr( optstring,*((
unsigned char*) _check_unknown_subscript( Cyc_std_nextchar, sizeof(
unsigned char), 0u)))).curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( print_errors){ if(*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 1)) == '-'){({ struct Cyc_std_String_pa_struct _temp15;
_temp15.tag= Cyc_std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_std_nextchar;{
struct Cyc_std_String_pa_struct _temp14; _temp14.tag= Cyc_std_String_pa; _temp14.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp13[ 2u]={& _temp14,& _temp15}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: unrecognized option `--%s'\n", sizeof(
unsigned char), 32u), _tag_arr( _temp13, sizeof( void*), 2u));}}});} else{({
struct Cyc_std_String_pa_struct _temp19; _temp19.tag= Cyc_std_String_pa; _temp19.f1=(
struct _tagged_arr) Cyc_std_nextchar;{ struct Cyc_std_Int_pa_struct _temp18;
_temp18.tag= Cyc_std_Int_pa; _temp18.f1=( int)(( unsigned int)(( int)*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind)),
sizeof( unsigned char), 0))));{ struct Cyc_std_String_pa_struct _temp17; _temp17.tag=
Cyc_std_String_pa; _temp17.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp16[
3u]={& _temp17,& _temp18,& _temp19}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s: unrecognized option `%c%s'\n",
sizeof( unsigned char), 32u), _tag_arr( _temp16, sizeof( void*), 3u));}}}});}}
Cyc_std_nextchar= _tag_arr(({ unsigned int _temp20=( unsigned int) 1;
unsigned char* _temp21=( unsigned char*) GC_malloc_atomic( sizeof( unsigned char)
*  _temp20);{ unsigned int _temp22= _temp20; unsigned int i; for( i= 0; i < 
_temp22; i ++){ _temp21[ i]='\000';}}; _temp21;}), sizeof( unsigned char),(
unsigned int) 1); Cyc_std_optind ++; Cyc_std_optopt= 0; return( int)'?';}}{
unsigned char c=*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& Cyc_std_nextchar, sizeof( unsigned char), 1),
sizeof( unsigned char), 0u)); struct _tagged_arr temp= Cyc_std_strchr( optstring,
c); if(*(( unsigned char*) _check_unknown_subscript( Cyc_std_nextchar, sizeof(
unsigned char), 0u)) == '\000'){ ++ Cyc_std_optind;} if( temp.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: c == ':'){ if( print_errors){ if((
unsigned int) Cyc_std_posixly_correct.curr){({ struct Cyc_std_Int_pa_struct
_temp25; _temp25.tag= Cyc_std_Int_pa; _temp25.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_std_String_pa_struct _temp24; _temp24.tag= Cyc_std_String_pa; _temp24.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp23[ 2u]={& _temp24,& _temp25}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: illegal option -- %c\n", sizeof( unsigned char),
26u), _tag_arr( _temp23, sizeof( void*), 2u));}}});} else{({ struct Cyc_std_Int_pa_struct
_temp28; _temp28.tag= Cyc_std_Int_pa; _temp28.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_std_String_pa_struct _temp27; _temp27.tag= Cyc_std_String_pa; _temp27.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp26[ 2u]={& _temp27,& _temp28}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: invalid option -- %c\n", sizeof( unsigned char),
26u), _tag_arr( _temp26, sizeof( void*), 2u));}}});}} Cyc_std_optopt=( int) c;
return( int)'?';} if(*(( const unsigned char*) _check_unknown_subscript( temp,
sizeof( unsigned char), 0)) == 'W'?*(( const unsigned char*)
_check_unknown_subscript( temp, sizeof( unsigned char), 1)) == ';': 0){ struct
_tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr pfound=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0; int indfound= 0;
int option_index; if(*(( unsigned char*) _check_unknown_subscript( Cyc_std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_std_optarg= Cyc_std_nextchar; Cyc_std_optind
++;} else{ if( Cyc_std_optind ==  argc){ if( print_errors){({ struct Cyc_std_Int_pa_struct
_temp31; _temp31.tag= Cyc_std_Int_pa; _temp31.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_std_String_pa_struct _temp30; _temp30.tag= Cyc_std_String_pa; _temp30.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp29[ 2u]={& _temp30,& _temp31}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option requires an argument -- %c\n", sizeof(
unsigned char), 39u), _tag_arr( _temp29, sizeof( void*), 2u));}}});} Cyc_std_optopt=(
int) c; if(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'){ c=':';} else{ c='?';} return( int) c;}
else{ Cyc_std_optarg=*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_std_optind ++));}} for( Cyc_std_nextchar=(
nameend= Cyc_std_optarg);( int)*(( unsigned char*) _check_unknown_subscript(
nameend, sizeof( unsigned char), 0u))?*(( unsigned char*)
_check_unknown_subscript( nameend, sizeof( unsigned char), 0u)) != '=': 0;
_tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;} for(( p=
longopts, option_index= 0);( unsigned int)((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_std_option), 1),
option_index ++)){ if( ! Cyc_std_strncmp((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name,( struct
_tagged_arr) Cyc_std_nextchar,( unsigned int)(( nameend.curr -  Cyc_std_nextchar.curr)
/  sizeof( unsigned char)))){ if(( unsigned int)(( nameend.curr -  Cyc_std_nextchar.curr)
/  sizeof( unsigned char)) ==  Cyc_std_strlen((( const struct Cyc_std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_std_option), 0u))->name)){
pfound= _tag_arr(( const struct Cyc_std_option*) _check_null( _untag_arr( p,
sizeof( struct Cyc_std_option), 1u)), sizeof( struct Cyc_std_option), 1u);
indfound= option_index; exact= 1; break;} else{ if( pfound.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= _tag_arr(( const struct Cyc_std_option*)
_check_null( _untag_arr( p, sizeof( struct Cyc_std_option), 1u)), sizeof( struct
Cyc_std_option), 1u); indfound= option_index;} else{ ambig= 1;}}}} if( ambig? !
exact: 0){ if( print_errors){({ struct Cyc_std_String_pa_struct _temp34; _temp34.tag=
Cyc_std_String_pa; _temp34.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind));{
struct Cyc_std_String_pa_struct _temp33; _temp33.tag= Cyc_std_String_pa; _temp33.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp32[ 2u]={& _temp33,& _temp34}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option `-W %s' is ambiguous\n", sizeof(
unsigned char), 33u), _tag_arr( _temp32, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen((
struct _tagged_arr) Cyc_std_nextchar)); Cyc_std_optind ++; return( int)'?';} if(
pfound.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ option_index=
indfound; if(( int)*(( unsigned char*) _check_unknown_subscript( nameend,
sizeof( unsigned char), 0u))){ if((( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->has_arg){
Cyc_std_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char), 1);} else{
if( print_errors){({ struct Cyc_std_String_pa_struct _temp37; _temp37.tag= Cyc_std_String_pa;
_temp37.f1=( struct _tagged_arr)(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->name;{
struct Cyc_std_String_pa_struct _temp36; _temp36.tag= Cyc_std_String_pa; _temp36.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp35[ 2u]={& _temp36,& _temp37}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option `-W %s' doesn't allow an argument\n",
sizeof( unsigned char), 46u), _tag_arr( _temp35, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen((
struct _tagged_arr) Cyc_std_nextchar)); return( int)'?';}} else{ if((( const
struct Cyc_std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_std_option),
0u))->has_arg ==  1){ if( Cyc_std_optind <  argc){ Cyc_std_optarg=*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind
++));} else{ if( print_errors){({ struct Cyc_std_String_pa_struct _temp40;
_temp40.tag= Cyc_std_String_pa; _temp40.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_std_optind
-  1));{ struct Cyc_std_String_pa_struct _temp39; _temp39.tag= Cyc_std_String_pa;
_temp39.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp38[
2u]={& _temp39,& _temp40}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s: option `%s' requires an argument\n",
sizeof( unsigned char), 38u), _tag_arr( _temp38, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen((
struct _tagged_arr) Cyc_std_nextchar)); return( int)(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) == ':'?':':'?');}}}
_tagged_arr_inplace_plus(& Cyc_std_nextchar, sizeof( unsigned char),( int) Cyc_std_strlen((
struct _tagged_arr) Cyc_std_nextchar)); if( longind !=  0){*(( int*) _check_null(
longind))= option_index;} if(( unsigned int)(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->flag){*((
int*) _check_null((( const struct Cyc_std_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_std_option), 0u))->flag))=(( const struct Cyc_std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_std_option), 0u))->val;
return 0;} return(( const struct Cyc_std_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_std_option), 0u))->val;} Cyc_std_nextchar= _tag_arr(
0u, 0u, 0u); return( int)'W';} if(*(( const unsigned char*)
_check_unknown_subscript( temp, sizeof( unsigned char), 1)) == ':'){ if(*((
const unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char), 2))
== ':'){ if(*(( unsigned char*) _check_unknown_subscript( Cyc_std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_std_optarg= Cyc_std_nextchar; Cyc_std_optind
++;} else{ Cyc_std_optarg= _tag_arr( 0u, 0u, 0u);} Cyc_std_nextchar= _tag_arr( 0u,
0u, 0u);} else{ if(*(( unsigned char*) _check_unknown_subscript( Cyc_std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_std_optarg= Cyc_std_nextchar; Cyc_std_optind
++;} else{ if( Cyc_std_optind ==  argc){ if( print_errors){({ struct Cyc_std_Int_pa_struct
_temp43; _temp43.tag= Cyc_std_Int_pa; _temp43.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_std_String_pa_struct _temp42; _temp42.tag= Cyc_std_String_pa; _temp42.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp41[ 2u]={& _temp42,& _temp43}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s: option requires an argument -- %c\n", sizeof(
unsigned char), 39u), _tag_arr( _temp41, sizeof( void*), 2u));}}});} Cyc_std_optopt=(
int) c; if(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'){ c=':';} else{ c='?';}} else{ Cyc_std_optarg=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_std_optind ++));}} Cyc_std_nextchar= _tag_arr( 0u, 0u, 0u);}} return( int) c;}}
int Cyc_std_getopt( int argc, struct _tagged_arr argv, struct _tagged_arr
optstring){ return Cyc_std__getopt_internal( argc, argv, optstring,( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u),( int*) 0, 0);}
