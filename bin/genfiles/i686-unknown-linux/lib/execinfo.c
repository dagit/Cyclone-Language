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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};int Cyc_Execinfo_backtrace(
struct _tagged_arr,int);int Cyc_Execinfo_bt();struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;int Cyc_Std_fclose(
struct Cyc_Std___cycFILE*);int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);int Cyc_Std_getc(
struct Cyc_Std___cycFILE*__stream);int Cyc_Std_putc(int __c,struct Cyc_Std___cycFILE*
__stream);struct Cyc_Std___cycFILE*Cyc_Std_fdopen(int __fd,struct _tagged_arr
__modes);extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];
struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Std_timeval{
int tv_sec;int tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};
struct Cyc_Std_itimerval{struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval
it_value;};typedef struct{unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t;struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;
struct Cyc_Std___fd_set{int __fds_bits[32];};struct Cyc_Std_rlimit{unsigned int
rlim_cur;unsigned int rlim_max;};struct Cyc_Std_rusage{struct Cyc_Std_timeval
ru_utime;struct Cyc_Std_timeval ru_stime;int ru_maxrss;int ru_ixrss;int ru_idrss;int
ru_isrss;int ru_minflt;int ru_majflt;int ru_nswap;int ru_inblock;int ru_oublock;int
ru_msgsnd;int ru_msgrcv;int ru_nsignals;int ru_nvcsw;int ru_nivcsw;};int waitpid(int
pid,int*status,int options);void(*Cyc_Std_signal(int sig,void(*func)(int)))(int);
int kill(int,int);struct Cyc_Std_option{struct _tagged_arr name;int has_arg;int*flag;
int val;};int close(int);int getpid();int fork();int dup2(int,int);int pipe(int*
filedes);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(struct
_tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned int,unsigned int);
struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,unsigned int size);int Cyc_Std_execl(
struct _tagged_arr path,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execlp(
struct _tagged_arr file,struct _tagged_arr arg0,struct _tagged_arr argv);int Cyc_Std_execve(
struct _tagged_arr filename,struct _tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,
unsigned int count);int Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(
int fd,struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct
_tagged_arr pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int
Cyc_Std_chroot(struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct
_tagged_arr prompt);int backtrace(int*,int);int Cyc_Execinfo_backtrace(struct
_tagged_arr array,int size){if(size > _get_arr_size(array,sizeof(int)))(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Core_Failure_struct _tmp1;_tmp1.tag=Cyc_Core_Failure;_tmp1.f1=_tag_arr("backtrace: size > array.size",
sizeof(char),29);_tmp1;});_tmp0;}));return backtrace((int*)_check_null(_untag_arr(
array,sizeof(int),1)),size);}int Cyc_Execinfo_bt(){int bt[20];{unsigned int _tmpA=
20;unsigned int i;for(i=0;i < _tmpA;i ++){bt[i]=0;}}{int tochild[2]={0,0};int
fromchild[2]={0,0};int pid;int self_pid;if(pipe(tochild)?1: pipe(fromchild))return 1;
self_pid=getpid();if((pid=fork())== 0){if(dup2(tochild[0],0)== - 1?1: dup2(
fromchild[1],1)== - 1)return 1;({struct _tagged_arr _tmp2[4];_tmp2[3]=(struct
_tagged_arr)_tag_arr(0,0,0);_tmp2[2]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp4;_tmp4.tag=1;_tmp4.f1=(int)((unsigned int)self_pid);{void*_tmp3[1]={& _tmp4};
Cyc_Std_aprintf(_tag_arr("/proc/%d/exe",sizeof(char),13),_tag_arr(_tmp3,sizeof(
void*),1));}});_tmp2[1]=_tag_arr("-e",sizeof(char),3);_tmp2[0]=_tag_arr("--functions",
sizeof(char),12);Cyc_Std_execlp(_tag_arr("addr2line",sizeof(char),10),_tag_arr("addr2line",
sizeof(char),10),_tag_arr(_tmp2,sizeof(struct _tagged_arr),4));});return 1;}else{
if(pid < 0){close(tochild[0]);close(tochild[1]);close(fromchild[0]);close(
fromchild[1]);return 1;}}{struct Cyc_Std___cycFILE*w=({struct Cyc_Std___cycFILE*f=
Cyc_Std_fdopen(tochild[1],_tag_arr("w",sizeof(char),2));if(!((unsigned int)f))
return 1;(struct Cyc_Std___cycFILE*)_check_null(f);});struct Cyc_Std___cycFILE*r=({
struct Cyc_Std___cycFILE*f=Cyc_Std_fdopen(fromchild[0],_tag_arr("r",sizeof(char),
2));if(!((unsigned int)f)){Cyc_Std_fclose(w);return 1;}(struct Cyc_Std___cycFILE*)
_check_null(f);});int n=Cyc_Execinfo_backtrace(_tag_arr(bt,sizeof(int),20),(int)
20);{int c=0;for(0;c < n;c ++){({struct Cyc_Std_Int_pa_struct _tmp6;_tmp6.tag=1;_tmp6.f1=(
unsigned int)bt[_check_known_subscript_notnull(20,c)];{void*_tmp5[1]={& _tmp6};
Cyc_Std_fprintf(w,_tag_arr("%#x\n",sizeof(char),5),_tag_arr(_tmp5,sizeof(void*),
1));}});}}Cyc_Std_fflush((struct Cyc_Std___cycFILE*)w);({void*_tmp7[0]={};Cyc_Std_printf(
_tag_arr("Backtrace:\n  Function          Location\n  ----------------  --------------------------------\n",
sizeof(char),94),_tag_arr(_tmp7,sizeof(void*),0));});{int c=0;for(0;c < n;c ++){int
d;int pos=0;({void*_tmp8[0]={};Cyc_Std_printf(_tag_arr("  ",sizeof(char),3),
_tag_arr(_tmp8,sizeof(void*),0));});while((d=Cyc_Std_getc(r))!= '\n'){(Cyc_Std_putc(
d,Cyc_Std_stdout),pos ++);}while(pos ++ < 16){Cyc_Std_putc((int)' ',Cyc_Std_stdout);}({
void*_tmp9[0]={};Cyc_Std_printf(_tag_arr("  ",sizeof(char),3),_tag_arr(_tmp9,
sizeof(void*),0));});while((d=Cyc_Std_getc(r))!= '\n'){Cyc_Std_putc(d,Cyc_Std_stdout);}
Cyc_Std_putc((int)'\n',Cyc_Std_stdout);}}Cyc_Std_fclose(r);Cyc_Std_fclose(w);
kill(pid,15);waitpid(pid,0,0);return 0;}}}
