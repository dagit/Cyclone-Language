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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct _tagged_arr s,int w);
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(
struct _tagged_arr);struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _tagged_arr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[
14];struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;
int q_restrict: 1;int real_const: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;};struct
Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{
int tag;struct Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*
tunion_info;struct Cyc_List_List*targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0*tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{
int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Conref*f2;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple2 condition;
struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Reference_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _tagged_arr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(void*);
struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct _tagged_arr
Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);
struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);struct _tagged_arr Cyc_Absynpp_prim2string(void*p);struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*
Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_char_escape(char);struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple6{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7
Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,
void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _tagged_arr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _tagged_arr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_tagged_arr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: return({
const char*_tmp2="\\a";_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,3));});
case '\b': _LL1: return({const char*_tmp3="\\b";_tag_arr(_tmp3,sizeof(char),
_get_zero_arr_size(_tmp3,3));});case '\f': _LL2: return({const char*_tmp4="\\f";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,3));});case '\n': _LL3: return({
const char*_tmp5="\\n";_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,3));});
case '\r': _LL4: return({const char*_tmp6="\\r";_tag_arr(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,3));});case '\t': _LL5: return({const char*_tmp7="\\t";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,3));});case '\v': _LL6: return({
const char*_tmp8="\\v";_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,3));});
case '\\': _LL7: return({const char*_tmp9="\\\\";_tag_arr(_tmp9,sizeof(char),
_get_zero_arr_size(_tmp9,3));});case '"': _LL8: return({const char*_tmpA="\"";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,2));});case '\'': _LL9: return({
const char*_tmpB="\\'";_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,3));});
default: _LLA: if(c >= ' '?c <= '~': 0){struct _tagged_arr _tmpC=Cyc_Core_new_string(2);({
struct _tagged_arr _tmpD=_tagged_arr_plus(_tmpC,sizeof(char),0);char _tmpE=*((char*)
_check_unknown_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(_get_arr_size(
_tmpD,sizeof(char))== 1?_tmpE == '\000'?_tmpF != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpD.curr)=_tmpF;});return(struct _tagged_arr)_tmpC;}else{struct
_tagged_arr _tmp10=Cyc_Core_new_string(5);int j=0;({struct _tagged_arr _tmp11=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((char*)
_check_unknown_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_arr_size(_tmp11,sizeof(char))== 1?_tmp12 == '\000'?_tmp13 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _tagged_arr _tmp14=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_unknown_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_arr_size(_tmp14,sizeof(char))== 1?_tmp15 == '\000'?
_tmp16 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp14.curr)=_tmp16;});({
struct _tagged_arr _tmp17=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp18=*((
char*)_check_unknown_subscript(_tmp17,sizeof(char),0));char _tmp19=(char)('0' + (c
>> 3 & 7));if(_get_arr_size(_tmp17,sizeof(char))== 1?_tmp18 == '\000'?_tmp19 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp17.curr)=_tmp19;});({struct _tagged_arr
_tmp1A=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp1B=*((char*)
_check_unknown_subscript(_tmp1A,sizeof(char),0));char _tmp1C=(char)('0' + (c & 7));
if(_get_arr_size(_tmp1A,sizeof(char))== 1?_tmp1B == '\000'?_tmp1C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1A.curr)=_tmp1C;});return(struct _tagged_arr)
_tmp10;}}}static int Cyc_Absynpp_special(struct _tagged_arr s){int sz=(int)(
_get_arr_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;i ++){char c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(((c <= ' '?1: c >= '~')?1: c == '"')?1:
c == '\\')return 1;}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_arr_size(s,
sizeof(char))- 1);if(n > 0?*((const char*)_check_unknown_subscript(s,sizeof(char),
n))== '\000': 0)n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE:
goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '?_tmp1D <= '~': 0)len ++;else{len +=4;}goto _LLC;
_LLC:;}}{struct _tagged_arr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{
int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:({struct _tagged_arr
_tmp1F=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp20=*((char*)
_check_unknown_subscript(_tmp1F,sizeof(char),0));char _tmp21='\\';if(
_get_arr_size(_tmp1F,sizeof(char))== 1?_tmp20 == '\000'?_tmp21 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({struct _tagged_arr _tmp22=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp23=*((char*)
_check_unknown_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(_get_arr_size(
_tmp22,sizeof(char))== 1?_tmp23 == '\000'?_tmp24 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:({
struct _tagged_arr _tmp25=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp26=*((char*)
_check_unknown_subscript(_tmp25,sizeof(char),0));char _tmp27='\\';if(
_get_arr_size(_tmp25,sizeof(char))== 1?_tmp26 == '\000'?_tmp27 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({struct _tagged_arr _tmp28=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp29=*((char*)
_check_unknown_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(_get_arr_size(
_tmp28,sizeof(char))== 1?_tmp29 == '\000'?_tmp2A != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:({
struct _tagged_arr _tmp2B=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp2C=*((char*)
_check_unknown_subscript(_tmp2B,sizeof(char),0));char _tmp2D='\\';if(
_get_arr_size(_tmp2B,sizeof(char))== 1?_tmp2C == '\000'?_tmp2D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({struct _tagged_arr _tmp2E=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp2F=*((char*)
_check_unknown_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(_get_arr_size(
_tmp2E,sizeof(char))== 1?_tmp2F == '\000'?_tmp30 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:({
struct _tagged_arr _tmp31=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp32=*((char*)
_check_unknown_subscript(_tmp31,sizeof(char),0));char _tmp33='\\';if(
_get_arr_size(_tmp31,sizeof(char))== 1?_tmp32 == '\000'?_tmp33 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({struct _tagged_arr _tmp34=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp35=*((char*)
_check_unknown_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(_get_arr_size(
_tmp34,sizeof(char))== 1?_tmp35 == '\000'?_tmp36 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:({
struct _tagged_arr _tmp37=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp38=*((char*)
_check_unknown_subscript(_tmp37,sizeof(char),0));char _tmp39='\\';if(
_get_arr_size(_tmp37,sizeof(char))== 1?_tmp38 == '\000'?_tmp39 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({struct _tagged_arr _tmp3A=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp3B=*((char*)
_check_unknown_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(_get_arr_size(
_tmp3A,sizeof(char))== 1?_tmp3B == '\000'?_tmp3C != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:({
struct _tagged_arr _tmp3D=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp3E=*((char*)
_check_unknown_subscript(_tmp3D,sizeof(char),0));char _tmp3F='\\';if(
_get_arr_size(_tmp3D,sizeof(char))== 1?_tmp3E == '\000'?_tmp3F != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({struct _tagged_arr _tmp40=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp41=*((char*)
_check_unknown_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(_get_arr_size(
_tmp40,sizeof(char))== 1?_tmp41 == '\000'?_tmp42 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:({
struct _tagged_arr _tmp43=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp44=*((char*)
_check_unknown_subscript(_tmp43,sizeof(char),0));char _tmp45='\\';if(
_get_arr_size(_tmp43,sizeof(char))== 1?_tmp44 == '\000'?_tmp45 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({struct _tagged_arr _tmp46=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp47=*((char*)
_check_unknown_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(_get_arr_size(
_tmp46,sizeof(char))== 1?_tmp47 == '\000'?_tmp48 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:({
struct _tagged_arr _tmp49=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp4A=*((char*)
_check_unknown_subscript(_tmp49,sizeof(char),0));char _tmp4B='\\';if(
_get_arr_size(_tmp49,sizeof(char))== 1?_tmp4A == '\000'?_tmp4B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({struct _tagged_arr _tmp4C=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp4D=*((char*)
_check_unknown_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_arr_size(_tmp4C,sizeof(char))== 1?_tmp4D == '\000'?_tmp4E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _tagged_arr _tmp4F=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp50=*((char*)_check_unknown_subscript(_tmp4F,sizeof(char),0));char _tmp51='\\';
if(_get_arr_size(_tmp4F,sizeof(char))== 1?_tmp50 == '\000'?_tmp51 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({struct _tagged_arr _tmp52=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp53=*((char*)
_check_unknown_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(_get_arr_size(
_tmp52,sizeof(char))== 1?_tmp53 == '\000'?_tmp54 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '?_tmp1E <= '~':
0)({struct _tagged_arr _tmp55=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp56=*((
char*)_check_unknown_subscript(_tmp55,sizeof(char),0));char _tmp57=_tmp1E;if(
_get_arr_size(_tmp55,sizeof(char))== 1?_tmp56 == '\000'?_tmp57 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});else{({struct _tagged_arr
_tmp58=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp59=*((char*)
_check_unknown_subscript(_tmp58,sizeof(char),0));char _tmp5A='\\';if(
_get_arr_size(_tmp58,sizeof(char))== 1?_tmp59 == '\000'?_tmp5A != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});({struct _tagged_arr _tmp5B=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5C=*((char*)
_check_unknown_subscript(_tmp5B,sizeof(char),0));char _tmp5D=(char)('0' + (_tmp1E
>> 6 & 7));if(_get_arr_size(_tmp5B,sizeof(char))== 1?_tmp5C == '\000'?_tmp5D != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});({struct _tagged_arr
_tmp5E=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5F=*((char*)
_check_unknown_subscript(_tmp5E,sizeof(char),0));char _tmp60=(char)('0' + (_tmp1E
>> 3 & 7));if(_get_arr_size(_tmp5E,sizeof(char))== 1?_tmp5F == '\000'?_tmp60 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});({struct _tagged_arr
_tmp61=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp62=*((char*)
_check_unknown_subscript(_tmp61,sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 
7));if(_get_arr_size(_tmp61,sizeof(char))== 1?_tmp62 == '\000'?_tmp63 != '\000': 0:
0)_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}
return(struct _tagged_arr)t;}}}}static char _tmp64[9]="restrict";static struct
_tagged_arr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _tagged_arr Cyc_Absynpp_volatile_string={_tmp65,
_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={
_tmp66,_tmp66,_tmp66 + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_arr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict)
l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=Cyc_Absynpp_restrict_sp;
_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({struct Cyc_List_List*_tmp68=_cycalloc(
sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;_tmp68->tl=l;_tmp68;});if(tq.print_const)
l=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;
_tmp69->tl=l;_tmp69;});return Cyc_PP_egroup(({const char*_tmp6A="";_tag_arr(_tmp6A,
sizeof(char),_get_zero_arr_size(_tmp6A,1));}),({const char*_tmp6B=" ";_tag_arr(
_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,2));}),({const char*_tmp6C=" ";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _tagged_arr Cyc_Absynpp_kind2string(void*k){void*_tmp6D=
k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";_tag_arr(
_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,2));});_LL39: if((int)_tmp6D != 1)
goto _LL3B;_LL3A: return({const char*_tmp6F="M";_tag_arr(_tmp6F,sizeof(char),
_get_zero_arr_size(_tmp6F,2));});_LL3B: if((int)_tmp6D != 2)goto _LL3D;_LL3C: return({
const char*_tmp70="B";_tag_arr(_tmp70,sizeof(char),_get_zero_arr_size(_tmp70,2));});
_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({const char*_tmp71="R";_tag_arr(
_tmp71,sizeof(char),_get_zero_arr_size(_tmp71,2));});_LL3F: if((int)_tmp6D != 4)
goto _LL41;_LL40: return({const char*_tmp72="E";_tag_arr(_tmp72,sizeof(char),
_get_zero_arr_size(_tmp72,2));});_LL41: if((int)_tmp6D != 5)goto _LL36;_LL42: return({
const char*_tmp73="I";_tag_arr(_tmp73,sizeof(char),_get_zero_arr_size(_tmp73,2));});
_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _tagged_arr Cyc_Absynpp_kindbound2string(void*c){void*_tmp74=Cyc_Absyn_compress_kb(
c);void*_tmp75;void*_tmp76;_LL44: if(*((int*)_tmp74)!= 0)goto _LL46;_tmp75=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp74)->f1;_LL45: return Cyc_Absynpp_kind2string(
_tmp75);_LL46: if(*((int*)_tmp74)!= 1)goto _LL48;_LL47: if(Cyc_PP_tex_output)return({
const char*_tmp77="{?}";_tag_arr(_tmp77,sizeof(char),_get_zero_arr_size(_tmp77,4));});
else{return({const char*_tmp78="?";_tag_arr(_tmp78,sizeof(char),
_get_zero_arr_size(_tmp78,2));});}_LL48: if(*((int*)_tmp74)!= 2)goto _LL43;_tmp76=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp74)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmp76);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp79=
Cyc_Absyn_compress_kb(c);void*_tmp7A;void*_tmp7B;_LL4B: if(*((int*)_tmp79)!= 0)
goto _LL4D;_tmp7A=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp79)->f1;_LL4C: return
Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7A));_LL4D: if(*((int*)_tmp79)!= 1)goto
_LL4F;_LL4E: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({const char*_tmp7C="{?}";
_tag_arr(_tmp7C,sizeof(char),_get_zero_arr_size(_tmp7C,4));}),1);else{return Cyc_PP_text(({
const char*_tmp7D="?";_tag_arr(_tmp7D,sizeof(char),_get_zero_arr_size(_tmp7D,2));}));}
_LL4F: if(*((int*)_tmp79)!= 2)goto _LL4A;_tmp7B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp79)->f2;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7B));_LL4A:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){return Cyc_PP_egroup(({
const char*_tmp7E="<";_tag_arr(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,2));}),({
const char*_tmp7F=">";_tag_arr(_tmp7F,sizeof(char),_get_zero_arr_size(_tmp7F,2));}),({
const char*_tmp80=",";_tag_arr(_tmp80,sizeof(char),_get_zero_arr_size(_tmp80,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp81=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp82;void*_tmp83;
void*_tmp84;_LL52: if(*((int*)_tmp81)!= 1)goto _LL54;_LL53: goto _LL55;_LL54: if(*((
int*)_tmp81)!= 0)goto _LL56;_tmp82=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;
if((int)_tmp82 != 2)goto _LL56;_LL55: return Cyc_PP_textptr(tv->name);_LL56: if(*((
int*)_tmp81)!= 2)goto _LL58;_tmp83=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp81)->f2;_LL57: _tmp84=_tmp83;goto _LL59;_LL58: if(*((int*)_tmp81)!= 0)goto _LL51;
_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp81)->f1;_LL59: return({struct
Cyc_PP_Doc*_tmp85[3];_tmp85[2]=Cyc_Absynpp_kind2doc(_tmp84);_tmp85[1]=Cyc_PP_text(({
const char*_tmp86="::";_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,3));}));
_tmp85[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_arr(_tmp85,sizeof(struct Cyc_PP_Doc*),
3));});_LL51:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return Cyc_PP_egroup(({const char*_tmp87="<";_tag_arr(_tmp87,sizeof(char),
_get_zero_arr_size(_tmp87,2));}),({const char*_tmp88=">";_tag_arr(_tmp88,sizeof(
char),_get_zero_arr_size(_tmp88,2));}),({const char*_tmp89=",";_tag_arr(_tmp89,
sizeof(char),_get_zero_arr_size(_tmp89,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({
const char*_tmp8A="<";_tag_arr(_tmp8A,sizeof(char),_get_zero_arr_size(_tmp8A,2));}),({
const char*_tmp8B=">";_tag_arr(_tmp8B,sizeof(char),_get_zero_arr_size(_tmp8B,2));}),({
const char*_tmp8C=",";_tag_arr(_tmp8C,sizeof(char),_get_zero_arr_size(_tmp8C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){return Cyc_PP_group(({const char*
_tmp8D="(";_tag_arr(_tmp8D,sizeof(char),_get_zero_arr_size(_tmp8D,2));}),({const
char*_tmp8E=")";_tag_arr(_tmp8E,sizeof(char),_get_zero_arr_size(_tmp8E,2));}),({
const char*_tmp8F=",";_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp90=att;_LL5B: if((int)_tmp90 != 0)goto _LL5D;_LL5C: goto _LL5E;
_LL5D: if((int)_tmp90 != 1)goto _LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp90 != 2)goto
_LL61;_LL60: return Cyc_PP_nil_doc();_LL61:;_LL62: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5A:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp91=(void*)atts->hd;_LL64: if((int)_tmp91 != 
0)goto _LL66;_LL65: return Cyc_PP_text(({const char*_tmp92=" _stdcall ";_tag_arr(
_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,11));}));_LL66: if((int)_tmp91 != 1)
goto _LL68;_LL67: return Cyc_PP_text(({const char*_tmp93=" _cdecl ";_tag_arr(_tmp93,
sizeof(char),_get_zero_arr_size(_tmp93,9));}));_LL68: if((int)_tmp91 != 2)goto
_LL6A;_LL69: return Cyc_PP_text(({const char*_tmp94=" _fastcall ";_tag_arr(_tmp94,
sizeof(char),_get_zero_arr_size(_tmp94,12));}));_LL6A:;_LL6B: goto _LL63;_LL63:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp95=(void*)atts2->hd;_LL6D: if((int)_tmp95 != 0)goto _LL6F;_LL6E: goto _LL70;
_LL6F: if((int)_tmp95 != 1)goto _LL71;_LL70: goto _LL72;_LL71: if((int)_tmp95 != 2)goto
_LL73;_LL72: goto _LL6C;_LL73:;_LL74: hasatt=1;goto _LL6C;_LL6C:;}}if(!hasatt)return
Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp96[3];_tmp96[2]=Cyc_PP_text(({
const char*_tmp9B=")";_tag_arr(_tmp9B,sizeof(char),_get_zero_arr_size(_tmp9B,2));}));
_tmp96[1]=Cyc_PP_group(({const char*_tmp98="";_tag_arr(_tmp98,sizeof(char),
_get_zero_arr_size(_tmp98,1));}),({const char*_tmp99="";_tag_arr(_tmp99,sizeof(
char),_get_zero_arr_size(_tmp99,1));}),({const char*_tmp9A=" ";_tag_arr(_tmp9A,
sizeof(char),_get_zero_arr_size(_tmp9A,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts));_tmp96[0]=Cyc_PP_text(({const char*_tmp97="__declspec(";_tag_arr(_tmp97,
sizeof(char),_get_zero_arr_size(_tmp97,12));}));Cyc_PP_cat(_tag_arr(_tmp96,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp9C[2];
_tmp9C[1]=Cyc_PP_group(({const char*_tmp9E="((";_tag_arr(_tmp9E,sizeof(char),
_get_zero_arr_size(_tmp9E,3));}),({const char*_tmp9F="))";_tag_arr(_tmp9F,sizeof(
char),_get_zero_arr_size(_tmp9F,3));}),({const char*_tmpA0=",";_tag_arr(_tmpA0,
sizeof(char),_get_zero_arr_size(_tmpA0,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp9C[0]=Cyc_PP_text(({const char*_tmp9D=" __attribute__";_tag_arr(_tmp9D,
sizeof(char),_get_zero_arr_size(_tmp9D,15));}));Cyc_PP_cat(_tag_arr(_tmp9C,
sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpA1=(void*)tms->hd;_LL76: if(*((int*)_tmpA1)!= 2)
goto _LL78;_LL77: return 1;_LL78: if(*((int*)_tmpA1)!= 5)goto _LL7A;_LL79: if(!Cyc_Absynpp_to_VC)
return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);_LL7A:;_LL7B: return 0;_LL75:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpA2="{?}";_tag_arr(_tmpA2,sizeof(char),_get_zero_arr_size(_tmpA2,4));}),
1);else{Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(({const char*
_tmpA3="?";_tag_arr(_tmpA3,sizeof(char),_get_zero_arr_size(_tmpA3,2));}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((
unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA4="{\\lb}";_tag_arr(_tmpA4,
sizeof(char),_get_zero_arr_size(_tmpA4,6));}),1);else{Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA5="{";_tag_arr(_tmpA5,sizeof(char),
_get_zero_arr_size(_tmpA5,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA6="{\\rb}";_tag_arr(_tmpA6,
sizeof(char),_get_zero_arr_size(_tmpA6,6));}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="}";_tag_arr(_tmpA7,sizeof(char),
_get_zero_arr_size(_tmpA7,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA8="\\$";_tag_arr(_tmpA8,
sizeof(char),_get_zero_arr_size(_tmpA8,3));}),1);else{Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA9="$";_tag_arr(_tmpA9,sizeof(char),
_get_zero_arr_size(_tmpA9,2));}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _tagged_arr sep,struct Cyc_List_List*
ss){return({struct Cyc_PP_Doc*_tmpAA[3];_tmpAA[2]=Cyc_Absynpp_rb();_tmpAA[1]=Cyc_PP_seq(
sep,ss);_tmpAA[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmpAA,sizeof(struct Cyc_PP_Doc*),
3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;
tms=tms->tl){void*_tmpAB=(void*)tms->hd;void*_tmpAC;struct Cyc_List_List*_tmpAD;
_LL7D: if(*((int*)_tmpAB)!= 0)goto _LL7F;_LL7E:({void*_tmpAE[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpAF="Carray_mod ";_tag_arr(_tmpAF,sizeof(char),_get_zero_arr_size(
_tmpAF,12));}),_tag_arr(_tmpAE,sizeof(void*),0));});goto _LL7C;_LL7F: if(*((int*)
_tmpAB)!= 1)goto _LL81;_LL80:({void*_tmpB0[0]={};Cyc_fprintf(Cyc_stderr,({const
char*_tmpB1="ConstArray_mod ";_tag_arr(_tmpB1,sizeof(char),_get_zero_arr_size(
_tmpB1,16));}),_tag_arr(_tmpB0,sizeof(void*),0));});goto _LL7C;_LL81: if(*((int*)
_tmpAB)!= 3)goto _LL83;_tmpAC=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAB)->f1;if(*((int*)_tmpAC)!= 1)goto _LL83;_tmpAD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpAC)->f1;_LL82:({void*_tmpB2[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpB3="Function_mod(";
_tag_arr(_tmpB3,sizeof(char),_get_zero_arr_size(_tmpB3,14));}),_tag_arr(_tmpB2,
sizeof(void*),0));});for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){struct Cyc_Core_Opt*
_tmpB4=(*((struct _tuple1*)_tmpAD->hd)).f1;if(_tmpB4 == 0)({void*_tmpB5[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB6="?";_tag_arr(_tmpB6,sizeof(char),
_get_zero_arr_size(_tmpB6,2));}),_tag_arr(_tmpB5,sizeof(void*),0));});else{({
void*_tmpB7[0]={};Cyc_fprintf(Cyc_stderr,*((struct _tagged_arr*)_tmpB4->v),
_tag_arr(_tmpB7,sizeof(void*),0));});}if(_tmpAD->tl != 0)({void*_tmpB8[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpB9=",";_tag_arr(_tmpB9,sizeof(char),
_get_zero_arr_size(_tmpB9,2));}),_tag_arr(_tmpB8,sizeof(void*),0));});}({void*
_tmpBA[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpBB=") ";_tag_arr(_tmpBB,
sizeof(char),_get_zero_arr_size(_tmpBB,3));}),_tag_arr(_tmpBA,sizeof(void*),0));});
goto _LL7C;_LL83: if(*((int*)_tmpAB)!= 3)goto _LL85;_LL84:({void*_tmpBC[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmpBD="Function_mod()";_tag_arr(_tmpBD,sizeof(char),
_get_zero_arr_size(_tmpBD,15));}),_tag_arr(_tmpBC,sizeof(void*),0));});goto _LL7C;
_LL85: if(*((int*)_tmpAB)!= 5)goto _LL87;_LL86:({void*_tmpBE[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpBF="Attributes_mod ";_tag_arr(_tmpBF,sizeof(char),
_get_zero_arr_size(_tmpBF,16));}),_tag_arr(_tmpBE,sizeof(void*),0));});goto _LL7C;
_LL87: if(*((int*)_tmpAB)!= 4)goto _LL89;_LL88:({void*_tmpC0[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpC1="TypeParams_mod ";_tag_arr(_tmpC1,sizeof(char),
_get_zero_arr_size(_tmpC1,16));}),_tag_arr(_tmpC0,sizeof(void*),0));});goto _LL7C;
_LL89: if(*((int*)_tmpAB)!= 2)goto _LL7C;_LL8A:({void*_tmpC2[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmpC3="Pointer_mod ";_tag_arr(_tmpC3,sizeof(char),_get_zero_arr_size(
_tmpC3,13));}),_tag_arr(_tmpC2,sizeof(void*),0));});goto _LL7C;_LL7C:;}({void*
_tmpC4[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpC5="\n";_tag_arr(_tmpC5,
sizeof(char),_get_zero_arr_size(_tmpC5,2));}),_tag_arr(_tmpC4,sizeof(void*),0));});}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp102[3];_tmp102[2]=
Cyc_PP_text(({const char*_tmp104=")";_tag_arr(_tmp104,sizeof(char),
_get_zero_arr_size(_tmp104,2));}));_tmp102[1]=rest;_tmp102[0]=Cyc_PP_text(({
const char*_tmp103="(";_tag_arr(_tmp103,sizeof(char),_get_zero_arr_size(_tmp103,2));}));
Cyc_PP_cat(_tag_arr(_tmp102,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpC6=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Conref*
_tmpC9;void*_tmpCA;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCC;struct
Cyc_Position_Segment*_tmpCD;int _tmpCE;struct Cyc_Absyn_PtrAtts _tmpCF;void*_tmpD0;
struct Cyc_Absyn_Conref*_tmpD1;struct Cyc_Absyn_Conref*_tmpD2;struct Cyc_Absyn_Conref*
_tmpD3;struct Cyc_Absyn_Tqual _tmpD4;_LL8C: if(*((int*)_tmpC6)!= 0)goto _LL8E;_tmpC7=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpC6)->f1;_LL8D: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmpD5[2];_tmpD5[1]=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpC7)?Cyc_PP_text(({const
char*_tmpD6="[]ZEROTERM ";_tag_arr(_tmpD6,sizeof(char),_get_zero_arr_size(_tmpD6,
12));})): Cyc_PP_text(({const char*_tmpD7="[]";_tag_arr(_tmpD7,sizeof(char),
_get_zero_arr_size(_tmpD7,3));}));_tmpD5[0]=rest;Cyc_PP_cat(_tag_arr(_tmpD5,
sizeof(struct Cyc_PP_Doc*),2));});_LL8E: if(*((int*)_tmpC6)!= 1)goto _LL90;_tmpC8=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpC6)->f1;_tmpC9=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpC6)->f2;_LL8F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpD8[4];_tmpD8[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpC9)?Cyc_PP_text(({const char*_tmpDA="]ZEROTERM ";_tag_arr(_tmpDA,sizeof(
char),_get_zero_arr_size(_tmpDA,11));})): Cyc_PP_text(({const char*_tmpDB="]";
_tag_arr(_tmpDB,sizeof(char),_get_zero_arr_size(_tmpDB,2));}));_tmpD8[2]=Cyc_Absynpp_exp2doc(
_tmpC8);_tmpD8[1]=Cyc_PP_text(({const char*_tmpD9="[";_tag_arr(_tmpD9,sizeof(char),
_get_zero_arr_size(_tmpD9,2));}));_tmpD8[0]=rest;Cyc_PP_cat(_tag_arr(_tmpD8,
sizeof(struct Cyc_PP_Doc*),4));});_LL90: if(*((int*)_tmpC6)!= 3)goto _LL92;_tmpCA=(
void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpC6)->f1;_LL91: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmpDC=_tmpCA;struct Cyc_List_List*_tmpDD;int _tmpDE;
struct Cyc_Absyn_VarargInfo*_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_List_List*
_tmpE1;struct Cyc_List_List*_tmpE2;struct Cyc_Position_Segment*_tmpE3;_LL99: if(*((
int*)_tmpDC)!= 1)goto _LL9B;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f1;
_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f3;_tmpE0=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f4;_tmpE1=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f5;_LL9A: return({struct Cyc_PP_Doc*
_tmpE4[2];_tmpE4[1]=Cyc_Absynpp_funargs2doc(_tmpDD,_tmpDE,_tmpDF,_tmpE0,_tmpE1);
_tmpE4[0]=rest;Cyc_PP_cat(_tag_arr(_tmpE4,sizeof(struct Cyc_PP_Doc*),2));});_LL9B:
if(*((int*)_tmpDC)!= 0)goto _LL98;_tmpE2=((struct Cyc_Absyn_NoTypes_struct*)_tmpDC)->f1;
_tmpE3=((struct Cyc_Absyn_NoTypes_struct*)_tmpDC)->f2;_LL9C: return({struct Cyc_PP_Doc*
_tmpE5[2];_tmpE5[1]=Cyc_PP_group(({const char*_tmpE6="(";_tag_arr(_tmpE6,sizeof(
char),_get_zero_arr_size(_tmpE6,2));}),({const char*_tmpE7=")";_tag_arr(_tmpE7,
sizeof(char),_get_zero_arr_size(_tmpE7,2));}),({const char*_tmpE8=",";_tag_arr(
_tmpE8,sizeof(char),_get_zero_arr_size(_tmpE8,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmpE2));_tmpE5[0]=rest;Cyc_PP_cat(_tag_arr(_tmpE5,sizeof(struct
Cyc_PP_Doc*),2));});_LL98:;}_LL92: if(*((int*)_tmpC6)!= 5)goto _LL94;_tmpCB=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpC6)->f2;_LL93: if(!Cyc_Absynpp_to_VC){
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpE9[2];_tmpE9[1]=Cyc_Absynpp_atts2doc(_tmpCB);_tmpE9[0]=rest;Cyc_PP_cat(
_tag_arr(_tmpE9,sizeof(struct Cyc_PP_Doc*),2));});}else{if(Cyc_Absynpp_next_is_pointer(
tms->tl))return({struct Cyc_PP_Doc*_tmpEA[2];_tmpEA[1]=rest;_tmpEA[0]=Cyc_Absynpp_callconv2doc(
_tmpCB);Cyc_PP_cat(_tag_arr(_tmpEA,sizeof(struct Cyc_PP_Doc*),2));});return rest;}
_LL94: if(*((int*)_tmpC6)!= 4)goto _LL96;_tmpCC=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpC6)->f1;_tmpCD=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f2;_tmpCE=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpC6)->f3;_LL95: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpCE)return({struct Cyc_PP_Doc*_tmpEB[2];_tmpEB[1]=Cyc_Absynpp_ktvars2doc(
_tmpCC);_tmpEB[0]=rest;Cyc_PP_cat(_tag_arr(_tmpEB,sizeof(struct Cyc_PP_Doc*),2));});
else{return({struct Cyc_PP_Doc*_tmpEC[2];_tmpEC[1]=Cyc_Absynpp_tvars2doc(_tmpCC);
_tmpEC[0]=rest;Cyc_PP_cat(_tag_arr(_tmpEC,sizeof(struct Cyc_PP_Doc*),2));});}
_LL96: if(*((int*)_tmpC6)!= 2)goto _LL8B;_tmpCF=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpC6)->f1;_tmpD0=(void*)_tmpCF.rgn;_tmpD1=_tmpCF.nullable;_tmpD2=_tmpCF.bounds;
_tmpD3=_tmpCF.zero_term;_tmpD4=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpC6)->f2;
_LL97: {struct Cyc_PP_Doc*ptr;{void*_tmpED=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD2);struct Cyc_Absyn_Exp*_tmpEE;void*_tmpEF;_LL9E: if((int)_tmpED != 0)goto
_LLA0;_LL9F: ptr=Cyc_Absynpp_question();goto _LL9D;_LLA0: if(_tmpED <= (void*)1?1:*((
int*)_tmpED)!= 0)goto _LLA2;_tmpEE=((struct Cyc_Absyn_Upper_b_struct*)_tmpED)->f1;
_LLA1: ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD1)?({const char*_tmpF0="*";_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(
_tmpF0,2));}):({const char*_tmpF1="@";_tag_arr(_tmpF1,sizeof(char),
_get_zero_arr_size(_tmpF1,2));}));{unsigned int _tmpF3;int _tmpF4;struct _tuple4
_tmpF2=Cyc_Evexp_eval_const_uint_exp(_tmpEE);_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;
if(!_tmpF4?1: _tmpF3 != 1)ptr=({struct Cyc_PP_Doc*_tmpF5[4];_tmpF5[3]=Cyc_Absynpp_rb();
_tmpF5[2]=Cyc_Absynpp_exp2doc(_tmpEE);_tmpF5[1]=Cyc_Absynpp_lb();_tmpF5[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpF5,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;}_LLA2: if(
_tmpED <= (void*)1?1:*((int*)_tmpED)!= 1)goto _LL9D;_tmpEF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmpED)->f1;_LLA3: ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD1)?({const char*_tmpF6="*";_tag_arr(_tmpF6,sizeof(char),_get_zero_arr_size(
_tmpF6,2));}):({const char*_tmpF7="@";_tag_arr(_tmpF7,sizeof(char),
_get_zero_arr_size(_tmpF7,2));}));ptr=({struct Cyc_PP_Doc*_tmpF8[4];_tmpF8[3]=Cyc_Absynpp_rb();
_tmpF8[2]=Cyc_Absynpp_typ2doc(_tmpEF);_tmpF8[1]=Cyc_Absynpp_lb();_tmpF8[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpF8,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;_LL9D:;}
if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr?((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3): 0)ptr=({struct Cyc_PP_Doc*_tmpF9[2];_tmpF9[1]=
Cyc_PP_text(({const char*_tmpFA="ZEROTERM ";_tag_arr(_tmpFA,sizeof(char),
_get_zero_arr_size(_tmpFA,10));}));_tmpF9[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpF9,
sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr?!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3): 0)ptr=({struct Cyc_PP_Doc*_tmpFB[2];_tmpFB[1]=
Cyc_PP_text(({const char*_tmpFC="NOZEROTERM ";_tag_arr(_tmpFC,sizeof(char),
_get_zero_arr_size(_tmpFC,12));}));_tmpFB[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpFB,
sizeof(struct Cyc_PP_Doc*),2));});}}{void*_tmpFD=Cyc_Tcutil_compress(_tmpD0);
_LLA5: if((int)_tmpFD != 2)goto _LLA7;_LLA6: goto _LLA4;_LLA7:;_LLA8: ptr=({struct Cyc_PP_Doc*
_tmpFE[3];_tmpFE[2]=Cyc_PP_text(({const char*_tmpFF=" ";_tag_arr(_tmpFF,sizeof(
char),_get_zero_arr_size(_tmpFF,2));}));_tmpFE[1]=Cyc_Absynpp_typ2doc(_tmpD0);
_tmpFE[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpFE,sizeof(struct Cyc_PP_Doc*),3));});_LLA4:;}
ptr=({struct Cyc_PP_Doc*_tmp100[2];_tmp100[1]=Cyc_Absynpp_tqual2doc(_tmpD4);
_tmp100[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp100,sizeof(struct Cyc_PP_Doc*),2));});
return({struct Cyc_PP_Doc*_tmp101[2];_tmp101[1]=rest;_tmp101[0]=ptr;Cyc_PP_cat(
_tag_arr(_tmp101,sizeof(struct Cyc_PP_Doc*),2));});}_LL8B:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp105=Cyc_Tcutil_compress(t);_LLAA: if((int)_tmp105 != 2)goto _LLAC;
_LLAB: return Cyc_PP_text(({const char*_tmp106="`H";_tag_arr(_tmp106,sizeof(char),
_get_zero_arr_size(_tmp106,3));}));_LLAC:;_LLAD: return Cyc_Absynpp_ntyp2doc(t);
_LLA9:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp107=Cyc_Tcutil_compress(t);void*_tmp108;struct Cyc_List_List*
_tmp109;_LLAF: if(_tmp107 <= (void*)3?1:*((int*)_tmp107)!= 19)goto _LLB1;_tmp108=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp107)->f1;_LLB0:*rgions=({struct Cyc_List_List*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->hd=Cyc_Absynpp_rgn2doc(_tmp108);
_tmp10A->tl=*rgions;_tmp10A;});goto _LLAE;_LLB1: if(_tmp107 <= (void*)3?1:*((int*)
_tmp107)!= 20)goto _LLB3;_tmp109=((struct Cyc_Absyn_JoinEff_struct*)_tmp107)->f1;
_LLB2: for(0;_tmp109 != 0;_tmp109=_tmp109->tl){Cyc_Absynpp_effects2docs(rgions,
effects,(void*)_tmp109->hd);}goto _LLAE;_LLB3:;_LLB4:*effects=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=Cyc_Absynpp_typ2doc(t);_tmp10B->tl=*
effects;_tmp10B;});goto _LLAE;_LLAE:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0?effects != 0: 0)return Cyc_PP_group(({const char*_tmp10C="";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,1));}),({const char*
_tmp10D="";_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,1));}),({
const char*_tmp10E="+";_tag_arr(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,2));}),
effects);else{struct Cyc_PP_Doc*_tmp10F=Cyc_Absynpp_group_braces(({const char*
_tmp114=",";_tag_arr(_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,2));}),
rgions);return Cyc_PP_group(({const char*_tmp110="";_tag_arr(_tmp110,sizeof(char),
_get_zero_arr_size(_tmp110,1));}),({const char*_tmp111="";_tag_arr(_tmp111,
sizeof(char),_get_zero_arr_size(_tmp111,1));}),({const char*_tmp112="+";_tag_arr(
_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,2));}),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({
struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=_tmp10F;
_tmp113->tl=0;_tmp113;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k){
void*_tmp115=k;_LLB6: if((int)_tmp115 != 0)goto _LLB8;_LLB7: return Cyc_PP_text(({
const char*_tmp116="struct ";_tag_arr(_tmp116,sizeof(char),_get_zero_arr_size(
_tmp116,8));}));_LLB8: if((int)_tmp115 != 1)goto _LLB5;_LLB9: return Cyc_PP_text(({
const char*_tmp117="union ";_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(
_tmp117,7));}));_LLB5:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*
s;{void*_tmp118=t;struct Cyc_Core_Opt*_tmp119;struct Cyc_Core_Opt*_tmp11A;int
_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Absyn_Tvar*_tmp11D;struct Cyc_Absyn_TunionInfo
_tmp11E;void*_tmp11F;struct Cyc_List_List*_tmp120;void*_tmp121;struct Cyc_Absyn_TunionFieldInfo
_tmp122;void*_tmp123;struct Cyc_List_List*_tmp124;void*_tmp125;void*_tmp126;int
_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_AggrInfo _tmp129;void*_tmp12A;
struct Cyc_List_List*_tmp12B;void*_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*
_tmp12E;struct _tuple0*_tmp12F;void*_tmp130;struct _tuple0*_tmp131;struct Cyc_List_List*
_tmp132;struct Cyc_Absyn_Typedefdecl*_tmp133;void*_tmp134;int _tmp135;void*_tmp136;
void*_tmp137;_LLBB: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 7)goto _LLBD;_LLBC:
return Cyc_PP_text(({const char*_tmp138="[[[array]]]";_tag_arr(_tmp138,sizeof(char),
_get_zero_arr_size(_tmp138,12));}));_LLBD: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 8)goto _LLBF;_LLBE: return Cyc_PP_nil_doc();_LLBF: if(_tmp118 <= (void*)3?
1:*((int*)_tmp118)!= 4)goto _LLC1;_LLC0: return Cyc_PP_nil_doc();_LLC1: if((int)
_tmp118 != 0)goto _LLC3;_LLC2: s=Cyc_PP_text(({const char*_tmp139="void";_tag_arr(
_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,5));}));goto _LLBA;_LLC3: if(
_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 0)goto _LLC5;_tmp119=((struct Cyc_Absyn_Evar_struct*)
_tmp118)->f1;_tmp11A=((struct Cyc_Absyn_Evar_struct*)_tmp118)->f2;_tmp11B=((
struct Cyc_Absyn_Evar_struct*)_tmp118)->f3;_tmp11C=((struct Cyc_Absyn_Evar_struct*)
_tmp118)->f4;_LLC4: if(_tmp11A != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11A->v);
else{void*_tmp13A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp119))->v;_LLEE:
if((int)_tmp13A != 3)goto _LLF0;_LLEF: s=({struct Cyc_PP_Doc*_tmp13B[2];_tmp13B[1]=
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp13F;_tmp13F.tag=1;
_tmp13F.f1=(unsigned int)_tmp11B;{void*_tmp13D[1]={& _tmp13F};Cyc_aprintf(({const
char*_tmp13E="%d";_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,3));}),
_tag_arr(_tmp13D,sizeof(void*),1));}}));_tmp13B[0]=Cyc_PP_text(({const char*
_tmp13C="`r";_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,3));}));
Cyc_PP_cat(_tag_arr(_tmp13B,sizeof(struct Cyc_PP_Doc*),2));});goto _LLED;_LLF0:;
_LLF1: s=({struct Cyc_PP_Doc*_tmp140[6];_tmp140[5]=_tmp119 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((void*)_tmp119->v);_tmp140[4]=Cyc_PP_text(({const char*
_tmp147=")::";_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,4));}));
_tmp140[3]=(!Cyc_Absynpp_print_full_evars?1: _tmp11C == 0)?Cyc_PP_text(({const char*
_tmp146="";_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,1));})): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)_tmp11C->v);_tmp140[2]=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp145;_tmp145.tag=1;_tmp145.f1=(unsigned int)_tmp11B;{
void*_tmp143[1]={& _tmp145};Cyc_aprintf(({const char*_tmp144="%d";_tag_arr(_tmp144,
sizeof(char),_get_zero_arr_size(_tmp144,3));}),_tag_arr(_tmp143,sizeof(void*),1));}}));
_tmp140[1]=Cyc_PP_text(({const char*_tmp142="(";_tag_arr(_tmp142,sizeof(char),
_get_zero_arr_size(_tmp142,2));}));_tmp140[0]=Cyc_PP_text(({const char*_tmp141="%";
_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,2));}));Cyc_PP_cat(
_tag_arr(_tmp140,sizeof(struct Cyc_PP_Doc*),6));});_LLED:;}goto _LLBA;_LLC5: if(
_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 1)goto _LLC7;_tmp11D=((struct Cyc_Absyn_VarType_struct*)
_tmp118)->f1;_LLC6: s=Cyc_PP_textptr(_tmp11D->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp148[3];_tmp148[2]=Cyc_Absynpp_kindbound2doc((void*)
_tmp11D->kind);_tmp148[1]=Cyc_PP_text(({const char*_tmp149="::";_tag_arr(_tmp149,
sizeof(char),_get_zero_arr_size(_tmp149,3));}));_tmp148[0]=s;Cyc_PP_cat(_tag_arr(
_tmp148,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?Cyc_Tcutil_is_temp_tvar(
_tmp11D): 0)s=({struct Cyc_PP_Doc*_tmp14A[3];_tmp14A[2]=Cyc_PP_text(({const char*
_tmp14C=" */";_tag_arr(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,4));}));
_tmp14A[1]=s;_tmp14A[0]=Cyc_PP_text(({const char*_tmp14B="_ /* ";_tag_arr(_tmp14B,
sizeof(char),_get_zero_arr_size(_tmp14B,6));}));Cyc_PP_cat(_tag_arr(_tmp14A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLC7: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 2)goto _LLC9;_tmp11E=((struct Cyc_Absyn_TunionType_struct*)_tmp118)->f1;
_tmp11F=(void*)_tmp11E.tunion_info;_tmp120=_tmp11E.targs;_tmp121=(void*)_tmp11E.rgn;
_LLC8:{void*_tmp14D=_tmp11F;struct Cyc_Absyn_UnknownTunionInfo _tmp14E;struct
_tuple0*_tmp14F;int _tmp150;struct Cyc_Absyn_Tuniondecl**_tmp151;struct Cyc_Absyn_Tuniondecl*
_tmp152;struct Cyc_Absyn_Tuniondecl _tmp153;struct _tuple0*_tmp154;int _tmp155;_LLF3:
if(*((int*)_tmp14D)!= 0)goto _LLF5;_tmp14E=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp14D)->f1;_tmp14F=_tmp14E.name;_tmp150=_tmp14E.is_xtunion;_LLF4: _tmp154=
_tmp14F;_tmp155=_tmp150;goto _LLF6;_LLF5: if(*((int*)_tmp14D)!= 1)goto _LLF2;
_tmp151=((struct Cyc_Absyn_KnownTunion_struct*)_tmp14D)->f1;_tmp152=*_tmp151;
_tmp153=*_tmp152;_tmp154=_tmp153.name;_tmp155=_tmp153.is_xtunion;_LLF6: {struct
Cyc_PP_Doc*_tmp156=Cyc_PP_text(_tmp155?({const char*_tmp15B="xtunion ";_tag_arr(
_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,9));}):({const char*_tmp15C="tunion ";
_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,8));}));{void*_tmp157=
Cyc_Tcutil_compress(_tmp121);_LLF8: if((int)_tmp157 != 2)goto _LLFA;_LLF9: s=({
struct Cyc_PP_Doc*_tmp158[3];_tmp158[2]=Cyc_Absynpp_tps2doc(_tmp120);_tmp158[1]=
Cyc_Absynpp_qvar2doc(_tmp154);_tmp158[0]=_tmp156;Cyc_PP_cat(_tag_arr(_tmp158,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLF7;_LLFA:;_LLFB: s=({struct Cyc_PP_Doc*
_tmp159[5];_tmp159[4]=Cyc_Absynpp_tps2doc(_tmp120);_tmp159[3]=Cyc_Absynpp_qvar2doc(
_tmp154);_tmp159[2]=Cyc_PP_text(({const char*_tmp15A=" ";_tag_arr(_tmp15A,sizeof(
char),_get_zero_arr_size(_tmp15A,2));}));_tmp159[1]=Cyc_Absynpp_typ2doc(_tmp121);
_tmp159[0]=_tmp156;Cyc_PP_cat(_tag_arr(_tmp159,sizeof(struct Cyc_PP_Doc*),5));});
goto _LLF7;_LLF7:;}goto _LLF2;}_LLF2:;}goto _LLBA;_LLC9: if(_tmp118 <= (void*)3?1:*((
int*)_tmp118)!= 3)goto _LLCB;_tmp122=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp118)->f1;_tmp123=(void*)_tmp122.field_info;_tmp124=_tmp122.targs;_LLCA:{void*
_tmp15D=_tmp123;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp15E;struct _tuple0*
_tmp15F;struct _tuple0*_tmp160;int _tmp161;struct Cyc_Absyn_Tuniondecl*_tmp162;
struct Cyc_Absyn_Tuniondecl _tmp163;struct _tuple0*_tmp164;int _tmp165;struct Cyc_Absyn_Tunionfield*
_tmp166;struct Cyc_Absyn_Tunionfield _tmp167;struct _tuple0*_tmp168;_LLFD: if(*((int*)
_tmp15D)!= 0)goto _LLFF;_tmp15E=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp15D)->f1;_tmp15F=_tmp15E.tunion_name;_tmp160=_tmp15E.field_name;_tmp161=
_tmp15E.is_xtunion;_LLFE: _tmp164=_tmp15F;_tmp165=_tmp161;_tmp168=_tmp160;goto
_LL100;_LLFF: if(*((int*)_tmp15D)!= 1)goto _LLFC;_tmp162=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp15D)->f1;_tmp163=*_tmp162;_tmp164=_tmp163.name;_tmp165=_tmp163.is_xtunion;
_tmp166=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp15D)->f2;_tmp167=*_tmp166;
_tmp168=_tmp167.name;_LL100: {struct Cyc_PP_Doc*_tmp169=Cyc_PP_text(_tmp165?({
const char*_tmp16C="xtunion ";_tag_arr(_tmp16C,sizeof(char),_get_zero_arr_size(
_tmp16C,9));}):({const char*_tmp16D="tunion ";_tag_arr(_tmp16D,sizeof(char),
_get_zero_arr_size(_tmp16D,8));}));s=({struct Cyc_PP_Doc*_tmp16A[4];_tmp16A[3]=
Cyc_Absynpp_qvar2doc(_tmp168);_tmp16A[2]=Cyc_PP_text(({const char*_tmp16B=".";
_tag_arr(_tmp16B,sizeof(char),_get_zero_arr_size(_tmp16B,2));}));_tmp16A[1]=Cyc_Absynpp_qvar2doc(
_tmp164);_tmp16A[0]=_tmp169;Cyc_PP_cat(_tag_arr(_tmp16A,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLFC;}_LLFC:;}goto _LLBA;_LLCB: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 5)goto _LLCD;_tmp125=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp118)->f1;
_tmp126=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp118)->f2;_LLCC: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp16E=_tmp125;_LL102: if((int)_tmp16E
!= 2)goto _LL104;_LL103: goto _LL105;_LL104: if((int)_tmp16E != 0)goto _LL106;_LL105:
sns=({const char*_tmp16F="";_tag_arr(_tmp16F,sizeof(char),_get_zero_arr_size(
_tmp16F,1));});goto _LL101;_LL106: if((int)_tmp16E != 1)goto _LL101;_LL107: sns=({
const char*_tmp170="unsigned ";_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,10));});goto _LL101;_LL101:;}{void*_tmp171=_tmp126;_LL109: if((int)_tmp171
!= 0)goto _LL10B;_LL10A:{void*_tmp172=_tmp125;_LL112: if((int)_tmp172 != 2)goto
_LL114;_LL113: sns=({const char*_tmp173="";_tag_arr(_tmp173,sizeof(char),
_get_zero_arr_size(_tmp173,1));});goto _LL111;_LL114: if((int)_tmp172 != 0)goto
_LL116;_LL115: sns=({const char*_tmp174="signed ";_tag_arr(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,8));});goto _LL111;_LL116: if((int)_tmp172 != 1)goto
_LL111;_LL117: sns=({const char*_tmp175="unsigned ";_tag_arr(_tmp175,sizeof(char),
_get_zero_arr_size(_tmp175,10));});goto _LL111;_LL111:;}ts=({const char*_tmp176="char";
_tag_arr(_tmp176,sizeof(char),_get_zero_arr_size(_tmp176,5));});goto _LL108;
_LL10B: if((int)_tmp171 != 1)goto _LL10D;_LL10C: ts=({const char*_tmp177="short";
_tag_arr(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,6));});goto _LL108;
_LL10D: if((int)_tmp171 != 2)goto _LL10F;_LL10E: ts=({const char*_tmp178="int";
_tag_arr(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,4));});goto _LL108;
_LL10F: if((int)_tmp171 != 3)goto _LL108;_LL110: ts=Cyc_Absynpp_to_VC?({const char*
_tmp179="__int64";_tag_arr(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,8));}):({
const char*_tmp17A="long long";_tag_arr(_tmp17A,sizeof(char),_get_zero_arr_size(
_tmp17A,10));});goto _LL108;_LL108:;}s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _tagged_arr)((struct _tagged_arr)ts);{
struct Cyc_String_pa_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=(struct _tagged_arr)((
struct _tagged_arr)sns);{void*_tmp17B[2]={& _tmp17D,& _tmp17E};Cyc_aprintf(({const
char*_tmp17C="%s%s";_tag_arr(_tmp17C,sizeof(char),_get_zero_arr_size(_tmp17C,5));}),
_tag_arr(_tmp17B,sizeof(void*),2));}}}));goto _LLBA;}_LLCD: if((int)_tmp118 != 1)
goto _LLCF;_LLCE: s=Cyc_PP_text(({const char*_tmp17F="float";_tag_arr(_tmp17F,
sizeof(char),_get_zero_arr_size(_tmp17F,6));}));goto _LLBA;_LLCF: if(_tmp118 <= (
void*)3?1:*((int*)_tmp118)!= 6)goto _LLD1;_tmp127=((struct Cyc_Absyn_DoubleType_struct*)
_tmp118)->f1;_LLD0: if(_tmp127)s=Cyc_PP_text(({const char*_tmp180="long double";
_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,12));}));else{s=Cyc_PP_text(({
const char*_tmp181="double";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(
_tmp181,7));}));}goto _LLBA;_LLD1: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 9)
goto _LLD3;_tmp128=((struct Cyc_Absyn_TupleType_struct*)_tmp118)->f1;_LLD2: s=({
struct Cyc_PP_Doc*_tmp182[2];_tmp182[1]=Cyc_Absynpp_args2doc(_tmp128);_tmp182[0]=
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp182,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBA;_LLD3: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 10)goto _LLD5;_tmp129=((
struct Cyc_Absyn_AggrType_struct*)_tmp118)->f1;_tmp12A=(void*)_tmp129.aggr_info;
_tmp12B=_tmp129.targs;_LLD4: {void*_tmp184;struct _tuple0*_tmp185;struct _tuple3
_tmp183=Cyc_Absyn_aggr_kinded_name(_tmp12A);_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;
s=({struct Cyc_PP_Doc*_tmp186[3];_tmp186[2]=Cyc_Absynpp_tps2doc(_tmp12B);_tmp186[
1]=Cyc_Absynpp_qvar2doc(_tmp185);_tmp186[0]=Cyc_Absynpp_aggr_kind2doc(_tmp184);
Cyc_PP_cat(_tag_arr(_tmp186,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;}_LLD5:
if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 11)goto _LLD7;_tmp12C=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp118)->f1;_tmp12D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp118)->f2;_LLD6: s=({struct Cyc_PP_Doc*_tmp187[4];_tmp187[3]=Cyc_Absynpp_rb();
_tmp187[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp12D));_tmp187[1]=Cyc_Absynpp_lb();
_tmp187[0]=Cyc_Absynpp_aggr_kind2doc(_tmp12C);Cyc_PP_cat(_tag_arr(_tmp187,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLBA;_LLD7: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 13)goto _LLD9;_tmp12E=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp118)->f1;
_LLD8: s=({struct Cyc_PP_Doc*_tmp188[4];_tmp188[3]=Cyc_Absynpp_rb();_tmp188[2]=Cyc_PP_nest(
2,Cyc_Absynpp_enumfields2doc(_tmp12E));_tmp188[1]=Cyc_Absynpp_lb();_tmp188[0]=
Cyc_PP_text(({const char*_tmp189="enum ";_tag_arr(_tmp189,sizeof(char),
_get_zero_arr_size(_tmp189,6));}));Cyc_PP_cat(_tag_arr(_tmp188,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLBA;_LLD9: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 12)goto _LLDB;
_tmp12F=((struct Cyc_Absyn_EnumType_struct*)_tmp118)->f1;_LLDA: s=({struct Cyc_PP_Doc*
_tmp18A[2];_tmp18A[1]=Cyc_Absynpp_qvar2doc(_tmp12F);_tmp18A[0]=Cyc_PP_text(({
const char*_tmp18B="enum ";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(
_tmp18B,6));}));Cyc_PP_cat(_tag_arr(_tmp18A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBA;_LLDB: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 14)goto _LLDD;_tmp130=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp118)->f1;_LLDC: s=({struct Cyc_PP_Doc*
_tmp18C[3];_tmp18C[2]=Cyc_PP_text(({const char*_tmp18E=">";_tag_arr(_tmp18E,
sizeof(char),_get_zero_arr_size(_tmp18E,2));}));_tmp18C[1]=Cyc_Absynpp_typ2doc(
_tmp130);_tmp18C[0]=Cyc_PP_text(({const char*_tmp18D="sizeof_t<";_tag_arr(_tmp18D,
sizeof(char),_get_zero_arr_size(_tmp18D,10));}));Cyc_PP_cat(_tag_arr(_tmp18C,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLDD: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 16)goto _LLDF;_tmp131=((struct Cyc_Absyn_TypedefType_struct*)_tmp118)->f1;
_tmp132=((struct Cyc_Absyn_TypedefType_struct*)_tmp118)->f2;_tmp133=((struct Cyc_Absyn_TypedefType_struct*)
_tmp118)->f3;_LLDE: s=({struct Cyc_PP_Doc*_tmp18F[2];_tmp18F[1]=Cyc_Absynpp_tps2doc(
_tmp132);_tmp18F[0]=Cyc_Absynpp_qvar2doc(_tmp131);Cyc_PP_cat(_tag_arr(_tmp18F,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBA;_LLDF: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 15)goto _LLE1;_tmp134=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp118)->f1;_LLE0: s=({struct Cyc_PP_Doc*_tmp190[3];_tmp190[2]=Cyc_PP_text(({
const char*_tmp192=">";_tag_arr(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,2));}));
_tmp190[1]=Cyc_Absynpp_rgn2doc(_tmp134);_tmp190[0]=Cyc_PP_text(({const char*
_tmp191="region_t<";_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,10));}));
Cyc_PP_cat(_tag_arr(_tmp190,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE1: if(
_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 18)goto _LLE3;_tmp135=((struct Cyc_Absyn_TypeInt_struct*)
_tmp118)->f1;_LLE2: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp195;_tmp195.tag=1;_tmp195.f1=(unsigned int)_tmp135;{void*_tmp193[1]={&
_tmp195};Cyc_aprintf(({const char*_tmp194="`%d";_tag_arr(_tmp194,sizeof(char),
_get_zero_arr_size(_tmp194,4));}),_tag_arr(_tmp193,sizeof(void*),1));}}));goto
_LLBA;_LLE3: if(_tmp118 <= (void*)3?1:*((int*)_tmp118)!= 17)goto _LLE5;_tmp136=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp118)->f1;_LLE4: s=({struct Cyc_PP_Doc*
_tmp196[3];_tmp196[2]=Cyc_PP_text(({const char*_tmp198=">";_tag_arr(_tmp198,
sizeof(char),_get_zero_arr_size(_tmp198,2));}));_tmp196[1]=Cyc_Absynpp_typ2doc(
_tmp136);_tmp196[0]=Cyc_PP_text(({const char*_tmp197="tag_t<";_tag_arr(_tmp197,
sizeof(char),_get_zero_arr_size(_tmp197,7));}));Cyc_PP_cat(_tag_arr(_tmp196,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE5: if((int)_tmp118 != 2)goto _LLE7;
_LLE6: s=Cyc_Absynpp_rgn2doc(t);goto _LLBA;_LLE7: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 21)goto _LLE9;_tmp137=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp118)->f1;
_LLE8: s=({struct Cyc_PP_Doc*_tmp199[3];_tmp199[2]=Cyc_PP_text(({const char*_tmp19B=")";
_tag_arr(_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,2));}));_tmp199[1]=Cyc_Absynpp_typ2doc(
_tmp137);_tmp199[0]=Cyc_PP_text(({const char*_tmp19A="regions(";_tag_arr(_tmp19A,
sizeof(char),_get_zero_arr_size(_tmp19A,9));}));Cyc_PP_cat(_tag_arr(_tmp199,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE9: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(_tmp118 <= (void*)3?1:*((int*)
_tmp118)!= 20)goto _LLBA;_LLEC: s=Cyc_Absynpp_eff2doc(t);goto _LLBA;_LLBA:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc():
Cyc_PP_text(*((struct _tagged_arr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple5*cmp){struct _tuple5 _tmp19D;void*_tmp19E;void*_tmp19F;struct _tuple5*
_tmp19C=cmp;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;return({
struct Cyc_PP_Doc*_tmp1A0[3];_tmp1A0[2]=Cyc_Absynpp_rgn2doc(_tmp19F);_tmp1A0[1]=
Cyc_PP_text(({const char*_tmp1A1=" > ";_tag_arr(_tmp1A1,sizeof(char),
_get_zero_arr_size(_tmp1A1,4));}));_tmp1A0[0]=Cyc_Absynpp_rgn2doc(_tmp19E);Cyc_PP_cat(
_tag_arr(_tmp1A0,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp1A2="";_tag_arr(
_tmp1A2,sizeof(char),_get_zero_arr_size(_tmp1A2,1));}),({const char*_tmp1A3="";
_tag_arr(_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,1));}),({const char*
_tmp1A4=",";_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->v=Cyc_PP_text(*((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmp1A5;});return Cyc_Absynpp_tqtd2doc((*
t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A6,({struct
Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->hd=Cyc_PP_text(({
const char*_tmp1A8="...";_tag_arr(_tmp1A8,sizeof(char),_get_zero_arr_size(_tmp1A8,
4));}));_tmp1A7->tl=0;_tmp1A7;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmp1A9=({struct Cyc_PP_Doc*_tmp1AB[3];_tmp1AB[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->f1=cyc_varargs->name;
_tmp1AF->f2=cyc_varargs->tq;_tmp1AF->f3=(void*)cyc_varargs->type;_tmp1AF;}));
_tmp1AB[1]=cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1AD=" inject ";
_tag_arr(_tmp1AD,sizeof(char),_get_zero_arr_size(_tmp1AD,9));})): Cyc_PP_text(({
const char*_tmp1AE=" ";_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,2));}));
_tmp1AB[0]=Cyc_PP_text(({const char*_tmp1AC="...";_tag_arr(_tmp1AC,sizeof(char),
_get_zero_arr_size(_tmp1AC,4));}));Cyc_PP_cat(_tag_arr(_tmp1AB,sizeof(struct Cyc_PP_Doc*),
3));});_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A6,({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*
_tmp1AA));_tmp1AA->hd=_tmp1A9;_tmp1AA->tl=0;_tmp1AA;}));}}{struct Cyc_PP_Doc*
_tmp1B0=Cyc_PP_group(({const char*_tmp1B8="";_tag_arr(_tmp1B8,sizeof(char),
_get_zero_arr_size(_tmp1B8,1));}),({const char*_tmp1B9="";_tag_arr(_tmp1B9,
sizeof(char),_get_zero_arr_size(_tmp1B9,1));}),({const char*_tmp1BA=",";_tag_arr(
_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,2));}),_tmp1A6);if(effopt != 0?
Cyc_Absynpp_print_all_effects: 0)_tmp1B0=({struct Cyc_PP_Doc*_tmp1B1[3];_tmp1B1[2]=
Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1B1[1]=Cyc_PP_text(({const char*_tmp1B2=";";
_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,2));}));_tmp1B1[0]=
_tmp1B0;Cyc_PP_cat(_tag_arr(_tmp1B1,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 
0)_tmp1B0=({struct Cyc_PP_Doc*_tmp1B3[3];_tmp1B3[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmp1B3[1]=Cyc_PP_text(({const char*_tmp1B4=":";_tag_arr(_tmp1B4,sizeof(char),
_get_zero_arr_size(_tmp1B4,2));}));_tmp1B3[0]=_tmp1B0;Cyc_PP_cat(_tag_arr(
_tmp1B3,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmp1B5[3];
_tmp1B5[2]=Cyc_PP_text(({const char*_tmp1B7=")";_tag_arr(_tmp1B7,sizeof(char),
_get_zero_arr_size(_tmp1B7,2));}));_tmp1B5[1]=_tmp1B0;_tmp1B5[0]=Cyc_PP_text(({
const char*_tmp1B6="(";_tag_arr(_tmp1B6,sizeof(char),_get_zero_arr_size(_tmp1B6,2));}));
Cyc_PP_cat(_tag_arr(_tmp1B5,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){return({struct _tuple1*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB->f1=({struct Cyc_Core_Opt*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->v=(*
arg).f1;_tmp1BC;});_tmp1BB->f2=(*arg).f2;_tmp1BB->f3=(*arg).f3;_tmp1BB;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _tagged_arr*v){return Cyc_PP_text(*v);}
struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BD=0;int match;{void*_tmp1BE=(*q).f1;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*
_tmp1C0;_LL119: if((int)_tmp1BE != 0)goto _LL11B;_LL11A: _tmp1BF=0;goto _LL11C;_LL11B:
if(_tmp1BE <= (void*)1?1:*((int*)_tmp1BE)!= 0)goto _LL11D;_tmp1BF=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1BE)->f1;_LL11C: match=0;_tmp1BD=_tmp1BF;goto _LL118;_LL11D: if(_tmp1BE <= (void*)
1?1:*((int*)_tmp1BE)!= 1)goto _LL118;_tmp1C0=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1BE)->f1;_LL11E: match=Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C0,Cyc_Absynpp_curr_namespace): 0;
_tmp1BD=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1C1->tl=_tmp1C0;_tmp1C1;}): _tmp1C0;goto _LL118;_LL118:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _tagged_arr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD,({struct Cyc_List_List*_tmp1C2=
_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=(*q).f2;_tmp1C2->tl=0;_tmp1C2;})),({
const char*_tmp1C3="_";_tag_arr(_tmp1C3,sizeof(char),_get_zero_arr_size(_tmp1C3,2));}));
else{if(match)return*(*q).f2;else{return(struct _tagged_arr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BD,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=(*
q).f2;_tmp1C4->tl=0;_tmp1C4;})),({const char*_tmp1C5="::";_tag_arr(_tmp1C5,
sizeof(char),_get_zero_arr_size(_tmp1C5,3));}));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _tagged_arr _tmp1C6=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_strconcat(({const char*_tmp1C7="\\textbf{";_tag_arr(_tmp1C7,
sizeof(char),_get_zero_arr_size(_tmp1C7,9));}),(struct _tagged_arr)_tmp1C6),({
const char*_tmp1C8="}";_tag_arr(_tmp1C8,sizeof(char),_get_zero_arr_size(_tmp1C8,2));})),(
int)Cyc_strlen((struct _tagged_arr)_tmp1C6));else{return Cyc_PP_text(_tmp1C6);}}
struct _tagged_arr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){void*_tmp1C9=(*
v).f1;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1CB;_LL120: if((int)
_tmp1C9 != 0)goto _LL122;_LL121: goto _LL123;_LL122: if(_tmp1C9 <= (void*)1?1:*((int*)
_tmp1C9)!= 0)goto _LL124;_tmp1CA=((struct Cyc_Absyn_Rel_n_struct*)_tmp1C9)->f1;if(
_tmp1CA != 0)goto _LL124;_LL123: return*(*v).f2;_LL124: if(_tmp1C9 <= (void*)1?1:*((
int*)_tmp1C9)!= 1)goto _LL126;_tmp1CB=((struct Cyc_Absyn_Abs_n_struct*)_tmp1C9)->f1;
_LL125: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CB,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL127;}_LL126:;_LL127: return(struct _tagged_arr)Cyc_strconcat(({
const char*_tmp1CC="/* bad namespace : */ ";_tag_arr(_tmp1CC,sizeof(char),
_get_zero_arr_size(_tmp1CC,23));}),(struct _tagged_arr)Cyc_Absynpp_qvar2string(v));
_LL11F:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct
Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){struct _tagged_arr
_tmp1CD=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _tagged_arr)Cyc_strconcat((struct _tagged_arr)Cyc_strconcat(({const char*
_tmp1CE="\\textbf{";_tag_arr(_tmp1CE,sizeof(char),_get_zero_arr_size(_tmp1CE,9));}),(
struct _tagged_arr)_tmp1CD),({const char*_tmp1CF="}";_tag_arr(_tmp1CF,sizeof(char),
_get_zero_arr_size(_tmp1CF,2));})),(int)Cyc_strlen((struct _tagged_arr)_tmp1CD));
else{return Cyc_PP_text(_tmp1CD);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D0=(void*)e->r;void*_tmp1D1;struct Cyc_Absyn_Exp*
_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;_LL129: if(*((int*)_tmp1D0)!= 0)goto _LL12B;
_LL12A: goto _LL12C;_LL12B: if(*((int*)_tmp1D0)!= 1)goto _LL12D;_LL12C: goto _LL12E;
_LL12D: if(*((int*)_tmp1D0)!= 2)goto _LL12F;_LL12E: return 10000;_LL12F: if(*((int*)
_tmp1D0)!= 3)goto _LL131;_tmp1D1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D0)->f1;_LL130: {void*_tmp1D4=_tmp1D1;_LL176: if((int)_tmp1D4 != 0)goto _LL178;
_LL177: return 100;_LL178: if((int)_tmp1D4 != 1)goto _LL17A;_LL179: return 110;_LL17A:
if((int)_tmp1D4 != 2)goto _LL17C;_LL17B: return 100;_LL17C: if((int)_tmp1D4 != 3)goto
_LL17E;_LL17D: goto _LL17F;_LL17E: if((int)_tmp1D4 != 4)goto _LL180;_LL17F: return 110;
_LL180: if((int)_tmp1D4 != 5)goto _LL182;_LL181: goto _LL183;_LL182: if((int)_tmp1D4 != 
6)goto _LL184;_LL183: return 70;_LL184: if((int)_tmp1D4 != 7)goto _LL186;_LL185: goto
_LL187;_LL186: if((int)_tmp1D4 != 8)goto _LL188;_LL187: goto _LL189;_LL188: if((int)
_tmp1D4 != 9)goto _LL18A;_LL189: goto _LL18B;_LL18A: if((int)_tmp1D4 != 10)goto _LL18C;
_LL18B: return 80;_LL18C: if((int)_tmp1D4 != 11)goto _LL18E;_LL18D: goto _LL18F;_LL18E:
if((int)_tmp1D4 != 12)goto _LL190;_LL18F: return 130;_LL190: if((int)_tmp1D4 != 13)
goto _LL192;_LL191: return 60;_LL192: if((int)_tmp1D4 != 14)goto _LL194;_LL193: return
40;_LL194: if((int)_tmp1D4 != 15)goto _LL196;_LL195: return 50;_LL196: if((int)_tmp1D4
!= 16)goto _LL198;_LL197: return 90;_LL198: if((int)_tmp1D4 != 17)goto _LL19A;_LL199:
return 80;_LL19A: if((int)_tmp1D4 != 18)goto _LL19C;_LL19B: return 80;_LL19C: if((int)
_tmp1D4 != 19)goto _LL175;_LL19D: return 140;_LL175:;}_LL131: if(*((int*)_tmp1D0)!= 4)
goto _LL133;_LL132: return 20;_LL133: if(*((int*)_tmp1D0)!= 5)goto _LL135;_LL134:
return 130;_LL135: if(*((int*)_tmp1D0)!= 6)goto _LL137;_LL136: return 30;_LL137: if(*((
int*)_tmp1D0)!= 7)goto _LL139;_LL138: return 10;_LL139: if(*((int*)_tmp1D0)!= 8)goto
_LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmp1D0)!= 9)goto _LL13D;_LL13C: return
140;_LL13D: if(*((int*)_tmp1D0)!= 10)goto _LL13F;_LL13E: return 130;_LL13F: if(*((int*)
_tmp1D0)!= 11)goto _LL141;_tmp1D2=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1D0)->f1;_LL140: return Cyc_Absynpp_exp_prec(_tmp1D2);_LL141: if(*((int*)
_tmp1D0)!= 12)goto _LL143;_tmp1D3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D0)->f1;
_LL142: return Cyc_Absynpp_exp_prec(_tmp1D3);_LL143: if(*((int*)_tmp1D0)!= 13)goto
_LL145;_LL144: return 120;_LL145: if(*((int*)_tmp1D0)!= 15)goto _LL147;_LL146: return
15;_LL147: if(*((int*)_tmp1D0)!= 14)goto _LL149;_LL148: goto _LL14A;_LL149: if(*((int*)
_tmp1D0)!= 16)goto _LL14B;_LL14A: goto _LL14C;_LL14B: if(*((int*)_tmp1D0)!= 17)goto
_LL14D;_LL14C: goto _LL14E;_LL14D: if(*((int*)_tmp1D0)!= 18)goto _LL14F;_LL14E: goto
_LL150;_LL14F: if(*((int*)_tmp1D0)!= 19)goto _LL151;_LL150: goto _LL152;_LL151: if(*((
int*)_tmp1D0)!= 20)goto _LL153;_LL152: return 130;_LL153: if(*((int*)_tmp1D0)!= 21)
goto _LL155;_LL154: goto _LL156;_LL155: if(*((int*)_tmp1D0)!= 22)goto _LL157;_LL156:
goto _LL158;_LL157: if(*((int*)_tmp1D0)!= 23)goto _LL159;_LL158: return 140;_LL159:
if(*((int*)_tmp1D0)!= 24)goto _LL15B;_LL15A: return 150;_LL15B: if(*((int*)_tmp1D0)
!= 25)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmp1D0)!= 26)goto _LL15F;
_LL15E: goto _LL160;_LL15F: if(*((int*)_tmp1D0)!= 27)goto _LL161;_LL160: goto _LL162;
_LL161: if(*((int*)_tmp1D0)!= 28)goto _LL163;_LL162: goto _LL164;_LL163: if(*((int*)
_tmp1D0)!= 29)goto _LL165;_LL164: goto _LL166;_LL165: if(*((int*)_tmp1D0)!= 30)goto
_LL167;_LL166: goto _LL168;_LL167: if(*((int*)_tmp1D0)!= 31)goto _LL169;_LL168: goto
_LL16A;_LL169: if(*((int*)_tmp1D0)!= 32)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((
int*)_tmp1D0)!= 33)goto _LL16D;_LL16C: goto _LL16E;_LL16D: if(*((int*)_tmp1D0)!= 34)
goto _LL16F;_LL16E: return 140;_LL16F: if(*((int*)_tmp1D0)!= 35)goto _LL171;_LL170:
return 10000;_LL171: if(*((int*)_tmp1D0)!= 36)goto _LL173;_LL172: goto _LL174;_LL173:
if(*((int*)_tmp1D0)!= 37)goto _LL128;_LL174: return 140;_LL128:;}struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1D5=(void*)e->r;void*
_tmp1D6;struct _tuple0*_tmp1D7;struct _tuple0*_tmp1D8;void*_tmp1D9;struct Cyc_List_List*
_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Core_Opt*_tmp1DC;struct Cyc_Absyn_Exp*
_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;void*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;
struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*
_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_List_List*
_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_Absyn_Exp*
_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EB;void*_tmp1EC;
struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*
_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;void*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;
void*_tmp1F3;void*_tmp1F4;struct _tagged_arr*_tmp1F5;void*_tmp1F6;void*_tmp1F7;
unsigned int _tmp1F8;struct Cyc_List_List*_tmp1F9;void*_tmp1FA;struct Cyc_Absyn_Exp*
_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct _tagged_arr*_tmp1FD;struct Cyc_Absyn_Exp*
_tmp1FE;struct _tagged_arr*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Absyn_Exp*
_tmp201;struct Cyc_List_List*_tmp202;struct _tuple1*_tmp203;struct Cyc_List_List*
_tmp204;struct Cyc_List_List*_tmp205;struct Cyc_Absyn_Vardecl*_tmp206;struct Cyc_Absyn_Exp*
_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct _tuple0*_tmp209;struct Cyc_List_List*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*
_tmp20D;struct Cyc_Absyn_Tunionfield*_tmp20E;struct _tuple0*_tmp20F;struct _tuple0*
_tmp210;struct Cyc_Absyn_MallocInfo _tmp211;int _tmp212;struct Cyc_Absyn_Exp*_tmp213;
void**_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Core_Opt*_tmp216;struct Cyc_List_List*
_tmp217;struct Cyc_Absyn_Stmt*_tmp218;struct Cyc_Absyn_Fndecl*_tmp219;struct Cyc_Absyn_Exp*
_tmp21A;_LL19F: if(*((int*)_tmp1D5)!= 0)goto _LL1A1;_tmp1D6=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp1D5)->f1;_LL1A0: s=Cyc_Absynpp_cnst2doc(_tmp1D6);goto _LL19E;_LL1A1: if(*((int*)
_tmp1D5)!= 1)goto _LL1A3;_tmp1D7=((struct Cyc_Absyn_Var_e_struct*)_tmp1D5)->f1;
_LL1A2: _tmp1D8=_tmp1D7;goto _LL1A4;_LL1A3: if(*((int*)_tmp1D5)!= 2)goto _LL1A5;
_tmp1D8=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1D5)->f1;_LL1A4: s=Cyc_Absynpp_qvar2doc(
_tmp1D8);goto _LL19E;_LL1A5: if(*((int*)_tmp1D5)!= 3)goto _LL1A7;_tmp1D9=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1D5)->f1;_tmp1DA=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D5)->f2;_LL1A6: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1D9,_tmp1DA);goto _LL19E;
_LL1A7: if(*((int*)_tmp1D5)!= 4)goto _LL1A9;_tmp1DB=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D5)->f1;_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f2;_tmp1DD=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f3;_LL1A8: s=({struct Cyc_PP_Doc*
_tmp21B[5];_tmp21B[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DD);_tmp21B[3]=Cyc_PP_text(({
const char*_tmp21E="= ";_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,
3));}));_tmp21B[2]=_tmp1DC == 0?Cyc_PP_text(({const char*_tmp21D="";_tag_arr(
_tmp21D,sizeof(char),_get_zero_arr_size(_tmp21D,1));})): Cyc_Absynpp_prim2doc((
void*)_tmp1DC->v);_tmp21B[1]=Cyc_PP_text(({const char*_tmp21C=" ";_tag_arr(
_tmp21C,sizeof(char),_get_zero_arr_size(_tmp21C,2));}));_tmp21B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DB);Cyc_PP_cat(_tag_arr(_tmp21B,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL19E;_LL1A9: if(*((int*)_tmp1D5)!= 5)goto _LL1AB;_tmp1DE=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1D5)->f1;_tmp1DF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f2;
_LL1AA: {struct Cyc_PP_Doc*_tmp21F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DE);{void*
_tmp220=_tmp1DF;_LL1EE: if((int)_tmp220 != 0)goto _LL1F0;_LL1EF: s=({struct Cyc_PP_Doc*
_tmp221[2];_tmp221[1]=_tmp21F;_tmp221[0]=Cyc_PP_text(({const char*_tmp222="++";
_tag_arr(_tmp222,sizeof(char),_get_zero_arr_size(_tmp222,3));}));Cyc_PP_cat(
_tag_arr(_tmp221,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1ED;_LL1F0: if((int)
_tmp220 != 2)goto _LL1F2;_LL1F1: s=({struct Cyc_PP_Doc*_tmp223[2];_tmp223[1]=_tmp21F;
_tmp223[0]=Cyc_PP_text(({const char*_tmp224="--";_tag_arr(_tmp224,sizeof(char),
_get_zero_arr_size(_tmp224,3));}));Cyc_PP_cat(_tag_arr(_tmp223,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1ED;_LL1F2: if((int)_tmp220 != 1)goto _LL1F4;_LL1F3: s=({struct Cyc_PP_Doc*
_tmp225[2];_tmp225[1]=Cyc_PP_text(({const char*_tmp226="++";_tag_arr(_tmp226,
sizeof(char),_get_zero_arr_size(_tmp226,3));}));_tmp225[0]=_tmp21F;Cyc_PP_cat(
_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1ED;_LL1F4: if((int)
_tmp220 != 3)goto _LL1ED;_LL1F5: s=({struct Cyc_PP_Doc*_tmp227[2];_tmp227[1]=Cyc_PP_text(({
const char*_tmp228="--";_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,
3));}));_tmp227[0]=_tmp21F;Cyc_PP_cat(_tag_arr(_tmp227,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1ED;_LL1ED:;}goto _LL19E;}_LL1AB: if(*((int*)_tmp1D5)!= 6)goto _LL1AD;
_tmp1E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f1;_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D5)->f2;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f3;_LL1AC:{
struct _tuple5 _tmp22A=({struct _tuple5 _tmp229;_tmp229.f1=(void*)_tmp1E1->r;_tmp229.f2=(
void*)_tmp1E2->r;_tmp229;});_LL1F7:;_LL1F8: s=({struct Cyc_PP_Doc*_tmp22B[5];
_tmp22B[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2);_tmp22B[3]=Cyc_PP_text(({
const char*_tmp22D=" : ";_tag_arr(_tmp22D,sizeof(char),_get_zero_arr_size(_tmp22D,
4));}));_tmp22B[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp1E1);_tmp22B[1]=Cyc_PP_text(({
const char*_tmp22C=" ? ";_tag_arr(_tmp22C,sizeof(char),_get_zero_arr_size(_tmp22C,
4));}));_tmp22B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E0);Cyc_PP_cat(_tag_arr(
_tmp22B,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1F6;_LL1F6:;}goto _LL19E;_LL1AD:
if(*((int*)_tmp1D5)!= 7)goto _LL1AF;_tmp1E3=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D5)->f1;_tmp1E4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D5)->f2;_LL1AE: s=({
struct Cyc_PP_Doc*_tmp22E[5];_tmp22E[4]=Cyc_PP_text(({const char*_tmp231=")";
_tag_arr(_tmp231,sizeof(char),_get_zero_arr_size(_tmp231,2));}));_tmp22E[3]=Cyc_Absynpp_exp2doc(
_tmp1E4);_tmp22E[2]=Cyc_PP_text(({const char*_tmp230=", ";_tag_arr(_tmp230,
sizeof(char),_get_zero_arr_size(_tmp230,3));}));_tmp22E[1]=Cyc_Absynpp_exp2doc(
_tmp1E3);_tmp22E[0]=Cyc_PP_text(({const char*_tmp22F="(";_tag_arr(_tmp22F,sizeof(
char),_get_zero_arr_size(_tmp22F,2));}));Cyc_PP_cat(_tag_arr(_tmp22E,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1AF: if(*((int*)_tmp1D5)!= 8)goto _LL1B1;
_tmp1E5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D5)->f1;_tmp1E6=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D5)->f2;_LL1B0: s=({struct Cyc_PP_Doc*_tmp232[4];_tmp232[3]=Cyc_PP_text(({
const char*_tmp234=")";_tag_arr(_tmp234,sizeof(char),_get_zero_arr_size(_tmp234,2));}));
_tmp232[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1E6);_tmp232[1]=Cyc_PP_text(({const
char*_tmp233="(";_tag_arr(_tmp233,sizeof(char),_get_zero_arr_size(_tmp233,2));}));
_tmp232[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E5);Cyc_PP_cat(_tag_arr(_tmp232,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1B1: if(*((int*)_tmp1D5)!= 9)goto
_LL1B3;_tmp1E7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f1;_tmp1E8=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f2;_LL1B2: s=({struct Cyc_PP_Doc*_tmp235[4];
_tmp235[3]=Cyc_PP_text(({const char*_tmp237=")";_tag_arr(_tmp237,sizeof(char),
_get_zero_arr_size(_tmp237,2));}));_tmp235[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1E8);_tmp235[1]=Cyc_PP_text(({const char*_tmp236="(";_tag_arr(_tmp236,sizeof(
char),_get_zero_arr_size(_tmp236,2));}));_tmp235[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E7);Cyc_PP_cat(_tag_arr(_tmp235,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL19E;_LL1B3: if(*((int*)_tmp1D5)!= 10)goto _LL1B5;_tmp1E9=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D5)->f1;_LL1B4: s=({struct Cyc_PP_Doc*_tmp238[2];_tmp238[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9);_tmp238[0]=Cyc_PP_text(({const char*_tmp239="throw ";_tag_arr(
_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,7));}));Cyc_PP_cat(_tag_arr(
_tmp238,sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1B5: if(*((int*)_tmp1D5)!= 
11)goto _LL1B7;_tmp1EA=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D5)->f1;
_LL1B6: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EA);goto _LL19E;_LL1B7: if(*((int*)
_tmp1D5)!= 12)goto _LL1B9;_tmp1EB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D5)->f1;
_LL1B8: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EB);goto _LL19E;_LL1B9: if(*((int*)
_tmp1D5)!= 13)goto _LL1BB;_tmp1EC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f1;
_tmp1ED=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f2;_LL1BA: s=({struct Cyc_PP_Doc*
_tmp23A[4];_tmp23A[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1ED);_tmp23A[2]=Cyc_PP_text(({
const char*_tmp23C=")";_tag_arr(_tmp23C,sizeof(char),_get_zero_arr_size(_tmp23C,2));}));
_tmp23A[1]=Cyc_Absynpp_typ2doc(_tmp1EC);_tmp23A[0]=Cyc_PP_text(({const char*
_tmp23B="(";_tag_arr(_tmp23B,sizeof(char),_get_zero_arr_size(_tmp23B,2));}));Cyc_PP_cat(
_tag_arr(_tmp23A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1BB: if(*((int*)
_tmp1D5)!= 14)goto _LL1BD;_tmp1EE=((struct Cyc_Absyn_Address_e_struct*)_tmp1D5)->f1;
_LL1BC: s=({struct Cyc_PP_Doc*_tmp23D[2];_tmp23D[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EE);_tmp23D[0]=Cyc_PP_text(({const char*_tmp23E="&";_tag_arr(_tmp23E,
sizeof(char),_get_zero_arr_size(_tmp23E,2));}));Cyc_PP_cat(_tag_arr(_tmp23D,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1BD: if(*((int*)_tmp1D5)!= 15)goto
_LL1BF;_tmp1EF=((struct Cyc_Absyn_New_e_struct*)_tmp1D5)->f1;_tmp1F0=((struct Cyc_Absyn_New_e_struct*)
_tmp1D5)->f2;_LL1BE: if(_tmp1EF == 0)s=({struct Cyc_PP_Doc*_tmp23F[2];_tmp23F[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F0);_tmp23F[0]=Cyc_PP_text(({const char*
_tmp240="new ";_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,5));}));
Cyc_PP_cat(_tag_arr(_tmp23F,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp241[4];_tmp241[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F0);_tmp241[2]=Cyc_PP_text(({
const char*_tmp243=") ";_tag_arr(_tmp243,sizeof(char),_get_zero_arr_size(_tmp243,
3));}));_tmp241[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp1EF);_tmp241[0]=
Cyc_PP_text(({const char*_tmp242="rnew(";_tag_arr(_tmp242,sizeof(char),
_get_zero_arr_size(_tmp242,6));}));Cyc_PP_cat(_tag_arr(_tmp241,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL19E;_LL1BF: if(*((int*)_tmp1D5)!= 16)goto _LL1C1;_tmp1F1=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D5)->f1;_LL1C0: s=({struct Cyc_PP_Doc*
_tmp244[3];_tmp244[2]=Cyc_PP_text(({const char*_tmp246=")";_tag_arr(_tmp246,
sizeof(char),_get_zero_arr_size(_tmp246,2));}));_tmp244[1]=Cyc_Absynpp_typ2doc(
_tmp1F1);_tmp244[0]=Cyc_PP_text(({const char*_tmp245="sizeof(";_tag_arr(_tmp245,
sizeof(char),_get_zero_arr_size(_tmp245,8));}));Cyc_PP_cat(_tag_arr(_tmp244,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1C1: if(*((int*)_tmp1D5)!= 17)goto
_LL1C3;_tmp1F2=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D5)->f1;_LL1C2: s=({
struct Cyc_PP_Doc*_tmp247[3];_tmp247[2]=Cyc_PP_text(({const char*_tmp249=")";
_tag_arr(_tmp249,sizeof(char),_get_zero_arr_size(_tmp249,2));}));_tmp247[1]=Cyc_Absynpp_exp2doc(
_tmp1F2);_tmp247[0]=Cyc_PP_text(({const char*_tmp248="sizeof(";_tag_arr(_tmp248,
sizeof(char),_get_zero_arr_size(_tmp248,8));}));Cyc_PP_cat(_tag_arr(_tmp247,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1C3: if(*((int*)_tmp1D5)!= 18)goto
_LL1C5;_tmp1F3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;_tmp1F4=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)_tmp1F4)!= 0)
goto _LL1C5;_tmp1F5=((struct Cyc_Absyn_StructField_struct*)_tmp1F4)->f1;_LL1C4: s=({
struct Cyc_PP_Doc*_tmp24A[5];_tmp24A[4]=Cyc_PP_text(({const char*_tmp24D=")";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,2));}));_tmp24A[3]=Cyc_PP_textptr(
_tmp1F5);_tmp24A[2]=Cyc_PP_text(({const char*_tmp24C=",";_tag_arr(_tmp24C,sizeof(
char),_get_zero_arr_size(_tmp24C,2));}));_tmp24A[1]=Cyc_Absynpp_typ2doc(_tmp1F3);
_tmp24A[0]=Cyc_PP_text(({const char*_tmp24B="offsetof(";_tag_arr(_tmp24B,sizeof(
char),_get_zero_arr_size(_tmp24B,10));}));Cyc_PP_cat(_tag_arr(_tmp24A,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1C5: if(*((int*)_tmp1D5)!= 18)goto _LL1C7;
_tmp1F6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;_tmp1F7=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)_tmp1F7)!= 1)goto
_LL1C7;_tmp1F8=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1F7)->f1;_LL1C6: s=({
struct Cyc_PP_Doc*_tmp24E[5];_tmp24E[4]=Cyc_PP_text(({const char*_tmp254=")";
_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,2));}));_tmp24E[3]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp253;_tmp253.tag=1;_tmp253.f1=(
unsigned int)((int)_tmp1F8);{void*_tmp251[1]={& _tmp253};Cyc_aprintf(({const char*
_tmp252="%d";_tag_arr(_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,3));}),
_tag_arr(_tmp251,sizeof(void*),1));}}));_tmp24E[2]=Cyc_PP_text(({const char*
_tmp250=",";_tag_arr(_tmp250,sizeof(char),_get_zero_arr_size(_tmp250,2));}));
_tmp24E[1]=Cyc_Absynpp_typ2doc(_tmp1F6);_tmp24E[0]=Cyc_PP_text(({const char*
_tmp24F="offsetof(";_tag_arr(_tmp24F,sizeof(char),_get_zero_arr_size(_tmp24F,10));}));
Cyc_PP_cat(_tag_arr(_tmp24E,sizeof(struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1C7:
if(*((int*)_tmp1D5)!= 19)goto _LL1C9;_tmp1F9=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D5)->f1;_tmp1FA=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D5)->f2;
_LL1C8: s=({struct Cyc_PP_Doc*_tmp255[4];_tmp255[3]=Cyc_PP_text(({const char*
_tmp257=")";_tag_arr(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,2));}));
_tmp255[2]=Cyc_Absynpp_typ2doc(_tmp1FA);_tmp255[1]=Cyc_Absynpp_tvars2doc(_tmp1F9);
_tmp255[0]=Cyc_PP_text(({const char*_tmp256="__gen(";_tag_arr(_tmp256,sizeof(char),
_get_zero_arr_size(_tmp256,7));}));Cyc_PP_cat(_tag_arr(_tmp255,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL19E;_LL1C9: if(*((int*)_tmp1D5)!= 20)goto _LL1CB;_tmp1FB=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D5)->f1;_LL1CA: s=({struct Cyc_PP_Doc*_tmp258[2];_tmp258[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FB);_tmp258[0]=Cyc_PP_text(({const char*_tmp259="*";_tag_arr(_tmp259,
sizeof(char),_get_zero_arr_size(_tmp259,2));}));Cyc_PP_cat(_tag_arr(_tmp258,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1CB: if(*((int*)_tmp1D5)!= 21)goto
_LL1CD;_tmp1FC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f1;_tmp1FD=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f2;_LL1CC: s=({struct Cyc_PP_Doc*
_tmp25A[3];_tmp25A[2]=Cyc_PP_textptr(_tmp1FD);_tmp25A[1]=Cyc_PP_text(({const char*
_tmp25B=".";_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,2));}));
_tmp25A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FC);Cyc_PP_cat(_tag_arr(_tmp25A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1CD: if(*((int*)_tmp1D5)!= 22)goto
_LL1CF;_tmp1FE=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f1;_tmp1FF=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f2;_LL1CE: s=({struct Cyc_PP_Doc*
_tmp25C[3];_tmp25C[2]=Cyc_PP_textptr(_tmp1FF);_tmp25C[1]=Cyc_PP_text(({const char*
_tmp25D="->";_tag_arr(_tmp25D,sizeof(char),_get_zero_arr_size(_tmp25D,3));}));
_tmp25C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FE);Cyc_PP_cat(_tag_arr(_tmp25C,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1CF: if(*((int*)_tmp1D5)!= 23)goto
_LL1D1;_tmp200=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f1;_tmp201=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f2;_LL1D0: s=({struct Cyc_PP_Doc*
_tmp25E[4];_tmp25E[3]=Cyc_PP_text(({const char*_tmp260="]";_tag_arr(_tmp260,
sizeof(char),_get_zero_arr_size(_tmp260,2));}));_tmp25E[2]=Cyc_Absynpp_exp2doc(
_tmp201);_tmp25E[1]=Cyc_PP_text(({const char*_tmp25F="[";_tag_arr(_tmp25F,sizeof(
char),_get_zero_arr_size(_tmp25F,2));}));_tmp25E[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp200);Cyc_PP_cat(_tag_arr(_tmp25E,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL19E;_LL1D1: if(*((int*)_tmp1D5)!= 24)goto _LL1D3;_tmp202=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D5)->f1;_LL1D2: s=({struct Cyc_PP_Doc*_tmp261[4];_tmp261[3]=Cyc_PP_text(({
const char*_tmp263=")";_tag_arr(_tmp263,sizeof(char),_get_zero_arr_size(_tmp263,2));}));
_tmp261[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp202);_tmp261[1]=Cyc_PP_text(({const
char*_tmp262="(";_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,2));}));
_tmp261[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp261,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL19E;_LL1D3: if(*((int*)_tmp1D5)!= 25)goto _LL1D5;_tmp203=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D5)->f1;_tmp204=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D5)->f2;_LL1D4:
s=({struct Cyc_PP_Doc*_tmp264[4];_tmp264[3]=Cyc_Absynpp_group_braces(({const char*
_tmp267=",";_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp204));_tmp264[2]=Cyc_PP_text(({const char*
_tmp266=")";_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,2));}));
_tmp264[1]=Cyc_Absynpp_typ2doc((*_tmp203).f3);_tmp264[0]=Cyc_PP_text(({const char*
_tmp265="(";_tag_arr(_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,2));}));Cyc_PP_cat(
_tag_arr(_tmp264,sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1D5: if(*((int*)
_tmp1D5)!= 26)goto _LL1D7;_tmp205=((struct Cyc_Absyn_Array_e_struct*)_tmp1D5)->f1;
_LL1D6: s=Cyc_Absynpp_group_braces(({const char*_tmp268=",";_tag_arr(_tmp268,
sizeof(char),_get_zero_arr_size(_tmp268,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp205));goto _LL19E;_LL1D7: if(*((int*)_tmp1D5)!= 27)goto _LL1D9;_tmp206=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f1;_tmp207=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D5)->f2;_tmp208=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f3;
_LL1D8: s=({struct Cyc_PP_Doc*_tmp269[8];_tmp269[7]=Cyc_Absynpp_rb();_tmp269[6]=
Cyc_Absynpp_exp2doc(_tmp208);_tmp269[5]=Cyc_PP_text(({const char*_tmp26C=" : ";
_tag_arr(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,4));}));_tmp269[4]=Cyc_Absynpp_exp2doc(
_tmp207);_tmp269[3]=Cyc_PP_text(({const char*_tmp26B=" < ";_tag_arr(_tmp26B,
sizeof(char),_get_zero_arr_size(_tmp26B,4));}));_tmp269[2]=Cyc_PP_text(*(*
_tmp206->name).f2);_tmp269[1]=Cyc_PP_text(({const char*_tmp26A="for ";_tag_arr(
_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,5));}));_tmp269[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp269,sizeof(struct Cyc_PP_Doc*),8));});goto _LL19E;_LL1D9:
if(*((int*)_tmp1D5)!= 28)goto _LL1DB;_tmp209=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D5)->f1;_tmp20A=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f2;_tmp20B=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f3;_LL1DA: {struct Cyc_List_List*
_tmp26D=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20B);s=({struct Cyc_PP_Doc*
_tmp26E[2];_tmp26E[1]=Cyc_Absynpp_group_braces(({const char*_tmp26F=",";_tag_arr(
_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,2));}),_tmp20A != 0?({struct Cyc_List_List*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->hd=Cyc_Absynpp_tps2doc(_tmp20A);
_tmp270->tl=_tmp26D;_tmp270;}): _tmp26D);_tmp26E[0]=Cyc_Absynpp_qvar2doc(_tmp209);
Cyc_PP_cat(_tag_arr(_tmp26E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;}_LL1DB:
if(*((int*)_tmp1D5)!= 29)goto _LL1DD;_tmp20C=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D5)->f2;_LL1DC: s=Cyc_Absynpp_group_braces(({const char*_tmp271=",";_tag_arr(
_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20C));goto _LL19E;_LL1DD: if(*((int*)_tmp1D5)!= 30)goto _LL1DF;_tmp20D=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1D5)->f1;_tmp20E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1D5)->f3;_LL1DE: if(_tmp20D == 0)s=Cyc_Absynpp_qvar2doc(_tmp20E->name);else{s=({
struct Cyc_PP_Doc*_tmp272[2];_tmp272[1]=Cyc_PP_egroup(({const char*_tmp273="(";
_tag_arr(_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,2));}),({const char*
_tmp274=")";_tag_arr(_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,2));}),({
const char*_tmp275=",";_tag_arr(_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp20D));_tmp272[0]=Cyc_Absynpp_qvar2doc(
_tmp20E->name);Cyc_PP_cat(_tag_arr(_tmp272,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL19E;_LL1DF: if(*((int*)_tmp1D5)!= 31)goto _LL1E1;_tmp20F=((struct Cyc_Absyn_Enum_e_struct*)
_tmp1D5)->f1;_LL1E0: s=Cyc_Absynpp_qvar2doc(_tmp20F);goto _LL19E;_LL1E1: if(*((int*)
_tmp1D5)!= 32)goto _LL1E3;_tmp210=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D5)->f1;
_LL1E2: s=Cyc_Absynpp_qvar2doc(_tmp210);goto _LL19E;_LL1E3: if(*((int*)_tmp1D5)!= 
33)goto _LL1E5;_tmp211=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D5)->f1;_tmp212=
_tmp211.is_calloc;_tmp213=_tmp211.rgn;_tmp214=_tmp211.elt_type;_tmp215=_tmp211.num_elts;
_LL1E4: if(_tmp212){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmp214)),0);if(_tmp213 == 0)s=({struct Cyc_PP_Doc*_tmp276[5];_tmp276[
4]=Cyc_PP_text(({const char*_tmp279=")";_tag_arr(_tmp279,sizeof(char),
_get_zero_arr_size(_tmp279,2));}));_tmp276[3]=Cyc_Absynpp_exp2doc(st);_tmp276[2]=
Cyc_PP_text(({const char*_tmp278=",";_tag_arr(_tmp278,sizeof(char),
_get_zero_arr_size(_tmp278,2));}));_tmp276[1]=Cyc_Absynpp_exp2doc(_tmp215);
_tmp276[0]=Cyc_PP_text(({const char*_tmp277="calloc(";_tag_arr(_tmp277,sizeof(
char),_get_zero_arr_size(_tmp277,8));}));Cyc_PP_cat(_tag_arr(_tmp276,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp27A[7];_tmp27A[6]=Cyc_PP_text(({
const char*_tmp27E=")";_tag_arr(_tmp27E,sizeof(char),_get_zero_arr_size(_tmp27E,2));}));
_tmp27A[5]=Cyc_Absynpp_exp2doc(st);_tmp27A[4]=Cyc_PP_text(({const char*_tmp27D=",";
_tag_arr(_tmp27D,sizeof(char),_get_zero_arr_size(_tmp27D,2));}));_tmp27A[3]=Cyc_Absynpp_exp2doc(
_tmp215);_tmp27A[2]=Cyc_PP_text(({const char*_tmp27C=",";_tag_arr(_tmp27C,sizeof(
char),_get_zero_arr_size(_tmp27C,2));}));_tmp27A[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp213);_tmp27A[0]=Cyc_PP_text(({const char*_tmp27B="rcalloc(";
_tag_arr(_tmp27B,sizeof(char),_get_zero_arr_size(_tmp27B,9));}));Cyc_PP_cat(
_tag_arr(_tmp27A,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp214 == 0)new_e=_tmp215;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp214,0),_tmp215,0);}if(_tmp213 == 0)s=({struct Cyc_PP_Doc*_tmp27F[3];_tmp27F[2]=
Cyc_PP_text(({const char*_tmp281=")";_tag_arr(_tmp281,sizeof(char),
_get_zero_arr_size(_tmp281,2));}));_tmp27F[1]=Cyc_Absynpp_exp2doc(new_e);_tmp27F[
0]=Cyc_PP_text(({const char*_tmp280="malloc(";_tag_arr(_tmp280,sizeof(char),
_get_zero_arr_size(_tmp280,8));}));Cyc_PP_cat(_tag_arr(_tmp27F,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp282[5];_tmp282[4]=Cyc_PP_text(({const char*
_tmp285=")";_tag_arr(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,2));}));
_tmp282[3]=Cyc_Absynpp_exp2doc(new_e);_tmp282[2]=Cyc_PP_text(({const char*_tmp284=",";
_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,2));}));_tmp282[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp213);_tmp282[0]=Cyc_PP_text(({const char*_tmp283="rmalloc(";
_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,9));}));Cyc_PP_cat(
_tag_arr(_tmp282,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL19E;_LL1E5: if(*((int*)
_tmp1D5)!= 34)goto _LL1E7;_tmp216=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D5)->f1;_tmp217=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D5)->f2;
_LL1E6: s=Cyc_Absynpp_group_braces(({const char*_tmp286=",";_tag_arr(_tmp286,
sizeof(char),_get_zero_arr_size(_tmp286,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp217));goto _LL19E;_LL1E7: if(*((int*)_tmp1D5)!= 35)goto _LL1E9;_tmp218=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1D5)->f1;_LL1E8: s=({struct Cyc_PP_Doc*_tmp287[7];
_tmp287[6]=Cyc_PP_text(({const char*_tmp289=")";_tag_arr(_tmp289,sizeof(char),
_get_zero_arr_size(_tmp289,2));}));_tmp287[5]=Cyc_Absynpp_rb();_tmp287[4]=Cyc_PP_blank_doc();
_tmp287[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp218));_tmp287[2]=Cyc_PP_blank_doc();
_tmp287[1]=Cyc_Absynpp_lb();_tmp287[0]=Cyc_PP_text(({const char*_tmp288="(";
_tag_arr(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,2));}));Cyc_PP_cat(
_tag_arr(_tmp287,sizeof(struct Cyc_PP_Doc*),7));});goto _LL19E;_LL1E9: if(*((int*)
_tmp1D5)!= 36)goto _LL1EB;_tmp219=((struct Cyc_Absyn_Codegen_e_struct*)_tmp1D5)->f1;
_LL1EA: s=({struct Cyc_PP_Doc*_tmp28A[3];_tmp28A[2]=Cyc_PP_text(({const char*
_tmp28F=")";_tag_arr(_tmp28F,sizeof(char),_get_zero_arr_size(_tmp28F,2));}));
_tmp28A[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp28C=
_cycalloc(sizeof(*_tmp28C));_tmp28C->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp28E;_tmp28E.tag=1;_tmp28E.f1=_tmp219;_tmp28E;});_tmp28D;}));_tmp28C->loc=e->loc;
_tmp28C;})));_tmp28A[0]=Cyc_PP_text(({const char*_tmp28B="codegen(";_tag_arr(
_tmp28B,sizeof(char),_get_zero_arr_size(_tmp28B,9));}));Cyc_PP_cat(_tag_arr(
_tmp28A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1EB: if(*((int*)_tmp1D5)!= 
37)goto _LL19E;_tmp21A=((struct Cyc_Absyn_Fill_e_struct*)_tmp1D5)->f1;_LL1EC: s=({
struct Cyc_PP_Doc*_tmp290[3];_tmp290[2]=Cyc_PP_text(({const char*_tmp292=")";
_tag_arr(_tmp292,sizeof(char),_get_zero_arr_size(_tmp292,2));}));_tmp290[1]=Cyc_PP_nest(
2,Cyc_Absynpp_exp2doc(_tmp21A));_tmp290[0]=Cyc_PP_text(({const char*_tmp291="fill(";
_tag_arr(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,6));}));Cyc_PP_cat(
_tag_arr(_tmp290,sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL19E:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp293[3];_tmp293[2]=Cyc_PP_text(({const char*
_tmp295=")";_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,2));}));
_tmp293[1]=s;_tmp293[0]=Cyc_PP_text(({const char*_tmp294="(";_tag_arr(_tmp294,
sizeof(char),_get_zero_arr_size(_tmp294,2));}));Cyc_PP_cat(_tag_arr(_tmp293,
sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp296=d;struct Cyc_Absyn_Exp*_tmp297;struct _tagged_arr*_tmp298;
_LL1FA: if(*((int*)_tmp296)!= 0)goto _LL1FC;_tmp297=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp296)->f1;_LL1FB: return({struct Cyc_PP_Doc*_tmp299[3];_tmp299[2]=Cyc_PP_text(({
const char*_tmp29B="]";_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,2));}));
_tmp299[1]=Cyc_Absynpp_exp2doc(_tmp297);_tmp299[0]=Cyc_PP_text(({const char*
_tmp29A=".[";_tag_arr(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,3));}));
Cyc_PP_cat(_tag_arr(_tmp299,sizeof(struct Cyc_PP_Doc*),3));});_LL1FC: if(*((int*)
_tmp296)!= 1)goto _LL1F9;_tmp298=((struct Cyc_Absyn_FieldName_struct*)_tmp296)->f1;
_LL1FD: return({struct Cyc_PP_Doc*_tmp29C[2];_tmp29C[1]=Cyc_PP_textptr(_tmp298);
_tmp29C[0]=Cyc_PP_text(({const char*_tmp29D=".";_tag_arr(_tmp29D,sizeof(char),
_get_zero_arr_size(_tmp29D,2));}));Cyc_PP_cat(_tag_arr(_tmp29C,sizeof(struct Cyc_PP_Doc*),
2));});_LL1F9:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1
== 0)return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp29E[2];
_tmp29E[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp29E[0]=Cyc_PP_egroup(({const char*
_tmp29F="";_tag_arr(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,1));}),({
const char*_tmp2A0="=";_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,2));}),({
const char*_tmp2A1="=";_tag_arr(_tmp2A1,sizeof(char),_get_zero_arr_size(_tmp2A1,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp29E,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp2A2="";_tag_arr(
_tmp2A2,sizeof(char),_get_zero_arr_size(_tmp2A2,1));}),({const char*_tmp2A3="";
_tag_arr(_tmp2A3,sizeof(char),_get_zero_arr_size(_tmp2A3,1));}),({const char*
_tmp2A4=",";_tag_arr(_tmp2A4,sizeof(char),_get_zero_arr_size(_tmp2A4,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp2A5=c;void*_tmp2A6;char
_tmp2A7;void*_tmp2A8;short _tmp2A9;void*_tmp2AA;int _tmp2AB;void*_tmp2AC;int
_tmp2AD;void*_tmp2AE;int _tmp2AF;void*_tmp2B0;long long _tmp2B1;struct _tagged_arr
_tmp2B2;struct _tagged_arr _tmp2B3;_LL1FF: if(_tmp2A5 <= (void*)1?1:*((int*)_tmp2A5)
!= 0)goto _LL201;_tmp2A6=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp2A5)->f1;
_tmp2A7=((struct Cyc_Absyn_Char_c_struct*)_tmp2A5)->f2;_LL200: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp2A7));{void*
_tmp2B4[1]={& _tmp2B6};Cyc_aprintf(({const char*_tmp2B5="'%s'";_tag_arr(_tmp2B5,
sizeof(char),_get_zero_arr_size(_tmp2B5,5));}),_tag_arr(_tmp2B4,sizeof(void*),1));}}));
_LL201: if(_tmp2A5 <= (void*)1?1:*((int*)_tmp2A5)!= 1)goto _LL203;_tmp2A8=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp2A5)->f1;_tmp2A9=((struct Cyc_Absyn_Short_c_struct*)
_tmp2A5)->f2;_LL202: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp2B9;_tmp2B9.tag=1;_tmp2B9.f1=(unsigned int)((int)_tmp2A9);{void*_tmp2B7[1]={&
_tmp2B9};Cyc_aprintf(({const char*_tmp2B8="%d";_tag_arr(_tmp2B8,sizeof(char),
_get_zero_arr_size(_tmp2B8,3));}),_tag_arr(_tmp2B7,sizeof(void*),1));}}));_LL203:
if(_tmp2A5 <= (void*)1?1:*((int*)_tmp2A5)!= 2)goto _LL205;_tmp2AA=(void*)((struct
Cyc_Absyn_Int_c_struct*)_tmp2A5)->f1;if((int)_tmp2AA != 2)goto _LL205;_tmp2AB=((
struct Cyc_Absyn_Int_c_struct*)_tmp2A5)->f2;_LL204: _tmp2AD=_tmp2AB;goto _LL206;
_LL205: if(_tmp2A5 <= (void*)1?1:*((int*)_tmp2A5)!= 2)goto _LL207;_tmp2AC=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp2A5)->f1;if((int)_tmp2AC != 0)goto _LL207;
_tmp2AD=((struct Cyc_Absyn_Int_c_struct*)_tmp2A5)->f2;_LL206: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2BC;_tmp2BC.tag=1;_tmp2BC.f1=(
unsigned int)_tmp2AD;{void*_tmp2BA[1]={& _tmp2BC};Cyc_aprintf(({const char*_tmp2BB="%d";
_tag_arr(_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,3));}),_tag_arr(_tmp2BA,
sizeof(void*),1));}}));_LL207: if(_tmp2A5 <= (void*)1?1:*((int*)_tmp2A5)!= 2)goto
_LL209;_tmp2AE=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2A5)->f1;if((int)
_tmp2AE != 1)goto _LL209;_tmp2AF=((struct Cyc_Absyn_Int_c_struct*)_tmp2A5)->f2;
_LL208: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2BF;
_tmp2BF.tag=1;_tmp2BF.f1=(unsigned int)_tmp2AF;{void*_tmp2BD[1]={& _tmp2BF};Cyc_aprintf(({
const char*_tmp2BE="%u";_tag_arr(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,
3));}),_tag_arr(_tmp2BD,sizeof(void*),1));}}));_LL209: if(_tmp2A5 <= (void*)1?1:*((
int*)_tmp2A5)!= 3)goto _LL20B;_tmp2B0=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp2A5)->f1;_tmp2B1=((struct Cyc_Absyn_LongLong_c_struct*)_tmp2A5)->f2;_LL20A:
return Cyc_PP_text(({const char*_tmp2C0="<<FIX LONG LONG CONSTANT>>";_tag_arr(
_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,27));}));_LL20B: if(_tmp2A5 <= (
void*)1?1:*((int*)_tmp2A5)!= 4)goto _LL20D;_tmp2B2=((struct Cyc_Absyn_Float_c_struct*)
_tmp2A5)->f1;_LL20C: return Cyc_PP_text(_tmp2B2);_LL20D: if((int)_tmp2A5 != 0)goto
_LL20F;_LL20E: return Cyc_PP_text(({const char*_tmp2C1="NULL";_tag_arr(_tmp2C1,
sizeof(char),_get_zero_arr_size(_tmp2C1,5));}));_LL20F: if(_tmp2A5 <= (void*)1?1:*((
int*)_tmp2A5)!= 5)goto _LL1FE;_tmp2B3=((struct Cyc_Absyn_String_c_struct*)_tmp2A5)->f1;
_LL210: return({struct Cyc_PP_Doc*_tmp2C2[3];_tmp2C2[2]=Cyc_PP_text(({const char*
_tmp2C4="\"";_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,2));}));
_tmp2C2[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2B3));_tmp2C2[0]=Cyc_PP_text(({
const char*_tmp2C3="\"";_tag_arr(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,
2));}));Cyc_PP_cat(_tag_arr(_tmp2C2,sizeof(struct Cyc_PP_Doc*),3));});_LL1FE:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0?
1: es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C5=
_cycalloc(sizeof(*_tmp2C5));_tmp2C5[0]=({struct Cyc_Core_Failure_struct _tmp2C6;
_tmp2C6.tag=Cyc_Core_Failure;_tmp2C6.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2C7[1]={& _tmp2C9};Cyc_aprintf(({const char*_tmp2C8="Absynpp::primapp2doc Size: %s with bad args";
_tag_arr(_tmp2C8,sizeof(char),_get_zero_arr_size(_tmp2C8,44));}),_tag_arr(
_tmp2C7,sizeof(void*),1));}});_tmp2C6;});_tmp2C5;}));{struct Cyc_PP_Doc*_tmp2CA=
Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*
_tmp2CB[2];_tmp2CB[1]=Cyc_PP_text(({const char*_tmp2CC=".size";_tag_arr(_tmp2CC,
sizeof(char),_get_zero_arr_size(_tmp2CC,6));}));_tmp2CB[0]=_tmp2CA;Cyc_PP_cat(
_tag_arr(_tmp2CB,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2CD=_cycalloc(
sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Core_Failure_struct _tmp2CE;_tmp2CE.tag=
Cyc_Core_Failure;_tmp2CE.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2CF[1]={& _tmp2D1};Cyc_aprintf(({const char*_tmp2D0="Absynpp::primapp2doc: %s with no args";
_tag_arr(_tmp2D0,sizeof(char),_get_zero_arr_size(_tmp2D0,38));}),_tag_arr(
_tmp2CF,sizeof(void*),1));}});_tmp2CE;});_tmp2CD;}));else{if(ds->tl == 0)return({
struct Cyc_PP_Doc*_tmp2D2[3];_tmp2D2[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2D2[1]=Cyc_PP_text(({
const char*_tmp2D3=" ";_tag_arr(_tmp2D3,sizeof(char),_get_zero_arr_size(_tmp2D3,2));}));
_tmp2D2[0]=ps;Cyc_PP_cat(_tag_arr(_tmp2D2,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({
struct Cyc_Core_Failure_struct _tmp2D5;_tmp2D5.tag=Cyc_Core_Failure;_tmp2D5.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2D8;_tmp2D8.tag=0;_tmp2D8.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(ps,72));{void*_tmp2D6[
1]={& _tmp2D8};Cyc_aprintf(({const char*_tmp2D7="Absynpp::primapp2doc: %s with more than 2 args";
_tag_arr(_tmp2D7,sizeof(char),_get_zero_arr_size(_tmp2D7,47));}),_tag_arr(
_tmp2D6,sizeof(void*),1));}});_tmp2D5;});_tmp2D4;}));else{return({struct Cyc_PP_Doc*
_tmp2D9[5];_tmp2D9[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp2D9[3]=Cyc_PP_text(({const char*_tmp2DB=" ";_tag_arr(_tmp2DB,sizeof(char),
_get_zero_arr_size(_tmp2DB,2));}));_tmp2D9[2]=ps;_tmp2D9[1]=Cyc_PP_text(({const
char*_tmp2DA=" ";_tag_arr(_tmp2DA,sizeof(char),_get_zero_arr_size(_tmp2DA,2));}));
_tmp2D9[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp2D9,sizeof(struct
Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){void*
_tmp2DC=p;_LL212: if((int)_tmp2DC != 0)goto _LL214;_LL213: return({const char*_tmp2DD="+";
_tag_arr(_tmp2DD,sizeof(char),_get_zero_arr_size(_tmp2DD,2));});_LL214: if((int)
_tmp2DC != 1)goto _LL216;_LL215: return({const char*_tmp2DE="*";_tag_arr(_tmp2DE,
sizeof(char),_get_zero_arr_size(_tmp2DE,2));});_LL216: if((int)_tmp2DC != 2)goto
_LL218;_LL217: return({const char*_tmp2DF="-";_tag_arr(_tmp2DF,sizeof(char),
_get_zero_arr_size(_tmp2DF,2));});_LL218: if((int)_tmp2DC != 3)goto _LL21A;_LL219:
return({const char*_tmp2E0="/";_tag_arr(_tmp2E0,sizeof(char),_get_zero_arr_size(
_tmp2E0,2));});_LL21A: if((int)_tmp2DC != 4)goto _LL21C;_LL21B: return({const char*
_tmp2E1="%";_tag_arr(_tmp2E1,sizeof(char),_get_zero_arr_size(_tmp2E1,2));});
_LL21C: if((int)_tmp2DC != 5)goto _LL21E;_LL21D: return({const char*_tmp2E2="==";
_tag_arr(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,3));});_LL21E: if((int)
_tmp2DC != 6)goto _LL220;_LL21F: return({const char*_tmp2E3="!=";_tag_arr(_tmp2E3,
sizeof(char),_get_zero_arr_size(_tmp2E3,3));});_LL220: if((int)_tmp2DC != 7)goto
_LL222;_LL221: return({const char*_tmp2E4=">";_tag_arr(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,2));});_LL222: if((int)_tmp2DC != 8)goto _LL224;_LL223:
return({const char*_tmp2E5="<";_tag_arr(_tmp2E5,sizeof(char),_get_zero_arr_size(
_tmp2E5,2));});_LL224: if((int)_tmp2DC != 9)goto _LL226;_LL225: return({const char*
_tmp2E6=">=";_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,3));});
_LL226: if((int)_tmp2DC != 10)goto _LL228;_LL227: return({const char*_tmp2E7="<=";
_tag_arr(_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,3));});_LL228: if((int)
_tmp2DC != 11)goto _LL22A;_LL229: return({const char*_tmp2E8="!";_tag_arr(_tmp2E8,
sizeof(char),_get_zero_arr_size(_tmp2E8,2));});_LL22A: if((int)_tmp2DC != 12)goto
_LL22C;_LL22B: return({const char*_tmp2E9="~";_tag_arr(_tmp2E9,sizeof(char),
_get_zero_arr_size(_tmp2E9,2));});_LL22C: if((int)_tmp2DC != 13)goto _LL22E;_LL22D:
return({const char*_tmp2EA="&";_tag_arr(_tmp2EA,sizeof(char),_get_zero_arr_size(
_tmp2EA,2));});_LL22E: if((int)_tmp2DC != 14)goto _LL230;_LL22F: return({const char*
_tmp2EB="|";_tag_arr(_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,2));});
_LL230: if((int)_tmp2DC != 15)goto _LL232;_LL231: return({const char*_tmp2EC="^";
_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,2));});_LL232: if((int)
_tmp2DC != 16)goto _LL234;_LL233: return({const char*_tmp2ED="<<";_tag_arr(_tmp2ED,
sizeof(char),_get_zero_arr_size(_tmp2ED,3));});_LL234: if((int)_tmp2DC != 17)goto
_LL236;_LL235: return({const char*_tmp2EE=">>";_tag_arr(_tmp2EE,sizeof(char),
_get_zero_arr_size(_tmp2EE,3));});_LL236: if((int)_tmp2DC != 18)goto _LL238;_LL237:
return({const char*_tmp2EF=">>>";_tag_arr(_tmp2EF,sizeof(char),_get_zero_arr_size(
_tmp2EF,4));});_LL238: if((int)_tmp2DC != 19)goto _LL211;_LL239: return({const char*
_tmp2F0="size";_tag_arr(_tmp2F0,sizeof(char),_get_zero_arr_size(_tmp2F0,5));});
_LL211:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2F1=(void*)s->r;
_LL23B: if(_tmp2F1 <= (void*)1?1:*((int*)_tmp2F1)!= 12)goto _LL23D;_LL23C: return 1;
_LL23D:;_LL23E: return 0;_LL23A:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp2F2=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F3;
struct Cyc_Absyn_Stmt*_tmp2F4;struct Cyc_Absyn_Stmt*_tmp2F5;struct Cyc_Absyn_Exp*
_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Stmt*_tmp2F8;struct Cyc_Absyn_Stmt*
_tmp2F9;struct _tuple2 _tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct Cyc_Absyn_Stmt*
_tmp2FC;struct _tagged_arr*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct _tuple2
_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;struct _tuple2 _tmp301;struct Cyc_Absyn_Exp*
_tmp302;struct Cyc_Absyn_Stmt*_tmp303;struct Cyc_Absyn_ForArrayInfo _tmp304;struct
Cyc_List_List*_tmp305;struct _tuple2 _tmp306;struct Cyc_Absyn_Exp*_tmp307;struct
_tuple2 _tmp308;struct Cyc_Absyn_Exp*_tmp309;struct Cyc_Absyn_Stmt*_tmp30A;struct
Cyc_Absyn_Exp*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;
struct Cyc_List_List*_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*
_tmp310;struct Cyc_Absyn_Decl*_tmp311;struct Cyc_Absyn_Stmt*_tmp312;struct Cyc_Absyn_Stmt*
_tmp313;struct Cyc_Absyn_Stmt*_tmp314;struct _tagged_arr*_tmp315;struct Cyc_Absyn_Stmt*
_tmp316;struct Cyc_Absyn_Stmt*_tmp317;struct _tuple2 _tmp318;struct Cyc_Absyn_Exp*
_tmp319;struct Cyc_Absyn_Stmt*_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_Absyn_Tvar*
_tmp31C;struct Cyc_Absyn_Vardecl*_tmp31D;int _tmp31E;struct Cyc_Absyn_Stmt*_tmp31F;
struct Cyc_Absyn_Exp*_tmp320;_LL240: if((int)_tmp2F2 != 0)goto _LL242;_LL241: s=Cyc_PP_text(({
const char*_tmp321=";";_tag_arr(_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,2));}));
goto _LL23F;_LL242: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 0)goto _LL244;
_tmp2F3=((struct Cyc_Absyn_Exp_s_struct*)_tmp2F2)->f1;_LL243: s=({struct Cyc_PP_Doc*
_tmp322[2];_tmp322[1]=Cyc_PP_text(({const char*_tmp323=";";_tag_arr(_tmp323,
sizeof(char),_get_zero_arr_size(_tmp323,2));}));_tmp322[0]=Cyc_Absynpp_exp2doc(
_tmp2F3);Cyc_PP_cat(_tag_arr(_tmp322,sizeof(struct Cyc_PP_Doc*),2));});goto _LL23F;
_LL244: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 1)goto _LL246;_tmp2F4=((struct
Cyc_Absyn_Seq_s_struct*)_tmp2F2)->f1;_tmp2F5=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F2)->f2;_LL245: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2F4))s=({struct Cyc_PP_Doc*_tmp324[7];_tmp324[6]=Cyc_Absynpp_is_declaration(
_tmp2F5)?({struct Cyc_PP_Doc*_tmp325[5];_tmp325[4]=Cyc_PP_line_doc();_tmp325[3]=
Cyc_Absynpp_rb();_tmp325[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F5));_tmp325[
1]=Cyc_PP_blank_doc();_tmp325[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp325,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2F5);_tmp324[5]=Cyc_PP_line_doc();
_tmp324[4]=Cyc_Absynpp_rb();_tmp324[3]=Cyc_PP_line_doc();_tmp324[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2F4));_tmp324[1]=Cyc_PP_blank_doc();_tmp324[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp324,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp2F5))s=({struct Cyc_PP_Doc*_tmp326[7];_tmp326[6]=Cyc_PP_line_doc();_tmp326[5]=
Cyc_Absynpp_rb();_tmp326[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F5));_tmp326[
3]=Cyc_PP_blank_doc();_tmp326[2]=Cyc_Absynpp_lb();_tmp326[1]=Cyc_PP_line_doc();
_tmp326[0]=Cyc_Absynpp_stmt2doc(_tmp2F4);Cyc_PP_cat(_tag_arr(_tmp326,sizeof(
struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,({const char*_tmp327="";_tag_arr(_tmp327,sizeof(char),
_get_zero_arr_size(_tmp327,1));}),({struct Cyc_List_List*_tmp328=_cycalloc(
sizeof(*_tmp328));_tmp328->hd=_tmp2F4;_tmp328->tl=({struct Cyc_List_List*_tmp329=
_cycalloc(sizeof(*_tmp329));_tmp329->hd=_tmp2F5;_tmp329->tl=0;_tmp329;});_tmp328;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp32A="";_tag_arr(_tmp32A,sizeof(char),_get_zero_arr_size(_tmp32A,1));}),({
struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->hd=_tmp2F4;
_tmp32B->tl=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->hd=
_tmp2F5;_tmp32C->tl=0;_tmp32C;});_tmp32B;}));}goto _LL23F;_LL246: if(_tmp2F2 <= (
void*)1?1:*((int*)_tmp2F2)!= 2)goto _LL248;_tmp2F6=((struct Cyc_Absyn_Return_s_struct*)
_tmp2F2)->f1;_LL247: if(_tmp2F6 == 0)s=Cyc_PP_text(({const char*_tmp32D="return;";
_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,8));}));else{s=({struct
Cyc_PP_Doc*_tmp32E[3];_tmp32E[2]=Cyc_PP_text(({const char*_tmp330=";";_tag_arr(
_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,2));}));_tmp32E[1]=_tmp2F6 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2F6);_tmp32E[0]=
Cyc_PP_text(({const char*_tmp32F="return ";_tag_arr(_tmp32F,sizeof(char),
_get_zero_arr_size(_tmp32F,8));}));Cyc_PP_cat(_tag_arr(_tmp32E,sizeof(struct Cyc_PP_Doc*),
3));});}goto _LL23F;_LL248: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 3)goto
_LL24A;_tmp2F7=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F2)->f1;_tmp2F8=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F2)->f2;_tmp2F9=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F2)->f3;_LL249: {int print_else;{void*_tmp331=(void*)_tmp2F9->r;_LL26F: if((
int)_tmp331 != 0)goto _LL271;_LL270: print_else=0;goto _LL26E;_LL271:;_LL272:
print_else=1;goto _LL26E;_LL26E:;}s=({struct Cyc_PP_Doc*_tmp332[8];_tmp332[7]=
print_else?({struct Cyc_PP_Doc*_tmp336[6];_tmp336[5]=Cyc_Absynpp_rb();_tmp336[4]=
Cyc_PP_line_doc();_tmp336[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp338[2];_tmp338[
1]=Cyc_Absynpp_stmt2doc(_tmp2F9);_tmp338[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp338,sizeof(struct Cyc_PP_Doc*),2));}));_tmp336[2]=Cyc_Absynpp_lb();
_tmp336[1]=Cyc_PP_text(({const char*_tmp337="else ";_tag_arr(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,6));}));_tmp336[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp336,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp332[6]=
Cyc_Absynpp_rb();_tmp332[5]=Cyc_PP_line_doc();_tmp332[4]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp335[2];_tmp335[1]=Cyc_Absynpp_stmt2doc(_tmp2F8);_tmp335[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp335,sizeof(struct Cyc_PP_Doc*),2));}));_tmp332[3]=Cyc_Absynpp_lb();
_tmp332[2]=Cyc_PP_text(({const char*_tmp334=") ";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,3));}));_tmp332[1]=Cyc_Absynpp_exp2doc(_tmp2F7);
_tmp332[0]=Cyc_PP_text(({const char*_tmp333="if (";_tag_arr(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,5));}));Cyc_PP_cat(_tag_arr(_tmp332,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL23F;}_LL24A: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 4)goto
_LL24C;_tmp2FA=((struct Cyc_Absyn_While_s_struct*)_tmp2F2)->f1;_tmp2FB=_tmp2FA.f1;
_tmp2FC=((struct Cyc_Absyn_While_s_struct*)_tmp2F2)->f2;_LL24B: s=({struct Cyc_PP_Doc*
_tmp339[7];_tmp339[6]=Cyc_Absynpp_rb();_tmp339[5]=Cyc_PP_line_doc();_tmp339[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp33C[2];_tmp33C[1]=Cyc_Absynpp_stmt2doc(
_tmp2FC);_tmp33C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp33C,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp339[3]=Cyc_Absynpp_lb();_tmp339[2]=Cyc_PP_text(({const
char*_tmp33B=") ";_tag_arr(_tmp33B,sizeof(char),_get_zero_arr_size(_tmp33B,3));}));
_tmp339[1]=Cyc_Absynpp_exp2doc(_tmp2FB);_tmp339[0]=Cyc_PP_text(({const char*
_tmp33A="while (";_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,8));}));
Cyc_PP_cat(_tag_arr(_tmp339,sizeof(struct Cyc_PP_Doc*),7));});goto _LL23F;_LL24C:
if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 5)goto _LL24E;_LL24D: s=Cyc_PP_text(({
const char*_tmp33D="break;";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,7));}));goto _LL23F;_LL24E: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 6)
goto _LL250;_LL24F: s=Cyc_PP_text(({const char*_tmp33E="continue;";_tag_arr(_tmp33E,
sizeof(char),_get_zero_arr_size(_tmp33E,10));}));goto _LL23F;_LL250: if(_tmp2F2 <= (
void*)1?1:*((int*)_tmp2F2)!= 7)goto _LL252;_tmp2FD=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2F2)->f1;_LL251: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp341;_tmp341.tag=0;_tmp341.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp2FD);{
void*_tmp33F[1]={& _tmp341};Cyc_aprintf(({const char*_tmp340="goto %s;";_tag_arr(
_tmp340,sizeof(char),_get_zero_arr_size(_tmp340,9));}),_tag_arr(_tmp33F,sizeof(
void*),1));}}));goto _LL23F;_LL252: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 8)
goto _LL254;_tmp2FE=((struct Cyc_Absyn_For_s_struct*)_tmp2F2)->f1;_tmp2FF=((struct
Cyc_Absyn_For_s_struct*)_tmp2F2)->f2;_tmp300=_tmp2FF.f1;_tmp301=((struct Cyc_Absyn_For_s_struct*)
_tmp2F2)->f3;_tmp302=_tmp301.f1;_tmp303=((struct Cyc_Absyn_For_s_struct*)_tmp2F2)->f4;
_LL253: s=({struct Cyc_PP_Doc*_tmp342[11];_tmp342[10]=Cyc_Absynpp_rb();_tmp342[9]=
Cyc_PP_line_doc();_tmp342[8]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp347[2];_tmp347[
1]=Cyc_Absynpp_stmt2doc(_tmp303);_tmp347[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp347,sizeof(struct Cyc_PP_Doc*),2));}));_tmp342[7]=Cyc_Absynpp_lb();
_tmp342[6]=Cyc_PP_text(({const char*_tmp346=") ";_tag_arr(_tmp346,sizeof(char),
_get_zero_arr_size(_tmp346,3));}));_tmp342[5]=Cyc_Absynpp_exp2doc(_tmp302);
_tmp342[4]=Cyc_PP_text(({const char*_tmp345="; ";_tag_arr(_tmp345,sizeof(char),
_get_zero_arr_size(_tmp345,3));}));_tmp342[3]=Cyc_Absynpp_exp2doc(_tmp300);
_tmp342[2]=Cyc_PP_text(({const char*_tmp344="; ";_tag_arr(_tmp344,sizeof(char),
_get_zero_arr_size(_tmp344,3));}));_tmp342[1]=Cyc_Absynpp_exp2doc(_tmp2FE);
_tmp342[0]=Cyc_PP_text(({const char*_tmp343="for(";_tag_arr(_tmp343,sizeof(char),
_get_zero_arr_size(_tmp343,5));}));Cyc_PP_cat(_tag_arr(_tmp342,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL23F;_LL254: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 19)goto
_LL256;_tmp304=((struct Cyc_Absyn_ForArray_s_struct*)_tmp2F2)->f1;_tmp305=_tmp304.defns;
_tmp306=_tmp304.condition;_tmp307=_tmp306.f1;_tmp308=_tmp304.delta;_tmp309=
_tmp308.f1;_tmp30A=_tmp304.body;_LL255: s=({struct Cyc_PP_Doc*_tmp348[10];_tmp348[
9]=Cyc_Absynpp_rb();_tmp348[8]=Cyc_PP_line_doc();_tmp348[7]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp34D[2];_tmp34D[1]=Cyc_Absynpp_stmt2doc(_tmp30A);_tmp34D[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp34D,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp348[6]=Cyc_Absynpp_lb();_tmp348[5]=Cyc_PP_text(({const char*_tmp34C=") ";
_tag_arr(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,3));}));_tmp348[4]=Cyc_Absynpp_exp2doc(
_tmp309);_tmp348[3]=Cyc_PP_text(({const char*_tmp34B="; ";_tag_arr(_tmp34B,
sizeof(char),_get_zero_arr_size(_tmp34B,3));}));_tmp348[2]=Cyc_Absynpp_exp2doc(
_tmp307);_tmp348[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,({
const char*_tmp34A=" ";_tag_arr(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,2));}),
_tmp305);_tmp348[0]=Cyc_PP_text(({const char*_tmp349="forarray(";_tag_arr(_tmp349,
sizeof(char),_get_zero_arr_size(_tmp349,10));}));Cyc_PP_cat(_tag_arr(_tmp348,
sizeof(struct Cyc_PP_Doc*),10));});goto _LL23F;_LL256: if(_tmp2F2 <= (void*)1?1:*((
int*)_tmp2F2)!= 9)goto _LL258;_tmp30B=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F2)->f1;
_tmp30C=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F2)->f2;_LL257: s=({struct Cyc_PP_Doc*
_tmp34E[8];_tmp34E[7]=Cyc_Absynpp_rb();_tmp34E[6]=Cyc_PP_line_doc();_tmp34E[5]=
Cyc_Absynpp_switchclauses2doc(_tmp30C);_tmp34E[4]=Cyc_PP_line_doc();_tmp34E[3]=
Cyc_Absynpp_lb();_tmp34E[2]=Cyc_PP_text(({const char*_tmp350=") ";_tag_arr(
_tmp350,sizeof(char),_get_zero_arr_size(_tmp350,3));}));_tmp34E[1]=Cyc_Absynpp_exp2doc(
_tmp30B);_tmp34E[0]=Cyc_PP_text(({const char*_tmp34F="switch (";_tag_arr(_tmp34F,
sizeof(char),_get_zero_arr_size(_tmp34F,9));}));Cyc_PP_cat(_tag_arr(_tmp34E,
sizeof(struct Cyc_PP_Doc*),8));});goto _LL23F;_LL258: if(_tmp2F2 <= (void*)1?1:*((
int*)_tmp2F2)!= 10)goto _LL25A;_tmp30D=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp2F2)->f1;_tmp30E=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp2F2)->f2;_LL259: s=({
struct Cyc_PP_Doc*_tmp351[8];_tmp351[7]=Cyc_Absynpp_rb();_tmp351[6]=Cyc_PP_line_doc();
_tmp351[5]=Cyc_Absynpp_switchCclauses2doc(_tmp30E);_tmp351[4]=Cyc_PP_line_doc();
_tmp351[3]=Cyc_Absynpp_lb();_tmp351[2]=Cyc_PP_text(({const char*_tmp353=") ";
_tag_arr(_tmp353,sizeof(char),_get_zero_arr_size(_tmp353,3));}));_tmp351[1]=Cyc_Absynpp_exp2doc(
_tmp30D);_tmp351[0]=Cyc_PP_text(({const char*_tmp352="switch \"C\" (";_tag_arr(
_tmp352,sizeof(char),_get_zero_arr_size(_tmp352,13));}));Cyc_PP_cat(_tag_arr(
_tmp351,sizeof(struct Cyc_PP_Doc*),8));});goto _LL23F;_LL25A: if(_tmp2F2 <= (void*)1?
1:*((int*)_tmp2F2)!= 11)goto _LL25C;_tmp30F=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F2)->f1;if(_tmp30F != 0)goto _LL25C;_LL25B: s=Cyc_PP_text(({const char*_tmp354="fallthru;";
_tag_arr(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,10));}));goto _LL23F;
_LL25C: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 11)goto _LL25E;_tmp310=((struct
Cyc_Absyn_Fallthru_s_struct*)_tmp2F2)->f1;_LL25D: s=({struct Cyc_PP_Doc*_tmp355[3];
_tmp355[2]=Cyc_PP_text(({const char*_tmp357=");";_tag_arr(_tmp357,sizeof(char),
_get_zero_arr_size(_tmp357,3));}));_tmp355[1]=Cyc_Absynpp_exps2doc_prec(20,
_tmp310);_tmp355[0]=Cyc_PP_text(({const char*_tmp356="fallthru(";_tag_arr(_tmp356,
sizeof(char),_get_zero_arr_size(_tmp356,10));}));Cyc_PP_cat(_tag_arr(_tmp355,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL23F;_LL25E: if(_tmp2F2 <= (void*)1?1:*((
int*)_tmp2F2)!= 12)goto _LL260;_tmp311=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F2)->f1;
_tmp312=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F2)->f2;_LL25F: s=({struct Cyc_PP_Doc*
_tmp358[3];_tmp358[2]=Cyc_Absynpp_stmt2doc(_tmp312);_tmp358[1]=Cyc_PP_line_doc();
_tmp358[0]=Cyc_Absynpp_decl2doc(_tmp311);Cyc_PP_cat(_tag_arr(_tmp358,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL23F;_LL260: if(_tmp2F2 <= (void*)1?1:*((int*)
_tmp2F2)!= 13)goto _LL262;_tmp313=((struct Cyc_Absyn_Cut_s_struct*)_tmp2F2)->f1;
_LL261: s=({struct Cyc_PP_Doc*_tmp359[2];_tmp359[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp313));_tmp359[0]=Cyc_PP_text(({const char*_tmp35A="cut ";_tag_arr(_tmp35A,
sizeof(char),_get_zero_arr_size(_tmp35A,5));}));Cyc_PP_cat(_tag_arr(_tmp359,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL23F;_LL262: if(_tmp2F2 <= (void*)1?1:*((
int*)_tmp2F2)!= 14)goto _LL264;_tmp314=((struct Cyc_Absyn_Splice_s_struct*)_tmp2F2)->f1;
_LL263: s=({struct Cyc_PP_Doc*_tmp35B[2];_tmp35B[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp314));_tmp35B[0]=Cyc_PP_text(({const char*_tmp35C="splice ";_tag_arr(_tmp35C,
sizeof(char),_get_zero_arr_size(_tmp35C,8));}));Cyc_PP_cat(_tag_arr(_tmp35B,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL23F;_LL264: if(_tmp2F2 <= (void*)1?1:*((
int*)_tmp2F2)!= 15)goto _LL266;_tmp315=((struct Cyc_Absyn_Label_s_struct*)_tmp2F2)->f1;
_tmp316=((struct Cyc_Absyn_Label_s_struct*)_tmp2F2)->f2;_LL265: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp316): 0)s=({struct Cyc_PP_Doc*_tmp35D[7];_tmp35D[6]=
Cyc_Absynpp_rb();_tmp35D[5]=Cyc_PP_line_doc();_tmp35D[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp316));_tmp35D[3]=Cyc_PP_line_doc();_tmp35D[2]=Cyc_Absynpp_lb();_tmp35D[1]=
Cyc_PP_text(({const char*_tmp35E=": ";_tag_arr(_tmp35E,sizeof(char),
_get_zero_arr_size(_tmp35E,3));}));_tmp35D[0]=Cyc_PP_textptr(_tmp315);Cyc_PP_cat(
_tag_arr(_tmp35D,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*
_tmp35F[3];_tmp35F[2]=Cyc_Absynpp_stmt2doc(_tmp316);_tmp35F[1]=Cyc_PP_text(({
const char*_tmp360=": ";_tag_arr(_tmp360,sizeof(char),_get_zero_arr_size(_tmp360,
3));}));_tmp35F[0]=Cyc_PP_textptr(_tmp315);Cyc_PP_cat(_tag_arr(_tmp35F,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL23F;_LL266: if(_tmp2F2 <= (void*)1?1:*((int*)
_tmp2F2)!= 16)goto _LL268;_tmp317=((struct Cyc_Absyn_Do_s_struct*)_tmp2F2)->f1;
_tmp318=((struct Cyc_Absyn_Do_s_struct*)_tmp2F2)->f2;_tmp319=_tmp318.f1;_LL267: s=({
struct Cyc_PP_Doc*_tmp361[9];_tmp361[8]=Cyc_PP_text(({const char*_tmp364=");";
_tag_arr(_tmp364,sizeof(char),_get_zero_arr_size(_tmp364,3));}));_tmp361[7]=Cyc_Absynpp_exp2doc(
_tmp319);_tmp361[6]=Cyc_PP_text(({const char*_tmp363=" while (";_tag_arr(_tmp363,
sizeof(char),_get_zero_arr_size(_tmp363,9));}));_tmp361[5]=Cyc_Absynpp_rb();
_tmp361[4]=Cyc_PP_line_doc();_tmp361[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp317));_tmp361[2]=Cyc_PP_line_doc();_tmp361[1]=Cyc_Absynpp_lb();_tmp361[0]=
Cyc_PP_text(({const char*_tmp362="do ";_tag_arr(_tmp362,sizeof(char),
_get_zero_arr_size(_tmp362,4));}));Cyc_PP_cat(_tag_arr(_tmp361,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL23F;_LL268: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 17)goto
_LL26A;_tmp31A=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F2)->f1;_tmp31B=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F2)->f2;_LL269: s=({struct Cyc_PP_Doc*
_tmp365[12];_tmp365[11]=Cyc_Absynpp_rb();_tmp365[10]=Cyc_PP_line_doc();_tmp365[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp31B));_tmp365[8]=Cyc_PP_line_doc();
_tmp365[7]=Cyc_Absynpp_lb();_tmp365[6]=Cyc_PP_text(({const char*_tmp367=" catch ";
_tag_arr(_tmp367,sizeof(char),_get_zero_arr_size(_tmp367,8));}));_tmp365[5]=Cyc_Absynpp_rb();
_tmp365[4]=Cyc_PP_line_doc();_tmp365[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp31A));_tmp365[2]=Cyc_PP_line_doc();_tmp365[1]=Cyc_Absynpp_lb();_tmp365[0]=
Cyc_PP_text(({const char*_tmp366="try ";_tag_arr(_tmp366,sizeof(char),
_get_zero_arr_size(_tmp366,5));}));Cyc_PP_cat(_tag_arr(_tmp365,sizeof(struct Cyc_PP_Doc*),
12));});goto _LL23F;_LL26A: if(_tmp2F2 <= (void*)1?1:*((int*)_tmp2F2)!= 18)goto
_LL26C;_tmp31C=((struct Cyc_Absyn_Region_s_struct*)_tmp2F2)->f1;_tmp31D=((struct
Cyc_Absyn_Region_s_struct*)_tmp2F2)->f2;_tmp31E=((struct Cyc_Absyn_Region_s_struct*)
_tmp2F2)->f3;_tmp31F=((struct Cyc_Absyn_Region_s_struct*)_tmp2F2)->f4;_LL26B: s=({
struct Cyc_PP_Doc*_tmp368[11];_tmp368[10]=Cyc_Absynpp_rb();_tmp368[9]=Cyc_PP_line_doc();
_tmp368[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp31F));_tmp368[7]=Cyc_PP_line_doc();
_tmp368[6]=Cyc_Absynpp_lb();_tmp368[5]=Cyc_Absynpp_qvar2doc(_tmp31D->name);
_tmp368[4]=Cyc_PP_text(({const char*_tmp36D=">";_tag_arr(_tmp36D,sizeof(char),
_get_zero_arr_size(_tmp36D,2));}));_tmp368[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp31C));_tmp368[2]=Cyc_PP_text(({const char*_tmp36C="<";_tag_arr(_tmp36C,
sizeof(char),_get_zero_arr_size(_tmp36C,2));}));_tmp368[1]=_tmp31E?Cyc_PP_text(({
const char*_tmp36A="";_tag_arr(_tmp36A,sizeof(char),_get_zero_arr_size(_tmp36A,1));})):
Cyc_PP_text(({const char*_tmp36B="[resetable]";_tag_arr(_tmp36B,sizeof(char),
_get_zero_arr_size(_tmp36B,12));}));_tmp368[0]=Cyc_PP_text(({const char*_tmp369="region";
_tag_arr(_tmp369,sizeof(char),_get_zero_arr_size(_tmp369,7));}));Cyc_PP_cat(
_tag_arr(_tmp368,sizeof(struct Cyc_PP_Doc*),11));});goto _LL23F;_LL26C: if(_tmp2F2
<= (void*)1?1:*((int*)_tmp2F2)!= 20)goto _LL23F;_tmp320=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2F2)->f1;_LL26D: s=({struct Cyc_PP_Doc*_tmp36E[3];_tmp36E[2]=Cyc_PP_text(({
const char*_tmp370=");";_tag_arr(_tmp370,sizeof(char),_get_zero_arr_size(_tmp370,
3));}));_tmp36E[1]=Cyc_Absynpp_exp2doc(_tmp320);_tmp36E[0]=Cyc_PP_text(({const
char*_tmp36F="reset_region(";_tag_arr(_tmp36F,sizeof(char),_get_zero_arr_size(
_tmp36F,14));}));Cyc_PP_cat(_tag_arr(_tmp36E,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL23F;_LL23F:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp371=(void*)p->r;void*_tmp372;int _tmp373;char
_tmp374;struct _tagged_arr _tmp375;struct Cyc_Absyn_Vardecl*_tmp376;struct Cyc_Absyn_Tvar*
_tmp377;struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Pat*
_tmp37A;struct Cyc_Absyn_Vardecl*_tmp37B;struct _tuple0*_tmp37C;struct _tuple0*
_tmp37D;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_AggrInfo _tmp37F;void*_tmp380;
struct Cyc_List_List*_tmp381;struct Cyc_List_List*_tmp382;struct Cyc_Absyn_Enumfield*
_tmp383;struct Cyc_Absyn_Enumfield*_tmp384;struct Cyc_Absyn_Tunionfield*_tmp385;
struct Cyc_List_List*_tmp386;struct Cyc_Absyn_Tunionfield*_tmp387;struct Cyc_List_List*
_tmp388;_LL274: if((int)_tmp371 != 0)goto _LL276;_LL275: s=Cyc_PP_text(({const char*
_tmp389="_";_tag_arr(_tmp389,sizeof(char),_get_zero_arr_size(_tmp389,2));}));
goto _LL273;_LL276: if((int)_tmp371 != 1)goto _LL278;_LL277: s=Cyc_PP_text(({const
char*_tmp38A="NULL";_tag_arr(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,5));}));
goto _LL273;_LL278: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 7)goto _LL27A;
_tmp372=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp371)->f1;_tmp373=((struct Cyc_Absyn_Int_p_struct*)
_tmp371)->f2;_LL279: if(_tmp372 != (void*)((void*)1))s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Int_pa_struct _tmp38D;_tmp38D.tag=1;_tmp38D.f1=(
unsigned int)_tmp373;{void*_tmp38B[1]={& _tmp38D};Cyc_aprintf(({const char*_tmp38C="%d";
_tag_arr(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,3));}),_tag_arr(_tmp38B,
sizeof(void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp390;_tmp390.tag=1;_tmp390.f1=(unsigned int)_tmp373;{void*_tmp38E[1]={&
_tmp390};Cyc_aprintf(({const char*_tmp38F="%u";_tag_arr(_tmp38F,sizeof(char),
_get_zero_arr_size(_tmp38F,3));}),_tag_arr(_tmp38E,sizeof(void*),1));}}));}goto
_LL273;_LL27A: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 8)goto _LL27C;_tmp374=((
struct Cyc_Absyn_Char_p_struct*)_tmp371)->f1;_LL27B: s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp393;_tmp393.tag=0;_tmp393.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp374));{void*_tmp391[
1]={& _tmp393};Cyc_aprintf(({const char*_tmp392="'%s'";_tag_arr(_tmp392,sizeof(
char),_get_zero_arr_size(_tmp392,5));}),_tag_arr(_tmp391,sizeof(void*),1));}}));
goto _LL273;_LL27C: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 9)goto _LL27E;
_tmp375=((struct Cyc_Absyn_Float_p_struct*)_tmp371)->f1;_LL27D: s=Cyc_PP_text(
_tmp375);goto _LL273;_LL27E: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 0)goto
_LL280;_tmp376=((struct Cyc_Absyn_Var_p_struct*)_tmp371)->f1;_LL27F: s=Cyc_Absynpp_qvar2doc(
_tmp376->name);goto _LL273;_LL280: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 2)
goto _LL282;_tmp377=((struct Cyc_Absyn_TagInt_p_struct*)_tmp371)->f1;_tmp378=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp371)->f2;_LL281: s=({struct Cyc_PP_Doc*
_tmp394[4];_tmp394[3]=Cyc_PP_text(({const char*_tmp396=">";_tag_arr(_tmp396,
sizeof(char),_get_zero_arr_size(_tmp396,2));}));_tmp394[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp377));_tmp394[1]=Cyc_PP_text(({const char*_tmp395="<";_tag_arr(_tmp395,
sizeof(char),_get_zero_arr_size(_tmp395,2));}));_tmp394[0]=Cyc_Absynpp_qvar2doc(
_tmp378->name);Cyc_PP_cat(_tag_arr(_tmp394,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL273;_LL282: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 3)goto _LL284;_tmp379=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp371)->f1;_LL283: s=({struct Cyc_PP_Doc*_tmp397[
4];_tmp397[3]=Cyc_PP_text(({const char*_tmp39A=")";_tag_arr(_tmp39A,sizeof(char),
_get_zero_arr_size(_tmp39A,2));}));_tmp397[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,({const char*_tmp399=",";_tag_arr(_tmp399,sizeof(char),
_get_zero_arr_size(_tmp399,2));}),_tmp379);_tmp397[1]=Cyc_PP_text(({const char*
_tmp398="(";_tag_arr(_tmp398,sizeof(char),_get_zero_arr_size(_tmp398,2));}));
_tmp397[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp397,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL273;_LL284: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 4)goto _LL286;
_tmp37A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp371)->f1;_LL285: s=({struct Cyc_PP_Doc*
_tmp39B[2];_tmp39B[1]=Cyc_Absynpp_pat2doc(_tmp37A);_tmp39B[0]=Cyc_PP_text(({
const char*_tmp39C="&";_tag_arr(_tmp39C,sizeof(char),_get_zero_arr_size(_tmp39C,2));}));
Cyc_PP_cat(_tag_arr(_tmp39B,sizeof(struct Cyc_PP_Doc*),2));});goto _LL273;_LL286:
if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 1)goto _LL288;_tmp37B=((struct Cyc_Absyn_Reference_p_struct*)
_tmp371)->f1;_LL287: s=({struct Cyc_PP_Doc*_tmp39D[2];_tmp39D[1]=Cyc_Absynpp_qvar2doc(
_tmp37B->name);_tmp39D[0]=Cyc_PP_text(({const char*_tmp39E="*";_tag_arr(_tmp39E,
sizeof(char),_get_zero_arr_size(_tmp39E,2));}));Cyc_PP_cat(_tag_arr(_tmp39D,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL273;_LL288: if(_tmp371 <= (void*)2?1:*((
int*)_tmp371)!= 12)goto _LL28A;_tmp37C=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp371)->f1;_LL289: s=Cyc_Absynpp_qvar2doc(_tmp37C);goto _LL273;_LL28A: if(_tmp371
<= (void*)2?1:*((int*)_tmp371)!= 13)goto _LL28C;_tmp37D=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp371)->f1;_tmp37E=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp371)->f2;_LL28B:
s=({struct Cyc_PP_Doc*_tmp39F[2];_tmp39F[1]=Cyc_PP_group(({const char*_tmp3A0="(";
_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,2));}),({const char*
_tmp3A1=")";_tag_arr(_tmp3A1,sizeof(char),_get_zero_arr_size(_tmp3A1,2));}),({
const char*_tmp3A2=",";_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp37E));_tmp39F[0]=Cyc_Absynpp_qvar2doc(
_tmp37D);Cyc_PP_cat(_tag_arr(_tmp39F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL273;
_LL28C: if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 5)goto _LL28E;_tmp37F=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp371)->f1;_tmp380=(void*)_tmp37F.aggr_info;_tmp381=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp371)->f2;_tmp382=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp371)->f3;_LL28D: {struct _tuple0*_tmp3A4;struct _tuple3 _tmp3A3=Cyc_Absyn_aggr_kinded_name(
_tmp380);_tmp3A4=_tmp3A3.f2;s=({struct Cyc_PP_Doc*_tmp3A5[4];_tmp3A5[3]=Cyc_Absynpp_group_braces(({
const char*_tmp3A9=",";_tag_arr(_tmp3A9,sizeof(char),_get_zero_arr_size(_tmp3A9,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp382));_tmp3A5[2]=Cyc_PP_egroup(({const
char*_tmp3A6="[";_tag_arr(_tmp3A6,sizeof(char),_get_zero_arr_size(_tmp3A6,2));}),({
const char*_tmp3A7="]";_tag_arr(_tmp3A7,sizeof(char),_get_zero_arr_size(_tmp3A7,2));}),({
const char*_tmp3A8=",";_tag_arr(_tmp3A8,sizeof(char),_get_zero_arr_size(_tmp3A8,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp381)));_tmp3A5[1]=Cyc_Absynpp_lb();_tmp3A5[0]=Cyc_Absynpp_qvar2doc(_tmp3A4);
Cyc_PP_cat(_tag_arr(_tmp3A5,sizeof(struct Cyc_PP_Doc*),4));});goto _LL273;}_LL28E:
if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 10)goto _LL290;_tmp383=((struct Cyc_Absyn_Enum_p_struct*)
_tmp371)->f2;_LL28F: _tmp384=_tmp383;goto _LL291;_LL290: if(_tmp371 <= (void*)2?1:*((
int*)_tmp371)!= 11)goto _LL292;_tmp384=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp371)->f2;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp384->name);goto _LL273;_LL292: if(
_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 6)goto _LL294;_tmp385=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp371)->f2;_tmp386=((struct Cyc_Absyn_Tunion_p_struct*)_tmp371)->f3;if(_tmp386
!= 0)goto _LL294;_LL293: s=Cyc_Absynpp_qvar2doc(_tmp385->name);goto _LL273;_LL294:
if(_tmp371 <= (void*)2?1:*((int*)_tmp371)!= 6)goto _LL273;_tmp387=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp371)->f2;_tmp388=((struct Cyc_Absyn_Tunion_p_struct*)_tmp371)->f3;_LL295: s=({
struct Cyc_PP_Doc*_tmp3AA[2];_tmp3AA[1]=Cyc_PP_egroup(({const char*_tmp3AB="(";
_tag_arr(_tmp3AB,sizeof(char),_get_zero_arr_size(_tmp3AB,2));}),({const char*
_tmp3AC=")";_tag_arr(_tmp3AC,sizeof(char),_get_zero_arr_size(_tmp3AC,2));}),({
const char*_tmp3AD=",";_tag_arr(_tmp3AD,sizeof(char),_get_zero_arr_size(_tmp3AD,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp388));_tmp3AA[0]=Cyc_Absynpp_qvar2doc(
_tmp387->name);Cyc_PP_cat(_tag_arr(_tmp3AA,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL273;_LL273:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){
return({struct Cyc_PP_Doc*_tmp3AE[2];_tmp3AE[1]=Cyc_Absynpp_pat2doc((*dp).f2);
_tmp3AE[0]=Cyc_PP_egroup(({const char*_tmp3AF="";_tag_arr(_tmp3AF,sizeof(char),
_get_zero_arr_size(_tmp3AF,1));}),({const char*_tmp3B0="=";_tag_arr(_tmp3B0,
sizeof(char),_get_zero_arr_size(_tmp3B0,2));}),({const char*_tmp3B1="=";_tag_arr(
_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp3AE,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0?(void*)(c->pattern)->r == (void*)((void*)0): 0)return({struct Cyc_PP_Doc*
_tmp3B2[2];_tmp3B2[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3B4[2];_tmp3B4[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3B4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3B4,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp3B2[0]=Cyc_PP_text(({const char*_tmp3B3="default: ";
_tag_arr(_tmp3B3,sizeof(char),_get_zero_arr_size(_tmp3B3,10));}));Cyc_PP_cat(
_tag_arr(_tmp3B2,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp3B5[4];_tmp3B5[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3B8[2];_tmp3B8[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3B8[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp3B8,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3B5[2]=Cyc_PP_text(({
const char*_tmp3B7=": ";_tag_arr(_tmp3B7,sizeof(char),_get_zero_arr_size(_tmp3B7,
3));}));_tmp3B5[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp3B5[0]=Cyc_PP_text(({
const char*_tmp3B6="case ";_tag_arr(_tmp3B6,sizeof(char),_get_zero_arr_size(
_tmp3B6,6));}));Cyc_PP_cat(_tag_arr(_tmp3B5,sizeof(struct Cyc_PP_Doc*),4));});
else{return({struct Cyc_PP_Doc*_tmp3B9[6];_tmp3B9[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3BD[2];_tmp3BD[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3BD[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp3BD,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3B9[4]=Cyc_PP_text(({
const char*_tmp3BC=": ";_tag_arr(_tmp3BC,sizeof(char),_get_zero_arr_size(_tmp3BC,
3));}));_tmp3B9[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3B9[2]=Cyc_PP_text(({const char*_tmp3BB=" && ";_tag_arr(_tmp3BB,sizeof(char),
_get_zero_arr_size(_tmp3BB,5));}));_tmp3B9[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3B9[0]=Cyc_PP_text(({const char*_tmp3BA="case ";_tag_arr(_tmp3BA,sizeof(char),
_get_zero_arr_size(_tmp3BA,6));}));Cyc_PP_cat(_tag_arr(_tmp3B9,sizeof(struct Cyc_PP_Doc*),
6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({
const char*_tmp3BE="";_tag_arr(_tmp3BE,sizeof(char),_get_zero_arr_size(_tmp3BE,1));}),
cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(struct Cyc_Absyn_SwitchC_clause*
c){struct Cyc_Absyn_SwitchC_clause _tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Stmt*
_tmp3C2;struct Cyc_Absyn_SwitchC_clause*_tmp3BF=c;_tmp3C0=*_tmp3BF;_tmp3C1=
_tmp3C0.cnst_exp;_tmp3C2=_tmp3C0.body;if(_tmp3C1 == 0)return({struct Cyc_PP_Doc*
_tmp3C3[2];_tmp3C3[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3C5[2];_tmp3C5[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3C5[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3C5,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp3C3[0]=Cyc_PP_text(({const char*_tmp3C4="default: ";
_tag_arr(_tmp3C4,sizeof(char),_get_zero_arr_size(_tmp3C4,10));}));Cyc_PP_cat(
_tag_arr(_tmp3C3,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp3C6[4];_tmp3C6[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3C9[2];_tmp3C9[1]=Cyc_Absynpp_stmt2doc(
_tmp3C2);_tmp3C9[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3C9,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp3C6[2]=Cyc_PP_text(({const char*_tmp3C8=": ";_tag_arr(
_tmp3C8,sizeof(char),_get_zero_arr_size(_tmp3C8,3));}));_tmp3C6[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3C1);_tmp3C6[0]=Cyc_PP_text(({const char*_tmp3C7="case ";
_tag_arr(_tmp3C7,sizeof(char),_get_zero_arr_size(_tmp3C7,6));}));Cyc_PP_cat(
_tag_arr(_tmp3C6,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchCclause2doc,({const char*_tmp3CA="";_tag_arr(_tmp3CA,sizeof(
char),_get_zero_arr_size(_tmp3CA,1));}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{return({struct Cyc_PP_Doc*_tmp3CB[3];_tmp3CB[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp3CB[1]=Cyc_PP_text(({const char*_tmp3CC=" = ";
_tag_arr(_tmp3CC,sizeof(char),_get_zero_arr_size(_tmp3CC,4));}));_tmp3CB[0]=Cyc_Absynpp_qvar2doc(
f->name);Cyc_PP_cat(_tag_arr(_tmp3CB,sizeof(struct Cyc_PP_Doc*),3));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*_tmp3CD=",";
_tag_arr(_tmp3CD,sizeof(char),_get_zero_arr_size(_tmp3CD,2));}),fs);}static
struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(
v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3CE=",";_tag_arr(_tmp3CE,sizeof(char),_get_zero_arr_size(_tmp3CE,2));}),
vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl _tmp3D0;void*_tmp3D1;struct _tuple0*_tmp3D2;struct Cyc_Absyn_Tqual
_tmp3D3;void*_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_List_List*_tmp3D6;
struct Cyc_Absyn_Vardecl*_tmp3CF=vd;_tmp3D0=*_tmp3CF;_tmp3D1=(void*)_tmp3D0.sc;
_tmp3D2=_tmp3D0.name;_tmp3D3=_tmp3D0.tq;_tmp3D4=(void*)_tmp3D0.type;_tmp3D5=
_tmp3D0.initializer;_tmp3D6=_tmp3D0.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3D2);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3D6);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp3D7=Cyc_Tcutil_compress(_tmp3D4);struct Cyc_Absyn_FnInfo
_tmp3D8;struct Cyc_List_List*_tmp3D9;_LL297: if(_tmp3D7 <= (void*)3?1:*((int*)
_tmp3D7)!= 8)goto _LL299;_tmp3D8=((struct Cyc_Absyn_FnType_struct*)_tmp3D7)->f1;
_tmp3D9=_tmp3D8.attributes;_LL298: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3D9);
goto _LL296;_LL299:;_LL29A: beforenamedoc=Cyc_PP_nil_doc();goto _LL296;_LL296:;}s=({
struct Cyc_PP_Doc*_tmp3DA[5];_tmp3DA[4]=Cyc_PP_text(({const char*_tmp3E0=";";
_tag_arr(_tmp3E0,sizeof(char),_get_zero_arr_size(_tmp3E0,2));}));_tmp3DA[3]=
_tmp3D5 == 0?Cyc_PP_text(({const char*_tmp3DD="";_tag_arr(_tmp3DD,sizeof(char),
_get_zero_arr_size(_tmp3DD,1));})):({struct Cyc_PP_Doc*_tmp3DE[2];_tmp3DE[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3D5);_tmp3DE[0]=Cyc_PP_text(({const char*_tmp3DF=" = ";
_tag_arr(_tmp3DF,sizeof(char),_get_zero_arr_size(_tmp3DF,4));}));Cyc_PP_cat(
_tag_arr(_tmp3DE,sizeof(struct Cyc_PP_Doc*),2));});_tmp3DA[2]=Cyc_Absynpp_tqtd2doc(
_tmp3D3,_tmp3D4,({struct Cyc_Core_Opt*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->v=({
struct Cyc_PP_Doc*_tmp3DC[2];_tmp3DC[1]=sn;_tmp3DC[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp3DC,sizeof(struct Cyc_PP_Doc*),2));});_tmp3DB;}));_tmp3DA[1]=Cyc_Absynpp_scope2doc(
_tmp3D1);_tmp3DA[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(
_tag_arr(_tmp3DA,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3E1=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp3E2;struct Cyc_Absyn_Aggrdecl*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E4;struct
Cyc_Absyn_Tuniondecl*_tmp3E5;struct Cyc_Absyn_Tuniondecl _tmp3E6;void*_tmp3E7;
struct _tuple0*_tmp3E8;struct Cyc_List_List*_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;int
_tmp3EB;struct Cyc_Absyn_Pat*_tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_List_List*
_tmp3EE;struct Cyc_Absyn_Enumdecl*_tmp3EF;struct Cyc_Absyn_Enumdecl _tmp3F0;void*
_tmp3F1;struct _tuple0*_tmp3F2;struct Cyc_Core_Opt*_tmp3F3;struct Cyc_Absyn_Typedefdecl*
_tmp3F4;struct _tagged_arr*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct _tuple0*
_tmp3F7;struct Cyc_List_List*_tmp3F8;struct Cyc_List_List*_tmp3F9;_LL29C: if(*((int*)
_tmp3E1)!= 1)goto _LL29E;_tmp3E2=((struct Cyc_Absyn_Fn_d_struct*)_tmp3E1)->f1;
_LL29D: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp402=_cycalloc(sizeof(*
_tmp402));_tmp402[0]=({struct Cyc_Absyn_FnType_struct _tmp403;_tmp403.tag=8;
_tmp403.f1=({struct Cyc_Absyn_FnInfo _tmp404;_tmp404.tvars=_tmp3E2->tvs;_tmp404.effect=
_tmp3E2->effect;_tmp404.ret_typ=(void*)((void*)_tmp3E2->ret_type);_tmp404.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3E2->args);_tmp404.c_varargs=_tmp3E2->c_varargs;
_tmp404.cyc_varargs=_tmp3E2->cyc_varargs;_tmp404.rgn_po=_tmp3E2->rgn_po;_tmp404.attributes=
0;_tmp404;});_tmp403;});_tmp402;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3E2->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3E2->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3FA="__inline ";_tag_arr(_tmp3FA,sizeof(
char),_get_zero_arr_size(_tmp3FA,10));}));else{inlinedoc=Cyc_PP_text(({const char*
_tmp3FB="inline ";_tag_arr(_tmp3FB,sizeof(char),_get_zero_arr_size(_tmp3FB,8));}));}}
else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3E2->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp3E2->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3E2->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),
t,({struct Cyc_Core_Opt*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->v=({struct
Cyc_PP_Doc*_tmp401[2];_tmp401[1]=namedoc;_tmp401[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp401,sizeof(struct Cyc_PP_Doc*),2));});_tmp400;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3FE[5];_tmp3FE[4]=Cyc_Absynpp_rb();_tmp3FE[3]=Cyc_PP_line_doc();
_tmp3FE[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3FF[2];_tmp3FF[1]=Cyc_Absynpp_stmt2doc(
_tmp3E2->body);_tmp3FF[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3FF,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3FE[1]=Cyc_Absynpp_lb();_tmp3FE[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_arr(_tmp3FE,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3FC[4];_tmp3FC[3]=bodydoc;_tmp3FC[2]=tqtddoc;_tmp3FC[1]=scopedoc;_tmp3FC[0]=
inlinedoc;Cyc_PP_cat(_tag_arr(_tmp3FC,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp3FD[2];_tmp3FD[1]=s;_tmp3FD[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp3FD,sizeof(struct Cyc_PP_Doc*),2));});goto _LL29B;}}_LL29E: if(*((int*)
_tmp3E1)!= 4)goto _LL2A0;_tmp3E3=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3E1)->f1;
_LL29F: if(_tmp3E3->impl == 0)s=({struct Cyc_PP_Doc*_tmp405[5];_tmp405[4]=Cyc_PP_text(({
const char*_tmp406=";";_tag_arr(_tmp406,sizeof(char),_get_zero_arr_size(_tmp406,2));}));
_tmp405[3]=Cyc_Absynpp_ktvars2doc(_tmp3E3->tvs);_tmp405[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3E3->name);_tmp405[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3E3->kind);
_tmp405[0]=Cyc_Absynpp_scope2doc((void*)_tmp3E3->sc);Cyc_PP_cat(_tag_arr(_tmp405,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp407[12];_tmp407[
11]=Cyc_PP_text(({const char*_tmp40B=";";_tag_arr(_tmp40B,sizeof(char),
_get_zero_arr_size(_tmp40B,2));}));_tmp407[10]=Cyc_Absynpp_atts2doc(_tmp3E3->attributes);
_tmp407[9]=Cyc_Absynpp_rb();_tmp407[8]=Cyc_PP_line_doc();_tmp407[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp40A[2];_tmp40A[1]=Cyc_Absynpp_aggrfields2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E3->impl))->fields);_tmp40A[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp40A,sizeof(struct Cyc_PP_Doc*),2));}));_tmp407[6]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E3->impl))->rgn_po == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp408[2];_tmp408[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3E3->impl))->rgn_po);_tmp408[0]=Cyc_PP_text(({const char*_tmp409=":";
_tag_arr(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,2));}));Cyc_PP_cat(
_tag_arr(_tmp408,sizeof(struct Cyc_PP_Doc*),2));});_tmp407[5]=Cyc_Absynpp_ktvars2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E3->impl))->exist_vars);_tmp407[4]=
Cyc_Absynpp_lb();_tmp407[3]=Cyc_PP_blank_doc();_tmp407[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3E3->name);_tmp407[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3E3->kind);
_tmp407[0]=Cyc_Absynpp_scope2doc((void*)_tmp3E3->sc);Cyc_PP_cat(_tag_arr(_tmp407,
sizeof(struct Cyc_PP_Doc*),12));});}goto _LL29B;_LL2A0: if(*((int*)_tmp3E1)!= 0)
goto _LL2A2;_tmp3E4=((struct Cyc_Absyn_Var_d_struct*)_tmp3E1)->f1;_LL2A1: s=Cyc_Absynpp_vardecl2doc(
_tmp3E4);goto _LL29B;_LL2A2: if(*((int*)_tmp3E1)!= 5)goto _LL2A4;_tmp3E5=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp3E1)->f1;_tmp3E6=*_tmp3E5;_tmp3E7=(void*)_tmp3E6.sc;
_tmp3E8=_tmp3E6.name;_tmp3E9=_tmp3E6.tvs;_tmp3EA=_tmp3E6.fields;_tmp3EB=_tmp3E6.is_xtunion;
_LL2A3: if(_tmp3EA == 0)s=({struct Cyc_PP_Doc*_tmp40C[5];_tmp40C[4]=Cyc_PP_text(({
const char*_tmp40F=";";_tag_arr(_tmp40F,sizeof(char),_get_zero_arr_size(_tmp40F,2));}));
_tmp40C[3]=Cyc_Absynpp_ktvars2doc(_tmp3E9);_tmp40C[2]=_tmp3EB?Cyc_Absynpp_qvar2bolddoc(
_tmp3E8): Cyc_Absynpp_typedef_name2bolddoc(_tmp3E8);_tmp40C[1]=_tmp3EB?Cyc_PP_text(({
const char*_tmp40D="xtunion ";_tag_arr(_tmp40D,sizeof(char),_get_zero_arr_size(
_tmp40D,9));})): Cyc_PP_text(({const char*_tmp40E="tunion ";_tag_arr(_tmp40E,
sizeof(char),_get_zero_arr_size(_tmp40E,8));}));_tmp40C[0]=Cyc_Absynpp_scope2doc(
_tmp3E7);Cyc_PP_cat(_tag_arr(_tmp40C,sizeof(struct Cyc_PP_Doc*),5));});else{s=({
struct Cyc_PP_Doc*_tmp410[10];_tmp410[9]=Cyc_PP_text(({const char*_tmp414=";";
_tag_arr(_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,2));}));_tmp410[8]=Cyc_Absynpp_rb();
_tmp410[7]=Cyc_PP_line_doc();_tmp410[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp413[
2];_tmp413[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)_tmp3EA->v);
_tmp413[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp413,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp410[5]=Cyc_Absynpp_lb();_tmp410[4]=Cyc_PP_blank_doc();_tmp410[3]=Cyc_Absynpp_ktvars2doc(
_tmp3E9);_tmp410[2]=_tmp3EB?Cyc_Absynpp_qvar2bolddoc(_tmp3E8): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E8);_tmp410[1]=_tmp3EB?Cyc_PP_text(({const char*_tmp411="xtunion ";_tag_arr(
_tmp411,sizeof(char),_get_zero_arr_size(_tmp411,9));})): Cyc_PP_text(({const char*
_tmp412="tunion ";_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,8));}));
_tmp410[0]=Cyc_Absynpp_scope2doc(_tmp3E7);Cyc_PP_cat(_tag_arr(_tmp410,sizeof(
struct Cyc_PP_Doc*),10));});}goto _LL29B;_LL2A4: if(*((int*)_tmp3E1)!= 2)goto _LL2A6;
_tmp3EC=((struct Cyc_Absyn_Let_d_struct*)_tmp3E1)->f1;_tmp3ED=((struct Cyc_Absyn_Let_d_struct*)
_tmp3E1)->f3;_LL2A5: s=({struct Cyc_PP_Doc*_tmp415[5];_tmp415[4]=Cyc_PP_text(({
const char*_tmp418=";";_tag_arr(_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,2));}));
_tmp415[3]=Cyc_Absynpp_exp2doc(_tmp3ED);_tmp415[2]=Cyc_PP_text(({const char*
_tmp417=" = ";_tag_arr(_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,4));}));
_tmp415[1]=Cyc_Absynpp_pat2doc(_tmp3EC);_tmp415[0]=Cyc_PP_text(({const char*
_tmp416="let ";_tag_arr(_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,5));}));
Cyc_PP_cat(_tag_arr(_tmp415,sizeof(struct Cyc_PP_Doc*),5));});goto _LL29B;_LL2A6:
if(*((int*)_tmp3E1)!= 3)goto _LL2A8;_tmp3EE=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3E1)->f1;_LL2A7: s=({struct Cyc_PP_Doc*_tmp419[3];_tmp419[2]=Cyc_PP_text(({
const char*_tmp41B=";";_tag_arr(_tmp41B,sizeof(char),_get_zero_arr_size(_tmp41B,2));}));
_tmp419[1]=Cyc_Absynpp_ids2doc(_tmp3EE);_tmp419[0]=Cyc_PP_text(({const char*
_tmp41A="let ";_tag_arr(_tmp41A,sizeof(char),_get_zero_arr_size(_tmp41A,5));}));
Cyc_PP_cat(_tag_arr(_tmp419,sizeof(struct Cyc_PP_Doc*),3));});goto _LL29B;_LL2A8:
if(*((int*)_tmp3E1)!= 6)goto _LL2AA;_tmp3EF=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3E1)->f1;_tmp3F0=*_tmp3EF;_tmp3F1=(void*)_tmp3F0.sc;_tmp3F2=_tmp3F0.name;
_tmp3F3=_tmp3F0.fields;_LL2A9: if(_tmp3F3 == 0)s=({struct Cyc_PP_Doc*_tmp41C[4];
_tmp41C[3]=Cyc_PP_text(({const char*_tmp41E=";";_tag_arr(_tmp41E,sizeof(char),
_get_zero_arr_size(_tmp41E,2));}));_tmp41C[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F2);_tmp41C[1]=Cyc_PP_text(({const char*_tmp41D="enum ";_tag_arr(_tmp41D,
sizeof(char),_get_zero_arr_size(_tmp41D,6));}));_tmp41C[0]=Cyc_Absynpp_scope2doc(
_tmp3F1);Cyc_PP_cat(_tag_arr(_tmp41C,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp41F[9];_tmp41F[8]=Cyc_PP_text(({const char*_tmp422=";";
_tag_arr(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,2));}));_tmp41F[7]=Cyc_Absynpp_rb();
_tmp41F[6]=Cyc_PP_line_doc();_tmp41F[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp421[
2];_tmp421[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3F3->v);
_tmp421[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp421,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp41F[4]=Cyc_Absynpp_lb();_tmp41F[3]=Cyc_PP_blank_doc();_tmp41F[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3F2);_tmp41F[1]=Cyc_PP_text(({const char*_tmp420="enum ";_tag_arr(_tmp420,
sizeof(char),_get_zero_arr_size(_tmp420,6));}));_tmp41F[0]=Cyc_Absynpp_scope2doc(
_tmp3F1);Cyc_PP_cat(_tag_arr(_tmp41F,sizeof(struct Cyc_PP_Doc*),9));});}goto
_LL29B;_LL2AA: if(*((int*)_tmp3E1)!= 7)goto _LL2AC;_tmp3F4=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3E1)->f1;_LL2AB: {void*t;if(_tmp3F4->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F4->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3F4->kind,0);}s=({
struct Cyc_PP_Doc*_tmp423[4];_tmp423[3]=Cyc_PP_text(({const char*_tmp427=";";
_tag_arr(_tmp427,sizeof(char),_get_zero_arr_size(_tmp427,2));}));_tmp423[2]=Cyc_Absynpp_atts2doc(
_tmp3F4->atts);_tmp423[1]=Cyc_Absynpp_tqtd2doc(_tmp3F4->tq,t,({struct Cyc_Core_Opt*
_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->v=({struct Cyc_PP_Doc*_tmp426[2];
_tmp426[1]=Cyc_Absynpp_tvars2doc(_tmp3F4->tvs);_tmp426[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3F4->name);Cyc_PP_cat(_tag_arr(_tmp426,sizeof(struct Cyc_PP_Doc*),2));});
_tmp425;}));_tmp423[0]=Cyc_PP_text(({const char*_tmp424="typedef ";_tag_arr(
_tmp424,sizeof(char),_get_zero_arr_size(_tmp424,9));}));Cyc_PP_cat(_tag_arr(
_tmp423,sizeof(struct Cyc_PP_Doc*),4));});goto _LL29B;}_LL2AC: if(*((int*)_tmp3E1)
!= 8)goto _LL2AE;_tmp3F5=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3E1)->f1;
_tmp3F6=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3E1)->f2;_LL2AD: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp3F5);s=({struct Cyc_PP_Doc*_tmp428[8];_tmp428[
7]=Cyc_Absynpp_rb();_tmp428[6]=Cyc_PP_line_doc();_tmp428[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*_tmp42A="";_tag_arr(_tmp42A,
sizeof(char),_get_zero_arr_size(_tmp42A,1));}),_tmp3F6);_tmp428[4]=Cyc_PP_line_doc();
_tmp428[3]=Cyc_Absynpp_lb();_tmp428[2]=Cyc_PP_blank_doc();_tmp428[1]=Cyc_PP_textptr(
_tmp3F5);_tmp428[0]=Cyc_PP_text(({const char*_tmp429="namespace ";_tag_arr(
_tmp429,sizeof(char),_get_zero_arr_size(_tmp429,11));}));Cyc_PP_cat(_tag_arr(
_tmp428,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL29B;_LL2AE: if(*((int*)_tmp3E1)!= 9)goto _LL2B0;_tmp3F7=((struct Cyc_Absyn_Using_d_struct*)
_tmp3E1)->f1;_tmp3F8=((struct Cyc_Absyn_Using_d_struct*)_tmp3E1)->f2;_LL2AF: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp42B[8];_tmp42B[7]=Cyc_Absynpp_rb();
_tmp42B[6]=Cyc_PP_line_doc();_tmp42B[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp42D="";_tag_arr(_tmp42D,sizeof(char),
_get_zero_arr_size(_tmp42D,1));}),_tmp3F8);_tmp42B[4]=Cyc_PP_line_doc();_tmp42B[
3]=Cyc_Absynpp_lb();_tmp42B[2]=Cyc_PP_blank_doc();_tmp42B[1]=Cyc_Absynpp_qvar2doc(
_tmp3F7);_tmp42B[0]=Cyc_PP_text(({const char*_tmp42C="using ";_tag_arr(_tmp42C,
sizeof(char),_get_zero_arr_size(_tmp42C,7));}));Cyc_PP_cat(_tag_arr(_tmp42B,
sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*_tmp42E[11];_tmp42E[
10]=Cyc_PP_text(({const char*_tmp433=" */";_tag_arr(_tmp433,sizeof(char),
_get_zero_arr_size(_tmp433,4));}));_tmp42E[9]=Cyc_Absynpp_rb();_tmp42E[8]=Cyc_PP_text(({
const char*_tmp432="/* ";_tag_arr(_tmp432,sizeof(char),_get_zero_arr_size(_tmp432,
4));}));_tmp42E[7]=Cyc_PP_line_doc();_tmp42E[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp431="";_tag_arr(_tmp431,sizeof(char),
_get_zero_arr_size(_tmp431,1));}),_tmp3F8);_tmp42E[5]=Cyc_PP_line_doc();_tmp42E[
4]=Cyc_PP_text(({const char*_tmp430=" */";_tag_arr(_tmp430,sizeof(char),
_get_zero_arr_size(_tmp430,4));}));_tmp42E[3]=Cyc_Absynpp_lb();_tmp42E[2]=Cyc_PP_blank_doc();
_tmp42E[1]=Cyc_Absynpp_qvar2doc(_tmp3F7);_tmp42E[0]=Cyc_PP_text(({const char*
_tmp42F="/* using ";_tag_arr(_tmp42F,sizeof(char),_get_zero_arr_size(_tmp42F,10));}));
Cyc_PP_cat(_tag_arr(_tmp42E,sizeof(struct Cyc_PP_Doc*),11));});}goto _LL29B;_LL2B0:
if(*((int*)_tmp3E1)!= 10)goto _LL29B;_tmp3F9=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3E1)->f1;_LL2B1: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*
_tmp434[6];_tmp434[5]=Cyc_Absynpp_rb();_tmp434[4]=Cyc_PP_line_doc();_tmp434[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp436="";_tag_arr(_tmp436,sizeof(char),_get_zero_arr_size(_tmp436,1));}),
_tmp3F9);_tmp434[2]=Cyc_PP_line_doc();_tmp434[1]=Cyc_Absynpp_lb();_tmp434[0]=Cyc_PP_text(({
const char*_tmp435="extern \"C\" ";_tag_arr(_tmp435,sizeof(char),
_get_zero_arr_size(_tmp435,12));}));Cyc_PP_cat(_tag_arr(_tmp434,sizeof(struct Cyc_PP_Doc*),
6));});else{s=({struct Cyc_PP_Doc*_tmp437[9];_tmp437[8]=Cyc_PP_text(({const char*
_tmp43C=" */";_tag_arr(_tmp43C,sizeof(char),_get_zero_arr_size(_tmp43C,4));}));
_tmp437[7]=Cyc_Absynpp_rb();_tmp437[6]=Cyc_PP_text(({const char*_tmp43B="/* ";
_tag_arr(_tmp43B,sizeof(char),_get_zero_arr_size(_tmp43B,4));}));_tmp437[5]=Cyc_PP_line_doc();
_tmp437[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp43A="";_tag_arr(_tmp43A,sizeof(char),_get_zero_arr_size(_tmp43A,1));}),
_tmp3F9);_tmp437[3]=Cyc_PP_line_doc();_tmp437[2]=Cyc_PP_text(({const char*_tmp439=" */";
_tag_arr(_tmp439,sizeof(char),_get_zero_arr_size(_tmp439,4));}));_tmp437[1]=Cyc_Absynpp_lb();
_tmp437[0]=Cyc_PP_text(({const char*_tmp438="/* extern \"C\" ";_tag_arr(_tmp438,
sizeof(char),_get_zero_arr_size(_tmp438,15));}));Cyc_PP_cat(_tag_arr(_tmp437,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL29B;_LL29B:;}return s;}int Cyc_Absynpp_print_scopes=
1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(!Cyc_Absynpp_print_scopes)
return Cyc_PP_nil_doc();{void*_tmp43D=sc;_LL2B3: if((int)_tmp43D != 0)goto _LL2B5;
_LL2B4: return Cyc_PP_text(({const char*_tmp43E="static ";_tag_arr(_tmp43E,sizeof(
char),_get_zero_arr_size(_tmp43E,8));}));_LL2B5: if((int)_tmp43D != 2)goto _LL2B7;
_LL2B6: return Cyc_PP_nil_doc();_LL2B7: if((int)_tmp43D != 3)goto _LL2B9;_LL2B8:
return Cyc_PP_text(({const char*_tmp43F="extern ";_tag_arr(_tmp43F,sizeof(char),
_get_zero_arr_size(_tmp43F,8));}));_LL2B9: if((int)_tmp43D != 4)goto _LL2BB;_LL2BA:
return Cyc_PP_text(({const char*_tmp440="extern \"C\" ";_tag_arr(_tmp440,sizeof(
char),_get_zero_arr_size(_tmp440,12));}));_LL2BB: if((int)_tmp43D != 1)goto _LL2BD;
_LL2BC: return Cyc_PP_text(({const char*_tmp441="abstract ";_tag_arr(_tmp441,
sizeof(char),_get_zero_arr_size(_tmp441,10));}));_LL2BD: if((int)_tmp43D != 5)goto
_LL2B2;_LL2BE: return Cyc_PP_text(({const char*_tmp442="register ";_tag_arr(_tmp442,
sizeof(char),_get_zero_arr_size(_tmp442,10));}));_LL2B2:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp443=t;struct Cyc_Absyn_Tvar*_tmp444;struct Cyc_List_List*_tmp445;
_LL2C0: if(_tmp443 <= (void*)3?1:*((int*)_tmp443)!= 1)goto _LL2C2;_tmp444=((struct
Cyc_Absyn_VarType_struct*)_tmp443)->f1;_LL2C1: return Cyc_Tcutil_is_temp_tvar(
_tmp444);_LL2C2: if(_tmp443 <= (void*)3?1:*((int*)_tmp443)!= 20)goto _LL2C4;_tmp445=((
struct Cyc_Absyn_JoinEff_struct*)_tmp443)->f1;_LL2C3: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp445);_LL2C4:;_LL2C5: return 0;_LL2BF:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp446=t;void**_tmp447;void*_tmp448;_LL2C7: if(_tmp446 <= (void*)3?1:*((int*)
_tmp446)!= 11)goto _LL2C9;_LL2C8: return 1;_LL2C9: if(_tmp446 <= (void*)3?1:*((int*)
_tmp446)!= 13)goto _LL2CB;_LL2CA: return 1;_LL2CB: if(_tmp446 <= (void*)3?1:*((int*)
_tmp446)!= 16)goto _LL2CD;_tmp447=((struct Cyc_Absyn_TypedefType_struct*)_tmp446)->f4;
if(_tmp447 == 0)goto _LL2CD;_tmp448=*_tmp447;_LL2CC: return Cyc_Absynpp_is_anon_aggrtype(
_tmp448);_LL2CD:;_LL2CE: return 0;_LL2C6:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
void*atts,struct Cyc_List_List*tms){if(tms != 0?tms->tl != 0: 0){struct _tuple5
_tmp44A=({struct _tuple5 _tmp449;_tmp449.f1=(void*)tms->hd;_tmp449.f2=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp449;});void*_tmp44B;void*
_tmp44C;_LL2D0: _tmp44B=_tmp44A.f1;if(*((int*)_tmp44B)!= 2)goto _LL2D2;_tmp44C=
_tmp44A.f2;if(*((int*)_tmp44C)!= 3)goto _LL2D2;_LL2D1: return({struct Cyc_List_List*
_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->hd=(void*)((void*)tms->hd);_tmp44D->tl=({
struct Cyc_List_List*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);_tmp44E->tl=Cyc_Absynpp_bubble_attributes(
atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp44E;});_tmp44D;});
_LL2D2:;_LL2D3: return({struct Cyc_List_List*_tmp44F=_cycalloc(sizeof(*_tmp44F));
_tmp44F->hd=(void*)atts;_tmp44F->tl=tms;_tmp44F;});_LL2CF:;}else{return({struct
Cyc_List_List*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->hd=(void*)atts;
_tmp450->tl=tms;_tmp450;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp451=t;struct Cyc_Absyn_ArrayInfo _tmp452;void*_tmp453;struct
Cyc_Absyn_Tqual _tmp454;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Conref*
_tmp456;struct Cyc_Absyn_PtrInfo _tmp457;void*_tmp458;struct Cyc_Absyn_Tqual _tmp459;
struct Cyc_Absyn_PtrAtts _tmp45A;struct Cyc_Absyn_FnInfo _tmp45B;struct Cyc_List_List*
_tmp45C;struct Cyc_Core_Opt*_tmp45D;void*_tmp45E;struct Cyc_List_List*_tmp45F;int
_tmp460;struct Cyc_Absyn_VarargInfo*_tmp461;struct Cyc_List_List*_tmp462;struct Cyc_List_List*
_tmp463;struct Cyc_Core_Opt*_tmp464;struct Cyc_Core_Opt*_tmp465;int _tmp466;struct
_tuple0*_tmp467;struct Cyc_List_List*_tmp468;void**_tmp469;_LL2D5: if(_tmp451 <= (
void*)3?1:*((int*)_tmp451)!= 7)goto _LL2D7;_tmp452=((struct Cyc_Absyn_ArrayType_struct*)
_tmp451)->f1;_tmp453=(void*)_tmp452.elt_type;_tmp454=_tmp452.tq;_tmp455=_tmp452.num_elts;
_tmp456=_tmp452.zero_term;_LL2D6: {struct Cyc_Absyn_Tqual _tmp46B;void*_tmp46C;
struct Cyc_List_List*_tmp46D;struct _tuple7 _tmp46A=Cyc_Absynpp_to_tms(_tmp454,
_tmp453);_tmp46B=_tmp46A.f1;_tmp46C=_tmp46A.f2;_tmp46D=_tmp46A.f3;{void*tm;if(
_tmp455 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp46E=_cycalloc(
sizeof(*_tmp46E));_tmp46E[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp46F;_tmp46F.tag=
0;_tmp46F.f1=_tmp456;_tmp46F;});_tmp46E;});else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp471;_tmp471.tag=1;_tmp471.f1=(struct Cyc_Absyn_Exp*)_tmp455;_tmp471.f2=
_tmp456;_tmp471;});_tmp470;});}return({struct _tuple7 _tmp472;_tmp472.f1=_tmp46B;
_tmp472.f2=_tmp46C;_tmp472.f3=({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*
_tmp473));_tmp473->hd=(void*)tm;_tmp473->tl=_tmp46D;_tmp473;});_tmp472;});}}
_LL2D7: if(_tmp451 <= (void*)3?1:*((int*)_tmp451)!= 4)goto _LL2D9;_tmp457=((struct
Cyc_Absyn_PointerType_struct*)_tmp451)->f1;_tmp458=(void*)_tmp457.elt_typ;
_tmp459=_tmp457.elt_tq;_tmp45A=_tmp457.ptr_atts;_LL2D8: {struct Cyc_Absyn_Tqual
_tmp475;void*_tmp476;struct Cyc_List_List*_tmp477;struct _tuple7 _tmp474=Cyc_Absynpp_to_tms(
_tmp459,_tmp458);_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_tmp477=_tmp474.f3;
_tmp477=({struct Cyc_List_List*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp479=_cycalloc(sizeof(*
_tmp479));_tmp479[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp47A;_tmp47A.tag=2;
_tmp47A.f1=_tmp45A;_tmp47A.f2=tq;_tmp47A;});_tmp479;}));_tmp478->tl=_tmp477;
_tmp478;});return({struct _tuple7 _tmp47B;_tmp47B.f1=_tmp475;_tmp47B.f2=_tmp476;
_tmp47B.f3=_tmp477;_tmp47B;});}_LL2D9: if(_tmp451 <= (void*)3?1:*((int*)_tmp451)!= 
8)goto _LL2DB;_tmp45B=((struct Cyc_Absyn_FnType_struct*)_tmp451)->f1;_tmp45C=
_tmp45B.tvars;_tmp45D=_tmp45B.effect;_tmp45E=(void*)_tmp45B.ret_typ;_tmp45F=
_tmp45B.args;_tmp460=_tmp45B.c_varargs;_tmp461=_tmp45B.cyc_varargs;_tmp462=
_tmp45B.rgn_po;_tmp463=_tmp45B.attributes;_LL2DA: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp45D == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp45D->v)){
_tmp45D=0;_tmp45C=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp45C);}{struct Cyc_Absyn_Tqual _tmp47D;void*_tmp47E;struct Cyc_List_List*_tmp47F;
struct _tuple7 _tmp47C=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp45E);_tmp47D=
_tmp47C.f1;_tmp47E=_tmp47C.f2;_tmp47F=_tmp47C.f3;{struct Cyc_List_List*tms=
_tmp47F;if(_tmp463 != 0?!Cyc_Absynpp_to_VC: 0)tms=Cyc_Absynpp_bubble_attributes((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp480=_cycalloc(sizeof(*_tmp480));
_tmp480[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp481;_tmp481.tag=5;_tmp481.f1=
0;_tmp481.f2=_tmp463;_tmp481;});_tmp480;}),tms);tms=({struct Cyc_List_List*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp484;_tmp484.tag=3;_tmp484.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp486;_tmp486.tag=1;_tmp486.f1=_tmp45F;_tmp486.f2=_tmp460;_tmp486.f3=_tmp461;
_tmp486.f4=_tmp45D;_tmp486.f5=_tmp462;_tmp486;});_tmp485;}));_tmp484;});_tmp483;}));
_tmp482->tl=tms;_tmp482;});if(Cyc_Absynpp_to_VC)for(0;_tmp463 != 0;_tmp463=
_tmp463->tl){void*_tmp487=(void*)_tmp463->hd;_LL2E2: if((int)_tmp487 != 0)goto
_LL2E4;_LL2E3: goto _LL2E5;_LL2E4: if((int)_tmp487 != 1)goto _LL2E6;_LL2E5: goto _LL2E7;
_LL2E6: if((int)_tmp487 != 2)goto _LL2E8;_LL2E7: tms=({struct Cyc_List_List*_tmp488=
_cycalloc(sizeof(*_tmp488));_tmp488->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp48A;_tmp48A.tag=5;_tmp48A.f1=0;_tmp48A.f2=({struct Cyc_List_List*_tmp48B=
_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=(void*)((void*)_tmp463->hd);_tmp48B->tl=
0;_tmp48B;});_tmp48A;});_tmp489;}));_tmp488->tl=tms;_tmp488;});goto AfterAtts;
_LL2E8:;_LL2E9: goto _LL2E1;_LL2E1:;}AfterAtts: if(_tmp45C != 0)tms=({struct Cyc_List_List*
_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp48E;_tmp48E.tag=4;_tmp48E.f1=_tmp45C;_tmp48E.f2=0;_tmp48E.f3=1;_tmp48E;});
_tmp48D;}));_tmp48C->tl=tms;_tmp48C;});return({struct _tuple7 _tmp48F;_tmp48F.f1=
_tmp47D;_tmp48F.f2=_tmp47E;_tmp48F.f3=tms;_tmp48F;});}}_LL2DB: if(_tmp451 <= (void*)
3?1:*((int*)_tmp451)!= 0)goto _LL2DD;_tmp464=((struct Cyc_Absyn_Evar_struct*)
_tmp451)->f1;_tmp465=((struct Cyc_Absyn_Evar_struct*)_tmp451)->f2;_tmp466=((
struct Cyc_Absyn_Evar_struct*)_tmp451)->f3;_LL2DC: if(_tmp465 == 0)return({struct
_tuple7 _tmp490;_tmp490.f1=tq;_tmp490.f2=t;_tmp490.f3=0;_tmp490;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp465->v);}_LL2DD: if(_tmp451 <= (void*)3?1:*((int*)_tmp451)!= 16)goto
_LL2DF;_tmp467=((struct Cyc_Absyn_TypedefType_struct*)_tmp451)->f1;_tmp468=((
struct Cyc_Absyn_TypedefType_struct*)_tmp451)->f2;_tmp469=((struct Cyc_Absyn_TypedefType_struct*)
_tmp451)->f4;_LL2DE: if((_tmp469 == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype(*
_tmp469))return({struct _tuple7 _tmp491;_tmp491.f1=tq;_tmp491.f2=t;_tmp491.f3=0;
_tmp491;});else{return Cyc_Absynpp_to_tms(tq,*_tmp469);}_LL2DF:;_LL2E0: return({
struct _tuple7 _tmp492;_tmp492.f1=tq;_tmp492.f2=t;_tmp492.f3=0;_tmp492;});_LL2D4:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp493=t;struct Cyc_Core_Opt*
_tmp494;struct Cyc_Core_Opt _tmp495;void*_tmp496;struct Cyc_Absyn_PtrInfo _tmp497;
void*_tmp498;_LL2EB: if(_tmp493 <= (void*)3?1:*((int*)_tmp493)!= 0)goto _LL2ED;
_tmp494=((struct Cyc_Absyn_Evar_struct*)_tmp493)->f2;if(_tmp494 == 0)goto _LL2ED;
_tmp495=*_tmp494;_tmp496=(void*)_tmp495.v;_LL2EC: return Cyc_Absynpp_is_char_ptr(
_tmp496);_LL2ED: if(_tmp493 <= (void*)3?1:*((int*)_tmp493)!= 4)goto _LL2EF;_tmp497=((
struct Cyc_Absyn_PointerType_struct*)_tmp493)->f1;_tmp498=(void*)_tmp497.elt_typ;
_LL2EE: L: {void*_tmp499=_tmp498;struct Cyc_Core_Opt*_tmp49A;struct Cyc_Core_Opt
_tmp49B;void*_tmp49C;void**_tmp49D;void*_tmp49E;void*_tmp49F;_LL2F2: if(_tmp499 <= (
void*)3?1:*((int*)_tmp499)!= 0)goto _LL2F4;_tmp49A=((struct Cyc_Absyn_Evar_struct*)
_tmp499)->f2;if(_tmp49A == 0)goto _LL2F4;_tmp49B=*_tmp49A;_tmp49C=(void*)_tmp49B.v;
_LL2F3: _tmp498=_tmp49C;goto L;_LL2F4: if(_tmp499 <= (void*)3?1:*((int*)_tmp499)!= 
16)goto _LL2F6;_tmp49D=((struct Cyc_Absyn_TypedefType_struct*)_tmp499)->f4;if(
_tmp49D == 0)goto _LL2F6;_tmp49E=*_tmp49D;_LL2F5: _tmp498=_tmp49E;goto L;_LL2F6: if(
_tmp499 <= (void*)3?1:*((int*)_tmp499)!= 5)goto _LL2F8;_tmp49F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp499)->f2;if((int)_tmp49F != 0)goto _LL2F8;_LL2F7: return 1;_LL2F8:;_LL2F9: return
0;_LL2F1:;}_LL2EF:;_LL2F0: return 0;_LL2EA:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual
_tmp4A1;void*_tmp4A2;struct Cyc_List_List*_tmp4A3;struct _tuple7 _tmp4A0=Cyc_Absynpp_to_tms(
tq,typ);_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_tmp4A3=_tmp4A0.f3;_tmp4A3=Cyc_List_imp_rev(
_tmp4A3);if(_tmp4A3 == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp4A4[2];_tmp4A4[1]=
Cyc_Absynpp_ntyp2doc(_tmp4A2);_tmp4A4[0]=Cyc_Absynpp_tqual2doc(_tmp4A1);Cyc_PP_cat(
_tag_arr(_tmp4A4,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp4A5[4];_tmp4A5[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4A3);_tmp4A5[2]=Cyc_PP_text(({
const char*_tmp4A6=" ";_tag_arr(_tmp4A6,sizeof(char),_get_zero_arr_size(_tmp4A6,2));}));
_tmp4A5[1]=Cyc_Absynpp_ntyp2doc(_tmp4A2);_tmp4A5[0]=Cyc_Absynpp_tqual2doc(
_tmp4A1);Cyc_PP_cat(_tag_arr(_tmp4A5,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){if(f->width != 
0)return({struct Cyc_PP_Doc*_tmp4A7[5];_tmp4A7[4]=Cyc_PP_text(({const char*_tmp4AA=";";
_tag_arr(_tmp4AA,sizeof(char),_get_zero_arr_size(_tmp4AA,2));}));_tmp4A7[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp4A7[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp4A7[1]=Cyc_PP_text(({const char*_tmp4A9=":";_tag_arr(_tmp4A9,
sizeof(char),_get_zero_arr_size(_tmp4A9,2));}));_tmp4A7[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));
_tmp4A8->v=Cyc_PP_textptr(f->name);_tmp4A8;}));Cyc_PP_cat(_tag_arr(_tmp4A7,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4AB[3];
_tmp4AB[2]=Cyc_PP_text(({const char*_tmp4AD=";";_tag_arr(_tmp4AD,sizeof(char),
_get_zero_arr_size(_tmp4AD,2));}));_tmp4AB[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4AB[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->v=Cyc_PP_textptr(f->name);_tmp4AC;}));
Cyc_PP_cat(_tag_arr(_tmp4AB,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp4AE="";
_tag_arr(_tmp4AE,sizeof(char),_get_zero_arr_size(_tmp4AE,1));}),fields);}struct
Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*f){return({
struct Cyc_PP_Doc*_tmp4AF[3];_tmp4AF[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs);_tmp4AF[1]=Cyc_Absynpp_typedef_name2doc(f->name);_tmp4AF[0]=Cyc_Absynpp_scope2doc((
void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp4AF,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Tunionfield*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({
const char*_tmp4B0=",";_tag_arr(_tmp4B0,sizeof(char),_get_zero_arr_size(_tmp4B0,2));}),
fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4B1[0]={};Cyc_fprintf(f,({const char*_tmp4B2="\n";
_tag_arr(_tmp4B2,sizeof(char),_get_zero_arr_size(_tmp4B2,2));}),_tag_arr(_tmp4B1,
sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp4B3="";_tag_arr(
_tmp4B3,sizeof(char),_get_zero_arr_size(_tmp4B3,1));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _tagged_arr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _tagged_arr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _tagged_arr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _tagged_arr s=Cyc_Absynpp_typ2string(
t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _tagged_arr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _tagged_arr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
