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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); int Cyc_Execinfo_backtrace(
struct _tagged_arr, int); int Cyc_Execinfo_bt(); struct Cyc_Std___sFILE; extern
struct Cyc_Std___sFILE* Cyc_Std_stdout; extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*);
extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern int Cyc_Std_getc(
struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_putc( int __c, struct Cyc_Std___sFILE*
__stream); extern struct Cyc_Std___sFILE* Cyc_Std_fdopen( int __fd, struct
_tagged_arr __modes); extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern
unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_Std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa=
0; struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int wait( int* status); extern int waitpid( int
pid, int* status, int options); void(* Cyc_Std_signal( int sig, void(* func)(
int)))( int); extern int raise( int sig); extern int kill( int, int); struct Cyc_Std_timeval{
int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{ int tz_minuteswest; int
tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone*
__z); struct Cyc_Std_option{ struct _tagged_arr name; int has_arg; int* flag;
int val; } ; extern unsigned int alarm( unsigned int seconds); extern int close(
int); extern void _exit( int); extern int getpid(); extern int getppid(); extern
int fork(); extern int fchdir( int); extern int fchown( int, unsigned int,
unsigned int); extern int fsync( int); extern int ftruncate( int, int); extern
int dup( int); extern int dup2( int, int); extern unsigned int getuid(); extern
int setuid( unsigned int uid); extern unsigned int geteuid(); extern int seteuid(
unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); int Cyc_Std_access(
struct _tagged_arr, int); int Cyc_Std_chdir( struct _tagged_arr); int Cyc_Std_chown(
struct _tagged_arr, unsigned int, unsigned int); struct _tagged_arr Cyc_Std_getcwd(
struct _tagged_arr buf, unsigned int size); int Cyc_Std_execl( struct
_tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execlp(
struct _tagged_arr file, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_Std_execve( struct _tagged_arr filename, struct _tagged_arr argv, struct
_tagged_arr envp); int Cyc_Std_link( struct _tagged_arr, struct _tagged_arr);
int Cyc_Std_read( int fd, struct _tagged_arr buf, unsigned int count); int Cyc_Std_rmdir(
struct _tagged_arr); int Cyc_Std_symlink( struct _tagged_arr, struct _tagged_arr);
int Cyc_Std_truncate( struct _tagged_arr, int); int Cyc_Std_write( int fd,
struct _tagged_arr buf, unsigned int count); int Cyc_Std_unlink( struct
_tagged_arr pathname); extern int backtrace( int*, int); int Cyc_Execinfo_backtrace(
struct _tagged_arr array, int size){ if( size >  _get_arr_size( array, sizeof(
int))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp0[ 0]=({ struct Cyc_Core_Failure_struct _temp1; _temp1.tag= Cyc_Core_Failure;
_temp1.f1= _tag_arr("backtrace: size > array.size", sizeof( unsigned char), 29u);
_temp1;}); _temp0;}));} return backtrace(( int*) _check_null( _untag_arr( array,
sizeof( int), 1u)), size);} int Cyc_Execinfo_bt(){ int bt[ 20u];{ unsigned int
_temp10= 20u; unsigned int i; for( i= 0; i <  _temp10; i ++){ bt[ i]= 0;}}{ int
tochild[ 2u]={ 0, 0}; int fromchild[ 2u]={ 0, 0}; int pid; int self_pid; if(
pipe( tochild)? 1: pipe( fromchild)){ return 1;} self_pid= getpid(); if(( pid=
fork()) ==  0){ if( dup2( tochild[ _check_known_subscript_notnull( 2u, 0)], 0)
==  - 1? 1: dup2( fromchild[ _check_known_subscript_notnull( 2u, 1)], 1) ==  - 1){
return 1;}({ struct _tagged_arr _temp2[ 4u]; _temp2[ 3u]=( struct _tagged_arr)
_tag_arr( 0u, 0u, 0u); _temp2[ 2u]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp4; _temp4.tag= Cyc_Std_Int_pa; _temp4.f1=( int)(( unsigned int) self_pid);{
void* _temp3[ 1u]={& _temp4}; Cyc_Std_aprintf( _tag_arr("/proc/%d/exe", sizeof(
unsigned char), 13u), _tag_arr( _temp3, sizeof( void*), 1u));}}); _temp2[ 1u]=
_tag_arr("-e", sizeof( unsigned char), 3u); _temp2[ 0u]= _tag_arr("--functions",
sizeof( unsigned char), 12u); Cyc_Std_execlp( _tag_arr("addr2line", sizeof(
unsigned char), 10u), _tag_arr("addr2line", sizeof( unsigned char), 10u),
_tag_arr( _temp2, sizeof( struct _tagged_arr), 4u));}); return 1;} else{ if( pid
<  0){ close( tochild[ _check_known_subscript_notnull( 2u, 0)]); close( tochild[
_check_known_subscript_notnull( 2u, 1)]); close( fromchild[
_check_known_subscript_notnull( 2u, 0)]); close( fromchild[
_check_known_subscript_notnull( 2u, 1)]); return 1;}}{ struct Cyc_Std___sFILE* w=({
struct Cyc_Std___sFILE* f= Cyc_Std_fdopen( tochild[
_check_known_subscript_notnull( 2u, 1)], _tag_arr("w", sizeof( unsigned char), 2u));
if( !(( unsigned int) f)){ return 1;}( struct Cyc_Std___sFILE*) _check_null( f);});
struct Cyc_Std___sFILE* r=({ struct Cyc_Std___sFILE* f= Cyc_Std_fdopen(
fromchild[ _check_known_subscript_notnull( 2u, 0)], _tag_arr("r", sizeof(
unsigned char), 2u)); if( !(( unsigned int) f)){ Cyc_Std_fclose( w); return 1;}(
struct Cyc_Std___sFILE*) _check_null( f);}); int n= Cyc_Execinfo_backtrace(
_tag_arr( bt, sizeof( int), 20u),( int) 20u);{ int c= 0; for( 0; c <  n; c ++){({
struct Cyc_Std_Int_pa_struct _temp6; _temp6.tag= Cyc_Std_Int_pa; _temp6.f1=(
unsigned int) bt[ _check_known_subscript_notnull( 20u, c)];{ void* _temp5[ 1u]={&
_temp6}; Cyc_Std_fprintf( w, _tag_arr("%#x\n", sizeof( unsigned char), 5u),
_tag_arr( _temp5, sizeof( void*), 1u));}});}} Cyc_Std_fflush(( struct Cyc_Std___sFILE*)
w);({ void* _temp7[ 0u]={}; Cyc_Std_printf( _tag_arr("Backtrace:\n  Function          Location\n  ----------------  --------------------------------\n",
sizeof( unsigned char), 94u), _tag_arr( _temp7, sizeof( void*), 0u));});{ int c=
0; for( 0; c <  n; c ++){ int d; int pos= 0;({ void* _temp8[ 0u]={}; Cyc_Std_printf(
_tag_arr("  ", sizeof( unsigned char), 3u), _tag_arr( _temp8, sizeof( void*), 0u));});
while(( d= Cyc_Std_getc( r)) != '\n') {( Cyc_Std_putc( d, Cyc_Std_stdout), pos
++);} while( pos ++ <  16) { Cyc_Std_putc(( int)' ', Cyc_Std_stdout);}({ void*
_temp9[ 0u]={}; Cyc_Std_printf( _tag_arr("  ", sizeof( unsigned char), 3u),
_tag_arr( _temp9, sizeof( void*), 0u));}); while(( d= Cyc_Std_getc( r)) != '\n') {
Cyc_Std_putc( d, Cyc_Std_stdout);} Cyc_Std_putc(( int)'\n', Cyc_Std_stdout);}}
Cyc_Std_fclose( r); Cyc_Std_fclose( w); kill( pid, 15); waitpid( pid, 0, 0);
return 0;}}}
