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
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*nullable;struct
Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;
struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*
tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct _tuple0*
name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo
f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int tag;
struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct Cyc_Absyn_Conref*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*f3;}
;struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual
f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
int tag;void*f1;char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
int tag;void*f1;long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int f1;}
;struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;
struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{int
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
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
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct
Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
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
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_typ2cstring(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
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
_get_zero_arr_size(_tmp106,3));}));_LLAC: if((int)_tmp105 != 3)goto _LLAE;_LLAD:
return Cyc_PP_text(({const char*_tmp107="`U";_tag_arr(_tmp107,sizeof(char),
_get_zero_arr_size(_tmp107,3));}));_LLAE:;_LLAF: return Cyc_Absynpp_ntyp2doc(t);
_LLA9:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp108=Cyc_Tcutil_compress(t);void*_tmp109;struct Cyc_List_List*
_tmp10A;_LLB1: if(_tmp108 <= (void*)4  || *((int*)_tmp108)!= 19)goto _LLB3;_tmp109=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp108)->f1;_LLB2:*rgions=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=Cyc_Absynpp_rgn2doc(_tmp109);
_tmp10B->tl=*rgions;_tmp10B;});goto _LLB0;_LLB3: if(_tmp108 <= (void*)4  || *((int*)
_tmp108)!= 20)goto _LLB5;_tmp10A=((struct Cyc_Absyn_JoinEff_struct*)_tmp108)->f1;
_LLB4: for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){Cyc_Absynpp_effects2docs(rgions,
effects,(void*)_tmp10A->hd);}goto _LLB0;_LLB5:;_LLB6:*effects=({struct Cyc_List_List*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=Cyc_Absynpp_typ2doc(t);_tmp10C->tl=*
effects;_tmp10C;});goto _LLB0;_LLB0:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp10D="";
_tag_arr(_tmp10D,sizeof(char),_get_zero_arr_size(_tmp10D,1));}),({const char*
_tmp10E="";_tag_arr(_tmp10E,sizeof(char),_get_zero_arr_size(_tmp10E,1));}),({
const char*_tmp10F="+";_tag_arr(_tmp10F,sizeof(char),_get_zero_arr_size(_tmp10F,2));}),
effects);else{struct Cyc_PP_Doc*_tmp110=Cyc_Absynpp_group_braces(({const char*
_tmp115=",";_tag_arr(_tmp115,sizeof(char),_get_zero_arr_size(_tmp115,2));}),
rgions);return Cyc_PP_group(({const char*_tmp111="";_tag_arr(_tmp111,sizeof(char),
_get_zero_arr_size(_tmp111,1));}),({const char*_tmp112="";_tag_arr(_tmp112,
sizeof(char),_get_zero_arr_size(_tmp112,1));}),({const char*_tmp113="+";_tag_arr(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,2));}),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({
struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=_tmp110;
_tmp114->tl=0;_tmp114;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k){
void*_tmp116=k;_LLB8: if((int)_tmp116 != 0)goto _LLBA;_LLB9: return Cyc_PP_text(({
const char*_tmp117="struct ";_tag_arr(_tmp117,sizeof(char),_get_zero_arr_size(
_tmp117,8));}));_LLBA: if((int)_tmp116 != 1)goto _LLB7;_LLBB: return Cyc_PP_text(({
const char*_tmp118="union ";_tag_arr(_tmp118,sizeof(char),_get_zero_arr_size(
_tmp118,7));}));_LLB7:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*
s;{void*_tmp119=t;struct Cyc_Core_Opt*_tmp11A;struct Cyc_Core_Opt*_tmp11B;int
_tmp11C;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Absyn_Tvar*_tmp11E;struct Cyc_Absyn_TunionInfo
_tmp11F;void*_tmp120;struct Cyc_List_List*_tmp121;void*_tmp122;struct Cyc_Absyn_TunionFieldInfo
_tmp123;void*_tmp124;struct Cyc_List_List*_tmp125;void*_tmp126;void*_tmp127;int
_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_AggrInfo _tmp12A;void*_tmp12B;
struct Cyc_List_List*_tmp12C;void*_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*
_tmp12F;struct _tuple0*_tmp130;void*_tmp131;struct _tuple0*_tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_Typedefdecl*_tmp134;void*_tmp135;int _tmp136;void*_tmp137;
void*_tmp138;_LLBD: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 7)goto _LLBF;_LLBE:
return Cyc_PP_text(({const char*_tmp139="[[[array]]]";_tag_arr(_tmp139,sizeof(char),
_get_zero_arr_size(_tmp139,12));}));_LLBF: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 8)goto _LLC1;_LLC0: return Cyc_PP_nil_doc();_LLC1: if(_tmp119 <= (void*)4
 || *((int*)_tmp119)!= 4)goto _LLC3;_LLC2: return Cyc_PP_nil_doc();_LLC3: if((int)
_tmp119 != 0)goto _LLC5;_LLC4: s=Cyc_PP_text(({const char*_tmp13A="void";_tag_arr(
_tmp13A,sizeof(char),_get_zero_arr_size(_tmp13A,5));}));goto _LLBC;_LLC5: if(
_tmp119 <= (void*)4  || *((int*)_tmp119)!= 0)goto _LLC7;_tmp11A=((struct Cyc_Absyn_Evar_struct*)
_tmp119)->f1;_tmp11B=((struct Cyc_Absyn_Evar_struct*)_tmp119)->f2;_tmp11C=((
struct Cyc_Absyn_Evar_struct*)_tmp119)->f3;_tmp11D=((struct Cyc_Absyn_Evar_struct*)
_tmp119)->f4;_LLC6: if(_tmp11B != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11B->v);
else{void*_tmp13B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp11A))->v;_LLF2:
if((int)_tmp13B != 3)goto _LLF4;_LLF3: s=({struct Cyc_PP_Doc*_tmp13C[2];_tmp13C[1]=
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp140;_tmp140.tag=1;
_tmp140.f1=(unsigned int)_tmp11C;{void*_tmp13E[1]={& _tmp140};Cyc_aprintf(({const
char*_tmp13F="%d";_tag_arr(_tmp13F,sizeof(char),_get_zero_arr_size(_tmp13F,3));}),
_tag_arr(_tmp13E,sizeof(void*),1));}}));_tmp13C[0]=Cyc_PP_text(({const char*
_tmp13D="`r";_tag_arr(_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,3));}));
Cyc_PP_cat(_tag_arr(_tmp13C,sizeof(struct Cyc_PP_Doc*),2));});goto _LLF1;_LLF4:;
_LLF5: s=({struct Cyc_PP_Doc*_tmp141[6];_tmp141[5]=_tmp11A == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((void*)_tmp11A->v);_tmp141[4]=Cyc_PP_text(({const char*
_tmp148=")::";_tag_arr(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,4));}));
_tmp141[3]=(!Cyc_Absynpp_print_full_evars  || _tmp11D == 0)?Cyc_PP_text(({const
char*_tmp147="";_tag_arr(_tmp147,sizeof(char),_get_zero_arr_size(_tmp147,1));})):
Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp11D->v);_tmp141[2]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp146;_tmp146.tag=1;_tmp146.f1=(
unsigned int)_tmp11C;{void*_tmp144[1]={& _tmp146};Cyc_aprintf(({const char*_tmp145="%d";
_tag_arr(_tmp145,sizeof(char),_get_zero_arr_size(_tmp145,3));}),_tag_arr(_tmp144,
sizeof(void*),1));}}));_tmp141[1]=Cyc_PP_text(({const char*_tmp143="(";_tag_arr(
_tmp143,sizeof(char),_get_zero_arr_size(_tmp143,2));}));_tmp141[0]=Cyc_PP_text(({
const char*_tmp142="%";_tag_arr(_tmp142,sizeof(char),_get_zero_arr_size(_tmp142,2));}));
Cyc_PP_cat(_tag_arr(_tmp141,sizeof(struct Cyc_PP_Doc*),6));});_LLF1:;}goto _LLBC;
_LLC7: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 1)goto _LLC9;_tmp11E=((struct
Cyc_Absyn_VarType_struct*)_tmp119)->f1;_LLC8: s=Cyc_PP_textptr(_tmp11E->name);if(
Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp149[3];_tmp149[2]=Cyc_Absynpp_kindbound2doc((
void*)_tmp11E->kind);_tmp149[1]=Cyc_PP_text(({const char*_tmp14A="::";_tag_arr(
_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,3));}));_tmp149[0]=s;Cyc_PP_cat(
_tag_arr(_tmp149,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp11E))s=({struct Cyc_PP_Doc*_tmp14B[3];_tmp14B[2]=
Cyc_PP_text(({const char*_tmp14D=" */";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,4));}));_tmp14B[1]=s;_tmp14B[0]=Cyc_PP_text(({const
char*_tmp14C="_ /* ";_tag_arr(_tmp14C,sizeof(char),_get_zero_arr_size(_tmp14C,6));}));
Cyc_PP_cat(_tag_arr(_tmp14B,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;_LLC9: if(
_tmp119 <= (void*)4  || *((int*)_tmp119)!= 2)goto _LLCB;_tmp11F=((struct Cyc_Absyn_TunionType_struct*)
_tmp119)->f1;_tmp120=(void*)_tmp11F.tunion_info;_tmp121=_tmp11F.targs;_tmp122=(
void*)_tmp11F.rgn;_LLCA:{void*_tmp14E=_tmp120;struct Cyc_Absyn_UnknownTunionInfo
_tmp14F;struct _tuple0*_tmp150;int _tmp151;struct Cyc_Absyn_Tuniondecl**_tmp152;
struct Cyc_Absyn_Tuniondecl*_tmp153;struct Cyc_Absyn_Tuniondecl _tmp154;struct
_tuple0*_tmp155;int _tmp156;_LLF7: if(*((int*)_tmp14E)!= 0)goto _LLF9;_tmp14F=((
struct Cyc_Absyn_UnknownTunion_struct*)_tmp14E)->f1;_tmp150=_tmp14F.name;_tmp151=
_tmp14F.is_xtunion;_LLF8: _tmp155=_tmp150;_tmp156=_tmp151;goto _LLFA;_LLF9: if(*((
int*)_tmp14E)!= 1)goto _LLF6;_tmp152=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp14E)->f1;_tmp153=*_tmp152;_tmp154=*_tmp153;_tmp155=_tmp154.name;_tmp156=
_tmp154.is_xtunion;_LLFA: {struct Cyc_PP_Doc*_tmp157=Cyc_PP_text(_tmp156?({const
char*_tmp15C="xtunion ";_tag_arr(_tmp15C,sizeof(char),_get_zero_arr_size(_tmp15C,
9));}):({const char*_tmp15D="tunion ";_tag_arr(_tmp15D,sizeof(char),
_get_zero_arr_size(_tmp15D,8));}));{void*_tmp158=Cyc_Tcutil_compress(_tmp122);
_LLFC: if((int)_tmp158 != 2)goto _LLFE;_LLFD: s=({struct Cyc_PP_Doc*_tmp159[3];
_tmp159[2]=Cyc_Absynpp_tps2doc(_tmp121);_tmp159[1]=Cyc_Absynpp_qvar2doc(_tmp155);
_tmp159[0]=_tmp157;Cyc_PP_cat(_tag_arr(_tmp159,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLFB;_LLFE:;_LLFF: s=({struct Cyc_PP_Doc*_tmp15A[5];_tmp15A[4]=Cyc_Absynpp_tps2doc(
_tmp121);_tmp15A[3]=Cyc_Absynpp_qvar2doc(_tmp155);_tmp15A[2]=Cyc_PP_text(({const
char*_tmp15B=" ";_tag_arr(_tmp15B,sizeof(char),_get_zero_arr_size(_tmp15B,2));}));
_tmp15A[1]=Cyc_Absynpp_typ2doc(_tmp122);_tmp15A[0]=_tmp157;Cyc_PP_cat(_tag_arr(
_tmp15A,sizeof(struct Cyc_PP_Doc*),5));});goto _LLFB;_LLFB:;}goto _LLF6;}_LLF6:;}
goto _LLBC;_LLCB: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 3)goto _LLCD;_tmp123=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp119)->f1;_tmp124=(void*)_tmp123.field_info;
_tmp125=_tmp123.targs;_LLCC:{void*_tmp15E=_tmp124;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp15F;struct _tuple0*_tmp160;struct _tuple0*_tmp161;int _tmp162;struct Cyc_Absyn_Tuniondecl*
_tmp163;struct Cyc_Absyn_Tuniondecl _tmp164;struct _tuple0*_tmp165;int _tmp166;
struct Cyc_Absyn_Tunionfield*_tmp167;struct Cyc_Absyn_Tunionfield _tmp168;struct
_tuple0*_tmp169;_LL101: if(*((int*)_tmp15E)!= 0)goto _LL103;_tmp15F=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp15E)->f1;_tmp160=_tmp15F.tunion_name;_tmp161=_tmp15F.field_name;_tmp162=
_tmp15F.is_xtunion;_LL102: _tmp165=_tmp160;_tmp166=_tmp162;_tmp169=_tmp161;goto
_LL104;_LL103: if(*((int*)_tmp15E)!= 1)goto _LL100;_tmp163=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp15E)->f1;_tmp164=*_tmp163;_tmp165=_tmp164.name;_tmp166=_tmp164.is_xtunion;
_tmp167=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp15E)->f2;_tmp168=*_tmp167;
_tmp169=_tmp168.name;_LL104: {struct Cyc_PP_Doc*_tmp16A=Cyc_PP_text(_tmp166?({
const char*_tmp16D="xtunion ";_tag_arr(_tmp16D,sizeof(char),_get_zero_arr_size(
_tmp16D,9));}):({const char*_tmp16E="tunion ";_tag_arr(_tmp16E,sizeof(char),
_get_zero_arr_size(_tmp16E,8));}));s=({struct Cyc_PP_Doc*_tmp16B[4];_tmp16B[3]=
Cyc_Absynpp_qvar2doc(_tmp169);_tmp16B[2]=Cyc_PP_text(({const char*_tmp16C=".";
_tag_arr(_tmp16C,sizeof(char),_get_zero_arr_size(_tmp16C,2));}));_tmp16B[1]=Cyc_Absynpp_qvar2doc(
_tmp165);_tmp16B[0]=_tmp16A;Cyc_PP_cat(_tag_arr(_tmp16B,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL100;}_LL100:;}goto _LLBC;_LLCD: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 5)goto _LLCF;_tmp126=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp119)->f1;
_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp119)->f2;_LLCE: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp16F=_tmp126;_LL106: if((int)_tmp16F
!= 2)goto _LL108;_LL107: goto _LL109;_LL108: if((int)_tmp16F != 0)goto _LL10A;_LL109:
sns=({const char*_tmp170="";_tag_arr(_tmp170,sizeof(char),_get_zero_arr_size(
_tmp170,1));});goto _LL105;_LL10A: if((int)_tmp16F != 1)goto _LL105;_LL10B: sns=({
const char*_tmp171="unsigned ";_tag_arr(_tmp171,sizeof(char),_get_zero_arr_size(
_tmp171,10));});goto _LL105;_LL105:;}{void*_tmp172=_tmp127;_LL10D: if((int)_tmp172
!= 0)goto _LL10F;_LL10E:{void*_tmp173=_tmp126;_LL116: if((int)_tmp173 != 2)goto
_LL118;_LL117: sns=({const char*_tmp174="";_tag_arr(_tmp174,sizeof(char),
_get_zero_arr_size(_tmp174,1));});goto _LL115;_LL118: if((int)_tmp173 != 0)goto
_LL11A;_LL119: sns=({const char*_tmp175="signed ";_tag_arr(_tmp175,sizeof(char),
_get_zero_arr_size(_tmp175,8));});goto _LL115;_LL11A: if((int)_tmp173 != 1)goto
_LL115;_LL11B: sns=({const char*_tmp176="unsigned ";_tag_arr(_tmp176,sizeof(char),
_get_zero_arr_size(_tmp176,10));});goto _LL115;_LL115:;}ts=({const char*_tmp177="char";
_tag_arr(_tmp177,sizeof(char),_get_zero_arr_size(_tmp177,5));});goto _LL10C;
_LL10F: if((int)_tmp172 != 1)goto _LL111;_LL110: ts=({const char*_tmp178="short";
_tag_arr(_tmp178,sizeof(char),_get_zero_arr_size(_tmp178,6));});goto _LL10C;
_LL111: if((int)_tmp172 != 2)goto _LL113;_LL112: ts=({const char*_tmp179="int";
_tag_arr(_tmp179,sizeof(char),_get_zero_arr_size(_tmp179,4));});goto _LL10C;
_LL113: if((int)_tmp172 != 3)goto _LL10C;_LL114: ts=Cyc_Absynpp_to_VC?({const char*
_tmp17A="__int64";_tag_arr(_tmp17A,sizeof(char),_get_zero_arr_size(_tmp17A,8));}):({
const char*_tmp17B="long long";_tag_arr(_tmp17B,sizeof(char),_get_zero_arr_size(
_tmp17B,10));});goto _LL10C;_LL10C:;}s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp17F;_tmp17F.tag=0;_tmp17F.f1=(struct _tagged_arr)((struct _tagged_arr)ts);{
struct Cyc_String_pa_struct _tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _tagged_arr)((
struct _tagged_arr)sns);{void*_tmp17C[2]={& _tmp17E,& _tmp17F};Cyc_aprintf(({const
char*_tmp17D="%s%s";_tag_arr(_tmp17D,sizeof(char),_get_zero_arr_size(_tmp17D,5));}),
_tag_arr(_tmp17C,sizeof(void*),2));}}}));goto _LLBC;}_LLCF: if((int)_tmp119 != 1)
goto _LLD1;_LLD0: s=Cyc_PP_text(({const char*_tmp180="float";_tag_arr(_tmp180,
sizeof(char),_get_zero_arr_size(_tmp180,6));}));goto _LLBC;_LLD1: if(_tmp119 <= (
void*)4  || *((int*)_tmp119)!= 6)goto _LLD3;_tmp128=((struct Cyc_Absyn_DoubleType_struct*)
_tmp119)->f1;_LLD2: if(_tmp128)s=Cyc_PP_text(({const char*_tmp181="long double";
_tag_arr(_tmp181,sizeof(char),_get_zero_arr_size(_tmp181,12));}));else{s=Cyc_PP_text(({
const char*_tmp182="double";_tag_arr(_tmp182,sizeof(char),_get_zero_arr_size(
_tmp182,7));}));}goto _LLBC;_LLD3: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 9)
goto _LLD5;_tmp129=((struct Cyc_Absyn_TupleType_struct*)_tmp119)->f1;_LLD4: s=({
struct Cyc_PP_Doc*_tmp183[2];_tmp183[1]=Cyc_Absynpp_args2doc(_tmp129);_tmp183[0]=
Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp183,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBC;_LLD5: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 10)goto _LLD7;_tmp12A=((
struct Cyc_Absyn_AggrType_struct*)_tmp119)->f1;_tmp12B=(void*)_tmp12A.aggr_info;
_tmp12C=_tmp12A.targs;_LLD6: {void*_tmp185;struct _tuple0*_tmp186;struct _tuple3
_tmp184=Cyc_Absyn_aggr_kinded_name(_tmp12B);_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;
s=({struct Cyc_PP_Doc*_tmp187[3];_tmp187[2]=Cyc_Absynpp_tps2doc(_tmp12C);_tmp187[
1]=Cyc_Absynpp_qvar2doc(_tmp186);_tmp187[0]=Cyc_Absynpp_aggr_kind2doc(_tmp185);
Cyc_PP_cat(_tag_arr(_tmp187,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;}_LLD7:
if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 11)goto _LLD9;_tmp12D=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp119)->f1;_tmp12E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp119)->f2;_LLD8: s=({struct Cyc_PP_Doc*_tmp188[4];_tmp188[3]=Cyc_Absynpp_rb();
_tmp188[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp12E));_tmp188[1]=Cyc_Absynpp_lb();
_tmp188[0]=Cyc_Absynpp_aggr_kind2doc(_tmp12D);Cyc_PP_cat(_tag_arr(_tmp188,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLBC;_LLD9: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 13)goto _LLDB;_tmp12F=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp119)->f1;
_LLDA: s=({struct Cyc_PP_Doc*_tmp189[4];_tmp189[3]=Cyc_Absynpp_rb();_tmp189[2]=Cyc_PP_nest(
2,Cyc_Absynpp_enumfields2doc(_tmp12F));_tmp189[1]=Cyc_Absynpp_lb();_tmp189[0]=
Cyc_PP_text(({const char*_tmp18A="enum ";_tag_arr(_tmp18A,sizeof(char),
_get_zero_arr_size(_tmp18A,6));}));Cyc_PP_cat(_tag_arr(_tmp189,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLBC;_LLDB: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 12)goto _LLDD;
_tmp130=((struct Cyc_Absyn_EnumType_struct*)_tmp119)->f1;_LLDC: s=({struct Cyc_PP_Doc*
_tmp18B[2];_tmp18B[1]=Cyc_Absynpp_qvar2doc(_tmp130);_tmp18B[0]=Cyc_PP_text(({
const char*_tmp18C="enum ";_tag_arr(_tmp18C,sizeof(char),_get_zero_arr_size(
_tmp18C,6));}));Cyc_PP_cat(_tag_arr(_tmp18B,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLBC;_LLDD: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 14)goto _LLDF;_tmp131=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp119)->f1;_LLDE: s=({struct Cyc_PP_Doc*
_tmp18D[3];_tmp18D[2]=Cyc_PP_text(({const char*_tmp18F=">";_tag_arr(_tmp18F,
sizeof(char),_get_zero_arr_size(_tmp18F,2));}));_tmp18D[1]=Cyc_Absynpp_typ2doc(
_tmp131);_tmp18D[0]=Cyc_PP_text(({const char*_tmp18E="sizeof_t<";_tag_arr(_tmp18E,
sizeof(char),_get_zero_arr_size(_tmp18E,10));}));Cyc_PP_cat(_tag_arr(_tmp18D,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;_LLDF: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 16)goto _LLE1;_tmp132=((struct Cyc_Absyn_TypedefType_struct*)_tmp119)->f1;
_tmp133=((struct Cyc_Absyn_TypedefType_struct*)_tmp119)->f2;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp119)->f3;_LLE0: s=({struct Cyc_PP_Doc*_tmp190[2];_tmp190[1]=Cyc_Absynpp_tps2doc(
_tmp133);_tmp190[0]=Cyc_Absynpp_qvar2doc(_tmp132);Cyc_PP_cat(_tag_arr(_tmp190,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBC;_LLE1: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 15)goto _LLE3;_tmp135=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp119)->f1;_LLE2: s=({struct Cyc_PP_Doc*_tmp191[3];_tmp191[2]=Cyc_PP_text(({
const char*_tmp193=">";_tag_arr(_tmp193,sizeof(char),_get_zero_arr_size(_tmp193,2));}));
_tmp191[1]=Cyc_Absynpp_rgn2doc(_tmp135);_tmp191[0]=Cyc_PP_text(({const char*
_tmp192="region_t<";_tag_arr(_tmp192,sizeof(char),_get_zero_arr_size(_tmp192,10));}));
Cyc_PP_cat(_tag_arr(_tmp191,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;_LLE3: if(
_tmp119 <= (void*)4  || *((int*)_tmp119)!= 18)goto _LLE5;_tmp136=((struct Cyc_Absyn_TypeInt_struct*)
_tmp119)->f1;_LLE4: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp196;_tmp196.tag=1;_tmp196.f1=(unsigned int)_tmp136;{void*_tmp194[1]={&
_tmp196};Cyc_aprintf(({const char*_tmp195="`%d";_tag_arr(_tmp195,sizeof(char),
_get_zero_arr_size(_tmp195,4));}),_tag_arr(_tmp194,sizeof(void*),1));}}));goto
_LLBC;_LLE5: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 17)goto _LLE7;_tmp137=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp119)->f1;_LLE6: s=({struct Cyc_PP_Doc*
_tmp197[3];_tmp197[2]=Cyc_PP_text(({const char*_tmp199=">";_tag_arr(_tmp199,
sizeof(char),_get_zero_arr_size(_tmp199,2));}));_tmp197[1]=Cyc_Absynpp_typ2doc(
_tmp137);_tmp197[0]=Cyc_PP_text(({const char*_tmp198="tag_t<";_tag_arr(_tmp198,
sizeof(char),_get_zero_arr_size(_tmp198,7));}));Cyc_PP_cat(_tag_arr(_tmp197,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;_LLE7: if((int)_tmp119 != 3)goto _LLE9;
_LLE8: goto _LLEA;_LLE9: if((int)_tmp119 != 2)goto _LLEB;_LLEA: s=Cyc_Absynpp_rgn2doc(
t);goto _LLBC;_LLEB: if(_tmp119 <= (void*)4  || *((int*)_tmp119)!= 21)goto _LLED;
_tmp138=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp119)->f1;_LLEC: s=({struct
Cyc_PP_Doc*_tmp19A[3];_tmp19A[2]=Cyc_PP_text(({const char*_tmp19C=")";_tag_arr(
_tmp19C,sizeof(char),_get_zero_arr_size(_tmp19C,2));}));_tmp19A[1]=Cyc_Absynpp_typ2doc(
_tmp138);_tmp19A[0]=Cyc_PP_text(({const char*_tmp19B="regions(";_tag_arr(_tmp19B,
sizeof(char),_get_zero_arr_size(_tmp19B,9));}));Cyc_PP_cat(_tag_arr(_tmp19A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBC;_LLED: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 19)goto _LLEF;_LLEE: goto _LLF0;_LLEF: if(_tmp119 <= (void*)4  || *((int*)
_tmp119)!= 20)goto _LLBC;_LLF0: s=Cyc_Absynpp_eff2doc(t);goto _LLBC;_LLBC:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc():
Cyc_PP_text(*((struct _tagged_arr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple5*cmp){struct _tuple5 _tmp19E;void*_tmp19F;void*_tmp1A0;struct _tuple5*
_tmp19D=cmp;_tmp19E=*_tmp19D;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;return({
struct Cyc_PP_Doc*_tmp1A1[3];_tmp1A1[2]=Cyc_Absynpp_rgn2doc(_tmp1A0);_tmp1A1[1]=
Cyc_PP_text(({const char*_tmp1A2=" > ";_tag_arr(_tmp1A2,sizeof(char),
_get_zero_arr_size(_tmp1A2,4));}));_tmp1A1[0]=Cyc_Absynpp_rgn2doc(_tmp19F);Cyc_PP_cat(
_tag_arr(_tmp1A1,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(({const char*_tmp1A3="";_tag_arr(
_tmp1A3,sizeof(char),_get_zero_arr_size(_tmp1A3,1));}),({const char*_tmp1A4="";
_tag_arr(_tmp1A4,sizeof(char),_get_zero_arr_size(_tmp1A4,1));}),({const char*
_tmp1A5=",";_tag_arr(_tmp1A5,sizeof(char),_get_zero_arr_size(_tmp1A5,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->v=Cyc_PP_text(*((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmp1A6;});return Cyc_Absynpp_tqtd2doc((*
t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp1A7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A7,({struct
Cyc_List_List*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->hd=Cyc_PP_text(({
const char*_tmp1A9="...";_tag_arr(_tmp1A9,sizeof(char),_get_zero_arr_size(_tmp1A9,
4));}));_tmp1A8->tl=0;_tmp1A8;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmp1AA=({struct Cyc_PP_Doc*_tmp1AC[3];_tmp1AC[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=cyc_varargs->name;
_tmp1B0->f2=cyc_varargs->tq;_tmp1B0->f3=(void*)cyc_varargs->type;_tmp1B0;}));
_tmp1AC[1]=cyc_varargs->inject?Cyc_PP_text(({const char*_tmp1AE=" inject ";
_tag_arr(_tmp1AE,sizeof(char),_get_zero_arr_size(_tmp1AE,9));})): Cyc_PP_text(({
const char*_tmp1AF=" ";_tag_arr(_tmp1AF,sizeof(char),_get_zero_arr_size(_tmp1AF,2));}));
_tmp1AC[0]=Cyc_PP_text(({const char*_tmp1AD="...";_tag_arr(_tmp1AD,sizeof(char),
_get_zero_arr_size(_tmp1AD,4));}));Cyc_PP_cat(_tag_arr(_tmp1AC,sizeof(struct Cyc_PP_Doc*),
3));});_tmp1A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A7,({struct Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*
_tmp1AB));_tmp1AB->hd=_tmp1AA;_tmp1AB->tl=0;_tmp1AB;}));}}{struct Cyc_PP_Doc*
_tmp1B1=Cyc_PP_group(({const char*_tmp1B9="";_tag_arr(_tmp1B9,sizeof(char),
_get_zero_arr_size(_tmp1B9,1));}),({const char*_tmp1BA="";_tag_arr(_tmp1BA,
sizeof(char),_get_zero_arr_size(_tmp1BA,1));}),({const char*_tmp1BB=",";_tag_arr(
_tmp1BB,sizeof(char),_get_zero_arr_size(_tmp1BB,2));}),_tmp1A7);if(effopt != 0
 && Cyc_Absynpp_print_all_effects)_tmp1B1=({struct Cyc_PP_Doc*_tmp1B2[3];_tmp1B2[
2]=Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1B2[1]=Cyc_PP_text(({const char*
_tmp1B3=";";_tag_arr(_tmp1B3,sizeof(char),_get_zero_arr_size(_tmp1B3,2));}));
_tmp1B2[0]=_tmp1B1;Cyc_PP_cat(_tag_arr(_tmp1B2,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po != 0)_tmp1B1=({struct Cyc_PP_Doc*_tmp1B4[3];_tmp1B4[2]=Cyc_Absynpp_rgnpo2doc(
rgn_po);_tmp1B4[1]=Cyc_PP_text(({const char*_tmp1B5=":";_tag_arr(_tmp1B5,sizeof(
char),_get_zero_arr_size(_tmp1B5,2));}));_tmp1B4[0]=_tmp1B1;Cyc_PP_cat(_tag_arr(
_tmp1B4,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmp1B6[3];
_tmp1B6[2]=Cyc_PP_text(({const char*_tmp1B8=")";_tag_arr(_tmp1B8,sizeof(char),
_get_zero_arr_size(_tmp1B8,2));}));_tmp1B6[1]=_tmp1B1;_tmp1B6[0]=Cyc_PP_text(({
const char*_tmp1B7="(";_tag_arr(_tmp1B7,sizeof(char),_get_zero_arr_size(_tmp1B7,2));}));
Cyc_PP_cat(_tag_arr(_tmp1B6,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){return({struct _tuple1*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));
_tmp1BC->f1=({struct Cyc_Core_Opt*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->v=(*
arg).f1;_tmp1BD;});_tmp1BC->f2=(*arg).f2;_tmp1BC->f3=(*arg).f3;_tmp1BC;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _tagged_arr*v){return Cyc_PP_text(*v);}
struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BE=0;int match;{void*_tmp1BF=(*q).f1;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*
_tmp1C1;_LL11D: if((int)_tmp1BF != 0)goto _LL11F;_LL11E: _tmp1C0=0;goto _LL120;_LL11F:
if(_tmp1BF <= (void*)1  || *((int*)_tmp1BF)!= 0)goto _LL121;_tmp1C0=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1BF)->f1;_LL120: match=0;_tmp1BE=_tmp1C0;goto _LL11C;_LL121: if(_tmp1BF <= (void*)
1  || *((int*)_tmp1BF)!= 1)goto _LL11C;_tmp1C1=((struct Cyc_Absyn_Abs_n_struct*)
_tmp1BF)->f1;_LL122: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C1,Cyc_Absynpp_curr_namespace);
_tmp1BE=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1C2->tl=_tmp1C1;_tmp1C2;}): _tmp1C1;goto _LL11C;_LL11C:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _tagged_arr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BE,({struct Cyc_List_List*_tmp1C3=
_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=(*q).f2;_tmp1C3->tl=0;_tmp1C3;})),({
const char*_tmp1C4="_";_tag_arr(_tmp1C4,sizeof(char),_get_zero_arr_size(_tmp1C4,2));}));
else{if(match)return*(*q).f2;else{return(struct _tagged_arr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BE,({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=(*
q).f2;_tmp1C5->tl=0;_tmp1C5;})),({const char*_tmp1C6="::";_tag_arr(_tmp1C6,
sizeof(char),_get_zero_arr_size(_tmp1C6,3));}));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _tagged_arr _tmp1C7=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _tagged_arr)Cyc_strconcat((
struct _tagged_arr)Cyc_strconcat(({const char*_tmp1C8="\\textbf{";_tag_arr(_tmp1C8,
sizeof(char),_get_zero_arr_size(_tmp1C8,9));}),(struct _tagged_arr)_tmp1C7),({
const char*_tmp1C9="}";_tag_arr(_tmp1C9,sizeof(char),_get_zero_arr_size(_tmp1C9,2));})),(
int)Cyc_strlen((struct _tagged_arr)_tmp1C7));else{return Cyc_PP_text(_tmp1C7);}}
struct _tagged_arr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){void*_tmp1CA=(*
v).f1;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;_LL124: if((int)
_tmp1CA != 0)goto _LL126;_LL125: goto _LL127;_LL126: if(_tmp1CA <= (void*)1  || *((int*)
_tmp1CA)!= 0)goto _LL128;_tmp1CB=((struct Cyc_Absyn_Rel_n_struct*)_tmp1CA)->f1;if(
_tmp1CB != 0)goto _LL128;_LL127: return*(*v).f2;_LL128: if(_tmp1CA <= (void*)1  || *((
int*)_tmp1CA)!= 1)goto _LL12A;_tmp1CC=((struct Cyc_Absyn_Abs_n_struct*)_tmp1CA)->f1;
_LL129: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CC,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL12B;}_LL12A:;_LL12B: return(struct _tagged_arr)Cyc_strconcat(({
const char*_tmp1CD="/* bad namespace : */ ";_tag_arr(_tmp1CD,sizeof(char),
_get_zero_arr_size(_tmp1CD,23));}),(struct _tagged_arr)Cyc_Absynpp_qvar2string(v));
_LL123:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct
Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){struct _tagged_arr
_tmp1CE=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _tagged_arr)Cyc_strconcat((struct _tagged_arr)Cyc_strconcat(({const char*
_tmp1CF="\\textbf{";_tag_arr(_tmp1CF,sizeof(char),_get_zero_arr_size(_tmp1CF,9));}),(
struct _tagged_arr)_tmp1CE),({const char*_tmp1D0="}";_tag_arr(_tmp1D0,sizeof(char),
_get_zero_arr_size(_tmp1D0,2));})),(int)Cyc_strlen((struct _tagged_arr)_tmp1CE));
else{return Cyc_PP_text(_tmp1CE);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp1D1=(void*)e->r;void*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;_LL12D: if(*((int*)_tmp1D1)!= 0)goto _LL12F;
_LL12E: goto _LL130;_LL12F: if(*((int*)_tmp1D1)!= 1)goto _LL131;_LL130: goto _LL132;
_LL131: if(*((int*)_tmp1D1)!= 2)goto _LL133;_LL132: return 10000;_LL133: if(*((int*)
_tmp1D1)!= 3)goto _LL135;_tmp1D2=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D1)->f1;_LL134: {void*_tmp1D5=_tmp1D2;_LL17A: if((int)_tmp1D5 != 0)goto _LL17C;
_LL17B: return 100;_LL17C: if((int)_tmp1D5 != 1)goto _LL17E;_LL17D: return 110;_LL17E:
if((int)_tmp1D5 != 2)goto _LL180;_LL17F: return 100;_LL180: if((int)_tmp1D5 != 3)goto
_LL182;_LL181: goto _LL183;_LL182: if((int)_tmp1D5 != 4)goto _LL184;_LL183: return 110;
_LL184: if((int)_tmp1D5 != 5)goto _LL186;_LL185: goto _LL187;_LL186: if((int)_tmp1D5 != 
6)goto _LL188;_LL187: return 70;_LL188: if((int)_tmp1D5 != 7)goto _LL18A;_LL189: goto
_LL18B;_LL18A: if((int)_tmp1D5 != 8)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if((int)
_tmp1D5 != 9)goto _LL18E;_LL18D: goto _LL18F;_LL18E: if((int)_tmp1D5 != 10)goto _LL190;
_LL18F: return 80;_LL190: if((int)_tmp1D5 != 11)goto _LL192;_LL191: goto _LL193;_LL192:
if((int)_tmp1D5 != 12)goto _LL194;_LL193: return 130;_LL194: if((int)_tmp1D5 != 13)
goto _LL196;_LL195: return 60;_LL196: if((int)_tmp1D5 != 14)goto _LL198;_LL197: return
40;_LL198: if((int)_tmp1D5 != 15)goto _LL19A;_LL199: return 50;_LL19A: if((int)_tmp1D5
!= 16)goto _LL19C;_LL19B: return 90;_LL19C: if((int)_tmp1D5 != 17)goto _LL19E;_LL19D:
return 80;_LL19E: if((int)_tmp1D5 != 18)goto _LL1A0;_LL19F: return 80;_LL1A0: if((int)
_tmp1D5 != 19)goto _LL179;_LL1A1: return 140;_LL179:;}_LL135: if(*((int*)_tmp1D1)!= 4)
goto _LL137;_LL136: return 20;_LL137: if(*((int*)_tmp1D1)!= 5)goto _LL139;_LL138:
return 130;_LL139: if(*((int*)_tmp1D1)!= 6)goto _LL13B;_LL13A: return 30;_LL13B: if(*((
int*)_tmp1D1)!= 7)goto _LL13D;_LL13C: return 35;_LL13D: if(*((int*)_tmp1D1)!= 8)goto
_LL13F;_LL13E: return 30;_LL13F: if(*((int*)_tmp1D1)!= 9)goto _LL141;_LL140: return 10;
_LL141: if(*((int*)_tmp1D1)!= 10)goto _LL143;_LL142: goto _LL144;_LL143: if(*((int*)
_tmp1D1)!= 11)goto _LL145;_LL144: return 140;_LL145: if(*((int*)_tmp1D1)!= 12)goto
_LL147;_LL146: return 130;_LL147: if(*((int*)_tmp1D1)!= 13)goto _LL149;_tmp1D3=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D1)->f1;_LL148: return Cyc_Absynpp_exp_prec(
_tmp1D3);_LL149: if(*((int*)_tmp1D1)!= 14)goto _LL14B;_tmp1D4=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D1)->f1;_LL14A: return Cyc_Absynpp_exp_prec(_tmp1D4);_LL14B: if(*((int*)
_tmp1D1)!= 15)goto _LL14D;_LL14C: return 120;_LL14D: if(*((int*)_tmp1D1)!= 17)goto
_LL14F;_LL14E: return 15;_LL14F: if(*((int*)_tmp1D1)!= 16)goto _LL151;_LL150: goto
_LL152;_LL151: if(*((int*)_tmp1D1)!= 18)goto _LL153;_LL152: goto _LL154;_LL153: if(*((
int*)_tmp1D1)!= 19)goto _LL155;_LL154: goto _LL156;_LL155: if(*((int*)_tmp1D1)!= 20)
goto _LL157;_LL156: goto _LL158;_LL157: if(*((int*)_tmp1D1)!= 21)goto _LL159;_LL158:
goto _LL15A;_LL159: if(*((int*)_tmp1D1)!= 22)goto _LL15B;_LL15A: return 130;_LL15B:
if(*((int*)_tmp1D1)!= 23)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmp1D1)
!= 24)goto _LL15F;_LL15E: goto _LL160;_LL15F: if(*((int*)_tmp1D1)!= 25)goto _LL161;
_LL160: return 140;_LL161: if(*((int*)_tmp1D1)!= 26)goto _LL163;_LL162: return 150;
_LL163: if(*((int*)_tmp1D1)!= 27)goto _LL165;_LL164: goto _LL166;_LL165: if(*((int*)
_tmp1D1)!= 28)goto _LL167;_LL166: goto _LL168;_LL167: if(*((int*)_tmp1D1)!= 29)goto
_LL169;_LL168: goto _LL16A;_LL169: if(*((int*)_tmp1D1)!= 30)goto _LL16B;_LL16A: goto
_LL16C;_LL16B: if(*((int*)_tmp1D1)!= 31)goto _LL16D;_LL16C: goto _LL16E;_LL16D: if(*((
int*)_tmp1D1)!= 32)goto _LL16F;_LL16E: goto _LL170;_LL16F: if(*((int*)_tmp1D1)!= 33)
goto _LL171;_LL170: goto _LL172;_LL171: if(*((int*)_tmp1D1)!= 34)goto _LL173;_LL172:
goto _LL174;_LL173: if(*((int*)_tmp1D1)!= 35)goto _LL175;_LL174: goto _LL176;_LL175:
if(*((int*)_tmp1D1)!= 36)goto _LL177;_LL176: return 140;_LL177: if(*((int*)_tmp1D1)
!= 37)goto _LL12C;_LL178: return 10000;_LL12C:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1D6=(void*)e->r;void*_tmp1D7;struct _tuple0*_tmp1D8;struct _tuple0*
_tmp1D9;void*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;
struct Cyc_Core_Opt*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*
_tmp1DF;void*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;
struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_List_List*
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;void*_tmp1F1;
struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;
void*_tmp1F8;void*_tmp1F9;struct _tagged_arr*_tmp1FA;void*_tmp1FB;void*_tmp1FC;
unsigned int _tmp1FD;struct Cyc_List_List*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Exp*
_tmp200;struct Cyc_Absyn_Exp*_tmp201;struct _tagged_arr*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct _tagged_arr*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_List_List*_tmp207;struct _tuple1*_tmp208;struct Cyc_List_List*
_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Exp*
_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct _tuple0*_tmp20E;struct Cyc_List_List*
_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_Absyn_Tunionfield*_tmp213;struct _tuple0*_tmp214;struct _tuple0*
_tmp215;struct Cyc_Absyn_MallocInfo _tmp216;int _tmp217;struct Cyc_Absyn_Exp*_tmp218;
void**_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Core_Opt*_tmp21B;struct Cyc_List_List*
_tmp21C;struct Cyc_Absyn_Stmt*_tmp21D;_LL1A3: if(*((int*)_tmp1D6)!= 0)goto _LL1A5;
_tmp1D7=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp1D6)->f1;_LL1A4: s=Cyc_Absynpp_cnst2doc(
_tmp1D7);goto _LL1A2;_LL1A5: if(*((int*)_tmp1D6)!= 1)goto _LL1A7;_tmp1D8=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D6)->f1;_LL1A6: _tmp1D9=_tmp1D8;goto _LL1A8;_LL1A7:
if(*((int*)_tmp1D6)!= 2)goto _LL1A9;_tmp1D9=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D6)->f1;_LL1A8: s=Cyc_Absynpp_qvar2doc(_tmp1D9);goto _LL1A2;_LL1A9: if(*((int*)
_tmp1D6)!= 3)goto _LL1AB;_tmp1DA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D6)->f1;_tmp1DB=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D6)->f2;_LL1AA: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1DA,_tmp1DB);goto _LL1A2;_LL1AB: if(*((int*)
_tmp1D6)!= 4)goto _LL1AD;_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D6)->f1;
_tmp1DD=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D6)->f2;_tmp1DE=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D6)->f3;_LL1AC: s=({struct Cyc_PP_Doc*_tmp21E[5];_tmp21E[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DE);_tmp21E[3]=Cyc_PP_text(({const char*_tmp221="= ";_tag_arr(_tmp221,
sizeof(char),_get_zero_arr_size(_tmp221,3));}));_tmp21E[2]=_tmp1DD == 0?Cyc_PP_text(({
const char*_tmp220="";_tag_arr(_tmp220,sizeof(char),_get_zero_arr_size(_tmp220,1));})):
Cyc_Absynpp_prim2doc((void*)_tmp1DD->v);_tmp21E[1]=Cyc_PP_text(({const char*
_tmp21F=" ";_tag_arr(_tmp21F,sizeof(char),_get_zero_arr_size(_tmp21F,2));}));
_tmp21E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DC);Cyc_PP_cat(_tag_arr(_tmp21E,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A2;_LL1AD: if(*((int*)_tmp1D6)!= 5)goto
_LL1AF;_tmp1DF=((struct Cyc_Absyn_Increment_e_struct*)_tmp1D6)->f1;_tmp1E0=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp1D6)->f2;_LL1AE: {struct Cyc_PP_Doc*
_tmp222=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DF);{void*_tmp223=_tmp1E0;_LL1F2:
if((int)_tmp223 != 0)goto _LL1F4;_LL1F3: s=({struct Cyc_PP_Doc*_tmp224[2];_tmp224[1]=
_tmp222;_tmp224[0]=Cyc_PP_text(({const char*_tmp225="++";_tag_arr(_tmp225,sizeof(
char),_get_zero_arr_size(_tmp225,3));}));Cyc_PP_cat(_tag_arr(_tmp224,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1F1;_LL1F4: if((int)_tmp223 != 2)goto _LL1F6;
_LL1F5: s=({struct Cyc_PP_Doc*_tmp226[2];_tmp226[1]=_tmp222;_tmp226[0]=Cyc_PP_text(({
const char*_tmp227="--";_tag_arr(_tmp227,sizeof(char),_get_zero_arr_size(_tmp227,
3));}));Cyc_PP_cat(_tag_arr(_tmp226,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F1;
_LL1F6: if((int)_tmp223 != 1)goto _LL1F8;_LL1F7: s=({struct Cyc_PP_Doc*_tmp228[2];
_tmp228[1]=Cyc_PP_text(({const char*_tmp229="++";_tag_arr(_tmp229,sizeof(char),
_get_zero_arr_size(_tmp229,3));}));_tmp228[0]=_tmp222;Cyc_PP_cat(_tag_arr(
_tmp228,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1F1;_LL1F8: if((int)_tmp223 != 3)
goto _LL1F1;_LL1F9: s=({struct Cyc_PP_Doc*_tmp22A[2];_tmp22A[1]=Cyc_PP_text(({const
char*_tmp22B="--";_tag_arr(_tmp22B,sizeof(char),_get_zero_arr_size(_tmp22B,3));}));
_tmp22A[0]=_tmp222;Cyc_PP_cat(_tag_arr(_tmp22A,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1F1;_LL1F1:;}goto _LL1A2;}_LL1AF: if(*((int*)_tmp1D6)!= 6)goto _LL1B1;
_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D6)->f1;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D6)->f2;_tmp1E3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D6)->f3;_LL1B0:
s=({struct Cyc_PP_Doc*_tmp22C[5];_tmp22C[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E3);_tmp22C[3]=Cyc_PP_text(({const char*_tmp22E=" : ";_tag_arr(_tmp22E,
sizeof(char),_get_zero_arr_size(_tmp22E,4));}));_tmp22C[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E2);_tmp22C[1]=Cyc_PP_text(({const char*_tmp22D=" ? ";_tag_arr(_tmp22D,
sizeof(char),_get_zero_arr_size(_tmp22D,4));}));_tmp22C[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E1);Cyc_PP_cat(_tag_arr(_tmp22C,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL1A2;_LL1B1: if(*((int*)_tmp1D6)!= 7)goto _LL1B3;_tmp1E4=((struct Cyc_Absyn_And_e_struct*)
_tmp1D6)->f1;_tmp1E5=((struct Cyc_Absyn_And_e_struct*)_tmp1D6)->f2;_LL1B2: s=({
struct Cyc_PP_Doc*_tmp22F[3];_tmp22F[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E5);
_tmp22F[1]=Cyc_PP_text(({const char*_tmp230=" && ";_tag_arr(_tmp230,sizeof(char),
_get_zero_arr_size(_tmp230,5));}));_tmp22F[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E4);Cyc_PP_cat(_tag_arr(_tmp22F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;
_LL1B3: if(*((int*)_tmp1D6)!= 8)goto _LL1B5;_tmp1E6=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D6)->f1;_tmp1E7=((struct Cyc_Absyn_Or_e_struct*)_tmp1D6)->f2;_LL1B4: s=({
struct Cyc_PP_Doc*_tmp231[3];_tmp231[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E7);
_tmp231[1]=Cyc_PP_text(({const char*_tmp232=" || ";_tag_arr(_tmp232,sizeof(char),
_get_zero_arr_size(_tmp232,5));}));_tmp231[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp1E6);Cyc_PP_cat(_tag_arr(_tmp231,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;
_LL1B5: if(*((int*)_tmp1D6)!= 9)goto _LL1B7;_tmp1E8=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D6)->f1;_tmp1E9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D6)->f2;_LL1B6: s=({
struct Cyc_PP_Doc*_tmp233[5];_tmp233[4]=Cyc_PP_text(({const char*_tmp236=")";
_tag_arr(_tmp236,sizeof(char),_get_zero_arr_size(_tmp236,2));}));_tmp233[3]=Cyc_Absynpp_exp2doc(
_tmp1E9);_tmp233[2]=Cyc_PP_text(({const char*_tmp235=", ";_tag_arr(_tmp235,
sizeof(char),_get_zero_arr_size(_tmp235,3));}));_tmp233[1]=Cyc_Absynpp_exp2doc(
_tmp1E8);_tmp233[0]=Cyc_PP_text(({const char*_tmp234="(";_tag_arr(_tmp234,sizeof(
char),_get_zero_arr_size(_tmp234,2));}));Cyc_PP_cat(_tag_arr(_tmp233,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A2;_LL1B7: if(*((int*)_tmp1D6)!= 10)goto _LL1B9;
_tmp1EA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D6)->f1;_tmp1EB=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D6)->f2;_LL1B8: s=({struct Cyc_PP_Doc*_tmp237[4];_tmp237[3]=Cyc_PP_text(({
const char*_tmp239=")";_tag_arr(_tmp239,sizeof(char),_get_zero_arr_size(_tmp239,2));}));
_tmp237[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1EB);_tmp237[1]=Cyc_PP_text(({const
char*_tmp238="(";_tag_arr(_tmp238,sizeof(char),_get_zero_arr_size(_tmp238,2));}));
_tmp237[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EA);Cyc_PP_cat(_tag_arr(_tmp237,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A2;_LL1B9: if(*((int*)_tmp1D6)!= 11)goto
_LL1BB;_tmp1EC=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D6)->f1;_tmp1ED=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp1D6)->f2;_LL1BA: s=({struct Cyc_PP_Doc*_tmp23A[4];
_tmp23A[3]=Cyc_PP_text(({const char*_tmp23C=")";_tag_arr(_tmp23C,sizeof(char),
_get_zero_arr_size(_tmp23C,2));}));_tmp23A[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1ED);_tmp23A[1]=Cyc_PP_text(({const char*_tmp23B="(";_tag_arr(_tmp23B,sizeof(
char),_get_zero_arr_size(_tmp23B,2));}));_tmp23A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EC);Cyc_PP_cat(_tag_arr(_tmp23A,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A2;_LL1BB: if(*((int*)_tmp1D6)!= 12)goto _LL1BD;_tmp1EE=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D6)->f1;_LL1BC: s=({struct Cyc_PP_Doc*_tmp23D[2];_tmp23D[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EE);_tmp23D[0]=Cyc_PP_text(({const char*_tmp23E="throw ";_tag_arr(
_tmp23E,sizeof(char),_get_zero_arr_size(_tmp23E,7));}));Cyc_PP_cat(_tag_arr(
_tmp23D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A2;_LL1BD: if(*((int*)_tmp1D6)!= 
13)goto _LL1BF;_tmp1EF=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D6)->f1;
_LL1BE: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EF);goto _LL1A2;_LL1BF: if(*((int*)
_tmp1D6)!= 14)goto _LL1C1;_tmp1F0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D6)->f1;
_LL1C0: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F0);goto _LL1A2;_LL1C1: if(*((int*)
_tmp1D6)!= 15)goto _LL1C3;_tmp1F1=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D6)->f1;
_tmp1F2=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D6)->f2;_LL1C2: s=({struct Cyc_PP_Doc*
_tmp23F[4];_tmp23F[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F2);_tmp23F[2]=Cyc_PP_text(({
const char*_tmp241=")";_tag_arr(_tmp241,sizeof(char),_get_zero_arr_size(_tmp241,2));}));
_tmp23F[1]=Cyc_Absynpp_typ2doc(_tmp1F1);_tmp23F[0]=Cyc_PP_text(({const char*
_tmp240="(";_tag_arr(_tmp240,sizeof(char),_get_zero_arr_size(_tmp240,2));}));Cyc_PP_cat(
_tag_arr(_tmp23F,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A2;_LL1C3: if(*((int*)
_tmp1D6)!= 16)goto _LL1C5;_tmp1F3=((struct Cyc_Absyn_Address_e_struct*)_tmp1D6)->f1;
_LL1C4: s=({struct Cyc_PP_Doc*_tmp242[2];_tmp242[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F3);_tmp242[0]=Cyc_PP_text(({const char*_tmp243="&";_tag_arr(_tmp243,
sizeof(char),_get_zero_arr_size(_tmp243,2));}));Cyc_PP_cat(_tag_arr(_tmp242,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A2;_LL1C5: if(*((int*)_tmp1D6)!= 17)goto
_LL1C7;_tmp1F4=((struct Cyc_Absyn_New_e_struct*)_tmp1D6)->f1;_tmp1F5=((struct Cyc_Absyn_New_e_struct*)
_tmp1D6)->f2;_LL1C6: if(_tmp1F4 == 0)s=({struct Cyc_PP_Doc*_tmp244[2];_tmp244[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F5);_tmp244[0]=Cyc_PP_text(({const char*
_tmp245="new ";_tag_arr(_tmp245,sizeof(char),_get_zero_arr_size(_tmp245,5));}));
Cyc_PP_cat(_tag_arr(_tmp244,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp246[4];_tmp246[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F5);_tmp246[2]=Cyc_PP_text(({
const char*_tmp248=") ";_tag_arr(_tmp248,sizeof(char),_get_zero_arr_size(_tmp248,
3));}));_tmp246[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp1F4);_tmp246[0]=
Cyc_PP_text(({const char*_tmp247="rnew(";_tag_arr(_tmp247,sizeof(char),
_get_zero_arr_size(_tmp247,6));}));Cyc_PP_cat(_tag_arr(_tmp246,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL1A2;_LL1C7: if(*((int*)_tmp1D6)!= 18)goto _LL1C9;_tmp1F6=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D6)->f1;_LL1C8: s=({struct Cyc_PP_Doc*
_tmp249[3];_tmp249[2]=Cyc_PP_text(({const char*_tmp24B=")";_tag_arr(_tmp24B,
sizeof(char),_get_zero_arr_size(_tmp24B,2));}));_tmp249[1]=Cyc_Absynpp_typ2doc(
_tmp1F6);_tmp249[0]=Cyc_PP_text(({const char*_tmp24A="sizeof(";_tag_arr(_tmp24A,
sizeof(char),_get_zero_arr_size(_tmp24A,8));}));Cyc_PP_cat(_tag_arr(_tmp249,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;_LL1C9: if(*((int*)_tmp1D6)!= 19)goto
_LL1CB;_tmp1F7=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D6)->f1;_LL1CA: s=({
struct Cyc_PP_Doc*_tmp24C[3];_tmp24C[2]=Cyc_PP_text(({const char*_tmp24E=")";
_tag_arr(_tmp24E,sizeof(char),_get_zero_arr_size(_tmp24E,2));}));_tmp24C[1]=Cyc_Absynpp_exp2doc(
_tmp1F7);_tmp24C[0]=Cyc_PP_text(({const char*_tmp24D="sizeof(";_tag_arr(_tmp24D,
sizeof(char),_get_zero_arr_size(_tmp24D,8));}));Cyc_PP_cat(_tag_arr(_tmp24C,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;_LL1CB: if(*((int*)_tmp1D6)!= 20)goto
_LL1CD;_tmp1F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f1;_tmp1F9=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f2;if(*((int*)_tmp1F9)!= 0)
goto _LL1CD;_tmp1FA=((struct Cyc_Absyn_StructField_struct*)_tmp1F9)->f1;_LL1CC: s=({
struct Cyc_PP_Doc*_tmp24F[5];_tmp24F[4]=Cyc_PP_text(({const char*_tmp252=")";
_tag_arr(_tmp252,sizeof(char),_get_zero_arr_size(_tmp252,2));}));_tmp24F[3]=Cyc_PP_textptr(
_tmp1FA);_tmp24F[2]=Cyc_PP_text(({const char*_tmp251=",";_tag_arr(_tmp251,sizeof(
char),_get_zero_arr_size(_tmp251,2));}));_tmp24F[1]=Cyc_Absynpp_typ2doc(_tmp1F8);
_tmp24F[0]=Cyc_PP_text(({const char*_tmp250="offsetof(";_tag_arr(_tmp250,sizeof(
char),_get_zero_arr_size(_tmp250,10));}));Cyc_PP_cat(_tag_arr(_tmp24F,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1A2;_LL1CD: if(*((int*)_tmp1D6)!= 20)goto _LL1CF;
_tmp1FB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f1;_tmp1FC=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f2;if(*((int*)_tmp1FC)!= 1)goto
_LL1CF;_tmp1FD=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FC)->f1;_LL1CE: s=({
struct Cyc_PP_Doc*_tmp253[5];_tmp253[4]=Cyc_PP_text(({const char*_tmp259=")";
_tag_arr(_tmp259,sizeof(char),_get_zero_arr_size(_tmp259,2));}));_tmp253[3]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp258;_tmp258.tag=1;_tmp258.f1=(
unsigned int)((int)_tmp1FD);{void*_tmp256[1]={& _tmp258};Cyc_aprintf(({const char*
_tmp257="%d";_tag_arr(_tmp257,sizeof(char),_get_zero_arr_size(_tmp257,3));}),
_tag_arr(_tmp256,sizeof(void*),1));}}));_tmp253[2]=Cyc_PP_text(({const char*
_tmp255=",";_tag_arr(_tmp255,sizeof(char),_get_zero_arr_size(_tmp255,2));}));
_tmp253[1]=Cyc_Absynpp_typ2doc(_tmp1FB);_tmp253[0]=Cyc_PP_text(({const char*
_tmp254="offsetof(";_tag_arr(_tmp254,sizeof(char),_get_zero_arr_size(_tmp254,10));}));
Cyc_PP_cat(_tag_arr(_tmp253,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1A2;_LL1CF:
if(*((int*)_tmp1D6)!= 21)goto _LL1D1;_tmp1FE=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D6)->f1;_tmp1FF=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D6)->f2;
_LL1D0: s=({struct Cyc_PP_Doc*_tmp25A[4];_tmp25A[3]=Cyc_PP_text(({const char*
_tmp25C=")";_tag_arr(_tmp25C,sizeof(char),_get_zero_arr_size(_tmp25C,2));}));
_tmp25A[2]=Cyc_Absynpp_typ2doc(_tmp1FF);_tmp25A[1]=Cyc_Absynpp_tvars2doc(_tmp1FE);
_tmp25A[0]=Cyc_PP_text(({const char*_tmp25B="__gen(";_tag_arr(_tmp25B,sizeof(char),
_get_zero_arr_size(_tmp25B,7));}));Cyc_PP_cat(_tag_arr(_tmp25A,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL1A2;_LL1D1: if(*((int*)_tmp1D6)!= 22)goto _LL1D3;_tmp200=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D6)->f1;_LL1D2: s=({struct Cyc_PP_Doc*_tmp25D[2];_tmp25D[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp200);_tmp25D[0]=Cyc_PP_text(({const char*_tmp25E="*";_tag_arr(_tmp25E,
sizeof(char),_get_zero_arr_size(_tmp25E,2));}));Cyc_PP_cat(_tag_arr(_tmp25D,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A2;_LL1D3: if(*((int*)_tmp1D6)!= 23)goto
_LL1D5;_tmp201=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D6)->f1;_tmp202=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D6)->f2;_LL1D4: s=({struct Cyc_PP_Doc*
_tmp25F[3];_tmp25F[2]=Cyc_PP_textptr(_tmp202);_tmp25F[1]=Cyc_PP_text(({const char*
_tmp260=".";_tag_arr(_tmp260,sizeof(char),_get_zero_arr_size(_tmp260,2));}));
_tmp25F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp201);Cyc_PP_cat(_tag_arr(_tmp25F,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;_LL1D5: if(*((int*)_tmp1D6)!= 24)goto
_LL1D7;_tmp203=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D6)->f1;_tmp204=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D6)->f2;_LL1D6: s=({struct Cyc_PP_Doc*
_tmp261[3];_tmp261[2]=Cyc_PP_textptr(_tmp204);_tmp261[1]=Cyc_PP_text(({const char*
_tmp262="->";_tag_arr(_tmp262,sizeof(char),_get_zero_arr_size(_tmp262,3));}));
_tmp261[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp203);Cyc_PP_cat(_tag_arr(_tmp261,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1A2;_LL1D7: if(*((int*)_tmp1D6)!= 25)goto
_LL1D9;_tmp205=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D6)->f1;_tmp206=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D6)->f2;_LL1D8: s=({struct Cyc_PP_Doc*
_tmp263[4];_tmp263[3]=Cyc_PP_text(({const char*_tmp265="]";_tag_arr(_tmp265,
sizeof(char),_get_zero_arr_size(_tmp265,2));}));_tmp263[2]=Cyc_Absynpp_exp2doc(
_tmp206);_tmp263[1]=Cyc_PP_text(({const char*_tmp264="[";_tag_arr(_tmp264,sizeof(
char),_get_zero_arr_size(_tmp264,2));}));_tmp263[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp205);Cyc_PP_cat(_tag_arr(_tmp263,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1A2;_LL1D9: if(*((int*)_tmp1D6)!= 26)goto _LL1DB;_tmp207=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D6)->f1;_LL1DA: s=({struct Cyc_PP_Doc*_tmp266[4];_tmp266[3]=Cyc_PP_text(({
const char*_tmp268=")";_tag_arr(_tmp268,sizeof(char),_get_zero_arr_size(_tmp268,2));}));
_tmp266[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp207);_tmp266[1]=Cyc_PP_text(({const
char*_tmp267="(";_tag_arr(_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,2));}));
_tmp266[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp266,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL1A2;_LL1DB: if(*((int*)_tmp1D6)!= 27)goto _LL1DD;_tmp208=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D6)->f1;_tmp209=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D6)->f2;_LL1DC:
s=({struct Cyc_PP_Doc*_tmp269[4];_tmp269[3]=Cyc_Absynpp_group_braces(({const char*
_tmp26C=",";_tag_arr(_tmp26C,sizeof(char),_get_zero_arr_size(_tmp26C,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp209));_tmp269[2]=Cyc_PP_text(({const char*
_tmp26B=")";_tag_arr(_tmp26B,sizeof(char),_get_zero_arr_size(_tmp26B,2));}));
_tmp269[1]=Cyc_Absynpp_typ2doc((*_tmp208).f3);_tmp269[0]=Cyc_PP_text(({const char*
_tmp26A="(";_tag_arr(_tmp26A,sizeof(char),_get_zero_arr_size(_tmp26A,2));}));Cyc_PP_cat(
_tag_arr(_tmp269,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1A2;_LL1DD: if(*((int*)
_tmp1D6)!= 28)goto _LL1DF;_tmp20A=((struct Cyc_Absyn_Array_e_struct*)_tmp1D6)->f1;
_LL1DE: s=Cyc_Absynpp_group_braces(({const char*_tmp26D=",";_tag_arr(_tmp26D,
sizeof(char),_get_zero_arr_size(_tmp26D,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20A));goto _LL1A2;_LL1DF: if(*((int*)_tmp1D6)!= 29)goto _LL1E1;_tmp20B=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp1D6)->f1;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D6)->f2;_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D6)->f3;
_LL1E0: s=({struct Cyc_PP_Doc*_tmp26E[8];_tmp26E[7]=Cyc_Absynpp_rb();_tmp26E[6]=
Cyc_Absynpp_exp2doc(_tmp20D);_tmp26E[5]=Cyc_PP_text(({const char*_tmp271=" : ";
_tag_arr(_tmp271,sizeof(char),_get_zero_arr_size(_tmp271,4));}));_tmp26E[4]=Cyc_Absynpp_exp2doc(
_tmp20C);_tmp26E[3]=Cyc_PP_text(({const char*_tmp270=" < ";_tag_arr(_tmp270,
sizeof(char),_get_zero_arr_size(_tmp270,4));}));_tmp26E[2]=Cyc_PP_text(*(*
_tmp20B->name).f2);_tmp26E[1]=Cyc_PP_text(({const char*_tmp26F="for ";_tag_arr(
_tmp26F,sizeof(char),_get_zero_arr_size(_tmp26F,5));}));_tmp26E[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp26E,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1A2;_LL1E1:
if(*((int*)_tmp1D6)!= 30)goto _LL1E3;_tmp20E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D6)->f1;_tmp20F=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D6)->f2;_tmp210=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1D6)->f3;_LL1E2: {struct Cyc_List_List*
_tmp272=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp210);s=({struct Cyc_PP_Doc*
_tmp273[2];_tmp273[1]=Cyc_Absynpp_group_braces(({const char*_tmp274=",";_tag_arr(
_tmp274,sizeof(char),_get_zero_arr_size(_tmp274,2));}),_tmp20F != 0?({struct Cyc_List_List*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->hd=Cyc_Absynpp_tps2doc(_tmp20F);
_tmp275->tl=_tmp272;_tmp275;}): _tmp272);_tmp273[0]=Cyc_Absynpp_qvar2doc(_tmp20E);
Cyc_PP_cat(_tag_arr(_tmp273,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1A2;}_LL1E3:
if(*((int*)_tmp1D6)!= 31)goto _LL1E5;_tmp211=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D6)->f2;_LL1E4: s=Cyc_Absynpp_group_braces(({const char*_tmp276=",";_tag_arr(
_tmp276,sizeof(char),_get_zero_arr_size(_tmp276,2));}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp211));goto _LL1A2;_LL1E5: if(*((int*)_tmp1D6)!= 32)goto _LL1E7;_tmp212=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp1D6)->f1;_tmp213=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1D6)->f3;_LL1E6: if(_tmp212 == 0)s=Cyc_Absynpp_qvar2doc(_tmp213->name);else{s=({
struct Cyc_PP_Doc*_tmp277[2];_tmp277[1]=Cyc_PP_egroup(({const char*_tmp278="(";
_tag_arr(_tmp278,sizeof(char),_get_zero_arr_size(_tmp278,2));}),({const char*
_tmp279=")";_tag_arr(_tmp279,sizeof(char),_get_zero_arr_size(_tmp279,2));}),({
const char*_tmp27A=",";_tag_arr(_tmp27A,sizeof(char),_get_zero_arr_size(_tmp27A,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp212));_tmp277[0]=Cyc_Absynpp_qvar2doc(
_tmp213->name);Cyc_PP_cat(_tag_arr(_tmp277,sizeof(struct Cyc_PP_Doc*),2));});}
goto _LL1A2;_LL1E7: if(*((int*)_tmp1D6)!= 33)goto _LL1E9;_tmp214=((struct Cyc_Absyn_Enum_e_struct*)
_tmp1D6)->f1;_LL1E8: s=Cyc_Absynpp_qvar2doc(_tmp214);goto _LL1A2;_LL1E9: if(*((int*)
_tmp1D6)!= 34)goto _LL1EB;_tmp215=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D6)->f1;
_LL1EA: s=Cyc_Absynpp_qvar2doc(_tmp215);goto _LL1A2;_LL1EB: if(*((int*)_tmp1D6)!= 
35)goto _LL1ED;_tmp216=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D6)->f1;_tmp217=
_tmp216.is_calloc;_tmp218=_tmp216.rgn;_tmp219=_tmp216.elt_type;_tmp21A=_tmp216.num_elts;
_LL1EC: if(_tmp217){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmp219)),0);if(_tmp218 == 0)s=({struct Cyc_PP_Doc*_tmp27B[5];_tmp27B[
4]=Cyc_PP_text(({const char*_tmp27E=")";_tag_arr(_tmp27E,sizeof(char),
_get_zero_arr_size(_tmp27E,2));}));_tmp27B[3]=Cyc_Absynpp_exp2doc(st);_tmp27B[2]=
Cyc_PP_text(({const char*_tmp27D=",";_tag_arr(_tmp27D,sizeof(char),
_get_zero_arr_size(_tmp27D,2));}));_tmp27B[1]=Cyc_Absynpp_exp2doc(_tmp21A);
_tmp27B[0]=Cyc_PP_text(({const char*_tmp27C="calloc(";_tag_arr(_tmp27C,sizeof(
char),_get_zero_arr_size(_tmp27C,8));}));Cyc_PP_cat(_tag_arr(_tmp27B,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp27F[7];_tmp27F[6]=Cyc_PP_text(({
const char*_tmp283=")";_tag_arr(_tmp283,sizeof(char),_get_zero_arr_size(_tmp283,2));}));
_tmp27F[5]=Cyc_Absynpp_exp2doc(st);_tmp27F[4]=Cyc_PP_text(({const char*_tmp282=",";
_tag_arr(_tmp282,sizeof(char),_get_zero_arr_size(_tmp282,2));}));_tmp27F[3]=Cyc_Absynpp_exp2doc(
_tmp21A);_tmp27F[2]=Cyc_PP_text(({const char*_tmp281=",";_tag_arr(_tmp281,sizeof(
char),_get_zero_arr_size(_tmp281,2));}));_tmp27F[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp218);_tmp27F[0]=Cyc_PP_text(({const char*_tmp280="rcalloc(";
_tag_arr(_tmp280,sizeof(char),_get_zero_arr_size(_tmp280,9));}));Cyc_PP_cat(
_tag_arr(_tmp27F,sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*
new_e;if(_tmp219 == 0)new_e=_tmp21A;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*
_tmp219,0),_tmp21A,0);}if(_tmp218 == 0)s=({struct Cyc_PP_Doc*_tmp284[3];_tmp284[2]=
Cyc_PP_text(({const char*_tmp286=")";_tag_arr(_tmp286,sizeof(char),
_get_zero_arr_size(_tmp286,2));}));_tmp284[1]=Cyc_Absynpp_exp2doc(new_e);_tmp284[
0]=Cyc_PP_text(({const char*_tmp285="malloc(";_tag_arr(_tmp285,sizeof(char),
_get_zero_arr_size(_tmp285,8));}));Cyc_PP_cat(_tag_arr(_tmp284,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp287[5];_tmp287[4]=Cyc_PP_text(({const char*
_tmp28A=")";_tag_arr(_tmp28A,sizeof(char),_get_zero_arr_size(_tmp28A,2));}));
_tmp287[3]=Cyc_Absynpp_exp2doc(new_e);_tmp287[2]=Cyc_PP_text(({const char*_tmp289=",";
_tag_arr(_tmp289,sizeof(char),_get_zero_arr_size(_tmp289,2));}));_tmp287[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp218);_tmp287[0]=Cyc_PP_text(({const char*_tmp288="rmalloc(";
_tag_arr(_tmp288,sizeof(char),_get_zero_arr_size(_tmp288,9));}));Cyc_PP_cat(
_tag_arr(_tmp287,sizeof(struct Cyc_PP_Doc*),5));});}}goto _LL1A2;_LL1ED: if(*((int*)
_tmp1D6)!= 36)goto _LL1EF;_tmp21B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D6)->f1;_tmp21C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D6)->f2;
_LL1EE: s=Cyc_Absynpp_group_braces(({const char*_tmp28B=",";_tag_arr(_tmp28B,
sizeof(char),_get_zero_arr_size(_tmp28B,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp21C));goto _LL1A2;_LL1EF: if(*((int*)_tmp1D6)!= 37)goto _LL1A2;_tmp21D=((struct
Cyc_Absyn_StmtExp_e_struct*)_tmp1D6)->f1;_LL1F0: s=({struct Cyc_PP_Doc*_tmp28C[7];
_tmp28C[6]=Cyc_PP_text(({const char*_tmp28E=")";_tag_arr(_tmp28E,sizeof(char),
_get_zero_arr_size(_tmp28E,2));}));_tmp28C[5]=Cyc_Absynpp_rb();_tmp28C[4]=Cyc_PP_blank_doc();
_tmp28C[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp21D));_tmp28C[2]=Cyc_PP_blank_doc();
_tmp28C[1]=Cyc_Absynpp_lb();_tmp28C[0]=Cyc_PP_text(({const char*_tmp28D="(";
_tag_arr(_tmp28D,sizeof(char),_get_zero_arr_size(_tmp28D,2));}));Cyc_PP_cat(
_tag_arr(_tmp28C,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1A2;_LL1A2:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp28F[3];_tmp28F[2]=Cyc_PP_text(({const char*
_tmp291=")";_tag_arr(_tmp291,sizeof(char),_get_zero_arr_size(_tmp291,2));}));
_tmp28F[1]=s;_tmp28F[0]=Cyc_PP_text(({const char*_tmp290="(";_tag_arr(_tmp290,
sizeof(char),_get_zero_arr_size(_tmp290,2));}));Cyc_PP_cat(_tag_arr(_tmp28F,
sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp292=d;struct Cyc_Absyn_Exp*_tmp293;struct _tagged_arr*_tmp294;
_LL1FB: if(*((int*)_tmp292)!= 0)goto _LL1FD;_tmp293=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp292)->f1;_LL1FC: return({struct Cyc_PP_Doc*_tmp295[3];_tmp295[2]=Cyc_PP_text(({
const char*_tmp297="]";_tag_arr(_tmp297,sizeof(char),_get_zero_arr_size(_tmp297,2));}));
_tmp295[1]=Cyc_Absynpp_exp2doc(_tmp293);_tmp295[0]=Cyc_PP_text(({const char*
_tmp296=".[";_tag_arr(_tmp296,sizeof(char),_get_zero_arr_size(_tmp296,3));}));
Cyc_PP_cat(_tag_arr(_tmp295,sizeof(struct Cyc_PP_Doc*),3));});_LL1FD: if(*((int*)
_tmp292)!= 1)goto _LL1FA;_tmp294=((struct Cyc_Absyn_FieldName_struct*)_tmp292)->f1;
_LL1FE: return({struct Cyc_PP_Doc*_tmp298[2];_tmp298[1]=Cyc_PP_textptr(_tmp294);
_tmp298[0]=Cyc_PP_text(({const char*_tmp299=".";_tag_arr(_tmp299,sizeof(char),
_get_zero_arr_size(_tmp299,2));}));Cyc_PP_cat(_tag_arr(_tmp298,sizeof(struct Cyc_PP_Doc*),
2));});_LL1FA:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1
== 0)return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp29A[2];
_tmp29A[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp29A[0]=Cyc_PP_egroup(({const char*
_tmp29B="";_tag_arr(_tmp29B,sizeof(char),_get_zero_arr_size(_tmp29B,1));}),({
const char*_tmp29C="=";_tag_arr(_tmp29C,sizeof(char),_get_zero_arr_size(_tmp29C,2));}),({
const char*_tmp29D="=";_tag_arr(_tmp29D,sizeof(char),_get_zero_arr_size(_tmp29D,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp29A,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp29E="";_tag_arr(
_tmp29E,sizeof(char),_get_zero_arr_size(_tmp29E,1));}),({const char*_tmp29F="";
_tag_arr(_tmp29F,sizeof(char),_get_zero_arr_size(_tmp29F,1));}),({const char*
_tmp2A0=",";_tag_arr(_tmp2A0,sizeof(char),_get_zero_arr_size(_tmp2A0,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp2A1=c;void*_tmp2A2;char
_tmp2A3;void*_tmp2A4;short _tmp2A5;void*_tmp2A6;int _tmp2A7;void*_tmp2A8;int
_tmp2A9;void*_tmp2AA;int _tmp2AB;void*_tmp2AC;long long _tmp2AD;struct _tagged_arr
_tmp2AE;struct _tagged_arr _tmp2AF;_LL200: if(_tmp2A1 <= (void*)1  || *((int*)_tmp2A1)
!= 0)goto _LL202;_tmp2A2=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp2A1)->f1;
_tmp2A3=((struct Cyc_Absyn_Char_c_struct*)_tmp2A1)->f2;_LL201: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp2A3));{void*
_tmp2B0[1]={& _tmp2B2};Cyc_aprintf(({const char*_tmp2B1="'%s'";_tag_arr(_tmp2B1,
sizeof(char),_get_zero_arr_size(_tmp2B1,5));}),_tag_arr(_tmp2B0,sizeof(void*),1));}}));
_LL202: if(_tmp2A1 <= (void*)1  || *((int*)_tmp2A1)!= 1)goto _LL204;_tmp2A4=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp2A1)->f1;_tmp2A5=((struct Cyc_Absyn_Short_c_struct*)
_tmp2A1)->f2;_LL203: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp2B5;_tmp2B5.tag=1;_tmp2B5.f1=(unsigned int)((int)_tmp2A5);{void*_tmp2B3[1]={&
_tmp2B5};Cyc_aprintf(({const char*_tmp2B4="%d";_tag_arr(_tmp2B4,sizeof(char),
_get_zero_arr_size(_tmp2B4,3));}),_tag_arr(_tmp2B3,sizeof(void*),1));}}));_LL204:
if(_tmp2A1 <= (void*)1  || *((int*)_tmp2A1)!= 2)goto _LL206;_tmp2A6=(void*)((struct
Cyc_Absyn_Int_c_struct*)_tmp2A1)->f1;if((int)_tmp2A6 != 2)goto _LL206;_tmp2A7=((
struct Cyc_Absyn_Int_c_struct*)_tmp2A1)->f2;_LL205: _tmp2A9=_tmp2A7;goto _LL207;
_LL206: if(_tmp2A1 <= (void*)1  || *((int*)_tmp2A1)!= 2)goto _LL208;_tmp2A8=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp2A1)->f1;if((int)_tmp2A8 != 0)goto _LL208;
_tmp2A9=((struct Cyc_Absyn_Int_c_struct*)_tmp2A1)->f2;_LL207: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2B8;_tmp2B8.tag=1;_tmp2B8.f1=(
unsigned int)_tmp2A9;{void*_tmp2B6[1]={& _tmp2B8};Cyc_aprintf(({const char*_tmp2B7="%d";
_tag_arr(_tmp2B7,sizeof(char),_get_zero_arr_size(_tmp2B7,3));}),_tag_arr(_tmp2B6,
sizeof(void*),1));}}));_LL208: if(_tmp2A1 <= (void*)1  || *((int*)_tmp2A1)!= 2)goto
_LL20A;_tmp2AA=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2A1)->f1;if((int)
_tmp2AA != 1)goto _LL20A;_tmp2AB=((struct Cyc_Absyn_Int_c_struct*)_tmp2A1)->f2;
_LL209: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct _tmp2BB;
_tmp2BB.tag=1;_tmp2BB.f1=(unsigned int)_tmp2AB;{void*_tmp2B9[1]={& _tmp2BB};Cyc_aprintf(({
const char*_tmp2BA="%u";_tag_arr(_tmp2BA,sizeof(char),_get_zero_arr_size(_tmp2BA,
3));}),_tag_arr(_tmp2B9,sizeof(void*),1));}}));_LL20A: if(_tmp2A1 <= (void*)1  || *((
int*)_tmp2A1)!= 3)goto _LL20C;_tmp2AC=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp2A1)->f1;_tmp2AD=((struct Cyc_Absyn_LongLong_c_struct*)_tmp2A1)->f2;_LL20B:
return Cyc_PP_text(({const char*_tmp2BC="<<FIX LONG LONG CONSTANT>>";_tag_arr(
_tmp2BC,sizeof(char),_get_zero_arr_size(_tmp2BC,27));}));_LL20C: if(_tmp2A1 <= (
void*)1  || *((int*)_tmp2A1)!= 4)goto _LL20E;_tmp2AE=((struct Cyc_Absyn_Float_c_struct*)
_tmp2A1)->f1;_LL20D: return Cyc_PP_text(_tmp2AE);_LL20E: if((int)_tmp2A1 != 0)goto
_LL210;_LL20F: return Cyc_PP_text(({const char*_tmp2BD="NULL";_tag_arr(_tmp2BD,
sizeof(char),_get_zero_arr_size(_tmp2BD,5));}));_LL210: if(_tmp2A1 <= (void*)1  || *((
int*)_tmp2A1)!= 5)goto _LL1FF;_tmp2AF=((struct Cyc_Absyn_String_c_struct*)_tmp2A1)->f1;
_LL211: return({struct Cyc_PP_Doc*_tmp2BE[3];_tmp2BE[2]=Cyc_PP_text(({const char*
_tmp2C0="\"";_tag_arr(_tmp2C0,sizeof(char),_get_zero_arr_size(_tmp2C0,2));}));
_tmp2BE[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2AF));_tmp2BE[0]=Cyc_PP_text(({
const char*_tmp2BF="\"";_tag_arr(_tmp2BF,sizeof(char),_get_zero_arr_size(_tmp2BF,
2));}));Cyc_PP_cat(_tag_arr(_tmp2BE,sizeof(struct Cyc_PP_Doc*),3));});_LL1FF:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es == 0
 || es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C1=
_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=({struct Cyc_Core_Failure_struct _tmp2C2;
_tmp2C2.tag=Cyc_Core_Failure;_tmp2C2.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2C5;_tmp2C5.tag=0;_tmp2C5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2C3[1]={& _tmp2C5};Cyc_aprintf(({const char*_tmp2C4="Absynpp::primapp2doc Size: %s with bad args";
_tag_arr(_tmp2C4,sizeof(char),_get_zero_arr_size(_tmp2C4,44));}),_tag_arr(
_tmp2C3,sizeof(void*),1));}});_tmp2C2;});_tmp2C1;}));{struct Cyc_PP_Doc*_tmp2C6=
Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*
_tmp2C7[2];_tmp2C7[1]=Cyc_PP_text(({const char*_tmp2C8=".size";_tag_arr(_tmp2C8,
sizeof(char),_get_zero_arr_size(_tmp2C8,6));}));_tmp2C7[0]=_tmp2C6;Cyc_PP_cat(
_tag_arr(_tmp2C7,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2C9=_cycalloc(
sizeof(*_tmp2C9));_tmp2C9[0]=({struct Cyc_Core_Failure_struct _tmp2CA;_tmp2CA.tag=
Cyc_Core_Failure;_tmp2CA.f1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72));{void*_tmp2CB[1]={& _tmp2CD};Cyc_aprintf(({const char*_tmp2CC="Absynpp::primapp2doc: %s with no args";
_tag_arr(_tmp2CC,sizeof(char),_get_zero_arr_size(_tmp2CC,38));}),_tag_arr(
_tmp2CB,sizeof(void*),1));}});_tmp2CA;});_tmp2C9;}));else{if(ds->tl == 0)return({
struct Cyc_PP_Doc*_tmp2CE[3];_tmp2CE[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp2CE[1]=Cyc_PP_text(({
const char*_tmp2CF=" ";_tag_arr(_tmp2CF,sizeof(char),_get_zero_arr_size(_tmp2CF,2));}));
_tmp2CE[0]=ps;Cyc_PP_cat(_tag_arr(_tmp2CE,sizeof(struct Cyc_PP_Doc*),3));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0[0]=({
struct Cyc_Core_Failure_struct _tmp2D1;_tmp2D1.tag=Cyc_Core_Failure;_tmp2D1.f1=(
struct _tagged_arr)({struct Cyc_String_pa_struct _tmp2D4;_tmp2D4.tag=0;_tmp2D4.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_PP_string_of_doc(ps,72));{void*_tmp2D2[
1]={& _tmp2D4};Cyc_aprintf(({const char*_tmp2D3="Absynpp::primapp2doc: %s with more than 2 args";
_tag_arr(_tmp2D3,sizeof(char),_get_zero_arr_size(_tmp2D3,47));}),_tag_arr(
_tmp2D2,sizeof(void*),1));}});_tmp2D1;});_tmp2D0;}));else{return({struct Cyc_PP_Doc*
_tmp2D5[5];_tmp2D5[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;
_tmp2D5[3]=Cyc_PP_text(({const char*_tmp2D7=" ";_tag_arr(_tmp2D7,sizeof(char),
_get_zero_arr_size(_tmp2D7,2));}));_tmp2D5[2]=ps;_tmp2D5[1]=Cyc_PP_text(({const
char*_tmp2D6=" ";_tag_arr(_tmp2D6,sizeof(char),_get_zero_arr_size(_tmp2D6,2));}));
_tmp2D5[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp2D5,sizeof(struct
Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){void*
_tmp2D8=p;_LL213: if((int)_tmp2D8 != 0)goto _LL215;_LL214: return({const char*_tmp2D9="+";
_tag_arr(_tmp2D9,sizeof(char),_get_zero_arr_size(_tmp2D9,2));});_LL215: if((int)
_tmp2D8 != 1)goto _LL217;_LL216: return({const char*_tmp2DA="*";_tag_arr(_tmp2DA,
sizeof(char),_get_zero_arr_size(_tmp2DA,2));});_LL217: if((int)_tmp2D8 != 2)goto
_LL219;_LL218: return({const char*_tmp2DB="-";_tag_arr(_tmp2DB,sizeof(char),
_get_zero_arr_size(_tmp2DB,2));});_LL219: if((int)_tmp2D8 != 3)goto _LL21B;_LL21A:
return({const char*_tmp2DC="/";_tag_arr(_tmp2DC,sizeof(char),_get_zero_arr_size(
_tmp2DC,2));});_LL21B: if((int)_tmp2D8 != 4)goto _LL21D;_LL21C: return({const char*
_tmp2DD="%";_tag_arr(_tmp2DD,sizeof(char),_get_zero_arr_size(_tmp2DD,2));});
_LL21D: if((int)_tmp2D8 != 5)goto _LL21F;_LL21E: return({const char*_tmp2DE="==";
_tag_arr(_tmp2DE,sizeof(char),_get_zero_arr_size(_tmp2DE,3));});_LL21F: if((int)
_tmp2D8 != 6)goto _LL221;_LL220: return({const char*_tmp2DF="!=";_tag_arr(_tmp2DF,
sizeof(char),_get_zero_arr_size(_tmp2DF,3));});_LL221: if((int)_tmp2D8 != 7)goto
_LL223;_LL222: return({const char*_tmp2E0=">";_tag_arr(_tmp2E0,sizeof(char),
_get_zero_arr_size(_tmp2E0,2));});_LL223: if((int)_tmp2D8 != 8)goto _LL225;_LL224:
return({const char*_tmp2E1="<";_tag_arr(_tmp2E1,sizeof(char),_get_zero_arr_size(
_tmp2E1,2));});_LL225: if((int)_tmp2D8 != 9)goto _LL227;_LL226: return({const char*
_tmp2E2=">=";_tag_arr(_tmp2E2,sizeof(char),_get_zero_arr_size(_tmp2E2,3));});
_LL227: if((int)_tmp2D8 != 10)goto _LL229;_LL228: return({const char*_tmp2E3="<=";
_tag_arr(_tmp2E3,sizeof(char),_get_zero_arr_size(_tmp2E3,3));});_LL229: if((int)
_tmp2D8 != 11)goto _LL22B;_LL22A: return({const char*_tmp2E4="!";_tag_arr(_tmp2E4,
sizeof(char),_get_zero_arr_size(_tmp2E4,2));});_LL22B: if((int)_tmp2D8 != 12)goto
_LL22D;_LL22C: return({const char*_tmp2E5="~";_tag_arr(_tmp2E5,sizeof(char),
_get_zero_arr_size(_tmp2E5,2));});_LL22D: if((int)_tmp2D8 != 13)goto _LL22F;_LL22E:
return({const char*_tmp2E6="&";_tag_arr(_tmp2E6,sizeof(char),_get_zero_arr_size(
_tmp2E6,2));});_LL22F: if((int)_tmp2D8 != 14)goto _LL231;_LL230: return({const char*
_tmp2E7="|";_tag_arr(_tmp2E7,sizeof(char),_get_zero_arr_size(_tmp2E7,2));});
_LL231: if((int)_tmp2D8 != 15)goto _LL233;_LL232: return({const char*_tmp2E8="^";
_tag_arr(_tmp2E8,sizeof(char),_get_zero_arr_size(_tmp2E8,2));});_LL233: if((int)
_tmp2D8 != 16)goto _LL235;_LL234: return({const char*_tmp2E9="<<";_tag_arr(_tmp2E9,
sizeof(char),_get_zero_arr_size(_tmp2E9,3));});_LL235: if((int)_tmp2D8 != 17)goto
_LL237;_LL236: return({const char*_tmp2EA=">>";_tag_arr(_tmp2EA,sizeof(char),
_get_zero_arr_size(_tmp2EA,3));});_LL237: if((int)_tmp2D8 != 18)goto _LL239;_LL238:
return({const char*_tmp2EB=">>>";_tag_arr(_tmp2EB,sizeof(char),_get_zero_arr_size(
_tmp2EB,4));});_LL239: if((int)_tmp2D8 != 19)goto _LL212;_LL23A: return({const char*
_tmp2EC="size";_tag_arr(_tmp2EC,sizeof(char),_get_zero_arr_size(_tmp2EC,5));});
_LL212:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){void*_tmp2ED=(void*)s->r;
_LL23C: if(_tmp2ED <= (void*)1  || *((int*)_tmp2ED)!= 11)goto _LL23E;_LL23D: return 1;
_LL23E:;_LL23F: return 0;_LL23B:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp2EE=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2EF;
struct Cyc_Absyn_Stmt*_tmp2F0;struct Cyc_Absyn_Stmt*_tmp2F1;struct Cyc_Absyn_Exp*
_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;struct Cyc_Absyn_Stmt*_tmp2F4;struct Cyc_Absyn_Stmt*
_tmp2F5;struct _tuple2 _tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Stmt*
_tmp2F8;struct _tagged_arr*_tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;struct _tuple2
_tmp2FB;struct Cyc_Absyn_Exp*_tmp2FC;struct _tuple2 _tmp2FD;struct Cyc_Absyn_Exp*
_tmp2FE;struct Cyc_Absyn_Stmt*_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_List_List*
_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;struct Cyc_Absyn_Decl*
_tmp304;struct Cyc_Absyn_Stmt*_tmp305;struct _tagged_arr*_tmp306;struct Cyc_Absyn_Stmt*
_tmp307;struct Cyc_Absyn_Stmt*_tmp308;struct _tuple2 _tmp309;struct Cyc_Absyn_Exp*
_tmp30A;struct Cyc_Absyn_Stmt*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_Tvar*
_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30E;int _tmp30F;struct Cyc_Absyn_Stmt*_tmp310;
struct Cyc_Absyn_Exp*_tmp311;_LL241: if((int)_tmp2EE != 0)goto _LL243;_LL242: s=Cyc_PP_text(({
const char*_tmp312=";";_tag_arr(_tmp312,sizeof(char),_get_zero_arr_size(_tmp312,2));}));
goto _LL240;_LL243: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 0)goto _LL245;
_tmp2EF=((struct Cyc_Absyn_Exp_s_struct*)_tmp2EE)->f1;_LL244: s=({struct Cyc_PP_Doc*
_tmp313[2];_tmp313[1]=Cyc_PP_text(({const char*_tmp314=";";_tag_arr(_tmp314,
sizeof(char),_get_zero_arr_size(_tmp314,2));}));_tmp313[0]=Cyc_Absynpp_exp2doc(
_tmp2EF);Cyc_PP_cat(_tag_arr(_tmp313,sizeof(struct Cyc_PP_Doc*),2));});goto _LL240;
_LL245: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 1)goto _LL247;_tmp2F0=((struct
Cyc_Absyn_Seq_s_struct*)_tmp2EE)->f1;_tmp2F1=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2EE)->f2;_LL246: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2F0))s=({struct Cyc_PP_Doc*_tmp315[7];_tmp315[6]=Cyc_Absynpp_is_declaration(
_tmp2F1)?({struct Cyc_PP_Doc*_tmp316[5];_tmp316[4]=Cyc_PP_line_doc();_tmp316[3]=
Cyc_Absynpp_rb();_tmp316[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F1));_tmp316[
1]=Cyc_PP_blank_doc();_tmp316[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_arr(_tmp316,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(_tmp2F1);_tmp315[5]=Cyc_PP_line_doc();
_tmp315[4]=Cyc_Absynpp_rb();_tmp315[3]=Cyc_PP_line_doc();_tmp315[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2F0));_tmp315[1]=Cyc_PP_blank_doc();_tmp315[0]=Cyc_Absynpp_lb();
Cyc_PP_cat(_tag_arr(_tmp315,sizeof(struct Cyc_PP_Doc*),7));});else{if(Cyc_Absynpp_is_declaration(
_tmp2F1))s=({struct Cyc_PP_Doc*_tmp317[7];_tmp317[6]=Cyc_PP_line_doc();_tmp317[5]=
Cyc_Absynpp_rb();_tmp317[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F1));_tmp317[
3]=Cyc_PP_blank_doc();_tmp317[2]=Cyc_Absynpp_lb();_tmp317[1]=Cyc_PP_line_doc();
_tmp317[0]=Cyc_Absynpp_stmt2doc(_tmp2F0);Cyc_PP_cat(_tag_arr(_tmp317,sizeof(
struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,({const char*_tmp318="";_tag_arr(_tmp318,sizeof(char),
_get_zero_arr_size(_tmp318,1));}),({struct Cyc_List_List*_tmp319=_cycalloc(
sizeof(*_tmp319));_tmp319->hd=_tmp2F0;_tmp319->tl=({struct Cyc_List_List*_tmp31A=
_cycalloc(sizeof(*_tmp31A));_tmp31A->hd=_tmp2F1;_tmp31A->tl=0;_tmp31A;});_tmp319;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp31B="";_tag_arr(_tmp31B,sizeof(char),_get_zero_arr_size(_tmp31B,1));}),({
struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->hd=_tmp2F0;
_tmp31C->tl=({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->hd=
_tmp2F1;_tmp31D->tl=0;_tmp31D;});_tmp31C;}));}goto _LL240;_LL247: if(_tmp2EE <= (
void*)1  || *((int*)_tmp2EE)!= 2)goto _LL249;_tmp2F2=((struct Cyc_Absyn_Return_s_struct*)
_tmp2EE)->f1;_LL248: if(_tmp2F2 == 0)s=Cyc_PP_text(({const char*_tmp31E="return;";
_tag_arr(_tmp31E,sizeof(char),_get_zero_arr_size(_tmp31E,8));}));else{s=({struct
Cyc_PP_Doc*_tmp31F[3];_tmp31F[2]=Cyc_PP_text(({const char*_tmp321=";";_tag_arr(
_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,2));}));_tmp31F[1]=_tmp2F2 == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2F2);_tmp31F[0]=
Cyc_PP_text(({const char*_tmp320="return ";_tag_arr(_tmp320,sizeof(char),
_get_zero_arr_size(_tmp320,8));}));Cyc_PP_cat(_tag_arr(_tmp31F,sizeof(struct Cyc_PP_Doc*),
3));});}goto _LL240;_LL249: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 3)goto
_LL24B;_tmp2F3=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2EE)->f1;_tmp2F4=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2EE)->f2;_tmp2F5=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2EE)->f3;_LL24A: {int print_else;{void*_tmp322=(void*)_tmp2F5->r;_LL268: if((
int)_tmp322 != 0)goto _LL26A;_LL269: print_else=0;goto _LL267;_LL26A:;_LL26B:
print_else=1;goto _LL267;_LL267:;}s=({struct Cyc_PP_Doc*_tmp323[8];_tmp323[7]=
print_else?({struct Cyc_PP_Doc*_tmp327[6];_tmp327[5]=Cyc_Absynpp_rb();_tmp327[4]=
Cyc_PP_line_doc();_tmp327[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp329[2];_tmp329[
1]=Cyc_Absynpp_stmt2doc(_tmp2F5);_tmp329[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp329,sizeof(struct Cyc_PP_Doc*),2));}));_tmp327[2]=Cyc_Absynpp_lb();
_tmp327[1]=Cyc_PP_text(({const char*_tmp328="else ";_tag_arr(_tmp328,sizeof(char),
_get_zero_arr_size(_tmp328,6));}));_tmp327[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp327,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp323[6]=
Cyc_Absynpp_rb();_tmp323[5]=Cyc_PP_line_doc();_tmp323[4]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp326[2];_tmp326[1]=Cyc_Absynpp_stmt2doc(_tmp2F4);_tmp326[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp326,sizeof(struct Cyc_PP_Doc*),2));}));_tmp323[3]=Cyc_Absynpp_lb();
_tmp323[2]=Cyc_PP_text(({const char*_tmp325=") ";_tag_arr(_tmp325,sizeof(char),
_get_zero_arr_size(_tmp325,3));}));_tmp323[1]=Cyc_Absynpp_exp2doc(_tmp2F3);
_tmp323[0]=Cyc_PP_text(({const char*_tmp324="if (";_tag_arr(_tmp324,sizeof(char),
_get_zero_arr_size(_tmp324,5));}));Cyc_PP_cat(_tag_arr(_tmp323,sizeof(struct Cyc_PP_Doc*),
8));});goto _LL240;}_LL24B: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 4)goto
_LL24D;_tmp2F6=((struct Cyc_Absyn_While_s_struct*)_tmp2EE)->f1;_tmp2F7=_tmp2F6.f1;
_tmp2F8=((struct Cyc_Absyn_While_s_struct*)_tmp2EE)->f2;_LL24C: s=({struct Cyc_PP_Doc*
_tmp32A[7];_tmp32A[6]=Cyc_Absynpp_rb();_tmp32A[5]=Cyc_PP_line_doc();_tmp32A[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp32D[2];_tmp32D[1]=Cyc_Absynpp_stmt2doc(
_tmp2F8);_tmp32D[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp32D,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp32A[3]=Cyc_Absynpp_lb();_tmp32A[2]=Cyc_PP_text(({const
char*_tmp32C=") ";_tag_arr(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,3));}));
_tmp32A[1]=Cyc_Absynpp_exp2doc(_tmp2F7);_tmp32A[0]=Cyc_PP_text(({const char*
_tmp32B="while (";_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,8));}));
Cyc_PP_cat(_tag_arr(_tmp32A,sizeof(struct Cyc_PP_Doc*),7));});goto _LL240;_LL24D:
if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 5)goto _LL24F;_LL24E: s=Cyc_PP_text(({
const char*_tmp32E="break;";_tag_arr(_tmp32E,sizeof(char),_get_zero_arr_size(
_tmp32E,7));}));goto _LL240;_LL24F: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 6)
goto _LL251;_LL250: s=Cyc_PP_text(({const char*_tmp32F="continue;";_tag_arr(_tmp32F,
sizeof(char),_get_zero_arr_size(_tmp32F,10));}));goto _LL240;_LL251: if(_tmp2EE <= (
void*)1  || *((int*)_tmp2EE)!= 7)goto _LL253;_tmp2F9=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2EE)->f1;_LL252: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp332;_tmp332.tag=0;_tmp332.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp2F9);{
void*_tmp330[1]={& _tmp332};Cyc_aprintf(({const char*_tmp331="goto %s;";_tag_arr(
_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,9));}),_tag_arr(_tmp330,sizeof(
void*),1));}}));goto _LL240;_LL253: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 8)
goto _LL255;_tmp2FA=((struct Cyc_Absyn_For_s_struct*)_tmp2EE)->f1;_tmp2FB=((struct
Cyc_Absyn_For_s_struct*)_tmp2EE)->f2;_tmp2FC=_tmp2FB.f1;_tmp2FD=((struct Cyc_Absyn_For_s_struct*)
_tmp2EE)->f3;_tmp2FE=_tmp2FD.f1;_tmp2FF=((struct Cyc_Absyn_For_s_struct*)_tmp2EE)->f4;
_LL254: s=({struct Cyc_PP_Doc*_tmp333[11];_tmp333[10]=Cyc_Absynpp_rb();_tmp333[9]=
Cyc_PP_line_doc();_tmp333[8]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp338[2];_tmp338[
1]=Cyc_Absynpp_stmt2doc(_tmp2FF);_tmp338[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp338,sizeof(struct Cyc_PP_Doc*),2));}));_tmp333[7]=Cyc_Absynpp_lb();
_tmp333[6]=Cyc_PP_text(({const char*_tmp337=") ";_tag_arr(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,3));}));_tmp333[5]=Cyc_Absynpp_exp2doc(_tmp2FE);
_tmp333[4]=Cyc_PP_text(({const char*_tmp336="; ";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,3));}));_tmp333[3]=Cyc_Absynpp_exp2doc(_tmp2FC);
_tmp333[2]=Cyc_PP_text(({const char*_tmp335="; ";_tag_arr(_tmp335,sizeof(char),
_get_zero_arr_size(_tmp335,3));}));_tmp333[1]=Cyc_Absynpp_exp2doc(_tmp2FA);
_tmp333[0]=Cyc_PP_text(({const char*_tmp334="for(";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,5));}));Cyc_PP_cat(_tag_arr(_tmp333,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL240;_LL255: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 9)goto
_LL257;_tmp300=((struct Cyc_Absyn_Switch_s_struct*)_tmp2EE)->f1;_tmp301=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2EE)->f2;_LL256: s=({struct Cyc_PP_Doc*_tmp339[8];
_tmp339[7]=Cyc_Absynpp_rb();_tmp339[6]=Cyc_PP_line_doc();_tmp339[5]=Cyc_Absynpp_switchclauses2doc(
_tmp301);_tmp339[4]=Cyc_PP_line_doc();_tmp339[3]=Cyc_Absynpp_lb();_tmp339[2]=Cyc_PP_text(({
const char*_tmp33B=") ";_tag_arr(_tmp33B,sizeof(char),_get_zero_arr_size(_tmp33B,
3));}));_tmp339[1]=Cyc_Absynpp_exp2doc(_tmp300);_tmp339[0]=Cyc_PP_text(({const
char*_tmp33A="switch (";_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,
9));}));Cyc_PP_cat(_tag_arr(_tmp339,sizeof(struct Cyc_PP_Doc*),8));});goto _LL240;
_LL257: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 10)goto _LL259;_tmp302=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp2EE)->f1;if(_tmp302 != 0)goto _LL259;_LL258:
s=Cyc_PP_text(({const char*_tmp33C="fallthru;";_tag_arr(_tmp33C,sizeof(char),
_get_zero_arr_size(_tmp33C,10));}));goto _LL240;_LL259: if(_tmp2EE <= (void*)1  || *((
int*)_tmp2EE)!= 10)goto _LL25B;_tmp303=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2EE)->f1;_LL25A: s=({struct Cyc_PP_Doc*_tmp33D[3];_tmp33D[2]=Cyc_PP_text(({
const char*_tmp33F=");";_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,
3));}));_tmp33D[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp303);_tmp33D[0]=Cyc_PP_text(({
const char*_tmp33E="fallthru(";_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(
_tmp33E,10));}));Cyc_PP_cat(_tag_arr(_tmp33D,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL240;_LL25B: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 11)goto _LL25D;
_tmp304=((struct Cyc_Absyn_Decl_s_struct*)_tmp2EE)->f1;_tmp305=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2EE)->f2;_LL25C: s=({struct Cyc_PP_Doc*_tmp340[3];_tmp340[2]=Cyc_Absynpp_stmt2doc(
_tmp305);_tmp340[1]=Cyc_PP_line_doc();_tmp340[0]=Cyc_Absynpp_decl2doc(_tmp304);
Cyc_PP_cat(_tag_arr(_tmp340,sizeof(struct Cyc_PP_Doc*),3));});goto _LL240;_LL25D:
if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 12)goto _LL25F;_tmp306=((struct Cyc_Absyn_Label_s_struct*)
_tmp2EE)->f1;_tmp307=((struct Cyc_Absyn_Label_s_struct*)_tmp2EE)->f2;_LL25E: if(
Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp307))s=({struct Cyc_PP_Doc*
_tmp341[7];_tmp341[6]=Cyc_Absynpp_rb();_tmp341[5]=Cyc_PP_line_doc();_tmp341[4]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp307));_tmp341[3]=Cyc_PP_line_doc();
_tmp341[2]=Cyc_Absynpp_lb();_tmp341[1]=Cyc_PP_text(({const char*_tmp342=": ";
_tag_arr(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,3));}));_tmp341[0]=Cyc_PP_textptr(
_tmp306);Cyc_PP_cat(_tag_arr(_tmp341,sizeof(struct Cyc_PP_Doc*),7));});else{s=({
struct Cyc_PP_Doc*_tmp343[3];_tmp343[2]=Cyc_Absynpp_stmt2doc(_tmp307);_tmp343[1]=
Cyc_PP_text(({const char*_tmp344=": ";_tag_arr(_tmp344,sizeof(char),
_get_zero_arr_size(_tmp344,3));}));_tmp343[0]=Cyc_PP_textptr(_tmp306);Cyc_PP_cat(
_tag_arr(_tmp343,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL240;_LL25F: if(_tmp2EE
<= (void*)1  || *((int*)_tmp2EE)!= 13)goto _LL261;_tmp308=((struct Cyc_Absyn_Do_s_struct*)
_tmp2EE)->f1;_tmp309=((struct Cyc_Absyn_Do_s_struct*)_tmp2EE)->f2;_tmp30A=_tmp309.f1;
_LL260: s=({struct Cyc_PP_Doc*_tmp345[9];_tmp345[8]=Cyc_PP_text(({const char*
_tmp348=");";_tag_arr(_tmp348,sizeof(char),_get_zero_arr_size(_tmp348,3));}));
_tmp345[7]=Cyc_Absynpp_exp2doc(_tmp30A);_tmp345[6]=Cyc_PP_text(({const char*
_tmp347=" while (";_tag_arr(_tmp347,sizeof(char),_get_zero_arr_size(_tmp347,9));}));
_tmp345[5]=Cyc_Absynpp_rb();_tmp345[4]=Cyc_PP_line_doc();_tmp345[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp308));_tmp345[2]=Cyc_PP_line_doc();_tmp345[1]=Cyc_Absynpp_lb();
_tmp345[0]=Cyc_PP_text(({const char*_tmp346="do ";_tag_arr(_tmp346,sizeof(char),
_get_zero_arr_size(_tmp346,4));}));Cyc_PP_cat(_tag_arr(_tmp345,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL240;_LL261: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 14)goto
_LL263;_tmp30B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2EE)->f1;_tmp30C=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2EE)->f2;_LL262: s=({struct Cyc_PP_Doc*
_tmp349[12];_tmp349[11]=Cyc_Absynpp_rb();_tmp349[10]=Cyc_PP_line_doc();_tmp349[9]=
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp30C));_tmp349[8]=Cyc_PP_line_doc();
_tmp349[7]=Cyc_Absynpp_lb();_tmp349[6]=Cyc_PP_text(({const char*_tmp34B=" catch ";
_tag_arr(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,8));}));_tmp349[5]=Cyc_Absynpp_rb();
_tmp349[4]=Cyc_PP_line_doc();_tmp349[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp30B));_tmp349[2]=Cyc_PP_line_doc();_tmp349[1]=Cyc_Absynpp_lb();_tmp349[0]=
Cyc_PP_text(({const char*_tmp34A="try ";_tag_arr(_tmp34A,sizeof(char),
_get_zero_arr_size(_tmp34A,5));}));Cyc_PP_cat(_tag_arr(_tmp349,sizeof(struct Cyc_PP_Doc*),
12));});goto _LL240;_LL263: if(_tmp2EE <= (void*)1  || *((int*)_tmp2EE)!= 15)goto
_LL265;_tmp30D=((struct Cyc_Absyn_Region_s_struct*)_tmp2EE)->f1;_tmp30E=((struct
Cyc_Absyn_Region_s_struct*)_tmp2EE)->f2;_tmp30F=((struct Cyc_Absyn_Region_s_struct*)
_tmp2EE)->f3;_tmp310=((struct Cyc_Absyn_Region_s_struct*)_tmp2EE)->f4;_LL264: s=({
struct Cyc_PP_Doc*_tmp34C[11];_tmp34C[10]=Cyc_Absynpp_rb();_tmp34C[9]=Cyc_PP_line_doc();
_tmp34C[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp310));_tmp34C[7]=Cyc_PP_line_doc();
_tmp34C[6]=Cyc_Absynpp_lb();_tmp34C[5]=Cyc_Absynpp_qvar2doc(_tmp30E->name);
_tmp34C[4]=Cyc_PP_text(({const char*_tmp351=">";_tag_arr(_tmp351,sizeof(char),
_get_zero_arr_size(_tmp351,2));}));_tmp34C[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp30D));_tmp34C[2]=Cyc_PP_text(({const char*_tmp350="<";_tag_arr(_tmp350,
sizeof(char),_get_zero_arr_size(_tmp350,2));}));_tmp34C[1]=_tmp30F?Cyc_PP_text(({
const char*_tmp34E="";_tag_arr(_tmp34E,sizeof(char),_get_zero_arr_size(_tmp34E,1));})):
Cyc_PP_text(({const char*_tmp34F="[resetable]";_tag_arr(_tmp34F,sizeof(char),
_get_zero_arr_size(_tmp34F,12));}));_tmp34C[0]=Cyc_PP_text(({const char*_tmp34D="region";
_tag_arr(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,7));}));Cyc_PP_cat(
_tag_arr(_tmp34C,sizeof(struct Cyc_PP_Doc*),11));});goto _LL240;_LL265: if(_tmp2EE
<= (void*)1  || *((int*)_tmp2EE)!= 16)goto _LL240;_tmp311=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp2EE)->f1;_LL266: s=({struct Cyc_PP_Doc*_tmp352[3];_tmp352[2]=Cyc_PP_text(({
const char*_tmp354=");";_tag_arr(_tmp354,sizeof(char),_get_zero_arr_size(_tmp354,
3));}));_tmp352[1]=Cyc_Absynpp_exp2doc(_tmp311);_tmp352[0]=Cyc_PP_text(({const
char*_tmp353="reset_region(";_tag_arr(_tmp353,sizeof(char),_get_zero_arr_size(
_tmp353,14));}));Cyc_PP_cat(_tag_arr(_tmp352,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL240;_LL240:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp355=(void*)p->r;void*_tmp356;int _tmp357;char
_tmp358;struct _tagged_arr _tmp359;struct Cyc_Absyn_Vardecl*_tmp35A;struct Cyc_Absyn_Tvar*
_tmp35B;struct Cyc_Absyn_Vardecl*_tmp35C;struct Cyc_List_List*_tmp35D;int _tmp35E;
struct Cyc_Absyn_Pat*_tmp35F;struct Cyc_Absyn_Vardecl*_tmp360;struct _tuple0*
_tmp361;struct _tuple0*_tmp362;struct Cyc_List_List*_tmp363;int _tmp364;struct Cyc_Absyn_AggrInfo
_tmp365;void*_tmp366;struct Cyc_List_List*_tmp367;struct Cyc_List_List*_tmp368;int
_tmp369;struct Cyc_Absyn_Enumfield*_tmp36A;struct Cyc_Absyn_Enumfield*_tmp36B;
struct Cyc_Absyn_Tunionfield*_tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_Absyn_Tunionfield*
_tmp36E;struct Cyc_List_List*_tmp36F;int _tmp370;struct Cyc_Absyn_Exp*_tmp371;
_LL26D: if((int)_tmp355 != 0)goto _LL26F;_LL26E: s=Cyc_PP_text(({const char*_tmp372="_";
_tag_arr(_tmp372,sizeof(char),_get_zero_arr_size(_tmp372,2));}));goto _LL26C;
_LL26F: if((int)_tmp355 != 1)goto _LL271;_LL270: s=Cyc_PP_text(({const char*_tmp373="NULL";
_tag_arr(_tmp373,sizeof(char),_get_zero_arr_size(_tmp373,5));}));goto _LL26C;
_LL271: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 7)goto _LL273;_tmp356=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp355)->f1;_tmp357=((struct Cyc_Absyn_Int_p_struct*)
_tmp355)->f2;_LL272: if(_tmp356 != (void*)((void*)1))s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Int_pa_struct _tmp376;_tmp376.tag=1;_tmp376.f1=(
unsigned int)_tmp357;{void*_tmp374[1]={& _tmp376};Cyc_aprintf(({const char*_tmp375="%d";
_tag_arr(_tmp375,sizeof(char),_get_zero_arr_size(_tmp375,3));}),_tag_arr(_tmp374,
sizeof(void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Int_pa_struct
_tmp379;_tmp379.tag=1;_tmp379.f1=(unsigned int)_tmp357;{void*_tmp377[1]={&
_tmp379};Cyc_aprintf(({const char*_tmp378="%u";_tag_arr(_tmp378,sizeof(char),
_get_zero_arr_size(_tmp378,3));}),_tag_arr(_tmp377,sizeof(void*),1));}}));}goto
_LL26C;_LL273: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 8)goto _LL275;_tmp358=((
struct Cyc_Absyn_Char_p_struct*)_tmp355)->f1;_LL274: s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_String_pa_struct _tmp37C;_tmp37C.tag=0;_tmp37C.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp358));{void*_tmp37A[
1]={& _tmp37C};Cyc_aprintf(({const char*_tmp37B="'%s'";_tag_arr(_tmp37B,sizeof(
char),_get_zero_arr_size(_tmp37B,5));}),_tag_arr(_tmp37A,sizeof(void*),1));}}));
goto _LL26C;_LL275: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 9)goto _LL277;
_tmp359=((struct Cyc_Absyn_Float_p_struct*)_tmp355)->f1;_LL276: s=Cyc_PP_text(
_tmp359);goto _LL26C;_LL277: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 0)goto
_LL279;_tmp35A=((struct Cyc_Absyn_Var_p_struct*)_tmp355)->f1;_LL278: s=Cyc_Absynpp_qvar2doc(
_tmp35A->name);goto _LL26C;_LL279: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 2)
goto _LL27B;_tmp35B=((struct Cyc_Absyn_TagInt_p_struct*)_tmp355)->f1;_tmp35C=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp355)->f2;_LL27A: s=({struct Cyc_PP_Doc*
_tmp37D[4];_tmp37D[3]=Cyc_PP_text(({const char*_tmp37F=">";_tag_arr(_tmp37F,
sizeof(char),_get_zero_arr_size(_tmp37F,2));}));_tmp37D[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp35B));_tmp37D[1]=Cyc_PP_text(({const char*_tmp37E="<";_tag_arr(_tmp37E,
sizeof(char),_get_zero_arr_size(_tmp37E,2));}));_tmp37D[0]=Cyc_Absynpp_qvar2doc(
_tmp35C->name);Cyc_PP_cat(_tag_arr(_tmp37D,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL26C;_LL27B: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 3)goto _LL27D;_tmp35D=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp355)->f1;_tmp35E=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp355)->f2;_LL27C: s=({struct Cyc_PP_Doc*_tmp380[4];_tmp380[3]=_tmp35E?Cyc_PP_text(({
const char*_tmp383=", ...)";_tag_arr(_tmp383,sizeof(char),_get_zero_arr_size(
_tmp383,7));})): Cyc_PP_text(({const char*_tmp384=")";_tag_arr(_tmp384,sizeof(char),
_get_zero_arr_size(_tmp384,2));}));_tmp380[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,({const char*_tmp382=",";_tag_arr(_tmp382,sizeof(char),
_get_zero_arr_size(_tmp382,2));}),_tmp35D);_tmp380[1]=Cyc_PP_text(({const char*
_tmp381="(";_tag_arr(_tmp381,sizeof(char),_get_zero_arr_size(_tmp381,2));}));
_tmp380[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_arr(_tmp380,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL26C;_LL27D: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 4)goto
_LL27F;_tmp35F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp355)->f1;_LL27E: s=({
struct Cyc_PP_Doc*_tmp385[2];_tmp385[1]=Cyc_Absynpp_pat2doc(_tmp35F);_tmp385[0]=
Cyc_PP_text(({const char*_tmp386="&";_tag_arr(_tmp386,sizeof(char),
_get_zero_arr_size(_tmp386,2));}));Cyc_PP_cat(_tag_arr(_tmp385,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL26C;_LL27F: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 1)goto
_LL281;_tmp360=((struct Cyc_Absyn_Reference_p_struct*)_tmp355)->f1;_LL280: s=({
struct Cyc_PP_Doc*_tmp387[2];_tmp387[1]=Cyc_Absynpp_qvar2doc(_tmp360->name);
_tmp387[0]=Cyc_PP_text(({const char*_tmp388="*";_tag_arr(_tmp388,sizeof(char),
_get_zero_arr_size(_tmp388,2));}));Cyc_PP_cat(_tag_arr(_tmp387,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL26C;_LL281: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 12)goto
_LL283;_tmp361=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp355)->f1;_LL282: s=Cyc_Absynpp_qvar2doc(
_tmp361);goto _LL26C;_LL283: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 13)goto
_LL285;_tmp362=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp355)->f1;_tmp363=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp355)->f2;_tmp364=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp355)->f3;_LL284: {struct _tagged_arr term=_tmp364?({const char*_tmp38C=", ...)";
_tag_arr(_tmp38C,sizeof(char),_get_zero_arr_size(_tmp38C,7));}):({const char*
_tmp38D=")";_tag_arr(_tmp38D,sizeof(char),_get_zero_arr_size(_tmp38D,2));});s=({
struct Cyc_PP_Doc*_tmp389[2];_tmp389[1]=Cyc_PP_group(({const char*_tmp38A="(";
_tag_arr(_tmp38A,sizeof(char),_get_zero_arr_size(_tmp38A,2));}),term,({const char*
_tmp38B=",";_tag_arr(_tmp38B,sizeof(char),_get_zero_arr_size(_tmp38B,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp363));_tmp389[0]=Cyc_Absynpp_qvar2doc(
_tmp362);Cyc_PP_cat(_tag_arr(_tmp389,sizeof(struct Cyc_PP_Doc*),2));});goto _LL26C;}
_LL285: if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 5)goto _LL287;_tmp365=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp355)->f1;_tmp366=(void*)_tmp365.aggr_info;_tmp367=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp355)->f2;_tmp368=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp355)->f3;_tmp369=((struct Cyc_Absyn_Aggr_p_struct*)_tmp355)->f4;_LL286: {
struct _tagged_arr term=_tmp369?({const char*_tmp396=", ...}";_tag_arr(_tmp396,
sizeof(char),_get_zero_arr_size(_tmp396,7));}):({const char*_tmp397="}";_tag_arr(
_tmp397,sizeof(char),_get_zero_arr_size(_tmp397,2));});struct _tuple0*_tmp38F;
struct _tuple3 _tmp38E=Cyc_Absyn_aggr_kinded_name(_tmp366);_tmp38F=_tmp38E.f2;s=({
struct Cyc_PP_Doc*_tmp390[4];_tmp390[3]=Cyc_PP_group(({const char*_tmp394="";
_tag_arr(_tmp394,sizeof(char),_get_zero_arr_size(_tmp394,1));}),term,({const char*
_tmp395=",";_tag_arr(_tmp395,sizeof(char),_get_zero_arr_size(_tmp395,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp368));_tmp390[2]=Cyc_PP_egroup(({const
char*_tmp391="[";_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,2));}),({
const char*_tmp392="]";_tag_arr(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,2));}),({
const char*_tmp393=",";_tag_arr(_tmp393,sizeof(char),_get_zero_arr_size(_tmp393,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp367)));_tmp390[1]=Cyc_Absynpp_lb();_tmp390[0]=Cyc_Absynpp_qvar2doc(_tmp38F);
Cyc_PP_cat(_tag_arr(_tmp390,sizeof(struct Cyc_PP_Doc*),4));});goto _LL26C;}_LL287:
if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 10)goto _LL289;_tmp36A=((struct Cyc_Absyn_Enum_p_struct*)
_tmp355)->f2;_LL288: _tmp36B=_tmp36A;goto _LL28A;_LL289: if(_tmp355 <= (void*)2  || *((
int*)_tmp355)!= 11)goto _LL28B;_tmp36B=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp355)->f2;_LL28A: s=Cyc_Absynpp_qvar2doc(_tmp36B->name);goto _LL26C;_LL28B: if(
_tmp355 <= (void*)2  || *((int*)_tmp355)!= 6)goto _LL28D;_tmp36C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp355)->f2;_tmp36D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp355)->f3;if(_tmp36D
!= 0)goto _LL28D;_LL28C: s=Cyc_Absynpp_qvar2doc(_tmp36C->name);goto _LL26C;_LL28D:
if(_tmp355 <= (void*)2  || *((int*)_tmp355)!= 6)goto _LL28F;_tmp36E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp355)->f2;_tmp36F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp355)->f3;_tmp370=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp355)->f4;_LL28E: {struct _tagged_arr term=
_tmp370?({const char*_tmp39B=", ...)";_tag_arr(_tmp39B,sizeof(char),
_get_zero_arr_size(_tmp39B,7));}):({const char*_tmp39C=")";_tag_arr(_tmp39C,
sizeof(char),_get_zero_arr_size(_tmp39C,2));});s=({struct Cyc_PP_Doc*_tmp398[2];
_tmp398[1]=Cyc_PP_egroup(({const char*_tmp399="(";_tag_arr(_tmp399,sizeof(char),
_get_zero_arr_size(_tmp399,2));}),term,({const char*_tmp39A=",";_tag_arr(_tmp39A,
sizeof(char),_get_zero_arr_size(_tmp39A,2));}),((struct Cyc_List_List*(*)(struct
Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp36F));_tmp398[0]=Cyc_Absynpp_qvar2doc(_tmp36E->name);Cyc_PP_cat(_tag_arr(
_tmp398,sizeof(struct Cyc_PP_Doc*),2));});goto _LL26C;}_LL28F: if(_tmp355 <= (void*)
2  || *((int*)_tmp355)!= 14)goto _LL26C;_tmp371=((struct Cyc_Absyn_Exp_p_struct*)
_tmp355)->f1;_LL290: s=Cyc_Absynpp_exp2doc(_tmp371);goto _LL26C;_LL26C:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){return({struct Cyc_PP_Doc*
_tmp39D[2];_tmp39D[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp39D[0]=Cyc_PP_egroup(({
const char*_tmp39E="";_tag_arr(_tmp39E,sizeof(char),_get_zero_arr_size(_tmp39E,1));}),({
const char*_tmp39F="=";_tag_arr(_tmp39F,sizeof(char),_get_zero_arr_size(_tmp39F,2));}),({
const char*_tmp3A0="=";_tag_arr(_tmp3A0,sizeof(char),_get_zero_arr_size(_tmp3A0,2));}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_arr(_tmp39D,sizeof(struct
Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*
c){if(c->where_clause == 0  && (void*)(c->pattern)->r == (void*)((void*)0))return({
struct Cyc_PP_Doc*_tmp3A1[2];_tmp3A1[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3A3[
2];_tmp3A3[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3A3[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp3A3,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3A1[0]=Cyc_PP_text(({
const char*_tmp3A2="default: ";_tag_arr(_tmp3A2,sizeof(char),_get_zero_arr_size(
_tmp3A2,10));}));Cyc_PP_cat(_tag_arr(_tmp3A1,sizeof(struct Cyc_PP_Doc*),2));});
else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp3A4[4];_tmp3A4[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3A7[2];_tmp3A7[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3A7[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3A7,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3A4[2]=Cyc_PP_text(({const char*_tmp3A6=": ";_tag_arr(_tmp3A6,sizeof(char),
_get_zero_arr_size(_tmp3A6,3));}));_tmp3A4[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3A4[0]=Cyc_PP_text(({const char*_tmp3A5="case ";_tag_arr(_tmp3A5,sizeof(char),
_get_zero_arr_size(_tmp3A5,6));}));Cyc_PP_cat(_tag_arr(_tmp3A4,sizeof(struct Cyc_PP_Doc*),
4));});else{return({struct Cyc_PP_Doc*_tmp3A8[6];_tmp3A8[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp3AC[2];_tmp3AC[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3AC[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3AC,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3A8[4]=Cyc_PP_text(({const char*_tmp3AB=": ";_tag_arr(_tmp3AB,sizeof(char),
_get_zero_arr_size(_tmp3AB,3));}));_tmp3A8[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp3A8[2]=Cyc_PP_text(({const char*_tmp3AA=" && ";
_tag_arr(_tmp3AA,sizeof(char),_get_zero_arr_size(_tmp3AA,5));}));_tmp3A8[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp3A8[0]=Cyc_PP_text(({const char*_tmp3A9="case ";_tag_arr(_tmp3A9,
sizeof(char),_get_zero_arr_size(_tmp3A9,6));}));Cyc_PP_cat(_tag_arr(_tmp3A8,
sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,({const char*_tmp3AD="";_tag_arr(_tmp3AD,sizeof(char),
_get_zero_arr_size(_tmp3AD,1));}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{return({struct Cyc_PP_Doc*_tmp3AE[3];_tmp3AE[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp3AE[1]=Cyc_PP_text(({const char*_tmp3AF=" = ";
_tag_arr(_tmp3AF,sizeof(char),_get_zero_arr_size(_tmp3AF,4));}));_tmp3AE[0]=Cyc_Absynpp_qvar2doc(
f->name);Cyc_PP_cat(_tag_arr(_tmp3AE,sizeof(struct Cyc_PP_Doc*),3));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*_tmp3B0=",";
_tag_arr(_tmp3B0,sizeof(char),_get_zero_arr_size(_tmp3B0,2));}),fs);}static
struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(
v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3B1=",";_tag_arr(_tmp3B1,sizeof(char),_get_zero_arr_size(_tmp3B1,2));}),
vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl _tmp3B3;void*_tmp3B4;struct _tuple0*_tmp3B5;struct Cyc_Absyn_Tqual
_tmp3B6;void*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_List_List*_tmp3B9;
struct Cyc_Absyn_Vardecl*_tmp3B2=vd;_tmp3B3=*_tmp3B2;_tmp3B4=(void*)_tmp3B3.sc;
_tmp3B5=_tmp3B3.name;_tmp3B6=_tmp3B3.tq;_tmp3B7=(void*)_tmp3B3.type;_tmp3B8=
_tmp3B3.initializer;_tmp3B9=_tmp3B3.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3B5);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3B9);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp3BA=Cyc_Tcutil_compress(_tmp3B7);struct Cyc_Absyn_FnInfo
_tmp3BB;struct Cyc_List_List*_tmp3BC;_LL292: if(_tmp3BA <= (void*)4  || *((int*)
_tmp3BA)!= 8)goto _LL294;_tmp3BB=((struct Cyc_Absyn_FnType_struct*)_tmp3BA)->f1;
_tmp3BC=_tmp3BB.attributes;_LL293: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3BC);
goto _LL291;_LL294:;_LL295: beforenamedoc=Cyc_PP_nil_doc();goto _LL291;_LL291:;}s=({
struct Cyc_PP_Doc*_tmp3BD[5];_tmp3BD[4]=Cyc_PP_text(({const char*_tmp3C3=";";
_tag_arr(_tmp3C3,sizeof(char),_get_zero_arr_size(_tmp3C3,2));}));_tmp3BD[3]=
_tmp3B8 == 0?Cyc_PP_text(({const char*_tmp3C0="";_tag_arr(_tmp3C0,sizeof(char),
_get_zero_arr_size(_tmp3C0,1));})):({struct Cyc_PP_Doc*_tmp3C1[2];_tmp3C1[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3B8);_tmp3C1[0]=Cyc_PP_text(({const char*_tmp3C2=" = ";
_tag_arr(_tmp3C2,sizeof(char),_get_zero_arr_size(_tmp3C2,4));}));Cyc_PP_cat(
_tag_arr(_tmp3C1,sizeof(struct Cyc_PP_Doc*),2));});_tmp3BD[2]=Cyc_Absynpp_tqtd2doc(
_tmp3B6,_tmp3B7,({struct Cyc_Core_Opt*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->v=({
struct Cyc_PP_Doc*_tmp3BF[2];_tmp3BF[1]=sn;_tmp3BF[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp3BF,sizeof(struct Cyc_PP_Doc*),2));});_tmp3BE;}));_tmp3BD[1]=Cyc_Absynpp_scope2doc(
_tmp3B4);_tmp3BD[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(
_tag_arr(_tmp3BD,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct _tuple11{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple11*x){struct _tuple0*_tmp3C5;struct _tuple11 _tmp3C4=*x;_tmp3C5=_tmp3C4.f2;
return Cyc_Absynpp_qvar2doc(_tmp3C5);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3C6=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp3C7;struct Cyc_Absyn_Aggrdecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3C9;struct
Cyc_Absyn_Tuniondecl*_tmp3CA;struct Cyc_Absyn_Tuniondecl _tmp3CB;void*_tmp3CC;
struct _tuple0*_tmp3CD;struct Cyc_List_List*_tmp3CE;struct Cyc_Core_Opt*_tmp3CF;int
_tmp3D0;struct Cyc_Absyn_Pat*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_List_List*
_tmp3D3;struct Cyc_Absyn_Enumdecl*_tmp3D4;struct Cyc_Absyn_Enumdecl _tmp3D5;void*
_tmp3D6;struct _tuple0*_tmp3D7;struct Cyc_Core_Opt*_tmp3D8;struct Cyc_Absyn_Typedefdecl*
_tmp3D9;struct _tagged_arr*_tmp3DA;struct Cyc_List_List*_tmp3DB;struct _tuple0*
_tmp3DC;struct Cyc_List_List*_tmp3DD;struct Cyc_List_List*_tmp3DE;struct Cyc_List_List*
_tmp3DF;struct Cyc_List_List*_tmp3E0;_LL297: if(_tmp3C6 <= (void*)2  || *((int*)
_tmp3C6)!= 1)goto _LL299;_tmp3C7=((struct Cyc_Absyn_Fn_d_struct*)_tmp3C6)->f1;
_LL298: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp3E9=_cycalloc(sizeof(*
_tmp3E9));_tmp3E9[0]=({struct Cyc_Absyn_FnType_struct _tmp3EA;_tmp3EA.tag=8;
_tmp3EA.f1=({struct Cyc_Absyn_FnInfo _tmp3EB;_tmp3EB.tvars=_tmp3C7->tvs;_tmp3EB.effect=
_tmp3C7->effect;_tmp3EB.ret_typ=(void*)((void*)_tmp3C7->ret_type);_tmp3EB.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3C7->args);_tmp3EB.c_varargs=_tmp3C7->c_varargs;
_tmp3EB.cyc_varargs=_tmp3C7->cyc_varargs;_tmp3EB.rgn_po=_tmp3C7->rgn_po;_tmp3EB.attributes=
0;_tmp3EB;});_tmp3EA;});_tmp3E9;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3C7->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3C7->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(({const char*_tmp3E1="__inline ";_tag_arr(_tmp3E1,sizeof(
char),_get_zero_arr_size(_tmp3E1,10));}));else{inlinedoc=Cyc_PP_text(({const char*
_tmp3E2="inline ";_tag_arr(_tmp3E2,sizeof(char),_get_zero_arr_size(_tmp3E2,8));}));}}
else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3C7->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp3C7->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3C7->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(
0),t,({struct Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=({
struct Cyc_PP_Doc*_tmp3E8[2];_tmp3E8[1]=namedoc;_tmp3E8[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp3E8,sizeof(struct Cyc_PP_Doc*),2));});_tmp3E7;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3E5[5];_tmp3E5[4]=Cyc_Absynpp_rb();_tmp3E5[3]=Cyc_PP_line_doc();
_tmp3E5[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3E6[2];_tmp3E6[1]=Cyc_Absynpp_stmt2doc(
_tmp3C7->body);_tmp3E6[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3E6,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3E5[1]=Cyc_Absynpp_lb();_tmp3E5[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_arr(_tmp3E5,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3E3[4];_tmp3E3[3]=bodydoc;_tmp3E3[2]=tqtddoc;_tmp3E3[1]=scopedoc;_tmp3E3[0]=
inlinedoc;Cyc_PP_cat(_tag_arr(_tmp3E3,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp3E4[2];_tmp3E4[1]=s;_tmp3E4[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp3E4,sizeof(struct Cyc_PP_Doc*),2));});goto _LL296;}}_LL299: if(_tmp3C6
<= (void*)2  || *((int*)_tmp3C6)!= 4)goto _LL29B;_tmp3C8=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp3C6)->f1;_LL29A: if(_tmp3C8->impl == 0)s=({struct Cyc_PP_Doc*_tmp3EC[5];_tmp3EC[
4]=Cyc_PP_text(({const char*_tmp3ED=";";_tag_arr(_tmp3ED,sizeof(char),
_get_zero_arr_size(_tmp3ED,2));}));_tmp3EC[3]=Cyc_Absynpp_ktvars2doc(_tmp3C8->tvs);
_tmp3EC[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3C8->name);_tmp3EC[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3C8->kind);_tmp3EC[0]=Cyc_Absynpp_scope2doc((void*)_tmp3C8->sc);Cyc_PP_cat(
_tag_arr(_tmp3EC,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp3EE[12];_tmp3EE[11]=Cyc_PP_text(({const char*_tmp3F2=";";_tag_arr(_tmp3F2,
sizeof(char),_get_zero_arr_size(_tmp3F2,2));}));_tmp3EE[10]=Cyc_Absynpp_atts2doc(
_tmp3C8->attributes);_tmp3EE[9]=Cyc_Absynpp_rb();_tmp3EE[8]=Cyc_PP_line_doc();
_tmp3EE[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3F1[2];_tmp3F1[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->fields);_tmp3F1[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp3F1,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3EE[6]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->rgn_po == 
0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*_tmp3EF[2];_tmp3EF[1]=Cyc_Absynpp_rgnpo2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->rgn_po);_tmp3EF[0]=
Cyc_PP_text(({const char*_tmp3F0=":";_tag_arr(_tmp3F0,sizeof(char),
_get_zero_arr_size(_tmp3F0,2));}));Cyc_PP_cat(_tag_arr(_tmp3EF,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3EE[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3C8->impl))->exist_vars);_tmp3EE[4]=Cyc_Absynpp_lb();_tmp3EE[3]=
Cyc_PP_blank_doc();_tmp3EE[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3C8->name);_tmp3EE[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3C8->kind);_tmp3EE[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3C8->sc);Cyc_PP_cat(_tag_arr(_tmp3EE,sizeof(struct Cyc_PP_Doc*),12));});}
goto _LL296;_LL29B: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 0)goto _LL29D;
_tmp3C9=((struct Cyc_Absyn_Var_d_struct*)_tmp3C6)->f1;_LL29C: s=Cyc_Absynpp_vardecl2doc(
_tmp3C9);goto _LL296;_LL29D: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 5)goto
_LL29F;_tmp3CA=((struct Cyc_Absyn_Tunion_d_struct*)_tmp3C6)->f1;_tmp3CB=*_tmp3CA;
_tmp3CC=(void*)_tmp3CB.sc;_tmp3CD=_tmp3CB.name;_tmp3CE=_tmp3CB.tvs;_tmp3CF=
_tmp3CB.fields;_tmp3D0=_tmp3CB.is_xtunion;_LL29E: if(_tmp3CF == 0)s=({struct Cyc_PP_Doc*
_tmp3F3[5];_tmp3F3[4]=Cyc_PP_text(({const char*_tmp3F6=";";_tag_arr(_tmp3F6,
sizeof(char),_get_zero_arr_size(_tmp3F6,2));}));_tmp3F3[3]=Cyc_Absynpp_ktvars2doc(
_tmp3CE);_tmp3F3[2]=_tmp3D0?Cyc_Absynpp_qvar2bolddoc(_tmp3CD): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3CD);_tmp3F3[1]=_tmp3D0?Cyc_PP_text(({const char*_tmp3F4="xtunion ";_tag_arr(
_tmp3F4,sizeof(char),_get_zero_arr_size(_tmp3F4,9));})): Cyc_PP_text(({const char*
_tmp3F5="tunion ";_tag_arr(_tmp3F5,sizeof(char),_get_zero_arr_size(_tmp3F5,8));}));
_tmp3F3[0]=Cyc_Absynpp_scope2doc(_tmp3CC);Cyc_PP_cat(_tag_arr(_tmp3F3,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp3F7[10];_tmp3F7[9]=Cyc_PP_text(({
const char*_tmp3FB=";";_tag_arr(_tmp3FB,sizeof(char),_get_zero_arr_size(_tmp3FB,2));}));
_tmp3F7[8]=Cyc_Absynpp_rb();_tmp3F7[7]=Cyc_PP_line_doc();_tmp3F7[6]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp3FA[2];_tmp3FA[1]=Cyc_Absynpp_tunionfields2doc((struct
Cyc_List_List*)_tmp3CF->v);_tmp3FA[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(
_tmp3FA,sizeof(struct Cyc_PP_Doc*),2));}));_tmp3F7[5]=Cyc_Absynpp_lb();_tmp3F7[4]=
Cyc_PP_blank_doc();_tmp3F7[3]=Cyc_Absynpp_ktvars2doc(_tmp3CE);_tmp3F7[2]=_tmp3D0?
Cyc_Absynpp_qvar2bolddoc(_tmp3CD): Cyc_Absynpp_typedef_name2bolddoc(_tmp3CD);
_tmp3F7[1]=_tmp3D0?Cyc_PP_text(({const char*_tmp3F8="xtunion ";_tag_arr(_tmp3F8,
sizeof(char),_get_zero_arr_size(_tmp3F8,9));})): Cyc_PP_text(({const char*_tmp3F9="tunion ";
_tag_arr(_tmp3F9,sizeof(char),_get_zero_arr_size(_tmp3F9,8));}));_tmp3F7[0]=Cyc_Absynpp_scope2doc(
_tmp3CC);Cyc_PP_cat(_tag_arr(_tmp3F7,sizeof(struct Cyc_PP_Doc*),10));});}goto
_LL296;_LL29F: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 2)goto _LL2A1;_tmp3D1=((
struct Cyc_Absyn_Let_d_struct*)_tmp3C6)->f1;_tmp3D2=((struct Cyc_Absyn_Let_d_struct*)
_tmp3C6)->f3;_LL2A0: s=({struct Cyc_PP_Doc*_tmp3FC[5];_tmp3FC[4]=Cyc_PP_text(({
const char*_tmp3FF=";";_tag_arr(_tmp3FF,sizeof(char),_get_zero_arr_size(_tmp3FF,2));}));
_tmp3FC[3]=Cyc_Absynpp_exp2doc(_tmp3D2);_tmp3FC[2]=Cyc_PP_text(({const char*
_tmp3FE=" = ";_tag_arr(_tmp3FE,sizeof(char),_get_zero_arr_size(_tmp3FE,4));}));
_tmp3FC[1]=Cyc_Absynpp_pat2doc(_tmp3D1);_tmp3FC[0]=Cyc_PP_text(({const char*
_tmp3FD="let ";_tag_arr(_tmp3FD,sizeof(char),_get_zero_arr_size(_tmp3FD,5));}));
Cyc_PP_cat(_tag_arr(_tmp3FC,sizeof(struct Cyc_PP_Doc*),5));});goto _LL296;_LL2A1:
if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 3)goto _LL2A3;_tmp3D3=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3C6)->f1;_LL2A2: s=({struct Cyc_PP_Doc*_tmp400[3];_tmp400[2]=Cyc_PP_text(({
const char*_tmp402=";";_tag_arr(_tmp402,sizeof(char),_get_zero_arr_size(_tmp402,2));}));
_tmp400[1]=Cyc_Absynpp_ids2doc(_tmp3D3);_tmp400[0]=Cyc_PP_text(({const char*
_tmp401="let ";_tag_arr(_tmp401,sizeof(char),_get_zero_arr_size(_tmp401,5));}));
Cyc_PP_cat(_tag_arr(_tmp400,sizeof(struct Cyc_PP_Doc*),3));});goto _LL296;_LL2A3:
if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 6)goto _LL2A5;_tmp3D4=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3C6)->f1;_tmp3D5=*_tmp3D4;_tmp3D6=(void*)_tmp3D5.sc;_tmp3D7=_tmp3D5.name;
_tmp3D8=_tmp3D5.fields;_LL2A4: if(_tmp3D8 == 0)s=({struct Cyc_PP_Doc*_tmp403[4];
_tmp403[3]=Cyc_PP_text(({const char*_tmp405=";";_tag_arr(_tmp405,sizeof(char),
_get_zero_arr_size(_tmp405,2));}));_tmp403[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3D7);_tmp403[1]=Cyc_PP_text(({const char*_tmp404="enum ";_tag_arr(_tmp404,
sizeof(char),_get_zero_arr_size(_tmp404,6));}));_tmp403[0]=Cyc_Absynpp_scope2doc(
_tmp3D6);Cyc_PP_cat(_tag_arr(_tmp403,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp406[9];_tmp406[8]=Cyc_PP_text(({const char*_tmp409=";";
_tag_arr(_tmp409,sizeof(char),_get_zero_arr_size(_tmp409,2));}));_tmp406[7]=Cyc_Absynpp_rb();
_tmp406[6]=Cyc_PP_line_doc();_tmp406[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp408[
2];_tmp408[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3D8->v);
_tmp408[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp408,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp406[4]=Cyc_Absynpp_lb();_tmp406[3]=Cyc_PP_blank_doc();_tmp406[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3D7);_tmp406[1]=Cyc_PP_text(({const char*_tmp407="enum ";_tag_arr(_tmp407,
sizeof(char),_get_zero_arr_size(_tmp407,6));}));_tmp406[0]=Cyc_Absynpp_scope2doc(
_tmp3D6);Cyc_PP_cat(_tag_arr(_tmp406,sizeof(struct Cyc_PP_Doc*),9));});}goto
_LL296;_LL2A5: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 7)goto _LL2A7;_tmp3D9=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp3C6)->f1;_LL2A6: {void*t;if(_tmp3D9->defn
!= 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3D9->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp3D9->kind,0);}s=({struct Cyc_PP_Doc*_tmp40A[4];_tmp40A[3]=Cyc_PP_text(({const
char*_tmp40E=";";_tag_arr(_tmp40E,sizeof(char),_get_zero_arr_size(_tmp40E,2));}));
_tmp40A[2]=Cyc_Absynpp_atts2doc(_tmp3D9->atts);_tmp40A[1]=Cyc_Absynpp_tqtd2doc(
_tmp3D9->tq,t,({struct Cyc_Core_Opt*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->v=({
struct Cyc_PP_Doc*_tmp40D[2];_tmp40D[1]=Cyc_Absynpp_tvars2doc(_tmp3D9->tvs);
_tmp40D[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3D9->name);Cyc_PP_cat(_tag_arr(
_tmp40D,sizeof(struct Cyc_PP_Doc*),2));});_tmp40C;}));_tmp40A[0]=Cyc_PP_text(({
const char*_tmp40B="typedef ";_tag_arr(_tmp40B,sizeof(char),_get_zero_arr_size(
_tmp40B,9));}));Cyc_PP_cat(_tag_arr(_tmp40A,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL296;}_LL2A7: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 8)goto _LL2A9;
_tmp3DA=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3C6)->f1;_tmp3DB=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3C6)->f2;_LL2A8: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(
_tmp3DA);s=({struct Cyc_PP_Doc*_tmp40F[8];_tmp40F[7]=Cyc_Absynpp_rb();_tmp40F[6]=
Cyc_PP_line_doc();_tmp40F[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp411="";_tag_arr(_tmp411,sizeof(char),
_get_zero_arr_size(_tmp411,1));}),_tmp3DB);_tmp40F[4]=Cyc_PP_line_doc();_tmp40F[
3]=Cyc_Absynpp_lb();_tmp40F[2]=Cyc_PP_blank_doc();_tmp40F[1]=Cyc_PP_textptr(
_tmp3DA);_tmp40F[0]=Cyc_PP_text(({const char*_tmp410="namespace ";_tag_arr(
_tmp410,sizeof(char),_get_zero_arr_size(_tmp410,11));}));Cyc_PP_cat(_tag_arr(
_tmp40F,sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL296;_LL2A9: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 9)goto _LL2AB;
_tmp3DC=((struct Cyc_Absyn_Using_d_struct*)_tmp3C6)->f1;_tmp3DD=((struct Cyc_Absyn_Using_d_struct*)
_tmp3C6)->f2;_LL2AA: if(Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*
_tmp412[8];_tmp412[7]=Cyc_Absynpp_rb();_tmp412[6]=Cyc_PP_line_doc();_tmp412[5]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp414="";_tag_arr(_tmp414,sizeof(char),_get_zero_arr_size(_tmp414,1));}),
_tmp3DD);_tmp412[4]=Cyc_PP_line_doc();_tmp412[3]=Cyc_Absynpp_lb();_tmp412[2]=Cyc_PP_blank_doc();
_tmp412[1]=Cyc_Absynpp_qvar2doc(_tmp3DC);_tmp412[0]=Cyc_PP_text(({const char*
_tmp413="using ";_tag_arr(_tmp413,sizeof(char),_get_zero_arr_size(_tmp413,7));}));
Cyc_PP_cat(_tag_arr(_tmp412,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp415[11];_tmp415[10]=Cyc_PP_text(({const char*_tmp41A=" */";_tag_arr(_tmp41A,
sizeof(char),_get_zero_arr_size(_tmp41A,4));}));_tmp415[9]=Cyc_Absynpp_rb();
_tmp415[8]=Cyc_PP_text(({const char*_tmp419="/* ";_tag_arr(_tmp419,sizeof(char),
_get_zero_arr_size(_tmp419,4));}));_tmp415[7]=Cyc_PP_line_doc();_tmp415[6]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp418="";_tag_arr(_tmp418,sizeof(char),_get_zero_arr_size(_tmp418,1));}),
_tmp3DD);_tmp415[5]=Cyc_PP_line_doc();_tmp415[4]=Cyc_PP_text(({const char*_tmp417=" */";
_tag_arr(_tmp417,sizeof(char),_get_zero_arr_size(_tmp417,4));}));_tmp415[3]=Cyc_Absynpp_lb();
_tmp415[2]=Cyc_PP_blank_doc();_tmp415[1]=Cyc_Absynpp_qvar2doc(_tmp3DC);_tmp415[0]=
Cyc_PP_text(({const char*_tmp416="/* using ";_tag_arr(_tmp416,sizeof(char),
_get_zero_arr_size(_tmp416,10));}));Cyc_PP_cat(_tag_arr(_tmp415,sizeof(struct Cyc_PP_Doc*),
11));});}goto _LL296;_LL2AB: if(_tmp3C6 <= (void*)2  || *((int*)_tmp3C6)!= 10)goto
_LL2AD;_tmp3DE=((struct Cyc_Absyn_ExternC_d_struct*)_tmp3C6)->f1;_LL2AC: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp41B[6];_tmp41B[5]=Cyc_Absynpp_rb();_tmp41B[4]=Cyc_PP_line_doc();
_tmp41B[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp41D="";_tag_arr(_tmp41D,sizeof(char),_get_zero_arr_size(_tmp41D,1));}),
_tmp3DE);_tmp41B[2]=Cyc_PP_line_doc();_tmp41B[1]=Cyc_Absynpp_lb();_tmp41B[0]=Cyc_PP_text(({
const char*_tmp41C="extern \"C\" ";_tag_arr(_tmp41C,sizeof(char),
_get_zero_arr_size(_tmp41C,12));}));Cyc_PP_cat(_tag_arr(_tmp41B,sizeof(struct Cyc_PP_Doc*),
6));});else{s=({struct Cyc_PP_Doc*_tmp41E[9];_tmp41E[8]=Cyc_PP_text(({const char*
_tmp423=" */";_tag_arr(_tmp423,sizeof(char),_get_zero_arr_size(_tmp423,4));}));
_tmp41E[7]=Cyc_Absynpp_rb();_tmp41E[6]=Cyc_PP_text(({const char*_tmp422="/* ";
_tag_arr(_tmp422,sizeof(char),_get_zero_arr_size(_tmp422,4));}));_tmp41E[5]=Cyc_PP_line_doc();
_tmp41E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp421="";_tag_arr(_tmp421,sizeof(char),_get_zero_arr_size(_tmp421,1));}),
_tmp3DE);_tmp41E[3]=Cyc_PP_line_doc();_tmp41E[2]=Cyc_PP_text(({const char*_tmp420=" */";
_tag_arr(_tmp420,sizeof(char),_get_zero_arr_size(_tmp420,4));}));_tmp41E[1]=Cyc_Absynpp_lb();
_tmp41E[0]=Cyc_PP_text(({const char*_tmp41F="/* extern \"C\" ";_tag_arr(_tmp41F,
sizeof(char),_get_zero_arr_size(_tmp41F,15));}));Cyc_PP_cat(_tag_arr(_tmp41E,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL296;_LL2AD: if(_tmp3C6 <= (void*)2  || *((
int*)_tmp3C6)!= 11)goto _LL2AF;_tmp3DF=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp3C6)->f1;_tmp3E0=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3C6)->f2;
_LL2AE: if(Cyc_Absynpp_print_externC_stmts){struct Cyc_PP_Doc*exs_doc;if(_tmp3E0 != 
0)exs_doc=({struct Cyc_PP_Doc*_tmp424[7];_tmp424[6]=Cyc_Absynpp_rb();_tmp424[5]=
Cyc_PP_line_doc();_tmp424[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,({
const char*_tmp426=",";_tag_arr(_tmp426,sizeof(char),_get_zero_arr_size(_tmp426,2));}),
_tmp3E0);_tmp424[3]=Cyc_PP_line_doc();_tmp424[2]=Cyc_Absynpp_lb();_tmp424[1]=Cyc_PP_text(({
const char*_tmp425=" export ";_tag_arr(_tmp425,sizeof(char),_get_zero_arr_size(
_tmp425,9));}));_tmp424[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_arr(_tmp424,sizeof(
struct Cyc_PP_Doc*),7));});else{exs_doc=Cyc_Absynpp_rb();}s=({struct Cyc_PP_Doc*
_tmp427[6];_tmp427[5]=exs_doc;_tmp427[4]=Cyc_PP_line_doc();_tmp427[3]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr
sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({const char*
_tmp429="";_tag_arr(_tmp429,sizeof(char),_get_zero_arr_size(_tmp429,1));}),
_tmp3DF);_tmp427[2]=Cyc_PP_line_doc();_tmp427[1]=Cyc_Absynpp_lb();_tmp427[0]=Cyc_PP_text(({
const char*_tmp428="extern \"C include\" ";_tag_arr(_tmp428,sizeof(char),
_get_zero_arr_size(_tmp428,20));}));Cyc_PP_cat(_tag_arr(_tmp427,sizeof(struct Cyc_PP_Doc*),
6));});}else{s=({struct Cyc_PP_Doc*_tmp42A[9];_tmp42A[8]=Cyc_PP_text(({const char*
_tmp42F=" */";_tag_arr(_tmp42F,sizeof(char),_get_zero_arr_size(_tmp42F,4));}));
_tmp42A[7]=Cyc_Absynpp_rb();_tmp42A[6]=Cyc_PP_text(({const char*_tmp42E="/* ";
_tag_arr(_tmp42E,sizeof(char),_get_zero_arr_size(_tmp42E,4));}));_tmp42A[5]=Cyc_PP_line_doc();
_tmp42A[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp42D="";_tag_arr(_tmp42D,sizeof(char),_get_zero_arr_size(_tmp42D,1));}),
_tmp3DF);_tmp42A[3]=Cyc_PP_line_doc();_tmp42A[2]=Cyc_PP_text(({const char*_tmp42C=" */";
_tag_arr(_tmp42C,sizeof(char),_get_zero_arr_size(_tmp42C,4));}));_tmp42A[1]=Cyc_Absynpp_lb();
_tmp42A[0]=Cyc_PP_text(({const char*_tmp42B="/* extern \"C include\" ";_tag_arr(
_tmp42B,sizeof(char),_get_zero_arr_size(_tmp42B,23));}));Cyc_PP_cat(_tag_arr(
_tmp42A,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL296;_LL2AF: if((int)_tmp3C6 != 0)
goto _LL2B1;_LL2B0: s=({struct Cyc_PP_Doc*_tmp430[2];_tmp430[1]=Cyc_Absynpp_lb();
_tmp430[0]=Cyc_PP_text(({const char*_tmp431="__cyclone_port_on__;";_tag_arr(
_tmp431,sizeof(char),_get_zero_arr_size(_tmp431,21));}));Cyc_PP_cat(_tag_arr(
_tmp430,sizeof(struct Cyc_PP_Doc*),2));});goto _LL296;_LL2B1: if((int)_tmp3C6 != 1)
goto _LL296;_LL2B2: s=({struct Cyc_PP_Doc*_tmp432[2];_tmp432[1]=Cyc_Absynpp_lb();
_tmp432[0]=Cyc_PP_text(({const char*_tmp433="__cyclone_port_off__;";_tag_arr(
_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,22));}));Cyc_PP_cat(_tag_arr(
_tmp432,sizeof(struct Cyc_PP_Doc*),2));});goto _LL296;_LL296:;}return s;}int Cyc_Absynpp_print_scopes=
1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(!Cyc_Absynpp_print_scopes)
return Cyc_PP_nil_doc();{void*_tmp434=sc;_LL2B4: if((int)_tmp434 != 0)goto _LL2B6;
_LL2B5: return Cyc_PP_text(({const char*_tmp435="static ";_tag_arr(_tmp435,sizeof(
char),_get_zero_arr_size(_tmp435,8));}));_LL2B6: if((int)_tmp434 != 2)goto _LL2B8;
_LL2B7: return Cyc_PP_nil_doc();_LL2B8: if((int)_tmp434 != 3)goto _LL2BA;_LL2B9:
return Cyc_PP_text(({const char*_tmp436="extern ";_tag_arr(_tmp436,sizeof(char),
_get_zero_arr_size(_tmp436,8));}));_LL2BA: if((int)_tmp434 != 4)goto _LL2BC;_LL2BB:
return Cyc_PP_text(({const char*_tmp437="extern \"C\" ";_tag_arr(_tmp437,sizeof(
char),_get_zero_arr_size(_tmp437,12));}));_LL2BC: if((int)_tmp434 != 1)goto _LL2BE;
_LL2BD: return Cyc_PP_text(({const char*_tmp438="abstract ";_tag_arr(_tmp438,
sizeof(char),_get_zero_arr_size(_tmp438,10));}));_LL2BE: if((int)_tmp434 != 5)goto
_LL2B3;_LL2BF: return Cyc_PP_text(({const char*_tmp439="register ";_tag_arr(_tmp439,
sizeof(char),_get_zero_arr_size(_tmp439,10));}));_LL2B3:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp43A=t;struct Cyc_Absyn_Tvar*_tmp43B;struct Cyc_List_List*_tmp43C;
_LL2C1: if(_tmp43A <= (void*)4  || *((int*)_tmp43A)!= 1)goto _LL2C3;_tmp43B=((struct
Cyc_Absyn_VarType_struct*)_tmp43A)->f1;_LL2C2: return Cyc_Tcutil_is_temp_tvar(
_tmp43B);_LL2C3: if(_tmp43A <= (void*)4  || *((int*)_tmp43A)!= 20)goto _LL2C5;
_tmp43C=((struct Cyc_Absyn_JoinEff_struct*)_tmp43A)->f1;_LL2C4: return Cyc_List_exists(
Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp43C);_LL2C5:;_LL2C6: return 0;_LL2C0:;}
int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp43D=t;void**_tmp43E;void*
_tmp43F;_LL2C8: if(_tmp43D <= (void*)4  || *((int*)_tmp43D)!= 11)goto _LL2CA;_LL2C9:
return 1;_LL2CA: if(_tmp43D <= (void*)4  || *((int*)_tmp43D)!= 13)goto _LL2CC;_LL2CB:
return 1;_LL2CC: if(_tmp43D <= (void*)4  || *((int*)_tmp43D)!= 16)goto _LL2CE;_tmp43E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp43D)->f4;if(_tmp43E == 0)goto _LL2CE;
_tmp43F=*_tmp43E;_LL2CD: return Cyc_Absynpp_is_anon_aggrtype(_tmp43F);_LL2CE:;
_LL2CF: return 0;_LL2C7:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){struct _tuple5
_tmp441=({struct _tuple5 _tmp440;_tmp440.f1=(void*)tms->hd;_tmp440.f2=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp440;});void*_tmp442;void*
_tmp443;_LL2D1: _tmp442=_tmp441.f1;if(*((int*)_tmp442)!= 2)goto _LL2D3;_tmp443=
_tmp441.f2;if(*((int*)_tmp443)!= 3)goto _LL2D3;_LL2D2: return({struct Cyc_List_List*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->hd=(void*)((void*)tms->hd);_tmp444->tl=({
struct Cyc_List_List*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);_tmp445->tl=Cyc_Absynpp_bubble_attributes(
atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp445;});_tmp444;});
_LL2D3:;_LL2D4: return({struct Cyc_List_List*_tmp446=_cycalloc(sizeof(*_tmp446));
_tmp446->hd=(void*)atts;_tmp446->tl=tms;_tmp446;});_LL2D0:;}else{return({struct
Cyc_List_List*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->hd=(void*)atts;
_tmp447->tl=tms;_tmp447;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp448=t;struct Cyc_Absyn_ArrayInfo _tmp449;void*_tmp44A;struct
Cyc_Absyn_Tqual _tmp44B;struct Cyc_Absyn_Exp*_tmp44C;struct Cyc_Absyn_Conref*
_tmp44D;struct Cyc_Position_Segment*_tmp44E;struct Cyc_Absyn_PtrInfo _tmp44F;void*
_tmp450;struct Cyc_Absyn_Tqual _tmp451;struct Cyc_Absyn_PtrAtts _tmp452;struct Cyc_Absyn_FnInfo
_tmp453;struct Cyc_List_List*_tmp454;struct Cyc_Core_Opt*_tmp455;void*_tmp456;
struct Cyc_List_List*_tmp457;int _tmp458;struct Cyc_Absyn_VarargInfo*_tmp459;struct
Cyc_List_List*_tmp45A;struct Cyc_List_List*_tmp45B;struct Cyc_Core_Opt*_tmp45C;
struct Cyc_Core_Opt*_tmp45D;int _tmp45E;struct _tuple0*_tmp45F;struct Cyc_List_List*
_tmp460;void**_tmp461;_LL2D6: if(_tmp448 <= (void*)4  || *((int*)_tmp448)!= 7)goto
_LL2D8;_tmp449=((struct Cyc_Absyn_ArrayType_struct*)_tmp448)->f1;_tmp44A=(void*)
_tmp449.elt_type;_tmp44B=_tmp449.tq;_tmp44C=_tmp449.num_elts;_tmp44D=_tmp449.zero_term;
_tmp44E=_tmp449.zt_loc;_LL2D7: {struct Cyc_Absyn_Tqual _tmp463;void*_tmp464;struct
Cyc_List_List*_tmp465;struct _tuple7 _tmp462=Cyc_Absynpp_to_tms(_tmp44B,_tmp44A);
_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_tmp465=_tmp462.f3;{void*tm;if(_tmp44C == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp466=_cycalloc(sizeof(*_tmp466));
_tmp466[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp467;_tmp467.tag=0;_tmp467.f1=
_tmp44D;_tmp467.f2=_tmp44E;_tmp467;});_tmp466;});else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp469;_tmp469.tag=1;_tmp469.f1=(struct Cyc_Absyn_Exp*)_tmp44C;_tmp469.f2=
_tmp44D;_tmp469.f3=_tmp44E;_tmp469;});_tmp468;});}return({struct _tuple7 _tmp46A;
_tmp46A.f1=_tmp463;_tmp46A.f2=_tmp464;_tmp46A.f3=({struct Cyc_List_List*_tmp46B=
_cycalloc(sizeof(*_tmp46B));_tmp46B->hd=(void*)tm;_tmp46B->tl=_tmp465;_tmp46B;});
_tmp46A;});}}_LL2D8: if(_tmp448 <= (void*)4  || *((int*)_tmp448)!= 4)goto _LL2DA;
_tmp44F=((struct Cyc_Absyn_PointerType_struct*)_tmp448)->f1;_tmp450=(void*)
_tmp44F.elt_typ;_tmp451=_tmp44F.elt_tq;_tmp452=_tmp44F.ptr_atts;_LL2D9: {struct
Cyc_Absyn_Tqual _tmp46D;void*_tmp46E;struct Cyc_List_List*_tmp46F;struct _tuple7
_tmp46C=Cyc_Absynpp_to_tms(_tmp451,_tmp450);_tmp46D=_tmp46C.f1;_tmp46E=_tmp46C.f2;
_tmp46F=_tmp46C.f3;_tmp46F=({struct Cyc_List_List*_tmp470=_cycalloc(sizeof(*
_tmp470));_tmp470->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp472;_tmp472.tag=2;_tmp472.f1=_tmp452;_tmp472.f2=tq;_tmp472;});_tmp471;}));
_tmp470->tl=_tmp46F;_tmp470;});return({struct _tuple7 _tmp473;_tmp473.f1=_tmp46D;
_tmp473.f2=_tmp46E;_tmp473.f3=_tmp46F;_tmp473;});}_LL2DA: if(_tmp448 <= (void*)4
 || *((int*)_tmp448)!= 8)goto _LL2DC;_tmp453=((struct Cyc_Absyn_FnType_struct*)
_tmp448)->f1;_tmp454=_tmp453.tvars;_tmp455=_tmp453.effect;_tmp456=(void*)_tmp453.ret_typ;
_tmp457=_tmp453.args;_tmp458=_tmp453.c_varargs;_tmp459=_tmp453.cyc_varargs;
_tmp45A=_tmp453.rgn_po;_tmp45B=_tmp453.attributes;_LL2DB: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp455 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp455->v)){
_tmp455=0;_tmp454=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp454);}{struct Cyc_Absyn_Tqual _tmp475;void*_tmp476;struct Cyc_List_List*_tmp477;
struct _tuple7 _tmp474=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(0),_tmp456);
_tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_tmp477=_tmp474.f3;{struct Cyc_List_List*
tms=_tmp477;if(_tmp45B != 0  && !Cyc_Absynpp_to_VC)tms=Cyc_Absynpp_bubble_attributes((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp479;_tmp479.tag=5;_tmp479.f1=
0;_tmp479.f2=_tmp45B;_tmp479;});_tmp478;}),tms);tms=({struct Cyc_List_List*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp47C;_tmp47C.tag=3;_tmp47C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp47E;_tmp47E.tag=1;_tmp47E.f1=_tmp457;_tmp47E.f2=_tmp458;_tmp47E.f3=_tmp459;
_tmp47E.f4=_tmp455;_tmp47E.f5=_tmp45A;_tmp47E;});_tmp47D;}));_tmp47C;});_tmp47B;}));
_tmp47A->tl=tms;_tmp47A;});if(Cyc_Absynpp_to_VC)for(0;_tmp45B != 0;_tmp45B=
_tmp45B->tl){void*_tmp47F=(void*)_tmp45B->hd;_LL2E3: if((int)_tmp47F != 0)goto
_LL2E5;_LL2E4: goto _LL2E6;_LL2E5: if((int)_tmp47F != 1)goto _LL2E7;_LL2E6: goto _LL2E8;
_LL2E7: if((int)_tmp47F != 2)goto _LL2E9;_LL2E8: tms=({struct Cyc_List_List*_tmp480=
_cycalloc(sizeof(*_tmp480));_tmp480->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp482;_tmp482.tag=5;_tmp482.f1=0;_tmp482.f2=({struct Cyc_List_List*_tmp483=
_cycalloc(sizeof(*_tmp483));_tmp483->hd=(void*)((void*)_tmp45B->hd);_tmp483->tl=
0;_tmp483;});_tmp482;});_tmp481;}));_tmp480->tl=tms;_tmp480;});goto AfterAtts;
_LL2E9:;_LL2EA: goto _LL2E2;_LL2E2:;}AfterAtts: if(_tmp454 != 0)tms=({struct Cyc_List_List*
_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp486;_tmp486.tag=4;_tmp486.f1=_tmp454;_tmp486.f2=0;_tmp486.f3=1;_tmp486;});
_tmp485;}));_tmp484->tl=tms;_tmp484;});return({struct _tuple7 _tmp487;_tmp487.f1=
_tmp475;_tmp487.f2=_tmp476;_tmp487.f3=tms;_tmp487;});}}_LL2DC: if(_tmp448 <= (void*)
4  || *((int*)_tmp448)!= 0)goto _LL2DE;_tmp45C=((struct Cyc_Absyn_Evar_struct*)
_tmp448)->f1;_tmp45D=((struct Cyc_Absyn_Evar_struct*)_tmp448)->f2;_tmp45E=((
struct Cyc_Absyn_Evar_struct*)_tmp448)->f3;_LL2DD: if(_tmp45D == 0)return({struct
_tuple7 _tmp488;_tmp488.f1=tq;_tmp488.f2=t;_tmp488.f3=0;_tmp488;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp45D->v);}_LL2DE: if(_tmp448 <= (void*)4  || *((int*)_tmp448)!= 16)
goto _LL2E0;_tmp45F=((struct Cyc_Absyn_TypedefType_struct*)_tmp448)->f1;_tmp460=((
struct Cyc_Absyn_TypedefType_struct*)_tmp448)->f2;_tmp461=((struct Cyc_Absyn_TypedefType_struct*)
_tmp448)->f4;_LL2DF: if((_tmp461 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp461))return({struct _tuple7 _tmp489;_tmp489.f1=tq;_tmp489.f2=t;_tmp489.f3=0;
_tmp489;});else{return Cyc_Absynpp_to_tms(tq,*_tmp461);}_LL2E0:;_LL2E1: return({
struct _tuple7 _tmp48A;_tmp48A.f1=tq;_tmp48A.f2=t;_tmp48A.f3=0;_tmp48A;});_LL2D5:;}
static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp48B=t;struct Cyc_Core_Opt*
_tmp48C;struct Cyc_Core_Opt _tmp48D;void*_tmp48E;struct Cyc_Absyn_PtrInfo _tmp48F;
void*_tmp490;_LL2EC: if(_tmp48B <= (void*)4  || *((int*)_tmp48B)!= 0)goto _LL2EE;
_tmp48C=((struct Cyc_Absyn_Evar_struct*)_tmp48B)->f2;if(_tmp48C == 0)goto _LL2EE;
_tmp48D=*_tmp48C;_tmp48E=(void*)_tmp48D.v;_LL2ED: return Cyc_Absynpp_is_char_ptr(
_tmp48E);_LL2EE: if(_tmp48B <= (void*)4  || *((int*)_tmp48B)!= 4)goto _LL2F0;_tmp48F=((
struct Cyc_Absyn_PointerType_struct*)_tmp48B)->f1;_tmp490=(void*)_tmp48F.elt_typ;
_LL2EF: L: {void*_tmp491=_tmp490;struct Cyc_Core_Opt*_tmp492;struct Cyc_Core_Opt
_tmp493;void*_tmp494;void**_tmp495;void*_tmp496;void*_tmp497;_LL2F3: if(_tmp491 <= (
void*)4  || *((int*)_tmp491)!= 0)goto _LL2F5;_tmp492=((struct Cyc_Absyn_Evar_struct*)
_tmp491)->f2;if(_tmp492 == 0)goto _LL2F5;_tmp493=*_tmp492;_tmp494=(void*)_tmp493.v;
_LL2F4: _tmp490=_tmp494;goto L;_LL2F5: if(_tmp491 <= (void*)4  || *((int*)_tmp491)!= 
16)goto _LL2F7;_tmp495=((struct Cyc_Absyn_TypedefType_struct*)_tmp491)->f4;if(
_tmp495 == 0)goto _LL2F7;_tmp496=*_tmp495;_LL2F6: _tmp490=_tmp496;goto L;_LL2F7: if(
_tmp491 <= (void*)4  || *((int*)_tmp491)!= 5)goto _LL2F9;_tmp497=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp491)->f2;if((int)_tmp497 != 0)goto _LL2F9;_LL2F8: return 1;_LL2F9:;_LL2FA: return
0;_LL2F2:;}_LL2F0:;_LL2F1: return 0;_LL2EB:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual
_tmp499;void*_tmp49A;struct Cyc_List_List*_tmp49B;struct _tuple7 _tmp498=Cyc_Absynpp_to_tms(
tq,typ);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;_tmp49B=_tmp498.f3;_tmp49B=Cyc_List_imp_rev(
_tmp49B);if(_tmp49B == 0  && dopt == 0)return({struct Cyc_PP_Doc*_tmp49C[2];_tmp49C[
1]=Cyc_Absynpp_ntyp2doc(_tmp49A);_tmp49C[0]=Cyc_Absynpp_tqual2doc(_tmp499);Cyc_PP_cat(
_tag_arr(_tmp49C,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp49D[4];_tmp49D[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp49B);_tmp49D[2]=Cyc_PP_text(({
const char*_tmp49E=" ";_tag_arr(_tmp49E,sizeof(char),_get_zero_arr_size(_tmp49E,2));}));
_tmp49D[1]=Cyc_Absynpp_ntyp2doc(_tmp49A);_tmp49D[0]=Cyc_Absynpp_tqual2doc(
_tmp499);Cyc_PP_cat(_tag_arr(_tmp49D,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){if(f->width != 
0)return({struct Cyc_PP_Doc*_tmp49F[5];_tmp49F[4]=Cyc_PP_text(({const char*_tmp4A2=";";
_tag_arr(_tmp4A2,sizeof(char),_get_zero_arr_size(_tmp4A2,2));}));_tmp49F[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp49F[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp49F[1]=Cyc_PP_text(({const char*_tmp4A1=":";_tag_arr(_tmp4A1,
sizeof(char),_get_zero_arr_size(_tmp4A1,2));}));_tmp49F[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));
_tmp4A0->v=Cyc_PP_textptr(f->name);_tmp4A0;}));Cyc_PP_cat(_tag_arr(_tmp49F,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp4A3[3];
_tmp4A3[2]=Cyc_PP_text(({const char*_tmp4A5=";";_tag_arr(_tmp4A5,sizeof(char),
_get_zero_arr_size(_tmp4A5,2));}));_tmp4A3[1]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp4A3[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->v=Cyc_PP_textptr(f->name);_tmp4A4;}));
Cyc_PP_cat(_tag_arr(_tmp4A3,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({const char*_tmp4A6="";
_tag_arr(_tmp4A6,sizeof(char),_get_zero_arr_size(_tmp4A6,1));}),fields);}struct
Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*f){return({
struct Cyc_PP_Doc*_tmp4A7[3];_tmp4A7[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs);_tmp4A7[1]=Cyc_Absynpp_typedef_name2doc(f->name);_tmp4A7[0]=Cyc_Absynpp_scope2doc((
void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp4A7,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Tunionfield*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,({
const char*_tmp4A8=",";_tag_arr(_tmp4A8,sizeof(char),_get_zero_arr_size(_tmp4A8,2));}),
fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp4A9[0]={};Cyc_fprintf(f,({const char*_tmp4AA="\n";
_tag_arr(_tmp4AA,sizeof(char),_get_zero_arr_size(_tmp4AA,2));}),_tag_arr(_tmp4A9,
sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*_tmp4AB="";_tag_arr(
_tmp4AB,sizeof(char),_get_zero_arr_size(_tmp4AB,1));}),((struct Cyc_List_List*(*)(
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
