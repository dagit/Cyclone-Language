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
 struct Cyc_Std_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{
int tz_minuteswest; int tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval*
__p, struct Cyc_Std_timezone* __z); struct Cyc_Std_sockaddr_in; struct Cyc_Std_sockaddr{
unsigned short sa_family; unsigned char sa_data[ 14u]; } ; static const int Cyc_Std_SA_sockaddr_in=
0; struct Cyc_Std_SA_sockaddr_in_struct{ int tag; struct Cyc_Std_sockaddr_in* f1;
} ; static const int Cyc_Std_SA_sockaddr= 1; struct Cyc_Std_SA_sockaddr_struct{
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
unsigned short); struct Cyc_Std_Csocket_sockaddr; extern int accept_in( int,
const struct Cyc_Std_sockaddr_in*, unsigned int*); extern int bind_in( int,
const struct Cyc_Std_sockaddr_in*, unsigned int); extern int connect_in( int,
const struct Cyc_Std_sockaddr_in*, unsigned int); extern int getpeername_in( int,
const struct Cyc_Std_sockaddr_in*, unsigned int*); extern int getsockname_in(
int, const struct Cyc_Std_sockaddr_in*, unsigned int*); extern int recvfrom_in(
int, struct _tagged_arr, unsigned int, int, const struct Cyc_Std_sockaddr_in*,
unsigned int*); extern int sendto_in( int, struct _tagged_arr, unsigned int, int,
const struct Cyc_Std_sockaddr_in*, unsigned int); extern int send_wrapped( int,
struct _tagged_arr, unsigned int, int); extern int recv_wrapped( int, struct
_tagged_arr, unsigned int, int); extern int getsockopt_int( int, int, int, int*,
unsigned int*); extern int setsockopt_int( int, int, int, const int*,
unsigned int); extern int getsockopt_timeval( int, int, int, struct Cyc_Std_timeval*,
unsigned int*); extern int setsockopt_timeval( int, int, int, const struct Cyc_Std_timeval*,
unsigned int); unsigned char Cyc_Std_SocketError[ 16u]="\000\000\000\000SocketError";
struct _tuple0{ void* f1; void* f2; } ; int Cyc_Std_accept( int fd, struct
_tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw((
void*) Cyc_Std_SocketError);}{ struct _tuple0 _temp1=({ struct _tuple0 _temp0;
_temp0.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp0.f2=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 1)); _temp0;}); void*
_temp7; unsigned int* _temp9; void* _temp11; struct Cyc_Std_sockaddr_in* _temp13;
_LL3: _LL12: _temp11= _temp1.f1; if(*(( int*) _temp11) ==  Cyc_Std_SA_sockaddr_in){
_LL14: _temp13=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp11)->f1; goto _LL8;}
else{ goto _LL5;} _LL8: _temp7= _temp1.f2; if(*(( int*) _temp7) ==  Cyc_Std_SA_socklenptr){
_LL10: _temp9=(( struct Cyc_Std_SA_socklenptr_struct*) _temp7)->f1; goto _LL4;}
else{ goto _LL5;} _LL5: goto _LL6; _LL4: return accept_in( fd,( const struct Cyc_Std_sockaddr_in*)
_temp13, _temp9); _LL6:( int) _throw(( void*) Cyc_Std_SocketError); _LL2:;}} int
Cyc_Std_bind( int fd, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof(
void*)) !=  2){( int) _throw(( void*) Cyc_Std_SocketError);}{ struct _tuple0
_temp16=({ struct _tuple0 _temp15; _temp15.f1=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0)); _temp15.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp15;}); void* _temp22;
unsigned int _temp24; void* _temp26; struct Cyc_Std_sockaddr_in* _temp28; _LL18:
_LL27: _temp26= _temp16.f1; if(*(( int*) _temp26) ==  Cyc_Std_SA_sockaddr_in){
_LL29: _temp28=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp26)->f1; goto
_LL23;} else{ goto _LL20;} _LL23: _temp22= _temp16.f2; if(*(( int*) _temp22) == 
Cyc_Std_SA_socklen){ _LL25: _temp24=(( struct Cyc_Std_SA_socklen_struct*)
_temp22)->f1; goto _LL19;} else{ goto _LL20;} _LL20: goto _LL21; _LL19: return
bind_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp28, _temp24); _LL21:( int)
_throw(( void*) Cyc_Std_SocketError); _LL17:;}} int Cyc_Std_connect( int fd,
struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){( int)
_throw(( void*) Cyc_Std_SocketError);}{ struct _tuple0 _temp31=({ struct _tuple0
_temp30; _temp30.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp30.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp30;}); void* _temp37; unsigned int _temp39; void* _temp41; struct Cyc_Std_sockaddr_in*
_temp43; _LL33: _LL42: _temp41= _temp31.f1; if(*(( int*) _temp41) ==  Cyc_Std_SA_sockaddr_in){
_LL44: _temp43=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp41)->f1; goto
_LL38;} else{ goto _LL35;} _LL38: _temp37= _temp31.f2; if(*(( int*) _temp37) == 
Cyc_Std_SA_socklen){ _LL40: _temp39=(( struct Cyc_Std_SA_socklen_struct*)
_temp37)->f1; goto _LL34;} else{ goto _LL35;} _LL35: goto _LL36; _LL34: return
connect_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp43, _temp39); _LL36:(
int) _throw(( void*) Cyc_Std_SocketError); _LL32:;}} int Cyc_Std_getpeername(
int fd, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){(
int) _throw(( void*) Cyc_Std_SocketError);}{ struct _tuple0 _temp46=({ struct
_tuple0 _temp45; _temp45.f1=*(( void**) _check_unknown_subscript( ap, sizeof(
void*), 0)); _temp45.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*),
1)); _temp45;}); void* _temp52; unsigned int* _temp54; void* _temp56; struct Cyc_Std_sockaddr_in*
_temp58; _LL48: _LL57: _temp56= _temp46.f1; if(*(( int*) _temp56) ==  Cyc_Std_SA_sockaddr_in){
_LL59: _temp58=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp56)->f1; goto
_LL53;} else{ goto _LL50;} _LL53: _temp52= _temp46.f2; if(*(( int*) _temp52) == 
Cyc_Std_SA_socklenptr){ _LL55: _temp54=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp52)->f1; goto _LL49;} else{ goto _LL50;} _LL50: goto _LL51; _LL49: return
getpeername_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp58, _temp54); _LL51:(
int) _throw(( void*) Cyc_Std_SocketError); _LL47:;}} int Cyc_Std_getsockname(
int fd, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){(
int) _throw(( void*) Cyc_Std_SocketError);}{ struct _tuple0 _temp61=({ struct
_tuple0 _temp60; _temp60.f1=*(( void**) _check_unknown_subscript( ap, sizeof(
void*), 0)); _temp60.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*),
1)); _temp60;}); void* _temp67; unsigned int* _temp69; void* _temp71; struct Cyc_Std_sockaddr_in*
_temp73; _LL63: _LL72: _temp71= _temp61.f1; if(*(( int*) _temp71) ==  Cyc_Std_SA_sockaddr_in){
_LL74: _temp73=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp71)->f1; goto
_LL68;} else{ goto _LL65;} _LL68: _temp67= _temp61.f2; if(*(( int*) _temp67) == 
Cyc_Std_SA_socklenptr){ _LL70: _temp69=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp67)->f1; goto _LL64;} else{ goto _LL65;} _LL65: goto _LL66; _LL64: return
getsockname_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp73, _temp69); _LL66:(
int) _throw(( void*) Cyc_Std_SocketError); _LL62:;}} int Cyc_Std_recvfrom( int
fd, struct _tagged_arr buf, unsigned int n, int flags, struct _tagged_arr ap){
if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*) Cyc_Std_SocketError);}{
struct _tuple0 _temp76=({ struct _tuple0 _temp75; _temp75.f1=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0)); _temp75.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp75;}); void* _temp82;
unsigned int* _temp84; void* _temp86; struct Cyc_Std_sockaddr_in* _temp88; _LL78:
_LL87: _temp86= _temp76.f1; if(*(( int*) _temp86) ==  Cyc_Std_SA_sockaddr_in){
_LL89: _temp88=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp86)->f1; goto
_LL83;} else{ goto _LL80;} _LL83: _temp82= _temp76.f2; if(*(( int*) _temp82) == 
Cyc_Std_SA_socklenptr){ _LL85: _temp84=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp82)->f1; goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL79: return
recvfrom_in( fd, buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp88,
_temp84); _LL81:( int) _throw(( void*) Cyc_Std_SocketError); _LL77:;}} int Cyc_Std_sendto(
int fd, struct _tagged_arr buf, unsigned int n, int flags, struct _tagged_arr ap){
if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*) Cyc_Std_SocketError);}{
struct _tuple0 _temp91=({ struct _tuple0 _temp90; _temp90.f1=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0)); _temp90.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp90;}); void* _temp97;
unsigned int _temp99; void* _temp101; struct Cyc_Std_sockaddr_in* _temp103;
_LL93: _LL102: _temp101= _temp91.f1; if(*(( int*) _temp101) ==  Cyc_Std_SA_sockaddr_in){
_LL104: _temp103=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp101)->f1; goto
_LL98;} else{ goto _LL95;} _LL98: _temp97= _temp91.f2; if(*(( int*) _temp97) == 
Cyc_Std_SA_socklen){ _LL100: _temp99=(( struct Cyc_Std_SA_socklen_struct*)
_temp97)->f1; goto _LL94;} else{ goto _LL95;} _LL95: goto _LL96; _LL94: return
sendto_in( fd, buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp103,
_temp99); _LL96:( int) _throw(( void*) Cyc_Std_SocketError); _LL92:;}} int Cyc_Std_send(
int fd, struct _tagged_arr buf, unsigned int n, int flags){ return send_wrapped(
fd, buf, n, flags);} int Cyc_Std_recv( int fd, struct _tagged_arr buf,
unsigned int n, int flags){ return recv_wrapped( fd, buf, n, flags);} int Cyc_Std_getsockopt(
int fd, int level, int optname, struct _tagged_arr ap){ if( _get_arr_size( ap,
sizeof( void*)) !=  2){( int) _throw(( void*) Cyc_Std_SocketError);}{ struct
_tuple0 _temp106=({ struct _tuple0 _temp105; _temp105.f1=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 0)); _temp105.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp105;}); void* _temp114;
unsigned int* _temp116; void* _temp118; int* _temp120; void* _temp122;
unsigned int* _temp124; void* _temp126; struct Cyc_Std_timeval* _temp128; _LL108:
_LL119: _temp118= _temp106.f1; if(*(( int*) _temp118) ==  Cyc_Std_SO_int){
_LL121: _temp120=(( struct Cyc_Std_SO_int_struct*) _temp118)->f1; goto _LL115;}
else{ goto _LL110;} _LL115: _temp114= _temp106.f2; if(*(( int*) _temp114) == 
Cyc_Std_SO_socklenptr){ _LL117: _temp116=(( struct Cyc_Std_SO_socklenptr_struct*)
_temp114)->f1; goto _LL109;} else{ goto _LL110;} _LL110: _LL127: _temp126=
_temp106.f1; if(*(( int*) _temp126) ==  Cyc_Std_SO_timeval){ _LL129: _temp128=((
struct Cyc_Std_SO_timeval_struct*) _temp126)->f1; goto _LL123;} else{ goto
_LL112;} _LL123: _temp122= _temp106.f2; if(*(( int*) _temp122) ==  Cyc_Std_SO_socklenptr){
_LL125: _temp124=(( struct Cyc_Std_SO_socklenptr_struct*) _temp122)->f1; goto
_LL111;} else{ goto _LL112;} _LL112: goto _LL113; _LL109: return getsockopt_int(
fd, level, optname, _temp120, _temp116); _LL111: return getsockopt_timeval( fd,
level, optname, _temp128, _temp124); _LL113:( int) _throw(( void*) Cyc_Std_SocketError);
_LL107:;}} int Cyc_Std_setsockopt( int fd, int level, int optname, struct
_tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw((
void*) Cyc_Std_SocketError);}{ struct _tuple0 _temp131=({ struct _tuple0
_temp130; _temp130.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*),
0)); _temp130.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp130;}); void* _temp139; unsigned int _temp141; void* _temp143; int*
_temp145; void* _temp147; unsigned int _temp149; void* _temp151; struct Cyc_Std_timeval*
_temp153; _LL133: _LL144: _temp143= _temp131.f1; if(*(( int*) _temp143) ==  Cyc_Std_SO_int){
_LL146: _temp145=(( struct Cyc_Std_SO_int_struct*) _temp143)->f1; goto _LL140;}
else{ goto _LL135;} _LL140: _temp139= _temp131.f2; if(*(( int*) _temp139) == 
Cyc_Std_SO_socklen){ _LL142: _temp141=(( struct Cyc_Std_SO_socklen_struct*)
_temp139)->f1; goto _LL134;} else{ goto _LL135;} _LL135: _LL152: _temp151=
_temp131.f1; if(*(( int*) _temp151) ==  Cyc_Std_SO_timeval){ _LL154: _temp153=((
struct Cyc_Std_SO_timeval_struct*) _temp151)->f1; goto _LL148;} else{ goto
_LL137;} _LL148: _temp147= _temp131.f2; if(*(( int*) _temp147) ==  Cyc_Std_SO_socklen){
_LL150: _temp149=(( struct Cyc_Std_SO_socklen_struct*) _temp147)->f1; goto
_LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL134: return setsockopt_int(
fd, level, optname,( const int*) _temp145, _temp141); _LL136: return
setsockopt_timeval( fd, level, optname,( const struct Cyc_Std_timeval*) _temp153,
_temp149); _LL138:( int) _throw(( void*) Cyc_Std_SocketError); _LL132:;}}
