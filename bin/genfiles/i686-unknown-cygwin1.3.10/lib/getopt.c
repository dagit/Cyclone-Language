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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static inline void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static inline char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
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
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tag_arr(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr *
_init_tag_arr(struct _tagged_arr *arr_ptr,
              void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _tagged_arr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_arr(struct _tagged_arr arr, unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_arr_size(struct _tagged_arr arr,unsigned elt_sz) {
  struct _tagged_arr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_plus(struct _tagged_arr arr,unsigned elt_sz,int change) {
  struct _tagged_arr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus_post(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  struct _tagged_arr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })
#endif

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
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
 void exit(int);void*abort();struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_fprintf(
struct Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};typedef struct{int quot;int rem;}Cyc_div_t;typedef
struct{int quot;int rem;}Cyc_ldiv_t;void*abort();void exit(int);char*getenv(const
char*);extern struct _tagged_arr Cyc_optarg;extern int Cyc_optind;extern int Cyc_opterr;
extern int Cyc_optopt;struct Cyc_option{struct _tagged_arr name;int has_arg;int*flag;
int val;};int Cyc_getopt(int __argc,struct _tagged_arr __argv,struct _tagged_arr
__shortopts);int Cyc__getopt_internal(int __argc,struct _tagged_arr __argv,struct
_tagged_arr __shortopts,struct _tagged_arr __longopts,int*__longind,int __long_only);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strncmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int len);struct _tagged_arr Cyc_strchr(struct
_tagged_arr s,char c);struct _tagged_arr Cyc_optarg;int Cyc_optind=1;int Cyc___getopt_initialized;
static struct _tagged_arr Cyc_nextchar;int Cyc_opterr=1;int Cyc_optopt=(int)'?';enum 
Cyc_ordering_tag{Cyc_REQUIRE_ORDER  = 0,Cyc_PERMUTE  = 1,Cyc_RETURN_IN_ORDER  = 2};
static enum Cyc_ordering_tag Cyc_ordering;static struct _tagged_arr Cyc_posixly_correct;
static int Cyc_first_nonopt;static int Cyc_last_nonopt;static int Cyc_nonoption_flags_max_len;
static int Cyc_nonoption_flags_len;static int Cyc_original_argc;static int Cyc_original_argv;
static void  __attribute__((unused )) Cyc_store_args_and_env(int argc,struct
_tagged_arr argv){Cyc_original_argc=argc;Cyc_original_argv=(int)((struct
_tagged_arr*)_check_null(_untag_arr(argv,sizeof(struct _tagged_arr),1)));}static
void Cyc_exchange(struct _tagged_arr argv){int bottom=Cyc_first_nonopt;int middle=Cyc_last_nonopt;
int top=Cyc_optind;struct _tagged_arr tem;while(top > middle?middle > bottom: 0){if(
top - middle > middle - bottom){int len=middle - bottom;register int i;for(i=0;i < len;i
++){tem=*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct
_tagged_arr),bottom + i));*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),bottom + i))=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(top - (middle - bottom))+ 
i));*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),(
top - (middle - bottom))+ i))=tem;}top -=len;}else{int len=top - middle;register int i;
for(i=0;i < len;i ++){tem=*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),bottom + i));*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),bottom + i))=*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),middle + i));*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),
middle + i))=tem;}bottom +=len;}}Cyc_first_nonopt +=Cyc_optind - Cyc_last_nonopt;Cyc_last_nonopt=
Cyc_optind;}static struct _tagged_arr Cyc__getopt_initialize(int argc,struct
_tagged_arr argv,struct _tagged_arr optstring){Cyc_first_nonopt=(Cyc_last_nonopt=
Cyc_optind);Cyc_nextchar=_tag_arr(0,0,0);Cyc_posixly_correct=({char*_tmp0=getenv((
const char*)"POSIXLY_CORRECT");_tag_arr(_tmp0,sizeof(char),_get_zero_arr_size(
_tmp0,1));});if(*((const char*)_check_unknown_subscript(optstring,sizeof(char),0))
== '-'){Cyc_ordering=Cyc_RETURN_IN_ORDER;_tagged_arr_inplace_plus(& optstring,
sizeof(char),1);}else{if(*((const char*)_check_unknown_subscript(optstring,
sizeof(char),0))== '+'){Cyc_ordering=Cyc_REQUIRE_ORDER;_tagged_arr_inplace_plus(&
optstring,sizeof(char),1);}else{if(Cyc_posixly_correct.curr != (_tag_arr(0,0,0)).curr)
Cyc_ordering=Cyc_REQUIRE_ORDER;else{Cyc_ordering=Cyc_PERMUTE;}}}return optstring;}
int Cyc__getopt_internal(int argc,struct _tagged_arr argv,struct _tagged_arr optstring,
struct _tagged_arr longopts,int*longind,int long_only){int print_errors=Cyc_opterr;
if(*((const char*)_check_unknown_subscript(optstring,sizeof(char),0))== ':')
print_errors=0;if(argc < 1)return - 1;Cyc_optarg=_tag_arr(0,0,0);if(Cyc_optind == 0?
1: !Cyc___getopt_initialized){if(Cyc_optind == 0)Cyc_optind=1;optstring=Cyc__getopt_initialize(
argc,argv,optstring);Cyc___getopt_initialized=1;}if(Cyc_nextchar.curr == (
_tag_arr(0,0,0)).curr?1:*((char*)_check_unknown_subscript(Cyc_nextchar,sizeof(
char),0))== '\000'){if(Cyc_last_nonopt > Cyc_optind)Cyc_last_nonopt=Cyc_optind;
if(Cyc_first_nonopt > Cyc_optind)Cyc_first_nonopt=Cyc_optind;if(Cyc_ordering == 
Cyc_PERMUTE){if(Cyc_first_nonopt != Cyc_last_nonopt?Cyc_last_nonopt != Cyc_optind:
0)Cyc_exchange(argv);else{if(Cyc_last_nonopt != Cyc_optind)Cyc_first_nonopt=Cyc_optind;}
while(Cyc_optind < argc?*((char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),
0))!= '-'?1:*((char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),
1))== '\000': 0){Cyc_optind ++;}Cyc_last_nonopt=Cyc_optind;}if(Cyc_optind != argc?!
Cyc_strcmp((struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_optind)),({const char*_tmp1="--";_tag_arr(
_tmp1,sizeof(char),_get_zero_arr_size(_tmp1,3));})): 0){Cyc_optind ++;if(Cyc_first_nonopt
!= Cyc_last_nonopt?Cyc_last_nonopt != Cyc_optind: 0)Cyc_exchange(argv);else{if(Cyc_first_nonopt
== Cyc_last_nonopt)Cyc_first_nonopt=Cyc_optind;}Cyc_last_nonopt=argc;Cyc_optind=
argc;}if(Cyc_optind == argc){if(Cyc_first_nonopt != Cyc_last_nonopt)Cyc_optind=Cyc_first_nonopt;
return - 1;}if(*((char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),
0))!= '-'?1:*((char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),
1))== '\000'){if(Cyc_ordering == Cyc_REQUIRE_ORDER)return - 1;Cyc_optarg=*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind
++));return 1;}Cyc_nextchar=_tagged_arr_plus(_tagged_arr_plus(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),
sizeof(char),1),sizeof(char),longopts.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?*((
char*)_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),1))== '-': 0);}if(
longopts.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr?*((char*)
_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),1))== '-'?1:(long_only?(int)*((
char*)_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),2))?1: !((unsigned int)(
Cyc_strchr(optstring,*((char*)_check_unknown_subscript(*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),
1)))).curr): 0): 0){struct _tagged_arr nameend;struct _tagged_arr p;struct _tagged_arr
pfound=(struct _tagged_arr)_tag_arr(0,0,0);int exact=0;int ambig=0;int indfound=- 1;
int option_index;for(nameend=Cyc_nextchar;(int)*((char*)_check_unknown_subscript(
nameend,sizeof(char),0))?*((char*)_check_unknown_subscript(nameend,sizeof(char),
0))!= '=': 0;_tagged_arr_inplace_plus_post(& nameend,sizeof(char),1)){;}for((p=
longopts,option_index=0);(unsigned int)(((const struct Cyc_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->name).curr;(
_tagged_arr_inplace_plus_post(& p,sizeof(struct Cyc_option),1),option_index ++)){
if(!Cyc_strncmp((struct _tagged_arr)((const struct Cyc_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->name,(struct _tagged_arr)
Cyc_nextchar,(unsigned int)((nameend.curr - Cyc_nextchar.curr)/ sizeof(char)))){
if((unsigned int)((nameend.curr - Cyc_nextchar.curr)/ sizeof(char))== (
unsigned int)Cyc_strlen((struct _tagged_arr)((const struct Cyc_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->name)){pfound=p;
indfound=option_index;exact=1;break;}else{if(pfound.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr){pfound=p;indfound=option_index;}else{if(((long_only?1:((
const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),
0))->has_arg != ((const struct Cyc_option*)_check_unknown_subscript(p,sizeof(struct
Cyc_option),0))->has_arg)?1:((const struct Cyc_option*)_check_unknown_subscript(
pfound,sizeof(struct Cyc_option),0))->flag != ((const struct Cyc_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->flag)?1:((const struct
Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->val != ((
const struct Cyc_option*)_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->val)
ambig=1;}}}}if(ambig?!exact: 0){if(print_errors)({struct Cyc_String_pa_struct _tmp5;
_tmp5.tag=0;_tmp5.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)));{struct Cyc_String_pa_struct
_tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*
_tmp2[2]={& _tmp4,& _tmp5};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="%s: option `%s' is ambiguous\n";
_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,30));}),_tag_arr(_tmp2,
sizeof(void*),2));}}});_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)
Cyc_strlen((struct _tagged_arr)Cyc_nextchar));Cyc_optind ++;Cyc_optopt=0;return(
int)'?';}if(pfound.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
option_index=indfound;Cyc_optind ++;if((int)*((char*)_check_unknown_subscript(
nameend,sizeof(char),0))){if(((const struct Cyc_option*)_check_unknown_subscript(
pfound,sizeof(struct Cyc_option),0))->has_arg)Cyc_optarg=_tagged_arr_plus(nameend,
sizeof(char),1);else{if(print_errors){if(*((char*)_check_unknown_subscript(*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind
- 1)),sizeof(char),1))== '-')({struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(
struct _tagged_arr)((struct _tagged_arr)((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->name);{struct Cyc_String_pa_struct
_tmp8;_tmp8.tag=0;_tmp8.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*
_tmp6[2]={& _tmp8,& _tmp9};Cyc_fprintf(Cyc_stderr,({const char*_tmp7="%s: option `--%s' doesn't allow an argument\n";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,45));}),_tag_arr(_tmp6,
sizeof(void*),2));}}});else{({struct Cyc_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(
struct _tagged_arr)((struct _tagged_arr)((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->name);{struct Cyc_Int_pa_struct
_tmpD;_tmpD.tag=1;_tmpD.f1=(unsigned int)((int)*((char*)_check_unknown_subscript(*((
struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind
- 1)),sizeof(char),0)));{struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmpA[3]={&
_tmpC,& _tmpD,& _tmpE};Cyc_fprintf(Cyc_stderr,({const char*_tmpB="%s: option `%c%s' doesn't allow an argument\n";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,45));}),_tag_arr(_tmpA,
sizeof(void*),3));}}}});}}_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(
int)Cyc_strlen((struct _tagged_arr)Cyc_nextchar));Cyc_optopt=((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->val;return(int)'?';}}
else{if(((const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct
Cyc_option),0))->has_arg == 1){if(Cyc_optind < argc)Cyc_optarg=*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind
++));else{if(print_errors)({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind - 1)));{struct
Cyc_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _tagged_arr)((struct
_tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(struct
_tagged_arr),0)));{void*_tmpF[2]={& _tmp11,& _tmp12};Cyc_fprintf(Cyc_stderr,({
const char*_tmp10="%s: option `%s' requires an argument\n";_tag_arr(_tmp10,
sizeof(char),_get_zero_arr_size(_tmp10,38));}),_tag_arr(_tmpF,sizeof(void*),2));}}});
_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)Cyc_strlen((struct
_tagged_arr)Cyc_nextchar));Cyc_optopt=((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->val;return(int)(*((
const char*)_check_unknown_subscript(optstring,sizeof(char),0))== ':'?':':'?');}}}
_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)Cyc_strlen((struct
_tagged_arr)Cyc_nextchar));if(longind != 0)*longind=option_index;if((unsigned int)((
const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),
0))->flag){*((int*)_check_null(((const struct Cyc_option*)_check_unknown_subscript(
pfound,sizeof(struct Cyc_option),0))->flag))=((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->val;return 0;}
return((const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),
0))->val;}if((!long_only?1:*((char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),
sizeof(char),1))== '-')?1:(Cyc_strchr(optstring,*((char*)
_check_unknown_subscript(Cyc_nextchar,sizeof(char),0)))).curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(print_errors){if(*((char*)
_check_unknown_subscript(*((struct _tagged_arr*)_check_unknown_subscript(argv,
sizeof(struct _tagged_arr),Cyc_optind)),sizeof(char),1))== '-')({struct Cyc_String_pa_struct
_tmp16;_tmp16.tag=0;_tmp16.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_nextchar);{
struct Cyc_String_pa_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),0)));{void*_tmp13[2]={& _tmp15,& _tmp16};Cyc_fprintf(Cyc_stderr,({
const char*_tmp14="%s: unrecognized option `--%s'\n";_tag_arr(_tmp14,sizeof(char),
_get_zero_arr_size(_tmp14,32));}),_tag_arr(_tmp13,sizeof(void*),2));}}});else{({
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_nextchar);{struct Cyc_Int_pa_struct _tmp1A;_tmp1A.tag=1;
_tmp1A.f1=(unsigned int)((int)*((char*)_check_unknown_subscript(*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind)),
sizeof(char),0)));{struct Cyc_String_pa_struct _tmp19;_tmp19.tag=0;_tmp19.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmp17[3]={&
_tmp19,& _tmp1A,& _tmp1B};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="%s: unrecognized option `%c%s'\n";
_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,32));}),_tag_arr(_tmp17,
sizeof(void*),3));}}}});}}Cyc_nextchar=({char*_tmp1F=({unsigned int _tmp1C=(
unsigned int)1;char*_tmp1D=(char*)_cycalloc_atomic(_check_times(sizeof(char),
_tmp1C + 1));{unsigned int _tmp1E=_tmp1C;unsigned int i;for(i=0;i < _tmp1E;i ++){
_tmp1D[i]='\000';}_tmp1D[_tmp1E]=(char)0;}_tmp1D;});_tag_arr(_tmp1F,sizeof(char),
_get_zero_arr_size(_tmp1F,(unsigned int)1 + 1));});Cyc_optind ++;Cyc_optopt=0;
return(int)'?';}}{char c=*((char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& Cyc_nextchar,sizeof(char),1),sizeof(char),0));
struct _tagged_arr temp=Cyc_strchr(optstring,c);if(*((char*)
_check_unknown_subscript(Cyc_nextchar,sizeof(char),0))== '\000')++ Cyc_optind;if(
temp.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: c == ':'){if(print_errors){
if((unsigned int)Cyc_posixly_correct.curr)({struct Cyc_Int_pa_struct _tmp23;_tmp23.tag=
1;_tmp23.f1=(unsigned int)((int)c);{struct Cyc_String_pa_struct _tmp22;_tmp22.tag=
0;_tmp22.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmp20[2]={&
_tmp22,& _tmp23};Cyc_fprintf(Cyc_stderr,({const char*_tmp21="%s: illegal option -- %c\n";
_tag_arr(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,26));}),_tag_arr(_tmp20,
sizeof(void*),2));}}});else{({struct Cyc_Int_pa_struct _tmp27;_tmp27.tag=1;_tmp27.f1=(
unsigned int)((int)c);{struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(
struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmp24[2]={&
_tmp26,& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp25="%s: invalid option -- %c\n";
_tag_arr(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,26));}),_tag_arr(_tmp24,
sizeof(void*),2));}}});}}Cyc_optopt=(int)c;return(int)'?';}if(*((const char*)
_check_unknown_subscript(temp,sizeof(char),0))== 'W'?*((const char*)
_check_unknown_subscript(temp,sizeof(char),1))== ';': 0){struct _tagged_arr nameend;
struct _tagged_arr p;struct _tagged_arr pfound=(struct _tagged_arr)_tag_arr(0,0,0);
int exact=0;int ambig=0;int indfound=0;int option_index;if(*((char*)
_check_unknown_subscript(Cyc_nextchar,sizeof(char),0))!= '\000'){Cyc_optarg=Cyc_nextchar;
Cyc_optind ++;}else{if(Cyc_optind == argc){if(print_errors)({struct Cyc_Int_pa_struct
_tmp2B;_tmp2B.tag=1;_tmp2B.f1=(unsigned int)((int)c);{struct Cyc_String_pa_struct
_tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*
_tmp28[2]={& _tmp2A,& _tmp2B};Cyc_fprintf(Cyc_stderr,({const char*_tmp29="%s: option requires an argument -- %c\n";
_tag_arr(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,39));}),_tag_arr(_tmp28,
sizeof(void*),2));}}});Cyc_optopt=(int)c;if(*((const char*)
_check_unknown_subscript(optstring,sizeof(char),0))== ':')c=':';else{c='?';}
return(int)c;}else{Cyc_optarg=*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_optind ++));}}for(Cyc_nextchar=(nameend=Cyc_optarg);(
int)*((char*)_check_unknown_subscript(nameend,sizeof(char),0))?*((char*)
_check_unknown_subscript(nameend,sizeof(char),0))!= '=': 0;
_tagged_arr_inplace_plus_post(& nameend,sizeof(char),1)){;}for((p=longopts,
option_index=0);(unsigned int)(((const struct Cyc_option*)_check_unknown_subscript(
p,sizeof(struct Cyc_option),0))->name).curr;(_tagged_arr_inplace_plus_post(& p,
sizeof(struct Cyc_option),1),option_index ++)){if(!Cyc_strncmp((struct _tagged_arr)((
const struct Cyc_option*)_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->name,(
struct _tagged_arr)Cyc_nextchar,(unsigned int)((nameend.curr - Cyc_nextchar.curr)/ 
sizeof(char)))){if((unsigned int)((nameend.curr - Cyc_nextchar.curr)/ sizeof(char))
== Cyc_strlen((struct _tagged_arr)((const struct Cyc_option*)
_check_unknown_subscript(p,sizeof(struct Cyc_option),0))->name)){pfound=_tag_arr((
const struct Cyc_option*)_check_null(_untag_arr(p,sizeof(struct Cyc_option),1)),
sizeof(struct Cyc_option),1);indfound=option_index;exact=1;break;}else{if(pfound.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr){pfound=_tag_arr((const struct Cyc_option*)
_check_null(_untag_arr(p,sizeof(struct Cyc_option),1)),sizeof(struct Cyc_option),1);
indfound=option_index;}else{ambig=1;}}}}if(ambig?!exact: 0){if(print_errors)({
struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),Cyc_optind)));{struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;
_tmp2E.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmp2C[2]={&
_tmp2E,& _tmp2F};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D="%s: option `-W %s' is ambiguous\n";
_tag_arr(_tmp2D,sizeof(char),_get_zero_arr_size(_tmp2D,33));}),_tag_arr(_tmp2C,
sizeof(void*),2));}}});_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)
Cyc_strlen((struct _tagged_arr)Cyc_nextchar));Cyc_optind ++;return(int)'?';}if(
pfound.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){option_index=indfound;
if((int)*((char*)_check_unknown_subscript(nameend,sizeof(char),0))){if(((const
struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->has_arg)
Cyc_optarg=_tagged_arr_plus(nameend,sizeof(char),1);else{if(print_errors)({
struct Cyc_String_pa_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _tagged_arr)((
struct _tagged_arr)((const struct Cyc_option*)_check_unknown_subscript(pfound,
sizeof(struct Cyc_option),0))->name);{struct Cyc_String_pa_struct _tmp32;_tmp32.tag=
0;_tmp32.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*_tmp30[2]={&
_tmp32,& _tmp33};Cyc_fprintf(Cyc_stderr,({const char*_tmp31="%s: option `-W %s' doesn't allow an argument\n";
_tag_arr(_tmp31,sizeof(char),_get_zero_arr_size(_tmp31,46));}),_tag_arr(_tmp30,
sizeof(void*),2));}}});_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)
Cyc_strlen((struct _tagged_arr)Cyc_nextchar));return(int)'?';}}else{if(((const
struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->has_arg
== 1){if(Cyc_optind < argc)Cyc_optarg=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind ++));else{if(
print_errors)({struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(
argv,sizeof(struct _tagged_arr),Cyc_optind - 1)));{struct Cyc_String_pa_struct
_tmp36;_tmp36.tag=0;_tmp36.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)_check_unknown_subscript(argv,sizeof(struct _tagged_arr),0)));{void*
_tmp34[2]={& _tmp36,& _tmp37};Cyc_fprintf(Cyc_stderr,({const char*_tmp35="%s: option `%s' requires an argument\n";
_tag_arr(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,38));}),_tag_arr(_tmp34,
sizeof(void*),2));}}});_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)
Cyc_strlen((struct _tagged_arr)Cyc_nextchar));return(int)(*((const char*)
_check_unknown_subscript(optstring,sizeof(char),0))== ':'?':':'?');}}}
_tagged_arr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)Cyc_strlen((struct
_tagged_arr)Cyc_nextchar));if(longind != 0)*longind=option_index;if((unsigned int)((
const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),
0))->flag){*((int*)_check_null(((const struct Cyc_option*)_check_unknown_subscript(
pfound,sizeof(struct Cyc_option),0))->flag))=((const struct Cyc_option*)
_check_unknown_subscript(pfound,sizeof(struct Cyc_option),0))->val;return 0;}
return((const struct Cyc_option*)_check_unknown_subscript(pfound,sizeof(struct Cyc_option),
0))->val;}Cyc_nextchar=_tag_arr(0,0,0);return(int)'W';}if(*((const char*)
_check_unknown_subscript(temp,sizeof(char),1))== ':'){if(*((const char*)
_check_unknown_subscript(temp,sizeof(char),2))== ':'){if(*((char*)
_check_unknown_subscript(Cyc_nextchar,sizeof(char),0))!= '\000'){Cyc_optarg=Cyc_nextchar;
Cyc_optind ++;}else{Cyc_optarg=_tag_arr(0,0,0);}Cyc_nextchar=_tag_arr(0,0,0);}
else{if(*((char*)_check_unknown_subscript(Cyc_nextchar,sizeof(char),0))!= '\000'){
Cyc_optarg=Cyc_nextchar;Cyc_optind ++;}else{if(Cyc_optind == argc){if(print_errors)({
struct Cyc_Int_pa_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=(unsigned int)((int)c);{
struct Cyc_String_pa_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _tagged_arr)((
struct _tagged_arr)*((struct _tagged_arr*)_check_unknown_subscript(argv,sizeof(
struct _tagged_arr),0)));{void*_tmp38[2]={& _tmp3A,& _tmp3B};Cyc_fprintf(Cyc_stderr,({
const char*_tmp39="%s: option requires an argument -- %c\n";_tag_arr(_tmp39,
sizeof(char),_get_zero_arr_size(_tmp39,39));}),_tag_arr(_tmp38,sizeof(void*),2));}}});
Cyc_optopt=(int)c;if(*((const char*)_check_unknown_subscript(optstring,sizeof(
char),0))== ':')c=':';else{c='?';}}else{Cyc_optarg=*((struct _tagged_arr*)
_check_unknown_subscript(argv,sizeof(struct _tagged_arr),Cyc_optind ++));}}Cyc_nextchar=
_tag_arr(0,0,0);}}return(int)c;}}int Cyc_getopt(int argc,struct _tagged_arr argv,
struct _tagged_arr optstring){return Cyc__getopt_internal(argc,argv,optstring,(
struct _tagged_arr)_tag_arr(0,0,0),(int*)0,0);}
