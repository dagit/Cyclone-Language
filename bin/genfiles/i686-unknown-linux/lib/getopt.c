// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file lib/runtime_cyc.c

#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};
struct _tagged_string {  // delete after bootstrapping
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
  struct _RegionPage *next;
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
};

extern struct _RegionHandle _new_region();
extern void * _region_malloc(struct _RegionHandle *, unsigned int);
extern void   _free_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void * e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
static inline 
void * _check_null(void * ptr) {
  if(!ptr)
    _throw_null();
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
  if(!ptr || index >= bound)
    _throw_null();
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
  if(index >= bound)
    _throw_arraybounds();
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
  if(!arr.base) 
    _throw_null();
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.last_plus_one - arr.curr) / elt_sz;
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);

static inline void * _cycalloc(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#ifdef CYC_REGION_PROFILE
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
#define _cycalloc(n) _profile_cycalloc(n,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_cycalloc_atomic(n,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern unsigned char Cyc_Std_FileCloseError[
19u]; extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int system( unsigned char*); struct Cyc_Std__Div{
int quot; int rem; } ; struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_Std__Div
div( int __numer, int __denom); extern struct Cyc_Std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern struct _tagged_arr Cyc_Std_getenv( struct _tagged_arr); struct Cyc_Std_timeval{
int tv_sec; int tv_usec; } ; extern struct _tagged_arr Cyc_Std_optarg; extern
int Cyc_Std_optind; extern int Cyc_Std_opterr; extern int Cyc_Std_optopt; struct
Cyc_Std_option{ struct _tagged_arr name; int has_arg; int* flag; int val; } ;
extern int Cyc_Std_getopt( int __argc, struct _tagged_arr __argv, struct
_tagged_arr __shortopts); extern int Cyc_Std__getopt_internal( int __argc,
struct _tagged_arr __argv, struct _tagged_arr __shortopts, struct _tagged_arr
__longopts, int* __longind, int __long_only); extern unsigned int alarm(
unsigned int seconds); extern int close( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int dup( int);
extern int dup2( int, int); extern unsigned int getuid(); extern int setuid(
unsigned int uid); extern unsigned int geteuid(); extern int seteuid(
unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int pipe( int* filedes); extern int lseek( int
filedes, int offset, int whence); int Cyc_Std_chdir( struct _tagged_arr); struct
_tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf, unsigned int size); int Cyc_Std_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_Std_execlp( struct _tagged_arr file, struct _tagged_arr arg0, struct
_tagged_arr argv); int Cyc_Std_execve( struct _tagged_arr filename, struct
_tagged_arr argv, struct _tagged_arr envp); int Cyc_Std_read( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_Std_write( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_Std_unlink( struct _tagged_arr
pathname); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; extern
unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern unsigned int Cyc_Std_strlen(
struct _tagged_arr s); extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct
_tagged_arr s2); extern int Cyc_Std_strncmp( struct _tagged_arr s1, struct
_tagged_arr s2, unsigned int len); extern struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s, unsigned char c); struct _tagged_arr Cyc_Std_optarg; int
Cyc_Std_optind= 1; int Cyc_Std___getopt_initialized; static struct _tagged_arr
Cyc_Std_nextchar; int Cyc_Std_opterr= 1; int Cyc_Std_optopt=( int)'?'; enum  Cyc_Std_ordering_tag{
Cyc_Std_REQUIRE_ORDER  =  0u, Cyc_Std_PERMUTE  =  1u, Cyc_Std_RETURN_IN_ORDER
 =  2u}; static enum  Cyc_Std_ordering_tag Cyc_Std_ordering; static struct
_tagged_arr Cyc_Std_posixly_correct; static int Cyc_Std_first_nonopt; static int
Cyc_Std_last_nonopt; static int Cyc_Std_nonoption_flags_max_len; static int Cyc_Std_nonoption_flags_len;
static int Cyc_Std_original_argc; static int Cyc_Std_original_argv; static void
 __attribute__(( unused ))  Cyc_Std_store_args_and_env( int argc, struct
_tagged_arr argv){ Cyc_Std_original_argc= argc; Cyc_Std_original_argv=( int)
argv.curr;} static void Cyc_Std_exchange( struct _tagged_arr argv){ int bottom=
Cyc_Std_first_nonopt; int middle= Cyc_Std_last_nonopt; int top= Cyc_Std_optind;
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
struct _tagged_arr), middle +  i))= tem;;} bottom += len;}} Cyc_Std_first_nonopt
+= Cyc_Std_optind -  Cyc_Std_last_nonopt; Cyc_Std_last_nonopt= Cyc_Std_optind;}
static struct _tagged_arr Cyc_Std__getopt_initialize( int argc, struct
_tagged_arr argv, struct _tagged_arr optstring){ Cyc_Std_first_nonopt=( Cyc_Std_last_nonopt=
Cyc_Std_optind); Cyc_Std_nextchar= _tag_arr( 0u, 0u, 0u); Cyc_Std_posixly_correct=
Cyc_Std_getenv( _tag_arr("POSIXLY_CORRECT", sizeof( unsigned char), 16u)); if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == '-'){ Cyc_Std_ordering= Cyc_Std_RETURN_IN_ORDER; _tagged_arr_inplace_plus(&
optstring, sizeof( unsigned char), 1);} else{ if(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) == '+'){ Cyc_Std_ordering=
Cyc_Std_REQUIRE_ORDER; _tagged_arr_inplace_plus(& optstring, sizeof(
unsigned char), 1);} else{ if( Cyc_Std_posixly_correct.curr != ( _tag_arr( 0u, 0u,
0u)).curr){ Cyc_Std_ordering= Cyc_Std_REQUIRE_ORDER;} else{ Cyc_Std_ordering=
Cyc_Std_PERMUTE;}}} return optstring;} int Cyc_Std__getopt_internal( int argc,
struct _tagged_arr argv, struct _tagged_arr optstring, struct _tagged_arr
longopts, int* longind, int long_only){ int print_errors= Cyc_Std_opterr; if(*((
const unsigned char*) _check_unknown_subscript( optstring, sizeof( unsigned char),
0)) == ':'){ print_errors= 0;} if( argc <  1){ return - 1;} Cyc_Std_optarg=
_tag_arr( 0u, 0u, 0u); if( Cyc_Std_optind ==  0? 1: ! Cyc_Std___getopt_initialized){
if( Cyc_Std_optind ==  0){ Cyc_Std_optind= 1;} optstring= Cyc_Std__getopt_initialize(
argc, argv, optstring); Cyc_Std___getopt_initialized= 1;} if( Cyc_Std_nextchar.curr
== ( _tag_arr( 0u, 0u, 0u)).curr? 1:*(( unsigned char*) _check_unknown_subscript(
Cyc_Std_nextchar, sizeof( unsigned char), 0u)) == '\000'){ if( Cyc_Std_last_nonopt
>  Cyc_Std_optind){ Cyc_Std_last_nonopt= Cyc_Std_optind;} if( Cyc_Std_first_nonopt
>  Cyc_Std_optind){ Cyc_Std_first_nonopt= Cyc_Std_optind;} if( Cyc_Std_ordering
==  Cyc_Std_PERMUTE){ if( Cyc_Std_first_nonopt !=  Cyc_Std_last_nonopt? Cyc_Std_last_nonopt
!=  Cyc_Std_optind: 0){ Cyc_Std_exchange( argv);} else{ if( Cyc_Std_last_nonopt
!=  Cyc_Std_optind){ Cyc_Std_first_nonopt= Cyc_Std_optind;}} while( Cyc_Std_optind
<  argc?*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 0)) != '-'? 1:*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind)), sizeof( unsigned char), 1)) == '\000':
0) { Cyc_Std_optind ++;} Cyc_Std_last_nonopt= Cyc_Std_optind;} if( Cyc_Std_optind
!=  argc? ! Cyc_Std_strcmp(( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
_tag_arr("--", sizeof( unsigned char), 3u)): 0){ Cyc_Std_optind ++; if( Cyc_Std_first_nonopt
!=  Cyc_Std_last_nonopt? Cyc_Std_last_nonopt !=  Cyc_Std_optind: 0){ Cyc_Std_exchange(
argv);} else{ if( Cyc_Std_first_nonopt ==  Cyc_Std_last_nonopt){ Cyc_Std_first_nonopt=
Cyc_Std_optind;}} Cyc_Std_last_nonopt= argc; Cyc_Std_optind= argc;} if( Cyc_Std_optind
==  argc){ if( Cyc_Std_first_nonopt !=  Cyc_Std_last_nonopt){ Cyc_Std_optind=
Cyc_Std_first_nonopt;} return - 1;} if(*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind)), sizeof( unsigned char), 0)) != '-'?
1:*(( unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 1)) == '\000'){ if( Cyc_Std_ordering ==  Cyc_Std_REQUIRE_ORDER){
return - 1;} Cyc_Std_optarg=*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), Cyc_Std_optind ++)); return 1;} Cyc_Std_nextchar=
_tagged_arr_plus( _tagged_arr_plus(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 1), sizeof( unsigned char), longopts.curr != (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind)), sizeof( unsigned char), 1)) == '-':
0);} if( longopts.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr?*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 1)) == '-'? 1:( long_only?( int)*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind)), sizeof( unsigned char), 2))? 1: !((
unsigned int)( Cyc_Std_strchr( optstring,*(( unsigned char*)
_check_unknown_subscript(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind)), sizeof( unsigned char), 1)))).curr):
0): 0){ struct _tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr
pfound=( struct _tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0;
int indfound= - 1; int option_index; for( nameend= Cyc_Std_nextchar;( int)*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))?*((
unsigned char*) _check_unknown_subscript( nameend, sizeof( unsigned char), 0u))
!= '=': 0; _tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;}
for(( p= longopts, option_index= 0);( unsigned int)((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_Std_option), 1),
option_index ++)){ if( ! Cyc_Std_strncmp((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name,( struct
_tagged_arr) Cyc_Std_nextchar,( unsigned int)(( nameend.curr -  Cyc_Std_nextchar.curr)
/  sizeof( unsigned char)))){ if(( unsigned int)(( nameend.curr -  Cyc_Std_nextchar.curr)
/  sizeof( unsigned char)) == ( unsigned int) Cyc_Std_strlen((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name)){
pfound= p; indfound= option_index; exact= 1; break;} else{ if( pfound.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= p; indfound=
option_index;} else{ if((( long_only? 1:(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->has_arg
!= (( const struct Cyc_Std_option*) _check_unknown_subscript( p, sizeof( struct
Cyc_Std_option), 0u))->has_arg)? 1:(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->flag != ((
const struct Cyc_Std_option*) _check_unknown_subscript( p, sizeof( struct Cyc_Std_option),
0u))->flag)? 1:(( const struct Cyc_Std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_Std_option), 0u))->val != (( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->val){ ambig=
1;}}}}} if( ambig? ! exact: 0){ if( print_errors){({ struct Cyc_Std_String_pa_struct
_temp2; _temp2.tag= Cyc_Std_String_pa; _temp2.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind));{
struct Cyc_Std_String_pa_struct _temp1; _temp1.tag= Cyc_Std_String_pa; _temp1.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp0[ 2u]={& _temp1,& _temp2}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option `%s' is ambiguous\n", sizeof( unsigned char),
30u), _tag_arr( _temp0, sizeof( void*), 2u));}}});} _tagged_arr_inplace_plus(&
Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen(( struct
_tagged_arr) Cyc_Std_nextchar)); Cyc_Std_optind ++; Cyc_Std_optopt= 0; return(
int)'?';} if( pfound.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
option_index= indfound; Cyc_Std_optind ++; if(( int)*(( unsigned char*)
_check_unknown_subscript( nameend, sizeof( unsigned char), 0u))){ if((( const
struct Cyc_Std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option),
0u))->has_arg){ Cyc_Std_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char),
1);} else{ if( print_errors){ if(*(( unsigned char*) _check_unknown_subscript(*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Std_optind -  1)), sizeof( unsigned char), 1)) == '-'){({ struct Cyc_Std_String_pa_struct
_temp5; _temp5.tag= Cyc_Std_String_pa; _temp5.f1=( struct _tagged_arr)(( const
struct Cyc_Std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option),
0u))->name;{ struct Cyc_Std_String_pa_struct _temp4; _temp4.tag= Cyc_Std_String_pa;
_temp4.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript(
argv, sizeof( struct _tagged_arr), 0));{ void* _temp3[ 2u]={& _temp4,& _temp5};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: option `--%s' doesn't allow an argument\n",
sizeof( unsigned char), 45u), _tag_arr( _temp3, sizeof( void*), 2u));}}});}
else{({ struct Cyc_Std_String_pa_struct _temp9; _temp9.tag= Cyc_Std_String_pa;
_temp9.f1=( struct _tagged_arr)(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->name;{
struct Cyc_Std_Int_pa_struct _temp8; _temp8.tag= Cyc_Std_Int_pa; _temp8.f1=( int)((
unsigned int)(( int)*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind
-  1)), sizeof( unsigned char), 0))));{ struct Cyc_Std_String_pa_struct _temp7;
_temp7.tag= Cyc_Std_String_pa; _temp7.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{
void* _temp6[ 3u]={& _temp7,& _temp8,& _temp9}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s: option `%c%s' doesn't allow an argument\n", sizeof( unsigned char),
45u), _tag_arr( _temp6, sizeof( void*), 3u));}}}});}} _tagged_arr_inplace_plus(&
Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen(( struct
_tagged_arr) Cyc_Std_nextchar)); Cyc_Std_optopt=(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->val;
return( int)'?';}} else{ if((( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->has_arg
==  1){ if( Cyc_Std_optind <  argc){ Cyc_Std_optarg=*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind ++));}
else{ if( print_errors){({ struct Cyc_Std_String_pa_struct _temp12; _temp12.tag=
Cyc_Std_String_pa; _temp12.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind -  1));{
struct Cyc_Std_String_pa_struct _temp11; _temp11.tag= Cyc_Std_String_pa; _temp11.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp10[ 2u]={& _temp11,& _temp12}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option `%s' requires an argument\n", sizeof(
unsigned char), 38u), _tag_arr( _temp10, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen((
struct _tagged_arr) Cyc_Std_nextchar)); Cyc_Std_optopt=(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->val;
return( int)(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'?':':'?');}}} _tagged_arr_inplace_plus(& Cyc_Std_nextchar,
sizeof( unsigned char),( int) Cyc_Std_strlen(( struct _tagged_arr) Cyc_Std_nextchar));
if( longind !=  0){*(( int*) _check_null( longind))= option_index;} if((
unsigned int)(( const struct Cyc_Std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_Std_option), 0u))->flag){*(( int*) _check_null((( const
struct Cyc_Std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option),
0u))->flag))=(( const struct Cyc_Std_option*) _check_unknown_subscript( pfound,
sizeof( struct Cyc_Std_option), 0u))->val; return 0;} return(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->val;}
if(( ! long_only? 1:*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 1)) == '-')? 1:( Cyc_Std_strchr( optstring,*((
unsigned char*) _check_unknown_subscript( Cyc_Std_nextchar, sizeof(
unsigned char), 0u)))).curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
if( print_errors){ if(*(( unsigned char*) _check_unknown_subscript(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 1)) == '-'){({ struct Cyc_Std_String_pa_struct _temp15;
_temp15.tag= Cyc_Std_String_pa; _temp15.f1=( struct _tagged_arr) Cyc_Std_nextchar;{
struct Cyc_Std_String_pa_struct _temp14; _temp14.tag= Cyc_Std_String_pa; _temp14.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp13[ 2u]={& _temp14,& _temp15}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: unrecognized option `--%s'\n", sizeof(
unsigned char), 32u), _tag_arr( _temp13, sizeof( void*), 2u));}}});} else{({
struct Cyc_Std_String_pa_struct _temp19; _temp19.tag= Cyc_Std_String_pa; _temp19.f1=(
struct _tagged_arr) Cyc_Std_nextchar;{ struct Cyc_Std_Int_pa_struct _temp18;
_temp18.tag= Cyc_Std_Int_pa; _temp18.f1=( int)(( unsigned int)(( int)*((
unsigned char*) _check_unknown_subscript(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind)),
sizeof( unsigned char), 0))));{ struct Cyc_Std_String_pa_struct _temp17; _temp17.tag=
Cyc_Std_String_pa; _temp17.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp16[
3u]={& _temp17,& _temp18,& _temp19}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: unrecognized option `%c%s'\n",
sizeof( unsigned char), 32u), _tag_arr( _temp16, sizeof( void*), 3u));}}}});}}
Cyc_Std_nextchar= _tag_arr(({ unsigned int _temp20=( unsigned int) 1;
unsigned char* _temp21=( unsigned char*) _cycalloc_atomic( _check_times( sizeof(
unsigned char), _temp20));{ unsigned int _temp22= _temp20; unsigned int i; for(
i= 0; i <  _temp22; i ++){ _temp21[ i]='\000';}}; _temp21;}), sizeof(
unsigned char),( unsigned int) 1); Cyc_Std_optind ++; Cyc_Std_optopt= 0; return(
int)'?';}}{ unsigned char c=*(( unsigned char*) _check_unknown_subscript(
_tagged_arr_inplace_plus_post(& Cyc_Std_nextchar, sizeof( unsigned char), 1),
sizeof( unsigned char), 0u)); struct _tagged_arr temp= Cyc_Std_strchr( optstring,
c); if(*(( unsigned char*) _check_unknown_subscript( Cyc_Std_nextchar, sizeof(
unsigned char), 0u)) == '\000'){ ++ Cyc_Std_optind;} if( temp.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: c == ':'){ if( print_errors){ if((
unsigned int) Cyc_Std_posixly_correct.curr){({ struct Cyc_Std_Int_pa_struct
_temp25; _temp25.tag= Cyc_Std_Int_pa; _temp25.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_Std_String_pa_struct _temp24; _temp24.tag= Cyc_Std_String_pa; _temp24.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp23[ 2u]={& _temp24,& _temp25}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: illegal option -- %c\n", sizeof( unsigned char),
26u), _tag_arr( _temp23, sizeof( void*), 2u));}}});} else{({ struct Cyc_Std_Int_pa_struct
_temp28; _temp28.tag= Cyc_Std_Int_pa; _temp28.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_Std_String_pa_struct _temp27; _temp27.tag= Cyc_Std_String_pa; _temp27.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp26[ 2u]={& _temp27,& _temp28}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: invalid option -- %c\n", sizeof( unsigned char),
26u), _tag_arr( _temp26, sizeof( void*), 2u));}}});}} Cyc_Std_optopt=( int) c;
return( int)'?';} if(*(( const unsigned char*) _check_unknown_subscript( temp,
sizeof( unsigned char), 0)) == 'W'?*(( const unsigned char*)
_check_unknown_subscript( temp, sizeof( unsigned char), 1)) == ';': 0){ struct
_tagged_arr nameend; struct _tagged_arr p; struct _tagged_arr pfound=( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u); int exact= 0; int ambig= 0; int indfound= 0;
int option_index; if(*(( unsigned char*) _check_unknown_subscript( Cyc_Std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_Std_optarg= Cyc_Std_nextchar; Cyc_Std_optind
++;} else{ if( Cyc_Std_optind ==  argc){ if( print_errors){({ struct Cyc_Std_Int_pa_struct
_temp31; _temp31.tag= Cyc_Std_Int_pa; _temp31.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_Std_String_pa_struct _temp30; _temp30.tag= Cyc_Std_String_pa; _temp30.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp29[ 2u]={& _temp30,& _temp31}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option requires an argument -- %c\n", sizeof(
unsigned char), 39u), _tag_arr( _temp29, sizeof( void*), 2u));}}});} Cyc_Std_optopt=(
int) c; if(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'){ c=':';} else{ c='?';} return( int) c;}
else{ Cyc_Std_optarg=*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), Cyc_Std_optind ++));}} for( Cyc_Std_nextchar=(
nameend= Cyc_Std_optarg);( int)*(( unsigned char*) _check_unknown_subscript(
nameend, sizeof( unsigned char), 0u))?*(( unsigned char*)
_check_unknown_subscript( nameend, sizeof( unsigned char), 0u)) != '=': 0;
_tagged_arr_inplace_plus_post(& nameend, sizeof( unsigned char), 1)){;} for(( p=
longopts, option_index= 0);( unsigned int)((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name).curr;(
_tagged_arr_inplace_plus_post(& p, sizeof( struct Cyc_Std_option), 1),
option_index ++)){ if( ! Cyc_Std_strncmp((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name,( struct
_tagged_arr) Cyc_Std_nextchar,( unsigned int)(( nameend.curr -  Cyc_Std_nextchar.curr)
/  sizeof( unsigned char)))){ if(( unsigned int)(( nameend.curr -  Cyc_Std_nextchar.curr)
/  sizeof( unsigned char)) ==  Cyc_Std_strlen((( const struct Cyc_Std_option*)
_check_unknown_subscript( p, sizeof( struct Cyc_Std_option), 0u))->name)){
pfound= _tag_arr(( const struct Cyc_Std_option*) _check_null( _untag_arr( p,
sizeof( struct Cyc_Std_option), 1u)), sizeof( struct Cyc_Std_option), 1u);
indfound= option_index; exact= 1; break;} else{ if( pfound.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ pfound= _tag_arr(( const struct Cyc_Std_option*)
_check_null( _untag_arr( p, sizeof( struct Cyc_Std_option), 1u)), sizeof( struct
Cyc_Std_option), 1u); indfound= option_index;} else{ ambig= 1;}}}} if( ambig? !
exact: 0){ if( print_errors){({ struct Cyc_Std_String_pa_struct _temp34; _temp34.tag=
Cyc_Std_String_pa; _temp34.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind));{
struct Cyc_Std_String_pa_struct _temp33; _temp33.tag= Cyc_Std_String_pa; _temp33.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp32[ 2u]={& _temp33,& _temp34}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option `-W %s' is ambiguous\n", sizeof(
unsigned char), 33u), _tag_arr( _temp32, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen((
struct _tagged_arr) Cyc_Std_nextchar)); Cyc_Std_optind ++; return( int)'?';} if(
pfound.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ option_index=
indfound; if(( int)*(( unsigned char*) _check_unknown_subscript( nameend,
sizeof( unsigned char), 0u))){ if((( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->has_arg){
Cyc_Std_optarg= _tagged_arr_plus( nameend, sizeof( unsigned char), 1);} else{
if( print_errors){({ struct Cyc_Std_String_pa_struct _temp37; _temp37.tag= Cyc_Std_String_pa;
_temp37.f1=( struct _tagged_arr)(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->name;{
struct Cyc_Std_String_pa_struct _temp36; _temp36.tag= Cyc_Std_String_pa; _temp36.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp35[ 2u]={& _temp36,& _temp37}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option `-W %s' doesn't allow an argument\n",
sizeof( unsigned char), 46u), _tag_arr( _temp35, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen((
struct _tagged_arr) Cyc_Std_nextchar)); return( int)'?';}} else{ if((( const
struct Cyc_Std_option*) _check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option),
0u))->has_arg ==  1){ if( Cyc_Std_optind <  argc){ Cyc_Std_optarg=*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind
++));} else{ if( print_errors){({ struct Cyc_Std_String_pa_struct _temp40;
_temp40.tag= Cyc_Std_String_pa; _temp40.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), Cyc_Std_optind
-  1));{ struct Cyc_Std_String_pa_struct _temp39; _temp39.tag= Cyc_Std_String_pa;
_temp39.f1=( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), 0));{ void* _temp38[
2u]={& _temp39,& _temp40}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("%s: option `%s' requires an argument\n",
sizeof( unsigned char), 38u), _tag_arr( _temp38, sizeof( void*), 2u));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen((
struct _tagged_arr) Cyc_Std_nextchar)); return( int)(*(( const unsigned char*)
_check_unknown_subscript( optstring, sizeof( unsigned char), 0)) == ':'?':':'?');}}}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar, sizeof( unsigned char),( int) Cyc_Std_strlen((
struct _tagged_arr) Cyc_Std_nextchar)); if( longind !=  0){*(( int*) _check_null(
longind))= option_index;} if(( unsigned int)(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->flag){*((
int*) _check_null((( const struct Cyc_Std_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Std_option), 0u))->flag))=(( const struct Cyc_Std_option*)
_check_unknown_subscript( pfound, sizeof( struct Cyc_Std_option), 0u))->val;
return 0;} return(( const struct Cyc_Std_option*) _check_unknown_subscript(
pfound, sizeof( struct Cyc_Std_option), 0u))->val;} Cyc_Std_nextchar= _tag_arr(
0u, 0u, 0u); return( int)'W';} if(*(( const unsigned char*)
_check_unknown_subscript( temp, sizeof( unsigned char), 1)) == ':'){ if(*((
const unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char), 2))
== ':'){ if(*(( unsigned char*) _check_unknown_subscript( Cyc_Std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_Std_optarg= Cyc_Std_nextchar; Cyc_Std_optind
++;} else{ Cyc_Std_optarg= _tag_arr( 0u, 0u, 0u);} Cyc_Std_nextchar= _tag_arr( 0u,
0u, 0u);} else{ if(*(( unsigned char*) _check_unknown_subscript( Cyc_Std_nextchar,
sizeof( unsigned char), 0u)) != '\000'){ Cyc_Std_optarg= Cyc_Std_nextchar; Cyc_Std_optind
++;} else{ if( Cyc_Std_optind ==  argc){ if( print_errors){({ struct Cyc_Std_Int_pa_struct
_temp43; _temp43.tag= Cyc_Std_Int_pa; _temp43.f1=( int)(( unsigned int)(( int) c));{
struct Cyc_Std_String_pa_struct _temp42; _temp42.tag= Cyc_Std_String_pa; _temp42.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), 0));{ void* _temp41[ 2u]={& _temp42,& _temp43}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("%s: option requires an argument -- %c\n", sizeof(
unsigned char), 39u), _tag_arr( _temp41, sizeof( void*), 2u));}}});} Cyc_Std_optopt=(
int) c; if(*(( const unsigned char*) _check_unknown_subscript( optstring,
sizeof( unsigned char), 0)) == ':'){ c=':';} else{ c='?';}} else{ Cyc_Std_optarg=*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),
Cyc_Std_optind ++));}} Cyc_Std_nextchar= _tag_arr( 0u, 0u, 0u);}} return( int) c;}}
int Cyc_Std_getopt( int argc, struct _tagged_arr argv, struct _tagged_arr
optstring){ return Cyc_Std__getopt_internal( argc, argv, optstring,( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u),( int*) 0, 0);}
