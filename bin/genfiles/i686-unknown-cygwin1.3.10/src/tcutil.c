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
int tag;void*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;
struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_RgnHandleType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
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
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct
Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
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
_tmp72;struct Cyc_Absyn_Conref*_tmp73;struct Cyc_Absyn_Conref*_tmp74;struct Cyc_Absyn_PtrLoc*
_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_Absyn_ArrayInfo _tmp79;void*
_tmp7A;struct Cyc_Absyn_Tqual _tmp7B;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Conref*
_tmp7D;struct Cyc_Position_Segment*_tmp7E;struct Cyc_Absyn_FnInfo _tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_Core_Opt*_tmp81;void*_tmp82;struct Cyc_List_List*_tmp83;int
_tmp84;struct Cyc_Absyn_VarargInfo*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*
_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_AggrInfo _tmp89;void*_tmp8A;
void*_tmp8B;struct _tuple1*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_AggrInfo
_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrdecl**_tmp90;struct Cyc_List_List*_tmp91;
void*_tmp92;struct Cyc_List_List*_tmp93;struct _tuple1*_tmp94;struct Cyc_Absyn_Enumdecl*
_tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;int _tmp98;void*_tmp99;void*_tmp9A;
struct _tuple1*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Typedefdecl*
_tmp9D;void*_tmp9E;struct Cyc_List_List*_tmp9F;void*_tmpA0;_LL1A: if((int)_tmp64 != 
0)goto _LL1C;_LL1B: goto _LL1D;_LL1C: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 0)
goto _LL1E;_LL1D: return t;_LL1E: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 1)goto
_LL20;_tmp65=((struct Cyc_Absyn_VarType_struct*)_tmp64)->f1;_LL1F: return(void*)({
struct Cyc_Absyn_VarType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_VarType_struct _tmpA2;_tmpA2.tag=1;_tmpA2.f1=Cyc_Tcutil_copy_tvar(
_tmp65);_tmpA2;});_tmpA1;});_LL20: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 2)
goto _LL22;_tmp66=((struct Cyc_Absyn_TunionType_struct*)_tmp64)->f1;_tmp67=(void*)
_tmp66.tunion_info;_tmp68=_tmp66.targs;_tmp69=(void*)_tmp66.rgn;_LL21: return(
void*)({struct Cyc_Absyn_TunionType_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3[0]=({struct Cyc_Absyn_TunionType_struct _tmpA4;_tmpA4.tag=2;_tmpA4.f1=({
struct Cyc_Absyn_TunionInfo _tmpA5;_tmpA5.tunion_info=(void*)_tmp67;_tmpA5.targs=
Cyc_Tcutil_copy_types(_tmp68);_tmpA5.rgn=(void*)Cyc_Tcutil_copy_type(_tmp69);
_tmpA5;});_tmpA4;});_tmpA3;});_LL22: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 3)
goto _LL24;_tmp6A=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp64)->f1;_tmp6B=(
void*)_tmp6A.field_info;_tmp6C=_tmp6A.targs;_LL23: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpA7;_tmpA7.tag=3;_tmpA7.f1=({struct Cyc_Absyn_TunionFieldInfo _tmpA8;_tmpA8.field_info=(
void*)_tmp6B;_tmpA8.targs=Cyc_Tcutil_copy_types(_tmp6C);_tmpA8;});_tmpA7;});
_tmpA6;});_LL24: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 4)goto _LL26;_tmp6D=((
struct Cyc_Absyn_PointerType_struct*)_tmp64)->f1;_tmp6E=(void*)_tmp6D.elt_typ;
_tmp6F=_tmp6D.elt_tq;_tmp70=_tmp6D.ptr_atts;_tmp71=(void*)_tmp70.rgn;_tmp72=
_tmp70.nullable;_tmp73=_tmp70.bounds;_tmp74=_tmp70.zero_term;_tmp75=_tmp70.ptrloc;
_LL25: {void*_tmpA9=Cyc_Tcutil_copy_type(_tmp6E);void*_tmpAA=Cyc_Tcutil_copy_type(
_tmp71);struct Cyc_Absyn_Conref*_tmpAB=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp72);struct Cyc_Absyn_Tqual _tmpAC=_tmp6F;struct Cyc_Absyn_Conref*
_tmpAD=Cyc_Tcutil_copy_conref(_tmp73);struct Cyc_Absyn_Conref*_tmpAE=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp74);return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_PointerType_struct _tmpB0;_tmpB0.tag=4;_tmpB0.f1=({struct Cyc_Absyn_PtrInfo
_tmpB1;_tmpB1.elt_typ=(void*)_tmpA9;_tmpB1.elt_tq=_tmpAC;_tmpB1.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpB2;_tmpB2.rgn=(void*)_tmpAA;_tmpB2.nullable=_tmpAB;
_tmpB2.bounds=_tmpAD;_tmpB2.zero_term=_tmpAE;_tmpB2.ptrloc=_tmp75;_tmpB2;});
_tmpB1;});_tmpB0;});_tmpAF;});}_LL26: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 5)
goto _LL28;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f1;_tmp77=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp64)->f2;_LL27: return(void*)({struct
Cyc_Absyn_IntType_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_IntType_struct
_tmpB4;_tmpB4.tag=5;_tmpB4.f1=(void*)_tmp76;_tmpB4.f2=(void*)_tmp77;_tmpB4;});
_tmpB3;});_LL28: if((int)_tmp64 != 1)goto _LL2A;_LL29: return t;_LL2A: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 6)goto _LL2C;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)
_tmp64)->f1;_LL2B: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmpB5=
_cycalloc_atomic(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_DoubleType_struct
_tmpB6;_tmpB6.tag=6;_tmpB6.f1=_tmp78;_tmpB6;});_tmpB5;});_LL2C: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 7)goto _LL2E;_tmp79=((struct Cyc_Absyn_ArrayType_struct*)
_tmp64)->f1;_tmp7A=(void*)_tmp79.elt_type;_tmp7B=_tmp79.tq;_tmp7C=_tmp79.num_elts;
_tmp7D=_tmp79.zero_term;_tmp7E=_tmp79.zt_loc;_LL2D: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpB8;_tmpB8.tag=7;_tmpB8.f1=({struct Cyc_Absyn_ArrayInfo _tmpB9;_tmpB9.elt_type=(
void*)Cyc_Tcutil_copy_type(_tmp7A);_tmpB9.tq=_tmp7B;_tmpB9.num_elts=_tmp7C;
_tmpB9.zero_term=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(
_tmp7D);_tmpB9.zt_loc=_tmp7E;_tmpB9;});_tmpB8;});_tmpB7;});_LL2E: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 8)goto _LL30;_tmp7F=((struct Cyc_Absyn_FnType_struct*)
_tmp64)->f1;_tmp80=_tmp7F.tvars;_tmp81=_tmp7F.effect;_tmp82=(void*)_tmp7F.ret_typ;
_tmp83=_tmp7F.args;_tmp84=_tmp7F.c_varargs;_tmp85=_tmp7F.cyc_varargs;_tmp86=
_tmp7F.rgn_po;_tmp87=_tmp7F.attributes;_LL2F: {struct Cyc_List_List*_tmpBA=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp80);struct Cyc_Core_Opt*
_tmpBB=_tmp81 == 0?0:({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));
_tmpC5->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp81->v);_tmpC5;});void*_tmpBC=
Cyc_Tcutil_copy_type(_tmp82);struct Cyc_List_List*_tmpBD=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp83);int _tmpBE=_tmp84;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp85 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp85;cyc_varargs2=({
struct Cyc_Absyn_VarargInfo*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->name=cv->name;
_tmpBF->tq=cv->tq;_tmpBF->type=(void*)Cyc_Tcutil_copy_type((void*)cv->type);
_tmpBF->inject=cv->inject;_tmpBF;});}{struct Cyc_List_List*_tmpC0=((struct Cyc_List_List*(*)(
struct _tuple6*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp86);struct Cyc_List_List*_tmpC1=_tmp87;return(void*)({struct Cyc_Absyn_FnType_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_FnType_struct
_tmpC3;_tmpC3.tag=8;_tmpC3.f1=({struct Cyc_Absyn_FnInfo _tmpC4;_tmpC4.tvars=_tmpBA;
_tmpC4.effect=_tmpBB;_tmpC4.ret_typ=(void*)_tmpBC;_tmpC4.args=_tmpBD;_tmpC4.c_varargs=
_tmpBE;_tmpC4.cyc_varargs=cyc_varargs2;_tmpC4.rgn_po=_tmpC0;_tmpC4.attributes=
_tmpC1;_tmpC4;});_tmpC3;});_tmpC2;});}}_LL30: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 9)goto _LL32;_tmp88=((struct Cyc_Absyn_TupleType_struct*)_tmp64)->f1;
_LL31: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpC6=_cycalloc(sizeof(*
_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_TupleType_struct _tmpC7;_tmpC7.tag=9;_tmpC7.f1=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp88);_tmpC7;});_tmpC6;});_LL32: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 10)goto _LL34;_tmp89=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8A=(void*)_tmp89.aggr_info;if(*((int*)_tmp8A)!= 0)goto _LL34;
_tmp8B=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f1;_tmp8C=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp8A)->f2;_tmp8D=_tmp89.targs;_LL33: return(void*)({
struct Cyc_Absyn_AggrType_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({
struct Cyc_Absyn_AggrType_struct _tmpC9;_tmpC9.tag=10;_tmpC9.f1=({struct Cyc_Absyn_AggrInfo
_tmpCA;_tmpCA.aggr_info=(void*)((void*)({struct Cyc_Absyn_UnknownAggr_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_UnknownAggr_struct
_tmpCC;_tmpCC.tag=0;_tmpCC.f1=(void*)_tmp8B;_tmpCC.f2=_tmp8C;_tmpCC;});_tmpCB;}));
_tmpCA.targs=Cyc_Tcutil_copy_types(_tmp8D);_tmpCA;});_tmpC9;});_tmpC8;});_LL34:
if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 10)goto _LL36;_tmp8E=((struct Cyc_Absyn_AggrType_struct*)
_tmp64)->f1;_tmp8F=(void*)_tmp8E.aggr_info;if(*((int*)_tmp8F)!= 1)goto _LL36;
_tmp90=((struct Cyc_Absyn_KnownAggr_struct*)_tmp8F)->f1;_tmp91=_tmp8E.targs;_LL35:
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=({struct Cyc_Absyn_AggrType_struct _tmpCE;_tmpCE.tag=10;_tmpCE.f1=({
struct Cyc_Absyn_AggrInfo _tmpCF;_tmpCF.aggr_info=(void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmpD1;_tmpD1.tag=1;_tmpD1.f1=_tmp90;_tmpD1;});_tmpD0;}));_tmpCF.targs=Cyc_Tcutil_copy_types(
_tmp91);_tmpCF;});_tmpCE;});_tmpCD;});_LL36: if(_tmp64 <= (void*)3  || *((int*)
_tmp64)!= 11)goto _LL38;_tmp92=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp64)->f1;_tmp93=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp64)->f2;_LL37:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpD2=_cycalloc(sizeof(*
_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpD3;_tmpD3.tag=11;
_tmpD3.f1=(void*)_tmp92;_tmpD3.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp93);_tmpD3;});_tmpD2;});_LL38: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 12)
goto _LL3A;_tmp94=((struct Cyc_Absyn_EnumType_struct*)_tmp64)->f1;_tmp95=((struct
Cyc_Absyn_EnumType_struct*)_tmp64)->f2;_LL39: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD5;_tmpD5.tag=12;_tmpD5.f1=_tmp94;_tmpD5.f2=_tmp95;_tmpD5;});_tmpD4;});_LL3A:
if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 13)goto _LL3C;_tmp96=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp64)->f1;_LL3B: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpD6=
_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpD7;
_tmpD7.tag=13;_tmpD7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmp96);_tmpD7;});_tmpD6;});_LL3C: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 14)
goto _LL3E;_tmp97=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp64)->f1;_LL3D:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));
_tmpD8[0]=({struct Cyc_Absyn_SizeofType_struct _tmpD9;_tmpD9.tag=14;_tmpD9.f1=(
void*)Cyc_Tcutil_copy_type(_tmp97);_tmpD9;});_tmpD8;});_LL3E: if(_tmp64 <= (void*)
3  || *((int*)_tmp64)!= 18)goto _LL40;_tmp98=((struct Cyc_Absyn_TypeInt_struct*)
_tmp64)->f1;_LL3F: return(void*)({struct Cyc_Absyn_TypeInt_struct*_tmpDA=
_cycalloc_atomic(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_TypeInt_struct
_tmpDB;_tmpDB.tag=18;_tmpDB.f1=_tmp98;_tmpDB;});_tmpDA;});_LL40: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 17)goto _LL42;_tmp99=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp64)->f1;_LL41: return(void*)({struct Cyc_Absyn_TagType_struct*_tmpDC=_cycalloc(
sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TagType_struct _tmpDD;_tmpDD.tag=17;
_tmpDD.f1=(void*)Cyc_Tcutil_copy_type(_tmp99);_tmpDD;});_tmpDC;});_LL42: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 15)goto _LL44;_tmp9A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp64)->f1;_LL43: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpDE=
_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmpDF;_tmpDF.tag=15;_tmpDF.f1=(void*)Cyc_Tcutil_copy_type(_tmp9A);_tmpDF;});
_tmpDE;});_LL44: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 16)goto _LL46;_tmp9B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f1;_tmp9C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp64)->f2;_tmp9D=((struct Cyc_Absyn_TypedefType_struct*)_tmp64)->f3;_LL45:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpE0=_cycalloc(sizeof(*
_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_TypedefType_struct _tmpE1;_tmpE1.tag=16;
_tmpE1.f1=_tmp9B;_tmpE1.f2=Cyc_Tcutil_copy_types(_tmp9C);_tmpE1.f3=_tmp9D;_tmpE1.f4=
0;_tmpE1;});_tmpE0;});_LL46: if((int)_tmp64 != 2)goto _LL48;_LL47: return t;_LL48: if(
_tmp64 <= (void*)3  || *((int*)_tmp64)!= 19)goto _LL4A;_tmp9E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp64)->f1;_LL49: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmpE2=
_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_AccessEff_struct _tmpE3;
_tmpE3.tag=19;_tmpE3.f1=(void*)Cyc_Tcutil_copy_type(_tmp9E);_tmpE3;});_tmpE2;});
_LL4A: if(_tmp64 <= (void*)3  || *((int*)_tmp64)!= 20)goto _LL4C;_tmp9F=((struct Cyc_Absyn_JoinEff_struct*)
_tmp64)->f1;_LL4B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpE4=_cycalloc(
sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_JoinEff_struct _tmpE5;_tmpE5.tag=20;
_tmpE5.f1=Cyc_Tcutil_copy_types(_tmp9F);_tmpE5;});_tmpE4;});_LL4C: if(_tmp64 <= (
void*)3  || *((int*)_tmp64)!= 21)goto _LL19;_tmpA0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp64)->f1;_LL4D: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpE6=_cycalloc(
sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpE7;_tmpE7.tag=21;
_tmpE7.f1=(void*)Cyc_Tcutil_copy_type(_tmpA0);_tmpE7;});_tmpE6;});_LL19:;}int Cyc_Tcutil_kind_leq(
void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple6 _tmpE9=({struct _tuple6 _tmpE8;
_tmpE8.f1=k1;_tmpE8.f2=k2;_tmpE8;});void*_tmpEA;void*_tmpEB;void*_tmpEC;void*
_tmpED;void*_tmpEE;void*_tmpEF;_LL4F: _tmpEA=_tmpE9.f1;if((int)_tmpEA != 2)goto
_LL51;_tmpEB=_tmpE9.f2;if((int)_tmpEB != 1)goto _LL51;_LL50: goto _LL52;_LL51: _tmpEC=
_tmpE9.f1;if((int)_tmpEC != 2)goto _LL53;_tmpED=_tmpE9.f2;if((int)_tmpED != 0)goto
_LL53;_LL52: goto _LL54;_LL53: _tmpEE=_tmpE9.f1;if((int)_tmpEE != 1)goto _LL55;_tmpEF=
_tmpE9.f2;if((int)_tmpEF != 0)goto _LL55;_LL54: return 1;_LL55:;_LL56: return 0;_LL4E:;}}
void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpF0=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpF1;void*_tmpF2;_LL58: if(*((int*)_tmpF0)!= 0)goto _LL5A;
_tmpF1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF0)->f1;_LL59: return _tmpF1;
_LL5A: if(*((int*)_tmpF0)!= 2)goto _LL5C;_tmpF2=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF0)->f2;_LL5B: return _tmpF2;_LL5C:;_LL5D:({void*_tmpF3[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="kind not suitably constrained!";
_tag_arr(_tmpF4,sizeof(char),_get_zero_arr_size(_tmpF4,31));}),_tag_arr(_tmpF3,
sizeof(void*),0));});_LL57:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpF5=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpF6;struct Cyc_Core_Opt*_tmpF7;struct Cyc_Absyn_Tvar*
_tmpF8;void*_tmpF9;struct Cyc_Absyn_TunionFieldInfo _tmpFA;void*_tmpFB;struct Cyc_Absyn_Tunionfield*
_tmpFC;struct Cyc_Absyn_TunionFieldInfo _tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumdecl*
_tmpFF;struct Cyc_Absyn_AggrInfo _tmp100;void*_tmp101;struct Cyc_Absyn_AggrInfo
_tmp102;void*_tmp103;struct Cyc_Absyn_Aggrdecl**_tmp104;struct Cyc_Absyn_Aggrdecl*
_tmp105;struct Cyc_Absyn_Aggrdecl _tmp106;struct Cyc_Absyn_AggrdeclImpl*_tmp107;
struct Cyc_Absyn_Enumdecl*_tmp108;struct Cyc_Absyn_PtrInfo _tmp109;struct Cyc_Absyn_Typedefdecl*
_tmp10A;_LL5F: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 0)goto _LL61;_tmpF6=((
struct Cyc_Absyn_Evar_struct*)_tmpF5)->f1;_tmpF7=((struct Cyc_Absyn_Evar_struct*)
_tmpF5)->f2;_LL60: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6))->v;
_LL61: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 1)goto _LL63;_tmpF8=((struct Cyc_Absyn_VarType_struct*)
_tmpF5)->f1;_LL62: return Cyc_Tcutil_tvar_kind(_tmpF8);_LL63: if((int)_tmpF5 != 0)
goto _LL65;_LL64: return(void*)1;_LL65: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 5)
goto _LL67;_tmpF9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF5)->f2;_LL66:
return _tmpF9 == (void*)((void*)2)?(void*)2:(void*)1;_LL67: if((int)_tmpF5 != 1)goto
_LL69;_LL68: goto _LL6A;_LL69: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 6)goto
_LL6B;_LL6A: goto _LL6C;_LL6B: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 8)goto
_LL6D;_LL6C: return(void*)1;_LL6D: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 15)
goto _LL6F;_LL6E: return(void*)2;_LL6F: if((int)_tmpF5 != 2)goto _LL71;_LL70: return(
void*)3;_LL71: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 2)goto _LL73;_LL72: return(
void*)2;_LL73: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 3)goto _LL75;_tmpFA=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF5)->f1;_tmpFB=(void*)_tmpFA.field_info;
if(*((int*)_tmpFB)!= 1)goto _LL75;_tmpFC=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpFB)->f2;_LL74: if(_tmpFC->typs == 0)return(void*)2;else{return(void*)1;}_LL75:
if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 3)goto _LL77;_tmpFD=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF5)->f1;_tmpFE=(void*)_tmpFD.field_info;if(*((int*)_tmpFE)!= 0)goto _LL77;
_LL76:({void*_tmp10B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp10C="typ_kind: Unresolved TunionFieldType";
_tag_arr(_tmp10C,sizeof(char),_get_zero_arr_size(_tmp10C,37));}),_tag_arr(
_tmp10B,sizeof(void*),0));});_LL77: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 12)
goto _LL79;_tmpFF=((struct Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;if(_tmpFF != 0)
goto _LL79;_LL78: goto _LL7A;_LL79: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 10)
goto _LL7B;_tmp100=((struct Cyc_Absyn_AggrType_struct*)_tmpF5)->f1;_tmp101=(void*)
_tmp100.aggr_info;if(*((int*)_tmp101)!= 0)goto _LL7B;_LL7A: return(void*)0;_LL7B:
if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 10)goto _LL7D;_tmp102=((struct Cyc_Absyn_AggrType_struct*)
_tmpF5)->f1;_tmp103=(void*)_tmp102.aggr_info;if(*((int*)_tmp103)!= 1)goto _LL7D;
_tmp104=((struct Cyc_Absyn_KnownAggr_struct*)_tmp103)->f1;_tmp105=*_tmp104;
_tmp106=*_tmp105;_tmp107=_tmp106.impl;_LL7C: return _tmp107 == 0?(void*)0:(void*)1;
_LL7D: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 11)goto _LL7F;_LL7E: goto _LL80;
_LL7F: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 13)goto _LL81;_LL80: return(void*)
1;_LL81: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 12)goto _LL83;_tmp108=((struct
Cyc_Absyn_EnumType_struct*)_tmpF5)->f2;_LL82: if(_tmp108->fields == 0)return(void*)
0;else{return(void*)1;}_LL83: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 4)goto
_LL85;_tmp109=((struct Cyc_Absyn_PointerType_struct*)_tmpF5)->f1;_LL84: {void*
_tmp10D=(void*)(Cyc_Absyn_compress_conref((_tmp109.ptr_atts).bounds))->v;void*
_tmp10E;void*_tmp10F;void*_tmp110;_LL98: if((int)_tmp10D != 0)goto _LL9A;_LL99: goto
_LL9B;_LL9A: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)goto _LL9C;_tmp10E=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if((int)_tmp10E != 0)goto
_LL9C;_LL9B: return(void*)1;_LL9C: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)
goto _LL9E;_tmp10F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D)->f1;if(
_tmp10F <= (void*)1  || *((int*)_tmp10F)!= 0)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(
_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 0)goto _LLA0;_tmp110=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp10D)->f1;if(_tmp110 <= (void*)1  || *((int*)_tmp110)!= 1)goto _LLA0;_LL9F:
return(void*)2;_LLA0: if(_tmp10D <= (void*)1  || *((int*)_tmp10D)!= 1)goto _LL97;
_LLA1:({void*_tmp111[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp112="typ_kind: forward constr in ptr bounds";
_tag_arr(_tmp112,sizeof(char),_get_zero_arr_size(_tmp112,39));}),_tag_arr(
_tmp111,sizeof(void*),0));});_LL97:;}_LL85: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 14)goto _LL87;_LL86: return(void*)2;_LL87: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 18)goto _LL89;_LL88: return(void*)5;_LL89: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 17)goto _LL8B;_LL8A: return(void*)2;_LL8B: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 7)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 9)goto _LL8F;_LL8E: return(void*)1;_LL8F: if(_tmpF5 <= (void*)3  || *((int*)
_tmpF5)!= 16)goto _LL91;_tmp10A=((struct Cyc_Absyn_TypedefType_struct*)_tmpF5)->f3;
_LL90: if(_tmp10A == 0  || _tmp10A->kind == 0)({struct Cyc_String_pa_struct _tmp115;
_tmp115.tag=0;_tmp115.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp113[1]={& _tmp115};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)(({const char*_tmp114="typ_kind: typedef found: %s";_tag_arr(
_tmp114,sizeof(char),_get_zero_arr_size(_tmp114,28));}),_tag_arr(_tmp113,sizeof(
void*),1));}});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp10A->kind))->v;
_LL91: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 19)goto _LL93;_LL92: goto _LL94;
_LL93: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 20)goto _LL95;_LL94: goto _LL96;
_LL95: if(_tmpF5 <= (void*)3  || *((int*)_tmpF5)!= 21)goto _LL5E;_LL96: return(void*)
4;_LL5E:;}int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmp116;
_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!
_tmp118){Cyc_Tcutil_unify_it(t1,t2);{int _tmp119=1;_npop_handler(0);return _tmp119;};
_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*_tmp11B=_tmp117;_LLA3:
if(_tmp11B != Cyc_Tcutil_Unify)goto _LLA5;_LLA4: return 0;_LLA5:;_LLA6:(void)_throw(
_tmp11B);_LLA2:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp11C=t;struct Cyc_Absyn_Tvar*_tmp11D;struct Cyc_Core_Opt*_tmp11E;
struct Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt**_tmp120;struct Cyc_Absyn_PtrInfo
_tmp121;struct Cyc_Absyn_ArrayInfo _tmp122;void*_tmp123;struct Cyc_Absyn_FnInfo
_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Core_Opt*_tmp126;void*_tmp127;
struct Cyc_List_List*_tmp128;int _tmp129;struct Cyc_Absyn_VarargInfo*_tmp12A;struct
Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12D;
struct Cyc_Absyn_TunionInfo _tmp12E;struct Cyc_List_List*_tmp12F;void*_tmp130;
struct Cyc_List_List*_tmp131;struct Cyc_Absyn_TunionFieldInfo _tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_AggrInfo _tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*
_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;struct
Cyc_List_List*_tmp13C;_LLA8: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 1)goto
_LLAA;_tmp11D=((struct Cyc_Absyn_VarType_struct*)_tmp11C)->f1;_LLA9: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp11D)){Cyc_Tcutil_failure_reason=({
const char*_tmp13D="(type variable would escape scope)";_tag_arr(_tmp13D,sizeof(
char),_get_zero_arr_size(_tmp13D,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}
goto _LLA7;_LLAA: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 0)goto _LLAC;_tmp11E=((
struct Cyc_Absyn_Evar_struct*)_tmp11C)->f2;_tmp11F=((struct Cyc_Absyn_Evar_struct*)
_tmp11C)->f4;_tmp120=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp11C)->f4;_LLAB: if(t == evar){Cyc_Tcutil_failure_reason=({const char*_tmp13E="(occurs check)";
_tag_arr(_tmp13E,sizeof(char),_get_zero_arr_size(_tmp13E,15));});(int)_throw((
void*)Cyc_Tcutil_Unify);}else{if(_tmp11E != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)
_tmp11E->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp120))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp13F=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp120))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd))_tmp13F=({struct Cyc_List_List*_tmp140=
_cycalloc(sizeof(*_tmp140));_tmp140->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp140->tl=
_tmp13F;_tmp140;});}}*_tmp120=({struct Cyc_Core_Opt*_tmp141=_cycalloc(sizeof(*
_tmp141));_tmp141->v=_tmp13F;_tmp141;});}}}goto _LLA7;_LLAC: if(_tmp11C <= (void*)3
 || *((int*)_tmp11C)!= 4)goto _LLAE;_tmp121=((struct Cyc_Absyn_PointerType_struct*)
_tmp11C)->f1;_LLAD: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp121.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp121.ptr_atts).rgn);{void*_tmp142=(void*)(Cyc_Absyn_compress_conref((
_tmp121.ptr_atts).bounds))->v;void*_tmp143;void*_tmp144;_LLCD: if(_tmp142 <= (void*)
1  || *((int*)_tmp142)!= 0)goto _LLCF;_tmp143=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp142)->f1;if(_tmp143 <= (void*)1  || *((int*)_tmp143)!= 1)goto _LLCF;_tmp144=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp143)->f1;_LLCE: Cyc_Tcutil_occurs(
evar,r,env,_tmp144);goto _LLCC;_LLCF:;_LLD0: goto _LLCC;_LLCC:;}goto _LLA7;_LLAE: if(
_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 7)goto _LLB0;_tmp122=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11C)->f1;_tmp123=(void*)_tmp122.elt_type;_LLAF: Cyc_Tcutil_occurs(evar,r,env,
_tmp123);goto _LLA7;_LLB0: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 8)goto _LLB2;
_tmp124=((struct Cyc_Absyn_FnType_struct*)_tmp11C)->f1;_tmp125=_tmp124.tvars;
_tmp126=_tmp124.effect;_tmp127=(void*)_tmp124.ret_typ;_tmp128=_tmp124.args;
_tmp129=_tmp124.c_varargs;_tmp12A=_tmp124.cyc_varargs;_tmp12B=_tmp124.rgn_po;
_tmp12C=_tmp124.attributes;_LLB1: env=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,
_tmp125,env);if(_tmp126 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp126->v);Cyc_Tcutil_occurs(
evar,r,env,_tmp127);for(0;_tmp128 != 0;_tmp128=_tmp128->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp128->hd)).f3);}if(_tmp12A != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp12A->type);for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){struct
_tuple6 _tmp146;void*_tmp147;void*_tmp148;struct _tuple6*_tmp145=(struct _tuple6*)
_tmp12B->hd;_tmp146=*_tmp145;_tmp147=_tmp146.f1;_tmp148=_tmp146.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp147);Cyc_Tcutil_occurs(evar,r,env,_tmp148);}goto _LLA7;_LLB2: if(
_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 9)goto _LLB4;_tmp12D=((struct Cyc_Absyn_TupleType_struct*)
_tmp11C)->f1;_LLB3: for(0;_tmp12D != 0;_tmp12D=_tmp12D->tl){Cyc_Tcutil_occurs(evar,
r,env,(*((struct _tuple4*)_tmp12D->hd)).f2);}goto _LLA7;_LLB4: if(_tmp11C <= (void*)
3  || *((int*)_tmp11C)!= 2)goto _LLB6;_tmp12E=((struct Cyc_Absyn_TunionType_struct*)
_tmp11C)->f1;_tmp12F=_tmp12E.targs;_tmp130=(void*)_tmp12E.rgn;_LLB5: Cyc_Tcutil_occurs(
evar,r,env,_tmp130);Cyc_Tcutil_occurslist(evar,r,env,_tmp12F);goto _LLA7;_LLB6:
if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 16)goto _LLB8;_tmp131=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11C)->f2;_LLB7: _tmp133=_tmp131;goto _LLB9;_LLB8: if(_tmp11C <= (void*)3  || *((
int*)_tmp11C)!= 3)goto _LLBA;_tmp132=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11C)->f1;_tmp133=_tmp132.targs;_LLB9: _tmp135=_tmp133;goto _LLBB;_LLBA: if(
_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 10)goto _LLBC;_tmp134=((struct Cyc_Absyn_AggrType_struct*)
_tmp11C)->f1;_tmp135=_tmp134.targs;_LLBB: Cyc_Tcutil_occurslist(evar,r,env,
_tmp135);goto _LLA7;_LLBC: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 11)goto
_LLBE;_tmp136=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11C)->f2;_LLBD: for(0;
_tmp136 != 0;_tmp136=_tmp136->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp136->hd)->type);}goto _LLA7;_LLBE: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 
17)goto _LLC0;_tmp137=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp11C)->f1;_LLBF:
_tmp138=_tmp137;goto _LLC1;_LLC0: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 14)
goto _LLC2;_tmp138=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp11C)->f1;_LLC1:
_tmp139=_tmp138;goto _LLC3;_LLC2: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 19)
goto _LLC4;_tmp139=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11C)->f1;_LLC3:
_tmp13A=_tmp139;goto _LLC5;_LLC4: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)!= 15)
goto _LLC6;_tmp13A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11C)->f1;
_LLC5: _tmp13B=_tmp13A;goto _LLC7;_LLC6: if(_tmp11C <= (void*)3  || *((int*)_tmp11C)
!= 21)goto _LLC8;_tmp13B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp11C)->f1;
_LLC7: Cyc_Tcutil_occurs(evar,r,env,_tmp13B);goto _LLA7;_LLC8: if(_tmp11C <= (void*)
3  || *((int*)_tmp11C)!= 20)goto _LLCA;_tmp13C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp11C)->f1;_LLC9: Cyc_Tcutil_occurslist(evar,r,env,_tmp13C);goto _LLA7;_LLCA:;
_LLCB: goto _LLA7;_LLA7:;}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=
ts->tl){Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(
struct Cyc_List_List*t1,struct Cyc_List_List*t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,
t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}if(t1 != 0  || t2 != 
0)(int)_throw((void*)Cyc_Tcutil_Unify);}static void Cyc_Tcutil_unify_tqual(struct
Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const
 && !tq1.real_const)({void*_tmp149[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp14A="tq1 real_const not set.";
_tag_arr(_tmp14A,sizeof(char),_get_zero_arr_size(_tmp14A,24));}),_tag_arr(
_tmp149,sizeof(void*),0));});if(tq2.print_const  && !tq2.real_const)({void*
_tmp14B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp14C="tq2 real_const not set.";_tag_arr(_tmp14C,sizeof(char),
_get_zero_arr_size(_tmp14C,24));}),_tag_arr(_tmp14B,sizeof(void*),0));});if((tq1.real_const
!= tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;Cyc_Tcutil_failure_reason=({const char*
_tmp14D="(qualifiers don't match)";_tag_arr(_tmp14D,sizeof(char),
_get_zero_arr_size(_tmp14D,25));});(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=
0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{void*_tmp14E=(void*)x->v;void*_tmp14F;_LLD2: if((int)_tmp14E != 0)
goto _LLD4;_LLD3:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp151;_tmp151.tag=1;_tmp151.f1=y;_tmp151;});_tmp150;})));return;_LLD4: if(
_tmp14E <= (void*)1  || *((int*)_tmp14E)!= 0)goto _LLD6;_tmp14F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14E)->f1;_LLD5: {void*_tmp152=(void*)y->v;void*_tmp153;_LLD9: if((int)_tmp152
!= 0)goto _LLDB;_LLDA:(void*)(y->v=(void*)((void*)x->v));return;_LLDB: if(_tmp152
<= (void*)1  || *((int*)_tmp152)!= 0)goto _LLDD;_tmp153=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp152)->f1;_LLDC: if(cmp(_tmp14F,_tmp153)!= 0){Cyc_Tcutil_failure_reason=reason;(
int)_throw((void*)Cyc_Tcutil_Unify);}return;_LLDD: if(_tmp152 <= (void*)1  || *((
int*)_tmp152)!= 1)goto _LLD8;_LLDE:({void*_tmp154[0]={};Cyc_Tcutil_impos(({const
char*_tmp155="unify_conref: forward after compress(2)";_tag_arr(_tmp155,sizeof(
char),_get_zero_arr_size(_tmp155,40));}),_tag_arr(_tmp154,sizeof(void*),0));});
_LLD8:;}_LLD6: if(_tmp14E <= (void*)1  || *((int*)_tmp14E)!= 1)goto _LLD1;_LLD7:({
void*_tmp156[0]={};Cyc_Tcutil_impos(({const char*_tmp157="unify_conref: forward after compress";
_tag_arr(_tmp157,sizeof(char),_get_zero_arr_size(_tmp157,37));}),_tag_arr(
_tmp156,sizeof(void*),0));});_LLD1:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct
_handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))
_tmp15A=1;if(!_tmp15A){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)
_tag_arr(0,0,0));{int _tmp15B=1;_npop_handler(0);return _tmp15B;};_pop_handler();}
else{void*_tmp159=(void*)_exn_thrown;void*_tmp15D=_tmp159;_LLE0: if(_tmp15D != Cyc_Tcutil_Unify)
goto _LLE2;_LLE1: return 0;_LLE2:;_LLE3:(void)_throw(_tmp15D);_LLDF:;}}}static int
Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple6 _tmp15F=({struct _tuple6
_tmp15E;_tmp15E.f1=b1;_tmp15E.f2=b2;_tmp15E;});void*_tmp160;void*_tmp161;void*
_tmp162;void*_tmp163;void*_tmp164;struct Cyc_Absyn_Exp*_tmp165;void*_tmp166;
struct Cyc_Absyn_Exp*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;
void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;_LLE5: _tmp160=_tmp15F.f1;if((
int)_tmp160 != 0)goto _LLE7;_tmp161=_tmp15F.f2;if((int)_tmp161 != 0)goto _LLE7;_LLE6:
return 0;_LLE7: _tmp162=_tmp15F.f1;if((int)_tmp162 != 0)goto _LLE9;_LLE8: return - 1;
_LLE9: _tmp163=_tmp15F.f2;if((int)_tmp163 != 0)goto _LLEB;_LLEA: return 1;_LLEB:
_tmp164=_tmp15F.f1;if(_tmp164 <= (void*)1  || *((int*)_tmp164)!= 0)goto _LLED;
_tmp165=((struct Cyc_Absyn_Upper_b_struct*)_tmp164)->f1;_tmp166=_tmp15F.f2;if(
_tmp166 <= (void*)1  || *((int*)_tmp166)!= 0)goto _LLED;_tmp167=((struct Cyc_Absyn_Upper_b_struct*)
_tmp166)->f1;_LLEC: return Cyc_Evexp_const_exp_cmp(_tmp165,_tmp167);_LLED: _tmp168=
_tmp15F.f1;if(_tmp168 <= (void*)1  || *((int*)_tmp168)!= 0)goto _LLEF;_tmp169=
_tmp15F.f2;if(_tmp169 <= (void*)1  || *((int*)_tmp169)!= 1)goto _LLEF;_LLEE: return -
1;_LLEF: _tmp16A=_tmp15F.f1;if(_tmp16A <= (void*)1  || *((int*)_tmp16A)!= 1)goto
_LLF1;_tmp16B=_tmp15F.f2;if(_tmp16B <= (void*)1  || *((int*)_tmp16B)!= 0)goto _LLF1;
_LLF0: return 1;_LLF1: _tmp16C=_tmp15F.f1;if(_tmp16C <= (void*)1  || *((int*)_tmp16C)
!= 1)goto _LLE4;_tmp16D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16C)->f1;
_tmp16E=_tmp15F.f2;if(_tmp16E <= (void*)1  || *((int*)_tmp16E)!= 1)goto _LLE4;
_tmp16F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp16E)->f1;_LLF2: return Cyc_Tcutil_typecmp(
_tmp16D,_tmp16F);_LLE4:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple6 _tmp171=({struct _tuple6 _tmp170;_tmp170.f1=b1;_tmp170.f2=b2;_tmp170;});
void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;void*_tmp178;struct Cyc_Absyn_Exp*_tmp179;void*_tmp17A;struct Cyc_Absyn_Exp*
_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;void*_tmp180;struct
Cyc_Absyn_Exp*_tmp181;void*_tmp182;void*_tmp183;void*_tmp184;void*_tmp185;_LLF4:
_tmp172=_tmp171.f1;if((int)_tmp172 != 0)goto _LLF6;_tmp173=_tmp171.f2;if((int)
_tmp173 != 0)goto _LLF6;_LLF5: return 0;_LLF6: _tmp174=_tmp171.f1;if((int)_tmp174 != 0)
goto _LLF8;_LLF7: return - 1;_LLF8: _tmp175=_tmp171.f2;if((int)_tmp175 != 0)goto _LLFA;
_LLF9: return 1;_LLFA: _tmp176=_tmp171.f1;if(_tmp176 <= (void*)1  || *((int*)_tmp176)
!= 0)goto _LLFC;_tmp177=((struct Cyc_Absyn_Upper_b_struct*)_tmp176)->f1;_tmp178=
_tmp171.f2;if(_tmp178 <= (void*)1  || *((int*)_tmp178)!= 0)goto _LLFC;_tmp179=((
struct Cyc_Absyn_Upper_b_struct*)_tmp178)->f1;_LLFB: return Cyc_Evexp_const_exp_cmp(
_tmp177,_tmp179);_LLFC: _tmp17A=_tmp171.f1;if(_tmp17A <= (void*)1  || *((int*)
_tmp17A)!= 0)goto _LLFE;_tmp17B=((struct Cyc_Absyn_Upper_b_struct*)_tmp17A)->f1;
_tmp17C=_tmp171.f2;if(_tmp17C <= (void*)1  || *((int*)_tmp17C)!= 1)goto _LLFE;
_tmp17D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp17C)->f1;_LLFD: _tmp17F=
_tmp17D;_tmp181=_tmp17B;goto _LLFF;_LLFE: _tmp17E=_tmp171.f1;if(_tmp17E <= (void*)1
 || *((int*)_tmp17E)!= 1)goto _LL100;_tmp17F=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp17E)->f1;_tmp180=_tmp171.f2;if(_tmp180 <= (void*)1  || *((int*)_tmp180)!= 0)
goto _LL100;_tmp181=((struct Cyc_Absyn_Upper_b_struct*)_tmp180)->f1;_LLFF: {
unsigned int _tmp187;int _tmp188;struct _tuple7 _tmp186=Cyc_Evexp_eval_const_uint_exp(
_tmp181);_tmp187=_tmp186.f1;_tmp188=_tmp186.f2;if(!_tmp188)return 1;_tmp183=
_tmp17F;_tmp185=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp189=_cycalloc_atomic(
sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Absyn_TypeInt_struct _tmp18A;_tmp18A.tag=
18;_tmp18A.f1=(int)_tmp187;_tmp18A;});_tmp189;});goto _LL101;}_LL100: _tmp182=
_tmp171.f1;if(_tmp182 <= (void*)1  || *((int*)_tmp182)!= 1)goto _LLF3;_tmp183=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp182)->f1;_tmp184=_tmp171.f2;if(_tmp184 <= (
void*)1  || *((int*)_tmp184)!= 1)goto _LLF3;_tmp185=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp184)->f1;_LL101: Cyc_Tcutil_unify_it(_tmp183,_tmp185);return 0;_LLF3:;}static
int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp18B=att;_LL103: if(_tmp18B
<= (void*)17  || *((int*)_tmp18B)!= 0)goto _LL105;_LL104: return 0;_LL105: if((int)
_tmp18B != 0)goto _LL107;_LL106: return 1;_LL107: if((int)_tmp18B != 1)goto _LL109;
_LL108: return 2;_LL109: if((int)_tmp18B != 2)goto _LL10B;_LL10A: return 3;_LL10B: if((
int)_tmp18B != 3)goto _LL10D;_LL10C: return 4;_LL10D: if((int)_tmp18B != 4)goto _LL10F;
_LL10E: return 5;_LL10F: if(_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 1)goto _LL111;
_LL110: return 6;_LL111: if((int)_tmp18B != 5)goto _LL113;_LL112: return 7;_LL113: if(
_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 2)goto _LL115;_LL114: return 8;_LL115: if((
int)_tmp18B != 6)goto _LL117;_LL116: return 9;_LL117: if((int)_tmp18B != 7)goto _LL119;
_LL118: return 10;_LL119: if((int)_tmp18B != 8)goto _LL11B;_LL11A: return 11;_LL11B: if((
int)_tmp18B != 9)goto _LL11D;_LL11C: return 12;_LL11D: if((int)_tmp18B != 10)goto
_LL11F;_LL11E: return 13;_LL11F: if((int)_tmp18B != 11)goto _LL121;_LL120: return 14;
_LL121: if((int)_tmp18B != 12)goto _LL123;_LL122: return 15;_LL123: if((int)_tmp18B != 
13)goto _LL125;_LL124: return 16;_LL125: if((int)_tmp18B != 14)goto _LL127;_LL126:
return 17;_LL127: if((int)_tmp18B != 15)goto _LL129;_LL128: return 18;_LL129: if(
_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 3)goto _LL12B;_LL12A: return 19;_LL12B:
if(_tmp18B <= (void*)17  || *((int*)_tmp18B)!= 4)goto _LL12D;_LL12C: return 20;_LL12D:;
_LL12E: return 21;_LL102:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple6 _tmp18D=({struct _tuple6 _tmp18C;_tmp18C.f1=att1;_tmp18C.f2=att2;
_tmp18C;});void*_tmp18E;int _tmp18F;void*_tmp190;int _tmp191;void*_tmp192;int
_tmp193;void*_tmp194;int _tmp195;void*_tmp196;int _tmp197;void*_tmp198;int _tmp199;
void*_tmp19A;struct _tagged_arr _tmp19B;void*_tmp19C;struct _tagged_arr _tmp19D;void*
_tmp19E;void*_tmp19F;int _tmp1A0;int _tmp1A1;void*_tmp1A2;void*_tmp1A3;int _tmp1A4;
int _tmp1A5;_LL130: _tmp18E=_tmp18D.f1;if(_tmp18E <= (void*)17  || *((int*)_tmp18E)
!= 0)goto _LL132;_tmp18F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp18E)->f1;
_tmp190=_tmp18D.f2;if(_tmp190 <= (void*)17  || *((int*)_tmp190)!= 0)goto _LL132;
_tmp191=((struct Cyc_Absyn_Regparm_att_struct*)_tmp190)->f1;_LL131: _tmp193=
_tmp18F;_tmp195=_tmp191;goto _LL133;_LL132: _tmp192=_tmp18D.f1;if(_tmp192 <= (void*)
17  || *((int*)_tmp192)!= 4)goto _LL134;_tmp193=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp192)->f1;_tmp194=_tmp18D.f2;if(_tmp194 <= (void*)17  || *((int*)_tmp194)!= 4)
goto _LL134;_tmp195=((struct Cyc_Absyn_Initializes_att_struct*)_tmp194)->f1;_LL133:
_tmp197=_tmp193;_tmp199=_tmp195;goto _LL135;_LL134: _tmp196=_tmp18D.f1;if(_tmp196
<= (void*)17  || *((int*)_tmp196)!= 1)goto _LL136;_tmp197=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp196)->f1;_tmp198=_tmp18D.f2;if(_tmp198 <= (void*)17  || *((int*)_tmp198)!= 1)
goto _LL136;_tmp199=((struct Cyc_Absyn_Aligned_att_struct*)_tmp198)->f1;_LL135:
return Cyc_Core_intcmp(_tmp197,_tmp199);_LL136: _tmp19A=_tmp18D.f1;if(_tmp19A <= (
void*)17  || *((int*)_tmp19A)!= 2)goto _LL138;_tmp19B=((struct Cyc_Absyn_Section_att_struct*)
_tmp19A)->f1;_tmp19C=_tmp18D.f2;if(_tmp19C <= (void*)17  || *((int*)_tmp19C)!= 2)
goto _LL138;_tmp19D=((struct Cyc_Absyn_Section_att_struct*)_tmp19C)->f1;_LL137:
return Cyc_strcmp((struct _tagged_arr)_tmp19B,(struct _tagged_arr)_tmp19D);_LL138:
_tmp19E=_tmp18D.f1;if(_tmp19E <= (void*)17  || *((int*)_tmp19E)!= 3)goto _LL13A;
_tmp19F=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f1;_tmp1A0=((
struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f2;_tmp1A1=((struct Cyc_Absyn_Format_att_struct*)
_tmp19E)->f3;_tmp1A2=_tmp18D.f2;if(_tmp1A2 <= (void*)17  || *((int*)_tmp1A2)!= 3)
goto _LL13A;_tmp1A3=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A2)->f1;
_tmp1A4=((struct Cyc_Absyn_Format_att_struct*)_tmp1A2)->f2;_tmp1A5=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A2)->f3;_LL139: {int _tmp1A6=Cyc_Core_intcmp((int)((unsigned int)_tmp19F),(
int)((unsigned int)_tmp1A3));if(_tmp1A6 != 0)return _tmp1A6;{int _tmp1A7=Cyc_Core_intcmp(
_tmp1A0,_tmp1A4);if(_tmp1A7 != 0)return _tmp1A7;return Cyc_Core_intcmp(_tmp1A1,
_tmp1A5);}}_LL13A:;_LL13B: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL12F:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return
0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((
void*)af->type);}static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1A8=Cyc_Tcutil_tvar_kind(tv);_LL13D: if((int)_tmp1A8 != 3)
goto _LL13F;_LL13E: t=(void*)2;goto _LL13C;_LL13F: if((int)_tmp1A8 != 4)goto _LL141;
_LL140: t=Cyc_Absyn_empty_effect;goto _LL13C;_LL141: if((int)_tmp1A8 != 5)goto _LL143;
_LL142: t=(void*)({struct Cyc_Absyn_TypeInt_struct*_tmp1A9=_cycalloc_atomic(
sizeof(*_tmp1A9));_tmp1A9[0]=({struct Cyc_Absyn_TypeInt_struct _tmp1AA;_tmp1AA.tag=
18;_tmp1AA.f1=0;_tmp1AA;});_tmp1A9;});goto _LL13C;_LL143:;_LL144: t=Cyc_Absyn_sint_typ;
goto _LL13C;_LL13C:;}return({struct _tuple8*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));
_tmp1AB->f1=tv;_tmp1AB->f2=t;_tmp1AB;});}static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1AC=Cyc_Tcutil_compress(t);void*_tmp1AD;struct Cyc_Absyn_TunionInfo
_tmp1AE;struct Cyc_List_List*_tmp1AF;void*_tmp1B0;struct Cyc_Absyn_PtrInfo _tmp1B1;
void*_tmp1B2;struct Cyc_Absyn_PtrAtts _tmp1B3;void*_tmp1B4;struct Cyc_Absyn_ArrayInfo
_tmp1B5;void*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_TunionFieldInfo
_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_AggrInfo _tmp1BA;struct Cyc_List_List*
_tmp1BB;struct Cyc_List_List*_tmp1BC;void*_tmp1BD;struct Cyc_Absyn_FnInfo _tmp1BE;
struct Cyc_List_List*_tmp1BF;struct Cyc_Core_Opt*_tmp1C0;void*_tmp1C1;struct Cyc_List_List*
_tmp1C2;struct Cyc_Absyn_VarargInfo*_tmp1C3;struct Cyc_List_List*_tmp1C4;void*
_tmp1C5;struct Cyc_List_List*_tmp1C6;_LL146: if((int)_tmp1AC != 0)goto _LL148;_LL147:
goto _LL149;_LL148: if((int)_tmp1AC != 1)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(
_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 6)goto _LL14C;_LL14B: goto _LL14D;_LL14C:
if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 12)goto _LL14E;_LL14D: goto _LL14F;
_LL14E: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 13)goto _LL150;_LL14F: goto
_LL151;_LL150: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 18)goto _LL152;_LL151:
goto _LL153;_LL152: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 5)goto _LL154;
_LL153: return Cyc_Absyn_empty_effect;_LL154: if(_tmp1AC <= (void*)3  || *((int*)
_tmp1AC)!= 0)goto _LL156;_LL155: goto _LL157;_LL156: if(_tmp1AC <= (void*)3  || *((int*)
_tmp1AC)!= 1)goto _LL158;_LL157: {void*_tmp1C7=Cyc_Tcutil_typ_kind(t);_LL179: if((
int)_tmp1C7 != 3)goto _LL17B;_LL17A: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1C9;_tmp1C9.tag=19;_tmp1C9.f1=(void*)t;_tmp1C9;});_tmp1C8;});_LL17B: if((int)
_tmp1C7 != 4)goto _LL17D;_LL17C: return t;_LL17D: if((int)_tmp1C7 != 5)goto _LL17F;
_LL17E: return Cyc_Absyn_empty_effect;_LL17F:;_LL180: return(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1CB;_tmp1CB.tag=21;_tmp1CB.f1=(void*)t;_tmp1CB;});_tmp1CA;});_LL178:;}_LL158:
if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 15)goto _LL15A;_tmp1AD=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1AC)->f1;_LL159: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp1CD;_tmp1CD.tag=19;_tmp1CD.f1=(void*)_tmp1AD;
_tmp1CD;});_tmp1CC;});_LL15A: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 2)goto
_LL15C;_tmp1AE=((struct Cyc_Absyn_TunionType_struct*)_tmp1AC)->f1;_tmp1AF=_tmp1AE.targs;
_tmp1B0=(void*)_tmp1AE.rgn;_LL15B: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1D2;_tmp1D2.tag=19;_tmp1D2.f1=(void*)_tmp1B0;_tmp1D2;});_tmp1D1;}));_tmp1D0->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp1AF);_tmp1D0;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));
_tmp1CE[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1CF;_tmp1CF.tag=20;_tmp1CF.f1=ts;
_tmp1CF;});_tmp1CE;}));}_LL15C: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 4)
goto _LL15E;_tmp1B1=((struct Cyc_Absyn_PointerType_struct*)_tmp1AC)->f1;_tmp1B2=(
void*)_tmp1B1.elt_typ;_tmp1B3=_tmp1B1.ptr_atts;_tmp1B4=(void*)_tmp1B3.rgn;_LL15D:
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1D4;_tmp1D4.tag=20;_tmp1D4.f1=({void*_tmp1D5[2];_tmp1D5[1]=Cyc_Tcutil_rgns_of(
_tmp1B2);_tmp1D5[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp1D6=_cycalloc(
sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_AccessEff_struct _tmp1D7;_tmp1D7.tag=
19;_tmp1D7.f1=(void*)_tmp1B4;_tmp1D7;});_tmp1D6;});Cyc_List_list(_tag_arr(
_tmp1D5,sizeof(void*),2));});_tmp1D4;});_tmp1D3;}));_LL15E: if(_tmp1AC <= (void*)3
 || *((int*)_tmp1AC)!= 7)goto _LL160;_tmp1B5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AC)->f1;_tmp1B6=(void*)_tmp1B5.elt_type;_LL15F: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1B6));_LL160: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 
9)goto _LL162;_tmp1B7=((struct Cyc_Absyn_TupleType_struct*)_tmp1AC)->f1;_LL161: {
struct Cyc_List_List*_tmp1D8=0;for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){_tmp1D8=({
struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=(void*)(*((
struct _tuple4*)_tmp1B7->hd)).f2;_tmp1D9->tl=_tmp1D8;_tmp1D9;});}_tmp1B9=_tmp1D8;
goto _LL163;}_LL162: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 3)goto _LL164;
_tmp1B8=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1AC)->f1;_tmp1B9=_tmp1B8.targs;
_LL163: _tmp1BB=_tmp1B9;goto _LL165;_LL164: if(_tmp1AC <= (void*)3  || *((int*)
_tmp1AC)!= 10)goto _LL166;_tmp1BA=((struct Cyc_Absyn_AggrType_struct*)_tmp1AC)->f1;
_tmp1BB=_tmp1BA.targs;_LL165: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1DB;_tmp1DB.tag=20;_tmp1DB.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1BB);_tmp1DB;});_tmp1DA;}));_LL166: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)
!= 11)goto _LL168;_tmp1BC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AC)->f2;
_LL167: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1DD;_tmp1DD.tag=20;_tmp1DD.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1BC);_tmp1DD;});
_tmp1DC;}));_LL168: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 14)goto _LL16A;
_tmp1BD=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1AC)->f1;_LL169: return
Cyc_Tcutil_rgns_of(_tmp1BD);_LL16A: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 
17)goto _LL16C;_LL16B: return Cyc_Tcutil_rgns_of(t);_LL16C: if(_tmp1AC <= (void*)3
 || *((int*)_tmp1AC)!= 8)goto _LL16E;_tmp1BE=((struct Cyc_Absyn_FnType_struct*)
_tmp1AC)->f1;_tmp1BF=_tmp1BE.tvars;_tmp1C0=_tmp1BE.effect;_tmp1C1=(void*)_tmp1BE.ret_typ;
_tmp1C2=_tmp1BE.args;_tmp1C3=_tmp1BE.cyc_varargs;_tmp1C4=_tmp1BE.rgn_po;_LL16D: {
void*_tmp1DE=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1BF),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C0))->v);return Cyc_Tcutil_normalize_effect(
_tmp1DE);}_LL16E: if((int)_tmp1AC != 2)goto _LL170;_LL16F: return Cyc_Absyn_empty_effect;
_LL170: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 19)goto _LL172;_LL171: goto
_LL173;_LL172: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 20)goto _LL174;_LL173:
return t;_LL174: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 21)goto _LL176;_tmp1C5=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AC)->f1;_LL175: return Cyc_Tcutil_rgns_of(
_tmp1C5);_LL176: if(_tmp1AC <= (void*)3  || *((int*)_tmp1AC)!= 16)goto _LL145;
_tmp1C6=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AC)->f2;_LL177: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1E0;_tmp1E0.tag=20;_tmp1E0.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp1C6);_tmp1E0;});_tmp1DF;}));_LL145:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1E1=e;struct Cyc_List_List*_tmp1E2;
struct Cyc_List_List**_tmp1E3;void*_tmp1E4;_LL182: if(_tmp1E1 <= (void*)3  || *((int*)
_tmp1E1)!= 20)goto _LL184;_tmp1E2=((struct Cyc_Absyn_JoinEff_struct*)_tmp1E1)->f1;
_tmp1E3=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)_tmp1E1)->f1;
_LL183: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp1E3;for(0;effs != 0;effs=
effs->tl){void*_tmp1E5=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp1E5)));{void*_tmp1E6=(void*)effs->hd;void*
_tmp1E7;_LL189: if(_tmp1E6 <= (void*)3  || *((int*)_tmp1E6)!= 20)goto _LL18B;_LL18A:
goto _LL18C;_LL18B: if(_tmp1E6 <= (void*)3  || *((int*)_tmp1E6)!= 19)goto _LL18D;
_tmp1E7=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1E6)->f1;if((int)_tmp1E7
!= 2)goto _LL18D;_LL18C: redo_join=1;goto _LL188;_LL18D:;_LL18E: goto _LL188;_LL188:;}}}
if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp1E3;for(0;effs != 0;effs=effs->tl){void*_tmp1E8=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp1E9;void*_tmp1EA;_LL190: if(_tmp1E8 <= (void*)3
 || *((int*)_tmp1E8)!= 20)goto _LL192;_tmp1E9=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E8)->f1;_LL191: effects=Cyc_List_revappend(_tmp1E9,effects);goto _LL18F;
_LL192: if(_tmp1E8 <= (void*)3  || *((int*)_tmp1E8)!= 19)goto _LL194;_tmp1EA=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp1E8)->f1;if((int)_tmp1EA != 2)goto _LL194;
_LL193: goto _LL18F;_LL194:;_LL195: effects=({struct Cyc_List_List*_tmp1EB=_cycalloc(
sizeof(*_tmp1EB));_tmp1EB->hd=(void*)_tmp1E8;_tmp1EB->tl=effects;_tmp1EB;});goto
_LL18F;_LL18F:;}}*_tmp1E3=Cyc_List_imp_rev(effects);return e;}}_LL184: if(_tmp1E1
<= (void*)3  || *((int*)_tmp1E1)!= 21)goto _LL186;_tmp1E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E1)->f1;_LL185: {void*_tmp1EC=Cyc_Tcutil_compress(_tmp1E4);_LL197: if(
_tmp1EC <= (void*)3  || *((int*)_tmp1EC)!= 0)goto _LL199;_LL198: goto _LL19A;_LL199:
if(_tmp1EC <= (void*)3  || *((int*)_tmp1EC)!= 1)goto _LL19B;_LL19A: return e;_LL19B:;
_LL19C: return Cyc_Tcutil_rgns_of(_tmp1E4);_LL196:;}_LL186:;_LL187: return e;_LL181:;}}
static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct*_tmp1ED=({struct Cyc_Absyn_FnType_struct*
_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Absyn_FnType_struct
_tmp1EF;_tmp1EF.tag=8;_tmp1EF.f1=({struct Cyc_Absyn_FnInfo _tmp1F0;_tmp1F0.tvars=0;
_tmp1F0.effect=({struct Cyc_Core_Opt*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->v=(
void*)eff;_tmp1F1;});_tmp1F0.ret_typ=(void*)((void*)0);_tmp1F0.args=0;_tmp1F0.c_varargs=
0;_tmp1F0.cyc_varargs=0;_tmp1F0.rgn_po=0;_tmp1F0.attributes=0;_tmp1F0;});_tmp1EF;});
_tmp1EE;});return Cyc_Absyn_atb_typ((void*)_tmp1ED,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2)return 1;{
void*_tmp1F2=Cyc_Tcutil_compress(e);void*_tmp1F3;struct Cyc_List_List*_tmp1F4;
void*_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Core_Opt*_tmp1F7;struct Cyc_Core_Opt**
_tmp1F8;struct Cyc_Core_Opt*_tmp1F9;_LL19E: if(_tmp1F2 <= (void*)3  || *((int*)
_tmp1F2)!= 19)goto _LL1A0;_tmp1F3=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F2)->f1;_LL19F: if(constrain)return Cyc_Tcutil_unify(r,_tmp1F3);_tmp1F3=Cyc_Tcutil_compress(
_tmp1F3);if(r == _tmp1F3)return 1;{struct _tuple6 _tmp1FB=({struct _tuple6 _tmp1FA;
_tmp1FA.f1=r;_tmp1FA.f2=_tmp1F3;_tmp1FA;});void*_tmp1FC;struct Cyc_Absyn_Tvar*
_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Tvar*_tmp1FF;_LL1A9: _tmp1FC=_tmp1FB.f1;if(
_tmp1FC <= (void*)3  || *((int*)_tmp1FC)!= 1)goto _LL1AB;_tmp1FD=((struct Cyc_Absyn_VarType_struct*)
_tmp1FC)->f1;_tmp1FE=_tmp1FB.f2;if(_tmp1FE <= (void*)3  || *((int*)_tmp1FE)!= 1)
goto _LL1AB;_tmp1FF=((struct Cyc_Absyn_VarType_struct*)_tmp1FE)->f1;_LL1AA: return
Cyc_Absyn_tvar_cmp(_tmp1FD,_tmp1FF)== 0;_LL1AB:;_LL1AC: return 0;_LL1A8:;}_LL1A0:
if(_tmp1F2 <= (void*)3  || *((int*)_tmp1F2)!= 20)goto _LL1A2;_tmp1F4=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F2)->f1;_LL1A1: for(0;_tmp1F4 != 0;_tmp1F4=_tmp1F4->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp1F4->hd))return 1;}return 0;_LL1A2: if(_tmp1F2 <= (void*)3
 || *((int*)_tmp1F2)!= 21)goto _LL1A4;_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1F2)->f1;_LL1A3: {void*_tmp200=Cyc_Tcutil_rgns_of(_tmp1F5);void*_tmp201;
_LL1AE: if(_tmp200 <= (void*)3  || *((int*)_tmp200)!= 21)goto _LL1B0;_tmp201=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp200)->f1;_LL1AF: if(!constrain)return 0;{void*
_tmp202=Cyc_Tcutil_compress(_tmp201);struct Cyc_Core_Opt*_tmp203;struct Cyc_Core_Opt*
_tmp204;struct Cyc_Core_Opt**_tmp205;struct Cyc_Core_Opt*_tmp206;_LL1B3: if(_tmp202
<= (void*)3  || *((int*)_tmp202)!= 0)goto _LL1B5;_tmp203=((struct Cyc_Absyn_Evar_struct*)
_tmp202)->f1;_tmp204=((struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp205=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp202)->f2;_tmp206=((struct Cyc_Absyn_Evar_struct*)
_tmp202)->f4;_LL1B4: {void*_tmp207=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp206);Cyc_Tcutil_occurs(_tmp207,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp206))->v,r);{void*_tmp208=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));
_tmp20A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp20B;_tmp20B.tag=20;_tmp20B.f1=({
void*_tmp20C[2];_tmp20C[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp20D=
_cycalloc(sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_AccessEff_struct _tmp20E;
_tmp20E.tag=19;_tmp20E.f1=(void*)r;_tmp20E;});_tmp20D;});_tmp20C[0]=_tmp207;Cyc_List_list(
_tag_arr(_tmp20C,sizeof(void*),2));});_tmp20B;});_tmp20A;}));*_tmp205=({struct
Cyc_Core_Opt*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->v=(void*)_tmp208;
_tmp209;});return 1;}}_LL1B5:;_LL1B6: return 0;_LL1B2:;}_LL1B0:;_LL1B1: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp200);_LL1AD:;}_LL1A4: if(_tmp1F2 <= (void*)3  || *((int*)_tmp1F2)!= 
0)goto _LL1A6;_tmp1F6=((struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f1;_tmp1F7=((
struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f2;_tmp1F8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1F2)->f2;_tmp1F9=((struct Cyc_Absyn_Evar_struct*)
_tmp1F2)->f4;_LL1A5: if(_tmp1F6 == 0  || (void*)_tmp1F6->v != (void*)4)({void*
_tmp20F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp210="effect evar has wrong kind";_tag_arr(_tmp210,sizeof(char),
_get_zero_arr_size(_tmp210,27));}),_tag_arr(_tmp20F,sizeof(void*),0));});if(!
constrain)return 0;{void*_tmp211=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1F9);Cyc_Tcutil_occurs(_tmp211,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F9))->v,r);{struct Cyc_Absyn_JoinEff_struct*
_tmp212=({struct Cyc_Absyn_JoinEff_struct*_tmp214=_cycalloc(sizeof(*_tmp214));
_tmp214[0]=({struct Cyc_Absyn_JoinEff_struct _tmp215;_tmp215.tag=20;_tmp215.f1=({
struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->hd=(void*)
_tmp211;_tmp216->tl=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));
_tmp217->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp218=_cycalloc(
sizeof(*_tmp218));_tmp218[0]=({struct Cyc_Absyn_AccessEff_struct _tmp219;_tmp219.tag=
19;_tmp219.f1=(void*)r;_tmp219;});_tmp218;}));_tmp217->tl=0;_tmp217;});_tmp216;});
_tmp215;});_tmp214;});*_tmp1F8=({struct Cyc_Core_Opt*_tmp213=_cycalloc(sizeof(*
_tmp213));_tmp213->v=(void*)((void*)_tmp212);_tmp213;});return 1;}}_LL1A6:;_LL1A7:
return 0;_LL19D:;}}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*
t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp21A=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(
e));struct Cyc_List_List*_tmp21B;void*_tmp21C;struct Cyc_Core_Opt*_tmp21D;struct
Cyc_Core_Opt*_tmp21E;struct Cyc_Core_Opt**_tmp21F;struct Cyc_Core_Opt*_tmp220;
_LL1B8: if(_tmp21A <= (void*)3  || *((int*)_tmp21A)!= 19)goto _LL1BA;_LL1B9: return 0;
_LL1BA: if(_tmp21A <= (void*)3  || *((int*)_tmp21A)!= 20)goto _LL1BC;_tmp21B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp21A)->f1;_LL1BB: for(0;_tmp21B != 0;_tmp21B=
_tmp21B->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp21B->hd))
return 1;}return 0;_LL1BC: if(_tmp21A <= (void*)3  || *((int*)_tmp21A)!= 21)goto
_LL1BE;_tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp21A)->f1;_LL1BD:
_tmp21C=Cyc_Tcutil_compress(_tmp21C);if(t == _tmp21C)return 1;if(
may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp21C);{void*_tmp221=Cyc_Tcutil_rgns_of(
t);void*_tmp222;_LL1C3: if(_tmp221 <= (void*)3  || *((int*)_tmp221)!= 21)goto _LL1C5;
_tmp222=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp221)->f1;_LL1C4: {struct
_tuple6 _tmp224=({struct _tuple6 _tmp223;_tmp223.f1=Cyc_Tcutil_compress(_tmp222);
_tmp223.f2=_tmp21C;_tmp223;});void*_tmp225;struct Cyc_Absyn_Tvar*_tmp226;void*
_tmp227;struct Cyc_Absyn_Tvar*_tmp228;_LL1C8: _tmp225=_tmp224.f1;if(_tmp225 <= (
void*)3  || *((int*)_tmp225)!= 1)goto _LL1CA;_tmp226=((struct Cyc_Absyn_VarType_struct*)
_tmp225)->f1;_tmp227=_tmp224.f2;if(_tmp227 <= (void*)3  || *((int*)_tmp227)!= 1)
goto _LL1CA;_tmp228=((struct Cyc_Absyn_VarType_struct*)_tmp227)->f1;_LL1C9: return
Cyc_Tcutil_unify(t,_tmp21C);_LL1CA:;_LL1CB: return _tmp222 == _tmp21C;_LL1C7:;}
_LL1C5:;_LL1C6: return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp221);
_LL1C2:;}_LL1BE: if(_tmp21A <= (void*)3  || *((int*)_tmp21A)!= 0)goto _LL1C0;_tmp21D=((
struct Cyc_Absyn_Evar_struct*)_tmp21A)->f1;_tmp21E=((struct Cyc_Absyn_Evar_struct*)
_tmp21A)->f2;_tmp21F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp21A)->f2;_tmp220=((struct Cyc_Absyn_Evar_struct*)_tmp21A)->f4;_LL1BF: if(
_tmp21D == 0  || (void*)_tmp21D->v != (void*)4)({void*_tmp229[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp22A="effect evar has wrong kind";
_tag_arr(_tmp22A,sizeof(char),_get_zero_arr_size(_tmp22A,27));}),_tag_arr(
_tmp229,sizeof(void*),0));});if(!may_constrain_evars)return 0;{void*_tmp22B=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp220);Cyc_Tcutil_occurs(_tmp22B,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp220))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp22C=({struct Cyc_Absyn_JoinEff_struct*_tmp22E=
_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Absyn_JoinEff_struct _tmp22F;
_tmp22F.tag=20;_tmp22F.f1=({struct Cyc_List_List*_tmp230=_cycalloc(sizeof(*
_tmp230));_tmp230->hd=(void*)_tmp22B;_tmp230->tl=({struct Cyc_List_List*_tmp231=
_cycalloc(sizeof(*_tmp231));_tmp231->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp233;_tmp233.tag=21;_tmp233.f1=(void*)t;_tmp233;});_tmp232;}));_tmp231->tl=0;
_tmp231;});_tmp230;});_tmp22F;});_tmp22E;});*_tmp21F=({struct Cyc_Core_Opt*
_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->v=(void*)((void*)_tmp22C);_tmp22D;});
return 1;}}_LL1C0:;_LL1C1: return 0;_LL1B7:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp234=e;struct Cyc_Absyn_Tvar*_tmp235;struct Cyc_List_List*_tmp236;void*
_tmp237;struct Cyc_Core_Opt*_tmp238;struct Cyc_Core_Opt*_tmp239;struct Cyc_Core_Opt**
_tmp23A;struct Cyc_Core_Opt*_tmp23B;_LL1CD: if(_tmp234 <= (void*)3  || *((int*)
_tmp234)!= 1)goto _LL1CF;_tmp235=((struct Cyc_Absyn_VarType_struct*)_tmp234)->f1;
_LL1CE: return Cyc_Absyn_tvar_cmp(v,_tmp235)== 0;_LL1CF: if(_tmp234 <= (void*)3  || *((
int*)_tmp234)!= 20)goto _LL1D1;_tmp236=((struct Cyc_Absyn_JoinEff_struct*)_tmp234)->f1;
_LL1D0: for(0;_tmp236 != 0;_tmp236=_tmp236->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp236->hd))return 1;}return 0;_LL1D1: if(_tmp234 <= (
void*)3  || *((int*)_tmp234)!= 21)goto _LL1D3;_tmp237=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp234)->f1;_LL1D2: {void*_tmp23C=Cyc_Tcutil_rgns_of(_tmp237);void*_tmp23D;
_LL1D8: if(_tmp23C <= (void*)3  || *((int*)_tmp23C)!= 21)goto _LL1DA;_tmp23D=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp23C)->f1;_LL1D9: if(!may_constrain_evars)
return 0;{void*_tmp23E=Cyc_Tcutil_compress(_tmp23D);struct Cyc_Core_Opt*_tmp23F;
struct Cyc_Core_Opt*_tmp240;struct Cyc_Core_Opt**_tmp241;struct Cyc_Core_Opt*
_tmp242;_LL1DD: if(_tmp23E <= (void*)3  || *((int*)_tmp23E)!= 0)goto _LL1DF;_tmp23F=((
struct Cyc_Absyn_Evar_struct*)_tmp23E)->f1;_tmp240=((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f2;_tmp241=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp23E)->f2;_tmp242=((struct Cyc_Absyn_Evar_struct*)_tmp23E)->f4;_LL1DE: {void*
_tmp243=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp242);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp242))->v,v))return 0;{void*_tmp244=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp246=_cycalloc(sizeof(*_tmp246));
_tmp246[0]=({struct Cyc_Absyn_JoinEff_struct _tmp247;_tmp247.tag=20;_tmp247.f1=({
void*_tmp248[2];_tmp248[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp249=
_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_VarType_struct _tmp24A;
_tmp24A.tag=1;_tmp24A.f1=v;_tmp24A;});_tmp249;});_tmp248[0]=_tmp243;Cyc_List_list(
_tag_arr(_tmp248,sizeof(void*),2));});_tmp247;});_tmp246;}));*_tmp241=({struct
Cyc_Core_Opt*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->v=(void*)_tmp244;
_tmp245;});return 1;}}_LL1DF:;_LL1E0: return 0;_LL1DC:;}_LL1DA:;_LL1DB: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp23C);_LL1D7:;}_LL1D3: if(_tmp234 <= (void*)3  || *((int*)
_tmp234)!= 0)goto _LL1D5;_tmp238=((struct Cyc_Absyn_Evar_struct*)_tmp234)->f1;
_tmp239=((struct Cyc_Absyn_Evar_struct*)_tmp234)->f2;_tmp23A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp234)->f2;_tmp23B=((struct Cyc_Absyn_Evar_struct*)
_tmp234)->f4;_LL1D4: if(_tmp238 == 0  || (void*)_tmp238->v != (void*)4)({void*
_tmp24B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp24C="effect evar has wrong kind";_tag_arr(_tmp24C,sizeof(char),
_get_zero_arr_size(_tmp24C,27));}),_tag_arr(_tmp24B,sizeof(void*),0));});{void*
_tmp24D=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp23B);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp23B))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct*
_tmp24E=({struct Cyc_Absyn_JoinEff_struct*_tmp250=_cycalloc(sizeof(*_tmp250));
_tmp250[0]=({struct Cyc_Absyn_JoinEff_struct _tmp251;_tmp251.tag=20;_tmp251.f1=({
struct Cyc_List_List*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->hd=(void*)
_tmp24D;_tmp252->tl=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));
_tmp253->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp254=_cycalloc(
sizeof(*_tmp254));_tmp254[0]=({struct Cyc_Absyn_VarType_struct _tmp255;_tmp255.tag=
1;_tmp255.f1=v;_tmp255;});_tmp254;}));_tmp253->tl=0;_tmp253;});_tmp252;});
_tmp251;});_tmp250;});*_tmp23A=({struct Cyc_Core_Opt*_tmp24F=_cycalloc(sizeof(*
_tmp24F));_tmp24F->v=(void*)((void*)_tmp24E);_tmp24F;});return 1;}}_LL1D5:;_LL1D6:
return 0;_LL1CC:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp256=e;struct Cyc_List_List*_tmp257;void*_tmp258;_LL1E2: if(_tmp256 <= (
void*)3  || *((int*)_tmp256)!= 20)goto _LL1E4;_tmp257=((struct Cyc_Absyn_JoinEff_struct*)
_tmp256)->f1;_LL1E3: for(0;_tmp257 != 0;_tmp257=_tmp257->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp257->hd))return 1;}return 0;_LL1E4: if(_tmp256 <= (void*)3  || *((int*)
_tmp256)!= 21)goto _LL1E6;_tmp258=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp256)->f1;_LL1E5: {void*_tmp259=Cyc_Tcutil_rgns_of(_tmp258);void*_tmp25A;
_LL1EB: if(_tmp259 <= (void*)3  || *((int*)_tmp259)!= 21)goto _LL1ED;_tmp25A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp259)->f1;_LL1EC: return 0;_LL1ED:;_LL1EE:
return Cyc_Tcutil_evar_in_effect(evar,_tmp259);_LL1EA:;}_LL1E6: if(_tmp256 <= (void*)
3  || *((int*)_tmp256)!= 0)goto _LL1E8;_LL1E7: return evar == e;_LL1E8:;_LL1E9: return
0;_LL1E1:;}}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp25B=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp25C;void*_tmp25D;
struct Cyc_Absyn_Tvar*_tmp25E;void*_tmp25F;struct Cyc_Core_Opt*_tmp260;struct Cyc_Core_Opt**
_tmp261;struct Cyc_Core_Opt*_tmp262;_LL1F0: if(_tmp25B <= (void*)3  || *((int*)
_tmp25B)!= 20)goto _LL1F2;_tmp25C=((struct Cyc_Absyn_JoinEff_struct*)_tmp25B)->f1;
_LL1F1: for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp25C->hd,e2))return 0;}return 1;_LL1F2: if(_tmp25B <= (
void*)3  || *((int*)_tmp25B)!= 19)goto _LL1F4;_tmp25D=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp25B)->f1;_LL1F3: return Cyc_Tcutil_region_in_effect(0,_tmp25D,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp25D,(void*)2);_LL1F4: if(_tmp25B <= (
void*)3  || *((int*)_tmp25B)!= 1)goto _LL1F6;_tmp25E=((struct Cyc_Absyn_VarType_struct*)
_tmp25B)->f1;_LL1F5: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,
_tmp25E,e2);_LL1F6: if(_tmp25B <= (void*)3  || *((int*)_tmp25B)!= 21)goto _LL1F8;
_tmp25F=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp25B)->f1;_LL1F7: {void*
_tmp263=Cyc_Tcutil_rgns_of(_tmp25F);void*_tmp264;_LL1FD: if(_tmp263 <= (void*)3
 || *((int*)_tmp263)!= 21)goto _LL1FF;_tmp264=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp263)->f1;_LL1FE: return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp264,
e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp264,Cyc_Absyn_sint_typ);
_LL1FF:;_LL200: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp263,e2);
_LL1FC:;}_LL1F8: if(_tmp25B <= (void*)3  || *((int*)_tmp25B)!= 0)goto _LL1FA;_tmp260=((
struct Cyc_Absyn_Evar_struct*)_tmp25B)->f2;_tmp261=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp25B)->f2;_tmp262=((struct Cyc_Absyn_Evar_struct*)
_tmp25B)->f4;_LL1F9: if(!Cyc_Tcutil_evar_in_effect(e1,e2))*_tmp261=({struct Cyc_Core_Opt*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->v=(void*)Cyc_Absyn_empty_effect;
_tmp265;});return 1;_LL1FA:;_LL1FB:({struct Cyc_String_pa_struct _tmp268;_tmp268.tag=
0;_tmp268.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e1));{
void*_tmp266[1]={& _tmp268};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp267="subset_effect: bad effect: %s";_tag_arr(
_tmp267,sizeof(char),_get_zero_arr_size(_tmp267,30));}),_tag_arr(_tmp266,sizeof(
void*),1));}});_LL1EF:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple6 _tmp26A;void*_tmp26B;void*_tmp26C;struct
_tuple6*_tmp269=(struct _tuple6*)r1->hd;_tmp26A=*_tmp269;_tmp26B=_tmp26A.f1;
_tmp26C=_tmp26A.f2;{int found=_tmp26B == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple6 _tmp26E;void*_tmp26F;void*_tmp270;
struct _tuple6*_tmp26D=(struct _tuple6*)r2->hd;_tmp26E=*_tmp26D;_tmp26F=_tmp26E.f1;
_tmp270=_tmp26E.f2;if(Cyc_Tcutil_unify(_tmp26B,_tmp26F) && Cyc_Tcutil_unify(
_tmp26C,_tmp270)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _tagged_arr)
_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)
return;{void*_tmp271=t1;struct Cyc_Core_Opt*_tmp272;struct Cyc_Core_Opt*_tmp273;
struct Cyc_Core_Opt**_tmp274;struct Cyc_Core_Opt*_tmp275;_LL202: if(_tmp271 <= (void*)
3  || *((int*)_tmp271)!= 0)goto _LL204;_tmp272=((struct Cyc_Absyn_Evar_struct*)
_tmp271)->f1;_tmp273=((struct Cyc_Absyn_Evar_struct*)_tmp271)->f2;_tmp274=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp271)->f2;_tmp275=((struct Cyc_Absyn_Evar_struct*)
_tmp271)->f4;_LL203: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp275))->v,t2);{void*_tmp276=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp276,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp272))->v)){*_tmp274=({struct Cyc_Core_Opt*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277->v=(void*)t2;_tmp277;});return;}else{{void*_tmp278=t2;struct Cyc_Core_Opt*
_tmp279;struct Cyc_Core_Opt**_tmp27A;struct Cyc_Core_Opt*_tmp27B;struct Cyc_Absyn_PtrInfo
_tmp27C;_LL207: if(_tmp278 <= (void*)3  || *((int*)_tmp278)!= 0)goto _LL209;_tmp279=((
struct Cyc_Absyn_Evar_struct*)_tmp278)->f2;_tmp27A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp278)->f2;_tmp27B=((struct Cyc_Absyn_Evar_struct*)
_tmp278)->f4;_LL208: {struct Cyc_List_List*_tmp27D=(struct Cyc_List_List*)_tmp275->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp27B))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp27D,(struct Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=({
const char*_tmp27E="(type variable would escape scope)";_tag_arr(_tmp27E,sizeof(
char),_get_zero_arr_size(_tmp27E,35));});(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp272->v,_tmp276)){*_tmp27A=({struct Cyc_Core_Opt*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->v=(void*)t1;_tmp27F;});return;}Cyc_Tcutil_failure_reason=({
const char*_tmp280="(kinds are incompatible)";_tag_arr(_tmp280,sizeof(char),
_get_zero_arr_size(_tmp280,25));});goto _LL206;}_LL209: if(_tmp278 <= (void*)3  || *((
int*)_tmp278)!= 4)goto _LL20B;_tmp27C=((struct Cyc_Absyn_PointerType_struct*)
_tmp278)->f1;if(!((void*)_tmp272->v == (void*)2))goto _LL20B;_LL20A: {struct Cyc_Absyn_Conref*
_tmp281=Cyc_Absyn_compress_conref((_tmp27C.ptr_atts).bounds);{void*_tmp282=(void*)
_tmp281->v;_LL20E: if((int)_tmp282 != 0)goto _LL210;_LL20F:(void*)(_tmp281->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp283=_cycalloc(sizeof(*_tmp283));
_tmp283[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp284;_tmp284.tag=0;_tmp284.f1=(
void*)Cyc_Absyn_bounds_one;_tmp284;});_tmp283;})));*_tmp274=({struct Cyc_Core_Opt*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->v=(void*)t2;_tmp285;});return;
_LL210:;_LL211: goto _LL20D;_LL20D:;}goto _LL206;}_LL20B:;_LL20C: goto _LL206;_LL206:;}
Cyc_Tcutil_failure_reason=({const char*_tmp286="(kinds are incompatible)";
_tag_arr(_tmp286,sizeof(char),_get_zero_arr_size(_tmp286,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}_LL204:;_LL205: goto _LL201;_LL201:;}{struct _tuple6
_tmp288=({struct _tuple6 _tmp287;_tmp287.f1=t2;_tmp287.f2=t1;_tmp287;});void*
_tmp289;void*_tmp28A;void*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Tvar*_tmp28D;void*
_tmp28E;struct Cyc_Absyn_Tvar*_tmp28F;void*_tmp290;struct Cyc_Absyn_AggrInfo
_tmp291;void*_tmp292;struct Cyc_List_List*_tmp293;void*_tmp294;struct Cyc_Absyn_AggrInfo
_tmp295;void*_tmp296;struct Cyc_List_List*_tmp297;void*_tmp298;struct _tuple1*
_tmp299;void*_tmp29A;struct _tuple1*_tmp29B;void*_tmp29C;struct Cyc_List_List*
_tmp29D;void*_tmp29E;struct Cyc_List_List*_tmp29F;void*_tmp2A0;struct Cyc_Absyn_TunionInfo
_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_Tuniondecl**_tmp2A3;struct Cyc_Absyn_Tuniondecl*
_tmp2A4;struct Cyc_List_List*_tmp2A5;void*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_TunionInfo
_tmp2A8;void*_tmp2A9;struct Cyc_Absyn_Tuniondecl**_tmp2AA;struct Cyc_Absyn_Tuniondecl*
_tmp2AB;struct Cyc_List_List*_tmp2AC;void*_tmp2AD;void*_tmp2AE;struct Cyc_Absyn_TunionFieldInfo
_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_Tuniondecl*_tmp2B1;struct Cyc_Absyn_Tunionfield*
_tmp2B2;struct Cyc_List_List*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_TunionFieldInfo
_tmp2B5;void*_tmp2B6;struct Cyc_Absyn_Tuniondecl*_tmp2B7;struct Cyc_Absyn_Tunionfield*
_tmp2B8;struct Cyc_List_List*_tmp2B9;void*_tmp2BA;struct Cyc_Absyn_PtrInfo _tmp2BB;
void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BD;struct Cyc_Absyn_PtrAtts _tmp2BE;void*
_tmp2BF;struct Cyc_Absyn_Conref*_tmp2C0;struct Cyc_Absyn_Conref*_tmp2C1;struct Cyc_Absyn_Conref*
_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_PtrInfo _tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Tqual
_tmp2C6;struct Cyc_Absyn_PtrAtts _tmp2C7;void*_tmp2C8;struct Cyc_Absyn_Conref*
_tmp2C9;struct Cyc_Absyn_Conref*_tmp2CA;struct Cyc_Absyn_Conref*_tmp2CB;void*
_tmp2CC;void*_tmp2CD;void*_tmp2CE;void*_tmp2CF;void*_tmp2D0;void*_tmp2D1;void*
_tmp2D2;void*_tmp2D3;void*_tmp2D4;int _tmp2D5;void*_tmp2D6;int _tmp2D7;void*
_tmp2D8;void*_tmp2D9;void*_tmp2DA;void*_tmp2DB;void*_tmp2DC;int _tmp2DD;void*
_tmp2DE;int _tmp2DF;void*_tmp2E0;void*_tmp2E1;void*_tmp2E2;void*_tmp2E3;void*
_tmp2E4;struct Cyc_Absyn_ArrayInfo _tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Tqual
_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Conref*_tmp2E9;void*_tmp2EA;
struct Cyc_Absyn_ArrayInfo _tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Tqual _tmp2ED;
struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_Absyn_Conref*_tmp2EF;void*_tmp2F0;struct
Cyc_Absyn_FnInfo _tmp2F1;struct Cyc_List_List*_tmp2F2;struct Cyc_Core_Opt*_tmp2F3;
void*_tmp2F4;struct Cyc_List_List*_tmp2F5;int _tmp2F6;struct Cyc_Absyn_VarargInfo*
_tmp2F7;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F9;void*_tmp2FA;
struct Cyc_Absyn_FnInfo _tmp2FB;struct Cyc_List_List*_tmp2FC;struct Cyc_Core_Opt*
_tmp2FD;void*_tmp2FE;struct Cyc_List_List*_tmp2FF;int _tmp300;struct Cyc_Absyn_VarargInfo*
_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp303;void*_tmp304;
struct Cyc_List_List*_tmp305;void*_tmp306;struct Cyc_List_List*_tmp307;void*
_tmp308;void*_tmp309;struct Cyc_List_List*_tmp30A;void*_tmp30B;void*_tmp30C;
struct Cyc_List_List*_tmp30D;void*_tmp30E;void*_tmp30F;void*_tmp310;void*_tmp311;
void*_tmp312;void*_tmp313;void*_tmp314;void*_tmp315;void*_tmp316;void*_tmp317;
void*_tmp318;void*_tmp319;_LL213: _tmp289=_tmp288.f1;if(_tmp289 <= (void*)3  || *((
int*)_tmp289)!= 0)goto _LL215;_LL214: Cyc_Tcutil_unify_it(t2,t1);return;_LL215:
_tmp28A=_tmp288.f1;if((int)_tmp28A != 0)goto _LL217;_tmp28B=_tmp288.f2;if((int)
_tmp28B != 0)goto _LL217;_LL216: return;_LL217: _tmp28C=_tmp288.f1;if(_tmp28C <= (
void*)3  || *((int*)_tmp28C)!= 1)goto _LL219;_tmp28D=((struct Cyc_Absyn_VarType_struct*)
_tmp28C)->f1;_tmp28E=_tmp288.f2;if(_tmp28E <= (void*)3  || *((int*)_tmp28E)!= 1)
goto _LL219;_tmp28F=((struct Cyc_Absyn_VarType_struct*)_tmp28E)->f1;_LL218: {
struct _tagged_arr*_tmp31A=_tmp28D->name;struct _tagged_arr*_tmp31B=_tmp28F->name;
int _tmp31C=*((int*)_check_null(_tmp28D->identity));int _tmp31D=*((int*)
_check_null(_tmp28F->identity));void*_tmp31E=Cyc_Tcutil_tvar_kind(_tmp28D);void*
_tmp31F=Cyc_Tcutil_tvar_kind(_tmp28F);if(_tmp31D == _tmp31C  && Cyc_strptrcmp(
_tmp31A,_tmp31B)== 0){if(_tmp31E != _tmp31F)({struct Cyc_String_pa_struct _tmp324;
_tmp324.tag=0;_tmp324.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp31F));{struct Cyc_String_pa_struct _tmp323;_tmp323.tag=0;_tmp323.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp31E));{struct Cyc_String_pa_struct
_tmp322;_tmp322.tag=0;_tmp322.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp31A);{
void*_tmp320[3]={& _tmp322,& _tmp323,& _tmp324};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp321="same type variable %s has kinds %s and %s";
_tag_arr(_tmp321,sizeof(char),_get_zero_arr_size(_tmp321,42));}),_tag_arr(
_tmp320,sizeof(void*),3));}}}});return;}Cyc_Tcutil_failure_reason=({const char*
_tmp325="(variable types are not the same)";_tag_arr(_tmp325,sizeof(char),
_get_zero_arr_size(_tmp325,34));});goto _LL212;}_LL219: _tmp290=_tmp288.f1;if(
_tmp290 <= (void*)3  || *((int*)_tmp290)!= 10)goto _LL21B;_tmp291=((struct Cyc_Absyn_AggrType_struct*)
_tmp290)->f1;_tmp292=(void*)_tmp291.aggr_info;_tmp293=_tmp291.targs;_tmp294=
_tmp288.f2;if(_tmp294 <= (void*)3  || *((int*)_tmp294)!= 10)goto _LL21B;_tmp295=((
struct Cyc_Absyn_AggrType_struct*)_tmp294)->f1;_tmp296=(void*)_tmp295.aggr_info;
_tmp297=_tmp295.targs;_LL21A: {void*_tmp327;struct _tuple1*_tmp328;struct _tuple5
_tmp326=Cyc_Absyn_aggr_kinded_name(_tmp296);_tmp327=_tmp326.f1;_tmp328=_tmp326.f2;{
void*_tmp32A;struct _tuple1*_tmp32B;struct _tuple5 _tmp329=Cyc_Absyn_aggr_kinded_name(
_tmp292);_tmp32A=_tmp329.f1;_tmp32B=_tmp329.f2;if(_tmp327 != _tmp32A){Cyc_Tcutil_failure_reason=({
const char*_tmp32C="(struct and union type)";_tag_arr(_tmp32C,sizeof(char),
_get_zero_arr_size(_tmp32C,24));});goto _LL212;}if(Cyc_Absyn_qvar_cmp(_tmp328,
_tmp32B)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32D="(different type name)";
_tag_arr(_tmp32D,sizeof(char),_get_zero_arr_size(_tmp32D,22));});goto _LL212;}Cyc_Tcutil_unify_list(
_tmp297,_tmp293);return;}}_LL21B: _tmp298=_tmp288.f1;if(_tmp298 <= (void*)3  || *((
int*)_tmp298)!= 12)goto _LL21D;_tmp299=((struct Cyc_Absyn_EnumType_struct*)_tmp298)->f1;
_tmp29A=_tmp288.f2;if(_tmp29A <= (void*)3  || *((int*)_tmp29A)!= 12)goto _LL21D;
_tmp29B=((struct Cyc_Absyn_EnumType_struct*)_tmp29A)->f1;_LL21C: if(Cyc_Absyn_qvar_cmp(
_tmp299,_tmp29B)== 0)return;Cyc_Tcutil_failure_reason=({const char*_tmp32E="(different enum types)";
_tag_arr(_tmp32E,sizeof(char),_get_zero_arr_size(_tmp32E,23));});goto _LL212;
_LL21D: _tmp29C=_tmp288.f1;if(_tmp29C <= (void*)3  || *((int*)_tmp29C)!= 13)goto
_LL21F;_tmp29D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp29C)->f1;_tmp29E=
_tmp288.f2;if(_tmp29E <= (void*)3  || *((int*)_tmp29E)!= 13)goto _LL21F;_tmp29F=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp29E)->f1;_LL21E: {int bad=0;for(0;
_tmp29D != 0  && _tmp29F != 0;(_tmp29D=_tmp29D->tl,_tmp29F=_tmp29F->tl)){struct Cyc_Absyn_Enumfield*
_tmp32F=(struct Cyc_Absyn_Enumfield*)_tmp29D->hd;struct Cyc_Absyn_Enumfield*
_tmp330=(struct Cyc_Absyn_Enumfield*)_tmp29F->hd;if(Cyc_Absyn_qvar_cmp(_tmp32F->name,
_tmp330->name)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp331="(different names for enum fields)";
_tag_arr(_tmp331,sizeof(char),_get_zero_arr_size(_tmp331,34));});bad=1;break;}
if(_tmp32F->tag == _tmp330->tag)continue;if(_tmp32F->tag == 0  || _tmp330->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp332="(different tag values for enum fields)";
_tag_arr(_tmp332,sizeof(char),_get_zero_arr_size(_tmp332,39));});bad=1;break;}
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp32F->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp330->tag))){Cyc_Tcutil_failure_reason=({
const char*_tmp333="(different tag values for enum fields)";_tag_arr(_tmp333,
sizeof(char),_get_zero_arr_size(_tmp333,39));});bad=1;break;}}if(bad)goto _LL212;
if(_tmp29D == 0  && _tmp29F == 0)return;Cyc_Tcutil_failure_reason=({const char*
_tmp334="(different number of fields for enums)";_tag_arr(_tmp334,sizeof(char),
_get_zero_arr_size(_tmp334,39));});goto _LL212;}_LL21F: _tmp2A0=_tmp288.f1;if(
_tmp2A0 <= (void*)3  || *((int*)_tmp2A0)!= 2)goto _LL221;_tmp2A1=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A0)->f1;_tmp2A2=(void*)_tmp2A1.tunion_info;if(*((int*)_tmp2A2)!= 1)goto
_LL221;_tmp2A3=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A2)->f1;_tmp2A4=*
_tmp2A3;_tmp2A5=_tmp2A1.targs;_tmp2A6=(void*)_tmp2A1.rgn;_tmp2A7=_tmp288.f2;if(
_tmp2A7 <= (void*)3  || *((int*)_tmp2A7)!= 2)goto _LL221;_tmp2A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A7)->f1;_tmp2A9=(void*)_tmp2A8.tunion_info;if(*((int*)_tmp2A9)!= 1)goto
_LL221;_tmp2AA=((struct Cyc_Absyn_KnownTunion_struct*)_tmp2A9)->f1;_tmp2AB=*
_tmp2AA;_tmp2AC=_tmp2A8.targs;_tmp2AD=(void*)_tmp2A8.rgn;_LL220: if(_tmp2A4 == 
_tmp2AB  || Cyc_Absyn_qvar_cmp(_tmp2A4->name,_tmp2AB->name)== 0){Cyc_Tcutil_unify_it(
_tmp2AD,_tmp2A6);Cyc_Tcutil_unify_list(_tmp2AC,_tmp2A5);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp335="(different tunion types)";_tag_arr(_tmp335,sizeof(char),
_get_zero_arr_size(_tmp335,25));});goto _LL212;_LL221: _tmp2AE=_tmp288.f1;if(
_tmp2AE <= (void*)3  || *((int*)_tmp2AE)!= 3)goto _LL223;_tmp2AF=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2AE)->f1;_tmp2B0=(void*)_tmp2AF.field_info;if(*((int*)_tmp2B0)!= 1)goto
_LL223;_tmp2B1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B0)->f1;_tmp2B2=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B0)->f2;_tmp2B3=_tmp2AF.targs;
_tmp2B4=_tmp288.f2;if(_tmp2B4 <= (void*)3  || *((int*)_tmp2B4)!= 3)goto _LL223;
_tmp2B5=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2B4)->f1;_tmp2B6=(void*)
_tmp2B5.field_info;if(*((int*)_tmp2B6)!= 1)goto _LL223;_tmp2B7=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp2B6)->f1;_tmp2B8=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp2B6)->f2;
_tmp2B9=_tmp2B5.targs;_LL222: if((_tmp2B1 == _tmp2B7  || Cyc_Absyn_qvar_cmp(_tmp2B1->name,
_tmp2B7->name)== 0) && (_tmp2B2 == _tmp2B8  || Cyc_Absyn_qvar_cmp(_tmp2B2->name,
_tmp2B8->name)== 0)){Cyc_Tcutil_unify_list(_tmp2B9,_tmp2B3);return;}Cyc_Tcutil_failure_reason=({
const char*_tmp336="(different tunion field types)";_tag_arr(_tmp336,sizeof(char),
_get_zero_arr_size(_tmp336,31));});goto _LL212;_LL223: _tmp2BA=_tmp288.f1;if(
_tmp2BA <= (void*)3  || *((int*)_tmp2BA)!= 4)goto _LL225;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BA)->f1;_tmp2BC=(void*)_tmp2BB.elt_typ;_tmp2BD=_tmp2BB.elt_tq;_tmp2BE=
_tmp2BB.ptr_atts;_tmp2BF=(void*)_tmp2BE.rgn;_tmp2C0=_tmp2BE.nullable;_tmp2C1=
_tmp2BE.bounds;_tmp2C2=_tmp2BE.zero_term;_tmp2C3=_tmp288.f2;if(_tmp2C3 <= (void*)
3  || *((int*)_tmp2C3)!= 4)goto _LL225;_tmp2C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C3)->f1;_tmp2C5=(void*)_tmp2C4.elt_typ;_tmp2C6=_tmp2C4.elt_tq;_tmp2C7=
_tmp2C4.ptr_atts;_tmp2C8=(void*)_tmp2C7.rgn;_tmp2C9=_tmp2C7.nullable;_tmp2CA=
_tmp2C7.bounds;_tmp2CB=_tmp2C7.zero_term;_LL224: Cyc_Tcutil_unify_it(_tmp2C5,
_tmp2BC);Cyc_Tcutil_unify_it(_tmp2BF,_tmp2C8);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2CB,
_tmp2C2,({const char*_tmp337="(not both zero terminated)";_tag_arr(_tmp337,
sizeof(char),_get_zero_arr_size(_tmp337,27));}));Cyc_Tcutil_unify_tqual(_tmp2C6,
_tmp2C5,_tmp2BD,_tmp2BC);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp2CA,_tmp2C1,({const char*_tmp338="(different pointer bounds)";_tag_arr(
_tmp338,sizeof(char),_get_zero_arr_size(_tmp338,27));}));{void*_tmp339=(void*)(
Cyc_Absyn_compress_conref(_tmp2CA))->v;void*_tmp33A;_LL24C: if(_tmp339 <= (void*)1
 || *((int*)_tmp339)!= 0)goto _LL24E;_tmp33A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp339)->f1;if((int)_tmp33A != 0)goto _LL24E;_LL24D: return;_LL24E:;_LL24F: goto
_LL24B;_LL24B:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2C9,
_tmp2C0,({const char*_tmp33B="(incompatible pointer types)";_tag_arr(_tmp33B,
sizeof(char),_get_zero_arr_size(_tmp33B,29));}));return;_LL225: _tmp2CC=_tmp288.f1;
if(_tmp2CC <= (void*)3  || *((int*)_tmp2CC)!= 5)goto _LL227;_tmp2CD=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp2CC)->f1;_tmp2CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2CC)->f2;_tmp2CF=_tmp288.f2;if(_tmp2CF <= (void*)3  || *((int*)_tmp2CF)!= 5)
goto _LL227;_tmp2D0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CF)->f1;_tmp2D1=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp2CF)->f2;_LL226: if(_tmp2D0 == _tmp2CD
 && _tmp2D1 == _tmp2CE)return;Cyc_Tcutil_failure_reason=({const char*_tmp33C="(different integral types)";
_tag_arr(_tmp33C,sizeof(char),_get_zero_arr_size(_tmp33C,27));});goto _LL212;
_LL227: _tmp2D2=_tmp288.f1;if((int)_tmp2D2 != 1)goto _LL229;_tmp2D3=_tmp288.f2;if((
int)_tmp2D3 != 1)goto _LL229;_LL228: return;_LL229: _tmp2D4=_tmp288.f1;if(_tmp2D4 <= (
void*)3  || *((int*)_tmp2D4)!= 6)goto _LL22B;_tmp2D5=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2D4)->f1;_tmp2D6=_tmp288.f2;if(_tmp2D6 <= (void*)3  || *((int*)_tmp2D6)!= 6)
goto _LL22B;_tmp2D7=((struct Cyc_Absyn_DoubleType_struct*)_tmp2D6)->f1;_LL22A: if(
_tmp2D5 == _tmp2D7)return;goto _LL212;_LL22B: _tmp2D8=_tmp288.f1;if(_tmp2D8 <= (void*)
3  || *((int*)_tmp2D8)!= 14)goto _LL22D;_tmp2D9=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2D8)->f1;_tmp2DA=_tmp288.f2;if(_tmp2DA <= (void*)3  || *((int*)_tmp2DA)!= 14)
goto _LL22D;_tmp2DB=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2DA)->f1;
_LL22C: Cyc_Tcutil_unify_it(_tmp2D9,_tmp2DB);return;_LL22D: _tmp2DC=_tmp288.f1;if(
_tmp2DC <= (void*)3  || *((int*)_tmp2DC)!= 18)goto _LL22F;_tmp2DD=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2DC)->f1;_tmp2DE=_tmp288.f2;if(_tmp2DE <= (void*)3  || *((int*)_tmp2DE)!= 18)
goto _LL22F;_tmp2DF=((struct Cyc_Absyn_TypeInt_struct*)_tmp2DE)->f1;_LL22E: if(
_tmp2DD == _tmp2DF)return;Cyc_Tcutil_failure_reason=({const char*_tmp33D="(different type integers)";
_tag_arr(_tmp33D,sizeof(char),_get_zero_arr_size(_tmp33D,26));});goto _LL212;
_LL22F: _tmp2E0=_tmp288.f1;if(_tmp2E0 <= (void*)3  || *((int*)_tmp2E0)!= 17)goto
_LL231;_tmp2E1=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E0)->f1;_tmp2E2=
_tmp288.f2;if(_tmp2E2 <= (void*)3  || *((int*)_tmp2E2)!= 17)goto _LL231;_tmp2E3=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp2E2)->f1;_LL230: Cyc_Tcutil_unify_it(
_tmp2E1,_tmp2E3);return;_LL231: _tmp2E4=_tmp288.f1;if(_tmp2E4 <= (void*)3  || *((
int*)_tmp2E4)!= 7)goto _LL233;_tmp2E5=((struct Cyc_Absyn_ArrayType_struct*)_tmp2E4)->f1;
_tmp2E6=(void*)_tmp2E5.elt_type;_tmp2E7=_tmp2E5.tq;_tmp2E8=_tmp2E5.num_elts;
_tmp2E9=_tmp2E5.zero_term;_tmp2EA=_tmp288.f2;if(_tmp2EA <= (void*)3  || *((int*)
_tmp2EA)!= 7)goto _LL233;_tmp2EB=((struct Cyc_Absyn_ArrayType_struct*)_tmp2EA)->f1;
_tmp2EC=(void*)_tmp2EB.elt_type;_tmp2ED=_tmp2EB.tq;_tmp2EE=_tmp2EB.num_elts;
_tmp2EF=_tmp2EB.zero_term;_LL232: Cyc_Tcutil_unify_it(_tmp2EC,_tmp2E6);Cyc_Tcutil_unify_tqual(
_tmp2ED,_tmp2EC,_tmp2E7,_tmp2E6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp2E9,
_tmp2EF,({const char*_tmp33E="(not both zero terminated)";_tag_arr(_tmp33E,
sizeof(char),_get_zero_arr_size(_tmp33E,27));}));if(_tmp2E8 == _tmp2EE)return;if(
_tmp2E8 == 0  || _tmp2EE == 0)goto _LL212;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)
_tmp2E8,(struct Cyc_Absyn_Exp*)_tmp2EE))return;Cyc_Tcutil_failure_reason=({const
char*_tmp33F="(different array sizes)";_tag_arr(_tmp33F,sizeof(char),
_get_zero_arr_size(_tmp33F,24));});goto _LL212;_LL233: _tmp2F0=_tmp288.f1;if(
_tmp2F0 <= (void*)3  || *((int*)_tmp2F0)!= 8)goto _LL235;_tmp2F1=((struct Cyc_Absyn_FnType_struct*)
_tmp2F0)->f1;_tmp2F2=_tmp2F1.tvars;_tmp2F3=_tmp2F1.effect;_tmp2F4=(void*)_tmp2F1.ret_typ;
_tmp2F5=_tmp2F1.args;_tmp2F6=_tmp2F1.c_varargs;_tmp2F7=_tmp2F1.cyc_varargs;
_tmp2F8=_tmp2F1.rgn_po;_tmp2F9=_tmp2F1.attributes;_tmp2FA=_tmp288.f2;if(_tmp2FA
<= (void*)3  || *((int*)_tmp2FA)!= 8)goto _LL235;_tmp2FB=((struct Cyc_Absyn_FnType_struct*)
_tmp2FA)->f1;_tmp2FC=_tmp2FB.tvars;_tmp2FD=_tmp2FB.effect;_tmp2FE=(void*)_tmp2FB.ret_typ;
_tmp2FF=_tmp2FB.args;_tmp300=_tmp2FB.c_varargs;_tmp301=_tmp2FB.cyc_varargs;
_tmp302=_tmp2FB.rgn_po;_tmp303=_tmp2FB.attributes;_LL234: {int done=0;{struct
_RegionHandle _tmp340=_new_region("rgn");struct _RegionHandle*rgn=& _tmp340;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp2FC != 0){if(_tmp2F2 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp341="(second function type has too few type variables)";
_tag_arr(_tmp341,sizeof(char),_get_zero_arr_size(_tmp341,50));});(int)_throw((
void*)Cyc_Tcutil_Unify);}{void*_tmp342=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2FC->hd);void*_tmp343=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2F2->hd);
if(_tmp342 != _tmp343){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp348;_tmp348.tag=0;_tmp348.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp343));{struct Cyc_String_pa_struct _tmp347;_tmp347.tag=0;_tmp347.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp342));{struct Cyc_String_pa_struct
_tmp346;_tmp346.tag=0;_tmp346.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp2FC->hd));{void*_tmp344[3]={& _tmp346,& _tmp347,& _tmp348};
Cyc_aprintf(({const char*_tmp345="(type var %s has different kinds %s and %s)";
_tag_arr(_tmp345,sizeof(char),_get_zero_arr_size(_tmp345,44));}),_tag_arr(
_tmp344,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}inst=({
struct Cyc_List_List*_tmp349=_region_malloc(rgn,sizeof(*_tmp349));_tmp349->hd=({
struct _tuple8*_tmp34A=_region_malloc(rgn,sizeof(*_tmp34A));_tmp34A->f1=(struct
Cyc_Absyn_Tvar*)_tmp2F2->hd;_tmp34A->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B[0]=({struct Cyc_Absyn_VarType_struct
_tmp34C;_tmp34C.tag=1;_tmp34C.f1=(struct Cyc_Absyn_Tvar*)_tmp2FC->hd;_tmp34C;});
_tmp34B;});_tmp34A;});_tmp349->tl=inst;_tmp349;});_tmp2FC=_tmp2FC->tl;_tmp2F2=
_tmp2F2->tl;}}if(_tmp2F2 != 0){Cyc_Tcutil_failure_reason=({const char*_tmp34D="(second function type has too many type variables)";
_tag_arr(_tmp34D,sizeof(char),_get_zero_arr_size(_tmp34D,51));});_npop_handler(0);
goto _LL212;}if(inst != 0){Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_struct*
_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=({struct Cyc_Absyn_FnType_struct
_tmp34F;_tmp34F.tag=8;_tmp34F.f1=({struct Cyc_Absyn_FnInfo _tmp350;_tmp350.tvars=0;
_tmp350.effect=_tmp2FD;_tmp350.ret_typ=(void*)_tmp2FE;_tmp350.args=_tmp2FF;
_tmp350.c_varargs=_tmp300;_tmp350.cyc_varargs=_tmp301;_tmp350.rgn_po=_tmp302;
_tmp350.attributes=_tmp303;_tmp350;});_tmp34F;});_tmp34E;}),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*_tmp351=_cycalloc(sizeof(*
_tmp351));_tmp351[0]=({struct Cyc_Absyn_FnType_struct _tmp352;_tmp352.tag=8;
_tmp352.f1=({struct Cyc_Absyn_FnInfo _tmp353;_tmp353.tvars=0;_tmp353.effect=
_tmp2F3;_tmp353.ret_typ=(void*)_tmp2F4;_tmp353.args=_tmp2F5;_tmp353.c_varargs=
_tmp2F6;_tmp353.cyc_varargs=_tmp2F7;_tmp353.rgn_po=_tmp2F8;_tmp353.attributes=
_tmp2F9;_tmp353;});_tmp352;});_tmp351;})));done=1;}};_pop_region(rgn);}if(done)
return;Cyc_Tcutil_unify_it(_tmp2FE,_tmp2F4);for(0;_tmp2FF != 0  && _tmp2F5 != 0;(
_tmp2FF=_tmp2FF->tl,_tmp2F5=_tmp2F5->tl)){struct Cyc_Absyn_Tqual _tmp355;void*
_tmp356;struct _tuple2 _tmp354=*((struct _tuple2*)_tmp2FF->hd);_tmp355=_tmp354.f2;
_tmp356=_tmp354.f3;{struct Cyc_Absyn_Tqual _tmp358;void*_tmp359;struct _tuple2
_tmp357=*((struct _tuple2*)_tmp2F5->hd);_tmp358=_tmp357.f2;_tmp359=_tmp357.f3;Cyc_Tcutil_unify_it(
_tmp356,_tmp359);Cyc_Tcutil_unify_tqual(_tmp355,_tmp356,_tmp358,_tmp359);}}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp2FF != 0  || _tmp2F5 != 0){Cyc_Tcutil_failure_reason=({
const char*_tmp35A="(function types have different number of arguments)";_tag_arr(
_tmp35A,sizeof(char),_get_zero_arr_size(_tmp35A,52));});goto _LL212;}if(_tmp300 != 
_tmp2F6){Cyc_Tcutil_failure_reason=({const char*_tmp35B="(only one function type takes C varargs)";
_tag_arr(_tmp35B,sizeof(char),_get_zero_arr_size(_tmp35B,41));});goto _LL212;}{
int bad_cyc_vararg=0;{struct _tuple11 _tmp35D=({struct _tuple11 _tmp35C;_tmp35C.f1=
_tmp301;_tmp35C.f2=_tmp2F7;_tmp35C;});struct Cyc_Absyn_VarargInfo*_tmp35E;struct
Cyc_Absyn_VarargInfo*_tmp35F;struct Cyc_Absyn_VarargInfo*_tmp360;struct Cyc_Absyn_VarargInfo*
_tmp361;struct Cyc_Absyn_VarargInfo*_tmp362;struct Cyc_Absyn_VarargInfo _tmp363;
struct Cyc_Core_Opt*_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;int _tmp367;
struct Cyc_Absyn_VarargInfo*_tmp368;struct Cyc_Absyn_VarargInfo _tmp369;struct Cyc_Core_Opt*
_tmp36A;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36C;int _tmp36D;_LL251: _tmp35E=
_tmp35D.f1;if(_tmp35E != 0)goto _LL253;_tmp35F=_tmp35D.f2;if(_tmp35F != 0)goto
_LL253;_LL252: goto _LL250;_LL253: _tmp360=_tmp35D.f1;if(_tmp360 != 0)goto _LL255;
_LL254: goto _LL256;_LL255: _tmp361=_tmp35D.f2;if(_tmp361 != 0)goto _LL257;_LL256:
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*_tmp36E="(only one function type takes varargs)";
_tag_arr(_tmp36E,sizeof(char),_get_zero_arr_size(_tmp36E,39));});goto _LL250;
_LL257: _tmp362=_tmp35D.f1;if(_tmp362 == 0)goto _LL250;_tmp363=*_tmp362;_tmp364=
_tmp363.name;_tmp365=_tmp363.tq;_tmp366=(void*)_tmp363.type;_tmp367=_tmp363.inject;
_tmp368=_tmp35D.f2;if(_tmp368 == 0)goto _LL250;_tmp369=*_tmp368;_tmp36A=_tmp369.name;
_tmp36B=_tmp369.tq;_tmp36C=(void*)_tmp369.type;_tmp36D=_tmp369.inject;_LL258: Cyc_Tcutil_unify_it(
_tmp366,_tmp36C);Cyc_Tcutil_unify_tqual(_tmp365,_tmp366,_tmp36B,_tmp36C);if(
_tmp367 != _tmp36D){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=({const char*
_tmp36F="(only one function type injects varargs)";_tag_arr(_tmp36F,sizeof(char),
_get_zero_arr_size(_tmp36F,41));});}goto _LL250;_LL250:;}if(bad_cyc_vararg)goto
_LL212;{int bad_effect=0;{struct _tuple12 _tmp371=({struct _tuple12 _tmp370;_tmp370.f1=
_tmp2FD;_tmp370.f2=_tmp2F3;_tmp370;});struct Cyc_Core_Opt*_tmp372;struct Cyc_Core_Opt*
_tmp373;struct Cyc_Core_Opt*_tmp374;struct Cyc_Core_Opt*_tmp375;_LL25A: _tmp372=
_tmp371.f1;if(_tmp372 != 0)goto _LL25C;_tmp373=_tmp371.f2;if(_tmp373 != 0)goto
_LL25C;_LL25B: goto _LL259;_LL25C: _tmp374=_tmp371.f1;if(_tmp374 != 0)goto _LL25E;
_LL25D: goto _LL25F;_LL25E: _tmp375=_tmp371.f2;if(_tmp375 != 0)goto _LL260;_LL25F:
bad_effect=1;goto _LL259;_LL260:;_LL261: bad_effect=!Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp2FD))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2F3))->v);goto _LL259;_LL259:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=({const char*_tmp376="(function type effects do not unify)";
_tag_arr(_tmp376,sizeof(char),_get_zero_arr_size(_tmp376,37));});goto _LL212;}if(
!Cyc_Tcutil_same_atts(_tmp2F9,_tmp303)){Cyc_Tcutil_failure_reason=({const char*
_tmp377="(function types have different attributes)";_tag_arr(_tmp377,sizeof(
char),_get_zero_arr_size(_tmp377,43));});goto _LL212;}if(!Cyc_Tcutil_same_rgn_po(
_tmp2F8,_tmp302)){Cyc_Tcutil_failure_reason=({const char*_tmp378="(function types have different region lifetime orderings)";
_tag_arr(_tmp378,sizeof(char),_get_zero_arr_size(_tmp378,58));});goto _LL212;}
return;}}}_LL235: _tmp304=_tmp288.f1;if(_tmp304 <= (void*)3  || *((int*)_tmp304)!= 
9)goto _LL237;_tmp305=((struct Cyc_Absyn_TupleType_struct*)_tmp304)->f1;_tmp306=
_tmp288.f2;if(_tmp306 <= (void*)3  || *((int*)_tmp306)!= 9)goto _LL237;_tmp307=((
struct Cyc_Absyn_TupleType_struct*)_tmp306)->f1;_LL236: for(0;_tmp307 != 0  && 
_tmp305 != 0;(_tmp307=_tmp307->tl,_tmp305=_tmp305->tl)){struct Cyc_Absyn_Tqual
_tmp37A;void*_tmp37B;struct _tuple4 _tmp379=*((struct _tuple4*)_tmp307->hd);_tmp37A=
_tmp379.f1;_tmp37B=_tmp379.f2;{struct Cyc_Absyn_Tqual _tmp37D;void*_tmp37E;struct
_tuple4 _tmp37C=*((struct _tuple4*)_tmp305->hd);_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;
Cyc_Tcutil_unify_it(_tmp37B,_tmp37E);Cyc_Tcutil_unify_tqual(_tmp37A,_tmp37B,
_tmp37D,_tmp37E);}}if(_tmp307 == 0  && _tmp305 == 0)return;Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp37F="(tuple types have different numbers of components)";
_tag_arr(_tmp37F,sizeof(char),_get_zero_arr_size(_tmp37F,51));});goto _LL212;
_LL237: _tmp308=_tmp288.f1;if(_tmp308 <= (void*)3  || *((int*)_tmp308)!= 11)goto
_LL239;_tmp309=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp308)->f1;
_tmp30A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp308)->f2;_tmp30B=_tmp288.f2;
if(_tmp30B <= (void*)3  || *((int*)_tmp30B)!= 11)goto _LL239;_tmp30C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp30B)->f1;_tmp30D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp30B)->f2;_LL238: if(_tmp30C != _tmp309){Cyc_Tcutil_failure_reason=({const char*
_tmp380="(struct and union type)";_tag_arr(_tmp380,sizeof(char),
_get_zero_arr_size(_tmp380,24));});goto _LL212;}for(0;_tmp30D != 0  && _tmp30A != 0;(
_tmp30D=_tmp30D->tl,_tmp30A=_tmp30A->tl)){struct Cyc_Absyn_Aggrfield*_tmp381=(
struct Cyc_Absyn_Aggrfield*)_tmp30D->hd;struct Cyc_Absyn_Aggrfield*_tmp382=(struct
Cyc_Absyn_Aggrfield*)_tmp30A->hd;if(Cyc_strptrcmp(_tmp381->name,_tmp382->name)!= 
0){Cyc_Tcutil_failure_reason=({const char*_tmp383="(different member names)";
_tag_arr(_tmp383,sizeof(char),_get_zero_arr_size(_tmp383,25));});(int)_throw((
void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp381->type,(void*)_tmp382->type);
Cyc_Tcutil_unify_tqual(_tmp381->tq,(void*)_tmp381->type,_tmp382->tq,(void*)
_tmp382->type);if(!Cyc_Tcutil_same_atts(_tmp381->attributes,_tmp382->attributes)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({
const char*_tmp384="(different attributes on member)";_tag_arr(_tmp384,sizeof(
char),_get_zero_arr_size(_tmp384,33));});(int)_throw((void*)Cyc_Tcutil_Unify);}
if((_tmp381->width != 0  && _tmp382->width == 0  || _tmp382->width != 0  && _tmp381->width
== 0) || (_tmp381->width != 0  && _tmp382->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp381->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp382->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp385="(different bitfield widths on member)";
_tag_arr(_tmp385,sizeof(char),_get_zero_arr_size(_tmp385,38));});(int)_throw((
void*)Cyc_Tcutil_Unify);}}if(_tmp30D == 0  && _tmp30A == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=({const char*_tmp386="(different number of members)";
_tag_arr(_tmp386,sizeof(char),_get_zero_arr_size(_tmp386,30));});goto _LL212;
_LL239: _tmp30E=_tmp288.f1;if((int)_tmp30E != 2)goto _LL23B;_tmp30F=_tmp288.f2;if((
int)_tmp30F != 2)goto _LL23B;_LL23A: return;_LL23B: _tmp310=_tmp288.f1;if(_tmp310 <= (
void*)3  || *((int*)_tmp310)!= 15)goto _LL23D;_tmp311=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp310)->f1;_tmp312=_tmp288.f2;if(_tmp312 <= (void*)3  || *((int*)_tmp312)!= 15)
goto _LL23D;_tmp313=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp312)->f1;
_LL23C: Cyc_Tcutil_unify_it(_tmp311,_tmp313);return;_LL23D: _tmp314=_tmp288.f1;if(
_tmp314 <= (void*)3  || *((int*)_tmp314)!= 20)goto _LL23F;_LL23E: goto _LL240;_LL23F:
_tmp315=_tmp288.f2;if(_tmp315 <= (void*)3  || *((int*)_tmp315)!= 20)goto _LL241;
_LL240: goto _LL242;_LL241: _tmp316=_tmp288.f1;if(_tmp316 <= (void*)3  || *((int*)
_tmp316)!= 19)goto _LL243;_LL242: goto _LL244;_LL243: _tmp317=_tmp288.f1;if(_tmp317
<= (void*)3  || *((int*)_tmp317)!= 21)goto _LL245;_LL244: goto _LL246;_LL245: _tmp318=
_tmp288.f2;if(_tmp318 <= (void*)3  || *((int*)_tmp318)!= 21)goto _LL247;_LL246: goto
_LL248;_LL247: _tmp319=_tmp288.f2;if(_tmp319 <= (void*)3  || *((int*)_tmp319)!= 19)
goto _LL249;_LL248: if(Cyc_Tcutil_unify_effect(t1,t2))return;Cyc_Tcutil_failure_reason=({
const char*_tmp387="(effects don't unify)";_tag_arr(_tmp387,sizeof(char),
_get_zero_arr_size(_tmp387,22));});goto _LL212;_LL249:;_LL24A: goto _LL212;_LL212:;}(
int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp388=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp389=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp388,_tmp389);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(x);y=
Cyc_Absyn_compress_conref(y);if(x == y)return 0;{void*_tmp38A=(void*)x->v;void*
_tmp38B;_LL263: if((int)_tmp38A != 0)goto _LL265;_LL264: return - 1;_LL265: if(_tmp38A
<= (void*)1  || *((int*)_tmp38A)!= 0)goto _LL267;_tmp38B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp38A)->f1;_LL266: {void*_tmp38C=(void*)y->v;void*_tmp38D;_LL26A: if((int)
_tmp38C != 0)goto _LL26C;_LL26B: return 1;_LL26C: if(_tmp38C <= (void*)1  || *((int*)
_tmp38C)!= 0)goto _LL26E;_tmp38D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp38C)->f1;_LL26D: return cmp(_tmp38B,_tmp38D);_LL26E: if(_tmp38C <= (void*)1  || *((
int*)_tmp38C)!= 1)goto _LL269;_LL26F:({void*_tmp38E[0]={};Cyc_Tcutil_impos(({
const char*_tmp38F="unify_conref: forward after compress(2)";_tag_arr(_tmp38F,
sizeof(char),_get_zero_arr_size(_tmp38F,40));}),_tag_arr(_tmp38E,sizeof(void*),0));});
_LL269:;}_LL267: if(_tmp38A <= (void*)1  || *((int*)_tmp38A)!= 1)goto _LL262;_LL268:({
void*_tmp390[0]={};Cyc_Tcutil_impos(({const char*_tmp391="unify_conref: forward after compress";
_tag_arr(_tmp391,sizeof(char),_get_zero_arr_size(_tmp391,37));}),_tag_arr(
_tmp390,sizeof(void*),0));});_LL262:;}}static int Cyc_Tcutil_tqual_type_cmp(struct
_tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp393;struct Cyc_Absyn_Tqual
_tmp394;void*_tmp395;struct _tuple4*_tmp392=tqt1;_tmp393=*_tmp392;_tmp394=_tmp393.f1;
_tmp395=_tmp393.f2;{struct _tuple4 _tmp397;struct Cyc_Absyn_Tqual _tmp398;void*
_tmp399;struct _tuple4*_tmp396=tqt2;_tmp397=*_tmp396;_tmp398=_tmp397.f1;_tmp399=
_tmp397.f2;{int _tmp39A=Cyc_Tcutil_tqual_cmp(_tmp394,_tmp398);if(_tmp39A != 0)
return _tmp39A;return Cyc_Tcutil_typecmp(_tmp395,_tmp399);}}}static int Cyc_Tcutil_aggrfield_cmp(
struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp39B=Cyc_strptrcmp(
f1->name,f2->name);if(_tmp39B != 0)return _tmp39B;{int _tmp39C=Cyc_Tcutil_tqual_cmp(
f1->tq,f2->tq);if(_tmp39C != 0)return _tmp39C;{int _tmp39D=Cyc_Tcutil_typecmp((void*)
f1->type,(void*)f2->type);if(_tmp39D != 0)return _tmp39D;{int _tmp39E=Cyc_List_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp39E != 0)return
_tmp39E;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp39F=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp39F != 0)return _tmp39F;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3A0=t;_LL271: if((int)_tmp3A0 != 0)goto _LL273;_LL272: return 0;
_LL273: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 0)goto _LL275;_LL274: return 1;
_LL275: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 1)goto _LL277;_LL276: return 2;
_LL277: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 2)goto _LL279;_LL278: return 3;
_LL279: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 3)goto _LL27B;_LL27A: return 4;
_LL27B: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 4)goto _LL27D;_LL27C: return 5;
_LL27D: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 5)goto _LL27F;_LL27E: return 6;
_LL27F: if((int)_tmp3A0 != 1)goto _LL281;_LL280: return 7;_LL281: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 6)goto _LL283;_LL282: return 8;_LL283: if(_tmp3A0 <= (void*)3
 || *((int*)_tmp3A0)!= 7)goto _LL285;_LL284: return 9;_LL285: if(_tmp3A0 <= (void*)3
 || *((int*)_tmp3A0)!= 8)goto _LL287;_LL286: return 10;_LL287: if(_tmp3A0 <= (void*)3
 || *((int*)_tmp3A0)!= 9)goto _LL289;_LL288: return 11;_LL289: if(_tmp3A0 <= (void*)3
 || *((int*)_tmp3A0)!= 10)goto _LL28B;_LL28A: return 12;_LL28B: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 11)goto _LL28D;_LL28C: return 14;_LL28D: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 12)goto _LL28F;_LL28E: return 16;_LL28F: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 13)goto _LL291;_LL290: return 17;_LL291: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 15)goto _LL293;_LL292: return 18;_LL293: if(_tmp3A0 <= (void*)
3  || *((int*)_tmp3A0)!= 16)goto _LL295;_LL294: return 19;_LL295: if((int)_tmp3A0 != 2)
goto _LL297;_LL296: return 20;_LL297: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 19)
goto _LL299;_LL298: return 21;_LL299: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 20)
goto _LL29B;_LL29A: return 22;_LL29B: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 21)
goto _LL29D;_LL29C: return 23;_LL29D: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 14)
goto _LL29F;_LL29E: return 24;_LL29F: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 18)
goto _LL2A1;_LL2A0: return 25;_LL2A1: if(_tmp3A0 <= (void*)3  || *((int*)_tmp3A0)!= 17)
goto _LL270;_LL2A2: return 26;_LL270:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=
Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int
_tmp3A1=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp3A1 != 0)return _tmp3A1;{struct _tuple6 _tmp3A3=({struct _tuple6 _tmp3A2;
_tmp3A2.f1=t2;_tmp3A2.f2=t1;_tmp3A2;});void*_tmp3A4;void*_tmp3A5;void*_tmp3A6;
struct Cyc_Absyn_Tvar*_tmp3A7;void*_tmp3A8;struct Cyc_Absyn_Tvar*_tmp3A9;void*
_tmp3AA;struct Cyc_Absyn_AggrInfo _tmp3AB;void*_tmp3AC;struct Cyc_List_List*_tmp3AD;
void*_tmp3AE;struct Cyc_Absyn_AggrInfo _tmp3AF;void*_tmp3B0;struct Cyc_List_List*
_tmp3B1;void*_tmp3B2;struct _tuple1*_tmp3B3;void*_tmp3B4;struct _tuple1*_tmp3B5;
void*_tmp3B6;struct Cyc_List_List*_tmp3B7;void*_tmp3B8;struct Cyc_List_List*
_tmp3B9;void*_tmp3BA;struct Cyc_Absyn_TunionInfo _tmp3BB;void*_tmp3BC;struct Cyc_Absyn_Tuniondecl**
_tmp3BD;struct Cyc_Absyn_Tuniondecl*_tmp3BE;struct Cyc_List_List*_tmp3BF;void*
_tmp3C0;void*_tmp3C1;struct Cyc_Absyn_TunionInfo _tmp3C2;void*_tmp3C3;struct Cyc_Absyn_Tuniondecl**
_tmp3C4;struct Cyc_Absyn_Tuniondecl*_tmp3C5;struct Cyc_List_List*_tmp3C6;void*
_tmp3C7;void*_tmp3C8;struct Cyc_Absyn_TunionFieldInfo _tmp3C9;void*_tmp3CA;struct
Cyc_Absyn_Tuniondecl*_tmp3CB;struct Cyc_Absyn_Tunionfield*_tmp3CC;struct Cyc_List_List*
_tmp3CD;void*_tmp3CE;struct Cyc_Absyn_TunionFieldInfo _tmp3CF;void*_tmp3D0;struct
Cyc_Absyn_Tuniondecl*_tmp3D1;struct Cyc_Absyn_Tunionfield*_tmp3D2;struct Cyc_List_List*
_tmp3D3;void*_tmp3D4;struct Cyc_Absyn_PtrInfo _tmp3D5;void*_tmp3D6;struct Cyc_Absyn_Tqual
_tmp3D7;struct Cyc_Absyn_PtrAtts _tmp3D8;void*_tmp3D9;struct Cyc_Absyn_Conref*
_tmp3DA;struct Cyc_Absyn_Conref*_tmp3DB;struct Cyc_Absyn_Conref*_tmp3DC;void*
_tmp3DD;struct Cyc_Absyn_PtrInfo _tmp3DE;void*_tmp3DF;struct Cyc_Absyn_Tqual _tmp3E0;
struct Cyc_Absyn_PtrAtts _tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Conref*_tmp3E3;
struct Cyc_Absyn_Conref*_tmp3E4;struct Cyc_Absyn_Conref*_tmp3E5;void*_tmp3E6;void*
_tmp3E7;void*_tmp3E8;void*_tmp3E9;void*_tmp3EA;void*_tmp3EB;void*_tmp3EC;void*
_tmp3ED;void*_tmp3EE;int _tmp3EF;void*_tmp3F0;int _tmp3F1;void*_tmp3F2;struct Cyc_Absyn_ArrayInfo
_tmp3F3;void*_tmp3F4;struct Cyc_Absyn_Tqual _tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;
struct Cyc_Absyn_Conref*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_ArrayInfo _tmp3F9;
void*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Conref*
_tmp3FD;void*_tmp3FE;struct Cyc_Absyn_FnInfo _tmp3FF;struct Cyc_List_List*_tmp400;
struct Cyc_Core_Opt*_tmp401;void*_tmp402;struct Cyc_List_List*_tmp403;int _tmp404;
struct Cyc_Absyn_VarargInfo*_tmp405;struct Cyc_List_List*_tmp406;struct Cyc_List_List*
_tmp407;void*_tmp408;struct Cyc_Absyn_FnInfo _tmp409;struct Cyc_List_List*_tmp40A;
struct Cyc_Core_Opt*_tmp40B;void*_tmp40C;struct Cyc_List_List*_tmp40D;int _tmp40E;
struct Cyc_Absyn_VarargInfo*_tmp40F;struct Cyc_List_List*_tmp410;struct Cyc_List_List*
_tmp411;void*_tmp412;struct Cyc_List_List*_tmp413;void*_tmp414;struct Cyc_List_List*
_tmp415;void*_tmp416;void*_tmp417;struct Cyc_List_List*_tmp418;void*_tmp419;void*
_tmp41A;struct Cyc_List_List*_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*
_tmp41F;void*_tmp420;void*_tmp421;void*_tmp422;void*_tmp423;void*_tmp424;void*
_tmp425;void*_tmp426;void*_tmp427;void*_tmp428;int _tmp429;void*_tmp42A;int
_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*
_tmp431;_LL2A4: _tmp3A4=_tmp3A3.f1;if(_tmp3A4 <= (void*)3  || *((int*)_tmp3A4)!= 0)
goto _LL2A6;_tmp3A5=_tmp3A3.f2;if(_tmp3A5 <= (void*)3  || *((int*)_tmp3A5)!= 0)goto
_LL2A6;_LL2A5:({void*_tmp432[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp433="typecmp: can only compare closed types";
_tag_arr(_tmp433,sizeof(char),_get_zero_arr_size(_tmp433,39));}),_tag_arr(
_tmp432,sizeof(void*),0));});_LL2A6: _tmp3A6=_tmp3A3.f1;if(_tmp3A6 <= (void*)3  || *((
int*)_tmp3A6)!= 1)goto _LL2A8;_tmp3A7=((struct Cyc_Absyn_VarType_struct*)_tmp3A6)->f1;
_tmp3A8=_tmp3A3.f2;if(_tmp3A8 <= (void*)3  || *((int*)_tmp3A8)!= 1)goto _LL2A8;
_tmp3A9=((struct Cyc_Absyn_VarType_struct*)_tmp3A8)->f1;_LL2A7: return Cyc_Core_intcmp(*((
int*)_check_null(_tmp3A9->identity)),*((int*)_check_null(_tmp3A7->identity)));
_LL2A8: _tmp3AA=_tmp3A3.f1;if(_tmp3AA <= (void*)3  || *((int*)_tmp3AA)!= 10)goto
_LL2AA;_tmp3AB=((struct Cyc_Absyn_AggrType_struct*)_tmp3AA)->f1;_tmp3AC=(void*)
_tmp3AB.aggr_info;_tmp3AD=_tmp3AB.targs;_tmp3AE=_tmp3A3.f2;if(_tmp3AE <= (void*)3
 || *((int*)_tmp3AE)!= 10)goto _LL2AA;_tmp3AF=((struct Cyc_Absyn_AggrType_struct*)
_tmp3AE)->f1;_tmp3B0=(void*)_tmp3AF.aggr_info;_tmp3B1=_tmp3AF.targs;_LL2A9: {
struct _tuple1*_tmp435;struct _tuple5 _tmp434=Cyc_Absyn_aggr_kinded_name(_tmp3AC);
_tmp435=_tmp434.f2;{struct _tuple1*_tmp437;struct _tuple5 _tmp436=Cyc_Absyn_aggr_kinded_name(
_tmp3B0);_tmp437=_tmp436.f2;{int _tmp438=Cyc_Absyn_qvar_cmp(_tmp435,_tmp437);if(
_tmp438 != 0)return _tmp438;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3AD,_tmp3B1);}}}}_LL2AA: _tmp3B2=_tmp3A3.f1;if(_tmp3B2 <= (void*)3  || *((int*)
_tmp3B2)!= 12)goto _LL2AC;_tmp3B3=((struct Cyc_Absyn_EnumType_struct*)_tmp3B2)->f1;
_tmp3B4=_tmp3A3.f2;if(_tmp3B4 <= (void*)3  || *((int*)_tmp3B4)!= 12)goto _LL2AC;
_tmp3B5=((struct Cyc_Absyn_EnumType_struct*)_tmp3B4)->f1;_LL2AB: return Cyc_Absyn_qvar_cmp(
_tmp3B3,_tmp3B5);_LL2AC: _tmp3B6=_tmp3A3.f1;if(_tmp3B6 <= (void*)3  || *((int*)
_tmp3B6)!= 13)goto _LL2AE;_tmp3B7=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B6)->f1;
_tmp3B8=_tmp3A3.f2;if(_tmp3B8 <= (void*)3  || *((int*)_tmp3B8)!= 13)goto _LL2AE;
_tmp3B9=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3B8)->f1;_LL2AD: return((int(*)(
int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B7,
_tmp3B9);_LL2AE: _tmp3BA=_tmp3A3.f1;if(_tmp3BA <= (void*)3  || *((int*)_tmp3BA)!= 2)
goto _LL2B0;_tmp3BB=((struct Cyc_Absyn_TunionType_struct*)_tmp3BA)->f1;_tmp3BC=(
void*)_tmp3BB.tunion_info;if(*((int*)_tmp3BC)!= 1)goto _LL2B0;_tmp3BD=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3BC)->f1;_tmp3BE=*_tmp3BD;_tmp3BF=_tmp3BB.targs;_tmp3C0=(void*)_tmp3BB.rgn;
_tmp3C1=_tmp3A3.f2;if(_tmp3C1 <= (void*)3  || *((int*)_tmp3C1)!= 2)goto _LL2B0;
_tmp3C2=((struct Cyc_Absyn_TunionType_struct*)_tmp3C1)->f1;_tmp3C3=(void*)_tmp3C2.tunion_info;
if(*((int*)_tmp3C3)!= 1)goto _LL2B0;_tmp3C4=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3C3)->f1;_tmp3C5=*_tmp3C4;_tmp3C6=_tmp3C2.targs;_tmp3C7=(void*)_tmp3C2.rgn;
_LL2AF: if(_tmp3C5 == _tmp3BE)return 0;{int _tmp439=Cyc_Absyn_qvar_cmp(_tmp3C5->name,
_tmp3BE->name);if(_tmp439 != 0)return _tmp439;{int _tmp43A=Cyc_Tcutil_typecmp(
_tmp3C7,_tmp3C0);if(_tmp43A != 0)return _tmp43A;return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,
_tmp3C6,_tmp3BF);}}_LL2B0: _tmp3C8=_tmp3A3.f1;if(_tmp3C8 <= (void*)3  || *((int*)
_tmp3C8)!= 3)goto _LL2B2;_tmp3C9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3C8)->f1;_tmp3CA=(void*)_tmp3C9.field_info;if(*((int*)_tmp3CA)!= 1)goto
_LL2B2;_tmp3CB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CA)->f1;_tmp3CC=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3CA)->f2;_tmp3CD=_tmp3C9.targs;
_tmp3CE=_tmp3A3.f2;if(_tmp3CE <= (void*)3  || *((int*)_tmp3CE)!= 3)goto _LL2B2;
_tmp3CF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3CE)->f1;_tmp3D0=(void*)
_tmp3CF.field_info;if(*((int*)_tmp3D0)!= 1)goto _LL2B2;_tmp3D1=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp3D0)->f1;_tmp3D2=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3D0)->f2;
_tmp3D3=_tmp3CF.targs;_LL2B1: if(_tmp3D1 == _tmp3CB)return 0;{int _tmp43B=Cyc_Absyn_qvar_cmp(
_tmp3CB->name,_tmp3D1->name);if(_tmp43B != 0)return _tmp43B;{int _tmp43C=Cyc_Absyn_qvar_cmp(
_tmp3CC->name,_tmp3D2->name);if(_tmp43C != 0)return _tmp43C;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3D3,_tmp3CD);}}_LL2B2: _tmp3D4=_tmp3A3.f1;if(_tmp3D4 <= (
void*)3  || *((int*)_tmp3D4)!= 4)goto _LL2B4;_tmp3D5=((struct Cyc_Absyn_PointerType_struct*)
_tmp3D4)->f1;_tmp3D6=(void*)_tmp3D5.elt_typ;_tmp3D7=_tmp3D5.elt_tq;_tmp3D8=
_tmp3D5.ptr_atts;_tmp3D9=(void*)_tmp3D8.rgn;_tmp3DA=_tmp3D8.nullable;_tmp3DB=
_tmp3D8.bounds;_tmp3DC=_tmp3D8.zero_term;_tmp3DD=_tmp3A3.f2;if(_tmp3DD <= (void*)
3  || *((int*)_tmp3DD)!= 4)goto _LL2B4;_tmp3DE=((struct Cyc_Absyn_PointerType_struct*)
_tmp3DD)->f1;_tmp3DF=(void*)_tmp3DE.elt_typ;_tmp3E0=_tmp3DE.elt_tq;_tmp3E1=
_tmp3DE.ptr_atts;_tmp3E2=(void*)_tmp3E1.rgn;_tmp3E3=_tmp3E1.nullable;_tmp3E4=
_tmp3E1.bounds;_tmp3E5=_tmp3E1.zero_term;_LL2B3: {int _tmp43D=Cyc_Tcutil_typecmp(
_tmp3DF,_tmp3D6);if(_tmp43D != 0)return _tmp43D;{int _tmp43E=Cyc_Tcutil_typecmp(
_tmp3E2,_tmp3D9);if(_tmp43E != 0)return _tmp43E;{int _tmp43F=Cyc_Tcutil_tqual_cmp(
_tmp3E0,_tmp3D7);if(_tmp43F != 0)return _tmp43F;{int _tmp440=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3E4,_tmp3DB);if(_tmp440 != 0)return _tmp440;{int _tmp441=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3E5,_tmp3DC);if(_tmp441 != 0)return _tmp441;{void*_tmp442=(
void*)(Cyc_Absyn_compress_conref(_tmp3E4))->v;void*_tmp443;_LL2D9: if(_tmp442 <= (
void*)1  || *((int*)_tmp442)!= 0)goto _LL2DB;_tmp443=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp442)->f1;if((int)_tmp443 != 0)goto _LL2DB;_LL2DA: return 0;_LL2DB:;_LL2DC: goto
_LL2D8;_LL2D8:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E3,
_tmp3DA);}}}}}_LL2B4: _tmp3E6=_tmp3A3.f1;if(_tmp3E6 <= (void*)3  || *((int*)_tmp3E6)
!= 5)goto _LL2B6;_tmp3E7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E6)->f1;
_tmp3E8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3E6)->f2;_tmp3E9=_tmp3A3.f2;
if(_tmp3E9 <= (void*)3  || *((int*)_tmp3E9)!= 5)goto _LL2B6;_tmp3EA=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3E9)->f1;_tmp3EB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3E9)->f2;_LL2B5: if(_tmp3EA != _tmp3E7)return Cyc_Core_intcmp((int)((
unsigned int)_tmp3EA),(int)((unsigned int)_tmp3E7));if(_tmp3EB != _tmp3E8)return
Cyc_Core_intcmp((int)((unsigned int)_tmp3EB),(int)((unsigned int)_tmp3E8));
return 0;_LL2B6: _tmp3EC=_tmp3A3.f1;if((int)_tmp3EC != 1)goto _LL2B8;_tmp3ED=_tmp3A3.f2;
if((int)_tmp3ED != 1)goto _LL2B8;_LL2B7: return 0;_LL2B8: _tmp3EE=_tmp3A3.f1;if(
_tmp3EE <= (void*)3  || *((int*)_tmp3EE)!= 6)goto _LL2BA;_tmp3EF=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3EE)->f1;_tmp3F0=_tmp3A3.f2;if(_tmp3F0 <= (void*)3  || *((int*)_tmp3F0)!= 6)
goto _LL2BA;_tmp3F1=((struct Cyc_Absyn_DoubleType_struct*)_tmp3F0)->f1;_LL2B9: if(
_tmp3EF == _tmp3F1)return 0;else{if(_tmp3EF)return - 1;else{return 1;}}_LL2BA: _tmp3F2=
_tmp3A3.f1;if(_tmp3F2 <= (void*)3  || *((int*)_tmp3F2)!= 7)goto _LL2BC;_tmp3F3=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F2)->f1;_tmp3F4=(void*)_tmp3F3.elt_type;
_tmp3F5=_tmp3F3.tq;_tmp3F6=_tmp3F3.num_elts;_tmp3F7=_tmp3F3.zero_term;_tmp3F8=
_tmp3A3.f2;if(_tmp3F8 <= (void*)3  || *((int*)_tmp3F8)!= 7)goto _LL2BC;_tmp3F9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3F8)->f1;_tmp3FA=(void*)_tmp3F9.elt_type;
_tmp3FB=_tmp3F9.tq;_tmp3FC=_tmp3F9.num_elts;_tmp3FD=_tmp3F9.zero_term;_LL2BB: {
int _tmp444=Cyc_Tcutil_tqual_cmp(_tmp3FB,_tmp3F5);if(_tmp444 != 0)return _tmp444;{
int _tmp445=Cyc_Tcutil_typecmp(_tmp3FA,_tmp3F4);if(_tmp445 != 0)return _tmp445;{int
_tmp446=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3F7,_tmp3FD);if(_tmp446 != 0)
return _tmp446;if(_tmp3F6 == _tmp3FC)return 0;if(_tmp3F6 == 0  || _tmp3FC == 0)({void*
_tmp447[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp448="missing expression in array index";_tag_arr(_tmp448,sizeof(
char),_get_zero_arr_size(_tmp448,34));}),_tag_arr(_tmp447,sizeof(void*),0));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*
a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3F6,
_tmp3FC);}}}_LL2BC: _tmp3FE=_tmp3A3.f1;if(_tmp3FE <= (void*)3  || *((int*)_tmp3FE)
!= 8)goto _LL2BE;_tmp3FF=((struct Cyc_Absyn_FnType_struct*)_tmp3FE)->f1;_tmp400=
_tmp3FF.tvars;_tmp401=_tmp3FF.effect;_tmp402=(void*)_tmp3FF.ret_typ;_tmp403=
_tmp3FF.args;_tmp404=_tmp3FF.c_varargs;_tmp405=_tmp3FF.cyc_varargs;_tmp406=
_tmp3FF.rgn_po;_tmp407=_tmp3FF.attributes;_tmp408=_tmp3A3.f2;if(_tmp408 <= (void*)
3  || *((int*)_tmp408)!= 8)goto _LL2BE;_tmp409=((struct Cyc_Absyn_FnType_struct*)
_tmp408)->f1;_tmp40A=_tmp409.tvars;_tmp40B=_tmp409.effect;_tmp40C=(void*)_tmp409.ret_typ;
_tmp40D=_tmp409.args;_tmp40E=_tmp409.c_varargs;_tmp40F=_tmp409.cyc_varargs;
_tmp410=_tmp409.rgn_po;_tmp411=_tmp409.attributes;_LL2BD:({void*_tmp449[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44A="typecmp: function types not handled";_tag_arr(_tmp44A,sizeof(char),
_get_zero_arr_size(_tmp44A,36));}),_tag_arr(_tmp449,sizeof(void*),0));});_LL2BE:
_tmp412=_tmp3A3.f1;if(_tmp412 <= (void*)3  || *((int*)_tmp412)!= 9)goto _LL2C0;
_tmp413=((struct Cyc_Absyn_TupleType_struct*)_tmp412)->f1;_tmp414=_tmp3A3.f2;if(
_tmp414 <= (void*)3  || *((int*)_tmp414)!= 9)goto _LL2C0;_tmp415=((struct Cyc_Absyn_TupleType_struct*)
_tmp414)->f1;_LL2BF: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp415,_tmp413);_LL2C0: _tmp416=_tmp3A3.f1;if(_tmp416 <= (void*)3  || *((int*)
_tmp416)!= 11)goto _LL2C2;_tmp417=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp416)->f1;_tmp418=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp416)->f2;_tmp419=
_tmp3A3.f2;if(_tmp419 <= (void*)3  || *((int*)_tmp419)!= 11)goto _LL2C2;_tmp41A=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp419)->f1;_tmp41B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp419)->f2;_LL2C1: if(_tmp41A != _tmp417){if(_tmp41A == (void*)0)return - 1;else{
return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp41B,_tmp418);_LL2C2: _tmp41C=_tmp3A3.f1;if(_tmp41C <= (void*)3  || *((int*)
_tmp41C)!= 15)goto _LL2C4;_tmp41D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41C)->f1;_tmp41E=_tmp3A3.f2;if(_tmp41E <= (void*)3  || *((int*)_tmp41E)!= 15)
goto _LL2C4;_tmp41F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp41E)->f1;
_LL2C3: return Cyc_Tcutil_typecmp(_tmp41D,_tmp41F);_LL2C4: _tmp420=_tmp3A3.f1;if(
_tmp420 <= (void*)3  || *((int*)_tmp420)!= 14)goto _LL2C6;_tmp421=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp420)->f1;_tmp422=_tmp3A3.f2;if(_tmp422 <= (void*)
3  || *((int*)_tmp422)!= 14)goto _LL2C6;_tmp423=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp422)->f1;_LL2C5: return Cyc_Tcutil_typecmp(_tmp421,_tmp423);_LL2C6: _tmp424=
_tmp3A3.f1;if(_tmp424 <= (void*)3  || *((int*)_tmp424)!= 17)goto _LL2C8;_tmp425=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp424)->f1;_tmp426=_tmp3A3.f2;if(
_tmp426 <= (void*)3  || *((int*)_tmp426)!= 17)goto _LL2C8;_tmp427=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp426)->f1;_LL2C7: return Cyc_Tcutil_typecmp(_tmp425,
_tmp427);_LL2C8: _tmp428=_tmp3A3.f1;if(_tmp428 <= (void*)3  || *((int*)_tmp428)!= 
18)goto _LL2CA;_tmp429=((struct Cyc_Absyn_TypeInt_struct*)_tmp428)->f1;_tmp42A=
_tmp3A3.f2;if(_tmp42A <= (void*)3  || *((int*)_tmp42A)!= 18)goto _LL2CA;_tmp42B=((
struct Cyc_Absyn_TypeInt_struct*)_tmp42A)->f1;_LL2C9: return Cyc_Core_intcmp(
_tmp429,_tmp42B);_LL2CA: _tmp42C=_tmp3A3.f1;if(_tmp42C <= (void*)3  || *((int*)
_tmp42C)!= 20)goto _LL2CC;_LL2CB: goto _LL2CD;_LL2CC: _tmp42D=_tmp3A3.f2;if(_tmp42D
<= (void*)3  || *((int*)_tmp42D)!= 20)goto _LL2CE;_LL2CD: goto _LL2CF;_LL2CE: _tmp42E=
_tmp3A3.f1;if(_tmp42E <= (void*)3  || *((int*)_tmp42E)!= 19)goto _LL2D0;_LL2CF: goto
_LL2D1;_LL2D0: _tmp42F=_tmp3A3.f1;if(_tmp42F <= (void*)3  || *((int*)_tmp42F)!= 21)
goto _LL2D2;_LL2D1: goto _LL2D3;_LL2D2: _tmp430=_tmp3A3.f2;if(_tmp430 <= (void*)3  || *((
int*)_tmp430)!= 21)goto _LL2D4;_LL2D3: goto _LL2D5;_LL2D4: _tmp431=_tmp3A3.f2;if(
_tmp431 <= (void*)3  || *((int*)_tmp431)!= 19)goto _LL2D6;_LL2D5:({void*_tmp44B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp44C="typecmp: effects not handled";_tag_arr(_tmp44C,sizeof(char),
_get_zero_arr_size(_tmp44C,29));}),_tag_arr(_tmp44B,sizeof(void*),0));});_LL2D6:;
_LL2D7:({void*_tmp44D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(({const char*_tmp44E="Unmatched case in typecmp";_tag_arr(
_tmp44E,sizeof(char),_get_zero_arr_size(_tmp44E,26));}),_tag_arr(_tmp44D,sizeof(
void*),0));});_LL2A3:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp44F=
Cyc_Tcutil_compress(t);_LL2DE: if(_tmp44F <= (void*)3  || *((int*)_tmp44F)!= 5)goto
_LL2E0;_LL2DF: goto _LL2E1;_LL2E0: if((int)_tmp44F != 1)goto _LL2E2;_LL2E1: goto _LL2E3;
_LL2E2: if(_tmp44F <= (void*)3  || *((int*)_tmp44F)!= 6)goto _LL2E4;_LL2E3: goto
_LL2E5;_LL2E4: if(_tmp44F <= (void*)3  || *((int*)_tmp44F)!= 12)goto _LL2E6;_LL2E5:
goto _LL2E7;_LL2E6: if(_tmp44F <= (void*)3  || *((int*)_tmp44F)!= 13)goto _LL2E8;
_LL2E7: return 1;_LL2E8:;_LL2E9: return 0;_LL2DD:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){struct _tuple6 _tmp451=({struct _tuple6 _tmp450;_tmp450.f1=Cyc_Tcutil_compress(
t1);_tmp450.f2=Cyc_Tcutil_compress(t2);_tmp450;});void*_tmp452;int _tmp453;void*
_tmp454;int _tmp455;void*_tmp456;void*_tmp457;void*_tmp458;void*_tmp459;void*
_tmp45A;void*_tmp45B;void*_tmp45C;void*_tmp45D;void*_tmp45E;void*_tmp45F;void*
_tmp460;void*_tmp461;void*_tmp462;void*_tmp463;void*_tmp464;void*_tmp465;void*
_tmp466;void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;void*_tmp46B;void*
_tmp46C;void*_tmp46D;void*_tmp46E;void*_tmp46F;void*_tmp470;void*_tmp471;void*
_tmp472;void*_tmp473;void*_tmp474;void*_tmp475;void*_tmp476;void*_tmp477;void*
_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*_tmp47C;void*_tmp47D;void*
_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*_tmp482;void*_tmp483;void*
_tmp484;_LL2EB: _tmp452=_tmp451.f1;if(_tmp452 <= (void*)3  || *((int*)_tmp452)!= 6)
goto _LL2ED;_tmp453=((struct Cyc_Absyn_DoubleType_struct*)_tmp452)->f1;_tmp454=
_tmp451.f2;if(_tmp454 <= (void*)3  || *((int*)_tmp454)!= 6)goto _LL2ED;_tmp455=((
struct Cyc_Absyn_DoubleType_struct*)_tmp454)->f1;_LL2EC: return !_tmp455  && _tmp453;
_LL2ED: _tmp456=_tmp451.f1;if(_tmp456 <= (void*)3  || *((int*)_tmp456)!= 6)goto
_LL2EF;_tmp457=_tmp451.f2;if((int)_tmp457 != 1)goto _LL2EF;_LL2EE: goto _LL2F0;
_LL2EF: _tmp458=_tmp451.f1;if(_tmp458 <= (void*)3  || *((int*)_tmp458)!= 6)goto
_LL2F1;_tmp459=_tmp451.f2;if(_tmp459 <= (void*)3  || *((int*)_tmp459)!= 5)goto
_LL2F1;_LL2F0: goto _LL2F2;_LL2F1: _tmp45A=_tmp451.f1;if(_tmp45A <= (void*)3  || *((
int*)_tmp45A)!= 6)goto _LL2F3;_tmp45B=_tmp451.f2;if(_tmp45B <= (void*)3  || *((int*)
_tmp45B)!= 14)goto _LL2F3;_LL2F2: goto _LL2F4;_LL2F3: _tmp45C=_tmp451.f1;if((int)
_tmp45C != 1)goto _LL2F5;_tmp45D=_tmp451.f2;if(_tmp45D <= (void*)3  || *((int*)
_tmp45D)!= 14)goto _LL2F5;_LL2F4: goto _LL2F6;_LL2F5: _tmp45E=_tmp451.f1;if(_tmp45E
<= (void*)3  || *((int*)_tmp45E)!= 6)goto _LL2F7;_tmp45F=_tmp451.f2;if(_tmp45F <= (
void*)3  || *((int*)_tmp45F)!= 17)goto _LL2F7;_LL2F6: goto _LL2F8;_LL2F7: _tmp460=
_tmp451.f1;if((int)_tmp460 != 1)goto _LL2F9;_tmp461=_tmp451.f2;if(_tmp461 <= (void*)
3  || *((int*)_tmp461)!= 17)goto _LL2F9;_LL2F8: goto _LL2FA;_LL2F9: _tmp462=_tmp451.f1;
if((int)_tmp462 != 1)goto _LL2FB;_tmp463=_tmp451.f2;if(_tmp463 <= (void*)3  || *((
int*)_tmp463)!= 5)goto _LL2FB;_LL2FA: return 1;_LL2FB: _tmp464=_tmp451.f1;if(_tmp464
<= (void*)3  || *((int*)_tmp464)!= 5)goto _LL2FD;_tmp465=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp464)->f2;if((int)_tmp465 != 3)goto _LL2FD;_tmp466=_tmp451.f2;if(_tmp466 <= (
void*)3  || *((int*)_tmp466)!= 5)goto _LL2FD;_tmp467=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp466)->f2;if((int)_tmp467 != 3)goto _LL2FD;_LL2FC: return 0;_LL2FD: _tmp468=
_tmp451.f1;if(_tmp468 <= (void*)3  || *((int*)_tmp468)!= 5)goto _LL2FF;_tmp469=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp468)->f2;if((int)_tmp469 != 3)goto
_LL2FF;_LL2FE: goto _LL300;_LL2FF: _tmp46A=_tmp451.f1;if(_tmp46A <= (void*)3  || *((
int*)_tmp46A)!= 5)goto _LL301;_tmp46B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46A)->f2;if((int)_tmp46B != 2)goto _LL301;_tmp46C=_tmp451.f2;if((int)_tmp46C != 
1)goto _LL301;_LL300: goto _LL302;_LL301: _tmp46D=_tmp451.f1;if(_tmp46D <= (void*)3
 || *((int*)_tmp46D)!= 5)goto _LL303;_tmp46E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46D)->f2;if((int)_tmp46E != 2)goto _LL303;_tmp46F=_tmp451.f2;if(_tmp46F <= (
void*)3  || *((int*)_tmp46F)!= 5)goto _LL303;_tmp470=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp46F)->f2;if((int)_tmp470 != 1)goto _LL303;_LL302: goto _LL304;_LL303: _tmp471=
_tmp451.f1;if(_tmp471 <= (void*)3  || *((int*)_tmp471)!= 5)goto _LL305;_tmp472=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp471)->f2;if((int)_tmp472 != 2)goto
_LL305;_tmp473=_tmp451.f2;if(_tmp473 <= (void*)3  || *((int*)_tmp473)!= 5)goto
_LL305;_tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp473)->f2;if((int)
_tmp474 != 0)goto _LL305;_LL304: goto _LL306;_LL305: _tmp475=_tmp451.f1;if(_tmp475 <= (
void*)3  || *((int*)_tmp475)!= 5)goto _LL307;_tmp476=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp475)->f2;if((int)_tmp476 != 1)goto _LL307;_tmp477=_tmp451.f2;if(_tmp477 <= (
void*)3  || *((int*)_tmp477)!= 5)goto _LL307;_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp477)->f2;if((int)_tmp478 != 0)goto _LL307;_LL306: goto _LL308;_LL307: _tmp479=
_tmp451.f1;if(_tmp479 <= (void*)3  || *((int*)_tmp479)!= 17)goto _LL309;_tmp47A=
_tmp451.f2;if(_tmp47A <= (void*)3  || *((int*)_tmp47A)!= 5)goto _LL309;_tmp47B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47A)->f2;if((int)_tmp47B != 1)goto
_LL309;_LL308: goto _LL30A;_LL309: _tmp47C=_tmp451.f1;if(_tmp47C <= (void*)3  || *((
int*)_tmp47C)!= 17)goto _LL30B;_tmp47D=_tmp451.f2;if(_tmp47D <= (void*)3  || *((int*)
_tmp47D)!= 5)goto _LL30B;_tmp47E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47D)->f2;
if((int)_tmp47E != 0)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp47F=_tmp451.f1;if(
_tmp47F <= (void*)3  || *((int*)_tmp47F)!= 14)goto _LL30D;_tmp480=_tmp451.f2;if(
_tmp480 <= (void*)3  || *((int*)_tmp480)!= 5)goto _LL30D;_tmp481=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp480)->f2;if((int)_tmp481 != 1)goto _LL30D;_LL30C: goto _LL30E;_LL30D: _tmp482=
_tmp451.f1;if(_tmp482 <= (void*)3  || *((int*)_tmp482)!= 14)goto _LL30F;_tmp483=
_tmp451.f2;if(_tmp483 <= (void*)3  || *((int*)_tmp483)!= 5)goto _LL30F;_tmp484=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp483)->f2;if((int)_tmp484 != 0)goto
_LL30F;_LL30E: return 1;_LL30F:;_LL310: return 0;_LL2EA:;}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v))max_arith_type=({struct Cyc_Core_Opt*_tmp485=
_cycalloc(sizeof(*_tmp485));_tmp485->v=(void*)t1;_tmp485;});}}}if(max_arith_type
!= 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)
el->hd,t)){({struct Cyc_String_pa_struct _tmp489;_tmp489.tag=0;_tmp489.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v));{struct Cyc_String_pa_struct
_tmp488;_tmp488.tag=0;_tmp488.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp486[2]={& _tmp488,& _tmp489};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,({const char*_tmp487="type mismatch: expecting %s but found %s";
_tag_arr(_tmp487,sizeof(char),_get_zero_arr_size(_tmp487,41));}),_tag_arr(
_tmp486,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp48A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL312: if(_tmp48A <= (void*)3  || *((int*)_tmp48A)!= 4)goto _LL314;_LL313: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL311;_LL314:;_LL315: return 0;_LL311:;}
return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp48B=Cyc_Tcutil_compress(
t);_LL317: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 5)goto _LL319;_LL318: goto
_LL31A;_LL319: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 14)goto _LL31B;_LL31A:
goto _LL31C;_LL31B: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 17)goto _LL31D;
_LL31C: goto _LL31E;_LL31D: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 12)goto
_LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp48B <= (void*)3  || *((int*)_tmp48B)!= 13)
goto _LL321;_LL320: return 1;_LL321:;_LL322: return 0;_LL316:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))({void*
_tmp48C[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48D="integral size mismatch; conversion supplied";
_tag_arr(_tmp48D,sizeof(char),_get_zero_arr_size(_tmp48D,44));}),_tag_arr(
_tmp48C,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))({void*
_tmp48E[0]={};Cyc_Tcutil_warn(e->loc,({const char*_tmp48F="integral size mismatch; conversion supplied";
_tag_arr(_tmp48F,sizeof(char),_get_zero_arr_size(_tmp48F,44));}),_tag_arr(
_tmp48E,sizeof(void*),0));});Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple6 _tmp491=({struct _tuple6 _tmp490;_tmp490.f1=
t1;_tmp490.f2=t2;_tmp490;});void*_tmp492;struct Cyc_Absyn_PtrInfo _tmp493;void*
_tmp494;struct Cyc_Absyn_PtrInfo _tmp495;void*_tmp496;struct Cyc_Absyn_ArrayInfo
_tmp497;void*_tmp498;struct Cyc_Absyn_Tqual _tmp499;struct Cyc_Absyn_Exp*_tmp49A;
struct Cyc_Absyn_Conref*_tmp49B;void*_tmp49C;struct Cyc_Absyn_ArrayInfo _tmp49D;
void*_tmp49E;struct Cyc_Absyn_Tqual _tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Conref*
_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_TunionFieldInfo _tmp4A3;void*_tmp4A4;struct
Cyc_Absyn_Tuniondecl*_tmp4A5;struct Cyc_Absyn_Tunionfield*_tmp4A6;struct Cyc_List_List*
_tmp4A7;void*_tmp4A8;struct Cyc_Absyn_TunionInfo _tmp4A9;void*_tmp4AA;struct Cyc_Absyn_Tuniondecl**
_tmp4AB;struct Cyc_Absyn_Tuniondecl*_tmp4AC;struct Cyc_List_List*_tmp4AD;void*
_tmp4AE;struct Cyc_Absyn_PtrInfo _tmp4AF;void*_tmp4B0;struct Cyc_Absyn_Tqual _tmp4B1;
struct Cyc_Absyn_PtrAtts _tmp4B2;void*_tmp4B3;struct Cyc_Absyn_Conref*_tmp4B4;
struct Cyc_Absyn_Conref*_tmp4B5;struct Cyc_Absyn_Conref*_tmp4B6;void*_tmp4B7;
struct Cyc_Absyn_TunionInfo _tmp4B8;void*_tmp4B9;struct Cyc_Absyn_Tuniondecl**
_tmp4BA;struct Cyc_Absyn_Tuniondecl*_tmp4BB;struct Cyc_List_List*_tmp4BC;void*
_tmp4BD;void*_tmp4BE;void*_tmp4BF;void*_tmp4C0;void*_tmp4C1;void*_tmp4C2;_LL324:
_tmp492=_tmp491.f1;if(_tmp492 <= (void*)3  || *((int*)_tmp492)!= 4)goto _LL326;
_tmp493=((struct Cyc_Absyn_PointerType_struct*)_tmp492)->f1;_tmp494=_tmp491.f2;
if(_tmp494 <= (void*)3  || *((int*)_tmp494)!= 4)goto _LL326;_tmp495=((struct Cyc_Absyn_PointerType_struct*)
_tmp494)->f1;_LL325: {int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp493.ptr_atts).nullable,(
_tmp495.ptr_atts).nullable)){void*_tmp4C3=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp493.ptr_atts).nullable))->v;
int _tmp4C4;_LL333: if(_tmp4C3 <= (void*)1  || *((int*)_tmp4C3)!= 0)goto _LL335;
_tmp4C4=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C3)->f1;_LL334: okay=!
_tmp4C4;goto _LL332;_LL335:;_LL336:({void*_tmp4C5[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4C6="silent_castable conref not eq";
_tag_arr(_tmp4C6,sizeof(char),_get_zero_arr_size(_tmp4C6,30));}),_tag_arr(
_tmp4C5,sizeof(void*),0));});_LL332:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(
_tmp493.ptr_atts).bounds,(_tmp495.ptr_atts).bounds)){struct _tuple6 _tmp4C8=({
struct _tuple6 _tmp4C7;_tmp4C7.f1=(void*)(Cyc_Absyn_compress_conref((_tmp493.ptr_atts).bounds))->v;
_tmp4C7.f2=(void*)(Cyc_Absyn_compress_conref((_tmp495.ptr_atts).bounds))->v;
_tmp4C7;});void*_tmp4C9;void*_tmp4CA;void*_tmp4CB;void*_tmp4CC;void*_tmp4CD;
_LL338: _tmp4C9=_tmp4C8.f1;if(_tmp4C9 <= (void*)1  || *((int*)_tmp4C9)!= 0)goto
_LL33A;_tmp4CA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4C9)->f1;_tmp4CB=
_tmp4C8.f2;if(_tmp4CB <= (void*)1  || *((int*)_tmp4CB)!= 0)goto _LL33A;_tmp4CC=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp4CB)->f1;_LL339:{struct _tuple6
_tmp4CF=({struct _tuple6 _tmp4CE;_tmp4CE.f1=_tmp4CA;_tmp4CE.f2=_tmp4CC;_tmp4CE;});
void*_tmp4D0;void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;struct Cyc_Absyn_Exp*
_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;void*_tmp4D8;void*_tmp4D9;
struct Cyc_Absyn_Exp*_tmp4DA;void*_tmp4DB;void*_tmp4DC;void*_tmp4DD;void*_tmp4DE;
void*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;void*_tmp4E1;void*_tmp4E2;void*_tmp4E3;
void*_tmp4E4;_LL33F: _tmp4D0=_tmp4CF.f1;if(_tmp4D0 <= (void*)1  || *((int*)_tmp4D0)
!= 0)goto _LL341;_tmp4D1=_tmp4CF.f2;if((int)_tmp4D1 != 0)goto _LL341;_LL340: goto
_LL342;_LL341: _tmp4D2=_tmp4CF.f1;if((int)_tmp4D2 != 0)goto _LL343;_tmp4D3=_tmp4CF.f2;
if((int)_tmp4D3 != 0)goto _LL343;_LL342: okay=1;goto _LL33E;_LL343: _tmp4D4=_tmp4CF.f1;
if(_tmp4D4 <= (void*)1  || *((int*)_tmp4D4)!= 0)goto _LL345;_tmp4D5=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4D4)->f1;_tmp4D6=_tmp4CF.f2;if(_tmp4D6 <= (void*)1  || *((int*)_tmp4D6)!= 0)
goto _LL345;_tmp4D7=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D6)->f1;_LL344: okay=
okay  && Cyc_Evexp_lte_const_exp(_tmp4D7,_tmp4D5);if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,(_tmp495.ptr_atts).zero_term))({void*_tmp4E5[0]={};
Cyc_Tcutil_warn(loc,({const char*_tmp4E6="implicit cast to shorter array";
_tag_arr(_tmp4E6,sizeof(char),_get_zero_arr_size(_tmp4E6,31));}),_tag_arr(
_tmp4E5,sizeof(void*),0));});goto _LL33E;_LL345: _tmp4D8=_tmp4CF.f1;if((int)
_tmp4D8 != 0)goto _LL347;_tmp4D9=_tmp4CF.f2;if(_tmp4D9 <= (void*)1  || *((int*)
_tmp4D9)!= 0)goto _LL347;_tmp4DA=((struct Cyc_Absyn_Upper_b_struct*)_tmp4D9)->f1;
_LL346: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(
_tmp493.ptr_atts).zero_term) && Cyc_Tcutil_is_bound_one((_tmp495.ptr_atts).bounds))
goto _LL33E;okay=0;goto _LL33E;_LL347: _tmp4DB=_tmp4CF.f1;if(_tmp4DB <= (void*)1  || *((
int*)_tmp4DB)!= 1)goto _LL349;_tmp4DC=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp4DB)->f1;_tmp4DD=_tmp4CF.f2;if(_tmp4DD <= (void*)1  || *((int*)_tmp4DD)!= 1)
goto _LL349;_tmp4DE=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4DD)->f1;
_LL348: if(!Cyc_Tcutil_unify(_tmp4DC,_tmp4DE)){struct _tuple6 _tmp4E8=({struct
_tuple6 _tmp4E7;_tmp4E7.f1=Cyc_Tcutil_compress(_tmp4DC);_tmp4E7.f2=Cyc_Tcutil_compress(
_tmp4DE);_tmp4E7;});void*_tmp4E9;int _tmp4EA;void*_tmp4EB;int _tmp4EC;_LL350:
_tmp4E9=_tmp4E8.f1;if(_tmp4E9 <= (void*)3  || *((int*)_tmp4E9)!= 18)goto _LL352;
_tmp4EA=((struct Cyc_Absyn_TypeInt_struct*)_tmp4E9)->f1;_tmp4EB=_tmp4E8.f2;if(
_tmp4EB <= (void*)3  || *((int*)_tmp4EB)!= 18)goto _LL352;_tmp4EC=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4EB)->f1;_LL351: if(_tmp4EA < _tmp4EC)okay=0;goto _LL34F;_LL352:;_LL353: okay=0;
goto _LL34F;_LL34F:;}goto _LL33E;_LL349: _tmp4DF=_tmp4CF.f1;if(_tmp4DF <= (void*)1
 || *((int*)_tmp4DF)!= 0)goto _LL34B;_tmp4E0=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4DF)->f1;_tmp4E1=_tmp4CF.f2;if(_tmp4E1 <= (void*)1  || *((int*)_tmp4E1)!= 1)
goto _LL34B;_tmp4E2=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4E1)->f1;
_LL34A: {unsigned int _tmp4EE;int _tmp4EF;struct _tuple7 _tmp4ED=Cyc_Evexp_eval_const_uint_exp(
_tmp4E0);_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;if(!_tmp4EF){okay=0;goto _LL33E;}{
void*_tmp4F0=Cyc_Tcutil_compress(_tmp4E2);int _tmp4F1;_LL355: if(_tmp4F0 <= (void*)
3  || *((int*)_tmp4F0)!= 18)goto _LL357;_tmp4F1=((struct Cyc_Absyn_TypeInt_struct*)
_tmp4F0)->f1;_LL356: if(_tmp4EE < _tmp4F1)okay=0;goto _LL354;_LL357:;_LL358: okay=0;
goto _LL354;_LL354:;}goto _LL33E;}_LL34B: _tmp4E3=_tmp4CF.f1;if(_tmp4E3 <= (void*)1
 || *((int*)_tmp4E3)!= 1)goto _LL34D;_LL34C: goto _LL34E;_LL34D: _tmp4E4=_tmp4CF.f2;
if(_tmp4E4 <= (void*)1  || *((int*)_tmp4E4)!= 1)goto _LL33E;_LL34E: okay=0;goto
_LL33E;_LL33E:;}goto _LL337;_LL33A: _tmp4CD=_tmp4C8.f1;if((int)_tmp4CD != 0)goto
_LL33C;_LL33B: okay=0;goto _LL337;_LL33C:;_LL33D:({void*_tmp4F2[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp4F3="silent_castable conrefs didn't unify";
_tag_arr(_tmp4F3,sizeof(char),_get_zero_arr_size(_tmp4F3,37));}),_tag_arr(
_tmp4F2,sizeof(void*),0));});_LL337:;}okay=okay  && Cyc_Tcutil_unify((void*)
_tmp493.elt_typ,(void*)_tmp495.elt_typ);okay=okay  && (Cyc_Tcutil_unify((void*)(
_tmp493.ptr_atts).rgn,(void*)(_tmp495.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp493.ptr_atts).rgn,(void*)(_tmp495.ptr_atts).rgn));okay=okay  && (!(
_tmp493.elt_tq).real_const  || (_tmp495.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp493.ptr_atts).zero_term,(_tmp495.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp493.ptr_atts).zero_term)
 && (_tmp495.elt_tq).real_const);return okay;}_LL326: _tmp496=_tmp491.f1;if(
_tmp496 <= (void*)3  || *((int*)_tmp496)!= 7)goto _LL328;_tmp497=((struct Cyc_Absyn_ArrayType_struct*)
_tmp496)->f1;_tmp498=(void*)_tmp497.elt_type;_tmp499=_tmp497.tq;_tmp49A=_tmp497.num_elts;
_tmp49B=_tmp497.zero_term;_tmp49C=_tmp491.f2;if(_tmp49C <= (void*)3  || *((int*)
_tmp49C)!= 7)goto _LL328;_tmp49D=((struct Cyc_Absyn_ArrayType_struct*)_tmp49C)->f1;
_tmp49E=(void*)_tmp49D.elt_type;_tmp49F=_tmp49D.tq;_tmp4A0=_tmp49D.num_elts;
_tmp4A1=_tmp49D.zero_term;_LL327: {int okay;okay=((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp49B,_tmp4A1) && ((_tmp49A != 0  && _tmp4A0 != 0) && Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp49A,(struct Cyc_Absyn_Exp*)_tmp4A0));return(okay  && Cyc_Tcutil_unify(
_tmp498,_tmp49E)) && (!_tmp499.real_const  || _tmp49F.real_const);}_LL328: _tmp4A2=
_tmp491.f1;if(_tmp4A2 <= (void*)3  || *((int*)_tmp4A2)!= 3)goto _LL32A;_tmp4A3=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4A2)->f1;_tmp4A4=(void*)_tmp4A3.field_info;
if(*((int*)_tmp4A4)!= 1)goto _LL32A;_tmp4A5=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp4A4)->f1;_tmp4A6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4A4)->f2;
_tmp4A7=_tmp4A3.targs;_tmp4A8=_tmp491.f2;if(_tmp4A8 <= (void*)3  || *((int*)
_tmp4A8)!= 2)goto _LL32A;_tmp4A9=((struct Cyc_Absyn_TunionType_struct*)_tmp4A8)->f1;
_tmp4AA=(void*)_tmp4A9.tunion_info;if(*((int*)_tmp4AA)!= 1)goto _LL32A;_tmp4AB=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp4AA)->f1;_tmp4AC=*_tmp4AB;_tmp4AD=
_tmp4A9.targs;_LL329: if((_tmp4A5 == _tmp4AC  || Cyc_Absyn_qvar_cmp(_tmp4A5->name,
_tmp4AC->name)== 0) && _tmp4A6->typs == 0){for(0;_tmp4A7 != 0  && _tmp4AD != 0;(
_tmp4A7=_tmp4A7->tl,_tmp4AD=_tmp4AD->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4A7->hd,(
void*)_tmp4AD->hd))break;}if(_tmp4A7 == 0  && _tmp4AD == 0)return 1;}return 0;_LL32A:
_tmp4AE=_tmp491.f1;if(_tmp4AE <= (void*)3  || *((int*)_tmp4AE)!= 4)goto _LL32C;
_tmp4AF=((struct Cyc_Absyn_PointerType_struct*)_tmp4AE)->f1;_tmp4B0=(void*)
_tmp4AF.elt_typ;_tmp4B1=_tmp4AF.elt_tq;_tmp4B2=_tmp4AF.ptr_atts;_tmp4B3=(void*)
_tmp4B2.rgn;_tmp4B4=_tmp4B2.nullable;_tmp4B5=_tmp4B2.bounds;_tmp4B6=_tmp4B2.zero_term;
_tmp4B7=_tmp491.f2;if(_tmp4B7 <= (void*)3  || *((int*)_tmp4B7)!= 2)goto _LL32C;
_tmp4B8=((struct Cyc_Absyn_TunionType_struct*)_tmp4B7)->f1;_tmp4B9=(void*)_tmp4B8.tunion_info;
if(*((int*)_tmp4B9)!= 1)goto _LL32C;_tmp4BA=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp4B9)->f1;_tmp4BB=*_tmp4BA;_tmp4BC=_tmp4B8.targs;_tmp4BD=(void*)_tmp4B8.rgn;
_LL32B:{void*_tmp4F4=Cyc_Tcutil_compress(_tmp4B0);struct Cyc_Absyn_TunionFieldInfo
_tmp4F5;void*_tmp4F6;struct Cyc_Absyn_Tuniondecl*_tmp4F7;struct Cyc_Absyn_Tunionfield*
_tmp4F8;struct Cyc_List_List*_tmp4F9;_LL35A: if(_tmp4F4 <= (void*)3  || *((int*)
_tmp4F4)!= 3)goto _LL35C;_tmp4F5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4F4)->f1;_tmp4F6=(void*)_tmp4F5.field_info;if(*((int*)_tmp4F6)!= 1)goto
_LL35C;_tmp4F7=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F6)->f1;_tmp4F8=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp4F6)->f2;_tmp4F9=_tmp4F5.targs;
_LL35B: if(!Cyc_Tcutil_unify(_tmp4B3,_tmp4BD) && !Cyc_Tcenv_region_outlives(te,
_tmp4B3,_tmp4BD))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4B4,
Cyc_Absyn_false_conref))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp4B5,Cyc_Absyn_bounds_one_conref))return 0;if(!((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp4B6,Cyc_Absyn_false_conref))return 0;if(Cyc_Absyn_qvar_cmp(_tmp4BB->name,
_tmp4F7->name)== 0  && _tmp4F8->typs != 0){int okay=1;for(0;_tmp4F9 != 0  && _tmp4BC
!= 0;(_tmp4F9=_tmp4F9->tl,_tmp4BC=_tmp4BC->tl)){if(!Cyc_Tcutil_unify((void*)
_tmp4F9->hd,(void*)_tmp4BC->hd)){okay=0;break;}}if((!okay  || _tmp4F9 != 0) || 
_tmp4BC != 0)return 0;return 1;}goto _LL359;_LL35C:;_LL35D: goto _LL359;_LL359:;}
return 0;_LL32C: _tmp4BE=_tmp491.f1;if(_tmp4BE <= (void*)3  || *((int*)_tmp4BE)!= 14)
goto _LL32E;_tmp4BF=_tmp491.f2;if(_tmp4BF <= (void*)3  || *((int*)_tmp4BF)!= 5)goto
_LL32E;_tmp4C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BF)->f2;if((int)
_tmp4C0 != 2)goto _LL32E;_LL32D: goto _LL32F;_LL32E: _tmp4C1=_tmp491.f1;if(_tmp4C1 <= (
void*)3  || *((int*)_tmp4C1)!= 17)goto _LL330;_tmp4C2=_tmp491.f2;if(_tmp4C2 <= (
void*)3  || *((int*)_tmp4C2)!= 5)goto _LL330;_LL32F: return 1;_LL330:;_LL331: return
Cyc_Tcutil_unify(t1,t2);_LL323:;}}int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp4FA=Cyc_Tcutil_compress(t);_LL35F: if(_tmp4FA <= (void*)3  || *((int*)_tmp4FA)
!= 4)goto _LL361;_LL360: return 1;_LL361:;_LL362: return 0;_LL35E:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp4FB=(void*)e->r;void*_tmp4FC;int _tmp4FD;void*
_tmp4FE;char _tmp4FF;void*_tmp500;short _tmp501;void*_tmp502;long long _tmp503;void*
_tmp504;struct Cyc_Absyn_Exp*_tmp505;_LL364: if(*((int*)_tmp4FB)!= 0)goto _LL366;
_tmp4FC=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;if(_tmp4FC <= (void*)
1  || *((int*)_tmp4FC)!= 2)goto _LL366;_tmp4FD=((struct Cyc_Absyn_Int_c_struct*)
_tmp4FC)->f2;if(_tmp4FD != 0)goto _LL366;_LL365: goto _LL367;_LL366: if(*((int*)
_tmp4FB)!= 0)goto _LL368;_tmp4FE=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;
if(_tmp4FE <= (void*)1  || *((int*)_tmp4FE)!= 0)goto _LL368;_tmp4FF=((struct Cyc_Absyn_Char_c_struct*)
_tmp4FE)->f2;if(_tmp4FF != 0)goto _LL368;_LL367: goto _LL369;_LL368: if(*((int*)
_tmp4FB)!= 0)goto _LL36A;_tmp500=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;
if(_tmp500 <= (void*)1  || *((int*)_tmp500)!= 1)goto _LL36A;_tmp501=((struct Cyc_Absyn_Short_c_struct*)
_tmp500)->f2;if(_tmp501 != 0)goto _LL36A;_LL369: goto _LL36B;_LL36A: if(*((int*)
_tmp4FB)!= 0)goto _LL36C;_tmp502=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp4FB)->f1;
if(_tmp502 <= (void*)1  || *((int*)_tmp502)!= 3)goto _LL36C;_tmp503=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp502)->f2;if(_tmp503 != 0)goto _LL36C;_LL36B: return 1;_LL36C: if(*((int*)_tmp4FB)
!= 15)goto _LL36E;_tmp504=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4FB)->f1;
_tmp505=((struct Cyc_Absyn_Cast_e_struct*)_tmp4FB)->f2;_LL36D: return Cyc_Tcutil_is_zero(
_tmp505) && Cyc_Tcutil_admits_zero(_tmp504);_LL36E:;_LL36F: return 0;_LL363:;}
struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp506=
_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Absyn_Const_e_struct _tmp507;
_tmp507.tag=0;_tmp507.f1=(void*)((void*)0);_tmp507;});_tmp506;})));{struct Cyc_List_List*
_tmp508=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp509=({struct Cyc_Absyn_PointerType_struct*_tmp50A=_cycalloc(sizeof(*_tmp50A));
_tmp50A[0]=({struct Cyc_Absyn_PointerType_struct _tmp50B;_tmp50B.tag=4;_tmp50B.f1=({
struct Cyc_Absyn_PtrInfo _tmp50C;_tmp50C.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp50F=_cycalloc(sizeof(*
_tmp50F));_tmp50F->v=_tmp508;_tmp50F;}));_tmp50C.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp50C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50D;_tmp50D.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp50E=_cycalloc(
sizeof(*_tmp50E));_tmp50E->v=_tmp508;_tmp50E;}));_tmp50D.nullable=Cyc_Absyn_true_conref;
_tmp50D.bounds=Cyc_Absyn_empty_conref();_tmp50D.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp50D.ptrloc=0;_tmp50D;});_tmp50C;});_tmp50B;});
_tmp50A;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)
_tmp509));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}struct _tagged_arr Cyc_Tcutil_coercion2string(
void*c){void*_tmp510=c;_LL371: if((int)_tmp510 != 0)goto _LL373;_LL372: return({
const char*_tmp511="unknown";_tag_arr(_tmp511,sizeof(char),_get_zero_arr_size(
_tmp511,8));});_LL373: if((int)_tmp510 != 1)goto _LL375;_LL374: return({const char*
_tmp512="no coercion";_tag_arr(_tmp512,sizeof(char),_get_zero_arr_size(_tmp512,
12));});_LL375: if((int)_tmp510 != 2)goto _LL377;_LL376: return({const char*_tmp513="null check";
_tag_arr(_tmp513,sizeof(char),_get_zero_arr_size(_tmp513,11));});_LL377: if((int)
_tmp510 != 3)goto _LL370;_LL378: return({const char*_tmp514="other coercion";
_tag_arr(_tmp514,sizeof(char),_get_zero_arr_size(_tmp514,15));});_LL370:;}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*c;if(Cyc_Tcutil_unify(
t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(
t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2))({struct Cyc_String_pa_struct
_tmp518;_tmp518.tag=0;_tmp518.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmp517;_tmp517.tag=0;_tmp517.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp515[2]={&
_tmp517,& _tmp518};Cyc_Tcutil_warn(e->loc,({const char*_tmp516="integral size mismatch; %s -> %s conversion supplied";
_tag_arr(_tmp516,sizeof(char),_get_zero_arr_size(_tmp516,53));}),_tag_arr(
_tmp515,sizeof(void*),2));}}});Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)1);
return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;
else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){Cyc_Tcutil_unchecked_cast(
te,e,t2,c);if(c != (void*)2)({struct Cyc_String_pa_struct _tmp51C;_tmp51C.tag=0;
_tmp51C.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t2));{
struct Cyc_String_pa_struct _tmp51B;_tmp51B.tag=0;_tmp51B.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_typ2string(t1));{void*_tmp519[2]={& _tmp51B,&
_tmp51C};Cyc_Tcutil_warn(e->loc,({const char*_tmp51A="implicit cast from %s to %s";
_tag_arr(_tmp51A,sizeof(char),_get_zero_arr_size(_tmp51A,28));}),_tag_arr(
_tmp519,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp51D=Cyc_Tcutil_compress(t);
_LL37A: if(_tmp51D <= (void*)3  || *((int*)_tmp51D)!= 5)goto _LL37C;_LL37B: goto
_LL37D;_LL37C: if((int)_tmp51D != 1)goto _LL37E;_LL37D: goto _LL37F;_LL37E: if(_tmp51D
<= (void*)3  || *((int*)_tmp51D)!= 6)goto _LL380;_LL37F: return 1;_LL380:;_LL381:
return 0;_LL379:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp51E=_cycalloc(
sizeof(*_tmp51E));_tmp51E->f1=x->tq;_tmp51E->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp51E;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp51F=t1;struct Cyc_List_List*
_tmp520;struct Cyc_Absyn_AggrInfo _tmp521;void*_tmp522;struct Cyc_Absyn_Aggrdecl**
_tmp523;struct Cyc_Absyn_Aggrdecl*_tmp524;struct Cyc_List_List*_tmp525;void*
_tmp526;struct Cyc_List_List*_tmp527;struct Cyc_Absyn_FnInfo _tmp528;_LL383: if((int)
_tmp51F != 0)goto _LL385;_LL384: return 0;_LL385: if(_tmp51F <= (void*)3  || *((int*)
_tmp51F)!= 9)goto _LL387;_tmp520=((struct Cyc_Absyn_TupleType_struct*)_tmp51F)->f1;
_LL386: return _tmp520;_LL387: if(_tmp51F <= (void*)3  || *((int*)_tmp51F)!= 10)goto
_LL389;_tmp521=((struct Cyc_Absyn_AggrType_struct*)_tmp51F)->f1;_tmp522=(void*)
_tmp521.aggr_info;if(*((int*)_tmp522)!= 1)goto _LL389;_tmp523=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp522)->f1;_tmp524=*_tmp523;_tmp525=_tmp521.targs;_LL388: if((((void*)_tmp524->kind
== (void*)1  || _tmp524->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp524->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp524->impl))->rgn_po != 0)return({struct Cyc_List_List*_tmp529=_cycalloc(
sizeof(*_tmp529));_tmp529->hd=({struct _tuple4*_tmp52A=_cycalloc(sizeof(*_tmp52A));
_tmp52A->f1=Cyc_Absyn_empty_tqual(0);_tmp52A->f2=t1;_tmp52A;});_tmp529->tl=0;
_tmp529;});{struct Cyc_List_List*_tmp52B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp524->tvs,_tmp525);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp52B,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp524->impl))->fields);}_LL389: if(
_tmp51F <= (void*)3  || *((int*)_tmp51F)!= 11)goto _LL38B;_tmp526=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp51F)->f1;if((int)_tmp526 != 0)goto _LL38B;
_tmp527=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp51F)->f2;_LL38A: return((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,
0,_tmp527);_LL38B: if(_tmp51F <= (void*)3  || *((int*)_tmp51F)!= 8)goto _LL38D;
_tmp528=((struct Cyc_Absyn_FnType_struct*)_tmp51F)->f1;_LL38C: return({struct Cyc_List_List*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->hd=({struct _tuple4*_tmp52D=
_cycalloc(sizeof(*_tmp52D));_tmp52D->f1=Cyc_Absyn_const_tqual(0);_tmp52D->f2=t1;
_tmp52D;});_tmp52C->tl=0;_tmp52C;});_LL38D:;_LL38E: return({struct Cyc_List_List*
_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->hd=({struct _tuple4*_tmp52F=
_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=Cyc_Absyn_empty_tqual(0);_tmp52F->f2=t1;
_tmp52F;});_tmp52E->tl=0;_tmp52E;});_LL382:;}}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp530=(void*)t->hd;_LL390: if((int)_tmp530 != 16)goto _LL392;
_LL391: goto _LL393;_LL392: if((int)_tmp530 != 3)goto _LL394;_LL393: goto _LL395;_LL394:
if(_tmp530 <= (void*)17  || *((int*)_tmp530)!= 4)goto _LL396;_LL395: continue;_LL396:;
_LL397: if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;
_LL38F:;}}for(0;a2 != 0;a2=a2->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple6*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple6*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple6
_tmp532=({struct _tuple6 _tmp531;_tmp531.f1=t1;_tmp531.f2=t2;_tmp531;});void*
_tmp533;struct Cyc_Absyn_PtrInfo _tmp534;void*_tmp535;struct Cyc_Absyn_Tqual _tmp536;
struct Cyc_Absyn_PtrAtts _tmp537;void*_tmp538;struct Cyc_Absyn_Conref*_tmp539;
struct Cyc_Absyn_Conref*_tmp53A;struct Cyc_Absyn_Conref*_tmp53B;void*_tmp53C;
struct Cyc_Absyn_PtrInfo _tmp53D;void*_tmp53E;struct Cyc_Absyn_Tqual _tmp53F;struct
Cyc_Absyn_PtrAtts _tmp540;void*_tmp541;struct Cyc_Absyn_Conref*_tmp542;struct Cyc_Absyn_Conref*
_tmp543;struct Cyc_Absyn_Conref*_tmp544;void*_tmp545;struct Cyc_Absyn_FnInfo
_tmp546;void*_tmp547;struct Cyc_Absyn_FnInfo _tmp548;_LL399: _tmp533=_tmp532.f1;if(
_tmp533 <= (void*)3  || *((int*)_tmp533)!= 4)goto _LL39B;_tmp534=((struct Cyc_Absyn_PointerType_struct*)
_tmp533)->f1;_tmp535=(void*)_tmp534.elt_typ;_tmp536=_tmp534.elt_tq;_tmp537=
_tmp534.ptr_atts;_tmp538=(void*)_tmp537.rgn;_tmp539=_tmp537.nullable;_tmp53A=
_tmp537.bounds;_tmp53B=_tmp537.zero_term;_tmp53C=_tmp532.f2;if(_tmp53C <= (void*)
3  || *((int*)_tmp53C)!= 4)goto _LL39B;_tmp53D=((struct Cyc_Absyn_PointerType_struct*)
_tmp53C)->f1;_tmp53E=(void*)_tmp53D.elt_typ;_tmp53F=_tmp53D.elt_tq;_tmp540=
_tmp53D.ptr_atts;_tmp541=(void*)_tmp540.rgn;_tmp542=_tmp540.nullable;_tmp543=
_tmp540.bounds;_tmp544=_tmp540.zero_term;_LL39A: if(_tmp536.real_const  && !
_tmp53F.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp539,
_tmp542) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp539)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp542))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp53B,
_tmp544) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp53B)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp544))return 0;if(!Cyc_Tcutil_unify(_tmp538,_tmp541) && !Cyc_Tcenv_region_outlives(
te,_tmp538,_tmp541))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp53A,_tmp543)){struct _tuple6 _tmp54A=({struct _tuple6 _tmp549;_tmp549.f1=Cyc_Absyn_conref_val(
_tmp53A);_tmp549.f2=Cyc_Absyn_conref_val(_tmp543);_tmp549;});void*_tmp54B;void*
_tmp54C;void*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;void*_tmp54F;struct Cyc_Absyn_Exp*
_tmp550;_LL3A0: _tmp54B=_tmp54A.f1;if(_tmp54B <= (void*)1  || *((int*)_tmp54B)!= 0)
goto _LL3A2;_tmp54C=_tmp54A.f2;if((int)_tmp54C != 0)goto _LL3A2;_LL3A1: goto _LL39F;
_LL3A2: _tmp54D=_tmp54A.f1;if(_tmp54D <= (void*)1  || *((int*)_tmp54D)!= 0)goto
_LL3A4;_tmp54E=((struct Cyc_Absyn_Upper_b_struct*)_tmp54D)->f1;_tmp54F=_tmp54A.f2;
if(_tmp54F <= (void*)1  || *((int*)_tmp54F)!= 0)goto _LL3A4;_tmp550=((struct Cyc_Absyn_Upper_b_struct*)
_tmp54F)->f1;_LL3A3: if(!Cyc_Evexp_lte_const_exp(_tmp550,_tmp54E))return 0;goto
_LL39F;_LL3A4:;_LL3A5: return 0;_LL39F:;}return Cyc_Tcutil_ptrsubtype(te,({struct
Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=({struct _tuple6*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->f1=t1;_tmp552->f2=t2;_tmp552;});
_tmp551->tl=assume;_tmp551;}),_tmp535,_tmp53E);_LL39B: _tmp545=_tmp532.f1;if(
_tmp545 <= (void*)3  || *((int*)_tmp545)!= 8)goto _LL39D;_tmp546=((struct Cyc_Absyn_FnType_struct*)
_tmp545)->f1;_tmp547=_tmp532.f2;if(_tmp547 <= (void*)3  || *((int*)_tmp547)!= 8)
goto _LL39D;_tmp548=((struct Cyc_Absyn_FnType_struct*)_tmp547)->f1;_LL39C: if(
_tmp546.tvars != 0  || _tmp548.tvars != 0){struct Cyc_List_List*_tmp553=_tmp546.tvars;
struct Cyc_List_List*_tmp554=_tmp548.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp553)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp554))return 0;{
struct _RegionHandle _tmp555=_new_region("r");struct _RegionHandle*r=& _tmp555;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp553 != 0){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp553->hd)!= Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp554))->hd)){int _tmp556=0;_npop_handler(0);
return _tmp556;}inst=({struct Cyc_List_List*_tmp557=_region_malloc(r,sizeof(*
_tmp557));_tmp557->hd=({struct _tuple8*_tmp558=_region_malloc(r,sizeof(*_tmp558));
_tmp558->f1=(struct Cyc_Absyn_Tvar*)_tmp554->hd;_tmp558->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_VarType_struct
_tmp55A;_tmp55A.tag=1;_tmp55A.f1=(struct Cyc_Absyn_Tvar*)_tmp553->hd;_tmp55A;});
_tmp559;});_tmp558;});_tmp557->tl=inst;_tmp557;});_tmp553=_tmp553->tl;_tmp554=
_tmp554->tl;}if(inst != 0){_tmp546.tvars=0;_tmp548.tvars=0;{int _tmp55F=Cyc_Tcutil_subtype(
te,assume,(void*)({struct Cyc_Absyn_FnType_struct*_tmp55B=_cycalloc(sizeof(*
_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_FnType_struct _tmp55C;_tmp55C.tag=8;
_tmp55C.f1=_tmp546;_tmp55C;});_tmp55B;}),(void*)({struct Cyc_Absyn_FnType_struct*
_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_FnType_struct
_tmp55E;_tmp55E.tag=8;_tmp55E.f1=_tmp548;_tmp55E;});_tmp55D;}));_npop_handler(0);
return _tmp55F;}}};_pop_region(r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)
_tmp546.ret_typ,(void*)_tmp548.ret_typ))return 0;{struct Cyc_List_List*_tmp560=
_tmp546.args;struct Cyc_List_List*_tmp561=_tmp548.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp560)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp561))return 0;for(0;_tmp560 != 0;(_tmp560=_tmp560->tl,_tmp561=_tmp561->tl)){
struct Cyc_Absyn_Tqual _tmp563;void*_tmp564;struct _tuple2 _tmp562=*((struct _tuple2*)
_tmp560->hd);_tmp563=_tmp562.f2;_tmp564=_tmp562.f3;{struct Cyc_Absyn_Tqual _tmp566;
void*_tmp567;struct _tuple2 _tmp565=*((struct _tuple2*)((struct Cyc_List_List*)
_check_null(_tmp561))->hd);_tmp566=_tmp565.f2;_tmp567=_tmp565.f3;if(_tmp566.real_const
 && !_tmp563.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp567,_tmp564))return
0;}}if(_tmp546.c_varargs != _tmp548.c_varargs)return 0;if(_tmp546.cyc_varargs != 0
 && _tmp548.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp568=*_tmp546.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp569=*_tmp548.cyc_varargs;if((_tmp569.tq).real_const
 && !(_tmp568.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp569.type,(
void*)_tmp568.type))return 0;}else{if(_tmp546.cyc_varargs != 0  || _tmp548.cyc_varargs
!= 0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp546.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp548.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp546.rgn_po,_tmp548.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp546.attributes,_tmp548.attributes))return 0;return 1;}_LL39D:;_LL39E: return 0;
_LL398:;}}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple6 _tmp56B=({
struct _tuple6 _tmp56A;_tmp56A.f1=Cyc_Tcutil_compress(t1);_tmp56A.f2=Cyc_Tcutil_compress(
t2);_tmp56A;});void*_tmp56C;void*_tmp56D;void*_tmp56E;void*_tmp56F;_LL3A7:
_tmp56C=_tmp56B.f1;if(_tmp56C <= (void*)3  || *((int*)_tmp56C)!= 5)goto _LL3A9;
_tmp56D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp56C)->f2;_tmp56E=_tmp56B.f2;
if(_tmp56E <= (void*)3  || *((int*)_tmp56E)!= 5)goto _LL3A9;_tmp56F=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp56E)->f2;_LL3A8: return _tmp56D == _tmp56F;_LL3A9:;
_LL3AA: return 0;_LL3A6:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple4 _tmp571;struct
Cyc_Absyn_Tqual _tmp572;void*_tmp573;struct _tuple4*_tmp570=(struct _tuple4*)tqs1->hd;
_tmp571=*_tmp570;_tmp572=_tmp571.f1;_tmp573=_tmp571.f2;{struct _tuple4 _tmp575;
struct Cyc_Absyn_Tqual _tmp576;void*_tmp577;struct _tuple4*_tmp574=(struct _tuple4*)
tqs2->hd;_tmp575=*_tmp574;_tmp576=_tmp575.f1;_tmp577=_tmp575.f2;if(_tmp576.real_const
 && Cyc_Tcutil_subtype(te,assume,_tmp573,_tmp577))continue;else{if(Cyc_Tcutil_unify(
_tmp573,_tmp577))continue;else{if(Cyc_Tcutil_isomorphic(_tmp573,_tmp577))
continue;else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp578=Cyc_Tcutil_compress(t);void*_tmp579;_LL3AC: if(_tmp578 <= (void*)3
 || *((int*)_tmp578)!= 5)goto _LL3AE;_tmp579=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp578)->f2;if((int)_tmp579 != 0)goto _LL3AE;_LL3AD: return 1;_LL3AE:;_LL3AF: return
0;_LL3AB:;}void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)0)return(void*)1;{void*_tmp57A=t2;
void*_tmp57B;_LL3B1: if(_tmp57A <= (void*)3  || *((int*)_tmp57A)!= 5)goto _LL3B3;
_tmp57B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp57A)->f2;if((int)_tmp57B != 
2)goto _LL3B3;_LL3B2: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto
_LL3B0;_LL3B3:;_LL3B4: goto _LL3B0;_LL3B0:;}{void*_tmp57C=t1;struct Cyc_Absyn_PtrInfo
_tmp57D;void*_tmp57E;struct Cyc_Absyn_Tqual _tmp57F;struct Cyc_Absyn_PtrAtts _tmp580;
void*_tmp581;struct Cyc_Absyn_Conref*_tmp582;struct Cyc_Absyn_Conref*_tmp583;
struct Cyc_Absyn_Conref*_tmp584;struct Cyc_Absyn_ArrayInfo _tmp585;void*_tmp586;
struct Cyc_Absyn_Tqual _tmp587;struct Cyc_Absyn_Exp*_tmp588;struct Cyc_Absyn_Conref*
_tmp589;struct Cyc_Absyn_Enumdecl*_tmp58A;_LL3B6: if(_tmp57C <= (void*)3  || *((int*)
_tmp57C)!= 4)goto _LL3B8;_tmp57D=((struct Cyc_Absyn_PointerType_struct*)_tmp57C)->f1;
_tmp57E=(void*)_tmp57D.elt_typ;_tmp57F=_tmp57D.elt_tq;_tmp580=_tmp57D.ptr_atts;
_tmp581=(void*)_tmp580.rgn;_tmp582=_tmp580.nullable;_tmp583=_tmp580.bounds;
_tmp584=_tmp580.zero_term;_LL3B7:{void*_tmp58B=t2;struct Cyc_Absyn_PtrInfo _tmp58C;
void*_tmp58D;struct Cyc_Absyn_Tqual _tmp58E;struct Cyc_Absyn_PtrAtts _tmp58F;void*
_tmp590;struct Cyc_Absyn_Conref*_tmp591;struct Cyc_Absyn_Conref*_tmp592;struct Cyc_Absyn_Conref*
_tmp593;_LL3C5: if(_tmp58B <= (void*)3  || *((int*)_tmp58B)!= 4)goto _LL3C7;_tmp58C=((
struct Cyc_Absyn_PointerType_struct*)_tmp58B)->f1;_tmp58D=(void*)_tmp58C.elt_typ;
_tmp58E=_tmp58C.elt_tq;_tmp58F=_tmp58C.ptr_atts;_tmp590=(void*)_tmp58F.rgn;
_tmp591=_tmp58F.nullable;_tmp592=_tmp58F.bounds;_tmp593=_tmp58F.zero_term;_LL3C6: {
void*coercion=(void*)3;struct Cyc_List_List*_tmp594=({struct Cyc_List_List*_tmp5A3=
_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=({struct _tuple6*_tmp5A4=_cycalloc(
sizeof(*_tmp5A4));_tmp5A4->f1=t1;_tmp5A4->f2=t2;_tmp5A4;});_tmp5A3->tl=0;_tmp5A3;});
int _tmp595=Cyc_Tcutil_ptrsubtype(te,_tmp594,_tmp57E,_tmp58D) && (!_tmp57F.real_const
 || _tmp58E.real_const);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int
zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp584,_tmp593) || !((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp593);int _tmp596=_tmp595?0:((
Cyc_Tcutil_bits_only(_tmp57E) && Cyc_Tcutil_is_char_type(_tmp58D)) && !((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp593)) && (_tmp58E.real_const
 || !_tmp57F.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp583,_tmp592);if(!bounds_ok  && !_tmp596){struct _tuple6 _tmp598=({struct _tuple6
_tmp597;_tmp597.f1=Cyc_Absyn_conref_val(_tmp583);_tmp597.f2=Cyc_Absyn_conref_val(
_tmp592);_tmp597;});void*_tmp599;struct Cyc_Absyn_Exp*_tmp59A;void*_tmp59B;struct
Cyc_Absyn_Exp*_tmp59C;void*_tmp59D;void*_tmp59E;void*_tmp59F;void*_tmp5A0;void*
_tmp5A1;void*_tmp5A2;_LL3CA: _tmp599=_tmp598.f1;if(_tmp599 <= (void*)1  || *((int*)
_tmp599)!= 0)goto _LL3CC;_tmp59A=((struct Cyc_Absyn_Upper_b_struct*)_tmp599)->f1;
_tmp59B=_tmp598.f2;if(_tmp59B <= (void*)1  || *((int*)_tmp59B)!= 0)goto _LL3CC;
_tmp59C=((struct Cyc_Absyn_Upper_b_struct*)_tmp59B)->f1;_LL3CB: if(Cyc_Evexp_lte_const_exp(
_tmp59C,_tmp59A))bounds_ok=1;goto _LL3C9;_LL3CC: _tmp59D=_tmp598.f1;if(_tmp59D <= (
void*)1  || *((int*)_tmp59D)!= 1)goto _LL3CE;_tmp59E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp59D)->f1;_tmp59F=_tmp598.f2;if(_tmp59F <= (void*)1  || *((int*)_tmp59F)!= 1)
goto _LL3CE;_tmp5A0=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp59F)->f1;
_LL3CD: bounds_ok=Cyc_Tcutil_unify(_tmp59E,_tmp5A0);goto _LL3C9;_LL3CE: _tmp5A1=
_tmp598.f1;if(_tmp5A1 <= (void*)1  || *((int*)_tmp5A1)!= 1)goto _LL3D0;_LL3CF: goto
_LL3D1;_LL3D0: _tmp5A2=_tmp598.f2;if(_tmp5A2 <= (void*)1  || *((int*)_tmp5A2)!= 1)
goto _LL3D2;_LL3D1: bounds_ok=0;goto _LL3C9;_LL3D2:;_LL3D3: bounds_ok=1;goto _LL3C9;
_LL3C9:;}if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp582) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp591))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok) && (_tmp595  || _tmp596))
 && (Cyc_Tcutil_unify(_tmp581,_tmp590) || Cyc_Tcenv_region_outlives(te,_tmp581,
_tmp590)))return coercion;else{return(void*)0;}}}_LL3C7:;_LL3C8: goto _LL3C4;_LL3C4:;}
return(void*)0;_LL3B8: if(_tmp57C <= (void*)3  || *((int*)_tmp57C)!= 7)goto _LL3BA;
_tmp585=((struct Cyc_Absyn_ArrayType_struct*)_tmp57C)->f1;_tmp586=(void*)_tmp585.elt_type;
_tmp587=_tmp585.tq;_tmp588=_tmp585.num_elts;_tmp589=_tmp585.zero_term;_LL3B9:{
void*_tmp5A5=t2;struct Cyc_Absyn_ArrayInfo _tmp5A6;void*_tmp5A7;struct Cyc_Absyn_Tqual
_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Conref*_tmp5AA;_LL3D5: if(
_tmp5A5 <= (void*)3  || *((int*)_tmp5A5)!= 7)goto _LL3D7;_tmp5A6=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A5)->f1;_tmp5A7=(void*)_tmp5A6.elt_type;_tmp5A8=_tmp5A6.tq;_tmp5A9=_tmp5A6.num_elts;
_tmp5AA=_tmp5A6.zero_term;_LL3D6: {int okay;okay=((_tmp588 != 0  && _tmp5A9 != 0)
 && ((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp589,_tmp5AA)) && Cyc_Evexp_lte_const_exp((
struct Cyc_Absyn_Exp*)_tmp5A9,(struct Cyc_Absyn_Exp*)_tmp588);return(okay  && Cyc_Tcutil_unify(
_tmp586,_tmp5A7)) && (!_tmp587.real_const  || _tmp5A8.real_const)?(void*)3:(void*)
0;}_LL3D7:;_LL3D8: return(void*)0;_LL3D4:;}return(void*)0;_LL3BA: if(_tmp57C <= (
void*)3  || *((int*)_tmp57C)!= 12)goto _LL3BC;_tmp58A=((struct Cyc_Absyn_EnumType_struct*)
_tmp57C)->f2;_LL3BB:{void*_tmp5AB=t2;struct Cyc_Absyn_Enumdecl*_tmp5AC;_LL3DA: if(
_tmp5AB <= (void*)3  || *((int*)_tmp5AB)!= 12)goto _LL3DC;_tmp5AC=((struct Cyc_Absyn_EnumType_struct*)
_tmp5AB)->f2;_LL3DB: if((_tmp58A->fields != 0  && _tmp5AC->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp58A->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AC->fields))->v))
return(void*)1;goto _LL3D9;_LL3DC:;_LL3DD: goto _LL3D9;_LL3D9:;}goto _LL3BD;_LL3BC:
if(_tmp57C <= (void*)3  || *((int*)_tmp57C)!= 5)goto _LL3BE;_LL3BD: goto _LL3BF;
_LL3BE: if((int)_tmp57C != 1)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(_tmp57C <= (
void*)3  || *((int*)_tmp57C)!= 6)goto _LL3C2;_LL3C1: return Cyc_Tcutil_coerceable(t2)?(
void*)1:(void*)0;_LL3C2:;_LL3C3: return(void*)0;_LL3B5:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp5AD=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5AF;_tmp5AF.tag=15;_tmp5AF.f1=(void*)t;_tmp5AF.f2=_tmp5AD;_tmp5AF.f3=0;
_tmp5AF.f4=(void*)c;_tmp5AF;});_tmp5AE;})));e->topt=({struct Cyc_Core_Opt*_tmp5B0=
_cycalloc(sizeof(*_tmp5B0));_tmp5B0->v=(void*)t;_tmp5B0;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp5B1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3DF: if(_tmp5B1 <= (void*)3  || *((int*)_tmp5B1)!= 5)
goto _LL3E1;_LL3E0: goto _LL3E2;_LL3E1: if(_tmp5B1 <= (void*)3  || *((int*)_tmp5B1)!= 
12)goto _LL3E3;_LL3E2: goto _LL3E4;_LL3E3: if(_tmp5B1 <= (void*)3  || *((int*)_tmp5B1)
!= 13)goto _LL3E5;_LL3E4: goto _LL3E6;_LL3E5: if(_tmp5B1 <= (void*)3  || *((int*)
_tmp5B1)!= 17)goto _LL3E7;_LL3E6: goto _LL3E8;_LL3E7: if(_tmp5B1 <= (void*)3  || *((
int*)_tmp5B1)!= 14)goto _LL3E9;_LL3E8: return 1;_LL3E9: if(_tmp5B1 <= (void*)3  || *((
int*)_tmp5B1)!= 0)goto _LL3EB;_LL3EA: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL3EB:;_LL3EC: return 0;_LL3DE:;}int
Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return
1;{void*_tmp5B2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3EE: if((int)_tmp5B2 != 1)goto _LL3F0;_LL3EF: goto _LL3F1;_LL3F0: if(_tmp5B2 <= (
void*)3  || *((int*)_tmp5B2)!= 6)goto _LL3F2;_LL3F1: return 1;_LL3F2:;_LL3F3: return 0;
_LL3ED:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp5B3=Cyc_Tcutil_compress(
t);_LL3F5: if(_tmp5B3 <= (void*)3  || *((int*)_tmp5B3)!= 8)goto _LL3F7;_LL3F6: return
1;_LL3F7:;_LL3F8: return 0;_LL3F4:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,
void*t2){struct _tuple6 _tmp5B5=({struct _tuple6 _tmp5B4;_tmp5B4.f1=t1;_tmp5B4.f2=t2;
_tmp5B4;});void*_tmp5B6;int _tmp5B7;void*_tmp5B8;int _tmp5B9;void*_tmp5BA;void*
_tmp5BB;void*_tmp5BC;void*_tmp5BD;void*_tmp5BE;void*_tmp5BF;void*_tmp5C0;void*
_tmp5C1;void*_tmp5C2;void*_tmp5C3;void*_tmp5C4;void*_tmp5C5;void*_tmp5C6;void*
_tmp5C7;void*_tmp5C8;void*_tmp5C9;void*_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*
_tmp5CD;void*_tmp5CE;void*_tmp5CF;void*_tmp5D0;void*_tmp5D1;void*_tmp5D2;void*
_tmp5D3;void*_tmp5D4;void*_tmp5D5;_LL3FA: _tmp5B6=_tmp5B5.f1;if(_tmp5B6 <= (void*)
3  || *((int*)_tmp5B6)!= 6)goto _LL3FC;_tmp5B7=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5B6)->f1;_tmp5B8=_tmp5B5.f2;if(_tmp5B8 <= (void*)3  || *((int*)_tmp5B8)!= 6)
goto _LL3FC;_tmp5B9=((struct Cyc_Absyn_DoubleType_struct*)_tmp5B8)->f1;_LL3FB: if(
_tmp5B7)return t1;else{return t2;}_LL3FC: _tmp5BA=_tmp5B5.f1;if(_tmp5BA <= (void*)3
 || *((int*)_tmp5BA)!= 6)goto _LL3FE;_LL3FD: return t1;_LL3FE: _tmp5BB=_tmp5B5.f2;
if(_tmp5BB <= (void*)3  || *((int*)_tmp5BB)!= 6)goto _LL400;_LL3FF: return t2;_LL400:
_tmp5BC=_tmp5B5.f1;if((int)_tmp5BC != 1)goto _LL402;_LL401: goto _LL403;_LL402:
_tmp5BD=_tmp5B5.f2;if((int)_tmp5BD != 1)goto _LL404;_LL403: return(void*)1;_LL404:
_tmp5BE=_tmp5B5.f1;if(_tmp5BE <= (void*)3  || *((int*)_tmp5BE)!= 5)goto _LL406;
_tmp5BF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BE)->f1;if((int)_tmp5BF != 
1)goto _LL406;_tmp5C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5BE)->f2;if((
int)_tmp5C0 != 3)goto _LL406;_LL405: goto _LL407;_LL406: _tmp5C1=_tmp5B5.f2;if(
_tmp5C1 <= (void*)3  || *((int*)_tmp5C1)!= 5)goto _LL408;_tmp5C2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C1)->f1;if((int)_tmp5C2 != 1)goto _LL408;_tmp5C3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C1)->f2;if((int)_tmp5C3 != 3)goto _LL408;_LL407: return Cyc_Absyn_ulonglong_typ;
_LL408: _tmp5C4=_tmp5B5.f1;if(_tmp5C4 <= (void*)3  || *((int*)_tmp5C4)!= 5)goto
_LL40A;_tmp5C5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C4)->f1;if((int)
_tmp5C5 != 2)goto _LL40A;_tmp5C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5C4)->f2;
if((int)_tmp5C6 != 3)goto _LL40A;_LL409: goto _LL40B;_LL40A: _tmp5C7=_tmp5B5.f2;if(
_tmp5C7 <= (void*)3  || *((int*)_tmp5C7)!= 5)goto _LL40C;_tmp5C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C7)->f1;if((int)_tmp5C8 != 2)goto _LL40C;_tmp5C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5C7)->f2;if((int)_tmp5C9 != 3)goto _LL40C;_LL40B: goto _LL40D;_LL40C: _tmp5CA=
_tmp5B5.f1;if(_tmp5CA <= (void*)3  || *((int*)_tmp5CA)!= 5)goto _LL40E;_tmp5CB=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CA)->f1;if((int)_tmp5CB != 0)goto
_LL40E;_tmp5CC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CA)->f2;if((int)
_tmp5CC != 3)goto _LL40E;_LL40D: goto _LL40F;_LL40E: _tmp5CD=_tmp5B5.f2;if(_tmp5CD <= (
void*)3  || *((int*)_tmp5CD)!= 5)goto _LL410;_tmp5CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CD)->f1;if((int)_tmp5CE != 0)goto _LL410;_tmp5CF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5CD)->f2;if((int)_tmp5CF != 3)goto _LL410;_LL40F: return Cyc_Absyn_slonglong_typ;
_LL410: _tmp5D0=_tmp5B5.f1;if(_tmp5D0 <= (void*)3  || *((int*)_tmp5D0)!= 5)goto
_LL412;_tmp5D1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D0)->f1;if((int)
_tmp5D1 != 1)goto _LL412;_tmp5D2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5D0)->f2;
if((int)_tmp5D2 != 2)goto _LL412;_LL411: goto _LL413;_LL412: _tmp5D3=_tmp5B5.f2;if(
_tmp5D3 <= (void*)3  || *((int*)_tmp5D3)!= 5)goto _LL414;_tmp5D4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D3)->f1;if((int)_tmp5D4 != 1)goto _LL414;_tmp5D5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5D3)->f2;if((int)_tmp5D5 != 2)goto _LL414;_LL413: return Cyc_Absyn_uint_typ;
_LL414:;_LL415: return Cyc_Absyn_sint_typ;_LL3F9:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp5D6=(void*)e->r;struct Cyc_Core_Opt*_tmp5D7;
_LL417: if(*((int*)_tmp5D6)!= 4)goto _LL419;_tmp5D7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D6)->f2;if(_tmp5D7 != 0)goto _LL419;_LL418:({void*_tmp5D8[0]={};Cyc_Tcutil_warn(
e->loc,({const char*_tmp5D9="assignment in test";_tag_arr(_tmp5D9,sizeof(char),
_get_zero_arr_size(_tmp5D9,19));}),_tag_arr(_tmp5D8,sizeof(void*),0));});goto
_LL416;_LL419:;_LL41A: goto _LL416;_LL416:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple6 _tmp5DB=({struct _tuple6 _tmp5DA;_tmp5DA.f1=c1;_tmp5DA.f2=c2;_tmp5DA;});
void*_tmp5DC;void*_tmp5DD;void*_tmp5DE;void*_tmp5DF;void*_tmp5E0;struct Cyc_Core_Opt*
_tmp5E1;struct Cyc_Core_Opt**_tmp5E2;void*_tmp5E3;struct Cyc_Core_Opt*_tmp5E4;
struct Cyc_Core_Opt**_tmp5E5;void*_tmp5E6;struct Cyc_Core_Opt*_tmp5E7;struct Cyc_Core_Opt**
_tmp5E8;void*_tmp5E9;void*_tmp5EA;void*_tmp5EB;void*_tmp5EC;void*_tmp5ED;void*
_tmp5EE;struct Cyc_Core_Opt*_tmp5EF;struct Cyc_Core_Opt**_tmp5F0;void*_tmp5F1;void*
_tmp5F2;struct Cyc_Core_Opt*_tmp5F3;struct Cyc_Core_Opt**_tmp5F4;void*_tmp5F5;void*
_tmp5F6;struct Cyc_Core_Opt*_tmp5F7;struct Cyc_Core_Opt**_tmp5F8;void*_tmp5F9;
_LL41C: _tmp5DC=_tmp5DB.f1;if(*((int*)_tmp5DC)!= 0)goto _LL41E;_tmp5DD=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp5DC)->f1;_tmp5DE=_tmp5DB.f2;if(*((int*)_tmp5DE)
!= 0)goto _LL41E;_tmp5DF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5DE)->f1;
_LL41D: return _tmp5DD == _tmp5DF;_LL41E: _tmp5E0=_tmp5DB.f2;if(*((int*)_tmp5E0)!= 1)
goto _LL420;_tmp5E1=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E0)->f1;_tmp5E2=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E0)->f1;_LL41F:*
_tmp5E2=({struct Cyc_Core_Opt*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->v=(
void*)c1;_tmp5FA;});return 1;_LL420: _tmp5E3=_tmp5DB.f1;if(*((int*)_tmp5E3)!= 1)
goto _LL422;_tmp5E4=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E3)->f1;_tmp5E5=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E3)->f1;_LL421:*
_tmp5E5=({struct Cyc_Core_Opt*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->v=(
void*)c2;_tmp5FB;});return 1;_LL422: _tmp5E6=_tmp5DB.f1;if(*((int*)_tmp5E6)!= 2)
goto _LL424;_tmp5E7=((struct Cyc_Absyn_Less_kb_struct*)_tmp5E6)->f1;_tmp5E8=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp5E6)->f1;_tmp5E9=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5E6)->f2;_tmp5EA=_tmp5DB.f2;if(*((
int*)_tmp5EA)!= 0)goto _LL424;_tmp5EB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp5EA)->f1;_LL423: if(Cyc_Tcutil_kind_leq(_tmp5EB,_tmp5E9)){*_tmp5E8=({struct
Cyc_Core_Opt*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->v=(void*)c2;_tmp5FC;});
return 1;}else{return 0;}_LL424: _tmp5EC=_tmp5DB.f1;if(*((int*)_tmp5EC)!= 0)goto
_LL426;_tmp5ED=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5EC)->f1;_tmp5EE=
_tmp5DB.f2;if(*((int*)_tmp5EE)!= 2)goto _LL426;_tmp5EF=((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EE)->f1;_tmp5F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp5EE)->f1;_tmp5F1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp5EE)->f2;
_LL425: if(Cyc_Tcutil_kind_leq(_tmp5ED,_tmp5F1)){*_tmp5F0=({struct Cyc_Core_Opt*
_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->v=(void*)c1;_tmp5FD;});return 1;}
else{return 0;}_LL426: _tmp5F2=_tmp5DB.f1;if(*((int*)_tmp5F2)!= 2)goto _LL41B;
_tmp5F3=((struct Cyc_Absyn_Less_kb_struct*)_tmp5F2)->f1;_tmp5F4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F2)->f1;_tmp5F5=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F2)->f2;_tmp5F6=_tmp5DB.f2;if(*((int*)_tmp5F6)!= 2)goto _LL41B;_tmp5F7=((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F6)->f1;_tmp5F8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp5F6)->f1;_tmp5F9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5F6)->f2;_LL427: if(Cyc_Tcutil_kind_leq(_tmp5F5,_tmp5F9)){*_tmp5F8=({struct
Cyc_Core_Opt*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->v=(void*)c1;_tmp5FE;});
return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp5F9,_tmp5F5)){*_tmp5F4=({struct Cyc_Core_Opt*
_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->v=(void*)c2;_tmp5FF;});return 1;}
else{return 0;}}_LL41B:;}}static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){
return({int*_tmp600=_cycalloc_atomic(sizeof(*_tmp600));_tmp600[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp600;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Int_pa_struct _tmp605;_tmp605.tag=1;_tmp605.f1=(unsigned int)i;{void*
_tmp603[1]={& _tmp605};Cyc_aprintf(({const char*_tmp604="#%d";_tag_arr(_tmp604,
sizeof(char),_get_zero_arr_size(_tmp604,4));}),_tag_arr(_tmp603,sizeof(void*),1));}});
return({struct Cyc_Absyn_Tvar*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->name=({
struct _tagged_arr*_tmp602=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp602[0]=s;
_tmp602;});_tmp601->identity=0;_tmp601->kind=(void*)k;_tmp601;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmp606=*t->name;return*((const char*)
_check_unknown_subscript(_tmp606,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_String_pa_struct _tmp609;_tmp609.tag=0;
_tmp609.f1=(struct _tagged_arr)((struct _tagged_arr)*t->name);{void*_tmp607[1]={&
_tmp609};Cyc_printf(({const char*_tmp608="%s";_tag_arr(_tmp608,sizeof(char),
_get_zero_arr_size(_tmp608,3));}),_tag_arr(_tmp607,sizeof(void*),1));}});if(!Cyc_Tcutil_is_temp_tvar(
t))return;{struct _tagged_arr _tmp60A=Cyc_strconcat(({const char*_tmp60F="`";
_tag_arr(_tmp60F,sizeof(char),_get_zero_arr_size(_tmp60F,2));}),(struct
_tagged_arr)*t->name);({struct _tagged_arr _tmp60B=_tagged_arr_plus(_tmp60A,
sizeof(char),1);char _tmp60C=*((char*)_check_unknown_subscript(_tmp60B,sizeof(
char),0));char _tmp60D='t';if(_get_arr_size(_tmp60B,sizeof(char))== 1  && (_tmp60C
== '\000'  && _tmp60D != '\000'))_throw_arraybounds();*((char*)_tmp60B.curr)=
_tmp60D;});t->name=({struct _tagged_arr*_tmp60E=_cycalloc(sizeof(struct
_tagged_arr)* 1);_tmp60E[0]=(struct _tagged_arr)_tmp60A;_tmp60E;});}}struct
_tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct
_tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple13*x){return({struct _tuple2*_tmp610=
_cycalloc(sizeof(*_tmp610));_tmp610->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*
_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->v=(*x).f1;_tmp611;});_tmp610->f2=(*
x).f2;_tmp610->f3=(*x).f3;_tmp610;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp612=0;{struct Cyc_List_List*
atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)
atts->hd))_tmp612=({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));
_tmp613->hd=(void*)((void*)atts->hd);_tmp613->tl=_tmp612;_tmp613;});}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp614=_cycalloc(sizeof(*_tmp614));
_tmp614[0]=({struct Cyc_Absyn_FnType_struct _tmp615;_tmp615.tag=8;_tmp615.f1=({
struct Cyc_Absyn_FnInfo _tmp616;_tmp616.tvars=fd->tvs;_tmp616.effect=fd->effect;
_tmp616.ret_typ=(void*)((void*)fd->ret_type);_tmp616.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp616.c_varargs=fd->c_varargs;_tmp616.cyc_varargs=fd->cyc_varargs;
_tmp616.rgn_po=fd->rgn_po;_tmp616.attributes=_tmp612;_tmp616;});_tmp615;});
_tmp614;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=(*pr).f1;_tmp617->f2=
t;_tmp617;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp618=
_region_malloc(rgn,sizeof(*_tmp618));_tmp618->f1=({struct _tuple15*_tmp619=
_region_malloc(rgn,sizeof(*_tmp619));_tmp619->f1=(*y).f1;_tmp619->f2=(*y).f2;
_tmp619;});_tmp618->f2=(*y).f3;_tmp618;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){struct _tuple15*_tmp61B;void*_tmp61C;struct _tuple16 _tmp61A=*w;
_tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;{struct Cyc_Core_Opt*_tmp61E;struct Cyc_Absyn_Tqual
_tmp61F;struct _tuple15 _tmp61D=*_tmp61B;_tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;
return({struct _tuple2*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=_tmp61E;
_tmp620->f2=_tmp61F;_tmp620->f3=_tmp61C;_tmp620;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->name=f->name;_tmp621->tq=f->tq;
_tmp621->type=(void*)t;_tmp621->width=f->width;_tmp621->attributes=f->attributes;
_tmp621;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp622=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp623;struct Cyc_Absyn_AggrInfo _tmp624;void*_tmp625;
struct Cyc_List_List*_tmp626;struct Cyc_Absyn_TunionInfo _tmp627;void*_tmp628;
struct Cyc_List_List*_tmp629;void*_tmp62A;struct Cyc_Absyn_TunionFieldInfo _tmp62B;
void*_tmp62C;struct Cyc_List_List*_tmp62D;struct _tuple1*_tmp62E;struct Cyc_List_List*
_tmp62F;struct Cyc_Absyn_Typedefdecl*_tmp630;void**_tmp631;struct Cyc_Absyn_ArrayInfo
_tmp632;void*_tmp633;struct Cyc_Absyn_Tqual _tmp634;struct Cyc_Absyn_Exp*_tmp635;
struct Cyc_Absyn_Conref*_tmp636;struct Cyc_Position_Segment*_tmp637;struct Cyc_Absyn_PtrInfo
_tmp638;void*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;
void*_tmp63C;struct Cyc_Absyn_Conref*_tmp63D;struct Cyc_Absyn_Conref*_tmp63E;
struct Cyc_Absyn_Conref*_tmp63F;struct Cyc_Absyn_FnInfo _tmp640;struct Cyc_List_List*
_tmp641;struct Cyc_Core_Opt*_tmp642;void*_tmp643;struct Cyc_List_List*_tmp644;int
_tmp645;struct Cyc_Absyn_VarargInfo*_tmp646;struct Cyc_List_List*_tmp647;struct Cyc_List_List*
_tmp648;struct Cyc_List_List*_tmp649;void*_tmp64A;struct Cyc_List_List*_tmp64B;
struct Cyc_Core_Opt*_tmp64C;void*_tmp64D;void*_tmp64E;void*_tmp64F;void*_tmp650;
void*_tmp651;struct Cyc_List_List*_tmp652;_LL429: if(_tmp622 <= (void*)3  || *((int*)
_tmp622)!= 1)goto _LL42B;_tmp623=((struct Cyc_Absyn_VarType_struct*)_tmp622)->f1;
_LL42A: {struct _handler_cons _tmp653;_push_handler(& _tmp653);{int _tmp655=0;if(
setjmp(_tmp653.handler))_tmp655=1;if(!_tmp655){{void*_tmp656=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp623);_npop_handler(0);return
_tmp656;};_pop_handler();}else{void*_tmp654=(void*)_exn_thrown;void*_tmp658=
_tmp654;_LL45C: if(_tmp658 != Cyc_Core_Not_found)goto _LL45E;_LL45D: return t;_LL45E:;
_LL45F:(void)_throw(_tmp658);_LL45B:;}}}_LL42B: if(_tmp622 <= (void*)3  || *((int*)
_tmp622)!= 10)goto _LL42D;_tmp624=((struct Cyc_Absyn_AggrType_struct*)_tmp622)->f1;
_tmp625=(void*)_tmp624.aggr_info;_tmp626=_tmp624.targs;_LL42C: {struct Cyc_List_List*
_tmp659=Cyc_Tcutil_substs(rgn,inst,_tmp626);return _tmp659 == _tmp626?t:(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({
struct Cyc_Absyn_AggrType_struct _tmp65B;_tmp65B.tag=10;_tmp65B.f1=({struct Cyc_Absyn_AggrInfo
_tmp65C;_tmp65C.aggr_info=(void*)_tmp625;_tmp65C.targs=_tmp659;_tmp65C;});
_tmp65B;});_tmp65A;});}_LL42D: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 2)goto
_LL42F;_tmp627=((struct Cyc_Absyn_TunionType_struct*)_tmp622)->f1;_tmp628=(void*)
_tmp627.tunion_info;_tmp629=_tmp627.targs;_tmp62A=(void*)_tmp627.rgn;_LL42E: {
struct Cyc_List_List*_tmp65D=Cyc_Tcutil_substs(rgn,inst,_tmp629);void*_tmp65E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp62A);return _tmp65D == _tmp629  && _tmp65E == _tmp62A?t:(void*)({struct
Cyc_Absyn_TunionType_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F[0]=({
struct Cyc_Absyn_TunionType_struct _tmp660;_tmp660.tag=2;_tmp660.f1=({struct Cyc_Absyn_TunionInfo
_tmp661;_tmp661.tunion_info=(void*)_tmp628;_tmp661.targs=_tmp65D;_tmp661.rgn=(
void*)_tmp65E;_tmp661;});_tmp660;});_tmp65F;});}_LL42F: if(_tmp622 <= (void*)3  || *((
int*)_tmp622)!= 3)goto _LL431;_tmp62B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp622)->f1;_tmp62C=(void*)_tmp62B.field_info;_tmp62D=_tmp62B.targs;_LL430: {
struct Cyc_List_List*_tmp662=Cyc_Tcutil_substs(rgn,inst,_tmp62D);return _tmp662 == 
_tmp62D?t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp663=_cycalloc(
sizeof(*_tmp663));_tmp663[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp664;
_tmp664.tag=3;_tmp664.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp665;_tmp665.field_info=(
void*)_tmp62C;_tmp665.targs=_tmp662;_tmp665;});_tmp664;});_tmp663;});}_LL431: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 16)goto _LL433;_tmp62E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp622)->f1;_tmp62F=((struct Cyc_Absyn_TypedefType_struct*)_tmp622)->f2;_tmp630=((
struct Cyc_Absyn_TypedefType_struct*)_tmp622)->f3;_tmp631=((struct Cyc_Absyn_TypedefType_struct*)
_tmp622)->f4;_LL432: {struct Cyc_List_List*_tmp666=Cyc_Tcutil_substs(rgn,inst,
_tmp62F);return _tmp666 == _tmp62F?t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp668;_tmp668.tag=16;_tmp668.f1=_tmp62E;_tmp668.f2=_tmp666;_tmp668.f3=_tmp630;
_tmp668.f4=_tmp631;_tmp668;});_tmp667;});}_LL433: if(_tmp622 <= (void*)3  || *((int*)
_tmp622)!= 7)goto _LL435;_tmp632=((struct Cyc_Absyn_ArrayType_struct*)_tmp622)->f1;
_tmp633=(void*)_tmp632.elt_type;_tmp634=_tmp632.tq;_tmp635=_tmp632.num_elts;
_tmp636=_tmp632.zero_term;_tmp637=_tmp632.zt_loc;_LL434: {void*_tmp669=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp633);return _tmp669 == _tmp633?t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp66B;_tmp66B.tag=7;_tmp66B.f1=({struct Cyc_Absyn_ArrayInfo _tmp66C;_tmp66C.elt_type=(
void*)_tmp669;_tmp66C.tq=_tmp634;_tmp66C.num_elts=_tmp635;_tmp66C.zero_term=
_tmp636;_tmp66C.zt_loc=_tmp637;_tmp66C;});_tmp66B;});_tmp66A;});}_LL435: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 4)goto _LL437;_tmp638=((struct Cyc_Absyn_PointerType_struct*)
_tmp622)->f1;_tmp639=(void*)_tmp638.elt_typ;_tmp63A=_tmp638.elt_tq;_tmp63B=
_tmp638.ptr_atts;_tmp63C=(void*)_tmp63B.rgn;_tmp63D=_tmp63B.nullable;_tmp63E=
_tmp63B.bounds;_tmp63F=_tmp63B.zero_term;_LL436: {void*_tmp66D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp639);void*_tmp66E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63C);struct
Cyc_Absyn_Conref*_tmp66F=_tmp63E;{void*_tmp670=(void*)(Cyc_Absyn_compress_conref(
_tmp63E))->v;void*_tmp671;void*_tmp672;_LL461: if(_tmp670 <= (void*)1  || *((int*)
_tmp670)!= 0)goto _LL463;_tmp671=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
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
_tmp63F;_tmp679.ptrloc=0;_tmp679;});_tmp678;});_tmp677;});_tmp676;});}_LL437: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 8)goto _LL439;_tmp640=((struct Cyc_Absyn_FnType_struct*)
_tmp622)->f1;_tmp641=_tmp640.tvars;_tmp642=_tmp640.effect;_tmp643=(void*)_tmp640.ret_typ;
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
_tmp693;});_tmp692;});}}}}}_LL439: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 9)
goto _LL43B;_tmp649=((struct Cyc_Absyn_TupleType_struct*)_tmp622)->f1;_LL43A: {
struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp649);struct Cyc_List_List*_tmp696=Cyc_Tcutil_substs(rgn,inst,_tmp695);if(
_tmp696 == _tmp695)return t;{struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp649,_tmp696);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_TupleType_struct
_tmp699;_tmp699.tag=9;_tmp699.f1=_tmp697;_tmp699;});_tmp698;});}}_LL43B: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 11)goto _LL43D;_tmp64A=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp622)->f1;_tmp64B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp622)->f2;_LL43C: {struct Cyc_List_List*_tmp69A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp64B);struct Cyc_List_List*_tmp69B=
Cyc_Tcutil_substs(rgn,inst,_tmp69A);if(_tmp69B == _tmp69A)return t;{struct Cyc_List_List*
_tmp69C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmp64B,_tmp69B);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp69E;_tmp69E.tag=11;_tmp69E.f1=(void*)_tmp64A;_tmp69E.f2=_tmp69C;_tmp69E;});
_tmp69D;});}}_LL43D: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 0)goto _LL43F;
_tmp64C=((struct Cyc_Absyn_Evar_struct*)_tmp622)->f2;_LL43E: if(_tmp64C != 0)return
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp64C->v);else{return t;}_LL43F: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 15)goto _LL441;_tmp64D=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp622)->f1;_LL440: {void*_tmp69F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64D);return _tmp69F == _tmp64D?t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp6A1;_tmp6A1.tag=15;_tmp6A1.f1=(void*)_tmp69F;_tmp6A1;});_tmp6A0;});}_LL441:
if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 14)goto _LL443;_tmp64E=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp622)->f1;_LL442: {void*_tmp6A2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64E);return _tmp6A2 == _tmp64E?t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp6A4;_tmp6A4.tag=14;_tmp6A4.f1=(void*)_tmp6A2;_tmp6A4;});_tmp6A3;});}_LL443:
if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 17)goto _LL445;_tmp64F=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp622)->f1;_LL444: {void*_tmp6A5=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp64F);return _tmp6A5 == _tmp64F?t:(void*)({struct Cyc_Absyn_TagType_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_TagType_struct
_tmp6A7;_tmp6A7.tag=17;_tmp6A7.f1=(void*)_tmp6A5;_tmp6A7;});_tmp6A6;});}_LL445:
if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 18)goto _LL447;_LL446: goto _LL448;
_LL447: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 12)goto _LL449;_LL448: goto
_LL44A;_LL449: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 13)goto _LL44B;_LL44A:
goto _LL44C;_LL44B: if((int)_tmp622 != 0)goto _LL44D;_LL44C: goto _LL44E;_LL44D: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 5)goto _LL44F;_LL44E: goto _LL450;_LL44F:
if((int)_tmp622 != 1)goto _LL451;_LL450: goto _LL452;_LL451: if(_tmp622 <= (void*)3
 || *((int*)_tmp622)!= 6)goto _LL453;_LL452: goto _LL454;_LL453: if((int)_tmp622 != 2)
goto _LL455;_LL454: return t;_LL455: if(_tmp622 <= (void*)3  || *((int*)_tmp622)!= 21)
goto _LL457;_tmp650=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp622)->f1;_LL456: {
void*_tmp6A8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp650);return _tmp6A8 == _tmp650?t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));
_tmp6A9[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp6AA;_tmp6AA.tag=21;_tmp6AA.f1=(
void*)_tmp6A8;_tmp6AA;});_tmp6A9;});}_LL457: if(_tmp622 <= (void*)3  || *((int*)
_tmp622)!= 19)goto _LL459;_tmp651=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp622)->f1;_LL458: {void*_tmp6AB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp651);
return _tmp6AB == _tmp651?t:(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp6AC=
_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_AccessEff_struct _tmp6AD;
_tmp6AD.tag=19;_tmp6AD.f1=(void*)_tmp6AB;_tmp6AD;});_tmp6AC;});}_LL459: if(
_tmp622 <= (void*)3  || *((int*)_tmp622)!= 20)goto _LL428;_tmp652=((struct Cyc_Absyn_JoinEff_struct*)
_tmp622)->f1;_LL45A: {struct Cyc_List_List*_tmp6AE=Cyc_Tcutil_substs(rgn,inst,
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
_tmp71C;struct Cyc_Absyn_Conref*_tmp71D;struct Cyc_Position_Segment*_tmp71E;struct
Cyc_Absyn_FnInfo _tmp71F;struct Cyc_List_List*_tmp720;struct Cyc_List_List**_tmp721;
struct Cyc_Core_Opt*_tmp722;struct Cyc_Core_Opt**_tmp723;void*_tmp724;struct Cyc_List_List*
_tmp725;int _tmp726;struct Cyc_Absyn_VarargInfo*_tmp727;struct Cyc_List_List*
_tmp728;struct Cyc_List_List*_tmp729;struct Cyc_List_List*_tmp72A;void*_tmp72B;
struct Cyc_List_List*_tmp72C;struct Cyc_Absyn_AggrInfo _tmp72D;void*_tmp72E;void**
_tmp72F;struct Cyc_List_List*_tmp730;struct Cyc_List_List**_tmp731;struct _tuple1*
_tmp732;struct Cyc_List_List*_tmp733;struct Cyc_List_List**_tmp734;struct Cyc_Absyn_Typedefdecl*
_tmp735;struct Cyc_Absyn_Typedefdecl**_tmp736;void**_tmp737;void***_tmp738;void*
_tmp739;void*_tmp73A;void*_tmp73B;struct Cyc_List_List*_tmp73C;_LL48A: if((int)
_tmp6F8 != 0)goto _LL48C;_LL48B: goto _LL489;_LL48C: if(_tmp6F8 <= (void*)3  || *((int*)
_tmp6F8)!= 0)goto _LL48E;_tmp6F9=((struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f1;
_tmp6FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f1;
_tmp6FB=((struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f2;_tmp6FC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp6F8)->f2;_LL48D: if(*_tmp6FA == 0)*_tmp6FA=({
struct Cyc_Core_Opt*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D->v=(void*)
expected_kind;_tmp73D;});if((cvtenv.fn_result  && cvtenv.generalize_evars) && 
expected_kind == (void*)3)*_tmp6FC=({struct Cyc_Core_Opt*_tmp73E=_cycalloc(sizeof(*
_tmp73E));_tmp73E->v=(void*)((void*)2);_tmp73E;});else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp73F=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp744;_tmp744.tag=2;_tmp744.f1=0;_tmp744.f2=(void*)expected_kind;_tmp744;});
_tmp743;}));*_tmp6FC=({struct Cyc_Core_Opt*_tmp740=_cycalloc(sizeof(*_tmp740));
_tmp740->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_VarType_struct _tmp742;_tmp742.tag=
1;_tmp742.f1=_tmp73F;_tmp742;});_tmp741;}));_tmp740;});_tmp6FD=_tmp73F;goto
_LL48F;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL489;_LL48E: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 1)goto _LL490;
_tmp6FD=((struct Cyc_Absyn_VarType_struct*)_tmp6F8)->f1;_LL48F:{void*_tmp745=Cyc_Absyn_compress_kb((
void*)_tmp6FD->kind);struct Cyc_Core_Opt*_tmp746;struct Cyc_Core_Opt**_tmp747;
_LL4BD: if(*((int*)_tmp745)!= 1)goto _LL4BF;_tmp746=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp745)->f1;_tmp747=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp745)->f1;_LL4BE:*_tmp747=({struct Cyc_Core_Opt*_tmp748=_cycalloc(sizeof(*
_tmp748));_tmp748->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp749=
_cycalloc(sizeof(*_tmp749));_tmp749[0]=({struct Cyc_Absyn_Less_kb_struct _tmp74A;
_tmp74A.tag=2;_tmp74A.f1=0;_tmp74A.f2=(void*)expected_kind;_tmp74A;});_tmp749;}));
_tmp748;});goto _LL4BC;_LL4BF:;_LL4C0: goto _LL4BC;_LL4BC:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp6FD);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp6FD);goto _LL489;_LL490: if(_tmp6F8 <= (void*)3  || *((int*)
_tmp6F8)!= 13)goto _LL492;_tmp6FE=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6F8)->f1;
_LL491: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp74B=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp74B;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp6FE != 0;_tmp6FE=_tmp6FE->tl){struct Cyc_Absyn_Enumfield*_tmp74C=(
struct Cyc_Absyn_Enumfield*)_tmp6FE->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_strptrcmp,prev_fields,(*_tmp74C->name).f2))({struct Cyc_String_pa_struct
_tmp74F;_tmp74F.tag=0;_tmp74F.f1=(struct _tagged_arr)((struct _tagged_arr)*(*
_tmp74C->name).f2);{void*_tmp74D[1]={& _tmp74F};Cyc_Tcutil_terr(_tmp74C->loc,({
const char*_tmp74E="duplicate enum field name %s";_tag_arr(_tmp74E,sizeof(char),
_get_zero_arr_size(_tmp74E,29));}),_tag_arr(_tmp74D,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp750=_region_malloc(uprev_rgn,sizeof(*
_tmp750));_tmp750->hd=(*_tmp74C->name).f2;_tmp750->tl=prev_fields;_tmp750;});}
if(_tmp74C->tag == 0)_tmp74C->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp74C->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp74C->tag)))({struct Cyc_String_pa_struct _tmp753;_tmp753.tag=0;
_tmp753.f1=(struct _tagged_arr)((struct _tagged_arr)*(*_tmp74C->name).f2);{void*
_tmp751[1]={& _tmp753};Cyc_Tcutil_terr(loc,({const char*_tmp752="enum field %s: expression is not constant";
_tag_arr(_tmp752,sizeof(char),_get_zero_arr_size(_tmp752,42));}),_tag_arr(
_tmp751,sizeof(void*),1));}});}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp74C->tag))).f1;tag_count=t1 + 1;(*_tmp74C->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754[
0]=({struct Cyc_Absyn_Abs_n_struct _tmp755;_tmp755.tag=1;_tmp755.f1=te->ns;_tmp755;});
_tmp754;});ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple17*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp74C->name).f2,({
struct _tuple17*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756->f1=(void*)({struct
Cyc_Tcenv_AnonEnumRes_struct*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmp758;_tmp758.tag=4;_tmp758.f1=(void*)t;
_tmp758.f2=_tmp74C;_tmp758;});_tmp757;});_tmp756->f2=1;_tmp756;}));}}};
_pop_region(uprev_rgn);}goto _LL489;}_LL492: if(_tmp6F8 <= (void*)3  || *((int*)
_tmp6F8)!= 12)goto _LL494;_tmp6FF=((struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f1;
_tmp700=((struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f2;_tmp701=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp6F8)->f2;_LL493: if(*_tmp701 == 0  || ((struct
Cyc_Absyn_Enumdecl*)_check_null(*_tmp701))->fields == 0){struct _handler_cons
_tmp759;_push_handler(& _tmp759);{int _tmp75B=0;if(setjmp(_tmp759.handler))_tmp75B=
1;if(!_tmp75B){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,
_tmp6FF);*_tmp701=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{void*
_tmp75A=(void*)_exn_thrown;void*_tmp75D=_tmp75A;_LL4C2: if(_tmp75D != Cyc_Dict_Absent)
goto _LL4C4;_LL4C3: {struct Cyc_Tcenv_Genv*_tmp75E=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp75F=({struct Cyc_Absyn_Enumdecl*_tmp760=_cycalloc(
sizeof(*_tmp760));_tmp760->sc=(void*)((void*)3);_tmp760->name=_tmp6FF;_tmp760->fields=
0;_tmp760;});Cyc_Tc_tcEnumdecl(te,_tmp75E,loc,_tmp75F);{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6FF);*_tmp701=(struct Cyc_Absyn_Enumdecl*)*
ed;goto _LL4C1;}}_LL4C4:;_LL4C5:(void)_throw(_tmp75D);_LL4C1:;}}}{struct Cyc_Absyn_Enumdecl*
ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp701);*_tmp6FF=(ed->name)[
_check_known_subscript_notnull(1,0)];goto _LL489;}_LL494: if(_tmp6F8 <= (void*)3
 || *((int*)_tmp6F8)!= 2)goto _LL496;_tmp702=((struct Cyc_Absyn_TunionType_struct*)
_tmp6F8)->f1;_tmp703=(void*)_tmp702.tunion_info;_tmp704=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp6F8)->f1).tunion_info;_tmp705=_tmp702.targs;_tmp706=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp6F8)->f1).targs;_tmp707=(void*)_tmp702.rgn;
_LL495: {struct Cyc_List_List*_tmp761=*_tmp706;{void*_tmp762=*_tmp704;struct Cyc_Absyn_UnknownTunionInfo
_tmp763;struct _tuple1*_tmp764;int _tmp765;struct Cyc_Absyn_Tuniondecl**_tmp766;
struct Cyc_Absyn_Tuniondecl*_tmp767;_LL4C7: if(*((int*)_tmp762)!= 0)goto _LL4C9;
_tmp763=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp762)->f1;_tmp764=_tmp763.name;
_tmp765=_tmp763.is_xtunion;_LL4C8: {struct Cyc_Absyn_Tuniondecl**tudp;{struct
_handler_cons _tmp768;_push_handler(& _tmp768);{int _tmp76A=0;if(setjmp(_tmp768.handler))
_tmp76A=1;if(!_tmp76A){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp764);;
_pop_handler();}else{void*_tmp769=(void*)_exn_thrown;void*_tmp76C=_tmp769;_LL4CC:
if(_tmp76C != Cyc_Dict_Absent)goto _LL4CE;_LL4CD: {struct Cyc_Tcenv_Genv*_tmp76D=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*_tmp76E=({struct Cyc_Absyn_Tuniondecl*
_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775->sc=(void*)((void*)3);_tmp775->name=
_tmp764;_tmp775->tvs=0;_tmp775->fields=0;_tmp775->is_xtunion=_tmp765;_tmp775;});
Cyc_Tc_tcTuniondecl(te,_tmp76D,loc,_tmp76E);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp764);if(_tmp761 != 0){({struct Cyc_String_pa_struct _tmp772;_tmp772.tag=0;
_tmp772.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp764));{struct Cyc_String_pa_struct _tmp771;_tmp771.tag=0;_tmp771.f1=(struct
_tagged_arr)(_tmp765?(struct _tagged_arr)((struct _tagged_arr)({const char*_tmp773="xtunion";
_tag_arr(_tmp773,sizeof(char),_get_zero_arr_size(_tmp773,8));})):(struct
_tagged_arr)({const char*_tmp774="tunion";_tag_arr(_tmp774,sizeof(char),
_get_zero_arr_size(_tmp774,7));}));{void*_tmp76F[2]={& _tmp771,& _tmp772};Cyc_Tcutil_terr(
loc,({const char*_tmp770="declare parameterized %s %s before using";_tag_arr(
_tmp770,sizeof(char),_get_zero_arr_size(_tmp770,41));}),_tag_arr(_tmp76F,sizeof(
void*),2));}}});return cvtenv;}goto _LL4CB;}_LL4CE:;_LL4CF:(void)_throw(_tmp76C);
_LL4CB:;}}}if((*tudp)->is_xtunion != _tmp765)({struct Cyc_String_pa_struct _tmp778;
_tmp778.tag=0;_tmp778.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp764));{void*_tmp776[1]={& _tmp778};Cyc_Tcutil_terr(loc,({const char*_tmp777="[x]tunion is different from type declaration %s";
_tag_arr(_tmp777,sizeof(char),_get_zero_arr_size(_tmp777,48));}),_tag_arr(
_tmp776,sizeof(void*),1));}});*_tmp704=(void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=({struct Cyc_Absyn_KnownTunion_struct
_tmp77A;_tmp77A.tag=1;_tmp77A.f1=tudp;_tmp77A;});_tmp779;});_tmp767=*tudp;goto
_LL4CA;}_LL4C9: if(*((int*)_tmp762)!= 1)goto _LL4C6;_tmp766=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp762)->f1;_tmp767=*_tmp766;_LL4CA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp707);{struct Cyc_List_List*tvs=_tmp767->tvs;for(0;_tmp761 != 0
 && tvs != 0;(_tmp761=_tmp761->tl,tvs=tvs->tl)){void*t1=(void*)_tmp761->hd;void*
k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp761 != 0)({struct Cyc_String_pa_struct _tmp77D;_tmp77D.tag=
0;_tmp77D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp767->name));{void*_tmp77B[1]={& _tmp77D};Cyc_Tcutil_terr(loc,({const char*
_tmp77C="too many type arguments for tunion %s";_tag_arr(_tmp77C,sizeof(char),
_get_zero_arr_size(_tmp77C,38));}),_tag_arr(_tmp77B,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->hd=(void*)e;_tmp77E->tl=
hidden_ts;_tmp77E;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp706=Cyc_List_imp_append(*_tmp706,Cyc_List_imp_rev(hidden_ts));}goto _LL4C6;}
_LL4C6:;}goto _LL489;}_LL496: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 3)goto
_LL498;_tmp708=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6F8)->f1;_tmp709=(
void*)_tmp708.field_info;_tmp70A=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6F8)->f1).field_info;_tmp70B=_tmp708.targs;_LL497:{void*_tmp77F=*_tmp70A;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp780;struct _tuple1*_tmp781;struct
_tuple1*_tmp782;int _tmp783;struct Cyc_Absyn_Tuniondecl*_tmp784;struct Cyc_Absyn_Tunionfield*
_tmp785;_LL4D1: if(*((int*)_tmp77F)!= 0)goto _LL4D3;_tmp780=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp77F)->f1;_tmp781=_tmp780.tunion_name;_tmp782=_tmp780.field_name;_tmp783=
_tmp780.is_xtunion;_LL4D2: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmp786;_push_handler(& _tmp786);{int _tmp788=0;if(setjmp(
_tmp786.handler))_tmp788=1;if(!_tmp788){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp781);;_pop_handler();}else{void*_tmp787=(void*)_exn_thrown;void*_tmp78A=
_tmp787;_LL4D6: if(_tmp78A != Cyc_Dict_Absent)goto _LL4D8;_LL4D7:({struct Cyc_String_pa_struct
_tmp78D;_tmp78D.tag=0;_tmp78D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp781));{void*_tmp78B[1]={& _tmp78D};Cyc_Tcutil_terr(loc,({const char*_tmp78C="unbound type tunion %s";
_tag_arr(_tmp78C,sizeof(char),_get_zero_arr_size(_tmp78C,23));}),_tag_arr(
_tmp78B,sizeof(void*),1));}});return cvtenv;_LL4D8:;_LL4D9:(void)_throw(_tmp78A);
_LL4D5:;}}}{struct _handler_cons _tmp78E;_push_handler(& _tmp78E);{int _tmp790=0;if(
setjmp(_tmp78E.handler))_tmp790=1;if(!_tmp790){{void*_tmp791=Cyc_Tcenv_lookup_ordinary(
te,loc,_tmp782);struct Cyc_Absyn_Tuniondecl*_tmp792;struct Cyc_Absyn_Tunionfield*
_tmp793;_LL4DB: if(*((int*)_tmp791)!= 2)goto _LL4DD;_tmp792=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp791)->f1;_tmp793=((struct Cyc_Tcenv_TunionRes_struct*)_tmp791)->f2;_LL4DC: tuf=
_tmp793;tud=_tmp792;if(tud->is_xtunion != _tmp783)({struct Cyc_String_pa_struct
_tmp796;_tmp796.tag=0;_tmp796.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp781));{void*_tmp794[1]={& _tmp796};Cyc_Tcutil_terr(loc,({const char*_tmp795="[x]tunion is different from type declaration %s";
_tag_arr(_tmp795,sizeof(char),_get_zero_arr_size(_tmp795,48));}),_tag_arr(
_tmp794,sizeof(void*),1));}});goto _LL4DA;_LL4DD:;_LL4DE:({struct Cyc_String_pa_struct
_tmp79A;_tmp79A.tag=0;_tmp79A.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp781));{struct Cyc_String_pa_struct _tmp799;_tmp799.tag=0;_tmp799.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp782));{void*_tmp797[
2]={& _tmp799,& _tmp79A};Cyc_Tcutil_terr(loc,({const char*_tmp798="unbound field %s in type tunion %s";
_tag_arr(_tmp798,sizeof(char),_get_zero_arr_size(_tmp798,35));}),_tag_arr(
_tmp797,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv _tmp79B=cvtenv;
_npop_handler(0);return _tmp79B;}_LL4DA:;};_pop_handler();}else{void*_tmp78F=(
void*)_exn_thrown;void*_tmp79D=_tmp78F;_LL4E0: if(_tmp79D != Cyc_Dict_Absent)goto
_LL4E2;_LL4E1:({struct Cyc_String_pa_struct _tmp7A1;_tmp7A1.tag=0;_tmp7A1.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp781));{struct
Cyc_String_pa_struct _tmp7A0;_tmp7A0.tag=0;_tmp7A0.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp782));{void*_tmp79E[2]={& _tmp7A0,&
_tmp7A1};Cyc_Tcutil_terr(loc,({const char*_tmp79F="unbound field %s in type tunion %s";
_tag_arr(_tmp79F,sizeof(char),_get_zero_arr_size(_tmp79F,35));}),_tag_arr(
_tmp79E,sizeof(void*),2));}}});return cvtenv;_LL4E2:;_LL4E3:(void)_throw(_tmp79D);
_LL4DF:;}}}*_tmp70A=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp7A3;_tmp7A3.tag=1;_tmp7A3.f1=tud;_tmp7A3.f2=tuf;_tmp7A3;});_tmp7A2;});
_tmp784=tud;_tmp785=tuf;goto _LL4D4;}_LL4D3: if(*((int*)_tmp77F)!= 1)goto _LL4D0;
_tmp784=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77F)->f1;_tmp785=((struct
Cyc_Absyn_KnownTunionfield_struct*)_tmp77F)->f2;_LL4D4: {struct Cyc_List_List*tvs=
_tmp784->tvs;for(0;_tmp70B != 0  && tvs != 0;(_tmp70B=_tmp70B->tl,tvs=tvs->tl)){
void*t1=(void*)_tmp70B->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp70B != 
0)({struct Cyc_String_pa_struct _tmp7A7;_tmp7A7.tag=0;_tmp7A7.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp785->name));{struct
Cyc_String_pa_struct _tmp7A6;_tmp7A6.tag=0;_tmp7A6.f1=(struct _tagged_arr)((struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp784->name));{void*_tmp7A4[2]={& _tmp7A6,&
_tmp7A7};Cyc_Tcutil_terr(loc,({const char*_tmp7A5="too many type arguments for tunion %s.%s";
_tag_arr(_tmp7A5,sizeof(char),_get_zero_arr_size(_tmp7A5,41));}),_tag_arr(
_tmp7A4,sizeof(void*),2));}}});if(tvs != 0)({struct Cyc_String_pa_struct _tmp7AB;
_tmp7AB.tag=0;_tmp7AB.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp785->name));{struct Cyc_String_pa_struct _tmp7AA;_tmp7AA.tag=0;_tmp7AA.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp784->name));{
void*_tmp7A8[2]={& _tmp7AA,& _tmp7AB};Cyc_Tcutil_terr(loc,({const char*_tmp7A9="too few type arguments for tunion %s.%s";
_tag_arr(_tmp7A9,sizeof(char),_get_zero_arr_size(_tmp7A9,40));}),_tag_arr(
_tmp7A8,sizeof(void*),2));}}});goto _LL4D0;}_LL4D0:;}goto _LL489;_LL498: if(_tmp6F8
<= (void*)3  || *((int*)_tmp6F8)!= 4)goto _LL49A;_tmp70C=((struct Cyc_Absyn_PointerType_struct*)
_tmp6F8)->f1;_tmp70D=(void*)_tmp70C.elt_typ;_tmp70E=_tmp70C.elt_tq;_tmp70F=(
struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)_tmp6F8)->f1).elt_tq;
_tmp710=_tmp70C.ptr_atts;_tmp711=(void*)_tmp710.rgn;_tmp712=_tmp710.nullable;
_tmp713=_tmp710.bounds;_tmp714=_tmp710.zero_term;_LL499: {int is_zero_terminated;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp70D);_tmp70F->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmp70F->print_const,_tmp70D);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp711);{void*_tmp7AC=(
void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp714))->v;int _tmp7AD;_LL4E5: if((int)_tmp7AC != 0)goto _LL4E7;_LL4E6:{void*
_tmp7AE=Cyc_Tcutil_compress(_tmp70D);void*_tmp7AF;_LL4EC: if(_tmp7AE <= (void*)3
 || *((int*)_tmp7AE)!= 5)goto _LL4EE;_tmp7AF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp7AE)->f2;if((int)_tmp7AF != 0)goto _LL4EE;_LL4ED:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp714,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL4EB;_LL4EE:;_LL4EF:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp714,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4EB;
_LL4EB:;}goto _LL4E4;_LL4E7: if(_tmp7AC <= (void*)1  || *((int*)_tmp7AC)!= 0)goto
_LL4E9;_tmp7AD=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7AC)->f1;if(_tmp7AD
!= 1)goto _LL4E9;_LL4E8: if(!Cyc_Tcutil_admits_zero(_tmp70D))({struct Cyc_String_pa_struct
_tmp7B2;_tmp7B2.tag=0;_tmp7B2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp70D));{void*_tmp7B0[1]={& _tmp7B2};Cyc_Tcutil_terr(loc,({const char*_tmp7B1="cannot have a pointer to zero-terminated %s elements";
_tag_arr(_tmp7B1,sizeof(char),_get_zero_arr_size(_tmp7B1,53));}),_tag_arr(
_tmp7B0,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4E4;_LL4E9:;_LL4EA:
is_zero_terminated=0;goto _LL4E4;_LL4E4:;}{void*_tmp7B3=(void*)(Cyc_Absyn_compress_conref(
_tmp713))->v;void*_tmp7B4;void*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;void*_tmp7B7;
void*_tmp7B8;_LL4F1: if((int)_tmp7B3 != 0)goto _LL4F3;_LL4F2: goto _LL4F4;_LL4F3: if(
_tmp7B3 <= (void*)1  || *((int*)_tmp7B3)!= 0)goto _LL4F5;_tmp7B4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B3)->f1;if((int)_tmp7B4 != 0)goto _LL4F5;_LL4F4: goto _LL4F0;_LL4F5: if(_tmp7B3
<= (void*)1  || *((int*)_tmp7B3)!= 0)goto _LL4F7;_tmp7B5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B3)->f1;if(_tmp7B5 <= (void*)1  || *((int*)_tmp7B5)!= 0)goto _LL4F7;_tmp7B6=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7B5)->f1;_LL4F6: Cyc_Tcexp_tcExp(te,0,_tmp7B6);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B6))({void*_tmp7B9[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp7BA="pointer bounds expression is not an unsigned int";
_tag_arr(_tmp7BA,sizeof(char),_get_zero_arr_size(_tmp7BA,49));}),_tag_arr(
_tmp7B9,sizeof(void*),0));});{unsigned int _tmp7BC;int _tmp7BD;struct _tuple7
_tmp7BB=Cyc_Evexp_eval_const_uint_exp(_tmp7B6);_tmp7BC=_tmp7BB.f1;_tmp7BD=
_tmp7BB.f2;if(is_zero_terminated  && (!_tmp7BD  || _tmp7BC < 1))({void*_tmp7BE[0]={};
Cyc_Tcutil_terr(loc,({const char*_tmp7BF="zero-terminated pointer cannot point to empty sequence";
_tag_arr(_tmp7BF,sizeof(char),_get_zero_arr_size(_tmp7BF,55));}),_tag_arr(
_tmp7BE,sizeof(void*),0));});goto _LL4F0;}_LL4F7: if(_tmp7B3 <= (void*)1  || *((int*)
_tmp7B3)!= 0)goto _LL4F9;_tmp7B7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B3)->f1;if(_tmp7B7 <= (void*)1  || *((int*)_tmp7B7)!= 1)goto _LL4F9;_tmp7B8=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp7B7)->f1;_LL4F8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)5,_tmp7B8);goto _LL4F0;_LL4F9: if(_tmp7B3 <= (void*)1  || *((
int*)_tmp7B3)!= 1)goto _LL4F0;_LL4FA:({void*_tmp7C0[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp7C1="forward constraint";
_tag_arr(_tmp7C1,sizeof(char),_get_zero_arr_size(_tmp7C1,19));}),_tag_arr(
_tmp7C0,sizeof(void*),0));});_LL4F0:;}goto _LL489;}_LL49A: if(_tmp6F8 <= (void*)3
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
_tmp6F8)->f1).num_elts;_tmp71D=_tmp717.zero_term;_tmp71E=_tmp717.zt_loc;_LL4A5: {
struct Cyc_Absyn_Exp*_tmp7C2=*_tmp71C;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp718);_tmp71A->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp71A->print_const,_tmp718);{int is_zero_terminated;{void*_tmp7C3=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp71D))->v;int _tmp7C4;_LL4FC: if((int)_tmp7C3 != 0)goto _LL4FE;_LL4FD:((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp71D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL4FB;
_LL4FE: if(_tmp7C3 <= (void*)1  || *((int*)_tmp7C3)!= 0)goto _LL500;_tmp7C4=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7C3)->f1;if(_tmp7C4 != 1)goto _LL500;_LL4FF:
if(!Cyc_Tcutil_admits_zero(_tmp718))({struct Cyc_String_pa_struct _tmp7C7;_tmp7C7.tag=
0;_tmp7C7.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp718));{void*_tmp7C5[1]={& _tmp7C7};Cyc_Tcutil_terr(loc,({const char*_tmp7C6="cannot have a zero-terminated array of %s elements";
_tag_arr(_tmp7C6,sizeof(char),_get_zero_arr_size(_tmp7C6,51));}),_tag_arr(
_tmp7C5,sizeof(void*),1));}});is_zero_terminated=1;goto _LL4FB;_LL500:;_LL501:
is_zero_terminated=0;goto _LL4FB;_LL4FB:;}if(_tmp7C2 == 0){if(is_zero_terminated)*
_tmp71C=(_tmp7C2=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{({void*
_tmp7C8[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7C9="array bound defaults to 0 here";
_tag_arr(_tmp7C9,sizeof(char),_get_zero_arr_size(_tmp7C9,31));}),_tag_arr(
_tmp7C8,sizeof(void*),0));});*_tmp71C=(_tmp7C2=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
0,0));}}Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7C2);if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_tmp7C2))({void*_tmp7CA[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7CB="array bounds expression is not constant";_tag_arr(_tmp7CB,
sizeof(char),_get_zero_arr_size(_tmp7CB,40));}),_tag_arr(_tmp7CA,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmp7C2))({void*_tmp7CC[
0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7CD="array bounds expression is not an unsigned int";
_tag_arr(_tmp7CD,sizeof(char),_get_zero_arr_size(_tmp7CD,47));}),_tag_arr(
_tmp7CC,sizeof(void*),0));});{unsigned int _tmp7CF;int _tmp7D0;struct _tuple7
_tmp7CE=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp7C2);_tmp7CF=
_tmp7CE.f1;_tmp7D0=_tmp7CE.f2;if((is_zero_terminated  && _tmp7D0) && _tmp7CF < 1)({
void*_tmp7D1[0]={};Cyc_Tcutil_warn(loc,({const char*_tmp7D2="zero terminated array cannot have zero elements";
_tag_arr(_tmp7D2,sizeof(char),_get_zero_arr_size(_tmp7D2,48));}),_tag_arr(
_tmp7D1,sizeof(void*),0));});goto _LL489;}}}_LL4A6: if(_tmp6F8 <= (void*)3  || *((
int*)_tmp6F8)!= 8)goto _LL4A8;_tmp71F=((struct Cyc_Absyn_FnType_struct*)_tmp6F8)->f1;
_tmp720=_tmp71F.tvars;_tmp721=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp6F8)->f1).tvars;_tmp722=_tmp71F.effect;_tmp723=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp6F8)->f1).effect;_tmp724=(void*)_tmp71F.ret_typ;
_tmp725=_tmp71F.args;_tmp726=_tmp71F.c_varargs;_tmp727=_tmp71F.cyc_varargs;
_tmp728=_tmp71F.rgn_po;_tmp729=_tmp71F.attributes;_LL4A7: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(void*)
0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp729 != 0;_tmp729=_tmp729->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp729->hd))({struct Cyc_String_pa_struct _tmp7D5;
_tmp7D5.tag=0;_tmp7D5.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp729->hd));{void*_tmp7D3[1]={& _tmp7D5};Cyc_Tcutil_terr(loc,({const char*
_tmp7D4="bad function type attribute %s";_tag_arr(_tmp7D4,sizeof(char),
_get_zero_arr_size(_tmp7D4,31));}),_tag_arr(_tmp7D3,sizeof(void*),1));}});{void*
_tmp7D6=(void*)_tmp729->hd;void*_tmp7D7;int _tmp7D8;int _tmp7D9;_LL503: if((int)
_tmp7D6 != 0)goto _LL505;_LL504: if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL502;_LL505: if((int)_tmp7D6 != 1)goto _LL507;_LL506: if(!seen_cdecl){seen_cdecl=1;
++ num_convs;}goto _LL502;_LL507: if((int)_tmp7D6 != 2)goto _LL509;_LL508: if(!
seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL502;_LL509: if(_tmp7D6 <= (void*)
17  || *((int*)_tmp7D6)!= 3)goto _LL50B;_tmp7D7=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp7D6)->f1;_tmp7D8=((struct Cyc_Absyn_Format_att_struct*)_tmp7D6)->f2;_tmp7D9=((
struct Cyc_Absyn_Format_att_struct*)_tmp7D6)->f3;_LL50A: if(!seen_format){
seen_format=1;ft=_tmp7D7;fmt_desc_arg=_tmp7D8;fmt_arg_start=_tmp7D9;}else{({void*
_tmp7DA[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DB="function can't have multiple format attributes";
_tag_arr(_tmp7DB,sizeof(char),_get_zero_arr_size(_tmp7DB,47));}),_tag_arr(
_tmp7DA,sizeof(void*),0));});}goto _LL502;_LL50B:;_LL50C: goto _LL502;_LL502:;}}if(
num_convs > 1)({void*_tmp7DC[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp7DD="function can't have multiple calling conventions";
_tag_arr(_tmp7DD,sizeof(char),_get_zero_arr_size(_tmp7DD,49));}),_tag_arr(
_tmp7DC,sizeof(void*),0));});Cyc_Tcutil_check_unique_tvars(loc,*_tmp721);{struct
Cyc_List_List*b=*_tmp721;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp7DE=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7DF;_LL50E: if(*((int*)_tmp7DE)!= 0)goto
_LL510;_tmp7DF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7DE)->f1;if((int)
_tmp7DF != 1)goto _LL510;_LL50F:({struct Cyc_String_pa_struct _tmp7E2;_tmp7E2.tag=0;
_tmp7E2.f1=(struct _tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);{
void*_tmp7E0[1]={& _tmp7E2};Cyc_Tcutil_terr(loc,({const char*_tmp7E1="function attempts to abstract Mem type variable %s";
_tag_arr(_tmp7E1,sizeof(char),_get_zero_arr_size(_tmp7E1,51));}),_tag_arr(
_tmp7E0,sizeof(void*),1));}});goto _LL50D;_LL510:;_LL511: goto _LL50D;_LL50D:;}}}{
struct Cyc_Tcutil_CVTEnv _tmp7E3=({struct Cyc_Tcutil_CVTEnv _tmp85F;_tmp85F.kind_env=
cvtenv.kind_env;_tmp85F.free_vars=0;_tmp85F.free_evars=0;_tmp85F.generalize_evars=
cvtenv.generalize_evars;_tmp85F.fn_result=1;_tmp85F;});_tmp7E3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E3,(void*)1,_tmp724);_tmp7E3.fn_result=0;{struct Cyc_List_List*a=
_tmp725;for(0;a != 0;a=a->tl){struct _tuple2*_tmp7E4=(struct _tuple2*)a->hd;void*
_tmp7E5=(*_tmp7E4).f3;_tmp7E3=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7E3,(void*)
1,_tmp7E5);((*_tmp7E4).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmp7E4).f2).print_const,_tmp7E5);}}if(_tmp727 != 0){if(_tmp726)({void*_tmp7E6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp7E7="both c_vararg and cyc_vararg";_tag_arr(_tmp7E7,sizeof(char),
_get_zero_arr_size(_tmp7E7,29));}),_tag_arr(_tmp7E6,sizeof(void*),0));});{void*
_tmp7E9;int _tmp7EA;struct Cyc_Absyn_VarargInfo _tmp7E8=*_tmp727;_tmp7E9=(void*)
_tmp7E8.type;_tmp7EA=_tmp7E8.inject;_tmp7E3=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E3,(void*)1,_tmp7E9);(_tmp727->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp727->tq).print_const,_tmp7E9);if(_tmp7EA){void*_tmp7EB=Cyc_Tcutil_compress(
_tmp7E9);struct Cyc_Absyn_TunionInfo _tmp7EC;void*_tmp7ED;_LL513: if(_tmp7EB <= (
void*)3  || *((int*)_tmp7EB)!= 2)goto _LL515;_tmp7EC=((struct Cyc_Absyn_TunionType_struct*)
_tmp7EB)->f1;_tmp7ED=(void*)_tmp7EC.tunion_info;if(*((int*)_tmp7ED)!= 1)goto
_LL515;_LL514: goto _LL512;_LL515:;_LL516:({void*_tmp7EE[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp7EF="can't inject a non-[x]tunion type";_tag_arr(_tmp7EF,sizeof(
char),_get_zero_arr_size(_tmp7EF,34));}),_tag_arr(_tmp7EE,sizeof(void*),0));});
goto _LL512;_LL512:;}}}if(seen_format){int _tmp7F0=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp725);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp7F0) || 
fmt_arg_start < 0) || _tmp727 == 0  && fmt_arg_start != 0) || _tmp727 != 0  && 
fmt_arg_start != _tmp7F0 + 1)({void*_tmp7F1[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp7F2="bad format descriptor";_tag_arr(_tmp7F2,sizeof(char),_get_zero_arr_size(
_tmp7F2,22));}),_tag_arr(_tmp7F1,sizeof(void*),0));});else{void*_tmp7F4;struct
_tuple2 _tmp7F3=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp725,fmt_desc_arg - 1);_tmp7F4=_tmp7F3.f3;{void*_tmp7F5=Cyc_Tcutil_compress(
_tmp7F4);struct Cyc_Absyn_PtrInfo _tmp7F6;void*_tmp7F7;struct Cyc_Absyn_PtrAtts
_tmp7F8;struct Cyc_Absyn_Conref*_tmp7F9;struct Cyc_Absyn_Conref*_tmp7FA;_LL518: if(
_tmp7F5 <= (void*)3  || *((int*)_tmp7F5)!= 4)goto _LL51A;_tmp7F6=((struct Cyc_Absyn_PointerType_struct*)
_tmp7F5)->f1;_tmp7F7=(void*)_tmp7F6.elt_typ;_tmp7F8=_tmp7F6.ptr_atts;_tmp7F9=
_tmp7F8.bounds;_tmp7FA=_tmp7F8.zero_term;_LL519:{struct _tuple6 _tmp7FC=({struct
_tuple6 _tmp7FB;_tmp7FB.f1=Cyc_Tcutil_compress(_tmp7F7);_tmp7FB.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp7F9);_tmp7FB;});void*_tmp7FD;void*_tmp7FE;void*_tmp7FF;void*
_tmp800;_LL51D: _tmp7FD=_tmp7FC.f1;if(_tmp7FD <= (void*)3  || *((int*)_tmp7FD)!= 5)
goto _LL51F;_tmp7FE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FD)->f1;if((int)
_tmp7FE != 2)goto _LL51F;_tmp7FF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp7FD)->f2;
if((int)_tmp7FF != 0)goto _LL51F;_tmp800=_tmp7FC.f2;if((int)_tmp800 != 0)goto _LL51F;
_LL51E: goto _LL51C;_LL51F:;_LL520:({void*_tmp801[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp802="format descriptor is not a char ? type";_tag_arr(_tmp802,
sizeof(char),_get_zero_arr_size(_tmp802,39));}),_tag_arr(_tmp801,sizeof(void*),0));});
goto _LL51C;_LL51C:;}goto _LL517;_LL51A:;_LL51B:({void*_tmp803[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp804="format descriptor is not a char ? type";_tag_arr(
_tmp804,sizeof(char),_get_zero_arr_size(_tmp804,39));}),_tag_arr(_tmp803,sizeof(
void*),0));});goto _LL517;_LL517:;}if(fmt_arg_start != 0){int problem;{struct
_tuple6 _tmp806=({struct _tuple6 _tmp805;_tmp805.f1=ft;_tmp805.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp727))->type);_tmp805;});void*
_tmp807;void*_tmp808;struct Cyc_Absyn_TunionInfo _tmp809;void*_tmp80A;struct Cyc_Absyn_Tuniondecl**
_tmp80B;struct Cyc_Absyn_Tuniondecl*_tmp80C;void*_tmp80D;void*_tmp80E;struct Cyc_Absyn_TunionInfo
_tmp80F;void*_tmp810;struct Cyc_Absyn_Tuniondecl**_tmp811;struct Cyc_Absyn_Tuniondecl*
_tmp812;_LL522: _tmp807=_tmp806.f1;if((int)_tmp807 != 0)goto _LL524;_tmp808=_tmp806.f2;
if(_tmp808 <= (void*)3  || *((int*)_tmp808)!= 2)goto _LL524;_tmp809=((struct Cyc_Absyn_TunionType_struct*)
_tmp808)->f1;_tmp80A=(void*)_tmp809.tunion_info;if(*((int*)_tmp80A)!= 1)goto
_LL524;_tmp80B=((struct Cyc_Absyn_KnownTunion_struct*)_tmp80A)->f1;_tmp80C=*
_tmp80B;_LL523: problem=Cyc_Absyn_qvar_cmp(_tmp80C->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL521;_LL524: _tmp80D=_tmp806.f1;if((int)_tmp80D != 1)goto _LL526;_tmp80E=
_tmp806.f2;if(_tmp80E <= (void*)3  || *((int*)_tmp80E)!= 2)goto _LL526;_tmp80F=((
struct Cyc_Absyn_TunionType_struct*)_tmp80E)->f1;_tmp810=(void*)_tmp80F.tunion_info;
if(*((int*)_tmp810)!= 1)goto _LL526;_tmp811=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp810)->f1;_tmp812=*_tmp811;_LL525: problem=Cyc_Absyn_qvar_cmp(_tmp812->name,
Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL521;_LL526:;_LL527: problem=1;goto
_LL521;_LL521:;}if(problem)({void*_tmp813[0]={};Cyc_Tcutil_terr(loc,({const char*
_tmp814="format attribute and vararg types don't match";_tag_arr(_tmp814,sizeof(
char),_get_zero_arr_size(_tmp814,46));}),_tag_arr(_tmp813,sizeof(void*),0));});}}}{
struct Cyc_List_List*rpo=_tmp728;for(0;rpo != 0;rpo=rpo->tl){struct _tuple6 _tmp816;
void*_tmp817;void*_tmp818;struct _tuple6*_tmp815=(struct _tuple6*)rpo->hd;_tmp816=*
_tmp815;_tmp817=_tmp816.f1;_tmp818=_tmp816.f2;_tmp7E3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E3,(void*)4,_tmp817);_tmp7E3=Cyc_Tcutil_i_check_valid_type(loc,te,
_tmp7E3,(void*)3,_tmp818);}}if(*_tmp723 != 0)_tmp7E3=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp7E3,(void*)4,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp723))->v);
else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmp7E3.free_vars;
for(0;tvs != 0;tvs=tvs->tl){void*_tmp819=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);struct Cyc_Core_Opt*_tmp81A;struct Cyc_Core_Opt**_tmp81B;void*
_tmp81C;void*_tmp81D;void*_tmp81E;void*_tmp81F;struct Cyc_Core_Opt*_tmp820;struct
Cyc_Core_Opt**_tmp821;void*_tmp822;void*_tmp823;struct Cyc_Core_Opt*_tmp824;
struct Cyc_Core_Opt**_tmp825;_LL529: if(*((int*)_tmp819)!= 2)goto _LL52B;_tmp81A=((
struct Cyc_Absyn_Less_kb_struct*)_tmp819)->f1;_tmp81B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp819)->f1;_tmp81C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp819)->f2;if((int)_tmp81C != 3)goto _LL52B;_LL52A:*_tmp81B=({struct Cyc_Core_Opt*
_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp828;_tmp828.tag=0;_tmp828.f1=(void*)((void*)3);_tmp828;});_tmp827;}));
_tmp826;});goto _LL52C;_LL52B: if(*((int*)_tmp819)!= 0)goto _LL52D;_tmp81D=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp819)->f1;if((int)_tmp81D != 3)goto _LL52D;_LL52C:
effect=({struct Cyc_List_List*_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp82A=_cycalloc(sizeof(*
_tmp82A));_tmp82A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp82B;_tmp82B.tag=19;
_tmp82B.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp82C=_cycalloc(
sizeof(*_tmp82C));_tmp82C[0]=({struct Cyc_Absyn_VarType_struct _tmp82D;_tmp82D.tag=
1;_tmp82D.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp82D;});_tmp82C;}));_tmp82B;});
_tmp82A;}));_tmp829->tl=effect;_tmp829;});goto _LL528;_LL52D: if(*((int*)_tmp819)
!= 2)goto _LL52F;_tmp81E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp819)->f2;
if((int)_tmp81E != 5)goto _LL52F;_LL52E: goto _LL530;_LL52F: if(*((int*)_tmp819)!= 0)
goto _LL531;_tmp81F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp819)->f1;if((int)
_tmp81F != 5)goto _LL531;_LL530: goto _LL528;_LL531: if(*((int*)_tmp819)!= 2)goto
_LL533;_tmp820=((struct Cyc_Absyn_Less_kb_struct*)_tmp819)->f1;_tmp821=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp819)->f1;_tmp822=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp819)->f2;if((int)_tmp822 != 4)goto _LL533;_LL532:*_tmp821=({struct Cyc_Core_Opt*
_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp830;_tmp830.tag=0;_tmp830.f1=(void*)((void*)4);_tmp830;});_tmp82F;}));
_tmp82E;});goto _LL534;_LL533: if(*((int*)_tmp819)!= 0)goto _LL535;_tmp823=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp819)->f1;if((int)_tmp823 != 4)goto _LL535;_LL534:
effect=({struct Cyc_List_List*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->hd=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp832=_cycalloc(sizeof(*_tmp832));
_tmp832[0]=({struct Cyc_Absyn_VarType_struct _tmp833;_tmp833.tag=1;_tmp833.f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp833;});_tmp832;}));_tmp831->tl=effect;_tmp831;});
goto _LL528;_LL535: if(*((int*)_tmp819)!= 1)goto _LL537;_tmp824=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp819)->f1;_tmp825=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp819)->f1;_LL536:*_tmp825=({struct Cyc_Core_Opt*_tmp834=_cycalloc(sizeof(*
_tmp834));_tmp834->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmp835=
_cycalloc(sizeof(*_tmp835));_tmp835[0]=({struct Cyc_Absyn_Less_kb_struct _tmp836;
_tmp836.tag=2;_tmp836.f1=0;_tmp836.f2=(void*)((void*)0);_tmp836;});_tmp835;}));
_tmp834;});goto _LL538;_LL537:;_LL538: effect=({struct Cyc_List_List*_tmp837=
_cycalloc(sizeof(*_tmp837));_tmp837->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp839;_tmp839.tag=21;_tmp839.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A[0]=({struct Cyc_Absyn_VarType_struct
_tmp83B;_tmp83B.tag=1;_tmp83B.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp83B;});
_tmp83A;}));_tmp839;});_tmp838;}));_tmp837->tl=effect;_tmp837;});goto _LL528;
_LL528:;}}{struct Cyc_List_List*ts=_tmp7E3.free_evars;for(0;ts != 0;ts=ts->tl){
void*_tmp83C=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL53A: if((int)_tmp83C != 3)goto
_LL53C;_LL53B: effect=({struct Cyc_List_List*_tmp83D=_cycalloc(sizeof(*_tmp83D));
_tmp83D->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp83E=_cycalloc(
sizeof(*_tmp83E));_tmp83E[0]=({struct Cyc_Absyn_AccessEff_struct _tmp83F;_tmp83F.tag=
19;_tmp83F.f1=(void*)((void*)ts->hd);_tmp83F;});_tmp83E;}));_tmp83D->tl=effect;
_tmp83D;});goto _LL539;_LL53C: if((int)_tmp83C != 4)goto _LL53E;_LL53D: effect=({
struct Cyc_List_List*_tmp840=_cycalloc(sizeof(*_tmp840));_tmp840->hd=(void*)((
void*)ts->hd);_tmp840->tl=effect;_tmp840;});goto _LL539;_LL53E: if((int)_tmp83C != 
5)goto _LL540;_LL53F: goto _LL539;_LL540:;_LL541: effect=({struct Cyc_List_List*
_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp843;_tmp843.tag=21;_tmp843.f1=(void*)((void*)ts->hd);_tmp843;});_tmp842;}));
_tmp841->tl=effect;_tmp841;});goto _LL539;_LL539:;}}*_tmp723=({struct Cyc_Core_Opt*
_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp846;_tmp846.tag=20;_tmp846.f1=effect;_tmp846;});_tmp845;}));_tmp844;});}if(*
_tmp721 != 0){struct Cyc_List_List*bs=*_tmp721;for(0;bs != 0;bs=bs->tl){void*
_tmp847=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp848;struct Cyc_Core_Opt**_tmp849;struct Cyc_Core_Opt*
_tmp84A;struct Cyc_Core_Opt**_tmp84B;void*_tmp84C;struct Cyc_Core_Opt*_tmp84D;
struct Cyc_Core_Opt**_tmp84E;void*_tmp84F;struct Cyc_Core_Opt*_tmp850;struct Cyc_Core_Opt**
_tmp851;void*_tmp852;void*_tmp853;_LL543: if(*((int*)_tmp847)!= 1)goto _LL545;
_tmp848=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp847)->f1;_tmp849=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp847)->f1;_LL544:({struct Cyc_String_pa_struct
_tmp856;_tmp856.tag=0;_tmp856.f1=(struct _tagged_arr)((struct _tagged_arr)*((
struct Cyc_Absyn_Tvar*)bs->hd)->name);{void*_tmp854[1]={& _tmp856};Cyc_Tcutil_warn(
loc,({const char*_tmp855="Type variable %s unconstrained, assuming boxed";
_tag_arr(_tmp855,sizeof(char),_get_zero_arr_size(_tmp855,47));}),_tag_arr(
_tmp854,sizeof(void*),1));}});_tmp84B=_tmp849;goto _LL546;_LL545: if(*((int*)
_tmp847)!= 2)goto _LL547;_tmp84A=((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f1;
_tmp84B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f1;
_tmp84C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f2;if((int)_tmp84C != 
1)goto _LL547;_LL546: _tmp84E=_tmp84B;goto _LL548;_LL547: if(*((int*)_tmp847)!= 2)
goto _LL549;_tmp84D=((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f1;_tmp84E=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f1;_tmp84F=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f2;if((int)_tmp84F != 0)goto
_LL549;_LL548:*_tmp84E=({struct Cyc_Core_Opt*_tmp857=_cycalloc(sizeof(*_tmp857));
_tmp857->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp858=_cycalloc(
sizeof(*_tmp858));_tmp858[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp859;_tmp859.tag=0;
_tmp859.f1=(void*)((void*)2);_tmp859;});_tmp858;}));_tmp857;});goto _LL542;_LL549:
if(*((int*)_tmp847)!= 2)goto _LL54B;_tmp850=((struct Cyc_Absyn_Less_kb_struct*)
_tmp847)->f1;_tmp851=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp847)->f1;_tmp852=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp847)->f2;
_LL54A:*_tmp851=({struct Cyc_Core_Opt*_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp85B=_cycalloc(sizeof(*_tmp85B));
_tmp85B[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp85C;_tmp85C.tag=0;_tmp85C.f1=(void*)
_tmp852;_tmp85C;});_tmp85B;}));_tmp85A;});goto _LL542;_LL54B: if(*((int*)_tmp847)
!= 0)goto _LL54D;_tmp853=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp847)->f1;if((
int)_tmp853 != 1)goto _LL54D;_LL54C:({void*_tmp85D[0]={};Cyc_Tcutil_terr(loc,({
const char*_tmp85E="functions can't abstract M types";_tag_arr(_tmp85E,sizeof(
char),_get_zero_arr_size(_tmp85E,33));}),_tag_arr(_tmp85D,sizeof(void*),0));});
goto _LL542;_LL54D:;_LL54E: goto _LL542;_LL542:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp7E3.kind_env,*_tmp721);_tmp7E3.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp7E3.free_vars,*_tmp721);{struct Cyc_List_List*tvs=_tmp7E3.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp7E3.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL489;}}_LL4A8: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 
9)goto _LL4AA;_tmp72A=((struct Cyc_Absyn_TupleType_struct*)_tmp6F8)->f1;_LL4A9:
for(0;_tmp72A != 0;_tmp72A=_tmp72A->tl){struct _tuple4*_tmp860=(struct _tuple4*)
_tmp72A->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*
_tmp860).f2);((*_tmp860).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp860).f1).print_const,(*_tmp860).f2);}goto _LL489;_LL4AA: if(_tmp6F8 <= (
void*)3  || *((int*)_tmp6F8)!= 11)goto _LL4AC;_tmp72B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6F8)->f1;_tmp72C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6F8)->f2;_LL4AB:{
struct _RegionHandle _tmp861=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp861;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp72C != 0;_tmp72C=_tmp72C->tl){struct Cyc_Absyn_Aggrfield _tmp863;struct
_tagged_arr*_tmp864;struct Cyc_Absyn_Tqual _tmp865;struct Cyc_Absyn_Tqual*_tmp866;
void*_tmp867;struct Cyc_Absyn_Exp*_tmp868;struct Cyc_List_List*_tmp869;struct Cyc_Absyn_Aggrfield*
_tmp862=(struct Cyc_Absyn_Aggrfield*)_tmp72C->hd;_tmp863=*_tmp862;_tmp864=_tmp863.name;
_tmp865=_tmp863.tq;_tmp866=(struct Cyc_Absyn_Tqual*)&(*_tmp862).tq;_tmp867=(void*)
_tmp863.type;_tmp868=_tmp863.width;_tmp869=_tmp863.attributes;if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp864))({struct Cyc_String_pa_struct
_tmp86C;_tmp86C.tag=0;_tmp86C.f1=(struct _tagged_arr)((struct _tagged_arr)*_tmp864);{
void*_tmp86A[1]={& _tmp86C};Cyc_Tcutil_terr(loc,({const char*_tmp86B="duplicate field %s";
_tag_arr(_tmp86B,sizeof(char),_get_zero_arr_size(_tmp86B,19));}),_tag_arr(
_tmp86A,sizeof(void*),1));}});if(Cyc_strcmp((struct _tagged_arr)*_tmp864,({const
char*_tmp86D="";_tag_arr(_tmp86D,sizeof(char),_get_zero_arr_size(_tmp86D,1));}))
!= 0)prev_fields=({struct Cyc_List_List*_tmp86E=_region_malloc(aprev_rgn,sizeof(*
_tmp86E));_tmp86E->hd=_tmp864;_tmp86E->tl=prev_fields;_tmp86E;});cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp867);_tmp866->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp866->print_const,_tmp867);if(_tmp72B == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp867))({struct Cyc_String_pa_struct _tmp871;_tmp871.tag=0;_tmp871.f1=(struct
_tagged_arr)((struct _tagged_arr)*_tmp864);{void*_tmp86F[1]={& _tmp871};Cyc_Tcutil_warn(
loc,({const char*_tmp870="union member %s is not `bits-only' so it can only be written and not read";
_tag_arr(_tmp870,sizeof(char),_get_zero_arr_size(_tmp870,74));}),_tag_arr(
_tmp86F,sizeof(void*),1));}});Cyc_Tcutil_check_bitfield(loc,te,_tmp867,_tmp868,
_tmp864);Cyc_Tcutil_check_field_atts(loc,_tmp864,_tmp869);}};_pop_region(
aprev_rgn);}goto _LL489;_LL4AC: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 10)
goto _LL4AE;_tmp72D=((struct Cyc_Absyn_AggrType_struct*)_tmp6F8)->f1;_tmp72E=(void*)
_tmp72D.aggr_info;_tmp72F=(void**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp6F8)->f1).aggr_info;
_tmp730=_tmp72D.targs;_tmp731=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp6F8)->f1).targs;_LL4AD:{void*_tmp872=*_tmp72F;void*_tmp873;struct _tuple1*
_tmp874;struct Cyc_Absyn_Aggrdecl**_tmp875;struct Cyc_Absyn_Aggrdecl*_tmp876;
_LL550: if(*((int*)_tmp872)!= 0)goto _LL552;_tmp873=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp872)->f1;_tmp874=((struct Cyc_Absyn_UnknownAggr_struct*)_tmp872)->f2;_LL551: {
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp877;_push_handler(& _tmp877);{
int _tmp879=0;if(setjmp(_tmp877.handler))_tmp879=1;if(!_tmp879){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp874);*_tmp72F=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp87A=
_cycalloc(sizeof(*_tmp87A));_tmp87A[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp87B;
_tmp87B.tag=1;_tmp87B.f1=adp;_tmp87B;});_tmp87A;});;_pop_handler();}else{void*
_tmp878=(void*)_exn_thrown;void*_tmp87D=_tmp878;_LL555: if(_tmp87D != Cyc_Dict_Absent)
goto _LL557;_LL556: {struct Cyc_Tcenv_Genv*_tmp87E=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp87F=({struct Cyc_Absyn_Aggrdecl*_tmp885=_cycalloc(
sizeof(*_tmp885));_tmp885->kind=(void*)_tmp873;_tmp885->sc=(void*)((void*)3);
_tmp885->name=_tmp874;_tmp885->tvs=0;_tmp885->impl=0;_tmp885->attributes=0;
_tmp885;});Cyc_Tc_tcAggrdecl(te,_tmp87E,loc,_tmp87F);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp874);*_tmp72F=(void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp880=
_cycalloc(sizeof(*_tmp880));_tmp880[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp881;
_tmp881.tag=1;_tmp881.f1=adp;_tmp881;});_tmp880;});if(*_tmp731 != 0){({struct Cyc_String_pa_struct
_tmp884;_tmp884.tag=0;_tmp884.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp874));{void*_tmp882[1]={& _tmp884};Cyc_Tcutil_terr(loc,({const char*_tmp883="declare parameterized type %s before using";
_tag_arr(_tmp883,sizeof(char),_get_zero_arr_size(_tmp883,43));}),_tag_arr(
_tmp882,sizeof(void*),1));}});return cvtenv;}goto _LL554;}_LL557:;_LL558:(void)
_throw(_tmp87D);_LL554:;}}}_tmp876=*adp;goto _LL553;}_LL552: if(*((int*)_tmp872)!= 
1)goto _LL54F;_tmp875=((struct Cyc_Absyn_KnownAggr_struct*)_tmp872)->f1;_tmp876=*
_tmp875;_LL553: {struct Cyc_List_List*tvs=_tmp876->tvs;struct Cyc_List_List*ts=*
_tmp731;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0)({struct Cyc_String_pa_struct _tmp888;_tmp888.tag=
0;_tmp888.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp876->name));{void*_tmp886[1]={& _tmp888};Cyc_Tcutil_terr(loc,({const char*
_tmp887="too many parameters for type %s";_tag_arr(_tmp887,sizeof(char),
_get_zero_arr_size(_tmp887,32));}),_tag_arr(_tmp886,sizeof(void*),1));}});if(tvs
!= 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889->hd=(void*)e;_tmp889->tl=
hidden_ts;_tmp889;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp731=Cyc_List_imp_append(*_tmp731,Cyc_List_imp_rev(hidden_ts));}}_LL54F:;}
goto _LL489;_LL4AE: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 16)goto _LL4B0;
_tmp732=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f1;_tmp733=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F8)->f2;_tmp734=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp6F8)->f2;_tmp735=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f3;_tmp736=(
struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f3;
_tmp737=((struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f4;_tmp738=(void***)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp6F8)->f4;_LL4AF: {struct Cyc_List_List*
targs=*_tmp734;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp88A;
_push_handler(& _tmp88A);{int _tmp88C=0;if(setjmp(_tmp88A.handler))_tmp88C=1;if(!
_tmp88C){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp732);;_pop_handler();}else{
void*_tmp88B=(void*)_exn_thrown;void*_tmp88E=_tmp88B;_LL55A: if(_tmp88E != Cyc_Dict_Absent)
goto _LL55C;_LL55B:({struct Cyc_String_pa_struct _tmp891;_tmp891.tag=0;_tmp891.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp732));{void*
_tmp88F[1]={& _tmp891};Cyc_Tcutil_terr(loc,({const char*_tmp890="unbound typedef name %s";
_tag_arr(_tmp890,sizeof(char),_get_zero_arr_size(_tmp890,24));}),_tag_arr(
_tmp88F,sizeof(void*),1));}});return cvtenv;_LL55C:;_LL55D:(void)_throw(_tmp88E);
_LL559:;}}}*_tmp736=(struct Cyc_Absyn_Typedefdecl*)td;_tmp732[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp892=_cycalloc(
sizeof(*_tmp892));_tmp892->hd=({struct _tuple8*_tmp893=_cycalloc(sizeof(*_tmp893));
_tmp893->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp893->f2=(void*)ts->hd;_tmp893;});
_tmp892->tl=inst;_tmp892;});}if(ts != 0)({struct Cyc_String_pa_struct _tmp896;
_tmp896.tag=0;_tmp896.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp732));{void*_tmp894[1]={& _tmp896};Cyc_Tcutil_terr(loc,({const char*_tmp895="too many parameters for typedef %s";
_tag_arr(_tmp895,sizeof(char),_get_zero_arr_size(_tmp895,35));}),_tag_arr(
_tmp894,sizeof(void*),1));}});if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp897=_cycalloc(
sizeof(*_tmp897));_tmp897->hd=(void*)e;_tmp897->tl=hidden_ts;_tmp897;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);inst=({struct Cyc_List_List*
_tmp898=_cycalloc(sizeof(*_tmp898));_tmp898->hd=({struct _tuple8*_tmp899=
_cycalloc(sizeof(*_tmp899));_tmp899->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp899->f2=
e;_tmp899;});_tmp898->tl=inst;_tmp898;});}*_tmp734=Cyc_List_imp_append(targs,Cyc_List_imp_rev(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_substitute(inst,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);*_tmp738=({void**_tmp89A=
_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=new_typ;_tmp89A;});}goto _LL489;}}_LL4B0:
if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 18)goto _LL4B2;_LL4B1: goto _LL4B3;
_LL4B2: if((int)_tmp6F8 != 2)goto _LL4B4;_LL4B3: goto _LL489;_LL4B4: if(_tmp6F8 <= (
void*)3  || *((int*)_tmp6F8)!= 15)goto _LL4B6;_tmp739=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6F8)->f1;_LL4B5: _tmp73A=_tmp739;goto _LL4B7;_LL4B6: if(_tmp6F8 <= (void*)3  || *((
int*)_tmp6F8)!= 19)goto _LL4B8;_tmp73A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6F8)->f1;_LL4B7: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp73A);goto _LL489;_LL4B8: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 21)goto
_LL4BA;_tmp73B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6F8)->f1;_LL4B9:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp73B);goto _LL489;
_LL4BA: if(_tmp6F8 <= (void*)3  || *((int*)_tmp6F8)!= 20)goto _LL489;_tmp73C=((
struct Cyc_Absyn_JoinEff_struct*)_tmp6F8)->f1;_LL4BB: for(0;_tmp73C != 0;_tmp73C=
_tmp73C->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp73C->hd);}goto _LL489;_LL489:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind))({struct Cyc_String_pa_struct _tmp89F;_tmp89F.tag=0;_tmp89F.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind));{
struct Cyc_String_pa_struct _tmp89E;_tmp89E.tag=0;_tmp89E.f1=(struct _tagged_arr)((
struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));{struct Cyc_String_pa_struct
_tmp89D;_tmp89D.tag=0;_tmp89D.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{void*_tmp89B[3]={& _tmp89D,& _tmp89E,& _tmp89F};Cyc_Tcutil_terr(loc,({const
char*_tmp89C="type %s has kind %s but as used here needs kind %s";_tag_arr(
_tmp89C,sizeof(char),_get_zero_arr_size(_tmp89C,51));}),_tag_arr(_tmp89B,sizeof(
void*),3));}}}});return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
kind_env,void*expected_kind,int generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp8A0=Cyc_Tcutil_i_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp8A9;
_tmp8A9.kind_env=kind_env;_tmp8A9.free_vars=0;_tmp8A9.free_evars=0;_tmp8A9.generalize_evars=
generalize_evars;_tmp8A9.fn_result=0;_tmp8A9;}),expected_kind,t);{struct Cyc_List_List*
vs=_tmp8A0.free_vars;for(0;vs != 0;vs=vs->tl){_tmp8A0.kind_env=Cyc_Tcutil_fast_add_free_tvar(
kind_env,(struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp8A0.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp8A1=Cyc_Tcutil_compress((void*)evs->hd);
struct Cyc_Core_Opt*_tmp8A2;struct Cyc_Core_Opt**_tmp8A3;_LL55F: if(_tmp8A1 <= (void*)
3  || *((int*)_tmp8A1)!= 0)goto _LL561;_tmp8A2=((struct Cyc_Absyn_Evar_struct*)
_tmp8A1)->f4;_tmp8A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp8A1)->f4;_LL560: if(*_tmp8A3 == 0)*_tmp8A3=({struct Cyc_Core_Opt*_tmp8A4=
_cycalloc(sizeof(*_tmp8A4));_tmp8A4->v=kind_env;_tmp8A4;});else{struct Cyc_List_List*
_tmp8A5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp8A3))->v;
struct Cyc_List_List*_tmp8A6=0;for(0;_tmp8A5 != 0;_tmp8A5=_tmp8A5->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct
Cyc_Absyn_Tvar*)_tmp8A5->hd))_tmp8A6=({struct Cyc_List_List*_tmp8A7=_cycalloc(
sizeof(*_tmp8A7));_tmp8A7->hd=(struct Cyc_Absyn_Tvar*)_tmp8A5->hd;_tmp8A7->tl=
_tmp8A6;_tmp8A7;});}*_tmp8A3=({struct Cyc_Core_Opt*_tmp8A8=_cycalloc(sizeof(*
_tmp8A8));_tmp8A8->v=_tmp8A6;_tmp8A8;});}goto _LL55E;_LL561:;_LL562: goto _LL55E;
_LL55E:;}}return _tmp8A0;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp8AA=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp8AB=_tmp8AA.free_vars;struct Cyc_List_List*
_tmp8AC=_tmp8AA.free_evars;{struct Cyc_List_List*x=_tmp8AB;for(0;x != 0;x=x->tl){
void*_tmp8AD=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp8AE;struct Cyc_Core_Opt**_tmp8AF;struct Cyc_Core_Opt*
_tmp8B0;struct Cyc_Core_Opt**_tmp8B1;void*_tmp8B2;struct Cyc_Core_Opt*_tmp8B3;
struct Cyc_Core_Opt**_tmp8B4;void*_tmp8B5;struct Cyc_Core_Opt*_tmp8B6;struct Cyc_Core_Opt**
_tmp8B7;void*_tmp8B8;void*_tmp8B9;_LL564: if(*((int*)_tmp8AD)!= 1)goto _LL566;
_tmp8AE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AD)->f1;_tmp8AF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8AD)->f1;_LL565: _tmp8B1=_tmp8AF;goto
_LL567;_LL566: if(*((int*)_tmp8AD)!= 2)goto _LL568;_tmp8B0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AD)->f1;_tmp8B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AD)->f1;_tmp8B2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;if((
int)_tmp8B2 != 1)goto _LL568;_LL567: _tmp8B4=_tmp8B1;goto _LL569;_LL568: if(*((int*)
_tmp8AD)!= 2)goto _LL56A;_tmp8B3=((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8B4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f1;
_tmp8B5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;if((int)_tmp8B5 != 
0)goto _LL56A;_LL569:*_tmp8B4=({struct Cyc_Core_Opt*_tmp8BA=_cycalloc(sizeof(*
_tmp8BA));_tmp8BA->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BB=
_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8BC;
_tmp8BC.tag=0;_tmp8BC.f1=(void*)((void*)2);_tmp8BC;});_tmp8BB;}));_tmp8BA;});
goto _LL563;_LL56A: if(*((int*)_tmp8AD)!= 2)goto _LL56C;_tmp8B6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AD)->f1;_tmp8B7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8AD)->f1;_tmp8B8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8AD)->f2;
_LL56B:*_tmp8B7=({struct Cyc_Core_Opt*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp8BE=_cycalloc(sizeof(*_tmp8BE));
_tmp8BE[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp8BF;_tmp8BF.tag=0;_tmp8BF.f1=(void*)
_tmp8B8;_tmp8BF;});_tmp8BE;}));_tmp8BD;});goto _LL563;_LL56C: if(*((int*)_tmp8AD)
!= 0)goto _LL56E;_tmp8B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8AD)->f1;if((
int)_tmp8B9 != 1)goto _LL56E;_LL56D:({struct Cyc_String_pa_struct _tmp8C2;_tmp8C2.tag=
0;_tmp8C2.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd));{void*_tmp8C0[1]={& _tmp8C2};Cyc_Tcutil_terr(loc,({
const char*_tmp8C1="type variable %s cannot have kind M";_tag_arr(_tmp8C1,sizeof(
char),_get_zero_arr_size(_tmp8C1,36));}),_tag_arr(_tmp8C0,sizeof(void*),1));}});
goto _LL563;_LL56E:;_LL56F: goto _LL563;_LL563:;}}if(_tmp8AB != 0  || _tmp8AC != 0){{
void*_tmp8C3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8C4;struct Cyc_List_List*
_tmp8C5;struct Cyc_List_List**_tmp8C6;struct Cyc_Core_Opt*_tmp8C7;void*_tmp8C8;
struct Cyc_List_List*_tmp8C9;int _tmp8CA;struct Cyc_Absyn_VarargInfo*_tmp8CB;struct
Cyc_List_List*_tmp8CC;struct Cyc_List_List*_tmp8CD;_LL571: if(_tmp8C3 <= (void*)3
 || *((int*)_tmp8C3)!= 8)goto _LL573;_tmp8C4=((struct Cyc_Absyn_FnType_struct*)
_tmp8C3)->f1;_tmp8C5=_tmp8C4.tvars;_tmp8C6=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp8C3)->f1).tvars;_tmp8C7=_tmp8C4.effect;_tmp8C8=(void*)_tmp8C4.ret_typ;
_tmp8C9=_tmp8C4.args;_tmp8CA=_tmp8C4.c_varargs;_tmp8CB=_tmp8C4.cyc_varargs;
_tmp8CC=_tmp8C4.rgn_po;_tmp8CD=_tmp8C4.attributes;_LL572: if(*_tmp8C6 == 0){*
_tmp8C6=_tmp8AB;_tmp8AB=0;}goto _LL570;_LL573:;_LL574: goto _LL570;_LL570:;}if(
_tmp8AB != 0)({struct Cyc_String_pa_struct _tmp8D0;_tmp8D0.tag=0;_tmp8D0.f1=(struct
_tagged_arr)((struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp8AB->hd)->name);{
void*_tmp8CE[1]={& _tmp8D0};Cyc_Tcutil_terr(loc,({const char*_tmp8CF="unbound type variable %s";
_tag_arr(_tmp8CF,sizeof(char),_get_zero_arr_size(_tmp8CF,25));}),_tag_arr(
_tmp8CE,sizeof(void*),1));}});if(_tmp8AC != 0)for(0;_tmp8AC != 0;_tmp8AC=_tmp8AC->tl){
void*e=(void*)_tmp8AC->hd;void*_tmp8D1=Cyc_Tcutil_typ_kind(e);_LL576: if((int)
_tmp8D1 != 3)goto _LL578;_LL577: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8D2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*
_tmp8D3="can't unify evar with heap!";_tag_arr(_tmp8D3,sizeof(char),
_get_zero_arr_size(_tmp8D3,28));}),_tag_arr(_tmp8D2,sizeof(void*),0));});goto
_LL575;_LL578: if((int)_tmp8D1 != 4)goto _LL57A;_LL579: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({
void*_tmp8D4[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp8D5="can't unify evar with {}!";_tag_arr(_tmp8D5,sizeof(char),
_get_zero_arr_size(_tmp8D5,26));}),_tag_arr(_tmp8D4,sizeof(void*),0));});goto
_LL575;_LL57A:;_LL57B:({struct Cyc_String_pa_struct _tmp8D9;_tmp8D9.tag=0;_tmp8D9.f1=(
struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp8D8;_tmp8D8.tag=0;_tmp8D8.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
e));{void*_tmp8D6[2]={& _tmp8D8,& _tmp8D9};Cyc_Tcutil_terr(loc,({const char*_tmp8D7="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8D7,sizeof(char),_get_zero_arr_size(_tmp8D7,52));}),_tag_arr(
_tmp8D6,sizeof(void*),2));}}});goto _LL575;_LL575:;}}}void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp8DA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp8DB;struct Cyc_List_List*
_tmp8DC;struct Cyc_Core_Opt*_tmp8DD;_LL57D: if(_tmp8DA <= (void*)3  || *((int*)
_tmp8DA)!= 8)goto _LL57F;_tmp8DB=((struct Cyc_Absyn_FnType_struct*)_tmp8DA)->f1;
_tmp8DC=_tmp8DB.tvars;_tmp8DD=_tmp8DB.effect;_LL57E: fd->tvs=_tmp8DC;fd->effect=
_tmp8DD;goto _LL57C;_LL57F:;_LL580:({void*_tmp8DE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8DF="check_fndecl_valid_type: not a FnType";
_tag_arr(_tmp8DF,sizeof(char),_get_zero_arr_size(_tmp8DF,38));}),_tag_arr(
_tmp8DE,sizeof(void*),0));});_LL57C:;}{struct _RegionHandle _tmp8E0=_new_region("r");
struct _RegionHandle*r=& _tmp8E0;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp8E1="function declaration has repeated parameter";
_tag_arr(_tmp8E1,sizeof(char),_get_zero_arr_size(_tmp8E1,44));}));;_pop_region(r);}
fd->cached_typ=({struct Cyc_Core_Opt*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));_tmp8E2->v=(
void*)t;_tmp8E2;});}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,void*expected_kind,int
allow_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp8E3=Cyc_Tcutil_check_valid_type(
loc,te,bound_tvars,expected_kind,0,t);struct Cyc_List_List*_tmp8E4=Cyc_Tcutil_remove_bound_tvars(
_tmp8E3.free_vars,bound_tvars);struct Cyc_List_List*_tmp8E5=_tmp8E3.free_evars;{
struct Cyc_List_List*fs=_tmp8E4;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp8E6=((
struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_String_pa_struct _tmp8EA;_tmp8EA.tag=
0;_tmp8EA.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(t));{
struct Cyc_String_pa_struct _tmp8E9;_tmp8E9.tag=0;_tmp8E9.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp8E6);{void*_tmp8E7[2]={& _tmp8E9,& _tmp8EA};Cyc_Tcutil_terr(
loc,({const char*_tmp8E8="unbound type variable %s in type %s";_tag_arr(_tmp8E8,
sizeof(char),_get_zero_arr_size(_tmp8E8,36));}),_tag_arr(_tmp8E7,sizeof(void*),2));}}});}}
if(!allow_evars  && _tmp8E5 != 0)for(0;_tmp8E5 != 0;_tmp8E5=_tmp8E5->tl){void*e=(
void*)_tmp8E5->hd;void*_tmp8EB=Cyc_Tcutil_typ_kind(e);_LL582: if((int)_tmp8EB != 3)
goto _LL584;_LL583: if(!Cyc_Tcutil_unify(e,(void*)2))({void*_tmp8EC[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8ED="can't unify evar with heap!";
_tag_arr(_tmp8ED,sizeof(char),_get_zero_arr_size(_tmp8ED,28));}),_tag_arr(
_tmp8EC,sizeof(void*),0));});goto _LL581;_LL584: if((int)_tmp8EB != 4)goto _LL586;
_LL585: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp8EE[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({const char*_tmp8EF="can't unify evar with {}!";
_tag_arr(_tmp8EF,sizeof(char),_get_zero_arr_size(_tmp8EF,26));}),_tag_arr(
_tmp8EE,sizeof(void*),0));});goto _LL581;_LL586:;_LL587:({struct Cyc_String_pa_struct
_tmp8F3;_tmp8F3.tag=0;_tmp8F3.f1=(struct _tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp8F2;_tmp8F2.tag=0;_tmp8F2.f1=(struct
_tagged_arr)((struct _tagged_arr)Cyc_Absynpp_typ2string(e));{void*_tmp8F0[2]={&
_tmp8F2,& _tmp8F3};Cyc_Tcutil_terr(loc,({const char*_tmp8F1="hidden type variable %s isn't abstracted in type %s";
_tag_arr(_tmp8F1,sizeof(char),_get_zero_arr_size(_tmp8F1,52));}),_tag_arr(
_tmp8F0,sizeof(void*),2));}}});goto _LL581;_LL581:;}}void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(
void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){struct Cyc_List_List*vs2=vs->tl;
for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(void*)vs2->hd)== 0)({struct Cyc_String_pa_struct
_tmp8F7;_tmp8F7.tag=0;_tmp8F7.f1=(struct _tagged_arr)((struct _tagged_arr)a2string((
void*)vs->hd));{struct Cyc_String_pa_struct _tmp8F6;_tmp8F6.tag=0;_tmp8F6.f1=(
struct _tagged_arr)((struct _tagged_arr)msg);{void*_tmp8F4[2]={& _tmp8F6,& _tmp8F7};
Cyc_Tcutil_terr(loc,({const char*_tmp8F5="%s: %s";_tag_arr(_tmp8F5,sizeof(char),
_get_zero_arr_size(_tmp8F5,7));}),_tag_arr(_tmp8F4,sizeof(void*),2));}}});}}}
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
const char*_tmp8F8="duplicate type variable";_tag_arr(_tmp8F8,sizeof(char),
_get_zero_arr_size(_tmp8F8,24));}));}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
int f2;};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_strcmp((
struct _tagged_arr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*
_tmp8F9="";_tag_arr(_tmp8F9,sizeof(char),_get_zero_arr_size(_tmp8F9,1));}))!= 0)
fields=({struct Cyc_List_List*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->hd=({
struct _tuple18*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd;_tmp8FB->f2=0;_tmp8FB;});_tmp8FA->tl=fields;_tmp8FA;});}}fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct
Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple19 _tmp8FD;struct Cyc_List_List*
_tmp8FE;void*_tmp8FF;struct _tuple19*_tmp8FC=(struct _tuple19*)des->hd;_tmp8FD=*
_tmp8FC;_tmp8FE=_tmp8FD.f1;_tmp8FF=_tmp8FD.f2;if(_tmp8FE == 0){struct Cyc_List_List*
_tmp900=fields;for(0;_tmp900 != 0;_tmp900=_tmp900->tl){if(!(*((struct _tuple18*)
_tmp900->hd)).f2){(*((struct _tuple18*)_tmp900->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp901=_cycalloc(sizeof(*_tmp901));
_tmp901->hd=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp902=_cycalloc(
sizeof(*_tmp902));_tmp902[0]=({struct Cyc_Absyn_FieldName_struct _tmp903;_tmp903.tag=
1;_tmp903.f1=((*((struct _tuple18*)_tmp900->hd)).f1)->name;_tmp903;});_tmp902;}));
_tmp901->tl=0;_tmp901;});ans=({struct Cyc_List_List*_tmp904=_region_malloc(rgn,
sizeof(*_tmp904));_tmp904->hd=({struct _tuple20*_tmp905=_region_malloc(rgn,
sizeof(*_tmp905));_tmp905->f1=(*((struct _tuple18*)_tmp900->hd)).f1;_tmp905->f2=
_tmp8FF;_tmp905;});_tmp904->tl=ans;_tmp904;});break;}}if(_tmp900 == 0)({void*
_tmp906[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp907="too many arguments to struct";
_tag_arr(_tmp907,sizeof(char),_get_zero_arr_size(_tmp907,29));}),_tag_arr(
_tmp906,sizeof(void*),0));});}else{if(_tmp8FE->tl != 0)({void*_tmp908[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp909="multiple designators are not supported";_tag_arr(
_tmp909,sizeof(char),_get_zero_arr_size(_tmp909,39));}),_tag_arr(_tmp908,sizeof(
void*),0));});else{void*_tmp90A=(void*)_tmp8FE->hd;struct _tagged_arr*_tmp90B;
_LL589: if(*((int*)_tmp90A)!= 0)goto _LL58B;_LL58A:({void*_tmp90C[0]={};Cyc_Tcutil_terr(
loc,({const char*_tmp90D="array designator used in argument to struct";_tag_arr(
_tmp90D,sizeof(char),_get_zero_arr_size(_tmp90D,44));}),_tag_arr(_tmp90C,sizeof(
void*),0));});goto _LL588;_LL58B: if(*((int*)_tmp90A)!= 1)goto _LL588;_tmp90B=((
struct Cyc_Absyn_FieldName_struct*)_tmp90A)->f1;_LL58C: {struct Cyc_List_List*
_tmp90E=fields;for(0;_tmp90E != 0;_tmp90E=_tmp90E->tl){if(Cyc_strptrcmp(_tmp90B,((*((
struct _tuple18*)_tmp90E->hd)).f1)->name)== 0){if((*((struct _tuple18*)_tmp90E->hd)).f2)({
struct Cyc_String_pa_struct _tmp911;_tmp911.tag=0;_tmp911.f1=(struct _tagged_arr)((
struct _tagged_arr)*_tmp90B);{void*_tmp90F[1]={& _tmp911};Cyc_Tcutil_terr(loc,({
const char*_tmp910="field %s has already been used as an argument";_tag_arr(
_tmp910,sizeof(char),_get_zero_arr_size(_tmp910,46));}),_tag_arr(_tmp90F,sizeof(
void*),1));}});(*((struct _tuple18*)_tmp90E->hd)).f2=1;ans=({struct Cyc_List_List*
_tmp912=_region_malloc(rgn,sizeof(*_tmp912));_tmp912->hd=({struct _tuple20*
_tmp913=_region_malloc(rgn,sizeof(*_tmp913));_tmp913->f1=(*((struct _tuple18*)
_tmp90E->hd)).f1;_tmp913->f2=_tmp8FF;_tmp913;});_tmp912->tl=ans;_tmp912;});
break;}}if(_tmp90E == 0)({struct Cyc_String_pa_struct _tmp916;_tmp916.tag=0;_tmp916.f1=(
struct _tagged_arr)((struct _tagged_arr)*_tmp90B);{void*_tmp914[1]={& _tmp916};Cyc_Tcutil_terr(
loc,({const char*_tmp915="bad field designator %s";_tag_arr(_tmp915,sizeof(char),
_get_zero_arr_size(_tmp915,24));}),_tag_arr(_tmp914,sizeof(void*),1));}});goto
_LL588;}_LL588:;}}}for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)
fields->hd)).f2){({void*_tmp917[0]={};Cyc_Tcutil_terr(loc,({const char*_tmp918="too few arguments to struct";
_tag_arr(_tmp918,sizeof(char),_get_zero_arr_size(_tmp918,28));}),_tag_arr(
_tmp917,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmp919=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp91A;
void*_tmp91B;struct Cyc_Absyn_PtrAtts _tmp91C;struct Cyc_Absyn_Conref*_tmp91D;
_LL58E: if(_tmp919 <= (void*)3  || *((int*)_tmp919)!= 4)goto _LL590;_tmp91A=((struct
Cyc_Absyn_PointerType_struct*)_tmp919)->f1;_tmp91B=(void*)_tmp91A.elt_typ;
_tmp91C=_tmp91A.ptr_atts;_tmp91D=_tmp91C.bounds;_LL58F: {struct Cyc_Absyn_Conref*
_tmp91E=Cyc_Absyn_compress_conref(_tmp91D);void*_tmp91F=(void*)(Cyc_Absyn_compress_conref(
_tmp91E))->v;void*_tmp920;_LL593: if(_tmp91F <= (void*)1  || *((int*)_tmp91F)!= 0)
goto _LL595;_tmp920=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp91F)->f1;if((
int)_tmp920 != 0)goto _LL595;_LL594:*elt_typ_dest=_tmp91B;return 1;_LL595: if((int)
_tmp91F != 0)goto _LL597;_LL596:(void*)(_tmp91E->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp921=_cycalloc(sizeof(*_tmp921));_tmp921[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp922;_tmp922.tag=0;_tmp922.f1=(void*)((void*)0);_tmp922;});_tmp921;})));*
elt_typ_dest=_tmp91B;return 1;_LL597:;_LL598: return 0;_LL592:;}_LL590:;_LL591:
return 0;_LL58D:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp923=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp924;void*_tmp925;
struct Cyc_Absyn_PtrAtts _tmp926;struct Cyc_Absyn_Conref*_tmp927;_LL59A: if(_tmp923
<= (void*)3  || *((int*)_tmp923)!= 4)goto _LL59C;_tmp924=((struct Cyc_Absyn_PointerType_struct*)
_tmp923)->f1;_tmp925=(void*)_tmp924.elt_typ;_tmp926=_tmp924.ptr_atts;_tmp927=
_tmp926.zero_term;_LL59B:*elt_typ_dest=_tmp925;return((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp927);_LL59C:;_LL59D: return 0;_LL599:;}int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple10 bogus_ans=({struct _tuple10 _tmp96B;_tmp96B.f1=0;
_tmp96B.f2=(void*)2;_tmp96B;});void*_tmp928=(void*)e->r;struct _tuple1*_tmp929;
void*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;struct _tagged_arr*_tmp92C;struct Cyc_Absyn_Exp*
_tmp92D;struct _tagged_arr*_tmp92E;struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*
_tmp930;struct Cyc_Absyn_Exp*_tmp931;_LL59F: if(*((int*)_tmp928)!= 1)goto _LL5A1;
_tmp929=((struct Cyc_Absyn_Var_e_struct*)_tmp928)->f1;_tmp92A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp928)->f2;_LL5A0: {void*_tmp932=_tmp92A;struct Cyc_Absyn_Vardecl*_tmp933;
struct Cyc_Absyn_Vardecl*_tmp934;struct Cyc_Absyn_Vardecl*_tmp935;struct Cyc_Absyn_Vardecl*
_tmp936;_LL5AC: if((int)_tmp932 != 0)goto _LL5AE;_LL5AD: goto _LL5AF;_LL5AE: if(
_tmp932 <= (void*)1  || *((int*)_tmp932)!= 1)goto _LL5B0;_LL5AF: return bogus_ans;
_LL5B0: if(_tmp932 <= (void*)1  || *((int*)_tmp932)!= 0)goto _LL5B2;_tmp933=((struct
Cyc_Absyn_Global_b_struct*)_tmp932)->f1;_LL5B1: {void*_tmp937=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5B9: if(_tmp937 <= (void*)
3  || *((int*)_tmp937)!= 7)goto _LL5BB;_LL5BA: return({struct _tuple10 _tmp938;
_tmp938.f1=1;_tmp938.f2=(void*)2;_tmp938;});_LL5BB:;_LL5BC: return({struct
_tuple10 _tmp939;_tmp939.f1=(_tmp933->tq).real_const;_tmp939.f2=(void*)2;_tmp939;});
_LL5B8:;}_LL5B2: if(_tmp932 <= (void*)1  || *((int*)_tmp932)!= 3)goto _LL5B4;_tmp934=((
struct Cyc_Absyn_Local_b_struct*)_tmp932)->f1;_LL5B3: {void*_tmp93A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL5BE: if(_tmp93A <= (void*)
3  || *((int*)_tmp93A)!= 7)goto _LL5C0;_LL5BF: return({struct _tuple10 _tmp93B;
_tmp93B.f1=1;_tmp93B.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp934->rgn))->v;
_tmp93B;});_LL5C0:;_LL5C1: _tmp934->escapes=1;return({struct _tuple10 _tmp93C;
_tmp93C.f1=(_tmp934->tq).real_const;_tmp93C.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp934->rgn))->v;_tmp93C;});_LL5BD:;}_LL5B4: if(_tmp932 <= (void*)1
 || *((int*)_tmp932)!= 4)goto _LL5B6;_tmp935=((struct Cyc_Absyn_Pat_b_struct*)
_tmp932)->f1;_LL5B5: _tmp936=_tmp935;goto _LL5B7;_LL5B6: if(_tmp932 <= (void*)1  || *((
int*)_tmp932)!= 2)goto _LL5AB;_tmp936=((struct Cyc_Absyn_Param_b_struct*)_tmp932)->f1;
_LL5B7: _tmp936->escapes=1;return({struct _tuple10 _tmp93D;_tmp93D.f1=(_tmp936->tq).real_const;
_tmp93D.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp936->rgn))->v;_tmp93D;});
_LL5AB:;}_LL5A1: if(*((int*)_tmp928)!= 23)goto _LL5A3;_tmp92B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp928)->f1;_tmp92C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp928)->f2;_LL5A2: {
void*_tmp93E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp92B->topt))->v);struct Cyc_List_List*_tmp93F;struct Cyc_Absyn_AggrInfo _tmp940;
void*_tmp941;struct Cyc_Absyn_Aggrdecl**_tmp942;struct Cyc_Absyn_Aggrdecl*_tmp943;
_LL5C3: if(_tmp93E <= (void*)3  || *((int*)_tmp93E)!= 11)goto _LL5C5;_tmp93F=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp93E)->f2;_LL5C4: {struct Cyc_Absyn_Aggrfield*
_tmp944=Cyc_Absyn_lookup_field(_tmp93F,_tmp92C);if(_tmp944 != 0  && _tmp944->width
!= 0)return({struct _tuple10 _tmp945;_tmp945.f1=(_tmp944->tq).real_const;_tmp945.f2=(
Cyc_Tcutil_addressof_props(te,_tmp92B)).f2;_tmp945;});return bogus_ans;}_LL5C5:
if(_tmp93E <= (void*)3  || *((int*)_tmp93E)!= 10)goto _LL5C7;_tmp940=((struct Cyc_Absyn_AggrType_struct*)
_tmp93E)->f1;_tmp941=(void*)_tmp940.aggr_info;if(*((int*)_tmp941)!= 1)goto _LL5C7;
_tmp942=((struct Cyc_Absyn_KnownAggr_struct*)_tmp941)->f1;_tmp943=*_tmp942;_LL5C6: {
struct Cyc_Absyn_Aggrfield*_tmp946=Cyc_Absyn_lookup_decl_field(_tmp943,_tmp92C);
if(_tmp946 != 0  && _tmp946->width != 0)return({struct _tuple10 _tmp947;_tmp947.f1=(
_tmp946->tq).real_const;_tmp947.f2=(Cyc_Tcutil_addressof_props(te,_tmp92B)).f2;
_tmp947;});return bogus_ans;}_LL5C7:;_LL5C8: return bogus_ans;_LL5C2:;}_LL5A3: if(*((
int*)_tmp928)!= 24)goto _LL5A5;_tmp92D=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp928)->f1;_tmp92E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp928)->f2;_LL5A4: {
void*_tmp948=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp92D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp949;void*_tmp94A;struct Cyc_Absyn_PtrAtts
_tmp94B;void*_tmp94C;_LL5CA: if(_tmp948 <= (void*)3  || *((int*)_tmp948)!= 4)goto
_LL5CC;_tmp949=((struct Cyc_Absyn_PointerType_struct*)_tmp948)->f1;_tmp94A=(void*)
_tmp949.elt_typ;_tmp94B=_tmp949.ptr_atts;_tmp94C=(void*)_tmp94B.rgn;_LL5CB: {
struct Cyc_Absyn_Aggrfield*finfo;{void*_tmp94D=Cyc_Tcutil_compress(_tmp94A);
struct Cyc_List_List*_tmp94E;struct Cyc_Absyn_AggrInfo _tmp94F;void*_tmp950;struct
Cyc_Absyn_Aggrdecl**_tmp951;struct Cyc_Absyn_Aggrdecl*_tmp952;_LL5CF: if(_tmp94D <= (
void*)3  || *((int*)_tmp94D)!= 11)goto _LL5D1;_tmp94E=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp94D)->f2;_LL5D0: finfo=Cyc_Absyn_lookup_field(_tmp94E,_tmp92E);goto _LL5CE;
_LL5D1: if(_tmp94D <= (void*)3  || *((int*)_tmp94D)!= 10)goto _LL5D3;_tmp94F=((
struct Cyc_Absyn_AggrType_struct*)_tmp94D)->f1;_tmp950=(void*)_tmp94F.aggr_info;
if(*((int*)_tmp950)!= 1)goto _LL5D3;_tmp951=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp950)->f1;_tmp952=*_tmp951;_LL5D2: finfo=Cyc_Absyn_lookup_decl_field(_tmp952,
_tmp92E);goto _LL5CE;_LL5D3:;_LL5D4: return bogus_ans;_LL5CE:;}if(finfo != 0  && 
finfo->width != 0)return({struct _tuple10 _tmp953;_tmp953.f1=(finfo->tq).real_const;
_tmp953.f2=_tmp94C;_tmp953;});return bogus_ans;}_LL5CC:;_LL5CD: return bogus_ans;
_LL5C9:;}_LL5A5: if(*((int*)_tmp928)!= 22)goto _LL5A7;_tmp92F=((struct Cyc_Absyn_Deref_e_struct*)
_tmp928)->f1;_LL5A6: {void*_tmp954=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp92F->topt))->v);struct Cyc_Absyn_PtrInfo _tmp955;struct Cyc_Absyn_Tqual
_tmp956;struct Cyc_Absyn_PtrAtts _tmp957;void*_tmp958;_LL5D6: if(_tmp954 <= (void*)3
 || *((int*)_tmp954)!= 4)goto _LL5D8;_tmp955=((struct Cyc_Absyn_PointerType_struct*)
_tmp954)->f1;_tmp956=_tmp955.elt_tq;_tmp957=_tmp955.ptr_atts;_tmp958=(void*)
_tmp957.rgn;_LL5D7: return({struct _tuple10 _tmp959;_tmp959.f1=_tmp956.real_const;
_tmp959.f2=_tmp958;_tmp959;});_LL5D8:;_LL5D9: return bogus_ans;_LL5D5:;}_LL5A7: if(*((
int*)_tmp928)!= 25)goto _LL5A9;_tmp930=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp928)->f1;_tmp931=((struct Cyc_Absyn_Subscript_e_struct*)_tmp928)->f2;_LL5A8: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp930->topt))->v);
void*_tmp95A=t;struct Cyc_List_List*_tmp95B;struct Cyc_Absyn_PtrInfo _tmp95C;struct
Cyc_Absyn_Tqual _tmp95D;struct Cyc_Absyn_PtrAtts _tmp95E;void*_tmp95F;struct Cyc_Absyn_ArrayInfo
_tmp960;struct Cyc_Absyn_Tqual _tmp961;_LL5DB: if(_tmp95A <= (void*)3  || *((int*)
_tmp95A)!= 9)goto _LL5DD;_tmp95B=((struct Cyc_Absyn_TupleType_struct*)_tmp95A)->f1;
_LL5DC: {unsigned int _tmp963;int _tmp964;struct _tuple7 _tmp962=Cyc_Evexp_eval_const_uint_exp(
_tmp931);_tmp963=_tmp962.f1;_tmp964=_tmp962.f2;if(!_tmp964)return bogus_ans;{
struct _tuple4*_tmp965=Cyc_Absyn_lookup_tuple_field(_tmp95B,(int)_tmp963);if(
_tmp965 != 0)return({struct _tuple10 _tmp966;_tmp966.f1=((*_tmp965).f1).real_const;
_tmp966.f2=(Cyc_Tcutil_addressof_props(te,_tmp930)).f2;_tmp966;});return
bogus_ans;}}_LL5DD: if(_tmp95A <= (void*)3  || *((int*)_tmp95A)!= 4)goto _LL5DF;
_tmp95C=((struct Cyc_Absyn_PointerType_struct*)_tmp95A)->f1;_tmp95D=_tmp95C.elt_tq;
_tmp95E=_tmp95C.ptr_atts;_tmp95F=(void*)_tmp95E.rgn;_LL5DE: return({struct
_tuple10 _tmp967;_tmp967.f1=_tmp95D.real_const;_tmp967.f2=_tmp95F;_tmp967;});
_LL5DF: if(_tmp95A <= (void*)3  || *((int*)_tmp95A)!= 7)goto _LL5E1;_tmp960=((struct
Cyc_Absyn_ArrayType_struct*)_tmp95A)->f1;_tmp961=_tmp960.tq;_LL5E0: return({
struct _tuple10 _tmp968;_tmp968.f1=_tmp961.real_const;_tmp968.f2=(Cyc_Tcutil_addressof_props(
te,_tmp930)).f2;_tmp968;});_LL5E1:;_LL5E2: return bogus_ans;_LL5DA:;}_LL5A9:;
_LL5AA:({void*_tmp969[0]={};Cyc_Tcutil_terr(e->loc,({const char*_tmp96A="unary & applied to non-lvalue";
_tag_arr(_tmp96A,sizeof(char),_get_zero_arr_size(_tmp96A,30));}),_tag_arr(
_tmp969,sizeof(void*),0));});return bogus_ans;_LL59E:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmp96C=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmp96D;void*_tmp96E;struct Cyc_Absyn_Tqual
_tmp96F;struct Cyc_Absyn_Conref*_tmp970;_LL5E4: if(_tmp96C <= (void*)3  || *((int*)
_tmp96C)!= 7)goto _LL5E6;_tmp96D=((struct Cyc_Absyn_ArrayType_struct*)_tmp96C)->f1;
_tmp96E=(void*)_tmp96D.elt_type;_tmp96F=_tmp96D.tq;_tmp970=_tmp96D.zero_term;
_LL5E5: {void*_tmp972;struct _tuple10 _tmp971=Cyc_Tcutil_addressof_props(te,e);
_tmp972=_tmp971.f2;return Cyc_Absyn_atb_typ(_tmp96E,_tmp972,_tmp96F,(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp973=_cycalloc(sizeof(*_tmp973));_tmp973[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp974;_tmp974.tag=0;_tmp974.f1=e;_tmp974;});
_tmp973;}),_tmp970);}_LL5E6:;_LL5E7: return e_typ;_LL5E3:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{void*_tmp975=(void*)b->v;void*_tmp976;void*_tmp977;struct Cyc_Absyn_Exp*
_tmp978;void*_tmp979;_LL5E9: if(_tmp975 <= (void*)1  || *((int*)_tmp975)!= 0)goto
_LL5EB;_tmp976=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp975)->f1;if((int)
_tmp976 != 0)goto _LL5EB;_LL5EA: return;_LL5EB: if(_tmp975 <= (void*)1  || *((int*)
_tmp975)!= 0)goto _LL5ED;_tmp977=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp975)->f1;if(_tmp977 <= (void*)1  || *((int*)_tmp977)!= 0)goto _LL5ED;_tmp978=((
struct Cyc_Absyn_Upper_b_struct*)_tmp977)->f1;_LL5EC: {unsigned int _tmp97B;int
_tmp97C;struct _tuple7 _tmp97A=Cyc_Evexp_eval_const_uint_exp(_tmp978);_tmp97B=
_tmp97A.f1;_tmp97C=_tmp97A.f2;if(_tmp97C  && _tmp97B <= i)({struct Cyc_Int_pa_struct
_tmp980;_tmp980.tag=1;_tmp980.f1=(unsigned int)((int)i);{struct Cyc_Int_pa_struct
_tmp97F;_tmp97F.tag=1;_tmp97F.f1=(unsigned int)((int)_tmp97B);{void*_tmp97D[2]={&
_tmp97F,& _tmp980};Cyc_Tcutil_terr(loc,({const char*_tmp97E="dereference is out of bounds: %d <= %d";
_tag_arr(_tmp97E,sizeof(char),_get_zero_arr_size(_tmp97E,39));}),_tag_arr(
_tmp97D,sizeof(void*),2));}}});return;}_LL5ED: if(_tmp975 <= (void*)1  || *((int*)
_tmp975)!= 0)goto _LL5EF;_tmp979=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp975)->f1;if(_tmp979 <= (void*)1  || *((int*)_tmp979)!= 1)goto _LL5EF;_LL5EE:
return;_LL5EF: if((int)_tmp975 != 0)goto _LL5F1;_LL5F0:(void*)(b->v=(void*)((void*)({
struct Cyc_Absyn_Eq_constr_struct*_tmp981=_cycalloc(sizeof(*_tmp981));_tmp981[0]=({
struct Cyc_Absyn_Eq_constr_struct _tmp982;_tmp982.tag=0;_tmp982.f1=(void*)((void*)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp983=_cycalloc(sizeof(*_tmp983));
_tmp983[0]=({struct Cyc_Absyn_Upper_b_struct _tmp984;_tmp984.tag=0;_tmp984.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp984;});_tmp983;})));_tmp982;});_tmp981;})));return;_LL5F1:;_LL5F2:({
void*_tmp985[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(({
const char*_tmp986="check_bound -- bad compressed conref";_tag_arr(_tmp986,
sizeof(char),_get_zero_arr_size(_tmp986,37));}),_tag_arr(_tmp985,sizeof(void*),0));});
_LL5E8:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp987=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp988;struct Cyc_Absyn_Exp*_tmp989;_LL5F4: if(_tmp987 <= (void*)1  || *((int*)
_tmp987)!= 0)goto _LL5F6;_tmp988=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp987)->f1;if(_tmp988 <= (void*)1  || *((int*)_tmp988)!= 0)goto _LL5F6;_tmp989=((
struct Cyc_Absyn_Upper_b_struct*)_tmp988)->f1;_LL5F5: {unsigned int _tmp98B;int
_tmp98C;struct _tuple7 _tmp98A=Cyc_Evexp_eval_const_uint_exp(_tmp989);_tmp98B=
_tmp98A.f1;_tmp98C=_tmp98A.f2;return _tmp98C  && _tmp98B == 1;}_LL5F6:;_LL5F7:
return 0;_LL5F3:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp98D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp98E;void*_tmp98F;struct Cyc_Absyn_Conref*_tmp990;
struct Cyc_List_List*_tmp991;struct Cyc_Absyn_AggrInfo _tmp992;void*_tmp993;struct
Cyc_Absyn_AggrInfo _tmp994;void*_tmp995;struct Cyc_Absyn_Aggrdecl**_tmp996;struct
Cyc_Absyn_Aggrdecl*_tmp997;struct Cyc_List_List*_tmp998;struct Cyc_List_List*
_tmp999;_LL5F9: if((int)_tmp98D != 0)goto _LL5FB;_LL5FA: goto _LL5FC;_LL5FB: if(
_tmp98D <= (void*)3  || *((int*)_tmp98D)!= 5)goto _LL5FD;_LL5FC: goto _LL5FE;_LL5FD:
if((int)_tmp98D != 1)goto _LL5FF;_LL5FE: goto _LL600;_LL5FF: if(_tmp98D <= (void*)3
 || *((int*)_tmp98D)!= 6)goto _LL601;_LL600: return 1;_LL601: if(_tmp98D <= (void*)3
 || *((int*)_tmp98D)!= 12)goto _LL603;_LL602: goto _LL604;_LL603: if(_tmp98D <= (void*)
3  || *((int*)_tmp98D)!= 13)goto _LL605;_LL604: return 0;_LL605: if(_tmp98D <= (void*)
3  || *((int*)_tmp98D)!= 7)goto _LL607;_tmp98E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp98D)->f1;_tmp98F=(void*)_tmp98E.elt_type;_tmp990=_tmp98E.zero_term;_LL606:
return !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp990)
 && Cyc_Tcutil_bits_only(_tmp98F);_LL607: if(_tmp98D <= (void*)3  || *((int*)
_tmp98D)!= 9)goto _LL609;_tmp991=((struct Cyc_Absyn_TupleType_struct*)_tmp98D)->f1;
_LL608: for(0;_tmp991 != 0;_tmp991=_tmp991->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp991->hd)).f2))return 0;}return 1;_LL609: if(_tmp98D <= (void*)3
 || *((int*)_tmp98D)!= 10)goto _LL60B;_tmp992=((struct Cyc_Absyn_AggrType_struct*)
_tmp98D)->f1;_tmp993=(void*)_tmp992.aggr_info;if(*((int*)_tmp993)!= 0)goto _LL60B;
_LL60A: return 0;_LL60B: if(_tmp98D <= (void*)3  || *((int*)_tmp98D)!= 10)goto _LL60D;
_tmp994=((struct Cyc_Absyn_AggrType_struct*)_tmp98D)->f1;_tmp995=(void*)_tmp994.aggr_info;
if(*((int*)_tmp995)!= 1)goto _LL60D;_tmp996=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp995)->f1;_tmp997=*_tmp996;_tmp998=_tmp994.targs;_LL60C: if(_tmp997->impl == 0)
return 0;{struct _RegionHandle _tmp99A=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp99A;_push_region(rgn);{struct Cyc_List_List*_tmp99B=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp997->tvs,_tmp998);{struct Cyc_List_List*fs=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp997->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp99B,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type))){int _tmp99C=0;_npop_handler(0);return _tmp99C;}}}{int _tmp99D=1;
_npop_handler(0);return _tmp99D;}};_pop_region(rgn);}_LL60D: if(_tmp98D <= (void*)3
 || *((int*)_tmp98D)!= 11)goto _LL60F;_tmp999=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp98D)->f2;_LL60E: for(0;_tmp999 != 0;_tmp999=_tmp999->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp999->hd)->type))return 0;}return 1;_LL60F:;
_LL610: return 0;_LL5F8:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp99E=(void*)e->r;struct _tuple1*_tmp99F;void*_tmp9A0;struct Cyc_Absyn_Exp*
_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*
_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*
_tmp9A7;void*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9A9;void*_tmp9AA;void*_tmp9AB;
struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*
_tmp9AE;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_List_List*_tmp9B0;struct Cyc_List_List*
_tmp9B1;struct Cyc_List_List*_tmp9B2;void*_tmp9B3;struct Cyc_List_List*_tmp9B4;
struct Cyc_List_List*_tmp9B5;struct Cyc_List_List*_tmp9B6;_LL612: if(*((int*)
_tmp99E)!= 0)goto _LL614;_LL613: goto _LL615;_LL614: if(*((int*)_tmp99E)!= 18)goto
_LL616;_LL615: goto _LL617;_LL616: if(*((int*)_tmp99E)!= 19)goto _LL618;_LL617: goto
_LL619;_LL618: if(*((int*)_tmp99E)!= 20)goto _LL61A;_LL619: goto _LL61B;_LL61A: if(*((
int*)_tmp99E)!= 21)goto _LL61C;_LL61B: goto _LL61D;_LL61C: if(*((int*)_tmp99E)!= 33)
goto _LL61E;_LL61D: goto _LL61F;_LL61E: if(*((int*)_tmp99E)!= 34)goto _LL620;_LL61F:
return 1;_LL620: if(*((int*)_tmp99E)!= 1)goto _LL622;_tmp99F=((struct Cyc_Absyn_Var_e_struct*)
_tmp99E)->f1;_tmp9A0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp99E)->f2;_LL621: {
void*_tmp9B7=_tmp9A0;struct Cyc_Absyn_Vardecl*_tmp9B8;_LL641: if(_tmp9B7 <= (void*)
1  || *((int*)_tmp9B7)!= 1)goto _LL643;_LL642: return 1;_LL643: if(_tmp9B7 <= (void*)1
 || *((int*)_tmp9B7)!= 0)goto _LL645;_tmp9B8=((struct Cyc_Absyn_Global_b_struct*)
_tmp9B7)->f1;_LL644: {void*_tmp9B9=Cyc_Tcutil_compress((void*)_tmp9B8->type);
_LL64A: if(_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 7)goto _LL64C;_LL64B: goto
_LL64D;_LL64C: if(_tmp9B9 <= (void*)3  || *((int*)_tmp9B9)!= 8)goto _LL64E;_LL64D:
return 1;_LL64E:;_LL64F: return var_okay;_LL649:;}_LL645: if((int)_tmp9B7 != 0)goto
_LL647;_LL646: return 0;_LL647:;_LL648: return var_okay;_LL640:;}_LL622: if(*((int*)
_tmp99E)!= 6)goto _LL624;_tmp9A1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99E)->f1;
_tmp9A2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp99E)->f2;_tmp9A3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp99E)->f3;_LL623: return(Cyc_Tcutil_cnst_exp(te,0,_tmp9A1) && Cyc_Tcutil_cnst_exp(
te,0,_tmp9A2)) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A3);_LL624: if(*((int*)_tmp99E)!= 
9)goto _LL626;_tmp9A4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp99E)->f1;_tmp9A5=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp99E)->f2;_LL625: return Cyc_Tcutil_cnst_exp(
te,0,_tmp9A4) && Cyc_Tcutil_cnst_exp(te,0,_tmp9A5);_LL626: if(*((int*)_tmp99E)!= 
13)goto _LL628;_tmp9A6=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp99E)->f1;
_LL627: _tmp9A7=_tmp9A6;goto _LL629;_LL628: if(*((int*)_tmp99E)!= 14)goto _LL62A;
_tmp9A7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp99E)->f1;_LL629: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp9A7);_LL62A: if(*((int*)_tmp99E)!= 15)goto _LL62C;_tmp9A8=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp99E)->f1;_tmp9A9=((struct Cyc_Absyn_Cast_e_struct*)
_tmp99E)->f2;_tmp9AA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp99E)->f4;if((
int)_tmp9AA != 1)goto _LL62C;_LL62B: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9A9);
_LL62C: if(*((int*)_tmp99E)!= 15)goto _LL62E;_tmp9AB=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp99E)->f1;_tmp9AC=((struct Cyc_Absyn_Cast_e_struct*)_tmp99E)->f2;_LL62D: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmp9AC);_LL62E: if(*((int*)_tmp99E)!= 16)goto
_LL630;_tmp9AD=((struct Cyc_Absyn_Address_e_struct*)_tmp99E)->f1;_LL62F: return Cyc_Tcutil_cnst_exp(
te,1,_tmp9AD);_LL630: if(*((int*)_tmp99E)!= 29)goto _LL632;_tmp9AE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp99E)->f2;_tmp9AF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp99E)->f3;
_LL631: return Cyc_Tcutil_cnst_exp(te,0,_tmp9AE) && Cyc_Tcutil_cnst_exp(te,0,
_tmp9AF);_LL632: if(*((int*)_tmp99E)!= 28)goto _LL634;_tmp9B0=((struct Cyc_Absyn_Array_e_struct*)
_tmp99E)->f1;_LL633: _tmp9B1=_tmp9B0;goto _LL635;_LL634: if(*((int*)_tmp99E)!= 31)
goto _LL636;_tmp9B1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp99E)->f2;_LL635:
_tmp9B2=_tmp9B1;goto _LL637;_LL636: if(*((int*)_tmp99E)!= 30)goto _LL638;_tmp9B2=((
struct Cyc_Absyn_Struct_e_struct*)_tmp99E)->f3;_LL637: for(0;_tmp9B2 != 0;_tmp9B2=
_tmp9B2->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp9B2->hd)).f2))
return 0;}return 1;_LL638: if(*((int*)_tmp99E)!= 3)goto _LL63A;_tmp9B3=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp99E)->f1;_tmp9B4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp99E)->f2;_LL639: _tmp9B5=_tmp9B4;goto _LL63B;_LL63A: if(*((int*)_tmp99E)!= 26)
goto _LL63C;_tmp9B5=((struct Cyc_Absyn_Tuple_e_struct*)_tmp99E)->f1;_LL63B: _tmp9B6=
_tmp9B5;goto _LL63D;_LL63C: if(*((int*)_tmp99E)!= 32)goto _LL63E;_tmp9B6=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp99E)->f1;_LL63D: for(0;_tmp9B6 != 0;_tmp9B6=_tmp9B6->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp9B6->hd))return 0;}return 1;
_LL63E:;_LL63F: return 0;_LL611:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp9BA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9BB;
struct Cyc_Absyn_PtrAtts _tmp9BC;struct Cyc_Absyn_Conref*_tmp9BD;struct Cyc_Absyn_Conref*
_tmp9BE;struct Cyc_Absyn_ArrayInfo _tmp9BF;void*_tmp9C0;struct Cyc_List_List*
_tmp9C1;struct Cyc_Absyn_AggrInfo _tmp9C2;void*_tmp9C3;struct Cyc_List_List*_tmp9C4;
struct Cyc_List_List*_tmp9C5;_LL651: if((int)_tmp9BA != 0)goto _LL653;_LL652: goto
_LL654;_LL653: if(_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 5)goto _LL655;_LL654:
goto _LL656;_LL655: if((int)_tmp9BA != 1)goto _LL657;_LL656: goto _LL658;_LL657: if(
_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 6)goto _LL659;_LL658: return 1;_LL659: if(
_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 4)goto _LL65B;_tmp9BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp9BA)->f1;_tmp9BC=_tmp9BB.ptr_atts;_tmp9BD=_tmp9BC.nullable;_tmp9BE=_tmp9BC.bounds;
_LL65A: {void*_tmp9C6=Cyc_Absyn_conref_def((void*)((void*)0),_tmp9BE);_LL66A: if((
int)_tmp9C6 != 0)goto _LL66C;_LL66B: return 1;_LL66C:;_LL66D: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp9BD);_LL669:;}_LL65B: if(
_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 7)goto _LL65D;_tmp9BF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp9BA)->f1;_tmp9C0=(void*)_tmp9BF.elt_type;_LL65C: return Cyc_Tcutil_supports_default(
_tmp9C0);_LL65D: if(_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 9)goto _LL65F;_tmp9C1=((
struct Cyc_Absyn_TupleType_struct*)_tmp9BA)->f1;_LL65E: for(0;_tmp9C1 != 0;_tmp9C1=
_tmp9C1->tl){if(!Cyc_Tcutil_supports_default((*((struct _tuple4*)_tmp9C1->hd)).f2))
return 0;}return 1;_LL65F: if(_tmp9BA <= (void*)3  || *((int*)_tmp9BA)!= 10)goto
_LL661;_tmp9C2=((struct Cyc_Absyn_AggrType_struct*)_tmp9BA)->f1;_tmp9C3=(void*)
_tmp9C2.aggr_info;_tmp9C4=_tmp9C2.targs;_LL660: {struct Cyc_Absyn_Aggrdecl*
_tmp9C7=Cyc_Absyn_get_known_aggrdecl(_tmp9C3);if(_tmp9C7->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C7->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmp9C7->tvs,_tmp9C4,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp9C7->impl))->fields);}_LL661: if(_tmp9BA <= (void*)3  || *((int*)
_tmp9BA)!= 11)goto _LL663;_tmp9C5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp9BA)->f2;
_LL662: return Cyc_Tcutil_fields_support_default(0,0,_tmp9C5);_LL663: if(_tmp9BA <= (
void*)3  || *((int*)_tmp9BA)!= 13)goto _LL665;_LL664: goto _LL666;_LL665: if(_tmp9BA
<= (void*)3  || *((int*)_tmp9BA)!= 12)goto _LL667;_LL666: return 1;_LL667:;_LL668:
return 0;_LL650:;}static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*
tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmp9C8=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp9C8;_push_region(rgn);{struct Cyc_List_List*
_tmp9C9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp9C9,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmp9CA=0;_npop_handler(0);
return _tmp9CA;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9CB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9CC;struct Cyc_Absyn_PtrAtts
_tmp9CD;struct Cyc_Absyn_Conref*_tmp9CE;struct Cyc_Absyn_Conref*_tmp9CF;_LL66F: if(
_tmp9CB <= (void*)3  || *((int*)_tmp9CB)!= 5)goto _LL671;_LL670: goto _LL672;_LL671:
if((int)_tmp9CB != 1)goto _LL673;_LL672: goto _LL674;_LL673: if(_tmp9CB <= (void*)3
 || *((int*)_tmp9CB)!= 6)goto _LL675;_LL674: return 1;_LL675: if(_tmp9CB <= (void*)3
 || *((int*)_tmp9CB)!= 4)goto _LL677;_tmp9CC=((struct Cyc_Absyn_PointerType_struct*)
_tmp9CB)->f1;_tmp9CD=_tmp9CC.ptr_atts;_tmp9CE=_tmp9CD.nullable;_tmp9CF=_tmp9CD.bounds;
_LL676: {void*_tmp9D0=(void*)(Cyc_Absyn_compress_conref(_tmp9CF))->v;void*
_tmp9D1;_LL67A: if(_tmp9D0 <= (void*)1  || *((int*)_tmp9D0)!= 0)goto _LL67C;_tmp9D1=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9D0)->f1;if((int)_tmp9D1 != 0)goto
_LL67C;_LL67B: return 0;_LL67C: if(_tmp9D0 <= (void*)1  || *((int*)_tmp9D0)!= 0)goto
_LL67E;_LL67D: {void*_tmp9D2=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp9CE))->v;int _tmp9D3;_LL681: if(_tmp9D2 <= (void*)
1  || *((int*)_tmp9D2)!= 0)goto _LL683;_tmp9D3=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D2)->f1;_LL682: return _tmp9D3;_LL683:;_LL684: return 0;_LL680:;}_LL67E:;_LL67F:
return 0;_LL679:;}_LL677:;_LL678: return 0;_LL66E:;}int Cyc_Tcutil_is_noreturn(void*
t){{void*_tmp9D4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp9D5;void*
_tmp9D6;struct Cyc_Absyn_FnInfo _tmp9D7;struct Cyc_List_List*_tmp9D8;_LL686: if(
_tmp9D4 <= (void*)3  || *((int*)_tmp9D4)!= 4)goto _LL688;_tmp9D5=((struct Cyc_Absyn_PointerType_struct*)
_tmp9D4)->f1;_tmp9D6=(void*)_tmp9D5.elt_typ;_LL687: return Cyc_Tcutil_is_noreturn(
_tmp9D6);_LL688: if(_tmp9D4 <= (void*)3  || *((int*)_tmp9D4)!= 8)goto _LL68A;_tmp9D7=((
struct Cyc_Absyn_FnType_struct*)_tmp9D4)->f1;_tmp9D8=_tmp9D7.attributes;_LL689:
for(0;_tmp9D8 != 0;_tmp9D8=_tmp9D8->tl){void*_tmp9D9=(void*)_tmp9D8->hd;_LL68D:
if((int)_tmp9D9 != 3)goto _LL68F;_LL68E: return 1;_LL68F:;_LL690: continue;_LL68C:;}
goto _LL685;_LL68A:;_LL68B: goto _LL685;_LL685:;}return 0;}
