// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
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

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
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
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
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
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_dirent{
int d_ino; int d_off; unsigned short d_reclen; unsigned char d_type;
unsigned char d_name[ 256u]; } ; void* Cyc_Std_opendir( struct _tagged_arr name);
extern struct Cyc_Std_dirent* readdir( void* dir); extern int closedir( void*
dir); extern void* opendir( unsigned char* name); void* Cyc_Std_opendir( struct
_tagged_arr name){ return opendir( string_to_Cstring( name));} struct Cyc_Std_flock{
short l_type; int l_start; short l_whence; int l_len; int l_pid; } ; static
const int Cyc_Std_Flock= 0; struct Cyc_Std_Flock_struct{ int tag; struct Cyc_Std_flock*
f1; } ; static const int Cyc_Std_Long= 1; struct Cyc_Std_Long_struct{ int tag;
int f1; } ; extern int Cyc_Std_fcntl( int fd, int cmd, struct _tagged_arr);
extern int Cyc_Std_open( struct _tagged_arr, int, struct _tagged_arr); extern
int Cyc_Std_creat( struct _tagged_arr, unsigned int); extern int fcntl( int fd,
int cmd); extern int fcntl_with_arg( int fd, int cmd, int arg); extern int
fcntl_with_lock( int fd, int cmd, struct Cyc_Std_flock* lock); extern int creat(
unsigned char*, unsigned int); extern int open_without_mode( unsigned char*, int);
extern int open_with_mode( unsigned char*, int, unsigned int); int Cyc_Std_fcntl(
int fd, int cmd, struct _tagged_arr argv){ if( _get_arr_size( argv, sizeof( void*))
==  0){ return fcntl( fd, cmd);} else{ if( _get_arr_size( argv, sizeof( void*))
!=  1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct
Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp0[ 0]=({ struct Cyc_Core_Failure_struct _temp1; _temp1.tag= Cyc_Core_Failure;
_temp1.f1= _tag_arr("fcntl: too many args", sizeof( unsigned char), 21u); _temp1;});
_temp0;}));} else{ void* _temp2=*(( void**) _check_unknown_subscript( argv,
sizeof( void*), 0)); int _temp8; struct Cyc_Std_flock* _temp10; _LL4: if(*(( int*)
_temp2) ==  Cyc_Std_Long){ _LL9: _temp8=(( struct Cyc_Std_Long_struct*) _temp2)->f1;
goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp2) ==  Cyc_Std_Flock){
_LL11: _temp10=(( struct Cyc_Std_Flock_struct*) _temp2)->f1; goto _LL7;} else{
goto _LL3;} _LL5: return fcntl_with_arg( fd, cmd, _temp8); _LL7: return
fcntl_with_lock( fd, cmd,( struct Cyc_Std_flock*) _temp10); _LL3:;}}} int Cyc_Std_creat(
struct _tagged_arr s, unsigned int m){ return creat( string_to_Cstring( s), m);}
int Cyc_Std_open( struct _tagged_arr s, int i, struct _tagged_arr ms){ if(
_get_arr_size( ms, sizeof( unsigned int)) >=  1){ return open_with_mode(
string_to_Cstring( s), i,*(( unsigned int*) _check_unknown_subscript( ms,
sizeof( unsigned int), 0)));} else{ return open_without_mode( string_to_Cstring(
s), i);}} struct Cyc_Std_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_Std_timezone{
int tz_minuteswest; int tz_dsttime; } ; extern int gettimeofday( struct Cyc_Std_timeval*
__p, struct Cyc_Std_timezone* __z); struct Cyc_Std_itimerval{ struct Cyc_Std_timeval
it_interval; struct Cyc_Std_timeval it_value; } ; extern int getitimer( int,
struct Cyc_Std_itimerval*); extern int setitimer( int, const struct Cyc_Std_itimerval*,
struct Cyc_Std_itimerval*); typedef struct { unsigned int __val[ 64u]; } Cyc_Std___sigset_t;
typedef Cyc_Std___sigset_t Cyc_Std_sigset_t; struct Cyc_Std_timespec{ int tv_sec;
int tv_nsec; } ; struct Cyc_Std_timeval; struct Cyc_Std___fd_set{ int __fds_bits[
32u]; } ; extern int select( int, struct Cyc_Std___fd_set*, struct Cyc_Std___fd_set*,
struct Cyc_Std___fd_set*, struct Cyc_Std_timeval*); struct Cyc_Std_sockaddr_in;
struct Cyc_Std_sockaddr{ unsigned short sa_family; unsigned char sa_data[ 14u];
} ; static const int Cyc_Std_SA_sockaddr_in= 0; struct Cyc_Std_SA_sockaddr_in_struct{
int tag; struct Cyc_Std_sockaddr_in* f1; } ; static const int Cyc_Std_SA_sockaddr=
1; struct Cyc_Std_SA_sockaddr_struct{ int tag; struct Cyc_Std_sockaddr* f1; } ;
static const int Cyc_Std_SA_socklenptr= 2; struct Cyc_Std_SA_socklenptr_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_SA_socklen= 3; struct
Cyc_Std_SA_socklen_struct{ int tag; unsigned int f1; } ; extern int socket( int
domain, int type, int protocol); extern int socketpair( int domain, int type,
int protocol, int* fds); extern int Cyc_Std_accept( int fd, struct _tagged_arr);
extern int Cyc_Std_bind( int fd, struct _tagged_arr); extern int Cyc_Std_connect(
int fd, struct _tagged_arr); extern int Cyc_Std_getpeername( int fd, struct
_tagged_arr); extern int Cyc_Std_getsockname( int fd, struct _tagged_arr);
extern int listen( int fd, int n); extern int shutdown( int fd, int how); extern
int Cyc_Std_send( int fd, struct _tagged_arr buf, unsigned int n, int flags);
extern int Cyc_Std_recv( int fd, struct _tagged_arr buf, unsigned int n, int
flags); extern int Cyc_Std_sendto( int fd, struct _tagged_arr buf, unsigned int
n, int flags, struct _tagged_arr); extern int Cyc_Std_recvfrom( int fd, struct
_tagged_arr buf, unsigned int n, int flags, struct _tagged_arr); static const
int Cyc_Std_SO_int= 0; struct Cyc_Std_SO_int_struct{ int tag; int* f1; } ;
static const int Cyc_Std_SO_timeval= 1; struct Cyc_Std_SO_timeval_struct{ int
tag; struct Cyc_Std_timeval* f1; } ; static const int Cyc_Std_SO_socklenptr= 2;
struct Cyc_Std_SO_socklenptr_struct{ int tag; unsigned int* f1; } ; static const
int Cyc_Std_SO_socklen= 3; struct Cyc_Std_SO_socklen_struct{ int tag;
unsigned int f1; } ; extern int Cyc_Std_getsockopt( int fd, int level, int
optname, struct _tagged_arr); extern int Cyc_Std_setsockopt( int fd, int level,
int optname, struct _tagged_arr); struct Cyc_Std_in_addr{ unsigned int s_addr; }
; struct Cyc_Std_sockaddr_in{ unsigned short sin_family; unsigned short sin_port;
struct Cyc_Std_in_addr sin_addr; unsigned char sin_zero[ 8u]; } ; extern
unsigned int htonl( unsigned int); extern unsigned short htons( unsigned short);
extern unsigned int ntohl( unsigned int); extern unsigned short ntohs(
unsigned short); struct Cyc_Std_servent{ struct _tagged_arr s_name; struct
_tagged_arr s_aliases; unsigned short s_port; struct _tagged_arr s_proto; } ;
struct Cyc_Std_hostent{ struct _tagged_arr h_name; struct _tagged_arr h_aliases;
int h_addrtype; int h_length; struct _tagged_arr h_addr_list; } ; struct Cyc_Std_protoent{
struct _tagged_arr p_name; struct _tagged_arr p_aliases; int p_proto; } ; extern
struct Cyc_Std_servent* Cyc_Std_getservbyname( struct _tagged_arr name, struct
_tagged_arr proto); extern struct Cyc_Std_hostent* Cyc_Std_gethostbyname( struct
_tagged_arr name); extern struct Cyc_Std_protoent* Cyc_Std_getprotobyname(
struct _tagged_arr name); extern void Cyc_Std_herror( struct _tagged_arr);
struct Cyc_Cnetdb_Cservent{ unsigned char* s_name; unsigned char** s_aliases;
unsigned short s_port; unsigned char* s_proto; } ; struct Cyc_Cnetdb_Chostent{
unsigned char* h_name; unsigned char** h_aliases; int h_addrtype; int h_length;
struct Cyc_Std_in_addr** h_addr_list; } ; struct Cyc_Cnetdb_Cprotoent{
unsigned char* p_name; unsigned char** p_aliases; int p_proto; } ; extern struct
Cyc_Cnetdb_Cservent* getservbyname( unsigned char* name, unsigned char* proto);
extern struct Cyc_Cnetdb_Chostent* gethostbyname( unsigned char* name); extern
struct Cyc_Cnetdb_Cprotoent* getprotobyname( unsigned char* name); extern void
herror( unsigned char*); extern struct _tagged_arr pntlp_toCyc( struct Cyc_Std_in_addr**);
struct Cyc_Std_servent* Cyc_Std_getservbyname( struct _tagged_arr name, struct
_tagged_arr proto){ struct Cyc_Cnetdb_Cservent* src= getservbyname(
string_to_Cstring( name), string_to_Cstring( proto)); return( unsigned int) src?({
struct Cyc_Std_servent* _temp12=( struct Cyc_Std_servent*) _cycalloc( sizeof(
struct Cyc_Std_servent)); _temp12->s_name= Cstring_to_string( src->s_name);
_temp12->s_aliases= ntCsl_to_ntsl( src->s_aliases); _temp12->s_port= src->s_port;
_temp12->s_proto= Cstring_to_string( src->s_proto); _temp12;}): 0;} struct Cyc_Std_hostent*
Cyc_Std_gethostbyname( struct _tagged_arr name){ struct Cyc_Cnetdb_Chostent* src=
gethostbyname( string_to_Cstring( name)); return( unsigned int) src?({ struct
Cyc_Std_hostent* _temp13=( struct Cyc_Std_hostent*) _cycalloc( sizeof( struct
Cyc_Std_hostent)); _temp13->h_name= Cstring_to_string( src->h_name); _temp13->h_aliases=
ntCsl_to_ntsl( src->h_aliases); _temp13->h_addrtype= src->h_addrtype; _temp13->h_length=
src->h_length; _temp13->h_addr_list= pntlp_toCyc( src->h_addr_list); _temp13;}):
0;} struct Cyc_Std_protoent* Cyc_Std_getprotobyname( struct _tagged_arr name){
struct Cyc_Cnetdb_Cprotoent* src= getprotobyname( string_to_Cstring( name));
return( unsigned int) src?({ struct Cyc_Std_protoent* _temp14=( struct Cyc_Std_protoent*)
_cycalloc( sizeof( struct Cyc_Std_protoent)); _temp14->p_name= Cstring_to_string(
src->p_name); _temp14->p_aliases= ntCsl_to_ntsl( src->p_aliases); _temp14->p_proto=
src->p_proto; _temp14;}): 0;} void Cyc_Std_herror( struct _tagged_arr s){ herror(
string_to_Cstring( s));} unsigned char Cyc_Std_sockaddr_in[ 16u]="\000\000\000\000sockaddr_in";
struct Cyc_Std_sockaddr_in_struct{ unsigned char* tag; struct Cyc_Std_sockaddr_in
f1; } ; extern int Cyc_Std_inet_aton( struct _tagged_arr cp, struct Cyc_Std_in_addr*
inp); extern struct _tagged_arr Cyc_Std_inet_ntoa( struct Cyc_Std_in_addr);
extern unsigned int Cyc_Std_inet_addr( struct _tagged_arr addr); extern int
inet_aton( unsigned char* cp, struct Cyc_Std_in_addr* inp); extern unsigned char*
inet_ntoa( struct Cyc_Std_in_addr); extern unsigned int inet_addr( unsigned char*);
int Cyc_Std_inet_aton( struct _tagged_arr cp, struct Cyc_Std_in_addr* inp){
return inet_aton( string_to_Cstring( cp), inp);} struct _tagged_arr Cyc_Std_inet_ntoa(
struct Cyc_Std_in_addr x){ return wrap_Cstring_as_string( inet_ntoa( x), - 1);}
unsigned int Cyc_Std_inet_addr( struct _tagged_arr addr){ return inet_addr(
string_to_Cstring( addr));} void(* Cyc_Std_signal( int sig, void(* func)( int)))(
int); extern int raise( int sig); extern void Cyc_Std__SIG_DFL( int); extern
void Cyc_Std__SIG_IGN( int); extern void Cyc_Std__SIG_ERR( int); extern int kill(
int, int); extern void(* signal_func( int sig, void(* func)( int)))( int); void
Cyc_Std__SIG_DFL( int n){;} void Cyc_Std__SIG_IGN( int n){;} void Cyc_Std__SIG_ERR(
int n){;} void(* Cyc_Std_signal( int sig, void(* func)( int)))( int){ return
signal_func( sig, func);} struct Cyc_Std_tm{ int tm_sec; int tm_min; int tm_hour;
int tm_mday; int tm_mon; int tm_year; int tm_wday; int tm_yday; int tm_isdst; }
; extern void tzset(); extern int time( int* t); extern struct _tagged_arr Cyc_Std_asctime(
const struct Cyc_Std_tm* timeptr); extern struct _tagged_arr Cyc_Std_ctime(
const int* timep); extern struct Cyc_Std_tm* gmtime( const int* timep); extern
struct Cyc_Std_tm* localtime( const int* timep); extern unsigned int Cyc_Std_strftime(
struct _tagged_arr s, unsigned int maxsize, struct _tagged_arr fmt, const struct
Cyc_Std_tm* t); extern struct _tagged_arr Cyc_Std_asctime_r( const struct Cyc_Std_tm*,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_ctime_r( const int*,
struct _tagged_arr); extern int timezone; extern int daylight; struct Cyc_Std_stat_t{
unsigned long long st_dev; unsigned short __pad1; unsigned int st_ino;
unsigned int st_mode; unsigned int st_nlink; unsigned int st_uid; unsigned int
st_gid; unsigned long long st_rdev; unsigned short __pad2; int st_size; int
st_blksize; int st_blocks; int st_atime; unsigned int __unused1; int st_mtime;
unsigned int __unused2; int st_ctime; unsigned int __unused3; unsigned int
__unused4; unsigned int __unused5; } ; extern int Cyc_Std_stat( struct
_tagged_arr filename, struct Cyc_Std_stat_t* buf); extern int fstat( int fd,
struct Cyc_Std_stat_t* buf); extern int Cyc_Std_lstat( struct _tagged_arr
filename, struct Cyc_Std_stat_t* buf); extern unsigned int umask( unsigned int
mask); extern int Cyc_Std_mkdir( struct _tagged_arr pathname, unsigned int mode);
extern int Cyc_Std_chmod( struct _tagged_arr path, unsigned int mode); extern
int fchmod( int fd, unsigned int mode); extern int stat( unsigned char* filename,
struct Cyc_Std_stat_t* buf); extern int fstat( int fd, struct Cyc_Std_stat_t*
buf); extern int lstat( unsigned char* filename, struct Cyc_Std_stat_t* buf);
extern int mkdir( unsigned char* pathname, unsigned int mode); extern int chmod(
unsigned char* pathname, unsigned int mode); int Cyc_Std_stat( struct
_tagged_arr filename, struct Cyc_Std_stat_t* buf){ return stat(
string_to_Cstring( filename), buf);} int Cyc_Std_lstat( struct _tagged_arr
filename, struct Cyc_Std_stat_t* buf){ return lstat( string_to_Cstring( filename),
buf);} int Cyc_Std_mkdir( struct _tagged_arr pathname, unsigned int mode){
return mkdir( string_to_Cstring( pathname), mode);} int Cyc_Std_chmod( struct
_tagged_arr pathname, unsigned int mode){ return chmod( string_to_Cstring(
pathname), mode);} struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern
struct Cyc_Std___sFILE* Cyc_Std_fromCfile( struct Cyc_Cstdio___sFILE* cf);
extern int Cyc_Std_remove( struct _tagged_arr); extern int Cyc_Std_rename(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_Std_fclose( struct Cyc_Std___sFILE*);
extern int Cyc_Std_fflush( struct Cyc_Std___sFILE*); extern struct Cyc_Std___sFILE*
Cyc_Std_fopen( struct _tagged_arr __filename, struct _tagged_arr __modes);
extern struct Cyc_Std___sFILE* Cyc_Std_freopen( struct _tagged_arr, struct
_tagged_arr, struct Cyc_Std___sFILE*); extern void Cyc_Std_setbuf( struct Cyc_Std___sFILE*
__stream, struct _tagged_arr __buf); extern int Cyc_Std_setvbuf( struct Cyc_Std___sFILE*
__stream, struct _tagged_arr __buf, int __modes, unsigned int __n); extern int
Cyc_Std_fgetc( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_getc(
struct Cyc_Std___sFILE* __stream); extern struct _tagged_arr Cyc_Std_fgets(
struct _tagged_arr __s, int __n, struct Cyc_Std___sFILE* __stream); extern int
Cyc_Std_fputc( int __c, struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_putc(
int __c, struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_fputs( struct
_tagged_arr __s, struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_puts(
struct _tagged_arr __s); extern int Cyc_Std_ungetc( int __c, struct Cyc_Std___sFILE*
__stream); extern unsigned int Cyc_Std_fread( struct _tagged_arr __ptr,
unsigned int __size, unsigned int __n, struct Cyc_Std___sFILE* __stream); extern
unsigned int Cyc_Std_fwrite( struct _tagged_arr __ptr, unsigned int __size,
unsigned int __n, struct Cyc_Std___sFILE* __s); extern int Cyc_Std_fseek( struct
Cyc_Std___sFILE* __stream, int __off, int __whence); extern int Cyc_Std_ftell(
struct Cyc_Std___sFILE* __stream); extern void Cyc_Std_rewind( struct Cyc_Std___sFILE*
__stream); extern int Cyc_Std_fgetpos( struct Cyc_Std___sFILE* __stream, int*
__pos); extern int Cyc_Std_fsetpos( struct Cyc_Std___sFILE* __stream, int* __pos);
extern void Cyc_Std_clearerr( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_feof(
struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_ferror( struct Cyc_Std___sFILE*
__stream); extern void Cyc_Std_perror( struct _tagged_arr __s); extern struct
Cyc_Std___sFILE* Cyc_Std_fdopen( int __fd, struct _tagged_arr __modes); extern
int Cyc_Std_fileno( struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_getw(
struct Cyc_Std___sFILE* __stream); extern int Cyc_Std_putw( int __w, struct Cyc_Std___sFILE*
__stream); extern void Cyc_Std_setbuffer( struct Cyc_Std___sFILE* __stream,
struct _tagged_arr __buf, unsigned int __size); extern void Cyc_Std_setlinebuf(
struct Cyc_Std___sFILE* __stream); extern struct Cyc_Std___sFILE* Cyc_Std_popen(
struct _tagged_arr command, struct _tagged_arr type); extern int Cyc_Std_pclose(
struct Cyc_Std___sFILE* stream); extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct Cyc_Std___sFILE*
Cyc_Std_file_open( struct _tagged_arr fname, struct _tagged_arr mode); extern
void Cyc_Std_file_close( struct Cyc_Std___sFILE*); static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
1; struct Cyc_Std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Std_IntPtr_sa= 2; struct Cyc_Std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_Std_UIntPtr_sa= 3; struct Cyc_Std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Std_StringPtr_sa= 4; struct
Cyc_Std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_Std_DoublePtr_sa= 5; struct Cyc_Std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_Std_FloatPtr_sa= 6; struct Cyc_Std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[
8u]; extern unsigned int Cyc_Std_strlen( struct _tagged_arr s); extern struct
_tagged_arr Cyc_Std_strconcat( struct _tagged_arr, struct _tagged_arr); extern
struct _tagged_arr Cyc_Std_strcpy( struct _tagged_arr dest, struct _tagged_arr
src); struct Cyc_Cstdio___sFILE; extern int remove( unsigned char*); extern int
rename( unsigned char*, unsigned char*); extern int fclose( struct Cyc_Cstdio___sFILE*);
extern int fflush( struct Cyc_Cstdio___sFILE*); extern struct Cyc_Cstdio___sFILE*
fopen( unsigned char* __filename, unsigned char* __modes); extern struct Cyc_Cstdio___sFILE*
freopen( unsigned char* __filename, unsigned char* __modes, struct Cyc_Cstdio___sFILE*
__stream); extern struct Cyc_Cstdio___sFILE* fdopen( int __fd, unsigned char*
__modes); extern int setvbuf( struct Cyc_Cstdio___sFILE* __stream, unsigned char*
__buf, int __modes, unsigned int __n); extern void setbuffer( struct Cyc_Cstdio___sFILE*
__stream, unsigned char* __buf, unsigned int __size); extern void setlinebuf(
struct Cyc_Cstdio___sFILE* __stream); extern int fgetc( struct Cyc_Cstdio___sFILE*
__stream); extern int getc( struct Cyc_Cstdio___sFILE* __stream); extern int
fputc( int __c, struct Cyc_Cstdio___sFILE* __stream); extern int putc( int __c,
struct Cyc_Cstdio___sFILE* __stream); extern int getw( struct Cyc_Cstdio___sFILE*
__stream); extern int putw( int __w, struct Cyc_Cstdio___sFILE* __stream);
extern unsigned char* fgets( unsigned char* __s, int __n, struct Cyc_Cstdio___sFILE*
__stream); extern unsigned char* gets( unsigned char* __s); extern int fputs(
unsigned char* __s, struct Cyc_Cstdio___sFILE* __stream); extern int puts(
unsigned char* __s); extern int ungetc( int __c, struct Cyc_Cstdio___sFILE*
__stream); extern unsigned int fread( unsigned char* __ptr, unsigned int __size,
unsigned int __n, struct Cyc_Cstdio___sFILE* __stream); extern unsigned int
fwrite( unsigned char* __ptr, unsigned int __size, unsigned int __n, struct Cyc_Cstdio___sFILE*
__s); extern int fseek( struct Cyc_Cstdio___sFILE* __stream, int __off, int
__whence); extern int ftell( struct Cyc_Cstdio___sFILE* __stream); extern void
rewind( struct Cyc_Cstdio___sFILE* __stream); extern int fgetpos( struct Cyc_Cstdio___sFILE*
__stream, int* __pos); extern int fsetpos( struct Cyc_Cstdio___sFILE* __stream,
int* __pos); extern void clearerr( struct Cyc_Cstdio___sFILE* __stream); extern
int feof( struct Cyc_Cstdio___sFILE* __stream); extern int ferror( struct Cyc_Cstdio___sFILE*
__stream); extern void perror( unsigned char* __s); extern int fileno( struct
Cyc_Cstdio___sFILE* __stream); extern struct Cyc_Cstdio___sFILE* popen(
unsigned char*, unsigned char*); extern int pclose( struct Cyc_Cstdio___sFILE*
__stream); struct Cyc_Std___sFILE{ struct Cyc_Cstdio___sFILE* file; } ; struct
Cyc_Std___sFILE* Cyc_Std_fromCfile( struct Cyc_Cstdio___sFILE* cf){ return(
unsigned int) cf?({ struct Cyc_Std___sFILE* _temp15=( struct Cyc_Std___sFILE*)
_cycalloc( sizeof( struct Cyc_Std___sFILE)); _temp15->file= cf; _temp15;}): 0;}
int Cyc_Std_remove( struct _tagged_arr filename){ return remove(
string_to_Cstring( filename));} int Cyc_Std_rename( struct _tagged_arr
old_filename, struct _tagged_arr new_filename){ return rename( string_to_Cstring(
old_filename), string_to_Cstring( new_filename));} int Cyc_Std_fclose( struct
Cyc_Std___sFILE* f){ int r= fclose(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));
if( r ==  0){ f->file= 0;} return r;} int Cyc_Std_fflush( struct Cyc_Std___sFILE*
f){ return( unsigned int) f? fflush( f->file): fflush( 0);} struct Cyc_Std___sFILE*
Cyc_Std_freopen( struct _tagged_arr x, struct _tagged_arr y, struct Cyc_Std___sFILE*
f){ struct Cyc_Cstdio___sFILE* cf= freopen( string_to_Cstring( x),
string_to_Cstring( y),( struct Cyc_Cstdio___sFILE*) _check_null( f->file));
return( unsigned int) cf?({ struct Cyc_Std___sFILE* _temp16=( struct Cyc_Std___sFILE*)
_cycalloc( sizeof( struct Cyc_Std___sFILE)); _temp16->file= cf; _temp16;}): 0;}
void Cyc_Std_setbuf( struct Cyc_Std___sFILE* f, struct _tagged_arr buf){ Cyc_Std_setvbuf(
f, buf,( unsigned int) buf.curr? 0: 2, 1024);} void Cyc_Std_setbuffer( struct
Cyc_Std___sFILE* f, struct _tagged_arr buf, unsigned int size){ Cyc_Std_setvbuf(
f, buf,( unsigned int) buf.curr? 0: 2, size);} void Cyc_Std_setlinebuf( struct
Cyc_Std___sFILE* f){ Cyc_Std_setvbuf( f, _tag_arr( 0u, 0u, 0u), 1, 0);} int Cyc_Std_setvbuf(
struct Cyc_Std___sFILE* f, struct _tagged_arr buf, int mode, unsigned int size){
if( _get_arr_size( buf, sizeof( unsigned char)) <  size){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp17=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp17[ 0]=({ struct Cyc_Core_Failure_struct
_temp18; _temp18.tag= Cyc_Core_Failure; _temp18.f1= _tag_arr("setvbuf: buffer insufficient",
sizeof( unsigned char), 29u); _temp18;}); _temp17;}));} return setvbuf(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file), underlying_Cstring(( struct
_tagged_arr) buf), mode, size);} int Cyc_Std_fgetc( struct Cyc_Std___sFILE* f){
return fgetc(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} struct
_tagged_arr Cyc_Std_fgets( struct _tagged_arr s, int n, struct Cyc_Std___sFILE*
f){ unsigned char* result; unsigned char* buffer= underlying_Cstring(( struct
_tagged_arr) s); unsigned int len= _get_arr_size( s, sizeof( unsigned char)); n=(
int)( len <  n? len:( unsigned int) n); result= fgets( buffer, n,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file)); if( result ==  0){ return _tag_arr( 0u, 0u, 0u);} else{
return s;}} int Cyc_Std_fputc( int i, struct Cyc_Std___sFILE* f){ return fputc(
i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_fputs(
struct _tagged_arr s, struct Cyc_Std___sFILE* f){ return fputs(
string_to_Cstring( s),( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int
Cyc_Std_getc( struct Cyc_Std___sFILE* f){ return getc(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Std_putc( int i, struct Cyc_Std___sFILE* f){
return putc( i,( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_puts(
struct _tagged_arr s){ return puts( string_to_Cstring( s));} int Cyc_Std_ungetc(
int i, struct Cyc_Std___sFILE* f){ return ungetc( i,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} unsigned int Cyc_Std_fread( struct _tagged_arr ptr,
unsigned int size, unsigned int nmemb, struct Cyc_Std___sFILE* f){ if( size * 
nmemb >  _get_arr_size( ptr, sizeof( unsigned char))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp19=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp19[ 0]=({ struct Cyc_Core_Failure_struct
_temp20; _temp20.tag= Cyc_Core_Failure; _temp20.f1= _tag_arr("fread: buffer insufficient",
sizeof( unsigned char), 27u); _temp20;}); _temp19;}));} return fread(
underlying_Cstring(( struct _tagged_arr) ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} unsigned int Cyc_Std_fwrite( struct _tagged_arr ptr,
unsigned int size, unsigned int nmemb, struct Cyc_Std___sFILE* f){ if( size * 
nmemb >  _get_arr_size( ptr, sizeof( unsigned char))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp21=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp21[ 0]=({ struct Cyc_Core_Failure_struct
_temp22; _temp22.tag= Cyc_Core_Failure; _temp22.f1= _tag_arr("fwrite: buffer insufficient",
sizeof( unsigned char), 28u); _temp22;}); _temp21;}));} return fwrite(
underlying_Cstring( ptr), size, nmemb,( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Std_fgetpos( struct Cyc_Std___sFILE* f, int* x){ return
fgetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file), x);} int Cyc_Std_fseek(
struct Cyc_Std___sFILE* f, int offset, int whence){ return fseek(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file), offset, whence);} int Cyc_Std_fsetpos( struct Cyc_Std___sFILE*
f, int* x){ return fsetpos(( struct Cyc_Cstdio___sFILE*) _check_null( f->file),
x);} int Cyc_Std_ftell( struct Cyc_Std___sFILE* f){ return ftell(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} void Cyc_Std_rewind( struct Cyc_Std___sFILE* f){ rewind((
struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void Cyc_Std_clearerr(
struct Cyc_Std___sFILE* f){ clearerr(( struct Cyc_Cstdio___sFILE*) _check_null(
f->file));} int Cyc_Std_feof( struct Cyc_Std___sFILE* f){ return feof(( struct
Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_ferror( struct Cyc_Std___sFILE*
f){ return ferror(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} void
Cyc_Std_perror( struct _tagged_arr s){ perror( string_to_Cstring( s));} struct
Cyc_Std___sFILE* Cyc_Std_fopen( struct _tagged_arr name, struct _tagged_arr type){
struct Cyc_Cstdio___sFILE* cf= fopen( string_to_Cstring( name),
string_to_Cstring( type)); return( unsigned int) cf?({ struct Cyc_Std___sFILE*
_temp23=( struct Cyc_Std___sFILE*) _cycalloc( sizeof( struct Cyc_Std___sFILE));
_temp23->file= cf; _temp23;}): 0;} struct Cyc_Std___sFILE* Cyc_Std_fdopen( int i,
struct _tagged_arr s){ struct Cyc_Cstdio___sFILE* cf= fdopen( i,
string_to_Cstring( s)); return( unsigned int) cf?({ struct Cyc_Std___sFILE*
_temp24=( struct Cyc_Std___sFILE*) _cycalloc( sizeof( struct Cyc_Std___sFILE));
_temp24->file= cf; _temp24;}): 0;} int Cyc_Std_fileno( struct Cyc_Std___sFILE* f){
return fileno(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} int Cyc_Std_getw(
struct Cyc_Std___sFILE* f){ return getw(( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} int Cyc_Std_pclose( struct Cyc_Std___sFILE* f){ return
pclose(( struct Cyc_Cstdio___sFILE*) _check_null( f->file));} struct Cyc_Std___sFILE*
Cyc_Std_popen( struct _tagged_arr s, struct _tagged_arr m){ struct Cyc_Cstdio___sFILE*
cf= popen( string_to_Cstring( s), string_to_Cstring( m)); return( unsigned int)
cf?({ struct Cyc_Std___sFILE* _temp25=( struct Cyc_Std___sFILE*) _cycalloc(
sizeof( struct Cyc_Std___sFILE)); _temp25->file= cf; _temp25;}): 0;} int Cyc_Std_putw(
int i, struct Cyc_Std___sFILE* f){ return putw( i,( struct Cyc_Cstdio___sFILE*)
_check_null( f->file));} unsigned char Cyc_Std_FileCloseError[ 19u]="\000\000\000\000FileCloseError";
unsigned char Cyc_Std_FileOpenError[ 18u]="\000\000\000\000FileOpenError";
struct Cyc_Std___sFILE* Cyc_Std_file_open( struct _tagged_arr fname, struct
_tagged_arr mode){ struct Cyc_Std___sFILE* f= Cyc_Std_fopen( fname, mode); if( f
==  0){ struct _tagged_arr fn=({ unsigned int _temp28= _get_arr_size( fname,
sizeof( unsigned char)); unsigned char* _temp29=( unsigned char*)
_cycalloc_atomic( _check_times( sizeof( unsigned char), _temp28)); struct
_tagged_arr _temp31= _tag_arr( _temp29, sizeof( unsigned char), _get_arr_size(
fname, sizeof( unsigned char)));{ unsigned int _temp30= _temp28; unsigned int i;
for( i= 0; i <  _temp30; i ++){ _temp29[ i]=(( const unsigned char*) fname.curr)[(
int) i];}}; _temp31;});( int) _throw(( void*)({ struct Cyc_Std_FileOpenError_struct*
_temp26=( struct Cyc_Std_FileOpenError_struct*) _cycalloc( sizeof( struct Cyc_Std_FileOpenError_struct));
_temp26[ 0]=({ struct Cyc_Std_FileOpenError_struct _temp27; _temp27.tag= Cyc_Std_FileOpenError;
_temp27.f1= fn; _temp27;}); _temp26;}));} return( struct Cyc_Std___sFILE*)
_check_null( f);} void Cyc_Std_file_close( struct Cyc_Std___sFILE* f){ if( Cyc_Std_fclose(
f) !=  0){( int) _throw(( void*) Cyc_Std_FileCloseError);}} extern unsigned char
Cyc_Array_Array_mismatch[ 19u]; struct Cyc_Std__Div{ int quot; int rem; } ;
struct Cyc_Std__Ldiv{ int quot; int rem; } ; extern int abs( int __x); extern
int atexit( void(* __func)()); extern struct Cyc_Std__Div div( int __numer, int
__denom); extern struct Cyc_Std__Ldiv ldiv( int __numer, int __denom); extern
int random(); extern void srandom( unsigned int __seed); extern int rand();
extern void srand( unsigned int __seed); extern int rand_r( unsigned int* __seed);
extern int grantpt( int __fd); extern int unlockpt( int __fd); extern double Cyc_Std_atof(
struct _tagged_arr); extern int Cyc_Std_atoi( struct _tagged_arr); extern int
Cyc_Std_atol( struct _tagged_arr); extern struct _tagged_arr Cyc_Std_getenv(
struct _tagged_arr); extern double Cyc_Std_strtod( struct _tagged_arr n, struct
_tagged_arr* end); extern int Cyc_Std_strtol( struct _tagged_arr n, struct
_tagged_arr* end, int base); extern unsigned int Cyc_Std_strtoul( struct
_tagged_arr n, struct _tagged_arr* end, int base); extern unsigned int Cyc_Std_mstrtoul(
struct _tagged_arr n, struct _tagged_arr* endptr, int base); extern void Cyc_Std_qsort(
struct _tagged_arr tab, unsigned int nmemb, unsigned int szmemb, int(* compar)(
const void*, const void*)); extern int Cyc_Std_system( struct _tagged_arr);
extern void Cyc_Std_free( struct _tagged_arr); extern double atof( unsigned char*);
extern int atoi( unsigned char*); extern int atol( unsigned char*); extern
unsigned char* getenv( unsigned char*); extern int putenv( unsigned char*);
extern double strtod( unsigned char*, unsigned char**); extern int strtol(
unsigned char*, unsigned char**, int); extern unsigned int strtoul(
unsigned char*, unsigned char**, int); extern void qsort( void* base,
unsigned int nmemb, unsigned int size, int(* compar)( const void*, const void*));
extern int system( unsigned char*); double Cyc_Std_atof( struct _tagged_arr
_nptr){ return atof( string_to_Cstring( _nptr));} int Cyc_Std_atoi( struct
_tagged_arr _nptr){ return atoi( string_to_Cstring( _nptr));} int Cyc_Std_atol(
struct _tagged_arr _nptr){ return atol( string_to_Cstring( _nptr));} struct
_tagged_arr Cyc_Std_getenv( struct _tagged_arr name){ return Cstring_to_string(
getenv( string_to_Cstring( name)));} int Cyc_Std_putenv( struct _tagged_arr s){
return putenv( string_to_Cstring( s));} static void Cyc_Std_check_valid_cstring(
struct _tagged_arr s){ if( s.curr == (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp32=( struct
Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp32[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp33; _temp33.tag= Cyc_Core_Invalid_argument;
_temp33.f1= _tag_arr("strtox NULL pointer", sizeof( unsigned char), 20u);
_temp33;}); _temp32;}));}{ int found_zero= 0;{ int i=( int)( _get_arr_size( s,
sizeof( unsigned char)) -  1); for( 0; i >=  0; i --){ if((( const unsigned char*)
s.curr)[ i] == '\000'){ found_zero= 1; break;}}} if( ! found_zero){( int) _throw((
void*)({ struct Cyc_Core_Invalid_argument_struct* _temp34=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp34[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp35; _temp35.tag= Cyc_Core_Invalid_argument;
_temp35.f1= _tag_arr("strtox: not a C string", sizeof( unsigned char), 23u);
_temp35;}); _temp34;}));}}} double Cyc_Std_strtod( struct _tagged_arr nptr,
struct _tagged_arr* endptr){ Cyc_Std_check_valid_cstring( nptr);{ unsigned char*
c= underlying_Cstring( nptr); unsigned char* e= endptr ==  0? 0: c; double d=
strtod( c,( unsigned char**)& e); if( endptr !=  0){ int n=( int)(( unsigned int)
e - ( unsigned int) c);* endptr= _tagged_arr_plus( nptr, sizeof( unsigned char),
n);} return d;}} int Cyc_Std_strtol( struct _tagged_arr n, struct _tagged_arr*
endptr, int base){ Cyc_Std_check_valid_cstring( n);{ unsigned char* c=
underlying_Cstring( n); unsigned char* e= endptr ==  0? 0: c; int r= strtol( c,(
unsigned char**)& e, base); if( endptr !=  0){ int m=( int)(( unsigned int) e - (
unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof( unsigned char), m);}
return r;}} unsigned int Cyc_Std_strtoul( struct _tagged_arr n, struct
_tagged_arr* endptr, int base){ Cyc_Std_check_valid_cstring( n);{ unsigned char*
c= underlying_Cstring( n); unsigned char* e= endptr ==  0? 0: c; unsigned int r=
strtoul( c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)((
unsigned int) e - ( unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof(
unsigned char), m);} return r;}} unsigned int Cyc_Std_mstrtoul( struct
_tagged_arr n, struct _tagged_arr* endptr, int base){ Cyc_Std_check_valid_cstring((
struct _tagged_arr) n);{ unsigned char* c= underlying_Cstring(( struct
_tagged_arr) n); unsigned char* e= endptr ==  0? 0: c; unsigned int r= strtoul(
c,( unsigned char**)& e, base); if( endptr !=  0){ int m=( int)(( unsigned int)
e - ( unsigned int) c);* endptr= _tagged_arr_plus( n, sizeof( unsigned char), m);}
return r;}} void Cyc_Std_qsort( struct _tagged_arr tab, unsigned int nmemb,
unsigned int szmemb, int(* compar)( const void*, const void*)){ if( tab.curr == (
_tag_arr( 0u, 0u, 0u)).curr? 1: _get_arr_size( tab, sizeof( void)) <  nmemb){(
int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp36=( struct
Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct));
_temp36[ 0]=({ struct Cyc_Core_Invalid_argument_struct _temp37; _temp37.tag= Cyc_Core_Invalid_argument;
_temp37.f1= _tag_arr("Std::qsort", sizeof( unsigned char), 11u); _temp37;});
_temp36;}));} qsort(( void*) _check_null( _untag_arr( tab, sizeof( void), 1u)),
nmemb,( unsigned int) szmemb,( int(*)( const void*, const void*)) compar);} int
Cyc_Std_system( struct _tagged_arr cmd){ return system( string_to_Cstring( cmd));}
void Cyc_Std_free( struct _tagged_arr ptr){;} extern int accept( int, const
struct Cyc_Std_sockaddr*, unsigned int*); extern int accept_in( int, const
struct Cyc_Std_sockaddr_in*, unsigned int*); extern int bind( int, const struct
Cyc_Std_sockaddr*, unsigned int); extern int bind_in( int, const struct Cyc_Std_sockaddr_in*,
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
_temp48=( unsigned char*) _cycalloc_atomic( sizeof( unsigned char) *  1);
_temp48[ 0]='\000'; _temp48;}), sizeof( unsigned char), 1u);{ int i= 0; for( 0;
i <  _get_arr_size( args, sizeof( void*)); i ++){ void* _temp38=(( void**) args.curr)[
i]; _LL40: if(*(( int*) _temp38) ==  Cyc_Std_SO_int){ goto _LL41;} else{ goto
_LL42;} _LL42: if(*(( int*) _temp38) ==  Cyc_Std_SO_timeval){ goto _LL43;} else{
goto _LL44;} _LL44: if(*(( int*) _temp38) ==  Cyc_Std_SO_socklenptr){ goto _LL45;}
else{ goto _LL46;} _LL46: if(*(( int*) _temp38) ==  Cyc_Std_SO_socklen){ goto
_LL47;} else{ goto _LL39;} _LL41: res= Cyc_Std_strconcat(( struct _tagged_arr)
res, _tag_arr("|SO_int", sizeof( unsigned char), 8u)); goto _LL39; _LL43: res=
Cyc_Std_strconcat(( struct _tagged_arr) res, _tag_arr("|SO_timeval", sizeof(
unsigned char), 12u)); goto _LL39; _LL45: res= Cyc_Std_strconcat(( struct
_tagged_arr) res, _tag_arr("|SO_socklenptr", sizeof( unsigned char), 15u)); goto
_LL39; _LL47: res= Cyc_Std_strconcat(( struct _tagged_arr) res, _tag_arr("|SO_socklen",
sizeof( unsigned char), 12u)); goto _LL39; _LL39:;}} return res;} struct _tuple0{
void* f1; void* f2; } ; int Cyc_Std_accept( int fd, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp49=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp49[ 0]=({ struct Cyc_Std_SocketError_struct _temp50; _temp50.tag= Cyc_Std_SocketError;
_temp50.f1= _tag_arr("accept---need 2 args", sizeof( unsigned char), 21u);
_temp50;}); _temp49;}));}{ struct _tuple0 _temp52=({ struct _tuple0 _temp51;
_temp51.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp51.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp51;}); void* _temp60; unsigned int* _temp62; void* _temp64; struct Cyc_Std_sockaddr_in*
_temp66; void* _temp68; unsigned int* _temp70; void* _temp72; struct Cyc_Std_sockaddr*
_temp74; _LL54: _LL65: _temp64= _temp52.f1; if(*(( int*) _temp64) ==  Cyc_Std_SA_sockaddr_in){
_LL67: _temp66=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp64)->f1; goto
_LL61;} else{ goto _LL56;} _LL61: _temp60= _temp52.f2; if(*(( int*) _temp60) == 
Cyc_Std_SA_socklenptr){ _LL63: _temp62=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp60)->f1; goto _LL55;} else{ goto _LL56;} _LL56: _LL73: _temp72= _temp52.f1;
if(*(( int*) _temp72) ==  Cyc_Std_SA_sockaddr){ _LL75: _temp74=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp72)->f1; goto _LL69;} else{ goto _LL58;} _LL69: _temp68= _temp52.f2; if(*((
int*) _temp68) ==  Cyc_Std_SA_socklenptr){ _LL71: _temp70=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp68)->f1; goto _LL57;} else{ goto _LL58;} _LL58: goto _LL59; _LL55: return
accept_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp66, _temp62); _LL57:
return accept( fd,( const struct Cyc_Std_sockaddr*) _temp74, _temp70); _LL59:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp76=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp76[ 0]=({ struct
Cyc_Std_SocketError_struct _temp77; _temp77.tag= Cyc_Std_SocketError; _temp77.f1=
_tag_arr("accept---bad args", sizeof( unsigned char), 18u); _temp77;}); _temp76;}));
_LL53:;}} int Cyc_Std_bind( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp78=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp78[ 0]=({ struct Cyc_Std_SocketError_struct _temp79; _temp79.tag= Cyc_Std_SocketError;
_temp79.f1= _tag_arr("bind---need 2 args", sizeof( unsigned char), 19u); _temp79;});
_temp78;}));}{ struct _tuple0 _temp81=({ struct _tuple0 _temp80; _temp80.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp80.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp80;}); void* _temp89;
unsigned int _temp91; void* _temp93; struct Cyc_Std_sockaddr_in* _temp95; void*
_temp97; unsigned int _temp99; void* _temp101; struct Cyc_Std_sockaddr* _temp103;
_LL83: _LL94: _temp93= _temp81.f1; if(*(( int*) _temp93) ==  Cyc_Std_SA_sockaddr_in){
_LL96: _temp95=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp93)->f1; goto
_LL90;} else{ goto _LL85;} _LL90: _temp89= _temp81.f2; if(*(( int*) _temp89) == 
Cyc_Std_SA_socklen){ _LL92: _temp91=(( struct Cyc_Std_SA_socklen_struct*)
_temp89)->f1; goto _LL84;} else{ goto _LL85;} _LL85: _LL102: _temp101= _temp81.f1;
if(*(( int*) _temp101) ==  Cyc_Std_SA_sockaddr){ _LL104: _temp103=(( struct Cyc_Std_SA_sockaddr_struct*)
_temp101)->f1; goto _LL98;} else{ goto _LL87;} _LL98: _temp97= _temp81.f2; if(*((
int*) _temp97) ==  Cyc_Std_SA_socklen){ _LL100: _temp99=(( struct Cyc_Std_SA_socklen_struct*)
_temp97)->f1; goto _LL86;} else{ goto _LL87;} _LL87: goto _LL88; _LL84: return
bind_in( fd,( const struct Cyc_Std_sockaddr_in*) _temp95, _temp91); _LL86:
return bind( fd,( const struct Cyc_Std_sockaddr*) _temp103, _temp99); _LL88:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp105=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp105[ 0]=({ struct
Cyc_Std_SocketError_struct _temp106; _temp106.tag= Cyc_Std_SocketError; _temp106.f1=
_tag_arr("bind---bad args", sizeof( unsigned char), 16u); _temp106;}); _temp105;}));
_LL82:;}} int Cyc_Std_connect( int fd, struct _tagged_arr ap){ if( _get_arr_size(
ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp107=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp107[ 0]=({ struct Cyc_Std_SocketError_struct _temp108; _temp108.tag= Cyc_Std_SocketError;
_temp108.f1= _tag_arr("connect---need 2 args", sizeof( unsigned char), 22u);
_temp108;}); _temp107;}));}{ struct _tuple0 _temp110=({ struct _tuple0 _temp109;
_temp109.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp109.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp109;}); void* _temp118; unsigned int _temp120; void* _temp122; struct Cyc_Std_sockaddr_in*
_temp124; void* _temp126; unsigned int _temp128; void* _temp130; struct Cyc_Std_sockaddr*
_temp132; _LL112: _LL123: _temp122= _temp110.f1; if(*(( int*) _temp122) ==  Cyc_Std_SA_sockaddr_in){
_LL125: _temp124=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp122)->f1; goto
_LL119;} else{ goto _LL114;} _LL119: _temp118= _temp110.f2; if(*(( int*)
_temp118) ==  Cyc_Std_SA_socklen){ _LL121: _temp120=(( struct Cyc_Std_SA_socklen_struct*)
_temp118)->f1; goto _LL113;} else{ goto _LL114;} _LL114: _LL131: _temp130=
_temp110.f1; if(*(( int*) _temp130) ==  Cyc_Std_SA_sockaddr){ _LL133: _temp132=((
struct Cyc_Std_SA_sockaddr_struct*) _temp130)->f1; goto _LL127;} else{ goto
_LL116;} _LL127: _temp126= _temp110.f2; if(*(( int*) _temp126) ==  Cyc_Std_SA_socklen){
_LL129: _temp128=(( struct Cyc_Std_SA_socklen_struct*) _temp126)->f1; goto
_LL115;} else{ goto _LL116;} _LL116: goto _LL117; _LL113: return connect_in( fd,(
const struct Cyc_Std_sockaddr_in*) _temp124, _temp120); _LL115: return connect(
fd,( const struct Cyc_Std_sockaddr*) _temp132, _temp128); _LL117:( int) _throw((
void*)({ struct Cyc_Std_SocketError_struct* _temp134=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp134[ 0]=({ struct
Cyc_Std_SocketError_struct _temp135; _temp135.tag= Cyc_Std_SocketError; _temp135.f1=
_tag_arr("connect---bad args", sizeof( unsigned char), 19u); _temp135;});
_temp134;})); _LL111:;}} int Cyc_Std_getpeername( int fd, struct _tagged_arr ap){
if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct
Cyc_Std_SocketError_struct* _temp136=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp136[ 0]=({ struct
Cyc_Std_SocketError_struct _temp137; _temp137.tag= Cyc_Std_SocketError; _temp137.f1=
_tag_arr("getpeername---need 2 args", sizeof( unsigned char), 26u); _temp137;});
_temp136;}));}{ struct _tuple0 _temp139=({ struct _tuple0 _temp138; _temp138.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp138.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp138;}); void* _temp147;
unsigned int* _temp149; void* _temp151; struct Cyc_Std_sockaddr_in* _temp153;
void* _temp155; unsigned int* _temp157; void* _temp159; struct Cyc_Std_sockaddr*
_temp161; _LL141: _LL152: _temp151= _temp139.f1; if(*(( int*) _temp151) ==  Cyc_Std_SA_sockaddr_in){
_LL154: _temp153=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp151)->f1; goto
_LL148;} else{ goto _LL143;} _LL148: _temp147= _temp139.f2; if(*(( int*)
_temp147) ==  Cyc_Std_SA_socklenptr){ _LL150: _temp149=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp147)->f1; goto _LL142;} else{ goto _LL143;} _LL143: _LL160: _temp159=
_temp139.f1; if(*(( int*) _temp159) ==  Cyc_Std_SA_sockaddr){ _LL162: _temp161=((
struct Cyc_Std_SA_sockaddr_struct*) _temp159)->f1; goto _LL156;} else{ goto
_LL145;} _LL156: _temp155= _temp139.f2; if(*(( int*) _temp155) ==  Cyc_Std_SA_socklenptr){
_LL158: _temp157=(( struct Cyc_Std_SA_socklenptr_struct*) _temp155)->f1; goto
_LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL142: return getpeername_in(
fd,( const struct Cyc_Std_sockaddr_in*) _temp153, _temp149); _LL144: return
getpeername( fd,( const struct Cyc_Std_sockaddr*) _temp161, _temp157); _LL146:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp163=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp163[ 0]=({ struct
Cyc_Std_SocketError_struct _temp164; _temp164.tag= Cyc_Std_SocketError; _temp164.f1=
_tag_arr("getpeername---bad args", sizeof( unsigned char), 23u); _temp164;});
_temp163;})); _LL140:;}} int Cyc_Std_getsockname( int fd, struct _tagged_arr ap){
if( _get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct
Cyc_Std_SocketError_struct* _temp165=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp165[ 0]=({ struct
Cyc_Std_SocketError_struct _temp166; _temp166.tag= Cyc_Std_SocketError; _temp166.f1=
_tag_arr("getsockname---need 2 args", sizeof( unsigned char), 26u); _temp166;});
_temp165;}));}{ struct _tuple0 _temp168=({ struct _tuple0 _temp167; _temp167.f1=*((
void**) _check_unknown_subscript( ap, sizeof( void*), 0)); _temp167.f2=*(( void**)
_check_unknown_subscript( ap, sizeof( void*), 1)); _temp167;}); void* _temp176;
unsigned int* _temp178; void* _temp180; struct Cyc_Std_sockaddr_in* _temp182;
void* _temp184; unsigned int* _temp186; void* _temp188; struct Cyc_Std_sockaddr*
_temp190; _LL170: _LL181: _temp180= _temp168.f1; if(*(( int*) _temp180) ==  Cyc_Std_SA_sockaddr_in){
_LL183: _temp182=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp180)->f1; goto
_LL177;} else{ goto _LL172;} _LL177: _temp176= _temp168.f2; if(*(( int*)
_temp176) ==  Cyc_Std_SA_socklenptr){ _LL179: _temp178=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp176)->f1; goto _LL171;} else{ goto _LL172;} _LL172: _LL189: _temp188=
_temp168.f1; if(*(( int*) _temp188) ==  Cyc_Std_SA_sockaddr){ _LL191: _temp190=((
struct Cyc_Std_SA_sockaddr_struct*) _temp188)->f1; goto _LL185;} else{ goto
_LL174;} _LL185: _temp184= _temp168.f2; if(*(( int*) _temp184) ==  Cyc_Std_SA_socklenptr){
_LL187: _temp186=(( struct Cyc_Std_SA_socklenptr_struct*) _temp184)->f1; goto
_LL173;} else{ goto _LL174;} _LL174: goto _LL175; _LL171: return getsockname_in(
fd,( const struct Cyc_Std_sockaddr_in*) _temp182, _temp178); _LL173: return
getsockname( fd,( const struct Cyc_Std_sockaddr*) _temp190, _temp186); _LL175:(
int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp192=( struct Cyc_Std_SocketError_struct*)
_cycalloc( sizeof( struct Cyc_Std_SocketError_struct)); _temp192[ 0]=({ struct
Cyc_Std_SocketError_struct _temp193; _temp193.tag= Cyc_Std_SocketError; _temp193.f1=
_tag_arr("getsockname---bad args", sizeof( unsigned char), 23u); _temp193;});
_temp192;})); _LL169:;}} int Cyc_Std_recvfrom( int fd, struct _tagged_arr buf,
unsigned int n, int flags, struct _tagged_arr ap){ if( _get_arr_size( ap,
sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp194=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp194[ 0]=({ struct Cyc_Std_SocketError_struct _temp195; _temp195.tag= Cyc_Std_SocketError;
_temp195.f1= _tag_arr("recvfrom---need 2 args", sizeof( unsigned char), 23u);
_temp195;}); _temp194;}));}{ struct _tuple0 _temp197=({ struct _tuple0 _temp196;
_temp196.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp196.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp196;}); void* _temp205; unsigned int* _temp207; void* _temp209; struct Cyc_Std_sockaddr_in*
_temp211; void* _temp213; unsigned int* _temp215; void* _temp217; struct Cyc_Std_sockaddr*
_temp219; _LL199: _LL210: _temp209= _temp197.f1; if(*(( int*) _temp209) ==  Cyc_Std_SA_sockaddr_in){
_LL212: _temp211=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp209)->f1; goto
_LL206;} else{ goto _LL201;} _LL206: _temp205= _temp197.f2; if(*(( int*)
_temp205) ==  Cyc_Std_SA_socklenptr){ _LL208: _temp207=(( struct Cyc_Std_SA_socklenptr_struct*)
_temp205)->f1; goto _LL200;} else{ goto _LL201;} _LL201: _LL218: _temp217=
_temp197.f1; if(*(( int*) _temp217) ==  Cyc_Std_SA_sockaddr){ _LL220: _temp219=((
struct Cyc_Std_SA_sockaddr_struct*) _temp217)->f1; goto _LL214;} else{ goto
_LL203;} _LL214: _temp213= _temp197.f2; if(*(( int*) _temp213) ==  Cyc_Std_SA_socklenptr){
_LL216: _temp215=(( struct Cyc_Std_SA_socklenptr_struct*) _temp213)->f1; goto
_LL202;} else{ goto _LL203;} _LL203: goto _LL204; _LL200: return recvfrom_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp211, _temp207); _LL202:
return recvfrom( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp219,
_temp215); _LL204:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp221=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp221[ 0]=({ struct Cyc_Std_SocketError_struct _temp222; _temp222.tag= Cyc_Std_SocketError;
_temp222.f1= _tag_arr("recvfrom---bad args", sizeof( unsigned char), 20u);
_temp222;}); _temp221;})); _LL198:;}} int Cyc_Std_sendto( int fd, struct
_tagged_arr buf, unsigned int n, int flags, struct _tagged_arr ap){ if(
_get_arr_size( ap, sizeof( void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp223=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp223[ 0]=({ struct Cyc_Std_SocketError_struct _temp224; _temp224.tag= Cyc_Std_SocketError;
_temp224.f1= _tag_arr("sendto---need 2 args", sizeof( unsigned char), 21u);
_temp224;}); _temp223;}));}{ struct _tuple0 _temp226=({ struct _tuple0 _temp225;
_temp225.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp225.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp225;}); void* _temp234; unsigned int _temp236; void* _temp238; struct Cyc_Std_sockaddr_in*
_temp240; void* _temp242; unsigned int _temp244; void* _temp246; struct Cyc_Std_sockaddr*
_temp248; _LL228: _LL239: _temp238= _temp226.f1; if(*(( int*) _temp238) ==  Cyc_Std_SA_sockaddr_in){
_LL241: _temp240=(( struct Cyc_Std_SA_sockaddr_in_struct*) _temp238)->f1; goto
_LL235;} else{ goto _LL230;} _LL235: _temp234= _temp226.f2; if(*(( int*)
_temp234) ==  Cyc_Std_SA_socklen){ _LL237: _temp236=(( struct Cyc_Std_SA_socklen_struct*)
_temp234)->f1; goto _LL229;} else{ goto _LL230;} _LL230: _LL247: _temp246=
_temp226.f1; if(*(( int*) _temp246) ==  Cyc_Std_SA_sockaddr){ _LL249: _temp248=((
struct Cyc_Std_SA_sockaddr_struct*) _temp246)->f1; goto _LL243;} else{ goto
_LL232;} _LL243: _temp242= _temp226.f2; if(*(( int*) _temp242) ==  Cyc_Std_SA_socklen){
_LL245: _temp244=(( struct Cyc_Std_SA_socklen_struct*) _temp242)->f1; goto
_LL231;} else{ goto _LL232;} _LL232: goto _LL233; _LL229: return sendto_in( fd,
buf, n, flags,( const struct Cyc_Std_sockaddr_in*) _temp240, _temp236); _LL231:
return sendto( fd, buf, n, flags,( const struct Cyc_Std_sockaddr*) _temp248,
_temp244); _LL233:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp250=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp250[ 0]=({ struct Cyc_Std_SocketError_struct _temp251; _temp251.tag= Cyc_Std_SocketError;
_temp251.f1= _tag_arr("sendto---bad args", sizeof( unsigned char), 18u);
_temp251;}); _temp250;})); _LL227:;}} int Cyc_Std_send( int fd, struct
_tagged_arr buf, unsigned int n, int flags){ return send_wrapped( fd, buf, n,
flags);} int Cyc_Std_recv( int fd, struct _tagged_arr buf, unsigned int n, int
flags){ return recv_wrapped( fd, buf, n, flags);} int Cyc_Std_getsockopt( int fd,
int level, int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof(
void*)) !=  2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp252=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp252[ 0]=({ struct Cyc_Std_SocketError_struct _temp253; _temp253.tag= Cyc_Std_SocketError;
_temp253.f1= _tag_arr("getsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp253;}); _temp252;}));}{ struct _tuple0 _temp255=({ struct _tuple0 _temp254;
_temp254.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp254.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp254;}); void* _temp267; unsigned int* _temp269; void* _temp271; int*
_temp273; void* _temp275; int* _temp277; void* _temp279; int* _temp281; void*
_temp283; unsigned int* _temp285; void* _temp287; struct Cyc_Std_timeval*
_temp289; void* _temp291; int* _temp293; void* _temp295; struct Cyc_Std_timeval*
_temp297; _LL257: _LL272: _temp271= _temp255.f1; if(*(( int*) _temp271) ==  Cyc_Std_SO_int){
_LL274: _temp273=(( struct Cyc_Std_SO_int_struct*) _temp271)->f1; goto _LL268;}
else{ goto _LL259;} _LL268: _temp267= _temp255.f2; if(*(( int*) _temp267) == 
Cyc_Std_SO_socklenptr){ _LL270: _temp269=(( struct Cyc_Std_SO_socklenptr_struct*)
_temp267)->f1; goto _LL258;} else{ goto _LL259;} _LL259: _LL280: _temp279=
_temp255.f1; if(*(( int*) _temp279) ==  Cyc_Std_SO_int){ _LL282: _temp281=((
struct Cyc_Std_SO_int_struct*) _temp279)->f1; goto _LL276;} else{ goto _LL261;}
_LL276: _temp275= _temp255.f2; if(*(( int*) _temp275) ==  Cyc_Std_SO_int){
_LL278: _temp277=(( struct Cyc_Std_SO_int_struct*) _temp275)->f1; goto _LL260;}
else{ goto _LL261;} _LL261: _LL288: _temp287= _temp255.f1; if(*(( int*) _temp287)
==  Cyc_Std_SO_timeval){ _LL290: _temp289=(( struct Cyc_Std_SO_timeval_struct*)
_temp287)->f1; goto _LL284;} else{ goto _LL263;} _LL284: _temp283= _temp255.f2;
if(*(( int*) _temp283) ==  Cyc_Std_SO_socklenptr){ _LL286: _temp285=(( struct
Cyc_Std_SO_socklenptr_struct*) _temp283)->f1; goto _LL262;} else{ goto _LL263;}
_LL263: _LL296: _temp295= _temp255.f1; if(*(( int*) _temp295) ==  Cyc_Std_SO_timeval){
_LL298: _temp297=(( struct Cyc_Std_SO_timeval_struct*) _temp295)->f1; goto
_LL292;} else{ goto _LL265;} _LL292: _temp291= _temp255.f2; if(*(( int*)
_temp291) ==  Cyc_Std_SO_int){ _LL294: _temp293=(( struct Cyc_Std_SO_int_struct*)
_temp291)->f1; goto _LL264;} else{ goto _LL265;} _LL265: goto _LL266; _LL258:
return getsockopt_int( fd, level, optname, _temp273, _temp269); _LL260: return
getsockopt_int( fd, level, optname, _temp281,( unsigned int*) _temp277); _LL262:
return getsockopt_timeval( fd, level, optname, _temp289, _temp285); _LL264:
return getsockopt_timeval( fd, level, optname, _temp297,( unsigned int*)
_temp293); _LL266:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp299=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp299[ 0]=({ struct Cyc_Std_SocketError_struct _temp300; _temp300.tag= Cyc_Std_SocketError;
_temp300.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp302;
_temp302.tag= Cyc_Std_String_pa; _temp302.f1=( struct _tagged_arr) Cyc_Std_sopts2string(
ap);{ void* _temp301[ 1u]={& _temp302}; Cyc_Std_aprintf( _tag_arr("getsockopt---bad args %s",
sizeof( unsigned char), 25u), _tag_arr( _temp301, sizeof( void*), 1u));}});
_temp300;}); _temp299;})); _LL256:;}} int Cyc_Std_setsockopt( int fd, int level,
int optname, struct _tagged_arr ap){ if( _get_arr_size( ap, sizeof( void*)) != 
2){( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct* _temp303=( struct
Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp303[ 0]=({ struct Cyc_Std_SocketError_struct _temp304; _temp304.tag= Cyc_Std_SocketError;
_temp304.f1= _tag_arr("setsockopt---need 2 args", sizeof( unsigned char), 25u);
_temp304;}); _temp303;}));}{ struct _tuple0 _temp306=({ struct _tuple0 _temp305;
_temp305.f1=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 0));
_temp305.f2=*(( void**) _check_unknown_subscript( ap, sizeof( void*), 1));
_temp305;}); void* _temp314; unsigned int _temp316; void* _temp318; int*
_temp320; void* _temp322; unsigned int _temp324; void* _temp326; struct Cyc_Std_timeval*
_temp328; _LL308: _LL319: _temp318= _temp306.f1; if(*(( int*) _temp318) ==  Cyc_Std_SO_int){
_LL321: _temp320=(( struct Cyc_Std_SO_int_struct*) _temp318)->f1; goto _LL315;}
else{ goto _LL310;} _LL315: _temp314= _temp306.f2; if(*(( int*) _temp314) == 
Cyc_Std_SO_socklen){ _LL317: _temp316=(( struct Cyc_Std_SO_socklen_struct*)
_temp314)->f1; goto _LL309;} else{ goto _LL310;} _LL310: _LL327: _temp326=
_temp306.f1; if(*(( int*) _temp326) ==  Cyc_Std_SO_timeval){ _LL329: _temp328=((
struct Cyc_Std_SO_timeval_struct*) _temp326)->f1; goto _LL323;} else{ goto
_LL312;} _LL323: _temp322= _temp306.f2; if(*(( int*) _temp322) ==  Cyc_Std_SO_socklen){
_LL325: _temp324=(( struct Cyc_Std_SO_socklen_struct*) _temp322)->f1; goto
_LL311;} else{ goto _LL312;} _LL312: goto _LL313; _LL309: return setsockopt_int(
fd, level, optname,( const int*) _temp320, _temp316); _LL311: return
setsockopt_timeval( fd, level, optname,( const struct Cyc_Std_timeval*) _temp328,
_temp324); _LL313:( int) _throw(( void*)({ struct Cyc_Std_SocketError_struct*
_temp330=( struct Cyc_Std_SocketError_struct*) _cycalloc( sizeof( struct Cyc_Std_SocketError_struct));
_temp330[ 0]=({ struct Cyc_Std_SocketError_struct _temp331; _temp331.tag= Cyc_Std_SocketError;
_temp331.f1=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp333;
_temp333.tag= Cyc_Std_String_pa; _temp333.f1=( struct _tagged_arr) Cyc_Std_sopts2string(
ap);{ void* _temp332[ 1u]={& _temp333}; Cyc_Std_aprintf( _tag_arr("getsockopt---bad args %s",
sizeof( unsigned char), 25u), _tag_arr( _temp332, sizeof( void*), 1u));}});
_temp331;}); _temp330;})); _LL307:;}} extern unsigned char* asctime( const
struct Cyc_Std_tm* timeptr); extern unsigned char* ctime( const int* timep);
extern unsigned int strftime( unsigned char* s, unsigned int maxsize,
unsigned char* fmt, const struct Cyc_Std_tm* t); extern unsigned char* asctime_r(
const struct Cyc_Std_tm*, unsigned char*); extern unsigned char* ctime_r( const
int*, unsigned char*); struct _tagged_arr Cyc_Std_asctime( const struct Cyc_Std_tm*
timeptr){ return wrap_Cstring_as_string( asctime( timeptr), - 1);} struct
_tagged_arr Cyc_Std_ctime( const int* timep){ return wrap_Cstring_as_string(
ctime( timep), - 1);} unsigned int Cyc_Std_strftime( struct _tagged_arr s,
unsigned int maxsize, struct _tagged_arr fmt, const struct Cyc_Std_tm* t){
unsigned int m= _get_arr_size( s, sizeof( unsigned char)) <  maxsize?
_get_arr_size( s, sizeof( unsigned char)): maxsize; return strftime(
underlying_Cstring( s), m, underlying_Cstring( fmt), t);} struct _tagged_arr Cyc_Std_asctime_r(
const struct Cyc_Std_tm* t, struct _tagged_arr s){ struct _tagged_arr _temp334=
wrap_Cstring_as_string( asctime( t), - 1); if( Cyc_Std_strlen(( struct
_tagged_arr) _temp334) +  1 >  _get_arr_size( s, sizeof( unsigned char))){( int)
_throw(( void*)({ struct Cyc_Core_Invalid_argument_struct* _temp335=( struct Cyc_Core_Invalid_argument_struct*)
_cycalloc( sizeof( struct Cyc_Core_Invalid_argument_struct)); _temp335[ 0]=({
struct Cyc_Core_Invalid_argument_struct _temp336; _temp336.tag= Cyc_Core_Invalid_argument;
_temp336.f1= _tag_arr("Time::asctime_r: string too small", sizeof( unsigned char),
34u); _temp336;}); _temp335;}));} else{ Cyc_Std_strcpy( s,( struct _tagged_arr)
_temp334); return s;}} struct _tagged_arr Cyc_Std_ctime_r( const int* t, struct
_tagged_arr s){ struct _tagged_arr _temp337= wrap_Cstring_as_string( ctime( t),
- 1); if( Cyc_Std_strlen(( struct _tagged_arr) _temp337) +  1 >  _get_arr_size(
s, sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Invalid_argument_struct*
_temp338=( struct Cyc_Core_Invalid_argument_struct*) _cycalloc( sizeof( struct
Cyc_Core_Invalid_argument_struct)); _temp338[ 0]=({ struct Cyc_Core_Invalid_argument_struct
_temp339; _temp339.tag= Cyc_Core_Invalid_argument; _temp339.f1= _tag_arr("Time::ctime_r: string too small",
sizeof( unsigned char), 32u); _temp339;}); _temp338;}));} else{ Cyc_Std_strcpy(
s,( struct _tagged_arr) _temp337); return s;}} struct Cyc_Std_option{ struct
_tagged_arr name; int has_arg; int* flag; int val; } ; extern unsigned int alarm(
unsigned int seconds); extern int close( int); extern void _exit( int); extern
int getpid(); extern int getppid(); extern int fork(); extern int fchdir( int);
extern int fchown( int, unsigned int, unsigned int); extern int fsync( int);
extern int ftruncate( int, int); extern int dup( int); extern int dup2( int, int);
extern int setsid(); extern int getsid( int pid); extern unsigned int getuid();
extern int setuid( unsigned int uid); extern unsigned int geteuid(); extern int
seteuid( unsigned int euid); extern unsigned int getgid(); extern int setgid(
unsigned int gid); extern unsigned int getegid(); extern int setegid(
unsigned int egid); extern int nice( int); extern int pause(); extern int pipe(
int* filedes); extern int lseek( int filedes, int offset, int whence); extern
unsigned int sleep( unsigned int); extern int isatty( int); extern int daemon(
int nochdir, int noclose); int Cyc_Std_access( struct _tagged_arr, int); int Cyc_Std_chdir(
struct _tagged_arr); int Cyc_Std_chown( struct _tagged_arr, unsigned int,
unsigned int); struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf,
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
unsigned char*); extern int chown( unsigned char*, unsigned int, unsigned int);
extern unsigned char* getcwd( unsigned char* buf, unsigned int size); extern int
execv( unsigned char* path, unsigned char** argv); extern int execvp(
unsigned char* file, unsigned char** argv); extern int execve( unsigned char*
path, unsigned char** argv, unsigned char** envp); extern int link(
unsigned char* path1, unsigned char* path2); extern int read( int fd,
unsigned char* buf, unsigned int count); extern int rmdir( unsigned char*);
extern int symlink( unsigned char* path1, unsigned char* path2); extern int
truncate( unsigned char*, int); extern int write( int fd, unsigned char* buf,
unsigned int count); extern int unlink( unsigned char* pathname); extern int
gethostname( unsigned char*, unsigned int); extern int chroot( unsigned char*);
extern unsigned char* getpass( unsigned char*); int Cyc_Std_access( struct
_tagged_arr path, int mode){ return access( string_to_Cstring( path), mode);}
int Cyc_Std_chdir( struct _tagged_arr path){ return chdir( string_to_Cstring(
path));} int Cyc_Std_chown( struct _tagged_arr path, unsigned int owner,
unsigned int group){ return chown( string_to_Cstring( path), owner, group);}
struct _tagged_arr Cyc_Std_getcwd( struct _tagged_arr buf, unsigned int size){
if( !(( unsigned int) buf.curr)? 1: _get_arr_size( buf, sizeof( unsigned char))
<  size){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp340=(
struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp340[ 0]=({ struct Cyc_Core_Failure_struct _temp341; _temp341.tag= Cyc_Core_Failure;
_temp341.f1= _tag_arr("getcwd: invalid buf argument", sizeof( unsigned char), 29u);
_temp341;}); _temp340;}));}{ unsigned char* response= getcwd(( unsigned char*)
_check_null( _untag_arr( buf, sizeof( unsigned char), 0u)), size); return(
unsigned int) response? buf: _tag_arr( 0u, 0u, 0u);}} int Cyc_Std_execl( struct
_tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv){ if((*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) -  1)))).curr != ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp342=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp342[ 0]=({ struct Cyc_Core_Failure_struct
_temp343; _temp343.tag= Cyc_Core_Failure; _temp343.f1= _tag_arr("execl: arg list must be NULL-terminated",
sizeof( unsigned char), 40u); _temp343;}); _temp342;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp344= 1 +  _get_arr_size( argv, sizeof( struct
_tagged_arr)); unsigned char** _temp345=( unsigned char**) _cycalloc(
_check_times( sizeof( unsigned char*), _temp344)); struct _tagged_arr _temp347=
_tag_arr( _temp345, sizeof( unsigned char*), 1 +  _get_arr_size( argv, sizeof(
struct _tagged_arr)));{ unsigned int _temp346= _temp344; unsigned int i; for( i=
0; i <  _temp346; i ++){ _temp345[ i]= 0;}}; _temp347;});*(( unsigned char**)
_check_unknown_subscript( newargs, sizeof( unsigned char*), 0))=
string_to_Cstring( arg0);{ int i= 0; for( 0; i <  _get_arr_size( argv, sizeof(
struct _tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), i +  1))= string_to_Cstring((( struct
_tagged_arr*) argv.curr)[ i]);}} return execv( string_to_Cstring( path),(
unsigned char**) _check_null( _untag_arr( newargs, sizeof( unsigned char*), 1u)));}}
int Cyc_Std_execlp( struct _tagged_arr path, struct _tagged_arr arg0, struct
_tagged_arr argv){ if((*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr),( int)( _get_arr_size( argv, sizeof( struct
_tagged_arr)) -  1)))).curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp348=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp348[ 0]=({ struct Cyc_Core_Failure_struct
_temp349; _temp349.tag= Cyc_Core_Failure; _temp349.f1= _tag_arr("execl: arg list must be NULL-terminated",
sizeof( unsigned char), 40u); _temp349;}); _temp348;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp350= 1 +  _get_arr_size( argv, sizeof( struct
_tagged_arr)); unsigned char** _temp351=( unsigned char**) _cycalloc(
_check_times( sizeof( unsigned char*), _temp350)); struct _tagged_arr _temp353=
_tag_arr( _temp351, sizeof( unsigned char*), 1 +  _get_arr_size( argv, sizeof(
struct _tagged_arr)));{ unsigned int _temp352= _temp350; unsigned int i; for( i=
0; i <  _temp352; i ++){ _temp351[ i]= 0;}}; _temp353;});*(( unsigned char**)
_check_unknown_subscript( newargs, sizeof( unsigned char*), 0))=
string_to_Cstring( arg0);{ int i= 0; for( 0; i <  _get_arr_size( argv, sizeof(
struct _tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), i +  1))= string_to_Cstring((( struct
_tagged_arr*) argv.curr)[ i]);}} return execvp( string_to_Cstring( path),(
unsigned char**) _check_null( _untag_arr( newargs, sizeof( unsigned char*), 1u)));}}
int Cyc_Std_execve( struct _tagged_arr filename, struct _tagged_arr argv, struct
_tagged_arr envp){ if((*(( struct _tagged_arr*) _check_unknown_subscript( argv,
sizeof( struct _tagged_arr),( int)( _get_arr_size( argv, sizeof( struct
_tagged_arr)) -  1)))).curr != ( _tag_arr( 0u, 0u, 0u)).curr){( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp354=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp354[ 0]=({ struct Cyc_Core_Failure_struct
_temp355; _temp355.tag= Cyc_Core_Failure; _temp355.f1= _tag_arr("execve: arg list must be NULL-terminated",
sizeof( unsigned char), 41u); _temp355;}); _temp354;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp360= _get_arr_size( argv, sizeof( struct
_tagged_arr)); unsigned char** _temp361=( unsigned char**) _cycalloc(
_check_times( sizeof( unsigned char*), _temp360)); struct _tagged_arr _temp363=
_tag_arr( _temp361, sizeof( unsigned char*), _get_arr_size( argv, sizeof( struct
_tagged_arr)));{ unsigned int _temp362= _temp360; unsigned int i; for( i= 0; i < 
_temp362; i ++){ _temp361[ i]= 0;}}; _temp363;});{ int i= 0; for( 0; i < 
_get_arr_size( argv, sizeof( struct _tagged_arr)); i ++){(( unsigned char**)
newargs.curr)[ i]= string_to_Cstring(( struct _tagged_arr)(( struct _tagged_arr*)
argv.curr)[ i]);}}{ struct _tagged_arr newenvp=({ unsigned int _temp356=
_get_arr_size( envp, sizeof( struct _tagged_arr)); unsigned char** _temp357=(
unsigned char**) _cycalloc( _check_times( sizeof( unsigned char*), _temp356));
struct _tagged_arr _temp359= _tag_arr( _temp357, sizeof( unsigned char*),
_get_arr_size( envp, sizeof( struct _tagged_arr)));{ unsigned int _temp358=
_temp356; unsigned int i; for( i= 0; i <  _temp358; i ++){ _temp357[ i]= 0;}};
_temp359;});{ int i= 0; for( 0; i <  _get_arr_size( envp, sizeof( struct
_tagged_arr)); i ++){(( unsigned char**) newenvp.curr)[ i]= string_to_Cstring((
struct _tagged_arr)(( struct _tagged_arr*) envp.curr)[ i]);}} return execve(
string_to_Cstring( filename),( unsigned char**) _check_null( _untag_arr( newargs,
sizeof( unsigned char*), 1u)),( unsigned char**) _check_null( _untag_arr(
newenvp, sizeof( unsigned char*), 1u)));}}} int Cyc_Std_link( struct _tagged_arr
path1, struct _tagged_arr path2){ return link( string_to_Cstring( path1),
string_to_Cstring( path2));} int Cyc_Std_read( int fd, struct _tagged_arr buf,
unsigned int count){ if( count >  _get_arr_size( buf, sizeof( unsigned char))){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp364=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp364[ 0]=({ struct Cyc_Core_Failure_struct
_temp365; _temp365.tag= Cyc_Core_Failure; _temp365.f1= _tag_arr("read: called with count > buf.size",
sizeof( unsigned char), 35u); _temp365;}); _temp364;}));} return read( fd,
underlying_Cstring(( struct _tagged_arr) buf), count);} int Cyc_Std_rmdir(
struct _tagged_arr path){ return rmdir( string_to_Cstring( path));} int Cyc_Std_symlink(
struct _tagged_arr path1, struct _tagged_arr path2){ return symlink(
string_to_Cstring( path1), string_to_Cstring( path2));} int Cyc_Std_truncate(
struct _tagged_arr path, int length){ return truncate( string_to_Cstring( path),
length);} int Cyc_Std_write( int fd, struct _tagged_arr buf, unsigned int count){
if( count >  _get_arr_size( buf, sizeof( unsigned char))){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp366=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp366[ 0]=({ struct Cyc_Core_Failure_struct
_temp367; _temp367.tag= Cyc_Core_Failure; _temp367.f1= _tag_arr("write: called with count > buf.size",
sizeof( unsigned char), 36u); _temp367;}); _temp366;}));} return write( fd,
underlying_Cstring( buf), count);} int Cyc_Std_unlink( struct _tagged_arr
pathname){ return unlink( string_to_Cstring( pathname));} int Cyc_Std_gethostname(
struct _tagged_arr buf, unsigned int count){ if( count >  _get_arr_size( buf,
sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp368=( struct Cyc_Core_Failure_struct*) _cycalloc( sizeof( struct Cyc_Core_Failure_struct));
_temp368[ 0]=({ struct Cyc_Core_Failure_struct _temp369; _temp369.tag= Cyc_Core_Failure;
_temp369.f1= _tag_arr("gethostname: called with count > buf.size", sizeof(
unsigned char), 42u); _temp369;}); _temp368;}));} return gethostname(
underlying_Cstring(( struct _tagged_arr) buf), count);} int Cyc_Std_chroot(
struct _tagged_arr pathname){ return chroot( string_to_Cstring( pathname));}
struct _tagged_arr Cyc_Std_getpass( struct _tagged_arr prompt){ return
wrap_Cstring_as_string( getpass( string_to_Cstring( prompt)), - 1);} struct Cyc_utimbuf{
int actime; int modtime; } ; extern int utime( unsigned char* filename, struct
Cyc_utimbuf* buf); int Cyc_Std_utime( struct _tagged_arr filename, struct Cyc_utimbuf*
buf){ return utime( string_to_Cstring( filename), buf);}
