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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;};struct _tagged_arr
Cyc_aprintf(struct _tagged_arr,struct _tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_printf(struct _tagged_arr,struct _tagged_arr);int
Cyc_putc(int,struct Cyc___cycFILE*);struct _tagged_arr Cyc_rprintf(struct
_RegionHandle*,struct _tagged_arr,struct _tagged_arr);int Cyc_snprintf(struct
_tagged_arr,unsigned int,struct _tagged_arr,struct _tagged_arr);int Cyc_sprintf(
struct _tagged_arr,struct _tagged_arr,struct _tagged_arr);int Cyc_vfprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);int Cyc_vprintf(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct
_tagged_arr,struct _tagged_arr);int Cyc_vsnprintf(struct _tagged_arr,unsigned int,
struct _tagged_arr,struct _tagged_arr);int Cyc_vsprintf(struct _tagged_arr,struct
_tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{int quot;
int rem;}Cyc_ldiv_t;void*abort();void exit(int);double modf(double,double*);static
struct _tagged_arr Cyc_parg2string(void*x){void*_tmp0=x;_LL1: if(*((int*)_tmp0)!= 0)
goto _LL3;_LL2: return({const char*_tmp1="string";_tag_arr(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,7));});_LL3: if(*((int*)_tmp0)!= 1)goto _LL5;_LL4: return({
const char*_tmp2="int";_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,4));});
_LL5: if(*((int*)_tmp0)!= 2)goto _LL7;_LL6: return({const char*_tmp3="double";
_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,7));});_LL7: if(*((int*)
_tmp0)!= 3)goto _LL9;_LL8: return({const char*_tmp4="short *";_tag_arr(_tmp4,
sizeof(char),_get_zero_arr_size(_tmp4,8));});_LL9: if(*((int*)_tmp0)!= 4)goto _LL0;
_LLA: return({const char*_tmp5="unsigned long *";_tag_arr(_tmp5,sizeof(char),
_get_zero_arr_size(_tmp5,16));});_LL0:;}static void*Cyc_badarg(struct _tagged_arr s){
return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp6=
_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Core_Invalid_argument_struct _tmp7;
_tmp7.tag=Cyc_Core_Invalid_argument;_tmp7.f1=s;_tmp7;});_tmp6;}));}static int Cyc_va_arg_int(
struct _tagged_arr ap){void*_tmp8=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmp9;_LLC: if(*((int*)_tmp8)!= 1)goto _LLE;_tmp9=((struct
Cyc_Int_pa_struct*)_tmp8)->f1;_LLD: return(int)_tmp9;_LLE:;_LLF: return((int(*)(
struct _tagged_arr s))Cyc_badarg)(({const char*_tmpA="printf expected int";_tag_arr(
_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,20));}));_LLB:;}static int Cyc_va_arg_long(
struct _tagged_arr ap){void*_tmpB=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmpC;_LL11: if(*((int*)_tmpB)!= 1)goto _LL13;_tmpC=((
struct Cyc_Int_pa_struct*)_tmpB)->f1;_LL12: return(int)_tmpC;_LL13:;_LL14: return((
int(*)(struct _tagged_arr s))Cyc_badarg)(({const char*_tmpD="printf expected int";
_tag_arr(_tmpD,sizeof(char),_get_zero_arr_size(_tmpD,20));}));_LL10:;}static
unsigned int Cyc_va_arg_ulong(struct _tagged_arr ap){void*_tmpE=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));unsigned int _tmpF;_LL16: if(*((int*)
_tmpE)!= 1)goto _LL18;_tmpF=((struct Cyc_Int_pa_struct*)_tmpE)->f1;_LL17: return(
unsigned int)_tmpF;_LL18:;_LL19: return((unsigned int(*)(struct _tagged_arr s))Cyc_badarg)(({
const char*_tmp10="printf expected int";_tag_arr(_tmp10,sizeof(char),
_get_zero_arr_size(_tmp10,20));}));_LL15:;}static unsigned int Cyc_va_arg_uint(
struct _tagged_arr ap){void*_tmp11=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmp12;_LL1B: if(*((int*)_tmp11)!= 1)goto _LL1D;_tmp12=((
struct Cyc_Int_pa_struct*)_tmp11)->f1;_LL1C: return(unsigned int)_tmp12;_LL1D:;
_LL1E: return((unsigned int(*)(struct _tagged_arr s))Cyc_badarg)(({const char*_tmp13="printf expected int";
_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,20));}));_LL1A:;}static
double Cyc_va_arg_double(struct _tagged_arr ap){void*_tmp14=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));double _tmp15;_LL20: if(*((int*)
_tmp14)!= 2)goto _LL22;_tmp15=((struct Cyc_Double_pa_struct*)_tmp14)->f1;_LL21:
return _tmp15;_LL22:;_LL23:(int)_throw(Cyc_badarg((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp18;_tmp18.tag=0;_tmp18.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_parg2string(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0))));{void*_tmp16[1]={& _tmp18};
Cyc_aprintf(({const char*_tmp17="printf expected double but found %s";_tag_arr(
_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,36));}),_tag_arr(_tmp16,sizeof(
void*),1));}})));_LL1F:;}static short*Cyc_va_arg_short_ptr(struct _tagged_arr ap){
void*_tmp19=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));short*_tmp1A;
_LL25: if(*((int*)_tmp19)!= 3)goto _LL27;_tmp1A=((struct Cyc_ShortPtr_pa_struct*)
_tmp19)->f1;_LL26: return _tmp1A;_LL27:;_LL28:(int)_throw(Cyc_badarg(({const char*
_tmp1B="printf expected short pointer";_tag_arr(_tmp1B,sizeof(char),
_get_zero_arr_size(_tmp1B,30));})));_LL24:;}static unsigned int*Cyc_va_arg_int_ptr(
struct _tagged_arr ap){void*_tmp1C=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int*_tmp1D;_LL2A: if(*((int*)_tmp1C)!= 4)goto _LL2C;_tmp1D=((
struct Cyc_IntPtr_pa_struct*)_tmp1C)->f1;_LL2B: return _tmp1D;_LL2C:;_LL2D:(int)
_throw(Cyc_badarg(({const char*_tmp1E="printf expected int pointer";_tag_arr(
_tmp1E,sizeof(char),_get_zero_arr_size(_tmp1E,28));})));_LL29:;}static struct
_tagged_arr Cyc_va_arg_string(struct _tagged_arr ap){void*_tmp1F=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));struct _tagged_arr _tmp20;_LL2F: if(*((
int*)_tmp1F)!= 0)goto _LL31;_tmp20=((struct Cyc_String_pa_struct*)_tmp1F)->f1;
_LL30: return _tmp20;_LL31:;_LL32:(int)_throw(Cyc_badarg(({const char*_tmp21="printf expected string";
_tag_arr(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,23));})));_LL2E:;}int Cyc___cvt_double(
double number,int prec,int flags,int*signp,int fmtch,struct _tagged_arr startp,struct
_tagged_arr endp);enum Cyc_BASE{Cyc_OCT  = 0,Cyc_DEC  = 1,Cyc_HEX  = 2};static int Cyc__IO_sputn(
int(*ioputc)(int,void*),void*ioputc_env,struct _tagged_arr s,int howmany){int i=0;
while(i < howmany){if(_get_arr_size(s,sizeof(char))== 0?1: ioputc((int)*((const
char*)_check_unknown_subscript(s,sizeof(char),0)),ioputc_env)== - 1)return i;
_tagged_arr_inplace_plus_post(& s,sizeof(char),1);i ++;}return i;}static int Cyc__IO_nzsputn(
int(*ioputc)(int,void*),void*ioputc_env,struct _tagged_arr s,int howmany){int i=0;
while(i < howmany){char c;if((_get_arr_size(s,sizeof(char))== 0?1:(c=*((const char*)
_check_unknown_subscript(s,sizeof(char),0)))== 0)?1: ioputc((int)c,ioputc_env)== 
- 1)return i;_tagged_arr_inplace_plus_post(& s,sizeof(char),1);i ++;}return i;}static
int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){int i=0;
while(i < howmany){if(ioputc((int)c,ioputc_env)== - 1)return i;i ++;}return i;}static
struct _tagged_arr Cyc_my_memchr(struct _tagged_arr s,char c,int n){int sz=(int)
_get_arr_size(s,sizeof(char));n=n < sz?n: sz;for(0;n != 0;(n --,
_tagged_arr_inplace_plus_post(& s,sizeof(char),1))){if(*((const char*)
_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return(struct
_tagged_arr)_tag_arr(0,0,0);}static struct _tagged_arr Cyc_my_nzmemchr(struct
_tagged_arr s,char c,int n){int sz=(int)_get_arr_size(s,sizeof(char));n=n < sz?n: sz;
for(0;n != 0;(n --,_tagged_arr_inplace_plus_post(& s,sizeof(char),1))){if(*((const
char*)_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return(struct
_tagged_arr)_tag_arr(0,0,0);}static unsigned int Cyc_my_strlen(struct _tagged_arr s){
unsigned int sz=_get_arr_size(s,sizeof(char));unsigned int i=0;while(i < sz?((const
char*)s.curr)[(int)i]!= 0: 0){i ++;}return i;}int Cyc__IO_vfprintf(int(*ioputc)(int,
void*),void*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap){struct
_tagged_arr fmt;register int ch;register int n;struct _tagged_arr cp=_tag_arr(0,0,0);
struct _tagged_arr cp2=(struct _tagged_arr)_tag_arr(0,0,0);struct _tagged_arr cp3=(
struct _tagged_arr)_tag_arr(0,0,0);int which_cp;struct _tagged_arr fmark;register int
flags;int ret;int width;int prec;char sign;char sign_string[2]={'\000','\000'};int
softsign=0;double _double;int fpprec;unsigned int _ulong;int dprec;int dpad;int
fieldsz;int size=0;char buf[349];{unsigned int _tmp47=348;unsigned int i;for(i=0;i < 
_tmp47;i ++){buf[i]='\000';}buf[_tmp47]=(char)0;}{char ox[2]={'\000','\000'};enum 
Cyc_BASE base;fmt=fmt0;ret=0;for(0;1;0){fmark=fmt;{unsigned int fmt_sz=
_get_arr_size(fmt,sizeof(char));for(n=0;(n < fmt_sz?(ch=(int)((const char*)fmt.curr)[
n])!= '\000': 0)?ch != '%': 0;n ++){;}fmt=_tagged_arr_plus(fmt,sizeof(char),n);if((n=(
fmt.curr - fmark.curr)/ sizeof(char))!= 0){do{if(Cyc__IO_sputn(ioputc,ioputc_env,
fmark,n)!= n)goto error;}while(0);ret +=n;}if(ch == '\000')goto done;
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1);flags=0;dprec=0;fpprec=0;
width=0;prec=- 1;sign='\000';rflag: ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));reswitch:
which_cp=0;switch(ch){case ' ': _LL33: if(!((int)sign))sign=' ';goto rflag;case '#':
_LL34: flags |=8;goto rflag;case '*': _LL35: width=Cyc_va_arg_int(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(width >= 0)goto rflag;width=
- width;goto _LL36;case '-': _LL36: flags |=16;flags &=~ 32;goto rflag;case '+': _LL37: sign='+';
goto rflag;case '.': _LL38: if((ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0)))== '*'){n=
Cyc_va_arg_int(ap);_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);prec=n < 0?
- 1: n;goto rflag;}n=0;while((unsigned int)(ch - '0')<= 9){n=10 * n + (ch - '0');ch=(
int)*((const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));}prec=n < 0?- 1: n;goto reswitch;case '0': _LL39: if(!(
flags & 16))flags |=32;goto rflag;case '1': _LL3A: goto _LL3B;case '2': _LL3B: goto _LL3C;
case '3': _LL3C: goto _LL3D;case '4': _LL3D: goto _LL3E;case '5': _LL3E: goto _LL3F;case '6':
_LL3F: goto _LL40;case '7': _LL40: goto _LL41;case '8': _LL41: goto _LL42;case '9': _LL42: n=
0;do{n=10 * n + (ch - '0');ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}while((
unsigned int)(ch - '0')<= 9);width=n;goto reswitch;case 'L': _LL43: flags |=2;goto
rflag;case 'h': _LL44: flags |=4;goto rflag;case 'l': _LL45: flags |=1;goto rflag;case 'c':
_LL46: cp=({char*_tmp22=buf;_tag_arr(_tmp22,sizeof(char),_get_zero_arr_size(
_tmp22,349));});({struct _tagged_arr _tmp23=cp;char _tmp24=*((char*)
_check_unknown_subscript(_tmp23,sizeof(char),0));char _tmp25=(char)Cyc_va_arg_int(
ap);if(_get_arr_size(_tmp23,sizeof(char))== 1?_tmp24 == '\000'?_tmp25 != '\000': 0:
0)_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);size=1;sign='\000';break;case
'D': _LL47: flags |=1;goto _LL48;case 'd': _LL48: goto _LL49;case 'i': _LL49: _ulong=(
unsigned int)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(int)((short)Cyc_va_arg_int(
ap)):(int)Cyc_va_arg_int(ap)));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);
if((int)_ulong < 0){_ulong=- _ulong;sign='-';}base=Cyc_DEC;goto number;case 'e':
_LL4A: goto _LL4B;case 'E': _LL4B: goto _LL4C;case 'f': _LL4C: goto _LL4D;case 'F': _LL4D:
goto _LL4E;case 'g': _LL4E: goto _LL4F;case 'G': _LL4F: _double=Cyc_va_arg_double(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(prec > 39){if((ch != 'g'?ch
!= 'G': 0)?1: flags & 8)fpprec=prec - 39;prec=39;}else{if(prec == - 1)prec=6;}cp=({
char*_tmp26=buf;_tag_arr(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,349));});({
struct _tagged_arr _tmp27=cp;char _tmp28=*((char*)_check_unknown_subscript(_tmp27,
sizeof(char),0));char _tmp29='\000';if(_get_arr_size(_tmp27,sizeof(char))== 1?
_tmp28 == '\000'?_tmp29 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp27.curr)=
_tmp29;});size=Cyc___cvt_double(_double,prec,flags,& softsign,ch,cp,
_tagged_arr_plus(({char*_tmp2A=buf;_tag_arr(_tmp2A,sizeof(char),
_get_zero_arr_size(_tmp2A,349));}),sizeof(char),(int)(sizeof(buf)- 1)));if(
softsign)sign='-';if(*((char*)_check_unknown_subscript(cp,sizeof(char),0))== '\000')
_tagged_arr_inplace_plus_post(& cp,sizeof(char),1);break;case 'n': _LL50: if(flags & 
1)*Cyc_va_arg_int_ptr(ap)=(unsigned int)ret;else{if(flags & 4)*Cyc_va_arg_short_ptr(
ap)=(short)ret;else{*Cyc_va_arg_int_ptr(ap)=(unsigned int)ret;}}
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);continue;case 'O': _LL51: flags |=
1;goto _LL52;case 'o': _LL52: _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(
unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)Cyc_va_arg_uint(
ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_OCT;goto nosign;
case 'p': _LL53: _ulong=(unsigned int)Cyc_va_arg_long(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_HEX;flags |=64;ch=(
int)'x';goto nosign;case 's': _LL54: {struct _tagged_arr b=Cyc_va_arg_string(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);which_cp=3;cp3=b;if(prec >= 0){
struct _tagged_arr p=Cyc_my_nzmemchr(cp3,'\000',prec);if(p.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){size=(p.curr - cp3.curr)/ sizeof(char);if(size
> prec)size=prec;}else{size=prec;}}else{size=(int)Cyc_my_strlen(cp3);}sign='\000';
break;}case 'U': _LL55: flags |=1;goto _LL56;case 'u': _LL56: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)
Cyc_va_arg_uint(ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_DEC;
goto nosign;case 'X': _LL57: goto _LL58;case 'x': _LL58: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)
Cyc_va_arg_uint(ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_HEX;
if(flags & 8?_ulong != 0: 0)flags |=64;nosign: sign='\000';number: if((dprec=prec)>= 0)
flags &=~ 32;cp=_tagged_arr_plus(({char*_tmp2B=buf;_tag_arr(_tmp2B,sizeof(char),
_get_zero_arr_size(_tmp2B,349));}),sizeof(char),(308 + 39)+ 1);if(_ulong != 0?1:
prec != 0){struct _tagged_arr xdigs;switch(base){case Cyc_OCT: _LL5A: do{({struct
_tagged_arr _tmp2C=_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char _tmp2D=*((
char*)_check_unknown_subscript(_tmp2C,sizeof(char),0));char _tmp2E=(char)((_ulong
& 7)+ '0');if(_get_arr_size(_tmp2C,sizeof(char))== 1?_tmp2D == '\000'?_tmp2E != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});_ulong >>=3;}while((int)
_ulong);if(flags & 8?*((char*)_check_unknown_subscript(cp,sizeof(char),0))!= '0':
0)({struct _tagged_arr _tmp2F=_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char
_tmp30=*((char*)_check_unknown_subscript(_tmp2F,sizeof(char),0));char _tmp31='0';
if(_get_arr_size(_tmp2F,sizeof(char))== 1?_tmp30 == '\000'?_tmp31 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});break;case Cyc_DEC: _LL5B:
while(_ulong >= 10){({struct _tagged_arr _tmp32=_tagged_arr_inplace_plus(& cp,
sizeof(char),-1);char _tmp33=*((char*)_check_unknown_subscript(_tmp32,sizeof(char),
0));char _tmp34=(char)(_ulong % 10 + '0');if(_get_arr_size(_tmp32,sizeof(char))== 1?
_tmp33 == '\000'?_tmp34 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp32.curr)=
_tmp34;});_ulong /=10;}({struct _tagged_arr _tmp35=_tagged_arr_inplace_plus(& cp,
sizeof(char),-1);char _tmp36=*((char*)_check_unknown_subscript(_tmp35,sizeof(char),
0));char _tmp37=(char)(_ulong + '0');if(_get_arr_size(_tmp35,sizeof(char))== 1?
_tmp36 == '\000'?_tmp37 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp35.curr)=
_tmp37;});break;case Cyc_HEX: _LL5C: if(ch == 'X')xdigs=({const char*_tmp38="0123456789ABCDEF";
_tag_arr(_tmp38,sizeof(char),_get_zero_arr_size(_tmp38,17));});else{xdigs=({
const char*_tmp39="0123456789abcdef";_tag_arr(_tmp39,sizeof(char),
_get_zero_arr_size(_tmp39,17));});}do{({struct _tagged_arr _tmp3A=
_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char _tmp3B=*((char*)
_check_unknown_subscript(_tmp3A,sizeof(char),0));char _tmp3C=*((const char*)
_check_unknown_subscript(xdigs,sizeof(char),(int)(_ulong & 15)));if(_get_arr_size(
_tmp3A,sizeof(char))== 1?_tmp3B == '\000'?_tmp3C != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp3A.curr)=_tmp3C;});_ulong >>=4;}while((int)_ulong);break;}}size=((
_tagged_arr_plus(({char*_tmp3D=buf;_tag_arr(_tmp3D,sizeof(char),
_get_zero_arr_size(_tmp3D,349));}),sizeof(char),(308 + 39)+ 1)).curr - cp.curr)/ 
sizeof(char);skipsize: break;default: _LL59: if(ch == '\000')goto done;cp=({char*
_tmp3E=buf;_tag_arr(_tmp3E,sizeof(char),_get_zero_arr_size(_tmp3E,349));});({
struct _tagged_arr _tmp3F=cp;char _tmp40=*((char*)_check_unknown_subscript(_tmp3F,
sizeof(char),0));char _tmp41=(char)ch;if(_get_arr_size(_tmp3F,sizeof(char))== 1?
_tmp40 == '\000'?_tmp41 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp3F.curr)=
_tmp41;});size=1;sign='\000';break;}fieldsz=size + fpprec;dpad=dprec - size;if(
dpad < 0)dpad=0;if((int)sign)fieldsz ++;else{if(flags & 64)fieldsz +=2;}fieldsz +=
dpad;if((flags & (16 | 32))== 0){if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - 
fieldsz)< width - fieldsz)goto error;}if((int)sign){({struct _tagged_arr _tmp43=
_tagged_arr_plus(({char*_tmp42=sign_string;_tag_arr(_tmp42,sizeof(char),
_get_zero_arr_size(_tmp42,2));}),sizeof(char),0);char _tmp44=*((char*)
_check_unknown_subscript(_tmp43,sizeof(char),0));char _tmp45=sign;if(
_get_arr_size(_tmp43,sizeof(char))== 1?_tmp44 == '\000'?_tmp45 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});do{if(Cyc__IO_sputn(ioputc,
ioputc_env,(struct _tagged_arr)({char*_tmp46=sign_string;_tag_arr(_tmp46,sizeof(
char),_get_zero_arr_size(_tmp46,2));}),1)!= 1)goto error;}while(0);}else{if(flags
& 64){ox[0]='0';ox[1]=(char)ch;do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,_tag_arr(
ox,sizeof(char),2),2)!= 2)goto error;}while(0);}}if((flags & (16 | 32))== 32){if(
Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz)< width - fieldsz)goto error;}
if(Cyc__IO_padn(ioputc,ioputc_env,'0',dpad)< dpad)goto error;if(which_cp == 0)do{
if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _tagged_arr)cp,size)!= size)goto error;}while(
0);else{if(which_cp == 2)do{if(Cyc__IO_sputn(ioputc,ioputc_env,cp2,size)!= size)
goto error;}while(0);else{if(which_cp == 3)do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,
cp3,size)!= size)goto error;}while(0);}}if(Cyc__IO_padn(ioputc,ioputc_env,'0',
fpprec)< fpprec)goto error;if(flags & 16){if(Cyc__IO_padn(ioputc,ioputc_env,' ',
width - fieldsz)< width - fieldsz)goto error;}ret +=width > fieldsz?width: fieldsz;}}
done: return ret;error: return - 1;}}static struct _tagged_arr Cyc_exponent(struct
_tagged_arr p,int exp,int fmtch){struct _tagged_arr t;char expbuf[309];{unsigned int
_tmp62=308;unsigned int i;for(i=0;i < _tmp62;i ++){expbuf[i]='0';}expbuf[_tmp62]=(
char)0;}({struct _tagged_arr _tmp48=_tagged_arr_inplace_plus_post(& p,sizeof(char),
1);char _tmp49=*((char*)_check_unknown_subscript(_tmp48,sizeof(char),0));char
_tmp4A=(char)fmtch;if(_get_arr_size(_tmp48,sizeof(char))== 1?_tmp49 == '\000'?
_tmp4A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp48.curr)=_tmp4A;});if(exp < 
0){exp=- exp;({struct _tagged_arr _tmp4B=_tagged_arr_inplace_plus_post(& p,sizeof(
char),1);char _tmp4C=*((char*)_check_unknown_subscript(_tmp4B,sizeof(char),0));
char _tmp4D='-';if(_get_arr_size(_tmp4B,sizeof(char))== 1?_tmp4C == '\000'?_tmp4D
!= '\000': 0: 0)_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});}else{({
struct _tagged_arr _tmp4E=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char
_tmp4F=*((char*)_check_unknown_subscript(_tmp4E,sizeof(char),0));char _tmp50='+';
if(_get_arr_size(_tmp4E,sizeof(char))== 1?_tmp4F == '\000'?_tmp50 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4E.curr)=_tmp50;});}t=_tagged_arr_plus(({char*
_tmp51=expbuf;_tag_arr(_tmp51,sizeof(char),_get_zero_arr_size(_tmp51,309));}),
sizeof(char),308);if(exp > 9){do{({struct _tagged_arr _tmp52=
_tagged_arr_inplace_plus(& t,sizeof(char),-1);char _tmp53=*((char*)
_check_unknown_subscript(_tmp52,sizeof(char),0));char _tmp54=(char)(exp % 10 + '0');
if(_get_arr_size(_tmp52,sizeof(char))== 1?_tmp53 == '\000'?_tmp54 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});}while((exp /=10)> 9);({
struct _tagged_arr _tmp55=_tagged_arr_inplace_plus(& t,sizeof(char),-1);char _tmp56=*((
char*)_check_unknown_subscript(_tmp55,sizeof(char),0));char _tmp57=(char)(exp + '0');
if(_get_arr_size(_tmp55,sizeof(char))== 1?_tmp56 == '\000'?_tmp57 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});for(0;t.curr < (
_tagged_arr_plus(({char*_tmp58=expbuf;_tag_arr(_tmp58,sizeof(char),
_get_zero_arr_size(_tmp58,309));}),sizeof(char),308)).curr;({struct _tagged_arr
_tmp59=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp5A=*((char*)
_check_unknown_subscript(_tmp59,sizeof(char),0));char _tmp5B=*((char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(char),1),
sizeof(char),0));if(_get_arr_size(_tmp59,sizeof(char))== 1?_tmp5A == '\000'?
_tmp5B != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;})){;}}
else{({struct _tagged_arr _tmp5C=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);
char _tmp5D=*((char*)_check_unknown_subscript(_tmp5C,sizeof(char),0));char _tmp5E='0';
if(_get_arr_size(_tmp5C,sizeof(char))== 1?_tmp5D == '\000'?_tmp5E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});({struct _tagged_arr _tmp5F=
_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp60=*((char*)
_check_unknown_subscript(_tmp5F,sizeof(char),0));char _tmp61=(char)(exp + '0');if(
_get_arr_size(_tmp5F,sizeof(char))== 1?_tmp60 == '\000'?_tmp61 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});}return p;}static struct
_tagged_arr Cyc_round(double fract,int*exp,struct _tagged_arr start,struct
_tagged_arr end,char ch,int*signp){double tmp=(double)0.0;if(fract != 0.0)modf(fract
* 10,& tmp);else{tmp=(double)(ch - '0');}if(tmp > 4)for(0;1;
_tagged_arr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_unknown_subscript(end,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
end,sizeof(char),-1);if(({struct _tagged_arr _tmp63=end;char _tmp64=*((char*)
_check_unknown_subscript(_tmp63,sizeof(char),0));char _tmp65=_tmp64 + '\001';if(
_get_arr_size(_tmp63,sizeof(char))== 1?_tmp64 == '\000'?_tmp65 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp63.curr)=_tmp65;})<= '9')break;({struct
_tagged_arr _tmp66=end;char _tmp67=*((char*)_check_unknown_subscript(_tmp66,
sizeof(char),0));char _tmp68='0';if(_get_arr_size(_tmp66,sizeof(char))== 1?_tmp67
== '\000'?_tmp68 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp66.curr)=_tmp68;});
if(end.curr == start.curr){if((unsigned int)exp){({struct _tagged_arr _tmp69=end;
char _tmp6A=*((char*)_check_unknown_subscript(_tmp69,sizeof(char),0));char _tmp6B='1';
if(_get_arr_size(_tmp69,sizeof(char))== 1?_tmp6A == '\000'?_tmp6B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp69.curr)=_tmp6B;});++(*exp);}else{({struct
_tagged_arr _tmp6C=_tagged_arr_inplace_plus(& end,sizeof(char),-1);char _tmp6D=*((
char*)_check_unknown_subscript(_tmp6C,sizeof(char),0));char _tmp6E='1';if(
_get_arr_size(_tmp6C,sizeof(char))== 1?_tmp6D == '\000'?_tmp6E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp6C.curr)=_tmp6E;});_tagged_arr_inplace_plus(&
start,sizeof(char),-1);}break;}}else{if(*signp == '-')for(0;1;
_tagged_arr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_unknown_subscript(end,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
end,sizeof(char),-1);if(*((char*)_check_unknown_subscript(end,sizeof(char),0))!= '0')
break;if(end.curr == start.curr)*signp=0;}}return start;}int Cyc___cvt_double(
double number,int prec,int flags,int*signp,int fmtch,struct _tagged_arr startp,struct
_tagged_arr endp){struct _tagged_arr p;struct _tagged_arr t;register double fract;int
dotrim=0;int expcnt;int gformat=0;double integer=(double)0.0;double tmp=(double)0.0;
expcnt=0;if(number < 0){number=- number;*signp=(int)'-';}else{*signp=0;}fract=modf(
number,& integer);t=_tagged_arr_inplace_plus(& startp,sizeof(char),1);for(p=
_tagged_arr_plus(endp,sizeof(char),- 1);p.curr >= startp.curr?integer != 0.0: 0;++
expcnt){tmp=modf(integer / 10,& integer);({struct _tagged_arr _tmp6F=
_tagged_arr_inplace_plus_post(& p,sizeof(char),-1);char _tmp70=*((char*)
_check_unknown_subscript(_tmp6F,sizeof(char),0));char _tmp71=(char)((int)((tmp + 
.01)* 10)+ '0');if(_get_arr_size(_tmp6F,sizeof(char))== 1?_tmp70 == '\000'?_tmp71
!= '\000': 0: 0)_throw_arraybounds();*((char*)_tmp6F.curr)=_tmp71;});}switch(fmtch){
case 'f': _LL5F: goto _LL60;case 'F': _LL60: if(expcnt)for(0;(_tagged_arr_inplace_plus(&
p,sizeof(char),1)).curr < endp.curr;({struct _tagged_arr _tmp72=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp73=*((char*)
_check_unknown_subscript(_tmp72,sizeof(char),0));char _tmp74=*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(_tmp72,sizeof(char))
== 1?_tmp73 == '\000'?_tmp74 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp72.curr)=
_tmp74;})){;}else{({struct _tagged_arr _tmp75=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp76=*((char*)_check_unknown_subscript(_tmp75,sizeof(char),
0));char _tmp77='0';if(_get_arr_size(_tmp75,sizeof(char))== 1?_tmp76 == '\000'?
_tmp77 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp75.curr)=_tmp77;});}if(
prec?1: flags & 8)({struct _tagged_arr _tmp78=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp79=*((char*)_check_unknown_subscript(_tmp78,sizeof(char),
0));char _tmp7A='.';if(_get_arr_size(_tmp78,sizeof(char))== 1?_tmp79 == '\000'?
_tmp7A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp78.curr)=_tmp7A;});if(
fract != 0.0){if(prec)do{fract=modf(fract * 10,& tmp);({struct _tagged_arr _tmp7B=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp7C=*((char*)
_check_unknown_subscript(_tmp7B,sizeof(char),0));char _tmp7D=(char)((int)tmp + '0');
if(_get_arr_size(_tmp7B,sizeof(char))== 1?_tmp7C == '\000'?_tmp7D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp7B.curr)=_tmp7D;});}while(-- prec?fract != 0.0: 0);
if(fract != 0.0)startp=Cyc_round(fract,0,startp,_tagged_arr_plus(t,sizeof(char),-
1),(char)0,signp);}for(0;prec --;({struct _tagged_arr _tmp7E=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp7F=*((char*)
_check_unknown_subscript(_tmp7E,sizeof(char),0));char _tmp80='0';if(_get_arr_size(
_tmp7E,sizeof(char))== 1?_tmp7F == '\000'?_tmp80 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp7E.curr)=_tmp80;})){;}break;case 'e': _LL61: goto _LL62;case 'E': _LL62:
eformat: if(expcnt){({struct _tagged_arr _tmp81=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp82=*((char*)_check_unknown_subscript(_tmp81,sizeof(char),
0));char _tmp83=*((char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& p,
sizeof(char),1),sizeof(char),0));if(_get_arr_size(_tmp81,sizeof(char))== 1?
_tmp82 == '\000'?_tmp83 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp81.curr)=
_tmp83;});if(prec?1: flags & 8)({struct _tagged_arr _tmp84=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp85=*((char*)
_check_unknown_subscript(_tmp84,sizeof(char),0));char _tmp86='.';if(_get_arr_size(
_tmp84,sizeof(char))== 1?_tmp85 == '\000'?_tmp86 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp84.curr)=_tmp86;});for(0;prec?(_tagged_arr_inplace_plus(& p,sizeof(char),
1)).curr < endp.curr: 0;-- prec){({struct _tagged_arr _tmp87=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp88=*((char*)
_check_unknown_subscript(_tmp87,sizeof(char),0));char _tmp89=*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(_tmp87,sizeof(char))
== 1?_tmp88 == '\000'?_tmp89 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp87.curr)=
_tmp89;});}if(!prec?(_tagged_arr_inplace_plus(& p,sizeof(char),1)).curr < endp.curr:
0){fract=(double)0;startp=Cyc_round((double)0,(int*)& expcnt,startp,
_tagged_arr_plus(t,sizeof(char),- 1),*((char*)_check_unknown_subscript(p,sizeof(
char),0)),signp);}-- expcnt;}else{if(fract != 0.0){for(expcnt=- 1;1;-- expcnt){fract=
modf(fract * 10,& tmp);if(tmp != 0.0)break;}({struct _tagged_arr _tmp8A=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp8B=*((char*)
_check_unknown_subscript(_tmp8A,sizeof(char),0));char _tmp8C=(char)((int)tmp + '0');
if(_get_arr_size(_tmp8A,sizeof(char))== 1?_tmp8B == '\000'?_tmp8C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp8A.curr)=_tmp8C;});if(prec?1: flags & 8)({struct
_tagged_arr _tmp8D=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp8E=*((
char*)_check_unknown_subscript(_tmp8D,sizeof(char),0));char _tmp8F='.';if(
_get_arr_size(_tmp8D,sizeof(char))== 1?_tmp8E == '\000'?_tmp8F != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp8D.curr)=_tmp8F;});}else{({struct _tagged_arr
_tmp90=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp91=*((char*)
_check_unknown_subscript(_tmp90,sizeof(char),0));char _tmp92='0';if(_get_arr_size(
_tmp90,sizeof(char))== 1?_tmp91 == '\000'?_tmp92 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp90.curr)=_tmp92;});if(prec?1: flags & 8)({struct _tagged_arr _tmp93=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp94=*((char*)
_check_unknown_subscript(_tmp93,sizeof(char),0));char _tmp95='.';if(_get_arr_size(
_tmp93,sizeof(char))== 1?_tmp94 == '\000'?_tmp95 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp93.curr)=_tmp95;});}}if(fract != 0.0){if(prec)do{fract=modf(fract * 10,&
tmp);({struct _tagged_arr _tmp96=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);
char _tmp97=*((char*)_check_unknown_subscript(_tmp96,sizeof(char),0));char _tmp98=(
char)((int)tmp + '0');if(_get_arr_size(_tmp96,sizeof(char))== 1?_tmp97 == '\000'?
_tmp98 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp96.curr)=_tmp98;});}while(
-- prec?fract != 0.0: 0);if(fract != 0.0)startp=Cyc_round(fract,(int*)& expcnt,startp,
_tagged_arr_plus(t,sizeof(char),- 1),(char)0,signp);}for(0;prec --;({struct
_tagged_arr _tmp99=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp9A=*((
char*)_check_unknown_subscript(_tmp99,sizeof(char),0));char _tmp9B='0';if(
_get_arr_size(_tmp99,sizeof(char))== 1?_tmp9A == '\000'?_tmp9B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp99.curr)=_tmp9B;})){;}if(gformat?!(flags & 8): 0){
while(t.curr > startp.curr?*((char*)_check_unknown_subscript(
_tagged_arr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0': 0){;}if(*((
char*)_check_unknown_subscript(t,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
t,sizeof(char),-1);_tagged_arr_inplace_plus(& t,sizeof(char),1);}t=Cyc_exponent(t,
expcnt,fmtch);break;case 'g': _LL63: goto _LL64;case 'G': _LL64: if(!prec)++ prec;if(
expcnt > prec?1:((!expcnt?fract != 0.0: 0)?fract < .0001: 0)){-- prec;fmtch -=2;gformat=
1;goto eformat;}if(expcnt)for(0;(_tagged_arr_inplace_plus(& p,sizeof(char),1)).curr
< endp.curr;(({struct _tagged_arr _tmp9C=_tagged_arr_inplace_plus_post(& t,sizeof(
char),1);char _tmp9D=*((char*)_check_unknown_subscript(_tmp9C,sizeof(char),0));
char _tmp9E=*((char*)_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(
_tmp9C,sizeof(char))== 1?_tmp9D == '\000'?_tmp9E != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp9C.curr)=_tmp9E;}),-- prec)){;}else{({struct _tagged_arr _tmp9F=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpA0=*((char*)
_check_unknown_subscript(_tmp9F,sizeof(char),0));char _tmpA1='0';if(_get_arr_size(
_tmp9F,sizeof(char))== 1?_tmpA0 == '\000'?_tmpA1 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp9F.curr)=_tmpA1;});}if(prec?1: flags & 8){dotrim=1;({struct _tagged_arr
_tmpA2=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpA3=*((char*)
_check_unknown_subscript(_tmpA2,sizeof(char),0));char _tmpA4='.';if(_get_arr_size(
_tmpA2,sizeof(char))== 1?_tmpA3 == '\000'?_tmpA4 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpA2.curr)=_tmpA4;});}else{dotrim=0;}if(fract != 0.0){if(prec){do{fract=
modf(fract * 10,& tmp);({struct _tagged_arr _tmpA5=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmpA6=*((char*)_check_unknown_subscript(_tmpA5,sizeof(char),
0));char _tmpA7=(char)((int)tmp + '0');if(_get_arr_size(_tmpA5,sizeof(char))== 1?
_tmpA6 == '\000'?_tmpA7 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpA5.curr)=
_tmpA7;});}while(tmp == 0.0?!expcnt: 0);while(-- prec?fract != 0.0: 0){fract=modf(
fract * 10,& tmp);({struct _tagged_arr _tmpA8=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmpA9=*((char*)_check_unknown_subscript(_tmpA8,sizeof(char),
0));char _tmpAA=(char)((int)tmp + '0');if(_get_arr_size(_tmpA8,sizeof(char))== 1?
_tmpA9 == '\000'?_tmpAA != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpA8.curr)=
_tmpAA;});}}if(fract != 0.0)startp=Cyc_round(fract,0,startp,_tagged_arr_plus(t,
sizeof(char),- 1),(char)0,signp);}if(flags & 8)for(0;prec --;({struct _tagged_arr
_tmpAB=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpAC=*((char*)
_check_unknown_subscript(_tmpAB,sizeof(char),0));char _tmpAD='0';if(_get_arr_size(
_tmpAB,sizeof(char))== 1?_tmpAC == '\000'?_tmpAD != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpAB.curr)=_tmpAD;})){;}else{if(dotrim){while(t.curr > startp.curr?*((
char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& t,sizeof(char),-1),
sizeof(char),0))== '0': 0){;}if(*((char*)_check_unknown_subscript(t,sizeof(char),
0))!= '.')_tagged_arr_inplace_plus(& t,sizeof(char),1);}}break;default: _LL65:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));
_tmpAE[0]=({struct Cyc_Core_Impossible_struct _tmpAF;_tmpAF.tag=Cyc_Core_Impossible;
_tmpAF.f1=({const char*_tmpB0="__cvt_double";_tag_arr(_tmpB0,sizeof(char),
_get_zero_arr_size(_tmpB0,13));});_tmpAF;});_tmpAE;}));}return(t.curr - startp.curr)
/ sizeof(char);}int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _tagged_arr fmt,
struct _tagged_arr ap){int ans;ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),
struct Cyc___cycFILE*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))Cyc__IO_vfprintf)(
Cyc_putc,f,fmt,ap);return ans;}int Cyc_fprintf(struct Cyc___cycFILE*f,struct
_tagged_arr fmt,struct _tagged_arr ap){return Cyc_vfprintf(f,fmt,ap);}int Cyc_vprintf(
struct _tagged_arr fmt,struct _tagged_arr ap){int ans;ans=((int(*)(int(*ioputc)(int,
struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _tagged_arr fmt0,
struct _tagged_arr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);return ans;}
int Cyc_printf(struct _tagged_arr fmt,struct _tagged_arr ap){int ans;ans=Cyc_vprintf(
fmt,ap);return ans;}struct _tuple0{struct _tagged_arr*f1;unsigned int*f2;};static
int Cyc_putc_string(int c,struct _tuple0*sptr_n){struct _tuple0 _tmpB2;struct
_tagged_arr*_tmpB3;unsigned int*_tmpB4;struct _tuple0*_tmpB1=sptr_n;_tmpB2=*
_tmpB1;_tmpB3=_tmpB2.f1;_tmpB4=_tmpB2.f2;{struct _tagged_arr s=*_tmpB3;
unsigned int n=*_tmpB4;if(_get_arr_size(s,sizeof(char))== 0?1: n == 0)return - 1;*((
char*)_check_unknown_subscript(s,sizeof(char),0))=(char)c;*_tmpB3=
_tagged_arr_plus(s,sizeof(char),1);*_tmpB4=n - 1;return 1;}}int Cyc_vsnprintf(
struct _tagged_arr s,unsigned int n,struct _tagged_arr fmt,struct _tagged_arr ap){int
ans;struct _tagged_arr _tmpB5=s;unsigned int _tmpB6=n;struct _tuple0 _tmpB7=({struct
_tuple0 _tmpB8;_tmpB8.f1=& _tmpB5;_tmpB8.f2=& _tmpB6;_tmpB8;});ans=((int(*)(int(*
ioputc)(int,struct _tuple0*),struct _tuple0*ioputc_env,struct _tagged_arr fmt0,
struct _tagged_arr ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpB7,fmt,ap);if(0 <= 
ans)*((char*)_check_unknown_subscript(s,sizeof(char),ans))='\000';return ans;}int
Cyc_snprintf(struct _tagged_arr s,unsigned int n,struct _tagged_arr fmt,struct
_tagged_arr ap){return Cyc_vsnprintf(s,n,fmt,ap);}int Cyc_vsprintf(struct
_tagged_arr s,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_vsnprintf(s,
_get_arr_size(s,sizeof(char)),fmt,ap);}int Cyc_sprintf(struct _tagged_arr s,struct
_tagged_arr fmt,struct _tagged_arr ap){return Cyc_vsnprintf(s,_get_arr_size(s,
sizeof(char)),fmt,ap);}static int Cyc_putc_void(int c,int dummy){return 1;}struct
_tagged_arr Cyc_vrprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,struct
_tagged_arr ap){int size=((int(*)(int(*ioputc)(int,int),int ioputc_env,struct
_tagged_arr fmt0,struct _tagged_arr ap))Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 
1;struct _tagged_arr s=({unsigned int _tmpB9=(unsigned int)size;char*_tmpBA=(char*)
_region_malloc(r1,_check_times(sizeof(char),_tmpB9 + 1));struct _tagged_arr _tmpBC=
_tag_arr(_tmpBA,sizeof(char),_tmpB9 + 1);{unsigned int _tmpBB=_tmpB9;unsigned int i;
for(i=0;i < _tmpBB;i ++){_tmpBA[i]='\000';}_tmpBA[_tmpBB]=(char)0;}_tmpBC;});Cyc_vsprintf(
_tagged_ptr_decrease_size(s,sizeof(char),1),fmt,ap);return s;}struct _tagged_arr
Cyc_rprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap){
return Cyc_vrprintf(r1,fmt,ap);}struct _tagged_arr Cyc_aprintf(struct _tagged_arr fmt,
struct _tagged_arr ap){return Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
