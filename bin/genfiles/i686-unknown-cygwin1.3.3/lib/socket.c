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
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
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
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
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
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char
Cyc_List_Nth[ 8u]; extern struct _tagged_arr Cyc_Std_strconcat( struct
_tagged_arr, struct _tagged_arr); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern struct _tagged_arr Cyc_Std_aprintf( struct _tagged_arr fmt, struct
_tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
int tag; unsigned short* f1; } ; static const int Cyc_Std_IntPtr_sa= 2; struct
Cyc_Std_IntPtr_sa_struct{ int tag; int* f1; } ; static const int Cyc_Std_UIntPtr_sa=
3; struct Cyc_Std_UIntPtr_sa_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_StringPtr_sa= 4; struct Cyc_Std_StringPtr_sa_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{
int tag; double* f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Std_timeval{ int tv_sec; int tv_usec; } ;
struct Cyc_Std_timezone{ int tz_minuteswest; int tz_dsttime; } ; extern int
gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone* __z); struct
Cyc_Std_itimerval{ struct Cyc_Std_timeval it_interval; struct Cyc_Std_timeval
it_value; } ; extern int getitimer( int, struct Cyc_Std_itimerval*); extern int
setitimer( int, const struct Cyc_Std_itimerval*, struct Cyc_Std_itimerval*);
typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t; typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t; struct Cyc_Std_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_Std_timeval; extern int select( int, struct Cyc_Std__types_fd_set*, struct
Cyc_Std__types_fd_set*, struct Cyc_Std__types_fd_set*, struct Cyc_Std_timeval*);
struct Cyc_Std_sockaddr_in; struct Cyc_Std_sockaddr{ unsigned short sa_family;
unsigned char sa_data[ 14u]; } ; static const int Cyc_Std_SA_sockaddr_in= 0;
struct Cyc_Std_SA_sockaddr_in_struct{ int tag; struct Cyc_Std_sockaddr_in* f1; }
; static const int Cyc_Std_SA_sockaddr= 1; struct Cyc_Std_SA_sockaddr_struct{
int tag; struct Cyc_Std_sockaddr* f1; } ; static const int Cyc_Std_SA_socklenptr=
2; struct Cyc_Std_SA_socklenptr_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Std_SA_socklen= 3; struct Cyc_Std_SA_socklen_struct{ int tag;
unsigned int f1; } ; extern int socket( int domain, int type, int protocol);
extern int socketpair( int domain, int type, int protocol, int* fds); extern int
Cyc_Std_accept( int fd, struct _tagged_arr); extern int Cyc_Std_bind( int fd,
struct _tagged_arr); extern int Cyc_Std_connect( int fd, struct _tagged_arr);
extern int Cyc_Std_getpeername( int fd, struct _tagged_arr); extern int Cyc_Std_getsockname(
int fd, struct _tagged_arr); extern int listen( int fd, int n); extern int
shutdown( int fd, int how); extern int Cyc_Std_send( int fd, struct _tagged_arr
buf, unsigned int n, int flags); extern int Cyc_Std_recv( int fd, struct
_tagged_arr buf, unsigned int n, int flags); extern int Cyc_Std_sendto( int fd,
struct _tagged_arr buf, unsigned int n, int flags, struct _tagged_arr); extern
int Cyc_Std_recvfrom( int fd, struct _tagged_arr buf, unsigned int n, int flags,
struct _tagged_arr); static const int Cyc_Std_SO_int= 0; struct Cyc_Std_SO_int_struct{
int tag; int* f1; } ; static const int Cyc_Std_SO_timeval= 1; struct Cyc_Std_SO_timeval_struct{
int tag; struct Cyc_Std_timeval* f1; } ; static const int Cyc_Std_SO_socklenptr=
2; struct Cyc_Std_SO_socklenptr_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Std_SO_socklen= 3; struct Cyc_Std_SO_socklen_struct{ int tag;
unsigned int f1; } ; extern int Cyc_Std_getsockopt( int fd, int level, int
optname, struct _tagged_arr); extern int Cyc_Std_setsockopt( int fd, int level,
int optname, struct _tagged_arr); struct Cyc_Std_in_addr{ unsigned int s_addr; }
; struct Cyc_Std_sockaddr_in{ unsigned short sin_family; unsigned short sin_port;
struct Cyc_Std_in_addr sin_addr; unsigned char sin_zero[ 8u]; } ; extern
unsigned int htonl( unsigned int); extern unsigned short htons( unsigned short);
extern unsigned int ntohl( unsigned int); extern unsigned short ntohs(
unsigned short); extern int accept( int, const struct Cyc_Std_sockaddr*,
unsigned int*); extern int accept_in( int, const struct Cyc_Std_sockaddr_in*,
unsigned int*); extern int bind( int, const struct Cyc_Std_sockaddr*,
unsigned int); extern int bind_in( int, const struct Cyc_Std_sockaddr_in*,
unsigned int); extern int connect( int, const struct Cyc_Std_sockaddr*,
unsigned int); extern int connect_in( int, const struct Cyc_Std_sockaddr_in*,
unsigned int); extern int getpeername( int, const struct Cyc_Std_sockaddr*,
unsigned int*); extern int getpeername_in( int, const struct Cyc_Std_sockaddr_in*,
unsigned int*); extern int getsockname( int, const struct Cyc_Std_sockaddr*,
unsigned int*); extern int getsockname_in( int, const struct Cyc_Std_sockaddr_in*,
unsigned int*); extern int recvfrom( int, struct _tagged_arr, unsigned int, int,
const struct Cyc_Std_sockaddr*, unsigned int*); extern int recvfrom_in( int,
struct _tagged_arr, unsigned int, int, const struct Cyc_Std_sockaddr_in*,
unsigned int*); extern int sendto( int, struct _tagged_arr, unsigned int, int,
const struct Cyc_Std_sockaddr*, unsigned int); extern int sendto_in( int, struct
_tagged_arr, unsigned int, int, const struct Cyc_Std_sockaddr_in*, unsigned int);
extern int send_wrapped( int, struct _tagged_arr, unsigned int, int); extern int
recv_wrapped( int, struct _tagged_arr, unsigned int, int); extern int
getsockopt_int( int, int, int, int*, unsigned int*); extern int setsockopt_int(
int, int, int, const int*, unsigned int); extern int getsockopt_timeval( int,
int, int, struct Cyc_Std_timeval*, unsigned int*); extern int setsockopt_timeval(
int, int, int, const struct Cyc_Std_timeval*, unsigned int); unsigned char Cyc_Std_SocketError[
16u]="\000\000\000\000SocketError"; struct Cyc_Std_SocketError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static struct _tagged_arr Cyc_Std_sopts2string(
struct _tagged_arr args){ struct _tagged_arr res= _tag_arr(({ unsigned char*
_temp10=( unsigned char*) _cycalloc_atomic( sizeof( unsigned char) *  1);
_temp10[ 0]='\000'; _temp10;}), sizeof( unsigned char), 1u);{ int i= 0; for( 0;
i <  _get_arr_size( args, sizeof( void*)); i ++){ void* _temp0=*(( void**)
_check_unknown_subscript( args, sizeof( void*), i)); _LL2: if(*(( int*) _temp0)
==  Cyc_Std_SO_int){ goto _LL3;} else{ goto _LL4;} _LL4: if(*(( int*) _temp0) == 
Cyc_Std_SO_timeval){ goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp0) == 
Cyc_Std_SO_socklenptr){ goto _LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp0)
==  Cyc_Std_SO_socklen){ goto _LL9;} else{ goto _LL1;} _LL3: res= Cyc_Std_strconcat((
struct _tagged_arr) res, _tag_arr("|SO_int", sizeof( unsigned char), 8u)); goto
_LL1; _LL5: res= Cyc_Std_strconcat(( struct _tagged_arr) res, _tag_arr("|SO_timeval",
sizeof( unsigned char), 12u)); goto _LL1; _LL7: res= Cyc_Std_strconcat(( struct
_tagged_arr) res, _tag_arr("|SO_socklenptr", sizeof( unsigned char), 15u)); goto
_LL1; _LL9: res= Cyc_Std_strconcat(( struct _tagged_arr) res, _tag_arr("|SO_socklen",
sizeof( unsigned char), 12u)); goto _LL1; _LL1:;}} return res;} struct _tuple0{
void* f1; void* f2; } ; int Cyc_Std_accept( int fd, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp11=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp11[ 0]=({ struct Cyc_Std_SocketError_struct _temp12; _temp12.tag= Cyc_Std_SocketError;
_temp12.f1= _tag_arr("accept---need 2 args", sizeof( unsigned char), 21u);
_temp12;}); _temp11;}));}{ struct _tuple0 _temp14=({ struct _tuple0 _temp13;
_temp13.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp13.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp13;}); void* _temp22; unsigned int* _temp24; void* _temp26; struct Cyc_Std_sockaddr_in*
_temp28; void* _temp30; unsigned int* _temp32; void* _temp34; struct Cyc_Std_sockaddr*
_temp36; _LL16: _LL27: _temp26= _temp14.f1; if(*(( int*) _temp26) ==  Cyc_Std_SA_sockaddr_in){
_LL29: _temp28=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp26)->f1; goto
_LL23;} else{ goto _LL18;} _LL23: _temp22= _temp14.f2; if(*(( int*) _temp22) == 
Cyc_Std_SA_socklenptr){ _LL25: _temp24=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp22)->f1; goto _LL17;} else{ goto _LL18;} _LL18: _LL35: _temp34= _temp14.f1;
if(*(( int*) _temp34) ==  Cyc_Std_SA_sockaddr){ _LL37: _temp36=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp34)->f1; goto _LL31;} else{ goto _LL20;} _LL31: _temp30= _temp14.f2; if(*((
int*) _temp30) ==  Cyc_Std_SA_socklenptr){ _LL33: _temp32=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp30)->f1; goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL17: return
accept_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp28, _temp24); _LL19:
return accept( fd,( const struct Cyc_Std_sockaddr*) _temp36, _temp32); _LL21:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp38=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp38[ 0]=({ struct
Cyc_Std_SocketError_struct _temp39; _temp39.tag= Cyc_Std_SocketError; _temp39.f1=
_tag_arr("accept---bad args", sizeof( unsigned char), 18u); _temp39;}); _temp38;}));
_LL15:;}} int Cyc_Std_bind( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp40=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp40[ 0]=({ struct Cyc_Std_SocketError_struct _temp41; _temp41.tag= Cyc_Std_SocketError;
_temp41.f1= _tag_arr("bind---need 2 args", sizeof( unsigned char), 19u); _temp41;});
_temp40;}));}{ struct _tuple0 _temp43=({ struct _tuple0 _temp42; _temp42.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp42.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp42;}); void* _temp51;
unsigned int _temp53; void* _temp55; struct Cyc_Std_sockaddr_in* _temp57; void*
_temp59; unsigned int _temp61; void* _temp63; struct Cyc_Std_sockaddr* _temp65;
_LL45: _LL56: _temp55= _temp43.f1; if(*(( int*) _temp55) ==  Cyc_Std_SA_sockaddr_in){
_LL58: _temp57=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp55)->f1; goto
_LL52;} else{ goto _LL47;} _LL52: _temp51= _temp43.f2; if(*(( int*) _temp51) == 
Cyc_Std_SA_socklen){ _LL54: _temp53=(( struct Cyc_Std_SA_socklen_struct*)
_temp51)->f1; goto _LL46;} else{ goto _LL47;} _LL47: _LL64: _temp63= _temp43.f1;
if(*(( int*) _temp63) ==  Cyc_Std_SA_sockaddr){ _LL66: _temp65=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp63)->f1; goto _LL60;} else{ goto _LL49;} _LL60: _temp59= _temp43.f2; if(*((
int*) _temp59) ==  Cyc_Std_SA_socklen){ _LL62: _temp61=(( struct Cyc_Std_SA_socklen_struct*)
_temp59)->f1; goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL46: return
bind_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp57, _temp53); _LL48:
return bind( fd,( const struct Cyc_Std_sockaddr*) _temp65, _temp61); _LL50:( int)
_throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp67=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp67[ 0]=({ struct
Cyc_Std_SocketError_struct _temp68; _temp68.tag= Cyc_Std_SocketError; _temp68.f1=
_tag_arr("bind---bad args", sizeof( unsigned char), 16u); _temp68;}); _temp67;}));
_LL44:;}} int Cyc_Std_connect( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp69=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp69[ 0]=({ struct Cyc_Std_SocketError_struct _temp70; _temp70.tag= Cyc_Std_SocketError;
_temp70.f1= _tag_arr("connect---need 2 args", sizeof( unsigned char), 22u);
_temp70;}); _temp69;}));}{ struct _tuple0 _temp72=({ struct _tuple0 _temp71;
_temp71.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp71.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp71;}); void* _temp80; unsigned int _temp82; void* _temp84; struct Cyc_Std_sockaddr_in*
_temp86; void* _temp88; unsigned int _temp90; void* _temp92; struct Cyc_Std_sockaddr*
_temp94; _LL74: _LL85: _temp84= _temp72.f1; if(*(( int*) _temp84) ==  Cyc_Std_SA_sockaddr_in){
_LL87: _temp86=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp84)->f1; goto
_LL81;} else{ goto _LL76;} _LL81: _temp80= _temp72.f2; if(*(( int*) _temp80) == 
Cyc_Std_SA_socklen){ _LL83: _temp82=(( struct Cyc_Std_SA_socklen_struct*)
_temp80)->f1; goto _LL75;} else{ goto _LL76;} _LL76: _LL93: _temp92= _temp72.f1;
if(*(( int*) _temp92) ==  Cyc_Std_SA_sockaddr){ _LL95: _temp94=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp92)->f1; goto _LL89;} else{ goto _LL78;} _LL89: _temp88= _temp72.f2; if(*((
int*) _temp88) ==  Cyc_Std_SA_socklen){ _LL91: _temp90=(( struct Cyc_Std_SA_socklen_struct*)
_temp88)->f1; goto _LL77;} else{ goto _LL78;} _LL78: goto _LL79; _LL75: return
connect_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp86, _temp82); _LL77:
return connect( fd,( const struct Cyc_Std_sockaddr*) _temp94, _temp90); _LL79:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp96=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp96[ 0]=({ struct
Cyc_Std_SocketError_struct _temp97; _temp97.tag= Cyc_Std_SocketError; _temp97.f1=
_tag_arr("connect---bad args", sizeof( unsigned char), 19u); _temp97;}); _temp96;}));
_LL73:;}} int Cyc_Std_getpeername( int fd, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp98=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp98[ 0]=({ struct Cyc_Std_SocketError_struct _temp99; _temp99.tag= Cyc_Std_SocketError;
_temp99.f1= _tag_arr("getpeername---need 2 args", sizeof( unsigned char), 26u);
_temp99;}); _temp98;}));}{ struct _tuple0 _temp101=({ struct _tuple0 _temp100;
_temp100.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp100.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp100;}); void* _temp109; unsigned int* _temp111; void* _temp113; struct Cyc_Std_sockaddr_in*
_temp115; void* _temp117; unsigned int* _temp119; void* _temp121; struct Cyc_Std_sockaddr*
_temp123; _LL103: _LL114: _temp113= _temp101.f1; if(*(( int*) _temp113) ==  Cyc_Std_SA_sockaddr_in){
_LL116: _temp115=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp113)->f1; goto
_LL110;} else{ goto _LL105;} _LL110: _temp109= _temp101.f2; if(*(( int*)
_temp109) ==  Cyc_Std_SA_socklenptr){ _LL112: _temp111=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp109)->f1; goto _LL104;} else{ goto _LL105;} _LL105: _LL122: _temp121=
_temp101.f1; if(*(( int*) _temp121) ==  Cyc_Std_SA_sockaddr){ _LL124: _temp123=((
struct Cyc_Std_SA_sockaddr_struct*) _temp121)->f1; goto _LL118;} else{ goto
_LL107;} _LL118: _temp117= _temp101.f2; if(*(( int*) _temp117) ==  Cyc_Std_SA_socklenptr){
_LL120: _temp119=(( struct Cyc_Std_SA_socklenptr_struct*) _temp117)->f1; goto
_LL106;} else{ goto _LL107;} _LL107: goto _LL108; _LL104: return getpeername_in(
fd,( const struct Cyc_Std_sockaddr_in*) _temp115, _temp111); _LL106: return
getpeername( fd,( const struct Cyc_Std_sockaddr*) _temp123, _temp119); _LL108:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp125=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp125[ 0]=({ struct
Cyc_Std_SocketError_struct _temp126; _temp126.tag= Cyc_Std_SocketError; _temp126.f1=
_tag_arr("getpeername---bad args", sizeof( unsigned char), 23u); _temp126;});
_temp125;})); _LL102:;}} int Cyc_Std_getsockname( int fd, struct _tagged_arr ap){
if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct
Cyc_Std_SocketError_struct* _temp127=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp127[ 0]=({ struct
Cyc_Std_SocketError_struct _temp128; _temp128.tag= Cyc_Std_SocketError; _temp128.f1=
_tag_arr("getsockname---need 2 args", sizeof( unsigned char), 26u); _temp128;});
_temp127;}));}{ struct _tuple0 _temp130=({ struct _tuple0 _temp129; _temp129.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp129.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp129;}); void* _temp138;
unsigned int* _temp140; void* _temp142; struct Cyc_Std_sockaddr_in* _temp144;
void* _temp146; unsigned int* _temp148; void* _temp150; struct Cyc_Std_sockaddr*
_temp152; _LL132: _LL143: _temp142= _temp130.f1; if(*(( int*) _temp142) ==  Cyc_Std_SA_sockaddr_in){
_LL145: _temp144=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp142)->f1; goto
_LL139;} else{ goto _LL134;} _LL139: _temp138= _temp130.f2; if(*(( int*)
_temp138) ==  Cyc_Std_SA_socklenptr){ _LL141: _temp140=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp138)->f1; goto _LL133;} else{ goto _LL134;} _LL134: _LL151: _temp150=
_temp130.f1; if(*(( int*) _temp150) ==  Cyc_Std_SA_sockaddr){ _LL153: _temp152=((
struct Cyc_Std_SA_sockaddr_struct*) _temp150)->f1; goto _LL147;} else{ goto
_LL136;} _LL147: _temp146= _temp130.f2; if(*(( int*) _temp146) ==  Cyc_Std_SA_socklenptr){
_LL149: _temp148=(( struct Cyc_Std_SA_socklenptr_struct*) _temp146)->f1; goto
_LL135;} else{ goto _LL136;} _LL136: goto _LL137; _LL133: return getsockname_in(
fd,( const struct Cyc_Std_sockaddr_in*) _temp144, _temp140); _LL135: return
getsockname( fd,( const struct Cyc_Std_sockaddr*) _temp152, _temp148); _LL137:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp154=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp154[ 0]=({ struct
Cyc_Std_SocketError_struct _temp155; _temp155.tag= Cyc_Std_SocketError; _temp155.f1=
_tag_arr("getsockname---bad args", sizeof( unsigned char), 23u); _temp155;});
_temp154;})); _LL131:;}} int Cyc_Std_recvfrom( int fd, struct _tagged_arr buf,
unsigned int n, int flags, struct _tagged_arr ap){ if( _get_arr_size( ap,
sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp156=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp156[ 0]=({ struct Cyc_Std_SocketError_struct _temp157; _temp157.tag= Cyc_Std_SocketError;
_temp157.f1= _tag_arr("recvfrom---need 2 args", sizeof( unsigned char), 23u);
_temp157;}); _temp156;}));}{ struct _tuple0 _temp159=({ struct _tuple0 _temp158;
_temp158.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp158.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp158;}); void* _temp167; unsigned int* _temp169; void* _temp171; struct Cyc_Std_sockaddr_in*
_temp173; void* _temp175; unsigned int* _temp177; void* _temp179; struct Cyc_Std_sockaddr*
_temp181; _LL161: _LL172: _temp171= _temp159.f1; if(*(( int*) _temp171) ==  Cyc_Std_SA_sockaddr_in){
_LL174: _temp173=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp171)->f1; goto
_LL168;} else{ goto _LL163;} _LL168: _temp167= _temp159.f2; if(*(( int*)
_temp167) ==  Cyc_Std_SA_socklenptr){ _LL170: _temp169=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp167)->f1; goto _LL162;} else{ goto _LL163;} _LL163: _LL180: _temp179=
_temp159.f1; if(*(( int*) _temp179) ==  Cyc_Std_SA_sockaddr){ _LL182: _temp181=((
struct Cyc_Std_SA_sockaddr_struct*) _temp179)->f1; goto _LL176;} else{ goto
_LL165;} _LL176: _temp175= _temp159.f2; if(*(( int*) _temp175) ==  Cyc_Std_SA_socklenptr){
_LL178: _temp177=(( struct Cyc_Std_SA_socklenptr_struct*) _temp175)->f1; goto
_LL164;} else{ goto _LL165;} _LL165: goto _LL166; _LL162: return recvfrom_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp173, _temp169); _LL164:
return recvfrom( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp181,
_temp177); _LL166:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp183=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp183[ 0]=({ struct Cyc_Std_SocketError_struct _temp184; _temp184.tag= Cyc_Std_SocketError;
_temp184.f1= _tag_arr("recvfrom---bad args", sizeof( unsigned char), 20u);
_temp184;}); _temp183;})); _LL160:;}} int Cyc_Std_sendto( int fd, struct
_tagged_arr buf, unsigned int n, int flags, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp185=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp185[ 0]=({ struct Cyc_Std_SocketError_struct _temp186; _temp186.tag= Cyc_Std_SocketError;
_temp186.f1= _tag_arr("sendto---need 2 args", sizeof( unsigned char), 21u);
_temp186;}); _temp185;}));}{ struct _tuple0 _temp188=({ struct _tuple0 _temp187;
_temp187.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp187.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp187;}); void* _temp196; unsigned int _temp198; void* _temp200; struct Cyc_Std_sockaddr_in*
_temp202; void* _temp204; unsigned int _temp206; void* _temp208; struct Cyc_Std_sockaddr*
_temp210; _LL190: _LL201: _temp200= _temp188.f1; if(*(( int*) _temp200) ==  Cyc_Std_SA_sockaddr_in){
_LL203: _temp202=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp200)->f1; goto
_LL197;} else{ goto _LL192;} _LL197: _temp196= _temp188.f2; if(*(( int*)
_temp196) ==  Cyc_Std_SA_socklen){ _LL199: _temp198=(( struct Cyc_Std_SA_socklen_struct*)
_temp196)->f1; goto _LL191;} else{ goto _LL192;} _LL192: _LL209: _temp208=
_temp188.f1; if(*(( int*) _temp208) ==  Cyc_Std_SA_sockaddr){ _LL211: _temp210=((
struct Cyc_Std_SA_sockaddr_struct*) _temp208)->f1; goto _LL205;} else{ goto
_LL194;} _LL205: _temp204= _temp188.f2; if(*(( int*) _temp204) ==  Cyc_Std_SA_socklen){
_LL207: _temp206=(( struct Cyc_Std_SA_socklen_struct*) _temp204)->f1; goto
_LL193;} else{ goto _LL194;} _LL194: goto _LL195; _LL191: return sendto_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp202, _temp198); _LL193:
return sendto( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp210,
_temp206); _LL195:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp212=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp212[ 0]=({ struct Cyc_Std_SocketError_struct _temp213; _temp213.tag= Cyc_Std_SocketError;
_temp213.f1= _tag_arr("sendto---bad args", sizeof( unsigned char), 18u);
_temp213;}); _temp212;})); _LL189:;}} int Cyc_Std_send( int fd, struct
_tagged_arr buf, unsigned int n, int flags){ return send_wrapped( fd, buf, n,
flags);} int Cyc_Std_recv( int fd, struct _tagged_arr buf, unsigned int n, int
flags){ return recv_wrapped( fd, buf, n, flags);} int Cyc_Std_getsockopt( int fd,
int level, int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof(
void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp214=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp214[ 0]=({ struct Cyc_Std_SocketError_struct _temp215; _temp215.tag= Cyc_Std_SocketError;
_temp215.f1= _tag_arr("getsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp215;}); _temp214;}));}{ struct _tuple0 _temp217=({ struct _tuple0 _temp216;
_temp216.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp216.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp216;}); void* _temp229; unsigned int* _temp231; void* _temp233; int*
_temp235; void* _temp237; int* _temp239; void* _temp241; int* _temp243; void*
_temp245; unsigned int* _temp247; void* _temp249; struct Cyc_Std_timeval*
_temp251; void* _temp253; int* _temp255; void* _temp257; struct Cyc_Std_timeval*
_temp259; _LL219: _LL234: _temp233= _temp217.f1; if(*(( int*) _temp233) ==  Cyc_Std_SO_int){
_LL236: _temp235=(( struct Cyc_Std_SO_int_struct*) _temp233)->f1; goto _LL230;}
else{ goto _LL221;} _LL230: _temp229= _temp217.f2; if(*(( int*) _temp229) == 
Cyc_Std_SO_socklenptr){ _LL232: _temp231=(( struct Cyc_Std_SO_socklenptr_struct*)
_temp229)->f1; goto _LL220;} else{ goto _LL221;} _LL221: _LL242: _temp241=
_temp217.f1; if(*(( int*) _temp241) ==  Cyc_Std_SO_int){ _LL244: _temp243=((
struct Cyc_Std_SO_int_struct*) _temp241)->f1; goto _LL238;} else{ goto _LL223;}
_LL238: _temp237= _temp217.f2; if(*(( int*) _temp237) ==  Cyc_Std_SO_int){
_LL240: _temp239=(( struct Cyc_Std_SO_int_struct*) _temp237)->f1; goto _LL222;}
else{ goto _LL223;} _LL223: _LL250: _temp249= _temp217.f1; if(*(( int*) _temp249)
==  Cyc_Std_SO_timeval){ _LL252: _temp251=(( struct Cyc_Std_SO_timeval_struct*)
_temp249)->f1; goto _LL246;} else{ goto _LL225;} _LL246: _temp245= _temp217.f2;
if(*(( int*) _temp245) ==  Cyc_Std_SO_socklenptr){ _LL248: _temp247=(( struct
Cyc_Std_SO_socklenptr_struct*) _temp245)->f1; goto _LL224;} else{ goto _LL225;}
_LL225: _LL258: _temp257= _temp217.f1; if(*(( int*) _temp257) ==  Cyc_Std_SO_timeval){
_LL260: _temp259=(( struct Cyc_Std_SO_timeval_struct*) _temp257)->f1; goto
_LL254;} else{ goto _LL227;} _LL254: _temp253= _temp217.f2; if(*(( int*)
_temp253) ==  Cyc_Std_SO_int){ _LL256: _temp255=(( struct Cyc_Std_SO_int_struct*)
_temp253)->f1; goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL220:
return getsockopt_int( fd, level, optname, _temp235, _temp231); _LL222: return
getsockopt_int( fd, level, optname, _temp243,( unsigned int*) _temp239); _LL224:
return getsockopt_timeval( fd, level, optname, _temp251, _temp247); _LL226:
return getsockopt_timeval( fd, level, optname, _temp259,( unsigned int*)
_temp255); _LL228:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp261=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp261[ 0]=({ struct Cyc_Std_SocketError_struct _temp262; _temp262.tag= Cyc_Std_SocketError;
_temp262.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp264;
_temp264.tag= Cyc_Std_String_pa; _temp264.f1=( struct _tagged_arr) Cyc_Std_sopts2string(
ap);{ void* _temp263[ 1u]={& _temp264}; Cyc_Std_aprintf( _tag_arr("getsockopt---bad args %s",
sizeof( unsigned char), 25u), _tag_arr( _temp263, sizeof( void*), 1u));}});
_temp262;}); _temp261;})); _LL218:;}} int Cyc_Std_setsockopt( int fd, int level,
int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) != 
2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp265=( struct
Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp265[ 0]=({ struct Cyc_Std_SocketError_struct _temp266; _temp266.tag= Cyc_Std_SocketError;
_temp266.f1= _tag_arr("setsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp266;}); _temp265;}));}{ struct _tuple0 _temp268=({ struct _tuple0 _temp267;
_temp267.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp267.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp267;}); void* _temp276; unsigned int _temp278; void* _temp280; int*
_temp282; void* _temp284; unsigned int _temp286; void* _temp288; struct Cyc_Std_timeval*
_temp290; _LL270: _LL281: _temp280= _temp268.f1; if(*(( int*) _temp280) ==  Cyc_Std_SO_int){
_LL283: _temp282=(( struct Cyc_Std_SO_int_struct*) _temp280)->f1; goto _LL277;}
else{ goto _LL272;} _LL277: _temp276= _temp268.f2; if(*(( int*) _temp276) == 
Cyc_Std_SO_socklen){ _LL279: _temp278=(( struct Cyc_Std_SO_socklen_struct*)
_temp276)->f1; goto _LL271;} else{ goto _LL272;} _LL272: _LL289: _temp288=
_temp268.f1; if(*(( int*) _temp288) ==  Cyc_Std_SO_timeval){ _LL291: _temp290=((
struct Cyc_Std_SO_timeval_struct*) _temp288)->f1; goto _LL285;} else{ goto
_LL274;} _LL285: _temp284= _temp268.f2; if(*(( int*) _temp284) ==  Cyc_Std_SO_socklen){
_LL287: _temp286=(( struct Cyc_Std_SO_socklen_struct*) _temp284)->f1; goto
_LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL271: return setsockopt_int(
fd, level, optname,( const int*) _temp282, _temp278); _LL273: return
setsockopt_timeval( fd, level, optname,( const struct Cyc_Std_timeval*) _temp290,
_temp286); _LL275:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp292=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp292[ 0]=({ struct Cyc_Std_SocketError_struct _temp293; _temp293.tag= Cyc_Std_SocketError;
_temp293.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp295;
_temp295.tag= Cyc_Std_String_pa; _temp295.f1=( struct _tagged_arr) Cyc_Std_sopts2string(
ap);{ void* _temp294[ 1u]={& _temp295}; Cyc_Std_aprintf( _tag_arr("getsockopt---bad args %s",
sizeof( unsigned char), 25u), _tag_arr( _temp294, sizeof( void*), 1u));}});
_temp293;}); _temp292;})); _LL269:;}}
