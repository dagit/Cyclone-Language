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
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
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
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
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
struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;struct
_tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Buffer_t;
unsigned int Cyc_strlen(struct _tagged_arr s);int Cyc_strptrcmp(struct _tagged_arr*s1,
struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
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
Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*
fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
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
default: _LLA: if(c >= ' '  && c <= '~'){struct _tagged_arr _tmpC=Cyc_Core_new_string(2);({
struct _tagged_arr _tmpD=_tagged_arr_plus(_tmpC,sizeof(char),0);char _tmpE=*((char*)
_check_unknown_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(_get_arr_size(
_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))_throw_arraybounds();*((
char*)_tmpD.curr)=_tmpF;});return(struct _tagged_arr)_tmpC;}else{struct
_tagged_arr _tmp10=Cyc_Core_new_string(5);int j=0;({struct _tagged_arr _tmp11=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((char*)
_check_unknown_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_arr_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _tagged_arr _tmp14=
_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_unknown_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_arr_size(_tmp14,sizeof(char))== 1  && (_tmp15 == '\000'
 && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=_tmp16;});({
struct _tagged_arr _tmp17=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp18=*((
char*)_check_unknown_subscript(_tmp17,sizeof(char),0));char _tmp19=(char)('0' + (c
>> 3 & 7));if(_get_arr_size(_tmp17,sizeof(char))== 1  && (_tmp18 == '\000'  && 
_tmp19 != '\000'))_throw_arraybounds();*((char*)_tmp17.curr)=_tmp19;});({struct
_tagged_arr _tmp1A=_tagged_arr_plus(_tmp10,sizeof(char),j ++);char _tmp1B=*((char*)
_check_unknown_subscript(_tmp1A,sizeof(char),0));char _tmp1C=(char)('0' + (c & 7));
if(_get_arr_size(_tmp1A,sizeof(char))== 1  && (_tmp1B == '\000'  && _tmp1C != '\000'))
_throw_arraybounds();*((char*)_tmp1A.curr)=_tmp1C;});return(struct _tagged_arr)
_tmp10;}}}static int Cyc_Absynpp_special(struct _tagged_arr s){int sz=(int)(
_get_arr_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;i ++){char c=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~') || c == '"')
 || c == '\\')return 1;}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_arr_size(
s,sizeof(char))- 1);if(n > 0  && *((const char*)_check_unknown_subscript(s,sizeof(
char),n))== '\000')n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp1D=*((const
char*)_check_unknown_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;
_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp1D != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;
_LL18: goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D
!= '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;
goto _LLC;_LL1F:;_LL20: if(_tmp1D >= ' '  && _tmp1D <= '~')len ++;else{len +=4;}goto
_LLC;_LLC:;}}{struct _tagged_arr t=Cyc_Core_new_string((unsigned int)(len + 1));int
j=0;{int i=0;for(0;i <= n;i ++){char _tmp1E=*((const char*)_check_unknown_subscript(s,
sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:({struct _tagged_arr
_tmp1F=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp20=*((char*)
_check_unknown_subscript(_tmp1F,sizeof(char),0));char _tmp21='\\';if(
_get_arr_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'  && _tmp21 != '\000'))
_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({struct _tagged_arr _tmp22=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp23=*((char*)
_check_unknown_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(_get_arr_size(
_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _tagged_arr _tmp25=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp26=*((char*)_check_unknown_subscript(_tmp25,sizeof(char),0));char _tmp27='\\';
if(_get_arr_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'  && _tmp27 != '\000'))
_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({struct _tagged_arr _tmp28=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp29=*((char*)
_check_unknown_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(_get_arr_size(
_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _tagged_arr _tmp2B=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp2C=*((char*)_check_unknown_subscript(_tmp2B,sizeof(char),0));char _tmp2D='\\';
if(_get_arr_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'  && _tmp2D != '\000'))
_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({struct _tagged_arr _tmp2E=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp2F=*((char*)
_check_unknown_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(_get_arr_size(
_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _tagged_arr _tmp31=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp32=*((char*)_check_unknown_subscript(_tmp31,sizeof(char),0));char _tmp33='\\';
if(_get_arr_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'  && _tmp33 != '\000'))
_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({struct _tagged_arr _tmp34=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp35=*((char*)
_check_unknown_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(_get_arr_size(
_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _tagged_arr _tmp37=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp38=*((char*)_check_unknown_subscript(_tmp37,sizeof(char),0));char _tmp39='\\';
if(_get_arr_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'  && _tmp39 != '\000'))
_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({struct _tagged_arr _tmp3A=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp3B=*((char*)
_check_unknown_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(_get_arr_size(
_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _tagged_arr _tmp3D=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp3E=*((char*)_check_unknown_subscript(_tmp3D,sizeof(char),0));char _tmp3F='\\';
if(_get_arr_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'  && _tmp3F != '\000'))
_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({struct _tagged_arr _tmp40=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp41=*((char*)
_check_unknown_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(_get_arr_size(
_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _tagged_arr _tmp43=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp44=*((char*)_check_unknown_subscript(_tmp43,sizeof(char),0));char _tmp45='\\';
if(_get_arr_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'  && _tmp45 != '\000'))
_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({struct _tagged_arr _tmp46=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp47=*((char*)
_check_unknown_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(_get_arr_size(
_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _tagged_arr _tmp49=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp4A=*((char*)_check_unknown_subscript(_tmp49,sizeof(char),0));char _tmp4B='\\';
if(_get_arr_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'  && _tmp4B != '\000'))
_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({struct _tagged_arr _tmp4C=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp4D=*((char*)
_check_unknown_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_arr_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _tagged_arr _tmp4F=_tagged_arr_plus(t,sizeof(char),j ++);
char _tmp50=*((char*)_check_unknown_subscript(_tmp4F,sizeof(char),0));char _tmp51='\\';
if(_get_arr_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'  && _tmp51 != '\000'))
_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({struct _tagged_arr _tmp52=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp53=*((char*)
_check_unknown_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(_get_arr_size(
_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _tagged_arr _tmp55=_tagged_arr_plus(t,
sizeof(char),j ++);char _tmp56=*((char*)_check_unknown_subscript(_tmp55,sizeof(
char),0));char _tmp57=_tmp1E;if(_get_arr_size(_tmp55,sizeof(char))== 1  && (_tmp56
== '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});
else{({struct _tagged_arr _tmp58=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp59=*((
char*)_check_unknown_subscript(_tmp58,sizeof(char),0));char _tmp5A='\\';if(
_get_arr_size(_tmp58,sizeof(char))== 1  && (_tmp59 == '\000'  && _tmp5A != '\000'))
_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});({struct _tagged_arr _tmp5B=
_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5C=*((char*)
_check_unknown_subscript(_tmp5B,sizeof(char),0));char _tmp5D=(char)('0' + (_tmp1E
>> 6 & 7));if(_get_arr_size(_tmp5B,sizeof(char))== 1  && (_tmp5C == '\000'  && 
_tmp5D != '\000'))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});({struct
_tagged_arr _tmp5E=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp5F=*((char*)
_check_unknown_subscript(_tmp5E,sizeof(char),0));char _tmp60=(char)('0' + (_tmp1E
>> 3 & 7));if(_get_arr_size(_tmp5E,sizeof(char))== 1  && (_tmp5F == '\000'  && 
_tmp60 != '\000'))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});({struct
_tagged_arr _tmp61=_tagged_arr_plus(t,sizeof(char),j ++);char _tmp62=*((char*)
_check_unknown_subscript(_tmp61,sizeof(char),0));char _tmp63=(char)('0' + (_tmp1E & 
7));if(_get_arr_size(_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'  && _tmp63 != '\000'))
_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto _LL21;_LL21:;}}return(
struct _tagged_arr)t;}}}}static char _tmp64[9]="restrict";static struct _tagged_arr
Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
static struct _tagged_arr Cyc_Absynpp_volatile_string={_tmp65,_tmp65,_tmp65 + 9};
static char _tmp66[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={_tmp66,
_tmp66,_tmp66 + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
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
_LLA0;_LL9F: ptr=Cyc_Absynpp_question();goto _LL9D;_LLA0: if(_tmpED <= (void*)1  || *((
int*)_tmpED)!= 0)goto _LLA2;_tmpEE=((struct Cyc_Absyn_Upper_b_struct*)_tmpED)->f1;
_LLA1: ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD1)?({const char*_tmpF0="*";_tag_arr(_tmpF0,sizeof(char),_get_zero_arr_size(
_tmpF0,2));}):({const char*_tmpF1="@";_tag_arr(_tmpF1,sizeof(char),
_get_zero_arr_size(_tmpF1,2));}));{unsigned int _tmpF3;int _tmpF4;struct _tuple4
_tmpF2=Cyc_Evexp_eval_const_uint_exp(_tmpEE);_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;
if(!_tmpF4  || _tmpF3 != 1)ptr=({struct Cyc_PP_Doc*_tmpF5[4];_tmpF5[3]=Cyc_Absynpp_rb();
_tmpF5[2]=Cyc_Absynpp_exp2doc(_tmpEE);_tmpF5[1]=Cyc_Absynpp_lb();_tmpF5[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpF5,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;}_LLA2: if(
_tmpED <= (void*)1  || *((int*)_tmpED)!= 1)goto _LL9D;_tmpEF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmpED)->f1;_LLA3: ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD1)?({const char*_tmpF6="*";_tag_arr(_tmpF6,sizeof(char),_get_zero_arr_size(
_tmpF6,2));}):({const char*_tmpF7="@";_tag_arr(_tmpF7,sizeof(char),
_get_zero_arr_size(_tmpF7,2));}));ptr=({struct Cyc_PP_Doc*_tmpF8[4];_tmpF8[3]=Cyc_Absynpp_rb();
_tmpF8[2]=Cyc_Absynpp_typ2doc(_tmpEF);_tmpF8[1]=Cyc_Absynpp_lb();_tmpF8[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpF8,sizeof(struct Cyc_PP_Doc*),4));});goto _LL9D;_LL9D:;}
if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*_tmpF9[2];_tmpF9[1]=
Cyc_PP_text(({const char*_tmpFA="ZEROTERM ";_tag_arr(_tmpFA,sizeof(char),
_get_zero_arr_size(_tmpFA,10));}));_tmpF9[0]=ptr;Cyc_PP_cat(_tag_arr(_tmpF9,
sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD3))ptr=({struct Cyc_PP_Doc*_tmpFB[2];_tmpFB[1]=
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
_tmp109;_LLAF: if(_tmp107 <= (void*)3  || *((int*)_tmp107)!= 19)goto _LLB1;_tmp108=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp107)->f1;_LLB0:*rgions=({struct Cyc_List_List*
_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->hd=Cyc_Absynpp_rgn2doc(_tmp108);
_tmp10A->tl=*rgions;_tmp10A;});goto _LLAE;_LLB1: if(_tmp107 <= (void*)3  || *((int*)
_tmp107)!= 20)goto _LLB3;_tmp109=((struct Cyc_Absyn_JoinEff_struct*)_tmp107)->f1;
_LLB2: for(0;_tmp109 != 0;_tmp109=_tmp109->tl){Cyc_Absynpp_effects2docs(rgions,
effects,(void*)_tmp109->hd);}goto _LLAE;_LLB3:;_LLB4:*effects=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=Cyc_Absynpp_typ2doc(t);_tmp10B->tl=*
effects;_tmp10B;});goto _LLAE;_LLAE:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp10C="";
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
void*_tmp137;_LLBB: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 7)goto _LLBD;_LLBC:
return Cyc_PP_text(({const char*_tmp138="[[[array]]]";_tag_arr(_tmp138,sizeof(char),
_get_zero_arr_size(_tmp138,12));}));_LLBD: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 8)goto _LLBF;_LLBE: return Cyc_PP_nil_doc();_LLBF: if(_tmp118 <= (void*)3
 || *((int*)_tmp118)!= 4)goto _LLC1;_LLC0: return Cyc_PP_nil_doc();_LLC1: if((int)
_tmp118 != 0)goto _LLC3;_LLC2: s=Cyc_PP_text(({const char*_tmp139="void";_tag_arr(
_tmp139,sizeof(char),_get_zero_arr_size(_tmp139,5));}));goto _LLBA;_LLC3: if(
_tmp118 <= (void*)3  || *((int*)_tmp118)!= 0)goto _LLC5;_tmp119=((struct Cyc_Absyn_Evar_struct*)
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
_tmp140[3]=(!Cyc_Absynpp_print_full_evars  || _tmp11C == 0)?Cyc_PP_text(({const
char*_tmp146="";_tag_arr(_tmp146,sizeof(char),_get_zero_arr_size(_tmp146,1));})):
Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp11C->v);_tmp140[2]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp145;_tmp145.tag=1;_tmp145.f1=(
unsigned int)_tmp11B;{void*_tmp143[1]={& _tmp145};Cyc_aprintf(({const char*_tmp144="%d";
_tag_arr(_tmp144,sizeof(char),_get_zero_arr_size(_tmp144,3));}),_tag_arr(_tmp143,
sizeof(void*),1));}}));_tmp140[1]=Cyc_PP_text(({const char*_tmp142="(";_tag_arr(
_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,2));}));_tmp140[0]=Cyc_PP_text(({
const char*_tmp141="%";_tag_arr(_tmp141,sizeof(char),_get_zero_arr_size(_tmp141,2));}));
Cyc_PP_cat(_tag_arr(_tmp140,sizeof(struct Cyc_PP_Doc*),6));});_LLED:;}goto _LLBA;
_LLC5: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 1)goto _LLC7;_tmp11D=((struct
Cyc_Absyn_VarType_struct*)_tmp118)->f1;_LLC6: s=Cyc_PP_textptr(_tmp11D->name);if(
Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp148[3];_tmp148[2]=Cyc_Absynpp_kindbound2doc((
void*)_tmp11D->kind);_tmp148[1]=Cyc_PP_text(({const char*_tmp149="::";_tag_arr(
_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,3));}));_tmp148[0]=s;Cyc_PP_cat(
_tag_arr(_tmp148,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp11D))s=({struct Cyc_PP_Doc*_tmp14A[3];_tmp14A[2]=
Cyc_PP_text(({const char*_tmp14C=" */";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,4));}));_tmp14A[1]=s;_tmp14A[0]=Cyc_PP_text(({const
char*_tmp14B="_ /* ";_tag_arr(_tmp14B,sizeof(char),_get_zero_arr_size(_tmp14B,6));}));
Cyc_PP_cat(_tag_arr(_tmp14A,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLC7: if(
_tmp118 <= (void*)3  || *((int*)_tmp118)!= 2)goto _LLC9;_tmp11E=((struct Cyc_Absyn_TunionType_struct*)
_tmp118)->f1;_tmp11F=(void*)_tmp11E.tunion_info;_tmp120=_tmp11E.targs;_tmp121=(
void*)_tmp11E.rgn;_LLC8:{void*_tmp14D=_tmp11F;struct Cyc_Absyn_UnknownTunionInfo
_tmp14E;struct _tuple0*_tmp14F;int _tmp150;struct Cyc_Absyn_Tuniondecl**_tmp151;
struct Cyc_Absyn_Tuniondecl*_tmp152;struct Cyc_Absyn_Tuniondecl _tmp153;struct
_tuple0*_tmp154;int _tmp155;_LLF3: if(*((int*)_tmp14D)!= 0)goto _LLF5;_tmp14E=((
struct Cyc_Absyn_UnknownTunion_struct*)_tmp14D)->f1;_tmp14F=_tmp14E.name;_tmp150=
_tmp14E.is_xtunion;_LLF4: _tmp154=_tmp14F;_tmp155=_tmp150;goto _LLF6;_LLF5: if(*((
int*)_tmp14D)!= 1)goto _LLF2;_tmp151=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp14D)->f1;_tmp152=*_tmp151;_tmp153=*_tmp152;_tmp154=_tmp153.name;_tmp155=
_tmp153.is_xtunion;_LLF6: {struct Cyc_PP_Doc*_tmp156=Cyc_PP_text(_tmp155?({const
char*_tmp15B="xtunion ";_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,
9));}):({const char*_tmp15C="tunion ";_tag_arr(_tmp15C,sizeof(char),
_get_zero_arr_size(_tmp15C,8));}));{void*_tmp157=Cyc_Tcutil_compress(_tmp121);
_LLF8: if((int)_tmp157 != 2)goto _LLFA;_LLF9: s=({struct Cyc_PP_Doc*_tmp158[3];
_tmp158[2]=Cyc_Absynpp_tps2doc(_tmp120);_tmp158[1]=Cyc_Absynpp_qvar2doc(_tmp154);
_tmp158[0]=_tmp156;Cyc_PP_cat(_tag_arr(_tmp158,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLF7;_LLFA:;_LLFB: s=({struct Cyc_PP_Doc*_tmp159[5];_tmp159[4]=Cyc_Absynpp_tps2doc(
_tmp120);_tmp159[3]=Cyc_Absynpp_qvar2doc(_tmp154);_tmp159[2]=Cyc_PP_text(({const
char*_tmp15A=" ";_tag_arr(_tmp15A,sizeof(char),_get_zero_arr_size(_tmp15A,2));}));
_tmp159[1]=Cyc_Absynpp_typ2doc(_tmp121);_tmp159[0]=_tmp156;Cyc_PP_cat(_tag_arr(
_tmp159,sizeof(struct Cyc_PP_Doc*),5));});goto _LLF7;_LLF7:;}goto _LLF2;}_LLF2:;}
goto _LLBA;_LLC9: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 3)goto _LLCB;_tmp122=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp118)->f1;_tmp123=(void*)_tmp122.field_info;
_tmp124=_tmp122.targs;_LLCA:{void*_tmp15D=_tmp123;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp15E;struct _tuple0*_tmp15F;struct _tuple0*_tmp160;int _tmp161;struct Cyc_Absyn_Tuniondecl*
_tmp162;struct Cyc_Absyn_Tuniondecl _tmp163;struct _tuple0*_tmp164;int _tmp165;
struct Cyc_Absyn_Tunionfield*_tmp166;struct Cyc_Absyn_Tunionfield _tmp167;struct
_tuple0*_tmp168;_LLFD: if(*((int*)_tmp15D)!= 0)goto _LLFF;_tmp15E=((struct Cyc_Absyn_UnknownTunionfield_struct*)
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
4));});goto _LLFC;}_LLFC:;}goto _LLBA;_LLCB: if(_tmp118 <= (void*)3  || *((int*)
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
void*)3  || *((int*)_tmp118)!= 6)goto _LLD1;_tmp127=((struct Cyc_Absyn_DoubleType_struct*)
_tmp118)->f1;_LLD0: if(_tmp127)s=Cyc_PP_text(({const char*_tmp180="long double";
_tag_arr(_tmp180,sizeof(char),_get_zero_arr_size(_tmp180,12));}));else{s=Cyc_PP_text(({
const char*_tmp181="double";_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(
_tmp181,7));}));}goto _LLBA;_LLD1: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 9)
goto _LLD3;_tmp128=((struct Cyc_Absyn_TupleType_struct*)_tmp118)->f1;_LLD2: s=({
struct Cyc_PP_Doc*_tmp182[2];_tmp182[1]=Cyc_Absynpp_args2doc(_tmp128);_tmp182[0]=
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp182,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBA;_LLD3: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 10)goto _LLD5;_tmp129=((
struct Cyc_Absyn_AggrType_struct*)_tmp118)->f1;_tmp12A=(void*)_tmp129.aggr_info;
_tmp12B=_tmp129.targs;_LLD4: {void*_tmp184;struct _tuple0*_tmp185;struct _tuple3
_tmp183=Cyc_Absyn_aggr_kinded_name(_tmp12A);_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;
s=({struct Cyc_PP_Doc*_tmp186[3];_tmp186[2]=Cyc_Absynpp_tps2doc(_tmp12B);_tmp186[
1]=Cyc_Absynpp_qvar2doc(_tmp185);_tmp186[0]=Cyc_Absynpp_aggr_kind2doc(_tmp184);
Cyc_PP_cat(_tag_arr(_tmp186,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;}_LLD5:
if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 11)goto _LLD7;_tmp12C=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp118)->f1;_tmp12D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp118)->f2;_LLD6: s=({struct Cyc_PP_Doc*_tmp187[4];_tmp187[3]=Cyc_Absynpp_rb();
_tmp187[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp12D));_tmp187[1]=Cyc_Absynpp_lb();
_tmp187[0]=Cyc_Absynpp_aggr_kind2doc(_tmp12C);Cyc_PP_cat(_tag_arr(_tmp187,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLBA;_LLD7: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 13)goto _LLD9;_tmp12E=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp118)->f1;
_LLD8: s=({struct Cyc_PP_Doc*_tmp188[4];_tmp188[3]=Cyc_Absynpp_rb();_tmp188[2]=Cyc_PP_nest(
2,Cyc_Absynpp_enumfields2doc(_tmp12E));_tmp188[1]=Cyc_Absynpp_lb();_tmp188[0]=
Cyc_PP_text(({const char*_tmp189="enum ";_tag_arr(_tmp189,sizeof(char),
_get_zero_arr_size(_tmp189,6));}));Cyc_PP_cat(_tag_arr(_tmp188,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLBA;_LLD9: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 12)goto _LLDB;
_tmp12F=((struct Cyc_Absyn_EnumType_struct*)_tmp118)->f1;_LLDA: s=({struct Cyc_PP_Doc*
_tmp18A[2];_tmp18A[1]=Cyc_Absynpp_qvar2doc(_tmp12F);_tmp18A[0]=Cyc_PP_text(({
const char*_tmp18B="enum ";_tag_arr(_tmp18B,sizeof(char),_get_zero_arr_size(
_tmp18B,6));}));Cyc_PP_cat(_tag_arr(_tmp18A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBA;_LLDB: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 14)goto _LLDD;_tmp130=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp118)->f1;_LLDC: s=({struct Cyc_PP_Doc*
_tmp18C[3];_tmp18C[2]=Cyc_PP_text(({const char*_tmp18E=">";_tag_arr(_tmp18E,
sizeof(char),_get_zero_arr_size(_tmp18E,2));}));_tmp18C[1]=Cyc_Absynpp_typ2doc(
_tmp130);_tmp18C[0]=Cyc_PP_text(({const char*_tmp18D="sizeof_t<";_tag_arr(_tmp18D,
sizeof(char),_get_zero_arr_size(_tmp18D,10));}));Cyc_PP_cat(_tag_arr(_tmp18C,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLDD: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 16)goto _LLDF;_tmp131=((struct Cyc_Absyn_TypedefType_struct*)_tmp118)->f1;
_tmp132=((struct Cyc_Absyn_TypedefType_struct*)_tmp118)->f2;_tmp133=((struct Cyc_Absyn_TypedefType_struct*)
_tmp118)->f3;_LLDE: s=({struct Cyc_PP_Doc*_tmp18F[2];_tmp18F[1]=Cyc_Absynpp_tps2doc(
_tmp132);_tmp18F[0]=Cyc_Absynpp_qvar2doc(_tmp131);Cyc_PP_cat(_tag_arr(_tmp18F,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBA;_LLDF: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 15)goto _LLE1;_tmp134=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp118)->f1;_LLE0: s=({struct Cyc_PP_Doc*_tmp190[3];_tmp190[2]=Cyc_PP_text(({
const char*_tmp192=">";_tag_arr(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,2));}));
_tmp190[1]=Cyc_Absynpp_rgn2doc(_tmp134);_tmp190[0]=Cyc_PP_text(({const char*
_tmp191="region_t<";_tag_arr(_tmp191,sizeof(char),_get_zero_arr_size(_tmp191,10));}));
Cyc_PP_cat(_tag_arr(_tmp190,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE1: if(
_tmp118 <= (void*)3  || *((int*)_tmp118)!= 18)goto _LLE3;_tmp135=((struct Cyc_Absyn_TypeInt_struct*)
_tmp118)->f1;_LLE2: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp195;_tmp195.tag=1;_tmp195.f1=(unsigned int)_tmp135;{void*_tmp193[1]={&
_tmp195};Cyc_aprintf(({const char*_tmp194="`%d";_tag_arr(_tmp194,sizeof(char),
_get_zero_arr_size(_tmp194,4));}),_tag_arr(_tmp193,sizeof(void*),1));}}));goto
_LLBA;_LLE3: if(_tmp118 <= (void*)3  || *((int*)_tmp118)!= 17)goto _LLE5;_tmp136=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp118)->f1;_LLE4: s=({struct Cyc_PP_Doc*
_tmp196[3];_tmp196[2]=Cyc_PP_text(({const char*_tmp198=">";_tag_arr(_tmp198,
sizeof(char),_get_zero_arr_size(_tmp198,2));}));_tmp196[1]=Cyc_Absynpp_typ2doc(
_tmp136);_tmp196[0]=Cyc_PP_text(({const char*_tmp197="tag_t<";_tag_arr(_tmp197,
sizeof(char),_get_zero_arr_size(_tmp197,7));}));Cyc_PP_cat(_tag_arr(_tmp196,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE5: if((int)_tmp118 != 2)goto _LLE7;
_LLE6: s=Cyc_Absynpp_rgn2doc(t);goto _LLBA;_LLE7: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 21)goto _LLE9;_tmp137=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp118)->f1;
_LLE8: s=({struct Cyc_PP_Doc*_tmp199[3];_tmp199[2]=Cyc_PP_text(({const char*_tmp19B=")";
_tag_arr(_tmp19B,sizeof(char),_get_zero_arr_size(_tmp19B,2));}));_tmp199[1]=Cyc_Absynpp_typ2doc(
_tmp137);_tmp199[0]=Cyc_PP_text(({const char*_tmp19A="regions(";_tag_arr(_tmp19A,
sizeof(char),_get_zero_arr_size(_tmp19A,9));}));Cyc_PP_cat(_tag_arr(_tmp199,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBA;_LLE9: if(_tmp118 <= (void*)3  || *((int*)
_tmp118)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(_tmp118 <= (void*)3  || *((int*)
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
_tmp1BA,sizeof(char),_get_zero_arr_size(_tmp1BA,2));}),_tmp1A6);if(effopt != 0
 && Cyc_Absynpp_print_all_effects)_tmp1B0=({struct Cyc_PP_Doc*_tmp1B1[3];_tmp1B1[
2]=Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1B1[1]=Cyc_PP_text(({const char*
_tmp1B2=";";_tag_arr(_tmp1B2,sizeof(char),_get_zero_arr_size(_tmp1B2,2));}));
_tmp1B1[0]=_tmp1B0;Cyc_PP_cat(_tag_arr(_tmp1B1,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po != 0)_tmp1B0=({struct Cyc_PP_Doc*_tmp1B3[3];_tmp1B3[2]=Cyc_Absynpp_rgnpo2doc(
rgn_po);_tmp1B3[1]=Cyc_PP_text(({const char*_tmp1B4=":";_tag_arr(_tmp1B4,sizeof(
char),_get_zero_arr_size(_tmp1B4,2));}));_tmp1B3[0]=_tmp1B0;Cyc_PP_cat(_tag_arr(
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
if(_tmp1BE <= (void*)1  || *((int*)_tmp1BE)!= 0)goto _LL11D;_tmp1BF=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1BE)->f1;_LL11C: match=0;_tmp1BD=_tmp1BF;goto _LL118;_LL11D: if(_tmp1BE <= (void*)
1  || *((int*)_tmp1BE)!= 1)goto _LL118;_tmp1C0=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1BE)->f1;_LL11E: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C0,Cyc_Absynpp_curr_namespace);
_tmp1BD=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
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
_tmp1C9 != 0)goto _LL122;_LL121: goto _LL123;_LL122: if(_tmp1C9 <= (void*)1  || *((int*)
_tmp1C9)!= 0)goto _LL124;_tmp1CA=((struct Cyc_Absyn_Rel_n_struct*)_tmp1C9)->f1;if(
_tmp1CA != 0)goto _LL124;_LL123: return*(*v).f2;_LL124: if(_tmp1C9 <= (void*)1  || *((
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
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
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
int*)_tmp1D0)!= 7)goto _LL139;_LL138: return 35;_LL139: if(*((int*)_tmp1D0)!= 8)goto
_LL13B;_LL13A: return 30;_LL13B: if(*((int*)_tmp1D0)!= 9)goto _LL13D;_LL13C: return 10;
_LL13D: if(*((int*)_tmp1D0)!= 10)goto _LL13F;_LL13E: goto _LL140;_LL13F: if(*((int*)
_tmp1D0)!= 11)goto _LL141;_LL140: return 140;_LL141: if(*((int*)_tmp1D0)!= 12)goto
_LL143;_LL142: return 130;_LL143: if(*((int*)_tmp1D0)!= 13)goto _LL145;_tmp1D2=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D0)->f1;_LL144: return Cyc_Absynpp_exp_prec(
_tmp1D2);_LL145: if(*((int*)_tmp1D0)!= 14)goto _LL147;_tmp1D3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D0)->f1;_LL146: return Cyc_Absynpp_exp_prec(_tmp1D3);_LL147: if(*((int*)
_tmp1D0)!= 15)goto _LL149;_LL148: return 120;_LL149: if(*((int*)_tmp1D0)!= 17)goto
_LL14B;_LL14A: return 15;_LL14B: if(*((int*)_tmp1D0)!= 16)goto _LL14D;_LL14C: goto
_LL14E;_LL14D: if(*((int*)_tmp1D0)!= 18)goto _LL14F;_LL14E: goto _LL150;_LL14F: if(*((
int*)_tmp1D0)!= 19)goto _LL151;_LL150: goto _LL152;_LL151: if(*((int*)_tmp1D0)!= 20)
goto _LL153;_LL152: goto _LL154;_LL153: if(*((int*)_tmp1D0)!= 21)goto _LL155;_LL154:
goto _LL156;_LL155: if(*((int*)_tmp1D0)!= 22)goto _LL157;_LL156: return 130;_LL157:
if(*((int*)_tmp1D0)!= 23)goto _LL159;_LL158: goto _LL15A;_LL159: if(*((int*)_tmp1D0)
!= 24)goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(*((int*)_tmp1D0)!= 25)goto _LL15D;
_LL15C: return 140;_LL15D: if(*((int*)_tmp1D0)!= 26)goto _LL15F;_LL15E: return 150;
_LL15F: if(*((int*)_tmp1D0)!= 27)goto _LL161;_LL160: goto _LL162;_LL161: if(*((int*)
_tmp1D0)!= 28)goto _LL163;_LL162: goto _LL164;_LL163: if(*((int*)_tmp1D0)!= 29)goto
_LL165;_LL164: goto _LL166;_LL165: if(*((int*)_tmp1D0)!= 30)goto _LL167;_LL166: goto
_LL168;_LL167: if(*((int*)_tmp1D0)!= 31)goto _LL169;_LL168: goto _LL16A;_LL169: if(*((
int*)_tmp1D0)!= 32)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if(*((int*)_tmp1D0)!= 33)
goto _LL16D;_LL16C: goto _LL16E;_LL16D: if(*((int*)_tmp1D0)!= 34)goto _LL16F;_LL16E:
goto _LL170;_LL16F: if(*((int*)_tmp1D0)!= 35)goto _LL171;_LL170: goto _LL172;_LL171:
if(*((int*)_tmp1D0)!= 36)goto _LL173;_LL172: return 140;_LL173: if(*((int*)_tmp1D0)
!= 37)goto _LL128;_LL174: return 10000;_LL128:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1D5=(void*)e->r;void*_tmp1D6;struct _tuple0*_tmp1D7;struct _tuple0*
_tmp1D8;void*_tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;
struct Cyc_Core_Opt*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*
_tmp1DE;void*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;
struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*
_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_List_List*
_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;void*_tmp1F0;
struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;
void*_tmp1F7;void*_tmp1F8;struct _tagged_arr*_tmp1F9;void*_tmp1FA;void*_tmp1FB;
unsigned int _tmp1FC;struct Cyc_List_List*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Exp*
_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct _tagged_arr*_tmp201;struct Cyc_Absyn_Exp*
_tmp202;struct _tagged_arr*_tmp203;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*
_tmp205;struct Cyc_List_List*_tmp206;struct _tuple1*_tmp207;struct Cyc_List_List*
_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Vardecl*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct _tuple0*_tmp20D;struct Cyc_List_List*
_tmp20E;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*
_tmp211;struct Cyc_Absyn_Tunionfield*_tmp212;struct _tuple0*_tmp213;struct _tuple0*
_tmp214;struct Cyc_Absyn_MallocInfo _tmp215;int _tmp216;struct Cyc_Absyn_Exp*_tmp217;
void**_tmp218;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Core_Opt*_tmp21A;struct Cyc_List_List*
_tmp21B;struct Cyc_Absyn_Stmt*_tmp21C;_LL19F: if(*((int*)_tmp1D5)!= 0)goto _LL1A1;
_tmp1D6=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1D5)->f1;_LL1A0: s=Cyc_Absynpp_cnst2doc(
_tmp1D6);goto _LL19E;_LL1A1: if(*((int*)_tmp1D5)!= 1)goto _LL1A3;_tmp1D7=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D5)->f1;_LL1A2: _tmp1D8=_tmp1D7;goto _LL1A4;_LL1A3:
if(*((int*)_tmp1D5)!= 2)goto _LL1A5;_tmp1D8=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D5)->f1;_LL1A4: s=Cyc_Absynpp_qvar2doc(_tmp1D8);goto _LL19E;_LL1A5: if(*((int*)
_tmp1D5)!= 3)goto _LL1A7;_tmp1D9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D5)->f1;_tmp1DA=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D5)->f2;_LL1A6: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1D9,_tmp1DA);goto _LL19E;_LL1A7: if(*((int*)
_tmp1D5)!= 4)goto _LL1A9;_tmp1DB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f1;
_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f2;_tmp1DD=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D5)->f3;_LL1A8: s=({struct Cyc_PP_Doc*_tmp21D[5];_tmp21D[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DD);_tmp21D[3]=Cyc_PP_text(({const char*_tmp220="= ";_tag_arr(_tmp220,
sizeof(char),_get_zero_arr_size(_tmp220,3));}));_tmp21D[2]=_tmp1DC == 0?Cyc_PP_text(({
const char*_tmp21F="";_tag_arr(_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,1));})):
Cyc_Absynpp_prim2doc((void*)_tmp1DC->v);_tmp21D[1]=Cyc_PP_text(({const char*
_tmp21E=" ";_tag_arr(_tmp21E,sizeof(char),_get_zero_arr_size(_tmp21E,2));}));
_tmp21D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DB);Cyc_PP_cat(_tag_arr(_tmp21D,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1A9: if(*((int*)_tmp1D5)!= 5)goto
_LL1AB;_tmp1DE=((struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f1;_tmp1DF=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f2;_LL1AA: {struct Cyc_PP_Doc*
_tmp221=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DE);{void*_tmp222=_tmp1DF;_LL1EE:
if((int)_tmp222 != 0)goto _LL1F0;_LL1EF: s=({struct Cyc_PP_Doc*_tmp223[2];_tmp223[1]=
_tmp221;_tmp223[0]=Cyc_PP_text(({const char*_tmp224="++";_tag_arr(_tmp224,sizeof(
char),_get_zero_arr_size(_tmp224,3));}));Cyc_PP_cat(_tag_arr(_tmp223,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1ED;_LL1F0: if((int)_tmp222 != 2)goto _LL1F2;
_LL1F1: s=({struct Cyc_PP_Doc*_tmp225[2];_tmp225[1]=_tmp221;_tmp225[0]=Cyc_PP_text(({
const char*_tmp226="--";_tag_arr(_tmp226,sizeof(char),_get_zero_arr_size(_tmp226,
3));}));Cyc_PP_cat(_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1ED;
_LL1F2: if((int)_tmp222 != 1)goto _LL1F4;_LL1F3: s=({struct Cyc_PP_Doc*_tmp227[2];
_tmp227[1]=Cyc_PP_text(({const char*_tmp228="++";_tag_arr(_tmp228,sizeof(char),
_get_zero_arr_size(_tmp228,3));}));_tmp227[0]=_tmp221;Cyc_PP_cat(_tag_arr(
_tmp227,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1ED;_LL1F4: if((int)_tmp222 != 3)
goto _LL1ED;_LL1F5: s=({struct Cyc_PP_Doc*_tmp229[2];_tmp229[1]=Cyc_PP_text(({const
char*_tmp22A="--";_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,3));}));
_tmp229[0]=_tmp221;Cyc_PP_cat(_tag_arr(_tmp229,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1ED;_LL1ED:;}goto _LL19E;}_LL1AB: if(*((int*)_tmp1D5)!= 6)goto _LL1AD;
_tmp1E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f1;_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D5)->f2;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f3;_LL1AC:
s=({struct Cyc_PP_Doc*_tmp22B[5];_tmp22B[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E2);_tmp22B[3]=Cyc_PP_text(({const char*_tmp22D=" : ";_tag_arr(_tmp22D,
sizeof(char),_get_zero_arr_size(_tmp22D,4));}));_tmp22B[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E1);_tmp22B[1]=Cyc_PP_text(({const char*_tmp22C=" ? ";_tag_arr(_tmp22C,
sizeof(char),_get_zero_arr_size(_tmp22C,4));}));_tmp22B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E0);Cyc_PP_cat(_tag_arr(_tmp22B,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL19E;_LL1AD: if(*((int*)_tmp1D5)!= 7)goto _LL1AF;_tmp1E3=((struct Cyc_Absyn_And_e_struct*)
_tmp1D5)->f1;_tmp1E4=((struct Cyc_Absyn_And_e_struct*)_tmp1D5)->f2;_LL1AE: s=({
struct Cyc_PP_Doc*_tmp22E[3];_tmp22E[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4);
_tmp22E[1]=Cyc_PP_text(({const char*_tmp22F=" && ";_tag_arr(_tmp22F,sizeof(char),
_get_zero_arr_size(_tmp22F,5));}));_tmp22E[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E3);Cyc_PP_cat(_tag_arr(_tmp22E,sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;
_LL1AF: if(*((int*)_tmp1D5)!= 8)goto _LL1B1;_tmp1E5=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D5)->f1;_tmp1E6=((struct Cyc_Absyn_Or_e_struct*)_tmp1D5)->f2;_LL1B0: s=({
struct Cyc_PP_Doc*_tmp230[3];_tmp230[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E6);
_tmp230[1]=Cyc_PP_text(({const char*_tmp231=" || ";_tag_arr(_tmp231,sizeof(char),
_get_zero_arr_size(_tmp231,5));}));_tmp230[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E5);Cyc_PP_cat(_tag_arr(_tmp230,sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;
_LL1B1: if(*((int*)_tmp1D5)!= 9)goto _LL1B3;_tmp1E7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D5)->f1;_tmp1E8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D5)->f2;_LL1B2: s=({
struct Cyc_PP_Doc*_tmp232[5];_tmp232[4]=Cyc_PP_text(({const char*_tmp235=")";
_tag_arr(_tmp235,sizeof(char),_get_zero_arr_size(_tmp235,2));}));_tmp232[3]=Cyc_Absynpp_exp2doc(
_tmp1E8);_tmp232[2]=Cyc_PP_text(({const char*_tmp234=", ";_tag_arr(_tmp234,
sizeof(char),_get_zero_arr_size(_tmp234,3));}));_tmp232[1]=Cyc_Absynpp_exp2doc(
_tmp1E7);_tmp232[0]=Cyc_PP_text(({const char*_tmp233="(";_tag_arr(_tmp233,sizeof(
char),_get_zero_arr_size(_tmp233,2));}));Cyc_PP_cat(_tag_arr(_tmp232,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1B3: if(*((int*)_tmp1D5)!= 10)goto _LL1B5;
_tmp1E9=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D5)->f1;_tmp1EA=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D5)->f2;_LL1B4: s=({struct Cyc_PP_Doc*_tmp236[4];_tmp236[3]=Cyc_PP_text(({
const char*_tmp238=")";_tag_arr(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,2));}));
_tmp236[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1EA);_tmp236[1]=Cyc_PP_text(({const
char*_tmp237="(";_tag_arr(_tmp237,sizeof(char),_get_zero_arr_size(_tmp237,2));}));
_tmp236[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E9);Cyc_PP_cat(_tag_arr(_tmp236,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1B5: if(*((int*)_tmp1D5)!= 11)goto
_LL1B7;_tmp1EB=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f1;_tmp1EC=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f2;_LL1B6: s=({struct Cyc_PP_Doc*_tmp239[4];
_tmp239[3]=Cyc_PP_text(({const char*_tmp23B=")";_tag_arr(_tmp23B,sizeof(char),
_get_zero_arr_size(_tmp23B,2));}));_tmp239[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1EC);_tmp239[1]=Cyc_PP_text(({const char*_tmp23A="(";_tag_arr(_tmp23A,sizeof(
char),_get_zero_arr_size(_tmp23A,2));}));_tmp239[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EB);Cyc_PP_cat(_tag_arr(_tmp239,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL19E;_LL1B7: if(*((int*)_tmp1D5)!= 12)goto _LL1B9;_tmp1ED=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D5)->f1;_LL1B8: s=({struct Cyc_PP_Doc*_tmp23C[2];_tmp23C[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1ED);_tmp23C[0]=Cyc_PP_text(({const char*_tmp23D="throw ";_tag_arr(
_tmp23D,sizeof(char),_get_zero_arr_size(_tmp23D,7));}));Cyc_PP_cat(_tag_arr(
_tmp23C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1B9: if(*((int*)_tmp1D5)!= 
13)goto _LL1BB;_tmp1EE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D5)->f1;
_LL1BA: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EE);goto _LL19E;_LL1BB: if(*((int*)
_tmp1D5)!= 14)goto _LL1BD;_tmp1EF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D5)->f1;
_LL1BC: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EF);goto _LL19E;_LL1BD: if(*((int*)
_tmp1D5)!= 15)goto _LL1BF;_tmp1F0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f1;
_tmp1F1=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f2;_LL1BE: s=({struct Cyc_PP_Doc*
_tmp23E[4];_tmp23E[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F1);_tmp23E[2]=Cyc_PP_text(({
const char*_tmp240=")";_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,2));}));
_tmp23E[1]=Cyc_Absynpp_typ2doc(_tmp1F0);_tmp23E[0]=Cyc_PP_text(({const char*
_tmp23F="(";_tag_arr(_tmp23F,sizeof(char),_get_zero_arr_size(_tmp23F,2));}));Cyc_PP_cat(
_tag_arr(_tmp23E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1BF: if(*((int*)
_tmp1D5)!= 16)goto _LL1C1;_tmp1F2=((struct Cyc_Absyn_Address_e_struct*)_tmp1D5)->f1;
_LL1C0: s=({struct Cyc_PP_Doc*_tmp241[2];_tmp241[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2);_tmp241[0]=Cyc_PP_text(({const char*_tmp242="&";_tag_arr(_tmp242,
sizeof(char),_get_zero_arr_size(_tmp242,2));}));Cyc_PP_cat(_tag_arr(_tmp241,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1C1: if(*((int*)_tmp1D5)!= 17)goto
_LL1C3;_tmp1F3=((struct Cyc_Absyn_New_e_struct*)_tmp1D5)->f1;_tmp1F4=((struct Cyc_Absyn_New_e_struct*)
_tmp1D5)->f2;_LL1C2: if(_tmp1F3 == 0)s=({struct Cyc_PP_Doc*_tmp243[2];_tmp243[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F4);_tmp243[0]=Cyc_PP_text(({const char*
_tmp244="new ";_tag_arr(_tmp244,sizeof(char),_get_zero_arr_size(_tmp244,5));}));
Cyc_PP_cat(_tag_arr(_tmp243,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp245[4];_tmp245[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F4);_tmp245[2]=Cyc_PP_text(({
const char*_tmp247=") ";_tag_arr(_tmp247,sizeof(char),_get_zero_arr_size(_tmp247,
3));}));_tmp245[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp1F3);_tmp245[0]=
Cyc_PP_text(({const char*_tmp246="rnew(";_tag_arr(_tmp246,sizeof(char),
_get_zero_arr_size(_tmp246,6));}));Cyc_PP_cat(_tag_arr(_tmp245,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL19E;_LL1C3: if(*((int*)_tmp1D5)!= 18)goto _LL1C5;_tmp1F5=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D5)->f1;_LL1C4: s=({struct Cyc_PP_Doc*
_tmp248[3];_tmp248[2]=Cyc_PP_text(({const char*_tmp24A=")";_tag_arr(_tmp24A,
sizeof(char),_get_zero_arr_size(_tmp24A,2));}));_tmp248[1]=Cyc_Absynpp_typ2doc(
_tmp1F5);_tmp248[0]=Cyc_PP_text(({const char*_tmp249="sizeof(";_tag_arr(_tmp249,
sizeof(char),_get_zero_arr_size(_tmp249,8));}));Cyc_PP_cat(_tag_arr(_tmp248,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1C5: if(*((int*)_tmp1D5)!= 19)goto
_LL1C7;_tmp1F6=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D5)->f1;_LL1C6: s=({
struct Cyc_PP_Doc*_tmp24B[3];_tmp24B[2]=Cyc_PP_text(({const char*_tmp24D=")";
_tag_arr(_tmp24D,sizeof(char),_get_zero_arr_size(_tmp24D,2));}));_tmp24B[1]=Cyc_Absynpp_exp2doc(
_tmp1F6);_tmp24B[0]=Cyc_PP_text(({const char*_tmp24C="sizeof(";_tag_arr(_tmp24C,
sizeof(char),_get_zero_arr_size(_tmp24C,8));}));Cyc_PP_cat(_tag_arr(_tmp24B,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1C7: if(*((int*)_tmp1D5)!= 20)goto
_LL1C9;_tmp1F7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;_tmp1F8=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)_tmp1F8)!= 0)
goto _LL1C9;_tmp1F9=((struct Cyc_Absyn_StructField_struct*)_tmp1F8)->f1;_LL1C8: s=({
struct Cyc_PP_Doc*_tmp24E[5];_tmp24E[4]=Cyc_PP_text(({const char*_tmp251=")";
_tag_arr(_tmp251,sizeof(char),_get_zero_arr_size(_tmp251,2));}));_tmp24E[3]=Cyc_PP_textptr(
_tmp1F9);_tmp24E[2]=Cyc_PP_text(({const char*_tmp250=",";_tag_arr(_tmp250,sizeof(
char),_get_zero_arr_size(_tmp250,2));}));_tmp24E[1]=Cyc_Absynpp_typ2doc(_tmp1F7);
_tmp24E[0]=Cyc_PP_text(({const char*_tmp24F="offsetof(";_tag_arr(_tmp24F,sizeof(
char),_get_zero_arr_size(_tmp24F,10));}));Cyc_PP_cat(_tag_arr(_tmp24E,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1C9: if(*((int*)_tmp1D5)!= 20)goto _LL1CB;
_tmp1FA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;_tmp1FB=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)_tmp1FB)!= 1)goto
_LL1CB;_tmp1FC=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FB)->f1;_LL1CA: s=({
struct Cyc_PP_Doc*_tmp252[5];_tmp252[4]=Cyc_PP_text(({const char*_tmp258=")";
_tag_arr(_tmp258,sizeof(char),_get_zero_arr_size(_tmp258,2));}));_tmp252[3]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp257;_tmp257.tag=1;_tmp257.f1=(
unsigned int)((int)_tmp1FC);{void*_tmp255[1]={& _tmp257};Cyc_aprintf(({const char*
_tmp256="%d";_tag_arr(_tmp256,sizeof(char),_get_zero_arr_size(_tmp256,3));}),
_tag_arr(_tmp255,sizeof(void*),1));}}));_tmp252[2]=Cyc_PP_text(({const char*
_tmp254=",";_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,2));}));
_tmp252[1]=Cyc_Absynpp_typ2doc(_tmp1FA);_tmp252[0]=Cyc_PP_text(({const char*
_tmp253="offsetof(";_tag_arr(_tmp253,sizeof(char),_get_zero_arr_size(_tmp253,10));}));
Cyc_PP_cat(_tag_arr(_tmp252,sizeof(struct Cyc_PP_Doc*),5));});goto _LL19E;_LL1CB:
if(*((int*)_tmp1D5)!= 21)goto _LL1CD;_tmp1FD=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D5)->f1;_tmp1FE=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D5)->f2;
_LL1CC: s=({struct Cyc_PP_Doc*_tmp259[4];_tmp259[3]=Cyc_PP_text(({const char*
_tmp25B=")";_tag_arr(_tmp25B,sizeof(char),_get_zero_arr_size(_tmp25B,2));}));
_tmp259[2]=Cyc_Absynpp_typ2doc(_tmp1FE);_tmp259[1]=Cyc_Absynpp_tvars2doc(_tmp1FD);
_tmp259[0]=Cyc_PP_text(({const char*_tmp25A="__gen(";_tag_arr(_tmp25A,sizeof(char),
_get_zero_arr_size(_tmp25A,7));}));Cyc_PP_cat(_tag_arr(_tmp259,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL19E;_LL1CD: if(*((int*)_tmp1D5)!= 22)goto _LL1CF;_tmp1FF=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D5)->f1;_LL1CE: s=({struct Cyc_PP_Doc*_tmp25C[2];_tmp25C[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FF);_tmp25C[0]=Cyc_PP_text(({const char*_tmp25D="*";_tag_arr(_tmp25D,
sizeof(char),_get_zero_arr_size(_tmp25D,2));}));Cyc_PP_cat(_tag_arr(_tmp25C,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;_LL1CF: if(*((int*)_tmp1D5)!= 23)goto
_LL1D1;_tmp200=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f1;_tmp201=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f2;_LL1D0: s=({struct Cyc_PP_Doc*
_tmp25E[3];_tmp25E[2]=Cyc_PP_textptr(_tmp201);_tmp25E[1]=Cyc_PP_text(({const char*
_tmp25F=".";_tag_arr(_tmp25F,sizeof(char),_get_zero_arr_size(_tmp25F,2));}));
_tmp25E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp200);Cyc_PP_cat(_tag_arr(_tmp25E,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1D1: if(*((int*)_tmp1D5)!= 24)goto
_LL1D3;_tmp202=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f1;_tmp203=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f2;_LL1D2: s=({struct Cyc_PP_Doc*
_tmp260[3];_tmp260[2]=Cyc_PP_textptr(_tmp203);_tmp260[1]=Cyc_PP_text(({const char*
_tmp261="->";_tag_arr(_tmp261,sizeof(char),_get_zero_arr_size(_tmp261,3));}));
_tmp260[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp202);Cyc_PP_cat(_tag_arr(_tmp260,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL19E;_LL1D3: if(*((int*)_tmp1D5)!= 25)goto
_LL1D5;_tmp204=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f1;_tmp205=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f2;_LL1D4: s=({struct Cyc_PP_Doc*
_tmp262[4];_tmp262[3]=Cyc_PP_text(({const char*_tmp264="]";_tag_arr(_tmp264,
sizeof(char),_get_zero_arr_size(_tmp264,2));}));_tmp262[2]=Cyc_Absynpp_exp2doc(
_tmp205);_tmp262[1]=Cyc_PP_text(({const char*_tmp263="[";_tag_arr(_tmp263,sizeof(
char),_get_zero_arr_size(_tmp263,2));}));_tmp262[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp204);Cyc_PP_cat(_tag_arr(_tmp262,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL19E;_LL1D5: if(*((int*)_tmp1D5)!= 26)goto _LL1D7;_tmp206=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D5)->f1;_LL1D6: s=({struct Cyc_PP_Doc*_tmp265[4];_tmp265[3]=Cyc_PP_text(({
const char*_tmp267=")";_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,2));}));
_tmp265[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp206);_tmp265[1]=Cyc_PP_text(({const
char*_tmp266="(";_tag_arr(_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,2));}));
_tmp265[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp265,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL19E;_LL1D7: if(*((int*)_tmp1D5)!= 27)goto _LL1D9;_tmp207=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D5)->f1;_tmp208=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D5)->f2;_LL1D8:
s=({struct Cyc_PP_Doc*_tmp268[4];_tmp268[3]=Cyc_Absynpp_group_braces(({const char*
_tmp26B=",";_tag_arr(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp208));_tmp268[2]=Cyc_PP_text(({const char*
_tmp26A=")";_tag_arr(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,2));}));
_tmp268[1]=Cyc_Absynpp_typ2doc((*_tmp207).f3);_tmp268[0]=Cyc_PP_text(({const char*
_tmp269="(";_tag_arr(_tmp269,sizeof(char),_get_zero_arr_size(_tmp269,2));}));Cyc_PP_cat(
_tag_arr(_tmp268,sizeof(struct Cyc_PP_Doc*),4));});goto _LL19E;_LL1D9: if(*((int*)
_tmp1D5)!= 28)goto _LL1DB;_tmp209=((struct Cyc_Absyn_Array_e_struct*)_tmp1D5)->f1;
_LL1DA: s=Cyc_Absynpp_group_braces(({const char*_tmp26C=",";_tag_arr(_tmp26C,
sizeof(char),_get_zero_arr_size(_tmp26C,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp209));goto _LL19E;_LL1DB: if(*((int*)_tmp1D5)!= 29)goto _LL1DD;_tmp20A=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f1;_tmp20B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D5)->f2;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f3;
_LL1DC: s=({struct Cyc_PP_Doc*_tmp26D[8];_tmp26D[7]=Cyc_Absynpp_rb();_tmp26D[6]=
Cyc_Absynpp_exp2doc(_tmp20C);_tmp26D[5]=Cyc_PP_text(({const char*_tmp270=" : ";
_tag_arr(_tmp270,sizeof(char),_get_zero_arr_size(_tmp270,4));}));_tmp26D[4]=Cyc_Absynpp_exp2doc(
_tmp20B);_tmp26D[3]=Cyc_PP_text(({const char*_tmp26F=" < ";_tag_arr(_tmp26F,
sizeof(char),_get_zero_arr_size(_tmp26F,4));}));_tmp26D[2]=Cyc_PP_text(*(*
_tmp20A->name).f2);_tmp26D[1]=Cyc_PP_text(({const char*_tmp26E="for ";_tag_arr(
_tmp26E,sizeof(char),_get_zero_arr_size(_tmp26E,5));}));_tmp26D[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp26D,sizeof(struct Cyc_PP_Doc*),8));});goto _LL19E;_LL1DD:
if(*((int*)_tmp1D5)!= 30)goto _LL1DF;_tmp20D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D5)->f1;_tmp20E=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f2;_tmp20F=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f3;_LL1DE: {struct Cyc_List_List*
_tmp271=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20F);s=({struct Cyc_PP_Doc*
_tmp272[2];_tmp272[1]=Cyc_Absynpp_group_braces(({const char*_tmp273=",";_tag_arr(
_tmp273,sizeof(char),_get_zero_arr_size(_tmp273,2));}),_tmp20E != 0?({struct Cyc_List_List*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=Cyc_Absynpp_tps2doc(_tmp20E);
_tmp274->tl=_tmp271;_tmp274;}): _tmp271);_tmp272[0]=Cyc_Absynpp_qvar2doc(_tmp20D);
Cyc_PP_cat(_tag_arr(_tmp272,sizeof(struct Cyc_PP_Doc*),2));});goto _LL19E;}_LL1DF:
if(*((int*)_tmp1D5)!= 31)goto _LL1E1;_tmp210=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D5)->f2;_LL1E0: s=Cyc_Absynpp_group_braces(({const char*_tmp275=",";_tag_arr(
_tmp275,sizeof(char),_get_zero_arr_size(_tmp275,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp210));goto _LL19E;_LL1E1: if(*((int*)_tmp1D5)!= 32)goto _LL1E3;_tmp211=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1D5)->f1;_tmp212=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1D5)->f3;_LL1E2: if(_tmp211 == 0)s=Cyc_Absynpp_qvar2doc(_tmp212->name);else{s=({
struct Cyc_PP_Doc*_tmp276[2];_tmp276[1]=Cyc_PP_egroup(({const char*_tmp277="(";
_tag_arr(_tmp277,sizeof(char),_get_zero_arr_size(_tmp277,2));}),({const char*
_tmp278=")";_tag_arr(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,2));}),({
const char*_tmp279=",";_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp211));_tmp276[0]=Cyc_Absynpp_qvar2doc(
_tmp212->name);Cyc_PP_cat(_tag_arr(_tmp276,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL19E;_LL1E3: if(*((int*)_tmp1D5)!= 33)goto _LL1E5;_tmp213=((struct Cyc_Absyn_Enum_e_struct*)
_tmp1D5)->f1;_LL1E4: s=Cyc_Absynpp_qvar2doc(_tmp213);goto _LL19E;_LL1E5: if(*((int*)
_tmp1D5)!= 34)goto _LL1E7;_tmp214=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D5)->f1;
_LL1E6: s=Cyc_Absynpp_qvar2doc(_tmp214);goto _LL19E;_LL1E7: if(*((int*)_tmp1D5)!= 
35)goto _LL1E9;_tmp215=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D5)->f1;_tmp216=
_tmp215.is_calloc;_tmp217=_tmp215.rgn;_tmp218=_tmp215.elt_type;_tmp219=_tmp215.num_elts;
_LL1E8: if(_tmp216){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmp218)),0);if(_tmp217 == 0)s=({struct Cyc_PP_Doc*_tmp27A[5];_tmp27A[
4]=Cyc_PP_text(({const char*_tmp27D=")";_tag_arr(_tmp27D,sizeof(char),
_get_zero_arr_size(_tmp27D,2));}));_tmp27A[3]=Cyc_Absynpp_exp2doc(st);_tmp27A[2]=
Cyc_PP_text(({const char*_tmp27C=",";_tag_arr(_tmp27C,sizeof(char),
_get_zero_arr_size(_tmp27C,2));}));_tmp27A[1]=Cyc_Absynpp_exp2doc(_tmp219);
_tmp27A[0]=Cyc_PP_text(({const char*_tmp27B="calloc(";_tag_arr(_tmp27B,sizeof(
char),_get_zero_arr_size(_tmp27B,8));}));Cyc_PP_cat(_tag_arr(_tmp27A,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp27E[7];_tmp27E[6]=Cyc_PP_text(({
const char*_tmp282=")";_tag_arr(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,2));}));
_tmp27E[5]=Cyc_Absynpp_exp2doc(st);_tmp27E[4]=Cyc_PP_text(({const char*_tmp281=",";
_tag_arr(_tmp281,sizeof(char),_get_zero_arr_size(_tmp281,2));}));_tmp27E[3]=Cyc_Absynpp_exp2doc(
_tmp219);_tmp27E[2]=Cyc_PP_text(({const char*_tmp280=",";_tag_arr(_tmp280,sizeof(
char),_get_zero_arr_size(_tmp280,2));}));_tmp27E[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp217);_tmp27E[0]=Cyc_PP_text(({const char*_tmp27F="rcalloc(";
_tag_arr(_tmp27F,sizeof(char),_get_zero_arr_size(_tmp27F,9));}));Cyc_PP_cat(
_tag_arr(_tmp27E,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp218 == 0)new_e=_tmp219;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp218,0),_tmp219,0);}if(_tmp217 == 0)s=({struct Cyc_PP_Doc*_tmp283[3];_tmp283[2]=
Cyc_PP_text(({const char*_tmp285=")";_tag_arr(_tmp285,sizeof(char),
_get_zero_arr_size(_tmp285,2));}));_tmp283[1]=Cyc_Absynpp_exp2doc(new_e);_tmp283[
0]=Cyc_PP_text(({const char*_tmp284="malloc(";_tag_arr(_tmp284,sizeof(char),
_get_zero_arr_size(_tmp284,8));}));Cyc_PP_cat(_tag_arr(_tmp283,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp286[5];_tmp286[4]=Cyc_PP_text(({const char*
_tmp289=")";_tag_arr(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,2));}));
_tmp286[3]=Cyc_Absynpp_exp2doc(new_e);_tmp286[2]=Cyc_PP_text(({const char*_tmp288=",";
_tag_arr(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,2));}));_tmp286[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp217);_tmp286[0]=Cyc_PP_text(({const char*_tmp287="rmalloc(";
_tag_arr(_tmp287,sizeof(char),_get_zero_arr_size(_tmp287,9));}));Cyc_PP_cat(
_tag_arr(_tmp286,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL19E;_LL1E9: if(*((int*)
_tmp1D5)!= 36)goto _LL1EB;_tmp21A=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D5)->f1;_tmp21B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D5)->f2;
_LL1EA: s=Cyc_Absynpp_group_braces(({const char*_tmp28A=",";_tag_arr(_tmp28A,
sizeof(char),_get_zero_arr_size(_tmp28A,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp21B));goto _LL19E;_LL1EB: if(*((int*)_tmp1D5)!= 37)goto _LL19E;_tmp21C=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1D5)->f1;_LL1EC: s=({struct Cyc_PP_Doc*_tmp28B[7];
_tmp28B[6]=Cyc_PP_text(({const char*_tmp28D=")";_tag_arr(_tmp28D,sizeof(char),
_get_zero_arr_size(_tmp28D,2));}));_tmp28B[5]=Cyc_Absynpp_rb();_tmp28B[4]=Cyc_PP_blank_doc();
_tmp28B[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp21C));_tmp28B[2]=Cyc_PP_blank_doc();
_tmp28B[1]=Cyc_Absynpp_lb();_tmp28B[0]=Cyc_PP_text(({const char*_tmp28C="(";
_tag_arr(_tmp28C,sizeof(char),_get_zero_arr_size(_tmp28C,2));}));Cyc_PP_cat(
_tag_arr(_tmp28B,sizeof(struct Cyc_PP_Doc*),7));});goto _LL19E;_LL19E:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp28E[3];_tmp28E[2]=Cyc_PP_text(({const char*
_tmp290=")";_tag_arr(_tmp290,sizeof(char),_get_zero_arr_size(_tmp290,2));}));
_tmp28E[1]=s;_tmp28E[0]=Cyc_PP_text(({const char*_tmp28F="(";_tag_arr(_tmp28F,
sizeof(char),_get_zero_arr_size(_tmp28F,2));}));Cyc_PP_cat(_tag_arr(_tmp28E,
sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp291=d;struct Cyc_Absyn_Exp*_tmp292;struct _tagged_arr*_tmp293;
_LL1F7: if(*((int*)_tmp291)!= 0)goto _LL1F9;_tmp292=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp291)->f1;_LL1F8: return({struct Cyc_PP_Doc*_tmp294[3];_tmp294[2]=Cyc_PP_text(({
const char*_tmp296="]";_tag_arr(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,2));}));
_tmp294[1]=Cyc_Absynpp_exp2doc(_tmp292);_tmp294[0]=Cyc_PP_text(({const char*
_tmp295=".[";_tag_arr(_tmp295,sizeof(char),_get_zero_arr_size(_tmp295,3));}));
Cyc_PP_cat(_tag_arr(_tmp294,sizeof(struct Cyc_PP_Doc*),3));});_LL1F9: if(*((int*)
_tmp291)!= 1)goto _LL1F6;_tmp293=((struct Cyc_Absyn_FieldName_struct*)_tmp291)->f1;
_LL1FA: return({struct Cyc_PP_Doc*_tmp297[2];_tmp297[1]=Cyc_PP_textptr(_tmp293);
_tmp297[0]=Cyc_PP_text(({const char*_tmp298=".";_tag_arr(_tmp298,sizeof(char),
_get_zero_arr_size(_tmp298,2));}));Cyc_PP_cat(_tag_arr(_tmp297,sizeof(struct Cyc_PP_Doc*),
2));});_LL1F6:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1
== 0)return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp299[2];
_tmp299[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp299[0]=Cyc_PP_egroup(({const char*
_tmp29A="";_tag_arr(_tmp29A,sizeof(char),_get_zero_arr_size(_tmp29A,1));}),({
const char*_tmp29B="=";_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,2));}),({
const char*_tmp29C="=";_tag_arr(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp299,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp29D="";_tag_arr(
_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,1));}),({const char*_tmp29E="";
_tag_arr(_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,1));}),({const char*
_tmp29F=",";_tag_arr(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp2A0=c;void*_tmp2A1;char
_tmp2A2;void*_tmp2A3;short _tmp2A4;void*_tmp2A5;int _tmp2A6;void*_tmp2A7;int
_tmp2A8;void*_tmp2A9;int _tmp2AA;void*_tmp2AB;long long _tmp2AC;struct _tagged_arr
_tmp2AD;struct _tagged_arr _tmp2AE;_LL1FC: if(_tmp2A0 <= (void*)1  || *((int*)_tmp2A0)
!= 0)goto _LL1FE;_tmp2A1=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp2A0)->f1;
_tmp2A2=((struct Cyc_Absyn_Char_c_struct*)_tmp2A0)->f2;_LL1FD: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp2A2));{void*
_tmp2AF[1]={& _tmp2B1};Cyc_aprintf(({const char*_tmp2B0="'%s'";_tag_arr(_tmp2B0,
sizeof(char),_get_zero_arr_size(_tmp2B0,5));}),_tag_arr(_tmp2AF,sizeof(void*),1));}}));
_LL1FE: if(_tmp2A0 <= (void*)1  || *((int*)_tmp2A0)!= 1)goto _LL200;_tmp2A3=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp2A0)->f1;_tmp2A4=((struct Cyc_Absyn_Short_c_struct*)
_tmp2A0)->f2;_LL1FF: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp2B4;_tmp2B4.tag=1;_tmp2B4.f1=(unsigned int)((int)_tmp2A4);{void*_tmp2B2[1]={&
_tmp2B4};Cyc_aprintf(({const char*_tmp2B3="%d";_tag_arr(_tmp2B3,sizeof(char),
_get_zero_arr_size(_tmp2B3,3));}),_tag_arr(_tmp2B2,sizeof(void*),1));}}));_LL200:
if(_tmp2A0 <= (void*)1  || *((int*)_tmp2A0)!= 2)goto _LL202;_tmp2A5=(void*)((struct
Cyc_Absyn_Int_c_struct*)_tmp2A0)->f1;if((int)_tmp2A5 != 2)goto _LL202;_tmp2A6=((
struct Cyc_Absyn_Int_c_struct*)_tmp2A0)->f2;_LL201: _tmp2A8=_tmp2A6;goto _LL203;
_LL202: if(_tmp2A0 <= (void*)1  || *((int*)_tmp2A0)!= 2)goto _LL204;_tmp2A7=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp2A0)->f1;if((int)_tmp2A7 != 0)goto _LL204;
_tmp2A8=((struct Cyc_Absyn_Int_c_struct*)_tmp2A0)->f2;_LL203: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2B7;_tmp2B7.tag=1;_tmp2B7.f1=(
unsigned int)_tmp2A8;{void*_tmp2B5[1]={& _tmp2B7};Cyc_aprintf(({const char*_tmp2B6="%d";
_tag_arr(_tmp2B6,sizeof(char),_get_zero_arr_size(_tmp2B6,3));}),_tag_arr(_tmp2B5,
sizeof(void*),1));}}));_LL204: if(_tmp2A0 <= (void*)1  || *((int*)_tmp2A0)!= 2)goto
_LL206;_tmp2A9=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2A0)->f1;if((int)
_tmp2A9 != 1)goto _LL206;_tmp2AA=((struct Cyc_Absyn_Int_c_struct*)_tmp2A0)->f2;
_LL205: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2BA;
_tmp2BA.tag=1;_tmp2BA.f1=(unsigned int)_tmp2AA;{void*_tmp2B8[1]={& _tmp2BA};Cyc_aprintf(({
const char*_tmp2B9="%u";_tag_arr(_tmp2B9,sizeof(char),_get_zero_arr_size(_tmp2B9,
3));}),_tag_arr(_tmp2B8,sizeof(void*),1));}}));_LL206: if(_tmp2A0 <= (void*)1  || *((
int*)_tmp2A0)!= 3)goto _LL208;_tmp2AB=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp2A0)->f1;_tmp2AC=((struct Cyc_Absyn_LongLong_c_struct*)_tmp2A0)->f2;_LL207:
return Cyc_PP_text(({const char*_tmp2BB="<<FIX LONG LONG CONSTANT>>";_tag_arr(
_tmp2BB,sizeof(char),_get_zero_arr_size(_tmp2BB,27));}));_LL208: if(_tmp2A0 <= (
void*)1  || *((int*)_tmp2A0)!= 4)goto _LL20A;_tmp2AD=((struct Cyc_Absyn_Float_c_struct*)
_tmp2A0)->f1;_LL209: return Cyc_PP_text(_tmp2AD);_LL20A: if((int)_tmp2A0 != 0)goto
_LL20C;_LL20B: return Cyc_PP_text(({const char*_tmp2BC="NULL";_tag_arr(_tmp2BC,
sizeof(char),_get_zero_arr_size(_tmp2BC,5));}));_LL20C: if(_tmp2A0 <= (void*)1  || *((
int*)_tmp2A0)!= 5)goto _LL1FB;_tmp2AE=((struct Cyc_Absyn_String_c_struct*)_tmp2A0)->f1;
_LL20D: return({struct Cyc_PP_Doc*_tmp2BD[3];_tmp2BD[2]=Cyc_PP_text(({const char*
_tmp2BF="\"";_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,2));}));
_tmp2BD[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2AE));_tmp2BD[0]=Cyc_PP_text(({
const char*_tmp2BE="\"";_tag_arr(_tmp2BE,sizeof(char),_get_zero_arr_size(_tmp2BE,
2));}));Cyc_PP_cat(_tag_arr(_tmp2BD,sizeof(struct Cyc_PP_Doc*),3));});_LL1FB:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0
 || es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C0=
_cycalloc(sizeof(*_tmp2C0));_tmp2C0[0]=({struct Cyc_Core_Failure_struct _tmp2C1;
_tmp2C1.tag=Cyc_Core_Failure;_tmp2C1.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2C2[1]={& _tmp2C4};Cyc_aprintf(({const char*_tmp2C3="Absynpp::primapp2doc Size: %s with bad args";
_tag_arr(_tmp2C3,sizeof(char),_get_zero_arr_size(_tmp2C3,44));}),_tag_arr(
_tmp2C2,sizeof(void*),1));}});_tmp2C1;});_tmp2C0;}));{struct Cyc_PP_Doc*_tmp2C5=
Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*
_tmp2C6[2];_tmp2C6[1]=Cyc_PP_text(({const char*_tmp2C7=".size";_tag_arr(_tmp2C7,
sizeof(char),_get_zero_arr_size(_tmp2C7,6));}));_tmp2C6[0]=_tmp2C5;Cyc_PP_cat(
_tag_arr(_tmp2C6,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C8=_cycalloc(
sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Core_Failure_struct _tmp2C9;_tmp2C9.tag=
Cyc_Core_Failure;_tmp2C9.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2CC;_tmp2CC.tag=0;_tmp2CC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2CA[1]={& _tmp2CC};Cyc_aprintf(({const char*_tmp2CB="Absynpp::primapp2doc: %s with no args";
_tag_arr(_tmp2CB,sizeof(char),_get_zero_arr_size(_tmp2CB,38));}),_tag_arr(
_tmp2CA,sizeof(void*),1));}});_tmp2C9;});_tmp2C8;}));else{if(ds->tl == 0)return({
struct Cyc_PP_Doc*_tmp2CD[3];_tmp2CD[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2CD[1]=Cyc_PP_text(({
const char*_tmp2CE=" ";_tag_arr(_tmp2CE,sizeof(char),_get_zero_arr_size(_tmp2CE,2));}));
_tmp2CD[0]=ps;Cyc_PP_cat(_tag_arr(_tmp2CD,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF[0]=({
struct Cyc_Core_Failure_struct _tmp2D0;_tmp2D0.tag=Cyc_Core_Failure;_tmp2D0.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(ps,72));{void*_tmp2D1[
1]={& _tmp2D3};Cyc_aprintf(({const char*_tmp2D2="Absynpp::primapp2doc: %s with more than 2 args";
_tag_arr(_tmp2D2,sizeof(char),_get_zero_arr_size(_tmp2D2,47));}),_tag_arr(
_tmp2D1,sizeof(void*),1));}});_tmp2D0;});_tmp2CF;}));else{return({struct Cyc_PP_Doc*
_tmp2D4[5];_tmp2D4[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp2D4[3]=Cyc_PP_text(({const char*_tmp2D6=" ";_tag_arr(_tmp2D6,sizeof(char),
_get_zero_arr_size(_tmp2D6,2));}));_tmp2D4[2]=ps;_tmp2D4[1]=Cyc_PP_text(({const
char*_tmp2D5=" ";_tag_arr(_tmp2D5,sizeof(char),_get_zero_arr_size(_tmp2D5,2));}));
_tmp2D4[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp2D4,sizeof(struct
Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){void*
_tmp2D7=p;_LL20F: if((int)_tmp2D7 != 0)goto _LL211;_LL210: return({const char*_tmp2D8="+";
_tag_arr(_tmp2D8,sizeof(char),_get_zero_arr_size(_tmp2D8,2));});_LL211: if((int)
_tmp2D7 != 1)goto _LL213;_LL212: return({const char*_tmp2D9="*";_tag_arr(_tmp2D9,
sizeof(char),_get_zero_arr_size(_tmp2D9,2));});_LL213: if((int)_tmp2D7 != 2)goto
_LL215;_LL214: return({const char*_tmp2DA="-";_tag_arr(_tmp2DA,sizeof(char),
_get_zero_arr_size(_tmp2DA,2));});_LL215: if((int)_tmp2D7 != 3)goto _LL217;_LL216:
return({const char*_tmp2DB="/";_tag_arr(_tmp2DB,sizeof(char),_get_zero_arr_size(
_tmp2DB,2));});_LL217: if((int)_tmp2D7 != 4)goto _LL219;_LL218: return({const char*
_tmp2DC="%";_tag_arr(_tmp2DC,sizeof(char),_get_zero_arr_size(_tmp2DC,2));});
_LL219: if((int)_tmp2D7 != 5)goto _LL21B;_LL21A: return({const char*_tmp2DD="==";
_tag_arr(_tmp2DD,sizeof(char),_get_zero_arr_size(_tmp2DD,3));});_LL21B: if((int)
_tmp2D7 != 6)goto _LL21D;_LL21C: return({const char*_tmp2DE="!=";_tag_arr(_tmp2DE,
sizeof(char),_get_zero_arr_size(_tmp2DE,3));});_LL21D: if((int)_tmp2D7 != 7)goto
_LL21F;_LL21E: return({const char*_tmp2DF=">";_tag_arr(_tmp2DF,sizeof(char),
_get_zero_arr_size(_tmp2DF,2));});_LL21F: if((int)_tmp2D7 != 8)goto _LL221;_LL220:
return({const char*_tmp2E0="<";_tag_arr(_tmp2E0,sizeof(char),_get_zero_arr_size(
_tmp2E0,2));});_LL221: if((int)_tmp2D7 != 9)goto _LL223;_LL222: return({const char*
_tmp2E1=">=";_tag_arr(_tmp2E1,sizeof(char),_get_zero_arr_size(_tmp2E1,3));});
_LL223: if((int)_tmp2D7 != 10)goto _LL225;_LL224: return({const char*_tmp2E2="<=";
_tag_arr(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,3));});_LL225: if((int)
_tmp2D7 != 11)goto _LL227;_LL226: return({const char*_tmp2E3="!";_tag_arr(_tmp2E3,
sizeof(char),_get_zero_arr_size(_tmp2E3,2));});_LL227: if((int)_tmp2D7 != 12)goto
_LL229;_LL228: return({const char*_tmp2E4="~";_tag_arr(_tmp2E4,sizeof(char),
_get_zero_arr_size(_tmp2E4,2));});_LL229: if((int)_tmp2D7 != 13)goto _LL22B;_LL22A:
return({const char*_tmp2E5="&";_tag_arr(_tmp2E5,sizeof(char),_get_zero_arr_size(
_tmp2E5,2));});_LL22B: if((int)_tmp2D7 != 14)goto _LL22D;_LL22C: return({const char*
_tmp2E6="|";_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(_tmp2E6,2));});
_LL22D: if((int)_tmp2D7 != 15)goto _LL22F;_LL22E: return({const char*_tmp2E7="^";
_tag_arr(_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,2));});_LL22F: if((int)
_tmp2D7 != 16)goto _LL231;_LL230: return({const char*_tmp2E8="<<";_tag_arr(_tmp2E8,
sizeof(char),_get_zero_arr_size(_tmp2E8,3));});_LL231: if((int)_tmp2D7 != 17)goto
_LL233;_LL232: return({const char*_tmp2E9=">>";_tag_arr(_tmp2E9,sizeof(char),
_get_zero_arr_size(_tmp2E9,3));});_LL233: if((int)_tmp2D7 != 18)goto _LL235;_LL234:
return({const char*_tmp2EA=">>>";_tag_arr(_tmp2EA,sizeof(char),_get_zero_arr_size(
_tmp2EA,4));});_LL235: if((int)_tmp2D7 != 19)goto _LL20E;_LL236: return({const char*
_tmp2EB="size";_tag_arr(_tmp2EB,sizeof(char),_get_zero_arr_size(_tmp2EB,5));});
_LL20E:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2EC=(void*)s->r;
_LL238: if(_tmp2EC <= (void*)1  || *((int*)_tmp2EC)!= 11)goto _LL23A;_LL239: return 1;
_LL23A:;_LL23B: return 0;_LL237:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp2ED=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2EE;
struct Cyc_Absyn_Stmt*_tmp2EF;struct Cyc_Absyn_Stmt*_tmp2F0;struct Cyc_Absyn_Exp*
_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;struct Cyc_Absyn_Stmt*_tmp2F3;struct Cyc_Absyn_Stmt*
_tmp2F4;struct _tuple2 _tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Stmt*
_tmp2F7;struct _tagged_arr*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct _tuple2
_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FB;struct _tuple2 _tmp2FC;struct Cyc_Absyn_Exp*
_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_List_List*
_tmp300;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Absyn_Decl*
_tmp303;struct Cyc_Absyn_Stmt*_tmp304;struct _tagged_arr*_tmp305;struct Cyc_Absyn_Stmt*
_tmp306;struct Cyc_Absyn_Stmt*_tmp307;struct _tuple2 _tmp308;struct Cyc_Absyn_Exp*
_tmp309;struct Cyc_Absyn_Stmt*_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_Absyn_Tvar*
_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30D;int _tmp30E;struct Cyc_Absyn_Stmt*_tmp30F;
struct Cyc_Absyn_Exp*_tmp310;_LL23D: if((int)_tmp2ED != 0)goto _LL23F;_LL23E: s=Cyc_PP_text(({
const char*_tmp311=";";_tag_arr(_tmp311,sizeof(char),_get_zero_arr_size(_tmp311,2));}));
goto _LL23C;_LL23F: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 0)goto _LL241;
_tmp2EE=((struct Cyc_Absyn_Exp_s_struct*)_tmp2ED)->f1;_LL240: s=({struct Cyc_PP_Doc*
_tmp312[2];_tmp312[1]=Cyc_PP_text(({const char*_tmp313=";";_tag_arr(_tmp313,
sizeof(char),_get_zero_arr_size(_tmp313,2));}));_tmp312[0]=Cyc_Absynpp_exp2doc(
_tmp2EE);Cyc_PP_cat(_tag_arr(_tmp312,sizeof(struct Cyc_PP_Doc*),2));});goto _LL23C;
_LL241: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 1)goto _LL243;_tmp2EF=((struct
Cyc_Absyn_Seq_s_struct*)_tmp2ED)->f1;_tmp2F0=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2ED)->f2;_LL242: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2EF))s=({struct Cyc_PP_Doc*_tmp314[7];_tmp314[6]=Cyc_Absynpp_is_declaration(
_tmp2F0)?({struct Cyc_PP_Doc*_tmp315[5];_tmp315[4]=Cyc_PP_line_doc();_tmp315[3]=
Cyc_Absynpp_rb();_tmp315[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F0));_tmp315[
1]=Cyc_PP_blank_doc();_tmp315[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp315,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2F0);_tmp314[5]=Cyc_PP_line_doc();
_tmp314[4]=Cyc_Absynpp_rb();_tmp314[3]=Cyc_PP_line_doc();_tmp314[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2EF));_tmp314[1]=Cyc_PP_blank_doc();_tmp314[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp314,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp2F0))s=({struct Cyc_PP_Doc*_tmp316[7];_tmp316[6]=Cyc_PP_line_doc();_tmp316[5]=
Cyc_Absynpp_rb();_tmp316[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F0));_tmp316[
3]=Cyc_PP_blank_doc();_tmp316[2]=Cyc_Absynpp_lb();_tmp316[1]=Cyc_PP_line_doc();
_tmp316[0]=Cyc_Absynpp_stmt2doc(_tmp2EF);Cyc_PP_cat(_tag_arr(_tmp316,sizeof(
struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,({const char*_tmp317="";_tag_arr(_tmp317,sizeof(char),
_get_zero_arr_size(_tmp317,1));}),({struct Cyc_List_List*_tmp318=_cycalloc(
sizeof(*_tmp318));_tmp318->hd=_tmp2EF;_tmp318->tl=({struct Cyc_List_List*_tmp319=
_cycalloc(sizeof(*_tmp319));_tmp319->hd=_tmp2F0;_tmp319->tl=0;_tmp319;});_tmp318;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp31A="";_tag_arr(_tmp31A,sizeof(char),_get_zero_arr_size(_tmp31A,1));}),({
struct Cyc_List_List*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->hd=_tmp2EF;
_tmp31B->tl=({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->hd=
_tmp2F0;_tmp31C->tl=0;_tmp31C;});_tmp31B;}));}goto _LL23C;_LL243: if(_tmp2ED <= (
void*)1  || *((int*)_tmp2ED)!= 2)goto _LL245;_tmp2F1=((struct Cyc_Absyn_Return_s_struct*)
_tmp2ED)->f1;_LL244: if(_tmp2F1 == 0)s=Cyc_PP_text(({const char*_tmp31D="return;";
_tag_arr(_tmp31D,sizeof(char),_get_zero_arr_size(_tmp31D,8));}));else{s=({struct
Cyc_PP_Doc*_tmp31E[3];_tmp31E[2]=Cyc_PP_text(({const char*_tmp320=";";_tag_arr(
_tmp320,sizeof(char),_get_zero_arr_size(_tmp320,2));}));_tmp31E[1]=_tmp2F1 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2F1);_tmp31E[0]=
Cyc_PP_text(({const char*_tmp31F="return ";_tag_arr(_tmp31F,sizeof(char),
_get_zero_arr_size(_tmp31F,8));}));Cyc_PP_cat(_tag_arr(_tmp31E,sizeof(struct Cyc_PP_Doc*),
3));});}goto _LL23C;_LL245: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 3)goto
_LL247;_tmp2F2=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2ED)->f1;_tmp2F3=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2ED)->f2;_tmp2F4=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2ED)->f3;_LL246: {int print_else;{void*_tmp321=(void*)_tmp2F4->r;_LL264: if((
int)_tmp321 != 0)goto _LL266;_LL265: print_else=0;goto _LL263;_LL266:;_LL267:
print_else=1;goto _LL263;_LL263:;}s=({struct Cyc_PP_Doc*_tmp322[8];_tmp322[7]=
print_else?({struct Cyc_PP_Doc*_tmp326[6];_tmp326[5]=Cyc_Absynpp_rb();_tmp326[4]=
Cyc_PP_line_doc();_tmp326[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp328[2];_tmp328[
1]=Cyc_Absynpp_stmt2doc(_tmp2F4);_tmp328[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp328,sizeof(struct Cyc_PP_Doc*),2));}));_tmp326[2]=Cyc_Absynpp_lb();
_tmp326[1]=Cyc_PP_text(({const char*_tmp327="else ";_tag_arr(_tmp327,sizeof(char),
_get_zero_arr_size(_tmp327,6));}));_tmp326[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp326,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp322[6]=
Cyc_Absynpp_rb();_tmp322[5]=Cyc_PP_line_doc();_tmp322[4]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp325[2];_tmp325[1]=Cyc_Absynpp_stmt2doc(_tmp2F3);_tmp325[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp325,sizeof(struct Cyc_PP_Doc*),2));}));_tmp322[3]=Cyc_Absynpp_lb();
_tmp322[2]=Cyc_PP_text(({const char*_tmp324=") ";_tag_arr(_tmp324,sizeof(char),
_get_zero_arr_size(_tmp324,3));}));_tmp322[1]=Cyc_Absynpp_exp2doc(_tmp2F2);
_tmp322[0]=Cyc_PP_text(({const char*_tmp323="if (";_tag_arr(_tmp323,sizeof(char),
_get_zero_arr_size(_tmp323,5));}));Cyc_PP_cat(_tag_arr(_tmp322,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL23C;}_LL247: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 4)goto
_LL249;_tmp2F5=((struct Cyc_Absyn_While_s_struct*)_tmp2ED)->f1;_tmp2F6=_tmp2F5.f1;
_tmp2F7=((struct Cyc_Absyn_While_s_struct*)_tmp2ED)->f2;_LL248: s=({struct Cyc_PP_Doc*
_tmp329[7];_tmp329[6]=Cyc_Absynpp_rb();_tmp329[5]=Cyc_PP_line_doc();_tmp329[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp32C[2];_tmp32C[1]=Cyc_Absynpp_stmt2doc(
_tmp2F7);_tmp32C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp32C,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp329[3]=Cyc_Absynpp_lb();_tmp329[2]=Cyc_PP_text(({const
char*_tmp32B=") ";_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,3));}));
_tmp329[1]=Cyc_Absynpp_exp2doc(_tmp2F6);_tmp329[0]=Cyc_PP_text(({const char*
_tmp32A="while (";_tag_arr(_tmp32A,sizeof(char),_get_zero_arr_size(_tmp32A,8));}));
Cyc_PP_cat(_tag_arr(_tmp329,sizeof(struct Cyc_PP_Doc*),7));});goto _LL23C;_LL249:
if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 5)goto _LL24B;_LL24A: s=Cyc_PP_text(({
const char*_tmp32D="break;";_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(
_tmp32D,7));}));goto _LL23C;_LL24B: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 6)
goto _LL24D;_LL24C: s=Cyc_PP_text(({const char*_tmp32E="continue;";_tag_arr(_tmp32E,
sizeof(char),_get_zero_arr_size(_tmp32E,10));}));goto _LL23C;_LL24D: if(_tmp2ED <= (
void*)1  || *((int*)_tmp2ED)!= 7)goto _LL24F;_tmp2F8=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2ED)->f1;_LL24E: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp331;_tmp331.tag=0;_tmp331.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp2F8);{
void*_tmp32F[1]={& _tmp331};Cyc_aprintf(({const char*_tmp330="goto %s;";_tag_arr(
_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,9));}),_tag_arr(_tmp32F,sizeof(
void*),1));}}));goto _LL23C;_LL24F: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 8)
goto _LL251;_tmp2F9=((struct Cyc_Absyn_For_s_struct*)_tmp2ED)->f1;_tmp2FA=((struct
Cyc_Absyn_For_s_struct*)_tmp2ED)->f2;_tmp2FB=_tmp2FA.f1;_tmp2FC=((struct Cyc_Absyn_For_s_struct*)
_tmp2ED)->f3;_tmp2FD=_tmp2FC.f1;_tmp2FE=((struct Cyc_Absyn_For_s_struct*)_tmp2ED)->f4;
_LL250: s=({struct Cyc_PP_Doc*_tmp332[11];_tmp332[10]=Cyc_Absynpp_rb();_tmp332[9]=
Cyc_PP_line_doc();_tmp332[8]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp337[2];_tmp337[
1]=Cyc_Absynpp_stmt2doc(_tmp2FE);_tmp337[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp337,sizeof(struct Cyc_PP_Doc*),2));}));_tmp332[7]=Cyc_Absynpp_lb();
_tmp332[6]=Cyc_PP_text(({const char*_tmp336=") ";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,3));}));_tmp332[5]=Cyc_Absynpp_exp2doc(_tmp2FD);
_tmp332[4]=Cyc_PP_text(({const char*_tmp335="; ";_tag_arr(_tmp335,sizeof(char),
_get_zero_arr_size(_tmp335,3));}));_tmp332[3]=Cyc_Absynpp_exp2doc(_tmp2FB);
_tmp332[2]=Cyc_PP_text(({const char*_tmp334="; ";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,3));}));_tmp332[1]=Cyc_Absynpp_exp2doc(_tmp2F9);
_tmp332[0]=Cyc_PP_text(({const char*_tmp333="for(";_tag_arr(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,5));}));Cyc_PP_cat(_tag_arr(_tmp332,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL23C;_LL251: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 9)goto
_LL253;_tmp2FF=((struct Cyc_Absyn_Switch_s_struct*)_tmp2ED)->f1;_tmp300=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2ED)->f2;_LL252: s=({struct Cyc_PP_Doc*_tmp338[8];
_tmp338[7]=Cyc_Absynpp_rb();_tmp338[6]=Cyc_PP_line_doc();_tmp338[5]=Cyc_Absynpp_switchclauses2doc(
_tmp300);_tmp338[4]=Cyc_PP_line_doc();_tmp338[3]=Cyc_Absynpp_lb();_tmp338[2]=Cyc_PP_text(({
const char*_tmp33A=") ";_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,
3));}));_tmp338[1]=Cyc_Absynpp_exp2doc(_tmp2FF);_tmp338[0]=Cyc_PP_text(({const
char*_tmp339="switch (";_tag_arr(_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,
9));}));Cyc_PP_cat(_tag_arr(_tmp338,sizeof(struct Cyc_PP_Doc*),8));});goto _LL23C;
_LL253: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 10)goto _LL255;_tmp301=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp2ED)->f1;if(_tmp301 != 0)goto _LL255;_LL254:
s=Cyc_PP_text(({const char*_tmp33B="fallthru;";_tag_arr(_tmp33B,sizeof(char),
_get_zero_arr_size(_tmp33B,10));}));goto _LL23C;_LL255: if(_tmp2ED <= (void*)1  || *((
int*)_tmp2ED)!= 10)goto _LL257;_tmp302=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2ED)->f1;_LL256: s=({struct Cyc_PP_Doc*_tmp33C[3];_tmp33C[2]=Cyc_PP_text(({
const char*_tmp33E=");";_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,
3));}));_tmp33C[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp302);_tmp33C[0]=Cyc_PP_text(({
const char*_tmp33D="fallthru(";_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(
_tmp33D,10));}));Cyc_PP_cat(_tag_arr(_tmp33C,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL23C;_LL257: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 11)goto _LL259;
_tmp303=((struct Cyc_Absyn_Decl_s_struct*)_tmp2ED)->f1;_tmp304=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2ED)->f2;_LL258: s=({struct Cyc_PP_Doc*_tmp33F[3];_tmp33F[2]=Cyc_Absynpp_stmt2doc(
_tmp304);_tmp33F[1]=Cyc_PP_line_doc();_tmp33F[0]=Cyc_Absynpp_decl2doc(_tmp303);
Cyc_PP_cat(_tag_arr(_tmp33F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL23C;_LL259:
if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 12)goto _LL25B;_tmp305=((struct Cyc_Absyn_Label_s_struct*)
_tmp2ED)->f1;_tmp306=((struct Cyc_Absyn_Label_s_struct*)_tmp2ED)->f2;_LL25A: if(
Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp306))s=({struct Cyc_PP_Doc*
_tmp340[7];_tmp340[6]=Cyc_Absynpp_rb();_tmp340[5]=Cyc_PP_line_doc();_tmp340[4]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp306));_tmp340[3]=Cyc_PP_line_doc();
_tmp340[2]=Cyc_Absynpp_lb();_tmp340[1]=Cyc_PP_text(({const char*_tmp341=": ";
_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(_tmp341,3));}));_tmp340[0]=Cyc_PP_textptr(
_tmp305);Cyc_PP_cat(_tag_arr(_tmp340,sizeof(struct Cyc_PP_Doc*),7));});else{s=({
struct Cyc_PP_Doc*_tmp342[3];_tmp342[2]=Cyc_Absynpp_stmt2doc(_tmp306);_tmp342[1]=
Cyc_PP_text(({const char*_tmp343=": ";_tag_arr(_tmp343,sizeof(char),
_get_zero_arr_size(_tmp343,3));}));_tmp342[0]=Cyc_PP_textptr(_tmp305);Cyc_PP_cat(
_tag_arr(_tmp342,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL23C;_LL25B: if(_tmp2ED
<= (void*)1  || *((int*)_tmp2ED)!= 13)goto _LL25D;_tmp307=((struct Cyc_Absyn_Do_s_struct*)
_tmp2ED)->f1;_tmp308=((struct Cyc_Absyn_Do_s_struct*)_tmp2ED)->f2;_tmp309=_tmp308.f1;
_LL25C: s=({struct Cyc_PP_Doc*_tmp344[9];_tmp344[8]=Cyc_PP_text(({const char*
_tmp347=");";_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,3));}));
_tmp344[7]=Cyc_Absynpp_exp2doc(_tmp309);_tmp344[6]=Cyc_PP_text(({const char*
_tmp346=" while (";_tag_arr(_tmp346,sizeof(char),_get_zero_arr_size(_tmp346,9));}));
_tmp344[5]=Cyc_Absynpp_rb();_tmp344[4]=Cyc_PP_line_doc();_tmp344[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp307));_tmp344[2]=Cyc_PP_line_doc();_tmp344[1]=Cyc_Absynpp_lb();
_tmp344[0]=Cyc_PP_text(({const char*_tmp345="do ";_tag_arr(_tmp345,sizeof(char),
_get_zero_arr_size(_tmp345,4));}));Cyc_PP_cat(_tag_arr(_tmp344,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL23C;_LL25D: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 14)goto
_LL25F;_tmp30A=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2ED)->f1;_tmp30B=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2ED)->f2;_LL25E: s=({struct Cyc_PP_Doc*
_tmp348[12];_tmp348[11]=Cyc_Absynpp_rb();_tmp348[10]=Cyc_PP_line_doc();_tmp348[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp30B));_tmp348[8]=Cyc_PP_line_doc();
_tmp348[7]=Cyc_Absynpp_lb();_tmp348[6]=Cyc_PP_text(({const char*_tmp34A=" catch ";
_tag_arr(_tmp34A,sizeof(char),_get_zero_arr_size(_tmp34A,8));}));_tmp348[5]=Cyc_Absynpp_rb();
_tmp348[4]=Cyc_PP_line_doc();_tmp348[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp30A));_tmp348[2]=Cyc_PP_line_doc();_tmp348[1]=Cyc_Absynpp_lb();_tmp348[0]=
Cyc_PP_text(({const char*_tmp349="try ";_tag_arr(_tmp349,sizeof(char),
_get_zero_arr_size(_tmp349,5));}));Cyc_PP_cat(_tag_arr(_tmp348,sizeof(struct Cyc_PP_Doc*),
12));});goto _LL23C;_LL25F: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 15)goto
_LL261;_tmp30C=((struct Cyc_Absyn_Region_s_struct*)_tmp2ED)->f1;_tmp30D=((struct
Cyc_Absyn_Region_s_struct*)_tmp2ED)->f2;_tmp30E=((struct Cyc_Absyn_Region_s_struct*)
_tmp2ED)->f3;_tmp30F=((struct Cyc_Absyn_Region_s_struct*)_tmp2ED)->f4;_LL260: s=({
struct Cyc_PP_Doc*_tmp34B[11];_tmp34B[10]=Cyc_Absynpp_rb();_tmp34B[9]=Cyc_PP_line_doc();
_tmp34B[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30F));_tmp34B[7]=Cyc_PP_line_doc();
_tmp34B[6]=Cyc_Absynpp_lb();_tmp34B[5]=Cyc_Absynpp_qvar2doc(_tmp30D->name);
_tmp34B[4]=Cyc_PP_text(({const char*_tmp350=">";_tag_arr(_tmp350,sizeof(char),
_get_zero_arr_size(_tmp350,2));}));_tmp34B[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp30C));_tmp34B[2]=Cyc_PP_text(({const char*_tmp34F="<";_tag_arr(_tmp34F,
sizeof(char),_get_zero_arr_size(_tmp34F,2));}));_tmp34B[1]=_tmp30E?Cyc_PP_text(({
const char*_tmp34D="";_tag_arr(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,1));})):
Cyc_PP_text(({const char*_tmp34E="[resetable]";_tag_arr(_tmp34E,sizeof(char),
_get_zero_arr_size(_tmp34E,12));}));_tmp34B[0]=Cyc_PP_text(({const char*_tmp34C="region";
_tag_arr(_tmp34C,sizeof(char),_get_zero_arr_size(_tmp34C,7));}));Cyc_PP_cat(
_tag_arr(_tmp34B,sizeof(struct Cyc_PP_Doc*),11));});goto _LL23C;_LL261: if(_tmp2ED
<= (void*)1  || *((int*)_tmp2ED)!= 16)goto _LL23C;_tmp310=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2ED)->f1;_LL262: s=({struct Cyc_PP_Doc*_tmp351[3];_tmp351[2]=Cyc_PP_text(({
const char*_tmp353=");";_tag_arr(_tmp353,sizeof(char),_get_zero_arr_size(_tmp353,
3));}));_tmp351[1]=Cyc_Absynpp_exp2doc(_tmp310);_tmp351[0]=Cyc_PP_text(({const
char*_tmp352="reset_region(";_tag_arr(_tmp352,sizeof(char),_get_zero_arr_size(
_tmp352,14));}));Cyc_PP_cat(_tag_arr(_tmp351,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL23C;_LL23C:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp354=(void*)p->r;void*_tmp355;int _tmp356;char
_tmp357;struct _tagged_arr _tmp358;struct Cyc_Absyn_Vardecl*_tmp359;struct Cyc_Absyn_Tvar*
_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Pat*
_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35E;struct _tuple0*_tmp35F;struct _tuple0*
_tmp360;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_AggrInfo _tmp362;void*_tmp363;
struct Cyc_List_List*_tmp364;struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Enumfield*
_tmp366;struct Cyc_Absyn_Enumfield*_tmp367;struct Cyc_Absyn_Tunionfield*_tmp368;
struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Tunionfield*_tmp36A;struct Cyc_List_List*
_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;_LL269: if((int)_tmp354 != 0)goto _LL26B;_LL26A:
s=Cyc_PP_text(({const char*_tmp36D="_";_tag_arr(_tmp36D,sizeof(char),
_get_zero_arr_size(_tmp36D,2));}));goto _LL268;_LL26B: if((int)_tmp354 != 1)goto
_LL26D;_LL26C: s=Cyc_PP_text(({const char*_tmp36E="NULL";_tag_arr(_tmp36E,sizeof(
char),_get_zero_arr_size(_tmp36E,5));}));goto _LL268;_LL26D: if(_tmp354 <= (void*)2
 || *((int*)_tmp354)!= 7)goto _LL26F;_tmp355=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp354)->f1;_tmp356=((struct Cyc_Absyn_Int_p_struct*)_tmp354)->f2;_LL26E: if(
_tmp355 != (void*)((void*)1))s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp371;_tmp371.tag=1;_tmp371.f1=(unsigned int)_tmp356;{void*_tmp36F[1]={&
_tmp371};Cyc_aprintf(({const char*_tmp370="%d";_tag_arr(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,3));}),_tag_arr(_tmp36F,sizeof(void*),1));}}));else{s=
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp374;_tmp374.tag=1;
_tmp374.f1=(unsigned int)_tmp356;{void*_tmp372[1]={& _tmp374};Cyc_aprintf(({const
char*_tmp373="%u";_tag_arr(_tmp373,sizeof(char),_get_zero_arr_size(_tmp373,3));}),
_tag_arr(_tmp372,sizeof(void*),1));}}));}goto _LL268;_LL26F: if(_tmp354 <= (void*)2
 || *((int*)_tmp354)!= 8)goto _LL271;_tmp357=((struct Cyc_Absyn_Char_p_struct*)
_tmp354)->f1;_LL270: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp377;_tmp377.tag=0;_tmp377.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(
_tmp357));{void*_tmp375[1]={& _tmp377};Cyc_aprintf(({const char*_tmp376="'%s'";
_tag_arr(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,5));}),_tag_arr(_tmp375,
sizeof(void*),1));}}));goto _LL268;_LL271: if(_tmp354 <= (void*)2  || *((int*)
_tmp354)!= 9)goto _LL273;_tmp358=((struct Cyc_Absyn_Float_p_struct*)_tmp354)->f1;
_LL272: s=Cyc_PP_text(_tmp358);goto _LL268;_LL273: if(_tmp354 <= (void*)2  || *((int*)
_tmp354)!= 0)goto _LL275;_tmp359=((struct Cyc_Absyn_Var_p_struct*)_tmp354)->f1;
_LL274: s=Cyc_Absynpp_qvar2doc(_tmp359->name);goto _LL268;_LL275: if(_tmp354 <= (
void*)2  || *((int*)_tmp354)!= 2)goto _LL277;_tmp35A=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp354)->f1;_tmp35B=((struct Cyc_Absyn_TagInt_p_struct*)_tmp354)->f2;_LL276: s=({
struct Cyc_PP_Doc*_tmp378[4];_tmp378[3]=Cyc_PP_text(({const char*_tmp37A=">";
_tag_arr(_tmp37A,sizeof(char),_get_zero_arr_size(_tmp37A,2));}));_tmp378[2]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp35A));_tmp378[1]=Cyc_PP_text(({const char*_tmp379="<";
_tag_arr(_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,2));}));_tmp378[0]=Cyc_Absynpp_qvar2doc(
_tmp35B->name);Cyc_PP_cat(_tag_arr(_tmp378,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL268;_LL277: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 3)goto _LL279;_tmp35C=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp354)->f1;_LL278: s=({struct Cyc_PP_Doc*_tmp37B[
4];_tmp37B[3]=Cyc_PP_text(({const char*_tmp37E=")";_tag_arr(_tmp37E,sizeof(char),
_get_zero_arr_size(_tmp37E,2));}));_tmp37B[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,({const char*_tmp37D=",";_tag_arr(_tmp37D,sizeof(char),
_get_zero_arr_size(_tmp37D,2));}),_tmp35C);_tmp37B[1]=Cyc_PP_text(({const char*
_tmp37C="(";_tag_arr(_tmp37C,sizeof(char),_get_zero_arr_size(_tmp37C,2));}));
_tmp37B[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp37B,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL268;_LL279: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 4)goto
_LL27B;_tmp35D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp354)->f1;_LL27A: s=({
struct Cyc_PP_Doc*_tmp37F[2];_tmp37F[1]=Cyc_Absynpp_pat2doc(_tmp35D);_tmp37F[0]=
Cyc_PP_text(({const char*_tmp380="&";_tag_arr(_tmp380,sizeof(char),
_get_zero_arr_size(_tmp380,2));}));Cyc_PP_cat(_tag_arr(_tmp37F,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL268;_LL27B: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 1)goto
_LL27D;_tmp35E=((struct Cyc_Absyn_Reference_p_struct*)_tmp354)->f1;_LL27C: s=({
struct Cyc_PP_Doc*_tmp381[2];_tmp381[1]=Cyc_Absynpp_qvar2doc(_tmp35E->name);
_tmp381[0]=Cyc_PP_text(({const char*_tmp382="*";_tag_arr(_tmp382,sizeof(char),
_get_zero_arr_size(_tmp382,2));}));Cyc_PP_cat(_tag_arr(_tmp381,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL268;_LL27D: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 12)goto
_LL27F;_tmp35F=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp354)->f1;_LL27E: s=Cyc_Absynpp_qvar2doc(
_tmp35F);goto _LL268;_LL27F: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 13)goto
_LL281;_tmp360=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp354)->f1;_tmp361=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp354)->f2;_LL280: s=({struct Cyc_PP_Doc*
_tmp383[2];_tmp383[1]=Cyc_PP_group(({const char*_tmp384="(";_tag_arr(_tmp384,
sizeof(char),_get_zero_arr_size(_tmp384,2));}),({const char*_tmp385=")";_tag_arr(
_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,2));}),({const char*_tmp386=",";
_tag_arr(_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp361));_tmp383[0]=Cyc_Absynpp_qvar2doc(_tmp360);Cyc_PP_cat(
_tag_arr(_tmp383,sizeof(struct Cyc_PP_Doc*),2));});goto _LL268;_LL281: if(_tmp354 <= (
void*)2  || *((int*)_tmp354)!= 5)goto _LL283;_tmp362=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp354)->f1;_tmp363=(void*)_tmp362.aggr_info;_tmp364=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp354)->f2;_tmp365=((struct Cyc_Absyn_Aggr_p_struct*)_tmp354)->f3;_LL282: {
struct _tuple0*_tmp388;struct _tuple3 _tmp387=Cyc_Absyn_aggr_kinded_name(_tmp363);
_tmp388=_tmp387.f2;s=({struct Cyc_PP_Doc*_tmp389[4];_tmp389[3]=Cyc_Absynpp_group_braces(({
const char*_tmp38D=",";_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp365));_tmp389[2]=Cyc_PP_egroup(({const
char*_tmp38A="[";_tag_arr(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,2));}),({
const char*_tmp38B="]";_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,2));}),({
const char*_tmp38C=",";_tag_arr(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp364)));_tmp389[1]=Cyc_Absynpp_lb();_tmp389[0]=Cyc_Absynpp_qvar2doc(_tmp388);
Cyc_PP_cat(_tag_arr(_tmp389,sizeof(struct Cyc_PP_Doc*),4));});goto _LL268;}_LL283:
if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 10)goto _LL285;_tmp366=((struct Cyc_Absyn_Enum_p_struct*)
_tmp354)->f2;_LL284: _tmp367=_tmp366;goto _LL286;_LL285: if(_tmp354 <= (void*)2  || *((
int*)_tmp354)!= 11)goto _LL287;_tmp367=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp354)->f2;_LL286: s=Cyc_Absynpp_qvar2doc(_tmp367->name);goto _LL268;_LL287: if(
_tmp354 <= (void*)2  || *((int*)_tmp354)!= 6)goto _LL289;_tmp368=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp354)->f2;_tmp369=((struct Cyc_Absyn_Tunion_p_struct*)_tmp354)->f3;if(_tmp369
!= 0)goto _LL289;_LL288: s=Cyc_Absynpp_qvar2doc(_tmp368->name);goto _LL268;_LL289:
if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 6)goto _LL28B;_tmp36A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp354)->f2;_tmp36B=((struct Cyc_Absyn_Tunion_p_struct*)_tmp354)->f3;_LL28A: s=({
struct Cyc_PP_Doc*_tmp38E[2];_tmp38E[1]=Cyc_PP_egroup(({const char*_tmp38F="(";
_tag_arr(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,2));}),({const char*
_tmp390=")";_tag_arr(_tmp390,sizeof(char),_get_zero_arr_size(_tmp390,2));}),({
const char*_tmp391=",";_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp36B));_tmp38E[0]=Cyc_Absynpp_qvar2doc(
_tmp36A->name);Cyc_PP_cat(_tag_arr(_tmp38E,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL268;_LL28B: if(_tmp354 <= (void*)2  || *((int*)_tmp354)!= 14)goto _LL268;_tmp36C=((
struct Cyc_Absyn_Exp_p_struct*)_tmp354)->f1;_LL28C: s=Cyc_Absynpp_exp2doc(_tmp36C);
goto _LL268;_LL268:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*
dp){return({struct Cyc_PP_Doc*_tmp392[2];_tmp392[1]=Cyc_Absynpp_pat2doc((*dp).f2);
_tmp392[0]=Cyc_PP_egroup(({const char*_tmp393="";_tag_arr(_tmp393,sizeof(char),
_get_zero_arr_size(_tmp393,1));}),({const char*_tmp394="=";_tag_arr(_tmp394,
sizeof(char),_get_zero_arr_size(_tmp394,2));}),({const char*_tmp395="=";_tag_arr(
_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp392,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0  && (void*)(c->pattern)->r == (void*)((void*)0))return({struct Cyc_PP_Doc*
_tmp396[2];_tmp396[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp398[2];_tmp398[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp398[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp398,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp396[0]=Cyc_PP_text(({const char*_tmp397="default: ";
_tag_arr(_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,10));}));Cyc_PP_cat(
_tag_arr(_tmp396,sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp399[4];_tmp399[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp39C[2];_tmp39C[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp39C[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp39C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp399[2]=Cyc_PP_text(({
const char*_tmp39B=": ";_tag_arr(_tmp39B,sizeof(char),_get_zero_arr_size(_tmp39B,
3));}));_tmp399[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp399[0]=Cyc_PP_text(({
const char*_tmp39A="case ";_tag_arr(_tmp39A,sizeof(char),_get_zero_arr_size(
_tmp39A,6));}));Cyc_PP_cat(_tag_arr(_tmp399,sizeof(struct Cyc_PP_Doc*),4));});
else{return({struct Cyc_PP_Doc*_tmp39D[6];_tmp39D[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp3A1[2];_tmp3A1[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3A1[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp3A1,sizeof(struct Cyc_PP_Doc*),2));}));_tmp39D[4]=Cyc_PP_text(({
const char*_tmp3A0=": ";_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,
3));}));_tmp39D[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp39D[2]=Cyc_PP_text(({const char*_tmp39F=" && ";_tag_arr(_tmp39F,sizeof(char),
_get_zero_arr_size(_tmp39F,5));}));_tmp39D[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp39D[0]=Cyc_PP_text(({const char*_tmp39E="case ";_tag_arr(_tmp39E,sizeof(char),
_get_zero_arr_size(_tmp39E,6));}));Cyc_PP_cat(_tag_arr(_tmp39D,sizeof(struct Cyc_PP_Doc*),
6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({
const char*_tmp3A2="";_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(_tmp3A2,1));}),
cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(
f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*
_tmp3A3[3];_tmp3A3[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp3A3[1]=Cyc_PP_text(({const char*_tmp3A4=" = ";_tag_arr(_tmp3A4,sizeof(char),
_get_zero_arr_size(_tmp3A4,4));}));_tmp3A3[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp3A3,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,({const char*_tmp3A5=",";_tag_arr(_tmp3A5,sizeof(char),
_get_zero_arr_size(_tmp3A5,2));}),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*
Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct
Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({const char*_tmp3A6=",";_tag_arr(_tmp3A6,
sizeof(char),_get_zero_arr_size(_tmp3A6,2));}),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp3A8;void*_tmp3A9;struct
_tuple0*_tmp3AA;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AC;struct Cyc_Absyn_Exp*
_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3A7=vd;_tmp3A8=*
_tmp3A7;_tmp3A9=(void*)_tmp3A8.sc;_tmp3AA=_tmp3A8.name;_tmp3AB=_tmp3A8.tq;
_tmp3AC=(void*)_tmp3A8.type;_tmp3AD=_tmp3A8.initializer;_tmp3AE=_tmp3A8.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3AA);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3AE);struct Cyc_PP_Doc*
beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=attsdoc;else{void*_tmp3AF=Cyc_Tcutil_compress(
_tmp3AC);struct Cyc_Absyn_FnInfo _tmp3B0;struct Cyc_List_List*_tmp3B1;_LL28E: if(
_tmp3AF <= (void*)3  || *((int*)_tmp3AF)!= 8)goto _LL290;_tmp3B0=((struct Cyc_Absyn_FnType_struct*)
_tmp3AF)->f1;_tmp3B1=_tmp3B0.attributes;_LL28F: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3B1);goto _LL28D;_LL290:;_LL291: beforenamedoc=Cyc_PP_nil_doc();goto _LL28D;
_LL28D:;}s=({struct Cyc_PP_Doc*_tmp3B2[5];_tmp3B2[4]=Cyc_PP_text(({const char*
_tmp3B8=";";_tag_arr(_tmp3B8,sizeof(char),_get_zero_arr_size(_tmp3B8,2));}));
_tmp3B2[3]=_tmp3AD == 0?Cyc_PP_text(({const char*_tmp3B5="";_tag_arr(_tmp3B5,
sizeof(char),_get_zero_arr_size(_tmp3B5,1));})):({struct Cyc_PP_Doc*_tmp3B6[2];
_tmp3B6[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3AD);_tmp3B6[0]=Cyc_PP_text(({
const char*_tmp3B7=" = ";_tag_arr(_tmp3B7,sizeof(char),_get_zero_arr_size(_tmp3B7,
4));}));Cyc_PP_cat(_tag_arr(_tmp3B6,sizeof(struct Cyc_PP_Doc*),2));});_tmp3B2[2]=
Cyc_Absynpp_tqtd2doc(_tmp3AB,_tmp3AC,({struct Cyc_Core_Opt*_tmp3B3=_cycalloc(
sizeof(*_tmp3B3));_tmp3B3->v=({struct Cyc_PP_Doc*_tmp3B4[2];_tmp3B4[1]=sn;_tmp3B4[
0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp3B4,sizeof(struct Cyc_PP_Doc*),2));});
_tmp3B3;}));_tmp3B2[1]=Cyc_Absynpp_scope2doc(_tmp3A9);_tmp3B2[0]=Cyc_Absynpp_to_VC?
attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(_tmp3B2,sizeof(struct Cyc_PP_Doc*),5));});
return s;}}struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*_tmp3BA;
struct _tuple11 _tmp3B9=*x;_tmp3BA=_tmp3B9.f2;return Cyc_Absynpp_qvar2doc(_tmp3BA);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp3BB=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3BC;struct Cyc_Absyn_Aggrdecl*
_tmp3BD;struct Cyc_Absyn_Vardecl*_tmp3BE;struct Cyc_Absyn_Tuniondecl*_tmp3BF;
struct Cyc_Absyn_Tuniondecl _tmp3C0;void*_tmp3C1;struct _tuple0*_tmp3C2;struct Cyc_List_List*
_tmp3C3;struct Cyc_Core_Opt*_tmp3C4;int _tmp3C5;struct Cyc_Absyn_Pat*_tmp3C6;struct
Cyc_Absyn_Exp*_tmp3C7;struct Cyc_List_List*_tmp3C8;struct Cyc_Absyn_Enumdecl*
_tmp3C9;struct Cyc_Absyn_Enumdecl _tmp3CA;void*_tmp3CB;struct _tuple0*_tmp3CC;
struct Cyc_Core_Opt*_tmp3CD;struct Cyc_Absyn_Typedefdecl*_tmp3CE;struct _tagged_arr*
_tmp3CF;struct Cyc_List_List*_tmp3D0;struct _tuple0*_tmp3D1;struct Cyc_List_List*
_tmp3D2;struct Cyc_List_List*_tmp3D3;struct Cyc_List_List*_tmp3D4;struct Cyc_List_List*
_tmp3D5;_LL293: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 1)goto _LL295;_tmp3BC=((
struct Cyc_Absyn_Fn_d_struct*)_tmp3BB)->f1;_LL294: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*
_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE[0]=({struct Cyc_Absyn_FnType_struct
_tmp3DF;_tmp3DF.tag=8;_tmp3DF.f1=({struct Cyc_Absyn_FnInfo _tmp3E0;_tmp3E0.tvars=
_tmp3BC->tvs;_tmp3E0.effect=_tmp3BC->effect;_tmp3E0.ret_typ=(void*)((void*)
_tmp3BC->ret_type);_tmp3E0.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(
struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp3BC->args);_tmp3E0.c_varargs=_tmp3BC->c_varargs;_tmp3E0.cyc_varargs=_tmp3BC->cyc_varargs;
_tmp3E0.rgn_po=_tmp3BC->rgn_po;_tmp3E0.attributes=0;_tmp3E0;});_tmp3DF;});
_tmp3DE;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3BC->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3BC->is_inline){if(Cyc_Absynpp_to_VC)inlinedoc=
Cyc_PP_text(({const char*_tmp3D6="__inline ";_tag_arr(_tmp3D6,sizeof(char),
_get_zero_arr_size(_tmp3D6,10));}));else{inlinedoc=Cyc_PP_text(({const char*
_tmp3D7="inline ";_tag_arr(_tmp3D7,sizeof(char),_get_zero_arr_size(_tmp3D7,8));}));}}
else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3BC->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp3BC->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3BC->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(
0),t,({struct Cyc_Core_Opt*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->v=({
struct Cyc_PP_Doc*_tmp3DD[2];_tmp3DD[1]=namedoc;_tmp3DD[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp3DD,sizeof(struct Cyc_PP_Doc*),2));});_tmp3DC;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3DA[5];_tmp3DA[4]=Cyc_Absynpp_rb();_tmp3DA[3]=Cyc_PP_line_doc();
_tmp3DA[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3DB[2];_tmp3DB[1]=Cyc_Absynpp_stmt2doc(
_tmp3BC->body);_tmp3DB[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3DB,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3DA[1]=Cyc_Absynpp_lb();_tmp3DA[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_arr(_tmp3DA,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3D8[4];_tmp3D8[3]=bodydoc;_tmp3D8[2]=tqtddoc;_tmp3D8[1]=scopedoc;_tmp3D8[0]=
inlinedoc;Cyc_PP_cat(_tag_arr(_tmp3D8,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp3D9[2];_tmp3D9[1]=s;_tmp3D9[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp3D9,sizeof(struct Cyc_PP_Doc*),2));});goto _LL292;}}_LL295: if(_tmp3BB
<= (void*)2  || *((int*)_tmp3BB)!= 4)goto _LL297;_tmp3BD=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp3BB)->f1;_LL296: if(_tmp3BD->impl == 0)s=({struct Cyc_PP_Doc*_tmp3E1[5];_tmp3E1[
4]=Cyc_PP_text(({const char*_tmp3E2=";";_tag_arr(_tmp3E2,sizeof(char),
_get_zero_arr_size(_tmp3E2,2));}));_tmp3E1[3]=Cyc_Absynpp_ktvars2doc(_tmp3BD->tvs);
_tmp3E1[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3BD->name);_tmp3E1[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3BD->kind);_tmp3E1[0]=Cyc_Absynpp_scope2doc((void*)_tmp3BD->sc);Cyc_PP_cat(
_tag_arr(_tmp3E1,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp3E3[12];_tmp3E3[11]=Cyc_PP_text(({const char*_tmp3E7=";";_tag_arr(_tmp3E7,
sizeof(char),_get_zero_arr_size(_tmp3E7,2));}));_tmp3E3[10]=Cyc_Absynpp_atts2doc(
_tmp3BD->attributes);_tmp3E3[9]=Cyc_Absynpp_rb();_tmp3E3[8]=Cyc_PP_line_doc();
_tmp3E3[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3E6[2];_tmp3E6[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BD->impl))->fields);_tmp3E6[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3E6,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3E3[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BD->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp3E4[2];_tmp3E4[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3BD->impl))->rgn_po);_tmp3E4[0]=
Cyc_PP_text(({const char*_tmp3E5=":";_tag_arr(_tmp3E5,sizeof(char),
_get_zero_arr_size(_tmp3E5,2));}));Cyc_PP_cat(_tag_arr(_tmp3E4,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3E3[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3BD->impl))->exist_vars);_tmp3E3[4]=Cyc_Absynpp_lb();_tmp3E3[3]=
Cyc_PP_blank_doc();_tmp3E3[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3BD->name);_tmp3E3[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3BD->kind);_tmp3E3[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3BD->sc);Cyc_PP_cat(_tag_arr(_tmp3E3,sizeof(struct Cyc_PP_Doc*),12));});}
goto _LL292;_LL297: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 0)goto _LL299;
_tmp3BE=((struct Cyc_Absyn_Var_d_struct*)_tmp3BB)->f1;_LL298: s=Cyc_Absynpp_vardecl2doc(
_tmp3BE);goto _LL292;_LL299: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 5)goto
_LL29B;_tmp3BF=((struct Cyc_Absyn_Tunion_d_struct*)_tmp3BB)->f1;_tmp3C0=*_tmp3BF;
_tmp3C1=(void*)_tmp3C0.sc;_tmp3C2=_tmp3C0.name;_tmp3C3=_tmp3C0.tvs;_tmp3C4=
_tmp3C0.fields;_tmp3C5=_tmp3C0.is_xtunion;_LL29A: if(_tmp3C4 == 0)s=({struct Cyc_PP_Doc*
_tmp3E8[5];_tmp3E8[4]=Cyc_PP_text(({const char*_tmp3EB=";";_tag_arr(_tmp3EB,
sizeof(char),_get_zero_arr_size(_tmp3EB,2));}));_tmp3E8[3]=Cyc_Absynpp_ktvars2doc(
_tmp3C3);_tmp3E8[2]=_tmp3C5?Cyc_Absynpp_qvar2bolddoc(_tmp3C2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3C2);_tmp3E8[1]=_tmp3C5?Cyc_PP_text(({const char*_tmp3E9="xtunion ";_tag_arr(
_tmp3E9,sizeof(char),_get_zero_arr_size(_tmp3E9,9));})): Cyc_PP_text(({const char*
_tmp3EA="tunion ";_tag_arr(_tmp3EA,sizeof(char),_get_zero_arr_size(_tmp3EA,8));}));
_tmp3E8[0]=Cyc_Absynpp_scope2doc(_tmp3C1);Cyc_PP_cat(_tag_arr(_tmp3E8,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp3EC[10];_tmp3EC[9]=Cyc_PP_text(({
const char*_tmp3F0=";";_tag_arr(_tmp3F0,sizeof(char),_get_zero_arr_size(_tmp3F0,2));}));
_tmp3EC[8]=Cyc_Absynpp_rb();_tmp3EC[7]=Cyc_PP_line_doc();_tmp3EC[6]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3EF[2];_tmp3EF[1]=Cyc_Absynpp_tunionfields2doc((struct
Cyc_List_List*)_tmp3C4->v);_tmp3EF[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(
_tmp3EF,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3EC[5]=Cyc_Absynpp_lb();_tmp3EC[4]=
Cyc_PP_blank_doc();_tmp3EC[3]=Cyc_Absynpp_ktvars2doc(_tmp3C3);_tmp3EC[2]=_tmp3C5?
Cyc_Absynpp_qvar2bolddoc(_tmp3C2): Cyc_Absynpp_typedef_name2bolddoc(_tmp3C2);
_tmp3EC[1]=_tmp3C5?Cyc_PP_text(({const char*_tmp3ED="xtunion ";_tag_arr(_tmp3ED,
sizeof(char),_get_zero_arr_size(_tmp3ED,9));})): Cyc_PP_text(({const char*_tmp3EE="tunion ";
_tag_arr(_tmp3EE,sizeof(char),_get_zero_arr_size(_tmp3EE,8));}));_tmp3EC[0]=Cyc_Absynpp_scope2doc(
_tmp3C1);Cyc_PP_cat(_tag_arr(_tmp3EC,sizeof(struct Cyc_PP_Doc*),10));});}goto
_LL292;_LL29B: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 2)goto _LL29D;_tmp3C6=((
struct Cyc_Absyn_Let_d_struct*)_tmp3BB)->f1;_tmp3C7=((struct Cyc_Absyn_Let_d_struct*)
_tmp3BB)->f3;_LL29C: s=({struct Cyc_PP_Doc*_tmp3F1[5];_tmp3F1[4]=Cyc_PP_text(({
const char*_tmp3F4=";";_tag_arr(_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,2));}));
_tmp3F1[3]=Cyc_Absynpp_exp2doc(_tmp3C7);_tmp3F1[2]=Cyc_PP_text(({const char*
_tmp3F3=" = ";_tag_arr(_tmp3F3,sizeof(char),_get_zero_arr_size(_tmp3F3,4));}));
_tmp3F1[1]=Cyc_Absynpp_pat2doc(_tmp3C6);_tmp3F1[0]=Cyc_PP_text(({const char*
_tmp3F2="let ";_tag_arr(_tmp3F2,sizeof(char),_get_zero_arr_size(_tmp3F2,5));}));
Cyc_PP_cat(_tag_arr(_tmp3F1,sizeof(struct Cyc_PP_Doc*),5));});goto _LL292;_LL29D:
if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 3)goto _LL29F;_tmp3C8=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3BB)->f1;_LL29E: s=({struct Cyc_PP_Doc*_tmp3F5[3];_tmp3F5[2]=Cyc_PP_text(({
const char*_tmp3F7=";";_tag_arr(_tmp3F7,sizeof(char),_get_zero_arr_size(_tmp3F7,2));}));
_tmp3F5[1]=Cyc_Absynpp_ids2doc(_tmp3C8);_tmp3F5[0]=Cyc_PP_text(({const char*
_tmp3F6="let ";_tag_arr(_tmp3F6,sizeof(char),_get_zero_arr_size(_tmp3F6,5));}));
Cyc_PP_cat(_tag_arr(_tmp3F5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL292;_LL29F:
if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 6)goto _LL2A1;_tmp3C9=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3BB)->f1;_tmp3CA=*_tmp3C9;_tmp3CB=(void*)_tmp3CA.sc;_tmp3CC=_tmp3CA.name;
_tmp3CD=_tmp3CA.fields;_LL2A0: if(_tmp3CD == 0)s=({struct Cyc_PP_Doc*_tmp3F8[4];
_tmp3F8[3]=Cyc_PP_text(({const char*_tmp3FA=";";_tag_arr(_tmp3FA,sizeof(char),
_get_zero_arr_size(_tmp3FA,2));}));_tmp3F8[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3CC);_tmp3F8[1]=Cyc_PP_text(({const char*_tmp3F9="enum ";_tag_arr(_tmp3F9,
sizeof(char),_get_zero_arr_size(_tmp3F9,6));}));_tmp3F8[0]=Cyc_Absynpp_scope2doc(
_tmp3CB);Cyc_PP_cat(_tag_arr(_tmp3F8,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp3FB[9];_tmp3FB[8]=Cyc_PP_text(({const char*_tmp3FE=";";
_tag_arr(_tmp3FE,sizeof(char),_get_zero_arr_size(_tmp3FE,2));}));_tmp3FB[7]=Cyc_Absynpp_rb();
_tmp3FB[6]=Cyc_PP_line_doc();_tmp3FB[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3FD[
2];_tmp3FD[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3CD->v);
_tmp3FD[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3FD,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp3FB[4]=Cyc_Absynpp_lb();_tmp3FB[3]=Cyc_PP_blank_doc();_tmp3FB[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3CC);_tmp3FB[1]=Cyc_PP_text(({const char*_tmp3FC="enum ";_tag_arr(_tmp3FC,
sizeof(char),_get_zero_arr_size(_tmp3FC,6));}));_tmp3FB[0]=Cyc_Absynpp_scope2doc(
_tmp3CB);Cyc_PP_cat(_tag_arr(_tmp3FB,sizeof(struct Cyc_PP_Doc*),9));});}goto
_LL292;_LL2A1: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 7)goto _LL2A3;_tmp3CE=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp3BB)->f1;_LL2A2: {void*t;if(_tmp3CE->defn
!= 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3CE->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp3CE->kind,0);}s=({struct Cyc_PP_Doc*_tmp3FF[4];_tmp3FF[3]=Cyc_PP_text(({const
char*_tmp403=";";_tag_arr(_tmp403,sizeof(char),_get_zero_arr_size(_tmp403,2));}));
_tmp3FF[2]=Cyc_Absynpp_atts2doc(_tmp3CE->atts);_tmp3FF[1]=Cyc_Absynpp_tqtd2doc(
_tmp3CE->tq,t,({struct Cyc_Core_Opt*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->v=({
struct Cyc_PP_Doc*_tmp402[2];_tmp402[1]=Cyc_Absynpp_tvars2doc(_tmp3CE->tvs);
_tmp402[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3CE->name);Cyc_PP_cat(_tag_arr(
_tmp402,sizeof(struct Cyc_PP_Doc*),2));});_tmp401;}));_tmp3FF[0]=Cyc_PP_text(({
const char*_tmp400="typedef ";_tag_arr(_tmp400,sizeof(char),_get_zero_arr_size(
_tmp400,9));}));Cyc_PP_cat(_tag_arr(_tmp3FF,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL292;}_LL2A3: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 8)goto _LL2A5;
_tmp3CF=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3BB)->f1;_tmp3D0=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3BB)->f2;_LL2A4: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(
_tmp3CF);s=({struct Cyc_PP_Doc*_tmp404[8];_tmp404[7]=Cyc_Absynpp_rb();_tmp404[6]=
Cyc_PP_line_doc();_tmp404[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp406="";_tag_arr(_tmp406,sizeof(char),
_get_zero_arr_size(_tmp406,1));}),_tmp3D0);_tmp404[4]=Cyc_PP_line_doc();_tmp404[
3]=Cyc_Absynpp_lb();_tmp404[2]=Cyc_PP_blank_doc();_tmp404[1]=Cyc_PP_textptr(
_tmp3CF);_tmp404[0]=Cyc_PP_text(({const char*_tmp405="namespace ";_tag_arr(
_tmp405,sizeof(char),_get_zero_arr_size(_tmp405,11));}));Cyc_PP_cat(_tag_arr(
_tmp404,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL292;_LL2A5: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 9)goto _LL2A7;
_tmp3D1=((struct Cyc_Absyn_Using_d_struct*)_tmp3BB)->f1;_tmp3D2=((struct Cyc_Absyn_Using_d_struct*)
_tmp3BB)->f2;_LL2A6: if(Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*
_tmp407[8];_tmp407[7]=Cyc_Absynpp_rb();_tmp407[6]=Cyc_PP_line_doc();_tmp407[5]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp409="";_tag_arr(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,1));}),
_tmp3D2);_tmp407[4]=Cyc_PP_line_doc();_tmp407[3]=Cyc_Absynpp_lb();_tmp407[2]=Cyc_PP_blank_doc();
_tmp407[1]=Cyc_Absynpp_qvar2doc(_tmp3D1);_tmp407[0]=Cyc_PP_text(({const char*
_tmp408="using ";_tag_arr(_tmp408,sizeof(char),_get_zero_arr_size(_tmp408,7));}));
Cyc_PP_cat(_tag_arr(_tmp407,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp40A[11];_tmp40A[10]=Cyc_PP_text(({const char*_tmp40F=" */";_tag_arr(_tmp40F,
sizeof(char),_get_zero_arr_size(_tmp40F,4));}));_tmp40A[9]=Cyc_Absynpp_rb();
_tmp40A[8]=Cyc_PP_text(({const char*_tmp40E="/* ";_tag_arr(_tmp40E,sizeof(char),
_get_zero_arr_size(_tmp40E,4));}));_tmp40A[7]=Cyc_PP_line_doc();_tmp40A[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp40D="";_tag_arr(_tmp40D,sizeof(char),_get_zero_arr_size(_tmp40D,1));}),
_tmp3D2);_tmp40A[5]=Cyc_PP_line_doc();_tmp40A[4]=Cyc_PP_text(({const char*_tmp40C=" */";
_tag_arr(_tmp40C,sizeof(char),_get_zero_arr_size(_tmp40C,4));}));_tmp40A[3]=Cyc_Absynpp_lb();
_tmp40A[2]=Cyc_PP_blank_doc();_tmp40A[1]=Cyc_Absynpp_qvar2doc(_tmp3D1);_tmp40A[0]=
Cyc_PP_text(({const char*_tmp40B="/* using ";_tag_arr(_tmp40B,sizeof(char),
_get_zero_arr_size(_tmp40B,10));}));Cyc_PP_cat(_tag_arr(_tmp40A,sizeof(struct Cyc_PP_Doc*),
11));});}goto _LL292;_LL2A7: if(_tmp3BB <= (void*)2  || *((int*)_tmp3BB)!= 10)goto
_LL2A9;_tmp3D3=((struct Cyc_Absyn_ExternC_d_struct*)_tmp3BB)->f1;_LL2A8: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp410[6];_tmp410[5]=Cyc_Absynpp_rb();_tmp410[4]=Cyc_PP_line_doc();
_tmp410[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp412="";_tag_arr(_tmp412,sizeof(char),_get_zero_arr_size(_tmp412,1));}),
_tmp3D3);_tmp410[2]=Cyc_PP_line_doc();_tmp410[1]=Cyc_Absynpp_lb();_tmp410[0]=Cyc_PP_text(({
const char*_tmp411="extern \"C\" ";_tag_arr(_tmp411,sizeof(char),
_get_zero_arr_size(_tmp411,12));}));Cyc_PP_cat(_tag_arr(_tmp410,sizeof(struct Cyc_PP_Doc*),
6));});else{s=({struct Cyc_PP_Doc*_tmp413[9];_tmp413[8]=Cyc_PP_text(({const char*
_tmp418=" */";_tag_arr(_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,4));}));
_tmp413[7]=Cyc_Absynpp_rb();_tmp413[6]=Cyc_PP_text(({const char*_tmp417="/* ";
_tag_arr(_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,4));}));_tmp413[5]=Cyc_PP_line_doc();
_tmp413[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp416="";_tag_arr(_tmp416,sizeof(char),_get_zero_arr_size(_tmp416,1));}),
_tmp3D3);_tmp413[3]=Cyc_PP_line_doc();_tmp413[2]=Cyc_PP_text(({const char*_tmp415=" */";
_tag_arr(_tmp415,sizeof(char),_get_zero_arr_size(_tmp415,4));}));_tmp413[1]=Cyc_Absynpp_lb();
_tmp413[0]=Cyc_PP_text(({const char*_tmp414="/* extern \"C\" ";_tag_arr(_tmp414,
sizeof(char),_get_zero_arr_size(_tmp414,15));}));Cyc_PP_cat(_tag_arr(_tmp413,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL292;_LL2A9: if(_tmp3BB <= (void*)2  || *((
int*)_tmp3BB)!= 11)goto _LL2AB;_tmp3D4=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp3BB)->f1;_tmp3D5=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3BB)->f2;
_LL2AA: if(Cyc_Absynpp_print_externC_stmts){struct Cyc_PP_Doc*exs_doc;if(_tmp3D5 != 
0)exs_doc=({struct Cyc_PP_Doc*_tmp419[7];_tmp419[6]=Cyc_Absynpp_rb();_tmp419[5]=
Cyc_PP_line_doc();_tmp419[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({
const char*_tmp41B=",";_tag_arr(_tmp41B,sizeof(char),_get_zero_arr_size(_tmp41B,2));}),
_tmp3D5);_tmp419[3]=Cyc_PP_line_doc();_tmp419[2]=Cyc_Absynpp_lb();_tmp419[1]=Cyc_PP_text(({
const char*_tmp41A=" export ";_tag_arr(_tmp41A,sizeof(char),_get_zero_arr_size(
_tmp41A,9));}));_tmp419[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_arr(_tmp419,sizeof(
struct Cyc_PP_Doc*),7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*
_tmp41C[6];_tmp41C[5]=exs_doc;_tmp41C[4]=Cyc_PP_line_doc();_tmp41C[3]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr
sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*
_tmp41E="";_tag_arr(_tmp41E,sizeof(char),_get_zero_arr_size(_tmp41E,1));}),
_tmp3D4);_tmp41C[2]=Cyc_PP_line_doc();_tmp41C[1]=Cyc_Absynpp_lb();_tmp41C[0]=Cyc_PP_text(({
const char*_tmp41D="extern \"C include\" ";_tag_arr(_tmp41D,sizeof(char),
_get_zero_arr_size(_tmp41D,20));}));Cyc_PP_cat(_tag_arr(_tmp41C,sizeof(struct Cyc_PP_Doc*),
6));});}else{s=({struct Cyc_PP_Doc*_tmp41F[9];_tmp41F[8]=Cyc_PP_text(({const char*
_tmp424=" */";_tag_arr(_tmp424,sizeof(char),_get_zero_arr_size(_tmp424,4));}));
_tmp41F[7]=Cyc_Absynpp_rb();_tmp41F[6]=Cyc_PP_text(({const char*_tmp423="/* ";
_tag_arr(_tmp423,sizeof(char),_get_zero_arr_size(_tmp423,4));}));_tmp41F[5]=Cyc_PP_line_doc();
_tmp41F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp422="";_tag_arr(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,1));}),
_tmp3D4);_tmp41F[3]=Cyc_PP_line_doc();_tmp41F[2]=Cyc_PP_text(({const char*_tmp421=" */";
_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,4));}));_tmp41F[1]=Cyc_Absynpp_lb();
_tmp41F[0]=Cyc_PP_text(({const char*_tmp420="/* extern \"C include\" ";_tag_arr(
_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,23));}));Cyc_PP_cat(_tag_arr(
_tmp41F,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL292;_LL2AB: if((int)_tmp3BB != 0)
goto _LL2AD;_LL2AC: s=({struct Cyc_PP_Doc*_tmp425[2];_tmp425[1]=Cyc_Absynpp_lb();
_tmp425[0]=Cyc_PP_text(({const char*_tmp426="__cyclone_port_on__;";_tag_arr(
_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,21));}));Cyc_PP_cat(_tag_arr(
_tmp425,sizeof(struct Cyc_PP_Doc*),2));});goto _LL292;_LL2AD: if((int)_tmp3BB != 1)
goto _LL292;_LL2AE: s=({struct Cyc_PP_Doc*_tmp427[2];_tmp427[1]=Cyc_Absynpp_lb();
_tmp427[0]=Cyc_PP_text(({const char*_tmp428="__cyclone_port_off__;";_tag_arr(
_tmp428,sizeof(char),_get_zero_arr_size(_tmp428,22));}));Cyc_PP_cat(_tag_arr(
_tmp427,sizeof(struct Cyc_PP_Doc*),2));});goto _LL292;_LL292:;}return s;}int Cyc_Absynpp_print_scopes=
1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(!Cyc_Absynpp_print_scopes)
return Cyc_PP_nil_doc();{void*_tmp429=sc;_LL2B0: if((int)_tmp429 != 0)goto _LL2B2;
_LL2B1: return Cyc_PP_text(({const char*_tmp42A="static ";_tag_arr(_tmp42A,sizeof(
char),_get_zero_arr_size(_tmp42A,8));}));_LL2B2: if((int)_tmp429 != 2)goto _LL2B4;
_LL2B3: return Cyc_PP_nil_doc();_LL2B4: if((int)_tmp429 != 3)goto _LL2B6;_LL2B5:
return Cyc_PP_text(({const char*_tmp42B="extern ";_tag_arr(_tmp42B,sizeof(char),
_get_zero_arr_size(_tmp42B,8));}));_LL2B6: if((int)_tmp429 != 4)goto _LL2B8;_LL2B7:
return Cyc_PP_text(({const char*_tmp42C="extern \"C\" ";_tag_arr(_tmp42C,sizeof(
char),_get_zero_arr_size(_tmp42C,12));}));_LL2B8: if((int)_tmp429 != 1)goto _LL2BA;
_LL2B9: return Cyc_PP_text(({const char*_tmp42D="abstract ";_tag_arr(_tmp42D,
sizeof(char),_get_zero_arr_size(_tmp42D,10));}));_LL2BA: if((int)_tmp429 != 5)goto
_LL2AF;_LL2BB: return Cyc_PP_text(({const char*_tmp42E="register ";_tag_arr(_tmp42E,
sizeof(char),_get_zero_arr_size(_tmp42E,10));}));_LL2AF:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp42F=t;struct Cyc_Absyn_Tvar*_tmp430;struct Cyc_List_List*_tmp431;
_LL2BD: if(_tmp42F <= (void*)3  || *((int*)_tmp42F)!= 1)goto _LL2BF;_tmp430=((struct
Cyc_Absyn_VarType_struct*)_tmp42F)->f1;_LL2BE: return Cyc_Tcutil_is_temp_tvar(
_tmp430);_LL2BF: if(_tmp42F <= (void*)3  || *((int*)_tmp42F)!= 20)goto _LL2C1;
_tmp431=((struct Cyc_Absyn_JoinEff_struct*)_tmp42F)->f1;_LL2C0: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp431);_LL2C1:;_LL2C2: return 0;_LL2BC:;}
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp432=t;void**_tmp433;void*
_tmp434;_LL2C4: if(_tmp432 <= (void*)3  || *((int*)_tmp432)!= 11)goto _LL2C6;_LL2C5:
return 1;_LL2C6: if(_tmp432 <= (void*)3  || *((int*)_tmp432)!= 13)goto _LL2C8;_LL2C7:
return 1;_LL2C8: if(_tmp432 <= (void*)3  || *((int*)_tmp432)!= 16)goto _LL2CA;_tmp433=((
struct Cyc_Absyn_TypedefType_struct*)_tmp432)->f4;if(_tmp433 == 0)goto _LL2CA;
_tmp434=*_tmp433;_LL2C9: return Cyc_Absynpp_is_anon_aggrtype(_tmp434);_LL2CA:;
_LL2CB: return 0;_LL2C3:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){struct _tuple5
_tmp436=({struct _tuple5 _tmp435;_tmp435.f1=(void*)tms->hd;_tmp435.f2=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp435;});void*_tmp437;void*
_tmp438;_LL2CD: _tmp437=_tmp436.f1;if(*((int*)_tmp437)!= 2)goto _LL2CF;_tmp438=
_tmp436.f2;if(*((int*)_tmp438)!= 3)goto _LL2CF;_LL2CE: return({struct Cyc_List_List*
_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->hd=(void*)((void*)tms->hd);_tmp439->tl=({
struct Cyc_List_List*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);_tmp43A->tl=Cyc_Absynpp_bubble_attributes(
atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp43A;});_tmp439;});
_LL2CF:;_LL2D0: return({struct Cyc_List_List*_tmp43B=_cycalloc(sizeof(*_tmp43B));
_tmp43B->hd=(void*)atts;_tmp43B->tl=tms;_tmp43B;});_LL2CC:;}else{return({struct
Cyc_List_List*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->hd=(void*)atts;
_tmp43C->tl=tms;_tmp43C;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp43D=t;struct Cyc_Absyn_ArrayInfo _tmp43E;void*_tmp43F;struct
Cyc_Absyn_Tqual _tmp440;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Conref*
_tmp442;struct Cyc_Absyn_PtrInfo _tmp443;void*_tmp444;struct Cyc_Absyn_Tqual _tmp445;
struct Cyc_Absyn_PtrAtts _tmp446;struct Cyc_Absyn_FnInfo _tmp447;struct Cyc_List_List*
_tmp448;struct Cyc_Core_Opt*_tmp449;void*_tmp44A;struct Cyc_List_List*_tmp44B;int
_tmp44C;struct Cyc_Absyn_VarargInfo*_tmp44D;struct Cyc_List_List*_tmp44E;struct Cyc_List_List*
_tmp44F;struct Cyc_Core_Opt*_tmp450;struct Cyc_Core_Opt*_tmp451;int _tmp452;struct
_tuple0*_tmp453;struct Cyc_List_List*_tmp454;void**_tmp455;_LL2D2: if(_tmp43D <= (
void*)3  || *((int*)_tmp43D)!= 7)goto _LL2D4;_tmp43E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp43D)->f1;_tmp43F=(void*)_tmp43E.elt_type;_tmp440=_tmp43E.tq;_tmp441=_tmp43E.num_elts;
_tmp442=_tmp43E.zero_term;_LL2D3: {struct Cyc_Absyn_Tqual _tmp457;void*_tmp458;
struct Cyc_List_List*_tmp459;struct _tuple7 _tmp456=Cyc_Absynpp_to_tms(_tmp440,
_tmp43F);_tmp457=_tmp456.f1;_tmp458=_tmp456.f2;_tmp459=_tmp456.f3;{void*tm;if(
_tmp441 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp45A=_cycalloc(
sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp45B;_tmp45B.tag=
0;_tmp45B.f1=_tmp442;_tmp45B;});_tmp45A;});else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp45D;_tmp45D.tag=1;_tmp45D.f1=(struct Cyc_Absyn_Exp*)_tmp441;_tmp45D.f2=
_tmp442;_tmp45D;});_tmp45C;});}return({struct _tuple7 _tmp45E;_tmp45E.f1=_tmp457;
_tmp45E.f2=_tmp458;_tmp45E.f3=({struct Cyc_List_List*_tmp45F=_cycalloc(sizeof(*
_tmp45F));_tmp45F->hd=(void*)tm;_tmp45F->tl=_tmp459;_tmp45F;});_tmp45E;});}}
_LL2D4: if(_tmp43D <= (void*)3  || *((int*)_tmp43D)!= 4)goto _LL2D6;_tmp443=((struct
Cyc_Absyn_PointerType_struct*)_tmp43D)->f1;_tmp444=(void*)_tmp443.elt_typ;
_tmp445=_tmp443.elt_tq;_tmp446=_tmp443.ptr_atts;_LL2D5: {struct Cyc_Absyn_Tqual
_tmp461;void*_tmp462;struct Cyc_List_List*_tmp463;struct _tuple7 _tmp460=Cyc_Absynpp_to_tms(
_tmp445,_tmp444);_tmp461=_tmp460.f1;_tmp462=_tmp460.f2;_tmp463=_tmp460.f3;
_tmp463=({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp465=_cycalloc(sizeof(*
_tmp465));_tmp465[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp466;_tmp466.tag=2;
_tmp466.f1=_tmp446;_tmp466.f2=tq;_tmp466;});_tmp465;}));_tmp464->tl=_tmp463;
_tmp464;});return({struct _tuple7 _tmp467;_tmp467.f1=_tmp461;_tmp467.f2=_tmp462;
_tmp467.f3=_tmp463;_tmp467;});}_LL2D6: if(_tmp43D <= (void*)3  || *((int*)_tmp43D)
!= 8)goto _LL2D8;_tmp447=((struct Cyc_Absyn_FnType_struct*)_tmp43D)->f1;_tmp448=
_tmp447.tvars;_tmp449=_tmp447.effect;_tmp44A=(void*)_tmp447.ret_typ;_tmp44B=
_tmp447.args;_tmp44C=_tmp447.c_varargs;_tmp44D=_tmp447.cyc_varargs;_tmp44E=
_tmp447.rgn_po;_tmp44F=_tmp447.attributes;_LL2D7: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp449 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp449->v)){
_tmp449=0;_tmp448=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp448);}{struct Cyc_Absyn_Tqual _tmp469;void*_tmp46A;struct Cyc_List_List*_tmp46B;
struct _tuple7 _tmp468=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(0),_tmp44A);
_tmp469=_tmp468.f1;_tmp46A=_tmp468.f2;_tmp46B=_tmp468.f3;{struct Cyc_List_List*
tms=_tmp46B;if(_tmp44F != 0  && !Cyc_Absynpp_to_VC)tms=Cyc_Absynpp_bubble_attributes((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));
_tmp46C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp46D;_tmp46D.tag=5;_tmp46D.f1=
0;_tmp46D.f2=_tmp44F;_tmp46D;});_tmp46C;}),tms);tms=({struct Cyc_List_List*
_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp470;_tmp470.tag=3;_tmp470.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp472;_tmp472.tag=1;_tmp472.f1=_tmp44B;_tmp472.f2=_tmp44C;_tmp472.f3=_tmp44D;
_tmp472.f4=_tmp449;_tmp472.f5=_tmp44E;_tmp472;});_tmp471;}));_tmp470;});_tmp46F;}));
_tmp46E->tl=tms;_tmp46E;});if(Cyc_Absynpp_to_VC)for(0;_tmp44F != 0;_tmp44F=
_tmp44F->tl){void*_tmp473=(void*)_tmp44F->hd;_LL2DF: if((int)_tmp473 != 0)goto
_LL2E1;_LL2E0: goto _LL2E2;_LL2E1: if((int)_tmp473 != 1)goto _LL2E3;_LL2E2: goto _LL2E4;
_LL2E3: if((int)_tmp473 != 2)goto _LL2E5;_LL2E4: tms=({struct Cyc_List_List*_tmp474=
_cycalloc(sizeof(*_tmp474));_tmp474->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp476;_tmp476.tag=5;_tmp476.f1=0;_tmp476.f2=({struct Cyc_List_List*_tmp477=
_cycalloc(sizeof(*_tmp477));_tmp477->hd=(void*)((void*)_tmp44F->hd);_tmp477->tl=
0;_tmp477;});_tmp476;});_tmp475;}));_tmp474->tl=tms;_tmp474;});goto AfterAtts;
_LL2E5:;_LL2E6: goto _LL2DE;_LL2DE:;}AfterAtts: if(_tmp448 != 0)tms=({struct Cyc_List_List*
_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp47A;_tmp47A.tag=4;_tmp47A.f1=_tmp448;_tmp47A.f2=0;_tmp47A.f3=1;_tmp47A;});
_tmp479;}));_tmp478->tl=tms;_tmp478;});return({struct _tuple7 _tmp47B;_tmp47B.f1=
_tmp469;_tmp47B.f2=_tmp46A;_tmp47B.f3=tms;_tmp47B;});}}_LL2D8: if(_tmp43D <= (void*)
3  || *((int*)_tmp43D)!= 0)goto _LL2DA;_tmp450=((struct Cyc_Absyn_Evar_struct*)
_tmp43D)->f1;_tmp451=((struct Cyc_Absyn_Evar_struct*)_tmp43D)->f2;_tmp452=((
struct Cyc_Absyn_Evar_struct*)_tmp43D)->f3;_LL2D9: if(_tmp451 == 0)return({struct
_tuple7 _tmp47C;_tmp47C.f1=tq;_tmp47C.f2=t;_tmp47C.f3=0;_tmp47C;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp451->v);}_LL2DA: if(_tmp43D <= (void*)3  || *((int*)_tmp43D)!= 16)
goto _LL2DC;_tmp453=((struct Cyc_Absyn_TypedefType_struct*)_tmp43D)->f1;_tmp454=((
struct Cyc_Absyn_TypedefType_struct*)_tmp43D)->f2;_tmp455=((struct Cyc_Absyn_TypedefType_struct*)
_tmp43D)->f4;_LL2DB: if((_tmp455 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp455))return({struct _tuple7 _tmp47D;_tmp47D.f1=tq;_tmp47D.f2=t;_tmp47D.f3=0;
_tmp47D;});else{return Cyc_Absynpp_to_tms(tq,*_tmp455);}_LL2DC:;_LL2DD: return({
struct _tuple7 _tmp47E;_tmp47E.f1=tq;_tmp47E.f2=t;_tmp47E.f3=0;_tmp47E;});_LL2D1:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp47F=t;struct Cyc_Core_Opt*
_tmp480;struct Cyc_Core_Opt _tmp481;void*_tmp482;struct Cyc_Absyn_PtrInfo _tmp483;
void*_tmp484;_LL2E8: if(_tmp47F <= (void*)3  || *((int*)_tmp47F)!= 0)goto _LL2EA;
_tmp480=((struct Cyc_Absyn_Evar_struct*)_tmp47F)->f2;if(_tmp480 == 0)goto _LL2EA;
_tmp481=*_tmp480;_tmp482=(void*)_tmp481.v;_LL2E9: return Cyc_Absynpp_is_char_ptr(
_tmp482);_LL2EA: if(_tmp47F <= (void*)3  || *((int*)_tmp47F)!= 4)goto _LL2EC;_tmp483=((
struct Cyc_Absyn_PointerType_struct*)_tmp47F)->f1;_tmp484=(void*)_tmp483.elt_typ;
_LL2EB: L: {void*_tmp485=_tmp484;struct Cyc_Core_Opt*_tmp486;struct Cyc_Core_Opt
_tmp487;void*_tmp488;void**_tmp489;void*_tmp48A;void*_tmp48B;_LL2EF: if(_tmp485 <= (
void*)3  || *((int*)_tmp485)!= 0)goto _LL2F1;_tmp486=((struct Cyc_Absyn_Evar_struct*)
_tmp485)->f2;if(_tmp486 == 0)goto _LL2F1;_tmp487=*_tmp486;_tmp488=(void*)_tmp487.v;
_LL2F0: _tmp484=_tmp488;goto L;_LL2F1: if(_tmp485 <= (void*)3  || *((int*)_tmp485)!= 
16)goto _LL2F3;_tmp489=((struct Cyc_Absyn_TypedefType_struct*)_tmp485)->f4;if(
_tmp489 == 0)goto _LL2F3;_tmp48A=*_tmp489;_LL2F2: _tmp484=_tmp48A;goto L;_LL2F3: if(
_tmp485 <= (void*)3  || *((int*)_tmp485)!= 5)goto _LL2F5;_tmp48B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp485)->f2;if((int)_tmp48B != 0)goto _LL2F5;_LL2F4: return 1;_LL2F5:;_LL2F6: return
0;_LL2EE:;}_LL2EC:;_LL2ED: return 0;_LL2E7:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual
_tmp48D;void*_tmp48E;struct Cyc_List_List*_tmp48F;struct _tuple7 _tmp48C=Cyc_Absynpp_to_tms(
tq,typ);_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;_tmp48F=_tmp48C.f3;_tmp48F=Cyc_List_imp_rev(
_tmp48F);if(_tmp48F == 0  && dopt == 0)return({struct Cyc_PP_Doc*_tmp490[2];_tmp490[
1]=Cyc_Absynpp_ntyp2doc(_tmp48E);_tmp490[0]=Cyc_Absynpp_tqual2doc(_tmp48D);Cyc_PP_cat(
_tag_arr(_tmp490,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp491[4];_tmp491[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp48F);_tmp491[2]=Cyc_PP_text(({
const char*_tmp492=" ";_tag_arr(_tmp492,sizeof(char),_get_zero_arr_size(_tmp492,2));}));
_tmp491[1]=Cyc_Absynpp_ntyp2doc(_tmp48E);_tmp491[0]=Cyc_Absynpp_tqual2doc(
_tmp48D);Cyc_PP_cat(_tag_arr(_tmp491,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){if(f->width != 
0)return({struct Cyc_PP_Doc*_tmp493[5];_tmp493[4]=Cyc_PP_text(({const char*_tmp496=";";
_tag_arr(_tmp496,sizeof(char),_get_zero_arr_size(_tmp496,2));}));_tmp493[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp493[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp493[1]=Cyc_PP_text(({const char*_tmp495=":";_tag_arr(_tmp495,
sizeof(char),_get_zero_arr_size(_tmp495,2));}));_tmp493[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp494=_cycalloc(sizeof(*_tmp494));
_tmp494->v=Cyc_PP_textptr(f->name);_tmp494;}));Cyc_PP_cat(_tag_arr(_tmp493,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp497[3];
_tmp497[2]=Cyc_PP_text(({const char*_tmp499=";";_tag_arr(_tmp499,sizeof(char),
_get_zero_arr_size(_tmp499,2));}));_tmp497[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp497[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->v=Cyc_PP_textptr(f->name);_tmp498;}));
Cyc_PP_cat(_tag_arr(_tmp497,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp49A="";
_tag_arr(_tmp49A,sizeof(char),_get_zero_arr_size(_tmp49A,1));}),fields);}struct
Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*f){return({
struct Cyc_PP_Doc*_tmp49B[3];_tmp49B[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs);_tmp49B[1]=Cyc_Absynpp_typedef_name2doc(f->name);_tmp49B[0]=Cyc_Absynpp_scope2doc((
void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp49B,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Tunionfield*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({
const char*_tmp49C=",";_tag_arr(_tmp49C,sizeof(char),_get_zero_arr_size(_tmp49C,2));}),
fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp49D[0]={};Cyc_fprintf(f,({const char*_tmp49E="\n";
_tag_arr(_tmp49E,sizeof(char),_get_zero_arr_size(_tmp49E,2));}),_tag_arr(_tmp49D,
sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp49F="";_tag_arr(
_tmp49F,sizeof(char),_get_zero_arr_size(_tmp49F,1));}),((struct Cyc_List_List*(*)(
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
