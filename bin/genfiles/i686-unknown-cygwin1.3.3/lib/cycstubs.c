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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char*string_to_Cstring(
struct _tagged_arr);extern unsigned char*underlying_Cstring(struct _tagged_arr);
extern struct _tagged_arr Cstring_to_string(unsigned char*);extern struct _tagged_arr
wrap_Cstring_as_string(unsigned char*,unsigned int);extern struct _tagged_arr
ntCsl_to_ntsl(unsigned char**);struct Cyc_Std_dirent{int d_ino;int d_off;
unsigned short d_reclen;unsigned char d_type;unsigned char d_name[256];};void*Cyc_Std_opendir(
struct _tagged_arr name);extern void*opendir(unsigned char*name);void*Cyc_Std_opendir(
struct _tagged_arr name){return opendir(string_to_Cstring(name));}struct Cyc_Std_flock{
short l_type;int l_start;short l_whence;int l_len;int l_pid;};static const int Cyc_Std_Flock=
0;struct Cyc_Std_Flock_struct{int tag;struct Cyc_Std_flock*f1;};static const int Cyc_Std_Long=
1;struct Cyc_Std_Long_struct{int tag;int f1;};extern int Cyc_Std_fcntl(int fd,int cmd,
struct _tagged_arr);extern int Cyc_Std_open(struct _tagged_arr,int,struct _tagged_arr);
extern int Cyc_Std_creat(struct _tagged_arr,int);extern int fcntl(int fd,int cmd);
extern int fcntl_with_arg(int fd,int cmd,int arg);extern int fcntl_with_lock(int fd,int
cmd,struct Cyc_Std_flock*lock);extern int creat(unsigned char*,int);extern int
open_without_mode(unsigned char*,int);extern int open_with_mode(unsigned char*,int,
int);int Cyc_Std_fcntl(int fd,int cmd,struct _tagged_arr argv){if(_get_arr_size(argv,
sizeof(void*))== 0){return fcntl(fd,cmd);}else{if(_get_arr_size(argv,sizeof(void*))
!= 1){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp0=_cycalloc(sizeof(
struct Cyc_Core_Failure_struct));_tmp0[0]=({struct Cyc_Core_Failure_struct _tmp1;
_tmp1.tag=Cyc_Core_Failure;_tmp1.f1=_tag_arr("fcntl: too many args",sizeof(
unsigned char),21);_tmp1;});_tmp0;}));}else{void*_tmp2=*((void**)
_check_unknown_subscript(argv,sizeof(void*),0));int _tmp8;struct Cyc_Std_flock*
_tmpA;_LL4: if(*((int*)_tmp2)== Cyc_Std_Long){_LL9: _tmp8=((struct Cyc_Std_Long_struct*)
_tmp2)->f1;goto _LL5;}else{goto _LL6;}_LL6: if(*((int*)_tmp2)== Cyc_Std_Flock){
_LL11: _tmpA=((struct Cyc_Std_Flock_struct*)_tmp2)->f1;goto _LL7;}else{goto _LL3;}
_LL5: return fcntl_with_arg(fd,cmd,_tmp8);_LL7: return fcntl_with_lock(fd,cmd,(
struct Cyc_Std_flock*)_tmpA);_LL3:;}}}int Cyc_Std_creat(struct _tagged_arr s,int m){
return creat(string_to_Cstring(s),m);}int Cyc_Std_open(struct _tagged_arr s,int i,
struct _tagged_arr ms){if(_get_arr_size(ms,sizeof(int))>= 1){return open_with_mode(
string_to_Cstring(s),i,*((int*)_check_unknown_subscript(ms,sizeof(int),0)));}
else{return open_without_mode(string_to_Cstring(s),i);}}struct Cyc_Std_timeval{int
tv_sec;int tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};
struct Cyc_Std_itimerval{struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval
it_value;};typedef struct {unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t;struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;
struct Cyc_Std_sockaddr_in;struct Cyc_Std_sockaddr{unsigned short sa_family;
unsigned char sa_data[14];};static const int Cyc_Std_SA_sockaddr_in=0;struct Cyc_Std_SA_sockaddr_in_struct{
int tag;struct Cyc_Std_sockaddr_in*f1;};static const int Cyc_Std_SA_sockaddr=1;struct
Cyc_Std_SA_sockaddr_struct{int tag;struct Cyc_Std_sockaddr*f1;};static const int Cyc_Std_SA_socklenptr=
2;struct Cyc_Std_SA_socklenptr_struct{int tag;unsigned int*f1;};static const int Cyc_Std_SA_socklen=
3;struct Cyc_Std_SA_socklen_struct{int tag;unsigned int f1;};extern int Cyc_Std_accept(
int fd,struct _tagged_arr);extern int Cyc_Std_bind(int fd,struct _tagged_arr);extern
int Cyc_Std_connect(int fd,struct _tagged_arr);extern int Cyc_Std_getpeername(int fd,
struct _tagged_arr);extern int Cyc_Std_getsockname(int fd,struct _tagged_arr);extern
int Cyc_Std_send(int fd,struct _tagged_arr buf,unsigned int n,int flags);extern int Cyc_Std_recv(
int fd,struct _tagged_arr buf,unsigned int n,int flags);extern int Cyc_Std_sendto(int fd,
struct _tagged_arr buf,unsigned int n,int flags,struct _tagged_arr);extern int Cyc_Std_recvfrom(
int fd,struct _tagged_arr buf,unsigned int n,int flags,struct _tagged_arr);static const
int Cyc_Std_SO_int=0;struct Cyc_Std_SO_int_struct{int tag;int*f1;};static const int
Cyc_Std_SO_timeval=1;struct Cyc_Std_SO_timeval_struct{int tag;struct Cyc_Std_timeval*
f1;};static const int Cyc_Std_SO_socklenptr=2;struct Cyc_Std_SO_socklenptr_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_SO_socklen=3;struct Cyc_Std_SO_socklen_struct{
int tag;unsigned int f1;};extern int Cyc_Std_getsockopt(int fd,int level,int optname,
struct _tagged_arr);extern int Cyc_Std_setsockopt(int fd,int level,int optname,struct
_tagged_arr);struct Cyc_Std_in_addr{unsigned int s_addr;};struct Cyc_Std_sockaddr_in{
unsigned short sin_family;unsigned short sin_port;struct Cyc_Std_in_addr sin_addr;
unsigned char sin_zero[8];};extern unsigned int htonl(unsigned int);extern
unsigned short htons(unsigned short);extern unsigned int ntohl(unsigned int);extern
unsigned short ntohs(unsigned short);struct Cyc_Std_servent{struct _tagged_arr
s_name;struct _tagged_arr s_aliases;unsigned short s_port;struct _tagged_arr s_proto;
};struct Cyc_Std_hostent{struct _tagged_arr h_name;struct _tagged_arr h_aliases;int
h_addrtype;int h_length;struct _tagged_arr h_addr_list;};struct Cyc_Std_protoent{
struct _tagged_arr p_name;struct _tagged_arr p_aliases;int p_proto;};extern struct Cyc_Std_servent*
Cyc_Std_getservbyname(struct _tagged_arr name,struct _tagged_arr proto);extern struct
Cyc_Std_hostent*Cyc_Std_gethostbyname(struct _tagged_arr name);extern struct Cyc_Std_protoent*
Cyc_Std_getprotobyname(struct _tagged_arr name);extern void Cyc_Std_herror(struct
_tagged_arr);struct Cyc_Cnetdb_Cservent{unsigned char*s_name;unsigned char**
s_aliases;unsigned short s_port;unsigned char*s_proto;};struct Cyc_Cnetdb_Chostent{
unsigned char*h_name;unsigned char**h_aliases;short h_addrtype;short h_length;
struct Cyc_Std_in_addr**h_addr_list;};struct Cyc_Cnetdb_Cprotoent{unsigned char*
p_name;unsigned char**p_aliases;int p_proto;};extern struct Cyc_Cnetdb_Cservent*
getservbyname(unsigned char*name,unsigned char*proto);extern struct Cyc_Cnetdb_Chostent*
gethostbyname(unsigned char*name);extern struct Cyc_Cnetdb_Cprotoent*
getprotobyname(unsigned char*name);extern void herror(unsigned char*);extern struct
_tagged_arr pntlp_toCyc(struct Cyc_Std_in_addr**);struct Cyc_Std_servent*Cyc_Std_getservbyname(
struct _tagged_arr name,struct _tagged_arr proto){struct Cyc_Cnetdb_Cservent*src=
getservbyname(string_to_Cstring(name),string_to_Cstring(proto));return(
unsigned int)src?({struct Cyc_Std_servent*_tmpC=_cycalloc(sizeof(struct Cyc_Std_servent));
_tmpC->s_name=Cstring_to_string(src->s_name);_tmpC->s_aliases=ntCsl_to_ntsl(src->s_aliases);
_tmpC->s_port=src->s_port;_tmpC->s_proto=Cstring_to_string(src->s_proto);_tmpC;}):
0;}struct Cyc_Std_hostent*Cyc_Std_gethostbyname(struct _tagged_arr name){struct Cyc_Cnetdb_Chostent*
src=gethostbyname(string_to_Cstring(name));return(unsigned int)src?({struct Cyc_Std_hostent*
_tmpD=_cycalloc(sizeof(struct Cyc_Std_hostent));_tmpD->h_name=Cstring_to_string(
src->h_name);_tmpD->h_aliases=ntCsl_to_ntsl(src->h_aliases);_tmpD->h_addrtype=(
int)src->h_addrtype;_tmpD->h_length=(int)src->h_length;_tmpD->h_addr_list=
pntlp_toCyc(src->h_addr_list);_tmpD;}): 0;}struct Cyc_Std_protoent*Cyc_Std_getprotobyname(
struct _tagged_arr name){struct Cyc_Cnetdb_Cprotoent*src=getprotobyname(
string_to_Cstring(name));return(unsigned int)src?({struct Cyc_Std_protoent*_tmpE=
_cycalloc(sizeof(struct Cyc_Std_protoent));_tmpE->p_name=Cstring_to_string(src->p_name);
_tmpE->p_aliases=ntCsl_to_ntsl(src->p_aliases);_tmpE->p_proto=src->p_proto;_tmpE;}):
0;}void Cyc_Std_herror(struct _tagged_arr s){herror(string_to_Cstring(s));}
unsigned char Cyc_Std_sockaddr_in[16]="\000\000\000\000sockaddr_in";struct Cyc_Std_sockaddr_in_struct{
unsigned char*tag;struct Cyc_Std_sockaddr_in f1;};extern int Cyc_Std_inet_aton(
struct _tagged_arr cp,struct Cyc_Std_in_addr*inp);extern struct _tagged_arr Cyc_Std_inet_ntoa(
struct Cyc_Std_in_addr);extern unsigned int Cyc_Std_inet_addr(struct _tagged_arr addr);
extern int inet_aton(unsigned char*cp,struct Cyc_Std_in_addr*inp);extern
unsigned char*inet_ntoa(struct Cyc_Std_in_addr);extern unsigned int inet_addr(
unsigned char*);int Cyc_Std_inet_aton(struct _tagged_arr cp,struct Cyc_Std_in_addr*
inp){return inet_aton(string_to_Cstring(cp),inp);}struct _tagged_arr Cyc_Std_inet_ntoa(
struct Cyc_Std_in_addr x){return wrap_Cstring_as_string(inet_ntoa(x),- 1);}
unsigned int Cyc_Std_inet_addr(struct _tagged_arr addr){return inet_addr(
string_to_Cstring(addr));}void(*Cyc_Std_signal(int sig,void(*func)(int)))(int);
extern void Cyc_Std__SIG_DFL(int);extern void Cyc_Std__SIG_IGN(int);extern void Cyc_Std__SIG_ERR(
int);extern void(*signal_func(int sig,void(*func)(int)))(int);void Cyc_Std__SIG_DFL(
int n){;}void Cyc_Std__SIG_IGN(int n){;}void Cyc_Std__SIG_ERR(int n){;}void(*Cyc_Std_signal(
int sig,void(*func)(int)))(int){return signal_func(sig,func);}struct Cyc_Std_tm{int
tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int
tm_yday;int tm_isdst;};extern struct _tagged_arr Cyc_Std_asctime(const struct Cyc_Std_tm*
timeptr);extern struct _tagged_arr Cyc_Std_ctime(const int*timep);extern unsigned int
Cyc_Std_strftime(struct _tagged_arr s,unsigned int maxsize,struct _tagged_arr fmt,
const struct Cyc_Std_tm*t);extern struct _tagged_arr Cyc_Std_asctime_r(const struct Cyc_Std_tm*,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_ctime_r(const int*,struct
_tagged_arr);struct Cyc_Std_stat_t{short st_dev;unsigned int st_ino;int st_mode;
unsigned short st_nlink;unsigned short st_uid;unsigned short st_gid;short st_rdev;
int st_size;int st_atime;int st_spare1;int st_mtime;int st_spare2;int st_ctime;int
st_spare3;int st_blksize;int st_blocks;int st_spare4[2];};extern int Cyc_Std_stat(
struct _tagged_arr filename,struct Cyc_Std_stat_t*buf);extern int Cyc_Std_lstat(
struct _tagged_arr filename,struct Cyc_Std_stat_t*buf);extern int Cyc_Std_mkdir(
struct _tagged_arr pathname,int mode);extern int Cyc_Std_chmod(struct _tagged_arr path,
int mode);extern int stat(unsigned char*filename,struct Cyc_Std_stat_t*buf);extern
int fstat(int fd,struct Cyc_Std_stat_t*buf);extern int lstat(unsigned char*filename,
struct Cyc_Std_stat_t*buf);extern int mkdir(unsigned char*pathname,int mode);extern
int chmod(unsigned char*pathname,int mode);int Cyc_Std_stat(struct _tagged_arr
filename,struct Cyc_Std_stat_t*buf){return stat(string_to_Cstring(filename),buf);}
int Cyc_Std_lstat(struct _tagged_arr filename,struct Cyc_Std_stat_t*buf){return lstat(
string_to_Cstring(filename),buf);}int Cyc_Std_mkdir(struct _tagged_arr pathname,int
mode){return mkdir(string_to_Cstring(pathname),mode);}int Cyc_Std_chmod(struct
_tagged_arr pathname,int mode){return chmod(string_to_Cstring(pathname),mode);}
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_fromCfile(
struct Cyc_Cstdio___sFILE*cf);extern int Cyc_Std_remove(struct _tagged_arr);extern
int Cyc_Std_rename(struct _tagged_arr,struct _tagged_arr);extern int Cyc_Std_fclose(
struct Cyc_Std___sFILE*);extern int Cyc_Std_fflush(struct Cyc_Std___sFILE*);extern
struct Cyc_Std___sFILE*Cyc_Std_fopen(struct _tagged_arr __filename,struct
_tagged_arr __modes);extern struct Cyc_Std___sFILE*Cyc_Std_freopen(struct
_tagged_arr,struct _tagged_arr,struct Cyc_Std___sFILE*);extern void Cyc_Std_setbuf(
struct Cyc_Std___sFILE*__stream,struct _tagged_arr __buf);extern int Cyc_Std_setvbuf(
struct Cyc_Std___sFILE*__stream,struct _tagged_arr __buf,int __modes,unsigned int __n);
extern int Cyc_Std_fgetc(struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_getc(
struct Cyc_Std___sFILE*__stream);extern struct _tagged_arr Cyc_Std_fgets(struct
_tagged_arr __s,int __n,struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_fputc(int
__c,struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_putc(int __c,struct Cyc_Std___sFILE*
__stream);extern int Cyc_Std_fputs(struct _tagged_arr __s,struct Cyc_Std___sFILE*
__stream);extern int Cyc_Std_puts(struct _tagged_arr __s);extern int Cyc_Std_ungetc(
int __c,struct Cyc_Std___sFILE*__stream);extern unsigned int Cyc_Std_fread(struct
_tagged_arr __ptr,unsigned int __size,unsigned int __n,struct Cyc_Std___sFILE*
__stream);extern unsigned int Cyc_Std_fwrite(struct _tagged_arr __ptr,unsigned int
__size,unsigned int __n,struct Cyc_Std___sFILE*__s);extern int Cyc_Std_fseek(struct
Cyc_Std___sFILE*__stream,int __off,int __whence);extern int Cyc_Std_ftell(struct Cyc_Std___sFILE*
__stream);extern void Cyc_Std_rewind(struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_fgetpos(
struct Cyc_Std___sFILE*__stream,int*__pos);extern int Cyc_Std_fsetpos(struct Cyc_Std___sFILE*
__stream,int*__pos);extern void Cyc_Std_clearerr(struct Cyc_Std___sFILE*__stream);
extern int Cyc_Std_feof(struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_ferror(
struct Cyc_Std___sFILE*__stream);extern void Cyc_Std_perror(struct _tagged_arr __s);
extern struct Cyc_Std___sFILE*Cyc_Std_fdopen(int __fd,struct _tagged_arr __modes);
extern int Cyc_Std_fileno(struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_getw(
struct Cyc_Std___sFILE*__stream);extern int Cyc_Std_putw(int __w,struct Cyc_Std___sFILE*
__stream);extern void Cyc_Std_setbuffer(struct Cyc_Std___sFILE*__stream,struct
_tagged_arr __buf,unsigned int __size);extern void Cyc_Std_setlinebuf(struct Cyc_Std___sFILE*
__stream);extern struct Cyc_Std___sFILE*Cyc_Std_popen(struct _tagged_arr command,
struct _tagged_arr type);extern int Cyc_Std_pclose(struct Cyc_Std___sFILE*stream);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct Cyc_Std___sFILE*Cyc_Std_file_open(struct _tagged_arr fname,struct
_tagged_arr mode);extern void Cyc_Std_file_close(struct Cyc_Std___sFILE*);static
const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[8];
extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_strcpy(
struct _tagged_arr dest,struct _tagged_arr src);struct Cyc_Cstdio___sFILE;extern int
remove(unsigned char*);extern int rename(unsigned char*,unsigned char*);extern int
fclose(struct Cyc_Cstdio___sFILE*);extern int fflush(struct Cyc_Cstdio___sFILE*);
extern struct Cyc_Cstdio___sFILE*fopen(unsigned char*__filename,unsigned char*
__modes);extern struct Cyc_Cstdio___sFILE*freopen(unsigned char*__filename,
unsigned char*__modes,struct Cyc_Cstdio___sFILE*__stream);extern struct Cyc_Cstdio___sFILE*
fdopen(int __fd,unsigned char*__modes);extern int setvbuf(struct Cyc_Cstdio___sFILE*
__stream,unsigned char*__buf,int __modes,unsigned int __n);extern void setbuffer(
struct Cyc_Cstdio___sFILE*__stream,unsigned char*__buf,unsigned int __size);extern
void setlinebuf(struct Cyc_Cstdio___sFILE*__stream);extern int fgetc(struct Cyc_Cstdio___sFILE*
__stream);extern int getc(struct Cyc_Cstdio___sFILE*__stream);extern int fputc(int __c,
struct Cyc_Cstdio___sFILE*__stream);extern int putc(int __c,struct Cyc_Cstdio___sFILE*
__stream);extern int getw(struct Cyc_Cstdio___sFILE*__stream);extern int putw(int __w,
struct Cyc_Cstdio___sFILE*__stream);extern unsigned char*fgets(unsigned char*__s,
int __n,struct Cyc_Cstdio___sFILE*__stream);extern unsigned char*gets(unsigned char*
__s);extern int fputs(unsigned char*__s,struct Cyc_Cstdio___sFILE*__stream);extern
int puts(unsigned char*__s);extern int ungetc(int __c,struct Cyc_Cstdio___sFILE*
__stream);extern unsigned int fread(unsigned char*__ptr,unsigned int __size,
unsigned int __n,struct Cyc_Cstdio___sFILE*__stream);extern unsigned int fwrite(
unsigned char*__ptr,unsigned int __size,unsigned int __n,struct Cyc_Cstdio___sFILE*
__s);extern int fseek(struct Cyc_Cstdio___sFILE*__stream,int __off,int __whence);
extern int ftell(struct Cyc_Cstdio___sFILE*__stream);extern void rewind(struct Cyc_Cstdio___sFILE*
__stream);extern int fgetpos(struct Cyc_Cstdio___sFILE*__stream,int*__pos);extern
int fsetpos(struct Cyc_Cstdio___sFILE*__stream,int*__pos);extern void clearerr(
struct Cyc_Cstdio___sFILE*__stream);extern int feof(struct Cyc_Cstdio___sFILE*
__stream);extern int ferror(struct Cyc_Cstdio___sFILE*__stream);extern void perror(
unsigned char*__s);extern int fileno(struct Cyc_Cstdio___sFILE*__stream);extern
struct Cyc_Cstdio___sFILE*popen(unsigned char*,unsigned char*);extern int pclose(
struct Cyc_Cstdio___sFILE*__stream);struct Cyc_Std___sFILE{struct Cyc_Cstdio___sFILE*
file;};struct Cyc_Std___sFILE*Cyc_Std_fromCfile(struct Cyc_Cstdio___sFILE*cf){
return(unsigned int)cf?({struct Cyc_Std___sFILE*_tmpF=_cycalloc(sizeof(struct Cyc_Std___sFILE));
_tmpF->file=cf;_tmpF;}): 0;}int Cyc_Std_remove(struct _tagged_arr filename){return
remove(string_to_Cstring(filename));}int Cyc_Std_rename(struct _tagged_arr
old_filename,struct _tagged_arr new_filename){return rename(string_to_Cstring(
old_filename),string_to_Cstring(new_filename));}int Cyc_Std_fclose(struct Cyc_Std___sFILE*
f){int r=fclose((struct Cyc_Cstdio___sFILE*)_check_null(f->file));if(r == 0){f->file=
0;}return r;}int Cyc_Std_fflush(struct Cyc_Std___sFILE*f){return(unsigned int)f?
fflush(f->file): fflush(0);}struct Cyc_Std___sFILE*Cyc_Std_freopen(struct
_tagged_arr x,struct _tagged_arr y,struct Cyc_Std___sFILE*f){struct Cyc_Cstdio___sFILE*
cf=freopen(string_to_Cstring(x),string_to_Cstring(y),(struct Cyc_Cstdio___sFILE*)
_check_null(f->file));return(unsigned int)cf?({struct Cyc_Std___sFILE*_tmp10=
_cycalloc(sizeof(struct Cyc_Std___sFILE));_tmp10->file=cf;_tmp10;}): 0;}void Cyc_Std_setbuf(
struct Cyc_Std___sFILE*f,struct _tagged_arr buf){Cyc_Std_setvbuf(f,buf,(
unsigned int)buf.curr? 0: 2,1024);}void Cyc_Std_setbuffer(struct Cyc_Std___sFILE*f,
struct _tagged_arr buf,unsigned int size){Cyc_Std_setvbuf(f,buf,(unsigned int)buf.curr?
0: 2,size);}void Cyc_Std_setlinebuf(struct Cyc_Std___sFILE*f){Cyc_Std_setvbuf(f,
_tag_arr(0,0,0),1,0);}int Cyc_Std_setvbuf(struct Cyc_Std___sFILE*f,struct
_tagged_arr buf,int mode,unsigned int size){if(_get_arr_size(buf,sizeof(
unsigned char))< size){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp11=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp11[0]=({struct Cyc_Core_Failure_struct
_tmp12;_tmp12.tag=Cyc_Core_Failure;_tmp12.f1=_tag_arr("setvbuf: buffer insufficient",
sizeof(unsigned char),29);_tmp12;});_tmp11;}));}return setvbuf((struct Cyc_Cstdio___sFILE*)
_check_null(f->file),underlying_Cstring((struct _tagged_arr)buf),mode,size);}int
Cyc_Std_fgetc(struct Cyc_Std___sFILE*f){return fgetc((struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}struct _tagged_arr Cyc_Std_fgets(struct _tagged_arr s,int n,
struct Cyc_Std___sFILE*f){unsigned char*result;unsigned char*buffer=
underlying_Cstring((struct _tagged_arr)s);unsigned int len=_get_arr_size(s,sizeof(
unsigned char));n=(int)(len < n? len:(unsigned int)n);result=fgets(buffer,n,(
struct Cyc_Cstdio___sFILE*)_check_null(f->file));if(result == 0){return _tag_arr(0,
0,0);}else{return s;}}int Cyc_Std_fputc(int i,struct Cyc_Std___sFILE*f){return fputc(
i,(struct Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_fputs(struct
_tagged_arr s,struct Cyc_Std___sFILE*f){return fputs(string_to_Cstring(s),(struct
Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_getc(struct Cyc_Std___sFILE*
f){return getc((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_putc(
int i,struct Cyc_Std___sFILE*f){return putc(i,(struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}int Cyc_Std_puts(struct _tagged_arr s){return puts(
string_to_Cstring(s));}int Cyc_Std_ungetc(int i,struct Cyc_Std___sFILE*f){return
ungetc(i,(struct Cyc_Cstdio___sFILE*)_check_null(f->file));}unsigned int Cyc_Std_fread(
struct _tagged_arr ptr,unsigned int size,unsigned int nmemb,struct Cyc_Std___sFILE*f){
if(size * nmemb > _get_arr_size(ptr,sizeof(unsigned char))){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp13=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp13[0]=({struct Cyc_Core_Failure_struct _tmp14;_tmp14.tag=Cyc_Core_Failure;
_tmp14.f1=_tag_arr("fread: buffer insufficient",sizeof(unsigned char),27);_tmp14;});
_tmp13;}));}return fread(underlying_Cstring((struct _tagged_arr)ptr),size,nmemb,(
struct Cyc_Cstdio___sFILE*)_check_null(f->file));}unsigned int Cyc_Std_fwrite(
struct _tagged_arr ptr,unsigned int size,unsigned int nmemb,struct Cyc_Std___sFILE*f){
if(size * nmemb > _get_arr_size(ptr,sizeof(unsigned char))){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp15=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp15[0]=({struct Cyc_Core_Failure_struct _tmp16;_tmp16.tag=Cyc_Core_Failure;
_tmp16.f1=_tag_arr("fwrite: buffer insufficient",sizeof(unsigned char),28);
_tmp16;});_tmp15;}));}return fwrite(underlying_Cstring(ptr),size,nmemb,(struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}int Cyc_Std_fgetpos(struct Cyc_Std___sFILE*f,int*x){return
fgetpos((struct Cyc_Cstdio___sFILE*)_check_null(f->file),x);}int Cyc_Std_fseek(
struct Cyc_Std___sFILE*f,int offset,int whence){return fseek((struct Cyc_Cstdio___sFILE*)
_check_null(f->file),offset,whence);}int Cyc_Std_fsetpos(struct Cyc_Std___sFILE*f,
int*x){return fsetpos((struct Cyc_Cstdio___sFILE*)_check_null(f->file),x);}int Cyc_Std_ftell(
struct Cyc_Std___sFILE*f){return ftell((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}
void Cyc_Std_rewind(struct Cyc_Std___sFILE*f){rewind((struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}void Cyc_Std_clearerr(struct Cyc_Std___sFILE*f){clearerr((
struct Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_feof(struct Cyc_Std___sFILE*
f){return feof((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_ferror(
struct Cyc_Std___sFILE*f){return ferror((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}
void Cyc_Std_perror(struct _tagged_arr s){perror(string_to_Cstring(s));}struct Cyc_Std___sFILE*
Cyc_Std_fopen(struct _tagged_arr name,struct _tagged_arr type){struct Cyc_Cstdio___sFILE*
cf=fopen(string_to_Cstring(name),string_to_Cstring(type));return(unsigned int)cf?({
struct Cyc_Std___sFILE*_tmp17=_cycalloc(sizeof(struct Cyc_Std___sFILE));_tmp17->file=
cf;_tmp17;}): 0;}struct Cyc_Std___sFILE*Cyc_Std_fdopen(int i,struct _tagged_arr s){
struct Cyc_Cstdio___sFILE*cf=fdopen(i,string_to_Cstring(s));return(unsigned int)
cf?({struct Cyc_Std___sFILE*_tmp18=_cycalloc(sizeof(struct Cyc_Std___sFILE));
_tmp18->file=cf;_tmp18;}): 0;}int Cyc_Std_fileno(struct Cyc_Std___sFILE*f){return
fileno((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}int Cyc_Std_getw(struct
Cyc_Std___sFILE*f){return getw((struct Cyc_Cstdio___sFILE*)_check_null(f->file));}
int Cyc_Std_pclose(struct Cyc_Std___sFILE*f){return pclose((struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}struct Cyc_Std___sFILE*Cyc_Std_popen(struct _tagged_arr s,
struct _tagged_arr m){struct Cyc_Cstdio___sFILE*cf=popen(string_to_Cstring(s),
string_to_Cstring(m));return(unsigned int)cf?({struct Cyc_Std___sFILE*_tmp19=
_cycalloc(sizeof(struct Cyc_Std___sFILE));_tmp19->file=cf;_tmp19;}): 0;}int Cyc_Std_putw(
int i,struct Cyc_Std___sFILE*f){return putw(i,(struct Cyc_Cstdio___sFILE*)
_check_null(f->file));}unsigned char Cyc_Std_FileCloseError[19]="\000\000\000\000FileCloseError";
unsigned char Cyc_Std_FileOpenError[18]="\000\000\000\000FileOpenError";struct Cyc_Std___sFILE*
Cyc_Std_file_open(struct _tagged_arr fname,struct _tagged_arr mode){struct Cyc_Std___sFILE*
f=Cyc_Std_fopen(fname,mode);if(f == 0){struct _tagged_arr fn=({unsigned int _tmp1C=
_get_arr_size(fname,sizeof(unsigned char));unsigned char*_tmp1D=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp1C));struct _tagged_arr
_tmp1F=_tag_arr(_tmp1D,sizeof(unsigned char),_get_arr_size(fname,sizeof(
unsigned char)));{unsigned int _tmp1E=_tmp1C;unsigned int i;for(i=0;i < _tmp1E;i ++){
_tmp1D[i]=((const unsigned char*)fname.curr)[(int)i];}};_tmp1F;});(int)_throw((
void*)({struct Cyc_Std_FileOpenError_struct*_tmp1A=_cycalloc(sizeof(struct Cyc_Std_FileOpenError_struct));
_tmp1A[0]=({struct Cyc_Std_FileOpenError_struct _tmp1B;_tmp1B.tag=Cyc_Std_FileOpenError;
_tmp1B.f1=fn;_tmp1B;});_tmp1A;}));}return(struct Cyc_Std___sFILE*)_check_null(f);}
void Cyc_Std_file_close(struct Cyc_Std___sFILE*f){if(Cyc_Std_fclose(f)!= 0){(int)
_throw((void*)Cyc_Std_FileCloseError);}}extern unsigned char Cyc_Array_Array_mismatch[
19];struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};
extern int abs(int __x);extern int atexit(void(*__func)());extern struct Cyc_Std__Div
div(int __numer,int __denom);extern struct Cyc_Std__Ldiv ldiv(int __numer,int __denom);
extern int random();extern void srandom(unsigned int __seed);extern int rand();extern
void srand(unsigned int __seed);extern int rand_r(unsigned int*__seed);extern int
grantpt(int __fd);extern int unlockpt(int __fd);extern double Cyc_Std_atof(struct
_tagged_arr);extern int Cyc_Std_atoi(struct _tagged_arr);extern int Cyc_Std_atol(
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_getenv(struct _tagged_arr);
extern double Cyc_Std_strtod(struct _tagged_arr n,struct _tagged_arr*end);extern int
Cyc_Std_strtol(struct _tagged_arr n,struct _tagged_arr*end,int base);extern
unsigned int Cyc_Std_strtoul(struct _tagged_arr n,struct _tagged_arr*end,int base);
extern unsigned int Cyc_Std_mstrtoul(struct _tagged_arr n,struct _tagged_arr*endptr,
int base);extern void Cyc_Std_qsort(struct _tagged_arr tab,unsigned int nmemb,
unsigned int szmemb,int(*compar)(const void*,const void*));extern int Cyc_Std_system(
struct _tagged_arr);extern void Cyc_Std_free(struct _tagged_arr);extern double atof(
unsigned char*);extern int atoi(unsigned char*);extern int atol(unsigned char*);
extern unsigned char*getenv(unsigned char*);extern int putenv(unsigned char*);
extern double strtod(unsigned char*,unsigned char**);extern int strtol(unsigned char*,
unsigned char**,int);extern unsigned int strtoul(unsigned char*,unsigned char**,
int);extern void qsort(void*base,unsigned int nmemb,unsigned int size,int(*compar)(
const void*,const void*));extern int system(unsigned char*);double Cyc_Std_atof(
struct _tagged_arr _nptr){return atof(string_to_Cstring(_nptr));}int Cyc_Std_atoi(
struct _tagged_arr _nptr){return atoi(string_to_Cstring(_nptr));}int Cyc_Std_atol(
struct _tagged_arr _nptr){return atol(string_to_Cstring(_nptr));}struct _tagged_arr
Cyc_Std_getenv(struct _tagged_arr name){return Cstring_to_string(getenv(
string_to_Cstring(name)));}int Cyc_Std_putenv(struct _tagged_arr s){return putenv(
string_to_Cstring(s));}static void Cyc_Std_check_valid_cstring(struct _tagged_arr s){
if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp20=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmp20[0]=({struct Cyc_Core_Invalid_argument_struct _tmp21;_tmp21.tag=Cyc_Core_Invalid_argument;
_tmp21.f1=_tag_arr("strtox NULL pointer",sizeof(unsigned char),20);_tmp21;});
_tmp20;}));}{int found_zero=0;{int i=(int)(_get_arr_size(s,sizeof(unsigned char))- 
1);for(0;i >= 0;i --){if(((const unsigned char*)s.curr)[i]== '\000'){found_zero=1;
break;}}}if(! found_zero){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp22=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp22[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp23;_tmp23.tag=Cyc_Core_Invalid_argument;
_tmp23.f1=_tag_arr("strtox: not a C string",sizeof(unsigned char),23);_tmp23;});
_tmp22;}));}}}double Cyc_Std_strtod(struct _tagged_arr nptr,struct _tagged_arr*
endptr){Cyc_Std_check_valid_cstring(nptr);{unsigned char*c=underlying_Cstring(
nptr);unsigned char*e=endptr == 0? 0: c;double d=strtod(c,(unsigned char**)& e);if(
endptr != 0){int n=(int)((unsigned int)e - (unsigned int)c);*endptr=
_tagged_arr_plus(nptr,sizeof(unsigned char),n);}return d;}}int Cyc_Std_strtol(
struct _tagged_arr n,struct _tagged_arr*endptr,int base){Cyc_Std_check_valid_cstring(
n);{unsigned char*c=underlying_Cstring(n);unsigned char*e=endptr == 0? 0: c;int r=
strtol(c,(unsigned char**)& e,base);if(endptr != 0){int m=(int)((unsigned int)e - (
unsigned int)c);*endptr=_tagged_arr_plus(n,sizeof(unsigned char),m);}return r;}}
unsigned int Cyc_Std_strtoul(struct _tagged_arr n,struct _tagged_arr*endptr,int base){
Cyc_Std_check_valid_cstring(n);{unsigned char*c=underlying_Cstring(n);
unsigned char*e=endptr == 0? 0: c;unsigned int r=strtoul(c,(unsigned char**)& e,base);
if(endptr != 0){int m=(int)((unsigned int)e - (unsigned int)c);*endptr=
_tagged_arr_plus(n,sizeof(unsigned char),m);}return r;}}unsigned int Cyc_Std_mstrtoul(
struct _tagged_arr n,struct _tagged_arr*endptr,int base){Cyc_Std_check_valid_cstring((
struct _tagged_arr)n);{unsigned char*c=underlying_Cstring((struct _tagged_arr)n);
unsigned char*e=endptr == 0? 0: c;unsigned int r=strtoul(c,(unsigned char**)& e,base);
if(endptr != 0){int m=(int)((unsigned int)e - (unsigned int)c);*endptr=
_tagged_arr_plus(n,sizeof(unsigned char),m);}return r;}}void Cyc_Std_qsort(struct
_tagged_arr tab,unsigned int nmemb,unsigned int szmemb,int(*compar)(const void*,
const void*)){if(tab.curr == (_tag_arr(0,0,0)).curr? 1: _get_arr_size(tab,sizeof(
void))< nmemb){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp24=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp24[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp25;_tmp25.tag=Cyc_Core_Invalid_argument;
_tmp25.f1=_tag_arr("Std::qsort",sizeof(unsigned char),11);_tmp25;});_tmp24;}));}
qsort((void*)_check_null(_untag_arr(tab,sizeof(void),1)),nmemb,(unsigned int)
szmemb,(int(*)(const void*,const void*))compar);}int Cyc_Std_system(struct
_tagged_arr cmd){return system(string_to_Cstring(cmd));}void Cyc_Std_free(struct
_tagged_arr ptr){;}extern int accept(int,const struct Cyc_Std_sockaddr*,unsigned int*);
extern int accept_in(int,const struct Cyc_Std_sockaddr_in*,unsigned int*);extern int
bind(int,const struct Cyc_Std_sockaddr*,unsigned int);extern int bind_in(int,const
struct Cyc_Std_sockaddr_in*,unsigned int);extern int connect(int,const struct Cyc_Std_sockaddr*,
unsigned int);extern int connect_in(int,const struct Cyc_Std_sockaddr_in*,
unsigned int);extern int getpeername(int,const struct Cyc_Std_sockaddr*,unsigned int*);
extern int getpeername_in(int,const struct Cyc_Std_sockaddr_in*,unsigned int*);
extern int getsockname(int,const struct Cyc_Std_sockaddr*,unsigned int*);extern int
getsockname_in(int,const struct Cyc_Std_sockaddr_in*,unsigned int*);extern int
recvfrom(int,struct _tagged_arr,unsigned int,int,const struct Cyc_Std_sockaddr*,
unsigned int*);extern int recvfrom_in(int,struct _tagged_arr,unsigned int,int,const
struct Cyc_Std_sockaddr_in*,unsigned int*);extern int sendto(int,struct _tagged_arr,
unsigned int,int,const struct Cyc_Std_sockaddr*,unsigned int);extern int sendto_in(
int,struct _tagged_arr,unsigned int,int,const struct Cyc_Std_sockaddr_in*,
unsigned int);extern int send_wrapped(int,struct _tagged_arr,unsigned int,int);
extern int recv_wrapped(int,struct _tagged_arr,unsigned int,int);extern int
getsockopt_int(int,int,int,int*,unsigned int*);extern int setsockopt_int(int,int,
int,const int*,unsigned int);extern int getsockopt_timeval(int,int,int,struct Cyc_Std_timeval*,
unsigned int*);extern int setsockopt_timeval(int,int,int,const struct Cyc_Std_timeval*,
unsigned int);unsigned char Cyc_Std_SocketError[16]="\000\000\000\000SocketError";
struct Cyc_Std_SocketError_struct{unsigned char*tag;struct _tagged_arr f1;};static
struct _tagged_arr Cyc_Std_sopts2string(struct _tagged_arr args){struct _tagged_arr
res=_tag_arr(({unsigned char*_tmp30=_cycalloc_atomic(sizeof(unsigned char)* 1);
_tmp30[0]='\000';_tmp30;}),sizeof(unsigned char),1);{int i=0;for(0;i < 
_get_arr_size(args,sizeof(void*));i ++){void*_tmp26=((void**)args.curr)[i];_LL40:
if(*((int*)_tmp26)== Cyc_Std_SO_int){goto _LL41;}else{goto _LL42;}_LL42: if(*((int*)
_tmp26)== Cyc_Std_SO_timeval){goto _LL43;}else{goto _LL44;}_LL44: if(*((int*)_tmp26)
== Cyc_Std_SO_socklenptr){goto _LL45;}else{goto _LL46;}_LL46: if(*((int*)_tmp26)== 
Cyc_Std_SO_socklen){goto _LL47;}else{goto _LL39;}_LL41: res=Cyc_Std_strconcat((
struct _tagged_arr)res,_tag_arr("|SO_int",sizeof(unsigned char),8));goto _LL39;
_LL43: res=Cyc_Std_strconcat((struct _tagged_arr)res,_tag_arr("|SO_timeval",
sizeof(unsigned char),12));goto _LL39;_LL45: res=Cyc_Std_strconcat((struct
_tagged_arr)res,_tag_arr("|SO_socklenptr",sizeof(unsigned char),15));goto _LL39;
_LL47: res=Cyc_Std_strconcat((struct _tagged_arr)res,_tag_arr("|SO_socklen",
sizeof(unsigned char),12));goto _LL39;_LL39:;}}return res;}struct _tuple0{void*f1;
void*f2;};int Cyc_Std_accept(int fd,struct _tagged_arr ap){if(_get_arr_size(ap,
sizeof(void*))!= 2){(int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp31=
_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));_tmp31[0]=({struct Cyc_Std_SocketError_struct
_tmp32;_tmp32.tag=Cyc_Std_SocketError;_tmp32.f1=_tag_arr("accept---need 2 args",
sizeof(unsigned char),21);_tmp32;});_tmp31;}));}{struct _tuple0 _tmp34=({struct
_tuple0 _tmp33;_tmp33.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
_tmp33.f2=*((void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmp33;});void*
_tmp3C;unsigned int*_tmp3E;void*_tmp40;struct Cyc_Std_sockaddr_in*_tmp42;void*
_tmp44;unsigned int*_tmp46;void*_tmp48;struct Cyc_Std_sockaddr*_tmp4A;_LL54: _LL65:
_tmp40=_tmp34.f1;if(*((int*)_tmp40)== Cyc_Std_SA_sockaddr_in){_LL67: _tmp42=((
struct Cyc_Std_SA_sockaddr_in_struct*)_tmp40)->f1;goto _LL61;}else{goto _LL56;}
_LL61: _tmp3C=_tmp34.f2;if(*((int*)_tmp3C)== Cyc_Std_SA_socklenptr){_LL63: _tmp3E=((
struct Cyc_Std_SA_socklenptr_struct*)_tmp3C)->f1;goto _LL55;}else{goto _LL56;}_LL56:
_LL73: _tmp48=_tmp34.f1;if(*((int*)_tmp48)== Cyc_Std_SA_sockaddr){_LL75: _tmp4A=((
struct Cyc_Std_SA_sockaddr_struct*)_tmp48)->f1;goto _LL69;}else{goto _LL58;}_LL69:
_tmp44=_tmp34.f2;if(*((int*)_tmp44)== Cyc_Std_SA_socklenptr){_LL71: _tmp46=((
struct Cyc_Std_SA_socklenptr_struct*)_tmp44)->f1;goto _LL57;}else{goto _LL58;}_LL58:
goto _LL59;_LL55: return accept_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmp42,
_tmp3E);_LL57: return accept(fd,(const struct Cyc_Std_sockaddr*)_tmp4A,_tmp46);_LL59:(
int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp4C=_cycalloc(sizeof(
struct Cyc_Std_SocketError_struct));_tmp4C[0]=({struct Cyc_Std_SocketError_struct
_tmp4D;_tmp4D.tag=Cyc_Std_SocketError;_tmp4D.f1=_tag_arr("accept---bad args",
sizeof(unsigned char),18);_tmp4D;});_tmp4C;}));_LL53:;}}int Cyc_Std_bind(int fd,
struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)_throw((void*)({
struct Cyc_Std_SocketError_struct*_tmp4E=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmp4E[0]=({struct Cyc_Std_SocketError_struct _tmp4F;_tmp4F.tag=Cyc_Std_SocketError;
_tmp4F.f1=_tag_arr("bind---need 2 args",sizeof(unsigned char),19);_tmp4F;});
_tmp4E;}));}{struct _tuple0 _tmp51=({struct _tuple0 _tmp50;_tmp50.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp50.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp50;});void*_tmp59;unsigned int
_tmp5B;void*_tmp5D;struct Cyc_Std_sockaddr_in*_tmp5F;void*_tmp61;unsigned int
_tmp63;void*_tmp65;struct Cyc_Std_sockaddr*_tmp67;_LL83: _LL94: _tmp5D=_tmp51.f1;
if(*((int*)_tmp5D)== Cyc_Std_SA_sockaddr_in){_LL96: _tmp5F=((struct Cyc_Std_SA_sockaddr_in_struct*)
_tmp5D)->f1;goto _LL90;}else{goto _LL85;}_LL90: _tmp59=_tmp51.f2;if(*((int*)_tmp59)
== Cyc_Std_SA_socklen){_LL92: _tmp5B=((struct Cyc_Std_SA_socklen_struct*)_tmp59)->f1;
goto _LL84;}else{goto _LL85;}_LL85: _LL102: _tmp65=_tmp51.f1;if(*((int*)_tmp65)== 
Cyc_Std_SA_sockaddr){_LL104: _tmp67=((struct Cyc_Std_SA_sockaddr_struct*)_tmp65)->f1;
goto _LL98;}else{goto _LL87;}_LL98: _tmp61=_tmp51.f2;if(*((int*)_tmp61)== Cyc_Std_SA_socklen){
_LL100: _tmp63=((struct Cyc_Std_SA_socklen_struct*)_tmp61)->f1;goto _LL86;}else{
goto _LL87;}_LL87: goto _LL88;_LL84: return bind_in(fd,(const struct Cyc_Std_sockaddr_in*)
_tmp5F,_tmp5B);_LL86: return bind(fd,(const struct Cyc_Std_sockaddr*)_tmp67,_tmp63);
_LL88:(int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp69=_cycalloc(
sizeof(struct Cyc_Std_SocketError_struct));_tmp69[0]=({struct Cyc_Std_SocketError_struct
_tmp6A;_tmp6A.tag=Cyc_Std_SocketError;_tmp6A.f1=_tag_arr("bind---bad args",
sizeof(unsigned char),16);_tmp6A;});_tmp69;}));_LL82:;}}int Cyc_Std_connect(int fd,
struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)_throw((void*)({
struct Cyc_Std_SocketError_struct*_tmp6B=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmp6B[0]=({struct Cyc_Std_SocketError_struct _tmp6C;_tmp6C.tag=Cyc_Std_SocketError;
_tmp6C.f1=_tag_arr("connect---need 2 args",sizeof(unsigned char),22);_tmp6C;});
_tmp6B;}));}{struct _tuple0 _tmp6E=({struct _tuple0 _tmp6D;_tmp6D.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp6D.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp6D;});void*_tmp76;unsigned int
_tmp78;void*_tmp7A;struct Cyc_Std_sockaddr_in*_tmp7C;void*_tmp7E;unsigned int
_tmp80;void*_tmp82;struct Cyc_Std_sockaddr*_tmp84;_LL112: _LL123: _tmp7A=_tmp6E.f1;
if(*((int*)_tmp7A)== Cyc_Std_SA_sockaddr_in){_LL125: _tmp7C=((struct Cyc_Std_SA_sockaddr_in_struct*)
_tmp7A)->f1;goto _LL119;}else{goto _LL114;}_LL119: _tmp76=_tmp6E.f2;if(*((int*)
_tmp76)== Cyc_Std_SA_socklen){_LL121: _tmp78=((struct Cyc_Std_SA_socklen_struct*)
_tmp76)->f1;goto _LL113;}else{goto _LL114;}_LL114: _LL131: _tmp82=_tmp6E.f1;if(*((
int*)_tmp82)== Cyc_Std_SA_sockaddr){_LL133: _tmp84=((struct Cyc_Std_SA_sockaddr_struct*)
_tmp82)->f1;goto _LL127;}else{goto _LL116;}_LL127: _tmp7E=_tmp6E.f2;if(*((int*)
_tmp7E)== Cyc_Std_SA_socklen){_LL129: _tmp80=((struct Cyc_Std_SA_socklen_struct*)
_tmp7E)->f1;goto _LL115;}else{goto _LL116;}_LL116: goto _LL117;_LL113: return
connect_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmp7C,_tmp78);_LL115: return
connect(fd,(const struct Cyc_Std_sockaddr*)_tmp84,_tmp80);_LL117:(int)_throw((void*)({
struct Cyc_Std_SocketError_struct*_tmp86=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmp86[0]=({struct Cyc_Std_SocketError_struct _tmp87;_tmp87.tag=Cyc_Std_SocketError;
_tmp87.f1=_tag_arr("connect---bad args",sizeof(unsigned char),19);_tmp87;});
_tmp86;}));_LL111:;}}int Cyc_Std_getpeername(int fd,struct _tagged_arr ap){if(
_get_arr_size(ap,sizeof(void*))!= 2){(int)_throw((void*)({struct Cyc_Std_SocketError_struct*
_tmp88=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));_tmp88[0]=({struct Cyc_Std_SocketError_struct
_tmp89;_tmp89.tag=Cyc_Std_SocketError;_tmp89.f1=_tag_arr("getpeername---need 2 args",
sizeof(unsigned char),26);_tmp89;});_tmp88;}));}{struct _tuple0 _tmp8B=({struct
_tuple0 _tmp8A;_tmp8A.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
_tmp8A.f2=*((void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmp8A;});void*
_tmp93;unsigned int*_tmp95;void*_tmp97;struct Cyc_Std_sockaddr_in*_tmp99;void*
_tmp9B;unsigned int*_tmp9D;void*_tmp9F;struct Cyc_Std_sockaddr*_tmpA1;_LL141:
_LL152: _tmp97=_tmp8B.f1;if(*((int*)_tmp97)== Cyc_Std_SA_sockaddr_in){_LL154:
_tmp99=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp97)->f1;goto _LL148;}else{goto
_LL143;}_LL148: _tmp93=_tmp8B.f2;if(*((int*)_tmp93)== Cyc_Std_SA_socklenptr){
_LL150: _tmp95=((struct Cyc_Std_SA_socklenptr_struct*)_tmp93)->f1;goto _LL142;}
else{goto _LL143;}_LL143: _LL160: _tmp9F=_tmp8B.f1;if(*((int*)_tmp9F)== Cyc_Std_SA_sockaddr){
_LL162: _tmpA1=((struct Cyc_Std_SA_sockaddr_struct*)_tmp9F)->f1;goto _LL156;}else{
goto _LL145;}_LL156: _tmp9B=_tmp8B.f2;if(*((int*)_tmp9B)== Cyc_Std_SA_socklenptr){
_LL158: _tmp9D=((struct Cyc_Std_SA_socklenptr_struct*)_tmp9B)->f1;goto _LL144;}
else{goto _LL145;}_LL145: goto _LL146;_LL142: return getpeername_in(fd,(const struct
Cyc_Std_sockaddr_in*)_tmp99,_tmp95);_LL144: return getpeername(fd,(const struct Cyc_Std_sockaddr*)
_tmpA1,_tmp9D);_LL146:(int)_throw((void*)({struct Cyc_Std_SocketError_struct*
_tmpA3=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));_tmpA3[0]=({struct Cyc_Std_SocketError_struct
_tmpA4;_tmpA4.tag=Cyc_Std_SocketError;_tmpA4.f1=_tag_arr("getpeername---bad args",
sizeof(unsigned char),23);_tmpA4;});_tmpA3;}));_LL140:;}}int Cyc_Std_getsockname(
int fd,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)_throw((
void*)({struct Cyc_Std_SocketError_struct*_tmpA5=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmpA5[0]=({struct Cyc_Std_SocketError_struct _tmpA6;_tmpA6.tag=Cyc_Std_SocketError;
_tmpA6.f1=_tag_arr("getsockname---need 2 args",sizeof(unsigned char),26);_tmpA6;});
_tmpA5;}));}{struct _tuple0 _tmpA8=({struct _tuple0 _tmpA7;_tmpA7.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmpA7.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmpA7;});void*_tmpB0;unsigned int*
_tmpB2;void*_tmpB4;struct Cyc_Std_sockaddr_in*_tmpB6;void*_tmpB8;unsigned int*
_tmpBA;void*_tmpBC;struct Cyc_Std_sockaddr*_tmpBE;_LL170: _LL181: _tmpB4=_tmpA8.f1;
if(*((int*)_tmpB4)== Cyc_Std_SA_sockaddr_in){_LL183: _tmpB6=((struct Cyc_Std_SA_sockaddr_in_struct*)
_tmpB4)->f1;goto _LL177;}else{goto _LL172;}_LL177: _tmpB0=_tmpA8.f2;if(*((int*)
_tmpB0)== Cyc_Std_SA_socklenptr){_LL179: _tmpB2=((struct Cyc_Std_SA_socklenptr_struct*)
_tmpB0)->f1;goto _LL171;}else{goto _LL172;}_LL172: _LL189: _tmpBC=_tmpA8.f1;if(*((
int*)_tmpBC)== Cyc_Std_SA_sockaddr){_LL191: _tmpBE=((struct Cyc_Std_SA_sockaddr_struct*)
_tmpBC)->f1;goto _LL185;}else{goto _LL174;}_LL185: _tmpB8=_tmpA8.f2;if(*((int*)
_tmpB8)== Cyc_Std_SA_socklenptr){_LL187: _tmpBA=((struct Cyc_Std_SA_socklenptr_struct*)
_tmpB8)->f1;goto _LL173;}else{goto _LL174;}_LL174: goto _LL175;_LL171: return
getsockname_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmpB6,_tmpB2);_LL173: return
getsockname(fd,(const struct Cyc_Std_sockaddr*)_tmpBE,_tmpBA);_LL175:(int)_throw((
void*)({struct Cyc_Std_SocketError_struct*_tmpC0=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmpC0[0]=({struct Cyc_Std_SocketError_struct _tmpC1;_tmpC1.tag=Cyc_Std_SocketError;
_tmpC1.f1=_tag_arr("getsockname---bad args",sizeof(unsigned char),23);_tmpC1;});
_tmpC0;}));_LL169:;}}int Cyc_Std_recvfrom(int fd,struct _tagged_arr buf,unsigned int
n,int flags,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)
_throw((void*)({struct Cyc_Std_SocketError_struct*_tmpC2=_cycalloc(sizeof(struct
Cyc_Std_SocketError_struct));_tmpC2[0]=({struct Cyc_Std_SocketError_struct _tmpC3;
_tmpC3.tag=Cyc_Std_SocketError;_tmpC3.f1=_tag_arr("recvfrom---need 2 args",
sizeof(unsigned char),23);_tmpC3;});_tmpC2;}));}{struct _tuple0 _tmpC5=({struct
_tuple0 _tmpC4;_tmpC4.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
_tmpC4.f2=*((void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmpC4;});void*
_tmpCD;unsigned int*_tmpCF;void*_tmpD1;struct Cyc_Std_sockaddr_in*_tmpD3;void*
_tmpD5;unsigned int*_tmpD7;void*_tmpD9;struct Cyc_Std_sockaddr*_tmpDB;_LL199:
_LL210: _tmpD1=_tmpC5.f1;if(*((int*)_tmpD1)== Cyc_Std_SA_sockaddr_in){_LL212:
_tmpD3=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmpD1)->f1;goto _LL206;}else{goto
_LL201;}_LL206: _tmpCD=_tmpC5.f2;if(*((int*)_tmpCD)== Cyc_Std_SA_socklenptr){
_LL208: _tmpCF=((struct Cyc_Std_SA_socklenptr_struct*)_tmpCD)->f1;goto _LL200;}
else{goto _LL201;}_LL201: _LL218: _tmpD9=_tmpC5.f1;if(*((int*)_tmpD9)== Cyc_Std_SA_sockaddr){
_LL220: _tmpDB=((struct Cyc_Std_SA_sockaddr_struct*)_tmpD9)->f1;goto _LL214;}else{
goto _LL203;}_LL214: _tmpD5=_tmpC5.f2;if(*((int*)_tmpD5)== Cyc_Std_SA_socklenptr){
_LL216: _tmpD7=((struct Cyc_Std_SA_socklenptr_struct*)_tmpD5)->f1;goto _LL202;}
else{goto _LL203;}_LL203: goto _LL204;_LL200: return recvfrom_in(fd,buf,n,flags,(
const struct Cyc_Std_sockaddr_in*)_tmpD3,_tmpCF);_LL202: return recvfrom(fd,buf,n,
flags,(const struct Cyc_Std_sockaddr*)_tmpDB,_tmpD7);_LL204:(int)_throw((void*)({
struct Cyc_Std_SocketError_struct*_tmpDD=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmpDD[0]=({struct Cyc_Std_SocketError_struct _tmpDE;_tmpDE.tag=Cyc_Std_SocketError;
_tmpDE.f1=_tag_arr("recvfrom---bad args",sizeof(unsigned char),20);_tmpDE;});
_tmpDD;}));_LL198:;}}int Cyc_Std_sendto(int fd,struct _tagged_arr buf,unsigned int n,
int flags,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)
_throw((void*)({struct Cyc_Std_SocketError_struct*_tmpDF=_cycalloc(sizeof(struct
Cyc_Std_SocketError_struct));_tmpDF[0]=({struct Cyc_Std_SocketError_struct _tmpE0;
_tmpE0.tag=Cyc_Std_SocketError;_tmpE0.f1=_tag_arr("sendto---need 2 args",sizeof(
unsigned char),21);_tmpE0;});_tmpDF;}));}{struct _tuple0 _tmpE2=({struct _tuple0
_tmpE1;_tmpE1.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));_tmpE1.f2=*((
void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmpE1;});void*_tmpEA;
unsigned int _tmpEC;void*_tmpEE;struct Cyc_Std_sockaddr_in*_tmpF0;void*_tmpF2;
unsigned int _tmpF4;void*_tmpF6;struct Cyc_Std_sockaddr*_tmpF8;_LL228: _LL239:
_tmpEE=_tmpE2.f1;if(*((int*)_tmpEE)== Cyc_Std_SA_sockaddr_in){_LL241: _tmpF0=((
struct Cyc_Std_SA_sockaddr_in_struct*)_tmpEE)->f1;goto _LL235;}else{goto _LL230;}
_LL235: _tmpEA=_tmpE2.f2;if(*((int*)_tmpEA)== Cyc_Std_SA_socklen){_LL237: _tmpEC=((
struct Cyc_Std_SA_socklen_struct*)_tmpEA)->f1;goto _LL229;}else{goto _LL230;}_LL230:
_LL247: _tmpF6=_tmpE2.f1;if(*((int*)_tmpF6)== Cyc_Std_SA_sockaddr){_LL249: _tmpF8=((
struct Cyc_Std_SA_sockaddr_struct*)_tmpF6)->f1;goto _LL243;}else{goto _LL232;}
_LL243: _tmpF2=_tmpE2.f2;if(*((int*)_tmpF2)== Cyc_Std_SA_socklen){_LL245: _tmpF4=((
struct Cyc_Std_SA_socklen_struct*)_tmpF2)->f1;goto _LL231;}else{goto _LL232;}_LL232:
goto _LL233;_LL229: return sendto_in(fd,buf,n,flags,(const struct Cyc_Std_sockaddr_in*)
_tmpF0,_tmpEC);_LL231: return sendto(fd,buf,n,flags,(const struct Cyc_Std_sockaddr*)
_tmpF8,_tmpF4);_LL233:(int)_throw((void*)({struct Cyc_Std_SocketError_struct*
_tmpFA=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));_tmpFA[0]=({struct Cyc_Std_SocketError_struct
_tmpFB;_tmpFB.tag=Cyc_Std_SocketError;_tmpFB.f1=_tag_arr("sendto---bad args",
sizeof(unsigned char),18);_tmpFB;});_tmpFA;}));_LL227:;}}int Cyc_Std_send(int fd,
struct _tagged_arr buf,unsigned int n,int flags){return send_wrapped(fd,buf,n,flags);}
int Cyc_Std_recv(int fd,struct _tagged_arr buf,unsigned int n,int flags){return
recv_wrapped(fd,buf,n,flags);}int Cyc_Std_getsockopt(int fd,int level,int optname,
struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2){(int)_throw((void*)({
struct Cyc_Std_SocketError_struct*_tmpFC=_cycalloc(sizeof(struct Cyc_Std_SocketError_struct));
_tmpFC[0]=({struct Cyc_Std_SocketError_struct _tmpFD;_tmpFD.tag=Cyc_Std_SocketError;
_tmpFD.f1=_tag_arr("getsockopt---need 2 args",sizeof(unsigned char),25);_tmpFD;});
_tmpFC;}));}{struct _tuple0 _tmpFF=({struct _tuple0 _tmpFE;_tmpFE.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmpFE.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmpFE;});void*_tmp10B;
unsigned int*_tmp10D;void*_tmp10F;int*_tmp111;void*_tmp113;int*_tmp115;void*
_tmp117;int*_tmp119;void*_tmp11B;unsigned int*_tmp11D;void*_tmp11F;struct Cyc_Std_timeval*
_tmp121;void*_tmp123;int*_tmp125;void*_tmp127;struct Cyc_Std_timeval*_tmp129;
_LL257: _LL272: _tmp10F=_tmpFF.f1;if(*((int*)_tmp10F)== Cyc_Std_SO_int){_LL274:
_tmp111=((struct Cyc_Std_SO_int_struct*)_tmp10F)->f1;goto _LL268;}else{goto _LL259;}
_LL268: _tmp10B=_tmpFF.f2;if(*((int*)_tmp10B)== Cyc_Std_SO_socklenptr){_LL270:
_tmp10D=((struct Cyc_Std_SO_socklenptr_struct*)_tmp10B)->f1;goto _LL258;}else{goto
_LL259;}_LL259: _LL280: _tmp117=_tmpFF.f1;if(*((int*)_tmp117)== Cyc_Std_SO_int){
_LL282: _tmp119=((struct Cyc_Std_SO_int_struct*)_tmp117)->f1;goto _LL276;}else{goto
_LL261;}_LL276: _tmp113=_tmpFF.f2;if(*((int*)_tmp113)== Cyc_Std_SO_int){_LL278:
_tmp115=((struct Cyc_Std_SO_int_struct*)_tmp113)->f1;goto _LL260;}else{goto _LL261;}
_LL261: _LL288: _tmp11F=_tmpFF.f1;if(*((int*)_tmp11F)== Cyc_Std_SO_timeval){_LL290:
_tmp121=((struct Cyc_Std_SO_timeval_struct*)_tmp11F)->f1;goto _LL284;}else{goto
_LL263;}_LL284: _tmp11B=_tmpFF.f2;if(*((int*)_tmp11B)== Cyc_Std_SO_socklenptr){
_LL286: _tmp11D=((struct Cyc_Std_SO_socklenptr_struct*)_tmp11B)->f1;goto _LL262;}
else{goto _LL263;}_LL263: _LL296: _tmp127=_tmpFF.f1;if(*((int*)_tmp127)== Cyc_Std_SO_timeval){
_LL298: _tmp129=((struct Cyc_Std_SO_timeval_struct*)_tmp127)->f1;goto _LL292;}else{
goto _LL265;}_LL292: _tmp123=_tmpFF.f2;if(*((int*)_tmp123)== Cyc_Std_SO_int){
_LL294: _tmp125=((struct Cyc_Std_SO_int_struct*)_tmp123)->f1;goto _LL264;}else{goto
_LL265;}_LL265: goto _LL266;_LL258: return getsockopt_int(fd,level,optname,_tmp111,
_tmp10D);_LL260: return getsockopt_int(fd,level,optname,_tmp119,(unsigned int*)
_tmp115);_LL262: return getsockopt_timeval(fd,level,optname,_tmp121,_tmp11D);
_LL264: return getsockopt_timeval(fd,level,optname,_tmp129,(unsigned int*)_tmp125);
_LL266:(int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp12B=_cycalloc(
sizeof(struct Cyc_Std_SocketError_struct));_tmp12B[0]=({struct Cyc_Std_SocketError_struct
_tmp12C;_tmp12C.tag=Cyc_Std_SocketError;_tmp12C.f1=(struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmp12E;_tmp12E.tag=Cyc_Std_String_pa;_tmp12E.f1=(struct
_tagged_arr)Cyc_Std_sopts2string(ap);{void*_tmp12D[1]={& _tmp12E};Cyc_Std_aprintf(
_tag_arr("getsockopt---bad args %s",sizeof(unsigned char),25),_tag_arr(_tmp12D,
sizeof(void*),1));}});_tmp12C;});_tmp12B;}));_LL256:;}}int Cyc_Std_setsockopt(int
fd,int level,int optname,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 
2){(int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp12F=_cycalloc(
sizeof(struct Cyc_Std_SocketError_struct));_tmp12F[0]=({struct Cyc_Std_SocketError_struct
_tmp130;_tmp130.tag=Cyc_Std_SocketError;_tmp130.f1=_tag_arr("setsockopt---need 2 args",
sizeof(unsigned char),25);_tmp130;});_tmp12F;}));}{struct _tuple0 _tmp132=({struct
_tuple0 _tmp131;_tmp131.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
_tmp131.f2=*((void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmp131;});
void*_tmp13A;unsigned int _tmp13C;void*_tmp13E;int*_tmp140;void*_tmp142;
unsigned int _tmp144;void*_tmp146;struct Cyc_Std_timeval*_tmp148;_LL308: _LL319:
_tmp13E=_tmp132.f1;if(*((int*)_tmp13E)== Cyc_Std_SO_int){_LL321: _tmp140=((struct
Cyc_Std_SO_int_struct*)_tmp13E)->f1;goto _LL315;}else{goto _LL310;}_LL315: _tmp13A=
_tmp132.f2;if(*((int*)_tmp13A)== Cyc_Std_SO_socklen){_LL317: _tmp13C=((struct Cyc_Std_SO_socklen_struct*)
_tmp13A)->f1;goto _LL309;}else{goto _LL310;}_LL310: _LL327: _tmp146=_tmp132.f1;if(*((
int*)_tmp146)== Cyc_Std_SO_timeval){_LL329: _tmp148=((struct Cyc_Std_SO_timeval_struct*)
_tmp146)->f1;goto _LL323;}else{goto _LL312;}_LL323: _tmp142=_tmp132.f2;if(*((int*)
_tmp142)== Cyc_Std_SO_socklen){_LL325: _tmp144=((struct Cyc_Std_SO_socklen_struct*)
_tmp142)->f1;goto _LL311;}else{goto _LL312;}_LL312: goto _LL313;_LL309: return
setsockopt_int(fd,level,optname,(const int*)_tmp140,_tmp13C);_LL311: return
setsockopt_timeval(fd,level,optname,(const struct Cyc_Std_timeval*)_tmp148,_tmp144);
_LL313:(int)_throw((void*)({struct Cyc_Std_SocketError_struct*_tmp14A=_cycalloc(
sizeof(struct Cyc_Std_SocketError_struct));_tmp14A[0]=({struct Cyc_Std_SocketError_struct
_tmp14B;_tmp14B.tag=Cyc_Std_SocketError;_tmp14B.f1=(struct _tagged_arr)({struct
Cyc_Std_String_pa_struct _tmp14D;_tmp14D.tag=Cyc_Std_String_pa;_tmp14D.f1=(struct
_tagged_arr)Cyc_Std_sopts2string(ap);{void*_tmp14C[1]={& _tmp14D};Cyc_Std_aprintf(
_tag_arr("getsockopt---bad args %s",sizeof(unsigned char),25),_tag_arr(_tmp14C,
sizeof(void*),1));}});_tmp14B;});_tmp14A;}));_LL307:;}}extern unsigned char*
asctime(const struct Cyc_Std_tm*timeptr);extern unsigned char*ctime(const int*timep);
extern unsigned int strftime(unsigned char*s,unsigned int maxsize,unsigned char*fmt,
const struct Cyc_Std_tm*t);extern unsigned char*asctime_r(const struct Cyc_Std_tm*,
unsigned char*);extern unsigned char*ctime_r(const int*,unsigned char*);struct
_tagged_arr Cyc_Std_asctime(const struct Cyc_Std_tm*timeptr){return
wrap_Cstring_as_string(asctime(timeptr),- 1);}struct _tagged_arr Cyc_Std_ctime(
const int*timep){return wrap_Cstring_as_string(ctime(timep),- 1);}unsigned int Cyc_Std_strftime(
struct _tagged_arr s,unsigned int maxsize,struct _tagged_arr fmt,const struct Cyc_Std_tm*
t){unsigned int m=_get_arr_size(s,sizeof(unsigned char))< maxsize? _get_arr_size(s,
sizeof(unsigned char)): maxsize;return strftime(underlying_Cstring(s),m,
underlying_Cstring(fmt),t);}struct _tagged_arr Cyc_Std_asctime_r(const struct Cyc_Std_tm*
t,struct _tagged_arr s){struct _tagged_arr _tmp14E=wrap_Cstring_as_string(asctime(t),
- 1);if(Cyc_Std_strlen((struct _tagged_arr)_tmp14E)+ 1 > _get_arr_size(s,sizeof(
unsigned char))){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp14F=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp14F[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp150;_tmp150.tag=Cyc_Core_Invalid_argument;
_tmp150.f1=_tag_arr("Time::asctime_r: string too small",sizeof(unsigned char),34);
_tmp150;});_tmp14F;}));}else{Cyc_Std_strcpy(s,(struct _tagged_arr)_tmp14E);return
s;}}struct _tagged_arr Cyc_Std_ctime_r(const int*t,struct _tagged_arr s){struct
_tagged_arr _tmp151=wrap_Cstring_as_string(ctime(t),- 1);if(Cyc_Std_strlen((struct
_tagged_arr)_tmp151)+ 1 > _get_arr_size(s,sizeof(unsigned char))){(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp152=_cycalloc(sizeof(struct
Cyc_Core_Invalid_argument_struct));_tmp152[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp153;_tmp153.tag=Cyc_Core_Invalid_argument;_tmp153.f1=_tag_arr("Time::ctime_r: string too small",
sizeof(unsigned char),32);_tmp153;});_tmp152;}));}else{Cyc_Std_strcpy(s,(struct
_tagged_arr)_tmp151);return s;}}struct Cyc_Std_option{struct _tagged_arr name;int
has_arg;int*flag;int val;};extern unsigned int alarm(unsigned int seconds);extern int
close(int);extern void _exit(int);extern int getpid();extern int getppid();extern int
fork();extern int fchdir(int);extern int fchown(int,unsigned short,unsigned short);
extern int fsync(int);extern int ftruncate(int,int);extern int dup(int);extern int dup2(
int,int);extern int setsid();extern int getsid(int pid);extern unsigned short getuid();
extern int setuid(unsigned short uid);extern unsigned short geteuid();extern int
seteuid(unsigned short euid);extern unsigned short getgid();extern int setgid(
unsigned short gid);extern unsigned short getegid();extern int setegid(unsigned short
egid);extern int nice(int);extern int pause();extern int pipe(int*filedes);extern int
lseek(int filedes,int offset,int whence);extern unsigned int sleep(unsigned int);
extern int isatty(int);extern int daemon(int nochdir,int noclose);int Cyc_Std_access(
struct _tagged_arr,int);int Cyc_Std_chdir(struct _tagged_arr);int Cyc_Std_chown(
struct _tagged_arr,unsigned short,unsigned short);struct _tagged_arr Cyc_Std_getcwd(
struct _tagged_arr buf,unsigned int size);int Cyc_Std_execl(struct _tagged_arr path,
struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execlp(struct _tagged_arr
file,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execve(struct
_tagged_arr filename,struct _tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,
unsigned int count);int Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(
int fd,struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct
_tagged_arr pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int
Cyc_Std_chroot(struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct
_tagged_arr prompt);extern int access(unsigned char*,int);extern int chdir(
unsigned char*);extern int chown(unsigned char*,unsigned short,unsigned short);
extern unsigned char*getcwd(unsigned char*buf,unsigned int size);extern int execv(
unsigned char*path,unsigned char**argv);extern int execvp(unsigned char*file,
unsigned char**argv);extern int execve(unsigned char*path,unsigned char**argv,
unsigned char**envp);extern int link(unsigned char*path1,unsigned char*path2);
extern int read(int fd,unsigned char*buf,unsigned int count);extern int rmdir(
unsigned char*);extern int symlink(unsigned char*path1,unsigned char*path2);extern
int truncate(unsigned char*,int);extern int write(int fd,unsigned char*buf,
unsigned int count);extern int unlink(unsigned char*pathname);extern int gethostname(
unsigned char*,unsigned int);extern int chroot(unsigned char*);extern unsigned char*
getpass(unsigned char*);int Cyc_Std_access(struct _tagged_arr path,int mode){return
access(string_to_Cstring(path),mode);}int Cyc_Std_chdir(struct _tagged_arr path){
return chdir(string_to_Cstring(path));}int Cyc_Std_chown(struct _tagged_arr path,
unsigned short owner,unsigned short group){return chown(string_to_Cstring(path),
owner,group);}struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int
size){if(!((unsigned int)buf.curr)? 1: _get_arr_size(buf,sizeof(unsigned char))< 
size){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp154=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmp154[0]=({struct Cyc_Core_Failure_struct
_tmp155;_tmp155.tag=Cyc_Core_Failure;_tmp155.f1=_tag_arr("getcwd: invalid buf argument",
sizeof(unsigned char),29);_tmp155;});_tmp154;}));}{unsigned char*response=getcwd((
unsigned char*)_check_null(_untag_arr(buf,sizeof(unsigned char),0)),size);return(
unsigned int)response? buf: _tag_arr(0,0,0);}}int Cyc_Std_execl(struct _tagged_arr
path,struct _tagged_arr arg0,struct _tagged_arr argv){if((*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(int)(_get_arr_size(argv,
sizeof(struct _tagged_arr))- 1)))).curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp156=_cycalloc(sizeof(
struct Cyc_Core_Failure_struct));_tmp156[0]=({struct Cyc_Core_Failure_struct
_tmp157;_tmp157.tag=Cyc_Core_Failure;_tmp157.f1=_tag_arr("execl: arg list must be NULL-terminated",
sizeof(unsigned char),40);_tmp157;});_tmp156;}));}{struct _tagged_arr newargs=({
unsigned int _tmp158=1 + _get_arr_size(argv,sizeof(struct _tagged_arr));
unsigned char**_tmp159=(unsigned char**)_cycalloc(_check_times(sizeof(
unsigned char*),_tmp158));struct _tagged_arr _tmp15B=_tag_arr(_tmp159,sizeof(
unsigned char*),1 + _get_arr_size(argv,sizeof(struct _tagged_arr)));{unsigned int
_tmp15A=_tmp158;unsigned int i;for(i=0;i < _tmp15A;i ++){_tmp159[i]=0;}};_tmp15B;});*((
unsigned char**)_check_unknown_subscript(newargs,sizeof(unsigned char*),0))=
string_to_Cstring(arg0);{int i=0;for(0;i < _get_arr_size(argv,sizeof(struct
_tagged_arr));i ++){*((unsigned char**)_check_unknown_subscript(newargs,sizeof(
unsigned char*),i + 1))=string_to_Cstring(((struct _tagged_arr*)argv.curr)[i]);}}
return execv(string_to_Cstring(path),(unsigned char**)_check_null(_untag_arr(
newargs,sizeof(unsigned char*),1)));}}int Cyc_Std_execlp(struct _tagged_arr path,
struct _tagged_arr arg0,struct _tagged_arr argv){if((*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(int)(_get_arr_size(argv,
sizeof(struct _tagged_arr))- 1)))).curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp15C=_cycalloc(sizeof(
struct Cyc_Core_Failure_struct));_tmp15C[0]=({struct Cyc_Core_Failure_struct
_tmp15D;_tmp15D.tag=Cyc_Core_Failure;_tmp15D.f1=_tag_arr("execl: arg list must be NULL-terminated",
sizeof(unsigned char),40);_tmp15D;});_tmp15C;}));}{struct _tagged_arr newargs=({
unsigned int _tmp15E=1 + _get_arr_size(argv,sizeof(struct _tagged_arr));
unsigned char**_tmp15F=(unsigned char**)_cycalloc(_check_times(sizeof(
unsigned char*),_tmp15E));struct _tagged_arr _tmp161=_tag_arr(_tmp15F,sizeof(
unsigned char*),1 + _get_arr_size(argv,sizeof(struct _tagged_arr)));{unsigned int
_tmp160=_tmp15E;unsigned int i;for(i=0;i < _tmp160;i ++){_tmp15F[i]=0;}};_tmp161;});*((
unsigned char**)_check_unknown_subscript(newargs,sizeof(unsigned char*),0))=
string_to_Cstring(arg0);{int i=0;for(0;i < _get_arr_size(argv,sizeof(struct
_tagged_arr));i ++){*((unsigned char**)_check_unknown_subscript(newargs,sizeof(
unsigned char*),i + 1))=string_to_Cstring(((struct _tagged_arr*)argv.curr)[i]);}}
return execvp(string_to_Cstring(path),(unsigned char**)_check_null(_untag_arr(
newargs,sizeof(unsigned char*),1)));}}int Cyc_Std_execve(struct _tagged_arr
filename,struct _tagged_arr argv,struct _tagged_arr envp){if((*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(int)(_get_arr_size(argv,
sizeof(struct _tagged_arr))- 1)))).curr != (_tag_arr(0,0,0)).curr){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp162=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp162[0]=({struct Cyc_Core_Failure_struct _tmp163;_tmp163.tag=Cyc_Core_Failure;
_tmp163.f1=_tag_arr("execve: arg list must be NULL-terminated",sizeof(
unsigned char),41);_tmp163;});_tmp162;}));}{struct _tagged_arr newargs=({
unsigned int _tmp168=_get_arr_size(argv,sizeof(struct _tagged_arr));unsigned char**
_tmp169=(unsigned char**)_cycalloc(_check_times(sizeof(unsigned char*),_tmp168));
struct _tagged_arr _tmp16B=_tag_arr(_tmp169,sizeof(unsigned char*),_get_arr_size(
argv,sizeof(struct _tagged_arr)));{unsigned int _tmp16A=_tmp168;unsigned int i;for(
i=0;i < _tmp16A;i ++){_tmp169[i]=0;}};_tmp16B;});{int i=0;for(0;i < _get_arr_size(
argv,sizeof(struct _tagged_arr));i ++){((unsigned char**)newargs.curr)[i]=
string_to_Cstring((struct _tagged_arr)((struct _tagged_arr*)argv.curr)[i]);}}{
struct _tagged_arr newenvp=({unsigned int _tmp164=_get_arr_size(envp,sizeof(struct
_tagged_arr));unsigned char**_tmp165=(unsigned char**)_cycalloc(_check_times(
sizeof(unsigned char*),_tmp164));struct _tagged_arr _tmp167=_tag_arr(_tmp165,
sizeof(unsigned char*),_get_arr_size(envp,sizeof(struct _tagged_arr)));{
unsigned int _tmp166=_tmp164;unsigned int i;for(i=0;i < _tmp166;i ++){_tmp165[i]=0;}};
_tmp167;});{int i=0;for(0;i < _get_arr_size(envp,sizeof(struct _tagged_arr));i ++){((
unsigned char**)newenvp.curr)[i]=string_to_Cstring((struct _tagged_arr)((struct
_tagged_arr*)envp.curr)[i]);}}return execve(string_to_Cstring(filename),(
unsigned char**)_check_null(_untag_arr(newargs,sizeof(unsigned char*),1)),(
unsigned char**)_check_null(_untag_arr(newenvp,sizeof(unsigned char*),1)));}}}
int Cyc_Std_link(struct _tagged_arr path1,struct _tagged_arr path2){return link(
string_to_Cstring(path1),string_to_Cstring(path2));}int Cyc_Std_read(int fd,struct
_tagged_arr buf,unsigned int count){if(count > _get_arr_size(buf,sizeof(
unsigned char))){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp16C=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp16C[0]=({struct Cyc_Core_Failure_struct
_tmp16D;_tmp16D.tag=Cyc_Core_Failure;_tmp16D.f1=_tag_arr("read: called with count > buf.size",
sizeof(unsigned char),35);_tmp16D;});_tmp16C;}));}return read(fd,
underlying_Cstring((struct _tagged_arr)buf),count);}int Cyc_Std_rmdir(struct
_tagged_arr path){return rmdir(string_to_Cstring(path));}int Cyc_Std_symlink(struct
_tagged_arr path1,struct _tagged_arr path2){return symlink(string_to_Cstring(path1),
string_to_Cstring(path2));}int Cyc_Std_truncate(struct _tagged_arr path,int length){
return truncate(string_to_Cstring(path),length);}int Cyc_Std_write(int fd,struct
_tagged_arr buf,unsigned int count){if(count > _get_arr_size(buf,sizeof(
unsigned char))){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp16E=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp16E[0]=({struct Cyc_Core_Failure_struct
_tmp16F;_tmp16F.tag=Cyc_Core_Failure;_tmp16F.f1=_tag_arr("write: called with count > buf.size",
sizeof(unsigned char),36);_tmp16F;});_tmp16E;}));}return write(fd,
underlying_Cstring(buf),count);}int Cyc_Std_unlink(struct _tagged_arr pathname){
return unlink(string_to_Cstring(pathname));}int Cyc_Std_gethostname(struct
_tagged_arr buf,unsigned int count){if(count > _get_arr_size(buf,sizeof(
unsigned char))){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp170=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp170[0]=({struct Cyc_Core_Failure_struct
_tmp171;_tmp171.tag=Cyc_Core_Failure;_tmp171.f1=_tag_arr("gethostname: called with count > buf.size",
sizeof(unsigned char),42);_tmp171;});_tmp170;}));}return gethostname(
underlying_Cstring((struct _tagged_arr)buf),count);}int Cyc_Std_chroot(struct
_tagged_arr pathname){return chroot(string_to_Cstring(pathname));}struct
_tagged_arr Cyc_Std_getpass(struct _tagged_arr prompt){return wrap_Cstring_as_string(
getpass(string_to_Cstring(prompt)),- 1);}struct Cyc_utimbuf{int actime;int modtime;}
;extern int utime(unsigned char*filename,struct Cyc_utimbuf*buf);int Cyc_Std_utime(
struct _tagged_arr filename,struct Cyc_utimbuf*buf){return utime(string_to_Cstring(
filename),buf);}
