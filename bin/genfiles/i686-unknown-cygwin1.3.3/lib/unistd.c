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
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
//  extern struct _RegionHandle _new_region();
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

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#endif

#endif
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_timeval{ int tv_sec; int tv_usec;
} ; struct Cyc_Std_timezone{ int tz_minuteswest; int tz_dsttime; } ; extern int
gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone* __z);
typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t; struct Cyc_Std_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_Std_timeval; extern int select( int,
struct Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*,
struct Cyc_Std_timeval*); struct Cyc_Std_option{ struct _tagged_arr name; int
has_arg; int* flag; int val; } ; extern unsigned int alarm( unsigned int seconds);
extern int close( int); extern void _exit( int); extern int getpid(); extern int
getppid(); extern int fork(); extern int fchdir( int); extern int fchown( int,
unsigned short, unsigned short); extern int fsync( int); extern int ftruncate(
int, int); extern int dup( int); extern int dup2( int, int); extern int setsid();
extern int getsid( int pid); extern unsigned short getuid(); extern int setuid(
unsigned short uid); extern unsigned short geteuid(); extern int seteuid(
unsigned short euid); extern unsigned short getgid(); extern int setgid(
unsigned short gid); extern unsigned short getegid(); extern int setegid(
unsigned short egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); extern int daemon(
int nochdir, int noclose); int Cyc_Std_access( struct _tagged_arr, int); int Cyc_Std_chdir(
struct _tagged_arr); int Cyc_Std_chown( struct _tagged_arr, unsigned short,
unsigned short); struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf,
unsigned int size); int Cyc_Std_execl( struct _tagged_arr path, struct
_tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execlp( struct
_tagged_arr file, struct _tagged_arr arg0, struct _tagged_arr argv); int Cyc_Std_execve(
struct _tagged_arr filename, struct _tagged_arr argv, struct _tagged_arr envp);
int Cyc_Std_link( struct _tagged_arr, struct _tagged_arr); int Cyc_Std_read( int
fd, struct _tagged_arr buf, unsigned int count); int Cyc_Std_rmdir( struct
_tagged_arr); int Cyc_Std_symlink( struct _tagged_arr, struct _tagged_arr); int
Cyc_Std_truncate( struct _tagged_arr, int); int Cyc_Std_write( int fd, struct
_tagged_arr buf, unsigned int count); int Cyc_Std_unlink( struct _tagged_arr
pathname); int Cyc_Std_gethostname( struct _tagged_arr, unsigned int); int Cyc_Std_chroot(
struct _tagged_arr); struct _tagged_arr Cyc_Std_getpass( struct _tagged_arr
prompt); extern int access( unsigned char*, int); extern int chdir(
unsigned char*); extern int chown( unsigned char*, unsigned short,
unsigned short); extern unsigned char* getcwd( unsigned char* buf, unsigned int
size); extern int execv( unsigned char* path, unsigned char** argv); extern int
execvp( unsigned char* file, unsigned char** argv); extern int execve(
unsigned char* path, unsigned char** argv, unsigned char** envp); extern int
link( unsigned char* path1, unsigned char* path2); extern int read( int fd,
unsigned char* buf, unsigned int count); extern int rmdir( unsigned char*);
extern int symlink( unsigned char* path1, unsigned char* path2); extern int
truncate( unsigned char*, int); extern int write( int fd, unsigned char* buf,
unsigned int count); extern int unlink( unsigned char* pathname); extern int
gethostname( unsigned char*, unsigned int); extern int chroot( unsigned char*);
extern unsigned char* getpass( unsigned char*); int Cyc_Std_access( struct
_tagged_arr path, int mode){ return access( string_to_Cstring( path), mode);}
int Cyc_Std_chdir( struct _tagged_arr path){ return chdir( string_to_Cstring(
path));} int Cyc_Std_chown( struct _tagged_arr path, unsigned short owner,
unsigned short group){ return chown( string_to_Cstring( path), owner, group);}
struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf, unsigned int size){
if( !(( unsigned int) buf.curr)? 1: _get_arr_size( buf, sizeof( unsigned char))
<  size){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp0[ 0]=({ struct Cyc_Core_Failure_struct _temp1; _temp1.tag= Cyc_Core_Failure;
_temp1.f1= _tag_arr("getcwd: invalid buf argument", sizeof( unsigned char), 29u);
_temp1;}); _temp0;}));}{ unsigned char* response= getcwd(( unsigned char*)
_check_null( _untag_arr( buf, sizeof( unsigned char), 0u)), size); return(
unsigned int) response? buf: _tag_arr( 0u, 0u, 0u);}} int Cyc_Std_execl( struct
_tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv){ if((*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) -  1)))).curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp2=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp2[ 0]=({ struct Cyc_Core_Failure_struct
_temp3; _temp3.tag= Cyc_Core_Failure; _temp3.f1= _tag_arr("execl: arg list must be NULL-terminated",
sizeof( unsigned char), 40u); _temp3;}); _temp2;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp4= 1 +  _get_arr_size( argv, sizeof( struct
_tagged_arr)); unsigned char** _temp5=( unsigned char**) _cycalloc( _check_times(
sizeof( unsigned char*), _temp4)); struct _tagged_arr _temp7= _tag_arr( _temp5,
sizeof( unsigned char*), 1 +  _get_arr_size( argv, sizeof( struct _tagged_arr)));{
unsigned int _temp6= _temp4; unsigned int i; for( i= 0; i <  _temp6; i ++){
_temp5[ i]= 0;}}; _temp7;});*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), 0))= string_to_Cstring( arg0);{ int i= 0; for(
0; i <  _get_arr_size( argv, sizeof( struct _tagged_arr)); i ++){*((
unsigned char**) _check_unknown_subscript( newargs, sizeof( unsigned char*), i + 
1))= string_to_Cstring(*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr), i)));}} return execv( string_to_Cstring( path),(
unsigned char**) _check_null( _untag_arr( newargs, sizeof( unsigned char*), 1u)));}}
int Cyc_Std_execlp( struct _tagged_arr path, struct _tagged_arr arg0, struct
_tagged_arr argv){ if((*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr),( int)( _get_arr_size( argv, sizeof( struct
_tagged_arr)) -  1)))).curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp8=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp8[ 0]=({ struct Cyc_Core_Failure_struct
_temp9; _temp9.tag= Cyc_Core_Failure; _temp9.f1= _tag_arr("execl: arg list must be NULL-terminated",
sizeof( unsigned char), 40u); _temp9;}); _temp8;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp10= 1 +  _get_arr_size( argv, sizeof( struct
_tagged_arr)); unsigned char** _temp11=( unsigned char**) _cycalloc(
_check_times( sizeof( unsigned char*), _temp10)); struct _tagged_arr _temp13=
_tag_arr( _temp11, sizeof( unsigned char*), 1 +  _get_arr_size( argv, sizeof(
struct _tagged_arr)));{ unsigned int _temp12= _temp10; unsigned int i; for( i= 0;
i <  _temp12; i ++){ _temp11[ i]= 0;}}; _temp13;});*(( unsigned char**)
_check_unknown_subscript( newargs, sizeof( unsigned char*), 0))=
string_to_Cstring( arg0);{ int i= 0; for( 0; i <  _get_arr_size( argv, sizeof(
struct _tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), i +  1))= string_to_Cstring(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), i)));}}
return execvp( string_to_Cstring( path),( unsigned char**) _check_null(
_untag_arr( newargs, sizeof( unsigned char*), 1u)));}} int Cyc_Std_execve(
struct _tagged_arr filename, struct _tagged_arr argv, struct _tagged_arr envp){
if((*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct
_tagged_arr),( int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) -  1)))).curr
!= ( _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp14=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp14[ 0]=({ struct Cyc_Core_Failure_struct _temp15; _temp15.tag= Cyc_Core_Failure;
_temp15.f1= _tag_arr("execve: arg list must be NULL-terminated", sizeof(
unsigned char), 41u); _temp15;}); _temp14;}));}{ struct _tagged_arr newargs=({
unsigned int _temp20= _get_arr_size( argv, sizeof( struct _tagged_arr));
unsigned char** _temp21=( unsigned char**) _cycalloc( _check_times( sizeof(
unsigned char*), _temp20)); struct _tagged_arr _temp23= _tag_arr( _temp21,
sizeof( unsigned char*), _get_arr_size( argv, sizeof( struct _tagged_arr)));{
unsigned int _temp22= _temp20; unsigned int i; for( i= 0; i <  _temp22; i ++){
_temp21[ i]= 0;}}; _temp23;});{ int i= 0; for( 0; i <  _get_arr_size( argv,
sizeof( struct _tagged_arr)); i ++){*(( unsigned char**)
_check_unknown_subscript( newargs, sizeof( unsigned char*), i))=
string_to_Cstring(( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), i)));}}{ struct
_tagged_arr newenvp=({ unsigned int _temp16= _get_arr_size( envp, sizeof( struct
_tagged_arr)); unsigned char** _temp17=( unsigned char**) _cycalloc(
_check_times( sizeof( unsigned char*), _temp16)); struct _tagged_arr _temp19=
_tag_arr( _temp17, sizeof( unsigned char*), _get_arr_size( envp, sizeof( struct
_tagged_arr)));{ unsigned int _temp18= _temp16; unsigned int i; for( i= 0; i < 
_temp18; i ++){ _temp17[ i]= 0;}}; _temp19;});{ int i= 0; for( 0; i < 
_get_arr_size( envp, sizeof( struct _tagged_arr)); i ++){*(( unsigned char**)
_check_unknown_subscript( newenvp, sizeof( unsigned char*), i))=
string_to_Cstring(( struct _tagged_arr)*(( struct _tagged_arr*)
_check_unknown_subscript( envp, sizeof( struct _tagged_arr), i)));}} return
execve( string_to_Cstring( filename),( unsigned char**) _check_null( _untag_arr(
newargs, sizeof( unsigned char*), 1u)),( unsigned char**) _check_null(
_untag_arr( newenvp, sizeof( unsigned char*), 1u)));}}} int Cyc_Std_link( struct
_tagged_arr path1, struct _tagged_arr path2){ return link( string_to_Cstring(
path1), string_to_Cstring( path2));} int Cyc_Std_read( int fd, struct
_tagged_arr buf, unsigned int count){ if( count >  _get_arr_size( buf, sizeof(
unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp24=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp24[ 0]=({ struct Cyc_Core_Failure_struct _temp25; _temp25.tag= Cyc_Core_Failure;
_temp25.f1= _tag_arr("read: called with count > buf.size", sizeof( unsigned char),
35u); _temp25;}); _temp24;}));} return read( fd, underlying_Cstring(( struct
_tagged_arr) buf), count);} int Cyc_Std_rmdir( struct _tagged_arr path){ return
rmdir( string_to_Cstring( path));} int Cyc_Std_symlink( struct _tagged_arr path1,
struct _tagged_arr path2){ return symlink( string_to_Cstring( path1),
string_to_Cstring( path2));} int Cyc_Std_truncate( struct _tagged_arr path, int
length){ return truncate( string_to_Cstring( path), length);} int Cyc_Std_write(
int fd, struct _tagged_arr buf, unsigned int count){ if( count >  _get_arr_size(
buf, sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp26=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp26[ 0]=({ struct Cyc_Core_Failure_struct _temp27; _temp27.tag= Cyc_Core_Failure;
_temp27.f1= _tag_arr("write: called with count > buf.size", sizeof(
unsigned char), 36u); _temp27;}); _temp26;}));} return write( fd,
underlying_Cstring( buf), count);} int Cyc_Std_unlink( struct _tagged_arr
pathname){ return unlink( string_to_Cstring( pathname));} int Cyc_Std_gethostname(
struct _tagged_arr buf, unsigned int count){ if( count >  _get_arr_size( buf,
sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp28=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp28[ 0]=({ struct Cyc_Core_Failure_struct _temp29; _temp29.tag= Cyc_Core_Failure;
_temp29.f1= _tag_arr("gethostname: called with count > buf.size", sizeof(
unsigned char), 42u); _temp29;}); _temp28;}));} return gethostname(
underlying_Cstring(( struct _tagged_arr) buf), count);} int Cyc_Std_chroot(
struct _tagged_arr pathname){ return chroot( string_to_Cstring( pathname));}
struct _tagged_arr Cyc_Std_getpass( struct _tagged_arr prompt){ return
wrap_Cstring_as_string( getpass( string_to_Cstring( prompt)), - 1);}
