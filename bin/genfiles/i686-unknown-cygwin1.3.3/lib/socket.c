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
unsigned char* tag; struct _tagged_arr f1; } ; struct _tuple0{ void* f1; void*
f2; } ; int Cyc_Std_accept( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp0=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp0[ 0]=({ struct Cyc_Std_SocketError_struct _temp1; _temp1.tag= Cyc_Std_SocketError;
_temp1.f1= _tag_arr("accept---need 2 args", sizeof( unsigned char), 21u); _temp1;});
_temp0;}));}{ struct _tuple0 _temp3=({ struct _tuple0 _temp2; _temp2.f1=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0)); _temp2.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp2;}); void* _temp11;
unsigned int* _temp13; void* _temp15; struct Cyc_Std_sockaddr_in* _temp17; void*
_temp19; unsigned int* _temp21; void* _temp23; struct Cyc_Std_sockaddr* _temp25;
_LL5: _LL16: _temp15= _temp3.f1; if(*(( int*) _temp15) ==  Cyc_Std_SA_sockaddr_in){
_LL18: _temp17=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp15)->f1; goto
_LL12;} else{ goto _LL7;} _LL12: _temp11= _temp3.f2; if(*(( int*) _temp11) == 
Cyc_Std_SA_socklenptr){ _LL14: _temp13=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp11)->f1; goto _LL6;} else{ goto _LL7;} _LL7: _LL24: _temp23= _temp3.f1; if(*((
int*) _temp23) ==  Cyc_Std_SA_sockaddr){ _LL26: _temp25=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp23)->f1; goto _LL20;} else{ goto _LL9;} _LL20: _temp19= _temp3.f2; if(*((
int*) _temp19) ==  Cyc_Std_SA_socklenptr){ _LL22: _temp21=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp19)->f1; goto _LL8;} else{ goto _LL9;} _LL9: goto _LL10; _LL6: return
accept_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp17, _temp13); _LL8:
return accept( fd,( const struct Cyc_Std_sockaddr*) _temp25, _temp21); _LL10:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp27=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp27[ 0]=({ struct
Cyc_Std_SocketError_struct _temp28; _temp28.tag= Cyc_Std_SocketError; _temp28.f1=
_tag_arr("accept---bad args", sizeof( unsigned char), 18u); _temp28;}); _temp27;}));
_LL4:;}} int Cyc_Std_bind( int fd, struct _tagged_arr ap){ if( _get_arr_size( ap,
sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp29=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp29[ 0]=({ struct Cyc_Std_SocketError_struct _temp30; _temp30.tag= Cyc_Std_SocketError;
_temp30.f1= _tag_arr("bind---need 2 args", sizeof( unsigned char), 19u); _temp30;});
_temp29;}));}{ struct _tuple0 _temp32=({ struct _tuple0 _temp31; _temp31.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp31.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp31;}); void* _temp40;
unsigned int _temp42; void* _temp44; struct Cyc_Std_sockaddr_in* _temp46; void*
_temp48; unsigned int _temp50; void* _temp52; struct Cyc_Std_sockaddr* _temp54;
_LL34: _LL45: _temp44= _temp32.f1; if(*(( int*) _temp44) ==  Cyc_Std_SA_sockaddr_in){
_LL47: _temp46=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp44)->f1; goto
_LL41;} else{ goto _LL36;} _LL41: _temp40= _temp32.f2; if(*(( int*) _temp40) == 
Cyc_Std_SA_socklen){ _LL43: _temp42=(( struct Cyc_Std_SA_socklen_struct*)
_temp40)->f1; goto _LL35;} else{ goto _LL36;} _LL36: _LL53: _temp52= _temp32.f1;
if(*(( int*) _temp52) ==  Cyc_Std_SA_sockaddr){ _LL55: _temp54=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp52)->f1; goto _LL49;} else{ goto _LL38;} _LL49: _temp48= _temp32.f2; if(*((
int*) _temp48) ==  Cyc_Std_SA_socklen){ _LL51: _temp50=(( struct Cyc_Std_SA_socklen_struct*)
_temp48)->f1; goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL35: return
bind_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp46, _temp42); _LL37:
return bind( fd,( const struct Cyc_Std_sockaddr*) _temp54, _temp50); _LL39:( int)
_throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp56=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp56[ 0]=({ struct
Cyc_Std_SocketError_struct _temp57; _temp57.tag= Cyc_Std_SocketError; _temp57.f1=
_tag_arr("bind---bad args", sizeof( unsigned char), 16u); _temp57;}); _temp56;}));
_LL33:;}} int Cyc_Std_connect( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp58=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp58[ 0]=({ struct Cyc_Std_SocketError_struct _temp59; _temp59.tag= Cyc_Std_SocketError;
_temp59.f1= _tag_arr("connect---need 2 args", sizeof( unsigned char), 22u);
_temp59;}); _temp58;}));}{ struct _tuple0 _temp61=({ struct _tuple0 _temp60;
_temp60.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp60.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp60;}); void* _temp69; unsigned int _temp71; void* _temp73; struct Cyc_Std_sockaddr_in*
_temp75; void* _temp77; unsigned int _temp79; void* _temp81; struct Cyc_Std_sockaddr*
_temp83; _LL63: _LL74: _temp73= _temp61.f1; if(*(( int*) _temp73) ==  Cyc_Std_SA_sockaddr_in){
_LL76: _temp75=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp73)->f1; goto
_LL70;} else{ goto _LL65;} _LL70: _temp69= _temp61.f2; if(*(( int*) _temp69) == 
Cyc_Std_SA_socklen){ _LL72: _temp71=(( struct Cyc_Std_SA_socklen_struct*)
_temp69)->f1; goto _LL64;} else{ goto _LL65;} _LL65: _LL82: _temp81= _temp61.f1;
if(*(( int*) _temp81) ==  Cyc_Std_SA_sockaddr){ _LL84: _temp83=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp81)->f1; goto _LL78;} else{ goto _LL67;} _LL78: _temp77= _temp61.f2; if(*((
int*) _temp77) ==  Cyc_Std_SA_socklen){ _LL80: _temp79=(( struct Cyc_Std_SA_socklen_struct*)
_temp77)->f1; goto _LL66;} else{ goto _LL67;} _LL67: goto _LL68; _LL64: return
connect_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp75, _temp71); _LL66:
return connect( fd,( const struct Cyc_Std_sockaddr*) _temp83, _temp79); _LL68:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp85=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp85[ 0]=({ struct
Cyc_Std_SocketError_struct _temp86; _temp86.tag= Cyc_Std_SocketError; _temp86.f1=
_tag_arr("connect---bad args", sizeof( unsigned char), 19u); _temp86;}); _temp85;}));
_LL62:;}} int Cyc_Std_getpeername( int fd, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp87=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp87[ 0]=({ struct Cyc_Std_SocketError_struct _temp88; _temp88.tag= Cyc_Std_SocketError;
_temp88.f1= _tag_arr("getpeername---need 2 args", sizeof( unsigned char), 26u);
_temp88;}); _temp87;}));}{ struct _tuple0 _temp90=({ struct _tuple0 _temp89;
_temp89.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp89.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp89;}); void* _temp98; unsigned int* _temp100; void* _temp102; struct Cyc_Std_sockaddr_in*
_temp104; void* _temp106; unsigned int* _temp108; void* _temp110; struct Cyc_Std_sockaddr*
_temp112; _LL92: _LL103: _temp102= _temp90.f1; if(*(( int*) _temp102) ==  Cyc_Std_SA_sockaddr_in){
_LL105: _temp104=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp102)->f1; goto
_LL99;} else{ goto _LL94;} _LL99: _temp98= _temp90.f2; if(*(( int*) _temp98) == 
Cyc_Std_SA_socklenptr){ _LL101: _temp100=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp98)->f1; goto _LL93;} else{ goto _LL94;} _LL94: _LL111: _temp110= _temp90.f1;
if(*(( int*) _temp110) ==  Cyc_Std_SA_sockaddr){ _LL113: _temp112=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp110)->f1; goto _LL107;} else{ goto _LL96;} _LL107: _temp106= _temp90.f2;
if(*(( int*) _temp106) ==  Cyc_Std_SA_socklenptr){ _LL109: _temp108=(( struct
Cyc_Std_SA_socklenptr_struct*) _temp106)->f1; goto _LL95;} else{ goto _LL96;}
_LL96: goto _LL97; _LL93: return getpeername_in( fd,( const struct Cyc_Std_sockaddr_in*)
_temp104, _temp100); _LL95: return getpeername( fd,( const struct Cyc_Std_sockaddr*)
_temp112, _temp108); _LL97:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp114=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp114[ 0]=({ struct Cyc_Std_SocketError_struct _temp115; _temp115.tag= Cyc_Std_SocketError;
_temp115.f1= _tag_arr("getpeername---bad args", sizeof( unsigned char), 23u);
_temp115;}); _temp114;})); _LL91:;}} int Cyc_Std_getsockname( int fd, struct
_tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw((
void*)({ struct Cyc_Std_SocketError_struct* _temp116=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp116[ 0]=({ struct
Cyc_Std_SocketError_struct _temp117; _temp117.tag= Cyc_Std_SocketError; _temp117.f1=
_tag_arr("getsockname---need 2 args", sizeof( unsigned char), 26u); _temp117;});
_temp116;}));}{ struct _tuple0 _temp119=({ struct _tuple0 _temp118; _temp118.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp118.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp118;}); void* _temp127;
unsigned int* _temp129; void* _temp131; struct Cyc_Std_sockaddr_in* _temp133;
void* _temp135; unsigned int* _temp137; void* _temp139; struct Cyc_Std_sockaddr*
_temp141; _LL121: _LL132: _temp131= _temp119.f1; if(*(( int*) _temp131) ==  Cyc_Std_SA_sockaddr_in){
_LL134: _temp133=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp131)->f1; goto
_LL128;} else{ goto _LL123;} _LL128: _temp127= _temp119.f2; if(*(( int*)
_temp127) ==  Cyc_Std_SA_socklenptr){ _LL130: _temp129=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp127)->f1; goto _LL122;} else{ goto _LL123;} _LL123: _LL140: _temp139=
_temp119.f1; if(*(( int*) _temp139) ==  Cyc_Std_SA_sockaddr){ _LL142: _temp141=((
struct Cyc_Std_SA_sockaddr_struct*) _temp139)->f1; goto _LL136;} else{ goto
_LL125;} _LL136: _temp135= _temp119.f2; if(*(( int*) _temp135) ==  Cyc_Std_SA_socklenptr){
_LL138: _temp137=(( struct Cyc_Std_SA_socklenptr_struct*) _temp135)->f1; goto
_LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL122: return getsockname_in(
fd,( const struct Cyc_Std_sockaddr_in*) _temp133, _temp129); _LL124: return
getsockname( fd,( const struct Cyc_Std_sockaddr*) _temp141, _temp137); _LL126:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp143=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp143[ 0]=({ struct
Cyc_Std_SocketError_struct _temp144; _temp144.tag= Cyc_Std_SocketError; _temp144.f1=
_tag_arr("getsockname---bad args", sizeof( unsigned char), 23u); _temp144;});
_temp143;})); _LL120:;}} int Cyc_Std_recvfrom( int fd, struct _tagged_arr buf,
unsigned int n, int flags, struct _tagged_arr ap){ if( _get_arr_size( ap,
sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp145=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp145[ 0]=({ struct Cyc_Std_SocketError_struct _temp146; _temp146.tag= Cyc_Std_SocketError;
_temp146.f1= _tag_arr("recvfrom---need 2 args", sizeof( unsigned char), 23u);
_temp146;}); _temp145;}));}{ struct _tuple0 _temp148=({ struct _tuple0 _temp147;
_temp147.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp147.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp147;}); void* _temp156; unsigned int* _temp158; void* _temp160; struct Cyc_Std_sockaddr_in*
_temp162; void* _temp164; unsigned int* _temp166; void* _temp168; struct Cyc_Std_sockaddr*
_temp170; _LL150: _LL161: _temp160= _temp148.f1; if(*(( int*) _temp160) ==  Cyc_Std_SA_sockaddr_in){
_LL163: _temp162=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp160)->f1; goto
_LL157;} else{ goto _LL152;} _LL157: _temp156= _temp148.f2; if(*(( int*)
_temp156) ==  Cyc_Std_SA_socklenptr){ _LL159: _temp158=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp156)->f1; goto _LL151;} else{ goto _LL152;} _LL152: _LL169: _temp168=
_temp148.f1; if(*(( int*) _temp168) ==  Cyc_Std_SA_sockaddr){ _LL171: _temp170=((
struct Cyc_Std_SA_sockaddr_struct*) _temp168)->f1; goto _LL165;} else{ goto
_LL154;} _LL165: _temp164= _temp148.f2; if(*(( int*) _temp164) ==  Cyc_Std_SA_socklenptr){
_LL167: _temp166=(( struct Cyc_Std_SA_socklenptr_struct*) _temp164)->f1; goto
_LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL151: return recvfrom_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp162, _temp158); _LL153:
return recvfrom( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp170,
_temp166); _LL155:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp172=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp172[ 0]=({ struct Cyc_Std_SocketError_struct _temp173; _temp173.tag= Cyc_Std_SocketError;
_temp173.f1= _tag_arr("recvfrom---bad args", sizeof( unsigned char), 20u);
_temp173;}); _temp172;})); _LL149:;}} int Cyc_Std_sendto( int fd, struct
_tagged_arr buf, unsigned int n, int flags, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp174=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp174[ 0]=({ struct Cyc_Std_SocketError_struct _temp175; _temp175.tag= Cyc_Std_SocketError;
_temp175.f1= _tag_arr("sendto---need 2 args", sizeof( unsigned char), 21u);
_temp175;}); _temp174;}));}{ struct _tuple0 _temp177=({ struct _tuple0 _temp176;
_temp176.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp176.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp176;}); void* _temp185; unsigned int _temp187; void* _temp189; struct Cyc_Std_sockaddr_in*
_temp191; void* _temp193; unsigned int _temp195; void* _temp197; struct Cyc_Std_sockaddr*
_temp199; _LL179: _LL190: _temp189= _temp177.f1; if(*(( int*) _temp189) ==  Cyc_Std_SA_sockaddr_in){
_LL192: _temp191=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp189)->f1; goto
_LL186;} else{ goto _LL181;} _LL186: _temp185= _temp177.f2; if(*(( int*)
_temp185) ==  Cyc_Std_SA_socklen){ _LL188: _temp187=(( struct Cyc_Std_SA_socklen_struct*)
_temp185)->f1; goto _LL180;} else{ goto _LL181;} _LL181: _LL198: _temp197=
_temp177.f1; if(*(( int*) _temp197) ==  Cyc_Std_SA_sockaddr){ _LL200: _temp199=((
struct Cyc_Std_SA_sockaddr_struct*) _temp197)->f1; goto _LL194;} else{ goto
_LL183;} _LL194: _temp193= _temp177.f2; if(*(( int*) _temp193) ==  Cyc_Std_SA_socklen){
_LL196: _temp195=(( struct Cyc_Std_SA_socklen_struct*) _temp193)->f1; goto
_LL182;} else{ goto _LL183;} _LL183: goto _LL184; _LL180: return sendto_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp191, _temp187); _LL182:
return sendto( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp199,
_temp195); _LL184:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp201=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp201[ 0]=({ struct Cyc_Std_SocketError_struct _temp202; _temp202.tag= Cyc_Std_SocketError;
_temp202.f1= _tag_arr("sendto---bad args", sizeof( unsigned char), 18u);
_temp202;}); _temp201;})); _LL178:;}} int Cyc_Std_send( int fd, struct
_tagged_arr buf, unsigned int n, int flags){ return send_wrapped( fd, buf, n,
flags);} int Cyc_Std_recv( int fd, struct _tagged_arr buf, unsigned int n, int
flags){ return recv_wrapped( fd, buf, n, flags);} int Cyc_Std_getsockopt( int fd,
int level, int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof(
void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp203=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp203[ 0]=({ struct Cyc_Std_SocketError_struct _temp204; _temp204.tag= Cyc_Std_SocketError;
_temp204.f1= _tag_arr("getsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp204;}); _temp203;}));}{ struct _tuple0 _temp206=({ struct _tuple0 _temp205;
_temp205.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp205.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp205;}); void* _temp214; unsigned int* _temp216; void* _temp218; int*
_temp220; void* _temp222; unsigned int* _temp224; void* _temp226; struct Cyc_Std_timeval*
_temp228; _LL208: _LL219: _temp218= _temp206.f1; if(*(( int*) _temp218) ==  Cyc_Std_SO_int){
_LL221: _temp220=(( struct Cyc_Std_SO_int_struct*) _temp218)->f1; goto _LL215;}
else{ goto _LL210;} _LL215: _temp214= _temp206.f2; if(*(( int*) _temp214) == 
Cyc_Std_SO_socklenptr){ _LL217: _temp216=(( struct Cyc_Std_SO_socklenptr_struct*)
_temp214)->f1; goto _LL209;} else{ goto _LL210;} _LL210: _LL227: _temp226=
_temp206.f1; if(*(( int*) _temp226) ==  Cyc_Std_SO_timeval){ _LL229: _temp228=((
struct Cyc_Std_SO_timeval_struct*) _temp226)->f1; goto _LL223;} else{ goto
_LL212;} _LL223: _temp222= _temp206.f2; if(*(( int*) _temp222) ==  Cyc_Std_SO_socklenptr){
_LL225: _temp224=(( struct Cyc_Std_SO_socklenptr_struct*) _temp222)->f1; goto
_LL211;} else{ goto _LL212;} _LL212: goto _LL213; _LL209: return getsockopt_int(
fd, level, optname, _temp220, _temp216); _LL211: return getsockopt_timeval( fd,
level, optname, _temp228, _temp224); _LL213:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp230=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp230[ 0]=({ struct Cyc_Std_SocketError_struct _temp231; _temp231.tag= Cyc_Std_SocketError;
_temp231.f1= _tag_arr("getsockopt---bad args", sizeof( unsigned char), 22u);
_temp231;}); _temp230;})); _LL207:;}} int Cyc_Std_setsockopt( int fd, int level,
int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) != 
2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp232=( struct
Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp232[ 0]=({ struct Cyc_Std_SocketError_struct _temp233; _temp233.tag= Cyc_Std_SocketError;
_temp233.f1= _tag_arr("setsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp233;}); _temp232;}));}{ struct _tuple0 _temp235=({ struct _tuple0 _temp234;
_temp234.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp234.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp234;}); void* _temp243; unsigned int _temp245; void* _temp247; int*
_temp249; void* _temp251; unsigned int _temp253; void* _temp255; struct Cyc_Std_timeval*
_temp257; _LL237: _LL248: _temp247= _temp235.f1; if(*(( int*) _temp247) ==  Cyc_Std_SO_int){
_LL250: _temp249=(( struct Cyc_Std_SO_int_struct*) _temp247)->f1; goto _LL244;}
else{ goto _LL239;} _LL244: _temp243= _temp235.f2; if(*(( int*) _temp243) == 
Cyc_Std_SO_socklen){ _LL246: _temp245=(( struct Cyc_Std_SO_socklen_struct*)
_temp243)->f1; goto _LL238;} else{ goto _LL239;} _LL239: _LL256: _temp255=
_temp235.f1; if(*(( int*) _temp255) ==  Cyc_Std_SO_timeval){ _LL258: _temp257=((
struct Cyc_Std_SO_timeval_struct*) _temp255)->f1; goto _LL252;} else{ goto
_LL241;} _LL252: _temp251= _temp235.f2; if(*(( int*) _temp251) ==  Cyc_Std_SO_socklen){
_LL254: _temp253=(( struct Cyc_Std_SO_socklen_struct*) _temp251)->f1; goto
_LL240;} else{ goto _LL241;} _LL241: goto _LL242; _LL238: return setsockopt_int(
fd, level, optname,( const int*) _temp249, _temp245); _LL240: return
setsockopt_timeval( fd, level, optname,( const struct Cyc_Std_timeval*) _temp257,
_temp253); _LL242:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp259=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp259[ 0]=({ struct Cyc_Std_SocketError_struct _temp260; _temp260.tag= Cyc_Std_SocketError;
_temp260.f1= _tag_arr("setsockopt---bad args", sizeof( unsigned char), 22u);
_temp260;}); _temp259;})); _LL236:;}}
