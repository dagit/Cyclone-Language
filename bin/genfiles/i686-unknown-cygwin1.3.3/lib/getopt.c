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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[18];
extern unsigned char Cyc_List_Nth[8];extern unsigned char Cyc_Array_Array_mismatch[
19];struct Cyc_Std__Div{int quot;int rem;};struct Cyc_Std__Ldiv{int quot;int rem;};
extern struct _tagged_arr Cyc_Std_getenv(struct _tagged_arr);struct Cyc_Std_timeval{
int tv_sec;int tv_usec;};struct Cyc_Std_timezone{int tz_minuteswest;int tz_dsttime;};
struct Cyc_Std_itimerval{struct Cyc_Std_timeval it_interval;struct Cyc_Std_timeval
it_value;};typedef struct {unsigned int __val[64];}Cyc_Std___sigset_t;typedef Cyc_Std___sigset_t
Cyc_Std_sigset_t;struct Cyc_Std_timespec{int tv_sec;int tv_nsec;};struct Cyc_Std_timeval;
extern struct _tagged_arr Cyc_Std_optarg;extern int Cyc_Std_optind;extern int Cyc_Std_opterr;
extern int Cyc_Std_optopt;struct Cyc_Std_option{struct _tagged_arr name;int has_arg;
int*flag;int val;};extern int Cyc_Std_getopt(int __argc,struct _tagged_arr __argv,
struct _tagged_arr __shortopts);extern int Cyc_Std__getopt_internal(int __argc,struct
_tagged_arr __argv,struct _tagged_arr __shortopts,struct _tagged_arr __longopts,int*
__longind,int __long_only);int Cyc_Std_access(struct _tagged_arr,int);int Cyc_Std_chdir(
struct _tagged_arr);int Cyc_Std_chown(struct _tagged_arr,unsigned short,
unsigned short);struct _tagged_arr Cyc_Std_getcwd(struct _tagged_arr buf,
unsigned int size);int Cyc_Std_execl(struct _tagged_arr path,struct _tagged_arr arg0,
struct _tagged_arr argv);int Cyc_Std_execlp(struct _tagged_arr file,struct _tagged_arr
arg0,struct _tagged_arr argv);int Cyc_Std_execve(struct _tagged_arr filename,struct
_tagged_arr argv,struct _tagged_arr envp);int Cyc_Std_link(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_read(int fd,struct _tagged_arr buf,unsigned int count);int
Cyc_Std_rmdir(struct _tagged_arr);int Cyc_Std_symlink(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_truncate(struct _tagged_arr,int);int Cyc_Std_write(int fd,
struct _tagged_arr buf,unsigned int count);int Cyc_Std_unlink(struct _tagged_arr
pathname);int Cyc_Std_gethostname(struct _tagged_arr,unsigned int);int Cyc_Std_chroot(
struct _tagged_arr);struct _tagged_arr Cyc_Std_getpass(struct _tagged_arr prompt);
extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_strncmp(struct
_tagged_arr s1,struct _tagged_arr s2,unsigned int len);extern struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s,unsigned char c);struct _tagged_arr Cyc_Std_optarg;int Cyc_Std_optind=
1;int Cyc_Std___getopt_initialized;static struct _tagged_arr Cyc_Std_nextchar;int Cyc_Std_opterr=
1;int Cyc_Std_optopt=(int)'?';enum Cyc_Std_ordering_tag{Cyc_Std_REQUIRE_ORDER  = 0,
Cyc_Std_PERMUTE  = 1,Cyc_Std_RETURN_IN_ORDER  = 2};static enum Cyc_Std_ordering_tag
Cyc_Std_ordering;static struct _tagged_arr Cyc_Std_posixly_correct;static int Cyc_Std_first_nonopt;
static int Cyc_Std_last_nonopt;static int Cyc_Std_nonoption_flags_max_len;static int
Cyc_Std_nonoption_flags_len;static int Cyc_Std_original_argc;static int Cyc_Std_original_argv;
static void  __attribute__((unused )) Cyc_Std_store_args_and_env(int argc,struct
_tagged_arr argv){Cyc_Std_original_argc=argc;Cyc_Std_original_argv=(int)argv.curr;}
static void Cyc_Std_exchange(struct _tagged_arr argv){int bottom=Cyc_Std_first_nonopt;
int middle=Cyc_Std_last_nonopt;int top=Cyc_Std_optind;struct _tagged_arr tem;while(
top > middle? middle > bottom: 0){if(top - middle > middle - bottom){int len=middle - 
bottom;int i;for(i=0;i < len;i ++){tem=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),bottom + i));*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),bottom + i))=*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(top - (
middle - bottom))+ i));*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),(top - (middle - bottom))+ i))=tem;;}top -=len;}else{int
len=top - middle;int i;for(i=0;i < len;i ++){tem=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),bottom + i));*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),bottom + i))=*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),
middle + i));*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct
_tagged_arr),middle + i))=tem;;}bottom +=len;}}Cyc_Std_first_nonopt +=Cyc_Std_optind
- Cyc_Std_last_nonopt;Cyc_Std_last_nonopt=Cyc_Std_optind;}static struct
_tagged_arr Cyc_Std__getopt_initialize(int argc,struct _tagged_arr argv,struct
_tagged_arr optstring){Cyc_Std_first_nonopt=(Cyc_Std_last_nonopt=Cyc_Std_optind);
Cyc_Std_nextchar=_tag_arr(0,0,0);Cyc_Std_posixly_correct=Cyc_Std_getenv(_tag_arr("POSIXLY_CORRECT",
sizeof(unsigned char),16));if(*((const unsigned char*)_check_unknown_subscript(
optstring,sizeof(unsigned char),0))== '-'){Cyc_Std_ordering=Cyc_Std_RETURN_IN_ORDER;
_tagged_arr_inplace_plus(& optstring,sizeof(unsigned char),1);}else{if(*((const
unsigned char*)_check_unknown_subscript(optstring,sizeof(unsigned char),0))== '+'){
Cyc_Std_ordering=Cyc_Std_REQUIRE_ORDER;_tagged_arr_inplace_plus(& optstring,
sizeof(unsigned char),1);}else{if(Cyc_Std_posixly_correct.curr != (_tag_arr(0,0,0)).curr){
Cyc_Std_ordering=Cyc_Std_REQUIRE_ORDER;}else{Cyc_Std_ordering=Cyc_Std_PERMUTE;}}}
return optstring;}int Cyc_Std__getopt_internal(int argc,struct _tagged_arr argv,
struct _tagged_arr optstring,struct _tagged_arr longopts,int*longind,int long_only){
int print_errors=Cyc_Std_opterr;if(*((const unsigned char*)
_check_unknown_subscript(optstring,sizeof(unsigned char),0))== ':'){print_errors=
0;}if(argc < 1){return - 1;}Cyc_Std_optarg=_tag_arr(0,0,0);if(Cyc_Std_optind == 0? 1:
! Cyc_Std___getopt_initialized){if(Cyc_Std_optind == 0){Cyc_Std_optind=1;}
optstring=Cyc_Std__getopt_initialize(argc,argv,optstring);Cyc_Std___getopt_initialized=
1;}if(Cyc_Std_nextchar.curr == (_tag_arr(0,0,0)).curr? 1:*((unsigned char*)
_check_unknown_subscript(Cyc_Std_nextchar,sizeof(unsigned char),0))== '\000'){
if(Cyc_Std_last_nonopt > Cyc_Std_optind){Cyc_Std_last_nonopt=Cyc_Std_optind;}if(
Cyc_Std_first_nonopt > Cyc_Std_optind){Cyc_Std_first_nonopt=Cyc_Std_optind;}if(
Cyc_Std_ordering == Cyc_Std_PERMUTE){if(Cyc_Std_first_nonopt != Cyc_Std_last_nonopt?
Cyc_Std_last_nonopt != Cyc_Std_optind: 0){Cyc_Std_exchange(argv);}else{if(Cyc_Std_last_nonopt
!= Cyc_Std_optind){Cyc_Std_first_nonopt=Cyc_Std_optind;}}while(Cyc_Std_optind < 
argc?*((unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(
unsigned char),0))!= '-'? 1:*((unsigned char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),
sizeof(unsigned char),1))== '\000': 0){Cyc_Std_optind ++;}Cyc_Std_last_nonopt=Cyc_Std_optind;}
if(Cyc_Std_optind != argc? ! Cyc_Std_strcmp((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),
_tag_arr("--",sizeof(unsigned char),3)): 0){Cyc_Std_optind ++;if(Cyc_Std_first_nonopt
!= Cyc_Std_last_nonopt? Cyc_Std_last_nonopt != Cyc_Std_optind: 0){Cyc_Std_exchange(
argv);}else{if(Cyc_Std_first_nonopt == Cyc_Std_last_nonopt){Cyc_Std_first_nonopt=
Cyc_Std_optind;}}Cyc_Std_last_nonopt=argc;Cyc_Std_optind=argc;}if(Cyc_Std_optind
== argc){if(Cyc_Std_first_nonopt != Cyc_Std_last_nonopt){Cyc_Std_optind=Cyc_Std_first_nonopt;}
return - 1;}if(*((unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(
unsigned char),0))!= '-'? 1:*((unsigned char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),
sizeof(unsigned char),1))== '\000'){if(Cyc_Std_ordering == Cyc_Std_REQUIRE_ORDER){
return - 1;}Cyc_Std_optarg=*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_Std_optind ++));return 1;}Cyc_Std_nextchar=
_tagged_arr_plus(_tagged_arr_plus(*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(unsigned char),1),sizeof(
unsigned char),longopts.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?*((
unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(
unsigned char),1))== '-': 0);}if(longopts.curr != ((struct _tagged_arr)_tag_arr(0,0,
0)).curr?*((unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(
unsigned char),1))== '-'? 1:(long_only?(int)*((unsigned char*)
_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(unsigned char),2))? 1: !((
unsigned int)(Cyc_Std_strchr(optstring,*((unsigned char*)
_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(unsigned char),1)))).curr): 0):
0){struct _tagged_arr nameend;struct _tagged_arr p;struct _tagged_arr pfound=(struct
_tagged_arr)_tag_arr(0,0,0);int exact=0;int ambig=0;int indfound=- 1;int option_index;
for(nameend=Cyc_Std_nextchar;(int)*((unsigned char*)_check_unknown_subscript(
nameend,sizeof(unsigned char),0))?*((unsigned char*)_check_unknown_subscript(
nameend,sizeof(unsigned char),0))!= '=': 0;_tagged_arr_inplace_plus_post(& nameend,
sizeof(unsigned char),1)){;}for((p=longopts,option_index=0);(unsigned int)(((
const struct Cyc_Std_option*)_check_unknown_subscript(p,sizeof(struct Cyc_Std_option),
0))->name).curr;(_tagged_arr_inplace_plus_post(& p,sizeof(struct Cyc_Std_option),1),
option_index ++)){if(! Cyc_Std_strncmp(((const struct Cyc_Std_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_Std_option),0))->name,(struct
_tagged_arr)Cyc_Std_nextchar,(unsigned int)((nameend.curr - Cyc_Std_nextchar.curr)
/ sizeof(unsigned char)))){if((unsigned int)((nameend.curr - Cyc_Std_nextchar.curr)
/ sizeof(unsigned char))== (unsigned int)Cyc_Std_strlen(((const struct Cyc_Std_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_Std_option),0))->name)){pfound=p;
indfound=option_index;exact=1;break;}else{if(pfound.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr){pfound=p;indfound=option_index;}else{if(((long_only? 1:((
const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),
0))->has_arg != ((const struct Cyc_Std_option*)_check_unknown_subscript(p,sizeof(
struct Cyc_Std_option),0))->has_arg)? 1:((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->flag != ((const
struct Cyc_Std_option*)_check_unknown_subscript(p,sizeof(struct Cyc_Std_option),0))->flag)?
1:((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),
0))->val != ((const struct Cyc_Std_option*)_check_unknown_subscript(p,sizeof(struct
Cyc_Std_option),0))->val){ambig=1;}}}}}if(ambig? ! exact: 0){if(print_errors){({
struct Cyc_Std_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind));{
struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{
void*_tmp0[2]={& _tmp1,& _tmp2};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `%s' is ambiguous\n",
sizeof(unsigned char),30),_tag_arr(_tmp0,sizeof(void*),2));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));Cyc_Std_optind ++;Cyc_Std_optopt=0;return(
int)'?';}if(pfound.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
option_index=indfound;Cyc_Std_optind ++;if((int)*((unsigned char*)
_check_unknown_subscript(nameend,sizeof(unsigned char),0))){if(((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->has_arg){Cyc_Std_optarg=
_tagged_arr_plus(nameend,sizeof(unsigned char),1);}else{if(print_errors){if(*((
unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind - 1)),
sizeof(unsigned char),1))== '-'){({struct Cyc_Std_String_pa_struct _tmp5;_tmp5.tag=
0;_tmp5.f1=(struct _tagged_arr)((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->name;{struct
Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*
_tmp3[2]={& _tmp4,& _tmp5};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `--%s' doesn't allow an argument\n",
sizeof(unsigned char),45),_tag_arr(_tmp3,sizeof(void*),2));}}});}else{({struct
Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _tagged_arr)((const
struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),
0))->name;{struct Cyc_Std_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(int)((
unsigned int)((int)*((unsigned char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind
- 1)),sizeof(unsigned char),0))));{struct Cyc_Std_String_pa_struct _tmp7;_tmp7.tag=
0;_tmp7.f1=(struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),0));{void*_tmp6[3]={& _tmp7,& _tmp8,& _tmp9};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: option `%c%s' doesn't allow an argument\n",sizeof(
unsigned char),45),_tag_arr(_tmp6,sizeof(void*),3));}}}});}}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));Cyc_Std_optopt=((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->val;return(int)'?';}}
else{if(((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(
struct Cyc_Std_option),0))->has_arg == 1){if(Cyc_Std_optind < argc){Cyc_Std_optarg=*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind
++));}else{if(print_errors){({struct Cyc_Std_String_pa_struct _tmpC;_tmpC.tag=0;
_tmpC.f1=(struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_Std_optind - 1));{struct Cyc_Std_String_pa_struct
_tmpB;_tmpB.tag=0;_tmpB.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*_tmpA[2]={&
_tmpB,& _tmpC};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `%s' requires an argument\n",
sizeof(unsigned char),38),_tag_arr(_tmpA,sizeof(void*),2));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));Cyc_Std_optopt=((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->val;return(int)(*((
const unsigned char*)_check_unknown_subscript(optstring,sizeof(unsigned char),0))
== ':'?':':'?');}}}_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(
unsigned char),(int)Cyc_Std_strlen((struct _tagged_arr)Cyc_Std_nextchar));if(
longind != 0){*longind=option_index;}if((unsigned int)((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->flag){*((int*)
_check_null(((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(
struct Cyc_Std_option),0))->flag))=((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->val;return 0;}
return((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct
Cyc_Std_option),0))->val;}if((! long_only? 1:*((unsigned char*)
_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(unsigned char),1))== '-')? 1:(
Cyc_Std_strchr(optstring,*((unsigned char*)_check_unknown_subscript(Cyc_Std_nextchar,
sizeof(unsigned char),0)))).curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
if(print_errors){if(*((unsigned char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),
sizeof(unsigned char),1))== '-'){({struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=
0;_tmpF.f1=(struct _tagged_arr)Cyc_Std_nextchar;{struct Cyc_Std_String_pa_struct
_tmpE;_tmpE.tag=0;_tmpE.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*_tmpD[2]={&
_tmpE,& _tmpF};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: unrecognized option `--%s'\n",
sizeof(unsigned char),32),_tag_arr(_tmpD,sizeof(void*),2));}}});}else{({struct
Cyc_Std_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _tagged_arr)Cyc_Std_nextchar;{
struct Cyc_Std_Int_pa_struct _tmp12;_tmp12.tag=1;_tmp12.f1=(int)((unsigned int)((
int)*((unsigned char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind)),sizeof(
unsigned char),0))));{struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),0));{void*_tmp10[3]={& _tmp11,& _tmp12,& _tmp13};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: unrecognized option `%c%s'\n",sizeof(unsigned char),
32),_tag_arr(_tmp10,sizeof(void*),3));}}}});}}Cyc_Std_nextchar=_tag_arr(({
unsigned int _tmp14=(unsigned int)1;unsigned char*_tmp15=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp14));{unsigned int _tmp16=
_tmp14;unsigned int i;for(i=0;i < _tmp16;i ++){_tmp15[i]='\000';}};_tmp15;}),
sizeof(unsigned char),(unsigned int)1);Cyc_Std_optind ++;Cyc_Std_optopt=0;return(
int)'?';}}{unsigned char c=*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& Cyc_Std_nextchar,sizeof(unsigned char),1),sizeof(
unsigned char),0));struct _tagged_arr temp=Cyc_Std_strchr(optstring,c);if(*((
unsigned char*)_check_unknown_subscript(Cyc_Std_nextchar,sizeof(unsigned char),0))
== '\000'){++ Cyc_Std_optind;}if(temp.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
1: c == ':'){if(print_errors){if((unsigned int)Cyc_Std_posixly_correct.curr){({
struct Cyc_Std_Int_pa_struct _tmp19;_tmp19.tag=1;_tmp19.f1=(int)((unsigned int)((
int)c));{struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct
_tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct
_tagged_arr),0));{void*_tmp17[2]={& _tmp18,& _tmp19};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: illegal option -- %c\n",sizeof(unsigned char),26),_tag_arr(_tmp17,
sizeof(void*),2));}}});}else{({struct Cyc_Std_Int_pa_struct _tmp1C;_tmp1C.tag=1;
_tmp1C.f1=(int)((unsigned int)((int)c));{struct Cyc_Std_String_pa_struct _tmp1B;
_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*_tmp1A[2]={&
_tmp1B,& _tmp1C};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: invalid option -- %c\n",
sizeof(unsigned char),26),_tag_arr(_tmp1A,sizeof(void*),2));}}});}}Cyc_Std_optopt=(
int)c;return(int)'?';}if(*((const unsigned char*)_check_unknown_subscript(temp,
sizeof(unsigned char),0))== 'W'?*((const unsigned char*)_check_unknown_subscript(
temp,sizeof(unsigned char),1))== ';': 0){struct _tagged_arr nameend;struct
_tagged_arr p;struct _tagged_arr pfound=(struct _tagged_arr)_tag_arr(0,0,0);int exact=
0;int ambig=0;int indfound=0;int option_index;if(*((unsigned char*)
_check_unknown_subscript(Cyc_Std_nextchar,sizeof(unsigned char),0))!= '\000'){
Cyc_Std_optarg=Cyc_Std_nextchar;Cyc_Std_optind ++;}else{if(Cyc_Std_optind == argc){
if(print_errors){({struct Cyc_Std_Int_pa_struct _tmp1F;_tmp1F.tag=1;_tmp1F.f1=(int)((
unsigned int)((int)c));{struct Cyc_Std_String_pa_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),0));{void*_tmp1D[2]={& _tmp1E,& _tmp1F};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: option requires an argument -- %c\n",sizeof(unsigned char),39),
_tag_arr(_tmp1D,sizeof(void*),2));}}});}Cyc_Std_optopt=(int)c;if(*((const
unsigned char*)_check_unknown_subscript(optstring,sizeof(unsigned char),0))== ':'){
c=':';}else{c='?';}return(int)c;}else{Cyc_Std_optarg=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind ++));}}
for(Cyc_Std_nextchar=(nameend=Cyc_Std_optarg);(int)*((unsigned char*)
_check_unknown_subscript(nameend,sizeof(unsigned char),0))?*((unsigned char*)
_check_unknown_subscript(nameend,sizeof(unsigned char),0))!= '=': 0;
_tagged_arr_inplace_plus_post(& nameend,sizeof(unsigned char),1)){;}for((p=
longopts,option_index=0);(unsigned int)(((const struct Cyc_Std_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_Std_option),0))->name).curr;(
_tagged_arr_inplace_plus_post(& p,sizeof(struct Cyc_Std_option),1),option_index ++)){
if(! Cyc_Std_strncmp(((const struct Cyc_Std_option*)_check_unknown_subscript(p,
sizeof(struct Cyc_Std_option),0))->name,(struct _tagged_arr)Cyc_Std_nextchar,(
unsigned int)((nameend.curr - Cyc_Std_nextchar.curr)/ sizeof(unsigned char)))){
if((unsigned int)((nameend.curr - Cyc_Std_nextchar.curr)/ sizeof(unsigned char))
== Cyc_Std_strlen(((const struct Cyc_Std_option*)_check_unknown_subscript(p,
sizeof(struct Cyc_Std_option),0))->name)){pfound=_tag_arr((const struct Cyc_Std_option*)
_check_null(_untag_arr(p,sizeof(struct Cyc_Std_option),1)),sizeof(struct Cyc_Std_option),
1);indfound=option_index;exact=1;break;}else{if(pfound.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){pfound=_tag_arr((const struct Cyc_Std_option*)
_check_null(_untag_arr(p,sizeof(struct Cyc_Std_option),1)),sizeof(struct Cyc_Std_option),
1);indfound=option_index;}else{ambig=1;}}}}if(ambig? ! exact: 0){if(print_errors){({
struct Cyc_Std_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind));{
struct Cyc_Std_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{
void*_tmp20[2]={& _tmp21,& _tmp22};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `-W %s' is ambiguous\n",
sizeof(unsigned char),33),_tag_arr(_tmp20,sizeof(void*),2));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));Cyc_Std_optind ++;return(int)'?';}if(pfound.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){option_index=indfound;if((int)*((
unsigned char*)_check_unknown_subscript(nameend,sizeof(unsigned char),0))){if(((
const struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),
0))->has_arg){Cyc_Std_optarg=_tagged_arr_plus(nameend,sizeof(unsigned char),1);}
else{if(print_errors){({struct Cyc_Std_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(
struct _tagged_arr)((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,
sizeof(struct Cyc_Std_option),0))->name;{struct Cyc_Std_String_pa_struct _tmp24;
_tmp24.tag=0;_tmp24.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*_tmp23[2]={&
_tmp24,& _tmp25};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `-W %s' doesn't allow an argument\n",
sizeof(unsigned char),46),_tag_arr(_tmp23,sizeof(void*),2));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));return(int)'?';}}else{if(((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->has_arg == 1){
if(Cyc_Std_optind < argc){Cyc_Std_optarg=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind ++));}
else{if(print_errors){({struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),Cyc_Std_optind - 1));{struct Cyc_Std_String_pa_struct _tmp27;
_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0));{void*_tmp26[2]={&
_tmp27,& _tmp28};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: option `%s' requires an argument\n",
sizeof(unsigned char),38),_tag_arr(_tmp26,sizeof(void*),2));}}});}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));return(int)(*((const unsigned char*)
_check_unknown_subscript(optstring,sizeof(unsigned char),0))== ':'?':':'?');}}}
_tagged_arr_inplace_plus(& Cyc_Std_nextchar,sizeof(unsigned char),(int)Cyc_Std_strlen((
struct _tagged_arr)Cyc_Std_nextchar));if(longind != 0){*longind=option_index;}if((
unsigned int)((const struct Cyc_Std_option*)_check_unknown_subscript(pfound,
sizeof(struct Cyc_Std_option),0))->flag){*((int*)_check_null(((const struct Cyc_Std_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),0))->flag))=((const
struct Cyc_Std_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_Std_option),
0))->val;return 0;}return((const struct Cyc_Std_option*)_check_unknown_subscript(
pfound,sizeof(struct Cyc_Std_option),0))->val;}Cyc_Std_nextchar=_tag_arr(0,0,0);
return(int)'W';}if(*((const unsigned char*)_check_unknown_subscript(temp,sizeof(
unsigned char),1))== ':'){if(*((const unsigned char*)_check_unknown_subscript(
temp,sizeof(unsigned char),2))== ':'){if(*((unsigned char*)
_check_unknown_subscript(Cyc_Std_nextchar,sizeof(unsigned char),0))!= '\000'){
Cyc_Std_optarg=Cyc_Std_nextchar;Cyc_Std_optind ++;}else{Cyc_Std_optarg=_tag_arr(0,
0,0);}Cyc_Std_nextchar=_tag_arr(0,0,0);}else{if(*((unsigned char*)
_check_unknown_subscript(Cyc_Std_nextchar,sizeof(unsigned char),0))!= '\000'){
Cyc_Std_optarg=Cyc_Std_nextchar;Cyc_Std_optind ++;}else{if(Cyc_Std_optind == argc){
if(print_errors){({struct Cyc_Std_Int_pa_struct _tmp2B;_tmp2B.tag=1;_tmp2B.f1=(int)((
unsigned int)((int)c));{struct Cyc_Std_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),0));{void*_tmp29[2]={& _tmp2A,& _tmp2B};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: option requires an argument -- %c\n",sizeof(unsigned char),39),
_tag_arr(_tmp29,sizeof(void*),2));}}});}Cyc_Std_optopt=(int)c;if(*((const
unsigned char*)_check_unknown_subscript(optstring,sizeof(unsigned char),0))== ':'){
c=':';}else{c='?';}}else{Cyc_Std_optarg=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_Std_optind ++));}}Cyc_Std_nextchar=
_tag_arr(0,0,0);}}return(int)c;}}int Cyc_Std_getopt(int argc,struct _tagged_arr argv,
struct _tagged_arr optstring){return Cyc_Std__getopt_internal(argc,argv,optstring,(
struct _tagged_arr)_tag_arr(0,0,0),(int*)0,0);}
