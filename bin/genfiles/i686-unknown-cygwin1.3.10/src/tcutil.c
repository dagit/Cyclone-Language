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
 struct Cyc_Core_Opt{void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_printf(struct _tagged_arr,struct _tagged_arr);
struct _tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_map2(void*(*f)(
void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_iter(void(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
14];extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple1{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;
struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
int tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
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
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple1*,struct _tuple1*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_tagged_arr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct _tagged_arr
Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);struct _tuple5{
void*f1;struct _tuple1*f2;};struct _tuple5 Cyc_Absyn_aggr_kinded_name(void*);struct
Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,
void*k);struct _tuple6{void*f1;void*f2;};struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);unsigned int Cyc_strlen(struct _tagged_arr s);
int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tuple7{unsigned int f1;int f2;};struct
_tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_flush_warnings();extern struct
Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*t);int Cyc_Tcutil_kind_leq(
void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);void*
Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();int
Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;};struct _tuple10 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int*Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple4*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);char Cyc_Tcutil_Unify[
10]="\000\000\000\000Unify\000";void Cyc_Tcutil_unify_it(void*t1,void*t2);void*
Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)0;int Cyc_Tcutil_tq2_const=0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct _tagged_arr s1=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure));{struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _tagged_arr)(Cyc_Tcutil_tq1_const?({const char*_tmp23="const ";_tag_arr(
_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,7));}):({const char*_tmp24="";
_tag_arr(_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,1));}));{void*_tmp1F[2]={&
_tmp21,& _tmp22};Cyc_aprintf(({const char*_tmp20="%s%s";_tag_arr(_tmp20,sizeof(
char),_get_zero_arr_size(_tmp20,5));}),_tag_arr(_tmp1F,sizeof(void*),2));}}});
struct _tagged_arr s2=(struct _tagged_arr)({struct Cyc_String_pa_struct _tmp1C;_tmp1C.tag=
0;_tmp1C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure));{
struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)(Cyc_Tcutil_tq2_const?({
const char*_tmp1D="const ";_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,
7));}):({const char*_tmp1E="";_tag_arr(_tmp1E,sizeof(char),_get_zero_arr_size(
_tmp1E,1));}));{void*_tmp19[2]={& _tmp1B,& _tmp1C};Cyc_aprintf(({const char*_tmp1A="%s%s";
_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,5));}),_tag_arr(_tmp19,
sizeof(void*),2));}}});int pos=2;({struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;
_tmp2.f1=(struct _tagged_arr)((struct _tagged_arr)s1);{void*_tmp0[1]={& _tmp2};Cyc_fprintf(
Cyc_stderr,({const char*_tmp1="  %s";_tag_arr(_tmp1,sizeof(char),
_get_zero_arr_size(_tmp1,5));}),_tag_arr(_tmp0,sizeof(void*),1));}});pos +=
_get_arr_size(s1,sizeof(char));if(pos + 5 >= 80){({void*_tmp3[0]={};Cyc_fprintf(
Cyc_stderr,({const char*_tmp4="\n\t";_tag_arr(_tmp4,sizeof(char),
_get_zero_arr_size(_tmp4,3));}),_tag_arr(_tmp3,sizeof(void*),0));});pos=8;}else{({
void*_tmp5[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp6=" ";_tag_arr(_tmp6,
sizeof(char),_get_zero_arr_size(_tmp6,2));}),_tag_arr(_tmp5,sizeof(void*),0));});
pos ++;}({void*_tmp7[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp8="and ";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,5));}),_tag_arr(_tmp7,
sizeof(void*),0));});pos +=4;if(pos + _get_arr_size(s2,sizeof(char))>= 80){({void*
_tmp9[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpA="\n\t";_tag_arr(_tmpA,
sizeof(char),_get_zero_arr_size(_tmpA,3));}),_tag_arr(_tmp9,sizeof(void*),0));});
pos=8;}({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _tagged_arr)((
struct _tagged_arr)s2);{void*_tmpB[1]={& _tmpD};Cyc_fprintf(Cyc_stderr,({const char*
_tmpC="%s ";_tag_arr(_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,4));}),_tag_arr(
_tmpB,sizeof(void*),1));}});pos +=_get_arr_size(s2,sizeof(char))+ 1;if(pos + 17 >= 
80){({void*_tmpE[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmpF="\n\t";_tag_arr(
_tmpF,sizeof(char),_get_zero_arr_size(_tmpF,3));}),_tag_arr(_tmpE,sizeof(void*),
0));});pos=8;}({void*_tmp10[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp11="are not compatible. ";
_tag_arr(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,21));}),_tag_arr(_tmp10,
sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){if(pos + Cyc_strlen((struct _tagged_arr)Cyc_Tcutil_failure_reason)
>= 80)({void*_tmp12[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="\n\t";
_tag_arr(_tmp13,sizeof(char),_get_zero_arr_size(_tmp13,3));}),_tag_arr(_tmp12,
sizeof(void*),0));});({struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_failure_reason);{void*_tmp14[1]={&
_tmp16};Cyc_fprintf(Cyc_stderr,({const char*_tmp15="%s";_tag_arr(_tmp15,sizeof(
char),_get_zero_arr_size(_tmp15,3));}),_tag_arr(_tmp14,sizeof(void*),1));}});}({
void*_tmp17[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="\n";_tag_arr(_tmp18,
sizeof(char),_get_zero_arr_size(_tmp18,2));}),_tag_arr(_tmp17,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)((struct _tagged_arr)msg);{void*
_tmp25[1]={& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="Compiler Error (Tcutil::impos): %s\n";
_tag_arr(_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,36));}),_tag_arr(_tmp25,
sizeof(void*),1));}});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp28=_cycalloc(sizeof(*_tmp28));
_tmp28[0]=({struct Cyc_Core_Impossible_struct _tmp29;_tmp29.tag=Cyc_Core_Impossible;
_tmp29.f1=msg;_tmp29;});_tmp28;}));}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;
_tmp2D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));{struct Cyc_String_pa_struct _tmp2C;
_tmp2C.tag=0;_tmp2C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd));{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B="%s::%s ";_tag_arr(_tmp2B,sizeof(char),
_get_zero_arr_size(_tmp2B,8));}),_tag_arr(_tmp2A,sizeof(void*),2));}}});}({void*
_tmp2E[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="\n";_tag_arr(_tmp2F,
sizeof(char),_get_zero_arr_size(_tmp2F,2));}),_tag_arr(_tmp2E,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=
0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct _tagged_arr ap){struct
_tagged_arr msg=(struct _tagged_arr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=sg;_tmp30->tl=
Cyc_Tcutil_warning_segs;_tmp30;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*
_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=({struct _tagged_arr*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=msg;_tmp32;});_tmp31->tl=Cyc_Tcutil_warning_msgs;
_tmp31;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;({void*_tmp33[0]={};Cyc_fprintf(Cyc_stderr,({const char*_tmp34="***Warnings***\n";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,16));}),_tag_arr(_tmp33,
sizeof(void*),0));});{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=
0;_tmp39.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct _tagged_arr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));{struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct
_tagged_arr*)((struct Cyc_List_List*)_check_null(_tmp35))->hd));{void*_tmp36[2]={&
_tmp38,& _tmp39};Cyc_fprintf(Cyc_stderr,({const char*_tmp37="%s: %s\n";_tag_arr(
_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,8));}),_tag_arr(_tmp36,sizeof(void*),
2));}}});_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp3A[0]={};Cyc_fprintf(Cyc_stderr,({
const char*_tmp3B="**************\n";_tag_arr(_tmp3B,sizeof(char),
_get_zero_arr_size(_tmp3B,16));}),_tag_arr(_tmp3A,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return*((int*)_check_null(tv1->identity))- *((int*)_check_null(tv2->identity));}
void*Cyc_Tcutil_compress(void*t){void*_tmp3C=t;struct Cyc_Core_Opt*_tmp3D;void**
_tmp3E;void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*_tmp41;struct Cyc_Core_Opt**
_tmp42;_LL1: if(_tmp3C <= (void*)3  || *((int*)_tmp3C)!= 0)goto _LL3;_tmp3D=((struct
Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D != 0)goto _LL3;_LL2: goto _LL4;_LL3: if(
_tmp3C <= (void*)3  || *((int*)_tmp3C)!= 16)goto _LL5;_tmp3E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4: return t;_LL5: if(_tmp3C <= (void*)3  || *((
int*)_tmp3C)!= 16)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;_LL6: {void*
t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((void**)
_check_null(*_tmp40)))*_tmp40=({void**_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=
t2;_tmp43;});return t2;}_LL7: if(_tmp3C <= (void*)3  || *((int*)_tmp3C)!= 0)goto _LL9;
_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v)*_tmp42=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44->v=(void*)t2;_tmp44;});return t2;}_LL9:;_LLA: return t;_LL0:;}void*
Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp45=(void*)c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((int)_tmp45
!= 0)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if(_tmp45 <= (void*)1  || *((
int*)_tmp45)!= 0)goto _LL10;_tmp46=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp45)->f1;_LLF: return Cyc_Absyn_new_conref(_tmp46);_LL10: if(_tmp45 <= (void*)1
 || *((int*)_tmp45)!= 1)goto _LLB;_tmp47=((struct Cyc_Absyn_Forward_constr_struct*)
_tmp45)->f1;_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp48=Cyc_Absyn_compress_kb(kb);void*_tmp49;void*_tmp4A;_LL13: if(*((
int*)_tmp48)!= 0)goto _LL15;_tmp49=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp48)->f1;
_LL14: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(
void*)_tmp49;_tmp4C;});_tmp4B;});_LL15: if(*((int*)_tmp48)!= 1)goto _LL17;_LL16:
return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp4E;_tmp4E.tag=1;_tmp4E.f1=0;
_tmp4E;});_tmp4D;});_LL17: if(*((int*)_tmp48)!= 2)goto _LL12;_tmp4A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp48)->f2;_LL18: return(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp50;_tmp50.tag=2;_tmp50.f1=0;_tmp50.f2=(void*)_tmp4A;_tmp50;});_tmp4F;});
_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name;
_tmp51->identity=0;_tmp51->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);
_tmp51;});}static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct
_tuple2 _tmp53;struct Cyc_Core_Opt*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;
struct _tuple2*_tmp52=arg;_tmp53=*_tmp52;_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_tmp56=
_tmp53.f3;return({struct _tuple2*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=
_tmp54;_tmp57->f2=_tmp55;_tmp57->f3=Cyc_Tcutil_copy_type(_tmp56);_tmp57;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){struct _tuple4 _tmp59;
struct Cyc_Absyn_Tqual _tmp5A;void*_tmp5B;struct _tuple4*_tmp58=arg;_tmp59=*_tmp58;
_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;return({struct _tuple4*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C->f1=_tmp5A;_tmp5C->f2=Cyc_Tcutil_copy_type(_tmp5B);_tmp5C;});}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->name=
f->name;_tmp5D->tq=f->tq;_tmp5D->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp5D->width=f->width;_tmp5D->attributes=f->attributes;_tmp5D;});}static struct
_tuple6*Cyc_Tcutil_copy_rgncmp(struct _tuple6*x){struct _tuple6 _tmp5F;void*_tmp60;
void*_tmp61;struct _tuple6*_tmp5E=x;_tmp5F=*_tmp5E;_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;
return({struct _tuple6*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=Cyc_Tcutil_copy_type(
_tmp60);_tmp62->f2=Cyc_Tcutil_copy_type(_tmp61);_tmp62;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->name=f->name;_tmp63->tag=f->tag;_tmp63->loc=
f->loc;_tmp63;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp64=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp65;struct Cyc_Absyn_TunionInfo _tmp66;void*_tmp67;
struct Cyc_List_List*_tmp68;void*_tmp69;struct Cyc_Absyn_TunionFieldInfo _tmp6A;
void*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_PtrInfo _tmp6D;void*_tmp6E;
struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_PtrAtts _tmp70;void*_tmp71;struct Cyc_Absyn_Conref*
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;void*_tmp75;
void*_tmp76;int _tmp77;struct Cyc_Absyn_ArrayInfo _tmp78;void*_tmp79;struct Cyc_Absyn_Tqual
_tmp7A;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_Absyn_Conref*_tmp7C;struct Cyc_Absyn_FnInfo
_tmp7D;struct Cyc_List_List*_tmp7E;struct Cyc_Core_Opt*_tmp7F;void*_tmp80;struct
Cyc_List_List*_tmp81;int _tmp82;struct Cyc_Absyn_VarargInfo*_tmp83;struct Cyc_List_List*
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_AggrInfo
_tmp87;void*_tmp88;void*_tmp89;struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;
struct Cyc_Absyn_AggrInfo _tmp8C;void*_tmp8D;struct Cyc_Absyn_Aggrdecl**_tmp8E;
struct Cyc_List_List*_tmp8F;void*_tmp90;struct Cyc_List_List*_tmp91;struct _tuple1*
_tmp92;struct Cyc_Absyn_Enumdecl*_tmp93;struct Cyc_List_List*_tmp94;void*_tmp95;
int _tmp96;void*_tmp97;void*_tmp98;struct _tuple1*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_Absyn_Typedefdecl*_tmp9B;void*_tmp9C;struct Cyc_List_List*_tmp9D;
void*_tmp9E;_LL1A: if((int)_tmp64 != 0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 0)goto _LL1E;_LL1D: return t;_LL1E: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 1)goto _LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)
_tmp64)->f1;_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*_tmp9F=_cycalloc(
sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_VarType_struct _tmpA0;_tmpA0.tag=1;
_tmpA0.f1=Cyc_Tcutil_copy_tvar(_tmp65);_tmpA0;});_tmp9F;});_LL20: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)
_tmp64)->f1;_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)
_tmp66.rgn;_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA1=
_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_TunionType_struct _tmpA2;
_tmpA2.tag=2;_tmpA2.f1=({struct Cyc_Absyn_TunionInfo _tmpA3;_tmpA3.tunion_info=(
void*)_tmp67;_tmpA3.targs=Cyc_Tcutil_copy_types(_tmp68);_tmpA3.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp69);_tmpA3;});_tmpA2;});_tmpA1;});_LL22: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 3)goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;
_tmp6B=(void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct
Cyc_Absyn_TunionFieldType_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmpA5;_tmpA5.tag=3;_tmpA5.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmpA6;_tmpA6.field_info=(void*)_tmp6B;_tmpA6.targs=Cyc_Tcutil_copy_types(_tmp6C);
_tmpA6;});_tmpA5;});_tmpA4;});_LL24: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 4)
goto _LL26;_tmp6D=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)
_tmp6D.elt_typ;_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;
_tmp72=_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_LL25: {void*
_tmpA7=Cyc_Tcutil_copy_type(_tmp6E);void*_tmpA8=Cyc_Tcutil_copy_type(_tmp71);
struct Cyc_Absyn_Conref*_tmpA9=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp72);struct Cyc_Absyn_Tqual _tmpAA=_tmp6F;struct Cyc_Absyn_Conref*
_tmpAB=Cyc_Tcutil_copy_conref(_tmp73);struct Cyc_Absyn_Conref*_tmpAC=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({
struct Cyc_Absyn_PointerType_struct _tmpAE;_tmpAE.tag=4;_tmpAE.f1=({struct Cyc_Absyn_PtrInfo
_tmpAF;_tmpAF.elt_typ=(void*)_tmpA7;_tmpAF.elt_tq=_tmpAA;_tmpAF.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB0;_tmpB0.rgn=(void*)_tmpA8;_tmpB0.nullable=_tmpA9;
_tmpB0.bounds=_tmpAB;_tmpB0.zero_term=_tmpAC;_tmpB0;});_tmpAF;});_tmpAE;});
_tmpAD;});}_LL26: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 5)goto _LL28;_tmp75=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f1;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64)->f2;_LL27: return(void*)({struct Cyc_Absyn_IntType_struct*_tmpB1=_cycalloc(
sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_IntType_struct _tmpB2;_tmpB2.tag=5;
_tmpB2.f1=(void*)_tmp75;_tmpB2.f2=(void*)_tmp76;_tmpB2;});_tmpB1;});_LL28: if((
int)_tmp64 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 6)goto _LL2C;_tmp77=((struct Cyc_Absyn_DoubleType_struct*)_tmp64)->f1;
_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB3=_cycalloc_atomic(
sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_DoubleType_struct _tmpB4;_tmpB4.tag=
6;_tmpB4.f1=_tmp77;_tmpB4;});_tmpB3;});_LL2C: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 7)goto _LL2E;_tmp78=((struct Cyc_Absyn_ArrayType_struct*)_tmp64)->f1;
_tmp79=(void*)_tmp78.elt_type;_tmp7A=_tmp78.tq;_tmp7B=_tmp78.num_elts;_tmp7C=
_tmp78.zero_term;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_ArrayType_struct _tmpB6;
_tmpB6.tag=7;_tmpB6.f1=({struct Cyc_Absyn_ArrayInfo _tmpB7;_tmpB7.elt_type=(void*)
Cyc_Tcutil_copy_type(_tmp79);_tmpB7.tq=_tmp7A;_tmpB7.num_elts=_tmp7B;_tmpB7.zero_term=((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp7C);_tmpB7;});_tmpB6;});_tmpB5;});_LL2E: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 8)goto _LL30;_tmp7D=((struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp7E=
_tmp7D.tvars;_tmp7F=_tmp7D.effect;_tmp80=(void*)_tmp7D.ret_typ;_tmp81=_tmp7D.args;
_tmp82=_tmp7D.c_varargs;_tmp83=_tmp7D.cyc_varargs;_tmp84=_tmp7D.rgn_po;_tmp85=
_tmp7D.attributes;_LL2F: {struct Cyc_List_List*_tmpB8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp7E);struct Cyc_Core_Opt*_tmpB9=_tmp7F == 0?0:({struct Cyc_Core_Opt*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp7F->v);_tmpC3;});void*_tmpBA=Cyc_Tcutil_copy_type(_tmp80);struct Cyc_List_List*
_tmpBB=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp81);int _tmpBC=_tmp82;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp83 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp83;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBD=_cycalloc(sizeof(*_tmpBD));
_tmpBD->name=cv->name;_tmpBD->tq=cv->tq;_tmpBD->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpBD->inject=cv->inject;_tmpBD;});}{struct Cyc_List_List*_tmpBE=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp84);struct Cyc_List_List*_tmpBF=_tmp85;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));
_tmpC0[0]=({struct Cyc_Absyn_FnType_struct _tmpC1;_tmpC1.tag=8;_tmpC1.f1=({struct
Cyc_Absyn_FnInfo _tmpC2;_tmpC2.tvars=_tmpB8;_tmpC2.effect=_tmpB9;_tmpC2.ret_typ=(
void*)_tmpBA;_tmpC2.args=_tmpBB;_tmpC2.c_varargs=_tmpBC;_tmpC2.cyc_varargs=
cyc_varargs2;_tmpC2.rgn_po=_tmpBE;_tmpC2.attributes=_tmpBF;_tmpC2;});_tmpC1;});
_tmpC0;});}}_LL30: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 9)goto _LL32;_tmp86=((
struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_TupleType_struct
_tmpC5;_tmpC5.tag=9;_tmpC5.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86);
_tmpC5;});_tmpC4;});_LL32: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 10)goto _LL34;
_tmp87=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;_tmp88=(void*)_tmp87.aggr_info;
if(*((int*)_tmp88)!= 0)goto _LL34;_tmp89=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp88)->f1;_tmp8A=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp88)->f2;_tmp8B=
_tmp87.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC6=
_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AggrType_struct _tmpC7;
_tmpC7.tag=10;_tmpC7.f1=({struct Cyc_Absyn_AggrInfo _tmpC8;_tmpC8.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(
void*)_tmp89;_tmpCA.f2=_tmp8A;_tmpCA;});_tmpC9;}));_tmpC8.targs=Cyc_Tcutil_copy_types(
_tmp8B);_tmpC8;});_tmpC7;});_tmpC6;});_LL34: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 10)goto _LL36;_tmp8C=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;
_tmp8D=(void*)_tmp8C.aggr_info;if(*((int*)_tmp8D)!= 1)goto _LL36;_tmp8E=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp8D)->f1;_tmp8F=_tmp8C.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_AggrType_struct _tmpCC;_tmpCC.tag=10;_tmpCC.f1=({struct Cyc_Absyn_AggrInfo
_tmpCD;_tmpCD.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpCF;
_tmpCF.tag=1;_tmpCF.f1=_tmp8E;_tmpCF;});_tmpCE;}));_tmpCD.targs=Cyc_Tcutil_copy_types(
_tmp8F);_tmpCD;});_tmpCC;});_tmpCB;});_LL36: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 11)goto _LL38;_tmp90=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp91=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD0=_cycalloc(sizeof(*
_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD1;_tmpD1.tag=11;
_tmpD1.f1=(void*)_tmp90;_tmpD1.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp91);_tmpD1;});_tmpD0;});_LL38: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 12)
goto _LL3A;_tmp92=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp93=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD3;_tmpD3.tag=12;_tmpD3.f1=_tmp92;_tmpD3.f2=_tmp93;_tmpD3;});_tmpD2;});_LL3A:
if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 13)goto _LL3C;_tmp94=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD4=
_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD5;
_tmpD5.tag=13;_tmpD5.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp94);_tmpD5;});_tmpD4;});_LL3C: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 14)
goto _LL3E;_tmp95=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));
_tmpD6[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD7;_tmpD7.tag=14;_tmpD7.f1=(
void*)Cyc_Tcutil_copy_type(_tmp95);_tmpD7;});_tmpD6;});_LL3E: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 18)goto _LL40;_tmp96=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpD8=
_cycalloc_atomic(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpD9;_tmpD9.tag=18;_tmpD9.f1=_tmp96;_tmpD9;});_tmpD8;});_LL40: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 17)goto _LL42;_tmp97=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDA=_cycalloc(
sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TagType_struct _tmpDB;_tmpDB.tag=17;
_tmpDB.f1=(void*)Cyc_Tcutil_copy_type(_tmp97);_tmpDB;});_tmpDA;});_LL42: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 15)goto _LL44;_tmp98=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDD;_tmpDD.tag=15;_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp98);_tmpDD;});
_tmpDC;});_LL44: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 16)goto _LL46;_tmp99=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpDE=_cycalloc(sizeof(*
_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_TypedefType_struct _tmpDF;_tmpDF.tag=16;
_tmpDF.f1=_tmp99;_tmpDF.f2=Cyc_Tcutil_copy_types(_tmp9A);_tmpDF.f3=_tmp9B;_tmpDF.f4=
0;_tmpDF;});_tmpDE;});_LL46: if((int)_tmp64 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 19)goto _LL4A;_tmp9C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE0=
_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE1;
_tmpE1.tag=19;_tmpE1.f1=(void*)Cyc_Tcutil_copy_type(_tmp9C);_tmpE1;});_tmpE0;});
_LL4A: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 20)goto _LL4C;_tmp9D=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE2=_cycalloc(
sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE3;_tmpE3.tag=20;
_tmpE3.f1=Cyc_Tcutil_copy_types(_tmp9D);_tmpE3;});_tmpE2;});_LL4C: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 21)goto _LL19;_tmp9E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpE4=_cycalloc(
sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpE5;_tmpE5.tag=21;
_tmpE5.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE5;});_tmpE4;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpE7=({struct _tuple6 _tmpE6;
_tmpE6.f1=k1;_tmpE6.f2=k2;_tmpE6;});void*_tmpE8;void*_tmpE9;void*_tmpEA;void*
_tmpEB;void*_tmpEC;void*_tmpED;_LL4F: _tmpE8=_tmpE7.f1;if((int)_tmpE8 != 2)goto
_LL51;_tmpE9=_tmpE7.f2;if((int)_tmpE9 != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpEA=
_tmpE7.f1;if((int)_tmpEA != 2)goto _LL53;_tmpEB=_tmpE7.f2;if((int)_tmpEB != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpEC=_tmpE7.f1;if((int)_tmpEC != 1)goto _LL55;_tmpED=
_tmpE7.f2;if((int)_tmpED != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEE=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpEF;void*_tmpF0;_LL58: if(*((int*)_tmpEE)!= 0)goto _LL5A;
_tmpEF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEE)->f1;_LL59: return _tmpEF;
_LL5A: if(*((int*)_tmpEE)!= 2)goto _LL5C;_tmpF0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEE)->f2;_LL5B: return _tmpF0;_LL5C:;_LL5D:({void*_tmpF1[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF2="kind not suitably constrained!";
_tag_arr(_tmpF2,sizeof(char),_get_zero_arr_size(_tmpF2,31));}),_tag_arr(_tmpF1,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF3=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpF4;struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Tvar*
_tmpF6;void*_tmpF7;struct Cyc_Absyn_TunionFieldInfo _tmpF8;void*_tmpF9;struct Cyc_Absyn_Tunionfield*
_tmpFA;struct Cyc_Absyn_TunionFieldInfo _tmpFB;void*_tmpFC;struct Cyc_Absyn_Enumdecl*
_tmpFD;struct Cyc_Absyn_AggrInfo _tmpFE;void*_tmpFF;struct Cyc_Absyn_AggrInfo
_tmp100;void*_tmp101;struct Cyc_Absyn_Aggrdecl**_tmp102;struct Cyc_Absyn_Aggrdecl*
_tmp103;struct Cyc_Absyn_Aggrdecl _tmp104;struct Cyc_Absyn_AggrdeclImpl*_tmp105;
struct Cyc_Absyn_Enumdecl*_tmp106;struct Cyc_Absyn_PtrInfo _tmp107;struct Cyc_Absyn_Typedefdecl*
_tmp108;_LL5F: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 0)goto _LL61;_tmpF4=((
struct Cyc_Absyn_Evar_struct*)_tmpF3)->f1;_tmpF5=((struct Cyc_Absyn_Evar_struct*)
_tmpF3)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4))->v;
_LL61: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 1)goto _LL63;_tmpF6=((struct Cyc_Absyn_VarType_struct*)
_tmpF3)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpF6);_LL63: if((int)_tmpF3 != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 5)
goto _LL67;_tmpF7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF3)->f2;_LL66:
return _tmpF7 == (void*)((void*)2)?(void*)2:(void*)1;_LL67: if((int)_tmpF3 != 1)goto
_LL69;_LL68: goto _LL6A;_LL69: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 6)goto
_LL6B;_LL6A: goto _LL6C;_LL6B: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 8)goto
_LL6D;_LL6C: return(void*)1;_LL6D: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 15)
goto _LL6F;_LL6E: return(void*)2;_LL6F: if((int)_tmpF3 != 2)goto _LL71;_LL70: return(
void*)3;_LL71: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 2)goto _LL73;_LL72: return(
void*)2;_LL73: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 3)goto _LL75;_tmpF8=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF3)->f1;_tmpF9=(void*)_tmpF8.field_info;
if(*((int*)_tmpF9)!= 1)goto _LL75;_tmpFA=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpF9)->f2;_LL74: if(_tmpFA->typs == 0)return(void*)2;else{return(void*)1;}_LL75:
if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 3)goto _LL77;_tmpFB=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF3)->f1;_tmpFC=(void*)_tmpFB.field_info;if(*((int*)_tmpFC)!= 0)goto _LL77;
_LL76:({void*_tmp109[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp10A="typ_kind: Unresolved TunionFieldType";
_tag_arr(_tmp10A,sizeof(char),_get_zero_arr_size(_tmp10A,37));}),_tag_arr(
_tmp109,sizeof(void*),0));});_LL77: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 12)
goto _LL79;_tmpFD=((struct Cyc_Absyn_EnumType_struct*)_tmpF3)->f2;if(_tmpFD != 0)
goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 10)
goto _LL7B;_tmpFE=((struct Cyc_Absyn_AggrType_struct*)_tmpF3)->f1;_tmpFF=(void*)
_tmpFE.aggr_info;if(*((int*)_tmpFF)!= 0)goto _LL7B;_LL7A: return(void*)0;_LL7B: if(
_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 10)goto _LL7D;_tmp100=((struct Cyc_Absyn_AggrType_struct*)
_tmpF3)->f1;_tmp101=(void*)_tmp100.aggr_info;if(*((int*)_tmp101)!= 1)goto _LL7D;
_tmp102=((struct Cyc_Absyn_KnownAggr_struct*)_tmp101)->f1;_tmp103=*_tmp102;
_tmp104=*_tmp103;_tmp105=_tmp104.impl;_LL7C: return _tmp105 == 0?(void*)0:(void*)1;
_LL7D: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 11)goto _LL7F;_LL7E: goto _LL80;
_LL7F: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 13)goto _LL81;_LL80: return(void*)
1;_LL81: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 12)goto _LL83;_tmp106=((struct
Cyc_Absyn_EnumType_struct*)_tmpF3)->f2;_LL82: if(_tmp106->fields == 0)return(void*)
0;else{return(void*)1;}_LL83: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 4)goto
_LL85;_tmp107=((struct Cyc_Absyn_PointerType_struct*)_tmpF3)->f1;_LL84: {void*
_tmp10B=(void*)(Cyc_Absyn_compress_conref((_tmp107.ptr_atts).bounds))->v;void*
_tmp10C;void*_tmp10D;void*_tmp10E;_LL98: if((int)_tmp10B != 0)goto _LL9A;_LL99: goto
_LL9B;_LL9A: if(_tmp10B <= (void*)1  || *((int*)_tmp10B)!= 0)goto _LL9C;_tmp10C=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if((int)_tmp10C != 0)goto
_LL9C;_LL9B: return(void*)1;_LL9C: if(_tmp10B <= (void*)1  || *((int*)_tmp10B)!= 0)
goto _LL9E;_tmp10D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10B)->f1;if(
_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(
_tmp10B <= (void*)1  || *((int*)_tmp10B)!= 0)goto _LLA0;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10B)->f1;if(_tmp10E <= (void*)1  || *((int*)_tmp10E)!= 1)goto _LLA0;_LL9F:
return(void*)2;_LLA0: if(_tmp10B <= (void*)1  || *((int*)_tmp10B)!= 1)goto _LL97;
_LLA1:({void*_tmp10F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp110="typ_kind: forward constr in ptr bounds";
_tag_arr(_tmp110,sizeof(char),_get_zero_arr_size(_tmp110,39));}),_tag_arr(
_tmp10F,sizeof(void*),0));});_LL97:;}_LL85: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 14)goto _LL87;_LL86: return(void*)2;_LL87: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 18)goto _LL89;_LL88: return(void*)5;_LL89: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 17)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 7)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 9)goto _LL8F;_LL8E: return(void*)1;_LL8F: if(_tmpF3 <= (void*)3  || *((int*)
_tmpF3)!= 16)goto _LL91;_tmp108=((struct Cyc_Absyn_TypedefType_struct*)_tmpF3)->f3;
_LL90: if(_tmp108 == 0  || _tmp108->kind == 0)({struct Cyc_String_pa_struct _tmp113;
_tmp113.tag=0;_tmp113.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp111[1]={& _tmp113};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp112="typ_kind: typedef found: %s";_tag_arr(
_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,28));}),_tag_arr(_tmp111,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp108->kind))->v;
_LL91: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 19)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 20)goto _LL95;_LL94: goto _LL96;
_LL95: if(_tmpF3 <= (void*)3  || *((int*)_tmpF3)!= 21)goto _LL5E;_LL96: return(void*)
4;_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp114;
_push_handler(& _tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))_tmp116=1;if(!
_tmp116){Cyc_Tcutil_unify_it(t1,t2);{int _tmp117=1;_npop_handler(0);return _tmp117;};
_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp119=_tmp115;_LLA3:
if(_tmp119 != Cyc_Tcutil_Unify)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(
_tmp119);_LLA2:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp11A=t;struct Cyc_Absyn_Tvar*_tmp11B;struct Cyc_Core_Opt*_tmp11C;
struct Cyc_Core_Opt*_tmp11D;struct Cyc_Core_Opt**_tmp11E;struct Cyc_Absyn_PtrInfo
_tmp11F;struct Cyc_Absyn_ArrayInfo _tmp120;void*_tmp121;struct Cyc_Absyn_FnInfo
_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Core_Opt*_tmp124;void*_tmp125;
struct Cyc_List_List*_tmp126;int _tmp127;struct Cyc_Absyn_VarargInfo*_tmp128;struct
Cyc_List_List*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp12B;
struct Cyc_Absyn_TunionInfo _tmp12C;struct Cyc_List_List*_tmp12D;void*_tmp12E;
struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_TunionFieldInfo _tmp130;struct Cyc_List_List*
_tmp131;struct Cyc_Absyn_AggrInfo _tmp132;struct Cyc_List_List*_tmp133;struct Cyc_List_List*
_tmp134;void*_tmp135;void*_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;struct
Cyc_List_List*_tmp13A;_LLA8: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 1)goto
_LLAA;_tmp11B=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;_LLA9: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp11B)){Cyc_Tcutil_failure_reason=({
const char*_tmp13B="(type variable would escape scope)";_tag_arr(_tmp13B,sizeof(
char),_get_zero_arr_size(_tmp13B,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLA7;_LLAA: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 0)goto _LLAC;_tmp11C=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f4;_tmp11E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13C="(occurs check)";
_tag_arr(_tmp13C,sizeof(char),_get_zero_arr_size(_tmp13C,15));});(int)_throw((
void*)Cyc_Tcutil_Unify);}else{if(_tmp11C != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)
_tmp11C->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp11E))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp13D=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp11E))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp13D=({struct Cyc_List_List*_tmp13E=
_cycalloc(sizeof(*_tmp13E));_tmp13E->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp13E->tl=
_tmp13D;_tmp13E;});}}*_tmp11E=({struct Cyc_Core_Opt*_tmp13F=_cycalloc(sizeof(*
_tmp13F));_tmp13F->v=_tmp13D;_tmp13F;});}}}goto _LLA7;_LLAC: if(_tmp11A <= (void*)3
 || *((int*)_tmp11A)!= 4)goto _LLAE;_tmp11F=((struct Cyc_Absyn_PointerType_struct*)
_tmp11A)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp11F.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp11F.ptr_atts).rgn);{void*_tmp140=(void*)(Cyc_Absyn_compress_conref((
_tmp11F.ptr_atts).bounds))->v;void*_tmp141;void*_tmp142;_LLCD: if(_tmp140 <= (void*)
1  || *((int*)_tmp140)!= 0)goto _LLCF;_tmp141=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp140)->f1;if(_tmp141 <= (void*)1  || *((int*)_tmp141)!= 1)goto _LLCF;_tmp142=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp141)->f1;_LLCE: Cyc_Tcutil_occurs(
evar,r,env,_tmp142);goto _LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(
_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 7)goto _LLB0;_tmp120=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11A)->f1;_tmp121=(void*)_tmp120.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,
_tmp121);goto _LLA7;_LLB0: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 8)goto _LLB2;
_tmp122=((struct Cyc_Absyn_FnType_struct*)_tmp11A)->f1;_tmp123=_tmp122.tvars;
_tmp124=_tmp122.effect;_tmp125=(void*)_tmp122.ret_typ;_tmp126=_tmp122.args;
_tmp127=_tmp122.c_varargs;_tmp128=_tmp122.cyc_varargs;_tmp129=_tmp122.rgn_po;
_tmp12A=_tmp122.attributes;_LLB1: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp123,env);if(_tmp124 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp124->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp125);for(0;_tmp126 != 0;_tmp126=_tmp126->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp126->hd)).f3);}if(_tmp128 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp128->type);for(0;_tmp129 != 0;_tmp129=_tmp129->tl){struct
_tuple6 _tmp144;void*_tmp145;void*_tmp146;struct _tuple6*_tmp143=(struct _tuple6*)
_tmp129->hd;_tmp144=*_tmp143;_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp145);Cyc_Tcutil_occurs(evar,r,env,_tmp146);}goto _LLA7;_LLB2: if(
_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 9)goto _LLB4;_tmp12B=((struct Cyc_Absyn_TupleType_struct*)
_tmp11A)->f1;_LLB3: for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12B->hd)).f2);}goto _LLA7;_LLB4: if(_tmp11A <= (void*)
3  || *((int*)_tmp11A)!= 2)goto _LLB6;_tmp12C=((struct Cyc_Absyn_TunionType_struct*)
_tmp11A)->f1;_tmp12D=_tmp12C.targs;_tmp12E=(void*)_tmp12C.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp12E);Cyc_Tcutil_occurslist(evar,r,env,_tmp12D);goto _LLA7;_LLB6:
if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 16)goto _LLB8;_tmp12F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_LLB7: _tmp131=_tmp12F;goto _LLB9;_LLB8: if(_tmp11A <= (void*)3  || *((
int*)_tmp11A)!= 3)goto _LLBA;_tmp130=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11A)->f1;_tmp131=_tmp130.targs;_LLB9: _tmp133=_tmp131;goto _LLBB;_LLBA: if(
_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 10)goto _LLBC;_tmp132=((struct Cyc_Absyn_AggrType_struct*)
_tmp11A)->f1;_tmp133=_tmp132.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp133);goto _LLA7;_LLBC: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 11)goto
_LLBE;_tmp134=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLBD: for(0;
_tmp134 != 0;_tmp134=_tmp134->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp134->hd)->type);}goto _LLA7;_LLBE: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 
17)goto _LLC0;_tmp135=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLBF:
_tmp136=_tmp135;goto _LLC1;_LLC0: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 14)
goto _LLC2;_tmp136=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11A)->f1;_LLC1:
_tmp137=_tmp136;goto _LLC3;_LLC2: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 19)
goto _LLC4;_tmp137=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11A)->f1;_LLC3:
_tmp138=_tmp137;goto _LLC5;_LLC4: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)!= 15)
goto _LLC6;_tmp138=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;
_LLC5: _tmp139=_tmp138;goto _LLC7;_LLC6: if(_tmp11A <= (void*)3  || *((int*)_tmp11A)
!= 21)goto _LLC8;_tmp139=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;
_LLC7: Cyc_Tcutil_occurs(evar,r,env,_tmp139);goto _LLA7;_LLC8: if(_tmp11A <= (void*)
3  || *((int*)_tmp11A)!= 20)goto _LLCA;_tmp13A=((struct Cyc_Absyn_JoinEff_struct*)
_tmp11A)->f1;_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp13A);goto _LLA7;_LLCA:;
_LLCB: goto _LLA7;_LLA7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp147[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp148="tq1 real_const not set.";
_tag_arr(_tmp148,sizeof(char),_get_zero_arr_size(_tmp148,24));}),_tag_arr(
_tmp147,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({void*
_tmp149[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp14A="tq2 real_const not set.";_tag_arr(_tmp14A,sizeof(char),
_get_zero_arr_size(_tmp14A,24));}),_tag_arr(_tmp149,sizeof(void*),0));});if((tq1.real_const
!= tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({const char*
_tmp14B="(qualifiers don't match)";_tag_arr(_tmp14B,sizeof(char),
_get_zero_arr_size(_tmp14B,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp14C=(void*)x->v;void*_tmp14D;_LLD2: if((int)_tmp14C != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp14F;_tmp14F.tag=1;_tmp14F.f1=y;_tmp14F;});_tmp14E;})));return;_LLD4: if(
_tmp14C <= (void*)1  || *((int*)_tmp14C)!= 0)goto _LLD6;_tmp14D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14C)->f1;_LLD5: {void*_tmp150=(void*)y->v;void*_tmp151;_LLD9: if((int)_tmp150
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp150
<= (void*)1  || *((int*)_tmp150)!= 0)goto _LLDD;_tmp151=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp150)->f1;_LLDC: if(cmp(_tmp14D,_tmp151)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp150 <= (void*)1  || *((
int*)_tmp150)!= 1)goto _LLD8;_LLDE:({void*_tmp152[0]={};Cyc_Tcutil_impos(({const
char*_tmp153="unify_conref: forward after compress(2)";_tag_arr(_tmp153,sizeof(
char),_get_zero_arr_size(_tmp153,40));}),_tag_arr(_tmp152,sizeof(void*),0));});
_LLD8:;}_LLD6: if(_tmp14C <= (void*)1  || *((int*)_tmp14C)!= 1)goto _LLD1;_LLD7:({
void*_tmp154[0]={};Cyc_Tcutil_impos(({const char*_tmp155="unify_conref: forward after compress";
_tag_arr(_tmp155,sizeof(char),_get_zero_arr_size(_tmp155,37));}),_tag_arr(
_tmp154,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp156;_push_handler(& _tmp156);{int _tmp158=0;if(setjmp(_tmp156.handler))
_tmp158=1;if(!_tmp158){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp159=1;_npop_handler(0);return _tmp159;};_pop_handler();}
else{void*_tmp157=(void*)_exn_thrown;void*_tmp15B=_tmp157;_LLE0: if(_tmp15B != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp15B);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp15D=({struct _tuple6
_tmp15C;_tmp15C.f1=b1;_tmp15C.f2=b2;_tmp15C;});void*_tmp15E;void*_tmp15F;void*
_tmp160;void*_tmp161;void*_tmp162;struct Cyc_Absyn_Exp*_tmp163;void*_tmp164;
struct Cyc_Absyn_Exp*_tmp165;void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;
void*_tmp16A;void*_tmp16B;void*_tmp16C;void*_tmp16D;_LLE5: _tmp15E=_tmp15D.f1;if((
int)_tmp15E != 0)goto _LLE7;_tmp15F=_tmp15D.f2;if((int)_tmp15F != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp160=_tmp15D.f1;if((int)_tmp160 != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp161=_tmp15D.f2;if((int)_tmp161 != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp162=_tmp15D.f1;if(_tmp162 <= (void*)1  || *((int*)_tmp162)!= 0)goto _LLED;
_tmp163=((struct Cyc_Absyn_Upper_b_struct*)_tmp162)->f1;_tmp164=_tmp15D.f2;if(
_tmp164 <= (void*)1  || *((int*)_tmp164)!= 0)goto _LLED;_tmp165=((struct Cyc_Absyn_Upper_b_struct*)
_tmp164)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp163,_tmp165);_LLED: _tmp166=
_tmp15D.f1;if(_tmp166 <= (void*)1  || *((int*)_tmp166)!= 0)goto _LLEF;_tmp167=
_tmp15D.f2;if(_tmp167 <= (void*)1  || *((int*)_tmp167)!= 1)goto _LLEF;_LLEE: return -
1;_LLEF: _tmp168=_tmp15D.f1;if(_tmp168 <= (void*)1  || *((int*)_tmp168)!= 1)goto
_LLF1;_tmp169=_tmp15D.f2;if(_tmp169 <= (void*)1  || *((int*)_tmp169)!= 0)goto _LLF1;
_LLF0: return 1;_LLF1: _tmp16A=_tmp15D.f1;if(_tmp16A <= (void*)1  || *((int*)_tmp16A)
!= 1)goto _LLE4;_tmp16B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16A)->f1;
_tmp16C=_tmp15D.f2;if(_tmp16C <= (void*)1  || *((int*)_tmp16C)!= 1)goto _LLE4;
_tmp16D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16C)->f1;_LLF2: return Cyc_Tcutil_typecmp(
_tmp16B,_tmp16D);_LLE4:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple6 _tmp16F=({struct _tuple6 _tmp16E;_tmp16E.f1=b1;_tmp16E.f2=b2;_tmp16E;});
void*_tmp170;void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;void*_tmp176;struct Cyc_Absyn_Exp*_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*
_tmp179;void*_tmp17A;void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;struct
Cyc_Absyn_Exp*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;_LLF4:
_tmp170=_tmp16F.f1;if((int)_tmp170 != 0)goto _LLF6;_tmp171=_tmp16F.f2;if((int)
_tmp171 != 0)goto _LLF6;_LLF5: return 0;_LLF6: _tmp172=_tmp16F.f1;if((int)_tmp172 != 0)
goto _LLF8;_LLF7: return - 1;_LLF8: _tmp173=_tmp16F.f2;if((int)_tmp173 != 0)goto _LLFA;
_LLF9: return 1;_LLFA: _tmp174=_tmp16F.f1;if(_tmp174 <= (void*)1  || *((int*)_tmp174)
!= 0)goto _LLFC;_tmp175=((struct Cyc_Absyn_Upper_b_struct*)_tmp174)->f1;_tmp176=
_tmp16F.f2;if(_tmp176 <= (void*)1  || *((int*)_tmp176)!= 0)goto _LLFC;_tmp177=((
struct Cyc_Absyn_Upper_b_struct*)_tmp176)->f1;_LLFB: return Cyc_Evexp_const_exp_cmp(
_tmp175,_tmp177);_LLFC: _tmp178=_tmp16F.f1;if(_tmp178 <= (void*)1  || *((int*)
_tmp178)!= 0)goto _LLFE;_tmp179=((struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;
_tmp17A=_tmp16F.f2;if(_tmp17A <= (void*)1  || *((int*)_tmp17A)!= 1)goto _LLFE;
_tmp17B=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17A)->f1;_LLFD: _tmp17D=
_tmp17B;_tmp17F=_tmp179;goto _LLFF;_LLFE: _tmp17C=_tmp16F.f1;if(_tmp17C <= (void*)1
 || *((int*)_tmp17C)!= 1)goto _LL100;_tmp17D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17C)->f1;_tmp17E=_tmp16F.f2;if(_tmp17E <= (void*)1  || *((int*)_tmp17E)!= 0)
goto _LL100;_tmp17F=((struct Cyc_Absyn_Upper_b_struct*)_tmp17E)->f1;_LLFF: {
unsigned int _tmp185;int _tmp186;struct _tuple7 _tmp184=Cyc_Evexp_eval_const_uint_exp(
_tmp17F);_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;if(!_tmp186)return 1;_tmp181=
_tmp17D;_tmp183=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp187=_cycalloc_atomic(
sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_TypeInt_struct _tmp188;_tmp188.tag=
18;_tmp188.f1=(int)_tmp185;_tmp188;});_tmp187;});goto _LL101;}_LL100: _tmp180=
_tmp16F.f1;if(_tmp180 <= (void*)1  || *((int*)_tmp180)!= 1)goto _LLF3;_tmp181=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp180)->f1;_tmp182=_tmp16F.f2;if(_tmp182 <= (
void*)1  || *((int*)_tmp182)!= 1)goto _LLF3;_tmp183=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp182)->f1;_LL101: Cyc_Tcutil_unify_it(_tmp181,_tmp183);return 0;_LLF3:;}static
int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp189=att;_LL103: if(_tmp189
<= (void*)17  || *((int*)_tmp189)!= 0)goto _LL105;_LL104: return 0;_LL105: if((int)
_tmp189 != 0)goto _LL107;_LL106: return 1;_LL107: if((int)_tmp189 != 1)goto _LL109;
_LL108: return 2;_LL109: if((int)_tmp189 != 2)goto _LL10B;_LL10A: return 3;_LL10B: if((
int)_tmp189 != 3)goto _LL10D;_LL10C: return 4;_LL10D: if((int)_tmp189 != 4)goto _LL10F;
_LL10E: return 5;_LL10F: if(_tmp189 <= (void*)17  || *((int*)_tmp189)!= 1)goto _LL111;
_LL110: return 6;_LL111: if((int)_tmp189 != 5)goto _LL113;_LL112: return 7;_LL113: if(
_tmp189 <= (void*)17  || *((int*)_tmp189)!= 2)goto _LL115;_LL114: return 8;_LL115: if((
int)_tmp189 != 6)goto _LL117;_LL116: return 9;_LL117: if((int)_tmp189 != 7)goto _LL119;
_LL118: return 10;_LL119: if((int)_tmp189 != 8)goto _LL11B;_LL11A: return 11;_LL11B: if((
int)_tmp189 != 9)goto _LL11D;_LL11C: return 12;_LL11D: if((int)_tmp189 != 10)goto
_LL11F;_LL11E: return 13;_LL11F: if((int)_tmp189 != 11)goto _LL121;_LL120: return 14;
_LL121: if((int)_tmp189 != 12)goto _LL123;_LL122: return 15;_LL123: if((int)_tmp189 != 
13)goto _LL125;_LL124: return 16;_LL125: if((int)_tmp189 != 14)goto _LL127;_LL126:
return 17;_LL127: if((int)_tmp189 != 15)goto _LL129;_LL128: return 18;_LL129: if(
_tmp189 <= (void*)17  || *((int*)_tmp189)!= 3)goto _LL12B;_LL12A: return 19;_LL12B:
if(_tmp189 <= (void*)17  || *((int*)_tmp189)!= 4)goto _LL12D;_LL12C: return 20;_LL12D:;
_LL12E: return 21;_LL102:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp18B=({struct _tuple6 _tmp18A;_tmp18A.f1=att1;_tmp18A.f2=att2;
_tmp18A;});void*_tmp18C;int _tmp18D;void*_tmp18E;int _tmp18F;void*_tmp190;int
_tmp191;void*_tmp192;int _tmp193;void*_tmp194;int _tmp195;void*_tmp196;int _tmp197;
void*_tmp198;struct _tagged_arr _tmp199;void*_tmp19A;struct _tagged_arr _tmp19B;void*
_tmp19C;void*_tmp19D;int _tmp19E;int _tmp19F;void*_tmp1A0;void*_tmp1A1;int _tmp1A2;
int _tmp1A3;_LL130: _tmp18C=_tmp18B.f1;if(_tmp18C <= (void*)17  || *((int*)_tmp18C)
!= 0)goto _LL132;_tmp18D=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18C)->f1;
_tmp18E=_tmp18B.f2;if(_tmp18E <= (void*)17  || *((int*)_tmp18E)!= 0)goto _LL132;
_tmp18F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18E)->f1;_LL131: _tmp191=
_tmp18D;_tmp193=_tmp18F;goto _LL133;_LL132: _tmp190=_tmp18B.f1;if(_tmp190 <= (void*)
17  || *((int*)_tmp190)!= 4)goto _LL134;_tmp191=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp190)->f1;_tmp192=_tmp18B.f2;if(_tmp192 <= (void*)17  || *((int*)_tmp192)!= 4)
goto _LL134;_tmp193=((struct Cyc_Absyn_Initializes_att_struct*)_tmp192)->f1;_LL133:
_tmp195=_tmp191;_tmp197=_tmp193;goto _LL135;_LL134: _tmp194=_tmp18B.f1;if(_tmp194
<= (void*)17  || *((int*)_tmp194)!= 1)goto _LL136;_tmp195=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp194)->f1;_tmp196=_tmp18B.f2;if(_tmp196 <= (void*)17  || *((int*)_tmp196)!= 1)
goto _LL136;_tmp197=((struct Cyc_Absyn_Aligned_att_struct*)_tmp196)->f1;_LL135:
return Cyc_Core_intcmp(_tmp195,_tmp197);_LL136: _tmp198=_tmp18B.f1;if(_tmp198 <= (
void*)17  || *((int*)_tmp198)!= 2)goto _LL138;_tmp199=((struct Cyc_Absyn_Section_att_struct*)
_tmp198)->f1;_tmp19A=_tmp18B.f2;if(_tmp19A <= (void*)17  || *((int*)_tmp19A)!= 2)
goto _LL138;_tmp19B=((struct Cyc_Absyn_Section_att_struct*)_tmp19A)->f1;_LL137:
return Cyc_strcmp((struct _tagged_arr)_tmp199,(struct _tagged_arr)_tmp19B);_LL138:
_tmp19C=_tmp18B.f1;if(_tmp19C <= (void*)17  || *((int*)_tmp19C)!= 3)goto _LL13A;
_tmp19D=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19C)->f1;_tmp19E=((
struct Cyc_Absyn_Format_att_struct*)_tmp19C)->f2;_tmp19F=((struct Cyc_Absyn_Format_att_struct*)
_tmp19C)->f3;_tmp1A0=_tmp18B.f2;if(_tmp1A0 <= (void*)17  || *((int*)_tmp1A0)!= 3)
goto _LL13A;_tmp1A1=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A0)->f1;
_tmp1A2=((struct Cyc_Absyn_Format_att_struct*)_tmp1A0)->f2;_tmp1A3=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A0)->f3;_LL139: {int _tmp1A4=Cyc_Core_intcmp((int)((unsigned int)_tmp19D),(
int)((unsigned int)_tmp1A1));if(_tmp1A4 != 0)return _tmp1A4;{int _tmp1A5=Cyc_Core_intcmp(
_tmp19E,_tmp1A2);if(_tmp1A5 != 0)return _tmp1A5;return Cyc_Core_intcmp(_tmp19F,
_tmp1A3);}}_LL13A:;_LL13B: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL12F:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1A6=Cyc_Tcutil_tvar_kind(tv);_LL13D: if((int)_tmp1A6 != 3)
goto _LL13F;_LL13E: t=(void*)2;goto _LL13C;_LL13F: if((int)_tmp1A6 != 4)goto _LL141;
_LL140: t=Cyc_Absyn_empty_effect;goto _LL13C;_LL141: if((int)_tmp1A6 != 5)goto _LL143;
_LL142: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A7=_cycalloc_atomic(
sizeof(*_tmp1A7));_tmp1A7[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1A8;_tmp1A8.tag=
18;_tmp1A8.f1=0;_tmp1A8;});_tmp1A7;});goto _LL13C;_LL143:;_LL144: t=Cyc_Absyn_sint_typ;
goto _LL13C;_LL13C:;}return({struct _tuple8*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));
_tmp1A9->f1=tv;_tmp1A9->f2=t;_tmp1A9;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1AA=Cyc_Tcutil_compress(t);void*_tmp1AB;struct Cyc_Absyn_TunionInfo
_tmp1AC;struct Cyc_List_List*_tmp1AD;void*_tmp1AE;struct Cyc_Absyn_PtrInfo _tmp1AF;
void*_tmp1B0;struct Cyc_Absyn_PtrAtts _tmp1B1;void*_tmp1B2;struct Cyc_Absyn_ArrayInfo
_tmp1B3;void*_tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_TunionFieldInfo
_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_AggrInfo _tmp1B8;struct Cyc_List_List*
_tmp1B9;struct Cyc_List_List*_tmp1BA;void*_tmp1BB;struct Cyc_Absyn_FnInfo _tmp1BC;
struct Cyc_List_List*_tmp1BD;struct Cyc_Core_Opt*_tmp1BE;void*_tmp1BF;struct Cyc_List_List*
_tmp1C0;struct Cyc_Absyn_VarargInfo*_tmp1C1;struct Cyc_List_List*_tmp1C2;void*
_tmp1C3;struct Cyc_List_List*_tmp1C4;_LL146: if((int)_tmp1AA != 0)goto _LL148;_LL147:
goto _LL149;_LL148: if((int)_tmp1AA != 1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(
_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 6)goto _LL14C;_LL14B: goto _LL14D;_LL14C:
if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 12)goto _LL14E;_LL14D: goto _LL14F;
_LL14E: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 13)goto _LL150;_LL14F: goto
_LL151;_LL150: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 18)goto _LL152;_LL151:
goto _LL153;_LL152: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 5)goto _LL154;
_LL153: return Cyc_Absyn_empty_effect;_LL154: if(_tmp1AA <= (void*)3  || *((int*)
_tmp1AA)!= 0)goto _LL156;_LL155: goto _LL157;_LL156: if(_tmp1AA <= (void*)3  || *((int*)
_tmp1AA)!= 1)goto _LL158;_LL157: {void*_tmp1C5=Cyc_Tcutil_typ_kind(t);_LL179: if((
int)_tmp1C5 != 3)goto _LL17B;_LL17A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C7;_tmp1C7.tag=19;_tmp1C7.f1=(void*)t;_tmp1C7;});_tmp1C6;});_LL17B: if((int)
_tmp1C5 != 4)goto _LL17D;_LL17C: return t;_LL17D: if((int)_tmp1C5 != 5)goto _LL17F;
_LL17E: return Cyc_Absyn_empty_effect;_LL17F:;_LL180: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1C9;_tmp1C9.tag=21;_tmp1C9.f1=(void*)t;_tmp1C9;});_tmp1C8;});_LL178:;}_LL158:
if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 15)goto _LL15A;_tmp1AB=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1AA)->f1;_LL159: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp1CB;_tmp1CB.tag=19;_tmp1CB.f1=(void*)_tmp1AB;
_tmp1CB;});_tmp1CA;});_LL15A: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 2)goto
_LL15C;_tmp1AC=((struct Cyc_Absyn_TunionType_struct*)_tmp1AA)->f1;_tmp1AD=_tmp1AC.targs;
_tmp1AE=(void*)_tmp1AC.rgn;_LL15B: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1D0;_tmp1D0.tag=19;_tmp1D0.f1=(void*)_tmp1AE;_tmp1D0;});_tmp1CF;}));_tmp1CE->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AD);_tmp1CE;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));
_tmp1CC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1CD;_tmp1CD.tag=20;_tmp1CD.f1=ts;
_tmp1CD;});_tmp1CC;}));}_LL15C: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 4)
goto _LL15E;_tmp1AF=((struct Cyc_Absyn_PointerType_struct*)_tmp1AA)->f1;_tmp1B0=(
void*)_tmp1AF.elt_typ;_tmp1B1=_tmp1AF.ptr_atts;_tmp1B2=(void*)_tmp1B1.rgn;_LL15D:
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D2;_tmp1D2.tag=20;_tmp1D2.f1=({void*_tmp1D3[2];_tmp1D3[1]=Cyc_Tcutil_rgns_of(
_tmp1B0);_tmp1D3[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D4=_cycalloc(
sizeof(*_tmp1D4));_tmp1D4[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D5;_tmp1D5.tag=
19;_tmp1D5.f1=(void*)_tmp1B2;_tmp1D5;});_tmp1D4;});Cyc_List_list(_tag_arr(
_tmp1D3,sizeof(void*),2));});_tmp1D2;});_tmp1D1;}));_LL15E: if(_tmp1AA <= (void*)3
 || *((int*)_tmp1AA)!= 7)goto _LL160;_tmp1B3=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AA)->f1;_tmp1B4=(void*)_tmp1B3.elt_type;_LL15F: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1B4));_LL160: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 
9)goto _LL162;_tmp1B5=((struct Cyc_Absyn_TupleType_struct*)_tmp1AA)->f1;_LL161: {
struct Cyc_List_List*_tmp1D6=0;for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){_tmp1D6=({
struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=(void*)(*((
struct _tuple4*)_tmp1B5->hd)).f2;_tmp1D7->tl=_tmp1D6;_tmp1D7;});}_tmp1B7=_tmp1D6;
goto _LL163;}_LL162: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 3)goto _LL164;
_tmp1B6=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1AA)->f1;_tmp1B7=_tmp1B6.targs;
_LL163: _tmp1B9=_tmp1B7;goto _LL165;_LL164: if(_tmp1AA <= (void*)3  || *((int*)
_tmp1AA)!= 10)goto _LL166;_tmp1B8=((struct Cyc_Absyn_AggrType_struct*)_tmp1AA)->f1;
_tmp1B9=_tmp1B8.targs;_LL165: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1D9;_tmp1D9.tag=20;_tmp1D9.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1B9);_tmp1D9;});_tmp1D8;}));_LL166: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)
!= 11)goto _LL168;_tmp1BA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AA)->f2;
_LL167: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DB;_tmp1DB.tag=20;_tmp1DB.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1BA);_tmp1DB;});
_tmp1DA;}));_LL168: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 14)goto _LL16A;
_tmp1BB=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1AA)->f1;_LL169: return
Cyc_Tcutil_rgns_of(_tmp1BB);_LL16A: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 
17)goto _LL16C;_LL16B: return Cyc_Tcutil_rgns_of(t);_LL16C: if(_tmp1AA <= (void*)3
 || *((int*)_tmp1AA)!= 8)goto _LL16E;_tmp1BC=((struct Cyc_Absyn_FnType_struct*)
_tmp1AA)->f1;_tmp1BD=_tmp1BC.tvars;_tmp1BE=_tmp1BC.effect;_tmp1BF=(void*)_tmp1BC.ret_typ;
_tmp1C0=_tmp1BC.args;_tmp1C1=_tmp1BC.cyc_varargs;_tmp1C2=_tmp1BC.rgn_po;_LL16D: {
void*_tmp1DC=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1BD),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BE))->v);return Cyc_Tcutil_normalize_effect(
_tmp1DC);}_LL16E: if((int)_tmp1AA != 2)goto _LL170;_LL16F: return Cyc_Absyn_empty_effect;
_LL170: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 19)goto _LL172;_LL171: goto
_LL173;_LL172: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 20)goto _LL174;_LL173:
return t;_LL174: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 21)goto _LL176;_tmp1C3=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AA)->f1;_LL175: return Cyc_Tcutil_rgns_of(
_tmp1C3);_LL176: if(_tmp1AA <= (void*)3  || *((int*)_tmp1AA)!= 16)goto _LL145;
_tmp1C4=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AA)->f2;_LL177: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));
_tmp1DD[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DE;_tmp1DE.tag=20;_tmp1DE.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C4);_tmp1DE;});_tmp1DD;}));_LL145:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1DF=e;struct Cyc_List_List*_tmp1E0;
struct Cyc_List_List**_tmp1E1;void*_tmp1E2;_LL182: if(_tmp1DF <= (void*)3  || *((int*)
_tmp1DF)!= 20)goto _LL184;_tmp1E0=((struct Cyc_Absyn_JoinEff_struct*)_tmp1DF)->f1;
_tmp1E1=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1DF)->f1;
_LL183: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1E1;for(0;effs != 0;effs=
effs->tl){void*_tmp1E3=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1E3)));{void*_tmp1E4=(void*)effs->hd;void*
_tmp1E5;_LL189: if(_tmp1E4 <= (void*)3  || *((int*)_tmp1E4)!= 20)goto _LL18B;_LL18A:
goto _LL18C;_LL18B: if(_tmp1E4 <= (void*)3  || *((int*)_tmp1E4)!= 19)goto _LL18D;
_tmp1E5=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E4)->f1;if((int)_tmp1E5
!= 2)goto _LL18D;_LL18C: redo_join=1;goto _LL188;_LL18D:;_LL18E: goto _LL188;_LL188:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1E1;for(0;effs != 0;effs=effs->tl){void*_tmp1E6=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1E7;void*_tmp1E8;_LL190: if(_tmp1E6 <= (void*)3
 || *((int*)_tmp1E6)!= 20)goto _LL192;_tmp1E7=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E6)->f1;_LL191: effects=Cyc_List_revappend(_tmp1E7,effects);goto _LL18F;
_LL192: if(_tmp1E6 <= (void*)3  || *((int*)_tmp1E6)!= 19)goto _LL194;_tmp1E8=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1E6)->f1;if((int)_tmp1E8 != 2)goto _LL194;
_LL193: goto _LL18F;_LL194:;_LL195: effects=({struct Cyc_List_List*_tmp1E9=_cycalloc(
sizeof(*_tmp1E9));_tmp1E9->hd=(void*)_tmp1E6;_tmp1E9->tl=effects;_tmp1E9;});goto
_LL18F;_LL18F:;}}*_tmp1E1=Cyc_List_imp_rev(effects);return e;}}_LL184: if(_tmp1DF
<= (void*)3  || *((int*)_tmp1DF)!= 21)goto _LL186;_tmp1E2=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1DF)->f1;_LL185: {void*_tmp1EA=Cyc_Tcutil_compress(_tmp1E2);_LL197: if(
_tmp1EA <= (void*)3  || *((int*)_tmp1EA)!= 0)goto _LL199;_LL198: goto _LL19A;_LL199:
if(_tmp1EA <= (void*)3  || *((int*)_tmp1EA)!= 1)goto _LL19B;_LL19A: return e;_LL19B:;
_LL19C: return Cyc_Tcutil_rgns_of(_tmp1E2);_LL196:;}_LL186:;_LL187: return e;_LL181:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1EB=({struct Cyc_Absyn_FnType_struct*
_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC[0]=({struct Cyc_Absyn_FnType_struct
_tmp1ED;_tmp1ED.tag=8;_tmp1ED.f1=({struct Cyc_Absyn_FnInfo _tmp1EE;_tmp1EE.tvars=0;
_tmp1EE.effect=({struct Cyc_Core_Opt*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->v=(
void*)eff;_tmp1EF;});_tmp1EE.ret_typ=(void*)((void*)0);_tmp1EE.args=0;_tmp1EE.c_varargs=
0;_tmp1EE.cyc_varargs=0;_tmp1EE.rgn_po=0;_tmp1EE.attributes=0;_tmp1EE;});_tmp1ED;});
_tmp1EC;});return Cyc_Absyn_atb_typ((void*)_tmp1EB,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{void*
_tmp1F0=Cyc_Tcutil_compress(e);void*_tmp1F1;struct Cyc_List_List*_tmp1F2;void*
_tmp1F3;struct Cyc_Core_Opt*_tmp1F4;struct Cyc_Core_Opt*_tmp1F5;struct Cyc_Core_Opt**
_tmp1F6;struct Cyc_Core_Opt*_tmp1F7;_LL19E: if(_tmp1F0 <= (void*)3  || *((int*)
_tmp1F0)!= 19)goto _LL1A0;_tmp1F1=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F0)->f1;_LL19F: if(constrain)return Cyc_Tcutil_unify(r,_tmp1F1);_tmp1F1=Cyc_Tcutil_compress(
_tmp1F1);if(r == _tmp1F1)return 1;{struct _tuple6 _tmp1F9=({struct _tuple6 _tmp1F8;
_tmp1F8.f1=r;_tmp1F8.f2=_tmp1F1;_tmp1F8;});void*_tmp1FA;struct Cyc_Absyn_Tvar*
_tmp1FB;void*_tmp1FC;struct Cyc_Absyn_Tvar*_tmp1FD;_LL1A9: _tmp1FA=_tmp1F9.f1;if(
_tmp1FA <= (void*)3  || *((int*)_tmp1FA)!= 1)goto _LL1AB;_tmp1FB=((struct Cyc_Absyn_VarType_struct*)
_tmp1FA)->f1;_tmp1FC=_tmp1F9.f2;if(_tmp1FC <= (void*)3  || *((int*)_tmp1FC)!= 1)
goto _LL1AB;_tmp1FD=((struct Cyc_Absyn_VarType_struct*)_tmp1FC)->f1;_LL1AA: return
Cyc_Absyn_tvar_cmp(_tmp1FB,_tmp1FD)== 0;_LL1AB:;_LL1AC: return 0;_LL1A8:;}_LL1A0:
if(_tmp1F0 <= (void*)3  || *((int*)_tmp1F0)!= 20)goto _LL1A2;_tmp1F2=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F0)->f1;_LL1A1: for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1F2->hd))return 1;}return 0;_LL1A2: if(_tmp1F0 <= (void*)3
 || *((int*)_tmp1F0)!= 21)goto _LL1A4;_tmp1F3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F0)->f1;_LL1A3: {void*_tmp1FE=Cyc_Tcutil_rgns_of(_tmp1F3);void*_tmp1FF;
_LL1AE: if(_tmp1FE <= (void*)3  || *((int*)_tmp1FE)!= 21)goto _LL1B0;_tmp1FF=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1FE)->f1;_LL1AF: if(!constrain)return 0;{void*
_tmp200=Cyc_Tcutil_compress(_tmp1FF);struct Cyc_Core_Opt*_tmp201;struct Cyc_Core_Opt*
_tmp202;struct Cyc_Core_Opt**_tmp203;struct Cyc_Core_Opt*_tmp204;_LL1B3: if(_tmp200
<= (void*)3  || *((int*)_tmp200)!= 0)goto _LL1B5;_tmp201=((struct Cyc_Absyn_Evar_struct*)
_tmp200)->f1;_tmp202=((struct Cyc_Absyn_Evar_struct*)_tmp200)->f2;_tmp203=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp200)->f2;_tmp204=((struct Cyc_Absyn_Evar_struct*)
_tmp200)->f4;_LL1B4: {void*_tmp205=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp204);Cyc_Tcutil_occurs(_tmp205,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp204))->v,r);{void*_tmp206=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp208=_cycalloc(sizeof(*_tmp208));
_tmp208[0]=({struct Cyc_Absyn_JoinEff_struct _tmp209;_tmp209.tag=20;_tmp209.f1=({
void*_tmp20A[2];_tmp20A[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp20B=
_cycalloc(sizeof(*_tmp20B));_tmp20B[0]=({struct Cyc_Absyn_AccessEff_struct _tmp20C;
_tmp20C.tag=19;_tmp20C.f1=(void*)r;_tmp20C;});_tmp20B;});_tmp20A[0]=_tmp205;Cyc_List_list(
_tag_arr(_tmp20A,sizeof(void*),2));});_tmp209;});_tmp208;}));*_tmp203=({struct
Cyc_Core_Opt*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->v=(void*)_tmp206;
_tmp207;});return 1;}}_LL1B5:;_LL1B6: return 0;_LL1B2:;}_LL1B0:;_LL1B1: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1FE);_LL1AD:;}_LL1A4: if(_tmp1F0 <= (void*)3  || *((int*)_tmp1F0)!= 
0)goto _LL1A6;_tmp1F4=((struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f1;_tmp1F5=((
struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f2;_tmp1F6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F0)->f2;_tmp1F7=((struct Cyc_Absyn_Evar_struct*)
_tmp1F0)->f4;_LL1A5: if(_tmp1F4 == 0  || (void*)_tmp1F4->v != (void*)4)({void*
_tmp20D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp20E="effect evar has wrong kind";_tag_arr(_tmp20E,sizeof(char),
_get_zero_arr_size(_tmp20E,27));}),_tag_arr(_tmp20D,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp20F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F7);Cyc_Tcutil_occurs(_tmp20F,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F7))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp210=({struct Cyc_Absyn_JoinEff_struct*_tmp212=_cycalloc(sizeof(*_tmp212));
_tmp212[0]=({struct Cyc_Absyn_JoinEff_struct _tmp213;_tmp213.tag=20;_tmp213.f1=({
struct Cyc_List_List*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->hd=(void*)
_tmp20F;_tmp214->tl=({struct Cyc_List_List*_tmp215=_cycalloc(sizeof(*_tmp215));
_tmp215->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp216=_cycalloc(
sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_AccessEff_struct _tmp217;_tmp217.tag=
19;_tmp217.f1=(void*)r;_tmp217;});_tmp216;}));_tmp215->tl=0;_tmp215;});_tmp214;});
_tmp213;});_tmp212;});*_tmp1F6=({struct Cyc_Core_Opt*_tmp211=_cycalloc(sizeof(*
_tmp211));_tmp211->v=(void*)((void*)_tmp210);_tmp211;});return 1;}}_LL1A6:;_LL1A7:
return 0;_LL19D:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp218=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp219;void*_tmp21A;struct Cyc_Core_Opt*_tmp21B;struct
Cyc_Core_Opt*_tmp21C;struct Cyc_Core_Opt**_tmp21D;struct Cyc_Core_Opt*_tmp21E;
_LL1B8: if(_tmp218 <= (void*)3  || *((int*)_tmp218)!= 19)goto _LL1BA;_LL1B9: return 0;
_LL1BA: if(_tmp218 <= (void*)3  || *((int*)_tmp218)!= 20)goto _LL1BC;_tmp219=((
struct Cyc_Absyn_JoinEff_struct*)_tmp218)->f1;_LL1BB: for(0;_tmp219 != 0;_tmp219=
_tmp219->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp219->hd))
return 1;}return 0;_LL1BC: if(_tmp218 <= (void*)3  || *((int*)_tmp218)!= 21)goto
_LL1BE;_tmp21A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp218)->f1;_LL1BD:
_tmp21A=Cyc_Tcutil_compress(_tmp21A);if(t == _tmp21A)return 1;if(
may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp21A);{void*_tmp21F=Cyc_Tcutil_rgns_of(
t);void*_tmp220;_LL1C3: if(_tmp21F <= (void*)3  || *((int*)_tmp21F)!= 21)goto _LL1C5;
_tmp220=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp21F)->f1;_LL1C4: {struct
_tuple6 _tmp222=({struct _tuple6 _tmp221;_tmp221.f1=Cyc_Tcutil_compress(_tmp220);
_tmp221.f2=_tmp21A;_tmp221;});void*_tmp223;struct Cyc_Absyn_Tvar*_tmp224;void*
_tmp225;struct Cyc_Absyn_Tvar*_tmp226;_LL1C8: _tmp223=_tmp222.f1;if(_tmp223 <= (
void*)3  || *((int*)_tmp223)!= 1)goto _LL1CA;_tmp224=((struct Cyc_Absyn_VarType_struct*)
_tmp223)->f1;_tmp225=_tmp222.f2;if(_tmp225 <= (void*)3  || *((int*)_tmp225)!= 1)
goto _LL1CA;_tmp226=((struct Cyc_Absyn_VarType_struct*)_tmp225)->f1;_LL1C9: return
Cyc_Tcutil_unify(t,_tmp21A);_LL1CA:;_LL1CB: return _tmp220 == _tmp21A;_LL1C7:;}
_LL1C5:;_LL1C6: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp21F);
_LL1C2:;}_LL1BE: if(_tmp218 <= (void*)3  || *((int*)_tmp218)!= 0)goto _LL1C0;_tmp21B=((
struct Cyc_Absyn_Evar_struct*)_tmp218)->f1;_tmp21C=((struct Cyc_Absyn_Evar_struct*)
_tmp218)->f2;_tmp21D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp218)->f2;_tmp21E=((struct Cyc_Absyn_Evar_struct*)_tmp218)->f4;_LL1BF: if(
_tmp21B == 0  || (void*)_tmp21B->v != (void*)4)({void*_tmp227[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp228="effect evar has wrong kind";
_tag_arr(_tmp228,sizeof(char),_get_zero_arr_size(_tmp228,27));}),_tag_arr(
_tmp227,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp229=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp21E);Cyc_Tcutil_occurs(_tmp229,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21E))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp22A=({struct Cyc_Absyn_JoinEff_struct*_tmp22C=
_cycalloc(sizeof(*_tmp22C));_tmp22C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22D;
_tmp22D.tag=20;_tmp22D.f1=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*
_tmp22E));_tmp22E->hd=(void*)_tmp229;_tmp22E->tl=({struct Cyc_List_List*_tmp22F=
_cycalloc(sizeof(*_tmp22F));_tmp22F->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp231;_tmp231.tag=21;_tmp231.f1=(void*)t;_tmp231;});_tmp230;}));_tmp22F->tl=0;
_tmp22F;});_tmp22E;});_tmp22D;});_tmp22C;});*_tmp21D=({struct Cyc_Core_Opt*
_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->v=(void*)((void*)_tmp22A);_tmp22B;});
return 1;}}_LL1C0:;_LL1C1: return 0;_LL1B7:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp232=e;struct Cyc_Absyn_Tvar*_tmp233;struct Cyc_List_List*_tmp234;void*
_tmp235;struct Cyc_Core_Opt*_tmp236;struct Cyc_Core_Opt*_tmp237;struct Cyc_Core_Opt**
_tmp238;struct Cyc_Core_Opt*_tmp239;_LL1CD: if(_tmp232 <= (void*)3  || *((int*)
_tmp232)!= 1)goto _LL1CF;_tmp233=((struct Cyc_Absyn_VarType_struct*)_tmp232)->f1;
_LL1CE: return Cyc_Absyn_tvar_cmp(v,_tmp233)== 0;_LL1CF: if(_tmp232 <= (void*)3  || *((
int*)_tmp232)!= 20)goto _LL1D1;_tmp234=((struct Cyc_Absyn_JoinEff_struct*)_tmp232)->f1;
_LL1D0: for(0;_tmp234 != 0;_tmp234=_tmp234->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp234->hd))return 1;}return 0;_LL1D1: if(_tmp232 <= (
void*)3  || *((int*)_tmp232)!= 21)goto _LL1D3;_tmp235=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp232)->f1;_LL1D2: {void*_tmp23A=Cyc_Tcutil_rgns_of(_tmp235);void*_tmp23B;
_LL1D8: if(_tmp23A <= (void*)3  || *((int*)_tmp23A)!= 21)goto _LL1DA;_tmp23B=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp23A)->f1;_LL1D9: if(!may_constrain_evars)
return 0;{void*_tmp23C=Cyc_Tcutil_compress(_tmp23B);struct Cyc_Core_Opt*_tmp23D;
struct Cyc_Core_Opt*_tmp23E;struct Cyc_Core_Opt**_tmp23F;struct Cyc_Core_Opt*
_tmp240;_LL1DD: if(_tmp23C <= (void*)3  || *((int*)_tmp23C)!= 0)goto _LL1DF;_tmp23D=((
struct Cyc_Absyn_Evar_struct*)_tmp23C)->f1;_tmp23E=((struct Cyc_Absyn_Evar_struct*)
_tmp23C)->f2;_tmp23F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23C)->f2;_tmp240=((struct Cyc_Absyn_Evar_struct*)_tmp23C)->f4;_LL1DE: {void*
_tmp241=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp240);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp240))->v,v))return 0;{void*_tmp242=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp244=_cycalloc(sizeof(*_tmp244));
_tmp244[0]=({struct Cyc_Absyn_JoinEff_struct _tmp245;_tmp245.tag=20;_tmp245.f1=({
void*_tmp246[2];_tmp246[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp247=
_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Absyn_VarType_struct _tmp248;
_tmp248.tag=1;_tmp248.f1=v;_tmp248;});_tmp247;});_tmp246[0]=_tmp241;Cyc_List_list(
_tag_arr(_tmp246,sizeof(void*),2));});_tmp245;});_tmp244;}));*_tmp23F=({struct
Cyc_Core_Opt*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->v=(void*)_tmp242;
_tmp243;});return 1;}}_LL1DF:;_LL1E0: return 0;_LL1DC:;}_LL1DA:;_LL1DB: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp23A);_LL1D7:;}_LL1D3: if(_tmp232 <= (void*)3  || *((int*)
_tmp232)!= 0)goto _LL1D5;_tmp236=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f1;
_tmp237=((struct Cyc_Absyn_Evar_struct*)_tmp232)->f2;_tmp238=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp232)->f2;_tmp239=((struct Cyc_Absyn_Evar_struct*)
_tmp232)->f4;_LL1D4: if(_tmp236 == 0  || (void*)_tmp236->v != (void*)4)({void*
_tmp249[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp24A="effect evar has wrong kind";_tag_arr(_tmp24A,sizeof(char),
_get_zero_arr_size(_tmp24A,27));}),_tag_arr(_tmp249,sizeof(void*),0));});{void*
_tmp24B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp239);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp239))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp24C=({struct Cyc_Absyn_JoinEff_struct*_tmp24E=_cycalloc(sizeof(*_tmp24E));
_tmp24E[0]=({struct Cyc_Absyn_JoinEff_struct _tmp24F;_tmp24F.tag=20;_tmp24F.f1=({
struct Cyc_List_List*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=(void*)
_tmp24B;_tmp250->tl=({struct Cyc_List_List*_tmp251=_cycalloc(sizeof(*_tmp251));
_tmp251->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp252=_cycalloc(
sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_VarType_struct _tmp253;_tmp253.tag=
1;_tmp253.f1=v;_tmp253;});_tmp252;}));_tmp251->tl=0;_tmp251;});_tmp250;});
_tmp24F;});_tmp24E;});*_tmp238=({struct Cyc_Core_Opt*_tmp24D=_cycalloc(sizeof(*
_tmp24D));_tmp24D->v=(void*)((void*)_tmp24C);_tmp24D;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp254=e;struct Cyc_List_List*_tmp255;void*_tmp256;_LL1E2: if(_tmp254 <= (
void*)3  || *((int*)_tmp254)!= 20)goto _LL1E4;_tmp255=((struct Cyc_Absyn_JoinEff_struct*)
_tmp254)->f1;_LL1E3: for(0;_tmp255 != 0;_tmp255=_tmp255->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp255->hd))return 1;}return 0;_LL1E4: if(_tmp254 <= (void*)3  || *((int*)
_tmp254)!= 21)goto _LL1E6;_tmp256=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp254)->f1;_LL1E5: {void*_tmp257=Cyc_Tcutil_rgns_of(_tmp256);void*_tmp258;
_LL1EB: if(_tmp257 <= (void*)3  || *((int*)_tmp257)!= 21)goto _LL1ED;_tmp258=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp257)->f1;_LL1EC: return 0;_LL1ED:;_LL1EE:
return Cyc_Tcutil_evar_in_effect(evar,_tmp257);_LL1EA:;}_LL1E6: if(_tmp254 <= (void*)
3  || *((int*)_tmp254)!= 0)goto _LL1E8;_LL1E7: return evar == e;_LL1E8:;_LL1E9: return
0;_LL1E1:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp259=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp25A;void*_tmp25B;
struct Cyc_Absyn_Tvar*_tmp25C;void*_tmp25D;struct Cyc_Core_Opt*_tmp25E;struct Cyc_Core_Opt**
_tmp25F;struct Cyc_Core_Opt*_tmp260;_LL1F0: if(_tmp259 <= (void*)3  || *((int*)
_tmp259)!= 20)goto _LL1F2;_tmp25A=((struct Cyc_Absyn_JoinEff_struct*)_tmp259)->f1;
_LL1F1: for(0;_tmp25A != 0;_tmp25A=_tmp25A->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp25A->hd,e2))return 0;}return 1;_LL1F2: if(_tmp259 <= (
void*)3  || *((int*)_tmp259)!= 19)goto _LL1F4;_tmp25B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp259)->f1;_LL1F3: return Cyc_Tcutil_region_in_effect(0,_tmp25B,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp25B,(void*)2);_LL1F4: if(_tmp259 <= (
void*)3  || *((int*)_tmp259)!= 1)goto _LL1F6;_tmp25C=((struct Cyc_Absyn_VarType_struct*)
_tmp259)->f1;_LL1F5: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp25C,e2);_LL1F6: if(_tmp259 <= (void*)3  || *((int*)_tmp259)!= 21)goto _LL1F8;
_tmp25D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp259)->f1;_LL1F7: {void*
_tmp261=Cyc_Tcutil_rgns_of(_tmp25D);void*_tmp262;_LL1FD: if(_tmp261 <= (void*)3
 || *((int*)_tmp261)!= 21)goto _LL1FF;_tmp262=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp261)->f1;_LL1FE: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp262,
e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp262,Cyc_Absyn_sint_typ);
_LL1FF:;_LL200: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp261,e2);
_LL1FC:;}_LL1F8: if(_tmp259 <= (void*)3  || *((int*)_tmp259)!= 0)goto _LL1FA;_tmp25E=((
struct Cyc_Absyn_Evar_struct*)_tmp259)->f2;_tmp25F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp259)->f2;_tmp260=((struct Cyc_Absyn_Evar_struct*)
_tmp259)->f4;_LL1F9: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp25F=({struct Cyc_Core_Opt*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->v=(void*)Cyc_Absyn_empty_effect;
_tmp263;});return 1;_LL1FA:;_LL1FB:({struct Cyc_String_pa_struct _tmp266;_tmp266.tag=
0;_tmp266.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp264[1]={& _tmp266};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp265="subset_effect: bad effect: %s";_tag_arr(
_tmp265,sizeof(char),_get_zero_arr_size(_tmp265,30));}),_tag_arr(_tmp264,sizeof(
void*),1));}});_LL1EF:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp268;void*_tmp269;void*_tmp26A;struct
_tuple6*_tmp267=(struct _tuple6*)r1->hd;_tmp268=*_tmp267;_tmp269=_tmp268.f1;
_tmp26A=_tmp268.f2;{int found=_tmp269 == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple6 _tmp26C;void*_tmp26D;void*_tmp26E;
struct _tuple6*_tmp26B=(struct _tuple6*)r2->hd;_tmp26C=*_tmp26B;_tmp26D=_tmp26C.f1;
_tmp26E=_tmp26C.f2;if(Cyc_Tcutil_unify(_tmp269,_tmp26D) && Cyc_Tcutil_unify(
_tmp26A,_tmp26E)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp26F=t1;struct Cyc_Core_Opt*_tmp270;struct Cyc_Core_Opt*_tmp271;
struct Cyc_Core_Opt**_tmp272;struct Cyc_Core_Opt*_tmp273;_LL202: if(_tmp26F <= (void*)
3  || *((int*)_tmp26F)!= 0)goto _LL204;_tmp270=((struct Cyc_Absyn_Evar_struct*)
_tmp26F)->f1;_tmp271=((struct Cyc_Absyn_Evar_struct*)_tmp26F)->f2;_tmp272=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp26F)->f2;_tmp273=((struct Cyc_Absyn_Evar_struct*)
_tmp26F)->f4;_LL203: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp273))->v,t2);{void*_tmp274=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp274,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp270))->v)){*_tmp272=({struct Cyc_Core_Opt*_tmp275=_cycalloc(sizeof(*_tmp275));
_tmp275->v=(void*)t2;_tmp275;});return;}else{{void*_tmp276=t2;struct Cyc_Core_Opt*
_tmp277;struct Cyc_Core_Opt**_tmp278;struct Cyc_Core_Opt*_tmp279;struct Cyc_Absyn_PtrInfo
_tmp27A;_LL207: if(_tmp276 <= (void*)3  || *((int*)_tmp276)!= 0)goto _LL209;_tmp277=((
struct Cyc_Absyn_Evar_struct*)_tmp276)->f2;_tmp278=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp276)->f2;_tmp279=((struct Cyc_Absyn_Evar_struct*)
_tmp276)->f4;_LL208: {struct Cyc_List_List*_tmp27B=(struct Cyc_List_List*)_tmp273->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp279))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp27B,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp27C="(type variable would escape scope)";_tag_arr(_tmp27C,sizeof(
char),_get_zero_arr_size(_tmp27C,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp270->v,_tmp274)){*_tmp278=({struct Cyc_Core_Opt*
_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->v=(void*)t1;_tmp27D;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp27E="(kinds are incompatible)";_tag_arr(_tmp27E,sizeof(char),
_get_zero_arr_size(_tmp27E,25));});goto _LL206;}_LL209: if(_tmp276 <= (void*)3  || *((
int*)_tmp276)!= 4)goto _LL20B;_tmp27A=((struct Cyc_Absyn_PointerType_struct*)
_tmp276)->f1;if(!((void*)_tmp270->v == (void*)2))goto _LL20B;_LL20A: {struct Cyc_Absyn_Conref*
_tmp27F=Cyc_Absyn_compress_conref((_tmp27A.ptr_atts).bounds);{void*_tmp280=(void*)
_tmp27F->v;_LL20E: if((int)_tmp280 != 0)goto _LL210;_LL20F:(void*)(_tmp27F->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp281=_cycalloc(sizeof(*_tmp281));
_tmp281[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp282;_tmp282.tag=0;_tmp282.f1=(
void*)Cyc_Absyn_bounds_one;_tmp282;});_tmp281;})));*_tmp272=({struct Cyc_Core_Opt*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->v=(void*)t2;_tmp283;});return;
_LL210:;_LL211: goto _LL20D;_LL20D:;}goto _LL206;}_LL20B:;_LL20C: goto _LL206;_LL206:;}
Cyc_Tcutil_failure_reason=({const char*_tmp284="(kinds are incompatible)";
_tag_arr(_tmp284,sizeof(char),_get_zero_arr_size(_tmp284,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL204:;_LL205: goto _LL201;_LL201:;}{struct _tuple6
_tmp286=({struct _tuple6 _tmp285;_tmp285.f1=t2;_tmp285.f2=t1;_tmp285;});void*
_tmp287;void*_tmp288;void*_tmp289;void*_tmp28A;struct Cyc_Absyn_Tvar*_tmp28B;void*
_tmp28C;struct Cyc_Absyn_Tvar*_tmp28D;void*_tmp28E;struct Cyc_Absyn_AggrInfo
_tmp28F;void*_tmp290;struct Cyc_List_List*_tmp291;void*_tmp292;struct Cyc_Absyn_AggrInfo
_tmp293;void*_tmp294;struct Cyc_List_List*_tmp295;void*_tmp296;struct _tuple1*
_tmp297;void*_tmp298;struct _tuple1*_tmp299;void*_tmp29A;struct Cyc_List_List*
_tmp29B;void*_tmp29C;struct Cyc_List_List*_tmp29D;void*_tmp29E;struct Cyc_Absyn_TunionInfo
_tmp29F;void*_tmp2A0;struct Cyc_Absyn_Tuniondecl**_tmp2A1;struct Cyc_Absyn_Tuniondecl*
_tmp2A2;struct Cyc_List_List*_tmp2A3;void*_tmp2A4;void*_tmp2A5;struct Cyc_Absyn_TunionInfo
_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Tuniondecl**_tmp2A8;struct Cyc_Absyn_Tuniondecl*
_tmp2A9;struct Cyc_List_List*_tmp2AA;void*_tmp2AB;void*_tmp2AC;struct Cyc_Absyn_TunionFieldInfo
_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Tuniondecl*_tmp2AF;struct Cyc_Absyn_Tunionfield*
_tmp2B0;struct Cyc_List_List*_tmp2B1;void*_tmp2B2;struct Cyc_Absyn_TunionFieldInfo
_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_Tuniondecl*_tmp2B5;struct Cyc_Absyn_Tunionfield*
_tmp2B6;struct Cyc_List_List*_tmp2B7;void*_tmp2B8;struct Cyc_Absyn_PtrInfo _tmp2B9;
void*_tmp2BA;struct Cyc_Absyn_Tqual _tmp2BB;struct Cyc_Absyn_PtrAtts _tmp2BC;void*
_tmp2BD;struct Cyc_Absyn_Conref*_tmp2BE;struct Cyc_Absyn_Conref*_tmp2BF;struct Cyc_Absyn_Conref*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_PtrInfo _tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Tqual
_tmp2C4;struct Cyc_Absyn_PtrAtts _tmp2C5;void*_tmp2C6;struct Cyc_Absyn_Conref*
_tmp2C7;struct Cyc_Absyn_Conref*_tmp2C8;struct Cyc_Absyn_Conref*_tmp2C9;void*
_tmp2CA;void*_tmp2CB;void*_tmp2CC;void*_tmp2CD;void*_tmp2CE;void*_tmp2CF;void*
_tmp2D0;void*_tmp2D1;void*_tmp2D2;int _tmp2D3;void*_tmp2D4;int _tmp2D5;void*
_tmp2D6;void*_tmp2D7;void*_tmp2D8;void*_tmp2D9;void*_tmp2DA;int _tmp2DB;void*
_tmp2DC;int _tmp2DD;void*_tmp2DE;void*_tmp2DF;void*_tmp2E0;void*_tmp2E1;void*
_tmp2E2;struct Cyc_Absyn_ArrayInfo _tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Tqual
_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Conref*_tmp2E7;void*_tmp2E8;
struct Cyc_Absyn_ArrayInfo _tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Tqual _tmp2EB;
struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_Absyn_Conref*_tmp2ED;void*_tmp2EE;struct
Cyc_Absyn_FnInfo _tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_Core_Opt*_tmp2F1;
void*_tmp2F2;struct Cyc_List_List*_tmp2F3;int _tmp2F4;struct Cyc_Absyn_VarargInfo*
_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_List_List*_tmp2F7;void*_tmp2F8;
struct Cyc_Absyn_FnInfo _tmp2F9;struct Cyc_List_List*_tmp2FA;struct Cyc_Core_Opt*
_tmp2FB;void*_tmp2FC;struct Cyc_List_List*_tmp2FD;int _tmp2FE;struct Cyc_Absyn_VarargInfo*
_tmp2FF;struct Cyc_List_List*_tmp300;struct Cyc_List_List*_tmp301;void*_tmp302;
struct Cyc_List_List*_tmp303;void*_tmp304;struct Cyc_List_List*_tmp305;void*
_tmp306;void*_tmp307;struct Cyc_List_List*_tmp308;void*_tmp309;void*_tmp30A;
struct Cyc_List_List*_tmp30B;void*_tmp30C;void*_tmp30D;void*_tmp30E;void*_tmp30F;
void*_tmp310;void*_tmp311;void*_tmp312;void*_tmp313;void*_tmp314;void*_tmp315;
void*_tmp316;void*_tmp317;_LL213: _tmp287=_tmp286.f1;if(_tmp287 <= (void*)3  || *((
int*)_tmp287)!= 0)goto _LL215;_LL214: Cyc_Tcutil_unify_it(t2,t1);return;_LL215:
_tmp288=_tmp286.f1;if((int)_tmp288 != 0)goto _LL217;_tmp289=_tmp286.f2;if((int)
_tmp289 != 0)goto _LL217;_LL216: return;_LL217: _tmp28A=_tmp286.f1;if(_tmp28A <= (
void*)3  || *((int*)_tmp28A)!= 1)goto _LL219;_tmp28B=((struct Cyc_Absyn_VarType_struct*)
_tmp28A)->f1;_tmp28C=_tmp286.f2;if(_tmp28C <= (void*)3  || *((int*)_tmp28C)!= 1)
goto _LL219;_tmp28D=((struct Cyc_Absyn_VarType_struct*)_tmp28C)->f1;_LL218: {
struct _tagged_arr*_tmp318=_tmp28B->name;struct _tagged_arr*_tmp319=_tmp28D->name;
int _tmp31A=*((int*)_check_null(_tmp28B->identity));int _tmp31B=*((int*)
_check_null(_tmp28D->identity));void*_tmp31C=Cyc_Tcutil_tvar_kind(_tmp28B);void*
_tmp31D=Cyc_Tcutil_tvar_kind(_tmp28D);if(_tmp31B == _tmp31A  && Cyc_strptrcmp(
_tmp318,_tmp319)== 0){if(_tmp31C != _tmp31D)({struct Cyc_String_pa_struct _tmp322;
_tmp322.tag=0;_tmp322.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp31D));{struct Cyc_String_pa_struct _tmp321;_tmp321.tag=0;_tmp321.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp31C));{struct Cyc_String_pa_struct
_tmp320;_tmp320.tag=0;_tmp320.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp318);{
void*_tmp31E[3]={& _tmp320,& _tmp321,& _tmp322};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp31F="same type variable %s has kinds %s and %s";
_tag_arr(_tmp31F,sizeof(char),_get_zero_arr_size(_tmp31F,42));}),_tag_arr(
_tmp31E,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp323="(variable types are not the same)";_tag_arr(_tmp323,sizeof(char),
_get_zero_arr_size(_tmp323,34));});goto _LL212;}_LL219: _tmp28E=_tmp286.f1;if(
_tmp28E <= (void*)3  || *((int*)_tmp28E)!= 10)goto _LL21B;_tmp28F=((struct Cyc_Absyn_AggrType_struct*)
_tmp28E)->f1;_tmp290=(void*)_tmp28F.aggr_info;_tmp291=_tmp28F.targs;_tmp292=
_tmp286.f2;if(_tmp292 <= (void*)3  || *((int*)_tmp292)!= 10)goto _LL21B;_tmp293=((
struct Cyc_Absyn_AggrType_struct*)_tmp292)->f1;_tmp294=(void*)_tmp293.aggr_info;
_tmp295=_tmp293.targs;_LL21A: {void*_tmp325;struct _tuple1*_tmp326;struct _tuple5
_tmp324=Cyc_Absyn_aggr_kinded_name(_tmp294);_tmp325=_tmp324.f1;_tmp326=_tmp324.f2;{
void*_tmp328;struct _tuple1*_tmp329;struct _tuple5 _tmp327=Cyc_Absyn_aggr_kinded_name(
_tmp290);_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;if(_tmp325 != _tmp328){Cyc_Tcutil_failure_reason=({
const char*_tmp32A="(struct and union type)";_tag_arr(_tmp32A,sizeof(char),
_get_zero_arr_size(_tmp32A,24));});goto _LL212;}if(Cyc_Absyn_qvar_cmp(_tmp326,
_tmp329)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32B="(different type name)";
_tag_arr(_tmp32B,sizeof(char),_get_zero_arr_size(_tmp32B,22));});goto _LL212;}Cyc_Tcutil_unify_list(
_tmp295,_tmp291);return;}}_LL21B: _tmp296=_tmp286.f1;if(_tmp296 <= (void*)3  || *((
int*)_tmp296)!= 12)goto _LL21D;_tmp297=((struct Cyc_Absyn_EnumType_struct*)_tmp296)->f1;
_tmp298=_tmp286.f2;if(_tmp298 <= (void*)3  || *((int*)_tmp298)!= 12)goto _LL21D;
_tmp299=((struct Cyc_Absyn_EnumType_struct*)_tmp298)->f1;_LL21C: if(Cyc_Absyn_qvar_cmp(
_tmp297,_tmp299)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp32C="(different enum types)";
_tag_arr(_tmp32C,sizeof(char),_get_zero_arr_size(_tmp32C,23));});goto _LL212;
_LL21D: _tmp29A=_tmp286.f1;if(_tmp29A <= (void*)3  || *((int*)_tmp29A)!= 13)goto
_LL21F;_tmp29B=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp29A)->f1;_tmp29C=
_tmp286.f2;if(_tmp29C <= (void*)3  || *((int*)_tmp29C)!= 13)goto _LL21F;_tmp29D=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp29C)->f1;_LL21E: {int bad=0;for(0;
_tmp29B != 0  && _tmp29D != 0;(_tmp29B=_tmp29B->tl,_tmp29D=_tmp29D->tl)){struct Cyc_Absyn_Enumfield*
_tmp32D=(struct Cyc_Absyn_Enumfield*)_tmp29B->hd;struct Cyc_Absyn_Enumfield*
_tmp32E=(struct Cyc_Absyn_Enumfield*)_tmp29D->hd;if(Cyc_Absyn_qvar_cmp(_tmp32D->name,
_tmp32E->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different names for enum fields)";
_tag_arr(_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,34));});bad=1;break;}
if(_tmp32D->tag == _tmp32E->tag)continue;if(_tmp32D->tag == 0  || _tmp32E->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp330="(different tag values for enum fields)";
_tag_arr(_tmp330,sizeof(char),_get_zero_arr_size(_tmp330,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp32D->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp32E->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp331="(different tag values for enum fields)";_tag_arr(_tmp331,
sizeof(char),_get_zero_arr_size(_tmp331,39));});bad=1;break;}}if(bad)goto _LL212;
if(_tmp29B == 0  && _tmp29D == 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp332="(different number of fields for enums)";_tag_arr(_tmp332,sizeof(char),
_get_zero_arr_size(_tmp332,39));});goto _LL212;}_LL21F: _tmp29E=_tmp286.f1;if(
_tmp29E <= (void*)3  || *((int*)_tmp29E)!= 2)goto _LL221;_tmp29F=((struct Cyc_Absyn_TunionType_struct*)
_tmp29E)->f1;_tmp2A0=(void*)_tmp29F.tunion_info;if(*((int*)_tmp2A0)!= 1)goto
_LL221;_tmp2A1=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A0)->f1;_tmp2A2=*
_tmp2A1;_tmp2A3=_tmp29F.targs;_tmp2A4=(void*)_tmp29F.rgn;_tmp2A5=_tmp286.f2;if(
_tmp2A5 <= (void*)3  || *((int*)_tmp2A5)!= 2)goto _LL221;_tmp2A6=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A5)->f1;_tmp2A7=(void*)_tmp2A6.tunion_info;if(*((int*)_tmp2A7)!= 1)goto
_LL221;_tmp2A8=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A7)->f1;_tmp2A9=*
_tmp2A8;_tmp2AA=_tmp2A6.targs;_tmp2AB=(void*)_tmp2A6.rgn;_LL220: if(_tmp2A2 == 
_tmp2A9  || Cyc_Absyn_qvar_cmp(_tmp2A2->name,_tmp2A9->name)== 0){Cyc_Tcutil_unify_it(
_tmp2AB,_tmp2A4);Cyc_Tcutil_unify_list(_tmp2AA,_tmp2A3);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp333="(different tunion types)";_tag_arr(_tmp333,sizeof(char),
_get_zero_arr_size(_tmp333,25));});goto _LL212;_LL221: _tmp2AC=_tmp286.f1;if(
_tmp2AC <= (void*)3  || *((int*)_tmp2AC)!= 3)goto _LL223;_tmp2AD=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2AC)->f1;_tmp2AE=(void*)_tmp2AD.field_info;if(*((int*)_tmp2AE)!= 1)goto
_LL223;_tmp2AF=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AE)->f1;_tmp2B0=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AE)->f2;_tmp2B1=_tmp2AD.targs;
_tmp2B2=_tmp286.f2;if(_tmp2B2 <= (void*)3  || *((int*)_tmp2B2)!= 3)goto _LL223;
_tmp2B3=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B2)->f1;_tmp2B4=(void*)
_tmp2B3.field_info;if(*((int*)_tmp2B4)!= 1)goto _LL223;_tmp2B5=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2B4)->f1;_tmp2B6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B4)->f2;
_tmp2B7=_tmp2B3.targs;_LL222: if((_tmp2AF == _tmp2B5  || Cyc_Absyn_qvar_cmp(_tmp2AF->name,
_tmp2B5->name)== 0) && (_tmp2B0 == _tmp2B6  || Cyc_Absyn_qvar_cmp(_tmp2B0->name,
_tmp2B6->name)== 0)){Cyc_Tcutil_unify_list(_tmp2B7,_tmp2B1);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp334="(different tunion field types)";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,31));});goto _LL212;_LL223: _tmp2B8=_tmp286.f1;if(
_tmp2B8 <= (void*)3  || *((int*)_tmp2B8)!= 4)goto _LL225;_tmp2B9=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B8)->f1;_tmp2BA=(void*)_tmp2B9.elt_typ;_tmp2BB=_tmp2B9.elt_tq;_tmp2BC=
_tmp2B9.ptr_atts;_tmp2BD=(void*)_tmp2BC.rgn;_tmp2BE=_tmp2BC.nullable;_tmp2BF=
_tmp2BC.bounds;_tmp2C0=_tmp2BC.zero_term;_tmp2C1=_tmp286.f2;if(_tmp2C1 <= (void*)
3  || *((int*)_tmp2C1)!= 4)goto _LL225;_tmp2C2=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C1)->f1;_tmp2C3=(void*)_tmp2C2.elt_typ;_tmp2C4=_tmp2C2.elt_tq;_tmp2C5=
_tmp2C2.ptr_atts;_tmp2C6=(void*)_tmp2C5.rgn;_tmp2C7=_tmp2C5.nullable;_tmp2C8=
_tmp2C5.bounds;_tmp2C9=_tmp2C5.zero_term;_LL224: Cyc_Tcutil_unify_it(_tmp2C3,
_tmp2BA);Cyc_Tcutil_unify_it(_tmp2BD,_tmp2C6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C9,
_tmp2C0,({const char*_tmp335="(not both zero terminated)";_tag_arr(_tmp335,
sizeof(char),_get_zero_arr_size(_tmp335,27));}));Cyc_Tcutil_unify_tqual(_tmp2C4,
_tmp2C3,_tmp2BB,_tmp2BA);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2C8,_tmp2BF,({const char*_tmp336="(different pointer bounds)";_tag_arr(
_tmp336,sizeof(char),_get_zero_arr_size(_tmp336,27));}));{void*_tmp337=(void*)(
Cyc_Absyn_compress_conref(_tmp2C8))->v;void*_tmp338;_LL24C: if(_tmp337 <= (void*)1
 || *((int*)_tmp337)!= 0)goto _LL24E;_tmp338=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp337)->f1;if((int)_tmp338 != 0)goto _LL24E;_LL24D: return;_LL24E:;_LL24F: goto
_LL24B;_LL24B:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C7,
_tmp2BE,({const char*_tmp339="(incompatible pointer types)";_tag_arr(_tmp339,
sizeof(char),_get_zero_arr_size(_tmp339,29));}));return;_LL225: _tmp2CA=_tmp286.f1;
if(_tmp2CA <= (void*)3  || *((int*)_tmp2CA)!= 5)goto _LL227;_tmp2CB=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2CA)->f1;_tmp2CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CA)->f2;_tmp2CD=_tmp286.f2;if(_tmp2CD <= (void*)3  || *((int*)_tmp2CD)!= 5)
goto _LL227;_tmp2CE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CD)->f1;_tmp2CF=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CD)->f2;_LL226: if(_tmp2CE == _tmp2CB
 && _tmp2CF == _tmp2CC)return;Cyc_Tcutil_failure_reason=({const char*_tmp33A="(different integral types)";
_tag_arr(_tmp33A,sizeof(char),_get_zero_arr_size(_tmp33A,27));});goto _LL212;
_LL227: _tmp2D0=_tmp286.f1;if((int)_tmp2D0 != 1)goto _LL229;_tmp2D1=_tmp286.f2;if((
int)_tmp2D1 != 1)goto _LL229;_LL228: return;_LL229: _tmp2D2=_tmp286.f1;if(_tmp2D2 <= (
void*)3  || *((int*)_tmp2D2)!= 6)goto _LL22B;_tmp2D3=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2D2)->f1;_tmp2D4=_tmp286.f2;if(_tmp2D4 <= (void*)3  || *((int*)_tmp2D4)!= 6)
goto _LL22B;_tmp2D5=((struct Cyc_Absyn_DoubleType_struct*)_tmp2D4)->f1;_LL22A: if(
_tmp2D3 == _tmp2D5)return;goto _LL212;_LL22B: _tmp2D6=_tmp286.f1;if(_tmp2D6 <= (void*)
3  || *((int*)_tmp2D6)!= 14)goto _LL22D;_tmp2D7=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2D6)->f1;_tmp2D8=_tmp286.f2;if(_tmp2D8 <= (void*)3  || *((int*)_tmp2D8)!= 14)
goto _LL22D;_tmp2D9=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2D8)->f1;
_LL22C: Cyc_Tcutil_unify_it(_tmp2D7,_tmp2D9);return;_LL22D: _tmp2DA=_tmp286.f1;if(
_tmp2DA <= (void*)3  || *((int*)_tmp2DA)!= 18)goto _LL22F;_tmp2DB=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2DA)->f1;_tmp2DC=_tmp286.f2;if(_tmp2DC <= (void*)3  || *((int*)_tmp2DC)!= 18)
goto _LL22F;_tmp2DD=((struct Cyc_Absyn_TypeInt_struct*)_tmp2DC)->f1;_LL22E: if(
_tmp2DB == _tmp2DD)return;Cyc_Tcutil_failure_reason=({const char*_tmp33B="(different type integers)";
_tag_arr(_tmp33B,sizeof(char),_get_zero_arr_size(_tmp33B,26));});goto _LL212;
_LL22F: _tmp2DE=_tmp286.f1;if(_tmp2DE <= (void*)3  || *((int*)_tmp2DE)!= 17)goto
_LL231;_tmp2DF=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2DE)->f1;_tmp2E0=
_tmp286.f2;if(_tmp2E0 <= (void*)3  || *((int*)_tmp2E0)!= 17)goto _LL231;_tmp2E1=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E0)->f1;_LL230: Cyc_Tcutil_unify_it(
_tmp2DF,_tmp2E1);return;_LL231: _tmp2E2=_tmp286.f1;if(_tmp2E2 <= (void*)3  || *((
int*)_tmp2E2)!= 7)goto _LL233;_tmp2E3=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E2)->f1;
_tmp2E4=(void*)_tmp2E3.elt_type;_tmp2E5=_tmp2E3.tq;_tmp2E6=_tmp2E3.num_elts;
_tmp2E7=_tmp2E3.zero_term;_tmp2E8=_tmp286.f2;if(_tmp2E8 <= (void*)3  || *((int*)
_tmp2E8)!= 7)goto _LL233;_tmp2E9=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E8)->f1;
_tmp2EA=(void*)_tmp2E9.elt_type;_tmp2EB=_tmp2E9.tq;_tmp2EC=_tmp2E9.num_elts;
_tmp2ED=_tmp2E9.zero_term;_LL232: Cyc_Tcutil_unify_it(_tmp2EA,_tmp2E4);Cyc_Tcutil_unify_tqual(
_tmp2EB,_tmp2EA,_tmp2E5,_tmp2E4);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E7,
_tmp2ED,({const char*_tmp33C="(not both zero terminated)";_tag_arr(_tmp33C,
sizeof(char),_get_zero_arr_size(_tmp33C,27));}));if(_tmp2E6 == _tmp2EC)return;if(
_tmp2E6 == 0  || _tmp2EC == 0)goto _LL212;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2E6,(struct Cyc_Absyn_Exp*)_tmp2EC))return;Cyc_Tcutil_failure_reason=({const
char*_tmp33D="(different array sizes)";_tag_arr(_tmp33D,sizeof(char),
_get_zero_arr_size(_tmp33D,24));});goto _LL212;_LL233: _tmp2EE=_tmp286.f1;if(
_tmp2EE <= (void*)3  || *((int*)_tmp2EE)!= 8)goto _LL235;_tmp2EF=((struct Cyc_Absyn_FnType_struct*)
_tmp2EE)->f1;_tmp2F0=_tmp2EF.tvars;_tmp2F1=_tmp2EF.effect;_tmp2F2=(void*)_tmp2EF.ret_typ;
_tmp2F3=_tmp2EF.args;_tmp2F4=_tmp2EF.c_varargs;_tmp2F5=_tmp2EF.cyc_varargs;
_tmp2F6=_tmp2EF.rgn_po;_tmp2F7=_tmp2EF.attributes;_tmp2F8=_tmp286.f2;if(_tmp2F8
<= (void*)3  || *((int*)_tmp2F8)!= 8)goto _LL235;_tmp2F9=((struct Cyc_Absyn_FnType_struct*)
_tmp2F8)->f1;_tmp2FA=_tmp2F9.tvars;_tmp2FB=_tmp2F9.effect;_tmp2FC=(void*)_tmp2F9.ret_typ;
_tmp2FD=_tmp2F9.args;_tmp2FE=_tmp2F9.c_varargs;_tmp2FF=_tmp2F9.cyc_varargs;
_tmp300=_tmp2F9.rgn_po;_tmp301=_tmp2F9.attributes;_LL234: {int done=0;{struct
_RegionHandle _tmp33E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33E;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2FA != 0){if(_tmp2F0 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp33F="(second function type has too few type variables)";
_tag_arr(_tmp33F,sizeof(char),_get_zero_arr_size(_tmp33F,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp340=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2FA->hd);void*_tmp341=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2F0->hd);
if(_tmp340 != _tmp341){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp346;_tmp346.tag=0;_tmp346.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp341));{struct Cyc_String_pa_struct _tmp345;_tmp345.tag=0;_tmp345.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp340));{struct Cyc_String_pa_struct
_tmp344;_tmp344.tag=0;_tmp344.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2FA->hd));{void*_tmp342[3]={& _tmp344,& _tmp345,& _tmp346};
Cyc_aprintf(({const char*_tmp343="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp343,sizeof(char),_get_zero_arr_size(_tmp343,44));}),_tag_arr(
_tmp342,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp347=_region_malloc(rgn,sizeof(*_tmp347));_tmp347->hd=({
struct _tuple8*_tmp348=_region_malloc(rgn,sizeof(*_tmp348));_tmp348->f1=(struct
Cyc_Absyn_Tvar*)_tmp2F0->hd;_tmp348->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349[0]=({struct Cyc_Absyn_VarType_struct
_tmp34A;_tmp34A.tag=1;_tmp34A.f1=(struct Cyc_Absyn_Tvar*)_tmp2FA->hd;_tmp34A;});
_tmp349;});_tmp348;});_tmp347->tl=inst;_tmp347;});_tmp2FA=_tmp2FA->tl;_tmp2F0=
_tmp2F0->tl;}}if(_tmp2F0 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp34B="(second function type has too many type variables)";
_tag_arr(_tmp34B,sizeof(char),_get_zero_arr_size(_tmp34B,51));});_npop_handler(0);
goto _LL212;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Absyn_FnType_struct
_tmp34D;_tmp34D.tag=8;_tmp34D.f1=({struct Cyc_Absyn_FnInfo _tmp34E;_tmp34E.tvars=0;
_tmp34E.effect=_tmp2FB;_tmp34E.ret_typ=(void*)_tmp2FC;_tmp34E.args=_tmp2FD;
_tmp34E.c_varargs=_tmp2FE;_tmp34E.cyc_varargs=_tmp2FF;_tmp34E.rgn_po=_tmp300;
_tmp34E.attributes=_tmp301;_tmp34E;});_tmp34D;});_tmp34C;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp34F=_cycalloc(sizeof(*
_tmp34F));_tmp34F[0]=({struct Cyc_Absyn_FnType_struct _tmp350;_tmp350.tag=8;
_tmp350.f1=({struct Cyc_Absyn_FnInfo _tmp351;_tmp351.tvars=0;_tmp351.effect=
_tmp2F1;_tmp351.ret_typ=(void*)_tmp2F2;_tmp351.args=_tmp2F3;_tmp351.c_varargs=
_tmp2F4;_tmp351.cyc_varargs=_tmp2F5;_tmp351.rgn_po=_tmp2F6;_tmp351.attributes=
_tmp2F7;_tmp351;});_tmp350;});_tmp34F;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2FC,_tmp2F2);for(0;_tmp2FD != 0  && _tmp2F3 != 0;(
_tmp2FD=_tmp2FD->tl,_tmp2F3=_tmp2F3->tl)){struct Cyc_Absyn_Tqual _tmp353;void*
_tmp354;struct _tuple2 _tmp352=*((struct _tuple2*)_tmp2FD->hd);_tmp353=_tmp352.f2;
_tmp354=_tmp352.f3;{struct Cyc_Absyn_Tqual _tmp356;void*_tmp357;struct _tuple2
_tmp355=*((struct _tuple2*)_tmp2F3->hd);_tmp356=_tmp355.f2;_tmp357=_tmp355.f3;Cyc_Tcutil_unify_it(
_tmp354,_tmp357);Cyc_Tcutil_unify_tqual(_tmp353,_tmp354,_tmp356,_tmp357);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2FD != 0  || _tmp2F3 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp358="(function types have different number of arguments)";_tag_arr(
_tmp358,sizeof(char),_get_zero_arr_size(_tmp358,52));});goto _LL212;}if(_tmp2FE != 
_tmp2F4){Cyc_Tcutil_failure_reason=({const char*_tmp359="(only one function type takes C varargs)";
_tag_arr(_tmp359,sizeof(char),_get_zero_arr_size(_tmp359,41));});goto _LL212;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp35B=({struct _tuple11 _tmp35A;_tmp35A.f1=
_tmp2FF;_tmp35A.f2=_tmp2F5;_tmp35A;});struct Cyc_Absyn_VarargInfo*_tmp35C;struct
Cyc_Absyn_VarargInfo*_tmp35D;struct Cyc_Absyn_VarargInfo*_tmp35E;struct Cyc_Absyn_VarargInfo*
_tmp35F;struct Cyc_Absyn_VarargInfo*_tmp360;struct Cyc_Absyn_VarargInfo _tmp361;
struct Cyc_Core_Opt*_tmp362;struct Cyc_Absyn_Tqual _tmp363;void*_tmp364;int _tmp365;
struct Cyc_Absyn_VarargInfo*_tmp366;struct Cyc_Absyn_VarargInfo _tmp367;struct Cyc_Core_Opt*
_tmp368;struct Cyc_Absyn_Tqual _tmp369;void*_tmp36A;int _tmp36B;_LL251: _tmp35C=
_tmp35B.f1;if(_tmp35C != 0)goto _LL253;_tmp35D=_tmp35B.f2;if(_tmp35D != 0)goto
_LL253;_LL252: goto _LL250;_LL253: _tmp35E=_tmp35B.f1;if(_tmp35E != 0)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp35F=_tmp35B.f2;if(_tmp35F != 0)goto _LL257;_LL256:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp36C="(only one function type takes varargs)";
_tag_arr(_tmp36C,sizeof(char),_get_zero_arr_size(_tmp36C,39));});goto _LL250;
_LL257: _tmp360=_tmp35B.f1;if(_tmp360 == 0)goto _LL250;_tmp361=*_tmp360;_tmp362=
_tmp361.name;_tmp363=_tmp361.tq;_tmp364=(void*)_tmp361.type;_tmp365=_tmp361.inject;
_tmp366=_tmp35B.f2;if(_tmp366 == 0)goto _LL250;_tmp367=*_tmp366;_tmp368=_tmp367.name;
_tmp369=_tmp367.tq;_tmp36A=(void*)_tmp367.type;_tmp36B=_tmp367.inject;_LL258: Cyc_Tcutil_unify_it(
_tmp364,_tmp36A);Cyc_Tcutil_unify_tqual(_tmp363,_tmp364,_tmp369,_tmp36A);if(
_tmp365 != _tmp36B){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp36D="(only one function type injects varargs)";_tag_arr(_tmp36D,sizeof(char),
_get_zero_arr_size(_tmp36D,41));});}goto _LL250;_LL250:;}if(bad_cyc_vararg)goto
_LL212;{int bad_effect=0;{struct _tuple12 _tmp36F=({struct _tuple12 _tmp36E;_tmp36E.f1=
_tmp2FB;_tmp36E.f2=_tmp2F1;_tmp36E;});struct Cyc_Core_Opt*_tmp370;struct Cyc_Core_Opt*
_tmp371;struct Cyc_Core_Opt*_tmp372;struct Cyc_Core_Opt*_tmp373;_LL25A: _tmp370=
_tmp36F.f1;if(_tmp370 != 0)goto _LL25C;_tmp371=_tmp36F.f2;if(_tmp371 != 0)goto
_LL25C;_LL25B: goto _LL259;_LL25C: _tmp372=_tmp36F.f1;if(_tmp372 != 0)goto _LL25E;
_LL25D: goto _LL25F;_LL25E: _tmp373=_tmp36F.f2;if(_tmp373 != 0)goto _LL260;_LL25F:
bad_effect=1;goto _LL259;_LL260:;_LL261: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2FB))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F1))->v);goto _LL259;_LL259:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp374="(function type effects do not unify)";
_tag_arr(_tmp374,sizeof(char),_get_zero_arr_size(_tmp374,37));});goto _LL212;}if(
!Cyc_Tcutil_same_atts(_tmp2F7,_tmp301)){Cyc_Tcutil_failure_reason=({const char*
_tmp375="(function types have different attributes)";_tag_arr(_tmp375,sizeof(
char),_get_zero_arr_size(_tmp375,43));});goto _LL212;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2F6,_tmp300)){Cyc_Tcutil_failure_reason=({const char*_tmp376="(function types have different region lifetime orderings)";
_tag_arr(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,58));});goto _LL212;}
return;}}}_LL235: _tmp302=_tmp286.f1;if(_tmp302 <= (void*)3  || *((int*)_tmp302)!= 
9)goto _LL237;_tmp303=((struct Cyc_Absyn_TupleType_struct*)_tmp302)->f1;_tmp304=
_tmp286.f2;if(_tmp304 <= (void*)3  || *((int*)_tmp304)!= 9)goto _LL237;_tmp305=((
struct Cyc_Absyn_TupleType_struct*)_tmp304)->f1;_LL236: for(0;_tmp305 != 0  && 
_tmp303 != 0;(_tmp305=_tmp305->tl,_tmp303=_tmp303->tl)){struct Cyc_Absyn_Tqual
_tmp378;void*_tmp379;struct _tuple4 _tmp377=*((struct _tuple4*)_tmp305->hd);_tmp378=
_tmp377.f1;_tmp379=_tmp377.f2;{struct Cyc_Absyn_Tqual _tmp37B;void*_tmp37C;struct
_tuple4 _tmp37A=*((struct _tuple4*)_tmp303->hd);_tmp37B=_tmp37A.f1;_tmp37C=_tmp37A.f2;
Cyc_Tcutil_unify_it(_tmp379,_tmp37C);Cyc_Tcutil_unify_tqual(_tmp378,_tmp379,
_tmp37B,_tmp37C);}}if(_tmp305 == 0  && _tmp303 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp37D="(tuple types have different numbers of components)";
_tag_arr(_tmp37D,sizeof(char),_get_zero_arr_size(_tmp37D,51));});goto _LL212;
_LL237: _tmp306=_tmp286.f1;if(_tmp306 <= (void*)3  || *((int*)_tmp306)!= 11)goto
_LL239;_tmp307=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp306)->f1;
_tmp308=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp306)->f2;_tmp309=_tmp286.f2;
if(_tmp309 <= (void*)3  || *((int*)_tmp309)!= 11)goto _LL239;_tmp30A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp309)->f1;_tmp30B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp309)->f2;_LL238: if(_tmp30A != _tmp307){Cyc_Tcutil_failure_reason=({const char*
_tmp37E="(struct and union type)";_tag_arr(_tmp37E,sizeof(char),
_get_zero_arr_size(_tmp37E,24));});goto _LL212;}for(0;_tmp30B != 0  && _tmp308 != 0;(
_tmp30B=_tmp30B->tl,_tmp308=_tmp308->tl)){struct Cyc_Absyn_Aggrfield*_tmp37F=(
struct Cyc_Absyn_Aggrfield*)_tmp30B->hd;struct Cyc_Absyn_Aggrfield*_tmp380=(struct
Cyc_Absyn_Aggrfield*)_tmp308->hd;if(Cyc_strptrcmp(_tmp37F->name,_tmp380->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp381="(different member names)";
_tag_arr(_tmp381,sizeof(char),_get_zero_arr_size(_tmp381,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp37F->type,(void*)_tmp380->type);
Cyc_Tcutil_unify_tqual(_tmp37F->tq,(void*)_tmp37F->type,_tmp380->tq,(void*)
_tmp380->type);if(!Cyc_Tcutil_same_atts(_tmp37F->attributes,_tmp380->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp382="(different attributes on member)";_tag_arr(_tmp382,sizeof(
char),_get_zero_arr_size(_tmp382,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp37F->width != 0  && _tmp380->width == 0  || _tmp380->width != 0  && _tmp37F->width
== 0) || (_tmp37F->width != 0  && _tmp380->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp37F->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp380->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp383="(different bitfield widths on member)";
_tag_arr(_tmp383,sizeof(char),_get_zero_arr_size(_tmp383,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp30B == 0  && _tmp308 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp384="(different number of members)";
_tag_arr(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,30));});goto _LL212;
_LL239: _tmp30C=_tmp286.f1;if((int)_tmp30C != 2)goto _LL23B;_tmp30D=_tmp286.f2;if((
int)_tmp30D != 2)goto _LL23B;_LL23A: return;_LL23B: _tmp30E=_tmp286.f1;if(_tmp30E <= (
void*)3  || *((int*)_tmp30E)!= 15)goto _LL23D;_tmp30F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp30E)->f1;_tmp310=_tmp286.f2;if(_tmp310 <= (void*)3  || *((int*)_tmp310)!= 15)
goto _LL23D;_tmp311=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp310)->f1;
_LL23C: Cyc_Tcutil_unify_it(_tmp30F,_tmp311);return;_LL23D: _tmp312=_tmp286.f1;if(
_tmp312 <= (void*)3  || *((int*)_tmp312)!= 20)goto _LL23F;_LL23E: goto _LL240;_LL23F:
_tmp313=_tmp286.f2;if(_tmp313 <= (void*)3  || *((int*)_tmp313)!= 20)goto _LL241;
_LL240: goto _LL242;_LL241: _tmp314=_tmp286.f1;if(_tmp314 <= (void*)3  || *((int*)
_tmp314)!= 19)goto _LL243;_LL242: goto _LL244;_LL243: _tmp315=_tmp286.f1;if(_tmp315
<= (void*)3  || *((int*)_tmp315)!= 21)goto _LL245;_LL244: goto _LL246;_LL245: _tmp316=
_tmp286.f2;if(_tmp316 <= (void*)3  || *((int*)_tmp316)!= 21)goto _LL247;_LL246: goto
_LL248;_LL247: _tmp317=_tmp286.f2;if(_tmp317 <= (void*)3  || *((int*)_tmp317)!= 19)
goto _LL249;_LL248: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp385="(effects don't unify)";_tag_arr(_tmp385,sizeof(char),
_get_zero_arr_size(_tmp385,22));});goto _LL212;_LL249:;_LL24A: goto _LL212;_LL212:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp386=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp387=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp386,_tmp387);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp388=(void*)x->v;void*
_tmp389;_LL263: if((int)_tmp388 != 0)goto _LL265;_LL264: return - 1;_LL265: if(_tmp388
<= (void*)1  || *((int*)_tmp388)!= 0)goto _LL267;_tmp389=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp388)->f1;_LL266: {void*_tmp38A=(void*)y->v;void*_tmp38B;_LL26A: if((int)
_tmp38A != 0)goto _LL26C;_LL26B: return 1;_LL26C: if(_tmp38A <= (void*)1  || *((int*)
_tmp38A)!= 0)goto _LL26E;_tmp38B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp38A)->f1;_LL26D: return cmp(_tmp389,_tmp38B);_LL26E: if(_tmp38A <= (void*)1  || *((
int*)_tmp38A)!= 1)goto _LL269;_LL26F:({void*_tmp38C[0]={};Cyc_Tcutil_impos(({
const char*_tmp38D="unify_conref: forward after compress(2)";_tag_arr(_tmp38D,
sizeof(char),_get_zero_arr_size(_tmp38D,40));}),_tag_arr(_tmp38C,sizeof(void*),0));});
_LL269:;}_LL267: if(_tmp388 <= (void*)1  || *((int*)_tmp388)!= 1)goto _LL262;_LL268:({
void*_tmp38E[0]={};Cyc_Tcutil_impos(({const char*_tmp38F="unify_conref: forward after compress";
_tag_arr(_tmp38F,sizeof(char),_get_zero_arr_size(_tmp38F,37));}),_tag_arr(
_tmp38E,sizeof(void*),0));});_LL262:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp391;struct Cyc_Absyn_Tqual
_tmp392;void*_tmp393;struct _tuple4*_tmp390=tqt1;_tmp391=*_tmp390;_tmp392=_tmp391.f1;
_tmp393=_tmp391.f2;{struct _tuple4 _tmp395;struct Cyc_Absyn_Tqual _tmp396;void*
_tmp397;struct _tuple4*_tmp394=tqt2;_tmp395=*_tmp394;_tmp396=_tmp395.f1;_tmp397=
_tmp395.f2;{int _tmp398=Cyc_Tcutil_tqual_cmp(_tmp392,_tmp396);if(_tmp398 != 0)
return _tmp398;return Cyc_Tcutil_typecmp(_tmp393,_tmp397);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp399=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp399 != 0)return _tmp399;{int _tmp39A=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp39A != 0)return _tmp39A;{int _tmp39B=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp39B != 0)return _tmp39B;{int _tmp39C=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp39C != 0)return
_tmp39C;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp39D=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp39D != 0)return _tmp39D;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp39E=t;_LL271: if((int)_tmp39E != 0)goto _LL273;_LL272: return 0;
_LL273: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 0)goto _LL275;_LL274: return 1;
_LL275: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 1)goto _LL277;_LL276: return 2;
_LL277: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 2)goto _LL279;_LL278: return 3;
_LL279: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 3)goto _LL27B;_LL27A: return 4;
_LL27B: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 4)goto _LL27D;_LL27C: return 5;
_LL27D: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 5)goto _LL27F;_LL27E: return 6;
_LL27F: if((int)_tmp39E != 1)goto _LL281;_LL280: return 7;_LL281: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 6)goto _LL283;_LL282: return 8;_LL283: if(_tmp39E <= (void*)3
 || *((int*)_tmp39E)!= 7)goto _LL285;_LL284: return 9;_LL285: if(_tmp39E <= (void*)3
 || *((int*)_tmp39E)!= 8)goto _LL287;_LL286: return 10;_LL287: if(_tmp39E <= (void*)3
 || *((int*)_tmp39E)!= 9)goto _LL289;_LL288: return 11;_LL289: if(_tmp39E <= (void*)3
 || *((int*)_tmp39E)!= 10)goto _LL28B;_LL28A: return 12;_LL28B: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 11)goto _LL28D;_LL28C: return 14;_LL28D: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 12)goto _LL28F;_LL28E: return 16;_LL28F: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 13)goto _LL291;_LL290: return 17;_LL291: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 15)goto _LL293;_LL292: return 18;_LL293: if(_tmp39E <= (void*)
3  || *((int*)_tmp39E)!= 16)goto _LL295;_LL294: return 19;_LL295: if((int)_tmp39E != 2)
goto _LL297;_LL296: return 20;_LL297: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 19)
goto _LL299;_LL298: return 21;_LL299: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 20)
goto _LL29B;_LL29A: return 22;_LL29B: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 21)
goto _LL29D;_LL29C: return 23;_LL29D: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 14)
goto _LL29F;_LL29E: return 24;_LL29F: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 18)
goto _LL2A1;_LL2A0: return 25;_LL2A1: if(_tmp39E <= (void*)3  || *((int*)_tmp39E)!= 17)
goto _LL270;_LL2A2: return 26;_LL270:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=
Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int
_tmp39F=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp39F != 0)return _tmp39F;{struct _tuple6 _tmp3A1=({struct _tuple6 _tmp3A0;
_tmp3A0.f1=t2;_tmp3A0.f2=t1;_tmp3A0;});void*_tmp3A2;void*_tmp3A3;void*_tmp3A4;
struct Cyc_Absyn_Tvar*_tmp3A5;void*_tmp3A6;struct Cyc_Absyn_Tvar*_tmp3A7;void*
_tmp3A8;struct Cyc_Absyn_AggrInfo _tmp3A9;void*_tmp3AA;struct Cyc_List_List*_tmp3AB;
void*_tmp3AC;struct Cyc_Absyn_AggrInfo _tmp3AD;void*_tmp3AE;struct Cyc_List_List*
_tmp3AF;void*_tmp3B0;struct _tuple1*_tmp3B1;void*_tmp3B2;struct _tuple1*_tmp3B3;
void*_tmp3B4;struct Cyc_List_List*_tmp3B5;void*_tmp3B6;struct Cyc_List_List*
_tmp3B7;void*_tmp3B8;struct Cyc_Absyn_TunionInfo _tmp3B9;void*_tmp3BA;struct Cyc_Absyn_Tuniondecl**
_tmp3BB;struct Cyc_Absyn_Tuniondecl*_tmp3BC;struct Cyc_List_List*_tmp3BD;void*
_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_TunionInfo _tmp3C0;void*_tmp3C1;struct Cyc_Absyn_Tuniondecl**
_tmp3C2;struct Cyc_Absyn_Tuniondecl*_tmp3C3;struct Cyc_List_List*_tmp3C4;void*
_tmp3C5;void*_tmp3C6;struct Cyc_Absyn_TunionFieldInfo _tmp3C7;void*_tmp3C8;struct
Cyc_Absyn_Tuniondecl*_tmp3C9;struct Cyc_Absyn_Tunionfield*_tmp3CA;struct Cyc_List_List*
_tmp3CB;void*_tmp3CC;struct Cyc_Absyn_TunionFieldInfo _tmp3CD;void*_tmp3CE;struct
Cyc_Absyn_Tuniondecl*_tmp3CF;struct Cyc_Absyn_Tunionfield*_tmp3D0;struct Cyc_List_List*
_tmp3D1;void*_tmp3D2;struct Cyc_Absyn_PtrInfo _tmp3D3;void*_tmp3D4;struct Cyc_Absyn_Tqual
_tmp3D5;struct Cyc_Absyn_PtrAtts _tmp3D6;void*_tmp3D7;struct Cyc_Absyn_Conref*
_tmp3D8;struct Cyc_Absyn_Conref*_tmp3D9;struct Cyc_Absyn_Conref*_tmp3DA;void*
_tmp3DB;struct Cyc_Absyn_PtrInfo _tmp3DC;void*_tmp3DD;struct Cyc_Absyn_Tqual _tmp3DE;
struct Cyc_Absyn_PtrAtts _tmp3DF;void*_tmp3E0;struct Cyc_Absyn_Conref*_tmp3E1;
struct Cyc_Absyn_Conref*_tmp3E2;struct Cyc_Absyn_Conref*_tmp3E3;void*_tmp3E4;void*
_tmp3E5;void*_tmp3E6;void*_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*
_tmp3EB;void*_tmp3EC;int _tmp3ED;void*_tmp3EE;int _tmp3EF;void*_tmp3F0;struct Cyc_Absyn_ArrayInfo
_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_Tqual _tmp3F3;struct Cyc_Absyn_Exp*_tmp3F4;
struct Cyc_Absyn_Conref*_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_ArrayInfo _tmp3F7;
void*_tmp3F8;struct Cyc_Absyn_Tqual _tmp3F9;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_Absyn_Conref*
_tmp3FB;void*_tmp3FC;struct Cyc_Absyn_FnInfo _tmp3FD;struct Cyc_List_List*_tmp3FE;
struct Cyc_Core_Opt*_tmp3FF;void*_tmp400;struct Cyc_List_List*_tmp401;int _tmp402;
struct Cyc_Absyn_VarargInfo*_tmp403;struct Cyc_List_List*_tmp404;struct Cyc_List_List*
_tmp405;void*_tmp406;struct Cyc_Absyn_FnInfo _tmp407;struct Cyc_List_List*_tmp408;
struct Cyc_Core_Opt*_tmp409;void*_tmp40A;struct Cyc_List_List*_tmp40B;int _tmp40C;
struct Cyc_Absyn_VarargInfo*_tmp40D;struct Cyc_List_List*_tmp40E;struct Cyc_List_List*
_tmp40F;void*_tmp410;struct Cyc_List_List*_tmp411;void*_tmp412;struct Cyc_List_List*
_tmp413;void*_tmp414;void*_tmp415;struct Cyc_List_List*_tmp416;void*_tmp417;void*
_tmp418;struct Cyc_List_List*_tmp419;void*_tmp41A;void*_tmp41B;void*_tmp41C;void*
_tmp41D;void*_tmp41E;void*_tmp41F;void*_tmp420;void*_tmp421;void*_tmp422;void*
_tmp423;void*_tmp424;void*_tmp425;void*_tmp426;int _tmp427;void*_tmp428;int
_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*
_tmp42F;_LL2A4: _tmp3A2=_tmp3A1.f1;if(_tmp3A2 <= (void*)3  || *((int*)_tmp3A2)!= 0)
goto _LL2A6;_tmp3A3=_tmp3A1.f2;if(_tmp3A3 <= (void*)3  || *((int*)_tmp3A3)!= 0)goto
_LL2A6;_LL2A5:({void*_tmp430[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp431="typecmp: can only compare closed types";
_tag_arr(_tmp431,sizeof(char),_get_zero_arr_size(_tmp431,39));}),_tag_arr(
_tmp430,sizeof(void*),0));});_LL2A6: _tmp3A4=_tmp3A1.f1;if(_tmp3A4 <= (void*)3  || *((
int*)_tmp3A4)!= 1)goto _LL2A8;_tmp3A5=((struct Cyc_Absyn_VarType_struct*)_tmp3A4)->f1;
_tmp3A6=_tmp3A1.f2;if(_tmp3A6 <= (void*)3  || *((int*)_tmp3A6)!= 1)goto _LL2A8;
_tmp3A7=((struct Cyc_Absyn_VarType_struct*)_tmp3A6)->f1;_LL2A7: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3A7->identity)),*((int*)_check_null(_tmp3A5->identity)));
_LL2A8: _tmp3A8=_tmp3A1.f1;if(_tmp3A8 <= (void*)3  || *((int*)_tmp3A8)!= 10)goto
_LL2AA;_tmp3A9=((struct Cyc_Absyn_AggrType_struct*)_tmp3A8)->f1;_tmp3AA=(void*)
_tmp3A9.aggr_info;_tmp3AB=_tmp3A9.targs;_tmp3AC=_tmp3A1.f2;if(_tmp3AC <= (void*)3
 || *((int*)_tmp3AC)!= 10)goto _LL2AA;_tmp3AD=((struct Cyc_Absyn_AggrType_struct*)
_tmp3AC)->f1;_tmp3AE=(void*)_tmp3AD.aggr_info;_tmp3AF=_tmp3AD.targs;_LL2A9: {
struct _tuple1*_tmp433;struct _tuple5 _tmp432=Cyc_Absyn_aggr_kinded_name(_tmp3AA);
_tmp433=_tmp432.f2;{struct _tuple1*_tmp435;struct _tuple5 _tmp434=Cyc_Absyn_aggr_kinded_name(
_tmp3AE);_tmp435=_tmp434.f2;{int _tmp436=Cyc_Absyn_qvar_cmp(_tmp433,_tmp435);if(
_tmp436 != 0)return _tmp436;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3AB,_tmp3AF);}}}}_LL2AA: _tmp3B0=_tmp3A1.f1;if(_tmp3B0 <= (void*)3  || *((int*)
_tmp3B0)!= 12)goto _LL2AC;_tmp3B1=((struct Cyc_Absyn_EnumType_struct*)_tmp3B0)->f1;
_tmp3B2=_tmp3A1.f2;if(_tmp3B2 <= (void*)3  || *((int*)_tmp3B2)!= 12)goto _LL2AC;
_tmp3B3=((struct Cyc_Absyn_EnumType_struct*)_tmp3B2)->f1;_LL2AB: return Cyc_Absyn_qvar_cmp(
_tmp3B1,_tmp3B3);_LL2AC: _tmp3B4=_tmp3A1.f1;if(_tmp3B4 <= (void*)3  || *((int*)
_tmp3B4)!= 13)goto _LL2AE;_tmp3B5=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B4)->f1;
_tmp3B6=_tmp3A1.f2;if(_tmp3B6 <= (void*)3  || *((int*)_tmp3B6)!= 13)goto _LL2AE;
_tmp3B7=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B6)->f1;_LL2AD: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B5,
_tmp3B7);_LL2AE: _tmp3B8=_tmp3A1.f1;if(_tmp3B8 <= (void*)3  || *((int*)_tmp3B8)!= 2)
goto _LL2B0;_tmp3B9=((struct Cyc_Absyn_TunionType_struct*)_tmp3B8)->f1;_tmp3BA=(
void*)_tmp3B9.tunion_info;if(*((int*)_tmp3BA)!= 1)goto _LL2B0;_tmp3BB=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3BA)->f1;_tmp3BC=*_tmp3BB;_tmp3BD=_tmp3B9.targs;_tmp3BE=(void*)_tmp3B9.rgn;
_tmp3BF=_tmp3A1.f2;if(_tmp3BF <= (void*)3  || *((int*)_tmp3BF)!= 2)goto _LL2B0;
_tmp3C0=((struct Cyc_Absyn_TunionType_struct*)_tmp3BF)->f1;_tmp3C1=(void*)_tmp3C0.tunion_info;
if(*((int*)_tmp3C1)!= 1)goto _LL2B0;_tmp3C2=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3C1)->f1;_tmp3C3=*_tmp3C2;_tmp3C4=_tmp3C0.targs;_tmp3C5=(void*)_tmp3C0.rgn;
_LL2AF: if(_tmp3C3 == _tmp3BC)return 0;{int _tmp437=Cyc_Absyn_qvar_cmp(_tmp3C3->name,
_tmp3BC->name);if(_tmp437 != 0)return _tmp437;{int _tmp438=Cyc_Tcutil_typecmp(
_tmp3C5,_tmp3BE);if(_tmp438 != 0)return _tmp438;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C4,_tmp3BD);}}_LL2B0: _tmp3C6=_tmp3A1.f1;if(_tmp3C6 <= (void*)3  || *((int*)
_tmp3C6)!= 3)goto _LL2B2;_tmp3C7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3C6)->f1;_tmp3C8=(void*)_tmp3C7.field_info;if(*((int*)_tmp3C8)!= 1)goto
_LL2B2;_tmp3C9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3C8)->f1;_tmp3CA=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3C8)->f2;_tmp3CB=_tmp3C7.targs;
_tmp3CC=_tmp3A1.f2;if(_tmp3CC <= (void*)3  || *((int*)_tmp3CC)!= 3)goto _LL2B2;
_tmp3CD=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3CC)->f1;_tmp3CE=(void*)
_tmp3CD.field_info;if(*((int*)_tmp3CE)!= 1)goto _LL2B2;_tmp3CF=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3CE)->f1;_tmp3D0=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CE)->f2;
_tmp3D1=_tmp3CD.targs;_LL2B1: if(_tmp3CF == _tmp3C9)return 0;{int _tmp439=Cyc_Absyn_qvar_cmp(
_tmp3C9->name,_tmp3CF->name);if(_tmp439 != 0)return _tmp439;{int _tmp43A=Cyc_Absyn_qvar_cmp(
_tmp3CA->name,_tmp3D0->name);if(_tmp43A != 0)return _tmp43A;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D1,_tmp3CB);}}_LL2B2: _tmp3D2=_tmp3A1.f1;if(_tmp3D2 <= (
void*)3  || *((int*)_tmp3D2)!= 4)goto _LL2B4;_tmp3D3=((struct Cyc_Absyn_PointerType_struct*)
_tmp3D2)->f1;_tmp3D4=(void*)_tmp3D3.elt_typ;_tmp3D5=_tmp3D3.elt_tq;_tmp3D6=
_tmp3D3.ptr_atts;_tmp3D7=(void*)_tmp3D6.rgn;_tmp3D8=_tmp3D6.nullable;_tmp3D9=
_tmp3D6.bounds;_tmp3DA=_tmp3D6.zero_term;_tmp3DB=_tmp3A1.f2;if(_tmp3DB <= (void*)
3  || *((int*)_tmp3DB)!= 4)goto _LL2B4;_tmp3DC=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DB)->f1;_tmp3DD=(void*)_tmp3DC.elt_typ;_tmp3DE=_tmp3DC.elt_tq;_tmp3DF=
_tmp3DC.ptr_atts;_tmp3E0=(void*)_tmp3DF.rgn;_tmp3E1=_tmp3DF.nullable;_tmp3E2=
_tmp3DF.bounds;_tmp3E3=_tmp3DF.zero_term;_LL2B3: {int _tmp43B=Cyc_Tcutil_typecmp(
_tmp3DD,_tmp3D4);if(_tmp43B != 0)return _tmp43B;{int _tmp43C=Cyc_Tcutil_typecmp(
_tmp3E0,_tmp3D7);if(_tmp43C != 0)return _tmp43C;{int _tmp43D=Cyc_Tcutil_tqual_cmp(
_tmp3DE,_tmp3D5);if(_tmp43D != 0)return _tmp43D;{int _tmp43E=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3E2,_tmp3D9);if(_tmp43E != 0)return _tmp43E;{int _tmp43F=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3E3,_tmp3DA);if(_tmp43F != 0)return _tmp43F;{void*_tmp440=(
void*)(Cyc_Absyn_compress_conref(_tmp3E2))->v;void*_tmp441;_LL2D9: if(_tmp440 <= (
void*)1  || *((int*)_tmp440)!= 0)goto _LL2DB;_tmp441=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp440)->f1;if((int)_tmp441 != 0)goto _LL2DB;_LL2DA: return 0;_LL2DB:;_LL2DC: goto
_LL2D8;_LL2D8:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E1,
_tmp3D8);}}}}}_LL2B4: _tmp3E4=_tmp3A1.f1;if(_tmp3E4 <= (void*)3  || *((int*)_tmp3E4)
!= 5)goto _LL2B6;_tmp3E5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E4)->f1;
_tmp3E6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E4)->f2;_tmp3E7=_tmp3A1.f2;
if(_tmp3E7 <= (void*)3  || *((int*)_tmp3E7)!= 5)goto _LL2B6;_tmp3E8=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3E7)->f1;_tmp3E9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E7)->f2;_LL2B5: if(_tmp3E8 != _tmp3E5)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3E8),(int)((unsigned int)_tmp3E5));if(_tmp3E9 != _tmp3E6)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3E9),(int)((unsigned int)_tmp3E6));
return 0;_LL2B6: _tmp3EA=_tmp3A1.f1;if((int)_tmp3EA != 1)goto _LL2B8;_tmp3EB=_tmp3A1.f2;
if((int)_tmp3EB != 1)goto _LL2B8;_LL2B7: return 0;_LL2B8: _tmp3EC=_tmp3A1.f1;if(
_tmp3EC <= (void*)3  || *((int*)_tmp3EC)!= 6)goto _LL2BA;_tmp3ED=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3EC)->f1;_tmp3EE=_tmp3A1.f2;if(_tmp3EE <= (void*)3  || *((int*)_tmp3EE)!= 6)
goto _LL2BA;_tmp3EF=((struct Cyc_Absyn_DoubleType_struct*)_tmp3EE)->f1;_LL2B9: if(
_tmp3ED == _tmp3EF)return 0;else{if(_tmp3ED)return - 1;else{return 1;}}_LL2BA: _tmp3F0=
_tmp3A1.f1;if(_tmp3F0 <= (void*)3  || *((int*)_tmp3F0)!= 7)goto _LL2BC;_tmp3F1=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F0)->f1;_tmp3F2=(void*)_tmp3F1.elt_type;
_tmp3F3=_tmp3F1.tq;_tmp3F4=_tmp3F1.num_elts;_tmp3F5=_tmp3F1.zero_term;_tmp3F6=
_tmp3A1.f2;if(_tmp3F6 <= (void*)3  || *((int*)_tmp3F6)!= 7)goto _LL2BC;_tmp3F7=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F6)->f1;_tmp3F8=(void*)_tmp3F7.elt_type;
_tmp3F9=_tmp3F7.tq;_tmp3FA=_tmp3F7.num_elts;_tmp3FB=_tmp3F7.zero_term;_LL2BB: {
int _tmp442=Cyc_Tcutil_tqual_cmp(_tmp3F9,_tmp3F3);if(_tmp442 != 0)return _tmp442;{
int _tmp443=Cyc_Tcutil_typecmp(_tmp3F8,_tmp3F2);if(_tmp443 != 0)return _tmp443;{int
_tmp444=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3F5,_tmp3FB);if(_tmp444 != 0)
return _tmp444;if(_tmp3F4 == _tmp3FA)return 0;if(_tmp3F4 == 0  || _tmp3FA == 0)({void*
_tmp445[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp446="missing expression in array index";_tag_arr(_tmp446,sizeof(
char),_get_zero_arr_size(_tmp446,34));}),_tag_arr(_tmp445,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3F4,
_tmp3FA);}}}_LL2BC: _tmp3FC=_tmp3A1.f1;if(_tmp3FC <= (void*)3  || *((int*)_tmp3FC)
!= 8)goto _LL2BE;_tmp3FD=((struct Cyc_Absyn_FnType_struct*)_tmp3FC)->f1;_tmp3FE=
_tmp3FD.tvars;_tmp3FF=_tmp3FD.effect;_tmp400=(void*)_tmp3FD.ret_typ;_tmp401=
_tmp3FD.args;_tmp402=_tmp3FD.c_varargs;_tmp403=_tmp3FD.cyc_varargs;_tmp404=
_tmp3FD.rgn_po;_tmp405=_tmp3FD.attributes;_tmp406=_tmp3A1.f2;if(_tmp406 <= (void*)
3  || *((int*)_tmp406)!= 8)goto _LL2BE;_tmp407=((struct Cyc_Absyn_FnType_struct*)
_tmp406)->f1;_tmp408=_tmp407.tvars;_tmp409=_tmp407.effect;_tmp40A=(void*)_tmp407.ret_typ;
_tmp40B=_tmp407.args;_tmp40C=_tmp407.c_varargs;_tmp40D=_tmp407.cyc_varargs;
_tmp40E=_tmp407.rgn_po;_tmp40F=_tmp407.attributes;_LL2BD:({void*_tmp447[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp448="typecmp: function types not handled";_tag_arr(_tmp448,sizeof(char),
_get_zero_arr_size(_tmp448,36));}),_tag_arr(_tmp447,sizeof(void*),0));});_LL2BE:
_tmp410=_tmp3A1.f1;if(_tmp410 <= (void*)3  || *((int*)_tmp410)!= 9)goto _LL2C0;
_tmp411=((struct Cyc_Absyn_TupleType_struct*)_tmp410)->f1;_tmp412=_tmp3A1.f2;if(
_tmp412 <= (void*)3  || *((int*)_tmp412)!= 9)goto _LL2C0;_tmp413=((struct Cyc_Absyn_TupleType_struct*)
_tmp412)->f1;_LL2BF: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp413,_tmp411);_LL2C0: _tmp414=_tmp3A1.f1;if(_tmp414 <= (void*)3  || *((int*)
_tmp414)!= 11)goto _LL2C2;_tmp415=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp414)->f1;_tmp416=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp414)->f2;_tmp417=
_tmp3A1.f2;if(_tmp417 <= (void*)3  || *((int*)_tmp417)!= 11)goto _LL2C2;_tmp418=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp417)->f1;_tmp419=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp417)->f2;_LL2C1: if(_tmp418 != _tmp415){if(_tmp418 == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp419,_tmp416);_LL2C2: _tmp41A=_tmp3A1.f1;if(_tmp41A <= (void*)3  || *((int*)
_tmp41A)!= 15)goto _LL2C4;_tmp41B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41A)->f1;_tmp41C=_tmp3A1.f2;if(_tmp41C <= (void*)3  || *((int*)_tmp41C)!= 15)
goto _LL2C4;_tmp41D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp41C)->f1;
_LL2C3: return Cyc_Tcutil_typecmp(_tmp41B,_tmp41D);_LL2C4: _tmp41E=_tmp3A1.f1;if(
_tmp41E <= (void*)3  || *((int*)_tmp41E)!= 14)goto _LL2C6;_tmp41F=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp41E)->f1;_tmp420=_tmp3A1.f2;if(_tmp420 <= (void*)
3  || *((int*)_tmp420)!= 14)goto _LL2C6;_tmp421=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp420)->f1;_LL2C5: return Cyc_Tcutil_typecmp(_tmp41F,_tmp421);_LL2C6: _tmp422=
_tmp3A1.f1;if(_tmp422 <= (void*)3  || *((int*)_tmp422)!= 17)goto _LL2C8;_tmp423=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp422)->f1;_tmp424=_tmp3A1.f2;if(
_tmp424 <= (void*)3  || *((int*)_tmp424)!= 17)goto _LL2C8;_tmp425=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp424)->f1;_LL2C7: return Cyc_Tcutil_typecmp(_tmp423,
_tmp425);_LL2C8: _tmp426=_tmp3A1.f1;if(_tmp426 <= (void*)3  || *((int*)_tmp426)!= 
18)goto _LL2CA;_tmp427=((struct Cyc_Absyn_TypeInt_struct*)_tmp426)->f1;_tmp428=
_tmp3A1.f2;if(_tmp428 <= (void*)3  || *((int*)_tmp428)!= 18)goto _LL2CA;_tmp429=((
struct Cyc_Absyn_TypeInt_struct*)_tmp428)->f1;_LL2C9: return Cyc_Core_intcmp(
_tmp427,_tmp429);_LL2CA: _tmp42A=_tmp3A1.f1;if(_tmp42A <= (void*)3  || *((int*)
_tmp42A)!= 20)goto _LL2CC;_LL2CB: goto _LL2CD;_LL2CC: _tmp42B=_tmp3A1.f2;if(_tmp42B
<= (void*)3  || *((int*)_tmp42B)!= 20)goto _LL2CE;_LL2CD: goto _LL2CF;_LL2CE: _tmp42C=
_tmp3A1.f1;if(_tmp42C <= (void*)3  || *((int*)_tmp42C)!= 19)goto _LL2D0;_LL2CF: goto
_LL2D1;_LL2D0: _tmp42D=_tmp3A1.f1;if(_tmp42D <= (void*)3  || *((int*)_tmp42D)!= 21)
goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp42E=_tmp3A1.f2;if(_tmp42E <= (void*)3  || *((
int*)_tmp42E)!= 21)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4: _tmp42F=_tmp3A1.f2;if(
_tmp42F <= (void*)3  || *((int*)_tmp42F)!= 19)goto _LL2D6;_LL2D5:({void*_tmp449[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44A="typecmp: effects not handled";_tag_arr(_tmp44A,sizeof(char),
_get_zero_arr_size(_tmp44A,29));}),_tag_arr(_tmp449,sizeof(void*),0));});_LL2D6:;
_LL2D7:({void*_tmp44B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp44C="Unmatched case in typecmp";_tag_arr(
_tmp44C,sizeof(char),_get_zero_arr_size(_tmp44C,26));}),_tag_arr(_tmp44B,sizeof(
void*),0));});_LL2A3:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp44D=
Cyc_Tcutil_compress(t);_LL2DE: if(_tmp44D <= (void*)3  || *((int*)_tmp44D)!= 5)goto
_LL2E0;_LL2DF: goto _LL2E1;_LL2E0: if((int)_tmp44D != 1)goto _LL2E2;_LL2E1: goto _LL2E3;
_LL2E2: if(_tmp44D <= (void*)3  || *((int*)_tmp44D)!= 6)goto _LL2E4;_LL2E3: goto
_LL2E5;_LL2E4: if(_tmp44D <= (void*)3  || *((int*)_tmp44D)!= 12)goto _LL2E6;_LL2E5:
goto _LL2E7;_LL2E6: if(_tmp44D <= (void*)3  || *((int*)_tmp44D)!= 13)goto _LL2E8;
_LL2E7: return 1;_LL2E8:;_LL2E9: return 0;_LL2DD:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp44F=({struct _tuple6 _tmp44E;_tmp44E.f1=Cyc_Tcutil_compress(
t1);_tmp44E.f2=Cyc_Tcutil_compress(t2);_tmp44E;});void*_tmp450;int _tmp451;void*
_tmp452;int _tmp453;void*_tmp454;void*_tmp455;void*_tmp456;void*_tmp457;void*
_tmp458;void*_tmp459;void*_tmp45A;void*_tmp45B;void*_tmp45C;void*_tmp45D;void*
_tmp45E;void*_tmp45F;void*_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;void*
_tmp464;void*_tmp465;void*_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*
_tmp46A;void*_tmp46B;void*_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*
_tmp470;void*_tmp471;void*_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*
_tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*
_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*
_tmp482;_LL2EB: _tmp450=_tmp44F.f1;if(_tmp450 <= (void*)3  || *((int*)_tmp450)!= 6)
goto _LL2ED;_tmp451=((struct Cyc_Absyn_DoubleType_struct*)_tmp450)->f1;_tmp452=
_tmp44F.f2;if(_tmp452 <= (void*)3  || *((int*)_tmp452)!= 6)goto _LL2ED;_tmp453=((
struct Cyc_Absyn_DoubleType_struct*)_tmp452)->f1;_LL2EC: return !_tmp453  && _tmp451;
_LL2ED: _tmp454=_tmp44F.f1;if(_tmp454 <= (void*)3  || *((int*)_tmp454)!= 6)goto
_LL2EF;_tmp455=_tmp44F.f2;if((int)_tmp455 != 1)goto _LL2EF;_LL2EE: goto _LL2F0;
_LL2EF: _tmp456=_tmp44F.f1;if(_tmp456 <= (void*)3  || *((int*)_tmp456)!= 6)goto
_LL2F1;_tmp457=_tmp44F.f2;if(_tmp457 <= (void*)3  || *((int*)_tmp457)!= 5)goto
_LL2F1;_LL2F0: goto _LL2F2;_LL2F1: _tmp458=_tmp44F.f1;if(_tmp458 <= (void*)3  || *((
int*)_tmp458)!= 6)goto _LL2F3;_tmp459=_tmp44F.f2;if(_tmp459 <= (void*)3  || *((int*)
_tmp459)!= 14)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: _tmp45A=_tmp44F.f1;if((int)
_tmp45A != 1)goto _LL2F5;_tmp45B=_tmp44F.f2;if(_tmp45B <= (void*)3  || *((int*)
_tmp45B)!= 14)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: _tmp45C=_tmp44F.f1;if(_tmp45C
<= (void*)3  || *((int*)_tmp45C)!= 6)goto _LL2F7;_tmp45D=_tmp44F.f2;if(_tmp45D <= (
void*)3  || *((int*)_tmp45D)!= 17)goto _LL2F7;_LL2F6: goto _LL2F8;_LL2F7: _tmp45E=
_tmp44F.f1;if((int)_tmp45E != 1)goto _LL2F9;_tmp45F=_tmp44F.f2;if(_tmp45F <= (void*)
3  || *((int*)_tmp45F)!= 17)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp460=_tmp44F.f1;
if((int)_tmp460 != 1)goto _LL2FB;_tmp461=_tmp44F.f2;if(_tmp461 <= (void*)3  || *((
int*)_tmp461)!= 5)goto _LL2FB;_LL2FA: return 1;_LL2FB: _tmp462=_tmp44F.f1;if(_tmp462
<= (void*)3  || *((int*)_tmp462)!= 5)goto _LL2FD;_tmp463=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp462)->f2;if((int)_tmp463 != 3)goto _LL2FD;_tmp464=_tmp44F.f2;if(_tmp464 <= (
void*)3  || *((int*)_tmp464)!= 5)goto _LL2FD;_tmp465=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp464)->f2;if((int)_tmp465 != 3)goto _LL2FD;_LL2FC: return 0;_LL2FD: _tmp466=
_tmp44F.f1;if(_tmp466 <= (void*)3  || *((int*)_tmp466)!= 5)goto _LL2FF;_tmp467=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp466)->f2;if((int)_tmp467 != 3)goto
_LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp468=_tmp44F.f1;if(_tmp468 <= (void*)3  || *((
int*)_tmp468)!= 5)goto _LL301;_tmp469=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp468)->f2;if((int)_tmp469 != 2)goto _LL301;_tmp46A=_tmp44F.f2;if((int)_tmp46A != 
1)goto _LL301;_LL300: goto _LL302;_LL301: _tmp46B=_tmp44F.f1;if(_tmp46B <= (void*)3
 || *((int*)_tmp46B)!= 5)goto _LL303;_tmp46C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46B)->f2;if((int)_tmp46C != 2)goto _LL303;_tmp46D=_tmp44F.f2;if(_tmp46D <= (
void*)3  || *((int*)_tmp46D)!= 5)goto _LL303;_tmp46E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46D)->f2;if((int)_tmp46E != 1)goto _LL303;_LL302: goto _LL304;_LL303: _tmp46F=
_tmp44F.f1;if(_tmp46F <= (void*)3  || *((int*)_tmp46F)!= 5)goto _LL305;_tmp470=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp46F)->f2;if((int)_tmp470 != 2)goto
_LL305;_tmp471=_tmp44F.f2;if(_tmp471 <= (void*)3  || *((int*)_tmp471)!= 5)goto
_LL305;_tmp472=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp471)->f2;if((int)
_tmp472 != 0)goto _LL305;_LL304: goto _LL306;_LL305: _tmp473=_tmp44F.f1;if(_tmp473 <= (
void*)3  || *((int*)_tmp473)!= 5)goto _LL307;_tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp473)->f2;if((int)_tmp474 != 1)goto _LL307;_tmp475=_tmp44F.f2;if(_tmp475 <= (
void*)3  || *((int*)_tmp475)!= 5)goto _LL307;_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp475)->f2;if((int)_tmp476 != 0)goto _LL307;_LL306: goto _LL308;_LL307: _tmp477=
_tmp44F.f1;if(_tmp477 <= (void*)3  || *((int*)_tmp477)!= 17)goto _LL309;_tmp478=
_tmp44F.f2;if(_tmp478 <= (void*)3  || *((int*)_tmp478)!= 5)goto _LL309;_tmp479=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp478)->f2;if((int)_tmp479 != 1)goto
_LL309;_LL308: goto _LL30A;_LL309: _tmp47A=_tmp44F.f1;if(_tmp47A <= (void*)3  || *((
int*)_tmp47A)!= 17)goto _LL30B;_tmp47B=_tmp44F.f2;if(_tmp47B <= (void*)3  || *((int*)
_tmp47B)!= 5)goto _LL30B;_tmp47C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47B)->f2;
if((int)_tmp47C != 0)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp47D=_tmp44F.f1;if(
_tmp47D <= (void*)3  || *((int*)_tmp47D)!= 14)goto _LL30D;_tmp47E=_tmp44F.f2;if(
_tmp47E <= (void*)3  || *((int*)_tmp47E)!= 5)goto _LL30D;_tmp47F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47E)->f2;if((int)_tmp47F != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp480=
_tmp44F.f1;if(_tmp480 <= (void*)3  || *((int*)_tmp480)!= 14)goto _LL30F;_tmp481=
_tmp44F.f2;if(_tmp481 <= (void*)3  || *((int*)_tmp481)!= 5)goto _LL30F;_tmp482=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp481)->f2;if((int)_tmp482 != 0)goto
_LL30F;_LL30E: return 1;_LL30F:;_LL310: return 0;_LL2EA:;}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp483=
_cycalloc(sizeof(*_tmp483));_tmp483->v=(void*)t1;_tmp483;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp487;_tmp487.tag=0;_tmp487.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp486;_tmp486.tag=0;_tmp486.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp484[2]={& _tmp486,& _tmp487};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,({const char*_tmp485="type mismatch: expecting %s but found %s";
_tag_arr(_tmp485,sizeof(char),_get_zero_arr_size(_tmp485,41));}),_tag_arr(
_tmp484,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp488=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL312: if(_tmp488 <= (void*)3  || *((int*)_tmp488)!= 4)goto _LL314;_LL313: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL311;_LL314:;_LL315: return 0;_LL311:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp489=Cyc_Tcutil_compress(
t);_LL317: if(_tmp489 <= (void*)3  || *((int*)_tmp489)!= 5)goto _LL319;_LL318: goto
_LL31A;_LL319: if(_tmp489 <= (void*)3  || *((int*)_tmp489)!= 14)goto _LL31B;_LL31A:
goto _LL31C;_LL31B: if(_tmp489 <= (void*)3  || *((int*)_tmp489)!= 17)goto _LL31D;
_LL31C: goto _LL31E;_LL31D: if(_tmp489 <= (void*)3  || *((int*)_tmp489)!= 12)goto
_LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp489 <= (void*)3  || *((int*)_tmp489)!= 13)
goto _LL321;_LL320: return 1;_LL321:;_LL322: return 0;_LL316:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp48A[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48B="integral size mismatch; conversion supplied";
_tag_arr(_tmp48B,sizeof(char),_get_zero_arr_size(_tmp48B,44));}),_tag_arr(
_tmp48A,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp48C[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48D="integral size mismatch; conversion supplied";
_tag_arr(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,44));}),_tag_arr(
_tmp48C,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp48F=({struct _tuple6 _tmp48E;_tmp48E.f1=
t1;_tmp48E.f2=t2;_tmp48E;});void*_tmp490;struct Cyc_Absyn_PtrInfo _tmp491;void*
_tmp492;struct Cyc_Absyn_PtrInfo _tmp493;void*_tmp494;struct Cyc_Absyn_ArrayInfo
_tmp495;void*_tmp496;struct Cyc_Absyn_Tqual _tmp497;struct Cyc_Absyn_Exp*_tmp498;
struct Cyc_Absyn_Conref*_tmp499;void*_tmp49A;struct Cyc_Absyn_ArrayInfo _tmp49B;
void*_tmp49C;struct Cyc_Absyn_Tqual _tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Conref*
_tmp49F;void*_tmp4A0;struct Cyc_Absyn_TunionFieldInfo _tmp4A1;void*_tmp4A2;struct
Cyc_Absyn_Tuniondecl*_tmp4A3;struct Cyc_Absyn_Tunionfield*_tmp4A4;struct Cyc_List_List*
_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_TunionInfo _tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Tuniondecl**
_tmp4A9;struct Cyc_Absyn_Tuniondecl*_tmp4AA;struct Cyc_List_List*_tmp4AB;void*
_tmp4AC;struct Cyc_Absyn_PtrInfo _tmp4AD;void*_tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;
struct Cyc_Absyn_PtrAtts _tmp4B0;void*_tmp4B1;struct Cyc_Absyn_Conref*_tmp4B2;
struct Cyc_Absyn_Conref*_tmp4B3;struct Cyc_Absyn_Conref*_tmp4B4;void*_tmp4B5;
struct Cyc_Absyn_TunionInfo _tmp4B6;void*_tmp4B7;struct Cyc_Absyn_Tuniondecl**
_tmp4B8;struct Cyc_Absyn_Tuniondecl*_tmp4B9;struct Cyc_List_List*_tmp4BA;void*
_tmp4BB;void*_tmp4BC;void*_tmp4BD;void*_tmp4BE;void*_tmp4BF;void*_tmp4C0;_LL324:
_tmp490=_tmp48F.f1;if(_tmp490 <= (void*)3  || *((int*)_tmp490)!= 4)goto _LL326;
_tmp491=((struct Cyc_Absyn_PointerType_struct*)_tmp490)->f1;_tmp492=_tmp48F.f2;
if(_tmp492 <= (void*)3  || *((int*)_tmp492)!= 4)goto _LL326;_tmp493=((struct Cyc_Absyn_PointerType_struct*)
_tmp492)->f1;_LL325: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp491.ptr_atts).nullable,(
_tmp493.ptr_atts).nullable)){void*_tmp4C1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp491.ptr_atts).nullable))->v;
int _tmp4C2;_LL333: if(_tmp4C1 <= (void*)1  || *((int*)_tmp4C1)!= 0)goto _LL335;
_tmp4C2=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C1)->f1;_LL334: okay=!
_tmp4C2;goto _LL332;_LL335:;_LL336:({void*_tmp4C3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4C4="silent_castable conref not eq";
_tag_arr(_tmp4C4,sizeof(char),_get_zero_arr_size(_tmp4C4,30));}),_tag_arr(
_tmp4C3,sizeof(void*),0));});_LL332:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp491.ptr_atts).bounds,(_tmp493.ptr_atts).bounds)){struct _tuple6 _tmp4C6=({
struct _tuple6 _tmp4C5;_tmp4C5.f1=(void*)(Cyc_Absyn_compress_conref((_tmp491.ptr_atts).bounds))->v;
_tmp4C5.f2=(void*)(Cyc_Absyn_compress_conref((_tmp493.ptr_atts).bounds))->v;
_tmp4C5;});void*_tmp4C7;void*_tmp4C8;void*_tmp4C9;void*_tmp4CA;void*_tmp4CB;
_LL338: _tmp4C7=_tmp4C6.f1;if(_tmp4C7 <= (void*)1  || *((int*)_tmp4C7)!= 0)goto
_LL33A;_tmp4C8=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C7)->f1;_tmp4C9=
_tmp4C6.f2;if(_tmp4C9 <= (void*)1  || *((int*)_tmp4C9)!= 0)goto _LL33A;_tmp4CA=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C9)->f1;_LL339:{struct _tuple6
_tmp4CD=({struct _tuple6 _tmp4CC;_tmp4CC.f1=_tmp4C8;_tmp4CC.f2=_tmp4CA;_tmp4CC;});
void*_tmp4CE;void*_tmp4CF;void*_tmp4D0;void*_tmp4D1;void*_tmp4D2;struct Cyc_Absyn_Exp*
_tmp4D3;void*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;void*_tmp4D6;void*_tmp4D7;
struct Cyc_Absyn_Exp*_tmp4D8;void*_tmp4D9;void*_tmp4DA;void*_tmp4DB;void*_tmp4DC;
void*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;void*_tmp4DF;void*_tmp4E0;void*_tmp4E1;
void*_tmp4E2;_LL33F: _tmp4CE=_tmp4CD.f1;if(_tmp4CE <= (void*)1  || *((int*)_tmp4CE)
!= 0)goto _LL341;_tmp4CF=_tmp4CD.f2;if((int)_tmp4CF != 0)goto _LL341;_LL340: goto
_LL342;_LL341: _tmp4D0=_tmp4CD.f1;if((int)_tmp4D0 != 0)goto _LL343;_tmp4D1=_tmp4CD.f2;
if((int)_tmp4D1 != 0)goto _LL343;_LL342: okay=1;goto _LL33E;_LL343: _tmp4D2=_tmp4CD.f1;
if(_tmp4D2 <= (void*)1  || *((int*)_tmp4D2)!= 0)goto _LL345;_tmp4D3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D2)->f1;_tmp4D4=_tmp4CD.f2;if(_tmp4D4 <= (void*)1  || *((int*)_tmp4D4)!= 0)
goto _LL345;_tmp4D5=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D4)->f1;_LL344: okay=
okay  && Cyc_Evexp_lte_const_exp(_tmp4D5,_tmp4D3);if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp493.ptr_atts).zero_term))({void*_tmp4E3[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4E4="implicit cast to shorter array";
_tag_arr(_tmp4E4,sizeof(char),_get_zero_arr_size(_tmp4E4,31));}),_tag_arr(
_tmp4E3,sizeof(void*),0));});goto _LL33E;_LL345: _tmp4D6=_tmp4CD.f1;if((int)
_tmp4D6 != 0)goto _LL347;_tmp4D7=_tmp4CD.f2;if(_tmp4D7 <= (void*)1  || *((int*)
_tmp4D7)!= 0)goto _LL347;_tmp4D8=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D7)->f1;
_LL346: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp491.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((_tmp493.ptr_atts).bounds))
goto _LL33E;okay=0;goto _LL33E;_LL347: _tmp4D9=_tmp4CD.f1;if(_tmp4D9 <= (void*)1  || *((
int*)_tmp4D9)!= 1)goto _LL349;_tmp4DA=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4D9)->f1;_tmp4DB=_tmp4CD.f2;if(_tmp4DB <= (void*)1  || *((int*)_tmp4DB)!= 1)
goto _LL349;_tmp4DC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DB)->f1;
_LL348: if(!Cyc_Tcutil_unify(_tmp4DA,_tmp4DC)){struct _tuple6 _tmp4E6=({struct
_tuple6 _tmp4E5;_tmp4E5.f1=Cyc_Tcutil_compress(_tmp4DA);_tmp4E5.f2=Cyc_Tcutil_compress(
_tmp4DC);_tmp4E5;});void*_tmp4E7;int _tmp4E8;void*_tmp4E9;int _tmp4EA;_LL350:
_tmp4E7=_tmp4E6.f1;if(_tmp4E7 <= (void*)3  || *((int*)_tmp4E7)!= 18)goto _LL352;
_tmp4E8=((struct Cyc_Absyn_TypeInt_struct*)_tmp4E7)->f1;_tmp4E9=_tmp4E6.f2;if(
_tmp4E9 <= (void*)3  || *((int*)_tmp4E9)!= 18)goto _LL352;_tmp4EA=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4E9)->f1;_LL351: if(_tmp4E8 < _tmp4EA)okay=0;goto _LL34F;_LL352:;_LL353: okay=0;
goto _LL34F;_LL34F:;}goto _LL33E;_LL349: _tmp4DD=_tmp4CD.f1;if(_tmp4DD <= (void*)1
 || *((int*)_tmp4DD)!= 0)goto _LL34B;_tmp4DE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DD)->f1;_tmp4DF=_tmp4CD.f2;if(_tmp4DF <= (void*)1  || *((int*)_tmp4DF)!= 1)
goto _LL34B;_tmp4E0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DF)->f1;
_LL34A: {unsigned int _tmp4EC;int _tmp4ED;struct _tuple7 _tmp4EB=Cyc_Evexp_eval_const_uint_exp(
_tmp4DE);_tmp4EC=_tmp4EB.f1;_tmp4ED=_tmp4EB.f2;if(!_tmp4ED){okay=0;goto _LL33E;}{
void*_tmp4EE=Cyc_Tcutil_compress(_tmp4E0);int _tmp4EF;_LL355: if(_tmp4EE <= (void*)
3  || *((int*)_tmp4EE)!= 18)goto _LL357;_tmp4EF=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4EE)->f1;_LL356: if(_tmp4EC < _tmp4EF)okay=0;goto _LL354;_LL357:;_LL358: okay=0;
goto _LL354;_LL354:;}goto _LL33E;}_LL34B: _tmp4E1=_tmp4CD.f1;if(_tmp4E1 <= (void*)1
 || *((int*)_tmp4E1)!= 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D: _tmp4E2=_tmp4CD.f2;
if(_tmp4E2 <= (void*)1  || *((int*)_tmp4E2)!= 1)goto _LL33E;_LL34E: okay=0;goto
_LL33E;_LL33E:;}goto _LL337;_LL33A: _tmp4CB=_tmp4C6.f1;if((int)_tmp4CB != 0)goto
_LL33C;_LL33B: okay=0;goto _LL337;_LL33C:;_LL33D:({void*_tmp4F0[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4F1="silent_castable conrefs didn't unify";
_tag_arr(_tmp4F1,sizeof(char),_get_zero_arr_size(_tmp4F1,37));}),_tag_arr(
_tmp4F0,sizeof(void*),0));});_LL337:;}okay=okay  && Cyc_Tcutil_unify((void*)
_tmp491.elt_typ,(void*)_tmp493.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(
_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp491.ptr_atts).rgn,(void*)(_tmp493.ptr_atts).rgn));okay=okay  && (!(
_tmp491.elt_tq).real_const  || (_tmp493.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp491.ptr_atts).zero_term,(_tmp493.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp491.ptr_atts).zero_term)
 && (_tmp493.elt_tq).real_const);return okay;}_LL326: _tmp494=_tmp48F.f1;if(
_tmp494 <= (void*)3  || *((int*)_tmp494)!= 7)goto _LL328;_tmp495=((struct Cyc_Absyn_ArrayType_struct*)
_tmp494)->f1;_tmp496=(void*)_tmp495.elt_type;_tmp497=_tmp495.tq;_tmp498=_tmp495.num_elts;
_tmp499=_tmp495.zero_term;_tmp49A=_tmp48F.f2;if(_tmp49A <= (void*)3  || *((int*)
_tmp49A)!= 7)goto _LL328;_tmp49B=((struct Cyc_Absyn_ArrayType_struct*)_tmp49A)->f1;
_tmp49C=(void*)_tmp49B.elt_type;_tmp49D=_tmp49B.tq;_tmp49E=_tmp49B.num_elts;
_tmp49F=_tmp49B.zero_term;_LL327: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp499,_tmp49F) && ((_tmp498 != 0  && _tmp49E != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp498,(struct Cyc_Absyn_Exp*)_tmp49E));return(okay  && Cyc_Tcutil_unify(
_tmp496,_tmp49C)) && (!_tmp497.real_const  || _tmp49D.real_const);}_LL328: _tmp4A0=
_tmp48F.f1;if(_tmp4A0 <= (void*)3  || *((int*)_tmp4A0)!= 3)goto _LL32A;_tmp4A1=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4A0)->f1;_tmp4A2=(void*)_tmp4A1.field_info;
if(*((int*)_tmp4A2)!= 1)goto _LL32A;_tmp4A3=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4A2)->f1;_tmp4A4=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4A2)->f2;
_tmp4A5=_tmp4A1.targs;_tmp4A6=_tmp48F.f2;if(_tmp4A6 <= (void*)3  || *((int*)
_tmp4A6)!= 2)goto _LL32A;_tmp4A7=((struct Cyc_Absyn_TunionType_struct*)_tmp4A6)->f1;
_tmp4A8=(void*)_tmp4A7.tunion_info;if(*((int*)_tmp4A8)!= 1)goto _LL32A;_tmp4A9=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp4A8)->f1;_tmp4AA=*_tmp4A9;_tmp4AB=
_tmp4A7.targs;_LL329: if((_tmp4A3 == _tmp4AA  || Cyc_Absyn_qvar_cmp(_tmp4A3->name,
_tmp4AA->name)== 0) && _tmp4A4->typs == 0){for(0;_tmp4A5 != 0  && _tmp4AB != 0;(
_tmp4A5=_tmp4A5->tl,_tmp4AB=_tmp4AB->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4A5->hd,(
void*)_tmp4AB->hd))break;}if(_tmp4A5 == 0  && _tmp4AB == 0)return 1;}return 0;_LL32A:
_tmp4AC=_tmp48F.f1;if(_tmp4AC <= (void*)3  || *((int*)_tmp4AC)!= 4)goto _LL32C;
_tmp4AD=((struct Cyc_Absyn_PointerType_struct*)_tmp4AC)->f1;_tmp4AE=(void*)
_tmp4AD.elt_typ;_tmp4AF=_tmp4AD.elt_tq;_tmp4B0=_tmp4AD.ptr_atts;_tmp4B1=(void*)
_tmp4B0.rgn;_tmp4B2=_tmp4B0.nullable;_tmp4B3=_tmp4B0.bounds;_tmp4B4=_tmp4B0.zero_term;
_tmp4B5=_tmp48F.f2;if(_tmp4B5 <= (void*)3  || *((int*)_tmp4B5)!= 2)goto _LL32C;
_tmp4B6=((struct Cyc_Absyn_TunionType_struct*)_tmp4B5)->f1;_tmp4B7=(void*)_tmp4B6.tunion_info;
if(*((int*)_tmp4B7)!= 1)goto _LL32C;_tmp4B8=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4B7)->f1;_tmp4B9=*_tmp4B8;_tmp4BA=_tmp4B6.targs;_tmp4BB=(void*)_tmp4B6.rgn;
_LL32B:{void*_tmp4F2=Cyc_Tcutil_compress(_tmp4AE);struct Cyc_Absyn_TunionFieldInfo
_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_Tuniondecl*_tmp4F5;struct Cyc_Absyn_Tunionfield*
_tmp4F6;struct Cyc_List_List*_tmp4F7;_LL35A: if(_tmp4F2 <= (void*)3  || *((int*)
_tmp4F2)!= 3)goto _LL35C;_tmp4F3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4F2)->f1;_tmp4F4=(void*)_tmp4F3.field_info;if(*((int*)_tmp4F4)!= 1)goto
_LL35C;_tmp4F5=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F4)->f1;_tmp4F6=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F4)->f2;_tmp4F7=_tmp4F3.targs;
_LL35B: if(!Cyc_Tcutil_unify(_tmp4B1,_tmp4BB) && !Cyc_Tcenv_region_outlives(te,
_tmp4B1,_tmp4BB))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4B2,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4B3,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4B4,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4B9->name,
_tmp4F5->name)== 0  && _tmp4F6->typs != 0){int okay=1;for(0;_tmp4F7 != 0  && _tmp4BA
!= 0;(_tmp4F7=_tmp4F7->tl,_tmp4BA=_tmp4BA->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp4F7->hd,(void*)_tmp4BA->hd)){okay=0;break;}}if((!okay  || _tmp4F7 != 0) || 
_tmp4BA != 0)return 0;return 1;}goto _LL359;_LL35C:;_LL35D: goto _LL359;_LL359:;}
return 0;_LL32C: _tmp4BC=_tmp48F.f1;if(_tmp4BC <= (void*)3  || *((int*)_tmp4BC)!= 14)
goto _LL32E;_tmp4BD=_tmp48F.f2;if(_tmp4BD <= (void*)3  || *((int*)_tmp4BD)!= 5)goto
_LL32E;_tmp4BE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BD)->f2;if((int)
_tmp4BE != 2)goto _LL32E;_LL32D: goto _LL32F;_LL32E: _tmp4BF=_tmp48F.f1;if(_tmp4BF <= (
void*)3  || *((int*)_tmp4BF)!= 17)goto _LL330;_tmp4C0=_tmp48F.f2;if(_tmp4C0 <= (
void*)3  || *((int*)_tmp4C0)!= 5)goto _LL330;_LL32F: return 1;_LL330:;_LL331: return
Cyc_Tcutil_unify(t1,t2);_LL323:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp4F8=Cyc_Tcutil_compress(t);_LL35F: if(_tmp4F8 <= (void*)3  || *((int*)_tmp4F8)
!= 4)goto _LL361;_LL360: return 1;_LL361:;_LL362: return 0;_LL35E:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4F9=(void*)e->r;void*_tmp4FA;int _tmp4FB;void*
_tmp4FC;char _tmp4FD;void*_tmp4FE;short _tmp4FF;void*_tmp500;long long _tmp501;void*
_tmp502;struct Cyc_Absyn_Exp*_tmp503;_LL364: if(*((int*)_tmp4F9)!= 0)goto _LL366;
_tmp4FA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4F9)->f1;if(_tmp4FA <= (void*)
1  || *((int*)_tmp4FA)!= 2)goto _LL366;_tmp4FB=((struct Cyc_Absyn_Int_c_struct*)
_tmp4FA)->f2;if(_tmp4FB != 0)goto _LL366;_LL365: goto _LL367;_LL366: if(*((int*)
_tmp4F9)!= 0)goto _LL368;_tmp4FC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4F9)->f1;
if(_tmp4FC <= (void*)1  || *((int*)_tmp4FC)!= 0)goto _LL368;_tmp4FD=((struct Cyc_Absyn_Char_c_struct*)
_tmp4FC)->f2;if(_tmp4FD != 0)goto _LL368;_LL367: goto _LL369;_LL368: if(*((int*)
_tmp4F9)!= 0)goto _LL36A;_tmp4FE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4F9)->f1;
if(_tmp4FE <= (void*)1  || *((int*)_tmp4FE)!= 1)goto _LL36A;_tmp4FF=((struct Cyc_Absyn_Short_c_struct*)
_tmp4FE)->f2;if(_tmp4FF != 0)goto _LL36A;_LL369: goto _LL36B;_LL36A: if(*((int*)
_tmp4F9)!= 0)goto _LL36C;_tmp500=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4F9)->f1;
if(_tmp500 <= (void*)1  || *((int*)_tmp500)!= 3)goto _LL36C;_tmp501=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp500)->f2;if(_tmp501 != 0)goto _LL36C;_LL36B: return 1;_LL36C: if(*((int*)_tmp4F9)
!= 15)goto _LL36E;_tmp502=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4F9)->f1;
_tmp503=((struct Cyc_Absyn_Cast_e_struct*)_tmp4F9)->f2;_LL36D: return Cyc_Tcutil_is_zero(
_tmp503) && Cyc_Tcutil_admits_zero(_tmp502);_LL36E:;_LL36F: return 0;_LL363:;}
struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp504=
_cycalloc(sizeof(*_tmp504));_tmp504[0]=({struct Cyc_Absyn_Const_e_struct _tmp505;
_tmp505.tag=0;_tmp505.f1=(void*)((void*)0);_tmp505;});_tmp504;})));{struct Cyc_List_List*
_tmp506=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp507=({struct Cyc_Absyn_PointerType_struct*_tmp508=_cycalloc(sizeof(*_tmp508));
_tmp508[0]=({struct Cyc_Absyn_PointerType_struct _tmp509;_tmp509.tag=4;_tmp509.f1=({
struct Cyc_Absyn_PtrInfo _tmp50A;_tmp50A.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp50D=_cycalloc(sizeof(*
_tmp50D));_tmp50D->v=_tmp506;_tmp50D;}));_tmp50A.elt_tq=Cyc_Absyn_empty_tqual();
_tmp50A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50B;_tmp50B.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp50C=_cycalloc(
sizeof(*_tmp50C));_tmp50C->v=_tmp506;_tmp50C;}));_tmp50B.nullable=Cyc_Absyn_true_conref;
_tmp50B.bounds=Cyc_Absyn_empty_conref();_tmp50B.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp50B;});_tmp50A;});_tmp509;});_tmp508;});(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp507));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(void*c){void*
_tmp50E=c;_LL371: if((int)_tmp50E != 0)goto _LL373;_LL372: return({const char*_tmp50F="unknown";
_tag_arr(_tmp50F,sizeof(char),_get_zero_arr_size(_tmp50F,8));});_LL373: if((int)
_tmp50E != 1)goto _LL375;_LL374: return({const char*_tmp510="no coercion";_tag_arr(
_tmp510,sizeof(char),_get_zero_arr_size(_tmp510,12));});_LL375: if((int)_tmp50E != 
2)goto _LL377;_LL376: return({const char*_tmp511="null check";_tag_arr(_tmp511,
sizeof(char),_get_zero_arr_size(_tmp511,11));});_LL377: if((int)_tmp50E != 3)goto
_LL370;_LL378: return({const char*_tmp512="other coercion";_tag_arr(_tmp512,
sizeof(char),_get_zero_arr_size(_tmp512,15));});_LL370:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp516;_tmp516.tag=0;_tmp516.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp515;_tmp515.tag=0;_tmp515.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp513[2]={&
_tmp515,& _tmp516};Cyc_Tcutil_warn(e->loc,({const char*_tmp514="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,53));}),_tag_arr(
_tmp513,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp51A;_tmp51A.tag=0;
_tmp51A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp519;_tmp519.tag=0;_tmp519.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp517[2]={& _tmp519,&
_tmp51A};Cyc_Tcutil_warn(e->loc,({const char*_tmp518="implicit cast from %s to %s";
_tag_arr(_tmp518,sizeof(char),_get_zero_arr_size(_tmp518,28));}),_tag_arr(
_tmp517,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp51B=Cyc_Tcutil_compress(t);
_LL37A: if(_tmp51B <= (void*)3  || *((int*)_tmp51B)!= 5)goto _LL37C;_LL37B: goto
_LL37D;_LL37C: if((int)_tmp51B != 1)goto _LL37E;_LL37D: goto _LL37F;_LL37E: if(_tmp51B
<= (void*)3  || *((int*)_tmp51B)!= 6)goto _LL380;_LL37F: return 1;_LL380:;_LL381:
return 0;_LL379:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp51C=_cycalloc(
sizeof(*_tmp51C));_tmp51C->f1=x->tq;_tmp51C->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp51C;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp51D=t1;struct Cyc_List_List*
_tmp51E;struct Cyc_Absyn_AggrInfo _tmp51F;void*_tmp520;struct Cyc_Absyn_Aggrdecl**
_tmp521;struct Cyc_Absyn_Aggrdecl*_tmp522;struct Cyc_List_List*_tmp523;void*
_tmp524;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_FnInfo _tmp526;_LL383: if((int)
_tmp51D != 0)goto _LL385;_LL384: return 0;_LL385: if(_tmp51D <= (void*)3  || *((int*)
_tmp51D)!= 9)goto _LL387;_tmp51E=((struct Cyc_Absyn_TupleType_struct*)_tmp51D)->f1;
_LL386: return _tmp51E;_LL387: if(_tmp51D <= (void*)3  || *((int*)_tmp51D)!= 10)goto
_LL389;_tmp51F=((struct Cyc_Absyn_AggrType_struct*)_tmp51D)->f1;_tmp520=(void*)
_tmp51F.aggr_info;if(*((int*)_tmp520)!= 1)goto _LL389;_tmp521=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp520)->f1;_tmp522=*_tmp521;_tmp523=_tmp51F.targs;_LL388: if((((void*)_tmp522->kind
== (void*)1  || _tmp522->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp522->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp522->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp527=_cycalloc(
sizeof(*_tmp527));_tmp527->hd=({struct _tuple4*_tmp528=_cycalloc(sizeof(*_tmp528));
_tmp528->f1=Cyc_Absyn_empty_tqual();_tmp528->f2=t1;_tmp528;});_tmp527->tl=0;
_tmp527;});{struct Cyc_List_List*_tmp529=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp522->tvs,_tmp523);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp529,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp522->impl))->fields);}_LL389: if(
_tmp51D <= (void*)3  || *((int*)_tmp51D)!= 11)goto _LL38B;_tmp524=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp51D)->f1;if((int)_tmp524 != 0)goto _LL38B;
_tmp525=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp51D)->f2;_LL38A: return((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,
0,_tmp525);_LL38B: if(_tmp51D <= (void*)3  || *((int*)_tmp51D)!= 8)goto _LL38D;
_tmp526=((struct Cyc_Absyn_FnType_struct*)_tmp51D)->f1;_LL38C: return({struct Cyc_List_List*
_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->hd=({struct _tuple4*_tmp52B=
_cycalloc(sizeof(*_tmp52B));_tmp52B->f1=Cyc_Absyn_const_tqual();_tmp52B->f2=t1;
_tmp52B;});_tmp52A->tl=0;_tmp52A;});_LL38D:;_LL38E: return({struct Cyc_List_List*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->hd=({struct _tuple4*_tmp52D=
_cycalloc(sizeof(*_tmp52D));_tmp52D->f1=Cyc_Absyn_empty_tqual();_tmp52D->f2=t1;
_tmp52D;});_tmp52C->tl=0;_tmp52C;});_LL382:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp52E=(void*)t->hd;_LL390: if((int)_tmp52E != 16)goto _LL392;
_LL391: goto _LL393;_LL392: if((int)_tmp52E != 3)goto _LL394;_LL393: goto _LL395;_LL394:
if(_tmp52E <= (void*)17  || *((int*)_tmp52E)!= 4)goto _LL396;_LL395: continue;_LL396:;
_LL397: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL38F:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp530=({struct _tuple6 _tmp52F;_tmp52F.f1=t1;_tmp52F.f2=t2;_tmp52F;});void*
_tmp531;struct Cyc_Absyn_PtrInfo _tmp532;void*_tmp533;struct Cyc_Absyn_Tqual _tmp534;
struct Cyc_Absyn_PtrAtts _tmp535;void*_tmp536;struct Cyc_Absyn_Conref*_tmp537;
struct Cyc_Absyn_Conref*_tmp538;struct Cyc_Absyn_Conref*_tmp539;void*_tmp53A;
struct Cyc_Absyn_PtrInfo _tmp53B;void*_tmp53C;struct Cyc_Absyn_Tqual _tmp53D;struct
Cyc_Absyn_PtrAtts _tmp53E;void*_tmp53F;struct Cyc_Absyn_Conref*_tmp540;struct Cyc_Absyn_Conref*
_tmp541;struct Cyc_Absyn_Conref*_tmp542;void*_tmp543;struct Cyc_Absyn_FnInfo
_tmp544;void*_tmp545;struct Cyc_Absyn_FnInfo _tmp546;_LL399: _tmp531=_tmp530.f1;if(
_tmp531 <= (void*)3  || *((int*)_tmp531)!= 4)goto _LL39B;_tmp532=((struct Cyc_Absyn_PointerType_struct*)
_tmp531)->f1;_tmp533=(void*)_tmp532.elt_typ;_tmp534=_tmp532.elt_tq;_tmp535=
_tmp532.ptr_atts;_tmp536=(void*)_tmp535.rgn;_tmp537=_tmp535.nullable;_tmp538=
_tmp535.bounds;_tmp539=_tmp535.zero_term;_tmp53A=_tmp530.f2;if(_tmp53A <= (void*)
3  || *((int*)_tmp53A)!= 4)goto _LL39B;_tmp53B=((struct Cyc_Absyn_PointerType_struct*)
_tmp53A)->f1;_tmp53C=(void*)_tmp53B.elt_typ;_tmp53D=_tmp53B.elt_tq;_tmp53E=
_tmp53B.ptr_atts;_tmp53F=(void*)_tmp53E.rgn;_tmp540=_tmp53E.nullable;_tmp541=
_tmp53E.bounds;_tmp542=_tmp53E.zero_term;_LL39A: if(_tmp534.real_const  && !
_tmp53D.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp537,
_tmp540) && ((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp537))
 && !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp540))return 0;
if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp539,_tmp542) && !((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp539)) && ((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp542))return 0;if(!Cyc_Tcutil_unify(_tmp536,_tmp53F)
 && !Cyc_Tcenv_region_outlives(te,_tmp536,_tmp53F))return 0;if(!Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp538,_tmp541)){struct _tuple6 _tmp548=({struct
_tuple6 _tmp547;_tmp547.f1=Cyc_Absyn_conref_val(_tmp538);_tmp547.f2=Cyc_Absyn_conref_val(
_tmp541);_tmp547;});void*_tmp549;void*_tmp54A;void*_tmp54B;struct Cyc_Absyn_Exp*
_tmp54C;void*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;_LL3A0: _tmp549=_tmp548.f1;if(
_tmp549 <= (void*)1  || *((int*)_tmp549)!= 0)goto _LL3A2;_tmp54A=_tmp548.f2;if((int)
_tmp54A != 0)goto _LL3A2;_LL3A1: goto _LL39F;_LL3A2: _tmp54B=_tmp548.f1;if(_tmp54B <= (
void*)1  || *((int*)_tmp54B)!= 0)goto _LL3A4;_tmp54C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp54B)->f1;_tmp54D=_tmp548.f2;if(_tmp54D <= (void*)1  || *((int*)_tmp54D)!= 0)
goto _LL3A4;_tmp54E=((struct Cyc_Absyn_Upper_b_struct*)_tmp54D)->f1;_LL3A3: if(!Cyc_Evexp_lte_const_exp(
_tmp54E,_tmp54C))return 0;goto _LL39F;_LL3A4:;_LL3A5: return 0;_LL39F:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->hd=({
struct _tuple6*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->f1=t1;_tmp550->f2=t2;
_tmp550;});_tmp54F->tl=assume;_tmp54F;}),_tmp533,_tmp53C);_LL39B: _tmp543=_tmp530.f1;
if(_tmp543 <= (void*)3  || *((int*)_tmp543)!= 8)goto _LL39D;_tmp544=((struct Cyc_Absyn_FnType_struct*)
_tmp543)->f1;_tmp545=_tmp530.f2;if(_tmp545 <= (void*)3  || *((int*)_tmp545)!= 8)
goto _LL39D;_tmp546=((struct Cyc_Absyn_FnType_struct*)_tmp545)->f1;_LL39C: if(
_tmp544.tvars != 0  || _tmp546.tvars != 0){struct Cyc_List_List*_tmp551=_tmp544.tvars;
struct Cyc_List_List*_tmp552=_tmp546.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp551)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp552))return 0;{
struct _RegionHandle _tmp553=_new_region("r");struct _RegionHandle*r=& _tmp553;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp551 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp551->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp552))->hd)){int _tmp554=0;_npop_handler(0);
return _tmp554;}inst=({struct Cyc_List_List*_tmp555=_region_malloc(r,sizeof(*
_tmp555));_tmp555->hd=({struct _tuple8*_tmp556=_region_malloc(r,sizeof(*_tmp556));
_tmp556->f1=(struct Cyc_Absyn_Tvar*)_tmp552->hd;_tmp556->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_VarType_struct
_tmp558;_tmp558.tag=1;_tmp558.f1=(struct Cyc_Absyn_Tvar*)_tmp551->hd;_tmp558;});
_tmp557;});_tmp556;});_tmp555->tl=inst;_tmp555;});_tmp551=_tmp551->tl;_tmp552=
_tmp552->tl;}if(inst != 0){_tmp544.tvars=0;_tmp546.tvars=0;{int _tmp55D=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp559=_cycalloc(sizeof(*
_tmp559));_tmp559[0]=({struct Cyc_Absyn_FnType_struct _tmp55A;_tmp55A.tag=8;
_tmp55A.f1=_tmp544;_tmp55A;});_tmp559;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_FnType_struct
_tmp55C;_tmp55C.tag=8;_tmp55C.f1=_tmp546;_tmp55C;});_tmp55B;}));_npop_handler(0);
return _tmp55D;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp544.ret_typ,(void*)_tmp546.ret_typ))return 0;{struct Cyc_List_List*_tmp55E=
_tmp544.args;struct Cyc_List_List*_tmp55F=_tmp546.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp55E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp55F))return 0;for(0;_tmp55E != 0;(_tmp55E=_tmp55E->tl,_tmp55F=_tmp55F->tl)){
struct Cyc_Absyn_Tqual _tmp561;void*_tmp562;struct _tuple2 _tmp560=*((struct _tuple2*)
_tmp55E->hd);_tmp561=_tmp560.f2;_tmp562=_tmp560.f3;{struct Cyc_Absyn_Tqual _tmp564;
void*_tmp565;struct _tuple2 _tmp563=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp55F))->hd);_tmp564=_tmp563.f2;_tmp565=_tmp563.f3;if(_tmp564.real_const
 && !_tmp561.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp565,_tmp562))return
0;}}if(_tmp544.c_varargs != _tmp546.c_varargs)return 0;if(_tmp544.cyc_varargs != 0
 && _tmp546.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp566=*_tmp544.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp567=*_tmp546.cyc_varargs;if((_tmp567.tq).real_const
 && !(_tmp566.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp567.type,(
void*)_tmp566.type))return 0;}else{if(_tmp544.cyc_varargs != 0  || _tmp546.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp544.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp546.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp544.rgn_po,_tmp546.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp544.attributes,_tmp546.attributes))return 0;return 1;}_LL39D:;_LL39E: return 0;
_LL398:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp569=({
struct _tuple6 _tmp568;_tmp568.f1=Cyc_Tcutil_compress(t1);_tmp568.f2=Cyc_Tcutil_compress(
t2);_tmp568;});void*_tmp56A;void*_tmp56B;void*_tmp56C;void*_tmp56D;_LL3A7:
_tmp56A=_tmp569.f1;if(_tmp56A <= (void*)3  || *((int*)_tmp56A)!= 5)goto _LL3A9;
_tmp56B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56A)->f2;_tmp56C=_tmp569.f2;
if(_tmp56C <= (void*)3  || *((int*)_tmp56C)!= 5)goto _LL3A9;_tmp56D=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp56C)->f2;_LL3A8: return _tmp56B == _tmp56D;_LL3A9:;
_LL3AA: return 0;_LL3A6:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp56F;struct
Cyc_Absyn_Tqual _tmp570;void*_tmp571;struct _tuple4*_tmp56E=(struct _tuple4*)tqs1->hd;
_tmp56F=*_tmp56E;_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;{struct _tuple4 _tmp573;
struct Cyc_Absyn_Tqual _tmp574;void*_tmp575;struct _tuple4*_tmp572=(struct _tuple4*)
tqs2->hd;_tmp573=*_tmp572;_tmp574=_tmp573.f1;_tmp575=_tmp573.f2;if(_tmp574.real_const
 && Cyc_Tcutil_subtype(te,assume,_tmp571,_tmp575))continue;else{if(Cyc_Tcutil_unify(
_tmp571,_tmp575))continue;else{if(Cyc_Tcutil_isomorphic(_tmp571,_tmp575))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp576=Cyc_Tcutil_compress(t);void*_tmp577;_LL3AC: if(_tmp576 <= (void*)3
 || *((int*)_tmp576)!= 5)goto _LL3AE;_tmp577=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp576)->f2;if((int)_tmp577 != 0)goto _LL3AE;_LL3AD: return 1;_LL3AE:;_LL3AF: return
0;_LL3AB:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp578=t2;
void*_tmp579;_LL3B1: if(_tmp578 <= (void*)3  || *((int*)_tmp578)!= 5)goto _LL3B3;
_tmp579=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp578)->f2;if((int)_tmp579 != 
2)goto _LL3B3;_LL3B2: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3B0;_LL3B3:;_LL3B4: goto _LL3B0;_LL3B0:;}{void*_tmp57A=t1;struct Cyc_Absyn_PtrInfo
_tmp57B;void*_tmp57C;struct Cyc_Absyn_Tqual _tmp57D;struct Cyc_Absyn_PtrAtts _tmp57E;
void*_tmp57F;struct Cyc_Absyn_Conref*_tmp580;struct Cyc_Absyn_Conref*_tmp581;
struct Cyc_Absyn_Conref*_tmp582;struct Cyc_Absyn_ArrayInfo _tmp583;void*_tmp584;
struct Cyc_Absyn_Tqual _tmp585;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Conref*
_tmp587;struct Cyc_Absyn_Enumdecl*_tmp588;_LL3B6: if(_tmp57A <= (void*)3  || *((int*)
_tmp57A)!= 4)goto _LL3B8;_tmp57B=((struct Cyc_Absyn_PointerType_struct*)_tmp57A)->f1;
_tmp57C=(void*)_tmp57B.elt_typ;_tmp57D=_tmp57B.elt_tq;_tmp57E=_tmp57B.ptr_atts;
_tmp57F=(void*)_tmp57E.rgn;_tmp580=_tmp57E.nullable;_tmp581=_tmp57E.bounds;
_tmp582=_tmp57E.zero_term;_LL3B7:{void*_tmp589=t2;struct Cyc_Absyn_PtrInfo _tmp58A;
void*_tmp58B;struct Cyc_Absyn_Tqual _tmp58C;struct Cyc_Absyn_PtrAtts _tmp58D;void*
_tmp58E;struct Cyc_Absyn_Conref*_tmp58F;struct Cyc_Absyn_Conref*_tmp590;struct Cyc_Absyn_Conref*
_tmp591;_LL3C5: if(_tmp589 <= (void*)3  || *((int*)_tmp589)!= 4)goto _LL3C7;_tmp58A=((
struct Cyc_Absyn_PointerType_struct*)_tmp589)->f1;_tmp58B=(void*)_tmp58A.elt_typ;
_tmp58C=_tmp58A.elt_tq;_tmp58D=_tmp58A.ptr_atts;_tmp58E=(void*)_tmp58D.rgn;
_tmp58F=_tmp58D.nullable;_tmp590=_tmp58D.bounds;_tmp591=_tmp58D.zero_term;_LL3C6: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp592=({struct Cyc_List_List*_tmp5A1=
_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=({struct _tuple6*_tmp5A2=_cycalloc(
sizeof(*_tmp5A2));_tmp5A2->f1=t1;_tmp5A2->f2=t2;_tmp5A2;});_tmp5A1->tl=0;_tmp5A1;});
int _tmp593=Cyc_Tcutil_ptrsubtype(te,_tmp592,_tmp57C,_tmp58B) && (!_tmp57D.real_const
 || _tmp58C.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp582,_tmp591) || !((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp591);int _tmp594=_tmp593?0:((
Cyc_Tcutil_bits_only(_tmp57C) && Cyc_Tcutil_is_char_type(_tmp58B)) && !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp591)) && (_tmp58C.real_const
 || !_tmp57D.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp581,_tmp590);if(!bounds_ok  && !_tmp594){struct _tuple6 _tmp596=({struct _tuple6
_tmp595;_tmp595.f1=Cyc_Absyn_conref_val(_tmp581);_tmp595.f2=Cyc_Absyn_conref_val(
_tmp590);_tmp595;});void*_tmp597;struct Cyc_Absyn_Exp*_tmp598;void*_tmp599;struct
Cyc_Absyn_Exp*_tmp59A;void*_tmp59B;void*_tmp59C;void*_tmp59D;void*_tmp59E;void*
_tmp59F;void*_tmp5A0;_LL3CA: _tmp597=_tmp596.f1;if(_tmp597 <= (void*)1  || *((int*)
_tmp597)!= 0)goto _LL3CC;_tmp598=((struct Cyc_Absyn_Upper_b_struct*)_tmp597)->f1;
_tmp599=_tmp596.f2;if(_tmp599 <= (void*)1  || *((int*)_tmp599)!= 0)goto _LL3CC;
_tmp59A=((struct Cyc_Absyn_Upper_b_struct*)_tmp599)->f1;_LL3CB: if(Cyc_Evexp_lte_const_exp(
_tmp59A,_tmp598))bounds_ok=1;goto _LL3C9;_LL3CC: _tmp59B=_tmp596.f1;if(_tmp59B <= (
void*)1  || *((int*)_tmp59B)!= 1)goto _LL3CE;_tmp59C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp59B)->f1;_tmp59D=_tmp596.f2;if(_tmp59D <= (void*)1  || *((int*)_tmp59D)!= 1)
goto _LL3CE;_tmp59E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp59D)->f1;
_LL3CD: bounds_ok=Cyc_Tcutil_unify(_tmp59C,_tmp59E);goto _LL3C9;_LL3CE: _tmp59F=
_tmp596.f1;if(_tmp59F <= (void*)1  || *((int*)_tmp59F)!= 1)goto _LL3D0;_LL3CF: goto
_LL3D1;_LL3D0: _tmp5A0=_tmp596.f2;if(_tmp5A0 <= (void*)1  || *((int*)_tmp5A0)!= 1)
goto _LL3D2;_LL3D1: bounds_ok=0;goto _LL3C9;_LL3D2:;_LL3D3: bounds_ok=1;goto _LL3C9;
_LL3C9:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp580) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp58F))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok) && (_tmp593  || _tmp594))
 && (Cyc_Tcutil_unify(_tmp57F,_tmp58E) || Cyc_Tcenv_region_outlives(te,_tmp57F,
_tmp58E)))return coercion;else{return(void*)0;}}}_LL3C7:;_LL3C8: goto _LL3C4;_LL3C4:;}
return(void*)0;_LL3B8: if(_tmp57A <= (void*)3  || *((int*)_tmp57A)!= 7)goto _LL3BA;
_tmp583=((struct Cyc_Absyn_ArrayType_struct*)_tmp57A)->f1;_tmp584=(void*)_tmp583.elt_type;
_tmp585=_tmp583.tq;_tmp586=_tmp583.num_elts;_tmp587=_tmp583.zero_term;_LL3B9:{
void*_tmp5A3=t2;struct Cyc_Absyn_ArrayInfo _tmp5A4;void*_tmp5A5;struct Cyc_Absyn_Tqual
_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Conref*_tmp5A8;_LL3D5: if(
_tmp5A3 <= (void*)3  || *((int*)_tmp5A3)!= 7)goto _LL3D7;_tmp5A4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A3)->f1;_tmp5A5=(void*)_tmp5A4.elt_type;_tmp5A6=_tmp5A4.tq;_tmp5A7=_tmp5A4.num_elts;
_tmp5A8=_tmp5A4.zero_term;_LL3D6: {int okay;okay=((_tmp586 != 0  && _tmp5A7 != 0)
 && ((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp587,_tmp5A8)) && Cyc_Evexp_lte_const_exp((
struct Cyc_Absyn_Exp*)_tmp5A7,(struct Cyc_Absyn_Exp*)_tmp586);return(okay  && Cyc_Tcutil_unify(
_tmp584,_tmp5A5)) && (!_tmp585.real_const  || _tmp5A6.real_const)?(void*)3:(void*)
0;}_LL3D7:;_LL3D8: return(void*)0;_LL3D4:;}return(void*)0;_LL3BA: if(_tmp57A <= (
void*)3  || *((int*)_tmp57A)!= 12)goto _LL3BC;_tmp588=((struct Cyc_Absyn_EnumType_struct*)
_tmp57A)->f2;_LL3BB:{void*_tmp5A9=t2;struct Cyc_Absyn_Enumdecl*_tmp5AA;_LL3DA: if(
_tmp5A9 <= (void*)3  || *((int*)_tmp5A9)!= 12)goto _LL3DC;_tmp5AA=((struct Cyc_Absyn_EnumType_struct*)
_tmp5A9)->f2;_LL3DB: if((_tmp588->fields != 0  && _tmp5AA->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp588->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AA->fields))->v))
return(void*)1;goto _LL3D9;_LL3DC:;_LL3DD: goto _LL3D9;_LL3D9:;}goto _LL3BD;_LL3BC:
if(_tmp57A <= (void*)3  || *((int*)_tmp57A)!= 5)goto _LL3BE;_LL3BD: goto _LL3BF;
_LL3BE: if((int)_tmp57A != 1)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(_tmp57A <= (
void*)3  || *((int*)_tmp57A)!= 6)goto _LL3C2;_LL3C1: return Cyc_Tcutil_coerceable(t2)?(
void*)1:(void*)0;_LL3C2:;_LL3C3: return(void*)0;_LL3B5:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp5AB=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5AD;_tmp5AD.tag=15;_tmp5AD.f1=(void*)t;_tmp5AD.f2=_tmp5AB;_tmp5AD.f3=0;
_tmp5AD.f4=(void*)c;_tmp5AD;});_tmp5AC;})));e->topt=({struct Cyc_Core_Opt*_tmp5AE=
_cycalloc(sizeof(*_tmp5AE));_tmp5AE->v=(void*)t;_tmp5AE;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp5AF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3DF: if(_tmp5AF <= (void*)3  || *((int*)_tmp5AF)!= 5)
goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if(_tmp5AF <= (void*)3  || *((int*)_tmp5AF)!= 
12)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp5AF <= (void*)3  || *((int*)_tmp5AF)
!= 13)goto _LL3E5;_LL3E4: goto _LL3E6;_LL3E5: if(_tmp5AF <= (void*)3  || *((int*)
_tmp5AF)!= 17)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: if(_tmp5AF <= (void*)3  || *((
int*)_tmp5AF)!= 14)goto _LL3E9;_LL3E8: return 1;_LL3E9: if(_tmp5AF <= (void*)3  || *((
int*)_tmp5AF)!= 0)goto _LL3EB;_LL3EA: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3EB:;_LL3EC: return 0;_LL3DE:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp5B0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3EE: if((int)_tmp5B0 != 1)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(_tmp5B0 <= (
void*)3  || *((int*)_tmp5B0)!= 6)goto _LL3F2;_LL3F1: return 1;_LL3F2:;_LL3F3: return 0;
_LL3ED:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp5B1=Cyc_Tcutil_compress(
t);_LL3F5: if(_tmp5B1 <= (void*)3  || *((int*)_tmp5B1)!= 8)goto _LL3F7;_LL3F6: return
1;_LL3F7:;_LL3F8: return 0;_LL3F4:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,
void*t2){struct _tuple6 _tmp5B3=({struct _tuple6 _tmp5B2;_tmp5B2.f1=t1;_tmp5B2.f2=t2;
_tmp5B2;});void*_tmp5B4;int _tmp5B5;void*_tmp5B6;int _tmp5B7;void*_tmp5B8;void*
_tmp5B9;void*_tmp5BA;void*_tmp5BB;void*_tmp5BC;void*_tmp5BD;void*_tmp5BE;void*
_tmp5BF;void*_tmp5C0;void*_tmp5C1;void*_tmp5C2;void*_tmp5C3;void*_tmp5C4;void*
_tmp5C5;void*_tmp5C6;void*_tmp5C7;void*_tmp5C8;void*_tmp5C9;void*_tmp5CA;void*
_tmp5CB;void*_tmp5CC;void*_tmp5CD;void*_tmp5CE;void*_tmp5CF;void*_tmp5D0;void*
_tmp5D1;void*_tmp5D2;void*_tmp5D3;_LL3FA: _tmp5B4=_tmp5B3.f1;if(_tmp5B4 <= (void*)
3  || *((int*)_tmp5B4)!= 6)goto _LL3FC;_tmp5B5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5B4)->f1;_tmp5B6=_tmp5B3.f2;if(_tmp5B6 <= (void*)3  || *((int*)_tmp5B6)!= 6)
goto _LL3FC;_tmp5B7=((struct Cyc_Absyn_DoubleType_struct*)_tmp5B6)->f1;_LL3FB: if(
_tmp5B5)return t1;else{return t2;}_LL3FC: _tmp5B8=_tmp5B3.f1;if(_tmp5B8 <= (void*)3
 || *((int*)_tmp5B8)!= 6)goto _LL3FE;_LL3FD: return t1;_LL3FE: _tmp5B9=_tmp5B3.f2;
if(_tmp5B9 <= (void*)3  || *((int*)_tmp5B9)!= 6)goto _LL400;_LL3FF: return t2;_LL400:
_tmp5BA=_tmp5B3.f1;if((int)_tmp5BA != 1)goto _LL402;_LL401: goto _LL403;_LL402:
_tmp5BB=_tmp5B3.f2;if((int)_tmp5BB != 1)goto _LL404;_LL403: return(void*)1;_LL404:
_tmp5BC=_tmp5B3.f1;if(_tmp5BC <= (void*)3  || *((int*)_tmp5BC)!= 5)goto _LL406;
_tmp5BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BC)->f1;if((int)_tmp5BD != 
1)goto _LL406;_tmp5BE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BC)->f2;if((
int)_tmp5BE != 3)goto _LL406;_LL405: goto _LL407;_LL406: _tmp5BF=_tmp5B3.f2;if(
_tmp5BF <= (void*)3  || *((int*)_tmp5BF)!= 5)goto _LL408;_tmp5C0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5BF)->f1;if((int)_tmp5C0 != 1)goto _LL408;_tmp5C1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5BF)->f2;if((int)_tmp5C1 != 3)goto _LL408;_LL407: return Cyc_Absyn_ulonglong_typ;
_LL408: _tmp5C2=_tmp5B3.f1;if(_tmp5C2 <= (void*)3  || *((int*)_tmp5C2)!= 5)goto
_LL40A;_tmp5C3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C2)->f1;if((int)
_tmp5C3 != 2)goto _LL40A;_tmp5C4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C2)->f2;
if((int)_tmp5C4 != 3)goto _LL40A;_LL409: goto _LL40B;_LL40A: _tmp5C5=_tmp5B3.f2;if(
_tmp5C5 <= (void*)3  || *((int*)_tmp5C5)!= 5)goto _LL40C;_tmp5C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C5)->f1;if((int)_tmp5C6 != 2)goto _LL40C;_tmp5C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C5)->f2;if((int)_tmp5C7 != 3)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp5C8=
_tmp5B3.f1;if(_tmp5C8 <= (void*)3  || *((int*)_tmp5C8)!= 5)goto _LL40E;_tmp5C9=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C8)->f1;if((int)_tmp5C9 != 0)goto
_LL40E;_tmp5CA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C8)->f2;if((int)
_tmp5CA != 3)goto _LL40E;_LL40D: goto _LL40F;_LL40E: _tmp5CB=_tmp5B3.f2;if(_tmp5CB <= (
void*)3  || *((int*)_tmp5CB)!= 5)goto _LL410;_tmp5CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CB)->f1;if((int)_tmp5CC != 0)goto _LL410;_tmp5CD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CB)->f2;if((int)_tmp5CD != 3)goto _LL410;_LL40F: return Cyc_Absyn_slonglong_typ;
_LL410: _tmp5CE=_tmp5B3.f1;if(_tmp5CE <= (void*)3  || *((int*)_tmp5CE)!= 5)goto
_LL412;_tmp5CF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CE)->f1;if((int)
_tmp5CF != 1)goto _LL412;_tmp5D0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CE)->f2;
if((int)_tmp5D0 != 2)goto _LL412;_LL411: goto _LL413;_LL412: _tmp5D1=_tmp5B3.f2;if(
_tmp5D1 <= (void*)3  || *((int*)_tmp5D1)!= 5)goto _LL414;_tmp5D2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D1)->f1;if((int)_tmp5D2 != 1)goto _LL414;_tmp5D3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D1)->f2;if((int)_tmp5D3 != 2)goto _LL414;_LL413: return Cyc_Absyn_uint_typ;
_LL414:;_LL415: return Cyc_Absyn_sint_typ;_LL3F9:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5D4=(void*)e->r;struct Cyc_Core_Opt*_tmp5D5;
_LL417: if(*((int*)_tmp5D4)!= 4)goto _LL419;_tmp5D5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D4)->f2;if(_tmp5D5 != 0)goto _LL419;_LL418:({void*_tmp5D6[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5D7="assignment in test";_tag_arr(_tmp5D7,sizeof(char),
_get_zero_arr_size(_tmp5D7,19));}),_tag_arr(_tmp5D6,sizeof(void*),0));});goto
_LL416;_LL419:;_LL41A: goto _LL416;_LL416:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5D9=({struct _tuple6 _tmp5D8;_tmp5D8.f1=c1;_tmp5D8.f2=c2;_tmp5D8;});
void*_tmp5DA;void*_tmp5DB;void*_tmp5DC;void*_tmp5DD;void*_tmp5DE;struct Cyc_Core_Opt*
_tmp5DF;struct Cyc_Core_Opt**_tmp5E0;void*_tmp5E1;struct Cyc_Core_Opt*_tmp5E2;
struct Cyc_Core_Opt**_tmp5E3;void*_tmp5E4;struct Cyc_Core_Opt*_tmp5E5;struct Cyc_Core_Opt**
_tmp5E6;void*_tmp5E7;void*_tmp5E8;void*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*
_tmp5EC;struct Cyc_Core_Opt*_tmp5ED;struct Cyc_Core_Opt**_tmp5EE;void*_tmp5EF;void*
_tmp5F0;struct Cyc_Core_Opt*_tmp5F1;struct Cyc_Core_Opt**_tmp5F2;void*_tmp5F3;void*
_tmp5F4;struct Cyc_Core_Opt*_tmp5F5;struct Cyc_Core_Opt**_tmp5F6;void*_tmp5F7;
_LL41C: _tmp5DA=_tmp5D9.f1;if(*((int*)_tmp5DA)!= 0)goto _LL41E;_tmp5DB=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5DA)->f1;_tmp5DC=_tmp5D9.f2;if(*((int*)_tmp5DC)
!= 0)goto _LL41E;_tmp5DD=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5DC)->f1;
_LL41D: return _tmp5DB == _tmp5DD;_LL41E: _tmp5DE=_tmp5D9.f2;if(*((int*)_tmp5DE)!= 1)
goto _LL420;_tmp5DF=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DE)->f1;_tmp5E0=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5DE)->f1;_LL41F:*
_tmp5E0=({struct Cyc_Core_Opt*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->v=(
void*)c1;_tmp5F8;});return 1;_LL420: _tmp5E1=_tmp5D9.f1;if(*((int*)_tmp5E1)!= 1)
goto _LL422;_tmp5E2=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E1)->f1;_tmp5E3=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E1)->f1;_LL421:*
_tmp5E3=({struct Cyc_Core_Opt*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->v=(
void*)c2;_tmp5F9;});return 1;_LL422: _tmp5E4=_tmp5D9.f1;if(*((int*)_tmp5E4)!= 2)
goto _LL424;_tmp5E5=((struct Cyc_Absyn_Less_kb_struct*)_tmp5E4)->f1;_tmp5E6=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5E4)->f1;_tmp5E7=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E4)->f2;_tmp5E8=_tmp5D9.f2;if(*((
int*)_tmp5E8)!= 0)goto _LL424;_tmp5E9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5E8)->f1;_LL423: if(Cyc_Tcutil_kind_leq(_tmp5E9,_tmp5E7)){*_tmp5E6=({struct
Cyc_Core_Opt*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->v=(void*)c2;_tmp5FA;});
return 1;}else{return 0;}_LL424: _tmp5EA=_tmp5D9.f1;if(*((int*)_tmp5EA)!= 0)goto
_LL426;_tmp5EB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5EA)->f1;_tmp5EC=
_tmp5D9.f2;if(*((int*)_tmp5EC)!= 2)goto _LL426;_tmp5ED=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EC)->f1;_tmp5EE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EC)->f1;_tmp5EF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5EC)->f2;
_LL425: if(Cyc_Tcutil_kind_leq(_tmp5EB,_tmp5EF)){*_tmp5EE=({struct Cyc_Core_Opt*
_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->v=(void*)c1;_tmp5FB;});return 1;}
else{return 0;}_LL426: _tmp5F0=_tmp5D9.f1;if(*((int*)_tmp5F0)!= 2)goto _LL41B;
_tmp5F1=((struct Cyc_Absyn_Less_kb_struct*)_tmp5F0)->f1;_tmp5F2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F0)->f1;_tmp5F3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F0)->f2;_tmp5F4=_tmp5D9.f2;if(*((int*)_tmp5F4)!= 2)goto _LL41B;_tmp5F5=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F4)->f1;_tmp5F6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F4)->f1;_tmp5F7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F4)->f2;_LL427: if(Cyc_Tcutil_kind_leq(_tmp5F3,_tmp5F7)){*_tmp5F6=({struct
Cyc_Core_Opt*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->v=(void*)c1;_tmp5FC;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5F7,_tmp5F3)){*_tmp5F2=({struct Cyc_Core_Opt*
_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->v=(void*)c2;_tmp5FD;});return 1;}
else{return 0;}}_LL41B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp5FE=_cycalloc_atomic(sizeof(*_tmp5FE));_tmp5FE[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp5FE;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp603;_tmp603.tag=1;_tmp603.f1=(unsigned int)i;{void*
_tmp601[1]={& _tmp603};Cyc_aprintf(({const char*_tmp602="#%d";_tag_arr(_tmp602,
sizeof(char),_get_zero_arr_size(_tmp602,4));}),_tag_arr(_tmp601,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->name=({
struct _tagged_arr*_tmp600=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp600[0]=s;
_tmp600;});_tmp5FF->identity=0;_tmp5FF->kind=(void*)k;_tmp5FF;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp604=*t->name;return*((const char*)
_check_unknown_subscript(_tmp604,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp607;_tmp607.tag=0;
_tmp607.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp605[1]={&
_tmp607};Cyc_printf(({const char*_tmp606="%s";_tag_arr(_tmp606,sizeof(char),
_get_zero_arr_size(_tmp606,3));}),_tag_arr(_tmp605,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp608=Cyc_strconcat(({const char*_tmp60D="`";
_tag_arr(_tmp60D,sizeof(char),_get_zero_arr_size(_tmp60D,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp609=_tagged_arr_plus(_tmp608,
sizeof(char),1);char _tmp60A=*((char*)_check_unknown_subscript(_tmp609,sizeof(
char),0));char _tmp60B='t';if(_get_arr_size(_tmp609,sizeof(char))== 1  && (_tmp60A
== '\000'  && _tmp60B != '\000'))_throw_arraybounds();*((char*)_tmp609.curr)=
_tmp60B;});t->name=({struct _tagged_arr*_tmp60C=_cycalloc(sizeof(struct
_tagged_arr)* 1);_tmp60C[0]=(struct _tagged_arr)_tmp608;_tmp60C;});}}struct
_tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct
_tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple13*x){return({struct _tuple2*_tmp60E=
_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*
_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->v=(*x).f1;_tmp60F;});_tmp60E->f2=(*
x).f2;_tmp60E->f3=(*x).f3;_tmp60E;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp610=0;{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)
atts->hd))_tmp610=({struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));
_tmp611->hd=(void*)((void*)atts->hd);_tmp611->tl=_tmp610;_tmp611;});}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612[0]=({struct Cyc_Absyn_FnType_struct _tmp613;_tmp613.tag=8;_tmp613.f1=({
struct Cyc_Absyn_FnInfo _tmp614;_tmp614.tvars=fd->tvs;_tmp614.effect=fd->effect;
_tmp614.ret_typ=(void*)((void*)fd->ret_type);_tmp614.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp614.c_varargs=fd->c_varargs;_tmp614.cyc_varargs=fd->cyc_varargs;
_tmp614.rgn_po=fd->rgn_po;_tmp614.attributes=_tmp610;_tmp614;});_tmp613;});
_tmp612;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->f1=(*pr).f1;_tmp615->f2=
t;_tmp615;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp616=
_region_malloc(rgn,sizeof(*_tmp616));_tmp616->f1=({struct _tuple15*_tmp617=
_region_malloc(rgn,sizeof(*_tmp617));_tmp617->f1=(*y).f1;_tmp617->f2=(*y).f2;
_tmp617;});_tmp616->f2=(*y).f3;_tmp616;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){struct _tuple15*_tmp619;void*_tmp61A;struct _tuple16 _tmp618=*w;
_tmp619=_tmp618.f1;_tmp61A=_tmp618.f2;{struct Cyc_Core_Opt*_tmp61C;struct Cyc_Absyn_Tqual
_tmp61D;struct _tuple15 _tmp61B=*_tmp619;_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;
return({struct _tuple2*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->f1=_tmp61C;
_tmp61E->f2=_tmp61D;_tmp61E->f3=_tmp61A;_tmp61E;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->name=f->name;_tmp61F->tq=f->tq;
_tmp61F->type=(void*)t;_tmp61F->width=f->width;_tmp61F->attributes=f->attributes;
_tmp61F;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp620=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp621;struct Cyc_Absyn_AggrInfo _tmp622;void*_tmp623;
struct Cyc_List_List*_tmp624;struct Cyc_Absyn_TunionInfo _tmp625;void*_tmp626;
struct Cyc_List_List*_tmp627;void*_tmp628;struct Cyc_Absyn_TunionFieldInfo _tmp629;
void*_tmp62A;struct Cyc_List_List*_tmp62B;struct _tuple1*_tmp62C;struct Cyc_List_List*
_tmp62D;struct Cyc_Absyn_Typedefdecl*_tmp62E;void**_tmp62F;struct Cyc_Absyn_ArrayInfo
_tmp630;void*_tmp631;struct Cyc_Absyn_Tqual _tmp632;struct Cyc_Absyn_Exp*_tmp633;
struct Cyc_Absyn_Conref*_tmp634;struct Cyc_Absyn_PtrInfo _tmp635;void*_tmp636;
struct Cyc_Absyn_Tqual _tmp637;struct Cyc_Absyn_PtrAtts _tmp638;void*_tmp639;struct
Cyc_Absyn_Conref*_tmp63A;struct Cyc_Absyn_Conref*_tmp63B;struct Cyc_Absyn_Conref*
_tmp63C;struct Cyc_Absyn_FnInfo _tmp63D;struct Cyc_List_List*_tmp63E;struct Cyc_Core_Opt*
_tmp63F;void*_tmp640;struct Cyc_List_List*_tmp641;int _tmp642;struct Cyc_Absyn_VarargInfo*
_tmp643;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp645;struct Cyc_List_List*
_tmp646;void*_tmp647;struct Cyc_List_List*_tmp648;struct Cyc_Core_Opt*_tmp649;void*
_tmp64A;void*_tmp64B;void*_tmp64C;void*_tmp64D;void*_tmp64E;struct Cyc_List_List*
_tmp64F;_LL429: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 1)goto _LL42B;_tmp621=((
struct Cyc_Absyn_VarType_struct*)_tmp620)->f1;_LL42A: {struct _handler_cons _tmp650;
_push_handler(& _tmp650);{int _tmp652=0;if(setjmp(_tmp650.handler))_tmp652=1;if(!
_tmp652){{void*_tmp653=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp621);_npop_handler(0);return _tmp653;};_pop_handler();}else{void*_tmp651=(
void*)_exn_thrown;void*_tmp655=_tmp651;_LL45C: if(_tmp655 != Cyc_Core_Not_found)
goto _LL45E;_LL45D: return t;_LL45E:;_LL45F:(void)_throw(_tmp655);_LL45B:;}}}_LL42B:
if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 10)goto _LL42D;_tmp622=((struct Cyc_Absyn_AggrType_struct*)
_tmp620)->f1;_tmp623=(void*)_tmp622.aggr_info;_tmp624=_tmp622.targs;_LL42C: {
struct Cyc_List_List*_tmp656=Cyc_Tcutil_substs(rgn,inst,_tmp624);return _tmp656 == 
_tmp624?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp657=_cycalloc(sizeof(*
_tmp657));_tmp657[0]=({struct Cyc_Absyn_AggrType_struct _tmp658;_tmp658.tag=10;
_tmp658.f1=({struct Cyc_Absyn_AggrInfo _tmp659;_tmp659.aggr_info=(void*)_tmp623;
_tmp659.targs=_tmp656;_tmp659;});_tmp658;});_tmp657;});}_LL42D: if(_tmp620 <= (
void*)3  || *((int*)_tmp620)!= 2)goto _LL42F;_tmp625=((struct Cyc_Absyn_TunionType_struct*)
_tmp620)->f1;_tmp626=(void*)_tmp625.tunion_info;_tmp627=_tmp625.targs;_tmp628=(
void*)_tmp625.rgn;_LL42E: {struct Cyc_List_List*_tmp65A=Cyc_Tcutil_substs(rgn,
inst,_tmp627);void*_tmp65B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp628);return
_tmp65A == _tmp627  && _tmp65B == _tmp628?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_TunionType_struct
_tmp65D;_tmp65D.tag=2;_tmp65D.f1=({struct Cyc_Absyn_TunionInfo _tmp65E;_tmp65E.tunion_info=(
void*)_tmp626;_tmp65E.targs=_tmp65A;_tmp65E.rgn=(void*)_tmp65B;_tmp65E;});
_tmp65D;});_tmp65C;});}_LL42F: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 3)goto
_LL431;_tmp629=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp620)->f1;_tmp62A=(
void*)_tmp629.field_info;_tmp62B=_tmp629.targs;_LL430: {struct Cyc_List_List*
_tmp65F=Cyc_Tcutil_substs(rgn,inst,_tmp62B);return _tmp65F == _tmp62B?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp660=_cycalloc(sizeof(*_tmp660));
_tmp660[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp661;_tmp661.tag=3;_tmp661.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp662;_tmp662.field_info=(void*)_tmp62A;_tmp662.targs=
_tmp65F;_tmp662;});_tmp661;});_tmp660;});}_LL431: if(_tmp620 <= (void*)3  || *((int*)
_tmp620)!= 16)goto _LL433;_tmp62C=((struct Cyc_Absyn_TypedefType_struct*)_tmp620)->f1;
_tmp62D=((struct Cyc_Absyn_TypedefType_struct*)_tmp620)->f2;_tmp62E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp620)->f3;_tmp62F=((struct Cyc_Absyn_TypedefType_struct*)_tmp620)->f4;_LL432: {
struct Cyc_List_List*_tmp663=Cyc_Tcutil_substs(rgn,inst,_tmp62D);return _tmp663 == 
_tmp62D?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp664=_cycalloc(sizeof(*
_tmp664));_tmp664[0]=({struct Cyc_Absyn_TypedefType_struct _tmp665;_tmp665.tag=16;
_tmp665.f1=_tmp62C;_tmp665.f2=_tmp663;_tmp665.f3=_tmp62E;_tmp665.f4=_tmp62F;
_tmp665;});_tmp664;});}_LL433: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 7)goto
_LL435;_tmp630=((struct Cyc_Absyn_ArrayType_struct*)_tmp620)->f1;_tmp631=(void*)
_tmp630.elt_type;_tmp632=_tmp630.tq;_tmp633=_tmp630.num_elts;_tmp634=_tmp630.zero_term;
_LL434: {void*_tmp666=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp631);return _tmp666 == 
_tmp631?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp667=_cycalloc(sizeof(*
_tmp667));_tmp667[0]=({struct Cyc_Absyn_ArrayType_struct _tmp668;_tmp668.tag=7;
_tmp668.f1=({struct Cyc_Absyn_ArrayInfo _tmp669;_tmp669.elt_type=(void*)_tmp666;
_tmp669.tq=_tmp632;_tmp669.num_elts=_tmp633;_tmp669.zero_term=_tmp634;_tmp669;});
_tmp668;});_tmp667;});}_LL435: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 4)goto
_LL437;_tmp635=((struct Cyc_Absyn_PointerType_struct*)_tmp620)->f1;_tmp636=(void*)
_tmp635.elt_typ;_tmp637=_tmp635.elt_tq;_tmp638=_tmp635.ptr_atts;_tmp639=(void*)
_tmp638.rgn;_tmp63A=_tmp638.nullable;_tmp63B=_tmp638.bounds;_tmp63C=_tmp638.zero_term;
_LL436: {void*_tmp66A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp636);void*_tmp66B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp639);struct Cyc_Absyn_Conref*_tmp66C=_tmp63B;{void*_tmp66D=(void*)(
Cyc_Absyn_compress_conref(_tmp63B))->v;void*_tmp66E;void*_tmp66F;_LL461: if(
_tmp66D <= (void*)1  || *((int*)_tmp66D)!= 0)goto _LL463;_tmp66E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp66D)->f1;if(_tmp66E <= (void*)1  || *((int*)_tmp66E)!= 1)goto _LL463;_tmp66F=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp66E)->f1;_LL462: {void*_tmp670=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp66F);if(_tmp66F != _tmp670)_tmp66C=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp671=_cycalloc(sizeof(*_tmp671));
_tmp671[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp672;_tmp672.tag=1;_tmp672.f1=(
void*)_tmp670;_tmp672;});_tmp671;}));goto _LL460;}_LL463:;_LL464: goto _LL460;
_LL460:;}if((_tmp66A == _tmp636  && _tmp66B == _tmp639) && _tmp66C == _tmp63B)return
t;return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp673=_cycalloc(sizeof(*
_tmp673));_tmp673[0]=({struct Cyc_Absyn_PointerType_struct _tmp674;_tmp674.tag=4;
_tmp674.f1=({struct Cyc_Absyn_PtrInfo _tmp675;_tmp675.elt_typ=(void*)_tmp66A;
_tmp675.elt_tq=_tmp637;_tmp675.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp676;_tmp676.rgn=(
void*)_tmp66B;_tmp676.nullable=_tmp63A;_tmp676.bounds=_tmp66C;_tmp676.zero_term=
_tmp63C;_tmp676;});_tmp675;});_tmp674;});_tmp673;});}_LL437: if(_tmp620 <= (void*)
3  || *((int*)_tmp620)!= 8)goto _LL439;_tmp63D=((struct Cyc_Absyn_FnType_struct*)
_tmp620)->f1;_tmp63E=_tmp63D.tvars;_tmp63F=_tmp63D.effect;_tmp640=(void*)_tmp63D.ret_typ;
_tmp641=_tmp63D.args;_tmp642=_tmp63D.c_varargs;_tmp643=_tmp63D.cyc_varargs;
_tmp644=_tmp63D.rgn_po;_tmp645=_tmp63D.attributes;_LL438:{struct Cyc_List_List*
_tmp677=_tmp63E;for(0;_tmp677 != 0;_tmp677=_tmp677->tl){inst=({struct Cyc_List_List*
_tmp678=_region_malloc(rgn,sizeof(*_tmp678));_tmp678->hd=({struct _tuple8*_tmp679=
_region_malloc(rgn,sizeof(*_tmp679));_tmp679->f1=(struct Cyc_Absyn_Tvar*)_tmp677->hd;
_tmp679->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp67A=_cycalloc(sizeof(*
_tmp67A));_tmp67A[0]=({struct Cyc_Absyn_VarType_struct _tmp67B;_tmp67B.tag=1;
_tmp67B.f1=(struct Cyc_Absyn_Tvar*)_tmp677->hd;_tmp67B;});_tmp67A;});_tmp679;});
_tmp678->tl=inst;_tmp678;});}}{struct Cyc_List_List*_tmp67D;struct Cyc_List_List*
_tmp67E;struct _tuple0 _tmp67C=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp641));_tmp67D=_tmp67C.f1;_tmp67E=_tmp67C.f2;{struct Cyc_List_List*_tmp67F=
Cyc_Tcutil_substs(rgn,inst,_tmp67E);struct Cyc_List_List*_tmp680=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp67D,_tmp67F));struct Cyc_Core_Opt*
eff2;if(_tmp63F == 0)eff2=0;else{void*_tmp681=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp63F->v);if(_tmp681 == (void*)_tmp63F->v)eff2=_tmp63F;else{eff2=({struct
Cyc_Core_Opt*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->v=(void*)_tmp681;
_tmp682;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp643 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp684;struct Cyc_Absyn_Tqual _tmp685;void*
_tmp686;int _tmp687;struct Cyc_Absyn_VarargInfo _tmp683=*_tmp643;_tmp684=_tmp683.name;
_tmp685=_tmp683.tq;_tmp686=(void*)_tmp683.type;_tmp687=_tmp683.inject;{void*
_tmp688=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp686);if(_tmp688 == _tmp686)
cyc_varargs2=_tmp643;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp689=
_cycalloc(sizeof(*_tmp689));_tmp689->name=_tmp684;_tmp689->tq=_tmp685;_tmp689->type=(
void*)_tmp688;_tmp689->inject=_tmp687;_tmp689;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp68B;struct Cyc_List_List*_tmp68C;struct _tuple0 _tmp68A=Cyc_List_rsplit(
rgn,rgn,_tmp644);_tmp68B=_tmp68A.f1;_tmp68C=_tmp68A.f2;{struct Cyc_List_List*
_tmp68D=Cyc_Tcutil_substs(rgn,inst,_tmp68B);struct Cyc_List_List*_tmp68E=Cyc_Tcutil_substs(
rgn,inst,_tmp68C);if(_tmp68D == _tmp68B  && _tmp68E == _tmp68C)rgn_po2=_tmp644;
else{rgn_po2=Cyc_List_zip(_tmp68D,_tmp68E);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_FnType_struct
_tmp690;_tmp690.tag=8;_tmp690.f1=({struct Cyc_Absyn_FnInfo _tmp691;_tmp691.tvars=
_tmp63E;_tmp691.effect=eff2;_tmp691.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp640);_tmp691.args=_tmp680;_tmp691.c_varargs=_tmp642;_tmp691.cyc_varargs=
cyc_varargs2;_tmp691.rgn_po=rgn_po2;_tmp691.attributes=_tmp645;_tmp691;});
_tmp690;});_tmp68F;});}}}}}_LL439: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 9)
goto _LL43B;_tmp646=((struct Cyc_Absyn_TupleType_struct*)_tmp620)->f1;_LL43A: {
struct Cyc_List_List*_tmp692=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp646);struct Cyc_List_List*_tmp693=Cyc_Tcutil_substs(rgn,inst,_tmp692);if(
_tmp693 == _tmp692)return t;{struct Cyc_List_List*_tmp694=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp646,_tmp693);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_TupleType_struct
_tmp696;_tmp696.tag=9;_tmp696.f1=_tmp694;_tmp696;});_tmp695;});}}_LL43B: if(
_tmp620 <= (void*)3  || *((int*)_tmp620)!= 11)goto _LL43D;_tmp647=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp620)->f1;_tmp648=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp620)->f2;_LL43C: {struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp648);struct Cyc_List_List*_tmp698=
Cyc_Tcutil_substs(rgn,inst,_tmp697);if(_tmp698 == _tmp697)return t;{struct Cyc_List_List*
_tmp699=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmp648,_tmp698);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp69A=
_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp69B;_tmp69B.tag=11;_tmp69B.f1=(void*)_tmp647;_tmp69B.f2=_tmp699;_tmp69B;});
_tmp69A;});}}_LL43D: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 0)goto _LL43F;
_tmp649=((struct Cyc_Absyn_Evar_struct*)_tmp620)->f2;_LL43E: if(_tmp649 != 0)return
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp649->v);else{return t;}_LL43F: if(
_tmp620 <= (void*)3  || *((int*)_tmp620)!= 15)goto _LL441;_tmp64A=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp620)->f1;_LL440: {void*_tmp69C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64A);return _tmp69C == _tmp64A?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp69E;_tmp69E.tag=15;_tmp69E.f1=(void*)_tmp69C;_tmp69E;});_tmp69D;});}_LL441:
if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 14)goto _LL443;_tmp64B=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp620)->f1;_LL442: {void*_tmp69F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64B);return _tmp69F == _tmp64B?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp6A1;_tmp6A1.tag=14;_tmp6A1.f1=(void*)_tmp69F;_tmp6A1;});_tmp6A0;});}_LL443:
if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 17)goto _LL445;_tmp64C=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp620)->f1;_LL444: {void*_tmp6A2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64C);return _tmp6A2 == _tmp64C?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_TagType_struct
_tmp6A4;_tmp6A4.tag=17;_tmp6A4.f1=(void*)_tmp6A2;_tmp6A4;});_tmp6A3;});}_LL445:
if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 18)goto _LL447;_LL446: goto _LL448;
_LL447: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 12)goto _LL449;_LL448: goto
_LL44A;_LL449: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 13)goto _LL44B;_LL44A:
goto _LL44C;_LL44B: if((int)_tmp620 != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if(
_tmp620 <= (void*)3  || *((int*)_tmp620)!= 5)goto _LL44F;_LL44E: goto _LL450;_LL44F:
if((int)_tmp620 != 1)goto _LL451;_LL450: goto _LL452;_LL451: if(_tmp620 <= (void*)3
 || *((int*)_tmp620)!= 6)goto _LL453;_LL452: goto _LL454;_LL453: if((int)_tmp620 != 2)
goto _LL455;_LL454: return t;_LL455: if(_tmp620 <= (void*)3  || *((int*)_tmp620)!= 21)
goto _LL457;_tmp64D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp620)->f1;_LL456: {
void*_tmp6A5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64D);return _tmp6A5 == _tmp64D?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));
_tmp6A6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6A7;_tmp6A7.tag=21;_tmp6A7.f1=(
void*)_tmp6A5;_tmp6A7;});_tmp6A6;});}_LL457: if(_tmp620 <= (void*)3  || *((int*)
_tmp620)!= 19)goto _LL459;_tmp64E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp620)->f1;_LL458: {void*_tmp6A8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64E);
return _tmp6A8 == _tmp64E?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6A9=
_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AA;
_tmp6AA.tag=19;_tmp6AA.f1=(void*)_tmp6A8;_tmp6AA;});_tmp6A9;});}_LL459: if(
_tmp620 <= (void*)3  || *((int*)_tmp620)!= 20)goto _LL428;_tmp64F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp620)->f1;_LL45A: {struct Cyc_List_List*_tmp6AB=Cyc_Tcutil_substs(rgn,inst,
_tmp64F);return _tmp6AB == _tmp64F?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6AD;_tmp6AD.tag=20;_tmp6AD.f1=_tmp6AB;_tmp6AD;});_tmp6AC;});}_LL428:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp6AE=(void*)ts->hd;
struct Cyc_List_List*_tmp6AF=ts->tl;void*_tmp6B0=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp6AE);struct Cyc_List_List*_tmp6B1=Cyc_Tcutil_substs(rgn,inst,_tmp6AF);if(
_tmp6AE == _tmp6B0  && _tmp6AF == _tmp6B1)return ts;return(struct Cyc_List_List*)((
struct Cyc_List_List*)({struct Cyc_List_List*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));
_tmp6B2->hd=(void*)_tmp6B0;_tmp6B2->tl=_tmp6B1;_tmp6B2;}));}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6B3=_cycalloc(
sizeof(*_tmp6B3));_tmp6B3->f1=tv;_tmp6B3->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4->v=(void*)k;_tmp6B4;}),({struct Cyc_Core_Opt*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->v=s;_tmp6B5;}));_tmp6B3;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp6B7;struct Cyc_List_List*_tmp6B8;struct _RegionHandle*_tmp6B9;
struct _tuple9*_tmp6B6=env;_tmp6B7=*_tmp6B6;_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6BA=_region_malloc(
_tmp6B9,sizeof(*_tmp6BA));_tmp6BA->f1=tv;_tmp6BA->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->v=(void*)k;_tmp6BB;}),({
struct Cyc_Core_Opt*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->v=_tmp6B8;
_tmp6BC;}));_tmp6BA;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp6C0;_tmp6C0.tag=0;_tmp6C0.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp6BF;_tmp6BF.tag=0;_tmp6BF.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp6BD[3]={& _tmp6BF,& _tmp6C0,&
_tmp6C1};Cyc_Tcutil_terr(loc,({const char*_tmp6BE="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp6BE,sizeof(char),_get_zero_arr_size(_tmp6BE,59));}),_tag_arr(
_tmp6BD,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp6C2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C3="same type variable has different identity!";_tag_arr(_tmp6C3,sizeof(
char),_get_zero_arr_size(_tmp6C3,43));}),_tag_arr(_tmp6C2,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->hd=tv;_tmp6C4->tl=tvs;_tmp6C4;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6C5[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp6C6="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp6C6,sizeof(char),_get_zero_arr_size(_tmp6C6,39));}),_tag_arr(
_tmp6C5,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6C7[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C8="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp6C8,sizeof(char),
_get_zero_arr_size(_tmp6C8,41));}),_tag_arr(_tmp6C7,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp6C9=
_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=tv;_tmp6C9->tl=tvs;_tmp6C9;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp6CC;_tmp6CC.tag=0;
_tmp6CC.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp6CA[1]={& _tmp6CC};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp6CB="bound tvar id for %s is NULL";_tag_arr(
_tmp6CB,sizeof(char),_get_zero_arr_size(_tmp6CB,29));}),_tag_arr(_tmp6CA,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));
_tmp6CD->hd=tv;_tmp6CD->tl=tvs;_tmp6CD;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp6CE=Cyc_Tcutil_compress(e);int _tmp6CF;
_LL466: if(_tmp6CE <= (void*)3  || *((int*)_tmp6CE)!= 0)goto _LL468;_tmp6CF=((struct
Cyc_Absyn_Evar_struct*)_tmp6CE)->f3;_LL467:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp6D0=Cyc_Tcutil_compress((void*)es2->hd);int _tmp6D1;
_LL46B: if(_tmp6D0 <= (void*)3  || *((int*)_tmp6D0)!= 0)goto _LL46D;_tmp6D1=((struct
Cyc_Absyn_Evar_struct*)_tmp6D0)->f3;_LL46C: if(_tmp6CF == _tmp6D1)return es;goto
_LL46A;_LL46D:;_LL46E: goto _LL46A;_LL46A:;}}return({struct Cyc_List_List*_tmp6D2=
_cycalloc(sizeof(*_tmp6D2));_tmp6D2->hd=(void*)e;_tmp6D2->tl=es;_tmp6D2;});
_LL468:;_LL469: return es;_LL465:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6D3->tl=r;_tmp6D3;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp6D6;_tmp6D6.tag=
0;_tmp6D6.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*_tmp6D4[1]={&
_tmp6D6};Cyc_Tcutil_terr(loc,({const char*_tmp6D5="bitfield %s does not have constant width";
_tag_arr(_tmp6D5,sizeof(char),_get_zero_arr_size(_tmp6D5,41));}),_tag_arr(
_tmp6D4,sizeof(void*),1));}});else{unsigned int _tmp6D8;int _tmp6D9;struct _tuple7
_tmp6D7=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmp6D8=
_tmp6D7.f1;_tmp6D9=_tmp6D7.f2;if(!_tmp6D9)({void*_tmp6DA[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6DB="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp6DB,sizeof(char),_get_zero_arr_size(_tmp6DB,45));}),_tag_arr(_tmp6DA,sizeof(
void*),0));});w=_tmp6D8;}{void*_tmp6DC=Cyc_Tcutil_compress(field_typ);void*
_tmp6DD;_LL470: if(_tmp6DC <= (void*)3  || *((int*)_tmp6DC)!= 5)goto _LL472;_tmp6DD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6DC)->f2;_LL471:{void*_tmp6DE=
_tmp6DD;_LL475: if((int)_tmp6DE != 0)goto _LL477;_LL476: if(w > 8)({void*_tmp6DF[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6E0="bitfield larger than type";_tag_arr(
_tmp6E0,sizeof(char),_get_zero_arr_size(_tmp6E0,26));}),_tag_arr(_tmp6DF,sizeof(
void*),0));});goto _LL474;_LL477: if((int)_tmp6DE != 1)goto _LL479;_LL478: if(w > 16)({
void*_tmp6E1[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E2="bitfield larger than type";
_tag_arr(_tmp6E2,sizeof(char),_get_zero_arr_size(_tmp6E2,26));}),_tag_arr(
_tmp6E1,sizeof(void*),0));});goto _LL474;_LL479: if((int)_tmp6DE != 2)goto _LL47B;
_LL47A: if(w > 32)({void*_tmp6E3[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E4="bitfield larger than type";
_tag_arr(_tmp6E4,sizeof(char),_get_zero_arr_size(_tmp6E4,26));}),_tag_arr(
_tmp6E3,sizeof(void*),0));});goto _LL474;_LL47B: if((int)_tmp6DE != 3)goto _LL474;
_LL47C: if(w > 64)({void*_tmp6E5[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E6="bitfield larger than type";
_tag_arr(_tmp6E6,sizeof(char),_get_zero_arr_size(_tmp6E6,26));}),_tag_arr(
_tmp6E5,sizeof(void*),0));});goto _LL474;_LL474:;}goto _LL46F;_LL472:;_LL473:({
struct Cyc_String_pa_struct _tmp6EA;_tmp6EA.tag=0;_tmp6EA.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp6E9;_tmp6E9.tag=0;_tmp6E9.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp6E7[2]={& _tmp6E9,& _tmp6EA};Cyc_Tcutil_terr(loc,({const char*_tmp6E8="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp6E8,sizeof(char),_get_zero_arr_size(_tmp6E8,52));}),_tag_arr(
_tmp6E7,sizeof(void*),2));}}});goto _LL46F;_LL46F:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp6EB=(void*)atts->hd;_LL47E: if((int)
_tmp6EB != 5)goto _LL480;_LL47F: continue;_LL480: if(_tmp6EB <= (void*)17  || *((int*)
_tmp6EB)!= 1)goto _LL482;_LL481: continue;_LL482:;_LL483:({struct Cyc_String_pa_struct
_tmp6EF;_tmp6EF.tag=0;_tmp6EF.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp6EE;_tmp6EE.tag=0;_tmp6EE.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp6EC[2]={&
_tmp6EE,& _tmp6EF};Cyc_Tcutil_terr(loc,({const char*_tmp6ED="bad attribute %s on member %s";
_tag_arr(_tmp6ED,sizeof(char),_get_zero_arr_size(_tmp6ED,30));}),_tag_arr(
_tmp6EC,sizeof(void*),2));}}});_LL47D:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp6F0=t;struct Cyc_Absyn_Typedefdecl*
_tmp6F1;void**_tmp6F2;_LL485: if(_tmp6F0 <= (void*)3  || *((int*)_tmp6F0)!= 16)goto
_LL487;_tmp6F1=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F0)->f3;_tmp6F2=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F0)->f4;_LL486: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp6F1))->tq).real_const  || (_tmp6F1->tq).print_const){if(
declared_const)({void*_tmp6F3[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp6F4="extra const";
_tag_arr(_tmp6F4,sizeof(char),_get_zero_arr_size(_tmp6F4,12));}),_tag_arr(
_tmp6F3,sizeof(void*),0));});return 1;}if((unsigned int)_tmp6F2)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp6F2);else{return declared_const;}_LL487:;_LL488: return
declared_const;_LL484:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp6F5=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp6F6;struct Cyc_Core_Opt**_tmp6F7;struct Cyc_Core_Opt*
_tmp6F8;struct Cyc_Core_Opt**_tmp6F9;struct Cyc_Absyn_Tvar*_tmp6FA;struct Cyc_List_List*
_tmp6FB;struct _tuple1*_tmp6FC;struct Cyc_Absyn_Enumdecl*_tmp6FD;struct Cyc_Absyn_Enumdecl**
_tmp6FE;struct Cyc_Absyn_TunionInfo _tmp6FF;void*_tmp700;void**_tmp701;struct Cyc_List_List*
_tmp702;struct Cyc_List_List**_tmp703;void*_tmp704;struct Cyc_Absyn_TunionFieldInfo
_tmp705;void*_tmp706;void**_tmp707;struct Cyc_List_List*_tmp708;struct Cyc_Absyn_PtrInfo
_tmp709;void*_tmp70A;struct Cyc_Absyn_Tqual _tmp70B;struct Cyc_Absyn_Tqual*_tmp70C;
struct Cyc_Absyn_PtrAtts _tmp70D;void*_tmp70E;struct Cyc_Absyn_Conref*_tmp70F;
struct Cyc_Absyn_Conref*_tmp710;struct Cyc_Absyn_Conref*_tmp711;void*_tmp712;void*
_tmp713;struct Cyc_Absyn_ArrayInfo _tmp714;void*_tmp715;struct Cyc_Absyn_Tqual
_tmp716;struct Cyc_Absyn_Tqual*_tmp717;struct Cyc_Absyn_Exp*_tmp718;struct Cyc_Absyn_Exp**
_tmp719;struct Cyc_Absyn_Conref*_tmp71A;struct Cyc_Absyn_FnInfo _tmp71B;struct Cyc_List_List*
_tmp71C;struct Cyc_List_List**_tmp71D;struct Cyc_Core_Opt*_tmp71E;struct Cyc_Core_Opt**
_tmp71F;void*_tmp720;struct Cyc_List_List*_tmp721;int _tmp722;struct Cyc_Absyn_VarargInfo*
_tmp723;struct Cyc_List_List*_tmp724;struct Cyc_List_List*_tmp725;struct Cyc_List_List*
_tmp726;void*_tmp727;struct Cyc_List_List*_tmp728;struct Cyc_Absyn_AggrInfo _tmp729;
void*_tmp72A;void**_tmp72B;struct Cyc_List_List*_tmp72C;struct Cyc_List_List**
_tmp72D;struct _tuple1*_tmp72E;struct Cyc_List_List*_tmp72F;struct Cyc_List_List**
_tmp730;struct Cyc_Absyn_Typedefdecl*_tmp731;struct Cyc_Absyn_Typedefdecl**_tmp732;
void**_tmp733;void***_tmp734;void*_tmp735;void*_tmp736;void*_tmp737;struct Cyc_List_List*
_tmp738;_LL48A: if((int)_tmp6F5 != 0)goto _LL48C;_LL48B: goto _LL489;_LL48C: if(
_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 0)goto _LL48E;_tmp6F6=((struct Cyc_Absyn_Evar_struct*)
_tmp6F5)->f1;_tmp6F7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp6F5)->f1;_tmp6F8=((struct Cyc_Absyn_Evar_struct*)_tmp6F5)->f2;_tmp6F9=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6F5)->f2;_LL48D: if(*_tmp6F7 == 
0)*_tmp6F7=({struct Cyc_Core_Opt*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->v=(
void*)expected_kind;_tmp739;});if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && expected_kind == (void*)3)*_tmp6F9=({struct Cyc_Core_Opt*_tmp73A=_cycalloc(
sizeof(*_tmp73A));_tmp73A->v=(void*)((void*)2);_tmp73A;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp73B=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp73F=_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp740;_tmp740.tag=2;_tmp740.f1=0;_tmp740.f2=(void*)expected_kind;_tmp740;});
_tmp73F;}));*_tmp6F9=({struct Cyc_Core_Opt*_tmp73C=_cycalloc(sizeof(*_tmp73C));
_tmp73C->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D[0]=({struct Cyc_Absyn_VarType_struct _tmp73E;_tmp73E.tag=
1;_tmp73E.f1=_tmp73B;_tmp73E;});_tmp73D;}));_tmp73C;});_tmp6FA=_tmp73B;goto
_LL48F;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL489;_LL48E: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 1)goto _LL490;
_tmp6FA=((struct Cyc_Absyn_VarType_struct*)_tmp6F5)->f1;_LL48F:{void*_tmp741=Cyc_Absyn_compress_kb((
void*)_tmp6FA->kind);struct Cyc_Core_Opt*_tmp742;struct Cyc_Core_Opt**_tmp743;
_LL4BD: if(*((int*)_tmp741)!= 1)goto _LL4BF;_tmp742=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp741)->f1;_tmp743=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp741)->f1;_LL4BE:*_tmp743=({struct Cyc_Core_Opt*_tmp744=_cycalloc(sizeof(*
_tmp744));_tmp744->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_Less_kb_struct _tmp746;
_tmp746.tag=2;_tmp746.f1=0;_tmp746.f2=(void*)expected_kind;_tmp746;});_tmp745;}));
_tmp744;});goto _LL4BC;_LL4BF:;_LL4C0: goto _LL4BC;_LL4BC:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp6FA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6FA);goto _LL489;_LL490: if(_tmp6F5 <= (void*)3  || *((int*)
_tmp6F5)!= 13)goto _LL492;_tmp6FB=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6F5)->f1;
_LL491: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp747=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp747;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp6FB != 0;_tmp6FB=_tmp6FB->tl){struct Cyc_Absyn_Enumfield*_tmp748=(
struct Cyc_Absyn_Enumfield*)_tmp6FB->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp748->name).f2))({struct Cyc_String_pa_struct
_tmp74B;_tmp74B.tag=0;_tmp74B.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp748->name).f2);{void*_tmp749[1]={& _tmp74B};Cyc_Tcutil_terr(_tmp748->loc,({
const char*_tmp74A="duplicate enum field name %s";_tag_arr(_tmp74A,sizeof(char),
_get_zero_arr_size(_tmp74A,29));}),_tag_arr(_tmp749,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp74C=_region_malloc(uprev_rgn,sizeof(*
_tmp74C));_tmp74C->hd=(*_tmp748->name).f2;_tmp74C->tl=prev_fields;_tmp74C;});}
if(_tmp748->tag == 0)_tmp748->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp748->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp748->tag)))({struct Cyc_String_pa_struct _tmp74F;_tmp74F.tag=0;
_tmp74F.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp748->name).f2);{void*
_tmp74D[1]={& _tmp74F};Cyc_Tcutil_terr(loc,({const char*_tmp74E="enum field %s: expression is not constant";
_tag_arr(_tmp74E,sizeof(char),_get_zero_arr_size(_tmp74E,42));}),_tag_arr(
_tmp74D,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp748->tag))).f1;tag_count=t1 + 1;(*_tmp748->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp751;_tmp751.tag=1;_tmp751.f1=te->ns;_tmp751;});
_tmp750;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp748->name).f2,({
struct _tuple17*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp754;_tmp754.tag=4;_tmp754.f1=(void*)t;
_tmp754.f2=_tmp748;_tmp754;});_tmp753;});_tmp752->f2=1;_tmp752;}));}}};
_pop_region(uprev_rgn);}goto _LL489;}_LL492: if(_tmp6F5 <= (void*)3  || *((int*)
_tmp6F5)!= 12)goto _LL494;_tmp6FC=((struct Cyc_Absyn_EnumType_struct*)_tmp6F5)->f1;
_tmp6FD=((struct Cyc_Absyn_EnumType_struct*)_tmp6F5)->f2;_tmp6FE=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6F5)->f2;_LL493: if(*_tmp6FE == 0  || ((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp6FE))->fields == 0){struct _handler_cons
_tmp755;_push_handler(& _tmp755);{int _tmp757=0;if(setjmp(_tmp755.handler))_tmp757=
1;if(!_tmp757){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp6FC);*_tmp6FE=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp756=(void*)_exn_thrown;void*_tmp759=_tmp756;_LL4C2: if(_tmp759 != Cyc_Dict_Absent)
goto _LL4C4;_LL4C3: {struct Cyc_Tcenv_Genv*_tmp75A=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp75B=({struct Cyc_Absyn_Enumdecl*_tmp75C=_cycalloc(
sizeof(*_tmp75C));_tmp75C->sc=(void*)((void*)3);_tmp75C->name=_tmp6FC;_tmp75C->fields=
0;_tmp75C;});Cyc_Tc_tcEnumdecl(te,_tmp75A,loc,_tmp75B);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6FC);*_tmp6FE=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4C1;}}_LL4C4:;_LL4C5:(void)_throw(_tmp759);_LL4C1:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6FE);*_tmp6FC=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL489;}_LL494: if(_tmp6F5 <= (void*)3
 || *((int*)_tmp6F5)!= 2)goto _LL496;_tmp6FF=((struct Cyc_Absyn_TunionType_struct*)
_tmp6F5)->f1;_tmp700=(void*)_tmp6FF.tunion_info;_tmp701=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6F5)->f1).tunion_info;_tmp702=_tmp6FF.targs;_tmp703=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6F5)->f1).targs;_tmp704=(void*)_tmp6FF.rgn;
_LL495: {struct Cyc_List_List*_tmp75D=*_tmp703;{void*_tmp75E=*_tmp701;struct Cyc_Absyn_UnknownTunionInfo
_tmp75F;struct _tuple1*_tmp760;int _tmp761;struct Cyc_Absyn_Tuniondecl**_tmp762;
struct Cyc_Absyn_Tuniondecl*_tmp763;_LL4C7: if(*((int*)_tmp75E)!= 0)goto _LL4C9;
_tmp75F=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp75E)->f1;_tmp760=_tmp75F.name;
_tmp761=_tmp75F.is_xtunion;_LL4C8: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp764;_push_handler(& _tmp764);{int _tmp766=0;if(setjmp(_tmp764.handler))
_tmp766=1;if(!_tmp766){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp760);;
_pop_handler();}else{void*_tmp765=(void*)_exn_thrown;void*_tmp768=_tmp765;_LL4CC:
if(_tmp768 != Cyc_Dict_Absent)goto _LL4CE;_LL4CD: {struct Cyc_Tcenv_Genv*_tmp769=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp76A=({struct Cyc_Absyn_Tuniondecl*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->sc=(void*)((void*)3);_tmp771->name=
_tmp760;_tmp771->tvs=0;_tmp771->fields=0;_tmp771->is_xtunion=_tmp761;_tmp771;});
Cyc_Tc_tcTuniondecl(te,_tmp769,loc,_tmp76A);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp760);if(_tmp75D != 0){({struct Cyc_String_pa_struct _tmp76E;_tmp76E.tag=0;
_tmp76E.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp760));{struct Cyc_String_pa_struct _tmp76D;_tmp76D.tag=0;_tmp76D.f1=(struct
_tagged_arr)(_tmp761?(struct _tagged_arr)((struct _tagged_arr)({const char*_tmp76F="xtunion";
_tag_arr(_tmp76F,sizeof(char),_get_zero_arr_size(_tmp76F,8));})):(struct
_tagged_arr)({const char*_tmp770="tunion";_tag_arr(_tmp770,sizeof(char),
_get_zero_arr_size(_tmp770,7));}));{void*_tmp76B[2]={& _tmp76D,& _tmp76E};Cyc_Tcutil_terr(
loc,({const char*_tmp76C="declare parameterized %s %s before using";_tag_arr(
_tmp76C,sizeof(char),_get_zero_arr_size(_tmp76C,41));}),_tag_arr(_tmp76B,sizeof(
void*),2));}}});return cvtenv;}goto _LL4CB;}_LL4CE:;_LL4CF:(void)_throw(_tmp768);
_LL4CB:;}}}if((*tudp)->is_xtunion != _tmp761)({struct Cyc_String_pa_struct _tmp774;
_tmp774.tag=0;_tmp774.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp760));{void*_tmp772[1]={& _tmp774};Cyc_Tcutil_terr(loc,({const char*_tmp773="[x]tunion is different from type declaration %s";
_tag_arr(_tmp773,sizeof(char),_get_zero_arr_size(_tmp773,48));}),_tag_arr(
_tmp772,sizeof(void*),1));}});*_tmp701=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp776;_tmp776.tag=1;_tmp776.f1=tudp;_tmp776;});_tmp775;});_tmp763=*tudp;goto
_LL4CA;}_LL4C9: if(*((int*)_tmp75E)!= 1)goto _LL4C6;_tmp762=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp75E)->f1;_tmp763=*_tmp762;_LL4CA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp704);{struct Cyc_List_List*tvs=_tmp763->tvs;for(0;_tmp75D != 0
 && tvs != 0;(_tmp75D=_tmp75D->tl,tvs=tvs->tl)){void*t1=(void*)_tmp75D->hd;void*
k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp75D != 0)({struct Cyc_String_pa_struct _tmp779;_tmp779.tag=
0;_tmp779.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp763->name));{void*_tmp777[1]={& _tmp779};Cyc_Tcutil_terr(loc,({const char*
_tmp778="too many type arguments for tunion %s";_tag_arr(_tmp778,sizeof(char),
_get_zero_arr_size(_tmp778,38));}),_tag_arr(_tmp777,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A->hd=(void*)e;_tmp77A->tl=
hidden_ts;_tmp77A;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp703=Cyc_List_imp_append(*_tmp703,Cyc_List_imp_rev(hidden_ts));}goto _LL4C6;}
_LL4C6:;}goto _LL489;}_LL496: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 3)goto
_LL498;_tmp705=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6F5)->f1;_tmp706=(
void*)_tmp705.field_info;_tmp707=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6F5)->f1).field_info;_tmp708=_tmp705.targs;_LL497:{void*_tmp77B=*_tmp707;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp77C;struct _tuple1*_tmp77D;struct
_tuple1*_tmp77E;int _tmp77F;struct Cyc_Absyn_Tuniondecl*_tmp780;struct Cyc_Absyn_Tunionfield*
_tmp781;_LL4D1: if(*((int*)_tmp77B)!= 0)goto _LL4D3;_tmp77C=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp77B)->f1;_tmp77D=_tmp77C.tunion_name;_tmp77E=_tmp77C.field_name;_tmp77F=
_tmp77C.is_xtunion;_LL4D2: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp782;_push_handler(& _tmp782);{int _tmp784=0;if(setjmp(
_tmp782.handler))_tmp784=1;if(!_tmp784){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp77D);;_pop_handler();}else{void*_tmp783=(void*)_exn_thrown;void*_tmp786=
_tmp783;_LL4D6: if(_tmp786 != Cyc_Dict_Absent)goto _LL4D8;_LL4D7:({struct Cyc_String_pa_struct
_tmp789;_tmp789.tag=0;_tmp789.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp77D));{void*_tmp787[1]={& _tmp789};Cyc_Tcutil_terr(loc,({const char*_tmp788="unbound type tunion %s";
_tag_arr(_tmp788,sizeof(char),_get_zero_arr_size(_tmp788,23));}),_tag_arr(
_tmp787,sizeof(void*),1));}});return cvtenv;_LL4D8:;_LL4D9:(void)_throw(_tmp786);
_LL4D5:;}}}{struct _handler_cons _tmp78A;_push_handler(& _tmp78A);{int _tmp78C=0;if(
setjmp(_tmp78A.handler))_tmp78C=1;if(!_tmp78C){{void*_tmp78D=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp77E);struct Cyc_Absyn_Tuniondecl*_tmp78E;struct Cyc_Absyn_Tunionfield*
_tmp78F;_LL4DB: if(*((int*)_tmp78D)!= 2)goto _LL4DD;_tmp78E=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp78D)->f1;_tmp78F=((struct Cyc_Tcenv_TunionRes_struct*)_tmp78D)->f2;_LL4DC: tuf=
_tmp78F;tud=_tmp78E;if(tud->is_xtunion != _tmp77F)({struct Cyc_String_pa_struct
_tmp792;_tmp792.tag=0;_tmp792.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp77D));{void*_tmp790[1]={& _tmp792};Cyc_Tcutil_terr(loc,({const char*_tmp791="[x]tunion is different from type declaration %s";
_tag_arr(_tmp791,sizeof(char),_get_zero_arr_size(_tmp791,48));}),_tag_arr(
_tmp790,sizeof(void*),1));}});goto _LL4DA;_LL4DD:;_LL4DE:({struct Cyc_String_pa_struct
_tmp796;_tmp796.tag=0;_tmp796.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp77D));{struct Cyc_String_pa_struct _tmp795;_tmp795.tag=0;_tmp795.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp77E));{void*_tmp793[
2]={& _tmp795,& _tmp796};Cyc_Tcutil_terr(loc,({const char*_tmp794="unbound field %s in type tunion %s";
_tag_arr(_tmp794,sizeof(char),_get_zero_arr_size(_tmp794,35));}),_tag_arr(
_tmp793,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp797=cvtenv;
_npop_handler(0);return _tmp797;}_LL4DA:;};_pop_handler();}else{void*_tmp78B=(
void*)_exn_thrown;void*_tmp799=_tmp78B;_LL4E0: if(_tmp799 != Cyc_Dict_Absent)goto
_LL4E2;_LL4E1:({struct Cyc_String_pa_struct _tmp79D;_tmp79D.tag=0;_tmp79D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp77D));{struct
Cyc_String_pa_struct _tmp79C;_tmp79C.tag=0;_tmp79C.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp77E));{void*_tmp79A[2]={& _tmp79C,&
_tmp79D};Cyc_Tcutil_terr(loc,({const char*_tmp79B="unbound field %s in type tunion %s";
_tag_arr(_tmp79B,sizeof(char),_get_zero_arr_size(_tmp79B,35));}),_tag_arr(
_tmp79A,sizeof(void*),2));}}});return cvtenv;_LL4E2:;_LL4E3:(void)_throw(_tmp799);
_LL4DF:;}}}*_tmp707=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp79E=
_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp79F;_tmp79F.tag=1;_tmp79F.f1=tud;_tmp79F.f2=tuf;_tmp79F;});_tmp79E;});
_tmp780=tud;_tmp781=tuf;goto _LL4D4;}_LL4D3: if(*((int*)_tmp77B)!= 1)goto _LL4D0;
_tmp780=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77B)->f1;_tmp781=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp77B)->f2;_LL4D4: {struct Cyc_List_List*tvs=
_tmp780->tvs;for(0;_tmp708 != 0  && tvs != 0;(_tmp708=_tmp708->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp708->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp708 != 
0)({struct Cyc_String_pa_struct _tmp7A3;_tmp7A3.tag=0;_tmp7A3.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp781->name));{struct
Cyc_String_pa_struct _tmp7A2;_tmp7A2.tag=0;_tmp7A2.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp780->name));{void*_tmp7A0[2]={& _tmp7A2,&
_tmp7A3};Cyc_Tcutil_terr(loc,({const char*_tmp7A1="too many type arguments for tunion %s.%s";
_tag_arr(_tmp7A1,sizeof(char),_get_zero_arr_size(_tmp7A1,41));}),_tag_arr(
_tmp7A0,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct _tmp7A7;
_tmp7A7.tag=0;_tmp7A7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp781->name));{struct Cyc_String_pa_struct _tmp7A6;_tmp7A6.tag=0;_tmp7A6.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp780->name));{
void*_tmp7A4[2]={& _tmp7A6,& _tmp7A7};Cyc_Tcutil_terr(loc,({const char*_tmp7A5="too few type arguments for tunion %s.%s";
_tag_arr(_tmp7A5,sizeof(char),_get_zero_arr_size(_tmp7A5,40));}),_tag_arr(
_tmp7A4,sizeof(void*),2));}}});goto _LL4D0;}_LL4D0:;}goto _LL489;_LL498: if(_tmp6F5
<= (void*)3  || *((int*)_tmp6F5)!= 4)goto _LL49A;_tmp709=((struct Cyc_Absyn_PointerType_struct*)
_tmp6F5)->f1;_tmp70A=(void*)_tmp709.elt_typ;_tmp70B=_tmp709.elt_tq;_tmp70C=(
struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)_tmp6F5)->f1).elt_tq;
_tmp70D=_tmp709.ptr_atts;_tmp70E=(void*)_tmp70D.rgn;_tmp70F=_tmp70D.nullable;
_tmp710=_tmp70D.bounds;_tmp711=_tmp70D.zero_term;_LL499: {int is_zero_terminated;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp70A);_tmp70C->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmp70C->print_const,_tmp70A);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp70E);{void*_tmp7A8=(
void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp711))->v;int _tmp7A9;_LL4E5: if((int)_tmp7A8 != 0)goto _LL4E7;_LL4E6:{void*
_tmp7AA=Cyc_Tcutil_compress(_tmp70A);void*_tmp7AB;_LL4EC: if(_tmp7AA <= (void*)3
 || *((int*)_tmp7AA)!= 5)goto _LL4EE;_tmp7AB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7AA)->f2;if((int)_tmp7AB != 0)goto _LL4EE;_LL4ED:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp711,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4EB;_LL4EE:;_LL4EF:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp711,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4EB;
_LL4EB:;}goto _LL4E4;_LL4E7: if(_tmp7A8 <= (void*)1  || *((int*)_tmp7A8)!= 0)goto
_LL4E9;_tmp7A9=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7A8)->f1;if(_tmp7A9
!= 1)goto _LL4E9;_LL4E8: if(!Cyc_Tcutil_admits_zero(_tmp70A))({struct Cyc_String_pa_struct
_tmp7AE;_tmp7AE.tag=0;_tmp7AE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp70A));{void*_tmp7AC[1]={& _tmp7AE};Cyc_Tcutil_terr(loc,({const char*_tmp7AD="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp7AD,sizeof(char),_get_zero_arr_size(_tmp7AD,53));}),_tag_arr(
_tmp7AC,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4E4;_LL4E9:;_LL4EA:
is_zero_terminated=0;goto _LL4E4;_LL4E4:;}{void*_tmp7AF=(void*)(Cyc_Absyn_compress_conref(
_tmp710))->v;void*_tmp7B0;void*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;void*_tmp7B3;
void*_tmp7B4;_LL4F1: if((int)_tmp7AF != 0)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if(
_tmp7AF <= (void*)1  || *((int*)_tmp7AF)!= 0)goto _LL4F5;_tmp7B0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7AF)->f1;if((int)_tmp7B0 != 0)goto _LL4F5;_LL4F4: goto _LL4F0;_LL4F5: if(_tmp7AF
<= (void*)1  || *((int*)_tmp7AF)!= 0)goto _LL4F7;_tmp7B1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7AF)->f1;if(_tmp7B1 <= (void*)1  || *((int*)_tmp7B1)!= 0)goto _LL4F7;_tmp7B2=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7B1)->f1;_LL4F6: Cyc_Tcexp_tcExp(te,0,_tmp7B2);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B2))({void*_tmp7B5[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7B6="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp7B6,sizeof(char),_get_zero_arr_size(_tmp7B6,49));}),_tag_arr(
_tmp7B5,sizeof(void*),0));});{unsigned int _tmp7B8;int _tmp7B9;struct _tuple7
_tmp7B7=Cyc_Evexp_eval_const_uint_exp(_tmp7B2);_tmp7B8=_tmp7B7.f1;_tmp7B9=
_tmp7B7.f2;if(is_zero_terminated  && (!_tmp7B9  || _tmp7B8 < 1))({void*_tmp7BA[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7BB="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp7BB,sizeof(char),_get_zero_arr_size(_tmp7BB,55));}),_tag_arr(
_tmp7BA,sizeof(void*),0));});goto _LL4F0;}_LL4F7: if(_tmp7AF <= (void*)1  || *((int*)
_tmp7AF)!= 0)goto _LL4F9;_tmp7B3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7AF)->f1;if(_tmp7B3 <= (void*)1  || *((int*)_tmp7B3)!= 1)goto _LL4F9;_tmp7B4=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7B3)->f1;_LL4F8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp7B4);goto _LL4F0;_LL4F9: if(_tmp7AF <= (void*)1  || *((
int*)_tmp7AF)!= 1)goto _LL4F0;_LL4FA:({void*_tmp7BC[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp7BD="forward constraint";
_tag_arr(_tmp7BD,sizeof(char),_get_zero_arr_size(_tmp7BD,19));}),_tag_arr(
_tmp7BC,sizeof(void*),0));});_LL4F0:;}goto _LL489;}_LL49A: if(_tmp6F5 <= (void*)3
 || *((int*)_tmp6F5)!= 17)goto _LL49C;_tmp712=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6F5)->f1;_LL49B: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp712);goto _LL489;_LL49C: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 14)goto
_LL49E;_tmp713=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6F5)->f1;_LL49D:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp713);goto _LL489;
_LL49E: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 5)goto _LL4A0;_LL49F: goto
_LL4A1;_LL4A0: if((int)_tmp6F5 != 1)goto _LL4A2;_LL4A1: goto _LL4A3;_LL4A2: if(_tmp6F5
<= (void*)3  || *((int*)_tmp6F5)!= 6)goto _LL4A4;_LL4A3: goto _LL489;_LL4A4: if(
_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 7)goto _LL4A6;_tmp714=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6F5)->f1;_tmp715=(void*)_tmp714.elt_type;_tmp716=_tmp714.tq;_tmp717=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp6F5)->f1).tq;_tmp718=
_tmp714.num_elts;_tmp719=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp6F5)->f1).num_elts;_tmp71A=_tmp714.zero_term;_LL4A5: {struct Cyc_Absyn_Exp*
_tmp7BE=*_tmp719;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp715);_tmp717->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp717->print_const,
_tmp715);{int is_zero_terminated;{void*_tmp7BF=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp71A))->v;int _tmp7C0;
_LL4FC: if((int)_tmp7BF != 0)goto _LL4FE;_LL4FD:((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp71A,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4FB;_LL4FE: if(
_tmp7BF <= (void*)1  || *((int*)_tmp7BF)!= 0)goto _LL500;_tmp7C0=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7BF)->f1;if(_tmp7C0 != 1)goto _LL500;_LL4FF: if(!Cyc_Tcutil_admits_zero(_tmp715))({
struct Cyc_String_pa_struct _tmp7C3;_tmp7C3.tag=0;_tmp7C3.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp715));{void*_tmp7C1[1]={& _tmp7C3};
Cyc_Tcutil_terr(loc,({const char*_tmp7C2="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp7C2,sizeof(char),_get_zero_arr_size(_tmp7C2,51));}),_tag_arr(
_tmp7C1,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4FB;_LL500:;_LL501:
is_zero_terminated=0;goto _LL4FB;_LL4FB:;}if(_tmp7BE == 0){if(is_zero_terminated)*
_tmp719=(_tmp7BE=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*
_tmp7C4[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7C5="array bound defaults to 0 here";
_tag_arr(_tmp7C5,sizeof(char),_get_zero_arr_size(_tmp7C5,31));}),_tag_arr(
_tmp7C4,sizeof(void*),0));});*_tmp719=(_tmp7BE=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7BE);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp7BE))({void*_tmp7C6[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7C7="array bounds expression is not constant";_tag_arr(_tmp7C7,
sizeof(char),_get_zero_arr_size(_tmp7C7,40));}),_tag_arr(_tmp7C6,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp7BE))({void*_tmp7C8[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7C9="array bounds expression is not an unsigned int";
_tag_arr(_tmp7C9,sizeof(char),_get_zero_arr_size(_tmp7C9,47));}),_tag_arr(
_tmp7C8,sizeof(void*),0));});{unsigned int _tmp7CB;int _tmp7CC;struct _tuple7
_tmp7CA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp7BE);_tmp7CB=
_tmp7CA.f1;_tmp7CC=_tmp7CA.f2;if((is_zero_terminated  && _tmp7CC) && _tmp7CB < 1)({
void*_tmp7CD[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7CE="zero terminated array cannot have zero elements";
_tag_arr(_tmp7CE,sizeof(char),_get_zero_arr_size(_tmp7CE,48));}),_tag_arr(
_tmp7CD,sizeof(void*),0));});goto _LL489;}}}_LL4A6: if(_tmp6F5 <= (void*)3  || *((
int*)_tmp6F5)!= 8)goto _LL4A8;_tmp71B=((struct Cyc_Absyn_FnType_struct*)_tmp6F5)->f1;
_tmp71C=_tmp71B.tvars;_tmp71D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6F5)->f1).tvars;_tmp71E=_tmp71B.effect;_tmp71F=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6F5)->f1).effect;_tmp720=(void*)_tmp71B.ret_typ;
_tmp721=_tmp71B.args;_tmp722=_tmp71B.c_varargs;_tmp723=_tmp71B.cyc_varargs;
_tmp724=_tmp71B.rgn_po;_tmp725=_tmp71B.attributes;_LL4A7: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp725 != 0;_tmp725=_tmp725->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp725->hd))({struct Cyc_String_pa_struct _tmp7D1;
_tmp7D1.tag=0;_tmp7D1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp725->hd));{void*_tmp7CF[1]={& _tmp7D1};Cyc_Tcutil_terr(loc,({const char*
_tmp7D0="bad function type attribute %s";_tag_arr(_tmp7D0,sizeof(char),
_get_zero_arr_size(_tmp7D0,31));}),_tag_arr(_tmp7CF,sizeof(void*),1));}});{void*
_tmp7D2=(void*)_tmp725->hd;void*_tmp7D3;int _tmp7D4;int _tmp7D5;_LL503: if((int)
_tmp7D2 != 0)goto _LL505;_LL504: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL502;_LL505: if((int)_tmp7D2 != 1)goto _LL507;_LL506: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL502;_LL507: if((int)_tmp7D2 != 2)goto _LL509;_LL508: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL502;_LL509: if(_tmp7D2 <= (void*)
17  || *((int*)_tmp7D2)!= 3)goto _LL50B;_tmp7D3=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7D2)->f1;_tmp7D4=((struct Cyc_Absyn_Format_att_struct*)_tmp7D2)->f2;_tmp7D5=((
struct Cyc_Absyn_Format_att_struct*)_tmp7D2)->f3;_LL50A: if(!seen_format){
seen_format=1;ft=_tmp7D3;fmt_desc_arg=_tmp7D4;fmt_arg_start=_tmp7D5;}else{({void*
_tmp7D6[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7D7="function can't have multiple format attributes";
_tag_arr(_tmp7D7,sizeof(char),_get_zero_arr_size(_tmp7D7,47));}),_tag_arr(
_tmp7D6,sizeof(void*),0));});}goto _LL502;_LL50B:;_LL50C: goto _LL502;_LL502:;}}if(
num_convs > 1)({void*_tmp7D8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7D9="function can't have multiple calling conventions";
_tag_arr(_tmp7D9,sizeof(char),_get_zero_arr_size(_tmp7D9,49));}),_tag_arr(
_tmp7D8,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp71D);{struct
Cyc_List_List*b=*_tmp71D;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7DA=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7DB;_LL50E: if(*((int*)_tmp7DA)!= 0)goto
_LL510;_tmp7DB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7DA)->f1;if((int)
_tmp7DB != 1)goto _LL510;_LL50F:({struct Cyc_String_pa_struct _tmp7DE;_tmp7DE.tag=0;
_tmp7DE.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7DC[1]={& _tmp7DE};Cyc_Tcutil_terr(loc,({const char*_tmp7DD="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7DD,sizeof(char),_get_zero_arr_size(_tmp7DD,51));}),_tag_arr(
_tmp7DC,sizeof(void*),1));}});goto _LL50D;_LL510:;_LL511: goto _LL50D;_LL50D:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7DF=({struct Cyc_Tcutil_CVTEnv _tmp85B;_tmp85B.kind_env=
cvtenv.kind_env;_tmp85B.free_vars=0;_tmp85B.free_evars=0;_tmp85B.generalize_evars=
cvtenv.generalize_evars;_tmp85B.fn_result=1;_tmp85B;});_tmp7DF=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7DF,(void*)1,_tmp720);_tmp7DF.fn_result=0;{struct Cyc_List_List*a=
_tmp721;for(0;a != 0;a=a->tl){struct _tuple2*_tmp7E0=(struct _tuple2*)a->hd;void*
_tmp7E1=(*_tmp7E0).f3;_tmp7DF=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7DF,(void*)
1,_tmp7E1);((*_tmp7E0).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmp7E0).f2).print_const,_tmp7E1);}}if(_tmp723 != 0){if(_tmp722)({void*_tmp7E2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp7E3="both c_vararg and cyc_vararg";_tag_arr(_tmp7E3,sizeof(char),
_get_zero_arr_size(_tmp7E3,29));}),_tag_arr(_tmp7E2,sizeof(void*),0));});{void*
_tmp7E5;int _tmp7E6;struct Cyc_Absyn_VarargInfo _tmp7E4=*_tmp723;_tmp7E5=(void*)
_tmp7E4.type;_tmp7E6=_tmp7E4.inject;_tmp7DF=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7DF,(void*)1,_tmp7E5);(_tmp723->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp723->tq).print_const,_tmp7E5);if(_tmp7E6){void*_tmp7E7=Cyc_Tcutil_compress(
_tmp7E5);struct Cyc_Absyn_TunionInfo _tmp7E8;void*_tmp7E9;_LL513: if(_tmp7E7 <= (
void*)3  || *((int*)_tmp7E7)!= 2)goto _LL515;_tmp7E8=((struct Cyc_Absyn_TunionType_struct*)
_tmp7E7)->f1;_tmp7E9=(void*)_tmp7E8.tunion_info;if(*((int*)_tmp7E9)!= 1)goto
_LL515;_LL514: goto _LL512;_LL515:;_LL516:({void*_tmp7EA[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7EB="can't inject a non-[x]tunion type";_tag_arr(_tmp7EB,sizeof(
char),_get_zero_arr_size(_tmp7EB,34));}),_tag_arr(_tmp7EA,sizeof(void*),0));});
goto _LL512;_LL512:;}}}if(seen_format){int _tmp7EC=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp721);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp7EC) || 
fmt_arg_start < 0) || _tmp723 == 0  && fmt_arg_start != 0) || _tmp723 != 0  && 
fmt_arg_start != _tmp7EC + 1)({void*_tmp7ED[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7EE="bad format descriptor";_tag_arr(_tmp7EE,sizeof(char),_get_zero_arr_size(
_tmp7EE,22));}),_tag_arr(_tmp7ED,sizeof(void*),0));});else{void*_tmp7F0;struct
_tuple2 _tmp7EF=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp721,fmt_desc_arg - 1);_tmp7F0=_tmp7EF.f3;{void*_tmp7F1=Cyc_Tcutil_compress(
_tmp7F0);struct Cyc_Absyn_PtrInfo _tmp7F2;void*_tmp7F3;struct Cyc_Absyn_PtrAtts
_tmp7F4;struct Cyc_Absyn_Conref*_tmp7F5;struct Cyc_Absyn_Conref*_tmp7F6;_LL518: if(
_tmp7F1 <= (void*)3  || *((int*)_tmp7F1)!= 4)goto _LL51A;_tmp7F2=((struct Cyc_Absyn_PointerType_struct*)
_tmp7F1)->f1;_tmp7F3=(void*)_tmp7F2.elt_typ;_tmp7F4=_tmp7F2.ptr_atts;_tmp7F5=
_tmp7F4.bounds;_tmp7F6=_tmp7F4.zero_term;_LL519:{struct _tuple6 _tmp7F8=({struct
_tuple6 _tmp7F7;_tmp7F7.f1=Cyc_Tcutil_compress(_tmp7F3);_tmp7F7.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp7F5);_tmp7F7;});void*_tmp7F9;void*_tmp7FA;void*_tmp7FB;void*
_tmp7FC;_LL51D: _tmp7F9=_tmp7F8.f1;if(_tmp7F9 <= (void*)3  || *((int*)_tmp7F9)!= 5)
goto _LL51F;_tmp7FA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7F9)->f1;if((int)
_tmp7FA != 2)goto _LL51F;_tmp7FB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7F9)->f2;
if((int)_tmp7FB != 0)goto _LL51F;_tmp7FC=_tmp7F8.f2;if((int)_tmp7FC != 0)goto _LL51F;
_LL51E: goto _LL51C;_LL51F:;_LL520:({void*_tmp7FD[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7FE="format descriptor is not a char ? type";_tag_arr(_tmp7FE,
sizeof(char),_get_zero_arr_size(_tmp7FE,39));}),_tag_arr(_tmp7FD,sizeof(void*),0));});
goto _LL51C;_LL51C:;}goto _LL517;_LL51A:;_LL51B:({void*_tmp7FF[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp800="format descriptor is not a char ? type";_tag_arr(
_tmp800,sizeof(char),_get_zero_arr_size(_tmp800,39));}),_tag_arr(_tmp7FF,sizeof(
void*),0));});goto _LL517;_LL517:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp802=({struct _tuple6 _tmp801;_tmp801.f1=ft;_tmp801.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp723))->type);_tmp801;});void*
_tmp803;void*_tmp804;struct Cyc_Absyn_TunionInfo _tmp805;void*_tmp806;struct Cyc_Absyn_Tuniondecl**
_tmp807;struct Cyc_Absyn_Tuniondecl*_tmp808;void*_tmp809;void*_tmp80A;struct Cyc_Absyn_TunionInfo
_tmp80B;void*_tmp80C;struct Cyc_Absyn_Tuniondecl**_tmp80D;struct Cyc_Absyn_Tuniondecl*
_tmp80E;_LL522: _tmp803=_tmp802.f1;if((int)_tmp803 != 0)goto _LL524;_tmp804=_tmp802.f2;
if(_tmp804 <= (void*)3  || *((int*)_tmp804)!= 2)goto _LL524;_tmp805=((struct Cyc_Absyn_TunionType_struct*)
_tmp804)->f1;_tmp806=(void*)_tmp805.tunion_info;if(*((int*)_tmp806)!= 1)goto
_LL524;_tmp807=((struct Cyc_Absyn_KnownTunion_struct*)_tmp806)->f1;_tmp808=*
_tmp807;_LL523: problem=Cyc_Absyn_qvar_cmp(_tmp808->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL521;_LL524: _tmp809=_tmp802.f1;if((int)_tmp809 != 1)goto _LL526;_tmp80A=
_tmp802.f2;if(_tmp80A <= (void*)3  || *((int*)_tmp80A)!= 2)goto _LL526;_tmp80B=((
struct Cyc_Absyn_TunionType_struct*)_tmp80A)->f1;_tmp80C=(void*)_tmp80B.tunion_info;
if(*((int*)_tmp80C)!= 1)goto _LL526;_tmp80D=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp80C)->f1;_tmp80E=*_tmp80D;_LL525: problem=Cyc_Absyn_qvar_cmp(_tmp80E->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL521;_LL526:;_LL527: problem=1;goto
_LL521;_LL521:;}if(problem)({void*_tmp80F[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp810="format attribute and vararg types don't match";_tag_arr(_tmp810,sizeof(
char),_get_zero_arr_size(_tmp810,46));}),_tag_arr(_tmp80F,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp724;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp812;
void*_tmp813;void*_tmp814;struct _tuple6*_tmp811=(struct _tuple6*)rpo->hd;_tmp812=*
_tmp811;_tmp813=_tmp812.f1;_tmp814=_tmp812.f2;_tmp7DF=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7DF,(void*)4,_tmp813);_tmp7DF=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7DF,(void*)3,_tmp814);}}if(*_tmp71F != 0)_tmp7DF=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7DF,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp71F))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7DF.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp815=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp816;struct Cyc_Core_Opt**_tmp817;void*
_tmp818;void*_tmp819;void*_tmp81A;void*_tmp81B;struct Cyc_Core_Opt*_tmp81C;struct
Cyc_Core_Opt**_tmp81D;void*_tmp81E;void*_tmp81F;struct Cyc_Core_Opt*_tmp820;
struct Cyc_Core_Opt**_tmp821;_LL529: if(*((int*)_tmp815)!= 2)goto _LL52B;_tmp816=((
struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp817=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp818=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp815)->f2;if((int)_tmp818 != 3)goto _LL52B;_LL52A:*_tmp817=({struct Cyc_Core_Opt*
_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp824;_tmp824.tag=0;_tmp824.f1=(void*)((void*)3);_tmp824;});_tmp823;}));
_tmp822;});goto _LL52C;_LL52B: if(*((int*)_tmp815)!= 0)goto _LL52D;_tmp819=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp815)->f1;if((int)_tmp819 != 3)goto _LL52D;_LL52C:
effect=({struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp826=_cycalloc(sizeof(*
_tmp826));_tmp826[0]=({struct Cyc_Absyn_AccessEff_struct _tmp827;_tmp827.tag=19;
_tmp827.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp828=_cycalloc(
sizeof(*_tmp828));_tmp828[0]=({struct Cyc_Absyn_VarType_struct _tmp829;_tmp829.tag=
1;_tmp829.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp829;});_tmp828;}));_tmp827;});
_tmp826;}));_tmp825->tl=effect;_tmp825;});goto _LL528;_LL52D: if(*((int*)_tmp815)
!= 2)goto _LL52F;_tmp81A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f2;
if((int)_tmp81A != 5)goto _LL52F;_LL52E: goto _LL530;_LL52F: if(*((int*)_tmp815)!= 0)
goto _LL531;_tmp81B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp815)->f1;if((int)
_tmp81B != 5)goto _LL531;_LL530: goto _LL528;_LL531: if(*((int*)_tmp815)!= 2)goto
_LL533;_tmp81C=((struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp81D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp815)->f1;_tmp81E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp815)->f2;if((int)_tmp81E != 4)goto _LL533;_LL532:*_tmp81D=({struct Cyc_Core_Opt*
_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp82C;_tmp82C.tag=0;_tmp82C.f1=(void*)((void*)4);_tmp82C;});_tmp82B;}));
_tmp82A;});goto _LL534;_LL533: if(*((int*)_tmp815)!= 0)goto _LL535;_tmp81F=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp815)->f1;if((int)_tmp81F != 4)goto _LL535;_LL534:
effect=({struct Cyc_List_List*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82E=_cycalloc(sizeof(*_tmp82E));
_tmp82E[0]=({struct Cyc_Absyn_VarType_struct _tmp82F;_tmp82F.tag=1;_tmp82F.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp82F;});_tmp82E;}));_tmp82D->tl=effect;_tmp82D;});
goto _LL528;_LL535: if(*((int*)_tmp815)!= 1)goto _LL537;_tmp820=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp815)->f1;_tmp821=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp815)->f1;_LL536:*_tmp821=({struct Cyc_Core_Opt*_tmp830=_cycalloc(sizeof(*
_tmp830));_tmp830->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp831=
_cycalloc(sizeof(*_tmp831));_tmp831[0]=({struct Cyc_Absyn_Less_kb_struct _tmp832;
_tmp832.tag=2;_tmp832.f1=0;_tmp832.f2=(void*)((void*)0);_tmp832;});_tmp831;}));
_tmp830;});goto _LL538;_LL537:;_LL538: effect=({struct Cyc_List_List*_tmp833=
_cycalloc(sizeof(*_tmp833));_tmp833->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp835;_tmp835.tag=21;_tmp835.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836[0]=({struct Cyc_Absyn_VarType_struct
_tmp837;_tmp837.tag=1;_tmp837.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp837;});
_tmp836;}));_tmp835;});_tmp834;}));_tmp833->tl=effect;_tmp833;});goto _LL528;
_LL528:;}}{struct Cyc_List_List*ts=_tmp7DF.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp838=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL53A: if((int)_tmp838 != 3)goto
_LL53C;_LL53B: effect=({struct Cyc_List_List*_tmp839=_cycalloc(sizeof(*_tmp839));
_tmp839->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp83A=_cycalloc(
sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp83B;_tmp83B.tag=
19;_tmp83B.f1=(void*)((void*)ts->hd);_tmp83B;});_tmp83A;}));_tmp839->tl=effect;
_tmp839;});goto _LL539;_LL53C: if((int)_tmp838 != 4)goto _LL53E;_LL53D: effect=({
struct Cyc_List_List*_tmp83C=_cycalloc(sizeof(*_tmp83C));_tmp83C->hd=(void*)((
void*)ts->hd);_tmp83C->tl=effect;_tmp83C;});goto _LL539;_LL53E: if((int)_tmp838 != 
5)goto _LL540;_LL53F: goto _LL539;_LL540:;_LL541: effect=({struct Cyc_List_List*
_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp83F;_tmp83F.tag=21;_tmp83F.f1=(void*)((void*)ts->hd);_tmp83F;});_tmp83E;}));
_tmp83D->tl=effect;_tmp83D;});goto _LL539;_LL539:;}}*_tmp71F=({struct Cyc_Core_Opt*
_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp842;_tmp842.tag=20;_tmp842.f1=effect;_tmp842;});_tmp841;}));_tmp840;});}if(*
_tmp71D != 0){struct Cyc_List_List*bs=*_tmp71D;for(0;bs != 0;bs=bs->tl){void*
_tmp843=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp844;struct Cyc_Core_Opt**_tmp845;struct Cyc_Core_Opt*
_tmp846;struct Cyc_Core_Opt**_tmp847;void*_tmp848;struct Cyc_Core_Opt*_tmp849;
struct Cyc_Core_Opt**_tmp84A;void*_tmp84B;struct Cyc_Core_Opt*_tmp84C;struct Cyc_Core_Opt**
_tmp84D;void*_tmp84E;void*_tmp84F;_LL543: if(*((int*)_tmp843)!= 1)goto _LL545;
_tmp844=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp843)->f1;_tmp845=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp843)->f1;_LL544:({struct Cyc_String_pa_struct
_tmp852;_tmp852.tag=0;_tmp852.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp850[1]={& _tmp852};Cyc_Tcutil_warn(
loc,({const char*_tmp851="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp851,sizeof(char),_get_zero_arr_size(_tmp851,47));}),_tag_arr(
_tmp850,sizeof(void*),1));}});_tmp847=_tmp845;goto _LL546;_LL545: if(*((int*)
_tmp843)!= 2)goto _LL547;_tmp846=((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f1;
_tmp847=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f1;
_tmp848=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f2;if((int)_tmp848 != 
1)goto _LL547;_LL546: _tmp84A=_tmp847;goto _LL548;_LL547: if(*((int*)_tmp843)!= 2)
goto _LL549;_tmp849=((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f1;_tmp84A=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f1;_tmp84B=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f2;if((int)_tmp84B != 0)goto
_LL549;_LL548:*_tmp84A=({struct Cyc_Core_Opt*_tmp853=_cycalloc(sizeof(*_tmp853));
_tmp853->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp854=_cycalloc(
sizeof(*_tmp854));_tmp854[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp855;_tmp855.tag=0;
_tmp855.f1=(void*)((void*)2);_tmp855;});_tmp854;}));_tmp853;});goto _LL542;_LL549:
if(*((int*)_tmp843)!= 2)goto _LL54B;_tmp84C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp843)->f1;_tmp84D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp843)->f1;_tmp84E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp843)->f2;
_LL54A:*_tmp84D=({struct Cyc_Core_Opt*_tmp856=_cycalloc(sizeof(*_tmp856));_tmp856->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp857=_cycalloc(sizeof(*_tmp857));
_tmp857[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp858;_tmp858.tag=0;_tmp858.f1=(void*)
_tmp84E;_tmp858;});_tmp857;}));_tmp856;});goto _LL542;_LL54B: if(*((int*)_tmp843)
!= 0)goto _LL54D;_tmp84F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp843)->f1;if((
int)_tmp84F != 1)goto _LL54D;_LL54C:({void*_tmp859[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp85A="functions can't abstract M types";_tag_arr(_tmp85A,sizeof(
char),_get_zero_arr_size(_tmp85A,33));}),_tag_arr(_tmp859,sizeof(void*),0));});
goto _LL542;_LL54D:;_LL54E: goto _LL542;_LL542:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7DF.kind_env,*_tmp71D);_tmp7DF.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7DF.free_vars,*_tmp71D);{struct Cyc_List_List*tvs=_tmp7DF.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7DF.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL489;}}_LL4A8: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 
9)goto _LL4AA;_tmp726=((struct Cyc_Absyn_TupleType_struct*)_tmp6F5)->f1;_LL4A9:
for(0;_tmp726 != 0;_tmp726=_tmp726->tl){struct _tuple4*_tmp85C=(struct _tuple4*)
_tmp726->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp85C).f2);((*_tmp85C).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp85C).f1).print_const,(*_tmp85C).f2);}goto _LL489;_LL4AA: if(_tmp6F5 <= (
void*)3  || *((int*)_tmp6F5)!= 11)goto _LL4AC;_tmp727=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6F5)->f1;_tmp728=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6F5)->f2;_LL4AB:{
struct _RegionHandle _tmp85D=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp85D;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp728 != 0;_tmp728=_tmp728->tl){struct Cyc_Absyn_Aggrfield _tmp85F;struct
_tagged_arr*_tmp860;struct Cyc_Absyn_Tqual _tmp861;struct Cyc_Absyn_Tqual*_tmp862;
void*_tmp863;struct Cyc_Absyn_Exp*_tmp864;struct Cyc_List_List*_tmp865;struct Cyc_Absyn_Aggrfield*
_tmp85E=(struct Cyc_Absyn_Aggrfield*)_tmp728->hd;_tmp85F=*_tmp85E;_tmp860=_tmp85F.name;
_tmp861=_tmp85F.tq;_tmp862=(struct Cyc_Absyn_Tqual*)&(*_tmp85E).tq;_tmp863=(void*)
_tmp85F.type;_tmp864=_tmp85F.width;_tmp865=_tmp85F.attributes;if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp860))({struct Cyc_String_pa_struct
_tmp868;_tmp868.tag=0;_tmp868.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp860);{
void*_tmp866[1]={& _tmp868};Cyc_Tcutil_terr(loc,({const char*_tmp867="duplicate field %s";
_tag_arr(_tmp867,sizeof(char),_get_zero_arr_size(_tmp867,19));}),_tag_arr(
_tmp866,sizeof(void*),1));}});if(Cyc_strcmp((struct _tagged_arr)*_tmp860,({const
char*_tmp869="";_tag_arr(_tmp869,sizeof(char),_get_zero_arr_size(_tmp869,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp86A=_region_malloc(aprev_rgn,sizeof(*
_tmp86A));_tmp86A->hd=_tmp860;_tmp86A->tl=prev_fields;_tmp86A;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp863);_tmp862->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp862->print_const,_tmp863);if(_tmp727 == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp863))({struct Cyc_String_pa_struct _tmp86D;_tmp86D.tag=0;_tmp86D.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp860);{void*_tmp86B[1]={& _tmp86D};Cyc_Tcutil_warn(
loc,({const char*_tmp86C="union member %s is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp86C,sizeof(char),_get_zero_arr_size(_tmp86C,74));}),_tag_arr(
_tmp86B,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp863,_tmp864,
_tmp860);Cyc_Tcutil_check_field_atts(loc,_tmp860,_tmp865);}};_pop_region(
aprev_rgn);}goto _LL489;_LL4AC: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 10)
goto _LL4AE;_tmp729=((struct Cyc_Absyn_AggrType_struct*)_tmp6F5)->f1;_tmp72A=(void*)
_tmp729.aggr_info;_tmp72B=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6F5)->f1).aggr_info;
_tmp72C=_tmp729.targs;_tmp72D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6F5)->f1).targs;_LL4AD:{void*_tmp86E=*_tmp72B;void*_tmp86F;struct _tuple1*
_tmp870;struct Cyc_Absyn_Aggrdecl**_tmp871;struct Cyc_Absyn_Aggrdecl*_tmp872;
_LL550: if(*((int*)_tmp86E)!= 0)goto _LL552;_tmp86F=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp86E)->f1;_tmp870=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp86E)->f2;_LL551: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp873;_push_handler(& _tmp873);{
int _tmp875=0;if(setjmp(_tmp873.handler))_tmp875=1;if(!_tmp875){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp870);*_tmp72B=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp876=
_cycalloc(sizeof(*_tmp876));_tmp876[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp877;
_tmp877.tag=1;_tmp877.f1=adp;_tmp877;});_tmp876;});;_pop_handler();}else{void*
_tmp874=(void*)_exn_thrown;void*_tmp879=_tmp874;_LL555: if(_tmp879 != Cyc_Dict_Absent)
goto _LL557;_LL556: {struct Cyc_Tcenv_Genv*_tmp87A=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp87B=({struct Cyc_Absyn_Aggrdecl*_tmp881=_cycalloc(
sizeof(*_tmp881));_tmp881->kind=(void*)_tmp86F;_tmp881->sc=(void*)((void*)3);
_tmp881->name=_tmp870;_tmp881->tvs=0;_tmp881->impl=0;_tmp881->attributes=0;
_tmp881;});Cyc_Tc_tcAggrdecl(te,_tmp87A,loc,_tmp87B);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp870);*_tmp72B=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp87C=
_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp87D;
_tmp87D.tag=1;_tmp87D.f1=adp;_tmp87D;});_tmp87C;});if(*_tmp72D != 0){({struct Cyc_String_pa_struct
_tmp880;_tmp880.tag=0;_tmp880.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp870));{void*_tmp87E[1]={& _tmp880};Cyc_Tcutil_terr(loc,({const char*_tmp87F="declare parameterized type %s before using";
_tag_arr(_tmp87F,sizeof(char),_get_zero_arr_size(_tmp87F,43));}),_tag_arr(
_tmp87E,sizeof(void*),1));}});return cvtenv;}goto _LL554;}_LL557:;_LL558:(void)
_throw(_tmp879);_LL554:;}}}_tmp872=*adp;goto _LL553;}_LL552: if(*((int*)_tmp86E)!= 
1)goto _LL54F;_tmp871=((struct Cyc_Absyn_KnownAggr_struct*)_tmp86E)->f1;_tmp872=*
_tmp871;_LL553: {struct Cyc_List_List*tvs=_tmp872->tvs;struct Cyc_List_List*ts=*
_tmp72D;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp884;_tmp884.tag=
0;_tmp884.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp872->name));{void*_tmp882[1]={& _tmp884};Cyc_Tcutil_terr(loc,({const char*
_tmp883="too many parameters for type %s";_tag_arr(_tmp883,sizeof(char),
_get_zero_arr_size(_tmp883,32));}),_tag_arr(_tmp882,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->hd=(void*)e;_tmp885->tl=
hidden_ts;_tmp885;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp72D=Cyc_List_imp_append(*_tmp72D,Cyc_List_imp_rev(hidden_ts));}}_LL54F:;}
goto _LL489;_LL4AE: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 16)goto _LL4B0;
_tmp72E=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f1;_tmp72F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F5)->f2;_tmp730=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F5)->f2;_tmp731=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f3;_tmp732=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f3;
_tmp733=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f4;_tmp734=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F5)->f4;_LL4AF: {struct Cyc_List_List*
targs=*_tmp730;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp886;
_push_handler(& _tmp886);{int _tmp888=0;if(setjmp(_tmp886.handler))_tmp888=1;if(!
_tmp888){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp72E);;_pop_handler();}else{
void*_tmp887=(void*)_exn_thrown;void*_tmp88A=_tmp887;_LL55A: if(_tmp88A != Cyc_Dict_Absent)
goto _LL55C;_LL55B:({struct Cyc_String_pa_struct _tmp88D;_tmp88D.tag=0;_tmp88D.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp72E));{void*
_tmp88B[1]={& _tmp88D};Cyc_Tcutil_terr(loc,({const char*_tmp88C="unbound typedef name %s";
_tag_arr(_tmp88C,sizeof(char),_get_zero_arr_size(_tmp88C,24));}),_tag_arr(
_tmp88B,sizeof(void*),1));}});return cvtenv;_LL55C:;_LL55D:(void)_throw(_tmp88A);
_LL559:;}}}*_tmp732=(struct Cyc_Absyn_Typedefdecl*)td;_tmp72E[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp88E=_cycalloc(
sizeof(*_tmp88E));_tmp88E->hd=({struct _tuple8*_tmp88F=_cycalloc(sizeof(*_tmp88F));
_tmp88F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp88F->f2=(void*)ts->hd;_tmp88F;});
_tmp88E->tl=inst;_tmp88E;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp892;
_tmp892.tag=0;_tmp892.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp72E));{void*_tmp890[1]={& _tmp892};Cyc_Tcutil_terr(loc,({const char*_tmp891="too many parameters for typedef %s";
_tag_arr(_tmp891,sizeof(char),_get_zero_arr_size(_tmp891,35));}),_tag_arr(
_tmp890,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp893=_cycalloc(
sizeof(*_tmp893));_tmp893->hd=(void*)e;_tmp893->tl=hidden_ts;_tmp893;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894->hd=({struct _tuple8*_tmp895=
_cycalloc(sizeof(*_tmp895));_tmp895->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp895->f2=
e;_tmp895;});_tmp894->tl=inst;_tmp894;});}*_tmp730=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp734=({void**_tmp896=
_cycalloc(sizeof(*_tmp896));_tmp896[0]=new_typ;_tmp896;});}goto _LL489;}}_LL4B0:
if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 18)goto _LL4B2;_LL4B1: goto _LL4B3;
_LL4B2: if((int)_tmp6F5 != 2)goto _LL4B4;_LL4B3: goto _LL489;_LL4B4: if(_tmp6F5 <= (
void*)3  || *((int*)_tmp6F5)!= 15)goto _LL4B6;_tmp735=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6F5)->f1;_LL4B5: _tmp736=_tmp735;goto _LL4B7;_LL4B6: if(_tmp6F5 <= (void*)3  || *((
int*)_tmp6F5)!= 19)goto _LL4B8;_tmp736=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6F5)->f1;_LL4B7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp736);goto _LL489;_LL4B8: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 21)goto
_LL4BA;_tmp737=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6F5)->f1;_LL4B9:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp737);goto _LL489;
_LL4BA: if(_tmp6F5 <= (void*)3  || *((int*)_tmp6F5)!= 20)goto _LL489;_tmp738=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6F5)->f1;_LL4BB: for(0;_tmp738 != 0;_tmp738=
_tmp738->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp738->hd);}goto _LL489;_LL489:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind))({struct Cyc_String_pa_struct _tmp89B;_tmp89B.tag=0;_tmp89B.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{
struct Cyc_String_pa_struct _tmp89A;_tmp89A.tag=0;_tmp89A.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct
_tmp899;_tmp899.tag=0;_tmp899.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp897[3]={& _tmp899,& _tmp89A,& _tmp89B};Cyc_Tcutil_terr(loc,({const
char*_tmp898="type %s has kind %s but as used here needs kind %s";_tag_arr(
_tmp898,sizeof(char),_get_zero_arr_size(_tmp898,51));}),_tag_arr(_tmp897,sizeof(
void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
kind_env,void*expected_kind,int generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp89C=Cyc_Tcutil_i_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8A5;
_tmp8A5.kind_env=kind_env;_tmp8A5.free_vars=0;_tmp8A5.free_evars=0;_tmp8A5.generalize_evars=
generalize_evars;_tmp8A5.fn_result=0;_tmp8A5;}),expected_kind,t);{struct Cyc_List_List*
vs=_tmp89C.free_vars;for(0;vs != 0;vs=vs->tl){_tmp89C.kind_env=Cyc_Tcutil_fast_add_free_tvar(
kind_env,(struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp89C.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp89D=Cyc_Tcutil_compress((void*)evs->hd);
struct Cyc_Core_Opt*_tmp89E;struct Cyc_Core_Opt**_tmp89F;_LL55F: if(_tmp89D <= (void*)
3  || *((int*)_tmp89D)!= 0)goto _LL561;_tmp89E=((struct Cyc_Absyn_Evar_struct*)
_tmp89D)->f4;_tmp89F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp89D)->f4;_LL560: if(*_tmp89F == 0)*_tmp89F=({struct Cyc_Core_Opt*_tmp8A0=
_cycalloc(sizeof(*_tmp8A0));_tmp8A0->v=kind_env;_tmp8A0;});else{struct Cyc_List_List*
_tmp8A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp89F))->v;
struct Cyc_List_List*_tmp8A2=0;for(0;_tmp8A1 != 0;_tmp8A1=_tmp8A1->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp8A1->hd))_tmp8A2=({struct Cyc_List_List*_tmp8A3=_cycalloc(
sizeof(*_tmp8A3));_tmp8A3->hd=(struct Cyc_Absyn_Tvar*)_tmp8A1->hd;_tmp8A3->tl=
_tmp8A2;_tmp8A3;});}*_tmp89F=({struct Cyc_Core_Opt*_tmp8A4=_cycalloc(sizeof(*
_tmp8A4));_tmp8A4->v=_tmp8A2;_tmp8A4;});}goto _LL55E;_LL561:;_LL562: goto _LL55E;
_LL55E:;}}return _tmp89C;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp8A6=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp8A7=_tmp8A6.free_vars;struct Cyc_List_List*
_tmp8A8=_tmp8A6.free_evars;{struct Cyc_List_List*x=_tmp8A7;for(0;x != 0;x=x->tl){
void*_tmp8A9=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp8AA;struct Cyc_Core_Opt**_tmp8AB;struct Cyc_Core_Opt*
_tmp8AC;struct Cyc_Core_Opt**_tmp8AD;void*_tmp8AE;struct Cyc_Core_Opt*_tmp8AF;
struct Cyc_Core_Opt**_tmp8B0;void*_tmp8B1;struct Cyc_Core_Opt*_tmp8B2;struct Cyc_Core_Opt**
_tmp8B3;void*_tmp8B4;void*_tmp8B5;_LL564: if(*((int*)_tmp8A9)!= 1)goto _LL566;
_tmp8AA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8A9)->f1;_tmp8AB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8A9)->f1;_LL565: _tmp8AD=_tmp8AB;goto
_LL567;_LL566: if(*((int*)_tmp8A9)!= 2)goto _LL568;_tmp8AC=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8A9)->f1;_tmp8AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8A9)->f1;_tmp8AE=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8A9)->f2;if((
int)_tmp8AE != 1)goto _LL568;_LL567: _tmp8B0=_tmp8AD;goto _LL569;_LL568: if(*((int*)
_tmp8A9)!= 2)goto _LL56A;_tmp8AF=((struct Cyc_Absyn_Less_kb_struct*)_tmp8A9)->f1;
_tmp8B0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8A9)->f1;
_tmp8B1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8A9)->f2;if((int)_tmp8B1 != 
0)goto _LL56A;_LL569:*_tmp8B0=({struct Cyc_Core_Opt*_tmp8B6=_cycalloc(sizeof(*
_tmp8B6));_tmp8B6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8B7=
_cycalloc(sizeof(*_tmp8B7));_tmp8B7[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8B8;
_tmp8B8.tag=0;_tmp8B8.f1=(void*)((void*)2);_tmp8B8;});_tmp8B7;}));_tmp8B6;});
goto _LL563;_LL56A: if(*((int*)_tmp8A9)!= 2)goto _LL56C;_tmp8B2=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8A9)->f1;_tmp8B3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8A9)->f1;_tmp8B4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8A9)->f2;
_LL56B:*_tmp8B3=({struct Cyc_Core_Opt*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BA=_cycalloc(sizeof(*_tmp8BA));
_tmp8BA[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8BB;_tmp8BB.tag=0;_tmp8BB.f1=(void*)
_tmp8B4;_tmp8BB;});_tmp8BA;}));_tmp8B9;});goto _LL563;_LL56C: if(*((int*)_tmp8A9)
!= 0)goto _LL56E;_tmp8B5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8A9)->f1;if((
int)_tmp8B5 != 1)goto _LL56E;_LL56D:({struct Cyc_String_pa_struct _tmp8BE;_tmp8BE.tag=
0;_tmp8BE.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8BC[1]={& _tmp8BE};Cyc_Tcutil_terr(loc,({
const char*_tmp8BD="type variable %s cannot have kind M";_tag_arr(_tmp8BD,sizeof(
char),_get_zero_arr_size(_tmp8BD,36));}),_tag_arr(_tmp8BC,sizeof(void*),1));}});
goto _LL563;_LL56E:;_LL56F: goto _LL563;_LL563:;}}if(_tmp8A7 != 0  || _tmp8A8 != 0){{
void*_tmp8BF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8C0;struct Cyc_List_List*
_tmp8C1;struct Cyc_List_List**_tmp8C2;struct Cyc_Core_Opt*_tmp8C3;void*_tmp8C4;
struct Cyc_List_List*_tmp8C5;int _tmp8C6;struct Cyc_Absyn_VarargInfo*_tmp8C7;struct
Cyc_List_List*_tmp8C8;struct Cyc_List_List*_tmp8C9;_LL571: if(_tmp8BF <= (void*)3
 || *((int*)_tmp8BF)!= 8)goto _LL573;_tmp8C0=((struct Cyc_Absyn_FnType_struct*)
_tmp8BF)->f1;_tmp8C1=_tmp8C0.tvars;_tmp8C2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp8BF)->f1).tvars;_tmp8C3=_tmp8C0.effect;_tmp8C4=(void*)_tmp8C0.ret_typ;
_tmp8C5=_tmp8C0.args;_tmp8C6=_tmp8C0.c_varargs;_tmp8C7=_tmp8C0.cyc_varargs;
_tmp8C8=_tmp8C0.rgn_po;_tmp8C9=_tmp8C0.attributes;_LL572: if(*_tmp8C2 == 0){*
_tmp8C2=_tmp8A7;_tmp8A7=0;}goto _LL570;_LL573:;_LL574: goto _LL570;_LL570:;}if(
_tmp8A7 != 0)({struct Cyc_String_pa_struct _tmp8CC;_tmp8CC.tag=0;_tmp8CC.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp8A7->hd)->name);{
void*_tmp8CA[1]={& _tmp8CC};Cyc_Tcutil_terr(loc,({const char*_tmp8CB="unbound type variable %s";
_tag_arr(_tmp8CB,sizeof(char),_get_zero_arr_size(_tmp8CB,25));}),_tag_arr(
_tmp8CA,sizeof(void*),1));}});if(_tmp8A8 != 0)for(0;_tmp8A8 != 0;_tmp8A8=_tmp8A8->tl){
void*e=(void*)_tmp8A8->hd;void*_tmp8CD=Cyc_Tcutil_typ_kind(e);_LL576: if((int)
_tmp8CD != 3)goto _LL578;_LL577: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8CE[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp8CF="can't unify evar with heap!";_tag_arr(_tmp8CF,sizeof(char),
_get_zero_arr_size(_tmp8CF,28));}),_tag_arr(_tmp8CE,sizeof(void*),0));});goto
_LL575;_LL578: if((int)_tmp8CD != 4)goto _LL57A;_LL579: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8D0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D1="can't unify evar with {}!";_tag_arr(_tmp8D1,sizeof(char),
_get_zero_arr_size(_tmp8D1,26));}),_tag_arr(_tmp8D0,sizeof(void*),0));});goto
_LL575;_LL57A:;_LL57B:({struct Cyc_String_pa_struct _tmp8D5;_tmp8D5.tag=0;_tmp8D5.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8D4;_tmp8D4.tag=0;_tmp8D4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8D2[2]={& _tmp8D4,& _tmp8D5};Cyc_Tcutil_terr(loc,({const char*_tmp8D3="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8D3,sizeof(char),_get_zero_arr_size(_tmp8D3,52));}),_tag_arr(
_tmp8D2,sizeof(void*),2));}}});goto _LL575;_LL575:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp8D6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8D7;struct Cyc_List_List*
_tmp8D8;struct Cyc_Core_Opt*_tmp8D9;_LL57D: if(_tmp8D6 <= (void*)3  || *((int*)
_tmp8D6)!= 8)goto _LL57F;_tmp8D7=((struct Cyc_Absyn_FnType_struct*)_tmp8D6)->f1;
_tmp8D8=_tmp8D7.tvars;_tmp8D9=_tmp8D7.effect;_LL57E: fd->tvs=_tmp8D8;fd->effect=
_tmp8D9;goto _LL57C;_LL57F:;_LL580:({void*_tmp8DA[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8DB="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8DB,sizeof(char),_get_zero_arr_size(_tmp8DB,38));}),_tag_arr(
_tmp8DA,sizeof(void*),0));});_LL57C:;}{struct _RegionHandle _tmp8DC=_new_region("r");
struct _RegionHandle*r=& _tmp8DC;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8DD="function declaration has repeated parameter";
_tag_arr(_tmp8DD,sizeof(char),_get_zero_arr_size(_tmp8DD,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8DE=_cycalloc(sizeof(*_tmp8DE));_tmp8DE->v=(
void*)t;_tmp8DE;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8DF=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp8E0=Cyc_Tcutil_remove_bound_tvars(
_tmp8DF.free_vars,bound_tvars);struct Cyc_List_List*_tmp8E1=_tmp8DF.free_evars;{
struct Cyc_List_List*fs=_tmp8E0;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8E2=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8E6;_tmp8E6.tag=
0;_tmp8E6.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8E5;_tmp8E5.tag=0;_tmp8E5.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8E2);{void*_tmp8E3[2]={& _tmp8E5,& _tmp8E6};Cyc_Tcutil_terr(
loc,({const char*_tmp8E4="unbound type variable %s in type %s";_tag_arr(_tmp8E4,
sizeof(char),_get_zero_arr_size(_tmp8E4,36));}),_tag_arr(_tmp8E3,sizeof(void*),2));}}});}}
if(!allow_evars  && _tmp8E1 != 0)for(0;_tmp8E1 != 0;_tmp8E1=_tmp8E1->tl){void*e=(
void*)_tmp8E1->hd;void*_tmp8E7=Cyc_Tcutil_typ_kind(e);_LL582: if((int)_tmp8E7 != 3)
goto _LL584;_LL583: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8E8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8E9="can't unify evar with heap!";
_tag_arr(_tmp8E9,sizeof(char),_get_zero_arr_size(_tmp8E9,28));}),_tag_arr(
_tmp8E8,sizeof(void*),0));});goto _LL581;_LL584: if((int)_tmp8E7 != 4)goto _LL586;
_LL585: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8EA[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8EB="can't unify evar with {}!";
_tag_arr(_tmp8EB,sizeof(char),_get_zero_arr_size(_tmp8EB,26));}),_tag_arr(
_tmp8EA,sizeof(void*),0));});goto _LL581;_LL586:;_LL587:({struct Cyc_String_pa_struct
_tmp8EF;_tmp8EF.tag=0;_tmp8EF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8EE;_tmp8EE.tag=0;_tmp8EE.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8EC[2]={&
_tmp8EE,& _tmp8EF};Cyc_Tcutil_terr(loc,({const char*_tmp8ED="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8ED,sizeof(char),_get_zero_arr_size(_tmp8ED,52));}),_tag_arr(
_tmp8EC,sizeof(void*),2));}}});goto _LL581;_LL581:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8F3;_tmp8F3.tag=0;_tmp8F3.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8F2;_tmp8F2.tag=0;_tmp8F2.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8F0[2]={& _tmp8F2,& _tmp8F3};
Cyc_Tcutil_terr(loc,({const char*_tmp8F1="%s: %s";_tag_arr(_tmp8F1,sizeof(char),
_get_zero_arr_size(_tmp8F1,7));}),_tag_arr(_tmp8F0,sizeof(void*),2));}}});}}}
static struct _tagged_arr Cyc_Tcutil_strptr2string(struct _tagged_arr*s){return*s;}
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr msg){((void(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({
const char*_tmp8F4="duplicate type variable";_tag_arr(_tmp8F4,sizeof(char),
_get_zero_arr_size(_tmp8F4,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8F5="";_tag_arr(_tmp8F5,sizeof(char),_get_zero_arr_size(_tmp8F5,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8F6=_cycalloc(sizeof(*_tmp8F6));_tmp8F6->hd=({
struct _tuple18*_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp8F7->f2=0;_tmp8F7;});_tmp8F6->tl=fields;_tmp8F6;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp8F9;struct Cyc_List_List*
_tmp8FA;void*_tmp8FB;struct _tuple19*_tmp8F8=(struct _tuple19*)des->hd;_tmp8F9=*
_tmp8F8;_tmp8FA=_tmp8F9.f1;_tmp8FB=_tmp8F9.f2;if(_tmp8FA == 0){struct Cyc_List_List*
_tmp8FC=fields;for(0;_tmp8FC != 0;_tmp8FC=_tmp8FC->tl){if(!(*((struct _tuple18*)
_tmp8FC->hd)).f2){(*((struct _tuple18*)_tmp8FC->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp8FD=_cycalloc(sizeof(*_tmp8FD));
_tmp8FD->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp8FE=_cycalloc(
sizeof(*_tmp8FE));_tmp8FE[0]=({struct Cyc_Absyn_FieldName_struct _tmp8FF;_tmp8FF.tag=
1;_tmp8FF.f1=((*((struct _tuple18*)_tmp8FC->hd)).f1)->name;_tmp8FF;});_tmp8FE;}));
_tmp8FD->tl=0;_tmp8FD;});ans=({struct Cyc_List_List*_tmp900=_region_malloc(rgn,
sizeof(*_tmp900));_tmp900->hd=({struct _tuple20*_tmp901=_region_malloc(rgn,
sizeof(*_tmp901));_tmp901->f1=(*((struct _tuple18*)_tmp8FC->hd)).f1;_tmp901->f2=
_tmp8FB;_tmp901;});_tmp900->tl=ans;_tmp900;});break;}}if(_tmp8FC == 0)({void*
_tmp902[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp903="too many arguments to struct";
_tag_arr(_tmp903,sizeof(char),_get_zero_arr_size(_tmp903,29));}),_tag_arr(
_tmp902,sizeof(void*),0));});}else{if(_tmp8FA->tl != 0)({void*_tmp904[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp905="multiple designators are not supported";_tag_arr(
_tmp905,sizeof(char),_get_zero_arr_size(_tmp905,39));}),_tag_arr(_tmp904,sizeof(
void*),0));});else{void*_tmp906=(void*)_tmp8FA->hd;struct _tagged_arr*_tmp907;
_LL589: if(*((int*)_tmp906)!= 0)goto _LL58B;_LL58A:({void*_tmp908[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp909="array designator used in argument to struct";_tag_arr(
_tmp909,sizeof(char),_get_zero_arr_size(_tmp909,44));}),_tag_arr(_tmp908,sizeof(
void*),0));});goto _LL588;_LL58B: if(*((int*)_tmp906)!= 1)goto _LL588;_tmp907=((
struct Cyc_Absyn_FieldName_struct*)_tmp906)->f1;_LL58C: {struct Cyc_List_List*
_tmp90A=fields;for(0;_tmp90A != 0;_tmp90A=_tmp90A->tl){if(Cyc_strptrcmp(_tmp907,((*((
struct _tuple18*)_tmp90A->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp90A->hd)).f2)({
struct Cyc_String_pa_struct _tmp90D;_tmp90D.tag=0;_tmp90D.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp907);{void*_tmp90B[1]={& _tmp90D};Cyc_Tcutil_terr(loc,({
const char*_tmp90C="field %s has already been used as an argument";_tag_arr(
_tmp90C,sizeof(char),_get_zero_arr_size(_tmp90C,46));}),_tag_arr(_tmp90B,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp90A->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp90E=_region_malloc(rgn,sizeof(*_tmp90E));_tmp90E->hd=({struct _tuple20*
_tmp90F=_region_malloc(rgn,sizeof(*_tmp90F));_tmp90F->f1=(*((struct _tuple18*)
_tmp90A->hd)).f1;_tmp90F->f2=_tmp8FB;_tmp90F;});_tmp90E->tl=ans;_tmp90E;});
break;}}if(_tmp90A == 0)({struct Cyc_String_pa_struct _tmp912;_tmp912.tag=0;_tmp912.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp907);{void*_tmp910[1]={& _tmp912};Cyc_Tcutil_terr(
loc,({const char*_tmp911="bad field designator %s";_tag_arr(_tmp911,sizeof(char),
_get_zero_arr_size(_tmp911,24));}),_tag_arr(_tmp910,sizeof(void*),1));}});goto
_LL588;}_LL588:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp913[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp914="too few arguments to struct";
_tag_arr(_tmp914,sizeof(char),_get_zero_arr_size(_tmp914,28));}),_tag_arr(
_tmp913,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp915=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp916;
void*_tmp917;struct Cyc_Absyn_PtrAtts _tmp918;struct Cyc_Absyn_Conref*_tmp919;
_LL58E: if(_tmp915 <= (void*)3  || *((int*)_tmp915)!= 4)goto _LL590;_tmp916=((struct
Cyc_Absyn_PointerType_struct*)_tmp915)->f1;_tmp917=(void*)_tmp916.elt_typ;
_tmp918=_tmp916.ptr_atts;_tmp919=_tmp918.bounds;_LL58F: {struct Cyc_Absyn_Conref*
_tmp91A=Cyc_Absyn_compress_conref(_tmp919);void*_tmp91B=(void*)(Cyc_Absyn_compress_conref(
_tmp91A))->v;void*_tmp91C;_LL593: if(_tmp91B <= (void*)1  || *((int*)_tmp91B)!= 0)
goto _LL595;_tmp91C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp91B)->f1;if((
int)_tmp91C != 0)goto _LL595;_LL594:*elt_typ_dest=_tmp917;return 1;_LL595: if((int)
_tmp91B != 0)goto _LL597;_LL596:(void*)(_tmp91A->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp91D=_cycalloc(sizeof(*_tmp91D));_tmp91D[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp91E;_tmp91E.tag=0;_tmp91E.f1=(void*)((void*)0);_tmp91E;});_tmp91D;})));*
elt_typ_dest=_tmp917;return 1;_LL597:;_LL598: return 0;_LL592:;}_LL590:;_LL591:
return 0;_LL58D:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp91F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp920;void*_tmp921;
struct Cyc_Absyn_PtrAtts _tmp922;struct Cyc_Absyn_Conref*_tmp923;_LL59A: if(_tmp91F
<= (void*)3  || *((int*)_tmp91F)!= 4)goto _LL59C;_tmp920=((struct Cyc_Absyn_PointerType_struct*)
_tmp91F)->f1;_tmp921=(void*)_tmp920.elt_typ;_tmp922=_tmp920.ptr_atts;_tmp923=
_tmp922.zero_term;_LL59B:*elt_typ_dest=_tmp921;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp923);_LL59C:;_LL59D: return 0;_LL599:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp967;_tmp967.f1=0;
_tmp967.f2=(void*)2;_tmp967;});void*_tmp924=(void*)e->r;struct _tuple1*_tmp925;
void*_tmp926;struct Cyc_Absyn_Exp*_tmp927;struct _tagged_arr*_tmp928;struct Cyc_Absyn_Exp*
_tmp929;struct _tagged_arr*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;struct Cyc_Absyn_Exp*
_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;_LL59F: if(*((int*)_tmp924)!= 1)goto _LL5A1;
_tmp925=((struct Cyc_Absyn_Var_e_struct*)_tmp924)->f1;_tmp926=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp924)->f2;_LL5A0: {void*_tmp92E=_tmp926;struct Cyc_Absyn_Vardecl*_tmp92F;
struct Cyc_Absyn_Vardecl*_tmp930;struct Cyc_Absyn_Vardecl*_tmp931;struct Cyc_Absyn_Vardecl*
_tmp932;_LL5AC: if((int)_tmp92E != 0)goto _LL5AE;_LL5AD: goto _LL5AF;_LL5AE: if(
_tmp92E <= (void*)1  || *((int*)_tmp92E)!= 1)goto _LL5B0;_LL5AF: return bogus_ans;
_LL5B0: if(_tmp92E <= (void*)1  || *((int*)_tmp92E)!= 0)goto _LL5B2;_tmp92F=((struct
Cyc_Absyn_Global_b_struct*)_tmp92E)->f1;_LL5B1: {void*_tmp933=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5B9: if(_tmp933 <= (void*)
3  || *((int*)_tmp933)!= 7)goto _LL5BB;_LL5BA: return({struct _tuple10 _tmp934;
_tmp934.f1=1;_tmp934.f2=(void*)2;_tmp934;});_LL5BB:;_LL5BC: return({struct
_tuple10 _tmp935;_tmp935.f1=(_tmp92F->tq).real_const;_tmp935.f2=(void*)2;_tmp935;});
_LL5B8:;}_LL5B2: if(_tmp92E <= (void*)1  || *((int*)_tmp92E)!= 3)goto _LL5B4;_tmp930=((
struct Cyc_Absyn_Local_b_struct*)_tmp92E)->f1;_LL5B3: {void*_tmp936=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5BE: if(_tmp936 <= (void*)
3  || *((int*)_tmp936)!= 7)goto _LL5C0;_LL5BF: return({struct _tuple10 _tmp937;
_tmp937.f1=1;_tmp937.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp930->rgn))->v;
_tmp937;});_LL5C0:;_LL5C1: _tmp930->escapes=1;return({struct _tuple10 _tmp938;
_tmp938.f1=(_tmp930->tq).real_const;_tmp938.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp930->rgn))->v;_tmp938;});_LL5BD:;}_LL5B4: if(_tmp92E <= (void*)1
 || *((int*)_tmp92E)!= 4)goto _LL5B6;_tmp931=((struct Cyc_Absyn_Pat_b_struct*)
_tmp92E)->f1;_LL5B5: _tmp932=_tmp931;goto _LL5B7;_LL5B6: if(_tmp92E <= (void*)1  || *((
int*)_tmp92E)!= 2)goto _LL5AB;_tmp932=((struct Cyc_Absyn_Param_b_struct*)_tmp92E)->f1;
_LL5B7: _tmp932->escapes=1;return({struct _tuple10 _tmp939;_tmp939.f1=(_tmp932->tq).real_const;
_tmp939.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp932->rgn))->v;_tmp939;});
_LL5AB:;}_LL5A1: if(*((int*)_tmp924)!= 23)goto _LL5A3;_tmp927=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp924)->f1;_tmp928=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp924)->f2;_LL5A2: {
void*_tmp93A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp927->topt))->v);struct Cyc_List_List*_tmp93B;struct Cyc_Absyn_AggrInfo _tmp93C;
void*_tmp93D;struct Cyc_Absyn_Aggrdecl**_tmp93E;struct Cyc_Absyn_Aggrdecl*_tmp93F;
_LL5C3: if(_tmp93A <= (void*)3  || *((int*)_tmp93A)!= 11)goto _LL5C5;_tmp93B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp93A)->f2;_LL5C4: {struct Cyc_Absyn_Aggrfield*
_tmp940=Cyc_Absyn_lookup_field(_tmp93B,_tmp928);if(_tmp940 != 0  && _tmp940->width
!= 0)return({struct _tuple10 _tmp941;_tmp941.f1=(_tmp940->tq).real_const;_tmp941.f2=(
Cyc_Tcutil_addressof_props(te,_tmp927)).f2;_tmp941;});return bogus_ans;}_LL5C5:
if(_tmp93A <= (void*)3  || *((int*)_tmp93A)!= 10)goto _LL5C7;_tmp93C=((struct Cyc_Absyn_AggrType_struct*)
_tmp93A)->f1;_tmp93D=(void*)_tmp93C.aggr_info;if(*((int*)_tmp93D)!= 1)goto _LL5C7;
_tmp93E=((struct Cyc_Absyn_KnownAggr_struct*)_tmp93D)->f1;_tmp93F=*_tmp93E;_LL5C6: {
struct Cyc_Absyn_Aggrfield*_tmp942=Cyc_Absyn_lookup_decl_field(_tmp93F,_tmp928);
if(_tmp942 != 0  && _tmp942->width != 0)return({struct _tuple10 _tmp943;_tmp943.f1=(
_tmp942->tq).real_const;_tmp943.f2=(Cyc_Tcutil_addressof_props(te,_tmp927)).f2;
_tmp943;});return bogus_ans;}_LL5C7:;_LL5C8: return bogus_ans;_LL5C2:;}_LL5A3: if(*((
int*)_tmp924)!= 24)goto _LL5A5;_tmp929=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp924)->f1;_tmp92A=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp924)->f2;_LL5A4: {
void*_tmp944=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp929->topt))->v);struct Cyc_Absyn_PtrInfo _tmp945;void*_tmp946;struct Cyc_Absyn_PtrAtts
_tmp947;void*_tmp948;_LL5CA: if(_tmp944 <= (void*)3  || *((int*)_tmp944)!= 4)goto
_LL5CC;_tmp945=((struct Cyc_Absyn_PointerType_struct*)_tmp944)->f1;_tmp946=(void*)
_tmp945.elt_typ;_tmp947=_tmp945.ptr_atts;_tmp948=(void*)_tmp947.rgn;_LL5CB: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp949=Cyc_Tcutil_compress(_tmp946);
struct Cyc_List_List*_tmp94A;struct Cyc_Absyn_AggrInfo _tmp94B;void*_tmp94C;struct
Cyc_Absyn_Aggrdecl**_tmp94D;struct Cyc_Absyn_Aggrdecl*_tmp94E;_LL5CF: if(_tmp949 <= (
void*)3  || *((int*)_tmp949)!= 11)goto _LL5D1;_tmp94A=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp949)->f2;_LL5D0: finfo=Cyc_Absyn_lookup_field(_tmp94A,_tmp92A);goto _LL5CE;
_LL5D1: if(_tmp949 <= (void*)3  || *((int*)_tmp949)!= 10)goto _LL5D3;_tmp94B=((
struct Cyc_Absyn_AggrType_struct*)_tmp949)->f1;_tmp94C=(void*)_tmp94B.aggr_info;
if(*((int*)_tmp94C)!= 1)goto _LL5D3;_tmp94D=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp94C)->f1;_tmp94E=*_tmp94D;_LL5D2: finfo=Cyc_Absyn_lookup_decl_field(_tmp94E,
_tmp92A);goto _LL5CE;_LL5D3:;_LL5D4: return bogus_ans;_LL5CE:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp94F;_tmp94F.f1=(finfo->tq).real_const;
_tmp94F.f2=_tmp948;_tmp94F;});return bogus_ans;}_LL5CC:;_LL5CD: return bogus_ans;
_LL5C9:;}_LL5A5: if(*((int*)_tmp924)!= 22)goto _LL5A7;_tmp92B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp924)->f1;_LL5A6: {void*_tmp950=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp92B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp951;struct Cyc_Absyn_Tqual
_tmp952;struct Cyc_Absyn_PtrAtts _tmp953;void*_tmp954;_LL5D6: if(_tmp950 <= (void*)3
 || *((int*)_tmp950)!= 4)goto _LL5D8;_tmp951=((struct Cyc_Absyn_PointerType_struct*)
_tmp950)->f1;_tmp952=_tmp951.elt_tq;_tmp953=_tmp951.ptr_atts;_tmp954=(void*)
_tmp953.rgn;_LL5D7: return({struct _tuple10 _tmp955;_tmp955.f1=_tmp952.real_const;
_tmp955.f2=_tmp954;_tmp955;});_LL5D8:;_LL5D9: return bogus_ans;_LL5D5:;}_LL5A7: if(*((
int*)_tmp924)!= 25)goto _LL5A9;_tmp92C=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp924)->f1;_tmp92D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp924)->f2;_LL5A8: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp92C->topt))->v);
void*_tmp956=t;struct Cyc_List_List*_tmp957;struct Cyc_Absyn_PtrInfo _tmp958;struct
Cyc_Absyn_Tqual _tmp959;struct Cyc_Absyn_PtrAtts _tmp95A;void*_tmp95B;struct Cyc_Absyn_ArrayInfo
_tmp95C;struct Cyc_Absyn_Tqual _tmp95D;_LL5DB: if(_tmp956 <= (void*)3  || *((int*)
_tmp956)!= 9)goto _LL5DD;_tmp957=((struct Cyc_Absyn_TupleType_struct*)_tmp956)->f1;
_LL5DC: {unsigned int _tmp95F;int _tmp960;struct _tuple7 _tmp95E=Cyc_Evexp_eval_const_uint_exp(
_tmp92D);_tmp95F=_tmp95E.f1;_tmp960=_tmp95E.f2;if(!_tmp960)return bogus_ans;{
struct _tuple4*_tmp961=Cyc_Absyn_lookup_tuple_field(_tmp957,(int)_tmp95F);if(
_tmp961 != 0)return({struct _tuple10 _tmp962;_tmp962.f1=((*_tmp961).f1).real_const;
_tmp962.f2=(Cyc_Tcutil_addressof_props(te,_tmp92C)).f2;_tmp962;});return
bogus_ans;}}_LL5DD: if(_tmp956 <= (void*)3  || *((int*)_tmp956)!= 4)goto _LL5DF;
_tmp958=((struct Cyc_Absyn_PointerType_struct*)_tmp956)->f1;_tmp959=_tmp958.elt_tq;
_tmp95A=_tmp958.ptr_atts;_tmp95B=(void*)_tmp95A.rgn;_LL5DE: return({struct
_tuple10 _tmp963;_tmp963.f1=_tmp959.real_const;_tmp963.f2=_tmp95B;_tmp963;});
_LL5DF: if(_tmp956 <= (void*)3  || *((int*)_tmp956)!= 7)goto _LL5E1;_tmp95C=((struct
Cyc_Absyn_ArrayType_struct*)_tmp956)->f1;_tmp95D=_tmp95C.tq;_LL5E0: return({
struct _tuple10 _tmp964;_tmp964.f1=_tmp95D.real_const;_tmp964.f2=(Cyc_Tcutil_addressof_props(
te,_tmp92C)).f2;_tmp964;});_LL5E1:;_LL5E2: return bogus_ans;_LL5DA:;}_LL5A9:;
_LL5AA:({void*_tmp965[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp966="unary & applied to non-lvalue";
_tag_arr(_tmp966,sizeof(char),_get_zero_arr_size(_tmp966,30));}),_tag_arr(
_tmp965,sizeof(void*),0));});return bogus_ans;_LL59E:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp968=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp969;void*_tmp96A;struct Cyc_Absyn_Tqual
_tmp96B;struct Cyc_Absyn_Conref*_tmp96C;_LL5E4: if(_tmp968 <= (void*)3  || *((int*)
_tmp968)!= 7)goto _LL5E6;_tmp969=((struct Cyc_Absyn_ArrayType_struct*)_tmp968)->f1;
_tmp96A=(void*)_tmp969.elt_type;_tmp96B=_tmp969.tq;_tmp96C=_tmp969.zero_term;
_LL5E5: {void*_tmp96E;struct _tuple10 _tmp96D=Cyc_Tcutil_addressof_props(te,e);
_tmp96E=_tmp96D.f2;return Cyc_Absyn_atb_typ(_tmp96A,_tmp96E,_tmp96B,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp96F=_cycalloc(sizeof(*_tmp96F));_tmp96F[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp970;_tmp970.tag=0;_tmp970.f1=e;_tmp970;});
_tmp96F;}),_tmp96C);}_LL5E6:;_LL5E7: return e_typ;_LL5E3:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp971=(void*)b->v;void*_tmp972;void*_tmp973;struct Cyc_Absyn_Exp*
_tmp974;void*_tmp975;_LL5E9: if(_tmp971 <= (void*)1  || *((int*)_tmp971)!= 0)goto
_LL5EB;_tmp972=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp971)->f1;if((int)
_tmp972 != 0)goto _LL5EB;_LL5EA: return;_LL5EB: if(_tmp971 <= (void*)1  || *((int*)
_tmp971)!= 0)goto _LL5ED;_tmp973=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp971)->f1;if(_tmp973 <= (void*)1  || *((int*)_tmp973)!= 0)goto _LL5ED;_tmp974=((
struct Cyc_Absyn_Upper_b_struct*)_tmp973)->f1;_LL5EC: {unsigned int _tmp977;int
_tmp978;struct _tuple7 _tmp976=Cyc_Evexp_eval_const_uint_exp(_tmp974);_tmp977=
_tmp976.f1;_tmp978=_tmp976.f2;if(_tmp978  && _tmp977 <= i)({struct Cyc_Int_pa_struct
_tmp97C;_tmp97C.tag=1;_tmp97C.f1=(unsigned int)((int)i);{struct Cyc_Int_pa_struct
_tmp97B;_tmp97B.tag=1;_tmp97B.f1=(unsigned int)((int)_tmp977);{void*_tmp979[2]={&
_tmp97B,& _tmp97C};Cyc_Tcutil_terr(loc,({const char*_tmp97A="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp97A,sizeof(char),_get_zero_arr_size(_tmp97A,39));}),_tag_arr(
_tmp979,sizeof(void*),2));}}});return;}_LL5ED: if(_tmp971 <= (void*)1  || *((int*)
_tmp971)!= 0)goto _LL5EF;_tmp975=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp971)->f1;if(_tmp975 <= (void*)1  || *((int*)_tmp975)!= 1)goto _LL5EF;_LL5EE:
return;_LL5EF: if((int)_tmp971 != 0)goto _LL5F1;_LL5F0:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp97D=_cycalloc(sizeof(*_tmp97D));_tmp97D[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp97E;_tmp97E.tag=0;_tmp97E.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp97F=_cycalloc(sizeof(*_tmp97F));
_tmp97F[0]=({struct Cyc_Absyn_Upper_b_struct _tmp980;_tmp980.tag=0;_tmp980.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp980;});_tmp97F;})));_tmp97E;});_tmp97D;})));return;_LL5F1:;_LL5F2:({
void*_tmp981[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp982="check_bound -- bad compressed conref";_tag_arr(_tmp982,
sizeof(char),_get_zero_arr_size(_tmp982,37));}),_tag_arr(_tmp981,sizeof(void*),0));});
_LL5E8:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp983=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp984;struct Cyc_Absyn_Exp*_tmp985;_LL5F4: if(_tmp983 <= (void*)1  || *((int*)
_tmp983)!= 0)goto _LL5F6;_tmp984=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp983)->f1;if(_tmp984 <= (void*)1  || *((int*)_tmp984)!= 0)goto _LL5F6;_tmp985=((
struct Cyc_Absyn_Upper_b_struct*)_tmp984)->f1;_LL5F5: {unsigned int _tmp987;int
_tmp988;struct _tuple7 _tmp986=Cyc_Evexp_eval_const_uint_exp(_tmp985);_tmp987=
_tmp986.f1;_tmp988=_tmp986.f2;return _tmp988  && _tmp987 == 1;}_LL5F6:;_LL5F7:
return 0;_LL5F3:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp989=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp98A;void*_tmp98B;struct Cyc_Absyn_Conref*_tmp98C;
struct Cyc_List_List*_tmp98D;struct Cyc_Absyn_AggrInfo _tmp98E;void*_tmp98F;struct
Cyc_Absyn_AggrInfo _tmp990;void*_tmp991;struct Cyc_Absyn_Aggrdecl**_tmp992;struct
Cyc_Absyn_Aggrdecl*_tmp993;struct Cyc_List_List*_tmp994;struct Cyc_List_List*
_tmp995;_LL5F9: if((int)_tmp989 != 0)goto _LL5FB;_LL5FA: goto _LL5FC;_LL5FB: if(
_tmp989 <= (void*)3  || *((int*)_tmp989)!= 5)goto _LL5FD;_LL5FC: goto _LL5FE;_LL5FD:
if((int)_tmp989 != 1)goto _LL5FF;_LL5FE: goto _LL600;_LL5FF: if(_tmp989 <= (void*)3
 || *((int*)_tmp989)!= 6)goto _LL601;_LL600: return 1;_LL601: if(_tmp989 <= (void*)3
 || *((int*)_tmp989)!= 12)goto _LL603;_LL602: goto _LL604;_LL603: if(_tmp989 <= (void*)
3  || *((int*)_tmp989)!= 13)goto _LL605;_LL604: return 0;_LL605: if(_tmp989 <= (void*)
3  || *((int*)_tmp989)!= 7)goto _LL607;_tmp98A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp989)->f1;_tmp98B=(void*)_tmp98A.elt_type;_tmp98C=_tmp98A.zero_term;_LL606:
return !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp98C)
 && Cyc_Tcutil_bits_only(_tmp98B);_LL607: if(_tmp989 <= (void*)3  || *((int*)
_tmp989)!= 9)goto _LL609;_tmp98D=((struct Cyc_Absyn_TupleType_struct*)_tmp989)->f1;
_LL608: for(0;_tmp98D != 0;_tmp98D=_tmp98D->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp98D->hd)).f2))return 0;}return 1;_LL609: if(_tmp989 <= (void*)3
 || *((int*)_tmp989)!= 10)goto _LL60B;_tmp98E=((struct Cyc_Absyn_AggrType_struct*)
_tmp989)->f1;_tmp98F=(void*)_tmp98E.aggr_info;if(*((int*)_tmp98F)!= 0)goto _LL60B;
_LL60A: return 0;_LL60B: if(_tmp989 <= (void*)3  || *((int*)_tmp989)!= 10)goto _LL60D;
_tmp990=((struct Cyc_Absyn_AggrType_struct*)_tmp989)->f1;_tmp991=(void*)_tmp990.aggr_info;
if(*((int*)_tmp991)!= 1)goto _LL60D;_tmp992=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp991)->f1;_tmp993=*_tmp992;_tmp994=_tmp990.targs;_LL60C: if(_tmp993->impl == 0)
return 0;{struct _RegionHandle _tmp996=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp996;_push_region(rgn);{struct Cyc_List_List*_tmp997=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp993->tvs,_tmp994);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp993->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp997,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp998=0;_npop_handler(0);return _tmp998;}}}{int _tmp999=1;
_npop_handler(0);return _tmp999;}};_pop_region(rgn);}_LL60D: if(_tmp989 <= (void*)3
 || *((int*)_tmp989)!= 11)goto _LL60F;_tmp995=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp989)->f2;_LL60E: for(0;_tmp995 != 0;_tmp995=_tmp995->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp995->hd)->type))return 0;}return 1;_LL60F:;
_LL610: return 0;_LL5F8:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp99A=(void*)e->r;struct _tuple1*_tmp99B;void*_tmp99C;struct Cyc_Absyn_Exp*
_tmp99D;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*_tmp99F;struct Cyc_Absyn_Exp*
_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;void*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A5;void*_tmp9A6;void*_tmp9A7;
struct Cyc_Absyn_Exp*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*
_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_List_List*_tmp9AC;struct Cyc_List_List*
_tmp9AD;struct Cyc_List_List*_tmp9AE;void*_tmp9AF;struct Cyc_List_List*_tmp9B0;
struct Cyc_List_List*_tmp9B1;struct Cyc_List_List*_tmp9B2;_LL612: if(*((int*)
_tmp99A)!= 0)goto _LL614;_LL613: goto _LL615;_LL614: if(*((int*)_tmp99A)!= 18)goto
_LL616;_LL615: goto _LL617;_LL616: if(*((int*)_tmp99A)!= 19)goto _LL618;_LL617: goto
_LL619;_LL618: if(*((int*)_tmp99A)!= 20)goto _LL61A;_LL619: goto _LL61B;_LL61A: if(*((
int*)_tmp99A)!= 21)goto _LL61C;_LL61B: goto _LL61D;_LL61C: if(*((int*)_tmp99A)!= 33)
goto _LL61E;_LL61D: goto _LL61F;_LL61E: if(*((int*)_tmp99A)!= 34)goto _LL620;_LL61F:
return 1;_LL620: if(*((int*)_tmp99A)!= 1)goto _LL622;_tmp99B=((struct Cyc_Absyn_Var_e_struct*)
_tmp99A)->f1;_tmp99C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp99A)->f2;_LL621: {
void*_tmp9B3=_tmp99C;struct Cyc_Absyn_Vardecl*_tmp9B4;_LL641: if(_tmp9B3 <= (void*)
1  || *((int*)_tmp9B3)!= 1)goto _LL643;_LL642: return 1;_LL643: if(_tmp9B3 <= (void*)1
 || *((int*)_tmp9B3)!= 0)goto _LL645;_tmp9B4=((struct Cyc_Absyn_Global_b_struct*)
_tmp9B3)->f1;_LL644: {void*_tmp9B5=Cyc_Tcutil_compress((void*)_tmp9B4->type);
_LL64A: if(_tmp9B5 <= (void*)3  || *((int*)_tmp9B5)!= 7)goto _LL64C;_LL64B: goto
_LL64D;_LL64C: if(_tmp9B5 <= (void*)3  || *((int*)_tmp9B5)!= 8)goto _LL64E;_LL64D:
return 1;_LL64E:;_LL64F: return var_okay;_LL649:;}_LL645: if((int)_tmp9B3 != 0)goto
_LL647;_LL646: return 0;_LL647:;_LL648: return var_okay;_LL640:;}_LL622: if(*((int*)
_tmp99A)!= 6)goto _LL624;_tmp99D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99A)->f1;
_tmp99E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99A)->f2;_tmp99F=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp99A)->f3;_LL623: return(Cyc_Tcutil_cnst_exp(te,0,_tmp99D) && Cyc_Tcutil_cnst_exp(
te,0,_tmp99E)) && Cyc_Tcutil_cnst_exp(te,0,_tmp99F);_LL624: if(*((int*)_tmp99A)!= 
9)goto _LL626;_tmp9A0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp99A)->f1;_tmp9A1=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp99A)->f2;_LL625: return Cyc_Tcutil_cnst_exp(
te,0,_tmp9A0) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A1);_LL626: if(*((int*)_tmp99A)!= 
13)goto _LL628;_tmp9A2=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp99A)->f1;
_LL627: _tmp9A3=_tmp9A2;goto _LL629;_LL628: if(*((int*)_tmp99A)!= 14)goto _LL62A;
_tmp9A3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp99A)->f1;_LL629: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp9A3);_LL62A: if(*((int*)_tmp99A)!= 15)goto _LL62C;_tmp9A4=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp99A)->f1;_tmp9A5=((struct Cyc_Absyn_Cast_e_struct*)
_tmp99A)->f2;_tmp9A6=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp99A)->f4;if((
int)_tmp9A6 != 1)goto _LL62C;_LL62B: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9A5);
_LL62C: if(*((int*)_tmp99A)!= 15)goto _LL62E;_tmp9A7=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp99A)->f1;_tmp9A8=((struct Cyc_Absyn_Cast_e_struct*)_tmp99A)->f2;_LL62D: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9A8);_LL62E: if(*((int*)_tmp99A)!= 16)goto
_LL630;_tmp9A9=((struct Cyc_Absyn_Address_e_struct*)_tmp99A)->f1;_LL62F: return Cyc_Tcutil_cnst_exp(
te,1,_tmp9A9);_LL630: if(*((int*)_tmp99A)!= 29)goto _LL632;_tmp9AA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp99A)->f2;_tmp9AB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp99A)->f3;
_LL631: return Cyc_Tcutil_cnst_exp(te,0,_tmp9AA) && Cyc_Tcutil_cnst_exp(te,0,
_tmp9AB);_LL632: if(*((int*)_tmp99A)!= 28)goto _LL634;_tmp9AC=((struct Cyc_Absyn_Array_e_struct*)
_tmp99A)->f1;_LL633: _tmp9AD=_tmp9AC;goto _LL635;_LL634: if(*((int*)_tmp99A)!= 31)
goto _LL636;_tmp9AD=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp99A)->f2;_LL635:
_tmp9AE=_tmp9AD;goto _LL637;_LL636: if(*((int*)_tmp99A)!= 30)goto _LL638;_tmp9AE=((
struct Cyc_Absyn_Struct_e_struct*)_tmp99A)->f3;_LL637: for(0;_tmp9AE != 0;_tmp9AE=
_tmp9AE->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp9AE->hd)).f2))
return 0;}return 1;_LL638: if(*((int*)_tmp99A)!= 3)goto _LL63A;_tmp9AF=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp99A)->f1;_tmp9B0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp99A)->f2;_LL639: _tmp9B1=_tmp9B0;goto _LL63B;_LL63A: if(*((int*)_tmp99A)!= 26)
goto _LL63C;_tmp9B1=((struct Cyc_Absyn_Tuple_e_struct*)_tmp99A)->f1;_LL63B: _tmp9B2=
_tmp9B1;goto _LL63D;_LL63C: if(*((int*)_tmp99A)!= 32)goto _LL63E;_tmp9B2=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp99A)->f1;_LL63D: for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9B2->hd))return 0;}return 1;
_LL63E:;_LL63F: return 0;_LL611:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9B6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9B7;
struct Cyc_Absyn_PtrAtts _tmp9B8;struct Cyc_Absyn_Conref*_tmp9B9;struct Cyc_Absyn_Conref*
_tmp9BA;struct Cyc_Absyn_ArrayInfo _tmp9BB;void*_tmp9BC;struct Cyc_List_List*
_tmp9BD;struct Cyc_Absyn_AggrInfo _tmp9BE;void*_tmp9BF;struct Cyc_List_List*_tmp9C0;
struct Cyc_List_List*_tmp9C1;_LL651: if((int)_tmp9B6 != 0)goto _LL653;_LL652: goto
_LL654;_LL653: if(_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 5)goto _LL655;_LL654:
goto _LL656;_LL655: if((int)_tmp9B6 != 1)goto _LL657;_LL656: goto _LL658;_LL657: if(
_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 6)goto _LL659;_LL658: return 1;_LL659: if(
_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 4)goto _LL65B;_tmp9B7=((struct Cyc_Absyn_PointerType_struct*)
_tmp9B6)->f1;_tmp9B8=_tmp9B7.ptr_atts;_tmp9B9=_tmp9B8.nullable;_tmp9BA=_tmp9B8.bounds;
_LL65A: {void*_tmp9C2=(void*)(Cyc_Absyn_compress_conref(_tmp9BA))->v;void*
_tmp9C3;_LL66A: if(_tmp9C2 <= (void*)1  || *((int*)_tmp9C2)!= 0)goto _LL66C;_tmp9C3=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9C2)->f1;if((int)_tmp9C3 != 0)goto
_LL66C;_LL66B: return 1;_LL66C: if(_tmp9C2 <= (void*)1  || *((int*)_tmp9C2)!= 0)goto
_LL66E;_LL66D: {void*_tmp9C4=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9B9))->v;int _tmp9C5;_LL671: if(_tmp9C4 <= (void*)
1  || *((int*)_tmp9C4)!= 0)goto _LL673;_tmp9C5=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9C4)->f1;_LL672: return _tmp9C5;_LL673:;_LL674: return 0;_LL670:;}_LL66E:;_LL66F:
return 0;_LL669:;}_LL65B: if(_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 7)goto _LL65D;
_tmp9BB=((struct Cyc_Absyn_ArrayType_struct*)_tmp9B6)->f1;_tmp9BC=(void*)_tmp9BB.elt_type;
_LL65C: return Cyc_Tcutil_supports_default(_tmp9BC);_LL65D: if(_tmp9B6 <= (void*)3
 || *((int*)_tmp9B6)!= 9)goto _LL65F;_tmp9BD=((struct Cyc_Absyn_TupleType_struct*)
_tmp9B6)->f1;_LL65E: for(0;_tmp9BD != 0;_tmp9BD=_tmp9BD->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmp9BD->hd)).f2))return 0;}return 1;_LL65F: if(_tmp9B6 <= (void*)3
 || *((int*)_tmp9B6)!= 10)goto _LL661;_tmp9BE=((struct Cyc_Absyn_AggrType_struct*)
_tmp9B6)->f1;_tmp9BF=(void*)_tmp9BE.aggr_info;_tmp9C0=_tmp9BE.targs;_LL660: {
struct Cyc_Absyn_Aggrdecl*_tmp9C6=Cyc_Absyn_get_known_aggrdecl(_tmp9BF);if(
_tmp9C6->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C6->impl))->exist_vars
!= 0)return 0;return Cyc_Tcutil_fields_support_default(_tmp9C6->tvs,_tmp9C0,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C6->impl))->fields);}_LL661: if(
_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 11)goto _LL663;_tmp9C1=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9B6)->f2;_LL662: return Cyc_Tcutil_fields_support_default(0,0,_tmp9C1);_LL663:
if(_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 13)goto _LL665;_LL664: goto _LL666;
_LL665: if(_tmp9B6 <= (void*)3  || *((int*)_tmp9B6)!= 12)goto _LL667;_LL666: return 1;
_LL667:;_LL668: return 0;_LL650:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp9C7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C7;
_push_region(rgn);{struct Cyc_List_List*_tmp9C8=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp9C8,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp9C9=0;_npop_handler(0);return _tmp9C9;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_admits_zero(void*t){void*_tmp9CA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp9CB;struct Cyc_Absyn_PtrAtts _tmp9CC;struct Cyc_Absyn_Conref*_tmp9CD;struct Cyc_Absyn_Conref*
_tmp9CE;_LL676: if(_tmp9CA <= (void*)3  || *((int*)_tmp9CA)!= 5)goto _LL678;_LL677:
goto _LL679;_LL678: if((int)_tmp9CA != 1)goto _LL67A;_LL679: goto _LL67B;_LL67A: if(
_tmp9CA <= (void*)3  || *((int*)_tmp9CA)!= 6)goto _LL67C;_LL67B: return 1;_LL67C: if(
_tmp9CA <= (void*)3  || *((int*)_tmp9CA)!= 4)goto _LL67E;_tmp9CB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9CA)->f1;_tmp9CC=_tmp9CB.ptr_atts;_tmp9CD=_tmp9CC.nullable;_tmp9CE=_tmp9CC.bounds;
_LL67D: {void*_tmp9CF=(void*)(Cyc_Absyn_compress_conref(_tmp9CE))->v;void*
_tmp9D0;_LL681: if(_tmp9CF <= (void*)1  || *((int*)_tmp9CF)!= 0)goto _LL683;_tmp9D0=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9CF)->f1;if((int)_tmp9D0 != 0)goto
_LL683;_LL682: return 0;_LL683: if(_tmp9CF <= (void*)1  || *((int*)_tmp9CF)!= 0)goto
_LL685;_LL684: {void*_tmp9D1=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9CD))->v;int _tmp9D2;_LL688: if(_tmp9D1 <= (void*)
1  || *((int*)_tmp9D1)!= 0)goto _LL68A;_tmp9D2=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D1)->f1;_LL689: return _tmp9D2;_LL68A:;_LL68B: return 0;_LL687:;}_LL685:;_LL686:
return 0;_LL680:;}_LL67E:;_LL67F: return 0;_LL675:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9D4;void*
_tmp9D5;struct Cyc_Absyn_FnInfo _tmp9D6;struct Cyc_List_List*_tmp9D7;_LL68D: if(
_tmp9D3 <= (void*)3  || *((int*)_tmp9D3)!= 4)goto _LL68F;_tmp9D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D3)->f1;_tmp9D5=(void*)_tmp9D4.elt_typ;_LL68E: return Cyc_Tcutil_is_noreturn(
_tmp9D5);_LL68F: if(_tmp9D3 <= (void*)3  || *((int*)_tmp9D3)!= 8)goto _LL691;_tmp9D6=((
struct Cyc_Absyn_FnType_struct*)_tmp9D3)->f1;_tmp9D7=_tmp9D6.attributes;_LL690:
for(0;_tmp9D7 != 0;_tmp9D7=_tmp9D7->tl){void*_tmp9D8=(void*)_tmp9D7->hd;_LL694:
if((int)_tmp9D8 != 3)goto _LL696;_LL695: return 1;_LL696:;_LL697: continue;_LL693:;}
goto _LL68C;_LL691:;_LL692: goto _LL68C;_LL68C:;}return 0;}
