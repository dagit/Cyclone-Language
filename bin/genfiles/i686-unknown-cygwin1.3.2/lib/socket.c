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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int socket(
int domain, int type, int protocol); extern int Cyc_Std_bind( int fd, void* addr);
extern int Cyc_Std_connect( int fd, void* addr); extern int Cyc_Std_setsockopt(
int fd, int level, int optname, struct _tagged_arr optval, unsigned int optlen);
extern int listen( int fd, int n); struct _tuple0{ int f1; void* f2; } ; extern
struct _tuple0 Cyc_Std_accept( int fd); extern int shutdown( int fd, int how);
struct Cyc_Std_in_addr{ unsigned int s_addr; } ; struct Cyc_Std_sockaddr_in{
unsigned short sin_family; unsigned short sin_port; struct Cyc_Std_in_addr
sin_addr; unsigned char sin_zero[ 8u]; } ; extern unsigned char Cyc_Std_sockaddr_in[
16u]; struct Cyc_Std_sockaddr_in_struct{ unsigned char* tag; struct Cyc_Std_sockaddr_in
f1; } ; extern unsigned int htonl( unsigned int); extern unsigned short htons(
unsigned short); extern unsigned int ntohl( unsigned int); extern unsigned short
ntohs( unsigned short); extern int* __errno(); extern int bind( int fd, const
struct Cyc_Std_sockaddr_in* addr, unsigned int len); extern int connect( int fd,
const struct Cyc_Std_sockaddr_in* addr, unsigned int len); extern int accept(
int fd, struct Cyc_Std_sockaddr_in* addr, unsigned int* len); extern struct Cyc_Std_sockaddr_in*
sockaddr_to_Csockaddr( void* addr); extern void* Csockaddr_to_sockaddr( const
struct Cyc_Std_sockaddr_in*, unsigned int); struct _tuple1{ struct Cyc_Std_sockaddr_in*
f1; unsigned int f2; } ; struct _tuple1 Cyc_Csocket_sa2Csa( void* addr){ struct
Cyc_Std_sockaddr_in* ptr= sockaddr_to_Csockaddr( addr); void* _temp0= addr;
struct Cyc_Std_sockaddr_in _temp6; _LL2: if(*(( void**) _temp0) ==  Cyc_Std_sockaddr_in){
_LL7: _temp6=(( struct Cyc_Std_sockaddr_in_struct*) _temp0)->f1; goto _LL3;}
else{ goto _LL4;} _LL4: goto _LL5; _LL3: return({ struct _tuple1 _temp8; _temp8.f1=
ptr; _temp8.f2=( unsigned int) sizeof( struct Cyc_Std_sockaddr_in); _temp8;});
_LL5:( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp9=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp9[ 0]=({ struct Cyc_Core_Failure_struct _temp10; _temp10.tag= Cyc_Core_Failure;
_temp10.f1= _tag_arr("Unexpected xtunion sockaddr found", sizeof( unsigned char),
34u); _temp10;}); _temp9;})); _LL1:;} void* Cyc_Csocket_Csa2sa( const struct Cyc_Std_sockaddr_in*
addr, unsigned int len){ return Csockaddr_to_sockaddr( addr, len);} int Cyc_Std_bind(
int fd, void* addr){ unsigned int _temp13; struct Cyc_Std_sockaddr_in* _temp15;
struct _tuple1 _temp11= Cyc_Csocket_sa2Csa( addr); _LL16: _temp15= _temp11.f1;
goto _LL14; _LL14: _temp13= _temp11.f2; goto _LL12; _LL12: return bind( fd,(
const struct Cyc_Std_sockaddr_in*) _temp15, _temp13);} int Cyc_Std_connect( int
fd, void* addr){ unsigned int _temp19; struct Cyc_Std_sockaddr_in* _temp21;
struct _tuple1 _temp17= Cyc_Csocket_sa2Csa( addr); _LL22: _temp21= _temp17.f1;
goto _LL20; _LL20: _temp19= _temp17.f2; goto _LL18; _LL18: return connect( fd,(
const struct Cyc_Std_sockaddr_in*) _temp21, _temp19);} int Cyc_Std_setsockopt(
int fd, int level, int optname, struct _tagged_arr optval, unsigned int optlen){*
__errno()= 109; return - 1;} struct _tuple0 Cyc_Std_accept( int fd){ struct Cyc_Std_sockaddr_in
Caddr=({ struct Cyc_Std_sockaddr_in _temp24; _temp24.sin_family= 0; _temp24.sin_port=
0; _temp24.sin_addr=({ struct Cyc_Std_in_addr _temp26; _temp26.s_addr= 0;
_temp26;});{ unsigned int _temp25= 8u; unsigned int i; for( i= 0; i <  _temp25;
i ++){( _temp24.sin_zero)[ i]='\000';}} _temp24;}); unsigned int len= sizeof(
struct Cyc_Std_sockaddr_in); int result= accept( fd,( struct Cyc_Std_sockaddr_in*)&
Caddr,( unsigned int*)& len); return({ struct _tuple0 _temp23; _temp23.f1=
result; _temp23.f2= Cyc_Csocket_Csa2sa(( const struct Cyc_Std_sockaddr_in*)&
Caddr, len); _temp23;});}
