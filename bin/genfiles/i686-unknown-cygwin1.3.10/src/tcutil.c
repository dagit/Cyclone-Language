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
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;
void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;
};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{
int tag;void*f1;};struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*zero_term;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual
elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
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
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
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
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Position_Segment*
_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*
_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*
_tmp7D;struct Cyc_Absyn_FnInfo _tmp7E;struct Cyc_List_List*_tmp7F;struct Cyc_Core_Opt*
_tmp80;void*_tmp81;struct Cyc_List_List*_tmp82;int _tmp83;struct Cyc_Absyn_VarargInfo*
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;struct Cyc_Absyn_AggrInfo _tmp88;void*_tmp89;void*_tmp8A;struct _tuple1*
_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_Absyn_AggrInfo _tmp8D;void*_tmp8E;
struct Cyc_Absyn_Aggrdecl**_tmp8F;struct Cyc_List_List*_tmp90;void*_tmp91;struct
Cyc_List_List*_tmp92;struct _tuple1*_tmp93;struct Cyc_Absyn_Enumdecl*_tmp94;struct
Cyc_List_List*_tmp95;void*_tmp96;int _tmp97;void*_tmp98;void*_tmp99;struct _tuple1*
_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Typedefdecl*_tmp9C;void*_tmp9D;
struct Cyc_List_List*_tmp9E;void*_tmp9F;_LL1A: if((int)_tmp64 != 0)goto _LL1C;_LL1B:
goto _LL1D;_LL1C: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 0)goto _LL1E;_LL1D:
return t;_LL1E: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 1)goto _LL20;_tmp65=((
struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F: return(void*)({struct Cyc_Absyn_VarType_struct*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_VarType_struct
_tmpA1;_tmpA1.tag=1;_tmpA1.f1=Cyc_Tcutil_copy_tvar(_tmp65);_tmpA1;});_tmpA0;});
_LL20: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 2)goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)
_tmp64)->f1;_tmp67=(void*)_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)
_tmp66.rgn;_LL21: return(void*)({struct Cyc_Absyn_TunionType_struct*_tmpA2=
_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_TunionType_struct _tmpA3;
_tmpA3.tag=2;_tmpA3.f1=({struct Cyc_Absyn_TunionInfo _tmpA4;_tmpA4.tunion_info=(
void*)_tmp67;_tmpA4.targs=Cyc_Tcutil_copy_types(_tmp68);_tmpA4.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp69);_tmpA4;});_tmpA3;});_tmpA2;});_LL22: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 3)goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;
_tmp6B=(void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct
Cyc_Absyn_TunionFieldType_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmpA6;_tmpA6.tag=3;_tmpA6.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmpA7;_tmpA7.field_info=(void*)_tmp6B;_tmpA7.targs=Cyc_Tcutil_copy_types(_tmp6C);
_tmpA7;});_tmpA6;});_tmpA5;});_LL24: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 4)
goto _LL26;_tmp6D=((struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)
_tmp6D.elt_typ;_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;
_tmp72=_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_tmp75=
_tmp70.loc;_LL25: {void*_tmpA8=Cyc_Tcutil_copy_type(_tmp6E);void*_tmpA9=Cyc_Tcutil_copy_type(
_tmp71);struct Cyc_Absyn_Conref*_tmpAA=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp72);struct Cyc_Absyn_Tqual _tmpAB=_tmp6F;struct Cyc_Absyn_Conref*
_tmpAC=Cyc_Tcutil_copy_conref(_tmp73);struct Cyc_Absyn_Conref*_tmpAD=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({
struct Cyc_Absyn_PointerType_struct _tmpAF;_tmpAF.tag=4;_tmpAF.f1=({struct Cyc_Absyn_PtrInfo
_tmpB0;_tmpB0.elt_typ=(void*)_tmpA8;_tmpB0.elt_tq=_tmpAB;_tmpB0.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB1;_tmpB1.rgn=(void*)_tmpA9;_tmpB1.nullable=_tmpAA;
_tmpB1.bounds=_tmpAC;_tmpB1.zero_term=_tmpAD;_tmpB1.loc=_tmp75;_tmpB1;});_tmpB0;});
_tmpAF;});_tmpAE;});}_LL26: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 5)goto _LL28;
_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f1;_tmp77=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27: return(void*)({struct Cyc_Absyn_IntType_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_IntType_struct
_tmpB3;_tmpB3.tag=5;_tmpB3.f1=(void*)_tmp76;_tmpB3.f2=(void*)_tmp77;_tmpB3;});
_tmpB2;});_LL28: if((int)_tmp64 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 6)goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)
_tmp64)->f1;_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB4=
_cycalloc_atomic(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpB5;_tmpB5.tag=6;_tmpB5.f1=_tmp78;_tmpB5;});_tmpB4;});_LL2C: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((struct Cyc_Absyn_ArrayType_struct*)
_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;
_tmp7D=_tmp79.zero_term;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB7;_tmpB7.tag=7;_tmpB7.f1=({struct Cyc_Absyn_ArrayInfo _tmpB8;_tmpB8.elt_type=(
void*)Cyc_Tcutil_copy_type(_tmp7A);_tmpB8.tq=_tmp7B;_tmpB8.num_elts=_tmp7C;
_tmpB8.zero_term=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp7D);_tmpB8;});_tmpB7;});_tmpB6;});_LL2E: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 8)goto _LL30;_tmp7E=((struct Cyc_Absyn_FnType_struct*)_tmp64)->f1;_tmp7F=
_tmp7E.tvars;_tmp80=_tmp7E.effect;_tmp81=(void*)_tmp7E.ret_typ;_tmp82=_tmp7E.args;
_tmp83=_tmp7E.c_varargs;_tmp84=_tmp7E.cyc_varargs;_tmp85=_tmp7E.rgn_po;_tmp86=
_tmp7E.attributes;_LL2F: {struct Cyc_List_List*_tmpB9=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp7F);struct Cyc_Core_Opt*_tmpBA=_tmp80 == 0?0:({struct Cyc_Core_Opt*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->v=(void*)Cyc_Tcutil_copy_type((void*)
_tmp80->v);_tmpC4;});void*_tmpBB=Cyc_Tcutil_copy_type(_tmp81);struct Cyc_List_List*
_tmpBC=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp82);int _tmpBD=_tmp83;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp84 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp84;cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpBE=_cycalloc(sizeof(*_tmpBE));
_tmpBE->name=cv->name;_tmpBE->tq=cv->tq;_tmpBE->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmpBE->inject=cv->inject;_tmpBE;});}{struct Cyc_List_List*_tmpBF=((
struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp85);struct Cyc_List_List*_tmpC0=_tmp86;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));
_tmpC1[0]=({struct Cyc_Absyn_FnType_struct _tmpC2;_tmpC2.tag=8;_tmpC2.f1=({struct
Cyc_Absyn_FnInfo _tmpC3;_tmpC3.tvars=_tmpB9;_tmpC3.effect=_tmpBA;_tmpC3.ret_typ=(
void*)_tmpBB;_tmpC3.args=_tmpBC;_tmpC3.c_varargs=_tmpBD;_tmpC3.cyc_varargs=
cyc_varargs2;_tmpC3.rgn_po=_tmpBF;_tmpC3.attributes=_tmpC0;_tmpC3;});_tmpC2;});
_tmpC1;});}}_LL30: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 9)goto _LL32;_tmp87=((
struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_TupleType_struct
_tmpC6;_tmpC6.tag=9;_tmpC6.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(
struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp87);
_tmpC6;});_tmpC5;});_LL32: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 10)goto _LL34;
_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;_tmp89=(void*)_tmp88.aggr_info;
if(*((int*)_tmp89)!= 0)goto _LL34;_tmp8A=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp89)->f1;_tmp8B=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp89)->f2;_tmp8C=
_tmp88.targs;_LL33: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC7=
_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_AggrType_struct _tmpC8;
_tmpC8.tag=10;_tmpC8.f1=({struct Cyc_Absyn_AggrInfo _tmpC9;_tmpC9.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));
_tmpCA[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(
void*)_tmp8A;_tmpCB.f2=_tmp8B;_tmpCB;});_tmpCA;}));_tmpC9.targs=Cyc_Tcutil_copy_types(
_tmp8C);_tmpC9;});_tmpC8;});_tmpC7;});_LL34: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 10)goto _LL36;_tmp8D=((struct Cyc_Absyn_AggrType_struct*)_tmp64)->f1;
_tmp8E=(void*)_tmp8D.aggr_info;if(*((int*)_tmp8E)!= 1)goto _LL36;_tmp8F=((struct
Cyc_Absyn_KnownAggr_struct*)_tmp8E)->f1;_tmp90=_tmp8D.targs;_LL35: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_AggrType_struct _tmpCD;_tmpCD.tag=10;_tmpCD.f1=({struct Cyc_Absyn_AggrInfo
_tmpCE;_tmpCE.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpD0;
_tmpD0.tag=1;_tmpD0.f1=_tmp8F;_tmpD0;});_tmpCF;}));_tmpCE.targs=Cyc_Tcutil_copy_types(
_tmp90);_tmpCE;});_tmpCD;});_tmpCC;});_LL36: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 11)goto _LL38;_tmp91=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp92=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD1=_cycalloc(sizeof(*
_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD2;_tmpD2.tag=11;
_tmpD2.f1=(void*)_tmp91;_tmpD2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp92);_tmpD2;});_tmpD1;});_LL38: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 12)
goto _LL3A;_tmp93=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp94=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD4;_tmpD4.tag=12;_tmpD4.f1=_tmp93;_tmpD4.f2=_tmp94;_tmpD4;});_tmpD3;});_LL3A:
if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 13)goto _LL3C;_tmp95=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD6;
_tmpD6.tag=13;_tmpD6.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp95);_tmpD6;});_tmpD5;});_LL3C: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 14)
goto _LL3E;_tmp96=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));
_tmpD7[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD8;_tmpD8.tag=14;_tmpD8.f1=(
void*)Cyc_Tcutil_copy_type(_tmp96);_tmpD8;});_tmpD7;});_LL3E: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 18)goto _LL40;_tmp97=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpD9=
_cycalloc_atomic(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDA;_tmpDA.tag=18;_tmpDA.f1=_tmp97;_tmpDA;});_tmpD9;});_LL40: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 17)goto _LL42;_tmp98=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDB=_cycalloc(
sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_TagType_struct _tmpDC;_tmpDC.tag=17;
_tmpDC.f1=(void*)Cyc_Tcutil_copy_type(_tmp98);_tmpDC;});_tmpDB;});_LL42: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 15)goto _LL44;_tmp99=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDD=
_cycalloc(sizeof(*_tmpDD));_tmpDD[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDE;_tmpDE.tag=15;_tmpDE.f1=(void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDE;});
_tmpDD;});_LL44: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 16)goto _LL46;_tmp9A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpDF=_cycalloc(sizeof(*
_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE0;_tmpE0.tag=16;
_tmpE0.f1=_tmp9A;_tmpE0.f2=Cyc_Tcutil_copy_types(_tmp9B);_tmpE0.f3=_tmp9C;_tmpE0.f4=
0;_tmpE0;});_tmpDF;});_LL46: if((int)_tmp64 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 19)goto _LL4A;_tmp9D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE1=
_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE2;
_tmpE2.tag=19;_tmpE2.f1=(void*)Cyc_Tcutil_copy_type(_tmp9D);_tmpE2;});_tmpE1;});
_LL4A: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 20)goto _LL4C;_tmp9E=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE3=_cycalloc(
sizeof(*_tmpE3));_tmpE3[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE4;_tmpE4.tag=20;
_tmpE4.f1=Cyc_Tcutil_copy_types(_tmp9E);_tmpE4;});_tmpE3;});_LL4C: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 21)goto _LL19;_tmp9F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpE5=_cycalloc(
sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpE6;_tmpE6.tag=21;
_tmpE6.f1=(void*)Cyc_Tcutil_copy_type(_tmp9F);_tmpE6;});_tmpE5;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpE8=({struct _tuple6 _tmpE7;
_tmpE7.f1=k1;_tmpE7.f2=k2;_tmpE7;});void*_tmpE9;void*_tmpEA;void*_tmpEB;void*
_tmpEC;void*_tmpED;void*_tmpEE;_LL4F: _tmpE9=_tmpE8.f1;if((int)_tmpE9 != 2)goto
_LL51;_tmpEA=_tmpE8.f2;if((int)_tmpEA != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpEB=
_tmpE8.f1;if((int)_tmpEB != 2)goto _LL53;_tmpEC=_tmpE8.f2;if((int)_tmpEC != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpED=_tmpE8.f1;if((int)_tmpED != 1)goto _LL55;_tmpEE=
_tmpE8.f2;if((int)_tmpEE != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEF=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpF0;void*_tmpF1;_LL58: if(*((int*)_tmpEF)!= 0)goto _LL5A;
_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEF)->f1;_LL59: return _tmpF0;
_LL5A: if(*((int*)_tmpEF)!= 2)goto _LL5C;_tmpF1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEF)->f2;_LL5B: return _tmpF1;_LL5C:;_LL5D:({void*_tmpF2[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF3="kind not suitably constrained!";
_tag_arr(_tmpF3,sizeof(char),_get_zero_arr_size(_tmpF3,31));}),_tag_arr(_tmpF2,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF4=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpF5;struct Cyc_Core_Opt*_tmpF6;struct Cyc_Absyn_Tvar*
_tmpF7;void*_tmpF8;struct Cyc_Absyn_TunionFieldInfo _tmpF9;void*_tmpFA;struct Cyc_Absyn_Tunionfield*
_tmpFB;struct Cyc_Absyn_TunionFieldInfo _tmpFC;void*_tmpFD;struct Cyc_Absyn_Enumdecl*
_tmpFE;struct Cyc_Absyn_AggrInfo _tmpFF;void*_tmp100;struct Cyc_Absyn_AggrInfo
_tmp101;void*_tmp102;struct Cyc_Absyn_Aggrdecl**_tmp103;struct Cyc_Absyn_Aggrdecl*
_tmp104;struct Cyc_Absyn_Aggrdecl _tmp105;struct Cyc_Absyn_AggrdeclImpl*_tmp106;
struct Cyc_Absyn_Enumdecl*_tmp107;struct Cyc_Absyn_PtrInfo _tmp108;struct Cyc_Absyn_Typedefdecl*
_tmp109;_LL5F: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 0)goto _LL61;_tmpF5=((
struct Cyc_Absyn_Evar_struct*)_tmpF4)->f1;_tmpF6=((struct Cyc_Absyn_Evar_struct*)
_tmpF4)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF5))->v;
_LL61: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 1)goto _LL63;_tmpF7=((struct Cyc_Absyn_VarType_struct*)
_tmpF4)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpF7);_LL63: if((int)_tmpF4 != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 5)
goto _LL67;_tmpF8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF4)->f2;_LL66:
return _tmpF8 == (void*)((void*)2)?(void*)2:(void*)1;_LL67: if((int)_tmpF4 != 1)goto
_LL69;_LL68: goto _LL6A;_LL69: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 6)goto
_LL6B;_LL6A: goto _LL6C;_LL6B: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 8)goto
_LL6D;_LL6C: return(void*)1;_LL6D: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 15)
goto _LL6F;_LL6E: return(void*)2;_LL6F: if((int)_tmpF4 != 2)goto _LL71;_LL70: return(
void*)3;_LL71: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 2)goto _LL73;_LL72: return(
void*)2;_LL73: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 3)goto _LL75;_tmpF9=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1;_tmpFA=(void*)_tmpF9.field_info;
if(*((int*)_tmpFA)!= 1)goto _LL75;_tmpFB=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpFA)->f2;_LL74: if(_tmpFB->typs == 0)return(void*)2;else{return(void*)1;}_LL75:
if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 3)goto _LL77;_tmpFC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF4)->f1;_tmpFD=(void*)_tmpFC.field_info;if(*((int*)_tmpFD)!= 0)goto _LL77;
_LL76:({void*_tmp10A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp10B="typ_kind: Unresolved TunionFieldType";
_tag_arr(_tmp10B,sizeof(char),_get_zero_arr_size(_tmp10B,37));}),_tag_arr(
_tmp10A,sizeof(void*),0));});_LL77: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 12)
goto _LL79;_tmpFE=((struct Cyc_Absyn_EnumType_struct*)_tmpF4)->f2;if(_tmpFE != 0)
goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 10)
goto _LL7B;_tmpFF=((struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1;_tmp100=(void*)
_tmpFF.aggr_info;if(*((int*)_tmp100)!= 0)goto _LL7B;_LL7A: return(void*)0;_LL7B:
if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 10)goto _LL7D;_tmp101=((struct Cyc_Absyn_AggrType_struct*)
_tmpF4)->f1;_tmp102=(void*)_tmp101.aggr_info;if(*((int*)_tmp102)!= 1)goto _LL7D;
_tmp103=((struct Cyc_Absyn_KnownAggr_struct*)_tmp102)->f1;_tmp104=*_tmp103;
_tmp105=*_tmp104;_tmp106=_tmp105.impl;_LL7C: return _tmp106 == 0?(void*)0:(void*)1;
_LL7D: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 11)goto _LL7F;_LL7E: goto _LL80;
_LL7F: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 13)goto _LL81;_LL80: return(void*)
1;_LL81: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 12)goto _LL83;_tmp107=((struct
Cyc_Absyn_EnumType_struct*)_tmpF4)->f2;_LL82: if(_tmp107->fields == 0)return(void*)
0;else{return(void*)1;}_LL83: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 4)goto
_LL85;_tmp108=((struct Cyc_Absyn_PointerType_struct*)_tmpF4)->f1;_LL84: {void*
_tmp10C=(void*)(Cyc_Absyn_compress_conref((_tmp108.ptr_atts).bounds))->v;void*
_tmp10D;void*_tmp10E;void*_tmp10F;_LL98: if((int)_tmp10C != 0)goto _LL9A;_LL99: goto
_LL9B;_LL9A: if(_tmp10C <= (void*)1  || *((int*)_tmp10C)!= 0)goto _LL9C;_tmp10D=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10C)->f1;if((int)_tmp10D != 0)goto
_LL9C;_LL9B: return(void*)1;_LL9C: if(_tmp10C <= (void*)1  || *((int*)_tmp10C)!= 0)
goto _LL9E;_tmp10E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10C)->f1;if(
_tmp10E <= (void*)1  || *((int*)_tmp10E)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(
_tmp10C <= (void*)1  || *((int*)_tmp10C)!= 0)goto _LLA0;_tmp10F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10C)->f1;if(_tmp10F <= (void*)1  || *((int*)_tmp10F)!= 1)goto _LLA0;_LL9F:
return(void*)2;_LLA0: if(_tmp10C <= (void*)1  || *((int*)_tmp10C)!= 1)goto _LL97;
_LLA1:({void*_tmp110[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp111="typ_kind: forward constr in ptr bounds";
_tag_arr(_tmp111,sizeof(char),_get_zero_arr_size(_tmp111,39));}),_tag_arr(
_tmp110,sizeof(void*),0));});_LL97:;}_LL85: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 14)goto _LL87;_LL86: return(void*)2;_LL87: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 18)goto _LL89;_LL88: return(void*)5;_LL89: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 17)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 7)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 9)goto _LL8F;_LL8E: return(void*)1;_LL8F: if(_tmpF4 <= (void*)3  || *((int*)
_tmpF4)!= 16)goto _LL91;_tmp109=((struct Cyc_Absyn_TypedefType_struct*)_tmpF4)->f3;
_LL90: if(_tmp109 == 0  || _tmp109->kind == 0)({struct Cyc_String_pa_struct _tmp114;
_tmp114.tag=0;_tmp114.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp112[1]={& _tmp114};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp113="typ_kind: typedef found: %s";_tag_arr(
_tmp113,sizeof(char),_get_zero_arr_size(_tmp113,28));}),_tag_arr(_tmp112,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp109->kind))->v;
_LL91: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 19)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 20)goto _LL95;_LL94: goto _LL96;
_LL95: if(_tmpF4 <= (void*)3  || *((int*)_tmpF4)!= 21)goto _LL5E;_LL96: return(void*)
4;_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp115;
_push_handler(& _tmp115);{int _tmp117=0;if(setjmp(_tmp115.handler))_tmp117=1;if(!
_tmp117){Cyc_Tcutil_unify_it(t1,t2);{int _tmp118=1;_npop_handler(0);return _tmp118;};
_pop_handler();}else{void*_tmp116=(void*)_exn_thrown;void*_tmp11A=_tmp116;_LLA3:
if(_tmp11A != Cyc_Tcutil_Unify)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(
_tmp11A);_LLA2:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp11B=t;struct Cyc_Absyn_Tvar*_tmp11C;struct Cyc_Core_Opt*_tmp11D;
struct Cyc_Core_Opt*_tmp11E;struct Cyc_Core_Opt**_tmp11F;struct Cyc_Absyn_PtrInfo
_tmp120;struct Cyc_Absyn_ArrayInfo _tmp121;void*_tmp122;struct Cyc_Absyn_FnInfo
_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Core_Opt*_tmp125;void*_tmp126;
struct Cyc_List_List*_tmp127;int _tmp128;struct Cyc_Absyn_VarargInfo*_tmp129;struct
Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12C;
struct Cyc_Absyn_TunionInfo _tmp12D;struct Cyc_List_List*_tmp12E;void*_tmp12F;
struct Cyc_List_List*_tmp130;struct Cyc_Absyn_TunionFieldInfo _tmp131;struct Cyc_List_List*
_tmp132;struct Cyc_Absyn_AggrInfo _tmp133;struct Cyc_List_List*_tmp134;struct Cyc_List_List*
_tmp135;void*_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;struct
Cyc_List_List*_tmp13B;_LLA8: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 1)goto
_LLAA;_tmp11C=((struct Cyc_Absyn_VarType_struct*)_tmp11B)->f1;_LLA9: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp11C)){Cyc_Tcutil_failure_reason=({
const char*_tmp13C="(type variable would escape scope)";_tag_arr(_tmp13C,sizeof(
char),_get_zero_arr_size(_tmp13C,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLA7;_LLAA: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 0)goto _LLAC;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11B)->f2;_tmp11E=((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f4;_tmp11F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11B)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13D="(occurs check)";
_tag_arr(_tmp13D,sizeof(char),_get_zero_arr_size(_tmp13D,15));});(int)_throw((
void*)Cyc_Tcutil_Unify);}else{if(_tmp11D != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)
_tmp11D->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp11F))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp13E=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp11F))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp13E=({struct Cyc_List_List*_tmp13F=
_cycalloc(sizeof(*_tmp13F));_tmp13F->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp13F->tl=
_tmp13E;_tmp13F;});}}*_tmp11F=({struct Cyc_Core_Opt*_tmp140=_cycalloc(sizeof(*
_tmp140));_tmp140->v=_tmp13E;_tmp140;});}}}goto _LLA7;_LLAC: if(_tmp11B <= (void*)3
 || *((int*)_tmp11B)!= 4)goto _LLAE;_tmp120=((struct Cyc_Absyn_PointerType_struct*)
_tmp11B)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp120.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp120.ptr_atts).rgn);{void*_tmp141=(void*)(Cyc_Absyn_compress_conref((
_tmp120.ptr_atts).bounds))->v;void*_tmp142;void*_tmp143;_LLCD: if(_tmp141 <= (void*)
1  || *((int*)_tmp141)!= 0)goto _LLCF;_tmp142=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp141)->f1;if(_tmp142 <= (void*)1  || *((int*)_tmp142)!= 1)goto _LLCF;_tmp143=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp142)->f1;_LLCE: Cyc_Tcutil_occurs(
evar,r,env,_tmp143);goto _LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(
_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 7)goto _LLB0;_tmp121=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11B)->f1;_tmp122=(void*)_tmp121.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,
_tmp122);goto _LLA7;_LLB0: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 8)goto _LLB2;
_tmp123=((struct Cyc_Absyn_FnType_struct*)_tmp11B)->f1;_tmp124=_tmp123.tvars;
_tmp125=_tmp123.effect;_tmp126=(void*)_tmp123.ret_typ;_tmp127=_tmp123.args;
_tmp128=_tmp123.c_varargs;_tmp129=_tmp123.cyc_varargs;_tmp12A=_tmp123.rgn_po;
_tmp12B=_tmp123.attributes;_LLB1: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp124,env);if(_tmp125 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp125->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp126);for(0;_tmp127 != 0;_tmp127=_tmp127->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp127->hd)).f3);}if(_tmp129 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp129->type);for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){struct
_tuple6 _tmp145;void*_tmp146;void*_tmp147;struct _tuple6*_tmp144=(struct _tuple6*)
_tmp12A->hd;_tmp145=*_tmp144;_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp146);Cyc_Tcutil_occurs(evar,r,env,_tmp147);}goto _LLA7;_LLB2: if(
_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 9)goto _LLB4;_tmp12C=((struct Cyc_Absyn_TupleType_struct*)
_tmp11B)->f1;_LLB3: for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12C->hd)).f2);}goto _LLA7;_LLB4: if(_tmp11B <= (void*)
3  || *((int*)_tmp11B)!= 2)goto _LLB6;_tmp12D=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp12E=_tmp12D.targs;_tmp12F=(void*)_tmp12D.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp12F);Cyc_Tcutil_occurslist(evar,r,env,_tmp12E);goto _LLA7;_LLB6:
if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 16)goto _LLB8;_tmp130=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11B)->f2;_LLB7: _tmp132=_tmp130;goto _LLB9;_LLB8: if(_tmp11B <= (void*)3  || *((
int*)_tmp11B)!= 3)goto _LLBA;_tmp131=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11B)->f1;_tmp132=_tmp131.targs;_LLB9: _tmp134=_tmp132;goto _LLBB;_LLBA: if(
_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 10)goto _LLBC;_tmp133=((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1;_tmp134=_tmp133.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp134);goto _LLA7;_LLBC: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 11)goto
_LLBE;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11B)->f2;_LLBD: for(0;
_tmp135 != 0;_tmp135=_tmp135->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp135->hd)->type);}goto _LLA7;_LLBE: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 
17)goto _LLC0;_tmp136=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11B)->f1;_LLBF:
_tmp137=_tmp136;goto _LLC1;_LLC0: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 14)
goto _LLC2;_tmp137=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11B)->f1;_LLC1:
_tmp138=_tmp137;goto _LLC3;_LLC2: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 19)
goto _LLC4;_tmp138=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11B)->f1;_LLC3:
_tmp139=_tmp138;goto _LLC5;_LLC4: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)!= 15)
goto _LLC6;_tmp139=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11B)->f1;
_LLC5: _tmp13A=_tmp139;goto _LLC7;_LLC6: if(_tmp11B <= (void*)3  || *((int*)_tmp11B)
!= 21)goto _LLC8;_tmp13A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11B)->f1;
_LLC7: Cyc_Tcutil_occurs(evar,r,env,_tmp13A);goto _LLA7;_LLC8: if(_tmp11B <= (void*)
3  || *((int*)_tmp11B)!= 20)goto _LLCA;_tmp13B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp11B)->f1;_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp13B);goto _LLA7;_LLCA:;
_LLCB: goto _LLA7;_LLA7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp148[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp149="tq1 real_const not set.";
_tag_arr(_tmp149,sizeof(char),_get_zero_arr_size(_tmp149,24));}),_tag_arr(
_tmp148,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({void*
_tmp14A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp14B="tq2 real_const not set.";_tag_arr(_tmp14B,sizeof(char),
_get_zero_arr_size(_tmp14B,24));}),_tag_arr(_tmp14A,sizeof(void*),0));});if((tq1.real_const
!= tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({const char*
_tmp14C="(qualifiers don't match)";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp14D=(void*)x->v;void*_tmp14E;_LLD2: if((int)_tmp14D != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp150;_tmp150.tag=1;_tmp150.f1=y;_tmp150;});_tmp14F;})));return;_LLD4: if(
_tmp14D <= (void*)1  || *((int*)_tmp14D)!= 0)goto _LLD6;_tmp14E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14D)->f1;_LLD5: {void*_tmp151=(void*)y->v;void*_tmp152;_LLD9: if((int)_tmp151
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp151
<= (void*)1  || *((int*)_tmp151)!= 0)goto _LLDD;_tmp152=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp151)->f1;_LLDC: if(cmp(_tmp14E,_tmp152)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp151 <= (void*)1  || *((
int*)_tmp151)!= 1)goto _LLD8;_LLDE:({void*_tmp153[0]={};Cyc_Tcutil_impos(({const
char*_tmp154="unify_conref: forward after compress(2)";_tag_arr(_tmp154,sizeof(
char),_get_zero_arr_size(_tmp154,40));}),_tag_arr(_tmp153,sizeof(void*),0));});
_LLD8:;}_LLD6: if(_tmp14D <= (void*)1  || *((int*)_tmp14D)!= 1)goto _LLD1;_LLD7:({
void*_tmp155[0]={};Cyc_Tcutil_impos(({const char*_tmp156="unify_conref: forward after compress";
_tag_arr(_tmp156,sizeof(char),_get_zero_arr_size(_tmp156,37));}),_tag_arr(
_tmp155,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp157;_push_handler(& _tmp157);{int _tmp159=0;if(setjmp(_tmp157.handler))
_tmp159=1;if(!_tmp159){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp15A=1;_npop_handler(0);return _tmp15A;};_pop_handler();}
else{void*_tmp158=(void*)_exn_thrown;void*_tmp15C=_tmp158;_LLE0: if(_tmp15C != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp15C);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp15E=({struct _tuple6
_tmp15D;_tmp15D.f1=b1;_tmp15D.f2=b2;_tmp15D;});void*_tmp15F;void*_tmp160;void*
_tmp161;void*_tmp162;void*_tmp163;struct Cyc_Absyn_Exp*_tmp164;void*_tmp165;
struct Cyc_Absyn_Exp*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;
void*_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;_LLE5: _tmp15F=_tmp15E.f1;if((
int)_tmp15F != 0)goto _LLE7;_tmp160=_tmp15E.f2;if((int)_tmp160 != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp161=_tmp15E.f1;if((int)_tmp161 != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp162=_tmp15E.f2;if((int)_tmp162 != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp163=_tmp15E.f1;if(_tmp163 <= (void*)1  || *((int*)_tmp163)!= 0)goto _LLED;
_tmp164=((struct Cyc_Absyn_Upper_b_struct*)_tmp163)->f1;_tmp165=_tmp15E.f2;if(
_tmp165 <= (void*)1  || *((int*)_tmp165)!= 0)goto _LLED;_tmp166=((struct Cyc_Absyn_Upper_b_struct*)
_tmp165)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp164,_tmp166);_LLED: _tmp167=
_tmp15E.f1;if(_tmp167 <= (void*)1  || *((int*)_tmp167)!= 0)goto _LLEF;_tmp168=
_tmp15E.f2;if(_tmp168 <= (void*)1  || *((int*)_tmp168)!= 1)goto _LLEF;_LLEE: return -
1;_LLEF: _tmp169=_tmp15E.f1;if(_tmp169 <= (void*)1  || *((int*)_tmp169)!= 1)goto
_LLF1;_tmp16A=_tmp15E.f2;if(_tmp16A <= (void*)1  || *((int*)_tmp16A)!= 0)goto _LLF1;
_LLF0: return 1;_LLF1: _tmp16B=_tmp15E.f1;if(_tmp16B <= (void*)1  || *((int*)_tmp16B)
!= 1)goto _LLE4;_tmp16C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16B)->f1;
_tmp16D=_tmp15E.f2;if(_tmp16D <= (void*)1  || *((int*)_tmp16D)!= 1)goto _LLE4;
_tmp16E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16D)->f1;_LLF2: return Cyc_Tcutil_typecmp(
_tmp16C,_tmp16E);_LLE4:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple6 _tmp170=({struct _tuple6 _tmp16F;_tmp16F.f1=b1;_tmp16F.f2=b2;_tmp16F;});
void*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;struct Cyc_Absyn_Exp*
_tmp176;void*_tmp177;struct Cyc_Absyn_Exp*_tmp178;void*_tmp179;struct Cyc_Absyn_Exp*
_tmp17A;void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;struct
Cyc_Absyn_Exp*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;void*_tmp184;_LLF4:
_tmp171=_tmp170.f1;if((int)_tmp171 != 0)goto _LLF6;_tmp172=_tmp170.f2;if((int)
_tmp172 != 0)goto _LLF6;_LLF5: return 0;_LLF6: _tmp173=_tmp170.f1;if((int)_tmp173 != 0)
goto _LLF8;_LLF7: return - 1;_LLF8: _tmp174=_tmp170.f2;if((int)_tmp174 != 0)goto _LLFA;
_LLF9: return 1;_LLFA: _tmp175=_tmp170.f1;if(_tmp175 <= (void*)1  || *((int*)_tmp175)
!= 0)goto _LLFC;_tmp176=((struct Cyc_Absyn_Upper_b_struct*)_tmp175)->f1;_tmp177=
_tmp170.f2;if(_tmp177 <= (void*)1  || *((int*)_tmp177)!= 0)goto _LLFC;_tmp178=((
struct Cyc_Absyn_Upper_b_struct*)_tmp177)->f1;_LLFB: return Cyc_Evexp_const_exp_cmp(
_tmp176,_tmp178);_LLFC: _tmp179=_tmp170.f1;if(_tmp179 <= (void*)1  || *((int*)
_tmp179)!= 0)goto _LLFE;_tmp17A=((struct Cyc_Absyn_Upper_b_struct*)_tmp179)->f1;
_tmp17B=_tmp170.f2;if(_tmp17B <= (void*)1  || *((int*)_tmp17B)!= 1)goto _LLFE;
_tmp17C=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17B)->f1;_LLFD: _tmp17E=
_tmp17C;_tmp180=_tmp17A;goto _LLFF;_LLFE: _tmp17D=_tmp170.f1;if(_tmp17D <= (void*)1
 || *((int*)_tmp17D)!= 1)goto _LL100;_tmp17E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17D)->f1;_tmp17F=_tmp170.f2;if(_tmp17F <= (void*)1  || *((int*)_tmp17F)!= 0)
goto _LL100;_tmp180=((struct Cyc_Absyn_Upper_b_struct*)_tmp17F)->f1;_LLFF: {
unsigned int _tmp186;int _tmp187;struct _tuple7 _tmp185=Cyc_Evexp_eval_const_uint_exp(
_tmp180);_tmp186=_tmp185.f1;_tmp187=_tmp185.f2;if(!_tmp187)return 1;_tmp182=
_tmp17E;_tmp184=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp188=_cycalloc_atomic(
sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_TypeInt_struct _tmp189;_tmp189.tag=
18;_tmp189.f1=(int)_tmp186;_tmp189;});_tmp188;});goto _LL101;}_LL100: _tmp181=
_tmp170.f1;if(_tmp181 <= (void*)1  || *((int*)_tmp181)!= 1)goto _LLF3;_tmp182=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp181)->f1;_tmp183=_tmp170.f2;if(_tmp183 <= (
void*)1  || *((int*)_tmp183)!= 1)goto _LLF3;_tmp184=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp183)->f1;_LL101: Cyc_Tcutil_unify_it(_tmp182,_tmp184);return 0;_LLF3:;}static
int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp18A=att;_LL103: if(_tmp18A
<= (void*)17  || *((int*)_tmp18A)!= 0)goto _LL105;_LL104: return 0;_LL105: if((int)
_tmp18A != 0)goto _LL107;_LL106: return 1;_LL107: if((int)_tmp18A != 1)goto _LL109;
_LL108: return 2;_LL109: if((int)_tmp18A != 2)goto _LL10B;_LL10A: return 3;_LL10B: if((
int)_tmp18A != 3)goto _LL10D;_LL10C: return 4;_LL10D: if((int)_tmp18A != 4)goto _LL10F;
_LL10E: return 5;_LL10F: if(_tmp18A <= (void*)17  || *((int*)_tmp18A)!= 1)goto _LL111;
_LL110: return 6;_LL111: if((int)_tmp18A != 5)goto _LL113;_LL112: return 7;_LL113: if(
_tmp18A <= (void*)17  || *((int*)_tmp18A)!= 2)goto _LL115;_LL114: return 8;_LL115: if((
int)_tmp18A != 6)goto _LL117;_LL116: return 9;_LL117: if((int)_tmp18A != 7)goto _LL119;
_LL118: return 10;_LL119: if((int)_tmp18A != 8)goto _LL11B;_LL11A: return 11;_LL11B: if((
int)_tmp18A != 9)goto _LL11D;_LL11C: return 12;_LL11D: if((int)_tmp18A != 10)goto
_LL11F;_LL11E: return 13;_LL11F: if((int)_tmp18A != 11)goto _LL121;_LL120: return 14;
_LL121: if((int)_tmp18A != 12)goto _LL123;_LL122: return 15;_LL123: if((int)_tmp18A != 
13)goto _LL125;_LL124: return 16;_LL125: if((int)_tmp18A != 14)goto _LL127;_LL126:
return 17;_LL127: if((int)_tmp18A != 15)goto _LL129;_LL128: return 18;_LL129: if(
_tmp18A <= (void*)17  || *((int*)_tmp18A)!= 3)goto _LL12B;_LL12A: return 19;_LL12B:
if(_tmp18A <= (void*)17  || *((int*)_tmp18A)!= 4)goto _LL12D;_LL12C: return 20;_LL12D:;
_LL12E: return 21;_LL102:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp18C=({struct _tuple6 _tmp18B;_tmp18B.f1=att1;_tmp18B.f2=att2;
_tmp18B;});void*_tmp18D;int _tmp18E;void*_tmp18F;int _tmp190;void*_tmp191;int
_tmp192;void*_tmp193;int _tmp194;void*_tmp195;int _tmp196;void*_tmp197;int _tmp198;
void*_tmp199;struct _tagged_arr _tmp19A;void*_tmp19B;struct _tagged_arr _tmp19C;void*
_tmp19D;void*_tmp19E;int _tmp19F;int _tmp1A0;void*_tmp1A1;void*_tmp1A2;int _tmp1A3;
int _tmp1A4;_LL130: _tmp18D=_tmp18C.f1;if(_tmp18D <= (void*)17  || *((int*)_tmp18D)
!= 0)goto _LL132;_tmp18E=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18D)->f1;
_tmp18F=_tmp18C.f2;if(_tmp18F <= (void*)17  || *((int*)_tmp18F)!= 0)goto _LL132;
_tmp190=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18F)->f1;_LL131: _tmp192=
_tmp18E;_tmp194=_tmp190;goto _LL133;_LL132: _tmp191=_tmp18C.f1;if(_tmp191 <= (void*)
17  || *((int*)_tmp191)!= 4)goto _LL134;_tmp192=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp191)->f1;_tmp193=_tmp18C.f2;if(_tmp193 <= (void*)17  || *((int*)_tmp193)!= 4)
goto _LL134;_tmp194=((struct Cyc_Absyn_Initializes_att_struct*)_tmp193)->f1;_LL133:
_tmp196=_tmp192;_tmp198=_tmp194;goto _LL135;_LL134: _tmp195=_tmp18C.f1;if(_tmp195
<= (void*)17  || *((int*)_tmp195)!= 1)goto _LL136;_tmp196=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp195)->f1;_tmp197=_tmp18C.f2;if(_tmp197 <= (void*)17  || *((int*)_tmp197)!= 1)
goto _LL136;_tmp198=((struct Cyc_Absyn_Aligned_att_struct*)_tmp197)->f1;_LL135:
return Cyc_Core_intcmp(_tmp196,_tmp198);_LL136: _tmp199=_tmp18C.f1;if(_tmp199 <= (
void*)17  || *((int*)_tmp199)!= 2)goto _LL138;_tmp19A=((struct Cyc_Absyn_Section_att_struct*)
_tmp199)->f1;_tmp19B=_tmp18C.f2;if(_tmp19B <= (void*)17  || *((int*)_tmp19B)!= 2)
goto _LL138;_tmp19C=((struct Cyc_Absyn_Section_att_struct*)_tmp19B)->f1;_LL137:
return Cyc_strcmp((struct _tagged_arr)_tmp19A,(struct _tagged_arr)_tmp19C);_LL138:
_tmp19D=_tmp18C.f1;if(_tmp19D <= (void*)17  || *((int*)_tmp19D)!= 3)goto _LL13A;
_tmp19E=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19D)->f1;_tmp19F=((
struct Cyc_Absyn_Format_att_struct*)_tmp19D)->f2;_tmp1A0=((struct Cyc_Absyn_Format_att_struct*)
_tmp19D)->f3;_tmp1A1=_tmp18C.f2;if(_tmp1A1 <= (void*)17  || *((int*)_tmp1A1)!= 3)
goto _LL13A;_tmp1A2=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f1;
_tmp1A3=((struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f2;_tmp1A4=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A1)->f3;_LL139: {int _tmp1A5=Cyc_Core_intcmp((int)((unsigned int)_tmp19E),(
int)((unsigned int)_tmp1A2));if(_tmp1A5 != 0)return _tmp1A5;{int _tmp1A6=Cyc_Core_intcmp(
_tmp19F,_tmp1A3);if(_tmp1A6 != 0)return _tmp1A6;return Cyc_Core_intcmp(_tmp1A0,
_tmp1A4);}}_LL13A:;_LL13B: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL12F:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1A7=Cyc_Tcutil_tvar_kind(tv);_LL13D: if((int)_tmp1A7 != 3)
goto _LL13F;_LL13E: t=(void*)2;goto _LL13C;_LL13F: if((int)_tmp1A7 != 4)goto _LL141;
_LL140: t=Cyc_Absyn_empty_effect;goto _LL13C;_LL141: if((int)_tmp1A7 != 5)goto _LL143;
_LL142: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A8=_cycalloc_atomic(
sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1A9;_tmp1A9.tag=
18;_tmp1A9.f1=0;_tmp1A9;});_tmp1A8;});goto _LL13C;_LL143:;_LL144: t=Cyc_Absyn_sint_typ;
goto _LL13C;_LL13C:;}return({struct _tuple8*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA->f1=tv;_tmp1AA->f2=t;_tmp1AA;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1AB=Cyc_Tcutil_compress(t);void*_tmp1AC;struct Cyc_Absyn_TunionInfo
_tmp1AD;struct Cyc_List_List*_tmp1AE;void*_tmp1AF;struct Cyc_Absyn_PtrInfo _tmp1B0;
void*_tmp1B1;struct Cyc_Absyn_PtrAtts _tmp1B2;void*_tmp1B3;struct Cyc_Absyn_ArrayInfo
_tmp1B4;void*_tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_Absyn_TunionFieldInfo
_tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_AggrInfo _tmp1B9;struct Cyc_List_List*
_tmp1BA;struct Cyc_List_List*_tmp1BB;void*_tmp1BC;struct Cyc_Absyn_FnInfo _tmp1BD;
struct Cyc_List_List*_tmp1BE;struct Cyc_Core_Opt*_tmp1BF;void*_tmp1C0;struct Cyc_List_List*
_tmp1C1;struct Cyc_Absyn_VarargInfo*_tmp1C2;struct Cyc_List_List*_tmp1C3;void*
_tmp1C4;struct Cyc_List_List*_tmp1C5;_LL146: if((int)_tmp1AB != 0)goto _LL148;_LL147:
goto _LL149;_LL148: if((int)_tmp1AB != 1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(
_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 6)goto _LL14C;_LL14B: goto _LL14D;_LL14C:
if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 12)goto _LL14E;_LL14D: goto _LL14F;
_LL14E: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 13)goto _LL150;_LL14F: goto
_LL151;_LL150: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 18)goto _LL152;_LL151:
goto _LL153;_LL152: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 5)goto _LL154;
_LL153: return Cyc_Absyn_empty_effect;_LL154: if(_tmp1AB <= (void*)3  || *((int*)
_tmp1AB)!= 0)goto _LL156;_LL155: goto _LL157;_LL156: if(_tmp1AB <= (void*)3  || *((int*)
_tmp1AB)!= 1)goto _LL158;_LL157: {void*_tmp1C6=Cyc_Tcutil_typ_kind(t);_LL179: if((
int)_tmp1C6 != 3)goto _LL17B;_LL17A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C8;_tmp1C8.tag=19;_tmp1C8.f1=(void*)t;_tmp1C8;});_tmp1C7;});_LL17B: if((int)
_tmp1C6 != 4)goto _LL17D;_LL17C: return t;_LL17D: if((int)_tmp1C6 != 5)goto _LL17F;
_LL17E: return Cyc_Absyn_empty_effect;_LL17F:;_LL180: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1CA;_tmp1CA.tag=21;_tmp1CA.f1=(void*)t;_tmp1CA;});_tmp1C9;});_LL178:;}_LL158:
if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 15)goto _LL15A;_tmp1AC=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1AB)->f1;_LL159: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp1CC;_tmp1CC.tag=19;_tmp1CC.f1=(void*)_tmp1AC;
_tmp1CC;});_tmp1CB;});_LL15A: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 2)goto
_LL15C;_tmp1AD=((struct Cyc_Absyn_TunionType_struct*)_tmp1AB)->f1;_tmp1AE=_tmp1AD.targs;
_tmp1AF=(void*)_tmp1AD.rgn;_LL15B: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1D1;_tmp1D1.tag=19;_tmp1D1.f1=(void*)_tmp1AF;_tmp1D1;});_tmp1D0;}));_tmp1CF->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AE);_tmp1CF;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1CE;_tmp1CE.tag=20;_tmp1CE.f1=ts;
_tmp1CE;});_tmp1CD;}));}_LL15C: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 4)
goto _LL15E;_tmp1B0=((struct Cyc_Absyn_PointerType_struct*)_tmp1AB)->f1;_tmp1B1=(
void*)_tmp1B0.elt_typ;_tmp1B2=_tmp1B0.ptr_atts;_tmp1B3=(void*)_tmp1B2.rgn;_LL15D:
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D3;_tmp1D3.tag=20;_tmp1D3.f1=({void*_tmp1D4[2];_tmp1D4[1]=Cyc_Tcutil_rgns_of(
_tmp1B1);_tmp1D4[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D5=_cycalloc(
sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D6;_tmp1D6.tag=
19;_tmp1D6.f1=(void*)_tmp1B3;_tmp1D6;});_tmp1D5;});Cyc_List_list(_tag_arr(
_tmp1D4,sizeof(void*),2));});_tmp1D3;});_tmp1D2;}));_LL15E: if(_tmp1AB <= (void*)3
 || *((int*)_tmp1AB)!= 7)goto _LL160;_tmp1B4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AB)->f1;_tmp1B5=(void*)_tmp1B4.elt_type;_LL15F: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1B5));_LL160: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 
9)goto _LL162;_tmp1B6=((struct Cyc_Absyn_TupleType_struct*)_tmp1AB)->f1;_LL161: {
struct Cyc_List_List*_tmp1D7=0;for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){_tmp1D7=({
struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->hd=(void*)(*((
struct _tuple4*)_tmp1B6->hd)).f2;_tmp1D8->tl=_tmp1D7;_tmp1D8;});}_tmp1B8=_tmp1D7;
goto _LL163;}_LL162: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 3)goto _LL164;
_tmp1B7=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1AB)->f1;_tmp1B8=_tmp1B7.targs;
_LL163: _tmp1BA=_tmp1B8;goto _LL165;_LL164: if(_tmp1AB <= (void*)3  || *((int*)
_tmp1AB)!= 10)goto _LL166;_tmp1B9=((struct Cyc_Absyn_AggrType_struct*)_tmp1AB)->f1;
_tmp1BA=_tmp1B9.targs;_LL165: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1DA;_tmp1DA.tag=20;_tmp1DA.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1BA);_tmp1DA;});_tmp1D9;}));_LL166: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)
!= 11)goto _LL168;_tmp1BB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AB)->f2;
_LL167: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DC;_tmp1DC.tag=20;_tmp1DC.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1BB);_tmp1DC;});
_tmp1DB;}));_LL168: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 14)goto _LL16A;
_tmp1BC=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1AB)->f1;_LL169: return
Cyc_Tcutil_rgns_of(_tmp1BC);_LL16A: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 
17)goto _LL16C;_LL16B: return Cyc_Tcutil_rgns_of(t);_LL16C: if(_tmp1AB <= (void*)3
 || *((int*)_tmp1AB)!= 8)goto _LL16E;_tmp1BD=((struct Cyc_Absyn_FnType_struct*)
_tmp1AB)->f1;_tmp1BE=_tmp1BD.tvars;_tmp1BF=_tmp1BD.effect;_tmp1C0=(void*)_tmp1BD.ret_typ;
_tmp1C1=_tmp1BD.args;_tmp1C2=_tmp1BD.cyc_varargs;_tmp1C3=_tmp1BD.rgn_po;_LL16D: {
void*_tmp1DD=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1BE),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BF))->v);return Cyc_Tcutil_normalize_effect(
_tmp1DD);}_LL16E: if((int)_tmp1AB != 2)goto _LL170;_LL16F: return Cyc_Absyn_empty_effect;
_LL170: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 19)goto _LL172;_LL171: goto
_LL173;_LL172: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 20)goto _LL174;_LL173:
return t;_LL174: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 21)goto _LL176;_tmp1C4=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AB)->f1;_LL175: return Cyc_Tcutil_rgns_of(
_tmp1C4);_LL176: if(_tmp1AB <= (void*)3  || *((int*)_tmp1AB)!= 16)goto _LL145;
_tmp1C5=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AB)->f2;_LL177: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));
_tmp1DE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DF;_tmp1DF.tag=20;_tmp1DF.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C5);_tmp1DF;});_tmp1DE;}));_LL145:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1E0=e;struct Cyc_List_List*_tmp1E1;
struct Cyc_List_List**_tmp1E2;void*_tmp1E3;_LL182: if(_tmp1E0 <= (void*)3  || *((int*)
_tmp1E0)!= 20)goto _LL184;_tmp1E1=((struct Cyc_Absyn_JoinEff_struct*)_tmp1E0)->f1;
_tmp1E2=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1E0)->f1;
_LL183: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1E2;for(0;effs != 0;effs=
effs->tl){void*_tmp1E4=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1E4)));{void*_tmp1E5=(void*)effs->hd;void*
_tmp1E6;_LL189: if(_tmp1E5 <= (void*)3  || *((int*)_tmp1E5)!= 20)goto _LL18B;_LL18A:
goto _LL18C;_LL18B: if(_tmp1E5 <= (void*)3  || *((int*)_tmp1E5)!= 19)goto _LL18D;
_tmp1E6=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E5)->f1;if((int)_tmp1E6
!= 2)goto _LL18D;_LL18C: redo_join=1;goto _LL188;_LL18D:;_LL18E: goto _LL188;_LL188:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1E2;for(0;effs != 0;effs=effs->tl){void*_tmp1E7=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1E8;void*_tmp1E9;_LL190: if(_tmp1E7 <= (void*)3
 || *((int*)_tmp1E7)!= 20)goto _LL192;_tmp1E8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E7)->f1;_LL191: effects=Cyc_List_revappend(_tmp1E8,effects);goto _LL18F;
_LL192: if(_tmp1E7 <= (void*)3  || *((int*)_tmp1E7)!= 19)goto _LL194;_tmp1E9=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1E7)->f1;if((int)_tmp1E9 != 2)goto _LL194;
_LL193: goto _LL18F;_LL194:;_LL195: effects=({struct Cyc_List_List*_tmp1EA=_cycalloc(
sizeof(*_tmp1EA));_tmp1EA->hd=(void*)_tmp1E7;_tmp1EA->tl=effects;_tmp1EA;});goto
_LL18F;_LL18F:;}}*_tmp1E2=Cyc_List_imp_rev(effects);return e;}}_LL184: if(_tmp1E0
<= (void*)3  || *((int*)_tmp1E0)!= 21)goto _LL186;_tmp1E3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E0)->f1;_LL185: {void*_tmp1EB=Cyc_Tcutil_compress(_tmp1E3);_LL197: if(
_tmp1EB <= (void*)3  || *((int*)_tmp1EB)!= 0)goto _LL199;_LL198: goto _LL19A;_LL199:
if(_tmp1EB <= (void*)3  || *((int*)_tmp1EB)!= 1)goto _LL19B;_LL19A: return e;_LL19B:;
_LL19C: return Cyc_Tcutil_rgns_of(_tmp1E3);_LL196:;}_LL186:;_LL187: return e;_LL181:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1EC=({struct Cyc_Absyn_FnType_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_FnType_struct
_tmp1EE;_tmp1EE.tag=8;_tmp1EE.f1=({struct Cyc_Absyn_FnInfo _tmp1EF;_tmp1EF.tvars=0;
_tmp1EF.effect=({struct Cyc_Core_Opt*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->v=(
void*)eff;_tmp1F0;});_tmp1EF.ret_typ=(void*)((void*)0);_tmp1EF.args=0;_tmp1EF.c_varargs=
0;_tmp1EF.cyc_varargs=0;_tmp1EF.rgn_po=0;_tmp1EF.attributes=0;_tmp1EF;});_tmp1EE;});
_tmp1ED;});return Cyc_Absyn_atb_typ((void*)_tmp1EC,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{
void*_tmp1F1=Cyc_Tcutil_compress(e);void*_tmp1F2;struct Cyc_List_List*_tmp1F3;
void*_tmp1F4;struct Cyc_Core_Opt*_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Core_Opt**
_tmp1F7;struct Cyc_Core_Opt*_tmp1F8;_LL19E: if(_tmp1F1 <= (void*)3  || *((int*)
_tmp1F1)!= 19)goto _LL1A0;_tmp1F2=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F1)->f1;_LL19F: if(constrain)return Cyc_Tcutil_unify(r,_tmp1F2);_tmp1F2=Cyc_Tcutil_compress(
_tmp1F2);if(r == _tmp1F2)return 1;{struct _tuple6 _tmp1FA=({struct _tuple6 _tmp1F9;
_tmp1F9.f1=r;_tmp1F9.f2=_tmp1F2;_tmp1F9;});void*_tmp1FB;struct Cyc_Absyn_Tvar*
_tmp1FC;void*_tmp1FD;struct Cyc_Absyn_Tvar*_tmp1FE;_LL1A9: _tmp1FB=_tmp1FA.f1;if(
_tmp1FB <= (void*)3  || *((int*)_tmp1FB)!= 1)goto _LL1AB;_tmp1FC=((struct Cyc_Absyn_VarType_struct*)
_tmp1FB)->f1;_tmp1FD=_tmp1FA.f2;if(_tmp1FD <= (void*)3  || *((int*)_tmp1FD)!= 1)
goto _LL1AB;_tmp1FE=((struct Cyc_Absyn_VarType_struct*)_tmp1FD)->f1;_LL1AA: return
Cyc_Absyn_tvar_cmp(_tmp1FC,_tmp1FE)== 0;_LL1AB:;_LL1AC: return 0;_LL1A8:;}_LL1A0:
if(_tmp1F1 <= (void*)3  || *((int*)_tmp1F1)!= 20)goto _LL1A2;_tmp1F3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F1)->f1;_LL1A1: for(0;_tmp1F3 != 0;_tmp1F3=_tmp1F3->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1F3->hd))return 1;}return 0;_LL1A2: if(_tmp1F1 <= (void*)3
 || *((int*)_tmp1F1)!= 21)goto _LL1A4;_tmp1F4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F1)->f1;_LL1A3: {void*_tmp1FF=Cyc_Tcutil_rgns_of(_tmp1F4);void*_tmp200;
_LL1AE: if(_tmp1FF <= (void*)3  || *((int*)_tmp1FF)!= 21)goto _LL1B0;_tmp200=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1FF)->f1;_LL1AF: if(!constrain)return 0;{void*
_tmp201=Cyc_Tcutil_compress(_tmp200);struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt*
_tmp203;struct Cyc_Core_Opt**_tmp204;struct Cyc_Core_Opt*_tmp205;_LL1B3: if(_tmp201
<= (void*)3  || *((int*)_tmp201)!= 0)goto _LL1B5;_tmp202=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f1;_tmp203=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp204=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;_tmp205=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f4;_LL1B4: {void*_tmp206=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp205);Cyc_Tcutil_occurs(_tmp206,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp205))->v,r);{void*_tmp207=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp209=_cycalloc(sizeof(*_tmp209));
_tmp209[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20A;_tmp20A.tag=20;_tmp20A.f1=({
void*_tmp20B[2];_tmp20B[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp20C=
_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp20D;
_tmp20D.tag=19;_tmp20D.f1=(void*)r;_tmp20D;});_tmp20C;});_tmp20B[0]=_tmp206;Cyc_List_list(
_tag_arr(_tmp20B,sizeof(void*),2));});_tmp20A;});_tmp209;}));*_tmp204=({struct
Cyc_Core_Opt*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->v=(void*)_tmp207;
_tmp208;});return 1;}}_LL1B5:;_LL1B6: return 0;_LL1B2:;}_LL1B0:;_LL1B1: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp1FF);_LL1AD:;}_LL1A4: if(_tmp1F1 <= (void*)3  || *((int*)_tmp1F1)!= 
0)goto _LL1A6;_tmp1F5=((struct Cyc_Absyn_Evar_struct*)_tmp1F1)->f1;_tmp1F6=((
struct Cyc_Absyn_Evar_struct*)_tmp1F1)->f2;_tmp1F7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F1)->f2;_tmp1F8=((struct Cyc_Absyn_Evar_struct*)
_tmp1F1)->f4;_LL1A5: if(_tmp1F5 == 0  || (void*)_tmp1F5->v != (void*)4)({void*
_tmp20E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp20F="effect evar has wrong kind";_tag_arr(_tmp20F,sizeof(char),
_get_zero_arr_size(_tmp20F,27));}),_tag_arr(_tmp20E,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp210=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F8);Cyc_Tcutil_occurs(_tmp210,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F8))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp211=({struct Cyc_Absyn_JoinEff_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Absyn_JoinEff_struct _tmp214;_tmp214.tag=20;_tmp214.f1=({
struct Cyc_List_List*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->hd=(void*)
_tmp210;_tmp215->tl=({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));
_tmp216->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp217=_cycalloc(
sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Absyn_AccessEff_struct _tmp218;_tmp218.tag=
19;_tmp218.f1=(void*)r;_tmp218;});_tmp217;}));_tmp216->tl=0;_tmp216;});_tmp215;});
_tmp214;});_tmp213;});*_tmp1F7=({struct Cyc_Core_Opt*_tmp212=_cycalloc(sizeof(*
_tmp212));_tmp212->v=(void*)((void*)_tmp211);_tmp212;});return 1;}}_LL1A6:;_LL1A7:
return 0;_LL19D:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp219=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp21A;void*_tmp21B;struct Cyc_Core_Opt*_tmp21C;struct
Cyc_Core_Opt*_tmp21D;struct Cyc_Core_Opt**_tmp21E;struct Cyc_Core_Opt*_tmp21F;
_LL1B8: if(_tmp219 <= (void*)3  || *((int*)_tmp219)!= 19)goto _LL1BA;_LL1B9: return 0;
_LL1BA: if(_tmp219 <= (void*)3  || *((int*)_tmp219)!= 20)goto _LL1BC;_tmp21A=((
struct Cyc_Absyn_JoinEff_struct*)_tmp219)->f1;_LL1BB: for(0;_tmp21A != 0;_tmp21A=
_tmp21A->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp21A->hd))
return 1;}return 0;_LL1BC: if(_tmp219 <= (void*)3  || *((int*)_tmp219)!= 21)goto
_LL1BE;_tmp21B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp219)->f1;_LL1BD:
_tmp21B=Cyc_Tcutil_compress(_tmp21B);if(t == _tmp21B)return 1;if(
may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp21B);{void*_tmp220=Cyc_Tcutil_rgns_of(
t);void*_tmp221;_LL1C3: if(_tmp220 <= (void*)3  || *((int*)_tmp220)!= 21)goto _LL1C5;
_tmp221=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp220)->f1;_LL1C4: {struct
_tuple6 _tmp223=({struct _tuple6 _tmp222;_tmp222.f1=Cyc_Tcutil_compress(_tmp221);
_tmp222.f2=_tmp21B;_tmp222;});void*_tmp224;struct Cyc_Absyn_Tvar*_tmp225;void*
_tmp226;struct Cyc_Absyn_Tvar*_tmp227;_LL1C8: _tmp224=_tmp223.f1;if(_tmp224 <= (
void*)3  || *((int*)_tmp224)!= 1)goto _LL1CA;_tmp225=((struct Cyc_Absyn_VarType_struct*)
_tmp224)->f1;_tmp226=_tmp223.f2;if(_tmp226 <= (void*)3  || *((int*)_tmp226)!= 1)
goto _LL1CA;_tmp227=((struct Cyc_Absyn_VarType_struct*)_tmp226)->f1;_LL1C9: return
Cyc_Tcutil_unify(t,_tmp21B);_LL1CA:;_LL1CB: return _tmp221 == _tmp21B;_LL1C7:;}
_LL1C5:;_LL1C6: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp220);
_LL1C2:;}_LL1BE: if(_tmp219 <= (void*)3  || *((int*)_tmp219)!= 0)goto _LL1C0;_tmp21C=((
struct Cyc_Absyn_Evar_struct*)_tmp219)->f1;_tmp21D=((struct Cyc_Absyn_Evar_struct*)
_tmp219)->f2;_tmp21E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp219)->f2;_tmp21F=((struct Cyc_Absyn_Evar_struct*)_tmp219)->f4;_LL1BF: if(
_tmp21C == 0  || (void*)_tmp21C->v != (void*)4)({void*_tmp228[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp229="effect evar has wrong kind";
_tag_arr(_tmp229,sizeof(char),_get_zero_arr_size(_tmp229,27));}),_tag_arr(
_tmp228,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp22A=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp21F);Cyc_Tcutil_occurs(_tmp22A,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21F))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp22B=({struct Cyc_Absyn_JoinEff_struct*_tmp22D=
_cycalloc(sizeof(*_tmp22D));_tmp22D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22E;
_tmp22E.tag=20;_tmp22E.f1=({struct Cyc_List_List*_tmp22F=_cycalloc(sizeof(*
_tmp22F));_tmp22F->hd=(void*)_tmp22A;_tmp22F->tl=({struct Cyc_List_List*_tmp230=
_cycalloc(sizeof(*_tmp230));_tmp230->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp232;_tmp232.tag=21;_tmp232.f1=(void*)t;_tmp232;});_tmp231;}));_tmp230->tl=0;
_tmp230;});_tmp22F;});_tmp22E;});_tmp22D;});*_tmp21E=({struct Cyc_Core_Opt*
_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->v=(void*)((void*)_tmp22B);_tmp22C;});
return 1;}}_LL1C0:;_LL1C1: return 0;_LL1B7:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp233=e;struct Cyc_Absyn_Tvar*_tmp234;struct Cyc_List_List*_tmp235;void*
_tmp236;struct Cyc_Core_Opt*_tmp237;struct Cyc_Core_Opt*_tmp238;struct Cyc_Core_Opt**
_tmp239;struct Cyc_Core_Opt*_tmp23A;_LL1CD: if(_tmp233 <= (void*)3  || *((int*)
_tmp233)!= 1)goto _LL1CF;_tmp234=((struct Cyc_Absyn_VarType_struct*)_tmp233)->f1;
_LL1CE: return Cyc_Absyn_tvar_cmp(v,_tmp234)== 0;_LL1CF: if(_tmp233 <= (void*)3  || *((
int*)_tmp233)!= 20)goto _LL1D1;_tmp235=((struct Cyc_Absyn_JoinEff_struct*)_tmp233)->f1;
_LL1D0: for(0;_tmp235 != 0;_tmp235=_tmp235->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp235->hd))return 1;}return 0;_LL1D1: if(_tmp233 <= (
void*)3  || *((int*)_tmp233)!= 21)goto _LL1D3;_tmp236=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp233)->f1;_LL1D2: {void*_tmp23B=Cyc_Tcutil_rgns_of(_tmp236);void*_tmp23C;
_LL1D8: if(_tmp23B <= (void*)3  || *((int*)_tmp23B)!= 21)goto _LL1DA;_tmp23C=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp23B)->f1;_LL1D9: if(!may_constrain_evars)
return 0;{void*_tmp23D=Cyc_Tcutil_compress(_tmp23C);struct Cyc_Core_Opt*_tmp23E;
struct Cyc_Core_Opt*_tmp23F;struct Cyc_Core_Opt**_tmp240;struct Cyc_Core_Opt*
_tmp241;_LL1DD: if(_tmp23D <= (void*)3  || *((int*)_tmp23D)!= 0)goto _LL1DF;_tmp23E=((
struct Cyc_Absyn_Evar_struct*)_tmp23D)->f1;_tmp23F=((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp240=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23D)->f2;_tmp241=((struct Cyc_Absyn_Evar_struct*)_tmp23D)->f4;_LL1DE: {void*
_tmp242=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp241);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp241))->v,v))return 0;{void*_tmp243=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp245=_cycalloc(sizeof(*_tmp245));
_tmp245[0]=({struct Cyc_Absyn_JoinEff_struct _tmp246;_tmp246.tag=20;_tmp246.f1=({
void*_tmp247[2];_tmp247[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp248=
_cycalloc(sizeof(*_tmp248));_tmp248[0]=({struct Cyc_Absyn_VarType_struct _tmp249;
_tmp249.tag=1;_tmp249.f1=v;_tmp249;});_tmp248;});_tmp247[0]=_tmp242;Cyc_List_list(
_tag_arr(_tmp247,sizeof(void*),2));});_tmp246;});_tmp245;}));*_tmp240=({struct
Cyc_Core_Opt*_tmp244=_cycalloc(sizeof(*_tmp244));_tmp244->v=(void*)_tmp243;
_tmp244;});return 1;}}_LL1DF:;_LL1E0: return 0;_LL1DC:;}_LL1DA:;_LL1DB: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp23B);_LL1D7:;}_LL1D3: if(_tmp233 <= (void*)3  || *((int*)
_tmp233)!= 0)goto _LL1D5;_tmp237=((struct Cyc_Absyn_Evar_struct*)_tmp233)->f1;
_tmp238=((struct Cyc_Absyn_Evar_struct*)_tmp233)->f2;_tmp239=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp233)->f2;_tmp23A=((struct Cyc_Absyn_Evar_struct*)
_tmp233)->f4;_LL1D4: if(_tmp237 == 0  || (void*)_tmp237->v != (void*)4)({void*
_tmp24A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp24B="effect evar has wrong kind";_tag_arr(_tmp24B,sizeof(char),
_get_zero_arr_size(_tmp24B,27));}),_tag_arr(_tmp24A,sizeof(void*),0));});{void*
_tmp24C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp23A);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp23A))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp24D=({struct Cyc_Absyn_JoinEff_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));
_tmp24F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp250;_tmp250.tag=20;_tmp250.f1=({
struct Cyc_List_List*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251->hd=(void*)
_tmp24C;_tmp251->tl=({struct Cyc_List_List*_tmp252=_cycalloc(sizeof(*_tmp252));
_tmp252->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp253=_cycalloc(
sizeof(*_tmp253));_tmp253[0]=({struct Cyc_Absyn_VarType_struct _tmp254;_tmp254.tag=
1;_tmp254.f1=v;_tmp254;});_tmp253;}));_tmp252->tl=0;_tmp252;});_tmp251;});
_tmp250;});_tmp24F;});*_tmp239=({struct Cyc_Core_Opt*_tmp24E=_cycalloc(sizeof(*
_tmp24E));_tmp24E->v=(void*)((void*)_tmp24D);_tmp24E;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp255=e;struct Cyc_List_List*_tmp256;void*_tmp257;_LL1E2: if(_tmp255 <= (
void*)3  || *((int*)_tmp255)!= 20)goto _LL1E4;_tmp256=((struct Cyc_Absyn_JoinEff_struct*)
_tmp255)->f1;_LL1E3: for(0;_tmp256 != 0;_tmp256=_tmp256->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp256->hd))return 1;}return 0;_LL1E4: if(_tmp255 <= (void*)3  || *((int*)
_tmp255)!= 21)goto _LL1E6;_tmp257=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp255)->f1;_LL1E5: {void*_tmp258=Cyc_Tcutil_rgns_of(_tmp257);void*_tmp259;
_LL1EB: if(_tmp258 <= (void*)3  || *((int*)_tmp258)!= 21)goto _LL1ED;_tmp259=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp258)->f1;_LL1EC: return 0;_LL1ED:;_LL1EE:
return Cyc_Tcutil_evar_in_effect(evar,_tmp258);_LL1EA:;}_LL1E6: if(_tmp255 <= (void*)
3  || *((int*)_tmp255)!= 0)goto _LL1E8;_LL1E7: return evar == e;_LL1E8:;_LL1E9: return
0;_LL1E1:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp25A=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp25B;void*_tmp25C;
struct Cyc_Absyn_Tvar*_tmp25D;void*_tmp25E;struct Cyc_Core_Opt*_tmp25F;struct Cyc_Core_Opt**
_tmp260;struct Cyc_Core_Opt*_tmp261;_LL1F0: if(_tmp25A <= (void*)3  || *((int*)
_tmp25A)!= 20)goto _LL1F2;_tmp25B=((struct Cyc_Absyn_JoinEff_struct*)_tmp25A)->f1;
_LL1F1: for(0;_tmp25B != 0;_tmp25B=_tmp25B->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp25B->hd,e2))return 0;}return 1;_LL1F2: if(_tmp25A <= (
void*)3  || *((int*)_tmp25A)!= 19)goto _LL1F4;_tmp25C=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp25A)->f1;_LL1F3: return Cyc_Tcutil_region_in_effect(0,_tmp25C,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp25C,(void*)2);_LL1F4: if(_tmp25A <= (
void*)3  || *((int*)_tmp25A)!= 1)goto _LL1F6;_tmp25D=((struct Cyc_Absyn_VarType_struct*)
_tmp25A)->f1;_LL1F5: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp25D,e2);_LL1F6: if(_tmp25A <= (void*)3  || *((int*)_tmp25A)!= 21)goto _LL1F8;
_tmp25E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp25A)->f1;_LL1F7: {void*
_tmp262=Cyc_Tcutil_rgns_of(_tmp25E);void*_tmp263;_LL1FD: if(_tmp262 <= (void*)3
 || *((int*)_tmp262)!= 21)goto _LL1FF;_tmp263=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp262)->f1;_LL1FE: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp263,
e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp263,Cyc_Absyn_sint_typ);
_LL1FF:;_LL200: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp262,e2);
_LL1FC:;}_LL1F8: if(_tmp25A <= (void*)3  || *((int*)_tmp25A)!= 0)goto _LL1FA;_tmp25F=((
struct Cyc_Absyn_Evar_struct*)_tmp25A)->f2;_tmp260=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp25A)->f2;_tmp261=((struct Cyc_Absyn_Evar_struct*)
_tmp25A)->f4;_LL1F9: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp260=({struct Cyc_Core_Opt*
_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264->v=(void*)Cyc_Absyn_empty_effect;
_tmp264;});return 1;_LL1FA:;_LL1FB:({struct Cyc_String_pa_struct _tmp267;_tmp267.tag=
0;_tmp267.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp265[1]={& _tmp267};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp266="subset_effect: bad effect: %s";_tag_arr(
_tmp266,sizeof(char),_get_zero_arr_size(_tmp266,30));}),_tag_arr(_tmp265,sizeof(
void*),1));}});_LL1EF:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp269;void*_tmp26A;void*_tmp26B;struct
_tuple6*_tmp268=(struct _tuple6*)r1->hd;_tmp269=*_tmp268;_tmp26A=_tmp269.f1;
_tmp26B=_tmp269.f2;{int found=_tmp26A == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple6 _tmp26D;void*_tmp26E;void*_tmp26F;
struct _tuple6*_tmp26C=(struct _tuple6*)r2->hd;_tmp26D=*_tmp26C;_tmp26E=_tmp26D.f1;
_tmp26F=_tmp26D.f2;if(Cyc_Tcutil_unify(_tmp26A,_tmp26E) && Cyc_Tcutil_unify(
_tmp26B,_tmp26F)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp270=t1;struct Cyc_Core_Opt*_tmp271;struct Cyc_Core_Opt*_tmp272;
struct Cyc_Core_Opt**_tmp273;struct Cyc_Core_Opt*_tmp274;_LL202: if(_tmp270 <= (void*)
3  || *((int*)_tmp270)!= 0)goto _LL204;_tmp271=((struct Cyc_Absyn_Evar_struct*)
_tmp270)->f1;_tmp272=((struct Cyc_Absyn_Evar_struct*)_tmp270)->f2;_tmp273=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp270)->f2;_tmp274=((struct Cyc_Absyn_Evar_struct*)
_tmp270)->f4;_LL203: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp274))->v,t2);{void*_tmp275=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp275,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp271))->v)){*_tmp273=({struct Cyc_Core_Opt*_tmp276=_cycalloc(sizeof(*_tmp276));
_tmp276->v=(void*)t2;_tmp276;});return;}else{{void*_tmp277=t2;struct Cyc_Core_Opt*
_tmp278;struct Cyc_Core_Opt**_tmp279;struct Cyc_Core_Opt*_tmp27A;struct Cyc_Absyn_PtrInfo
_tmp27B;_LL207: if(_tmp277 <= (void*)3  || *((int*)_tmp277)!= 0)goto _LL209;_tmp278=((
struct Cyc_Absyn_Evar_struct*)_tmp277)->f2;_tmp279=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp277)->f2;_tmp27A=((struct Cyc_Absyn_Evar_struct*)
_tmp277)->f4;_LL208: {struct Cyc_List_List*_tmp27C=(struct Cyc_List_List*)_tmp274->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp27A))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp27C,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp27D="(type variable would escape scope)";_tag_arr(_tmp27D,sizeof(
char),_get_zero_arr_size(_tmp27D,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp271->v,_tmp275)){*_tmp279=({struct Cyc_Core_Opt*
_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->v=(void*)t1;_tmp27E;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp27F="(kinds are incompatible)";_tag_arr(_tmp27F,sizeof(char),
_get_zero_arr_size(_tmp27F,25));});goto _LL206;}_LL209: if(_tmp277 <= (void*)3  || *((
int*)_tmp277)!= 4)goto _LL20B;_tmp27B=((struct Cyc_Absyn_PointerType_struct*)
_tmp277)->f1;if(!((void*)_tmp271->v == (void*)2))goto _LL20B;_LL20A: {struct Cyc_Absyn_Conref*
_tmp280=Cyc_Absyn_compress_conref((_tmp27B.ptr_atts).bounds);{void*_tmp281=(void*)
_tmp280->v;_LL20E: if((int)_tmp281 != 0)goto _LL210;_LL20F:(void*)(_tmp280->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp282=_cycalloc(sizeof(*_tmp282));
_tmp282[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp283;_tmp283.tag=0;_tmp283.f1=(
void*)Cyc_Absyn_bounds_one;_tmp283;});_tmp282;})));*_tmp273=({struct Cyc_Core_Opt*
_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->v=(void*)t2;_tmp284;});return;
_LL210:;_LL211: goto _LL20D;_LL20D:;}goto _LL206;}_LL20B:;_LL20C: goto _LL206;_LL206:;}
Cyc_Tcutil_failure_reason=({const char*_tmp285="(kinds are incompatible)";
_tag_arr(_tmp285,sizeof(char),_get_zero_arr_size(_tmp285,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL204:;_LL205: goto _LL201;_LL201:;}{struct _tuple6
_tmp287=({struct _tuple6 _tmp286;_tmp286.f1=t2;_tmp286.f2=t1;_tmp286;});void*
_tmp288;void*_tmp289;void*_tmp28A;void*_tmp28B;struct Cyc_Absyn_Tvar*_tmp28C;void*
_tmp28D;struct Cyc_Absyn_Tvar*_tmp28E;void*_tmp28F;struct Cyc_Absyn_AggrInfo
_tmp290;void*_tmp291;struct Cyc_List_List*_tmp292;void*_tmp293;struct Cyc_Absyn_AggrInfo
_tmp294;void*_tmp295;struct Cyc_List_List*_tmp296;void*_tmp297;struct _tuple1*
_tmp298;void*_tmp299;struct _tuple1*_tmp29A;void*_tmp29B;struct Cyc_List_List*
_tmp29C;void*_tmp29D;struct Cyc_List_List*_tmp29E;void*_tmp29F;struct Cyc_Absyn_TunionInfo
_tmp2A0;void*_tmp2A1;struct Cyc_Absyn_Tuniondecl**_tmp2A2;struct Cyc_Absyn_Tuniondecl*
_tmp2A3;struct Cyc_List_List*_tmp2A4;void*_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_TunionInfo
_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Tuniondecl**_tmp2A9;struct Cyc_Absyn_Tuniondecl*
_tmp2AA;struct Cyc_List_List*_tmp2AB;void*_tmp2AC;void*_tmp2AD;struct Cyc_Absyn_TunionFieldInfo
_tmp2AE;void*_tmp2AF;struct Cyc_Absyn_Tuniondecl*_tmp2B0;struct Cyc_Absyn_Tunionfield*
_tmp2B1;struct Cyc_List_List*_tmp2B2;void*_tmp2B3;struct Cyc_Absyn_TunionFieldInfo
_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Tuniondecl*_tmp2B6;struct Cyc_Absyn_Tunionfield*
_tmp2B7;struct Cyc_List_List*_tmp2B8;void*_tmp2B9;struct Cyc_Absyn_PtrInfo _tmp2BA;
void*_tmp2BB;struct Cyc_Absyn_Tqual _tmp2BC;struct Cyc_Absyn_PtrAtts _tmp2BD;void*
_tmp2BE;struct Cyc_Absyn_Conref*_tmp2BF;struct Cyc_Absyn_Conref*_tmp2C0;struct Cyc_Absyn_Conref*
_tmp2C1;struct Cyc_Position_Segment*_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_PtrInfo
_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Tqual _tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;
void*_tmp2C8;struct Cyc_Absyn_Conref*_tmp2C9;struct Cyc_Absyn_Conref*_tmp2CA;
struct Cyc_Absyn_Conref*_tmp2CB;struct Cyc_Position_Segment*_tmp2CC;void*_tmp2CD;
void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*_tmp2D2;void*_tmp2D3;
void*_tmp2D4;void*_tmp2D5;int _tmp2D6;void*_tmp2D7;int _tmp2D8;void*_tmp2D9;void*
_tmp2DA;void*_tmp2DB;void*_tmp2DC;void*_tmp2DD;int _tmp2DE;void*_tmp2DF;int
_tmp2E0;void*_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*_tmp2E4;void*_tmp2E5;struct
Cyc_Absyn_ArrayInfo _tmp2E6;void*_tmp2E7;struct Cyc_Absyn_Tqual _tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;struct Cyc_Absyn_Conref*_tmp2EA;void*_tmp2EB;struct Cyc_Absyn_ArrayInfo
_tmp2EC;void*_tmp2ED;struct Cyc_Absyn_Tqual _tmp2EE;struct Cyc_Absyn_Exp*_tmp2EF;
struct Cyc_Absyn_Conref*_tmp2F0;void*_tmp2F1;struct Cyc_Absyn_FnInfo _tmp2F2;struct
Cyc_List_List*_tmp2F3;struct Cyc_Core_Opt*_tmp2F4;void*_tmp2F5;struct Cyc_List_List*
_tmp2F6;int _tmp2F7;struct Cyc_Absyn_VarargInfo*_tmp2F8;struct Cyc_List_List*
_tmp2F9;struct Cyc_List_List*_tmp2FA;void*_tmp2FB;struct Cyc_Absyn_FnInfo _tmp2FC;
struct Cyc_List_List*_tmp2FD;struct Cyc_Core_Opt*_tmp2FE;void*_tmp2FF;struct Cyc_List_List*
_tmp300;int _tmp301;struct Cyc_Absyn_VarargInfo*_tmp302;struct Cyc_List_List*
_tmp303;struct Cyc_List_List*_tmp304;void*_tmp305;struct Cyc_List_List*_tmp306;
void*_tmp307;struct Cyc_List_List*_tmp308;void*_tmp309;void*_tmp30A;struct Cyc_List_List*
_tmp30B;void*_tmp30C;void*_tmp30D;struct Cyc_List_List*_tmp30E;void*_tmp30F;void*
_tmp310;void*_tmp311;void*_tmp312;void*_tmp313;void*_tmp314;void*_tmp315;void*
_tmp316;void*_tmp317;void*_tmp318;void*_tmp319;void*_tmp31A;_LL213: _tmp288=
_tmp287.f1;if(_tmp288 <= (void*)3  || *((int*)_tmp288)!= 0)goto _LL215;_LL214: Cyc_Tcutil_unify_it(
t2,t1);return;_LL215: _tmp289=_tmp287.f1;if((int)_tmp289 != 0)goto _LL217;_tmp28A=
_tmp287.f2;if((int)_tmp28A != 0)goto _LL217;_LL216: return;_LL217: _tmp28B=_tmp287.f1;
if(_tmp28B <= (void*)3  || *((int*)_tmp28B)!= 1)goto _LL219;_tmp28C=((struct Cyc_Absyn_VarType_struct*)
_tmp28B)->f1;_tmp28D=_tmp287.f2;if(_tmp28D <= (void*)3  || *((int*)_tmp28D)!= 1)
goto _LL219;_tmp28E=((struct Cyc_Absyn_VarType_struct*)_tmp28D)->f1;_LL218: {
struct _tagged_arr*_tmp31B=_tmp28C->name;struct _tagged_arr*_tmp31C=_tmp28E->name;
int _tmp31D=*((int*)_check_null(_tmp28C->identity));int _tmp31E=*((int*)
_check_null(_tmp28E->identity));void*_tmp31F=Cyc_Tcutil_tvar_kind(_tmp28C);void*
_tmp320=Cyc_Tcutil_tvar_kind(_tmp28E);if(_tmp31E == _tmp31D  && Cyc_strptrcmp(
_tmp31B,_tmp31C)== 0){if(_tmp31F != _tmp320)({struct Cyc_String_pa_struct _tmp325;
_tmp325.tag=0;_tmp325.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp320));{struct Cyc_String_pa_struct _tmp324;_tmp324.tag=0;_tmp324.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp31F));{struct Cyc_String_pa_struct
_tmp323;_tmp323.tag=0;_tmp323.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp31B);{
void*_tmp321[3]={& _tmp323,& _tmp324,& _tmp325};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp322="same type variable %s has kinds %s and %s";
_tag_arr(_tmp322,sizeof(char),_get_zero_arr_size(_tmp322,42));}),_tag_arr(
_tmp321,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp326="(variable types are not the same)";_tag_arr(_tmp326,sizeof(char),
_get_zero_arr_size(_tmp326,34));});goto _LL212;}_LL219: _tmp28F=_tmp287.f1;if(
_tmp28F <= (void*)3  || *((int*)_tmp28F)!= 10)goto _LL21B;_tmp290=((struct Cyc_Absyn_AggrType_struct*)
_tmp28F)->f1;_tmp291=(void*)_tmp290.aggr_info;_tmp292=_tmp290.targs;_tmp293=
_tmp287.f2;if(_tmp293 <= (void*)3  || *((int*)_tmp293)!= 10)goto _LL21B;_tmp294=((
struct Cyc_Absyn_AggrType_struct*)_tmp293)->f1;_tmp295=(void*)_tmp294.aggr_info;
_tmp296=_tmp294.targs;_LL21A: {void*_tmp328;struct _tuple1*_tmp329;struct _tuple5
_tmp327=Cyc_Absyn_aggr_kinded_name(_tmp295);_tmp328=_tmp327.f1;_tmp329=_tmp327.f2;{
void*_tmp32B;struct _tuple1*_tmp32C;struct _tuple5 _tmp32A=Cyc_Absyn_aggr_kinded_name(
_tmp291);_tmp32B=_tmp32A.f1;_tmp32C=_tmp32A.f2;if(_tmp328 != _tmp32B){Cyc_Tcutil_failure_reason=({
const char*_tmp32D="(struct and union type)";_tag_arr(_tmp32D,sizeof(char),
_get_zero_arr_size(_tmp32D,24));});goto _LL212;}if(Cyc_Absyn_qvar_cmp(_tmp329,
_tmp32C)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32E="(different type name)";
_tag_arr(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,22));});goto _LL212;}Cyc_Tcutil_unify_list(
_tmp296,_tmp292);return;}}_LL21B: _tmp297=_tmp287.f1;if(_tmp297 <= (void*)3  || *((
int*)_tmp297)!= 12)goto _LL21D;_tmp298=((struct Cyc_Absyn_EnumType_struct*)_tmp297)->f1;
_tmp299=_tmp287.f2;if(_tmp299 <= (void*)3  || *((int*)_tmp299)!= 12)goto _LL21D;
_tmp29A=((struct Cyc_Absyn_EnumType_struct*)_tmp299)->f1;_LL21C: if(Cyc_Absyn_qvar_cmp(
_tmp298,_tmp29A)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different enum types)";
_tag_arr(_tmp32F,sizeof(char),_get_zero_arr_size(_tmp32F,23));});goto _LL212;
_LL21D: _tmp29B=_tmp287.f1;if(_tmp29B <= (void*)3  || *((int*)_tmp29B)!= 13)goto
_LL21F;_tmp29C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp29B)->f1;_tmp29D=
_tmp287.f2;if(_tmp29D <= (void*)3  || *((int*)_tmp29D)!= 13)goto _LL21F;_tmp29E=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp29D)->f1;_LL21E: {int bad=0;for(0;
_tmp29C != 0  && _tmp29E != 0;(_tmp29C=_tmp29C->tl,_tmp29E=_tmp29E->tl)){struct Cyc_Absyn_Enumfield*
_tmp330=(struct Cyc_Absyn_Enumfield*)_tmp29C->hd;struct Cyc_Absyn_Enumfield*
_tmp331=(struct Cyc_Absyn_Enumfield*)_tmp29E->hd;if(Cyc_Absyn_qvar_cmp(_tmp330->name,
_tmp331->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp332="(different names for enum fields)";
_tag_arr(_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,34));});bad=1;break;}
if(_tmp330->tag == _tmp331->tag)continue;if(_tmp330->tag == 0  || _tmp331->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp333="(different tag values for enum fields)";
_tag_arr(_tmp333,sizeof(char),_get_zero_arr_size(_tmp333,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp330->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp331->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp334="(different tag values for enum fields)";_tag_arr(_tmp334,
sizeof(char),_get_zero_arr_size(_tmp334,39));});bad=1;break;}}if(bad)goto _LL212;
if(_tmp29C == 0  && _tmp29E == 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp335="(different number of fields for enums)";_tag_arr(_tmp335,sizeof(char),
_get_zero_arr_size(_tmp335,39));});goto _LL212;}_LL21F: _tmp29F=_tmp287.f1;if(
_tmp29F <= (void*)3  || *((int*)_tmp29F)!= 2)goto _LL221;_tmp2A0=((struct Cyc_Absyn_TunionType_struct*)
_tmp29F)->f1;_tmp2A1=(void*)_tmp2A0.tunion_info;if(*((int*)_tmp2A1)!= 1)goto
_LL221;_tmp2A2=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A1)->f1;_tmp2A3=*
_tmp2A2;_tmp2A4=_tmp2A0.targs;_tmp2A5=(void*)_tmp2A0.rgn;_tmp2A6=_tmp287.f2;if(
_tmp2A6 <= (void*)3  || *((int*)_tmp2A6)!= 2)goto _LL221;_tmp2A7=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A6)->f1;_tmp2A8=(void*)_tmp2A7.tunion_info;if(*((int*)_tmp2A8)!= 1)goto
_LL221;_tmp2A9=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A8)->f1;_tmp2AA=*
_tmp2A9;_tmp2AB=_tmp2A7.targs;_tmp2AC=(void*)_tmp2A7.rgn;_LL220: if(_tmp2A3 == 
_tmp2AA  || Cyc_Absyn_qvar_cmp(_tmp2A3->name,_tmp2AA->name)== 0){Cyc_Tcutil_unify_it(
_tmp2AC,_tmp2A5);Cyc_Tcutil_unify_list(_tmp2AB,_tmp2A4);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp336="(different tunion types)";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,25));});goto _LL212;_LL221: _tmp2AD=_tmp287.f1;if(
_tmp2AD <= (void*)3  || *((int*)_tmp2AD)!= 3)goto _LL223;_tmp2AE=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2AD)->f1;_tmp2AF=(void*)_tmp2AE.field_info;if(*((int*)_tmp2AF)!= 1)goto
_LL223;_tmp2B0=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AF)->f1;_tmp2B1=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2AF)->f2;_tmp2B2=_tmp2AE.targs;
_tmp2B3=_tmp287.f2;if(_tmp2B3 <= (void*)3  || *((int*)_tmp2B3)!= 3)goto _LL223;
_tmp2B4=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B3)->f1;_tmp2B5=(void*)
_tmp2B4.field_info;if(*((int*)_tmp2B5)!= 1)goto _LL223;_tmp2B6=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2B5)->f1;_tmp2B7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B5)->f2;
_tmp2B8=_tmp2B4.targs;_LL222: if((_tmp2B0 == _tmp2B6  || Cyc_Absyn_qvar_cmp(_tmp2B0->name,
_tmp2B6->name)== 0) && (_tmp2B1 == _tmp2B7  || Cyc_Absyn_qvar_cmp(_tmp2B1->name,
_tmp2B7->name)== 0)){Cyc_Tcutil_unify_list(_tmp2B8,_tmp2B2);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp337="(different tunion field types)";_tag_arr(_tmp337,sizeof(char),
_get_zero_arr_size(_tmp337,31));});goto _LL212;_LL223: _tmp2B9=_tmp287.f1;if(
_tmp2B9 <= (void*)3  || *((int*)_tmp2B9)!= 4)goto _LL225;_tmp2BA=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B9)->f1;_tmp2BB=(void*)_tmp2BA.elt_typ;_tmp2BC=_tmp2BA.elt_tq;_tmp2BD=
_tmp2BA.ptr_atts;_tmp2BE=(void*)_tmp2BD.rgn;_tmp2BF=_tmp2BD.nullable;_tmp2C0=
_tmp2BD.bounds;_tmp2C1=_tmp2BD.zero_term;_tmp2C2=_tmp2BD.loc;_tmp2C3=_tmp287.f2;
if(_tmp2C3 <= (void*)3  || *((int*)_tmp2C3)!= 4)goto _LL225;_tmp2C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C3)->f1;_tmp2C5=(void*)_tmp2C4.elt_typ;_tmp2C6=_tmp2C4.elt_tq;_tmp2C7=
_tmp2C4.ptr_atts;_tmp2C8=(void*)_tmp2C7.rgn;_tmp2C9=_tmp2C7.nullable;_tmp2CA=
_tmp2C7.bounds;_tmp2CB=_tmp2C7.zero_term;_tmp2CC=_tmp2C7.loc;_LL224: Cyc_Tcutil_unify_it(
_tmp2C5,_tmp2BB);Cyc_Tcutil_unify_it(_tmp2BE,_tmp2C8);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2CB,_tmp2C1,({const char*_tmp338="(not both zero terminated)";
_tag_arr(_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,27));}));Cyc_Tcutil_unify_tqual(
_tmp2C6,_tmp2C5,_tmp2BC,_tmp2BB);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2CA,_tmp2C0,({const char*_tmp339="(different pointer bounds)";_tag_arr(
_tmp339,sizeof(char),_get_zero_arr_size(_tmp339,27));}));{void*_tmp33A=(void*)(
Cyc_Absyn_compress_conref(_tmp2CA))->v;void*_tmp33B;_LL24C: if(_tmp33A <= (void*)1
 || *((int*)_tmp33A)!= 0)goto _LL24E;_tmp33B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp33A)->f1;if((int)_tmp33B != 0)goto _LL24E;_LL24D: return;_LL24E:;_LL24F: goto
_LL24B;_LL24B:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C9,
_tmp2BF,({const char*_tmp33C="(incompatible pointer types)";_tag_arr(_tmp33C,
sizeof(char),_get_zero_arr_size(_tmp33C,29));}));return;_LL225: _tmp2CD=_tmp287.f1;
if(_tmp2CD <= (void*)3  || *((int*)_tmp2CD)!= 5)goto _LL227;_tmp2CE=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2CD)->f1;_tmp2CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CD)->f2;_tmp2D0=_tmp287.f2;if(_tmp2D0 <= (void*)3  || *((int*)_tmp2D0)!= 5)
goto _LL227;_tmp2D1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D0)->f1;_tmp2D2=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2D0)->f2;_LL226: if(_tmp2D1 == _tmp2CE
 && _tmp2D2 == _tmp2CF)return;Cyc_Tcutil_failure_reason=({const char*_tmp33D="(different integral types)";
_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,27));});goto _LL212;
_LL227: _tmp2D3=_tmp287.f1;if((int)_tmp2D3 != 1)goto _LL229;_tmp2D4=_tmp287.f2;if((
int)_tmp2D4 != 1)goto _LL229;_LL228: return;_LL229: _tmp2D5=_tmp287.f1;if(_tmp2D5 <= (
void*)3  || *((int*)_tmp2D5)!= 6)goto _LL22B;_tmp2D6=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2D5)->f1;_tmp2D7=_tmp287.f2;if(_tmp2D7 <= (void*)3  || *((int*)_tmp2D7)!= 6)
goto _LL22B;_tmp2D8=((struct Cyc_Absyn_DoubleType_struct*)_tmp2D7)->f1;_LL22A: if(
_tmp2D6 == _tmp2D8)return;goto _LL212;_LL22B: _tmp2D9=_tmp287.f1;if(_tmp2D9 <= (void*)
3  || *((int*)_tmp2D9)!= 14)goto _LL22D;_tmp2DA=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2D9)->f1;_tmp2DB=_tmp287.f2;if(_tmp2DB <= (void*)3  || *((int*)_tmp2DB)!= 14)
goto _LL22D;_tmp2DC=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2DB)->f1;
_LL22C: Cyc_Tcutil_unify_it(_tmp2DA,_tmp2DC);return;_LL22D: _tmp2DD=_tmp287.f1;if(
_tmp2DD <= (void*)3  || *((int*)_tmp2DD)!= 18)goto _LL22F;_tmp2DE=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2DD)->f1;_tmp2DF=_tmp287.f2;if(_tmp2DF <= (void*)3  || *((int*)_tmp2DF)!= 18)
goto _LL22F;_tmp2E0=((struct Cyc_Absyn_TypeInt_struct*)_tmp2DF)->f1;_LL22E: if(
_tmp2DE == _tmp2E0)return;Cyc_Tcutil_failure_reason=({const char*_tmp33E="(different type integers)";
_tag_arr(_tmp33E,sizeof(char),_get_zero_arr_size(_tmp33E,26));});goto _LL212;
_LL22F: _tmp2E1=_tmp287.f1;if(_tmp2E1 <= (void*)3  || *((int*)_tmp2E1)!= 17)goto
_LL231;_tmp2E2=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E1)->f1;_tmp2E3=
_tmp287.f2;if(_tmp2E3 <= (void*)3  || *((int*)_tmp2E3)!= 17)goto _LL231;_tmp2E4=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E3)->f1;_LL230: Cyc_Tcutil_unify_it(
_tmp2E2,_tmp2E4);return;_LL231: _tmp2E5=_tmp287.f1;if(_tmp2E5 <= (void*)3  || *((
int*)_tmp2E5)!= 7)goto _LL233;_tmp2E6=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E5)->f1;
_tmp2E7=(void*)_tmp2E6.elt_type;_tmp2E8=_tmp2E6.tq;_tmp2E9=_tmp2E6.num_elts;
_tmp2EA=_tmp2E6.zero_term;_tmp2EB=_tmp287.f2;if(_tmp2EB <= (void*)3  || *((int*)
_tmp2EB)!= 7)goto _LL233;_tmp2EC=((struct Cyc_Absyn_ArrayType_struct*)_tmp2EB)->f1;
_tmp2ED=(void*)_tmp2EC.elt_type;_tmp2EE=_tmp2EC.tq;_tmp2EF=_tmp2EC.num_elts;
_tmp2F0=_tmp2EC.zero_term;_LL232: Cyc_Tcutil_unify_it(_tmp2ED,_tmp2E7);Cyc_Tcutil_unify_tqual(
_tmp2EE,_tmp2ED,_tmp2E8,_tmp2E7);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2EA,
_tmp2F0,({const char*_tmp33F="(not both zero terminated)";_tag_arr(_tmp33F,
sizeof(char),_get_zero_arr_size(_tmp33F,27));}));if(_tmp2E9 == _tmp2EF)return;if(
_tmp2E9 == 0  || _tmp2EF == 0)goto _LL212;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2E9,(struct Cyc_Absyn_Exp*)_tmp2EF))return;Cyc_Tcutil_failure_reason=({const
char*_tmp340="(different array sizes)";_tag_arr(_tmp340,sizeof(char),
_get_zero_arr_size(_tmp340,24));});goto _LL212;_LL233: _tmp2F1=_tmp287.f1;if(
_tmp2F1 <= (void*)3  || *((int*)_tmp2F1)!= 8)goto _LL235;_tmp2F2=((struct Cyc_Absyn_FnType_struct*)
_tmp2F1)->f1;_tmp2F3=_tmp2F2.tvars;_tmp2F4=_tmp2F2.effect;_tmp2F5=(void*)_tmp2F2.ret_typ;
_tmp2F6=_tmp2F2.args;_tmp2F7=_tmp2F2.c_varargs;_tmp2F8=_tmp2F2.cyc_varargs;
_tmp2F9=_tmp2F2.rgn_po;_tmp2FA=_tmp2F2.attributes;_tmp2FB=_tmp287.f2;if(_tmp2FB
<= (void*)3  || *((int*)_tmp2FB)!= 8)goto _LL235;_tmp2FC=((struct Cyc_Absyn_FnType_struct*)
_tmp2FB)->f1;_tmp2FD=_tmp2FC.tvars;_tmp2FE=_tmp2FC.effect;_tmp2FF=(void*)_tmp2FC.ret_typ;
_tmp300=_tmp2FC.args;_tmp301=_tmp2FC.c_varargs;_tmp302=_tmp2FC.cyc_varargs;
_tmp303=_tmp2FC.rgn_po;_tmp304=_tmp2FC.attributes;_LL234: {int done=0;{struct
_RegionHandle _tmp341=_new_region("rgn");struct _RegionHandle*rgn=& _tmp341;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2FD != 0){if(_tmp2F3 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp342="(second function type has too few type variables)";
_tag_arr(_tmp342,sizeof(char),_get_zero_arr_size(_tmp342,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp343=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2FD->hd);void*_tmp344=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2F3->hd);
if(_tmp343 != _tmp344){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp349;_tmp349.tag=0;_tmp349.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp344));{struct Cyc_String_pa_struct _tmp348;_tmp348.tag=0;_tmp348.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp343));{struct Cyc_String_pa_struct
_tmp347;_tmp347.tag=0;_tmp347.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2FD->hd));{void*_tmp345[3]={& _tmp347,& _tmp348,& _tmp349};
Cyc_aprintf(({const char*_tmp346="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp346,sizeof(char),_get_zero_arr_size(_tmp346,44));}),_tag_arr(
_tmp345,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp34A=_region_malloc(rgn,sizeof(*_tmp34A));_tmp34A->hd=({
struct _tuple8*_tmp34B=_region_malloc(rgn,sizeof(*_tmp34B));_tmp34B->f1=(struct
Cyc_Absyn_Tvar*)_tmp2F3->hd;_tmp34B->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Absyn_VarType_struct
_tmp34D;_tmp34D.tag=1;_tmp34D.f1=(struct Cyc_Absyn_Tvar*)_tmp2FD->hd;_tmp34D;});
_tmp34C;});_tmp34B;});_tmp34A->tl=inst;_tmp34A;});_tmp2FD=_tmp2FD->tl;_tmp2F3=
_tmp2F3->tl;}}if(_tmp2F3 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp34E="(second function type has too many type variables)";
_tag_arr(_tmp34E,sizeof(char),_get_zero_arr_size(_tmp34E,51));});_npop_handler(0);
goto _LL212;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=({struct Cyc_Absyn_FnType_struct
_tmp350;_tmp350.tag=8;_tmp350.f1=({struct Cyc_Absyn_FnInfo _tmp351;_tmp351.tvars=0;
_tmp351.effect=_tmp2FE;_tmp351.ret_typ=(void*)_tmp2FF;_tmp351.args=_tmp300;
_tmp351.c_varargs=_tmp301;_tmp351.cyc_varargs=_tmp302;_tmp351.rgn_po=_tmp303;
_tmp351.attributes=_tmp304;_tmp351;});_tmp350;});_tmp34F;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp352=_cycalloc(sizeof(*
_tmp352));_tmp352[0]=({struct Cyc_Absyn_FnType_struct _tmp353;_tmp353.tag=8;
_tmp353.f1=({struct Cyc_Absyn_FnInfo _tmp354;_tmp354.tvars=0;_tmp354.effect=
_tmp2F4;_tmp354.ret_typ=(void*)_tmp2F5;_tmp354.args=_tmp2F6;_tmp354.c_varargs=
_tmp2F7;_tmp354.cyc_varargs=_tmp2F8;_tmp354.rgn_po=_tmp2F9;_tmp354.attributes=
_tmp2FA;_tmp354;});_tmp353;});_tmp352;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2FF,_tmp2F5);for(0;_tmp300 != 0  && _tmp2F6 != 0;(
_tmp300=_tmp300->tl,_tmp2F6=_tmp2F6->tl)){struct Cyc_Absyn_Tqual _tmp356;void*
_tmp357;struct _tuple2 _tmp355=*((struct _tuple2*)_tmp300->hd);_tmp356=_tmp355.f2;
_tmp357=_tmp355.f3;{struct Cyc_Absyn_Tqual _tmp359;void*_tmp35A;struct _tuple2
_tmp358=*((struct _tuple2*)_tmp2F6->hd);_tmp359=_tmp358.f2;_tmp35A=_tmp358.f3;Cyc_Tcutil_unify_it(
_tmp357,_tmp35A);Cyc_Tcutil_unify_tqual(_tmp356,_tmp357,_tmp359,_tmp35A);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp300 != 0  || _tmp2F6 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp35B="(function types have different number of arguments)";_tag_arr(
_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,52));});goto _LL212;}if(_tmp301 != 
_tmp2F7){Cyc_Tcutil_failure_reason=({const char*_tmp35C="(only one function type takes C varargs)";
_tag_arr(_tmp35C,sizeof(char),_get_zero_arr_size(_tmp35C,41));});goto _LL212;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp35E=({struct _tuple11 _tmp35D;_tmp35D.f1=
_tmp302;_tmp35D.f2=_tmp2F8;_tmp35D;});struct Cyc_Absyn_VarargInfo*_tmp35F;struct
Cyc_Absyn_VarargInfo*_tmp360;struct Cyc_Absyn_VarargInfo*_tmp361;struct Cyc_Absyn_VarargInfo*
_tmp362;struct Cyc_Absyn_VarargInfo*_tmp363;struct Cyc_Absyn_VarargInfo _tmp364;
struct Cyc_Core_Opt*_tmp365;struct Cyc_Absyn_Tqual _tmp366;void*_tmp367;int _tmp368;
struct Cyc_Absyn_VarargInfo*_tmp369;struct Cyc_Absyn_VarargInfo _tmp36A;struct Cyc_Core_Opt*
_tmp36B;struct Cyc_Absyn_Tqual _tmp36C;void*_tmp36D;int _tmp36E;_LL251: _tmp35F=
_tmp35E.f1;if(_tmp35F != 0)goto _LL253;_tmp360=_tmp35E.f2;if(_tmp360 != 0)goto
_LL253;_LL252: goto _LL250;_LL253: _tmp361=_tmp35E.f1;if(_tmp361 != 0)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp362=_tmp35E.f2;if(_tmp362 != 0)goto _LL257;_LL256:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp36F="(only one function type takes varargs)";
_tag_arr(_tmp36F,sizeof(char),_get_zero_arr_size(_tmp36F,39));});goto _LL250;
_LL257: _tmp363=_tmp35E.f1;if(_tmp363 == 0)goto _LL250;_tmp364=*_tmp363;_tmp365=
_tmp364.name;_tmp366=_tmp364.tq;_tmp367=(void*)_tmp364.type;_tmp368=_tmp364.inject;
_tmp369=_tmp35E.f2;if(_tmp369 == 0)goto _LL250;_tmp36A=*_tmp369;_tmp36B=_tmp36A.name;
_tmp36C=_tmp36A.tq;_tmp36D=(void*)_tmp36A.type;_tmp36E=_tmp36A.inject;_LL258: Cyc_Tcutil_unify_it(
_tmp367,_tmp36D);Cyc_Tcutil_unify_tqual(_tmp366,_tmp367,_tmp36C,_tmp36D);if(
_tmp368 != _tmp36E){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp370="(only one function type injects varargs)";_tag_arr(_tmp370,sizeof(char),
_get_zero_arr_size(_tmp370,41));});}goto _LL250;_LL250:;}if(bad_cyc_vararg)goto
_LL212;{int bad_effect=0;{struct _tuple12 _tmp372=({struct _tuple12 _tmp371;_tmp371.f1=
_tmp2FE;_tmp371.f2=_tmp2F4;_tmp371;});struct Cyc_Core_Opt*_tmp373;struct Cyc_Core_Opt*
_tmp374;struct Cyc_Core_Opt*_tmp375;struct Cyc_Core_Opt*_tmp376;_LL25A: _tmp373=
_tmp372.f1;if(_tmp373 != 0)goto _LL25C;_tmp374=_tmp372.f2;if(_tmp374 != 0)goto
_LL25C;_LL25B: goto _LL259;_LL25C: _tmp375=_tmp372.f1;if(_tmp375 != 0)goto _LL25E;
_LL25D: goto _LL25F;_LL25E: _tmp376=_tmp372.f2;if(_tmp376 != 0)goto _LL260;_LL25F:
bad_effect=1;goto _LL259;_LL260:;_LL261: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2FE))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F4))->v);goto _LL259;_LL259:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp377="(function type effects do not unify)";
_tag_arr(_tmp377,sizeof(char),_get_zero_arr_size(_tmp377,37));});goto _LL212;}if(
!Cyc_Tcutil_same_atts(_tmp2FA,_tmp304)){Cyc_Tcutil_failure_reason=({const char*
_tmp378="(function types have different attributes)";_tag_arr(_tmp378,sizeof(
char),_get_zero_arr_size(_tmp378,43));});goto _LL212;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2F9,_tmp303)){Cyc_Tcutil_failure_reason=({const char*_tmp379="(function types have different region lifetime orderings)";
_tag_arr(_tmp379,sizeof(char),_get_zero_arr_size(_tmp379,58));});goto _LL212;}
return;}}}_LL235: _tmp305=_tmp287.f1;if(_tmp305 <= (void*)3  || *((int*)_tmp305)!= 
9)goto _LL237;_tmp306=((struct Cyc_Absyn_TupleType_struct*)_tmp305)->f1;_tmp307=
_tmp287.f2;if(_tmp307 <= (void*)3  || *((int*)_tmp307)!= 9)goto _LL237;_tmp308=((
struct Cyc_Absyn_TupleType_struct*)_tmp307)->f1;_LL236: for(0;_tmp308 != 0  && 
_tmp306 != 0;(_tmp308=_tmp308->tl,_tmp306=_tmp306->tl)){struct Cyc_Absyn_Tqual
_tmp37B;void*_tmp37C;struct _tuple4 _tmp37A=*((struct _tuple4*)_tmp308->hd);_tmp37B=
_tmp37A.f1;_tmp37C=_tmp37A.f2;{struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37F;struct
_tuple4 _tmp37D=*((struct _tuple4*)_tmp306->hd);_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;
Cyc_Tcutil_unify_it(_tmp37C,_tmp37F);Cyc_Tcutil_unify_tqual(_tmp37B,_tmp37C,
_tmp37E,_tmp37F);}}if(_tmp308 == 0  && _tmp306 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp380="(tuple types have different numbers of components)";
_tag_arr(_tmp380,sizeof(char),_get_zero_arr_size(_tmp380,51));});goto _LL212;
_LL237: _tmp309=_tmp287.f1;if(_tmp309 <= (void*)3  || *((int*)_tmp309)!= 11)goto
_LL239;_tmp30A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp309)->f1;
_tmp30B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp309)->f2;_tmp30C=_tmp287.f2;
if(_tmp30C <= (void*)3  || *((int*)_tmp30C)!= 11)goto _LL239;_tmp30D=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp30C)->f1;_tmp30E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp30C)->f2;_LL238: if(_tmp30D != _tmp30A){Cyc_Tcutil_failure_reason=({const char*
_tmp381="(struct and union type)";_tag_arr(_tmp381,sizeof(char),
_get_zero_arr_size(_tmp381,24));});goto _LL212;}for(0;_tmp30E != 0  && _tmp30B != 0;(
_tmp30E=_tmp30E->tl,_tmp30B=_tmp30B->tl)){struct Cyc_Absyn_Aggrfield*_tmp382=(
struct Cyc_Absyn_Aggrfield*)_tmp30E->hd;struct Cyc_Absyn_Aggrfield*_tmp383=(struct
Cyc_Absyn_Aggrfield*)_tmp30B->hd;if(Cyc_strptrcmp(_tmp382->name,_tmp383->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp384="(different member names)";
_tag_arr(_tmp384,sizeof(char),_get_zero_arr_size(_tmp384,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp382->type,(void*)_tmp383->type);
Cyc_Tcutil_unify_tqual(_tmp382->tq,(void*)_tmp382->type,_tmp383->tq,(void*)
_tmp383->type);if(!Cyc_Tcutil_same_atts(_tmp382->attributes,_tmp383->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp385="(different attributes on member)";_tag_arr(_tmp385,sizeof(
char),_get_zero_arr_size(_tmp385,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp382->width != 0  && _tmp383->width == 0  || _tmp383->width != 0  && _tmp382->width
== 0) || (_tmp382->width != 0  && _tmp383->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp382->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp383->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp386="(different bitfield widths on member)";
_tag_arr(_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp30E == 0  && _tmp30B == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp387="(different number of members)";
_tag_arr(_tmp387,sizeof(char),_get_zero_arr_size(_tmp387,30));});goto _LL212;
_LL239: _tmp30F=_tmp287.f1;if((int)_tmp30F != 2)goto _LL23B;_tmp310=_tmp287.f2;if((
int)_tmp310 != 2)goto _LL23B;_LL23A: return;_LL23B: _tmp311=_tmp287.f1;if(_tmp311 <= (
void*)3  || *((int*)_tmp311)!= 15)goto _LL23D;_tmp312=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp311)->f1;_tmp313=_tmp287.f2;if(_tmp313 <= (void*)3  || *((int*)_tmp313)!= 15)
goto _LL23D;_tmp314=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp313)->f1;
_LL23C: Cyc_Tcutil_unify_it(_tmp312,_tmp314);return;_LL23D: _tmp315=_tmp287.f1;if(
_tmp315 <= (void*)3  || *((int*)_tmp315)!= 20)goto _LL23F;_LL23E: goto _LL240;_LL23F:
_tmp316=_tmp287.f2;if(_tmp316 <= (void*)3  || *((int*)_tmp316)!= 20)goto _LL241;
_LL240: goto _LL242;_LL241: _tmp317=_tmp287.f1;if(_tmp317 <= (void*)3  || *((int*)
_tmp317)!= 19)goto _LL243;_LL242: goto _LL244;_LL243: _tmp318=_tmp287.f1;if(_tmp318
<= (void*)3  || *((int*)_tmp318)!= 21)goto _LL245;_LL244: goto _LL246;_LL245: _tmp319=
_tmp287.f2;if(_tmp319 <= (void*)3  || *((int*)_tmp319)!= 21)goto _LL247;_LL246: goto
_LL248;_LL247: _tmp31A=_tmp287.f2;if(_tmp31A <= (void*)3  || *((int*)_tmp31A)!= 19)
goto _LL249;_LL248: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp388="(effects don't unify)";_tag_arr(_tmp388,sizeof(char),
_get_zero_arr_size(_tmp388,22));});goto _LL212;_LL249:;_LL24A: goto _LL212;_LL212:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp389=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp38A=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp389,_tmp38A);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp38B=(void*)x->v;void*
_tmp38C;_LL263: if((int)_tmp38B != 0)goto _LL265;_LL264: return - 1;_LL265: if(_tmp38B
<= (void*)1  || *((int*)_tmp38B)!= 0)goto _LL267;_tmp38C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp38B)->f1;_LL266: {void*_tmp38D=(void*)y->v;void*_tmp38E;_LL26A: if((int)
_tmp38D != 0)goto _LL26C;_LL26B: return 1;_LL26C: if(_tmp38D <= (void*)1  || *((int*)
_tmp38D)!= 0)goto _LL26E;_tmp38E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp38D)->f1;_LL26D: return cmp(_tmp38C,_tmp38E);_LL26E: if(_tmp38D <= (void*)1  || *((
int*)_tmp38D)!= 1)goto _LL269;_LL26F:({void*_tmp38F[0]={};Cyc_Tcutil_impos(({
const char*_tmp390="unify_conref: forward after compress(2)";_tag_arr(_tmp390,
sizeof(char),_get_zero_arr_size(_tmp390,40));}),_tag_arr(_tmp38F,sizeof(void*),0));});
_LL269:;}_LL267: if(_tmp38B <= (void*)1  || *((int*)_tmp38B)!= 1)goto _LL262;_LL268:({
void*_tmp391[0]={};Cyc_Tcutil_impos(({const char*_tmp392="unify_conref: forward after compress";
_tag_arr(_tmp392,sizeof(char),_get_zero_arr_size(_tmp392,37));}),_tag_arr(
_tmp391,sizeof(void*),0));});_LL262:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp394;struct Cyc_Absyn_Tqual
_tmp395;void*_tmp396;struct _tuple4*_tmp393=tqt1;_tmp394=*_tmp393;_tmp395=_tmp394.f1;
_tmp396=_tmp394.f2;{struct _tuple4 _tmp398;struct Cyc_Absyn_Tqual _tmp399;void*
_tmp39A;struct _tuple4*_tmp397=tqt2;_tmp398=*_tmp397;_tmp399=_tmp398.f1;_tmp39A=
_tmp398.f2;{int _tmp39B=Cyc_Tcutil_tqual_cmp(_tmp395,_tmp399);if(_tmp39B != 0)
return _tmp39B;return Cyc_Tcutil_typecmp(_tmp396,_tmp39A);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp39C=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp39C != 0)return _tmp39C;{int _tmp39D=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp39D != 0)return _tmp39D;{int _tmp39E=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp39E != 0)return _tmp39E;{int _tmp39F=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp39F != 0)return
_tmp39F;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp3A0=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3A0 != 0)return _tmp3A0;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3A1=t;_LL271: if((int)_tmp3A1 != 0)goto _LL273;_LL272: return 0;
_LL273: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 0)goto _LL275;_LL274: return 1;
_LL275: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 1)goto _LL277;_LL276: return 2;
_LL277: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 2)goto _LL279;_LL278: return 3;
_LL279: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 3)goto _LL27B;_LL27A: return 4;
_LL27B: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 4)goto _LL27D;_LL27C: return 5;
_LL27D: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 5)goto _LL27F;_LL27E: return 6;
_LL27F: if((int)_tmp3A1 != 1)goto _LL281;_LL280: return 7;_LL281: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 6)goto _LL283;_LL282: return 8;_LL283: if(_tmp3A1 <= (void*)3
 || *((int*)_tmp3A1)!= 7)goto _LL285;_LL284: return 9;_LL285: if(_tmp3A1 <= (void*)3
 || *((int*)_tmp3A1)!= 8)goto _LL287;_LL286: return 10;_LL287: if(_tmp3A1 <= (void*)3
 || *((int*)_tmp3A1)!= 9)goto _LL289;_LL288: return 11;_LL289: if(_tmp3A1 <= (void*)3
 || *((int*)_tmp3A1)!= 10)goto _LL28B;_LL28A: return 12;_LL28B: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 11)goto _LL28D;_LL28C: return 14;_LL28D: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 12)goto _LL28F;_LL28E: return 16;_LL28F: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 13)goto _LL291;_LL290: return 17;_LL291: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 15)goto _LL293;_LL292: return 18;_LL293: if(_tmp3A1 <= (void*)
3  || *((int*)_tmp3A1)!= 16)goto _LL295;_LL294: return 19;_LL295: if((int)_tmp3A1 != 2)
goto _LL297;_LL296: return 20;_LL297: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 19)
goto _LL299;_LL298: return 21;_LL299: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 20)
goto _LL29B;_LL29A: return 22;_LL29B: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 21)
goto _LL29D;_LL29C: return 23;_LL29D: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 14)
goto _LL29F;_LL29E: return 24;_LL29F: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 18)
goto _LL2A1;_LL2A0: return 25;_LL2A1: if(_tmp3A1 <= (void*)3  || *((int*)_tmp3A1)!= 17)
goto _LL270;_LL2A2: return 26;_LL270:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=
Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int
_tmp3A2=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3A2 != 0)return _tmp3A2;{struct _tuple6 _tmp3A4=({struct _tuple6 _tmp3A3;
_tmp3A3.f1=t2;_tmp3A3.f2=t1;_tmp3A3;});void*_tmp3A5;void*_tmp3A6;void*_tmp3A7;
struct Cyc_Absyn_Tvar*_tmp3A8;void*_tmp3A9;struct Cyc_Absyn_Tvar*_tmp3AA;void*
_tmp3AB;struct Cyc_Absyn_AggrInfo _tmp3AC;void*_tmp3AD;struct Cyc_List_List*_tmp3AE;
void*_tmp3AF;struct Cyc_Absyn_AggrInfo _tmp3B0;void*_tmp3B1;struct Cyc_List_List*
_tmp3B2;void*_tmp3B3;struct _tuple1*_tmp3B4;void*_tmp3B5;struct _tuple1*_tmp3B6;
void*_tmp3B7;struct Cyc_List_List*_tmp3B8;void*_tmp3B9;struct Cyc_List_List*
_tmp3BA;void*_tmp3BB;struct Cyc_Absyn_TunionInfo _tmp3BC;void*_tmp3BD;struct Cyc_Absyn_Tuniondecl**
_tmp3BE;struct Cyc_Absyn_Tuniondecl*_tmp3BF;struct Cyc_List_List*_tmp3C0;void*
_tmp3C1;void*_tmp3C2;struct Cyc_Absyn_TunionInfo _tmp3C3;void*_tmp3C4;struct Cyc_Absyn_Tuniondecl**
_tmp3C5;struct Cyc_Absyn_Tuniondecl*_tmp3C6;struct Cyc_List_List*_tmp3C7;void*
_tmp3C8;void*_tmp3C9;struct Cyc_Absyn_TunionFieldInfo _tmp3CA;void*_tmp3CB;struct
Cyc_Absyn_Tuniondecl*_tmp3CC;struct Cyc_Absyn_Tunionfield*_tmp3CD;struct Cyc_List_List*
_tmp3CE;void*_tmp3CF;struct Cyc_Absyn_TunionFieldInfo _tmp3D0;void*_tmp3D1;struct
Cyc_Absyn_Tuniondecl*_tmp3D2;struct Cyc_Absyn_Tunionfield*_tmp3D3;struct Cyc_List_List*
_tmp3D4;void*_tmp3D5;struct Cyc_Absyn_PtrInfo _tmp3D6;void*_tmp3D7;struct Cyc_Absyn_Tqual
_tmp3D8;struct Cyc_Absyn_PtrAtts _tmp3D9;void*_tmp3DA;struct Cyc_Absyn_Conref*
_tmp3DB;struct Cyc_Absyn_Conref*_tmp3DC;struct Cyc_Absyn_Conref*_tmp3DD;void*
_tmp3DE;struct Cyc_Absyn_PtrInfo _tmp3DF;void*_tmp3E0;struct Cyc_Absyn_Tqual _tmp3E1;
struct Cyc_Absyn_PtrAtts _tmp3E2;void*_tmp3E3;struct Cyc_Absyn_Conref*_tmp3E4;
struct Cyc_Absyn_Conref*_tmp3E5;struct Cyc_Absyn_Conref*_tmp3E6;void*_tmp3E7;void*
_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*_tmp3EB;void*_tmp3EC;void*_tmp3ED;void*
_tmp3EE;void*_tmp3EF;int _tmp3F0;void*_tmp3F1;int _tmp3F2;void*_tmp3F3;struct Cyc_Absyn_ArrayInfo
_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_Tqual _tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;
struct Cyc_Absyn_Conref*_tmp3F8;void*_tmp3F9;struct Cyc_Absyn_ArrayInfo _tmp3FA;
void*_tmp3FB;struct Cyc_Absyn_Tqual _tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Conref*
_tmp3FE;void*_tmp3FF;struct Cyc_Absyn_FnInfo _tmp400;struct Cyc_List_List*_tmp401;
struct Cyc_Core_Opt*_tmp402;void*_tmp403;struct Cyc_List_List*_tmp404;int _tmp405;
struct Cyc_Absyn_VarargInfo*_tmp406;struct Cyc_List_List*_tmp407;struct Cyc_List_List*
_tmp408;void*_tmp409;struct Cyc_Absyn_FnInfo _tmp40A;struct Cyc_List_List*_tmp40B;
struct Cyc_Core_Opt*_tmp40C;void*_tmp40D;struct Cyc_List_List*_tmp40E;int _tmp40F;
struct Cyc_Absyn_VarargInfo*_tmp410;struct Cyc_List_List*_tmp411;struct Cyc_List_List*
_tmp412;void*_tmp413;struct Cyc_List_List*_tmp414;void*_tmp415;struct Cyc_List_List*
_tmp416;void*_tmp417;void*_tmp418;struct Cyc_List_List*_tmp419;void*_tmp41A;void*
_tmp41B;struct Cyc_List_List*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp41F;void*
_tmp420;void*_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*_tmp425;void*
_tmp426;void*_tmp427;void*_tmp428;void*_tmp429;int _tmp42A;void*_tmp42B;int
_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*_tmp431;void*
_tmp432;_LL2A4: _tmp3A5=_tmp3A4.f1;if(_tmp3A5 <= (void*)3  || *((int*)_tmp3A5)!= 0)
goto _LL2A6;_tmp3A6=_tmp3A4.f2;if(_tmp3A6 <= (void*)3  || *((int*)_tmp3A6)!= 0)goto
_LL2A6;_LL2A5:({void*_tmp433[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp434="typecmp: can only compare closed types";
_tag_arr(_tmp434,sizeof(char),_get_zero_arr_size(_tmp434,39));}),_tag_arr(
_tmp433,sizeof(void*),0));});_LL2A6: _tmp3A7=_tmp3A4.f1;if(_tmp3A7 <= (void*)3  || *((
int*)_tmp3A7)!= 1)goto _LL2A8;_tmp3A8=((struct Cyc_Absyn_VarType_struct*)_tmp3A7)->f1;
_tmp3A9=_tmp3A4.f2;if(_tmp3A9 <= (void*)3  || *((int*)_tmp3A9)!= 1)goto _LL2A8;
_tmp3AA=((struct Cyc_Absyn_VarType_struct*)_tmp3A9)->f1;_LL2A7: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3AA->identity)),*((int*)_check_null(_tmp3A8->identity)));
_LL2A8: _tmp3AB=_tmp3A4.f1;if(_tmp3AB <= (void*)3  || *((int*)_tmp3AB)!= 10)goto
_LL2AA;_tmp3AC=((struct Cyc_Absyn_AggrType_struct*)_tmp3AB)->f1;_tmp3AD=(void*)
_tmp3AC.aggr_info;_tmp3AE=_tmp3AC.targs;_tmp3AF=_tmp3A4.f2;if(_tmp3AF <= (void*)3
 || *((int*)_tmp3AF)!= 10)goto _LL2AA;_tmp3B0=((struct Cyc_Absyn_AggrType_struct*)
_tmp3AF)->f1;_tmp3B1=(void*)_tmp3B0.aggr_info;_tmp3B2=_tmp3B0.targs;_LL2A9: {
struct _tuple1*_tmp436;struct _tuple5 _tmp435=Cyc_Absyn_aggr_kinded_name(_tmp3AD);
_tmp436=_tmp435.f2;{struct _tuple1*_tmp438;struct _tuple5 _tmp437=Cyc_Absyn_aggr_kinded_name(
_tmp3B1);_tmp438=_tmp437.f2;{int _tmp439=Cyc_Absyn_qvar_cmp(_tmp436,_tmp438);if(
_tmp439 != 0)return _tmp439;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3AE,_tmp3B2);}}}}_LL2AA: _tmp3B3=_tmp3A4.f1;if(_tmp3B3 <= (void*)3  || *((int*)
_tmp3B3)!= 12)goto _LL2AC;_tmp3B4=((struct Cyc_Absyn_EnumType_struct*)_tmp3B3)->f1;
_tmp3B5=_tmp3A4.f2;if(_tmp3B5 <= (void*)3  || *((int*)_tmp3B5)!= 12)goto _LL2AC;
_tmp3B6=((struct Cyc_Absyn_EnumType_struct*)_tmp3B5)->f1;_LL2AB: return Cyc_Absyn_qvar_cmp(
_tmp3B4,_tmp3B6);_LL2AC: _tmp3B7=_tmp3A4.f1;if(_tmp3B7 <= (void*)3  || *((int*)
_tmp3B7)!= 13)goto _LL2AE;_tmp3B8=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B7)->f1;
_tmp3B9=_tmp3A4.f2;if(_tmp3B9 <= (void*)3  || *((int*)_tmp3B9)!= 13)goto _LL2AE;
_tmp3BA=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B9)->f1;_LL2AD: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B8,
_tmp3BA);_LL2AE: _tmp3BB=_tmp3A4.f1;if(_tmp3BB <= (void*)3  || *((int*)_tmp3BB)!= 2)
goto _LL2B0;_tmp3BC=((struct Cyc_Absyn_TunionType_struct*)_tmp3BB)->f1;_tmp3BD=(
void*)_tmp3BC.tunion_info;if(*((int*)_tmp3BD)!= 1)goto _LL2B0;_tmp3BE=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3BD)->f1;_tmp3BF=*_tmp3BE;_tmp3C0=_tmp3BC.targs;_tmp3C1=(void*)_tmp3BC.rgn;
_tmp3C2=_tmp3A4.f2;if(_tmp3C2 <= (void*)3  || *((int*)_tmp3C2)!= 2)goto _LL2B0;
_tmp3C3=((struct Cyc_Absyn_TunionType_struct*)_tmp3C2)->f1;_tmp3C4=(void*)_tmp3C3.tunion_info;
if(*((int*)_tmp3C4)!= 1)goto _LL2B0;_tmp3C5=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3C4)->f1;_tmp3C6=*_tmp3C5;_tmp3C7=_tmp3C3.targs;_tmp3C8=(void*)_tmp3C3.rgn;
_LL2AF: if(_tmp3C6 == _tmp3BF)return 0;{int _tmp43A=Cyc_Absyn_qvar_cmp(_tmp3C6->name,
_tmp3BF->name);if(_tmp43A != 0)return _tmp43A;{int _tmp43B=Cyc_Tcutil_typecmp(
_tmp3C8,_tmp3C1);if(_tmp43B != 0)return _tmp43B;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C7,_tmp3C0);}}_LL2B0: _tmp3C9=_tmp3A4.f1;if(_tmp3C9 <= (void*)3  || *((int*)
_tmp3C9)!= 3)goto _LL2B2;_tmp3CA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3C9)->f1;_tmp3CB=(void*)_tmp3CA.field_info;if(*((int*)_tmp3CB)!= 1)goto
_LL2B2;_tmp3CC=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CB)->f1;_tmp3CD=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CB)->f2;_tmp3CE=_tmp3CA.targs;
_tmp3CF=_tmp3A4.f2;if(_tmp3CF <= (void*)3  || *((int*)_tmp3CF)!= 3)goto _LL2B2;
_tmp3D0=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3CF)->f1;_tmp3D1=(void*)
_tmp3D0.field_info;if(*((int*)_tmp3D1)!= 1)goto _LL2B2;_tmp3D2=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3D1)->f1;_tmp3D3=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D1)->f2;
_tmp3D4=_tmp3D0.targs;_LL2B1: if(_tmp3D2 == _tmp3CC)return 0;{int _tmp43C=Cyc_Absyn_qvar_cmp(
_tmp3CC->name,_tmp3D2->name);if(_tmp43C != 0)return _tmp43C;{int _tmp43D=Cyc_Absyn_qvar_cmp(
_tmp3CD->name,_tmp3D3->name);if(_tmp43D != 0)return _tmp43D;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D4,_tmp3CE);}}_LL2B2: _tmp3D5=_tmp3A4.f1;if(_tmp3D5 <= (
void*)3  || *((int*)_tmp3D5)!= 4)goto _LL2B4;_tmp3D6=((struct Cyc_Absyn_PointerType_struct*)
_tmp3D5)->f1;_tmp3D7=(void*)_tmp3D6.elt_typ;_tmp3D8=_tmp3D6.elt_tq;_tmp3D9=
_tmp3D6.ptr_atts;_tmp3DA=(void*)_tmp3D9.rgn;_tmp3DB=_tmp3D9.nullable;_tmp3DC=
_tmp3D9.bounds;_tmp3DD=_tmp3D9.zero_term;_tmp3DE=_tmp3A4.f2;if(_tmp3DE <= (void*)
3  || *((int*)_tmp3DE)!= 4)goto _LL2B4;_tmp3DF=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DE)->f1;_tmp3E0=(void*)_tmp3DF.elt_typ;_tmp3E1=_tmp3DF.elt_tq;_tmp3E2=
_tmp3DF.ptr_atts;_tmp3E3=(void*)_tmp3E2.rgn;_tmp3E4=_tmp3E2.nullable;_tmp3E5=
_tmp3E2.bounds;_tmp3E6=_tmp3E2.zero_term;_LL2B3: {int _tmp43E=Cyc_Tcutil_typecmp(
_tmp3E0,_tmp3D7);if(_tmp43E != 0)return _tmp43E;{int _tmp43F=Cyc_Tcutil_typecmp(
_tmp3E3,_tmp3DA);if(_tmp43F != 0)return _tmp43F;{int _tmp440=Cyc_Tcutil_tqual_cmp(
_tmp3E1,_tmp3D8);if(_tmp440 != 0)return _tmp440;{int _tmp441=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3E5,_tmp3DC);if(_tmp441 != 0)return _tmp441;{int _tmp442=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3E6,_tmp3DD);if(_tmp442 != 0)return _tmp442;{void*_tmp443=(
void*)(Cyc_Absyn_compress_conref(_tmp3E5))->v;void*_tmp444;_LL2D9: if(_tmp443 <= (
void*)1  || *((int*)_tmp443)!= 0)goto _LL2DB;_tmp444=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp443)->f1;if((int)_tmp444 != 0)goto _LL2DB;_LL2DA: return 0;_LL2DB:;_LL2DC: goto
_LL2D8;_LL2D8:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E4,
_tmp3DB);}}}}}_LL2B4: _tmp3E7=_tmp3A4.f1;if(_tmp3E7 <= (void*)3  || *((int*)_tmp3E7)
!= 5)goto _LL2B6;_tmp3E8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E7)->f1;
_tmp3E9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E7)->f2;_tmp3EA=_tmp3A4.f2;
if(_tmp3EA <= (void*)3  || *((int*)_tmp3EA)!= 5)goto _LL2B6;_tmp3EB=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3EA)->f1;_tmp3EC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3EA)->f2;_LL2B5: if(_tmp3EB != _tmp3E8)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3EB),(int)((unsigned int)_tmp3E8));if(_tmp3EC != _tmp3E9)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3EC),(int)((unsigned int)_tmp3E9));
return 0;_LL2B6: _tmp3ED=_tmp3A4.f1;if((int)_tmp3ED != 1)goto _LL2B8;_tmp3EE=_tmp3A4.f2;
if((int)_tmp3EE != 1)goto _LL2B8;_LL2B7: return 0;_LL2B8: _tmp3EF=_tmp3A4.f1;if(
_tmp3EF <= (void*)3  || *((int*)_tmp3EF)!= 6)goto _LL2BA;_tmp3F0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3EF)->f1;_tmp3F1=_tmp3A4.f2;if(_tmp3F1 <= (void*)3  || *((int*)_tmp3F1)!= 6)
goto _LL2BA;_tmp3F2=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F1)->f1;_LL2B9: if(
_tmp3F0 == _tmp3F2)return 0;else{if(_tmp3F0)return - 1;else{return 1;}}_LL2BA: _tmp3F3=
_tmp3A4.f1;if(_tmp3F3 <= (void*)3  || *((int*)_tmp3F3)!= 7)goto _LL2BC;_tmp3F4=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F3)->f1;_tmp3F5=(void*)_tmp3F4.elt_type;
_tmp3F6=_tmp3F4.tq;_tmp3F7=_tmp3F4.num_elts;_tmp3F8=_tmp3F4.zero_term;_tmp3F9=
_tmp3A4.f2;if(_tmp3F9 <= (void*)3  || *((int*)_tmp3F9)!= 7)goto _LL2BC;_tmp3FA=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F9)->f1;_tmp3FB=(void*)_tmp3FA.elt_type;
_tmp3FC=_tmp3FA.tq;_tmp3FD=_tmp3FA.num_elts;_tmp3FE=_tmp3FA.zero_term;_LL2BB: {
int _tmp445=Cyc_Tcutil_tqual_cmp(_tmp3FC,_tmp3F6);if(_tmp445 != 0)return _tmp445;{
int _tmp446=Cyc_Tcutil_typecmp(_tmp3FB,_tmp3F5);if(_tmp446 != 0)return _tmp446;{int
_tmp447=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3F8,_tmp3FE);if(_tmp447 != 0)
return _tmp447;if(_tmp3F7 == _tmp3FD)return 0;if(_tmp3F7 == 0  || _tmp3FD == 0)({void*
_tmp448[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp449="missing expression in array index";_tag_arr(_tmp449,sizeof(
char),_get_zero_arr_size(_tmp449,34));}),_tag_arr(_tmp448,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3F7,
_tmp3FD);}}}_LL2BC: _tmp3FF=_tmp3A4.f1;if(_tmp3FF <= (void*)3  || *((int*)_tmp3FF)
!= 8)goto _LL2BE;_tmp400=((struct Cyc_Absyn_FnType_struct*)_tmp3FF)->f1;_tmp401=
_tmp400.tvars;_tmp402=_tmp400.effect;_tmp403=(void*)_tmp400.ret_typ;_tmp404=
_tmp400.args;_tmp405=_tmp400.c_varargs;_tmp406=_tmp400.cyc_varargs;_tmp407=
_tmp400.rgn_po;_tmp408=_tmp400.attributes;_tmp409=_tmp3A4.f2;if(_tmp409 <= (void*)
3  || *((int*)_tmp409)!= 8)goto _LL2BE;_tmp40A=((struct Cyc_Absyn_FnType_struct*)
_tmp409)->f1;_tmp40B=_tmp40A.tvars;_tmp40C=_tmp40A.effect;_tmp40D=(void*)_tmp40A.ret_typ;
_tmp40E=_tmp40A.args;_tmp40F=_tmp40A.c_varargs;_tmp410=_tmp40A.cyc_varargs;
_tmp411=_tmp40A.rgn_po;_tmp412=_tmp40A.attributes;_LL2BD:({void*_tmp44A[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44B="typecmp: function types not handled";_tag_arr(_tmp44B,sizeof(char),
_get_zero_arr_size(_tmp44B,36));}),_tag_arr(_tmp44A,sizeof(void*),0));});_LL2BE:
_tmp413=_tmp3A4.f1;if(_tmp413 <= (void*)3  || *((int*)_tmp413)!= 9)goto _LL2C0;
_tmp414=((struct Cyc_Absyn_TupleType_struct*)_tmp413)->f1;_tmp415=_tmp3A4.f2;if(
_tmp415 <= (void*)3  || *((int*)_tmp415)!= 9)goto _LL2C0;_tmp416=((struct Cyc_Absyn_TupleType_struct*)
_tmp415)->f1;_LL2BF: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp416,_tmp414);_LL2C0: _tmp417=_tmp3A4.f1;if(_tmp417 <= (void*)3  || *((int*)
_tmp417)!= 11)goto _LL2C2;_tmp418=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp417)->f1;_tmp419=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp417)->f2;_tmp41A=
_tmp3A4.f2;if(_tmp41A <= (void*)3  || *((int*)_tmp41A)!= 11)goto _LL2C2;_tmp41B=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41A)->f1;_tmp41C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp41A)->f2;_LL2C1: if(_tmp41B != _tmp418){if(_tmp41B == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp41C,_tmp419);_LL2C2: _tmp41D=_tmp3A4.f1;if(_tmp41D <= (void*)3  || *((int*)
_tmp41D)!= 15)goto _LL2C4;_tmp41E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41D)->f1;_tmp41F=_tmp3A4.f2;if(_tmp41F <= (void*)3  || *((int*)_tmp41F)!= 15)
goto _LL2C4;_tmp420=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp41F)->f1;
_LL2C3: return Cyc_Tcutil_typecmp(_tmp41E,_tmp420);_LL2C4: _tmp421=_tmp3A4.f1;if(
_tmp421 <= (void*)3  || *((int*)_tmp421)!= 14)goto _LL2C6;_tmp422=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp421)->f1;_tmp423=_tmp3A4.f2;if(_tmp423 <= (void*)
3  || *((int*)_tmp423)!= 14)goto _LL2C6;_tmp424=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp423)->f1;_LL2C5: return Cyc_Tcutil_typecmp(_tmp422,_tmp424);_LL2C6: _tmp425=
_tmp3A4.f1;if(_tmp425 <= (void*)3  || *((int*)_tmp425)!= 17)goto _LL2C8;_tmp426=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp425)->f1;_tmp427=_tmp3A4.f2;if(
_tmp427 <= (void*)3  || *((int*)_tmp427)!= 17)goto _LL2C8;_tmp428=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp427)->f1;_LL2C7: return Cyc_Tcutil_typecmp(_tmp426,
_tmp428);_LL2C8: _tmp429=_tmp3A4.f1;if(_tmp429 <= (void*)3  || *((int*)_tmp429)!= 
18)goto _LL2CA;_tmp42A=((struct Cyc_Absyn_TypeInt_struct*)_tmp429)->f1;_tmp42B=
_tmp3A4.f2;if(_tmp42B <= (void*)3  || *((int*)_tmp42B)!= 18)goto _LL2CA;_tmp42C=((
struct Cyc_Absyn_TypeInt_struct*)_tmp42B)->f1;_LL2C9: return Cyc_Core_intcmp(
_tmp42A,_tmp42C);_LL2CA: _tmp42D=_tmp3A4.f1;if(_tmp42D <= (void*)3  || *((int*)
_tmp42D)!= 20)goto _LL2CC;_LL2CB: goto _LL2CD;_LL2CC: _tmp42E=_tmp3A4.f2;if(_tmp42E
<= (void*)3  || *((int*)_tmp42E)!= 20)goto _LL2CE;_LL2CD: goto _LL2CF;_LL2CE: _tmp42F=
_tmp3A4.f1;if(_tmp42F <= (void*)3  || *((int*)_tmp42F)!= 19)goto _LL2D0;_LL2CF: goto
_LL2D1;_LL2D0: _tmp430=_tmp3A4.f1;if(_tmp430 <= (void*)3  || *((int*)_tmp430)!= 21)
goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp431=_tmp3A4.f2;if(_tmp431 <= (void*)3  || *((
int*)_tmp431)!= 21)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4: _tmp432=_tmp3A4.f2;if(
_tmp432 <= (void*)3  || *((int*)_tmp432)!= 19)goto _LL2D6;_LL2D5:({void*_tmp44C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44D="typecmp: effects not handled";_tag_arr(_tmp44D,sizeof(char),
_get_zero_arr_size(_tmp44D,29));}),_tag_arr(_tmp44C,sizeof(void*),0));});_LL2D6:;
_LL2D7:({void*_tmp44E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp44F="Unmatched case in typecmp";_tag_arr(
_tmp44F,sizeof(char),_get_zero_arr_size(_tmp44F,26));}),_tag_arr(_tmp44E,sizeof(
void*),0));});_LL2A3:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp450=
Cyc_Tcutil_compress(t);_LL2DE: if(_tmp450 <= (void*)3  || *((int*)_tmp450)!= 5)goto
_LL2E0;_LL2DF: goto _LL2E1;_LL2E0: if((int)_tmp450 != 1)goto _LL2E2;_LL2E1: goto _LL2E3;
_LL2E2: if(_tmp450 <= (void*)3  || *((int*)_tmp450)!= 6)goto _LL2E4;_LL2E3: goto
_LL2E5;_LL2E4: if(_tmp450 <= (void*)3  || *((int*)_tmp450)!= 12)goto _LL2E6;_LL2E5:
goto _LL2E7;_LL2E6: if(_tmp450 <= (void*)3  || *((int*)_tmp450)!= 13)goto _LL2E8;
_LL2E7: return 1;_LL2E8:;_LL2E9: return 0;_LL2DD:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp452=({struct _tuple6 _tmp451;_tmp451.f1=Cyc_Tcutil_compress(
t1);_tmp451.f2=Cyc_Tcutil_compress(t2);_tmp451;});void*_tmp453;int _tmp454;void*
_tmp455;int _tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*_tmp45A;void*
_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;void*
_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*_tmp466;void*
_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*_tmp46C;void*
_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*_tmp472;void*
_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*_tmp478;void*
_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*_tmp47E;void*
_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*_tmp484;void*
_tmp485;_LL2EB: _tmp453=_tmp452.f1;if(_tmp453 <= (void*)3  || *((int*)_tmp453)!= 6)
goto _LL2ED;_tmp454=((struct Cyc_Absyn_DoubleType_struct*)_tmp453)->f1;_tmp455=
_tmp452.f2;if(_tmp455 <= (void*)3  || *((int*)_tmp455)!= 6)goto _LL2ED;_tmp456=((
struct Cyc_Absyn_DoubleType_struct*)_tmp455)->f1;_LL2EC: return !_tmp456  && _tmp454;
_LL2ED: _tmp457=_tmp452.f1;if(_tmp457 <= (void*)3  || *((int*)_tmp457)!= 6)goto
_LL2EF;_tmp458=_tmp452.f2;if((int)_tmp458 != 1)goto _LL2EF;_LL2EE: goto _LL2F0;
_LL2EF: _tmp459=_tmp452.f1;if(_tmp459 <= (void*)3  || *((int*)_tmp459)!= 6)goto
_LL2F1;_tmp45A=_tmp452.f2;if(_tmp45A <= (void*)3  || *((int*)_tmp45A)!= 5)goto
_LL2F1;_LL2F0: goto _LL2F2;_LL2F1: _tmp45B=_tmp452.f1;if(_tmp45B <= (void*)3  || *((
int*)_tmp45B)!= 6)goto _LL2F3;_tmp45C=_tmp452.f2;if(_tmp45C <= (void*)3  || *((int*)
_tmp45C)!= 14)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: _tmp45D=_tmp452.f1;if((int)
_tmp45D != 1)goto _LL2F5;_tmp45E=_tmp452.f2;if(_tmp45E <= (void*)3  || *((int*)
_tmp45E)!= 14)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: _tmp45F=_tmp452.f1;if(_tmp45F
<= (void*)3  || *((int*)_tmp45F)!= 6)goto _LL2F7;_tmp460=_tmp452.f2;if(_tmp460 <= (
void*)3  || *((int*)_tmp460)!= 17)goto _LL2F7;_LL2F6: goto _LL2F8;_LL2F7: _tmp461=
_tmp452.f1;if((int)_tmp461 != 1)goto _LL2F9;_tmp462=_tmp452.f2;if(_tmp462 <= (void*)
3  || *((int*)_tmp462)!= 17)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp463=_tmp452.f1;
if((int)_tmp463 != 1)goto _LL2FB;_tmp464=_tmp452.f2;if(_tmp464 <= (void*)3  || *((
int*)_tmp464)!= 5)goto _LL2FB;_LL2FA: return 1;_LL2FB: _tmp465=_tmp452.f1;if(_tmp465
<= (void*)3  || *((int*)_tmp465)!= 5)goto _LL2FD;_tmp466=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp465)->f2;if((int)_tmp466 != 3)goto _LL2FD;_tmp467=_tmp452.f2;if(_tmp467 <= (
void*)3  || *((int*)_tmp467)!= 5)goto _LL2FD;_tmp468=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp467)->f2;if((int)_tmp468 != 3)goto _LL2FD;_LL2FC: return 0;_LL2FD: _tmp469=
_tmp452.f1;if(_tmp469 <= (void*)3  || *((int*)_tmp469)!= 5)goto _LL2FF;_tmp46A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp469)->f2;if((int)_tmp46A != 3)goto
_LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp46B=_tmp452.f1;if(_tmp46B <= (void*)3  || *((
int*)_tmp46B)!= 5)goto _LL301;_tmp46C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46B)->f2;if((int)_tmp46C != 2)goto _LL301;_tmp46D=_tmp452.f2;if((int)_tmp46D != 
1)goto _LL301;_LL300: goto _LL302;_LL301: _tmp46E=_tmp452.f1;if(_tmp46E <= (void*)3
 || *((int*)_tmp46E)!= 5)goto _LL303;_tmp46F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46E)->f2;if((int)_tmp46F != 2)goto _LL303;_tmp470=_tmp452.f2;if(_tmp470 <= (
void*)3  || *((int*)_tmp470)!= 5)goto _LL303;_tmp471=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp470)->f2;if((int)_tmp471 != 1)goto _LL303;_LL302: goto _LL304;_LL303: _tmp472=
_tmp452.f1;if(_tmp472 <= (void*)3  || *((int*)_tmp472)!= 5)goto _LL305;_tmp473=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp472)->f2;if((int)_tmp473 != 2)goto
_LL305;_tmp474=_tmp452.f2;if(_tmp474 <= (void*)3  || *((int*)_tmp474)!= 5)goto
_LL305;_tmp475=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp474)->f2;if((int)
_tmp475 != 0)goto _LL305;_LL304: goto _LL306;_LL305: _tmp476=_tmp452.f1;if(_tmp476 <= (
void*)3  || *((int*)_tmp476)!= 5)goto _LL307;_tmp477=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp476)->f2;if((int)_tmp477 != 1)goto _LL307;_tmp478=_tmp452.f2;if(_tmp478 <= (
void*)3  || *((int*)_tmp478)!= 5)goto _LL307;_tmp479=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp478)->f2;if((int)_tmp479 != 0)goto _LL307;_LL306: goto _LL308;_LL307: _tmp47A=
_tmp452.f1;if(_tmp47A <= (void*)3  || *((int*)_tmp47A)!= 17)goto _LL309;_tmp47B=
_tmp452.f2;if(_tmp47B <= (void*)3  || *((int*)_tmp47B)!= 5)goto _LL309;_tmp47C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47B)->f2;if((int)_tmp47C != 1)goto
_LL309;_LL308: goto _LL30A;_LL309: _tmp47D=_tmp452.f1;if(_tmp47D <= (void*)3  || *((
int*)_tmp47D)!= 17)goto _LL30B;_tmp47E=_tmp452.f2;if(_tmp47E <= (void*)3  || *((int*)
_tmp47E)!= 5)goto _LL30B;_tmp47F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47E)->f2;
if((int)_tmp47F != 0)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp480=_tmp452.f1;if(
_tmp480 <= (void*)3  || *((int*)_tmp480)!= 14)goto _LL30D;_tmp481=_tmp452.f2;if(
_tmp481 <= (void*)3  || *((int*)_tmp481)!= 5)goto _LL30D;_tmp482=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp481)->f2;if((int)_tmp482 != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp483=
_tmp452.f1;if(_tmp483 <= (void*)3  || *((int*)_tmp483)!= 14)goto _LL30F;_tmp484=
_tmp452.f2;if(_tmp484 <= (void*)3  || *((int*)_tmp484)!= 5)goto _LL30F;_tmp485=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp484)->f2;if((int)_tmp485 != 0)goto
_LL30F;_LL30E: return 1;_LL30F:;_LL310: return 0;_LL2EA:;}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp486=
_cycalloc(sizeof(*_tmp486));_tmp486->v=(void*)t1;_tmp486;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp48A;_tmp48A.tag=0;_tmp48A.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp489;_tmp489.tag=0;_tmp489.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp487[2]={& _tmp489,& _tmp48A};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,({const char*_tmp488="type mismatch: expecting %s but found %s";
_tag_arr(_tmp488,sizeof(char),_get_zero_arr_size(_tmp488,41));}),_tag_arr(
_tmp487,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp48B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL312: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 4)goto _LL314;_LL313: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL311;_LL314:;_LL315: return 0;_LL311:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp48C=Cyc_Tcutil_compress(
t);_LL317: if(_tmp48C <= (void*)3  || *((int*)_tmp48C)!= 5)goto _LL319;_LL318: goto
_LL31A;_LL319: if(_tmp48C <= (void*)3  || *((int*)_tmp48C)!= 14)goto _LL31B;_LL31A:
goto _LL31C;_LL31B: if(_tmp48C <= (void*)3  || *((int*)_tmp48C)!= 17)goto _LL31D;
_LL31C: goto _LL31E;_LL31D: if(_tmp48C <= (void*)3  || *((int*)_tmp48C)!= 12)goto
_LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp48C <= (void*)3  || *((int*)_tmp48C)!= 13)
goto _LL321;_LL320: return 1;_LL321:;_LL322: return 0;_LL316:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp48D[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48E="integral size mismatch; conversion supplied";
_tag_arr(_tmp48E,sizeof(char),_get_zero_arr_size(_tmp48E,44));}),_tag_arr(
_tmp48D,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp48F[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp490="integral size mismatch; conversion supplied";
_tag_arr(_tmp490,sizeof(char),_get_zero_arr_size(_tmp490,44));}),_tag_arr(
_tmp48F,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp492=({struct _tuple6 _tmp491;_tmp491.f1=
t1;_tmp491.f2=t2;_tmp491;});void*_tmp493;struct Cyc_Absyn_PtrInfo _tmp494;void*
_tmp495;struct Cyc_Absyn_PtrInfo _tmp496;void*_tmp497;struct Cyc_Absyn_ArrayInfo
_tmp498;void*_tmp499;struct Cyc_Absyn_Tqual _tmp49A;struct Cyc_Absyn_Exp*_tmp49B;
struct Cyc_Absyn_Conref*_tmp49C;void*_tmp49D;struct Cyc_Absyn_ArrayInfo _tmp49E;
void*_tmp49F;struct Cyc_Absyn_Tqual _tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Conref*
_tmp4A2;void*_tmp4A3;struct Cyc_Absyn_TunionFieldInfo _tmp4A4;void*_tmp4A5;struct
Cyc_Absyn_Tuniondecl*_tmp4A6;struct Cyc_Absyn_Tunionfield*_tmp4A7;struct Cyc_List_List*
_tmp4A8;void*_tmp4A9;struct Cyc_Absyn_TunionInfo _tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Tuniondecl**
_tmp4AC;struct Cyc_Absyn_Tuniondecl*_tmp4AD;struct Cyc_List_List*_tmp4AE;void*
_tmp4AF;struct Cyc_Absyn_PtrInfo _tmp4B0;void*_tmp4B1;struct Cyc_Absyn_Tqual _tmp4B2;
struct Cyc_Absyn_PtrAtts _tmp4B3;void*_tmp4B4;struct Cyc_Absyn_Conref*_tmp4B5;
struct Cyc_Absyn_Conref*_tmp4B6;struct Cyc_Absyn_Conref*_tmp4B7;void*_tmp4B8;
struct Cyc_Absyn_TunionInfo _tmp4B9;void*_tmp4BA;struct Cyc_Absyn_Tuniondecl**
_tmp4BB;struct Cyc_Absyn_Tuniondecl*_tmp4BC;struct Cyc_List_List*_tmp4BD;void*
_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;void*_tmp4C3;_LL324:
_tmp493=_tmp492.f1;if(_tmp493 <= (void*)3  || *((int*)_tmp493)!= 4)goto _LL326;
_tmp494=((struct Cyc_Absyn_PointerType_struct*)_tmp493)->f1;_tmp495=_tmp492.f2;
if(_tmp495 <= (void*)3  || *((int*)_tmp495)!= 4)goto _LL326;_tmp496=((struct Cyc_Absyn_PointerType_struct*)
_tmp495)->f1;_LL325: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp494.ptr_atts).nullable,(
_tmp496.ptr_atts).nullable)){void*_tmp4C4=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp494.ptr_atts).nullable))->v;
int _tmp4C5;_LL333: if(_tmp4C4 <= (void*)1  || *((int*)_tmp4C4)!= 0)goto _LL335;
_tmp4C5=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C4)->f1;_LL334: okay=!
_tmp4C5;goto _LL332;_LL335:;_LL336:({void*_tmp4C6[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4C7="silent_castable conref not eq";
_tag_arr(_tmp4C7,sizeof(char),_get_zero_arr_size(_tmp4C7,30));}),_tag_arr(
_tmp4C6,sizeof(void*),0));});_LL332:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp494.ptr_atts).bounds,(_tmp496.ptr_atts).bounds)){struct _tuple6 _tmp4C9=({
struct _tuple6 _tmp4C8;_tmp4C8.f1=(void*)(Cyc_Absyn_compress_conref((_tmp494.ptr_atts).bounds))->v;
_tmp4C8.f2=(void*)(Cyc_Absyn_compress_conref((_tmp496.ptr_atts).bounds))->v;
_tmp4C8;});void*_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;
_LL338: _tmp4CA=_tmp4C9.f1;if(_tmp4CA <= (void*)1  || *((int*)_tmp4CA)!= 0)goto
_LL33A;_tmp4CB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4CA)->f1;_tmp4CC=
_tmp4C9.f2;if(_tmp4CC <= (void*)1  || *((int*)_tmp4CC)!= 0)goto _LL33A;_tmp4CD=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4CC)->f1;_LL339:{struct _tuple6
_tmp4D0=({struct _tuple6 _tmp4CF;_tmp4CF.f1=_tmp4CB;_tmp4CF.f2=_tmp4CD;_tmp4CF;});
void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_Exp*
_tmp4D6;void*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;void*_tmp4D9;void*_tmp4DA;
struct Cyc_Absyn_Exp*_tmp4DB;void*_tmp4DC;void*_tmp4DD;void*_tmp4DE;void*_tmp4DF;
void*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;void*_tmp4E2;void*_tmp4E3;void*_tmp4E4;
void*_tmp4E5;_LL33F: _tmp4D1=_tmp4D0.f1;if(_tmp4D1 <= (void*)1  || *((int*)_tmp4D1)
!= 0)goto _LL341;_tmp4D2=_tmp4D0.f2;if((int)_tmp4D2 != 0)goto _LL341;_LL340: goto
_LL342;_LL341: _tmp4D3=_tmp4D0.f1;if((int)_tmp4D3 != 0)goto _LL343;_tmp4D4=_tmp4D0.f2;
if((int)_tmp4D4 != 0)goto _LL343;_LL342: okay=1;goto _LL33E;_LL343: _tmp4D5=_tmp4D0.f1;
if(_tmp4D5 <= (void*)1  || *((int*)_tmp4D5)!= 0)goto _LL345;_tmp4D6=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D5)->f1;_tmp4D7=_tmp4D0.f2;if(_tmp4D7 <= (void*)1  || *((int*)_tmp4D7)!= 0)
goto _LL345;_tmp4D8=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D7)->f1;_LL344: okay=
okay  && Cyc_Evexp_lte_const_exp(_tmp4D8,_tmp4D6);if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp496.ptr_atts).zero_term))({void*_tmp4E6[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4E7="implicit cast to shorter array";
_tag_arr(_tmp4E7,sizeof(char),_get_zero_arr_size(_tmp4E7,31));}),_tag_arr(
_tmp4E6,sizeof(void*),0));});goto _LL33E;_LL345: _tmp4D9=_tmp4D0.f1;if((int)
_tmp4D9 != 0)goto _LL347;_tmp4DA=_tmp4D0.f2;if(_tmp4DA <= (void*)1  || *((int*)
_tmp4DA)!= 0)goto _LL347;_tmp4DB=((struct Cyc_Absyn_Upper_b_struct*)_tmp4DA)->f1;
_LL346: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp494.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((_tmp496.ptr_atts).bounds))
goto _LL33E;okay=0;goto _LL33E;_LL347: _tmp4DC=_tmp4D0.f1;if(_tmp4DC <= (void*)1  || *((
int*)_tmp4DC)!= 1)goto _LL349;_tmp4DD=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4DC)->f1;_tmp4DE=_tmp4D0.f2;if(_tmp4DE <= (void*)1  || *((int*)_tmp4DE)!= 1)
goto _LL349;_tmp4DF=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DE)->f1;
_LL348: if(!Cyc_Tcutil_unify(_tmp4DD,_tmp4DF)){struct _tuple6 _tmp4E9=({struct
_tuple6 _tmp4E8;_tmp4E8.f1=Cyc_Tcutil_compress(_tmp4DD);_tmp4E8.f2=Cyc_Tcutil_compress(
_tmp4DF);_tmp4E8;});void*_tmp4EA;int _tmp4EB;void*_tmp4EC;int _tmp4ED;_LL350:
_tmp4EA=_tmp4E9.f1;if(_tmp4EA <= (void*)3  || *((int*)_tmp4EA)!= 18)goto _LL352;
_tmp4EB=((struct Cyc_Absyn_TypeInt_struct*)_tmp4EA)->f1;_tmp4EC=_tmp4E9.f2;if(
_tmp4EC <= (void*)3  || *((int*)_tmp4EC)!= 18)goto _LL352;_tmp4ED=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4EC)->f1;_LL351: if(_tmp4EB < _tmp4ED)okay=0;goto _LL34F;_LL352:;_LL353: okay=0;
goto _LL34F;_LL34F:;}goto _LL33E;_LL349: _tmp4E0=_tmp4D0.f1;if(_tmp4E0 <= (void*)1
 || *((int*)_tmp4E0)!= 0)goto _LL34B;_tmp4E1=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4E0)->f1;_tmp4E2=_tmp4D0.f2;if(_tmp4E2 <= (void*)1  || *((int*)_tmp4E2)!= 1)
goto _LL34B;_tmp4E3=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4E2)->f1;
_LL34A: {unsigned int _tmp4EF;int _tmp4F0;struct _tuple7 _tmp4EE=Cyc_Evexp_eval_const_uint_exp(
_tmp4E1);_tmp4EF=_tmp4EE.f1;_tmp4F0=_tmp4EE.f2;if(!_tmp4F0){okay=0;goto _LL33E;}{
void*_tmp4F1=Cyc_Tcutil_compress(_tmp4E3);int _tmp4F2;_LL355: if(_tmp4F1 <= (void*)
3  || *((int*)_tmp4F1)!= 18)goto _LL357;_tmp4F2=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4F1)->f1;_LL356: if(_tmp4EF < _tmp4F2)okay=0;goto _LL354;_LL357:;_LL358: okay=0;
goto _LL354;_LL354:;}goto _LL33E;}_LL34B: _tmp4E4=_tmp4D0.f1;if(_tmp4E4 <= (void*)1
 || *((int*)_tmp4E4)!= 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D: _tmp4E5=_tmp4D0.f2;
if(_tmp4E5 <= (void*)1  || *((int*)_tmp4E5)!= 1)goto _LL33E;_LL34E: okay=0;goto
_LL33E;_LL33E:;}goto _LL337;_LL33A: _tmp4CE=_tmp4C9.f1;if((int)_tmp4CE != 0)goto
_LL33C;_LL33B: okay=0;goto _LL337;_LL33C:;_LL33D:({void*_tmp4F3[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4F4="silent_castable conrefs didn't unify";
_tag_arr(_tmp4F4,sizeof(char),_get_zero_arr_size(_tmp4F4,37));}),_tag_arr(
_tmp4F3,sizeof(void*),0));});_LL337:;}okay=okay  && Cyc_Tcutil_unify((void*)
_tmp494.elt_typ,(void*)_tmp496.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(
_tmp494.ptr_atts).rgn,(void*)(_tmp496.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp494.ptr_atts).rgn,(void*)(_tmp496.ptr_atts).rgn));okay=okay  && (!(
_tmp494.elt_tq).real_const  || (_tmp496.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp494.ptr_atts).zero_term,(_tmp496.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp494.ptr_atts).zero_term)
 && (_tmp496.elt_tq).real_const);return okay;}_LL326: _tmp497=_tmp492.f1;if(
_tmp497 <= (void*)3  || *((int*)_tmp497)!= 7)goto _LL328;_tmp498=((struct Cyc_Absyn_ArrayType_struct*)
_tmp497)->f1;_tmp499=(void*)_tmp498.elt_type;_tmp49A=_tmp498.tq;_tmp49B=_tmp498.num_elts;
_tmp49C=_tmp498.zero_term;_tmp49D=_tmp492.f2;if(_tmp49D <= (void*)3  || *((int*)
_tmp49D)!= 7)goto _LL328;_tmp49E=((struct Cyc_Absyn_ArrayType_struct*)_tmp49D)->f1;
_tmp49F=(void*)_tmp49E.elt_type;_tmp4A0=_tmp49E.tq;_tmp4A1=_tmp49E.num_elts;
_tmp4A2=_tmp49E.zero_term;_LL327: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp49C,_tmp4A2) && ((_tmp49B != 0  && _tmp4A1 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp49B,(struct Cyc_Absyn_Exp*)_tmp4A1));return(okay  && Cyc_Tcutil_unify(
_tmp499,_tmp49F)) && (!_tmp49A.real_const  || _tmp4A0.real_const);}_LL328: _tmp4A3=
_tmp492.f1;if(_tmp4A3 <= (void*)3  || *((int*)_tmp4A3)!= 3)goto _LL32A;_tmp4A4=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4A3)->f1;_tmp4A5=(void*)_tmp4A4.field_info;
if(*((int*)_tmp4A5)!= 1)goto _LL32A;_tmp4A6=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4A5)->f1;_tmp4A7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4A5)->f2;
_tmp4A8=_tmp4A4.targs;_tmp4A9=_tmp492.f2;if(_tmp4A9 <= (void*)3  || *((int*)
_tmp4A9)!= 2)goto _LL32A;_tmp4AA=((struct Cyc_Absyn_TunionType_struct*)_tmp4A9)->f1;
_tmp4AB=(void*)_tmp4AA.tunion_info;if(*((int*)_tmp4AB)!= 1)goto _LL32A;_tmp4AC=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp4AB)->f1;_tmp4AD=*_tmp4AC;_tmp4AE=
_tmp4AA.targs;_LL329: if((_tmp4A6 == _tmp4AD  || Cyc_Absyn_qvar_cmp(_tmp4A6->name,
_tmp4AD->name)== 0) && _tmp4A7->typs == 0){for(0;_tmp4A8 != 0  && _tmp4AE != 0;(
_tmp4A8=_tmp4A8->tl,_tmp4AE=_tmp4AE->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4A8->hd,(
void*)_tmp4AE->hd))break;}if(_tmp4A8 == 0  && _tmp4AE == 0)return 1;}return 0;_LL32A:
_tmp4AF=_tmp492.f1;if(_tmp4AF <= (void*)3  || *((int*)_tmp4AF)!= 4)goto _LL32C;
_tmp4B0=((struct Cyc_Absyn_PointerType_struct*)_tmp4AF)->f1;_tmp4B1=(void*)
_tmp4B0.elt_typ;_tmp4B2=_tmp4B0.elt_tq;_tmp4B3=_tmp4B0.ptr_atts;_tmp4B4=(void*)
_tmp4B3.rgn;_tmp4B5=_tmp4B3.nullable;_tmp4B6=_tmp4B3.bounds;_tmp4B7=_tmp4B3.zero_term;
_tmp4B8=_tmp492.f2;if(_tmp4B8 <= (void*)3  || *((int*)_tmp4B8)!= 2)goto _LL32C;
_tmp4B9=((struct Cyc_Absyn_TunionType_struct*)_tmp4B8)->f1;_tmp4BA=(void*)_tmp4B9.tunion_info;
if(*((int*)_tmp4BA)!= 1)goto _LL32C;_tmp4BB=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4BA)->f1;_tmp4BC=*_tmp4BB;_tmp4BD=_tmp4B9.targs;_tmp4BE=(void*)_tmp4B9.rgn;
_LL32B:{void*_tmp4F5=Cyc_Tcutil_compress(_tmp4B1);struct Cyc_Absyn_TunionFieldInfo
_tmp4F6;void*_tmp4F7;struct Cyc_Absyn_Tuniondecl*_tmp4F8;struct Cyc_Absyn_Tunionfield*
_tmp4F9;struct Cyc_List_List*_tmp4FA;_LL35A: if(_tmp4F5 <= (void*)3  || *((int*)
_tmp4F5)!= 3)goto _LL35C;_tmp4F6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4F5)->f1;_tmp4F7=(void*)_tmp4F6.field_info;if(*((int*)_tmp4F7)!= 1)goto
_LL35C;_tmp4F8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F7)->f1;_tmp4F9=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F7)->f2;_tmp4FA=_tmp4F6.targs;
_LL35B: if(!Cyc_Tcutil_unify(_tmp4B4,_tmp4BE) && !Cyc_Tcenv_region_outlives(te,
_tmp4B4,_tmp4BE))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4B5,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4B6,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4B7,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4BC->name,
_tmp4F8->name)== 0  && _tmp4F9->typs != 0){int okay=1;for(0;_tmp4FA != 0  && _tmp4BD
!= 0;(_tmp4FA=_tmp4FA->tl,_tmp4BD=_tmp4BD->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp4FA->hd,(void*)_tmp4BD->hd)){okay=0;break;}}if((!okay  || _tmp4FA != 0) || 
_tmp4BD != 0)return 0;return 1;}goto _LL359;_LL35C:;_LL35D: goto _LL359;_LL359:;}
return 0;_LL32C: _tmp4BF=_tmp492.f1;if(_tmp4BF <= (void*)3  || *((int*)_tmp4BF)!= 14)
goto _LL32E;_tmp4C0=_tmp492.f2;if(_tmp4C0 <= (void*)3  || *((int*)_tmp4C0)!= 5)goto
_LL32E;_tmp4C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4C0)->f2;if((int)
_tmp4C1 != 2)goto _LL32E;_LL32D: goto _LL32F;_LL32E: _tmp4C2=_tmp492.f1;if(_tmp4C2 <= (
void*)3  || *((int*)_tmp4C2)!= 17)goto _LL330;_tmp4C3=_tmp492.f2;if(_tmp4C3 <= (
void*)3  || *((int*)_tmp4C3)!= 5)goto _LL330;_LL32F: return 1;_LL330:;_LL331: return
Cyc_Tcutil_unify(t1,t2);_LL323:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp4FB=Cyc_Tcutil_compress(t);_LL35F: if(_tmp4FB <= (void*)3  || *((int*)_tmp4FB)
!= 4)goto _LL361;_LL360: return 1;_LL361:;_LL362: return 0;_LL35E:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4FC=(void*)e->r;void*_tmp4FD;int _tmp4FE;void*
_tmp4FF;char _tmp500;void*_tmp501;short _tmp502;void*_tmp503;long long _tmp504;void*
_tmp505;struct Cyc_Absyn_Exp*_tmp506;_LL364: if(*((int*)_tmp4FC)!= 0)goto _LL366;
_tmp4FD=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FC)->f1;if(_tmp4FD <= (void*)
1  || *((int*)_tmp4FD)!= 2)goto _LL366;_tmp4FE=((struct Cyc_Absyn_Int_c_struct*)
_tmp4FD)->f2;if(_tmp4FE != 0)goto _LL366;_LL365: goto _LL367;_LL366: if(*((int*)
_tmp4FC)!= 0)goto _LL368;_tmp4FF=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FC)->f1;
if(_tmp4FF <= (void*)1  || *((int*)_tmp4FF)!= 0)goto _LL368;_tmp500=((struct Cyc_Absyn_Char_c_struct*)
_tmp4FF)->f2;if(_tmp500 != 0)goto _LL368;_LL367: goto _LL369;_LL368: if(*((int*)
_tmp4FC)!= 0)goto _LL36A;_tmp501=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FC)->f1;
if(_tmp501 <= (void*)1  || *((int*)_tmp501)!= 1)goto _LL36A;_tmp502=((struct Cyc_Absyn_Short_c_struct*)
_tmp501)->f2;if(_tmp502 != 0)goto _LL36A;_LL369: goto _LL36B;_LL36A: if(*((int*)
_tmp4FC)!= 0)goto _LL36C;_tmp503=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FC)->f1;
if(_tmp503 <= (void*)1  || *((int*)_tmp503)!= 3)goto _LL36C;_tmp504=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp503)->f2;if(_tmp504 != 0)goto _LL36C;_LL36B: return 1;_LL36C: if(*((int*)_tmp4FC)
!= 15)goto _LL36E;_tmp505=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4FC)->f1;
_tmp506=((struct Cyc_Absyn_Cast_e_struct*)_tmp4FC)->f2;_LL36D: return Cyc_Tcutil_is_zero(
_tmp506) && Cyc_Tcutil_admits_zero(_tmp505);_LL36E:;_LL36F: return 0;_LL363:;}
struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp507=
_cycalloc(sizeof(*_tmp507));_tmp507[0]=({struct Cyc_Absyn_Const_e_struct _tmp508;
_tmp508.tag=0;_tmp508.f1=(void*)((void*)0);_tmp508;});_tmp507;})));{struct Cyc_List_List*
_tmp509=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp50A=({struct Cyc_Absyn_PointerType_struct*_tmp50B=_cycalloc(sizeof(*_tmp50B));
_tmp50B[0]=({struct Cyc_Absyn_PointerType_struct _tmp50C;_tmp50C.tag=4;_tmp50C.f1=({
struct Cyc_Absyn_PtrInfo _tmp50D;_tmp50D.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp510=_cycalloc(sizeof(*
_tmp510));_tmp510->v=_tmp509;_tmp510;}));_tmp50D.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp50D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50E;_tmp50E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp50F=_cycalloc(
sizeof(*_tmp50F));_tmp50F->v=_tmp509;_tmp50F;}));_tmp50E.nullable=Cyc_Absyn_true_conref;
_tmp50E.bounds=Cyc_Absyn_empty_conref();_tmp50E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp50E.loc=0;_tmp50E;});_tmp50D;});_tmp50C;});_tmp50B;});(
void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp50A));
return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(
void*c){void*_tmp511=c;_LL371: if((int)_tmp511 != 0)goto _LL373;_LL372: return({
const char*_tmp512="unknown";_tag_arr(_tmp512,sizeof(char),_get_zero_arr_size(
_tmp512,8));});_LL373: if((int)_tmp511 != 1)goto _LL375;_LL374: return({const char*
_tmp513="no coercion";_tag_arr(_tmp513,sizeof(char),_get_zero_arr_size(_tmp513,
12));});_LL375: if((int)_tmp511 != 2)goto _LL377;_LL376: return({const char*_tmp514="null check";
_tag_arr(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,11));});_LL377: if((int)
_tmp511 != 3)goto _LL370;_LL378: return({const char*_tmp515="other coercion";
_tag_arr(_tmp515,sizeof(char),_get_zero_arr_size(_tmp515,15));});_LL370:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp519;_tmp519.tag=0;_tmp519.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp518;_tmp518.tag=0;_tmp518.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp516[2]={&
_tmp518,& _tmp519};Cyc_Tcutil_warn(e->loc,({const char*_tmp517="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp517,sizeof(char),_get_zero_arr_size(_tmp517,53));}),_tag_arr(
_tmp516,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp51D;_tmp51D.tag=0;
_tmp51D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp51C;_tmp51C.tag=0;_tmp51C.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp51A[2]={& _tmp51C,&
_tmp51D};Cyc_Tcutil_warn(e->loc,({const char*_tmp51B="implicit cast from %s to %s";
_tag_arr(_tmp51B,sizeof(char),_get_zero_arr_size(_tmp51B,28));}),_tag_arr(
_tmp51A,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp51E=Cyc_Tcutil_compress(t);
_LL37A: if(_tmp51E <= (void*)3  || *((int*)_tmp51E)!= 5)goto _LL37C;_LL37B: goto
_LL37D;_LL37C: if((int)_tmp51E != 1)goto _LL37E;_LL37D: goto _LL37F;_LL37E: if(_tmp51E
<= (void*)3  || *((int*)_tmp51E)!= 6)goto _LL380;_LL37F: return 1;_LL380:;_LL381:
return 0;_LL379:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp51F=_cycalloc(
sizeof(*_tmp51F));_tmp51F->f1=x->tq;_tmp51F->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp51F;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp520=t1;struct Cyc_List_List*
_tmp521;struct Cyc_Absyn_AggrInfo _tmp522;void*_tmp523;struct Cyc_Absyn_Aggrdecl**
_tmp524;struct Cyc_Absyn_Aggrdecl*_tmp525;struct Cyc_List_List*_tmp526;void*
_tmp527;struct Cyc_List_List*_tmp528;struct Cyc_Absyn_FnInfo _tmp529;_LL383: if((int)
_tmp520 != 0)goto _LL385;_LL384: return 0;_LL385: if(_tmp520 <= (void*)3  || *((int*)
_tmp520)!= 9)goto _LL387;_tmp521=((struct Cyc_Absyn_TupleType_struct*)_tmp520)->f1;
_LL386: return _tmp521;_LL387: if(_tmp520 <= (void*)3  || *((int*)_tmp520)!= 10)goto
_LL389;_tmp522=((struct Cyc_Absyn_AggrType_struct*)_tmp520)->f1;_tmp523=(void*)
_tmp522.aggr_info;if(*((int*)_tmp523)!= 1)goto _LL389;_tmp524=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp523)->f1;_tmp525=*_tmp524;_tmp526=_tmp522.targs;_LL388: if((((void*)_tmp525->kind
== (void*)1  || _tmp525->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp525->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp525->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp52A=_cycalloc(
sizeof(*_tmp52A));_tmp52A->hd=({struct _tuple4*_tmp52B=_cycalloc(sizeof(*_tmp52B));
_tmp52B->f1=Cyc_Absyn_empty_tqual(0);_tmp52B->f2=t1;_tmp52B;});_tmp52A->tl=0;
_tmp52A;});{struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp525->tvs,_tmp526);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp52C,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp525->impl))->fields);}_LL389: if(
_tmp520 <= (void*)3  || *((int*)_tmp520)!= 11)goto _LL38B;_tmp527=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp520)->f1;if((int)_tmp527 != 0)goto _LL38B;
_tmp528=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp520)->f2;_LL38A: return((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,
0,_tmp528);_LL38B: if(_tmp520 <= (void*)3  || *((int*)_tmp520)!= 8)goto _LL38D;
_tmp529=((struct Cyc_Absyn_FnType_struct*)_tmp520)->f1;_LL38C: return({struct Cyc_List_List*
_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->hd=({struct _tuple4*_tmp52E=
_cycalloc(sizeof(*_tmp52E));_tmp52E->f1=Cyc_Absyn_const_tqual(0);_tmp52E->f2=t1;
_tmp52E;});_tmp52D->tl=0;_tmp52D;});_LL38D:;_LL38E: return({struct Cyc_List_List*
_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->hd=({struct _tuple4*_tmp530=
_cycalloc(sizeof(*_tmp530));_tmp530->f1=Cyc_Absyn_empty_tqual(0);_tmp530->f2=t1;
_tmp530;});_tmp52F->tl=0;_tmp52F;});_LL382:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp531=(void*)t->hd;_LL390: if((int)_tmp531 != 16)goto _LL392;
_LL391: goto _LL393;_LL392: if((int)_tmp531 != 3)goto _LL394;_LL393: goto _LL395;_LL394:
if(_tmp531 <= (void*)17  || *((int*)_tmp531)!= 4)goto _LL396;_LL395: continue;_LL396:;
_LL397: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL38F:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp533=({struct _tuple6 _tmp532;_tmp532.f1=t1;_tmp532.f2=t2;_tmp532;});void*
_tmp534;struct Cyc_Absyn_PtrInfo _tmp535;void*_tmp536;struct Cyc_Absyn_Tqual _tmp537;
struct Cyc_Absyn_PtrAtts _tmp538;void*_tmp539;struct Cyc_Absyn_Conref*_tmp53A;
struct Cyc_Absyn_Conref*_tmp53B;struct Cyc_Absyn_Conref*_tmp53C;void*_tmp53D;
struct Cyc_Absyn_PtrInfo _tmp53E;void*_tmp53F;struct Cyc_Absyn_Tqual _tmp540;struct
Cyc_Absyn_PtrAtts _tmp541;void*_tmp542;struct Cyc_Absyn_Conref*_tmp543;struct Cyc_Absyn_Conref*
_tmp544;struct Cyc_Absyn_Conref*_tmp545;void*_tmp546;struct Cyc_Absyn_FnInfo
_tmp547;void*_tmp548;struct Cyc_Absyn_FnInfo _tmp549;_LL399: _tmp534=_tmp533.f1;if(
_tmp534 <= (void*)3  || *((int*)_tmp534)!= 4)goto _LL39B;_tmp535=((struct Cyc_Absyn_PointerType_struct*)
_tmp534)->f1;_tmp536=(void*)_tmp535.elt_typ;_tmp537=_tmp535.elt_tq;_tmp538=
_tmp535.ptr_atts;_tmp539=(void*)_tmp538.rgn;_tmp53A=_tmp538.nullable;_tmp53B=
_tmp538.bounds;_tmp53C=_tmp538.zero_term;_tmp53D=_tmp533.f2;if(_tmp53D <= (void*)
3  || *((int*)_tmp53D)!= 4)goto _LL39B;_tmp53E=((struct Cyc_Absyn_PointerType_struct*)
_tmp53D)->f1;_tmp53F=(void*)_tmp53E.elt_typ;_tmp540=_tmp53E.elt_tq;_tmp541=
_tmp53E.ptr_atts;_tmp542=(void*)_tmp541.rgn;_tmp543=_tmp541.nullable;_tmp544=
_tmp541.bounds;_tmp545=_tmp541.zero_term;_LL39A: if(_tmp537.real_const  && !
_tmp540.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp53A,
_tmp543) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp53A)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp543))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp53C,
_tmp545) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp53C)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp545))return 0;if(!Cyc_Tcutil_unify(_tmp539,_tmp542) && !Cyc_Tcenv_region_outlives(
te,_tmp539,_tmp542))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp53B,_tmp544)){struct _tuple6 _tmp54B=({struct _tuple6 _tmp54A;_tmp54A.f1=Cyc_Absyn_conref_val(
_tmp53B);_tmp54A.f2=Cyc_Absyn_conref_val(_tmp544);_tmp54A;});void*_tmp54C;void*
_tmp54D;void*_tmp54E;struct Cyc_Absyn_Exp*_tmp54F;void*_tmp550;struct Cyc_Absyn_Exp*
_tmp551;_LL3A0: _tmp54C=_tmp54B.f1;if(_tmp54C <= (void*)1  || *((int*)_tmp54C)!= 0)
goto _LL3A2;_tmp54D=_tmp54B.f2;if((int)_tmp54D != 0)goto _LL3A2;_LL3A1: goto _LL39F;
_LL3A2: _tmp54E=_tmp54B.f1;if(_tmp54E <= (void*)1  || *((int*)_tmp54E)!= 0)goto
_LL3A4;_tmp54F=((struct Cyc_Absyn_Upper_b_struct*)_tmp54E)->f1;_tmp550=_tmp54B.f2;
if(_tmp550 <= (void*)1  || *((int*)_tmp550)!= 0)goto _LL3A4;_tmp551=((struct Cyc_Absyn_Upper_b_struct*)
_tmp550)->f1;_LL3A3: if(!Cyc_Evexp_lte_const_exp(_tmp551,_tmp54F))return 0;goto
_LL39F;_LL3A4:;_LL3A5: return 0;_LL39F:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->hd=({struct _tuple6*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->f1=t1;_tmp553->f2=t2;_tmp553;});
_tmp552->tl=assume;_tmp552;}),_tmp536,_tmp53F);_LL39B: _tmp546=_tmp533.f1;if(
_tmp546 <= (void*)3  || *((int*)_tmp546)!= 8)goto _LL39D;_tmp547=((struct Cyc_Absyn_FnType_struct*)
_tmp546)->f1;_tmp548=_tmp533.f2;if(_tmp548 <= (void*)3  || *((int*)_tmp548)!= 8)
goto _LL39D;_tmp549=((struct Cyc_Absyn_FnType_struct*)_tmp548)->f1;_LL39C: if(
_tmp547.tvars != 0  || _tmp549.tvars != 0){struct Cyc_List_List*_tmp554=_tmp547.tvars;
struct Cyc_List_List*_tmp555=_tmp549.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp554)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp555))return 0;{
struct _RegionHandle _tmp556=_new_region("r");struct _RegionHandle*r=& _tmp556;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp554 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp554->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp555))->hd)){int _tmp557=0;_npop_handler(0);
return _tmp557;}inst=({struct Cyc_List_List*_tmp558=_region_malloc(r,sizeof(*
_tmp558));_tmp558->hd=({struct _tuple8*_tmp559=_region_malloc(r,sizeof(*_tmp559));
_tmp559->f1=(struct Cyc_Absyn_Tvar*)_tmp555->hd;_tmp559->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A[0]=({struct Cyc_Absyn_VarType_struct
_tmp55B;_tmp55B.tag=1;_tmp55B.f1=(struct Cyc_Absyn_Tvar*)_tmp554->hd;_tmp55B;});
_tmp55A;});_tmp559;});_tmp558->tl=inst;_tmp558;});_tmp554=_tmp554->tl;_tmp555=
_tmp555->tl;}if(inst != 0){_tmp547.tvars=0;_tmp549.tvars=0;{int _tmp560=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp55C=_cycalloc(sizeof(*
_tmp55C));_tmp55C[0]=({struct Cyc_Absyn_FnType_struct _tmp55D;_tmp55D.tag=8;
_tmp55D.f1=_tmp547;_tmp55D;});_tmp55C;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Absyn_FnType_struct
_tmp55F;_tmp55F.tag=8;_tmp55F.f1=_tmp549;_tmp55F;});_tmp55E;}));_npop_handler(0);
return _tmp560;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp547.ret_typ,(void*)_tmp549.ret_typ))return 0;{struct Cyc_List_List*_tmp561=
_tmp547.args;struct Cyc_List_List*_tmp562=_tmp549.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp561)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp562))return 0;for(0;_tmp561 != 0;(_tmp561=_tmp561->tl,_tmp562=_tmp562->tl)){
struct Cyc_Absyn_Tqual _tmp564;void*_tmp565;struct _tuple2 _tmp563=*((struct _tuple2*)
_tmp561->hd);_tmp564=_tmp563.f2;_tmp565=_tmp563.f3;{struct Cyc_Absyn_Tqual _tmp567;
void*_tmp568;struct _tuple2 _tmp566=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp562))->hd);_tmp567=_tmp566.f2;_tmp568=_tmp566.f3;if(_tmp567.real_const
 && !_tmp564.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp568,_tmp565))return
0;}}if(_tmp547.c_varargs != _tmp549.c_varargs)return 0;if(_tmp547.cyc_varargs != 0
 && _tmp549.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp569=*_tmp547.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp56A=*_tmp549.cyc_varargs;if((_tmp56A.tq).real_const
 && !(_tmp569.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp56A.type,(
void*)_tmp569.type))return 0;}else{if(_tmp547.cyc_varargs != 0  || _tmp549.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp547.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp549.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp547.rgn_po,_tmp549.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp547.attributes,_tmp549.attributes))return 0;return 1;}_LL39D:;_LL39E: return 0;
_LL398:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp56C=({
struct _tuple6 _tmp56B;_tmp56B.f1=Cyc_Tcutil_compress(t1);_tmp56B.f2=Cyc_Tcutil_compress(
t2);_tmp56B;});void*_tmp56D;void*_tmp56E;void*_tmp56F;void*_tmp570;_LL3A7:
_tmp56D=_tmp56C.f1;if(_tmp56D <= (void*)3  || *((int*)_tmp56D)!= 5)goto _LL3A9;
_tmp56E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56D)->f2;_tmp56F=_tmp56C.f2;
if(_tmp56F <= (void*)3  || *((int*)_tmp56F)!= 5)goto _LL3A9;_tmp570=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp56F)->f2;_LL3A8: return _tmp56E == _tmp570;_LL3A9:;
_LL3AA: return 0;_LL3A6:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp572;struct
Cyc_Absyn_Tqual _tmp573;void*_tmp574;struct _tuple4*_tmp571=(struct _tuple4*)tqs1->hd;
_tmp572=*_tmp571;_tmp573=_tmp572.f1;_tmp574=_tmp572.f2;{struct _tuple4 _tmp576;
struct Cyc_Absyn_Tqual _tmp577;void*_tmp578;struct _tuple4*_tmp575=(struct _tuple4*)
tqs2->hd;_tmp576=*_tmp575;_tmp577=_tmp576.f1;_tmp578=_tmp576.f2;if(_tmp577.real_const
 && Cyc_Tcutil_subtype(te,assume,_tmp574,_tmp578))continue;else{if(Cyc_Tcutil_unify(
_tmp574,_tmp578))continue;else{if(Cyc_Tcutil_isomorphic(_tmp574,_tmp578))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp579=Cyc_Tcutil_compress(t);void*_tmp57A;_LL3AC: if(_tmp579 <= (void*)3
 || *((int*)_tmp579)!= 5)goto _LL3AE;_tmp57A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp579)->f2;if((int)_tmp57A != 0)goto _LL3AE;_LL3AD: return 1;_LL3AE:;_LL3AF: return
0;_LL3AB:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp57B=t2;
void*_tmp57C;_LL3B1: if(_tmp57B <= (void*)3  || *((int*)_tmp57B)!= 5)goto _LL3B3;
_tmp57C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp57B)->f2;if((int)_tmp57C != 
2)goto _LL3B3;_LL3B2: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3B0;_LL3B3:;_LL3B4: goto _LL3B0;_LL3B0:;}{void*_tmp57D=t1;struct Cyc_Absyn_PtrInfo
_tmp57E;void*_tmp57F;struct Cyc_Absyn_Tqual _tmp580;struct Cyc_Absyn_PtrAtts _tmp581;
void*_tmp582;struct Cyc_Absyn_Conref*_tmp583;struct Cyc_Absyn_Conref*_tmp584;
struct Cyc_Absyn_Conref*_tmp585;struct Cyc_Absyn_ArrayInfo _tmp586;void*_tmp587;
struct Cyc_Absyn_Tqual _tmp588;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Conref*
_tmp58A;struct Cyc_Absyn_Enumdecl*_tmp58B;_LL3B6: if(_tmp57D <= (void*)3  || *((int*)
_tmp57D)!= 4)goto _LL3B8;_tmp57E=((struct Cyc_Absyn_PointerType_struct*)_tmp57D)->f1;
_tmp57F=(void*)_tmp57E.elt_typ;_tmp580=_tmp57E.elt_tq;_tmp581=_tmp57E.ptr_atts;
_tmp582=(void*)_tmp581.rgn;_tmp583=_tmp581.nullable;_tmp584=_tmp581.bounds;
_tmp585=_tmp581.zero_term;_LL3B7:{void*_tmp58C=t2;struct Cyc_Absyn_PtrInfo _tmp58D;
void*_tmp58E;struct Cyc_Absyn_Tqual _tmp58F;struct Cyc_Absyn_PtrAtts _tmp590;void*
_tmp591;struct Cyc_Absyn_Conref*_tmp592;struct Cyc_Absyn_Conref*_tmp593;struct Cyc_Absyn_Conref*
_tmp594;_LL3C5: if(_tmp58C <= (void*)3  || *((int*)_tmp58C)!= 4)goto _LL3C7;_tmp58D=((
struct Cyc_Absyn_PointerType_struct*)_tmp58C)->f1;_tmp58E=(void*)_tmp58D.elt_typ;
_tmp58F=_tmp58D.elt_tq;_tmp590=_tmp58D.ptr_atts;_tmp591=(void*)_tmp590.rgn;
_tmp592=_tmp590.nullable;_tmp593=_tmp590.bounds;_tmp594=_tmp590.zero_term;_LL3C6: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp595=({struct Cyc_List_List*_tmp5A4=
_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=({struct _tuple6*_tmp5A5=_cycalloc(
sizeof(*_tmp5A5));_tmp5A5->f1=t1;_tmp5A5->f2=t2;_tmp5A5;});_tmp5A4->tl=0;_tmp5A4;});
int _tmp596=Cyc_Tcutil_ptrsubtype(te,_tmp595,_tmp57F,_tmp58E) && (!_tmp580.real_const
 || _tmp58F.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp585,_tmp594) || !((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp594);int _tmp597=_tmp596?0:((
Cyc_Tcutil_bits_only(_tmp57F) && Cyc_Tcutil_is_char_type(_tmp58E)) && !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp594)) && (_tmp58F.real_const
 || !_tmp580.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp584,_tmp593);if(!bounds_ok  && !_tmp597){struct _tuple6 _tmp599=({struct _tuple6
_tmp598;_tmp598.f1=Cyc_Absyn_conref_val(_tmp584);_tmp598.f2=Cyc_Absyn_conref_val(
_tmp593);_tmp598;});void*_tmp59A;struct Cyc_Absyn_Exp*_tmp59B;void*_tmp59C;struct
Cyc_Absyn_Exp*_tmp59D;void*_tmp59E;void*_tmp59F;void*_tmp5A0;void*_tmp5A1;void*
_tmp5A2;void*_tmp5A3;_LL3CA: _tmp59A=_tmp599.f1;if(_tmp59A <= (void*)1  || *((int*)
_tmp59A)!= 0)goto _LL3CC;_tmp59B=((struct Cyc_Absyn_Upper_b_struct*)_tmp59A)->f1;
_tmp59C=_tmp599.f2;if(_tmp59C <= (void*)1  || *((int*)_tmp59C)!= 0)goto _LL3CC;
_tmp59D=((struct Cyc_Absyn_Upper_b_struct*)_tmp59C)->f1;_LL3CB: if(Cyc_Evexp_lte_const_exp(
_tmp59D,_tmp59B))bounds_ok=1;goto _LL3C9;_LL3CC: _tmp59E=_tmp599.f1;if(_tmp59E <= (
void*)1  || *((int*)_tmp59E)!= 1)goto _LL3CE;_tmp59F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp59E)->f1;_tmp5A0=_tmp599.f2;if(_tmp5A0 <= (void*)1  || *((int*)_tmp5A0)!= 1)
goto _LL3CE;_tmp5A1=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp5A0)->f1;
_LL3CD: bounds_ok=Cyc_Tcutil_unify(_tmp59F,_tmp5A1);goto _LL3C9;_LL3CE: _tmp5A2=
_tmp599.f1;if(_tmp5A2 <= (void*)1  || *((int*)_tmp5A2)!= 1)goto _LL3D0;_LL3CF: goto
_LL3D1;_LL3D0: _tmp5A3=_tmp599.f2;if(_tmp5A3 <= (void*)1  || *((int*)_tmp5A3)!= 1)
goto _LL3D2;_LL3D1: bounds_ok=0;goto _LL3C9;_LL3D2:;_LL3D3: bounds_ok=1;goto _LL3C9;
_LL3C9:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp583) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp592))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok) && (_tmp596  || _tmp597))
 && (Cyc_Tcutil_unify(_tmp582,_tmp591) || Cyc_Tcenv_region_outlives(te,_tmp582,
_tmp591)))return coercion;else{return(void*)0;}}}_LL3C7:;_LL3C8: goto _LL3C4;_LL3C4:;}
return(void*)0;_LL3B8: if(_tmp57D <= (void*)3  || *((int*)_tmp57D)!= 7)goto _LL3BA;
_tmp586=((struct Cyc_Absyn_ArrayType_struct*)_tmp57D)->f1;_tmp587=(void*)_tmp586.elt_type;
_tmp588=_tmp586.tq;_tmp589=_tmp586.num_elts;_tmp58A=_tmp586.zero_term;_LL3B9:{
void*_tmp5A6=t2;struct Cyc_Absyn_ArrayInfo _tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Tqual
_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Conref*_tmp5AB;_LL3D5: if(
_tmp5A6 <= (void*)3  || *((int*)_tmp5A6)!= 7)goto _LL3D7;_tmp5A7=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A6)->f1;_tmp5A8=(void*)_tmp5A7.elt_type;_tmp5A9=_tmp5A7.tq;_tmp5AA=_tmp5A7.num_elts;
_tmp5AB=_tmp5A7.zero_term;_LL3D6: {int okay;okay=((_tmp589 != 0  && _tmp5AA != 0)
 && ((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp58A,_tmp5AB)) && Cyc_Evexp_lte_const_exp((
struct Cyc_Absyn_Exp*)_tmp5AA,(struct Cyc_Absyn_Exp*)_tmp589);return(okay  && Cyc_Tcutil_unify(
_tmp587,_tmp5A8)) && (!_tmp588.real_const  || _tmp5A9.real_const)?(void*)3:(void*)
0;}_LL3D7:;_LL3D8: return(void*)0;_LL3D4:;}return(void*)0;_LL3BA: if(_tmp57D <= (
void*)3  || *((int*)_tmp57D)!= 12)goto _LL3BC;_tmp58B=((struct Cyc_Absyn_EnumType_struct*)
_tmp57D)->f2;_LL3BB:{void*_tmp5AC=t2;struct Cyc_Absyn_Enumdecl*_tmp5AD;_LL3DA: if(
_tmp5AC <= (void*)3  || *((int*)_tmp5AC)!= 12)goto _LL3DC;_tmp5AD=((struct Cyc_Absyn_EnumType_struct*)
_tmp5AC)->f2;_LL3DB: if((_tmp58B->fields != 0  && _tmp5AD->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp58B->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AD->fields))->v))
return(void*)1;goto _LL3D9;_LL3DC:;_LL3DD: goto _LL3D9;_LL3D9:;}goto _LL3BD;_LL3BC:
if(_tmp57D <= (void*)3  || *((int*)_tmp57D)!= 5)goto _LL3BE;_LL3BD: goto _LL3BF;
_LL3BE: if((int)_tmp57D != 1)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(_tmp57D <= (
void*)3  || *((int*)_tmp57D)!= 6)goto _LL3C2;_LL3C1: return Cyc_Tcutil_coerceable(t2)?(
void*)1:(void*)0;_LL3C2:;_LL3C3: return(void*)0;_LL3B5:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp5AE=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5B0;_tmp5B0.tag=15;_tmp5B0.f1=(void*)t;_tmp5B0.f2=_tmp5AE;_tmp5B0.f3=0;
_tmp5B0.f4=(void*)c;_tmp5B0;});_tmp5AF;})));e->topt=({struct Cyc_Core_Opt*_tmp5B1=
_cycalloc(sizeof(*_tmp5B1));_tmp5B1->v=(void*)t;_tmp5B1;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp5B2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3DF: if(_tmp5B2 <= (void*)3  || *((int*)_tmp5B2)!= 5)
goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if(_tmp5B2 <= (void*)3  || *((int*)_tmp5B2)!= 
12)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp5B2 <= (void*)3  || *((int*)_tmp5B2)
!= 13)goto _LL3E5;_LL3E4: goto _LL3E6;_LL3E5: if(_tmp5B2 <= (void*)3  || *((int*)
_tmp5B2)!= 17)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: if(_tmp5B2 <= (void*)3  || *((
int*)_tmp5B2)!= 14)goto _LL3E9;_LL3E8: return 1;_LL3E9: if(_tmp5B2 <= (void*)3  || *((
int*)_tmp5B2)!= 0)goto _LL3EB;_LL3EA: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3EB:;_LL3EC: return 0;_LL3DE:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp5B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3EE: if((int)_tmp5B3 != 1)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(_tmp5B3 <= (
void*)3  || *((int*)_tmp5B3)!= 6)goto _LL3F2;_LL3F1: return 1;_LL3F2:;_LL3F3: return 0;
_LL3ED:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp5B4=Cyc_Tcutil_compress(
t);_LL3F5: if(_tmp5B4 <= (void*)3  || *((int*)_tmp5B4)!= 8)goto _LL3F7;_LL3F6: return
1;_LL3F7:;_LL3F8: return 0;_LL3F4:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,
void*t2){struct _tuple6 _tmp5B6=({struct _tuple6 _tmp5B5;_tmp5B5.f1=t1;_tmp5B5.f2=t2;
_tmp5B5;});void*_tmp5B7;int _tmp5B8;void*_tmp5B9;int _tmp5BA;void*_tmp5BB;void*
_tmp5BC;void*_tmp5BD;void*_tmp5BE;void*_tmp5BF;void*_tmp5C0;void*_tmp5C1;void*
_tmp5C2;void*_tmp5C3;void*_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*_tmp5C7;void*
_tmp5C8;void*_tmp5C9;void*_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*_tmp5CD;void*
_tmp5CE;void*_tmp5CF;void*_tmp5D0;void*_tmp5D1;void*_tmp5D2;void*_tmp5D3;void*
_tmp5D4;void*_tmp5D5;void*_tmp5D6;_LL3FA: _tmp5B7=_tmp5B6.f1;if(_tmp5B7 <= (void*)
3  || *((int*)_tmp5B7)!= 6)goto _LL3FC;_tmp5B8=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5B7)->f1;_tmp5B9=_tmp5B6.f2;if(_tmp5B9 <= (void*)3  || *((int*)_tmp5B9)!= 6)
goto _LL3FC;_tmp5BA=((struct Cyc_Absyn_DoubleType_struct*)_tmp5B9)->f1;_LL3FB: if(
_tmp5B8)return t1;else{return t2;}_LL3FC: _tmp5BB=_tmp5B6.f1;if(_tmp5BB <= (void*)3
 || *((int*)_tmp5BB)!= 6)goto _LL3FE;_LL3FD: return t1;_LL3FE: _tmp5BC=_tmp5B6.f2;
if(_tmp5BC <= (void*)3  || *((int*)_tmp5BC)!= 6)goto _LL400;_LL3FF: return t2;_LL400:
_tmp5BD=_tmp5B6.f1;if((int)_tmp5BD != 1)goto _LL402;_LL401: goto _LL403;_LL402:
_tmp5BE=_tmp5B6.f2;if((int)_tmp5BE != 1)goto _LL404;_LL403: return(void*)1;_LL404:
_tmp5BF=_tmp5B6.f1;if(_tmp5BF <= (void*)3  || *((int*)_tmp5BF)!= 5)goto _LL406;
_tmp5C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BF)->f1;if((int)_tmp5C0 != 
1)goto _LL406;_tmp5C1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BF)->f2;if((
int)_tmp5C1 != 3)goto _LL406;_LL405: goto _LL407;_LL406: _tmp5C2=_tmp5B6.f2;if(
_tmp5C2 <= (void*)3  || *((int*)_tmp5C2)!= 5)goto _LL408;_tmp5C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C2)->f1;if((int)_tmp5C3 != 1)goto _LL408;_tmp5C4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C2)->f2;if((int)_tmp5C4 != 3)goto _LL408;_LL407: return Cyc_Absyn_ulonglong_typ;
_LL408: _tmp5C5=_tmp5B6.f1;if(_tmp5C5 <= (void*)3  || *((int*)_tmp5C5)!= 5)goto
_LL40A;_tmp5C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C5)->f1;if((int)
_tmp5C6 != 2)goto _LL40A;_tmp5C7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C5)->f2;
if((int)_tmp5C7 != 3)goto _LL40A;_LL409: goto _LL40B;_LL40A: _tmp5C8=_tmp5B6.f2;if(
_tmp5C8 <= (void*)3  || *((int*)_tmp5C8)!= 5)goto _LL40C;_tmp5C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C8)->f1;if((int)_tmp5C9 != 2)goto _LL40C;_tmp5CA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C8)->f2;if((int)_tmp5CA != 3)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp5CB=
_tmp5B6.f1;if(_tmp5CB <= (void*)3  || *((int*)_tmp5CB)!= 5)goto _LL40E;_tmp5CC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CB)->f1;if((int)_tmp5CC != 0)goto
_LL40E;_tmp5CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CB)->f2;if((int)
_tmp5CD != 3)goto _LL40E;_LL40D: goto _LL40F;_LL40E: _tmp5CE=_tmp5B6.f2;if(_tmp5CE <= (
void*)3  || *((int*)_tmp5CE)!= 5)goto _LL410;_tmp5CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CE)->f1;if((int)_tmp5CF != 0)goto _LL410;_tmp5D0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CE)->f2;if((int)_tmp5D0 != 3)goto _LL410;_LL40F: return Cyc_Absyn_slonglong_typ;
_LL410: _tmp5D1=_tmp5B6.f1;if(_tmp5D1 <= (void*)3  || *((int*)_tmp5D1)!= 5)goto
_LL412;_tmp5D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D1)->f1;if((int)
_tmp5D2 != 1)goto _LL412;_tmp5D3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D1)->f2;
if((int)_tmp5D3 != 2)goto _LL412;_LL411: goto _LL413;_LL412: _tmp5D4=_tmp5B6.f2;if(
_tmp5D4 <= (void*)3  || *((int*)_tmp5D4)!= 5)goto _LL414;_tmp5D5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D4)->f1;if((int)_tmp5D5 != 1)goto _LL414;_tmp5D6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D4)->f2;if((int)_tmp5D6 != 2)goto _LL414;_LL413: return Cyc_Absyn_uint_typ;
_LL414:;_LL415: return Cyc_Absyn_sint_typ;_LL3F9:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5D7=(void*)e->r;struct Cyc_Core_Opt*_tmp5D8;
_LL417: if(*((int*)_tmp5D7)!= 4)goto _LL419;_tmp5D8=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D7)->f2;if(_tmp5D8 != 0)goto _LL419;_LL418:({void*_tmp5D9[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5DA="assignment in test";_tag_arr(_tmp5DA,sizeof(char),
_get_zero_arr_size(_tmp5DA,19));}),_tag_arr(_tmp5D9,sizeof(void*),0));});goto
_LL416;_LL419:;_LL41A: goto _LL416;_LL416:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5DC=({struct _tuple6 _tmp5DB;_tmp5DB.f1=c1;_tmp5DB.f2=c2;_tmp5DB;});
void*_tmp5DD;void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;void*_tmp5E1;struct Cyc_Core_Opt*
_tmp5E2;struct Cyc_Core_Opt**_tmp5E3;void*_tmp5E4;struct Cyc_Core_Opt*_tmp5E5;
struct Cyc_Core_Opt**_tmp5E6;void*_tmp5E7;struct Cyc_Core_Opt*_tmp5E8;struct Cyc_Core_Opt**
_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*_tmp5EE;void*
_tmp5EF;struct Cyc_Core_Opt*_tmp5F0;struct Cyc_Core_Opt**_tmp5F1;void*_tmp5F2;void*
_tmp5F3;struct Cyc_Core_Opt*_tmp5F4;struct Cyc_Core_Opt**_tmp5F5;void*_tmp5F6;void*
_tmp5F7;struct Cyc_Core_Opt*_tmp5F8;struct Cyc_Core_Opt**_tmp5F9;void*_tmp5FA;
_LL41C: _tmp5DD=_tmp5DC.f1;if(*((int*)_tmp5DD)!= 0)goto _LL41E;_tmp5DE=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5DD)->f1;_tmp5DF=_tmp5DC.f2;if(*((int*)_tmp5DF)
!= 0)goto _LL41E;_tmp5E0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5DF)->f1;
_LL41D: return _tmp5DE == _tmp5E0;_LL41E: _tmp5E1=_tmp5DC.f2;if(*((int*)_tmp5E1)!= 1)
goto _LL420;_tmp5E2=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E1)->f1;_tmp5E3=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E1)->f1;_LL41F:*
_tmp5E3=({struct Cyc_Core_Opt*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->v=(
void*)c1;_tmp5FB;});return 1;_LL420: _tmp5E4=_tmp5DC.f1;if(*((int*)_tmp5E4)!= 1)
goto _LL422;_tmp5E5=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E4)->f1;_tmp5E6=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E4)->f1;_LL421:*
_tmp5E6=({struct Cyc_Core_Opt*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->v=(
void*)c2;_tmp5FC;});return 1;_LL422: _tmp5E7=_tmp5DC.f1;if(*((int*)_tmp5E7)!= 2)
goto _LL424;_tmp5E8=((struct Cyc_Absyn_Less_kb_struct*)_tmp5E7)->f1;_tmp5E9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5E7)->f1;_tmp5EA=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E7)->f2;_tmp5EB=_tmp5DC.f2;if(*((
int*)_tmp5EB)!= 0)goto _LL424;_tmp5EC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5EB)->f1;_LL423: if(Cyc_Tcutil_kind_leq(_tmp5EC,_tmp5EA)){*_tmp5E9=({struct
Cyc_Core_Opt*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->v=(void*)c2;_tmp5FD;});
return 1;}else{return 0;}_LL424: _tmp5ED=_tmp5DC.f1;if(*((int*)_tmp5ED)!= 0)goto
_LL426;_tmp5EE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5ED)->f1;_tmp5EF=
_tmp5DC.f2;if(*((int*)_tmp5EF)!= 2)goto _LL426;_tmp5F0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EF)->f1;_tmp5F1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EF)->f1;_tmp5F2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5EF)->f2;
_LL425: if(Cyc_Tcutil_kind_leq(_tmp5EE,_tmp5F2)){*_tmp5F1=({struct Cyc_Core_Opt*
_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->v=(void*)c1;_tmp5FE;});return 1;}
else{return 0;}_LL426: _tmp5F3=_tmp5DC.f1;if(*((int*)_tmp5F3)!= 2)goto _LL41B;
_tmp5F4=((struct Cyc_Absyn_Less_kb_struct*)_tmp5F3)->f1;_tmp5F5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F3)->f1;_tmp5F6=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F3)->f2;_tmp5F7=_tmp5DC.f2;if(*((int*)_tmp5F7)!= 2)goto _LL41B;_tmp5F8=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F7)->f1;_tmp5F9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F7)->f1;_tmp5FA=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F7)->f2;_LL427: if(Cyc_Tcutil_kind_leq(_tmp5F6,_tmp5FA)){*_tmp5F9=({struct
Cyc_Core_Opt*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->v=(void*)c1;_tmp5FF;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5FA,_tmp5F6)){*_tmp5F5=({struct Cyc_Core_Opt*
_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->v=(void*)c2;_tmp600;});return 1;}
else{return 0;}}_LL41B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp601=_cycalloc_atomic(sizeof(*_tmp601));_tmp601[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp601;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp606;_tmp606.tag=1;_tmp606.f1=(unsigned int)i;{void*
_tmp604[1]={& _tmp606};Cyc_aprintf(({const char*_tmp605="#%d";_tag_arr(_tmp605,
sizeof(char),_get_zero_arr_size(_tmp605,4));}),_tag_arr(_tmp604,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->name=({
struct _tagged_arr*_tmp603=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp603[0]=s;
_tmp603;});_tmp602->identity=0;_tmp602->kind=(void*)k;_tmp602;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp607=*t->name;return*((const char*)
_check_unknown_subscript(_tmp607,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp60A;_tmp60A.tag=0;
_tmp60A.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp608[1]={&
_tmp60A};Cyc_printf(({const char*_tmp609="%s";_tag_arr(_tmp609,sizeof(char),
_get_zero_arr_size(_tmp609,3));}),_tag_arr(_tmp608,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp60B=Cyc_strconcat(({const char*_tmp610="`";
_tag_arr(_tmp610,sizeof(char),_get_zero_arr_size(_tmp610,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp60C=_tagged_arr_plus(_tmp60B,
sizeof(char),1);char _tmp60D=*((char*)_check_unknown_subscript(_tmp60C,sizeof(
char),0));char _tmp60E='t';if(_get_arr_size(_tmp60C,sizeof(char))== 1  && (_tmp60D
== '\000'  && _tmp60E != '\000'))_throw_arraybounds();*((char*)_tmp60C.curr)=
_tmp60E;});t->name=({struct _tagged_arr*_tmp60F=_cycalloc(sizeof(struct
_tagged_arr)* 1);_tmp60F[0]=(struct _tagged_arr)_tmp60B;_tmp60F;});}}struct
_tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct
_tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple13*x){return({struct _tuple2*_tmp611=
_cycalloc(sizeof(*_tmp611));_tmp611->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*
_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->v=(*x).f1;_tmp612;});_tmp611->f2=(*
x).f2;_tmp611->f3=(*x).f3;_tmp611;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp613=0;{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)
atts->hd))_tmp613=({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));
_tmp614->hd=(void*)((void*)atts->hd);_tmp614->tl=_tmp613;_tmp614;});}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp615=_cycalloc(sizeof(*_tmp615));
_tmp615[0]=({struct Cyc_Absyn_FnType_struct _tmp616;_tmp616.tag=8;_tmp616.f1=({
struct Cyc_Absyn_FnInfo _tmp617;_tmp617.tvars=fd->tvs;_tmp617.effect=fd->effect;
_tmp617.ret_typ=(void*)((void*)fd->ret_type);_tmp617.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp617.c_varargs=fd->c_varargs;_tmp617.cyc_varargs=fd->cyc_varargs;
_tmp617.rgn_po=fd->rgn_po;_tmp617.attributes=_tmp613;_tmp617;});_tmp616;});
_tmp615;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->f1=(*pr).f1;_tmp618->f2=
t;_tmp618;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp619=
_region_malloc(rgn,sizeof(*_tmp619));_tmp619->f1=({struct _tuple15*_tmp61A=
_region_malloc(rgn,sizeof(*_tmp61A));_tmp61A->f1=(*y).f1;_tmp61A->f2=(*y).f2;
_tmp61A;});_tmp619->f2=(*y).f3;_tmp619;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){struct _tuple15*_tmp61C;void*_tmp61D;struct _tuple16 _tmp61B=*w;
_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;{struct Cyc_Core_Opt*_tmp61F;struct Cyc_Absyn_Tqual
_tmp620;struct _tuple15 _tmp61E=*_tmp61C;_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;
return({struct _tuple2*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->f1=_tmp61F;
_tmp621->f2=_tmp620;_tmp621->f3=_tmp61D;_tmp621;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->name=f->name;_tmp622->tq=f->tq;
_tmp622->type=(void*)t;_tmp622->width=f->width;_tmp622->attributes=f->attributes;
_tmp622;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp623=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp624;struct Cyc_Absyn_AggrInfo _tmp625;void*_tmp626;
struct Cyc_List_List*_tmp627;struct Cyc_Absyn_TunionInfo _tmp628;void*_tmp629;
struct Cyc_List_List*_tmp62A;void*_tmp62B;struct Cyc_Absyn_TunionFieldInfo _tmp62C;
void*_tmp62D;struct Cyc_List_List*_tmp62E;struct _tuple1*_tmp62F;struct Cyc_List_List*
_tmp630;struct Cyc_Absyn_Typedefdecl*_tmp631;void**_tmp632;struct Cyc_Absyn_ArrayInfo
_tmp633;void*_tmp634;struct Cyc_Absyn_Tqual _tmp635;struct Cyc_Absyn_Exp*_tmp636;
struct Cyc_Absyn_Conref*_tmp637;struct Cyc_Absyn_PtrInfo _tmp638;void*_tmp639;
struct Cyc_Absyn_Tqual _tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;void*_tmp63C;struct
Cyc_Absyn_Conref*_tmp63D;struct Cyc_Absyn_Conref*_tmp63E;struct Cyc_Absyn_Conref*
_tmp63F;struct Cyc_Absyn_FnInfo _tmp640;struct Cyc_List_List*_tmp641;struct Cyc_Core_Opt*
_tmp642;void*_tmp643;struct Cyc_List_List*_tmp644;int _tmp645;struct Cyc_Absyn_VarargInfo*
_tmp646;struct Cyc_List_List*_tmp647;struct Cyc_List_List*_tmp648;struct Cyc_List_List*
_tmp649;void*_tmp64A;struct Cyc_List_List*_tmp64B;struct Cyc_Core_Opt*_tmp64C;void*
_tmp64D;void*_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;struct Cyc_List_List*
_tmp652;_LL429: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 1)goto _LL42B;_tmp624=((
struct Cyc_Absyn_VarType_struct*)_tmp623)->f1;_LL42A: {struct _handler_cons _tmp653;
_push_handler(& _tmp653);{int _tmp655=0;if(setjmp(_tmp653.handler))_tmp655=1;if(!
_tmp655){{void*_tmp656=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp624);_npop_handler(0);return _tmp656;};_pop_handler();}else{void*_tmp654=(
void*)_exn_thrown;void*_tmp658=_tmp654;_LL45C: if(_tmp658 != Cyc_Core_Not_found)
goto _LL45E;_LL45D: return t;_LL45E:;_LL45F:(void)_throw(_tmp658);_LL45B:;}}}_LL42B:
if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 10)goto _LL42D;_tmp625=((struct Cyc_Absyn_AggrType_struct*)
_tmp623)->f1;_tmp626=(void*)_tmp625.aggr_info;_tmp627=_tmp625.targs;_LL42C: {
struct Cyc_List_List*_tmp659=Cyc_Tcutil_substs(rgn,inst,_tmp627);return _tmp659 == 
_tmp627?t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp65A=_cycalloc(sizeof(*
_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_AggrType_struct _tmp65B;_tmp65B.tag=10;
_tmp65B.f1=({struct Cyc_Absyn_AggrInfo _tmp65C;_tmp65C.aggr_info=(void*)_tmp626;
_tmp65C.targs=_tmp659;_tmp65C;});_tmp65B;});_tmp65A;});}_LL42D: if(_tmp623 <= (
void*)3  || *((int*)_tmp623)!= 2)goto _LL42F;_tmp628=((struct Cyc_Absyn_TunionType_struct*)
_tmp623)->f1;_tmp629=(void*)_tmp628.tunion_info;_tmp62A=_tmp628.targs;_tmp62B=(
void*)_tmp628.rgn;_LL42E: {struct Cyc_List_List*_tmp65D=Cyc_Tcutil_substs(rgn,
inst,_tmp62A);void*_tmp65E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp62B);return
_tmp65D == _tmp62A  && _tmp65E == _tmp62B?t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_Absyn_TunionType_struct
_tmp660;_tmp660.tag=2;_tmp660.f1=({struct Cyc_Absyn_TunionInfo _tmp661;_tmp661.tunion_info=(
void*)_tmp629;_tmp661.targs=_tmp65D;_tmp661.rgn=(void*)_tmp65E;_tmp661;});
_tmp660;});_tmp65F;});}_LL42F: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 3)goto
_LL431;_tmp62C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp623)->f1;_tmp62D=(
void*)_tmp62C.field_info;_tmp62E=_tmp62C.targs;_LL430: {struct Cyc_List_List*
_tmp662=Cyc_Tcutil_substs(rgn,inst,_tmp62E);return _tmp662 == _tmp62E?t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp663=_cycalloc(sizeof(*_tmp663));
_tmp663[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp664;_tmp664.tag=3;_tmp664.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp665;_tmp665.field_info=(void*)_tmp62D;_tmp665.targs=
_tmp662;_tmp665;});_tmp664;});_tmp663;});}_LL431: if(_tmp623 <= (void*)3  || *((int*)
_tmp623)!= 16)goto _LL433;_tmp62F=((struct Cyc_Absyn_TypedefType_struct*)_tmp623)->f1;
_tmp630=((struct Cyc_Absyn_TypedefType_struct*)_tmp623)->f2;_tmp631=((struct Cyc_Absyn_TypedefType_struct*)
_tmp623)->f3;_tmp632=((struct Cyc_Absyn_TypedefType_struct*)_tmp623)->f4;_LL432: {
struct Cyc_List_List*_tmp666=Cyc_Tcutil_substs(rgn,inst,_tmp630);return _tmp666 == 
_tmp630?t:(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp667=_cycalloc(sizeof(*
_tmp667));_tmp667[0]=({struct Cyc_Absyn_TypedefType_struct _tmp668;_tmp668.tag=16;
_tmp668.f1=_tmp62F;_tmp668.f2=_tmp666;_tmp668.f3=_tmp631;_tmp668.f4=_tmp632;
_tmp668;});_tmp667;});}_LL433: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 7)goto
_LL435;_tmp633=((struct Cyc_Absyn_ArrayType_struct*)_tmp623)->f1;_tmp634=(void*)
_tmp633.elt_type;_tmp635=_tmp633.tq;_tmp636=_tmp633.num_elts;_tmp637=_tmp633.zero_term;
_LL434: {void*_tmp669=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp634);return _tmp669 == 
_tmp634?t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp66A=_cycalloc(sizeof(*
_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_ArrayType_struct _tmp66B;_tmp66B.tag=7;
_tmp66B.f1=({struct Cyc_Absyn_ArrayInfo _tmp66C;_tmp66C.elt_type=(void*)_tmp669;
_tmp66C.tq=_tmp635;_tmp66C.num_elts=_tmp636;_tmp66C.zero_term=_tmp637;_tmp66C;});
_tmp66B;});_tmp66A;});}_LL435: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 4)goto
_LL437;_tmp638=((struct Cyc_Absyn_PointerType_struct*)_tmp623)->f1;_tmp639=(void*)
_tmp638.elt_typ;_tmp63A=_tmp638.elt_tq;_tmp63B=_tmp638.ptr_atts;_tmp63C=(void*)
_tmp63B.rgn;_tmp63D=_tmp63B.nullable;_tmp63E=_tmp63B.bounds;_tmp63F=_tmp63B.zero_term;
_LL436: {void*_tmp66D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp639);void*_tmp66E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp63C);struct Cyc_Absyn_Conref*_tmp66F=_tmp63E;{void*_tmp670=(void*)(
Cyc_Absyn_compress_conref(_tmp63E))->v;void*_tmp671;void*_tmp672;_LL461: if(
_tmp670 <= (void*)1  || *((int*)_tmp670)!= 0)goto _LL463;_tmp671=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp670)->f1;if(_tmp671 <= (void*)1  || *((int*)_tmp671)!= 1)goto _LL463;_tmp672=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp671)->f1;_LL462: {void*_tmp673=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp672);if(_tmp672 != _tmp673)_tmp66F=Cyc_Absyn_new_conref((
void*)({struct Cyc_Absyn_AbsUpper_b_struct*_tmp674=_cycalloc(sizeof(*_tmp674));
_tmp674[0]=({struct Cyc_Absyn_AbsUpper_b_struct _tmp675;_tmp675.tag=1;_tmp675.f1=(
void*)_tmp673;_tmp675;});_tmp674;}));goto _LL460;}_LL463:;_LL464: goto _LL460;
_LL460:;}if((_tmp66D == _tmp639  && _tmp66E == _tmp63C) && _tmp66F == _tmp63E)return
t;return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp676=_cycalloc(sizeof(*
_tmp676));_tmp676[0]=({struct Cyc_Absyn_PointerType_struct _tmp677;_tmp677.tag=4;
_tmp677.f1=({struct Cyc_Absyn_PtrInfo _tmp678;_tmp678.elt_typ=(void*)_tmp66D;
_tmp678.elt_tq=_tmp63A;_tmp678.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp679;_tmp679.rgn=(
void*)_tmp66E;_tmp679.nullable=_tmp63D;_tmp679.bounds=_tmp66F;_tmp679.zero_term=
_tmp63F;_tmp679.loc=0;_tmp679;});_tmp678;});_tmp677;});_tmp676;});}_LL437: if(
_tmp623 <= (void*)3  || *((int*)_tmp623)!= 8)goto _LL439;_tmp640=((struct Cyc_Absyn_FnType_struct*)
_tmp623)->f1;_tmp641=_tmp640.tvars;_tmp642=_tmp640.effect;_tmp643=(void*)_tmp640.ret_typ;
_tmp644=_tmp640.args;_tmp645=_tmp640.c_varargs;_tmp646=_tmp640.cyc_varargs;
_tmp647=_tmp640.rgn_po;_tmp648=_tmp640.attributes;_LL438:{struct Cyc_List_List*
_tmp67A=_tmp641;for(0;_tmp67A != 0;_tmp67A=_tmp67A->tl){inst=({struct Cyc_List_List*
_tmp67B=_region_malloc(rgn,sizeof(*_tmp67B));_tmp67B->hd=({struct _tuple8*_tmp67C=
_region_malloc(rgn,sizeof(*_tmp67C));_tmp67C->f1=(struct Cyc_Absyn_Tvar*)_tmp67A->hd;
_tmp67C->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp67D=_cycalloc(sizeof(*
_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_VarType_struct _tmp67E;_tmp67E.tag=1;
_tmp67E.f1=(struct Cyc_Absyn_Tvar*)_tmp67A->hd;_tmp67E;});_tmp67D;});_tmp67C;});
_tmp67B->tl=inst;_tmp67B;});}}{struct Cyc_List_List*_tmp680;struct Cyc_List_List*
_tmp681;struct _tuple0 _tmp67F=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct _tuple2*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmp644));_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;{struct Cyc_List_List*_tmp682=
Cyc_Tcutil_substs(rgn,inst,_tmp681);struct Cyc_List_List*_tmp683=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp680,_tmp682));struct Cyc_Core_Opt*
eff2;if(_tmp642 == 0)eff2=0;else{void*_tmp684=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp642->v);if(_tmp684 == (void*)_tmp642->v)eff2=_tmp642;else{eff2=({struct
Cyc_Core_Opt*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->v=(void*)_tmp684;
_tmp685;});}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp646 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp687;struct Cyc_Absyn_Tqual _tmp688;void*
_tmp689;int _tmp68A;struct Cyc_Absyn_VarargInfo _tmp686=*_tmp646;_tmp687=_tmp686.name;
_tmp688=_tmp686.tq;_tmp689=(void*)_tmp686.type;_tmp68A=_tmp686.inject;{void*
_tmp68B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp689);if(_tmp68B == _tmp689)
cyc_varargs2=_tmp646;else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp68C=
_cycalloc(sizeof(*_tmp68C));_tmp68C->name=_tmp687;_tmp68C->tq=_tmp688;_tmp68C->type=(
void*)_tmp68B;_tmp68C->inject=_tmp68A;_tmp68C;});}}}{struct Cyc_List_List*rgn_po2;
struct Cyc_List_List*_tmp68E;struct Cyc_List_List*_tmp68F;struct _tuple0 _tmp68D=Cyc_List_rsplit(
rgn,rgn,_tmp647);_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;{struct Cyc_List_List*
_tmp690=Cyc_Tcutil_substs(rgn,inst,_tmp68E);struct Cyc_List_List*_tmp691=Cyc_Tcutil_substs(
rgn,inst,_tmp68F);if(_tmp690 == _tmp68E  && _tmp691 == _tmp68F)rgn_po2=_tmp647;
else{rgn_po2=Cyc_List_zip(_tmp690,_tmp691);}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_FnType_struct
_tmp693;_tmp693.tag=8;_tmp693.f1=({struct Cyc_Absyn_FnInfo _tmp694;_tmp694.tvars=
_tmp641;_tmp694.effect=eff2;_tmp694.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp643);_tmp694.args=_tmp683;_tmp694.c_varargs=_tmp645;_tmp694.cyc_varargs=
cyc_varargs2;_tmp694.rgn_po=rgn_po2;_tmp694.attributes=_tmp648;_tmp694;});
_tmp693;});_tmp692;});}}}}}_LL439: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 9)
goto _LL43B;_tmp649=((struct Cyc_Absyn_TupleType_struct*)_tmp623)->f1;_LL43A: {
struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp649);struct Cyc_List_List*_tmp696=Cyc_Tcutil_substs(rgn,inst,_tmp695);if(
_tmp696 == _tmp695)return t;{struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp649,_tmp696);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_TupleType_struct
_tmp699;_tmp699.tag=9;_tmp699.f1=_tmp697;_tmp699;});_tmp698;});}}_LL43B: if(
_tmp623 <= (void*)3  || *((int*)_tmp623)!= 11)goto _LL43D;_tmp64A=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp623)->f1;_tmp64B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp623)->f2;_LL43C: {struct Cyc_List_List*_tmp69A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp64B);struct Cyc_List_List*_tmp69B=
Cyc_Tcutil_substs(rgn,inst,_tmp69A);if(_tmp69B == _tmp69A)return t;{struct Cyc_List_List*
_tmp69C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmp64B,_tmp69B);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp69E;_tmp69E.tag=11;_tmp69E.f1=(void*)_tmp64A;_tmp69E.f2=_tmp69C;_tmp69E;});
_tmp69D;});}}_LL43D: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 0)goto _LL43F;
_tmp64C=((struct Cyc_Absyn_Evar_struct*)_tmp623)->f2;_LL43E: if(_tmp64C != 0)return
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp64C->v);else{return t;}_LL43F: if(
_tmp623 <= (void*)3  || *((int*)_tmp623)!= 15)goto _LL441;_tmp64D=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp623)->f1;_LL440: {void*_tmp69F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64D);return _tmp69F == _tmp64D?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp6A1;_tmp6A1.tag=15;_tmp6A1.f1=(void*)_tmp69F;_tmp6A1;});_tmp6A0;});}_LL441:
if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 14)goto _LL443;_tmp64E=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp623)->f1;_LL442: {void*_tmp6A2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64E);return _tmp6A2 == _tmp64E?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp6A4;_tmp6A4.tag=14;_tmp6A4.f1=(void*)_tmp6A2;_tmp6A4;});_tmp6A3;});}_LL443:
if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 17)goto _LL445;_tmp64F=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp623)->f1;_LL444: {void*_tmp6A5=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64F);return _tmp6A5 == _tmp64F?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_TagType_struct
_tmp6A7;_tmp6A7.tag=17;_tmp6A7.f1=(void*)_tmp6A5;_tmp6A7;});_tmp6A6;});}_LL445:
if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 18)goto _LL447;_LL446: goto _LL448;
_LL447: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 12)goto _LL449;_LL448: goto
_LL44A;_LL449: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 13)goto _LL44B;_LL44A:
goto _LL44C;_LL44B: if((int)_tmp623 != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if(
_tmp623 <= (void*)3  || *((int*)_tmp623)!= 5)goto _LL44F;_LL44E: goto _LL450;_LL44F:
if((int)_tmp623 != 1)goto _LL451;_LL450: goto _LL452;_LL451: if(_tmp623 <= (void*)3
 || *((int*)_tmp623)!= 6)goto _LL453;_LL452: goto _LL454;_LL453: if((int)_tmp623 != 2)
goto _LL455;_LL454: return t;_LL455: if(_tmp623 <= (void*)3  || *((int*)_tmp623)!= 21)
goto _LL457;_tmp650=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp623)->f1;_LL456: {
void*_tmp6A8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp650);return _tmp6A8 == _tmp650?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));
_tmp6A9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6AA;_tmp6AA.tag=21;_tmp6AA.f1=(
void*)_tmp6A8;_tmp6AA;});_tmp6A9;});}_LL457: if(_tmp623 <= (void*)3  || *((int*)
_tmp623)!= 19)goto _LL459;_tmp651=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp623)->f1;_LL458: {void*_tmp6AB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp651);
return _tmp6AB == _tmp651?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AC=
_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AD;
_tmp6AD.tag=19;_tmp6AD.f1=(void*)_tmp6AB;_tmp6AD;});_tmp6AC;});}_LL459: if(
_tmp623 <= (void*)3  || *((int*)_tmp623)!= 20)goto _LL428;_tmp652=((struct Cyc_Absyn_JoinEff_struct*)
_tmp623)->f1;_LL45A: {struct Cyc_List_List*_tmp6AE=Cyc_Tcutil_substs(rgn,inst,
_tmp652);return _tmp6AE == _tmp652?t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6B0;_tmp6B0.tag=20;_tmp6B0.f1=_tmp6AE;_tmp6B0;});_tmp6AF;});}_LL428:;}static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp6B1=(void*)ts->hd;
struct Cyc_List_List*_tmp6B2=ts->tl;void*_tmp6B3=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp6B1);struct Cyc_List_List*_tmp6B4=Cyc_Tcutil_substs(rgn,inst,_tmp6B2);if(
_tmp6B1 == _tmp6B3  && _tmp6B2 == _tmp6B4)return ts;return(struct Cyc_List_List*)((
struct Cyc_List_List*)({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));
_tmp6B5->hd=(void*)_tmp6B3;_tmp6B5->tl=_tmp6B4;_tmp6B5;}));}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6B6=_cycalloc(
sizeof(*_tmp6B6));_tmp6B6->f1=tv;_tmp6B6->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->v=(void*)k;_tmp6B7;}),({struct Cyc_Core_Opt*
_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->v=s;_tmp6B8;}));_tmp6B6;});}struct
_tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple9 _tmp6BA;struct Cyc_List_List*_tmp6BB;struct _RegionHandle*_tmp6BC;
struct _tuple9*_tmp6B9=env;_tmp6BA=*_tmp6B9;_tmp6BB=_tmp6BA.f1;_tmp6BC=_tmp6BA.f2;{
void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple8*_tmp6BD=_region_malloc(
_tmp6BC,sizeof(*_tmp6BD));_tmp6BD->f1=tv;_tmp6BD->f2=Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE->v=(void*)k;_tmp6BE;}),({
struct Cyc_Core_Opt*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->v=_tmp6BB;
_tmp6BF;}));_tmp6BD;});}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2))({
struct Cyc_String_pa_struct _tmp6C4;_tmp6C4.tag=0;_tmp6C4.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2));{struct Cyc_String_pa_struct
_tmp6C3;_tmp6C3.tag=0;_tmp6C3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k1));{struct Cyc_String_pa_struct _tmp6C2;_tmp6C2.tag=0;_tmp6C2.f1=(struct
_tagged_arr)((struct _tagged_arr)*tv->name);{void*_tmp6C0[3]={& _tmp6C2,& _tmp6C3,&
_tmp6C4};Cyc_Tcutil_terr(loc,({const char*_tmp6C1="type variable %s is used with inconsistent kinds %s and %s";
_tag_arr(_tmp6C1,sizeof(char),_get_zero_arr_size(_tmp6C1,59));}),_tag_arr(
_tmp6C0,sizeof(void*),3));}}}});if(tv->identity == 0)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(*((int*)_check_null(tv->identity))!= *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)))({void*_tmp6C5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6C6="same type variable has different identity!";_tag_arr(_tmp6C6,sizeof(
char),_get_zero_arr_size(_tmp6C6,43));}),_tag_arr(_tmp6C5,sizeof(void*),0));});}
return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();return({struct Cyc_List_List*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->hd=tv;_tmp6C7->tl=tvs;_tmp6C7;});}
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)({void*_tmp6C8[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp6C9="fast_add_free_tvar: bad identity in tv";
_tag_arr(_tmp6C9,sizeof(char),_get_zero_arr_size(_tmp6C9,39));}),_tag_arr(
_tmp6C8,sizeof(void*),0));});{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0)({void*_tmp6CA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp6CB="fast_add_free_tvar: bad identity in tvs2";_tag_arr(_tmp6CB,sizeof(char),
_get_zero_arr_size(_tmp6CB,41));}),_tag_arr(_tmp6CA,sizeof(void*),0));});if(*((
int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))== *((int*)
_check_null(tv->identity)))return tvs;}}return({struct Cyc_List_List*_tmp6CC=
_cycalloc(sizeof(*_tmp6CC));_tmp6CC->hd=tv;_tmp6CC->tl=tvs;_tmp6CC;});}static
struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*
tv){if(tv->identity == 0)({struct Cyc_String_pa_struct _tmp6CF;_tmp6CF.tag=0;
_tmp6CF.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string(tv));{
void*_tmp6CD[1]={& _tmp6CF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp6CE="bound tvar id for %s is NULL";_tag_arr(
_tmp6CE,sizeof(char),_get_zero_arr_size(_tmp6CE,29));}),_tag_arr(_tmp6CD,sizeof(
void*),1));}});return({struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0->hd=tv;_tmp6D0->tl=tvs;_tmp6D0;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmp6D1=Cyc_Tcutil_compress(e);int _tmp6D2;
_LL466: if(_tmp6D1 <= (void*)3  || *((int*)_tmp6D1)!= 0)goto _LL468;_tmp6D2=((struct
Cyc_Absyn_Evar_struct*)_tmp6D1)->f3;_LL467:{struct Cyc_List_List*es2=es;for(0;es2
!= 0;es2=es2->tl){void*_tmp6D3=Cyc_Tcutil_compress((void*)es2->hd);int _tmp6D4;
_LL46B: if(_tmp6D3 <= (void*)3  || *((int*)_tmp6D3)!= 0)goto _LL46D;_tmp6D4=((struct
Cyc_Absyn_Evar_struct*)_tmp6D3)->f3;_LL46C: if(_tmp6D2 == _tmp6D4)return es;goto
_LL46A;_LL46D:;_LL46E: goto _LL46A;_LL46A:;}}return({struct Cyc_List_List*_tmp6D5=
_cycalloc(sizeof(*_tmp6D5));_tmp6D5->hd=(void*)e;_tmp6D5->tl=es;_tmp6D5;});
_LL468:;_LL469: return es;_LL465:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;
tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)
_check_null(((struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(!
present)r=({struct Cyc_List_List*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6D6->tl=r;_tmp6D6;});}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)width))({struct Cyc_String_pa_struct _tmp6D9;_tmp6D9.tag=
0;_tmp6D9.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{void*_tmp6D7[1]={&
_tmp6D9};Cyc_Tcutil_terr(loc,({const char*_tmp6D8="bitfield %s does not have constant width";
_tag_arr(_tmp6D8,sizeof(char),_get_zero_arr_size(_tmp6D8,41));}),_tag_arr(
_tmp6D7,sizeof(void*),1));}});else{unsigned int _tmp6DB;int _tmp6DC;struct _tuple7
_tmp6DA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmp6DB=
_tmp6DA.f1;_tmp6DC=_tmp6DA.f2;if(!_tmp6DC)({void*_tmp6DD[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp6DE="bitfield width cannot use sizeof or offsetof";_tag_arr(
_tmp6DE,sizeof(char),_get_zero_arr_size(_tmp6DE,45));}),_tag_arr(_tmp6DD,sizeof(
void*),0));});w=_tmp6DB;}{void*_tmp6DF=Cyc_Tcutil_compress(field_typ);void*
_tmp6E0;_LL470: if(_tmp6DF <= (void*)3  || *((int*)_tmp6DF)!= 5)goto _LL472;_tmp6E0=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp6DF)->f2;_LL471:{void*_tmp6E1=
_tmp6E0;_LL475: if((int)_tmp6E1 != 0)goto _LL477;_LL476: if(w > 8)({void*_tmp6E2[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp6E3="bitfield larger than type";_tag_arr(
_tmp6E3,sizeof(char),_get_zero_arr_size(_tmp6E3,26));}),_tag_arr(_tmp6E2,sizeof(
void*),0));});goto _LL474;_LL477: if((int)_tmp6E1 != 1)goto _LL479;_LL478: if(w > 16)({
void*_tmp6E4[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E5="bitfield larger than type";
_tag_arr(_tmp6E5,sizeof(char),_get_zero_arr_size(_tmp6E5,26));}),_tag_arr(
_tmp6E4,sizeof(void*),0));});goto _LL474;_LL479: if((int)_tmp6E1 != 2)goto _LL47B;
_LL47A: if(w > 32)({void*_tmp6E6[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E7="bitfield larger than type";
_tag_arr(_tmp6E7,sizeof(char),_get_zero_arr_size(_tmp6E7,26));}),_tag_arr(
_tmp6E6,sizeof(void*),0));});goto _LL474;_LL47B: if((int)_tmp6E1 != 3)goto _LL474;
_LL47C: if(w > 64)({void*_tmp6E8[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp6E9="bitfield larger than type";
_tag_arr(_tmp6E9,sizeof(char),_get_zero_arr_size(_tmp6E9,26));}),_tag_arr(
_tmp6E8,sizeof(void*),0));});goto _LL474;_LL474:;}goto _LL46F;_LL472:;_LL473:({
struct Cyc_String_pa_struct _tmp6ED;_tmp6ED.tag=0;_tmp6ED.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ));{struct Cyc_String_pa_struct
_tmp6EC;_tmp6EC.tag=0;_tmp6EC.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
void*_tmp6EA[2]={& _tmp6EC,& _tmp6ED};Cyc_Tcutil_terr(loc,({const char*_tmp6EB="bitfield %s must have integral type but has type %s";
_tag_arr(_tmp6EB,sizeof(char),_get_zero_arr_size(_tmp6EB,52));}),_tag_arr(
_tmp6EA,sizeof(void*),2));}}});goto _LL46F;_LL46F:;}}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _tagged_arr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp6EE=(void*)atts->hd;_LL47E: if((int)
_tmp6EE != 5)goto _LL480;_LL47F: continue;_LL480: if(_tmp6EE <= (void*)17  || *((int*)
_tmp6EE)!= 1)goto _LL482;_LL481: continue;_LL482:;_LL483:({struct Cyc_String_pa_struct
_tmp6F2;_tmp6F2.tag=0;_tmp6F2.f1=(struct _tagged_arr)((struct _tagged_arr)*fn);{
struct Cyc_String_pa_struct _tmp6F1;_tmp6F1.tag=0;_tmp6F1.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd));{void*_tmp6EF[2]={&
_tmp6F1,& _tmp6F2};Cyc_Tcutil_terr(loc,({const char*_tmp6F0="bad attribute %s on member %s";
_tag_arr(_tmp6F0,sizeof(char),_get_zero_arr_size(_tmp6F0,30));}),_tag_arr(
_tmp6EF,sizeof(void*),2));}}});_LL47D:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*
kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int
generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(struct
Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp6F3=t;struct Cyc_Absyn_Typedefdecl*
_tmp6F4;void**_tmp6F5;_LL485: if(_tmp6F3 <= (void*)3  || *((int*)_tmp6F3)!= 16)goto
_LL487;_tmp6F4=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F3)->f3;_tmp6F5=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F3)->f4;_LL486: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp6F4))->tq).real_const  || (_tmp6F4->tq).print_const){if(
declared_const)({void*_tmp6F6[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp6F7="extra const";
_tag_arr(_tmp6F7,sizeof(char),_get_zero_arr_size(_tmp6F7,12));}),_tag_arr(
_tmp6F6,sizeof(void*),0));});return 1;}if((unsigned int)_tmp6F5)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp6F5);else{return declared_const;}_LL487:;_LL488: return
declared_const;_LL484:;}struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t){{void*_tmp6F8=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmp6F9;struct Cyc_Core_Opt**_tmp6FA;struct Cyc_Core_Opt*
_tmp6FB;struct Cyc_Core_Opt**_tmp6FC;struct Cyc_Absyn_Tvar*_tmp6FD;struct Cyc_List_List*
_tmp6FE;struct _tuple1*_tmp6FF;struct Cyc_Absyn_Enumdecl*_tmp700;struct Cyc_Absyn_Enumdecl**
_tmp701;struct Cyc_Absyn_TunionInfo _tmp702;void*_tmp703;void**_tmp704;struct Cyc_List_List*
_tmp705;struct Cyc_List_List**_tmp706;void*_tmp707;struct Cyc_Absyn_TunionFieldInfo
_tmp708;void*_tmp709;void**_tmp70A;struct Cyc_List_List*_tmp70B;struct Cyc_Absyn_PtrInfo
_tmp70C;void*_tmp70D;struct Cyc_Absyn_Tqual _tmp70E;struct Cyc_Absyn_Tqual*_tmp70F;
struct Cyc_Absyn_PtrAtts _tmp710;void*_tmp711;struct Cyc_Absyn_Conref*_tmp712;
struct Cyc_Absyn_Conref*_tmp713;struct Cyc_Absyn_Conref*_tmp714;void*_tmp715;void*
_tmp716;struct Cyc_Absyn_ArrayInfo _tmp717;void*_tmp718;struct Cyc_Absyn_Tqual
_tmp719;struct Cyc_Absyn_Tqual*_tmp71A;struct Cyc_Absyn_Exp*_tmp71B;struct Cyc_Absyn_Exp**
_tmp71C;struct Cyc_Absyn_Conref*_tmp71D;struct Cyc_Absyn_FnInfo _tmp71E;struct Cyc_List_List*
_tmp71F;struct Cyc_List_List**_tmp720;struct Cyc_Core_Opt*_tmp721;struct Cyc_Core_Opt**
_tmp722;void*_tmp723;struct Cyc_List_List*_tmp724;int _tmp725;struct Cyc_Absyn_VarargInfo*
_tmp726;struct Cyc_List_List*_tmp727;struct Cyc_List_List*_tmp728;struct Cyc_List_List*
_tmp729;void*_tmp72A;struct Cyc_List_List*_tmp72B;struct Cyc_Absyn_AggrInfo _tmp72C;
void*_tmp72D;void**_tmp72E;struct Cyc_List_List*_tmp72F;struct Cyc_List_List**
_tmp730;struct _tuple1*_tmp731;struct Cyc_List_List*_tmp732;struct Cyc_List_List**
_tmp733;struct Cyc_Absyn_Typedefdecl*_tmp734;struct Cyc_Absyn_Typedefdecl**_tmp735;
void**_tmp736;void***_tmp737;void*_tmp738;void*_tmp739;void*_tmp73A;struct Cyc_List_List*
_tmp73B;_LL48A: if((int)_tmp6F8 != 0)goto _LL48C;_LL48B: goto _LL489;_LL48C: if(
_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 0)goto _LL48E;_tmp6F9=((struct Cyc_Absyn_Evar_struct*)
_tmp6F8)->f1;_tmp6FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp6F8)->f1;_tmp6FB=((struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f2;_tmp6FC=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f2;_LL48D: if(*_tmp6FA == 
0)*_tmp6FA=({struct Cyc_Core_Opt*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C->v=(
void*)expected_kind;_tmp73C;});if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && expected_kind == (void*)3)*_tmp6FC=({struct Cyc_Core_Opt*_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D->v=(void*)((void*)2);_tmp73D;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp73E=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp743;_tmp743.tag=2;_tmp743.f1=0;_tmp743.f2=(void*)expected_kind;_tmp743;});
_tmp742;}));*_tmp6FC=({struct Cyc_Core_Opt*_tmp73F=_cycalloc(sizeof(*_tmp73F));
_tmp73F->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp740=_cycalloc(
sizeof(*_tmp740));_tmp740[0]=({struct Cyc_Absyn_VarType_struct _tmp741;_tmp741.tag=
1;_tmp741.f1=_tmp73E;_tmp741;});_tmp740;}));_tmp73F;});_tmp6FD=_tmp73E;goto
_LL48F;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL489;_LL48E: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 1)goto _LL490;
_tmp6FD=((struct Cyc_Absyn_VarType_struct*)_tmp6F8)->f1;_LL48F:{void*_tmp744=Cyc_Absyn_compress_kb((
void*)_tmp6FD->kind);struct Cyc_Core_Opt*_tmp745;struct Cyc_Core_Opt**_tmp746;
_LL4BD: if(*((int*)_tmp744)!= 1)goto _LL4BF;_tmp745=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp744)->f1;_tmp746=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp744)->f1;_LL4BE:*_tmp746=({struct Cyc_Core_Opt*_tmp747=_cycalloc(sizeof(*
_tmp747));_tmp747->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp748=
_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Absyn_Less_kb_struct _tmp749;
_tmp749.tag=2;_tmp749.f1=0;_tmp749.f2=(void*)expected_kind;_tmp749;});_tmp748;}));
_tmp747;});goto _LL4BC;_LL4BF:;_LL4C0: goto _LL4BC;_LL4BC:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp6FD);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6FD);goto _LL489;_LL490: if(_tmp6F8 <= (void*)3  || *((int*)
_tmp6F8)!= 13)goto _LL492;_tmp6FE=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6F8)->f1;
_LL491: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp74A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp74A;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){struct Cyc_Absyn_Enumfield*_tmp74B=(
struct Cyc_Absyn_Enumfield*)_tmp6FE->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp74B->name).f2))({struct Cyc_String_pa_struct
_tmp74E;_tmp74E.tag=0;_tmp74E.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp74B->name).f2);{void*_tmp74C[1]={& _tmp74E};Cyc_Tcutil_terr(_tmp74B->loc,({
const char*_tmp74D="duplicate enum field name %s";_tag_arr(_tmp74D,sizeof(char),
_get_zero_arr_size(_tmp74D,29));}),_tag_arr(_tmp74C,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp74F=_region_malloc(uprev_rgn,sizeof(*
_tmp74F));_tmp74F->hd=(*_tmp74B->name).f2;_tmp74F->tl=prev_fields;_tmp74F;});}
if(_tmp74B->tag == 0)_tmp74B->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp74B->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp74B->tag)))({struct Cyc_String_pa_struct _tmp752;_tmp752.tag=0;
_tmp752.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp74B->name).f2);{void*
_tmp750[1]={& _tmp752};Cyc_Tcutil_terr(loc,({const char*_tmp751="enum field %s: expression is not constant";
_tag_arr(_tmp751,sizeof(char),_get_zero_arr_size(_tmp751,42));}),_tag_arr(
_tmp750,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp74B->tag))).f1;tag_count=t1 + 1;(*_tmp74B->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp754;_tmp754.tag=1;_tmp754.f1=te->ns;_tmp754;});
_tmp753;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp74B->name).f2,({
struct _tuple17*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp757;_tmp757.tag=4;_tmp757.f1=(void*)t;
_tmp757.f2=_tmp74B;_tmp757;});_tmp756;});_tmp755->f2=1;_tmp755;}));}}};
_pop_region(uprev_rgn);}goto _LL489;}_LL492: if(_tmp6F8 <= (void*)3  || *((int*)
_tmp6F8)!= 12)goto _LL494;_tmp6FF=((struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f1;
_tmp700=((struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f2;_tmp701=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f2;_LL493: if(*_tmp701 == 0  || ((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp701))->fields == 0){struct _handler_cons
_tmp758;_push_handler(& _tmp758);{int _tmp75A=0;if(setjmp(_tmp758.handler))_tmp75A=
1;if(!_tmp75A){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp6FF);*_tmp701=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp759=(void*)_exn_thrown;void*_tmp75C=_tmp759;_LL4C2: if(_tmp75C != Cyc_Dict_Absent)
goto _LL4C4;_LL4C3: {struct Cyc_Tcenv_Genv*_tmp75D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp75E=({struct Cyc_Absyn_Enumdecl*_tmp75F=_cycalloc(
sizeof(*_tmp75F));_tmp75F->sc=(void*)((void*)3);_tmp75F->name=_tmp6FF;_tmp75F->fields=
0;_tmp75F;});Cyc_Tc_tcEnumdecl(te,_tmp75D,loc,_tmp75E);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6FF);*_tmp701=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4C1;}}_LL4C4:;_LL4C5:(void)_throw(_tmp75C);_LL4C1:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp701);*_tmp6FF=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL489;}_LL494: if(_tmp6F8 <= (void*)3
 || *((int*)_tmp6F8)!= 2)goto _LL496;_tmp702=((struct Cyc_Absyn_TunionType_struct*)
_tmp6F8)->f1;_tmp703=(void*)_tmp702.tunion_info;_tmp704=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6F8)->f1).tunion_info;_tmp705=_tmp702.targs;_tmp706=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6F8)->f1).targs;_tmp707=(void*)_tmp702.rgn;
_LL495: {struct Cyc_List_List*_tmp760=*_tmp706;{void*_tmp761=*_tmp704;struct Cyc_Absyn_UnknownTunionInfo
_tmp762;struct _tuple1*_tmp763;int _tmp764;struct Cyc_Absyn_Tuniondecl**_tmp765;
struct Cyc_Absyn_Tuniondecl*_tmp766;_LL4C7: if(*((int*)_tmp761)!= 0)goto _LL4C9;
_tmp762=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp761)->f1;_tmp763=_tmp762.name;
_tmp764=_tmp762.is_xtunion;_LL4C8: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp767;_push_handler(& _tmp767);{int _tmp769=0;if(setjmp(_tmp767.handler))
_tmp769=1;if(!_tmp769){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp763);;
_pop_handler();}else{void*_tmp768=(void*)_exn_thrown;void*_tmp76B=_tmp768;_LL4CC:
if(_tmp76B != Cyc_Dict_Absent)goto _LL4CE;_LL4CD: {struct Cyc_Tcenv_Genv*_tmp76C=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp76D=({struct Cyc_Absyn_Tuniondecl*
_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->sc=(void*)((void*)3);_tmp774->name=
_tmp763;_tmp774->tvs=0;_tmp774->fields=0;_tmp774->is_xtunion=_tmp764;_tmp774;});
Cyc_Tc_tcTuniondecl(te,_tmp76C,loc,_tmp76D);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp763);if(_tmp760 != 0){({struct Cyc_String_pa_struct _tmp771;_tmp771.tag=0;
_tmp771.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp763));{struct Cyc_String_pa_struct _tmp770;_tmp770.tag=0;_tmp770.f1=(struct
_tagged_arr)(_tmp764?(struct _tagged_arr)((struct _tagged_arr)({const char*_tmp772="xtunion";
_tag_arr(_tmp772,sizeof(char),_get_zero_arr_size(_tmp772,8));})):(struct
_tagged_arr)({const char*_tmp773="tunion";_tag_arr(_tmp773,sizeof(char),
_get_zero_arr_size(_tmp773,7));}));{void*_tmp76E[2]={& _tmp770,& _tmp771};Cyc_Tcutil_terr(
loc,({const char*_tmp76F="declare parameterized %s %s before using";_tag_arr(
_tmp76F,sizeof(char),_get_zero_arr_size(_tmp76F,41));}),_tag_arr(_tmp76E,sizeof(
void*),2));}}});return cvtenv;}goto _LL4CB;}_LL4CE:;_LL4CF:(void)_throw(_tmp76B);
_LL4CB:;}}}if((*tudp)->is_xtunion != _tmp764)({struct Cyc_String_pa_struct _tmp777;
_tmp777.tag=0;_tmp777.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp763));{void*_tmp775[1]={& _tmp777};Cyc_Tcutil_terr(loc,({const char*_tmp776="[x]tunion is different from type declaration %s";
_tag_arr(_tmp776,sizeof(char),_get_zero_arr_size(_tmp776,48));}),_tag_arr(
_tmp775,sizeof(void*),1));}});*_tmp704=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp779;_tmp779.tag=1;_tmp779.f1=tudp;_tmp779;});_tmp778;});_tmp766=*tudp;goto
_LL4CA;}_LL4C9: if(*((int*)_tmp761)!= 1)goto _LL4C6;_tmp765=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp761)->f1;_tmp766=*_tmp765;_LL4CA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp707);{struct Cyc_List_List*tvs=_tmp766->tvs;for(0;_tmp760 != 0
 && tvs != 0;(_tmp760=_tmp760->tl,tvs=tvs->tl)){void*t1=(void*)_tmp760->hd;void*
k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp760 != 0)({struct Cyc_String_pa_struct _tmp77C;_tmp77C.tag=
0;_tmp77C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp766->name));{void*_tmp77A[1]={& _tmp77C};Cyc_Tcutil_terr(loc,({const char*
_tmp77B="too many type arguments for tunion %s";_tag_arr(_tmp77B,sizeof(char),
_get_zero_arr_size(_tmp77B,38));}),_tag_arr(_tmp77A,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->hd=(void*)e;_tmp77D->tl=
hidden_ts;_tmp77D;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp706=Cyc_List_imp_append(*_tmp706,Cyc_List_imp_rev(hidden_ts));}goto _LL4C6;}
_LL4C6:;}goto _LL489;}_LL496: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 3)goto
_LL498;_tmp708=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6F8)->f1;_tmp709=(
void*)_tmp708.field_info;_tmp70A=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6F8)->f1).field_info;_tmp70B=_tmp708.targs;_LL497:{void*_tmp77E=*_tmp70A;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp77F;struct _tuple1*_tmp780;struct
_tuple1*_tmp781;int _tmp782;struct Cyc_Absyn_Tuniondecl*_tmp783;struct Cyc_Absyn_Tunionfield*
_tmp784;_LL4D1: if(*((int*)_tmp77E)!= 0)goto _LL4D3;_tmp77F=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp77E)->f1;_tmp780=_tmp77F.tunion_name;_tmp781=_tmp77F.field_name;_tmp782=
_tmp77F.is_xtunion;_LL4D2: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp785;_push_handler(& _tmp785);{int _tmp787=0;if(setjmp(
_tmp785.handler))_tmp787=1;if(!_tmp787){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp780);;_pop_handler();}else{void*_tmp786=(void*)_exn_thrown;void*_tmp789=
_tmp786;_LL4D6: if(_tmp789 != Cyc_Dict_Absent)goto _LL4D8;_LL4D7:({struct Cyc_String_pa_struct
_tmp78C;_tmp78C.tag=0;_tmp78C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp780));{void*_tmp78A[1]={& _tmp78C};Cyc_Tcutil_terr(loc,({const char*_tmp78B="unbound type tunion %s";
_tag_arr(_tmp78B,sizeof(char),_get_zero_arr_size(_tmp78B,23));}),_tag_arr(
_tmp78A,sizeof(void*),1));}});return cvtenv;_LL4D8:;_LL4D9:(void)_throw(_tmp789);
_LL4D5:;}}}{struct _handler_cons _tmp78D;_push_handler(& _tmp78D);{int _tmp78F=0;if(
setjmp(_tmp78D.handler))_tmp78F=1;if(!_tmp78F){{void*_tmp790=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp781);struct Cyc_Absyn_Tuniondecl*_tmp791;struct Cyc_Absyn_Tunionfield*
_tmp792;_LL4DB: if(*((int*)_tmp790)!= 2)goto _LL4DD;_tmp791=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp790)->f1;_tmp792=((struct Cyc_Tcenv_TunionRes_struct*)_tmp790)->f2;_LL4DC: tuf=
_tmp792;tud=_tmp791;if(tud->is_xtunion != _tmp782)({struct Cyc_String_pa_struct
_tmp795;_tmp795.tag=0;_tmp795.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp780));{void*_tmp793[1]={& _tmp795};Cyc_Tcutil_terr(loc,({const char*_tmp794="[x]tunion is different from type declaration %s";
_tag_arr(_tmp794,sizeof(char),_get_zero_arr_size(_tmp794,48));}),_tag_arr(
_tmp793,sizeof(void*),1));}});goto _LL4DA;_LL4DD:;_LL4DE:({struct Cyc_String_pa_struct
_tmp799;_tmp799.tag=0;_tmp799.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp780));{struct Cyc_String_pa_struct _tmp798;_tmp798.tag=0;_tmp798.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp781));{void*_tmp796[
2]={& _tmp798,& _tmp799};Cyc_Tcutil_terr(loc,({const char*_tmp797="unbound field %s in type tunion %s";
_tag_arr(_tmp797,sizeof(char),_get_zero_arr_size(_tmp797,35));}),_tag_arr(
_tmp796,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp79A=cvtenv;
_npop_handler(0);return _tmp79A;}_LL4DA:;};_pop_handler();}else{void*_tmp78E=(
void*)_exn_thrown;void*_tmp79C=_tmp78E;_LL4E0: if(_tmp79C != Cyc_Dict_Absent)goto
_LL4E2;_LL4E1:({struct Cyc_String_pa_struct _tmp7A0;_tmp7A0.tag=0;_tmp7A0.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp780));{struct
Cyc_String_pa_struct _tmp79F;_tmp79F.tag=0;_tmp79F.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp781));{void*_tmp79D[2]={& _tmp79F,&
_tmp7A0};Cyc_Tcutil_terr(loc,({const char*_tmp79E="unbound field %s in type tunion %s";
_tag_arr(_tmp79E,sizeof(char),_get_zero_arr_size(_tmp79E,35));}),_tag_arr(
_tmp79D,sizeof(void*),2));}}});return cvtenv;_LL4E2:;_LL4E3:(void)_throw(_tmp79C);
_LL4DF:;}}}*_tmp70A=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp7A2;_tmp7A2.tag=1;_tmp7A2.f1=tud;_tmp7A2.f2=tuf;_tmp7A2;});_tmp7A1;});
_tmp783=tud;_tmp784=tuf;goto _LL4D4;}_LL4D3: if(*((int*)_tmp77E)!= 1)goto _LL4D0;
_tmp783=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77E)->f1;_tmp784=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp77E)->f2;_LL4D4: {struct Cyc_List_List*tvs=
_tmp783->tvs;for(0;_tmp70B != 0  && tvs != 0;(_tmp70B=_tmp70B->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp70B->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp70B != 
0)({struct Cyc_String_pa_struct _tmp7A6;_tmp7A6.tag=0;_tmp7A6.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp784->name));{struct
Cyc_String_pa_struct _tmp7A5;_tmp7A5.tag=0;_tmp7A5.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp783->name));{void*_tmp7A3[2]={& _tmp7A5,&
_tmp7A6};Cyc_Tcutil_terr(loc,({const char*_tmp7A4="too many type arguments for tunion %s.%s";
_tag_arr(_tmp7A4,sizeof(char),_get_zero_arr_size(_tmp7A4,41));}),_tag_arr(
_tmp7A3,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct _tmp7AA;
_tmp7AA.tag=0;_tmp7AA.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp784->name));{struct Cyc_String_pa_struct _tmp7A9;_tmp7A9.tag=0;_tmp7A9.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp783->name));{
void*_tmp7A7[2]={& _tmp7A9,& _tmp7AA};Cyc_Tcutil_terr(loc,({const char*_tmp7A8="too few type arguments for tunion %s.%s";
_tag_arr(_tmp7A8,sizeof(char),_get_zero_arr_size(_tmp7A8,40));}),_tag_arr(
_tmp7A7,sizeof(void*),2));}}});goto _LL4D0;}_LL4D0:;}goto _LL489;_LL498: if(_tmp6F8
<= (void*)3  || *((int*)_tmp6F8)!= 4)goto _LL49A;_tmp70C=((struct Cyc_Absyn_PointerType_struct*)
_tmp6F8)->f1;_tmp70D=(void*)_tmp70C.elt_typ;_tmp70E=_tmp70C.elt_tq;_tmp70F=(
struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)_tmp6F8)->f1).elt_tq;
_tmp710=_tmp70C.ptr_atts;_tmp711=(void*)_tmp710.rgn;_tmp712=_tmp710.nullable;
_tmp713=_tmp710.bounds;_tmp714=_tmp710.zero_term;_LL499: {int is_zero_terminated;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp70D);_tmp70F->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmp70F->print_const,_tmp70D);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp711);{void*_tmp7AB=(
void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp714))->v;int _tmp7AC;_LL4E5: if((int)_tmp7AB != 0)goto _LL4E7;_LL4E6:{void*
_tmp7AD=Cyc_Tcutil_compress(_tmp70D);void*_tmp7AE;_LL4EC: if(_tmp7AD <= (void*)3
 || *((int*)_tmp7AD)!= 5)goto _LL4EE;_tmp7AE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7AD)->f2;if((int)_tmp7AE != 0)goto _LL4EE;_LL4ED:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp714,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4EB;_LL4EE:;_LL4EF:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp714,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4EB;
_LL4EB:;}goto _LL4E4;_LL4E7: if(_tmp7AB <= (void*)1  || *((int*)_tmp7AB)!= 0)goto
_LL4E9;_tmp7AC=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7AB)->f1;if(_tmp7AC
!= 1)goto _LL4E9;_LL4E8: if(!Cyc_Tcutil_admits_zero(_tmp70D))({struct Cyc_String_pa_struct
_tmp7B1;_tmp7B1.tag=0;_tmp7B1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp70D));{void*_tmp7AF[1]={& _tmp7B1};Cyc_Tcutil_terr(loc,({const char*_tmp7B0="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp7B0,sizeof(char),_get_zero_arr_size(_tmp7B0,53));}),_tag_arr(
_tmp7AF,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4E4;_LL4E9:;_LL4EA:
is_zero_terminated=0;goto _LL4E4;_LL4E4:;}{void*_tmp7B2=(void*)(Cyc_Absyn_compress_conref(
_tmp713))->v;void*_tmp7B3;void*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;void*_tmp7B6;
void*_tmp7B7;_LL4F1: if((int)_tmp7B2 != 0)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if(
_tmp7B2 <= (void*)1  || *((int*)_tmp7B2)!= 0)goto _LL4F5;_tmp7B3=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B2)->f1;if((int)_tmp7B3 != 0)goto _LL4F5;_LL4F4: goto _LL4F0;_LL4F5: if(_tmp7B2
<= (void*)1  || *((int*)_tmp7B2)!= 0)goto _LL4F7;_tmp7B4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B2)->f1;if(_tmp7B4 <= (void*)1  || *((int*)_tmp7B4)!= 0)goto _LL4F7;_tmp7B5=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7B4)->f1;_LL4F6: Cyc_Tcexp_tcExp(te,0,_tmp7B5);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B5))({void*_tmp7B8[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7B9="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp7B9,sizeof(char),_get_zero_arr_size(_tmp7B9,49));}),_tag_arr(
_tmp7B8,sizeof(void*),0));});{unsigned int _tmp7BB;int _tmp7BC;struct _tuple7
_tmp7BA=Cyc_Evexp_eval_const_uint_exp(_tmp7B5);_tmp7BB=_tmp7BA.f1;_tmp7BC=
_tmp7BA.f2;if(is_zero_terminated  && (!_tmp7BC  || _tmp7BB < 1))({void*_tmp7BD[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7BE="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp7BE,sizeof(char),_get_zero_arr_size(_tmp7BE,55));}),_tag_arr(
_tmp7BD,sizeof(void*),0));});goto _LL4F0;}_LL4F7: if(_tmp7B2 <= (void*)1  || *((int*)
_tmp7B2)!= 0)goto _LL4F9;_tmp7B6=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B2)->f1;if(_tmp7B6 <= (void*)1  || *((int*)_tmp7B6)!= 1)goto _LL4F9;_tmp7B7=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7B6)->f1;_LL4F8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp7B7);goto _LL4F0;_LL4F9: if(_tmp7B2 <= (void*)1  || *((
int*)_tmp7B2)!= 1)goto _LL4F0;_LL4FA:({void*_tmp7BF[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp7C0="forward constraint";
_tag_arr(_tmp7C0,sizeof(char),_get_zero_arr_size(_tmp7C0,19));}),_tag_arr(
_tmp7BF,sizeof(void*),0));});_LL4F0:;}goto _LL489;}_LL49A: if(_tmp6F8 <= (void*)3
 || *((int*)_tmp6F8)!= 17)goto _LL49C;_tmp715=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6F8)->f1;_LL49B: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp715);goto _LL489;_LL49C: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 14)goto
_LL49E;_tmp716=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6F8)->f1;_LL49D:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp716);goto _LL489;
_LL49E: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 5)goto _LL4A0;_LL49F: goto
_LL4A1;_LL4A0: if((int)_tmp6F8 != 1)goto _LL4A2;_LL4A1: goto _LL4A3;_LL4A2: if(_tmp6F8
<= (void*)3  || *((int*)_tmp6F8)!= 6)goto _LL4A4;_LL4A3: goto _LL489;_LL4A4: if(
_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 7)goto _LL4A6;_tmp717=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6F8)->f1;_tmp718=(void*)_tmp717.elt_type;_tmp719=_tmp717.tq;_tmp71A=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp6F8)->f1).tq;_tmp71B=
_tmp717.num_elts;_tmp71C=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp6F8)->f1).num_elts;_tmp71D=_tmp717.zero_term;_LL4A5: {struct Cyc_Absyn_Exp*
_tmp7C1=*_tmp71C;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,
_tmp718);_tmp71A->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp71A->print_const,
_tmp718);{int is_zero_terminated;{void*_tmp7C2=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp71D))->v;int _tmp7C3;
_LL4FC: if((int)_tmp7C2 != 0)goto _LL4FE;_LL4FD:((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp71D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4FB;_LL4FE: if(
_tmp7C2 <= (void*)1  || *((int*)_tmp7C2)!= 0)goto _LL500;_tmp7C3=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7C2)->f1;if(_tmp7C3 != 1)goto _LL500;_LL4FF: if(!Cyc_Tcutil_admits_zero(_tmp718))({
struct Cyc_String_pa_struct _tmp7C6;_tmp7C6.tag=0;_tmp7C6.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp718));{void*_tmp7C4[1]={& _tmp7C6};
Cyc_Tcutil_terr(loc,({const char*_tmp7C5="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp7C5,sizeof(char),_get_zero_arr_size(_tmp7C5,51));}),_tag_arr(
_tmp7C4,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4FB;_LL500:;_LL501:
is_zero_terminated=0;goto _LL4FB;_LL4FB:;}if(_tmp7C1 == 0){if(is_zero_terminated)*
_tmp71C=(_tmp7C1=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*
_tmp7C7[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7C8="array bound defaults to 0 here";
_tag_arr(_tmp7C8,sizeof(char),_get_zero_arr_size(_tmp7C8,31));}),_tag_arr(
_tmp7C7,sizeof(void*),0));});*_tmp71C=(_tmp7C1=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7C1);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp7C1))({void*_tmp7C9[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7CA="array bounds expression is not constant";_tag_arr(_tmp7CA,
sizeof(char),_get_zero_arr_size(_tmp7CA,40));}),_tag_arr(_tmp7C9,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp7C1))({void*_tmp7CB[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7CC="array bounds expression is not an unsigned int";
_tag_arr(_tmp7CC,sizeof(char),_get_zero_arr_size(_tmp7CC,47));}),_tag_arr(
_tmp7CB,sizeof(void*),0));});{unsigned int _tmp7CE;int _tmp7CF;struct _tuple7
_tmp7CD=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp7C1);_tmp7CE=
_tmp7CD.f1;_tmp7CF=_tmp7CD.f2;if((is_zero_terminated  && _tmp7CF) && _tmp7CE < 1)({
void*_tmp7D0[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7D1="zero terminated array cannot have zero elements";
_tag_arr(_tmp7D1,sizeof(char),_get_zero_arr_size(_tmp7D1,48));}),_tag_arr(
_tmp7D0,sizeof(void*),0));});goto _LL489;}}}_LL4A6: if(_tmp6F8 <= (void*)3  || *((
int*)_tmp6F8)!= 8)goto _LL4A8;_tmp71E=((struct Cyc_Absyn_FnType_struct*)_tmp6F8)->f1;
_tmp71F=_tmp71E.tvars;_tmp720=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6F8)->f1).tvars;_tmp721=_tmp71E.effect;_tmp722=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6F8)->f1).effect;_tmp723=(void*)_tmp71E.ret_typ;
_tmp724=_tmp71E.args;_tmp725=_tmp71E.c_varargs;_tmp726=_tmp71E.cyc_varargs;
_tmp727=_tmp71E.rgn_po;_tmp728=_tmp71E.attributes;_LL4A7: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp728 != 0;_tmp728=_tmp728->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp728->hd))({struct Cyc_String_pa_struct _tmp7D4;
_tmp7D4.tag=0;_tmp7D4.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp728->hd));{void*_tmp7D2[1]={& _tmp7D4};Cyc_Tcutil_terr(loc,({const char*
_tmp7D3="bad function type attribute %s";_tag_arr(_tmp7D3,sizeof(char),
_get_zero_arr_size(_tmp7D3,31));}),_tag_arr(_tmp7D2,sizeof(void*),1));}});{void*
_tmp7D5=(void*)_tmp728->hd;void*_tmp7D6;int _tmp7D7;int _tmp7D8;_LL503: if((int)
_tmp7D5 != 0)goto _LL505;_LL504: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL502;_LL505: if((int)_tmp7D5 != 1)goto _LL507;_LL506: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL502;_LL507: if((int)_tmp7D5 != 2)goto _LL509;_LL508: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL502;_LL509: if(_tmp7D5 <= (void*)
17  || *((int*)_tmp7D5)!= 3)goto _LL50B;_tmp7D6=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7D5)->f1;_tmp7D7=((struct Cyc_Absyn_Format_att_struct*)_tmp7D5)->f2;_tmp7D8=((
struct Cyc_Absyn_Format_att_struct*)_tmp7D5)->f3;_LL50A: if(!seen_format){
seen_format=1;ft=_tmp7D6;fmt_desc_arg=_tmp7D7;fmt_arg_start=_tmp7D8;}else{({void*
_tmp7D9[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DA="function can't have multiple format attributes";
_tag_arr(_tmp7DA,sizeof(char),_get_zero_arr_size(_tmp7DA,47));}),_tag_arr(
_tmp7D9,sizeof(void*),0));});}goto _LL502;_LL50B:;_LL50C: goto _LL502;_LL502:;}}if(
num_convs > 1)({void*_tmp7DB[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DC="function can't have multiple calling conventions";
_tag_arr(_tmp7DC,sizeof(char),_get_zero_arr_size(_tmp7DC,49));}),_tag_arr(
_tmp7DB,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp720);{struct
Cyc_List_List*b=*_tmp720;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7DD=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7DE;_LL50E: if(*((int*)_tmp7DD)!= 0)goto
_LL510;_tmp7DE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7DD)->f1;if((int)
_tmp7DE != 1)goto _LL510;_LL50F:({struct Cyc_String_pa_struct _tmp7E1;_tmp7E1.tag=0;
_tmp7E1.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7DF[1]={& _tmp7E1};Cyc_Tcutil_terr(loc,({const char*_tmp7E0="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7E0,sizeof(char),_get_zero_arr_size(_tmp7E0,51));}),_tag_arr(
_tmp7DF,sizeof(void*),1));}});goto _LL50D;_LL510:;_LL511: goto _LL50D;_LL50D:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7E2=({struct Cyc_Tcutil_CVTEnv _tmp85E;_tmp85E.kind_env=
cvtenv.kind_env;_tmp85E.free_vars=0;_tmp85E.free_evars=0;_tmp85E.generalize_evars=
cvtenv.generalize_evars;_tmp85E.fn_result=1;_tmp85E;});_tmp7E2=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E2,(void*)1,_tmp723);_tmp7E2.fn_result=0;{struct Cyc_List_List*a=
_tmp724;for(0;a != 0;a=a->tl){struct _tuple2*_tmp7E3=(struct _tuple2*)a->hd;void*
_tmp7E4=(*_tmp7E3).f3;_tmp7E2=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7E2,(void*)
1,_tmp7E4);((*_tmp7E3).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmp7E3).f2).print_const,_tmp7E4);}}if(_tmp726 != 0){if(_tmp725)({void*_tmp7E5[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp7E6="both c_vararg and cyc_vararg";_tag_arr(_tmp7E6,sizeof(char),
_get_zero_arr_size(_tmp7E6,29));}),_tag_arr(_tmp7E5,sizeof(void*),0));});{void*
_tmp7E8;int _tmp7E9;struct Cyc_Absyn_VarargInfo _tmp7E7=*_tmp726;_tmp7E8=(void*)
_tmp7E7.type;_tmp7E9=_tmp7E7.inject;_tmp7E2=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E2,(void*)1,_tmp7E8);(_tmp726->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp726->tq).print_const,_tmp7E8);if(_tmp7E9){void*_tmp7EA=Cyc_Tcutil_compress(
_tmp7E8);struct Cyc_Absyn_TunionInfo _tmp7EB;void*_tmp7EC;_LL513: if(_tmp7EA <= (
void*)3  || *((int*)_tmp7EA)!= 2)goto _LL515;_tmp7EB=((struct Cyc_Absyn_TunionType_struct*)
_tmp7EA)->f1;_tmp7EC=(void*)_tmp7EB.tunion_info;if(*((int*)_tmp7EC)!= 1)goto
_LL515;_LL514: goto _LL512;_LL515:;_LL516:({void*_tmp7ED[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7EE="can't inject a non-[x]tunion type";_tag_arr(_tmp7EE,sizeof(
char),_get_zero_arr_size(_tmp7EE,34));}),_tag_arr(_tmp7ED,sizeof(void*),0));});
goto _LL512;_LL512:;}}}if(seen_format){int _tmp7EF=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp724);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp7EF) || 
fmt_arg_start < 0) || _tmp726 == 0  && fmt_arg_start != 0) || _tmp726 != 0  && 
fmt_arg_start != _tmp7EF + 1)({void*_tmp7F0[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7F1="bad format descriptor";_tag_arr(_tmp7F1,sizeof(char),_get_zero_arr_size(
_tmp7F1,22));}),_tag_arr(_tmp7F0,sizeof(void*),0));});else{void*_tmp7F3;struct
_tuple2 _tmp7F2=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp724,fmt_desc_arg - 1);_tmp7F3=_tmp7F2.f3;{void*_tmp7F4=Cyc_Tcutil_compress(
_tmp7F3);struct Cyc_Absyn_PtrInfo _tmp7F5;void*_tmp7F6;struct Cyc_Absyn_PtrAtts
_tmp7F7;struct Cyc_Absyn_Conref*_tmp7F8;struct Cyc_Absyn_Conref*_tmp7F9;_LL518: if(
_tmp7F4 <= (void*)3  || *((int*)_tmp7F4)!= 4)goto _LL51A;_tmp7F5=((struct Cyc_Absyn_PointerType_struct*)
_tmp7F4)->f1;_tmp7F6=(void*)_tmp7F5.elt_typ;_tmp7F7=_tmp7F5.ptr_atts;_tmp7F8=
_tmp7F7.bounds;_tmp7F9=_tmp7F7.zero_term;_LL519:{struct _tuple6 _tmp7FB=({struct
_tuple6 _tmp7FA;_tmp7FA.f1=Cyc_Tcutil_compress(_tmp7F6);_tmp7FA.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp7F8);_tmp7FA;});void*_tmp7FC;void*_tmp7FD;void*_tmp7FE;void*
_tmp7FF;_LL51D: _tmp7FC=_tmp7FB.f1;if(_tmp7FC <= (void*)3  || *((int*)_tmp7FC)!= 5)
goto _LL51F;_tmp7FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FC)->f1;if((int)
_tmp7FD != 2)goto _LL51F;_tmp7FE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FC)->f2;
if((int)_tmp7FE != 0)goto _LL51F;_tmp7FF=_tmp7FB.f2;if((int)_tmp7FF != 0)goto _LL51F;
_LL51E: goto _LL51C;_LL51F:;_LL520:({void*_tmp800[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp801="format descriptor is not a char ? type";_tag_arr(_tmp801,
sizeof(char),_get_zero_arr_size(_tmp801,39));}),_tag_arr(_tmp800,sizeof(void*),0));});
goto _LL51C;_LL51C:;}goto _LL517;_LL51A:;_LL51B:({void*_tmp802[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp803="format descriptor is not a char ? type";_tag_arr(
_tmp803,sizeof(char),_get_zero_arr_size(_tmp803,39));}),_tag_arr(_tmp802,sizeof(
void*),0));});goto _LL517;_LL517:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp805=({struct _tuple6 _tmp804;_tmp804.f1=ft;_tmp804.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp726))->type);_tmp804;});void*
_tmp806;void*_tmp807;struct Cyc_Absyn_TunionInfo _tmp808;void*_tmp809;struct Cyc_Absyn_Tuniondecl**
_tmp80A;struct Cyc_Absyn_Tuniondecl*_tmp80B;void*_tmp80C;void*_tmp80D;struct Cyc_Absyn_TunionInfo
_tmp80E;void*_tmp80F;struct Cyc_Absyn_Tuniondecl**_tmp810;struct Cyc_Absyn_Tuniondecl*
_tmp811;_LL522: _tmp806=_tmp805.f1;if((int)_tmp806 != 0)goto _LL524;_tmp807=_tmp805.f2;
if(_tmp807 <= (void*)3  || *((int*)_tmp807)!= 2)goto _LL524;_tmp808=((struct Cyc_Absyn_TunionType_struct*)
_tmp807)->f1;_tmp809=(void*)_tmp808.tunion_info;if(*((int*)_tmp809)!= 1)goto
_LL524;_tmp80A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp809)->f1;_tmp80B=*
_tmp80A;_LL523: problem=Cyc_Absyn_qvar_cmp(_tmp80B->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL521;_LL524: _tmp80C=_tmp805.f1;if((int)_tmp80C != 1)goto _LL526;_tmp80D=
_tmp805.f2;if(_tmp80D <= (void*)3  || *((int*)_tmp80D)!= 2)goto _LL526;_tmp80E=((
struct Cyc_Absyn_TunionType_struct*)_tmp80D)->f1;_tmp80F=(void*)_tmp80E.tunion_info;
if(*((int*)_tmp80F)!= 1)goto _LL526;_tmp810=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp80F)->f1;_tmp811=*_tmp810;_LL525: problem=Cyc_Absyn_qvar_cmp(_tmp811->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL521;_LL526:;_LL527: problem=1;goto
_LL521;_LL521:;}if(problem)({void*_tmp812[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp813="format attribute and vararg types don't match";_tag_arr(_tmp813,sizeof(
char),_get_zero_arr_size(_tmp813,46));}),_tag_arr(_tmp812,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp727;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp815;
void*_tmp816;void*_tmp817;struct _tuple6*_tmp814=(struct _tuple6*)rpo->hd;_tmp815=*
_tmp814;_tmp816=_tmp815.f1;_tmp817=_tmp815.f2;_tmp7E2=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E2,(void*)4,_tmp816);_tmp7E2=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E2,(void*)3,_tmp817);}}if(*_tmp722 != 0)_tmp7E2=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E2,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp722))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7E2.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp818=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp819;struct Cyc_Core_Opt**_tmp81A;void*
_tmp81B;void*_tmp81C;void*_tmp81D;void*_tmp81E;struct Cyc_Core_Opt*_tmp81F;struct
Cyc_Core_Opt**_tmp820;void*_tmp821;void*_tmp822;struct Cyc_Core_Opt*_tmp823;
struct Cyc_Core_Opt**_tmp824;_LL529: if(*((int*)_tmp818)!= 2)goto _LL52B;_tmp819=((
struct Cyc_Absyn_Less_kb_struct*)_tmp818)->f1;_tmp81A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp818)->f1;_tmp81B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp818)->f2;if((int)_tmp81B != 3)goto _LL52B;_LL52A:*_tmp81A=({struct Cyc_Core_Opt*
_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp827;_tmp827.tag=0;_tmp827.f1=(void*)((void*)3);_tmp827;});_tmp826;}));
_tmp825;});goto _LL52C;_LL52B: if(*((int*)_tmp818)!= 0)goto _LL52D;_tmp81C=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp818)->f1;if((int)_tmp81C != 3)goto _LL52D;_LL52C:
effect=({struct Cyc_List_List*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp829=_cycalloc(sizeof(*
_tmp829));_tmp829[0]=({struct Cyc_Absyn_AccessEff_struct _tmp82A;_tmp82A.tag=19;
_tmp82A.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82B=_cycalloc(
sizeof(*_tmp82B));_tmp82B[0]=({struct Cyc_Absyn_VarType_struct _tmp82C;_tmp82C.tag=
1;_tmp82C.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp82C;});_tmp82B;}));_tmp82A;});
_tmp829;}));_tmp828->tl=effect;_tmp828;});goto _LL528;_LL52D: if(*((int*)_tmp818)
!= 2)goto _LL52F;_tmp81D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp818)->f2;
if((int)_tmp81D != 5)goto _LL52F;_LL52E: goto _LL530;_LL52F: if(*((int*)_tmp818)!= 0)
goto _LL531;_tmp81E=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp818)->f1;if((int)
_tmp81E != 5)goto _LL531;_LL530: goto _LL528;_LL531: if(*((int*)_tmp818)!= 2)goto
_LL533;_tmp81F=((struct Cyc_Absyn_Less_kb_struct*)_tmp818)->f1;_tmp820=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp818)->f1;_tmp821=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp818)->f2;if((int)_tmp821 != 4)goto _LL533;_LL532:*_tmp820=({struct Cyc_Core_Opt*
_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp82F;_tmp82F.tag=0;_tmp82F.f1=(void*)((void*)4);_tmp82F;});_tmp82E;}));
_tmp82D;});goto _LL534;_LL533: if(*((int*)_tmp818)!= 0)goto _LL535;_tmp822=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp818)->f1;if((int)_tmp822 != 4)goto _LL535;_LL534:
effect=({struct Cyc_List_List*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp831=_cycalloc(sizeof(*_tmp831));
_tmp831[0]=({struct Cyc_Absyn_VarType_struct _tmp832;_tmp832.tag=1;_tmp832.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp832;});_tmp831;}));_tmp830->tl=effect;_tmp830;});
goto _LL528;_LL535: if(*((int*)_tmp818)!= 1)goto _LL537;_tmp823=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp818)->f1;_tmp824=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp818)->f1;_LL536:*_tmp824=({struct Cyc_Core_Opt*_tmp833=_cycalloc(sizeof(*
_tmp833));_tmp833->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp834=
_cycalloc(sizeof(*_tmp834));_tmp834[0]=({struct Cyc_Absyn_Less_kb_struct _tmp835;
_tmp835.tag=2;_tmp835.f1=0;_tmp835.f2=(void*)((void*)0);_tmp835;});_tmp834;}));
_tmp833;});goto _LL538;_LL537:;_LL538: effect=({struct Cyc_List_List*_tmp836=
_cycalloc(sizeof(*_tmp836));_tmp836->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp837=_cycalloc(sizeof(*_tmp837));_tmp837[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp838;_tmp838.tag=21;_tmp838.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839[0]=({struct Cyc_Absyn_VarType_struct
_tmp83A;_tmp83A.tag=1;_tmp83A.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp83A;});
_tmp839;}));_tmp838;});_tmp837;}));_tmp836->tl=effect;_tmp836;});goto _LL528;
_LL528:;}}{struct Cyc_List_List*ts=_tmp7E2.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp83B=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL53A: if((int)_tmp83B != 3)goto
_LL53C;_LL53B: effect=({struct Cyc_List_List*_tmp83C=_cycalloc(sizeof(*_tmp83C));
_tmp83C->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp83D=_cycalloc(
sizeof(*_tmp83D));_tmp83D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp83E;_tmp83E.tag=
19;_tmp83E.f1=(void*)((void*)ts->hd);_tmp83E;});_tmp83D;}));_tmp83C->tl=effect;
_tmp83C;});goto _LL539;_LL53C: if((int)_tmp83B != 4)goto _LL53E;_LL53D: effect=({
struct Cyc_List_List*_tmp83F=_cycalloc(sizeof(*_tmp83F));_tmp83F->hd=(void*)((
void*)ts->hd);_tmp83F->tl=effect;_tmp83F;});goto _LL539;_LL53E: if((int)_tmp83B != 
5)goto _LL540;_LL53F: goto _LL539;_LL540:;_LL541: effect=({struct Cyc_List_List*
_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp842;_tmp842.tag=21;_tmp842.f1=(void*)((void*)ts->hd);_tmp842;});_tmp841;}));
_tmp840->tl=effect;_tmp840;});goto _LL539;_LL539:;}}*_tmp722=({struct Cyc_Core_Opt*
_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp845;_tmp845.tag=20;_tmp845.f1=effect;_tmp845;});_tmp844;}));_tmp843;});}if(*
_tmp720 != 0){struct Cyc_List_List*bs=*_tmp720;for(0;bs != 0;bs=bs->tl){void*
_tmp846=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp847;struct Cyc_Core_Opt**_tmp848;struct Cyc_Core_Opt*
_tmp849;struct Cyc_Core_Opt**_tmp84A;void*_tmp84B;struct Cyc_Core_Opt*_tmp84C;
struct Cyc_Core_Opt**_tmp84D;void*_tmp84E;struct Cyc_Core_Opt*_tmp84F;struct Cyc_Core_Opt**
_tmp850;void*_tmp851;void*_tmp852;_LL543: if(*((int*)_tmp846)!= 1)goto _LL545;
_tmp847=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp846)->f1;_tmp848=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp846)->f1;_LL544:({struct Cyc_String_pa_struct
_tmp855;_tmp855.tag=0;_tmp855.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp853[1]={& _tmp855};Cyc_Tcutil_warn(
loc,({const char*_tmp854="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp854,sizeof(char),_get_zero_arr_size(_tmp854,47));}),_tag_arr(
_tmp853,sizeof(void*),1));}});_tmp84A=_tmp848;goto _LL546;_LL545: if(*((int*)
_tmp846)!= 2)goto _LL547;_tmp849=((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f1;
_tmp84A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f1;
_tmp84B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f2;if((int)_tmp84B != 
1)goto _LL547;_LL546: _tmp84D=_tmp84A;goto _LL548;_LL547: if(*((int*)_tmp846)!= 2)
goto _LL549;_tmp84C=((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f1;_tmp84D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f1;_tmp84E=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f2;if((int)_tmp84E != 0)goto
_LL549;_LL548:*_tmp84D=({struct Cyc_Core_Opt*_tmp856=_cycalloc(sizeof(*_tmp856));
_tmp856->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp857=_cycalloc(
sizeof(*_tmp857));_tmp857[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp858;_tmp858.tag=0;
_tmp858.f1=(void*)((void*)2);_tmp858;});_tmp857;}));_tmp856;});goto _LL542;_LL549:
if(*((int*)_tmp846)!= 2)goto _LL54B;_tmp84F=((struct Cyc_Absyn_Less_kb_struct*)
_tmp846)->f1;_tmp850=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp846)->f1;_tmp851=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp846)->f2;
_LL54A:*_tmp850=({struct Cyc_Core_Opt*_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp85A=_cycalloc(sizeof(*_tmp85A));
_tmp85A[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp85B;_tmp85B.tag=0;_tmp85B.f1=(void*)
_tmp851;_tmp85B;});_tmp85A;}));_tmp859;});goto _LL542;_LL54B: if(*((int*)_tmp846)
!= 0)goto _LL54D;_tmp852=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp846)->f1;if((
int)_tmp852 != 1)goto _LL54D;_LL54C:({void*_tmp85C[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp85D="functions can't abstract M types";_tag_arr(_tmp85D,sizeof(
char),_get_zero_arr_size(_tmp85D,33));}),_tag_arr(_tmp85C,sizeof(void*),0));});
goto _LL542;_LL54D:;_LL54E: goto _LL542;_LL542:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7E2.kind_env,*_tmp720);_tmp7E2.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7E2.free_vars,*_tmp720);{struct Cyc_List_List*tvs=_tmp7E2.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7E2.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL489;}}_LL4A8: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 
9)goto _LL4AA;_tmp729=((struct Cyc_Absyn_TupleType_struct*)_tmp6F8)->f1;_LL4A9:
for(0;_tmp729 != 0;_tmp729=_tmp729->tl){struct _tuple4*_tmp85F=(struct _tuple4*)
_tmp729->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp85F).f2);((*_tmp85F).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp85F).f1).print_const,(*_tmp85F).f2);}goto _LL489;_LL4AA: if(_tmp6F8 <= (
void*)3  || *((int*)_tmp6F8)!= 11)goto _LL4AC;_tmp72A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6F8)->f1;_tmp72B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6F8)->f2;_LL4AB:{
struct _RegionHandle _tmp860=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp860;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp72B != 0;_tmp72B=_tmp72B->tl){struct Cyc_Absyn_Aggrfield _tmp862;struct
_tagged_arr*_tmp863;struct Cyc_Absyn_Tqual _tmp864;struct Cyc_Absyn_Tqual*_tmp865;
void*_tmp866;struct Cyc_Absyn_Exp*_tmp867;struct Cyc_List_List*_tmp868;struct Cyc_Absyn_Aggrfield*
_tmp861=(struct Cyc_Absyn_Aggrfield*)_tmp72B->hd;_tmp862=*_tmp861;_tmp863=_tmp862.name;
_tmp864=_tmp862.tq;_tmp865=(struct Cyc_Absyn_Tqual*)&(*_tmp861).tq;_tmp866=(void*)
_tmp862.type;_tmp867=_tmp862.width;_tmp868=_tmp862.attributes;if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp863))({struct Cyc_String_pa_struct
_tmp86B;_tmp86B.tag=0;_tmp86B.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp863);{
void*_tmp869[1]={& _tmp86B};Cyc_Tcutil_terr(loc,({const char*_tmp86A="duplicate field %s";
_tag_arr(_tmp86A,sizeof(char),_get_zero_arr_size(_tmp86A,19));}),_tag_arr(
_tmp869,sizeof(void*),1));}});if(Cyc_strcmp((struct _tagged_arr)*_tmp863,({const
char*_tmp86C="";_tag_arr(_tmp86C,sizeof(char),_get_zero_arr_size(_tmp86C,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp86D=_region_malloc(aprev_rgn,sizeof(*
_tmp86D));_tmp86D->hd=_tmp863;_tmp86D->tl=prev_fields;_tmp86D;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp866);_tmp865->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp865->print_const,_tmp866);if(_tmp72A == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp866))({struct Cyc_String_pa_struct _tmp870;_tmp870.tag=0;_tmp870.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp863);{void*_tmp86E[1]={& _tmp870};Cyc_Tcutil_warn(
loc,({const char*_tmp86F="union member %s is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp86F,sizeof(char),_get_zero_arr_size(_tmp86F,74));}),_tag_arr(
_tmp86E,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp866,_tmp867,
_tmp863);Cyc_Tcutil_check_field_atts(loc,_tmp863,_tmp868);}};_pop_region(
aprev_rgn);}goto _LL489;_LL4AC: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 10)
goto _LL4AE;_tmp72C=((struct Cyc_Absyn_AggrType_struct*)_tmp6F8)->f1;_tmp72D=(void*)
_tmp72C.aggr_info;_tmp72E=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6F8)->f1).aggr_info;
_tmp72F=_tmp72C.targs;_tmp730=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6F8)->f1).targs;_LL4AD:{void*_tmp871=*_tmp72E;void*_tmp872;struct _tuple1*
_tmp873;struct Cyc_Absyn_Aggrdecl**_tmp874;struct Cyc_Absyn_Aggrdecl*_tmp875;
_LL550: if(*((int*)_tmp871)!= 0)goto _LL552;_tmp872=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp871)->f1;_tmp873=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp871)->f2;_LL551: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp876;_push_handler(& _tmp876);{
int _tmp878=0;if(setjmp(_tmp876.handler))_tmp878=1;if(!_tmp878){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp873);*_tmp72E=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp879=
_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp87A;
_tmp87A.tag=1;_tmp87A.f1=adp;_tmp87A;});_tmp879;});;_pop_handler();}else{void*
_tmp877=(void*)_exn_thrown;void*_tmp87C=_tmp877;_LL555: if(_tmp87C != Cyc_Dict_Absent)
goto _LL557;_LL556: {struct Cyc_Tcenv_Genv*_tmp87D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp87E=({struct Cyc_Absyn_Aggrdecl*_tmp884=_cycalloc(
sizeof(*_tmp884));_tmp884->kind=(void*)_tmp872;_tmp884->sc=(void*)((void*)3);
_tmp884->name=_tmp873;_tmp884->tvs=0;_tmp884->impl=0;_tmp884->attributes=0;
_tmp884;});Cyc_Tc_tcAggrdecl(te,_tmp87D,loc,_tmp87E);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp873);*_tmp72E=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp87F=
_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp880;
_tmp880.tag=1;_tmp880.f1=adp;_tmp880;});_tmp87F;});if(*_tmp730 != 0){({struct Cyc_String_pa_struct
_tmp883;_tmp883.tag=0;_tmp883.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp873));{void*_tmp881[1]={& _tmp883};Cyc_Tcutil_terr(loc,({const char*_tmp882="declare parameterized type %s before using";
_tag_arr(_tmp882,sizeof(char),_get_zero_arr_size(_tmp882,43));}),_tag_arr(
_tmp881,sizeof(void*),1));}});return cvtenv;}goto _LL554;}_LL557:;_LL558:(void)
_throw(_tmp87C);_LL554:;}}}_tmp875=*adp;goto _LL553;}_LL552: if(*((int*)_tmp871)!= 
1)goto _LL54F;_tmp874=((struct Cyc_Absyn_KnownAggr_struct*)_tmp871)->f1;_tmp875=*
_tmp874;_LL553: {struct Cyc_List_List*tvs=_tmp875->tvs;struct Cyc_List_List*ts=*
_tmp730;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp887;_tmp887.tag=
0;_tmp887.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp875->name));{void*_tmp885[1]={& _tmp887};Cyc_Tcutil_terr(loc,({const char*
_tmp886="too many parameters for type %s";_tag_arr(_tmp886,sizeof(char),
_get_zero_arr_size(_tmp886,32));}),_tag_arr(_tmp885,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888->hd=(void*)e;_tmp888->tl=
hidden_ts;_tmp888;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp730=Cyc_List_imp_append(*_tmp730,Cyc_List_imp_rev(hidden_ts));}}_LL54F:;}
goto _LL489;_LL4AE: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 16)goto _LL4B0;
_tmp731=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f1;_tmp732=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F8)->f2;_tmp733=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F8)->f2;_tmp734=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f3;_tmp735=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f3;
_tmp736=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f4;_tmp737=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f4;_LL4AF: {struct Cyc_List_List*
targs=*_tmp733;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp889;
_push_handler(& _tmp889);{int _tmp88B=0;if(setjmp(_tmp889.handler))_tmp88B=1;if(!
_tmp88B){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp731);;_pop_handler();}else{
void*_tmp88A=(void*)_exn_thrown;void*_tmp88D=_tmp88A;_LL55A: if(_tmp88D != Cyc_Dict_Absent)
goto _LL55C;_LL55B:({struct Cyc_String_pa_struct _tmp890;_tmp890.tag=0;_tmp890.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp731));{void*
_tmp88E[1]={& _tmp890};Cyc_Tcutil_terr(loc,({const char*_tmp88F="unbound typedef name %s";
_tag_arr(_tmp88F,sizeof(char),_get_zero_arr_size(_tmp88F,24));}),_tag_arr(
_tmp88E,sizeof(void*),1));}});return cvtenv;_LL55C:;_LL55D:(void)_throw(_tmp88D);
_LL559:;}}}*_tmp735=(struct Cyc_Absyn_Typedefdecl*)td;_tmp731[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp891=_cycalloc(
sizeof(*_tmp891));_tmp891->hd=({struct _tuple8*_tmp892=_cycalloc(sizeof(*_tmp892));
_tmp892->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp892->f2=(void*)ts->hd;_tmp892;});
_tmp891->tl=inst;_tmp891;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp895;
_tmp895.tag=0;_tmp895.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp731));{void*_tmp893[1]={& _tmp895};Cyc_Tcutil_terr(loc,({const char*_tmp894="too many parameters for typedef %s";
_tag_arr(_tmp894,sizeof(char),_get_zero_arr_size(_tmp894,35));}),_tag_arr(
_tmp893,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp896=_cycalloc(
sizeof(*_tmp896));_tmp896->hd=(void*)e;_tmp896->tl=hidden_ts;_tmp896;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897->hd=({struct _tuple8*_tmp898=
_cycalloc(sizeof(*_tmp898));_tmp898->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp898->f2=
e;_tmp898;});_tmp897->tl=inst;_tmp897;});}*_tmp733=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp737=({void**_tmp899=
_cycalloc(sizeof(*_tmp899));_tmp899[0]=new_typ;_tmp899;});}goto _LL489;}}_LL4B0:
if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 18)goto _LL4B2;_LL4B1: goto _LL4B3;
_LL4B2: if((int)_tmp6F8 != 2)goto _LL4B4;_LL4B3: goto _LL489;_LL4B4: if(_tmp6F8 <= (
void*)3  || *((int*)_tmp6F8)!= 15)goto _LL4B6;_tmp738=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6F8)->f1;_LL4B5: _tmp739=_tmp738;goto _LL4B7;_LL4B6: if(_tmp6F8 <= (void*)3  || *((
int*)_tmp6F8)!= 19)goto _LL4B8;_tmp739=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6F8)->f1;_LL4B7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp739);goto _LL489;_LL4B8: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 21)goto
_LL4BA;_tmp73A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6F8)->f1;_LL4B9:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp73A);goto _LL489;
_LL4BA: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 20)goto _LL489;_tmp73B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6F8)->f1;_LL4BB: for(0;_tmp73B != 0;_tmp73B=
_tmp73B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp73B->hd);}goto _LL489;_LL489:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind))({struct Cyc_String_pa_struct _tmp89E;_tmp89E.tag=0;_tmp89E.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{
struct Cyc_String_pa_struct _tmp89D;_tmp89D.tag=0;_tmp89D.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct
_tmp89C;_tmp89C.tag=0;_tmp89C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp89A[3]={& _tmp89C,& _tmp89D,& _tmp89E};Cyc_Tcutil_terr(loc,({const
char*_tmp89B="type %s has kind %s but as used here needs kind %s";_tag_arr(
_tmp89B,sizeof(char),_get_zero_arr_size(_tmp89B,51));}),_tag_arr(_tmp89A,sizeof(
void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
kind_env,void*expected_kind,int generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp89F=Cyc_Tcutil_i_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8A8;
_tmp8A8.kind_env=kind_env;_tmp8A8.free_vars=0;_tmp8A8.free_evars=0;_tmp8A8.generalize_evars=
generalize_evars;_tmp8A8.fn_result=0;_tmp8A8;}),expected_kind,t);{struct Cyc_List_List*
vs=_tmp89F.free_vars;for(0;vs != 0;vs=vs->tl){_tmp89F.kind_env=Cyc_Tcutil_fast_add_free_tvar(
kind_env,(struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp89F.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp8A0=Cyc_Tcutil_compress((void*)evs->hd);
struct Cyc_Core_Opt*_tmp8A1;struct Cyc_Core_Opt**_tmp8A2;_LL55F: if(_tmp8A0 <= (void*)
3  || *((int*)_tmp8A0)!= 0)goto _LL561;_tmp8A1=((struct Cyc_Absyn_Evar_struct*)
_tmp8A0)->f4;_tmp8A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp8A0)->f4;_LL560: if(*_tmp8A2 == 0)*_tmp8A2=({struct Cyc_Core_Opt*_tmp8A3=
_cycalloc(sizeof(*_tmp8A3));_tmp8A3->v=kind_env;_tmp8A3;});else{struct Cyc_List_List*
_tmp8A4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp8A2))->v;
struct Cyc_List_List*_tmp8A5=0;for(0;_tmp8A4 != 0;_tmp8A4=_tmp8A4->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp8A4->hd))_tmp8A5=({struct Cyc_List_List*_tmp8A6=_cycalloc(
sizeof(*_tmp8A6));_tmp8A6->hd=(struct Cyc_Absyn_Tvar*)_tmp8A4->hd;_tmp8A6->tl=
_tmp8A5;_tmp8A6;});}*_tmp8A2=({struct Cyc_Core_Opt*_tmp8A7=_cycalloc(sizeof(*
_tmp8A7));_tmp8A7->v=_tmp8A5;_tmp8A7;});}goto _LL55E;_LL561:;_LL562: goto _LL55E;
_LL55E:;}}return _tmp89F;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp8A9=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp8AA=_tmp8A9.free_vars;struct Cyc_List_List*
_tmp8AB=_tmp8A9.free_evars;{struct Cyc_List_List*x=_tmp8AA;for(0;x != 0;x=x->tl){
void*_tmp8AC=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp8AD;struct Cyc_Core_Opt**_tmp8AE;struct Cyc_Core_Opt*
_tmp8AF;struct Cyc_Core_Opt**_tmp8B0;void*_tmp8B1;struct Cyc_Core_Opt*_tmp8B2;
struct Cyc_Core_Opt**_tmp8B3;void*_tmp8B4;struct Cyc_Core_Opt*_tmp8B5;struct Cyc_Core_Opt**
_tmp8B6;void*_tmp8B7;void*_tmp8B8;_LL564: if(*((int*)_tmp8AC)!= 1)goto _LL566;
_tmp8AD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AC)->f1;_tmp8AE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AC)->f1;_LL565: _tmp8B0=_tmp8AE;goto
_LL567;_LL566: if(*((int*)_tmp8AC)!= 2)goto _LL568;_tmp8AF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AC)->f1;_tmp8B0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AC)->f1;_tmp8B1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AC)->f2;if((
int)_tmp8B1 != 1)goto _LL568;_LL567: _tmp8B3=_tmp8B0;goto _LL569;_LL568: if(*((int*)
_tmp8AC)!= 2)goto _LL56A;_tmp8B2=((struct Cyc_Absyn_Less_kb_struct*)_tmp8AC)->f1;
_tmp8B3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8AC)->f1;
_tmp8B4=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AC)->f2;if((int)_tmp8B4 != 
0)goto _LL56A;_LL569:*_tmp8B3=({struct Cyc_Core_Opt*_tmp8B9=_cycalloc(sizeof(*
_tmp8B9));_tmp8B9->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BA=
_cycalloc(sizeof(*_tmp8BA));_tmp8BA[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8BB;
_tmp8BB.tag=0;_tmp8BB.f1=(void*)((void*)2);_tmp8BB;});_tmp8BA;}));_tmp8B9;});
goto _LL563;_LL56A: if(*((int*)_tmp8AC)!= 2)goto _LL56C;_tmp8B5=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AC)->f1;_tmp8B6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AC)->f1;_tmp8B7=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AC)->f2;
_LL56B:*_tmp8B6=({struct Cyc_Core_Opt*_tmp8BC=_cycalloc(sizeof(*_tmp8BC));_tmp8BC->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));
_tmp8BD[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8BE;_tmp8BE.tag=0;_tmp8BE.f1=(void*)
_tmp8B7;_tmp8BE;});_tmp8BD;}));_tmp8BC;});goto _LL563;_LL56C: if(*((int*)_tmp8AC)
!= 0)goto _LL56E;_tmp8B8=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8AC)->f1;if((
int)_tmp8B8 != 1)goto _LL56E;_LL56D:({struct Cyc_String_pa_struct _tmp8C1;_tmp8C1.tag=
0;_tmp8C1.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8BF[1]={& _tmp8C1};Cyc_Tcutil_terr(loc,({
const char*_tmp8C0="type variable %s cannot have kind M";_tag_arr(_tmp8C0,sizeof(
char),_get_zero_arr_size(_tmp8C0,36));}),_tag_arr(_tmp8BF,sizeof(void*),1));}});
goto _LL563;_LL56E:;_LL56F: goto _LL563;_LL563:;}}if(_tmp8AA != 0  || _tmp8AB != 0){{
void*_tmp8C2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8C3;struct Cyc_List_List*
_tmp8C4;struct Cyc_List_List**_tmp8C5;struct Cyc_Core_Opt*_tmp8C6;void*_tmp8C7;
struct Cyc_List_List*_tmp8C8;int _tmp8C9;struct Cyc_Absyn_VarargInfo*_tmp8CA;struct
Cyc_List_List*_tmp8CB;struct Cyc_List_List*_tmp8CC;_LL571: if(_tmp8C2 <= (void*)3
 || *((int*)_tmp8C2)!= 8)goto _LL573;_tmp8C3=((struct Cyc_Absyn_FnType_struct*)
_tmp8C2)->f1;_tmp8C4=_tmp8C3.tvars;_tmp8C5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp8C2)->f1).tvars;_tmp8C6=_tmp8C3.effect;_tmp8C7=(void*)_tmp8C3.ret_typ;
_tmp8C8=_tmp8C3.args;_tmp8C9=_tmp8C3.c_varargs;_tmp8CA=_tmp8C3.cyc_varargs;
_tmp8CB=_tmp8C3.rgn_po;_tmp8CC=_tmp8C3.attributes;_LL572: if(*_tmp8C5 == 0){*
_tmp8C5=_tmp8AA;_tmp8AA=0;}goto _LL570;_LL573:;_LL574: goto _LL570;_LL570:;}if(
_tmp8AA != 0)({struct Cyc_String_pa_struct _tmp8CF;_tmp8CF.tag=0;_tmp8CF.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp8AA->hd)->name);{
void*_tmp8CD[1]={& _tmp8CF};Cyc_Tcutil_terr(loc,({const char*_tmp8CE="unbound type variable %s";
_tag_arr(_tmp8CE,sizeof(char),_get_zero_arr_size(_tmp8CE,25));}),_tag_arr(
_tmp8CD,sizeof(void*),1));}});if(_tmp8AB != 0)for(0;_tmp8AB != 0;_tmp8AB=_tmp8AB->tl){
void*e=(void*)_tmp8AB->hd;void*_tmp8D0=Cyc_Tcutil_typ_kind(e);_LL576: if((int)
_tmp8D0 != 3)goto _LL578;_LL577: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8D1[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp8D2="can't unify evar with heap!";_tag_arr(_tmp8D2,sizeof(char),
_get_zero_arr_size(_tmp8D2,28));}),_tag_arr(_tmp8D1,sizeof(void*),0));});goto
_LL575;_LL578: if((int)_tmp8D0 != 4)goto _LL57A;_LL579: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8D3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D4="can't unify evar with {}!";_tag_arr(_tmp8D4,sizeof(char),
_get_zero_arr_size(_tmp8D4,26));}),_tag_arr(_tmp8D3,sizeof(void*),0));});goto
_LL575;_LL57A:;_LL57B:({struct Cyc_String_pa_struct _tmp8D8;_tmp8D8.tag=0;_tmp8D8.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8D7;_tmp8D7.tag=0;_tmp8D7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8D5[2]={& _tmp8D7,& _tmp8D8};Cyc_Tcutil_terr(loc,({const char*_tmp8D6="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8D6,sizeof(char),_get_zero_arr_size(_tmp8D6,52));}),_tag_arr(
_tmp8D5,sizeof(void*),2));}}});goto _LL575;_LL575:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp8D9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8DA;struct Cyc_List_List*
_tmp8DB;struct Cyc_Core_Opt*_tmp8DC;_LL57D: if(_tmp8D9 <= (void*)3  || *((int*)
_tmp8D9)!= 8)goto _LL57F;_tmp8DA=((struct Cyc_Absyn_FnType_struct*)_tmp8D9)->f1;
_tmp8DB=_tmp8DA.tvars;_tmp8DC=_tmp8DA.effect;_LL57E: fd->tvs=_tmp8DB;fd->effect=
_tmp8DC;goto _LL57C;_LL57F:;_LL580:({void*_tmp8DD[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8DE="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8DE,sizeof(char),_get_zero_arr_size(_tmp8DE,38));}),_tag_arr(
_tmp8DD,sizeof(void*),0));});_LL57C:;}{struct _RegionHandle _tmp8DF=_new_region("r");
struct _RegionHandle*r=& _tmp8DF;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8E0="function declaration has repeated parameter";
_tag_arr(_tmp8E0,sizeof(char),_get_zero_arr_size(_tmp8E0,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8E1=_cycalloc(sizeof(*_tmp8E1));_tmp8E1->v=(
void*)t;_tmp8E1;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8E2=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp8E3=Cyc_Tcutil_remove_bound_tvars(
_tmp8E2.free_vars,bound_tvars);struct Cyc_List_List*_tmp8E4=_tmp8E2.free_evars;{
struct Cyc_List_List*fs=_tmp8E3;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8E5=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8E9;_tmp8E9.tag=
0;_tmp8E9.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8E8;_tmp8E8.tag=0;_tmp8E8.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8E5);{void*_tmp8E6[2]={& _tmp8E8,& _tmp8E9};Cyc_Tcutil_terr(
loc,({const char*_tmp8E7="unbound type variable %s in type %s";_tag_arr(_tmp8E7,
sizeof(char),_get_zero_arr_size(_tmp8E7,36));}),_tag_arr(_tmp8E6,sizeof(void*),2));}}});}}
if(!allow_evars  && _tmp8E4 != 0)for(0;_tmp8E4 != 0;_tmp8E4=_tmp8E4->tl){void*e=(
void*)_tmp8E4->hd;void*_tmp8EA=Cyc_Tcutil_typ_kind(e);_LL582: if((int)_tmp8EA != 3)
goto _LL584;_LL583: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8EB[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8EC="can't unify evar with heap!";
_tag_arr(_tmp8EC,sizeof(char),_get_zero_arr_size(_tmp8EC,28));}),_tag_arr(
_tmp8EB,sizeof(void*),0));});goto _LL581;_LL584: if((int)_tmp8EA != 4)goto _LL586;
_LL585: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8ED[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8EE="can't unify evar with {}!";
_tag_arr(_tmp8EE,sizeof(char),_get_zero_arr_size(_tmp8EE,26));}),_tag_arr(
_tmp8ED,sizeof(void*),0));});goto _LL581;_LL586:;_LL587:({struct Cyc_String_pa_struct
_tmp8F2;_tmp8F2.tag=0;_tmp8F2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8F1;_tmp8F1.tag=0;_tmp8F1.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8EF[2]={&
_tmp8F1,& _tmp8F2};Cyc_Tcutil_terr(loc,({const char*_tmp8F0="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8F0,sizeof(char),_get_zero_arr_size(_tmp8F0,52));}),_tag_arr(
_tmp8EF,sizeof(void*),2));}}});goto _LL581;_LL581:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8F6;_tmp8F6.tag=0;_tmp8F6.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8F5;_tmp8F5.tag=0;_tmp8F5.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8F3[2]={& _tmp8F5,& _tmp8F6};
Cyc_Tcutil_terr(loc,({const char*_tmp8F4="%s: %s";_tag_arr(_tmp8F4,sizeof(char),
_get_zero_arr_size(_tmp8F4,7));}),_tag_arr(_tmp8F3,sizeof(void*),2));}}});}}}
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
const char*_tmp8F7="duplicate type variable";_tag_arr(_tmp8F7,sizeof(char),
_get_zero_arr_size(_tmp8F7,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8F8="";_tag_arr(_tmp8F8,sizeof(char),_get_zero_arr_size(_tmp8F8,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8F9=_cycalloc(sizeof(*_tmp8F9));_tmp8F9->hd=({
struct _tuple18*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp8FA->f2=0;_tmp8FA;});_tmp8F9->tl=fields;_tmp8F9;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp8FC;struct Cyc_List_List*
_tmp8FD;void*_tmp8FE;struct _tuple19*_tmp8FB=(struct _tuple19*)des->hd;_tmp8FC=*
_tmp8FB;_tmp8FD=_tmp8FC.f1;_tmp8FE=_tmp8FC.f2;if(_tmp8FD == 0){struct Cyc_List_List*
_tmp8FF=fields;for(0;_tmp8FF != 0;_tmp8FF=_tmp8FF->tl){if(!(*((struct _tuple18*)
_tmp8FF->hd)).f2){(*((struct _tuple18*)_tmp8FF->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp900=_cycalloc(sizeof(*_tmp900));
_tmp900->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp901=_cycalloc(
sizeof(*_tmp901));_tmp901[0]=({struct Cyc_Absyn_FieldName_struct _tmp902;_tmp902.tag=
1;_tmp902.f1=((*((struct _tuple18*)_tmp8FF->hd)).f1)->name;_tmp902;});_tmp901;}));
_tmp900->tl=0;_tmp900;});ans=({struct Cyc_List_List*_tmp903=_region_malloc(rgn,
sizeof(*_tmp903));_tmp903->hd=({struct _tuple20*_tmp904=_region_malloc(rgn,
sizeof(*_tmp904));_tmp904->f1=(*((struct _tuple18*)_tmp8FF->hd)).f1;_tmp904->f2=
_tmp8FE;_tmp904;});_tmp903->tl=ans;_tmp903;});break;}}if(_tmp8FF == 0)({void*
_tmp905[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp906="too many arguments to struct";
_tag_arr(_tmp906,sizeof(char),_get_zero_arr_size(_tmp906,29));}),_tag_arr(
_tmp905,sizeof(void*),0));});}else{if(_tmp8FD->tl != 0)({void*_tmp907[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp908="multiple designators are not supported";_tag_arr(
_tmp908,sizeof(char),_get_zero_arr_size(_tmp908,39));}),_tag_arr(_tmp907,sizeof(
void*),0));});else{void*_tmp909=(void*)_tmp8FD->hd;struct _tagged_arr*_tmp90A;
_LL589: if(*((int*)_tmp909)!= 0)goto _LL58B;_LL58A:({void*_tmp90B[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp90C="array designator used in argument to struct";_tag_arr(
_tmp90C,sizeof(char),_get_zero_arr_size(_tmp90C,44));}),_tag_arr(_tmp90B,sizeof(
void*),0));});goto _LL588;_LL58B: if(*((int*)_tmp909)!= 1)goto _LL588;_tmp90A=((
struct Cyc_Absyn_FieldName_struct*)_tmp909)->f1;_LL58C: {struct Cyc_List_List*
_tmp90D=fields;for(0;_tmp90D != 0;_tmp90D=_tmp90D->tl){if(Cyc_strptrcmp(_tmp90A,((*((
struct _tuple18*)_tmp90D->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp90D->hd)).f2)({
struct Cyc_String_pa_struct _tmp910;_tmp910.tag=0;_tmp910.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp90A);{void*_tmp90E[1]={& _tmp910};Cyc_Tcutil_terr(loc,({
const char*_tmp90F="field %s has already been used as an argument";_tag_arr(
_tmp90F,sizeof(char),_get_zero_arr_size(_tmp90F,46));}),_tag_arr(_tmp90E,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp90D->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp911=_region_malloc(rgn,sizeof(*_tmp911));_tmp911->hd=({struct _tuple20*
_tmp912=_region_malloc(rgn,sizeof(*_tmp912));_tmp912->f1=(*((struct _tuple18*)
_tmp90D->hd)).f1;_tmp912->f2=_tmp8FE;_tmp912;});_tmp911->tl=ans;_tmp911;});
break;}}if(_tmp90D == 0)({struct Cyc_String_pa_struct _tmp915;_tmp915.tag=0;_tmp915.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp90A);{void*_tmp913[1]={& _tmp915};Cyc_Tcutil_terr(
loc,({const char*_tmp914="bad field designator %s";_tag_arr(_tmp914,sizeof(char),
_get_zero_arr_size(_tmp914,24));}),_tag_arr(_tmp913,sizeof(void*),1));}});goto
_LL588;}_LL588:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp916[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp917="too few arguments to struct";
_tag_arr(_tmp917,sizeof(char),_get_zero_arr_size(_tmp917,28));}),_tag_arr(
_tmp916,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp918=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp919;
void*_tmp91A;struct Cyc_Absyn_PtrAtts _tmp91B;struct Cyc_Absyn_Conref*_tmp91C;
_LL58E: if(_tmp918 <= (void*)3  || *((int*)_tmp918)!= 4)goto _LL590;_tmp919=((struct
Cyc_Absyn_PointerType_struct*)_tmp918)->f1;_tmp91A=(void*)_tmp919.elt_typ;
_tmp91B=_tmp919.ptr_atts;_tmp91C=_tmp91B.bounds;_LL58F: {struct Cyc_Absyn_Conref*
_tmp91D=Cyc_Absyn_compress_conref(_tmp91C);void*_tmp91E=(void*)(Cyc_Absyn_compress_conref(
_tmp91D))->v;void*_tmp91F;_LL593: if(_tmp91E <= (void*)1  || *((int*)_tmp91E)!= 0)
goto _LL595;_tmp91F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp91E)->f1;if((
int)_tmp91F != 0)goto _LL595;_LL594:*elt_typ_dest=_tmp91A;return 1;_LL595: if((int)
_tmp91E != 0)goto _LL597;_LL596:(void*)(_tmp91D->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp920=_cycalloc(sizeof(*_tmp920));_tmp920[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp921;_tmp921.tag=0;_tmp921.f1=(void*)((void*)0);_tmp921;});_tmp920;})));*
elt_typ_dest=_tmp91A;return 1;_LL597:;_LL598: return 0;_LL592:;}_LL590:;_LL591:
return 0;_LL58D:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp922=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp923;void*_tmp924;
struct Cyc_Absyn_PtrAtts _tmp925;struct Cyc_Absyn_Conref*_tmp926;_LL59A: if(_tmp922
<= (void*)3  || *((int*)_tmp922)!= 4)goto _LL59C;_tmp923=((struct Cyc_Absyn_PointerType_struct*)
_tmp922)->f1;_tmp924=(void*)_tmp923.elt_typ;_tmp925=_tmp923.ptr_atts;_tmp926=
_tmp925.zero_term;_LL59B:*elt_typ_dest=_tmp924;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp926);_LL59C:;_LL59D: return 0;_LL599:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp96A;_tmp96A.f1=0;
_tmp96A.f2=(void*)2;_tmp96A;});void*_tmp927=(void*)e->r;struct _tuple1*_tmp928;
void*_tmp929;struct Cyc_Absyn_Exp*_tmp92A;struct _tagged_arr*_tmp92B;struct Cyc_Absyn_Exp*
_tmp92C;struct _tagged_arr*_tmp92D;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*
_tmp92F;struct Cyc_Absyn_Exp*_tmp930;_LL59F: if(*((int*)_tmp927)!= 1)goto _LL5A1;
_tmp928=((struct Cyc_Absyn_Var_e_struct*)_tmp927)->f1;_tmp929=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp927)->f2;_LL5A0: {void*_tmp931=_tmp929;struct Cyc_Absyn_Vardecl*_tmp932;
struct Cyc_Absyn_Vardecl*_tmp933;struct Cyc_Absyn_Vardecl*_tmp934;struct Cyc_Absyn_Vardecl*
_tmp935;_LL5AC: if((int)_tmp931 != 0)goto _LL5AE;_LL5AD: goto _LL5AF;_LL5AE: if(
_tmp931 <= (void*)1  || *((int*)_tmp931)!= 1)goto _LL5B0;_LL5AF: return bogus_ans;
_LL5B0: if(_tmp931 <= (void*)1  || *((int*)_tmp931)!= 0)goto _LL5B2;_tmp932=((struct
Cyc_Absyn_Global_b_struct*)_tmp931)->f1;_LL5B1: {void*_tmp936=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5B9: if(_tmp936 <= (void*)
3  || *((int*)_tmp936)!= 7)goto _LL5BB;_LL5BA: return({struct _tuple10 _tmp937;
_tmp937.f1=1;_tmp937.f2=(void*)2;_tmp937;});_LL5BB:;_LL5BC: return({struct
_tuple10 _tmp938;_tmp938.f1=(_tmp932->tq).real_const;_tmp938.f2=(void*)2;_tmp938;});
_LL5B8:;}_LL5B2: if(_tmp931 <= (void*)1  || *((int*)_tmp931)!= 3)goto _LL5B4;_tmp933=((
struct Cyc_Absyn_Local_b_struct*)_tmp931)->f1;_LL5B3: {void*_tmp939=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5BE: if(_tmp939 <= (void*)
3  || *((int*)_tmp939)!= 7)goto _LL5C0;_LL5BF: return({struct _tuple10 _tmp93A;
_tmp93A.f1=1;_tmp93A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp933->rgn))->v;
_tmp93A;});_LL5C0:;_LL5C1: _tmp933->escapes=1;return({struct _tuple10 _tmp93B;
_tmp93B.f1=(_tmp933->tq).real_const;_tmp93B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp933->rgn))->v;_tmp93B;});_LL5BD:;}_LL5B4: if(_tmp931 <= (void*)1
 || *((int*)_tmp931)!= 4)goto _LL5B6;_tmp934=((struct Cyc_Absyn_Pat_b_struct*)
_tmp931)->f1;_LL5B5: _tmp935=_tmp934;goto _LL5B7;_LL5B6: if(_tmp931 <= (void*)1  || *((
int*)_tmp931)!= 2)goto _LL5AB;_tmp935=((struct Cyc_Absyn_Param_b_struct*)_tmp931)->f1;
_LL5B7: _tmp935->escapes=1;return({struct _tuple10 _tmp93C;_tmp93C.f1=(_tmp935->tq).real_const;
_tmp93C.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp935->rgn))->v;_tmp93C;});
_LL5AB:;}_LL5A1: if(*((int*)_tmp927)!= 23)goto _LL5A3;_tmp92A=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp927)->f1;_tmp92B=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp927)->f2;_LL5A2: {
void*_tmp93D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp92A->topt))->v);struct Cyc_List_List*_tmp93E;struct Cyc_Absyn_AggrInfo _tmp93F;
void*_tmp940;struct Cyc_Absyn_Aggrdecl**_tmp941;struct Cyc_Absyn_Aggrdecl*_tmp942;
_LL5C3: if(_tmp93D <= (void*)3  || *((int*)_tmp93D)!= 11)goto _LL5C5;_tmp93E=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp93D)->f2;_LL5C4: {struct Cyc_Absyn_Aggrfield*
_tmp943=Cyc_Absyn_lookup_field(_tmp93E,_tmp92B);if(_tmp943 != 0  && _tmp943->width
!= 0)return({struct _tuple10 _tmp944;_tmp944.f1=(_tmp943->tq).real_const;_tmp944.f2=(
Cyc_Tcutil_addressof_props(te,_tmp92A)).f2;_tmp944;});return bogus_ans;}_LL5C5:
if(_tmp93D <= (void*)3  || *((int*)_tmp93D)!= 10)goto _LL5C7;_tmp93F=((struct Cyc_Absyn_AggrType_struct*)
_tmp93D)->f1;_tmp940=(void*)_tmp93F.aggr_info;if(*((int*)_tmp940)!= 1)goto _LL5C7;
_tmp941=((struct Cyc_Absyn_KnownAggr_struct*)_tmp940)->f1;_tmp942=*_tmp941;_LL5C6: {
struct Cyc_Absyn_Aggrfield*_tmp945=Cyc_Absyn_lookup_decl_field(_tmp942,_tmp92B);
if(_tmp945 != 0  && _tmp945->width != 0)return({struct _tuple10 _tmp946;_tmp946.f1=(
_tmp945->tq).real_const;_tmp946.f2=(Cyc_Tcutil_addressof_props(te,_tmp92A)).f2;
_tmp946;});return bogus_ans;}_LL5C7:;_LL5C8: return bogus_ans;_LL5C2:;}_LL5A3: if(*((
int*)_tmp927)!= 24)goto _LL5A5;_tmp92C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp927)->f1;_tmp92D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp927)->f2;_LL5A4: {
void*_tmp947=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp92C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp948;void*_tmp949;struct Cyc_Absyn_PtrAtts
_tmp94A;void*_tmp94B;_LL5CA: if(_tmp947 <= (void*)3  || *((int*)_tmp947)!= 4)goto
_LL5CC;_tmp948=((struct Cyc_Absyn_PointerType_struct*)_tmp947)->f1;_tmp949=(void*)
_tmp948.elt_typ;_tmp94A=_tmp948.ptr_atts;_tmp94B=(void*)_tmp94A.rgn;_LL5CB: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp94C=Cyc_Tcutil_compress(_tmp949);
struct Cyc_List_List*_tmp94D;struct Cyc_Absyn_AggrInfo _tmp94E;void*_tmp94F;struct
Cyc_Absyn_Aggrdecl**_tmp950;struct Cyc_Absyn_Aggrdecl*_tmp951;_LL5CF: if(_tmp94C <= (
void*)3  || *((int*)_tmp94C)!= 11)goto _LL5D1;_tmp94D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp94C)->f2;_LL5D0: finfo=Cyc_Absyn_lookup_field(_tmp94D,_tmp92D);goto _LL5CE;
_LL5D1: if(_tmp94C <= (void*)3  || *((int*)_tmp94C)!= 10)goto _LL5D3;_tmp94E=((
struct Cyc_Absyn_AggrType_struct*)_tmp94C)->f1;_tmp94F=(void*)_tmp94E.aggr_info;
if(*((int*)_tmp94F)!= 1)goto _LL5D3;_tmp950=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp94F)->f1;_tmp951=*_tmp950;_LL5D2: finfo=Cyc_Absyn_lookup_decl_field(_tmp951,
_tmp92D);goto _LL5CE;_LL5D3:;_LL5D4: return bogus_ans;_LL5CE:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp952;_tmp952.f1=(finfo->tq).real_const;
_tmp952.f2=_tmp94B;_tmp952;});return bogus_ans;}_LL5CC:;_LL5CD: return bogus_ans;
_LL5C9:;}_LL5A5: if(*((int*)_tmp927)!= 22)goto _LL5A7;_tmp92E=((struct Cyc_Absyn_Deref_e_struct*)
_tmp927)->f1;_LL5A6: {void*_tmp953=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp92E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp954;struct Cyc_Absyn_Tqual
_tmp955;struct Cyc_Absyn_PtrAtts _tmp956;void*_tmp957;_LL5D6: if(_tmp953 <= (void*)3
 || *((int*)_tmp953)!= 4)goto _LL5D8;_tmp954=((struct Cyc_Absyn_PointerType_struct*)
_tmp953)->f1;_tmp955=_tmp954.elt_tq;_tmp956=_tmp954.ptr_atts;_tmp957=(void*)
_tmp956.rgn;_LL5D7: return({struct _tuple10 _tmp958;_tmp958.f1=_tmp955.real_const;
_tmp958.f2=_tmp957;_tmp958;});_LL5D8:;_LL5D9: return bogus_ans;_LL5D5:;}_LL5A7: if(*((
int*)_tmp927)!= 25)goto _LL5A9;_tmp92F=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp927)->f1;_tmp930=((struct Cyc_Absyn_Subscript_e_struct*)_tmp927)->f2;_LL5A8: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp92F->topt))->v);
void*_tmp959=t;struct Cyc_List_List*_tmp95A;struct Cyc_Absyn_PtrInfo _tmp95B;struct
Cyc_Absyn_Tqual _tmp95C;struct Cyc_Absyn_PtrAtts _tmp95D;void*_tmp95E;struct Cyc_Absyn_ArrayInfo
_tmp95F;struct Cyc_Absyn_Tqual _tmp960;_LL5DB: if(_tmp959 <= (void*)3  || *((int*)
_tmp959)!= 9)goto _LL5DD;_tmp95A=((struct Cyc_Absyn_TupleType_struct*)_tmp959)->f1;
_LL5DC: {unsigned int _tmp962;int _tmp963;struct _tuple7 _tmp961=Cyc_Evexp_eval_const_uint_exp(
_tmp930);_tmp962=_tmp961.f1;_tmp963=_tmp961.f2;if(!_tmp963)return bogus_ans;{
struct _tuple4*_tmp964=Cyc_Absyn_lookup_tuple_field(_tmp95A,(int)_tmp962);if(
_tmp964 != 0)return({struct _tuple10 _tmp965;_tmp965.f1=((*_tmp964).f1).real_const;
_tmp965.f2=(Cyc_Tcutil_addressof_props(te,_tmp92F)).f2;_tmp965;});return
bogus_ans;}}_LL5DD: if(_tmp959 <= (void*)3  || *((int*)_tmp959)!= 4)goto _LL5DF;
_tmp95B=((struct Cyc_Absyn_PointerType_struct*)_tmp959)->f1;_tmp95C=_tmp95B.elt_tq;
_tmp95D=_tmp95B.ptr_atts;_tmp95E=(void*)_tmp95D.rgn;_LL5DE: return({struct
_tuple10 _tmp966;_tmp966.f1=_tmp95C.real_const;_tmp966.f2=_tmp95E;_tmp966;});
_LL5DF: if(_tmp959 <= (void*)3  || *((int*)_tmp959)!= 7)goto _LL5E1;_tmp95F=((struct
Cyc_Absyn_ArrayType_struct*)_tmp959)->f1;_tmp960=_tmp95F.tq;_LL5E0: return({
struct _tuple10 _tmp967;_tmp967.f1=_tmp960.real_const;_tmp967.f2=(Cyc_Tcutil_addressof_props(
te,_tmp92F)).f2;_tmp967;});_LL5E1:;_LL5E2: return bogus_ans;_LL5DA:;}_LL5A9:;
_LL5AA:({void*_tmp968[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp969="unary & applied to non-lvalue";
_tag_arr(_tmp969,sizeof(char),_get_zero_arr_size(_tmp969,30));}),_tag_arr(
_tmp968,sizeof(void*),0));});return bogus_ans;_LL59E:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp96B=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp96C;void*_tmp96D;struct Cyc_Absyn_Tqual
_tmp96E;struct Cyc_Absyn_Conref*_tmp96F;_LL5E4: if(_tmp96B <= (void*)3  || *((int*)
_tmp96B)!= 7)goto _LL5E6;_tmp96C=((struct Cyc_Absyn_ArrayType_struct*)_tmp96B)->f1;
_tmp96D=(void*)_tmp96C.elt_type;_tmp96E=_tmp96C.tq;_tmp96F=_tmp96C.zero_term;
_LL5E5: {void*_tmp971;struct _tuple10 _tmp970=Cyc_Tcutil_addressof_props(te,e);
_tmp971=_tmp970.f2;return Cyc_Absyn_atb_typ(_tmp96D,_tmp971,_tmp96E,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp972=_cycalloc(sizeof(*_tmp972));_tmp972[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp973;_tmp973.tag=0;_tmp973.f1=e;_tmp973;});
_tmp972;}),_tmp96F);}_LL5E6:;_LL5E7: return e_typ;_LL5E3:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp974=(void*)b->v;void*_tmp975;void*_tmp976;struct Cyc_Absyn_Exp*
_tmp977;void*_tmp978;_LL5E9: if(_tmp974 <= (void*)1  || *((int*)_tmp974)!= 0)goto
_LL5EB;_tmp975=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp974)->f1;if((int)
_tmp975 != 0)goto _LL5EB;_LL5EA: return;_LL5EB: if(_tmp974 <= (void*)1  || *((int*)
_tmp974)!= 0)goto _LL5ED;_tmp976=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp974)->f1;if(_tmp976 <= (void*)1  || *((int*)_tmp976)!= 0)goto _LL5ED;_tmp977=((
struct Cyc_Absyn_Upper_b_struct*)_tmp976)->f1;_LL5EC: {unsigned int _tmp97A;int
_tmp97B;struct _tuple7 _tmp979=Cyc_Evexp_eval_const_uint_exp(_tmp977);_tmp97A=
_tmp979.f1;_tmp97B=_tmp979.f2;if(_tmp97B  && _tmp97A <= i)({struct Cyc_Int_pa_struct
_tmp97F;_tmp97F.tag=1;_tmp97F.f1=(unsigned int)((int)i);{struct Cyc_Int_pa_struct
_tmp97E;_tmp97E.tag=1;_tmp97E.f1=(unsigned int)((int)_tmp97A);{void*_tmp97C[2]={&
_tmp97E,& _tmp97F};Cyc_Tcutil_terr(loc,({const char*_tmp97D="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp97D,sizeof(char),_get_zero_arr_size(_tmp97D,39));}),_tag_arr(
_tmp97C,sizeof(void*),2));}}});return;}_LL5ED: if(_tmp974 <= (void*)1  || *((int*)
_tmp974)!= 0)goto _LL5EF;_tmp978=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp974)->f1;if(_tmp978 <= (void*)1  || *((int*)_tmp978)!= 1)goto _LL5EF;_LL5EE:
return;_LL5EF: if((int)_tmp974 != 0)goto _LL5F1;_LL5F0:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp980=_cycalloc(sizeof(*_tmp980));_tmp980[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp981;_tmp981.tag=0;_tmp981.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp982=_cycalloc(sizeof(*_tmp982));
_tmp982[0]=({struct Cyc_Absyn_Upper_b_struct _tmp983;_tmp983.tag=0;_tmp983.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp983;});_tmp982;})));_tmp981;});_tmp980;})));return;_LL5F1:;_LL5F2:({
void*_tmp984[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp985="check_bound -- bad compressed conref";_tag_arr(_tmp985,
sizeof(char),_get_zero_arr_size(_tmp985,37));}),_tag_arr(_tmp984,sizeof(void*),0));});
_LL5E8:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp986=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp987;struct Cyc_Absyn_Exp*_tmp988;_LL5F4: if(_tmp986 <= (void*)1  || *((int*)
_tmp986)!= 0)goto _LL5F6;_tmp987=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp986)->f1;if(_tmp987 <= (void*)1  || *((int*)_tmp987)!= 0)goto _LL5F6;_tmp988=((
struct Cyc_Absyn_Upper_b_struct*)_tmp987)->f1;_LL5F5: {unsigned int _tmp98A;int
_tmp98B;struct _tuple7 _tmp989=Cyc_Evexp_eval_const_uint_exp(_tmp988);_tmp98A=
_tmp989.f1;_tmp98B=_tmp989.f2;return _tmp98B  && _tmp98A == 1;}_LL5F6:;_LL5F7:
return 0;_LL5F3:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp98C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp98D;void*_tmp98E;struct Cyc_Absyn_Conref*_tmp98F;
struct Cyc_List_List*_tmp990;struct Cyc_Absyn_AggrInfo _tmp991;void*_tmp992;struct
Cyc_Absyn_AggrInfo _tmp993;void*_tmp994;struct Cyc_Absyn_Aggrdecl**_tmp995;struct
Cyc_Absyn_Aggrdecl*_tmp996;struct Cyc_List_List*_tmp997;struct Cyc_List_List*
_tmp998;_LL5F9: if((int)_tmp98C != 0)goto _LL5FB;_LL5FA: goto _LL5FC;_LL5FB: if(
_tmp98C <= (void*)3  || *((int*)_tmp98C)!= 5)goto _LL5FD;_LL5FC: goto _LL5FE;_LL5FD:
if((int)_tmp98C != 1)goto _LL5FF;_LL5FE: goto _LL600;_LL5FF: if(_tmp98C <= (void*)3
 || *((int*)_tmp98C)!= 6)goto _LL601;_LL600: return 1;_LL601: if(_tmp98C <= (void*)3
 || *((int*)_tmp98C)!= 12)goto _LL603;_LL602: goto _LL604;_LL603: if(_tmp98C <= (void*)
3  || *((int*)_tmp98C)!= 13)goto _LL605;_LL604: return 0;_LL605: if(_tmp98C <= (void*)
3  || *((int*)_tmp98C)!= 7)goto _LL607;_tmp98D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp98C)->f1;_tmp98E=(void*)_tmp98D.elt_type;_tmp98F=_tmp98D.zero_term;_LL606:
return !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp98F)
 && Cyc_Tcutil_bits_only(_tmp98E);_LL607: if(_tmp98C <= (void*)3  || *((int*)
_tmp98C)!= 9)goto _LL609;_tmp990=((struct Cyc_Absyn_TupleType_struct*)_tmp98C)->f1;
_LL608: for(0;_tmp990 != 0;_tmp990=_tmp990->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp990->hd)).f2))return 0;}return 1;_LL609: if(_tmp98C <= (void*)3
 || *((int*)_tmp98C)!= 10)goto _LL60B;_tmp991=((struct Cyc_Absyn_AggrType_struct*)
_tmp98C)->f1;_tmp992=(void*)_tmp991.aggr_info;if(*((int*)_tmp992)!= 0)goto _LL60B;
_LL60A: return 0;_LL60B: if(_tmp98C <= (void*)3  || *((int*)_tmp98C)!= 10)goto _LL60D;
_tmp993=((struct Cyc_Absyn_AggrType_struct*)_tmp98C)->f1;_tmp994=(void*)_tmp993.aggr_info;
if(*((int*)_tmp994)!= 1)goto _LL60D;_tmp995=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp994)->f1;_tmp996=*_tmp995;_tmp997=_tmp993.targs;_LL60C: if(_tmp996->impl == 0)
return 0;{struct _RegionHandle _tmp999=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp999;_push_region(rgn);{struct Cyc_List_List*_tmp99A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp996->tvs,_tmp997);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp996->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp99A,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp99B=0;_npop_handler(0);return _tmp99B;}}}{int _tmp99C=1;
_npop_handler(0);return _tmp99C;}};_pop_region(rgn);}_LL60D: if(_tmp98C <= (void*)3
 || *((int*)_tmp98C)!= 11)goto _LL60F;_tmp998=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp98C)->f2;_LL60E: for(0;_tmp998 != 0;_tmp998=_tmp998->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp998->hd)->type))return 0;}return 1;_LL60F:;
_LL610: return 0;_LL5F8:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp99D=(void*)e->r;struct _tuple1*_tmp99E;void*_tmp99F;struct Cyc_Absyn_Exp*
_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*
_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*
_tmp9A6;void*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A8;void*_tmp9A9;void*_tmp9AA;
struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*
_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_List_List*_tmp9AF;struct Cyc_List_List*
_tmp9B0;struct Cyc_List_List*_tmp9B1;void*_tmp9B2;struct Cyc_List_List*_tmp9B3;
struct Cyc_List_List*_tmp9B4;struct Cyc_List_List*_tmp9B5;_LL612: if(*((int*)
_tmp99D)!= 0)goto _LL614;_LL613: goto _LL615;_LL614: if(*((int*)_tmp99D)!= 18)goto
_LL616;_LL615: goto _LL617;_LL616: if(*((int*)_tmp99D)!= 19)goto _LL618;_LL617: goto
_LL619;_LL618: if(*((int*)_tmp99D)!= 20)goto _LL61A;_LL619: goto _LL61B;_LL61A: if(*((
int*)_tmp99D)!= 21)goto _LL61C;_LL61B: goto _LL61D;_LL61C: if(*((int*)_tmp99D)!= 33)
goto _LL61E;_LL61D: goto _LL61F;_LL61E: if(*((int*)_tmp99D)!= 34)goto _LL620;_LL61F:
return 1;_LL620: if(*((int*)_tmp99D)!= 1)goto _LL622;_tmp99E=((struct Cyc_Absyn_Var_e_struct*)
_tmp99D)->f1;_tmp99F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp99D)->f2;_LL621: {
void*_tmp9B6=_tmp99F;struct Cyc_Absyn_Vardecl*_tmp9B7;_LL641: if(_tmp9B6 <= (void*)
1  || *((int*)_tmp9B6)!= 1)goto _LL643;_LL642: return 1;_LL643: if(_tmp9B6 <= (void*)1
 || *((int*)_tmp9B6)!= 0)goto _LL645;_tmp9B7=((struct Cyc_Absyn_Global_b_struct*)
_tmp9B6)->f1;_LL644: {void*_tmp9B8=Cyc_Tcutil_compress((void*)_tmp9B7->type);
_LL64A: if(_tmp9B8 <= (void*)3  || *((int*)_tmp9B8)!= 7)goto _LL64C;_LL64B: goto
_LL64D;_LL64C: if(_tmp9B8 <= (void*)3  || *((int*)_tmp9B8)!= 8)goto _LL64E;_LL64D:
return 1;_LL64E:;_LL64F: return var_okay;_LL649:;}_LL645: if((int)_tmp9B6 != 0)goto
_LL647;_LL646: return 0;_LL647:;_LL648: return var_okay;_LL640:;}_LL622: if(*((int*)
_tmp99D)!= 6)goto _LL624;_tmp9A0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99D)->f1;
_tmp9A1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99D)->f2;_tmp9A2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp99D)->f3;_LL623: return(Cyc_Tcutil_cnst_exp(te,0,_tmp9A0) && Cyc_Tcutil_cnst_exp(
te,0,_tmp9A1)) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A2);_LL624: if(*((int*)_tmp99D)!= 
9)goto _LL626;_tmp9A3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp99D)->f1;_tmp9A4=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp99D)->f2;_LL625: return Cyc_Tcutil_cnst_exp(
te,0,_tmp9A3) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A4);_LL626: if(*((int*)_tmp99D)!= 
13)goto _LL628;_tmp9A5=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp99D)->f1;
_LL627: _tmp9A6=_tmp9A5;goto _LL629;_LL628: if(*((int*)_tmp99D)!= 14)goto _LL62A;
_tmp9A6=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp99D)->f1;_LL629: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp9A6);_LL62A: if(*((int*)_tmp99D)!= 15)goto _LL62C;_tmp9A7=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp99D)->f1;_tmp9A8=((struct Cyc_Absyn_Cast_e_struct*)
_tmp99D)->f2;_tmp9A9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp99D)->f4;if((
int)_tmp9A9 != 1)goto _LL62C;_LL62B: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9A8);
_LL62C: if(*((int*)_tmp99D)!= 15)goto _LL62E;_tmp9AA=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp99D)->f1;_tmp9AB=((struct Cyc_Absyn_Cast_e_struct*)_tmp99D)->f2;_LL62D: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9AB);_LL62E: if(*((int*)_tmp99D)!= 16)goto
_LL630;_tmp9AC=((struct Cyc_Absyn_Address_e_struct*)_tmp99D)->f1;_LL62F: return Cyc_Tcutil_cnst_exp(
te,1,_tmp9AC);_LL630: if(*((int*)_tmp99D)!= 29)goto _LL632;_tmp9AD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp99D)->f2;_tmp9AE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp99D)->f3;
_LL631: return Cyc_Tcutil_cnst_exp(te,0,_tmp9AD) && Cyc_Tcutil_cnst_exp(te,0,
_tmp9AE);_LL632: if(*((int*)_tmp99D)!= 28)goto _LL634;_tmp9AF=((struct Cyc_Absyn_Array_e_struct*)
_tmp99D)->f1;_LL633: _tmp9B0=_tmp9AF;goto _LL635;_LL634: if(*((int*)_tmp99D)!= 31)
goto _LL636;_tmp9B0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp99D)->f2;_LL635:
_tmp9B1=_tmp9B0;goto _LL637;_LL636: if(*((int*)_tmp99D)!= 30)goto _LL638;_tmp9B1=((
struct Cyc_Absyn_Struct_e_struct*)_tmp99D)->f3;_LL637: for(0;_tmp9B1 != 0;_tmp9B1=
_tmp9B1->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp9B1->hd)).f2))
return 0;}return 1;_LL638: if(*((int*)_tmp99D)!= 3)goto _LL63A;_tmp9B2=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp99D)->f1;_tmp9B3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp99D)->f2;_LL639: _tmp9B4=_tmp9B3;goto _LL63B;_LL63A: if(*((int*)_tmp99D)!= 26)
goto _LL63C;_tmp9B4=((struct Cyc_Absyn_Tuple_e_struct*)_tmp99D)->f1;_LL63B: _tmp9B5=
_tmp9B4;goto _LL63D;_LL63C: if(*((int*)_tmp99D)!= 32)goto _LL63E;_tmp9B5=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp99D)->f1;_LL63D: for(0;_tmp9B5 != 0;_tmp9B5=_tmp9B5->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9B5->hd))return 0;}return 1;
_LL63E:;_LL63F: return 0;_LL611:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9B9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9BA;
struct Cyc_Absyn_PtrAtts _tmp9BB;struct Cyc_Absyn_Conref*_tmp9BC;struct Cyc_Absyn_Conref*
_tmp9BD;struct Cyc_Absyn_ArrayInfo _tmp9BE;void*_tmp9BF;struct Cyc_List_List*
_tmp9C0;struct Cyc_Absyn_AggrInfo _tmp9C1;void*_tmp9C2;struct Cyc_List_List*_tmp9C3;
struct Cyc_List_List*_tmp9C4;_LL651: if((int)_tmp9B9 != 0)goto _LL653;_LL652: goto
_LL654;_LL653: if(_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 5)goto _LL655;_LL654:
goto _LL656;_LL655: if((int)_tmp9B9 != 1)goto _LL657;_LL656: goto _LL658;_LL657: if(
_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 6)goto _LL659;_LL658: return 1;_LL659: if(
_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 4)goto _LL65B;_tmp9BA=((struct Cyc_Absyn_PointerType_struct*)
_tmp9B9)->f1;_tmp9BB=_tmp9BA.ptr_atts;_tmp9BC=_tmp9BB.nullable;_tmp9BD=_tmp9BB.bounds;
_LL65A: {void*_tmp9C5=Cyc_Absyn_conref_def((void*)((void*)0),_tmp9BD);_LL66A: if((
int)_tmp9C5 != 0)goto _LL66C;_LL66B: return 1;_LL66C:;_LL66D: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp9BC);_LL669:;}_LL65B: if(
_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 7)goto _LL65D;_tmp9BE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9B9)->f1;_tmp9BF=(void*)_tmp9BE.elt_type;_LL65C: return Cyc_Tcutil_supports_default(
_tmp9BF);_LL65D: if(_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 9)goto _LL65F;_tmp9C0=((
struct Cyc_Absyn_TupleType_struct*)_tmp9B9)->f1;_LL65E: for(0;_tmp9C0 != 0;_tmp9C0=
_tmp9C0->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp9C0->hd)).f2))
return 0;}return 1;_LL65F: if(_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 10)goto
_LL661;_tmp9C1=((struct Cyc_Absyn_AggrType_struct*)_tmp9B9)->f1;_tmp9C2=(void*)
_tmp9C1.aggr_info;_tmp9C3=_tmp9C1.targs;_LL660: {struct Cyc_Absyn_Aggrdecl*
_tmp9C6=Cyc_Absyn_get_known_aggrdecl(_tmp9C2);if(_tmp9C6->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C6->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp9C6->tvs,_tmp9C3,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9C6->impl))->fields);}_LL661: if(_tmp9B9 <= (void*)3  || *((int*)
_tmp9B9)!= 11)goto _LL663;_tmp9C4=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9B9)->f2;
_LL662: return Cyc_Tcutil_fields_support_default(0,0,_tmp9C4);_LL663: if(_tmp9B9 <= (
void*)3  || *((int*)_tmp9B9)!= 13)goto _LL665;_LL664: goto _LL666;_LL665: if(_tmp9B9
<= (void*)3  || *((int*)_tmp9B9)!= 12)goto _LL667;_LL666: return 1;_LL667:;_LL668:
return 0;_LL650:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*
tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp9C7=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C7;_push_region(rgn);{struct Cyc_List_List*
_tmp9C8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp9C8,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp9C9=0;_npop_handler(0);
return _tmp9C9;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9CA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9CB;struct Cyc_Absyn_PtrAtts
_tmp9CC;struct Cyc_Absyn_Conref*_tmp9CD;struct Cyc_Absyn_Conref*_tmp9CE;_LL66F: if(
_tmp9CA <= (void*)3  || *((int*)_tmp9CA)!= 5)goto _LL671;_LL670: goto _LL672;_LL671:
if((int)_tmp9CA != 1)goto _LL673;_LL672: goto _LL674;_LL673: if(_tmp9CA <= (void*)3
 || *((int*)_tmp9CA)!= 6)goto _LL675;_LL674: return 1;_LL675: if(_tmp9CA <= (void*)3
 || *((int*)_tmp9CA)!= 4)goto _LL677;_tmp9CB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9CA)->f1;_tmp9CC=_tmp9CB.ptr_atts;_tmp9CD=_tmp9CC.nullable;_tmp9CE=_tmp9CC.bounds;
_LL676: {void*_tmp9CF=(void*)(Cyc_Absyn_compress_conref(_tmp9CE))->v;void*
_tmp9D0;_LL67A: if(_tmp9CF <= (void*)1  || *((int*)_tmp9CF)!= 0)goto _LL67C;_tmp9D0=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9CF)->f1;if((int)_tmp9D0 != 0)goto
_LL67C;_LL67B: return 0;_LL67C: if(_tmp9CF <= (void*)1  || *((int*)_tmp9CF)!= 0)goto
_LL67E;_LL67D: {void*_tmp9D1=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9CD))->v;int _tmp9D2;_LL681: if(_tmp9D1 <= (void*)
1  || *((int*)_tmp9D1)!= 0)goto _LL683;_tmp9D2=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D1)->f1;_LL682: return _tmp9D2;_LL683:;_LL684: return 0;_LL680:;}_LL67E:;_LL67F:
return 0;_LL679:;}_LL677:;_LL678: return 0;_LL66E:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9D4;void*
_tmp9D5;struct Cyc_Absyn_FnInfo _tmp9D6;struct Cyc_List_List*_tmp9D7;_LL686: if(
_tmp9D3 <= (void*)3  || *((int*)_tmp9D3)!= 4)goto _LL688;_tmp9D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D3)->f1;_tmp9D5=(void*)_tmp9D4.elt_typ;_LL687: return Cyc_Tcutil_is_noreturn(
_tmp9D5);_LL688: if(_tmp9D3 <= (void*)3  || *((int*)_tmp9D3)!= 8)goto _LL68A;_tmp9D6=((
struct Cyc_Absyn_FnType_struct*)_tmp9D3)->f1;_tmp9D7=_tmp9D6.attributes;_LL689:
for(0;_tmp9D7 != 0;_tmp9D7=_tmp9D7->tl){void*_tmp9D8=(void*)_tmp9D7->hd;_LL68D:
if((int)_tmp9D8 != 3)goto _LL68F;_LL68E: return 1;_LL68F:;_LL690: continue;_LL68C:;}
goto _LL685;_LL68A:;_LL68B: goto _LL685;_LL685:;}return 0;}
