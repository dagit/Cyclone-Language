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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_timeval{
int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{ int tz_minuteswest; int
tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval* __p, struct Cyc_Std_timezone*
__z); struct Cyc_Std_sockaddr_in; static const int Cyc_Std_SA_sockaddr_in= 0;
struct Cyc_Std_SA_sockaddr_in_struct{ int tag; struct Cyc_Std_sockaddr_in* f1; }
; static const int Cyc_Std_SA_socklenptr= 1; struct Cyc_Std_SA_socklenptr_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_SA_socklen= 2; struct
Cyc_Std_SA_socklen_struct{ int tag; unsigned int f1; } ; extern int socket( int
domain, int type, int protocol); extern int socketpair( int domain, int type,
int protocol, int* fds); extern int listen( int fd, int n); extern int shutdown(
int fd, int how); static const int Cyc_Std_SO_int= 0; struct Cyc_Std_SO_int_struct{
int tag; int* f1; } ; static const int Cyc_Std_SO_timeval= 1; struct Cyc_Std_SO_timeval_struct{
int tag; struct Cyc_Std_timeval* f1; } ; static const int Cyc_Std_SO_socklenptr=
2; struct Cyc_Std_SO_socklenptr_struct{ int tag; unsigned int* f1; } ; static
const int Cyc_Std_SO_socklen= 3; struct Cyc_Std_SO_socklen_struct{ int tag;
unsigned int f1; } ; struct Cyc_Std_in_addr{ unsigned int s_addr; } ; struct Cyc_Std_sockaddr_in{
unsigned short sin_family; unsigned short sin_port; struct Cyc_Std_in_addr
sin_addr; unsigned char sin_zero[ 8u]; } ; extern unsigned int htonl(
unsigned int); extern unsigned short htons( unsigned short); extern unsigned int
ntohl( unsigned int); extern unsigned short ntohs( unsigned short); struct Cyc_Std_servent{
struct _tagged_arr s_name; struct _tagged_arr s_aliases; unsigned short s_port;
struct _tagged_arr s_proto; } ; struct Cyc_Std_hostent{ struct _tagged_arr
h_name; struct _tagged_arr h_aliases; int h_addrtype; int h_length; struct
_tagged_arr h_addr_list; } ; extern struct Cyc_Std_servent* Cyc_Std_getservbyname(
struct _tagged_arr name, struct _tagged_arr proto); extern struct Cyc_Std_hostent*
Cyc_Std_gethostbyname( struct _tagged_arr name); extern void Cyc_Std_herror(
struct _tagged_arr); struct Cyc_Cnetdb_Cservent{ unsigned char* s_name;
unsigned char** s_aliases; unsigned short s_port; unsigned char* s_proto; } ;
struct Cyc_Cnetdb_Chostent{ unsigned char* h_name; unsigned char** h_aliases;
int h_addrtype; int h_length; struct Cyc_Std_in_addr** h_addr_list; } ; extern
struct Cyc_Cnetdb_Cservent* getservbyname( unsigned char* name, unsigned char*
proto); extern struct Cyc_Cnetdb_Chostent* gethostbyname( unsigned char* name);
extern void herror( unsigned char*); extern struct _tagged_arr pntlp_toCyc(
struct Cyc_Std_in_addr**); struct Cyc_Std_servent* Cyc_Std_getservbyname( struct
_tagged_arr name, struct _tagged_arr proto){ struct Cyc_Cnetdb_Cservent* src=
getservbyname( string_to_Cstring( name), string_to_Cstring( proto)); return(
unsigned int) src?({ struct Cyc_Std_servent* _temp0=( struct Cyc_Std_servent*)
_cycalloc( sizeof( struct Cyc_Std_servent)); _temp0->s_name= Cstring_to_string(((
struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_name); _temp0->s_aliases=
ntCsl_to_ntsl((( struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_aliases);
_temp0->s_port=(( struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_port;
_temp0->s_proto= Cstring_to_string((( struct Cyc_Cnetdb_Cservent*) _check_null(
src))->s_proto); _temp0;}): 0;} struct Cyc_Std_hostent* Cyc_Std_gethostbyname(
struct _tagged_arr name){ struct Cyc_Cnetdb_Chostent* src= gethostbyname(
string_to_Cstring( name)); return( unsigned int) src?({ struct Cyc_Std_hostent*
_temp1=( struct Cyc_Std_hostent*) _cycalloc( sizeof( struct Cyc_Std_hostent));
_temp1->h_name= Cstring_to_string((( struct Cyc_Cnetdb_Chostent*) _check_null(
src))->h_name); _temp1->h_aliases= ntCsl_to_ntsl((( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_aliases); _temp1->h_addrtype=(( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_addrtype; _temp1->h_length=(( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_length; _temp1->h_addr_list= pntlp_toCyc((( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_addr_list); _temp1;}): 0;} void Cyc_Std_herror( struct
_tagged_arr s){ herror( string_to_Cstring( s));}
