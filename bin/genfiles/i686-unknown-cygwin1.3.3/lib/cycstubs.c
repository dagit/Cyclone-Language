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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void   _reset_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

//// Allocation
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};char*
string_to_Cstring(struct _tagged_arr);char*underlying_Cstring(struct _tagged_arr);
struct _tagged_arr Cstring_to_string(char*);struct _tagged_arr
wrap_Cstring_as_string(char*,unsigned int);struct Cyc_Std_in_addr{unsigned int
s_addr;};unsigned int htonl(unsigned int);unsigned short htons(unsigned short);
unsigned int ntohl(unsigned int);unsigned short ntohs(unsigned short);unsigned int
Cyc_Std_inet_addr(struct _tagged_arr);int Cyc_Std_inet_aton(struct _tagged_arr,
struct Cyc_Std_in_addr*);struct _tagged_arr Cyc_Std_inet_ntoa(struct Cyc_Std_in_addr);
unsigned int inet_addr(char*);unsigned int Cyc_Std_inet_addr(struct _tagged_arr addr){
return inet_addr(string_to_Cstring(addr));}int inet_aton(char*,struct Cyc_Std_in_addr*);
int Cyc_Std_inet_aton(struct _tagged_arr cp,struct Cyc_Std_in_addr*inp){return
inet_aton(string_to_Cstring(cp),inp);}char*inet_ntoa(struct Cyc_Std_in_addr);
struct _tagged_arr Cyc_Std_inet_ntoa(struct Cyc_Std_in_addr x){return
wrap_Cstring_as_string(inet_ntoa(x),- 1);}struct Cyc_Std_dirent{int d_version;int
__d_reserved[2];int d_fd;unsigned int d_ino;char d_name[256];};struct Cyc_Std___cycDIR;
int Cyc_Std_closedir(struct Cyc_Std___cycDIR*);struct Cyc_Std___cycDIR*Cyc_Std_opendir(
struct _tagged_arr);struct Cyc_Std_dirent*Cyc_Std_readdir(struct Cyc_Std___cycDIR*);
void Cyc_Std_rewinddir(struct Cyc_Std___cycDIR*);void Cyc_Std_seekdir(struct Cyc_Std___cycDIR*,
int);int Cyc_Std_telldir(struct Cyc_Std___cycDIR*);struct Cyc_Std_Cdirent___abstractDIR;
struct Cyc_Std___cycDIR{struct Cyc_Std_Cdirent___abstractDIR*dir;};int closedir(
struct Cyc_Std_Cdirent___abstractDIR*d);int Cyc_Std_closedir(struct Cyc_Std___cycDIR*
d){return closedir(d->dir);}struct Cyc_Std_Cdirent___abstractDIR*opendir(char*);
static char _tmp1[32]="opendir called with NULL string";static struct Cyc_Core_Failure_struct
Cyc_Std___opendir_failure={Cyc_Core_Failure,{_tmp1,_tmp1,_tmp1 + 32}};struct Cyc_Std___cycDIR*
Cyc_Std_opendir(struct _tagged_arr f){if(!((unsigned int)f.curr))(int)_throw((void*)&
Cyc_Std___opendir_failure);{struct Cyc_Std_Cdirent___abstractDIR*_tmp2=opendir(
string_to_Cstring(f));return(unsigned int)_tmp2?({struct Cyc_Std___cycDIR*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3->dir=(struct Cyc_Std_Cdirent___abstractDIR*)
_check_null(_tmp2);_tmp3;}): 0;}}struct Cyc_Std_dirent*readdir(struct Cyc_Std_Cdirent___abstractDIR*);
struct Cyc_Std_dirent*Cyc_Std_readdir(struct Cyc_Std___cycDIR*d){return readdir(d->dir);}
void rewinddir(struct Cyc_Std_Cdirent___abstractDIR*d);void Cyc_Std_rewinddir(
struct Cyc_Std___cycDIR*d){return rewinddir(d->dir);}void seekdir(struct Cyc_Std_Cdirent___abstractDIR*
d,int);void Cyc_Std_seekdir(struct Cyc_Std___cycDIR*d,int x){return seekdir(d->dir,x);}
int telldir(struct Cyc_Std_Cdirent___abstractDIR*d);int Cyc_Std_telldir(struct Cyc_Std___cycDIR*
d){return telldir(d->dir);}struct Cyc_Std_flock{short l_type;short l_whence;int
l_start;int l_len;short l_pid;short l_xxx;};struct Cyc_Std_Flock_struct{int tag;
struct Cyc_Std_flock*f1;};struct Cyc_Std_Long_struct{int tag;int f1;};int Cyc_Std_fcntl(
int fd,int cmd,struct _tagged_arr);int Cyc_Std_open(struct _tagged_arr,int,struct
_tagged_arr);int Cyc_Std_creat(struct _tagged_arr,int);int fcntl(int fd,int cmd);int
fcntl_with_arg(int fd,int cmd,int arg);int fcntl_with_lock(int fd,int cmd,struct Cyc_Std_flock*
lock);int creat(char*,int);int open_without_mode(char*,int);int open_with_mode(char*,
int,int);static char _tmp5[21]="fcntl: too many args";static struct Cyc_Core_Failure_struct
Cyc_Std___fcntl_failure={Cyc_Core_Failure,{_tmp5,_tmp5,_tmp5 + 21}};int Cyc_Std_fcntl(
int fd,int cmd,struct _tagged_arr argv){if(_get_arr_size(argv,sizeof(void*))== 0)
return fcntl(fd,cmd);else{if(_get_arr_size(argv,sizeof(void*))!= 1)(int)_throw((
void*)& Cyc_Std___fcntl_failure);else{void*_tmp6=*((void**)
_check_unknown_subscript(argv,sizeof(void*),0));int _tmp7;struct Cyc_Std_flock*
_tmp8;_LL1: if(*((int*)_tmp6)!= 1)goto _LL3;_tmp7=((struct Cyc_Std_Long_struct*)
_tmp6)->f1;_LL2: return fcntl_with_arg(fd,cmd,_tmp7);_LL3: if(*((int*)_tmp6)!= 0)
goto _LL0;_tmp8=((struct Cyc_Std_Flock_struct*)_tmp6)->f1;_LL4: return
fcntl_with_lock(fd,cmd,(struct Cyc_Std_flock*)_tmp8);_LL0:;}}}int Cyc_Std_creat(
struct _tagged_arr s,int m){return creat(string_to_Cstring(s),m);}int Cyc_Std_open(
struct _tagged_arr s,int i,struct _tagged_arr ms){if(_get_arr_size(ms,sizeof(int))>= 
1)return open_with_mode(string_to_Cstring(s),i,*((int*)_check_unknown_subscript(
ms,sizeof(int),0)));else{return open_without_mode(string_to_Cstring(s),i);}}enum 
Cyc_Std___anonymous_enum_30__{Cyc_Std_IPPROTO_IP  = 0,Cyc_Std_IPPROTO_ICMP  = 1,
Cyc_Std_IPPROTO_IGMP  = 2,Cyc_Std_IPPROTO_IPIP  = 4,Cyc_Std_IPPROTO_TCP  = 6,Cyc_Std_IPPROTO_EGP
 = 8,Cyc_Std_IPPROTO_PUP  = 12,Cyc_Std_IPPROTO_UDP  = 17,Cyc_Std_IPPROTO_IDP  = 22,
Cyc_Std_IPPROTO_RAW  = 255,Cyc_Std_IPPROTO_MAX  = 256};struct Cyc_Std_sockaddr_in{
short sin_family;unsigned short sin_port;struct Cyc_Std_in_addr sin_addr;
unsigned char __pad[8];};struct Cyc_Std_in6_addr{unsigned char s6_addr[16];};struct
Cyc_Std_sockaddr_in6{unsigned short sin6_family;unsigned short sin6_port;
unsigned int sin6_flowinfo;struct Cyc_Std_in6_addr sin6_addr;};unsigned int htonl(
unsigned int);unsigned short htons(unsigned short);unsigned int ntohl(unsigned int);
unsigned short ntohs(unsigned short);struct Cyc_Std_pollfd{int fd;short events;short
revents;};int Cyc_Std_poll(struct _tagged_arr,unsigned int,int);int poll(struct Cyc_Std_pollfd*,
unsigned int b,int c);static char _tmpA[22]="poll called with NULL";static struct Cyc_Core_Failure_struct
Cyc_Std___poll_failure_1={Cyc_Core_Failure,{_tmpA,_tmpA,_tmpA + 22}};static char
_tmpC[39]="poll called with out-of-bounds pointer";static struct Cyc_Core_Failure_struct
Cyc_Std___poll_failure_2={Cyc_Core_Failure,{_tmpC,_tmpC,_tmpC + 39}};int Cyc_Std_poll(
struct _tagged_arr a,unsigned int b,int c){if(!((unsigned int)a.curr))(int)_throw((
void*)& Cyc_Std___poll_failure_1);{struct _handler_cons _tmpD;_push_handler(& _tmpD);{
int _tmpF=0;if(setjmp(_tmpD.handler))_tmpF=1;if(!_tmpF){*((struct Cyc_Std_pollfd*)
_check_unknown_subscript(a,sizeof(struct Cyc_Std_pollfd),0));;_pop_handler();}
else{void*_tmpE=(void*)_exn_thrown;void*_tmp11=_tmpE;_LL6: if(_tmp11 != Cyc_Array_bounds)
goto _LL8;_LL7:(int)_throw((void*)& Cyc_Std___poll_failure_2);_LL8:;_LL9:(void)
_throw(_tmp11);_LL5:;}}}{int len=(int)_get_arr_size(a,sizeof(struct Cyc_Std_pollfd));
return poll((struct Cyc_Std_pollfd*)_untag_arr(a,sizeof(struct Cyc_Std_pollfd),0),b
< (unsigned int)len?b:(unsigned int)len,c);}}struct Cyc_Std___SIG_BUILTIN_struct{
int tag;void*f1;};struct Cyc_Std___SIG_HANDLER_struct{int tag;void(*f1)(int);};void*
Cyc_Std_Csignal_make_SIG_DFL(){return(void*)({struct Cyc_Std___SIG_BUILTIN_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Std___SIG_BUILTIN_struct
_tmp13;_tmp13.tag=0;_tmp13.f1=(void*)((void*)0);_tmp13;});_tmp12;});}void*Cyc_Std_Csignal_make_SIG_ERR(){
return(void*)({struct Cyc_Std___SIG_BUILTIN_struct*_tmp14=_cycalloc(sizeof(*
_tmp14));_tmp14[0]=({struct Cyc_Std___SIG_BUILTIN_struct _tmp15;_tmp15.tag=0;
_tmp15.f1=(void*)((void*)1);_tmp15;});_tmp14;});}void*Cyc_Std_Csignal_make_SIG_IGN(){
return(void*)({struct Cyc_Std___SIG_BUILTIN_struct*_tmp16=_cycalloc(sizeof(*
_tmp16));_tmp16[0]=({struct Cyc_Std___SIG_BUILTIN_struct _tmp17;_tmp17.tag=0;
_tmp17.f1=(void*)((void*)1);_tmp17;});_tmp16;});}void*Cyc_Std_Csignal_make_SIG_HANDLER(
void(*f)(int)){return(void*)({struct Cyc_Std___SIG_HANDLER_struct*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Std___SIG_HANDLER_struct _tmp19;
_tmp19.tag=1;_tmp19.f1=f;_tmp19;});_tmp18;});}struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE*Cyc_Std_fromCfile(struct Cyc_Std_Cstdio___abstractFILE*cf);
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};void Cyc_Std_clearerr(struct Cyc_Std___cycFILE*);int Cyc_Std_fclose(
struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fdopen(int,struct
_tagged_arr);int Cyc_Std_feof(struct Cyc_Std___cycFILE*);int Cyc_Std_ferror(struct
Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);int Cyc_Std_fgetc(
struct Cyc_Std___cycFILE*);int Cyc_Std_fgetpos(struct Cyc_Std___cycFILE*,int*);
struct _tagged_arr Cyc_Std_fgets(struct _tagged_arr,int,struct Cyc_Std___cycFILE*);
int Cyc_Std_fileno(struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*Cyc_Std_fopen(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_fputc(int,struct Cyc_Std___cycFILE*);
int Cyc_Std_fputs(struct _tagged_arr,struct Cyc_Std___cycFILE*);unsigned int Cyc_Std_fread(
struct _tagged_arr,unsigned int,unsigned int,struct Cyc_Std___cycFILE*);struct Cyc_Std___cycFILE*
Cyc_Std_freopen(struct _tagged_arr,struct _tagged_arr,struct Cyc_Std___cycFILE*);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_fseek(struct Cyc_Std___cycFILE*,
int,int);int Cyc_Std_fsetpos(struct Cyc_Std___cycFILE*,const int*);int Cyc_Std_ftell(
struct Cyc_Std___cycFILE*);unsigned int Cyc_Std_fwrite(struct _tagged_arr,
unsigned int,unsigned int,struct Cyc_Std___cycFILE*);int Cyc_Std_getc(struct Cyc_Std___cycFILE*);
int Cyc_Std_getchar();int Cyc_Std_pclose(struct Cyc_Std___cycFILE*);void Cyc_Std_perror(
struct _tagged_arr);struct Cyc_Std___cycFILE*Cyc_Std_popen(struct _tagged_arr,
struct _tagged_arr);int Cyc_Std_putc(int,struct Cyc_Std___cycFILE*);int Cyc_Std_puts(
struct _tagged_arr);int Cyc_Std_remove(struct _tagged_arr);int Cyc_Std_rename(struct
_tagged_arr,struct _tagged_arr);void Cyc_Std_rewind(struct Cyc_Std___cycFILE*);void
Cyc_Std_setbuf(struct Cyc_Std___cycFILE*,struct _tagged_arr);int Cyc_Std_setvbuf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,int,unsigned int);int Cyc_Std_ungetc(
int,struct Cyc_Std___cycFILE*);int Cyc_Std_getw(struct Cyc_Std___cycFILE*);int Cyc_Std_putw(
int,struct Cyc_Std___cycFILE*);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std___cycFILE*
Cyc_Std_file_open(struct _tagged_arr,struct _tagged_arr);void Cyc_Std_file_close(
struct Cyc_Std___cycFILE*);struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std___cycFILE{
struct Cyc_Std_Cstdio___abstractFILE*file;};struct Cyc_Std___cycFILE*Cyc_Std_fromCfile(
struct Cyc_Std_Cstdio___abstractFILE*cf){return(unsigned int)cf?({struct Cyc_Std___cycFILE*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->file=(struct Cyc_Std_Cstdio___abstractFILE*)
_check_null(cf);_tmp1A;}): 0;}void clearerr(struct Cyc_Std_Cstdio___abstractFILE*);
void Cyc_Std_clearerr(struct Cyc_Std___cycFILE*f){return clearerr(f->file);}int
fclose(struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_fclose(struct Cyc_Std___cycFILE*
f){return fclose(f->file);}struct Cyc_Std_Cstdio___abstractFILE*fdopen(int,char*);
struct Cyc_Std___cycFILE*Cyc_Std_fdopen(int i,struct _tagged_arr s){struct Cyc_Std_Cstdio___abstractFILE*
_tmp1B=fdopen(i,string_to_Cstring(s));return(unsigned int)_tmp1B?({struct Cyc_Std___cycFILE*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->file=(struct Cyc_Std_Cstdio___abstractFILE*)
_check_null(_tmp1B);_tmp1C;}): 0;}int feof(struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_feof(struct Cyc_Std___cycFILE*f){return feof(f->file);}int ferror(struct
Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_ferror(struct Cyc_Std___cycFILE*f){
return ferror(f->file);}int fflush(struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_fflush(
struct Cyc_Std___cycFILE*f){if((unsigned int)f)return fflush((struct Cyc_Std_Cstdio___abstractFILE*)
f->file);else{return fflush(0);}}int fgetc(struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_fgetc(struct Cyc_Std___cycFILE*f){return fgetc(f->file);}int fgetpos(
struct Cyc_Std_Cstdio___abstractFILE*,int*);int Cyc_Std_fgetpos(struct Cyc_Std___cycFILE*
f,int*x){return fgetpos(f->file,x);}char*fgets(char*,int,struct Cyc_Std_Cstdio___abstractFILE*);
struct _tagged_arr Cyc_Std_fgets(struct _tagged_arr s,int n,struct Cyc_Std___cycFILE*f){
char*result;char*buffer=underlying_Cstring((struct _tagged_arr)s);unsigned int len=
_get_arr_size(s,sizeof(char));n=(int)(len < n?len:(unsigned int)n);result=fgets(
buffer,n,f->file);if(result == 0)return _tag_arr(0,0,0);else{return s;}}int fileno(
struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_fileno(struct Cyc_Std___cycFILE*
f){return fileno(f->file);}struct Cyc_Std_Cstdio___abstractFILE*fopen(char*,char*);
static char _tmp1E[23]="fopen called with NULL";static struct Cyc_Core_Failure_struct
Cyc_Std___fopen_failure={Cyc_Core_Failure,{_tmp1E,_tmp1E,_tmp1E + 23}};struct Cyc_Std___cycFILE*
Cyc_Std_fopen(struct _tagged_arr name,struct _tagged_arr type){if(!((unsigned int)
name.curr)?1: !((unsigned int)type.curr))(int)_throw((void*)& Cyc_Std___fopen_failure);{
struct Cyc_Std_Cstdio___abstractFILE*_tmp1F=fopen(string_to_Cstring(name),
string_to_Cstring(type));return(unsigned int)_tmp1F?({struct Cyc_Std___cycFILE*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->file=(struct Cyc_Std_Cstdio___abstractFILE*)
_check_null(_tmp1F);_tmp20;}): 0;}}int fputc(int,struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_fputc(int x,struct Cyc_Std___cycFILE*f){return fputc(x,f->file);}int
fputs(char*,struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_fputs(struct
_tagged_arr x,struct Cyc_Std___cycFILE*f){if(!((unsigned int)x.curr))return 0;
return fputs(string_to_Cstring(x),f->file);}unsigned int fread(char*,unsigned int,
unsigned int,struct Cyc_Std_Cstdio___abstractFILE*);static char _tmp22[27]="fread: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc_Std___fread_failure={Cyc_Core_Failure,{
_tmp22,_tmp22,_tmp22 + 27}};unsigned int Cyc_Std_fread(struct _tagged_arr ptr,
unsigned int size,unsigned int nmemb,struct Cyc_Std___cycFILE*f){if(size * nmemb > 
_get_arr_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc_Std___fread_failure);
return fread(underlying_Cstring((struct _tagged_arr)ptr),size,nmemb,f->file);}
struct Cyc_Std_Cstdio___abstractFILE*freopen(char*,char*,struct Cyc_Std_Cstdio___abstractFILE*);
static char _tmp24[36]="freopen called with NULL file modes";static struct Cyc_Core_Failure_struct
Cyc_Std___freopen_failure={Cyc_Core_Failure,{_tmp24,_tmp24,_tmp24 + 36}};struct
Cyc_Std___cycFILE*Cyc_Std_freopen(struct _tagged_arr filename,struct _tagged_arr
modes,struct Cyc_Std___cycFILE*f){if(!((unsigned int)modes.curr))(int)_throw((
void*)& Cyc_Std___freopen_failure);{struct Cyc_Std_Cstdio___abstractFILE*_tmp25=
freopen(string_to_Cstring(filename),string_to_Cstring(modes),f->file);return(
unsigned int)_tmp25?(struct Cyc_Std___cycFILE*)({struct Cyc_Std___cycFILE*_tmp26=
_cycalloc(sizeof(*_tmp26));_tmp26->file=(struct Cyc_Std_Cstdio___abstractFILE*)
_check_null(_tmp25);_tmp26;}): 0;}}int fseek(struct Cyc_Std_Cstdio___abstractFILE*,
int,int);int Cyc_Std_fseek(struct Cyc_Std___cycFILE*a,int b,int c){return fseek(a->file,
b,c);}int fsetpos(struct Cyc_Std_Cstdio___abstractFILE*,const int*);int Cyc_Std_fsetpos(
struct Cyc_Std___cycFILE*f,const int*b){return fsetpos(f->file,b);}int ftell(struct
Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_ftell(struct Cyc_Std___cycFILE*f){
return ftell(f->file);}unsigned int fwrite(char*,unsigned int,unsigned int,struct
Cyc_Std_Cstdio___abstractFILE*);static char _tmp28[31]="fwrite called with NULL string";
static struct Cyc_Core_Failure_struct Cyc_Std___fwrite_failure_1={Cyc_Core_Failure,{
_tmp28,_tmp28,_tmp28 + 31}};static char _tmp2A[28]="fwrite: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc_Std___fwrite_failure_2={Cyc_Core_Failure,{
_tmp2A,_tmp2A,_tmp2A + 28}};unsigned int Cyc_Std_fwrite(struct _tagged_arr ptr,
unsigned int size,unsigned int nmemb,struct Cyc_Std___cycFILE*f){if(!((unsigned int)
ptr.curr))(int)_throw((void*)& Cyc_Std___fwrite_failure_1);if(size * nmemb > 
_get_arr_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc_Std___fwrite_failure_2);
return fwrite(underlying_Cstring(ptr),size,nmemb,f->file);}int getc(struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_getc(struct Cyc_Std___cycFILE*f){return getc(f->file);}int getchar();int
Cyc_Std_getchar(){return getchar();}int pclose(struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_pclose(struct Cyc_Std___cycFILE*f){return pclose(f->file);}void perror(
char*);void Cyc_Std_perror(struct _tagged_arr x){return perror(string_to_Cstring(x));}
struct Cyc_Std_Cstdio___abstractFILE*popen(char*,char*);static char _tmp2C[23]="popen called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___popen_failure={Cyc_Core_Failure,{
_tmp2C,_tmp2C,_tmp2C + 23}};struct Cyc_Std___cycFILE*Cyc_Std_popen(struct
_tagged_arr name,struct _tagged_arr type){if(!((unsigned int)name.curr)?1: !((
unsigned int)type.curr))(int)_throw((void*)& Cyc_Std___popen_failure);{struct Cyc_Std_Cstdio___abstractFILE*
_tmp2D=popen(string_to_Cstring(name),string_to_Cstring(type));return(
unsigned int)_tmp2D?({struct Cyc_Std___cycFILE*_tmp2E=_cycalloc(sizeof(*_tmp2E));
_tmp2E->file=(struct Cyc_Std_Cstdio___abstractFILE*)_check_null(_tmp2D);_tmp2E;}):
0;}}int putc(int,struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_putc(int x,
struct Cyc_Std___cycFILE*f){return putc(x,f->file);}int puts(char*);static char
_tmp30[22]="puts called with NULL";static struct Cyc_Core_Failure_struct Cyc_Std___puts_failure={
Cyc_Core_Failure,{_tmp30,_tmp30,_tmp30 + 22}};int Cyc_Std_puts(struct _tagged_arr x){
if(!((unsigned int)x.curr))(int)_throw((void*)& Cyc_Std___puts_failure);return
puts(string_to_Cstring(x));}int remove(char*);static char _tmp32[24]="remove called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___remove_failure={Cyc_Core_Failure,{
_tmp32,_tmp32,_tmp32 + 24}};int Cyc_Std_remove(struct _tagged_arr x){if(!((
unsigned int)x.curr))(int)_throw((void*)& Cyc_Std___remove_failure);return remove(
string_to_Cstring(x));}int rename(char*,char*);static char _tmp34[24]="rename called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___rename_failure={Cyc_Core_Failure,{
_tmp34,_tmp34,_tmp34 + 24}};int Cyc_Std_rename(struct _tagged_arr x,struct
_tagged_arr y){if(!((unsigned int)x.curr)?1: !((unsigned int)y.curr))(int)_throw((
void*)& Cyc_Std___rename_failure);return rename(string_to_Cstring(x),
string_to_Cstring(y));}void rewind(struct Cyc_Std_Cstdio___abstractFILE*);void Cyc_Std_rewind(
struct Cyc_Std___cycFILE*f){return rewind(f->file);}void Cyc_Std_setbuf(struct Cyc_Std___cycFILE*
f,struct _tagged_arr buf){Cyc_Std_setvbuf(f,buf,(unsigned int)buf.curr?0: 2,1024);
return;}int setvbuf(struct Cyc_Std_Cstdio___abstractFILE*,char*,int,unsigned int);
static char _tmp36[29]="setvbuf: buffer insufficient";static struct Cyc_Core_Failure_struct
Cyc_Std___setvbuf_failure={Cyc_Core_Failure,{_tmp36,_tmp36,_tmp36 + 29}};int Cyc_Std_setvbuf(
struct Cyc_Std___cycFILE*f,struct _tagged_arr buf,int type,unsigned int size){if(
_get_arr_size(buf,sizeof(char))< size)(int)_throw((void*)& Cyc_Std___setvbuf_failure);
return setvbuf(f->file,underlying_Cstring((struct _tagged_arr)buf),type,size);}int
ungetc(int,struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_ungetc(int x,struct
Cyc_Std___cycFILE*f){return ungetc(x,f->file);}int getw(struct Cyc_Std_Cstdio___abstractFILE*);
int Cyc_Std_getw(struct Cyc_Std___cycFILE*f){return getw(f->file);}int putw(int,
struct Cyc_Std_Cstdio___abstractFILE*);int Cyc_Std_putw(int x,struct Cyc_Std___cycFILE*
f){return putw(x,f->file);}char Cyc_Std_FileCloseError[19]="\000\000\000\000FileCloseError";
char Cyc_Std_FileOpenError[18]="\000\000\000\000FileOpenError";struct Cyc_Std___cycFILE*
Cyc_Std_file_open(struct _tagged_arr fname,struct _tagged_arr mode){struct Cyc_Std___cycFILE*
f=Cyc_Std_fopen(fname,mode);if(f == 0){struct _tagged_arr fn=({unsigned int _tmp39=
_get_arr_size(fname,sizeof(char));char*_tmp3A=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp39));struct _tagged_arr _tmp3C=_tag_arr(_tmp3A,
sizeof(char),_get_arr_size(fname,sizeof(char)));{unsigned int _tmp3B=_tmp39;
unsigned int i;for(i=0;i < _tmp3B;i ++){_tmp3A[i]=((const char*)fname.curr)[(int)i];}}
_tmp3C;});(int)_throw((void*)({struct Cyc_Std_FileOpenError_struct*_tmp37=
_cycalloc(sizeof(*_tmp37));_tmp37[0]=({struct Cyc_Std_FileOpenError_struct _tmp38;
_tmp38.tag=Cyc_Std_FileOpenError;_tmp38.f1=fn;_tmp38;});_tmp37;}));}return(
struct Cyc_Std___cycFILE*)_check_null(f);}void Cyc_Std_file_close(struct Cyc_Std___cycFILE*
f){if(Cyc_Std_fclose(f)!= 0)(int)_throw((void*)Cyc_Std_FileCloseError);}typedef
struct{int quot;int rem;}Cyc_Std_div_t;typedef struct{int quot;int rem;}Cyc_Std_ldiv_t;
double Cyc_Std_atof(struct _tagged_arr);int Cyc_Std_atoi(struct _tagged_arr);int Cyc_Std_atol(
struct _tagged_arr);void Cyc_Std_free(struct _tagged_arr);struct _tagged_arr Cyc_Std_getenv(
struct _tagged_arr);int Cyc_Std_putenv(struct _tagged_arr);void Cyc_Std_qsort(struct
_tagged_arr,unsigned int,unsigned int,int(*)(const void*,const void*));double Cyc_Std_strtod(
struct _tagged_arr,struct _tagged_arr*);int Cyc_Std_strtol(struct _tagged_arr,struct
_tagged_arr*,int);unsigned int Cyc_Std_strtoul(struct _tagged_arr,struct
_tagged_arr*,int);int Cyc_Std_system(struct _tagged_arr);double atof(char*);double
Cyc_Std_atof(struct _tagged_arr x){return atof(string_to_Cstring(x));}int atoi(char*);
int Cyc_Std_atoi(struct _tagged_arr x){return atoi(string_to_Cstring(x));}int atol(
char*);int Cyc_Std_atol(struct _tagged_arr x){return atol(string_to_Cstring(x));}
void Cyc_Std_free(struct _tagged_arr ptr){;}char*getenv(char*);struct _tagged_arr Cyc_Std_getenv(
struct _tagged_arr name){return Cstring_to_string(getenv(string_to_Cstring(name)));}
int putenv(char*);int Cyc_Std_putenv(struct _tagged_arr s){return putenv(
string_to_Cstring((struct _tagged_arr)s));}void qsort(void*,unsigned int,
unsigned int,int(*)(const void*,const void*));static char _tmp3E[23]="qsort called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___qsort_failure={Cyc_Core_Failure,{
_tmp3E,_tmp3E,_tmp3E + 23}};void Cyc_Std_qsort(struct _tagged_arr tab,unsigned int
nmemb,unsigned int szmemb,int(*compar)(const void*,const void*)){if(tab.curr == (
_tag_arr(0,0,0)).curr?1: _get_arr_size(tab,sizeof(void))< nmemb)(int)_throw((void*)&
Cyc_Std___qsort_failure);qsort((void*)_untag_arr(tab,sizeof(void),1),nmemb,(
unsigned int)szmemb,compar);}static char _tmp40[32]="strto* called with NULL pointer";
static struct Cyc_Core_Failure_struct Cyc_Std___strto_failure_1={Cyc_Core_Failure,{
_tmp40,_tmp40,_tmp40 + 32}};static char _tmp42[32]="strto* called with bad C string";
static struct Cyc_Core_Failure_struct Cyc_Std___strto_failure_2={Cyc_Core_Failure,{
_tmp42,_tmp42,_tmp42 + 32}};static void Cyc_Std_check_valid_cstring(struct
_tagged_arr s){if(!((unsigned int)s.curr))(int)_throw((void*)& Cyc_Std___strto_failure_1);{
int found_zero=0;{int i=(int)(_get_arr_size(s,sizeof(char))- 1);for(0;i >= 0;i --){
if(*((const char*)_check_unknown_subscript(s,sizeof(char),i))== '\000'){
found_zero=1;break;}}}if(!found_zero)(int)_throw((void*)& Cyc_Std___strto_failure_2);}}
double strtod(char*,char**);double Cyc_Std_strtod(struct _tagged_arr nptr,struct
_tagged_arr*endptr){Cyc_Std_check_valid_cstring(nptr);{char*c=underlying_Cstring(
nptr);char*e=endptr == 0?0: c;double d=strtod(c,(char**)& e);if(endptr != 0){int n=(
int)((unsigned int)e - (unsigned int)c);*endptr=_tagged_arr_plus(nptr,sizeof(char),
n);}return d;}}int strtol(char*,char**,int);int Cyc_Std_strtol(struct _tagged_arr n,
struct _tagged_arr*endptr,int base){Cyc_Std_check_valid_cstring(n);{char*c=
underlying_Cstring(n);char*e=endptr == 0?0: c;int r=strtol(c,(char**)& e,base);if(
endptr != 0){int m=(int)((unsigned int)e - (unsigned int)c);*endptr=
_tagged_arr_plus(n,sizeof(char),m);}return r;}}unsigned int strtoul(char*,char**,
int);unsigned int Cyc_Std_strtoul(struct _tagged_arr n,struct _tagged_arr*endptr,int
base){Cyc_Std_check_valid_cstring(n);{char*c=underlying_Cstring(n);char*e=endptr
== 0?0: c;unsigned int r=strtoul(c,(char**)& e,base);if(endptr != 0){int m=(int)((
unsigned int)e - (unsigned int)c);*endptr=_tagged_arr_plus(n,sizeof(char),m);}
return r;}}int system(char*);int Cyc_Std_system(struct _tagged_arr x){return system(
string_to_Cstring(x));}struct Cyc_Std_timeval{int tv_sec;int tv_usec;};struct Cyc_Std_rlimit{
unsigned int rlim_cur;unsigned int rlim_max;};struct Cyc_Std_rusage{struct Cyc_Std_timeval
ru_utime;struct Cyc_Std_timeval ru_stime;int ru_maxrss;int ru_ixrss;int ru_idrss;int
ru_isrss;int ru_minflt;int ru_majflt;int ru_nswap;int ru_inblock;int ru_oublock;int
ru_msgsnd;int ru_msgrcv;int ru_nsignals;int ru_nvcsw;int ru_nivcsw;};struct Cyc_Std_timespec{
int tv_sec;int tv_nsec;};struct Cyc_Std__types_fd_set{int fds_bits[2];};int select(
int,struct Cyc_Std__types_fd_set*,struct Cyc_Std__types_fd_set*,struct Cyc_Std__types_fd_set*,
struct Cyc_Std_timeval*);void Cyc_Std_FD_CLR(int,struct Cyc_Std__types_fd_set*);int
Cyc_Std_FD_ISSET(int,struct Cyc_Std__types_fd_set*);void Cyc_Std_FD_SET(int,struct
Cyc_Std__types_fd_set*);void Cyc_Std_FD_ZERO(struct Cyc_Std__types_fd_set*);void
__stub_FD_CLR(int,struct Cyc_Std__types_fd_set*);int __stub_FD_ISSET(int,struct Cyc_Std__types_fd_set*);
void __stub_FD_SET(int,struct Cyc_Std__types_fd_set*);void __stub_FD_ZERO(struct Cyc_Std__types_fd_set*);
void Cyc_Std_FD_CLR(int a,struct Cyc_Std__types_fd_set*b){return __stub_FD_CLR(a,b);}
int Cyc_Std_FD_ISSET(int a,struct Cyc_Std__types_fd_set*b){return __stub_FD_ISSET(a,
b);}void Cyc_Std_FD_SET(int a,struct Cyc_Std__types_fd_set*b){return __stub_FD_SET(a,
b);}void Cyc_Std_FD_ZERO(struct Cyc_Std__types_fd_set*a){return __stub_FD_ZERO(a);}
struct Cyc_Std_linger{unsigned short l_onoff;unsigned short l_linger;};struct Cyc_Std_SA_sockaddr_in_struct{
int tag;struct Cyc_Std_sockaddr_in*f1;};struct Cyc_Std_SA_socklenptr_struct{int tag;
int*f1;};struct Cyc_Std_SA_socklen_struct{int tag;int f1;};struct Cyc_Std_SO_int_struct{
int tag;int*f1;};struct Cyc_Std_SO_timeval_struct{int tag;struct Cyc_Std_timeval*f1;
};struct Cyc_Std_SO_socklenptr_struct{int tag;int*f1;};struct Cyc_Std_SO_socklen_struct{
int tag;int f1;};int Cyc_Std_accept(int fd,struct _tagged_arr);int Cyc_Std_bind(int fd,
struct _tagged_arr);int Cyc_Std_connect(int fd,struct _tagged_arr);int Cyc_Std_getpeername(
int fd,struct _tagged_arr);int Cyc_Std_getsockname(int fd,struct _tagged_arr);int Cyc_Std_getsockopt(
int fd,int level,int optname,struct _tagged_arr);int Cyc_Std_recv(int fd,struct
_tagged_arr buf,unsigned int n,int flags);int Cyc_Std_recvfrom(int fd,struct
_tagged_arr buf,unsigned int n,int flags,struct _tagged_arr);int Cyc_Std_send(int fd,
struct _tagged_arr buf,unsigned int n,int flags);int Cyc_Std_sendto(int fd,struct
_tagged_arr buf,unsigned int n,int flags,struct _tagged_arr);int Cyc_Std_setsockopt(
int fd,int level,int optname,struct _tagged_arr);char Cyc_Std_SocketError[16]="\000\000\000\000SocketError";
int accept_in(int,const struct Cyc_Std_sockaddr_in*,int*);struct _tuple0{void*f1;
void*f2;};int Cyc_Std_accept(int fd,struct _tagged_arr ap){if(_get_arr_size(ap,
sizeof(void*))!= 2)(int)_throw((void*)Cyc_Std_SocketError);{struct _tuple0 _tmp44=({
struct _tuple0 _tmp43;_tmp43.f1=*((void**)_check_unknown_subscript(ap,sizeof(void*),
0));_tmp43.f2=*((void**)_check_unknown_subscript(ap,sizeof(void*),1));_tmp43;});
void*_tmp45;struct Cyc_Std_sockaddr_in*_tmp46;void*_tmp47;int*_tmp48;_LLB: _tmp45=
_tmp44.f1;if(*((int*)_tmp45)!= 0)goto _LLD;_tmp46=((struct Cyc_Std_SA_sockaddr_in_struct*)
_tmp45)->f1;_tmp47=_tmp44.f2;if(*((int*)_tmp47)!= 1)goto _LLD;_tmp48=((struct Cyc_Std_SA_socklenptr_struct*)
_tmp47)->f1;_LLC: return accept_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmp46,
_tmp48);_LLD:;_LLE:(int)_throw((void*)Cyc_Std_SocketError);_LLA:;}}int bind_in(
int,const struct Cyc_Std_sockaddr_in*,int);int Cyc_Std_bind(int fd,struct _tagged_arr
ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_Std_SocketError);{
struct _tuple0 _tmp4A=({struct _tuple0 _tmp49;_tmp49.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp49.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp49;});void*_tmp4B;struct Cyc_Std_sockaddr_in*
_tmp4C;void*_tmp4D;int _tmp4E;_LL10: _tmp4B=_tmp4A.f1;if(*((int*)_tmp4B)!= 0)goto
_LL12;_tmp4C=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp4B)->f1;_tmp4D=_tmp4A.f2;
if(*((int*)_tmp4D)!= 2)goto _LL12;_tmp4E=((struct Cyc_Std_SA_socklen_struct*)
_tmp4D)->f1;_LL11: return bind_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmp4C,
_tmp4E);_LL12:;_LL13:(int)_throw((void*)Cyc_Std_SocketError);_LLF:;}}int
connect_in(int,const struct Cyc_Std_sockaddr_in*,int);int Cyc_Std_connect(int fd,
struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)
Cyc_Std_SocketError);{struct _tuple0 _tmp50=({struct _tuple0 _tmp4F;_tmp4F.f1=*((
void**)_check_unknown_subscript(ap,sizeof(void*),0));_tmp4F.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp4F;});void*_tmp51;struct Cyc_Std_sockaddr_in*
_tmp52;void*_tmp53;int _tmp54;_LL15: _tmp51=_tmp50.f1;if(*((int*)_tmp51)!= 0)goto
_LL17;_tmp52=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp51)->f1;_tmp53=_tmp50.f2;
if(*((int*)_tmp53)!= 2)goto _LL17;_tmp54=((struct Cyc_Std_SA_socklen_struct*)
_tmp53)->f1;_LL16: return connect_in(fd,(const struct Cyc_Std_sockaddr_in*)_tmp52,
_tmp54);_LL17:;_LL18:(int)_throw((void*)Cyc_Std_SocketError);_LL14:;}}int
getpeername_in(int,const struct Cyc_Std_sockaddr_in*,int*);int Cyc_Std_getpeername(
int fd,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_Std_SocketError);{struct _tuple0 _tmp56=({struct _tuple0 _tmp55;_tmp55.f1=*((
void**)_check_unknown_subscript(ap,sizeof(void*),0));_tmp55.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp55;});void*_tmp57;struct Cyc_Std_sockaddr_in*
_tmp58;void*_tmp59;int*_tmp5A;_LL1A: _tmp57=_tmp56.f1;if(*((int*)_tmp57)!= 0)goto
_LL1C;_tmp58=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp57)->f1;_tmp59=_tmp56.f2;
if(*((int*)_tmp59)!= 1)goto _LL1C;_tmp5A=((struct Cyc_Std_SA_socklenptr_struct*)
_tmp59)->f1;_LL1B: return getpeername_in(fd,(const struct Cyc_Std_sockaddr_in*)
_tmp58,_tmp5A);_LL1C:;_LL1D:(int)_throw((void*)Cyc_Std_SocketError);_LL19:;}}int
getsockname_in(int,const struct Cyc_Std_sockaddr_in*,int*);int Cyc_Std_getsockname(
int fd,struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_Std_SocketError);{struct _tuple0 _tmp5C=({struct _tuple0 _tmp5B;_tmp5B.f1=*((
void**)_check_unknown_subscript(ap,sizeof(void*),0));_tmp5B.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp5B;});void*_tmp5D;struct Cyc_Std_sockaddr_in*
_tmp5E;void*_tmp5F;int*_tmp60;_LL1F: _tmp5D=_tmp5C.f1;if(*((int*)_tmp5D)!= 0)goto
_LL21;_tmp5E=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp5D)->f1;_tmp5F=_tmp5C.f2;
if(*((int*)_tmp5F)!= 1)goto _LL21;_tmp60=((struct Cyc_Std_SA_socklenptr_struct*)
_tmp5F)->f1;_LL20: return getsockname_in(fd,(const struct Cyc_Std_sockaddr_in*)
_tmp5E,_tmp60);_LL21:;_LL22:(int)_throw((void*)Cyc_Std_SocketError);_LL1E:;}}int
getsockopt_int(int,int,int,int*,int*);int getsockopt_timeval(int,int,int,struct
Cyc_Std_timeval*,int*);int Cyc_Std_getsockopt(int fd,int level,int optname,struct
_tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_Std_SocketError);{
struct _tuple0 _tmp62=({struct _tuple0 _tmp61;_tmp61.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp61.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp61;});void*_tmp63;int*_tmp64;
void*_tmp65;int*_tmp66;void*_tmp67;int*_tmp68;void*_tmp69;int*_tmp6A;void*_tmp6B;
struct Cyc_Std_timeval*_tmp6C;void*_tmp6D;int*_tmp6E;void*_tmp6F;struct Cyc_Std_timeval*
_tmp70;void*_tmp71;int*_tmp72;_LL24: _tmp63=_tmp62.f1;if(*((int*)_tmp63)!= 0)goto
_LL26;_tmp64=((struct Cyc_Std_SO_int_struct*)_tmp63)->f1;_tmp65=_tmp62.f2;if(*((
int*)_tmp65)!= 2)goto _LL26;_tmp66=((struct Cyc_Std_SO_socklenptr_struct*)_tmp65)->f1;
_LL25: return getsockopt_int(fd,level,optname,_tmp64,_tmp66);_LL26: _tmp67=_tmp62.f1;
if(*((int*)_tmp67)!= 0)goto _LL28;_tmp68=((struct Cyc_Std_SO_int_struct*)_tmp67)->f1;
_tmp69=_tmp62.f2;if(*((int*)_tmp69)!= 0)goto _LL28;_tmp6A=((struct Cyc_Std_SO_int_struct*)
_tmp69)->f1;_LL27: return getsockopt_int(fd,level,optname,_tmp68,(int*)_tmp6A);
_LL28: _tmp6B=_tmp62.f1;if(*((int*)_tmp6B)!= 1)goto _LL2A;_tmp6C=((struct Cyc_Std_SO_timeval_struct*)
_tmp6B)->f1;_tmp6D=_tmp62.f2;if(*((int*)_tmp6D)!= 2)goto _LL2A;_tmp6E=((struct Cyc_Std_SO_socklenptr_struct*)
_tmp6D)->f1;_LL29: return getsockopt_timeval(fd,level,optname,_tmp6C,_tmp6E);_LL2A:
_tmp6F=_tmp62.f1;if(*((int*)_tmp6F)!= 1)goto _LL2C;_tmp70=((struct Cyc_Std_SO_timeval_struct*)
_tmp6F)->f1;_tmp71=_tmp62.f2;if(*((int*)_tmp71)!= 0)goto _LL2C;_tmp72=((struct Cyc_Std_SO_int_struct*)
_tmp71)->f1;_LL2B: return getsockopt_timeval(fd,level,optname,_tmp70,(int*)_tmp72);
_LL2C:;_LL2D:(int)_throw((void*)Cyc_Std_SocketError);_LL23:;}}int recv_wrapped(
int,struct _tagged_arr,unsigned int,int);int Cyc_Std_recv(int fd,struct _tagged_arr
buf,unsigned int n,int flags){return recv_wrapped(fd,buf,n,flags);}int recvfrom_in(
int,struct _tagged_arr,unsigned int,int,const struct Cyc_Std_sockaddr_in*,int*);int
Cyc_Std_recvfrom(int fd,struct _tagged_arr buf,unsigned int n,int flags,struct
_tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_Std_SocketError);{
struct _tuple0 _tmp74=({struct _tuple0 _tmp73;_tmp73.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp73.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp73;});void*_tmp75;struct Cyc_Std_sockaddr_in*
_tmp76;void*_tmp77;int*_tmp78;_LL2F: _tmp75=_tmp74.f1;if(*((int*)_tmp75)!= 0)goto
_LL31;_tmp76=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp75)->f1;_tmp77=_tmp74.f2;
if(*((int*)_tmp77)!= 1)goto _LL31;_tmp78=((struct Cyc_Std_SA_socklenptr_struct*)
_tmp77)->f1;_LL30: return recvfrom_in(fd,buf,n,flags,(const struct Cyc_Std_sockaddr_in*)
_tmp76,_tmp78);_LL31:;_LL32:(int)_throw((void*)Cyc_Std_SocketError);_LL2E:;}}int
send_wrapped(int,struct _tagged_arr,unsigned int,int);int Cyc_Std_send(int fd,
struct _tagged_arr buf,unsigned int n,int flags){return send_wrapped(fd,buf,n,flags);}
int sendto_in(int,struct _tagged_arr,unsigned int,int,const struct Cyc_Std_sockaddr_in*,
int);int Cyc_Std_sendto(int fd,struct _tagged_arr buf,unsigned int n,int flags,struct
_tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_Std_SocketError);{
struct _tuple0 _tmp7A=({struct _tuple0 _tmp79;_tmp79.f1=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));_tmp79.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp79;});void*_tmp7B;struct Cyc_Std_sockaddr_in*
_tmp7C;void*_tmp7D;int _tmp7E;_LL34: _tmp7B=_tmp7A.f1;if(*((int*)_tmp7B)!= 0)goto
_LL36;_tmp7C=((struct Cyc_Std_SA_sockaddr_in_struct*)_tmp7B)->f1;_tmp7D=_tmp7A.f2;
if(*((int*)_tmp7D)!= 2)goto _LL36;_tmp7E=((struct Cyc_Std_SA_socklen_struct*)
_tmp7D)->f1;_LL35: return sendto_in(fd,buf,n,flags,(const struct Cyc_Std_sockaddr_in*)
_tmp7C,_tmp7E);_LL36:;_LL37:(int)_throw((void*)Cyc_Std_SocketError);_LL33:;}}int
setsockopt_int(int,int,int,const int*,int);int setsockopt_timeval(int,int,int,
const struct Cyc_Std_timeval*,int);int Cyc_Std_setsockopt(int fd,int level,int optname,
struct _tagged_arr ap){if(_get_arr_size(ap,sizeof(void*))!= 2)(int)_throw((void*)
Cyc_Std_SocketError);{struct _tuple0 _tmp80=({struct _tuple0 _tmp7F;_tmp7F.f1=*((
void**)_check_unknown_subscript(ap,sizeof(void*),0));_tmp7F.f2=*((void**)
_check_unknown_subscript(ap,sizeof(void*),1));_tmp7F;});void*_tmp81;int*_tmp82;
void*_tmp83;int _tmp84;void*_tmp85;struct Cyc_Std_timeval*_tmp86;void*_tmp87;int
_tmp88;_LL39: _tmp81=_tmp80.f1;if(*((int*)_tmp81)!= 0)goto _LL3B;_tmp82=((struct
Cyc_Std_SO_int_struct*)_tmp81)->f1;_tmp83=_tmp80.f2;if(*((int*)_tmp83)!= 3)goto
_LL3B;_tmp84=((struct Cyc_Std_SO_socklen_struct*)_tmp83)->f1;_LL3A: return
setsockopt_int(fd,level,optname,(const int*)_tmp82,_tmp84);_LL3B: _tmp85=_tmp80.f1;
if(*((int*)_tmp85)!= 1)goto _LL3D;_tmp86=((struct Cyc_Std_SO_timeval_struct*)
_tmp85)->f1;_tmp87=_tmp80.f2;if(*((int*)_tmp87)!= 3)goto _LL3D;_tmp88=((struct Cyc_Std_SO_socklen_struct*)
_tmp87)->f1;_LL3C: return setsockopt_timeval(fd,level,optname,(const struct Cyc_Std_timeval*)
_tmp86,_tmp88);_LL3D:;_LL3E:(int)_throw((void*)Cyc_Std_SocketError);_LL38:;}}
struct Cyc_Std_stat{short st_dev;unsigned int st_ino;int st_mode;unsigned short
st_nlink;unsigned short st_uid;unsigned short st_gid;short st_rdev;int st_size;int
st_atime;int st_spare1;int st_mtime;int st_spare2;int st_ctime;int st_spare3;int
st_blksize;int st_blocks;int st_spare4[2];};int Cyc_Std_chmod(struct _tagged_arr,int);
int Cyc_Std_lstat(struct _tagged_arr filename,struct Cyc_Std_stat*buf);int Cyc_Std_mkdir(
struct _tagged_arr pathname,int mode);int Cyc_Std_mkfifo(struct _tagged_arr pathname,
int mode);int Cyc_Std_mknod(struct _tagged_arr pathname,int mode,short);int Cyc_Std_stat(
struct _tagged_arr,struct Cyc_Std_stat*);int chmod(char*,int);static char _tmp8A[23]="chmod called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___chmod_failure={Cyc_Core_Failure,{
_tmp8A,_tmp8A,_tmp8A + 23}};int Cyc_Std_chmod(struct _tagged_arr a,int b){if(!((
unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___chmod_failure);return chmod(
string_to_Cstring(a),b);}int lstat(char*,struct Cyc_Std_stat*);static char _tmp8C[23]="lstat called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___lstat_failure={Cyc_Core_Failure,{
_tmp8C,_tmp8C,_tmp8C + 23}};int Cyc_Std_lstat(struct _tagged_arr a,struct Cyc_Std_stat*
b){if(!((unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___lstat_failure);
return lstat(string_to_Cstring(a),b);}int mkdir(char*,int);static char _tmp8E[23]="mkdir called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___mkdir_failure={Cyc_Core_Failure,{
_tmp8E,_tmp8E,_tmp8E + 23}};int Cyc_Std_mkdir(struct _tagged_arr a,int b){if(!((
unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___mkdir_failure);return mkdir(
string_to_Cstring(a),b);}int mkfifo(char*,int);static char _tmp90[24]="mkfifo called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___mkfifo_failure={Cyc_Core_Failure,{
_tmp90,_tmp90,_tmp90 + 24}};int Cyc_Std_mkfifo(struct _tagged_arr a,int b){if(!((
unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___mkfifo_failure);return mkfifo(
string_to_Cstring(a),b);}int mknod(char*,int,short);static char _tmp92[23]="mknod called with NULL";
static struct Cyc_Core_Failure_struct Cyc_Std___mknod_failure={Cyc_Core_Failure,{
_tmp92,_tmp92,_tmp92 + 23}};int Cyc_Std_mknod(struct _tagged_arr a,int b,short c){if(!((
unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___mknod_failure);return mknod(
string_to_Cstring(a),b,c);}int stat(char*,struct Cyc_Std_stat*);static char _tmp94[
22]="stat called with NULL";static struct Cyc_Core_Failure_struct Cyc_Std___stat_failure={
Cyc_Core_Failure,{_tmp94,_tmp94,_tmp94 + 22}};int Cyc_Std_stat(struct _tagged_arr a,
struct Cyc_Std_stat*b){if(!((unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___stat_failure);
return stat(string_to_Cstring(a),b);}struct Cyc_Std_timezone{int tz_minuteswest;int
tz_dsttime;};struct Cyc_Std_itimerval{struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval
it_value;};int Cyc_Std_gettimeofday(struct Cyc_Std_timeval*,struct Cyc_Std_timezone*);
int select(int,struct Cyc_Std__types_fd_set*,struct Cyc_Std__types_fd_set*,struct
Cyc_Std__types_fd_set*,struct Cyc_Std_timeval*);void Cyc_Std_FD_CLR(int,struct Cyc_Std__types_fd_set*);
int Cyc_Std_FD_ISSET(int,struct Cyc_Std__types_fd_set*);void Cyc_Std_FD_SET(int,
struct Cyc_Std__types_fd_set*);void Cyc_Std_FD_ZERO(struct Cyc_Std__types_fd_set*);
int Cyc_Std_utimes(struct _tagged_arr,const struct Cyc_Std_timeval*);int gettimeofday(
struct Cyc_Std_timeval*,struct Cyc_Std_timezone*);int Cyc_Std_gettimeofday(struct
Cyc_Std_timeval*a,struct Cyc_Std_timezone*b){return gettimeofday(a,0);}static char
_tmp96[24]="utimes called with NULL";static struct Cyc_Core_Failure_struct Cyc_Std___utimes_failure={
Cyc_Core_Failure,{_tmp96,_tmp96,_tmp96 + 24}};int Cyc_Std_utimes(struct _tagged_arr
a,const struct Cyc_Std_timeval*b){if(!((unsigned int)a.curr))(int)_throw((void*)&
Cyc_Std___utimes_failure);return Cyc_Std_utimes(_tag_arr(string_to_Cstring(a),
sizeof(char),0),b);}struct Cyc_Std_timeb{int time;unsigned short millitm;short
timezone;short dstflag;};struct Cyc_Std_tms{unsigned int tms_utime;unsigned int
tms_stime;unsigned int tms_cutime;unsigned int tms_cstime;};struct Cyc_Std_sockaddr_un{
unsigned short sun_family;char sun_path[108];};struct Cyc_Std_utsname{char sysname[
20];char nodename[20];char release[20];char version[20];char machine[20];};struct Cyc_Std_itimerspec{
struct Cyc_Std_timespec it_interval;struct Cyc_Std_timespec it_value;};extern int
timezone;int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(struct
_tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned short,unsigned short);
int Cyc_Std_chroot(struct _tagged_arr);int Cyc_Std_execlp(struct _tagged_arr,struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_execve(struct _tagged_arr,struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr
buf,unsigned int size);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);
struct _tagged_arr Cyc_Std_getpass(struct _tagged_arr);int Cyc_Std_link(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_read(int,struct _tagged_arr,
unsigned int);int Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_unlink(
struct _tagged_arr);int Cyc_Std_write(int,struct _tagged_arr,unsigned int);int
access(char*,int);int Cyc_Std_access(struct _tagged_arr path,int mode){return access(
string_to_Cstring(path),mode);}int chdir(char*);int Cyc_Std_chdir(struct
_tagged_arr path){return chdir(string_to_Cstring(path));}int chown(char*,
unsigned short,unsigned short);int Cyc_Std_chown(struct _tagged_arr path,
unsigned short owner,unsigned short group){return chown(string_to_Cstring(path),
owner,group);}int chroot(char*);int Cyc_Std_chroot(struct _tagged_arr pathname){
return chroot(string_to_Cstring(pathname));}int execvp(char*file,char**argv);int
Cyc_Std_execlp(struct _tagged_arr path,struct _tagged_arr arg0,struct _tagged_arr argv){
if((*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(
int)(_get_arr_size(argv,sizeof(struct _tagged_arr))- 1)))).curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Core_Failure_struct
_tmp98;_tmp98.tag=Cyc_Core_Failure;_tmp98.f1=_tag_arr("execl: arg list must be NULL-terminated",
sizeof(char),40);_tmp98;});_tmp97;}));{struct _tagged_arr newargs=({unsigned int
_tmp99=1 + _get_arr_size(argv,sizeof(struct _tagged_arr));char**_tmp9A=(char**)
_cycalloc(_check_times(sizeof(char*),_tmp99));struct _tagged_arr _tmp9C=_tag_arr(
_tmp9A,sizeof(char*),1 + _get_arr_size(argv,sizeof(struct _tagged_arr)));{
unsigned int _tmp9B=_tmp99;unsigned int i;for(i=0;i < _tmp9B;i ++){_tmp9A[i]=0;}}
_tmp9C;});*((char**)_check_unknown_subscript(newargs,sizeof(char*),0))=
string_to_Cstring(arg0);{int i=0;for(0;i < _get_arr_size(argv,sizeof(struct
_tagged_arr));i ++){*((char**)_check_unknown_subscript(newargs,sizeof(char*),i + 1))=
string_to_Cstring(((struct _tagged_arr*)argv.curr)[i]);}}return execvp(
string_to_Cstring(path),(char**)_check_null(_untag_arr(newargs,sizeof(char*),1)));}}
int execve(char*path,char**argv,char**envp);static char _tmp9E[41]="execve: arg list must be NULL-terminated";
static struct Cyc_Core_Failure_struct Cyc_Std___execve_failure={Cyc_Core_Failure,{
_tmp9E,_tmp9E,_tmp9E + 41}};int Cyc_Std_execve(struct _tagged_arr filename,struct
_tagged_arr argv,struct _tagged_arr envp){if((*((const struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(int)(_get_arr_size(argv,
sizeof(struct _tagged_arr))- 1)))).curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)(
int)_throw((void*)& Cyc_Std___execve_failure);{struct _tagged_arr newargs=({
unsigned int _tmpA3=_get_arr_size(argv,sizeof(struct _tagged_arr));char**_tmpA4=(
char**)_cycalloc(_check_times(sizeof(char*),_tmpA3));struct _tagged_arr _tmpA6=
_tag_arr(_tmpA4,sizeof(char*),_get_arr_size(argv,sizeof(struct _tagged_arr)));{
unsigned int _tmpA5=_tmpA3;unsigned int i;for(i=0;i < _tmpA5;i ++){_tmpA4[i]=0;}}
_tmpA6;});{int i=0;for(0;i < _get_arr_size(argv,sizeof(struct _tagged_arr));i ++){*((
char**)_check_unknown_subscript(newargs,sizeof(char*),i))=string_to_Cstring(((
const struct _tagged_arr*)argv.curr)[i]);}}{struct _tagged_arr newenvp=({
unsigned int _tmp9F=_get_arr_size(envp,sizeof(struct _tagged_arr));char**_tmpA0=(
char**)_cycalloc(_check_times(sizeof(char*),_tmp9F));struct _tagged_arr _tmpA2=
_tag_arr(_tmpA0,sizeof(char*),_get_arr_size(envp,sizeof(struct _tagged_arr)));{
unsigned int _tmpA1=_tmp9F;unsigned int i;for(i=0;i < _tmpA1;i ++){_tmpA0[i]=0;}}
_tmpA2;});{int i=0;for(0;i < _get_arr_size(envp,sizeof(struct _tagged_arr));i ++){*((
char**)_check_unknown_subscript(newenvp,sizeof(char*),i))=string_to_Cstring(((
const struct _tagged_arr*)envp.curr)[i]);}}return execve(string_to_Cstring(filename),(
char**)_check_null(_untag_arr(newargs,sizeof(char*),1)),(char**)_check_null(
_untag_arr(newenvp,sizeof(char*),1)));}}}char*getcwd(char*buf,unsigned int size);
static char _tmpA8[29]="getcwd: invalid buf argument";static struct Cyc_Core_Failure_struct
Cyc_Std___getcwd_failure={Cyc_Core_Failure,{_tmpA8,_tmpA8,_tmpA8 + 29}};struct
_tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int size){if(!((
unsigned int)buf.curr)?1: _get_arr_size(buf,sizeof(char))< size)(int)_throw((void*)&
Cyc_Std___getcwd_failure);{char*response=getcwd((char*)_check_null(_untag_arr(
buf,sizeof(char),0)),size);return(unsigned int)response?buf: _tag_arr(0,0,0);}}
int gethostname(char*,unsigned int);static char _tmpAA[42]="gethostname: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc_Std___gethostname_failure={Cyc_Core_Failure,{
_tmpAA,_tmpAA,_tmpAA + 42}};int Cyc_Std_gethostname(struct _tagged_arr buf,
unsigned int count){if(count > _get_arr_size(buf,sizeof(char)))(int)_throw((void*)&
Cyc_Std___gethostname_failure);return gethostname(underlying_Cstring((struct
_tagged_arr)buf),count);}char*getpass(char*);struct _tagged_arr Cyc_Std_getpass(
struct _tagged_arr prompt){return wrap_Cstring_as_string(getpass(string_to_Cstring(
prompt)),- 1);}int link(char*path1,char*path2);int Cyc_Std_link(struct _tagged_arr
path1,struct _tagged_arr path2){return link(string_to_Cstring(path1),
string_to_Cstring(path2));}int read(int fd,char*buf,unsigned int count);static char
_tmpAC[35]="read: called with count > buf.size";static struct Cyc_Core_Failure_struct
Cyc_Std___read_failure={Cyc_Core_Failure,{_tmpAC,_tmpAC,_tmpAC + 35}};int Cyc_Std_read(
int fd,struct _tagged_arr buf,unsigned int count){if(count > _get_arr_size(buf,
sizeof(char)))(int)_throw((void*)& Cyc_Std___read_failure);return read(fd,
underlying_Cstring((struct _tagged_arr)buf),count);}int rmdir(char*);int Cyc_Std_rmdir(
struct _tagged_arr path){return rmdir(string_to_Cstring(path));}int symlink(char*
path1,char*path2);int Cyc_Std_symlink(struct _tagged_arr path1,struct _tagged_arr
path2){return symlink(string_to_Cstring(path1),string_to_Cstring(path2));}int
truncate(char*,int);int Cyc_Std_truncate(struct _tagged_arr path,int length){return
truncate(string_to_Cstring(path),length);}int unlink(char*pathname);int Cyc_Std_unlink(
struct _tagged_arr pathname){return unlink(string_to_Cstring(pathname));}int write(
int fd,char*buf,unsigned int count);static char _tmpAE[36]="write: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc_Std___write_failure={Cyc_Core_Failure,{
_tmpAE,_tmpAE,_tmpAE + 36}};int Cyc_Std_write(int fd,struct _tagged_arr buf,
unsigned int count){if(count > _get_arr_size(buf,sizeof(char)))(int)_throw((void*)&
Cyc_Std___write_failure);return write(fd,underlying_Cstring(buf),count);}struct
Cyc_Std_utimbuf{int actime;int modtime;};int Cyc_Std_utime(struct _tagged_arr,const
struct Cyc_Std_utimbuf*);static char _tmpB0[23]="utime called with NULL";static
struct Cyc_Core_Failure_struct Cyc_Std___utime_failure={Cyc_Core_Failure,{_tmpB0,
_tmpB0,_tmpB0 + 23}};int Cyc_Std_utime(struct _tagged_arr a,const struct Cyc_Std_utimbuf*
b){if(!((unsigned int)a.curr))(int)_throw((void*)& Cyc_Std___utime_failure);
return Cyc_Std_utime(_tag_arr(string_to_Cstring(a),sizeof(char),0),b);}
